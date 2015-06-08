/* unixSio.h - header file for unix simulator serial driver */

/* Copyright 1984-2001 Wind River Systems, Inc. */

/*
modification history
--------------------
01b,30oct01,jmp  moved from target/h/drv/sio to solaris bsp.
01a,28mar97,kab  written
*/

#ifndef __INCunixSioh
#define __INCunixSioh

#include "sioLib.h"

/* device and channel structures */

typedef struct
    {
    /* always goes first */
    SIO_CHAN		sio;	/* driver functions */

    /* callbacks */

    STATUS	        (*getTxChar) ();
    STATUS	        (*putRcvChar) ();
    void *	        getTxArg;
    void *	        putRcvArg;

    int			u_fd;		/* unix file descriptor */
    int			u_pid;		/* unix process */
    
    /* misc */

    BOOL		created;
    int                 mode;		/* current mode (interrupt or poll) */
    int                 baudRate;	/* input clock frequency */
    } UNIX_CHAN;

/* definitions */

	/* master control register definitions */

#define UNIX_RESET_CHIP	0x1

	/* channel control register definitions */

#define UNIX_RESET_TX	0x1	/* reset the transmitter */
#define UNIX_RESET_ERR	0x2	/* reset error condition */
#define UNIX_RESET_INT	0x4	/* acknoledge the interrupt */
#define	UNIX_INT_ENABLE	0x8	/* enable interrupts */

	/* channel status register definitions */

#define UNIX_TX_READY	0x1	/* txmitter ready for another char */
#define UNIX_RX_AVAIL	0x2	/* character has arrived */

/* function prototypes */

#if defined(__STDC__)

extern void 	unixDevInit	(UNIX_CHAN *pChan); 
extern void 	unixDevInit2	(UNIX_CHAN *pChan); 
extern void	unixIntRcv	(UNIX_CHAN *pChan);
extern void	unixIntTx	(UNIX_CHAN *pChan);
extern void	unixIntErr	(UNIX_CHAN *pChan);

#else   /* __STDC__ */

extern void 	unixDevInit	();
extern void	unixIntRcv	();
extern void	unixIntTx	();
extern void	unixIntErr	();

#endif  /* __STDC__ */

#endif  /* __INCunixSioh */
