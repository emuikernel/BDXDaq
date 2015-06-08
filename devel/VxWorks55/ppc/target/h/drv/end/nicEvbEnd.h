/* nicEvbEnd.h - NIC driver header (for EVB403) */

/* Copyright 1984-1999 Wind River Systems, Inc. */

/*
modification history
--------------------
01a,20jan99,sut  from netif/if_nicEvb.h
*/

#ifndef __INCnicEvbEndh
#define __INCnicEvbEndh

#ifdef __cplusplus
extern "C" {
#endif

/* driver flags */
typedef UINT8 NIC_DRV_FLAG;

#define NIC_FLAG_POLL 0x01    
    
typedef	union nic_regs
{
    u_char reg [16];
    struct
    {
	volatile u_char cr;
	volatile u_char pstart;
	volatile u_char pstop;
	volatile u_char bnry;
	volatile u_char tpsr;
	volatile u_char tbcr0;
	volatile u_char tbcr1;
	volatile u_char isr;
	volatile u_char rsar0;
	volatile u_char rsar1;
	volatile u_char rbcr0;
	volatile u_char rbcr1;
	volatile u_char rcr;
	volatile u_char tcr;
	volatile u_char dcr;
	volatile u_char imr;
    } nic_pg0;
    struct
    {
	volatile u_char cr;
	volatile u_char par0;
	volatile u_char par1;
	volatile u_char par2;
	volatile u_char par3;
	volatile u_char par4;
	volatile u_char par5;
	volatile u_char curr;
	volatile u_char mar0;
	volatile u_char mar1;
	volatile u_char mar2;
	volatile u_char mar3;
	volatile u_char mar4;
	volatile u_char mar5;
	volatile u_char mar6;
	volatile u_char mar7;
    } nic_pg1;
} NIC_DEVICE;

/* Page 1 offsets IR - Internal Register */
#define NIC_IR_CR	0x00	/* Command register */
#define NIC_IR_PAR0	0x01	/* Phsical Address Register 0 */
#define NIC_IR_PAR1	0x02	/* Phsical Address Register 1 */
#define NIC_IR_PAR2	0x03	/* Phsical Address Register 2 */
#define NIC_IR_PAR3	0x04	/* Phsical Address Register 3 */
#define NIC_IR_PAR4	0x05	/* Phsical Address Register 4 */
#define NIC_IR_PAR5	0x06	/* Phsical Address Register 5 */
#define NIC_IR_CURR	0x07	/* Current Page Register */
#define NIC_IR_MAR0	0x08	/* Multicast Address Register 0 */
#define NIC_IR_MAR1	0x09	/* Multicast Address Register 1 */
#define NIC_IR_MAR2	0x0a	/* Multicast Address Register 2 */
#define NIC_IR_MAR3	0x0b	/* Multicast Address Register 3 */
#define NIC_IR_MAR4	0x0c	/* Multicast Address Register 4 */
#define NIC_IR_MAR5	0x0d	/* Multicast Address Register 5 */
#define NIC_IR_MAR6	0x0e	/* Multicast Address Register 6 */    
#define NIC_IR_MAR7	0x0f	/* Multicast Address Register 7 */
    
#define Cr		nic_pg0.cr
#define Pstart	nic_pg0.pstart
#define Pstop	nic_pg0.pstop
#define Bnry	nic_pg0.bnry
#define Tpsr	nic_pg0.tpsr
#define Tsr		nic_pg0.tpsr
#define Tbcr0	nic_pg0.tbcr0
#define Tbcr1	nic_pg0.tbcr1
#define Isr		nic_pg0.isr
#define Rsar0	nic_pg0.rsar0
#define Rsar1	nic_pg0.rsar1
#define Rbcr0	nic_pg0.rbcr0
#define Rbcr1	nic_pg0.rbcr1
#define Rcr		nic_pg0.rcr
#define Rsr		nic_pg0.rcr
#define Tcr		nic_pg0.tcr
#define Dcr		nic_pg0.dcr
#define Imr		nic_pg0.imr

#define Par0	nic_pg1.par0
#define Par1	nic_pg1.par1
#define Par2	nic_pg1.par2
#define Par3	nic_pg1.par3
#define Par4	nic_pg1.par4
#define Par5	nic_pg1.par5
#define Curr	nic_pg1.curr

#define NIC_BASE	0xf4000000
#define NIC_PORT	0xf4000010
#define NIC_ACC		0xf4000030
#define NIC_DMA		0xf4000020

#define NICRAM_BOT	0x8000
#define NICRAM_TOP	0xFFFF
#define NICROM_BOT	0x0000
#define NICROM_TOP	0x001F
#define NICROMSIZ	(NICROM_TOP - NICROM_BOT)
#define NICRAMSIZ	(NICRAM_TOP - NICRAM_BOT)

#define PBMASK		0xc0		/* XXX */
#define ACC_MASK	0x01		/* XXX */
#define PACKET_SIZE	256


#define	PSTART		8
#define	PSTOP		32
#define MAXXMT		10
#define	BNRY		PSTART
#define CURR		PSTART+1
#define XMTBUF		0xe100

/*
 * CR Register bits (SAME)
 */

#define STP		0x1
#define STA		0x2
#define TXP		0x4
#define RREAD	0x8
#define RWRITE	0x10
#define SPKT	0x18
#define ABORT	0x20
#define RPAGE0	0x00
#define RPAGE1	0x40
#define RPAGE2	0x80

/*
 * ISR Register (SAME)
 */

#define PRX		0x1
#define PTX		0x2
#define RXE		0x4
#define TXE		0x8
#define OVW		0x10
#define CNT		0x20
#define RDC		0x40
#define RST		0x80

/*
 * IMR(SAme)
 */

#define PRXE	0x1
#define PTXE	0x2
#define RXEE	0x4
#define TXEE	0x8
#define OVWE	0x10
#define CNTE	0x20
#define RDCE	0x40

/*
 * DCR
 */

#define WTS		0x1
#define BOS		0x2
#define LAS		0x4
#define NOTLS	0x8
#define DCR_ARM		0x10
#define FIFO2	0x00
#define FIFO4	0x20
#define FIFO8	0x40
#define FIFO12	0x60

/*
 * TCR
 */

#define CRC 	0x1
#define MODE0	0x0
#define MODE1	0x2
#define MODE2	0x4
#define MODE3	0x6
#define ATD		0x8
#define OFST	0x10

/*
 * TSR
 */

#define TPTX	0x1
#define COL		0x4
#define ABT		0x8
#define CRS		0x10
#define FU		0x20
#define CDH		0x40
#define OWC		0x80

/*
 * RCR
 */

#define	SEP		0x1
#define AR		0x2
#define AB		0x4
#define AM		0x8
#define PRO		0x10
#define MON		0x20

/*
 * RSR
 */

#define CRCR	0x2
#define FAE		0x4
#define FO		0x8
#define MPA		0x10
#define PHY		0x20
#define DIS		0x40
#define DFR		0x80

#define MINPKTSIZE	64

#ifdef __cplusplus
}
#endif

#endif /* __INCnicEvbEndh */
