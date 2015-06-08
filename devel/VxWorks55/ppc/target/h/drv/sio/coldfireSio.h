/* coldfireSio.h - Motorola ColdFire internal UART header file */

/* Copyright 1984-1998 Wind River Systems, Inc. */

/*
modification history
--------------------
01b,09jul98,gls  Adapted to WRS coding conventions
01a,01may98,mem	 created, based on m5204Sio.h.
*/

/*
This file contains constants and defines for the UART contained in several
of the Motorola ColdFire ports.
*/

#ifndef __INCcoldfireSioh
#define __INCcoldfireSioh

#ifdef __cplusplus
extern "C" {
#endif

#ifndef	_ASMLANGUAGE

#include "sioLib.h"

typedef struct		
    {
    SIO_DRV_FUNCS  * pDrvFuncs;	/* Driver functions */
				/* CallBacks */
    STATUS   (*getTxChar)();
    STATUS   (*putRcvChar)();
    void *   getTxArg;
    void *   putRcvArg;
    
    UINT		clkRate;	/* system clock rate */
    UINT16		mode;		/* SIO_MODE */
    unsigned char	intVec;
    int			options;
    int			intEnable;
    UINT		baudRate;
    UCHAR		oprCopy;
    UCHAR		acrCopy;
    UCHAR		imrCopy;

    volatile UCHAR	*mr;	/* UART mode register */
    volatile UCHAR	*sr;	/* UART status register */
    volatile UCHAR	*csr;	/* UART clock select register */
    volatile UCHAR	*cr;	/* UART command register */
    volatile UCHAR	*rb;	/* UART receive buffer register */
    volatile UCHAR	*tb;	/* UART transmit buffer register */
    volatile UCHAR	*ipcr;	/* UART input port change register */
    volatile UCHAR	*acr;	/* UART auxiliary control register */
    volatile UCHAR	*isr;	/* UART interrupt status register */
    volatile UCHAR	*imr;	/* UART interrupt mask register */
    volatile UCHAR	*bg1;	/* UART baud generator prescale register 1 */
    volatile UCHAR	*bg2;	/* UART baud generator prescale register 2 */
    volatile UCHAR	*ivr;	/* UART interrupt vector register */
    volatile UCHAR	*ip;	/* UART input port register */
    volatile UCHAR	*op1;	/* UART output port set cmd */
    volatile UCHAR	*op2;	/* UART output port reset cmd */
    } COLDFIRE_CHAN;

#endif	/* _ASMLANGUAGE */

/* register bit definitions */

/* MR1 - mode register 1 */

#define COLDFIRE_UART_MR1_BC0	(1 << 0)	/* bits per char 0 */
#define COLDFIRE_UART_MR1_BC1	(1 << 1)	/* bits per char 1 */
#define COLDFIRE_UART_MR1_PT	(1 << 2)	/* parity type */
#define COLDFIRE_UART_MR1_PM0	(1 << 3)	/* parity mode 0 */
#define COLDFIRE_UART_MR1_PM1	(1 << 4)	/* parity mode 1 */
#define COLDFIRE_UART_MR1_ERR	(1 << 5)	/* error mode */
#define COLDFIRE_UART_MR1_RX_IRQ (1 << 6)       /* receiver interrupt select */
#define COLDFIRE_UART_MR1_RX_RTS (1 << 7)	/* Rx RTS control */

/* Some common modes */

#define COLDFIRE_UART_MR1_BITS_CHAR_5	0
#define COLDFIRE_UART_MR1_BITS_CHAR_6	COLDFIRE_UART_MR1_BC0
#define COLDFIRE_UART_MR1_BITS_CHAR_7	COLDFIRE_UART_MR1_BC1
#define COLDFIRE_UART_MR1_BITS_CHAR_8	(COLDFIRE_UART_MR1_BC0 \
					 | COLDFIRE_UART_MR1_BC1)
#define COLDFIRE_UART_MR1_NO_PARITY	(COLDFIRE_UART_MR1_PM1)
#define COLDFIRE_UART_MR1_PAR_MODE_MULTI	(COLDFIRE_UART_MR1_PM0 \
					 | COLDFIRE_UART_MR1_PM1)
#define COLDFIRE_UART_MR1_EVEN_PARITY	0
#define COLDFIRE_UART_MR1_ODD_PARITY	COLDFIRE_UART_MR1_PT

/* MR2 - mode register 2 */

#define COLDFIRE_UART_MR2_SB0	(1 << 0)	/* stop bit control 0 */
#define COLDFIRE_UART_MR2_SB1	(1 << 1)	/* stop bit control 1 */
#define COLDFIRE_UART_MR2_SB2	(1 << 2)	/* stop bit control 2 */
#define COLDFIRE_UART_MR2_SB3	(1 << 3)	/* stop bit control 3 */
#define COLDFIRE_UART_MR2_TX_CTS	(1 << 4)	/* Tx CTS control */
#define COLDFIRE_UART_MR2_TX_RTS	(1 << 5)	/* Tx RTS control */
#define COLDFIRE_UART_MR2_CM0	(1 << 6)	/* channel mode 0 */
#define COLDFIRE_UART_MR2_CM1	(1 << 7)	/* channel mode 1 */

/* Some common bit lengths */

#define COLDFIRE_UART_MR2_STOP_BITS_1	(COLDFIRE_UART_MR2_SB0 \
					 | COLDFIRE_UART_MR2_SB1 \
					 | COLDFIRE_UART_MR2_SB2)
#define COLDFIRE_UART_MR2_STOP_BITS_2	(COLDFIRE_UART_MR2_SB0 \
					 | COLDFIRE_UART_MR2_SB1 \
					 | COLDFIRE_UART_MR2_SB2 \
					 | COLDFIRE_UART_MR2_SB3)

/* SR - status register */

#define COLDFIRE_UART_SR_RXRDY	(1 << 0)	/* Rx ready */
#define COLDFIRE_UART_SR_FFULL	(1 << 1)	/* FIFO full */
#define COLDFIRE_UART_SR_TXRDY	(1 << 2)	/* Tx ready */
#define COLDFIRE_UART_SR_TXEMP	(1 << 3)	/* Tx empty */
#define COLDFIRE_UART_SR_OE	(1 << 4)	/* overrun error */
#define COLDFIRE_UART_SR_PE	(1 << 5)	/* parity error */
#define COLDFIRE_UART_SR_FE	(1 << 6)	/* framing error */
#define COLDFIRE_UART_SR_RB	(1 << 7)	/* received break */

/* CSR - clock select register */

#define COLDFIRE_UART_CSR_TCS0	(1 << 0)	/* Tx clock select 0 */
#define COLDFIRE_UART_CSR_TCS1	(1 << 1)	/* Tx clock select 1 */
#define COLDFIRE_UART_CSR_TCS2	(1 << 2)	/* Tx clock select 2 */
#define COLDFIRE_UART_CSR_TCS3	(1 << 3)	/* Tx clock select 3 */
#define COLDFIRE_UART_CSR_RCS0	(1 << 4)	/* Rx clock select 0 */
#define COLDFIRE_UART_CSR_RCS1	(1 << 5)	/* Rx clock select 1 */
#define COLDFIRE_UART_CSR_RCS2	(1 << 6)	/* Rx clock select 2 */
#define COLDFIRE_UART_CSR_RCS3	(1 << 7)	/* Rx clock select 3 */

#define COLDFIRE_UART_CSR_TIMER_TX	(COLDFIRE_UART_CSR_TCS0 \
				 | COLDFIRE_UART_CSR_TCS2 \
				 | COLDFIRE_UART_CSR_TCS3)
#define COLDFIRE_UART_CSR_TIMER_RX	(COLDFIRE_UART_CSR_RCS0 \
				 | COLDFIRE_UART_CSR_RCS2 \
				 | COLDFIRE_UART_CSR_RCS3)

/* CR - command register */

#define COLDFIRE_UART_CR_RC0	(1 << 0)	/* receiver control bit 0 */
#define COLDFIRE_UART_CR_RC1	(1 << 1)	/* receiver control bit 1 */
#define COLDFIRE_UART_CR_TC0	(1 << 2)	/* transmitter control bit 0 */
#define COLDFIRE_UART_CR_TC1	(1 << 3)	/* transmitter control bit 1 */
#define COLDFIRE_UART_CR_MISC0	(1 << 4)	/* misc control bit 0 */
#define COLDFIRE_UART_CR_MISC1	(1 << 5)	/* misc control bit 1 */
#define COLDFIRE_UART_CR_MISC2	(1 << 6)	/* misc control bit 2 */

/* Some common commands */

#define COLDFIRE_UART_CR_TX_ENABLE	(COLDFIRE_UART_CR_TC0)
#define COLDFIRE_UART_CR_TX_DISABLE	(COLDFIRE_UART_CR_TC1)
#define COLDFIRE_UART_CR_RX_ENABLE	(COLDFIRE_UART_CR_RC0)
#define COLDFIRE_UART_CR_RX_DISABLE	(COLDFIRE_UART_CR_RC1)
#define COLDFIRE_UART_CR_RESET_MODE_PTR	(COLDFIRE_UART_CR_MISC0)
#define COLDFIRE_UART_CR_RESET_RX	(COLDFIRE_UART_CR_MISC1)
#define COLDFIRE_UART_CR_RESET_TX	(COLDFIRE_UART_CR_MISC1 \
					 | COLDFIRE_UART_CR_MISC0)
#define COLDFIRE_UART_CR_RESET_ERR	(COLDFIRE_UART_CR_MISC2)
#define COLDFIRE_UART_CR_RESET_BRK	(COLDFIRE_UART_CR_MISC2 \
					 | COLDFIRE_UART_CR_MISC0)

/* ACR - auxiliary control register */

#define COLDFIRE_UART_ACR_IEC	(1 << 0)	/* input enable control */

/* IMR - interrupt mask register */

#define COLDFIRE_UART_IMR_TXRDY	(1 << 0)	/* transmitter ready */
#define COLDFIRE_UART_IMR_RXRDY	(1 << 1)	/* receiver ready */
#define COLDFIRE_UART_IMR_DB	(1 << 2)	/* delta break */
#define COLDFIRE_UART_IMR_COS	(1 << 7)	/* change of CTS state */

/* ISR - interrupt status register */

#define COLDFIRE_UART_ISR_TXRDY	(1 << 0)	/* transmitter ready */
#define COLDFIRE_UART_ISR_RXRDY	(1 << 1)	/* receiver ready */
#define COLDFIRE_UART_ISR_DB	(1 << 2)	/* delta break */
#define COLDFIRE_UART_ISR_COS	(1 << 7)	/* change of CTS state */

/* IP - input port register */

#define COLDFIRE_UART_IP_CTS	(1 << 0)	/* current CTS state */

/* OP1 - output set register */

#define COLDFIRE_UART_OP1_RTS	(1 << 0)	/* set RTS */

/* OP2 - output reset register */

#define COLDFIRE_UART_OP2_RTS	(1 << 0)	/* clear RTS */

/* function declarations */

#if defined(__STDC__) || defined(__cplusplus)

IMPORT	void	coldfireDevInit (COLDFIRE_CHAN *pChan);
IMPORT	void	coldfireDevInit2 (COLDFIRE_CHAN *pChan);
IMPORT  void    coldfireInt (COLDFIRE_CHAN *pChan);

#else	/* __STDC__ */

IMPORT	void	coldfireDevInit ();
IMPORT	void	coldfireDevInit2 ();
IMPORT  void    coldfireInt ();

#endif	/* __STDC__ */

#ifdef __cplusplus
}
#endif

#endif /* __INCcoldfireSioh */
