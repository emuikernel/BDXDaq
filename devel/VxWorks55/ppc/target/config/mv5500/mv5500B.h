/* mv5500B.h - Motorola MVME5500 board dependency header file */

/* Copyright 1984-2000 Wind River Systems, Inc. */
/* Copyright 1999-2003 Motorola, Inc. All Rights Reserved */

/*
modification history
--------------------
01h,11mar03,cak Adjust PCI space compiler warnings after having moved
		PCI 16-bit I/O space from PCI bus 1.0 to 0.0 for IMPC
		serial support.
01g,26feb03,scb Fix check for "PCI1_MSTR_MEMIO_LOCAL must be integer 
		  multiple of PCI1_MSTR_MEMIO_SIZE".
01f,25feb03,scb Rescind (01e) below, enabling ECC fixes this.
01e,20feb03,scb Disable only data cache (not instruction cache) for
		  shared memory support.
01d,10feb03,scb Shared memory pool DRAM cache state to not cacheable,
		check setting for ANY_BRDS_IN_CHASSIS_NOT_RMW.
01c,03dec02,scb Remove all references to __asm__ to make diab compiler happy.
01b,21nov02,scb Addtional compile time checks for VME access.
01a,01oct02,scb Adapted from hxeb100 base, (ver 01b of "hxeb100B.h").
*/

/*
This file contains board header file dependencies (#ifdef's) as well as
config.h dependencies for the Motorola MVME5500 board.
*/

#ifndef	__INCmv5500Bh
#define	__INCmv5500Bh

#ifdef __cplusplus
    extern "C" {
#endif /* __cplusplus */

/* includes */

#include "mv5500A.h"
#include "mv64260Smc.h"

/* PCI I/O function defines */

#ifndef _ASMLANGUAGE

#ifndef PCI_IN_BYTE
#   define PCI_IN_BYTE(x)	sysPciInByte (x)
    IMPORT  UINT8		sysPciInByte  (UINT32 address);
#endif /* PCI_IN_BYTE */
#ifndef PCI_IN_WORD
#   define PCI_IN_WORD(x)	sysPciInWord (x)
    IMPORT  UINT16		sysPciInWord  (UINT32 address);
#endif /* PCI_IN_WORD */
#ifndef PCI_IN_LONG
#   define PCI_IN_LONG(x)	sysPciInLong (x)
    IMPORT  UINT32		sysPciInLong  (UINT32 address);
#endif /* PCI_IN_LONG */
#ifndef PCI_OUT_BYTE
#   define PCI_OUT_BYTE(x,y)	sysPciOutByte (x,y)
    IMPORT  void		sysPciOutByte (UINT32 address, UINT8  data);
#endif /* PCI_OUT_BYTE */
#ifndef PCI_OUT_WORD
#   define PCI_OUT_WORD(x,y)   sysPciOutWord (x,y)
    IMPORT  void	       sysPciOutWord (UINT32 address, UINT16 data);
#endif /* PCI_OUT_WORD */
#ifndef PCI_OUT_LONG
#   define PCI_OUT_LONG(x,y)	sysPciOutLong (x,y)
    IMPORT  void		sysPciOutLong (UINT32 address, UINT32 data);
#endif /* PCI_OUT_LONG */
#ifndef PCI_INSERT_LONG
#   define PCI_INSERT_LONG(a,m,d) sysPciInsertLong((a),(m),(d))
    IMPORT  void                  sysPciInsertLong(UINT32 adrs, UINT32 mask, \
                                                   UINT32 data);
#endif /* PCI_INSERT_LONG */
#ifndef PCI_INSERT_WORD
#   define PCI_INSERT_WORD(a,m,d) sysPciInsertWord((a),(m),(d))
    IMPORT  void                  sysPciInsertWord(UINT32 adrs, UINT16 mask, \
                                                   UINT16 data);
#endif /* PCI_INSERT_WORD */
#ifndef PCI_INSERT_BYTE
#   define PCI_INSERT_BYTE(a,m,d) sysPciInsertByte((a),(m),(d))
    IMPORT  void                  sysPciInsertByte(UINT32 adrs, UINT8 mask, \
                                                   UINT8 data);
#endif /* PCI_INSERT_BYTE */

#endif  /* _ASMLANGUAGE */

/* Common I/O synchronizing instructions */

#ifndef EIEIO_SYNC
#   define EIEIO_SYNC  WRS_ASM("eieio;sync")
#endif	/* EIEIO_SYNC */

#ifndef EIEIO
#   define EIEIO   WRS_ASM("eieio")
#endif /* WRS_ASM */

#ifndef SYNC
#   define SYNC   WRS_ASM("sync")
#endif	/* SYNC */

/* System Memory (DRAM) */

#ifdef LOCAL_MEM_AUTOSIZE
#   define DRAM_SIZE	((ULONG)sysPhysMemTop() - LOCAL_MEM_LOCAL_ADRS)
#else /* LOCAL_MEM_AUTOSIZE */
#   define DRAM_SIZE	(LOCAL_MEM_SIZE - LOCAL_MEM_LOCAL_ADRS)
#endif /* LOCAL_MEM_AUTOSIZE */

/* Vital Product Data */

#ifdef VPD_ERRORS_NONFATAL
#   define  DEFAULT_BSP_ERROR_BEHAVIOR  CONTINUE_EXECUTION
#else /* VPD_ERRORS_NONFATAL */
#   define  DEFAULT_BSP_ERROR_BEHAVIOR  EXIT_TO_SYSTEM_MONITOR
#endif /* VPD_ERRORS_NONFATAL */

/*
 * AuxClock Support
 *
 * Auxiliary Clock support is an optional feature that is not supported
 * by all BSPs. The auxiliary clock is supported by an MV64260 timer.
 */

#ifndef INCLUDE_AUX_CLK
#    undef INCLUDE_SPY
#endif /* INCLUDE_AUX_CLK */

/*
 * Compiler Errors:
 *
 * The following compiler warnings are intended to prevent an illegal
 * cache configuration.  Due to an errata on the MCP7455 processor
 * the L1 cache must be enabled if either the L2 or L3 cache is
 * enabled.  As well as prevent a user from setting non-configurable
 * bits in the registers: SDRAM Configuration, SDRAM Address Control,
 * SDRAM Open Pages Control, and SDRAM Interface Crossbar Timeout.
 */

#if ((! defined INCLUDE_CACHE_SUPPORT) && (defined INCLUDE_CACHE_L2))
#     error L1 cache must be enabled when the L2 cache is enabled
#endif

#if ((! defined INCLUDE_CACHE_SUPPORT) && (defined INCLUDE_CACHE_L3))
#     error L1 cache must be enabled when the L3 cache is enabled
#endif

#if (MV64260_SDRAM_CFG_DFLT & MV64260_SDRAM_CFG_NON_CFG_BITS)
#    error: setting invalid fields in MV64260_SDRAM_CFG_DFLT
#endif

#if (MV64260_SDRAM_ADDR_CTRL_DFLT & MV64260_SDRAM_ADDR_CTRL_NON_CFG_BITS)
#    error: setting invalid fields in MV64260_SDRAM_ADDR_CTRL_DFLT
#endif

#if (MV64260_SDRAM_IF_XBAR_TMOUT_DFLT & \
     MV64260_SDRAM_IF_XBAR_TMOUT_NON_CFG_BITS)
#    error: setting invalid fields in MV64260_SDRAM_IF_XBAR_TMOUT_DFLT
#endif

#if (MV64260_SDRAM_BANK0_PARAMS_DFLT & MV64260_SDRAM_BANK_PARAMS_NON_CFG_BITS)
#    error: setting invalid fields in MV64260_SDRAM_BANK0_PARAMS_DFLT
#endif

#if (MV64260_SDRAM_BANK1_PARAMS_DFLT & MV64260_SDRAM_BANK_PARAMS_NON_CFG_BITS)
#    error: setting invalid fields in MV64260_SDRAM_BANK1_PARAMS_DFLT
#endif

#if (MV64260_SDRAM_BANK2_PARAMS_DFLT & MV64260_SDRAM_BANK_PARAMS_NON_CFG_BITS)
#    error: setting invalid fields in MV64260_SDRAM_BANK2_PARAMS_DFLT
#endif

#if (MV64260_SDRAM_BANK3_PARAMS_DFLT & MV64260_SDRAM_BANK_PARAMS_NON_CFG_BITS)
#    error: setting invalid fields in MV64260_SDRAM_BANK3_PARAMS_DFLT
#endif

/*
 * The following compile time checks deal with PCI and VME spaces.
 * The checks are self-explanatory.
 */

#if (PCI_MEM_LOCAL_START & 0x000fffff)
#    error PCI_MEM_LOCAL_START must be multipleof 0x00100000
#endif

#if (PCI1_MSTR_MEMIO_SIZE & (PCI1_MSTR_MEMIO_SIZE - 1))
#   error PCI1_MSTR_MEMIO_SIZE must be a power of 2
#endif

#if (PCI1_MSTR_MEMIO_SIZE < 0x00100000)
#   error PCI1_MSTR_MEMIO_MEM_SIZE must be >= 0x00100000
#endif

#if (((PCI1_MSTR_MEMIO_SIZE-1) & (PCI1_MSTR_MEMIO_LOCAL)) != 0)
#   error PCI1_MSTR_MEMIO_LOCAL	must be	integer	multiple of PCI1_MSTR_MEMIO_SIZE
#endif

#if (PCI1_MSTR_MEM_SIZE & (PCI1_MSTR_MEM_SIZE - 1))
#   error PCI1_MSTR_MEM_SIZE must be a power of 2
#endif

#if (PCI1_MSTR_MEM_SIZE < 0x00100000)
#   error PCI1_MSTR_MEM_SIZE must be >= 0x00100000
#endif

#if (PCI1_MSTR_MEM_SIZE > PCI1_MSTR_MEM_LOCAL)
#   error PCI1_MSTR_MEM_LOCAL must be integer multiple of PCI1_MSTR_MEM_SIZE
#endif

#if (PCI0_IO_SIZE & (PCI0_IO_SIZE - 1))
#   error ISA_MSTR_IO_SIZE plus PCI0_MSTR_IO_SIZE must be power of 2
#endif

#if (PCI0_IO_SIZE & 0x00100000)
#   error ISA_MSTR_IO_SIZE plus PCI0_MSTR_IO_SIZE must be >= 0x00100000
#endif

#if (PCI0_IO_SIZE  > ISA_MSTR_IO_LOCAL)
#   error ISA_MSTR_IO_LOCAL must be integer multiple of (ISA_MSTR_IO_SIZE + PCI0_MSTR_IO_SIZE)
#endif

#if (PCI1_MSTR_IO_SIZE & (PCI1_MSTR_IO_SIZE - 1))
#   error PCI1_MSTR_IO_SIZE must be power of 2
#endif

#if (PCI1_MSTR_IO_SIZE & 0x00100000)
#   error PCI1_MSTR_IO_SIZE must be >= 0x00100000
#endif

#if (PCI1_MSTR_IO_SIZE > PCI1_MSTR_IO_LOCAL)
#   error PCI1_MSTR_IO_LOCAL must be integer multiple of PCI1_MSTR_IO_SIZE
#endif

#if (PCI0_MSTR_MEM_SIZE	& (PCI0_MSTR_MEM_SIZE -	1))
#   error PCI0_MSTR_MEM_SIZE must be a power of	2
#endif

#if (PCI0_MSTR_MEM_SIZE < 0x00100000)
#   error PCI0_MSTR_MEM_SIZE must be >= 0x00100000
#endif

#if (PCI0_MSTR_MEM_SIZE > PCI0_MSTR_MEM_LOCAL)
#   error PCI0_MSTR_MEM_LOCAL must be integer multiple of PCI0_MSTR_MEM_SIZE
#endif

#if (PCI0_MSTR_MEMIO_SIZE & (PCI0_MSTR_MEMIO_SIZE - 1))
#   error PCI0_MSTR_MEMIO_SIZE must be a power of 2
#endif

#if (PCI0_MSTR_MEMIO_SIZE < 0x00100000)
#   error PCI0_MSTR_MEMIO_SIZE must be >= 0x00100000
#endif

#if (PCI0_MSTR_MEMIO_SIZE > PCI0_MSTR_MEMIO_LOCAL)
#   error PCI0_MSTR_MEMIO_LOCAL	must be	integer	multiple of PCI0_MSTR_MEMIO_SIZE
#endif

#if (VME_A16_MSTR_SIZE & 0x0000ffff)
#   error VME_A16_MSTR_SIZE must be be a multiple of 0x00010000
#endif

#if (VME_A16_MSTR_SIZE < 0x00010000)
#   error VME_A16_MSTR_SIZE must be >= 0x00010000
#endif

#if (VME_A24_MSTR_SIZE & 0x0000ffff)
#   error VME_A24_MSTR_SIZE must be be a multiple of 0x00010000
#endif

#if (VME_A24_MSTR_SIZE < 0x00010000)
#   error VME_A24_MSTR_SIZE must be >= 0x00010000
#endif

#if (VME_A32_MSTR_SIZE & 0x0000ffff)
#   error VME_A32_MSTR_SIZE must be be a multiple of 0x00010000
#endif

#if (VME_A32_MSTR_SIZE < 0x00010000)
#   error VME_A32_MSTR_SIZE must be >= 0x00010000
#endif

#if (VME_MEM_LOCAL_START <  PCI0_MEM_LOCAL_END)
#   error local VME memory space extends into local PCI memory space
#endif

/* Make sure that ANY_BRDS_IN_CHASSIS_NOT_RMW is #defined, if necessary */

#ifdef INCLUDE_SM_NET
#   if (SM_OFF_BOARD == FALSE)
#       ifndef ANY_BRDS_IN_CHASSIS_NOT_RMW
#warning defining ANY_BRDS_IN_CHASSIS_NOT_RMW
#           define ANY_BRDS_IN_CHASSIS_NOT_RMW
#       endif /* ! ANY_BRDS_IN_CHASSIS_NOT_RMW */
#   endif /* SM_OFF_BOARD == FALSE */
#endif /* INCLUDE_SM_NET */

#ifdef __cplusplus
    }
#endif /* __cplusplus */

#endif /* __INCmv5500Bh */
