h20939
s 00000/00000/00000
d R 1.2 01/11/19 19:05:58 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 clas/cmon/trlib.s/dcdocam.c
e
s 00120/00000/00000
d D 1.1 01/11/19 19:05:57 boiarino 1 0
c date and time created 01/11/19 19:05:57 by boiarino
e
u
U
f e 0
t
T
I 1
/*
   Purpose and Methods : Calculates Distance to the hit wire


   Input   :  is          - SuperLayer number
              digi[la][0] - Wire number "iw" - only good (positive) !!!
              digi[la][1] - Raw TDC [counts]
              digi[la][2] - Drift Time Td [ns] to the hit wire# "iw"

   Outputs :  digi[la][3] - Drift Distance (measured) Dm [cm]
              digi[la][4] - Sigma of Drift Distance [cm]
              ndigi,digi - bad hits will be removed from 'digi' array !!!

   Controls:

   Library belongs    : libsda.a

   Calls              : none

   Created   27-FEB-1996  Bogdan Niczyporuk
   C remake by Sergey Boyarinov

   Called by dcdcam
*/

#include <stdio.h>
#include "dclib.h"
#include "sdacalib.h"
#include "sdakeys.h"
SDAkeys sdakeys_;

#define ABS(x) ((x) < 0 ? -(x) : (x))

#define NLAY 6       /* Maximum layers in SuperLayer */

void
dcdocam_(int *is, int ndigi[NLAY], float digi[NLAY][nhlmx][5])
{
  dcdocam(*is, ndigi, digi);
  return;
}

void
dcdocam(int is, int ndigi[NLAY], float digi[NLAY][nhlmx][5])
{
  int i, j, il, la, ig, k, n;
  float slope;

  /* Here is the option of constant velocity in a drift cell (the old way) */

  if(sdakeys_.zderr[8] == 1.)
  {
    for(la=0; la<NLAY; la++)
    {
      for(k=0; k<ndigi[la]; k++)
      {
        if(digi[la][k][0] <= 0.) continue; /* Discard bad wire */
        digi[la][k][4] = sdakeys_.zderr[is];
        digi[la][k][3] = digi[la][k][2] * sdacalib_.vgas;
      }
    }
    return;
  }

    /* Convert the Drift Time into the Drift Distance:  Dm = f(Td)
       Get average slope at Tmax/2 */

  for(la=0; la<NLAY; la++)
  {
    il = (is-1)*NLAY + la;
    n = ndigi[la];
    k = 0;
    while(k<n)
    {
      /* Check that wire number > 0 and Drift Time range: {0 - Tmax} */
      if(digi[la][k][0] <= 0. || digi[la][k][2] <= -10.0 ||
         digi[la][k][2] >= sdacalib_.td_h[il][1])
      {
        n--;
        for(i=k; i<n; i++)
        {
          for(j=0; j<5; j++) digi[la][i][j] = digi[la][i+1][j];
        }
      }
      else
      {
        if(digi[la][k][2] > -10. && digi[la][k][2] <= 0.)
        {
          digi[la][k][2] = -digi[la][k][2]; /* ... */
        }
        /* Get Drift Distance */
        ig = digi[la][k][2] / sdacalib_.td_f[il][1][0] + 1.0;
        slope =  (sdacalib_.td_f[il][ig][1] - sdacalib_.td_f[il][ig-1][1])
               / (sdacalib_.td_f[il][ig][0] - sdacalib_.td_f[il][ig-1][0]);
        digi[la][k][3] = sdacalib_.td_f[il][ig-1][1]
                      + slope*(digi[la][k][2] - sdacalib_.td_f[il][ig-1][0]);
        /* Get Drift Distance error */
        digi[la][k][4] = sdakeys_.zderr[is];
        k++;
      }
    }
    ndigi[la] = n;
  }

  return;
}














E 1
