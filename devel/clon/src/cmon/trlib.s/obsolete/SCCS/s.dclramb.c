h02725
s 00000/00000/00000
d R 1.2 01/11/19 19:06:33 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 clas/cmon/trlib.s/obsolete/dclramb.c
e
s 00409/00000/00000
d D 1.1 01/11/19 19:06:32 boiarino 1 0
c date and time created 01/11/19 19:06:32 by boiarino
e
u
U
f e 0
t
T
I 1
/*
   Purpose and Methods : Resolves Left-Right AMBiguities for one track segment in a SuperLayer

   Input:    isec       - sector number
             is         - superlayer number
             psi        - projection track angle w.r. to R (or normal to SL)
             digi[6][5] - hit info {iw,tdc,Td,-/+Dm,SigDm} for segment in a SL:
               digi[la][0] - Wire number "iw"
               digi[la][1] - Raw TDC [counts]
               digi[la][2] - Drift Time Td [ns] to the hit wire# "iw"
               digi[la][3] - Drift Distance (measured) -/+Dm [cm]
               digi[la][4] - Sigma of Drift Distance SigDm [cm]

 sdageom._pln[ip][isec][6]  - Radius of DC arc
 sdageom._pln[ip][isec][11] - Phi angle of the 1-st logical sense wire in BCS
 sdageom._pln[ip][isec][12] - DeltaPhi (between adjacent sense wires in cyl. part)
 sdageom._pln[ip][isec][16] - Last sense wire number in cylindrical part
 sdageom._pln[ip][isec][18] - Radius of a cell (= DeltaPhi*R/2)



   Outputs : chib       - best chi2 from all 4 solutions.
             slopD      - slope difference between track segment & PR fit
             dat0B      - strait line intercept at layer#1

   Controls:


   Library belongs    : libana.a

   Calls              : none

   Created   26-OCT-1990   Bogdan Niczyporuk
   Modified  29-JULY-1992  Billy Leung
   Modified  23-FEB-1996   Bogdan Niczyporuk
   C remake by Sergey Boyarinov

   Called by dcdcam
*/

#include <stdio.h>
#include <math.h>
#include "dclib.h"
#include "sdakeys.h"
#include "sdageom.h"

#define ABS(x) ((x) < 0 ? -(x) : (x))
#define SIGN(x1,x2) ((x2) < 0 ? -ABS(x1) : ABS(x1))

#define NLAY     6
#define MIN_NLAY 3    /* min. number of layers needed */
#define SLcut    0.10


static float
MIN(float a1, float a2, float a3, float a4)
{
  float amin;

  amin = a1;
  if(a2 < amin) amin = a2;
  if(a3 < amin) amin = a3;
  if(a4 < amin) amin = a4;
  return(amin);
}

void
dclramb_(int *isec, int *is, float digi[6][5], float *psi, float *chib,
              float *slopD, float *dat0B)
{
  dclramb(*isec, *is, digi, *psi, chib, slopD, dat0B);
  return;
}

void
dclramb(int isec, int is, float digi[6][5], float psi, float *chib,
              float *slopD, float *dat0B)
{
  int i, j, il,il3, istart, J1,J2,JW, J1old[NLAY], max_miss, nmiss, nw0,
      ilmin, LLL, LLR, LRL, LRR, LRB;
  int miss[NLAY];  /* inefficiency of any cause */
  float xr[NLAY], phw[NLAY],dlw[NLAY], wstg[NLAY], w, Scut;
  float dw1LL, dw2LL, dw1LR, dw2LR, dw1RL, dw2RL, dw1RR, dw2RR;
  float fw0, fw1, fw2, fdet;
  float chiLL, chiLR, chiRL, chiRR;
  float dat0LL, dat0LR, dat0RL, dat0RR;
  float drawLL, drawLR, drawRL, drawRR;
  float dsegLL, dsegLR, dsegRL, dsegRR;
  float predLL, predLR, predRL, predRR;
  float sgnLL, sgnLR, sgnRL, sgnRR;
  float slopLL, slopLR, slopRL, slopRR, slopTF, slopB;
  float cpsi, df, deti, ddist[NLAY], derr[NLAY], deltaR;

  static int NLinSL[6] = {4, 6, 6, 6, 6, 6};

  /* Maximum inefficiency (in terms of #layer) possible */
  max_miss =  NLAY - MIN_NLAY;

  /* Initialize chi-square best and second best to huge values */
  *chib = 1000.0;

  /* When "istart" set to non-zero value will skip re-doing with
     different starting layers (see last IF block) even if bad chi2 results */

  istart = 1;   /* istart can be set to 2 */

  cpsi = cos(psi);
  slopTF = tan(psi);
  Scut = SLcut * sqrt( (float)NLAY / (float)NLinSL[is-1] );

  /* Local coordinate system where "xr" is X-axis along R of SL (or normal
     to SL) and "wstg", Y-axis along the layer (arc) */

  ilmin = (is-1)*NLAY + nst_max;
  il3 = ilmin + 3;
  deltaR = sdageom_.pln[isec-1][il3-1][6] - sdageom_.pln[isec-1][il3-1-1][6];
  for(i=1; i<=NLAY; i++)
  {
    il = ilmin + i;
    w = ABS(digi[i-1][0]);
    if(w > sdageom_.pln[isec-1][il-1][16])
    {
      phw[i-1] = sdageom_.pln[isec-1][il-1][11]
               + (sdageom_.pln[isec-1][il-1][16] - 1.)
               * sdageom_.pln[isec-1][il-1][12];
      dlw[i-1] = (w - sdageom_.pln[isec-1][il-1][16])
               *2.*sdageom_.pln[isec-1][il3-1][18];
    }
    else
    {
      phw[i-1] = sdageom_.pln[isec-1][il-1][11]
               + (w-1.)*sdageom_.pln[isec-1][il-1][12];
      dlw[i-1] = 0.0;
    }
    xr[i-1]    = (float)(i - 1)*deltaR;
    wstg[i-1]  = (phw[i-1] - phw[0])*sdageom_.pln[isec-1][il-1][6]
               + dlw[i-1] - dlw[0];
    ddist[i-1] = ABS(digi[i-1][3]) / cpsi;
    derr[i-1]  = digi[i-1][4] / cpsi;
  }


  /* Remember the old starting layers, initially none */

  for(i=1; i<=NLAY; i++)
  {
    miss[i-1] = 0;
    J1old[i-1] = 0;
  }

a30:

  /* Find first 2 starting layers, total the no. of inefficient layers
     if no. of required layers met, J1 will be the first; J2 the second */

  nmiss = 0;
  J1 = 0;
  J2 = 0;
  for(il=1; il<=NLAY; il++)
  {
    if(digi[il-1][0] <= 0.)   /* -ve no. mean inefficiency */
    {
      nmiss++;
      miss[il-1] = 1;
    }
    else if(J1old[il-1] != 1)
    {
      if(J1 == 0)          /* Test if J1 is picked */
      {
        J1 = il;
      }
      else
      {
        if(J2 == 0)        /* Test if J2 is picked */
        {
          J2 = il;
        }
      }
    }
  }

  /* Too many inefficient layers, leave subroutine */

  if((nmiss > max_miss) || (J1 == 0) || (J2 == 0)) return;

  /* Initialize four starting solutions (first 2 layers) */

  nw0 = 2;
  fw0 = nw0;
  fw1 = xr[J1-1] + xr[J2-1];                    /* Sum of the x's */
  fw2 = xr[J1-1]*xr[J1-1] + xr[J2-1]*xr[J2-1];  /* Sum of the x squares */

  dw1LL = -ddist[J1-1]+wstg[J1-1]            -ddist[J2-1]+wstg[J2-1];
  dw2LL =(-ddist[J1-1]+wstg[J1-1])*xr[J1-1]+(-ddist[J2-1]+wstg[J2-1])*xr[J2-1];
  dw1LR = -ddist[J1-1]+wstg[J1-1]            +ddist[J2-1]+wstg[J2-1];
  dw2LR =(-ddist[J1-1]+wstg[J1-1])*xr[J1-1]+( ddist[J2-1]+wstg[J2-1])*xr[J2-1];
  dw1RL =  ddist[J1-1]+wstg[J1-1]            -ddist[J2-1]+wstg[J2-1];
  dw2RL =( ddist[J1-1]+wstg[J1-1])*xr[J1-1]+(-ddist[J2-1]+wstg[J2-1])*xr[J2-1];
  dw1RR =  ddist[J1-1]+wstg[J1-1]            +ddist[J2-1]+wstg[J2-1];
  dw2RR =( ddist[J1-1]+wstg[J1-1])*xr[J1-1]+( ddist[J2-1]+wstg[J2-1])*xr[J2-1];

  /* We have two hits to get started, solve lin. regress */

  fdet = fw0*fw2-fw1*fw1;
  if(fdet <= 0.0) return;
  deti = 1. / fdet;
  dat0LL = ( dw1LL*fw2 - dw2LL*fw1)*deti;
  slopLL = (-dw1LL*fw1 + dw2LL*fw0)*deti;
  dat0LR = ( dw1LR*fw2 - dw2LR*fw1)*deti;
  slopLR = (-dw1LR*fw1 + dw2LR*fw0)*deti;
  dat0RL = ( dw1RL*fw2 - dw2RL*fw1)*deti;
  slopRL = (-dw1RL*fw1 + dw2RL*fw0)*deti;
  dat0RR = ( dw1RR*fw2 - dw2RR*fw1)*deti;
  slopRR = (-dw1RR*fw1 + dw2RR*fw0)*deti;

  /* Loop over remaining wires, picking up hits for segment */

  for(JW=1; JW<=NLAY; JW++)
  {
    if(digi[JW-1][0] <= 0.0) continue;
    if(JW == J1 || JW == J2) continue;

    /* Determine predicted distance, using dat0 and slop */

    predLL = dat0LL + slopLL*xr[JW-1] - wstg[JW-1];
    sgnLL  = SIGN(1.,predLL);
    predLR = dat0LR + slopLR*xr[JW-1] - wstg[JW-1];
    sgnLR  = SIGN(1.,predLR);
    predRL = dat0RL + slopRL*xr[JW-1] - wstg[JW-1];
    sgnRL  = SIGN(1.,predRL);
    predRR = dat0RR + slopRR*xr[JW-1] - wstg[JW-1];
    sgnRR  = SIGN(1.,predRR);

    nw0++;
    fw0  = nw0;
    fw1  = fw1 + xr[JW-1];            /* Sum of the x's */
    fw2  = fw2 + xr[JW-1]*xr[JW-1];   /* Sum of the x squares */

    dsegLL = sgnLL*ddist[JW-1] + wstg[JW-1];
    dw1LL  = dw1LL + dsegLL;
    dw2LL  = dw2LL + dsegLL*xr[JW-1];
    dsegLR = sgnLR*ddist[JW-1] + wstg[JW-1];
    dw1LR  = dw1LR + dsegLR;
    dw2LR  = dw2LR + dsegLR*xr[JW-1];
    dsegRL = sgnRL*ddist[JW-1] + wstg[JW-1];
    dw1RL  = dw1RL + dsegRL;
    dw2RL  = dw2RL + dsegRL*xr[JW-1];
    dsegRR = sgnRR*ddist[JW-1] + wstg[JW-1];
    dw1RR  = dw1RR + dsegRR;
    dw2RR  = dw2RR + dsegRR*xr[JW-1];

    /* If we have more than one hit in seg, solve lin. regress. */

    fdet = fw0*fw2 - fw1*fw1;
    if(fdet <= 0.) continue;       /* avoid divding by delta=0 */
    deti = 1. / fdet;
    dat0LL = ( dw1LL*fw2 - dw2LL*fw1)*deti;
    slopLL = (-dw1LL*fw1 + dw2LL*fw0)*deti;
    dat0LR = ( dw1LR*fw2 - dw2LR*fw1)*deti;
    slopLR = (-dw1LR*fw1 + dw2LR*fw0)*deti;
    dat0RL = ( dw1RL*fw2 - dw2RL*fw1)*deti;
    slopRL = (-dw1RL*fw1 + dw2RL*fw0)*deti;
    dat0RR = ( dw1RR*fw2 - dw2RR*fw1)*deti;
    slopRR = (-dw1RR*fw1 + dw2RR*fw0)*deti;
  }


  /* Check if min. required layers is met */

  if(nw0 < MIN_NLAY) return;

  /* Get left/right ambig resol for the four solutions */

  chiLL = 0.;
  chiLR = 0.;
  chiRL = 0.;
  chiRR = 0.;
  LLL  = 0;
  LLR  = 0;
  LRL  = 0;
  LRR  = 0;

  /* "Lxx" has its first 6 bits set to one if the xx solution ends
      up on the RIGHT-hand side of the hit wire for each of the 6 layers */

  for(j=0; j<NLAY; j++)
  {
    if(!miss[j])
    {
      int itmp;
      float tmp;

      predLL = dat0LL + slopLL*xr[j] - wstg[j];
      if(predLL > 0.) {itmp=1; LLL = LLL | (itmp << j);}
      drawLL = SIGN(ddist[j],predLL);
      tmp = (drawLL - predLL)/derr[j];
      chiLL += tmp*tmp;

      predLR = dat0LR + slopLR*xr[j] - wstg[j];
      if(predLR > 0.) {itmp=1; LLR = LLR | (itmp << j);}
      drawLR = SIGN(ddist[j],predLR);
      tmp = (drawLR - predLR)/derr[j];
      chiLR += tmp*tmp;

      predRL = dat0RL + slopRL*xr[j] - wstg[j];
      if(predRL > 0.) {itmp=1; LRL = LRL | (itmp << j);}
      drawRL = SIGN(ddist[j],predRL);
      tmp = (drawRL - predRL)/derr[j];
      chiRL += tmp*tmp;

      predRR = dat0RR + slopRR*xr[j] - wstg[j];
      if(predRR > 0.) {itmp=1; LRR = LRR | (itmp << j);}
      drawRR = SIGN(ddist[j],predRR);
      tmp = (drawRR - predRR)/derr[j];
      chiRR += tmp*tmp;
    }
  }

  /* Normalize chi-square's by dividing by number of degrees of
   freedom for all four solutions, and correcting for the slope */

  df = nw0 - 2;
  chiLL = chiLL / df;
  chiLR = chiLR / df;
  chiRL = chiRL / df;
  chiRR = chiRR / df;

  /* Record the slope difference with the best solution */

  if(ABS(slopLL-slopTF) > Scut) chiLL += 100000.;
  if(ABS(slopLR-slopTF) > Scut) chiLR += 100000.;
  if(ABS(slopRL-slopTF) > Scut) chiRL += 100000.;
  if(ABS(slopRR-slopTF) > Scut) chiRR += 100000.;

  /* Pick the solution with the least chi-square */

  *chib = MIN(chiLL,chiLR,chiRL,chiRR);

  /* Save the set bits for the best solution and then pick the
     solution with the second best chi-square */

  if(ABS(*chib - chiLL) < 0.00001)
  {
    LRB   = LLL;
    slopB = slopLL;
    *dat0B = dat0LL * cpsi;
  }
  else if(ABS(*chib - chiLR) < 0.00001)
  {
    LRB   = LLR;
    slopB = slopLR;
    *dat0B = dat0LR * cpsi;
  }
  else if(ABS(*chib - chiRL) < 0.00001)
  {
    LRB   = LRL;
    slopB = slopRL;
    *dat0B = dat0RL * cpsi;
  }
  else if(ABS(*chib - chiRR) < 0.00001)
  {
    LRB   = LRR;
    slopB = slopRR;
    *dat0B = dat0RR * cpsi;
  }
  else
  {
    ;
  }
  *slopD = slopB - slopTF;

  /* Mark the sign to indicate LR, "-" => Left of the wire */

  for(i=1; i<=NLAY; i++)
  {
    if(!miss[i-1])
    {
      int tmp;
      tmp = 1;

      if( LRB & (tmp << (i-1)) )
      {
        digi[i-1][3] =  ABS(digi[i-1][3]);
      }
      else
      {
        digi[i-1][3] = -ABS(digi[i-1][3]);
      }
    }
  }

  /* if chi-square >7.0 (CL < 1%), re-do LR_amb with 2 diff. starting layers */

  if(*chib <= sdakeys_.zcut[5]) return;

  /* The layers J1 & J2 failed, get started with other two layers, */
  if(istart > 0)
  {
    J1old[J1-1] = 1;
    J1old[J2-1] = 1;
    /* Reset "istart" only to re-do once with another 2 starting layers. */
    istart--;
    for(il=1; il<=NLAY; il++) miss[il-1] = 0;
    goto a30;
  }

  return;
}

E 1
