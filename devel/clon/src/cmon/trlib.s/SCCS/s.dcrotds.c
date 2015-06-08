h53154
s 00000/00000/00000
d R 1.2 01/11/19 19:06:00 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 clas/cmon/trlib.s/dcrotds.c
e
s 00117/00000/00000
d D 1.1 01/11/19 19:05:59 boiarino 1 0
c date and time created 01/11/19 19:05:59 by boiarino
e
u
U
f e 0
t
T
I 1
/*
   Purpose and Methods : Vector transformation from Detector to Sector
                         Coordinate System and reverse { DCS <--> SCS }

   Inputs  : Vector d(3) in DCS  or s(3) & isec in SCS [call sda_rotsd]
   Outputs : Vector s(3) in SCS and sector number "isec"
   Controls: None

   Library belongs: libsda.a

   Calls: none

   Created    13-FEB-1996   Bogdan Niczyporuk
   C remake by Sergey Boyarinov
*/

#include <stdio.h>
#include <math.h>
#include "dclib.h"

#ifndef PI
#define PI 3.14159265358979323846
#endif
#define TWOPI  2.*PI
#define RADDEG 180./PI

/* sector's angle(degree): 0,60,120,180,240,300 */
static float sinsec[6] = { 0.000000000, 0.866025388, 0.866025388,
                           0.000000000,-0.866025388,-0.866025388 };
static float cossec[6] = { 1.000000000, 0.500000000,-0.500000000,
						  -1.000000000,-0.500000000, 0.500000000 };

/* Replacement and Rotation (input: d(3) ) {DCS --> SCS} */

void
dcrotds_(float d[9], float s[9], int *isec)
{
  dcrotds(d, s, isec);
  return;
}

void
dcrotds(float d[9], float s[9], int *isec)
{
  int i;
  float phi;

  /* Find sector number */

  phi = atan2(d[4],d[3]) * RADDEG;
  if(phi < 0.) phi += 360.;
  *isec = (phi + 30.)/60. + 1.;
  if(*isec > 6) *isec -= 6;

  /* Vertex {x,y,z} */

  s[0] = d[2];
  s[1] = d[0] * cossec[*isec-1] + d[1] * sinsec[*isec-1];
  s[2] =-d[0] * sinsec[*isec-1] + d[1] * cossec[*isec-1];

  /* Direction cosines {Cx,Cy,Cz} */

  s[3] = d[5];
  s[4] = d[3] * cossec[*isec-1] + d[4] * sinsec[*isec-1];
  s[5] =-d[3] * sinsec[*isec-1] + d[4] * cossec[*isec-1];

  /* {p,m,Q} */

  s[6] = d[6];
  s[7] = d[7];
  s[8] = d[8];

  return;
}


/* Replacement and Rotation (input: s(3) & isec )  {SCS --> DCS} */

void
dcrotsd_(float d[9], float s[9], int *isec)
{
  dcrotsd(d, s, isec);
  return;
}

void
dcrotsd(float d[9], float s[9], int *isec)
{
  int i;

  /* Vertex {x,y,z} */

  d[0] = s[1] * cossec[*isec-1] - s[2] * sinsec[*isec-1];
  d[1] = s[1] * sinsec[*isec-1] + s[2] * cossec[*isec-1];
  d[2] = s[0];

  /* Direction cosines {Cx,Cy,Cz} */

  d[3] = s[4] * cossec[*isec-1] - s[5] * sinsec[*isec-1];
  d[4] = s[4] * sinsec[*isec-1] + s[5] * cossec[*isec-1];
  d[5] = s[3];

  /* {p,m,Q} */

  d[6] = s[6];
  d[7] = s[7];
  d[8] = s[8];

  return;
}







E 1
