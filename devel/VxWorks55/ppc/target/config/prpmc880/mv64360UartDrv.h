/* mv64360MpscSerial.h - Low Level MV64360 Mpsc Header File */

/* Copyright 2002 Motorola, Inc., All Rights Reserved */

/*
modification history
--------------------
01a,29may03,simon  ported from Motoload.
*/

/* 
DESCRIPTION
This file contains the MV64360 MPSC definitions.
*/

#ifndef _mv64360UartDrv_h		
#define _mv64360UartDrv_h

#ifdef __cplusplus
extern "C" {
#endif

/* typedefs */

typedef  struct 	/* mv64360Serial_CHAN */
    {
    /* always goes first */

    SIO_DRV_FUNCS *     pDrvFuncs;      /* driver functions */

    /* callbacks */

    STATUS      (*getTxChar) (); /* pointer to xmitr function */	
    STATUS      (*putRcvChar) (); /* pointer to rcvr function */
    void *      getTxArg;
    void *      putRcvArg;

    volatile INT16 channelMode;	   /* SIO_MODE    	*/
    int            MPSCNum;    /* the mpsc number 0 or 1 */
    int            baudRate;   	   /* Serial channel baud rate 	*/
    int            sysCLK;   	   
    int            BRGNum;    /* The clock source */
    void      *pDriverSpecifics;
    } mv64360Serial_CHAN;

/* defines  */
#define CHANNEL0_REGISTER1 0x800C
#define CHANNEL0_REGISTER2 0x8010
#define CHANNEL0_REGISTER3 0x8014
#define CHANNEL0_REGISTER4 0x8018
#define CHANNEL0_REGISTER5 0x801C
#define CHANNEL0_REGISTER6 0x8020
#define CHANNEL0_REGISTER7 0x8024
#define CHANNEL0_REGISTER8 0x8028
#define CHANNEL0_REGISTER9 0x802C
#define CHANNEL0_REGISTER10 0x8030
#define CHANNEL0_REGISTER11 0x8034
#define MPSC0_PROTOCOL_CONFIGURATION 0x8008

/*
 * mpsc main configuration low register bit definitions
 */
#define MV64360_UART_MAIN_CONFIG_LOW_MODE_HDLC 	(0<<0)	/* HDLC mode */ 
#define MV64360_UART_MAIN_CONFIG_LOW_MODE_UART 	(4<<0)	/* UART mode */ 
#define MV64360_UART_MAIN_CONFIG_LOW_MODE_BISYNC (5<<0)	/* BISYNC mode */ 
#define MV64360_UART_MAIN_CONFIG_LOW_ET 	(1<<6)	/* enable transmit */ 
#define MV64360_UART_MAIN_CONFIG_LOW_ER 	(1<<7)	/* enable receive */ 
#define MV64360_UART_MAIN_CONFIG_LOW_LOOP	(1<<8)	/* internal loopback */
#define MV64360_UART_MAIN_CONFIG_LOW_NLM	(1<<10)	/* NULL modem */
#define MV64360_UART_MAIN_CONFIG_LOW_TIDL	(0<<16)	/* Transmis Idle */
#define MV64360_UART_MAIN_CONFIG_LOW_RTSM	(0<<17)	/* RTS# Mode */
#define MV64360_UART_MAIN_CONFIG_LOW_CTSS	(0<<19)	/* CTS# Asynchronous */
#define MV64360_UART_MAIN_CONFIG_LOW_CDS	(0<<20)	/* RTS# Asynchronous */
#define MV64360_UART_MAIN_CONFIG_LOW_CRS_16	(1<<23)	/* RTS# Asynchronous */

/*
 * mpsc main configuration high register bit definitions
 */
#define MV64360_UART_MAIN_CONFIG_HIGH_TPL_16 	(6<<2)	/* transmit Preamble Length */ 
#define MV64360_UART_MAIN_CONFIG_HIGH_TPPT 	(15<<5)	/* transmit Preamble Pattern */
#define MV64360_UART_MAIN_CONFIG_HIGH_TCDV_1X 	(0<<9)	/* transmit clock divider, 1X clock mode */ 
#define MV64360_UART_MAIN_CONFIG_HIGH_TCDV_8X 	(1<<9)	/* transmit clock divider, 8X clock mode */
#define MV64360_UART_MAIN_CONFIG_HIGH_TCDV_16X 	(2<<9)	/* transmit clock divider, 16X clock mode */
#define MV64360_UART_MAIN_CONFIG_HIGH_TCDV_32X 	(3<<9)	/* transmit clock divider, 32X clock mode */
#define MV64360_UART_MAIN_CONFIG_HIGH_RDW 	(1<<22)	/* receive data width 8 bit */
#define MV64360_UART_MAIN_CONFIG_HIGH_RCDV_1X 	(0<<25)	/* receive clock divider, 1X clock mode */ 
#define MV64360_UART_MAIN_CONFIG_HIGH_RCDV_8X 	(1<<25)	/* receive clock divider, 8X clock mode */
#define MV64360_UART_MAIN_CONFIG_HIGH_RCDV_16X 	(2<<25)	/* receive clock divider, 16X clock mode */
#define MV64360_UART_MAIN_CONFIG_HIGH_RCDV_32X 	(3<<25)	/* receive clock divider, 32X clock mode */

/*
 * mpsc protocol control register bit definitions
 */

#define MV64360_UART_PROTOCOL_CONTROL_ISO_ASYN	(0<<7)	/* iso asynchronous mode */
#define MV64360_UART_PROTOCOL_CONTROL_ISO_ISYN	(1<<7)	/* iso synchronous mode */
#define MV64360_UART_PROTOCOL_CONTROL_WLS_5BITS	(0<<12)	/* word select 5 bits */
#define MV64360_UART_PROTOCOL_CONTROL_WLS_6BITS	(1<<12)	/* word select 6 bits */
#define MV64360_UART_PROTOCOL_CONTROL_WLS_7BITS	(2<<12)	/* word select 7 bits */
#define MV64360_UART_PROTOCOL_CONTROL_WLS_8BITS	(3<<12)	/* word select 8 bits */
#define MV64360_UART_PROTOCOL_CONTROL_STB_1BITS	(0<<14)	/* number of stop bits, 1 */
#define MV64360_UART_PROTOCOL_CONTROL_STB_2BITS	(1<<14)	/* number of stop bits, 2 */

/*
 * channel 1 register bit definitions
 */
#define MV64360_UART_CHR1_UBSR_BRK_DEFAULT	(0x22<<0)	/* Break default value */
#define MV64360_UART_CHR1_UBSR_TCS_DEFAULT	(0x33<<16)	/* TCS default value */

/*
 * channel 2 register bit definitions
 */
#define MV64360_UART_CHR2_COMMAND_TPM_OPS	(0<<2)	/* TPM odd parity select */
#define MV64360_UART_CHR2_COMMAND_TPM_EPS	(2<<2)	/* TPM even parity select */
#define MV64360_UART_CHR2_COMMAND_TCS		(1<<9)	/* command register transmit TCS character */
#define MV64360_UART_CHR2_COMMAND_RPM_OPS	(0<<18)	/* RPM odd parity select */
#define MV64360_UART_CHR2_COMMAND_RPM_EPS	(2<<18)	/* RPM even parity select */
#define MV64360_UART_CHR2_COMMAND_EH		(1<<31)	/* command register enter hunt */

/*
 * channel 4 register bit definitions
 */
#define MV64360_UART_CHR4_FILTER_CFR	(0<<0)	/* control filter register BCE */
#define MV64360_UART_CHR4_FILTER_Z	(1<<29)	/* control filter register enable debug mode */

/*
 * channel 5 register bit definitions
 */
#define MV64360_UART_CHR5_CHAR_INT	(1<<12)	/* control character register interrupt */
#define MV64360_UART_CHR5_CHAR_VALID	(1<<15)	/* control character register valid */

/*
 * baud rate configuration register bit definitions
 */
#define MV64360_UART_BRG0_EN    	(1<<16)	/* baud rate generators 0 enalbe */
#define MV64360_UART_BRG0_RESET   	(1<<17)	/* baud rate generators 0 reset */
#define MV64360_UART_BRG0_CLKS_BCLKIN	(0<<18)	/* input clock to the BRG is bclkin */
#define MV64360_UART_BRG0_CLKS_SCLK0	(2<<18)	/* input clock to the BRG is tsclk0 */
#define MV64360_UART_BRG0_CLKS_TSCLK0	(3<<18)	/* input clock to the BRG is tsclk0 */
#define MV64360_UART_BRG0_CLKS_SCLK1	(6<<18)	/* input clock to the BRG is sclk1 */
#define MV64360_UART_BRG0_CLKS_TSCLK1	(7<<18)	/* input clock to the BRG is tsclk1 */
#define MV64360_UART_BRG0_CLKS_SYSCLK	(8<<18)	/* input clock to the BRG is sysclk */

/*
 * interrupt enable register bit definitions
 */

#define MV64360_UART_INTERRUPT_MASK_RCC	(1<<6)	/* received data */
#define MV64360_UART_INTERRUPT_MASK_TEIDL	(1<<12)	/* transmit data */
#define MV64360_UART_INTERRUPT_MASK_MASK     (0x1040<<0)  /* interrupt mask 0x1040 */ 

/*
 * interrupt identification register bit definitions
 */

#define MV64360_UART_INTERRUPT_CAUSE_RCC	(1<<6)	/* received data */
#define MV64360_UART_INTERRUPT_CAUSE_TEIDL	(1<<12)	/* transmit data */
#define MV64360_UART_INTERRUPT_CAUSE_MASK     (0x1040<<0)  /* interrupt mask 0x1040 */ 

/* function declarations */

#if defined(__STDC__) || defined(__cplusplus)

extern void mv64360UartDrvInt (mv64360Serial_CHAN *);
extern void mv64360UartDrvInit (mv64360Serial_CHAN *);

#else

extern void mv64360UartDrvInt ();
extern void mv64360UartDrvInit ();	

#endif  /* __STDC__ */

#ifdef __cplusplus
}
#endif

#endif				/* } */
