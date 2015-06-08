/* nvr4101SIUSio.h - header file for nvr4101 SIU serial driver */

/* Copyright 1984-2002 Wind River Systems, Inc. */

/*
modification history
--------------------
01b,26apr02,dat  Adding cplusplus protection, SPR 75017
01a,11jun97,sru  written.
*/

#ifndef __INCnvr4101SIUSioh
#define __INCnvr4101SIUSioh

#include "sioLib.h"

#ifdef __cplusplus
extern "C" {
#endif

/* device and channel structures */

typedef struct
    {
    /* must be first */

    SIO_CHAN		sio;		/* standard SIO_CHAN element */

    /* callbacks */

    STATUS	        (*getTxChar) ();
    STATUS	        (*putRcvChar) ();
    void *	        getTxArg;
    void *	        putRcvArg;

    /* send and receive buffers */
    char *		pTxBuf;		/* transmit buffer */
    char *		pRxBuf;		/* receive buffer */
    UINT16 *		pTxWord;	/* last word of tx buffer */
    UINT16 *		pRxWord;	/* last word of rx buffer */

    /* state */
    int                 channelMode;    /* current mode (interrupt or poll) */
    int			txActive;	/* transmitter active state */
    int			baudRate;	/* for faster lookup */
    } NVR4101_SIU_CHAN;

/* definitions */

#define DMA_PAGE_SIZE	(2048)

/* function prototypes */

#if defined(__STDC__)

extern void 	nvr4101SIUDevInit	(NVR4101_SIU_CHAN *pChan);
extern void	nvr4101SIUInt		(NVR4101_SIU_CHAN *pChan);

#else   /* __STDC__ */

extern void 	nvr4101SIUDevInit	();
extern void	nvr4101SIUInt		();

#endif  /* __STDC__ */

#ifdef __cplusplus
}
#endif

#endif  /* __INCnvr4101SIUSioh */
