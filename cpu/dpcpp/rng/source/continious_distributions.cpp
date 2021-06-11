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
*       continious distributions with oneapi::mkl::rng::default_engine
*       random number generator
*
*       Continious distribution list:
*           oneapi::mkl::rng::beta                     (available for host and CPU)
*           oneapi::mkl::rng::cauchy                   (available for host, CPU and GPU)
*           oneapi::mkl::rng::chi_square               (available for host, CPU and GPU)
*           oneapi::mkl::rng::exponential              (available for host, CPU and GPU)
*           oneapi::mkl::rng::gamma                    (available for host, and CPU)
*           oneapi::mkl::rng::gaussian                 (available for host, CPU and GPU)
*           oneapi::mkl::rng::gumbel                   (available for host, CPU and GPU)
*           oneapi::mkl::rng::laplace                  (available for host, CPU and GPU)
*           oneapi::mkl::rng::lognormal                (available for host, CPU and GPU)
*           oneapi::mkl::rng::rayleigh                 (available for host, CPU and GPU)
*           oneapi::mkl::rng::weibull                  (available for host, CPU and GPU)
*
*       The supported floating point data types for random numbers are:
*           float
*           double
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

template <typename RealType>
bool run_beta_example(sycl::queue& queue) {
    oneapi::mkl::rng::default_engine engine(queue);
    // set shape p
    RealType p = set_fp_value(RealType(2.0));
    // set shape q
    RealType q  = set_fp_value(RealType(10.0));
    // set displacement
    RealType a  = set_fp_value(RealType(0.0));
    // set scalefactor
    RealType b  = set_fp_value(RealType(1.0));

    oneapi::mkl::rng::beta<RealType> distribution(p, q, a, b);

    // prepare array for random numbers
    std::vector<RealType, mkl_allocator<RealType, 64>> r(N);

    {
        sycl::buffer<RealType, 1> r_buffer(r.data(), r.size());

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
        std::cout << "\n\t\tOutput of generator with beta distribution:" << std::endl;
        print_output(r_buffer, N_PRINT);
    } // buffer life-time ends

    // validation
    return statistics<RealType, oneapi::mkl::rng::beta<RealType>>{}.check(r, distribution);
}

template <typename RealType>
bool run_cauchy_example(sycl::queue& queue) {
    oneapi::mkl::rng::default_engine engine(queue);

    // set displacement
    RealType a  = set_fp_value(RealType(0.0));
    // set scalefactor
    RealType b  = set_fp_value(RealType(1.0));

    oneapi::mkl::rng::cauchy<RealType> distribution(a, b);

    // prepare array for random numbers
    std::vector<RealType, mkl_allocator<RealType, 64>> r(N);

    {
        sycl::buffer<RealType, 1> r_buffer(r.data(), r.size());

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
        std::cout << "\n\t\tOutput of generator with cauchy distribution:" << std::endl;
        print_output(r_buffer, N_PRINT);
    } // buffer life-time ends

    return true;
}

template <typename RealType>
bool run_chi_square_example(sycl::queue& queue) {
    oneapi::mkl::rng::default_engine engine(queue);

    // set degrees of freedom
    std::int32_t n = 10;

    oneapi::mkl::rng::chi_square<RealType> distribution(n);

    // prepare array for random numbers
    std::vector<RealType, mkl_allocator<RealType, 64>> r(N);

    {
        sycl::buffer<RealType, 1> r_buffer(r.data(), r.size());

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
        std::cout << "\n\t\tOutput of generator with chi_square distribution:" << std::endl;
        print_output(r_buffer, N_PRINT);
    } // buffer life-time ends

    // validation
    return statistics<RealType, oneapi::mkl::rng::chi_square<RealType>>{}.check(r, distribution);
}

template <typename RealType>
bool run_exponential_example(sycl::queue& queue) {
    oneapi::mkl::rng::default_engine engine(queue);

     // set displacement
    RealType a  = set_fp_value(RealType(0.0));
    // set scalefactor
    RealType beta  = set_fp_value(RealType(1.0));

    oneapi::mkl::rng::exponential<RealType> distribution(a, beta);

    // prepare array for random numbers
    std::vector<RealType, mkl_allocator<RealType, 64>> r(N);

    {
        sycl::buffer<RealType, 1> r_buffer(r.data(), r.size());

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
        std::cout << "\n\t\tOutput of generator with exponential distribution:" << std::endl;
        print_output(r_buffer, N_PRINT);
    } // buffer life-time ends

    // validation
    return statistics<RealType, oneapi::mkl::rng::exponential<RealType>>{}.check(r, distribution);
}


template <typename RealType>
bool run_gamma_example(sycl::queue& queue) {
    oneapi::mkl::rng::default_engine engine(queue);

     // set displacement
    RealType a  = set_fp_value(RealType(0.0));
    // set scalefactor
    RealType beta  = set_fp_value(RealType(1.0));

    oneapi::mkl::rng::exponential<RealType> distribution(a, beta);

    // prepare array for random numbers
    std::vector<RealType, mkl_allocator<RealType, 64>> r(N);

    {
        sycl::buffer<RealType, 1> r_buffer(r.data(), r.size());

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
        std::cout << "\n\t\tOutput of generator with exponential distribution:" << std::endl;
        print_output(r_buffer, N_PRINT);
    } // buffer life-time ends

    // validation
    return statistics<RealType, oneapi::mkl::rng::exponential<RealType>>{}.check(r, distribution);
}

template <typename RealType>
bool run_gaussian_example(sycl::queue& queue) {
    oneapi::mkl::rng::default_engine engine(queue);

    // set mean value
    RealType mean = set_fp_value(RealType(0.0));
    // set standard deviation
    RealType stddev  = set_fp_value(RealType(1.0));

    oneapi::mkl::rng::gaussian<RealType> distribution(mean, stddev);

    // prepare array for random numbers
    std::vector<RealType, mkl_allocator<RealType, 64>> r(N);

    {
        sycl::buffer<RealType, 1> r_buffer(r.data(), r.size());

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
        std::cout << "\n\t\tOutput of generator with gaussian distribution:" << std::endl;
        print_output(r_buffer, N_PRINT);
    } // buffer life-time ends

    // validation
    return statistics<RealType, oneapi::mkl::rng::gaussian<RealType>>{}.check(r, distribution);
}

template <typename RealType>
bool run_gaussian_mv_example(sycl::queue& queue) {
    oneapi::mkl::rng::default_engine engine(queue);

    // set quantity of dimensions
    std::uint32_t dimen = 3;
    // set mean values for each dimension
    std::vector<RealType> mean = {3.0, 5.0, 2.0};
    // set lower triangular matrix T (by default packed layout is used), it can be
    // computed using Cholesky factorization to the source symmetric matrix.
    // Source symmetric matrix can be computed as T * T' where T' is transposed matrix
    // for this case source symmetric matrix is:
    //   16.0     8.0     4.0
    //    8.0    13.0    17.0
    //    4.0    17.0    62.0
    // where diagonal elements are dispersions and other elements are cross covariation coefficients
    std::vector<RealType> lower_triang_matrix  = {4.0, 2.0, 1.0, 3.0, 5.0, 6.0};

    oneapi::mkl::rng::gaussian_mv<RealType> distribution(dimen, mean, lower_triang_matrix);

    // prepare array for random numbers
    std::vector<RealType, mkl_allocator<RealType, 64>> r(N * dimen);

    {
        sycl::buffer<RealType, 1> r_buffer(r.data(), r.size());

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
        std::cout << "\n\t\tOutput of generator with gaussian_mv distribution:" << std::endl;
        print_output(r_buffer, N_PRINT);
    } // buffer life-time ends

    return statistics<RealType, oneapi::mkl::rng::gaussian_mv<RealType>>{}.check(r, distribution);
}

template <typename RealType>
bool run_gumbel_example(sycl::queue& queue) {
    oneapi::mkl::rng::default_engine engine(queue);

    // set displacement
    RealType a  = set_fp_value(RealType(0.0));
     // set scalefactor
    RealType b  = set_fp_value(RealType(1.0));

    oneapi::mkl::rng::gumbel<RealType> distribution(a, b);

    // prepare array for random numbers
    std::vector<RealType, mkl_allocator<RealType, 64>> r(N);

    {
        sycl::buffer<RealType, 1> r_buffer(r.data(), r.size());

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
        std::cout << "\n\t\tOutput of generator with gumbel distribution:" << std::endl;
        print_output(r_buffer, N_PRINT);
    } // buffer life-time ends

    // validation
    return statistics<RealType, oneapi::mkl::rng::gumbel<RealType>>{}.check(r, distribution);
}

template <typename RealType>
bool run_laplace_example(sycl::queue& queue) {
    oneapi::mkl::rng::default_engine engine(queue);

    // set mean value
    RealType a  = set_fp_value(RealType(0.0));
    // set scalefactor
    RealType b  = set_fp_value(RealType(1.0));

    oneapi::mkl::rng::laplace<RealType> distribution(a, b);

    // prepare array for random numbers
    std::vector<RealType, mkl_allocator<RealType, 64>> r(N);

    {
        sycl::buffer<RealType, 1> r_buffer(r.data(), r.size());

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
        std::cout << "\n\t\tOutput of generator with laplace distribution:" << std::endl;
        print_output(r_buffer, N_PRINT);
    } // buffer life-time ends

    // validation
    return statistics<RealType, oneapi::mkl::rng::laplace<RealType>>{}.check(r, distribution);
}

template <typename RealType>
bool run_lognormal_example(sycl::queue& queue) {
    oneapi::mkl::rng::default_engine engine(queue);

    // set average of the subject normal distribution
    RealType m = set_fp_value(RealType(0.0));
    // set standard deviation of the subject normal distribution
    RealType s  = set_fp_value(RealType(1.0));
    // set displacement
    RealType displ = set_fp_value(RealType(0.0));
    // set scalefactor
    RealType scale = set_fp_value(RealType(1.0));

    oneapi::mkl::rng::lognormal<RealType> distribution(m, s, displ, scale);

    // prepare array for random numbers
    std::vector<RealType, mkl_allocator<RealType, 64>> r(N);

    {
        sycl::buffer<RealType, 1> r_buffer(r.data(), r.size());

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
        std::cout << "\n\t\tOutput of generator with lognormal distribution:" << std::endl;
        print_output(r_buffer, N_PRINT);
    } // buffer life-time ends

    // validation
    return statistics<RealType, oneapi::mkl::rng::lognormal<RealType>>{}.check(r, distribution);
}

template <typename RealType>
bool run_rayleigh_example(sycl::queue& queue) {
    oneapi::mkl::rng::default_engine engine(queue);

    // set displacement
    RealType a  = set_fp_value(RealType(0.0));
    // set scalefactor
    RealType b  = set_fp_value(RealType(1.0));

    oneapi::mkl::rng::rayleigh<RealType> distribution(a, b);

    // prepare array for random numbers
    std::vector<RealType, mkl_allocator<RealType, 64>> r(N);

    {
        sycl::buffer<RealType, 1> r_buffer(r.data(), r.size());

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
        std::cout << "\n\t\tOutput of generator with rayleigh distribution:" << std::endl;
        print_output(r_buffer, N_PRINT);
    } // buffer life-time ends

    // validation
    return statistics<RealType, oneapi::mkl::rng::rayleigh<RealType>>{}.check(r, distribution);
}

template <typename RealType>
bool run_weibull_example(sycl::queue& queue) {
    oneapi::mkl::rng::default_engine engine(queue);

    // set shape
    RealType alpha  = set_fp_value(RealType(1.0));
    // set displacement
    RealType a  = set_fp_value(RealType(0.0));
    // set scalefactor
    RealType beta  = set_fp_value(RealType(1.0));

    oneapi::mkl::rng::weibull<RealType> distribution(alpha, a, beta);

    // prepare array for random numbers
    std::vector<RealType, mkl_allocator<RealType, 64>> r(N);

    {
        sycl::buffer<RealType, 1> r_buffer(r.data(), r.size());

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
        std::cout << "\n\t\tOutput of generator with weibull distribution:" << std::endl;
        print_output(r_buffer, N_PRINT);
    } // buffer life-time ends

    // validation
    return statistics<RealType, oneapi::mkl::rng::weibull<RealType>>{}.check(r, distribution);
}

void print_example_banner() {

    std::cout << "" << std::endl;
    std::cout << "########################################################################" << std::endl;
    std::cout << "# Generate random numbers with continious rng distributions:" << std::endl;
    std::cout << "# " << std::endl;
    std::cout << "# Using apis:" << std::endl;
    std::cout << "#   oneapi::mkl::rng::beta" << std::endl;
    std::cout << "#   oneapi::mkl::rng::cauchy" << std::endl;
    std::cout << "#   oneapi::mkl::rng::chi_square" << std::endl;
    std::cout << "#   oneapi::mkl::rng::exponential" << std::endl;
    std::cout << "#   oneapi::mkl::rng::gamma" << std::endl;
    std::cout << "#   oneapi::mkl::rng::gaussian" << std::endl;
    std::cout << "#   oneapi::mkl::rng::gumbel" << std::endl;
    std::cout << "#   oneapi::mkl::rng::laplace" << std::endl;
    std::cout << "#   oneapi::mkl::rng::lognormal" << std::endl;
    std::cout << "#   oneapi::mkl::rng::rayleigh" << std::endl;
    std::cout << "#   oneapi::mkl::rng::weibull" << std::endl;
    std::cout << "# " << std::endl;
    std::cout << "# Supported floating point type precisions:" << std::endl;
    std::cout << "#   float double" << std::endl;
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
                std::cout << "\tRunning with single precision real data type:" << std::endl;
                if(!run_beta_example<float>(queue) ||
                    !run_cauchy_example<float>(queue) ||
                    !run_chi_square_example<float>(queue) ||
                    !run_exponential_example<float>(queue) ||
                    !run_gamma_example<float>(queue)  ||
                    !run_gaussian_example<float>(queue) ||
                    !run_gaussian_mv_example<float>(queue) ||
                    !run_gumbel_example<float>(queue) ||
                    !run_laplace_example<float>(queue) ||
                    !run_lognormal_example<float>(queue) ||
                    !run_rayleigh_example<float>(queue) ||
                    !run_weibull_example<float>(queue)) {
                    std::cout << "FAILED" << std::endl;
                    return 1;
                }
                if(my_dev.get_info<sycl::info::device::double_fp_config>().size() != 0) {
                    std::cout << "\tRunning with double precision real data type:" << std::endl;
                    if(!run_beta_example<double>(queue) ||
                    !run_cauchy_example<double>(queue) ||
                    !run_chi_square_example<double>(queue) ||
                    !run_exponential_example<double>(queue) ||
                    !run_gamma_example<double>(queue) ||
                    !run_gaussian_example<double>(queue) ||
                    !run_gaussian_mv_example<double>(queue) ||
                    !run_gumbel_example<double>(queue) ||
                    !run_laplace_example<double>(queue) ||
                    !run_lognormal_example<double>(queue) ||
                    !run_rayleigh_example<double>(queue) ||
                    !run_weibull_example<double>(queue)) {
                        std::cout << "FAILED" << std::endl;
                        return 1;
                    }
                }
                else {
                    std::cout << "Double precision is not supported for this device" << std::endl;
                }
            } else {
                std::cout << "\tRunning with single precision real data type:" << std::endl;
                if(!run_cauchy_example<float>(queue) ||
                    !run_exponential_example<float>(queue) ||
                    !run_gaussian_example<float>(queue) ||
                    !run_gaussian_mv_example<float>(queue) ||
                    !run_gumbel_example<float>(queue) ||
                    !run_laplace_example<float>(queue) ||
                    !run_lognormal_example<float>(queue) ||
                    !run_rayleigh_example<float>(queue) ||
                    !run_weibull_example<float>(queue)) {
                    std::cout << "FAILED" << std::endl;
                    return 1;
                }
                if(isDoubleSupported(my_dev)) {
                    std::cout << "\tRunning with double precision real data type:" << std::endl;
                    if(!run_cauchy_example<double>(queue) ||
                    !run_exponential_example<double>(queue) ||
                    !run_gaussian_example<double>(queue) ||
                    !run_gaussian_mv_example<double>(queue) ||
                    !run_gumbel_example<double>(queue) ||
                    !run_laplace_example<double>(queue) ||
                    !run_lognormal_example<double>(queue) ||
                    !run_rayleigh_example<double>(queue) ||
                    !run_weibull_example<double>(queue)) {
                        std::cout << "FAILED" << std::endl;
                        return 1;
                    }
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
