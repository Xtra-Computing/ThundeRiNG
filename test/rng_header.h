

#define __HOST_VERIFICATE__
#include "mpcg.h"

#include "pcg_basic.h"

#include "limits.h"

#ifndef SELECTED_METHOD
#error "SELECTED_METHOD is not defined!\n"
#endif

#ifndef CO_NUM
#error "CO_NUM is not defined!\n"
#endif

#include "bit_operation.h"

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(arr) sizeof(arr)/sizeof((arr)[0])
#endif


typedef unsigned int(* get_rng)(void);

typedef unsigned int(* init_state)(int index);

typedef struct
{
    const char  *name;
    get_rng     func;
    init_state  init;
    get_rng     golden;
} rng_method_t;


inline uint32_t pcg_output_xsh_rs_64_32(uint64_t state)
{

    return (uint32_t)(((state >> 22u) ^ state) >> ((state >> 61u) + 22u));
}

inline rng32_t  pcg_output_xsh_rr_64_32(rng_state_t oldstate)
{
    rng32_t xorshifted = ((oldstate >> 18u) ^ oldstate) >> 27u;
    rng32_t rot = oldstate >> 59u;
    return (xorshifted >> rot) | (xorshifted << ((-rot) & 31));
}

unsigned int reverseBits(unsigned int num)
{
    unsigned int count = sizeof(num) * 8 - 1;
    unsigned int reverse_num = num;

    num >>= 1;
    while(num)
    {
       reverse_num <<= 1;
       reverse_num |= num & 1;
       num >>= 1;
       count--;
    }
    reverse_num <<= count;
    return reverse_num;
}

#ifndef C_SHIFT
#define C_SHIFT  (0)
#endif


#include "rngs/ompcg.h"
#include "rngs/xorshift128p.h"
#include "rngs/xorshift128.h"
#include "rngs/xorshift32.h"
#include "rngs/xorwow.h"
#include "rngs/lut_sr.h"

#include "rngs/sota1.h"

