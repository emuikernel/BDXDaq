/* m68681Sio.h - Motorola M68681 serial chip header file */

/* Copyright 1984-1996 Wind River Systems, Inc. */

/*
modification history
--------------------
01b,03jun96,dat	 added opcrCopy and oprCopy fields, new rtns
01a,08feb96,dds	 created.
*/

/*
This file contains constants for the Motorola M68681 serial chip.
The constants DUART_REG_OFFSET and SIO must defined when
including this header file.
*/

#ifndef __INCm68681Sioh
#define __INCm68681Sioh

#ifdef __cplusplus
extern "C" {
#endif

#ifndef	_ASMLANGUAGE

#include "sioLib.h"

/* device and channel structures */

/* 
 * The M68681_CHAN structure defines a serial I/O channel which
 * describes the baudrate, interrupt mode, control 
 * registers mode registers and the I/O port registers for a given channel. 
 * Also the various SIO driver functions ( ioctl, txStarup, callbackInstall,
 * pollInput, polloutput ) that this channel could utilize are declared 
 * here.
*/

typedef volatile UCHAR VCHAR;	/* shorthand for volatile UCHAR */

typedef struct m68681_chan	/* M68681_CHAN */
    {
    /* always goes first */
    SIO_CHAN	sio;		/* driver functions */
    
    struct m68681_duart *pDuart;/* pointer to the mc68681 DUART */

    /* callbacks */

    STATUS	(*getTxChar)();	/* pointer to a xmitr function */
    STATUS	(*putRcvChar)();/* pointer to a recvr function */
    void *	getTxArg;
    void *	putRcvArg;

    /* control and data register addresses for each channel */

    VCHAR *	mr;		/* mode register address */
    VCHAR *	sr;		/* status register address */
    VCHAR *	csr;		/* clock select  register address */
    VCHAR *	cr;		/* command register address */
    VCHAR *	rb;		/* receiver buffer address */
    VCHAR *	tb;		/* transmit buffer address */

    UCHAR	xmitEnb;	/* bit for transmit enable */
    UCHAR	rcvrEnb;	/* bit for transmit enable */
    UCHAR	rxBreak;	/* bit for enabling break interrupt */

    /* misc values */

    UINT	baudRate;	/* current baud rate */
    UINT	options;	/* current options, SIO_HW_OPTS_SET */

    /* interrupt/polled mode configuration info */

    UINT	mode;		/* SIO_MODE_[INT | POLL] */
    UCHAR       intVec;		/* interrupt vector address */
    UCHAR       intType;	/* type of vector to supply from M68681 */
    UINT	channel;	/* channel number (0 or 1) */
    } M68681_CHAN;

/* This structure defines that the m68681 has two channels */

typedef struct m68681_duart	/* M68681_DUART */
    {
    M68681_CHAN  portA;		/* port A device descriptor */
    M68681_CHAN  portB;		/* port B device descriptor */

    /* The registers defined below are common to both channels */

    VCHAR *	ipcr;		/* input port change register address */
    VCHAR *	acr;		/* auxilliary control register address */
    VCHAR *	isr;		/* interupt status register address */
    VCHAR *	imr;		/* interupt mask register address */
    VCHAR *	ip;		/* input port register address */
    VCHAR *	opcr;		/* output port config register address */
    VCHAR *	sopbc;		/* set output port register address */
    VCHAR *	ropbc;		/* reset output port register address */
    VCHAR *	ctroff;		/* Counter off */
    VCHAR *	ctron;		/* counter on  */
    VCHAR *	ctlr;		/* counter timer lower register  */
    VCHAR *	ctur;		/* counter timer upper register  */
    VCHAR *	ivr;		/* interupt vector register address */

    UCHAR 	imrCopy;	/* stores interrupt mask register value  */
    UINT	baudFreq;	/* baud clock frequency */
    UCHAR	acrCopy;	/* copy of acr */
    UCHAR	opcrCopy;	/* copy of opcr */
    UCHAR	oprCopy;	/* copy of opr */

    VOIDFUNCPTR	tickRtn;	/* Timer interrupt service routine */
    void *	tickArg;	/* pointer to the system clock */
    BOOL	intEnable;	/* allow interrupt mode flag */

    } M68681_DUART;

typedef struct m68681_clock	/* M68681_CLK */
    {
    M68681_DUART * pDuart;	/* pointer to the DUART */
    VOIDFUNCPTR	routine;	/* user routine */
    void *	arg;		/* arg for user routine */
    BOOL	running;	/* current status */
    UINT	rate;		/* interrupt rate, ticks per second */
    UINT	hertz;		/* oscillator input rate, hertz */
    UINT	min;		/* min rate */
    UINT	max;		/* max rate */
    UINT	preload;	/* counter/timer preload value */
    } M68681_CLK;


#if defined(__STDC__) || defined(__cplusplus)

/* serial procedures */
IMPORT	void	m68681DevInit	(M68681_DUART *);
IMPORT	void	m68681DevInit2	(M68681_DUART *);
IMPORT	void	m68681Int	(M68681_DUART *);
IMPORT	void	m68681ImrSetClr	(M68681_DUART *, UCHAR, UCHAR);
IMPORT	void	m68681AcrSetClr	(M68681_DUART *, UCHAR, UCHAR);
IMPORT	void	m68681OprSetClr	(M68681_DUART *, UCHAR, UCHAR);
IMPORT	void	m68681OpcrSetClr(M68681_DUART *, UCHAR, UCHAR);
IMPORT	UCHAR	m68681Imr	(M68681_DUART *);
IMPORT	UCHAR	m68681Acr	(M68681_DUART *);
IMPORT	UCHAR	m68681Opcr	(M68681_DUART *);
IMPORT	UCHAR	m68681Opr	(M68681_DUART *);

/* clock procedures */
IMPORT	STATUS	m68681ClkInit	(M68681_CLK *, M68681_DUART *);
IMPORT	void	m68681ClkInt	(M68681_CLK *);
IMPORT	STATUS	m68681ClkConnect(M68681_CLK *, VOIDFUNCPTR, void *);
IMPORT	void	m68681ClkEnable (M68681_CLK *);
IMPORT	void	m68681ClkDisable(M68681_CLK *); 
IMPORT	int	m68681ClkRateGet(M68681_CLK *);
IMPORT	STATUS	m68681ClkRateSet(M68681_CLK *, int);
IMPORT	int	m68681ClkReadOnFly(M68681_CLK *);

#else   /* __STDC__ */

IMPORT	void	m68681DevInit	();
IMPORT	void	m68681DevInit2	();
IMPORT	void	m68681Int	();
IMPORT	void	m68681ImrSetClr	();
IMPORT	void	m68681AcrSetClr	();

IMPORT	STATUS	m68681ClkInit	();
IMPORT	void	m68681ClkInt	();
IMPORT	STATUS	m68681ClkConnect();
IMPORT	void	m68681ClkEnable ();
IMPORT	void	m68681ClkDisable(); 
IMPORT	int	m68681ClkRateGet();
IMPORT	STATUS	m68681ClkRateSet();
IMPORT	int	m68681ClkReadOnFly();

#endif  /* __STDC__ */

#endif	/* _ASMLANGUAGE */

/* channels */

#define M68681_CHANNEL_A   0
#define M68681_CHANNEL_B   1


#define M68681_N_CHANS    2	/* number of serial channels on chip */

#ifdef	_ASMLANGUAGE
#   define M681_ADRS(reg)	(M68681_BASE + (reg * M68681_REG_OFFSET))
#else
#   define M681_ADRS(reg)	((VCHAR *)M68681_BASE+(reg*M68681_REG_OFFSET))
#endif	/* _ASMLANGUAGE */

/* SIO -- m68681 serial channel chip -- register definitions */

#define M68681_MRA	M681_ADRS(0x00)	/* R/W: mode reg. A */
#define M68681_SRA	M681_ADRS(0x01)	/* R: status reg. A */
#define M68681_CSRA	M681_ADRS(0x01)	/* W: clock select reg. A */
#define M68681_CRA	M681_ADRS(0x02)	/* W: command reg. A */
#define M68681_RHRA	M681_ADRS(0x03)	/* R: receive buffer A */
#define M68681_THRA	M681_ADRS(0x03)	/* W: transmit buffer A */
#define M68681_IPCR	M681_ADRS(0x04)	/* R: input port change reg. */
#define M68681_ACR	M681_ADRS(0x04)	/* W: auxiliary control reg. */
#define M68681_ISR	M681_ADRS(0x05)	/* R: int. status reg. */
#define M68681_IMR	M681_ADRS(0x05)	/* W: int. mask reg. */
#define M68681_CUR	M681_ADRS(0x06)	/* R: current MSB of counter */
#define M68681_CTUR	M681_ADRS(0x06)	/* W: counter timer upper reg. */
#define M68681_CLR	M681_ADRS(0x07)	/* W: current LSB of counter */
#define M68681_CTLR	M681_ADRS(0x07)	/* W: counter timer lower reg. */
#define M68681_MRB	M681_ADRS(0x08)	/* R/W: mode reg. B */
#define M68681_SRB	M681_ADRS(0x09)	/* R: status reg. B */
#define M68681_CSRB	M681_ADRS(0x09)	/* W: clock select reg. B */
#define M68681_CRB	M681_ADRS(0x0a)	/* W: command reg. B */
#define M68681_RHRB	M681_ADRS(0x0b)	/* R: receive buffer B */
#define M68681_THRB	M681_ADRS(0x0b)	/* W: transmit buffer B */
#define M68681_IVR	M681_ADRS(0x0c)	/* R/W: int. vector reg. */
#define M68681_IP	M681_ADRS(0x0d)	/* R: input port (Unlatched) */
#define M68681_OPCR	M681_ADRS(0x0d)	/* W: output port config. reg. */
#define M68681_CTRON	M681_ADRS(0x0e)	/* R: counter on */
#define M68681_SOPBC	M681_ADRS(0x0e)	/* W: set output port bits */
#define M68681_CTROFF	M681_ADRS(0x0f)	/* R: counter off */
#define M68681_ROPBC	M681_ADRS(0x0f)	/* W: reset output port bits */

/* equates for mode reg. 1 for channel  A or B */

#define M68681_MR1_RX_RTS          0x80 /* receiver RTS enabled */
#define M68681_MR1_RX_INT          0x40 /* receiver intrupt enabled */
#define M68681_MR1_ERR_MODE        0x20 /* block error mode */
#define M68681_MR1_PAR_MODE_MULTI  0x18 /* multi_drop mode */
#define M68681_MR1_NO_PARITY       0x10 /* no parity mode */
#define M68681_MR1_ALWAYS_0        0x08 /* force parity mode */
#define M68681_MR1_ALWAYS_1        0x0c /* force parity mode */
#define M68681_MR1_EVEN_PARITY     0x00 /* parity mode */
#define M68681_MR1_ODD_PARITY      0x04 /* 0 = even, 1 = odd */
#define M68681_MR1_BITS_CHAR_8     0x03 /* 8 bits */
#define M68681_MR1_BITS_CHAR_7     0x02 /* 7 bits */
#define M68681_MR1_BITS_CHAR_6     0x01 /* 6 bits */
#define M68681_MR1_BITS_CHAR_5     0x00 /* 5 bits */

/* equates for mode reg. 2 for channel  A or B */

#define M68681_MR2_NORMAL_MODE     0x00 /* normal channel mode */
#define M68681_MR2_AUTO_MODE       0x40 /* automatic channel mode */
#define M68681_MR2_LOOPBACK_LOCL   0x80 /* local loopback channel mode */
#define M68681_MR2_LOOPBACK_REMT   0xc0 /* remote loopback channel mode */
#define M68681_MR2_TX_RTS          0x20 /* transmitter RTS enabled */
#define M68681_MR2_TX_CTS          0x10 /* transmitter CTS enabled */
#define M68681_MR2_STOP_BITS_2     0x0f /* 2 stop bits */
#define M68681_MR2_STOP_BITS_1     0x07 /* 1 stop bit */

/* equates for command reg. A or B */

#define	M68681_CR_NO_COMMAND    	0x00    /* no command */
#define	M68681_CR_RST_MR_PTR_CMD	0x10    /* reset mr pointer command */
#define	M68681_CR_RST_RX_CMD	        0x20	/* reset receiver command */
#define	M68681_CR_RST_TX_CMD	        0x30	/* reset transmitter command */
#define	M68681_CR_RST_ERR_STS_CMD	0x40	/* reset error status cmnd */
#define	M68681_CR_RST_BRK_INT_CMD	0x50    /* reset break int. command */
#define	M68681_CR_STR_BREAK_CMD	        0x60	/* start break command */
#define	M68681_CR_STP_BREAK_CMD	        0x70	/* stop break command */

#define	M68681_CR_RX_ENABLE     	0x01	/* receiver enabled */
#define	M68681_CR_RX_DISABLE	        0x02	/* receiver disabled */
#define	M68681_CR_TX_ENABLE	        0x04	/* transmitter enabled */
#define	M68681_CR_TX_DISABLE	        0x08	/* transmitter disabled */

/* equates for clock select reg. A or B */

#define M68681_CSR_RX_SEL      0xf0            /* receiver clock select */
#define M68681_CSR_RX_38400    0xc0            /* 38400, set 1 */
#define M68681_CSR_RX_19200    0xc0            /* 19200 */
#define M68681_CSR_RX_9600     0xb0            /* 9600 */
#define M68681_CSR_RX_1800     0xa0            /* 1800 */
#define M68681_CSR_RX_4800     0x90            /* 4800 */
#define M68681_CSR_RX_2400     0x80            /* 2400 */
#define M68681_CSR_RX_2000     0x70            /* 2000 */
#define M68681_CSR_RX_1200     0x60            /* 1200 */
#define M68681_CSR_RX_600      0x50            /* 600 */
#define M68681_CSR_RX_300      0x40            /* 300 */
#define M68681_CSR_RX_150      0x30            /* 150 */
#define M68681_CSR_RX_134_5    0x20            /* 134.5 */
#define M68681_CSR_RX_110      0x10            /* 110 */
#define M68681_CSR_RX_75       0x00            /* 75 */
#define M68681_CSR_TX_SEL      0x0f            /* transmitter clock select */
#define M68681_CSR_TX_38400    0x0c            /* 38400, set 1 */
#define M68681_CSR_TX_19200    0x0c            /* 19200 */
#define M68681_CSR_TX_9600     0x0b            /* 9600 */
#define M68681_CSR_TX_1800     0x0a            /* 1800 */
#define M68681_CSR_TX_4800     0x09            /* 4800 */
#define M68681_CSR_TX_2400     0x08            /* 2400 */
#define M68681_CSR_TX_2000     0x07            /* 2000 */
#define M68681_CSR_TX_1200     0x06            /* 1200 */
#define M68681_CSR_TX_600      0x05            /* 600 */
#define M68681_CSR_TX_300      0x04            /* 300 */
#define M68681_CSR_TX_150      0x03            /* 150 */
#define M68681_CSR_TX_134_5    0x02            /* 134.5 */
#define M68681_CSR_TX_110      0x01            /* 110 */
#define M68681_CSR_TX_75       0x00            /* 75 */

/* equates for status reg. A or B */

#define	M68681_SR_BREAK          	0x80	/* received break */
#define	M68681_SR_FRAMING   	        0x40	/* framing error */
#define	M68681_SR_PARITY        	0x20	/* parity error */
#define	M68681_SR_OVERRUN       	0x10	/* overrun error */
#define	M68681_SR_TXEMT 		0x08	/* transmitter empty */
#define	M68681_SR_TXRDY 		0x04	/* transmitter ready*/
#define	M68681_SR_FFULL 		0x02	/* fifo full */
#define	M68681_SR_RXRDY 		0x01	/* receiver ready */

/* equates for int. status reg. */

#define	M68681_ISR_INPUT_DELTA_INT	0x80	/* input port change*/
#define	M68681_ISR_BREAK_B_INT   	0x40	/* delta break B */
#define	M68681_ISR_RX_RDY_B_INT	        0x20	/* rcvr ready / fifo full B */
#define	M68681_ISR_TX_RDY_B_INT 	0x10	/* transmitter ready B */
#define	M68681_ISR_CTR_RDY_INT  	0x08	/* counter / timer ready */
#define	M68681_ISR_BREAK_A_INT  	0x04	/* delta break A */
#define	M68681_ISR_RX_RDY_A_INT  	0x02	/* receiver rdy /fifo full A */
#define	M68681_ISR_TX_RDY_A_INT 	0x01	/* transmitter ready A */

/* equates for int. mask reg. */

#define M68681_IMR_CLEAR                0xff    /* Clear the imr */
#define	M68681_IMR_INPUT_DELTA  	0x80	/* input port change*/
#define	M68681_IMR_BREAK_B		0x40	/* delta break B */
#define	M68681_IMR_RX_RDY_B    	        0x20	/* rcvr rdy / fifo full B */
#define	M68681_IMR_TX_RDY_B	        0x10	/* transmitter ready B */
#define	M68681_IMR_CTR_RDY		0x08	/* counter / timer ready */
#define	M68681_IMR_BREAK_A		0x04	/* delta break A */
#define	M68681_IMR_RX_RDY_A     	0x02	/* rcvr ready / fifo full A */
#define	M68681_IMR_TX_RDY_A   	        0x01	/* transmitter ready A */

/* equates for auxiliary control reg. (timer and counter clock selects) */

#define	M68681_ACR_BRG_SELECT	        0x80	/* baud rate gen select */
				                /* 0 = set 1; 1 = set 2 */
				                /* equates are set 2 ONLY */
#define	M68681_ACR_TMR_EXT_CLK_16	0x70    /* xtnl clock divided by 16 */
#define	M68681_ACR_TMR_EXT_CLK	        0x60	/* external clock */
#define	M68681_ACR_TMR_IP2_16	        0x50	/* ip2 divided by 16 */
#define	M68681_ACR_TMR_IP2		0x40	/* ip2 */
#define	M68681_ACR_CTR_EXT_CLK_16	0x30	/* xtnl clock divided by 16 */
#define	M68681_ACR_CTR_TXCB	        0x20	/* channel B xmitr clock */
#define	M68681_ACR_CTR_TXCA      	0x10	/* channel A xmitr clock */
#define	M68681_ACR_CTR_IP2		0x00	/* ip2 */
#define	M68681_ACR_DELTA_IP3_INT	0x08	/* delta ip3 int. */
#define	M68681_ACR_DELTA_IP2_INT	0x04	/* delta ip2 int. */
#define	M68681_ACR_DELTA_IP1_INT	0x02	/* delta ip1 int. */
#define	M68681_ACR_DELTA_IP0_INT	0x01	/* delta ip0 int. */

/* equates for input port change reg. */

#define	M68681_IPCR_DELTA_IP3	        0x80	/* 0 = no, 1 = yes */
#define	M68681_IPCR_DELTA_IP2	        0x40	/* 0 = no, 1 = yes */
#define	M68681_IPCR_DELTA_IP1	        0x20	/* 0 = no, 1 = yes */
#define	M68681_IPCR_DELTA_IP0	        0x10	/* 0 = no, 1 = yes */
#define	M68681_IPCR_IP3	        	0x08	/* 0 = low, 1 = high */
#define	M68681_IPCR_IP2	           	0x04	/* 0 = low, 1 = high */
#define	M68681_IPCR_IP1    		0x02	/* 0 = low, 1 = high */
#define	M68681_IPCR_IP0	        	0x01	/* 0 = low, 1 = high */

/* equates for output port config. reg. */

#define	M68681_OPCR_OP7		        0x80	/* 0 = OPR[7], 1 = TxRDYB */
#define	M68681_OPCR_OP6 		0x40	/* 0 = OPR[6], 1 = TxRDYA */
#define	M68681_OPCR_OP5	        	0x20	/* 0 = OPR[5], 1 = RxRDYB */
#define	M68681_OPCR_OP4  		0x10	/* 0 = OPR[4], 1 = RxRDYA */
#define	M68681_OPCR_OP3 		0x00	/* 0 = OPR[3] */
#define	M68681_OPCR_RXCB_1X		0x0c	/* RCXB_1X */
#define	M68681_OPCR_TXCB_1X		0x08	/* TXCB_1X */
#define	M68681_OPCR_C_T_OUTPUT  	0x04	/* C_T_OUTPUT */

#define	M68681_OPCR_TXCA_16X    	0x01	/* TXCA_16X */
#define	M68681_OPCR_TXCA_1X		0x02	/* TXCA_1X */
#define	M68681_OPCR_RXCA_1X		0x03	/* RXCA_1X */
#define	M68681_OPCR_OP2 		0x00	/* 0 = OPR[2] */

/* Output port bit assignments */
#define M68681_RTS_A_LOW		0x01	/* 1= low, 0=hi */
#define M68681_RTS_B_LOW		0x02	/* 1= low, 0=hi */

/* Macros for default baud, parity, stop bits & data bits */

#define M68681_DEFAULT_BAUD		(9600)

#define M68681_ERROR_STATUS (M68681_SR_BREAK|M68681_SR_FRAMING|M68681_SR_PARITY \
                      |M68681_SR_OVERRUN) 

#define M68681_CHAN_A_INT (M68681_ISR_BREAK_A_INT|M68681_ISR_RX_RDY_A_INT \
                    |M68681_ISR_TX_RDY_A_INT)


#ifdef __cplusplus
}
#endif

#endif /* __INCm68681Sioh */
