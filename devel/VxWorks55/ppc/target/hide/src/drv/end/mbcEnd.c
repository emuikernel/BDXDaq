/* mbcEnd.c - Motorola 68302fads END network interface driver */

/* Copyright 1996-2000 Wind River Systems, Inc. */
#include "copyright_wrs.h"

/*
modification history
--------------------
01e,11jun00,ham  removed reference to etherLib.
01d,27jan00,dat  fixed use of NULL
01c,29mar99,dat  SPR 26119, documentation, usage of .bS/.bE
01b,13mar99,db   suppressed warnings. integrated driver(SPR #25386)
01a,28jan99,dra  written.
*/

/*
DESCRIPTION
This is a driver for the Ethernet controller on the 68EN302 chip.  The device
supports a 16-bit interface, data rates up to 10 Mbps, a dual-ported RAM, and
transparent DMA.  The dual-ported RAM is used for a 64-entry CAM table, and a
128-entry buffer descriptor table.  The CAM table is used to set the Ethernet
address of the Ethernet device or to program multicast addresses.  The buffer
descriptor table is partitioned into fixed-size transmit and receive tables.
The DMA operation is transparent and transfers data between the internal FIFOs
and external buffers pointed to by the receive and transmit-buffer descriptors
during transmits and receives.

The driver requires that the memory used for transmit and receive buffers be
allocated in cache-safe RAM area.  

Up to 61 multicast addresses are supported.  Multicast addresses are
supported by adding the multicast ethernet addresses to the address
table in the ethernet part.  If more than 61 multicast addresses are
desired, address hashing must be used (the address table holds 62
entries at most). However, address hashing does not appear to work in
this ethernet part.

A glitch in the EN302 Rev 0.1 device causes the Ethernet transmitter to
lock up from time to time. The driver uses a watchdog timer to reset the
Ethernet device when the device runs out of transmit buffers and cannot
recover within 20 clock ticks.

BOARD LAYOUT
This device is on-chip.  No jumpering diagram is necessary.  

EXTERNAL INTERFACE
The only external interface is the mbcEndLoad() routine, which expects
the <initString> parameter as input.  This parameter passes in a 
colon-delimited string of the format:

unit:memAddr:ivec:txBdNum:rxBdNum:dmaParms:bufBase:offset

TARGET-SPECIFIC PARAMETERS
.IP <unit>
A convenient holdover from the former model.  This parameter is used only
in the string name for the driver.
.IP <memAddr>
This parameter is the base address of the Ethernet module. The driver
addresses all other Ethernet device registers as offsets from this address.
.IP <ivec>
The interrupt vector to be used in connecting the interrupt handler.
.IP <txBdNum>
The number of transmit buffer descriptors to use.
.IP <rxBdNum>
The number of receive buffer descriptors to use.

The number of transmit and receive buffer descriptors (BDs) used is
configurable by the user while attaching the driver.  Each BD is 8
bytes in size and resides in the chip's dual-ported memory, while its
associated buffer, 1520 bytes in size, resides in cache-safe
conventional RAM. A minimum of 2 receive and 2 transmit BDs should be
allocated.  If this parameter is 0, a default of 32 BDs will be used.
The maximum number of BDs depends on how the dual-ported BD RAM is
partitioned.  The 128 BDs in the dual-ported BD RAM can partitioned
into transmit and receive BD regions with 8, 16, 32, or 64 transmit
BDs and corresponding 120, 112, 96, or 64 receive BDs.
.IP <dmaParms>
Ethernet DMA parameters.

This parameter is used to specify the settings of burst limit, water-mark, and
transmit early, which control the Ethernet DMA, and is used to set the EDMA
register.
.IP <bufBase>
Base address of the buffer pool.

This parameter is used to notify the driver that space for the
transmit and receive buffers need not be allocated, but should be
taken from a cache-coherent private memory space provided by the user
at the given address.  The user should be aware that memory used for
buffers must be 4-byte aligned and non-cacheable.  All the buffers
must fit in the given memory space; no checking will be performed.
Each buffer is 1520 bytes.  If this parameter is "NULL", space for
buffers will be obtained by calling cacheDmaMalloc() in mbcMemInit().
.IP <offset>
Specifies the memory alignment offset.

.LP

EXTERNAL SUPPORT REQUIREMENTS
This driver requires several external support functions, defined as macros:
.CS
    SYS_INT_CONNECT(pDrvCtrl, routine, arg)
    SYS_INT_DISCONNECT (pDrvCtrl, routine, arg)
    SYS_INT_ENABLE(pDrvCtrl)
    SYS_OUT_SHORT(pDrvCtrl, reg, data)
    SYS_IN_SHORT(pDrvCtrl, reg, pData)
.CE

There are default values in the source code for these macros.  They presume
memory-mapped accesses to the device registers and the normal intConnect(),
and intEnable() BSP functions.  The first argument to each is the device
controller structure. Thus, each has access back to all the device-specific
information.  Having the pointer in the macro facilitates the addition 
of new features to this driver.

SYSTEM RESOURCE USAGE
The driver requires the following system resources:

    - one watchdog timer
    - one interrupt vector
    - 52 bytes in the initialized data section (data)
    - 0 bytes in the uninitialized data section (bss)

The above data and BSS requirements are for the MC68000 architecture
and can vary for other architectures.  Code size (text) varies greatly 
between architectures and is therefore not quoted here.

If the driver allocates the memory shared with the Ethernet device
unit, it does so by calling the cacheDmaMalloc() routine.  For the
default case of 32 transmit buffers, 32 receive buffers, the total
size requested is roughly 100,000 bytes.  If a memory region is
provided to the driver, the size of this region is adjustable to suit
user needs.

This driver can only operate if the shared memory region is non-cacheable,
or if the hardware implements bus snooping.  The driver cannot maintain
cache coherency for the device because the buffers are asynchronously
modified by both the driver and the device, and these fields may share the
same cache line.  Additionally, the chip's dual-ported RAM must be
declared as non-cacheable memory where applicable.

INCLUDES:
end.h endLib.h etherMultiLib.h

SEE ALSO: muxLib, endLib
.I "Writing and Enhanced Network Driver"
*/

/* includes */

#include "vxWorks.h"
#include "stdlib.h"
#include "cacheLib.h"
#include "intLib.h"
#include "end.h"			/* Common END structures. */
#include "endLib.h"
#include "lstLib.h"			/* Needed to maintain protocol list. */
#include "wdLib.h"
#include "iv.h"
#include "semLib.h"
#include "logLib.h"
#include "netLib.h"
#include "stdio.h"
#include "sysLib.h"
#include "errno.h"
#include "errnoLib.h"
#include "memLib.h"
#include "iosLib.h"
#undef	ETHER_MAP_IP_MULTICAST
#include "etherMultiLib.h"		/* multicast stuff. */

#include "net/mbuf.h"
#include "net/unixLib.h"
#include "net/protosw.h"
#include "net/systm.h"
#include "net/if_subr.h"
#include "net/route.h"

#include "sys/socket.h"
#include "sys/ioctl.h"
#include "sys/times.h"
#include "drv/end/mbcEnd.h"

/* defines */

#define FRAME_MAX	1518	/* maximum frame size */
#define FRAME_MIN	64	/* minimum frame size */
#define ENET_ADDR_SIZE	6	/* size of ethernet src/dst addresses */
#define TX_BD_MIN	2	/* minimum number of Tx buffer descriptors */
#define RX_BD_MIN	2	/* minimum number of Rx buffer descriptors */
#define TX_BD_DEFAULT	0x20	/* default number of Tx buffer descriptors */
#define RX_BD_DEFAULT	0x20	/* default number of Rx buffer descriptors */
#define WD_TIMEOUT	20	/* number of clock ticks for reset timeout */

#define MBC_BUFSIZ      (1520)
#define MBC_MIN_FBUF	(1520)		/* min first buffer size */
#define EH_SIZE		(14)
#define MBC_SPEED_10M	10000000	/* 10Mbs */
#define MBC_SPEED	MBC_SPEED_10M

/* Cache macros */

#define MBC_CACHE_INVALIDATE(address, len) \
        CACHE_DRV_INVALIDATE (&pDrvCtrl->cacheFuncs, (address), (len))

#define MBC_CACHE_PHYS_TO_VIRT(address) \
        CACHE_DRV_PHYS_TO_VIRT (&pDrvCtrl->cacheFuncs, (address))

#define MBC_CACHE_VIRT_TO_PHYS(address) \
        CACHE_DRV_VIRT_TO_PHYS (&pDrvCtrl->cacheFuncs, (address))

/*
 * Default macro definitions for BSP interface.
 * These macros can be redefined in a wrapper file, to generate
 * a new module with an optimized interface.
 */

/* Macro to connect interrupt handler to vector */

#ifndef SYS_INT_CONNECT
#define SYS_INT_CONNECT(pDrvCtrl,rtn,arg,pResult) \
    { \
    IMPORT STATUS sysIntConnect(); \
    *pResult = intConnect ((VOIDFUNCPTR *)INUM_TO_IVEC ((pDrvCtrl)->ivec), \
			   rtn, (int)(arg)); \
    }
#endif

/* Macro to disconnect interrupt handler from vector */

LOCAL void dummyIsr (void) { };

#ifndef SYS_INT_DISCONNECT
#define SYS_INT_DISCONNECT(pDrvCtrl,rtn,arg,pResult) \
    { \
    IMPORT STATUS intConnect(); \
    *pResult = intConnect ((VOIDFUNCPTR *)INUM_TO_IVEC ((pDrvCtrl)->ivec), \
			    dummyIsr, (int)(arg)); \
    }
#endif

/* Macro to get the ethernet address from the BSP */

#ifndef SYS_ENET_ADDR_GET
#define SYS_ENET_ADDR_GET(pAddress) \
    { \
    sysEnetAddrGet (pDrvCtrl->unit, (char *) (pAddress)); \
    }
#endif /* SYS_ENET_ADDR_GET */

/*
 * Macros to do a short (UINT16) access to the chip. Default
 * assumes a normal memory mapped device.
 */

#ifndef SYS_OUT_SHORT
#define SYS_OUT_SHORT(pDrvCtrl, addr, value) \
	(*(volatile UINT16 *)((pDrvCtrl)->memAddr + (addr)) = (value))
#endif

#ifndef SYS_IN_SHORT
#define SYS_IN_SHORT(pDrvCtrl, addr, pData) \
	(*(pData) = (*(volatile UINT16 *)((pDrvCtrl)->memAddr + (addr))))
#endif

#ifndef SYS_UPDATE_SHORT
#define SYS_UPDATE_SHORT(pDrvCtrl, addr, val) \
	((*(volatile UINT16 *)((pDrvCtrl)->memAddr + (addr))) |= (val))
#endif

#ifndef SYS_RESET_SHORT
#define SYS_RESET_SHORT(pDrvCtrl, addr, val) \
	((*(volatile UINT16 *)((pDrvCtrl)->memAddr + (addr))) &= ~(val))
#endif
		    
/* A shortcut for getting the hardware address from the MIB II stuff. */

#define END_HADDR(pEnd)	\
		((pEnd)->mib2Tbl.ifPhysAddress.phyAddress)

#define END_HADDR_LEN(pEnd) \
		((pEnd)->mib2Tbl.ifPhysAddress.addrLength)

/* Definitions for the flags field */

#define MBC_POLLING		0x1
#define MBC_MEM_ALLOC_FLAG      0x2

/* DEBUG MACROS */

#undef DRV_DEBUG
#ifdef	DRV_DEBUG
#define DRV_DEBUG_OFF		0x0000
#define DRV_DEBUG_RX		0x0001
#define	DRV_DEBUG_TX		0x0002
#define DRV_DEBUG_INT		0x0004
#define	DRV_DEBUG_POLL		(DRV_DEBUG_POLL_RX | DRV_DEBUG_POLL_TX)
#define	DRV_DEBUG_POLL_RX	0x0008
#define	DRV_DEBUG_POLL_TX	0x0010
#define	DRV_DEBUG_LOAD		0x0020
#define	DRV_DEBUG_IOCTL		0x0040
#define	DRV_DEBUG_MULTI		0x0080
#define	DRV_DEBUG_ERROR		0x0100

int	mbcDebug = DRV_DEBUG_ERROR;

#define DRV_LOG(FLG, X0, X1, X2, X3, X4, X5, X6)		\
    do								\
        {							\
	if (mbcDebug & (FLG))					\
	    logMsg (X0, X1, X2, X3, X4, X5, X6);		\
	}							\
    while (0)

#define DRV_PRINT(FLG, X)					\
    do								\
        {							\
	if (mbcDebug & (FLG))					\
	    printf X;						\
	}							\
    while (0)

#else /* DRV_DEBUG */
    
#define DRV_LOG(FLG, X0, X1, X2, X3, X4, X5, X6)	do {} while (0)
#define DRV_PRINT(FLG, X)				do {} while (0)
    
#endif /* DRV_DEBUG */
	
/* typedefs */

typedef struct free_args
    {
    void *	arg1;
    void *	arg2;
    } FREE_ARGS;
        
/* The definition of the driver control structure */
typedef struct MBC_DEVICE
    {
    END_OBJ     end;			/* The class we inherit from. */
    int		unit;			/* unit number */
    int         ivec;                   /* interrupt vector */
    WDOG_ID	wdId;			/* watchdog id */
    UCHAR *	memAddr;		/* address of mbc's shared memory */
    int		txBdNum;		/* number of tx buffer descriptors */
    int		rxBdNum;		/* number of rx buffer descriptors */
    int		dmaParms;		/* DMA parameters */
    int         offset;			/* packet data offset */
    UCHAR	enetAddr[6];		/* ethernet address */    
    ULONG	flags;			/* Our local flags. */
    BOOL	txBlocked;		/* TX is blocked */
    BOOL	txCleaning;		/* TX is being cleaned */
    BOOL	rxHandling;		/* rcv task is scheduled */
    char *	bufBase;		/* address of memory pool; */
    int		bdSize;			/* buffer descriptor size */
    MBC_BD *	txBdBase;		/* transmit BD array address */
    MBC_BD *	rxBdBase;		/* receive BD array address */
    int		txBdNext;		/* next transmit BD to fill */
    int		txClNext;		/* next transmit BD to clean */
    int		rxBdNext;		/* next receive BD to read */
    int		resetCounter;		/* number of times the device reset */
    CACHE_FUNCS cacheFuncs;             /* cache function pointers */
    UCHAR	mcastFilter[8];		/* multicast address mask */
    FUNCPTR     freeRtn[128];           /* Array of free routines. */
    struct free_args    freeData[128];  /* Array of free arguments */
                                        /* to the free routines. */
    CL_POOL_ID	pClPoolId;		/* cluster pool */
    } MBC_DEVICE;

/* globals */

IMPORT STATUS	sysEnetAddrGet (int unit, char * addr);
IMPORT int	endMultiLstCnt (END_OBJ * pEnd);


/* locals */

/* forward static functions */

LOCAL void	mbcReset	(MBC_DEVICE * pDrvCtrl);
LOCAL void	mbcInt		(MBC_DEVICE * pDrvCtrl);
LOCAL void	mbcHandleRcvInt (MBC_DEVICE * pDrvCtrl);
LOCAL STATUS	mbcRecv		(MBC_DEVICE * pDrvCtrl, char * pData);
LOCAL void	mbcConfig	(MBC_DEVICE * pDrvCtrl);
LOCAL STATUS	mbcInit		(MBC_DEVICE * pDrvCtrl);
LOCAL void	mbcAddrFilterSet(MBC_DEVICE * pDrvCtrl);
LOCAL void	mbcScrubTRing	(MBC_DEVICE * pDrvCtrl);
LOCAL STATUS	mbcPollStart	(MBC_DEVICE * pDrvCtrl);
LOCAL STATUS	mbcPollStop	(MBC_DEVICE * pDrvCtrl);
LOCAL void	mbcDeviceRestart(MBC_DEVICE * pDrvCtrl);
LOCAL STATUS	mbcParse	(MBC_DEVICE * pDrvCtrl, char * initString);
LOCAL STATUS	mbcMemInit	(MBC_DEVICE * pDrvCtrl);

/* END Specific interfaces. */

/* This is the only externally visible interface. */

END_OBJ * 	mbcEndLoad	(char * initString);

LOCAL STATUS	mbcStart	(void * pDrvCtrl);
LOCAL STATUS	mbcStop		(void * pDrvCtrl);
LOCAL STATUS	mbcUnload	(void * pDrvCtrl);
LOCAL int	mbcIoctl	(void * pDrvCtrl, int cmd, caddr_t data);
LOCAL STATUS	mbcSend		(void * pDrvCtrl, M_BLK_ID pBuf);
LOCAL STATUS	mbcMCastAdd	(void * pDrvCtrl, char * pAddress);
LOCAL STATUS	mbcMCastDel	(void * pDrvCtrl, char * pAddress);
LOCAL STATUS	mbcMCastGet	(void * pDrvCtrl, MULTI_TABLE * pTable);
LOCAL STATUS	mbcPollSend	(void * pDrvCtrl, M_BLK_ID pBuf);
LOCAL STATUS	mbcPollRcv	(void * pDrvCtrl, M_BLK_ID pBuf);

/*
 * Declare our function table.  This is static across all driver
 * instances.
 */

LOCAL NET_FUNCS mbcFuncTable =
    {
    (FUNCPTR) mbcStart,	   /* Function to start the device. */
    (FUNCPTR) mbcStop,	   /* Function to stop the device. */
    (FUNCPTR) mbcUnload,   /* Unloading function for the driver. */
    (FUNCPTR) mbcIoctl,	   /* Ioctl function for the driver. */
    (FUNCPTR) mbcSend,	   /* Send function for the driver. */
    (FUNCPTR) mbcMCastAdd, /* Multicast add function for the  driver*/
    (FUNCPTR) mbcMCastDel, /* Multicast delete function for driver */
    (FUNCPTR) mbcMCastGet, /* Multicast retrieve function for driver*/
    (FUNCPTR) mbcPollSend, /* Polling send function */
    (FUNCPTR) mbcPollRcv,  /* Polling receive function */
    endEtherAddressForm,   /* put address info into a NET_BUFFER */
    endEtherPacketDataGet, /* get pointer to data in NET_BUFFER */
    endEtherPacketAddrGet  /* Get packet addresses. */
    };

/******************************************************************************
*
* mbcEndLoad - initialize the driver and device
*
* This routine initializes the driver and the device to the operational state.
* All of the device specific parameters are passed in the initString.
*
* The string contains the target specific parameters like this:
*
* "unit:memAddr:ivec:txBdNum:rxBdNum:dmaParms:bufBase:offset"
* 
* RETURNS: An END object pointer or NULL on error.
*/

END_OBJ* mbcEndLoad
    (
    char *	initString	/* String to be parsed by the driver. */
    )
    {
    MBC_DEVICE *	pDrvCtrl;
      
    if (initString == NULL)
        return (NULL);
    
    if (initString[0] == 0)
        {
        bcopy ((char *)DEV_NAME, initString, DEV_NAME_LEN);
        return (NULL);
        }
 
    DRV_LOG (DRV_DEBUG_LOAD, "Loading mbc...\n", 1, 2, 3, 4, 5, 6);
   
    /* allocate the device structure */

    pDrvCtrl = (MBC_DEVICE *) calloc (sizeof (MBC_DEVICE), 1);
    if (pDrvCtrl == NULL)
	goto errorExit;
          
    /* parse the init string, filling in the device structure */

    if (mbcParse (pDrvCtrl, initString) == ERROR)
	goto errorExit;

    /* Ask the BSP to provide the ethernet address. */
    
    SYS_ENET_ADDR_GET (&pDrvCtrl->enetAddr);
   
    /* Allocate a watchdog timer */

    pDrvCtrl->wdId = wdCreate ();
    if (pDrvCtrl->wdId == NULL)
	goto errorExit;

    /* initialize the END and MIB2 parts of the structure */

    /*
     * The M2 element must come from m2Lib.h 
     * This mbcis set up for a DIX type ethernet device.
     */
    
    if (END_OBJ_INIT (&pDrvCtrl->end, (DEV_OBJ *)pDrvCtrl, DEV_NAME,
                      pDrvCtrl->unit, &mbcFuncTable,
                      "END mbcDriver.") == ERROR)
	goto errorExit;
    if (END_MIB_INIT (&pDrvCtrl->end, M2_ifType_ethernet_csmacd,
                      &pDrvCtrl->enetAddr[0], 6, ETHERMTU,
                      MBC_SPEED) == ERROR)
	goto errorExit;

    /* Perform memory allocation/distribution */

    if (mbcMemInit (pDrvCtrl) == ERROR)
	goto errorExit;

    /* reset and reconfigure the device */

    mbcReset (pDrvCtrl);
    mbcInit (pDrvCtrl);
    mbcConfig (pDrvCtrl);

    /* set the flags to indicate readiness */

    END_OBJ_READY (&pDrvCtrl->end,
		   IFF_UP | IFF_RUNNING | IFF_NOTRAILERS | IFF_BROADCAST
		   | IFF_MULTICAST);

    DRV_LOG (DRV_DEBUG_RX, "%s%d: mbcEndLoad: offset=%d\n",
	     (int) DEV_NAME, pDrvCtrl->unit, pDrvCtrl->offset, 0, 0, 0);

    return (&pDrvCtrl->end);

errorExit:
    if (pDrvCtrl != NULL)
	{
	if (pDrvCtrl->wdId)
	    wdDelete (pDrvCtrl->wdId);
	free ((char *)pDrvCtrl);
	}

    return (NULL);
    }

/******************************************************************************
*
* mbcParse - parse the init string
*
* Parse the input string.  Fill in values in the driver control structure.
*
* The initialization string format is:
* .CS
*    "unit:memAddr:ivec:txBdNum:rxBdNum:dmaParms:bufBase:offset"
* .CE
*
* .IP <unit>
* Device unit number, a small integer.
* .IP <memAddr>
* ethernet module base address.
* .IP <ivec>
* Interrupt vector number (used with sysIntConnect)
* .IP <txBdNum>
* transmit buffer descriptor
* .IP <rxBdNum>
* receive buffer descriptor
* .IP <dmaParms>
* dma parameters
* .IP <bufBase>
* address of memory pool
* .IP <offset>
* packet data offset 
* .LP
*
* RETURNS: OK or ERROR for invalid arguments.
*/

STATUS mbcParse
    (
    MBC_DEVICE *	pDrvCtrl,	/* device pointer */
    char *		initString	/* information string */
    )
    {
    char *	tok;
    char *	pHolder = NULL;
    
    /* Parse the initString */

    /* Unit number. */

    tok = strtok_r (initString, ":", &pHolder);
    if (tok == NULL)
	return (ERROR);
    pDrvCtrl->unit = strtoul (tok, NULL, 16);

    /* ethernet module base address */

    tok = strtok_r (NULL, ":", &pHolder);
    if (tok == NULL)
	return (ERROR);
    pDrvCtrl->memAddr = (void *) strtoul (tok, NULL, 16);
    
    /* Interrupt vector. */

    tok = strtok_r (NULL, ":", &pHolder);
    if (tok == NULL)
	return (ERROR);
    pDrvCtrl->ivec = strtoul (tok, NULL, 16);

    /* number of transmit buffer descriptors */

    tok = strtok_r (NULL, ":", &pHolder);
    if (tok == NULL)
	return (ERROR);
    pDrvCtrl->txBdNum = strtoul (tok, NULL, 16);

    /* number of receive buffer descriptors */

    tok = strtok_r (NULL, ":", &pHolder);
    if (tok == NULL)
	return (ERROR);
    pDrvCtrl->rxBdNum = strtoul (tok, NULL, 16);

    /* DMA parameters */

    tok = strtok_r (NULL, ":", &pHolder);
    if (tok == NULL)
	return (ERROR);
    pDrvCtrl->dmaParms = strtoul (tok, NULL, 16);

    /* address of memory pool; NONE = malloc it */

    tok = strtok_r (NULL, ":", &pHolder);
    if (tok == NULL)
	return (ERROR);
    pDrvCtrl->bufBase = (void *) strtoul (tok, NULL, 16);

    /* packet data offset */

    tok = strtok_r (NULL, ":", &pHolder);
    if (tok == NULL)
	return (ERROR);
    pDrvCtrl->offset = strtoul (tok, NULL, 16);

    return (OK);
    }

/******************************************************************************
*
* mbcMemInit - initialize memory for the chip
*
* Allocates and initializes the memory pools for the mbc device.
*
* RETURNS: OK or ERROR.
*/

STATUS mbcMemInit
    (
    MBC_DEVICE *	pDrvCtrl	/* device to be initialized */
    )
    {
    MBC_BD *		pBd;
    int			counter;
    int			bdSize;
    int			txBdMax;
    int			rxBdMax;
    M_CL_CONFIG		mbcMclBlkConfig;
    CL_DESC		mbcClDesc;

    /* Setup the netPool */
    
    if ((pDrvCtrl->end.pNetPool = malloc (sizeof(NET_POOL))) == NULL)
        return (ERROR);

    /* Use default values if the parameters are NULL */

    if (pDrvCtrl->txBdNum == 0)
        pDrvCtrl->txBdNum = TX_BD_DEFAULT;
    if (pDrvCtrl->rxBdNum == 0)
        pDrvCtrl->rxBdNum = RX_BD_DEFAULT;

    /* Use the default minimum value if the given value is smaller */

    pDrvCtrl->txBdNum = max (TX_BD_MIN, pDrvCtrl->txBdNum);
    pDrvCtrl->rxBdNum = max (RX_BD_MIN, pDrvCtrl->rxBdNum);
    
    /* Ensure that txBdNum, and rxBdNum are within range */
    
    if (pDrvCtrl->txBdNum > 64)
        pDrvCtrl->txBdNum = 64;

    for (bdSize = MBC_EDMA_BDS_8; bdSize <= MBC_EDMA_BDS_64; bdSize++) 
	{
	txBdMax = 1 << (bdSize + 3);   /* set txBdMax to (8, 16, 32, 64) */
        rxBdMax = MBC_BD_MAX - txBdMax;
        
        if (pDrvCtrl->txBdNum <= txBdMax)
            {
            if (pDrvCtrl->rxBdNum > rxBdMax)
                pDrvCtrl->rxBdNum = rxBdMax;
            break;
            }
        }

    /* fill in the driver control parameters */
    
    pDrvCtrl->bdSize = bdSize;
    pDrvCtrl->txBdBase = (MBC_BD *) ((int) pDrvCtrl->memAddr + MBC_EBD);
    pDrvCtrl->rxBdBase = (MBC_BD *) ((int) pDrvCtrl->memAddr + MBC_EBD
				     + sizeof (MBC_BD) * (1 << (bdSize + 3)));
    pDrvCtrl->txBdNext = 0;
    pDrvCtrl->rxBdNext = 0;
    pDrvCtrl->resetCounter = 0;

    DRV_LOG (DRV_DEBUG_LOAD,
	     "%s%d: rxBdNum=%d txBdNum=%d rxBdBase=%x txBdBase=%x\n",
	     (int) DEV_NAME, pDrvCtrl->unit,
	     pDrvCtrl->rxBdNum, pDrvCtrl->txBdNum,
	     (int) pDrvCtrl->rxBdBase, (int) pDrvCtrl->txBdBase);

    mbcClDesc.clNum = 4 * (pDrvCtrl->rxBdNum + pDrvCtrl->txBdNum);
    mbcClDesc.clSize = MEM_ROUND_UP (MBC_BUFSIZ + pDrvCtrl->offset);
    mbcClDesc.memSize = (mbcClDesc.clNum * (mbcClDesc.clSize + 4)) + 4;

    /* Calculate the total memory for all the M-Blks and CL-Blks. */

    mbcMclBlkConfig.mBlkNum = mbcClDesc.clNum;
    mbcMclBlkConfig.clBlkNum = mbcClDesc.clNum;
    mbcMclBlkConfig.memSize =
      (mbcMclBlkConfig.mBlkNum * (MSIZE + sizeof (long)))
      + (mbcMclBlkConfig.clBlkNum * (CL_BLK_SZ + sizeof(long)));
    mbcMclBlkConfig.memArea = (char *) memalign (sizeof(long),
						 mbcMclBlkConfig.memSize);
    if (mbcMclBlkConfig.memArea == NULL)
	{
	free (pDrvCtrl->end.pNetPool);
	return (ERROR);
	}
    
    /* Allocate the memory for the clusters from cache safe memory. */
    
    if (pDrvCtrl->bufBase == NULL)
	{
	/* Get our own memory */
	pDrvCtrl->cacheFuncs = cacheDmaFuncs;
	pDrvCtrl->bufBase = (char *) cacheDmaMalloc (mbcClDesc.memSize);
	pDrvCtrl->flags |= MBC_MEM_ALLOC_FLAG;
	if (pDrvCtrl->bufBase == NULL)
            {
	    DRV_LOG (DRV_DEBUG_LOAD | DRV_DEBUG_ERROR,
		     "%s%d: system memory unavailable (%d bytes)\n",
		     (int) DEV_NAME, pDrvCtrl->unit, mbcClDesc.memSize,
		     4, 5, 6);
	    free (pDrvCtrl->end.pNetPool);
	    free (mbcMclBlkConfig.memArea);
	    return (ERROR);
	    }
	}
    else
	{
	/* Caller provided memory */
	pDrvCtrl->cacheFuncs = cacheNullFuncs;
	}
   
    mbcClDesc.memArea = pDrvCtrl->bufBase;
    
    /* Initialize the memory pool. */

    if (netPoolInit(pDrvCtrl->end.pNetPool, &mbcMclBlkConfig,
                    &mbcClDesc, 1, NULL) == ERROR)
        {
        DRV_LOG (DRV_DEBUG_LOAD | DRV_DEBUG_ERROR,
		 "Could not init buffering\n",
		 1, 2, 3, 4, 5, 6);
	free (pDrvCtrl->end.pNetPool);
	free (mbcMclBlkConfig.memArea);
	if (pDrvCtrl->flags & MBC_MEM_ALLOC_FLAG)
	    cacheDmaFree (pDrvCtrl->bufBase);
	return (ERROR);
        }

    /*
     * If you need clusters to store received packets into then get them
     * here ahead of time.
     */
    pDrvCtrl->pClPoolId = netClPoolIdGet (pDrvCtrl->end.pNetPool,
					  MBC_BUFSIZ, FALSE);
    if (pDrvCtrl->pClPoolId == NULL)
	{
	free (pDrvCtrl->end.pNetPool);
	free (mbcMclBlkConfig.memArea);
	if (pDrvCtrl->flags & MBC_MEM_ALLOC_FLAG)
	    cacheDmaFree (pDrvCtrl->bufBase);
	return (ERROR);
	}
    
    /* take the device out of reset state */

    SYS_OUT_SHORT (pDrvCtrl, MBC_ECNTL, MBC_ECNTL_RES_OFF);

    /* set up transmit buffer descriptors, setting 'wrap' bit on last */
    
    pDrvCtrl->txBdNext = 0;
    pDrvCtrl->txClNext = 0;
    pBd = pDrvCtrl->txBdBase;
    for (counter = pDrvCtrl->txBdNum; counter; counter--, pBd++)
	{
        pBd->statusMode = (MBC_TXBD_I | MBC_TXBD_L | MBC_TXBD_TC);
	pBd->dataPointer = NULL;
	}
    pBd--;
    pBd->statusMode |= MBC_TXBD_W;

    /* set up the receive buffer descriptors, setting 'wrap' bit on last */

    pDrvCtrl->rxBdNext = 0;
    pBd = pDrvCtrl->rxBdBase;
    for (counter = pDrvCtrl->rxBdNum; counter; counter--, pBd++)
	{
        pBd->statusMode = MBC_RXBD_I;
	pBd->dataPointer = NULL;
	}
    pBd--;
    pBd->statusMode |= MBC_RXBD_W;

    DRV_LOG (DRV_DEBUG_LOAD, "%s%d: Memory setup complete\n",
	     (int) DEV_NAME, pDrvCtrl->unit, 3, 4, 5, 6);
      
    return (OK);
    }

/******************************************************************************
*
* mbcInit - initialize the MBC network device
* 
* Mbc initializes registers, and enables interrupts.
*
* The complement of this routine is mbcReset().  Once a unit is reset by
* mbcReset(), it may be re-initialized to a running state by this routine.
*
* RETURNS: OK or ERROR.
*
* SEE ALSO: mbcIoctl(), mbcReset()
*
*/

LOCAL STATUS mbcInit
    (
    MBC_DEVICE *	pDrvCtrl
    )
    {
    UINT16	regValue;
    MBC_BD *	pBd;
    int		counter;

    DRV_LOG (DRV_DEBUG_LOAD, "%s%d: mbcInit\n",
	     (int) DEV_NAME, pDrvCtrl->unit, 3, 4, 5, 6);

    /* take the device out of reset state */

    SYS_OUT_SHORT (pDrvCtrl, MBC_ECNTL, MBC_ECNTL_RES_OFF);

    /*
     * set up transmit buffer descriptors, setting 'wrap' bit on last.
     * clean up any transmit buffers left behind.
     */
    
    pDrvCtrl->txBdNext = 0;
    pDrvCtrl->txClNext = 0;
    pBd = pDrvCtrl->txBdBase;
    for (counter = pDrvCtrl->txBdNum; counter; counter--, pBd++)
	{
	if ((pBd->dataPointer != NULL) && (pDrvCtrl->freeRtn[counter] != NULL))
	    pDrvCtrl->freeRtn[counter] (pDrvCtrl->freeData[counter].arg1,
					pDrvCtrl->freeData[counter].arg2); 
	pDrvCtrl->freeRtn[counter] = NULL;
	pDrvCtrl->freeData[counter].arg1 = NULL; 
	pDrvCtrl->freeData[counter].arg2 = NULL; 
	pBd->dataPointer = NULL;
        pBd->statusMode = (MBC_TXBD_I | MBC_TXBD_L | MBC_TXBD_TC);
	}
    pBd--;
    pBd->statusMode |= MBC_TXBD_W;

    /*
     * set up the receive buffer descriptors, setting 'wrap' bit on last.
     * Free up any old receive clusters left behind.
     */

    pDrvCtrl->rxBdNext = 0;
    pBd = pDrvCtrl->rxBdBase;
    for (counter = pDrvCtrl->rxBdNum; counter; counter--, pBd++)
	{
	char * pBuf;

	if (pBd->dataPointer != NULL)
	    netClFree (pDrvCtrl->end.pNetPool,
		       (pBd->dataPointer - pDrvCtrl->offset));
	pBuf = netClusterGet (pDrvCtrl->end.pNetPool, pDrvCtrl->pClPoolId);
	if (pBuf == NULL)
	    {
            DRV_LOG (DRV_DEBUG_LOAD | DRV_DEBUG_ERROR,
		     "Could not get a buffer\n", 1, 2, 3, 4, 5, 6);
            return (ERROR);
	    }
        pBd->statusMode = (MBC_RXBD_E | MBC_RXBD_I);
	pBd->dataPointer = pBuf + pDrvCtrl->offset;
	}
    pBd--;
    pBd->statusMode |= MBC_RXBD_W;

    /* set the interrupt vector number */

    SYS_OUT_SHORT (pDrvCtrl, MBC_IVEC, pDrvCtrl->ivec);

    /* set BD size and other DMA parameters */

    regValue = pDrvCtrl->dmaParms | (pDrvCtrl->bdSize << MBC_EDMA_BDS_SHFT);
    SYS_OUT_SHORT (pDrvCtrl, MBC_EDMA, regValue);

    /*
     * set the operating mode to, no internal loopback, no full duplex, and
     * no hearbeat control.
     */
    
    SYS_OUT_SHORT (pDrvCtrl, MBC_ECFG, 0);

    /*
     * set address control to, no hash enable, no index enable, no multicast,
     * no physical address rejection.
     */
    
    SYS_OUT_SHORT (pDrvCtrl, MBC_AR, 0x00);

    /* set max receive frame size */

    SYS_OUT_SHORT (pDrvCtrl, MBC_EMRBLR, FRAME_MAX);
        
    return (OK);
    }

/******************************************************************************
*
* mbcDeviceRestart - restart transmits
*
* This routine re-resets the Ethernet device. Since on a transmit lock
* up, the transmitter can be restarted only by resetting the device, this
* routine resets, and reinitializes the device.  The device reset counter will
* be updated on each reset.
*
* RETURNS: N/A
*
* SEE ALSO: mbcInt(), mbcSend(), mbcPacketGet()
*
* NOMANUAL
*/

LOCAL void mbcDeviceRestart
    (
    MBC_DEVICE *	pDrvCtrl	
    )
    {
    mbcReset (pDrvCtrl);
    mbcInit (pDrvCtrl);
    }

/******************************************************************************
*
* mbcConfig - reconfigure the interface under us.
*
* Reconfigure the interface setting promiscuous mode, and changing the
* multicast interface list.
*
* RETURNS: N/A.
*
* NOMANUAL
*/

LOCAL void mbcConfig
    (
    MBC_DEVICE *	pDrvCtrl	/* device to be re-configured */
    )
    {
    int		i;
    int		numMulti;	/* Number of multicast addresses */
    UINT16	regValue;

    /* Determine the number of multicast addresses */
	
    numMulti = END_MULTI_LST_CNT (&pDrvCtrl->end);

    DRV_LOG (DRV_DEBUG_LOAD, "%s%d: mbcConfig: %d multicast addresses\n",
	     (int) DEV_NAME, pDrvCtrl->unit, numMulti, 4, 5, 6);

    /* install the ethernet address */
    
    bcopy ((char *) pDrvCtrl->enetAddr,
           (char *) (pDrvCtrl->memAddr + MBC_ARTAB),
           ENET_ADDR_SIZE);
    
    /*
     * Setup address control w/o multicasting enabled, and write
     * the entire address arrray.
     */
    
    regValue = MBC_AR_MULTI_00;
    if (END_FLAGS_GET(&pDrvCtrl->end) & IFF_PROMISC)
        regValue |= MBC_AR_PROM;
    SYS_OUT_SHORT (pDrvCtrl, MBC_AR, regValue);

    for (i = 1; i < MBC_ARTAB_SIZE; i++)
        {
        SYS_OUT_SHORT (pDrvCtrl, MBC_ARTAB + (i * 8), ~0);
        SYS_OUT_SHORT (pDrvCtrl, MBC_ARTAB + (i * 8) + 2, ~0);
        SYS_OUT_SHORT (pDrvCtrl, MBC_ARTAB + (i * 8) + 4, ~0);
        }
    
    /* Now install the multicast addresses or filter */

    if ((numMulti > 0) && (numMulti <= MBC_ARTAB_SIZE-1))
	{
	ETHER_MULTI * pCurr;

	/* Install the multicast addresses directly in the table */

	for (i = 1, pCurr = END_MULTI_LST_FIRST (&pDrvCtrl->end);
	     pCurr != NULL;
	     pCurr = END_MULTI_LST_NEXT(pCurr), ++i)
	    {
	    bcopy ((char *) &pCurr->addr,
		   (char *) (pDrvCtrl->memAddr + MBC_ARTAB + i*8),
		   ENET_ADDR_SIZE);
	    }
	}
    else if (numMulti > MBC_ARTAB_SIZE-1)
	{
	/* Setup the multicast filter */

	mbcAddrFilterSet (pDrvCtrl);
	
	DRV_LOG (DRV_DEBUG_LOAD | DRV_DEBUG_MULTI,
		 "Multicast Filter 0-3 %02x:%02x:%02x:%02x\n",
		 pDrvCtrl->mcastFilter[0], pDrvCtrl->mcastFilter[1],
		 pDrvCtrl->mcastFilter[2], pDrvCtrl->mcastFilter[3], 0, 0);
	DRV_LOG (DRV_DEBUG_LOAD | DRV_DEBUG_MULTI,
		 "Multicast Filter 4-7 %02x:%02x:%02x:%02x\n",
		 pDrvCtrl->mcastFilter[4], pDrvCtrl->mcastFilter[5],
		 pDrvCtrl->mcastFilter[6], pDrvCtrl->mcastFilter[7], 0, 0);

	/* Write out multicast filter */

        SYS_OUT_SHORT (pDrvCtrl, MBC_ARTAB + 0x01f0,
		       (pDrvCtrl->mcastFilter[0] << 8)
		       | pDrvCtrl->mcastFilter[1]);
        SYS_OUT_SHORT (pDrvCtrl, MBC_ARTAB + 0x01f2,
		       (pDrvCtrl->mcastFilter[2] << 8)
		       | pDrvCtrl->mcastFilter[3]);
        SYS_OUT_SHORT (pDrvCtrl, MBC_ARTAB + 0x01f8,
		       (pDrvCtrl->mcastFilter[4] << 8)
		       | pDrvCtrl->mcastFilter[5]);
        SYS_OUT_SHORT (pDrvCtrl, MBC_ARTAB + 0x01fa,
		       (pDrvCtrl->mcastFilter[6] << 8)
		       | pDrvCtrl->mcastFilter[7]);

	/* Now enable multicast addressing */

	SYS_IN_SHORT (pDrvCtrl, MBC_AR, &regValue);
	regValue |= MBC_AR_HASH;
	SYS_OUT_SHORT (pDrvCtrl, MBC_AR, regValue);
	}
    }

/******************************************************************************
*
* mbcStart - start the device
*
* This function calls BSP functions to connect interrupts and start the
* device running in interrupt mode.
*
* RETURNS: OK or ERROR
*
* NOMANUAL
*/


LOCAL STATUS mbcStart
    (
    void *	pObj		/* device ID */
    )
    {
    STATUS	 result;
    MBC_DEVICE * pDrvCtrl = pObj;	/* device to be started */

    DRV_LOG (DRV_DEBUG_LOAD, "%s%d: mbcStart\n",
	     (int) DEV_NAME, pDrvCtrl->unit, 3, 4, 5, 6);

    SYS_INT_CONNECT (pDrvCtrl, mbcInt, (int)pDrvCtrl, &result);
    if (result == ERROR)
	return (ERROR);

    DRV_LOG (DRV_DEBUG_LOAD, "Interrupt connected.\n", 1, 2, 3, 4, 5, 6);

    /* enable interrupts - clear events and set mask */

    SYS_OUT_SHORT (pDrvCtrl, MBC_IEVNT, 0xffff);
    SYS_OUT_SHORT (pDrvCtrl, MBC_IMASK, MBC_IMASK_RXF | MBC_IMASK_EBE);

    /* enable the device */

    SYS_UPDATE_SHORT (pDrvCtrl, MBC_ECNTL, MBC_ECNTL_ENBL);
    
    DRV_LOG (DRV_DEBUG_LOAD, "interrupt enabled.\n", 1, 2, 3, 4, 5, 6);

    return (OK);
    }

/******************************************************************************
*
* mbcInt - handle controller interrupt
*
* This routine is called at interrupt level in response to an interrupt from
* the controller.
*
* RETURNS: N/A.
*/

LOCAL void mbcInt
    (
    MBC_DEVICE *	pDrvCtrl	/* interrupting device */
    )
    {
    UINT16 	mask;
    UINT16 	events;

    /* identify and acknowledge all interrupt events */

    SYS_IN_SHORT (pDrvCtrl, MBC_IEVNT, &events);
    SYS_IN_SHORT (pDrvCtrl, MBC_IMASK, &mask);
    events = (events & (mask | MBC_IMASK_BSY));
    SYS_OUT_SHORT (pDrvCtrl, MBC_IEVNT, events);

    DRV_LOG (DRV_DEBUG_INT,
	     "%s%d: mbcInt events=%04x mask=%04x\n",
	     (int) DEV_NAME, pDrvCtrl->unit, events, mask, 0, 0);

    /* handle receive events */

    if (events & MBC_IEVNT_RXF)
	{
        if (pDrvCtrl->rxHandling == FALSE)
            {
	    pDrvCtrl->rxHandling = TRUE;
            netJobAdd ((FUNCPTR)mbcHandleRcvInt, (int)pDrvCtrl, 0, 0, 0, 0);
	    }
        }

    /* handle transmitter events - BD full condition -> ever happen ? */

    if (events & MBC_IEVNT_TXF)
	{
	DRV_LOG (DRV_DEBUG_INT | DRV_DEBUG_ERROR,
		 "%s%d: mbcInt: transmit full\n",
		 (int) DEV_NAME, pDrvCtrl->unit, 0, 0, 0, 0);
     
        wdCancel (pDrvCtrl->wdId);

	if (pDrvCtrl->txBlocked)	/* cause a restart */
	    {
	    netJobAdd ((FUNCPTR)muxTxRestart, (int) &pDrvCtrl->end,
		       0, 0, 0, 0);
	    pDrvCtrl->txBlocked = FALSE;
	    }
	SYS_RESET_SHORT (pDrvCtrl, MBC_IMASK, MBC_IEVNT_TXF);
	}

    /*
     * check for input busy condition, we don't enable this interrupt
     * but we check for it with each interrupt.
     */

    if (events & MBC_IEVNT_BSY)
	{
	DRV_LOG (DRV_DEBUG_INT | DRV_DEBUG_ERROR,
		 "%s%d: mbcInt: input busy\n",
		 (int) DEV_NAME, pDrvCtrl->unit, 0, 0, 0, 0);
        /* count discarded frames as errors */
	END_ERR_ADD (&pDrvCtrl->end, MIB2_IN_ERRS, +1);
	}

    /* restart the transmitter on a ethernet bus error */
    if (events & MBC_IEVNT_EBE)
        {
	DRV_LOG (DRV_DEBUG_INT | DRV_DEBUG_ERROR,
		 "%s%d: mbcInt: ethernet bus error\n",
		 (int) DEV_NAME, pDrvCtrl->unit, 0, 0, 0, 0);

	netJobAdd ((FUNCPTR)mbcDeviceRestart, (int) pDrvCtrl, 0, 0, 0, 0);
	}

    if (!pDrvCtrl->txCleaning)
        {
        pDrvCtrl->txCleaning = TRUE;
        netJobAdd ((FUNCPTR)mbcScrubTRing, (int) pDrvCtrl, 0, 0, 0, 0);
        }

    /* ignore and reset all other events */

    SYS_OUT_SHORT (pDrvCtrl, MBC_IEVNT, ~mask);
    }

/******************************************************************************
*
* mbcScrubTRing - clean the transmit ring
*
* Cleans the transmit ring.
*
* RETURNS: N/A
*
* NOMANUAL
*/

LOCAL void mbcScrubTRing
    (
    MBC_DEVICE *	pDrvCtrl
    )
    {
    int			oldLevel;
    volatile MBC_BD *	pTxBd;
    
    pDrvCtrl->txCleaning = TRUE;
    pTxBd = &pDrvCtrl->txBdBase [pDrvCtrl->txClNext];
    while (pTxBd->dataPointer != NULL)
        {
        /* If the buffer is still owned by MBC, don't touch it */

	if (pTxBd->statusMode & MBC_TXBD_R)
	    break;

        oldLevel = intLock ();
        if (pDrvCtrl->freeRtn[pDrvCtrl->txClNext] != NULL)
            {
            pDrvCtrl->freeRtn[pDrvCtrl->txClNext]
	      (pDrvCtrl->freeData[pDrvCtrl->txClNext].arg1,
	       pDrvCtrl->freeData[pDrvCtrl->txClNext].arg2); 
            pDrvCtrl->freeRtn[pDrvCtrl->txClNext] = NULL;
            pDrvCtrl->freeData[pDrvCtrl->txClNext].arg1 = NULL; 
            pDrvCtrl->freeData[pDrvCtrl->txClNext].arg2 = NULL; 
            }
        
	pTxBd->dataPointer = NULL;
	pTxBd->statusMode &= ~MBC_TXBD_ERRS;

        /* now bump the disposal index pointer around the ring */

	pDrvCtrl->txClNext = (pDrvCtrl->txClNext + 1) % pDrvCtrl->txBdNum;
	pTxBd = &pDrvCtrl->txBdBase [pDrvCtrl->txClNext];
        intUnlock (oldLevel);
        }
    pDrvCtrl->txCleaning = FALSE;
    }

/******************************************************************************
*
* mbcSend - the driver send routine
*
* This routine takes a M_BLK_ID sends off the data in the M_BLK_ID.
* The buffer must already have the addressing information properly installed
* in it.  This is done by a higher layer.
*
* RETURNS: OK or ERROR.
*/

LOCAL STATUS mbcSend
    (
    void *	pObj,		/* device ptr */
    M_BLK_ID	pMblk		/* data to send */
    )
    {
    int		len;
    char *	pBuf;
    char *	pOrig;
    int		oldLevel;
    MBC_DEVICE *	pDrvCtrl = pObj;
    volatile MBC_BD *	pTxBd;
    
    DRV_LOG (DRV_DEBUG_TX,
	     "%s%d: mbcSend txBlocked=%d txCleaning=%d\n",
	     (int) DEV_NAME, pDrvCtrl->unit,
	     pDrvCtrl->txBlocked, pDrvCtrl->txCleaning, 5, 6);

    /*
     * Obtain exclusive access to transmitter.  This is necessary because
     * we might have more than one stack transmitting at once.
     */

    if (!(pDrvCtrl->flags & MBC_POLLING))
	END_TX_SEM_TAKE (&pDrvCtrl->end, WAIT_FOREVER);

    /* check if a transmit buffer descriptor is available */

    pTxBd = &pDrvCtrl->txBdBase [pDrvCtrl->txBdNext];
    if (pTxBd->statusMode & MBC_TXBD_R)
	{
	DRV_LOG (DRV_DEBUG_TX | DRV_DEBUG_ERROR,
		 "%s%d: mbcSend: #%d still ready\n",
		 (int) DEV_NAME, pDrvCtrl->unit,
		 pDrvCtrl->txBdNext, 4, 5, 6);
	wdStart (pDrvCtrl->wdId, WD_TIMEOUT, (FUNCPTR) mbcDeviceRestart,
		 (int) pDrvCtrl);
	pDrvCtrl->txBlocked = TRUE;
	SYS_UPDATE_SHORT (pDrvCtrl, MBC_IMASK, MBC_IEVNT_TXF);
	if (!(pDrvCtrl->flags & MBC_POLLING))
	    END_TX_SEM_GIVE (&pDrvCtrl->end);
	return (END_ERR_BLOCK);
	}

    if (pTxBd->dataPointer)
	{
	/* cleaning still needed, queue cleaning task */

	DRV_LOG (DRV_DEBUG_TX | DRV_DEBUG_ERROR,
		 "%s%d: mbcSend: #%d needs cleaning (cleaning=%d)\n",
		 (int) DEV_NAME, pDrvCtrl->unit,
		  pDrvCtrl->txBdNext, pDrvCtrl->txCleaning, 5, 6);
	pDrvCtrl->txBlocked = TRUE;
	if (pDrvCtrl->txCleaning == FALSE)
	    {
	    pDrvCtrl->txCleaning = TRUE;
	    netJobAdd ((FUNCPTR)mbcScrubTRing, (int) pDrvCtrl, 0, 0, 0, 0);
	    }
	if (!(pDrvCtrl->flags & MBC_POLLING))
	    END_TX_SEM_GIVE (&pDrvCtrl->end);
        return (END_ERR_BLOCK);
	}

    pOrig = pBuf = netClusterGet (pDrvCtrl->end.pNetPool, pDrvCtrl->pClPoolId);
    if (pBuf == NULL)
        {
	DRV_LOG (DRV_DEBUG_TX | DRV_DEBUG_ERROR,
		 "%s%d: mbcSend: out of clusters\n",
		 (int) DEV_NAME, pDrvCtrl->unit, 3, 4, 5, 6);
	if (!(pDrvCtrl->flags & MBC_POLLING))
	    END_TX_SEM_GIVE (&pDrvCtrl->end);
        netMblkClChainFree (pMblk);
        return (ERROR);
        }

    pBuf += pDrvCtrl->offset;
    len = netMblkToBufCopy (pMblk, pBuf, NULL);
    netMblkClChainFree (pMblk); 
    len = max (len, ETHERSMALL);

    /* place a transmit request */

    if (!(pDrvCtrl->flags & MBC_POLLING))
        oldLevel = intLock ();
    
    pTxBd->dataPointer = pBuf;
    pTxBd->dataLength = len;
    pTxBd->statusMode &= ~(MBC_TXBD_ERRS|MBC_TXBD_RC);
    pTxBd->statusMode |= MBC_TXBD_R;

    /* Advance our management index, and register for cleaning */

    pDrvCtrl->txBdNext = (pDrvCtrl->txBdNext + 1) % pDrvCtrl->txBdNum;

    pDrvCtrl->freeRtn [pDrvCtrl->txBdNext] = (FUNCPTR) netClFree;
    pDrvCtrl->freeData [pDrvCtrl->txBdNext].arg1 = pDrvCtrl->end.pNetPool;
    pDrvCtrl->freeData [pDrvCtrl->txBdNext].arg2 = pOrig;

    if (!(pDrvCtrl->flags & MBC_POLLING))
	{
        intUnlock (oldLevel);
	END_TX_SEM_GIVE (&pDrvCtrl->end);
	}
    
    /* Bump the statistic counter. */

    END_ERR_ADD (&pDrvCtrl->end, MIB2_OUT_UCAST, +1);

    return (OK);
    }

/******************************************************************************
*
* mbcPacketGet - get next received message
*
* Get next received message.  Returns NULL if none are
* ready.
*
* RETURNS: ptr to next packet, or NULL if none ready.
*
* NOMANUAL
*/

char* mbcPacketGet
    (
    MBC_DEVICE *	pDrvCtrl	/* device structure */
    )
    {
    volatile MBC_BD *	pRxBd;
    
    pRxBd = &pDrvCtrl->rxBdBase [pDrvCtrl->rxBdNext];
    
    while (!(pRxBd->statusMode & MBC_RXBD_E))
        {
        /* data chaining is not supported - check all error conditions */

        if (((pRxBd->statusMode & (MBC_RXBD_F  | MBC_RXBD_L))
	                       == (MBC_RXBD_F  | MBC_RXBD_L)) &&
            !(pRxBd->statusMode & (MBC_RXBD_CL | MBC_RXBD_OV |
                                   MBC_RXBD_CR | MBC_RXBD_SH |
                                   MBC_RXBD_NO | MBC_RXBD_LG)))
	    {
	    return (pRxBd->dataPointer);
	    }

	END_ERR_ADD (&pDrvCtrl->end, MIB2_IN_UCAST, +1);
	
	/*
	 * Fix for errata #9 -- Device Errata, Feb 20, 1997.
	 * Ethernet frames may be incorrectly received after a rx FIFO
	 * overflow.
	 */
	if ((pRxBd->statusMode & MBC_RXBD_L)
	    && !(pRxBd->statusMode & MBC_RXBD_F))
	    {
	    mbcDeviceRestart (pDrvCtrl);
	    return (NULL);
	    }

        /* reset buffer descriptor as empty */

        pRxBd->statusMode |= MBC_RXBD_E;
        
        /* Advance our management index */

        pDrvCtrl->rxBdNext = (pDrvCtrl->rxBdNext + 1) % pDrvCtrl->rxBdNum;
        pRxBd = &pDrvCtrl->rxBdBase [pDrvCtrl->rxBdNext];
        }

    return (NULL);
    }

/******************************************************************************
*
* mbcRecv - process the next incoming packet
*
* Handles one incoming packet.  The packet is checked for errors.
*
* RETURNS: OK.
*
* NOMANUAL
*
*/

LOCAL STATUS mbcRecv
    (
    MBC_DEVICE *	pDrvCtrl,	/* device structure */
    char *		pData		/* packet to process */
    )
    {
    int			len;
    M_BLK_ID		pMblk;
    char *		pNewCluster;
    char *		pCluster;
    CL_BLK_ID		pClBlk;
    volatile MBC_BD *	pRxBd;

    pRxBd = &pDrvCtrl->rxBdBase [pDrvCtrl->rxBdNext];

    /*
     * We implicitly are loaning here, if copying is necessary this
     * step may be skipped, but the data must be copied before being
     * passed up to the protocols.
     */
    
    pNewCluster = netClusterGet (pDrvCtrl->end.pNetPool, pDrvCtrl->pClPoolId);
    if (pNewCluster == NULL)
        {
	DRV_LOG (DRV_DEBUG_RX | DRV_DEBUG_ERROR,
		 "%s%d: mbcRecv: out of clusters\n",
		 (int) DEV_NAME, pDrvCtrl->unit, 3, 4, 5, 6);
	END_ERR_ADD (&pDrvCtrl->end, MIB2_IN_ERRS, +1);
	goto cleanRXD;
        }

    /* Grab a cluster block to marry to the cluster we received. */

    pClBlk = netClBlkGet (pDrvCtrl->end.pNetPool, M_DONTWAIT);
    if (pClBlk == NULL)
        {
	DRV_LOG (DRV_DEBUG_RX | DRV_DEBUG_ERROR,
		 "%s%d: mbcRecv: out of cluster blocks\n",
		 (int) DEV_NAME, pDrvCtrl->unit, 3, 4, 5, 6);
        netClFree (pDrvCtrl->end.pNetPool, pNewCluster);
	END_ERR_ADD (&pDrvCtrl->end, MIB2_IN_ERRS, +1);
	goto cleanRXD;
        }
    
    /*
     * OK we've got a spare, let's get an M_BLK_ID and marry it to the
     * one in the ring.
     */

    pMblk = mBlkGet (pDrvCtrl->end.pNetPool, M_DONTWAIT, MT_DATA);
    if (pMblk == NULL)
        {
	DRV_LOG (DRV_DEBUG_RX | DRV_DEBUG_ERROR,
		 "%s%d: mbcRecv: out of M blocks\n",
		 (int) DEV_NAME, pDrvCtrl->unit, 3, 4, 5, 6);
        netClBlkFree (pDrvCtrl->end.pNetPool, pClBlk); 
        netClFree (pDrvCtrl->end.pNetPool, pNewCluster);
	END_ERR_ADD (&pDrvCtrl->end, MIB2_IN_ERRS, +1);
	goto cleanRXD;
        }

    DRV_LOG (DRV_DEBUG_RX,
	     "%s%d: mbcRecv %d: dataLength=%d dataPointer=%x data=%x\n",
	     (int) DEV_NAME, pDrvCtrl->unit,
	     pDrvCtrl->rxBdNext, pRxBd->dataLength,
	     (int) pRxBd->dataPointer, (int) pData);

    /* Add one to our unicast data. */

    END_ERR_ADD (&pDrvCtrl->end, MIB2_IN_UCAST, +1);

    len = pRxBd->dataLength;
    pCluster = pRxBd->dataPointer - pDrvCtrl->offset;

    /* Join the cluster to the MBlock */

    netClBlkJoin (pClBlk, pCluster, len, NULL, 0, 0, 0);
    netMblkClJoin (pMblk, pClBlk);

    pMblk->mBlkHdr.mData += pDrvCtrl->offset;
    pMblk->mBlkHdr.mLen = len;
    pMblk->mBlkHdr.mFlags |= M_PKTHDR;
    pMblk->mBlkPktHdr.len = len;

    /* make the packet data coherent */

    MBC_CACHE_INVALIDATE (pMblk->mBlkHdr.mData, len);

    /* Installed (adjusted) new cluster, and reset descriptor */

    pRxBd->dataPointer = pNewCluster + pDrvCtrl->offset;
    pRxBd->statusMode |= MBC_RXBD_E;

    /* Advance our management index */

    pDrvCtrl->rxBdNext = (pDrvCtrl->rxBdNext + 1) % pDrvCtrl->rxBdNum;

#ifdef DRV_DEBUG
    {
    char *pAddress = pCluster + pDrvCtrl->offset;

    if (pAddress[0] & 0x01)
	DRV_LOG (DRV_DEBUG_RX | DRV_DEBUG_MULTI,
		 "Recv Multicast %02x:%02x:%02x:%02x:%02x:%02x\n",
		 pAddress[0]&0xff, pAddress[1]&0xff, pAddress[2]&0xff,
		 pAddress[3]&0xff, pAddress[4]&0xff, pAddress[5]&0xff);
    }
#endif

    /* Call the upper layer's receive routine. */

    END_RCV_RTN_CALL (&pDrvCtrl->end, pMblk);

    return (OK);

cleanRXD:

    /* Reset descriptor and advance management index */

    pRxBd->statusMode |= MBC_RXBD_E;
    pDrvCtrl->rxBdNext = (pDrvCtrl->rxBdNext + 1) % pDrvCtrl->rxBdNum;

    return (OK);
    }

/******************************************************************************
*
* mbcHandleRcvInt - task level interrupt service for input packets
*
* This routine is called at task level indirectly by the interrupt
* service routine to do any message received processing.
*
* The double loop is to protect against a race condition where the interrupt
* code see rxHandling as TRUE, but it is then turned off by task code.
* This race is not fatal, but does cause occassional delays until a second
* packet is received and then triggers the netTask to call this routine again.
*
* RETURNS: N/A.
*/

LOCAL void mbcHandleRcvInt
    (
    MBC_DEVICE *	pDrvCtrl	/* interrupting device */
    )
    {
    char *	pData;

    do
        {
        pDrvCtrl->rxHandling = TRUE;

        while ((pData = mbcPacketGet (pDrvCtrl)) != NULL)
            mbcRecv (pDrvCtrl, pData);

        pDrvCtrl->rxHandling = FALSE;
        }
    while (mbcPacketGet (pDrvCtrl) != NULL);
    }

/******************************************************************************
*
* mbcIoctl - the driver I/O control routine
*
* Process an ioctl request.
*
* RETURNS: A command specific response, usually OK or ERROR.
*/

LOCAL int mbcIoctl
    (
    void *		pObj,		/* device receiving command */
    int			cmd,		/* ioctl command code */
    caddr_t		data		/* command argument */
    )
    {
    MBC_DEVICE *	pDrvCtrl = pObj;	/* device receiving command */
    int			error = 0;
    long		value;

    switch (cmd)
            {
        case EIOCSADDR:
	    if (data == NULL)
		return (EINVAL);
            bcopy ((char *)data, (char *)END_HADDR(&pDrvCtrl->end),
		   END_HADDR_LEN(&pDrvCtrl->end));
            break;

        case EIOCGADDR:
	    if (data == NULL)
		return (EINVAL);
            bcopy ((char *)END_HADDR(&pDrvCtrl->end), (char *) data,
		   END_HADDR_LEN(&pDrvCtrl->end));
            break;

        case EIOCSFLAGS:
	    value = (long)data;
	    if (value < 0)
		{
		value = -(value - 1);
		END_FLAGS_CLR (&pDrvCtrl->end, value);
		}
	    else
		{
		END_FLAGS_SET (&pDrvCtrl->end, value);
		}
	    mbcConfig (pDrvCtrl);
            break;

        case EIOCGFLAGS:
	    *(int *)data = END_FLAGS_GET(&pDrvCtrl->end);
            break;

	case EIOCPOLLSTART:	/* Begin polled operation */
	    mbcPollStart (pDrvCtrl);
	    break;

	case EIOCPOLLSTOP:	/* End polled operation */
	    mbcPollStop (pDrvCtrl);
	    break;

        case EIOCGMIB2:		/* return MIB information */
            if (data == NULL)
                return (EINVAL);
            bcopy ((char *)&pDrvCtrl->end.mib2Tbl, (char *) data,
		   sizeof(pDrvCtrl->end.mib2Tbl));
            break;
	    
        case EIOCGFBUF:		/* return minimum First Buffer for chaining */
            if (data == NULL)
                return (EINVAL);
            *(int *) data = MBC_MIN_FBUF;
            break;
	    
        default:
            error = EINVAL;
        }

    return (error);
    }

/******************************************************************************
*
* mbcAddrFilterSet - set the address filter for multicast addresses
*
* This routine goes through all of the multicast addresses on the list
* of addresses (added with the endAddrAdd() routine) and sets the
* device's filter correctly.
*
* RETURNS: N/A.
*/

void mbcAddrFilterSet
    (
    MBC_DEVICE *	pDrvCtrl		/* device to be updated */
    )
    {
    int			len;
    int			count;
    u_char		c;
    u_char *		pCp;
    u_long		crc;
    ETHER_MULTI *	pCurr;
    
    pDrvCtrl->mcastFilter[0] = 0x00;
    pDrvCtrl->mcastFilter[1] = 0x00;
    pDrvCtrl->mcastFilter[2] = 0x00;
    pDrvCtrl->mcastFilter[3] = 0x00;
    pDrvCtrl->mcastFilter[4] = 0x00;
    pDrvCtrl->mcastFilter[5] = 0x00;
    pDrvCtrl->mcastFilter[6] = 0x00;
    pDrvCtrl->mcastFilter[7] = 0x00;

    for (pCurr = END_MULTI_LST_FIRST (&pDrvCtrl->end);
	 pCurr != NULL;
	 pCurr = END_MULTI_LST_NEXT(pCurr))
	{
	/*
	 * AUTODIN-II, adapted for ethernet (bit reversed),
	 * taken from the ln7990End.c driver
	 */
	pCp = (unsigned char *)&pCurr->addr;
	crc = 0xffffffff;
	for (len = 6; --len >= 0;)
	    {
	    c = *pCp++;
	    for (count = 0; count < 8; count++)
		{
		if ((c & 0x01) ^ (crc & 0x01))
		    {
		    crc >>= 1;
		    crc = crc ^ 0xedb88320;
		    }
		else
		    {
		    crc >>= 1;
		    }
		c >>= 1;
		}
	    }

	/* Just want the 6 most significant bits. */
	crc = crc >> 26;

	/* Turn on the corresponding bit in the filter. */
	pDrvCtrl->mcastFilter [crc >> 3] |= (1 << (crc & 0x07));
	}
    }

/******************************************************************************
*
* mbcPollRcv - routine to receive a packet in polled mode.
*
* This routine is called by a user to try and get a packet from the
* device.
*
* RETURNS: OK upon success.  EAGAIN is returned when no packet is available.
*/

LOCAL STATUS mbcPollRcv
    (
    void * 	pObj,	/* device to be polled */
    M_BLK_ID	pMblk	/* ptr to buffer */
    )
    {
    int			len;
    MBC_DEVICE *	pDrvCtrl = pObj;	/* device to be polled */
    volatile MBC_BD *	pRxBd;

    pRxBd = &pDrvCtrl->rxBdBase [pDrvCtrl->rxBdNext];
    len = pRxBd->dataLength;

    /* If no packet is available return immediately */

    if (pRxBd->statusMode & MBC_RXBD_E)
        return (EAGAIN);

    /* Upper layer must provide a valid buffer. */

    if ((pMblk->mBlkHdr.mLen < len) || (!(pMblk->mBlkHdr.mFlags & M_EXT)))
	{
	DRV_LOG (DRV_DEBUG_POLL_RX, "%s%d: mbcPollRcv: rxBdNext=%d bad mblk\n",
		 (int) DEV_NAME, pDrvCtrl->unit, pDrvCtrl->rxBdNext, 4, 5, 6);
	return (EAGAIN);
	}

    /* Check packet and device for errors */

    if (((pRxBd->statusMode & (MBC_RXBD_F | MBC_RXBD_L))
	 != (MBC_RXBD_F | MBC_RXBD_L))
	|| (pRxBd->statusMode & (MBC_RXBD_CL | MBC_RXBD_OV |
				 MBC_RXBD_CR | MBC_RXBD_SH |
				 MBC_RXBD_NO | MBC_RXBD_LG)))
	{
	/* packet has an error */

	DRV_LOG (DRV_DEBUG_POLL_RX | DRV_DEBUG_ERROR,
		 "%s%d: mbcPollRcv: rxBdNext=%d bad packet stat=%x\n",
		 (int) DEV_NAME, pDrvCtrl->unit, pDrvCtrl->rxBdNext,
		 pRxBd->statusMode, 5, 6);
	
	pRxBd->statusMode |= MBC_RXBD_E;
	pDrvCtrl->rxBdNext = (pDrvCtrl->rxBdNext + 1) % pDrvCtrl->rxBdNum;

        return (EAGAIN);
	}

    END_ERR_ADD (&pDrvCtrl->end, MIB2_IN_UCAST, +1);

    pMblk->mBlkHdr.mData += pDrvCtrl->offset;
    bcopy (pRxBd->dataPointer, pMblk->mBlkHdr.mData, len);
    pMblk->mBlkHdr.mLen = len;
    pMblk->mBlkHdr.mFlags |= M_PKTHDR;
    pMblk->mBlkPktHdr.len = len;

    /* Done with packet, clean up and give it to the device. */

    pRxBd->statusMode |= MBC_RXBD_E;
    pDrvCtrl->rxBdNext = (pDrvCtrl->rxBdNext + 1) % pDrvCtrl->rxBdNum;
        
    DRV_LOG (DRV_DEBUG_POLL_RX,
	     "%s%d: mbcPollRcv: rxBdNext=%d good packet len=%d\n",
	     (int) DEV_NAME, pDrvCtrl->unit, pDrvCtrl->rxBdNext,
	     len, 5, 6);

    return (OK);
    }

/******************************************************************************
*
* mbcPollSend - routine to send a packet in polled mode.
*
* This routine is called by a user to try to send a packet on the
* device.
*
* RETURNS: OK upon success.  EAGAIN if device is busy.
*/

LOCAL STATUS mbcPollSend
    (
    void * 	pObj,	/* device to be polled */
    M_BLK_ID    pMblk	/* packet to send */
    )
    {
    char *		pBuf = NULL;
    int			len;
    int			oldLevel;
    MBC_DEVICE *	pDrvCtrl = pObj;	/* device to be polled */
    volatile MBC_BD *	pTxBd;
    
    /* test to see if TX is busy */
    
    pTxBd = &pDrvCtrl->txBdBase [pDrvCtrl->txBdNext];
    if (pTxBd->statusMode & MBC_TXBD_R)
	{
	DRV_LOG (DRV_DEBUG_POLL_TX | DRV_DEBUG_ERROR,
		 "%s%d: mbcPollSend: no Tx desc. txBdNext=%d\n",
		 (int) DEV_NAME, pDrvCtrl->unit, pDrvCtrl->txBdNext, 4, 5, 6);
	return (EAGAIN);
	}

    /* Check if descriptor is clean */

    if (pTxBd->dataPointer)
	{
	DRV_LOG (DRV_DEBUG_POLL_TX | DRV_DEBUG_ERROR,
		 "%s%d: mbcPollSend: needs cleaning\n",
		 (int) DEV_NAME, pDrvCtrl->unit, 3, 4, 5, 6);
	if (!pDrvCtrl->txCleaning == TRUE);
	    netJobAdd ((FUNCPTR)mbcScrubTRing, (int) pDrvCtrl, 0, 0, 0, 0);
	return (EAGAIN);
	}

    /*
     * If packet is in a single block, we can transmit
     * directly from the M_BLK, otherwise we have to copy.
     */
    
#if 0	/* XXX */
    if (pMblk->mBlkHdr.mNext == NULL)
        {
        len = max (ETHERSMALL, pMblk->m_len);
	pTxBd->dataPointer = pMblk->m_data;
        }
    else
#endif  /* 0 */
        {
        pBuf = netClusterGet (pDrvCtrl->end.pNetPool, pDrvCtrl->pClPoolId);
        if (pBuf == NULL)
	    {
	    DRV_LOG (DRV_DEBUG_POLL_TX | DRV_DEBUG_ERROR,
		     "%s%d: mbcPollSend: no clusters\n",
		     (int) DEV_NAME, pDrvCtrl->unit, 3, 4, 5, 6);
            return (EAGAIN);
	    }
        len = netMblkToBufCopy (pMblk, pBuf, NULL);
	pTxBd->dataPointer = pBuf;
        }
    
    DRV_LOG (DRV_DEBUG_POLL_TX, "%s%d: mbcPollSend: txBdNext=%d len=%d\n",
	     (int) DEV_NAME, pDrvCtrl->unit, pDrvCtrl->txBdNext, len, 5, 6);

    CACHE_PIPE_FLUSH ();

    /* Bump the statistic counter. */

    END_ERR_ADD (&pDrvCtrl->end, MIB2_OUT_UCAST, +1);
  
    /* Start transmission */

    oldLevel = intLock ();
    pTxBd->dataLength = len;
    pTxBd->statusMode &= ~(MBC_TXBD_ERRS|MBC_TXBD_RC);
    pTxBd->statusMode |= MBC_TXBD_R;
    intUnlock (oldLevel);

    /* Advance our management index. */

    pDrvCtrl->txBdNext = (pDrvCtrl->txBdNext + 1) % pDrvCtrl->txBdNum;

    /* Spin until packet has been sent. */

    while (pTxBd->statusMode & MBC_TXBD_R)
	;

    /* Manually clean descriptor. */

    pTxBd->dataPointer = NULL;
    pTxBd->statusMode &= ~MBC_TXBD_ERRS;

    /* Free allocated cluster (if any), and clean tx ring (if needed) */

    if (pBuf != NULL)
        netClFree (pDrvCtrl->end.pNetPool, pBuf);
    if (!pDrvCtrl->txCleaning)
        {
        pDrvCtrl->txCleaning = TRUE;
        mbcScrubTRing (pDrvCtrl);
        }
    
    return (OK);
    }

/******************************************************************************
*
* mbcMCastAdd - add a multicast address for the device
*
* This routine adds a multicast address to whatever the driver
* is already listening for.  It then resets the address filter.
*
* RETURNS: OK or ERROR.
*/

LOCAL STATUS mbcMCastAdd
    (
    void *	pObj,		/* device pointer */
    char *	pAddress	/* new address to add */
    )
    {
    int error;
    MBC_DEVICE * pDrvCtrl = pObj;	/* device pointer */

    DRV_LOG (DRV_DEBUG_LOAD | DRV_DEBUG_MULTI,
	     "mbcMCastAdd %02x:%02x:%02x:%02x:%02x:%02x\n",
	     pAddress[0]&0xff, pAddress[1]&0xff, pAddress[2]&0xff,
	     pAddress[3]&0xff, pAddress[4]&0xff, pAddress[5]&0xff);

    error = etherMultiAdd (&pDrvCtrl->end.multiList, pAddress);
    if (error == ENETRESET)
	mbcConfig (pDrvCtrl);

    return (OK);
    }

/******************************************************************************
*
* mbcMCastDel - delete a multicast address for the device
*
* This routine removes a multicast address from whatever the driver
* is listening for.  It then resets the address filter.
*
* RETURNS: OK or ERROR.
*/

LOCAL STATUS mbcMCastDel
    (
    void *	pObj,		/* device pointer */
    char *	pAddress	/* address to be deleted */
    )
    {
    int		 error;
    MBC_DEVICE * pDrvCtrl = pObj;	/* device pointer */

    DRV_LOG (DRV_DEBUG_LOAD | DRV_DEBUG_MULTI,
	     "mbcMCastDel %02x:%02x:%02x:%02x:%02x:%02x\n",
	     pAddress[0]&0xff, pAddress[1]&0xff, pAddress[2]&0xff,
	     pAddress[3]&0xff, pAddress[4]&0xff, pAddress[5]&0xff);

    error = etherMultiDel (&pDrvCtrl->end.multiList, pAddress);
    if (error == ENETRESET)
	mbcConfig (pDrvCtrl);

    return (OK);
    }

/******************************************************************************
*
* mbcMCastGet - get the multicast address list for the device
*
* This routine gets the multicast list of whatever the driver
* is already listening for.
*
* RETURNS: OK or ERROR.
*/

LOCAL STATUS mbcMCastGet
    (
    void *		pObj,		/* device pointer */
    MULTI_TABLE *	pTable		/* address table to be filled in */
    )
    {
    MBC_DEVICE * pDrvCtrl = pObj;	/* device pointer */

    return (etherMultiGet (&pDrvCtrl->end.multiList, pTable));
    }

/******************************************************************************
*
* mbcStop - stop the device
*
* This function calls BSP functions to disconnect interrupts and stop
* the device from operating in interrupt mode.
*
* RETURNS: OK or ERROR.
*/

LOCAL STATUS mbcStop
    (
    void *	pObj	/* device to be stopped */
    )
    {
    MBC_DEVICE *	pDrvCtrl = pObj;	/* device to be stopped */
    STATUS		result = OK;

    /* stop/disable the device. */
    
    mbcReset (pDrvCtrl);
    
    SYS_INT_DISCONNECT (pDrvCtrl, mbcInt, (int)pDrvCtrl, &result);

#ifdef DRV_DEBUG
    if (result == ERROR)
	{
	DRV_LOG (DRV_DEBUG_LOAD | DRV_DEBUG_ERROR,
		 "Could not disconnect interrupt!\n",
		 1, 2, 3, 4, 5, 6);
	}
#endif	/* DRV_DEBUG */
    return (result);
    }

/******************************************************************************
*
* mbcUnload - unload a driver from the system
*
* This function first brings down the device, and then frees any
* stuff that was allocated by the driver in the load function.
*
* RETURNS: OK.
*/

LOCAL STATUS mbcUnload
    (
    void *	pObj	/* device to be unloaded */
    )
    {
    MBC_DEVICE * pDrvCtrl = pObj;	/* device to be unloaded */

    END_OBJECT_UNLOAD (&pDrvCtrl->end);
    
    /* Free the shared DMA memory. */

    if (pDrvCtrl->flags & MBC_MEM_ALLOC_FLAG)
	cacheDmaFree (pDrvCtrl->bufBase);
    
    return (OK);
    }

/******************************************************************************
*
* mbcPollStart - start polled mode operations
*
* RETURNS: OK.
*/

LOCAL STATUS mbcPollStart
    (
    MBC_DEVICE *	pDrvCtrl	/* device to be polled */
    )
    {
    int	oldLevel;

    DRV_LOG (DRV_DEBUG_POLL, "%s%d: ultraPollStart\n",
	     (int) DEV_NAME, pDrvCtrl->unit, 3, 4, 5, 6);
     
    oldLevel = intLock ();		/* disable ints during update */
    SYS_OUT_SHORT (pDrvCtrl, MBC_IMASK, 0x00);
    pDrvCtrl->flags |= MBC_POLLING;
    intUnlock (oldLevel);		/* now mbcInt won't get confused */

    return (OK);
    }

/******************************************************************************
*
* mbcPollStop - stop polled mode operations
*
* This function terminates polled mode operation.  The device returns to
* interrupt mode.
*
* The device interrupts are enabled, the current mode flag is switched
* to indicate interrupt mode and the device is then reconfigured for
* interrupt operation.
*
* RETURNS: OK or ERROR.
*/

LOCAL STATUS mbcPollStop
    (
    MBC_DEVICE *	pDrvCtrl	/* device to be polled */
    )
    {
    int	oldLevel;

    DRV_LOG (DRV_DEBUG_POLL, "%s%d: ultraPollStop\n",
	     (int) DEV_NAME, pDrvCtrl->unit, 3, 4, 5, 6);
     
    /* re-enable interrupts */
      
    oldLevel = intLock ();	/* disable ints during register updates */
    SYS_OUT_SHORT (pDrvCtrl, MBC_IMASK, MBC_IMASK_RXF | MBC_IMASK_EBE);
    pDrvCtrl->flags &= ~MBC_POLLING;
    intUnlock (oldLevel);

    return (OK);
    }

/******************************************************************************
*
* mbcReset - reset device
*
* RETURNS: N/A.
*
* NOMANUAL
*/

LOCAL void mbcReset
    (
    MBC_DEVICE *	pDrvCtrl	/* interrupting device */
    )
    {
    int counter = 0xffff;
    
    /* Disable interrupts */
    
    SYS_OUT_SHORT (pDrvCtrl, MBC_IMASK, 0x00);

    /* clear pending `graceful stop complete' event, and start one */

    SYS_OUT_SHORT (pDrvCtrl, MBC_IEVNT, MBC_IEVNT_GRA);
    SYS_OUT_SHORT (pDrvCtrl, MBC_ECNTL, MBC_ECNTL_GTS);

    /* wait for graceful stop to register */

    while (counter--)
	{
	USHORT event;
	
	SYS_IN_SHORT (pDrvCtrl, MBC_IEVNT, &event);
	if (event & MBC_IEVNT_GRA)
	    break;
	}

    /* disable the receiver and transmitter. */

    SYS_RESET_SHORT (pDrvCtrl, MBC_ECNTL, MBC_ECNTL_ENBL);
    
    pDrvCtrl->resetCounter++;
    }
