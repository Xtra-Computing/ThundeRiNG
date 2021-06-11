#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <climits>

//#include "../pcg_basic/pcg_basic.h"
//#include "../pcg_basic/pcg_basic.c"


#define __HOST_VERIFICATE__
#include "../../mpcg.h"

#define CO_NUM  4096
#include "corr_xorshift128.h"

#define SEQ_NUM (4096)

#ifndef TARGET_SEQ
#error "TARGET_SEQ is not defined\n"
#endif



int main(int argc, char** argv)
{
    xor128_s_init(TARGET_SEQ);
    static rng_state_t state = RNG_INIT_STATE;

    //for (unsigned int j = 0; j < SEQ_NUM; j ++)
    {
        for (unsigned int i = 0; i < 1048573; i++)
        {
            rng_state_t oldstate = state;
            state_transition_funciton(&state);
            rng_state_t updated_state = multi_stream(oldstate, 2 * TARGET_SEQ + 1);
            unsigned int rng = output_function(updated_state);

            uint32_t a =mxor128_index(TARGET_SEQ);

            printf("%u\n", rng^a);
        }
    }
    fflush(stdout);
}