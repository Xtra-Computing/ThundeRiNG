#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include "../xor32s.h"

static unsigned int y = 2463534242U;

extern unsigned int tmp_xor32_s[];
unsigned int xorshift (void)
{
    y ^= (y << 13);
    y ^= (y >> 17);
    return y ^= (y << 5);
}




#define SEQ_NUM (4096)

#ifndef TARGET_SEQ
#error "TARGET_SEQ is not defined\n"
#endif

int main(int argc, char** argv)
{
    y = tmp_xor32_s[TARGET_SEQ% SEQ_NUM];
    //for (unsigned int j = 0; j < SEQ_NUM; j ++)
    {
        //1048573
        for (unsigned int i = 0; i < 1048573; i++)
        {
            uint32_t v = xorshift();
            printf("%u\n",v);
        }
    }
    fflush(stdout);
}