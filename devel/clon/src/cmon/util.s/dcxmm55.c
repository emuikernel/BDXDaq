/*
C-
C-   Purpose and Methods : Fast multiplication of 5X5 matreces: A*B -> C
C-                         (B and C may well be the same matrix).
C-
C-   Inputs  : A(25), B(25)
C-   Outputs : C(25)
C-   Controls:
C-
C-   Library belongs: libsda.a
C-
C-   Calls: none
C-
C-   Author: A. Haas (Freiburg University)
C-   Modified:  17-OCT-1990   Bogdan Niczyporuk
C-
C-
C-   Called by dctrmat
C-
*/

#include <stdio.h>
#include "dclib.h"

void
dcxmm55_(float a[25], float b[25], float c[25])
{
  int j1, j2, j3, j4, j5;
  float B1J,B2J,B3J,B4J,B5J;

  for(j5=4; j5<25; j5+=5)
  {
    B5J=b[j5];
    j4=j5-1;
    B4J=b[j4];
    j3=j4-1;
    B3J=b[j3];
    j2=j3-1;
    B2J=b[j2];
    j1=j2-1;
    B1J=b[j1];
    c[j1]=a[ 0]*B1J+a[ 5]*B2J+a[10]*B3J+a[15]*B4J+a[20]*B5J;
    c[j2]=a[ 1]*B1J+a[ 6]*B2J+a[11]*B3J+a[16]*B4J+a[21]*B5J;
    c[j3]=a[ 2]*B1J+a[ 7]*B2J+a[12]*B3J+a[17]*B4J+a[22]*B5J;
    c[j4]=a[ 3]*B1J+a[ 8]*B2J+a[13]*B3J+a[18]*B4J+a[23]*B5J;
    c[j5]=a[ 4]*B1J+a[ 9]*B2J+a[14]*B3J+a[19]*B4J+a[24]*B5J;
  }

  return;
}

