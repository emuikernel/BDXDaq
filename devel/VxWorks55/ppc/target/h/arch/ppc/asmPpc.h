/* asmPpc.h - PowerPC assembler definitions header file */

/* Copyright 1984-1996 Wind River Systems, Inc. */

/*
modification history
--------------------
01o,26mar02,pcs  Add defines for I & D BAT's 4-7.
01n,02may01,pch  Documentation cleanup
01m,25oct00,s_m  renamed PPC405 cpu types
01l,12jun00,alp  Added PPC405 Support
01k,18mar97,tam  made INT_MASK definition conditional.
01j,08apr96,tpr  move implementation specific definition in ppcXXX.h.
01i,04mar96,tam  added some SPR definitions for the PPC403.
01h,26feb96,kkk  added HID0 for PPC601.
01g,14feb96,tpr  added PPC604 support.
01f,09feb95,yao  added RFCI, MTBESR_P0 macro for PPC403.
01e,30jan95,yao  added DBCR for PPC403.  fixed mtdcr macros for PPC403.
01d,06jan95,vin  corrected HID0 (was 8, now 1008).
01c,15nov94,caf  moved register defines to toolPpc.h.
01b,04nov94,yao  changed PPC403GA to PPC403.  moved tool related macros
		 to tools.h.
01a,25may94,yao  written.
*/

#ifndef __INCasmPpch
#define __INCasmPpch

#ifdef __cplusplus
extern "C" {
#endif

/* special purpose register encoding */

#define XER		1	/* external exception register */
#define LR		8	/* link register (return address) */
#define	CTR		9	/* counter register */
#define DSISR		18	/* data storage interrupt status */
#define	DAR		19	/* data address register */
#define	DEC		22	/* decrement register */
#define SDR1		25	/* storage description register 1 */
#define SRR0		26	/* save and restore register 0 */
#define SRR1		27	/* save and restore register 1 */

#define SPRG0		272	/* software program register 0 */
#define SPRG1		273	/* software program register 1 */
#define SPRG2		274	/* software program register 2 */
#define	SPRG3		275	/* software program register 3 */
#define SPRG4       276 /* software program register 4 */
#define SPRG5       277 /* software program register 5 */
#define SPRG6       278 /* software program register 6 */
#define SPRG7       279 /* software program register 7 */

#define ASR		280	/* address space register 
				 * (64 bit implementation only) */
#define EAR		282	/* external address register */

#define TBL		284	/* lower time base register */
#define TBU		285	/* upper time base register */

#define PVR		287	/* processor version register */

#define IBAT0U		528	/* instruction BAT register */
#define IBAT0L		529	/* instruction BAT register */
#define IBAT1U		530	/* instruction BAT register */
#define IBAT1L		531	/* instruction BAT register */
#define IBAT2U		532	/* instruction BAT register */
#define IBAT2L		533	/* instruction BAT register */
#define IBAT3U		534	/* instruction BAT register */
#define IBAT3L		535	/* instruction BAT register */

#define DBAT0U		536	/* data BAT register */
#define DBAT0L		537	/* data BAT register */
#define DBAT1U		538	/* data BAT register */
#define DBAT1L		539	/* data BAT register */
#define DBAT2U		540	/* data BAT register */
#define DBAT2L		541	/* data BAT register */
#define DBAT3U		542	/* data BAT register */
#define DBAT3L		543	/* data BAT register */

#define IBAT4U		560	/* instruction BAT register */
#define IBAT4L		561	/* instruction BAT register */
#define IBAT5U		562	/* instruction BAT register */
#define IBAT5L		563	/* instruction BAT register */
#define IBAT6U		564	/* instruction BAT register */
#define IBAT6L		565	/* instruction BAT register */
#define IBAT7U		566	/* instruction BAT register */
#define IBAT7L		567	/* instruction BAT register */

#define DBAT4U		568	/* data BAT register */
#define DBAT4L		569	/* data BAT register */
#define DBAT5U		570	/* data BAT register */
#define DBAT5L		571	/* data BAT register */
#define DBAT6U		572	/* data BAT register */
#define DBAT6L		573	/* data BAT register */
#define DBAT7U		574	/* data BAT register */
#define DBAT7L		575	/* data BAT register */

#define	BO_dCTR_NZERO_AND_NOT	0
#define BO_dCTR_ZERO_AND_NOT	2
#define BO_IF_NOT		4
#define BO_dCTR_ZERO_AND	10
#define BO_IF			12
#define BO_dCTR_NZERO		16
#define BO_dCTR_ZERO		18
#define BO_ALWAYS		20
#define CR0_LT			8

/* macros to mask one bit off using rotate left word immediate then and
 * with mask instruction by setting SH to zero, MB to n+1, ME to n-1. */

#ifndef	INT_MASK
#define INT_MASK(src, des)	rlwinm	des, src, 0, 17, 15
#endif	/* INT_MASK */

#define RI_MASK(src, des)	rlwinm	des, src, 0, 31, 29

#define SE_MASK(src, des)	rlwinm	des, src, 0, 22, 20

#define IC_MASK(src, des)	rlwinm	des, src, 0, 5, 3

#ifdef __cplusplus
}
#endif

#endif /* __INCasmPpch */
