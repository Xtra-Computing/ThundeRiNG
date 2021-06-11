#include <ap_axi_sdata.h>
#include <ap_int.h>
#include <hls_stream.h>


#include "rng_type.h"
#include "init_state.h"
#include "stream_operation.h"

#include "sou_function.h"


void soum_instance(  rng_state_inner_stream_t        &s_in,
                     rng_state_inner_stream_t        &s_out,
                     rng_output_inner_stream_t       &o_in,
                     rng_output_inner_stream_t       &o_out,
                     const int id)
{

#pragma HLS interface ap_ctrl_none port=return
#pragma HLS function_instantiate variable=id
    const rng32_t tmp = tmp_xor32_s[id];
    rng32_t sg_state = tmp;
    while (1) {
        rng_state_t s_pkt = s_in.read();
        s_out.write(s_pkt);
        rng32_t o_pkg = o_in.read();

        rng_state_t oldstate = s_pkt;
        rng_state_t update = multi_stream(oldstate,  54u  + id);
        rng32_t rng = pcg_output_xsh_rs_64_32(update);
        rng32_t sg = xorshiftjump(sg_state);
        sg_state = sg;
        rng32_t output = (sg ^ rng);

        rng32_t v;
        v = application_function(output, o_pkg);
        o_out.write(v);
    }
}






#define NRNG (64)

extern "C" {
    void rng_instance_1(  rng_state_stream_t        &s_in,
                          rng_state_stream_t        &s_out,
                          rng_output_stream_t       &o_in,
                          rng_output_stream_t       &o_out)
    {
#pragma HLS interface ap_ctrl_none port=return

        static rng_state_inner_stream_t s[NRNG + 1];
#pragma HLS stream variable=s  depth=2

        static rng_output_inner_stream_t o[NRNG + 1];
#pragma HLS stream variable=o  depth=2

#pragma HLS dataflow
        axis_to_hls_stream(s_in,s[0]);
        axis_to_hls_stream(o_in,o[0]);

        for (int i = 0; i < NRNG; i++)
        {
#pragma HLS unroll
           soum_instance(s[0 + i], s[1 + i], o[0 + i] , o[1 + i], 0);
        }

        hls_to_axis_stream(s[NRNG], s_out);
        hls_to_axis_stream(o[NRNG], o_out);
    }
}