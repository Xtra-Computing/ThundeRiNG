#include <ap_axi_sdata.h>
#include <ap_int.h>
#include <hls_stream.h>

#include "rng_type.h"
#include "init_state.h"


#define BREAK_NUM           (9)

extern "C" {
    void ssrng_state_merge(rng_state_stream_t &in0,
                                rng_state_stream_t &in1,
                                rng_state_stream_t &in2,
                                rng_state_stream_t &in3,
                                rng_state_stream_t &in4,
                                rng_state_stream_t &in5,
                                rng_state_stream_t &in6,
                                rng_state_stream_t &in7,
                                rng_state_stream_t &in8,
                                rng_state_stream_t &out
                               )
    {


#pragma HLS interface ap_ctrl_none port = return

        while (1) {
#pragma HLS pipeline

            rng_state_pkt_t v[BREAK_NUM];
            v[0] = in0.read();
            v[1] = in1.read();
            v[2] = in2.read();
            v[3] = in3.read();
            v[4] = in4.read();
            v[5] = in5.read();
            v[6] = in6.read();
            v[7] = in7.read();
            v[8] = in8.read();

            for (int i = 0; i < BREAK_NUM; i++)
            {
                out.write(v[i]);
            }
        }
    }
}
