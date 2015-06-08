/* shSciSio.h - header file for Hitachi SH on-chip SCI serial driver */

/* Copyright 1996-2001 Wind River Systems, Inc. */

/*
modification history
--------------------
01p,15feb01,hk   add #ifdef SCI_TYPE_SH7604 to adjust baud rate for SH7604.
01o,09nov00,csi  modification to optimize SMR  and BRR define
01n,30aug00,zl   added 30MHz PCLK support for SH7600
01m,20aug00,hk   merged SH7040 to SH7600.
01l,04aug00,rsh  Add frequency defines for sh7729
01k,31may00,frf  SH7751 for T2
01j,29dec98,hk   merged for SH7750,SH7729,SH7700,SH7600,SH7410,SH7040,SH7000.
01i,10aug98,jmb  add 57600 baud for Manta-DSP.
01h,09jul98,jmb  add support for Manta-SH3 frequencies.
01g,01jun98,jmb  added "options" to support SIO_HW_OPTS_GET.
01f,19may98,jmc  added support for SH-DSP and SH3-DSP.
01g,18sep98,hk   fixed SH7750 support for 33.33MHz PCLK.
01f,02jul98,st   added SH7750 support.
01e,14jan98,jmc  added baud rate table entries for 20/10/5MHz.
01d,25apr97,hk   changed SH704X to SH7040.
01c,21mar97,hk   added SCI_BAUD typedef. enhanced defs for smr and brr.
01b,10mar97,hk   renamed this header as shSciSio.h from shSio.h. changed 
                 SH_CHAN to SCI_CHAN. deleted SH_DUSART. changed function
                 prototype names to shSciXyz. changed definition layout.
01a,16dec96,kn   derived from templateSio.h
*/

#ifndef __INCshSciSioh
#define __INCshSciSioh

#ifdef __cplusplus
extern "C" {
#endif

#include "sioLib.h"


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

    /* register addresses */

    volatile UINT8 *	smr;		/* serial mode register */
    volatile UINT8 *	brr;		/* bit rate register */
    volatile UINT8 *	scr;		/* serial control register */
    volatile UINT8 *	tdr;		/* transmit data register */
    volatile UINT8 *	ssr;		/* serial status register */
    volatile UINT8 *	rdr;		/* receive data register */

    int			mode;		/* current mode (interrupt or poll) */
    int			baud;		/* baud rate */
    UINT             options;           /* data format */
    } SCI_CHAN;


/* entry structure for baud-rate table */

typedef struct
    {
    int   rate;		/* baud-rate to be set */
    UINT8 cksVal;	/* clock select bits for smr */
    UINT8 brrVal;	/* value to set in brr */
    } SCI_BAUD;


/* bit values for smr (serial mode register) */

#define	SCI_SMR_ASYNC		0x00	/* async mode */
#define	SCI_SMR_SYNC		0x80	/* synchronous mode */
#define	SCI_SMR_8_BITS		0x00	/* 8 bits data */
#define	SCI_SMR_7_BITS		0x40	/* 7 bits data */
#define	SCI_SMR_PAR_DIS		0x00	/* parity disable */
#define	SCI_SMR_PAR_EN		0x20	/* parity enable */
#define	SCI_SMR_PAR_EVEN	0x00	/* even parity */
#define	SCI_SMR_PAR_ODD		0x10	/* odd parity  */
#define	SCI_SMR_1_STOP		0x00	/* 1 stop bit */
#define	SCI_SMR_2_STOP		0x08	/* 2 stop bit */
#define	SCI_SMR_MP_DIS		0x00	/* multiprocessor format disable */
#define	SCI_SMR_MP_EN		0x04	/* multiprocessor format enable */
#define	SCI_SMR_CKS_MASK	0xfc	/* mask for clock select bits */

#ifdef SYS_PCLK_FREQ
#  define SCI_FREQ		SYS_PCLK_FREQ
#else
#  ifdef SCI_TYPE_SH7604
#    define SCI_FREQ		(SYS_CPU_FREQ / 4)
#  else /* SH704x, SH705x */
#    define SCI_FREQ		SYS_CPU_FREQ
#  endif
#endif

/*
 * To calculate BRR value, the following formula used. 
 *   BRR = (FREQ / (32 * 4^n) * BaudRate)) - 1 
 *
 * In the following macros are used
 *    SMR_VALUE is n
 *    CLK_DIVIDE(y) 4^n
 *
 * In the BRR_VALUE macro the extra mutiplication, division and addition 
 * is used to get correct rounding.
 */

#define SMR_VALUE(x)		(((SCI_FREQ/(64*32*(x))) >= 64) ? 3 :        \
			 	 ((SCI_FREQ/(64*32*(x))) >= 16) ? 2 :        \
			 	 ((SCI_FREQ/(64*32*(x))) >=  4) ? 1 : 0)

#define CLK_DIVIDE(y)		(((SCI_FREQ/(64*32*(y))) >= 64) ? 64 :       \
			 	 ((SCI_FREQ/(64*32*(y))) >= 16) ? 16 :       \
			 	 ((SCI_FREQ/(64*32*(y))) >=  4) ?  4 : 1)

#define BRR_FORMULA(s)		(((10*SCI_FREQ)/(32*CLK_DIVIDE(s)*(s))-5)/10)

#define BRR_VALUE(z)		((BRR_FORMULA(z) > 255) ? 0 : BRR_FORMULA(z))

#define SCI_SMR_CKS_50          SMR_VALUE(50)
#define SCI_SMR_CKS_75          SMR_VALUE(75)
#define SCI_SMR_CKS_110         SMR_VALUE(110)
#define SCI_SMR_CKS_134         SMR_VALUE(134)
#define SCI_SMR_CKS_150         SMR_VALUE(150)
#define SCI_SMR_CKS_200         SMR_VALUE(200)
#define SCI_SMR_CKS_300         SMR_VALUE(300)
#define SCI_SMR_CKS_600         SMR_VALUE(600)
#define SCI_SMR_CKS_1200        SMR_VALUE(1200)
#define SCI_SMR_CKS_1800        SMR_VALUE(1800)
#define SCI_SMR_CKS_2400        SMR_VALUE(2400)
#define SCI_SMR_CKS_3600        SMR_VALUE(3600)
#define SCI_SMR_CKS_4800        SMR_VALUE(4800)
#define SCI_SMR_CKS_7200        SMR_VALUE(7200)
#define SCI_SMR_CKS_9600        SMR_VALUE(9600)
#define SCI_SMR_CKS_19200       SMR_VALUE(19200)
#define SCI_SMR_CKS_31250       SMR_VALUE(31250)
#define SCI_SMR_CKS_38400       SMR_VALUE(38400)
#define SCI_SMR_CKS_57600       SMR_VALUE(57600)
#define SCI_SMR_CKS_115200      SMR_VALUE(115200)
#define SCI_SMR_CKS_500000      SMR_VALUE(500000)


/* bit values for brr (bit rate register) */

#define SCI_BRR_50      	BRR_VALUE(50)
#define SCI_BRR_75      	BRR_VALUE(75)
#define SCI_BRR_110     	BRR_VALUE(110)
#define SCI_BRR_134     	BRR_VALUE(134)
#define SCI_BRR_150     	BRR_VALUE(150)
#define SCI_BRR_200     	BRR_VALUE(200)
#define SCI_BRR_300   		BRR_VALUE(300) 
#define SCI_BRR_600   		BRR_VALUE(600) 
#define SCI_BRR_1200  		BRR_VALUE(1200) 
#define SCI_BRR_1800  		BRR_VALUE(1800) 
#define SCI_BRR_2400  		BRR_VALUE(2400) 
#define SCI_BRR_3600  		BRR_VALUE(3600) 
#define SCI_BRR_4800  		BRR_VALUE(4800) 
#define SCI_BRR_7200  		BRR_VALUE(9600) 
#define SCI_BRR_9600  		BRR_VALUE(9600) 
#define SCI_BRR_19200 		BRR_VALUE(19200) 
#define SCI_BRR_31250 		BRR_VALUE(31250) 
#define SCI_BRR_38400 		BRR_VALUE(38400) 
#define SCI_BRR_57600   	BRR_VALUE(57600) 
#define SCI_BRR_115200   	BRR_VALUE(115200) 
#define SCI_BRR_500000   	BRR_VALUE(500000) 


/* bit values for scr (serial control register) */

#define	SCI_SCR_TIE		0x80	/* transmit interrupt enable  */
#define	SCI_SCR_RIE		0x40	/* receive interrupt enable  */
#define	SCI_SCR_TXE		0x20	/* transmit enable */
#define	SCI_SCR_RXE		0x10	/* receive enable */
#ifdef JMC_NEVER  /* JMC Can this be removed ! */
#define	SCI_SCR_MPIE		0x08	/* multiprocessor interrupt enable */
#endif
#define	SCI_SCR_TEIE		0x04	/* transmit end interrupt enable */


/* bit values for ssr (serial status register) */

#define	SCI_SSR_TDRE		0x80	/* transmit data register empty */
#define	SCI_SSR_RDRF		0x40	/* receive data register full */
#define	SCI_SSR_ORER		0x20	/* overrun error */
#define	SCI_SSR_FER		0x10	/* framing error */
#define	SCI_SSR_PER		0x08	/* parity error */
#define	SCI_SSR_TEND		0x04	/* transmit end */
#ifdef JMC_NEVER /* JMC Can this be removed ! */
#define	SCI_SSR_MPB		0x02	/* multiprocessor bit */
#define	SCI_SSR_MPBT		0x01	/* multiprocessor bit transfer */
#endif

/* function prototypes */

#if defined(__STDC__)

extern void shSciDevInit (SCI_CHAN *pChan); 
extern void shSciIntRcv (SCI_CHAN *pChan);
extern void shSciIntTx (SCI_CHAN *pChan);
extern void shSciIntErr (SCI_CHAN *pChan);

#else /* __STDC__ */

extern void shSciDevInit ();
extern void shSciIntRcv ();
extern void shSciIntTx ();
extern void shSciIntErr ();

#endif /* __STDC__ */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif  /* __INCshSciSioh */
