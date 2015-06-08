/* m8260Scc.h - Motorola MPC8260 Serial Communications Controller header file */

/* Copyright 1984-1999 Wind River Systems, Inc. */

/*
modification history
--------------------
01a,12sep99,ms_	 created from m8260Cpm.h, 01d.
*/

/*
 * This file contains constants for the Serial Communications Controllers
 * (SCCs) in the Motorola MPC8260 PowerQUICC II integrated Communications 
 * Processor
 */

#ifndef __INCm8260Scch
#define __INCm8260Scch

#ifdef __cplusplus
extern "C" {
#endif
    
#ifndef M8260ABBREVIATIONS
#define M8260ABBREVIATIONS
   
#ifdef  _ASMLANGUAGE
#define CAST(x)
#else /* _ASMLANGUAGE */
typedef volatile UCHAR VCHAR;   /* shorthand for volatile UCHAR */
typedef volatile INT32 VINT32; /* volatile unsigned word */
typedef volatile INT16 VINT16; /* volatile unsigned halfword */
typedef volatile INT8 VINT8;   /* volatile unsigned byte */
typedef volatile UINT32 VUINT32; /* volatile unsigned word */
typedef volatile UINT16 VUINT16; /* volatile unsigned halfword */
typedef volatile UINT8 VUINT8;   /* volatile unsigned byte */
#define CAST(x) (x)
#endif  /* _ASMLANGUAGE */

#endif /* M8260ABBREVIATIONS */

/*
 * MPC8260 internal register/memory map (section 17 of prelim. spec)
 * note that these are offsets from the value stored in the IMMR
 * register. Also note that in the MPC8260, the IMMR is not a special
 * purpose register, but it is memory mapped.
 */
 

/* SCC Parameter Ram */

#define M8260_SCC_PRAM_BASE		0x00008000
#define M8260_SCC_PRAM_OFFSET_NEXT_PRAM 0x00000100

#define M8260_SCC_PRAM_RBASE		0x00000000
#define M8260_SCC_PRAM_TBASE		0x00000002
#define M8260_SCC_PRAM_RFCR		0x00000004
#define M8260_SCC_PRAM_TFCR		0x00000005
#define M8260_SCC_PRAM_MRBLR		0x00000006

#define M8260_SCC_UART_PRAM_PAREC	0x0000003E
#define M8260_SCC_UART_PRAM_FRMEC	0x00000040
#define M8260_SCC_UART_PRAM_NOSEC	0x00000042
#define M8260_SCC_UART_PRAM_BRKEC	0x00000044


/* SCC - Serial Communication Controller */

#define M8260_SCC_BASE  		0x00011A00
#define M8260_SCC_OFFSET_NEXT_SCC  	0x00000020

#define M8260_GSMR_L_OFFSET		0x00000000
#define M8260_GSMR_H_OFFSET		0x00000004
#define M8260_PSMR_OFFSET		0x00000008
#define M8260_TODR_OFFSET		0x0000000C
#define M8260_DSR_OFFSET		0x0000000E
#define M8260_SCCE_OFFSET		0x00000010
#define M8260_SCCM_OFFSET		0x00000014
#define M8260_SCCS_OFFSET		0x00000017


/* General SCC Mode Register definitions  */

#define M8260_SCC_GSMRL_HDLC		0x00000000	/* HDLC mode */

/* AppleTalk mode (LocalTalk) */

#define M8260_SCC_GSMRL_APPLETALK	0x00000002	

/* SS7 mode (microcode) */

#define M8260_SCC_GSMRL_SS7		0x00000003	
#define M8260_SCC_GSMRL_UART		0x00000004	/* UART mode */

/* Profi-Bus mode (microcode) */

#define M8260_SCC_GSMRL_PROFI_BUS	0x00000005	

/* async HDLC mode (microcode)*/

#define M8260_SCC_GSMRL_ASYNC_HDLC	0x00000006	
#define M8260_SCC_GSMRL_V14		0x00000007	/* V.14 mode */
#define M8260_SCC_GSMRL_BISYNC		0x00000008	/* BISYNC mode */

/* DDCMP mode (microcode) */

#define M8260_SCC_GSMRL_DDCMP		0x00000009	

/* enable bits for transmit and receive */

#define M8260_SCC_GSMRL_ENT		0x00000010	/* enable transmitter */
#define M8260_SCC_GSMRL_ENR		0x00000020	/* enable receiver */

/* local loopback mode */

#define M8260_SCC_GSMRL_LOOPBACK	0x00000040	

/* automatic echo mode */

#define M8260_SCC_GSMRL_ECHO		0x00000080	

/* transmitter encoding method*/

#define M8260_SCC_GSMRL_TENC		0x00000700	

/* receiver encoding method */

#define M8260_SCC_GSMRL_RENC		0x00003800	

/* receive DPLL clock x8 */

#define M8260_SCC_GSMRL_RDCR_X8		0x00004000	

/* receive DPLL clock x16 */

#define M8260_SCC_GSMRL_RDCR_X16	0x00008000	

/* receive DPLL clock x32 */

#define M8260_SCC_GSMRL_RDCR_X32	0x0000c000	

/* transmit DPLL clock x8 */

#define M8260_SCC_GSMRL_TDCR_X8		0x00010000	

/* transmit DPLL clock x16 */

#define M8260_SCC_GSMRL_TDCR_X16	0x00020000	

/* transmit DPLL clock x32 */

#define M8260_SCC_GSMRL_TDCR_X32	0x00030000	


#define M8260_SCC_GSMRL_TEND	0x00040000	/* transmitter frame ending */
#define M8260_SCC_GSMRL_TPP_00	0x00180000	/* Tx preamble pattern = 00 */
#define M8260_SCC_GSMRL_TPP_10	0x00080000	/* Tx preamble pattern = 10 */
#define M8260_SCC_GSMRL_TPP_01	0x00100000	/* Tx preamble pattern = 01 */
#define M8260_SCC_GSMRL_TPP_11	0x00180000	/* Tx preamble pattern = 11 */

/* no Tx preamble (default) */

#define M8260_SCC_GSMRL_TPL_NONE	0x00000000	

/* Tx preamble = 1 byte */

#define M8260_SCC_GSMRL_TPL_8	0x00200000	
#define M8260_SCC_GSMRL_TPL_16	0x00400000	/* Tx preamble = 2 bytes */
#define M8260_SCC_GSMRL_TPL_32	0x00600000	/* Tx preamble = 4 bytes */
#define M8260_SCC_GSMRL_TPL_48	0x00800000	/* Tx preamble = 6 bytes */
#define M8260_SCC_GSMRL_TPL_64	0x00a00000	/* Tx preamble = 8 bytes */
#define M8260_SCC_GSMRL_TPL_128	0x00c00000	/* Tx preamble = 16 bytes */
#define M8260_SCC_GSMRL_TINV	0x01000000	/* DPLL transmit input invert */
#define M8260_SCC_GSMRL_RINV	0x02000000	/* DPLL receive input invert */
#define M8260_SCC_GSMRL_TSNC	0x0c000000	/* transmit sense */
#define M8260_SCC_GSMRL_TCI	0x10000000	/* transmit clock invert */
#define M8260_SCC_GSMRL_EDGE	0x60000000	/* adjustment edge +/- */

#define M8260_SCC_GSMRH_RSYN	0x00000001	/* receive sync timing*/
#define M8260_SCC_GSMRH_RTSM	0x00000002	/* RTS* mode */
#define M8260_SCC_GSMRH_SYNL	0x0000000c	/* sync length */
#define M8260_SCC_GSMRH_TXSY	0x00000010	/* transmitter/receiver sync */
#define M8260_SCC_GSMRH_RFW	0x00000020	/* Rx FIFO width */
#define M8260_SCC_GSMRH_TFL	0x00000040	/* transmit FIFO length */
#define M8260_SCC_GSMRH_CTSS	0x00000080	/* CTS* sampling */
#define M8260_SCC_GSMRH_CDS	0x00000100	/* CD* sampling */
#define M8260_SCC_GSMRH_CTSP	0x00000200	/* CTS* pulse */
#define M8260_SCC_GSMRH_CDP	0x00000400	/* CD* pulse */
#define M8260_SCC_GSMRH_TTX	0x00000800	/* transparent transmitter */
#define M8260_SCC_GSMRH_TRX	0x00001000	/* transparent receiver */
#define M8260_SCC_GSMRH_REVD	0x00002000	/* reverse data */
#define M8260_SCC_GSMRH_TCRC	0x0000c000	/* transparent CRC */
#define M8260_SCC_GSMRH_GDE	0x00010000	/* glitch detect enable */

/* SCC UART Protocol Specific Mode Register definitions */

#define M8260_SCC_UART_PSMR_TPM_ODD	0x0000	/* odd parity mode (Tx) */
#define M8260_SCC_UART_PSMR_TPM_LOW	0x0001	/* low parity mode (Tx) */
#define M8260_SCC_UART_PSMR_TPM_EVEN	0x0002	/* even parity mode (Tx) */
#define M8260_SCC_UART_PSMR_TPM_HIGH	0x0003	/* high parity mode (Tx) */
#define M8260_SCC_UART_PSMR_RPM_ODD	0x0000	/* odd parity mode (Rx) */
#define M8260_SCC_UART_PSMR_RPM_LOW	0x0004	/* low parity mode (Rx) */
#define M8260_SCC_UART_PSMR_RPM_EVEN	0x0008	/* even parity mode (Rx) */
#define M8260_SCC_UART_PSMR_RPM_HIGH	0x000c	/* high parity mode (Rx) */
#define M8260_SCC_UART_PSMR_PEN		0x0010	/* parity enable */
#define M8260_SCC_UART_PSMR_DRT		0x0040	/* disable Rx while Tx */
#define M8260_SCC_UART_PSMR_SYN		0x0080	/* synchronous mode */
#define M8260_SCC_UART_PSMR_RZS		0x0100	/* receive zero stop bits */
#define M8260_SCC_UART_PSMR_FRZ		0x0200	/* freeze transmission */
#define M8260_SCC_UART_PSMR_UM_NML	0x0000	/* noraml UART operation */
#define M8260_SCC_UART_PSMR_UM_MULT_M	0x0400	/* multidrop non-auto mode */
#define M8260_SCC_UART_PSMR_UM_MULT_A	0x0c00	/* multidrop automatic mode */
#define M8260_SCC_UART_PSMR_CL_5BIT	0x0000	/* 5 bit character length */
#define M8260_SCC_UART_PSMR_CL_6BIT	0x1000	/* 6 bit character length */
#define M8260_SCC_UART_PSMR_CL_7BIT	0x2000	/* 7 bit character length */
#define M8260_SCC_UART_PSMR_CL_8BIT	0x3000	/* 8 bit character length */
#define M8260_SCC_UART_PSMR_SL		0x4000	/* 1 or 2 bit stop length */
#define M8260_SCC_UART_PSMR_FLC		0x8000	/* flow control */
	
/* SCC UART Event and Mask Register definitions */

#define M8260_SCC_UART_SCCX_RX 		0x0001	/* buffer received */
#define M8260_SCC_UART_SCCX_TX 		0x0002	/* buffer transmitted */
#define M8260_SCC_UART_SCCX_BSY 	0x0004	/* busy condition */
#define M8260_SCC_UART_SCCX_CCR 	0x0008	/* control character received */
#define M8260_SCC_UART_SCCX_BRK_S 	0x0020 	/* break start */
#define M8260_SCC_UART_SCCX_BRK_E 	0x0040	/* break end */
#define M8260_SCC_UART_SCCX_GRA 	0x0080	/* graceful stop complete */
#define M8260_SCC_UART_SCCX_IDL		0x0100	/* idle sequence stat changed */
#define M8260_SCC_UART_SCCX_AB  	0x0200	/* autobaud lock */
#define M8260_SCC_UART_SCCX_GL_T 	0x0800	/* glitch on Tx */
#define M8260_SCC_UART_SCCX_GL_R 	0x1000	/* glitch on Rx */
#define M8260_SCC_UART_SCCX_ALL_EVENTS  0xFFFF	/* all events */

/* SCC UART Receive Buffer Descriptor definitions */

#define M8260_SCC_UART_RX_BD_CD	0x0001		/* carrier detect loss */
#define M8260_SCC_UART_RX_BD_OV	0x0002		/* receiver overrun */
#define M8260_SCC_UART_RX_BD_PR	0x0008		/* parity error */
#define M8260_SCC_UART_RX_BD_FR	0x0010		/* framing error */
#define M8260_SCC_UART_RX_BD_BR	0x0020		/* break received */
#define M8260_SCC_UART_RX_BD_AM	0x0080		/* address match */
#define M8260_SCC_UART_RX_BD_ID	0x0100		/* buf closed on IDLES */
#define M8260_SCC_UART_RX_BD_CM	0x0200		/* continous mode */
#define M8260_SCC_UART_RX_BD_ADDR	0x0400	/* buffer contains address */
#define M8260_SCC_UART_RX_BD_CNT	0x0800	/* control character */
#define M8260_SCC_UART_RX_BD_INT	0x1000	/* interrupt generated */
#define M8260_SCC_UART_RX_BD_WRAP	0x2000	/* wrap back to first BD */
#define M8260_SCC_UART_RX_BD_EMPTY	0x8000	/* buffer is empty */

/* SCC UART Transmit Buffer Descriptor definitions */

#define M8260_SCC_UART_TX_BD_CT	0x0001		/* cts was lost during tx */
#define M8260_SCC_UART_TX_BD_NS	0x0080		/* no stop bit transmitted */
#define M8260_SCC_UART_TX_BD_PREAMBLE	0x0100	/* enable preamble */
#define M8260_SCC_UART_TX_BD_CM	0x0200		/* continous mode */
#define M8260_SCC_UART_TX_BD_ADDR	0x0400	/* buffer contains address */
#define M8260_SCC_UART_TX_BD_CTSR	0x0800	/* normal cts error reporting */
#define M8260_SCC_UART_TX_BD_INT	0x1000	/* interrupt generated */
#define M8260_SCC_UART_TX_BD_WRAP	0x2000	/* wrap back to first BD */
#define M8260_SCC_UART_TX_BD_READY	0x8000	/* buffer is being sent */

/* Miscellaneous SCC UART definitions */

#define M8260_SCC_POLL_OUT_DELAY	10000	/* polled mode delay */

#ifdef __cplusplus
}
#endif

#endif /* __INCm8260Scch */
