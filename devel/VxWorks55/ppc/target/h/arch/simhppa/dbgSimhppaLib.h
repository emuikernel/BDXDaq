/* dbgSimhppaLib.h - simhppa debugger header */

/* Copyright 1993-1994 Wind River Systems, Inc. */

/*
modification history
-------------------
01d,16mar98,dbt  modified for new breakpoint scheme.
01c,20jul94,ms   allowed stack trace and cret.
01b,25jan94,gae  made BREAK_ESF same as EXC_INFO.
01a,11aug93,gae  from rrr.
*/

#ifndef __INCdbgSimhppaLibh
#define __INCdbgSimhppaLibh

#ifdef __cplusplus
extern "C" {
#endif

#include "excLib.h"

#define	BREAK_ESF	EXC_INFO
#define	TRACE_ESF	BREAK_ESF		/* in case needed */

/*#define DBG_TRAP_NUM    5 */

#define DBG_BREAK_INST		0x0
#define DBG_NO_SINGLE_STEP	1		/* no trace support */

#ifdef __cplusplus
}
#endif

#endif /* __INCdbgSimhppaLibh */
