#include <cstdio>
#include <cstring>
#include <iostream>
#include <vector>
#include <xcl2.hpp>

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(arr) sizeof(arr)/sizeof((arr)[0])
#endif


const char * free_run_kernel_list[] = {
//    "increment"
    "ssrng_state_update_8", "ssrng_state_update_2", "ssrng_state_update_3",
    "ssrng_state_update_4", "ssrng_state_merge", "ssrng_state_update_5", "ssrng_state_update_0",
    "ssrng_state_update_6", "ssrng_state_update_1", "ssrng_state_update_7",
    "rng_start", "rng_end", "rng_main",
};

int main(int argc, char** argv) {
    if (argc != 2) {
        std::cout << "Usage: " << argv[0] << " <XCLBIN File>" << std::endl;
        return EXIT_FAILURE;
    }

    std::string binaryFile = argv[1];

    size_t data_size = 1024;
    cl_int err;
    cl::CommandQueue q;
    cl::Context context;
    cl::Kernel krnl_free_run[ARRAY_SIZE(free_run_kernel_list)], krnl_mem_read, krnl_mem_write;

    // Reducing the data size for emulation mode
    char* xcl_mode = getenv("XCL_EMULATION_MODE");
    if (xcl_mode != nullptr) {
        data_size = 1024;
    }

    // Allocate Memory in Host Memory
    size_t vector_size_bytes = sizeof(int) * data_size;
    std::vector<int, aligned_allocator<int> > source_input(data_size);
    std::vector<int, aligned_allocator<int> > source_hw_results(data_size);
    std::vector<int, aligned_allocator<int> > source_sw_results(data_size);

    // Create the test data and Software Result
    for (size_t i = 0; i < data_size; i++) {
        source_input[i] = i;
        source_sw_results[i] = i + 1;
    }

    // OPENCL HOST CODE AREA START
    // get_xil_devices() is a utility API which will find the Xilinx
    // platforms and will return list of devices connected to Xilinx platform
    std::vector<cl::Device> devices = xcl::get_xil_devices();

    // read_binary_file() is a utility API which will load the binaryFile
    // and will return pointer to file buffer.
    auto fileBuf = xcl::read_binary_file(binaryFile);
    cl::Program::Binaries bins{{fileBuf.data(), fileBuf.size()}};
    bool valid_device = false;
    for (unsigned int i = 0; i < devices.size(); i++) {
        auto device = devices[i];
        // Creating Context and Command Queue for selected Device
        OCL_CHECK(err, context = cl::Context(device, nullptr, nullptr, nullptr, &err));
        OCL_CHECK(err, q = cl::CommandQueue(context, device,
                                            CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE | CL_QUEUE_PROFILING_ENABLE, &err));

        std::cout << "Trying to program device[" << i << "]: " << device.getInfo<CL_DEVICE_NAME>() << std::endl;
        cl::Program program(context, {device}, bins, nullptr, &err);
        if (err != CL_SUCCESS) {
            std::cout << "Failed to program device[" << i << "] with xclbin file!\n";
        } else {
            std::cout << "Device[" << i << "]: program successful!\n";
            for (unsigned int j = 0; j < ARRAY_SIZE(free_run_kernel_list); j++)
            {
                OCL_CHECK(err, krnl_free_run[j] = cl::Kernel(program, free_run_kernel_list[j], &err));
            }
            OCL_CHECK(err, krnl_mem_read = cl::Kernel(program, "mem_read", &err));
            OCL_CHECK(err, krnl_mem_write = cl::Kernel(program, "mem_write", &err));
            valid_device = true;
            break; // we break because we found a valid device
        }
    }
    if (!valid_device) {
        std::cout << "Failed to program any device found, exit!\n";
        exit(EXIT_FAILURE);
    }

    // Allocate Buffer in Global Memory
    // Buffers are allocated using CL_MEM_USE_HOST_PTR for efficient memory and
    // Device-to-host communication
    std::cout << "Creating Buffers..." << std::endl;
    OCL_CHECK(err, cl::Buffer buffer_input(context,
                                           CL_MEM_USE_HOST_PTR, // | CL_INCREMENT_ONLY,
                                           vector_size_bytes, source_input.data(), &err));
    OCL_CHECK(err, cl::Buffer buffer_output(context,
                                            CL_MEM_USE_HOST_PTR, // | CL_MEM_WRITE_ONLY,
                                            vector_size_bytes, source_hw_results.data(), &err));

    // Set the Kernel Arguments
    int size = data_size;
    OCL_CHECK(err, err = krnl_mem_read.setArg(0, buffer_input));
    OCL_CHECK(err, err = krnl_mem_read.setArg(2, size));
    OCL_CHECK(err, err = krnl_mem_write.setArg(1, buffer_output));
    OCL_CHECK(err, err = krnl_mem_write.setArg(2, size));

    // Copy input data to device global memory
    std::cout << "Copying data..." << std::endl;
    OCL_CHECK(err, err = q.enqueueMigrateMemObjects({buffer_input}, 0 /*0 means from host*/));

    OCL_CHECK(err, err = q.finish());

    for (int count = 0 ; count < 2; count++) {

        // Launch the Kernel
        std::cout << "Launching Kernel..." << std::endl;
        OCL_CHECK(err, err = q.enqueueTask(krnl_mem_read));
        OCL_CHECK(err, err = q.enqueueTask(krnl_mem_write));

        // wait for all kernels to finish their operations
        OCL_CHECK(err, err = q.finish());

        // Copy Result from Device Global Memory to Host Local Memory
        std::cout << "Getting Results..." << std::endl;
        OCL_CHECK(err, err = q.enqueueMigrateMemObjects({buffer_output}, CL_MIGRATE_MEM_OBJECT_HOST));
        OCL_CHECK(err, err = q.finish());

        // OPENCL HOST CODE AREA END

        // Compare the results of the Device to the simulation
        bool match = true;
        for (size_t i = 0; i < data_size; i++) {
            if (source_hw_results[i] != source_sw_results[i]) {
                std::cout << "Error: Result mismatch" << std::endl;
                std::cout << "i = " << i << " CPU result = " << source_sw_results[i]
                          << " Device result = " << source_hw_results[i] << std::endl;
                match = false;
                break;
            }
        }

        std::cout << "TEST " << (match ? "PASSED" : "FAILED") << std::endl;
    }
    return 0;
}
