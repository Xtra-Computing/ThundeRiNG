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
#include <math.h>
#include "euro_opt.h"

/*
  Black-Scholes formula to verify Monte Carlo computation
  See Black-Scholes formula example on additional optimization techniques
*/

void BlackScholesFormulaRef( int nopt, 
    double r, double sig, double s0[], double x[], 
    double t[], double vcall[], double vput[] )
{
    int i;
    double a, b, c, y, z, e, d1, d2, w1, w2;

    for ( i = 0; i < nopt; i++ )
    {
        a = log( s0[i] / x[i] );
        b = t[i] * r;
        z = t[i]*sig*sig;
    
        c = 0.5 * z;
        e = exp( -b );
        y = 1.0/sqrt( z );
                         
        w1 = ( a + b + c ) * y;
        w2 = ( a + b - c ) * y;
        
        d1 = 0.5 + 0.5 * erf( w1 / sqrt(2.0) );
        d2 = 0.5 + 0.5 * erf( w2 / sqrt(2.0) );
        
        vcall[i] = s0[i]*d1 - x[i]*e*d2;
        vput[i]  = vcall[i] - s0[i] + x[i]*e;
    }
}
