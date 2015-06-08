/* mv2600.h - Motorola PowerPlus board header */

/* Copyright 1984-2001 Wind River Systems, Inc. */
/* Copyright 1996,1997,1998,1999 Motorola, Inc. All Rights Reserved */

/*
modification history
--------------------
02r,21oct01,dtr  Changing default secondary scsi id.
02q,09oct01,dat  SPR 24258 adding prototypes for unique functions
02p,16sep01,dat  Use of WRS_ASM macro
02o,11oct99,dmw  Removed VME_A32_MSTR_LOCAL, moved to config.h
02n,27sep99,stv	 added reporting of configuration error for mv2300 (SPR 28572). 
02m,18aug99,dmw  Added 21143 define and mv2300SC define.
02l,16feb99,mas  added RAVEN_MPC_WDT1CNTL/_WDT2CNTL defs for Raven3 (SPR 24453)
02k,10feb99,tm   Fixed duplicate definition of PCI2DRAM_BASE_ADRS (SPR 21488)
02j,07aug98,tb   Fixed support for SM_OFF_BOARD
02i,07aug98,tb   Added TAS support which uses VMEbus RMW
02h,07aug98,scb  added support for MPC750 (Arthur).
02g,13oct98,dat  SPR 20654, added ravenAuxClk for mv2300
02f,17jul98,tm   added PCI_ID_PRI_LAN, PCI_ID_SEC_LAN for PCI AutoConfig
02e,12jun98,mas  made MV2600_BMFR_BRGP define board dependent (SPR 21402).
02d,15apr98,db   added support for floppy disk driver.
02c,14apr98,ms_  merged Motorola mv2700 support
02b,06apr98,dat  added PCI_ID_SEC_SCSI and INCLUDE_PCI
02a,07nov97,mas/ corrected VME range comments on A32 slave window; changed VME
	    tb   A24 base address to 0xff000000; enabled 64-bit PCI xfers for
		 VME A32 slave window (SPR 9726).
01z,05nov97,mas  made all VME master windows D64 with write posting (SPR 9717).
01y,31oct97,mas  fixed P2P_NPMEM_SPACE_LIMIT_ADRS definition (SPR 9654).
01x,10sep97,mas  added PMC_INT_LVL1 - 4 for non-MPIC configurations.
01w,02sep97,mas  removed un-needed check of VME_A32_MSTR_BUS, VME_A24_MSTR_BUS.
01v,14aug97,mas  Extended VME addressing is now default; no slave VME A24 space
		 (for cumulative patch release 9/97).
01u,01aug97,mas  added primary PCI bus interrupt vector numbers PCI_PRI_INTA_VEC
		 PCI_PRI_INTB_VEC, PCI_PRI_INTC_VEC, PCI_PRI_INTD_VEC (SPR 9063)
01t,25jul97,srr/ added 604r (Mach 5) type define CPU_TYPE_604R (SPR 8911).
	    mas
01s,17jul97,mas  added FALCON_DRAM_ATTR and DRAM_SIZE (SPR 8824).
01r,14jul97,mas  added IS_VME_ADDRESS and IS_PCI_ADDRESS defs (SPR 8022).
		 Added reg bit masks for MPC Error Enable/Status and PCI Status.
01q,09jul97,mas  added UNIV_VOWN_INT_VEC definition (SPR 8896); added 'volatile'
		 to in-line assembler macros; disabled write posting in Universe
		 VME slave setup (SPR 8897); NCR810_DELAY_MULT is now defined as
		 actual loop count (SPR 8842).
01p,05jun97,mas  added support for serial ports 3 & 4 (Z85230 ESCC). (SPR 8566)
01o,29may97,srr  Add RAVEN macros for vxMemProbe, chg'd mislabelled macros
		 (MCG MR #67, 69)
01n,12may97,dat  moving things between config.h and mv2600.h
01m,09may97,mas  added extended VME addressing (SPR 8410).
		 added z8536 I/O port bit mappings (SPR 8432).
01l,29apr97,dat  added PCI_CLINE_SZ and PCI_LAT_TIMER, from old pci.h
01k,24apr97,mas  added Motorola support for MPIC (SPR 8170).
01j,11apr97,dat  moved macros between here and config.h.  User configurable
		 macros are in config.h, fixed ones are here in mv2600.h
01i,10apr97,mas  added defs for EIEIO_SYNC, EIEIO, PCI_PRIMARY_CAR,
                 PCI_PRIMARY_CDR, CNFG_PMCSPAN_ADRS, PCI_PRIMARY_BUS,
                 PCI_SECONDARY_BUS, PCI_SUBORD_BUS, MV2600_BMFR_BRGP,
                 PCI_ID_BR_DEC21150, PCI_MAX_BUS and PMC_SPAN; updated 
		 PCI_ID_IBC (SPR 8226).
01h,01apr97,dat  new VME configuration macros, SPR 8271
01g,18feb97,mas  added DEC_CLOCK_FREQ definition; changed UNIV_SW_IACK_INT_VEC
		 to UNIV_PCI_SW_INT_VEC; added UNIV_VME_SW_IACK_INT_VEC
		 (SPR 7772, 7811).
01f,10jan97,dat  chg'd BBRAM_SIZE, to exclude RTC chip
01e,02jan97,dat  mod history fix, chg'd MV1600_SIOP* to MV2600_SIOP*
01d,18dec96,tb   [Motorola] added MV2600_MCR_ (SPR 7525).
01c,17dec96,mas  moved raven.h contents to this file (SPR 7525).
01b,14jul96,rcp  modified the reference to the PCI configuration regs.
01a,14jul96,mot  written. (from ver 01f, mv1600.h)
*/

/*
This file contains I/O addresses and related constants for the
Motorola PowerPlus VME board. 
*/

#ifndef	INCmv260xh
#define	INCmv260xh

#ifdef __cplusplus
extern "C" {
#endif

/* Prototypes for special BSP functions, may be useful for customer */

#ifndef _ASMLANGUAGE
void	sysUsDelay (UINT32);	/* spin delay for X microseconds */
int	sysGetBusSpd (void);	/* return MPC bus speed */
UINT	sysGetMpuSpd (void);	/* return MPU speed */
void	sysDebugMsg (char *);	/* print message using polled mode */
#endif /* _ASMLANGUAGE*/

/* Aux Clock available only with MPIC for mv2300 */

#if defined(MV2300) && (!defined(INCLUDE_MPIC)) && defined(INCLUDE_AUXCLK)
#   error "INCLUDE_AUXCLK not available on MV2300 without INCLUDE_MPIC"
#endif				

#define INCLUDE_PCI		/* always include pci */

/* Floppy disk support */

#define FD_MAX_DRIVES	4
#define FD_BASE_ADDR	pc87303_FDC	/* See super I/O section */
#define FD_DMA_CHAN	2

#ifdef	INCLUDE_FD
#   ifndef	INCLUDE_DOSFS
#       define	INCLUDE_DOSFS	/* file system to be used */
#   endif
#   ifndef	INCLUDE_ISADMA
#       define	INCLUDE_ISADMA	/* uses ISA DMA driver */
#   endif
#   ifndef _ASMLANGUAGE
#	include "blkIo.h"
	/* to stop compiler warnings */
	IMPORT STATUS	fdDrv (UINT, UINT);
	IMPORT BLK_DEV*	fdDevCreate (UINT, UINT, UINT, UINT);
#   endif
#endif


/* Boot Line parameters are stored in the 2nd 256 byte block */

#undef	NV_BOOT_OFFSET
#define NV_BOOT_OFFSET		256 /* skip 1st 256 bytes */
#define NV_RAM_SIZE    		BBRAM_SIZE		
#define NV_RAM_ADRS    		((char *) BBRAM_ADRS)
#define NV_RAM_INTRVL           1

/* Dec 21140 (unit 0) vector and level */

#define	INT_VEC_DC	LN_INT_VEC 		/* interrupt vector */
#define	INT_LVL_DC	LN_INT_LVL		/* interrupt level */
#define DC_DATA_WIDTH   NONE			/* all data widths */
#define IO_ADRS_DC      LAN_BASE_ADRS		/* Base adrs */
#define DC_RAM_PCI_ADRS PCI2DRAM_BASE_ADRS	/* RAM seen from PCI */

/* Dec 21040 (unit 1) vector and level */

#define INT_VEC_DC1     LN2_INT_VEC		/* interrupt vector */
#define INT_LVL_DC1     PMC_INT_LVL2		/* interrupt level */
#define DC1_DATA_WIDTH  NONE			/* all data widths */
#define IO_ADRS_DC1     LAN2_BASE_ADRS		/* Base adrs */
#define DC1_RAM_PCI_ADRS        PCI2DRAM_BASE_ADRS /* RAM seen from PCI */

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

/* Cache Line Size - 32 32-bit value = 128 bytes */

#define PCI_CLINE_SZ		0x20

/* Latency Timer value - 255 PCI clocks */

#define PCI_LAT_TIMER		0xff

/* clock rates */

/* Calculate Memory Bus Rate in Hertz */

#define MEMORY_BUS_SPEED                ( sysGetBusSpd() * 1000000)

/* System clock (decrementer counter) frequency determination */

#define DEC_CLOCK_FREQ          ((sysGetBusSpd()==67)?66666666:33333333)

/* CIO clocks and stuff */

#define	CIO_RESET_DELAY		5000
#define	ZCIO_HZ			2500000 /* 2.5 MHz clock */
#define	CIO_INT_VEC		9
#define	Z8536_TC		ZCIO_HZ

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

#ifndef EXTENDED_VME

/* Psuedo PREP memory map as seen from CPU */

#define CPU_PCI_ISA_IO_ADRS     0x80000000      /* base of ISA I/O space */
#define CPU_PCI_ISA_MEM_ADRS    0xc0000000      /* base of ISA mem space */
#define CPU_PCI_IO_ADRS         0x81000000      /* base of PCI I/O space */
#define CPU_PCI_MEM_ADRS        0xc1000000      /* base of PCI mem space */

#else

/* Extended VME memory map as seen from the CPU */

#define CPU_PCI_ISA_IO_ADRS     0xfe000000      /* base of ISA I/O space */
#define CPU_PCI_ISA_MEM_ADRS    0xfd000000      /* base of ISA mem space */
#define CPU_PCI_IO_ADRS         CPU_PCI_ISA_IO_ADRS  /* base of PCI I/O space */
#define CPU_PCI_MEM_ADRS        CPU_PCI_ISA_MEM_ADRS /* base of PCI mem space */
#endif

#define CPU_PCI_ISA_IO_SIZE	0x00010000	/* 64 kbytes */
#define CPU_PCI_IO_SIZE		0x00800000	/* 8 meg */
#define CPU_PCI_IO_UPPER_ADRS   (CPU_PCI_ISA_IO_ADRS>>16)
#define CPUCRA_HI               (CPU_PCI_ISA_IO_ADRS>>16)
#define CPUCRA_LO               0x0800

#define CPU_PCI_ISA_MEM_SIZE	0x00010000	/* 64 kbytes */
#define CPU_PCI_MEM_SIZE	0x01000000	/* 16 meg */
#define CPU_PCI_MEM_UPPER_ADRS  (CPU_PCI_ISA_MEM_ADRS>>16)

/*
 * PCI MASTER MEMORY WINDOW LIMITS
 *
 * These values are strictly defined by the base memory addresses and window
 * sizes of the spaces defined above.  These values must be correct for the
 * sysBusProbe() memory range checks for the PCI bus to work properly.
 */

#ifndef EXTENDED_VME
#  define PCI_MSTR_LO_ADRS      (CPU_PCI_ISA_IO_ADRS)
#  define PCI_MSTR_HI_ADRS      (CPU_PCI_MEM_ADRS + CPU_PCI_MEM_SIZE)
#else
#  define PCI_MSTR_LO_ADRS      (CPU_PCI_MEM_ADRS)
#  define PCI_MSTR_HI_ADRS      (CPU_PCI_IO_ADRS + CPU_PCI_IO_SIZE)
#endif  /* EXTENDED_VME */

#ifndef INCLUDE_MPIC

/*
 * All starting addresses are correct except for CPU_PCI_IACK_ADRS.
 * This address was selected to allow a minimum 8k memory range
 * for the MMU table entry.
 */

#define CPU_PCI_IACK_ADRS	0xbfffe000	/* covers PCI IACK space */
						/* base = 0xbffffff0; */
#define CPU_PCI_IACK_SIZE	0x00002000	/* 8 kbytes */

#define MPIC_ADDR(reg)		(MPIC_BASE_ADRS + reg)
#define MPIC_GLOBAL_CONFIG_REG	0x01020
#define RESET_CNTRLR		0x80000000

#endif  /* INCLUDE_MPIC */

/* Base address of HW devices as seen from CPU */

#define FALCON_BASE_ADRS	0xfef80000
#define	FALCON_REG_SIZE		0x00010000
#define FALCON_BASE_UPPER_ADRS	(FALCON_BASE_ADRS>>16)
#define FALCON_DRAM_ATTR	0xfef80010
#define RAVEN_BASE_ADRS		0xfeff0000
#define	RAVEN_REG_SIZE		0x00010000
#define	FLASH_BASE_ADRS		0xFF000000
#define	FLASH_MEM_SIZE		0x01000000

/* MPIC configuration defines */

#define MPIC_BASE_ADRS		0xfc000000
#define MPIC_REG_SIZE		0x00040000
#ifndef EXTENDED_VME 
#  define MPIC_PCI_BASE_ADRS	( MPIC_BASE_ADRS - CPU_PCI_ISA_MEM_ADRS )
#else
#  define MPIC_PCI_BASE_ADRS	MPIC_BASE_ADRS
#endif /* Extended VME config */

/* memory map as seen on the PCI bus */

#define PCI_CNFG_ADRS		0x00800000      /* base of PCI config space */
#ifndef EXTENDED_VME
#  define PCI_IO_ADRS		0x01000000      /* base of PCI I/O address */
#  define PCI_MEM_ADRS		0x01000000      /* base of PCI MEM address */
#  define PCI2DRAM_BASE_ADRS	0x80000000      /* memory seen from PCI bus */
#else
#  define PCI_IO_ADRS		0x00000000      /* base of PCI I/O address */
#  define PCI_MEM_ADRS		CPU_PCI_MEM_ADRS  /* base of PCI MEM address */
#  define PCI2DRAM_BASE_ADRS	0x00000000      /* memory seen from PCI bus */
#endif /* Extended VME config */

/*
 * Primary PCI bus configuration space address and data register addresses
 * as seen by the CPU on the local bus.
 */

#ifndef EXTENDED_VME
#  define PCI_PRIMARY_CAR	0x80000CF8 /* PCI config address register */
#  define PCI_PRIMARY_CDR	0x80000CFC /* PCI config data    register */
#else
#  define PCI_PRIMARY_CAR	0xFE000CF8 /* PCI config address register */
#  define PCI_PRIMARY_CDR	0xFE000CFC /* PCI config data    register */
#endif /* Extended VME config */


/*
 * PCI Config Space device addresses based on their device number
 *
 * Bit 32 is set to enable CONFIG_DATA accesses to PCI Cycles
 */

#define CNFG_START_SEARCH	0x5800     /* PCI Space starting offset */
#define CNFG_RAVEN_ADRS		CPU_PCI_ISA_IO_ADRS /* Raven PCI and MPIC ASIC*/
#define CNFG_IBC_ADRS		0x80005800 /* IBC */
#define CNFG_SCSI_ADRS		0x80006000 /* SCSI */
#define CNFG_UNIVERSE_ADRS	0x80006800 /* VMEbus Bridge */
#define CNFG_LN_ADRS		0x80007000 /* Ethernet Device */
#define CNFG_VGA_ADRS		0x80007800 /* Graphics Device */
#define CNFG_PMC1_ADRS		0x80008000 /* PMC Slot1 */
#define CNFG_SCSI2_ADRS		0x80008800 /* Secondary SCSI */
#define CNFG_LN2_ADRS		0x80009000 /* Secondary Ethernet */
#define CNFG_PMC2_ADRS		0x80009800 /* PMC Slot 2 or PCIX */
     
/* Number of PCI devices */

#define NUM_PCI_DEVS		4

/* PCI Device/Vendor IDs */

#define PCI_ID_RAVEN            0x48011057      /* Id for RAVEN ASIC */
#define PCI_ID_IBC              0x056510ad      /* Id for W83C553 PIB */
#define PCI_ID_UNIVERSE         0x000010e3      /* Id for Universe VME chip */
#define PCI_ID_LN_DEC21140      0x00091011      /* Id DEC chip 21140 */
#define PCI_ID_LN_DEC21143      0x00191011      /* Id DEC chip 21143 */
#define PCI_ID_SCSI             0x00031000      /* Id for SYM53C825/75 Chips */
#define PCI_ID_SEC_SCSI         0x000f1000      /* Id for SYM53C810A Chip */
#define PCI_ID_NCR810           0x00011000      /* Id for SYM53C810A Chip */
#define PCI_ID_NCR860           0x00061000      /* Id for SYM53C860 Chip */
#define PCI_ID_NCR825           0x00031000      /* Id for SYM53C825/75 Chips*/
#define PCI_ID_NCR875           0x000f1000      /* Id for SYM53C875 Chip*/
#define PCI_ID_LN_DEC21040      0x00021011      /* Id DEC chip 21040 */
#define PCI_ID_5434             0x00a81013      /* Id for CL-GD534 chip */
#define PCI_ID_5436             0x00ac1013      /* Id for CL-GD536 chip */
#define PCI_ID_BR_DEC21150      0x00221011      /* Id DEC 21150 PCI bridge */

#define PCI_ID_PRI_LAN		PCI_ID_LN_DEC21140 /* Id for Primary LAN */
#define PCI_ID_SEC_LAN		PCI_ID_LN_DEC21040 /* Id for Secondary LAN */

/* PCI Space Definitions  -- For configuring the RAVEN */

/* CPU to PCI definitions */

#define CPU2PCI_MSATT_MEM	0x00c2
#define CPU2PCI_MSATT_IO	0x00c0

#ifndef EXTENDED_VME

/* STANDARD ( PREP ) mapping of PCI space */

/* 
 *                      MAP FOR STANDARD vxWorks 
 * This maps: VME RANGE:     0x08000000 - (0x08000000 + VME_A32_SLV_SIZE - 1)
 *        to: PCI RANGE:     0x80000000 - 0x80000000 + VME_A32_SLV_SIZE-1
 *        to: MPU RANGE:     0x00000000 - VME_A32_SLV_SIZE-1
 */

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
#define CPU2PCI_ADDR2_START_VAL	CPU_PCI_MEM_UPPER_ADRS
#define CPU2PCI_ADDR2_END_VAL	0xfcff
#define CPU2PCI_OFFSET2_VAL	((0x0-CPU2PCI_ADDR2_START_VAL) & 0xffff)

/* setup address space 3 for PCI I/O space */
#define CPU2PCI_ADDR3_START_VAL	CPU_PCI_IO_UPPER_ADRS
#define CPU2PCI_ADDR3_END_VAL	0xbf7f
#define CPU2PCI_OFFSET3_VAL	((0x0-CPU2PCI_ADDR3_START_VAL) & 0xffff)

#else

/* EXTENDED VME PCI mapping */

/*
 *                      MAP FOR EXTENDED VME vxWorks
 * This maps: VME RANGE:     0x08000000 - (0x08000000 + VME_A32_SLV_SIZE - 1)
 *        to: PCI RANGE:     0x00000000 - VME_A32_SLV_SIZE-1
 *        to: MPU RANGE:     0x00000000 - VME_A32_SLV_SIZE-1
 */

/* setup address space 0 for PCI MEM space -- maps VME address space */
#define CPU2PCI_ADDR0_START_VAL	0x4000
#define CPU2PCI_ADDR0_END_VAL	0xfbff
#define CPU2PCI_OFFSET0_VAL	0x0

/* setup address space 1 for PCI MEM, for MPIC regs */
#define CPU2PCI_ADDR1_START_VAL	(MPIC_BASE_ADRS >>16)
#define CPU2PCI_ADDR1_END_VAL	(((MPIC_BASE_ADRS + 0x00ffffff) \
                                  >> 16) & 0x0000ffff)
#define CPU2PCI_OFFSET1_VAL	0x0
#define CPU2PCI_MSATT1_VAL	CPU2PCI_MSATT_MEM

/* setup address space 2 for PCI MEM -- maps reg. space */
#define CPU2PCI_ADDR2_START_VAL	(CPU_PCI_MEM_ADRS >>16)
#define CPU2PCI_ADDR2_END_VAL	(((CPU_PCI_MEM_ADRS + 0x00ffffff) \
                                  >> 16) & 0x0000ffff)
#define CPU2PCI_OFFSET2_VAL	0x0

/* setup address space 3 for PCI I/O */
#define CPU2PCI_ADDR3_START_VAL	(CPU_PCI_IO_ADRS >>16)
#define CPU2PCI_ADDR3_END_VAL	(((CPU_PCI_IO_ADRS + 0x007fffff) \
                                  >> 16) & 0x0000ffff)
#define CPU2PCI_OFFSET3_VAL	((0x0-CPU2PCI_ADDR3_START_VAL) & 0xffff)

#endif

/* defines that are used in romInit.s */

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

/* Raven PCI Slave Window definitions */

#define PCI2CPU_ADDR0_START     (PCI2DRAM_BASE_ADRS & 0xffff0000)
# define PCI2CPU_ADDR0_END       ((PCI2DRAM_BASE_ADRS + DRAM_SIZE \
                                  - 0x10001) >> 16)
#define PCI2CPU_ADDR0_RANGE     (PCI2CPU_ADDR0_START | PCI2CPU_ADDR0_END)
#define PCI2CPU_OFFSET0         (((0x0-PCI2DRAM_BASE_ADRS)>>16) & 0x0000ffff)
#define PCI2CPU_ATT0            0xf2

#define PCI2CPU_ADDR1_START     ((PCI2DRAM_BASE_ADRS + DRAM_SIZE \
                                  - 0x10000) & 0xffff0000)
#define PCI2CPU_ADDR1_END       ((PCI2DRAM_BASE_ADRS + DRAM_SIZE \
                                  - 0x10000) >> 16)
#define PCI2CPU_ADDR1_RANGE     (PCI2CPU_ADDR1_START | PCI2CPU_ADDR1_END)
#define PCI2CPU_OFFSET1         (((0x0-PCI2DRAM_BASE_ADRS)>>16) & 0x0000ffff)
#define PCI2CPU_ATT1            0xe2

/*
 * Address decoders 2 and 3 are not currently used, so they are
 * set to point to an address that is not used on the PCI bus
 */
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

#define PCI_IO_SCSI_ADRS	(PCI_IO_ADRS + SCSI_DEV_SPACE)	/* 53C825 */
#define PCI_IO_LN_ADRS		(PCI_IO_ADRS + LAN_DEV_SPACE)	/* PCnet */
#define PCI_IO_VGA_ADRS		(PCI_IO_ADRS + VGA_DEV_SPACE)	/* GD5434 */
#define PCI_IO_PMC_ADRS		(PCI_IO_ADRS + PMC_DEV_SPACE)	/* extra */
#define PCI_IO_LN2_ADRS		(PCI_IO_ADRS + LAN2_DEV_SPACE)  /* 21040   */
#define PCI_IO_SCSI_ADRS2	(PCI_IO_ADRS + SCSI_DEV_SPACE2) /* 53C825A */

/* PCI view of PCI Memory Space for PCI devices */

#define PCI_MEM_UNIVERSE_ADRS    (PCI_MEM_ADRS + VME_DEV_SPACE)	/* UNIVERSE */

#ifndef INCLUDE_MPIC
/* PCI IACK for ISA */

#define ISA_INTR_ACK_REG	(CPU_PCI_IACK_ADRS + 0x1ff0)
#endif /* INCLUDE_MPIC */

/* Allocated base address of HW devices as seen from CPU */

#define SCSI_BASE_ADRS		( CPU_PCI_IO_ADRS + SCSI_DEV_SPACE )
#define LAN_BASE_ADRS		( CPU_PCI_IO_ADRS + LAN_DEV_SPACE )
#define UNIVERSE_BASE_ADRS	( CPU_PCI_MEM_ADRS + VME_DEV_SPACE )
#define LAN2_BASE_ADRS		( CPU_PCI_IO_ADRS + LAN2_DEV_SPACE )
#define SCSI_BASE_ADRS2		( CPU_PCI_IO_ADRS + SCSI_DEV_SPACE2 )

/* pc87303 ISA super IO device (ISASIO) keybrd, serial, Parallel port */

#define pc87303_KBD_CTRL	(CPU_PCI_ISA_IO_ADRS + 0x0064)	/* keyboard */
#define pc87303_INDX_REG	(CPU_PCI_ISA_IO_ADRS + 0x0398)	/* index reg */
#define pc87303_DATA_REG	(CPU_PCI_ISA_IO_ADRS + 0x0399)	/* data reg */
#define pc87303_PP		(CPU_PCI_ISA_IO_ADRS + 0x03bc)	/* parallel */
#define pc87303_COM1		(CPU_PCI_ISA_IO_ADRS + 0x03f8)	/* serial 1 */
#define pc87303_COM2		(CPU_PCI_ISA_IO_ADRS + 0x02f8)	/* serial 2 */
#define pc87303_FDC		(CPU_PCI_ISA_IO_ADRS + 0x03f0)	/* floppy */

/* z85230 synchronous & Asynchronous serial communications chip */

#define z85230_PORTB_CTRL	(CPU_PCI_ISA_IO_ADRS + 0x0840)	/* serial 4 */
#define z85230_PORTB_DATA	(CPU_PCI_ISA_IO_ADRS + 0x0841)	
#define z85230_PORTA_CTRL	(CPU_PCI_ISA_IO_ADRS + 0x0842)	/* serial 3 */
#define z85230_PORTA_DATA	(CPU_PCI_ISA_IO_ADRS + 0x0843)

/* z8536 aux timer and I/O chip */

#define	z8536_PORTC_DATA	(CPU_PCI_ISA_IO_ADRS + 0x0844)
#define	z8536_PORTB_DATA	(CPU_PCI_ISA_IO_ADRS + 0x0845)
#define	z8536_PORTA_DATA	(CPU_PCI_ISA_IO_ADRS + 0x0846)
#define	z8536_PORT_CTRL		(CPU_PCI_ISA_IO_ADRS + 0x0847)
#define ZCIO_CNTRL_ADRS		(UINT8 *)(CPU_PCI_ISA_IO_ADRS + 0x847)
#define ZCIO_IACK_ADRS		(UINT8 *)(CPU_PCI_ISA_IO_ADRS + 0x84F)

/* m48TXX non volatile ram, RTC and watchdog timer */

#define m48TXX_LSB_REG		(CPU_PCI_ISA_IO_ADRS + 0x0074)
#define	m48TXX_MSB_REG		(CPU_PCI_ISA_IO_ADRS + 0x0075)
#define m48TXX_DAT_REG		(CPU_PCI_ISA_IO_ADRS + 0x0077)

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

/* L2CR register (MPC750 - Arthur) */

#define MPC750_L2CR_E           0x80000000
#define MPC750_L2CR_256K        0x10000000
#define MPC750_L2CR_512K        0x20000000
#define MPC750_L2CR_1024K       0x30000000
#define MPC750_L2CR_I           0x00200000
#define MPC750_L2CR_SL          0x00008000
#define MPC750_L2CR_IP          0x00000001

/* System Configuration register */

#define	MV2600_GCR		((unsigned int *)(FALCON_BASE_ADRS + 0x08))
#define	MV2600_GCR_MSK		0x0000011e
#define	MV2600_GCR_FREF		0x00000008
#define MV2600_GCR_DRAM_70ns	0x00000000
#define MV2600_GCR_DRAM_60ns 	0x00000002
#define MV2600_GCR_DRAM_50ns 	0x00000006


#define MV2600_CCR_		(FALCON_BASE_ADRS + 0x400)
#define MV2600_CCR		((unsigned int *)(FALCON_BASE_ADRS + 0x400))

#define MV2600_CCR_ID_MSK	0xff000000	/* System ID mask */
#define MV2600_CCR_GEN2x	0xfe000000	/* genesis 2.x cpu type */
#define MV2600_CCR_2300         0xfd000000      /* hummingbird board type */

#define MV2600_CCR_CLK_MSK	0x00f00000	/* Bus clock Mask */
#define MV2600_CCR_CPU_CLK_66	0x00f00000 /* cpu external Bus clock 66 Mhz */
#define MV2600_CCR_CPU_CLK_60	0x00e00000 /* cpu external Bus clock 60 Mhz */
#define MV2600_CCR_CPU_CLK_50	0x00d00000 /* cpu external Bus clock 50 Mhz */

#define MV2600_CCR_SYSXC_MSK	0x000f0000	/* lookaside l2 cache mask */
#define MV2600_CCR_SYSXC_256	0x000e0000	/* lookaside 256kb L2 cache */
#define MV2600_CCR_SYSXC_512	0x000d0000	/* lookaside 512kb L2 cache */
#define MV2600_CCR_SYSXC_1024	0x000c0000	/* lookaside 1Mb L2 cache */
#define MV2600_CCR_SYSXC_NC	0x000f0000	/* lookaside no cache */

#define MV2600_CCR_P0STAT_MSK	0x0000f000	/* in-line l2 cache mask */
#define MV2600_CCR_P0STAT_256	0x00006000	/* in-line 256kb L2 cache */
#define MV2600_CCR_P0STAT_512	0x00005000	/* in-line 512kb L2 cache */
#define MV2600_CCR_P0STAT_1024	0x00004000	/* in-line 1Mb L2 cache */
#define MV2600_CCR_P0STAT_NC	0x00007000	/* in-line no cache */

/* DRAM configuration registers */

#define MV2600_MCR_		(FALCON_BASE_ADRS + 0x404)
#define MV2600_MCR		((unsigned int *)(FALCON_BASE_ADRS + 0x404))
#define MV2600_MCR_L2TYPE_MSK   0x0000F000
#define MV2600_MCR_L2TYPE_LWP   0x00000000
#define MV2600_MCR_L2TYPE_BP    0x00001000
#define MV2600_MCR_L2TYPE_LWNP  0x00002000
#define MV2600_MCR_L2TYPE_BNP   0x00003000
#define MV2600_MCR_L2PLL_MSK    0x00000F00

#define MV2600_MCR_MSK		0x03000000
#define MV2600_MCR_DRAM_50ns	0x03000000
#define MV2600_MCR_DRAM_60ns	0x01000000
#define MV2600_MCR_DRAM_70ns	0x00000000

#define DRAM_70ns		0x0
#define DRAM_60ns 		0x02
#define DRAM_50ns 		0x06

#define DRAM_REG_SIZE           (FALCON_BASE_ADRS + 0x10)
#define DRAM_REG_BASE           (FALCON_BASE_ADRS + 0x18)

/*
 * Base Module Feature Register
 *
 * The bits in this register use negative logic, i.e., a feature is present
 * when its corresponding bit is cleared, not set.  Not all bits are used
 * by all base modules; each base module has its own set of features.
 */

#define MV2600_CCR_OLD		((char *)(CPU_PCI_ISA_IO_ADRS + 0x0800))
#define MV2600_BMFR		((char *)(CPU_PCI_ISA_IO_ADRS + 0x0802))

#define MV2600_BMFR_GIOP	0x80	/* genio Module */
#define MV2600_BMFR_SCCP	0x40	/* z85230 sync serial Port */
#define MV2600_BMFR_PMC2P       0x20    /* pmc #2 Present (MVME2300) */
#define	MV2600_BMFR_PMCP	0x10	/* pmc Present */
#define MV2600_BMFR_VMEP	0x08	/* vme bus Present */
#define MV2600_BMFR_GFXP	0x04	/* graphics Present */
#define MV2600_BMFR_LANP	0x02	/* ethernet Present */
#define MV2600_BMFR_SCIP	0x01	/* scsi present */

#ifdef MV2300
# define MV2600_BMFR_BRGP	0x80	/* PMC Span (Bridge) present */
#else
# define MV2600_BMFR_BRGP	0x20	/* PMC Span (Bridge) present */
#endif /* MV2300 */

#define DEVICE_PRESENT(x) 	(!((*MV2600_BMFR) & (x)))

/* Base Module Status Register */

#define	MV2600_BMSR		((char *)(CPU_PCI_ISA_IO_ADRS + 0x0803))

#define MV2600_BMSR_2300	0xf9    /* Hummingbird */
#define MV2600_BMSR_2300_SC	0xfa    /* mv2300SC (has 21143) */
#define MV2600_BMSR_SG712	0xfb	/* Slim Gen with 712 */
#define	MV2600_BMSR_SG761	0xfc	/* Slim Gen with 761 */
#define MV2600_BMSR_FG712	0xfd	/* Full Gen with 712 */
#define MV2600_BMSR_FG761	0xfe	/* Full Gen with 761 */
#define MV2600_BMSR_1600	0xff	/* MV1600 */

/* Assembly define for L2 cache */

#define MV2600_SXCCR_A		(FALCON_BASE_ADRS + 0x8000)

/* defines for L2 cache routines */

#define MV2600_SXCCR		((unsigned char *)(FALCON_BASE_ADRS + 0x8000))

#define L2_DISABLE		0x80
#define L2_RESET		0x40
#define L2_ENABLE		0x80
#define L2_FLUSH		0x10
#define L2_END_FLUSH		0x10
#define L2_FLUSH_LOOP		4100

/* z8536 I/O port bit mapping */

#define	z8536_PORTA_BRDFAIL	0x40
#define	z8536_PORTB_FUSE	0x40
#define	z8536_PORTB_ABORT	0x80

/*
 * Raven Extensions to Standard PCI Header
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

#define PCI_CFG_RAVEN_PSADD0	0x80
#define PCI_CFG_RAVEN_PSATT0	0x84
#define PCI_CFG_RAVEN_PSOFF0	0x86
#define PCI_CFG_RAVEN_PSADD1	0x88
#define PCI_CFG_RAVEN_PSATT1	0x8c
#define PCI_CFG_RAVEN_PSOFF1	0x8e
#define PCI_CFG_RAVEN_PSADD2	0x90
#define PCI_CFG_RAVEN_PSATT2	0x94
#define PCI_CFG_RAVEN_PSOFF2	0x96
#define PCI_CFG_RAVEN_PSADD3	0x98
#define PCI_CFG_RAVEN_PSATT3	0x9c
#define PCI_CFG_RAVEN_PSOFF3	0x9e

/* Raven MPC registers */

#define RAVEN_MPC_VENID		0x00
#define RAVEN_MPC_DEVID		0x02
#define RAVEN_MPC_REVID		0x05
#define RAVEN_MPC_GCSR		0x08
#define RAVEN_MPC_FEAT		0x0a
#define RAVEN_MPC_MARB		0x0e
#define RAVEN_MPC_PADJ		0x13
#define RAVEN_MPC_MEREN		0x22
#define RAVEN_MPC_MERST		0x27
#define RAVEN_MPC_MERAD		0x28
#define RAVEN_MPC_MERAT		0x2e
#define RAVEN_MPC_PIACK		0x30
#define RAVEN_MPC_MSADD0	0x40
#define RAVEN_MPC_MSOFF0	0x44
#define RAVEN_MPC_MSATT0	0x47
#define RAVEN_MPC_MSADD1	0x48
#define RAVEN_MPC_MSOFF1	0x4c
#define RAVEN_MPC_MSATT1	0x4f
#define RAVEN_MPC_MSADD2	0x50
#define RAVEN_MPC_MSOFF2	0x54
#define RAVEN_MPC_MSATT2	0x57
#define RAVEN_MPC_MSADD3	0x58
#define RAVEN_MPC_MSOFF3	0x5c
#define RAVEN_MPC_MSATT3	0x5f
#define RAVEN_MPC_WDT1CNTL      0x60
#define RAVEN_MPC_WDT2CNTL      0x68
#define RAVEN_MPC_GPREG0_U	0x70
#define RAVEN_MPC_GPREG0_L	0x74
#define RAVEN_MPC_GPREG1_U	0x78
#define RAVEN_MPC_GPREG1_L	0x7c

/*
 * Raven register bit masks
 *
 * Bits marked with 'C' indicate conditions which can be cleared by
 * writing a 1 to the bits.
 */

/* Raven MPC Error Enable (MEREN) register bit masks */

#define RAVEN_MPC_MEREN_RTAI	0x0001	/* PCI mstr Recvd Target Abort Int */
#define RAVEN_MPC_MEREN_SMAI	0x0002	/* PCI mstr Signld Target Abort Int */
#define RAVEN_MPC_MEREN_SERRI	0x0004	/* PCI System Error Int */
#define RAVEN_MPC_MEREN_PERRI	0x0008	/* PCI Parity Error Int */
#define RAVEN_MPC_MEREN_MATOI	0x0020	/* MPC Address Bus Time-out Int */
#define RAVEN_MPC_MEREN_RTAM	0x0100	/* RTAI machine check enable */
#define RAVEN_MPC_MEREN_SMAM	0x0200	/* SMAI machine check enable */
#define RAVEN_MPC_MEREN_SERRM	0x0400	/* SERRI machine check enable */
#define RAVEN_MPC_MEREN_PERRM	0x0800	/* PERRI machine check enable */
#define RAVEN_MPC_MEREN_MATOM	0x2000	/* MATOI machine check enable */
#define RAVEN_MPC_MEREN_DFLT	0x4000	/* Default MPC Master ID select */
#define RAVEN_MPC_MEREN_VALID	0x6F2F	/* Mask for valid MEREN bits */

/* Raven MPC Error Status (MERST) register bit masks */

#define RAVEN_MPC_MERST_RTA	0x01	/* C PCI mstr Recvd Target Abort */
#define RAVEN_MPC_MERST_SMA	0x02	/* C PCI mstr Signld Target Abort */
#define RAVEN_MPC_MERST_SERR	0x04	/* C PCI System Error */
#define RAVEN_MPC_MERST_PERR	0x08	/* C PCI Parity Error */
#define RAVEN_MPC_MERST_MATO	0x20	/* C MPC Address Bus Time-out */
#define RAVEN_MPC_MERST_OVF	0x80	/* C Error Status Overflow */
#define RAVEN_MPC_MERST_VALID	0xAF	/*   Mask for valid MERST bits */
#define RAVEN_MPC_MERST_CLR	0xAF	/*   Clears all errors */

/* Raven PCI Configuration Status register bit masks */

#define RAVEN_PCI_CFG_STATUS_FAST    0x0010  /*   Fast back-to-back capable */
#define RAVEN_PCI_CFG_STATUS_DPAR    0x0100  /* C Data prity error detected */
#define RAVEN_PCI_CFG_STATUS_SELTIM0 0x0200  /*   Device select timing bit 0 */
#define RAVEN_PCI_CFG_STATUS_SELTIM1 0x0400  /*   Device select timing bit 1 */
#define RAVEN_PCI_CFG_STATUS_SIGTA   0x0800  /* C Signalled Target Abort */
#define RAVEN_PCI_CFG_STATUS_RCVTA   0x1000  /* C Received Target Abort */
#define RAVEN_PCI_CFG_STATUS_RCVMA   0x2000  /* C Received Master Abort */
#define RAVEN_PCI_CFG_STATUS_SIGSE   0x4000  /* C Signalled System Error */
#define RAVEN_PCI_CFG_STATUS_RCVPE   0x8000  /* C Detected Parity Error */
#define RAVEN_PCI_CFG_STATUS_VALID   0xFF10  /*   Valid status bits */
#define RAVEN_PCI_CFG_STATUS_CLR     0xF900  /*   Clears all conditions */

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
#  define COM1_BASE_ADR		pc87303_COM1	/* serial port 1 */
#  define COM2_BASE_ADR		pc87303_COM2	/* serial port 2 */
#  define UART_REG_ADDR_INTERVAL  1       	/* addr diff of adjacent regs */
#  define N_UART_CHANNELS 	2		/* No. serial I/O channels */
#endif /* INCLUDE_I8250_SIO */

#ifdef INCLUDE_Z85230_SIO
#  define BAUD_CLK_FREQ		10000000        /* 10 MHz "P Clock" (fixed) */
#  define REG_8530_WRITE(reg,val) sysOutByte((UINT32)(reg), (UINT8)(val))
#  define REG_8530_READ(reg,pVal) *(UINT8 *)pVal = sysInByte((UINT32)reg)
#  define DATA_REG_8530_DIRECT
/* #  define Z8530_RESET_DELAY_COUNT 2000 */
#define Z8530_RESET_DELAY \
        { \
        int i; \
        for (i = 0; i < Z8530_RESET_DELAY_COUNT; i++) \
                ; /* do nothing */ \
        }
#endif /* INCLUDE_Z85230_SIO */

/* total number of serial ports */

#if defined(INCLUDE_I8250_SIO) && defined(INCLUDE_Z85230_SIO)
#  define N_SIO_CHANNELS	4		/* No. serial I/O channels */
#elif defined(INCLUDE_I8250_SIO)
#  define N_SIO_CHANNELS	2		/* No. serial I/O channels */
#elif defined(INCLUDE_Z85230_SIO)
#  define N_SIO_CHANNELS	2		/* No. serial I/O channels */
#else
#  define N_SIO_CHANNELS	0		/* No. serial I/O channels */
#endif

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

/* ncr810/ncr825 delay loop count */

#define NCR810_DELAY_MULT       10


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
#  define VAL_LSI0_BS_VALUE	(CPU_VME_WINDOW_REG_BASE - CPU_PCI_ISA_MEM_ADRS)
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
#  define VAL_LSI2_BS_VALUE    (VME_A24_MSTR_LOCAL - CPU_PCI_ISA_MEM_ADRS)
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
#  define VAL_LSI1_BS_VALUE	(VME_A32_MSTR_LOCAL - CPU_PCI_ISA_MEM_ADRS)
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
#define VAL_LSI3_BS_VALUE	(VME_A16_MSTR_LOCAL - CPU_PCI_ISA_MEM_ADRS)
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

#define VAL_VSI1_TO	(PCI2DRAM_BASE_ADRS - VAL_VSI1_BS + VME_A32_SLV_LOCAL)
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
#define VAL_VSI4_TO	(PCI2DRAM_BASE_ADRS - VAL_VSI1_BS + VME_A32_SLV_LOCAL)
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
#define VAL_VSI5_TO	(PCI2DRAM_BASE_ADRS - VAL_VSI1_BS + VME_A32_SLV_LOCAL)
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

#define CPU_VME_HW_REGS_BASE	( CPU_PCI_ISA_IO_ADRS + 0x1000 )
#define CPU_SIG_LM_CONTROL_REG	( CPU_PCI_ISA_IO_ADRS + 0x1000 )
#define CPU_SIG_LM_STATUS_REG	( CPU_PCI_ISA_IO_ADRS + 0x1001 )
#define CPU_VME_LM_UBA		( CPU_PCI_ISA_IO_ADRS + 0x1002 )
#define CPU_VME_LM_LBA		( CPU_PCI_ISA_IO_ADRS + 0x1003 )
#define CPU_VME_SEM_REG1	( CPU_PCI_ISA_IO_ADRS + 0x1004 )
#define CPU_VME_SEM_REG2	( CPU_PCI_ISA_IO_ADRS + 0x1005 )
#define CPU_VME_GEOG_STAT	( CPU_PCI_ISA_IO_ADRS + 0x1006 )
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
 
/* PCI interrupt levels */
 
/* Falcon-ECC error interrupt */
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

/* Timer Interrupt (IPI0) */

#define TIMER0_INT_LVL            ( 0x00 + TIMER_INTERRUPT_BASE )

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

#define TIMER0_INT_VEC		INT_VEC_IRQ0 + TIMER0_INT_LVL

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

#define MV2600_SIOP_HW_REGS    {0,0,0,0,0,1,0,0,0,0,0}

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

#define SEC_CLR_STATUS             0xFFFF0000
#define SEC_CACHE_LINE_SIZE        (0x08)
#define SEC_PRIM_LATENCY           0
#define SEC_IO_SPACE_BASE_ADRS     (0x01)
#define SEC_IO_SPACE_LIMIT_ADRS    (0xF1 << 8)
#define SEC_MEM_SPACE_BASE_ADRS    ((PCI_MEM_ADRS + PMC_SPAN_MEM_BASE) >> 16)
#define SEC_MEM_SPACE_LIMIT_ADRS   ((PCI_MEM_ADRS + PMC_SPAN_MEM_BASE + \
				     PMC_SPAN_MEM_SIZE - 1) & 0xFFFF)
#define SEC_MEM_MAP_IO_BASE_ADRS   (0x7F00)
#define SEC_MEM_MAP_IO_LIMIT_ADRS  (0x7000 << 16)
#define	SEC_IO_32SPACE_BASE_ADRS   ((PCI_IO_ADRS + PMC_SPAN_IO_BASE) >> 16)
#define	SEC_IO_32SPACE_LIMIT_ADRS  ((PCI_IO_ADRS + PMC_SPAN_IO_BASE + \
				     PMC_SPAN_IO_SIZE - 1) & 0xFFFF)
#define SEC_ISA_BUS                (0x0000 << 16) /* 0 = none, 4 = ISA */
#define SEC_PMC_DISABLE            (0x00)
#define SEC_PMC_ENABLE             (0x07)


#ifdef __cplusplus
}
#endif

#endif /* INCmv260xh */
