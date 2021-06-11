#include <ap_axi_sdata.h>
#include <ap_int.h>
#include <hls_stream.h>



#include "rng_type.h"
#include "init_state.h"



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

extern "C" {
    void ssrng(rng_state_stream_t &s_stream,
               hls::stream<ap_axiu<32, 0, 0, 0>> &output) {

// For free running kernel, user needs to specify ap_ctrl_none for return port.
// This will create the kernel without AXI lite interface. Kernel will always be
// in running states.
#pragma HLS interface ap_ctrl_none port = return

        rng32_t sg_state = 0xa0f16c6dU;

        while (1) {
            rng_state_pkt_t s_pkt = s_stream.read();

            rng_state_t oldstate = s_pkt.data;
            rng_state_t update = multi_stream(oldstate,  54u  + 1u);
            rng32_t rng = output_function(update);
            rng32_t sg = xorshiftjump(sg_state);
            sg_state = sg;
            ap_axiu<32, 0, 0, 0> v;

            v.data = (sg^rng);
            output.write(v);
        }
    }
}
