/* if_lnIsa.h - AMD LANCE 79C961 Ethernet interface header */

/* Copyright 1984-1996 Wind River Systems, Inc. */

/*
modification history
--------------------
01a,21Aug96,joes  written from version 01a of if_lnPci.h.
*/

/*
DESCRIPTION
-----------
This device is strictly an ISA bus device. All register setup is done
through sysIn/OutByte calls which are x86 IO accesses
*/

#ifndef __INCif_lnIsah
#define __INCif_lnIsah

#ifdef __cplusplus
extern "C" {
#endif

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
  USHORT  	hwAddr[8]; /* E'net hardware address */
  USHORT	rdp;		/* register data Port */
  USHORT	rap;		/* Register Address Port */
  USHORT	rr;		/* Reset Register */
  USHORT	idp;		/* ISA Configuration Register Data Port */
} LN_DEVICE;

/*
 * Initialization Block.
 * Specifies addresses of receive and transmit descriptor rings.
 */
typedef struct lnIB
{
  USHORT	lnIBMode;		/* mode register */
  char		lnIBPadr [6];		/* PADR: bswapd ethnt phys address */
                                        /* LADRF: logical address filter */
  USHORT	lnIBLadrfLow;		/* least significant word */
  USHORT	lnIBLadrfMidLow;	/* low middle word */
  USHORT	lnIBLadrfMidHigh;  	/* high middle word */
  USHORT	lnIBLadrfHigh;		/* most significant word */
					/* RDRA: read ring address */
  USHORT	lnIBRdraLow;		/* Bits [15:0]*/
  UCHAR		lnIBRdraHigh;		/* Bits [23:16]  */
  UCHAR		lnRLEN;			/* Recv desc. ring len */
					/* TDRA: transmit ring address */
  USHORT	lnIBTdraLow;		/* Bits [15:0]*/
  UCHAR		lnIBTdraHigh;		/* Bits [23:16]  */
  UCHAR		lnTLEN;			/* Transmit desc. ring len */
} ln_ib;

/*
 * Receive Message Descriptor Entry.
 * Four words per entry.  Number of entries must be a power of two.
 */
typedef struct lnRMD
{
  USHORT	lnRMD0;		/* bits 15:00 of receive buffer address */
  USHORT	lnRMD1;		/* status & Hi order address bits */
  USHORT	lnRMD2;		/* buffer byte count */
  USHORT	lnRMD3;		/* #bytes received */
} ln_rmd;

/*
 * Transmit Message Descriptor Entry.
 * Four words per entry.  Number of entries must be a power of two.
 */
typedef struct lnTMD
{
  USHORT	lnTMD0;		/* bits [15:00] of transmit buffer address */
  USHORT	lnTMD1;		/* Status and  bits [23:16] of tx buffer */
  USHORT	lnTMD2;		/* Buffer Byte count */
  USHORT	lnTMD3;		/* Errors */
} ln_tmd;

/* csr and bcr addresses */

#define CSR(x)			(x)
#define BCR(x)			(x)

/* Definitions for fields and bits in the LN_DEVICE */

#define  CSR0_ERR		0x8000	/* (RO) err flg(BABL|CERR|MISS|MERR)*/
#define  CSR0_BABL		0x4000	/* (RC) babble transmitter timeout */
#define  CSR0_CERR		0x2000	/* (RC) collision error */
#define  CSR0_MISS		0x1000	/* (RC) missed packet */
#define  CSR0_MERR		0x0800	/* (RC) memory error */
#define  CSR0_RINT		0x0400	/* (RC) receiver interrupt */
#define  CSR0_TINT		0x0200	/* (RC) transmitter interrupt */
#define  CSR0_IDON		0x0100	/* (RC) initialization done */
#define  CSR0_INTR		0x0080	/* (RO) interrupt flag */
#define  CSR0_IENA		0x0040	/* (RW) interrupt enable */
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


/* receive descriptor */

#define RMD1_OWN		0x8000	/* Own */
#define RMD1_ERR		0x4000	/* Error */
#define RMD1_FRAM		0x2000	/* Framing Error */
#define RMD1_OFLO		0x1000	/* Overflow */
#define RMD1_CRC		0x0800	/* CRC */
#define RMD1_BUFF		0x0400	/* Buffer Error */
#define RMD1_STP		0x0200	/* Start of Packet */
#define RMD1_ENP		0x0100	/* End of Packet */
#define RMD1_HADR_MASK		0x00ff	/* Hi Address bits */

/* transmit descriptor */

#define TMD1_OWN		0x8000	/* Own */
#define TMD1_ERR		0x4000	/* Error */
#define TMD1_ADD_FCS		0x2000	/* Add FCS dynamically */
#define TMD1_MORE		0x1000	/* More than One Retry */
#define TMD1_ONE		0x0800	/* One Retry */
#define TMD1_DEF		0x0400	/* Deferred */
#define TMD1_STP		0x0200	/* Start of Packet */
#define TMD1_ENP		0x0100	/* End of Packet */
#define TMD1_RES		0x00FF	/* High Address bits */

#define TMD3_BUFF		0x8000	/* Buffer Error */
#define TMD3_UFLO		0x4000	/* Underflow Error */
#define TMD3_LCOL		0x1000	/* Late Collision */
#define TMD3_LCAR		0x0800	/* Lost Carrier */
#define TMD3_RTRY		0x0400	/* Retry Error */
#define TMD3_TDR		0x03FF	/* Time Domain Reflectometry */

#define RMD2_BCNT_MSK		0x0fff	/* buffer cnt mask */
#define RMD2_MBO		0xf000	/* RMD2 Must be Ones bits */
#define RMD3_MCNT_MSK		0x0fff	/* message buffer cnt mask */

#define	rbuf_adr		lnRMD0
#define	rbuf_rmd1		lnRMD1
#define	rbuf_bcnt		lnRMD2
#define rbuf_mcnt		lnRMD3

#define TMD2_BCNT_MSK		0x0fff	/* buffer cnt mask */
#define TMD2_MBO		0xf000	/* TMD2 Must be ones */

#define	tbuf_adr		lnTMD0
#define tbuf_tmd1		lnTMD1
#define tbuf_tmd2		lnTMD2
#define tbuf_tmd3		lnTMD3

#define CSR15_PROM	0x8000	/* promiscuous mode */
#define CSR15_DRCVBC	0x4000	/* Disable Recv Broadcast */
#define CSR15_DRVCPA	0x2000	/* Disable Resc Phys Address */
#define CSR15_DLNKTST	0x1000	/* Disable Link Status */
#define CSR15_DAPC	0x0800	/* Disable Auto Polarity Correction */ 
#define CSR15_MENDECL	0x0400	/* MENDEC Loopback */
#define CSR15_LRT	0x0200	/* Low Recv Threshold (T-MAU mode only) */
#define CSR15_GPSI	0x0100	/* PortSel [0:1] = 10 is GPSI */
#define CSR15_10T	0x0080	/* PortSel [0:1] = 01 is 10baseT */
#define CSR15_AUI	0x0000	/* PortSel [0:1] = 00 is AUI */
#define CSR15_INTL	0x0040	/* Internal loopback */
#define CSR15_DRTY	0x0020	/* Disable Retry */ 
#define CSR15_FCOLL	0x0010	/* Force Collision */
#define CSR15_DXMTFCS	0x0008	/* Disable Tx FCS */
#define CSR15_LOOP	0x0004	/* Loopback Enable */ 
#define CSR15_DTX	0x0002	/* Disable Tx */
#define CSR15_DRX	0x0001	/* Disable Rx */

/* All definitions below here are related to the PnP and EEPROM Interface */
     /* lnIdpXXXX function register numbers */
#define ISACSR0		0
#define ISACSR1		1
#define ISACSR2		2
#define ISACSR3		3
#define ISACSR4		4
#define ISACSR5		5
#define ISACSR6		6
#define ISACSR7		7
#define ISACSR8		8

/* ISA CSR 3 EEPROM configuration bit definitions */
#define EE_VALID	0x8000
#define EE_LOAD		0x4000
#define EE_EN		0x0010
#define EE_SHFBUSY	0x0008
#define EE_CS		0x0004
#define EE_SK		0x0002
#define EE_DATA		0x0001

/* 
 * The Following values are used in defining the EEPROM contents. Refer to
 * the diagram outlining EEPROM data in the description of lnIsa_eewrite()
 */
#define EE_PAD 		0x00
#define EISA0		0x00
#define EISA1		0x00
#define EISA2		0x00
#define EISA3		0x00

#define ISACSR0_HI	0x00
#define ISACSR0_LO	0x05
#define ISACSR1_HI	0x00
#define ISACSR1_LO	0x05
#define ISACSR2_HI	0x02	/* PNP Active */
#define ISACSR2_LO	0x01	/* MedSel = 10BaseT */
#define ISACSR5_HI	0x00
#define ISACSR5_LO	0x84	/* LED1 = RCV */
#define ISACSR6_HI	0x00
#define ISACSR6_LO	0x08	/* LED2 = RCVPOL */
#define ISACSR7_HI	0x00
#define ISACSR7_LO	0x90	/* LED3 = XMT */

#define PNP_60		(IO_ADRS_LNISA>>8)	/* IO adrs hi byte */
#define PNP_61		(IO_ADRS_LNISA & 0xff)  /* IO Adrs lo byte */
#define PNP_70		INT_LVL_LNISA		/* Interrupt Level */
#define PNP_71		INT_TYPE_LNISA		/* Interrupt type */
#define PNP_74		DMA_CHAN_LNISA		/* DMA Channel */

/* define these values in config.h if BOOT PROM support required */
#ifndef BPROM_LNISA_PRESENT	
#define BPROM_ADRS_LNISA	0
#define BPROM_CTRL_LNISA 	0
#define BPROM_SIZE_HI		0
#define BPROM_SIZE_LO		0
#endif
#define PNP_40		(BPROM_ADRS_LNISA>>16)	/* Boot Prom Addr Hi */
#define PNP_41		(BPROM_ADRS_LNISA>>8)	/* Boot Prom Addr Lo */
#define PNP_42		BPROM_CTRL_LNISA	/* Boot Prom control */
#define PNP_43		BPROM_SIZE_HI		/* Boot Prom size hi*/
#define PNP_44		BPROM_SIZE_LO		/* Boot Prom size lo*/

/* define these values in config.h if SRAM support required */
#ifndef SRAM_LNISA_PRESENT
#define SRAM_ADRS_LNISA		0
#define SRAM_CTRL_LNISA		0
#define SRAM_SIZE_LO_LNISA	0
#define SRAM_SIZE_HI_LNISA	0
#endif
#define PNP_48		(SRAM_ADRS_LNISA>>16)	/* SRAM addr hi */
#define PNP_49		(SRAM_ADRS_LNISA>>8)	/* SRAM addr lo */
#define PNP_4A		SRAM_CTRL_LNISA		/* SRAM SR16B */
#define PNP_4B		SRAM_SIZE_LO_LNISA	/* SRAM size lo */
#define PNP_4C		SRAM_SIZE_HI_LNISA	/* SRAM size hi */
#define PNP_F0		GENERAL_CTRL_LNISA	/* General Control */
#define EE_CHKSUM	0x00			/* place holder for checksum */

#ifndef EXT_SHIFT0
#define EXT_SHIFT0	0xAA			/* External shift chain */
#endif
#ifndef EXT_SHIFT1
#define EXT_SHIFT1	0xAA			/* External shift chain */
#endif

#define EE_CHKSUM1_OFFSET 6	/* word offset to first chksum word */
#define EE_CHKSUM2_OFFSET 51	/* byte offset to second chksum byte */

#define EE_WORD_LEN_MIN	 0x1B   /* Minimum number of words in valid EEPROM */
#define EE_WORD_LEN_MAX	 128   /* Maximum number of words in valid EEPROM */

#define EE_TWP		3	/* number of ticks to wait for EEPROM write  */


/* Microsoft Plug n'Play Register definitions */

#define PNP_ADDR_PORT 	0x279	/* PnP Register Address Port */
#define PNP_WR_PORT	0xA79	/* PnP Register Write Data Port */
#define PNP_RD_PORT	0x223	/* PnP Register Read Data Port (initial) */

#define PNP_INIT_KEY_LEN 32

#define PNP_SET_RD_DATA 0x0
#define PNP_SERIAL_ISO  0x1
#define PNP_CONFIG_CTRL 0x2
#define PNP_WAKE	0x3
#define PNP_RSRC_DATA	0x4
#define PNP_STATUS	0x5
#define PNP_CSN		0x6
#define PNP_DEV_NUM	0x7

#define PNP_ACTIVATE	0x30
#define PNP_IO_RANGE	0x31

#define PNP_MEM_BASE0_HIBYTE	0x40	/* Mem Desc 0 Base Addr bits[23:16]*/
#define PNP_MEM_BASE0_LOBYTE	0x41	/* Mem Desc 0 Base Addr bits[15:8]*/
#define PNP_MEM_CONTROL		0x42	/* Memory Control Register */
#define PNP_MEM_LIMIT0_HIBYTE	0x43	/* Memory upper limit bits[23:16] */
#define PNP_MEM_LIMIT0_LOBYTE	0x44	/* Memory upper limit bits[15:8] */

#define PNP_IO_BASE0_HIBYTE	0x60	/* IO Desc 0 lower limit bits[15:8] */
#define PNP_IO_BASE0_LOBYTE	0x61	/* IO Desc 0 lower limit bits[7:0] */

#define PNP_IRQ_LEVEL_SEL0	0x70	/* Interrupt level select 0 */
#define PNP_IRQ_TYPE_SEL0	0x71	/* Interrupt type select 0 */

#define PNP_DMA_CHANNEL_SEL0	0x74	/* DMA Channel Select 0 */

/* PNP Config Control Bit definitions */
#define PNP_CONFIG_CTRL_RESETDEV	0x1
#define PNP_CONFIG_CTRL_WAITFORKEY	0x2
#define PNP_CONFIG_CTRL_RESETCSN	0x4

/* PNP Memory Control Register bit defintions */
#define PNP_CONTROL_RANGE	0x0
#define PNP_CONTROL_LIMIT	0x1
#define PNP_CONTROL_8BIT	0x00
#define PNP_CONTROL_16BIT	0x02
#define PNP_CONTROL_16OR8BIT	0x04
#define PNP_CONTROL_32BIT	0x06 /*not supported by AMD961 */

/* PNP memory range definitions */
#define PNP_MEM_RANGE_NONE	0x0
#define PNP_MEM_RANGE_8K	0xf
#define PNP_MEM_RANGE_16K	0xe
#define PNP_MEM_RANGE_32K	0xc
#define PNP_MEM_RANGE_64K	0x8


#ifdef __cplusplus
}
#endif

#endif /* __INCif_lnIsah */


