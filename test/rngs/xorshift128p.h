#ifndef __XORSHIFT128P_H__
#define __XORSHIFT128P_H__



uint64_t xor128p_s[CO_NUM][2];

rng_state_t mxorpcg_last_s[CO_NUM];
uint64_t xor128next(uint64_t *s) {
    uint64_t s1 = s[0];
    const uint64_t s0 = s[1];
    const uint64_t result = s0 + s1;
    s[0] = s0;
    s1 ^= s1 << 23; // a
    s[1] = s1 ^ s0 ^ (s1 >> 18) ^ (s0 >> 5); // b, c
    return result;
}


/* This is the jump function for the generator. It is equivalent
   to 2^64 calls to next(); it can be used to generate 2^64
   non-overlapping subsequences for parallel computations. */

void xor128jump(uint64_t *s) {
    static const uint64_t JUMP[] = { 0x8a5cd789635d2dff, 0x121fd2155c472f96 };

    uint64_t s0 = 0;
    uint64_t s1 = 0;
    for (unsigned int i = 0; i < sizeof JUMP / sizeof * JUMP; i++)
        for (int b = 0; b < 64; b++) {
            if (JUMP[i] & UINT64_C(1) << b) {
                s0 ^= s[0];
                s1 ^= s[1];
            }
            xor128next(s);
        }

    s[0] = s0;
    s[1] = s1;
}

unsigned int xor128p_s_init(int index)
{
    xor128p_s[index][0] = 0x5c472f121fd215LLU;
    xor128p_s[index][1] = 0x121fd2155c472fLLU;
    for (int i = 0; i < index * 2; i++)
    {
        xor128jump(&xor128p_s[index][0]);
    }
    return 0;
}

unsigned int mxor128p_pcg_rr(void)
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

    rng_state_t tmp_update = multi_stream(tmp,  54u + current_rng);
    rng_state_t updated_state = tmp_update;
    unsigned int rng = output_function(mxorpcg_last_s[current_rng]);
    uint32_t a = (xor128next(&xor128p_s[current_rng][0]))>>32;
    mxorpcg_last_s[current_rng] = updated_state;

    return (rng ^ a);
}


#endif /* __XORSHIFT128_H__ */
