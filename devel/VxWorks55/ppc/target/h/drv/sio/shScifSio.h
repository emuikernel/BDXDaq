/* shScifSio.h - header file for Hitachi SH on-chip SCIF serial driver */

/* Copyright 1984-1999 Wind River Systems, Inc. */

/*
modification history
--------------------
01n,06mar02,h_k  added lsr2 and ss2r2 to SCIF_CHAN and SCIF_LSR2_ORER and
                 SCIF_SS2R2_ORER definitions (SPR #73777).
01m,15nov00,csi  modification to optimize SMR  and BRR define
01l,30aug00,zl   added 30MHz PCLK support for SH7600
01k,04aug00,rsh  Add 33.33Mhz frequency defines for sh7729
01j,31may00,frf  SH7751 for T2
01i,05mar99,hk   Sorted out CPU conditional order.
01h,26nov98,hk   Added support for SH7750.
01g,10aug98,jmb  reg size fix.
01f,10aug98,jmb  bugfix.
01e,10aug98,jmb  Added support for Manta-DSP.
01d,30jul98,jmb  Added support for Manta-SH3 frequencies.
01c,30may98,jmb  Added options to support multiple data formats.
01b,23may98,jmb  Fixed decl's and FCR2 macros, transmit triggers.
01a,20may98,jmb  Created.
*/

#ifndef __INCshScifSioh
#define __INCshScifSioh

#ifdef __cplusplus
extern "C" {
#endif

#include "sioLib.h"
#include "drv/sio/shSciSio.h"

#if (CPU==SH7750)
#define SZ_SMR2 UINT16
#define SZ_SCR2 UINT16
#define SZ_FCR2 UINT16
#else
#define SZ_SMR2 UINT8
#define SZ_SCR2 UINT8
#define SZ_FCR2 UINT8
#endif

/* device channel structure */

typedef struct
    {
    /* always goes first */

    SIO_DRV_FUNCS *	pDrvFuncs;	/* driver functions */

    /* callbacks */

    STATUS		(*getTxChar) ();
    STATUS		(*putRcvChar) ();
    void *		getTxArg;
    void *		putRcvArg;

    /* info */

    int			mode;		/* current mode (interrupt or poll) */
    int			baud;		/* baud rate */
    UINT		options;	/* data format options */

    /* register addresses */

    volatile SZ_SMR2 *	smr2;		/* serial mode register */
    volatile UINT8 *	brr2;		/* bit rate register */
    volatile SZ_SCR2 *	scr2;		/* serial control register */
    volatile UINT8 *	sfdr2;		/* transmit FIFO data register */
    volatile UINT16 *	ssr2;		/* serial status register */
    volatile UINT8 *	frdr2;		/* receive FIFO data register */
    volatile SZ_FCR2 *	fcr2;		/* FIFO control register */
    volatile UINT16 *	fdr2;		/* data count set register */
    volatile UINT16 *	lsr2;		/* line status register */
    volatile UINT8 *	ss2r2;		/* serial status 2 register */
    } SCIF_CHAN;

/* Maximum FIFO length */

#define SCIF_FIFO_LEN		16

/* Bit values for FIFO control register */

#define SCIF_FCR2_RTRG		0xc0	/* Read mask for receive trigger */
#define SCIF_FCR2_RTRG_1	0x00    /* 1 char in FIFO triggers receive */
#define SCIF_FCR2_RTRG_4	0x40    /* 4 chars in FIFO triggers receive */
#define SCIF_FCR2_RTRG_8	0x80    /* 8 chars in FIFO triggers receive */
#define SCIF_FCR2_RTRG_14	0xc0    /* 14 chars in FIFO triggers receive */

#define SCIF_FCR2_TTRG		0x30    /* Read mask for transmit trigger */
#define SCIF_FCR2_TTRG_8	0x00    /* */
#define SCIF_FCR2_TTRG_4	0x10    /* */
#define SCIF_FCR2_TTRG_2	0x20    /* */
#define SCIF_FCR2_TTRG_1	0x30    /* */

#define SCIF_FCR2_MCE		0x8     /* Enable RTS/CTS signals */
#define SCIF_FCR2_TFRST		0x4	/* Clear transmit FIFO on reset */
#define SCIF_FCR2_RFRST		0x2     /* Clear receive RIFO on reset */
#define SCIF_FCR2_LOOP		0x1     /* Loopback mode */

/* bit values for ssr (serial status register) */

#define	SCIF_SSR2_ER		0x80	/* framing or parity error */
#define	SCIF_SSR2_TEND		0x40	/* tranmission end indicator */
#define	SCIF_SSR2_TDFE		0x20	/* transmit data FIFO empty */
#define	SCIF_SSR2_BRK		0x10	/* break signal received */
#define	SCIF_SSR2_FER		0x08	/* framing error */
#define	SCIF_SSR2_PER		0x04	/* parity error */
#define	SCIF_SSR2_RDF		0x02	/* receive FIFO full */
#define	SCIF_SSR2_DR		0x01	/* data ready in receive FIFO */

/* bit value for lsr (line status register) */

#define SCIF_LSR2_ORER		0x1	/* overrun error */

/* bit value for ss2r (serial status 2 register) */

#define SCIF_SS2R2_ORER		0x1	/* overrun error */

/* function prototypes */

#if defined(__STDC__)

extern void shScifDevInit (SCIF_CHAN *pChan); 
extern void shScifIntRcv (SCIF_CHAN *pChan);
extern void shScifIntTx (SCIF_CHAN *pChan);
extern void shScifIntErr (SCIF_CHAN *pChan);

#else /* __STDC__ */

extern void shScifDevInit ();
extern void shScifIntRcv ();
extern void shScifIntTx ();
extern void shScifIntErr ();

#endif /* __STDC__ */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif  /* __INCshScifSioh */

