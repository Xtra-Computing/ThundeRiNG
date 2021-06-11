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

//  Content:
//    Use of Intel(R) oneMKL Sobol QRNG with direction numbers designed
//    by S. Joe,  F. Y. Kuo and supporting dimension up to 21201
//    Example Program Text
//*****************************************************************************/

//   This sample demonsrates registration of user-defined parameters
//   in Sobol QRNG. Set of initial Sobol parameters used here
//   follows to [1],[2].

//   Bibliography
//   1. S. Joe,  F. Y. Kuo. Remark on Algorithm 659: Implementing Sobol_s
//   Quasirandom Sequence Generator. ACM Transactions on Mathematical Software,
//   Vol. 29, No. 1, 50-57, March 2003
//   2. http://web.maths.unsw.edu.au/~fkuo/sobol/

// stl includes
#include <iostream>
#include <fstream>
#include <string>

// mkl/sycl includes
#include <CL/sycl.hpp>
#include "oneapi/mkl/rng.hpp"

// local includes
#include "common_for_rng_examples.hpp"

#define ALIGN_FACTOR_FOR_DIR_NUMS   32
// dimension of vectors to generate
#define DIM                         250
// number of vectors to generate in one block
#define M                           10000
// number of blocks of vectors to generate
#define NBLOCKS                     100
// size of buffer for quasi-random sequence
#define N                           (M * DIM)

// print parameters
#define DIM_TO_PRINT                5
#define N_PRINT                     10

// Routine to read initial Sobol direction numbers
static void read_sobol_dir_nums(std::string filename, std::uint32_t dimen, std::vector<std::uint32_t>& init_vec) {
    std::vector<std::uint32_t> polynomials(dimen, 0);
    std::vector<std::uint32_t> poly_degree(dimen, 0);
    std::ifstream stream(filename);
    for (std::uint32_t i = 0; i < ALIGN_FACTOR_FOR_DIR_NUMS; ++i) {
        init_vec[i] = 1u;
    }
    if (!stream.is_open()) {
        std::cout << "Can't open " << filename << "." <<std::endl;
    } else {
        std::uint32_t dim;
        std::string first_line; // not used in further computations (contains names of the columns in data file)
        std::getline(stream, first_line);
        std::uint32_t a;
        for (std::uint32_t i = 1; i < dimen; ++i) {
            // data is stored starting from 2nd dimension
            stream >> dim;
            stream >> poly_degree[i];
            stream >> polynomials[i];
            // modify the coding of polynomials to the format expected by the library
            polynomials[i] <<= 1;
            polynomials[i] |= (1 | (1 << poly_degree[i]));
            std::uint32_t current_poly_deg = poly_degree[i];
            for (std::uint32_t j = 0; j < current_poly_deg; ++j) {
                stream >> init_vec[i * ALIGN_FACTOR_FOR_DIR_NUMS + j];
            }
            for (std::uint32_t j  = current_poly_deg; j < ALIGN_FACTOR_FOR_DIR_NUMS; ++j) {
                std::uint32_t current_poly_i = polynomials[i];
                std::uint32_t uMNew = init_vec[i * ALIGN_FACTOR_FOR_DIR_NUMS + j - current_poly_deg];
                for (std::uint32_t d = current_poly_deg; d > 0 ; --d) {
                    if (current_poly_i & 0x1) {
                        uMNew ^= init_vec[i * ALIGN_FACTOR_FOR_DIR_NUMS + j - d] << d;
                    }
                    current_poly_i >>= 1;
                }
                init_vec[i * ALIGN_FACTOR_FOR_DIR_NUMS + j] = uMNew;
            }
        }
        stream.close();
        for (std::uint32_t i = 0; i < ALIGN_FACTOR_FOR_DIR_NUMS; ++i) {
            for (std::uint32_t j = 0; j < dimen; ++j) {
                init_vec[j * ALIGN_FACTOR_FOR_DIR_NUMS + i] <<= ALIGN_FACTOR_FOR_DIR_NUMS - i - 1;
            }
        }
    }
}

// Routine to calculate integral of the function
// f(X) = Prod((|4*Xj - 2| + Cj)/(1+Cj)), j = 1,...,dimen
// over unit hypercube [0,1]^dimen. Integration result should be equal to 1.
// Here Cj = j^(1/3).
// Model function is taken from [1]
template<typename RealType>
static bool integrate(oneapi::mkl::rng::sobol& engine, std::uint32_t dimen, std::uint32_t m, std::uint32_t n_blocks) {

    std::cout << "Calculate integral for the function:\nf(X) = Prod((|4*Xj - 2| + Cj)/(1+Cj)),  j = 1,...," <<
    dimen << "\n" << "  Cj = j^(1/3)\nover unit hypercube [0,1]^" << dimen << "\n\n";

    RealType err = 0.0;
    RealType inv_n = 1.0 / (static_cast<RealType>(m) * static_cast<RealType>(n_blocks));
    RealType integration_result = 0.0;
    RealType f2 = 0.0;

    oneapi::mkl::rng::uniform<RealType> distribution;
    std::vector<RealType> res_vec(dimen * m);

    for (size_t i_block = 0; i_block < n_blocks; ++i_block) {
        {
        sycl::buffer<RealType, 1> r_buffer(res_vec.data(), sycl::range<1>(res_vec.size()));
        oneapi::mkl::rng::generate(distribution, engine, (dimen * m), r_buffer);
        } // buffer life-time ends
        for (size_t i = 0; i < m; ++i) {
            RealType prod = 1.0;
            for (size_t j = 0; j < dimen; ++j) {
                RealType cubic_root = std::cbrt(static_cast<RealType>(j));
                prod *= (fabs(4.0 * res_vec[i * dimen + j] - 2.0) + cubic_root);
                prod /= (1.0 + cubic_root);
            }
            integration_result += prod;
            f2 += prod * prod;
        }
    }
    integration_result *= inv_n;
    f2 *= inv_n;
    err = sqrt((f2 - integration_result * integration_result) * inv_n);
    std::cout << "Computed result: " << integration_result << std::endl;
    std::cout << "Expected result: " << 1.0 << std::endl;
    std::cout << "Calculation error: " << err << std::endl;
    if (((1.0 - 3.0 * err) <= integration_result) && (integration_result <= (1.0 + 3.0 * err))) {
        std::cout << "\nNumerical integration results agree with theory\n";
        return true;
    }
    std::cout << "\nError: Numerical integration results do not agree with theory\n";

    return false;
}

// function to print Sobol's output from buffer
template<typename Type>
void print_sobol(sycl::buffer<Type, 1>& r, std::uint32_t dim, std::uint32_t n_print) {
    auto r_accessor = r.template get_access<sycl::access::mode::read>();
    std::cout << "First "<< n_print << " numbers of " << M << " for dimension " << dim << ": " << std::endl;
    for(int i = 0 ; i < n_print; i++) {
        std::cout << r_accessor[i * DIM + dim] << " ";
    }
    std::cout << std::endl;
}

template <typename RealType>
bool run_sobol_example(const sycl::device &dev) {
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

    sycl::queue queue(dev, exception_handler);

    // read vector with user defined direction numbers
    std::vector<std::uint32_t> init_vec(DIM * ALIGN_FACTOR_FOR_DIR_NUMS, 0);
    read_sobol_dir_nums("./soboluserdirnums-joe-kuo.dat", DIM, init_vec);

    // init engine with user defined direction numbers
    oneapi::mkl::rng::sobol engine(queue, init_vec);

    oneapi::mkl::rng::uniform<RealType> distribution;

    // prepare array for random numbers
    std::vector<RealType, mkl_allocator<RealType, 64>> r(N);

    {
        sycl::buffer<RealType, 1> r_buffer(r.data(), sycl::range<1>(r.size()));
        try {
            // perform generation
            oneapi::mkl::rng::generate(distribution, engine, N, r_buffer);
        }
        catch(sycl::exception const& e) {
            std::cout << "\t\tSYCL exception during generation\n"
                      << e.what() << std::endl << "OpenCL status: " << e.get_cl_code() << std::endl;
            return false;
        }
        queue.wait_and_throw();
        // print output
        std::cout << "\n\t\tOutput of generator for dimension" << DIM_TO_PRINT << ":" << std::endl;
        print_sobol(r_buffer, DIM_TO_PRINT, N_PRINT);
    } //buffer life-time ends

    // validation
    return integrate<RealType>(engine, DIM, M, NBLOCKS);
}

void print_example_banner() {

    std::cout << "" << std::endl;
    std::cout << "########################################################################" << std::endl;
    std::cout << "# Generate random numbers with Sobol random number generator and user defined direction numbers:" << std::endl;
    std::cout << "# " << std::endl;
    std::cout << "# Using apis:" << std::endl;
    std::cout << "#   oneapi::mkl::rng::sobol" << std::endl;
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

    for(auto it = list_of_devices.begin(); it != list_of_devices.end(); ++it) {

        sycl::device my_dev;
        bool my_dev_is_found = false;
        get_sycl_device(my_dev, my_dev_is_found, *it);

        if(my_dev_is_found) {
            std::cout << "Running tests on " << sycl_device_names[*it] << ".\n";
            std::cout << "\tRunning with single precision real data type:" << std::endl;
            if(!run_sobol_example<float>(my_dev)) {
                std::cout << "FAILED" << std::endl;
                return 1;
            }
            if(isDoubleSupported(my_dev)) {
                std::cout << "\tRunning with double precision real data type:" << std::endl;
                if(!run_sobol_example<double>(my_dev)) {
                    std::cout << "FAILED" << std::endl;
                    return 1;
                }
            }
            else {
                std::cout << "Double precision is not supported for this device" << std::endl;
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
