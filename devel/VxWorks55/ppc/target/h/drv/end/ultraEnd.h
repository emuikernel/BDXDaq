/* ultraEnd.h - SMC Elite Ultra END network interface header */ 

/* Copyright 1984-1999 Wind River Systems, Inc. */

/*
modification history
--------------------
01a,11jan99,dra  written. 
*/

#ifndef	__INCultraEndh
#define	__INCultraEndh

#ifdef __cplusplus
extern "C" {
#endif

#define DEV_NAME		"ultra"
#define DEV_NAME_LEN		6

#define	ULTRA_RAMSIZE		0x4000		/* 16 Kbytes */
#define	ULTRA_TSTART0		0x00
#define	ULTRA_TSTART1		0x08
#define	ULTRA_PSTART		0x10
#define	ULTRA_PSTOP		0x40
#define ULTRA_MIN_SIZE		ETHERSMALL
#define ULTRA_MAX_SIZE		(SIZEOF_ETHERHEADER+ETHERMTU)

#define ULTRA_BUFSIZ		(ULTRA_MAX_SIZE + 6)
#define ULTRA_MIN_FBUF		(ULTRA_BUFSIZ)

typedef struct 
    {
    UCHAR rstat;
    UCHAR next;
    UCHAR lowByteCnt;
    UCHAR uppByteCnt;
    } ULTRA_HEADER;

typedef struct
    {
    char irq;
    char reg;
    } IRQ_TABLE;

/* SMC Elite Ultra */ 

#define CTRL_CON		0x00	/* Control */
#define CTRL_EEROM		0x01	/* EEROM */
#define CTRL_HARD		0x04	/* Hardware */
#define CTRL_BIOS		0x05	/* BIOS page */
#define CTRL_INT		0x06	/* Interrupt control */
#define CTRL_REV		0x07	/* Revision */
#define CTRL_LAN0		0x08	/* SWH=0 LAN address */
#define CTRL_LAN1		0x09	/* SWH=0 LAN address */
#define CTRL_LAN2		0x0a	/* SWH=0 LAN address */
#define CTRL_LAN3		0x0b	/* SWH=0 LAN address */
#define CTRL_LAN4		0x0c	/* SWH=0 LAN address */
#define CTRL_LAN5		0x0d	/* SWH=0 LAN address */
#define CTRL_BDID		0x0e	/* SWH=0 Board ID */
#define CTRL_CKSM		0x0f	/* SWH=0 Checksum */

#define CTRL_PIDL		0x08	/* SWH=1 POS ID */
#define CTRL_PIDH		0x09	/* SWH=1 POS ID */
#define CTRL_IOADDR		0x0a	/* SWH=1 IO address */
#define CTRL_MEMADDR		0x0b	/* SWH=1 MEM address */
#define CTRL_BIO		0x0c	/* SWH=1 BIOS base */
#define CTRL_GCON		0x0d	/* SWH=1 Gen control */

/* page-0, read */

#define LAN_PAGESIZE		0x20	/* page size */    
#define LAN_CMD			0x10	/* command */
#define LAN_BOUND		0x13	/* boundary page */
#define LAN_TSTAT		0x14	/* transmit status */
#define LAN_COLCNT		0x15	/* collision error */
#define LAN_INTSTAT		0x17	/* interrupt status */
#define LAN_RSTAT		0x1c	/* receive status */
#define LAN_ALICNT		0x1d	/* alignment error */
#define LAN_CRCCNT		0x1e	/* crc error */
#define LAN_MPCNT		0x1f	/* missed packet */

/* page-0, write */

#define LAN_RSTART		0x11	/* receive start */
#define LAN_RSTOP		0x12	/* receive stop */
#define LAN_TSTART		0x14	/* transmit start */
#define LAN_TCNTL		0x15	/* transmit counter */
#define LAN_TCNTH		0x16	/* transmit counter */
#define LAN_RCON		0x1c	/* receive config */
#define LAN_TCON		0x1d	/* transmit config */
#define LAN_DCON		0x1e	/* data config */
#define LAN_INTMASK		0x1f	/* interrupt mask */

/* page-1, read and write */

#define LAN_STA0		0x11	/* station address */
#define LAN_STA1		0x12	/* station address */
#define LAN_STA2		0x13	/* station address */
#define LAN_STA3		0x14	/* station address */
#define LAN_STA4		0x15	/* station address */
#define LAN_STA5		0x16	/* station address */
#define LAN_CURR		0x17	/* current page */
#define LAN_MAR0		0x18	/* multicast address */
#define LAN_MAR1		0x19	/* multicast address */
#define LAN_MAR2		0x1a	/* multicast address */
#define LAN_MAR3		0x1b	/* multicast address */
#define LAN_MAR4		0x1c	/* multicast address */
#define LAN_MAR5		0x1d	/* multicast address */
#define LAN_MAR6		0x1e	/* multicast address */
#define LAN_MAR7		0x1f	/* multicast address */

/* page-2, read and write */

#define LAN_NEXT		0x15	/* next page */
#define LAN_ENH			0x17	/* enable features */

/* Control Register */

#define CON_RESET		0x80
#define CON_MENABLE		0x40

/* Hardware Support Register */

#define HARD_SWH		0x80

/* BIOS Page Register */

#define BIOS_M16EN		0x80

/* Interrupt Control Register */

#define INT_ENABLE		0x01

/* Command Register */

#define CMD_PS1			0x80
#define CMD_PS0			0x40
#define CMD_TXP			0x04
#define CMD_STA			0x02
#define CMD_STP			0x01

/* Receive Configuration Register */

#define RCON_MON		0x20
#define RCON_PROM		0x10
#define RCON_GROUP		0x08
#define RCON_BROAD		0x04
#define RCON_RUNTS		0x02
#define RCON_SEP		0x01

/* Transmit Configuration Register */

#define TCON_LB1		0x04
#define TCON_LB0		0x02
#define TCON_CRCN		0x01

/* Data Configuration Register */

#define DCON_BSIZE1		0x40
#define DCON_BSIZE0		0x20
#define DCON_BUS16		0x01

/* Interrupt Mask Register */

#define IM_CNTE			0x20
#define IM_OVWE			0x10
#define IM_TXEE			0x08
#define IM_RXEE			0x04
#define IM_PTXE			0x02
#define IM_PRXE			0x01

/* Interrupt Status Register */

#define ISTAT_RST		0x80
#define ISTAT_CNT		0x20
#define ISTAT_OVW		0x10
#define ISTAT_TXE		0x08
#define ISTAT_RXE		0x04
#define ISTAT_PTX		0x02
#define ISTAT_PRX		0x01

/* Transmit Status Register */

#define TSTAT_OWC		0x80
#define TSTAT_CDH		0x40
#define TSTAT_UNDER		0x20
#define TSTAT_CRL		0x10
#define TSTAT_ABORT		0x08
#define TSTAT_TWC		0x04
#define TSTAT_NDT		0x02
#define TSTAT_PTX		0x01

/* Receive Status Register */

#define RSTAT_DFR		0x80
#define RSTAT_DIS		0x40
#define RSTAT_GROUP		0x20
#define RSTAT_MPA		0x10
#define RSTAT_OVER		0x08
#define RSTAT_FAE		0x04
#define RSTAT_CRC		0x02
#define RSTAT_PRX		0x01

/* Enable Features */

#define ENH_WAIT1		0x80
#define ENH_WAIT0		0x40
#define ENH_SLOT1		0x10
#define ENH_SLOT0		0x08

/* BIO Base Register */

#define BIO_FINE16		0x80

#ifdef __cplusplus
}
#endif

#endif	/* __INCultraEndh */
