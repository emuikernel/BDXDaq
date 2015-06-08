/* ppc403Timerh.h - PPC 403 Timers header */

/* Copyright 1984-2002, Wind River Systems, Inc. */

/*
modification history
--------------------
01c,26apr02,dat  Adding cplusplus protection, SPR 75017
01b,19sep01,pch  Fix reversal of PIT and FIT in comments (for SPR 20698)
01a,23feb96,tam  created.
*/


#ifndef	__INCppc403Timerh
#define __INCppc403Timerh	

#ifdef __cplusplus
extern "C" {
#endif

/* defines */

#define _PPC403_TSR_ENW		0x80000000	/* enable next watchdog mask */
#define _PPC403_TSR_WIS		0x40000000	/* WDT interrupt status mask */
#define _PPC403_TSR_WRS		0x30000000	/* watchdog reset status mask */
#define _PPC403_TSR_PIS		0x08000000	/* PIT interrupt status mask */
#define _PPC403_TSR_FIS		0x04000000	/* FIT interrupt status mask */

#define _PPC403_TCR_WP		0xC0000000	/* watchdog period mask */
#define _PPC403_TCR_WRC		0x30000000	/* wdog reset control mask */
#define _PPC403_TCR_WIE		0x08000000	/* WDT interrupt enable mask */
#define _PPC403_TCR_PIE		0x04000000	/* PIT interrupt enable mask */
#define _PPC403_TCR_FP		0x03000000	/* FIT period mask */
#define _PPC403_TCR_FIE		0x00800000	/* FIT interrupt enable mask */
#define _PPC403_TCR_ARE		0x00400000	/* auto-reload enable mask */

/* typedefs */

typedef struct {
        UINT32   fitPeriod;		/* Fixed Interval Timer periods */
        UINT32   fpMask;		/* corresponding TCR mask */
}FIT_PERIOD;

typedef struct {
        UINT32   wdtPeriod;		/* Watchdog Timer periods */
        UINT32   fpMask;		/* corresponding TCR mask */
}WDT_PERIOD;

#ifdef __cplusplus
}
#endif

#endif	/* __INCppc403Timerh */
