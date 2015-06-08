/* trgLibP.h - event triggers header */

/* Copyright 1997 Wind River Systems, Inc. */

/*
modification history
--------------------
01f,07sep98,pr   Replaced LIST_MAX_LENGTH with TRG_MAX_REQUESTS
01e,12feb98,pr   Added hitCnt field in struct trigger. 
		 Moved some definitions in ../trgLib.h. Cleanup.
01d,15dec97,nps  Added 'library' conditional type.
01c,20nov97,pr   added several triggering macros, reworked triggering
                 structure, added trgActionDefPerform 
01b,27oct97,pr   modified the struct trigger into an object. cleanup
01a,18jul97,pr   written

*/

#ifndef __INCtrglibph
#define __INCtrglibph


/* This file contains event triggering information. See also eventP.h */

#define TRIGGER_MASK   0x0000ffff

#define TRG_MSGQ_MAX_MSGS 	50
#define TRG_MSGQ_OPTIONS  	(MSG_Q_FIFO)
#define TRG_MSG_LEN       	4
#define TRG_ACT_PRIORITY          100
#define TRG_ACT_OPTIONS           VX_UNBREAKABLE
#define TRG_ACT_SIZE              2048
 
#ifndef _ASMLANGUAGE

#include "private/eventP.h"
#include "msgQLib.h"
#include "trgLib.h"

#define TRG_MAX_REQUESTS 50

typedef struct trigger 
    {
    OBJ_CORE		objCore;	/* trigger object core */
    event_t             eventId;        /* event type */
    UINT16              status;    	/* status of the trigger, */
					/* i.e. enabled, disabled, etc */
    BOOL                disable;	/* check if trigger needs to be */
					/* disabled after use */
    int                 contextType;    /* type of the context where the event occurs */
    UINT32              contextId;      /* id of the context where the event occurs */
    OBJ_ID		objId;     	/* object type */
    struct trigger      *chain;         /* pointer to chained trigger */
    struct trigger      *next;          /* ptr to next trigger in the list */
    int 		conditional;  	/* check if a condition is set */
    int 		condType;  	/* check the expression type (var/fn) */
    void *		condEx1;  	/* pointer to conditional expression */
    int 		condOp;  	/* conditional operator (==, !=, ...) */
    int 		condEx2;  	/* second operand (constant) */
    int         	actionType; 	/* type of action (none, fn, lib) */
    FUNCPTR     	actionFunc; 	/* pointer to the action */
    int	                actionArg;    	/* argument passed to the action */
    BOOL         	actionDef; 	/* defer the action */
    int			hitCnt;
    } TRIGGER;

#if defined(__STDC__) || defined(__cplusplus)
extern STATUS   	trgInit (void);
extern STATUS 		trgActionDefPerform (void);
extern STATUS 		trgActionDefStart (void);
extern int    		trgEvtToIndex (event_t event);
extern BOOL    		trgContextMatch (TRIGGER_ID pTrg);
extern BOOL    		trgCondTest (TRIGGER_ID pTrg, int objId);
extern TRIGGER_ID    	trgAddTcl (char * buff);
extern void    		trgActionPerform (TRIGGER_ID pTrg);
extern void    		trgCheck (event_t event, int index, int obj, int arg1, int arg2, 
				 int arg3, int arg4, int arg5);
#else   /* __STDC__ */
extern STATUS   	trgInit ();
extern STATUS 		trgActionDefPerform ();
extern STATUS 		trgActionDefStart ();
extern int    		trgEvtToIndex ();
extern BOOL    		trgContextMatch ();
extern BOOL    		trgCondTest ();
extern TRIGGER_ID    	trgAddTcl ();
extern void    		trgActionPerform ();
extern void    		trgCheck () :
#endif  /* __STDC__ */

#endif /* _ASMLANGUAGE */ 

#endif /* __INCtrglibph*/
