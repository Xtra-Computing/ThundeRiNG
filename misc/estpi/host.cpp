#include <cstdio>
#include <cstring>
#include <iostream>
#include <vector>
#include <xcl2.hpp>


auto constexpr NCU = 4;

int main(int argc, char **argv) {
    if (argc != 2) {
        std::cout << "Usage: " << argv[0] << " <XCLBIN File>" << std::endl;
        return EXIT_FAILURE;
    }

    std::string binaryFile = argv[1];

    size_t data_size = 1024 * 1024;
    cl_int err;
    cl::CommandQueue q;
    cl::Context context;

    std::string cu_id;


    std::string krnl_name_mem_read = "mem_read";
    std::string krnl_name_mem_write = "mem_write";

    std::vector<cl::Kernel> krnl_mem_read(NCU);
    std::vector<cl::Kernel> krnl_mem_write(NCU);

    // Reducing the data size for emulation mode
    char *xcl_mode = getenv("XCL_EMULATION_MODE");
    if (xcl_mode != NULL) {
        //  data_size = 1024;
    }

    // Allocate Memory in Host Memory
    size_t vector_size_bytes = sizeof(int) * data_size ;
    std::vector<int, aligned_allocator<unsigned int>> source_hw_results(data_size * NCU);

    // OPENCL HOST CODE AREA START
    // get_xil_devices() is a utility API which will find the Xilinx
    // platforms and will return list of devices connected to Xilinx platform
    auto devices = xcl::get_xil_devices();

    // read_binary_file() is a utility API which will load the binaryFile
    // and will return the pointer to file buffer.
    auto fileBuf = xcl::read_binary_file(binaryFile);
    cl::Program::Binaries bins{{fileBuf.data(), fileBuf.size()}};
    bool valid_device = false;
    for (unsigned int i = 0; i < devices.size(); i++) {
        auto device = devices[i];
        // Creating Context and Command Queue for selected Device
        OCL_CHECK(err, context = cl::Context(device, NULL, NULL, NULL, &err));
        OCL_CHECK(err,
                  q = cl::CommandQueue(context, device,
                                       CL_QUEUE_PROFILING_ENABLE |
                                       CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE,
                                       &err));

        std::cout << "Trying to program device[" << i
                  << "]: " << device.getInfo<CL_DEVICE_NAME>() << std::endl;
        cl::Program program(context, {device}, bins, NULL, &err);
        if (err != CL_SUCCESS) {
            std::cout << "Failed to program device[" << i << "] with xclbin file!\n";
        } else {
            std::cout << "Device[" << i << "]: program successful!\n";
            // Creating Kernel object using Compute unit names
            for (int i = 0; i < NCU; i++) {
                cu_id = std::to_string(i + 1);
                std::string krnl_name_full = krnl_name_mem_write + ":{" + krnl_name_mem_write + "_" + cu_id + "}";
                printf("Creating a kernel [%s] for CU(%d)\n", krnl_name_full.c_str(),
                       i);
                // Here Kernel object is created by specifying kernel name along with
                // compute unit.
                // For such case, this kernel object can only access the specific
                // Compute unit
                OCL_CHECK(err,
                          krnl_mem_write[i] = cl::Kernel(program, krnl_name_full.c_str(), &err));
            }
            for (int i = 0; i < NCU; i++) {
                cu_id = std::to_string(i + 1);
                std::string krnl_name_full = krnl_name_mem_read + ":{" + krnl_name_mem_read + "_" + cu_id + "}";
                printf("Creating a kernel [%s] for CU(%d)\n", krnl_name_full.c_str(),
                       i);
                // Here Kernel object is created by specifying kernel name along with
                // compute unit.
                // For such case, this kernel object can only access the specific
                // Compute unit
                OCL_CHECK(err,
                          krnl_mem_read[i] = cl::Kernel(program, krnl_name_full.c_str(), &err));
            }
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

    std::vector<cl::Buffer> buffer_output(NCU);
    std::cout << "Creating Buffers..." << std::endl;

    for (int i = 0; i < NCU; i++) {

        OCL_CHECK(err, buffer_output[i] = cl::Buffer(
                                              context, CL_MEM_USE_HOST_PTR | CL_MEM_WRITE_ONLY,
                                              vector_size_bytes,
                                              source_hw_results.data() + (i * data_size), &err));
    }

    for (int i = 0; i < NCU; i++) {

        // Set the Kernel Arguments
        int size = data_size;
        OCL_CHECK(err, err = krnl_mem_read[i].setArg(1, size));
        OCL_CHECK(err, err = krnl_mem_write[i].setArg(1, buffer_output[i]));
        OCL_CHECK(err, err = krnl_mem_write[i].setArg(2, size));

    }




    // Launch the Kernel
    std::cout << "Launching Kernel..." << std::endl;

    double kernel_time_in_sec = 0, result = 0;
    std::chrono::duration<double> kernel_time(0);
    auto kernel_start = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < NCU; i++) {
        //OCL_CHECK(err, err = q.enqueueTask(krnl_mem_read));
        OCL_CHECK(err, err = q.enqueueTask(krnl_mem_write[i]));
    }

    // wait for all kernels to finish their operations
    OCL_CHECK(err, err = q.finish());

    auto kernel_end = std::chrono::high_resolution_clock::now();
    kernel_time = std::chrono::duration<double>(kernel_end - kernel_start);
    kernel_time_in_sec = kernel_time.count();
    std::cout << "Time: " << kernel_time_in_sec << "s" << std::endl;

    bool match = true;
    // Copy Result from Device Global Memory to Host Local Memory
    for (int i = 0; i < NCU; i++) {
        std::cout << "Getting Results..." << std::endl;
        OCL_CHECK(err, err = q.enqueueMigrateMemObjects({buffer_output[i]},
                             CL_MIGRATE_MEM_OBJECT_HOST));
        OCL_CHECK(err, err = q.finish());
        // OPENCL HOST CODE AREA END

        // Compare the results of the Device to the simulation

        std::cout << "Dump CU "<< i << " : " << std::endl;
        for (size_t j = 0; j < 4; j++) {
            std::cout << "addr = " << j
                      << " Device result = " << source_hw_results[j] << std::endl;
        }
    }

    //std::cout << "TEST " << (match ? "PASSED" : "FAILED") << std::endl;
    return (match ? EXIT_SUCCESS : EXIT_FAILURE);
}
