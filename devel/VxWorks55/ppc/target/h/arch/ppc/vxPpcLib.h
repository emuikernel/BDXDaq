/* vxPpcLib.h - header for board dependent library routines for the PowerPC */

/*
modification history
--------------------
01r,28jan02,rcs  added vxImmrIsbGet() and vxImmrDevGet()
01q,24oct01,jtp  Add support for 440 decrementer
01p,22oct01,dtr  Inclusion of vxKeyedDecReload function for some 860 bsps.
                 SPR65678.
01o,08oct01,mil  added comments to various prototypes (SPR 9221).
01n,15aug01,pch  Add support for PPC_NO_REAL_MODE & PPC440
01m,02may01,dat  65984, Portable C Code, added vxDecReload, vxHid1Get
01l,25oct00,s_m  renamed PPC405 cpu types
01k,12jun00,alp  Added PPC405 support
01j,22mar00,tam  added missing prototypes for PPC860
01i,14jul97,mas  prototypes for accessing DAR, DSISR, SRR0, SRR1 (SPR 8918).
01h,31mar97,tam  added prototypes for DMA register access routines. 
01g,10feb97,tam  added prototypes for vxFpscrGet/Set()
01f,29oct96,tam  added prototypes for vxPowerDown() , vxPowerModeSet() and
		 vxPowerModeGet(); added power management mode macros.
01e,18jun96,tam  added prototypes for PPC403 debug register access routines.
01d,17jun96,tpr  added vxDerGet(), vxDerSet(), vxImmrGet() and
		 vxImmrSet() prototypes.
01c,29may96,tam  added prototypes for vxTimeBaseSet/Get (spr #6604).
01b,28feb96,tam  added some PPC403 declarations.
01a,23feb96,tpr  written.
*/

#ifndef __INCvxPpcLibh
#define __INCvxPpcLibh

#ifdef __cplusplus
extern "C" {
#endif

/* power management mode definitions - used by vxPowerModeSet/Get */

#define VX_POWER_MODE_DISABLE      0x1     /* power mgt disable: MSR(POW) = 0 */
#define VX_POWER_MODE_FULL         0x2     /* All CPU units are active */
#define VX_POWER_MODE_DOZE         0x4     /* doze power management mode */
#define VX_POWER_MODE_NAP          0x8     /* nap power management mode */
#define VX_POWER_MODE_SLEEP        0x10    /* sleep power management mode */
#define VX_POWER_MODE_DEEP_SLEEP   0x20    /* deep sleep power mgt mode */
#define VX_POWER_MODE_DPM          0x40    /* dynamic power management mode */
#define VX_POWER_MODE_DOWN         0x80    /* all units down */

/* DMA channel definition for the vxDmaXXGet/Set() routines */

#define	DMA_CHANNEL_0		0
#define	DMA_CHANNEL_1		1
#define	DMA_CHANNEL_2		2
#define	DMA_CHANNEL_3		3

/* function declarations */

#ifndef	_ASMLANGUAGE
#if defined(__STDC__) || defined(__cplusplus)

IMPORT void	vxMsrSet (UINT32);          /* machine status reg */
IMPORT UINT32	vxMsrGet (void);
IMPORT void	vxDarSet (UINT32);          /* data addr reg */
IMPORT UINT32	vxDarGet (void);
IMPORT void	vxDsisrSet (UINT32);        /* DSI source reg */
IMPORT UINT32	vxDsisrGet (void);
IMPORT void	vxSrr0Set (UINT32);         /* save/restore reg 0 */
IMPORT UINT32	vxSrr0Get (void);
IMPORT void	vxSrr1Set (UINT32);         /* save/restore reg 1 */
IMPORT UINT32	vxSrr1Get (void);
IMPORT UINT32	vxFirstBit (UINT32);        /* get 1st bit set using cntlzw */
IMPORT UINT32	vxPvrGet (void);            /* processor version reg */
IMPORT void	vxDecSet (UINT32);          /* decrementer reg */
IMPORT UINT32	vxDecGet (void);
IMPORT UINT32	vxDecReload (UINT32);       /* reload DEC with interrupt
                                             * lantency skew compensation */
IMPORT void	vxHid0Set (UINT32);         /* hardware imp dependent reg 0 */
IMPORT UINT32	vxHid0Get (void);
IMPORT UINT32	vxHid1Get (void);           /* hardware imp dependent reg 1 */
IMPORT STATUS	vxPowerDown (void);
IMPORT STATUS	vxPowerModeSet (UINT32);
IMPORT UINT32	vxPowerModeGet (void);
IMPORT void	vxTimeBaseSet (UINT32 tbu, UINT32 tbl); /* time base up/lower */
IMPORT void	vxTimeBaseGet (UINT32 * pTbu, UINT32 * pTbl);
IMPORT void	vxFpscrSet (UINT32);        /* FP status control reg */
IMPORT UINT32	vxFpscrGet (void);
#if 	((CPU == PPC403) || (CPU == PPC405) || (CPU == PPC405F) || \
	 (CPU == PPC440))
# if	(CPU == PPC440)
IMPORT void     vxDecIntEnable (void);
IMPORT void     vxDecIntAck (void);
#else
IMPORT UINT32	vxExierEnable (UINT32);     /* ext interrupt enable reg */
IMPORT UINT32	vxExierDisable (UINT32);
IMPORT void     vxPitSet (UINT32);          /* prog interval timer */
IMPORT UINT32   vxPitGet (void);
IMPORT void     vxPitIntEnable (void);      /* timer ctrl reg set PIT int en */
IMPORT void     vxPitIntAck (void);         /* clear TSR PIT pending bit */
# endif	/* CPU != PPC440 */
IMPORT void     vxFitIntEnable (void);      /* timer ctrl reg set FIT int en */
IMPORT void     vxFitIntDisable (void);
IMPORT void     vxFitIntAck (void);         /* clear TSR FIT pending bit */
# if	(CPU != PPC440)
IMPORT UINT32   vxExisrGet (void);          /* ext interrupt status reg */
IMPORT void     vxExisrClear (UINT32);
IMPORT UINT32   vxExierGet (void);          /* ext interrupt enable reg */
IMPORT void     vxExierSet (UINT32);
# endif	/* CPU != PPC440 */
# ifndef  PPC_NO_REAL_MODE
IMPORT UINT32   vxDccrGet (void);           /* data cache ctrl reg */
IMPORT void     vxDccrSet (UINT32);
IMPORT UINT32   vxIccrGet (void);           /* instr cache ctrl reg */
IMPORT void     vxIccrSet (UINT32);
# endif /* PPC_NO_REAL_MODE */
# if	(CPU == PPC440)
IMPORT UINT32   vxIvprGet (void);
IMPORT void     vxIvprSet (UINT32);
# else	/* CPU == PPC440 */
IMPORT UINT32   vxEvprGet (void);           /* exception prefix reg */
IMPORT void     vxEvprSet (UINT32);
IMPORT UINT32   vxIocrGet (void);           /* input/ouput config reg */
IMPORT void     vxIocrSet (UINT32);
# endif	/* CPU == PPC440 */
IMPORT UINT32   vxTcrGet (void);            /* timer ctrl reg */
IMPORT void     vxTcrSet (UINT32);
IMPORT UINT32   vxTsrGet (void);            /* timer status reg */
IMPORT void     vxTsrSet (UINT32);
# if	(CPU == PPC440)
IMPORT UINT32   vxDbcr0Get (void);
IMPORT void     vxDbcr0Set (UINT32);
IMPORT UINT32   vxDbcr1Get (void);
IMPORT void     vxDbcr1Set (UINT32);
IMPORT UINT32   vxDbcr2Get (void);
IMPORT void     vxDbcr2Set (UINT32);
IMPORT UINT32   vxDbdrGet (void);
IMPORT void     vxDbdrClear (UINT32);
# else	/* CPU == PPC440 */
IMPORT void     vxBesrSet (UINT32);         /* bus error syndrome reg */
IMPORT UINT32   vxDbcrGet (void);           /* debug ctrl reg */
IMPORT void     vxDbcrSet (UINT32);
# endif	/* CPU == PPC440 */
IMPORT UINT32   vxDbsrGet (void);           /* debug status reg */
IMPORT void     vxDbsrClear (UINT32);
IMPORT UINT32   vxDac1Get (void);           /* data addr cmp reg 1 */
IMPORT void     vxDac1Set (UINT32);
IMPORT UINT32   vxDac2Get (void);           /* data addr cmp reg 2 */
IMPORT void     vxDac2Set (UINT32);
IMPORT UINT32   vxIac1Get (void);           /* instr addr cmp reg 1 */
IMPORT void     vxIac1Set (UINT32);
IMPORT UINT32   vxIac2Get (void);           /* instr addr cmp reg 2 */
IMPORT void     vxIac2Set (UINT32);
# if	(CPU == PPC403)
IMPORT void	vxDmacrSet (UINT32 regVal, UINT32 dmaRegNo);
IMPORT UINT32	vxDmacrGet (UINT32 dmaRegNo);
IMPORT void	vxDmasrSet (UINT32 regVal);
IMPORT UINT32	vxDmasrGet (void);
IMPORT void	vxDmaccSet (UINT32 regVal, UINT32 dmaRegNo);
IMPORT UINT32	vxDmaccGet (UINT32 dmaRegNo);
IMPORT void	vxDmactSet (UINT32 regVal, UINT32 dmaRegNo);
IMPORT UINT32	vxDmactGet (UINT32 dmaRegNo);
IMPORT void	vxDmadaSet (UINT32 regVal, UINT32 dmaRegNo);
IMPORT UINT32	vxDmadaGet (UINT32 dmaRegNo);
IMPORT void	vxDmasaSet (UINT32 regVal, UINT32 dmaRegNo);
IMPORT UINT32	vxDmasaGet (UINT32 dmaRegNo);
# else	/* CPU == PPC403 */
IMPORT UINT32   vxIac3Get (void);
IMPORT void     vxIac3Set (UINT32);
IMPORT UINT32   vxIac4Get (void);
IMPORT void     vxIac4Set (UINT32);
# endif	/* CPU == PPC403 */
#endif /* (CPU == PPC4xx) */

#if 	((CPU == PPC603) || (CPU == PPC604))
IMPORT  UINT32  vxImmrDevGet (void);        /* Get PART/MASK bits of IMMR */
IMPORT  UINT32  vxImmrIsbGet (void);        /* Get ISB bits of IMMR */
#endif /* (CPU == PPC603) || (CPU == PPC604) */

#if	(CPU == PPC860)
IMPORT	void	vxImmrSet (UINT32);         /* internal mem map reg */
IMPORT	UINT32	vxImmrGet (void);
IMPORT	UINT32	vxImmrDevGet (void);        /* Get PART/MASK bits of IMMR */
IMPORT	UINT32	vxImmrIsbGet (void);        /* Get ISB bits of IMMR */
IMPORT	void	vxDerSet (UINT32);          /* debug enable reg */
IMPORT	UINT32	vxDerGet (void);
IMPORT  UINT32	vxKeyedDecReload (UINT32);  /* vxDecReload a locked DEC */
IMPORT	void	vxMTwbSet (UINT32);         /* MMU table walk base reg */
IMPORT	UINT32	vxMTwbGet (void);
IMPORT	void	vxMdCtrSet (UINT32);        /* data MMU ctrl reg */
IMPORT	UINT32	vxMdCtrGet (void);
IMPORT	void	vxMiCtrSet (UINT32);        /* instr MMU ctrl reg */
IMPORT	UINT32	vxMiCtrGet (void);
IMPORT	void	vxIcCstSet (UINT32);        /* instr cache ctrl status reg */
IMPORT	UINT32	vxIcCstGet (void);
IMPORT	void	vxIcAdrSet (UINT32);        /* instr cache addr reg */
IMPORT	UINT32	vxIcAdrGet (void);
IMPORT	UINT32	vxIcDatGet (void);          /* instr cache data port */
IMPORT	void	vxDcCstSet (UINT32);        /* data cache ctrl status reg */
IMPORT	UINT32	vxDcCstGet (void);
IMPORT	void	vxDcAdrSet (UINT32);        /* data cache addr reg */
IMPORT	UINT32	vxDcAdrGet (void);
#endif  /* (CPU == PPC860) */

#else  	/* __STDC__ */

IMPORT void	vxMsrSet ();
IMPORT UINT32	vxMsrGet ();
IMPORT void	vxDarSet ();
IMPORT UINT32	vxDarGet ();
IMPORT void	vxDsisrSet ();
IMPORT UINT32	vxDsisrGet ();
IMPORT void	vxSrr0Set ();
IMPORT UINT32	vxSrr0Get ();
IMPORT void	vxSrr1Set ();
IMPORT UINT32	vxSrr1Get ();
IMPORT UINT32	vxFirstBit ();
IMPORT UINT32	vxPvrGet ();
IMPORT void	vxDecSet ();
IMPORT UINT32	vxDecGet ();
IMPORT UINT32	vxDecReload ();
IMPORT void	vxHid0Set ();
IMPORT UINT32	vxHid0Get ();
IMPORT UINT32	vxHid1Get ();
IMPORT STATUS	vxPowerDown ();
IMPORT STATUS	vxPowerModeSet ();
IMPORT UINT32	vxPowerModeGet ();
IMPORT void	vxTimeBaseSet ();
IMPORT void	vxTimeBaseGet ();
IMPORT void	vxFpscrSet ();
IMPORT UINT32	vxFpscrGet ();
#if 	((CPU == PPC403) || (CPU == PPC405) || (CPU == PPC405F) || \
	 (CPU == PPC440))
# if	(CPU != PPC440)
IMPORT UINT32	vxExierEnable ();
IMPORT UINT32	vxExierDisable ();
IMPORT void     vxPitSet ();
IMPORT UINT32   vxPitGet ();
IMPORT void     vxPitIntEnable ();
IMPORT void     vxPitIntAck ();
# endif	/* CPU != PPC440 */
IMPORT void     vxFitIntEnable ();
IMPORT void     vxFitIntDisable ();
IMPORT void     vxFitIntAck ();
# if	(CPU != PPC440)
IMPORT UINT32   vxExisrGet ();
IMPORT void     vxExisrClear ();
IMPORT UINT32   vxExierGet ();
IMPORT void     vxExierSet ();
# endif	/* CPU != PPC440 */
# ifndef  PPC_NO_REAL_MODE
IMPORT UINT32   vxDccrGet ();
IMPORT void     vxDccrSet ();
IMPORT UINT32   vxIccrGet ();
IMPORT void     vxIccrSet ();
# endif /* PPC_NO_REAL_MODE */
# if	(CPU == PPC440)
IMPORT UINT32   vxIvprGet ();
IMPORT void     vxIvprSet ();
# else	/* CPU == PPC440 */
IMPORT UINT32   vxEvprGet ();
IMPORT void     vxEvprSet ();
IMPORT UINT32   vxIocrGet ();
IMPORT void     vxIocrSet ();
# endif	/* CPU == PPC440 */
IMPORT UINT32   vxTcrGet ();
IMPORT void     vxTcrSet ();
IMPORT UINT32   vxTsrGet ();
IMPORT void     vxTsrSet ();
# if	(CPU == PPC440)
IMPORT UINT32   vxDbcr0Get ();
IMPORT void     vxDbcr0Set ();
IMPORT UINT32   vxDbcr1Get ();
IMPORT void     vxDbcr1Set ();
IMPORT UINT32   vxDbcr2Get ();
IMPORT void     vxDbcr2Set ();
IMPORT UINT32   vxDbdrGet ();
IMPORT void     vxDbdrClear ();
# else	/* CPU == PPC440 */
IMPORT void     vxBesrSet ();
IMPORT UINT32   vxDbcrGet ();
IMPORT void     vxDbcrSet ();
# endif	/* CPU == PPC440 */
IMPORT UINT32   vxDbsrGet ();
IMPORT void     vxDbsrClear ();
IMPORT UINT32   vxDac1Get ();
IMPORT void     vxDac1Set ();
IMPORT UINT32   vxDac2Get ();
IMPORT void     vxDac2Set ();
IMPORT UINT32   vxIac1Get ();
IMPORT void     vxIac1Set ();
IMPORT UINT32   vxIac2Get ();
IMPORT void     vxIac2Set ();
# if	(CPU == PPC403)
IMPORT void	vxDmacrSet ();
IMPORT UINT32	vxDmacrGet ();
IMPORT void	vxDmasrSet ();
IMPORT UINT32	vxDmasrGet ();
IMPORT void	vxDmaccSet ();
IMPORT UINT32	vxDmaccGet ();
IMPORT void	vxDmactSet ();
IMPORT UINT32	vxDmactGet ();
IMPORT void	vxDmadaSet ();
IMPORT UINT32	vxDmadaGet ();
IMPORT void	vxDmasaSet ();
IMPORT UINT32	vxDmasaGet ();
# else	/* CPU == PPC403 */
IMPORT UINT32   vxIac3Get ();
IMPORT void     vxIac3Set ();
IMPORT UINT32   vxIac4Get ();
IMPORT void     vxIac4Set ();
# endif	/* CPU == PPC403 */
#endif /* (CPU == PPC4xx) */

#if	(CPU == PPC860)
IMPORT	void	vxImmrSet ();
IMPORT	UINT32	vxImmrGet ();
IMPORT	void	vxDerSet ();
IMPORT	UINT32	vxDerGet ();
IMPORT  UINT32	vxKeyedDecReload ();
IMPORT	void	vxMTwbSet ();
IMPORT	UINT32	vxMTwbGet ();
IMPORT	void	vxMdCtrSet ();
IMPORT	UINT32	vxMdCtrGet ();
IMPORT	void	vxMiCtrSet ();
IMPORT	UINT32	vxMiCtrGet ();
IMPORT	void	vxIcCstSet ();
IMPORT	UINT32	vxIcCstGet ();
IMPORT	void	vxIcAdrSet ();
IMPORT	UINT32	vxIcAdrGet ();
IMPORT	UINT32	vxIcDatGet ();
IMPORT	void	vxDcCstSet ();
IMPORT	UINT32	vxDcCstGet ();
IMPORT	void	vxDcAdrSet ();
IMPORT	UINT32	vxDcAdrGet ();
#endif  /* (CPU == PPC860) */

#endif 	/* __STDC__ */
#endif  /* _ASMLANGUAGE */

#ifdef __cplusplus
}
#endif

#endif /* __INCvxPpcLibh */
