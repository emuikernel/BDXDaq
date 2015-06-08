/************************************************************
 * 
 * gen_list.c - generic "Primary" Readout list routines
 *
 * Usage:
 *
 *    #include "gen_list.c"
 *
 *  then define the following routines:
 * 
 *    void rocDownload(); 
 *    void rocPrestart(); 
 *    void rocGo(); 
 *    void rocEnd(); 
 *
 * SVN: $Rev: 396 $
 *
 */

 
#define ROL_NAME__ "GEN_USER"
#ifndef MAX_EVENT_LENGTH
#define MAX_EVENT_LENGTH 10240
#endif
#ifndef MAX_EVENT_POOL
#define MAX_EVENT_POOL   400
#endif
/* POLLING_MODE */
#define POLLING___
#define POLLING_MODE
#define GEN_MODE
/* INIT_NAME should be defined by roc_### (maybe at compilation time - check Makefile-rol) */
#ifndef INIT_NAME
#define INIT_NAME gen_list__init
#endif
#include <rol.h>
#include "jvme.h"
#include <GEN_source.h>
#include "tirLib.h"
extern int bigendian_out;
extern unsigned int tirNeedAck;

void rocDownload();
void rocPrestart();
void rocGo();
void rocEnd();
int  getOutQueueCount();


static void __download()
{
  daLogMsg("INFO","Readout list compiled %s", DAYTIME);
#ifdef POLLING___
  rol->poll = 1;
#endif
  *(rol->async_roc) = 0; /* Normal ROC */
  {  /* begin user */
    bigendian_out=1;
    {/* inline c-code */
 
      /* Open the default VME windows */
      vmeOpenDefaultWindows();
/*       /\* Disable IRQ for VME Bus Errors *\/ */
/*       vmeDisableBERRIrq(); */

      rocDownload();
 
    }/*end inline c-code */
    daLogMsg("INFO","User Download Executed");

  }  /* end user */
} /*end download */     

static void __prestart()
{
  CTRIGINIT;
  *(rol->nevents) = 0;
  {  /* begin user */
    unsigned long jj, adc_id;
    daLogMsg("INFO","Entering User Prestart");

    GEN_INIT;
    CTRIGRSS(GEN,1,usrtrig,usrtrig_done);
    CRTTYPE(1,GEN,1);
    rocPrestart();
    daLogMsg("INFO","User Prestart Executed");

  }  /* end user */
  if (__the_event__) WRITE_EVENT_;
  *(rol->nevents) = 0;
  rol->recNb = 0;
} /*end prestart */     

static void __end()
{
  {  /* begin user */
    int ii, ievt, rem_count;
    int len, type, lock_key;
    DMANODE *outEvent;
    int oldnumber;

    rocEnd();
    CDODISABLE(GEN,1,0);
    {/* inline c-code */
 
      /* we need to make sure all events taken by the
	 VME are collected from the vmeOUT queue */

      rem_count = getOutQueueCount();
      if (rem_count > 0) {
	printf("gen_list End: %d events left on vmeOUT queue (will now de-queue them)\n",rem_count);
	/* This wont work without a secondary readout list (will crash EB or hang the ROC) */
	for(ii=0;ii<rem_count;ii++) {
	  __poll();
	}
	dmaPStatsAll();

      }else{
	printf("gen_list End: vmeOUT queue Empty\n");
      }
 
    }/*end inline c-code */
    daLogMsg("INFO","User End Executed");


  }  /* end user */
  if (__the_event__) WRITE_EVENT_;
} /* end end block */

static void __pause()
{
  {  /* begin user */
    CDODISABLE(GEN,1,0);
    daLogMsg("INFO","User Pause Executed");

  }  /* end user */
  if (__the_event__) WRITE_EVENT_;
} /*end pause */
static void __go()
{

  {  /* begin user */
    daLogMsg("INFO","Entering User Go");

    CDOENABLE(GEN,1,1);
    rocGo();
  }  /* end user */
  if (__the_event__) WRITE_EVENT_;
}

void usrtrig(unsigned long EVTYPE,unsigned long EVSOURCE)
{
  long EVENT_LENGTH;
  {  /* begin user */
    int ii, len, type, lock_key;
    DMANODE *outEvent;
    {/* inline c-code */
 

/*       INTLOCK; */
      outEvent = dmaPGetItem(vmeOUT);
      if(outEvent != NULL) {
	len = outEvent->length;
	type = outEvent->type;
	CEOPEN(type, BT_UI4);

	if(rol->dabufp != NULL) {
	  for(ii=0;ii<len;ii++) {
	    *rol->dabufp++ = LSWAP(outEvent->data[ii]);
	  }
	} else {
	  printf("gen_list: ERROR rol->dabufp is NULL -- Event lost\n");
	}

      /* Free up the trigger after freeing up the buffer - do it
	 this way to prevent triggers from coming in after freeing
	 the buffer and before the Ack */
	if(tirNeedAck > 0) 
	  {
	    outEvent->part->free_cmd = *tirIntAck;
	  }
	else
	  {
	    outEvent->part->free_cmd = NULL;
	  }

	CECLOSE;
	dmaPFreeItem(outEvent);  /* IntAck performed in here, if NeedAck */
      }else{
	logMsg("Error: no Event in vmeOUT queue\n",0,0,0,0,0,0);
      }
	

/*       INTUNLOCK; */

 
    }/*end inline c-code */
  }  /* end user */
} /*end trigger */

void usrtrig_done()
{
  {  /* begin user */
  }  /* end user */
} /*end done */

void __done()
{
  poolEmpty = 0; /* global Done, Buffers have been freed */
  {  /* begin user */
  }  /* end user */
} /*end done */

static void __status()
{
  {  /* begin user */
  }  /* end user */
} /* end status */

int
getOutQueueCount()
{
  if(vmeOUT) 
    return(vmeOUT->list.c);
  else
    return(0);
}
