#include "xor32s.h"



inline rng_state_t multi_stream(rng_state_t state,  const rng_state_t  seq)
{
    return state + ((RNG_A * (seq << 1u)) | 1u);
}


inline rng32_t  output_function(rng_state_t oldstate)
{
    rng32_t xorshifted = ((oldstate >> 18u) ^ oldstate) >> 27u;
    rng32_t rot = oldstate >> 59u;
    return (xorshifted >> rot) | (xorshifted << ((-rot) & 31));
}
//return (uint32_t)(((state >> 22u) ^ state) >> ((state >> 61u) + 22u));


inline rng32_t pcg_output_xsh_rs_64_32(rng_state_t state)
{

    return (rng32_t)(((state >> 22u) ^ state) >> ((state >> 61u) + 22u));
}


inline rng32_t xorshiftjump (rng32_t s)
{
    unsigned int y = s;
    y ^= (y << 13);
    y ^= (y >> 17);
    y ^= (y << 5);
    return y;
}

/*
    rng_state_t tmp = oldstate; // _rotl(oldstate, current_rng % 63);

    rng_state_t tmp_update = multi_stream(tmp,  54u + current_rng + 1u);
    rng_state_t updated_state = tmp_update;
    unsigned int rng = output_function(mxorpcg_last_s[current_rng]);
    uint32_t a = (xorshiftjump(&xor32_s[current_rng]));
    mxorpcg_last_s[current_rng] = updated_state;

    return (rng ^ a);
*/

inline rng32_t application_function(rng32_t my, rng32_t other)
{
    return my ^ other;
}

