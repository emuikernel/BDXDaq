/* esfSimsparc.h - simsparc exception stack frame */

/* Copyright 1993-1995 Wind River Systems, Inc. */

/*
modification history
--------------------
01a,07jun95,ism  derived from simsparc
*/

#ifndef __INCesfSimsparch
#define __INCesfSimsparch

#ifdef __cplusplus
extern "C" {
#endif

#include "regs.h"
#include "excLib.h"

#define ESF             EXC_INFO

/* Overflow/Underflow Stack Frame */

#define STACK_FRAME      0x00                  /* Stack Frame */
#define SF_LOCALS        (STACK_FRAME + 0x00)  /* Local Registers */
#define SF_INS           (STACK_FRAME + 0x20)  /* In Registers */
#define SF_SPARE         (STACK_FRAME + 0x5C)  /* Spare Slot */
#define STACK_FRAME_SIZE 0x60


#define	INIT_STACK_FRAME	STACK_ROUND_UP ((((MAX_TASK_ARGS - 6) \
				* sizeof (int)) + (STACK_FRAME_SIZE - 4)))

#ifdef __cplusplus
}
#endif

#endif /* __INCesfSimsparch */
