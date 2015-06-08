/*
   Purpose and Methods : This Routine is called at the begin of each RUN
                        

   Inputs  :
   Outputs :
   Controls:

   Library belongs: libsda.a

   Calls: dccalib

   Created    9-OCT-1995   Bogdan Niczyporuk
   C remake by Sergey Boyarinov

   Called by SDA_RUN
*/

#include <stdio.h>
#include <time.h>

#include "uttime.h"
#include "dclib.h"
#include "sdakeys.h"
SDAkeys sdakeys_;

void
dcbrun_(int *jw)
{
  dcbrun(jw);
  return;
}

void
dcbrun(int *jw)
{
  int ind, RunNo, EvtType, EvtClass, RunDate;
  char dtimes[25];

  /* Report Begin of Run */
  if((ind = etNlink(jw,"HEAD",0)) > 0)
  {
    RunNo    = jw[ind+1];
    /*RunNo = 14317;*/
    if(RunNo == 0) RunNo = sdakeys_.ltrig[0];
    RunDate  = jw[ind+3];
    EvtType  = jw[ind+4];
    EvtClass = jw[ind+6];
    {int i; for(i=0; i<25; i++) dtimes[i] = ' '; dtimes[24] = '\0';}
    getasciitime_(&RunDate,dtimes,strlen(dtimes));
    printf("\n\n BEGIN RUN#%6d  Type/Clas=%4d/%2d Date= %24.24s\n",
                                              RunNo,EvtType,EvtClass,dtimes);
  }
  else
  {
    RunNo = sdakeys_.ltrig[0];
  }

  return;
}




