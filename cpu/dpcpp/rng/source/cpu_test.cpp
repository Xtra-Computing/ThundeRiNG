#include <iostream>
#include <vector>

#include "CL/sycl.hpp"
#include "oneapi/mkl/rng.hpp"
#define SEED 777

int main() {
    sycl::queue queue;

    const size_t n = 10000;
    std::vector<double> r(n);

    // create basic random number generator object
    oneapi::mkl::rng::philox4x32x10 engine(queue, SEED);
    // create distribution object
    oneapi::mkl::rng::gaussian<double, oneapi::mkl::rng::gaussian_method::icdf> distr(5.0, 2.0);

    {
        // buffer for random numbers
        sycl::buffer<double, 1> r_buf(r.data(), r.size());
        // perform generation
        oneapi::mkl::rng::generate(distr, engine, n, r_buf);

    }

    double s = 0.0;
    for(int i = 0; i < n; i++) {
       s += r[i];
    }
    s /= n;

    std::cout << "Average = " << s << std::endl;

    return 0;
}