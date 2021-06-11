#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

static unsigned int y = 2463534242U;

unsigned int xorshift (void)
{
    y ^= (y << 13);
    y ^= (y >> 17);
    return y ^= (y << 5);
}





#define SEQ_NUM (16384)
int main(int argc, char** argv)
{

    const uint64_t period = ((uint64_t)1 )<< 32;

    printf("//period %lu %lu\n", period,period / SEQ_NUM);
    printf("#define SEQ_NUM  (%lu)\n", SEQ_NUM);
    printf("unsigned int tmp_xor32_s[]={\n");
    fflush(stdout);

    for (unsigned int j = 0; j < SEQ_NUM; j ++)
    {
        //1048573
        for (unsigned int i = 0; i <  period / SEQ_NUM ; i++)
        {
            xorshift();
        }
        printf("0x%08xU,\n",y); fflush(stdout);
        //fflush(stdout);
    }
    printf("};\n");
}