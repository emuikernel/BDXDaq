/* cvmeSqsys1.h - Cyclone Squall Sys-1 module header */

/* Copyright 1984-2002 Wind River Systems, Inc. */

/*
modification history
--------------------
01b,26apr02,dat  Adding cplusplus protection, SPR 75017
01a,02mar93,ccc  created.
*/

/*
This file contains the configuration parameters for the Cyclone Squall Sys-1
module.
*/


#ifndef __INCcvmeSqsys1h
#define	__INCcvmeSqsys1h

/* includes */

#include "drv/scsi/wd33c93.h"

#ifdef __cplusplus
extern "C" {
#endif

/* defines */

#define	INCLUDE_EI	/* include 82596 driver */

#undef	IO_ADRS_EI
#define	IO_ADRS_EI	((char *) 0x000fff00)	/* 32A,32D i82596CA Ethernet */
#undef	INT_LVL_EI
#define	INT_LVL_EI	0x0
#undef	INT_VEC_EI
#define	INT_VEC_EI	CVME960_VEC_XINT4

#define	CVME960_EI_CA		((volatile UINT8 *) 0xc0000040)
#define	CVME960_EI_PORT		((volatile UINT8 *) 0xc0000030)
#define	CVME960_EI_UNUSED	((volatile UINT8 *) 0xc0000020)
#define	EI_SYSBUS		0x6c
#define	EI_POOL_ADRS		NONE

#undef	DEFAULT_BOOT_LINE
#define	DEFAULT_BOOT_LINE \
"ei(0,0)host:/usr/vw/config/cvme960/vxWorks h=90.0.0.3 e=90.0.0.50 u=target"

/*
 * WD33C93 SCSI registers - start at 0xc0000000, 4 byte offsets.  Chip
 * is driven with a 16 MHz clock, giving a clock period of 62.5 nsec
 */

#define	CVME960_SBIC_BASE_ADRS	((unsigned char *) 0xc0000000)
#define	CVME960_SBIC_REG_OFFSET	0x04
#define	CVME960_SBIC_CLK_PERIOD	62	/* nanosec */
#define	CVME960_SBIC_DEV_TYPE	SBIC_WD33C93A

#ifdef	INCLUDE_SCSI_MODULE	/* change FALSE to TRUE for SCSI interface */
#define	INCLUDE_SCSI		/* include wd33c93 SCSI interface */
#define	INCLUDE_SCSI_BOOT	/* include ability to boot from SCSI */
#define	INCLUDE_DOSFS		/* file system to be used */
#define	INT_VEC_SCSI	CVME960_VEC_XINT3
#define	INCLUDE_SCSI_DMA
#endif	/* INCLUDE_SCSI_MODULE */

/*
 * 0xc0000010 is the Squall module status register on a read,
 * and the SCSI bus reset control register on a write.
 */

#define	SQ_STATUS_REG		((volatile unsigned char *) 0xc0000010)
#define	SCSI_BUS_RESET_CTRL	SQ_STATUS_REG

/* bit definitions of status register (0xc0000010) - bits are active low */
#define	SCSI_BUS_RESET_BIT	2
#define	SCSI_IRQ_BIT		1
#define	SCC_IRQ_BIT		0

/* writes to SCSI_BUS_RESET_CTRL - assert/deassert RST* */
#define	ASSERT_SCSI_BUS_RESET	0x06
#define	DEASSERT_SCSI_BUS_RESET	0x02
#define	CLEAR_SCSI_RESET_IRQ	0x00

/* SCSI DMA channel */
#define	SCSI_DMA_CHANNEL	0

#ifdef __cplusplus
}
#endif

#endif	/* __INCcvmeSqsys1h */
