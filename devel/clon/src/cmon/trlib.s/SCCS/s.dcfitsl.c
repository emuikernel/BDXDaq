h07404
s 00000/00000/00000
d R 1.2 01/11/19 19:05:59 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 clas/cmon/trlib.s/dcfitsl.c
e
s 00373/00000/00000
d D 1.1 01/11/19 19:05:58 boiarino 1 0
c date and time created 01/11/19 19:05:58 by boiarino
e
u
U
f e 0
t
T
I 1
/*
   Purpose and Methods : fit one track segment in a SuperLayer
     (replacement for dclramb.c)

   Input:    isect      - sector number (1-6)
             isl        - superlayer number (1-6)
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



   Outputs : chibest     - best chi2
             slopDbest   - slope difference between track segment & PR fit
             dat0B       - strait line intercept at layer#1
             digi[la][3] - SIGNED Drift Distance (measured) -/+Dm [cm]
             !!! bad hits will be marked as negative wire number !!!
             digib[6][5] - chousen hits (one or none per layer)

   Controls:


   Library belongs    : libana.a

   Calls              : none

   Created    2-SEP-2001   Sergey Boyarinov

   Called by dcdcam
*/

#include <stdio.h>
#include <math.h>
#include "dclib.h"
#include "sdageom.h"
#include "sdakeys.h"
SDAkeys sdakeys_;

#define ABS(x) ((x) < 0 ? -(x) : (x))
#define SIGN(x1,x2) ((x2) < 0 ? -ABS(x1) : ABS(x1))
#define NLAY 6

#define dpsi  0.04
#define NBIN  100
#define resSL 0.1 /* space resolution in SL (cm) */
#define xcell (resSL*3.)

#define NBIN2 (NBIN/2)


void
dcfitsl_(int *isect, int *isl, int ndigi[NLAY], float digi[NLAY][nhlmx][5],
         float *psi, float *chibest, float *slopDbest, float *dat0B,
         float digib[NLAY][5])
{
  dcfitsl(*isect, *isl, ndigi, digi, *psi, chibest, slopDbest, dat0B, digib);
  return;
}

void
dcfitsl(int isect, int isl, int ndigi[NLAY], float digi[NLAY][nhlmx][5],
        float psi, float *chibest, float *slopDbest, float *dat0B,
        float digib[NLAY][5])
{
  int isec, is, la, i, j, k, il, il3, nw0, ilmin;
  float xr[NLAY], phw[NLAY], dlw[NLAY], wstg[NLAY], slopTF, w;
  float cpsi, df, deti, ddist[NLAY], derr[NLAY], deltaR;
  int itmp, jtmp, lamin, imax, jmax;
  float tmpi, tmpj, tmp, tmp1, chib, slopD, dat0, y[6], a, b, e;
  static int NLinSL[6]  = {4, 6, 6, 6, 6, 6};
  int ibin, jbin, ibin1, ibin2, iarray[NBIN], jarray[NBIN];
  int kdigi[NLAY], jdigi[NLAY], npeak;
  float yr[NLAY][nhlmx][2], peak[NBIN];
  static float wfit[6]  = {10., 10., 10., 10., 10., 10.};

  isec = isect - 1;
  is = isl - 1;

  /* Initialize chi-square to huge values */
  *chibest = 1.0e+8;

  cpsi = cos(psi);
  slopTF = tan(psi);

  /* Local coordinate system where "xr" is X-axis along R of SL (or normal
     to SL) and "wstg", Y-axis along the layer (arc) */

  ilmin = is*NLAY + nst_max;
  il3 = ilmin + 3;
  deltaR = sdageom_.pln[isec][il3-1][6] - sdageom_.pln[isec][il3-1-1][6];

  phw[0] = -1000.;
  for(la=0; la<NLAY; la++)
  {
    il = ilmin + la;
    xr[la] = (float)la*deltaR;
    for(k=0; k<ndigi[la]; k++)
    {
      w = digi[la][k][0];
/*
printf("! is[%1d] la[%1d] k[%1d] %f %f %f %f %f\n",is,la,k,
digi[la][k][0],digi[la][k][1],digi[la][k][2],digi[la][k][3],digi[la][k][4]);
*/
      if(w > sdageom_.pln[isec][il][16])
      {
        phw[la] = sdageom_.pln[isec][il][11]
                 + (sdageom_.pln[isec][il][16] - 1.)
                 * sdageom_.pln[isec][il][12];
        dlw[la] = (w - sdageom_.pln[isec][il][16])
                 *2.*sdageom_.pln[isec][il3-1][18];
      }
      else
      {
        phw[la] = sdageom_.pln[isec][il][11]
                 + (w-1.)*sdageom_.pln[isec][il][12];
        dlw[la] = 0.0;
      }
/*
printf("phw[0]=%f\n",phw[0]);
*/
      /* phw[0] and  dlw[0] is NOT defined if layer 1 is missing */
      /* so set it here (can it be any number ???) */
      if(phw[0] < -999.)
      {
        phw[0] = phw[la];
        dlw[0] = dlw[la];
      }

      wstg[la]  = (phw[la] - phw[0])*sdageom_.pln[isec][il][6]
                 + dlw[la] - dlw[0];

      ddist[la] = ABS(digi[la][k][3]) / cpsi;
      derr[la]  = digi[la][k][4] / cpsi;
      yr[la][k][0] = wstg[la] - xr[la]*slopTF - ddist[la];
      yr[la][k][1] = wstg[la] - xr[la]*slopTF + ddist[la];
/*
printf("==>%d %d -> %f %f\n",la,k,yr[la][k][0],yr[la][k][1]);
printf("   (%f %f %f %f %f)\n",
wstg[la],xr[la],slopTF,ddist[la],digi[la][k][3],cpsi);
*/
    }
  }

  /* fill 'pattern recognition' histogram */
  for(i=0; i<NBIN; i++) iarray[i] = 0;
  for(la=0; la<NLAY; la++)
  {
    for(k=0; k<ndigi[la]; k++)
    {
      for(j=0; j<2; j++)
      {
        ibin1 = (int)((yr[la][k][j]-resSL)/resSL) + NBIN2;
        ibin2 = (int)((yr[la][k][j]+resSL)/resSL) + NBIN2;
        for(ibin=ibin1; ibin<=ibin2; ibin++)
        {
          if(ibin>=0 && ibin<NBIN) iarray[ibin] ++;
        }
      }
    }
  }

  /* print histogram
  for(i=1; i<(NBIN-1); i++)
  {
    if(iarray[i]>9) printf("x");
    else            printf("%1d",iarray[i]);
  }
  printf("\n");
  */

  /* find histogram maximum */
  itmp = -1;
  npeak = 0;
  i = 1;
  while(i<NBIN-1)
  {
    jtmp = iarray[i];
    if(jtmp>=itmp)
    {
      if(jtmp>itmp) npeak = 0;
      itmp = jtmp;
      ibin1 = ibin2 = i;
      while(iarray[++i]==jtmp && i<NBIN-1) ibin2 = i;
      /*peak[npeak++] = ((float)(ibin1+ibin2))/2.;*/
      peak[npeak++] = (float)ibin1;
    }
    else
    {
      i++;
    }
  }

/*
printf("\nnpeak=%d:",npeak);
for(i=0; i<npeak; i++) printf(" %f",peak[i]);
printf("\ntmp=%f\n",tmp);
*/

  /* put to zero hits located too far and resolve left-right ambiguity */
  for(i=0; i<npeak; i++)
  {
    /*tmp = (peak[i] - (float)NBIN2 - 0.5)*resSL;*/
    tmp = (peak[i] - (float)NBIN2)*resSL;
/*
printf("tmp[%1d]=%f\n",i,tmp);
*/

    lamin = 0;
    for(la=0; la<NLAY; la++)
    {
      il = is*NLAY + nst_max + la;
      y[la] = 0.;
      tmp1 = 10000.;
      kdigi[la] = -1;
      for(k=0; k<ndigi[la]; k++)
      {
        for(j=0; j<2; j++)
        {
          if(ABS(yr[la][k][j]-tmp) < tmp1 &&
             ABS(yr[la][k][j]-tmp) < xcell*sdageom_.pln[isec][il][18])
          {
            tmp1 = ABS(yr[la][k][j]-tmp);
            kdigi[la] = k;
            jdigi[la] = j;
          }
        }
      }

      if(kdigi[la] >= 0)
      {
        lamin ++;
        y[la] = yr[la][kdigi[la]][jdigi[la]];
        if(jdigi[la]==0)
        {
          digi[la][kdigi[la]][3] = -ABS(digi[la][kdigi[la]][3]);
        }
        else
        {
          digi[la][kdigi[la]][3] = ABS(digi[la][kdigi[la]][3]);
        }
      }
    }
    if(lamin < sdakeys_.lanal[is+2])
    {
/*
printf("is=%d lamin=%d lanal[is+2]=%d\n",
is,lamin,sdakeys_.lanal[is+2]);
*/
    }
    else
    {
      break;
    }
  }
  if(lamin < sdakeys_.lanal[is+2]) return;




  /* straight line fit */
/*
printf("    y1=%f %f %f %f %f %f\n",y[0],y[1],y[2],y[3],y[4],y[5]);
*/
  for(la=0; la<NLAY; la++) if(y[la] != 0.) y[la] += xr[la]*slopTF;
/*
printf("    y2=%f %f %f %f %f %f\n",y[0],y[1],y[2],y[3],y[4],y[5]);
*/
  /*lfit(xr,y,NLAY,0,&a,&b,&e);*/
  lfitw(xr,y,wfit,NLAY,0,&a,&b,&e);


  /*
  printf("  digi1-> ");
  for(la=0; la<NLAY; la++)
  {
    if(kdigi[la] >= 0) printf(" %6.2f",digi[la][kdigi[la]][3]);
    else               printf(" ------");
  }
  printf("\n  e=%f a_=%f b=%f\n\n",e,a-slopTF,b);
  */


  /* try to improve removing one hit if necessary 
  if(e > 0.1)
  {
    lamin = 0;
    for(la=0; la<NLAY; la++) if(kdigi[la]  >= 0) lamin++;
    if(lamin > sdakeys_.lanal[is+2])
	{
      itmp = -1;
      for(la=0; la<NLAY; la++)
      {
        if(kdigi[la]  >= 0)
        {
          tmp = y[la];
          y[la] = 0.;
          lfitw(xr,y,wfit,NLAY,0,&a,&b,&tmp1);
          if(tmp1 < e)
          {
            itmp = la;
            e = tmp1;
          }
          y[la] = tmp;
        }
      }
      if(itmp >= 0)
      {
        kdigi[itmp] = -1;
        y[itmp] = 0.;
printf("    y3=%f %f %f %f %f %f\n",y[0],y[1],y[2],y[3],y[4],y[5]);
      }
	}
  }
  */  

/*
printf("  digi2-> ");
for(la=0; la<NLAY; la++)
{
  if(kdigi[la] >= 0) printf(" %6.2f",digi[la][kdigi[la]][3]);
  else               printf(" ------");
}
printf("\n  e=%f a_=%f b=%f\n\n",e,a-slopTF,b); fflush(stdout);
*/



  chib  = e;
  slopD = a-slopTF;
  dat0  = b;


  if(chib < *chibest)
  {
    *chibest   = chib;
    *slopDbest = slopD;
    *dat0B = dat0;

    lamin = 0;
    for(la=0; la<NLAY; la++)
    {
      if(kdigi[la] >= 0)
      {
        for(i=0; i<5; i++) digib[la][i] = digi[la][kdigi[la]][i];
      }
      else
	  {
        digib[la][0] = -1.; /* negative wire number means missing layer */
	  }
      if(digib[la][0] > 0.) lamin++;
    }
  }

  return;
}








E 1
