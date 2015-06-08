/* ambaTimer.h - ARM AMBA timer header */

/* Copyright 1996-1997 Wind River Systems, Inc. */

/*
modification history
--------------------
01d,31oct01,rec  integrator AP changes
01c,24nov97,jpd  renamed from amba.h to ambaTimer.h.
01b,18aug97,jpd  Added timers 3 and 4, added TC_EXTERNAL definition.
01a,10jul96,jpd	 Written, based on i8253.h version 01b.
*/

#ifndef	__INCambaTimerh
#define	__INCambaTimerh

#ifdef __cplusplus
extern "C" {
#endif

/* default definitions */


/* register definitions */

#ifndef AMBA_BASE_OFFSET
#define AMBA_BASE_OFFSET 0X20
#endif

#define AMBA_TIMER_T1LOAD(base)	\
	    (volatile UINT32 *)((base) + 0x00)	/* T1 Load (R/W) */
#define AMBA_TIMER_T1VALUE(base) \
	    (volatile UINT32 *)((base) + 0x04)	/* T1 Value (R/O) */
#define AMBA_TIMER_T1CTRL(base) \
	    (volatile UINT32 *)((base) + 0x08)	/* T1 Control (R/W) */
#define AMBA_TIMER_T1CLEAR(base) \
	    (volatile UINT32 *)((base) + 0x0C)	/* T1 Clear (W/O) */

#define AMBA_TIMER_T2LOAD(base) \
        (volatile UINT32 *)((base) + AMBA_BASE_OFFSET + 0x00)  /* T2 Load (R/W) */
#define AMBA_TIMER_T2VALUE(base) \
        (volatile UINT32 *)((base) + AMBA_BASE_OFFSET + 0x04)  /* T2 Value (R/O) */
#define AMBA_TIMER_T2CTRL(base) \
        (volatile UINT32 *)((base) + AMBA_BASE_OFFSET + 0x08)  /* T2 Control (R/W) */
#define AMBA_TIMER_T2CLEAR(base) \
        (volatile UINT32 *)((base) + AMBA_BASE_OFFSET + 0x0C)  /* T2 Clear (W/O) */

#define AMBA_TIMER_T3LOAD(base) \
        (volatile UINT32 *)((base) + 2 * AMBA_BASE_OFFSET + 0x00)  /* T3 Load (R/W) */
#define AMBA_TIMER_T3VALUE(base) \
        (volatile UINT32 *)((base) + 2 * AMBA_BASE_OFFSET + 0x04)  /* T3 Value (R/O) */
#define AMBA_TIMER_T3CTRL(base) \
        (volatile UINT32 *)((base) + 2 * AMBA_BASE_OFFSET + 0x08)  /* T3 Control (R/W) */
#define AMBA_TIMER_T3CLEAR(base) \
        (volatile UINT32 *)((base) + 2 * AMBA_BASE_OFFSET + 0x0C)  /* T3 Clear (W/O) */

#define AMBA_TIMER_T4LOAD(base) \
        (volatile UINT32 *)((base) + 3 * AMBA_BASE_OFFSET + 0x00)  /* T4 Load (R/W) */
#define AMBA_TIMER_T4VALUE(base) \
        (volatile UINT32 *)((base) + 3 * AMBA_BASE_OFFSET + 0x04)  /* T4 Value (R/O) */
#define AMBA_TIMER_T4CTRL(base) \
        (volatile UINT32 *)((base) + 3 * AMBA_BASE_OFFSET + 0x08)  /* T4 Control (R/W) */
#define AMBA_TIMER_T4CLEAR(base) \
        (volatile UINT32 *)((base) + 3 * AMBA_BASE_OFFSET + 0x0C)  /* T4 Clear (W/O) */

/* Timer Control register bit definitions */

#define TC_DIV1		0
#define TC_DIV16	(1<<2)
#define TC_DIV256	(1<<3)
#define	TC_EXTERNAL	(3<<2) 
#define TC_PERIODIC	(1<<6)
#define TC_FREERUN	0
#define TC_ENABLE	(1<<7)
#define	TC_DISABLE	0

#ifdef __cplusplus
}
#endif

#endif	/* __INCambaTimerh */

