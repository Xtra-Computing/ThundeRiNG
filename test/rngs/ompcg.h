#ifndef __OMPCG_H__
#define __OMPCG_H__


pcg32_random_t opcg_rng[CO_NUM];
unsigned int mopcg (void)
{
    static unsigned long long counter = 0;
    int current_rng = counter % CO_NUM;
    counter++;
    return pcg32_random_r(&opcg_rng[current_rng]);
}



uint32_t lcg_random_r(pcg32_random_t* rng)
{
    uint64_t oldstate = rng->state;
    rng->state = oldstate * 6364136223846793005ULL + rng->inc;

    return (oldstate >> 32);
}

unsigned int molcg (void)
{
    static unsigned long long counter = 0;
    int current_rng = counter % CO_NUM;
    counter++;
    return lcg_random_r(&opcg_rng[current_rng]);
}

unsigned int single_ss_pcg (void)
{
    static rng_state_t state = RNG_INIT_STATE;
    rng_state_t oldstate = state;
    state_transition_funciton(&state);
    rng_state_t updated_state = multi_stream(oldstate, 3);
    return output_function(updated_state);
}

unsigned int single_pcg (void)
{
    return pcg_golden();
}

unsigned int original_lcg_init(int index)
{
    int i = index;
    //for (int i = 0 ; i < CO_NUM; i++)
    {
        pcg32_srandom_r(&opcg_rng[i], 42u, 54u + i);
    }
    return 0;
}

#endif /* __OMPCG_H__ */
