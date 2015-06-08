h18149
s 00011/00002/00095
d D 1.2 03/04/17 16:55:16 boiarino 3 1
c *** empty log message ***
e
s 00000/00000/00000
d R 1.2 01/11/19 19:06:03 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 clas/cmon/trlib.s/trinit.c
e
s 00097/00000/00000
d D 1.1 01/11/19 19:06:02 boiarino 1 0
c date and time created 01/11/19 19:06:02 by boiarino
e
u
U
f e 0
t
T
I 1

/* trinit.c - track reconstruction library initialization */

#include <stdio.h>
I 3
#include <time.h>

#include "uttime.h"
E 3
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
D 3
  getunixtime_(&dtime);
E 3
I 3
  getunixtime_((time_t *)&dtime);
E 3
  {int i; for(i=0; i<25; i++) dtimes[i] = ' '; dtimes[24] = '\0';}
D 3
  getasciitime_(&dtime,dtimes,strlen(dtimes));
E 3
I 3
  getasciitime_((time_t *)&dtime,dtimes,strlen(dtimes));
E 3

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
I 3
  /*printf("1\n");fflush(stdout);*/
E 3
  scinit(sdakeys_.ltrig[0]);

  /* tagger initialization */
I 3
/*printf("2\n");fflush(stdout);*/
E 3
  if(sdakeys_.zbeam[0] == 0.) tginit(sdakeys_.ltrig[0]);
I 3
/*printf("3\n");fflush(stdout);*/
E 3

  /* open input file */
  tmp=10;
  if(iucomp_("REVB",&sdakeys_.levb,&tmp,4) != 0)
  {
I 3
/*printf("4\n");fflush(stdout);*/
E 3
    strcpy(str,"OPEN EVBINPUT UNIT=1 FILE=\"");
    strcat(str,sdakeys_.inputfile);
    strcat(str,"\"");
    fparm_(str,strlen(str));
I 3
/*printf("5\n");fflush(stdout);*/
E 3
  }
  else
  {
    printf("trinit: input file is not set, so opening data source up to user.\n");
    fflush(stdout);
  }
I 3
/*printf("6\n");fflush(stdout);*/
E 3

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
E 1
