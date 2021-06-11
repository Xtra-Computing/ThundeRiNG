#ifndef __FPGA_MPCG_H__
#define __FPGA_MPCG_H__

#include "mpcg.h"

void rng_state_update(hls::stream<rng_state_t> &s, int size)
{
    static rng_state_t state = RNG_INIT_STATE;
    for (int i = 0 ; i < size ; i++)
    {
        rng_state_t oldstate = state;
        state_transition_funciton(&state);
        write_to_stream(s, oldstate);
    }
}

inline rng32_t get_rng_value(rng_state_t &oldstate)
{
    rng32_t xorshifted = ((oldstate >> 18u) ^ oldstate) >> 27u;
    rng32_t rot = oldstate >> 59u;
    return (xorshifted >> rot) | (xorshifted << ((-rot) & 31));
}



#endif /* __FPGA_MPCG_H__ */
