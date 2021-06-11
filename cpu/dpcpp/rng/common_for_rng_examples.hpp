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
*       File contains checkers for statistics of various rng distributions
*
*******************************************************************************/

#ifndef __COMMON_FOR_RNG_EXAMPLES_HPP__
#define __COMMON_FOR_RNG_EXAMPLES_HPP__

// stl includes
#include <iostream>
#include <vector>
#include <math.h>

// mkl/sycl includes
#include <CL/sycl.hpp>
#include "oneapi/mkl.hpp"

// local includes
#include "common_for_examples.hpp"

#define _PI 3.141592653589793238462643383279502884197

// function to print rng output from buffer
template<typename Type>
void print_output(sycl::buffer<Type, 1>& r, int n_print) {
    auto r_accessor = r.template get_access<sycl::access::mode::read>();
    std::cout << "First "<< n_print << " numbers of " << r.get_count() << ": " << std::endl;
    for(int i = 0 ; i < n_print; i++) {
        std::cout << r_accessor[i] << " ";
    }
    std::cout << std::endl;
}

// function to compare theoretical moments and sample moments
template<typename Type>
bool compare_moments(std::vector<Type, mkl_allocator<Type, 64>>& r, double tM, double tD, double tQ) {
    double tD2;
    double sM, sD;
    double sum, sum2;
    double n, s;
    double DeltaM, DeltaD;

    // sample moments
    sum = 0.0;
    sum2 = 0.0;
    for(int i = 0; i < r.size(); i++) {
        sum += (double)r[i];
        sum2 += (double)r[i] * (double)r[i];
    }
    sM = sum / ((double)r.size());
    sD = sum2 / (double)r.size() - (sM * sM);

    // comparison of theoretical and sample moments
    n = (double)r.size();
    tD2 = tD * tD;
    s = ((tQ-tD2) / n) - ( 2 * (tQ - 2 * tD2) / (n * n))+((tQ - 3 * tD2) /
                                                            (n * n * n));

    DeltaM = (tM - sM) / sqrt(tD / n);
    DeltaD = (tD - sD) / sqrt(s);
    if(fabs(DeltaM) > 3.0 || fabs(DeltaD) > 3.0) {
        std::cout << "Error: sample moments (mean=" << sM << ", variance=" << sD
            << ") disagree with theory (mean=" << tM << ", variance=" << tD <<
            ")" << std:: endl;
        return false;
    }
    std::cout << "Success: sample moments (mean=" << sM << ", variance=" << sD
        << ") agree with theory (mean=" << tM << ", variance=" << tD <<
        ")" << std:: endl;
    return true;
}

// function to compare theoretical moments and sample moments for multidimensional gaussian distribution
template<typename Type>
bool compare_moments_gaussian_mv(std::vector<Type, mkl_allocator<Type, 64>>& r, std::vector<Type> mean_theory,
    std::vector<Type> variance_theory, std::uint32_t dimen) {
    // vectors for computed moments
    std::vector<Type> mean_generated(dimen, 0.0);
    std::vector<Type> var_generated(dimen, 0.0); // elements are stored in layout::packed format

    std::vector<Type> tmp(dimen * 2, 0.0);
    for (size_t i = 0; i < r.size() / dimen; ++i) {
        for (size_t j = 0; j < dimen; ++j) {
            tmp[j] += r[dimen * i + j];
            tmp[dimen + j] += r[dimen * i + j] * r[dimen * i + j];
        }
    }
    // compute moments
    for (size_t j = 0; j < dimen; ++j) {
        mean_generated[j] = tmp[j] / r.size() * dimen;
        var_generated[j] = tmp[dimen + j] / r.size() * dimen - mean_generated[j] * mean_generated[j];
    }
    std::vector<Type> delta_mean(dimen, 0.0);
    std::vector<Type> delta_var(dimen, 0.0);
    for (size_t j = 0; j < dimen; ++j) {
        delta_mean[j] = fabs(mean_generated[j] - mean_theory[j]) / sqrt(((double) dimen / r.size()) * variance_theory[j]);
        delta_var[j] = fabs(var_generated[j] - variance_theory[j]) / (variance_theory[j] *
            sqrt(2 * ((double) dimen / r.size())) * (1 - (double) dimen / r.size()));
    }
    for (size_t j = 0; j < dimen; ++j) {
        if((delta_mean[j] > 3.0) || (delta_var[j] > 3.0)) {
            for (size_t j = 0; j < dimen; ++j) {
                std::cout << "Mean for dimenstion " << j << ": " << mean_generated[j] << ". Theoretical value: " <<
                    mean_theory[j] << std::endl;
                std::cout << "Variance for dimenstion " << j << ": " << var_generated[j] << ". Theoretical value: " <<
                    variance_theory[j] << std::endl;
            }
            return false;
        }
    }
    return true;
}

// structure is used to calculate theoretical moments of particular distribution
// and compare them with sample moments
template <typename Type, typename Distribution>
struct statistics {};

template<typename Type>
struct statistics<Type, oneapi::mkl::rng::uniform<Type>> {
    bool check(std::vector<Type, mkl_allocator<Type, 64>>& r, const oneapi::mkl::rng::uniform<Type>& distr) {
        double tM, tD, tQ;
        Type a = distr.a();
        Type b = distr.b();

        // theoretical moments of uniform real type distribution
        tM = (b + a) / 2.0;
        tD = ((b - a) * (b - a)) / 12.0;
        tQ = ((b - a)*(b - a)*(b - a)*(b - a)) / 80.0;

        return compare_moments(r, tM, tD, tQ);
    }
};

template<>
struct statistics<std::int32_t, oneapi::mkl::rng::uniform<std::int32_t>> {
    bool check(std::vector<std::int32_t, mkl_allocator<std::int32_t, 64>>& r, const oneapi::mkl::rng::uniform<std::int32_t>& distr) {
        double tM, tD, tQ;
        std::int32_t a = distr.a();
        std::int32_t b = distr.b();

        // theoretical moments of uniform int distribution
        tM = (a + b - 1.0) / 2.0;
        tD = ((b - a)*(b - a) - 1.0) / 12.0;
        tQ = (((b - a) * (b - a)) * ((1.0 / 80.0)*(b - a)*(b - a) - (1.0 / 24.0))) + (7.0 / 240.0);

        return compare_moments(r, tM, tD, tQ);
    }
};

template<typename Type>
struct statistics<Type, oneapi::mkl::rng::gaussian<Type>> {
    bool check(std::vector<Type, mkl_allocator<Type, 64>>& r, const oneapi::mkl::rng::gaussian<Type>& distr) {
        double tM, tD, tQ;
        Type a = distr.mean();
        Type sigma = distr.stddev();

        // theoretical moments of gaussian distribution
        tM = a;
        tD = sigma * sigma;
        tQ = 720.0 * sigma * sigma * sigma * sigma;

        return compare_moments(r, tM, tD, tQ);
    }
};

template<typename Type>
struct statistics<Type, oneapi::mkl::rng::gaussian_mv<Type>> {
    bool check(std::vector<Type, mkl_allocator<Type, 64>>& r, const oneapi::mkl::rng::gaussian_mv<Type>& distr) {
        // quantity of dimensions
        std::uint32_t dimen = distr.dimen();
        // theoretical moments
        std::vector<Type> mean_theory = distr.mean();
        std::vector<Type> lower_triangular_matrix = distr.matrix();
        std::vector<Type> variance_theory(dimen, 0.0);
        std::vector<Type> lower_triangular_matrix_full(dimen * dimen, 0.0);
        std::uint32_t n = 0;
        for (size_t j = 0; j < dimen; ++j) {
            for (size_t i = j; i < dimen; ++i) {
                lower_triangular_matrix_full[i * dimen + j] = lower_triangular_matrix[n];
                ++n;
            }
        }
        // transpose matrix
        std::vector<Type> lower_triangular_matrix_full_transposed(dimen * dimen, 0.0);
        for (size_t i = 0; i < dimen; ++i) {
            for (size_t j = 0; j < dimen; ++j) {
                lower_triangular_matrix_full_transposed[i * dimen + j] = lower_triangular_matrix_full[j * dimen + i];
            }
        }
        std::vector<Type> cov_theory_full(dimen * dimen, 0.0);
        // multiply matrices
        for (size_t i = 0; i < dimen; ++i) {
            for (size_t j = 0; j < dimen; ++j) {
                for (size_t k = 0; k < dimen; ++k) {
                    cov_theory_full[i * dimen + j] += lower_triangular_matrix_full[i * dimen + k] *
                        lower_triangular_matrix_full_transposed[k * dimen + j];
                }
            }
        }
        // take diagonal elements
        for (size_t i = 0; i < dimen; ++i) {
            variance_theory[i] = cov_theory_full[i * dimen + i];
        }

        return compare_moments_gaussian_mv(r, mean_theory, variance_theory, dimen);
    }
};

template<typename Type>
struct statistics<Type, oneapi::mkl::rng::gaussian_mv<Type, oneapi::mkl::rng::layout::full>> {
    bool check(std::vector<Type, mkl_allocator<Type, 64>>& r, const oneapi::mkl::rng::gaussian_mv<Type, oneapi::mkl::rng::layout::full>& distr) {
        // quantity of dimensions
        std::uint32_t dimen = distr.dimen();
        // theoretical moments
        std::vector<Type> mean_theory = distr.mean();
        std::vector<Type> lower_triangular_matrix = distr.matrix();
        std::vector<Type> variance_theory(dimen, 0.0);
        std::vector<Type> lower_triangular_matrix_full(dimen * dimen, 0.0);
        // transpose matrix
        std::vector<Type> lower_triangular_matrix_full_transposed(dimen * dimen, 0.0);
        for (size_t i = 0; i < dimen; ++i) {
            for (size_t j = 0; j < dimen; ++j) {
                lower_triangular_matrix_full_transposed[i * dimen + j] = lower_triangular_matrix[j * dimen + i];
            }
        }
        std::vector<Type> cov_theory_full(dimen * dimen, 0.0);
        // multiply matrices
        for (size_t i = 0; i < dimen; ++i) {
            for (size_t j = 0; j < dimen; ++j) {
                for (size_t k = 0; k < dimen; ++k) {
                    cov_theory_full[i * dimen + j] += lower_triangular_matrix[i * dimen + k] *
                        lower_triangular_matrix_full_transposed[k * dimen + j];
                }
            }
        }
        // take diagonal elements
        for (size_t i = 0; i < dimen; ++i) {
            variance_theory[i] = cov_theory_full[i * dimen + i];
        }

        return compare_moments_gaussian_mv(r, mean_theory, variance_theory, dimen);
    }
};

template<typename Type>
struct statistics<Type, oneapi::mkl::rng::gaussian_mv<Type, oneapi::mkl::rng::layout::diagonal>> {
    bool check(std::vector<Type, mkl_allocator<Type, 64>>& r, const oneapi::mkl::rng::gaussian_mv<Type, oneapi::mkl::rng::layout::diagonal>& distr) {
        // quantity of dimensions
        std::uint32_t dimen = distr.dimen();
        // theoretical moments
        std::vector<Type> mean_theory = distr.mean();
        std::vector<Type> lower_triangular_matrix = distr.matrix();
        std::vector<Type> variance_theory(dimen, 0.0);

        for (size_t i = 0; i < dimen; ++i) {
            // if layout is diagonal lower_triangular_matrix becomes diagonal
            variance_theory[i] = lower_triangular_matrix[i] * lower_triangular_matrix[i];
        }

        return compare_moments_gaussian_mv(r, mean_theory, variance_theory, dimen);
    }
};

template<typename Type>
struct statistics<Type, oneapi::mkl::rng::lognormal<Type>> {
    bool check(std::vector<Type, mkl_allocator<Type, 64>>& r, const oneapi::mkl::rng::lognormal<Type>& distr) {
        double tM, tD, tQ;
        Type a = distr.m();
        Type b = distr.displ();
        Type sigma = distr.s();
        Type beta = distr.scale();

        // theoretical moments of lognormal distribution
        tM = b + beta * exp(a + sigma * sigma * 0.5);
        tD = beta * beta * exp(2.0 * a + sigma * sigma) * (exp(sigma * sigma) - 1.0);
        tQ = beta * beta * beta * beta * exp(4.0 * a + 2.0 * sigma * sigma) *
            (exp(6.0 * sigma * sigma) - 4.0 * exp(3.0 * sigma * sigma) + 6.0 * exp(sigma * sigma) - 3.0);

        return compare_moments(r, tM, tD, tQ);
    }
};

template<typename Type>
struct statistics<Type, oneapi::mkl::rng::beta<Type>> {
    bool check(std::vector<Type, mkl_allocator<Type, 64>>& r, const oneapi::mkl::rng::beta<Type>& distr) {
        double tM, tD, tQ;
        double b, c, d, e, e2, b2, sum_pq;
        Type p = distr.p();
        Type q = distr.q();
        Type a = distr.a();
        Type beta = distr.b();

        // theoretical moments of beta distribution
        b2 = beta * beta;
        sum_pq = p + q;
        b = (p + 1.0) / (sum_pq + 1.0);
        c = (p + 2.0) / (sum_pq + 2.0);
        d = (p + 3.0) / (sum_pq + 3.0);
        e =  p / sum_pq;
        e2 = e * e;

        tM = a + e * beta;
        tD = b2 * p * q / (sum_pq * sum_pq * (sum_pq + 1.0));
        tQ = b2 * b2 * (e * b * c * d - 4.0 * e2 * b * c + 6.0 * e2 * e * b - 3.0 * e2 * e2);

        return compare_moments(r, tM, tD, tQ);
    }
};

template<typename Type>
struct statistics<Type, oneapi::mkl::rng::chi_square<Type>> {
    bool check(std::vector<Type, mkl_allocator<Type, 64>>& r, const oneapi::mkl::rng::chi_square<Type>& distr) {
        double tM, tD, tQ;
        std::int32_t n = distr.n();

        // theoretical moments of chi_square distribution
        tM = n;
        tD = 2 * n;
        tQ = 12 * n * n + 48 * n;

        return compare_moments(r, tM, tD, tQ);
    }
};

template<typename Type>
struct statistics<Type, oneapi::mkl::rng::exponential<Type>> {
    bool check(std::vector<Type, mkl_allocator<Type, 64>>& r, const oneapi::mkl::rng::exponential<Type>& distr) {
        double tM, tD, tQ;
        Type a = distr.a();
        Type beta = distr.beta();

        // theoretical moments of exponential distribution
        tM = a + beta;
        tD = beta * beta;
        tQ = 9.0 * beta * beta * beta * beta;

        return compare_moments(r, tM, tD, tQ);
    }
};

template<typename Type>
struct statistics<Type, oneapi::mkl::rng::gamma<Type>> {
    bool check(std::vector<Type, mkl_allocator<Type, 64>>& r, const oneapi::mkl::rng::gamma<Type>& distr) {
        double tM, tD, tQ;
        Type a = distr.a();
        Type alpha = distr.alpha();
        Type beta = distr.beta();

        // theoretical moments of gamma distribution
        tM = a + beta * alpha;
        tD = beta * beta * alpha;
        tQ = beta * beta * beta * beta * 3 * alpha * (alpha + 2);

        return compare_moments(r, tM, tD, tQ);
    }
};

template<typename Type>
struct statistics<Type, oneapi::mkl::rng::gumbel<Type>> {
    bool check(std::vector<Type, mkl_allocator<Type, 64>>& r, const oneapi::mkl::rng::gumbel<Type>& distr) {
        double tM, tD, tQ;
        Type a = distr.a();
        Type beta = distr.b();

        // theoretical moments of gumbel distribution
        tM = a + beta * (-0.5772156649);
        tD = beta * beta * _PI * _PI / 6.0;
        tQ = 14.61136365 * beta * beta * beta * beta;

        return compare_moments(r, tM, tD, tQ);
    }
};

template<typename Type>
struct statistics<Type, oneapi::mkl::rng::laplace<Type>> {
    bool check(std::vector<Type, mkl_allocator<Type, 64>>& r, const oneapi::mkl::rng::laplace<Type>& distr) {
        double tM, tD, tQ;
        Type a = distr.a();
        Type beta = distr.b();

        // theoretical moments of laplace distribution
        tM = a;
        tD = 2 * beta * beta;
        tQ = 24.0 * beta * beta * beta * beta;

        return compare_moments(r, tM, tD, tQ);
    }
};

template<typename Type>
struct statistics<Type, oneapi::mkl::rng::rayleigh<Type>> {
    bool check(std::vector<Type, mkl_allocator<Type, 64>>& r, const oneapi::mkl::rng::rayleigh<Type>& distr) {
        double tM, tD, tQ;
        Type a = distr.a();
        Type beta = distr.b();

        // theoretical moments of rayleigh distribution
        tM = a + beta * sqrt(_PI) * 0.5;
        tD = (1.0 - _PI * 0.25) * beta * beta;
        tQ = (2.0 - 0.1875 * _PI * _PI) * beta * beta * beta * beta;

        return compare_moments(r, tM, tD, tQ);
    }
};

template<typename Type>
struct statistics<Type, oneapi::mkl::rng::weibull<Type>> {
    bool check(std::vector<Type, mkl_allocator<Type, 64>>& r, const oneapi::mkl::rng::weibull<Type>& distr) {
        double tM, tD, tQ;
        Type a = distr.a();
        Type beta = distr.beta();

        // theoretical moments of weibull distribution
        tM = a + beta;
        tD = beta * beta;
        tQ = beta * beta * beta * beta * 9;

        return compare_moments(r, tM, tD, tQ);
    }
};

template<typename Type>
struct statistics<Type, oneapi::mkl::rng::bernoulli<Type>> {
    bool check(std::vector<Type, mkl_allocator<Type, 64>>& r, const oneapi::mkl::rng::bernoulli<Type>& distr) {
        double tM, tD, tQ;
        double p = distr.p();

        // theoretical moments of bernoulli distribution
        tM = p;
        tD = p * (1.0 - p);
        tQ = p * (1.0 - 4.0 * p + 6.0 * p * p - 3.0 * p * p * p);

        return compare_moments(r, tM, tD, tQ);
    }
};

template<typename Type>
struct statistics<Type, oneapi::mkl::rng::binomial<Type>> {
    bool check(std::vector<Type, mkl_allocator<Type, 64>>& r, const oneapi::mkl::rng::binomial<Type>& distr) {
        double tM, tD, tQ;
        double p = distr.p();
        std::int32_t ntrial = distr.ntrial();

        // theoretical moments of binomial distribution
        tM = ntrial * p;
        tD = ntrial * p * (1.0 - p);
        tQ = ntrial * (1.0 - p) * (4.0 * ntrial * p - 4.0 * ntrial * p * p + 4.0 * p - 3.0);

        return compare_moments(r, tM, tD, tQ);
    }
};

template<typename Type>
struct statistics<Type, oneapi::mkl::rng::geometric<Type>> {
    bool check(std::vector<Type, mkl_allocator<Type, 64>>& r, const oneapi::mkl::rng::geometric<Type>& distr) {
        double tM, tD, tQ;
        double p = distr.p();

        // theoretical moments of geometric distribution
        tM = (1.0 - p) / p;
        tD = (1.0 - p) / (p * p);
        tQ = (1.0 - p) * (p * p - 9.0 * p + 9.0) / (p * p * p * p);

        return compare_moments(r, tM, tD, tQ);
    }
};

template<typename Type>
struct statistics<Type, oneapi::mkl::rng::hypergeometric<Type>> {
    bool check(std::vector<Type, mkl_allocator<Type, 64>>& r, const oneapi::mkl::rng::hypergeometric<Type>& distr) {
        double tM, tD, tQ;
        double K, L2, L3, L4, L5, L6, KL, KL4, S2, S3, S4, M2, M3, M4;
        std::int32_t l = distr.l();
        std::int32_t s = distr.s();
        std::int32_t m = distr.m();

        // theoretical moments of hypergeometric distribution
        K = (l - 1) * (l - 2) * (l - 3);
        L2 = l * l;
        L3 = L2 * l;
        L4 = L2 * L2;
        L5 = L3 * L2;
        L6 = L3 * L3;
        KL = K * l;
        KL4 = K * L4;
        S2 = s * s;
        S3 = S2 * s;
        S4 = S2 * S2;
        M2 = m * m;
        M3 = M2 * m;
        M4 = M2 * M2;

        tM = (double)m * (double)s / (double)l;
        tD = (double)(m * s *(l - m) * (l - s)) / (double)(l * l * (l-1));
        tQ = ( (3*l+18)    *S4/KL4 - (6*L2+36*l)  *S3/KL4 + (3*L3+24*L2)   *S2/KL4 - 6        *s/KL  ) * M4 +
             ( (-6*L2-36*l)*S4/KL4 + (12*L3+72*L2)*S3/KL4 - (6*L4+38*L3)   *S2/KL4 + 12       *s/K   ) * M3 +
             ( (3*L3+24*L2)*S4/KL4 - (6*L4+48*L3) *S3/KL4 + (31*L4+3*L5+L3)*S2/KL4 - (L4+7*L5)*s/KL4 ) * M2 +
             ( -6          *S4/KL  + 12           *S3/K   - (4*L4+7*L5)    *S2/KL4 + (L6+L5)  *s/KL4 ) * m;

        return compare_moments(r, tM, tD, tQ);
    }
};

template<typename Type>
struct statistics<Type, oneapi::mkl::rng::multinomial<Type>> {
    bool check(std::vector<Type, mkl_allocator<Type, 64>>& r, const oneapi::mkl::rng::multinomial<Type>& distr) {
        double tM,tD,tQ,tD2;
        double sM,sD;
        double sum, sum2;
        double s;
        double DeltaM,DeltaD;
        bool res = true;
        std::int32_t ntrial = distr.ntrial();
        std::int32_t k = (std::int32_t)(distr.p().size());
        std::vector<double> p = distr.p();
        std::int32_t n = r.size() / k;
        for(int i = 0; i < k; i++) {

            // theoretical moments of multinomial distribution
            tM = ntrial * p[i];
            tD = ntrial * p[i] * (1.0 - p[i]);
            tQ = ntrial * (1.0 - p[i]) * (4.0 * ntrial * p[i] - 4.0 * ntrial * p[i] * p[i] + 4.0 * p[i] - 3.0);

            sum=0.0;
            sum2=0.0;
            for(int j=0; j < n; j++) {
                sum += (double)r[j * k + i];
                sum2 += (double)r[j * k + i] * (double)r[j * k + i];
            }
            sM = sum / ((double)n);
            sD = sum2 / (double)n - (sM * sM);

            tD2 = tD * tD;
            s = ((tQ - tD2) / (double)n) - (2 * (tQ - 2 * tD2) / (double)(n * n))
                + ((tQ - 3 * tD2) / (double)(n * n * n));

            DeltaM = (tM - sM) / sqrt(tD / (double)n);
            DeltaD = (tD - sD) / sqrt(s);
            if(fabs(DeltaM) > 3.0 || fabs(DeltaD) > 3.0) {
                res = false;
            }
        }
        return res;
    }
};

template<typename Type>
struct statistics<Type, oneapi::mkl::rng::negative_binomial<Type>> {
    bool check(std::vector<Type, mkl_allocator<Type, 64>>& r, const oneapi::mkl::rng::negative_binomial<Type>& distr) {
        double tM, tD, tQ;
        double p = distr.p();
        double a = distr.a();

        // theoretical moments of negative_binomial distribution
        tM = a * (1 - p) / p;
        tD = a * (1 - p) / (p * p);
        tQ = (a * (1 - p) * (p * p - 3 * a * p - 6 * p + 3 * a + 6)) / (p * p * p * p);

        return compare_moments(r, tM, tD, tQ);
    }
};

template<typename Type>
struct statistics<Type, oneapi::mkl::rng::poisson<Type>> {
    bool check(std::vector<Type, mkl_allocator<Type, 64>>& r, const oneapi::mkl::rng::poisson<Type>& distr) {
        double tM, tD, tQ;
        double lambda = distr.lambda();

        // theoretical moments of poisson distribution
        tM = lambda;
        tD = lambda;
        tQ = 4 * lambda * lambda + lambda;

        return compare_moments(r, tM, tD, tQ);
    }
};

template<typename Type>
struct statistics<Type, oneapi::mkl::rng::poisson_v<Type>> {
    bool check(std::vector<Type, mkl_allocator<Type, 64>>& r, const oneapi::mkl::rng::poisson_v<Type>& distr) {
        double tM, tD, tQ;
        std::vector<double> lambda = distr.lambda();

        // theoretical moments of poisson_v distribution
        tM = lambda[0];
        tD = lambda[0];
        tQ = 4 * lambda[0] * lambda[0] + lambda[0];

        return compare_moments(r, tM, tD, tQ);
    }
};

#endif // __COMMON_FOR_RNG_EXAMPLES_HPP__
