/* ln7990End.h - END based AMD LANCE Ethernet header */

/* Copyright 1984-1998 Wind River Systems, Inc. */
/*
modification history
--------------------
01f,22sep98,dat  Fixed lint warnings in LN_RMD_TO_ADDR by adding parens
01e,02jun97,gnn  Changed LN_TMD_TO_ADDR to take into account Force 30 issue.
01d,21jan97,gnn  Added LN_TMD_TO_ADDR to map a address from a TMD.
01c,13nov96,dat  Removed LN_REGS.
01b,22oct96,gnn  moved some stuff to ln7990EndCommon.h
                 added macros for accesses to registers
                 cleaned up some structs and unions.
01a,15aug96,gnn  copied from if_ln.h.
*/

#ifndef __INCln7990Endh
#define __INCln7990Endh

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Control block definitions for AMD LANCE (Ethernet) chip.
 * This has some of the same (mis)features as the Intel 82586 with
 * regards to byte ordering.  It assumes that a memory address specifies
 * the location of the least significant byte of a multi-byte value.  This
 * is correct for most Intel & DEC processors, but is wrong for 680x0s.
 * As a result, all addresses specified to the chip must have their
 * words swapped.  At least it has a control bit to automatically swap
 * bytes during data transfer dma.  (The 82586 is much worse.)
 */

/*
 * Initialization Block.
 * Specifies addresses of receive and transmit descriptor rings.
 */
typedef struct lnIB
    {
    u_short	lnIBMode;		/* mode register */
    char	lnIBPadr [6]; /* PADR: byte swapped ethernet physical address */
					/* LADRF: logical address filter */
    u_short	lnIBLadrfLow;		/* least significant word */
    u_short	lnIBLadrfMidLow;	/* low middle word */
    u_short	lnIBLadrfMidHigh;  	/* high middle word */
    u_short	lnIBLadrfHigh;		/* most significant word */
					/* RDRA: read ring address */
    u_short	lnIBRdraLow;		/* low word */
    u_short	lnIBRdraHigh;		/* high word */
					/* TDRA: transmit ring address */
    u_short	lnIBTdraLow;		/* low word */
    u_short	lnIBTdraHigh;		/* high word */
    } ln_ib;

/*
 * Receive Message Descriptor Entry.
 * Four words per entry.  Number of entries must be a power of two.
 */
typedef struct lnRMD
    {
    u_short	lnRMD0;		/* bits 15:00 of receive buffer address */
    u_short	lnRMD1;	        /* bits 23:16 of receive buffer address */
    u_short	lnRMD2;			/* buffer byte count (negative) */
    u_short	lnRMD3;			/* message byte count */
    } ln_rmd;


#define	rbuf_ladr	lnRMD0
#define rbuf_hadr	lnRMD1
#define	rbuf_bcnt	lnRMD2
#define	rbuf_mcnt	lnRMD3

/*
 * Transmit Message Descriptor Entry.
 * Four words per entry.  Number of entries must be a power of two.
 */
typedef struct lnTMD
    {
    u_short	lnTMD0;		/* bits 15:00 of transmit buffer address */

    union
		{
        u_short	    lnTMD1;	/* bits 23:16 of transmit buffer address */
		u_short     ln_tmd1b;
		} ln_tmd1;

    u_short	lnTMD2;			/* message byte count */

    union
		{
		u_short	    lnTMD3;		/* errors */
		u_short     lntmd3b;
        } ln_tmd3;
    } ln_tmd;

#define	tbuf_ladr	lnTMD0
#define tbuf_hadr	ln_tmd1.lnTMD1
#define tbuf_stat	ln_tmd1.lnTMD1
#define	tbuf_bcnt	lnTMD2
#define tbuf_err	ln_tmd3.lnTMD3

/* Definitions for fields and bits in the LN_REGS */

#define lnrmd1_OWN      0x8000          /* Own */
#define lnrmd1_ERR      0x4000          /* Error */
#define lnrmd1_FRAM     0x2000          /* Framming Error */
#define lnrmd1_OFLO     0x1000          /* Overflow */
#define lnrmd1_CRC      0x0800          /* CRC */
#define lnrmd1_BUFF     0x0400          /* Buffer Error */
#define lnrmd1_STP      0x0200          /* Start of Packet */
#define lnrmd1_ENP      0x0100          /* End of Packet */
#define lnrmd1_HADR     0x00FF          /* High Address */

#define lntmd1_OWN      0x8000          /* Own */
#define lntmd1_ERR      0x4000          /* Error */
#define lntmd1_MORE     0x1000          /* More than One Retry */
#define lntmd1_ONE      0x0800          /* One Retry */
#define lntmd1_DEF      0x0400          /* Deferred */
#define lntmd1_STP      0x0200          /* Start of Packet */
#define lntmd1_ENP      0x0100          /* End of Packet */
#define lntmd1_HADR     0x00FF          /* High Address */

#define lntmd3_BUFF     0x8000          /* Buffer Error */
#define lntmd3_UFLO     0x4000          /* Underflow Error */

#define lntmd3_LCOL     0x1000          /* Late Collision */
#define lntmd3_LCAR     0x0800          /* Lost Carrier */
#define lntmd3_RTRY     0x0400          /* Retry Error */
#define lntmd3_TDR      0x03FF          /* Time Domain Reflectometry */

/*
 * Generic shared memory read and write macros.
 */

#ifndef SHMEM_RD
#    define SHMEM_RD(x)	*(x)
#endif

#ifndef SHMEM_WR
#    define SHMEM_WR(x,y)	(*(x) = y)
#endif

/*
 * Macros for read and write descriptors.
 *
 */
#define LN_RMD_BUF_TO_ADDR(rmd, tmp, buf) \
	tmp = LN_CACHE_VIRT_TO_PHYS (buf); \
	rmd->rbuf_ladr = (u_long) tmp;\
	rmd->rbuf_hadr = (((u_long) tmp >> 16) & lnrmd1_HADR) | lnrmd1_OWN;

#define LN_TMD_BUF_TO_ADDR(tmd, tmp, buf) \
	tmp = LN_CACHE_VIRT_TO_PHYS (buf); \
	tmd->tbuf_ladr = (u_long) tmp;\
	tmd->tbuf_hadr = (u_long) tmp >> 16;

#define LN_TMD_TO_ADDR(tmd, addr) \
	addr = (tmd->tbuf_ladr | (tmd->tbuf_hadr << 16)) & 0x00ffffff;

#define LN_RMD_TO_ADDR(pool, rmd, addr) \
	addr = (((ULONG)pool & 0xff000000) | \
            ((rmd->rbuf_ladr | (rmd->rbuf_hadr << 16)) & 0x00ffffff));

#define LN_ADDR_TO_RMD(addr, rmd) \
	rmd->rbuf_ladr = addr; \
	rmd->rbuf_hadr = (addr >> 16) & lnrmd1_HADR;

#define LN_CLEAN_RXD(rmd) \
	pRmd->rbuf_hadr &= 0xff;\
	pRmd->rbuf_mcnt = 0; \
	pRmd->lnRMD1 |= lnrmd1_OWN; 

#define LN_ADDR_TO_IB_RMD(addr, ib, rsize) \
	ib->lnIBRdraLow = (u_long) addr; \
	ib->lnIBRdraHigh = (((u_long) addr >> 16) & 0xff) | (rsize << 13);
	
#define LN_ADDR_TO_IB_TMD(addr, ib, tsize) \
	ib->lnIBTdraLow = (u_long) addr; \
	ib->lnIBTdraHigh = (((u_long) addr >> 16) & 0xff) | (tsize << 13);

#define LN_RMD_OWNED(rmd) \
	(rmd->lnRMD1 & lnrmd1_OWN)
	
#define LN_ADDRF_CLEAR(ib) \
	ib->lnIBLadrfLow = 0; \
	ib->lnIBLadrfMidLow = 0; \
	ib->lnIBLadrfMidHigh = 0; \
	ib->lnIBLadrfHigh = 0;

#define LN_ADDRF_SET(ib, crc) \
	switch (crc >> 4)\
	    {\
	    case 0:\
		pIb->lnIBLadrfLow |= 1 << (crc & 0xf);\
		break;\
	    case 1:	\
		pIb->lnIBLadrfMidLow |= 1 << (crc & 0xf);\
		break;\
	    case 2:\
		pIb->lnIBLadrfMidHigh |= 1 << (crc & 0xf);\
		break;\
	    case 3:\
		pIb->lnIBLadrfHigh |= 1 << (crc & 0xf);\
		break;\
	    default:\
		break;\
	    }

#define LN_PKT_LEN_GET(rmd) \
	pRmd->rbuf_mcnt - 4;

#define LN_RMD_ERR(rmd) \
	(rmd->lnRMD1 & lnrmd1_ERR) || \
	( \
	(rmd->lnRMD1 & (lnrmd1_STP | lnrmd1_ENP)) != \
		       (lnrmd1_STP | lnrmd1_ENP))

#define LN_TMD_CLR_ERR(tmd) \
	tmd->tbuf_stat &= lnrmd1_HADR
#ifdef __cplusplus
}
#endif

#endif /* __INCln7990Endh */
