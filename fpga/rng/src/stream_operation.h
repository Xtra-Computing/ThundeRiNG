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

template <int T>
void axis_to_hls_stream( hls::stream< ap_axiu<T, 0, 0, 0> > &in, hls::stream< ap_uint<T> > &out )
{
#pragma HLS interface ap_ctrl_none port=return
    while (1) {
        ap_axiu<T, 0, 0, 0>  sdata = in.read();
        out.write(sdata.data);
    }
}


template <int T>
void hls_to_axis_stream( hls::stream< ap_uint<T> > &in, hls::stream< ap_axiu<T, 0, 0, 0> > &out)
{
#pragma HLS interface ap_ctrl_none port=return
    while (1) {
        ap_uint<T> data = in.read();
        ap_axiu<T, 0, 0, 0>  pkg;
        pkg.data = data;
        out.write(pkg);
    }
}
