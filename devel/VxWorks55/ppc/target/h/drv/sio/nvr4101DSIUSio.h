/* nvr4101DSIUSio.h - header file for nvr4101 DSIU serial driver */

/* Copyright 1984-2002 Wind River Systems, Inc. */

/*
modification history
--------------------
01b,26apr02,dat  Adding cplusplus protection, SPR 75017
01a,23jun97,sru  written.
*/

#ifndef __INCnvr4101DSIUSioh
#define __INCnvr4101DSIUSioh

#include "sioLib.h"

#ifdef __cplusplus
extern "C" {
#endif

/* definitions */

#define VR4101_DSIU_PARITY_ODD		2
#define VR4101_DSIU_PARITY_EVEN		3
#define VR4101_DSIU_PARITY_ZERO		1

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

    /* configuration (set before calling DevInit) */
    int			parityStyle;	/* only the listed styles are allowed */
    int			wordLength;	/* only 7 and 8 are supported */
  
    /* state */
    int                 channelMode;    /* current mode (interrupt or poll) */
    int			txActive;	/* transmitter active state */
    int			baudRate;	/* for faster lookup */
    } NVR4101_DSIU_CHAN;

/* function prototypes */

#if defined(__STDC__)

extern void 	nvr4101DSIUDevInit	(NVR4101_DSIU_CHAN *pChan);
extern void	nvr4101DSIUInt		(NVR4101_DSIU_CHAN *pChan);

#else   /* __STDC__ */

extern void 	nvr4101DSIUDevInit	();
extern void	nvr4101DSIUInt		();

#endif  /* __STDC__ */

#ifdef __cplusplus
}
#endif

#endif  /* __INCnvr4101DSIUSioh */
