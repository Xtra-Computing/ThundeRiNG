#include <ap_axi_sdata.h>
#include <ap_int.h>
#include <hls_stream.h>
#include "stream_operation.h"

#include "rng_type.h"


extern "C" {
    void rng_start(rng_output_stream_t &output) {
#pragma HLS interface ap_ctrl_none port = return
        dummy_stream(output);
    }
}
