rng_method_t local_method[] =
{
    {   //0
        .name = "Xorshift 32",
        .func = xorshift,
        .init = NULL,
    },
    {   //1
        .name = "single_pcg",
        .func = single_pcg,
        .init = NULL,
    },
    {   //2
        .name = "single_ss_pcg",
        .func = single_ss_pcg,
        .init = NULL,
    },
    {   //3
        .name = "mxorwowpcg",
        .func = mxorwowpcg,
        .init = xorwow_init,
    },
    {   //4
        .name = "mopcg",
        .func = mopcg,
        .init = original_lcg_init,
    },
    {   //5
        .name = "mxor128p_pcg_rr",
        .func = mxor128p_pcg_rr,
        .init = xor128p_s_init,
    },
    {   //6
        .name = "mxor32pcg",
        .func = mxor32pcg,
        .init = xor32_s_init,
    },
    {   //7
        .name = "lut-sr",
        .func = lut_sr_srng,
        .init = lut_sr_init,
    },
    {   //8
        .name = "lut-sr-mrng",
        .func = lut_sr_mrng,
        .init = lut_sr_init,
    },
    {   //9
        .name = "molcg",
        .func = molcg,
        .init = original_lcg_init,
    },
    {   //10
        .name = "mxor32lcgs",
        .func = mxor32lcgs,
        .init = xor32_s_init,
    },
    {   //11
        .name = "mxor32pcg_rs",
        .func = mxor32pcg_rs,
        .init = xor32_s_init,
    },
    {   //12
        .name = "mxor128_pcg_rr",
        .func = mxor128_pcg_rr,
        .init = xor128_s_init,
    },
    {   //13
        .name = "mxor128_pcg_rs",
        .func = mxor128_pcg_rs,
        .init = xor128_s_init,
    },
    {   //14
        .name = "mxor128",
        .func = mxor128,
        .init = xor128_s_init,
    },
    {   //15
        .name = "mxor128_pcg_rr_mixed",
        .func = mxor128_pcg_rr_mixed,
        .init = xor128_s_init,
    },
    {
        //16
        .name = "sota1_s_init",
        .func = sota1,
        .init = sota1_s_init,
    },
    {
        //17
        .name = "mxor128_pcg_rr_no_multi",
        .func = mxor128_pcg_rr_no_multi,
        .init = xor128_s_init,
    }
};