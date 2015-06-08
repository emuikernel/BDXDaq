/* m68302Sio.h - header file for m68302Sio serial driver */

/* Copyright 1984-1996 Wind River Systems, Inc. */

/*
modification history
--------------------
01b,22may96,dat added intEnalbe to structure
01a,16apr96,dat  written.
*/

/*
DESCRIPTION:

INCLUDE FILES:
sioLib.h  drv/multi/m68302.h
*/

#ifndef __INCm68302Sioh
#define __INCm68302Sioh

#ifdef __cplusplus
extern "C" {
#endif

#ifndef	_ASMLANGUAGE

#include "sioLib.h"

#include "drv/multi/m68302.h"

/* per channel structure */

typedef struct	/* M68302_CHAN */
    {
    /* always goes first */

    SIO_CHAN		sio;

    /* callbacks */

    FUNCPTR		getTxChar;
    FUNCPTR		putRcvChar;
    void *		getTxArg;
    void *		putRcvArg;

    /* Must be initialized BEFORE m68302SioInit */
    SCC *		pScc;		/* serial comm ctlr */
    SCC_REG *		pSccReg;	/* serial comm ctlr registers */
    SCC_PARAM *		pParam;
    PROT_UART *		pProt;
    UINT16    		intAck;		/* interrupt acknowledge mask */
    int			channel;	/* channel #, or -1 to disable */

    /* Initialized by m68302SioInit */
    int			options;	/* SIO_HW_OPTS */
    int                 mode;           /* current mode (interrupt or poll) */
    int                 baud;		/* baud rate */
    int       		rxBdNext;	/* next rcv buff desc to fill */
    struct m68302Cp *	pCp;		/* back pointer to M68302_CP */

    } M68302_CHAN;

/* per device structure */

typedef struct m68302Cp /* M68302_CP */
    {
    /* Initialized BEFORE m68302SioInit */
    volatile char *	pCr;		/* command reset register */
    volatile UINT16 *	pSiMask;		/* HELP register */
    volatile UINT16 *	pSiMode;		/* HELP register */
    volatile UINT16 *	pImr;		/* interrupt mask register */
    volatile UINT16 *	pIsr;		/* interrupt status register */
    UINT16		imrMask; 	/* imr value to unmask channels */
    char *		buffer; 	/* buffer area, at least BUF_SIZE */
    int      		clockRate;	/* CPU clock frequency (Hz) */
    BOOL		intEnable;	/* allow interrupt mode flag */

    /* See Above for Channel initialization requirements */

    M68302_CHAN		portA;
    M68302_CHAN		portB;
    M68302_CHAN		portC;

    } M68302_CP;

/* function prototypes */

#if defined(__STDC__)

IMPORT VOID 	m68302SioInit	(M68302_CP *pCp); 
IMPORT VOID 	m68302SioInit2	(M68302_CP *pCp); 
IMPORT VOID	m68302SioInt	(M68302_CHAN *pChan);

#else   /* __STDC__ */

IMPORT VOID 	m68302SioInit	();
IMPORT VOID 	m68302SioInit2	(); 
IMPORT VOID	m68302SioInt	();

#endif  /* __STDC__ */

#endif	/* _ASMLANGUAGE */

#ifdef __cplusplus
}
#endif

#endif  /* __INCm68302Sioh */
