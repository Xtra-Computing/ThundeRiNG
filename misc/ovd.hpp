#ifndef __OVD_HPP__
#define __OVD_HPP__

#include <ap_int.h>

// bit summation      :  OR   & 
// bit multiplication :  AND  |

inline ap_uint<1> get_o_i_1(ap_uint<1> & o_i,
                            ap_uint<1> & a_n_i)
{
#pragma HLS INLINE
	ap_uint<1> o_i_1;
	o_i_1 = o_i & a_n_i;
	return o_i_1;
}


inline ap_uint<1> get_v_i_1(ap_uint<1> & v_i,
                            ap_uint<1> & o_i,
                            ap_uint<1> & a_n_i,
                            ap_uint<1> & b_i)
{
#pragma HLS INLINE
	ap_uint<1> o_i_1 = get_o_i_1(o_i, a_n_i);
	ap_uint<1> v_i_1 = v_i &  (o_i_1 | b_i);
	return v_i_1;
}



#endif /* __OVD_HPP__ */
