/* mcpm905B.h - Motorola MCPM905 board dependency header file */

/* Copyright 1984-2000 Wind River Systems, Inc. */
/* Copyright 1999-2003 Motorola, Inc. All Rights Reserved */

/*
modification history
--------------------
01b,02Jun03,simon  updated based on peer review results
01a,31mar03,simon  Ported. (from ver 01f, mcpm905/mcpm905A.h)
*/

/*
This file contains board header file dependencies (#ifdef's) as well as
config.h dependencies for the Motorola MCPM905 board.
*/

#ifndef	__INCmcpm905Bh
#define	__INCmcpm905Bh

#ifdef __cplusplus
    extern "C" {
#endif /* __cplusplus */

/* includes */

#include "prpmc880A.h"
#include "mv64360Smc.h"

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

#define NONE_CACHEABLE(address)	(address)  
#define CACHEABLE(address)		(address)  

#define REG_ADDR(offset)      (NONE_CACHEABLE(0xf1000000 | (offset)))
#define REG_CONTENT(offset)   (*(unsigned int*)REG_ADDR(offset))
#define VIRTUAL_TO_PHY(address) ((unsigned int)address)
#define PHY_TO_VIRTUAL(address) ((unsigned int)address)

/* typedefs */

#ifndef __USING_GNU_CPP__
    typedef enum _bool {false,true} bool;
#endif /* __USING_GNU_CPP__ */

/* API list for function in syslib.c */
unsigned int sysVirtToPhys(unsigned int addr);
unsigned int sysPhysToVirt(unsigned int addr);

#endif  /* _ASMLANGUAGE */

/* Common I/O synchronizing instructions */

#ifndef EIEIO_SYNC
#   define EIEIO_SYNC	WRS_ASM("eieio;sync")
#endif	/* EIEIO_SYNC */

#ifndef EIEIO
#   define EIEIO	WRS_ASM("eieio")
#endif	/* EIEIO */

#ifndef SYNC 
#   define SYNC		WRS_ASM("sync")
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
 * by all BSPs. The auxiliary clock is supported by an MV64360 timer.
 */

#ifndef INCLUDE_AUXCLK
#    undef INCLUDE_SPY
#endif /* INCLUDE_AUXCLK */

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

#if (MV64360_DDR_SDRAM_CFG_DFLT & MV64360_DDR_SDRAM_CFG_NON_CFG_BITS)
#    error: setting invalid fields in MV64360_DDR_SDRAM_CFG_DFLT 
#endif

#if (MV64360_DDR_SDRAM_ADDR_CTRL_DFLT & \
     MV64360_DDR_SDRAM_ADDR_CTRL_NON_CFG_BITS)
#    error: setting invalid fields in MV64360_DDR_SDRAM_ADDR_CTRL_DFLT
#endif

#if (MV64360_DDR_SDRAM_OPEN_PAGES_CTRL_DFLT & \
     MV64360_DDR_SDRAM_OPEN_PAGES_CTRL_NON_CFG_BITS)
#    error: setting invalid fields in MV64360_DDR_SDRAM_OPEN_PAGES_CTRL_DFLT
#endif

#if (MV64360_DDR_SDRAM_IF_XBAR_TMOUT_DFLT & \
     MV64360_DDR_SDRAM_IF_XBAR_TMOUT_NON_CFG_BITS)
#    error: setting invalid fields in \
            MV64360_DDR_SDRAM_IF_XBAR_TMOUT_DFLT
#endif

/*
 * The following compile time checks deal with PCI spaces.
 * The checks are self-explanatory.
 */

#if (PCI_MEM_LOCAL_START & 0x000fffff)
#    error PCI_MEM_LOCAL_START must be multipleof 0x00100000
#endif

#if (PCI0_MSTR_IO_SIZE & (PCI0_MSTR_IO_SIZE - 1))
#   error PCI0_MSTR_IO_SIZE must be power of 2
#endif

#if (PCI0_MSTR_IO_SIZE < 0x00100000)
#   error PCI0_MSTR_IO_SIZE must be >= 0x00100000
#endif

#if (PCI0_MSTR_IO_SIZE > PCI0_MSTR_IO_LOCAL)
#   error PCI0_MSTR_IO_LOCAL must be integer multiple of PCI0_MSTR_IO_SIZE
#endif

#if (PCI0_MSTR_MEM_SIZE & (PCI0_MSTR_MEM_SIZE - 1))
#   error PCI0_MSTR_MEM_SIZE must be a power of 2
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
#   error PCI0_MSTR_MEMIO_LOCAL must be integer multiple of PCI0_MSTR_MEMIO_SIZE
#endif

#ifdef __cplusplus
    }
#endif /* __cplusplus */

#endif /* __INCmcpm905Bh */
