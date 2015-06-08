/*
   Purpose and Methods : Find SC slab "idsc" in data the track points to


   Input   :  isec - sector number
              x0 - ... for this track

   Outputs :  scid[0] = idsc
              scid[1] = ipsc
              scid[2] = ihsc
              iflg = 0  means OK

   Controls:

   Library belongs    : libsda.a

   Calls              : none

   Created   3-MAY-1996  Bogdan Niczyporuk
   C remake by Sergey Boyarinov

   Called by dcdcam, dcfinde, dcfit
*/

#include <stdio.h>
#include "dclib.h"
#include "sdadigi.h"
#include "sdacalib.h"
#include "sdageom.h"
#include "sdakeys.h"

#define ABS(x) ((x) < 0 ? -(x) : (x))

void
dcscid_(int *isec, DCscid *scid, float x0[npl_sc], int *iflg)
{
  dcscid(*isec, scid, x0, iflg);
  return;
}

void
dcscid(int isec, DCscid *scid, float x0[npl_sc], int *iflg)
{
  int i, ip, jp, id;
  static int isc, islab, ndig, adc, max_adc, ipsc, ihsc, idsc;
  static float xx0, dx;
  static int tm = 4094;

  /* Derive from the track hit position the slab "id" from the data */

  scid->slab  = 0;
  scid->plane = 0;
  scid->hit   = 0;
  *iflg= 1;
  isc = 0;
  jp  = 0;
  for(ip=nsc_min; ip<=nsc_max; ip++)
  {
    jp++;
    xx0 = x0[ip-nsc_min];
    if(xx0 > 999.) continue;
    /* Check if hit is outside of 4-planes) */
    if(xx0 > sdageom_.pln[isec-1][nsc_min-1][11])
    {
      ipsc = nsc_min;
      isc  = 1;
      break;
    }
    if(xx0 <= sdageom_.pln[isec-1][nsc_max-1][13])
    {
      ipsc = nsc_max;
      isc  = 48;
      break;
    }
    /* Hit is inside of one plane */
    if(xx0 > sdageom_.pln[isec-1][ip-1][11]
      || xx0 <= sdageom_.pln[isec-1][ip-1][13])
      continue;
    ipsc = ip;
    if(jp < 3)
    {
      dx = (sdageom_.pln[isec-1][ip-1][11] - sdageom_.pln[isec-1][ip-1][13])
          / sdageom_.pln[isec-1][ip-1][17];
      islab = (sdageom_.pln[isec-1][ip-1][11] - xx0) / dx;
      if(jp == 1) isc = islab + 1;
      if(jp == 2) isc = islab + 24;
      break;
    }
    else
    {
      if(xx0 > sdageom_.pln[isec-1][ip-1][15])
      {
        dx = (sdageom_.pln[isec-1][ip-1][11] - sdageom_.pln[isec-1][ip-1][15])
            / sdageom_.pln[isec-1][ip-1][17];
        islab = (sdageom_.pln[isec-1][ip-1][11] - xx0) / dx;
        if(jp == 3) isc  = islab + 35;
        if(jp == 4) isc  = islab + 43;
        break;
      }
      else
      {
        dx = (sdageom_.pln[isec-1][ip-1][15] - sdageom_.pln[isec-1][ip-1][13])
            / sdageom_.pln[isec-1][ip-1][18];
        islab = (sdageom_.pln[isec-1][ip-1][15] - xx0) / dx;
        if(jp == 3) isc  = islab + 40;
        if(jp == 4) isc  = islab + 47;
        break;
      }
    }
  }
  if(isc == 0) return;

  /* Now find match of "isc" with data SC "id" */
  ihsc = 0;
  max_adc = 10;



  ndig = sdadigi_.sc_ndig[isec-1][0];
  if(ndig <= 0) return;
  for(i=1; i<=ndig; i++)
  {
    if(ABS(sdadigi_.sc_digi[isec-1][0][i-1][0] - isc) > 1) continue;
    if(sdadigi_.sc_digi[isec-1][0][i-1][1] < 1 ||
       sdadigi_.sc_digi[isec-1][0][i-1][1] > tm) continue;
    if(sdadigi_.sc_digi[isec-1][0][i-1][3] < 1 ||
       sdadigi_.sc_digi[isec-1][0][i-1][3] > tm) continue;
    adc = sdadigi_.sc_digi[isec-1][0][i-1][2]
        + sdadigi_.sc_digi[isec-1][0][i-1][4];
    if(adc > max_adc)
    {
      max_adc = adc;
      ihsc = i;
    }
  }
  if(ihsc == 0) return;
  idsc = sdadigi_.sc_digi[isec-1][0][ihsc-1][0];



  /* No bad slab checking for data and single track simulation */
  if(sdakeys_.ifsim == 1 && sdakeys_.lanal[9] > 0)
  {
    id = idsc;
    if(sdacalib_.cal_sc[isec-1][id-1][8] > 0. ||
       sdacalib_.cal_sc[isec-1][id-1][9] > 0.) return;
  }

  *iflg = 0;
  scid->slab  = idsc;
  scid->plane = ipsc;
  scid->hit   = ihsc;

  return;
}




