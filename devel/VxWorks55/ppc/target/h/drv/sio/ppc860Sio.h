/* Copyright 1984-1998 Wind River Systems, Inc. */

/*
modification history
--------------------
01g,07jan99,cn   fixed properly SPR# 22503 and SPR# 24296.
01f,22oct98,cn   added missing parenthesis in some defines (SPR# 22503).
01e,13apr98,map  moved SCC device definitions to ../multi/ppc860Cpm.h
                 renamed SCC_DEV to SCC_UART_DEV
01d,26mar98,map  added SMC, SCC, PIP defines.
01c,24may96,tpr  changed statusMode from VINT16 to VUINT16.
01b,22apr96,cah  change definitions to be offsets from a known #define
		 constant, rather than from a run-time IMMR register value.
		 Changes are required in sysSerial.c for this as well.
01a,11apr96,cah	 created.
*/

/*
 * This file contains constants for the Motorola MPC860 PowerPC
 * microcontroller chip.
 */

#ifndef __INCppc860Sioh
#define __INCppc860Sioh

#ifdef __cplusplus
extern "C" {
#endif

#ifndef	_ASMLANGUAGE

#include "sioLib.h"
#include "drv/multi/ppc860Cpm.h"
#include "drv/multi/ppc860Siu.h"

/* device and channel structures */

/* 
 * The PPC860SMC_CHAN structure defines a serial I/O channel which
 * describes the (TBD) registers for a given channel. 
 * Also the various SIO driver functions ( ioctl, txStarup, callbackInstall,
 * pollInput, polloutput ) that this channel could utilize are declared 
 * here.
 */


/*  roughly things copied in structure form from multi/m68360.h    */

/* MPC860 Dual Ported Ram addresses */

#define PPC860_DPR_SCC1(dprbase)     ((VINT32 *) ((dprbase) + 0x1c00))
#define PPC860_DPR_I2C(dprbase)      ((VINT32 *) ((dprbase) + 0x1c80))
#define PPC860_DPR_MISC(dprbase)     ((VINT32 *) ((dprbase) + 0x1cb0))
#define PPC860_DPR_IDMA1(dprbase)    ((VINT32 *) ((dprbase) + 0x1cc0))
#define PPC860_DPR_SCC2(dprbase)     ((VINT32 *) ((dprbase) + 0x1d00))
#define PPC860_DPR_SPI(dprbase)      ((VINT32 *) ((dprbase) + 0x1d80))
#define PPC860_DPR_TMR(dprbase)      ((VINT32 *) ((dprbase) + 0x1db0))
#define PPC860_DPR_IDMA2(dprbase)    ((VINT32 *) ((dprbase) + 0x1dc0))
#define PPC860_DPR_SCC3(dprbase)     ((VINT32 *) ((dprbase) + 0x1e00))
#define PPC860_DPR_SMC1(dprbase)     ((VINT32 *) ((dprbase) + 0x1e80))
#define PPC860_DPR_DSP1(dprbase)     ((VINT32 *) ((dprbase) + 0x1ec0))
#define PPC860_DPR_SCC4(dprbase)     ((VINT32 *) ((dprbase) + 0x1f00))
#define PPC860_DPR_SMC2(dprbase)     ((VINT32 *) ((dprbase) + 0x1f80))
#define PPC860_DPR_DSP2(dprbase)     ((VINT32 *) ((dprbase) + 0x1fc0))

/*
 * dual-ported parameter RAM SMC offsets                     
 * access these with SMC_RBASE(base_dpram_addr_of_this_SMC1) 
 * base_dpram_addr_of_this_SMC1 == PPC860_DPR_SMC1(dprbase)  
 * or == PPC860_DPR_SMC2(dprbase)                            
 */
#define SMC_RBASE(base)   ((VINT16 *) ((base) + 0x00)) /* Rx Buff Descr Base */
#define SMC_TBASE(base)   ((VINT16 *) ((base) + 0x02)) /* Tx Buff Descr Base */
#define SMC_RFCR(base)    ((VINT8  *) ((base) + 0x04)) /* Rx Function Code */
#define SMC_TFCR(base)    ((VINT8  *) ((base) + 0x05)) /* Tx Function Code */
#define SMC_MRBLR(base)   ((VINT16 *) ((base) + 0x06)) /* Max Rcv Buff Length */
#define SMC_RSTATE(base)  ((VINT32 *) ((base) + 0x08)) /* Rx Internal State */
#define SMC_RBPTR(base)   ((VINT16 *) ((base) + 0x10)) /* Rx Buffer Pointer */
#define SMC_TSTATE(base)  ((VINT32 *) ((base) + 0x18)) /* Tx Internal State */
#define SMC_TBPTR(base)   ((VINT16 *) ((base) + 0x20)) /* Tx Buffer Pointer */
#define SMC_MAX_IDL(base) ((VINT16 *) ((base) + 0x28)) /* Max Idle characters */
#define SMC_IDLC(base)    ((VINT16 *) ((base) + 0x2A)) /* Temp Idle counter */
#define SMC_BRKLN(base)   ((VINT16 *) ((base) + 0x2C)) /* Last Rx Break length*/
#define SMC_BRKEC(base)   ((VINT16 *) ((base) + 0x2E)) /* Rx Brk Cond counter */
#define SMC_BRKCR(base)   ((VINT16 *) ((base) + 0x30)) /* Break Count reg (Tx)*/
#define SMC_RMASK(base)   ((VINT16 *) ((base) + 0x32)) /* Temp bit mask */

/* dual ported parameter RAM PIP offsets */
                           
#define PIP_RBASE(base)   ((VINT16 *)((base) + 0x00)) /* Rx Buff Descr Base */
#define PIP_TBASE(base)   ((VINT16 *)((base) + 0x01)) /* Tx Buff Descr Base */
#define PIP_CFCR(base)    ((VINT8  *)((base) + 0x04)) /* Cent. Function Code */
#define PIP_SMASK(base)   ((VINT8  *)((base) + 0x05)) /* Status mask */

/* 
 * dual-ported parameter RAM SCC offsets                       
 * access these with SCC_RBASE(base_dpram_addr_of_this_SCC1)    
 * base_dpram_addr_of_this_SCC1 == PPC860_DPR_SCC1(dprbase)    
 * or == PPC860_DPR_SCC1(dprbase)                              
 */

#define SCC_RBASE(base)   ((VINT16 *) ((base) + 0x00)) /* Rx Buff Descr Base */
#define SCC_TBASE(base)   ((VINT16 *) ((base) + 0x02)) /* Tx Buff Descr Base */
#define SCC_RFCR(base)    ((VINT8  *) ((base) + 0x04)) /* Rx Function Code */
#define SCC_TFCR(base)    ((VINT8  *) ((base) + 0x05)) /* Tx Function Code */
#define SCC_MRBLR(base)   ((VINT16 *) ((base) + 0x06)) /* Max Rcv Buff Length */
#define SCC_RSTATE(base)  ((VINT32 *) ((base) + 0x08)) /* Rx Internal State */
#define SCC_RBPTR(base)   ((VINT16 *) ((base) + 0x10)) /* Rx Buffer Pointer */
#define SCC_TSTATE(base)  ((VINT32 *) ((base) + 0x18)) /* Tx Internal State */
#define SCC_TBPTR(base)   ((VINT16 *) ((base) + 0x20)) /* Tx Buffer Pointer */
#define SCC_RCRC(base)    ((VINT16 *) ((base) + 0x28)) /* Max Idle characters */
#define SCC_TCRC(base)    ((VINT16 *) ((base) + 0x2C)) /* Max Idle characters */
#define SCC_MAX_IDL(base) ((VINT16 *) ((base) + 0x38)) /* Max Idle characters */
#define SCC_IDLC(base)    ((VINT16 *) ((base) + 0x3A)) /* Temp Idle counter */
#define SCC_BRKCR(base)   ((VINT16 *) ((base) + 0x3C)) /* Break Count Reg (Tx)*/
#define SCC_PAREC(base)   ((VINT16 *) ((base) + 0x3E)) /* Rx Parity Err Cnt */
#define SCC_FRMEC(base)   ((VINT16 *) ((base) + 0x40)) /* Rx Frame Err Cnt */
#define SCC_NOSEC(base)   ((VINT16 *) ((base) + 0x42)) /* Receive Noise Cnt */

/*
 * dual-ported parameter RAM I2C offsets
 * access these with I2C_RPBASE(base_dpram_addr_of_I2C)
 */

#define I2C_RPBASE(base)  ((VINT16 *) ((char *)base + 0x2C))



typedef struct          /* SMC_BUF */
    {
    VUINT16	statusMode;             /* status and control */
    VINT16      dataLength;             /* length of data buffer in bytes */
    u_char *    dataPointer;            /* points to data buffer */
    } SMC_BUF;

typedef struct          /* SMC_PARAM */
    {		                /* offset description*/
    VINT16      rbase;          /* 00 Rx buffer descriptor base address */
    VINT16      tbase;          /* 02 Tx buffer descriptor base address */
    VINT8       rfcr;           /* 04 Rx function code */
    VINT8       tfcr;           /* 05 Tx function code */
    VINT16      mrblr;          /* 06 maximum receive buffer length */
    VINT32      rstate;         /* 08 Rx internal state */
    VINT32      res1;           /* 0C Rx internal data pointer */
    VINT16      rbptr;          /* 10 Rx buffer descriptor pointer */
    VINT16      res2;           /* 12 reserved/internal */
    VINT32      res3;           /* 14 reserved/internal */
    VINT32      tstate;         /* 18 Tx internal state */
    VINT32      res4;           /* 1C reserved/internal */
    VINT16      tbptr;          /* 20 Tx buffer descriptor pointer */
    VINT16      res5;           /* 22 reserved/internal */
    VINT32      res6;           /* 24 reserved/internal */
    VINT16      maxidl;         /* 28 Maximum idle characters */
    VINT16      idlc;           /* 2A temporary idle counter */
    VINT16      brkln;          /* 2C last recv break length */
    VINT16      brkec;          /* 2E recv break condition counter */
    VINT16      brkcr;          /* 30 xmit break count register */
    VINT16      r_mask;         /* 32 temporary bit mask */
    } SMC_PARAM;

typedef struct          /* SMC */
    {
    SMC_PARAM   param;                  /* SMC parameters */
    } SMC;

typedef struct          /* SMC_REG */
    {
    VINT16      smcmr;                  /* SMC Mode register */
    VINT8	res1[2];		/* reserved */
    VINT8       smce;                   /* SMC Event register */
    VINT8	res2[3];		/* reserved */
    VINT8       smcm;                   /* SMC Mask register */
    } SMC_REG;

/* SMC device descriptor */

typedef struct          /* SMC_DEV */
    {
    int                 smcNum;         /* number of SMC device (1 or 2)*/
    int                 txBdNum;        /* number of transmit buf descriptors */
    int                 rxBdNum;        /* number of receive buf descriptors */
    SMC_BUF *           txBdBase;       /* transmit BD base address */
    SMC_BUF *           rxBdBase;       /* receive BD base address */
    u_char *            txBufBase;      /* transmit buffer base address */
    u_char *            rxBufBase;      /* receive buffer base address */
    VINT32              txBufSize;      /* transmit buffer size */
    VINT32              rxBufSize;      /* receive buffer size */
    int                 txBdNext;       /* next transmit BD to fill */
    int                 rxBdNext;       /* next receive BD to read */
    volatile SMC *      pSmc;           /* SMC parameter RAM. Must point */
					/* at DPRAM area for SMC1 or SMC2 */
    volatile SMC_REG *  pSmcReg;        /* SMC registers must point at */
					/* SMCMR1 or SMCMR2 */
    VINT32              intMask;        /* interrupt acknowledge mask */
    } SMC_DEV;

/* SCC device descriptor */

typedef struct          /* SCC_UART_DEV */
    {
    int 		sccNum;		/* number of SCC device */
    int 		txBdNum;	/* number of transmit buf descriptors */
    int 		rxBdNum;	/* number of receive buf descriptors */
    SCC_BUF * 		txBdBase;	/* transmit BD base address */
    SCC_BUF * 		rxBdBase;	/* receive BD base address */
    u_char * 		txBufBase;	/* transmit buffer base address */
    u_char *		rxBufBase;	/* receive buffer base address */
    VINT32 		txBufSize;	/* transmit buffer size */
    VINT32 		rxBufSize;	/* receive buffer size */
    int			txBdNext;	/* next transmit BD to fill */
    int			rxBdNext;	/* next receive BD to read */
    volatile SCC *	pScc;		/* SCC parameter RAM */
    volatile SCC_REG *	pSccReg;	/* SCC registers */
    VINT32		intMask;	/* interrupt acknowledge mask */
    } SCC_UART_DEV;

typedef struct          	/* I2C_PARAM */
    {				/* offset description */
    VINT16 	rbase;		/* 00 Rx buffer descriptors base address */
    VINT16 	tbase;		/* 02 Tx buffer descriptors base address */
    VINT8 	rfcr;		/* 04 Rx function code */
    VINT8 	tfcr;		/* 05 Tx function code */
    VINT16 	mrblr;		/* 06 maximum receive buffer length */
    VINT32 	ris;		/* 08 Rx internal state */
    VINT32 	ridp;		/* 0c Rx internal data pointer */
    VINT16 	rbptr;		/* 10 Rx buffer descriptor pointer */
    VINT16 	ribc;		/* 12 Rx internal byte count */
    VINT32 	rt;		/* 14 Rx temp */
    VINT32 	tis;		/* 18 Tx internal state */
    VINT32 	tidp;		/* 1c Tx internal data pointer */
    VINT16 	tbptr;		/* 20 Tx buffer descriptor pointer */
    VINT16 	tibc;		/* 22 Tx internal byte count */
    VINT32 	tt;		/* 24 Tx temp */
    VINT32 	notused;	/* 28 not used */
    VINT16 	rpbase;		/* 2c relocatable parameter RAM base */
    VINT16 	reserved;	/* 2e reserved */
    } I2C_PARAM;

/* standard dual-mode serial driver header structure follows      */

typedef struct ppc860_chan	/* PPC860SMC_CHAN */
    {
    /* always goes first */
    SIO_DRV_FUNCS	*pDrvFuncs;	/* driver functions */
    
    /* callbacks */

    STATUS	(*getTxChar)();	/* pointer to a xmitr function */
    STATUS	(*putRcvChar)();/* pointer to a recvr function */
    void *	getTxArg;
    void *	putRcvArg;

    VINT16              int_vec;        /* interrupt vector number */
    VINT16              channelMode;    /* SIO_MODE                */
    int                 baudRate;
    int                 clockRate;      /* CPU clock frequency (Hz) */
    int                 bgrNum;         /* number of BRG being used */
    VINT32 *            pBaud;          /* BRG registers */
    VINT32              regBase;        /* register/DPR base address */
    SMC_DEV             uart;           /* UART SCC device */
    } PPC860SMC_CHAN;


/* serial procedures */
IMPORT	void	ppc860DevInit		(PPC860SMC_CHAN *);
IMPORT	void	ppc860Int		(PPC860SMC_CHAN *);
IMPORT	void	ppc800DevInit		(PPC860SMC_CHAN *);
IMPORT	void	ppc800Int		(PPC860SMC_CHAN *);

/*  standard dual-mode serial driver header structure follows - SCC */

typedef struct ppc860Scc_chan	/* PPC860SCC_CHAN */
    {
    /* always goes first */

    SIO_DRV_FUNCS	*pDrvFuncs;	/* driver functions */
    
    /* callbacks */

    STATUS	(*getTxChar)();	/* pointer to a xmitr function */
    STATUS	(*putRcvChar)();/* pointer to a recvr function */
    void *	getTxArg;
    void *	putRcvArg;

    VINT16              int_vec;        /* interrupt vector number */
    VINT16              channelMode;    /* SIO_MODE                */
    int                 baudRate;
    int                 clockRate;      /* CPU clock frequency (Hz) */
    int                 bgrNum;         /* number of BRG being used */
    VINT32 *            pBaud;          /* BRG registers */
    VINT32              regBase;        /* register/DPR base address */
    SCC_UART_DEV        uart;           /* UART SCC device */
    } PPC860SCC_CHAN;

/* function declarations */

IMPORT	void	ppc860SccInt		(PPC860SCC_CHAN *);
IMPORT	void	ppc860SccDevInit	(PPC860SCC_CHAN *);

/* channels */

#define PPC860_CHANNEL_A	0
#define PPC860_CHANNEL_B	1

#define PPC860SMC_N_CHANS    2	/* number of serial channels on chip */

/* SIO -- mpc860 serial channel chip -- register definitions */

/* Buffer Descriptor Pointer Definitions */

/* Receive BD status bits 16-bit value */

#define BD_RX_EMPTY_BIT		0x8000	/* buffer is empty */
#define BD_RX_WRAP_BIT		0x2000	/* last BD in chain */
#define BD_RX_INTERRUPT_BIT	0x1000	/* set interrupt when filled */
#define BD_RX_CON_MODE_BIT	0x0200	/* Continuous Mode bit */
#define BD_RX_IDLE_CLOSE_BIT	0x0100	/* Close on IDLE recv bit */
#define BD_RX_BREAK_CLOSE_BIT	0x0020	/* Close on break recv bit */
#define BD_RX_FRAME_CLOSE_BIT	0x0010	/* Close on frame error bit */
#define BD_RX_PARITY_ERROR_BIT	0x0008	/* Parity error in last byte */
#define BD_RX_OVERRUN_ERROR_BIT	0x0002	/* Overrun occurred */

/* Transmit BD status bits 16-bit value */

#define BD_TX_READY_BIT		0x8000	/* Transmit ready/busy bit */
#define BD_TX_WRAP_BIT		0x2000	/* last BD in chain */
#define BD_TX_INTERRUPT_BIT	0x1000	/* set interrupt when emptied */
#define BD_TX_CON_MODE_BIT	0x0200	/* Continuous Mode bit */
#define BD_TX_PREAMBLE_BIT	0x0100	/* send preamble sequence */

#define BD_STATUS_OFFSET	0x00	/* two bytes */
#define BD_DATA_LENGTH_OFFSET	0x02	/* two bytes */
#define	BD_BUF_POINTER_OFFSET	0x04	/* four bytes */

/*
 * MPC860 internal register/memory map (section 17 of prelim. spec)
 * note that these are offsets from the value stored in the IMMR
 * register, which is in the PowerPC special register address space
 * at register number 638
 */

/**** SIU module configuration ***/
#define MPC860_SIUMCR(base)	((VINT32 *) ((base) + 0x00))
/**** System Protection Control ***/
#define	MPC860_SYPCR(base)	((VINT32 *) ((base) + 0x04))
/**** SW watch dog timer value ***/
#define	MPC860_SWT(base)	((VINT32 *) ((base) + 0x08))
/*** Software service reg ***/
#define	MPC860_SWSR(base)	((VINT16 *) ((base) + 0x0E))
/*** Interrupt pending reg ***/
#define	MPC860_SIPEND(base)	((VINT32 *) ((base) + 0x10))
/*** Interrupt Mask reg ***/
#define	MPC860_SIMASK(base)	((VINT32 *) ((base) + 0x14))
/*** Interrupt Edge level mask ***/
#define	MPC860_SIEL(base)	((VINT32 *) ((base) + 0x18))
/*** Interrupt Vector reg ***/
#define	MPC860_SIVEC(base)	((VINT32 *) ((base) + 0x1C))
/*** Transfer error status ***/
#define	MPC860_TESR(base)	((VINT32 *) ((base) + 0x20))
/*** SDMA Config reg ***/
#define	MPC860_SDCR(base)	((VINT32 *) ((base) + 0x30))

/*** CP Interrupt vector reg ***/
#define	MPC860_CIVR(base)	((VINT16 *) ((base) + 0x930))
/*** CP Interrupt config reg ***/
#define	MPC860_CICR(base)	((VINT32 *) ((base) + 0x940))
/*** CP Interrupt pending reg ***/
#define	MPC860_CIPR(base)	((VINT32 *) ((base) + 0x944))
/*** CP Interrupt mask reg ***/
#define	MPC860_CIMR(base)	((VINT32 *) ((base) + 0x948))
/*** CP Interrupt in-service reg ***/
#define	MPC860_CISR(base)	((VINT32 *) ((base) + 0x94C))
/*** Port A data direction reg ***/
#define	MPC860_PADIR(base)	((VINT16 *) ((base) + 0x950))
/*** Port A pin assign reg ***/
#define	MPC860_PAPAR(base)	((VINT16 *) ((base) + 0x952))
/*** Port A open drain reg ***/
#define	MPC860_PAODR(base)	((VINT16 *) ((base) + 0x954))
/*** Port A data reg ***/
#define	MPC860_PADAT(base)	((VINT16 *) ((base) + 0x956))
/*** Port C data direction reg ***/
#define	MPC860_PCDIR(base)	((VINT16 *) ((base) + 0x960))
/*** Port C pin assign reg ***/
#define	MPC860_PCPAR(base)	((VINT16 *) ((base) + 0x962))
/*** Port C special options reg ***/
#define	MPC860_PCSO(base)	((VINT16 *) ((base) + 0x964))
/*** Port C data reg ***/
#define	MPC860_PCDAT(base)	((VINT16 *) ((base) + 0x966))
/*** Port C interrupt control reg ***/
#define	MPC860_PCINT(base)	((VINT16 *) ((base) + 0x968))
/*** Port D data direction reg ***/
#define	MPC860_PDDIR(base)	((VINT16 *) ((base) + 0x970))
/*** Port D pin assign reg ***/
#define	MPC860_PDPAR(base)	((VINT16 *) ((base) + 0x972))
/*** Port D data reg ***/
#define	MPC860_PDDAT(base)	((VINT16 *) ((base) + 0x976))

/*** Comm processor cmd reg ***/
#define	MPC860_CPCR(base)	((VINT16 *) ((base) + 0x9C0))
/*** RISC config reg ***/
#define	MPC860_RCCR(base)	((VINT16 *) ((base) + 0x9C4))
/*** RISC dev support status reg ***/
#define	MPC860_RMDS(base)	((VINT8  *) ((base) + 0x9C7))
/*** RISC ucode dev spt control reg ***/
#define	MPC860_RMDR(base)	((VINT32 *) ((base) + 0x9C8))
/*** RISC controller trap reg 1 ***/
#define	MPC860_RCTR1(base)	((VINT16 *) ((base) + 0x9CC))
/*** RISC controller trap reg 2 ***/
#define	MPC860_RCTR2(base)	((VINT16 *) ((base) + 0x9CE))
/*** RISC controller trap reg 3 ***/
#define	MPC860_RCTR3(base)	((VINT16 *) ((base) + 0x9D0))
/*** RISC controller trap reg 4 ***/
#define	MPC860_RCTR4(base)	((VINT16 *) ((base) + 0x9D2))
/*** RISC Timers event reg ***/
#define	MPC860_RTER(base)	((VINT16 *) ((base) + 0x9D6))
/*** RISC Timers mask reg ***/
#define	MPC860_RTMR(base)	((VINT16 *) ((base) + 0x9DA))

/*** baud rate generation register set ***/
/*** BRG1 configuration reg ***/
#define	MPC860_BRGC1(base)	((VINT32 *) ((base) + 0x9F0))
/*** BRG2 configuration reg ***/
#define	MPC860_BRGC2(base)	((VINT32 *) ((base) + 0x9F4))
/*** BRG3 configuration reg ***/
#define	MPC860_BRGC3(base)	((VINT32 *) ((base) + 0x9F8))
/*** BRG4 configuration reg ***/
#define	MPC860_BRGC4(base)	((VINT32 *) ((base) + 0x9FC))
/*** SCC1 Mode reg low ***/
#define	MPC860_GSMR_L1(base)	((VINT32 *) ((base) + 0xA00))
/*** SCC1 Mode reg High ***/
#define	MPC860_GSMR_H1(base)	((VINT32 *) ((base) + 0xA04))
/*** SCC1 Protocol Specific Mode reg ***/
#define	MPC860_PSMR1(base)	((VINT16 *) ((base) + 0xA08))
/*** SCC1 Transmit-On-Demand reg ***/
#define	MPC860_TODR1(base)	((VINT16 *) ((base) + 0xA0C))
/*** SCC1 Data Synchronization reg ***/
#define	MPC860_DSR1(base)	((VINT16 *) ((base) + 0xA0E))
/*** SCC1 Event reg ***/
#define	MPC860_SCCE1(base)	((VINT16 *) ((base) + 0xA10))
/*** SCC1 Mask reg ***/
#define	MPC860_SCCM1(base)	((VINT16 *) ((base) + 0xA14))
/*** SCC1 Status reg ***/
#define	MPC860_SCCS1(base)	((VINT8  *) ((base) + 0xA17))

/*** SCC2 Mode reg low ***/
#define	MPC860_GSMR_L2(base)	((VINT32 *) ((base) + 0xA20))
/*** SCC2 Mode reg High ***/
#define	MPC860_GSMR_H2(base)	((VINT32 *) ((base) + 0xA24))
/*** SCC2 Protocol Specific Mode reg ***/
#define	MPC860_PSMR2(base)	((VINT16 *) ((base) + 0xA28))
/*** SCC2 Transmit-On-Demand reg ***/
#define	MPC860_TODR2(base)	((VINT16 *) ((base) + 0xA2C))
/*** SCC2 Data Synchronization reg ***/
#define	MPC860_DSR2(base)	((VINT16 *) ((base) + 0xA2E))
/*** SCC2 Event reg ***/
#define	MPC860_SCCE2(base)	((VINT16 *) ((base) + 0xA30))
/*** SCC2 Mask reg ***/
#define	MPC860_SCCM2(base)	((VINT16 *) ((base) + 0xA34))
/*** SCC2 Status reg ***/
#define	MPC860_SCCS2(base)	((VINT8  *) ((base) + 0xA37))

/*** SCC3 Mode reg low ***/
#define	MPC860_GSMR_L3(base)	((VINT32 *) ((base) + 0xA40))
/*** SCC3 Mode reg High ***/
#define	MPC860_GSMR_H3(base)	((VINT32 *) ((base) + 0xA44))
/*** SCC3 Protocol Specific Mode reg ***/
#define	MPC860_PSMR3(base)	((VINT16 *) ((base) + 0xA48))
/*** SCC3 Transmit-On-Demand reg ***/
#define	MPC860_TODR3(base)	((VINT16 *) ((base) + 0xA4C))
/*** SCC3 Data Synchronization reg ***/
#define	MPC860_DSR3(base)	((VINT16 *) ((base) + 0xA4E))
/*** SCC3 Event reg ***/
#define	MPC860_SCCE3(base)	((VINT16 *) ((base) + 0xA50))
/*** SCC3 Mask reg ***/
#define	MPC860_SCCM3(base)	((VINT16 *) ((base) + 0xA54))
/*** SCC3 Status reg ***/
#define	MPC860_SCCS3(base)	((VINT8  *) ((base) + 0xA57))

/*** SCC4 Mode reg low ***/
#define	MPC860_GSMR_L4(base)	((VINT32 *) ((base) + 0xA60))
/*** SCC4 Mode reg High ***/
#define	MPC860_GSMR_H4(base)	((VINT32 *) ((base) + 0xA64))
/*** SCC4 Protocol Specific Mode reg ***/
#define	MPC860_PSMR4(base)	((VINT16 *) ((base) + 0xA68))
/*** SCC4 Transmit-On-Demand reg ***/
#define	MPC860_TODR4(base)	((VINT16 *) ((base) + 0xA6C))
/*** SCC4 Data Synchronization reg ***/
#define	MPC860_DSR4(base)	((VINT16 *) ((base) + 0xA6E))
/*** SCC4 Event reg ***/
#define	MPC860_SCCE4(base)	((VINT16 *) ((base) + 0xA70))
/*** SCC4 Mask reg ***/
#define	MPC860_SCCM4(base)	((VINT16 *) ((base) + 0xA74))
/*** SCC4 Status reg ***/
#define	MPC860_SCCS4(base)	((VINT8  *) ((base) + 0xA77))

/*** SMC1 Mode reg ***/
#define	MPC860_SMCMR1(base)	((VINT16 *) ((base) + 0xA82))
/*** SMC1 Event reg ***/
#define	MPC860_SMCE1(base)	((VINT8  *) ((base) + 0xA86))
/*** SMC1 Mask reg ***/
#define	MPC860_SMCM1(base)	((VINT8  *) ((base) + 0xA8A))
/*** SMC2 Mode reg ***/
#define	MPC860_SMCMR2(base)	((VINT16 *) ((base) + 0xA92))
/*** SMC2 or PIP Event reg ***/
#define	MPC860_SMCE2(base)	((VINT8  *) ((base) + 0xA96))
/*** SMC2 Mask reg ***/
#define	MPC860_SMCM2(base)	((VINT8  *) ((base) + 0xA9A))

/*** SPI Mode reg ***/
#define	MPC860_SPMODE(base)	((VINT16 *) ((base) + 0xAA0))
/*** SPI Event reg ***/
#define	MPC860_SPIE(base)	((VINT8  *) ((base) + 0xAA6))
/*** SPI Mask reg ***/
#define	MPC860_SPIM(base)	((VINT8  *) ((base) + 0xAAA))
/*** SPI Command reg ***/
#define	MPC860_SPCOM(base)	((VINT8  *) ((base) + 0xAAD))

/*** PIP Configuration reg ***/
#define	MPC860_PIPC(base)	((VINT16 *) ((base) + 0xAB2))
/*** PIP Timing params reg ***/
#define	MPC860_PTPR(base)	((VINT16 *) ((base) + 0xAB6))
/*** Port B data direction reg ***/
#define	MPC860_PBDIR(base)	((VINT32 *) ((base) + 0xAB8))
/*** Port B pin assign reg ***/
#define	MPC860_PBPAR(base)	((VINT32 *) ((base) + 0xABC))
/*** Port B open drain reg ***/
#define	MPC860_PBODR(base)	((VINT16 *) ((base) + 0xAC2))
/*** Port B Data register ***/
#define	MPC860_PBDAT(base)	((VINT32 *) ((base) + 0xAC4))

/*** SI Mode reg ***/
#define	MPC860_SIMODE(base)	((VINT32 *) ((base) + 0xAE0))
/*** SI Global Mode reg ***/
#define	MPC860_SIGMR(base)	((VINT8  *) ((base) + 0xAE4))
/*** SI Status reg ***/
#define	MPC860_SISTR(base)	((VINT8  *) ((base) + 0xAE6))
/*** SI Command reg ***/
#define	MPC860_SICMR(base)	((VINT8  *) ((base) + 0xAE7))
/*** SI Clock route reg ***/
#define	MPC860_SICR(base)	((VINT32 *) ((base) + 0xAEC))
/*** SI RAM pointers reg ***/
#define	MPC860_SIRP(base)	((VINT32 *) ((base) + 0xAF0))
/*** SI Routing RAM base addr ***/
#define	MPC860_SIRAM_BASE(base)	((base) + 0xC00)

/*** Data Param RAM base addr ***/
#define	MPC860_DPRAM_BASE(base)	((base) + 0x2000)

/* Register Equates By Bit */

/* Equates for Baud Rate Generation Registers */
#define MPC860_BRGC_RST		0x20000	/* 1 = reset BRG */
#define MPC860_BRGC_ENABLE_CNT	0x10000	/* 1 = enable clocks to BRG */
#define MPC860_BRGC_BRGCLK_SRC	0x00000	/* Baud Rate Gen clock src */
#define MPC860_BRGC_CLK2_SRC 	0x04000	/* CLK2 pin = BRG source */
#define MPC860_BRGC_CLK6_SRC	0x08000	/* CLK6 pin = BRG source */
#define MPC860_BRGC_AUTOBAUD	0x02000	/* 1 = autobaud on Rx */
					/* 0 = normal operation */
#define MPC860_BRGC_CLKDIV_MASK	0x01FFE	/* 12 bit value */
#define MPC860_BRGC_CLKDIV_SHIFT    0x1	/* shifted up one bit */
#define MPC860_BRGC_PRESCALE_16	0x00001	/* 1 = divide-by-16 clock */

/* Equates for SIMODE register */
#define MPC860_SIMODE_SMC2_MUX	0x80000000	/* connected to mux SI */
#define MPC860_SIMODE_SMC2_NMSI	0x00000000	/* connected to mux SI */
#define MPC860_SIMODE_SMC2_BRG1	0x00000000	/* BRG1 is clock source */
#define MPC860_SIMODE_SMC2_BRG2	0x10000000	/* BRG2 is clock source */
#define MPC860_SIMODE_SMC2_BRG3	0x20000000	/* BRG3 is clock source */
#define MPC860_SIMODE_SMC2_BRG4	0x30000000	/* BRG4 is clock source */
#define MPC860_SIMODE_SMC2_CLK5	0x40000000	/* CLK5 is clock source */
#define MPC860_SIMODE_SMC2_CLK6	0x50000000	/* CLK6 is clock source */
#define MPC860_SIMODE_SMC2_CLK7	0x60000000	/* CLK7 is clock source */
#define MPC860_SIMODE_SMC2_CLK8	0x70000000	/* CLK8 is clock source */

#define MPC860_SIMODE_SMC1_MUX	0x00008000	/* connected to mux SI */
#define MPC860_SIMODE_SMC1_NMSI	0x00000000	/* connected to mux SI */
#define MPC860_SIMODE_SMC1_BRG1	0x00000000	/* BRG1 is clock source */
#define MPC860_SIMODE_SMC1_BRG2	0x00001000	/* BRG2 is clock source */
#define MPC860_SIMODE_SMC1_BRG3	0x00002000	/* BRG3 is clock source */
#define MPC860_SIMODE_SMC1_BRG4	0x00003000	/* BRG4 is clock source */
#define MPC860_SIMODE_SMC1_CLK1	0x00004000	/* CLK1 is clock source */
#define MPC860_SIMODE_SMC1_CLK2	0x00005000	/* CLK2 is clock source */
#define MPC860_SIMODE_SMC1_CLK3	0x00006000	/* CLK3 is clock source */
#define MPC860_SIMODE_SMC1_CLK4	0x00007000	/* CLK4 is clock source */

#define MPC860_SIMODE_TDMB_NORM	0x00000000	/* normal operation */
#define MPC860_SIMODE_TDMB_ECHO	0x04000000	/* auto echo */
#define MPC860_SIMODE_TDMB_LOOP	0x08000000	/* internal loopback */
#define MPC860_SIMODE_TDMB_LCTR	0x0C000000	/* loopback control */

/*
 * Equates for SMCE SM UART-mode Event Register
 * writing a one to these location clears an event/interrupt
 */
#define MPC860_SMCE_UART_BRK_EVENT	0x10	/* break char received */
#define MPC860_SMCE_UART_BSY_EVENT	0x04	/* char discarded no bufs */
#define MPC860_SMCE_UART_TX_EVENT	0x02	/* char transmitted */
#define MPC860_SMCE_UART_RX_EVENT	0x01	/* char received  */
#define MPC860_SMCE_UART_ALL_EVENTS ( \
	MPC860_SMCE_UART_BRK_EVENT    | \
	MPC860_SMCE_UART_BSY_EVENT    | \
	MPC860_SMCE_UART_TX_EVENT     | \
	MPC860_SMCE_UART_RX_EVENT      )
	

/* 
 * Equates for SMCM SM UART-mode Mask Register
 * writing a one to these locations masks an event/interrupt
 */
#define MPC860_SMCM_UART_BRK_MASK	0x10	/* break char received */
#define MPC860_SMCM_UART_BSY_MASK	0x04	/* char discarded no bufs */
#define MPC860_SMCM_UART_TX_MASK	0x02	/* char transmitted */
#define MPC860_SMCM_UART_RX_MASK	0x01	/* char received  */

/* Equates for CICR register CP interrupt configuration register */
#define MPC860_CICR_IRL_LEVEL_0	0x00000000	/* highest interrupt level */
#define MPC860_CICR_IRL_LEVEL_1	0x00020000
#define MPC860_CICR_IRL_LEVEL_2	0x00040000
#define MPC860_CICR_IRL_LEVEL_3	0x00060000
#define MPC860_CICR_IRL_LEVEL_4	0x00080000	/* standard value */
#define MPC860_CICR_IRL_LEVEL_5	0x000A0000
#define MPC860_CICR_IRL_LEVEL_6	0x000C0000
#define MPC860_CICR_IRL_LEVEL_7	0x000E0000	/* lowest */
#define MPC860_CICR_HP_SRC_STD	0x0001F000	/* highest priority int */
#define MPC860_CICR_MASTER_IEN	0x00000080	/* master interrupt enable */

/* Equates for CIMR register CP interrupt mask register */
#define MPC860_CIMR_SCC1_MASK	0x40000000	/* mask SCC1 interrupt */
#define MPC860_CIMR_SCC2_MASK	0x20000000	/* mask SCC2 interrupt */
#define MPC860_CIMR_SMC1_MASK	0x00000010	/* mask SMC1 interrupt */
#define MPC860_CIMR_SMC2_MASK	0x00000008	/* mask SMC2 interrupt */

/* Equates for RFCR register receive function code register */
#define MPC860_RFCR_DEC_LE	0x00	/* DEC little endian mode */
#define MPC860_RFCR_PPC_LE	0x08	/* PPC little endian mode */
					/* reverse transmission order of */
					/* bytes compared to DEC/Intel mode */
#define MPC860_RFCR_MOT_BE	0x18	/* big endian mode */

/* Equates for TFCR register transmission function code register */
#define MPC860_TFCR_DEC_LE	0x00	/* DEC little endian mode */
#define MPC860_TFCR_PPC_LE	0x08	/* PPC little endian mode */
					/* reverse transmission order of */
					/* bytes compared to DEC/Intel mode */
#define MPC860_TFCR_MOT_BE	0x18	/* big endian mode */

/* Equates for SMC Mode Register SMCMR */
#define MPC860_SMCMR_CLEN_MASK	0x7800	/* # bits in char minus one */
					/* set to # data bits + # parity */
					/* bits + # stop bits */
#define MPC860_SMCMR_CLEN_STD	0x4800	/* for 8-N-1 std serial I/O */
#define MPC860_SMCMR_STOPLEN_1	0x0000	/* stop length = 1 */
#define MPC860_SMCMR_STOPLEN_2	0x0400	/* stop length = 2 */
#define MPC860_SMCMR_NO_PARITY	0x0000	/* parity disabled */
#define MPC860_SMCMR_PARITY	0x0200	/* parity enabled */
#define MPC860_SMCMR_PARITY_ODD	0x0000	/* odd parity if enabled */
#define MPC860_SMCMR_PARITY_EVEN 0x0100	/* even parity if enabled */
#define MPC860_SMCMR_GCI_MODE	0x0000	/* GCI or SCIT mode */
#define MPC860_SMCMR_UART_MODE	0x0020	/* UART mode */
#define MPC860_SMCMR_TRANS_MODE	0x0030	/* totally transparent mode */
#define MPC860_SMCMR_NORM_MODE	0x0000	/* normal operation mode */
#define MPC860_SMCMR_LOOP_MODE	0x0004	/* local loopback mode */
#define MPC860_SMCMR_ECHO_MODE	0x0008	/* echo mode */
#define MPC860_SMCMR_TX_ENABLE	0x0002	/* enable transmitter */
#define MPC860_SMCMR_TX_DISABLE	0x0000	/* disable transmitter */
#define MPC860_SMCMR_RX_ENABLE	0x0001	/* enable receiver */
#define MPC860_SMCMR_RX_DISABLE	0x0000	/* disable receiver */

#define MPC860_SMCMR_STD_MODE  ( \
	MPC860_SMCMR_CLEN_STD  | \
	MPC860_SMCMR_STOPLEN_1 | \
	MPC860_SMCMR_NO_PARITY | \
	MPC860_SMCMR_UART_MODE | \
	MPC860_SMCMR_NORM_MODE )
#define MPC860_SCMCR_STD_MODE_ENABLED ( \
	MPC860_SMCMR_STD_MODE  | \
	MPC860_SMCMR_TX_ENABLE | \
	MPC860_SMCMR_RX_ENABLE        )

#endif	/* _ASMLANGUAGE */

#ifdef __cplusplus
}
#endif

#endif /* __INCppc860Sioh */
