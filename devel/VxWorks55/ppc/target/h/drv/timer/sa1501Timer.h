/* sa1501Timer.h - Digital Semiconductor SA-1501 timer header */

/* Copyright 1996-1998 Wind River Systems, Inc. */

/*
modification history
--------------------
01a,25nov98,jpd	 written, based on ambaTimer.h version 01c.
*/

#ifndef	__INCsa1501Timerh
#define	__INCsa1501Timerh

#ifdef __cplusplus
extern "C" {
#endif

/* register definitions */

#ifndef _ASMLANGUAGE
#define SA1501_TIMER_T0LIMIT(base)	\
	    (volatile UINT32 *)((base) + 0x00)	/* T0 Limit (R/W) */
#define SA1501_TIMER_T0VALUE(base) \
	    (volatile UINT32 *)((base) + 0x10)	/* T0 Value (R/O) */
#define SA1501_TIMER_T0RESET(base) \
	    (volatile UINT32 *)((base) + 0x50)	/* T0 Reset (R/W) */

#define SA1501_TIMER_T1LIMIT(base)	\
	    (volatile UINT32 *)((base) + 0x04)	/* T1 Limit (R/W) */
#define SA1501_TIMER_T1VALUE(base) \
	    (volatile UINT32 *)((base) + 0x14)	/* T1 Value (R/O) */
#define SA1501_TIMER_T1RESET(base) \
	    (volatile UINT32 *)((base) + 0x54)	/* T1 Reset (R/W) */

#define SA1501_TIMER_T2LIMIT(base)	\
	    (volatile UINT32 *)((base) + 0x08)	/* T2 Limit (R/W) */
#define SA1501_TIMER_T2VALUE(base) \
	    (volatile UINT32 *)((base) + 0x18)	/* T2 Value (R/O) */
#define SA1501_TIMER_T2RESET(base) \
	    (volatile UINT32 *)((base) + 0x58)	/* T2 Reset (R/W) */

#define SA1501_TIMER_T3LIMIT(base)	\
	    (volatile UINT32 *)((base) + 0x0C)	/* T3 Limit (R/W) */
#define SA1501_TIMER_T3VALUE(base) \
	    (volatile UINT32 *)((base) + 0x1C)	/* T3 Value (R/O) */
#define SA1501_TIMER_T3RESET(base) \
	    (volatile UINT32 *)((base) + 0x5C)	/* T3 Reset (R/W) */

#define SA1501_TIMER_WDCTL(base) \
	    (volatile UINT32 *)((base) + 0x7C)	/* T3 watchdog ctrl (W/O) */

#else /* _ASMLANGUAGE */

#define SA1501_TIMER_T0LIMIT(base) ((base) + 0x00)
#define SA1501_TIMER_T0VALUE(base) ((base) + 0x10)
#define SA1501_TIMER_T0RESET(base) ((base) + 0x50)

#define SA1501_TIMER_T1LIMIT(base) ((base) + 0x04)
#define SA1501_TIMER_T1VALUE(base) ((base) + 0x14)
#define SA1501_TIMER_T1RESET(base) ((base) + 0x54)

#define SA1501_TIMER_T2LIMIT(base) ((base) + 0x08)
#define SA1501_TIMER_T2VALUE(base) ((base) + 0x18)
#define SA1501_TIMER_T2RESET(base) ((base) + 0x58)

#define SA1501_TIMER_T3LIMIT(base) ((base) + 0x0C)
#define SA1501_TIMER_T3VALUE(base) ((base) + 0x1C)
#define SA1501_TIMER_T3RESET(base) ((base) + 0x5C)

#define SA1501_TIMER_WDCTL(base)   ((base) + 0x7C)


#endif /* _ASMLANGUAGE */


#ifdef __cplusplus
}
#endif

#endif	/* __INCsa1501Timerh */

