#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include "../pcg_basic/pcg_basic.h"
#include "../pcg_basic/pcg_basic.c"


#define __HOST_VERIFICATE__
#include "../../mpcg.h"



#define SEQ_NUM (4096)

#ifndef TARGET_SEQ
#error "TARGET_SEQ is not defined\n"
#endif



int main(int argc, char** argv)
{
    static rng_state_t state = RNG_INIT_STATE;

    //for (unsigned int j = 0; j < SEQ_NUM; j ++)
    {
        for (unsigned int i = 0; i < 8192; i++)
        {
            rng_state_t oldstate = state;
            state_transition_funciton(&state);
            rng_state_t updated_state = multi_stream(oldstate, 2 * TARGET_SEQ + 1);
            uint32_t v = (updated_state);

            printf("%u\n", v);
        }
    }
    fflush(stdout);
}