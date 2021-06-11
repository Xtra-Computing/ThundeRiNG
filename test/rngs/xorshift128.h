#ifndef __XORSHIFT128_H__
#define __XORSHIFT128_H__


#include <array>

using state_t = std::array<uint32_t, 4>;
const size_t STATE_SIZE_EXP  = 128;
const size_t STATE_WORD_SIZE = CHAR_BIT * sizeof(uint32_t);
const size_t STATE_SIZE      = STATE_SIZE_EXP / STATE_WORD_SIZE;

static_assert(STATE_WORD_SIZE * STATE_SIZE == STATE_SIZE_EXP, "Inconsistent state parameters");

void init(state_t &s)
{
    s[0] = 123456789;
    s[1] = 362436069;
    s[2] = 521288629;
    s[3] = 88675123;
}

void step_forward(state_t &s)
{
    uint32_t &x = s[0], &y = s[1], &z = s[2], &w = s[3];

    uint32_t t = x ^ (x << 11);

    x = y; y = z; z = w;
    w = w ^ (w >> 19) ^ (t ^ (t >> 8));
}

void step_backward(state_t &s)
{
    uint32_t &x = s[0], &y = s[1], &z = s[2], &w = s[3];

    uint32_t tt = w ^ (z ^ z >> 19); // (t ^ (t >> 8))

    uint32_t t_3 = tt & 0xFF000000U;
    uint32_t t_2 = (tt & 0x00FF0000U) ^ (t_3 >> 8);
    uint32_t t_1 = (tt & 0x0000FF00U) ^ (t_2 >> 8);
    uint32_t t_0 = (tt & 0x000000FFU) ^ (t_1 >> 8);

    uint32_t t = t_3 | t_2 | t_1 | t_0; // x ^ (x << 11)

    uint32_t x_10_00 = t & 0x000007FFU;
    uint32_t x_21_11 = (t & 0x003FF800U) ^ (x_10_00 << 11);
    uint32_t x_31_22 = (t & 0xFFC00000U) ^ (x_21_11 << 11);

    w = z;
    z = y;
    y = x;
    x = x_31_22 | x_21_11 | x_10_00;
}

uint32_t gen_u32(state_t &s)
{
    step_forward(s);

    return s[3];
}

using tr_t = uint32_t[STATE_SIZE_EXP][STATE_SIZE];

inline uint32_t get_bit(const tr_t A, size_t row, size_t col)
{
    size_t col_ndx = col / 32;
    size_t col_ofs = col % 32;

    return (A[row][col_ndx] >> col_ofs) & 0x00000001u;
}

inline void set_bit(tr_t A, size_t row, size_t col, uint32_t val)
{
    size_t col_ndx = col / 32;
    size_t col_ofs = col % 32;

    uint32_t mask = 1u << col_ofs;

    A[row][col_ndx] = (A[row][col_ndx] & ~mask) | (val << col_ofs);
}

void calculate_bwd_transition_matrix(tr_t invT);

enum class Direction {FWD, BWD};

void prepare_transition(tr_t T_k, uint64_t k, Direction dir);

void do_transition(state_t &s, const tr_t T);

uint32_t T[128][4] = {
    {0x00000000U, 0x00000001U, 0x00000000U, 0x00000000U},
    {0x00000000U, 0x00000002U, 0x00000000U, 0x00000000U},
    {0x00000000U, 0x00000004U, 0x00000000U, 0x00000000U},
    {0x00000000U, 0x00000008U, 0x00000000U, 0x00000000U},
    {0x00000000U, 0x00000010U, 0x00000000U, 0x00000000U},
    {0x00000000U, 0x00000020U, 0x00000000U, 0x00000000U},
    {0x00000000U, 0x00000040U, 0x00000000U, 0x00000000U},
    {0x00000000U, 0x00000080U, 0x00000000U, 0x00000000U},
    {0x00000000U, 0x00000100U, 0x00000000U, 0x00000000U},
    {0x00000000U, 0x00000200U, 0x00000000U, 0x00000000U},
    {0x00000000U, 0x00000400U, 0x00000000U, 0x00000000U},
    {0x00000000U, 0x00000800U, 0x00000000U, 0x00000000U},
    {0x00000000U, 0x00001000U, 0x00000000U, 0x00000000U},
    {0x00000000U, 0x00002000U, 0x00000000U, 0x00000000U},
    {0x00000000U, 0x00004000U, 0x00000000U, 0x00000000U},
    {0x00000000U, 0x00008000U, 0x00000000U, 0x00000000U},
    {0x00000000U, 0x00010000U, 0x00000000U, 0x00000000U},
    {0x00000000U, 0x00020000U, 0x00000000U, 0x00000000U},
    {0x00000000U, 0x00040000U, 0x00000000U, 0x00000000U},
    {0x00000000U, 0x00080000U, 0x00000000U, 0x00000000U},
    {0x00000000U, 0x00100000U, 0x00000000U, 0x00000000U},
    {0x00000000U, 0x00200000U, 0x00000000U, 0x00000000U},
    {0x00000000U, 0x00400000U, 0x00000000U, 0x00000000U},
    {0x00000000U, 0x00800000U, 0x00000000U, 0x00000000U},
    {0x00000000U, 0x01000000U, 0x00000000U, 0x00000000U},
    {0x00000000U, 0x02000000U, 0x00000000U, 0x00000000U},
    {0x00000000U, 0x04000000U, 0x00000000U, 0x00000000U},
    {0x00000000U, 0x08000000U, 0x00000000U, 0x00000000U},
    {0x00000000U, 0x10000000U, 0x00000000U, 0x00000000U},
    {0x00000000U, 0x20000000U, 0x00000000U, 0x00000000U},
    {0x00000000U, 0x40000000U, 0x00000000U, 0x00000000U},
    {0x00000000U, 0x80000000U, 0x00000000U, 0x00000000U},
    {0x00000000U, 0x00000000U, 0x00000001U, 0x00000000U},
    {0x00000000U, 0x00000000U, 0x00000002U, 0x00000000U},
    {0x00000000U, 0x00000000U, 0x00000004U, 0x00000000U},
    {0x00000000U, 0x00000000U, 0x00000008U, 0x00000000U},
    {0x00000000U, 0x00000000U, 0x00000010U, 0x00000000U},
    {0x00000000U, 0x00000000U, 0x00000020U, 0x00000000U},
    {0x00000000U, 0x00000000U, 0x00000040U, 0x00000000U},
    {0x00000000U, 0x00000000U, 0x00000080U, 0x00000000U},
    {0x00000000U, 0x00000000U, 0x00000100U, 0x00000000U},
    {0x00000000U, 0x00000000U, 0x00000200U, 0x00000000U},
    {0x00000000U, 0x00000000U, 0x00000400U, 0x00000000U},
    {0x00000000U, 0x00000000U, 0x00000800U, 0x00000000U},
    {0x00000000U, 0x00000000U, 0x00001000U, 0x00000000U},
    {0x00000000U, 0x00000000U, 0x00002000U, 0x00000000U},
    {0x00000000U, 0x00000000U, 0x00004000U, 0x00000000U},
    {0x00000000U, 0x00000000U, 0x00008000U, 0x00000000U},
    {0x00000000U, 0x00000000U, 0x00010000U, 0x00000000U},
    {0x00000000U, 0x00000000U, 0x00020000U, 0x00000000U},
    {0x00000000U, 0x00000000U, 0x00040000U, 0x00000000U},
    {0x00000000U, 0x00000000U, 0x00080000U, 0x00000000U},
    {0x00000000U, 0x00000000U, 0x00100000U, 0x00000000U},
    {0x00000000U, 0x00000000U, 0x00200000U, 0x00000000U},
    {0x00000000U, 0x00000000U, 0x00400000U, 0x00000000U},
    {0x00000000U, 0x00000000U, 0x00800000U, 0x00000000U},
    {0x00000000U, 0x00000000U, 0x01000000U, 0x00000000U},
    {0x00000000U, 0x00000000U, 0x02000000U, 0x00000000U},
    {0x00000000U, 0x00000000U, 0x04000000U, 0x00000000U},
    {0x00000000U, 0x00000000U, 0x08000000U, 0x00000000U},
    {0x00000000U, 0x00000000U, 0x10000000U, 0x00000000U},
    {0x00000000U, 0x00000000U, 0x20000000U, 0x00000000U},
    {0x00000000U, 0x00000000U, 0x40000000U, 0x00000000U},
    {0x00000000U, 0x00000000U, 0x80000000U, 0x00000000U},
    {0x00000000U, 0x00000000U, 0x00000000U, 0x00000001U},
    {0x00000000U, 0x00000000U, 0x00000000U, 0x00000002U},
    {0x00000000U, 0x00000000U, 0x00000000U, 0x00000004U},
    {0x00000000U, 0x00000000U, 0x00000000U, 0x00000008U},
    {0x00000000U, 0x00000000U, 0x00000000U, 0x00000010U},
    {0x00000000U, 0x00000000U, 0x00000000U, 0x00000020U},
    {0x00000000U, 0x00000000U, 0x00000000U, 0x00000040U},
    {0x00000000U, 0x00000000U, 0x00000000U, 0x00000080U},
    {0x00000000U, 0x00000000U, 0x00000000U, 0x00000100U},
    {0x00000000U, 0x00000000U, 0x00000000U, 0x00000200U},
    {0x00000000U, 0x00000000U, 0x00000000U, 0x00000400U},
    {0x00000000U, 0x00000000U, 0x00000000U, 0x00000800U},
    {0x00000000U, 0x00000000U, 0x00000000U, 0x00001000U},
    {0x00000000U, 0x00000000U, 0x00000000U, 0x00002000U},
    {0x00000000U, 0x00000000U, 0x00000000U, 0x00004000U},
    {0x00000000U, 0x00000000U, 0x00000000U, 0x00008000U},
    {0x00000000U, 0x00000000U, 0x00000000U, 0x00010000U},
    {0x00000000U, 0x00000000U, 0x00000000U, 0x00020000U},
    {0x00000000U, 0x00000000U, 0x00000000U, 0x00040000U},
    {0x00000000U, 0x00000000U, 0x00000000U, 0x00080000U},
    {0x00000000U, 0x00000000U, 0x00000000U, 0x00100000U},
    {0x00000000U, 0x00000000U, 0x00000000U, 0x00200000U},
    {0x00000000U, 0x00000000U, 0x00000000U, 0x00400000U},
    {0x00000000U, 0x00000000U, 0x00000000U, 0x00800000U},
    {0x00000000U, 0x00000000U, 0x00000000U, 0x01000000U},
    {0x00000000U, 0x00000000U, 0x00000000U, 0x02000000U},
    {0x00000000U, 0x00000000U, 0x00000000U, 0x04000000U},
    {0x00000000U, 0x00000000U, 0x00000000U, 0x08000000U},
    {0x00000000U, 0x00000000U, 0x00000000U, 0x10000000U},
    {0x00000000U, 0x00000000U, 0x00000000U, 0x20000000U},
    {0x00000000U, 0x00000000U, 0x00000000U, 0x40000000U},
    {0x00000000U, 0x00000000U, 0x00000000U, 0x80000000U},
    {0x00000101U, 0x00000000U, 0x00000000U, 0x00080001U},
    {0x00000202U, 0x00000000U, 0x00000000U, 0x00100002U},
    {0x00000404U, 0x00000000U, 0x00000000U, 0x00200004U},
    {0x00000809U, 0x00000000U, 0x00000000U, 0x00400008U},
    {0x00001012U, 0x00000000U, 0x00000000U, 0x00800010U},
    {0x00002024U, 0x00000000U, 0x00000000U, 0x01000020U},
    {0x00004048U, 0x00000000U, 0x00000000U, 0x02000040U},
    {0x00008090U, 0x00000000U, 0x00000000U, 0x04000080U},
    {0x00010120U, 0x00000000U, 0x00000000U, 0x08000100U},
    {0x00020240U, 0x00000000U, 0x00000000U, 0x10000200U},
    {0x00040480U, 0x00000000U, 0x00000000U, 0x20000400U},
    {0x00080901U, 0x00000000U, 0x00000000U, 0x40000800U},
    {0x00101202U, 0x00000000U, 0x00000000U, 0x80001000U},
    {0x00202404U, 0x00000000U, 0x00000000U, 0x00002000U},
    {0x00404808U, 0x00000000U, 0x00000000U, 0x00004000U},
    {0x00809010U, 0x00000000U, 0x00000000U, 0x00008000U},
    {0x01012020U, 0x00000000U, 0x00000000U, 0x00010000U},
    {0x02024040U, 0x00000000U, 0x00000000U, 0x00020000U},
    {0x04048080U, 0x00000000U, 0x00000000U, 0x00040000U},
    {0x08090100U, 0x00000000U, 0x00000000U, 0x00080000U},
    {0x10120200U, 0x00000000U, 0x00000000U, 0x00100000U},
    {0x20240400U, 0x00000000U, 0x00000000U, 0x00200000U},
    {0x40480800U, 0x00000000U, 0x00000000U, 0x00400000U},
    {0x80901000U, 0x00000000U, 0x00000000U, 0x00800000U},
    {0x01002000U, 0x00000000U, 0x00000000U, 0x01000000U},
    {0x02004000U, 0x00000000U, 0x00000000U, 0x02000000U},
    {0x04008000U, 0x00000000U, 0x00000000U, 0x04000000U},
    {0x08010000U, 0x00000000U, 0x00000000U, 0x08000000U},
    {0x10020000U, 0x00000000U, 0x00000000U, 0x10000000U},
    {0x20040000U, 0x00000000U, 0x00000000U, 0x20000000U},
    {0x40080000U, 0x00000000U, 0x00000000U, 0x40000000U},
    {0x80100000U, 0x00000000U, 0x00000000U, 0x80000000U}
};


void mat_mul(tr_t C, const tr_t A, const tr_t B)
{
    tr_t T;

    for (size_t i = 0; i < 128; i++)
    {
        for (size_t j = 0; j < 128; j++)
        {
            uint32_t v = 0;

            for (size_t k = 0; k < 128; k++)
                v ^= get_bit(A, i, k) & get_bit(B, k, j);

            set_bit(T, i, j, v);
        }
    }

    for (size_t i = 0; i < 128; i++)
    {
        for (size_t j = 0; j < 4; j++)
        {
            C[i][j] = T[i][j];
        }
    }
}

void mat_pow(tr_t B, const tr_t A, uint64_t n)
{
    tr_t T;

    for (size_t i = 0; i < 128; i++)
    {
        for (size_t j = 0; j < 4; j++)
        {
            T[i][j] = A[i][j];
            B[i][j] = 0;
        }

        set_bit(B, i, i, 1);
    }

    for (uint64_t k = n; k > 0; k /= 2)
    {
        if (k % 2)
        {
            mat_mul(B, T, B);
        }

        mat_mul(T, T, T);
    }
}

void calculate_bwd_transition_matrix(tr_t invT)
{
    tr_t tmpT;

    for (size_t i = 0; i < 128; i++)
    {
        for (size_t j = 0; j < 4; j++)
        {
            tmpT[i][j] = T[i][j];
            invT[i][j] = 0;
        }

        set_bit(invT, i, i, 1);
    }

    for (size_t i = 0; i < 128; i++)
    {
        if (i > 0)
            mat_mul(invT, tmpT, invT);

        mat_mul(tmpT, tmpT, tmpT);
    }
}

void prepare_transition(tr_t T_k, uint64_t k, Direction dir)
{
    switch (dir)
    {
    case Direction::FWD:
        mat_pow(T_k, T, k);
        break;

    case Direction::BWD:
    {
        static bool invT_initialized = false;

        static tr_t invT;

        if (!invT_initialized)
        {
            calculate_bwd_transition_matrix(invT);
            invT_initialized = true;
        }

        mat_pow(T_k, invT, k);
        break;
    }
    }
}

void do_transition(state_t &s, const tr_t T)
{
    state_t t;

    for (size_t i = 0; i < 4; i++)
    {
        t[i] = 0;

        for (size_t j = 0; j < 32; j++)
        {
            uint32_t row_ndx = i * 32 + j;

            uint32_t w = 0;

            for (size_t k = 0; k < 4; k++)
                w ^= T[row_ndx][k] & s[k];

            uint32_t u = w ;

            for (size_t k = 1; k < 32; k++)
                u ^= (w << k);

            t[i] = t[i] >> 1 | (u & 0x80000000U);
        }
    }

    s = t;
}








//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static state_t xor128_s[CO_NUM];
static tr_t T_k;
static int xor128_s_T_init_flag = 0;


unsigned int xor128_s_init(int index)
{
    if (xor128_s_T_init_flag == 0)
    {
        uint64_t k = ((uint64_t)1) << 32;
        prepare_transition(T_k, k, Direction::FWD);
        xor128_s_T_init_flag = 1;
    }
    state_t s_1;

    init(s_1);

    for(int i = 0; i < index * 3; i++)
    {
        do_transition(s_1, T_k);
    }
    xor128_s[index] = s_1;

    return 0;
}



unsigned int mxor128_pcg_rr(void)
{
    static unsigned long long counter = 0;
    static rng_state_t state = RNG_INIT_STATE;
    rng_state_t oldstate = state;
    int current_rng = counter % CO_NUM;
    if (current_rng == 0)
    {
        state_transition_funciton(&state);
    }
    counter++;
    rng_state_t tmp = oldstate; // _rotl(oldstate, current_rng % 63);

    rng_state_t tmp_update = multi_stream_seeked(tmp,  54u + mapping[current_rng]);
    rng_state_t updated_state = tmp_update;
    unsigned int rng = output_function(mxorpcg_last_s[current_rng]);

    uint32_t a =gen_u32(xor128_s[current_rng]);
    mxorpcg_last_s[current_rng] = updated_state;

    return (rng ^ a);
}


unsigned int mxor128_pcg_rr_no_multi(void)
{
    static unsigned long long counter = 0;
    static rng_state_t state = RNG_INIT_STATE;
    rng_state_t oldstate = state;
    int current_rng = counter % CO_NUM;
    if (current_rng == 0)
    {
        state_transition_funciton(&state);
    }
    counter++;
    rng_state_t tmp = oldstate; // _rotl(oldstate, current_rng % 63);

    //rng_state_t tmp_update = multi_stream_seeked(tmp,  54u);
    rng_state_t updated_state = tmp;
    unsigned int rng = output_function(mxorpcg_last_s[current_rng]);

    uint32_t a =gen_u32(xor128_s[current_rng]);
    mxorpcg_last_s[current_rng] = updated_state;

    return (rng ^ a);
}


inline rng32_t  mix_permutation(rng_state_t oldstate, uint32_t a)
{
    rng32_t xorshifted = ((oldstate >> 18u) ^ oldstate) >> 27u;
    rng32_t rot = oldstate >> 27;
    return (xorshifted >> rot) | (xorshifted << ((-rot) & 31));
}


unsigned int mxor128_pcg_rr_mixed(void)
{
    static unsigned long long counter = 0;
    static rng_state_t state = RNG_INIT_STATE;
    rng_state_t oldstate = state;
    int current_rng = counter % CO_NUM;
    if (current_rng == 0)
    {
        state_transition_funciton(&state);
    }
    counter++;
    rng_state_t tmp = oldstate; // _rotl(oldstate, current_rng % 63);

    rng_state_t tmp_update = multi_stream_seeked(tmp,  54u + mapping[current_rng]);
    rng_state_t updated_state = tmp_update;
    unsigned int rng = mix_permutation(mxorpcg_last_s[current_rng],xor128_s[current_rng][1]);

    uint32_t a =gen_u32(xor128_s[current_rng]);
    mxorpcg_last_s[current_rng] = updated_state;

    return (rng ^ a);
}

unsigned int mxor128_pcg_rs(void)
{
    static unsigned long long counter = 0;
    static rng_state_t state = RNG_INIT_STATE;
    rng_state_t oldstate = state;
    int current_rng = counter % CO_NUM;
    if (current_rng == 0)
    {
        state_transition_funciton(&state);
    }
    counter++;
    rng_state_t tmp = oldstate; // _rotl(oldstate, current_rng % 63);

    rng_state_t tmp_update = multi_stream_seeked(tmp,  54u + mapping[current_rng]);
    rng_state_t updated_state = tmp_update;
    unsigned int rng = pcg_output_xsh_rs_64_32(mxorpcg_last_s[current_rng]);

    uint32_t a =gen_u32(xor128_s[current_rng]);
    mxorpcg_last_s[current_rng] = updated_state;

    return (rng ^ a);
}


unsigned int mxor128(void)
{
    static unsigned long long counter = 0;

    int current_rng = counter % CO_NUM;
    counter++;
    uint32_t a =gen_u32(xor128_s[current_rng]);

    return ( a);
}


unsigned int mxor128_index(int i)
{
    uint32_t a =gen_u32(xor128_s[i]);

    return ( a);
}





void xorshift_jump_test(void)
{
    uint64_t k = 10;

    tr_t T_k;

    prepare_transition(T_k, k, Direction::FWD);

    state_t s_1, s_2;

    init(s_1);

    s_2 = s_1;

    do_transition(s_1, T_k);

    for (uint64_t i = 0 ; i < k; i++)
        step_forward(s_2);
    for (int i = 0; i < 4; i++)
    {
        if (s_1[i] != s_2[i])
        {
            printf("error@%d \n", i);
        }
    }
    printf("jump test done \n");
    fflush(stdout);
}


#endif /* __XORSHIFT128_H__ */
