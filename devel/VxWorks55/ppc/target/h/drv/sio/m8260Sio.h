/* m8260Sio.h - Motorola MPC8260 SIO header file */

/* Copyright 1984-2002 Wind River Systems, Inc. */

/*
modification history
--------------------
01e,05jun02,pmr  SPR 78051: fixed typo
01d,09may02,gjc  Fixing SPR #75922.
01c,12sep99,ms_  move I/O Port definitions to m8260IOPort.h
01b,15jul99,ms_  fix to meet coding standards
01a,08mar99,ms_  adapted from h/drv/multi/ppc860Cpm.h
*/

/*
 * This file contains constants for an SIO using the Serial
 * Communications Controller (SCC) of the Communications Processor 
 * Module (CPM) of the Motorola MPC8260 PowerPC microcontroller.
 */

#ifndef __INCm8260Sioh
#define __INCm8260Sioh
#ifdef __cplusplus
extern "C" {
#endif

#ifndef _ASMLANGUAGE

#include "sioLib.h"

/* defines */

#define DEFAULT_BAUD 9600

#ifndef SCC_BUF
#define SCC_BUF VINT8           /* byte so that offset works */
#endif

typedef struct
    {

    /* must be first */

    SIO_DRV_FUNCS *pDrvFuncs;	/* driver functions */

    /* callbacks */

    STATUS (* getTxChar) ();		/* pointer to xmit function */
    STATUS (* putRcvChar) ();		/* pointer to recv function */
    void *      getTxArg;
    void *      putRcvArg;

    VINT16	channelMode; 		/* polled or interrupt driven */
    int		baudRate;
    int		sccNum;			/* SCC associated with this channel */

    VINT32	immrVal;		/* Internal Memory Map Register */
    char *	pBdBase;		/* Buffer Descriptor base */
    char *	rcvBufferAddr;		/* address of receive buffer */
    char *	txBufferAddr;		/* address of transmit buffer */

    char ch;

    VINT16 *    pRBASE;
    VINT16 *    pTBASE;

    } M8260_SCC_CHAN;

#define M8260_SCC_BD_SIZE 	8	/* size, in bytes, of a single BD */

#define M8260_SCC_RCV_BD_OFF	0	/* offset from BD base to receive BD */
#define M8260_SCC_TX_BD_OFF	M8260_SCC_BD_SIZE	

/* offset from BD base to transmit BD, since there is just one BD each */

#define M8260_SCC_BD_STAT_OFF	0	/* offset to status field */
#define M8260_SCC_BD_LEN_OFF	2	/* offset to data length field */
#define M8260_SCC_BD_ADDR_OFF	4	/* offset to address pointer field */

#define M8260_SCC_NEXT_SCC_OFF	0x20	/* offset between SCCs */

#define M8260_SCC_32_WR(addr, value) (* ((UINT32 *)(addr)) = ((UINT32) (value)))
 
#define M8260_SCC_16_WR(addr, value) (* ((UINT16 *)(addr)) = ((UINT16) (value)))
 
#define M8260_SCC_8_WR(addr, value) (* ((UINT8 *)(addr)) = ((UINT8) (value)))
 
#define M8260_SCC_32_RD(addr, value) ((value) = (* (UINT32 *) ((UINT32 *)(addr))))
 
#define M8260_SCC_16_RD(addr, value) ((value) = (* (UINT16 *) ((UINT16 *)(addr))))
 
#define M8260_SCC_8_RD(addr, value) ((value) = (* (UINT8 *) ((UINT8 *)(addr))))



#if defined(__STDC__)

IMPORT	void    m8260SioDevInit (M8260_SCC_CHAN *);
IMPORT  void    m8260SioInt     (M8260_SCC_CHAN *);
IMPORT  UINT32  vxImmrGet  	(void);

#else   /* __STDC__ */

IMPORT	void    m8260SioDevInit ();
IMPORT  void    m8260SioInt ();
IMPORT  UINT32  vxImmrGet ();

#endif  /* __STDC__ */

#endif  /* _ASMLANGUAGE */

#ifdef __cplusplus
}
#endif

#endif /* __INCm8260Sioh */
