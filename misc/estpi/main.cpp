#include <ap_axi_sdata.h>
#include <ap_int.h>
#include <hls_stream.h>



#include "rng_type.h"
#include "init_state.h"




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
    void rmain(hls::stream<ap_axiu<32, 0, 0, 0>>  &rng,
               hls::stream<ap_axiu<32, 0, 0, 0>>  &input,
               hls::stream<ap_axiu<32, 0, 0, 0>>  &output) {

// For free running kernel, user needs to specify ap_ctrl_none for return port.
// This will create the kernel without AXI lite interface. Kernel will always be
// in running states.
#pragma HLS interface ap_ctrl_none port = return

        while (1) {
            ap_axiu<32, 0, 0, 0> v = input.read();
            ap_axiu<32, 0, 0, 0> r = rng.read();

            v.data = v.data + r.data;
            output.write(v);
            if (v.last) {
                break;
            }
        }
    }
}
