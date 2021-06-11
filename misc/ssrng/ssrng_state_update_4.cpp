#include <ap_axi_sdata.h>
#include <ap_int.h>
#include <hls_stream.h>

#include "rng_type.h"
#include "init_state.h"


#define SEEK_SIZE    (1)



extern "C" {
    void state_transition_4_stage(rng_state_stream_t &output) {
#pragma HLS interface ap_ctrl_none port = return

        static rng_state_t state INIT_STATE_4;
        while (1) {
#pragma HLS pipeline II=9
            rng_state_pkt_t v;
            rng_state_t oldstate = state;
            state = oldstate * RNG_A_SEEKED + RNG_INC_SEEKED;
            v.data = oldstate;
            output.write(v);
        }
    }
}
