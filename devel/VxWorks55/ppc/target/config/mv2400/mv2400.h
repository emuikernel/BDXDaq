/* mv2400.h - Motorola PowerPlus board header */

/* Copyright 1984-2001 Wind River Systems, Inc. */
/* Copyright 1996,1997,1998,1999 Motorola, Inc. All Rights Reserved */

/*
modification history
--------------------
01v,05nov01,dat  Remove extra ';'s, Diab was choking
01u,16sep01,dat  Use of WRS_ASM macro
01t,19nov99,srr  Removed VME_A32_MSTR_LOCAL, moved to config.h
01s,16jul99,rhv  Correcting error in PCI_CLINE_SZ definition.
01r,09jul99,rhv  Added #defines for Hawk configurations constants.
01q,01jul99,rhv  Incorporating WRS code review changes.
01p,19may99,rhv  Fixing an error in file history and correcting some comment
                 errors.
01o,14may99,rhv  Set INV bit in Hawk PCI slave attributes to correct a live-lock
                 problem related to Universe II RMW cycles and the use of the
                 PCI LOCK# signal.
01n,12may99,rhv  Changing PCI2DRAM_BASE_ADRS to PCI_SLV_MEM_BUS as part of
                 WRS PCI symbol update.
01m,12may99,rhv  Modified to use WRS PCI symbols.
01l,27jan99,mdp  Added 21143 doze bit for SENS support.
01k,27jan99,rhv  Changing name of ROM startup errors to be consistent with
                 WRS naming (Bootrom instead of RomStart).
01j,22jan99,rhv  Adding ROM startup error reporting.
01i,22jan99,mdp  Moved MPC750 CCR defines to sysCache.h.
01h,21jan99,dmw  Add masks for addressing extened vs. non-extended ranges.
01g,21jan99,rhv  Adding processor bus parity support.
01f,20jan99,rhv  Adjusting value of CPU2PCI_ADDR0_START_VAL to eliminate 2603
                 work-around.
01e,19jan99,rhv  Removing obsolete definitions.
01d,14jan99,mdp  Fix for Hawk Aux Clock.
01c,14jan99,dmw  Add PCI_ID_IDE for device exclusion. 
01b,13jan99,rhv  Added SPD EEPROM address, new Hawk register definitions
                 and updated copyright.
01a,15dec98,mdp  written. (from ver 02h, mv2600.h)

*/

/*
This file contains I/O addresses and related constants for the
Motorola PowerPlus VME board. 
*/

#ifndef	INCmv240xh
#define	INCmv240xh

#ifdef __cplusplus
extern "C" {
#endif

#define INCLUDE_PCI		/* always include pci */

/* Boot Line parameters are stored in the 2nd 256 byte block */

#undef	NV_BOOT_OFFSET
#define NV_BOOT_OFFSET		256 /* skip 1st 256 bytes */
#define NV_RAM_SIZE    		BBRAM_SIZE		
#define NV_RAM_ADRS    		((char *) BBRAM_ADRS)
#define NV_RAM_INTRVL           1

/* PMC-Span setup values */

#define P2P_IO_SPACE_BASE_ADRS  ((P2P_IO_BASE & 0x0000F000) >> 8)
#define P2P_IO_SPACE_LIMIT_ADRS ((P2P_IO_BASE + P2P_IO_SIZE - 1) & 0x0000F000)
#define	P2P_IO_HI16_BASE_ADRS   ((P2P_IO_BASE & 0xFFFF0000) >> 16)
#define	P2P_IO_HI16_LIMIT_ADRS  ((P2P_IO_BASE + P2P_IO_SIZE - 1) & 0xFFFF0000)
#define P2P_NPMEM_SPACE_BASE_ADRS ((P2P_NONPREF_MEM_BASE & 0xFFFF0000) >> 16)
#define P2P_NPMEM_SPACE_LIMIT_ADRS ((P2P_NONPREF_MEM_BASE + \
                                     P2P_NONPREF_MEM_SIZE - 1) & 0xFFFF0000)
#define P2P_PREF_MEM_BASE_ADRS  ((P2P_PREF_MEM_BASE & 0xFFFF0000) >> 16)
#define P2P_PREF_MEM_LIMIT_ADRS ((P2P_PREF_MEM_BASE + P2P_PREF_MEM_SIZE - 1) & \
                                 0xFFFF0000)

/* PCI I/O function defines */

#define INT_NUM_IRQ0            INT_VEC_IRQ0

#ifndef _ASMLANGUAGE

extern  UINT sysGetBusSpd (void);

#ifndef PCI_IN_BYTE
#define PCI_IN_BYTE(x)          sysPciInByte (x)
IMPORT  UINT8                   sysPciInByte  (UINT32 address);
#endif
#ifndef PCI_IN_WORD
#define PCI_IN_WORD(x)          sysPciInWord (x)
IMPORT  UINT16                  sysPciInWord  (UINT32 address);
#endif
#ifndef PCI_IN_LONG
#define PCI_IN_LONG(x)          sysPciInLong (x)
IMPORT  UINT32                  sysPciInLong  (UINT32 address);
#endif
#ifndef PCI_OUT_BYTE
#define PCI_OUT_BYTE(x,y)       sysPciOutByte (x,y)
IMPORT  void                    sysPciOutByte (UINT32 address, UINT8  data);
#endif
#ifndef PCI_OUT_WORD
#define PCI_OUT_WORD(x,y)       sysPciOutWord (x,y)
IMPORT  void                    sysPciOutWord (UINT32 address, UINT16 data);
#endif
#ifndef PCI_OUT_LONG
#define PCI_OUT_LONG(x,y)       sysPciOutLong (x,y)
IMPORT  void                    sysPciOutLong (UINT32 address, UINT32 data);
#endif

#endif  /* _ASMLANGUAGE */

/* Cache Line Size - 8 32-bit value = 32 bytes */

#define PCI_CLINE_SZ		(_CACHE_ALIGN_SIZE/4)

/* Latency Timer value - 255 PCI clocks */

#define PCI_LAT_TIMER		0xff

/* clock rates */

/* Calculate Memory Bus Rate in Hertz */

#define MEMORY_BUS_SPEED                ( sysGetBusSpd() )

/* System clock (decrementer counter) frequency determination */

#define DEC_CLOCK_FREQ          ( sysGetBusSpd() )

/*
 * The PowerPC Decrementer is used as the system clock.
 * It is always included in this BSP.  The following defines
 * are used by the system clock library.
 */

#define SYS_CLK_RATE_MIN  	10		/* minimum system clock rate */
#define SYS_CLK_RATE_MAX  	5000		/* maximum system clock rate */

/*
 * This macro returns the positive difference between two signed ints.
 * Useful for determining delta between two successive decrementer reads.
 */

#define DELTA(a,b)	( abs((int)a - (int)b) )

/*
 * Auxiliary Clock support is an optional feature that is not supported
 * by all BSPs.  The following defines are used by the aux clock library.
 */

#define AUX_CLK_RATE_MIN  	40		/* min auxiliary clock */
#define AUX_CLK_RATE_MAX	5000		/* max auxiliary clock rate */

/*
 * Shared Memory Interrupt Type.
 * Interrupt this target with a 1-byte write mailbox.
 * VME_A32 space, address based on procNum, value is SIG1_INTR_SET.
 */

#define SM_INT_ARG1 	VME_AM_EXT_SUP_DATA
#define SM_INT_ARG2 	(VME_A32_REG_BASE +(sysProcNumGet() * VME_A32_REG_SIZE))
#define SM_INT_ARG3	SIG1_INTR_SET

/*
 * Semaphore Test-and-Set Register as seen from a slave
 * Only used with a special version of sysBusTas().
 */

#define OFFBRD_VME_SEM_REG1	(CPU_VME_WINDOW_REG_BASE + \
				 (CPU_VME_SEM_REG1 - CPU_VME_HW_REGS_BASE)) 

/* Common I/O synchronizing instructions */

#ifndef EIEIO_SYNC
# define EIEIO_SYNC  WRS_ASM (" eieio; sync")
#endif  /* EIEIO_SYNC */
#ifndef EIEIO
# define EIEIO    WRS_ASM (" eieio")
#endif  /* EIEIO */

/* Translation macro */

#define TRANSLATE(x,y,z)\
        ((UINT)(x) - (UINT)(y) + (UINT)(z))

/*
 * Legacy ISA space size. Reserved for kybd, com1, com2,...
 */

#define ISA_LEGACY_SIZE 0x00004000

#ifndef EXTENDED_VME

/* Pseudo PREP memory map (maximizes available PCI addressing space) */

/* ISA I/O space within PCI I/O space (includes ISA legacy space) */

#define ISA_MSTR_IO_LOCAL	0x80000000
#define ISA_MSTR_IO_BUS		0x00000000	/* must be zero */
#define ISA_MSTR_IO_SIZE	0x00010000	/* 64 kbytes */

/*
 * PCI I/O space (above ISA I/O space)
 *
 * NOTE: (PCI_MSTR_IO_LOCAL+PCI_MSTR_IO_SIZE) must be less than
 *       PCI_MSTR_MEMIO_LOCAL to prevent overlap. PCI_MSTR_IO_SIZE
 *       is defined in config.h
 */

#define PCI_MSTR_IO_LOCAL   (ISA_MSTR_IO_LOCAL+ISA_MSTR_IO_SIZE)
#define PCI_MSTR_IO_BUS     (TRANSLATE(PCI_MSTR_IO_LOCAL,\
                                       ISA_MSTR_IO_LOCAL,\
                                       ISA_MSTR_IO_BUS))

/*
 * PCI Memory I/O (non-prefetchable)
 *
 * NOTE: PCI_MSTR_MEMIO_SIZE is defined in config.h
 */

#define PCI_MSTR_MEMIO_LOCAL	0xc0000000      /* base of ISA mem space */
#define PCI_MSTR_MEMIO_BUS	0x00000000	/* must be zero */

/*
 * PCI (pre-fetchable) Memory space (above PCI I/O Memory space)
 *
 * NOTE: (PCI_MSTR_MEM_LOCAL+PCI_MSTR_MEM_SIZE) must be less than
 *        VME_A32_MSTR_LOCAL to prevent overlap. PCI_MSTR_MEM_SIZE is
 *        defined in config.h
 */

#define PCI_MSTR_MEM_LOCAL	(PCI_MSTR_MEMIO_LOCAL+PCI_MSTR_MEMIO_SIZE)
#define PCI_MSTR_MEM_BUS	(TRANSLATE(PCI_MSTR_MEM_LOCAL,\
                                           PCI_MSTR_MEMIO_LOCAL,\
                                           PCI_MSTR_MEMIO_BUS))

#else

/* Extended VME memory map (maximizes VME A32 space) */

/* ISA I/O space within PCI I/O space (includes ISA legacy space) */

#define ISA_MSTR_IO_LOCAL	0xfe000000  /* base of ISA I/O space */
#define ISA_MSTR_IO_BUS		0x00000000  /* must be zero */
#define ISA_MSTR_IO_SIZE	0x00010000  /* 64 kbytes (includes legacy) */

/*
 * PCI I/O space (above ISA I/O space)
 *
 * NOTE: (PCI_MSTR_IO_LOCAL+PCI_MSTR_IO_SIZE) must be less than
 *       HAWK_SMC_BASE_ADRS to prevent overlap. PCI_MSTR_IO_SIZE
 *       is defined in config.h
 */

#define PCI_MSTR_IO_LOCAL	(ISA_MSTR_IO_LOCAL+ISA_MSTR_IO_SIZE)
#define PCI_MSTR_IO_BUS         (TRANSLATE(PCI_MSTR_IO_LOCAL,\
                                           ISA_MSTR_IO_LOCAL,\
                                           ISA_MSTR_IO_BUS))

/*
 * PCI Memory I/O (non-prefetchable)
 *
 * NOTE: PCI_MSTR_MEMIO_SIZE is defined in config.h
 */

#define PCI_MSTR_MEMIO_LOCAL	0xfd000000      /* base of ISA mem space */
#define PCI_MSTR_MEMIO_BUS	PCI_MSTR_MEMIO_LOCAL	/* 1-1 mapping */

/*
 * PCI (pre-fetchable) Memory space (above PCI I/O Memory space)
 *
 * NOTE: (PCI_MSTR_MEM_LOCAL+PCI_MSTR_MEM_SIZE) must be less than
 *        ISA_MSTR_IO_LOCAL to prevent overlap. PCI_MSTR_MEM_SIZE is
 *        defined in config.h
 */

#define PCI_MSTR_MEM_LOCAL	(PCI_MSTR_MEMIO_LOCAL+PCI_MSTR_MEMIO_SIZE)
#define PCI_MSTR_MEM_BUS	(TRANSLATE(PCI_MSTR_MEM_LOCAL,\
                                           PCI_MSTR_MEMIO_LOCAL,\
                                           PCI_MSTR_MEMIO_BUS))
#endif

/*
 * PCI MASTER MEMORY WINDOW LIMITS
 *
 * These values are strictly defined by the base memory addresses and window
 * sizes of the spaces defined above.  These values must be correct for the
 * sysBusProbe() memory range checks for the PCI bus to work properly.
 */

#ifndef EXTENDED_VME
#  define PCI_MSTR_LO_ADRS      (ISA_MSTR_IO_LOCAL)
#  define PCI_MSTR_HI_ADRS      (PCI_MSTR_MEM_LOCAL + PCI_MSTR_MEM_SIZE)
#else
#  define PCI_MSTR_LO_ADRS      (PCI_MSTR_MEMIO_LOCAL)
#  define PCI_MSTR_HI_ADRS      (PCI_MSTR_IO_LOCAL + PCI_MSTR_IO_SIZE)
#endif  /* EXTENDED_VME */

#ifndef INCLUDE_MPIC

#define MPIC_ADDR(reg)		(MPIC_BASE_ADRS + reg)
#define MPIC_GLOBAL_CONFIG_REG	0x01020
#define RESET_CNTRLR		0x80000000

#endif  /* INCLUDE_MPIC */

/*
 * Base address of HW devices as seen from CPU.
 *
 * Note: The HAWK ASIC is a combined FALCON/RAVEN. So the HAWK SMC
 * (System Memory Contoller) is equivalent to the FALCON and the
 * HAWK PHB (PCI Host Bridge) is equivalent to the RAVEN.
 */

#define HAWK_SMC_BASE_ADRS	0xfef80000
#define	HAWK_SMC_REG_SIZE	0x00010000
#define HAWK_SMC_BASE_UPPER_ADRS	(HAWK_SMC_BASE_ADRS>>16)
#define HAWK_SMC_SDRAM_ATTR_AD	0xfef80010
#define HAWK_SMC_SDRAM_BASE_AD	0xfef80018
#define HAWK_SMC_CLOCK_FREQ	0xfef80020
#define HAWK_SMC_SDRAM_ATTR_EH	0xfef800c0
#define HAWK_SMC_SDRAM_BASE_EH	0xfef800c8
#define HAWK_SMC_SDRAM_CNTRL	0xfef800d0

#define HAWK_PHB_BASE_ADRS	0xfeff0000
#define	HAWK_PHB_REG_SIZE	0x00010000
#define	FLASH_BASE_ADRS		0xFF000000
#define	FLASH_MEM_SIZE		0x01000000

/* MPIC configuration defines */

#define MPIC_BASE_ADRS		0xfc000000
#define MPIC_REG_SIZE		0x00040000
#ifndef EXTENDED_VME 
#  define MPIC_PCI_BASE_ADRS	(TRANSLATE(MPIC_BASE_ADRS,\
                                           PCI_MSTR_MEMIO_LOCAL,\
                                           PCI_MSTR_MEMIO_BUS))
#else
#  define MPIC_PCI_BASE_ADRS	MPIC_BASE_ADRS
#endif /* Extended VME config */

/* memory map as seen on the PCI bus */

#define PCI_SLV_MEM_LOCAL	LOCAL_MEM_LOCAL_ADRS

#ifndef EXTENDED_VME
#  define PCI_SLV_MEM_BUS	0x80000000      /* memory seen from PCI bus */
#else
/* Sergey: we have this */
#  define PCI_SLV_MEM_BUS	0x00000000      /* memory seen from PCI bus */
#endif /* Extended VME config */

#define PCI_SLV_MEM_SIZE	DRAM_SIZE

#define PCI2DRAM_BASE_ADRS	PCI_SLV_MEM_BUS

/*
 * Primary PCI bus configuration space address and data register addresses
 * as seen by the CPU on the local bus.
 */

#define PCI_PRIMARY_CAR	(ISA_MSTR_IO_LOCAL+0xcf8)
#define PCI_PRIMARY_CDR (ISA_MSTR_IO_LOCAL+0xcfc)

/* Special dec21143 configuration device driver area register */

#define PCI_CFG_21143_DA        0x40

/* PCI Device/Vendor IDs */

#define PCI_ID_HAWK             0x48031057      /* Id for HAWK PHB */
#define PCI_ID_IBC              0x056510ad      /* Id for W83C553 PIB */
#define PCI_ID_IDE              0x010510ad      /* Id for IDE */
#define PCI_ID_UNIVERSE         0x000010e3      /* Id for Universe VME chip */
#define PCI_ID_LN_DEC21143      0x00191011      /* Id DEC chip 21143 */
#define PCI_ID_PRI_LAN          PCI_ID_LN_DEC21143 /* Id for Primary LAN */
#define PCI_ID_BR_DEC21150      0x00221011      /* Id DEC 21150 PCI bridge */

/* PCI Space Definitions  -- For configuring the Hawk */

/* CPU to PCI definitions */

#define CPU2PCI_MSATT_MEM	0x00c2
#define CPU2PCI_MSATT_IO	0x00c0

#ifndef EXTENDED_VME

/* STANDARD ( PREP ) mapping of PCI space */

/* setup address space 0 for PCI MEM space */
#define CPU2PCI_ADDR0_START_VAL	0xfd00
#define CPU2PCI_ADDR0_END_VAL	0xfdff
#define CPU2PCI_OFFSET0_VAL	((0x0-CPU2PCI_ADDR0_START_VAL) & 0xffff)

/* setup address space 1 for PCI I/O space */
#define CPU2PCI_ADDR1_START_VAL	0xfe00
#define CPU2PCI_ADDR1_END_VAL	0xfe7f
#define CPU2PCI_OFFSET1_VAL	((0x0-CPU2PCI_ADDR1_START_VAL) & 0xffff)
#define CPU2PCI_MSATT1_VAL	CPU2PCI_MSATT_IO

/* setup address space 2 for PCI MEM space -- maps VME address space */
#define CPU2PCI_ADDR2_START_VAL	(PCI_MSTR_MEMIO_LOCAL >>16)
#define CPU2PCI_ADDR2_END_VAL	0xfcff
#define CPU2PCI_OFFSET2_VAL	((0x0-CPU2PCI_ADDR2_START_VAL) & 0xffff)

/* setup address space 3 for PCI I/O space */
#define CPU2PCI_ADDR3_START_VAL	(ISA_MSTR_IO_LOCAL >>16)
#define CPU2PCI_ADDR3_END_VAL	0xbf7f
#define CPU2PCI_OFFSET3_VAL	((0x0-CPU2PCI_ADDR3_START_VAL) & 0xffff)

#else

/* EXTENDED VME PCI mapping */

/* setup address space 0 for PCI MEM space -- maps VME address space */
#define CPU2PCI_ADDR0_START_VAL	(VME_A32_MSTR_LOCAL>>16)
#define CPU2PCI_ADDR0_END_VAL	0xfbff
#define CPU2PCI_OFFSET0_VAL	0x0

/* setup address space 1 for PCI MEM, for MPIC regs */
#define CPU2PCI_ADDR1_START_VAL	(MPIC_BASE_ADRS >>16)
#define CPU2PCI_ADDR1_END_VAL	(((MPIC_BASE_ADRS + 0x00ffffff) \
                                  >> 16) & 0x0000ffff)
#define CPU2PCI_OFFSET1_VAL	0x0
#define CPU2PCI_MSATT1_VAL	CPU2PCI_MSATT_MEM

/* setup address space 2 for PCI MEM -- maps reg. space */
#define CPU2PCI_ADDR2_START_VAL	(PCI_MSTR_MEMIO_LOCAL >>16)
#define CPU2PCI_ADDR2_END_VAL	(((PCI_MSTR_MEM_LOCAL+PCI_MSTR_MEM_SIZE-1)\
                                   >> 16) & 0x0000ffff)
#define CPU2PCI_OFFSET2_VAL	0x0

/* setup address space 3 for PCI I/O */
#define CPU2PCI_ADDR3_START_VAL	(ISA_MSTR_IO_LOCAL >>16)
#define CPU2PCI_ADDR3_END_VAL	(((PCI_MSTR_IO_LOCAL + PCI_MSTR_IO_SIZE-1) \
                                   >> 16) & 0x0000ffff)
#define CPU2PCI_OFFSET3_VAL	((0x0-CPU2PCI_ADDR3_START_VAL) & 0xffff)

#endif

/* defines that are used in hawkPhb.c */

#define CPU2PCI_ADDR0_START	CPU2PCI_ADDR0_START_VAL
#define CPU2PCI_ADDR0_END	CPU2PCI_ADDR0_END_VAL
#define CPU2PCI_OFFSET0		CPU2PCI_OFFSET0_VAL
#define CPU2PCI_MSATT0		CPU2PCI_MSATT_MEM
#define CPU2PCI_ADDR1_START	CPU2PCI_ADDR1_START_VAL
#define CPU2PCI_ADDR1_END	CPU2PCI_ADDR1_END_VAL
#define CPU2PCI_OFFSET1		CPU2PCI_OFFSET1_VAL
#define CPU2PCI_MSATT1		CPU2PCI_MSATT1_VAL
#define CPU2PCI_ADDR2_START	CPU2PCI_ADDR2_START_VAL
#define CPU2PCI_ADDR2_END	CPU2PCI_ADDR2_END_VAL
#define CPU2PCI_OFFSET2		CPU2PCI_OFFSET2_VAL
#define CPU2PCI_MSATT2		CPU2PCI_MSATT_MEM
#define CPU2PCI_ADDR3_START	CPU2PCI_ADDR3_START_VAL
#define CPU2PCI_ADDR3_END	CPU2PCI_ADDR3_END_VAL
#define CPU2PCI_OFFSET3		CPU2PCI_OFFSET3_VAL
#define CPU2PCI_MSATT3		CPU2PCI_MSATT_IO

/* PCI to CPU definitions */

#ifdef LOCAL_MEM_AUTOSIZE
# define DRAM_SIZE		((ULONG)sysPhysMemTop() - LOCAL_MEM_LOCAL_ADRS)
#else
# define DRAM_SIZE		(LOCAL_MEM_SIZE - LOCAL_MEM_LOCAL_ADRS)
#endif

/* Hawk Slave Window Attributes */

#define HAWK_PCI_SLV_ATTR_REN_BIT   7	/* allow reads from PCI bus */
#define HAWK_PCI_SLV_ATTR_WEN_BIT   6	/* allow writes from PCI bus */
#define HAWK_PCI_SLV_ATTR_WPEN_BIT  5	/* enable write posting */
#define HAWK_PCI_SLV_ATTR_RAEN_BIT  4	/* enable read-ahead */
#define HAWK_PCI_SLV_ATTR_GBL_BIT   1	/* mark read and write cycles global */
#define HAWK_PCI_SLV_ATTR_INV_BIT   0	/* enable invalidate transactions */

#define HAWK_PCI_SLV_ATTR_REN_MASK	(1<<HAWK_PCI_SLV_ATTR_REN_BIT)
#define HAWK_PCI_SLV_ATTR_WEN_MASK	(1<<HAWK_PCI_SLV_ATTR_WEN_BIT)
#define HAWK_PCI_SLV_ATTR_WPEN_MASK	(1<<HAWK_PCI_SLV_ATTR_WPEN_BIT)
#define HAWK_PCI_SLV_ATTR_RAEN_MASK	(1<<HAWK_PCI_SLV_ATTR_RAEN_BIT)
#define HAWK_PCI_SLV_ATTR_GBL_MASK	(1<<HAWK_PCI_SLV_ATTR_GBL_BIT)
#define HAWK_PCI_SLV_ATTR_INV_MASK	(1<<HAWK_PCI_SLV_ATTR_INV_BIT)

/* Hawk PCI Slave Window definitions */

#define PCI2CPU_ADDR0_START     (PCI_SLV_MEM_BUS & 0xffff0000)
#define PCI2CPU_ADDR0_END       ((PCI_SLV_MEM_BUS + DRAM_SIZE - 1) >> 16)
#define PCI2CPU_ADDR0_RANGE     (PCI2CPU_ADDR0_START | PCI2CPU_ADDR0_END)
#define PCI2CPU_OFFSET0         (((0x0-PCI_SLV_MEM_BUS)>>16) & 0x0000ffff)
#define PCI2CPU_ATT0            (HAWK_PCI_SLV_ATTR_REN_MASK  | \
				 HAWK_PCI_SLV_ATTR_WEN_MASK  | \
				 HAWK_PCI_SLV_ATTR_WPEN_MASK | \
				 HAWK_PCI_SLV_ATTR_RAEN_MASK | \
				 HAWK_PCI_SLV_ATTR_GBL_MASK  | \
				 HAWK_PCI_SLV_ATTR_INV_MASK)

/*
 * Address decoders 1, 2 and 3 are not currently used, so they are
 * set to point to an address that is not used on the PCI bus
 */
#define PCI2CPU_ADDR1_RANGE     0xfff0fff0
#define PCI2CPU_OFFSET1         0x0
#define PCI2CPU_ATT1            0x0
#define PCI2CPU_ADDR2_RANGE     0xfff0fff0
#define PCI2CPU_OFFSET2         0x0
#define PCI2CPU_ATT2            0x0
#define PCI2CPU_ADDR3_RANGE     0xfff0fff0
#define PCI2CPU_OFFSET3         0x0
#define PCI2CPU_ATT3            0x0

/*
 * Allocate PCI Memory and I/O Space Offsets for PCI devices
 *
 * All devices on the primary and secondary busses are allocated 64 kb spaces.
 * The PMC Span can control up to four PMCs.
 */

#define SCSI_DEV_SPACE		0x10000
#define LAN_DEV_SPACE		0x20000
#define VGA_DEV_SPACE		0x30000
#define PMC_DEV_SPACE		0x40000
#define VME_DEV_SPACE		0x50000
#define LAN2_DEV_SPACE		0x60000
#define SCSI_DEV_SPACE2		0x70000
#define	SPAN_IO_DEV_SPACE	0x80000
#define	SPAN_IO_DEV_SIZE	0x40000
#define SPAN_MEM_DEV_SPACE	0x02000000
#define SPAN_MEM_DEV_SIZE	0x01000000
#define SPAN_PREF_DEV_SPACE	0x01000000
#define SPAN_PREF_DEV_SIZE	0x01000000

/* PCI view of PCI I/O Space for PCI devices */

#define PCI_IO_SCSI_ADRS	(PCI_MSTR_IO_BUS + SCSI_DEV_SPACE)	/* 53C825 */
#define PCI_IO_LN_ADRS		(PCI_MSTR_IO_BUS + LAN_DEV_SPACE)	/* PCnet */
#define PCI_IO_VGA_ADRS		(PCI_MSTR_IO_BUS + VGA_DEV_SPACE)	/* GD5434 */
#define PCI_IO_PMC_ADRS		(PCI_MSTR_IO_BUS + PMC_DEV_SPACE)	/* extra */
#define PCI_IO_LN2_ADRS		(PCI_MSTR_IO_BUS + LAN2_DEV_SPACE)  /* 21040   */
#define PCI_IO_SCSI_ADRS2	(PCI_MSTR_IO_BUS + SCSI_DEV_SPACE2) /* 53C825A */

/* PCI view of PCI Memory Space for PCI devices */

#define PCI_MEM_UNIVERSE_ADRS    (PCI_MSTR_MEM_BUS + VME_DEV_SPACE)	/* UNIVERSE */

/* Allocated base address of HW devices as seen from CPU */

#define SCSI_BASE_ADRS		( PCI_MSTR_IO_LOCAL + SCSI_DEV_SPACE )
#define LAN_BASE_ADRS		( PCI_MSTR_IO_LOCAL + LAN_DEV_SPACE )
#define UNIVERSE_BASE_ADRS	( PCI_MSTR_MEM_LOCAL + VME_DEV_SPACE )
#define LAN2_BASE_ADRS		( PCI_MSTR_IO_LOCAL + LAN2_DEV_SPACE )
#define SCSI_BASE_ADRS2		( PCI_MSTR_IO_LOCAL + SCSI_DEV_SPACE2 )

/* z8536 aux timer and I/O chip */

#define z8536_PORTC_DATA        (ISA_MSTR_IO_LOCAL + 0x0844)
#define z8536_PORTB_DATA        (ISA_MSTR_IO_LOCAL + 0x0845)
#define z8536_PORTA_DATA        (ISA_MSTR_IO_LOCAL + 0x0846)
#define z8536_PORT_CTRL         (ISA_MSTR_IO_LOCAL + 0x0847)
#define ZCIO_CNTRL_ADRS         (UINT8 *)(ISA_MSTR_IO_LOCAL + 0x847)
#define ZCIO_IACK_ADRS          (UINT8 *)(ISA_MSTR_IO_LOCAL + 0x84F)

/* m48TXX non volatile ram, RTC and watchdog timer */

#define m48TXX_LSB_REG		(ISA_MSTR_IO_LOCAL + 0x0074)
#define	m48TXX_MSB_REG		(ISA_MSTR_IO_LOCAL + 0x0075)
#define m48TXX_DAT_REG		(ISA_MSTR_IO_LOCAL + 0x0077)

/* CPU type */

#define CPU_TYPE                ((vxPvrGet() >> 16) & 0xffff)
#define CPU_TYPE_601		0x01	/* PPC 601 CPU */
#define CPU_TYPE_602		0x02	/* PPC 602 CPU */
#define CPU_TYPE_603		0x03	/* PPC 603 CPU */
#define CPU_TYPE_603E           0x06    /* PPC 603e CPU */
#define CPU_TYPE_603P           0x07    /* PPC 603p CPU */
#define CPU_TYPE_750            0x08    /* PPC 750 CPU */
#define CPU_TYPE_604		0x04	/* PPC 604 CPU */
#define CPU_TYPE_604E		0x09	/* PPC 604e CPU */
#define CPU_TYPE_604R		0x0A	/* PPC 604r CPU */
#define CPU_TYPE_MAX            0x0C    /* PPC MAX CPU */

/* Vital Product Data Support */

#ifdef MV2400
#define SPD_EEPROM_ADRS 0xa8        /* i2c address of first SPD EEPROM */
#define VPD_BRD_EEPROM_ADRS 0xa0    /* i2c address of board's SROM */
#define VPD_BRD_OFFSET      0   /* offset into board's eeprom for vpd data */
#define VPD_PKT_LIMIT       25  /* Max number of packets expected */

#define DEFAULT_PCI_CLOCK	33333333
#define DEFAULT_BUS_CLOCK	100000000
#define DEFAULT_INTERNAL_CLOCK	350000000
#define DEFAULT_PRODUCT_ID	"Unknown"
#endif

/* Product Configuration Options (PCO) */

#define PCO_PCI0_CONN1		0
#define PCO_PCI0_CONN2		1
#define PCO_PCI0_CONN3		2
#define PCO_PCI0_CONN4		3

#define PCO_PCI1_CONN1		4
#define PCO_PCI1_CONN2		5
#define PCO_PCI1_CONN3		6
#define PCO_PCI1_CONN4		7

#define PCO_ISA_CONN1		8
#define PCO_ISA_CONN2		9
#define PCO_ISA_CONN3		10
#define PCO_ISA_CONN4		11

#define PCO_EIDE1_CONN1		12
#define PCO_EIDE1_CONN2		13
#define PCO_EIDE2_CONN1		14
#define PCO_EIDE2_CONN2		15

#define PCO_ENET1_CONN		16
#define PCO_ENET2_CONN		17
#define PCO_ENET3_CONN		18
#define PCO_ENET4_CONN		19

#define PCO_SCSI1_CONN		20
#define PCO_SCSI2_CONN		21
#define PCO_SCSI3_CONN		22
#define PCO_SCSI4_CONN		23

#define PCO_SERIAL1_CONN	24
#define PCO_SERIAL2_CONN	25
#define PCO_SERIAL3_CONN	26
#define PCO_SERIAL4_CONN	27

#define PCO_FLOPPY_CONN1	28
#define PCO_FLOPPY_CONN2	29
#define PCO_PARALLEL1_CONN	30
#define PCO_PARALLEL2_CONN	31

#define PCO_PMC1_IO_CONN	32
#define PCO_PMC2_IO_CONN	33
#define PCO_USB0_CONN		34
#define PCO_USB1_CONN		35

#define PCO_KEYBOARD_CONN	36
#define PCO_MOUSE_CONN		37
#define PCO_VGA1_CONN		38
#define PCO_SPEAKER_CONN	39

#define PCO_VME_CONN		40
#define PCO_CPCI_CONN		41
#define PCO_ABORT_SWITCH	42
#define PCO_BDFAIL_LIGHT	43

#define PCO_SWREAD_HEADER	44
#define PCO_MEMMEZ_CONN		45
#define PCO_PCI0EXP_CONN	46

/* BSP configuration error policy */

#define CONTINUE_EXECUTION	0       /* Tolerate VPD/Configuration errors */
#define EXIT_TO_SYSTEM_MONITOR	1       /* Transfer to System Monitor */

#ifdef VPD_ERRORS_NONFATAL
#    define     DEFAULT_BSP_ERROR_BEHAVIOR  CONTINUE_EXECUTION
#else
#    define     DEFAULT_BSP_ERROR_BEHAVIOR  EXIT_TO_SYSTEM_MONITOR
#endif

/* z8536 I/O port bit mapping */

#define z8536_PORTA_BRDFAIL     0x40
#define z8536_PORTB_ABORT       0x80

/*
 * Hawk Extensions to Standard PCI Header
 *
 * Type declarations for the PCI Header and the macros in regards to the
 * PCI BUS.  These definitions have been made with respect to PCI LOCAL
 * BUS SPECIFICATION REVISION 2.1.  Every device on the PCI BUS has to
 * support 256 bytes of configuration space of which the first 64 bytes
 * are a predefined header portion defined by the PCI commitee.  Bytes
 * 64 to 255 are dedicated to the device specific registers.
 *
 * Note: the PCI bus is inherently little endian. 
 */

#define PCI_CFG_HAWK_PSADD0	0x80
#define PCI_CFG_HAWK_PSATT0	0x84
#define PCI_CFG_HAWK_PSOFF0	0x86
#define PCI_CFG_HAWK_PSADD1	0x88
#define PCI_CFG_HAWK_PSATT1	0x8c
#define PCI_CFG_HAWK_PSOFF1	0x8e
#define PCI_CFG_HAWK_PSADD2	0x90
#define PCI_CFG_HAWK_PSATT2	0x94
#define PCI_CFG_HAWK_PSOFF2	0x96
#define PCI_CFG_HAWK_PSADD3	0x98
#define PCI_CFG_HAWK_PSATT3	0x9c
#define PCI_CFG_HAWK_PSOFF3	0x9e

/* Hawk MPC registers */

#define HAWK_MPC_VENID		0x00
#define HAWK_MPC_DEVID		0x02
#define HAWK_MPC_REVID		0x05
#define HAWK_MPC_GCSR		0x08
#define HAWK_MPC_FEAT		0x0a
#define HAWK_MPC_MARB		0x0c
#define HAWK_MPC_PADJ		0x13
#define HAWK_MPC_MEREN		0x22
#define HAWK_MPC_MERST		0x27
#define HAWK_MPC_MERAD		0x28
#define HAWK_MPC_MERAT		0x2e
#define HAWK_MPC_PIACK		0x30
#define HAWK_MPC_MSADD0	0x40
#define HAWK_MPC_MSOFF0	0x44
#define HAWK_MPC_MSATT0	0x47
#define HAWK_MPC_MSADD1	0x48
#define HAWK_MPC_MSOFF1	0x4c
#define HAWK_MPC_MSATT1	0x4f
#define HAWK_MPC_MSADD2	0x50
#define HAWK_MPC_MSOFF2	0x54
#define HAWK_MPC_MSATT2	0x57
#define HAWK_MPC_MSADD3	0x58
#define HAWK_MPC_MSOFF3	0x5c
#define HAWK_MPC_MSATT3	0x5f
#define HAWK_MPC_WDT1CNTL      0x60
#define HAWK_MPC_WDT2CNTL      0x68
#define HAWK_MPC_GPREG0_U	0x70
#define HAWK_MPC_GPREG0_L	0x74
#define HAWK_MPC_GPREG1_U	0x78
#define HAWK_MPC_GPREG1_L	0x7c

/*
 * Bootrom error bits.
 * These bits are set during ROM startup before error annunciation is available
 * to save error conditions for later reporting.
 */

/* bits in Group A (Hawk GPREG0_U [0x0070]) */

#define BOOTROM_NO_VPD_BUS_SPEED       0x80000000     /* Couldn't read VPD */
#define BOOTROM_DEFAULT_SMC_TIMING     0x40000000     /* Couldn't read SPD */

/*
 * Hawk MPC General Status and Configuration bits
 */

#define HAWK_MPC_GCSR_OPIC_ENA	0x0020

/*
 * Hawk MPC Arbiter control bits.
 *
 * Enable round-robin PCI bus arbitration, with PCI bus parked on Hawk,
 * PPC bus parked on CPU0 (only one on this board) and park the arbiter
 * at any PCI bus master which has successfully locked the PCI bus.
 */

#define HAWK_MPC_MARB_VAL	0x00206041

/*
 * Hawk register bit masks
 *
 * Bits marked with 'C' indicate conditions which can be cleared by
 * writing a 1 to the bits.
 */

/* Hawk MPC Error Enable (MEREN) register bit masks */

#define HAWK_MPC_MEREN_RTAI	0x0001	/* PCI mstr Recvd Target Abort Int */
#define HAWK_MPC_MEREN_SMAI	0x0002	/* PCI mstr Signld Target Abort Int */
#define HAWK_MPC_MEREN_SERRI	0x0004	/* PCI System Error Int */
#define HAWK_MPC_MEREN_PERRI	0x0008	/* PCI Parity Error Int */
#define HAWK_MPC_MEREN_XDPEI    0x0010  /* MPC Data Parity Error Int */
#define HAWK_MPC_MEREN_MATOI	0x0020	/* MPC Address Bus Time-out Int */
#define HAWK_MPC_MEREN_RTAM	0x0100	/* RTAI machine check enable */
#define HAWK_MPC_MEREN_SMAM	0x0200	/* SMAI machine check enable */
#define HAWK_MPC_MEREN_SERRM	0x0400	/* SERRI machine check enable */
#define HAWK_MPC_MEREN_PERRM	0x0800	/* PERRI machine check enable */
#define HAWK_MPC_MEREN_XDPEM	0x1000	/* XDPEI machine check enable */
#define HAWK_MPC_MEREN_MATOM	0x2000	/* MATOI machine check enable */
#define HAWK_MPC_MEREN_DFLT	0x4000	/* Default MPC Master ID select */
#define HAWK_MPC_MEREN_VALID	0x7F3F	/* Mask for valid MEREN bits */

/* Hawk MPC Error Status (MERST) register bit masks */

#define HAWK_MPC_MERST_RTA	0x01	/* C PCI mstr Recvd Target Abort */
#define HAWK_MPC_MERST_SMA	0x02	/* C PCI mstr Signld Target Abort */
#define HAWK_MPC_MERST_SERR	0x04	/* C PCI System Error */
#define HAWK_MPC_MERST_PERR	0x08	/* C PCI Parity Error */
#define HAWK_MPC_MERST_XDPE	0x10	/* C MPC Data Parity Error */
#define HAWK_MPC_MERST_MATO	0x20	/* C MPC Address Bus Time-out */
#define HAWK_MPC_MERST_OVF	0x80	/* C Error Status Overflow */
#define HAWK_MPC_MERST_VALID	0xBF	/*   Mask for valid MERST bits */
#define HAWK_MPC_MERST_CLR	0xBF	/*   Clears all errors */

/* Hawk PCI Configuration Status register bit masks */

#define HAWK_PCI_CFG_STATUS_FAST    0x0010  /*   Fast back-to-back capable */
#define HAWK_PCI_CFG_STATUS_DPAR    0x0100  /* C Data prity error detected */
#define HAWK_PCI_CFG_STATUS_SELTIM0 0x0200  /*   Device select timing bit 0 */
#define HAWK_PCI_CFG_STATUS_SELTIM1 0x0400  /*   Device select timing bit 1 */
#define HAWK_PCI_CFG_STATUS_SIGTA   0x0800  /* C Signalled Target Abort */
#define HAWK_PCI_CFG_STATUS_RCVTA   0x1000  /* C Received Target Abort */
#define HAWK_PCI_CFG_STATUS_RCVMA   0x2000  /* C Received Master Abort */
#define HAWK_PCI_CFG_STATUS_SIGSE   0x4000  /* C Signalled System Error */
#define HAWK_PCI_CFG_STATUS_RCVPE   0x8000  /* C Detected Parity Error */
#define HAWK_PCI_CFG_STATUS_VALID   0xFF10  /*   Valid status bits */
#define HAWK_PCI_CFG_STATUS_CLR     0xF900  /*   Clears all conditions */

/* Hawk PCI Configuration Command register bit masks */

#define HAWK_PCI_CFG_CMD_IOSP       0x0001  /* IO Space Enable */
#define HAWK_PCI_CFG_CMD_MEMSP      0x0002  /* Memory Space Enable */
#define HAWK_PCI_CFG_CMD_MSTR       0x0004  /* Bus Master Enable */
#define HAWK_PCI_CFG_CMD_PERR       0x0040  /* Parity Err Response Enable */
#define HAWK_PCI_CFG_CMD_SERR       0x0100  /* System Error Enable */

/*
 * Hawk SMC Register Offsets
 */

#define HAWK_SMC_DPE_LOG_REG 0x68
#define HAWK_SMC_DPE_ENA_REG 0x6a
#define HAWK_SMC_APE_LOG_REG 0xe0
#define HAWK_SMC_APE_ENA_REG 0xe2

#define HAWK_SMC_DPELOG    (1 << 7) /* assuming byte-wide access */
#define HAWK_SMC_DPE_CKALL (1 << 1) /* assuming byte-wide access */
#define HAWK_SMC_DPE_ME    (1 << 0) /* assuming byte-wide access */

#define HAWK_SMC_APELOG    (1 << 7) /* assuming byte-wide access */
#define HAWK_SMC_APE_ME    (1 << 0) /* assuming byte-wide access */

/*
 * PMC Span DEC21150 PCI-to-PCI Bridge device-specific registers
 *
 * These registers are in Configuration Space and are extensions to a
 * standard type 1 PCI header.
 */

#define PCI_CFG_DEC21150_CHIP_CTRL 0x40
#define PCI_CFG_DEC21150_DIAG_CTRL 0x41
#define PCI_CFG_DEC21150_ARB_CTRL  0x42
#define PCI_CFG_DEC21150_EVNT_DSBL 0x64
#define PCI_CFG_DEC21150_GPIO_DOUT 0x65
#define PCI_CFG_DEC21150_GPIO_CTRL 0x66
#define PCI_CFG_DEC21150_GPIO_DIN  0x67
#define PCI_CFG_DEC21150_SEC_CLK   0x68     /* secondary clock controll reg */
#define PCI_CFG_DEC21150_SERR_STAT 0x6A

/* programmable interrupt controller (PIC) */

#define	PIC_REG_ADDR_INTERVAL	1	/* address diff of adjacent regs. */

/* programmable interrupt timers */

#define PIT_BASE_ADR		SL82565_TMR1_CNT0	/* timeraddrs */
#define PIT_REG_ADDR_INTERVAL	1
#define PIT_CLOCK		1193180

/* serial ports (COM1 - COM4) */

#ifdef INCLUDE_I8250_SIO
#  define COM1_BASE_ADR	(ISA_MSTR_IO_LOCAL+0x3f8)	/* serial port 1 */
#  define COM2_BASE_ADR	(ISA_MSTR_IO_LOCAL+0x2f8)	/* serial port 2 */
#  define UART_REG_ADDR_INTERVAL  1       	/* addr diff of adjacent regs */
#  define N_UART_CHANNELS 	1		/* No. serial I/O channels */
#endif /* INCLUDE_I8250_SIO */

/* total number of serial ports */

#  define N_SIO_CHANNELS	1		/* No. serial I/O channels */

/* non-volatile (battery-backed) ram defines
 *
 * the top 16 bytes are used for the RTC registers
 */

#define	BBRAM_ADRS		0		/* base address */
#define	BBRAM_SIZE		0x1ff0		/* 8k NVRAM Total Size */

/* factory ethernet address */

#define	BB_ENET			((char *)(BBRAM_ADRS + 0x1f2c))

/* MK48TXX register settings */

/* flag register */

#define MK48T_FLAGS		((char *)(BBRAM_ADRS + 0x1ff0))

/* alarm clock registers, 4 1byte locations */

#define ALARM_CLOCK		((char *)(BBRAM_ADRS + 0x1ff2))

/* interrupt register */

#define MK48T_INTR		((char *)(BBRAM_ADRS + 0x1ff6))

/* watchdog timer register */

#define WD_TIMER		((char *)(BBRAM_ADRS + 0x1ff7))

/* MK48TXX bb time of day clk, 8 1byte locations */

#define	TOD_CLOCK 		((char *)(BBRAM_ADRS + 0x1ff8))

#define NV_RAM_IO_MAPPED	/* nvram is io mapped in ISA space */
#define NV_RAM_READ(x)		sysNvRead (x)
#define NV_RAM_WRITE(x,y)	sysNvWrite (x,y)
#define NV_RAM_LSB_REG		m48TXX_LSB_REG
#define NV_RAM_MSB_REG		m48TXX_MSB_REG
#define NV_RAM_DAT_REG		m48TXX_DAT_REG

/*
 * UNIVERSE REGISTER SETUP FOR CPU <-> VME MAPPING (via PCI)
 *
 * ALL VAL_LSIx and VAL_VSIx values MUST be multiples of 64KB !!!
 * Except VAL_LSI0 and VAL_VSI0, which must be multiples of 4KB !
 */

/*
 * VME MASTER WINDOW FOR LM/SEM (MAILBOX) REGISTERS
 *
 * Universe PCI slave (VME master) window 0
 * 
 * Map access to A32 VMEbus (VME LM/SEM Regs) (64K)
 */

#define VME_A32_REG_SIZE	0x00001000	/* individual reg space */
#define	VME_A32_REG_SPACE	0x00010000	/* total reg space */

#ifndef EXTENDED_VME
/*
 * Map access to A32 VMEbus (LM/SEM Regs) - 64K  MAP FOR STANDARD vxWorks
 * This maps: MPU RANGE:     0xf0000000 - 0xf000ffff  
 *        to: PCI RANGE:     0x30000000 - 0x3000ffff
 *        to: VME RANGE:     0x40000000 - 0x4000ffff
 */
#  define CPU_VME_WINDOW_REG_BASE	0xf0000000
#  define VME_A32_REG_BASE		0x40000000
#  define VAL_LSI0_BS_VALUE	(CPU_VME_WINDOW_REG_BASE - PCI_MSTR_MEMIO_LOCAL)
#else
/*
 * Map access to A32 VMEbus (LM/SEM Regs) - 64K  MAP FOR EXTENDED VME vxWorks
 * This maps: MPU RANGE:     0xfb000000 - 0xfb00ffff
 *        to: PCI RANGE:     0xfb000000 - 0xfb00ffff
 *        to: VME RANGE:     0xfb000000 - 0xfb00ffff
 */
#  define CPU_VME_WINDOW_REG_BASE	0xfb000000
#  define VME_A32_REG_BASE	0xfb000000
#  define VAL_LSI0_BS_VALUE	(CPU_VME_WINDOW_REG_BASE)
#endif
#define VAL_LSI0_BS		(VAL_LSI0_BS_VALUE)
#define VAL_LSI0_BD		(VAL_LSI0_BS + VME_A32_REG_SPACE)
#define VAL_LSI0_TO		(VME_A32_REG_BASE - VAL_LSI0_BS)
#define VAL_LSI0_CTL		( LSI_CTL_EN     | LSI_CTL_WP  |\
                                  LSI_CTL_D64    | LSI_CTL_A32 |\
                                  LSI_CTL_DATA   | LSI_CTL_USR |\
                                  LSI_CTL_SINGLE | LSI_CTL_PCI_MEM )

/*
 * Semaphore Test-and-Set Register as seen from a slave.
 * Only used with a special version of sysBusTas() (not included)
 * and when mv2300, mv2600 or mv3600 boards are the ONLY boards
 * used in the configuration.
 */

#define OFFBRD_VME_SEM_REG1     (CPU_VME_WINDOW_REG_BASE + \
                                 (CPU_VME_SEM_REG1 - CPU_VME_HW_REGS_BASE)) 

/*
 * VME MASTER WINDOW FOR A24 SPACE
 *
 * Universe PCI slave (VME master) window 2
 */

#ifndef EXTENDED_VME
/*
 * Map access to A24 VMEbus - 16M       MAP FOR STANDARD vxWorks
 * This maps: MPU RANGE:     0xe0000000 - 0xe0ffffff
 *        to: PCI RANGE:     0x20000000 - 0x20ffffff
 *        to: VME RANGE:     0xff000000 - 0xffffffff
 */
#  define VME_A24_MSTR_LOCAL	0xe0000000
#  define VAL_LSI2_BS_VALUE    (VME_A24_MSTR_LOCAL - PCI_MSTR_MEMIO_LOCAL)
#else
/*
 * Map access to A24 VMEbus - 16M       MAP FOR EXTENDED VME vxWorks
 * This maps: MPU RANGE:     0xfa000000 - 0xfaffffff
 *        to: PCI RANGE:     0xfa000000 - 0xfaffffff
 *        to: VME RANGE:     0xff000000 - 0xffffffff
 */
#  define VME_A24_MSTR_LOCAL	0xfa000000
#  define VAL_LSI2_BS_VALUE	(VME_A24_MSTR_LOCAL)
#endif
#  if  (VME_A24_MSTR_SIZE > 0x01000000)
#  error Maximum VME_A24_MSTR_SIZE cannot exceed 16 MB
#  endif
#define VAL_LSI2_BS	(VAL_LSI2_BS_VALUE)
#define VAL_LSI2_BD	(VAL_LSI2_BS + VME_A24_MSTR_SIZE)
#define VAL_LSI2_TO	(0xff000000 + VME_A24_MSTR_BUS - VAL_LSI2_BS)
#define VAL_LSI2_CTL	( LSI_CTL_EN     | LSI_CTL_WP  |\
                          LSI_CTL_D64    | LSI_CTL_A24 |\
                          LSI_CTL_DATA   | LSI_CTL_USR |\
                          LSI_CTL_SINGLE | LSI_CTL_PCI_MEM )

/*
 * VME MASTER WINDOW FOR A32 SPACE
 *
 * Universe PCI slave (VME master) window 1
 */

#ifndef EXTENDED_VME
/*                      
 * Map access to A32 VMEbus - 128M      MAP FOR STANDARD vxWorks
 * This maps: MPU RANGE: 0xd8000000       - 0xdfffffff  
 *        to: PCI RANGE: 0x18000000       - 0x1fffffff
 *        to: VME RANGE: VME_A32_MSTR_BUS - VME_A32_MSTR_BUS + VME_A32_MSTR_SIZE
 */
#  define VAL_LSI1_BS_VALUE	(VME_A32_MSTR_LOCAL - PCI_MSTR_MEMIO_LOCAL)
#  if  (VME_A32_MSTR_SIZE > 0x08000000)
#  error VME_A32_MSTR_SIZE cannot exceed 128 MB
#  endif
#else
/*
 * Map access to A32 VMEbus - (up to ~3.7GB)  MAP FOR EXTENDED VME vxWorks
 * This maps: MPU RANGE: VME_A32_MSTR_LOCAL  - VME_A32_MSTR_LOCAL +
 *                                             VME_A32_MSTR_SIZE
 *        to: PCI RANGE: VME_A32_MSTR_LOCAL  - VME_A32_MSTR_LOCAL +
 *                                             VME_A32_MSTR_SIZE
 *        to: VME RANGE: VME_A32_MSTR_BUS    - VME_A32_MSTR_BUS +
 *                                             VME_A32_MSTR_SIZE
 *
 * NOTE:
 *  - VME_A32_MSTR_LOCAL + VME_A32_MSTR_SIZE must not exceed
 *    VME_A24_MSTR_LOCAL (no overlap of A32 and A24 address spaces)
 *  - for local memory sizes greater than 256 MB, set VME_A32_MSTR_LOCAL equal
 *    to the memory size.
 */
#  define VAL_LSI1_BS_VALUE	(VME_A32_MSTR_LOCAL)
#  if  ((VME_A32_MSTR_LOCAL + VME_A32_MSTR_SIZE) > VME_A24_MSTR_LOCAL)
#  error VME A32 space extends into VME A24 space
#  endif
#endif
#  if  ((VME_A32_MSTR_BUS + VME_A32_MSTR_SIZE) > VME_A32_REG_BASE) && \
       (VME_A32_MSTR_BUS < (VME_A32_REG_BASE + VME_A32_REG_SPACE))
#  warning VME A32 space overlaps VME LM/SEM (MAILBOX) REG space
#  endif

#define VAL_LSI1_BS	(VAL_LSI1_BS_VALUE)
#define VAL_LSI1_BD	(VAL_LSI1_BS + VME_A32_MSTR_SIZE)
#define VAL_LSI1_TO	(VME_A32_MSTR_BUS - VAL_LSI1_BS)   /*  LSI_CTL_WP  | */
#define VAL_LSI1_CTL	( LSI_CTL_EN     | LSI_CTL_WP  |\
                          LSI_CTL_D64    | LSI_CTL_A32 |\
                          LSI_CTL_DATA   | LSI_CTL_USR |\
                          LSI_CTL_SINGLE | LSI_CTL_PCI_MEM )

/*
 * VME MASTER WINDOW FOR A16 SPACE
 *
 * Universe PCI slave (VME master) window 3
 */

#ifndef EXTENDED_VME
/*
 * Map access to A16 VMEbus - 64K       MAP FOR STANDARD vxWorks
 * This maps: MPU RANGE:     0xefff0000 - 0xefffffff  
 *        to: PCI RANGE:     0x2fff0000 - 0x2fffffff
 *        to: VME RANGE:     0xffff0000 - 0xffffffff
 */
#define VME_A16_MSTR_LOCAL	0xefff0000
#define VAL_LSI3_BS_VALUE	(VME_A16_MSTR_LOCAL - PCI_MSTR_MEMIO_LOCAL)
#else
/*
 * Map access to A16 VMEbus - 64K       MAP FOR EXTENDED VME vxWorks
 * This maps: MPU RANGE:     0xfbff0000 - 0xfbffffff
 *        to: PCI RANGE:     0xfbff0000 - 0xfbffffff
 *        to: VME RANGE:     0xfbff0000 - 0xfbffffff
 */
#  define VME_A16_MSTR_LOCAL	0xfbff0000
#  define VAL_LSI3_BS_VALUE	(VME_A16_MSTR_LOCAL)
#endif
#  if  (VME_A16_MSTR_SIZE > 0x00010000)
#  error Maximum VME_A16_MSTR_SIZE cannot exceed 64 KB
#  endif
#define	VME_A16_MSTR_BUS	0x00000000	/* must be 0 */
#define VAL_LSI3_BS		(VAL_LSI3_BS_VALUE)
#define VAL_LSI3_BD		(VAL_LSI3_BS + VME_A16_MSTR_SIZE)
#define VAL_LSI3_TO		(0xffff0000 + VME_A16_MSTR_BUS - VAL_LSI3_BS)
#define VAL_LSI3_CTL		( LSI_CTL_EN     | LSI_CTL_WP  |\
                                  LSI_CTL_D64    | LSI_CTL_A16 |\
                                  LSI_CTL_DATA   | LSI_CTL_USR |\
                                  LSI_CTL_SINGLE | LSI_CTL_PCI_MEM )

/*
 * VME MASTER MEMORY WINDOW LIMITS
 *
 * These values are strictly defined by the base memory addresses and window
 * sizes of the spaces defined above.  These values must be correct for the
 * sysBusProbe() memory range checks of the VME bus to work properly.
 */

#ifndef EXTENDED_VME
#  define VME_MSTR_LO_ADRS      (VME_A32_MSTR_LOCAL)
#  define VME_MSTR_HI_ADRS      (CPU_VME_WINDOW_REG_BASE + VME_A32_REG_SPACE)
#else
#  define VME_MSTR_LO_ADRS      (VME_A32_MSTR_LOCAL)
#  define VME_MSTR_HI_ADRS      (VME_A16_MSTR_LOCAL + VME_A16_MSTR_SIZE)
#endif  /* EXTENDED_VME */

/*
 * VME SLAVE WINDOW FOR REG SPACE
 *
 * Universe VME slave window 0.
 *
 * Setup to access the node's VME LM/SEM Regs via
 * A32 space on VMEbus (4KB)
 * This range needs to be unique for each target board.
 */

#ifndef EXTENDED_VME
/*
 *                      MAP FOR STANDARD vxWorks 
 * This maps: VME RANGE:     (0x40000000 + (0x1000 * ProcNum)) -
 *                           (0x40000fff + (0x1000 * ProcNum))
 *        to: PCI RANGE:     0x00001000 - 0x00001fff - ISA I/O Space
 */
#define VAL_VSI0_TO_VALUE	(0xc0001000 - (sysProcNumGet() * \
                                               VME_A32_REG_SIZE))
#else
/*
 *                      MAP FOR EXTENDED VME vxWorks
 * This maps: VME RANGE:     (0xfb000000 + (0x1000 * ProcNum)) -
 *                           (0xfb000fff + (0x1000 * ProcNum))
 *        to: PCI RANGE:     0x00001000 - 0x00001fff
 */
#define VAL_VSI0_TO_VALUE	(0x05001000 - (sysProcNumGet() * \
                                               VME_A32_REG_SIZE))
#endif

#define VAL_VSI0_BS	(VME_A32_REG_BASE + (sysProcNumGet() * \
                                             VME_A32_REG_SIZE))
#define VAL_VSI0_BD	(VME_A32_REG_BASE + ((sysProcNumGet() + 1) * \
                                             VME_A32_REG_SIZE))
#define VAL_VSI0_TO	(VAL_VSI0_TO_VALUE)
#define VAL_VSI0_CTL	(VSI_CTL_EN       | \
                         VSI_CTL_AM_DATA  | VSI_CTL_AM_PGM  |\
                         VSI_CTL_AM_SUPER | VSI_CTL_AM_USER |\
                         VSI_CTL_VAS_A32  | VSI_CTL_LAS_IO )

/*
 * VME SLAVE WINDOW FOR A32 SPACE
 *
 * Universe VME slave window 1
 *
 * VME bus A32 window to access the master node's local memory.
 * This VME Slave window is only used by the master node.
 *
 *
 *                      MAP FOR STANDARD vxWorks 
 * This maps: VME RANGE:     0x08000000 - (0x08000000 + VME_A32_SLV_SIZE - 1)
 *        to: PCI RANGE:     0x80000000 - 0x80000000 + VME_A32_SLV_SIZE-1
 *        to: MPU RANGE:     0x00000000 - VME_A32_SLV_SIZE-1
 *
 *
 *                      MAP FOR EXTENDED VME vxWorks
 * This maps: VME RANGE:     0x08000000 - (0x08000000 + VME_A32_SLV_SIZE - 1)
 *        to: PCI RANGE:     0x00000000 - VME_A32_SLV_SIZE-1
 *        to: MPU RANGE:     0x00000000 - VME_A32_SLV_SIZE-1
 */

#define VAL_VSI1_BS	(VME_A32_SLV_BUS)

#if	(SM_OFF_BOARD == TRUE)
#  define VAL_VSI1_BD	(VAL_VSI1_BS + VME_A32_SLV_SIZE)
#else
#ifdef	ANY_BRDS_IN_CHASSIS_NOT_RMW
#  define VAL_VSI1_BD	(VAL_VSI1_BS + VME_A32_SLV_SIZE)
#else
#  define VAL_VSI1_BD     (VAL_VSI1_BS + SM_MEM_ADRS)
#endif	/* ANY_BRDS_IN_CHASSIS_NOT_RMW */
#endif	/* SM_OFF_BOARD */

#define VAL_VSI1_TO	(PCI_SLV_MEM_BUS - VAL_VSI1_BS + VME_A32_SLV_LOCAL)
#define VAL_VSI1_CTL	( VSI_CTL_EN       | VSI_CTL_PREN    | \
			  VSI_CTL_AM_DATA  | VSI_CTL_AM_PGM  | \
			  VSI_CTL_AM_SUPER | VSI_CTL_AM_USER | \
			  VSI_CTL_VAS_A32  | VSI_CTL_LAS_MEM | \
			  VSI_CTL_LD64EN )

/*  VSI4 and VSI5 are only applicable to the Universe II */

#define VAL_VSI4_BS	(VAL_VSI1_BS + \
			 SM_MEM_ADRS)
#define VAL_VSI4_BD	(VAL_VSI1_BS + \
			 SM_MEM_ADRS + \
			 SM_MEM_SIZE + \
			 SM_OBJ_MEM_SIZE)
#define VAL_VSI4_TO	(PCI_SLV_MEM_BUS - VAL_VSI1_BS + VME_A32_SLV_LOCAL)
#define VAL_VSI4_CTL	( VSI_CTL_EN       | VSI_CTL_PREN    | \
			  VSI_CTL_AM_DATA  | VSI_CTL_AM_PGM  | \
			  VSI_CTL_AM_SUPER | VSI_CTL_AM_USER | \
			  VSI_CTL_VAS_A32  | VSI_CTL_LAS_MEM | \
			  VSI_CTL_LD64EN   | VSI_CTL_PWEN    | \
			  VSI_CTL_LLRMW )

#define VAL_VSI5_BS	(VAL_VSI1_BS + \
			 SM_MEM_ADRS + \
			 SM_MEM_SIZE + \
			 SM_OBJ_MEM_SIZE)
#define VAL_VSI5_BD	(VAL_VSI1_BS + VME_A32_SLV_SIZE)
#define VAL_VSI5_TO	(PCI_SLV_MEM_BUS - VAL_VSI1_BS + VME_A32_SLV_LOCAL)
#define VAL_VSI5_CTL	( VSI_CTL_EN       | VSI_CTL_PREN    | \
			  VSI_CTL_AM_DATA  | VSI_CTL_AM_PGM  | \
			  VSI_CTL_AM_SUPER | VSI_CTL_AM_USER | \
			  VSI_CTL_VAS_A32  | VSI_CTL_LAS_MEM | \
			  VSI_CTL_LD64EN   | VSI_CTL_PWEN    )

/*
 * VME SLAVE WINDOW FOR A24 SPACE
 *
 * Universe VME A24 Slave window, does not exist.  (Could use window 2.)
 */

#define VME_A24_SLV_BUS         0x0
#define VME_A24_SLV_SIZE        0x0	/* 0, window is disabled */
#define VME_A24_SLV_LOCAL       0x0

/*
 * VME SLAVE WINDOW FOR A16 SPACE
 *
 * Universe VME A16 Slave window, does not exist.  (Could use window 3.)
 */

#define VME_A16_SLV_BUS		0x0
#define VME_A16_SLV_SIZE	0x0	/* 0, window is disabled */
#define VME_A16_SLV_LOCAL	0x0


/* VME Registers as seen from CPU */

#define CPU_VME_HW_REGS_BASE	( ISA_MSTR_IO_LOCAL + 0x1000 )
#define CPU_SIG_LM_CONTROL_REG	( ISA_MSTR_IO_LOCAL + 0x1000 )
#define CPU_SIG_LM_STATUS_REG	( ISA_MSTR_IO_LOCAL + 0x1001 )
#define CPU_VME_LM_UBA		( ISA_MSTR_IO_LOCAL + 0x1002 )
#define CPU_VME_LM_LBA		( ISA_MSTR_IO_LOCAL + 0x1003 )
#define CPU_VME_SEM_REG1	( ISA_MSTR_IO_LOCAL + 0x1004 )
#define CPU_VME_SEM_REG2	( ISA_MSTR_IO_LOCAL + 0x1005 )
#define CPU_VME_GEOG_STAT	( ISA_MSTR_IO_LOCAL + 0x1006 )
#define CPU_VME_HW_REGS_SZ	7

/* Universe Special Cycle Generator values */

#define	VME_SCG_COMPARE_MASK		0xffffffff
#define VME_SCG_COMPARE_TO_SET		0x00000000
#define VME_SCG_SWAP_TO_SET		0x80000000
#define VME_SCG_COMPARE_TO_CLEAR	0x80000000
#define VME_SCG_SWAP_TO_CLEAR		0x00000000

/* INTERRUPT DEFINES */

#define ISA_INTERRUPT_BASE	0x00
#define EXT_INTERRUPT_BASE	0x10
#define TIMER_INTERRUPT_BASE	0x20
#define IPI_INTERRUPT_BASE	0x24
#define ERR_INTERRUPT_BASE	0x28
#define	ESCC_INTERRUPT_BASE	0x00

/* interrupt Level definitions */

#ifdef INCLUDE_MPIC

/* PIB (8259) interrupt connection */

#define PIB_INT_LVL		( 0x00 + EXT_INTERRUPT_BASE )
 
/* ISA interrupt defines (NOTE: these are int. NUMBERS, not levels) */
 
/* programable timer interrup level */
#define PIT_INT_LVL		( 0x00 + ISA_INTERRUPT_BASE )

/* keyboard interrupt level ( currently not supported ) */
#define KBD_INT_LVL		( 0x01 + ISA_INTERRUPT_BASE )

/* com port 2 interrupt level */
#define COM2_INT_LVL		( 0x03 + ISA_INTERRUPT_BASE )

/* com port 1 interrupt level */
#define COM1_INT_LVL		( 0x04 + ISA_INTERRUPT_BASE )

/* floppy interrupt */
#define FD_INT_LVL		( 0x06 + ISA_INTERRUPT_BASE )

/* parallel port interrupt level */
#define PP_INT_LVL		( 0x07 + ISA_INTERRUPT_BASE )

/* z8536 timer interrupt level */
#define Z8536_INT_LVL		( 0x09 + ISA_INTERRUPT_BASE )

/* z85230 ESCC interrupt level (shared with z8536) */
#define Z85230_INT_LVL		( 0x09 + ISA_INTERRUPT_BASE )

/* mouse interrupt ( currently not supported ) */
#define MOUSE_INT_LVL		( 0x0c + ISA_INTERRUPT_BASE )
 
/* Timer interrupt level (IPI0) */
#define TIMER0_INT_LVL          ( 0x00 + TIMER_INTERRUPT_BASE )

/* PCI interrupt levels */
 
/* Hawk SMC ECC error interrupt */
#define ECC_INT_LVL		( 0x01 + EXT_INTERRUPT_BASE )

/* ethernet interrupt level */
#define LN_INT_LVL		( 0x02 + EXT_INTERRUPT_BASE )

/* SCSI interrupt level */
#define SCSI_INT_LVL		( 0x03 + EXT_INTERRUPT_BASE )

/* Graphics interrupt level ( not currently used ) */
#define GRPH_INT_LVL		( 0x04 + EXT_INTERRUPT_BASE )

/* Universe LINT#0 interrupt level ( used for the UNIVERSE chip )*/
#define UNIV_INT_LVL0		( 0x05 + EXT_INTERRUPT_BASE )

/* Universe LINT#1 interrupt level */
#define UNIV_LINT_LVL1		( 0x06 + EXT_INTERRUPT_BASE )

/* Universe LINT#2 interrupt level */
#define UNIV_INT_LVL2		( 0x07 + EXT_INTERRUPT_BASE )

/* Universe LINT#3 interrupt level */
#define UNIV_INT_LVL3		( 0x08 + EXT_INTERRUPT_BASE )

/* PMC1 INTA#, PMC2 INTD# */
#define PMC_INT_LVL1		( 0x09 + EXT_INTERRUPT_BASE )

/* PMC1 INTB#, PMC2 INTC# */
#define PMC_INT_LVL2		( 0x0a + EXT_INTERRUPT_BASE )

/* PMC1 INTC#, PMC2 INTB# */
#define PMC_INT_LVL3		( 0x0b + EXT_INTERRUPT_BASE )

/* PMC1 INTD#, PMC2 INTA# */
#define PMC_INT_LVL4		( 0x0c + EXT_INTERRUPT_BASE )

/* Location Monitor/SIG interrupt 0 */
#define LM_SIG_INT_LVL0		( 0x0d + EXT_INTERRUPT_BASE )

/* Location Monitor/SIG interrupt 1 ( used for the mailbox intr. ) */
#define LM_SIG_INT_LVL1		( 0x0e + EXT_INTERRUPT_BASE )
 
#define LM_SIG_INT_LVL		LM_SIG_INT_LVL1   /* used for mailbox intr */
#define UNIV_INT_LVL		UNIV_INT_LVL0   /* universe int level */

#else

#define PIT_INT_LVL	0x00	/* programable timer interrup level */
#define KBD_INT_LVL	0x01	/* keyboard interrupt level */
#define COM2_INT_LVL	0x03	/* com port 2 interrupt level */
#define COM1_INT_LVL	0x04	/* com port 1 interrupt level */
#define LM_SIG_INT_LVL	0x05	/* used for mailbox intr */
#define FD_INT_LVL	0x06    /* floppy disk interrupt level */
#define PP_INT_LVL	0x07	/* parallel port interrupt level */
#define Z8536_INT_LVL	0x09	/* z8536 timer interrupt level */
#define Z85230_INT_LVL	0x09	/* z85230 ESCC interrupt level */
#define LN_INT_LVL	0x0a	/* ethernet interrupt level */
#define UNIV_INT_LVL	0x0b	/* universe int level */
#define SCSI_INT_LVL	0x0e	/* scsi interrupt level */
#define PMC_INT_LVL1	0x0a	/* PCI INTA */
#define PMC_INT_LVL2	0x0b	/* PCI INTB */
#define PMC_INT_LVL3	0x0e	/* PCI INTC */
#define PMC_INT_LVL4	0x0f	/* PCI INTD */

#endif /* INCLUDE_MPIC */

/* interrupt vector definitions */

#define INT_VEC_IRQ0		0x00	/* vector for IRQ0 */

/* ISA interrupt vectors */

#define PIT_INT_VEC		INT_VEC_IRQ0 + PIT_INT_LVL
#define KBD_INT_VEC		INT_VEC_IRQ0 + KBD_INT_LVL
#define PP_INT_VEC		INT_VEC_IRQ0 + PP_INT_LVL
#define COM1_INT_VEC            INT_VEC_IRQ0 + COM1_INT_LVL
#define COM2_INT_VEC            INT_VEC_IRQ0 + COM2_INT_LVL
#define Z8536_INT_VEC		INT_VEC_IRQ0 + Z8536_INT_LVL
#define Z85230_INT_VEC		INT_VEC_IRQ0 + Z85230_INT_LVL
#define FD_INT_VEC		INT_VEC_IRQ0 + FD_INT_LVL

/* PCI/MPIC interrupt vectors */

#define LN_INT_VEC		INT_VEC_IRQ0 + LN_INT_LVL
#define SCSI_INT_VEC		INT_VEC_IRQ0 + SCSI_INT_LVL
#define UNIV_INT_VEC		INT_VEC_IRQ0 + UNIV_INT_LVL
#define LN2_INT_VEC             INT_VEC_IRQ0 + PMC_INT_LVL2
#define SCSI_INT_VEC2           INT_VEC_IRQ0 + PMC_INT_LVL3
#define PIB_INT_VEC		INT_VEC_IRQ0 + PIB_INT_LVL
#define	PCI_PRI_INTA_VEC	INT_VEC_IRQ0 + PMC_INT_LVL1
#define	PCI_PRI_INTB_VEC	INT_VEC_IRQ0 + PMC_INT_LVL2
#define	PCI_PRI_INTC_VEC	INT_VEC_IRQ0 + PMC_INT_LVL3
#define	PCI_PRI_INTD_VEC	INT_VEC_IRQ0 + PMC_INT_LVL4

/* Timer interrupt vectors */

#define TIMER0_INT_VEC          ( INT_VEC_IRQ0 + TIMER0_INT_LVL )

/* UNIVERSE chip interrupt vector defines */

#define UNIV_DMA_INT_VEC	0x56
#define UNIV_VME_SW_IACK_INT_VEC 0x57
#define UNIV_PCI_SW_INT_VEC	0x58
#define UNIV_VOWN_INT_VEC	0x59
#define UNIV_LERR_INT_VEC	0x5a
#define UNIV_VERR_INT_VEC	0x5c
#define UNIV_SYSFAIL_INT_VEC	0x5d
#define UNIV_ACFAIL_INT_VEC	0x5f

/*
 * Address range definitions for VME and PCI buses.
 *
 * Used with vxMemProbe() hook sysBusProbe().
 */

#define IS_VME_ADDRESS(adrs) (((UINT32)(adrs) >= (UINT32)VME_MSTR_LO_ADRS) && \
((UINT32)(adrs) < (UINT32)VME_MSTR_HI_ADRS))

#define IS_PCI_ADDRESS(adrs) (((UINT32)(adrs) >= (UINT32)PCI_MSTR_LO_ADRS) && \
((UINT32)(adrs) < (UINT32)PCI_MSTR_HI_ADRS))

/* VME Interrupt Bit definitions */

#define SIG1_INTR_SET		0x80
#define SIG0_INTR_SET		0x40
#define LM1_INTR_SET		0x20
#define LM0_INTR_SET		0x10
#define SIG1_INTR_CLEAR		0x08
#define SIG0_INTR_CLEAR		0x04
#define LM1_INTR_CLEAR		0x02
#define LM0_INTR_CLEAR		0x01

#define SIG1_INTR_ENABL		0x80
#define SIG0_INTR_ENABL		0x40
#define LM1_INTR_ENABL		0x20
#define LM0_INTR_ENABL		0x10
#define SIG1_INTR_STATUS	0x08
#define SIG0_INTR_STATUS	0x04
#define LM1_INTR_STATUS		0x02
#define LM0_INTR_STATUS		0x01

#define MV2400_SIOP_HW_REGS    {0,0,0,0,0,1,0,0,0,0,0}

/* PCI bus number for primary PCI bus */

#define PCI_PRIMARY_BUS         0

/* Fixed PMC Span (PCI-to-PCI Bridge) configuration parameters */

#define P2P_CLR_STATUS          0xFFFF0000
#define P2P_SEC_BUS_RESET       (0x0040 << 16)
#define P2P_CLK_ENABLE          0x00       /* enable clocks on all slots */
#define P2P_PMC_DISABLE         0
#define P2P_PMC_ENABLE          7

#ifndef _ASMLANGUAGE

/* PMC Span (DEC21150 PCI-to-PCI Bridge) Configuration Parameter Structure */

typedef struct pmc_span_parm	/* PMC_SPAN */
    {
    UINT16  parmOffset;		/* offset into configuration header */
    UINT16  parmSize;		/* parmValue size (1, 2 or 4 bytes) */
    UINT32  parmValue;		/* parameter value placed at this offset */
    } PMC_SPAN;

#endif  /* _ASMLANGUAGE */

/*
 * Support for determining if we're ROM based or not.  _sysInit
 * saves the startType parameter at location ROM_BASED_FLAG.
 */

#define PCI_AUTOCONFIG_FLAG_OFFSET ( 0x4c00 )
#define PCI_AUTOCONFIG_FLAG ( *(UCHAR *)(LOCAL_MEM_LOCAL_ADRS + \
                                     PCI_AUTOCONFIG_FLAG_OFFSET) )
#define PCI_AUTOCONFIG_DONE ( PCI_AUTOCONFIG_FLAG != 0 )

#ifdef __cplusplus
}
#endif

#endif /* INCmv240xh */
