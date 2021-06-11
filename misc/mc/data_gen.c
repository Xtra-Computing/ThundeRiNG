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

#include <stdlib.h>
#include "euro_opt.h"
#include <stdio.h>

static double RandFloat( double a, double b )
{
    return rand()/(double)RAND_MAX*(b-a) + a;
}

void euroInitData( int nopt, double* *s0, double* *x, double* *t,
                   double* *vcall, double* *vput, double* *vcallerr, double* *vputerr,
                   double* *vcall_ref, double* *vput_ref )
{
    int i;

    /* Allocate memory */
    *s0  = (double*)malloc( nopt * sizeof(double) );
    *x   = (double*)malloc( nopt * sizeof(double) );
    *t   = (double*)malloc( nopt * sizeof(double) );
    *vcall = (double*)malloc( nopt * sizeof(double) );
    *vput  = (double*)malloc( nopt * sizeof(double) );
    *vcallerr = (double*)malloc( nopt * sizeof(double) );
    *vputerr  = (double*)malloc( nopt * sizeof(double) );
    *vcall_ref = (double*)malloc( nopt * sizeof(double) );
    *vput_ref  = (double*)malloc( nopt * sizeof(double) );

    if ( (*s0 == NULL) || (*x == NULL) || (*t == NULL) ||
         (*vcall == NULL) || (*vput == NULL) ||
         (*vcallerr == NULL) || (*vputerr == NULL) ||
         (*vcall_ref == NULL) || (*vput_ref == NULL) )
    {
        printf("Error in memory allocation\n");
        exit(-1);
    }

    srand( SEED );

    /* Initialize data */
    for ( i=0; i<nopt; i++ )
    {
        (*s0)[i] = RandFloat( S0L, S0H );
        (*x)[i]  = RandFloat( XL, XH );
        (*t)[i]  = RandFloat( TL, TH );

        (*vcall)[i]=0.0;
        (*vput )[i]=0.0;
        (*vcallerr)[i]=0.0;
        (*vputerr )[i]=0.0;
    }
}

void euroFreeData( double *s0, double *x, double *t,
                   double *vcall, double *vput, double *vcallerr, double *vputerr,
                   double *vcall_ref, double *vput_ref )
{
    /* Free memory */
    free(s0);
    free(x);
    free(t);
    free(vcall);
    free(vput);
    free(vcallerr);
    free(vputerr);
    free(vcall_ref);
    free(vput_ref);
}
