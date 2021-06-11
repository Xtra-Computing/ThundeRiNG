#ifndef __RNG_TYPD_H__
#define __RNG_TYPD_H__


#define RNG_WIDTH               (32)
#define RNG_S_WIDTH             (64)

typedef ap_uint<RNG_WIDTH>      rng32_t;
typedef ap_uint<RNG_S_WIDTH>    rng_state_t;

typedef ap_axiu<RNG_S_WIDTH, 0, 0, 0> rng_state_pkt_t;
typedef hls::stream< rng_state_pkt_t >  rng_state_stream_t;

#endif /* __RNG_TYPD_H__ */
