#include <ap_axi_sdata.h>
#include <ap_int.h>
#include <hls_stream.h>


#include "rng_type.h"
#include "init_state.h"

#include "xor32s.h"

#include "ssrng.h"



#define NRMG (64 + 1)

typedef union{
    float f;
    unsigned int u;
} convertor;

template <typename T>
inline int read_from_stream_nb (hls::stream<T> &stream, T & value)
{
#pragma HLS INLINE
    if (stream.empty())
    {
        return 0;
    }
    else
    {
        value = stream.read();
        return 1;
    }
}



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


inline rng32_t application_function(rng32_t my, rng32_t other)
{
    return my ^ other;
}
*/


#define U2FLOAT  (2.3283064365386963e-10)
void application(
        hls::stream<ap_axiu<32, 0, 0, 0>>  &rng,
        hls::stream<ap_axiu<32, 0, 0, 0>>  &o_in,
        hls::stream<ap_axiu<32, 0, 0, 0>>  &o_out)
{
#pragma HLS interface ap_ctrl_none port=return
    while(1){
    	ap_uint<66> random[2];
#pragma HLS ARRAY_PARTITION variable=random dim=0 complete
    	for (int i = 0; i < 2 ; i++)
    	{
    		ap_axiu<32, 0, 0, 0> rng_pkg = rng.read();
    		random[i] = rng_pkg.data * rng_pkg.data;
    	}

        ap_axiu<32, 0, 0, 0> o_pkg ;
        ap_axiu<32, 0, 0, 0> v;

        if (read_from_stream_nb(o_in, o_pkg))
        {
        	ap_uint<66> dist = random[0] + random[1];
        	if (dist.range(65,64) == 0 )
        		v.data = o_pkg.data + 1;
        	else
        		v.data = o_pkg.data;
        }
        else
        {
        	v.data = 0;
        }

        o_out.write(v);
    }
}





#define MAX_CYCLE (65536)


template <typename T>
inline int write_to_stream (hls::stream<T> &stream, T const& value)
{
#pragma HLS INLINE
    int count = 0;
    stream << value;
    return 0;
}


template <typename T>
inline int read_from_stream (hls::stream<T> &stream, T & value)
{
#pragma HLS INLINE
    value = stream.read();
    return 0;
}


template <typename T>
void sink_stream(hls::stream<T> &stream)
{
#pragma HLS interface ap_ctrl_none port=return

//sink: for (int i = 0 ; i < size ; i++) {
    while (1) {
#pragma HLS PIPELINE II=1
        T data;
        read_from_stream(stream, data);
    }
}


template <typename T>
void dummy_stream(hls::stream<T> &stream)
{
#pragma HLS interface ap_ctrl_none port=return
//sink: for (int i = 0 ; i < size ; i++) {
    while (1) {
#pragma HLS PIPELINE II=1
        T data;
        data.data = 0;
        write_to_stream(stream, data);
    }
}

template <typename T>
void pad(hls::stream<T> &in, hls::stream<T> &out)
{
#pragma HLS interface ap_ctrl_none port=return
    while (1) {
        T data;
        read_from_stream(in, data);
        write_to_stream(out, data);
    }
}


void rng_instance(rng_state_stream_t                      &s_in,
                  rng_state_stream_t                      &s_out,
                  hls::stream<ap_axiu<32, 0, 0, 0>>       &rng,
                  const int id)
{
#pragma HLS interface ap_ctrl_none port=return
#pragma HLS function_instantiate variable=id
    const rng32_t tmp = tmp_xor32_s[id];
    rng32_t sg_state = tmp;
    while (1) {
        rng_state_pkt_t s_pkt = s_in.read();
        s_out.write(s_pkt);

        rng_state_t oldstate = s_pkt.data;
        rng_state_t update = multi_stream(oldstate,  54u  + id);
        rng32_t base = output_function(update);
        rng32_t sg = xorshiftjump(sg_state);
        sg_state = sg;
        rng32_t output = (sg ^ base);

        ap_axiu<32, 0, 0, 0> v;
        v.data = output;
        rng.write(v);
    }
}

void dump_state(  rng_state_stream_t                      &s_in,
                  hls::stream<ap_axiu<32, 0, 0, 0>>       &rng
                 )
{
#pragma HLS interface ap_ctrl_none port=return
    while (1) {
        rng_state_pkt_t s_pkt = s_in.read();

        ap_axiu<32, 0, 0, 0> v;
        v.data = s_pkt.data.range(63,32);
        rng.write(v);
        v.data = s_pkt.data.range(31,0);
        rng.write(v);
    }
}




#define BREAK_NUM           (9)
void state_transition_merge(rng_state_stream_t &in0,
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






void ssrng(hls::stream<ap_axiu<32, 0, 0, 0>> &output) {

// For free running kernel, user needs to specify ap_ctrl_none for return port.
// This will create the kernel without AXI lite interface. Kernel will always be
// in running states.
#pragma HLS interface ap_ctrl_none port = return

#pragma HLS stream variable=output  depth=4
    static rng_state_stream_t stf[9];
#pragma HLS stream variable=stf  depth=4

    static rng_state_stream_t s_stream;
#pragma HLS stream variable=s_stream  depth=16

    static rng_state_stream_t s[NRMG];
#pragma HLS stream variable=s  depth=2

    static hls::stream<ap_axiu<32, 0, 0, 0>> o[NRMG];
#pragma HLS stream variable=o  depth=2

    static hls::stream<ap_axiu<32, 0, 0, 0>> rng[NRMG - 1];
#pragma HLS stream variable=rng  depth=4


#pragma HLS dataflow


    state_transition_0_stage(stf[0]);
    state_transition_1_stage(stf[1]);
    state_transition_2_stage(stf[2]);
    state_transition_3_stage(stf[3]);
    state_transition_4_stage(stf[4]);
    state_transition_5_stage(stf[5]);
    state_transition_6_stage(stf[6]);
    state_transition_7_stage(stf[7]);
    state_transition_8_stage(stf[8]);

    state_transition_merge(
        stf[0],
        stf[1],
        stf[2],
        stf[3],
        stf[4],
        stf[5],
        stf[6],
        stf[7],
        stf[8],
        s_stream
    );
#if 1
    dummy_stream(o[0]);

    pad(s_stream, s[0]);

    for (int i = 0; i < NRMG - 1; i++)
    {
#pragma HLS unroll
        rng_instance(s[i], s[1 + i], rng[i] , i);
    }

    for (int i = 0; i < NRMG - 1; i++)
    {
#pragma HLS unroll
        application(rng[i],o[i],o[i + 1]);
    }

    sink_stream(s[NRMG - 1]);
    pad(o[NRMG - 1], output);
#else
    dummy_stream(o[0]);

    pad(s_stream, s[0]);

    for (int i = 0; i < NRMG - 1; i++)
    {
#pragma HLS unroll
        rng_instance(s[i], s[1 + i], rng[i] , i);
    }
    sink_stream(s[NRMG - 1]);
    pad(rng[0], output);
/* state verify */
    //dump_state(s_stream, output);
#endif

}
