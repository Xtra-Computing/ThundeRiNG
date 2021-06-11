#ifndef __XORSHIFT32_H__
#define __XORSHIFT32_H__


#include "xor32s.h"

static unsigned int y = 2463534242U;

unsigned int xorshift (void)
{
    y ^= (y << 13);
    y ^= (y >> 17);
    return y ^= (y << 5);
}

unsigned int xorshiftjump (unsigned int *s)
{
    unsigned int y = *s;
    y ^= (y << 13);
    y ^= (y >> 17);
    y ^= (y << 5);
    *s = y;
    return y;
}

unsigned int xor32_s[CO_NUM];
uint64_t xor128_tmp_s[2];


extern void xor128jump(uint64_t *s);
unsigned int xor32_s_init(int index)
{
    unsigned int index_jump = 4096/CO_NUM;
    xor32_s[index]= tmp_xor32_s[index_jump * index];
    return 0;
}

unsigned int mxor32pcg(void)
{
    static unsigned long long counter = 0;
    static rng_state_t state = RNG_INIT_STATE;
    rng_state_t oldstate = state;
    int current_rng = counter % CO_NUM;
    if (current_rng == 0)
    {
        state_transition_funciton(&state);
    }
    counter++;
    rng_state_t tmp = oldstate; // _rotl(oldstate, current_rng % 63);

    rng_state_t tmp_update = multi_stream_seeked(tmp,  54u + current_rng*2 + 1);
    rng_state_t updated_state = tmp_update;
    unsigned int rng = output_function(mxorpcg_last_s[current_rng]);
    uint32_t a = (xorshiftjump(&xor32_s[current_rng]));
    mxorpcg_last_s[current_rng] = updated_state;

    return (rng ^ a);
}


unsigned int mxor32pcg_rs(void)
{
    static unsigned long long counter = 0;
    static rng_state_t state = RNG_INIT_STATE;
    rng_state_t oldstate = state;
    int current_rng = counter % CO_NUM;
    if (current_rng == 0)
    {
        state_transition_funciton(&state);
    }
    counter++;
    rng_state_t tmp = oldstate; // _rotl(oldstate, current_rng % 63);

    rng_state_t tmp_update = multi_stream_seeked(tmp,  54u + current_rng*2 + 1);
    rng_state_t updated_state = tmp_update;
    unsigned int rng = pcg_output_xsh_rs_64_32(mxorpcg_last_s[current_rng]);
    uint32_t a = (xorshiftjump(&xor32_s[current_rng]));
    mxorpcg_last_s[current_rng] = updated_state;

    return (rng ^ a);
}



unsigned int mxor32lcgs(void)
{
    static unsigned long long counter = 0;
    static rng_state_t state = RNG_INIT_STATE;
    rng_state_t oldstate = state;
    int current_rng = counter % CO_NUM;
    if (current_rng == 0)
    {
        state_transition_funciton(&state);
    }
    counter++;
    rng_state_t tmp = oldstate;
    rng_state_t tmp_update = multi_stream_seeked(tmp,  54u + current_rng*2 + 1);
    rng_state_t updated_state = tmp_update;
    unsigned int rng = (tmp_update>>32 );

    //unsigned int rng = ( mxorpcg_last_s[current_rng]>>32 )  + current_rng * 1664525UL;
    uint32_t a = (xorshiftjump(&xor32_s[current_rng]));
    mxorpcg_last_s[current_rng] = updated_state;

    return (rng ^ a);
}





#endif /* __XORSHIFT32_H__ */
