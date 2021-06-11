/*******************************************************************************
* Copyright 2019-2021 Intel Corporation.
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
*       This example demonstrates use of DPC++ API oneapi::mkl::rng::uniform distribution with
*       oneapi::mkl::rng::philox4x32x10 random number generator to produce random numbers on
*       a SYCL device (Host, CPU, GPU) with Unified Shared Memory(USM) API.
*
*       The supported data types for random numbers are:
*           float
*           double
*           std::int32_t
*           std::uint32_t
*
*******************************************************************************/

// stl includes
#include <iostream>
#include <vector>

// mkl/sycl includes
#include <CL/sycl.hpp>
#include "oneapi/mkl.hpp"

// local includes
#include "common_for_examples.hpp"

// example parameters defines
#define SEED    777
#define N       1000
#define N_PRINT 10

template <typename Type>
int check_statistics(Type* r, Type a, Type b);

//
// Main example for Uniform random number generation consisting of
// initialization of random number engine philox4x32x10 object, distribution
// object. Then random number generation performed and
// the output is post-processed and validated.
//
template <typename Type>
int run_uniform_example(const sycl::device &dev) {

    //
    // Initialization
    //

    // Catch asynchronous exceptions
    auto exception_handler = [] (sycl::exception_list exceptions) {
        for (std::exception_ptr const& e : exceptions) {
            try {
                std::rethrow_exception(e);
            } catch(sycl::exception const& e) {
                std::cout << "Caught asynchronous SYCL exception during generation:\n"
                << e.what() << std::endl;
            }
        }
    };

    sycl::queue queue(dev, exception_handler);

    sycl::context context = queue.get_context();

    // set scalar Type values
    Type a = set_fp_value(Type(0.0));
    Type b  = set_fp_value(Type(10.0));

    oneapi::mkl::rng::default_engine engine(queue, SEED);

    oneapi::mkl::rng::uniform<Type> distribution(a, b);

    // prepare array for random numbers
    sycl::usm_allocator<Type, sycl::usm::alloc::shared, 64> allocator(context, dev);
    std::vector <Type, sycl::usm_allocator<Type, sycl::usm::alloc::shared, 64>> r(N, allocator);

    //
    // Perform generation
    //

    try {
        auto event_out = oneapi::mkl::rng::generate(distribution, engine, N, r.data());
    }
    catch(sycl::exception const& e) {
        std::cout << "\t\tSYCL exception during generation\n"
                  << e.what() << std::endl << "OpenCL status: " << e.get_cl_code() << std::endl;
        return 1;
    }

    queue.wait_and_throw();

    //
    // Post Processing
    //

    std::cout << "\n\t\tgeneration parameters:\n";
    std::cout << "\t\t\tseed = " << SEED << ", a = " << a << ", b = " << b << std::endl;

    std::cout << "\n\t\tOutput of generator:" << std::endl;

    std::cout << "first "<< N_PRINT << " numbers of " << N << ": " << std::endl;
    for(int i = 0 ; i < N_PRINT; i++) {
        std::cout << r[i] << " ";
    }
    std::cout << std::endl;

    //
    // Validation
    //
    return check_statistics(r.data(), a, b);
}

//
// Description of example setup, APIs used and supported floating point type precisions
//
void print_example_banner() {

    std::cout << "" << std::endl;
    std::cout << "########################################################################" << std::endl;
    std::cout << "# Generate uniformly distributed random numbers with philox4x32x10\n# generator example: " << std::endl;
    std::cout << "# " << std::endl;
    std::cout << "# Using APIs:" << std::endl;
    std::cout << "#   default_engine uniform" << std::endl;
    std::cout << "# " << std::endl;
    std::cout << "# Supported precisions:" << std::endl;
    std::cout << "#   float double std::int32_t" << std::endl;
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

int main (int argc, char ** argv) {

    print_example_banner();

    std::list<my_sycl_device_types> list_of_devices;
    set_list_of_devices(list_of_devices);

    for (auto it = list_of_devices.begin(); it != list_of_devices.end(); ++it) {

        sycl::device my_dev;
        bool my_dev_is_found = false;
        get_sycl_device(my_dev, my_dev_is_found, *it);

            if(my_dev_is_found) {
                std::cout << "Running tests on " << sycl_device_names[*it] << ".\n";

                std::cout << "\tRunning with single precision real data type:" << std::endl;
                if(run_uniform_example<float>(my_dev)) {
                    std::cout << "FAILED" << std::endl;
                    return 1;
                }
                if(isDoubleSupported(my_dev)) {
                    std::cout << "\tRunning with double precision real data type:" << std::endl;
                    if(run_uniform_example<double>(my_dev)) {
                        std::cout << "FAILED" << std::endl;
                        return 1;
                    }
                }
                else {
                    std::cout << "Double precision is not supported for this device" << std::endl;
                }
                std::cout << "\tRunning with integer data type:" << std::endl;
                if(run_uniform_example<std::int32_t>(my_dev)) {
                    std::cout << "FAILED" << std::endl;
                    return 1;
                }
                if(run_uniform_example<std::uint32_t>(my_dev)) {
                    std::cout << "FAILED" << std::endl;
                    return 1;
                }
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

template <typename Type>
int check_statistics(Type* r, Type a, Type b) {
    double tM,tD,tQ,tD2;
    double sM,sD;
    double sum, sum2;
    double n,s;
    double DeltaM,DeltaD;

    /***** Theoretical moments *****/
    if constexpr (std::is_integral<Type>::value) {
        double a_ = (double) a;
        double b_ = (double) b;
        tM=(a_+b_-1.0)/2.0;
        tD=((b_-a_)*(b_-a_)-1.0)/12.0;
        tQ=(((b_-a_)*(b_-a_))*((1.0/80.0)*(b_-a_)*(b_-a_)-(1.0/24.0)))+(7.0/240.0);
    } else {
        tM=(b+a)/2.0;
        tD=((b-a)*(b-a))/12.0;
        tQ=((b-a)*(b-a)*(b-a)*(b-a))/80.0;
    }

    /***** Sample moments *****/
    sum=0.0;
    sum2=0.0;
    for(int i=0;i<N;i++) {
        sum+=(double)r[i];
        sum2+=(double)r[i]*(double)r[i];
    }
    sM=sum/((double)N);
    sD=sum2/(double)N-(sM*sM);

    /***** Comparison of theoretical and sample moments *****/
    n=(double)N;
    tD2=tD*tD;
    s=((tQ-tD2)/n)-(2*(tQ-2*tD2)/(n*n))+((tQ-3*tD2)/(n*n*n));

    DeltaM=(tM-sM)/sqrt(tD/n);
    DeltaD=(tD-sD)/sqrt(s);

    if(fabs(DeltaM)>3.0 || fabs(DeltaD)>3.0) {
        std::cout << "Error: sample moments (mean=" << sM << ", variance=" << sD
            << ") disagree with theory (mean=" << tM << ", variance=" << tD <<
            ")" << std:: endl;
        return 1;
    }
    else {
        std::cout << "Success: sample moments (mean=" << sM << ", variance=" << sD
            << ") agree with theory (mean=" << tM << ", variance=" << tD <<
            ")" << std:: endl;
        return 0;
    }
}
