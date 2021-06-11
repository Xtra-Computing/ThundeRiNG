#include <hls_stream.h>
#include <string.h>
#include <ap_int.h>

#include "asap_type.h"
#include "stream.hpp"
#include "config.h"

#include "fpga_mpcg.h"
#include "fpga_edge_match.h"
#include "mem_interface.h"




static void merge(hls::stream<uint32_t> &s, hls::stream<uint32_t> &d, hls::stream<edge_t> &edge, int size)
{
    for (int i = 0 ; i < size ; i++) {
#pragma HLS PIPELINE II=1
        uint32_t s_value, d_value;
        read_from_stream(s, s_value);
        read_from_stream(d, d_value);
        edge_t e;
        if (s_value > d_value)
        {
            e.s = d_value;
            e.d = s_value;
        }
        else
        {
            e.s = s_value;
            e.d = d_value;
        }
        write_to_stream(edge, e);
    }
}


void first_edge_sampling (hls::stream<rng_state_t>  &s,
                          hls::stream<rng_state_t>  &s_out,
                          hls::stream<fe_select_t>  &fe_index,
                          int size)
{
    ap_uint<32> counter = 0;
    ap_uint<64> sample_condition;

    fe_select_t  out = 0;
first_edge: for (int i = 0 ; i < size ; i ++) {
        rng_state_t state;
        read_from_stream(s, state);
        write_to_stream(s_out, state);

        rng_state_t l1_state = multi_stream(state, 1);
        rng32_t r = output_function(l1_state);
        counter ++;
        ap_uint < RNG_WIDTH + 1 >  r_0 = r + 1;
        sample_condition = counter * r_0;
        if (sample_condition.range(63, 32) <= E_NUM)
        {
            out = sample_condition.range(63, 32);
        }
        else
        {
            out = 0;
        }
        write_to_stream(fe_index, out);

    }
}

void reservoir_sampling(
    hls::stream<edge_t>       &edge_in,
    hls::stream<edge_t>       &edge_out,
    hls::stream<rng_state_t>  &s_in,
    hls::stream<rng_state_t>  &s_out,
    hls::stream<uint32_t>     &res_int,
    hls::stream<uint32_t>     &res_out,
    hls::stream<fe_select_t>  &fe_in,
    hls::stream<fe_select_t>  &fe_out,
    int                       size,
    const int                 e_index,
    const int                 secondary_seq
)
{

    edge_t first_edge;
    ap_uint<1> first_edge_valid = FLAG_RESET;

    edge_t second_edge;
    ap_uint<1> second_edge_valid = FLAG_RESET;
    ap_uint<32> adj_count = 0;
#pragma HLS dependence variable=adj_count inter false
    edge_t match_edge;
    ap_uint<1> matched_flag = FLAG_RESET;

    uint32_t res_value;

    ap_uint<64> l2_sample_condition;

execute: for (int i = 0 ; i < size ; i ++) {
#pragma HLS PIPELINE II=1
#pragma HLS LOOP_TRIPCOUNT min=c_size max=c_size


        read_from_stream(res_int, res_value);

        edge_t edge;
        read_from_stream(edge_in, edge);
        write_to_stream(edge_out, edge);

        rng_state_t primary;
        read_from_stream(s_in, primary);
        write_to_stream(s_out, primary);

        fe_select_t fe;
        read_from_stream(fe_in, fe);
        write_to_stream(fe_out, fe);

        rng_state_t l2_state = multi_stream(primary, secondary_seq);
        rng32_t r2 = output_function(l2_state);

        ap_uint<32> tmp_counter = adj_count + 1;

        {
#pragma HLS RESOURCE variable=l2_sample_condition latency=2
            l2_sample_condition = tmp_counter * r2;
        }

        if (fe == e_index)
        {

            first_edge = edge;
            first_edge_valid = FLAG_SET;
            adj_count = 0;
            second_edge_valid = FLAG_RESET;
            matched_flag = FLAG_RESET;
        }
        else
        {
            if (first_edge_valid == FLAG_SET)
            {
                if (IS_ADJACENT(edge, first_edge))
                {
                    adj_count ++;

                    if (l2_sample_condition.range(63, 32) == 0)
                    {
                        second_edge = edge;
                        second_edge_valid = FLAG_SET;
                        match_edge.s = get_ms(first_edge, second_edge);
                        match_edge.d = get_md(first_edge, second_edge);
                    }
                    else
                    {
                        if (second_edge_valid == FLAG_SET)
                        {
                            if (IS_CLOSE(match_edge, edge))
                            {
                                matched_flag = FLAG_SET;
                            }
                        }
                    }
                }
            }
        }

        uint32_t out_value;

        if (matched_flag == FLAG_SET)
            out_value = adj_count + res_value;
        else
            out_value = res_value;
        write_to_stream(res_out, out_value);
    }
}





extern "C" {
    void data_flow(unsigned int *s, unsigned int *d, unsigned int *output, int incr, int size1)
    {
#pragma HLS INTERFACE m_axi port=s offset=slave bundle=gmem1
#pragma HLS INTERFACE s_axilite port=s bundle=control

#pragma HLS INTERFACE m_axi port=d offset=slave bundle=gmem2
#pragma HLS INTERFACE s_axilite port=d bundle=control

#pragma HLS INTERFACE m_axi port=output offset=slave bundle=gmem
#pragma HLS INTERFACE s_axilite port=output bundle=control

#pragma HLS INTERFACE s_axilite port=incr bundle=control
#pragma HLS INTERFACE s_axilite port=size1 bundle=control
#pragma HLS INTERFACE s_axilite port=return bundle=control
        static hls::stream<uint32_t> s_stream;
        static hls::stream<uint32_t> d_stream;


        const int size = DATA_SIZE;
        static hls::stream<edge_t> edge[E_NUM];
#pragma HLS stream variable=edge  depth=2


        static hls::stream<rng_state_t> primary;
#pragma HLS stream variable=primary  depth=2

        static hls::stream<rng_state_t> state[E_NUM];
#pragma HLS stream variable=state  depth=2


        static hls::stream<uint32_t> res[E_NUM];
#pragma HLS stream variable=res  depth=2

        static hls::stream<fe_select_t> fe_index[E_NUM];
#pragma HLS stream variable=fe_index  depth=2



#pragma HLS dataflow

        read_input(s, s_stream, size);
        read_input(d, d_stream, size);
        merge(s_stream, d_stream, edge[0], size);

        dummy_stream(res[0], size);
        rng_state_update(primary, size);

        first_edge_sampling(primary, state[0], fe_index[0],size);

        for (int i = 0; i < E_NUM - 1; i ++)
        {
#pragma HLS UNROLL
            reservoir_sampling( edge[i], edge[i + 1],
                                state[i], state[i + 1],
                                res[i], res[i + 1],
                                fe_index[i], fe_index[i + 1],
                                size,
                                i,
                                ((E_NUM + 1) * 2 + 3));
        }
        sink_stream(edge[E_NUM - 1], size);
        sink_stream(state[E_NUM - 1], size);
        sink_stream(fe_index[E_NUM - 1], size);


        write_result(output, res[E_NUM - 1], size);
    }
}
