/* sysCache.h - L2 and L3 Cache Header for MPC7455 */

/* Copyright 1999-2000 Wind River Systems, Inc. */
/* Copyright 1999-2003 Motorola, Inc., All Rights Reserved. */

/*
modification history
--------------------
01d,29mar04,cak  Added support for the L3 Cache Output Hold Control
		 Register (L3OHCR - SPR 1000).
01c,05nov03,cak  BSP update.
01b,18sep02,cak  Removed (duplicate) SYNC macro.
01a,22apr02,cak  Ported from ver 01b, mcp820/sysCache.h and modified.
*/

#ifndef    INCsysCacheh
#define    INCsysCacheh

#ifdef __cplusplus
    extern "C" {
#endif /* __cplusplus */

/* includes */

/* Assembly Macros */

#define DSSALL WRS_ASM(".long 0x7E00066C")  /* explicit encoding for DSSALL */

/* L2 Cache Control Register */

#define L2CR_L2E_MASK		0x80000000	/* L2 enable */
#define L2CR_PE_MASK		0x40000000	/* L2 data parity enable */
#define L2CR_L2I_MASK		0x00200000      /* Global invalidate */
#define L2CR_L2IO_MASK		0x00100000	/* L2 Instruction-only mode */
#define L2CR_L2DO_MASK		0x00010000	/* L2 Data-only mode */
#define L2CR_REP_MASK		0x00001000	/* L2 Replacement Algorithm */
#define L2CR_HWF_MASK		0x00000800	/* L2 Hardware Flush */

/* Memory Subsystem Control Register */

#define MSSCR0_L2PFE_MASK	0x00000003	/* L2 Prefetching */

/* L2 Cache Mode Definitions */

#define L2_MODE_INST_AND_DATA	0x00000000	/* Instruction & Data allowed */
#define L2_MODE_I_ONLY		0x00100000	/* Instruction-only mode */
#define L2_MODE_D_ONLY		0x00010000	/* Data-only mode */

/* L2 Cache Replacement Algorithm Definitions */

#define L2_REP_ALG_DEFAULT	0x00000000	/* Use Default algorithm */
#define L2_REP_ALG_SECONDARY	0x00001000	/* Use Secondary algorithm */

/* MSSCR0 L2 Prefetching Definitions */

#define MSSCR0_L2PFE_NONE	0x00000000	/* Prefetching disabled */
#define MSSCR0_L2PFE_ONE	0x00000001	/* 1 prefetch engine enabled */
#define MSSCR0_L2PFE_TWO	0x00000002	/* 2 prefetch engines enabled */
#define MSSCR0_L2PFE_THREE	0x00000003	/* 3 prefetch engines enabled */

/* L3 Cache Control Register */

#define L3CR_L3E_MASK		0x80000000	/* L3 enable */
#define L3CR_L3PE_MASK		0x40000000	/* L3 data parity enable */
#define L3CR_L3APE_MASK	        0x20000000      /* L3 address parity enable */
#define L3CR_L3SIZ_MASK		0x10000000	/* L3 Size */ 
#define L3CR_L3CLKEN_MASK	0x08000000	/* L3 Clock Enable */
#define L3CR_RES_INIT_MASK	0x04000000	/* Reserved, set during init */
#define L3CR_L3CLK_MASK		0x03800000	/* L3 Clock Ratio */
#define L3CR_L3IO_MASK		0x00400000	/* L3 Instruction-only mode */
#define L3CR_L3CLKEXT_MASK      0x00200000      /* L3 Clock Ratio Extension */
#define L3CR_L3CKSPEXT_MASK     0x00010000      /* L3 Clock Sample Point Ext */
#define L3CR_L3SPO_MASK		0x00040000	/* L3 sample point override */
#define L3CR_L3CKSP_MASK	0x00030000	/* L3 clock sample point */
#define L3CR_L3PSP_MASK		0x0000E000	/* L3 P-clock sample point */
#define L3CR_L3REP_MASK		0x00001000	/* L3 replacement algorithm */
#define L3CR_L3HWF_MASK		0x00000800	/* L3 Hardware Flush */
#define L3CR_L3I_MASK		0x00000400	/* L3 Global Invalidate */
#define L3CR_L3RT_MASK		0x00000300	/* L3 SRAM Type */
#define L3CR_L3NIRCA_MASK	0x00000080	/* L3 non-int ratio clk adj */
#define L3CR_L3DO_MASK		0x00000040	/* L3 data-only mode */
#define L3CR_PMEN_MASK		0x00000004	/* Private Memory Enable */
#define L3CR_PMSIZ_MASK		0x00000001	/* Private Memory Size */

/* L3 Cache Control Bit Definitions */

#define L3CR_L3SIZ_BIT		28		/* L3CR bit 28 */
#define L3CR_L3CLK_BIT		23		/* L3CR bits 23 to 25 */
#define L3CR_L3CLKEXT_BIT       21              /* L3CR bit 21 */
#define L3CR_L3CLKSPEXT_BIT     20              /* L3CR bit 20 */
#define L3CR_L3CKSP_BIT		16		/* L3CR bits 16 to 17 */
#define L3CR_L3PSP_BIT		13		/* L3CR bits 13 to 15 */
#define L3CR_L3RT_BIT		8		/* L3CR bits 8 to 9 */
#define L3CR_L3PE_BIT		30		/* L3CR bit 30 */
#define L3CR_L3APE_BIT		29		/* L3CR bit 29 */
#define L3CR_L3SPO_BIT		18		/* L3CR bit 18 */
#define L3CR_L3NIRCA_BIT	7		/* L3CR bit 7 */
#define L3CR_PMSIZ_BIT		0		/* L3CR bit 0 */

/* L3 Cache Size Definitions */

#define L3CR_L3SIZ_1MB		0		/* 1MB */
#define L3CR_L3SIZ_2MB		1		/* 2MB */

/* L3 Cache Private Memory Definitions */

#define L3CR_PMSIZ_1MB		0		/* 1MB */
#define L3CR_PMSIZ_2MB		1		/* 2MB */

/* L3 Replacement Algorithm Definitions */

#define L3_REP_ALG_DEFAULT	0x00000000	/* Default algorithm */
#define L3_REP_ALG_SECONDARY	0x00001000	/* Secondary algorithm */

/* L3 Cache Mode Definitions */

#define L3_MODE_INST_AND_DATA	0x00000000	/* Instruction & Data allowed */
#define L3_MODE_I_ONLY		0x00400000	/* Instruction-only mode */
#define L3_MODE_D_ONLY		0x00000040	/* Data-only mode */

/* L3 Cache Output Hold Control Register Bit Definitions */

#define L3OHCR_L3DOH56_BIT	0		/* L3DOH56 */
#define L3OHCR_L3DOH48_BIT	3		/* L3DOH48 */
#define L3OHCR_L3DOH40_BIT	6		/* L3DOH40 */
#define L3OHCR_L3DOH32_BIT	9		/* L3DOH32 */
#define L3OHCR_L3DOH24_BIT	12		/* L3DOH24 */
#define L3OHCR_L3DOH16_BIT	15		/* L3DOH16 */
#define L3OHCR_L3DOH8_BIT	18		/* L3DOH8 */
#define L3OHCR_L3DOH0_BIT	21		/* L3DOH0 */
#define L3OHCR_L3CLK1_OH_BIT	24		/* L3CLK1_OH */
#define L3OHCR_L3CLK0_OH_BIT	27		/* L3CLK0_OH */
#define L3OHCR_L3AOH_BIT	30		/* L3AOH */

#ifdef __cplusplus
    }
#endif /* __cplusplus */

#endif    /* INCsysCacheh */
