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
*       This example demonstrates usage of DPC++ buffer-based API for MKL RNG
*       discrete distributions with oneapi::mkl::rng::default_engine
*       random number generator
*
*       Continious distribution list:
*           oneapi::mkl::rng::bernoulli        (available for host, CPU and GPU)
*           oneapi::mkl::rng::binomial         (available for host and CPU)
*           oneapi::mkl::rng::geometric        (available for host, CPU and GPU)
*           oneapi::mkl::rng::hypergeometric   (available for host and CPU)
*           oneapi::mkl::rng::multinomial      (available for host and CPU)
*           oneapi::mkl::rng::negative_binomial(available for host and CPU)
*           oneapi::mkl::rng::poisson          (available for host, CPU and GPU)
*           oneapi::mkl::rng::poisson_v        (available for host and CPU)
*           oneapi::mkl::rng::uniform_bits     (available for host, CPU and GPU)
*           oneapi::mkl::rng::bits             (available for host, CPU and GPU)

*       The supported data types for random numbers are:
*           int32_t
*           uint32_t
*           uint64_t (for uniform_bits)
*
*
*******************************************************************************/

// stl includes
#include <iostream>
#include <vector>

// mkl/sycl includes
#include <CL/sycl.hpp>
#include "oneapi/mkl.hpp"

// local includes
#include "common_for_rng_examples.hpp"

// example parameters defines
#define N       2000
#define N_PRINT 10

template <typename IntType>
bool run_bernoulli_example(sycl::queue& queue) {
    oneapi::mkl::rng::default_engine engine(queue);
    // set success probability
    float p = 0.5f;

    oneapi::mkl::rng::bernoulli<IntType> distribution(p);

    // prepare array for random numbers
    std::vector<IntType, mkl_allocator<IntType, 64>> r(N);

    {
        sycl::buffer<IntType, 1> r_buffer(r.data(), r.size());

        try {
            // perform generation
            oneapi::mkl::rng::generate(distribution, engine, N, r_buffer);
            queue.wait_and_throw();
        }
        catch(sycl::exception const& e) {
            std::cout << "\t\tSYCL exception during generation\n"
                      << e.what() << std::endl << "OpenCL status: " << e.get_cl_code() << std::endl;
            return false;
        }

        // print output
        std::cout << "\n\t\tOutput of generator with bernoulli distribution:" << std::endl;
        print_output(r_buffer, N_PRINT);
    } // buffer life-time ends

    // validation
    return statistics<IntType, oneapi::mkl::rng::bernoulli<IntType>>{}.check(r, distribution);
}

template <typename IntType>
bool run_binomial_example(sycl::queue& queue) {
    oneapi::mkl::rng::default_engine engine(queue);

    // set number of independent trials
    std::int32_t ntrial = 10;
    // set probability
    double p = 0.5;

    oneapi::mkl::rng::binomial<IntType> distribution(ntrial, p);

    // prepare array for random numbers
    std::vector<IntType, mkl_allocator<IntType, 64>> r(N);

    {
        sycl::buffer<IntType, 1> r_buffer(r.data(), r.size());

        try {
            // perform generation
            oneapi::mkl::rng::generate(distribution, engine, N, r_buffer);
            queue.wait_and_throw();
        }
        catch(sycl::exception const& e) {
            std::cout << "\t\tSYCL exception during generation\n"
                      << e.what() << std::endl << "OpenCL status: " << e.get_cl_code() << std::endl;
            return false;
        }

        // print output
        std::cout << "\n\t\tOutput of generator with binomial distribution:" << std::endl;
        print_output(r_buffer, N_PRINT);
    } // buffer life-time ends

    // validation
    return statistics<IntType, oneapi::mkl::rng::binomial<IntType>>{}.check(r, distribution);
}

template <typename IntType>
bool run_geometric_example(sycl::queue& queue) {
    oneapi::mkl::rng::default_engine engine(queue);

    // set probability of trial
    float p = 0.5f;

    oneapi::mkl::rng::geometric<IntType> distribution(p);

    // prepare array for random numbers
    std::vector<IntType, mkl_allocator<IntType, 64>> r(N);

    {
        sycl::buffer<IntType, 1> r_buffer(r.data(), r.size());

        try {
            // perform generation
            oneapi::mkl::rng::generate(distribution, engine, N, r_buffer);
            queue.wait_and_throw();
        }
        catch(sycl::exception const& e) {
            std::cout << "\t\tSYCL exception during generation\n"
                      << e.what() << std::endl << "OpenCL status: " << e.get_cl_code() << std::endl;
            return false;
        }

        // print output
        std::cout << "\n\t\tOutput of generator with geometric distribution:" << std::endl;
        print_output(r_buffer, N_PRINT);
    } // buffer life-time ends

    // validation
    return statistics<IntType, oneapi::mkl::rng::geometric<IntType>>{}.check(r, distribution);
}

template <typename IntType>
bool run_hypergeometric_example(sycl::queue& queue) {
    oneapi::mkl::rng::default_engine engine(queue);

    // set lot size
    std::int32_t l = 100;
    // set size of sampling without replacement
    std::int32_t s = 10;
    // set number of marked elements
    std::int32_t m = 30;

    oneapi::mkl::rng::hypergeometric<IntType> distribution(l, s, m);

    // prepare array for random numbers
    std::vector<IntType, mkl_allocator<IntType, 64>> r(N);

    {
        sycl::buffer<IntType, 1> r_buffer(r.data(), r.size());

        try {
            // perform generation
            oneapi::mkl::rng::generate(distribution, engine, N, r_buffer);
            queue.wait_and_throw();
        }
        catch(sycl::exception const& e) {
            std::cout << "\t\tSYCL exception during generation\n"
                      << e.what() << std::endl << "OpenCL status: " << e.get_cl_code() << std::endl;
            return false;
        }

        // print output
        std::cout << "\n\t\tOutput of generator with hypergeometric distribution:" << std::endl;
        print_output(r_buffer, N_PRINT);
    } // buffer life-time ends

    // validation
    return statistics<IntType, oneapi::mkl::rng::hypergeometric<IntType>>{}.check(r, distribution);
}

template <typename IntType>
bool run_multinomial_example(sycl::queue& queue) {
    oneapi::mkl::rng::default_engine engine(queue);

    // set number of independent trials
    std::int32_t ntrial = 10;
    // set probability vector of possible outcomes
    std::vector<double> p{0.2, 0.2, 0.2, 0.2, 0.2};

    oneapi::mkl::rng::multinomial<IntType> distribution(ntrial, p);

    // prepare array for random numbers
    std::vector<IntType, mkl_allocator<IntType, 64>> r(N * p.size());

    {
        sycl::buffer<IntType, 1> r_buffer(r.data(), r.size());

        try {
            // perform generation
            oneapi::mkl::rng::generate(distribution, engine, N, r_buffer);
            queue.wait_and_throw();
        }
        catch(sycl::exception const& e) {
            std::cout << "\t\tSYCL exception during generation\n"
                      << e.what() << std::endl << "OpenCL status: " << e.get_cl_code() << std::endl;
            return false;
        }

        // print output
        std::cout << "\n\t\tOutput of generator with multinomial distribution:" << std::endl;
        print_output(r_buffer, N_PRINT);
    } // buffer life-time ends

    // validation
    return statistics<IntType, oneapi::mkl::rng::multinomial<IntType>>{}.check(r, distribution);
}

template <typename IntType>
bool run_negative_binomial_example(sycl::queue& queue) {
    oneapi::mkl::rng::default_engine engine(queue);

    // set the first distribution parameter
    double a = 1.0;
    // set the second distribution parameter
    double p = 0.75;

    oneapi::mkl::rng::negative_binomial<IntType> distribution(a, p);

    // prepare array for random numbers
    std::vector<IntType, mkl_allocator<IntType, 64>> r(N);

    {
        sycl::buffer<IntType, 1> r_buffer(r.data(), r.size());

        try {
            // perform generation
            oneapi::mkl::rng::generate(distribution, engine, N, r_buffer);
            queue.wait_and_throw();
        }
        catch(sycl::exception const& e) {
            std::cout << "\t\tSYCL exception during generation\n"
                      << e.what() << std::endl << "OpenCL status: " << e.get_cl_code() << std::endl;
            return false;
        }

        // print output
        std::cout << "\n\t\tOutput of generator with negative_binomial distribution:" << std::endl;
        print_output(r_buffer, N_PRINT);
    } // buffer life-time ends

    // validation
    return statistics<IntType, oneapi::mkl::rng::negative_binomial<IntType>>{}.check(r, distribution);
}

template <typename IntType>
bool run_poisson_example(sycl::queue& queue) {
    oneapi::mkl::rng::default_engine engine(queue);

    // set distribution parameter
    double lambda = 1.0;

    oneapi::mkl::rng::poisson<IntType> distribution(lambda);

    // prepare array for random numbers
    std::vector<IntType, mkl_allocator<IntType, 64>> r(N);

    {
        sycl::buffer<IntType, 1> r_buffer(r.data(), r.size());

        try {
            // perform generation
            oneapi::mkl::rng::generate(distribution, engine, N, r_buffer);
            queue.wait_and_throw();
        }
        catch(sycl::exception const& e) {
            std::cout << "\t\tSYCL exception during generation\n"
                      << e.what() << std::endl << "OpenCL status: " << e.get_cl_code() << std::endl;
            return false;
        }

        // print output
        std::cout << "\n\t\tOutput of generator with poisson distribution:" << std::endl;
        print_output(r_buffer, N_PRINT);
    } // buffer life-time ends

    // validation
    return statistics<IntType, oneapi::mkl::rng::poisson<IntType>>{}.check(r, distribution);
}

template <typename IntType>
bool run_poisson_v_example(sycl::queue& queue) {
    oneapi::mkl::rng::default_engine engine(queue);

    std::vector<double> lambda(N, 1.0);

    oneapi::mkl::rng::poisson_v<IntType> distribution(lambda);

    // prepare array for random numbers
    std::vector<IntType, mkl_allocator<IntType, 64>> r(N);

    {
        sycl::buffer<IntType, 1> r_buffer(r.data(), r.size());

        try {
            // perform generation
            oneapi::mkl::rng::generate(distribution, engine, N, r_buffer);
            queue.wait_and_throw();
        }
        catch(sycl::exception const& e) {
            std::cout << "\t\tSYCL exception during generation\n"
                      << e.what() << std::endl << "OpenCL status: " << e.get_cl_code() << std::endl;
            return false;
        }

        // print output
        std::cout << "\n\t\tOutput of generator with poisson_v distribution:" << std::endl;
        print_output(r_buffer, N_PRINT);
    } // buffer life-time ends

    // validation
    return statistics<IntType, oneapi::mkl::rng::poisson_v<IntType>>{}.check(r, distribution);
}

template <typename IntType>
bool run_uniform_bits_example(sycl::queue& queue) {
    oneapi::mkl::rng::default_engine engine(queue);

    // distribution provides Uniformly distributed bits in 32/64-bit chunks
    oneapi::mkl::rng::uniform_bits<IntType> distribution;

    // prepare array for random numbers
    std::vector<IntType, mkl_allocator<IntType, 64>> r(N);

    {
        sycl::buffer<IntType, 1> r_buffer(r.data(), r.size());

        try {
            // perform generation
            oneapi::mkl::rng::generate(distribution, engine, N, r_buffer);
            queue.wait_and_throw();
        }
        catch(sycl::exception const& e) {
            std::cout << "\t\tSYCL exception during generation\n"
                      << e.what() << std::endl << "OpenCL status: " << e.get_cl_code() << std::endl;
            return false;
        }

        // print output
        std::cout << "\n\t\tOutput of generator with uniform_bits distribution:" << std::endl;
        print_output(r_buffer, N_PRINT);
    } // buffer life-time ends

    return true;
}

template <typename IntType>
bool run_bits_example(sycl::queue& queue) {
    oneapi::mkl::rng::default_engine engine(queue);

    // distributions provides  bits of underlying engine integer reccurents
    oneapi::mkl::rng::bits<IntType> distribution;

    // prepare array for random numbers
    std::vector<IntType, mkl_allocator<IntType, 64>> r(N);

    {
        sycl::buffer<IntType, 1> r_buffer(r.data(), r.size());

        try {
            // perform generation
            oneapi::mkl::rng::generate(distribution, engine, N, r_buffer);
            queue.wait_and_throw();
        }
        catch(sycl::exception const& e) {
            std::cout << "\t\tSYCL exception during generation\n"
                      << e.what() << std::endl << "OpenCL status: " << e.get_cl_code() << std::endl;
            return false;
        }

        // print output
        std::cout << "\n\t\tOutput of generator with bits distribution:" << std::endl;
        print_output(r_buffer, N_PRINT);
    } // buffer life-time ends

    return true;
}

void print_example_banner() {

    std::cout << "" << std::endl;
    std::cout << "########################################################################" << std::endl;
    std::cout << "# Generate random numbers with discrete rng distributions:" << std::endl;
    std::cout << "# " << std::endl;
    std::cout << "# Using apis:" << std::endl;
    std::cout << "#   oneapi::mkl::rng::bernoulli" << std::endl;
    std::cout << "#   oneapi::mkl::rng::binomial" << std::endl;
    std::cout << "#   oneapi::mkl::rng::geometric" << std::endl;
    std::cout << "#   oneapi::mkl::rng::hypergeometric" << std::endl;
    std::cout << "#   oneapi::mkl::rng::multinomial" << std::endl;
    std::cout << "#   oneapi::mkl::rng::negative_binomial" << std::endl;
    std::cout << "#   oneapi::mkl::rng::poisson" << std::endl;
    std::cout << "#   oneapi::mkl::rng::poisson_v" << std::endl;
    std::cout << "#   oneapi::mkl::rng::uniform_bits" << std::endl;
    std::cout << "#   oneapi::mkl::rng::bits" << std::endl;
    std::cout << "# " << std::endl;
    std::cout << "# Supported types:" << std::endl;
    std::cout << "#   int32_t uint32_t uint64_t" << std::endl;
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

    // catch asynchronous exceptions
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

    print_example_banner();

    std::list<my_sycl_device_types> list_of_devices;
    set_list_of_devices(list_of_devices);

    for(auto it = list_of_devices.begin(); it != list_of_devices.end(); ++it) {

        sycl::device my_dev;
        bool my_dev_is_found = false;
        get_sycl_device(my_dev, my_dev_is_found, *it);

        if(my_dev_is_found) {
            std::cout << "Running tests on " << sycl_device_names[*it] << ".\n";
            sycl::queue queue(my_dev, exception_handler);
            if(my_dev.is_host() || my_dev.is_cpu()) {
                if(!run_bernoulli_example<std::int32_t>(queue) || !run_bernoulli_example<std::uint32_t>(queue) ||
                    !run_binomial_example<std::int32_t>(queue) || !run_binomial_example<std::uint32_t>(queue) ||
                    !run_geometric_example<std::int32_t>(queue) || !run_geometric_example<std::uint32_t>(queue) ||
                    !run_hypergeometric_example<std::int32_t>(queue) || !run_hypergeometric_example<std::uint32_t>(queue) ||
                    !run_multinomial_example<std::int32_t>(queue) || !run_multinomial_example<std::uint32_t>(queue) ||
                    !run_negative_binomial_example<std::int32_t>(queue) || !run_negative_binomial_example<std::uint32_t>(queue) ||
                    !run_poisson_example<std::int32_t>(queue) || !run_poisson_example<std::uint32_t>(queue) ||
                    !run_poisson_v_example<std::int32_t>(queue) || !run_poisson_v_example<std::uint32_t>(queue) ||
                    !run_uniform_bits_example<std::uint32_t>(queue) || !run_uniform_bits_example<std::uint64_t>(queue) ||
                    !run_bits_example<std::uint32_t>(queue)) {
                    std::cout << "FAILED" << std::endl;
                    return 1;
                }
            } else {
                if(!run_bernoulli_example<std::int32_t>(queue) || !run_bernoulli_example<std::uint32_t>(queue) ||
                    !run_geometric_example<std::int32_t>(queue) || !run_geometric_example<std::uint32_t>(queue) ||
                    !run_poisson_example<std::int32_t>(queue) || !run_poisson_example<std::uint32_t>(queue) ||
                    !run_uniform_bits_example<std::uint32_t>(queue) || !run_uniform_bits_example<std::uint64_t>(queue) ||
                    !run_bits_example<std::uint32_t>(queue)) {
                    std::cout << "FAILED" << std::endl;
                    return 1;
                }
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
