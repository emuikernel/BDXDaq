/*
 *--------------------------------------------------------------------------
 *                       COPYRIGHT NOTICE
 *
 *       Copyright (C) 1997 VME Microsystems International Corporation
 *	 International copyright secured.  All rights reserved.
 *--------------------------------------------------------------------------
 *	SCCS/s.dev6016.h 1.2 11/13/97 10:46:11
 *--------------------------------------------------------------------------
 *	Device Register Declarations
 *--------------------------------------------------------------------------
 */

#ifndef	DEV6016_H
#define DEV6016_H

#ifndef	lint
static char dev6016_h_sccs_id[] = "@(#)dev6016.h 1.2 98/07/24 VMIC";
#endif	/* lint */

#define CHANNELS_PER_CARD	16	/* Maximum channels per card	 */
#define MINIMUM_CHANNEL		0	/* The first channel		 */

#define CHBASE			0x20	/* Channel Register Base Address */

#define CCB_SZ			0x0E	/* Channel Control Blocks Size	 */

/* CTR0 Masks								 */

#define MASTER_MASK		0x80	/* Master/Slave Bit Mask	 */
#define SLV_OK_MASK		0x40	/* Slave OK/Not OK Bit Mask	 */
#define SLV_32_MASK		0x20	/* Slave A32 or A24 Bit Mask	 */
#define SUPONLY_MASK	0x10	/* Slave Supervisor Only? Mask	 */
#define TXBRK_MASK		0x08	/* Transmit Break Bit Mask	 */
#define SLFTST_MASK		0x04	/* Perform Selftest Bit Mask	 */
#define RST_BRD_MASK	0x02	/* Reset Board Bit Mask		 */
#define LEDN_MASK		0x01	/* LED On/Off Bit Mask		 */

/* ROM version register masks						 */

#define INCONSIST_OPT	0x8000	/* Inconsistent Option Bit Mask	 */
#define OLD_VIC_OPT		0x2000	/* VMIC silicon ID old Bit Mask	 */
#define MBLT_OPT		0x1000	/* MBLT possible? Bit Mask	 */
#define SLOW_CPU_OPT	0x0800	/* Slow CPU Bit Mask		 */
#define CHAN_OPT		0x0400	/* Eight or Sixteen Channels	 */
#define MEM_OPT 0x0300			/* User buffer size bits mask	 */

/* GST Register masks							 */

#define ANY_ERR			0x8000	/* GLBILL,RFNZ,STFAIL,HDWEERR,BERR */
#define GLBILL			0x4000	/* Global Register Illegal Field */
#define ST_BUSY			0x2000	/* Selftest Busy		 */
#define SCON			0x1000	/* VME Bus System Controller	 */
#define RFNZ			0x0800	/* Reserved field non-zero	 */
#define STFAIL			0x0400	/* Selftest has failed		 */
#define HDWEERR			0x0200	/* Hardware Error		 */
#define BERR			0x0100	/* VME Bus Error		 */
#define REGBUSY			0x0080	/* Reg Busy (CR0,CR1,GO,TX,RX)	 */

/* CST Register Masks							 */

#define DCD_COS			0x8000	/* Data-Carrier-Detect changed	 */
#define CTS_COS			0x4000	/* Clear-To-Send changed	 */
#define DCD             0x2000  /* Data-Carrier-Detect state	 */
#define CTS             0x1000  /* Clear-To-Send state		 */
#define INTBSY			0x0800	/* Host Interrupt routine busy	 */
#define ILL             0x0400	/* Illegal CCB reg. field value	 */
#define URCV_RDY		0x0200	/* User receive buffer has data	 */
#define UTX_RDY			0x0100	/* User transmit can take data	 */
#define BRK_RCVD		0x0080	/* Serial channel break rec'd	 */
#define FERR			0x0040	/* Serial channel framing error	 */
#define PERR			0x0020	/* Serial channel parity error	 */
#define OVERRUN			0x0010	/* Serial receiver overrun	 */
#define TXEMT			0x0008	/* Serial transmitter empty	 */
#define TXRDY			0x0004	/* Transmitter can take data	 */
#define FFUL			0x0002	/* Serial receiver FIFO full	 */
#define RXRDY			0x0001	/* Serial receive has data	 */

/* Masks for the SZ_AM Register						 */

#define ADDRMOD			0x3f	/* VME Bus Address Modifier Mask */
#define VMESIZE			0xc0	/* VME Bus Data Size Mask	 */

/* Mask for the channel interrupt level					 */

#define CH_LEVEL		0x07

/* CTR1 Masks								 */

#define ARBMOD			0x08	/* VME Bus Arbitration Mode Mask */
#define BERR_TO			0x07	/* VME Bus Error Timeout *	 */
#define BERR_4US		0x00	/* Bus Error Timeout 4 usec	 */
#define BERR_16US		0x01	/* Bus Error Timeout 16 usec	 */
#define BERR_32US		0x02	/* Bus Error Timeout 32 usec	 */
#define BERR_64US		0x03	/* Bus Error Timeout 64 usec	 */
#define BERR_128US		0x04	/* Bus Error Timeout 128 usec	 */
#define BERR_256US		0x05	/* Bus Error Timeout 256 usec	 */
#define BERR_512US		0x06	/* Bus Error Timeout 512 usec	 */
#define BERR_INF		0x07	/* Bus Error Timeout infinite	 */

/* CTR2 Masks								 */

#define BRLEVEL			0xc0	/* VME Bus Request Level Mask	 */
#define RELMOD			0x30	/* VME Bus Release Mode Mask	 */
#define FAIR_TO			0x0f	/* VME Bus Fairness Timeout Mask */

/* SZ_RING Register values in number of blocks				 */

#define SZ_RING_256K	8	/* Ring Buffer 256K Bytes	 */
#define SZ_RING_512K	40	/* Ring Buffer 512K Bytes	 */
#define SZ_RING_1M		104	/* Ring Buffer 1M Bytes		 */
#define SZ_RING_2M		232	/* Ring Buffer 2M Bytes		 */

/* CH_CON1 masks							 */

#define UB_RING			0x80	/* Ring/Linear Buffer Mask	 */
#define FLOWCON			0x60	/* Serial Flow Control Mask	 */
#define BAUD			0x1f	/* Serial Baud Rate Mask	 */

/* CH_CON2 masks							 */

#define INP_TIMEOUT		0xc0	/* Serial InputTimeout Mask	 */
#define TWO_STOP		0x20	/* One/Two Stop Bits Mask	 */
#define PAR_TYP			0x18	/* Parity Type Mask		 */
#define PAR_ODD			0x04	/* Parity Sense Odd/Even Mask	 */
#define SZ_CHAR			0x03	/* Num Data Bits Mask		 */

/* SZ_UBUF values							 */

#define SZ_UBUF_4K		32	/* 4K Bytes User Buffer Size	 */
#define SZ_UBUF_8K		64	/* 8K Bytes User Buffer Size	 */
#define SZ_UBUF_16K		128	/* 16K Bytes User Buffer Size	 */
#define SZ_UBUF_32K		0	/* 32K Bytes User Buffer Size	 */

/* Buffer Modes								 */

#define MASTER_MODE		0x80	/* Master Buffer Mode		 */
#define SLAVE_MODE		0x00	/* Slave Buffer Mode		 */

/* Slave Address Modes							 */

#define SLV_32			0x20	/* A32 Slave			 */
#define SLV_NOT_32		0x00	/* A24 Slave			 */

/* Slave Supervisor Modes						 */

#define SUPONLY			0x10	/* Supervisor only slave	 */
#define NOTSUPONLY		0x00	/* Not supervisor only slave	 */

/* Master Data Sizes							 */

#define VME_SIZE_D32	0	/* VME Bus Data Size 32 Bits	 */
#define VME_SIZE_D8		1	/* VME Bus Data Size 8 Bits	 */
#define VME_SIZE_D16	2	/* VME Bus Data Size 16 Bits	 */

/* Master VME Address Modifier values					 */

#define AM_A32_NOP_DATA		0x09	/* A32 non-privileged data	 */
#define AM_A32_NOP_PROG		0x0A	/* A32 non-privileged program	 */
#define AM_A32_NOP_BLK		0x0B	/* A32 non-privileged block	 */
#define AM_A32_SUP_DATA		0x0D	/* A32 supervisor data		 */
#define AM_A32_SUP_PROG		0x0E	/* A32 supervisor program	 */
#define AM_A32_SUP_BLK		0x0F	/* A32 supervisor block		 */
#define AM_A16_NOP_DATA		0x29	/* A16 non-privileged data	 */
#define AM_A16_SUP_DATA		0x2D	/* A16 supervisor data		 */
#define AM_A24_NOP_DATA		0x39	/* A24 non-privileged data	 */
#define AM_A24_NOP_PROG		0x3A	/* A24 non-privileged program	 */
#define AM_A24_NOP_BLK		0x3B	/* A24 non-privileged block	 */
#define AM_A24_SUP_DATA		0x3D	/* A24 supervisor data		 */
#define AM_A24_SUP_PROG		0x3E	/* A24 supervisor program	 */
#define AM_A24_SUP_BLK		0x3F	/* A24 supervisor block		 */

/* Buffer Types */

#define RING_BUFFER		    0x80	/* Serial Ring Buffer */
#define LINEAR_BUFFER		0x00	/* Linear Ring Buffer */

#endif	/* DEV6016_H */
