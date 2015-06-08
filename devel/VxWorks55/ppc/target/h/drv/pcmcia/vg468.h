/* vg468.h - Vadem VG-468 PCMCIA Host Bus Adaptor chip header */

/* Copyright 1984-1996 Wind River Systems, Inc. */
/* Copyright (c) 1994 David A. Hinds -- All Rights Reserved */

/*
modification history
--------------------
01b,22feb96,hdn  cleaned up.
01a,04dec95,hdn  written based on David Hinds's version 2.6.3.
*/

#ifndef __INCvg468h
#define __INCvg468h

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _ASMLANGUAGE


/* Special bit in I365_IDENT used for Vadem chip detection */

#define PCIC_IDENT_VADEM	0x08

/* Special definitions in I365_POWER */

#define VG468_VPP2_MASK		0x0c
#define VG468_VPP2_5V		0x04
#define VG468_VPP2_12V		0x08

/* Unique Vadem registers */

#define VG468_CTL		0x38	/* Control register */
#define VG468_TIMER		0x39	/* Timer control */
#define VG468_MISC		0x3a	/* Miscellaneous */
#define VG468_GPIO_CFG		0x3b	/* GPIO configuration */
#define VG468_SELECT		0x3d	/* Programmable chip select */
#define VG468_SELECT_CFG	0x3e	/* Chip select configuration */
#define VG468_ATA		0x3f	/* ATA control */

/* Flags for VG468_CTL */

#define VG468_CTL_SLOW		0x01	/* 600ns memory timing */
#define VG468_CTL_ASYNC		0x02	/* Asynchronous bus clocking */
#define VG468_CTL_TSSI		0x08	/* Tri-state some outputs */
#define VG468_CTL_DELAY		0x10	/* Card detect debounce */
#define VG468_CTL_INPACK	0x20	/* Obey INPACK signal? */
#define VG468_CTL_POLARITY	0x40	/* VCCEN polarity */
#define VG468_CTL_COMPAT	0x80	/* Compatibility stuff */

/* Flags for VG468_TIMER */

#define VG468_TIMER_ZEROPWR	0x10	/* Zero power control */
#define VG468_TIMER_SIGEN	0x20	/* Power up */
#define VG468_TIMER_STATUS	0x40	/* Activity timer status */
#define VG468_TIMER_RES		0x80	/* Timer resolution */
#define VG468_TIMER_MASK	0x0f	/* Activity timer timeout */

/* Flags for VT468_MISC */

#define VG468_MISC_UNLOCK	0x80	/* Unique register lock */
#define VG468_MISC_VADEMREV	0x40	/* Vadem revision control */
#define VG468_MISC_DMAWSB	0x08	/* DMA wait state control */
#define VG468_MISC_GPIO		0x04	/* General-purpose IO */


#endif	/* _ASMLANGUAGE */

#ifdef __cplusplus
}
#endif

#endif	/* __INCvg468h */
