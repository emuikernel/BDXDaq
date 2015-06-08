h43028
s 00008/00025/00321
d D 1.2 10/03/09 12:47:14 boiarino 2 1
c *** empty log message ***
e
s 00346/00000/00000
d D 1.1 06/05/26 16:10:47 boiarino 1 0
c date and time created 06/05/26 16:10:47 by boiarino
e
u
U
f e 0
t
T
I 1

/* rol1_vme.c - first readout list for 'parallel' VME readout */

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

D 2

/* need in one place in trigger_dispatch.h; must be undefined in rol1 */
#undef EVENT_MODE


E 2
/*???*/
/* 0: External Mode   1: Trigger Supervisor Mode */
#define TS_MODE  0
/*#define IRQ_SOURCE  0x10 - use TIR_SOURCE here*/


#define INIT_NAME rol1_vme__init


#include "rol.h"

/* general readout */
#include "GEN_source.h"

/************************/
/************************/

/* new
#include "vmeLib.h"
*/
#define TIR_SOURCE 1
extern unsigned int vmeNeedAck;



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

  vmeUserDownload();

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

D 2
  /* Register a physics event type (up to 16 types) */
  CRTTYPE(1, GEN, 1);
  CRTTYPE(2, GEN, 1);
  CRTTYPE(3, GEN, 1);
  CRTTYPE(4, GEN, 1);
  CRTTYPE(5, GEN, 1);
  CRTTYPE(6, GEN, 1);
  CRTTYPE(7, GEN, 1);
  CRTTYPE(8, GEN, 1);
  CRTTYPE(9, GEN, 1);

E 2
  rol->poll = 1;

  vmeUserPrestart();

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

  CDOENABLE(GEN,1,1);

  vmeUserGo();

  mynev = 0;

  printf("INFO: User Go 1 Executed\n");

  return;
}

static void
__end()
{
  int ii, total_count, rem_count;

  vmeUserEnd();

  CDODISABLE(GEN,1,0);

  /* we need to make sure all events taken by the
   VME are collected from the vmeOUT queue */

  rem_count = vmeUserGetCount();
  if(rem_count > 0)
  {
    printf("GEN End: %d events left on vmeOUT queue\n",rem_count);
    for(ii=0; ii<rem_count; ii++)
    {
	  __poll();
    }
    /*make sure nothing left*/
    rem_count = vmeUserGetCount();
    printf("GEN End(second check): %d events left on vmeOUT queue\n",rem_count);    
  }
  else
  {
    printf("GEN End: vmeOUT queue Empty\n");
  }

  printf("INFO: User End 1 Executed\n");

  return;
}

void
davetrig(unsigned long EVTYPE, unsigned long EVSOURCE)
{
  long EVENT_LENGTH;
  int ii, len, len1, type, lock_key, *tmp;
  DANODE *outEvent;


  lock_key = intLock();

  outEvent = dmaPGetItem(vmeOUT);
  if(outEvent != NULL)
  {
    len = outEvent->length;
	/*printf("len=%d\n",len);*/

    /*CEOPEN(type, BT_UI4);*/
    /* get new event from pool */
    if(__the_event__ == (DANODE *) 0 && rol->dabufp == NULL)
    {
      partGetItem(rol->pool,__the_event__);
      if(__the_event__ == (DANODE *) 0)
      {
  	    logMsg("TRIG ERROR: no pool buffer available\n",1,2,3,4,5,6);
        return;
	  }
	  rol->dabufp = (long *) &__the_event__->length;
      __the_event__->nevent = *(rol->nevents);
    }
    /* set pointer and fill CODA header */
D 2
	StartOfEvent[event_depth__++] = (rol->dabufp);
E 2
I 2
	StartOfEvent = (rol->dabufp);
E 2
	/*
    *(++(rol->dabufp)) = (syncFlag<<24) |
        ((type) << 16) | ((BT_UI4_ty) << 8) |
        (0xff & *(rol->nevents));
    (rol->dabufp)++;
    for(ii=0; ii<len; ii++)
    {
      *rol->dabufp++ = outEvent->data[ii];
    }
	*/

    len1 = outEvent->data[-1];
	/*printf("len1=%d\n",len1);*/

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
    tmp[1] = (tmp[1]&0xFFFFFF00)+((__the_event__->nevent)&0xFF);
	/*printf("tmp[1]=0x%08x\n",tmp[1]);*/


    /*CECLOSE;*/
    {
D 2
      event_depth__--;
      *StartOfEvent[event_depth__] = (long) (((char *) (rol->dabufp))
            - ((char *) StartOfEvent[event_depth__]));
	  if((*StartOfEvent[event_depth__] & 1) != 0)
E 2
I 2
      *StartOfEvent = (long) (((char *) (rol->dabufp))
            - ((char *) StartOfEvent));
	  if((*StartOfEvent & 1) != 0)
E 2
      {
        (rol->dabufp) = ((long *)((char *) (rol->dabufp))+1);
D 2
        *StartOfEvent[event_depth__] += 1;
E 2
I 2
        *StartOfEvent += 1;
E 2
      };
D 2
      if((*StartOfEvent[event_depth__] & 2) !=0)
E 2
I 2
      if((*StartOfEvent & 2) !=0)
E 2
      {
D 2
        *StartOfEvent[event_depth__] = *StartOfEvent[event_depth__] + 2;
E 2
I 2
        *StartOfEvent = *StartOfEvent + 2;
E 2
        (rol->dabufp) = ((long *)((short *) (rol->dabufp))+1);
      };
D 2
	  *StartOfEvent[event_depth__] = ( (*StartOfEvent[event_depth__]) >> 2) -1;
E 2
I 2
	  *StartOfEvent = ( (*StartOfEvent) >> 2) -1;
E 2
    }


    dmaPFreeItem(outEvent);
  }
  else
  {
    printf("rol1: ERROR: outEvent == NULL\n");
  }



  /* check for full buffer condition */
  if(vmeNeedAck > 0)
  {
    if(dmaPEmpty(vmeIN)) logMsg("SHOULD NEVER BE HERE !!!\n",1,2,3,4,5,6);
	/*printf("done called, vmeNeedAck=%d\n",vmeNeedAck);*/
    vmeIntAck(TIR_SOURCE, TS_MODE);
  }
  else
  {
	/*printf("vmeNeedAck=%d\n",vmeNeedAck)*/;
  }



  intUnlock(lock_key);


  return;
}
  
void
davetrig_done()
{
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
E 1
