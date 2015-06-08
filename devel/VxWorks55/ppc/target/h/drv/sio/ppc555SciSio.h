/* ppc555SciSio.h - header file for ppc555Sci serial driver */

/* Copyright 1984-1997 Wind River Systems, Inc. */

/*
modification history
--------------------
01a,22mar99,zl  written based on templateSio.h, m68332Sio.h and m68332.h
*/

#ifndef __INCppc555SciSioh
#define __INCppc555SciSioh

#ifdef __cplusplus
extern "C" {
#endif

#ifndef	_ASMLANGUAGE

#include "sioLib.h"
#include "drv/multi/ppc555Siu.h"

/* 
 * Additional hardware options to the ones in sioLib.h
 */

#define QUEUED	0x100

/* 
 * QSM - Register definitions for the Queued Serial Module (QSM) 
 */

#define PPC555_QSM_MCR(base)		((VUINT16 *) (base + 0x305000))
#define PPC555_QSM_TEST(base)		((VUINT16 *) (base + 0x305002))
#define PPC555_QSM_DSCI_ILR(base)	((VUINT16 *) (base + 0x305004))
#define PPC555_QSM_QSPI_ILR(base)	((VUINT16 *) (base + 0x305006))
#define PPC555_QSM_SCI1_CR0(base)	((VUINT16 *) (base + 0x305008))
#define PPC555_QSM_SCI1_CR1(base)	((VUINT16 *) (base + 0x30500a))
#define PPC555_QSM_SCI1_SR(base)	((VUINT16 *) (base + 0x30500c))
#define PPC555_QSM_SCI1_DR(base)	((VUINT16 *) (base + 0x30500e))
#define PPC555_QSM_QPDR(base)		((VUINT16 *) (base + 0x305014))
#define PPC555_QSM_QPAR(base)		((VUINT8  *) (base + 0x305016))
#define PPC555_QSM_QDDR(base)		((VUINT8  *) (base + 0x305017))
#define PPC555_QSM_SPCR0(base)		((VUINT16 *) (base + 0x305018))
#define PPC555_QSM_SPCR1(base)		((VUINT16 *) (base + 0x30501a))
#define PPC555_QSM_SPCR2(base)		((VUINT16 *) (base + 0x30501c))
#define PPC555_QSM_SPCR3(base)		((VUINT8  *) (base + 0x30501e))
#define PPC555_QSM_SPSR(base)		((VUINT8  *) (base + 0x30501f))
#define PPC555_QSM_SCI2_CR0(base)	((VUINT16 *) (base + 0x305020))
#define PPC555_QSM_SCI2_CR1(base)	((VUINT16 *) (base + 0x305022))
#define PPC555_QSM_SCI2_SR(base)	((VUINT16 *) (base + 0x305024))
#define PPC555_QSM_SCI2_DR(base)	((VUINT16 *) (base + 0x305026))
#define PPC555_QSM_QSCI1_CR(base)	((VUINT16 *) (base + 0x305028))
#define PPC555_QSM_QSCI1_SR(base)	((VUINT16 *) (base + 0x30502a))
#define PPC555_QSM_QSCI1_SCTQ(base)	((VUINT16 *) (base + 0x30502c))
#define PPC555_QSM_QSCI1_SCRQ(base)	((VUINT16 *) (base + 0x30504c))


/* QSM_DSCI_ILR -  DSCI Interrupt Level Register */

#define QSM_QILR_SCI_MSK	0x8f00	/* SCI interrupt level mask */
#define QSM_QILR_SCI_SHIFT	8	/* SCI interrupt level shift */

/* QSM_SCCR0 - Control Register 0 */

#define QSM_SCCR0_OTHR		0x8000	/* Select baud rate source clk */
#define QSM_SCCR0_LNKBD		0x4000	/* Link baud */
#define QSM_SCCR0_SCBR_MSK	0x1FFF	/* Baud rate mask */


/* QSM_SCCR1 - Control Register 1 */

#define QSM_SCCR1_SBK		0x0001	/* send break */
#define QSM_SCCR1_RWU		0x0002	/* receiver wakeup enable */
#define QSM_SCCR1_RE		0x0004	/* receiver enable */
#define QSM_SCCR1_TE		0x0008	/* transmitter enable */
#define QSM_SCCR1_ILIE		0x0010	/* idle-line interrupt enable */
#define QSM_SCCR1_RIE		0x0020	/* receiver interrupt enable */
#define QSM_SCCR1_TCIE		0x0040	/* transmit complete interrupt enable */
#define QSM_SCCR1_TIE		0x0080	/* transmit interrupt enable */
#define QSM_SCCR1_WAKE		0x0100	/* wakeup by address mark */
#define QSM_SCCR1_M		0x0200	/* mode select: 0= 8 data bits, 1 = 9 */
#define QSM_SCCR1_PE		0x0400	/* parity enable */
#define QSM_SCCR1_PT		0x0800	/* parity type: 1=Odd, 0=Even */
#define QSM_SCCR1_ILT		0x1000	/* idle line detect type */
#define QSM_SCCR1_WOMS		0x2000	/* wired or mode for SCI Pins */
#define QSM_SCCR1_LOOPS		0x4000	/* test SCI operation */

/* QSM_SCSR - Status Register */

#define QSM_SCSR_PF		0x0001	/* parity error flag */
#define QSM_SCSR_FE		0x0002	/* framing error flag */
#define QSM_SCSR_NF		0x0004	/* noise error flag */
#define QSM_SCSR_OR		0x0008	/* overrun error flag */
#define QSM_SCSR_IDLE		0x0010	/* idle-line detected flag */
#define QSM_SCSR_RAF		0x0020	/* receiver active flag */
#define QSM_SCSR_RDRF		0x0040	/* receive data register full flag */
#define QSM_SCSR_TC		0x0080	/* transmit complete */
#define QSM_SCSR_TDRE		0x0100	/* transmit date register empty flag */

/* QSM_QSCCR - Queued SCI Control Register */

#define QSM_QSCCR_QTPNT_MSK	0xF000	/* Queue transmit pointer */
#define QSM_QSCCR_QTHFI		0x0800	/* Rcv queue top-half full intr. */
#define QSM_QSCCR_QBHFI		0x0400	/* Rcv queue bottom-half full intr. */
#define QSM_QSCCR_QTHEI		0x0200	/* Xmt queue top-half empty intr. */
#define QSM_QSCCR_QBHEI		0x0100	/* Xmt queue bottom-half empty intr. */
#define QSM_QSCCR_QTE		0x0040	/* Queue transmit enable */
#define QSM_QSCCR_QRE		0x0020	/* Queue receive enable */
#define QSM_QSCCR_QTWE		0x0010	/* Queue transmit wrap enable */
#define QSM_QSCCR_QTSZ_MSK	0x000F	/* Queue transfer size */

/* QSM_SCSR - Queued SCI Status Register */

#define QSM_QSCSR_QOR		0x1000	/* Receiver queue overrun error */
#define QSM_QSCSR_QTHF		0x0800	/* Receiver queue top-half full */
#define QSM_QSCSR_QBHF		0x0400	/* Receiver queue bottom-half full */
#define QSM_QSCSR_QTHE		0x0200	/* Xmitter queue top-half empty */
#define QSM_QSCSR_QBHE		0x0100	/* Xmitter queue bottom-half empty */
#define QSM_QSCSR_QRPNT_MSK	0x00F0	/* Queue receive pointer */
#define QSM_QSCSR_QPEND_MSK	0x000F	/* Queue pending */


/* 
 * device and channel structures 
 */

typedef struct
    {
    /* must be first */

    SIO_CHAN		sio;		/* standard SIO_CHAN element */

    /* callbacks */

    STATUS	        (*getTxChar) ();
    STATUS	        (*putRcvChar) ();
    void *	        getTxArg;
    void *	        putRcvArg;

    /* register addresses */

    volatile UINT16 *   sccr0;		/* control register 0 */
    volatile UINT16 *   sccr1;		/* control register 1 */
    volatile UINT16 *   scsr;		/* status register */
    volatile UINT16 *   scdr;		/* data register */
    volatile UINT16 *	scilr;		/* interrupt level register */
    volatile UINT16 *	qsccr;		/* queued control register */
    volatile UINT16 *	qscsr;		/* queued status register */
    volatile UINT16 *	scrq;		/* receive queue */
    volatile UINT16 *	sctq;		/* transmit queue */

    /* misc */

    int			intLevel;
    int                 baudRate;        
    int			clockRate;	/* CPU clock frequency (Hz) */
    int                 mode;		/* current mode (interrupt or poll) */
    int			options;	/* Hardware options */
    } PPC555SCI_CHAN;



/* 
 * function prototypes 
 */

IMPORT void 	ppc555SciDevInit (PPC555SCI_CHAN *pChan); 
IMPORT void 	ppc555SciDevInit2(PPC555SCI_CHAN *pChan); 
IMPORT void	ppc555SciInt	 (PPC555SCI_CHAN *pChan);


#endif  /* _ASMLANGUAGE */


#endif  /* __STDC__ */
#ifdef  __cplusplus
}

#endif  /* __INCppc555SciSioh */
