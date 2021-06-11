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

#define SEED 7777777

#define S0L     10.0f
#define S0H     50.0f
#define XL      10.0f
#define XH      50.0f
#define TL      0.2f
#define TH      2.0f
#define RISK_FREE  0.05f
#define VOLATILITY 0.05f

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

void MonteCarloEuroOpt( int nopt, int nsamp,
    double r, double sig, double s0[], double x[], 
    double t[], double vcall[], double vput[], double vcallerr[], double vputerr[] );

void euroInitData( int nopt, double* *s0, double* *x, double* *t,
                   double* *vcall, double* *vput, double* *vcallerr, double* *vputerr,
                   double* *vcall_ref, double* *vput_ref );

void euroFreeData( double *s0, double *x, double *t, 
                   double *vcall, double *vput, double *vcallerr, double *vputerr,
                   double *vcall_ref, double *vput_ref );

void BlackScholesFormulaRef( int nopt, 
    double r, double sig, double s0[], double x[], 
    double t[], double vcall[], double vput[] );

#ifdef __cplusplus
}
#endif /* __cplusplus */
