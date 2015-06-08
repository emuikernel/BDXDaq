/* ne2000End.h - Novell/Eagle 2000 network interface header */

/* Copyright 1984-2000 Wind River Systems, Inc. */

/*
modification history
--------------------
01c,11jun00,ham  removed reference to etherLib.
01b,28may00,dat  added more macros, gen clean up
01a,16mar98,mem  written, based on drv/netif/if_ene.h
*/


#ifndef __INCne2000Endh
#define __INCne2000Endh

#ifdef __cplusplus
extern "C" {
#endif

#include "netinet/if_ether.h"

#define EADDR_LEN		6
#define NE2000_BUFSIZ		(ETHERMTU + SIZEOF_ETHERHEADER + EADDR_LEN)

/* NE 2000 on-board memory page addresses */

#define NE2000_CONFIG_PAGE      0x00    /* where the Ethernet address is */
#define NE2000_EADDR_LOC        0x00    /* location within config. page */
#define NE2000_TSTART           0x40
#define NE2000_PSTART           0x46
#define NE2000_PSTOP            0x80

#define ENE_PAGESIZE            256

typedef struct
    {
    UCHAR rstat;
    UCHAR next;
    UCHAR lowByteCnt;
    UCHAR uppByteCnt;
    } NE2000_HEADER;

/* NS 8390 (Ethernet LAN Controller) */

/* page-0, read */

#define ENE_CMD		0x00	/* command */
#define ENE_TRINCRL	0x01	/* TRDMA incrementer */
#define ENE_TRINCRH	0x02	/* TRDMA incrementer */
#define ENE_BOUND	0x03	/* boundary page */
#define ENE_TSTAT	0x04	/* transmit status */
#define ENE_COLCNT	0x05	/* collision error */
#define ENE_INTSTAT	0x07	/* interrupt status */
#define ENE_RSTAT	0x0c	/* receive status */
#define ENE_ALICNT	0x0d	/* alignment error */
#define ENE_CRCCNT	0x0e	/* crc error */
#define ENE_MPCNT	0x0f	/* missed packet */

/* page-0, write */

#define ENE_RSTART	0x01	/* receive start */
#define ENE_RSTOP	0x02	/* receive stop */
#define ENE_TSTART	0x04	/* transmit start */
#define ENE_TCNTL	0x05	/* transmit counter */
#define ENE_TCNTH	0x06	/* transmit counter */
#define ENE_RSAR0	0x08	/* remote address */
#define ENE_RSAR1	0x09	/* remote address */
#define ENE_RBCR0	0x0a	/* remote byte count */
#define ENE_RBCR1	0x0b	/* remote byte count */
#define ENE_RCON	0x0c	/* receive config */
#define ENE_TCON	0x0d	/* transmit config */
#define ENE_DCON	0x0e	/* data config */
#define ENE_INTMASK	0x0f	/* interrupt mask */

/* page-1, read and write */

#define ENE_STA0	0x01	/* station address */
#define ENE_STA1	0x02	/* station address */
#define ENE_STA2	0x03	/* station address */
#define ENE_STA3	0x04	/* station address */
#define ENE_STA4	0x05	/* station address */
#define ENE_STA5	0x06	/* station address */
#define ENE_CURR	0x07	/* current page */
#define ENE_MAR0	0x08	/* multicast address */
#define ENE_MAR1	0x09	/* multicast address */
#define ENE_MAR2	0x0a	/* multicast address */
#define ENE_MAR3	0x0b	/* multicast address */
#define ENE_MAR4	0x0c	/* multicast address */
#define ENE_MAR5	0x0d	/* multicast address */
#define ENE_MAR6	0x0e	/* multicast address */
#define ENE_MAR7	0x0f	/* multicast address */

/* page-2, read and write */

#define ENE_NEXT	0x05	/* next page */
#define ENE_BLOCK	0x06	/* block address */
#define ENE_ENH		0x07	/* enable features */

/* NE2000 board registers */

#define ENE_DATA	0x10	/* data window */
#define ENE_RESET	0x1f	/* read here to reset */


/* COMMAND: Command Register */
#define CMD_PAGE2		0x80
#define CMD_PAGE1		0x40
#define CMD_PAGE0		0x00
#define CMD_NODMA		0x20
#define CMD_RWRITE		0x10
#define CMD_RREAD		0x08
#define CMD_TXP			0x04
#define CMD_START		0x02
#define CMD_STOP		0x01

/* RCON: Receive Configuration Register */
#define RCON_MON		0x20
#define RCON_PROM		0x10
#define RCON_GROUP		0x08
#define RCON_BROAD		0x04
#define RCON_RUNTS		0x02
#define RCON_SEP		0x01

/* TCON: Transmit Configuration Register */
#define TCON_LB1		0x04
#define TCON_LB0		0x02
#define TCON_CRCN		0x01
#define TCON_NORMAL		0x00 /* normal operation mode */

/* DCON: Data Configuration Register */
#define DCON_BSIZE1		0x40
#define DCON_BSIZE0		0x20
#define DCON_AUTO_INIT		0x10
#define DCON_LOOPBK_OFF		0x08
#define DCON_LOOPBK_ON		0x00
#define DCON_DMA_32		0x04
#define DCON_DMA_16		0x00
#define DCON_BIG_ENDIAN		0x02
#define DCON_LITTLE_ENDIAN	0x00
#define DCON_BUS16		0x01
#define DCON_BUS_8		0x00

/* INTMASK: Interrupt Mask Register */
#define IM_XDCE			0x40
#define IM_CNTE			0x20
#define IM_OVWE			0x10
#define IM_TXEE			0x08
#define IM_RXEE			0x04
#define IM_PTXE			0x02
#define IM_PRXE			0x01

/* INTSTAT: Interrupt Status Register */
#define ISTAT_RST		0x80
#define ISTAT_RDC		0x40		/* remote DMA complete */
#define ISTAT_CNT		0x20
#define ISTAT_OVW		0x10
#define ISTAT_TXE		0x08
#define ISTAT_RXE		0x04
#define ISTAT_PTX		0x02
#define ISTAT_PRX		0x01

/* TSTAT: Transmit Status Register */
#define TSTAT_OWC		0x80
#define TSTAT_CDH		0x40
#define TSTAT_UNDER		0x20
#define TSTAT_CRL		0x10
#define TSTAT_ABORT		0x08
#define TSTAT_TWC		0x04
#define TSTAT_NDT		0x02
#define TSTAT_PTX		0x01

/* RSTAT: Receive Status Register */
#define RSTAT_DFR		0x80
#define RSTAT_DIS		0x40
#define RSTAT_GROUP		0x20
#define RSTAT_MPA		0x10
#define RSTAT_OVER		0x08
#define RSTAT_FAE		0x04
#define RSTAT_CRC		0x02
#define RSTAT_PRX		0x01

/* ENH: Enable Features */
#define ENH_WAIT1		0x80
#define ENH_WAIT0		0x40
#define ENH_SLOT1		0x10
#define ENH_SLOT0		0x08

/* flags - software synchronize bit definitions */
#define TXING			0x01
#define RXING			0x02
#define TXREQ			0x04

#ifdef __cplusplus
}
#endif

#endif  /* __INCne2000Endh */
