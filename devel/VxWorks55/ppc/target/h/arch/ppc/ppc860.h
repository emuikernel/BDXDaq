/* ppc860.h - PowerPC 860 specific header */

/* Copyright 1984-1996 Wind River Systems, Inc. */

/*
modification history
--------------------
01e,13jun02,jtp  identify class of MMU support (SPR #78396)
01d,28jul98,elg  added hardware breakpoints
01c,14feb97,tam undefined _PPC_MSR_FP and removed double macro definitions.
01b,06nov96,tpr changed _CACHE_ALIGN_SIZE to 16 (SPR #7372).
		+ added _PPC_MSR_BIT_XXX macros.
01a,08apr96,tpr written.
*/

#ifndef __INCppc860h
#define __INCppc860h

#ifdef __cplusplus
extern "C" {
#endif

/* MMU supports software TLB miss handler */
#define _WRS_TLB_MISS_CLASS_SW	1

#define _PPC_MSR_ISF_U		0x0002		/* Implementation specific fct*/

#ifdef 	_CACHE_ALIGN_SIZE
#undef	_CACHE_ALIGN_SIZE
#endif	/* _CACHE_ALIGN_SIZE */

#define	_CACHE_ALIGN_SIZE	16	/* cache line size */

/* exception offset PowerPC860 specific */

#define _EXC_OFF_SW_EMUL	0x01000		/* Software Emulation */
#define _EXC_OFF_INST_MISS	0x01100		/* Instruction TLB Miss */
#define _EXC_OFF_DATA_MISS	0x01200		/* Data TLB Miss */
#define _EXC_OFF_INST_ERROR	0x01300		/* Instruction TLB Error */
#define _EXC_OFF_DATA_ERROR	0x01400		/* Data TLB Error */
#define _EXC_OFF_DATA_BKPT	0x01c00		/* Data Breakpoint */
#define _EXC_OFF_INST_BKPT	0x01d00		/* Instruction Breakpoint */
#define	_EXC_OFF_PERI_BKPT	0x01e00		/* Peripheral Breakpoint */
#define _EXC_OFF_NM_DEV_PORT	0x01f00		/* Nom Maskable develop. port */

/* MSR bit definitions */

#undef  _PPC_MSR_FP             	/* hardware floating point unsupported*/

#define _PPC_MSR_BIT_ILE        15      /* MSR Excep little endian bit - ILE */
#define _PPC_MSR_BIT_SE         21      /* MSR Single Step Trace bit - SE */
#define _PPC_MSR_BIT_BE         22      /* MSR Branch Trace Enable bit - BE */
#define _PPC_MSR_BIT_IP         25      /* MSR Exception Prefix bit - EP */
#define _PPC_MSR_BIT_IR         26      /* MSR Inst Translation bit - IR */
#define _PPC_MSR_BIT_DR         27      /* MSR Data Translation bit - DR */
#define _PPC_MSR_BIT_RI         30      /* MSR Exception Recoverable bit - RI */

/* Special Purpose Registers PowerPC860 specific */

#define	EIE		80	/* External Interrupt Enable */
#define EID		81	/* External Interrupt Disable */
#define NRI		82	/* Non Recoverable Interrupt */
#define CMPA		144	/* Comparator A value register */
#define CMPB		145	/* Comparator B value register */
#define CMPC		146	/* Comparator C value register */
#define CMPD		147	/* Comparator D value register */
#define ICR		148	/* Interrupt Cause Register */
#define DER		149	/* Debug Enable Register*/
#define COUNTA		150	/* Breakpoint Counter A value & control reg. */
#define COUNTB		151	/* Breakpoint Counter B value & control reg. */
#define CMPE		152	/* Comparator E value register */
#define CMPF		153	/* Comparator F value register */
#define CMPG		154	/* Comparator G value register */
#define CMPH		155	/* Comparator H value register */
#define LCTRL1		156	/* Load store support comparator Control reg. */
#define LCTRL2		157	/* Load store support AND-OR Control register */
#define ICTRL		158	/* Instruction support Control register */
#define BAR		159	/* Breakpoint Address register */

#define IC_CST		560	/* Instruction Cache Control and Status reg. */
#define IC_ADR		561	/* Instruction Cache Address register */
#define IC_DAT		562	/* Instruction Cache Data port */
#define DC_CST		568	/* Data Cache Control and Status register */
#define DC_ADR		569	/* Data Cache Address register */
#define DC_DAT		570	/* Data Cache Data port*/

#define DPDR		630	/* Development Port Data Register */
#define DPIR		631	/* */
#define IMMR		638	/* */

#define MI_CTR		784	/* Instruction MMU Control Register */
#define MI_AP		786	/* Instruction MMU Access Protection Register */
#define MI_EPN		787	/* Instruction MMU Effective Number Register */
#define MI_TWC		789	/* Instruction MMU Tablewalk control Register */
#define MI_RPN		790	/* Instruction MMU Real Page Number Port */

#define MD_CTR		792	/* Data MMU Control Register */
#define M_CASID		793	/* CASID register */
#define MD_AP		794	/* Data Access Protection Register */
#define MD_EPN		795	/* Data Effective Number Register */
#define M_TWB		796	/* MMU Tablewalk base register */
#define MD_TWC		797	/* Data Tablewalk control Register */
#define MD_RPN		798	/* Data Real Page Number Port */
#define M_TW		799	/* MMU Tablewalk special register */

#define MI_DBCAM	816	/* Instruction MMU CAM entry read register */
#define MI_DBRAM0	817	/* Instruction MMU RAM entry read register 0 */
#define MI_DBRAM1	818	/* Instruction MMU RAM entry read register 1 */
#define MD_DBCAM	824	/* Data MMU CAM entry read register */
#define MD_DBRAM0	825	/* Data MMU RAM entry read register 0 */
#define MD_DBRAM1	826	/* Data MMU RAM entry read register 1 */

/* comparators masks */

#define _PPC_CMP_MSK		~(0x3)

/* LCTRL1 bit definitions */

/* compare types */

#define _PPC_LCTRL1_CT_EQ	0x4		/* compare type equal */
#define _PPC_LCTRL1_CT_LT	0x5		/* compare type less than */
#define _PPC_LCTRL1_CT_GT	0x6		/* compare type greater than */
#define _PPC_LCTRL1_CT_NE	0x7		/* compare type not equal */

/* access to compare types */

#define _PPC_LCTRL1_CTE(x)	(((x) << 29) & 0xE0000000)
#define _PPC_LCTRL1_CTF(x)	(((x) << 26) & 0x1C000000)
#define _PPC_LCTRL1_CTG(x)	(((x) << 23) & 0x03800000)
#define _PPC_LCTRL1_CTH(x)	(((x) << 20) & 0x00700000)

/* select match on read/write of comparators E and F */

#define _PPC_LCTRL1_CRWE(x)	(((x) << 18) & 0x000C0000)
#define _PPC_LCTRL1_CRWF(x)	(((x) << 16) & 0x00030000)
#define _PPC_LCTRL1_TYPE_E(x)	(((x) & 0x000C0000) >> 18)
#define _PPC_LCTRL1_TYPE_F(x)	(((x) & 0x00030000) >> 16)

/* LCTRL2 bit definitions */

/* first load/store watchpoint */

#define _PPC_LCTRL2_LW0EN	0x80000000	/* watchpoint enabled */

#define _PPC_LCTRL2_LW0IA_IW0	0x00000000	/* IW0 selection */
#define _PPC_LCTRL2_LW0IA_IW1	0x20000000	/* IW1 selection */
#define _PPC_LCTRL2_LW0IA_IW2	0x40000000	/* IW2 selection */
#define _PPC_LCTRL2_LW0IA_IW3	0x60000000	/* IW3 selection */
#define _PPC_LCTRL2_LW0IADC	0x10000000	/* care instruction events */

#define _PPC_LCTRL2_LW0LA_MSK	0x0C000000	/* address event mask */
#define _PPC_LCTRL2_LW0LA_E	0x00000000	/* match from comparator E */
#define _PPC_LCTRL2_LW0LA_F	0x04000000	/* match from comparator F */
#define _PPC_LCTRL2_LW0LA_E_AND_F	0x08000000	/* match from E&F */
#define _PPC_LCTRL2_LW0LA_E_OR_F	0x0C000000	/* match from E|F */
#define _PPC_LCTRL2_LW0LADC	0x02000000	/* care address events */

#define _PPC_LCTRL2_LW0LD_G	0x00000000	/* match from comparator G */
#define _PPC_LCTRL2_LW0LD_H	0x00800000	/* match from comparator H */
#define _PPC_LCTRL2_LW0LD_G_AND_H	0x01000000	/* match from G&H */
#define _PPC_LCTRL2_LW0LD_G_OR_H	0x01800000	/* match from G|H */
#define _PPC_LCTRL2_LW0LDDC	0x00400000	/* care data events */

#define _PPC_LCTRL2_DLW0EN	0x00000008	/* development port trap */
#define _PPC_LCTRL2_SLW0EN	0x00000002	/* software trap enabled */

/* second load/store watchpoint */

#define _PPC_LCTRL2_LW1EN	0x00200000	/* watchpoint enabled */

#define _PPC_LCTRL2_LW1IA_IW0	0x00000000	/* IW0 selection */
#define _PPC_LCTRL2_LW1IA_IW1	0x00080000	/* IW1 selection */
#define _PPC_LCTRL2_LW1IA_IW2	0x00100000	/* IW2 selection */
#define _PPC_LCTRL2_LW1IA_IW3	0x00180000	/* IW3 selection */
#define _PPC_LCTRL2_LW1IADC	0x00040000	/* care instruction events */

#define _PPC_LCTRL2_LW1LA_E	0x00000000	/* match from comparator E */
#define _PPC_LCTRL2_LW1LA_F	0x00010000	/* match from comparator F */
#define _PPC_LCTRL2_LW1LA_E_AND_F	0x00020000	/* match from E&F */
#define _PPC_LCTRL2_LW1LA_E_OR_F	0x00030000	/* match from E|F */
#define _PPC_LCTRL2_LW1LADC	0x00008000	/* care address events */

#define _PPC_LCTRL2_LW1LD_G	0x00000000	/* match from comparator G */
#define _PPC_LCTRL2_LW1LD_H	0x00002000	/* match from comparator H */
#define _PPC_LCTRL2_LW1LD_G_AND_H	0x00004000	/* match from G&H */
#define _PPC_LCTRL2_LW1LD_G_OR_H	0x00006000	/* match from G|H */
#define _PPC_LCTRL2_LW1LDDC	0x00001000	/* care data events */

#define _PPC_LCTRL2_BRKNOMSK	0x00000800	/* nonmasked mode */

#define _PPC_LCTRL2_DLW1EN	0x00000004	/* development port trap */
#define _PPC_LCTRL2_SLW1EN	0x00000001	/* software trap enabled */

/* ICTRL bit definitions */

/* the different compare types */

#define _PPC_ICTRL_CT_EQ	0x4		/* compare type equal */
#define _PPC_ICTRL_CT_LT	0x5		/* compare type less than */
#define _PPC_ICTRL_CT_GT	0x6		/* compare type greater than */
#define _PPC_ICTRL_CT_NE	0x7		/* compare type not equal */

/* access to the compare types */

#define _PPC_ICTRL_CTA(x)	(((x) << 29) & 0xE0000000)
#define _PPC_ICTRL_CTB(x)	(((x) << 26) & 0x1C000000)
#define _PPC_ICTRL_CTC(x)	(((x) << 23) & 0x03800000)
#define _PPC_ICTRL_CTD(x)	(((x) << 20) & 0x00700000)

/* logic operations between comparators */

#define _PPC_ICTRL_IW0_A	0x00080000	/* match from comparator A */
#define _PPC_ICTRL_IW0_A_AND_B	0x000C0000	/* match from comparators A&B */
#define _PPC_ICTRL_IW1_B	0x00020000	/* match from comparator B */
#define _PPC_ICTRL_IW1_A_OR_B	0x00030000	/* match from comparators A|B */
#define _PPC_ICTRL_IW2_C	0x00008000	/* match from comparator C */
#define _PPC_ICTRL_IW2_C_AND_D	0x0000C000	/* match from comparators C&D */
#define _PPC_ICTRL_IW3_D	0x00002000	/* match from comparator D */
#define _PPC_ICTRL_IW3_C_OR_D	0x00003000	/* match from comparators C|D */

/* software trap enable selection */

#define _PPC_ICTRL_SIWEN_MSK	0x00000F00	/* software trap mask */
#define _PPC_ICTRL_SIW0EN	0x00000800	/* software trap of IW0 */
#define _PPC_ICTRL_SIW1EN	0x00000400	/* software trap of IW1 */
#define _PPC_ICTRL_SIW2EN	0x00000200	/* software trap of IW2 */
#define _PPC_ICTRL_SIW3EN	0x00000100	/* software trap of IW3 */

/* mask of used bits for hardware breakpoints */

#define _PPC_ICTRL_HWBP_MSK	0xFFFFFF00

#ifdef __cplusplus
}
#endif

#endif /* __INCppc860h */
