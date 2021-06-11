/*******************************************************************************
!  Copyright(C) 2015 Intel Corporation. All Rights Reserved.
!
!  The source code, information  and  material ("Material") contained herein is
!  owned  by Intel Corporation or its suppliers or licensors, and title to such
!  Material remains  with Intel Corporation  or its suppliers or licensors. The
!  Material  contains proprietary information  of  Intel or  its  suppliers and
!  licensors. The  Material is protected by worldwide copyright laws and treaty
!  provisions. No  part  of  the  Material  may  be  used,  copied, reproduced,
!  modified, published, uploaded, posted, transmitted, distributed or disclosed
!  in any way  without Intel's  prior  express written  permission. No  license
!  under  any patent, copyright  or  other intellectual property rights  in the
!  Material  is  granted  to  or  conferred  upon  you,  either  expressly,  by
!  implication, inducement,  estoppel or  otherwise.  Any  license  under  such
!  intellectual  property  rights must  be express  and  approved  by  Intel in
!  writing.
!
!  *Third Party trademarks are the property of their respective owners.
!
!  Unless otherwise  agreed  by Intel  in writing, you may not remove  or alter
!  this  notice or  any other notice embedded  in Materials by Intel or Intel's
!  suppliers or licensors in any way.
!
!*******************************************************************************
!  Content:
!      Monte Carlo European Options Pricing Example
!******************************************************************************/

#include <stdio.h>
#include <mkl.h>
#include <omp.h>
#include <math.h>
#include "euro_opt.h"

#define NOPT    2048
#define NSAMP 262144
#define NBUF    1024
#define MAX( a, b ) ((a) > (b)) ? (a) : (b)

static void MonteCarloEuroOptKernel( VSLStreamStatePtr stream, int nsamp,
    double r, double sig, double s0, double x,
    double t, double *vcall, double *vput, double* vcallerr, double* vputerr,
    double * rn )
{
    double a, nu, sc, sp, sc2, sp2, st, vc, vp;
    int i, j;
    int nblocks;

    const int nbuf = NBUF;
    nblocks = nsamp/nbuf;

    a  = (r - sig*sig*0.5)*t;
    nu = sig * sqrt(t);

    sc  = sp  = 0.0;
    sc2 = sp2 = 0.0;

    /* Blocked computations */

    for ( i = 0; i < nblocks; i++ )
    {
        int errcode;
        /* Make sure that tail is correctly computed */
        int block_size = (i != nblocks-1)?(nbuf):(nsamp - (nblocks-1)*nbuf);

        /* Generating block of random numbers */

        errcode = vdRngLognormal( VSL_RNG_METHOD_LOGNORMAL_ICDF, stream, block_size, rn, a, nu, 0.0, 1.0 );
        if( errcode != VSL_ERROR_OK)
        {
            printf("Error in vdRngLognormal\n");
            exit(-1);
        }

        /* Reduction */

      #pragma vector aligned
      #pragma simd
        for ( j=0; j<block_size; j++ )
        {
            st = s0*rn[j];

            vc = MAX( st-x, 0.0 );
            vp = MAX( x-st, 0.0 );

            sc += vc;
            sp += vp;

            sc2 += vc*vc;
            sp2 += vp*vp;
        }
    }

    *vcall = sc/nsamp * exp(-r*t);
    *vput  = sp/nsamp * exp(-r*t);

    *vcallerr = sqrt( (sc2 - sc*sc/nsamp) / (nsamp-1) / nsamp );
    *vputerr  = sqrt( (sp2 - sp*sp/nsamp) / (nsamp-1) / nsamp );
}


void MonteCarloEuroOpt( int nopt, int nsamp,
    double r, double sig, double s0[], double x[],
    double t[], double vcall[], double vput[], double vcallerr[], double vputerr[] )
{
  #pragma omp parallel
    {
        int i, j;
        __declspec(align(64)) double rn[NBUF*2];

        VSLStreamStatePtr stream;

        j = omp_get_thread_num();

        /* Initialize RNG */
        vslNewStream( &stream, VSL_BRNG_MT2203 + j, SEED );

        /* Price options */
      #pragma omp for
        for(i=0;i<nopt;i++)
        {
            MonteCarloEuroOptKernel( stream, nsamp, r, sig, s0[i], x[i], t[i], &(vcall[i]), &(vput[i]), &(vcallerr[i]), &(vputerr[i]),  rn );
        }

        /* Deinitialize RNG */
        vslDeleteStream( &stream );
    }
}

int main()
{


    int i;

    double *s0, *x, *t, *vcall, *vput, *vcallerr, *vputerr;
    double *vcall_ref, *vput_ref;

    double put_abs_err = 0.0;
    double call_abs_err = 0.0;
    double abs_err = 0.0;

    /* Allocate arrays and prepare input data */

    euroInitData( NOPT, &s0, &x, &t, &vcall, &vput, &vcall_ref, &vput_ref, &vcallerr, &vputerr );

    /* Main computation */

    MonteCarloEuroOpt( NOPT, NSAMP, RISK_FREE, VOLATILITY, s0, x, t, vcall, vput, vcallerr, vputerr );

    /* Calculate reference values */

    BlackScholesFormulaRef( NOPT, RISK_FREE, VOLATILITY, s0, x, t, vcall_ref, vput_ref );

    printf("NOPT  = %i\n", NOPT );
    printf("NSAMP = %i\n", NSAMP);

    /* Compare Monte Carlo with reference via worst absolute error */

    for(i=0;i<NOPT;i++)
    {
        abs_err = (vput_ref[i]-vput[i]);
        if(abs_err < 0) abs_err=-abs_err;

        if(put_abs_err < abs_err) put_abs_err = abs_err;

        abs_err = (vcall_ref[i]-vcall[i]);
        if(abs_err < 0) abs_err=-abs_err;

        if(call_abs_err < abs_err) call_abs_err = abs_err;
    }

    printf(" put_abs_err = %.3le\n", put_abs_err );
    printf("call_abs_err = %.3le\n", call_abs_err );

    /* Free used arrays */

    euroFreeData( s0, x, t, vcall, vput, vcall_ref, vput_ref, vcallerr, vputerr );

    return 0;
}
