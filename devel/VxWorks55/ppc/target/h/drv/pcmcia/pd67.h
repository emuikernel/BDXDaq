/* pd67.h - Cirrus CL-PD67xx PCMCIA Host Bus Adaptor chip header */

/* Copyright 1984-1996 Wind River Systems, Inc. */
/* Copyright (c) 1994 David A. Hinds -- All Rights Reserved */

/*
modification history
--------------------
01b,22feb96,hdn  cleaned up.
01a,12oct95,hdn  written based on David Hinds's version 2.2.3.
*/

#ifndef __INCpd67h
#define __INCpd67h

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _ASMLANGUAGE


#define PD67_MISC_CTL_1		0x16	/* Misc control 1 */
#define PD67_FIFO_CTL		0x17	/* FIFO control */
#define PD67_MISC_CTL_2		0x1E	/* Misc control 2 */
#define PD67_CHIP_INFO		0x1f	/* Chip information */

/* I/O window address offset */

#define PD67_IO_OFF(w)		(0x36+((w)<<1))

/* Timing register sets */

#define PD67_TIME_SETUP(n)	(0x3a + 3*(n))
#define PD67_TIME_CMD(n)	(0x3b + 3*(n))
#define PD67_TIME_RECOV(n)	(0x3c + 3*(n))

/* Flags for PD67_MISC_CTL_1 */

#define PD67_MC1_5V_DET		0x01	/* 5v detect */
#define PD67_MC1_VCC_3V		0x02	/* 3.3v Vcc */
#define PD67_MC1_PULSE_MGMT	0x04
#define PD67_MC1_PULSE_IRQ	0x08
#define PD67_MC1_SPKR_ENA	0x10
#define PD67_MC1_INPACK_ENA	0x80

/* Flags for PD67_FIFO_CTL */

#define PD67_FIFO_EMPTY		0x80

/* Flags for PD67_MISC_CTL_2 */

#define PD67_MC2_FREQ_BYPASS	0x01
#define PD67_MC2_DYNAMIC_MODE	0x02
#define PD67_MC2_SUSPEND	0x04
#define PD67_MC2_5V_CORE	0x08
#define PD67_MC2_LED_ENA	0x10	/* IRQ 12 is LED enable */
#define PD67_MC2_3STATE_BIT7	0x20	/* Floppy change bit */
#define PD67_MC2_DMA_MODE	0x40
#define PD67_MC2_IRQ15_RI	0x80	/* IRQ 15 is ring enable */

/* Flags for PD67_CHIP_INFO */

#define PD67_INFO_SLOTS		0x20	/* 0 = 1 slot, 1 = 2 slots */
#define PD67_INFO_CHIP_ID	0xc0
#define PD67_INFO_REV		0x1c

/* Fields in PD67_TIME_* registers */

#define PD67_TIME_SCALE		0xc0
#define PD67_TIME_SCALE_1	0x00
#define PD67_TIME_SCALE_16	0x40
#define PD67_TIME_SCALE_256	0x80
#define PD67_TIME_SCALE_4096	0xc0
#define PD67_TIME_MULT		0x3f


#endif	/* _ASMLANGUAGE */

#ifdef __cplusplus
}
#endif

#endif	/* __INCpd67h */
