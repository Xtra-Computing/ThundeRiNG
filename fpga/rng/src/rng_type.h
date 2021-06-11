#ifndef __RNG_TYPD_H__
#define __RNG_TYPD_H__


#define RNG_WIDTH               (32)
#define RNG_S_WIDTH             (64)

typedef ap_uint<RNG_WIDTH>      rng32_t;
typedef ap_uint<RNG_S_WIDTH>    rng_state_t;

typedef ap_axiu<RNG_S_WIDTH, 0, 0, 0> rng_state_pkt_t;
typedef hls::stream< rng_state_pkt_t >  rng_state_stream_t;



typedef ap_axiu<RNG_WIDTH, 0, 0, 0> rng_output_pkt_t;
typedef hls::stream<rng_output_pkt_t > rng_output_stream_t;


typedef hls::stream<rng32_t > rng_output_inner_stream_t;
typedef hls::stream< rng_state_t >  rng_state_inner_stream_t;

#ifndef __SYNTHESIS__
    #define DEBUG_PRINT(f_, ...) printf((f_), __VA_ARGS__);
#else
    #define DEBUG_PRINT(f_, ...) ;
#endif


#endif /* __RNG_TYPD_H__ */
