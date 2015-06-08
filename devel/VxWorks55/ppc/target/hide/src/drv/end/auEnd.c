/* auEnd.c - END style Au MAC Ethernet driver */

/* Copyright 1984-2002 Wind River Systems, Inc., and Cetia Inc. */

#include "copyright_wrs.h"

/*
 * This file has been developed or significantly modified by the
 * MIPS Center of Excellence Dedicated Engineering Staff.
 * This notice is as per the MIPS Center of Excellence Master Partner
 * Agreement, do not remove this notice without checking first with
 * WR/Platforms MIPS Center of Excellence engineering management.
 */

/*
modification history
--------------------
01k,24jun02,zmm  Removed kernel calls from polling routines, Fix SPR 75773.
01j,14may02,zmm  Global au1000 name changes. SPR 77333.
01i,18apr02,zmm  Add muxError calls, if mBlkGet, netClBlkGet, or netClusterGet
                 fails. Fix SPR 73756.
01h,04apr02,zmm  Move enabling the receiver from Reset to Config routine.
                 Fix SPR 75126.
01g,15nov01,zmm  Supported auto-negotiation and cache coherency, 
                 plus general cleanups. SPR 71884.
01f,11oct01,tlc  Change register names according to new revision of Au1000
                 Databook.  General cleanup.
01e,21sep01,agf  add cluster alloc and MAC enable address to LOAD string
                 generalize driver to work on MAC 0 or 1
01d,20sep01,agf  increase pCluster allocation, check Done bit prior to Tx load
01c,19jul01,zmm  Supported polling driver mode, and few interrupt mode fixes.
01b,19jun01,zmm  Fix race condition and Tx handling.
01a,17may01,mem  written.
*/

/*
DESCRIPTION  
This module implements the Alchemey Semiconductor au on-chip
ethernet MACs.

The software interface to the driver is divided into three parts.
The first part is the interrupt registers and their setup. This part
is done at the BSP level in the various BSPs which use this driver. 
The second and third part are addressed in the driver. The second part of
the interface comprises of the I/O control registers and their
programming. The third part of the interface comprises of the descriptors
and the buffers. 

This driver is designed to be moderately generic. Though it currently is
implemented on one processor, in the future it may be added to other
Alchemey product offerings. Thus, it would be desirable to use the same
driver with no source level changes. To achieve this, the driver must be
given several target-specific parameters, and some external support
routines must be provided. These target-specific values and the external
support routines are described below.

This driver supports multiple units per CPU.  The driver can be
configured to support big-endian or little-endian architectures.

BOARD LAYOUT
This device is on-board.  No jumpering diagram is necessary.

EXTERNAL INTERFACE
The only external interface is the auEndLoad() routine, which expects
the <initString> parameter as input.  This parameter passes in a 
colon-delimited string of the format:

<unit>:<devMemAddr>:<devIoAddr>:<enableAddr>:<vecNum>:<intLvl>:<offset>
:<qtyCluster>:<flags>

The auEndLoad() function uses strtok() to parse the string.

TARGET-SPECIFIC PARAMETERS
.IP <unit>
A convenient holdover from the former model.  This parameter is used only
in the string name for the driver.

.IP <devAddr>
This parameter is the memory base address of the device registers in the
memory map of the CPU. It indicates to the driver where to find the
base MAC register.

.IP <devIoAddr>
This parameter in the base address of the device registers for the
dedicated DMA channel for the MAC device.  It indicates to the driver
where to find the DMA registers. 

.IP <enableAddr>
This parameter is the address MAC enable register. It is necessary to 
specify selection between MAC 0 and MAC 1.

.IP <vecNum>
This parameter is the vector associated with the device interrupt.
This driver configures the MAC device to generate hardware interrupts
for various events within the device; thus it contains an interrupt handler
routine.  The driver calls intConnect() via the macro SYS_INT_CONNECT()
to connect its interrupt handler to the interrupt vector generated as a
result of the MAC interrupt.

.IP <intLvl>
Some targets use additional interrupt controller devices to help organize
and service the various interrupt sources.  This driver avoids all board-
specific knowledge of such devices.  During the driver's initialization,
the external routine sysLanAuIntEnable() is called to perform any
board-specific operations required to allow the servicing of an interrupt.
For a description of sysLanAuIntEnable(), see "External Support
Requirements" below.

.IP <offset>
This parameter specifies the offset from which the packet has to be
loaded from the begining of the device buffer. Normally this parameter is
zero except for architectures which access long words only on aligned
addresses. For these architectures the value of this offset should be 2.

.IP <qtyCluster>
This parameter is used to explicitly allocate the number of clusters that
will be allocated. This allows the user to suit the stack to the amount of
physical memory on the board.

.IP <flags>
This is parameter is reserved for future use. Its value should be zero.

EXTERNAL SUPPORT REQUIREMENTS
This driver requires several external support functions, defined as macros:
.CS
    SYS_INT_CONNECT(pDrvCtrl, routine, arg)
    SYS_INT_DISCONNECT (pDrvCtrl, routine, arg)
    SYS_INT_ENABLE(pDrvCtrl)
    SYS_INT_DISABLE(pDrvCtrl)
    SYS_OUT_BYTE(pDrvCtrl, reg, data)
    SYS_IN_BYTE(pDrvCtrl, reg, data)
    SYS_OUT_WORD(pDrvCtrl, reg, data)
    SYS_IN_WORD(pDrvCtrl, reg, data)
    SYS_OUT_LONG(pDrvCtrl, reg, data)
    SYS_IN_LONG(pDrvCtrl, reg, data)
    SYS_ENET_ADDR_GET(pDrvCtrl, pAddress)    
    sysLanAuIntEnable(pDrvCtrl->intLevel) 
    sysLanAuIntDisable(pDrvCtrl->intLevel) 
    sysLanAuEnetAddrGet(pDrvCtrl, enetAdrs)
.CE

There are default values in the source code for these macros.  They presume
memory mapped accesses to the device registers and the intConnect(), and
intEnable() BSP functions.  The first argument to each is the device
controller structure. Thus, each has access back to all the device-specific
information.  Having the pointer in the macro facilitates the addition of new
features to this driver.

The macros SYS_INT_CONNECT, SYS_INT_DISCONNECT, SYS_INT_ENABLE and
SYS_INT_DISABLE allow the driver to be customized for BSPs that use special
versions of these routines.

The macro SYS_INT_CONNECT is used to connect the interrupt handler to
the appropriate vector.  By default it is the routine intConnect().

The macro SYS_INT_DISCONNECT is used to disconnect the interrupt handler prior
to unloading the module.  By default this routine is not implemented.

The macro SYS_INT_ENABLE is used to enable the interrupt level for the
end device.  It is called once during initialization.  It calls an
external board level routine sysLanAuIntEnable(). 

The macro SYS_INT_DISABLE is used to disable the interrupt level for the
end device.  It is called during stop.  It calls an
external board level routine sysLanAuIntDisable(). 

The macro SYS_ENET_ADDR_GET is used get the ethernet hardware of the
chip. This macro calls an external board level routine namely
sysLanAuEnetAddrGet() to get the ethernet address.

SYSTEM RESOURCE USAGE
When implemented, this driver requires the following system resources:

    - one mutual exclusion semaphore
    - one interrupt vector
    - 64 bytes in the initialized data section (data)
    - 0 bytes in the uninitialized data section (BSS)

The driver allocates clusters of size 1520 bytes for receive frames and
and transmit frames.

INCLUDES:
end.h endLib.h etherMultiLib.h auEnd.h

SEE ALSO: muxLib, endLib, netBufLib
.I "Writing and Enhanced Network Driver"
*/

#include "vxWorks.h"
#include "wdLib.h"
#include "stdlib.h"
#include "taskLib.h"
#include "logLib.h"
#include "intLib.h"
#include "netLib.h"
#include "stdio.h"
#include "stdlib.h"
#include "sysLib.h"
#include "iv.h"
#include "memLib.h"
#include "semLib.h"
#include "cacheLib.h"
#include "sys/ioctl.h"

#ifndef DOC             /* don't include when building documentation */
#include "net/mbuf.h"
#endif  /* DOC */

#include "net/protosw.h"
#include "sys/socket.h"
#include "errno.h"
#include "net/if.h"
#include "net/route.h"
#include "netinet/in.h"
#include "netinet/in_systm.h"
#include "netinet/in_var.h"
#include "netinet/ip.h"
#include "netinet/if_ether.h"
#include "net/if_subr.h"
#include "m2Lib.h"
#include "miiLib.h"

#include "etherMultiLib.h"              /* multicast stuff. */
#include "end.h"                        /* Common END structures. */
#include "netBufLib.h"
#include "muxLib.h"

#undef END_MACROS

#include "endLib.h"
#include "lstLib.h"                     /* Needed to maintain protocol list. */
#include "logLib.h"
#include "drv/end/auEnd.h"
#include "drv/multi/auLib.h"

/* local defines */

#define DRV_DEBUG	/* temporary should be taken out */
#undef DRV_DEBUG

/* define the various levels of debugging if the DRV_DEBUG is defined */    

#ifdef	DRV_DEBUG
#define DRV_DEBUG_OFF		0x0000
#define DRV_DEBUG_RX		0x0001
#define	DRV_DEBUG_TX		0x0002
#define DRV_DEBUG_INT		0x0004
#define	DRV_DEBUG_POLL		(DRV_DEBUG_POLL_RX | DRV_DEBUG_POLL_TX)
#define	DRV_DEBUG_POLL_RX	0x0008
#define	DRV_DEBUG_POLL_TX	0x0010
#define	DRV_DEBUG_LOAD		0x0020
#define	DRV_DEBUG_LOAD2		0x0040
#define	DRV_DEBUG_IOCTL		0x0080
#define	DRV_DEBUG_RESET		0x0100
#define	DRV_DEBUG_MCAST		0x0200
#define	DRV_DEBUG_CSR		0x0400
#define DRV_DEBUG_RX_PKT        0x0800
#define DRV_DEBUG_POLL_REDIR	0x10000
#define	DRV_DEBUG_LOG_NVRAM	0x20000
#define	DRV_DEBUG_MII		0x40000
#define DRV_DEBUG_ALL           0xfffff
#endif /* DRV_DEBUG */

/* MII macro */

#define DRV_PHY_FLAGS_ISSET(setBits)                                    \
        (pDrvCtrl->miiPhyFlags & (setBits))

/* Cache macros */

#define AU_CACHE_VIRT_TO_PHYS(address) \
        CACHE_DRV_VIRT_TO_PHYS (&pDrvCtrl->cacheFuncs, (address))

#define AU_CACHE_PHYS_TO_VIRT(address) \
        CACHE_DRV_PHYS_TO_VIRT (&pDrvCtrl->cacheFuncs, (address))

/*
 * Default macro definitions for BSP interface.
 * These macros can be redefined in a wrapper file, to generate
 * a new module with an optimized interface.
 */

#ifndef SYS_INT_CONNECT
#define SYS_INT_CONNECT(pDrvCtrl,rtn,arg,pResult)                       \
    {                                                                   \
    IMPORT STATUS intConnect();                                      \
    *pResult = intConnect ((VOIDFUNCPTR *)INUM_TO_IVEC (pDrvCtrl->ivec),\
                             (rtn), (int)(arg));                        \
    }
#endif /*SYS_INT_CONNECT*/

#ifndef SYS_INT_DISCONNECT
#define SYS_INT_DISCONNECT(pDrvCtrl,rtn,arg,pResult)                    \
    {                                                                   \
    }
#endif /*SYS_INT_DISCONNECT*/

#ifndef SYS_INT_ENABLE
#define SYS_INT_ENABLE(pDrvCtrl) \
    { \
    IMPORT STATUS sysLanAuIntEnable(); \
    sysLanAuIntEnable (pDrvCtrl->ilevel); \
    }
#endif /* SYS_INT_ENABLE*/

/* Macro to disable the appropriate interrupt level */

#ifndef SYS_INT_DISABLE
#define SYS_INT_DISABLE(pDrvCtrl) \
    { \
    IMPORT STATUS sysLanAuIntDisable (); \
    sysLanAuIntDisable (pDrvCtrl->ilevel); \
    }
#endif /* SYS_INT_DISABLE */

#ifndef SYS_ENET_ADDR_GET
#define SYS_ENET_ADDR_GET(pDrvCtrl, pAddress) \
    { \
    IMPORT STATUS sysLanAuEnetAddrGet (AU_DRV_CTRL *pDrvCtrl, \
                                     char * enetAdrs); \
    sysLanAuEnetAddrGet (pDrvCtrl, pAddress); \
    }
#endif /* SYS_ENET_ADDR_GET */

#ifndef SYS_WB_FLUSH
#define SYS_WB_FLUSH() \
    { \
    IMPORT void sysWbFlush (void); \
    sysWbFlush(); \
    }
#endif/* SYS_WB_FLUSH */

/* A shortcut for getting the hardware address from the MIB II stuff. */

#define END_HADDR(pEnd) \
                ((pEnd)->mib2Tbl.ifPhysAddress.phyAddress)

#define END_HADDR_LEN(pEnd) \
                ((pEnd)->mib2Tbl.ifPhysAddress.addrLength)

#define END_FLAGS_ISSET(pEnd, setBits) \
            ((pEnd)->flags & (setBits))

/* externs */

IMPORT int endMultiLstCnt (END_OBJ *);

#ifdef DRV_DEBUG	/* if debugging driver */

int         auDebug = (DRV_DEBUG_LOAD | DRV_DEBUG_TX | DRV_DEBUG_RX
			   | DRV_DEBUG_MII | DRV_DEBUG_INT);

#define DRV_LOG(FLG, X0, X1, X2, X3, X4, X5, X6) \
        if (auDebug & FLG) \
            logMsg((char *)X0, (int)X1, (int)X2, (int)X3, (int)X4, \
		    (int)X5, (int)X6);

#define DRV_PRINT(FLG,X) \
        if (auDebug & FLG) printf X;

#else /*DRV_DEBUG*/

#define DRV_LOG(DBG_SW, X0, X1, X2, X3, X4, X5, X6)
#define DRV_PRINT(DBG_SW,X)

#endif /*DRV_DEBUG*/

/* locals */

#ifdef DRV_DEBUG
AU_DRV_CTRL * 	dbgDrvCtrl[2];
#endif

/* forward static functions */

LOCAL int 	auReset (AU_DRV_CTRL * pDrvCtrl);
LOCAL void 	auInt (AU_DRV_CTRL * pDrvCtrl);
LOCAL void 	auHandleInt (AU_DRV_CTRL * pDrvCtrl);
LOCAL STATUS 	auRecv (AU_DRV_CTRL * pDrvCtrl);
LOCAL void 	auRestart (AU_DRV_CTRL * pDrvCtrl);
LOCAL STATUS 	auRestartSetup (AU_DRV_CTRL * pDrvCtrl);
LOCAL void 	auAddrFilterSet (AU_DRV_CTRL * pDrvCtrl);
LOCAL void	auConfig (AU_DRV_CTRL * pDrvCtrl);
LOCAL STATUS 	auMemInit (AU_DRV_CTRL * pDrvCtrl);
LOCAL void	auTRingScrub (AU_DRV_CTRL * pDrvCtrl);

LOCAL STATUS 	auPhyPreInit  (AU_DRV_CTRL *pDrvCtrl);
LOCAL STATUS 	auMiiInit  (AU_DRV_CTRL *pDrvCtrl);

LOCAL STATUS	auMiiRead (AU_DRV_CTRL *pDrvCtrl, UINT8 phyAdrs,
			       UINT8 phyReg, UINT16 *pRetVal);
LOCAL STATUS	auMiiWrite (AU_DRV_CTRL *pDrvCtrl, UINT8 phyAdrs,
				UINT8 phyReg, USHORT data);
LOCAL UINT8	auPhyFind (AU_DRV_CTRL *pDrvCtrl);

/* END Specific interfaces. */

LOCAL STATUS    auStart (AU_DRV_CTRL * pDrvCtrl);
LOCAL STATUS    auStop (AU_DRV_CTRL * pDrvCtrl);
LOCAL STATUS    auUnload (AU_DRV_CTRL * pDrvCtrl);
LOCAL int       auIoctl (AU_DRV_CTRL * pDrvCtrl, int cmd, caddr_t data);
LOCAL STATUS    auSend (AU_DRV_CTRL * pDrvCtrl, M_BLK_ID pBuf);
LOCAL STATUS    auMCastAddrAdd (AU_DRV_CTRL* pDrvCtrl, char * pAddress);
LOCAL STATUS    auMCastAddrDel (AU_DRV_CTRL * pDrvCtrl,
                                   char * pAddress);
LOCAL STATUS    auMCastAddrGet (AU_DRV_CTRL * pDrvCtrl,
                                    MULTI_TABLE * pTable);
LOCAL STATUS    auPollSend (AU_DRV_CTRL * pDrvCtrl, M_BLK_ID pBuf);
LOCAL STATUS    auPollReceive (AU_DRV_CTRL * pDrvCtrl, M_BLK_ID pBuf);
LOCAL STATUS    auPollStart (AU_DRV_CTRL * pDrvCtrl);
LOCAL STATUS    auPollStop (AU_DRV_CTRL * pDrvCtrl);

/*
 * Declare our function table.  This is static across all driver
 * instances.
 */
LOCAL NET_FUNCS auFuncTable =
    {
    (FUNCPTR) auStart,               /* Function to start the device.      */
    (FUNCPTR) auStop,                /* Function to stop the device.       */
    (FUNCPTR) auUnload,		 /* Unloading function for the driver. */
    (FUNCPTR) auIoctl,               /* Ioctl function for the driver.     */
    (FUNCPTR) auSend,                /* Send function for the driver.      */
    (FUNCPTR) auMCastAddrAdd,        /* Multicast address add              */
    (FUNCPTR) auMCastAddrDel,	 /* Multicast address delete           */
    (FUNCPTR) auMCastAddrGet,	 /* Multicast table retrieve           */
    (FUNCPTR) auPollSend,            /* Polling send function              */
    (FUNCPTR) auPollReceive,	 /* Polling receive function           */
    endEtherAddressForm,       		 /* Put address info into a packet.    */
    endEtherPacketDataGet,     		 /* Get a pointer to packet data.      */
    endEtherPacketAddrGet      		 /* Get packet addresses.              */
    };

/******************************************************************************
*
* auEndLoad - initialize the driver and device
*
* This routine initializes the driver and the device to the operational state.
* All of the device-specific parameters are passed in <initString>, which
* expects a string of the following format:
*
* <unit>:<devMemAddr>:<devIoAddr>:<enableAddr>:<vecNum>:<intLvl>:<offset>
* :<qtyCluster>:<flags>
*
* This routine can be called in two modes. If it is called with an empty but
* allocated string, it places the name of this device (that is, "au") into 
* the <initString> and returns 0.
*
* If the string is allocated and not empty, the routine attempts to load
* the driver using the values specified in the string.
*
* RETURNS: An END object pointer, or NULL on error, or 0 and the name of the
* device if the <initString> was NULL.
*/

END_OBJ * auEndLoad
    (
    char * initString            /* string to be parse by the driver */
    )
    {
    AU_DRV_CTRL *	pDrvCtrl;

    DRV_LOG (DRV_DEBUG_LOAD, "Loading au...debug @ 0X%X\n",
             (int)&auDebug, 2, 3, 4, 5, 6);

    if (initString == NULL)
        return (NULL);

    if (initString [0] == 0)
        {
        bcopy ((char *)AU_DEV_NAME, initString, AU_DEV_NAME_LEN);
	DRV_LOG (DRV_DEBUG_LOAD, "Returning string...\n", 1, 2, 3, 4, 5, 6);
        return ((END_OBJ *)OK);
        }

    DRV_LOG (DRV_DEBUG_LOAD, "austring: [%s]\n",
	    (int)initString, 2, 3, 4, 5, 6);

    /* allocate the device structure */

    pDrvCtrl = (AU_DRV_CTRL *)calloc (sizeof (AU_DRV_CTRL), 1);

    if (pDrvCtrl == NULL)
        goto errorExit;

    DRV_LOG (DRV_DEBUG_LOAD, "DrvControl : 0x%X\n", (int)pDrvCtrl,
             2, 3, 4, 5, 6);

    /* parse the init string, filling in the device structure */

    if (auInitParse (pDrvCtrl, initString) == ERROR)
	{
	DRV_LOG (DRV_DEBUG_LOAD, "Parse failed ...\n", 1, 2, 3, 4, 5, 6);
        goto errorExit;
	}

    DRV_LOG (DRV_DEBUG_LOAD, "mac   addr %08x %08x\n",
	     AU_MAC_ADDRESS_HIGH,
	     AU_MAC_ADDRESS_LOW, 0, 0, 0, 0);
    DRV_LOG (DRV_DEBUG_LOAD, "multi addr %08x %08x\n",
	     AU_MULTICAST_HASH_ADDRESS_HIGH,
	     AU_MULTICAST_HASH_ADDRESS_LOW, 0, 0, 0, 0);

    /* Have the BSP hand us our address. */

    SYS_ENET_ADDR_GET (pDrvCtrl, &(pDrvCtrl->enetAddr [0]));

    /* initialize the END and MIB2 parts of the structure */

    if (END_OBJ_INIT (&pDrvCtrl->endObj, (DEV_OBJ *)pDrvCtrl, AU_DEV_NAME,
                      pDrvCtrl->unit, &auFuncTable,
                      "Au MAC Enhanced Network Driver") == ERROR
     || END_MIB_INIT (&pDrvCtrl->endObj, M2_ifType_ethernet_csmacd,
                      &pDrvCtrl->enetAddr[0], 6, ETHERMTU,
                      AU_SPEED)
                    == ERROR)
        goto errorExit;

    DRV_LOG (DRV_DEBUG_LOAD, "END init done ...\n", 1, 2, 3, 4, 5, 6);

    /* Allocate PHY structure */

    /* Perform memory allocation */

    if (auMemInit (pDrvCtrl) == ERROR)
        goto errorExit;

    DRV_LOG (DRV_DEBUG_LOAD, "Malloc done ...\n", 1, 2, 3, 4, 5, 6);

    /* Perform memory distribution and reset and reconfigure the device */

    if (auRestartSetup (pDrvCtrl) == ERROR)
        goto errorExit;

    DRV_LOG (DRV_DEBUG_LOAD, "Restart setup done ...\n", 1, 2, 3, 4, 5, 6);

    /* set the flags to indicate readiness */

    END_OBJ_READY (&pDrvCtrl->endObj, IFF_NOTRAILERS | IFF_BROADCAST |
		                       IFF_MULTICAST | IFF_SIMPLEX);

    DRV_LOG (DRV_DEBUG_LOAD, "Done loading au...\n", 1, 2, 3, 4, 5, 6);

#ifdef DRV_DEBUG
    dbgDrvCtrl[pDrvCtrl->unit] = pDrvCtrl;
#endif
    
    return (&pDrvCtrl->endObj);

errorExit:
    if (pDrvCtrl != NULL)
        free ((char *)pDrvCtrl);

    return ((END_OBJ *)NULL);
    }

/*******************************************************************************
*
* auInitParse - parse the initialization string
*
* Parse the input string. This routine is called from auEndLoad() which
* intializes some values in the driver control structure with the values
* passed in the intialization string.
*
* The initialization string format is:
* <unit>:<devMemAddr>:<devIoAddr>:<vecNum>:<intLvl>:<offset>:<flags>
*
* .IP <unit>
* Device unit number, a small integer.
* .IP <devMemAddr>
* Device register base memory address
* .IP <devIoAddr>
* I/O register base memory address
* .IP <enableAddr>
* Address of MAC enable register
* .IP <vecNum>
* Interrupt vector number.
* .IP <intLvl>
* Interrupt level.
* .IP <offset>
* Offset of starting of data in the device buffers.
* .IP <qtyCluster>
* Number of clusters to allocate
* .IP <flags>
* Device specific flags, for future use.
*
* RETURNS: OK, or ERROR if any arguments are invalid.
*/

STATUS auInitParse
    (
    AU_DRV_CTRL *	pDrvCtrl,	/* pointer to the control structure */
    char * 		initString	/* initialization string */
    )
    {
    char*       tok;
    char*       holder = NULL;

    DRV_LOG (DRV_DEBUG_LOAD, "Parse starting ...\n", 1, 2, 3, 4, 5, 6);

    /* Parse the initString */

    /* Unit number. */

    tok = strtok_r (initString, ":", &holder);
    if (tok == NULL)
        return ERROR;

    pDrvCtrl->unit = atoi (tok);

    DRV_LOG (DRV_DEBUG_LOAD, "Unit : %d ...\n", pDrvCtrl->unit, 2, 3, 4, 5, 6);

    /* devAdrs address. */

    tok = strtok_r (NULL, ":", &holder);
    if (tok == NULL)
        return ERROR;
    pDrvCtrl->devAdrs = (UINT32) strtoul (tok, NULL, 16);

    DRV_LOG (DRV_DEBUG_LOAD, "devAdrs : 0x%X ...\n", pDrvCtrl->devAdrs,
             2, 3, 4, 5, 6);

    /* dmaAdrs address. */

    tok = strtok_r (NULL, ":", &holder);
    if (tok == NULL)
        return ERROR;
    pDrvCtrl->dmaAdrs = (UINT32) strtoul (tok, NULL, 16);

    DRV_LOG (DRV_DEBUG_LOAD, "dmaAdrs : 0x%X ...\n", pDrvCtrl->dmaAdrs,
             2, 3, 4, 5, 6);

    /* enableAdrs address. */

    tok = strtok_r (NULL, ":", &holder);
    if (tok == NULL)
        return ERROR;
    pDrvCtrl->enableAdrs = (UINT32) strtoul (tok, NULL, 16);

    DRV_LOG (DRV_DEBUG_LOAD, "enableAdrs : 0x%X ...\n", pDrvCtrl->enableAdrs,
             2, 3, 4, 5, 6);

    /* Interrupt vector. */

    tok = strtok_r (NULL, ":", &holder);
    if (tok == NULL)
        return ERROR;
    pDrvCtrl->ivec = atoi (tok);

    DRV_LOG (DRV_DEBUG_LOAD, "ivec : 0x%X ...\n", pDrvCtrl->ivec,
             2, 3, 4, 5, 6);

    /* Interrupt level. */

    tok = strtok_r (NULL, ":", &holder);
    if (tok == NULL)
        return ERROR;
    pDrvCtrl->ilevel = atoi (tok);
    DRV_LOG (DRV_DEBUG_LOAD, "ilevel : 0x%X ...\n", pDrvCtrl->ilevel,
             2, 3, 4, 5, 6);

    /* Caller supplied alignment offset. */

    tok = strtok_r (NULL, ":", &holder);
    if (tok == NULL)
	return ERROR;
    pDrvCtrl->offset = atoi (tok);
    DRV_LOG (DRV_DEBUG_LOAD, "Offset : 0x%X ...\n", pDrvCtrl->offset,
             2, 3, 4, 5, 6);

    /* Caller supplied cluster qty */

    tok = strtok_r (NULL, ":", &holder);
    if (tok == NULL)
	return ERROR;
    pDrvCtrl->clDesc.clNum    = atoi (tok);
    DRV_LOG (DRV_DEBUG_LOAD, "Cluster Qty : 0x%X ...\n", pDrvCtrl->clDesc.clNum,
             2, 3, 4, 5, 6);

    /* caller supplied flags */

    tok = strtok_r (NULL, ":", &holder);
    if (tok == NULL)
        return ERROR;

    pDrvCtrl->flags |= strtoul (tok, NULL, 16);
    DRV_LOG (DRV_DEBUG_LOAD, "flags : 0x%X ...\n", pDrvCtrl->flags,
             2, 3, 4, 5, 6);

    return (OK);
    }

/*******************************************************************************
*
* auMemInit - initialize memory.
*
* Using data in the control structure, setup and initialize the memory
* areas needed.  If the memory address is not already specified, then allocate
* cache safe memory.
*
* RETURNS: OK or ERROR.
*/

LOCAL STATUS auMemInit
    (
    AU_DRV_CTRL * pDrvCtrl /* device to be initialized */
    )
    {
    int         i;
    char *	pRxMem;
    char *	pTxMem;
    UINT32      rxSize;
    UINT32      txSize;
    UINT32      bufPtr;

    pDrvCtrl->rringSize = AU_N_RX_BUF;
    pDrvCtrl->tringSize = AU_N_TX_BUF;
    
    rxSize = pDrvCtrl->rringSize * ROUND_UP(AU_DMA_BUFSIZ, 32) + 32;
    pRxMem = (char *) cacheDmaMalloc (rxSize);
    txSize = pDrvCtrl->tringSize * ROUND_UP(AU_DMA_BUFSIZ, 32) + 32;
    pTxMem = (char *) cacheDmaMalloc (txSize);

    if ((pRxMem == NULL) || (pTxMem == NULL))
	{
	printf ("au: system memory unavailable\n");
	return (ERROR);
	}

    pDrvCtrl->pRxMemBase = pRxMem;
    pDrvCtrl->pTxMemBase = pTxMem;

    /* copy the DMA structure */

    pDrvCtrl->cacheFuncs = cacheDmaFuncs;

    bufPtr = (UINT32)pRxMem;
    for (i = 0; i < pDrvCtrl->rringSize; ++i)
	{
	pDrvCtrl->pRxMem[i] = (char *)ROUND_UP(bufPtr, 32);
	bufPtr += AU_DMA_BUFSIZ;
	}

    bufPtr = (UINT32)pTxMem;
    for (i = 0; i < pDrvCtrl->tringSize; ++i)
	{
	pDrvCtrl->pTxMem[i] = (char *)ROUND_UP(bufPtr, 32);
	bufPtr += AU_DMA_BUFSIZ;
	}

    /* allocate pool structure for mblks, clBlk, and clusters */

    if ((pDrvCtrl->endObj.pNetPool = malloc (sizeof(NET_POOL))) == NULL)
	return (ERROR);

    /* number of clusters is passed in by the endLoadString 
     * pDrvCtrl->clDesc.clNum    = pDrvCtrl->rringSize * 2;
     */
    pDrvCtrl->mClCfg.clBlkNum = pDrvCtrl->clDesc.clNum;
    pDrvCtrl->mClCfg.mBlkNum  = pDrvCtrl->mClCfg.clBlkNum * 2;

    /* total memory size for mBlks and clBlks */
    
    pDrvCtrl->mClCfg.memSize =
        (pDrvCtrl->mClCfg.mBlkNum  *  (MSIZE + sizeof (long))) +
        (pDrvCtrl->mClCfg.clBlkNum * (CL_BLK_SZ + sizeof (long)));

    /* total memory for mBlks and clBlks */

    if ((pDrvCtrl->mClCfg.memArea =
         (char *) memalign (sizeof(long), pDrvCtrl->mClCfg.memSize)) == NULL)
        return (ERROR);

    /* total memory size for all clusters */

    pDrvCtrl->clDesc.clSize  = AU_BUFSIZ;
    pDrvCtrl->clDesc.memSize =
        (pDrvCtrl->clDesc.clNum * (pDrvCtrl->clDesc.clSize + 8)) + sizeof(int);

    /* Allocate cluster memory */

    pDrvCtrl->clDesc.memArea = malloc (pDrvCtrl->clDesc.memSize);
    if (pDrvCtrl->clDesc.memArea == NULL)
	{
	DRV_LOG(DRV_DEBUG_LOAD,
		"system memory unavailable\n", 1, 2, 3, 4, 5, 6);
	return (ERROR);
	}

    /* initialize the device net pool */
    
    if (netPoolInit (pDrvCtrl->endObj.pNetPool, &pDrvCtrl->mClCfg,
                     &pDrvCtrl->clDesc, 1, NULL) == ERROR)
        {
        DRV_LOG (DRV_DEBUG_LOAD, "Could not init buffering\n",
                 1, 2, 3, 4, 5, 6);
        return (ERROR);
        }
    
    /* Store the cluster pool id as others need it later. */

    pDrvCtrl->pClPoolId = clPoolIdGet (pDrvCtrl->endObj.pNetPool,
                                       AU_BUFSIZ, FALSE);

    return (OK);
    }

/***************************************************************************
*
* auMiiInit - initialize the chip to use the MII interface
*
* This routine initializes the chip to use the MII interface.
*
* RETURNS: OK, or ERROR
*/
 
LOCAL STATUS auMiiInit
    (
    AU_DRV_CTRL *  pDrvCtrl       /* pointer to DRV_CTRL structure */
    )
    {
    DRV_LOG (DRV_DEBUG_MII, "auMiiInit\n",
             0, 0, 0, 0, 0, 0);

    /* initialize some fields in the PHY info structure */

    if (auPhyPreInit (pDrvCtrl) != OK)
	{
	DRV_LOG (DRV_DEBUG_LOAD, ("Failed to pre-initialize PHY\n"),
				  0, 0, 0, 0, 0, 0);
	return (ERROR);
	}

    DRV_LOG (DRV_DEBUG_MII, "miiPhyInit\n",
             0, 0, 0, 0, 0, 0);
    if (miiPhyInit (pDrvCtrl->pPhyInfo) != OK)
        {
	DRV_LOG (DRV_DEBUG_LOAD, "Failed to initialise PHY\n", 0,0,0,0,0,0);
	return (ERROR);
        }

    /* deal with full duplex mode and speed */

    if (pDrvCtrl->pPhyInfo->phyFlags & MII_PHY_FD)
	{
        AU_MAC_CONTROL |= AU_MAC_CONTROL_F;
	SYS_WB_FLUSH();
	DRV_LOG (DRV_DEBUG_LOAD, "In full duplex mode\n", 0,0,0,0,0,0);
	}
    else
	{
	DRV_LOG (DRV_DEBUG_LOAD, "In half duplex mode\n", 0,0,0,0,0,0);
	}

    if (pDrvCtrl->pPhyInfo->phyFlags & MII_PHY_100)
	{
	DRV_LOG (DRV_DEBUG_LOAD, "In 100Mbps mode\n", 0,0,0,0,0,0);
	}
    else
	{
	DRV_LOG (DRV_DEBUG_LOAD, "In 10Mbps mode\n", 0,0,0,0,0,0);
	}

    return (OK);
    }

/***************************************************************************
*
* auPhyPreInit - initialize some fields in the phy info structure
*
* This routine initializes some fields in the phy info structure,
* for use of the phyInit() routine.
*
* RETURNS: OK, or ERROR if could not obtain memory.
*/
 
LOCAL STATUS auPhyPreInit
    (
    AU_DRV_CTRL *  pDrvCtrl	/* pointer to DRV_CTRL structure */
    )
    {
    PHY_INFO *	pPhyInfo = NULL;
    
    DRV_LOG (DRV_DEBUG_MII, "auMiiPhyPreInit\n",
             0, 0, 0, 0, 0, 0);

    /* set MII defaults */

    pDrvCtrl->pMiiPhyTbl = NULL;
    pDrvCtrl->miiPhyFlags = (AU_USR_MII_10MB | AU_USR_MII_HD |
                            AU_USR_MII_100MB | AU_USR_MII_FD);

    /* get memory for the phyInfo structure */
	 
    if ((pDrvCtrl->pPhyInfo = calloc (sizeof (PHY_INFO), 1)) == NULL)
	return (ERROR);

    pPhyInfo = pDrvCtrl->pPhyInfo;

    /* set some default values */
 
    pDrvCtrl->pPhyInfo->pDrvCtrl = (void *) pDrvCtrl;
 
    pDrvCtrl->pPhyInfo->phyAnOrderTbl = pDrvCtrl->pMiiPhyTbl;

    pDrvCtrl->pPhyInfo->phyAddr = auPhyFind (pDrvCtrl);

    /* 
     * in case of link failure, set a default mode for the PHY 
     * if we intend to use a different media, this flag should 
     * be cleared
     */
 
    pDrvCtrl->pPhyInfo->phyFlags |= MII_PHY_DEF_SET;

    pDrvCtrl->pPhyInfo->phyWriteRtn = (FUNCPTR) auMiiWrite;
    pDrvCtrl->pPhyInfo->phyReadRtn = (FUNCPTR) auMiiRead;
    pDrvCtrl->pPhyInfo->phyDelayRtn = (FUNCPTR) taskDelay;
    pDrvCtrl->pPhyInfo->phyMaxDelay = MII_PHY_DEF_DELAY;
    pDrvCtrl->pPhyInfo->phyDelayParm = 1;

    /* in case the cable is not there, leave the PHY ready to auto-negotiate */

    pDrvCtrl->pPhyInfo->phyDefMode = PHY_AN_ENABLE;

    /* handle some user-to-physical flags */

    if (!(DRV_PHY_FLAGS_ISSET (AU_USR_MII_NO_AN)))
        MII_PHY_FLAGS_SET (MII_PHY_AUTO);
    else
        MII_PHY_FLAGS_CLEAR (MII_PHY_AUTO);

    if (DRV_PHY_FLAGS_ISSET (AU_USR_MII_AN_TBL))
        MII_PHY_FLAGS_SET (MII_PHY_TBL);
    else
        MII_PHY_FLAGS_CLEAR (MII_PHY_TBL);

    if (DRV_PHY_FLAGS_ISSET (AU_USR_MII_100MB))
        MII_PHY_FLAGS_SET (MII_PHY_100);
    else
        MII_PHY_FLAGS_CLEAR (MII_PHY_100);

    if (DRV_PHY_FLAGS_ISSET (AU_USR_MII_FD))
        MII_PHY_FLAGS_SET (MII_PHY_FD);
    else
        MII_PHY_FLAGS_CLEAR (MII_PHY_FD);

    if (DRV_PHY_FLAGS_ISSET (AU_USR_MII_10MB))
        MII_PHY_FLAGS_SET (MII_PHY_10);
    else
        MII_PHY_FLAGS_CLEAR (MII_PHY_10);

    if (DRV_PHY_FLAGS_ISSET (AU_USR_MII_HD))
        MII_PHY_FLAGS_SET (MII_PHY_HD);
    else
        MII_PHY_FLAGS_CLEAR (MII_PHY_HD);

    if (DRV_PHY_FLAGS_ISSET (AU_USR_MII_BUS_MON))
        MII_PHY_FLAGS_SET (MII_PHY_MONITOR);
    else
        MII_PHY_FLAGS_CLEAR (MII_PHY_MONITOR);

    MII_PHY_FLAGS_SET (MII_PHY_PRE_INIT);
 
    DRV_LOG (DRV_DEBUG_LOAD, ("auPhyPreInit pPhyInfo = 0x%x read=0x%x
			       write=0x%x tbl=0x%x addr=0x%x
			       flags=0x%x \n"),
			       (int) pDrvCtrl->pPhyInfo,
			       (int) pDrvCtrl->pPhyInfo->phyReadRtn,
			       (int) pDrvCtrl->pPhyInfo->phyWriteRtn,
			       (int) pDrvCtrl->pPhyInfo->phyAnOrderTbl,
			       (int) pDrvCtrl->pPhyInfo->phyAddr,
			       (int) pDrvCtrl->pPhyInfo->phyFlags);
 
    return (OK);
    }

/***************************************************************************
* 
* auMiiRead - read a PHY device register via MII
* 
* RETURNS: the contents of a PHY device register in retVal arg, OK always
*/
LOCAL STATUS auMiiRead
    (
    AU_DRV_CTRL *	pDrvCtrl,
    UINT8		phyAdrs,		/* PHY address to access */
    UINT8		phyReg,			/* PHY register to read */
    UINT16 *		pRetVal						
    )
    {
    int timeout;
    
    AU_MII_CONTROL = (phyAdrs << 11) | (phyReg << 6);

    timeout = 20;
    while (AU_MII_CONTROL & AU_MII_CONTROL_BUSY)
	{
	taskDelay (1);
	if (--timeout == 0)
	    {
	    DRV_LOG (DRV_DEBUG_MII, "PHY access timeout\n",
		     0, 0, 0, 0, 0, 0);
	    return (ERROR);
	    }
	}

    *pRetVal = AU_MII_DATA;
    DRV_LOG (DRV_DEBUG_MII, "auMiiRead adr=%02x reg=%02x data=%04x\n",
             phyAdrs, phyReg, *pRetVal, 0, 0, 0);
    return (OK);
    }

/***************************************************************************
*
* auMiiWrite - write to a PHY device register via MII
*
* RETURNS: OK or ERROR
*/

LOCAL STATUS auMiiWrite
    (
    AU_DRV_CTRL *	pDrvCtrl,
    UINT8		phyAdrs,	/* PHY address to access */
    UINT8		phyReg,		/* PHY register to write */
    UINT16		data		/* Data to write */
    )
    {
    int timeout;
    
    DRV_LOG (DRV_DEBUG_MII, "auMiiWrite adr=%02x reg=%02x data=%04x\n",
             phyAdrs, phyReg, data, 0, 0, 0);

    timeout = 20;
    while (AU_MII_CONTROL & AU_MII_CONTROL_BUSY)
	{
	taskDelay (1);
	if (--timeout == 0)
	    {
	    DRV_LOG (DRV_DEBUG_MII, "PHY access timeout\n",
		     0, 0, 0, 0, 0, 0);
	    return (ERROR);
	    }
	}

    AU_MII_DATA = data;
    AU_MII_CONTROL = ((phyAdrs << 11) | (phyReg << 6)
			  | AU_MII_CONTROL_WRITE);

    timeout = 20;
    while (AU_MII_CONTROL & AU_MII_CONTROL_BUSY)
	{
	taskDelay (1);
	if (--timeout == 0)
	    {
	    DRV_LOG (DRV_DEBUG_MII, "PHY access timeout\n",
		     0, 0, 0, 0, 0, 0);
	    return (ERROR);
	    }
	}

    return (OK);
    }

/***************************************************************************
*
* auPhyFind - Find the first PHY.
*
* RETURNS: Address of PHY or 0xFF if not found.
*/

LOCAL UINT8 auPhyFind
    (
    AU_DRV_CTRL *pDrvCtrl
    )
    {
    UINT16  miiData;
    UINT8   phyAddr;

    DRV_LOG (DRV_DEBUG_MII, "auMiiPhyFind\n",
             0, 0, 0, 0, 0, 0);

    for (phyAddr = 0; phyAddr < (UINT8)AU_MAX_PHY; phyAddr++) 
        {
         auMiiRead (pDrvCtrl, phyAddr, MII_PHY_ID0, &miiData);

        /* Verify PHY address read */

        if ((miiData != 0xFFFF) && (miiData != 0)) /* Found PHY */
            {
	    DRV_LOG (DRV_DEBUG_MII, "PHY @ %x\n", phyAddr, 0,0,0,0,0);
            return (phyAddr);
            }
        }

    DRV_LOG (DRV_DEBUG_LOAD, "No PHY found\n", 0,0,0,0,0,0);
    return (0xFF);
    }

/*******************************************************************************
*
* auStart - start the device
*
* This function calls BSP functions to connect interrupts and start the
* device running in interrupt mode.
*
* RETURNS: OK or ERROR
*/

LOCAL STATUS auStart
    (
    AU_DRV_CTRL * 	pDrvCtrl /* device to be initialized */
    )
    {
    STATUS 		result;

    pDrvCtrl->txCleaning = FALSE;
    pDrvCtrl->txBlocked  = FALSE;

    if (auMiiInit (pDrvCtrl) == ERROR)
	return (ERROR);

    SYS_INT_CONNECT (pDrvCtrl, auInt, (int)pDrvCtrl, &result);

    if (result == ERROR)
        return ERROR;

    /* mark the interface as up */

    END_FLAGS_SET (&pDrvCtrl->endObj, (IFF_UP | IFF_RUNNING));

    DRV_LOG (DRV_DEBUG_LOAD, "Interrupt connected.\n", 1, 2, 3, 4, 5, 6);

    SYS_INT_ENABLE (pDrvCtrl);

    DRV_LOG (DRV_DEBUG_LOAD, "interrupt enabled.\n", 1, 2, 3, 4, 5, 6);

    return (OK);
    }

/*******************************************************************************
*
* auInt - handle controller interrupt
*
* This routine is called at interrupt level in response to an interrupt from
* the controller.
*/

LOCAL void auInt
    (
    AU_DRV_CTRL * 	pDrvCtrl /* device to be initialized */
    )
    {
    int i;
    int txDone = FALSE;
    int rxDone = FALSE;

    DRV_LOG (DRV_DEBUG_INT, "auInt control=%08x rising=%08x, req0 %08x\n",
	     AU_MAC_CONTROL, AU_INTC_RISING_EDGE_CLEAR(0),
	     AU_INTC_REQUEST0_INT(0), 0, 0, 0);

    /* Acknowledge interrupt */

    SYS_INT_DISABLE(pDrvCtrl);
    SYS_WB_FLUSH();

    /* Examine receive buffers */

    for (i = 0; i < pDrvCtrl->rringSize; ++i)
	{
	if (AU_RX_ADDRESS(i) & (1 << 1))
	    rxDone = TRUE;
	}

    /* Examine transmit buffers */

    for (i = 0; i < pDrvCtrl->tringSize; ++i)
	{
	if (AU_TX_ADDRESS(i) & (1 << 1))
	    txDone = TRUE;
	}

    /* Have netTask handle all packets */
    if (rxDone || txDone)
	{
        netJobAdd ((FUNCPTR)auHandleInt,(int)pDrvCtrl, 0, 0, 0, 0);
	}
	
    if (pDrvCtrl->txBlocked)
        {
        pDrvCtrl->txBlocked = FALSE;
        netJobAdd ((FUNCPTR)muxTxRestart, (int)&pDrvCtrl->endObj, 0, 0, 0, 0);
        }
    }

/*******************************************************************************
*
* auHandleInt - task level interrupt service for all packets
*
* This routine is called at task level indirectly by the interrupt
* service routine to do any message received processing.
*/

LOCAL void auHandleInt
    (
    AU_DRV_CTRL * 	pDrvCtrl /* device to be initialized */
    )
    {

	/*
	 * Handle zero or more previously transmitted frames
	 */
	while (AU_TX_ADDRESS(pDrvCtrl->tmdLastIndex) & (1<<1))
	{
		AU_TX_ADDRESS(pDrvCtrl->tmdLastIndex) = 0;
    		pDrvCtrl->tmdLastIndex = (pDrvCtrl->tmdLastIndex + 1) &
					 (pDrvCtrl->tringSize - 1);
	}
	SYS_WB_FLUSH();

	/*
	 * Handle zero or more newly received frames
	 */
	while (AU_RX_ADDRESS(pDrvCtrl->rmdIndex) & (1<<1)) auRecv (pDrvCtrl);
	
	/* 
	 * Re-enable interrupts 
	 */
	SYS_INT_ENABLE(pDrvCtrl);
	SYS_WB_FLUSH();

    }

/*******************************************************************************
*
* auRecv - process the next incoming packet
*
* RETURNS: OK/ERROR
*/

LOCAL STATUS auRecv
    (
    AU_DRV_CTRL *	pDrvCtrl /* device to be initialized */
    )
    {
    int			len;
    M_BLK_ID		pMblk;
    char *		pCluster;
    char *		pBuf;
    CL_BLK_ID		pClBlk;
    UINT32		status;
    int			rmdIndex;

    /* Extract packet particulators */
	rmdIndex = pDrvCtrl->rmdIndex;

    status = AU_RX_STATUS(rmdIndex);
    len = AU_RX_STATUS (rmdIndex) & 0x3fff;	/* get packet length */
    pBuf = pDrvCtrl->pRxMem[rmdIndex];

    DRV_LOG (DRV_DEBUG_RX, "auRecv index = %d status = %08x len=%d\n",
	     rmdIndex, status, len, 0, 0, 0);
    DRV_LOG (DRV_DEBUG_RX, "auRecv dst=%02x:%02x:%02x:%02x:%02x:%02x\n",
	     pBuf[0]&0xff, pBuf[1]&0xff, pBuf[2]&0xff,
	     pBuf[3]&0xff, pBuf[4]&0xff, pBuf[5]&0xff);
    DRV_LOG (DRV_DEBUG_RX, "auRecv src=%02x:%02x:%02x:%02x:%02x:%02x\n",
	     pBuf[6]&0xff, pBuf[7]&0xff, pBuf[8]&0xff,
	     pBuf[9]&0xff, pBuf[10]&0xff, pBuf[11]&0xff);

    /* If error flag OR if packet is not completely in one buffer */

    if (!len)
	{
	DRV_LOG (DRV_DEBUG_RX, "RMD error!\n", 1, 2, 3, 4, 5, 6);
	END_ERR_ADD (&pDrvCtrl->endObj, MIB2_IN_ERRS, +1);
	goto cleanRXD;				/* skip to clean up */
	}

    /* If we cannot get a buffer to loan then bail out. */

    pCluster = netClusterGet (pDrvCtrl->endObj.pNetPool,
			      pDrvCtrl->pClPoolId);

    if (pCluster == NULL)
	{
	DRV_LOG (DRV_DEBUG_RX, "Cannot loan!\n", 1, 2, 3, 4, 5, 6);
	END_ERR_ADD (&pDrvCtrl->endObj, MIB2_IN_ERRS, +1);
        pDrvCtrl->lastError.errCode = END_ERR_NO_BUF;
        muxError(&pDrvCtrl->endObj, &pDrvCtrl->lastError);
	goto cleanRXD;
	}

    if ((pClBlk = netClBlkGet (pDrvCtrl->endObj.pNetPool, M_DONTWAIT)) == NULL)
	{
	netClFree (pDrvCtrl->endObj.pNetPool, pCluster);
	DRV_LOG (DRV_DEBUG_RX, "Out of Cluster Blocks!\n", 1, 2, 3, 4, 5, 6);
	END_ERR_ADD (&pDrvCtrl->endObj, MIB2_IN_ERRS, +1);
        pDrvCtrl->lastError.errCode = END_ERR_NO_BUF;
        muxError(&pDrvCtrl->endObj, &pDrvCtrl->lastError);
	goto cleanRXD;
	}

    /*
     * OK we've got a spare, let's get an M_BLK_ID and marry it to the
     * one in the ring.
     */

    if ((pMblk = mBlkGet(pDrvCtrl->endObj.pNetPool, M_DONTWAIT, MT_DATA))
	== NULL)
	{
	netClBlkFree (pDrvCtrl->endObj.pNetPool, pClBlk);
	netClFree (pDrvCtrl->endObj.pNetPool, pCluster);
	DRV_LOG (DRV_DEBUG_RX, "Out of M Blocks!\n", 1, 2, 3, 4, 5, 6);
	END_ERR_ADD (&pDrvCtrl->endObj, MIB2_IN_ERRS, +1);
        pDrvCtrl->lastError.errCode = END_ERR_NO_BUF;
        muxError(&pDrvCtrl->endObj, &pDrvCtrl->lastError);
	goto cleanRXD;
	}

    END_ERR_ADD (&pDrvCtrl->endObj, MIB2_IN_UCAST, +1);

    bcopy (pBuf, pCluster + pDrvCtrl->offset, len);

    /* Join the cluster to the MBlock */

    netClBlkJoin (pClBlk, pCluster, len, NULL, 0, 0, 0);
    netMblkClJoin (pMblk, pClBlk);

    pMblk->mBlkHdr.mData  += pDrvCtrl->offset;
    pMblk->mBlkHdr.mLen	  = len;
    pMblk->mBlkHdr.mFlags |= M_PKTHDR;
    pMblk->mBlkPktHdr.len = len;

    AU_RX_STATUS(rmdIndex) = 0;
    AU_RX_ADDRESS(rmdIndex)
      = ((UINT32) AU_CACHE_VIRT_TO_PHYS(pDrvCtrl->pRxMem[rmdIndex])) | 1;
	SYS_WB_FLUSH();

    /* Advance our management index */	
    pDrvCtrl->rmdIndex = (pDrvCtrl->rmdIndex + 1) & (pDrvCtrl->rringSize - 1);
	
    DRV_LOG (DRV_DEBUG_RX, "auRecv clean index %d %08x\n",
	     rmdIndex, AU_RX_ADDRESS(rmdIndex) & 0x1f, 0, 0, 0, 0);

    /* Call the upper layer's receive routine. */

    END_RCV_RTN_CALL(&pDrvCtrl->endObj, pMblk);

    return (OK);

cleanRXD:

    /* Restore the receiver buffer */

    AU_RX_STATUS(rmdIndex) = 0;
    AU_RX_ADDRESS(rmdIndex)
      = ((UINT32) AU_CACHE_VIRT_TO_PHYS(pDrvCtrl->pRxMem[rmdIndex])) | 1;
	SYS_WB_FLUSH();

	
    DRV_LOG (DRV_DEBUG_RX, "auRecv clean index %d %08x\n",
	     rmdIndex, AU_RX_ADDRESS(rmdIndex) & 0x1f, 0, 0, 0, 0);

    /* Advance our management index */

    pDrvCtrl->rmdIndex = (pDrvCtrl->rmdIndex + 1) & (pDrvCtrl->rringSize - 1);

    return (OK);
    }

/*******************************************************************************
*
* auSend - the driver send routine
*
* This routine takes a M_BLK_ID sends off the data in the M_BLK_ID.
* The buffer must already have the addressing information properly installed
* in it.  This is done by a higher layer.  The last arguments are a free
* routine to be called when the device is done with the buffer and a pointer
* to the argument to pass to the free routine.  
*
* RETURNS: OK or ERROR.
*/

LOCAL STATUS auSend
    (
    AU_DRV_CTRL * 	pDrvCtrl,  /* device to be initialized */
    M_BLK_ID 		pMblk      /* data to send */
    )
    {
    char *       	pBuf;
    int			index;
    int         	len = 0;
	UINT32 addr;

    DRV_LOG (DRV_DEBUG_TX, "auSend\n", 1, 2, 3, 4, 5, 6);

    /* check device mode */

    if (pDrvCtrl->flags & AU_POLLING)
        {
        netMblkClChainFree (pMblk);
        errno = EINVAL;
        return (ERROR);
        }

    /*
     * Obtain exclusive access to transmitter.  This is necessary because
     * we might have more than one stack transmitting at once.
     */

    END_TX_SEM_TAKE (&pDrvCtrl->endObj, WAIT_FOREVER);

    /* Advance our management index (first) */

    index = pDrvCtrl->tmdIndex;

    if (AU_TX_ADDRESS(index) & 0x03)
	{
        /* Are we still on the first chunk? */

        DRV_LOG (DRV_DEBUG_TX, "Out of TMDs! addr=%08x astat=%08x stat=%08x\n",
		 AU_TX_ADDRESS(index) & ~0x1f,
		 AU_TX_ADDRESS(index) & 0x1f,
		 AU_TX_STATUS(index), 4, 5, 6);
#ifdef DRV_DEBUG
	auDump (pDrvCtrl->unit);
#endif
        pDrvCtrl->txBlocked = TRUE;
        END_TX_SEM_GIVE (&pDrvCtrl->endObj);
        return (END_ERR_BLOCK);
        }

    pBuf = pDrvCtrl->pTxMem[index];
    DRV_LOG (DRV_DEBUG_TX, "pBuf = 0x%X\n", (int)pBuf, 2, 3, 4, 5, 6);
    if (pBuf == NULL)
        {
        pDrvCtrl->txBlocked = TRUE;
        END_TX_SEM_GIVE (&pDrvCtrl->endObj);
        return (END_ERR_BLOCK);
        }

    /* copy and release the packet */
    
    len = netMblkToBufCopy (pMblk, pBuf, NULL);
    netMblkClChainFree (pMblk);

    if (len < ETHERSMALL)
	{
	/* Pad to ETHERSMALL with zeros, required by H/W */
	bzero (&pBuf[len], ETHERSMALL - len);
	len = ETHERSMALL;
	}
    addr  = ((UINT32)AU_CACHE_VIRT_TO_PHYS(pDrvCtrl->pTxMem[index]) | 1);
	AU_TX_LENGTH(index) = len;
	AU_TX_STATUS(index) = 0;
    AU_TX_ADDRESS(index) = addr;
	SYS_WB_FLUSH();

    DRV_LOG (DRV_DEBUG_TX, "auSend: index %d len %d\n",
             index, len, 3, 4, 5, 6);

    DRV_LOG (DRV_DEBUG_TX, "auSend dst=%02x:%02x:%02x:%02x:%02x:%02x\n",
	     pBuf[0]&0xff, pBuf[1]&0xff, pBuf[2]&0xff,
	     pBuf[3]&0xff, pBuf[4]&0xff, pBuf[5]&0xff);
    DRV_LOG (DRV_DEBUG_TX, "auSend src=%02x:%02x:%02x:%02x:%02x:%02x\n",
	     pBuf[6]&0xff, pBuf[7]&0xff, pBuf[8]&0xff,
	     pBuf[9]&0xff, pBuf[10]&0xff, pBuf[11]&0xff);
    
    /* Advance our management index */

    index = (pDrvCtrl->tmdIndex + 1) & (pDrvCtrl->tringSize - 1);
    pDrvCtrl->tmdIndex = index;

    /* Bump the statistic counter. */

    END_ERR_ADD (&pDrvCtrl->endObj, MIB2_OUT_UCAST, +1);

    END_TX_SEM_GIVE (&pDrvCtrl->endObj);

    DRV_LOG (DRV_DEBUG_TX, "auSend Done\n", 1, 2, 3, 4, 5, 6);

    return (OK);
    }

/*******************************************************************************
*
* auIoctl - the driver I/O control routine
*
* Process an ioctl request.
*
* RETURNS OK or ERROR value
*/

LOCAL int auIoctl
    (
    AU_DRV_CTRL * 	pDrvCtrl, /* device to be initialized */
    int 		cmd,	  /* ioctl command to execute */
    caddr_t 		data	  /* data to get or set       */
    )
    {
    long 		value;
    int 		error = 0;

    switch (cmd)
        {
        case EIOCSADDR:
	    if (data == NULL)
		return (EINVAL);
            bcopy ((char *)data, (char *)END_HADDR(&pDrvCtrl->endObj),
		   END_HADDR_LEN (&pDrvCtrl->endObj));
            break;

        case EIOCGADDR:
	    if (data == NULL)
		return (EINVAL);
            bcopy ((char *)END_HADDR (&pDrvCtrl->endObj), (char *)data,
		    END_HADDR_LEN (&pDrvCtrl->endObj));
            break;

        case EIOCSFLAGS:
	    value = (long)data;
            
	    if (value < 0)
		{
		value = -value;
		value--;		/* HELP: WHY ??? */
		END_FLAGS_CLR (&pDrvCtrl->endObj, value);
		}
	    else
		{
		END_FLAGS_SET (&pDrvCtrl->endObj, value);
		}
	    auConfig (pDrvCtrl);
            break;
        case EIOCGFLAGS:
	    *(int *)data = END_FLAGS_GET (&pDrvCtrl->endObj);
            break;

        case EIOCMULTIADD:
            error = auMCastAddrAdd (pDrvCtrl, (char *) data);
            break;

        case EIOCMULTIDEL:
            error = auMCastAddrDel (pDrvCtrl, (char *) data);
            break;

        case EIOCMULTIGET:
            error = auMCastAddrGet (pDrvCtrl, (MULTI_TABLE *) data);
            break;

	case EIOCPOLLSTART:
	    error = auPollStart (pDrvCtrl);
	    break;

	case EIOCPOLLSTOP:
	    error = auPollStop (pDrvCtrl);
	    break;

        case EIOCGMIB2:
            if (data == NULL)
                return (EINVAL);
            bcopy((char *)&pDrvCtrl->endObj.mib2Tbl, (char *)data,
                  sizeof(pDrvCtrl->endObj.mib2Tbl));
            break;
        case EIOCGFBUF:
            if (data == NULL)
                return (EINVAL);
            *(int *)data = AU_MIN_FBUF;
            break;
        case EIOCGMWIDTH:
            if (data == NULL)
                return (EINVAL);
            *(int *)data = 4;
            break;
        case EIOCGHDRLEN:
            if (data == NULL)
                return (EINVAL);
            *(int *)data = 14;
            break;
        default:
            error = EINVAL;
        }
    return (error);
    }

/*******************************************************************************
*
* auReset - hardware reset of chip (stop it)
*
* This routine is responsible for resetting the device and switching into
* 32 bit mode.
*
* RETURNS: OK/ERROR
*/

LOCAL int auReset
    (
    AU_DRV_CTRL * 	pDrvCtrl /* device to be initialized */
    )
    {
    int		i;

    DRV_LOG (DRV_DEBUG_LOAD, "auReset\n", 1, 2, 3, 4, 5, 6);

    /* Reset MAC & DMA units */
    AU_MAC_ENABLE = 0;

    /* setup Rx memory pointers */

    pDrvCtrl->rmdIndex = 0;
    for (i = 0; i < pDrvCtrl->rringSize; ++i)
        {	
        AU_RX_STATUS(i) = 0;
        AU_RX_ADDRESS(i) = 0;
	}

    /* setup Tx memory pointers */

    pDrvCtrl->tmdIndex = 0;
    pDrvCtrl->tmdLastIndex = 0;
    for (i = 0; i < pDrvCtrl->tringSize; ++i)
	{
	AU_TX_STATUS(i) = 0;
	AU_TX_LENGTH(i) = 0;
	AU_TX_ADDRESS(i) = 0;
	}

	SYS_WB_FLUSH();

    return (OK);
    }

/*******************************************************************************
*
* auRestartSetup - setup memory descriptors and turn on chip
*
* This routine initializes all the shared memory structures and turns on
* the chip.
*
* RETURNS OK/ERROR
*/

LOCAL STATUS auRestartSetup
    (
    AU_DRV_CTRL * 	pDrvCtrl /* device to be initialized */
    )
    {  
    DRV_LOG (DRV_DEBUG_LOAD, "auRestartSetup\n", 1, 2, 3, 4, 5, 6);

    /* reset the device */

    auReset (pDrvCtrl);

    DRV_LOG (DRV_DEBUG_LOAD, "Memory setup complete\n", 1, 2, 3, 4, 5, 6);

    /* reconfigure the device */

    auConfig (pDrvCtrl);

    return (OK);
    }

/*******************************************************************************
*
* auRestart - restart the device after a fatal error
*
* This routine takes care of all the messy details of a restart.  The device
* is reset and re-initialized.  The driver state is re-synchronized.
*
* RETURNS: N/A
*/

LOCAL void auRestart
    (
    AU_DRV_CTRL * 	pDrvCtrl /* device to be initialized */
    )
    {
    auRestartSetup (pDrvCtrl);

    /* set the flags to indicate readiness */

    END_OBJ_READY (&pDrvCtrl->endObj,
                    IFF_UP | IFF_RUNNING | IFF_NOTRAILERS | IFF_BROADCAST
                    | IFF_MULTICAST);
    }

/******************************************************************************
*
* auConfig - reconfigure the interface under us.
*
* Reconfigure the interface setting promiscuous mode, and changing the
* multicast interface list.
*
* RETURNS: N/A
*/

LOCAL void auConfig
    (
    AU_DRV_CTRL * 	pDrvCtrl /* device to be initialized */
    )
    {
    UCHAR *		enetAddr;
    int i;

    DRV_LOG (DRV_DEBUG_LOAD, "auConfig\n", 1, 2, 3, 4, 5, 6);

    /* Set promiscuous mode if it's asked for. */

    if (END_FLAGS_GET (&pDrvCtrl->endObj) & IFF_PROMISC)
        {
        DRV_LOG (DRV_DEBUG_LOAD, "Setting promiscuous mode on!\n",
                 1, 2, 3, 4, 5, 6);

        /* chip will be in promiscuous mode */

        pDrvCtrl->flags |= AU_PROMISCUOUS_FLAG; 
        }
    else
        {
        DRV_LOG (DRV_DEBUG_LOAD, "Setting promiscuous mode off!\n",
                 1, 2, 3, 4, 5, 6);

        pDrvCtrl->flags &= ~AU_PROMISCUOUS_FLAG; 
        }

    /* Set up address filter for multicasting. */

    if (END_MULTI_LST_CNT (&pDrvCtrl->endObj) > 0)
        {
        auAddrFilterSet (pDrvCtrl);
        }

    /* Enable the MAC */

    AU_MAC_ENABLE = AU_MAC_ENABLE_CLOCKS_ONLY;
    SYS_WB_FLUSH();
    AU_MAC_ENABLE = AU_MAC_ENABLE_NORMAL & ~AU_MAC_ENABLE_TS;
    SYS_WB_FLUSH();

    /* Configure the MAC */

#if (_BYTE_ORDER == _BIG_ENDIAN)
    AU_MAC_CONTROL = AU_MAC_CONTROL_EM_BIG | AU_MAC_CONTROL_DO;
#else
    AU_MAC_CONTROL = AU_MAC_CONTROL_DO;
#endif

    enetAddr = pDrvCtrl->enetAddr;
    AU_MAC_ADDRESS_HIGH = (enetAddr[5] << 8) | enetAddr[4];
    AU_MAC_ADDRESS_LOW = ((enetAddr[3] << 24)
			      | (enetAddr[2] << 16)
			      | (enetAddr[1] << 8)
			      | (enetAddr[0] << 0));
    DRV_LOG (DRV_DEBUG_LOAD, "mac   addr %08x %08x\n",
	     AU_MAC_ADDRESS_HIGH,
	     AU_MAC_ADDRESS_LOW, 0, 0, 0, 0);
    DRV_LOG (DRV_DEBUG_LOAD, "multi addr %08x %08x\n",
	     AU_MULTICAST_HASH_ADDRESS_HIGH,
	     AU_MULTICAST_HASH_ADDRESS_LOW, 0, 0, 0, 0);

    /* Configure MII */

    /* Setup RxD and TxD pointers */

    pDrvCtrl->rmdIndex = (AU_RX_ADDRESS(0) >> 2) & 0x03;
    pDrvCtrl->tmdIndex = (AU_TX_ADDRESS(0) >> 2) & 0x03;
    pDrvCtrl->tmdLastIndex = pDrvCtrl->tmdIndex;

#ifdef AU_PROMISCUOS_MODE
    /* Handle promiscuous mode */
    if (pDrvCtrl->flags & AU_PROMISCUOUS_FLAG)
	AU_MAC_CONTROL |= AU_MAC_CONTROL_PR;
#endif
    for (i = 0; i < pDrvCtrl->rringSize; ++i)
        AU_RX_ADDRESS(i) 
          = ((UINT32) AU_CACHE_VIRT_TO_PHYS(pDrvCtrl->pRxMem[i])) | 1;

    /* Enable transmit and receive */
    AU_MAC_CONTROL |= AU_MAC_CONTROL_TE | AU_MAC_CONTROL_RE;
    SYS_WB_FLUSH();
    }

/******************************************************************************
*
* auAddrFilterSet - set the address filter for multicast addresses
*
* This routine goes through all of the multicast addresses on the list
* of addresses (added with the auAddrAdd() routine) and sets the
* device's filter correctly.
*
* RETURNS: N/A
*/

LOCAL void auAddrFilterSet
    (
    AU_DRV_CTRL * 	pDrvCtrl /* device to be initialized */
    )
    {
    ETHER_MULTI * 	pCurr;
    UINT8 *		pCp;
    UINT8 		byte;
    UINT32 		crc;
    int 		len;
    int 		count;

    pCurr = END_MULTI_LST_FIRST (&pDrvCtrl->endObj);

    while (pCurr != NULL)
	{
	pCp = (UINT8 *)&pCurr->addr;

	crc = 0xffffffff;

	for (len = AU_LA_LEN; --len >= 0;)
	    {
	    byte = *pCp++;

            for (count = 0; count < AU_LAF_LEN; count++)
		{
		if ((byte & 0x01) ^ (crc & 0x01))
		    {
		    crc >>= 1;
		    crc = crc ^ AU_CRC_POLYNOMIAL;
		    }
		else
		    {
		    crc >>= 1;
		    }
		byte >>= 1;
		}
	    }

	/* Just want the 6 most significant bits. */

	crc = AU_CRC_TO_LAF_IX (crc);

	pCurr = END_MULTI_LST_NEXT(pCurr);
	}
    }

/*******************************************************************************
*
* auPollReceive - routine to receive a packet in polled mode.
*
* This routine is called by a user to try and get a packet from the
* device. This routine return OK if it is successful in getting the packet
*
* RETURNS: OK or EAGAIN.
*/

LOCAL STATUS auPollReceive
    (
    AU_DRV_CTRL * 	pDrvCtrl, /* device to be initialized */
    M_BLK_ID 		pMblk
    )
    {
    int                 len;
    char *              pBuf;
    UINT32              status, ERROR_MASK=0x812bc000;
    int                 rmdIndex,oldLevel;

    DRV_LOG (DRV_DEBUG_POLL_RX, "PRX b\n", 1, 2, 3, 4, 5, 6);

 /*
  * Handle zero or more newly received frames
  */
if (!(AU_RX_ADDRESS(pDrvCtrl->rmdIndex) & (1<<1)))
    {
    pDrvCtrl->rmdIndex = (pDrvCtrl->rmdIndex + 1) & (pDrvCtrl->rringSize - 1);
    /* goto cleanRx; */
    return (EAGAIN);
    }

    /* Extract packet particulators */
    rmdIndex = pDrvCtrl->rmdIndex;
    /* Read the device status register */
    status = AU_RX_STATUS(rmdIndex);
    len = AU_RX_STATUS (rmdIndex) & 0x3fff; /* get packet length */
    pBuf = pDrvCtrl->pRxMem[rmdIndex];

    if (status & (ERROR_MASK & ~0x0))
        {
        DRV_LOG (DRV_DEBUG_POLL_RX, "PRX bad error\n", 1, 2, 3, 4, 5, 6);
	END_ERR_ADD (&pDrvCtrl->endObj, MIB2_IN_ERRS, +1);

        /* restart chip on fatal error */
        if (status & ERROR_MASK & 0x0)        /* memory error */
            {
	    END_FLAGS_CLR (&pDrvCtrl->endObj, (IFF_UP | IFF_RUNNING)); 
            DRV_LOG (DRV_DEBUG_POLL_RX, "PRX restart\n", 1, 2, 3, 4, 5, 6);
            auRestart (pDrvCtrl);
            return (EAGAIN);
            }
        /* goto cleanRx;  */
        goto cleanRx;
        }

    if (!len)
        {
        DRV_LOG (DRV_DEBUG_RX, "RMD error!\n", 1, 2, 3, 4, 5, 6);
        END_ERR_ADD (&pDrvCtrl->endObj, MIB2_IN_ERRS, +1);
        goto cleanRx;                          /* skip to clean up */
        }

    END_ERR_ADD (&pDrvCtrl->endObj, MIB2_IN_UCAST, +1);

    /* Upper layer provides the buffer. */

    if ((pMblk->mBlkHdr.mLen < len) || (!(pMblk->mBlkHdr.mFlags & M_EXT)))
	{
        DRV_LOG (DRV_DEBUG_POLL_RX, "PRX bad mblk len:%d flags:%d\n",
                 pMblk->mBlkHdr.mLen, pMblk->mBlkHdr.mFlags, 3, 4, 5, 6);
	return (EAGAIN);
	}
    pMblk->mBlkHdr.mData  += pDrvCtrl->offset;
    bcopy (pBuf, pMblk->mBlkHdr.mData, len);

    pMblk->mBlkHdr.mLen = len;
    pMblk->mBlkHdr.mFlags |= M_PKTHDR;
    pMblk->mBlkPktHdr.len = len;

cleanRx:
    /* Restore the receiver buffer */

    AU_RX_STATUS(rmdIndex) = 0;
    AU_RX_ADDRESS(rmdIndex)
      = ((UINT32) AU_CACHE_VIRT_TO_PHYS(pDrvCtrl->pRxMem[rmdIndex])) | 1;
    SYS_WB_FLUSH();

    DRV_LOG (DRV_DEBUG_RX, "auPollReceive clean index %d %08x\n",
             rmdIndex, AU_RX_ADDRESS(rmdIndex) & 0x1f, 0, 0, 0, 0);

    /* Advance our management index */
    pDrvCtrl->rmdIndex = (pDrvCtrl->rmdIndex + 1) & (pDrvCtrl->rringSize - 1);

    DRV_LOG (DRV_DEBUG_POLL_RX, "PRX ok\n", 1, 2, 3, 4, 5, 6);

    return (OK);
    }

/*******************************************************************************
*
* auPollSend - routine to send a packet in polled mode.
*
* This routine is called by a user to try and send a packet on the
* device.
*
* RETURNS: OK or EAGAIN.
*/

LOCAL STATUS auPollSend
    (
    AU_DRV_CTRL * 	pDrvCtrl, /* device to be initialized */
    M_BLK_ID 		pMblk
    )
    {
    char *       	pBuf;
    int			index;
    int         	len = 0;
    int         	oldLevel;
    UINT32 addr;

    DRV_LOG (DRV_DEBUG_POLL_TX, "PTX b\n", 1, 2, 3, 4, 5, 6);

    /* Advance our management index (first) */
    index = pDrvCtrl->tmdIndex;

    if (AU_TX_ADDRESS(index) & 0x01) 
        {
        DRV_LOG (DRV_DEBUG_POLL_TX, "Out of tmds.\n", 1, 2, 3, 4, 5, 6);
        
        if (!pDrvCtrl->txCleaning) 
	    auTRingScrub (pDrvCtrl);
	return (EAGAIN);
        }

    pBuf = pDrvCtrl->pTxMem[index];
    
    if (pBuf == NULL)
         return (EAGAIN);

    /* copy and release the packet */
    len = netMblkToBufCopy (pMblk, pBuf, NULL);

    if (len < ETHERSMALL)
	{
	/* Pad to ETHERSMALL with zeros, required by H/W */
	bzero (&pBuf[len], ETHERSMALL - len);
	len = ETHERSMALL;
	}

    addr  = ((UINT32)AU_CACHE_VIRT_TO_PHYS(pDrvCtrl->pTxMem[index]) | 1);
    AU_TX_LENGTH(index) = len;
    AU_TX_STATUS(index) = 0;
    AU_TX_ADDRESS(index) = addr;
    SYS_WB_FLUSH();

    /* Bump the statistic counter. */

    END_ERR_ADD (&pDrvCtrl->endObj, MIB2_OUT_UCAST, +1);

    /* Spin until we think we've sent it.  */
    
    while (!(AU_TX_ADDRESS(index) & (1<<1)));
    AU_TX_ADDRESS(index) = 0;
    AU_TX_STATUS(index) = 0;
    AU_TX_LENGTH(index) = 0;

    SYS_WB_FLUSH();
    
    index = (pDrvCtrl->tmdIndex + 1) & (pDrvCtrl->tringSize - 1);
    pDrvCtrl->tmdIndex = index;
    pDrvCtrl->tmdLastIndex = (pDrvCtrl->tmdLastIndex + 1) &
                              (pDrvCtrl->tringSize - 1);

    DRV_LOG (DRV_DEBUG_POLL_TX, "PTX e\n", 1, 2, 3, 4, 5, 6);

    return (OK);
    }

/*****************************************************************************
*
* auMCastAddrAdd - add a multicast address for the device
*
* This routine adds a multicast address to whatever the driver
* is already listening for.  It then resets the address filter.
*
* RETURNS: OK
*/

LOCAL STATUS auMCastAddrAdd
    (
    AU_DRV_CTRL * 	pDrvCtrl, /* device to be initialized */
    char * 		pAddress
    )
    {
#ifdef AU_END_MULTICAST
    int 		error;

    if ((error = etherMultiAdd (&pDrvCtrl->endObj.multiList,
		pAddress)) == ENETRESET)
	    auConfig (pDrvCtrl);
#endif
    return (OK);
    }

/*****************************************************************************
*
* auMCastAddrDel - delete a multicast address for the device
*
* This routine removes a multicast address from whatever the driver
* is listening for.  It then resets the address filter.
*
* RETURNS: OK.
*/

LOCAL STATUS auMCastAddrDel
    (
    AU_DRV_CTRL * 	pDrvCtrl, /* device to be initialized */
    char * 		pAddress
    )
    {
#if AU_END_MULTICAST
    int 		error;

    if ((error = etherMultiDel (&pDrvCtrl->endObj.multiList,
	     (char *)pAddress)) == ENETRESET)
	    auConfig (pDrvCtrl);
#endif
    return (OK);
    }

/*****************************************************************************
*
* auMCastAddrGet - get the multicast address list for the device
*
* This routine gets the multicast list of whatever the driver
* is already listening for.
*
* RETURNS: OK/ERROR
*/

LOCAL STATUS auMCastAddrGet
    (
    AU_DRV_CTRL * 	pDrvCtrl, /* device to be initialized */
    MULTI_TABLE * 	pTable
    )
    {
    int 		error;

    error = etherMultiGet (&pDrvCtrl->endObj.multiList, pTable);

    return (error);
    }

/*******************************************************************************
*
* auStop - stop the device
*
* This function calls BSP functions to disconnect interrupts and stop
* the device from operating in interrupt mode.
*
* RETURNS: OK or ERROR
*/

LOCAL STATUS auStop
    (
    AU_DRV_CTRL * 	pDrvCtrl /* device to be initialized */
    )
    {
    STATUS 		result = OK;

    /* Stop the device. */

    auReset (pDrvCtrl);

    SYS_INT_DISABLE (pDrvCtrl);

    /* mark the interface as down */

    END_FLAGS_CLR (&pDrvCtrl->endObj, (IFF_UP | IFF_RUNNING));

    /* disconnect the interrupt handler */

    SYS_INT_DISCONNECT (pDrvCtrl, auInt, (int)pDrvCtrl, &result);

    if (result == ERROR)
	{
	DRV_LOG (DRV_DEBUG_LOAD, "Could not disconnect interrupt!\n",
                 1, 2, 3, 4, 5, 6);
	}

    return (result);
    }

/******************************************************************************
*
* auUnload - unload a driver from the system
*
* This function first brings down the device, and then frees any
* stuff that was allocated by the driver in the load function. The controller
* structure should be free by who ever is calling this function.
*
* RETURNS: OK
*/

LOCAL STATUS auUnload
    (
    AU_DRV_CTRL * 	pDrvCtrl /* device to be initialized */
    )
    {
    END_OBJECT_UNLOAD (&pDrvCtrl->endObj);

    /* Free the shared DMA memory. */

    if (pDrvCtrl->pRxMem)
	{
	cacheDmaFree (pDrvCtrl->pRxMemBase);
	pDrvCtrl->pRxMemBase = NULL;
	}
    
    if (pDrvCtrl->pTxMem)
	{
	cacheDmaFree (pDrvCtrl->pTxMemBase);
	pDrvCtrl->pTxMemBase = NULL;
	}

    /* Free the shared DMA memory allocated for clusters */
    
    if (pDrvCtrl->flags & AU_MEM_ALLOC_FLAG)
        cacheDmaFree (pDrvCtrl->clDesc.memArea);

    /* Free the memory allocated for mBlks and clBlks */
    
    if (pDrvCtrl->mClCfg.memArea != NULL)
        free (pDrvCtrl->mClCfg.memArea);

    /* Free the memory allocated for driver pool structure */

    if (pDrvCtrl->endObj.pNetPool != NULL)
        free (pDrvCtrl->endObj.pNetPool);

    return (OK);
    }


/*******************************************************************************
*
* auPollStart - start polled mode operations
*
* RETURNS: OK or ERROR.
*/

LOCAL STATUS auPollStart
    (
    AU_DRV_CTRL * 	pDrvCtrl /* device to be initialized */
    )
    {

    SYS_INT_DISABLE(pDrvCtrl);

    pDrvCtrl->flags |= AU_POLLING;

    DRV_LOG (DRV_DEBUG_POLL, "POLL STARTED\n", 1, 2, 3, 4, 5, 6);

    return (OK);
    }

/*******************************************************************************
*
* auPollStop - stop polled mode operations
*
* RETURNS: OK or ERROR.
*/

LOCAL STATUS auPollStop
    (
    AU_DRV_CTRL * 	pDrvCtrl /* device to be initialized */
    )
    {

    SYS_INT_ENABLE(pDrvCtrl);

    pDrvCtrl->flags &= ~AU_POLLING;

    DRV_LOG (DRV_DEBUG_POLL, "STOPPED\n", 1, 2, 3, 4, 5, 6);

    return (OK);
    }


/******************************************************************************
*
* auTRingScrub - clean the transmit ring
*
* RETURNS: N/A
*
*/

LOCAL void auTRingScrub
    (
    AU_DRV_CTRL * 	pDrvCtrl /* device to be initialized */
    )
    {
    
    pDrvCtrl->txCleaning = TRUE;
    
    do
        {
        /* if the buffer is still owned by the MAC, don't touch it */

	if (AU_TX_ADDRESS(pDrvCtrl->tmdLastIndex) & (1 << 1))
	    break;

	/* Clear the TX entry */

        DRV_LOG (DRV_DEBUG_TX, "auTRingScrub %d\n",
		 pDrvCtrl->tmdLastIndex, 0, 0, 0, 0, 0);

	AU_TX_STATUS(pDrvCtrl->tmdLastIndex) = 0;
	AU_TX_LENGTH(pDrvCtrl->tmdLastIndex) = 0;
	AU_TX_ADDRESS(pDrvCtrl->tmdLastIndex) = 0;

        /* Increment the tmd disposal index pointer around the ring */

        pDrvCtrl->tmdLastIndex = (pDrvCtrl->tmdLastIndex + 1) &
			      (pDrvCtrl->tringSize - 1);
        }
    while (pDrvCtrl->tmdLastIndex != pDrvCtrl->tmdIndex);

    pDrvCtrl->txCleaning = FALSE;
    }

/******************************************************************************
*
* auDump - display device status
*
*/

void auDump
    (
    int unit
    )
    {
#ifdef DRV_DEBUG
    int i;
    AU_DRV_CTRL * 	pDrvCtrl = dbgDrvCtrl[unit];

    /* Examine receive buffers */

    printf ("Rx index %d (rmdIndex=%d)\n",
	    (AU_RX_ADDRESS(0) >> 2) & 0x03,
	    pDrvCtrl->rmdIndex);
    for (i = 0; i < pDrvCtrl->rringSize; ++i)
	{
	printf ("rx%d addr=%08x astat=%02x stat=%08x len=%d\n", i,
		AU_RX_ADDRESS(i) & ~0x1f,
		AU_RX_ADDRESS(i) & 0x1f,
		AU_RX_STATUS(i) & 0xffffc000,
		AU_RX_STATUS(i) & 0x00003fff);
	    printf ("receive %d done\n", AU_RX_ADDRESS(i) & (1 << 1) ? 1 : 0);
	}

    /* Examine transmit buffers */

    printf ("Tx index %d\n",
	    (AU_TX_ADDRESS(0) >> 2) & 0x03);
    for (i = 0; i < pDrvCtrl->tringSize; ++i)
	{
	printf ("tx%d addr=%08x astat=%02x stat=%08x len=%d\n", i,
		AU_TX_ADDRESS(i) & ~0x1f,
		AU_TX_ADDRESS(i) & 0x1f,
		AU_TX_STATUS(i),
		AU_TX_LENGTH(i));
	    printf ("transmit %d done\n", AU_TX_ADDRESS(i) & (1 << 1) ? 1 : 0);
	}
#endif
    }
