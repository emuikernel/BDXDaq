h42374
s 00001/00000/00047
d D 1.2 03/04/17 16:52:58 boiarino 3 1
c *** empty log message ***
e
s 00000/00000/00000
d R 1.2 01/11/19 19:05:18 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 clas/cmon/sim.s/trigreadcalib.c
e
s 00047/00000/00000
d D 1.1 01/11/19 19:05:17 boiarino 1 0
c date and time created 01/11/19 19:05:17 by boiarino
e
u
U
f e 0
t
T
I 1

#include <stdio.h>
I 3
#include <stdlib.h>
E 3
#include "sdakeys.h"
SDAkeys sdakeys_;

/* returns TRG constants from SDA calibration file */

#define NWTRG 4

void
trigreadcalib_(int *runno, float *Tdel, float *Tjit, float *DL1, float *vflt)
{
  int i;
  float data[NWTRG];
  char *parm, fname[256];

  if((parm = (char *) getenv("CLON_PARMS")) == NULL)
  {
    printf("trigrealcalib: CLON_PARMS is not defined - exit.\n");
    exit(0);
  }
  sprintf(fname,"%s/TRLIB/%s",parm,sdakeys_.cbosfile);
  printf("trigreadcalib: reading calibration file >%s< for run # %d\n",fname,*runno);

  if(utGetFpackRecord(fname, "CTRG", NWTRG, data))
  {
    *Tdel = data[0];
    *Tjit = data[1];
    *DL1 =  data[2];
    *vflt = data[3];
  }

  return;
}

/* returns TRG constants from MAP (no MAP realy here) */

void
triggetcalib_(int *runno, float *Tdel, float *Tjit, float *DL1, float *vflt)
{
  *Tdel = 0.;
  *Tjit = 8.0;      /* Sigma of TRIG jitter [ns] */
  *DL1  = 1000.;    /* time delay for DC common TDC stop [ns] */
  *vflt = 29.97925;

  return;
}
E 1
