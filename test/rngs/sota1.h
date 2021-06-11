/*  Written in 2018 by David Blackman and Sebastiano Vigna (vigna@acm.org)

To the extent possible under law, the author has dedicated all copyright
and related and neighboring rights to this software to the public domain
worldwide. This software is distributed without any warranty.

See <http://creativecommons.org/publicdomain/zero/1.0/>. */

#include <stdint.h>

/* This is xoroshiro128** 1.0, one of our all-purpose, rock-solid,
   small-state generators. It is extremely (sub-ns) fast and it passes all
   tests we are aware of, but its state space is large enough only for
   mild parallelism.

   For generating just floating-point numbers, xoroshiro128+ is even
   faster (but it has a very mild bias, see notes in the comments).

   The state must be seeded so that it is not everywhere zero. If you have
   a 64-bit seed, we suggest to seed a splitmix64 generator and use its
   output to fill s. */


static inline uint64_t rotl(const uint64_t x, int k) {
    return (x << k) | (x >> (64 - k));
}


static uint64_t s[2];

uint64_t sota1_next(uint64_t *s) {
    const uint64_t s0 = s[0];
    uint64_t s1 = s[1];
    const uint64_t result = rotl(s0 * 5, 7) * 9;

    s1 ^= s0;
    s[0] = rotl(s0, 24) ^ s1 ^ (s1 << 16); // a, b
    s[1] = rotl(s1, 37); // c

    return result;
}

static void sota1_init(uint64_t  *s)
{
    s[0] = (uint64_t)123456789 * 362436069;
    s[1] = (uint64_t)521288629 * 88675123;
}





uint64_t xor128p_sota_s[CO_NUM][2];





/* This is the jump function for the generator. It is equivalent
   to 2^64 calls to next(); it can be used to generate 2^64
   non-overlapping subsequences for parallel computations. */

void sota1_jump(uint64_t *s) {
    //printf("init\n");
    static const uint64_t JUMP[] = { 0xdf900294d8f554a5, 0x170865df4b3201fc };

    uint64_t s0 = 0;
    uint64_t s1 = 0;
    for (int i = 0; i < sizeof JUMP / sizeof * JUMP; i++)
        for (int b = 0; b < 64; b++) {
            if (JUMP[i] & UINT64_C(1) << b) {
                s0 ^= s[0];
                s1 ^= s[1];
            }
            sota1_next(s);
        }

    s[0] = s0;
    s[1] = s1;
}

unsigned int sota1_s_init(int index)
{
    //printf("index %d\n", index);
    sota1_init(xor128p_sota_s[index]);

    for (int i = 0; i < index; i++)
    {
        sota1_jump(xor128p_sota_s[index]);
    }
    return 0;
}


unsigned int sota1(void)
{
    static unsigned long long counter = 0;

    int current_rng = counter % CO_NUM;
    counter++;
    uint32_t a = sota1_next(xor128p_sota_s[current_rng]);

    return ( a);
}
