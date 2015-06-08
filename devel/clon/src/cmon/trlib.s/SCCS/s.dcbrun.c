h44506
s 00003/00000/00061
d D 1.2 03/04/17 16:53:24 boiarino 3 1
c *** empty log message ***
e
s 00000/00000/00000
d R 1.2 01/11/19 19:05:58 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 clas/cmon/trlib.s/dcbrun.c
e
s 00061/00000/00000
d D 1.1 01/11/19 19:05:57 boiarino 1 0
c date and time created 01/11/19 19:05:57 by boiarino
e
u
U
f e 0
t
T
I 1
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
I 3
#include <time.h>

#include "uttime.h"
E 3
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




E 1
