/* pciCfgShow.h */

/* Copyright 2004 Motorola, Inc. All Rights Reserved */

/*
modification history
--------------------
01a,01mar04,cak  Ported from pcicfgdump.h and overhauled.
*/

#include "vxWorks.h"

#ifndef __INCpciCfgShowh
#define __INCpciCfgShowh

#ifdef __cplusplus
    extern "C" {
#endif /* __cplusplus */

/* General configuration register offsets */

#define PCI_DEV_VEND	0    /* Device ID, Vendor ID */
#define PCI_STAT_COM    1    /* Status register, Command register */
#define PCI_CLASS_REV   2    /* Class code, Revision ID */
#define PCI_HEADER      3    /* Header type, etc. */
#define PCI_BASE_ADDR0  4    /* Base Address 0 */
#define PCI_BASE_ADDR1  5    /* Base Address 1 */
#define PCI_BASE_ADDR2  6    /* Base Address 2 */
#define PCI_BASE_ADDR3  7    /* Base Address 3 */
#define PCI_BASE_ADDR4  8    /* Base Address 4 */
#define PCI_BASE_ADDR5  9    /* Base Address 5 */
#define PCI_CISPTR      10    /* CardBus CIS Pointer */
#define PCI_SUBSYSIDS   11    /* Subsystem IDs */
#define PCI_EROMBA      12    /* Expansion ROM Base Address */
#define PCI_INT_BMSTR   15    /* Max_Lat, Min_Gnt, Interrupt Pin, Int Line */

/* PCI bridge configuration register offsets */

#define PCI_BUS_RANGE   6    /* Sec. Latency, Sub. Bus, Sec. Bus, Pri. Bus */
#define PCI_STAT_IO_LO  7    /* Sec. status, I/O limit low, I/O base low */
#define PCI_MEM_HI      8    /* Memory limit and base */
#define PCI_PREMEM_LO   9    /* Prefetchable low base and low limit */
#define PCI_PREMEM_B64  10    /* Prefetchable base upper 32 bits */
#define PCI_PREMEM_L64  11    /* Prefetchable limit upper 32 bits */
#define PCI_IO_HI       12    /* I/O limit high, I/O base high */
#define PCI_BR_EROMBA   14    /* Expansion ROM Base Address */

#define PCICLASSCODEMASK 0xff000000     /* Base Class Code Mask */
#define PCICLASSCODESHIFT 24            /* shift upper byte class code down */

#define PCISUBCLASSMASK 0x00ff0000     /* Sub - Class Code Mask */
#define PCISUBCLASSSHIFT 16            /* shift upper byte subclass code down*/

#define PCIMULTIFUNC    (1 << 23)
#define HEADERTYPE0     (0 << 16)
#define HEADERTYPE1     (1 << 16)
#define PCI_WR 0        /* write */
#define PCI_RD 1        /* read */

/* Command Register Definitions  */

#define PCIHDR_CMDREG_IOSP      (1<<0)  /* Enable IO space accesses */
#define PCIHDR_CMDREG_MEMSP     (1<<1)  /* Enable MEM space accesses */
#define PCIHDR_CMDREG_MASTR     (1<<2)  /* Enable PCI Mastership */
#define PCIHDR_CMDREG_SPCYC     (1<<3)  /* Monitor special cycles */
#define PCIHDR_CMDREG_MEMWINV   (1<<4)  /* Enable memory write and invalidate */
#define PCIHDR_CMDREG_VGASNP    (1<<5)  /* Enable VGA palette snooping */
#define PCIHDR_CMDREG_PERR      (1<<6)  /* Enable Parity error response */
#define PCIHDR_CMDREG_STEP      (1<<7)  /* Enable address/data stepping */
#define PCIHDR_CMDREG_SERR      (1<<8)  /* Enable SERR driver */
#define PCIHDR_CMDREG_FSTB2B    (1<<9)  /* Enable back to back transactions */

/* Base address register definitions */

#define PCIHDR_BASEADDR_IO              (1<<0)  /* PCI I/O space */
#define PCIHDR_BASEADDR_MEM             (0<<0)  /* PCI Memory space */
#define PCIHDR_BASEADDR_MEM_TYPE        (3<<1)  /* memory type mask */
#define PCIHDR_BASEADDR_MEM_32BIT       (0<<1)  /* map anywhere in 32-bit  */
                                                /* address space */
#define PCIHDR_BASEADDR_MEM_ONEMEG      (1<<1)  /* map below 1MB */
#define PCIHDR_BASEADDR_MEM_64BIT       (2<<1)  /* map anywhere in 64-bit  */
					    	/* address space */
#define PCIHDR_BASEADDR_MEM_PREFETCH    (1<<3)  /* prefetchable */

/* Class Codes */

#define PCIHDR_CLASS_NONE       0       /* Device has no class code */
#define PCIHDR_CLASS_MASS       1       /* Mass storage controller */
#define PCIHDR_CLASS_NETWORK    2       /* Network controller */
#define PCIHDR_CLASS_DISPLAY    3       /* Display controller */
#define PCIHDR_CLASS_MMEDIA     4       /* Multi-media controller */
#define PCIHDR_CLASS_MEMCTL     5       /* Memory controller */
#define PCIHDR_CLASS_BRIDGE     6       /* Bridge controller */
#define PCIHDR_CLASS_SCC        7       /* Simple communication controller */
#define PCIHDR_CLASS_PER        8       /* Base System Peripherals */
#define PCIHDR_CLASS_IN         9       /* Input Devices */
#define PCIHDR_CLASS_DOCK       0xA     /* Docking Stations */
#define PCIHDR_CLASS_PROC       0xB     /* Processors */
#define PCIHDR_CLASS_SBUS       0xC     /* Serial Bus Controllers */
#define PCIHDR_CLASS_NOFIT      0xFF    /* Device doesn't fit any classcodes*/

#define PCIHDR_SUBCLASS_PCI     4       /* PCI */


#define PCI_NOBODYHOME		0xFFFFFFFF
#define PCIARYSIZE			64

#define MAXPCIDEV 32            /* Number of PCI devices on a bus */
#define MAXPCIFUNC 8            /* Number of PCI functions on a device */

#define BYTEMASK        0x000000FF
#define PCICONENA       (1 << 31)


/* Peripheral Bus Controller (PBC) id definitions */

#define PBC_ISA_IDS     0x05861106      /* ISA bridge (func 0 of PBC) */
#define PBC_IDE_IDS     0x05711106      /* IDE (function 1 of PBC) */
#define PBC_USB_IDS     0x30381106      /* USB (function 2 of PBC) */
#define PBC_PM_IDS      0x30401106      /* Power Mgmt (func 3 of PBC) */

#define RAVEN_IDS       0x48011057      /* Motorola MPC10x (Raven) */
#define VME3PCI_IDS     0x000010E3      /* PCI to VME bus ctl device */
#define NCR53C825_IDS   0x00031000      /* SCSI ctl device, NCR */
#define NCR53C875_IDS   0x000F1000      /* SCSI ctl device, NCR */
#define DEC21554_IDS	0x00461011

/* Winbond */

#define PIB_ISA_IDS     0x056510AD      /* ISA bridge func 0 of PIB */

#define CPCI_LOGIGAL_SLOTS	8
#define INTA2INTD			4

/* Subclass Codes */

#define PCIHDR_SUBCLASS_OTHER   0x80    /* Other subclass */
   
/* for class 0  (pre rev 2.0 of PCI)*/

#define PCIHDR_SUBCLASS_NONVGA  0       /* Non VGA device */
#define PCIHDR_SUBCLASS_VGADEV  1       /* VGA compatible device */
	
/* for class 1 (Mass Storage) */

#define PCIHDR_SUBCLASS_SCSI    0       /* SCSI */
#define PCIHDR_SUBCLASS_IDE     1       /* IDE  */
#define PCIHDR_SUBCLASS_FLOP    2       /* Floppy  */
#define PCIHDR_SUBCLASS_IPI     3       /* IPI */
#define PCIHDR_SUBCLASS_RAID    4       /* Raid Controller */

/* for class 2 (Network) */

#define PCIHDR_SUBCLASS_ETHER   0       /* Ethernet */
#define PCIHDR_SUBCLASS_TOKEN   1       /* Token */
#define PCIHDR_SUBCLASS_FDDI    2       /* FDDI */
#define PCIHDR_SUBCLASS_ATM     3       /* ATM Controller */

/* for class 3 (Display) */

#define PCIHDR_SUBCLASS_VGA     0       /* VGA */
#define PCIHDR_SUBCLASS_XGA     1       /* XGA */

/* for class 4 (Multimedia) */

#define PCIHDR_SUBCLASS_VIDEO   0       /* Video */
#define PCIHDR_SUBCLASS_AUDIO   1       /* Audio */

/* for class 5 (Memory Controller) */

#define PCIHDR_SUBCLASS_RAM     0       /* RAM */
#define PCIHDR_SUBCLASS_FLASH   1       /* FLASH */

/* for class 6 (Bridge Devices) */

#define PCIHDR_SUBCLASS_HOST    0       /* Host bridge */
#define PCIHDR_SUBCLASS_ISA     1       /* ISA */
#define PCIHDR_SUBCLASS_EISA    2       /* EISA */
#define PCIHDR_SUBCLASS_MC      3       /* MC */
#define PCIHDR_SUBCLASS_PCI     4       /* PCI */
#define PCIHDR_SUBCLASS_PCMCIA  5       /* PCMCIA */
#define PCIHDR_SUBCLASS_NBUS    6       /* PCI NuBus */
#define PCIHDR_SUBCLASS_CBUS    7       /* PCI CardBus */

/* for class 7 (Simple Communications Controllers) */

#define PCIHDR_SUBCLASS_SER     0       /* Serial Controller */
#define PCIHDR_SUBCLASS_PAR     1       /* Parallel Controller */

/* for class 8 (Base System Peripherals) */

#define PCIHDR_SUBCLASS_PIC     0       /* Interrupt Controller */
#define PCIHDR_SUBCLASS_DMA     1       /* DMA Controller */
#define PCIHDR_SUBCLASS_TIMER   2       /* Timer */
#define PCIHDR_SUBCLASS_RTC     3       /* RTC Controller */

/* for class 9 (Input Devices) */

#define PCIHDR_SUBCLASS_KEY     0       /* Keyboard Controller */
#define PCIHDR_SUBCLASS_PEN     1       /* Digitizer Pen */
#define PCIHDR_SUBCLASS_MOUSE   2       /* Mouse Controller */

/* for class 0xA (Docking Stations) */

#define PCIHDR_SUBCLASS_STN     0       /* Docking Station */

/* for class 0xB (Processors) */

#define PCIHDR_SUBCLASS_386     0       /* 386 processor */
#define PCIHDR_SUBCLASS_486     1       /* 486 processor */
#define PCIHDR_SUBCLASS_PENTIUM 2       /* Pentium processor */
#define PCIHDR_SUBCLASS_ALPHA   0x10    /* Alpha processor */
#define PCIHDR_SUBCLASS_POWERPC 0x20    /* PowerPC */
#define PCIHDR_SUBCLASS_COPROC  0x40    /* Co-processor */

/* for class 0xC (Serial Bus Controllers) */

#define PCIHDR_SUBCLASS_FIRE    0       /* Firewire */
#define PCIHDR_SUBCLASS_ACCESS  1       /* Access.bus */
#define PCIHDR_SUBCLASS_SSA     2       /* Serial Storage Architecture */
#define PCIHDR_SUBCLASS_USB     3       /* USB -- Universal Serial Bus */
#define PCIHDR_SUBCLASS_FIBRE   4       /* Fibre Channel */

/* Big Endian to Little Endian conversion macros */

#define ULONG_BE2LE(x)  ((((x) & 0x000000FF) << 24) |      \
                         (((x) & 0x0000FF00) <<  8) |      \
                         (((x) & 0x00FF0000) >>  8) |      \
                         (((x) & 0xFF000000) >> 24))
#define ULONG_LE2BE(x)  ULONG_BE2LE(x)

#ifndef _ASMLANGUAGE

typedef struct 
    {
    UINT32 bus;
    UINT32 device;
    UINT32 function;
    UINT32 offset;
    UINT32 value;
    UINT32 bytes;
    UINT32 devVend;
    UINT8  confRegs[256];
    } PCI_IOCTL;

/* Structure for PCI class code information */

typedef struct 
    {
    UCHAR classcode;
    UCHAR *classtext;
    } CLASSCODE_DATA;

/* Structure for PCI subclass code information */

typedef struct
    {
    UINT8 classcode;
    UINT8 subclasscode;
    UINT8 *subclasstext;
    } SUBCLASSCODE_DATA;

/* structure for the device & bridge header */

typedef struct
    {
    int		dsConfigAdr;	/* 0x80: Downstream Configuration Address */
    int		dsConfigData;	/* 0x84: Downstream Configuration Data */
    int		usConfigAdr;	/* 0x88: Upstream Configuration Address */
    int		usConfigData;   /* 0x8c: Upstream Configuration Data */
    short	configOwn;	/* 0x90: Configuration Own Bits */
    short	configCtrl;	/* 0x92: Configuration Control and Status */
    int		dsMem0TB;	/* 0x94: Downstream Memory 0 Translated Base */
    int		dsIOMem1TB;	/* 0x98: Downstream IO Memory1 TranslatedBase */
    int		dsMem2TB;	/* 0x9C: Downstream Memory 2 Translated Base */
    int		dsMem3TB;	/* 0xA0: Downstream Memory 3 Translated Base */
    int		usIOMem0TB;	/* 0xA4: Upstream IO Memory 0 Translated Base */
    int		usMem1TB;	/* 0xA8: Upstream Memory 1 Translated Base */
    int		dsMem0Setup;	/* 0xAC: Downstream Memory 0 Setup Register */
    int		dsIOMem1Setup;	/* 0xB0: Downstream IO Memory1 Setup Register */
    int		dsMem2Setup;	/* 0xB4: Downstream Memory 2 Setup Register */
    int		dsMem3Setup;	/* 0xB8: Downstream Memory 3 Setup Register */
    int		u32DsMemSetup;	/* 0xBC: Upper32 Downstream Mem3 Setup Reg */
    int		priExROMSetup;	/* 0xC0: Primary Expansion ROM Setup Register */
    int		usIOMem0Setup;	/* 0xC4: Upstream I/O Memory 0 Setup Register */
    int		usMem1Setup;	/* 0xC8: Upstream Memory 1 Setup Register */
    short	chipCtrl0;	/* 0xCC: Chip Control 0 */
    short	chipCtrl1;	/* 0xCE: Chip Control 1 */
    short	chipStatus;	/* 0xD0: Chip Status */
    short	arbiterCtrl;	/* 0xD2: Arbiter Control */
    unsigned char priSERR;	/* 0xD4: Primary SERR# Disables */
    unsigned char secSERR;	/* 0xD5: Secondary SERR# Disables */
    short	reserved1;	/* 0xD6: reserved */
    int		resetCtrl;	/* 0xD8: Reset Control */
    unsigned char capId1;	/* 0xDC: Capability ID */
    unsigned char nextItem1;	/* 0xDD: Next Item Ptr */
    short	powerMgt;	/* 0xDE: Power Management Capabilities */
    short	powerMgtCsr;	/* 0xE0: Power Management CSR */
    unsigned char pmcsr;	/* 0xE2: Capability ID */
    unsigned char pmData;       /* 0xE3: Next Item Ptr */
    unsigned char capId2;	/* 0xE4: Capability ID */
    unsigned char nextItem2;    /* 0xE5: Next Item Ptr */
    short	vpdAdr;		/* 0xE6: VPD Address */
    int		vpdData;	/* 0xE8: VPD Data */
    unsigned char capId3;       /* 0xEC: Capability ID */
    unsigned char nextItem3;    /* 0xED: Next Item Ptr */
    unsigned char hotSwapCtrl;	/* 0xEE: Hot Swap Control */
    unsigned char reserved2;	/* reserved */
    unsigned char reserved3[4];	/* reserved */
    } PCI_DB_CONFIG_MAP;
#endif /* _ASMLANGUAGE */

/* Shared configuration offsets viewed from primary or secondary side */

#define DEC2155X_CFG_DS_CONFIG_ADDRESS     0x80
#define DEC2155X_CFG_DS_CONFIG_DATA        0x84
#define DEC2155X_CFG_US_CONFIG_ADDRESS     0x88
#define DEC2155X_CFG_US_CONFIG_DATA        0x8c
#define DEC2155X_CFG_CFG_OWN_BITS          0x90
#define DEC2155X_CFG_CFG_CTRL_AND_STS      0x92
#define DEC2155X_CFG_DS_MEM0_TB            0x94
#define DEC2155X_CFG_DS_IO_OR_MEM1_TB      0x98
#define DEC2155X_CFG_DS_MEM2_TB            0x9c
#define DEC2155X_CFG_DS_MEM3_TB            0xa0
#define DEC2155X_CFG_US_IO_OR_MEM0_TB      0xa4
#define DEC2155X_CFG_US_MEM1_TB            0xa8
#define DEC2155X_CFG_DS_MEM0_SETUP         0xac
#define DEC2155X_CFG_DS_IO_OR_MEM1_SETUP   0xb0
#define DEC2155X_CFG_DS_MEM2_SETUP         0xb4
#define DEC2155X_CFG_DS_MEM3_SETUP         0xb8
#define DEC2155X_CFG_UPR32_BITS_DS_MEM3_SU 0xbc
#define DEC2155X_CFG_PRI_EXP_ROM_SETUP     0xc0
#define DEC2155X_CFG_US_IO_OR_MEM0_SETUP   0xc4
#define DEC2155X_CFG_US_MEM1_SETUP         0xc8
#define DEC2155X_CFG_CHP_CTRL0             0xcc
#define DEC2155X_CFG_CHP_CTRL1             0xce
#define DEC2155X_CFG_CHP_STATUS            0xd0
#define DEC2155X_CFG_ARB_CTRL              0xd2
#define DEC2155X_CFG_PRI_SERR_DISABLES     0xd4
#define DEC2155X_CFG_SEC_SERR_DISABLES     0xd5
#define DEC2155X_CFG_RESET_CONTROL         0xd8
#define DEC2155X_CFG_PWR_MGMT_CAP_ID       0xdc
#define DEC2155X_CFG_PWR_MGMT_NXT_PTR      0xdd
#define DEC2155X_CFG_PWR_MGMT_CAP          0xde
#define DEC2155X_CFG_PWR_MGMT_CSR          0xe0
#define DEC2155X_CFG_PMCSR_BSE             0xe2
#define DEC2155X_CFG_PM_DATA               0xe3
#define DEC2155X_CFG_VPD_CAP_ID            0xe4
#define DEC2155X_CFG_VPD_NXT_PTR           0xe5
#define DEC2155X_CFG_VPD_ADRS              0xe6
#define DEC2155X_CFG_VPD_DATA              0xe8
#define DEC2155X_CFG_HS_CTRL_CAP_ID        0xec
#define DEC2155X_CFG_HS_CTRL_NXT_PTR       0xed
#define DEC2155X_CFG_HS_CTRL               0xee

#ifdef __cplusplus
    }
#endif /* __cplusplus */

#endif /* __INCpciCfgShowh */
