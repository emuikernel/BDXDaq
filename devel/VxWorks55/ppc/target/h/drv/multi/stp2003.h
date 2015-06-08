/* stp2003.h - PCIO: PCI Input Output Controller (Cheerio) */

/* Copyright 1984-1998 Wind River Systems, Inc. */

/*
modification history
--------------------
01a,01jan98,mem  written.
*/

/*
This file contains constants for the stp2003 PCI Input Output Controller
chip (also known as PCIO and Cheerio).
*/

#ifndef __INCstp2003h
#define __INCstp2003h

#ifdef __cplusplus
extern "C" {
#endif

/* PCI memory region sizes */
#define PCIO_ROM_SIZE		0x01000000	/* 16Meg */
#define PCIO_EBUS_SIZE		0x00800000	/* 8Meg */
#define PCIO_ENET_SIZE		0x00008000	/* 32k */

/* EBus offsets with typical uses */
#define PCIO_CS1_OFFSET		0x00000000	/* TOD/NVRAM */
#define PCIO_CS2_OFFSET		0x00100000	/* Ebus2 device */
#define PCIO_CS3_OFFSET		0x00200000	/* Audio */
#define PCIO_CS4_OFFSET		0x00300000	/* Super IO */
#define PCIO_CS5_OFFSET		0x00400000	/* sync. serial */
#define PCIO_CS6_OFFSET		0x00500000	/* SC/lab console/Freq. */
#define PCIO_CS7_OFFSET		0x00600000	/* Ebus2 device */
#define PCIO_DCTL1_OFFSET	0x00700000	/* Parallel */
#define PCIO_DCTL2_OFFSET	0x00702000	/* Audio (playback) */
#define PCIO_DCTL3_OFFSET	0x00704000	/* Audio (capture) */
#define PCIO_DCTL4_OFFSET	0x00706000	/* Floppy */
#define PCIO_TCR1_OFFSET	0x00710000	/* Int. Ctrl Ebus2 */
#define PCIO_TCR2_OFFSET	0x00710004	/* Int. Ctrl Ebus2 */
#define PCIO_TCR3_OFFSET	0x00710008	/* Int. Ctrl Ebus2 */
#define PCIO_FLP_AUX_OFFSET	0x00720000	/* Int. Reg Ebus2 */
#define PCIO_AUD_AUX_OFFSET	0x00722000	/* Int. Reg Ebus2 */
#define PCIO_PWR_AUX_OFFSET	0x00724000	/* Int. Reg Ebus2 */
#define PCIO_LED_AUX_OFFSET	0x00726000	/* Int. Reg Ebus2 */
#define PCIO_PMD_AUX_OFFSET	0x00728000	/* Int. Reg Ebus2 */
#define PCIO_FREQ_AUX_OFFSET	0x0072a000	/* Int. Reg Ebus2 */
#define PCIO_OSC_AUX_OFFSET	0x0072c000	/* Int. Reg Ebus2 */
#define PCIO_TEMP_AUX_OFFSET	0x0072f000	/* Int. Reg Ebus2 */
#define PCIO_BNK0_OFFSET	0x00730000	/* Bank 0 (64 words) */
#define PCIO_BNK1_OFFSET	0x00730100	/* Bank 1 (64 words) */
#define PCIO_DCSR1_OFFSET	0x00700000	/* DMA1 CSR Reg */
#define PCIO_DACR1_OFFSET	0x00700004	/* DMA1 addr count Reg */
#define PCIO_DBCR1_OFFSET	0x00700008	/* DMA1 byte count Reg */
#define PCIO_DCSR2_OFFSET	0x00702000	/* DMA2 CSR Reg */
#define PCIO_DACR2_OFFSET	0x00702004	/* DMA2 addr count Reg */
#define PCIO_DBCR2_OFFSET	0x00702008	/* DMA2 byte count Reg */
#define PCIO_DCSR3_OFFSET	0x00704000	/* DMA3 CSR Reg */
#define PCIO_DACR3_OFFSET	0x00704004	/* DMA3 addr count Reg */
#define PCIO_DBCR3_OFFSET	0x00704008	/* DMA3 byte count Reg */
#define PCIO_DCSR4_OFFSET	0x00706000	/* DMA4 CSR Reg */
#define PCIO_DACR4_OFFSET	0x00706004	/* DMA4 addr count Reg */
#define PCIO_DBCR4_OFFSET	0x00706008	/* DMA4 byte count Reg */

/* Bit definitions for a subset of the PCIO registers */

#define PCIO_CSR_INT_PEND	(1 << 0)
#define PCIO_CSR_ERR_PEND	(1 << 1)
#define PCIO_CSR_DRAIN		(1 << 2)
#define PCIO_CSR_INT_EN		(1 << 4)
#define PCIO_CSR_RESET		(1 << 7)
#define PCIO_CSR_WRITE		(1 << 8)  /* from device to memory */
#define PCIO_CSR_READ		(0 << 8)  /* from memory to device */
#define PCIO_CSR_EN_DMA		(1 << 9)
#define PCIO_CSR_CYC_PENDING	(1 << 10)
#define PCIO_CSR_DIAG_RD_DONE	(1 << 11)
#define PCIO_CSR_DIAG_WR_DONE	(1 << 12)
#define PCIO_CSR_EN_CNT		(1 << 13)
#define PCIO_CSR_TC		(1 << 14)
#define PCIO_CSR_DIS_CSR_DRN	(1 << 16)
#define PCIO_CSR_BURST_SIZE_4	(2 << 18) /* burst size in bytes */
#define PCIO_CSR_BURST_SIZE_16	(0 << 18)
#define PCIO_CSR_BURST_SIZE_32	(1 << 18)
#define PCIO_CSR_BURST_SIZE_64	(3 << 18)
#define PCIO_CSR_DIAG_EN	(1 << 20)
#define PCIO_CSR_DIS_ERR_PEND	(1 << 22)
#define PCIO_CSR_TCI_DIS	(1 << 23)
#define PCIO_CSR_EN_NEXT	(1 << 24)
#define PCIO_CSR_DMA_ON		(1 << 25)
#define PCIO_CSR_A_LOADED	(1 << 26)
#define PCIO_CSR_NA_LOADED	(1 << 27)
#define PCIO_CSR_DEV_ID		(1 << 28)

#ifdef __cplusplus
}
#endif

#endif /* __INCstp2003h */
