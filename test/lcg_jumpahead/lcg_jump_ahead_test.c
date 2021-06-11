
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include "pcg_basic/pcg_basic.h"
#include "../init_state.h"
/* Multi-step advance functions (jump-ahead, jump-back)
 *
 * The method used here is based on Brown, "Random Number Generation
 * with Arbitrary Stride,", Transactions of the American Nuclear
 * Society (Nov. 1994).  The algorithm is very similar to fast
 * exponentiation.
 *
 * Even though delta is an unsigned integer, we can pass a
 * signed integer to go backwards, it just goes "the long way round".
 */

uint64_t pcg_advance_lcg_64(uint64_t state, uint64_t delta, uint64_t cur_mult,
                            uint64_t cur_plus)
{
    uint64_t acc_mult = 1u;
    uint64_t acc_plus = 0u;
    while (delta > 0) {
        if (delta & 1) {
            acc_mult *= cur_mult;
            acc_plus = acc_plus * cur_mult + cur_plus;
        }
        cur_plus = (cur_mult + 1) * cur_plus;
        cur_mult *= cur_mult;
        delta /= 2;
    }
    return acc_mult * state + acc_plus;
}

int calculate_acc_value(uint64_t delta, uint64_t cur_mult,
                        uint64_t cur_plus, uint64_t *p_m, uint64_t *p_inc)
{
    uint64_t acc_mult = 1u;
    uint64_t acc_plus = 0u;
    while (delta > 0) {
        if (delta & 1) {
            acc_mult *= cur_mult;
            acc_plus = acc_plus * cur_mult + cur_plus;
        }
        cur_plus = (cur_mult + 1) * cur_plus;
        cur_mult *= cur_mult;
        delta /= 2;
    }

    if (p_m)
        *p_m = acc_mult;
    if (p_inc)
        *p_inc = acc_plus;
}

#define PCG_DEFAULT_MULTIPLIER_64  6364136223846793005ULL







/* x_{n+i} = (a^i * X_n  + c(a^i - 1)/(a - 1))mod m */

#define SEEK_NUM (9)
#define SEQ_NUM  (25)
#define JUMP_OFFSET (1)


uint32_t pcg32_random_r_seek(pcg32_random_t* rng, uint64_t m, uint64_t inc)
{
    uint64_t oldstate = rng->state;
    rng->state = oldstate * m + inc;
    uint32_t xorshifted = ((oldstate >> 18u) ^ oldstate) >> 27u;
    uint32_t rot = oldstate >> 59u;
    return (xorshifted >> rot) | (xorshifted << ((-rot) & 31));
}


uint32_t pcg32_random_r_inner_seek(pcg32_random_t* rng, uint64_t m, uint64_t inc)
{
    uint64_t oldstate = rng->state;
    rng->state = pcg_advance_lcg_64 (oldstate, SEEK_NUM,
                                     PCG_DEFAULT_MULTIPLIER_64, rng->inc);

    uint32_t xorshifted = ((oldstate >> 18u) ^ oldstate) >> 27u;
    uint32_t rot = oldstate >> 59u;
    return (xorshifted >> rot) | (xorshifted << ((-rot) & 31));
}

typedef uint32_t                rng32_t;
typedef uint64_t                rng_state_t;

void state_transition_funciton(rng_state_t *state)
{
    rng_state_t oldstate = (*state);
    *state = oldstate * RNG_A + RNG_INC;
}

void state_transition_t_funciton(rng_state_t *state, uint64_t inc)
{
    rng_state_t oldstate = (*state);
    *state = oldstate * RNG_A + inc;
}

rng_state_t multi_stream(rng_state_t state,  const int  seq)
{
    return state + ((RNG_A * (seq << 1u)) | 1u);
}

rng32_t  output_function(rng_state_t oldstate)
{
    rng32_t xorshifted = ((oldstate >> 18u) ^ oldstate) >> 27u;
    rng32_t rot = oldstate >> 59u;
    return (xorshifted >> rot) | (xorshifted << ((-rot) & 31));
}


uint32_t pcg32_random_t_seek(pcg32_random_t* rng, uint64_t m, uint64_t inc)
{
    uint64_t s = rng->state;
    rng->state = s * m + inc;
    uint64_t oldstate = rng->state;
    uint32_t xorshifted = ((oldstate >> 18u) ^ oldstate) >> 27u;
    uint32_t rot = oldstate >> 59u;
    return (xorshifted >> rot) | (xorshifted << ((-rot) & 31));
}


pcg32_random_t golden;
pcg32_random_t jumped;

pcg32_random_t seekdump;


int main (int argc, char** argv)
{

    pcg32_srandom_r(&golden, 42u, 54u);

    pcg32_srandom_r(&seekdump, 42u, 54u);

    printf("golden:\n");
    for (int i = 0; i < SEQ_NUM; i ++)
    {
        uint32_t tmp = pcg32_random_r(&golden);
        //if (i % SEEK_NUM == JUMP_OFFSET)
            printf("[%d] 0x%08x state %lu %lu\n", i, tmp, (golden.state)>>32, (golden.state)&0xffffffff);
    }

    uint64_t new_a = 0;
    uint64_t new_inc = 0;
    pcg32_srandom_r(&jumped, 42u, 54u);

    printf("state :0x%016lxULL\n", jumped.state);
    printf("a :0x%016llxULL\n", PCG_DEFAULT_MULTIPLIER_64);
    calculate_acc_value(SEEK_NUM, PCG_DEFAULT_MULTIPLIER_64, jumped.inc, &new_a, &new_inc);

    printf("original : 0x%016llxULL , 0x%016lxULL\n", PCG_DEFAULT_MULTIPLIER_64, jumped.inc);
    printf("new      : 0x%016lxULL , 0x%016lxULL\n", new_a, new_inc);



    for (int i = 0; i < SEEK_NUM; i++)
    {
        printf("state for seek @%d, : 0x%016lxULL\n",i, seekdump.state);
        pcg32_random_r(&seekdump);
    }


    printf("jumped:\n");
    for (int i = 0; i < JUMP_OFFSET; i++)
    {
        pcg32_random_r(&jumped);

    }

    for (int i = 0; i < SEQ_NUM / SEEK_NUM; i ++)
    {
        printf("[%d] 0x%08x\n", i * SEEK_NUM + JUMP_OFFSET, pcg32_random_r_seek(&jumped, new_a, new_inc));
    }


//jumped.state = pcg_advance_lcg_64(jumped.state, 5, PCG_DEFAULT_MULTIPLIER_64, 0u);
#if 0
    {
        printf("before share:\n");
        rng_state_t init_state = RNG_INIT_STATE;
        for (int i = 0; i < 10 ; i ++)
        {
            state_transition_funciton(&init_state);
            rng_state_t new_s = multi_stream(init_state, 0);
            printf("[%d] 0x%08x\n", i, output_function(new_s));
        }
    }

    {
        printf("after share:\n");
        rng_state_t init_state = RNG_INIT_STATE;
        for (int i = 0; i < 10 ; i ++)
        {
            rng_state_t new_s = multi_stream(init_state, 1);
            printf("[%d] 0x%08x\n", i, output_function(new_s));
            state_transition_funciton(&init_state);
        }
    }
    {
        pcg32_random_t dut;
        printf("corrosponding:\n");
        rng_state_t init_state = RNG_INIT_STATE;
        dut.state = init_state;
        for (int i = 0; i < 10 ; i ++)
        {

            printf("[%d] 0x%08x\n", i, pcg32_random_t_seek(&dut, PCG_DEFAULT_MULTIPLIER_64, 109 + (1 - RNG_A) * RNG_A ));
        }
    }

    {
        pcg32_random_t dut;
        printf("corrosponding:\n");
        rng_state_t init_state = RNG_INIT_STATE;
        dut.state = init_state;
        for (int i = 0; i < 10 ; i ++)
        {

            printf("[%d] 0x%08x\n", i, pcg32_random_t_seek(&dut, PCG_DEFAULT_MULTIPLIER_64, 109 + RNG_MOD));
        }
    }
#endif

    return 0;
}

