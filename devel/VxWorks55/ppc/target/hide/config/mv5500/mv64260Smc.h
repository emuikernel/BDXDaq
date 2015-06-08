/* mv64260Smc.h - MV64260 System Memory Controller (SMC) definitions file */

/* Copyright 1999 Wind River Systems, Inc. */
/* Copyright 1998-2002 Motorola, Inc. All Rights Reserved */

/*
modification history
--------------------
01a,01oct02,scb Copied from hxeb100 base (ver 01a).
*/

/*
 * This file contains the structures and definitions used to program
 * the MV64260 system memory controller.
 *
 */

#ifndef	INCmv64260Smch
#define	INCmv64260Smch

#ifdef __cplusplus
   extern "C" {
#endif

#define MV64260_SDRAM_BANKS   	4 

/* short-cut DRAM control structure template */

#ifndef _ASMLANGUAGE

typedef struct mv64260Smc 
    {
    UINT32 scsxLoDecodeAddr [MV64260_SDRAM_BANKS]; /* Low Decode Address */
    UINT32 scsxHiDecodeAddr [MV64260_SDRAM_BANKS]; /* High Decode Address */
    UINT32 sdramBankParams [MV64260_SDRAM_BANKS];  /* SDRAM Bank Parameters */
    UINT32 sdramCfg;		/* SDRAM Configuration */
    UINT32 sdramAddrCtrl;	/* SDRAM Address Control */
    UINT32 sdramTmngParams;	/* SDRAM Timing Parameters */
    UINT32 sdramUmaCtrl;	/* SDRAM UMA Control */
    UINT32 sdramIfXbarCtrlLo;	/* SDRAM IF Crossbar Ctrl(Low) */
    UINT32 sdramIfXbarCtrlHi;	/* SDRAM IF Crossbar Ctrl(High) */
    UINT32 sdramIfXbarTmout;	/* SDRAM IF Crossbar Timeout */
    UINT32 sdramErrDataLo;	/* SDRAM Error Data (Low) */
    UINT32 sdramErrDataHi;	/* SDRAM Error Data (High) */
    UINT32 sdramErrAddr;	/* SDRAM Error Address */
    UINT32 sdramRcvEcc;		/* SDRAM Received ECC */
    UINT32 sdramCalcEcc;	/* SDRAM Calculated ECC */
    UINT32 sdramEccCtrl;	/* SDRAM ECC Control */
    UINT32 sdramEccErrCntr;	/* SDRAM ECC Error Counter */	
    } MV64260_SMC;

#endif  /* _ASMLANGUAGE */

#ifdef __cplusplus
    }
#endif

#endif /* INCmv64260Smch */
