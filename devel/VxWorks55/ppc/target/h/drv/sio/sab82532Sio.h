/* sab82532Sio.h - Siemens 82532 DUART header file */

/*
modification history
--------------------
01a,25nov97,mem  written.
*/

#ifndef __INCsab82532Sioh 
#define __INCsab82532Sioh 

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _ASMLANGUAGE

#include "vxWorks.h"
#include "sioLib.h"

#endif	/* _ASMLANGUAGE */

/* Register offsets from base address (ASYNC mode only) */

#define SAB82532_RFIFO	0x00	/* r : rfifo 0 */
#define SAB82532_XFIFO	0x00	/*  w: xfifo 0 */
#define SAB82532_STAR	0x20	/* r : status register */
#define SAB82532_CMDR	0x20	/*  w: command register */
#define SAB82532_MODE	0x22	/* rw: mode register */
#define SAB82532_TIMR	0x23	/* rw: timer register */
#define SAB82532_XON	0x24	/* rw: XON character */
#define SAB82532_XOFF	0x25	/* rw: XOFF character */
#define SAB82532_TCR	0x26	/* rw: termination character register */
#define SAB82532_DAFO	0x27	/* rw: data format */
#define SAB82532_RFC	0x28	/* rw: rfifo control register */
#define SAB82532_RBCL	0x2a	/* r : receive byte count low */
#define SAB82532_XBCL	0x2a	/*  w: transmit byte count low */
#define SAB82532_RBCH	0x2b	/* r : receive byte count high */
#define SAB82532_XBCH	0x2b	/*  w: transmit byte count high */
#define SAB82532_CCR0	0x2c	/* rw: channel config register 0 */
#define SAB82532_CCR1	0x2d	/* rw: channel config register 1 */
#define SAB82532_CCR2	0x2e	/* rw: channel config register 2 */
#define SAB82532_CCR3	0x2f	/* rw: channel config register 3 */
#define SAB82532_TSAX	0x30	/*  w: time-slot assignment transmit */
#define SAB82532_TSAR	0x31	/*  w: time-slot assignment receive */
#define SAB82532_XCCR	0x32	/*  w: transmit channel capacity register */
#define SAB82532_RCCR	0x33	/*  w: receive channel capacity register */
#define SAB82532_VSTR	0x34	/* r : version status */
#define SAB82532_BGR	0x34	/*  w: baud rate generator register */
#define SAB82532_TIC	0x35	/*  w: transmit immediate character */
#define SAB82532_MXN	0x36	/*  w: mask XON character */
#define SAB82532_MXF	0x37	/*  w: mask XOFF character */
#define SAB82532_GIS	0x38	/* r : global interrupt status */
#define SAB82532_IVA	0x38	/*  w: interrupt vector address */
#define SAB82532_IPC	0x39	/* rw: interrupt port configuration */
#define SAB82532_ISR0	0x3a	/* r : interrupt status 0 */
#define SAB82532_IMR0	0x3a	/*  w: interrupt mask 0 */
#define SAB82532_ISR1	0x3b	/* r : interrupt status 1 */
#define SAB82532_IMR1	0x3b	/*  w: interrupt mask 1 */
#define SAB82532_PVR	0x3c	/* rw: port value register */
#define SAB82532_PIS	0x3d	/* r : port interrupt status */
#define SAB82532_PIM	0x3d	/*  w: port interrupt mask */
#define SAB82532_PCR	0x3e	/* rw: port configuration register */
#define SAB82532_CCR4	0x3f	/* rw: channel config register 4 */

#ifndef _ASMLANGUAGE

typedef struct SAB82532_CHAN
    {
    /* always goes first */

    SIO_DRV_FUNCS *	pDrvFuncs;	/* driver functions */

    /* callbacks */

    STATUS	(*getTxChar) ();
    STATUS	(*putRcvChar) ();
    void *	getTxArg;
    void *	putRcvArg;

    volatile UINT8 *	regs;		/* SAB82532 registers */

    UINT16	channelMode;		/* such as INT, POLL modes */
    int		baudRate;
    UINT32	xtal;			/* UART clock frequency */
    } SAB82532_CHAN;

typedef struct SAB82532_DUART
    {
    SAB82532_CHAN	channel[2];
    } SAB82532_DUART;

/* function declarations */

#if defined(__STDC__) || defined(__cplusplus)

extern void sab82532Int (SAB82532_DUART *);
extern void sab82532DevInit (SAB82532_DUART *);

#else

extern void sab82532Int ();
extern void sab82532DevInit ();

#endif  /* __STDC__ */

#endif	/* _ASMLANGUAGE */

#ifdef __cplusplus
}
#endif
 
#endif /* __INCsab82532Sioh */
