/* prpmc800.h - Motorola PrPMC800 board header */

/* Copyright 1984-2001 Wind River Systems, Inc. */
/* Copyright 1999-2001 Motorola, Inc. All Rights Reserved */

/*
modification history
--------------------
01t,31jan02,kab  Remove obsolete vmxExcLoad
01s,16nov01,scb  Harrier outbound 2 maps local PCI at address 0.
01r,15nov01,srr  Updated for Tornado 2.1.
01q,15oct01,scb  Mods to integrate WRS fei driver in lieu of i82559 driver.
01p,10oct01,scb  Shared memory support.
01o,10aug01,srr  Added PrPMC-G carrier board support.
01n,18jun01,srr  Added Harrier DMA support.
01m,08jun01,srr  Remove unused macros.
01l,02may01,bvc  Corrected LN2_INT_LVL to 0x0c
01k,07dec00,krp  Added support for Watchdog Timer
01j,17nov00,dmw  Added slave Ethernet support.
01i,14nov00,dmw  Added slave to Monarch inbound translation.
01h,31oct00,krp  Corrected NvRam Address and size to SROM.
01g,16oct00,dmw  Corrected outbound translation addresses.
01f,12oct00,dmw  Moved ISA_MSTR_IO_LOCAL to 0xFD000000.
01e,09oct00,dmw  Moved ISA_MSTR_IO_LOCAL to 0xFE000000.
01d,08oct00,dmw  Modified memory map for PowerPlusIII.
01c,09sep00,dmw  Updated the number of UARTS/SIO drivers for PrPMC800XT.
01b,09sep00,dmw  Added define for dual-address I2C EEPROM accesses.
01a,31jul00,dmw  Written (from version 01m of mcpn765/mcpn765.h).
*/

/*
This file contains I/O addresses and related constants for the
Motorola PowerPlusIII PrPMC800 board.
*/

#ifndef	INCprpmc800h
#define	INCprpmc800h

#ifdef __cplusplus
    extern "C" {
#endif /* __cplusplus */

/* ATA/EIDE support */

#include "drv/hdisk/ataDrv.h"

#include "harrier.h"

/*
 * Carrier boards available for selection:
 * PRPMC_BASE (default), PRPMC_CARRIER_1, PRPMC_G
 */

#define PRPMC_BASE		0	/* default */
#define PRPMC_CARRIER_1		1
#define PRPMC_G			2

/* Harrier Support */

#define HARRIER_MAX_DEV        1	/* Maximun number of Harriers */

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

/* Max HID0 bit definitions */

#define _PPC_HID0_NOPDST       0x2	/* Nop dst, dstt, dstst, &dststt*/
#define _PPC_HID0_NOPTI        0x1	/* Nop dcbt and dbtst */


#define INCLUDE_PCI		/* always include pci */

/* Boot Line parameters are stored in the 2nd 256 byte block */

#define NV_RAM_SIZE    		USR_SROM_SIZE	
#define NV_RAM_ADRS    		USR_BRD_EEPROM_ADRS
#define NV_RAM_INTRVL           1

/* PCI I/O function defines */

#define INT_NUM_IRQ0            INT_VEC_IRQ0

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

/* Common I/O synchronizing instructions */

#ifndef EIEIO_SYNC
#   define EIEIO_SYNC  WRS_ASM ("eieio;sync")
#endif	/* EIEIO_SYNC */
#ifndef EIEIO
#   define EIEIO    WRS_ASM ("eieio")
#endif	/* EIEIO */

/* Translation macro */

#define TRANSLATE(x,y,z)\
        ((UINT)(x) - (UINT)(y) + (UINT)(z))

/* Legacy ISA space size. Reserved for kybd, com1, com2,... */

#define ISA_LEGACY_SIZE		0x00004000

#define ISA_MSTR_IO_SIZE	0x00010000	/* 64 kbytes (includes legacy)*/

#define ISA_MSTR_IO_BUS		0x00000000	/* must be zero */

/* ISA I/O space within PCI I/O space (includes ISA legacy space) */

#define ISA_MSTR_IO_LOCAL	0xFD000000  /* base of ISA I/O space */

/* Access to PCI nonprefetchable memory space (including ISA memory space)*/

#define PCI_MSTR_MEMIO_BUS	PCI_MSTR_MEMIO_LOCAL	/* 1-1 mapping */

/* PCI I/O space (above ISA I/O space) */

/*
 * NOTE: (PCI_MSTR_IO_LOCAL+PCI_MSTR_IO_SIZE) must be less than
 *        PCI_MSTR_MEMIO_LOCAL to prevent overlap.
 */

#define PCI_MSTR_IO_LOCAL	(ISA_MSTR_IO_LOCAL+ISA_MSTR_IO_SIZE)
#define PCI_MSTR_IO_BUS		(TRANSLATE(PCI_MSTR_IO_LOCAL,\
				 ISA_MSTR_IO_LOCAL,\
				 ISA_MSTR_IO_BUS))

/* PCI (pre-fetchable) Memory space (above ISA memory space) */

#ifdef MVME5500
#define PCI_MSTR_MEM_LOCAL 0x82810000 /* 0x82800000 ??? */
#else
#define PCI_MSTR_MEM_LOCAL (PCI_MSTR_MEMIO_LOCAL+PCI_MSTR_MEMIO_SIZE)
#endif



#define PCI_MSTR_MEM_BUS	(TRANSLATE(PCI_MSTR_MEM_LOCAL,\
				 PCI_MSTR_MEMIO_LOCAL,\
				 PCI_MSTR_MEMIO_BUS))

/*
 * Local address which maps to PCI address zero, so PrPMC can "see"
 * DRAM on CPU card on which it is mounted.  
 * Note: PCI_MSTR_MEM_LOCAL + PCI_MSTR_MEM_SIZE must be <= ISA_MSTR_IO_LOCAL
 */

#define PCI_MSTR_ZERO_LOCAL PCI_MSTR_MEM_LOCAL + \
			    PCI_MSTR_MEM_SIZE

/* defines for memory */

#define SIZE_32MB           0x02000000
#define SIZE_64MB           0x04000000
#define SIZE_128MB          0x08000000
#define SIZE_256MB          0x10000000
#define SIZE_512MB          0x20000000

#define	FLASH_BASE_ADRS     0xF0000000
#define	FLASH_MEM_SIZE      0x0E000000
#define FLASH_ADRS	    FLASH_BASE_ADRS
#define FLASH_SIZE          0x02000000
#define FLASH_WIDTH         0x10

/* MPIC configuration defines */

#define MPIC_BASE_ADRS      0xFC000000
#define MPIC_REG_SIZE       0x00040000

#define MPIC_PCI_BASE_ADRS MPIC_BASE_ADRS

/* Ethernet buffer alignment for low memory */

#define PPC_PAGE_SIZE       0x0001000
#define ENET_BUF_ALIGN(addr)                         \
        (((UINT32)(addr)+PPC_PAGE_SIZE) & ~(PPC_PAGE_SIZE-1))

/* memory map as seen on the PCI bus */

#define PCI_SLV_MEM_LOCAL   LOCAL_MEM_LOCAL_ADRS

/* memory seen from PCI bus */

#define PCI_SLV_MEM_BUS  (LONGSWAP(*(UINT32 *)HARRIER_INBOUND_TRANSLATION_BASE_ADDRESS_0) &  HARRIER_PCFS_ITBAR_BASE_MASK)

#define PCI_SLV_PMEP_BUS (LONGSWAP(*(UINT32 *)HARRIER_XCSR_MPBAR) & HARRIER_PCFS_MPBAR_BASE_MASK)

/*
 * The following #define is set to indicate that the drawbridge has dynamic
 * downstream translation (translation that cannot be computed until run time).
 * Setting this #define will disable the compile-time downstream 
 * cPCI window size/translation compatibility check.
 */

#define CPCI_DOWNSTREAM_DYNAMIC_TRANSLATION 

#define PCI2DRAM_BASE_ADRS  PCI_SLV_MEM_BUS

/*
 * Primary PCI bus configuration space address and data register addresses
 * as seen by the CPU on the local bus.
 */

#define PCI_PRIMARY_CAR     ISA_MSTR_IO_LOCAL + 0xCF8
#define PCI_PRIMARY_CDR     ISA_MSTR_IO_LOCAL + 0xCFC

/* Special dec21143 configuration device driver area register */

#define PCI_CFG_21143_DA    0x40

/* PCI Device/Vendor IDs */

#define PCI_ID_HARRIER      0x480B1057      /* Id for Harrier PHB */
#define PCI_ID_UNIVERSE     0x000010e3      /* Id for Universe VME chip */
#define PCI_ID_LN_DEC21040  0x00021011      /* Id DEC 21040 */
#define PCI_ID_LN_DEC21140  0x00091011      /* Id DEC 21140 */
#define PCI_ID_LN_DEC21143  0x00191011      /* Id DEC 21143 */
#define PCI_ID_I82559       0x12298086      /* Id for Intel 82559 */
#define PCI_ID_I82559ER     0x12098086      /* Id for Intel 82559 ER */
#define PCI_ID_BR_DEC21554  0x00461011      /* Id DEC 21554 PCI bridge */


/* CPU to PCI definitions */

/* 
 * Outbound translation address space 0 for PCI MEM space.
 * This mapped area starts at PCI_MSTR_MEMIO_LOCAL for PCI_MSTR_MEM_SIZE.
 * PCI_MSTR_MEM_SIZE is 32MB by default and can be extended to 3.75GB minus
 * the PCI_MSTR_MEMIO_LOCAL.   Note that PCI_MSTR_MEMIO_LOCAL must be greater
 * than or equal to the amount of DRAM.  This area has a null offset associated 
 * with it and is marked as prefetchable PCI memory.
 */


/* add PCI access macros */
#define PCI_MEMIO2LOCAL(x) \
     ((int)(x) + PCI_MSTR_MEMIO_LOCAL - PCI_MSTR_MEMIO_BUS)

/* PCI IO memory adrs to CPU (60x bus) adrs */

#define PCI_IO2LOCAL(x) \
     ((int)(x) + PCI_MSTR_IO_LOCAL - PCI_MSTR_IO_BUS)


/* 60x bus adrs to PCI (non-prefetchable) memory address */

#define LOCAL2PCI_MEMIO(x) \
     ((int)(x) + PCI_SLV_MEM_BUS - PCI_SLV_MEM_LOCAL)
#define PCI2LOCAL_MEMIO(x) \
     ((int)(x) - PCI_SLV_MEM_BUS + PCI_SLV_MEM_LOCAL)

/* Cpu PCI adrs to PCI system address */

#define CPCI2PCI(x) \
     ((int)(x) - PCI_SLV_MEM_LOCAL)
#define PCI2CPCI(x) \
     ((int)(x) + PCI_SLV_MEM_LOCAL)


#define CPU2PCI_ADDR0_START     (PCI_MSTR_MEMIO_LOCAL >> 16)
#define CPU2PCI_ADDR0_END       (((PCI_MSTR_MEM_LOCAL + \
                                (PCI_MSTR_MEM_SIZE-1)) >> 16) & 0x0000FFFF)

#define CPU2PCI_OFFSET0         0x00 
#define CPU2PCI_MSATT0          (HARRIER_PCFS_ITAT_RXT | \
                                 HARRIER_PCFS_ITAT_RXS_MASK | \
                                 HARRIER_PCFS_ITAT_RXS_128BYTES | \
                                 HARRIER_PCFS_OTAT_MEM | \
                                 HARRIER_PCFS_OTAT_ENA)

/* Outbound translation address space 1 for PCI I/O space. */

#define CPU2PCI_ADDR1_START     (ISA_MSTR_IO_LOCAL >> 16)
#define CPU2PCI_ADDR1_END       (((ISA_MSTR_IO_LOCAL + ISA_MSTR_IO_SIZE + \
				  PCI_MSTR_IO_SIZE) - 1) >> 16)

#define CPU2PCI_OFFSET1         (ISA_MSTR_IO_BUS - (ISA_MSTR_IO_LOCAL >> 16))
#define CPU2PCI_MSATT1          (HARRIER_PCFS_ITAT_RXT | \
                                 HARRIER_PCFS_ITAT_RXS_128BYTES | \
                                 HARRIER_PCFS_OTAT_ENA)

/* Outbound translation address space 2 maps to local PCI address 0 */

#define CPU2PCI_ADDR2_START     ((PCI_MSTR_ZERO_LOCAL) >> 16)

#define CPU2PCI_ADDR2_END       ( (PCI_MSTR_ZERO_LOCAL + \
				   PCI_MSTR_ZERO_SIZE  - 1) >> 16 )

#define CPU2PCI_OFFSET2         ( (0 - (PCI_MSTR_ZERO_LOCAL)) >> 16 )

#define CPU2PCI_MSATT2          (HARRIER_PCFS_ITAT_RXT | \
                                 HARRIER_PCFS_ITAT_RXS_MASK | \
                                 HARRIER_PCFS_ITAT_RXS_128BYTES | \
                                 HARRIER_PCFS_OTAT_MEM | \
                                 HARRIER_PCFS_OTAT_ENA)

/* 
 * Outbound translation address space 3 for PCI I/O space.
 * This mapped area starts at ISA_MSTR_IO_LOCAL and maps thru I/O memory.
 */

#define CPU2PCI_ADDR3_START     (ISA_MSTR_IO_LOCAL >> 16)
#define CPU2PCI_ADDR3_END       (((ISA_MSTR_IO_LOCAL + ISA_MSTR_IO_SIZE + \
				  PCI_MSTR_IO_SIZE) - 1) >> 16)

#define CPU2PCI_OFFSET3         (ISA_MSTR_IO_BUS - (ISA_MSTR_IO_LOCAL >> 16))
#define CPU2PCI_MSATT3          (HARRIER_PCFS_OTAT_ENA)

/* PCI to CPU definitions (inbound translations) */

#ifdef LOCAL_MEM_AUTOSIZE
#   define DRAM_SIZE		((ULONG)sysPhysMemTop() - LOCAL_MEM_LOCAL_ADRS)
#else /* LOCAL_MEM_AUTOSIZE */
#   define DRAM_SIZE		(LOCAL_MEM_SIZE - LOCAL_MEM_LOCAL_ADRS)
#endif /* LOCAL_MEM_AUTOSIZE */

/* Inbound mapping for low-order DRAM */

#define INBOUND_DRAM_BAR	HARRIER_INBOUND_TRANSLATION_BASE_ADDRESS_0

/* Local base (target) addresses of input mapping */

#define PCI2CPU_0_TARG		0x0
#define PCI2CPU_1_TARG		0x0
#define PCI2CPU_2_TARG		0x0
#define PCI2CPU_3_TARG		0x0

#define PCI2CPU_0_BASE          (PCI2DRAM_BASE_ADRS & \
                                 HARRIER_PCFS_ITBAR_BASE_MASK)
#define PCI2CPU_0_SIZE          (PCI2DRAM_MAP_SIZE) 	/* 16Mb */
#define PCI2CPU_0_OFFSET        (((0x0-PCI2CPU_0_BASE)>>16) & 0x0000FFFF)
#define PCI2CPU_0_ATTR          (HARRIER_ITAT_AWL | \
                                 HARRIER_ITAT_GBL | \
                                 HARRIER_ITAT_RMT | \
                                 HARRIER_ITAT_RMS_256BYTES | \
                                 HARRIER_ITAT_RXT | \
                                 HARRIER_ITAT_RXS_256BYTES | \
                                 HARRIER_ITAT_ENA | \
                                 HARRIER_ITAT_MEM | \
                                 HARRIER_ITAT_WPE | \
                                 HARRIER_ITAT_RAE | \
                                 HARRIER_ITAT_PRE)

#define PCI2CPU_1_BASE          0x00000000
#define PCI2CPU_1_SIZE          0x00
#define PCI2CPU_1_OFFSET        0x0000
#define PCI2CPU_1_ATTR          0x00000000

#define PCI2CPU_2_BASE          0x00000000
#define PCI2CPU_2_SIZE          0x00
#define PCI2CPU_2_OFFSET        0x0000
#define PCI2CPU_2_ATTR          0x00000000

#define PCI2CPU_3_BASE          0x00000000
#define PCI2CPU_3_SIZE          0x00
#define PCI2CPU_3_OFFSET        0x0000
#define PCI2CPU_3_ATTR          0x00000000


/* Harrier PCI Slave Window definitions */

#define PCI2CPU_MEM0_BUS    PCI_SLV_MEM_BUS
#define PCI2CPU_MEM0_LOCAL  PCI_SLV_MEM_LOCAL
#define PCI2CPU_MEM0_SIZE   DRAM_SIZE
#define PCI2CPU_MEM0_ATT        (HARRIER_PCI_SLV_ATTR_REN_MASK  | \
                 HARRIER_PCI_SLV_ATTR_WEN_MASK  | \
                 HARRIER_PCI_SLV_ATTR_WPEN_MASK | \
                 HARRIER_PCI_SLV_ATTR_RAEN_MASK | \
                 HARRIER_PCI_SLV_ATTR_GBL_MASK  | \
                 HARRIER_PCI_SLV_ATTR_INV_MASK)

/* Inbound translation for access to slave's low-order DRAM from PCI mem space */

#define SLAVE_PCI2CPU_0_BASE	(0x00000000)   /* Slave PCI to CPU memory */
#define SLAVE_PCI2CPU_0_SIZE	(0x01000000) /* 16Mb */
#define SLAVE_PCI2CPU_0_ATTR	(HARRIER_ITAT_AWL | \
                                 HARRIER_ITAT_GBL | \
                                 HARRIER_ITAT_RMT | \
                                 HARRIER_ITAT_RMS_256BYTES | \
                                 HARRIER_ITAT_RXT | \
                                 HARRIER_ITAT_RXS_256BYTES | \
                                 HARRIER_ITAT_ENA | \
                                 HARRIER_ITAT_MEM | \
                                 HARRIER_ITAT_WPE | \
                                 HARRIER_ITAT_RAE | \
                                 HARRIER_ITAT_PRE)

/*
 * Address decoders 1, 2 and 3 are not currently used, so they are
 * set to point to an address that is not used on the PCI bus
 */

#define PCI2CPU_MEM1_BUS    0
#define PCI2CPU_MEM1_LOCAL  0
#define PCI2CPU_MEM1_SIZE   0
#define PCI2CPU_MEM1_ATT    0

#define PCI2CPU_MEM2_BUS    0
#define PCI2CPU_MEM2_LOCAL  0
#define PCI2CPU_MEM2_SIZE   0
#define PCI2CPU_MEM2_ATT    0

#define PCI2CPU_MEM3_BUS    0
#define PCI2CPU_MEM3_LOCAL  0
#define PCI2CPU_MEM3_SIZE   0
#define PCI2CPU_MEM3_ATT    0

/* m48TXX non volatile ram, RTC and watchdog timer */

#ifdef INCLUDE_PRPMC800XT
#   ifdef nonXBus
#      define m48TXX_LSB_REG	(ISA_MSTR_IO_LOCAL + 0x0074)
#      define m48TXX_MSB_REG	(ISA_MSTR_IO_LOCAL + 0x0075)
#      define m48TXX_DAT_REG	(ISA_MSTR_IO_LOCAL + 0x0077)
#   endif /* nonXBus */

    /* RTC - Alarm interrupt level - M48T37V */

#   define RTC_ALARM_LVL       ( 0x04 + EXT_INTERRUPT_BASE )
#   define RTC_ALARM_VEC       ( INT_VEC_IRQ0 + RTC_ALARM_LVL )
#endif /* INCLUDE_PRPMC800XT */

/* CPU type */

#define CPU_TYPE            ((vxPvrGet() >> 16) & 0xffff)
#define CPU_REV             (vxPvrGet() & 0xffff)
#define CPU_TYPE_601        0x01	/* PPC 601 CPU */
#define CPU_TYPE_602        0x02	/* PPC 602 CPU */
#define CPU_TYPE_603        0x03	/* PPC 603 CPU */
#define CPU_TYPE_603E       0x06	/* PPC 603e CPU */
#define CPU_TYPE_603P       0x07	/* PPC 603p CPU */
#define CPU_TYPE_750        0x08	/* PPC 750 CPU */
#define CPU_TYPE_604        0x04	/* PPC 604 CPU */
#define CPU_TYPE_604E       0x09	/* PPC 604e CPU */
#define CPU_TYPE_604R       0x0A	/* PPC 604r CPU */
#define CPU_TYPE_MAX        0x0C    /* PPC MAX CPU */
#define CPU_TYPE_NITRO      0x800C	/* PPC NITRO CPU */
#define CPU_REV_NITRO       0x1100	/* PPC Nitro Rev 1.0 CPU */

/* Vital Product Data (VPD) Support */

#define VPD_BRD_EEPROM_ADRS 0xA0    /* i2c address of board's SROM */
#define USR_BRD_EEPROM_ADRS 0xA2    /* i2c address of user config EEPROM */
#define OPT_BASE_BRD_EEPROM 0xA6    /* Optional Base Board Configuration */
#define SPD_EEPROM_ADRS	    0xA8    /* i2c address of first SPD EEPROM */

#if (CARRIER_TYPE == PRPMC_G)
#   define GPIO_I2C_ADRS    0x30    /* i2c address of W83601 GPIO */
#   define W83782_I2C_ADRS  0x50    /* i2c address of W83782 Registers */
#endif /* (CARRIER_TYPE == PRPMC_G) */

#define VPD_BRD_OFFSET      0	    /* offset into brd's eeprom for vpd data */
#define VPD_PKT_LIMIT       25      /* Max number of packets expected */

#define I2C_SINGLE_ADDRESS  1       /* num addr bytes to write for I2C access */
#define I2C_DUAL_ADDRESS    2       /* num addr bytes to write for I2C access */
#define I2C_ADDRESS_BYTES   I2C_DUAL_ADDRESS /* VPD and user ROM accesses */

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
#else /* VPD_ERRORS_NONFATAL */
#   define  DEFAULT_BSP_ERROR_BEHAVIOR  EXIT_TO_SYSTEM_MONITOR
#endif /* VPD_ERRORS_NONFATAL */

/*
 * Bootrom error bits.
 * These bits are set during ROM startup before error annunciation is available
 * to save error conditions for later reporting.
 */

#define BOOTROM_NO_VPD_BUS_SPEED       0x80000000     /* Couldn't read VPD */
#define BOOTROM_DEFAULT_SMC_TIMING     0x40000000     /* Couldn't read SPD */

/* programmable interrupt controller (PIC) */

#define	PIC_REG_ADDR_INTERVAL	1	/* address diff of adjacent regs. */

/* serial ports (COM1 - COM2) */

#ifdef INCLUDE_I8250_SIO
#   define COM1_BASE_ADR	    HARRIER_UART_0_RTDL_REG   /* serial port 1 */
#   define COM2_BASE_ADR	    HARRIER_UART_1_RTDL_REG   /* serial port 2 */
#   define UART_REG_ADDR_INTERVAL   1  	/* addr diff of adjacent regs */
#   ifdef  INCLUDE_PRPMC800XT           /* Extended version of the PrPMC800 */
#      define N_UART_CHANNELS       2   /* No. serial I/O channels */
#   else  /* INCLUDE_PRPMC800XT */
#      define N_UART_CHANNELS       1   /* No. serial I/O channels */
#   endif /* INCLUDE_PRPMC800XT */
#endif /* INCLUDE_I8250_SIO */

/* total number of serial ports */

#ifdef  INCLUDE_PRPMC800XT              /* Extended version of the PrPMC800 */
#   define N_SIO_CHANNELS	2           /* No. serial I/O channels */
#else  /* INCLUDE_PRPMC800XT */
#   define N_SIO_CHANNELS	1           /* No. serial I/O channels */
#endif /* INCLUDE_PRPMC800XT */

/* non-volatile (battery-backed) ram defines
 *
 * the top 16 bytes are used for the RTC registers
 */
 
#define BBRAM_ADRS              PRPMC800_XPORT3_ADDR   /* base address */
#define BBRAM_SIZE              0x8000         /* 32K NVRAM Total Size */
 
/* factory ethernet address */
 
#define BB_ENET                 ((char *)(BBRAM_ADRS + 0x7f2c))
 
/* MK48TXX register settings */
 
/* flag register */
 
#define MK48T_FLAGS             ((char *)(BBRAM_ADRS + 0x7ff0))
 
/* alarm clock registers, 4 1byte locations */
 
#define ALARM_CLOCK             ((char *)(BBRAM_ADRS + 0x7ff2))
 
/* interrupt register */
 
#define MK48T_INTR              ((char *)(BBRAM_ADRS + 0x7ff6))
 
/* watchdog timer register */
 
#define WD_TIMER                ((char *)(BBRAM_ADRS + 0x7ff7))
 
/* MK48TXX bb time of day clk, 8 1byte locations */
 
#define TOD_CLOCK               ((char *)(BBRAM_ADRS + 0x7ff8))
 
/* User configuration SROM */

#define USR_SROM_ADRS	USR_BRD_EEPROM_ADRS
#define USR_SROM_SIZE   256

#define NV_RAM_READ(x)		sysNvRead (x)
#define NV_RAM_WRITE(x,y)	sysNvWrite (x,y)

/* Slave-Mode PRPMC750 related defines */

/*
 * The following define causes the IntPin value presented in the emulated PCI
 * to track the PCI Bus interrupt routing established by the value of
 * PRPMC_SLAVE_BUS_INT_ROUTE in config.h
 */

#define PRPMC_SLAVE_BUS_INT_PIN          (PRPMC_SLAVE_BUS_INT_ROUTE+1)

/*
 * The following define causes the trigger offset to track the value of
 * SM_BUS_INT_LVL. So if SM_BUS_INT_LVL is set to IPI3_INT_LVL, the resulting
 * value of SM_BUS_INT_TRIGGER_OFFSET will be MPIC_CPU1_IPI3_DISP_REG (0x21070).
 */

#define SM_BUS_INT_TRIGGER_OFFSET   (MPIC_CPU1_IPI0_DISP_REG +\
                                     ((SM_BUS_INT_LVL - IPI0_INT_LVL) *\
                          (MPIC_CPU1_IPI1_DISP_REG -\
                                       MPIC_CPU1_IPI0_DISP_REG)))

#define SM_BUS_INT_BIT         1
#define SM_BUS_INT_PRIORITY    PRIORITY_LVL7

/*
 * The following define causes the trigger offset to track the value of
 * SM_MAILBOX_INT_LVL. So if SM_MAILBOX_INT_LVL is set to IPI0_INT_LVL, the
 * resulting value of SM_MAILBOX_INT_OFFSET will be MPIC_CPU0_IPI0_DISP_REG
 * (0x20070).
 */

#define SM_MAILBOX_INT_OFFSET   (MPIC_CPU0_IPI0_DISP_REG +\
                                 ((SM_MAILBOX_INT_LVL - IPI0_INT_LVL) *\
                                  (MPIC_CPU0_IPI1_DISP_REG -\
                                   MPIC_CPU0_IPI0_DISP_REG)))
#define SM_MAILBOX_INT_BIT  0
#define SM_MAILBOX_INT_VEC 	HARR_FUNC_INT_LVL

/* Dec2155x (Drawbridge) related defines */

#define DEC2155X_MAILBOX_INT_VEC (DEC2155X_DOORBELL0_INT_VEC + \
				  DEC2155X_SM_DOORBELL_BIT)

#ifdef INCLUDE_DEC2155X
#   define DEC2155X_BIST_VAL         0x00
#   define DEC2155X_PRI_PRG_IF_VAL   0x00
#   define DEC2155X_PRI_SUBCLASS_VAL 0x20
#   define DEC2155X_PRI_CLASS_VAL    0x0b
#   define DEC2155X_SEC_PRG_IF_VAL   0x00
#   define DEC2155X_SEC_SUBCLASS_VAL 0x80
#   define DEC2155X_SEC_CLASS_VAL    0x06
#   define DEC2155X_MAX_LAT_VAL      0x00
#   define DEC2155X_MIN_GNT_VAL      0xff

#   define DEC2155X_CHP_CTRL0_VAL 0x0000
#   define DEC2155X_CHP_CTRL1_VAL 0x0000

#   define DEC2155X_PRI_SERR_VAL (DEC2155X_SERR_DIS_DLYD_TRNS_MSTR_ABRT | \
			          DEC2155X_SERR_DIS_DLYD_RD_TRNS_TO     | \
			          DEC2155X_SERR_DIS_DLYD_WRT_TRNS_DISC  | \
			          DEC2155X_SERR_DIS_PSTD_WRT_DATA_DISC  | \
			          DEC2155X_SERR_DIS_PSTD_WRT_TRGT_ABRT  | \
			          DEC2155X_SERR_DIS_PSTD_WRT_MSTR_ABRT  | \
			          DEC2155X_SERR_DIS_PSTD_WRT_PAR_ERROR)

#   define DEC2155X_SEC_SERR_VAL (DEC2155X_SERR_DIS_DLYD_TRNS_MSTR_ABRT | \
			          DEC2155X_SERR_DIS_DLYD_RD_TRNS_TO     | \
			          DEC2155X_SERR_DIS_DLYD_WRT_TRNS_DISC  | \
			          DEC2155X_SERR_DIS_PSTD_WRT_DATA_DISC  | \
			          DEC2155X_SERR_DIS_PSTD_WRT_TRGT_ABRT  | \
			          DEC2155X_SERR_DIS_PSTD_WRT_MSTR_ABRT  | \
			          DEC2155X_SERR_DIS_PSTD_WRT_PAR_ERROR)
#endif /* INCLUDE_DEC2155X */

/* Interrupt bases  */

#define ISA_INTERRUPT_BASE       0x00
#define EXT_INTERRUPT_BASE       0x10
#define TIMER_INTERRUPT_BASE     0x20
#define IPI_INTERRUPT_BASE       0x24
#define INTERNAL_INTERRUPT_BASE  0x28
#define	ESCC_INTERRUPT_BASE      0x00

#define DEC2155X_INTERRUPT_BASE 0x60

/* interrupt Level definitions */

#define PIB_INT_LVL         ( 0x00 + EXT_INTERRUPT_BASE )

    /* UART interrupt level is shared by the 2 COM Ports */

#define XPORT_UART_INT_LVL  ( 0x01 + EXT_INTERRUPT_BASE )

    /* Front panel Ethernet interrupt level */

#define LN1_INT_LVL         ( 0x0a + EXT_INTERRUPT_BASE )

    /* P14 carrier board Ethernet interrupt level */

#define LN2_INT_LVL         ( 0x0c + EXT_INTERRUPT_BASE )

    /* Slave Ethernet interrupt level */

#define SLV_LN1_INT_LVL     ( 0x0b + EXT_INTERRUPT_BASE )

    /* Watchdog Timer Level 1 interrupt level */

#define WDTL1_INT_LVL       ( 0x03 + EXT_INTERRUPT_BASE )

    /* Watchdog Timer Level 2 interrupt level */

#define WDTL2_INT_LVL       ( 0x03 + EXT_INTERRUPT_BASE )

    /* Drawbridge interrupt level */

#define DEC2155X_INT_LVL    ( 0x09 + EXT_INTERRUPT_BASE )

    /* Abort interrupt level */

#define ABORT_INT_LVL       ( 0x0e + EXT_INTERRUPT_BASE )

    /* PCI INTA# */

#define PCI_INT_LVL1        ( 0x09 + EXT_INTERRUPT_BASE )

    /* PCI INTB# */

#define PCI_INT_LVL2        ( 0x0a + EXT_INTERRUPT_BASE )

    /* PCI INTC# */

#define PCI_INT_LVL3        ( 0x0b + EXT_INTERRUPT_BASE )

    /* PCI INTD# */

#define PCI_INT_LVL4        ( 0x0c + EXT_INTERRUPT_BASE )

    /* Timer interrupt level (IPI0) */

#define TIMER0_INT_LVL      ( 0x00 + TIMER_INTERRUPT_BASE )

/* Inter-Processor Interrupts */

#define IPI0_INT_LVL        ( 0x00 + IPI_INTERRUPT_BASE )
#define IPI1_INT_LVL        ( 0x01 + IPI_INTERRUPT_BASE )
#define IPI2_INT_LVL        ( 0x02 + IPI_INTERRUPT_BASE )
#define IPI3_INT_LVL        ( 0x03 + IPI_INTERRUPT_BASE )

/* Harrier Internal Functional Interrupts  */

#define HARR_FUNC_INT_LVL   ( 0x00 + INTERNAL_INTERRUPT_BASE )
#define COM1_INT_LVL        HARR_FUNC_INT_LVL
#define COM2_INT_LVL        HARR_FUNC_INT_LVL

/* Harrier Internal Error Interrupts */

#define HARR_ERR_INT_LVL    ( 0x01 + INTERNAL_INTERRUPT_BASE )

#ifdef  INCLUDE_ATA

	/* IDE controller interrupt level */

#   define IDE_CNTRLR0_INT_LVL     ( 0x0e + ISA_INTERRUPT_BASE )
#endif /* INCLUDE_ATA */

/* Drawbridge internal interrupt levels */

#define DEC2155X_DOORBELL0_INT_LVL  ( 0x00 + DEC2155X_INTERRUPT_BASE )
#define DEC2155X_DOORBELL1_INT_LVL  ( 0x01 + DEC2155X_INTERRUPT_BASE )
#define DEC2155X_DOORBELL2_INT_LVL  ( 0x02 + DEC2155X_INTERRUPT_BASE )
#define DEC2155X_DOORBELL3_INT_LVL  ( 0x03 + DEC2155X_INTERRUPT_BASE )
#define DEC2155X_DOORBELL4_INT_LVL  ( 0x04 + DEC2155X_INTERRUPT_BASE )
#define DEC2155X_DOORBELL5_INT_LVL  ( 0x05 + DEC2155X_INTERRUPT_BASE )
#define DEC2155X_DOORBELL6_INT_LVL  ( 0x06 + DEC2155X_INTERRUPT_BASE )
#define DEC2155X_DOORBELL7_INT_LVL  ( 0x07 + DEC2155X_INTERRUPT_BASE )
#define DEC2155X_DOORBELL8_INT_LVL  ( 0x08 + DEC2155X_INTERRUPT_BASE )
#define DEC2155X_DOORBELL9_INT_LVL  ( 0x09 + DEC2155X_INTERRUPT_BASE )
#define DEC2155X_DOORBELL10_INT_LVL ( 0x0a + DEC2155X_INTERRUPT_BASE )
#define DEC2155X_DOORBELL11_INT_LVL ( 0x0b + DEC2155X_INTERRUPT_BASE )
#define DEC2155X_DOORBELL12_INT_LVL ( 0x0c + DEC2155X_INTERRUPT_BASE )
#define DEC2155X_DOORBELL13_INT_LVL ( 0x0d + DEC2155X_INTERRUPT_BASE )
#define DEC2155X_DOORBELL14_INT_LVL ( 0x0e + DEC2155X_INTERRUPT_BASE )
#define DEC2155X_DOORBELL15_INT_LVL ( 0x0f + DEC2155X_INTERRUPT_BASE )
#define DEC2155X_PWR_MGMT_INT_LVL   ( 0x10 + DEC2155X_INTERRUPT_BASE )
#define DEC2155X_I2O_INT_LVL		( 0x11 + DEC2155X_INTERRUPT_BASE )
#define DEC2155X_PG_CRSSNG_INT_LVL	( 0x12 + DEC2155X_INTERRUPT_BASE )

/* interrupt vector definitions */

#define INT_VEC_IRQ0			0x00	/* vector for IRQ0 */

/* PCI/MPIC interrupt vectors */

#define COM1_INT_VEC        ( INT_VEC_IRQ0 + COM1_INT_LVL )
#define COM2_INT_VEC        ( INT_VEC_IRQ0 + COM2_INT_LVL )
#define COM3_INT_VEC        ( INT_VEC_IRQ0 + COM3_INT_LVL )
#define COM4_INT_VEC        ( INT_VEC_IRQ0 + COM4_INT_LVL )
#define WDTL1_INT_VEC       ( INT_VEC_IRQ0 + WDTL1_INT_LVL )
#define WDTL2_INT_VEC       ( INT_VEC_IRQ0 + WDTL2_INT_LVL )
#define	CPCI_INT_VEC        ( INT_VEC_IRQ0 + DEC2155X_INT_LVL )
#define	PCI_INTA_VEC        ( INT_VEC_IRQ0 + PCI_INT_LVL1 )
#define	PCI_INTB_VEC        ( INT_VEC_IRQ0 + PCI_INT_LVL2 )
#define	PCI_INTC_VEC        ( INT_VEC_IRQ0 + PCI_INT_LVL3 )
#define	PCI_INTD_VEC        ( INT_VEC_IRQ0 + PCI_INT_LVL4 )
#define LN1_INT_VEC         ( INT_VEC_IRQ0 + LN1_INT_LVL )
#define LN2_INT_VEC         ( INT_VEC_IRQ0 + LN2_INT_LVL )
#define PIB_INT_VEC         ( INT_VEC_IRQ0 + PIB_INT_LVL )

#ifndef SLAVE_OWNS_ETHERNET
#    define SLV_LN1_INT_VEC     ( INT_VEC_IRQ0 + SLV_LN1_INT_LVL )
#else /* SLAVE_OWNS_ETHERNET */
#    define SLV_LN1_INT_VEC     ( INT_VEC_IRQ0 + LN1_INT_LVL )
#endif /* SLAVE_OWNS_ETHERNET */

/* Internal Harrier interrupt vectors */

#define HARR_FUNC_INT_VEC   ( INT_VEC_IRQ0 + HARR_FUNC_INT_LVL )
#define HARR_ERR_INT_VEC    ( INT_VEC_IRQ0 + HARR_ERR_INT_LVL )


#ifdef       INCLUDE_ATA
#   define IDE_CNTRLR0_INT_VEC      ( INT_VEC_IRQ0 + IDE_CNTRLR0_INT_LVL )
#endif       /* INCLUDE_ATA */

/* Timer interrupt vectors */

#define TIMER0_INT_VEC      ( INT_VEC_IRQ0 + TIMER0_INT_LVL )

/* Drawbridge interrupt vectors */

#define DEC2155X_DOORBELL0_INT_VEC  (INT_VEC_IRQ0 + DEC2155X_DOORBELL0_INT_LVL)
#define DEC2155X_DOORBELL1_INT_VEC  (INT_VEC_IRQ0 + DEC2155X_DOORBELL1_INT_LVL)
#define DEC2155X_DOORBELL2_INT_VEC  (INT_VEC_IRQ0 + DEC2155X_DOORBELL2_INT_LVL)
#define DEC2155X_DOORBELL3_INT_VEC  (INT_VEC_IRQ0 + DEC2155X_DOORBELL3_INT_LVL)
#define DEC2155X_DOORBELL4_INT_VEC  (INT_VEC_IRQ0 + DEC2155X_DOORBELL4_INT_LVL)
#define DEC2155X_DOORBELL5_INT_VEC  (INT_VEC_IRQ0 + DEC2155X_DOORBELL5_INT_LVL)
#define DEC2155X_DOORBELL6_INT_VEC  (INT_VEC_IRQ0 + DEC2155X_DOORBELL6_INT_LVL)
#define DEC2155X_DOORBELL7_INT_VEC  (INT_VEC_IRQ0 + DEC2155X_DOORBELL7_INT_LVL)
#define DEC2155X_DOORBELL8_INT_VEC  (INT_VEC_IRQ0 + DEC2155X_DOORBELL8_INT_LVL)
#define DEC2155X_DOORBELL9_INT_VEC  (INT_VEC_IRQ0 + DEC2155X_DOORBELL9_INT_LVL)
#define DEC2155X_DOORBELL10_INT_VEC (INT_VEC_IRQ0 + DEC2155X_DOORBELL10_INT_LVL)
#define DEC2155X_DOORBELL11_INT_VEC (INT_VEC_IRQ0 + DEC2155X_DOORBELL11_INT_LVL)
#define DEC2155X_DOORBELL12_INT_VEC (INT_VEC_IRQ0 + DEC2155X_DOORBELL12_INT_LVL)
#define DEC2155X_DOORBELL13_INT_VEC (INT_VEC_IRQ0 + DEC2155X_DOORBELL13_INT_LVL)
#define DEC2155X_DOORBELL14_INT_VEC (INT_VEC_IRQ0 + DEC2155X_DOORBELL14_INT_LVL)
#define DEC2155X_DOORBELL15_INT_VEC (INT_VEC_IRQ0 + DEC2155X_DOORBELL15_INT_LVL)
#define DEC2155X_PWR_MGMT_INT_VEC   (INT_VEC_IRQ0 + DEC2155X_PWR_MGMT_INT_LVL)
#define DEC2155X_I2O_INT_VEC        (INT_VEC_IRQ0 + DEC2155X_I2O_INT_LVL)
#define DEC2155X_PG_CRSSNG_INT_VEC  (INT_VEC_IRQ0 + DEC2155X_PG_CRSSNG_INT_LVL)

/*
 * Address range definitions for PCI bus.
 *
 * Used with vxMemProbe() hook sysBusProbe().
 */

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

/*
 * Support for determining if we're ROM based or not.  _sysInit
 * saves the startType parameter at location ROM_BASED_FLAG.
 */

#define PCI_AUTOCONFIG_FLAG_OFFSET ( 0x4c00 )
#define PCI_AUTOCONFIG_FLAG        ( *(UCHAR *)(LOCAL_MEM_LOCAL_ADRS + \
				     PCI_AUTOCONFIG_FLAG_OFFSET) )
#define PCI_AUTOCONFIG_DONE        ( PCI_AUTOCONFIG_FLAG != 0 )

#ifdef __cplusplus
    }
#endif /* __cplusplus */

#endif /* INCprpmc800h */