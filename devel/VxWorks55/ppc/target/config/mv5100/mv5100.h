/* mv5100.h - Motorola MVME5100 board header */

/* Copyright 1984-2002 Wind River Systems, Inc. */
/* Copyright 1999-2001 Motorola, Inc. All Rights Reserved */

/*
modification history
--------------------
01o,30jan02,kab  Added MPC7410 (Nitro) support
01n,14dec01,kab  Add _WRS_ASM macro
01m,05nov01,kab  Fix SPR #69249: set sym895DelayCount for CPU speed.
                 Fix SPR #71519: 64bit RMW hangs, fix by reducing
		 Hawk bus timeout - added Hawk timeout macros.
01l,04sep01,efb  Changed from Location Monitor to Universe II
                 mailbox registers for shared memory system mailbox
                 interrupt.
01k,22may01,dtr  Including INCLUDE_PCI_AUTOCONF.
01j,09mar01,rhk  Additional Wind River coding standards changes.
01i,16nov00,rhk  Changed start of address space 1 of the Hawk windows
		 to include the VME spaces that were deleted from 
		 window 0 when TFFS was added.
01h,14sep00,cak  Added Hawk ROM bank A configuration for TrueFFS.
01g,16aug00,dmw  Wind River coding standards review.
01f,06jun00,dmw  N_UART_CHANNELS based on INCLUDE_I8250_SIO.
01e,26may00,dmw  RTC Watchdog support.
01d,10apr00,dmw  Updated VME mapping to mv2100 BSP conventions.
01c,03apr00,dmw  Added defines for PMCSpan interrupt lines.
01b,30mar00,dmw  Added second UART register offset.
01a,10mar00,dmw  Created from version 01j of the mcpn765.h
*/

/*
This file contains I/O addresses and related constants for the
Motorola PowerPlusII MVME5100 board.
*/

#ifndef	INCmv5100h
#define	INCmv5100h

#ifdef __cplusplus
    extern "C" {
#endif /* __cplusplus */

/* add PCI access macros */
#define PCI_MEMIO2LOCAL(x) \
     ((int)(x) + PCI_MSTR_MEMIO_LOCAL - PCI_MSTR_MEMIO_BUS)
      
/* PCI IO memory adrs to CPU (60x bus) adrs */
       
#define PCI_IO2LOCAL(x) \
     ((int)(x) + PCI_MSTR_IO_LOCAL - PCI_MSTR_IO_BUS)
	     
/* 60x bus adrs to PCI (non-prefetchable) memory address */
	      
#define LOCAL2PCI_MEMIO(x) \
     ((int)(x) + PCI_SLV_MEM_LOCAL)
		    
/* MPC7400 (Max) Support */

/* Max SPRs */

#define VRSAVE                 256	/* VMX Save register */
#define UBAMR                  935	/* Performance monitor mask */
#define UMMCR2                 928	/* Performance monitor control */
#define BAMR                   951 	/* Performance monitor mask */
#define MMCR2                  944 	/* Performance monitor control */
#define MSSCR0                 1014	/* Memory Subsystem control */
#define MSSCR1                 1015	/* Memory Subsystem debug */
#define PIR                    1023	/* Processor ID register */

/* MPC7410 'Nitro' support */
#define MSSCR_NITRO1 	       0x00040000  /* Memory SubSystem Control Reg 1 */
				        /* bit 13 for Nitro errata 1 */

/* Max HID0 bit definitions */

#define _PPC_HID0_NOPDST       0x2	/* Nop dst, dstt, dstst, &dststt*/
#define _PPC_HID0_NOPTI        0x1	/* Nop dcbt and dbtst */

/* AltiVec Exceptions */

#define _EXC_VMX_UNAVAIL       0x0f20	/* VMX Unavailable Exception */
#define _EXC_VMX_ASSIST        0x1600	/* VMX Assist Exception */
#define SIZEOF_EXCEPTION       0x0030	/* VxWorks Exc is 48 bytes Max*/


#define INCLUDE_PCI		/* always include pci */
#define INCLUDE_PCI_AUTOCONF    /* always include pci auto config*/

/* Boot Line parameters are stored in the 2nd 256 byte block */

#undef	NV_BOOT_OFFSET
#define NV_BOOT_OFFSET          256		/* skip 1st 256 bytes */
#define NV_RAM_SIZE             BBRAM_SIZE		
#define NV_RAM_ADRS             ((char *) BBRAM_ADRS)
#define NV_RAM_INTRVL           1

/* PCI I/O function defines */

#define INT_NUM_IRQ0            INT_VEC_IRQ0

#ifndef _ASMLANGUAGE

#   ifndef PCI_IN_BYTE
#       define PCI_IN_BYTE(x)	sysPciInByte (x)
        IMPORT  UINT8		sysPciInByte  (UINT32 address);
#   endif /* PCI_IN_BYTE */
#   ifndef PCI_IN_WORD
#       define PCI_IN_WORD(x)	sysPciInWord (x)
        IMPORT  UINT16		sysPciInWord  (UINT32 address);
#   endif /* PCI_IN_WORD */
#   ifndef PCI_IN_LONG
#       define PCI_IN_LONG(x)	sysPciInLong (x)
        IMPORT  UINT32		sysPciInLong  (UINT32 address);
#   endif /* PCI_IN_LONG */
#   ifndef PCI_OUT_BYTE
#       define PCI_OUT_BYTE(x,y)	sysPciOutByte (x,y)
        IMPORT  void		sysPciOutByte (UINT32 address, UINT8  data);
#   endif /* PCI_OUT_BYTE */
#   ifndef PCI_OUT_WORD
#       define PCI_OUT_WORD(x,y)   sysPciOutWord (x,y)
        IMPORT  void	       sysPciOutWord (UINT32 address, UINT16 data);
#   endif /* PCI_OUT_WORD */
#   ifndef PCI_OUT_LONG
#       define PCI_OUT_LONG(x,y)	sysPciOutLong (x,y)
        IMPORT  void		sysPciOutLong (UINT32 address, UINT32 data);
#   endif /* PCI_OUT_LONG */
#   ifndef PCI_INSERT_LONG
#       define PCI_INSERT_LONG(a,m,d) sysPciInsertLong((a),(m),(d))
        IMPORT  void		sysPciInsertLong(UINT32 adrs, UINT32 mask, \
                                                  UINT32 data);
#   endif /* PCI_INSERT_LONG */
#   ifndef PCI_INSERT_WORD
#       define PCI_INSERT_WORD(a,m,d) sysPciInsertWord((a),(m),(d))
        IMPORT  void		sysPciInsertWord(UINT32 adrs, UINT16 mask, \
                                                  UINT16 data);
#   endif /* PCI_INSERT_WORD */
#   ifndef PCI_INSERT_BYTE
#       define PCI_INSERT_BYTE(a,m,d) sysPciInsertByte((a),(m),(d))
        IMPORT  void		sysPciInsertByte(UINT32 adrs, UINT8 mask, \
                                                  UINT8 data);
#   endif /* PCI_INSERT_BYTE */

#endif  /* _ASMLANGUAGE */

/* Cache Line Size - 8 32-bit values = 32 bytes */

#define PCI_CLINE_SZ		(_CACHE_ALIGN_SIZE/4)

/* Latency Timer value - 255 PCI clocks */

#define PCI_LAT_TIMER		0xff

/* clock rates */

/* Calculate Memory Bus Rate in Hertz */

#define MEMORY_BUS_SPEED	( sysGetBusSpd() )

/* System clock (decrementer counter) frequency determination */

#define DEC_CLOCK_FREQ		( sysGetBusSpd() )

/*
 * The PowerPC Decrementer is used as the system clock.
 * It is always included in this BSP.  The following defines
 * are used by the system clock library.
 */

#define SYS_CLK_RATE_MIN	10		/* minimum system clock rate */
#define SYS_CLK_RATE_MAX	5000		/* maximum system clock rate */

/*
 * This macro returns the positive difference between two signed ints.
 * Useful for determining delta between two successive decrementer reads.
 */

#define DELTA(a,b)		( abs((int)a - (int)b) )

/*
 * Auxiliary Clock support is an optional feature that is not supported
 * by all BSPs.  The following defines are used by the aux clock library.
 */

#define AUX_CLK_RATE_MIN	40		/* min auxiliary clock */
#define AUX_CLK_RATE_MAX	5000		/* max auxiliary clock rate */

/*
 * Shared Memory Interrupt Type.
 * Interrupt this target with a 1-byte write mailbox.
 * VME_A32 space, address based on procNum, value is SIG1_INTR_SET.
 */

#define SM_INT_ARG1 	VME_AM_EXT_SUP_DATA
#define SM_INT_ARG2 	(VME_RAI_MSTR_BUS + VME_MBOX0_OFFSET + \
                         (sysProcNumGet() * VME_A32_REG_SIZE))
#define SM_INT_ARG3	SIG1_INTR_SET

/*
 * Semaphore Test-and-Set Register as seen from a slave
 * Only used with a special version of sysBusTas().
 */

#define OFFBRD_VME_SEM_REG1	(VME_RAI_MSTR_LOCAL + \
                            (CPU_VME_SEM_REG1 - CPU_VME_HW_REGS_BASE)) 

/* Common I/O synchronizing instructions */

#ifndef EIEIO_SYNC
#   define EIEIO_SYNC  _WRS_ASM ("	eieio; sync")
#endif	/* EIEIO_SYNC */
#ifndef EIEIO
#   define EIEIO    _WRS_ASM ("	eieio")
#endif	/* EIEIO */

/* Translation macro */

#define TRANSLATE(x,y,z)\
        ((UINT)(x) - (UINT)(y) + (UINT)(z))

/* Legacy ISA space size. Reserved for kybd, com1, com2,... */

#define ISA_LEGACY_SIZE		0x00004000

#ifndef EXTENDED_VME

    /* Pseudo PREP memory map (maximizes available PCI addressing space) */

    /* ISA I/O space within PCI I/O space (includes ISA legacy space) */;

#define ISA_MSTR_IO_LOCAL 0x80000000

/*Sergey ???*/
#define ISA_MSTR_IO_BUS	  0x00000000	/* must be zero */
/*#define ISA_MSTR_IO_BUS	  ISA_MSTR_IO_LOCAL*/

#define ISA_MSTR_IO_SIZE  0x00010000	/* 64 kbytes */

/*
 * PCI I/O space (above ISA I/O space)
 *
 * NOTE: (PCI_MSTR_IO_LOCAL+PCI_MSTR_IO_SIZE) must be less than
 *       PCI_MSTR_MEMIO_LOCAL to prevent overlap. PCI_MSTR_IO_SIZE
 *       is defined in config.h
 */

#   define PCI_MSTR_IO_LOCAL   (ISA_MSTR_IO_LOCAL+ISA_MSTR_IO_SIZE)
#   define PCI_MSTR_IO_BUS     (TRANSLATE(PCI_MSTR_IO_LOCAL,\
                                       ISA_MSTR_IO_LOCAL,\
                                       ISA_MSTR_IO_BUS))

/*
 * PCI Memory I/O (non-prefetchable)
 *
 * NOTE: PCI_MSTR_MEMIO_SIZE is defined in config.h
 */

#   define PCI_MSTR_MEMIO_LOCAL	0xc0000000      /* base of ISA mem space */

/*Sergey ???*/
/*#define PCI_MSTR_MEMIO_BUS	0x00000000*/	/* must be zero */
#define PCI_MSTR_MEMIO_BUS PCI_MSTR_MEMIO_LOCAL

/*
 * PCI (pre-fetchable) Memory space (above PCI I/O Memory space)
 *
 * NOTE: (PCI_MSTR_MEM_LOCAL+PCI_MSTR_MEM_SIZE) must be less than
 *        VME_A32_MSTR_LOCAL to prevent overlap. PCI_MSTR_MEM_SIZE is
 *        defined in config.h
 */

#   define PCI_MSTR_MEM_LOCAL	(PCI_MSTR_MEMIO_LOCAL+PCI_MSTR_MEMIO_SIZE)
#   define PCI_MSTR_MEM_BUS	(TRANSLATE(PCI_MSTR_MEM_LOCAL,\
                                           PCI_MSTR_MEMIO_LOCAL,\
                                           PCI_MSTR_MEMIO_BUS))

#else /* EXTENDED_VME */

/* Extended VME memory map (maximizes VME A32 space) */

/*
 * Semaphore Test-and-Set Register as seen from a slave.
 * Only used with a special version of sysBusTas() (not included)
 * and when mv2300, mv2600 or mv3600 boards are the ONLY boards
 * used in the configuration.
 */

/* ISA I/O space within PCI I/O space (includes ISA legacy space) */

#   define ISA_MSTR_IO_LOCAL	0xfe000000  /* base of ISA I/O space */
#   define ISA_MSTR_IO_BUS	0x00000000  /* must be zero */
#   define ISA_MSTR_IO_SIZE	0x00010000  /* 64 kbytes (includes legacy) */

/*
 * PCI I/O space (above ISA I/O space)
 *
 * NOTE: (PCI_MSTR_IO_LOCAL+PCI_MSTR_IO_SIZE) must be less than
 *       HAWK_SMC_BASE_ADRS to prevent overlap. PCI_MSTR_IO_SIZE
 *       is defined in config.h
 */

#   define PCI_MSTR_IO_LOCAL	(ISA_MSTR_IO_LOCAL+ISA_MSTR_IO_SIZE)
#   define PCI_MSTR_IO_BUS         (TRANSLATE(PCI_MSTR_IO_LOCAL,\
                                           ISA_MSTR_IO_LOCAL,\
                                           ISA_MSTR_IO_BUS))

/*
 * PCI Memory I/O (non-prefetchable)
 *
 * NOTE: PCI_MSTR_MEMIO_SIZE is defined in config.h
 */

/*Sergey */
#define PCI_MSTR_MEMIO_LOCAL	0xfd000000      /* base of ISA mem space */
/*#   define PCI_MSTR_MEMIO_LOCAL	0xfd000000*/      /* base of ISA mem space */

#   define PCI_MSTR_MEMIO_BUS	PCI_MSTR_MEMIO_LOCAL	/* 1-1 mapping */

/*
 * PCI (pre-fetchable) Memory space (above PCI I/O Memory space)
 *
 * NOTE: (PCI_MSTR_MEM_LOCAL+PCI_MSTR_MEM_SIZE) must be less than
 *        ISA_MSTR_IO_LOCAL to prevent overlap. PCI_MSTR_MEM_SIZE is
 *        defined in config.h
 */

#   define PCI_MSTR_MEM_LOCAL	(PCI_MSTR_MEMIO_LOCAL+PCI_MSTR_MEMIO_SIZE)
#   define PCI_MSTR_MEM_BUS	(TRANSLATE(PCI_MSTR_MEM_LOCAL,\
                                           PCI_MSTR_MEMIO_LOCAL,\
                                           PCI_MSTR_MEMIO_BUS))
#endif /* EXTENDED_VME */

/*
 * PCI MASTER MEMORY WINDOW LIMITS
 *
 * These values are strictly defined by the base memory addresses and window
 * sizes of the spaces defined above.  These values must be correct for the
 * sysBusProbe() memory range checks for the PCI bus to work properly.
 */

#ifndef EXTENDED_VME
#   define PCI_MSTR_LO_ADRS      (ISA_MSTR_IO_LOCAL)
#   define PCI_MSTR_HI_ADRS      (PCI_MSTR_MEM_LOCAL + PCI_MSTR_MEM_SIZE)
#else /* EXTENDED_VME */
#   define PCI_MSTR_LO_ADRS      (PCI_MSTR_MEMIO_LOCAL)
#   define PCI_MSTR_HI_ADRS      (PCI_MSTR_IO_LOCAL + PCI_MSTR_IO_SIZE)
#endif  /* EXTENDED_VME */

#ifndef INCLUDE_MPIC

#   define MPIC_ADDR(reg)	(MPIC_BASE_ADRS + reg)
#   define MPIC_GLOBAL_CONFIG_REG	0x01020
#   define RESET_CNTRLR		0x80000000

#endif  /* INCLUDE_MPIC */

/* defines for memory */

#define SIZE_32MB     0x02000000
#define SIZE_64MB     0x04000000
#define SIZE_128MB    0x08000000
#define SIZE_256MB    0x10000000
#define SIZE_512MB    0x20000000

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

#define	FLASH_BASE_ADRS		0xF4000000
#define	FLASH_MEM_SIZE		0x01000000

/* MPIC configuration defines */

#define MPIC_BASE_ADRS		0xfc000000
#define MPIC_REG_SIZE		0x00040000
#ifndef EXTENDED_VME 
#   define MPIC_PCI_BASE_ADRS	(TRANSLATE(MPIC_BASE_ADRS,\
                                           PCI_MSTR_MEMIO_LOCAL,\
                                           PCI_MSTR_MEMIO_BUS))
#else /* EXTENDED_VME */
#   define MPIC_PCI_BASE_ADRS	MPIC_BASE_ADRS
#endif /* Extended VME config */

/* memory map as seen on the PCI bus */

#define PCI_SLV_MEM_LOCAL	LOCAL_MEM_LOCAL_ADRS

#ifndef EXTENDED_VME
#   define PCI_SLV_MEM_BUS	0x80000000      /* memory seen from PCI bus */
#else /* EXTENDED_VME */
/* Sergey: we have this */
#   define PCI_SLV_MEM_BUS	0x00000000      /* memory seen from PCI bus */
#endif /* EXTENDED_VME */

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
#define PCI_ID_LN_DEC21040      0x00021011      /* Id DEC chip 21040 */
#define PCI_ID_LN_DEC21140      0x00091011      /* Id DEC chip 21140 */
#define PCI_ID_LN_DEC21143      0x00191011      /* Id DEC chip 21143 */
#define PCI_ID_BR_DEC21150      0x00221011      /* Id DEC 21150 PCI bridge */
#define PCI_ID_BR_DEC21554      0x00461011      /* Id DEC 21554 PCI bridge */
#define PCI_ID_I82559           0x12298086      /* Id for Intel 82559 */
#define PCI_ID_I82559ER         0x12098086      /* Id for Intel 82559 ER */
#define PCI_ID_NCR810           0x00011000      /* Id for SYM53C810A Chip */
#define PCI_ID_NCR825           0x00031000      /* Id for SYM53C825 Chip */
#define PCI_ID_NCR860           0x00061000      /* Id for SYM53C860 Chip */
#define PCI_ID_NCR875           0x000f1000      /* Id for SYM53C875 Chip */
#define PCI_ID_NCR895           0x000c1000      /* Id for SYM53C895 Chip */
#define PCI_ID_NCR895A          0x00121000      /* Id for SYM53C895A Chip */
#define PCI_ID_SCSI             PCI_ID_NCR895A  /* Id for Primary SCSI */
#define PCI_ID_SEC_SCSI         PCI_ID_NCR895   /* Id for Secondary SCSI */

/* PCI Space Definitions  -- For configuring the Hawk */

#define CPU2PCI_ATTR_REN     (1<<7)  /* Read enable */
#define CPU2PCI_ATTR_WEN     (1<<6)  /* Write enable */
#define CPU2PCI_ATTR_WPEN    (1<<4)  /* Write post enable */
#define CPU2PCI_ATTR_MEM     (1<<1)  /* PCI memory cycle */
#define CPU2PCI_ATTR_IOM     (1<<0)  /* PCI I/O mode */

/* CPU to PCI definitions */

#define CPU2PCI_MSATT_MEM	( CPU2PCI_ATTR_REN | CPU2PCI_ATTR_WEN | \
				  CPU2PCI_ATTR_MEM )

#define CPU2PCI_MSATT_IO	( CPU2PCI_ATTR_REN | CPU2PCI_ATTR_WEN )
#define CPU2PCI_MSATT_DISABLED  ( CPU2PCI_MSATT_MEM & ~( CPU2PCI_ATTR_REN | \
						         CPU2PCI_ATTR_WEN ) )

#ifndef EXTENDED_VME

/* STANDARD ( PReP ) mapping of PCI space */

/* setup address space 0 for MPIC register space */
#   define CPU2PCI_ADDR0_START_VAL      (MPIC_BASE_ADRS >> 16)
#   define CPU2PCI_ADDR0_END_VAL        (((MPIC_BASE_ADRS + 0x00ffffff) \
                                          >> 16) & 0x0000ffff) 	
#   define CPU2PCI_OFFSET0_VAL          (((MPIC_PCI_BASE_ADRS - \
                                           MPIC_BASE_ADRS) >> 16) & 0xffff) 	

/* setup address space 1 for PCI I/O Space */
#   define CPU2PCI_ADDR1_START_VAL	0xfe00
#   define CPU2PCI_ADDR1_END_VAL	0xfe7f
#   define CPU2PCI_OFFSET1_VAL	((0x0-CPU2PCI_ADDR1_START_VAL) & 0xffff)
#   define CPU2PCI_MSATT1_VAL	CPU2PCI_MSATT_IO

/* setup address space 2 for PCI MEM space -- maps VME address space */
#   define CPU2PCI_ADDR2_START_VAL	(PCI_MSTR_MEMIO_LOCAL >>16)
#   define CPU2PCI_ADDR2_END_VAL        ((FLASH_BASE_ADRS - 1) >> 16)	
#   define CPU2PCI_OFFSET2_VAL	((0x0-CPU2PCI_ADDR2_START_VAL) & 0xffff)

/* setup address space 3 for PCI I/O space */
#   define CPU2PCI_ADDR3_START_VAL	(ISA_MSTR_IO_LOCAL >>16)
#   define CPU2PCI_ADDR3_END_VAL	0xbf7f
#   define CPU2PCI_OFFSET3_VAL	((0x0-CPU2PCI_ADDR3_START_VAL) & 0xffff)

#else /* EXTENDED_VME */

/* EXTENDED VME PCI mapping */

/* setup address space 0 for PCI MEM space -- maps VME address space */
#   define CPU2PCI_ADDR0_START_VAL	(VME_A32_MSTR_LOCAL >> 16)
#   define CPU2PCI_ADDR0_END_VAL	0xf3ff
#   define CPU2PCI_OFFSET0_VAL	0x0

/* setup address space 1 for PCI MEM, for extra VME space, MPIC regs */
#   define CPU2PCI_ADDR1_START_VAL	(VME_A24_MSTR_LOCAL >> 16)
#   define CPU2PCI_ADDR1_END_VAL	(((MPIC_BASE_ADRS + 0x00ffffff) \
                                         >> 16) & 0x0000ffff)
#   define CPU2PCI_OFFSET1_VAL	0x0
#   define CPU2PCI_MSATT1_VAL	CPU2PCI_MSATT_MEM

/* setup address space 2 for PCI MEM -- maps reg. space */
#   define CPU2PCI_ADDR2_START_VAL	(PCI_MSTR_MEMIO_LOCAL >> 16)
#   define CPU2PCI_ADDR2_END_VAL	\
	    (((PCI_MSTR_MEM_LOCAL+PCI_MSTR_MEM_SIZE-1) >> 16) & 0x0000ffff)
#   define CPU2PCI_OFFSET2_VAL	0x0

/* setup address space 3 for PCI I/O */
#   define CPU2PCI_ADDR3_START_VAL	(ISA_MSTR_IO_LOCAL >> 16)
#   define CPU2PCI_ADDR3_END_VAL	\
	    (((PCI_MSTR_IO_LOCAL + PCI_MSTR_IO_SIZE-1) >> 16) & 0x0000ffff)
#   define CPU2PCI_OFFSET3_VAL	((0x0-CPU2PCI_ADDR3_START_VAL) & 0xffff)

#endif /* EXTENDED_VME */

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
#   define DRAM_SIZE		((ULONG)sysPhysMemTop() - LOCAL_MEM_LOCAL_ADRS)
#else
#   define DRAM_SIZE		(LOCAL_MEM_SIZE - LOCAL_MEM_LOCAL_ADRS)
#endif /* LOCAL_MEM_AUTOSIZE */

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

/* pc97303 ISA super IO device (ISASIO) keybrd, serial, Parallel port */

#define pc97303_KBD_CTRL	(ISA_MSTR_IO_LOCAL + 0x0064)	/* keyboard */
#define pc97303_INDX_REG	(ISA_MSTR_IO_LOCAL + 0x0398)	/* index reg */
#define pc97303_DATA_REG	(ISA_MSTR_IO_LOCAL + 0x0399)	/* data reg */
#define pc97303_PP		(ISA_MSTR_IO_LOCAL + 0x03bc)	/* parallel */
#define pc97303_COM1		(ISA_MSTR_IO_LOCAL + 0x03f8)	/* serial 1 */
#define pc97303_COM2		(ISA_MSTR_IO_LOCAL + 0x02f8)	/* serial 2 */
#define pc97303_FDC		(ISA_MSTR_IO_LOCAL + 0x03f0)	/* floppy */

/* z85230 synchronous & Asynchronous serial communications chip */

#define z85230_PORTB_CTRL	(ISA_MSTR_IO_LOCAL + 0x0840)	/* serial 6 */
#define z85230_PORTB_DATA	(ISA_MSTR_IO_LOCAL + 0x0841)	
#define z85230_PORTA_CTRL	(ISA_MSTR_IO_LOCAL + 0x0842)	/* serial 5 */
#define z85230_PORTA_DATA	(ISA_MSTR_IO_LOCAL + 0x0843)

/* z8536 aux timer and I/O chip */

#define z8536_PORTC_DATA        (ISA_MSTR_IO_LOCAL + 0x0844)
#define z8536_PORTB_DATA        (ISA_MSTR_IO_LOCAL + 0x0845)
#define z8536_PORTA_DATA        (ISA_MSTR_IO_LOCAL + 0x0846)
#define z8536_PORT_CTRL         (ISA_MSTR_IO_LOCAL + 0x0847)
#define ZCIO_CNTRL_ADRS         (UINT8 *)(ISA_MSTR_IO_LOCAL + 0x847)
#define ZCIO_IACK_ADRS          (UINT8 *)(ISA_MSTR_IO_LOCAL + 0x84F)

/* m48TXX non volatile ram, RTC and watchdog timer */

#ifdef nonXBus
#   define m48TXX_LSB_REG		(ISA_MSTR_IO_LOCAL + 0x0074)
#   define	m48TXX_MSB_REG		(ISA_MSTR_IO_LOCAL + 0x0075)
#   define m48TXX_DAT_REG		(ISA_MSTR_IO_LOCAL + 0x0077)
#else
#   define m48TXX_LSB_REG		(HAWK_SMC_BASE_ADRS + 0x80C8)
#   define	m48TXX_MSB_REG		(HAWK_SMC_BASE_ADRS + 0x80D0)
#   define m48TXX_DAT_REG		(HAWK_SMC_BASE_ADRS + 0x80D8)
#endif /* nonXBus */

/* CPU type */

#define CPU_TYPE		((vxPvrGet() >> 16) & 0xffff)
#define CPU_REV                 (vxPvrGet() & 0xffff)
#define CPU_TYPE_601		0x01	/* PPC 601 CPU */
#define CPU_TYPE_602		0x02	/* PPC 602 CPU */
#define CPU_TYPE_603		0x03	/* PPC 603 CPU */
#define CPU_TYPE_603E		0x06	/* PPC 603e CPU */
#define CPU_TYPE_603P		0x07	/* PPC 603p CPU */
#define CPU_TYPE_750		0x08	/* PPC 750 CPU */
#define CPU_TYPE_604		0x04	/* PPC 604 CPU */
#define CPU_TYPE_604E		0x09	/* PPC 604e CPU */
#define CPU_TYPE_604R		0x0A	/* PPC 604r CPU */
#define CPU_TYPE_7400           0x0C    /* PPC 7400 (codename MAX) CPU */
#define CPU_TYPE_7410           0x800C  /* PPC 7410 (codename Nitro) CPU */
#define CPU_REV_7410            0x1100  /* PPC 7410 (codename Nitro) Rev */

/* Vital Product Data (VPD) Support */

#define SPD_EEPROM_ADRS	    0xA8    /* i2c address of first SPD EEPROM */
#define SPD_MEZZ1_ADRS      0xAA    /* i2c address of first mezzanine board */
#define SPD_MEZZ2_ADRS      0xAC    /* i2c address of 2nd mezzanine board */
#define VPD_BRD_EEPROM_ADRS 0xA0    /* i2c address of board's SROM */
#define USR_BRD_EEPROM_ADRS 0xA2    /* i2c address of user config EEPROM */
#define VPD_IPMC761_ADRS    0xA4    /* i2c address of the IPMC's VPD */
#define VPD_BRD_OFFSET      0	    /* offset into brd's eeprom for vpd data */
#define VPD_PKT_LIMIT       25      /* Max number of packets expected */

#define DEFAULT_PCI_CLOCK	33333333
#define DEFAULT_BUS_CLOCK	100000000
#define DEFAULT_INTERNAL_CLOCK	350000000
#define DEFAULT_PRODUCT_ID	"Unknown"

/* Generic VPD Product Configuration Options (PCO) */

#define PCO_PCI0_CONN1		0	/* PCI Bus 0, connector 1 populated */
#define PCO_PCI0_CONN2		1	/* PCI Bus 0, connector 2 populated */
#define PCO_PCI0_CONN3		2	/* PCI Bus 0, connector 3 populated */
#define PCO_PCI0_CONN4		3	/* PCI Bus 0, connector 4 populated */

#define PCO_PCI1_CONN1		4	/* PCI Bus 1, connector 1 populated */
#define PCO_PCI1_CONN2		5	/* PCI Bus 1, connector 2 populated */
#define PCO_PCI1_CONN3		6	/* PCI Bus 1, connector 3 populated */
#define PCO_PCI1_CONN4		7	/* PCI Bus 1, connector 4 populated */

#define PCO_ISA_CONN1		8	/* ISA Bus connector 1 populated */
#define PCO_ISA_CONN2		9	/* ISA Bus connector 2 populated */
#define PCO_ISA_CONN3		10	/* ISA Bus connector 3 populated */
#define PCO_ISA_CONN4		11	/* ISA Bus connector 4 populated */

#define PCO_EIDE1_CONN1		12	/* IDE controller 1, conn 1 populated */
#define PCO_EIDE1_CONN2		13	/* IDE controller 1, conn 2 populated */
#define PCO_EIDE2_CONN1		14	/* IDE controller 2, conn 1 populated */
#define PCO_EIDE2_CONN2		15	/* IDE controller 2, conn 2 populated */

#define PCO_ENET1_CONN		16	/* Ethernet 1 connector populated */
#define PCO_ENET2_CONN		17	/* Ethernet 2 connector populated */
#define PCO_ENET3_CONN		18	/* Ethernet 3 connector populated */
#define PCO_ENET4_CONN		19	/* Ethernet 4 connector populated */

#define PCO_SCSI1_CONN		20	/* SCSI controller 1 conn populated */
#define PCO_SCSI2_CONN		21	/* SCSI controller 2 conn populated */
#define PCO_SCSI3_CONN		22	/* SCSI controller 3 conn populated */
#define PCO_SCSI4_CONN		23	/* SCSI controller 4 conn populated */

#define PCO_SERIAL1_CONN	24	/* Serial port 1 connector populated */
#define PCO_SERIAL2_CONN	25	/* Serial port 2 connector populated */
#define PCO_SERIAL3_CONN	26	/* Serial port 3 connector populated */
#define PCO_SERIAL4_CONN	27	/* Serial port 4 connector populated */

#define PCO_FLOPPY_CONN1	28	/* Floppy 1 connector populated */
#define PCO_FLOPPY_CONN2	29	/* Floppy 2 connector populated */
#define PCO_PARALLEL1_CONN	30	/* Parallel port 1 conn populated */
#define PCO_PARALLEL2_CONN	31	/* Parallel port 2 conn populated */

#define PCO_PMC1_IO_CONN	32	/* PMC Site 1 I/O conn populated */
#define PCO_PMC2_IO_CONN	33	/* PMC Site 2 I/O conn populated */
#define PCO_USB0_CONN		34	/* USB 0 connector populated */
#define PCO_USB1_CONN		35	/* USB 1 connector populated */

#define PCO_KEYBOARD_CONN	36	/* Keyboard connector populated */
#define PCO_MOUSE_CONN		37	/* Mouse connector populated */
#define PCO_VGA1_CONN		38	/* VGA connector populated */
#define PCO_SPEAKER_CONN	39	/* Speaker connector populated */

#define PCO_VME_CONN		40	/* VME connector populated */
#define PCO_CPCI_CONN		41	/* cPCI connector populated */
#define PCO_ABORT_SWITCH	42	/* Abort switch present */
#define PCO_BDFAIL_LIGHT	43	/* Board Fail LED present */

#define PCO_SWREAD_HEADER	44	/* Sofware-readable header present */
#define PCO_MEMMEZ_CONN		45	/* Memory mezanine connector present */
#define PCO_PCI0EXP_CONN	46	/* PCI Expansion connector present */

/* BSP configuration error policy */

#define CONTINUE_EXECUTION	0       /* Tolerate VPD/Configuration errors */
#define EXIT_TO_SYSTEM_MONITOR	1       /* Transfer to System Monitor */

#ifdef VPD_ERRORS_NONFATAL
#   define  DEFAULT_BSP_ERROR_BEHAVIOR  CONTINUE_EXECUTION
#else
#   define  DEFAULT_BSP_ERROR_BEHAVIOR  EXIT_TO_SYSTEM_MONITOR
#endif /* VPD_ERRORS_NONFATAL */

/* board register addresses */

#define BRD_UART_BASE_ADRS	   (HAWK_SMC_BASE_ADRS + 0x8000)
#define BRD_UART1_BASE_ADRS	   (HAWK_SMC_BASE_ADRS + 0x8000)
#define BRD_STATUS_REG_ADRS	   (HAWK_SMC_BASE_ADRS + 0x8080)
#define BRD_MOD_FAIL_REG_ADRS	   (HAWK_SMC_BASE_ADRS + 0x8090)
#define BRD_MOD_RESET_REG_ADRS	   (HAWK_SMC_BASE_ADRS + 0x80a0)
#define BRD_TBEN_REG_ADR           (HAWK_SMC_BASE_ADRS + 0x80c0)
#define BRD_SW_HDR_SW_BASE_ADRS    (HAWK_SMC_BASE_ADRS + 0x80e0)
#define BRD_GEOG_REG_BASE_ADRS	   (HAWK_SMC_BASE_ADRS + 0x80e8)
#define BRD_EXT_FEAT_REG_BASE_ADRS (HAWK_SMC_BASE_ADRS + 0x80f0)
#define BRD_LAST_RESET_BASE_ADRS   (HAWK_SMC_BASE_ADRS + 0x80f8)
#define BRD_UART2_BASE_ADRS	   (HAWK_SMC_BASE_ADRS + 0x8200)
#define BRD_UART3_BASE_ADRS	   (HAWK_SMC_BASE_ADRS + 0x8400)
#define BRD_UART4_BASE_ADRS	   (HAWK_SMC_BASE_ADRS + 0x8600)

/* bits in the board status register */

#define BRD_STATUS_REG_SYSCON_BIT	0x00
#define BRD_STATUS_REG_BAUD_BIT		0x01
#define BRD_STATUS_REG_SYSCON_MASK	(1<<BRD_STATUS_REG_SYSCON_BIT)
#define BRD_STATUS_REG_BAUD_MASK	(1<<BRD_STATUS_REG_BAUD_BIT)

/* bits in the board module fail register */

#define BRD_MOD_FAIL_BIT		0x00
#define BRD_MOD_ABORT_BIT		0x01
#define BRD_MOD_FAIL_MASK		(1<<BRD_MOD_FAIL_BIT)
#define BRD_MOD_ABORT_MASK		(1<<BRD_MOD_ABORT_BIT)

/* bits in the module reset register */

#define BRD_MOD_RESET_BIT		0x00
#define BRD_MOD_RESET_MASK		(1<<BRD_RESET_BIT)

/* bits in the TBEN register */

#define BRD_TBEN_BIT			0x01
#define BRD_TBEN_MASK			(1<<BRD_TBEN_BIT)

/* bits in the Extended Features register */

#define BRD_MMEZ2_P_L_BIT		0x00
#define BRD_MMEZ1_P_L_BIT               0x01
#define BRD_PMC2_ERDY_BIT               0x02
#define BRD_PMC2P_L_BIT                 0x03
#define BRD_PMC1P_L_BIT                 0x04
#define BRD_TMPRSNT_L_BIT               0x05
#define BRD_USBV0_OK_BIT                0x06
#define BRD_USBV1_OK_BIT                0x07
#define BRD_MMEZ2_P_L_MASK              (1<<BRD_MMEZ2_P_L_BIT)
#define BRD_MMEZ1_P_L_MASK              (1<<BRD_MMEZ1_P_L_BIT)
#define BRD_PMC2_ERDY_MASK              (1<<BRD_PMC2_ERDY_BIT)
#define BRD_PMC2P_L_MASK                (1<<BRD_PMC2P_L_BIT)
#define BRD_PMC1P_L_MASK                (1<<BRD_PMC1P_L_BIT)
#define BRD_TMPRSNT_L_MASK              (1<<BRD_TMPRSNT_L_BIT)
#define BRD_USBV0_OK_MASK               (1<<BRD_USBV0_OK_BIT)
#define BRD_USBV1_OK_MASK               (1<<BRD_USBV1_OK_BIT)

/*
 * Bootrom error bits.
 * These bits are set during ROM startup before error annunciation is available
 * to save error conditions for later reporting.
 */

/* bits in Group A (Hawk GPREG0_U [0x0070]) */

#define BOOTROM_NO_VPD_BUS_SPEED       0x80000000     /* Couldn't read VPD */
#define BOOTROM_DEFAULT_SMC_TIMING     0x40000000     /* Couldn't read SPD */

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

/* Hawk MPC register offsets */

#define HAWK_MPC_VENID		0x00
#define HAWK_MPC_DEVID		0x02
#define HAWK_MPC_REVID		0x05
#define HAWK_MPC_GCSR		0x08
#define HAWK_MPC_FEAT		0x0a
#define HAWK_MPC_MARB		0x0c
#define HAWK_MPC_HCSR		0x10
#define HAWK_MPC_PADJ		0x13
#define HAWK_MPC_MEREN		0x22
#define HAWK_MPC_MERST		0x27
#define HAWK_MPC_MERAD		0x28
#define HAWK_MPC_MERAT		0x2e
#define HAWK_MPC_PIACK		0x30
#define HAWK_MPC_MSADD0		0x40
#define HAWK_MPC_MSOFF0		0x44
#define HAWK_MPC_MSATT0		0x47
#define HAWK_MPC_MSADD1		0x48
#define HAWK_MPC_MSOFF1		0x4c
#define HAWK_MPC_MSATT1		0x4f
#define HAWK_MPC_MSADD2		0x50
#define HAWK_MPC_MSOFF2		0x54
#define HAWK_MPC_MSATT2		0x57
#define HAWK_MPC_MSADD3		0x58
#define HAWK_MPC_MSOFF3		0x5c
#define HAWK_MPC_MSATT3		0x5f
#define HAWK_MPC_WDT1CNTL	0x60
#define HAWK_MPC_WDT2CNTL	0x68
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
 * Hawk Bus Timeouts: 256/64/8 usec or disabled (never times out)
 */
#define HAWK_MPC_GCSR_XBT_256    0x0000
#define HAWK_MPC_GCSR_XBT_64     0x0100
#define HAWK_MPC_GCSR_XBT_8      0x0200
#define HAWK_MPC_GCSR_XBT_DISABL 0x0300
	
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

/* Hawk MPC Hardware Control/Status register bit masks */

#define HAWK_MPC_HCSR_XPR_MASK	0x07	/* isolates the 60x/pci freq ratio */

#define HAWK_MPC_HCSR_SPRQ_BIT	11	/* PCI speculative request control */

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

/* Hawk SMC Register Offsets */

#define HAWK_SMC_ROMA_BS_REG 0x50   /* ROM A Base/Size Register register */
#define HAWK_SMC_DPE_LOG_REG 0x68   /* CPU Data Bus Parity error log register */
#define HAWK_SMC_DPE_ENA_REG 0x6a   /* CPU Data Bus Parity error enable reg */
#define HAWK_SMC_APE_LOG_REG 0xe0   /* CPU Adrs Bus Parity error log register */
#define HAWK_SMC_APE_ENA_REG 0xe2   /* CPU Adrs Bus Parity error enable reg */

#define HAWK_SMC_DPELOG    (1 << 7) /* assuming byte-wide access */
#define HAWK_SMC_DPE_CKALL (1 << 1) /* assuming byte-wide access */
#define HAWK_SMC_DPE_ME    (1 << 0) /* assuming byte-wide access */

#define HAWK_SMC_APELOG    (1 << 7) /* assuming byte-wide access */
#define HAWK_SMC_APE_ME    (1 << 0) /* assuming byte-wide access */

/* Hawk SMC ROM A Base/Size register bit masks */

#define HAWK_SMC_ROMA_BASE_MASK     0xFFF00000  /* Base Address mask */
#define HAWK_SMC_ROMA_64BIT         0x00080000  /* 64-bit wide */
#define HAWK_SMC_ROMA_SIZE_MASK     0x00070000  /* Bank size mask */
#define HAWK_SMC_ROMA_SIZE_64MB     0x00060000  /* 64MB Bank size */
#define HAWK_SMC_ROMA_SIZE_32MB     0x00050000  /* 32MB Bank size */
#define HAWK_SMC_ROMA_SIZE_16MB     0x00040000  /* 16MB Bank size */
#define HAWK_SMC_ROMA_SIZE_8MB      0x00030000  /*  8MB Bank size */
#define HAWK_SMC_ROMA_SIZE_4MB      0x00020000  /*  4MB Bank size */
#define HAWK_SMC_ROMA_SIZE_2MB      0x00010000  /*  2MB Bank size */
#define HAWK_SMC_ROMA_SIZE_1MB      0x00000000  /*  1MB Bank size */
#define HAWK_SMC_ROMA_RESET_VEC     0x00000004  /* Bank A reset vectors */
#define HAWK_SMC_ROMA_BANK_EN       0x00000002  /* Bank A bank enable */
#define HAWK_SMC_ROMA_WRITE_EN      0x00000001  /* Bank A write enable */

/* Hawk SMC ROM A configuration value */

#define HAWK_SMC_ROMA_CNFG_VALUE    (HAWK_SMC_ROMA_64BIT     | \
                                     HAWK_SMC_ROMA_RESET_VEC | \
                                     HAWK_SMC_ROMA_BANK_EN   | \
                                     HAWK_SMC_ROMA_WRITE_EN)

/* Create the register value to configure the Hawk for True FFS operations. */

#define HAWK_SMC_ROMA_VALUE         (HAWK_SMC_ROMA_CNFG_VALUE | \
                                     FLASH_BASE_ADRS | \
                                     HAWK_SMC_ROMA_SIZE_16MB)

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

/* Hawk X-bus 16550C asynchronous serial ports (COM1 & COM2) */

#define COM1_BASE_ADR	    BRD_UART1_BASE_ADRS   /* serial port 1 */
#define COM2_BASE_ADR	    BRD_UART2_BASE_ADRS   /* serial port 2 */

/* SuperI/O serial ports (COM3 & COM4) */

#ifdef INCLUDE_I8250_SIO
#   define COM3_BASE_ADR	    pc97303_COM1   /* serial port 3 */
#   define COM4_BASE_ADR	    pc97303_COM2   /* serial port 4 */
#endif /* INCLUDE_I8250_SIO */

#define UART_REG_ADDR_INTERVAL1  16  	/* addr diff of adjacent regs 16550 */
#define UART_REG_ADDR_INTERVAL2  1    	/* addr diff of adjacent regs SIO */


#ifdef INCLUDE_I8250_SIO
#   define N_UART_CHANNELS	    4		/* No. UART I/O channels */
#else /* INCLUDE_I8250_SIO */
#   define N_UART_CHANNELS	    2		/* No. UART I/O channels */
#endif /* INCLUDE_I8250_SIO */

/* z85230 asyncronous serial ports (COM5 & COM6) */

#ifdef INCLUDE_Z85230_SIO
#   define BAUD_CLK_FREQ	    10000000    /* 10 MHz "P Clock" (fixed) */
#   define REG_8530_WRITE(reg,val) sysOutByte((UINT32)(reg), (UINT8)(val))
#   define REG_8530_READ(reg,pVal) *(UINT8 *)pVal = sysInByte((UINT32)reg)
#   define DATA_REG_8530_DIRECT
/* #  define Z8530_RESET_DELAY_COUNT 2000 */
#   define Z8530_RESET_DELAY \
        { \
        int i; \
        for (i = 0; i < Z8530_RESET_DELAY_COUNT; i++) \
                ; /* do nothing */ \
        }
#endif /* INCLUDE_Z85230_SIO */

/* total number of serial ports */

#if defined(INCLUDE_I8250_SIO) && defined(INCLUDE_Z85230_SIO)
#   define N_SIO_CHANNELS	6		/* No. serial I/O channels */
#elif defined(INCLUDE_I8250_SIO)
#   define N_SIO_CHANNELS	4		/* No. serial I/O channels */
#elif defined(INCLUDE_Z85230_SIO)
#   define N_SIO_CHANNELS	4		/* No. serial I/O channels */
#else
#   define N_SIO_CHANNELS	2		/* No. serial I/O channels */
#endif /* defined(INCLUDE_I8250_SIO) && defined(INCLUDE_Z85230_SIO) */

/* 
 * non-volatile (battery-backed) ram defines.  The top 16 bytes are used for 
 * the RTC registers.
 */

#define	BBRAM_ADRS		0		/* base address */
#define	BBRAM_SIZE		0x7ff0		/* 32k NVRAM Total Size */

/* MK48T37V register settings */

/* flag register */

#define MK48T_FLAGS		((char *)(BBRAM_ADRS + 0x7ff0))

/* alarm clock registers, 4 1byte locations */

#define ALARM_CLOCK		((char *)(BBRAM_ADRS + 0x7ff2))

/* interrupt register */

#define MK48T_INTR		((char *)(BBRAM_ADRS + 0x7ff6))

/* watchdog timer register */

#define WD_TIMER		((char *)(BBRAM_ADRS + 0x7ff7))

/* MK48TXX bb time of day clk, 8 1byte locations */

#define	TOD_CLOCK 		((char *)(BBRAM_ADRS + 0x7ff8))

#define NV_RAM_IO_MAPPED	/* nvram is io mapped in ISA space */
#define NV_RAM_READ(x)		sysNvRead (x)
#define NV_RAM_WRITE(x,y)	sysNvWrite (x,y)
#define NV_RAM_LSB_REG		m48TXX_LSB_REG
#define NV_RAM_MSB_REG		m48TXX_MSB_REG
#define NV_RAM_DAT_REG		m48TXX_DAT_REG

/*
 * VME MASTER WINDOW FOR UNIV II REGISTERS (to access mailbox registers)
 *
 * Universe PCI slave (VME master) window 0
 * 
 * Map access to A32 VMEbus (Universe Register Access Images - RAI) (64K)
 */

#define VME_MBOX0_OFFSET    0x348       /* offset to mailbox 0 reg */
#define VME_RAI_SLV_SIZE    0x00001000	/* this boards reg space 4KB */
#define	VME_RAI_MSTR_SIZE   0x00010000	/* total system reg space 64KB*/

#define VME_A32_REG_SIZE    VME_RAI_SLV_SIZE   /* individual reg space */
#define VME_A32_REG_SPACE   VME_RAI_MSTR_SIZE  /* total reg space */

#ifndef EXTENDED_VME	/* PReP values */

/*
 * Map access to A32 VMEbus (Universe Regs) - 64K  MAP FOR PReP vxWorks
 * This maps: MPU RANGE:     0xf0000000 - 0xf000ffff  
 *        to: PCI RANGE:     0x30000000 - 0x3000ffff
 *        to: VME RANGE:     0x40000000 - 0x4000ffff
 */

#   define VME_RAI_MSTR_LOCAL	0xf0000000
#   define VME_RAI_MSTR_BUS	0x40000000
#   define VAL_LSI0_BS_VALUE	(VME_RAI_MSTR_LOCAL - PCI_MSTR_MEMIO_LOCAL)

#else			/* EXTENDED_VME values */

/*
 * Map access to A32 VMEbus (Universe Regs) - 64K  MAP FOR EXTENDED_VME 
 * map vxWorks
 * This maps: MPU RANGE:     0xfb000000 - 0xfb00ffff
 *        to: PCI RANGE:     0xfb000000 - 0xfb00ffff
 *        to: VME RANGE:     0xfb000000 - 0xfb00ffff
 */

#   define VME_RAI_MSTR_LOCAL	0xfb000000 
#   define VME_RAI_MSTR_BUS	0xfb000000 
#   define VAL_LSI0_BS_VALUE	VME_RAI_MSTR_LOCAL
#endif /* EXTENDED_VME */

#define VAL_LSI0_BS		(VAL_LSI0_BS_VALUE)
#define VAL_LSI0_BD		(VAL_LSI0_BS + VME_RAI_MSTR_SIZE)
#define VAL_LSI0_TO		(VME_RAI_MSTR_BUS - VAL_LSI0_BS)
#define VAL_LSI0_CTL		( LSI_CTL_EN     | LSI_CTL_WP  |\
                                  LSI_CTL_D64    | LSI_CTL_A32 |\
                                  LSI_CTL_DATA   | LSI_CTL_USR |\
                                  LSI_CTL_SINGLE | LSI_CTL_PCI_MEM )

/*
 * VME MASTER WINDOW FOR A32 SPACE
 *
 * Universe PCI slave (VME master) window 1
 */

#ifndef EXTENDED_VME           /* PReP values */

/*
 * Map access to A32 VMEbus - 128M      MAP FOR PReP vxWorks
 * This maps: MPU RANGE: 0xd8000000       - 0xdfffffff
 *        to: PCI RANGE: 0x18000000       - 0x1fffffff
 *        to: VME RANGE: VME_A32_MSTR_BUS - VME_A32_MSTR_BUS + VME_A32_MSTR_SIZE
 */

#   define VAL_LSI1_BS_VALUE     (VME_A32_MSTR_LOCAL - PCI_MSTR_MEMIO_LOCAL)
#   if  (VME_A32_MSTR_SIZE > 0x08000000)
#       error VME_A32_MSTR_SIZE cannot exceed 128 MB
#   endif /* (VME_A32_MSTR_SIZE > 0x08000000) */

#else                           /* EXTENDED_VME values */

/*
 * Map access to A32 VMEbus - (up to ~1.75GB)  MAP FOR EXTENDED_VME map vxWorks
 * This maps: MPU RANGE: VME_A32_MSTR_LOCAL  - VME_A32_MSTR_LOCAL +
 *                                             VME_A32_MSTR_SIZE
 *        to: PCI RANGE: VME_A32_MSTR_LOCAL  - VME_A32_MSTR_LOCAL +
 *                                             VME_A32_MSTR_SIZE
 *        to: VME RANGE: VME_A32_MSTR_BUS    - VME_A32_MSTR_BUS +
 *                                             VME_A32_MSTR_SIZE
 */

#   define VAL_LSI1_BS_VALUE     (VME_A32_MSTR_LOCAL)
#endif /* EXTENDED_VME */

#if  ((VME_A32_MSTR_BUS + VME_A32_MSTR_SIZE) > VME_RAI_MSTR_BUS) && \
      (VME_A32_MSTR_BUS < (VME_RAI_MSTR_BUS + VME_RAI_MSTR_SIZE))
#   warning VME A32 space overlaps VME Universe/SEM (MAILBOX) REG space
#endif /* VME_A32_MSTR_BUS + ... */

#define VAL_LSI1_BS     (VAL_LSI1_BS_VALUE)
#define VAL_LSI1_BD     (VAL_LSI1_BS + VME_A32_MSTR_SIZE)
#define VAL_LSI1_TO     (VME_A32_MSTR_BUS - VAL_LSI1_BS)   /*  LSI_CTL_WP  | */
#define VAL_LSI1_CTL    ( LSI_CTL_EN     | LSI_CTL_WP  |\
                          LSI_CTL_D64    | LSI_CTL_A32 |\
                          LSI_CTL_DATA   | LSI_CTL_USR |\
                          LSI_CTL_SINGLE | LSI_CTL_PCI_MEM )

/*
 * VME MASTER WINDOW FOR A24 SPACE
 *
 * Universe PCI slave (VME master) window 2
 */

#ifndef EXTENDED_VME		/* PReP values */

/*
 * Map access to A24 VMEbus - 16M       MAP FOR PReP vxWorks
 * This maps: MPU RANGE:     0xe0000000 - 0xe0ffffff
 *        to: PCI RANGE:     0x20000000 - 0x20ffffff
 *        to: VME RANGE:     0xff000000 - 0xffffffff
 */

#   define VME_A24_MSTR_LOCAL   0xe0000000
#   define VAL_LSI2_BS_VALUE    (VME_A24_MSTR_LOCAL - PCI_MSTR_MEMIO_LOCAL)
#else				/* EXTENDED_VME values */

/*
 * Map access to A24 VMEbus - 16M       MAP FOR EXTENDED_VME map vxWorks
 * This maps: MPU RANGE:     0xfa000000 - 0xfaffffff
 *        to: PCI RANGE:     0xfa000000 - 0xfaffffff
 *        to: VME RANGE:     0xff000000 - 0xffffffff
 */

#   define VME_A24_MSTR_LOCAL 0xfa000000
#   define VAL_LSI2_BS_VALUE	(VME_A24_MSTR_LOCAL)

#   if  ((VME_A32_MSTR_LOCAL + VME_A32_MSTR_SIZE) > VME_A24_MSTR_LOCAL)
#       error VME A32 space extends into the A24 memory space
#   endif /* ((VME_A32_MSTR_LOCAL + VME_A32_MSTR_SIZE) > VME_A24_MSTR_LOCAL) */

#endif /* EXTENDED_VME */
#if (VME_A24_MSTR_SIZE > 0x01000000)
#   error Maximum VME_A24_MSTR_SIZE cannot exceed 16 MB
#endif /* (VME_A24_MSTR_SIZE > 0x01000000) */
#define VAL_LSI2_BS	(VAL_LSI2_BS_VALUE)
#define VAL_LSI2_BD	(VAL_LSI2_BS + VME_A24_MSTR_SIZE)
#define VAL_LSI2_TO	(0xff000000 + VME_A24_MSTR_BUS - VAL_LSI2_BS)
#define VAL_LSI2_CTL	( LSI_CTL_EN     | LSI_CTL_WP  |\
                          LSI_CTL_D64    | LSI_CTL_A24 |\
                          LSI_CTL_DATA   | LSI_CTL_USR |\
                          LSI_CTL_SINGLE | LSI_CTL_PCI_MEM )

/*
 * VME MASTER WINDOW FOR A16 SPACE
 *
 * Universe PCI slave (VME master) window 3
 */

#ifndef EXTENDED_VME		/* PReP values */

/*
 * Map access to A16 VMEbus - 64K       MAP FOR PReP vxWorks
 * This maps: MPU RANGE:     0xefff0000 - 0xefffffff  
 *        to: PCI RANGE:     0x2fff0000 - 0x2fffffff
 *        to: VME RANGE:     0xffff0000 - 0xffffffff
 */

#   define VME_A16_MSTR_LOCAL	0xefff0000	
#   define VAL_LSI3_BS_VALUE	(VME_A16_MSTR_LOCAL - PCI_MSTR_MEMIO_LOCAL)
#else				/* EXTENDED_VME values */

/*
 * Map access to A16 VMEbus - 64K       MAP FOR EXTENDED_VME map vxWorks
 * This maps: MPU RANGE:     0xfbff0000 - 0xfbffffff
 *        to: PCI RANGE:     0xfbff0000 - 0xfbffffff
 *        to: VME RANGE:     0xfbff0000 - 0xfbffffff
 */

#   define VME_A16_MSTR_LOCAL 0xfbff0000
#   define VAL_LSI3_BS_VALUE	(VME_A16_MSTR_LOCAL)
#endif /* EXTENDED_VME */
#if (VME_A16_MSTR_SIZE > 0x00010000)
#   error Maximum VME_A16_MSTR_SIZE cannot exceed 64 KB
#endif /* (VME_A16_MSTR_SIZE > 0x00010000) */
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
#   define VME_MSTR_LO_ADRS      (VME_A32_MSTR_LOCAL)
#   define VME_MSTR_HI_ADRS      (VME_RAI_MSTR_LOCAL + VME_RAI_MSTR_SIZE)
#else
#   define VME_MSTR_LO_ADRS      (VME_A32_MSTR_LOCAL)
#   define VME_MSTR_HI_ADRS      (VME_A16_MSTR_LOCAL + VME_A16_MSTR_SIZE)
#endif  /* EXTENDED_VME */

/* 
 * Defines for the VME SLAVE WINDOWS
 *
 * Allows for the decoding of addresses on the VMEbus.
 */

/*
 * VME SLAVE WINDOW FOR A32 SPACE
 *
 * Universe VME slave window 1
 *
 * VME bus A32 window to access the master node's local memory.
 * This VME Slave window is only used by the master node.
 * 
 *                      vxWorks PReP Map
 * This maps: VME RANGE:     0x08000000 - (0x08000000 + VME_A32_SLV_SIZE - 1)
 *        to: PCI RANGE:     0x80000000 - 0x80000000 + VME_A32_SLV_SIZE-1
 *        to: MPU RANGE:     0x00000000 - VME_A32_SLV_SIZE-1
 *
 *                      vxWorks EXTENDED_VME Map
 * This maps: VME RANGE:     0x08000000 - (0x08000000 + VME_A32_SLV_SIZE - 1)
 *        to: PCI RANGE:     0x00000000 - VME_A32_SLV_SIZE-1
 *        to: MPU RANGE:     0x00000000 - VME_A32_SLV_SIZE-1
 */

#define VAL_VSI1_BS	(VME_A32_SLV_BUS)

#if (SM_OFF_BOARD == TRUE)
#   define VAL_VSI1_BD	(VAL_VSI1_BS + VME_A32_SLV_SIZE)
#else
#   ifdef ANY_BRDS_IN_CHASSIS_NOT_RMW
#       define VAL_VSI1_BD	(VAL_VSI1_BS + VME_A32_SLV_SIZE)
#   else
#       define VAL_VSI1_BD     (VAL_VSI1_BS + SM_MEM_ADRS)
#   endif /* ANY_BRDS_IN_CHASSIS_NOT_RMW */
#endif /* SM_OFF_BOARD */

#define VAL_VSI1_TO	(PCI_SLV_MEM_BUS - VAL_VSI1_BS + VME_A32_SLV_LOCAL)
#define VAL_VSI1_CTL	( VSI_CTL_EN       | VSI_CTL_PREN    | \
			  VSI_CTL_AM_DATA  | VSI_CTL_AM_PGM  | \
			  VSI_CTL_AM_SUPER | VSI_CTL_AM_USER | \
			  VSI_CTL_VAS_A32  | VSI_CTL_LAS_MEM | \
			  VSI_CTL_LD64EN )

#ifdef A24_SLV_WINDOW
/*
 * VME SLAVE WINDOW FOR A24 SPACE
 *
 * Universe VME slave window 2
 *
 * VME bus A24 window to access the master node's local memory.
 * This VME Slave window is only used by the master node.
 *
 *                      vxWorks PReP Map
 * This maps: VME RANGE:     0x00000000 - (VME_A24_SLV_SIZE - 1)
 *        to: PCI RANGE:     0x80000000 - 0x80000000 + VME_A24_SLV_SIZE-1
 *        to: MPU RANGE:     0x00000000 - VME_A24_SLV_SIZE-1
 *
 *                      vxWorks EXTENDED_VME Map
 * This maps: VME RANGE:     0x00000000 - (VME_A24_SLV_SIZE - 1)
 *        to: PCI RANGE:     0x00000000 - VME_A24_SLV_SIZE-1
 *        to: MPU RANGE:     0x00000000 - VME_A24_SLV_SIZE-1
 */

#   define VAL_VSI2_BS  (VME_A24_SLV_BUS)
#   define VAL_VSI2_BD	(VAL_VSI2_BS + VME_A24_SLV_SIZE)
#   define VAL_VSI2_TO	(PCI_SLV_MEM_BUS - VAL_VSI2_BS + VME_A24_SLV_LOCAL)
#   define VAL_VSI2_CTL ( VSI_CTL_EN       | VSI_CTL_PREN    | \
                          VSI_CTL_AM_DATA  | VSI_CTL_AM_PGM  | \
                          VSI_CTL_AM_SUPER | VSI_CTL_AM_USER | \
                          VSI_CTL_VAS_A24  | VSI_CTL_LAS_MEM | \
                          VSI_CTL_LD64EN )
#endif /* A24_SLV_WINDOW */

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
 * VME SLAVE WINDOW FOR A16 SPACE
 *
 * Universe VME A16 Slave window, does not exist.  (Could use window 3.)
 */

#define VME_A16_SLV_BUS		0x0
#define VME_A16_SLV_SIZE	0x0	/* 0, window is disabled */
#define VME_A16_SLV_LOCAL	0x0

/* defines for the UNIV II mailbox register space */

#define VAL_RAI_CTL		(VRAI_CTL_EN       | \
                                 VRAI_CTL_AM_DATA  | VRAI_CTL_AM_PGM  | \
                                 VRAI_CTL_AM_SUPER | VRAI_CTL_AM_USER | \
                                 VRAI_CTL_VAS_A32)
#define VAL_RAI_BS		(VME_RAI_MSTR_BUS + (sysProcNumGet() * \
				 VME_RAI_SLV_SIZE))

/* Universe Special Cycle Generator values */

#define	VME_SCG_COMPARE_MASK		0xffffffff
#define VME_SCG_COMPARE_TO_SET		0x00000000
#define VME_SCG_SWAP_TO_SET		0x80000000
#define VME_SCG_COMPARE_TO_CLEAR	0x80000000
#define VME_SCG_SWAP_TO_CLEAR		0x00000000


/* sym895 delay loop count - must be adjusted for CPU speed */

#define SYM895_DELAY_MULT	60

/* ncr810/ncr825 delay loop count */

#define NCR810_DELAY_MULT	10

#define SYS_REG_SIOP_HW_REGS	{0,0,0,0,0,1,0,0,0,0,0}

/* SUPER I/O defines */

#define SUPER_IO_BASE1		0x002e
#define SUPER_IO_BASE2		0x015c
#define SUPER_IO_KBC_BASE_ADR	0x60
#define SUPER_IO_KBC_CMND_ADR	0x64
#define SUPER_IO_FDC_BASEHI	0x03
#define SUPER_IO_FDC_BASELO	0xf0
#define SUPER_IO_COM1_BASEHI    0x03
#define SUPER_IO_COM1_BASELO    0xf8
#define SUPER_IO_COM2_BASEHI    0x02
#define SUPER_IO_COM2_BASELO    0xf8
#define SUPER_IO_PP_BASEHI      0x03
#define SUPER_IO_PP_BASELO      0xbc
#define SUPER_IO_FDC_PS2	0x40
#define SUPER_IO_PP_CFG		0x12
#define SUPER_IO_ENBL_INTS	0x02

/* INTERRUPT DEFINES */

#define ISA_INTERRUPT_BASE	0x00
#define EXT_INTERRUPT_BASE	0x10
#define TIMER_INTERRUPT_BASE	0x20
#define IPI_INTERRUPT_BASE	0x24
#define ERR_INTERRUPT_BASE	0x28
#define	ESCC_INTERRUPT_BASE	0x00

/* interrupt Level definitions */

#define PIB_INT_LVL		( 0x00 + EXT_INTERRUPT_BASE )

/* UART interrupt level is shared by 2 16550 COM Ports */

#define COM1_INT_LVL		( 0x01 + EXT_INTERRUPT_BASE )
#define COM2_INT_LVL		( 0x01 + EXT_INTERRUPT_BASE )

/* Super I/O (when IPMC761 is installed) Com port interrupts */

#define COM3_INT_LVL		( 0x04 + ISA_INTERRUPT_BASE )
#define COM4_INT_LVL		( 0x03 + ISA_INTERRUPT_BASE )

/* z85230 (when IPMC761 is installed) Com port interrupts (IRQ9 is shared) */

#define COM5_INT_LVL		( 0x09 + ISA_INTERRUPT_BASE )
#define COM6_INT_LVL		( 0x09 + ISA_INTERRUPT_BASE )
#define Z85230_INT_LVL		COM5_INT_LVL

/* Front panel Ethernet interrupt level */
#define LN1_INT_LVL             ( 0x02 + EXT_INTERRUPT_BASE )

/* Rear panel Ethernet interrupt level */
#define LN2_INT_LVL             ( 0x0d + EXT_INTERRUPT_BASE )

/* Universe LINT#0 interrupt level ( used for the UNIVERSE chip )*/
#define UNIV_INT_LVL0           ( 0x05 + EXT_INTERRUPT_BASE )

/* Universe LINT#1 interrupt level */
#define UNIV_INT_LVL1           ( 0x06 + EXT_INTERRUPT_BASE )

/* Universe LINT#2 interrupt level */
#define UNIV_INT_LVL2           ( 0x07 + EXT_INTERRUPT_BASE )

/* Universe LINT#3 interrupt level */
#define UNIV_INT_LVL3           ( 0x08 + EXT_INTERRUPT_BASE )

/* Watchdog Timer Level 1 interrupt level */
#define WDTL1_INT_LVL           ( 0x03 + EXT_INTERRUPT_BASE )

/* Watchdog Timer Level 2 interrupt level */
#define WDTL2_INT_LVL           ( 0x03 + EXT_INTERRUPT_BASE )

/* PCI expansion INTA */
#define PCI_INTA_LVL            ( 0x09 + EXT_INTERRUPT_BASE )

/*  PCI expansion INTB */
#define PCI_INTB_LVL            ( 0x0a + EXT_INTERRUPT_BASE )

/* PCI expansion INTC */
#define PCI_INTC_LVL            ( 0x0b + EXT_INTERRUPT_BASE )

/* PCI expansion INTD */
#define PCI_INTD_LVL            ( 0x0c + EXT_INTERRUPT_BASE )

/* Abort interrupt level */
#define ABORT_INT_LVL		( 0x0e + EXT_INTERRUPT_BASE )

/* PCI INTA# */
#define PCI_INT_LVL1		( 0x09 + EXT_INTERRUPT_BASE )

/* PCI INTB# */
#define PCI_INT_LVL2		( 0x0a + EXT_INTERRUPT_BASE )

/* PCI INTC# */
#define PCI_INT_LVL3		( 0x0b + EXT_INTERRUPT_BASE )

/* PCI INTD# */
#define PCI_INT_LVL4		( 0x0c + EXT_INTERRUPT_BASE )

/* Timer interrupt level (IPI0) */
#define TIMER0_INT_LVL          ( 0x00 + TIMER_INTERRUPT_BASE )

/* parallel port interrupt level */
#define PP_INT_LVL		( 0x07 + ISA_INTERRUPT_BASE )

/* z8536 timer interrupt level */
#define Z8536_INT_LVL		( 0x00 + ISA_INTERRUPT_BASE )

/* Abort interrupt level */
#define ABORT_LVL		( 0x0e + EXT_INTERRUPT_BASE )

/* RTC - Alarm interrupt level */
#define RTC_ALARM_LVL		( 0x0f + EXT_INTERRUPT_BASE )
 
#define UNIV_INT_LVL		UNIV_INT_LVL0   /* universe int level */

/* interrupt vector definitions */

#define INT_VEC_IRQ0			0x00	/* vector for IRQ0 */

/* PCI/MPIC interrupt vectors */

#define COM1_INT_VEC        INT_VEC_IRQ0 + COM1_INT_LVL
#define COM2_INT_VEC        INT_VEC_IRQ0 + COM2_INT_LVL
#define COM3_INT_VEC        INT_VEC_IRQ0 + COM3_INT_LVL
#define COM4_INT_VEC        INT_VEC_IRQ0 + COM4_INT_LVL
#define COM5_INT_VEC        INT_VEC_IRQ0 + COM5_INT_LVL
#define COM6_INT_VEC        INT_VEC_IRQ0 + COM6_INT_LVL
#define Z8536_INT_VEC       INT_VEC_IRQ0 + Z8536_INT_LVL
#define Z85230_INT_VEC      INT_VEC_IRQ0 + Z85230_INT_LVL
#define WDTL1_INT_VEC       INT_VEC_IRQ0 + WDTL1_INT_LVL
#define WDTL2_INT_VEC       INT_VEC_IRQ0 + WDTL2_INT_LVL
#define UNIV_INT_VEC        INT_VEC_IRQ0 + UNIV_INT_LVL
#define	PCI_INTA_VEC        INT_VEC_IRQ0 + PCI_INT_LVL1
#define	PCI_INTB_VEC        INT_VEC_IRQ0 + PCI_INT_LVL2
#define	PCI_INTC_VEC        INT_VEC_IRQ0 + PCI_INT_LVL3
#define	PCI_INTD_VEC        INT_VEC_IRQ0 + PCI_INT_LVL4
#define LN1_INT_VEC         INT_VEC_IRQ0 + LN1_INT_LVL
#define LN2_INT_VEC         INT_VEC_IRQ0 + LN2_INT_LVL
#define PIB_INT_VEC         INT_VEC_IRQ0 + PIB_INT_LVL

#define RTC_ALARM_VEC       INT_VEC_IRQ0 + RTC_ALARM_LVL

/* Timer interrupt vectors */

#define TIMER0_INT_VEC              ( INT_VEC_IRQ0 + TIMER0_INT_LVL )

/* UNIVERSE chip interrupt vector defines */

#define UNIV_DMA_INT_VEC         0x56
#define UNIV_VME_SW_IACK_INT_VEC 0x57
#define UNIV_PCI_SW_INT_VEC      0x58
#define UNIV_VOWN_INT_VEC        0x59
#define UNIV_LERR_INT_VEC        0x5a
#define UNIV_VERR_INT_VEC        0x5c
#define UNIV_SYSFAIL_INT_VEC     0x5d
#define UNIV_ACFAIL_INT_VEC      0x5f
#define UNIV_MBOX0_INT_VEC       0x60    /* Mailbox 0 interrupt vector */
#define UNIV_MBOX1_INT_VEC       0x61    /* Mailbox 1 interrupt vector */
#define UNIV_MBOX2_INT_VEC       0x62    /* Mailbox 2 interrupt vector */
#define UNIV_MBOX3_INT_VEC       0x63    /* Mailbox 3 interrupt vector */
#define UNIV_LM0_INT_VEC         0x64    /* Loc. Mon. 0 interrupt vec. */
#define UNIV_LM1_INT_VEC         0x65    /* Loc. Mon. 1 interrupt vec. */
#define UNIV_LM2_INT_VEC         0x66    /* Loc. Mon. 2 interrupt vec. */
#define UNIV_LM3_INT_VEC         0x67    /* Loc. Mon. 3 interrupt vec. */

/*
 * UNIVERSE REGISTER SETUP FOR CPU <-> VME MAPPING (via PCI)
 *
 * ALL VAL_LSIx and VAL_VSIx values MUST be multiples of 64KB !!!
 * Except VAL_LSI0 and VAL_VSI0, which must be multiples of 4KB !
 */

/*
 * Address range definitions for VME and PCI buses.
 *
 * Used with vxMemProbe() hook sysBusProbe().
 */

#define IS_VME_ADDRESS(adrs) (((UINT32)(adrs) >= (UINT32)VME_MSTR_LO_ADRS) && \
((UINT32)(adrs) < (UINT32)VME_MSTR_HI_ADRS))

#define IS_PCI_ADDRESS(adrs)	((((UINT32)(adrs) >= \
				 (UINT32)PCI_MSTR_MEMIO_LOCAL) && \
				 ((UINT32)(adrs) < \
				 (UINT32)(PCI_MSTR_MEM_LOCAL + \
				 (PCI_MSTR_MEM_SIZE-1)))) || \
				(((UINT32)(adrs) >= \
				 (UINT32)ISA_MSTR_IO_LOCAL) && \
				 ((UINT32)(adrs) < \
				 (UINT32)(PCI_MSTR_IO_LOCAL + \
				 (PCI_MSTR_IO_SIZE-1)))))

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

#define SYS_SIOP_HW_REGS    {0,0,0,0,0,1,0,0,0,0,0}

/* PCI bus number for primary PCI bus */

#define PCI_PRIMARY_BUS         0

/* Fixed PMC Span (PCI-to-PCI Bridge) configuration parameters */

#define P2P_CLR_STATUS          0xFFFF0000
#define P2P_SEC_BUS_RESET       (0x0040 << 16)
#define P2P_CLK_ENABLE          0x00       /* enable clocks on all slots */
#define P2P_PMC_DISABLE         0
#define P2P_PMC_ENABLE          7

/*
 * Support for determining if we're ROM based or not.  _sysInit
 * saves the startType parameter at location ROM_BASED_FLAG.
 */

#define PCI_AUTOCONFIG_FLAG_OFFSET ( 0x4c00 )
#define PCI_AUTOCONFIG_FLAG        ( *(UCHAR *)(LOCAL_MEM_LOCAL_ADRS + \
                                    PCI_AUTOCONFIG_FLAG_OFFSET) )
#define PCI_AUTOCONFIG_DONE        ( PCI_AUTOCONFIG_FLAG != 0 )

#ifndef _ASMLANGUAGE

/* PMC Span (DEC21150 PCI-to-PCI Bridge) Configuration Parameter Structure */

typedef struct pmc_span_parm	/* PMC_SPAN */
    {
    UINT16  parmOffset;		/* offset into configuration header */
    UINT16  parmSize;		/* parmValue size (1, 2 or 4 bytes) */
    UINT32  parmValue;		/* parameter value placed at this offset */
    } PMC_SPAN;

#endif  /* _ASMLANGUAGE */

#ifdef __cplusplus
    }
#endif /* __cplusplus */

#endif /* INCmv5100h */
