/* 403gaSerial.h - 403ga on chip serial i/o header */

/* Copyright 1984-1995 Wind River Systems, Inc. */

/*
modification history
--------------------
01b,02feb95,yao added _SPLS_RX_ERR.
01a,02sep94,yao written.
*/

#ifndef __INC403gaSerialh
#define __INC403gaSerialh

#ifdef __cplusplus
extern "C" {
#endif

/* memory mapped i/o registers */

#define _PPC403GA_SPLS	0x40000000	/* line status register read/clear */
#define _PPC403GA_SPHS	0x40000002	/* handshake status register r/clear */
#define _PPC403GA_BRDH	0x40000004	/* baudrate divisor high reg r/w */
#define _PPC403GA_BRDL	0x40000005	/* baudrate divisor low reg r/w */
#define _PPC403GA_SPCTL	0x40000006	/* control register r/w */
#define _PPC403GA_SPRC	0x40000007	/* receiver command register r/w */
#define _PPC403GA_SPTC	0x40000008	/* transmitter command register r/w */
#define _PPC403GA_SPRB	0x40000009	/* receive buffer r */
#define _PPC403GA_SPTB	0x40000009	/* transmit buffer w */

/* masks and definitions for serial port control register */

#define _SPCTL_LM_MASK	0xc0		/* loop back modes */
#define _SPCTL_DTR_MASK	0x20		/* data terminal ready 0-inactive */
#define _SPCTL_RTS_MASK	0x10		/* request to send 0-inactive */
#define _SPCTL_DB_MASK	0x08		/* data bits mask */
#define _SPCTL_PE_MASK	0x04		/* parity enable */
#define _SPCTL_PTY_MASK	0x02		/* parity */
#define _SPCTL_SB_MASK	0x01		/* stop bit mask */

#define _SPCTL_LM_NORM	0x00		/* normal operation */
#define _SPCTL_LM_LOOP	0x40		/* internal loopback mode */
#define _SPCTL_LM_ECHO	0x80		/* automatic echo mode */
#define _SPCTL_LM_RES	0xc0		/* reserved */

#define _SPCTL_DTR_ACTIVE	_SPCTL_DTR_MASK	/* DTR is active */
#define _SPCTL_RTS_ACTIVE	_SPCTL_RTS_MASK	/* RTS is active */
#define _SPCTL_DB_8_BITS	_SPCTL_DB_MASK	/*  8 data bits */
#define _SPCTL_DB_7_BITS	0x00		/*  7 data bits */
#define _SPCTL_PE_ENABLE	_SPCTL_PE_MASK	/* parity enabled */
#define _SPCTL_PTY_EVEN		0x00		/* even parity */
#define _SPCTL_PTY_ODD		_SPCTL_PTY_MASK /* odd parity */
#define _SPCTL_SB_1_BIT		0x00		/* one stop bit */
#define _SPCTL_SB_2_BIT		_SPCTL_SB_MASK	/* two stop bit */

/* serial port handshake register */

#define _SPHS_DIS_MASK	0x80		/* DSR input inactive error mask */
#define _SPHS_CS_MASK	0x40		/* CTS input inactive error mask */
#define _SPHS_DIS_ACT 	0x00		/* dsr input is active */
#define _SPHS_DIS_INACT _SPHS_DIS_MASK	/* dsr input is inactive */
#define _SPHS_CS_ACT	0x00		/* cts input is active */
#define _SPHS_CS_INACT	_SPHS_CS_MASK	/* cts input is active */

/* serial port line status register */

#define _SPLS_RBR_MASK	0x80		/* receive buffer ready mask */
#define _SPLS_FE_MASK	0x40		/* framing error */
#define _SPLS_OE_MASK	0x20		/* overrun error */
#define	_SPLS_PE_MASK	0x10		/* parity error */
#define _SPLS_LB_MASK	0x08		/* line break */
#define _SPLS_TBR_MASK	0x04		/* transmit buffer ready */
#define _SPLS_TSR_MASK	0x02		/* transmit shift register ready */

#define _SPLS_RBR_FULL	_SPLS_RBR_MASK	/* receive buffer is full */
#define _SPLS_FE_ERROR	_SPLS_FE_MASK	/* framing error detected */
#define _SPLS_OE_ERROR	_SPLS_OE_MASK	/* overrun error detected */
#define _SPLS_PE_ERROR	_SPLS_PE_MASK	/* parity error detected */
#define _SPLS_LB_BREAK	_SPLS_LB_MASK	/* line break detected */
#define _SPLS_TBR_EMPTY	_SPLS_TBR_MASK	/* transmit buffer is ready */
#define _SPLS_TSR_EMPTY	_SPLS_TSR_MASK	/* transmit shift register is empty */

#define _SPLS_RX_ERR	(_SPLS_LB_BREAK | _SPLS_FE_MASK | _SPLS_OE_MASK | \
			 _SPLS_PE_MASK)

/* serial port reciever command register */

#define _SPRC_ER_MASK	0x80		/* enable receiver mask */
#define _SPRC_DME_MASK	0x60		/* dma mode */
#define _SPRC_EIE_MASK	0x10		/* error interrupt enable mask */
#define _SPRC_PME_MASK	0x08		/* pause mode mask */

#define _SPRC_ER_ENABLE	_SPRC_ER_MASK	/* receiver enabled */ 
#define _SPRC_DME_DISABLE 0x00		/* dma disabled */
#define _SPRC_DME_RXRDY	0x20		/* dma disabled, RxRDY interrupt enabled */
#define _SPRC_DME_ENABLE2 0x40		/* dma enabled,receiver src channel 2 */
#define _SPRC_DME_ENABLE3 0x60		/* dma enabled,receiver src channel 3 */
#define _SPRC_PME_HARD	_SPRC_PME_MASK	/* RTS controlled by hardware */
#define _SPRC_PME_SOFT	0x00		/* RTS controlled by software */

/* serial port transmit command register */

#define _SPTC_ET_MASK	0x80		/* transmiter enable mask */
#define _SPTC_DME_MASK	0x60		/* dma mode mask */
#define _SPTC_TIE_MASK	0x10		/* empty interrupt enable mask */
#define _SPTC_EIE_MASK	0x08		/* error interrupt enable mask */
#define _SPTC_SPE_MASK	0x04		/* stop/pause mask */
#define _SPTC_TB_MASK	0x02		/* transmit break mask */

#define _SPTC_ET_ENABLE _SPTC_ET_MASK	/* transmiter enabled */
#define _SPTC_DME_DISABLE 0x00		/* transmiter disabled, TBR intr disabled */
#define _SPTC_DME_TBR	0x20		/* transmiter disabled, TBR intr enabled */
#define _SPTC_DME_CHAN_2 0x40		/* dma enabled, destination chann 2 */
#define _SPTC_DME_CHAN_3 0x60		/* dma enabled, destination chann 3 */


#ifdef __cplusplus
}
#endif

#endif /* __INC403gaSerialh */

