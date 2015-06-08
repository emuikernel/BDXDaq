/* mv64360Smc.h - MV64360 System Memory Controller (SMC) definitions file */

/* Copyright 1999 Wind River Systems, Inc. */
/* Copyright 1998-2003 Motorola, Inc. All Rights Reserved */

/*
modification history
--------------------
01a,31mar03,simon  Ported. (from ver 01a, mcpm905/mv64360Smc.h)
*/

/*
 * This file contains the structures and definitions used to program
 * the MV64360 system memory controller.
 *
 */

#ifndef	INCmv64360Smch
#define	INCmv64360Smch

#ifdef __cplusplus
   extern "C" {
#endif

#define MV64360_SDRAM_BANKS   	4 

/* short-cut DRAM control structure template */

#ifndef _ASMLANGUAGE

typedef struct mv64360Smc 
    {
    UINT32 csxBaseAddr [MV64360_SDRAM_BANKS]; /* Chip Select Base Addresses */
    UINT32 csxSz [MV64360_SDRAM_BANKS];       /* Chip Select Sizes */
    UINT32 baseAddrEnb;		/* Base Address Enable */
    UINT32 sdramCfg;		/* SDRAM Configuration */
    UINT32 dUnitCtrlLo;		/* DUnit Control Low */
    UINT32 sdramTmngLo;		/* SDRAM Timing (Low) */
    UINT32 sdramTmngHi;		/* SDRAM Timing (High) */
    UINT32 sdramAddrCtrl;	/* SDRAM Address Control */
    UINT32 sdramOpenPagesCtrl;	/* SDRAM Open Pages Control */
    UINT32 sdramMode;		/* SDRAM Mode */
    UINT32 sdramIfXbarCtrlLo;	/* SDRAM IF Crossbar Ctrl(Low) */
    UINT32 sdramIfXbarCtrlHi;	/* SDRAM IF Crossbar Ctrl(High) */
    UINT32 sdramIfXbarTmout;	/* SDRAM IF Crossbar Timeout */
    UINT32 sdramErrDataLo;	/* SDRAM Error Data (Low) */
    UINT32 sdramErrDataHi;	/* SDRAM Error Data (High) */
    UINT32 sdramErrAddr;	/* SDRAM Error Address */
    UINT32 sdramReceivedEcc;	/* SDRAM Received ECC */
    UINT32 sdramCalcEcc;	/* SDRAM Calculated ECC */
    UINT32 sdramEccCtrl;	/* SDRAM ECC Control */
    UINT32 sdramEccCntr;	/* SDRAM ECC Counter */	
    } MV64360_SMC;

#endif  /* _ASMLANGUAGE */

#ifdef __cplusplus
    }
#endif

#endif /* INCmv64360Smch */
