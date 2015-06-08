/* m5206Sio.h - Motorola MCF5206 internal UART header file */

/* Copyright 1984-1997 Wind River Systems, Inc. */

/*
modification history
--------------------
01a,21jan96,kab  written
*/

/*
This file contains constants for the UART contained in the Motorola MCF5206.
The constant SIO must defined when
including this header file.
*/

#ifndef __INCm5206Sioh
#define __INCm5206Sioh

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
    } M5206_CHAN;

#endif	/* _ASMLANGUAGE */

/* register bit definitions */

/* MR1 - mode register 1 */
#define M5206_UART_MR1_BC0	(1 << 0)	/* bits per char 0 */
#define M5206_UART_MR1_BC1	(1 << 1)	/* bits per char 1 */
#define M5206_UART_MR1_PT	(1 << 2)	/* parity type */
#define M5206_UART_MR1_PM0	(1 << 3)	/* parity mode 0 */
#define M5206_UART_MR1_PM1	(1 << 4)	/* parity mode 1 */
#define M5206_UART_MR1_ERR	(1 << 5)	/* error mode */
#define M5206_UART_MR1_RX_IRQ	(1 << 6)	/* receiver interrupt select */
#define M5206_UART_MR1_RX_RTS	(1 << 7)	/* Rx RTS control */

/* Some common modes */
#define M5206_UART_MR1_BITS_CHAR_5	0
#define M5206_UART_MR1_BITS_CHAR_6	M5206_UART_MR1_BC0
#define M5206_UART_MR1_BITS_CHAR_7	M5206_UART_MR1_BC1
#define M5206_UART_MR1_BITS_CHAR_8	(M5206_UART_MR1_BC0 \
					 | M5206_UART_MR1_BC1)
#define M5206_UART_MR1_NO_PARITY	(M5206_UART_MR1_PM1)
#define M5206_UART_MR1_PAR_MODE_MULTI	(M5206_UART_MR1_PM0 \
					 | M5206_UART_MR1_PM1)
#define M5206_UART_MR1_EVEN_PARITY	0
#define M5206_UART_MR1_ODD_PARITY	M5206_UART_MR1_PT

/* MR2 - mode register 2 */
#define M5206_UART_MR2_SB0	(1 << 0)	/* stop bit control 0 */
#define M5206_UART_MR2_SB1	(1 << 1)	/* stop bit control 1 */
#define M5206_UART_MR2_SB2	(1 << 2)	/* stop bit control 2 */
#define M5206_UART_MR2_SB3	(1 << 3)	/* stop bit control 3 */
#define M5206_UART_MR2_TX_CTS	(1 << 4)	/* Tx CTS control */
#define M5206_UART_MR2_TX_RTS	(1 << 5)	/* Tx RTS control */
#define M5206_UART_MR2_CM0	(1 << 6)	/* channel mode 0 */
#define M5206_UART_MR2_CM1	(1 << 7)	/* channel mode 1 */

/* Some common bit lengths */
#define M5206_UART_MR2_STOP_BITS_1	(M5206_UART_MR2_SB0 \
					 | M5206_UART_MR2_SB1 \
					 | M5206_UART_MR2_SB2)
#define M5206_UART_MR2_STOP_BITS_2	(M5206_UART_MR2_SB0 \
					 | M5206_UART_MR2_SB1 \
					 | M5206_UART_MR2_SB2 \
					 | M5206_UART_MR2_SB3)

/* SR - status register */
#define M5206_UART_SR_RXRDY	(1 << 0)	/* Rx ready */
#define M5206_UART_SR_FFULL	(1 << 1)	/* FIFO full */
#define M5206_UART_SR_TXRDY	(1 << 2)	/* Tx ready */
#define M5206_UART_SR_TXEMP	(1 << 3)	/* Tx empty */
#define M5206_UART_SR_OE	(1 << 4)	/* overrun error */
#define M5206_UART_SR_PE	(1 << 5)	/* parity error */
#define M5206_UART_SR_FE	(1 << 6)	/* framing error */
#define M5206_UART_SR_RB	(1 << 7)	/* received break */

/* CSR - clock select register */
#define M5206_UART_CSR_TCS0	(1 << 0)	/* Tx clock select 0 */
#define M5206_UART_CSR_TCS1	(1 << 1)	/* Tx clock select 1 */
#define M5206_UART_CSR_TCS2	(1 << 2)	/* Tx clock select 2 */
#define M5206_UART_CSR_TCS3	(1 << 3)	/* Tx clock select 3 */
#define M5206_UART_CSR_RCS0	(1 << 4)	/* Rx clock select 0 */
#define M5206_UART_CSR_RCS1	(1 << 5)	/* Rx clock select 1 */
#define M5206_UART_CSR_RCS2	(1 << 6)	/* Rx clock select 2 */
#define M5206_UART_CSR_RCS3	(1 << 7)	/* Rx clock select 3 */

#define M5206_UART_CSR_TIMER_TX	(M5206_UART_CSR_TCS0 \
				 | M5206_UART_CSR_TCS2 \
				 | M5206_UART_CSR_TCS3)
#define M5206_UART_CSR_TIMER_RX	(M5206_UART_CSR_RCS0 \
				 | M5206_UART_CSR_RCS2 \
				 | M5206_UART_CSR_RCS3)

/* CR - command register */
#define M5206_UART_CR_RC0	(1 << 0)	/* receiver control bit 0 */
#define M5206_UART_CR_RC1	(1 << 1)	/* receiver control bit 1 */
#define M5206_UART_CR_TC0	(1 << 2)	/* transmitter control bit 0 */
#define M5206_UART_CR_TC1	(1 << 3)	/* transmitter control bit 1 */
#define M5206_UART_CR_MISC0	(1 << 4)	/* misc control bit 0 */
#define M5206_UART_CR_MISC1	(1 << 5)	/* misc control bit 1 */
#define M5206_UART_CR_MISC2	(1 << 6)	/* misc control bit 2 */

/* Some common commands */
#define M5206_UART_CR_TX_ENABLE		(M5206_UART_CR_TC0)
#define M5206_UART_CR_TX_DISABLE	(M5206_UART_CR_TC1)
#define M5206_UART_CR_RX_ENABLE		(M5206_UART_CR_RC0)
#define M5206_UART_CR_RX_DISABLE	(M5206_UART_CR_RC1)
#define M5206_UART_CR_RESET_MODE_PTR	(M5206_UART_CR_MISC0)
#define M5206_UART_CR_RESET_RX		(M5206_UART_CR_MISC1)
#define M5206_UART_CR_RESET_TX		(M5206_UART_CR_MISC0 \
					 | M5206_UART_CR_MISC1)
#define M5206_UART_CR_RESET_ERR		(M5206_UART_CR_MISC2)

/* ACR - auxiliary control register */
#define M5206_UART_ACR_IEC	(1 << 0)	/* input enable control */

/* IMR - interrupt mask register */
#define M5206_UART_IMR_TXRDY	(1 << 0)	/* transmitter ready */
#define M5206_UART_IMR_RXRDY	(1 << 1)	/* receiver ready */
#define M5206_UART_IMR_DB	(1 << 2)	/* delta break */
#define M5206_UART_IMR_COS	(1 << 7)	/* change of CTS state */

/* ISR - interrupt status register */
#define M5206_UART_ISR_TXRDY	(1 << 0)	/* transmitter ready */
#define M5206_UART_ISR_RXRDY	(1 << 1)	/* receiver ready */
#define M5206_UART_ISR_DB	(1 << 2)	/* delta break */
#define M5206_UART_ISR_COS	(1 << 7)	/* change of CTS state */

/* IP - input port register */
#define M5206_UART_IP_CTS	(1 << 0)	/* current CTS state */

/* OP1 - output set register */
#define M5206_UART_OP1_RTS	(1 << 0)	/* set RTS */

/* OP2 - output reset register */
#define M5206_UART_OP2_RTS	(1 << 0)	/* clear RTS */

/* function declarations */

#if defined(__STDC__) || defined(__cplusplus)

IMPORT	void	m5206DevInit (M5206_CHAN *pChan);
IMPORT	void	m5206DevInit2 (M5206_CHAN *pChan);
IMPORT  void    m5206Int (M5206_CHAN *pChan);

#else	/* __STDC__ */

IMPORT	void	m5206DevInit ();
IMPORT	void	m5206DevInit2 ();
IMPORT  void    m5206Int ();

#endif	/* __STDC__ */

#ifdef __cplusplus
}
#endif

#endif /* __INCm5206Sioh */
