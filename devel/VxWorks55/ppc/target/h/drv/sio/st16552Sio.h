/* st16552Sio.h - Startech (Exar) 16552 DUART header file */

/* Copyright 1984-2000 Wind River Systems, Inc. */

/*
modification history
--------------------
01c,12apr00,jpd  added include of sioLib.h to work with project builds.
01d,01dec97,jpd  updated to latest standards.
01c,04apr97,jpd  added ST16552_MUX struct for mutltiplexing interrupts.
01b,01apr97,jpd  changed UINT16s to UINT32s for efficiency on ARM.
01a,18jul96,jpd  written from ns16552Sio.h, version 01a.
*/

#ifndef __INCst16552Sioh
#define __INCst16552Sioh

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Copyright (c) 1990,1991 Intel Corporation
 *
 * Intel hereby grants you permission to copy, modify, and
 * distribute this software and its documentation.  Intel grants
 * this permission provided that the above copyright notice
 * appears in all copies and that both the copyright notice and
 * this permission notice appear in supporting documentation.  In
 * addition, Intel grants this permission provided that you
 * prominently mark as not part of the original any modifications
 * made to this software or documentation, and that the name of
 * Intel Corporation not be used in advertising or publicity
 * pertaining to distribution of the software or the documentation
 * without specific, written prior permission.
 *
 * Intel Corporation does not warrant, guarantee or make any
 * representations regarding the use of, or the results of the use
 * of, the software and documentation in terms of correctness,
 * accuracy, reliability, currentness, or otherwise; and you rely
 * on the software, documentation and results solely at your own risk.
 */


/* REGISTER DESCRIPTION OF STARTECH 16552 DUART */

#ifndef _ASMLANGUAGE

#include "vxWorks.h"
#include "sioLib.h"


/* Register offsets from base address */

#define RBR	0x00	/* Receive Holding Register (R/O) */
#define THR	0x00	/* Transmit Holding Register (W/O)*/
#define DLL	0x00	/* Divisor Latch Low */
#define IER	0x01	/* Interrupt Enable Register */
#define DLM	0x01	/* Divisor Latch Middle */
#define IIR	0x02	/* Interupt identification Register (R/O) */
#define FCR	0x02	/* FIFO Control register (W/O) */
#define LCR	0x03	/* Line Control Register */
#define MCR	0x04	/* Modem Control Register */
#define LSR	0x05	/* Line Status register */
#define MSR	0x06	/* Modem Status Register */
#define SCR	0x07	/* Scratchpad Register */

#define BAUD_LO(baud)  ((XTAL/(16*baud)) & 0xFF)
#define BAUD_HI(baud)  (((XTAL/(16*baud)) & 0xFF00) >> 8)

/* Line Control Register values */

#define CHAR_LEN_5	0x00
#define CHAR_LEN_6	0x01
#define CHAR_LEN_7	0x02
#define CHAR_LEN_8	0x03
#define LCR_STB		0x04	/* Stop bit control */
#define ONE_STOP	0x00	/* One stop bit! */
#define LCR_PEN		0x08	/* Parity Enable */
#define PARITY_NONE	0x00
#define LCR_EPS		0x10	/* Even Parity Select */
#define LCR_SP		0x20	/* Force Parity */
#define LCR_SBRK	0x40	/* Start Break */
#define LCR_DLAB	0x80	/* Divisor Latch Access Bit */
#define DLAB		LCR_DLAB

/* Line Status Register */

#define LSR_DR		0x01	/* Data Ready */
#define RxCHAR_AVAIL	LSR_DR
#define LSR_OE		0x02	/* Overrun Error */
#define LSR_PE		0x04	/* Parity Error */
#define LSR_FE		0x08	/* Framing Error */
#define LSR_BI		0x10	/* Received Break Signal */
#define LSR_THRE	0x20	/* Transmit Holding Register Empty */
#define LSR_TEMT	0x40	/* THR and FIFO empty */
#define LSR_FERR	0x80	/* Parity, Framing error or break in FIFO */

/* Interrupt Identification Register */

#define IIR_IP		0x01		/* Interrupt Pending */
#define IIR_ID		0x0E		/* Interrupt source mask */
#define IIR_RLS		0x06		/* Rx Line Status Int */
#define Rx_INT		IIR_RLS
#define IIR_RDA		0x04		/* Rx Data Available */
#define RxFIFO_INT	IIR_RDA
#define IIR_THRE	0x02		/* THR Empty */
#define TxFIFO_INT	IIR_THRE
#define IIR_MSTAT	0x00		/* Modem Status Register Int */
#define IIR_TIMEOUT	0x0C		/* Rx Data Timeout */

/* Interrupt Enable Register */

#define IER_ERDAI	0x01		/* Enable Rx Data Available Int */
#define RxFIFO_BIT	IER_ERDAI
#define IER_ETHREI	0x02		/* Enable THR Empty Int */
#define TxFIFO_BIT	IER_ETHREI
#define IER_ELSI	0x04		/* Enable Line Status Int */
#define Rx_BIT		IER_ELSI
#define IER_EMSI	0x08		/* Enable Modem Status Int */

/* Modem Control Register */

#define MCR_DTR		0x01		/* state of DTR output */
#define DTR		MCR_DTR
#define MCR_RTS		0x02		/* state of RTS output */
#define MCR_OUT1	0x04		/* UNUSED in ST16552 */
#define MCR_INT		0x08		/* Int Mode */
#define MCR_LOOP	0x10		/* Enable Loopback mode */

/* Modem Status Register */

#define MSR_DCTS	0x01		/* change in CTS */
#define MSR_DDSR	0x02		/* change in DSR */
#define MSR_TERI	0x04		/* change in RI */
#define MSR_DDCD	0x08		/* change in DCD */
#define MSR_CTS		0x10		/* state of CTS input */
#define MSR_DSR		0x20		/* state of DSR input */
#define MSR_RI		0x40		/* state of RI input */
#define MSR_DCD		0x80		/* state of DCD input */

/* FIFO Control Register */

#define FCR_EN		0x01		/* FIFO Enable */
#define FIFO_ENABLE	FCR_EN
#define FCR_RXCLR	0x02		/* Rx FIFO Clear */
#define RxCLEAR		FCR_RXCLR
#define FCR_TXCLR	0x04		/* Tx FIFO Clear */
#define TxCLEAR		FCR_TXCLR
#define FCR_DMA		0x08		/* FIFO Mode Control */
#define FCR_RXTRIG_L	0x40		/* FIFO Trigger level Low */
#define FCR_RXTRIG_H	0x80		/* FIFO Trigger level High */


typedef struct st16552_chan		/* ST16552_CHAN */
    {
    /* must be first */

    SIO_CHAN	sio;			/* standard SIO_CHAN element */

    /* callbacks */

    STATUS	(*getTxChar) ();	/* installed Tx callback routine */
    STATUS	(*putRcvChar) ();	/* installed Rx callback routine */
    void *	getTxArg;		/* argument to Tx callback routine */
    void *	putRcvArg;		/* argument to Rx callback routine */

    UINT8 *	regs;			/* ST16552 registers */
    UINT8	level;			/* Interrupt level for this device */
    UINT8	ier;			/* copy of IER */
    UINT8	lcr;			/* copy of LCR */

    UINT32	channelMode;		/* such as INT, POLL modes */
    UINT32	regDelta;		/* register address spacing */
    int		baudRate;		/* the current baud rate */
    UINT32	xtal;			/* UART clock frequency */

    } ST16552_CHAN;


/* structure used as parameter to multiplexed interrupt handler */

typedef struct st16552_mux		/* ST16552_MUX */
    {
    int			nextChan;	/* next channel to examine on int */
    ST16552_CHAN *	pChan;		/* array of ST16552_CHAN structs */
    } ST16552_MUX;


/* function declarations */

extern void st16552Int (ST16552_CHAN *);
extern void st16552MuxInt(ST16552_MUX *);
extern void st16552DevInit (ST16552_CHAN *);

#endif	/* _ASMLANGUAGE */

#ifdef __cplusplus
}
#endif

#endif /* __INCst16552Sioh */
