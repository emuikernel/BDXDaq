/* 
 *   Trigger dispatch Macros for multiple trigger sources in CODA.
 *
 * SVN: $Rev: 396 $
 *
 */

#ifndef __TRIGGER_DISPATCH
#define __TRIGGER_DISPATCH

#include <pthread.h>
#include <string.h>
/* private global */

#define MAXSRC    32
#define MAXETY    16

static unsigned char dispatch_busy; 

static int intLockKey,trigId;

static int poolEmpty;

static unsigned long theEvMask, currEvMask, currType, evMasks[MAXETY];

static VOIDFUNCPTR wrapperGenerator;

static FUNCPTR trigRtns[MAXSRC], syncTRtns[MAXSRC], doneRtns[MAXSRC], ttypeRtns[MAXSRC];

static unsigned long Tcode[MAXSRC];

static DANODE *__the_event__, *input_event__, *__user_event__;

pthread_mutex_t trigger_mutex=PTHREAD_MUTEX_INITIALIZER;

/* 
   Clear some global variables etc for a clean start.
   Should be done at Prestart.
*/

#define CTRIGINIT					\
  {							\
    dispatch_busy = 0;					\
    memset((char *) evMasks, 0, sizeof(evMasks));	\
    memset((char *) syncTRtns, 0, sizeof(syncTRtns));	\
    memset((char *) ttypeRtns, 0, sizeof(ttypeRtns));	\
    memset((char *) Tcode, 0, sizeof(Tcode));		\
    wrapperGenerator = 0;				\
    theEvMask = 0;					\
    currEvMask = 0;					\
    trigId = 1;						\
    poolEmpty = 0;					\
    __the_event__ = (DANODE *) 0;			\
    input_event__ = (DANODE *) 0;			\
  }

/*
  Register a procedure to do the wrapping NULL = don't wrap
*/

#define CRWRAPP(p) {wrapperGenerator = (p);}

/* 
   Register an async trigger source, (up to 32 sources).
*/

#define CTRIGRSA(source,code,handler,done) {	\
    void handler();void done();			\
    doneRtns[trigId]  = (FUNCPTR) (done) ;	\
    trigRtns[trigId]  = (FUNCPTR) (handler) ;	\
    Tcode[trigId]     = (code) ;		\
    ttypeRtns[trigId] = source##_TTYPE ;	\
    source##_ASYNC(code,trigId);trigId++;}

/* 
   Register an sync trigger source, (up to 32 sources).
*/

#define CTRIGRSS(source,code,handler,done) {		\
    void handler();void done();				\
    doneRtns[trigId]  = (FUNCPTR) (done) ;		\
    trigRtns[trigId]  = (FUNCPTR) (handler) ;		\
    syncTRtns[trigId] = (FUNCPTR) source##_TEST ;	\
    Tcode[trigId]     = (code) ;			\
    ttypeRtns[trigId] = source##_TTYPE ;		\
    source##_SYNC(code,trigId);trigId++;}
   
/* 
   Register a physics event type, (up to 16 types) .
*/

#define CRTTYPE(type,source,code) {evMasks[type] |= (1<<(source##_GETID(code)));}

#ifdef VXWORKS
#define LOCKINTS intLockKey = intLock();
#else
#define LOCKINTS {				\
    if(pthread_mutex_lock(&trigger_mutex)<0)	\
      perror("pthread_mutex_lock");		\
  }
#endif

#ifdef VXWORKS
#define UNLOCKINTS intUnlock(intLockKey);
#else
#define UNLOCKINTS {				\
    if(pthread_mutex_unlock(&trigger_mutex)<0)	\
      perror("pthread_mutex_lock");		\
  }
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

#define WRITE_EVENT_   rol->dabufp = NULL;				\
  if (__the_event__) {							\
    if (rol->output) {							\
      listAdd(&(rol->output->list),__the_event__);			\
    } else {								\
      partFreeItem(__the_event__);					\
    }									\
    __the_event__ = (DANODE *) 0;					\
  }									\
  if(input_event__) {							\
    partFreeItem(input_event__);					\
    input_event__ = (DANODE *) 0;					\
  }									\
                       {int ix;						\
			 if (currEvMask) {				\
			   for (ix=0; ix < trigId; ix++) {		\
			     if (currEvMask & (1<<ix)) (*doneRtns[ix])(); \
			   }						\
			   if (rol->pool->list.c) {			\
			     currEvMask = 0;				\
			     __done();					\
			     rol->doDone = 0;				\
			   } else {					\
			     poolEmpty = 1;				\
			     rol->doDone = 1;				\
			   }						\
			 }						\
		       }
/*
  Actually dispatch the triggers to the correct routines.
*/

static void cdodispatch()
{
  unsigned long theType,theSource;
  int ix, go_on;
  DANODE *theNode;

  dispatch_busy = 1;
  go_on = 1;

  /* While there are events */

  while ((rol->dispQ->list.c) && (go_on)) {

    /* dequeue off head */
    listGet(&rol->dispQ->list, theNode);
    theType = theNode->type;
    theSource = theNode->source;
    if (theEvMask) { 
      /* We are already in an event */
      /* is this source required for this event ? */
      if ((theEvMask & (1<<theSource)) && (theType == currType)) {
	/* yes */
	/* remember we have seen it! */
	theEvMask = theEvMask & ~(1<<theSource);

	/* call the routine */
	UNLOCKINTS;
	(*theNode->reader)(theType, Tcode[theSource]);
	LOCKINTS;

	/* done with this trigger */
	if (theNode)
	  /* partFreeItem(theNode); */
	  if (!theEvMask) {
	    if (wrapperGenerator) CECLOSE;	    /* if we called a wrap routine close the bank */
	    WRITE_EVENT_;
	  }
      } else {
	/* no so requeue on tail*/
	listAdd(&rol->dispQ->list, theNode);
	go_on = 0;
      }
    } else { 
      /* We are not in an event */
      /* get a new buffer... */

      if ((1<<theSource) & evMasks[theType]) {
	currEvMask = theEvMask = evMasks[theType];
	currType = theType;
      } else {
        currEvMask = (1<<theSource);
      }

      if (wrapperGenerator) {
	(*wrapperGenerator)(theType);
      }

#ifdef EVENT_MODE
      partGetItem(rol->input,input_event__);
      rol->dabufpi = (long *) &input_event__->length;
#else
      (*(rol->nevents))++;
#endif

      /* call the routine */
      UNLOCKINTS;
      (*theNode->reader)(theType, Tcode[theSource]);
      LOCKINTS;

      /* done with this trigger */
      if (theNode)
	partFreeItem(theNode);
      if (theEvMask) {
	/* remember we have seen it! */
	theEvMask = theEvMask & ~(1<<theSource);
      } 
      if (!theEvMask) {
	WRITE_EVENT_;
      }
      
    } /* if (theEvMask) */

  }
  dispatch_busy = 0;
}

/* 
   Generic interrupt handler.
   
   This routine looks to see if the dispatcher is already running.
   If dispatcher is busy the trigger is queued, this code is protected
   by INTLOCK so we know the flag dispatch_busy can't change behind our 
   backs.
*/


static int theIntHandler(int theSource)
{
  if (theSource == 0) return(0);
  {  
    DANODE *theNode;

    LOCKINTS;
    partGetItem(rol->dispatch, theNode);
    theNode->source = theSource;
    theNode->type = (*ttypeRtns[theSource])(Tcode[theSource]);
    theNode->reader = trigRtns[theSource]; 
    listAdd(&rol->dispQ->list, theNode); 
    if (!dispatch_busy)
      cdodispatch();
    UNLOCKINTS;
  }
}

static int cdopolldispatch()
{
  unsigned long theSource, theType;
  int stat = 0;
  DANODE *theNode;

  if (!poolEmpty) {
    for (theSource=1;theSource<trigId;theSource++){
      if (syncTRtns[theSource]){
	if ( theNode = (*syncTRtns[theSource])(Tcode[theSource])) {
	  stat = 1;
	  {  
	    LOCKINTS;
	    if (theNode) 
	      partGetItem(rol->dispatch, theNode);
	    theNode->source = theSource; 
	    theNode->type = (*ttypeRtns[theSource])(Tcode[theSource]); 
	    theNode->reader = trigRtns[theSource]; 
	    listAdd(&rol->dispQ->list, theNode); 
	    if (!dispatch_busy) 
	      cdodispatch();
	    UNLOCKINTS;
	  }
	}
      }
    }   
  } else {
    stat = -1;
  }
  return (stat);
}

#endif
