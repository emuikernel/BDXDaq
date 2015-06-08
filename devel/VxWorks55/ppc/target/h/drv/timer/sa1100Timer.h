/* sa1100Timer.h - Digital Semiconductor SA-1100 timer header file */

/* Copyright 1997-1998 Wind River Systems, Inc. */
 
/*
modification history
--------------------
01d,15dec97,cdp  created from 01d of cma220.
*/
 
#ifndef __INCsa1100Timerh
#define __INCsa1100Timerh
 
#ifdef __cplusplus
extern "C" {
#endif
 
/* register definitions */
 
#define SA1100_TIMER_CSR_OSCR	(SA1100_TIMER_BASE + 0x10) /* OSCR    (R/W) */
#define SA1100_TIMER_CSR_OSMR_0	(SA1100_TIMER_BASE + 0x00) /* OSMR[0] (R/W) */
#define SA1100_TIMER_CSR_OSMR_1	(SA1100_TIMER_BASE + 0x04) /* OSMR[1] (R/W) */
#define SA1100_TIMER_CSR_OSMR_2	(SA1100_TIMER_BASE + 0x08) /* OSMR[2] (R/W) */
#define SA1100_TIMER_CSR_OSSR	(SA1100_TIMER_BASE + 0x14) /* OSSR    (R/W) */
#define SA1100_TIMER_CSR_OIER	(SA1100_TIMER_BASE + 0x1C) /* OIER    (R/W) */
 
#ifdef __cplusplus
}
#endif
 
#endif  /* __INCsa1100Timerh */
