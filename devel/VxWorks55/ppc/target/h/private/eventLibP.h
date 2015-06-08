/* eventLibP.h - private VxWorks events library header file */

/* Copyright 2001-2003 Wind River Systems, Inc. */

/*
modification history
--------------------
01e,20jan03,bwa  Added EVENTS_SYSFLAGS_DEL_RSRC definition.
01d,15jan02,bwa  Created list of events reserved for system use.
01c,06nov01,aeg  added eventRsrcShow() and eventTaskShow() prototypes.
01b,17oct01,bwa  Added eventRsrcSend() prototype.
01a,20sep01,bwa  Created
*/

#ifndef __INCeventLibPh
#define __INCeventLibPh

#ifdef __cplusplus
extern "C" {
#endif

#ifndef	_ASMLANGUAGE
#include "vxWorks.h"

#if CPU_FAMILY==I960
#pragma align 1			/* tell gcc960 not to optimize alignments */
#endif	/* CPU_FAMILY==I960 */

extern VOIDFUNCPTR eventEvtRtn; /* windview - level 1 event logging */

/* event structure needed in TCB */

typedef struct eventsCb
    {
    UINT32 wanted;	/* 0x00: events wanted				*/
    volatile UINT32 received;	/* 0x04: all events received		*/
    UINT8  options;	/* 0x08: user options				*/
    UINT8  sysflags;	/* 0x09: flags used by internal code only	*/
    UINT8  pad[2];	/* 0x0a: alignment on 32bit, possible extension	*/
    } EVENTS;		/* 0x0c: total size				*/

/* EVENTS.options masks */

#define EVENTS_WAIT_MASK	0x01

/* EVENTS system flags */

#define EVENTS_SYSFLAGS_WAITING	  0x01	/* task is waiting for events 	*/
#define EVENTS_SYSFLAGS_DEL_RSRC  0x02	/* task is deleting rsrc on */
					/* which it is registered	*/
#define EVENTS_SYSFLAGS_UNUSED	  0xfc	/* unused, for future extension	*/

/* event structure needed in resource (semaphores, msg queues) structures */

typedef struct eventsResourceCb
    {
    UINT32 registered;	/* 0x00: events registered for that resource	*/
    int    taskId;	/* 0x04: task registered for that resource	*/
    UINT8  options;	/* 0x08: user options				*/
    UINT8  pad[3];	/* 0x09: alignment on 32bit, possible extension	*/
    } EVENTS_RSRC;	/* 0x0c: total size				*/

#if CPU_FAMILY==I960
#pragma align 0			/* tell gcc960 not to optimize alignments */
#endif	/* CPU_FAMILY==I960 */

/* These events are for system use only. */

#define VXEV25 0x01000000
#define VXEV26 0x02000000
#define VXEV27 0x04000000
#define VXEV28 0x08000000
#define VXEV29 0x10000000
#define VXEV30 0x20000000
#define VXEV31 0x40000000
#define VXEV32 0x80000000

#define VXEV_RESERVED 0xff000000

#if defined(__STDC__) || defined(__cplusplus)

extern void 	eventLibInit	(void);
extern void 	eventInit	(EVENTS_RSRC * pEvRsrc);
extern void 	eventTerminate 	(const EVENTS_RSRC * pEvRsrc);
extern void 	semEvLibInit	(void);
extern void 	msgQEvLibInit	(void);
extern STATUS 	eventStart	(OBJ_ID objId, EVENTS_RSRC * pEvRsrc,
				 FUNCPTR isRsrcFree, UINT32 events,
				 UINT8 options);
extern STATUS	eventRsrcSend	(int taskId, UINT32 events);
extern STATUS	eventRsrcShow	(EVENTS_RSRC * pEvRsrc);
extern STATUS   eventTaskShow	(EVENTS	* pEvents);

#else /* __STDC__ */

extern void 	eventLibInit   	();
extern void 	eventInit	();
extern void 	eventTerminate 	();
extern void 	semEvLibInit	();
extern void 	msgQEvLibInit	();
extern STATUS 	eventStart	();
extern STATUS	eventRsrcSend	();
extern STATUS	eventRsrcShow	();
extern STATUS   eventTaskShow	();

#endif /* __STDC */

#else	/* _ASMLANGUAGE */

#define EVENTS_RSRC_REGISTERED		0x0
#define EVENTS_RSRC_TASKID		0x4
#define EVENTS_RSRC_OPTIONS		0x8

#endif	/* _ASMLANGUAGE */

#ifdef __cplusplus
}
#endif

#endif /* __INCeventLibPh */

