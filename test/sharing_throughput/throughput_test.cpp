#include <random>
#include <string>
#include <cstdio>
#include <cstdint>
#include <cstddef>
#include <ctime>
#include <cstring>
#include <algorithm>

#define __HOST_VERIFICATE__
#include "../../mpcg.h"


#include <array>

using state_t = std::array<uint32_t, 4>;

static state_t decorrelator_state;
void init(state_t &s)
{
    s[0] = 123456789;
    s[1] = 362436069;
    s[2] = 521288629;
    s[3] = 88675123;
}

static uint64_t s_[ 2 ] = { 1, 1 };

static uint64_t __inline next0(void) {
    uint64_t s1 = s_[ 0 ];
    const uint64_t s0 = s_[ 1 ];
    s_[ 0 ] = s0;
    s1 ^= s1 << 23;
    return ( s_[ 1 ] = ( s1 ^ s0 ^ ( s1 >> 17 ) ^ ( s0 >> 26 ) ) );
}


void step_forward(state_t &s)
{
    uint32_t &x = s[0], &y = s[1], &z = s[2], &w = s[3];

    uint32_t t = x ^ (x << 11);

    x = y; y = z; z = w;
    w = w ^ (w >> 19) ^ (t ^ (t >> 8));
}


uint32_t __inline gen_u32(state_t &s)
{
    next0();

    return s[1];
}


static inline uint64_t rotl(const uint64_t x, int k) {
    return (x << k) | (x >> (64 - k));
}


static uint64_t s[2];

uint64_t next(void) {
    const uint64_t s0 = s[0];
    uint64_t s1 = s[1];
    const uint64_t result = rotl(s0 * 5, 7) * 9;

    s1 ^= s0;
    s[0] = rotl(s0, 24) ^ s1 ^ (s1 << 16); // a, b
    s[1] = rotl(s1, 37); // c

    return result;
}


/* This is the jump function for the generator. It is equivalent
   to 2^64 calls to next(); it can be used to generate 2^64
   non-overlapping subsequences for parallel computations. */

void jump(void) {
    static const uint64_t JUMP[] = { 0xdf900294d8f554a5, 0x170865df4b3201fc };

    uint64_t s0 = 0;
    uint64_t s1 = 0;
    for(int i = 0; i < sizeof JUMP / sizeof *JUMP; i++)
        for(int b = 0; b < 64; b++) {
            if (JUMP[i] & UINT64_C(1) << b) {
                s0 ^= s[0];
                s1 ^= s[1];
            }
            next();
        }

    s[0] = s0;
    s[1] = s1;
}


/* This is the long-jump function for the generator. It is equivalent to
   2^96 calls to next(); it can be used to generate 2^32 starting points,
   from each of which jump() will generate 2^32 non-overlapping
   subsequences for parallel distributed computations. */

void long_jump(void) {
    static const uint64_t LONG_JUMP[] = { 0xd2a98b26625eee7b, 0xdddf9b1090aa7ac1 };

    uint64_t s0 = 0;
    uint64_t s1 = 0;
    for(int i = 0; i < sizeof LONG_JUMP / sizeof *LONG_JUMP; i++)
        for(int b = 0; b < 64; b++) {
            if (LONG_JUMP[i] & UINT64_C(1) << b) {
                s0 ^= s[0];
                s1 ^= s[1];
            }
            next();
        }

    s[0] = s0;
    s[1] = s1;
}



typedef uint64_t   uint64_rng_t;
typedef uint32_t   uint32_rng_t;


struct pcg_state_setseq_64 {    // Internals are *Private*.
    uint64_rng_t state;             // RNG state.  All values are possible.
    uint64_rng_t inc;               // Controls which RNG sequence (stream) is
    // selected. Must *always* be odd.
    uint32_t sub;
};

typedef struct pcg_state_setseq_64 pcg32_random_t;


inline uint32_t pcg32_random_r(pcg32_random_t* rng)
{
    uint64_rng_t oldstate = rng->state;
    rng->state = oldstate * 6364136223846793005ULL + rng->inc;
    //rng->sub ++;
    uint32_rng_t xorshifted = ((oldstate >> 18u) ^ oldstate) >> 27u;
    uint32_rng_t rot = oldstate >> 59u;
    return (xorshifted >> rot) | (xorshifted << ((-rot) & 31));
}

void pcg32_srandom_r(pcg32_random_t* rng, uint64_t initstate, uint64_t initseq)
{
    rng->state = 0U;
    rng->inc = (initseq << 1u) | 1u;
    pcg32_random_r(rng);
    rng->state += initstate;
    pcg32_random_r(rng);
    rng->sub = 0;
    //printf("s: 0x%016lx, i: 0x%016lx, b: 0x%016x  \n", rng->state, rng->inc, rng->sub);
}

unsigned int mpcg (void)
{
    static rng_state_t state = RNG_INIT_STATE;
    rng_state_t oldstate = state;
    state_transition_funciton(&state);
    rng_state_t updated_state = multi_stream(oldstate, 3);
    return output_function(updated_state);
}



double getCurrentTimestamp(void) {
    timespec a;
    clock_gettime(CLOCK_MONOTONIC, &a);
    return (double(a.tv_nsec) * 1.0e-9) + double(a.tv_sec);
}

#define MAX_STREAM                   (1024)
#define SEQ_LENGTH                   (16384*4 )


#define OPERATOR_TEST_LENGTH         (16384 * 32* 100)

void  mult_test(int n)
{
    {
        uint64_t *tmp = (uint64_t *)malloc(sizeof(uint64_t)  * OPERATOR_TEST_LENGTH * n);
        double start_time = getCurrentTimestamp();
        for (int i = 0; i < OPERATOR_TEST_LENGTH * n; i++)
        {
            tmp[i] =  tmp[i]  * 0x853c49e6748fea9bULL;
        }
        double end_time = getCurrentTimestamp();

        printf("mult within %lf ->%ld\n", end_time - start_time, tmp[0]);
        free(tmp);
    }
}


void  mult_add_test(int n)
{
    {
        uint64_t *tmp = (uint64_t *)malloc(sizeof(uint64_t)  * OPERATOR_TEST_LENGTH * n);
        double start_time = getCurrentTimestamp();
        for (int i = 0; i < OPERATOR_TEST_LENGTH * n; i++)
        {
            tmp[i] = tmp[i] * 0x853c49e6748fea9bULL + 3;
        }
        double end_time = getCurrentTimestamp();

        printf("mult add within %lf ->%ld\n", end_time - start_time, tmp[0]);
        free(tmp);
    }
}

void add_test(int n )
{
    {

        uint64_t *tmp = (uint64_t *)malloc(sizeof(uint64_t)  * OPERATOR_TEST_LENGTH * n);
        double start_time = getCurrentTimestamp();
        for (int i = 0; i < OPERATOR_TEST_LENGTH * n; i++)
        {
            tmp[i] = tmp[i] + 0x853c49e6748fea9bULL;
        }
        double end_time = getCurrentTimestamp();

        printf("add within %lf ->%ld\n", end_time - start_time, tmp[0]);
        free(tmp);
    }
}

void ref_test(int n)
{
    {

        uint64_t *tmp = (uint64_t *)malloc(sizeof(uint64_t)  * OPERATOR_TEST_LENGTH * n);
        double start_time = getCurrentTimestamp();
        for (int i = 0; i < OPERATOR_TEST_LENGTH * n; i++)
        {
            tmp[i] = i;
        }
        double end_time = getCurrentTimestamp();

        printf("ref within %lf ->%ld\n", end_time - start_time, tmp[0]);
        free(tmp);
    }
}


pcg32_random_t lmt[MAX_STREAM];
uint32_t value[MAX_STREAM][SEQ_LENGTH];



double xoro_test(int stream_number, int *p_sum)
{
    double start_time = getCurrentTimestamp();
    for (int i = 0; i < SEQ_LENGTH/2; i++)
    {
        for (int j = 0; j < stream_number; j++)
        {
            value[j][0] = next();
        }
    }
    double end_time = getCurrentTimestamp();
    uint64_t sum = 0;
    for (int i = 0; i < SEQ_LENGTH; i++)
    {
        for (int j = 0; j < stream_number; j++)
        {
            sum += value[j][i];
        }
    }
    //printf("stream number %d, mpcg sum: %lu within %lf\n", stream_number , sum, end_time - start_time); fflush(stdout);
    if (p_sum)
        *p_sum = sum;
    return (end_time - start_time);
}


double golden_test(int stream_number, int *p_sum)
{
    init(decorrelator_state);
    double start_time = getCurrentTimestamp();
    for (int i = 0; i < SEQ_LENGTH; i++)
    {
        static rng_state_t state = RNG_INIT_STATE;
        rng_state_t oldstate = state;
        //state_transition_funciton(&state);
        for (int j = 0; j < stream_number; j++)
        {
            rng_state_t updated_state = multi_stream(oldstate, j);
            uint32_t d = gen_u32(decorrelator_state);
            value[j][0] = output_function(updated_state) ^ d;
        }
    }
    double end_time = getCurrentTimestamp();
    uint64_t sum = 0;
    for (int i = 0; i < SEQ_LENGTH; i++)
    {
        for (int j = 0; j < stream_number; j++)
        {
            sum += value[j][i];
        }
    }
    //printf("stream number %d, mpcg sum: %lu within %lf\n", stream_number , sum, end_time - start_time); fflush(stdout);
    if (p_sum)
        *p_sum = sum;
    return (end_time - start_time);
}

double mpcg_test(int stream_number, int *p_sum)
{
    double start_time = getCurrentTimestamp();
    for (int i = 0; i < SEQ_LENGTH; i++)
    {
        for (int j = 0; j < stream_number; j++)
        {
            uint32_t c  = pcg32_random_r(&lmt[j]);
            uint32_t d = gen_u32(decorrelator_state);
            value[j][0] = c ^ d;
        }
    }
    double end_time = getCurrentTimestamp();
    uint64_t sum = 0;
    for (int i = 0; i < SEQ_LENGTH; i++)
    {
        for (int j = 0; j < stream_number; j++)
        {
            sum += value[j][i];
        }
    }
    //printf("stream number %d, pcg sum: %lu within %lf\n", stream_number , sum, end_time - start_time); fflush(stdout);
    if (p_sum)
        *p_sum = sum;
    return (end_time - start_time);
}

#define REPEAT_TIMES   (16)
int main(int argc, char* argv[])
{
    ref_test(1);
    add_test(1);
    mult_test(1);
    mult_add_test(1);
    double exe_time[REPEAT_TIMES];

    for (int i = 0; i < MAX_STREAM; i++)
    {
        pcg32_srandom_r(&lmt[i], 0x853c49e6748fea9bULL + i,  0 );
    }

    //int stream_number = 4096;
    for (int tmp_stream_number = 1; tmp_stream_number < 4096 + 1; tmp_stream_number = tmp_stream_number * 2)
    {
        int stream_number = tmp_stream_number;
        for (int i = 0; i < REPEAT_TIMES; i++)
        {
            exe_time[i] = 0;
        }
        for (int  i = 0; i < REPEAT_TIMES; i++)
        {
            memset(value, 0, sizeof(int) * MAX_STREAM * SEQ_LENGTH );
            exe_time[i] = golden_test(stream_number, NULL);
        }
        std::sort(exe_time, exe_time + REPEAT_TIMES);
        printf("%d pcg time : %lf \n", stream_number , exe_time[REPEAT_TIMES / 2]); fflush(stdout);

        for (int i = 0; i < REPEAT_TIMES; i++)
        {
            exe_time[i] = 0;
        }
        for (int  i = 0; i < REPEAT_TIMES; i++)
        {
            memset(value, 0, sizeof(int) * MAX_STREAM * SEQ_LENGTH );
            exe_time[i] = mpcg_test(stream_number, NULL);
        }
        std::sort(exe_time, exe_time + REPEAT_TIMES);
        printf("%d mpcg time : %lf \n", stream_number , exe_time[REPEAT_TIMES / 2]); fflush(stdout);


        for (int i = 0; i < REPEAT_TIMES; i++)
        {
            exe_time[i] = 0;
        }
        for (int  i = 0; i < REPEAT_TIMES; i++)
        {
            memset(value, 0, sizeof(int) * MAX_STREAM * SEQ_LENGTH );
            exe_time[i] = xoro_test(stream_number, NULL);
        }
        std::sort(exe_time, exe_time + REPEAT_TIMES);
        printf("%d xoro time : %lf \n", stream_number , exe_time[REPEAT_TIMES / 2]); fflush(stdout);
    }

}



