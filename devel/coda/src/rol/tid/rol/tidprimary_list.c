/*****************************************************************
 * 
 * tidprimary_list.c - "Primary" Readout list routines for tidprimary
 *
 * Usage:
 *
 *    #include "tidprimary_list.c"
 *
 *  then define the following routines:
 * 
 *    void rocDownload(); 
 *    void rocPrestart(); 
 *    void rocGo(); 
 *    void rocEnd(); 
 *    void rocTrigger();
 *
 * SVN: $Rev: 420 $
 *
 */


#define ROL_NAME__ "TIDPRIMARY"
#ifndef MAX_EVENT_LENGTH
/* Check if an older definition is used */
#ifdef MAX_SIZE_EVENTS
#define MAX_EVENT_LENGTH MAX_SIZE_EVENTS
#else
#define MAX_EVENT_LENGTH 10240
#endif /* MAX_SIZE_EVENTS */
#endif /* MAX_EVENT_LENGTH */
#ifndef MAX_EVENT_POOL
/* Check if an older definition is used */
#ifdef MAX_NUM_EVENTS
#define MAX_EVENT_POOL MAX_NUM_EVENTS
#else
#define MAX_EVENT_POOL   400
#endif /* MAX_NUM_EVENTS */
#endif /* MAX_EVENT_POOL */
/* POLLING_MODE */
#define POLLING___
#define POLLING_MODE
/* INIT_NAME should be defined by roc_### (maybe at compilation time - check Makefile-rol) */
#ifndef INIT_NAME
#warn "INIT_NAME undefined. Setting to tidprimary_list__init"
#define INIT_NAME tidprimary_list__init
#endif
#include <stdio.h>
#include <rol.h>
#include "jvme.h"
#include <TIDPRIMARY_source.h>
#include "tidLib.h"
extern int bigendian_out;

extern int tidDoAck;
extern int tidNeedAck;

#define ISR_INTLOCK INTLOCK
#define ISR_INTUNLOCK INTUNLOCK

/* ROC Function prototypes defined by the user */
void rocDownload();
void rocPrestart();
void rocGo();
void rocEnd();
void rocTrigger();
int  getOutQueueCount();

/* Asynchronous (to tidprimary rol) trigger routine, connects to rocTrigger */
void asyncTrigger();

/* Input and Output Partitions for VME Readout */
DMA_MEM_ID vmeIN, vmeOUT;


static void __download()
{
  int status;

  daLogMsg("INFO","Readout list compiled %s", DAYTIME);
#ifdef POLLING___
  rol->poll = 1;
#endif
  *(rol->async_roc) = 0; /* Normal ROC */

  bigendian_out=1;
 
  /* Open the default VME windows */
  vmeOpenDefaultWindows();

  /* Initialize memory partition library */
  dmaPartInit();

  /* Setup Buffer memory to store events */
  dmaPFreeAll();
  vmeIN  = dmaPCreate("vmeIN",MAX_EVENT_LENGTH,MAX_EVENT_POOL,0);
  vmeOUT = dmaPCreate("vmeOUT",0,0,0);

  /* Reinitialize the Buffer memory */
  dmaPReInitAll();
  dmaPStatsAll();

  /* Initialize VME Interrupt interface - use defaults */
  tidInit(TID_ADDR,TID_READOUT,0);

  /* Execute User defined download */
  rocDownload();
 
  daLogMsg("INFO","Download Executed");


} /*end download */     

static void __prestart()
{
  CTRIGINIT;
  *(rol->nevents) = 0;
  unsigned long jj, adc_id;
  daLogMsg("INFO","Entering Prestart");

  TIDPRIMARY_INIT;
  CTRIGRSS(TIDPRIMARY,1,usrtrig,usrtrig_done);
  CRTTYPE(1,TIDPRIMARY,1);

  /* If the TI Master, send a Clock and Trig Link Reset */
#ifdef TID_MASTER
  tidClockReset();
  taskDelay(2);
  tidTrigLinkReset();
#endif

  /* Execute User defined prestart */
  rocPrestart();

  /* If the TI Master, send a Sync Reset 
     - required by FADC250 after it is enabled */
#ifdef TID_MASTER
  tidSyncReset();
#endif

  /* Connect User Trigger Routine */
  tidIntConnect(TID_INT_VEC,asyncTrigger,0);

  tidStatus();

  daLogMsg("INFO","Prestart Executed");

  if (__the_event__) WRITE_EVENT_;
  *(rol->nevents) = 0;
  rol->recNb = 0;
} /*end prestart */     

static void __end()
{
  int ii, ievt, rem_count;
  int len, type, lock_key;
  DMANODE *outEvent;
  int oldnumber;
  int iwait=0;
  int blocksLeft=0;

  /* Disable Triggers */
#ifdef TID_MASTER
  tidDisableTriggerSource(1);
#endif

  /* Before disconnecting... wait for blocks to be emptied */
  blocksLeft=tidBReady();
  if(blocksLeft)
    {
      printf("... end:  Blocks left on the TID (%d)\n",blocksLeft);
      while(iwait < 100)
	{
	  if(blocksLeft<=0)
	    break;
	  blocksLeft=tidBReady();
	  iwait++;
	}
      printf("... end:  Blocks left on the TID (%d)\n",blocksLeft);
    }
  tidStatus();
  tidIntDisable();
  tidIntDisconnect();

  /* Execute User defined end */
  rocEnd();

  CDODISABLE(TIDPRIMARY,1,0);
 
  /* we need to make sure all events taken by the
     VME are collected from the vmeOUT queue */

  rem_count = getOutQueueCount();
  if (rem_count > 0) 
    {
      printf("tidprimary_list End: %d events left on vmeOUT queue (will now de-queue them)\n",rem_count);
      /* This wont work without a secondary readout list (will crash EB or hang the ROC) */
      for(ii=0;ii<rem_count;ii++) 
	{
	  __poll();
	}
    }
  else
    {
      printf("tidprimary_list End: vmeOUT queue Empty\n");
    }
      
  daLogMsg("INFO","End Executed");

  if (__the_event__) WRITE_EVENT_;
} /* end end block */

static void __pause()
{
  CDODISABLE(TIDPRIMARY,1,0);
  daLogMsg("INFO","Pause Executed");
  
  if (__the_event__) WRITE_EVENT_;
} /*end pause */

static void __go()
{
  daLogMsg("INFO","Entering Go");
  
  CDOENABLE(TIDPRIMARY,1,1);
  rocGo();
  
  tidIntEnable(0); 
  
  if (__the_event__) WRITE_EVENT_;
}

#define FA_DATA_TYPE_DEFINE       0x80000000
#define FA_DATA_TYPE_MASK         0x78000000
#define FA_DATA_BLOCK_HEADER      0x00000000
#define FA_DATA_BLOCK_TRAILER     0x08000000

void usrtrig(unsigned long EVTYPE,unsigned long EVSOURCE)
{
  long EVENT_LENGTH;
  int ii, len, data, type=0, lock_key;
  int syncFlag=0, lateFail=0;
  unsigned int event_number=0;
  DMANODE *outEvent;

  outEvent = dmaPGetItem(vmeOUT);
  if(outEvent != NULL) 
    {
      len = outEvent->length;
      type = outEvent->type;
      event_number = outEvent->nevent;

      CEOPEN(type, BT_UI4);

      if(rol->dabufp != NULL) 
	{
	  for(ii=0;ii<len;ii++) 
	    {
	      *rol->dabufp++ = LSWAP(outEvent->data[ii]);
	    }
	}
      else 
	{
	  printf("tidprimary_list: ERROR rol->dabufp is NULL -- Event lost\n");
	}

      /* Free up the trigger after freeing up the buffer - do it
	 this way to prevent triggers from coming in after freeing
	 the buffer and before the Ack */
      if(tidNeedAck > 0) 
	{
	  outEvent->part->free_cmd = *tidIntAck;
	}
      else
	{
	  outEvent->part->free_cmd = NULL;
	}
      
      CECLOSE;
      dmaPFreeItem(outEvent); /* IntAck performed in here, if NeedAck */
    }
  else
    {
      logMsg("Error: no Event in vmeOUT queue\n",0,0,0,0,0,0);
    }
  
  
  
} /*end trigger */

void asyncTrigger()
{
  int intCount=0;
  int length,size;
  unsigned int tirval;

  intCount = tidGetIntCount();

  /* grap a buffer from the queue */
  GETEVENT(vmeIN,intCount);
  if(the_event->length!=0) 
    {
      printf("Interrupt Count = %d\t",intCount);
      printf("the_event->length = %d\n",the_event->length);
    }

  the_event->type = 1;
  
  /* Execute user defined Trigger Routine */
  rocTrigger();

  /* Put this event's buffer into the OUT queue. */
  PUTEVENT(vmeOUT);
  /* Check if the event length is larger than expected */
  length = (((int)(dma_dabufp) - (int)(&the_event->length))) - 4;
  size = the_event->part->size - sizeof(DMANODE);

  if(length>size) 
    {
      printf("rocLib: ERROR: Event length > Buffer size (%d > %d).  Event %d\n",
	     length,size,the_event->nevent);
    }
  if(dmaPEmpty(vmeIN)) 
    {
      printf("WARN: vmeIN out of event buffers.\n");
      tidDoAck = 0;
      tidNeedAck = 1;
    }


}

void usrtrig_done()
{
} /*end done */

void __done()
{
  poolEmpty = 0; /* global Done, Buffers have been freed */
} /*end done */

static void __status()
{
} /* end status */

int
getOutQueueCount()
{
  if(vmeOUT) 
    return(vmeOUT->list.c);
  else
    return(0);
}

/* This routine is automatically executed just before the shared libary
   is unloaded.

   Clean up memory that was allocated 
*/
__attribute__((destructor)) void end (void)
{
  printf("ROC Cleanup\n");
  dmaPFreeAll();
  vmeCloseDefaultWindows();
}
