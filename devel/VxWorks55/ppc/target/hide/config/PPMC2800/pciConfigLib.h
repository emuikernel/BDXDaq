/******************************************************************************
*              (c), Copyright, MOTOROLA  INC  - ECC                  *
*                                                                             *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MOTOROLA , INC.        *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT *
* OF MOTOROLA   OR ANY THIRD PARTY. MOTOROLA   RESERVES THE RIGHT AT ITS SOLE DISCRETION*
* TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MOTOROLA .       *
* THIS CODE IS PROVIDED "AS IS". MOTOROLA  MAKES NO WARRANTIES, EXPRESS,*
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.  *
*******************************************************************************

 * pciConfigLib.h - PCI bus constants header file */

/* Copyright 1984-1998 Wind River Systems, Inc. */

/*
modification history
--------------------
01z,27aug98,tm   changed PCI_MAX_BUS default to 255 (SPR 22256)
01y,28jul98,tm   PCI_MAX_BUS,.._DEV, .._FUNC moved here from pciConfigLib.c to
                   allow config.h (included before this file) values to override
01x,24jul98,tm   added networking subclass definitions
01w,12jul98,dat  added compatbility for pciConfigBDFPack.
01v,29may98,tm   added prototypes for pciConfigModify{Byte, Word}
01u,05may98,tm   added PCI_MAX_DEV, PCI_MAX_FUNC
01t,24apr98,tm   merged BSP rls changes from dat: removed non-ANSI protos (?)
01s,11apr98,tm   added prototype for pciDevConfig, SPR 9098
01r,24mar98,tm   added pciConfigModifyLong in support of autoconfig
01q,23mar98,tm   added PCI subclass, BAR definitions in support of autoconfig
01p,11mar98,tm   renamed from pciIomapLib.h to pciConfigLib.h
                 scope changed: LOCAL int pciPack() --> int pciConfigBdfPack()
01o,08mar98,tm   moved PCI_IN_*, PCI_OUT_* macros here from pciIomapLib.c
01n,04mar98,tm   moved pciIomapShow.c prototypes to pciIomapShow.h
01m,04mar98,tm   updated argument types on all I/O functions 
01l,04mar98,dat  added PCI_MECHANISM_0 for user defined config mechanism
01k,04mar98,tm   moved config space header definitions to pciHeaderDefs.h
01j,24feb98,tm   moved interrupt handling functions to pciIntLib.{c,h}
                 added PCI class definitions
01i,06dec97,rlp  added SNOOZE_MODE, SLEEP_MODE_DIS and PCI_CFG_MODE.
01h,25jun97,jpd  added PCI_MECHANISM_3, increased PCI_IRQ_LINES to 32.
01g,12jan97,hdn  changed member/variable name "vender" to "vendor".
01f,12jan97,hdn  changed member/variable name "class" to "classCode".
01e,03dec96,hdn  added header-type bits definitions.
01d,13nov96,hdn  added command-bit and address-mask definitions.
01c,06aug96,hdn  added a structure PCI_INT_NODE.
01b,28mar96,hdn  removed BIOS dependent stuff.
01a,23nov94,bcs  written.
*/

#ifndef __INCpciConfigLibh
#define __INCpciConfigLibh

#include "pciScan.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Bus Topology Constraints
 *
 * The following defines specify, by default, the maximum number of busses,
 * devices and functions allowed by the PCI 2.1 Specification.
 *
 * NOTE: config.h MUST be included before this file if either PCI_MAX_BUS,
 * PCI_MAX_DEV, or PCI_MAX_FUNC is changed to a BSP-specific value in 
 * config.h
 *
 */
#define SWAPING 
#ifdef SWAPING

#define BYTE_SWAP_32(data) \
((((data) & 0x000000ff) << 24) | (((data) & 0x0000ff00) << 8) | (((data) & 0x00ff0000) >> 8) | (((data) & 0xff000000) >> 24))

#define BYTE_SWAP_16(data) \
   ((((data) & 0x00FF) << 8) | (((data) & 0xFF00) >> 8))
#else

#define BYTE_SWAP_32(data) (data)
#define BYTE_SWAP_16(data) (data)

#endif

#ifndef PCI_MAX_BUS
    #define PCI_MAX_BUS	255
#endif  /* PCI_MAX_BUS */

#ifndef MAX_PCI0_DEVICES /* defined in pciScan.h */
    #define PCI_MAX_DEV	32
#else
    #define PCI_MAX_DEV MAX_PCI0_DEVICES
#endif  /* PCI_MAX_DEV */

#ifndef PCI_MAX_FUNC
    #define PCI_MAX_FUNC	8
#endif  /* PCI_MAX_FUNC */

/* PCI Configuration mechanisms */

#define PCI_MECHANISM_0     0   /* non-std user-supplied interface */
#define	PCI_MECHANISM_1	    1   /* current PC-AT hardware mechanism */
#define	PCI_MECHANISM_2	    2   /* deprecated */

/* Configuration I/O addresses for mechanism 1 */

#define	PCI_CONFIG_ADDR		0x0cf8	/* write 32 bits to set address */
#define	PCI_CONFIG_DATA		0x0cfc	/* 8, 16, or 32 bit accesses */

/* Configuration I/O addresses for mechanism 2 */

#define	PCI_CONFIG_CSE		0x0cf8	/* CSE register */
#define	PCI_CONFIG_FORWARD	0x0cfa	/* forward register */
#define	PCI_CONFIG_BASE		0xc000	/* base register */

/* PCI command bits */

#define PCI_CMD_IO_ENABLE     0x0001 /* IO access enable */
#define PCI_CMD_MEM_ENABLE    0x0002 /* memory access enable */
#define PCI_CMD_MASTER_ENABLE 0x0004 /* bus master enable */
#define PCI_CMD_MON_ENABLE    0x0008 /* monitor special cycles enable */
#define PCI_CMD_WI_ENABLE     0x0010 /* write and invalidate enable */
#define PCI_CMD_SNOOP_ENABLE  0x0020 /* palette snoop enable */
#define PCI_CMD_PERR_ENABLE   0x0040 /* parity error enable */
#define PCI_CMD_WC_ENABLE     0x0080 /* wait cycle enable */
#define PCI_CMD_SERR_ENABLE   0x0100 /* system error enable */
#define PCI_CMD_FBTB_ENABLE   0x0200 /* fast back to back enable */

/* PCI base address mask bits */

#define PCI_MEMBASE_MASK	~0xf	/* mask for memory base address */
#define PCI_IOBASE_MASK		~0x3	/* mask for IO base address */
#define PCI_BASE_IO	         0x1    /* IO space indicator */
#define PCI_BASE_BELOW_1M	 0x2    /* memory locate below 1MB */
#define PCI_BASE_IN_64BITS	 0x4    /* memory locate anywhere in 64 bits */
#define PCI_BASE_PREFETCH	 0x8    /* memory prefetchable */

/* Base Address Register Memory/IO Attribute bits */

#define PCI_BAR_SPACE_MASK	0x01
#define PCI_BAR_SPACE_IO	0x01
#define PCI_BAR_SPACE_MEM	0x00

#define PCI_BAR_MEM_TYPE_MASK   (0x03 << 1)
#define PCI_BAR_MEM_ADDR32      (0x00 << 1)
#define PCI_BAR_MEM_BELOW_1MB	(0x01 << 1)
#define PCI_BAR_MEM_ADDR64      (0x02 << 1)
#define PCI_BAR_MEM_RESERVED    (0x03 << 1)

#define PCI_BAR_MEM_PREFETCH    (0x01 << 3)

/* PCI header type bits */

#define PCI_HEADER_TYPE_MASK	0x7f	/* mask for header type */
#define PCI_HEADER_PCI_PCI	0x01	/* PCI to PCI bridge */
#define PCI_HEADER_TYPE0        0x00    /* normal device header */
#define PCI_HEADER_MULTI_FUNC	0x80	/* multi function device */

/* PCI configuration device and driver */
 
#define SNOOZE_MODE             0x40    /* snooze mode */
#define SLEEP_MODE_DIS          0x00    /* sleep mode disable */

/* Standard device configuration register offsets */
/* Note that only modulo-4 addresses are written to the address register */

#define	PCI_CFG_VENDOR_ID	    0x00
#define	PCI_CFG_DEVICE_ID	    0x02
#define	PCI_CFG_COMMAND		    0x04
#define	PCI_CFG_STATUS		    0x06
#define	PCI_CFG_REVISION	    0x08
#define	PCI_CFG_PROGRAMMING_IF	0x09
#define	PCI_CFG_SUBCLASS	    0x0a
#define	PCI_CFG_CLASS		    0x0b
#define	PCI_CFG_CACHE_LINE_SIZE	0x0c
#define	PCI_CFG_LATENCY_TIMER	0x0d
#define	PCI_CFG_HEADER_TYPE	    0x0e
#define	PCI_CFG_BIST		    0x0f
#define	PCI_CFG_BASE_ADDRESS_0	0x10
#define	PCI_CFG_BASE_ADDRESS_1	0x14
#define	PCI_CFG_BASE_ADDRESS_2	0x18
#define	PCI_CFG_BASE_ADDRESS_3	0x1c
#define	PCI_CFG_BASE_ADDRESS_4	0x20
#define	PCI_CFG_BASE_ADDRESS_5	0x24
#define	PCI_CFG_CIS		        0x28
#define	PCI_CFG_SUB_VENDER_ID	0x2c
#define	PCI_CFG_SUB_SYSTEM_ID	0x2e
#define	PCI_CFG_EXPANSION_ROM	0x30
#define	PCI_CFG_RESERVED_0	    0x34
#define	PCI_CFG_RESERVED_1	    0x38
#define	PCI_CFG_DEV_INT_LINE	0x3c
#define	PCI_CFG_DEV_INT_PIN	    0x3d
#define	PCI_CFG_MIN_GRANT	    0x3e
#define	PCI_CFG_MAX_LATENCY	    0x3f
#define PCI_CFG_SPECIAL_USE     0x41
#define PCI_CFG_MODE            0x43


/* PCI-to-PCI bridge configuration register offsets */
/* Note that only modulo-4 addresses are written to the address register */

#define	PCI_CFG_PRIMARY_BUS	    0x18
#define	PCI_CFG_SECONDARY_BUS	0x19
#define	PCI_CFG_SUBORDINATE_BUS	0x1a
#define	PCI_CFG_SEC_LATENCY	    0x1b
#define	PCI_CFG_IO_BASE		    0x1c
#define	PCI_CFG_IO_LIMIT	    0x1d
#define	PCI_CFG_SEC_STATUS	    0x1e
#define	PCI_CFG_MEM_BASE	    0x20
#define	PCI_CFG_MEM_LIMIT	    0x22
#define	PCI_CFG_PRE_MEM_BASE	0x24
#define	PCI_CFG_PRE_MEM_LIMIT	0x26
#define	PCI_CFG_PRE_MEM_BASE_U	0x28
#define	PCI_CFG_PRE_MEM_LIMIT_U	0x2c
#define	PCI_CFG_IO_BASE_U	    0x30
#define	PCI_CFG_IO_LIMIT_U	    0x32
#define	PCI_CFG_ROM_BASE	    0x38
#define	PCI_CFG_BRG_INT_LINE	0x3c
#define	PCI_CFG_BRG_INT_PIN     0x3d
#define	PCI_CFG_BRIDGE_CONTROL	0x3e

/* PCI Class definitions for find by class function */

#define PCI_CLASS_PRE_PCI20	    0x00
#define PCI_CLASS_MASS_STORAGE	0x01
#define PCI_CLASS_NETWORK_CTLR	0x02
#define PCI_CLASS_DISPLAY_CTLR	0x03
#define PCI_CLASS_MMEDIA_DEVICE	0x04
#define PCI_CLASS_MEM_CTLR	    0x05
#define PCI_CLASS_BRIDGE_CTLR	0x06
#define PCI_CLASS_COMM_CTLR	    0x07
#define PCI_CLASS_BASE_PERIPH	0x08
#define PCI_CLASS_INPUT_DEVICE	0x09
#define PCI_CLASS_DOCK_DEVICE	0x0A
#define PCI_CLASS_PROCESSOR	    0x0B
#define PCI_CLASS_SERIAL_BUS	0x0C
#define PCI_CLASS_UNDEFINED	    0xFF

/* PCI Subclass definitions */

#define PCI_SUBCLASS_00         0x00
#define PCI_SUBCLASS_01         0x01
#define PCI_SUBCLASS_02         0x02
#define PCI_SUBCLASS_03         0x03
#define PCI_SUBCLASS_04         0x04
#define PCI_SUBCLASS_05         0x05
#define PCI_SUBCLASS_06         0x06
#define PCI_SUBCLASS_07         0x07
#define PCI_SUBCLASS_10         0x10
#define PCI_SUBCLASS_20         0x20
#define PCI_SUBCLASS_80         0x80

/* Bridge Device subclasses */

#define PCI_SUBCLASS_HOST_PCI_BRIDGE (PCI_SUBCLASS_00)
#define PCI_SUBCLASS_ISA_BRIDGE		 (PCI_SUBCLASS_01)
#define PCI_SUBCLASS_P2P_BRIDGE		 (PCI_SUBCLASS_04)
#define PCI_SUBCLASS_PCMCIA_BRIDGE	 (PCI_SUBCLASS_05)
#define PCI_SUBCLASS_CARDBUS_BRIDGE	 (PCI_SUBCLASS_07)

/* Processor subclasses */

#define PCI_SUBCLASS_PROCESSOR_386	   (PCI_SUBCLASS_00)
#define PCI_SUBCLASS_PROCESSOR_486	   (PCI_SUBCLASS_01)
#define PCI_SUBCLASS_PROCESSOR_PENTIUM (PCI_SUBCLASS_02)
#define PCI_SUBCLASS_PROCESSOR_ALPHA   (PCI_SUBCLASS_10)
#define PCI_SUBCLASS_PROCESSOR_POWERPC (PCI_SUBCLASS_20)

/* Serial bus subclasses */

#define PCI_SUBCLASS_SERBUS_FIREWIRE (PCI_SUBCLASS_00)
#define PCI_SUBCLASS_SERBUS_USB		 (PCI_SUBCLASS_03)

/* Network subclasses */

#define PCI_SUBCLASS_NET_ETHERNET	(PCI_SUBCLASS_00)
#define PCI_SUBCLASS_NET_TOKEN_RING	(PCI_SUBCLASS_01)
#define PCI_SUBCLASS_NET_FDDI		(PCI_SUBCLASS_02)
#define PCI_SUBCLASS_NET_ATM		(PCI_SUBCLASS_03)

/* Conditional defines for new configuration definitions */

/* Cache Line Size - 32 32-bit value = 128 bytes */
#ifndef PCI_CACHE_LINE_SIZE
#define PCI_CACHE_LINE_SIZE            0x20
#endif /* PCI_CACHE_LINE_SIZE */

/* Latency Timer value - 255 PCI clocks */
#ifndef PCI_LATENCY_TIMER
#define PCI_LATENCY_TIMER           0xff
#endif /* PCI_LATENCY_TIMER */

#ifndef _ASMLANGUAGE

/*
 * Configuration space access macros
 */

#ifndef	PCI_IN_BYTE
#define PCI_IN_BYTE(x)		sysInByte (x)
#endif

#ifndef	PCI_IN_WORD
#define PCI_IN_WORD(x)		sysInWord (x)
#endif

#ifndef	PCI_IN_LONG
#define PCI_IN_LONG(x)		sysInLong (x)
#endif

#ifndef	PCI_OUT_BYTE
#define PCI_OUT_BYTE(x,y)	sysOutByte (x,y)
#endif

#ifndef	PCI_OUT_WORD
#define PCI_OUT_WORD(x,y)	sysOutWord (x,y)
#endif

#ifndef	PCI_OUT_LONG
#define PCI_OUT_LONG(x,y)	sysOutLong (x,y)
#endif

STATUS pciConfigLibInit	(int mechanism, ULONG addr0, ULONG addr1, ULONG addr2);
STATUS pciFindDevice	(int vendorId, int deviceId, int index,	
			             int *pBusNo, int *pDeviceNo, int *pFuncNo);
STATUS pciFindClass	    (int classCode, int index, int *pBusNo,
                         int *pDeviceNo, int *pFuncNo);
STATUS pciConfigInByte	(int busNo, int	deviceNo, int funcNo, int address,
			             UINT8 *pData);
STATUS pciConfigInWord	(int busNo, int	deviceNo, int funcNo, int address,
			             UINT16 *pData);
STATUS pciConfigInLong	(int busNo, int	deviceNo, int funcNo, int address,
			             UINT32 *pData);
STATUS pciConfigOutByte	(int busNo, int	deviceNo, int funcNo, int address,
			             UINT8 data);
STATUS pciConfigOutWord	(int busNo, int	deviceNo, int funcNo, int address,
			             UINT16 data);
STATUS pciConfigOutLong	(int busNo, int	deviceNo, int funcNo, int address,
			             UINT32 data);


void sysOutByte (ULONG address, UINT8 data);
UINT8 sysInByte (ULONG address);


STATUS pciSpecialCycle (int busNo, UINT32 message);
int    pciConfigBdfPack(int busNo, int deviceNo, int funcNo);
STATUS pciDevConfig    (int busNo, int deviceNo, int funcNo,
		                UINT32 devIoBaseAdrs, UINT32 devMemBaseAdrs,
		                UINT32 command );

STATUS pciConfigModifyLong(int busNo, int deviceNo, int funcNo, int offset,   
			               UINT32 bitMask, UINT32 data );
STATUS pciConfigModifyWord(int busNo, int deviceNo, int funcNo, int offset,   
			               UINT16 bitMask, UINT16 data );
STATUS pciConfigModifyByte(int busNo, int deviceNo, int funcNo, int offset,   
			               UINT8 bitMask, UINT8 data );
#endif	/* _ASMLANGUAGE */

/* For backward compatibility pciConfigBDFPack == pciConfigBdfPack */

#define pciConfigBDFPack	pciConfigBdfPack

#ifdef __cplusplus
}
#endif

#endif /* __INCpciConfigLibh */
