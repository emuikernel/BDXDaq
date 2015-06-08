/*
   Purpose and Methods : This Routine is called at the End of each RUN
                        

   Inputs  :
   Outputs :
   Controls:

   Library belongs: libsda.a

   Calls: dclast

   Created    9-OCT-1995   Bogdan Niczyporuk
   C remake by Sergey Boyarinov

   Called by sda_anal
*/

#include <stdio.h>
#include <time.h>

#include "uttime.h"
#include "dclib.h"
#include "sdakeys.h"
SDAkeys sdakeys_;


void
dcerun_(int *jw)
{
  dcerun(jw);
  return;
}

void
dcerun(int *jw)
{
  int ind, icycle, uout,ierr, Version, RunNo, EvtNo, RunDate, EvtType,
      RocSt, EvtClass, EvtWt, istat, tmp, tmp1, tmp2;
  char dtimes[25];

  /* Report End of Run */
  RunNo = 0;
  ind = etNlink(jw,"HEAD",0);
  if(ind != 0)
  {
    Version  = jw[ind];
    RunNo    = jw[ind+1];
    if(RunNo == 0) RunNo = sdakeys_.ltrig[0];
    EvtNo    = jw[ind+2];
    RunDate  = jw[ind+3];
    EvtType  = jw[ind+4];
    RocSt    = jw[ind+5];
    EvtClass = jw[ind+6];
    EvtWt    = jw[ind+7];
  }

  tmp = 10;
  if(iucomp_("WDST",&sdakeys_.levb,&tmp,4) != 0)
  {
    etLdrop(jw,"E");
    etLctl(jw,"E=","HEAD");
    ind = etNcreate(jw,"HEAD",0,8,1);
    jw[ind]   = Version;
    jw[ind+1] = RunNo;
    jw[ind+2] = EvtNo ;
    jw[ind+3] = RunDate;
    jw[ind+4] = EvtType;
    jw[ind+5] = RocSt;
    jw[ind+6] = EvtClass;
    jw[ind+7] = EvtWt;
    uout = 2;
    fwbos_(jw,&uout,"E",&ierr,1);
  }

  if(RunNo == 0)
  {
    RunNo = sdakeys_.ltrig[0];
    getunixtime_(&RunDate);
  }
  {int i; for(i=0; i<25; i++) dtimes[i] = ' '; dtimes[24] = '\0';}
  getasciitime_(&RunDate,dtimes,strlen(dtimes));
  printf("\n\n END RUN#%6d  Type/Clas=%4d/%2d  Date= %24.24s\n",
                                       RunNo, EvtType, EvtClass, dtimes);

  /* Compute the average CPU time per one event */

  tmp = 1;
  if(iucomp_("BAT ",&sdakeys_.lmode,&tmp,4) != 0)
  {
    dc_statp1();
  }

  return;
}








