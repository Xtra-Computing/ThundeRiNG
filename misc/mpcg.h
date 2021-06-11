#ifndef __MPCG_H__
#define __MPCG_H__
#include "stdint.h"

#include "init_state.h"



#define RNG_WIDTH               (32)
#define RNG_S_WIDTH             (64)

#ifndef __HOST_VERIFICATE__
typedef ap_uint<RNG_WIDTH>      rng32_t;
typedef ap_uint<RNG_S_WIDTH>    rng_state_t;
#else
typedef uint32_t                rng32_t;
typedef uint64_t                rng_state_t;
#endif

inline void state_transition_funciton(rng_state_t *state)
{
    rng_state_t oldstate = (*state);
    *state = oldstate * RNG_A + RNG_INC;
}

inline rng_state_t multi_stream(rng_state_t state,  const int  seq)
{
    return state + ((RNG_A * (seq << 1u)) | 1u);
}

inline rng_state_t multi_stream_seeked(rng_state_t state,  const int  seq)
{
   return state + ((0x5b5fee90a1001dcd * seq  + 0x219b40fff5054705) | 1u);
}

inline rng32_t  output_function(rng_state_t oldstate)
{
    rng32_t xorshifted = ((oldstate >> 18u) ^ oldstate) >> 27u;
    rng32_t rot = oldstate >> 59u;
    return (xorshifted >> rot) | (xorshifted << ((-rot) & 31));
}



inline unsigned int pcg_golden(void)
{
    static uint64_t state = RNG_INIT_STATE;
    uint64_t oldstate = state;
    state = oldstate * RNG_A + RNG_INC;

    unsigned int xorshifted = ((oldstate >> 18u) ^ oldstate) >> 27u;
    unsigned int rot = oldstate >> 59u;
    return (xorshifted >> rot) | (xorshifted << ((-rot) & 31));
}


#endif /* __MPCG_H__ */
