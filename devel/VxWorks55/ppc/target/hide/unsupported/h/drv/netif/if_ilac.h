/* if_ilac.h - AMD 79C900 Ethernet network interface driver */

/*
 * @(#)ilacc.h	1.1 CETIA 93/03/31
 *
 * This header file contains information specific to the AM 79C900 ILACC
 */

/*
modification history
--------------------
01a,02jan93,kat  written based on code supplied by cetia.
*/

#ifndef _IF_ILAC_H_
#define _IF_ILAC_H_

#define ILAC_ADDR               0xFD800000
#define ROUND_BUF_SIZE          0x600
#define ROUND_TRING_SIZE        0x10
#define ROUND_RRING_SIZE        0x10
#define RECV_DES_CNT            0x20
#define L2RECV_DES_CNT          0x5
#define XMIT_DES_CNT            0x1
#define L2XMIT_DES_CNT          0x0

/*
 * There are five control and status registers:
 *	csr0 contains misc error and status bits.
 *	csr1 contains the low order word of the address of the init block.
 *	csr2 contains the high order byte of the address of the init block.
 *	csr3 allows redefinition of the bus master interface and masking IT.
 *	csr4 
 */
/* bits in csr0 */
#define CSR0_ERR	0x8000	/* error summary */
#define CSR0_BABL	0x4000	/* babble error */
#define CSR0_CERR	0x2000	/* collision error */
#define CSR0_MISS	0x1000	/* missed packet */
#define CSR0_MERR	0x0800	/* memory error */
#define CSR0_RINT	0x0400	/* receiver interrupt */
#define CSR0_TINT	0x0200	/* transmitter interrupt */
#define CSR0_IDON	0x0100	/* initialization done */
#define CSR0_INTR	0x0080	/* interrupt flag */
#define CSR0_INEA	0x0040	/* interrupt enable */
#define CSR0_RXON	0x0020	/* receiver on */
#define CSR0_TXON	0x0010	/* transmitter on */
#define CSR0_TDMD	0x0008	/* transmit demand */
#define CSR0_STOP	0x0004	/* stop the ilacc */
#define CSR0_STRT	0x0002	/* start the ilacc */
#define CSR0_INIT	0x0001	/* initialize the ilacc */

/* bits in csr3 */
#define CSR3_BABLM	0x4000	/* BABL Mask */
#define CSR3_MISSM	0x1000	/* MISSed packet Mask */
#define CSR3_MERRM	0x0800	/* MEmory eRRor Mask */
#define CSR3_RINTM	0x0400	/* Receive INTerrupt Mask */
#define CSR3_TINTM	0x0200	/* Transmit INTerrupt Mask */
#define CSR3_IDONM	0x0100	/* Initialization DOne Mask */
#define CSR3_BSWP	0x0004	/* byte swap (now read-only)*/
#define CSR3_ACON	0x0002	/* ale control */

/* bits in csr4 */
#define CSR4_BACON_68K  0x0040	/* 32 bit 680x0 */
#define CSR4_TXSTRTM    0x0004	/* Transmit STaRT interrupt Mask  */
#define CSR4_LBDM       0x0001	/* LoopBack Done interrupt Mask */


/* initial setting of csr0 */
#define CSR0_IVALUE	(CSR0_IDON | CSR0_INEA | CSR0_STRT | CSR0_INIT)

/* clear interrupt cause bits */
#define CSR0_ACKINT     (CSR0_RINT | CSR0_TINT | CSR0_MERR | CSR0_MISS | CSR0_IDON)

/* our setting of csr3 */
#define CSR3_VALUE	(CSR3_ACON | CSR3_BSWP)

/*
 * Initialization Block.
 *	Chip initialization includes the reading of the init block to obtain
 *	the operating parameters.
 */

typedef struct
{
    unsigned int   tlen:4;	/* xmit ring length, power of 2 */
    unsigned int   res_t:4;	/* reserved */
    unsigned int   rlen:4;	/* rcv. ring length, power of 2 */
    unsigned int   res_r:4;	/* reserved */
    unsigned short mode;	/* mode register */
    
    unsigned char  padr[8];	/* physical address */
    unsigned short ladrf[4];	/* logical address filter */
    unsigned long  rdra;	/* rcv ring desc addr */
    unsigned long  tdra;	/* xmit ring desc addr */
} INIT_BLK;

/* bits in mode register: allows alteration of the chips operating parameters */
#define IBM_PROM	0x8000	/* promiscuous mode */
#define IBM_INTL	0x0040	/* internal loopback */
#define IBM_DRTY	0x0020	/* disable retry */
#define IBM_COLL	0x0010	/* force collision */
#define IBM_DTCR	0x0008	/* disable transmit crc */
#define IBM_LOOP	0x0004	/* loopback */
#define IBM_DTX		0x0002	/* disable transmitter */
#define IBM_DRX		0x0001	/* disable receiver */

/* 
 * Buffer Management is accomplished through message descriptors organized
 * in ring structures in main memory. There are two rings allocated for the
 * device: a receive ring and a transmit ring. The following defines the 
 * structure of the descriptor rings.
 */

/*****************************************************************************/
/*                  Receive  List type definition                            */
/*****************************************************************************/

typedef struct
{
    unsigned long addr;		/* buf addr */

    unsigned char flags;	/* misc error and status bits */
    unsigned char res;		/* reserved */
    unsigned int  ones:4;	/* 1111*/
    unsigned int  bcnt:12;	/* buffer byte count */

    unsigned char rcc;		/* receive collision count */
    unsigned char rpc;		/* runt packet count */
    unsigned int  zeros:4;	/* 0000 */
    unsigned int  mcnt:12;	/* message byte count */

    unsigned long align;
} RM_DESC;


/* bits in the flags field */
#define RFLG_OWN	0x80	/* ownership bit, 1==LANCE */
#define RFLG_ERR	0x40	/* error summary */
#define RFLG_FRAM	0x20	/* framing error */
#define RFLG_OFLO	0x10	/* overflow error */
#define RFLG_CRC	0x08	/* crc error */
#define RFLG_BUFF	0x04	/* buffer error */
#define RFLG_STP	0x02	/* start of packet */
#define RFLG_ENP	0x01	/* end of packet */

/* bits in the buffer byte count field */
#define RBCNT_ONES	0xf000	/* must be ones */
#define RBCNT_BCNT	0x0fff	/* buf byte count, in 2's compl */

/* bits in the message byte count field */
#define RMCNT_RES	0xf000	/* reserved, read as zeros */
#define RMCNT_BCNT	0x0fff	/* message byte count */

/*****************************************************************************/
/*                  Transmit List type definition                            */
/*****************************************************************************/

/* transmit message descriptor entry */
typedef struct
{
    unsigned long  addr;	/* buf addr */

    unsigned char  status;	/* misc error and status bits */
    unsigned char  res;		/* reserved */
/*
    unsigned int   ones:4;   
    unsigned int   bcnt:12; 
*/ 
    unsigned short bcnt; 

    unsigned int   error:6;	/* errors flags */
    unsigned int   tdr:10;	/* time domain reflectometry */
    unsigned int   res2:12;	/* reserved */
    unsigned int   tcc:4;	/* transmit collision count */

    unsigned long  align;
} TM_DESC;

/* bits in the status field */
#define TST_OWN		0x80	/* ownership bit, 1==LANCE */
#define TST_ERR		0x40	/* error summary */
#define TST_RES		0x20	/* reserved bit */
#define TST_MORE	0x10	/* more than one retry was needed */
#define TST_ONE		0x08	/* one retry was needed */
#define TST_DEF		0x04	/* defer while trying to transmit */
#define TST_STP		0x02	/* start of packet */
#define TST_ENP		0x01	/* end of packet */

/* setting of status field when packet is to be transmitted */
#define TST_XMIT	(TST_STP | TST_ENP | TST_OWN)

/* bits in the buffer byte count field */
#define TBCNT_ONES	0xf000	/* must be ones */
#define TBCNT_BCNT	0x0fff	/* buf byte count, in 2's compl */
#define TBCNT_MIN	64	/* minimum length of buf */
#define TBCNT_MINFC	100	/* minimum length of first chain buf */

/* bits in the error field */
#define TERR_BUFF	0x8000	/* buffer error */
#define TERR_UFLO	0x4000	/* underflow error */
#define TERR_RES	0x2000	/* reserved bit */
#define TERR_LCOL	0x1000	/* late collision */
#define TERR_LCAR	0x0800	/* loss of carrier */
#define TERR_RTRY	0x0400	/* retry error */
#define TERR_TDR	0x03ff	/* time domain reflectometry */
	
#define HW_DELAY        0x2000000           /* Hardware timing */
#define INIT_TMO        HW_DELAY
#define STRT_TMO        HW_DELAY
#define RECV_TMO        HW_DELAY
#define XMIT_TMO        HW_DELAY/3

typedef struct
{				/* device registers */
    unsigned short unused;	/* unused */
    unsigned short rdp;		/* one of registers csr00-58 */
    unsigned char  reserv[7];	/* unused */
    unsigned char  rap;		/* register select */
} ILAC_DEVICE;

#endif
