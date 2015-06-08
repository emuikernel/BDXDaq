h09733
s 00000/00000/00000
d R 1.2 01/11/19 19:12:18 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 clas/cmon/util.s/dcmvprod.c
e
s 00052/00000/00000
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
   Purpose and Methods : Symmetric Matrix by Vector: C(j) --> A(i,j)xB(i)

   Inputs  : N = Number of points (=< 36), M = number of parameters (=5)
   Outputs : C(M)
   Controls:

   Library belongs: libana.a

   Calls          : none

   Created   25-OCT-1990   Bogdan Niczyporuk
   C remake by Sergey Boyarinov

   Called by dctrfit
*/

#include <stdio.h>
#include "dclib.h"

void
dcmvprod_(float *a, float *b, float *c, int *m, int *n)
{
  dcmvprod(a, b, c, *m, *n);
  return;
}

void
dcmvprod(float *a, float *b, float *c, int m, int n)
{
  int i, j, k;

  for(i=0; i<m; i++)
  {
    c[i] = 0.0;
    k = i;
    for(j=0; j<n; j++)
    {
      c[i] += a[k] * b[j];
	  /*printf("ind=%3d%3d%3d\n",j,i,k);*/
      k += m;
    }
  }

  return;
}






E 1
