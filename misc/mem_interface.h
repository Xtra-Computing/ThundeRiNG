#ifndef __MEM_INTERFACE_H__
#define __MEM_INTERFACE_H__



const unsigned int c_size = DATA_SIZE;


void read_input(unsigned int *input, hls::stream<uint32_t> &inStream , int size)
{
#pragma HLS function_instantiate variable=input
mem_rd: for (int i = 0 ; i < size ; i++) {
#pragma HLS PIPELINE II=1
#pragma HLS LOOP_TRIPCOUNT min=c_size max=c_size
        inStream << input[i];
    }
}



//write_result(): Read result from outStream and write the result to Global Memory
void write_result(unsigned int *output, hls::stream<uint32_t> &outStream , int size)
{
mem_wr: for (int i = 0 ; i < size ; i++) {
#pragma HLS PIPELINE II=1
#pragma HLS LOOP_TRIPCOUNT min=c_size max=c_size
        //Blocking read command from OutStream
        output[i] = outStream.read();
    }
}




#endif  /* __MEM_INTERFACE_H__ */
