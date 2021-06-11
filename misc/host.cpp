#include "xcl2.hpp"
#include <vector>
#include <stdint.h>

#define __HOST_VERIFICATE__
#include "mpcg.h"


#ifndef DEBUG_PRINTF
#define DEBUG_PRINTF(fmt,...)   printf(fmt,##__VA_ARGS__); fflush(stdout);
#endif





int main(int argc, char** argv)
{
    if (argc != 2) {
        std::cout << "Usage: " << argv[0] << " <XCLBIN File>" << std::endl;
        return EXIT_FAILURE;
    }

    std::string binaryFile = argv[1];

    int size = DATA_SIZE;
    int incr = 0;
    cl_int err;
    unsigned fileBufSize;

    //Allocate Memory in Host Memory
    size_t vector_size_bytes = sizeof(int) * DATA_SIZE;
    std::vector<int, aligned_allocator<int> > source_input1     (DATA_SIZE);
    std::vector<int, aligned_allocator<int> > source_input2     (DATA_SIZE);
    std::vector<uint32_t, aligned_allocator<uint32_t> > source_hw_results(DATA_SIZE);
    std::vector<uint32_t, aligned_allocator<uint32_t> > source_sw_results(DATA_SIZE);

    for (int i = 0 ; i < DATA_SIZE ; i++) {
        source_input1[i] = i;
        source_input2[i] = i;
        source_sw_results[i] = pcg_golden(); //setting the same value of input
        source_hw_results[i] = 0;
        DEBUG_PRINTF("r%04d: 0x%08x\n", i, source_sw_results[i]);
    }

//OPENCL HOST CODE AREA START
    std::vector<cl::Device> devices = xcl::get_xil_devices();
    cl::Device device = devices[0];

    OCL_CHECK(err, cl::Context context(device, NULL, NULL, NULL, &err));
    OCL_CHECK(err, cl::CommandQueue q(context, device, CL_QUEUE_PROFILING_ENABLE, &err));
    OCL_CHECK(err, std::string device_name = device.getInfo<CL_DEVICE_NAME>(&err));

    //Create Program and Kernel
    char* fileBuf = xcl::read_binary_file(binaryFile, fileBufSize);
    cl::Program::Binaries bins{{fileBuf, fileBufSize}};
    devices.resize(1);
    OCL_CHECK(err, cl::Program program(context, devices, bins, NULL, &err));
    OCL_CHECK(err, cl::Kernel krnl_adders(program, "data_flow", &err));

    //Allocate Buffer in Global Memory
    OCL_CHECK(err, cl::Buffer buffer_input1 (context, CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY,
              vector_size_bytes, source_input1.data(), &err));
    OCL_CHECK(err, cl::Buffer buffer_input2 (context, CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY,
              vector_size_bytes, source_input2.data(), &err));
    OCL_CHECK(err, cl::Buffer buffer_output(context, CL_MEM_USE_HOST_PTR | CL_MEM_WRITE_ONLY,
                                            vector_size_bytes, source_hw_results.data(), &err));

    //Set the Kernel Arguments
    int narg = 0;
    OCL_CHECK(err, err = krnl_adders.setArg(narg++, buffer_input1));
    OCL_CHECK(err, err = krnl_adders.setArg(narg++, buffer_input2));
    OCL_CHECK(err, err = krnl_adders.setArg(narg++, buffer_output));
    OCL_CHECK(err, err = krnl_adders.setArg(narg++, incr));
    OCL_CHECK(err, err = krnl_adders.setArg(narg++, size));

    //Copy input data to device global memory
    OCL_CHECK(err, err = q.enqueueMigrateMemObjects({buffer_input1}, 0/* 0 means from host*/));
    OCL_CHECK(err, err = q.enqueueMigrateMemObjects({buffer_input2}, 0/* 0 means from host*/));

    //Launch the Kernel
    OCL_CHECK(err, err = q.enqueueTask(krnl_adders));

    //Copy Result from Device Global Memory to Host Local Memory
    OCL_CHECK(err, err = q.enqueueMigrateMemObjects({buffer_output}, CL_MIGRATE_MEM_OBJECT_HOST));
    q.finish();
//OPENCL HOST CODE AREA END

    // Compare the results of the Device to the simulation
    int match = 0;
    for (int i = 0 ; i < DATA_SIZE ; i++) {
        if (source_hw_results[i] != source_sw_results[i]) {
            std::cout << "Error: Result mismatch" << std::endl;
            std::cout << "i = " << i << " CPU result = " << source_sw_results[i]
                      << " Device result = " << source_hw_results[i] << std::endl;
            match = 1;
            break;
        }
    }

    delete[] fileBuf;

    std::cout << "TEST " << (match ? "FAILED" : "PASSED") << std::endl;
    return (match ? EXIT_FAILURE :  EXIT_SUCCESS);
}
