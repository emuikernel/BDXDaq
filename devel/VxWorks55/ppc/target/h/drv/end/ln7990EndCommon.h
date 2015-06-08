/* ln7990EndCommon.h - defines common across all versions of the driver.  */

/* Copyright 1984-2002 Wind River Systems, Inc. */

/*
modification history
--------------------
01c,26apr02,dat  Adding cplusplus protection, SPR 75017
01b,21jan97,gnn Added LN_MIN_FBUF for the minimum size of the first TMD buf
                in a buffer chain.
01a,01oct96,gnn	written.
*/
 
/*
DESCRIPTION

INCLUDE FILES:
*/

#ifndef __INCln7990EndCommonh
#define __INCln7990EndCommonh

/* includes */

#ifdef __cplusplus
extern "C" {
#endif

/* defints */
#define TMD_OWN		0x8000
#define TMD_ERR		0x6000

#define TMD_BUFF	0x8000
#define TMD_UFLO	0x4000
#define TMD_LCAR	0x0800

#define LN_MIN_FBUF     100    /* Minimum size of the first buffer in a */
                               /* chain. */

/* Configuration items */

#define RMD_SIZ	sizeof(ln_rmd)
#define TMD_SIZ	sizeof(ln_tmd)
#define IB_SIZ	sizeof(ln_ib)

#define  lncsr_ERR    0x8000	/* (RO) error flag (BABL|CERR|MISS|MERR) */
#define  lncsr_BABL   0x4000	/* (RC) babble transmitter timeout */
#define  lncsr_CERR   0x2000	/* (RC) collision error */
#define  lncsr_MISS   0x1000	/* (RC) missed packet */
#define  lncsr_MERR   0x0800	/* (RC) memory error */
#define  lncsr_RINT   0x0400	/* (RC) receiver interrupt */
#define  lncsr_TINT   0x0200	/* (RC) transmitter interrupt */
#define  lncsr_IDON   0x0100	/* (RC) initialization done */
#define  lncsr_INTR   0x0080	/* (RO) interrupt flag */
#define  lncsr_INEA   0x0040	/* (RW) interrupt enable */
#define  lncsr_RXON   0x0020	/* (RO) receiver on */
#define  lncsr_TXON   0x0010	/* (RO) transmitter on */
#define  lncsr_TDMD   0x0008	/* (WOO)transmit demand */
#define  lncsr_STOP   0x0004	/* (WOO)stop (& reset) chip */
#define  lncsr_STRT   0x0002	/* (RW) start chip */
#define  lncsr_INIT   0x0001	/* (RW) initialize (acces init block) */

#define lncsr3_BSWP     0x0004  /* Byte Swap */
#define lncsr3_ACON     0x0002  /* ALE Control */
#define lncsr3_BCON     0x0001  /* Byte Control */


/* typedefs */

/* globals */

/* locals */

/* forward declarations */


#ifdef __cplusplus
}
#endif

#endif /* __INCln7990EndCommonh */
