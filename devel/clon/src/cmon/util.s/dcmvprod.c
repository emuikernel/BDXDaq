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






