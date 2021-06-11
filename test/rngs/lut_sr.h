#ifndef __LUT_SR_H__
#define __LUT_SR_H__



#include <stdio.h>
#include <set>
#include <vector>
#include <stdlib.h>
#include <stdint.h>
#include <iostream>
using namespace std;

#include "rng.hpp"

rng g(19937, 624, 5, 32, 0x2fffb);

std::vector<int> g_state;

#define START_POS       (0)
unsigned int lut_sr_init(int index)
{
    std::vector<int> state(g.n, 1);

    for (int i = 0; i < g.n; i++) {
        int x = rand() % 2;
        g.Step(state, 1, x);
    }
    //printf("init done\n"); fflush(stdout);
    g_state = state;
    return 0;
}


unsigned int lut_sr_srng(void)
{
    unsigned int rng_value = 0;
    std::pair<std::vector<int>, int> out;
    out = g.Step(g_state, 0, 0);
    for (int j = START_POS; j < START_POS + 32; j++) {
        rng_value = ((rng_value << 1 ) | (out.first[j] & 0x01));
    }
    return rng_value;
}

std::pair<std::vector<int>, int> out;

unsigned int lut_sr_mrng(void)
{

    unsigned int rng_value = 0;
    static uint64_t counter = 0;
    int current_rng = counter % CO_NUM;
    if (current_rng == 0)
    {
        out = g.Step(g_state, 0, 0);
    }
    counter++;

    for (int j = (current_rng * 32 + START_POS); j < (current_rng * 32 + START_POS + 32); j++) {
        rng_value = ((rng_value << 1 ) | (out.first[j] & 0x01));
    }
    return rng_value;
}

#endif /* __LUT_SR_H__ */
