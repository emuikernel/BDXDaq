/*----------------------------------------------------------------------------*
 *  Copyright (c) 1991, 1992  Southeastern Universities Research Association, *
 *                            Continuous Electron Beam Accelerator Facility   *
 *                                                                            *
 *    This software was developed under a United States Government license    *
 *    described in the NOTICE file included as part of this distribution.     *
 *                                                                            *
 * CEBAF Data Acquisition Group, 12000 Jefferson Ave., Newport News, VA 23606 *
 *      heyes@cebaf.gov   Tel: (804) 249-7030    Fax: (804) 249-7363          *
 *----------------------------------------------------------------------------*
 * Discription: follows this header.
 *
 * Author:
 *	Graham Heyes
 *	CEBAF Data Acquisition Group
 *
 * Revision History:
 *      $Log: rol.h,v $
 *      Revision 1.5  1998/07/15 18:38:35  abbottd
 *       Return status on partCreate calls.
 *
 *      Revision 1.4  1998/01/23 15:27:53  heyes
 *      commit LINUX changes for Carl
 *
 *      Revision 1.3  1997/12/03 15:06:40  heyes
 *      support for multithread
 *
 *      Revision 1.2  1997/05/19 18:51:22  abbottd
 *      various changes and enhancements over several months
 *
 *      Revision 1.1.1.1  1996/08/21 19:20:56  heyes
 *      Imported sources
 *
*	  Revision 1.2  94/03/16  11:00:33  11:00:33  heyes (Graham Heyes)
*	  auto replace
*	  
*	  Revision 1.1  94/03/16  10:01:10  10:01:10  heyes (Graham Heyes)
*	  Initial revision
*	  
 *
 *----------------------------------------------------------------------------*/
#ifndef _ROLDEFINED
#define _ROLDEFINED

#define EIEIO    __asm__ volatile ("eieio")
#define SYNC     __asm__ volatile ("sync")

typedef struct semaphore *SEM_ID;

static void __download ();
static void __prestart ();
static void __end ();
static void __pause ();
static void __go ();
static void __done ();
static void __status ();

static int theIntHandler ();

/* Include the memory partition structures and routines */
#include "libpart.h"

/* The following structure is used per readout list to store it's global data */
#include "rolInt.h"     

/* Define external variables and internal rol structures */
extern ROLPARAMS rolP;
static rolParam rol;

/* Identify external variables from roc_component and VxWorks kernel */

extern int global_env[];
extern long global_env_depth;
extern char *global_routine[100];

extern long data_tx_mode;

#ifdef VXWORKSPPC
extern int cacheInvalidate();
extern int cacheFlush();
#endif

static int syncFlag;
static int lateFail;




/************************************************************************/
/************************************************************************/
/*#include "BankTools.h"*/

/* CBOPEN  == OPEN  BANK <bank_name> of <bank_type>  */
/* CBCLOSE == CLOSE BANK <bank_len>  of <bank_type>  */

#ifndef _Bank_Tools_h
#define _Bank_Tools_h

/* Event types : */
#define EV_SYNC     16
#define EV_PRESTART 17
#define EV_GO       18
#define EV_PAUSE    19
#define EV_END      20


#define EV_BANK_HDR  0x00000100
#define EV_BAD       0x10000000

#define PHYS_BANK_HDR(t,e) (unsigned long)((((t)&0xf)<<16) | \
					     ((e)&0xff) | \
					     EV_BANK_HDR)

#define CTL_BANK_HDR(t) (unsigned long)((((t)&0xffff)<<16) | \
					0x000001CC)
     
#define IS_BANK(b) (((unsigned long) (b) && EV_BANK_HDR)==EV_BANK_HDR)
     
#define DECODE_BANK_HEADER(b,t,e) { t = (b[1]>>16)&0xffff;\
				      e = b[1]&0xff;}    
     
#define EV_BANK_ID 0xc0010100
#define EV_HDR_LEN 4


/* Event type source */
static int EVENT_type;


/* Param : */

#define BT_BANK_ty 0x10
#define BT_UB1_ty  0x07
#define BT_UI2_ty  0x05
#define BT_UI4_ty  0x01

#define BC_RUNCONTROL 0xCC
#define CURRENT_TIME time(0)
#define RUN_NUMBER rol->runNumber
#define RUN_TYPE   rol->runType
#define EVENT_NUMBER *(rol->nevents)

#define MAX_DEPTH__ 32 

long *StartOfEvent[MAX_DEPTH__], event_depth__, *StartOfUEvent;

/* Macro : */
#ifdef VXWORKS
#define LOGIT logMsg
#else
#define LOGIT printf
#endif

/* moved here from 'mempart.h' */
#define partGetItem(p,i) \
{ \
  listGet(&(p->list),i); /* returns 'i' */ \
}
#define partFreeItem(pItem) \
{ \
  if ((pItem)->part == 0) \
  { \
    free(pItem); \
    pItem = 0; \
  } \
  else \
  { \
    listAdd (&pItem->part->list, pItem); \
  } \
}

#define NEWEVENT \
{ \
  if(__the_event__ == (DANODE *) 0 && rol->dabufp == NULL) \
  { \
    partGetItem(rol->pool,__the_event__); \
    if(__the_event__ == (DANODE *) 0) \
    { \
      LOGIT("TRIG ERROR: no pool buffer available\n"); \
      return; \
    } \
    rol->dabufp = (long *) &__the_event__->length; \
    if(input_event__) \
    { \
      __the_event__->nevent = input_event__->nevent; \
    } \
    else \
    { \
      __the_event__->nevent = *(rol->nevents); \
    } \
  } \
}

#define COPYEVENT \
{ \
  if(__the_event__ == (DANODE *) 0) \
  { \
    partGetItem(rol->pool,__the_event__); \
    rol->dabufp = (long *) &__the_event__->length; \
    if(input_event__) \
    { \
      int jj, len; \
      len = *(rol->dabufpi); \
      __the_event__->nevent = input_event__->nevent; \
      for(jj=0; jj<=len; jj++) \
        *(rol->dabufp)++ = *(rol->dabufpi)++; \
    } \
    else \
    { \
      __the_event__->nevent = *(rol->nevents); \
    } \
  } \
}


#define PASSEVENT {if(input_event__) \
		     { \
			 if (rol->output) { \
			    listAdd(&(rol->output->list),input_event__); \
			 } else {  \
			    partFreeItem(input_event__);\
				 } \
			 input_event__ = (DANODE *) 0; \
		     } \
		 }


#define USEREVENT {if(__user_event__ == (DANODE *) 0) \
		    { \
			partGetItem(rol->pool,__user_event__); \
			if(__user_event__ == (DANODE *) 0) { \
			     LOGIT("TRIG ERROR: no pool buffer available\n"); \
                              return; \
			   } \
			rol->dabufp = (long *) &__user_event__->length; \
			__user_event__->nevent = -1; \
		    } \
		}


/* - cbopen ---------------------------------------------------------- 
   crl	: open bank <bank_name> of <bank_type>
   - <bank_name> : converted in bnum through Bank_Dic_ID 
   - <bank_type> : UI2 , UI4, UB1 
   
   example	: open bank 0x1234 of UI2 
   
   call	: cbopen (long bnum, long btype); 
   
   Function: 
   open a CODA bank with CODA Bank Header Format 
   leaves (rol->dabufp) pointing to ready next address 
   keep pointer to length of bank in GblTopBp for length adjustment 
   - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

#define CBOPEN(bnum, btype, code) {\
				     long *StartOfBank; \
				     StartOfBank = (rol->dabufp); \
				       *(++(rol->dabufp)) = (((bnum) << 16) | (btype##_ty) << 8) | (code);\
					 ((rol->dabufp))++;
				   
#define CEOPEN(bnum, btype) \
{ \
  NEWEVENT; \
  StartOfEvent[event_depth__++] = (rol->dabufp); \
  if(input_event__) \
  { \
    *(++(rol->dabufp)) = ((bnum) << 16) | ((btype##_ty) << 8) | (0xff & (input_event__->nevent));\
  } \
  else \
  { \
    *(++(rol->dabufp)) = (syncFlag<<24) | ((bnum) << 16) | ((btype##_ty) << 8) | (0xff & *(rol->nevents));\
  } \
  ((rol->dabufp))++; \
}


#define UEOPEN(bnum, btype, code) {\
			       USEREVENT; \
			       StartOfUEvent = (rol->dabufp); \
			       *(++(rol->dabufp)) = (((bnum) << 16) | (btype##_ty) << 8) | (0xff & code);\
			       ((rol->dabufp))++;}
				     
/* - cbclose --------------------------------------------------------- 
 crl	: close bank <buff_len> of <bank_type> 
  - <bank_type> : UI2 , UI4, UB1 
  - <buff_len>  : number of "bank_type" word written 

 example	: close bank

  Call	: cbclose (btype, &buflen); 

  Function: 
    Close a CODA bank created by "cbopen" 
    leaves the (rol->dabufp) pointing to next empty long after bank 
    returns the actual length of that bank 
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

#define CBCLOSE \
         *StartOfBank = (long) (((char *) (rol->dabufp)) - ((char *) StartOfBank));\
	 if ((*StartOfBank & 1) != 0) {\
                        (rol->dabufp) = ((long *)((char *) (rol->dabufp))+1);\
                        *StartOfBank += 1;\
                      };\
         if ((*StartOfBank & 2) !=0) {\
                        *StartOfBank = *StartOfBank + 2;\
                        (rol->dabufp) = ((long *)((short *) (rol->dabufp))+1);;\
                      };\
	 *StartOfBank = ( (*StartOfBank) >> 2) - 1;};

#define CECLOSE \
{ \
  event_depth__--; \
  *StartOfEvent[event_depth__] = \
  (long) (((char *) (rol->dabufp)) - ((char *) StartOfEvent[event_depth__])); \
  if((*StartOfEvent[event_depth__] & 1) != 0) \
  { \
    (rol->dabufp) = ((long *)((char *) (rol->dabufp))+1); \
    *StartOfEvent[event_depth__] += 1; \
  } \
  if((*StartOfEvent[event_depth__] & 2) !=0) \
  { \
    *StartOfEvent[event_depth__] = *StartOfEvent[event_depth__] + 2; \
    (rol->dabufp) = ((long *)((short *) (rol->dabufp))+1); \
  } \
  *StartOfEvent[event_depth__] = ( (*StartOfEvent[event_depth__]) >> 2) - 1; \
}

#define UECLOSE \
{ \
*StartOfUEvent = (long) (((char *) (rol->dabufp)) - ((char *) StartOfUEvent));\
	 if ((*StartOfUEvent & 1) != 0) {\
                        (rol->dabufp) = ((long *)((char *) (rol->dabufp))+1);\
                        *StartOfUEvent += 1;\
                      };\
         if ((*StartOfUEvent & 2) !=0) {\
                        *StartOfUEvent = *StartOfUEvent + 2;\
                        (rol->dabufp) = ((long *)((short *) (rol->dabufp))+1);;\
                      };\
	 *StartOfUEvent = ( (*StartOfUEvent) >> 2) - 1;\
  /* NOW write the Event onto the Output Queue */ \
	 if (rol->output) { \
	    listAdd(&(rol->output->list),__user_event__); \
	 } else {  \
	    partFreeItem(__user_event__);\
	 } \
	 __user_event__ = (DANODE *) 0; \
	 };


#define CBWRITE32(dat) {*(rol->dabufp)++ = (dat);}
#define CBWRITE16(dat) {*((short *) rol->dabufp)++ = (dat);}
#define CBWRITE8(dat)  {*((char  *)rol->dabufp)++ = (dat);}

#define CBPOINTER (rol->dabufp)
#endif

/************************************************************************/
/************************************************************************/




/* Disable Readout List error Recovery for Now */
/*#define RECOVER {if (global_env_depth>=0) global_env_depth--; \
  longjmp(global_env[global_env_depth+1],global_code);} */
#define RECOVER  return




/************************************************************************/
/************************************************************************/
/*#include "trigger_dispatch.h"*/


/* Trigger dispatch Macros for multiple trigger sources in CODA */

#ifndef __TRIGGER_DISPATCH
#define __TRIGGER_DISPATCH

/* private global */

#define MAXSRC    32
#define MAXETY    16

static unsigned char dispatch_busy; 
static int trigId;
static int poolEmpty;
static unsigned long theEvMask, currEvMask, currType, evMasks[MAXETY];

#ifdef VXWORKS
static int intLockKey;

static VOIDFUNCPTR wrapperGenerator;
static FUNCPTR trigRtns[MAXSRC];
static FUNCPTR syncTRtns[MAXSRC];
static FUNCPTR doneRtns[MAXSRC];
static FUNCPTR ttypeRtns[MAXSRC];

#else

static VOIDFUNCPTR1 wrapperGenerator;
static FUNCPTR1 trigRtns[MAXSRC];
static FUNCPTR2 syncTRtns[MAXSRC];
static FUNCPTR3 doneRtns[MAXSRC];
static FUNCPTR4 ttypeRtns[MAXSRC];

#endif

static unsigned long Tcode[MAXSRC];
static DANODE *__the_event__, *input_event__, *__user_event__;

/* 
   Clear some global variables etc for a clean start.
   Should be done at Prestart.
*/

#define CTRIGINIT \
{ \
    dispatch_busy = 0; \
    memset((char *) evMasks, 0, sizeof(evMasks)); \
    memset((char *) syncTRtns, 0, sizeof(syncTRtns)); \
    memset((char *) ttypeRtns, 0, sizeof(ttypeRtns)); \
    memset((char *) Tcode, 0, sizeof(Tcode)); \
    wrapperGenerator = 0; \
    theEvMask = 0; \
    currEvMask = 0; \
    trigId = 1; \
    poolEmpty = 0; \
    input_event__ = (DANODE *) 0; /* input events pool */ \
    __the_event__ = (DANODE *) 0; /* output events pool */ \
}

/*
   Register a procedure to do the wrapping NULL = don't wrap
   */

#define CRWRAPP(p) {wrapperGenerator = (p);}



#ifdef VXWORKS
/* 
   Register an async trigger source, (up to 32 sources).
   */

#define CTRIGRSA(source,code,handler,done) \
{ \
  void handler(); \
  void done(); \
  trigRtns[trigId]  = (FUNCPTR) (handler) ; \
  doneRtns[trigId]  = (FUNCPTR) (done) ; \
  Tcode[trigId]     = (code) ; \
  ttypeRtns[trigId] = (FUNCPTR) source##_TTYPE ; \
  source##_ASYNC(code,trigId); \
  printf("CTRIGRSA: set handler and done, trigId=%d, code=%d\n",trigId,code); \
  printf("CTRIGRSA: 0x%08x 0x%08x 0x%08x 0x%08x\n", \
    doneRtns[trigId],trigRtns[trigId],Tcode[trigId],ttypeRtns[trigId]); \
  trigId++; \
}

/* 
   Register an sync trigger source, (up to 32 sources).
   */

#define CTRIGRSS(source,code,handler,done) \
{ \
  void handler(); \
  void done(); \
  trigRtns[trigId]  = (FUNCPTR) (handler) ; \
  doneRtns[trigId]  = (FUNCPTR) (done) ; \
  syncTRtns[trigId] = (FUNCPTR) source##_TEST ; \
  Tcode[trigId]     = (code) ; \
  ttypeRtns[trigId] = (FUNCPTR) source##_TTYPE ; \
  source##_SYNC(code,trigId); \
  printf("CTRIGRSS: set handler and done, trigId=%d, code=%d\n",trigId,code); \
  printf("CTRIGRSS: 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x\n", \
    doneRtns[trigId],trigRtns[trigId],syncTRtns[trigId],Tcode[trigId],ttypeRtns[trigId]); \
  trigId++; \
}

#else

#define CTRIGRSA(source,code,handler,done) \
{ \
  trigRtns[trigId]  = (FUNCPTR1) (handler); \
  doneRtns[trigId]  = (FUNCPTR3) (done); \
  Tcode[trigId]     = (code); \
  ttypeRtns[trigId] = (FUNCPTR4) source##_TTYPE; \
  source##_ASYNC(code,trigId); \
  printf("CTRIGRSA: set handler and done, trigId=%d, code=%d\n",trigId,code); \
  printf("CTRIGRSA: 0x%08x 0x%08x 0x%08x 0x%08x\n", \
    doneRtns[trigId],trigRtns[trigId],Tcode[trigId],ttypeRtns[trigId]); \
  trigId++; \
}

/* 
   Register an sync trigger source, (up to 32 sources).
   */

#define CTRIGRSS(source,code,handler,done) \
{ \
  trigRtns[trigId]  = (FUNCPTR1) (handler) ; \
  doneRtns[trigId]  = (FUNCPTR3) (done) ; \
  syncTRtns[trigId] = (FUNCPTR2) source##_TEST ; \
  Tcode[trigId]     = (code) ; \
  ttypeRtns[trigId] = (FUNCPTR4) source##_TTYPE ; \
  source##_SYNC(code,trigId); \
  printf("CTRIGRSS_##source: set handler and done, trigId=%d, code=%d\n",trigId,code); \
  printf("CTRIGRSS_##source: 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x\n", \
    doneRtns[trigId],trigRtns[trigId],syncTRtns[trigId],Tcode[trigId],ttypeRtns[trigId]); \
  trigId++; \
}

#endif

/* 
   Register a physics event type, (up to 16 types) .
   */

#define CRTTYPE(type,source,code) \
{ \
  evMasks[type] |= (1<<(source##_GETID(code))); \
}

#ifdef VXWORKS
#define LOCKINTS intLockKey = intLock();
#else
#define LOCKINTS
#endif

#ifdef VXWORKS
#define UNLOCKINTS intUnlock(intLockKey);
#else
#define UNLOCKINTS
#endif

#define CDOPOLL {cdopolldispatch();}

/* Define/Set trigger type macros */
#define CETREAD EVENT_type
#define CETWRITE(tt) EVENT_type = (tt);

#define CDOINIT(source) source##_INIT
#define CDOENABLE(source,code,val) source##_ENA(code,val)
#define CDODISABLE(source,code,val) source##_DIS(code,val)
#define CDOACK(source,code,val) source##_ACK(code,val)
#define CDOSTART(source,val) source##_START(val)
#define CDOSTOP(source,val) source##_STOP(val)

#define WRITE_EVENT_ \
  rol->dabufp = NULL; \
  /* if there is output, add event '__the_event__' to the pool */ \
  if(__the_event__) \
  { \
    if(rol->output) \
    { \
      listAdd(&(rol->output->list), __the_event__); \
	} \
    else \
    { \
      if ((__the_event__)->part == 0) \
      { \
        free(__the_event__); \
        __the_event__ = 0; \
      } \
      else \
      { \
        listAdd(&__the_event__->part->list, __the_event__); \
      } \
	} \
	__the_event__ = (DANODE *) 0; \
  } \
  /* if there is input, add event 'input_event__' to the pool */ \
  if(input_event__) \
  { \
    if ((input_event__)->part == 0) \
    { \
      free(input_event__); \
      input_event__ = 0; \
    } \
    else \
    { \
      listAdd(&input_event__->part->list, input_event__); \
    } \
    input_event__ = (DANODE *) 0; \
  } \
  /* */ \
  if(currEvMask) \
  { \
    int ix; \
    for(ix=0; ix < trigId; ix++) \
    { \
	  if (currEvMask & (1<<ix)) (*doneRtns[ix])(); \
	} \
    if (rol->pool->list.c) \
    { \
      currEvMask = 0; \
	  __done(); \
	  rol->doDone = 0; \
    } \
    else \
    { \
	  poolEmpty = 1; \
	  rol->doDone = 1; \
    } \
  }




/* Actually dispatch the triggers to the correct routines */

static void
cdodispatch()
{
  unsigned long theType,theSource;
  int ix, go_on;
  DANODE *theNode;

  LOGIT("cdodispatch reached\n");fflush(stdout);

  dispatch_busy = 1;
  go_on = 1;

#ifdef PROC_MODE

  LOGIT("cdodispatch: PROC_MODE reached\n");fflush(stdout);

  LOGIT("cdodispatch: PROC_MODE 1\n");fflush(stdout);
  theType = 1;
  LOGIT("cdodispatch: PROC_MODE 2\n");fflush(stdout);
  theSource = 1;
  theNode->reader = trigRtns[theSource];
  LOGIT("cdodispatch: PROC_MODE 3 (%d %d %d ->0x%08x)\n",theType,theSource,Tcode[theSource],theNode->reader);fflush(stdout);

  (*theNode->reader)(theType, Tcode[theSource]);
  LOGIT("cdodispatch: PROC_MODE 4\n");fflush(stdout);

  LOGIT("cdodispatch: PROC_MODE done\n");fflush(stdout);

#else

  LOGIT("cdodispatch: NOT PROC_MODE\n");fflush(stdout);

  /* While there are events */
  while((rol->dispQ->list.c) && (go_on))
  {
    /* dequeue off head */
    listGet(&rol->dispQ->list, theNode);
    theType = theNode->type;
    theSource = theNode->source;
    if(theEvMask)
    { 
      /* We are already in an event */
      /* is this source required for this event ? */
      if((theEvMask & (1<<theSource)) && (theType == currType))
      {
        /* yes */
        /* remember we have seen it! */
        theEvMask = theEvMask & ~(1<<theSource);

        /* call the routine */
        UNLOCKINTS;
        (*theNode->reader)(theType, Tcode[theSource]);
        LOCKINTS;

        /* done with this trigger */
        if(theNode)
        {
          /*partFreeItem(theNode);*/
          if(!theEvMask)
          {
            if(wrapperGenerator) CECLOSE; /* if we called a wrap routine close the bank */
            WRITE_EVENT_;
          }
        }
      }
      else
      {
        /* no so requeue on tail*/
        listAdd(&rol->dispQ->list, theNode);
        go_on = 0;
      }
    }
    else
    { 
      /* We are not in an event */
      /* get a new buffer... */

      if((1<<theSource) & evMasks[theType])
      {
        currEvMask = theEvMask = evMasks[theType];
        currType = theType;
      }
      else
      {
        currEvMask = (1<<theSource);
      }

      if(wrapperGenerator)
      {
        (*wrapperGenerator)(theType);
      }

#ifdef EVENT_MODE /* for second readout lists only !!! */
      /* following macro sets 'input_event__' !!! */
      listGet(&(rol->input->list), input_event__);
      rol->dabufpi = (long *) &input_event__->length;
#else
      (*(rol->nevents))++;
#endif

      /* call the routine */
      UNLOCKINTS;
      (*theNode->reader)(theType, Tcode[theSource]);
      LOCKINTS;

      /* done with this trigger */
      if(theNode)
      {
        if((theNode)->part == 0)
        {
          free(theNode);
          theNode = 0;
        }
        else
        {
          listAdd (&theNode->part->list, theNode);
        }
      }

      if(theEvMask)
      {
        /* remember we have seen it! */
        theEvMask = theEvMask & ~(1<<theSource);
      } 
      if(!theEvMask)
      {
        WRITE_EVENT_;
      }

    } /* if (theEvMask) */

  }

#endif

  dispatch_busy = 0;
}



/* Generic interrupt handler.
This routine looks to see if the dispatcher is already running.
If dispatcher is busy the trigger is queued, this code is protected
by INTLOCK so we know the flag dispatch_busy can't change behind our backs */

static int
theIntHandler(int theSource)
{
  if(theSource == 0) return(0);
  {
    DANODE *theNode;

    LOCKINTS;
    listGet(&(rol->dispatch->list), theNode);
    theNode->source = theSource;
    theNode->type = (*ttypeRtns[theSource])(Tcode[theSource]);
    theNode->reader = trigRtns[theSource]; 
    listAdd(&rol->dispQ->list, theNode); 
    if(!dispatch_busy) cdodispatch();
    UNLOCKINTS;
  }
}


/* polling handler: if running in polling mode, called instead of interrupt
   handler */

static int
cdopolldispatch()
{
  unsigned long theSource, theType;
  int stat = 0;
  DANODE *theNode;

  /*LOGIT("cdopolldispatch reached\n");fflush(stdout);*/
  if(!poolEmpty)
  {
/*LOGIT("cdopolldispatch reached\n");fflush(stdout);*/
    for(theSource=1;theSource<trigId;theSource++)
    {
/*LOGIT("cdopolldispatch 1: theSource=%d Tcode=%d\n",theSource,Tcode[theSource]);fflush(stdout);*/
      if(syncTRtns[theSource])
      {
        theNode = (DANODE *)(*syncTRtns[theSource])(Tcode[theSource]);
/*LOGIT("cdopolldispatch 2: syncTRtns[theSource]=0x%08x, theNode=%d\n",syncTRtns[theSource],theNode);fflush(stdout);*/
        if(theNode)
        {
LOGIT("cdopolldispatch 31\n");fflush(stdout);
          stat = 1;
          {  
            LOCKINTS;
            if(theNode) listGet(&(rol->dispatch->list), theNode);
            theNode->source = theSource; 
            theNode->type = (*ttypeRtns[theSource])(Tcode[theSource]); 
            theNode->reader = trigRtns[theSource]; 
            listAdd(&rol->dispQ->list, theNode); 
            if(!dispatch_busy)
            {
LOGIT("????????????????? (%d -> 0x%08x)\n",theSource,theNode->reader);fflush(stdout);
              cdodispatch();
LOGIT("!!!!!!!!!!!!!!!!!\n");fflush(stdout);
			}
            UNLOCKINTS;
          }
LOGIT("cdopolldispatch 32\n");fflush(stdout);
        }
      }
    }   
  }
  else
  {
    stat = -1;
  }

  return(stat);
}

#endif


/************************************************************************/
/************************************************************************/




/* Maximum Event Length in bytes 
#ifndef MAX_EVENT_LENGTH
#define MAX_EVENT_LENGTH 2048
#endif
*/

/* Maximum number of buffers in the Pool 
#ifndef MAX_EVENT_POOL
#define MAX_EVENT_POOL 512
#endif
*/

/* Define Default ROL Name 
#ifndef ROL_NAME__
#define ROL_NAME__ "Un-named"
#endif
*/

static char rol_name__[40];
static char temp_string__[132];


static void
__poll()
{
  CDOPOLL;
}


#ifdef	__cplusplus
extern "C"
#endif
/*static-error in dlsym*/ void
INIT_NAME(rolParam rolp)
{
  if( (rolp->daproc != DA_DONE_PROC) && (rolp->daproc != DA_POLL_PROC) )
  {
    LOGIT("INIT_NAME: rolp->daproc = %d\n",rolp->daproc);
  }

  switch(rolp->daproc)
  {
    case DA_INIT_PROC:
	{
	  char name[40];
	  rol = rolp;
	  rolp->inited = 1;
	  strcpy(rol_name__, ROL_NAME__);
	  rolp->listName = rol_name__;
	  LOGIT("INIT_NAME: Initializing new rol structures for %s\n",rol_name__);




	  LOGIT("INIT_NAME: MAX_EVENT_LENGTH = %d bytes, MAX_EVENT_POOL = %d\n",
        MAX_EVENT_LENGTH, MAX_EVENT_POOL);
	  strcpy(name, rolp->listName);
	  strcat(name, ":pool");
      LOGIT("INIT_NAME: name >%s<\n",name);
	  /* will do it in roc_component.c 
	  rolp->pool  = partCreate(name, MAX_EVENT_LENGTH , MAX_EVENT_POOL,1);
      LOGIT("INIT_NAME: rolp->pool=0x%08x\n",rolp->pool);
	  if(rolp->pool == 0)
      {
	    rolp->inited = -1;
	    break;
	  }
      */

	  strcpy(name, rolp->listName);
	  strcat(name, ":dispatch");
      LOGIT("INIT_NAME: name1 >%s<\n",name);
	  rolp->dispatch  = partCreate(name, 0, 32, 0);
	  if(rolp->dispatch == 0)
      {
	    rolp->inited = -1;
	    break;
	  }
      else
      {
        LOGIT("rolp->dispatch=0x%08x 0x%08x 0x%08x 0x%08x\n",&(rolp->dispatch),
          rolp->dispatch,&(rolp->dispatch->list),rolp->dispatch->list);
      }




	  strcpy(name, rolp->listName);
	  strcat(name, ":dispQ");
      LOGIT("INIT_NAME: name2 >%s<\n",name);
	  rolp->dispQ = partCreate(name, 0, 0, 0);
	  if(rolp->dispQ == 0)
      {
	    rolp->inited = -1;
	    break;
	  }
      else
      {
        LOGIT("rolp->dispQ=0x%08x 0x%08x 0x%08x 0x%08x\n",&(rolp->dispQ),
          rolp->dispQ,&(rolp->dispQ->list),rolp->dispQ->list);
      }




	  rolp->inited = 1;
	  LOGIT("Init - Done\n");
	  break;
	}

    case DA_FREE_PROC:
      rolp->inited = 0;
      break;
    case DA_DOWNLOAD_PROC:
	  __download();
	  break;
    case DA_PRESTART_PROC:
	  __prestart();
	  break;
    case DA_PAUSE_PROC:
	  __pause();
	  break;
    case DA_END_PROC:
	  __end();
	  break;
    case DA_GO_PROC:
	  __go();
	  break;
    case DA_POLL_PROC:
	  __poll();
	  break;
    case DA_DONE_PROC:
	  __done();
	  break;
    default:
	  LOGIT("WARN: unsupported rol action = %d\n",rolp->daproc);
	  break;
  }
}

#endif

