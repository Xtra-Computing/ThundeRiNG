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
#include <semaphore.h>
#include <pthread.h>

#include "mkl_vsl.h"

// local includes
#include "common_for_rng_examples.hpp"



#define __HOST_VERIFICATE__
#include "mpcg.h"


// example parameters defines

#define S  (__TEST_S__)

#define N       (NS * S)
#define NS      (1024 * 8 )
#define N_PRINT 100
#define N_TIMES 1000

#define MAX_CPU  (__TEST_MAX_CPU__)
#define STATE_SHARE (__TEST_STATE_SHARE__)
#define THUNDERING_TEST (__TEST_THUNDERING_TEST__)

#define RNG_TYPE   (VSL_BRNG_PHILOX4X32X10)
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
    sem_t start;
    sem_t sync1;
    sem_t sync2;
    double time;
    uint64_t dstate[2];
    rng_state_t state;
} local_rng;

local_rng rng_thread[S];
uint64_t single_buffer[NS * S];

rng_state_t rs[NS];
const rng_state_t a = 0xb59dda5f38413d21ULL;
const rng_state_t c = 0xcd654675013e3ff8ULL;

rng_state_t rs_cpu[MAX_CPU][NS];


sem_t start;
void* root_state_thread(void* data)
{
    sem_wait(&start);
    for (int k = 0; k < N_TIMES; k++)
    {
        rng_state_t  tmp_state[8];
        static rng_state_t state[] =
        {
            0x185706b82c2e03f8ULL,
            0x2b47fed88766bb05ULL,
            0x8b33296d19bf5b4eULL,
            0xf7079824c154bf23ULL,
            0xebbf9e97aa16f694ULL,
            0x8303569fbe80c471ULL,
            0xbeb6d0b73fdb974aULL,
            0xed81149f2fb94e6fULL,
        };

        for (int i = 0; i < NS / 8; i ++)
        {
            for (int j = 0; j < 8; j++)
            {
                tmp_state[j] =  state[j] * a + c;
            }
            for (int j = 0; j < 8; j++)
            {
                rs[i * 8 + j] = tmp_state[j];
            }
            for (int j = 0; j < 8; j++)
            {
                state[j] = tmp_state[j];
            }
        }
        for (int i = 0; i < MAX_CPU; i++ )
        {
            //memcpy(rs_cpu[i], rs, NS * sizeof(rng_state_t));
        }
        for (int i = 0; i < S ; i ++)
        {
            sem_post(&rng_thread[i].sync1);
        }
        for (int i = 0; i < S ; i ++)
        {
            //sem_wait(&rng_thread[i].sync2);
        }
    }
    pthread_exit(NULL);
}



void initThunderingStream(local_rng *rng)
{
    rng->state = RNG_INIT_STATE;
    rng->dstate[0] =521288629;
    rng->dstate[1] =88675123;
}


static uint32_t __inline next0(uint64_t *s_) {

    /*uint64_t s1 = s_[ 0 ];
    const uint64_t s0 = s_[ 1 ];
    s_[ 0 ] = s0;
    s1 ^= s1 << 23;
    return ( s_[ 1 ] = ( s1 ^ s0 ^ ( s1 >> 17 ) ^ ( s0 >> 26 ) ) );
    */
    uint32_t *s = (uint32_t * )s_;
    uint32_t &x = s[0], &y = s[1], &z = s[2], &w = s[3];

    uint32_t t = x ^ (x << 11);

    x = y; y = z; z = w;
    w = w ^ (w >> 19) ^ (t ^ (t >> 8));
    return z;
}





inline rng32_t pcg_output_xsh_rs_64_32(rng_state_t state)
{

    return (rng32_t)(((state >> 22u) ^ state) >> ((state >> 61u) + 22u));
}


void thunderingStream(uint32_t *states, local_rng *rng, int n, void* buffer)
{
    unsigned int memid= (rng->id)%MAX_CPU;

#if THUNDERING_TEST
    for (int j = 0; j < n; j++)
    {

        //buffer[i] = pcg_golden();
        //
#if STATE_SHARE
        rng_state_t oldstate = rs[j];
        rng_state_t updated_state = multi_stream(oldstate, rng->id);
#else
        rng_state_t oldstate = rng->state;
        rng->state= oldstate * RNG_A + rng->id;
        rng_state_t updated_state = oldstate;
#endif

        uint32_t d = next0(rng->dstate);
        rng->buffer[j] = pcg_output_xsh_rs_64_32(updated_state)^ d ;
    }
#else
    viRngUniformBits32( VSL_RNG_METHOD_UNIFORMBITS32_STD, stream_vec[rng->id], n, (unsigned int *)buffer);
#endif
}




void* generation_thread(void* data)
{
    local_rng* rng = (local_rng*)(data);
    //printf("%d\n", rng->id);
    cpu_set_t new_mask;
    cpu_set_t was_mask;
    uint32_t tid = (rng->id) % MAX_CPU;

    CPU_ZERO(&new_mask);

    CPU_SET( tid, &new_mask);

    if (sched_getaffinity(0, sizeof(was_mask), &was_mask) == -1) {
        printf("Error: sched_getaffinity(%d, sizeof(was_mask), &was_mask)\n", tid);
    }
    if (sched_setaffinity(0, sizeof(new_mask), &new_mask) == -1) {
        printf("Error: sched_setaffinity(%d, sizeof(new_mask), &new_mask)\n", tid);
    }
    if (sched_getaffinity(0, sizeof(new_mask), &new_mask) == -1) {
        printf("Error: sched_getaffinity(%d, sizeof(was_mask), &was_mask)\n", tid);
    }

    double startStamp, endStamp;

    sem_wait(&rng->start);
    startStamp = getCurrentTimestamp();


    for (int i = 0; i < N_TIMES ; i++)
    {
#if STATE_SHARE
        sem_wait(&rng->sync1);
#endif
        thunderingStream(NULL , rng, NS,rng->buffer);
#if STATE_SHARE
        //sem_post(&rng->sync2);
#endif
        //viRngUniformBits32( VSL_RNG_METHOD_UNIFORMBITS32_STD, stream_vec[rng->id], NS, (unsigned int *)rng->buffer);
        //oneapi::mkl::rng::generate(oneapi::mkl::rng::uniform<std::uint32_t> {}, engine_vec[rng->id], NS, buffer_vec[rng->id]);
        //
    }
    //queue_vec[rng->id].wait_and_throw();
    endStamp = getCurrentTimestamp();
    rng->time = endStamp - startStamp;

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


    std::list<my_sycl_device_types> list_of_devices;
    set_list_of_devices(list_of_devices);

    for (auto it = list_of_devices.begin(); it != list_of_devices.end(); ++it) {

        sycl::device my_dev;
        bool my_dev_is_found = false;
        get_sycl_device(my_dev, my_dev_is_found, *it);
        pthread_t rs_tid;

        if (my_dev_is_found) {
            for (int i = 0; i < S; i++) {
                VSLStreamStatePtr stream;
                vslNewStream( &stream, RNG_TYPE, 777 );
                stream_vec.push_back( stream );
                sem_init(&rng_thread[i].start, 0, 0);
                sem_init(&rng_thread[i].sync1, 0, 0);
                sem_init(&rng_thread[i].sync2, 0, 0);
                initThunderingStream(&rng_thread[i]);

            }
            sem_init(&start, 0, 0);

            pthread_create(&rs_tid, NULL, root_state_thread, NULL);
            for (int i = 0; i < S; i++) {
                rng_thread[i].id = i;
                pthread_create(&rng_thread[i].tid, NULL, generation_thread, (void *)&rng_thread[i]) ;
            }
            sleep(1);
            double startStamp, endStamp;
            startStamp = getCurrentTimestamp();

            sem_post(&start);
            for (int i = 0; i < S; i ++)
            {
                sem_post(&rng_thread[i].start);
            }



            for (int i = 0; i < S; i++)
                pthread_join(rng_thread[i].tid, NULL);
            endStamp = getCurrentTimestamp();
            for (int i = 0; i < S; i++)
            {
                std::cout << "\t\t " << rng_thread[i].id << " in:" << rng_thread[i].time << std::endl;
            }
            std::cout << "\t"<< S<<" -"<<THUNDERING_TEST<< STATE_SHARE<<" "<<"Total Time: " << endStamp - startStamp << std::endl;

            {
                VSLStreamStatePtr stream;
                vslNewStream( &stream, RNG_TYPE, 777 );


                double startStamp, endStamp;
                startStamp = getCurrentTimestamp();
                for (int i = 0; i < N_TIMES * S ; i++)
                {
                    thunderingStream(NULL , &rng_thread[0], NS, rng_thread[0].buffer);
                    //viRngUniformBits32( VSL_RNG_METHOD_UNIFORMBITS32_STD, stream, N, (unsigned int *)single_buffer);
                    //oneapi::mkl::rng::generate(oneapi::mkl::rng::uniform<std::uint32_t> {}, engine_vec[rng->id], NS, buffer_vec[rng->id]);
                    //
                }
                //queue_vec[rng->id].wait_and_throw();
                endStamp = getCurrentTimestamp();

                std::cout << "\t"<< S<<" -"<<THUNDERING_TEST<< STATE_SHARE<<" "<<"Single" << " in:  " << endStamp - startStamp << std::endl;
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


