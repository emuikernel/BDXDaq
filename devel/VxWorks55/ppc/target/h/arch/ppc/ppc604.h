/* ppc604.h - PowerPC 604 specific header */

/* Copyright 1984-2002 Wind River Systems, Inc. */

/*
modification history
--------------------
01n,12sep02,pch  SPR 80642: add MSSCR0 for MPC7400 and MPC7410 L1 dCache flush
01m,13jun02,jtp  identify class of MMU support (SPR #78396)
01l,23may02,pcs  Implemented code review changes for Extra BAT support.
01k,08may02,mil  Relocated _EXC_OFF_PERF to avoid corruption by the extended
                 _EXC_ALTIVEC_UNAVAILABLE vector (SPR #76916).
                 Added _EXC_OFF_THERMAL for PPC604 (SPR #77552).
01j,29apr02,pcs  Add defined for EXTRA BAT support.
01i,12dec01,kab  add immr reg
01h,02may01,dat  65984, Portable C Code, added HID1
01g,30mar01,pcs  ADDED _PPC_MSR_VEC if _WRS_ALTIVEC_SUPPORT is defined.
01f,28mar01,dtr  Adding define _WRS_ALTIVEC_SUPPORT.
01e,09nov98,ms   fixed last checkin - removed extra __cplusplus bracket.
01d,14oct98,elg  added hardware breakpoints for PPC603 and PPC604
01c,08oct96,tam  added MSR and HID0 bit definition macros.
01b,09apr96,tpr  added PowerPC 604 specific macros.
01a,14feb96,tpr  written.
*/

#ifndef __INCppc604h
#define __INCppc604h

#ifdef __cplusplus
extern "C" {
#endif

/* MMU supports TLB miss with hardware (not SRR0/SRR1) */
#undef	_WRS_TLB_MISS_CLASS_SW

/* This is essentially a compiler switch to remove/add altivec support
 * in the target src. The BSP code is controlled using INCLUDE_ALTIVEC
 * in config.h
 */
#define _WRS_ALTIVEC_SUPPORT 1

/* Special Purpose Register PowerPC604 specific */

#define IMMR    638     /* Internal mem map reg - from 82xx slave SIU */

#undef	ASR		/* 64 bit implementation only */

#define MMCR0	952	/* Monitor Mode Control Register 0 */
#define PMC1	953	/* Performance Monitor Counter Register 1 */
#define PMC2 	954	/* Performance Monitor Counter Register 2 */
#define SIA	955	/* Sampled Instruction Address Register */
#define SDA	959	/* Sampled Data Address Register */
#define	HID0	1008	/* hardware implementation register 0 */
#define	HID1	1009	/* hardware implementation register 1 */
#define HID2    1011    /* hardware implementation register 2 (MPC755)*/
#define IABR	1010	/* Instruction Address Breakpoint Register */
#define DABR	1013	/* Data Address Breakpoint Register */
#define PIR	1023	/* Processor Identification Register */

#ifdef	_WRS_ALTIVEC_SUPPORT
/*
 * MSSCR0 exists only in MPC7400 (PVR=000Cxxxx) and MPC7410 (PVR=800Cxxxx).
 * We use the DL1HWF bit in cacheALib.s, and define the other fields here
 * for completeness.  All are in the MS half of the register.  MSSCR0 is not
 * present in MPC744x or MPC745x.
 */
#define	MSSCR0	1014	/* Memory Subsystem Control Register */
#define	_PPC_MSSCR0_SHDEN_U	0x8000	/* Shared-state (MESI) enable */
#define	_PPC_MSSCR0_SHDPEN3_U	0x4000	/* MEI mode SHD0/SHD1 signal enable */
#define	_PPC_MSSCR0_L1_INTVEN_U	0x3800	/* L1 data cache HIT intervention */
#define	_PPC_MSSCR0_L2_INTVEN_U	0x0700	/* L2 data cache HIT intervention */
#define	_PPC_MSSCR0_DL1HWF_U	0x0080	/* L1 data cache hardware flush */
#define	_PPC_MSSCR0_EMODE_U	0x0020	/* MPX bus mode (read-only) */
#define	_PPC_MSSCR0_ABD_U	0x0010	/* Address bus driven (read-only) */
#endif	/* _WRS_ALTIVEC_SUPPORT */

#ifdef 	_CACHE_ALIGN_SIZE
#undef	_CACHE_ALIGN_SIZE
#endif	/* _CACHE_ALIGN_SIZE */

#define	_CACHE_ALIGN_SIZE	32	/* cache line size */

/* spr976 - DMISS data tlb miss address register 
 * spr977 - DCMP data tlb miss compare register 
 * spr978 - HASH1 PTEG1 address register
 * spr980 - HASH2 PTEG2 address register
 * IMISS  - instruction tlb miss address register
 * ICMP   - instruction TLB mis compare register
 * RPA    - real page address register
 * HID0   - hardware implemntation register
 * HID2   - instruction address breakpoint register
 */

#define	_PPC_HID0_EMCP		0x80000000	/* Enable machine check pin */
#define _PPC_HID0_ECPC		0x40000000	/* Enable cache parity check */
#define	_PPC_HID0_EBA		0x20000000	/* Enable address bus parity */
#define	_PPC_HID0_EBD		0x10000000	/* Enable data bus parity */
#define	_PPC_HID0_PAR		0x01000000	 
#define	_PPC_HID0_NHR		0x00010000	/* Not hard reset */
#define _PPC_HID0_ICE   	0x00008000	/* inst cache enable */
#define _PPC_HID0_DCE		0x00004000	/* data cache enable */
#define _PPC_HID0_ILOCK 	0x00002000	/* inst cache lock */
#define _PPC_HID0_DLOCK		0x00001000	/* data cache lock */
#define _PPC_HID0_ICFI		0x00000800	/* inst cache flash invalidate*/
#define _PPC_HID0_DCFI		0x00000400	/* data cache flash invalidate*/
#define _PPC_HID0_SIED		0x00000080	/* serial instr exec disable */
#define _PPC_HID0_DCFA		0x00000040	/* dCache flush assist on 7xx */
#define _PPC_HID0_BHTE		0x00000004	/* branch history table enable*/

#define _PPC_HID0_XBSEN         0x0100          /* Extended Block Size enable */
#define _PPC_HID0_HIGH_BAT_EN_U 0x0080          /* High Bat enable on MPC7455 */
#define _PPC_HID2_HIGH_BAT_EN_U 0x0004          /* High Bat enable on MPC755 */

#define	PVR			287		/* processor version register */

/* HID0 bit definitions */

#define _PPC_HID0_BIT_ICE	16		/* HID0 ICE bit for 604 */
#define _PPC_HID0_BIT_DCE	17		/* HID0 DCE bit for 604 */
#define _PPC_HID0_BIT_ILOCK	18		/* HID0 ILOCK bit for 604 */
#define _PPC_HID0_BIT_DLOCK	19		/* HID0 DLOCK bit for 604 */
#define _PPC_HID0_BIT_ICFI	20		/* HID0 ICFI bit for 604 */
#define _PPC_HID0_BIT_DCFI	21		/* HID0 DCFI bit for 604 */
#define _PPC_HID0_BIT_SIED	24		/* HID0 SIED bit for 604 */
#define _PPC_HID0_BIT_DCFA	25		/* HID0 DCFA bit for 7xx/74xx */
#define _PPC_HID0_BIT_BHTE	29		/* HID0 BHTE bit for 604 */

#define _PPC_HID0_BIT_XBSEN	23		/* HID0 XBSEN bit for 7455 */
#define _PPC_HID0_BIT_HIGH_BAT_EN 8       /* HID0 HIGH_BAT_EN bit for 7455 */
#define _PPC_HID2_BIT_HIGH_BAT_EN 13      /* HID2 HIGH_BAT_EN bit for 755 */

/* MSR bit definitions */

#define _PPC_MSR_BIT_POW 	13	/* MSR Power Management bit - POW */
#define _PPC_MSR_BIT_ILE 	15	/* MSR Excep little endian bit - ILE */
#define _PPC_MSR_BIT_FP  	18	/* MSR Floating Ponit Aval. bit - FP */
#define _PPC_MSR_BIT_FE0 	20	/* MSR FP exception mode 0 bit - FE0 */
#define _PPC_MSR_BIT_SE  	21	/* MSR Single Step Trace bit - SE */
#define _PPC_MSR_BIT_BE  	22	/* MSR Branch Trace Enable bit - BE */
#define _PPC_MSR_BIT_FE1 	23	/* MSR FP exception mode 1 bit - FE1 */
#define _PPC_MSR_BIT_IP  	25	/* MSR Exception Prefix bit - EP */
#define _PPC_MSR_BIT_IR  	26	/* MSR Inst Translation bit - IR */
#define _PPC_MSR_BIT_DR  	27	/* MSR Data Translation bit - DR */
#define _PPC_MSR_BIT_PM  	29	/* MSR Performance Monitor bit - MR */
#define _PPC_MSR_BIT_RI  	30	/* MSR Exception Recoverable bit - RI */

#ifdef _WRS_ALTIVEC_SUPPORT
#define _PPC_MSR_VEC        0x0200      /* Bit 6 of MSR                      */
#define _PPC_MSR_BIT_VEC  	06	/* MSR Altivec Available bit - VEC */
#endif 


/* PPC604 specific exceptions */

/*
 * the PERF excExtConnectCode stub (extended vector) won't fit at the
 * default location 0xf00 because _EXC_ALTIVEC_UNAVAILABLE is at 0x0f20,
 * so we need to put them where we've got some available space and jump
 * to it from _EXC_OFF_PERF.  The stub being 22 long words max currently
 * (including 4xx critical exception and extended vectors), 0x0f80 - 0x0f20
 * = 0x60 = 96 >= 88 = 22 * 4 has enough room for _EXC_ALTIVEC_UNAVAILABLE.
 */
#define _EXC_OFF_PERF           0x0f00          /* performance monitoring intr*/
#define _EXC_NEW_OFF_PERF       0x0f80          /* relocated perf monitor */

#define _EXC_OFF_INST_BKPT	0x1300	        /* instruction address BP */
#define _EXC_OFF_THERMAL        0x1700          /* performance monitoring intr*/

/* IABR bit definitions */

/* set and get address in IABR */

#define _PPC_IABR_ADD(x)    	((x) & 0xFFFFFFFC)

#define _PPC_IABR_BE            0x00000002      /* breakpoint enabled */
#define	_PPC_IABR_TE		0x00000001	/* translation enabled */

/* DABR bits definition */

/* set and get address in DABR */

#define _PPC_DABR_DAB(x)	((x) & 0xFFFFFFF8)

#define _PPC_DABR_BT		0x00000004	/* breakpoint translation */
#define	_PPC_DABR_DW		0x00000002	/* data write enable */
#define _PPC_DABR_DR		0x00000001	/* data read enable */

/* mask for read and write operations */

#define _PPC_DABR_D_MSK		(_PPC_DABR_DW | _PPC_DABR_DR)

/* DSISR bits definition */

#define _PPC_DSISR_BRK		0x00400000	/* DABR match occurs */

#ifdef __cplusplus
}
#endif

#endif /* __INCppc604h */
