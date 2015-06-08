/* ambaSio.h - ARM AMBA UART header file */

/* Copyright 1997-2000 Wind River Systems, Inc. */

/*
modification history
--------------------
01c,12apr00,jpd  added include of sioLib.h to work with project builds.
01b,19nov97,jpd  updated comments/documentation in line with latest standards.
01a,04sep97,jpd  written from st16552Sio.h, version 01c.
*/

#ifndef __INCambaSioh 
#define __INCambaSioh 

#ifdef __cplusplus
extern "C" {
#endif

/* Register description OF ARM AMBA UART */

#ifndef _ASMLANGUAGE

#include "vxWorks.h"
#include "sioLib.h"


/* Register offsets from base address */

#define	UARTDR		0x00		/* UART data register (R/W) */
#define	RXSTAT		0x04		/* Rx data status register (R/O) */
#define	UMSEOI		0x04		/* Clr modem status changed int (W/O) */
#define	H_UBRLCR	0x08		/* } High middle and low bytes (R/W) */
#define	M_UBRLCR	0x0C		/* } of bit rate and line (W/O) */
#define	L_UBRLCR	0x10		/* } register (W/O) */
#define	UARTCON		0x14		/* control register (R/W) */
#define	UARTFLG		0x18		/* FIFO status register (R/O) */

/* bit definitions within H_UBRLCR register */

#define PARITY_NONE	0x00		/* set no parity */
#define ONE_STOP	0x00		/* set one stop bit */
#define FIFO_ENABLE	0x10		/* Enable both FIFOs */
#define WORD_LEN_5	(0x00 << 5)	/* Set UART word lengths */
#define WORD_LEN_6	(0x01 << 5)
#define WORD_LEN_7	(0x02 << 5)
#define WORD_LEN_8	(0x03 << 5)


/* bit definitions within UARTCON register */

#define UART_ENABLE	0x01		/* Enable the UART */


/* bit definitions within UARTFLG register */

#define FLG_UTXFF	(0x01 << 5)	/* UART Tx FIFO Full */
#define FLG_URXFE	(0x01 << 4)	/* UART Rx FIFO Empty */


typedef struct AMBA_CHAN
    {
    /* must be first */

    SIO_CHAN	sio;		/* standard SIO_CHAN element */

    /* callbacks */

    STATUS	(*getTxChar) ();  /* installed Tx callback routine */
    STATUS	(*putRcvChar) (); /* installed Rx callback routine */
    void *	getTxArg;	/* argument to Tx callback routine */
    void *	putRcvArg;	/* argument to Rx callback routine */

    UINT32 *	regs;		/* AMBA registers */
    UINT8 	levelRx;	/* Rx Interrupt level for this device */
    UINT8 	levelTx;	/* Tx Interrupt level for this device */

    UINT32	channelMode;	/* such as INT, POLL modes */
    int		baudRate;	/* the current baud rate */
    UINT32	xtal;		/* UART clock frequency */     

    } AMBA_CHAN;


/* function declarations */

extern void ambaIntTx (AMBA_CHAN *pChan);
extern void ambaIntRx (AMBA_CHAN *pChan);
extern void ambaDevInit (AMBA_CHAN *pChan);

#endif	/* _ASMLANGUAGE */

#ifdef __cplusplus
}
#endif
 
#endif /* __INCambaSioh */
