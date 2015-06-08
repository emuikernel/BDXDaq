/*

   Purpose and Methods : Intersection of a Track with a Plane.

      Calculates intersection of track  with a plane situated at     
      the distance 'd' from the center of coordinates. The vector    
      'ortvec' is the unit length vector directed from the center    
      of coordinates to the direction of the plane and orthogonal    
      to it.  The track is approximated by a cubic in the track      
      length.     To improve stability, the coordinate system        
      is shifted. Normally the Newton method is used to find the     
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
      IFLAG     =1 if track intersects the surface, =0 if not        

   Controls:
      EPSI     (internal variable) - accuracy in the track length with
               which the intersection is found. The accuracy is estimated
               as the difference in the track length between two last
               iterations for Newton method (actual error in the value is
               expected to be much smaller). The Half-Interval Division
               method uses this value as a limit to the accuracy in the
               direction orthogonal to the plane
               Actual error in this method is expected to be of the order
               of EPSI.
               Default value: 20 microns.

      MAXHIT   (internal variable) - limit to the number of iterations
               for the fitting procedures. Normally number of iterations
               doesn't exceed 3-4 for Newton method and 10-12 for
               Half-Interval Division method. Default value: 100

   Library belongs : libsda.a

   Calls : sda_gcubs

   Created : by Pavel Degtyarenko (September 1992) based on INT_CYL.
            
   Authors of INT_CYL: R.Brun and J.Dumont from an original routine by
                       H. Boerner, KEK October 1982.
                       Modified: S. Laubach August,1990 and by
                                 B. Niczyporuk October, 1990
                                 B. Niczyporuk December,1996

   Called by sda_swim

*/

#include <stdio.h>
#include "dclib.h"

#define ABS(x) ((x) < 0 ? -(x) : (x))
#define maxhit  100
#define epsi    0.002

void
dcipln_(float pln[npln][20], float vect[7], float vout[7], int *ip,
               float *sint, float xint[7], int *iflag)
{
  dcipln(pln, vect, vout, *ip, sint, xint, iflag);
  return;
}

void
dcipln(float pln[npln][20], float vect[7], float vout[7], int ip,
               float *sint, float xint[7], int *iflag)
{
  int i;
  float x1[3],x2[3],a[4],b[4],c[4],ortvec[3],drctn,xshft,yshft,zshft,sshft,
        shiftx,shifty,shiftz,d,r1,r2,rprim0,rprim1,s,s1,dinter;

  *iflag  = 1;

  if(pln[ip-1][7] <= 0.)
  {
    *iflag = 0;
    return;
  }

  ortvec[0] = pln[ip-1][3];
  ortvec[1] = pln[ip-1][4];
  ortvec[2] = pln[ip-1][5];
  d         = pln[ip-1][7];

  /* translate to offset axes */

  for(i=0; i<3; i++)
  {
    x1[i] = vect[i] - pln[ip-1][i];
    x2[i] = vout[i] - pln[ip-1][i];
  }

  /* the distances of the 1-st and 2-nd point to the plane parallel to */

  r1 = x1[0]*ortvec[0] + x1[1]*ortvec[1] + x1[2]*ortvec[2];
  r2 = x2[0]*ortvec[0] + x2[1]*ortvec[1] + x2[2]*ortvec[2];

  /* TRACK CROSSING THE PLANE FROM INSIDE OR OUTSIDE ? */

  if(r2 > r1)
  {
    if((d < r1) || (d > r2))
    {
      *iflag = 0;
      return;
    }
    drctn = 1.0;
  }
  else
  {
    if((d < r2) || (d > r1))
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
   THE INTERSECTION OF TRACK AND PLANE.
   START AT S=0. - MIDDLE OF THE SHIFTED INTERVAL.
   First try to use Newton method: search for zero of the
   function f(s): s   = s - f(s )/f'(s )
                   i+1   i     i      i
  */

  rprim0 = a[1]*ortvec[0] + b[1]*ortvec[1] + c[1]*ortvec[2];
  if(rprim0 == 0.) goto a14;
  s1 = ( d - (shiftx + a[0]) * ortvec[0]
           - (shifty + b[0]) * ortvec[1]
           - (shiftz + c[0]) * ortvec[2] ) / rprim0;

  for(i=0; i<maxhit; i++)
  {
    rprim1 = ( ortvec[0] * (a[1] + s1 * (2. * a[2] + 3. * a[3] * s1)) +
               ortvec[1] * (b[1] + s1 * (2. * b[2] + 3. * b[3] * s1)) +
               ortvec[2] * (c[1] + s1 * (2. * c[2] + 3. * c[3] * s1))  );
    if(rprim1 == 0.) goto a14;
    s = s1 + (d -
      ( (shiftx + a[0] + s1*(a[1]+s1*(a[2]+s1*a[3]))) * ortvec[0] +
        (shifty + b[0] + s1*(b[1]+s1*(b[2]+s1*b[3]))) * ortvec[1] +
        (shiftz + c[0] + s1*(c[1]+s1*(c[2]+s1*c[3]))) * ortvec[2] ))
        / rprim1;
    if(ABS(s) > (*sint)*0.5) goto a14;
    if(ABS(s-s1) < epsi) goto a20;
    s1 = s;
  }

  /* Good Old Iteration Process */

a14:

  dinter = (*sint) * 0.5;
  s      = 0.;

  for(i=0; i<maxhit; i++)
  {
    float x,y,z,rn,dr;

    x = shiftx + a[0] + s*(a[1] + s*(a[2] + s*a[3]));
    y = shifty + b[0] + s*(b[1] + s*(b[2] + s*b[3]));
    z = shiftz + c[0] + s*(c[1] + s*(c[2] + s*c[3]));
    rn = x*ortvec[0] + y*ortvec[1] + z*ortvec[2];
    dr = (d - rn) * drctn;
    if(ABS(dr) < epsi) goto a20;
    dinter = dinter * 0.5;
    if(dr < 0.) s = s - dinter;
    if(dr >=0.) s = s + dinter;
  }

  /* COMPUTE INTERSECTION IN Offset COORDINATES */

a20:

  xint[0] = shiftx+a[0] +s*(a[1]+s*(a[2]+s*a[3]))+pln[ip-1][0];
  xint[1] = shifty+b[0] +s*(b[1]+s*(b[2]+s*b[3]))+pln[ip-1][1];
  xint[2] = shiftz+c[0] +s*(c[1]+s*(c[2]+s*c[3]))+pln[ip-1][2];
  xint[3] = a[1] + s*(2.*a[2] + 3.*s*a[3]);
  xint[4] = b[1] + s*(2.*b[2] + 3.*s*b[3]);
  xint[5] = c[1] + s*(2.*c[2] + 3.*s*c[3]);
  xint[6] = 0.5*( vect[6] + vout[6] );
  i = 3;
  vunit_(&xint[3], &xint[3], &i);
  *sint = (*sint)*0.5 + s;

  return;
}
