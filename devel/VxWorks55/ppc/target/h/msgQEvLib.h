/* msgQEvLib.h - message queue events library header file */

/* Copyright 2001 Wind River Systems, Inc. */

/*
modification history
--------------------
01a,20sep01,bwa  Written.
*/

#ifndef __INCmsgQEvLibh
#define __INCmsgQEvLibh

#ifdef __cplusplus
extern "C" {
#endif

#include "vxWorks.h"
#include "msgQLib.h"

#ifndef	_ASMLANGUAGE

/* function declarations */

#if defined(__STDC__) || defined(__cplusplus)

extern STATUS	msgQEvStart (MSG_Q_ID msgQId, UINT32 events, UINT8 options);
extern STATUS	msgQEvStop  (MSG_Q_ID msgQId);

#else /* __STDC__ */

extern STATUS	msgQEvStart ();
extern STATUS	msgQEvStop  ();

#endif /* __STDC__ */

#endif	/* _ASMLANGUAGE */

#ifdef __cplusplus
}
#endif

#endif /* __INCmsgQEvLibh */
