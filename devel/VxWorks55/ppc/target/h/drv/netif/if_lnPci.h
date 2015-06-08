/* if_lnPci.h - AMD LANCE 79C970 Ethernet interface header */

/* Copyright 1984-2001 Wind River Systems, Inc. */

/*
modification history
--------------------
01b,09nov01,dat  Adding obsolescence warnings to outdated drivers
01a,11Jan95,vin  written from version 01p of if_ln.h.
*/

/*
DESCRIPTION
-----------
Pci Bus is inherently little endian. So if a big endian processor is 
interfaced with the PCI bus all the values programmed to the registers
on the PCI bus should be swapped.
*/

#ifndef __INCif_lnPcih
#define __INCif_lnPcih

#warning "if_lnPci driver is obsolete, please use ln97xEnd driver"

#ifdef __cplusplus
extern "C" {
#endif

#if (_BYTE_ORDER == _BIG_ENDIAN)

#define PCISWAP(x)	LONGSWAP(x)		/* processor big endian */

#else

#define PCISWAP(x)	(x)			/* processor little endian */

#endif /* _BYTE_ORDER == _BIG_ENDIAN */

/*
 * Control and Status Register access structure
 * WARNING: bit operations should not be done on CSR0!
 * Many of the bits are Read/Clear and will be cleared by any other
 * bit operation on the register.  Note especially that if the
 * stop bit is set, all other operations will retain the stopped
 * condition.  This means that a bit set to do init and start will
 * always fail because stop has priority.
 */
typedef struct
    {
    ULONG	rdp;		/* register data Port */
    ULONG	rap;		/* Register Address Port */
    ULONG	rr;		/* Reset Register */
    ULONG	bdp;		/* Bus Configuration Register Data Port */
    } LN_DEVICE;

/*
 * Initialization Block.
 * Specifies addresses of receive and transmit descriptor rings.
 */
typedef struct lnIB
    {
    ULONG	lnIBMode;		/* mode register */
    char	lnIBPadr [8]; 		/* PADR: bswapd ethnt phys address */
					/* LADRF: logical address filter */
    ULONG	lnIBLadrfLow;		/* least significant long word */
    ULONG	lnIBLadrfHigh;		/* most significant long word */
					/* RDRA: read ring address */
    ULONG	lnIBRdra;		/* long word */
					/* TDRA: transmit ring address */
    ULONG	lnIBTdra;		/* long word */
    } ln_ib;

/*
 * Receive Message Descriptor Entry.
 * Four words per entry.  Number of entries must be a power of two.
 */
typedef struct lnRMD
    {
    ULONG	lnRMD0;		/* bits 31:00 of receive buffer address */
    ULONG	lnRMD1;		/* status & buffer byte count (negative) */
    ULONG	lnRMD2;		/* message byte count */
    ULONG	lnRMD3;		/* reserved */
    } ln_rmd;

/*
 * Transmit Message Descriptor Entry.
 * Four words per entry.  Number of entries must be a power of two.
 */
typedef struct lnTMD
    {
    ULONG	lnTMD0;		/* bits 31:00 of transmit buffer address */
    ULONG	lnTMD1;		/* message byte count */
    ULONG	lnTMD2;		/* errors */
    ULONG	lnTMD3;		/* reserved */
    } ln_tmd;

/* csr and bcr addresses */

#define CSR(x)			(x)
#define BCR(x)			(x)

/* Definitions for fields and bits in the LN_DEVICE */

#define  CSR0_ERR		0x8000	/* (RO) err flg (BABL|CERR|MISS|MERR) */
#define  CSR0_BABL		0x4000	/* (RC) babble transmitter timeout */
#define  CSR0_CERR		0x2000	/* (RC) collision error */
#define  CSR0_MISS		0x1000	/* (RC) missed packet */
#define  CSR0_MERR		0x0800	/* (RC) memory error */
#define  CSR0_RINT		0x0400	/* (RC) receiver interrupt */
#define  CSR0_TINT		0x0200	/* (RC) transmitter interrupt */
#define  CSR0_IDON		0x0100	/* (RC) initialization done */
#define  CSR0_INTR		0x0080	/* (RO) interrupt flag */
#define  CSR0_INEA		0x0040	/* (RW) interrupt enable */
#define  CSR0_RXON		0x0020	/* (RO) receiver on */
#define  CSR0_TXON		0x0010	/* (RO) transmitter on */
#define  CSR0_TDMD		0x0008	/* (WOO)transmit demand */
#define  CSR0_STOP		0x0004	/* (WOO)stop (& reset) chip */
#define  CSR0_STRT		0x0002	/* (RW) start chip */
#define  CSR0_INIT		0x0001	/* (RW) initialize (acces init block) */

#define CSR0_INTMASK 		(CSR0_BABL \
				 | CSR0_CERR \
				 | CSR0_MISS \
				 | CSR0_MERR \
				 | CSR0_RINT \
				 | CSR0_TINT \
				 | CSR0_IDON \
				 | CSR0_INEA)


#define	CSR3_BSWP		0x0004  	/* Byte Swap */

#define BCR20_SSIZE32		0x0100
#define BCR20_SWSTYLE_LANCE	0x0000
#define BCR20_SWSTYLE_ILACC	0x0001
#define BCR20_SWSTYLE_PCNET	0x0002

/* initialization block */

#define IB_MODE_TLEN_MSK	0xf0000000
#define IB_MODE_RLEN_MSK	0x00f00000

/* receive descriptor */

#define RMD1_OWN		0x80000000	/* Own */
#define RMD1_ERR		0x40000000	/* Error */
#define RMD1_FRAM		0x20000000	/* Framming Error */
#define RMD1_OFLO		0x10000000	/* Overflow */
#define RMD1_CRC		0x08000000	/* CRC */
#define RMD1_BUFF		0x04000000	/* Buffer Error */
#define RMD1_STP		0x02000000	/* Start of Packet */
#define RMD1_ENP		0x01000000	/* End of Packet */
#define RMD1_RES		0x00ff0000	/* reserved */
#define RMD1_CNST		0x0000f000	/* rmd1 constant value */

#define RMD1_BCNT_MSK		0x00000fff	/* buffer cnt mask */
#define RMD2_MCNT_MSK		0x00000fff	/* message buffer cnt mask */

/* transmit descriptor */

#define TMD1_OWN		0x80000000	/* Own */
#define TMD1_ERR		0x40000000	/* Error */
#define TMD1_MORE		0x10000000	/* More than One Retry */
#define TMD1_ONE		0x08000000	/* One Retry */
#define TMD1_DEF		0x04000000	/* Deferred */
#define TMD1_STP		0x02000000	/* Start of Packet */
#define TMD1_ENP		0x01000000	/* End of Packet */
#define TMD1_RES		0x00FF0000	/* High Address */
#define TMD1_CNST		0x0000f000	/* tmd1 constant value */

#define TMD2_BUFF		0x80000000	/* Buffer Error */
#define TMD2_UFLO		0x40000000	/* Underflow Error */
#define TMD2_LCOL		0x10000000	/* Late Collision */
#define TMD2_LCAR		0x08000000	/* Lost Carrier */
#define TMD2_RTRY		0x04000000	/* Retry Error */
#define TMD2_TDR		0x03FF0000	/* Time Domain Reflectometry */

#define TMD1_BCNT_MSK		0x00000fff	/* buffer cnt mask */

#define	rbuf_adr		lnRMD0
#define	rbuf_rmd1		lnRMD1
#define	rbuf_mcnt		lnRMD2
#define rbuf_rmd3		lnRMD3

#define	tbuf_adr		lnTMD0
#define tbuf_tmd1		lnTMD1
#define tbuf_tmd2		lnTMD2
#define tbuf_tmd3		lnTMD3

#ifdef __cplusplus
}
#endif

#endif /* __INCif_lnPcih */


