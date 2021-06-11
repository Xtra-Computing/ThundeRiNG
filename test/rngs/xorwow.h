#ifndef __XORWOW_H__
#define __XORWOW_H__

struct xorwow_state {
    uint32_t a, b, c, d, e;
    uint32_t counter;
};

rng_state_t last_s[CO_NUM];

xorwow_state local_state[CO_NUM];
int co_index[CO_NUM];

/* The state array must be initialized to not be all zero in the first four words */
uint32_t xorwow(struct xorwow_state *state, int i)
{
    /* Algorithm "xorwow" from p. 5 of Marsaglia, "Xorshift RNGs" */
    uint32_t t = state->e;
    uint32_t s = state->a;
    state->e = state->d;
    state->d = state->c;
    state->c = state->b;
    state->b = s;
    t ^= t >> 2;
    t ^= t << 1;
    t ^= s ^ (s << 4);
    state->a = t;
    state->counter += 362437 * ( i << 2 | 1);
    return t + state->counter;
}


inline rng32_t  output_function_inter(rng_state_t oldstate, rng_state_t newstate, int shift)
{
    rng32_t xorshifted = ((newstate >> 18u) ^ newstate) >> 27u;
    rng32_t rot = ((oldstate ) >> 53u) +  (shift & 2047);
    return (xorshifted >> rot) | (xorshifted << ((-rot) & 31));
}

unsigned int mxorwowpcg (void)
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

    rng_state_t tmp_update = multi_stream(tmp,  1 + co_index[current_rng] * 2);
    //rng_state_t tmp_inc = co_index[current_rng] * 2;
    //rng_state_t inc = rev64(&tmp_inc);
    //rng_state_t updated_state = tmp_update ^ (inc + 1);
    rng_state_t updated_state = tmp_update;
    unsigned int rng = output_function_inter(last_s[current_rng], updated_state, (current_rng * 3) | 1);
    uint32_t a = xorwow(&local_state[current_rng], 0);
    last_s[current_rng] = updated_state;

    return (rng ^ a);
}


unsigned int xorwow_init(int index)
{
    int i = index;
    co_index[i] = (single_pcg() / 10000);
    local_state[i].a = co_index[i] | 1;
    local_state[i].b = (co_index[i] / 3 ) | 1;
    local_state[i].c = co_index[i];
    local_state[i].d = (co_index[i] / 7 )  * 2;
    local_state[i].e = co_index[i] * 2;
    return 0;
}


#endif /* __XORWOW_H__ */
