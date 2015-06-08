/* pciIomapLib.h - PCI bus constants header file */

/* Copyright 1984-2001 Wind River Systems, Inc. */

/*
modification history
--------------------
01i,27oct01,dat  Adding warnings about obsolete drivers
01h,15oct98,dat  SPR 9098, added dllLib.h
01g,12jan97,hdn  changed member/variable name "vender" to "vendor".
01f,12jan97,hdn  changed member/variable name "class" to "classCode".
01e,03dec96,hdn  added header-type bits definitions.
01d,13nov96,hdn  added command-bit and address-mask definitions.
01c,06aug96,hdn  added a structure PCI_INT_NODE.
01b,28mar96,hdn  removed BIOS dependent stuff.
01a,23nov94,bcs  written.
*/

#ifndef __INCpciIomapLibh
#define __INCpciIomapLibh

#include "dllLib.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
WARNING: The pciIomapLib driver is no longer supported. Please switch
all BSPs to use the pciConfigLib driver instead.
*/

#warning "h/drv/pci/pciIomapLib.h is being obsoleted, please use pciConfigLib"


/* PCI Configuration mechanisms */

#define	PCI_MECHANISM_1		1	/* current PC-AT hardware mechanism */
#define	PCI_MECHANISM_2		2	/* deprecated */

/* Configuration I/O addresses for mechanism 1 */

#define	PCI_CONFIG_ADDR		0x0cf8	/* write 32 bits to set address */
#define	PCI_CONFIG_DATA		0x0cfc	/* 8, 16, or 32 bit accesses */

/* Configuration I/O addresses for mechanism 2 */

#define	PCI_CONFIG_CSE		0x0cf8	/* CSE register */
#define	PCI_CONFIG_FORWARD	0x0cfa	/* forward register */
#define	PCI_CONFIG_BASE		0xc000	/* base register */

/* number of IRQs mapped on PCI interrupt */

#define PCI_IRQ_LINES		16

/* PCI command bits */

#define PCI_CMD_IO_ENABLE	0x0001	/* IO access enable */
#define PCI_CMD_MEM_ENABLE	0x0002	/* memory access enable */
#define PCI_CMD_MASTER_ENABLE	0x0004	/* bus master enable */
#define PCI_CMD_MON_ENABLE	0x0008	/* monitor special cycles enable */
#define PCI_CMD_WI_ENABLE	0x0010	/* write and invalidate enable */
#define PCI_CMD_SNOOP_ENABLE	0x0020	/* palette snoop enable */
#define PCI_CMD_PERR_ENABLE	0x0040	/* parity error enable */
#define PCI_CMD_WC_ENABLE	0x0080	/* wait cycle enable */
#define PCI_CMD_SERR_ENABLE	0x0100	/* system error enable */
#define PCI_CMD_FBTB_ENABLE	0x0200	/* fast back to back enable */

/* PCI base address mask bits */

#define PCI_MEMBASE_MASK	~0xf	/* mask for memory base address */
#define PCI_IOBASE_MASK		~0x3	/* mask for IO base address */
#define PCI_BASE_IO		0x1	/* IO space indicator */
#define PCI_BASE_BELOW_1M	0x2	/* memory locate below 1MB */
#define PCI_BASE_IN_64BITS	0x4	/* memory locate anywhere in 64 bits */
#define PCI_BASE_PREFETCH	0x8	/* memory prefetchable */

/* PCI header type bits */

#define PCI_HEADER_TYPE_MASK	0x7f	/* mask for header type */
#define PCI_HEADER_PCI_PCI	0x01	/* PCI to PCI bridge */
#define PCI_HEADER_MULTI_FUNC	0x80	/* multi function device */


/* Standard device configuration register offsets */
/* Note that only modulo-4 addresses are written to the address register */

#define	PCI_CFG_VENDOR_ID	0x00
#define	PCI_CFG_DEVICE_ID	0x02
#define	PCI_CFG_COMMAND		0x04
#define	PCI_CFG_STATUS		0x06
#define	PCI_CFG_REVISION	0x08
#define	PCI_CFG_PROGRAMMING_IF	0x09
#define	PCI_CFG_SUBCLASS	0x0a
#define	PCI_CFG_CLASS		0x0b
#define	PCI_CFG_CACHE_LINE_SIZE	0x0c
#define	PCI_CFG_LATENCY_TIMER	0x0d
#define	PCI_CFG_HEADER_TYPE	0x0e
#define	PCI_CFG_BIST		0x0f
#define	PCI_CFG_BASE_ADDRESS_0	0x10
#define	PCI_CFG_BASE_ADDRESS_1	0x14
#define	PCI_CFG_BASE_ADDRESS_2	0x18
#define	PCI_CFG_BASE_ADDRESS_3	0x1c
#define	PCI_CFG_BASE_ADDRESS_4	0x20
#define	PCI_CFG_BASE_ADDRESS_5	0x24
#define	PCI_CFG_CIS		0x28
#define	PCI_CFG_SUB_VENDER_ID	0x2c
#define	PCI_CFG_SUB_SYSTEM_ID	0x2e
#define	PCI_CFG_EXPANSION_ROM	0x30
#define	PCI_CFG_RESERVED_0	0x34
#define	PCI_CFG_RESERVED_1	0x38
#define	PCI_CFG_DEV_INT_LINE	0x3c
#define	PCI_CFG_DEV_INT_PIN	0x3d
#define	PCI_CFG_MIN_GRANT	0x3e
#define	PCI_CFG_MAX_LATENCY	0x3f


/* PCI-to-PCI bridge configuration register offsets */
/* Note that only modulo-4 addresses are written to the address register */

#define	PCI_CFG_PRIMARY_BUS	0x18
#define	PCI_CFG_SECONDARY_BUS	0x19
#define	PCI_CFG_SUBORDINATE_BUS	0x1a
#define	PCI_CFG_SEC_LATENCY	0x1b
#define	PCI_CFG_IO_BASE		0x1c
#define	PCI_CFG_IO_LIMIT	0x1d
#define	PCI_CFG_SEC_STATUS	0x1e
#define	PCI_CFG_MEM_BASE	0x20
#define	PCI_CFG_MEM_LIMIT	0x22
#define	PCI_CFG_PRE_MEM_BASE	0x24
#define	PCI_CFG_PRE_MEM_LIMIT	0x26
#define	PCI_CFG_PRE_MEM_BASE_U	0x28
#define	PCI_CFG_PRE_MEM_LIMIT_U	0x2c
#define	PCI_CFG_IO_BASE_U	0x30
#define	PCI_CFG_IO_LIMIT_U	0x32
#define	PCI_CFG_ROM_BASE	0x38
#define	PCI_CFG_BRG_INT_LINE	0x3c
#define	PCI_CFG_BRG_INT_PIN	0x3d
#define	PCI_CFG_BRIDGE_CONTROL	0x3e


#ifndef _ASMLANGUAGE

/* structure for the device & bridge header */

typedef struct pciHeaderDevice
    {
    short	vendorId;	/* vendor ID */
    short	deviceId;	/* device ID */
    short	command;	/* command register */
    short	status;		/* status register */
    char	revisionId;	/* revision ID */
    char	classCode;	/* class code */
    char	subClass;	/* sub class code */
    char	progIf;		/* programming interface */
    char	cacheLine;	/* cache line */
    char	latency;	/* latency time */
    char	headerType;	/* header type */
    char	bist;		/* BIST */
    int		base0;		/* base address 0 */
    int		base1;		/* base address 1 */
    int		base2;		/* base address 2 */
    int		base3;		/* base address 3 */
    int		base4;		/* base address 4 */
    int		base5;		/* base address 5 */
    int		cis;		/* cardBus CIS pointer */
    short	subVendorId;	/* sub system vendor ID */
    short	subSystemId;	/* sub system ID */
    int		romBase;	/* expansion ROM base address */
    int		reserved0;	/* reserved */
    int		reserved1;	/* reserved */
    char	intLine;	/* interrupt line */
    char	intPin;		/* interrupt pin */
    char	minGrant;	/* min Grant */
    char	maxLatency;	/* max Latency */
    } PCI_HEADER_DEVICE;

typedef struct pciHeaderBridge
    {
    short	vendorId;	/* vendor ID */
    short	deviceId;	/* device ID */
    short	command;	/* command register */
    short	status;		/* status register */
    char	revisionId;	/* revision ID */
    char	classCode;	/* class code */
    char	subClass;	/* sub class code */
    char	progIf;		/* programming interface */
    char	cacheLine;	/* cache line */
    char	latency;	/* latency time */
    char	headerType;	/* header type */
    char	bist;		/* BIST */
    int		base0;		/* base address 0 */
    int		base1;		/* base address 1 */
    char	priBus;		/* primary bus number */
    char	secBus;		/* secondary bus number */
    char	subBus;		/* subordinate bus number */
    char	secLatency;	/* secondary latency timer */
    char	ioBase;		/* IO base */
    char	ioLimit;	/* IO limit */
    short	secStatus;	/* secondary status */
    short	memBase;	/* memory base */
    short	memLimit;	/* memory limit */
    short	preBase;	/* prefetchable memory base */
    short	preLimit;	/* prefetchable memory limit */
    int		preBaseUpper;	/* prefetchable memory base upper 32 bits */
    int		preLimitUpper;	/* prefetchable memory base upper 32 bits */
    short	ioBaseUpper;	/* IO base upper 16 bits */
    short	ioLimitUpper;	/* IO limit upper 16 bits */
    int		reserved;	/* reserved */
    int		romBase;	/* expansion ROM base address */
    char	intLine;	/* interrupt line */
    char	intPin;		/* interrupt pin */
    short	control;	/* bridge control */
    } PCI_HEADER_BRIDGE;

typedef struct pciIntRtn
    {
    DL_NODE	node;		/* double link list */
    VOIDFUNCPTR	routine;	/* interrupt handler */
    int		parameter;	/* parameter of the handler */
    } PCI_INT_RTN;


#if defined(__STDC__) || defined(__cplusplus)

STATUS pciIomapLibInit	(int mechanism, int addr0, int addr1, int addr2);
STATUS pciFindDevice	(int vendorId, int deviceId, int index,	
			 int * pBusNo, int * pDeviceNo, int * pFuncNo);
STATUS pciFindClass	(int classCode, int index,	
			 int * pBusNo, int * pDeviceNo, int * pFuncNo);
STATUS pciConfigInByte	(int busNo, int	deviceNo, int funcNo, int address,
			 char * pData);
STATUS pciConfigInWord	(int busNo, int	deviceNo, int funcNo, int address,
			 short * pData);
STATUS pciConfigInLong	(int busNo, int	deviceNo, int funcNo, int address,
			 int * pData);
STATUS pciConfigOutByte	(int busNo, int	deviceNo, int funcNo, int address,
			 char data);
STATUS pciConfigOutWord	(int busNo, int	deviceNo, int funcNo, int address,
			 short data);
STATUS pciConfigOutLong	(int busNo, int	deviceNo, int funcNo, int address,
			 int data);
STATUS pciSpecialCycle	(int busNo, int message);
STATUS pciDeviceShow	(int busNo);
STATUS pciHeaderShow	(int busNo, int	deviceNo, int funcNo);
STATUS pciFindDeviceShow(int vendorId, int deviceId, int index);
STATUS pciFindClassShow	(int classCode, int index);
STATUS pciIntConnect	(VOIDFUNCPTR *vector, VOIDFUNCPTR routine,
			 int parameter);
STATUS pciIntDisconnect	(VOIDFUNCPTR *vector, VOIDFUNCPTR routine);

#else	/* __STDC__ */

STATUS pciIomapLibInit	();
STATUS pciFindDevice	();
STATUS pciFindClass	();
STATUS pciConfigInByte	();
STATUS pciConfigInWord	();
STATUS pciConfigInLong	();
STATUS pciConfigOutByte	();
STATUS pciConfigOutWord	();
STATUS pciConfigOutLong	();
STATUS pciSpecialCycle	();
STATUS pciDeviceShow	();
STATUS pciHeaderShow	();
STATUS pciFindDeviceShow();
STATUS pciFindClassShow	();
STATUS pciIntConnect	();
STATUS pciIntDisconnect	();

#endif	/* __STDC__ */

#endif	/* _ASMLANGUAGE */

#ifdef __cplusplus
}
#endif

#endif /* __INCpciIomapLibh */
