/* sa1100Sio.h - Digital Semiconductor SA-1100 UART header file */

/* Copyright 1998-2000 Wind River Systems, Inc. */

/*
modification history
--------------------
01b,12apr00,jpd  added include of sioLib.h to work with project builds.
01a,22jan98,jpd  written from ambaSio.h, version 01b.
*/

#ifndef __INCsa1100Sioh 
#define __INCsa1100Sioh 

#ifdef __cplusplus
extern "C" {
#endif

/* Register description of SA-1100 UART */

#ifndef _ASMLANGUAGE

#include "vxWorks.h"
#include "sioLib.h"

/* Register offsets from base address */

#define UTCR0		0x00		/* UART Control Register 0 */
#define UTCR1		0x04		/* UART Control Register 1 */
#define UTCR2		0x08		/* UART Control Register 2 */
#define UTCR3		0x0C		/* UART Control Register 3 */
#define UTDR		0x14		/* UART Control Register */
#define	UTSR0		0x1C		/* UART Status Register 0 */
#define	UTSR1		0x20		/* UART Status Register 1 */

/* bit definitions within UTCR0 register */

#define PARITY_NONE	0x00		/* set no parity */
#define ONE_STOP	0x00		/* set one stop bit */
#define WORD_LEN_8	(1 << 3)	/* set 8 data bits */


/* bit definitions within UTCR3 register */

#define UTCR3_RXE	0x01		/* Receiver Enable */
#define UTCR3_TXE	0x02		/* Transmitter Enable */
#define UTCR3_RIM	0x08		/* Rx Interrupt Mask */
#define UTCR3_TIM	0x10		/* Tx Interrupt Mask */


/* bit definitions within UTSR0 register */

#define UTSR0_TFS	(1 << 0)	/* Tx FIFO Service Request */
#define UTSR0_RFS	(1 << 1)	/* Rx FIFO Service Request */
#define UTSR0_RID	(1 << 2)	/* Receiver Idle */
#define UTSR0_RBB	(1 << 3)	/* Rx beginning of Break */
#define UTSR0_REB	(1 << 4)	/* Rx end of break */
#define UTSR0_EIF	(1 << 5)	/* Error in FIFO */


/* bit definitions within UTSR1 register */

#define UTSR1_RNE	(1 << 1)	/* Receiver FIFO Not Empty */
#define UTSR1_TNF	(1 << 2)	/* Transmitter FIFO Not Full */


typedef struct SA1100_CHAN
    {
    /* must be first */

    SIO_CHAN	sio;		/* standard SIO_CHAN element */

    /* callbacks */

    STATUS	(*getTxChar) ();  /* installed Tx callback routine */
    STATUS	(*putRcvChar) (); /* installed Rx callback routine */
    void *	getTxArg;	/* argument to Tx callback routine */
    void *	putRcvArg;	/* argument to Rx callback routine */

    UINT32 *	regs;		/* UART registers */
    UINT8 	level;		/* Interrupt level for this device */

    UINT32	channelMode;	/* such as INT, POLL modes */
    int		baudRate;	/* the current baud rate */
    UINT32	xtal;		/* UART clock frequency */     

    } SA1100_CHAN;


/* function declarations */

extern void sa1100Int (SA1100_CHAN *pChan);
extern void sa1100DevInit (SA1100_CHAN *pChan);

#endif	/* _ASMLANGUAGE */

#ifdef __cplusplus
}
#endif
 
#endif /* __INCsa1100Sioh */
