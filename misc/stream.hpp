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
void sink_stream(hls::stream<T> &stream, int size)
{
sink: for (int i = 0 ; i < size ; i++) {
#pragma HLS PIPELINE II=1
        T data;
        read_from_stream(stream, data);
    }
}


template <typename T>
void dummy_stream(hls::stream<T> &stream, int size)
{
sink: for (int i = 0 ; i < size ; i++) {
#pragma HLS PIPELINE II=1
        T data = 0;
        write_to_stream(stream, data);
    }
}
