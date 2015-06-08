
/* trinit.c - track reconstruction library initialization */

#include <stdio.h>
#include <time.h>

#include "uttime.h"
#include "dclib.h"
#include "prlib.h"
#include "sdakeys.h"
SDAkeys sdakeys_;


void
trinit_()
{
  trinit();
}

void
trinit()
{
  int i, dtime, tmp;
  char dtimes[25], str[100];
  float r2r3cor[2][6];

  /* get job time */
  getunixtime_((time_t *)&dtime);
  {int i; for(i=0; i<25; i++) dtimes[i] = ' '; dtimes[24] = '\0';}
  getasciitime_((time_t *)&dtime,dtimes,strlen(dtimes));

  /* magnetic field initialization */
  dcminit_();

  /* initialize the descriptors for cuts */
  dc_statreset();
  dc_statinit_();

  /* Set simulation flag */
  sdakeys_.ifsim = 0;
  tmp = 10;
  if(iucomp_("SIM ",&sdakeys_.levb,&tmp,4) != 0) sdakeys_.ifsim = 1;
  printf("trinit: ifsim=%d\n",sdakeys_.ifsim);

  /* read geometry */
  if(sdakeys_.ifsim == 1)
  {
    sda_ginit_();
    /*sda_ginit1_();*/
  }
  else
  {
    tmp = 1;
    dc_make_dgeo_(&sdakeys_.ltrig[0], &tmp, r2r3cor);
    dc_geom_banks_();
    sda_ginit2_();
  }

  /* get calibration constants for the current run */
  printf("trinit: calibration run# %d\n",sdakeys_.ltrig[0]);
  sda_calib_(&sdakeys_.ltrig[0]);

  /* SC initialization */
  /*printf("1\n");fflush(stdout);*/
  scinit(sdakeys_.ltrig[0]);

  /* tagger initialization */
/*printf("2\n");fflush(stdout);*/
  if(sdakeys_.zbeam[0] == 0.) tginit(sdakeys_.ltrig[0]);
/*printf("3\n");fflush(stdout);*/

  /* open input file */
  tmp=10;
  if(iucomp_("REVB",&sdakeys_.levb,&tmp,4) != 0)
  {
/*printf("4\n");fflush(stdout);*/
    strcpy(str,"OPEN EVBINPUT UNIT=1 FILE=\"");
    strcat(str,sdakeys_.inputfile);
    strcat(str,"\"");
    fparm_(str,strlen(str));
/*printf("5\n");fflush(stdout);*/
  }
  else
  {
    printf("trinit: input file is not set, so opening data source up to user.\n");
    fflush(stdout);
  }
/*printf("6\n");fflush(stdout);*/

  /* open output file */
  tmp=10;
  if(iucomp_("WDST",&sdakeys_.levb,&tmp,4) != 0)
  {
    strcpy(str,"OPEN DSTOUTPUT UNIT=2 FILE=\"");
    strcat(str,sdakeys_.outputfile);
    strcat(str,"\" RECL=32760 SPLITMB=2047 WRITE STATUS=NEW");
    fparm_(str,strlen(str));
  }
  else
  {
    printf("trinit: output file is not set.\n");
    fflush(stdout);
  }

  return;
}
