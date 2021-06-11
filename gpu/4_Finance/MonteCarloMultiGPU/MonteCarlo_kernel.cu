/**
 * Copyright 1993-2015 NVIDIA Corporation.  All rights reserved.
 *
 * Please refer to the NVIDIA end user license agreement (EULA) associated
 * with this source code for terms and conditions that govern your use of
 * this software. Any use, reproduction, disclosure, or distribution of
 * this software and related documentation outside the terms of the EULA
 * is strictly prohibited.
 *
 */

////////////////////////////////////////////////////////////////////////////////
// Global types
////////////////////////////////////////////////////////////////////////////////
#include <stdlib.h>
#include <stdio.h>
#include <cooperative_groups.h>

namespace cg = cooperative_groups;
#include <helper_cuda.h>
#include <curand_kernel.h>
#include "MonteCarlo_common.h"

////////////////////////////////////////////////////////////////////////////////
// Helper reduction template
// Please see the "reduction" CUDA Sample for more information
////////////////////////////////////////////////////////////////////////////////
#include "MonteCarlo_reduction.cuh"

////////////////////////////////////////////////////////////////////////////////
// Internal GPU-side data structures
////////////////////////////////////////////////////////////////////////////////
#define MAX_OPTIONS (1024*1024)

#define THREAD_N         (__TEST_S__)



#ifndef __TEST_MAX_CPU__
#error "No define of __TEST_MAX_CPU__"
#endif

#define MAX_CPU  (__TEST_MAX_CPU__)
#define STATE_SHARE (__TEST_STATE_SHARE__)
#define THUNDERING_TEST (__TEST_THUNDERING_TEST__)

//Preprocessed input option data
typedef struct
{
    real S;
    real X;
    real MuByT;
    real VBySqrtT;
} __TOptionData;

////////////////////////////////////////////////////////////////////////////////
// Overloaded shortcut payoff functions for different precision modes
////////////////////////////////////////////////////////////////////////////////
__device__ inline float endCallValue(float S, float X, float r, float MuByT, float VBySqrtT)
{
    float callValue = S * __expf(MuByT + VBySqrtT * r) - X;
    return (callValue > 0.0F) ? callValue : 0.0F;
}

__device__ inline double endCallValue(double S, double X, double r, double MuByT, double VBySqrtT)
{
    double callValue = S * exp(MuByT + VBySqrtT * r) - X;
    return (callValue > 0.0) ? callValue : 0.0;
}


typedef uint32_t                rng32_t;
typedef uint64_t                rng_state_t;

#define RNG_INC             (0x000000000000006dULL)
#define RNG_A               (0x5851f42d4c957f2dULL)


__device__ inline uint32_t next0(uint64_t *s_) {

    uint64_t s1 = s_[ 0 ];
    const uint64_t s0 = s_[ 1 ];
    s_[ 0 ] = s0;
    s1 ^= s1 << 23;
    return ( s_[ 1 ] = ( s1 ^ s0 ^ ( s1 >> 17 ) ^ ( s0 >> 26 ) ) );
    /*
    uint32_t *s = (uint32_t * )s_;
    uint32_t &x = s[0], &y = s[1], &z = s[2], &w = s[3];

    uint32_t t = x ^ (x << 11);

    x = y; y = z; z = w;
    w = w ^ (w >> 19) ^ (t ^ (t >> 8));
    return z;
     */
}

__device__ inline rng32_t pcg_output_xsh_rs_64_32(rng_state_t state)
{

    return (rng32_t)(((state >> 22u) ^ state) >> ((state >> 61u) + 22u));
}


__device__ inline rng32_t  output_function(rng_state_t oldstate)
{
    rng32_t xorshifted = ((oldstate >> 18u) ^ oldstate) >> 27u;
    rng32_t rot = oldstate >> 59u;
    return (xorshifted >> rot) | (xorshifted << ((-rot) & 31));
}


__device__ inline rng_state_t multi_stream(rng_state_t state,  const int  seq)
{
    return state + ((RNG_A * (seq << 1u)) | 1u);
}


////////////////////////////////////////////////////////////////////////////////
// This kernel computes the integral over all paths using a single thread block
// per option. It is fastest when the number of thread blocks times the work per
// block is high enough to keep the GPU busy.
////////////////////////////////////////////////////////////////////////////////
static __global__ void MonteCarloOneBlockPerOption(
    curandStatePhilox4_32_10_t * __restrict rngStates,
    const __TOptionData * __restrict d_OptionData,
    __TOptionValue * __restrict d_CallValue,
    int pathN,
    int optionN)
{
    // Handle to thread block group
    cg::thread_block cta = cg::this_thread_block();
    cg::thread_block_tile<32> tile32 = cg::tiled_partition<32>(cta);

    const int SUM_N = THREAD_N;
    __shared__ real s_SumCall[SUM_N];
    __shared__ real s_Sum2Call[SUM_N];

    __shared__ rng_state_t  state[SUM_N];


    // determine global thread id
    int tid = threadIdx.x + blockIdx.x * blockDim.x;

    // Copy random number state to local memory for efficiency
    curandStatePhilox4_32_10_t localState = rngStates[tid]; // common for thundering and cuda

    __shared__ rng_state_t share_state[2];
    // = {
    //    0x185706b82c2e03f8ULL,
    //    0x2b47fed88766bb05ULL
    //}
     __shared__ uint64_t  dstate[SUM_N][2];



    share_state[0] = 0x185706b82c2e03f8ULL;
    share_state[1] = 0x2b47fed88766bb05ULL;

    for (int optionIndex = blockIdx.x; optionIndex < optionN; optionIndex += gridDim.x)
    {
        unsigned int count = 0;
        unsigned int sum =  0;


        for (int iSum = threadIdx.x; iSum < SUM_N; iSum += blockDim.x)
        {
            state[iSum] = 0x185706b82c2e03f8ULL + tid;
            dstate[iSum][0] = ((uint64_t *)(& localState))[0];
            dstate[iSum][1] = ((uint64_t *)(& localState))[1];
#pragma unroll 8
            for (int i = iSum; i < pathN; i += SUM_N)
            {

#if THUNDERING_TEST
#if STATE_SHARE

                rng_state_t oldstate = share_state[count & 0x01];
                if (i == 0)
                {
                    share_state[(count + 1) & 0x01] = oldstate * RNG_A + ((tid << 1) | 0x01);
                }
                cg::sync(cta);
                rng_state_t updated_state = multi_stream(share_state[(count + 1) & 0x01], tid);
                count ++;
#else

                rng_state_t oldstate = state[iSum];
                state[iSum] = oldstate * RNG_A + ((tid << 1) | 0x01);
                rng_state_t updated_state = state[iSum];
#endif

                //rng_state_t updated_state = oldstate;

                uint32_t d = next0(dstate[iSum]);
                //uint32_t d = 1;
                unsigned int          r = output_function(updated_state)^ d ;
#else

                unsigned int          r = curand(&localState);
#endif
                sum += r;
            }

            s_SumCall[iSum]  = sum;
        }

        //Reduce shared memory accumulators
        //and write final result to global memory
        cg::sync(cta);
        sumReduce<real, SUM_N, THREAD_N>(s_SumCall, s_Sum2Call, cta, tile32, &d_CallValue[optionIndex]);
    }
}

static __global__ void rngSetupStates(
    curandStatePhilox4_32_10_t *rngState,
    int device_id)
{
    // determine global thread id
    int tid = threadIdx.x + blockIdx.x * blockDim.x;
    // Each threadblock gets different seed,
    // Threads within a threadblock get different sequence numbers
    curand_init(blockIdx.x + gridDim.x * device_id, threadIdx.x, 0, &rngState[tid]);
}



////////////////////////////////////////////////////////////////////////////////
// Host-side interface to GPU Monte Carlo
////////////////////////////////////////////////////////////////////////////////
extern "C" void initMonteCarloGPU(TOptionPlan *plan)
{
    checkCudaErrors(cudaMalloc(&plan->d_OptionData, sizeof(__TOptionData) * (plan->optionCount)));
    checkCudaErrors(cudaMalloc(&plan->d_CallValue, sizeof(__TOptionValue) * (plan->optionCount)));
    checkCudaErrors(cudaMallocHost(&plan->h_OptionData, sizeof(__TOptionData) * (plan->optionCount)));
    //Allocate internal device memory
    checkCudaErrors(cudaMallocHost(&plan->h_CallValue, sizeof(__TOptionValue) * (plan->optionCount)));
    //Allocate states for pseudo random number generators
    checkCudaErrors(cudaMalloc((void **) &plan->rngStates,
                               plan->gridSize * THREAD_N * sizeof(curandStatePhilox4_32_10_t)));
    checkCudaErrors(cudaMemset(plan->rngStates, 0, plan->gridSize * THREAD_N * sizeof(curandStatePhilox4_32_10_t)));

    // place each device pathN random numbers apart on the random number sequence
    rngSetupStates <<< plan->gridSize, THREAD_N >>> (plan->rngStates, plan->device);
    getLastCudaError("rngSetupStates kernel failed.\n");
}

//Compute statistics and deallocate internal device memory
extern "C" void closeMonteCarloGPU(TOptionPlan *plan)
{
    for (int i = 0; i < plan->optionCount; i++)
    {
        const double    RT = plan->optionData[i].R * plan->optionData[i].T;
        const double   sum = plan->h_CallValue[i].Expected;
        const double  sum2 = plan->h_CallValue[i].Confidence;
        const double pathN = plan->pathN;
        //Derive average from the total sum and discount by riskfree rate
        plan->callValue[i].Expected = (float)(exp(-RT) * sum / pathN);
        //Standard deviation
        double stdDev = sqrt((pathN * sum2 - sum * sum) / (pathN * (pathN - 1)));
        //Confidence width; in 95% of all cases theoretical value lies within these borders
        plan->callValue[i].Confidence = (float)(exp(-RT) * 1.96 * stdDev / sqrt(pathN));
    }

    checkCudaErrors(cudaFree(plan->rngStates));
    checkCudaErrors(cudaFreeHost(plan->h_CallValue));
    checkCudaErrors(cudaFreeHost(plan->h_OptionData));
    checkCudaErrors(cudaFree(plan->d_CallValue));
    checkCudaErrors(cudaFree(plan->d_OptionData));
}

//Main computations
extern "C" void MonteCarloGPU(TOptionPlan *plan, cudaStream_t stream)
{
    __TOptionValue *h_CallValue = plan->h_CallValue;

    if (plan->optionCount <= 0 || plan->optionCount > MAX_OPTIONS)
    {
        printf("MonteCarloGPU(): bad option count.\n");
        return;
    }
    printf("Thread %d.\n",THREAD_N);

    __TOptionData * h_OptionData = (__TOptionData *)plan->h_OptionData;

    for (int i = 0; i < plan->optionCount; i++)
    {
        const double           T = plan->optionData[i].T;
        const double           R = plan->optionData[i].R;
        const double           V = plan->optionData[i].V;
        const double       MuByT = (R - 0.5 * V * V) * T;
        const double    VBySqrtT = V * sqrt(T);
        h_OptionData[i].S        = (real)plan->optionData[i].S;
        h_OptionData[i].X        = (real)plan->optionData[i].X;
        h_OptionData[i].MuByT    = (real)MuByT;
        h_OptionData[i].VBySqrtT = (real)VBySqrtT;
    }

    checkCudaErrors(cudaMemcpyAsync(
                        plan->d_OptionData,
                        h_OptionData,
                        plan->optionCount * sizeof(__TOptionData),
                        cudaMemcpyHostToDevice, stream
                    ));

    MonteCarloOneBlockPerOption <<<plan->gridSize, THREAD_N, 0, stream >>> (
        plan->rngStates,
        (__TOptionData *)(plan->d_OptionData),
        (__TOptionValue *)(plan->d_CallValue),
        plan->pathN,
        plan->optionCount
    );
    getLastCudaError("MonteCarloOneBlockPerOption() execution failed\n");


    checkCudaErrors(cudaMemcpyAsync(
                        h_CallValue,
                        plan->d_CallValue,
                        plan->optionCount * sizeof(__TOptionValue), cudaMemcpyDeviceToHost, stream
                    ));

    //cudaDeviceSynchronize();
}

