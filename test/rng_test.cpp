// Adapted from TestU01 manual, Figure 2.2, Figure 2.4

#include <random>
#include <string>
#include <cstdio>
#include <cstdint>
#include <cstddef>


extern "C" {
#include "TestU01.h"
}



int mapping[CO_NUM];


#include "rng_header.h"
#include "rng_list.h"

const char* progname;

void usage()
{
    printf("%s: [-v] [-r] [seeds...]\n", progname);
    exit(1);
}



int main (int argc, char** argv)
{
    for (int i = 0; i < CO_NUM; i++)
    {
        mapping[i] = pcg_golden();
    }
    //printf("init start\n");
    //xorshift_jump_test();
    //for (unsigned int i = 0; i < ARRAY_SIZE(local_method); i++)
    {
        unsigned int i = SELECTED_METHOD;
        if (local_method[i].init != NULL)
        {
            for (int j = 0 ; j < CO_NUM; j++)
            {
                local_method[i].init(j);
            }

        }
    }
    //printf("init done\n");
    fflush(stdout);


    progname = argv[0];
    // Create TestU01 PRNG object for our generator
    unif01_Gen* gen = unif01_CreateExternGenBits((char *)local_method[SELECTED_METHOD].name, local_method[SELECTED_METHOD].func);

    // Config options for TestU01
    swrite_Basic = FALSE;  // Turn off TestU01 verbosity by default
    // reenable by -v option.

    // Config options for tests
    bool testSmallCrush = false;
    bool testCrush = false;
    bool testBigCrush = false;
    bool testLinComp = false;

    // Handle command-line option switches
    while (1) {
        --argc; ++argv;
        if ((argc == 0) || (argv[0][0] != '-'))
            break;
        if ((argv[0][1] == '\0') || (argv[0][2] != '\0'))
            usage();
        switch (argv[0][1]) {
        case 's':
            testSmallCrush = true;
            break;
        case 'm':
            testCrush = true;
            break;
        case 'b':
            testBigCrush = true;
            break;
        case 'l':
            testLinComp = true;
            break;
        case 'v':
            swrite_Basic = TRUE;
            break;
        default:
            usage();
        }
    }
    //printf("rng test of %s, with co_num %d\n", local_method[SELECTED_METHOD].name, CO_NUM);

#ifndef PRT
    // Determine a default test if need be

    if (!(testSmallCrush || testCrush || testBigCrush || testLinComp)) {
        testCrush = true;
    }

    printf("rng test of %s, with co_num %d\n", local_method[SELECTED_METHOD].name, CO_NUM);


    if (testSmallCrush) {
        bbattery_SmallCrush(gen);
        fflush(stdout);
    }
    if (testCrush) {
        bbattery_Crush(gen);
        fflush(stdout);
    }
    if (testBigCrush) {
        bbattery_BigCrush(gen);
        fflush(stdout);
    }
    if (testLinComp) {
        scomp_Res* res = scomp_CreateRes();
        swrite_Basic = TRUE;
        for (int size : {250, 500, 1000, 5000, 25000, 50000, 75000})
            scomp_LinearComp(gen, res, 1, size, 0, 1);
        scomp_DeleteRes(res);
        fflush(stdout);
    }

    // Clean up.

    unif01_DeleteExternGenBits(gen);
#else
    freopen(NULL, "wb", stdout);  // Only necessary on Windows, but harmless.
    while (1) {
        uint32_t value = local_method[SELECTED_METHOD].func();
        fwrite((void*) &value, sizeof(value), 1, stdout);
    }
#endif
    return 0;
}
