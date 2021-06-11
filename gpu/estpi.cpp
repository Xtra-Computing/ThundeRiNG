#include <thrust/iterator/counting_iterator.h>
#include <thrust/functional.h>
#include <thrust/transform_reduce.h>
#include <curand_kernel.h>

#include <iostream>
#include <iomanip>

#include <cstdio>
#include <cstring>
#include <iostream>
#include <vector>
#include <chrono>


using namespace std;

struct estimate_pi :
    public thrust::unary_function<unsigned int, float>
{
    __device__
    float operator()(unsigned int thread_id)
    {
        float sum = 0;
        unsigned int N = 10000000;

        unsigned int seed = thread_id;

        curandState s;


        //curand_init(seed, 0, 0, &s);


        for (unsigned int i = 0; i < N; ++i)
        {
            float x = curand_uniform(&s);
            float y = curand_uniform(&s);
            float dist = sqrtf(x * x + y * y);
            if (dist <= 1.0f)
                sum += 1.0f;
        }

        sum *= 4.0f;

        return sum / N;
    }
};
// 37.6877s

int main(void)
{

    int M = 30000;
    double kernel_time_in_sec = 0, result = 0;

    std::chrono::duration<double> kernel_time(0);

    auto kernel_start = std::chrono::high_resolution_clock::now();

    float estimate = thrust::transform_reduce(
                         thrust::counting_iterator<int>(0),
                         thrust::counting_iterator<int>(M),
                         estimate_pi(),
                         0.0f,
                         thrust::plus<float>());
    estimate /= M;
    auto kernel_end = std::chrono::high_resolution_clock::now();

    kernel_time = std::chrono::duration<double>(kernel_end - kernel_start);

    kernel_time_in_sec = kernel_time.count();
    std::cout << "Time: " << kernel_time_in_sec << "s" << std::endl;

    std::cout << std::setprecision(20);
    std::cout << "pi is approximately ";
    std::cout << estimate << std::endl;
    return 0;
}