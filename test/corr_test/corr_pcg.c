#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include "../pcg_basic/pcg_basic.h"
#include "../pcg_basic/pcg_basic.c"


#define SEQ_NUM (4096)

#ifndef TARGET_SEQ
#error "TARGET_SEQ is not defined\n"
#endif




int main(int argc, char** argv)
{
    pcg32_random_t golden;
    pcg32_srandom_r(&golden, 42u, 54u + TARGET_SEQ);
    //for (unsigned int j = 0; j < SEQ_NUM; j ++)
    {
        for (unsigned int i = 0; i < 1048573; i++)
        {
            uint32_t v = pcg32_random_r(&golden);
            printf("%u\n",v);
        }
    }
    fflush(stdout);
}