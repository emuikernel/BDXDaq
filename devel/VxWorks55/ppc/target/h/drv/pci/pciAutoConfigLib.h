/* pciAutoConfigLib.h - PCI bus automatic resource allocation facility */

/* Copyright 1984-2001 Wind River Systems, Inc. */
/* Copyright 1997,1998 Motorola, Inc. All Rights Reserved */

/*
modification history
--------------------
01g,16oct01,tor  Added support macros for pciAutoCfgCtl(), see
		 pciAutoConfigLib.c for details.
01f,07may99,tm   Added pciAutoAddrAlign to perform alignment checking
                 pciAutoFuncDis-/En-able, pciAutoRegConfig public (SPR 26484)
01e,30mar99,dat  SPR 25312, changed class to pciClass
01d,29jul98,scb  added roll call capability
01c,29jul98,tm   code review changes / added _DEV_DISPLAY attribute
01b,24jul98,tm   added int routing offset to PCI_LOC structure 
                 added autoIntRouting boolean to PCI_SYSTEM structure
		 added protos for pciAutoBusNumberSet, pciAutoDevReset
01a,19feb98,dat  written, from Motorola.
*/

#ifndef __INCpciAutoConfigLibh
#define __INCpciAutoConfigLibh

#ifdef __cplusplus
extern "C" {
#endif

/* PCI_LOC attributes */

#define PCI_AUTO_ATTR_DEV_EXCLUDE	(UINT8)(0x01) /* Exclude this device */
#define PCI_AUTO_ATTR_DEV_DISPLAY	(UINT8)(0x02) /* (S)VGA disp device  */
#define PCI_AUTO_ATTR_DEV_PREFETCH	(UINT8)(0x04) /* Device requests PF  */

#define PCI_AUTO_ATTR_BUS_PREFETCH	(UINT8)(0x08) /* Bridge implements   */
#define PCI_AUTO_ATTR_BUS_PCI		(UINT8)(0x10) /* PCI-PCI Bridge      */
#define PCI_AUTO_ATTR_BUS_HOST		(UINT8)(0x20) /* PCI Host Bridge     */
#define PCI_AUTO_ATTR_BUS_ISA		(UINT8)(0x40) /* PCI-ISA Bridge      */
#define PCI_AUTO_ATTR_BUS_4GB_IO	(UINT8)(0x80) /* 4G/64K IO Adresses  */


/* OPTION COMMANDS for use with pciAutoCfgCtl() */

/* 0 used for pSystem structure copy */
#define PCI_PSYSTEM_STRUCT_COPY		0x0000

/* 1-7 reserved for Fast Back To Back functions */
#define PCI_FBB_ENABLE			0x0001
#define PCI_FBB_DISABLE			0x0002
#define PCI_FBB_UPDATE			0x0003
#define PCI_FBB_STATUS_GET		0x0004

/* 8-11 reserved for MAX_LAT */
#define PCI_MAX_LATENCY_FUNC_SET	0x0008
#define PCI_MAX_LATENCY_ARG_SET		0x0009
#define PCI_MAX_LAT_ALL_SET		0x000a
#define PCI_MAX_LAT_ALL_GET		0x000b

/* 12-15 reserved for message output (logMsg) */
#define PCI_MSG_LOG_SET			0x000c

/* 16-47 reserved for pSystem functionality */
#define PCI_MAX_BUS_SET			0x0010
#define PCI_MAX_BUS_GET			0x0011
#define PCI_CACHE_SIZE_SET		0x0012
#define PCI_CACHE_SIZE_GET		0x0013
#define PCI_AUTO_INT_ROUTE_SET		0x0014
#define PCI_AUTO_INT_ROUTE_GET		0x0015
#define PCI_MEM32_LOC_SET		0x0016
#define PCI_MEM32_SIZE_SET		0x0017
#define PCI_MEMIO32_LOC_SET		0x0018
#define PCI_MEMIO32_SIZE_SET		0x0019
#define PCI_IO32_LOC_SET		0x001a
#define PCI_IO32_SIZE_SET		0x001b
#define PCI_IO16_LOC_SET		0x001c
#define PCI_IO16_SIZE_SET		0x001d
#define PCI_INCLUDE_FUNC_SET		0x001e
#define PCI_INT_ASSIGN_FUNC_SET		0x001f
#define PCI_BRIDGE_PRE_CONFIG_FUNC_SET	0x0020
#define PCI_BRIDGE_POST_CONFIG_FUNC_SET	0x0021
#define PCI_ROLLCALL_FUNC_SET		0x0022

/* 48-511 reserved for other memory configuration */
#define PCI_MEM32_SIZE_GET		0x0030
#define PCI_MEMIO32_SIZE_GET		0x0031
#define PCI_IO32_SIZE_GET		0x0032
#define PCI_IO16_SIZE_GET		0x0033

/* 512-0xffff available for misc items */
#define PCI_TEMP_SPACE_SET		0x0200
#define PCI_MINIMIZE_RESOURCES		0x0201

#ifndef _ASMLANGUAGE

typedef struct pciMemPtr
    {
    void *	pMem;
    int		memSize;
    } PCI_MEM_PTR;

typedef struct /* PCI_LOC, a standard bus location */
    {
    UINT8 bus;
    UINT8 device;
    UINT8 function;
    UINT8 attribute;
    UINT8 offset;       /* interrupt routing for this device */
    } PCI_LOC;

/* PCI identification structure */

typedef struct
    {
    PCI_LOC loc;
    UINT devVend;
    } PCI_ID;


/* obsolete structure */
typedef struct pciSystem /* PCI_SYSTEM, auto configuration info */
    {
    UINT pciMem32;		/* 32 bit prefetchable memory location */
    UINT pciMem32Size;		/* 32 bit prefetchable memory size */
    UINT pciMemIo32;		/* 32 bit non-prefetchable memory location */
    UINT pciMemIo32Size;	/* 32 bit non-prefetchable memory size */
    UINT pciIo32;		/* 32 bit io location */
    UINT pciIo32Size;		/* 32 bit io size */
    UINT pciIo16;		/* 16 bit io location */
    UINT pciIo16Size;		/* 16 bit io size */
    int maxBus;			/* Highest subbus number */
    int cacheSize;		/* cache line size */
    UINT maxLatency;		/* max latency */
    BOOL autoIntRouting;        /* automatic routing strategy */
    STATUS (* includeRtn)	/* returns OK to include */
	   (
	   struct pciSystem * pSystem,
	   PCI_LOC * pLoc,
	   UINT devVend
	   );
    UCHAR  (* intAssignRtn)	/* returns int line, given int pin */
	   (
	   struct pciSystem * pSystem,
	   PCI_LOC * pLoc,
	   UCHAR pin
	   );
    void (* bridgePreConfigInit) /* bridge pre-enumeration initialization */
	   (
	   struct pciSystem * pSystem,
	   PCI_LOC * pLoc,
	   UINT devVend
	   );
    void (* bridgePostConfigInit)/* bridge post-enumeration initialization */
	   (
	   struct pciSystem * pSystem,
	   PCI_LOC * pLoc,
	   UINT devVend
	   );
    STATUS (* pciRollcallRtn) ();  /* Roll call check */
    } PCI_SYSTEM;

typedef int (*PCI_LOGMSG_FUNC)(char *fmt, int i1, int i2, int i3, int i4, int i5, int i6);
typedef UINT8 (*PCI_MAX_LAT_FUNC)(int bus, int device, int func, void *pArg);
typedef UINT32 (*PCI_MEM_BUS_EXTRA_FUNC)(int bus, int device, int func, void *pArg);
typedef STATUS (*PCI_INCLUDE_FUNC)(PCI_SYSTEM *pSystem, PCI_LOC *pLoc, UINT devVend);
typedef UCHAR (*PCI_INT_ASSIGN_FUNC)(PCI_SYSTEM *pSystem, PCI_LOC *pLoc, UINT devVend);
typedef void (*PCI_BRIDGE_PRE_CONFIG_FUNC)(PCI_SYSTEM *pSystem, PCI_LOC *pLoc, UINT devVend);
typedef void (*PCI_BRIDGE_POST_CONFIG_FUNC)(PCI_SYSTEM *pSystem, PCI_LOC *pLoc, UINT devVend);
typedef STATUS (*PCI_ROLL_CALL_FUNC)();

void pciAutoConfig (PCI_SYSTEM *);
STATUS pciAutoGetNextClass ( PCI_SYSTEM *pSystem, PCI_LOC *pciFunc,
                             UINT *index, UINT pciClass, UINT mask);
STATUS pciAutoBusNumberSet ( PCI_LOC * pPciLoc, UINT primary, UINT secondary,
			    UINT subordinate);
STATUS pciAutoDevReset ( PCI_LOC * pPciLoc);
STATUS pciAutoAddrAlign ( UINT32 base, UINT32 limit, UINT32 reqSize,
			  UINT32 *pAlignedBase );
void pciAutoFuncEnable ( PCI_SYSTEM * pSystem, PCI_LOC * pFunc);
void pciAutoFuncDisable ( PCI_LOC *pPciFunc);
UINT pciAutoRegConfig ( PCI_SYSTEM * pSystem, PCI_LOC *pPciFunc,
    		UINT baseAddr, UINT nSize, UINT addrInfo);

void * pciAutoConfigLibInit(void * pArg);
STATUS pciAutoCfgCtl ( void *pCookie, int cmd, void *pArg );
STATUS pciAutoCfg( void *pCookie );

#endif	/* _ASMLANGUAGE */

#ifdef __cplusplus
}
#endif

#endif /* __INCpciAutoConfigLibh */
