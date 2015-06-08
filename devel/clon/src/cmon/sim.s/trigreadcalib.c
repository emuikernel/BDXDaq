
#include <stdio.h>
#include <stdlib.h>
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
