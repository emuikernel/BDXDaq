/*
C-
C-    Purpose:  Calculates transport matrix for one step.
C-              Records matrix at point of layer penetration.
C-
C-    Inputs:  NEW = 1 TRANSMAT called 1-st time, otherwise: NEW = 0
C-             VECT(9)  -  vector {x,y,z,px/p,py/p,pz/p,p,m,Q} at begining step
C-             VOUT(9)  -  vector at end of step
C-             SL       -  Current step length
C-             BFLD(3)  -  Magnetic field: Bx,By,Bz
C-             IFLAG       = 1 track crossed a layer
C-                         = 0 not
C-    Output:  tmat(5,5) - Transport matrix
C-
C-    Controls:         
C-
C-  Library belongs: libsda.a
C-
C-     Calls: dcxmm55
C-
C-    Author:  S. Laubach, B. Niczyporuk (Jul. 1990)
C-    Modified by: B. Niczyporuk in October 1990 (see CEBAF-PR-91-004)
C-                 B. Niczyporuk on Dec.9, 1996 (when 1-st plane < 1-st step) 
C-                 S.Boiarinov - Jan 1998 - made tmat() as parameter
C-
C-    Called by dcswim
C-
*/

#include <stdio.h>
#include <math.h>
#include "dclib.h"

#define ABS(x) ((x) < 0 ? -(x) : (x))
#define EC 2.9979251e-4

/*void
dctrmat_(int *new, float vect[9], float vout[9], float *sltmp,
              float bfld[3], float b[5][5], float tmat[5][5], int *iflagtmp)*/

void
dctrmat_(int *new, float vect[9], float vout[9], float *sltmp,
              float bfld[3], float bb[25], float ttt[25], int *iflagtmp)
{
  int i, k, iflag;
  float pm12, sinl, cosl, cosl1, sinp, cosp, hamx, fact, b0,
        b2, b3, tgl, tn[3], hn[3], ch, sl, tmp1, tmp2;
  /*float a[5][5];*/
  float aa[25];

  iflag = *iflagtmp;
  sl    = *sltmp;

  /* error propagation on a HELIX */

  ch    = vect[8];
  tn[0] = vect[3] + vout[3];
  tn[1] = vect[4] + vout[4];
  tn[2] = vect[5] + vout[5];
  pm12  = 1. / sqrt(tn[0]*tn[0] + tn[1]*tn[1] + tn[2]*tn[2]);
  tn[0] = tn[0] * pm12;
  tn[1] = tn[1] * pm12;
  tn[2] = tn[2] * pm12;

  sinl = tn[2];
  cosl = sqrt(ABS(1. - sinl*sinl));
  if(cosl < 0.0000001)
  {
    /*printf("dctrmat: particle travels in z-direction\n");*/
    return;
  }
  cosl1 = 1. / cosl;
  sinp  = tn[1] * cosl1;
  cosp  = tn[0] * cosl1;

  /* define transformation matrix between x1 and x2
     for neutral particle or fieldfree region */



  /*
  for(i=0; i<5; i++)
  {
    for(k=0; k<5; k++)
    {
      a[k][i] = 0.;
    }
    a[i][i] = 1.;
  }
  */
  for(i=0; i<25; i++) aa[i]=0.0;
  aa[0]=aa[6]=aa[12]=aa[18]=aa[24]=1.0;

  /*
  a[2][3] = sl * cosl;
  a[1][4] = sl;
  */
  aa[13] = sl * cosl;
  aa[9]  = sl;



  if(ch == 0.) goto a100;
  /* boy: check original CERN function */
  /* IF(mgtyp.EQ.5) GOTO 100 - calling program have to use ch=0 to
     let this program know that we are runing without field !!! */
  pm12 = 2./(vect[6] + vout[6]);
  hamx  = sqrt(bfld[0]*bfld[0] + bfld[1]*bfld[1] + bfld[2]*bfld[2])*pm12;
  if(hamx <= 0.005) goto a100;

  /* define average magnetic field and gradient */

  fact = ch * EC;
  hn[0] = bfld[0] * fact;
  hn[1] = bfld[1] * fact;
  hn[2] = bfld[2] * fact;

  b0  =  hn[0] * cosp + hn[1] * sinp;
  b2  = -hn[0] * sinp + hn[1] * cosp;
  b3  = -b0 * sinl + hn[2] * cosl;
  tgl =  sinl * cosl1;

  /* complete transformation matrix between errors at x1 and x2 */

  tmp1=sl*pm12;
  tmp2=tmp1*pm12;


  /*
  a[0][1] =  sl * b2;
  a[2][1] = -b0 * tmp1;
  a[3][1] =  b2 * b3 * tmp2;
  a[4][1] = -b2 * b2 * tmp2;

  a[0][2] = -sl * b3 * cosl1;
  a[1][2] =  b0 * tmp1 * cosl1*cosl1;
  a[2][2] =  1. + tgl * b2 * tmp1;
  a[3][2] = -b3 * b3 * tmp2 * cosl1;
  a[4][2] =  b3 * b2 * tmp2 * cosl1;
  a[4][3] = -b3 * tgl * tmp1;
  a[3][4] =  b3 * tgl * tmp1;
  */
  aa[1]  =  sl * b2;
  aa[11] = -b0 * tmp1;
  aa[16] =  b2 * b3 * tmp2;
  aa[21] = -b2 * b2 * tmp2;
  aa[2]  = -sl * b3 * cosl1;
  aa[7]  =  b0 * tmp1 * cosl1*cosl1;
  aa[12] =  1. + tgl * b2 * tmp1;
  aa[17] = -b3 * b3 * tmp2 * cosl1;
  aa[22] =  b3 * b2 * tmp2 * cosl1;
  aa[23] = -b3 * tgl * tmp1;
  aa[19] =  b3 * tgl * tmp1;



a100:

  /* *new = 0  transformation matrix is updated
     *new = 1  transformation matrix is initialized */

  if(*new == 1)
  {
    *new = 0;


	/*
    for(i=0; i<5; i++)
    {
      for(k=0; k<5; k++)
      {
        b[k][i] = a[k][i];
      }
    }
	*/
    for(i=0; i<25; i++) bb[i] = aa[i];


    /* Already the track crossed VTX plane on the 1-st step */

    if(iflag == 1)
    {


	  /*
      for(i=0; i<5; i++)
      {
        for(k=0; k<5; k++)
        {
          tmat[k][i] = a[k][i];
        }
      }
	  */
      for(i=0; i<25; i++) ttt[i] = aa[i];


    }
    return;
  }

  if(iflag == 1)
  { /* track have cross a layer (plane), store transport matrix tmat(5,5) */


    /*sda_xmm55_(a,b,tmat);*/
    dcxmm55_(aa,bb,ttt);


  }
  else
  { /* not crossed, continue the matrix error propagation */


    /*sda_xmm55_(a,b,b);*/
    dcxmm55_(aa,bb,bb);


  }

  return;
}
