h05281
s 00000/00000/00000
d R 1.2 01/11/19 19:12:18 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 clas/cmon/util.s/dcmxinv.c
e
s 00161/00000/00000
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
   Purpose and Methods : Invert a symmetric matrix ARRAY and calculate
                         and calculate its determinant DET

   Inputs  : Norder, ARRAY(Norder,Norder)
   Outputs : Inverted ARRAY and its DET
   Controls:

   Library belongs: libana.a

   Calls          : none

   Created   25-OCT-1990   Bogdan Niczyporuk
   Source    "IBM System/360 Scientific Subroutine Package"

    Called by ana_trfit
*/

#include <stdio.h>
#include "dclib.h"

#define ABS(x) ((x) < 0 ? -(x) : (x))

void
dcmxinv_(float array[5][5], int *norder, float *det)
{
  dcmxinv(array, *norder, det);
  return;
}

void
dcmxinv(float array[5][5], int norder, float *det)
{
  int i, j, k, l, ik[10], jk[10];
  float amax, save;

  *det = 1.0;

  for(k=1; k<=norder; k++)
  {

    /* FIND LARGEST ELEMENT ARRAY(I,J) IN REST OF MATRIX */

    amax = 0.0;

a21:

    for(i=k; i<=norder; i++)
    {
      for(j=k; j<=norder; j++)
      {
        if( (ABS(amax)-ABS(array[j-1][i-1])) > 0 ) goto a30;
        amax=array[j-1][i-1];
        ik[k-1] = i;
        jk[k-1] = j;
a30:
        ;
      }
    }

    /* INTERCHANGE ROWS AND COLUMNS TO PUT AMAX IN ARRAY(K,K) */


    if(amax == 0.0)
    {
      *det = 0.0;
      return;;
    }

a41:

    i = ik[k-1];
    if(i<k) goto a21;
    else if(i==k) goto a51;
    else goto a43;

a43:

    for(j=0; j<norder; j++)
    {
      save = array[j][k-1];
      array[j][k-1] = array[j][i-1];
      array[j][i-1] = -save;
    }

a51:

    j = jk[k-1];
    if(j<k) goto a21;
    else if(j==k) goto a61;
    else goto a53;

a53:

    for(i=0; i<norder; i++)
    {
      save = array[k-1][i];
      array[k-1][i] = array[j-1][i];
      array[j-1][i] = -save;
    }

    /* ACCUMULATE ELEMENTS OF INVERSE MATRIX */

a61:

    for(i=1; i<=norder; i++)
    {
      if(i != k) array[k-1][i-1] = -array[k-1][i-1] / amax;
    }


    for(i=1; i<=norder; i++)
    {
      for(j=1; j<=norder; j++)
      {
        if(i!=k && j!=k) array[j-1][i-1] = array[j-1][i-1]
                           + array[k-1][i-1]*array[j-1][k-1];
      }
    }

    for(j=1; j<=norder; j++)
    {
      if(j != k)  array[j-1][k-1] = array[j-1][k-1] / amax;
    }
    array[k-1][k-1] = 1.0 / amax;
  }

  /* RESTORE ORDERING OF MATRIX */

  for(l=1; l<=norder; l++)
  {
    k = norder - l + 1;

    j = ik[k-1];
    if(j > k)
    {
      for(i=0; i<norder; i++)
      {
        save = array[k-1][i];
        array[k-1][i] = -array[j-1][i];
        array[j-1][i] = save;
      }
    }

    i = jk[k-1];
    if(i > k)
    {
      for(j=0; j<norder; j++)
      {
        save = array[j][k-1];
        array[j][k-1] = -array[j][i-1];
        array[j][i-1] = save;
      }
    }

  }

  return;
}


E 1
