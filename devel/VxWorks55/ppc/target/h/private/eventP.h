/* eventP.h - event log header */

/* Copyright 1994-2001 Wind River Systems, Inc. */

/*
modification history
--------------------
02s,09may02,tcr  Fix for SPR 74987 (add #ifdef __cplusplus)
02r,31oct01,tcr  add event range for RTI
02q,18oct01,tcr  add support for VxWorks Events, move to VxWorks 5.5
02q,27sep01,tcr  Fix SPR 24971 - instrument PPC interrupts
02p,13sep01,tcr  Fix SPR 29673 - macro for msgQSmLib events
02o,09sep98,cjtc reverting to byte ordering based on _BYTE_ORDER macro
02n,04sep98,cjtc porting for ARM architechture
02m,02sep98,nps  corrected WV2.0 -> T2 merge.
02l,24aug98,cjtc intEnt logging for PPC is now performed in a single step
		 instead of the two-stage approach which gave problems with
		 out-of-order timestamps in the event log (SPR 21868)
02k,17aug98,pr   replaced OBJ_VERIFY check with check on NULL pointer
02j,08may98,pr   added number of params for EVT_OBJ_SIG macro
02j,08may98,pr   added argument for wv logging in SIG and SM_MSGQ macros 
02i,15apr98,cth  added event class WV_CLASS_TASKNAMES_PRESERVE/ON, and call to
		 _func_evtLogReserveTaskName
02h,08apr98,pr   reordered the definition of EVENT_SEMFLUSH and EVENT_SEMDELETE
02g,07apr98,cjtc added () to EVENT_INT_ENT to prevent problem with macro
		 expansion.
02f,22mar98,pr   modified EVT_CTX_BUF to accept TRG_USER_INDEX events. 
		 modified definition of EVENT_SEMFLUSH and EVENT_SEMDELETE.
02e,04mar98,nps  modified xxx_CLASS_n_ON macros so they can be used by
                 inadequate assmblers.
02d,17feb98,pr   added number of params to EVT_OBJ macros.
02c,17feb98,dvs  added events for memory instrumentation. (pr)
02b,27jan98,cth  removed EVENT_BUFFER, CONFIG, BEGIN, END _SIZE definitions,
		 updated copyright
02a,22jan98,pr   made some macros readable also by the assembly.
		 added extra bit to evtAction.
		 added EVT_CTX_IDLE, EVT_CTX_NODISP macros.
01z,12jan98,dbs  added EVENT_NIL and EVENT_VALUE to pseudo-events.
01y,13dec97,pr   reduced the number of args passed to _func_trgCheck
                 moved some extern declaration in funcBindP.h
01x,20nov97,pr   added some event triggering defines and modified event 
                 logging macros
01w,17nov97,cth  added EVENT_CONFIG_SIZE, _BEGIN_SIZE, _END_SIZE, _BUFFER_SIZE
01v,27oct97,pr   redefined levels as classes. 
		 redesigned all the macros for managing classes. 
01u,28aug97,pr   modified WV_OFF to reflect new levels.
01t,26aug97,pr   replaced WV_IS_ON with proper LEVEL
01s,18aug97,pr   added trigger calls.
01r,24jun97,pr   Added declarations for WindView 2.0 variables
		 modified macros to include new variable and triggers
01q,11Apr96,pr   changed if statement in EVT_OBJ_SM_MSGQ, SPR #6048, #6271
01p,22feb95,rdc  added EVENT_TASK_STATECHANGE.
01o,01feb95,rdc  added EVENT_TIMER_SYNC.
01n,27jan95,rdc  added processor number to EVENT_CONFIG.
01m,28nov94,rdc  changed 32 bit store macros for x86.
01l,02nov94,rdc  added protocol revision to EVENT_CONFIG.  
		 added lockCnt param to EVT_CTX_TASKINFO.
01k,02nov94,rdc  added 960 macros.
01j,27may94,pad  added alignment macros. Corrected some parameter comments.
01i,14apr94,smb  optimised EVT_OBJ_ macros.
01h,04mar94,smb  added EVENT_INT_EXIT_K 
		 changed parameters for EVENT_WIND_EXIT_NODISPATCH
		 removed EVENT_WIND_EXIT_IDLENOT
		 level 1 event optimisations
		 added macro for shared memory objects
01h,22feb94,smb  changed typedef EVENT_TYPE to event_t (SPR #3064)
01g,16feb94,smb  introduced new OBJ_ macros, SPR #2982
01f,26jan94,smb  redid pseudo events - generated host side
01e,24jan94,smb  added EVENT_WIND_EXIT_DISPATCH_PI and 
		 EVENT_WIND_EXIT_NODISPATCH_PI
		 added event logging macros
01d,17dec93,smb  changed typedef of EVENT_TYPE
01c,10dec93,smb  modified MAX_LEVEL3_ID to include interrupt events
	  	 renamed EVENT_CLOCK_CONFIG to EVENT_CONFIG
01b,08dec93,kdl  added include of classLibP.h; made def of OBJ_INST_FUNC
	   +smb	 conditional on OBJ_INST_RTN; reformatted macro comments;
		 made includes and obj macros depend on CPU (i.e. not host).
01a,07dec93,smb  written.
*/

#ifndef __INCeventph
#define __INCeventph

#ifdef __cplusplus
extern "C" {
#endif

/* This file contains all windview event identifiers */

/* Triggering and Windview events definitions and macros. 
 * They correspond to the old levels and are the only classes we 
 * identify for the moment. They are also used by the architecture 
 * dependent files, therefore are defined also for assembly languages.
 */

#ifdef	CPU				/* only for target, not host */

/* this is used by both WV and TRG */

#define CLASS_NONE    			0x00000000 

#define WV_CLASS_1    			0x00000001   /* Context Switch */
#define WV_CLASS_2    			0x00000003   /* Task State Transition */
#define WV_CLASS_3    			0x00000007   /* Object and System Libraries */
#define WV_CLASS_TASKNAMES_PRESERVE	0x00001000
#define WV_ON           		0x10000000
#define WV_CLASS_1_ON  			0x10000001
#define WV_CLASS_2_ON 			0x10000003
#define WV_CLASS_3_ON  			0x10000007
#define WV_CLASS_TASKNAMES_PRESERVE_ON	0x10001000

#define TRG_CLASS_1     		0x00000001
#define TRG_CLASS_2     		0x00000010
#define TRG_CLASS_3     		0x00000100
#define TRG_CLASS_4     		0x00001000
#define TRG_CLASS_5     		0x00010000
#define TRG_CLASS_6     		0x01111111
#define TRG_ON          		0x10000000
#define TRG_CLASS_1_ON  		0x10000001
#define TRG_CLASS_2_ON  		0x10000010
#define TRG_CLASS_3_ON  		0x10000100
#define TRG_CLASS_4_ON  		0x10001000
#define TRG_CLASS_5_ON  		0x10010000
#define TRG_CLASS_6_ON  		0x11111111

#define TRG_CLASS1_INDEX	0
#define TRG_CLASS2_INDEX	1
#define TRG_CLASS3_INDEX	2
#define TRG_USER_INDEX		3
#define TRG_INT_ENT_INDEX	4
#define TRG_ANY_EVENT_INDEX	5
#define TRG_CONTROL_INDEX	6

#endif /* CPU */

/* types */

#ifndef _ASMLANGUAGE

typedef unsigned short event_t;

#ifdef	CPU				/* only for target, not host */

#include "private/funcBindP.h"
#include "private/classLibP.h"
#include "private/objLibP.h"

#define ACTION_IS_SET			(evtAction != 0)

#define TRG_ACTION_SET                  \
        {                               \
        evtAction |= 0x0001;             \
        TRG_EVTCLASS_SET(TRG_ON);               \
        }

#define TRG_ACTION_UNSET                        \
        {                               \
        TRG_EVTCLASS_UNSET(TRG_ON);             \
        evtAction &= ~(0x0001);            \
        }

#define TRG_ACTION_IS_SET		( (evtAction&0x00ff) == 0x0001)

#define WV_ACTION_SET                   \
        {                               \
        evtAction |= 0x0100;             \
        WV_EVTCLASS_SET(WV_ON);         \
        }

#define WV_ACTION_UNSET                 \
        {                               \
        WV_EVTCLASS_UNSET(WV_ON);               \
        evtAction &= ~(0x0100);            \
        }

#define WV_ACTION_IS_SET		( (evtAction&0xff00) == 0x0100)

#define WV_EVTCLASS_IS_SET(class) 	( (wvEvtClass&(class)) == (class))
#define WV_EVTCLASS_SET(class) 		(wvEvtClass |= (class))
#define WV_EVTCLASS_UNSET(class)        (wvEvtClass &= ~(class))
#define WV_EVTCLASS_EMPTY	 	(wvEvtClass = CLASS_NONE)

#define TRG_EVTCLASS_IS_SET(class) 	((trgEvtClass&(class)) == (class))

#define TRG_EVTCLASS_SET(class) 	(trgEvtClass |= (class))
#define TRG_EVTCLASS_UNSET(class)	(trgEvtClass &= ~(class))
#define TRG_EVTCLASS_EMPTY	 	(trgEvtClass = CLASS_NONE)
#define TRG_EVTCLASS_IS_EMPTY	 	(trgEvtClass == CLASS_NONE)

/* needed to handle redefinition of obj_core helpRtn field to instRtn 
 * for instrumented kernel
 */

#ifdef OBJ_INST_RTN

#define OBJ_EVT_RTN(objId)					\
    (((OBJ_ID)(objId))->pObjClass)->instRtn

#define TASK_EVT_RTN(tId)					\
    (((OBJ_ID)(&((WIND_TCB *)(tId))->objCore))->pObjClass)->instRtn

#else  /* OBJ_INST_RTN */

#define OBJ_EVT_RTN(objId)					\
    (((OBJ_ID)(objId))->pObjClass)->helpRtn

#define TASK_EVT_RTN(tId)					\
    (((OBJ_ID)(&((WIND_TCB *)(tId))->objCore))->pObjClass)->helpRtn

#endif /* OBJ_INST_RTN */

/* event logging macros */

/******************************************************************************
*
* OBJ_INST_FUNC - check if object is instrumented
*
* RETURNS: instrumentation routine if instrumented, otherwise NULL
* NOMANUAL
*/

#define OBJ_INST_FUNC(objId, classId)   			\
    ((objId != NULL) ? (OBJ_EVT_RTN (objId)) : (NULL)) 

/******************************************************************************
*
* TASK_INST_FUNC - check if task is instrumented
*
*
* RETURNS: instrumentation routine if instrumented, otherwise NULL
* NOMANUAL
*/

#define TASK_INST_FUNC(tid, classId)  				\
    (((TASK_ID_VERIFY(tid)) == OK)		\
     ? (TASK_EVT_RTN (tid)) : (NULL))

/******************************************************************************
*
* EVT_CTX_0 - context switch event logging with event id 
*
* NOMANUAL
*/

#define EVT_CTX_0(evtId)                                        \
        if (ACTION_IS_SET)					\
        {                                                       \
            if ( WV_EVTCLASS_IS_SET(WV_CLASS_1|WV_ON) )                     \
                ( * _func_evtLogT0) (evtId);                    \
            if (TRG_EVTCLASS_IS_SET(TRG_CLASS_1|TRG_ON))			\
		( * _func_trgCheck) (evtId, TRG_CLASS1_INDEX, NULL, NULL, NULL, NULL, NULL, NULL);                            \
        }


/******************************************************************************
*
* EVT_CTX_1 - context switch event logging with one parameter
*
* NOMANUAL
*/

#define EVT_CTX_1(evtId, arg)                                   \
    if ( ACTION_IS_SET )					\
        {                                                       \
        if ( WV_EVTCLASS_IS_SET(WV_CLASS_1|WV_ON) )                         \
            ( * _func_evtLogT1) (evtId, arg);                   \
        if ( TRG_EVTCLASS_IS_SET(TRG_CLASS_1|TRG_ON) )                                   \
		( * _func_trgCheck) (evtId, TRG_CLASS1_INDEX, NULL, arg, NULL, NULL, NULL, NULL);                            \
        }



/******************************************************************************
*
* EVT_CTX_IDLE - log EVENT_WIND_EXIT_IDLE event
*
* This macro stores information into the event buffer if the system is idle.
*
* NOMANUAL
*/

#define EVT_CTX_IDLE(evtId, arg1)           	                \
     if ( ACTION_IS_SET && (Q_FIRST (arg1) == NULL))					\
        {                                                       \
        if (WV_EVTCLASS_IS_SET(WV_CLASS_1|WV_ON))			        \
            (* _func_evtLogT0) (evtId);                 \
        if ( TRG_EVTCLASS_IS_SET(TRG_CLASS_1|TRG_ON) )                  \
		( * _func_trgCheck) (evtId, TRG_CLASS1_INDEX, NULL, NULL, NULL, NULL, NULL, NULL);      \
        }  


/******************************************************************************
*
* EVT_CTX_DISP - context switch event logging for reschedule 
*
* This macro stores information into the event buffer about rescheduling tasks
* in the system. In order to determine the right event type, it first checks the
* two last args. It is used to log the DISPATCH event. Therefore the two last args 
* are the current priority and the normal priority.
*
* NOMANUAL
*/

#define EVT_CTX_DISP(evtId, arg1, arg2, arg3)           	                \
    if ( ACTION_IS_SET )					\
        {                                                       \
        if (arg3 > arg2)                                                \
	    {                                                           \
            if (WV_EVTCLASS_IS_SET(WV_CLASS_1|WV_ON))			        \
                (* _func_evtLogTSched) (EVENT_WIND_EXIT_DISPATCH_PI, arg1, arg2);     \
            if ( TRG_EVTCLASS_IS_SET(TRG_CLASS_1|TRG_ON) )                  \
		( * _func_trgCheck) (EVENT_WIND_EXIT_DISPATCH_PI, TRG_CLASS1_INDEX, NULL, arg1, arg2, NULL, NULL, NULL);      \
	    }                                                           \
	    else                                                        \
	    {                                                           \
            if (WV_EVTCLASS_IS_SET(WV_CLASS_1|WV_ON))			        \
                (* _func_evtLogTSched) (EVENT_WIND_EXIT_DISPATCH, arg1, arg2);     \
            if ( TRG_EVTCLASS_IS_SET(TRG_CLASS_1|TRG_ON) )                  \
		( * _func_trgCheck) (EVENT_WIND_EXIT_DISPATCH, TRG_CLASS1_INDEX, NULL, arg1, arg2, NULL, NULL, NULL);      \
	    }                                                           \
        }  

/******************************************************************************
*
* EVT_CTX_NODISP - context switch event logging for reschedule 
*
* This macro stores information into the event buffer about rescheduling tasks
* in the system. In order to determine the right event type, it first checks the
* two last args. It is used to log the NODISPATCH event. Therefore the two last 
* args are the current priority and the normal priority.
*
* NOMANUAL
*/

#define EVT_CTX_NODISP(arg1, arg2, arg3)           	                \
    if (ACTION_IS_SET)                                                      \
        {                                                               \
        if (arg3 > arg2)                                                \
	    {                                                           \
            if (WV_EVTCLASS_IS_SET(WV_CLASS_1|WV_ON))			        \
                (* _func_evtLogTSched) (EVENT_WIND_EXIT_NODISPATCH_PI, arg1, arg2);\
            if ( TRG_EVTCLASS_IS_SET(TRG_CLASS_1|TRG_ON) )				\
		( * _func_trgCheck) (EVENT_WIND_EXIT_NODISPATCH_PI, TRG_CLASS1_INDEX, NULL, arg1, arg2, NULL, NULL, NULL);                            \
	    }                                                           \
	    else                                                        \
	    {                                                           \
            if (WV_EVTCLASS_IS_SET(WV_CLASS_1|WV_ON))			        \
               (* _func_evtLogTSched) (EVENT_WIND_EXIT_NODISPATCH, arg1, arg2);\
            if ( TRG_EVTCLASS_IS_SET(TRG_CLASS_1|TRG_ON) )				\
		( * _func_trgCheck) (EVENT_WIND_EXIT_NODISPATCH, TRG_CLASS1_INDEX, NULL, arg1, arg2, NULL, NULL, NULL);                            \
	    }                                                           \
        }

/******************************************************************************
*
* EVT_CTX_TASKINFO - context switch event logging with task information
*
* This macro stores information into the event buffer about a task
* in the system.
*
* NOMANUAL
*/

#define EVT_CTX_TASKINFO(evtId, state, pri, lockCnt, tid, name)              \
    if ( ACTION_IS_SET )						     \
        {                                                       	     \
        if ( WV_EVTCLASS_IS_SET(WV_CLASS_1|WV_ON) )                          \
	    {								     \
            ( * _func_evtLogString) (evtId, state, pri, lockCnt, tid, name); \
									     \
	    if (WV_EVTCLASS_IS_SET(WV_CLASS_TASKNAMES_PRESERVE))	     \
	        ( * _func_evtLogReserveTaskName) (evtId, state, pri, lockCnt, tid, name);	     	     						     \
	    }								     \
        if ( TRG_EVTCLASS_IS_SET(TRG_CLASS_1|TRG_ON) )                                  \
		( * _func_trgCheck) (evtId, TRG_CLASS1_INDEX, NULL, state, pri, lockCnt, tid, name);                            \
        }

/******************************************************************************
*
* EVT_CTX_BUF - context switch event logging which logs a buffer,
*
* This macro stores eventpoint and user events into the event buffer.
*
* NOMANUAL
*/

#define EVT_CTX_BUF(evtId, addr, bufSize, BufferAddr)		\
    if ( ACTION_IS_SET )					\
        {                                                               \
        if ( WV_EVTCLASS_IS_SET(WV_CLASS_1|WV_ON) )                                  \
             ( * _func_evtLogPoint) (evtId, addr, bufSize, BufferAddr); \
        if ( TRG_EVTCLASS_IS_SET(TRG_CLASS_4|TRG_ON) )                                   \
		( * _func_trgCheck) (evtId, TRG_USER_INDEX, NULL, addr, bufSize, BufferAddr, NULL, NULL);                            \
        }

/******************************************************************************
*
* EVT_TASK_0 - task state transition event logging with event id.
*
*
* NOMANUAL
*/

#define EVT_TASK_0(evtId)                                       \
    if ( ACTION_IS_SET )					\
        {                                                       \
        if ( WV_EVTCLASS_IS_SET(WV_CLASS_2|WV_ON) )                         \
            (* _func_evtLogM0) (evtId);                         \
        if ( TRG_EVTCLASS_IS_SET(TRG_CLASS_2|TRG_ON) )                                   \
		( * _func_trgCheck) (evtId, TRG_CLASS2_INDEX, NULL, NULL, NULL, NULL, NULL, NULL);                            \
        }

/******************************************************************************
*
* EVT_TASK_1 - task state transition event logging with one argument.
*
*
* NOMANUAL
*/

#define EVT_TASK_1(evtId, arg)                                  \
  do \
   { \
    if ( ACTION_IS_SET )					\
        {                                                       \
        if ( WV_EVTCLASS_IS_SET(WV_CLASS_2|WV_ON) )                         \
            (* _func_evtLogM1) (evtId, arg);                    \
        if ( TRG_EVTCLASS_IS_SET(TRG_CLASS_2|TRG_ON) )                                   \
		( * _func_trgCheck) (evtId, TRG_CLASS2_INDEX, NULL, arg, NULL, NULL, NULL, NULL);                            \
        }  \
   } while (0)


/******************************************************************************
*
* EVT_TASK_2 - task state transition event logging with two arguments.
*
*
* NOMANUAL
*/

#define EVT_TASK_2(evtId, arg1, arg2)                           \
    if ( ACTION_IS_SET )					\
        {                                                       \
        if ( WV_EVTCLASS_IS_SET(WV_CLASS_2|WV_ON) )                         \
            (* _func_evtLogM2) (evtId, arg1, arg2);             \
        if ( TRG_EVTCLASS_IS_SET(TRG_CLASS_2|TRG_ON) )                                   \
		( * _func_trgCheck) (evtId, TRG_CLASS2_INDEX, NULL, arg1, arg2, NULL, NULL, NULL);                            \
        }

/******************************************************************************
*
* EVT_TASK_3 - task state transition event logging with three arguments.
*
*
* NOMANUAL
*/

#define EVT_TASK_3(evtId, arg1, arg2, arg3)                     \
    if ( ACTION_IS_SET )					\
        {                                                       \
        if ( WV_EVTCLASS_IS_SET(WV_CLASS_2|WV_ON) )                         \
            (* _func_evtLogM3) (evtId, arg1, arg2, arg3);       \
        if ( TRG_EVTCLASS_IS_SET(TRG_CLASS_2|TRG_ON) )                                   \
		( * _func_trgCheck) (evtId, TRG_CLASS2_INDEX, NULL, arg1, arg2, arg3, NULL, NULL);                            \
        }

/******************************************************************************
*
* EVT_OBJ_6 - object status event logging with six arguments.
*
*
* NOMANUAL
*/

#define EVT_OBJ_6(objType, objId, objClassId, evtId, arg1, arg2, arg3,  \
		     arg4, arg5, arg6)					\
    if ( ACTION_IS_SET )					\
        {                                                               \
        if ( WV_EVTCLASS_IS_SET(WV_CLASS_3|WV_ON) )                                 \
            {                                                           \
            VOIDFUNCPTR __evtRtn__;                                     \
            if ((__evtRtn__ = (objType##_INST_FUNC(objId,objClassId))) != NULL)\
                (* __evtRtn__) (evtId, 6, arg1, arg2, arg3, arg4, arg5, arg6);\
            }                                                           \
        if ( TRG_EVTCLASS_IS_SET(TRG_CLASS_3|TRG_ON) )                                   \
		( * _func_trgCheck) (evtId, TRG_CLASS3_INDEX, objId, arg1, arg2, arg3, arg4, arg5);                            \
        }

/******************************************************************************
*
* EVT_OBJ_5 - object status event logging with five arguments.
*
*
* NOMANUAL
*/

#define EVT_OBJ_5(objType, objId, objClassId, evtId, arg1, arg2, arg3,  \
		     arg4, arg5)					\
   if ( ACTION_IS_SET )					\
        {                                                               \
        if ( WV_EVTCLASS_IS_SET(WV_CLASS_3|WV_ON) )                                 \
            {                                                           \
            VOIDFUNCPTR __evtRtn__;                                     \
            if ((__evtRtn__ = (objType##_INST_FUNC(objId,objClassId))) != NULL)\
                (* __evtRtn__) (evtId, 5, arg1, arg2, arg3, arg4, arg5);   \
            }                                                           \
        if ( TRG_EVTCLASS_IS_SET(TRG_CLASS_3|TRG_ON) )                                    \
		( * _func_trgCheck) (evtId, TRG_CLASS3_INDEX, objId, arg1, arg2, arg3, arg4, arg5);                            \
        }

/******************************************************************************
*
* EVT_OBJ_4 - object status event logging with four arguments.
*
*
* NOMANUAL
*/

#define EVT_OBJ_4(objType, objId, objClassId, evtId, arg1, arg2, arg3, arg4)\
    if ( ACTION_IS_SET )					\
        {                                                               \
        if ( WV_EVTCLASS_IS_SET(WV_CLASS_3|WV_ON) )                                 \
            {                                                           \
                        VOIDFUNCPTR __evtRtn__; \
            if ((__evtRtn__ = (objType##_INST_FUNC(objId,objClassId))) != NULL)\
                (* __evtRtn__) (evtId, 4, arg1, arg2, arg3, arg4);         \
            }                                                           \
        if ( TRG_EVTCLASS_IS_SET(TRG_CLASS_3|TRG_ON) )                                    \
		( * _func_trgCheck) (evtId, TRG_CLASS3_INDEX, objId, arg1, arg2, arg3, arg4, NULL);                            \
        }

/******************************************************************************
*
* EVT_OBJ_3 - object status event logging with three arguments.
*
*
* NOMANUAL
*/

#define EVT_OBJ_3(objType, objId, objClassId, evtId, arg1, arg2, arg3)  \
    if ( ACTION_IS_SET )					\
        {                                                               \
        if ( WV_EVTCLASS_IS_SET(WV_CLASS_3|WV_ON) )                                 \
            {                                                           \
                        VOIDFUNCPTR __evtRtn__; \
            if ((__evtRtn__ = (objType##_INST_FUNC(objId,objClassId))) != NULL)\
                (* __evtRtn__) (evtId, 3, arg1, arg2, arg3);               \
            }                                                           \
        if ( TRG_EVTCLASS_IS_SET(TRG_CLASS_3|TRG_ON) )                                    \
{\
		( * _func_trgCheck) (evtId, TRG_CLASS3_INDEX, objId, arg1, arg2, arg3, NULL, NULL);                            \
        }\
        }

/******************************************************************************
*
* EVT_OBJ_2 - object status event logging with two arguments.
*
*
* NOMANUAL
*/

#define EVT_OBJ_2(objType, objId, objClassId, evtId, arg1, arg2)	       \
    if ( ACTION_IS_SET )					               \
        {                                                                      \
        if ( WV_EVTCLASS_IS_SET(WV_CLASS_3|WV_ON) )                                     \
            {                                                                  \
            VOIDFUNCPTR __evtRtn__;                                            \
            if ((__evtRtn__ = (objType##_INST_FUNC(objId,objClassId))) != NULL)\
                (* __evtRtn__)(evtId, 2, arg1, arg2);                             \
            }                                                                  \
        if ( TRG_EVTCLASS_IS_SET(TRG_CLASS_3|TRG_ON) )                         \
		( * _func_trgCheck) (evtId, TRG_CLASS3_INDEX, objId, arg1, arg2, NULL, NULL, NULL);                            \
        }

/******************************************************************************
*
* EVT_OBJ_1 - object status event logging with an argument.
*
*
* NOMANUAL
*/

#define EVT_OBJ_1(objType, objId, objClassId, evtId, arg1)		\
    if ( ACTION_IS_SET )					\
        {                                                               \
        if ( WV_EVTCLASS_IS_SET(WV_CLASS_3|WV_ON) )                                 \
            {                                                           \
                        VOIDFUNCPTR __evtRtn__; \
            if ((__evtRtn__ = (objType##_INST_FUNC(objId,objClassId))) != NULL)\
                (* __evtRtn__) (evtId, 1, arg1);                           \
            }                                                           \
        if ( TRG_EVTCLASS_IS_SET(TRG_CLASS_3|TRG_ON) )                                    \
		( * _func_trgCheck) (evtId, TRG_CLASS3_INDEX, objId, arg1, NULL, NULL, NULL, NULL);                            \
        }

/******************************************************************************
*
* EVT_OBJ_TASKSPAWN - object status event logging for a spawned task.
*
*
* NOMANUAL
*/

#define EVT_OBJ_TASKSPAWN(evtId, arg1, arg2, arg3, arg4, arg5)  \
    if ( ACTION_IS_SET )					\
        {                                                               \
        if ( WV_EVTCLASS_IS_SET(WV_CLASS_3|WV_ON) )                                 \
              ( * _func_evtLogOIntLock) (evtId, 5, arg1, arg2, arg3, arg4, arg5); \
        if ( TRG_EVTCLASS_IS_SET(TRG_CLASS_3|TRG_ON) )                                    \
		( * _func_trgCheck) (evtId, TRG_CLASS3_INDEX, NULL, arg1, arg2, arg3, arg4, arg5);                            \
        }

/******************************************************************************
*
* EVT_OBJ_SIG - signal event logging
*
*
* NOMANUAL
*/

#define EVT_OBJ_SIG(evtId, nParam, arg1, arg2)				\
    if ( ACTION_IS_SET )					\
        {                                                               \
        if ( WV_EVTCLASS_IS_SET(WV_CLASS_3|WV_ON) )                                  \
                        if (sigEvtRtn != NULL) \
                                (* sigEvtRtn) (evtId, nParam, arg1, arg2); \
        if ( TRG_EVTCLASS_IS_SET(TRG_CLASS_3|TRG_ON) )                                     \
		( * _func_trgCheck) (evtId, TRG_CLASS3_INDEX, NULL, arg1, arg2, NULL, NULL, NULL);                            \
        }

/******************************************************************************
*
* EVT_OBJ_EVENT - events (from eventLib) event logging
*
*
* NOMANUAL
*/

#define EVT_OBJ_EVENT(evtId, nParam, arg1, arg2, arg3)	                \
    if ( ACTION_IS_SET )					        \
        {                                                               \
        if ( WV_EVTCLASS_IS_SET(WV_CLASS_3|WV_ON) )                     \
            if (eventEvtRtn != NULL)                                    \
                (* eventEvtRtn) (evtId, nParam, arg1, arg2, arg3);      \
        if ( TRG_EVTCLASS_IS_SET(TRG_CLASS_3|TRG_ON) )                  \
		( * _func_trgCheck) (evtId, TRG_CLASS3_INDEX, NULL,     \
                                     arg1, arg2, arg3, NULL, NULL);     \
        }

/******************************************************************************
*
* INST_SIG_INSTALL - install event logging routine for signals
*
*
* NOMANUAL
*/

#define INST_SIG_INSTALL					\
    if ( ACTION_IS_SET )					\
        {                                                       \
        if ( WV_EVTCLASS_IS_SET(WV_CLASS_3|WV_ON) )                         \
            {                                                   \
                sigEvtRtn = _func_evtLogOIntLock;               \
            else                                                \
                sigEvtRtn = NULL;                               \
            }                                                   \
        }

/******************************************************************************
*
* EVT_OBJ_SM_MSGQ - object status event logging for SMO message queues.
*
*
* NOMANUAL
*/

#define EVT_OBJ_SM_MSGQ(evtId, arg1, arg2, arg3, arg4, arg5, arg6)     \
    if ( ACTION_IS_SET )					\
        {                                                               \
        if ( WV_EVTCLASS_IS_SET(WV_CLASS_3|WV_ON) )                                 \
                (* _func_evtLogOIntLock) (evtId, arg6, arg1, arg2, arg3,  \
                                          arg4, arg5);        \
        if ( TRG_EVTCLASS_IS_SET(TRG_CLASS_3|TRG_ON) )                                      \
		( * _func_trgCheck) (evtId, TRG_CLASS3_INDEX, arg1, arg2, arg3, arg4, arg5);                            \
        }


#endif	/* CPU */			/* end target-only section */
#endif  /* _ASMLANGUAGE */

#define WV_REV_ID_T2 0xb0b00000
#define WV_REV_ID_T3 0xcdcd0000

#define WV_EVT_PROTO_REV_1_0_FCS 1

#define WV_EVT_PROTO_REV_2_0_FCS 2
#define WV_EVT_PROTO_REV_3_0_FCS 3
#define WV_EVT_PROTO_REV_3_1_FCS 4
#define WV_EVT_PROTO_REV_2_2_FCS 5

#define WV_REV_ID_CURRENT WV_REV_ID_T2
#define WV_EVT_PROTO_REV_CURRENT WV_EVT_PROTO_REV_2_2_FCS

/* ranges for the different classes of events */

#define MIN_CONTROL_ID 	0
#define MAX_CONTROL_ID	49
#define MIN_CLASS1_ID	50
#define MAX_CLASS1_ID	599
#define MIN_INT_ID	102
#define PPC_DECR_INT_ID	599
#define MAX_INT_ID	599
#define MIN_CLASS2_ID	600
#define MAX_CLASS2_ID	9999
#define MIN_CLASS3_ID	10000
#define MAX_CLASS3_ID	19999 
#define MIN_RESERVE_ID	20000
#define MIN_RTI_RESERVE_ID      39900
#define MAX_RTI_RESERVE_ID      39999
#define MAX_RESERVE_ID	39999 
#define MIN_USER_ID	40000
#define MAX_USER_ID	65535

#define INT_LEVEL(eventNum) ((eventNum)-MIN_INT_ID)

#define IS_CLASS1_EVENT(event) \
    ((event >= MIN_CLASS1_ID) && (event <= MAX_CLASS1_ID))

#define IS_CLASS2_EVENT(event) \
    ((event >= MIN_CLASS2_ID) && (event <= MAX_CLASS2_ID))

#define IS_CLASS3_EVENT(event) \
    ((event >= MIN_CLASS3_ID) && (event <= MAX_CLASS3_ID))

#define IS_INT_ENT_EVENT(event) \
    ((event >= MIN_INT_ID) && (event <= MAX_INT_ID))

#define IS_CONTROL_EVENT(event) \
    ((event >= MIN_CONTROL_ID) && (event <= MAX_CONTROL_ID))

#define IS_USER_EVENT(event) \
    ((event >= MIN_USER_ID) && (event <= MAX_USER_ID))

#define CONTROL_EVENT(id) (MIN_CONTROL_ID + id)
#define CLASS1_EVENT(id)  (MIN_CLASS1_ID + id)
#define CLASS2_EVENT(id)  (MIN_CLASS2_ID + id)
#define CLASS3_EVENT(id)  (MIN_CLASS3_ID + id)
#define INT_EVENT(id)     (MIN_INT_ID + id)


/* EVENT Id's */

/* Interrupt events */

#define EVENT_INT_ENT(k) 		((k) + MIN_INT_ID)
    /* Param:
     *   short EVENT_INT_ENT(k), 
     *   int timeStamp
     */

#define EVENT_INT_EXIT 	 		(MIN_INT_ID - 1)
    /* Param:
     *   short EVENT_INT_EXIT, 
     *   int timeStamp
     */

#define EVENT_INT_EXIT_K                (MIN_INT_ID - 2)
    /* Param:
     *   short EVENT_INT_EXIT_K,
     *   int timeStamp
     */

/* Control events */

#define EVENT_BEGIN                     CONTROL_EVENT(0)
    /* Param:
     *   short EVENT_BEGIN, 
     *   int CPU, 
     *   int bspSize, 
     *   char * bspName, 
     *   int taskIdCurrent, 
     *   int collectionMode, 
     *   int revision
     */


#define EVENT_END                       CONTROL_EVENT(1)
    /* Param:
     *   short EVENT_END
     */


#define EVENT_TIMER_ROLLOVER            CONTROL_EVENT(2)
    /* Param:
     *   short EVENT_TIMER_ROLLOVER
     *   int timeStamp, 
     */

#define	EVENT_TASKNAME			CONTROL_EVENT(3)
    /* Param:
     *   short EVENT_TASKNAME, 
     *   int status, 
     *   int priority, 
     *   int taskLockCount
     *   int tid, 
     *   int nameSize, 
     *   char * name
     */

#define EVENT_CONFIG              	CONTROL_EVENT(4)
    /* Param:
     *   int revision,                  WV_REV_ID | WV_EVT_PROTO_REV
     *   int timestampFreq, 
     *   int timestampPeriod, 
     *   int autoRollover, 
     *   int clkRate,
     *   int collectionMode,
     *   int processorNum
     */


#define EVENT_BUFFER                    CONTROL_EVENT(5)
    /* Param:
     *   short EVENT_BUFFER,
     *   int taskIdCurrent
     */


#define EVENT_TIMER_SYNC		CONTROL_EVENT(6)
    /* Param:
     *   int timeStamp,
     *   int spare
     */

#define EVENT_LOGCOMMENT              CONTROL_EVENT(7)
     /* Param:
      *   int length
      *   string comment
      */

#define EVENT_ANY_EVENT			CONTROL_EVENT(48)
    /* This is added for triggering, when no particular event is required */

/* CLASS3 events */

#define	EVENT_TASKSPAWN				CLASS3_EVENT(0)
    /* Param:
     *   short EVENT_TASKSPAWN, 
     *   int timeStamp, 
     *   int options, 
     *   int entryPt, 
     *   intstackSize, 
     *   int priority, 
     *   int pTcb
     */

#define	EVENT_TASKDESTROY			CLASS3_EVENT(1)
    /* Param:
     *    short EVENT_TASKDESTROY, 
     *    int timeStamp, 
     *    int safeCnt, 
     *    int pTcb
     */

#define	EVENT_TASKDELAY				CLASS3_EVENT(2)
    /* Param:
     *   short EVENT_TASKDELAY, 
     *   int timeStamp, 
     *   int ticks
     */

#define	EVENT_TASKPRIORITYSET			CLASS3_EVENT(3)
    /* Param:
     *   short EVENT_TASKPRIORITYSET, 
     *   int timeStamp, 
     *   int oldPri, 
     *   int newPri, 
     *   int pTcb
     */

#define	EVENT_TASKSUSPEND			CLASS3_EVENT(4)
    /* Param:
     *   short EVENT_TASKSUSPEND, 
     *   int timeStamp, 
     *   int pTcb
     */

#define	EVENT_TASKRESUME			CLASS3_EVENT(5)
    /* Param:
     *   short EVENT_TASKRESUME, 
     *   int timeStamp, 
     *   int priority, 
     *   int pTcb
     */

#define	EVENT_TASKSAFE				CLASS3_EVENT(6)
    /* Param:
     *   short EVENT_TASKSAFE, 
     *   int timeStamp, 
     *   int safeCnt, 
     *   int tid
     */

#define	EVENT_TASKUNSAFE			CLASS3_EVENT(7)
    /* Param:
     *   short EVENT_TASKUNSAFE, 
     *   int timeStamp, 
     *   int safeCnt, 
     *   int tid
     */

#define	EVENT_SEMBCREATE			CLASS3_EVENT(8)
    /* Param:
     *   short EVENT_SEMBCREATE, 
     *   int timeStamp, 
     *   int semOwner, 
     *   int options, 
     *   int semId
     */

#define	EVENT_SEMCCREATE			CLASS3_EVENT(9)
    /* Param:
     *   short EVENT_SEMCCREATE, 
     *   int timeStamp, 
     *   int initialCount, 
     *   int options, 
     *   int semId
     */

#define	EVENT_SEMDELETE				CLASS3_EVENT(10)
    /* Param:
     *   short EVENT_SEMDELETE, 
     *   int timeStamp, 
     *   int qHead
     *   int recurse
     *   int state
     *   int semId
     */

#define	EVENT_SEMFLUSH				CLASS3_EVENT(11)
    /* Param:
     *   short EVENT_SEMFLUSH, 
     *   int timeStamp, 
     *   int qHead
     *   int recurse
     *   int state
     *   int semId
     */

#define	EVENT_SEMGIVE				CLASS3_EVENT(12)
    /* Param:
     *   short EVENT_SEMGIVE, 
     *   int timeStamp, 
     *   int recurse, 
     *   int semOwner, 
     *   int semId
     */

#define	EVENT_SEMMCREATE			CLASS3_EVENT(13)
    /* Param:
     *   short EVENT_SEMMCREATE, 
     *   int timeStamp, 
     *   int semOwner, 
     *   int options, 
     *   int semId
     */

#define	EVENT_SEMMGIVEFORCE			CLASS3_EVENT(14)
    /* Param:
     *   short EVENT_SEMMGIVEFORCE, 
     *   int timeStamp, 
     *   int semOwner, 
     *   int options, 
     *   int semId
     */

#define	EVENT_SEMTAKE				CLASS3_EVENT(15)
    /* Param:
     *   short EVENT_SEMTAKE, 
     *   int timeStamp, 
     *   int recurse, 
     *   int semOwner, 
     *   int semId
     */

#define	EVENT_WDCREATE				CLASS3_EVENT(16)
    /* Param:
     *   short EVENT_WDCREATE, 
     *   int timeStamp, 
     *   int wdId
     */

#define	EVENT_WDDELETE				CLASS3_EVENT(17)
    /* Param:
     *   short EVENT_WDDELETE, 
     *   int timeStamp, 
     *   int wdId
     */

#define	EVENT_WDSTART				CLASS3_EVENT(18)
    /* Param:
     *   short EVENT_WDSTART, 
     *   int timeStamp, 
     *   int delay, 
     *   int wdId
     */

#define	EVENT_WDCANCEL				CLASS3_EVENT(19)
    /* Param:
     *   short EVENT_WDCANCEL, 
     *   int timeStamp, 
     *   int wdId
     */

#define	EVENT_MSGQCREATE			CLASS3_EVENT(20)
    /* Param:
     *   short EVENT_MSGQCREATE, 
     *   int timeStamp, 
     *   int options, 
     *   int maxMsgLen, 
     *   int maxMsg, 
     *   int msgId
     */

#define	EVENT_MSGQDELETE			CLASS3_EVENT(21)
    /* Param:
     *   short EVENT_MSGQDELETE, 
     *   int timeStamp, 
     *   int msgId
     */

#define	EVENT_MSGQRECEIVE			CLASS3_EVENT(22)
    /* Param:
     *   short EVENT_MSGQRECEIVE, 
     *   int timeStamp, 
     *   int timeout, 
     *   int bufSize, 
     *   int buffer, 
     *   int msgId
     */

#define	EVENT_MSGQSEND				CLASS3_EVENT(23)
    /* Param:
     *   short EVENT_MSGQSEND, 
     *   int timeStamp, 
     *   int priority, 
     *   int timeout, 
     *   int bufSize, 
     *   int buffer, 
     *   int msgId
     */

#define	EVENT_SIGNAL				CLASS3_EVENT(24)
    /* Param:
     *   short EVENT_SIGNAL, 
     *   int timeStamp, 
     *   int handler, 
     *   int signo
     */

#define	EVENT_SIGSUSPEND			CLASS3_EVENT(25)
    /* Param:
     *   short EVENT_SIGSUSPEND, 
     *   int timeStamp, 
     *   int pSet
     */

#define	EVENT_PAUSE				CLASS3_EVENT(26)
    /* Param:
     *   short EVENT_PAUSE, 
     *   int timeStamp, 
     *   int tid
     */

#define	EVENT_KILL				CLASS3_EVENT(27)
    /* Param:
     *   short EVENT_KILL, 
     *   int timeStamp, 
     *   int tid, 
     *   int signo
     */

#define EVENT_SAFE_PEND                         CLASS3_EVENT(28)
    /* Param:
     *   short EVENT_SAFE_PEND, 
     *   int tid
     */

#define EVENT_SIGWRAPPER                        CLASS3_EVENT(29)
    /* Param:
     *   short EVENT_SIGWRAPPER, 
     *   int signo, 
     *   int tid
     */


#define EVENT_MEMALLOC                          CLASS3_EVENT(30)
    /* Param:
     *   int timeStamp,
     *   unsigned nBytes,
     *   unsigned nBytesPlusHeaderAlign,
     *   int pBlock,
     *   int partId
     */

#define EVENT_MEMFREE                           CLASS3_EVENT(31)
    /* Param:
     *   int timeStamp,
     *   unsigned nBytesPlusHeaderAlign,
     *   int pBlock,
     *   int partId
     */

#define EVENT_MEMPARTCREATE                     CLASS3_EVENT(32)
    /* Param:
     *   int timeStamp,
     *   unsigned poolSize,
     *   int partId
     */

#define EVENT_MEMREALLOC                        CLASS3_EVENT(33)
    /* Param:
     *   int timeStamp,
     *   unsigned nBytes,
     *   unsigned nBytesPlusHeaderAlign,
     *   int pBlock,
     *   int partId
     */

#define EVENT_MEMADDTOPOOL                      CLASS3_EVENT(34)
    /* Param:
     *   int timeStamp,
     *   unsigned poolSize,
     *   int partId
     */

#define EVENT_EVENTSEND				CLASS3_EVENT(57)
    /* Param:
     *   int taskId,
     *   UINT32 events
     */

#define EVENT_EVENTRECEIVE			CLASS3_EVENT(58)
    /* Param:
     *   UINT32 events,
     *   int timeout,
     *   UINT8 flags
     */

/* CLASS2 events */

#define	EVENT_WINDSPAWN				CLASS2_EVENT(0)
    /* Param:
     *   short EVENT_WINDSPAWN, 
     *   int pTcb, 
     *   int priority
     */

#define	EVENT_WINDDELETE			CLASS2_EVENT(1)
    /* Param:
     *   short EVENT_WINDDELETE, 
     *   int pTcb
     */

#define	EVENT_WINDSUSPEND			CLASS2_EVENT(2)
    /* Param:
     *   short EVENT_WINDSUSPEND, 
     *   int pTcb
     */

#define	EVENT_WINDRESUME			CLASS2_EVENT(3)
    /* Param:
     *   short EVENT_WINDRESUME, 
     *   int pTcb
     */

#define	EVENT_WINDPRIORITYSETRAISE		CLASS2_EVENT(4)
    /* Param:
     *   short EVENT_WINDPRIORITYSETRAISE, 
     *   int pTcb, 
     *   int oldPriority, 
     *   int priority
     */


#define	EVENT_WINDPRIORITYSETLOWER		CLASS2_EVENT(5)
    /* Param:
     *   short EVENT_WINDPRIORITYSETLOWER, 
     *   int pTcb, 
     *   int oldPriority, 
     *   int priority
     */

#define	EVENT_WINDSEMDELETE			CLASS2_EVENT(6)
    /* Param:
     *   short EVENT_WINDSEMDELETE, 
     *   int semId
     */

#define	EVENT_WINDTICKANNOUNCETMRSLC		CLASS2_EVENT(7)
    /* Param:
     *   short EVENT_WINDTICKANNOUNCETMRSLC
     */

#define	EVENT_WINDTICKANNOUNCETMRWD		CLASS2_EVENT(8)
    /* Param:
     *   short EVENT_WINDTICKANNOUNCETMRWD, 
     *   int wdId
     */

#define	EVENT_WINDDELAY				CLASS2_EVENT(9)
    /* Param:
     *   short EVENT_WINDDELAY
     *   int ticks
     */

#define	EVENT_WINDUNDELAY			CLASS2_EVENT(10)
    /* Param:
     *   short EVENT_WINDUNDELAY, 
     *   int pTcb
     */

#define	EVENT_WINDWDSTART			CLASS2_EVENT(11)
    /* Param:
     *   short EVENT_WINDWDSTART, 
     *   int wdId
     */

#define	EVENT_WINDWDCANCEL			CLASS2_EVENT(12)
    /* Param:
     *   short EVENT_WINDWDCANCEL, 
     *   int wdId
     */

#define	EVENT_WINDPENDQGET			CLASS2_EVENT(13)
    /* Param:
     *   short EVENT_WINDPENDQGET, 
     *   int pTcb
     */

#define	EVENT_WINDPENDQFLUSH			CLASS2_EVENT(14)
    /* Param:
     *   short EVENT_WINDPENDQFLUSH, 
     *   int pTcb
     */

#define	EVENT_WINDPENDQPUT			CLASS2_EVENT(15)
    /* Param:
     *   short EVENT_WINDPENDQPUT
     */

#define	EVENT_WINDPENDQTERMINATE		CLASS2_EVENT(17)
    /* Param:
     *   short EVENT_WINDPENDQTERMINATE, 
     *   int pTcb
     */

#define	EVENT_WINDTICKUNDELAY			CLASS2_EVENT(18)
    /* Param:
     *   short EVENT_WINDTICKUNDELAY, 
     *   int pTcb
     */

#define EVENT_OBJ_TASK				CLASS2_EVENT(19)
    /* Param:
     *   short EVENT_OBJ_TASK, 
     *   int pTcb
     */

#define EVENT_OBJ_SEMGIVE			CLASS2_EVENT(20)
    /* Param:
     *   short EVENT_OBJ_SEMGIVE, 
     *   int semId
     */

#define EVENT_OBJ_SEMTAKE			CLASS2_EVENT(21)
    /* Param:
     *   short EVENT_OBJ_SEMTAKE, 
     *   int semId
     */

#define EVENT_OBJ_SEMFLUSH			CLASS2_EVENT(22)
    /* Param:
     *   short EVENT_OBJ_SEMFLUSH, 
     *   int semId
     */

#define EVENT_OBJ_MSGSEND			CLASS2_EVENT(23)
    /* Param:
     *   short EVENT_OBJ_MSGSEND, 
     *   int msgQId
     */

#define EVENT_OBJ_MSGRECEIVE			CLASS2_EVENT(24)
    /* Param:
     *   short EVENT_OBJ_MSGRECEIVE, 
     *   int msgQId
     */

#define EVENT_OBJ_MSGDELETE			CLASS2_EVENT(25)
    /* Param:
     *   short EVENT_OBJ_MSGDELETE, 
     *   int msgQId
     */

#define EVENT_OBJ_SIGPAUSE			CLASS2_EVENT(26)
    /* Param:
     *   short EVENT_OBJ_SIGPAUSE, 
     *   int qHead
     */

#define EVENT_OBJ_SIGSUSPEND			CLASS2_EVENT(27)
    /* Param:
     *   short EVENT_OBJ_SIGSUSPEND, 
     *   int sigset
     */

#define EVENT_OBJ_SIGKILL			CLASS2_EVENT(28)
    /* Param:
     *   short EVENT_OBJ_SIGKILL, 
     *   int tid
     */

#define EVENT_WINDTICKTIMEOUT                   CLASS2_EVENT(31)
    /* Param:
     *   short EVENT_WINDTICKTIMEOUT,
     *   int tId
     */

#define EVENT_OBJ_SIGWAIT                       CLASS2_EVENT(32)
    /* Param:
     *   short EVENT_OBJ_SIGWAIT, 
     *   int tid
     */

#define	EVENT_OBJ_EVENTSEND			CLASS2_EVENT(35)
    /* Param:
     *   short EVENT_OBJ_EVENTSEND 
     */

#define	EVENT_OBJ_EVENTRECEIVE			CLASS2_EVENT(36)
    /* Param:
     *   short EVENT_OBJ_EVENTRECEIVE
     */


/* CLASS1 events */

#define EVENT_WIND_EXIT_DISPATCH        	CLASS1_EVENT(2)
    /* Param:
     *   short EVENT_WIND_EXIT_DISPATCH, 
     *   int timestamp,
     *   int tId,
     *   int priority
     */

#define EVENT_WIND_EXIT_NODISPATCH        	CLASS1_EVENT(3)
    /* Param:
     *   short EVENT_WIND_EXIT_NODISPATCH, 
     *   int timestamp,
     *   int priority
     */

#define EVENT_WIND_EXIT_DISPATCH_PI        	CLASS1_EVENT(4)
    /* Param:
     *   short EVENT_WIND_EXIT_DISPATCH_PI, 
     *   int timestamp,
     *   int tId,
     *   int priority
     */

#define EVENT_WIND_EXIT_NODISPATCH_PI        	CLASS1_EVENT(5)
    /* Param:
     *   short EVENT_WIND_EXIT_NODISPATCH_PI, 
     *   int timestamp,
     *   int priority
     */

#define EVENT_DISPATCH_OFFSET   0xe
#define EVENT_NODISPATCH_OFFSET   0xa
    /*
     * This definition is needed for the logging of the above _PI events
     */

#define	EVENT_WIND_EXIT_IDLE			CLASS1_EVENT(6)
    /* Param:
     *   short EVENT_WIND_EXIT_IDLE, 
     *   int timestamp
     */

#define	EVENT_TASKLOCK				CLASS1_EVENT(7)
    /* Param:
     *   short EVENT_TASKLOCK, 
     *   int timeStamp
     */

#define	EVENT_TASKUNLOCK			CLASS1_EVENT(8)
    /* Param:
     *   short EVENT_TASKUNLOCK, 
     *   int timeStamp
     */

#define	EVENT_TICKANNOUNCE			CLASS1_EVENT(9)
    /* Param:
     *   short EVENT_TICKANNOUNCE, 
     *   int timeStamp
     */

#define	EVENT_EXCEPTION				CLASS1_EVENT(10)
    /* Param:
     *   short EVENT_EXCEPTION, 
     *   int timeStamp, 
     *   int exception
     */

#define EVENT_TASK_STATECHANGE			CLASS1_EVENT(11)
    /* Param:
     *   short EVENT_STATECHANGE
     *   int timeStamp,
     *   int taskId,
     *   int newState
     */

/* pseudo events - generated host side */

#define MIN_PSEUDO_EVENT                        CLASS1_EVENT(20)

#define EVENT_STATECHANGE                       CLASS1_EVENT(20)
    /* Param:
     *   short EVENT_STATECHANGE
     *   int newState
     */

#define EVENT_NIL                               CLASS1_EVENT(21)
    /* Param:
     *   short EVENT_NIL
     */

#define EVENT_VALUE                             CLASS1_EVENT(22)
    /* Param:
     *   short EVENT_VALUE
     *   int value
     */

#define MAX_PSEUDO_EVENT                        CLASS1_EVENT(24)

#define IS_PSEUDO_EVENT(event) \
    ((event >= MIN_PSEUDO_EVENT) && (event <= MAX_PSEUDO_EVENT))





#ifdef CPU   /* This is for target side build only */

#ifndef _BYTE_ORDER
#error _BYTE_ORDER must be defined
#endif /* _BYTE_ORDER */

#ifndef _BIG_ENDIAN
#error _BIG_ENDIAN must be defined
#endif /* _BIG_ENDIAN */

/*
 * Alignment macros used to store unaligned short (16 bits) and unaligned
 * int (32 bits).
 */

#define EVT_STORE_UINT16(pBuf, event_id) \
    *pBuf++ = (event_id)

#if (_BYTE_ORDER==_BIG_ENDIAN)

#   if  CPU_FAMILY==MC680X0 
    	/* unaligned access supported */
#   	define EVT_STORE_UINT32(pBuf, value) \
    		*pBuf++ = (value)
#   else /* CPU_FAMILY==MC680X0 */
	/* unaligned access not supported */
#	define EVT_STORE_UINT32(pBuf, value) \
    		do { *((short *) pBuf) = (value) >> 16; \
	 	     *(((short *) pBuf) + 1) = (value); \
	 	     pBuf++; } while (0)
#   endif /* CPU_FAMILY==MC680X0 */

#else	/* (_BYTE_ORDER==_BIG_ENDIAN) */

#   define EVT_STORE_UINT32(pBuf, value) \
    		do { *((short *) pBuf) = (value); \
	 	     *(((short *) pBuf) + 1) = (value) >> 16; \
	 	     pBuf++; } while (0)

#endif	/* (_BYTE_ORDER==_BIG_ENDIAN) */

#endif /* CPU */


/*
 * Macro for event logging of intEnt events for PPC architechtures.
 * The PPC architecture is different in that there is only one external
 * interrupt line coming into the CPU. At the time of the intEnt, the
 * external interrupt number is unknown, meaning that logging cannot
 * take place at this time. In this architechture, the logging of an
 * intEnt event is deferred until the interrupt controller driver
 * is exectued.
 * Since the logging has to take place in drivers and/or BSP world files,
 * the details of how the logging to be done has been deferred to a macro 
 * which is defined here.
 *
 * Note: This replaces the previous technique of saving the timestamp
 * during the intEnt code to be used later, when the logging was done.
 * SPR 21868 refers to a problem in which this can give rise to out-of-order
 * timestamps in the event log.
 */

#if (CPU_FAMILY == PPC) || (CPU_FAMILY == ARM)
#define WV_EVT_INT_ENT(intNum)	    EVT_CTX_0(EVENT_INT_ENT((UINT32)intNum))
#endif /* CPU_FAMILY == PPC */

#ifdef __cplusplus
}
#endif

#endif /* __INCeventph*/



