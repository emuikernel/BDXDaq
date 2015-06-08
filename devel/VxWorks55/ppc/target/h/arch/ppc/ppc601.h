/* ppc601.h - PowerPC 601 specific header */

/* Copyright 1984-1996 Wind River Systems, Inc. */

/*
modification history
--------------------
01f,18may01,pch  added HID1 definition
01e,08oct96,tam  added MSR bit definition macros.
01d,09apr96,tpr  added register and macro PowerPC 601 specifics.
01c,26feb96,kkk  added PVR register.
01b,08feb95,caf  changed _CACHE_ALIGN_SIZE to 32.
01a,17mar94,yao  written.
*/

#ifndef __INCppc601h
#define __INCppc601h

#ifdef __cplusplus
extern "C" {
#endif

/* Machine State Register (MSR) masks PowerPC 601 specific */

#undef 	_PPC_MSR_BE		/* branch trace enable (not in 601) */
#undef 	_PPC_MSR_RI		/* recoverable interrupt (not in 601) */
#undef 	_PPC_MSR_POW_U		/* power managemnet not supported */

/* Special Porpous Register PowerPC 601 specific */

#undef	ASR					/* 64 bit implementation only */

#define	HID0			1008		/* hardware implemen. reg. 0 */
#define	HID1			1009		/* hardware implemen. reg. 1 */
#define MQ			0		/* */
#define	RTCUR			4		/* upper real time counter */
#define RTCLR			5		/* lower real time counter */
#define	DECR			6		/* decrement reg. (read only) */

#define	_CACHE_ALIGN_SIZE	32
#define _PPC_CACHE_UNIFIED	TRUE

/* HID0 - check sources and enables register definition */

#define PPC601_HID0_CE		0x80000000	/* master check enable */
#define PPC601_HID0_S		0x40000000	/* microcode check stop detected*/
#define PPC601_HID0_M		0x20000000	/* machine check detected */
#define PPC601_HID0_TD		0x10000000	/* mutiple TLB hit checkstop */
#define PPC601_HID0_CD		0x08000000	/* mutiple cache hit checkstop */
#define PPC601_HID0_SH		0x04000000	/* sequencer time out checkstop */
#define PPC601_HID0_DT		0x02000000	/* dispatch time out checkstop */
#define PPC601_HID0_BA		0x01000000	/* bus address parity error */
#define PPC601_HID0_BD		0x00800000	/* bus data parity error */
#define PPC601_HID0_CP		0x00400000	/* cache parity error */
#define PPC601_HID0_IU		0x00200000	/* invalid microcode instruction */
#define PPC601_HID0_PP		0x00100000	/* i/o control interface error */
#define PPC601_HID0_ES		0x00010000	/* enable microcode checkstop */
#define PPC601_HID0_EM		0x00008000	/* enable machine checkstop */
#define PPC601_HID0_ETD		0x00004000	/* enable TLB checkstop */
#define PPC601_HID0_ECD		0x00002000	/* enable cache checkstop */
#define PPC601_HID0_ESH		0x00001000	/* enable sequencer time out checkstop */
#define PPC601_HID0_EDT		0x00000800	/* enable dispatch time out checkstop */
#define PPC601_HID0_EBA		0x00000400	/* enable bus address parity checkstop */
#define PPC601_HID0_EBD		0x00000200	/* enable bus data parity checkstop */
#define PPC601_HID0_ECP		0x00000100	/* enable cache parity checkstop */
#define PPC601_HID0_EIU		0x00000080	/* enable invalid ucode checkstop */
#define PPC601_HID0_EPP		0x00000040	/* enable i/o protocol checkstop */
#define PPC601_HID0_DRF		0x00000020	/* optional reload of laternate sector on instruction fetch miss enabled */
#define PPC601_HID0_DRL		0x00000010	/* optional reload of laternate sector on load/store miss enabled */
#define PPC601_HID0_LM		0x00000008	/* little-endian mode */
#define PPC601_HID0_PAR		0x00000004	/* precharge enabled */
#define PPC601_HID0_EMC		0x00000002	/* no error detected in main cache during array initialization */
#define PPC601_HID0_EHP		0x00000001	/* HP_SNP_REQ disabled */

/* MSR bits definitions */

#define _PPC_MSR_BIT_FP         18      /* MSR Floating Ponit Aval. bit - FP */
#define _PPC_MSR_BIT_FE0        20      /* MSR FP exception mode 0 bit - FE0 */
#define _PPC_MSR_BIT_SE         21      /* MSR Single Step Trace bit - SE */
#define _PPC_MSR_BIT_FE1        23      /* MSR FP exception mode 1 bit - FE1 */
#define _PPC_MSR_BIT_IP         25      /* MSR Exception Prefix bit - EP */
#define _PPC_MSR_BIT_IR         26      /* MSR Inst Translation bit - IR */
#define _PPC_MSR_BIT_DR         27      /* MSR Data Translation bit - DR */

#ifdef __cplusplus
}
#endif

#endif /* __INCppc601h */
