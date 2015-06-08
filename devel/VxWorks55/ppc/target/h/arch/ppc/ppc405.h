/* ppc405.h - PowerPC 405 specific header */

/* Copyright 1984-2002 Wind River Systems, Inc. */
/*
modification history
--------------------
01i,06dec02,pch  Add MSR bit number definitions
01h,13jun02,jtp  identify class of MMU support (SPR #78396)
01g,22nov01,pch  Add _WRS_STRICT_ALIGNMENT for target/src/ostool/loadElfLib.c
01f,04dec00,s_m  register cleanup
01e,30nov00,s_m  fixed PLB error address regs
01d,30oct00,s_m  changed DBCR to DBCR0
01c,25oct00,s_m  renamed PPC405 cpu types
01b,20oct00,s_m  Added defines from walnut bsp ppc405.h
01a,7mar2K,alp written.
*/

#ifndef __INCppc405h
#define __INCppc405h

#ifdef __cplusplus
extern "C" {
#endif

/* INT_MASK definition (mask EE & CE bits) : overwrite the one in asmPpc.h */

#undef	INT_MASK
#define INT_MASK(src, des)	rlwinm  des, src, 0, 17, 15; \
				rlwinm  des, des, 0, 15, 13

/*
 * Tell the loader this processor can't handle any misalignment.
 * It's supposed to be able to, but this seems a
 * worthwhile precaution considering various errata.
 */
#define	_WRS_STRICT_ALIGNMENT	1

/* MMU supports software TLB miss handler */
#define _WRS_TLB_MISS_CLASS_SW	1

/* MSR definitions */

#undef 	_PPC_MSR_POW_U		/* power management not supported */
#undef 	_PPC_MSR_ILE_U		/* little endian mode not supported */
#undef 	_PPC_MSR_SF_U		/* 64 bit mode not implemented */

#undef 	_PPC_MSR_BE		/* branch trace not supported */
#undef 	_PPC_MSR_IT		/* instruction address translation unsupported*/
#undef 	_PPC_MSR_DT		/* data address translation unsupported */
#undef 	_PPC_MSR_RI		/* recoverable interrupt unsupported */
#undef 	_PPC_MSR_LE		/* little endian mode unsupported */
#undef 	_PPC_MSR_SE		/* single step unsupported */
#undef 	_PPC_MSR_PE		/* protection enable unsupported */
#undef 	_PPC_MSR_PX		/* protection exclusive mode unsupported */

/* PPC405F supports a FPU whereas the PPC405 does not */
#if (CPU == PPC405)
#undef 	_PPC_MSR_FP		/* floating point unsupported */
#undef 	_PPC_MSR_FE1	/* floating point not supported */
#undef 	_PPC_MSR_FE0	/* floating point not supported */
#endif	/* PPC405 */

#define	_PPC_MSR_WE_U	0x0004	/* wait state enable */
#define _PPC_MSR_CE_U	0x0002	/* critical interrupt enable */
#define _PPC_MSR_DE	0x0200	/* debug exception enable */
#define _PPC_MSR_IR	0x0020	/* instruction relocate */
#define _PPC_MSR_DR	0x0010	/* data relocate */
#define _PPC_MSR_CE	0x00020000	/* critical interrupt enable mask */
#define _PPC_MSR_AP_U   0x0200  /* Auxiliary Processor Unit available */
#define _PPC_MSR_APE_U  0x0008  /* APU Exception Enable */

/* MSR bit number definitions */

#if (CPU == PPC405F)
#define _PPC_MSR_BIT_FP  	18	/* MSR Floating Ponit Aval. bit - FP */
#define _PPC_MSR_BIT_FE0 	20	/* MSR FP exception mode 0 bit - FE0 */
#define _PPC_MSR_BIT_FE1 	23	/* MSR FP exception mode 1 bit - FE1 */
#endif	/* PPC405 */
#define _PPC_MSR_BIT_IR  	26	/* MSR Inst Translation bit - IR */
#define _PPC_MSR_BIT_DR  	27	/* MSR Data Translation bit - DR */

/* read-only access to SPRG4-7 */
#define  SPRG4R 0x104               /* Special Purpose General 4 - read only  */
#define  SPRG5R 0x105               /* Special Purpose General 5 - read only  */
#define  SPRG6R 0x106               /* Special Purpose General 6 - read only  */
#define  SPRG7R 0x107               /* Special Purpose General 7 - read only  */

/* Device Control Register PowerPC405 specific */

#undef  DEC		/* decrementer not supported */

/* Special Purpose Register PowerPC405 specific */

#define CCR0	0x3b3	/* core configuration register r/w */
#define DAC1	0x3f6	/* data adress compare register 1 r/w */
#define DAC2	0x3f7	/* data adress compare register 2 r/w */
#define	DBCR0	0x3f2	/* debug control register read/write */
#define	DBCR1	0x3bd	/* debug control register read/write */
#define	DBSR	0x3f0	/* debug status register read/clear */
#define	DCCR	0x3fa	/* data cache control register r/w */
#define DCWR    0x3ba	/* data cache write-thru register (PPC405GC) r/w */
#define DVC1	0x3b6	/* Data value compare 1 */
#define DVC2	0x3b7	/* Data value compare 2 */
#define	DEAR	0x3d5	/* data exception address registers r */
#define	ESR		0x3d4	/* exception syndrom register r/w */
#define	EVPR	0x3d6	/* exception prefix register r/w */
#define IAC1	0x3f4	/* instruction adress compare register 1 r/w */
#define IAC2	0x3f5	/* instruction adress compare register 2 r/w */
#define IAC3	0x3b4	/* instruction adress compare register 3 r/w */
#define IAC4	0x3b5	/* instruction adress compare register 4 r/w */
#define	ICCR	0x3fb	/* instruction cache cacheability register r/w */
#define	ICDBDR	0x3d3	/* instruction cache debug data register r */
#define	PID		0x3b1	/* process id (405GC/GCX) r */
#define	PIT		0x3db	/* programmable interval timer r/w */
#define SGR    	0x3b9	/* storage guarded register (PPC405GC) r/w */
#define SLER    0x3bb   /* Storage Little Endian Register */
#define	SRR2	0x3de	/* save/restore register 2 r/w */
#define	SRR3	0x3df	/* save/restore register 3 r/w */
#define SU0R    0x3bc   /* Storage User Defined 0 Register */
#define	TBLO	0x11c	/* time base low r/w */
#define	TBHI	0x11d	/* time base high r/w */
#define	TCR	0x3da	/* timer control register r/w */
#define	TSR	0x3d8	/* timer status register read/clear */
#define	TSRS	0x3d9	/* timer status register set (set only) */
#define	ZPR	0x3b0	/* zone protection register (PPC405GC/GCX) r/w */

#define mtdcr(dcrn, rs) .long   (0x7c0001c3 | (rs << 21) | (dcrf << 11))

#define MTDCR		0x7c0001c3
#define MFDCR		0x7c000143

/* defines for cache support */
#define	_PPC405_ICCR_DEFAULT_VAL	0x80000001 /* def. inst. cachability */
#define	_PPC405_DCCR_DEFAULT_VAL	0x80000001 /* def. data cachability */

#undef	_CACHE_ALIGN_SIZE
#define _CACHE_ALIGN_SIZE	32	/* all 405 cores have 32-byte cache lines */

#define	_CACHE_LINE_MASK	0x0000001F

#define _PPC_CACHE_UNIFIED	FALSE	

/* 405 specific special purpose registers */

#define	_PPC405_DBSR	0x3f0	/* debug status register read/clear */
#define _PPC405_DCCR	0x3fa	/* data cache control register r/w */
#define _PPC405_DEAR	0x3d5	/*  data exception address registers r */
#define _PPC405_ESR		0x3d4	/* exception syndrom register r/w */
#define _PPC405_EVPR	0x3d6	/* exception prefix register r/w */
#define _PPC405_ICCR	0x3fb	/* instruction cache control register r/w */
#define _PPC405_PBL1	0x3fc	/* protection bound lower 1 r/w */
#define _PPC405_PBL2	0x3fe	/* protection bound lower 2 r/w */
#define _PPC405_PBU1	0x3fd	/* protection bound upper 1 r/w */
#define _PPC405_PBU2	0x3ff	/* protection bound upper 2 r/w */
#define _PPC405_PIT		0x3db	/* programmable interval timer r/w */
#define _PPC405_SRR2	0x3de	/* save/restore register 2 r/w */
#define _PPC405_SRR3	0x3df	/* save/restore register 3 r/w */
#define _PPC405_SGR    	0x3b9	/* storage guarded register (PPC405GC) r/w */
#define _PPC405_DCWR    0x3ba   /* data cache write-thru (PPC405GC)r/w */

/* debug control register 0 */

#define _DBCR0_EDM_U	0x8000		/* external debug mode */
#define _DBCR0_IDM_U	0x4000		/* internal debug mode */
#define _DBCR0_RST_U	0x3000		/* reset */
#define _DBCR0_IC_U	    0x0800		/* instruction completion debug event */
#define _DBCR0_BT_U	    0x0400		/* branch taken */
#define _DBCR0_EDE_U	0x0200		/* exception debug event */
#define _DBCR0_TDE_U	0x0100		/* trap debug event */
#define _DBCR0_IA1_U	0x0080		/* instruction address compare 1 */
#define _DBCR0_IA2_U	0x0040		/* instruction address compare 2 */
#define _DBCR0_IA12_U	0x0020		/* instruction address compare 1:2 */
#define _DBCR0_IA12X_U	0x0010		/* instruction address compare 1:2 excl. */
#define _DBCR0_IA3_U	0x0008		/* instruction address compare 3 */
#define _DBCR0_IA4_U	0x0004		/* instruction address compare 4 */
#define _DBCR0_IA34_U	0x0002		/* instruction address compare 3:4 */
#define _DBCR0_IA34X_U	0x0001		/* instruction address compare 3:4 excl. */
#define _DBCR0_EDM	0x80000000	/* external debug mode */
#define _DBCR0_IDM	0x40000000	/* internal debug mode */
#define _DBCR0_RST	0x30000000	/* reset */
#define _DBCR0_IC	0x08000000	/* instruction completion debug event */
#define _DBCR0_BT	0x04000000	/* branch taken */
#define _DBCR0_EDE	0x02000000	/* exception debug event */
#define _DBCR0_TDE	0x01000000	/* trap debug event */
#define _DBCR0_IA1	0x00800000	/* instruction address compare 1 */
#define _DBCR0_IA2	0x00400000	/* instruction address compare 2 */
#define _DBCR0_IA12	0x00200000	/* instruction address compare 1:2 */
#define _DBCR0_IA12X	0x00100000	/* instruction address compare 1:2 excl. */
#define _DBCR0_IA3	0x00080000	/* instruction address compare 3 */
#define _DBCR0_IA4	0x00040000	/* instruction address compare 4 */
#define _DBCR0_IA34	0x00020000	/* instruction address compare 3:4 */
#define _DBCR0_IA34X	0x00010000	/* instruction address compare 3:4 excl. */
#define _DBCR0_FT	0x00000002	/* freeze timers on debug */
#define _DBCR0_DNI	0x00000001	/* disable non-critical interrupts */

/* debug control register 1 */
#define _DBCR1_D1R_U	0x8000		/* data address compare 1 read */
#define _DBCR1_D2R_U	0x4000		/* data address compare 2 read */
#define _DBCR1_D1W_U	0x2000		/* data address compare 1 write */
#define _DBCR1_D2W_U	0x1000		/* data address compare 2 write */
#define _DBCR1_D1S_U	0x0C00		/* DAC 1 size */
#define _DBCR1_D2S_U	0x0300		/* DAC 2 size */
#define _DBCR1_DA12_U	0x0080		/* DA range compare 1:2 */
#define _DBCR1_DA12X_U	0x0040		/* DA range excl. compare 1:2 */
#define _DBCR1_DV1M_U	0x000C		/* Data value compare 1 mode */
#define _DBCR1_DV2M_U	0x0003		/* Data value compare 2 mode */
#define _DBCR1_D1R  	0x80000000		/* data address compare 1 read */
#define _DBCR1_D2R		0x40000000		/* data address compare 2 read */
#define _DBCR1_D1W  	0x20000000		/* data address compare 1 write */
#define _DBCR1_D2W  	0x10000000		/* data address compare 2 write */
#define _DBCR1_D1S  	0x0C000000		/* DAC 1 size */
#define _DBCR1_D2S  	0x03000000		/* DAC 2 size */
#define _DBCR1_DA12  	0x00800000		/* DA range compare 1:2 */
#define _DBCR1_DA12X  	0x00400000		/* DA range excl. compare 1:2 */
#define _DBCR1_DV1M  	0x000C0000		/* Data value compare 1 mode */
#define _DBCR1_DV2M  	0x00030000		/* Data value compare 2 mode */
#define _DBCR1_DV1BE 	0x0000F000		/* Data value compare 1 byte enable */
#define _DBCR1_DV2BE 	0x00000F00		/* Data value compare 2 byte enable */

/* data access sizes */
#define _DBCR1_D1S_BYTE			0x00000000		/* byte address */
#define _DBCR1_D1S_HWORD		0x04000000		/* halfword address */
#define _DBCR1_D1S_WORD			0x08000000		/* word address */
#define _DBCR1_D1S_CACHE_LINE	0x0C000000		/* cache line (8-word) addrs. */

#define _DBCR1_D2S_BYTE			0x00000000		/* byte address */
#define _DBCR1_D2S_HWORD		0x01000000		/* halfword address */
#define _DBCR1_D2S_WORD			0x02000000		/* word address */
#define _DBCR1_D2S_CACHE_LINE	0x03000000		/* cache line (8-word) addrs. */


/* debug status register */

#define _DBSR_IC_U	0x8000		/* instruction completion */
#define _DBSR_BT_U	0x4000		/* branch taken */
#define _DBSR_EXC_U	0x2000		/* exception debug */
#define _DBSR_TIE_U	0x1000		/* trap instruction */
#define _DBSR_UDE_U	0x0800		/* unconditional debug */
#define _DBSR_IA1_U	0x0400		/* instruction address compare 1 */
#define _DBSR_IA2_U	0x0200		/* instruction address compare 2 */
#define _DBSR_DR1_U	0x0100		/* data address compare read 1 */
#define _DBSR_DW1_U	0x0080		/* data address compare write 1 */
#define _DBSR_DR2_U	0x0040		/* data address compare read 2 */
#define _DBSR_DW2_U	0x0020		/* data address compare write 2 */
#define _DBSR_IDE_U	0x0010		/* imprecise debug */
#define _DBSR_IA3_U	0x0008		/* instruction address compare 3 */
#define _DBSR_IA4_U	0x0004		/* instruction address compare 4 */
#define _DBSR_IC	0x80000000	/* instruction completion */
#define _DBSR_BT	0x40000000	/* branch taken */
#define _DBSR_EXC	0x20000000	/* exception debug */
#define _DBSR_TIE	0x10000000	/* trap instruction */
#define _DBSR_UDE	0x08000000	/* unconditional debug */
#define _DBSR_IA1	0x04000000	/* instruction address compare 1 */
#define _DBSR_IA2	0x02000000	/* instruction address compare 2 */
#define _DBSR_DR1	0x01000000	/* data address compare read 1 */
#define _DBSR_DW1	0x00800000	/* data address compare write 1 */
#define _DBSR_DR2	0x00400000	/* data address compare read 2 */
#define _DBSR_DW2	0x00200000	/* data address compare write 2 */
#define _DBSR_IDE	0x00100000	/* imprecise debug */
#define _DBSR_IA3	0x00080000	/* instruction address compare 3 */
#define _DBSR_IA4	0x00040000	/* instruction address compare 4 */
#define _DBSR_MRRM	0x00000300	/* Most recent reset mask */
#define _DBSR_MRR(n)	((n) << 8) 	/* Most recent reset */

/* mask for hardware breakpoints */

#define _DBSR_HWBP_MSK	(_DBSR_IA1 | _DBSR_IA2 | \
                         _DBSR_DR1 | _DBSR_DW1 | \
			 			 _DBSR_DR2 | _DBSR_DW2 | \
						 _DBSR_IA3 | _DBSR_IA4)

/* timer control register */

#define _PPC405_TCR_WPM_U	0xc000	/* watchdog period */
#define _PPC405_TCR_WRCM_U	0x3000	/* watchdog reset control */
#define _PPC405_TCR_WIEM_U	0x0800	/* watchdog interrupt enable */
#define _PPC405_TCR_PIEM_U	0x0400	/* pit interrupt enable */
#define _PPC405_TCR_FPM_U	0x0300	/* fit period */
#define _PPC405_TCR_FIEM_U	0x0080	/* fit interrupt enable */
#define _PPC405_TCR_ARE_U	0x0040	/* fit auto-reload enable */

/* timer status register */

#define _PPC405_TSR_SNWM_U	0x8000	/* supress next watchdog */
#define _PPC405_TSR_WISM_U	0x4000	/* watchdog interrupt status */
#define _PPC405_TSR_WRSM_U	0x3000	/* watchdog reset status */
#define _PPC405_TSR_PISM_U	0x0800	/* pit interrupt status */
#define _PPC405_TSR_FISM_U	0x0400	/* fit interrupt status */


/* exception syndrome register mask bits: 
 * 0 - error not occured 1 - error occured */

#define _PPC405_ESR_IMCOM 0x80000000  /* instr machine check occurred */
#define _PPC405_ESR_PEIM  0x08000000  /* program exception - illegal */
#define _PPC405_ESR_PEPM  0x04000000  /* program exception - previledged */
#define _PPC405_ESR_PETM  0x02000000  /* program exception - trap */
#define _PPC405_ESR_DST   0x00800000  /* data storage exception - store fault */
#define _PPC405_ESR_DIZ   0x00400000  /* data/inst storage exception - zone f.*/
#define _PPC405_ESR_PFP	  0x00080000  /* Program exception - FPU */
#define _PPC405_ESR_U0F	  0x00008000  /* data storage exception - U0 fault */ 


/* exception vector prefix register */

#define _PPC405_EVPR_EVPM	0xffff0000  /* exception vector prefix mask */


/* the following defines are needed by the 403/405 cache library */

#define _PVR_CONF_403GA	    0x0000	/* processor conf. bits for 403GA */
#define _PVR_CONF_403GC	    0x0002	/* processor conf. bits for 403GC */
#define _PVR_CONF_403GCX    0x0014	/* processor conf. bits for 403GCX */

#ifdef __cplusplus
}
#endif

#endif /* __INCppc405h */
