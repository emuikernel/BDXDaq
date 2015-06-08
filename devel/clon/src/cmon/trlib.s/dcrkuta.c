/*
   dcrkuta.c - one-step tracking a particle through a magnetic field using
   Runge-Kutta method
                                           ->  ->  ->
                                         | i   j   k  |
     2 ->              ->                |            |
    d  r     e       d r   ->      e     | dx  dy  dz |
    ---- = ----- * [ --- x H ] = ----- * | --  --  -- |
       2   p * c     ds          p * c   | ds  ds  ds |
     ds                                  |            |
                                         | Hx  Hy  Hz |

      2
     d x     e       dy        dz              dy   dz
     --- = ----- * ( -- * Hz - -- * Hy ) = Fx( -- , -- )
       2   p * c     ds        ds              ds   ds
     ds

      2
     d y     e       dz        dx              dx   dz
     --- = ----- * ( -- * Hx - -- * Hz ) = Fy( -- , -- )
       2   p * c     ds        ds              ds   ds
     ds

      2
     d z     e       dx        dy              dx   dy
     --- = ----- * ( -- * Hy - -- * Hx ) = Fz( -- , -- )
       2   p * c     ds        ds              ds   ds
     ds

  method:

    X   = X + (1/6) * ( Kx[0] + 2 * Kx[1] + 2 * Kx[2] + Kx[3] )
     k+1   k

    Y   = Y + (1/6) * ( Ky[0] + 2 * Ky[1] + 2 * Ky[2] + Ky[3] )
     k+1   k

    Z   = Z + (1/6) * ( Kz[0] + 2 * Kz[1] + 2 * Kz[2] + Kz[3] )
     k+1   k

    dx|      dx|
    --|   =  --|  + (1/6) * ( Lx[0] + 2 * Lx[1] + 2 * Lx[2] + Lx[3] )
    ds|k+1   ds|k


    dy|      dy|
    --|   =  --|  + (1/6) * ( Ly[0] + 2 * Ly[1] + 2 * Ly[2] + Ly[3] )
    ds|k+1   ds|k


    dz|      dz|
    --|  =   --|  + (1/6) * ( Lz[0] + 2 * Lz[1] + 2 * Lz[2] + Lz[3] )
    ds|k+1   ds|k


  where:

            dx|
    Kx[0] = --| * step
            ds|k

            dy|
    Ky[0] = --| * step
            ds|k

            dz|
    Kz[0] = --| * step
            ds|k

                dy|    dz|
    Lx[0] = Fx( --| ,  --| ) * step
                ds|k   ds|k

                dx|    dz|
    Ly[0] = Fy( --| ,  --| ) * step
                ds|k   ds|k

                dx|    dy|
    Lz[0] = Fz( --| ,  --| ) * step
                ds|k   ds|k

  ---------------------------------

              dx|   Lx[0]
    Kx[1] = ( --| + ----- ) * step
              ds|k    2

              dy|   Ly[0]
    Ky[1] = ( --| + ----- ) * step
              ds|k    2

              dz|   Lz[0]
    Kz[1] = ( --| + ----- ) * step
              ds|k    2

                  dy|   Ly[0]       dz|   Lz[0]
    Lx[1] = Fx( ( --| + ----- ) , ( --| + ----- ) ) * step
                  ds|k    2         ds|k    2

                  dx|   Lx[0]       dz|   Lz[0]
    Ly[1] = Fy( ( --| + ----- ) , ( --| + ----- ) ) * step
                  ds|k    2         ds|k    2

                  dx|   Lx[0]       dy|   Ly[0]
    Lz[1] = Fz( ( --| + ----- ) , ( --| + ----- ) ) * step
                  ds|k    2         ds|k    2

  ---------------------------------

              dx|   Lx[1]
    Kx[2] = ( --| + ----- ) * step
              ds|k    2

              dy|   Ly[1]
    Ky[2] = ( --| + ----- ) * step
              ds|k    2

              dz|   Lz[1]
    Kz[2] = ( --| + ----- ) * step
              ds|k    2

                  dy|   Ly[1]       dz|   Lz[1]
    Lx[2] = Fx( ( --| + ----- ) , ( --| + ----- ) ) * step
                  ds|k    2         ds|k    2

                  dx|   Lx[1]       dz|   Lz[1]
    Ly[2] = Fy( ( --| + ----- ) , ( --| + ----- ) ) * step
                  ds|k    2         ds|k    2

                  dx|   Lx[1]       dy|   Ly[1]
    Lz[2] = Fz( ( --| + ----- ) , ( --| + ----- ) ) * step
                  ds|k    2         ds|k    2

  ---------------------------------

              dx|
    Kx[3] = ( --| + Lx[2] ) * step
              ds|k

              dy|
    Ky[3] = ( --| + Ly[2] ) * step
              ds|k

              dz|
    Kz[3] = ( --| + Lz[2] ) * step
              ds|k

                  dy|               dz|
    Lx[3] = Fx( ( --| + Ly[2] ) , ( --| + Lz[2] ) ) * step
                  ds|k              ds|k

                  dx|               dz|
    Ly[3] = Fy( ( --| + Lx[2] ) , ( --| + Lz[2] ) ) * step
                  ds|k              ds|k

                  dx|               dy|
    Lz[3] = Fz( ( --| + Lx[2] ) , ( --| + Ly[2] ) ) * step
                  ds|k              ds|k


  units:

    p * c [GeV]                 -7  H [Gauss]
    ----------- = 2.9979251 * 10  * ---------
      charge                        ro [1/cm]

*/

#include <stdio.h>
#include <math.h>
#include "dclib.h"

#define ABS(x) ((x) < 0 ? -(x) : (x))

#define MAXIT  1992
#define MAXCUT 11
#define EC     2.9979251e-4
#define DLT    1.0e-4
#define DLT32  DLT/32.
#define ZERO   0.
#define ONE    1.
#define TWO    2.
#define THREE  3.
#define SIX    6.
#define THIRD  ONE/THREE
#define HALF   ONE/TWO
#define SIXTH  ONE/SIX
#define PISQUA .986960440109e+01


#define GETFIELD \
{ \
  float xyz[3],ff[3],fp[3]; \
  xyz[2] = xyzt[0]; \
  xyz[0] = xyzt[1]; \
  xyz[1] = xyzt[2]; \
  sda_snake_(xyz,ff); \
printf("Told=%7.3f%7.3f%7.3f ",ff[0],ff[1],ff[2]); \
  xyz[2] = xyzt[0]; \
  xyz[0] = xyzt[1]; \
  xyz[1] = xyzt[2]; \
  dcfieldT(xyz,ff); \
printf("Tnew=%7.3f%7.3f%7.3f\n",ff[0],ff[1],ff[2]); \
  xyz[2] = xyz[2] + 55.0; \
  sda_snake_pt_(xyz,fp); \
printf("Pold=%7.3f%7.3f%7.3f ",fp[0],fp[1],fp[2]); \
  dcfieldP(xyz,fp); \
printf("Pnew=%7.3f%7.3f%7.3f\n",fp[0],fp[1],fp[2]); \
  ff[0] += fp[0]; \
  ff[1] += fp[1]; \
  ff[2] += fp[2]; \
  f[0] = ff[2]; \
  f[1] = ff[0]; \
  f[2] = ff[1]; \
}

#define GETFIELD1 \
{ \
  float xyz[3],ff[3],fp[3]; \
  xyz[2] = xyzt[0]; \
  xyz[0] = xyzt[1]; \
  xyz[1] = xyzt[2]; \
  sda_snake_(xyz,ff); \
  xyz[1] = xyz[1] - 0.0; \
  xyz[2] = xyz[2] + 55.0; \
  sda_snake_pt_(xyz,fp); \
  ff[0] += fp[0]; \
  ff[1] += fp[1]; \
  ff[2] += fp[2]; \
  f[0] = ff[2]; \
  f[1] = ff[0]; \
  f[2] = ff[1]; \
}

#define GETFIELD2 \
{ \
  float xyz[3],ff[3],fp[3]; \
  xyz[2] = xyzt[0]; \
  xyz[0] = xyzt[1]; \
  xyz[1] = xyzt[2]; \
  dcfieldT(xyz,ff); \
  xyz[1] = xyz[1] - 0.0; \
  xyz[2] = xyz[2] + 55.0; \
  dcfieldP(xyz,fp); \
  ff[0] += fp[0]; \
  ff[1] += fp[1]; \
  ff[2] += fp[2]; \
  f[0] = ff[2]; \
  f[1] = ff[0]; \
  f[2] = ff[1]; \
}


void
dcrkuta_(float *step, float vect[9], float vout[9], float bfld[3])
{
  dcrkuta(*step, vect, vout, bfld);
}

void
dcrkuta(float step, float vect[9], float vout[9], float bfld[3])
{
  int j, ncut, iter;
  float charge, f[4], xyzt[3], x, y, z, xt, yt, zt;
  double Lx[4], Ly[4], Lz[4], hxp[3], pinv, tl, rest, h, h2, h4, ph, ph2,
         a, b, c, ang2, dxt, dyt, dzt, est, at, bt, ct, cba, f1, f2, f3,
         f4, rho, tet, hnorm, hp, rho1, sint, cost, g1, g2, g3, g4, g5, g6;

  iter = 0;
  ncut = 0;
  for(j=0; j<9; j++) vout[j] = vect[j];
  charge = vect[8];
  pinv   = EC * charge / vect[6];
  tl     = 0.;
  h      = step;

a20:

  rest  = step - tl;
  if(ABS(h) > ABS(rest)) h = rest;

/*
  dcfield(vout,f);
*/
{xyzt[0] = vout[0]; xyzt[1] = vout[1]; xyzt[2] = vout[2];}
GETFIELD2;

/* start of integration */

  x        = vout[0];
  y        = vout[1];
  z        = vout[2];
  a        = vout[3];
  b        = vout[4];
  c        = vout[5];
  h2       = HALF * h;
  h4       = HALF * h2;
  ph       = pinv * h;
  ph2      = HALF * ph;
  Lx[0] = (b * f[2] - c * f[1]) * ph2;
  Ly[0] = (c * f[0] - a * f[2]) * ph2;
  Lz[0] = (a * f[1] - b * f[0]) * ph2;
  ang2 = Lx[0]*Lx[0] + Ly[0]*Ly[0] + Lz[0]*Lz[0];

  if(ang2 > PISQUA) goto a40;

  dxt      = h2 * a + h4 * Lx[0];
  dyt      = h2 * b + h4 * Ly[0];
  dzt      = h2 * c + h4 * Lz[0];
  xt       = x + dxt;
  yt       = y + dyt;
  zt       = z + dzt;

/* second intermediate point */

  est = ABS(dxt)+ABS(dyt)+ABS(dzt);
  if(est > h) goto a30;
{xyzt[0] = xt; xyzt[1] = yt; xyzt[2] = zt;}
/*
  dcfield(xyzt,f);
*/
GETFIELD2;

  bfld[0] = f[0]; bfld[1] = f[1]; bfld[2] = f[2];

  at       = a + Lx[0];
  bt       = b + Ly[0];
  ct       = c + Lz[0];

  Lx[1] = (bt * f[2] - ct * f[1]) * ph2;
  Ly[1] = (ct * f[0] - at * f[2]) * ph2;
  Lz[1] = (at * f[1] - bt * f[0]) * ph2;
  at       = a + Lx[1];
  bt       = b + Ly[1];
  ct       = c + Lz[1];
  Lx[2] = (bt * f[2] - ct * f[1]) * ph2;
  Ly[2] = (ct * f[0] - at * f[2]) * ph2;
  Lz[2] = (at * f[1] - bt * f[0]) * ph2;
  dxt      = h * (a + Lx[2]);
  dyt      = h * (b + Ly[2]);
  dzt      = h * (c + Lz[2]);
  xt       = x + dxt;
  yt       = y + dyt;
  zt       = z + dzt;
  at       = a + TWO * Lx[2];
  bt       = b + TWO * Ly[2];
  ct       = c + TWO * Lz[2];

  est = ABS(dxt)+ABS(dyt)+ABS(dzt);
  if(est > 2.*ABS(h)) goto a30;
{xyzt[0] = xt; xyzt[1] = yt; xyzt[2] = zt;}
/*
  dcfield(xyzt,f);
*/
GETFIELD2;

  z        = z + (c + (Lz[0] + Lz[1] + Lz[2]) * THIRD) * h;
  y        = y + (b + (Ly[0] + Ly[1] + Ly[2]) * THIRD) * h;
  x        = x + (a + (Lx[0] + Lx[1] + Lx[2]) * THIRD) * h;

  Lx[3] = (bt * f[2] - ct * f[1]) * ph2;
  Ly[3] = (ct * f[0] - at * f[2]) * ph2;
  Lz[3] = (at * f[1] - bt * f[0]) * ph2;
  a        = a + (Lx[0]+Lx[3] + TWO * (Lx[1]+Lx[2])) * THIRD;
  b        = b + (Ly[0]+Ly[3] + TWO * (Ly[1]+Ly[2])) * THIRD;
  c        = c + (Lz[0]+Lz[3] + TWO * (Lz[1]+Lz[2])) * THIRD;

  est      = ABS(Lx[0] + Lx[3] - (Lx[1] + Lx[2]))
           + ABS(Ly[0] + Ly[3] - (Ly[1] + Ly[2]))
           + ABS(Lz[0] + Lz[3] - (Lz[1] + Lz[2]));

  if(est > DLT && ABS(h) > 1.0e-4) goto a30;
  iter++;
  ncut = 0;

/* if too many iterations, go to HELIX */

  if(iter > MAXIT) goto a40;

/* */

  tl = tl + h;
  if(est < DLT32)
  {
    h = h * TWO;
  }
  cba    = ONE / sqrt(a*a + b*b + c*c);
  vout[0] = x;
  vout[1] = y;
  vout[2] = z;
  vout[3] = cba * a;
  vout[4] = cba * b;
  vout[5] = cba * c;
  rest = step - tl;
  if(step < 0.) rest = -rest;
  if(rest > 1.0e-5 * ABS(step)) goto a20;

  return;

/* cut step */

a30:

  ncut++;

/* if too many cuts, go to HELIX */

  if(ncut > MAXCUT) goto a40;

  h = h * HALF;
  goto a20;

/* angle too big, use HELIX */

a40:

printf("dc_rkuta: never come here\n");
exit(0);

/*
  F1  = F(1)
  F2  = F(2)
  F3  = F(3)
  F4  = SQRT(F1**2+F2**2+F3**2)
  RHO = -F4*PINV
  TET = RHO * STEP
  IF(TET.NE.0.) THEN
    HNORM = ONE/F4
    F1 = F1*HNORM
    F2 = F2*HNORM
    F3 = F3*HNORM

    HXP(1) = F2*VECT(IPZ) - F3*VECT(IPY)
    HXP(2) = F3*VECT(IPX) - F1*VECT(IPZ)
    HXP(3) = F1*VECT(IPY) - F2*VECT(IPX)

    HP = F1*VECT(IPX) + F2*VECT(IPY) + F3*VECT(IPZ)

    RHO1 = ONE/RHO
    SINT = SIN(TET)
    COST = TWO*SIN(HALF*TET)**2

    G1 = SINT*RHO1
    G2 = COST*RHO1
    G3 = (TET-SINT) * HP*RHO1
    G4 = -COST
    G5 = SINT
    G6 = COST * HP

    VOUT(IX) = VECT(IX) + (G1*VECT(IPX) + G2*HXP(1) + G3*F1)
    VOUT(IY) = VECT(IY) + (G1*VECT(IPY) + G2*HXP(2) + G3*F2)
    VOUT(IZ) = VECT(IZ) + (G1*VECT(IPZ) + G2*HXP(3) + G3*F3)

    VOUT(IPX) = VECT(IPX) + (G4*VECT(IPX) + G5*HXP(1) + G6*F1)
    VOUT(IPY) = VECT(IPY) + (G4*VECT(IPY) + G5*HXP(2) + G6*F2)
    VOUT(IPZ) = VECT(IPZ) + (G4*VECT(IPZ) + G5*HXP(3) + G6*F3)
  ELSE
    VOUT(IX) = VECT(IX) + STEP*VECT(IPX)
    VOUT(IY) = VECT(IY) + STEP*VECT(IPY)
    VOUT(IZ) = VECT(IZ) + STEP*VECT(IPZ)
  ENDIF
*/

}

/*
   Performes tracking of one step in a magnetic field. The trajectory is assumed
   to be helix in a constant field taken at the mid point of the step.

   Units are kgauss,centimeters,gev/c.

   Inputs  : step - arc length of the step asked (cm)
             vect - input vector {position,direction cos,p,m,Q}
             

   Outputs : vout - the same as vect after completion of the step
             bfld - magnetic field of the step (mid step)

   Remake from CERN GHELIX() FORTRAN function and adjustment for
   the CLAS by Sergey Boyarinov
*/

void
dchelix_(float *step, float vect[9], float vout[9], float bfld[3])
{
  dchelix(*step, vect, vout, bfld);
}

void
dchelix(float step, float vect[9], float vout[9], float bfld[3])
{
  int i;
  float xyz[3], f[4], hxp[3];
  float h2xy, hp, rho, tet, sint, sintt, tsint, cos1t, tmp;
  float f1, f2, f3, f4, f5, f6;
  float charge, xyzt[3];
  
  vout[6] = vect[6];
  vout[7] = vect[7];
  vout[8] = charge = vect[8];

  if(charge == 0.) goto a10; /* if no charge do step without field */


  xyz[0] = vect[0] + 0.5 * step * vect[3];
  xyz[1] = vect[1] + 0.5 * step * vect[4];
  xyz[2] = vect[2] + 0.5 * step * vect[5];



{xyzt[0] = xyz[0]; xyzt[1] = xyz[1]; xyzt[2] = xyz[2];}
/*
  dcfield(xyzt,f);
*/
GETFIELD2;

bfld[0] = f[0];
bfld[1] = f[1];
bfld[2] = f[2];

  f[3] = f[0]*f[0] + f[1]*f[1] + f[2]*f[2];
  if(f[3] <= 1.e-12) goto a10;

  f[3] = sqrt(f[3]);
  f[0] = f[0]/f[3];
  f[1] = f[1]/f[3];
  f[2] = f[2]/f[3];
  f[3] = f[3]*EC;

  hxp[0] = f[1]*vect[5] - f[2]*vect[4];
  hxp[1] = f[2]*vect[3] - f[0]*vect[5];
  hxp[2] = f[0]*vect[4] - f[1]*vect[3];
 
  hp = f[0]*vect[3] + f[1]*vect[4] + f[2]*vect[5];

  rho = -charge*f[3]/vect[6];
  tet = rho * step;
  if(ABS(tet) > 0.15)
  {
    sint = sin(tet);
    sintt = sint/tet;
    tsint = (tet-sint)/tet;
    tmp = sin(0.5*tet);
    cos1t = 2.*tmp*tmp/tet;
  }
  else
  {
    tsint = SIXTH * tet * tet;
    sintt = 1. - tsint;
    sint = tet * sintt;
    cos1t = 0.5 * tet;
  }

  f1 = step * sintt;
  f2 = step * cos1t;
  f3 = step * tsint * hp;
  f4 = -tet * cos1t;
  f5 = sint;
  f6 = tet * cos1t * hp;
 
  vout[0] = vect[0] + (f1*vect[3] + f2*hxp[0] + f3*f[0]);
  vout[1] = vect[1] + (f1*vect[4] + f2*hxp[1] + f3*f[1]);
  vout[2] = vect[2] + (f1*vect[5] + f2*hxp[2] + f3*f[2]);
 
  vout[3] = vect[3] + (f4*vect[3] + f5*hxp[0] + f6*f[0]);
  vout[4] = vect[4] + (f4*vect[4] + f5*hxp[1] + f6*f[1]);
  vout[5] = vect[5] + (f4*vect[5] + f5*hxp[2] + f6*f[2]);

  return;

a10:

  for(i=0; i<3; i++)
  {
    vout[i]   = vect[i] + step * vect[i+3];
    vout[i+3] = vect[i+3];
  }

  return;
}





