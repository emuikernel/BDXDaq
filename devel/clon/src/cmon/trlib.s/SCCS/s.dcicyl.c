h47049
s 00000/00000/00000
d R 1.2 01/11/19 19:05:59 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 clas/cmon/trlib.s/dcicyl.c
e
s 00222/00000/00000
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

   Purpose and Methods : Intersection of a Track with a Cylinder.

      Calculates intersection of track  with cylindrical surface     
      of radius R.  The track is approximated by a cubic in the      
      track length.  To improve stability, the coordinate system     
      is shifted. First the Newton method (by P.D.) is used to find the     
      intersection. Derivatives are calculated analytically using    
      the parameters of cubic approximation. For input parameters    
      given correctly and for the track not too steep to the surface 
      the Newton method converges in 1-2 iterations and gives        
      ultimate accuracy. For the reasons of safety of the procedure  
      the old Half-Interval Division method is used if the first     
      method is failed.


   Inputs  :
      pln(20,npln) - plane array
      VECT(7)   x,y,z,px/p,py/p,pz/p,p at the beginning of step      
      VOUT(7)   x,y,z,px/p,py/p,pz/p,p at the end of step            
      IP        plane number                               
      SINT      step size (The value of SINT is changed!!!)          

   Outputs :
      SINT      step size from the beginning of the step to the      
                intersection point                                   
      XINT(7)   x,y,z,px/p,py/p,pz/p,p at the intersection point     
                if p at the beginning is different from p at the end 
                of the step then p at the intersection is the linear 
                interpolation between the input values               
      IFLAG     =1 if track intersects cylinder, =0 if not           

   Controls:
      EPSI     (internal variable) - accuracy in the track length with
               which the intersection is found. The accuracy is estimated
               as the difference in the track length between two last
               iterations for Newton method (actual error in the value is
               expected to be much smaller). The Half-Interval Division
               method uses this value as a limit to the accuracy in the
               direction orthogonal to the surface of the cylinder.
               Actual error in this method is expected to be of the order
               of 0.5*EPSI/R.
               Default value: 20 microns.

      MAXHIT   (internal variable) - limit to the number of iterations
               for the fitting procedures. Normally number of iterations
               doesn't exceed 3-4 for Newton method and 10-12 for
               Half-Interval Division method. Default value: 100

   Library belongs : libsda.a

   Calls : sda_gcubs

   Authors : R.Brun and J.Dumont from an original routine by
             H. Boerner, KEK October 1982.

   Modified: S. Laubach August,1990 and by B. Niczyporuk October,1990
   Modified: Pavel Degtyarenko added the Newton Method September 1992.
            

   Called by sda_swim

*/

#include <stdio.h>
#include "dclib.h"

#define ABS(x) ((x) < 0 ? -(x) : (x))
#define maxhit  100
#define epsi    0.002

void
dcicyl_(float pln[npln][20], float vect[7], float vout[7], int *ip,
               float *sint, float xint[7], int *iflag)
{
  dcicyl(pln, vect, vout, *ip, sint, xint, iflag);
  return;
}

void
dcicyl(float pln[npln][20], float vect[7], float vout[7], int ip,
               float *sint, float xint[7], int *iflag)
{
  int i;
  float x1[3],x2[3],a[4],b[4],c[4],r2,r12,r22,drctn,xshft,yshft,zshft,sshft,
        shiftx,shifty,shiftz,ash1,bsh1,rprim0,rprim1,s,s1,dinter;

  *iflag  = 1;

  if(pln[ip-1][6] <= 0.)
  {
    *iflag = 0;
    return;
  }

  /* translate to offset axes */

  for(i=0; i<3; i++)
  {
    x1[i] = vect[i] - pln[ip-1][i];
    x2[i] = vout[i] - pln[ip-1][i];
  }

  r12 = x1[0]*x1[0] + x1[1]*x1[1];
  r22 = x2[0]*x2[0] + x2[1]*x2[1];
  r2  = pln[ip-1][6] * pln[ip-1][6];
/*  dr2 = r22 - r2; - do not used !? */

  /* TRACK CROSSING THE CYLINDER FROM INSIDE OR OUTSIDE ? */

  if(r22 > r12)
  {
    if((r2 < r12) || (r2 > r22))
    {
      *iflag = 0;
      return;
    }
    drctn = 1.0;
  }
  else
  {
    if((r2 < r22) || (r2 > r12))
    {
      *iflag = 0;
      return;
    }
    drctn = -1.0;
  }

  /* SHIFT COORDINATE SYSTEM SUCH THAT CENTER OF GRAVITY=0 */

  shiftx = (x1[0] + x2[0]) * 0.5;
  shifty = (x1[1] + x2[1]) * 0.5;
  shiftz = (x1[2] + x2[2]) * 0.5;

  /* ONLY ONE VALUE NECESSARY SINCE X1= -X2 ETC. */

  xshft  = x1[0] - shiftx;
  yshft  = x1[1] - shifty;
  zshft  = x1[2] - shiftz;
  sshft  = - (*sint) * 0.5;

  /* PARAMETRIZE THE TRACK BY A CUBIC THROUGH X1, X2 */

  dcgcubs(sshft, xshft, vect[3], vout[3], a);
  dcgcubs(sshft, yshft, vect[4], vout[4], b);
  dcgcubs(sshft, zshft, vect[5], vout[5], c);

  /*
   ITERATE TO FIND THE TRACK LENGTH CORRESPONDING TO
   THE INTERSECTION OF TRACK AND CYLINDER.
   START AT S=0. - MIDDLE OF THE SHIFTED INTERVAL.
   First try to use Newton method: search for zero of the
   function f(s): s   = s - f(s )/f'(s )
                   i+1   i     i      i
  */

  ash1 = shiftx + a[0];
  bsh1 = shifty + b[0];
  rprim0 = 2.*(ash1*a[1] + bsh1*b[1]);
  if(rprim0 == 0.) goto a14;
  s1 = (r2 - ash1*ash1 - bsh1*bsh1) / rprim0;

  for(i=0; i<maxhit; i++)
  {
    float tmp1,tmp2;

    rprim1 = 2.*( ash1*a[1]         +   bsh1*b[1]            +
      s1*(2.*ash1*a[2]+a[1]*a[1]    +2.*bsh1*b[2]+b[1]*b[1]  +
      s1*(3.*(ash1*a[3]+a[1]*a[2]   +   bsh1*b[3]+b[1]*b[2]) +
      s1*(2.*(2.*a[1]*a[3]+a[2]*a[2]+2.*b[1]*b[3]+b[2]*b[2]) +
      s1*(5.*(a[2]*a[3]             +   b[2]*b[3]          ) +
      s1*(3.*(a[3]*a[3]             +   b[3]*b[3]          )
      ))))));
    if(rprim1 == 0.) goto a14;
    tmp1 = (ash1 + s1*(a[1] + s1*(a[2] + s1*a[3])));
    tmp2 = (bsh1 + s1*(b[1] + s1*(b[2] + s1*b[3])));
    s = s1 + (r2 - tmp1*tmp1 - tmp2*tmp2) / rprim1;
    if(ABS(s) > (*sint)*0.5) goto a14;
    if(ABS(s-s1) < epsi) goto a20;
    s1 = s;
  }

  /* Good Old Iteration Process */

a14:

  dinter = (*sint) * 0.5;
  s      = 0.0;

  for(i=0; i<maxhit; i++)
  {
    float x,y,rn2,dr2;

    x = shiftx + a[0] + s*(a[1] + s*(a[2] + s*a[3]));
    y = shifty + b[0] + s*(b[1] + s*(b[2] + s*b[3]));
    rn2 = x*x + y*y;
    dr2 = (r2 - rn2) * drctn;
    if(ABS(dr2) < epsi) goto a20;
    dinter = dinter * 0.5;
    if(dr2 < 0.) s = s - dinter;
    if(dr2 >= 0.) s = s + dinter;
  }

  /* COMPUTE INTERSECTION IN Offset COORDINATES */

a20:

  xint[0] = shiftx + a[0] +s*(a[1] + s*(a[2] + s*a[3])) + pln[ip-1][0];
  xint[1] = shifty + b[0] +s*(b[1] + s*(b[2] + s*b[3])) + pln[ip-1][1];
  xint[2] = shiftz + c[0] +s*(c[1] + s*(c[2] + s*c[3])) + pln[ip-1][2];
  xint[3] = a[1] + s*(2.*a[2] + 3.*s*a[3]);
  xint[4] = b[1] + s*(2.*b[2] + 3.*s*b[3]);
  xint[5] = c[1] + s*(2.*c[2] + 3.*s*c[3]);
  xint[6] = 0.5 * (vect[6] + vout[6]);
  i = 3;
  vunit_(&xint[3], &xint[3], &i);
  *sint = (*sint)*0.5 + s;

  return;
}
E 1
