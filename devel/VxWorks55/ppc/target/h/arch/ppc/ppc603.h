/* ppc603.h - PowerPC 603 and PowerPC EC 603 specific header */

/* Copyright 1984-2002 Wind River Systems, Inc. */

/*
modification history
--------------------
01k,13jun02,jtp  identify class of MMU support (SPR #78396)
01j,12dec01,kab  add immr reg
01i,02may01,dat  65984, Portable C Code, added HID1
01h,14oct98,elg  added hardware breakpoints for PPC603 and PPC604
01g,19aug98,tpr added PowerPC EC 603 support.
01f,11nov96,tam added/modified HID0 macros.
01e,08oct96,tam added MSR and HID0 bit definition macros.
01d,17jun96,tpr added PowerPC 603 specific macros.
01c,21feb96,tpr added new macros.
01b,05oct95,kvk filled the dummy defines with proper values.
01a,17mar94,yao written.
*/

#ifndef __INCppc603h
#define __INCppc603h

#ifdef __cplusplus
extern "C" {
#endif

#define _PPC_MSR_TGPR	0x00020000	/* temporary gpr remapping */

/* MMU supports software TLB miss handler */
#define _WRS_TLB_MISS_CLASS_SW	1

#define	TBLR	268	/* lower time base register (read only) */
#define	TBUR	269	/* upper time base register (read only) */
#define	TBLW	TBL	/* lower time base register (write only) */
#define	TBUW	TBU	/* upper time base register (write only) */

#define IMMR    638     /* Internal mem map reg - from 82xx slave SIU */

/* software table search registers */

#define	DMISS	976	/* data tlb miss address register */
#define	DCMP	977	/* data tlb miss compare register */
#define	HASH1	978	/* PTEG1 address register */
#define	HASH2	979	/* PTEG2 address register */
#define	IMISS	980	/* instruction tlb miss address register */
#define	ICMP	981	/* instruction tlb mis compare register */
#define	RPA	982	/* real page address register */

#define	HID0	1008	/* hardware implementation register 0 */
#define	HID1	1009	/* hardware implementation register 1 */
#define	IABR	1010	/* instruction address breakpoint register */

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

#define	_PPC_HID0_EMCP	0x80000000	/* enable machine check pin */
#define	_PPC_HID0_EBA	0x20000000	/* enable bus adress parity checking */
#define	_PPC_HID0_EBD	0x10000000	/* enable bus data parity checking */
#define	_PPC_HID0_SBCLK	0x08000000	/* select bus clck for test clck pin */
#define	_PPC_HID0_EICE	0x04000000	/* enable ICE outputs */
#define	_PPC_HID0_ECLK	0x02000000	/* enable external test clock pin */
#define	_PPC_HID0_PAR	0x01000000	/* disable precharge of ARTRY */
#define	_PPC_HID0_DOZE	0x00800000	/* DOZE power management mode */
#define	_PPC_HID0_NAP	0x00400000	/* NAP power management mode */
#define	_PPC_HID0_SLEEP	0x00200000	/* SLEEP power management mode */
#define	_PPC_HID0_DPM	0x00100000	/* enable dynamic power management */
#define	_PPC_HID0_RISEG	0x00080000	/* reserved for test */
#define	_PPC_HID0_NHR	0x00010000	/* reserved */
#define _PPC_HID0_ICE   0x00008000	/* inst cache enable */
#define _PPC_HID0_DCE   0x00004000	/* data cache enable */
#define _PPC_HID0_ILOCK 0x00002000	/* inst cache lock */
#define _PPC_HID0_DLOCK 0x00001000	/* data cache lock */
#define _PPC_HID0_ICFI  0x00000800	/* inst cache flash invalidate */
#define _PPC_HID0_DCFI  0x00000400	/* data cache flash invalidate */
#define _PPC_HID0_SIED  0x00000080	/* serial instr exec disable */
#define _PPC_HID0_BHTE  0x00000004	/* branch history table enable */

/* HID0 bit definitions */

#define _PPC_HID0_BIT_ICE       16	/* HID0 ICE bit for 603 */
#define _PPC_HID0_BIT_DCE       17	/* HID0 DCE bit for 603 */
#define _PPC_HID0_BIT_ILOCK     18	/* HID0 ILOCK bit for 603 */
#define _PPC_HID0_BIT_DLOCK     19	/* HID0 DLOCK bit for 603 */
#define _PPC_HID0_BIT_ICFI      20	/* HID0 ICFI bit for 603 */
#define _PPC_HID0_BIT_DCFI      21	/* HID0 DCFI bit for 603 */
#define _PPC_HID0_BIT_SIED      24	/* HID0 SIED bit for 603 */
#define _PPC_HID0_BIT_BHTE      29	/* HID0 BHTE bit for 603 */

/* MSR bit definitions */

#define _PPC_MSR_BIT_POW 	13	/* MSR Power Management bit - POW */
#define _PPC_MSR_BIT_TGPR 	14	/* MSR Temporary GPR remapping - TGPR */
#define _PPC_MSR_BIT_ILE 	15	/* MSR Excep little endian bit - ILE */
#define _PPC_MSR_BIT_FP  	18	/* MSR Floating Ponit Aval. bit - FP */
#define _PPC_MSR_BIT_FE0 	20  	/* MSR FP exception mode 0 bit - FE0 */
#define _PPC_MSR_BIT_SE  	21  	/* MSR Single Step Trace bit - SE */
#define _PPC_MSR_BIT_BE  	22  	/* MSR Branch Trace Enable bit - BE */
#define _PPC_MSR_BIT_FE1 	23  	/* MSR FP exception mode 1 bit - FE1 */
#define _PPC_MSR_BIT_IP  	25	/* MSR Exception Prefix bit - EP */
#define _PPC_MSR_BIT_IR  	26	/* MSR Inst Translation bit - IR */
#define _PPC_MSR_BIT_DR  	27	/* MSR Data Translation bit - DR */
#define _PPC_MSR_BIT_RI  	30	/* MSR Exception Recoverable bit - RI */

/* PowerPC EC 603 does not have floating point unit */

#if	(CPU == PPCEC603)
#undef  _PPC_MSR_FP                     /* hardware floating point unsupported*/
#undef	_PPC_MSR_BIT_FP  		/* MSR Floating Ponit Aval. bit - FP */
#undef	_PPC_MSR_BIT_FE0 	  	/* MSR FP exception mode 0 bit - FE0 */
#undef	_PPC_MSR_BIT_FE1 	  	/* MSR FP exception mode 1 bit - FE1 */
#endif	/* (CPU == PPCEC603) */

/* PPC603 specific exceptions */

#define _EXC_OFF_INST_BKPT	0x1300	/* Instruction Address Breakpoint */

/* IABR bit definitions */

/* set and get address in IABR */

#define _PPC_IABR_ADD(x)	((x) & 0xFFFFFFFC)

#define _PPC_IABR_BE		0x00000002	/* breakpoint enabled */

#ifdef __cplusplus
}
#endif

#endif /* __INCppc603h */
