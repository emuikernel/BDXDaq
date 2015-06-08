/* eventLib.h - events library header file */

/* Copyright 2001-2003 Wind River Systems, Inc. */

/*
modification history
--------------------
01e,18nov02,bwa  Added EVENTS_KEEP_UNWANTED option (SPR 84417).
01d,15jan02,bwa  Removed events 25 to 32 from list of events available to user.
01c,13nov01,bwa  Changed EVENTxx defs to VXEVxx.
01b,17oct01,bwa  Added #include "vwModNum.h". Added 2 errnos.
01a,20sep01,bwa  Created
*/

#ifndef __INCeventLibh
#define __INCeventLibh

#ifdef __cplusplus
extern "C" {
#endif

#include "vxWorks.h"
#include "vwModNum.h"

/* errnos */

#define S_eventLib_TIMEOUT			(M_eventLib | 0x0001)
#define S_eventLib_NOT_ALL_EVENTS		(M_eventLib | 0x0002)
#define S_eventLib_ALREADY_REGISTERED		(M_eventLib | 0x0003)
#define S_eventLib_EVENTSEND_FAILED		(M_eventLib | 0x0004)
#define S_eventLib_ZERO_EVENTS			(M_eventLib | 0x0005)
#define S_eventLib_TASK_NOT_REGISTERED		(M_eventLib | 0x0006)
#define S_eventLib_NULL_TASKID_AT_INT_LEVEL	(M_eventLib | 0x0007)

/* options */

#define EVENTS_WAIT_ALL		0x00	/* wait for all events to occur */
#define EVENTS_WAIT_ANY		0x01	/* wait for one of many events  */
#define EVENTS_RETURN_ALL	0x02	/* return all events received   */
#define EVENTS_KEEP_UNWANTED	0x04	/* don't clear unwanted events	*/
#define EVENTS_FETCH		0x80	/* return events received immediately */

/* resource-related options */

#define EVENTS_OPTIONS_NONE	0x00
#define EVENTS_SEND_ONCE	0x01  /* unregister events after eventSend   */
#define EVENTS_ALLOW_OVERWRITE	0x02  /* can overwrite previous registration */
#define EVENTS_SEND_IF_FREE	0x04  /* send events in xxxEvStart()         */

/*
 * Defines for all 24 events available to users. Events 25 to 32 are reserved
 * for system use and should never be used outside of that scope
 */

#define VXEV01 0x00000001
#define VXEV02 0x00000002
#define VXEV03 0x00000004
#define VXEV04 0x00000008
#define VXEV05 0x00000010
#define VXEV06 0x00000020
#define VXEV07 0x00000040
#define VXEV08 0x00000080
#define VXEV09 0x00000100
#define VXEV10 0x00000200
#define VXEV11 0x00000400
#define VXEV12 0x00000800
#define VXEV13 0x00001000
#define VXEV14 0x00002000
#define VXEV15 0x00004000
#define VXEV16 0x00008000
#define VXEV17 0x00010000
#define VXEV18 0x00020000
#define VXEV19 0x00040000
#define VXEV20 0x00080000
#define VXEV21 0x00100000
#define VXEV22 0x00200000
#define VXEV23 0x00400000
#define VXEV24 0x00800000

#define VXEV_USER_MASK 0x00ffffff

#ifndef	_ASMLANGUAGE

/* function declaration */

#if defined(__STDC__) || defined(__cplusplus)

extern STATUS eventSend (int taskId, UINT32 events);
extern STATUS eventReceive (
			    UINT32 events,
			    UINT8 options,
			    int timeout,
			    UINT32 *eventsReceived
			   );
extern STATUS eventClear (void);

#else

extern STATUS eventSend ();
extern STATUS eventReceive ();
extern STATUS eventClear ();

#endif

#endif	/* _ASMLANGUAGE */

#ifdef __cplusplus
}
#endif

#endif /* __INCeventLibh */

