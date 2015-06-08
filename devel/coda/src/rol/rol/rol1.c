
/* rol1.c - first readout list for 'parallel' FASTBUS readout */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#include "circbuf.h"

/*****************************/
/* former 'crl' control keys */

/* readout list ROL1 */
#define ROL_NAME__ "ROL1"

/* polling */
#define POLLING_MODE




#define IRQ_SOURCE  0x10
#define TS_MODE  0           /* 0: External Mode   1: Trigger Supervisor Mode */
/* ts control 
#ifndef STANDALONE
#define TRIG_SUPV
#endif
*/




#define INIT_NAME rol1__init


#include "rol.h"
#include "../sfiDmaLib/dmaPList.h"

/* general readout */
#include "GEN_source.h"

/************************/
/************************/

extern int sfiNeedAck;
extern ROL_MEM_ID sfiIN, sfiOUT;

#include "coda.h"
#include "tt.h"

/*extern int TT_nev;*/

long decrement;     /* local decrement counter */
/*extern long nevent;*/ /* local event counter */
long mynev;


static void
__download()
{
  int poolsize;

#ifdef POLLING_MODE
  rol->poll = 1;
#endif

  sfiUserDownload();

  printf("INFO: User Download 1 Executed\n");

  return;
}

static void
__prestart()
{
  unsigned long jj, adc_id, sl;
  char *env;

  /* Clear some global variables etc for a clean start */
  CTRIGINIT;
  *(rol->nevents) = 0;

  /* init trig source GEN */
  GEN_INIT;

  /* Register a sync trigger source (up to 32 sources) */
  CTRIGRSS(GEN, 1, davetrig, davetrig_done); /* second arg=1 - what is that ? */

  rol->poll = 1;

  sfiUserPrestart();

  printf("INFO: User Prestart 1 executed\n");

  /* from parser (do we need that in rol2 ???) */
  *(rol->nevents) = 0;
  rol->recNb = 0;

  return;
}

static void
__pause()
{
  CDODISABLE(GEN,1,0);

  printf("INFO: User Pause 1 Executed\n");

  return;
}

static void
__go()
{
  char *env;

  rol->poll = 1;

  CDOENABLE(GEN,1,1);

  sfiUserGo();

  mynev = 0;

  printf("INFO: User Go 1 Executed\n");

  return;
}

static void
__end()
{
  int ii, total_count, rem_count;

  sfiUserEnd();

  CDODISABLE(GEN,1,0);

  /* we need to make sure all events taken by the
   SFI are collected from the sfiOUT queue */

  rem_count = sfiOUT->list.c;
  if(rem_count > 0)
  {
    printf("GEN End: %d events left on sfiOUT queue\n",rem_count);
    for(ii=0; ii<rem_count; ii++)
    {
	  __poll();
    }
    /*make sure nothing left*/
    rem_count = sfiOUT->list.c;
    printf("GEN End(second check): %d events left on sfiOUT queue\n",rem_count);    
  }
  else
  {
    printf("GEN End: sfiOUT queue Empty\n");
  }

  printf("INFO: User End 1 Executed\n");

  return;
}

void
davetrig(unsigned long EVTYPE, unsigned long EVSOURCE)
{
  long EVENT_LENGTH;
  int ii, len, len1, type, lock_key, *tmp;
  DANODE *outEvent; /*filled by 'parallel' readout, for us it is an input*/


  lock_key = intLock();

  outEvent = dmaPGetItem(sfiOUT);
  if(outEvent != NULL)
  {
    len = outEvent->length;

    CEOPEN1;

    len1 = outEvent->data[-1];

    /* save pointer */
    tmp = rol->dabufp;

    /* copy CODA fragment length */
    *rol->dabufp++ = len1;

    /* copy CODA fragment starting from second CODA header word */
    for(ii=0; ii<len1; ii++)
    {
      *rol->dabufp++ = outEvent->data[ii];
    }

    /* update 'nevents' field in CODA header */
    tmp[1] = (tmp[1]&0xFFFFFF00)+((EVENT_NUMBER)&0xFF);

    CECLOSE;

    dmaPFreeItem(outEvent);
  }
  else
  {
    logMsg("rol1: ERROR: outEvent == NULL\n",1,2,3,4,5,6);
  }

  /* check for full buffer condition */
  if(sfiNeedAck > 0)
  {
    if(dmaPEmpty(sfiIN)) logMsg("SHOULD NEVER BE HERE !!!\n",1,2,3,4,5,6);
    /*logMsg("done called, sfiNeedAck=%d\n",sfiNeedAck,2,3,4,5,6);*/
    sfiIntAck(IRQ_SOURCE, TS_MODE);
  }



  intUnlock(lock_key);


  return;
}

void
davetrig_done()
{
  /* check for full buffer condition */
  /* Sergey: must do it in previous routine, when intLock'ed ! */

  return;
}  

void
__done()
{
  /* from parser */
  poolEmpty = 0; /* global Done, Buffers have been freed */

  return;
}
  
static void
__status()
{
  return;
}  



/*



<   unsigned int auxVal;
98,113c96,98
<   / Total length of the event /
<   len = *dcpuCntlPtr->local++;
< 
<   / Trigger info comes from first data word in the event /
<   auxVal = *dcpuCntlPtr->local++;
< 
< #ifdef TRIG_SUPV
<   type     = ((auxVal&0x3c)>>2);
<   lateFail = ((auxVal&0x02)>>1);
<   syncFlag = ((auxVal&0x01));
< #else
<   type     = (auxVal&0xf);
<   lateFail = 0;
<   syncFlag = 0;
< #endif
< 
---
>       len = *dcpuCntlPtr->local++;
> 	
>       type = 1;
116,117c101
<       / Drop Aux data and get the rest of the event /
<       for(ii=0;ii<(len-1);ii++) {
---
>       for(ii=0;ii<len;ii++) {
124d107
< 




 */
