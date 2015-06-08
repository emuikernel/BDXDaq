h22322
s 00000/00000/00000
d R 1.2 01/11/19 19:12:18 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 clas/cmon/util.s/dcgcubs.c
e
s 00056/00000/00000
d D 1.1 01/11/19 19:12:17 boiarino 1 0
c date and time created 01/11/19 19:12:17 by boiarino
e
u
U
f e 0
t
T
I 1
/*

   Purpose and Methods : Calculates a cubic through P1,(-X,Y1),(X,Y2),P2
                         where Y2=-Y1
                         Y = A(1) + A(2)*X + A(3)*X**2 + A(4)*X**3
                         The coordinate system is assumed to be the cms
                         system of P1,P2.
   Inputs  : X,Y,D1,D2
   Outputs : A(4)
   Controls:

   Library belongs: libsda.a

   Calls: none

   Author:   H. Boerner
   Modified: 1-OCT-1990   Bogdan Niczyporuk


    Called by SDA_CYL, SDA_PLN

*/

#include <stdio.h>
#include "dclib.h"

void
dcgcubs_(float *x, float *y, float *d1, float *d2, float a[4])
{
  dcgcubs(*x, *y, *d1, *d2, a);
  return;
}

void
dcgcubs(float x, float y, float d1, float d2, float a[4])
{
  if(x == 0.)
  {
    a[0] = 0.0;
    a[1] = 1.0;
    a[2] = 0.0;
    a[3] = 0.0;
  }
  else
  {
    float fact;

    fact = (d1 - d2) * 0.25;
    a[0] = - 1.0 * fact * x;
    a[2] = fact / x;
    a[1] = (6.0 * y - (d1 + d2) * x) / (4.0 * x);
    a[3] = ((d1 + d2)*x - 2.0*y) / (4.0*x*x*x);
  }

  return;
}
E 1
