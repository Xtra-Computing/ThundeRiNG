#include <stdio.h>

#define STR ("\
stream_connect=ssrng_state_update_0_1.output:ssrng_state_merge_1.in0 \n\
stream_connect=ssrng_state_update_1_1.output:ssrng_state_merge_1.in1 \n\
stream_connect=ssrng_state_update_2_1.output:ssrng_state_merge_1.in2 \n\
stream_connect=ssrng_state_update_3_1.output:ssrng_state_merge_1.in3 \n\
stream_connect=ssrng_state_update_4_1.output:ssrng_state_merge_1.in4 \n\
stream_connect=ssrng_state_update_5_1.output:ssrng_state_merge_1.in5 \n\
stream_connect=ssrng_state_update_6_1.output:ssrng_state_merge_1.in6 \n\
stream_connect=ssrng_state_update_7_1.output:ssrng_state_merge_1.in7 \n\
stream_connect=ssrng_state_update_8_1.output:ssrng_state_merge_1.in8 \n\
stream_connect=mem_read_1.stream:rng_main_1.input \n\
stream_connect=rng_main_1.output:mem_write_1.stream  \n\
")

#define SLR_STR ("\
slr=ssrng_state_update_0_1:SLR0 \n\
slr=ssrng_state_update_1_1:SLR0 \n\
slr=ssrng_state_update_2_1:SLR0 \n\
slr=ssrng_state_update_3_1:SLR0 \n\
slr=ssrng_state_update_4_1:SLR0 \n\
slr=ssrng_state_update_5_1:SLR0 \n\
slr=ssrng_state_update_6_1:SLR0 \n\
slr=ssrng_state_update_7_1:SLR0 \n\
slr=ssrng_state_update_8_1:SLR0 \n\
slr=ssrng_state_merge_1:SLR0 \n\
slr=mem_read_1:SLR0 \n\
slr=rng_main_1:SLR0 \n\
slr=mem_write_1:SLR0 \n\
slr=rng_start_1:SLR0 \n\
slr=rng_end_1:SLR0 \n\
")

#define END_STR ("\
[vivado] \n\
prop=run.impl_1.STEPS.OPT_DESIGN.ARGS.DIRECTIVE=Explore \n\
prop=run.impl_1.STEPS.PLACE_DESIGN.ARGS.DIRECTIVE=Explore \n\
prop=run.impl_1.STEPS.PHYS_OPT_DESIGN.IS_ENABLED=true \n\
prop=run.impl_1.STEPS.PHYS_OPT_DESIGN.ARGS.DIRECTIVE=Explore \n\
prop=run.impl_1.STEPS.ROUTE_DESIGN.ARGS.DIRECTIVE=Explore \n\
")

#if 0
int main(int argc, char **argv) {
    printf("[connectivity]\n");
    printf(STR);
    printf("\n");

    printf("stream_connect=ssrng_state_merge_1.out:rng_instance_1_1.s_in \n");
    printf("stream_connect=rng_start_1.output:rng_instance_1_1.o_in \n");

    for(int i = 0;i < CUs - 1; i++)
    {
        printf("stream_connect=rng_instance_%d_1.s_out:rng_instance_%d_1.s_in\n", i + 1,i + 2);
        printf("stream_connect=rng_instance_%d_1.o_out:rng_instance_%d_1.o_in\n", i + 1,i + 2);
    }
    printf("\n");
    printf("stream_connect=rng_instance_%d_1.s_out:rng_end_1.input\n", CUs);
    printf("stream_connect=rng_instance_%d_1.o_out:rng_main_1.rng\n", CUs);

    printf(END_STR);

    return 0;

}
#else

int main(int argc, char **argv) {
    printf("[connectivity]\n");
    printf(STR);
    printf("\n");

    printf("stream_connect=ssrng_state_merge_1.out:rng_instance_1_1.s_in \n");


    for(int i = 0;i < CUs - 1; i++)
    {
        printf("stream_connect=rng_instance_%d_1.s_out:rng_instance_%d_1.s_in\n", i + 1,i + 2);
        printf("stream_connect=rng_instance_%d_1.o_out:rng_instance_%d_1.o_in\n", i + 2,i + 1);
    }
    printf("\n");
    printf("stream_connect=rng_start_1.output:rng_instance_%d_1.o_in \n", CUs);
    printf("stream_connect=rng_instance_%d_1.s_out:rng_end_1.input\n", CUs);
    printf("stream_connect=rng_instance_1_1.o_out:rng_main_1.rng\n");

    printf(SLR_STR);
    for(int i = 0;i < CUs - 1; i++)
    {
        printf("slr=rng_instance_%d_1:SLR0 \n", i + 1);
    }
    printf("slr=rng_instance_%d_1:SLR0 \n", CUs);

    printf(END_STR);

    return 0;
}

#endif