#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include "../pcg_basic/pcg_basic.h"
#include "../pcg_basic/pcg_basic.c"


#define SEQ_NUM (4096)

#ifndef TARGET_SEQ
#error "TARGET_SEQ is not defined\n"
#endif




uint32_t lcg_random_r(pcg32_random_t* rng)
{
    uint64_t oldstate = rng->state;
    rng->state = oldstate * 6364136223846793005ULL + rng->inc;

    return ((oldstate >> 32) + TARGET_SEQ * 1664525UL);
}

int main(int argc, char** argv)
{
    pcg32_random_t golden;
    pcg32_srandom_r(&golden, 42u, 54u);
    //for (unsigned int j = 0; j < SEQ_NUM; j ++)
    {
        for (unsigned int i = 0; i < 1048573; i++)
        {
            uint32_t v = lcg_random_r(&golden);
            printf("%u\n",v);
        }
    }
    fflush(stdout);
}