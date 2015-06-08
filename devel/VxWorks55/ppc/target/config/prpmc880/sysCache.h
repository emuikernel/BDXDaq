/* sysCache.h - L2 Cache Header for MPC7455 */

/* Copyright 1999-2000 Wind River Systems, Inc. */
/* Copyright 1999-2003 Motorola, Inc., All Rights Reserved. */

/*
modification history
--------------------
01a,31mar03,simon  Ported. from ver 01d mcp905/sysCache.h
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

#ifdef __cplusplus
    }
#endif /* __cplusplus */

#endif    /* INCsysCacheh */
