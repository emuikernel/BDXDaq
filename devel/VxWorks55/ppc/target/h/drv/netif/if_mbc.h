/* if_mbc.h - Motorola Module Bus Ethernet interface header */

/* Copyright 2002 Wind River Systems, Inc. */

/*
modification history
--------------------
01b,19apr02,rcs  added obsolescence warning SPR# 76010
01a,09may96,dzb  written.
*/

/*
This file contains I/O addresses and related constants for the Motorola
Module Bus Ethernet controller used in the M68EN360 communications
controller.
*/

#ifndef	__INCif_mbch
#define	__INCif_mbch

#ifdef	__cplusplus
extern "C" {
#endif

#warning "if_mbc driver is obsolete, please use mbcEnd driver"

/* defines */

/* MBC - Module Bus Ethernet controller */

#define MBC_IFNAME		"mbc"	/* device interface name */
 
/* Module Bus Ethernet Controller registers */

#define MBC_ECNTL		0x000	/* ethernet control register */
#define MBC_EDMA		0x002	/* DMA configuration register */
#define MBC_EMRBLR		0x004	/* max receive buffer length */
#define MBC_IVEC		0x006	/* interrupt vector register */
#define MBC_IEVNT		0x008	/* interrupt event */
#define MBC_IMASK		0x00a	/* interrupt mask register */
#define MBC_ECFG		0x00c	/* configurartion register */
#define MBC_TEST		0x00e	/* test register */
#define MBC_AR			0x010	/* address recognition control */
#define MBC_ARTAB		0x200	/* address recognition table */
#define MBC_EBD			0x400	/* buffer descriptors table */

/* Ethernet Control register definitions */

#define MBC_ECNTL_RES		0x0000	/* ethernet reset */
#define MBC_ECNTL_RES_OFF	0x0001	/* ethernet reset off */
#define MBC_ECNTL_ENBL		0x0002	/* ethernet enable */
#define MBC_ECNTL_GTS		0x0004	/* graceful tx stop */

/* Ethernet DMA config/status register definitions */
 
#define MBC_EDMA_BLIM_MSK	0x0007	/* burst limit mask */
#define MBC_EDMA_BLIM_1		0x0000	/* burst limit, 1 */
#define MBC_EDMA_BLIM_2		0x0001	/* burst limit, 2 */
#define MBC_EDMA_BLIM_4		0x0002	/* burst limit, 4 */
#define MBC_EDMA_BLIM_8		0x0003	/* burst limit, 8 */
#define MBC_EDMA_BLIM_16	0x0004	/* burst limit, 16 */
#define MBC_EDMA_BLIM_32	0x0005	/* burst limit, 32 */
#define MBC_EDMA_BLIM_64	0x0006	/* burst limit, 64 */
#define MBC_EDMA_BLIM_UNL	0x0007	/* burst limit, unlimited */
#define MBC_EDMA_WMRK_MSK	0x0018	/* fifo watermark mask */
#define MBC_EDMA_WMRK_8		0x0000	/* fifo watermark 8 bytes */
#define MBC_EDMA_WMRK_16	0x0008	/* fifo watermark 16 bytes */
#define MBC_EDMA_WMRK_24	0x0010	/* fifo watermark 24 bytes */
#define MBC_EDMA_WMRK_32	0x0018	/* fifo watermark 32 bytes */
#define MBC_EDMA_TSRLY		0x0020	/* transmit start early */
#define MBC_EDMA_BDS_MSK	0x00c0	/* bdsize mask */
#define MBC_EDMA_BDS_SHFT	0x0006	/* shift cnt for bd size */
#define MBC_EDMA_BDS_8		0x0000	/* bdsize 8tx/120rx */
#define MBC_EDMA_BDS_16		0x0001	/* bdsize 16tx/112rx */
#define MBC_EDMA_BDS_32		0x0002	/* bdsize 32tx/96rx */
#define MBC_EDMA_BDS_64		0x0003	/* bdsize 64tx/64rx */
#define MBC_EDMA_BDE_MSK	0xfe00	/* bd error # */
#define MBC_EDMA_BDE_SHFT	0x0009	/* shift cnt for bd error # */
#define MBC_BD_MAX		128	/* total number of buffer descriptors */

/* Interrupt Vector register definitions */
 
#define MBC_IVEC_MSK		0x00ff	/* interrupt vector mask */
#define MBC_IVEC_RX		0x0000	/* receive interrupt */
#define MBC_IVEC_TX		0x0001	/* transmit interrupt */
#define MBC_IVEC_NTC		0x0002	/* non time critical interrupt */
#define MBC_IVEC_TC		0x0003	/* time critical interrupt */
#define MBC_IVEC_VG		0x0100	/* vector granularity */

/* Interrupt Event register definitions */

#define MBC_IEVNT_RXB		0x0001	/* receive buffer interrupt */
#define MBC_IEVNT_TXB		0x0002	/* transmit buffer interrupt */
#define MBC_IEVNT_BSY		0x0004	/* busy interrupt */
#define MBC_IEVNT_RXF		0x0008	/* receive frame interrupt */
#define MBC_IEVNT_TXF		0x0010	/* transmit frame interrupt */
#define MBC_IEVNT_EBE		0x0020	/* ethernet bus error */
#define MBC_IEVNT_BOD		0x0040	/* backoff done */
#define MBC_IEVNT_GRA		0x0080	/* graceful stop complete */
#define MBC_IEVNT_BTE		0x0100	/* babbling transmitter error */
#define MBC_IEVNT_BRE		0x0200	/* babbling receiver error */
#define MBC_IEVNT_HBE		0x0400	/* heartbeat error */

/* Interrupt Mask register */

#define MBC_IMASK_RXB		0x0001	/* receive buffer interrupt */
#define MBC_IMASK_TXB		0x0002	/* transmit buffer interrupt */
#define MBC_IMASK_BSY		0x0004	/* busy interrupt */
#define MBC_IMASK_RXF		0x0008	/* receive frame interrupt */
#define MBC_IMASK_TXF		0x0010	/* transmit frame interrupt */
#define MBC_IMASK_EBE		0x0020	/* ethernet bus error */
#define MBC_IMASK_BOD		0x0040	/* backoff done */
#define MBC_IMASK_GRA		0x0080	/* graceful stop complete */
#define MBC_IMASK_BTE		0x0100	/* babbling transmitter error */
#define MBC_IMASK_BRE		0x0200	/* babbling receiver error */
#define MBC_IMASK_HBE		0x0400	/* heartbeat error */

/* Ethernet Configuration register definitions */
 
#define MBC_ECFG_LOOP		0x0001	/* internal loopback */
#define MBC_ECFG_FDEN		0x0002	/* full duplex enable */
#define MBC_ECFG_HBC		0x0004	/* heartbeat control */

/* Address Recognition register definitions */
/* written only when ethernet controller is disabled */
 
#define MBC_AR_PROM		0x0400	/* promiscuous */
#define MBC_AR_PAREJ		0x0800	/* phys address reject  */
#define MBC_AR_INDEX		0x4000	/* index enable */
#define MBC_AR_MULTI_00		0x0000	/* multicast and broadcast */
#define MBC_AR_MULTI_01		0x1000	/* multicast, but no broadcast */
#define MBC_AR_MULTI_10		0x2000	/* all multicast and broadcast */
#define MBC_AR_MULTI_11		0x3000	/* no multicast or broadcast */
#define MBC_AR_HASH		0x8000	/* hash mode enable */

#define MBC_ARTAB_SIZE		64	/* number of entries in the AR table */


/* SCC Ethernet Protocol Specific Mode Register definitions */

/* Receive/Transmit Buffer Descriptor */
#ifndef _ASMLANGUAGE

typedef struct	  /* MBC_BD */
    {
    UINT16	statusMode;		/* status and control */
    UINT16	dataLength;		/* length of data buffer in bytes */
    UCHAR *	dataPointer;		/* points to data buffer */
    } MBC_BD;

#endif /* _ASMLANGUAGE*/


/* Receive Buffer Descriptor statusMode field definitions */
 
#define MBC_RXBD_CL		0x0001	/* collision condition */
#define MBC_RXBD_OV		0x0002	/* overrun condition */
#define MBC_RXBD_CR		0x0004	/* Rx CRC error */
#define MBC_RXBD_SH		0x0008	/* short frame received */
#define MBC_RXBD_NO		0x0010	/* Rx nonoctet aligned frame */
#define MBC_RXBD_LG		0x0020	/* Rx frame length violation */
#define MBC_RXBD_M		0x0100	/* miss bit for prom mode */
#define MBC_RXBD_F		0x0400	/* buffer is first in frame */
#define MBC_RXBD_L		0x0800	/* buffer is last in frame */
#define MBC_RXBD_I		0x1000	/* interrupt on receive */
#define MBC_RXBD_W		0x2000	/* last BD in ring */
#define MBC_RXBD_E		0x8000	/* buffer is empty */
 
/* Transmit Buffer Descriptor statusMode field definitions */
 
#define MBC_TXBD_CSL		0x0001	/* carrier sense lost */
#define MBC_TXBD_UN		0x0002	/* underrun */
#define MBC_TXBD_RC		0x003c	/* retry count */
#define MBC_TXBD_RC_SHFT	0x0002	/* shift cnt for RC */
#define MBC_TXBD_RL		0x0040	/* retransmission limit */
#define MBC_TXBD_LC		0x0080	/* late collision */
#define MBC_TXBD_HB		0x0100	/* heartbeat */
#define MBC_TXBD_DEF		0x0200	/* defer indication */
#define MBC_TXBD_TC		0x0400	/* auto transmit CRC */
#define MBC_TXBD_L		0x0800	/* buffer is last in frame */
#define MBC_TXBD_I		0x1000	/* interrupt on transmit */
#define MBC_TXBD_W		0x2000	/* last BD in ring */
#define MBC_TXBD_PAD		0x4000	/* auto pad short frames */
#define MBC_TXBD_R		0x8000	/* buffer is ready */
#define MBC_TXBD_ERRS		(MBC_TXBD_DEF | MBC_TXBD_HB | \
                                 MBC_TXBD_LC  | MBC_TXBD_RL | \
                                 MBC_TXBD_UN  | MBC_TXBD_CSL)


/* typedefs */
 
#ifndef _ASMLANGUAGE

typedef struct	  /* MBC_DEV */
    {
    int			mbcNum;		/* number of MBC device */
    UINT8		bdSize;		/* buffer descriptor size */
    UINT8		txBdNum;	/* number of tx buf descriptors */
    UINT8		rxBdNum;	/* number of rx buf descriptors */
    MBC_BD *		txBdBase;	/* transmit BD array address */
    MBC_BD *		rxBdBase;	/* receive BD array address */
    u_char *		txBufBase;	/* transmit buffer pool base address */
    u_char *		rxBufBase;	/* receive buffer pool base address */
    UINT8		txBdNext;	/* next transmit BD to fill */
    UINT8		rxBdNext;	/* next receive BD to read */
    volatile char *	pDevBase;	/* ethenet module base address */
    } MBC_DEV;

#endif /* _ASMLANGUAGE */

#ifdef	__cplusplus
}
#endif

#endif	/* __INCif_mbch */
