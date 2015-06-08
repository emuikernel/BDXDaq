/* harrierSmc.h - Harrier System Memory Controller (SMC) definitions file */

/* Copyright 1999-2001 Wind River Systems, Inc. */
/* Copyright 1998-2001 Motorola, Inc. All Rights Reserved */

/*
modification history
--------------------
01b,20jun01,srr  Added #ifndef protection.
01a,31aug00,dmw  Written (from version 01b of mcpn765/harrierSmc.h).
*/

/*
 * The Harrier consists of a single chip that combines that same functionality
 * of the Harrier chipset and the RAVEN chip.  Accesses to the CSR are
 * internal register set, data for the external register set can be
 * written and read on both the upper and lower halves of the PPC60x data
 * bus.
 *
 * The CSR (control and status register set) consists of the chip's internal
 * register set and its external register set.  The base address of the CSR
 * is hard coded to the address $FEF80000.
 *
 */

#ifndef	INCharrierSmch
#define	INCharrierSmch

#ifdef __cplusplus
   extern "C" {
#endif

#define HARRIER_SDRAM_BANKS    8

/* short-cut DRAM control structure template */

typedef struct harrierSmc 
    {
    UINT sdramBlkAddrA;          /* Base Addressing Block A */
    UINT sdramBlkAddrB;          /* Base Addressing Block B */
    UINT sdramBlkAddrC;          /* Base Addressing Block C */
    UINT sdramBlkAddrD;          /* Base Addressing Block D */
    UINT sdramBlkAddrE;          /* Base Addressing Block E */
    UINT sdramBlkAddrF;          /* Base Addressing Block F */
    UINT sdramBlkAddrG;          /* Base Addressing Block G */
    UINT sdramBlkAddrH;          /* Base Addressing Block H */
    UINT sdramGeneralControl;    /* SDRAM General Control settings */
    UINT sdramTimingControl;     /* SDRAM Timing Control settings */
    UCHAR clkFrequency;          /* Operating clock frequency */
    } HARRIER_SMC;

#define SDRAM_CL3_DEFAULT       1      /* cl3 default              */
#define SDRAM_TRC_DEFAULT      11      /* tRC default              */
#define SDRAM_TRAS_DEFAULT      7      /* tRAS default             */
#define SDRAM_SWR_DPL_DEFAULT   1      /* swr_dpl default          */
#define SDRAM_TDP_DEFAULT       2      /* tDP default              */
#define SDRAM_TRP_DEFAULT       3      /* tRP default              */
#define SDRAM_TRCD_DEFAULT      3      /* tRCD default             */
#define SDRAM_TCYC_DEFAULT      0      /* Cycle Time default       */
#define SDRAM_DIMM_TYPE_DEFAULT 0      /* Dimm Config Type default */

#ifdef __cplusplus
    }
#endif

#endif /* INCharrierSmch */
