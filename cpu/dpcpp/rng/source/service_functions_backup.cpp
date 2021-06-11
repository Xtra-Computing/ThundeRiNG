/*******************************************************************************
* Copyright 2020-2021 Intel Corporation.
*
* This software and the related documents are Intel copyrighted  materials,  and
* your use of  them is  governed by the  express license  under which  they were
* provided to you (License).  Unless the License provides otherwise, you may not
* use, modify, copy, publish, distribute,  disclose or transmit this software or
* the related documents without Intel's prior written permission.
*
* This software and the related documents  are provided as  is,  with no express
* or implied  warranties,  other  than those  that are  expressly stated  in the
* License.
*******************************************************************************/

/*
*
*  Content:
*       This example demonstrates usage of DPC++ API for MKL RNG service
*       functionality
*
*       Functions list:
*           oneapi::mkl::rng::skip_ahead
*           oneapi::mkl::rng::leapfrog
*
*
*******************************************************************************/

// stl includes
#include <iostream>
#include <vector>
#include <math.h>
#include <time.h>

#include <pthread.h>

double getCurrentTimestamp(void) {
    timespec a;
    clock_gettime(CLOCK_MONOTONIC, &a);
    return (double(a.tv_nsec) * 1.0e-9) + double(a.tv_sec);
}


// mkl/sycl includes
#include <CL/sycl.hpp>
#include "oneapi/mkl.hpp"
#include <omp.h>
#include <unistd.h>

#include "mkl_vsl.h"

// local includes
#include "common_for_rng_examples.hpp"

// example parameters defines
#define N       (NS * S)
#define S       4
#define NS      1024
#define N_PRINT 100
#define N_TIMES 1000

template <typename RealType>
bool run_skip_ahead_example(sycl::queue& queue) {
    int NCPUs = sysconf(_SC_NPROCESSORS_CONF);
    printf("Using thread affinity on %i NCPUs\n", NCPUs);

    std::cout << "\n\tRun skip_ahead example" << std::endl;

    oneapi::mkl::rng::philox4x32x10 engine(queue);
    /*
        std::vector<oneapi::mkl::rng::philox4x32x10> engine_vec;

        for (int i = 0; i < S; i++) {
            // copy reference engine to engine_vec[i]
            engine_vec.push_back(oneapi::mkl::rng::philox4x32x10{engine});
            // skip ahead engine
            oneapi::mkl::rng::skip_ahead(engine_vec[i], i * NS);
        }
    */


    // prepare array for random numbers
    std::vector<RealType, mkl_allocator<RealType, 64>> r_ref(N);
//    std::vector<RealType, mkl_allocator<RealType, 64>> r(NS * S);

    {
        sycl::buffer<RealType, 1> r_ref_buffer(r_ref.data(), r_ref.size());

        try {
            {
                // fill r_ref with N random numbers

                double startStamp, endStamp;
                startStamp = getCurrentTimestamp();
                for (int i = 0; i < N_TIMES ; i++)
                {
                    oneapi::mkl::rng::generate(oneapi::mkl::rng::uniform<RealType> {}, engine, N, r_ref_buffer);
                    //
                }
                //queue.wait_and_throw();
                endStamp = getCurrentTimestamp();
                std::cout << "\t\tSingle Output:" << endStamp - startStamp << std::endl;
            }

            // fill r with random numbers by portions of NS
            /*
                        {

                            double startStamp, endStamp;
                            std::vector<sycl::buffer<RealType, 1>>  vector_r_buffer;
                            for (int i = 0; i < S; i++) {
                                sycl::buffer<RealType, 1> r_buffer(r.data() + i * NS, NS);
                                vector_r_buffer.push_back(r_buffer);
                            }

                            startStamp = getCurrentTimestamp();
                            #pragma omp parallel
                            {
                                cpu_set_t new_mask;
                                cpu_set_t was_mask;
                                int tid = omp_get_thread_num();

                                CPU_ZERO(&new_mask);

                                // 2 packages x 2 cores/pkg x 1 threads/core (4 total cores)
                                CPU_SET( tid, &new_mask);

                                if (sched_getaffinity(0, sizeof(was_mask), &was_mask) == -1) {
                                    printf("Error1: sched_getaffinity(%d, sizeof(was_mask), &was_mask)\n", tid);
                                }
                                if (sched_setaffinity(0, sizeof(new_mask), &new_mask) == -1) {
                                    printf("Error2: sched_setaffinity(%d, sizeof(new_mask), &new_mask)\n", tid);
                                }
                                if (sched_getaffinity(0, sizeof(new_mask), &new_mask) == -1) {
                                    printf("Error1: sched_getaffinity(%d, sizeof(was_mask), &was_mask)\n", tid);
                                }
                                printf("tid=%d new_mask=%08X was_mask=%08X\n", tid,
                                       *(unsigned int*)(&new_mask), *(unsigned int*)(&was_mask));
                                int i = omp_get_thread_num();
                                oneapi::mkl::rng::generate(oneapi::mkl::rng::uniform<RealType> {}, engine_vec[i], NS, vector_r_buffer[i]);
                            }
                            queue.wait_and_throw();
                            endStamp = getCurrentTimestamp();
                            std::cout << "\t\tMultiple Output:" << endStamp - startStamp << std::endl;
                        }
            */
        }
        catch (sycl::exception const& e) {
            std::cout << "\t\tSYCL exception during generation\n"
                      << e.what() << std::endl << "OpenCL status: " << e.get_cl_code() << std::endl;
            return false;
        }
    }
    std::cout << "Success" << std::endl;
    return true;
}


void print_example_banner() {

    std::cout << "" << std::endl;
    std::cout << "########################################################################" << std::endl;
    std::cout << "# Demonstrate service functionality usage for random number generators:" << std::endl;
    std::cout << "# " << std::endl;
    std::cout << "# Using apis:" << std::endl;
    std::cout << "#   oneapi::mkl::rng::skip_ahead" << std::endl;
    std::cout << "#   oneapi::mkl::rng::leapfrog" << std::endl;
    std::cout << "# " << std::endl;
    std::cout << "########################################################################" << std::endl;
    std::cout << std::endl;

}

//
// Main entry point for example.
//
// Dispatches to appropriate device types as set at build time with flag:
// -DSYCL_DEVICES_host -- only runs host implementation
// -DSYCL_DEVICES_cpu -- only runs SYCL CPU implementation
// -DSYCL_DEVICES_gpu -- only runs SYCL GPU implementation
// -DSYCL_DEVICES_all (default) -- runs on all: host, cpu and gpu devices
//
#include <mkl.h>


std::vector<oneapi::mkl::rng::philox4x32x10> engine_vec;

std::vector<sycl::queue> queue_vec;

std::vector<sycl::buffer<std::uint32_t, 1>> buffer_vec;

std::vector<VSLStreamStatePtr> stream_vec;


typedef struct {
    uint32_t id;
    pthread_t tid;
    void * data;
    uint64_t buffer[NS];
} local_rng;

local_rng rng_thread[S];
uint64_t single_buffer[NS * S];

#define MAX_CPU  (4)

void* generation_thread(void* data)
{
    local_rng* rng = (local_rng*)(data);
    //printf("%d\n", rng->id);
    cpu_set_t new_mask;
    cpu_set_t was_mask;
    uint32_t tid = (rng->id) % MAX_CPU;


    CPU_ZERO(&new_mask);

    // 2 packages x 2 cores/pkg x 1 threads/core (4 total cores)
    CPU_SET( tid, &new_mask);

    if (sched_getaffinity(0, sizeof(was_mask), &was_mask) == -1) {
        printf("Error1: sched_getaffinity(%d, sizeof(was_mask), &was_mask)\n", tid);
    }
    if (sched_setaffinity(0, sizeof(new_mask), &new_mask) == -1) {
        printf("Error2: sched_setaffinity(%d, sizeof(new_mask), &new_mask)\n", tid);
    }
    if (sched_getaffinity(0, sizeof(new_mask), &new_mask) == -1) {
        printf("Error1: sched_getaffinity(%d, sizeof(was_mask), &was_mask)\n", tid);
    }
    //printf("tid=%d new_mask=%08X was_mask=%08X\n", tid,
    //       *(unsigned int*)(&new_mask), *(unsigned int*)(&was_mask));
    double startStamp, endStamp;
    startStamp = getCurrentTimestamp();
    for (int i = 0; i < N_TIMES ; i++)
    {
        viRngUniformBits32( VSL_RNG_METHOD_UNIFORMBITS32_STD, stream_vec[rng->id], NS, (unsigned int *)rng->buffer);
        //oneapi::mkl::rng::generate(oneapi::mkl::rng::uniform<std::uint32_t> {}, engine_vec[rng->id], NS, buffer_vec[rng->id]);
        //
    }
    //queue_vec[rng->id].wait_and_throw();
    endStamp = getCurrentTimestamp();

    std::cout << "\t\t " << rng->id << " in:" << endStamp - startStamp << std::endl;

    pthread_exit(NULL);
}




int main (int argc, char ** argv) {
    // catch asynchronous exceptions
    auto exception_handler = [] (sycl::exception_list exceptions) {
        for (std::exception_ptr const& e : exceptions) {
            try {
                std::rethrow_exception(e);
            } catch (sycl::exception const& e) {
                std::cout << "Caught asynchronous SYCL exception during generation:\n"
                          << e.what() << std::endl;
            }
        }
    };

    print_example_banner();

    std::list<my_sycl_device_types> list_of_devices;
    set_list_of_devices(list_of_devices);

    for (auto it = list_of_devices.begin(); it != list_of_devices.end(); ++it) {

        sycl::device my_dev;
        bool my_dev_is_found = false;
        get_sycl_device(my_dev, my_dev_is_found, *it);

        if (my_dev_is_found) {
            for (int i = 0; i < S; i++) {
                VSLStreamStatePtr stream;
                vslNewStream( &stream, VSL_BRNG_MT19937, 777 );
                stream_vec.push_back( stream );

                //sycl::queue queue(my_dev, exception_handler);
                //queue_vec.push_back(queue);

                //oneapi::mkl::rng::philox4x32x10 engine(queue);
                //engine_vec.push_back(engine);
                //oneapi::mkl::rng::skip_ahead(engine_vec[i], i * NS);

                //sycl::buffer<std::uint32_t, 1> buffer((unsigned int *)rng_thread[i].buffer, NS);
                //buffer_vec.push_back(buffer);
            }
            double startStamp, endStamp;
            startStamp = getCurrentTimestamp();
            for (int i = 0; i < S; i++) {
                rng_thread[i].id = i;
                pthread_create(&rng_thread[i].tid, NULL, generation_thread, (void *)&rng_thread[i]) ;
            }
            for (int i = 0; i < S; i++)
                pthread_join(rng_thread[i].tid, NULL);
            endStamp = getCurrentTimestamp();
            std::cout << "\tTotal Time:" << endStamp - startStamp << std::endl;

            {
                VSLStreamStatePtr stream;
                vslNewStream( &stream, VSL_BRNG_MT19937, 777 );

                double startStamp, endStamp;
                startStamp = getCurrentTimestamp();
                for (int i = 0; i < N_TIMES ; i++)
                {
                    viRngUniformBits32( VSL_RNG_METHOD_UNIFORMBITS32_STD, stream, N, (unsigned int *)single_buffer);
                    //oneapi::mkl::rng::generate(oneapi::mkl::rng::uniform<std::uint32_t> {}, engine_vec[rng->id], NS, buffer_vec[rng->id]);
                    //
                }
                //queue_vec[rng->id].wait_and_throw();
                endStamp = getCurrentTimestamp();

                std::cout << "\tSingle" << " in: " << endStamp - startStamp << std::endl;
            }

            //std::cout << "Running tests on " << sycl_device_names[*it] << ".\n";
            //sycl::queue queue(my_dev, exception_handler);



            //run_skip_ahead_example<std::uint32_t>(queue);


        } else {
#ifdef FAIL_ON_MISSING_DEVICES
            std::cout << "No " << sycl_device_names[*it] << " devices found; Fail on missing devices is enabled.\n";
            std::cout << "FAILED" << std::endl;
            return 1;
#else
            std::cout << "No " << sycl_device_names[*it] << " devices found; skipping " << sycl_device_names[*it] << " tests.\n";
#endif
        }
    }
    std::cout << "PASSED" << std::endl;
    return 0;
}


