/* el3c90xEnd.c - END  network interface driver for 3COM 3C90xB XL */

/* Copyright 1984 - 2002 Wind River Systems, Inc. */

#include "copyright_wrs.h"

/*
modification history
--------------------
01o,22jan02,rcs  facilitated common source base with either RFC 1213 or
                 RFC 2233 depending on INCLUDE_RFC_1213 macro.
01n,28aug01,ann  adding the code that copies the pMib2Tbl contents into
                 mib2Tbl for backward compatibility - SPR 69697 & 69758
01m,16aug01,ann  fix for SPR 69673 : we now set the RFC2233 flag in the
                 END_OBJ_READY macro call.
01l,13jul01,ann  eliminating the mibStyle variable and accomodated the
                 END_MIB_2233 flag in the END flags field
01k,29mar01,spm  merged changes from version 01e of tor2_0.open_stack
                 branch (wpwr VOB, base 01b) for unified code base
01j,10nov00,mks  passing in correct type of pointer in strtok_r.
                 (SPR 35435 & 62224).
01i,20sep00,dat  SPR 32058, allow for alternate intConnect rtn
01h,28aug00,stv  corrected the handling of EIOCSFLAGS ioctl (SPR# 29423).
01g,08aug00,stv	 IFF_UP flag set in the el3c90xStart routine (SPR# 32893,32344,
		 32118).
01f,02aug00,stv	 removed netMblkClChainFree() in Pollsend routine (SPR# 32885).
01e,11jun00,ham  removed reference to etherLib.
01d,27jan00,dat  fixed use of NULL
01c,01dec99,stv  freed mBlk chain before returning ERROR (SPR #28492).
01b,11jan99,mtl	 big endian fix to eprm read rtn.
01a,11jan99,mtl	 written by teamF1 Inc.
*/

/*
DESCRIPTION  
This module implements the device driver for the 3COM EtherLink Xl and
Fast EtherLink XL PCI network interface cards.

The 3c90x PCI ethernet controller is inherently little endian because
the chip is designed to operate on a PCI bus which is a little endian
bus. The software interface to the driver is divided into three parts.
The first part is the PCI configuration registers and their set up. 
This part is done at the BSP level in the various BSPs which use this
driver. The second and third part are dealt in the driver. The second
part of the interface comprises of the I/O control registers and their
programming. The third part of the interface comprises of the descriptors
and the buffers. 

This driver is designed to be moderately generic, operating unmodified
across the range of architectures and targets supported by VxWorks.  To
achieve this, the driver must be given several target-specific parameters,
and some external support routines must be provided. These target-specific
values and the external support routines are described below.

This driver supports multiple units per CPU.  The driver can be
configured to support big-endian or little-endian architectures.  It
contains error recovery code to handle known device errata related to DMA
activity. 

Big endian processors can be connected to the PCI bus through some controllers
which take care of hardware byte swapping. In such cases all the registers 
which the chip DMA s to have to be swapped and written to, so that when the
hardware swaps the accesses, the chip would see them correctly. The chip still
has to be programmed to operated in little endian mode as it is on the PCI bus.
If the cpu board hardware automatically swaps all the accesses to and from the
PCI bus, then input and output byte stream need not be swapped.

The 3c90x series chips use a bus-master DMA interface for transfering
packets to and from the controller chip. Some of the old 3c59x cards
also supported a bus master mode, however for those chips
you could only DMA packets to and from a contiguous memory buffer. For
transmission this would mean copying the contents of the queued M_BLK
chain into a an M_BLK cluster and then DMAing the cluster. This extra
copy would sort of defeat the purpose of the bus master support for
any packet that doesn't fit into a single M_BLK. By contrast, the 3c90x cards
support a fragment-based bus master mode where M_BLK chains can be
encapsulated using TX descriptors. This is also called the gather technique,
where the fragments in an mBlk chain are directly incorporated into the
download transmit descriptor. This avoids any copying of data from the
mBlk chain. 

NETWORK CARDS SUPPORTED:

	- 3Com 3c900-TPO 10Mbps/RJ-45
	- 3Com 3c900-COMBO 10Mbps/RJ-45,AUI,BNC
        - 3Com 3c905-TX	10/100Mbps/RJ-45
        - 3Com 3c905-T4	10/100Mbps/RJ-45
        - 3Com 3c900B-TPO 10Mbps/RJ-45
        - 3Com 3c900B-COMBO 10Mbps/RJ-45,AUI,BNC
        - 3Com 3c905B-TX 10/100Mbps/RJ-45
        - 3Com 3c905B-FL/FX 10/100Mbps/Fiber-optic
        - 3Com 3c980-TX	10/100Mbps server adapter
        - Dell Optiplex GX1 on-board 3c918 10/100Mbps/RJ-45
        
BOARD LAYOUT
This device is on-board.  No jumpering diagram is necessary.

EXTERNAL INTERFACE
The only external interface is the el3c90xEndLoad() routine, which expects
the <initString> parameter as input.  This parameter passes in a 
colon-delimited string of the format:

<unit>:<devMemAddr>:<devIoAddr>:<pciMemBase:<vecNum>:<intLvl>:<memAdrs>:
<memSize>:<memWidth>:<flags>:<buffMultiplier>

The el3c90xEndLoad() function uses strtok() to parse the string.

TARGET-SPECIFIC PARAMETERS
.IP <unit>
A convenient holdover from the former model.  This parameter is used only
in the string name for the driver.

.IP <devMemAddr>
This parameter in the memory base address of the device registers in the
memory map of the CPU. It indicates to the driver where to find the
register set. < This parameter should be equal to NONE if the device
does not support memory mapped registers.


.IP <devIoAddr>
This parameter in the IO base address of the device registers in the
IO map of some CPUs. It indicates to the driver where to find the RDP
register. If both <devIoAddr> and <devMemAddr> are given then the device
chooses <devMemAddr> which is a memory mapped register base address.
This parameter should be equal to NONE if the device does not support 
IO mapped registers.


. <pciMemBase>
This parameter is the base address of the CPU memory as seen from the
PCI bus. This parameter is zero for most intel architectures.

.IP <vecNum>
This parameter is the vector associated with the device interrupt.
This driver configures the LANCE device to generate hardware interrupts
for various events within the device; thus it contains
an interrupt handler routine.  The driver calls intConnect() to connect 
its interrupt handler to the interrupt vector generated as a result of 
the LANCE interrupt. The BSP can use a different routine for interrupt
connection by changing the point el3c90xIntConnectRtn to point to a
different routine.

.IP <intLvl>
Some targets use additional interrupt controller devices to help organize
and service the various interrupt sources.  This driver avoids all
board-specific knowledge of such devices.  During the driver's
initialization, the external routine sysEl3c90xIntEnable() is called to
perform any board-specific operations required to allow the servicing of a
NIC interrupt.  For a description of sysEl3c90xIntEnable(), see "External
Support Requirements" below.

.IP <memAdrs>
This parameter gives the driver the memory address to carve out its
buffers and data structures. If this parameter is specified to be
NONE then the driver allocates cache coherent memory for buffers
and descriptors from the system pool.
The 3C90x NIC is a DMA type of device and typically shares access to
some region of memory with the CPU.  This driver is designed for systems
that directly share memory between the CPU and the NIC.  It
assumes that this shared memory is directly available to it
without any arbitration or timing concerns.

.IP <memSize>
This parameter can be used to explicitly limit the amount of shared
memory (bytes) this driver will use.  The constant NONE can be used to
indicate no specific size limitation.  This parameter is used only if
a specific memory region is provided to the driver.

.IP <memWidth>
Some target hardware that restricts the shared memory region to a
specific location also restricts the access width to this region by
the CPU.  On these targets, performing an access of an invalid width
will cause a bus error.

This parameter can be used to specify the number of bytes of access
width to be used by the driver during access to the shared memory.
The constant NONE can be used to indicate no restrictions.

Current internal support for this mechanism is not robust; implementation 
may not work on all targets requiring these restrictions.

.IP <flags>
This is parameter is used for future use, currently its value should be
zero.

.IP <buffMultiplier>
This parameter is used increase the number of buffers allocated in the
driver pool. If this parameter is -1 then a default multiplier of 2 is
choosen. With a multiplier of 2 the total number of clusters allocated
is 64 which is twice the cumulative number of upload and download descriptors.
The device has 16 upload and 16 download descriptors. For example on choosing
the buffer multiplier of 3, the total number of clusters allocated will be
96 ((16 + 16)*3). There are as many clBlks as the number of clusters.
The number of mBlks allocated are twice the number of clBlks.
By default there are 64 clusters, 64 clBlks and 128 mBlks allocated in the
pool for the device. Depending on the load of the system increase the
number of clusters allocated by incrementing the buffer multiplier.

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
    SYS_DELAY (delay)
    sysEl3c90xIntEnable(pDrvCtrl->intLevel) 
    sysEl3c90xIntDisable(pDrvCtrl->intLevel)
    sysDelay (delay)
.CE

There are default values in the source code for these macros.  They presume
memory mapped accesses to the device registers and the normal intConnect(),
and intEnable() BSP functions.  The first argument to each is the device
controller structure. Thus, each has access back to all the device-specific
information.  Having the pointer in the macro facilitates the addition 
of new features to this driver.

The macros SYS_INT_CONNECT, SYS_INT_DISCONNECT, SYS_INT_ENABLE and
SYS_INT_DISABLE allow the driver to be customized for BSPs that use special
versions of these routines.

The macro SYS_INT_CONNECT is used to connect the interrupt handler to
the appropriate vector.  By default it is the routine intConnect().

The macro SYS_INT_DISCONNECT is used to disconnect the interrupt handler prior
to unloading the module.  By default this is a dummy routine that
returns OK.

The macro SYS_INT_ENABLE is used to enable the interrupt level for the
end device.  It is called once during initialization.  It calls an
external board level routine sysEl3c90xIntEnable(). 

The macro SYS_INT_DISABLE is used to disable the interrupt level for the
end device.  It is called during stop.  It calls an
external board level routine sysEl3c90xIntDisable().

The macro SYS_DELAY is used for a delay loop. It calls an external board
level routine sysDelay(delay). The granularity of delay is one microsecond.

SYSTEM RESOURCE USAGE
When implemented, this driver requires the following system resources:

    - one mutual exclusion semaphore
    - one interrupt vector
    - 24072 bytes in text for a I80486 target
    - 112 bytes in the initialized data section (data)
    - 0 bytes in the uninitialized data section (BSS)

The driver allocates clusters of size 1536 bytes for receive frames and
and transmit frames. There are 16 descriptors in the upload ring
and 16 descriptors in the download ring. The buffer multiplier by default
is 2, which means that the total number of clusters allocated by default
are 64 ((upload descriptors + download descriptors)*2). There are as many
clBlks as the number of clusters. The number of mBlks allocated are twice
the number of clBlks. By default there are 64 clusters, 64 clBlks and 128
mBlks allocated in the pool for the device. Depending on the load of the
system increase the number of clusters allocated by incrementing the buffer
multiplier.


INCLUDES:
end.h endLib.h etherMultiLib.h el3c90xEnd.h

SEE ALSO: muxLib, endLib, netBufLib
.pG "Writing and Enhanced Network Driver"

.SH "BIBLIOGRAPHY"
.iB "3COM 3c90x and 3c90xB NICs Technical reference."
*/

#include "vxWorks.h"
#include "wdLib.h"
#include "stdlib.h"
#include "taskLib.h"
#include "tickLib.h"
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
#include "etherMultiLib.h"		/* multicast stuff. */
#include "end.h"			/* Common END structures. */
#include "endLib.h"
#include "lstLib.h"			/* Needed to maintain protocol list. */
#include "netBufLib.h"
#include "muxLib.h"
#include "m2Lib.h"
#include "drv/end/el3c90xEnd.h"

#ifndef EL3C90X_CACHE_INVALIDATE
#define EL3C90X_CACHE_INVALIDATE(address, len) \
        CACHE_DRV_INVALIDATE (&pDrvCtrl->cacheFuncs, (address), (len))
#endif /* EL3C90X_CACHE_INVALIDATE */   
    
#ifndef EL3C90X_CACHE_VIRT_TO_PHYS
#define EL3C90X_CACHE_VIRT_TO_PHYS(address) \
    (MEM_TO_PCI_PHYS((UINT32)(CACHE_DRV_VIRT_TO_PHYS \
                                      (&pDrvCtrl->cacheFuncs,(address)))))
#endif /* EL3C90X_CACHE_VIRT_TO_PHYS */

/* memory to PCI address translation macro */

#ifndef MEM_TO_PCI_PHYS
#define MEM_TO_PCI_PHYS(memAdrs) \
    ((memAdrs) + (pDrvCtrl->pciMemBase))
#endif    
    
/*
 * Default macro definitions for BSP interface.
 * These macros can be redefined in a wrapper file, to generate
 * a new module with an optimized interface.
 */

/* Macro to connect interrupt handler to vector */

#ifndef SYS_INT_CONNECT
#define SYS_INT_CONNECT(pDrvCtrl,rtn,arg,pResult) \
    { \
    *pResult = (*el3c90xIntConnectRtn) ((VOIDFUNCPTR *) \
			INUM_TO_IVEC (pDrvCtrl->ivec), \
		       rtn, (int)arg); \
    }
#endif

/* Macro to disconnect interrupt handler from vector */

#ifndef SYS_INT_DISCONNECT
#define SYS_INT_DISCONNECT(pDrvCtrl,rtn,arg,pResult) \
    { \
    *pResult = OK;  \
    }
#endif

/* Macro to enable the appropriate interrupt level */

#ifndef SYS_INT_ENABLE
#define SYS_INT_ENABLE(pDrvCtrl) \
    { \
    IMPORT STATUS sysEl3c90xIntEnable(); \
    sysEl3c90xIntEnable ((pDrvCtrl)->intLevel); \
    }
#endif /* SYS_INT_ENABLE*/

/* Macro to disable the appropriate interrupt level */

#ifndef SYS_INT_DISABLE
#define SYS_INT_DISABLE(pDrvCtrl) \
    { \
    IMPORT void sysEl3c90xIntDisable (); \
    sysEl3c90xIntDisable ((pDrvCtrl)->intLevel); \
    }
#endif

#ifndef SYS_OUT_LONG
#define SYS_OUT_LONG(pDrvCtrl,addr,value) \
    { \
    *((ULONG *)(addr)) = PCI_SWAP((value)); \
    }
#endif /* SYS_OUT_LONG */

#ifndef SYS_IN_LONG
#define SYS_IN_LONG(pDrvCtrl,addr,data) \
    { \
    ((data) = PCI_SWAP(*((ULONG *)(addr)))); \
    }
#endif /* SYS_IN_LONG */

#ifndef SYS_OUT_SHORT
#define SYS_OUT_SHORT(pDrvCtrl,addr,value) \
    { \
    *((USHORT *)(addr)) = PCI_WORD_SWAP((value)); \
    }
#endif /* SYS_OUT_SHORT*/

#ifndef SYS_IN_SHORT
#define SYS_IN_SHORT(pDrvCtrl,addr,data) \
    { \
    ((data) = PCI_WORD_SWAP(*((USHORT *)(addr)))); \
    }      
#endif /* SYS_IN_SHORT*/

#ifndef SYS_OUT_BYTE
#define SYS_OUT_BYTE(pDrvCtrl,addr,value) \
    { \
    *((UCHAR *)(addr)) = (value); \
    }
#endif /* SYS_OUT_BYTE */

#ifndef SYS_IN_BYTE
#define SYS_IN_BYTE(pDrvCtrl,addr,data) \
    { \
    ((data) = *((UCHAR *)(addr))); \
    }
#endif /* SYS_IN_BYTE */

#ifndef SYS_DELAY    
#define SYS_DELAY(x)           \
    {			       \
    int loop;                  \
    loop = (x);                \
    loop = ((loop * 3) >> 1);  \
    while (loop--)	       \
        sysDelay();            \
    }
#endif /* SYS_DELAY */

/* A shortcut for getting the hardware address from the MIB II stuff. */

#ifdef INCLUDE_RFC_1213

/* Old RFC 1213 mib2 interface */

#define END_HADDR(pEnd) \
                ((pEnd)->mib2Tbl.ifPhysAddress.phyAddress)

#define END_HADDR_LEN(pEnd) \
                ((pEnd)->mib2Tbl.ifPhysAddress.addrLength)
#else

/* New RFC 2233 mib2 interface */

#define END_HADDR(pEnd)                                                 \
                ((pEnd)->pMib2Tbl->m2Data.mibIfTbl.ifPhysAddress.phyAddress)

#define END_HADDR_LEN(pEnd)                                             \
                ((pEnd)->pMib2Tbl->m2Data.mibIfTbl.ifPhysAddress.addrLength)

#endif /* INCLUDE_RFC_1213 */

#define END_FLAGS_ISSET(pEnd, setBits) \
            ((pEnd)->flags & (setBits))

#define VOID_TO_DRVCTRL(pVoid,pDrvCtrl) ((pDrvCtrl)=(EL3C90X_DEVICE *)(pVoid))

/* externs */
IMPORT int 	endMultiLstCnt (END_OBJ *);

IMPORT void 	sysDelay(); 	/* x86 bSP implements around 720 ns delay */

/* DEBUG MACROS */

#ifdef DRV_DEBUG	/* if debugging driver */
int      el3c90xDebug = DRV_DEBUG_LOAD| DRV_DEBUG_INT | DRV_DEBUG_TX;
NET_POOL * pElXlPool;

#define DRV_LOG(FLG, X0, X1, X2, X3, X4, X5, X6) \
        if (el3c90xDebug & FLG) \
            logMsg((char *)X0, (int)X1, (int)X2, (int)X3, (int)X4, \
		    (int)X5, (int)X6);

#define ENDLOGMSG(x) \
	if (el3c90xDebug) \
	    { \
	    logMsg x; \
	    }

#define DRV_PRINT(FLG,X) \
        if (el3c90xDebug & FLG) printf X;

#else /*DRV_DEBUG*/

#define DRV_LOG(DBG_SW, X0, X1, X2, X3, X4, X5, X6)
#define DRV_PRINT(DBG_SW,X)
#define ENDLOGMSG(x)
#endif /*DRV_DEBUG*/

FUNCPTR	el3c90xIntConnectRtn = intConnect;

/*
 * Various supported PHY vendors/types and their names. Note that
 * this driver will work with pretty much any MII-compliant PHY,
 * so failure to positively identify the chip is not a fatal error.
 */

LOCAL EL_DEV_TYPE el3c90xPhyTbl[ ] =
    {
    { TI_PHY_VENDORID, TI_PHY_10BT, "<TI ThunderLAN 10BT (internal)>" },
    { TI_PHY_VENDORID, TI_PHY_100VGPMI, "<TI TNETE211 100VG Any-LAN>" },
    { NS_PHY_VENDORID, NS_PHY_83840A, "<National Semiconductor DP83840A>"},
    { LEVEL1_PHY_VENDORID, LEVEL1_PHY_LXT970, "<Level 1 LXT970>" }, 
    { INTEL_PHY_VENDORID, INTEL_PHY_82555, "<Intel 82555>" },
    { SEEQ_PHY_VENDORID, SEEQ_PHY_80220, "<SEEQ 80220>" },
    { 0, 0, "<MII-compliant physical interface>" }
    };

LOCAL STATUS el3c90xStart (void * pEnd);
LOCAL STATUS el3c90xStop  (void * pEnd);
LOCAL STATUS el3c90xUnload (void * pEnd);
LOCAL int    el3c90xIoctl (void * pEnd, int cmd, caddr_t data);
LOCAL STATUS el3c90xSend (void * pEnd, M_BLK_ID pMblk);
LOCAL STATUS el3c90xMCastAdd (void * pEnd, char * pAddress);
LOCAL STATUS el3c90xMCastDel (void * pEnd, char * pAddress);
LOCAL STATUS el3c90xMCastGet (void * pEnd, MULTI_TABLE * pTable);
LOCAL STATUS el3c90xPollSend (void * pEnd, M_BLK_ID pMblk);
LOCAL STATUS el3c90xPollRcv  (void * pEnd, M_BLK_ID pMblk);

LOCAL void   el3c90xInit (EL3C90X_DEVICE *);
LOCAL STATUS el3c90xMemInit (EL3C90X_DEVICE * pDrvCtrl);
LOCAL void   el3c90xDevStop (EL3C90X_DEVICE *);
LOCAL void   el3c90xReset (EL3C90X_DEVICE *);
LOCAL void   el3c90xIntEnable (EL3C90X_DEVICE * pDrvCtrl);
LOCAL void   el3c90xIntDisable (EL3C90X_DEVICE * pDrvCtrl);
LOCAL STATUS el3c90xPollStart(EL3C90X_DEVICE * pDrvCtrl);
LOCAL STATUS el3c90xPollStop (EL3C90X_DEVICE * pDrvCtrl);
LOCAL int    el3c90xUpdInit (EL3C90X_DEVICE *);
LOCAL int    el3c90xDndInit (EL3C90X_DEVICE *);

LOCAL UINT8  el3c90xHashGet (char *);
LOCAL void   el3c90xFilterSet (EL3C90X_DEVICE *);
LOCAL void   el3c90xHashFilterSet (EL3C90X_DEVICE *);
LOCAL void   el3c90xMcastConfig (EL3C90X_DEVICE * pDrvCtrl);

LOCAL int    el3c90xUpdFill (EL3C90X_DEVICE *, EL_SIMPLE_DESC_CHAIN *);
LOCAL STATUS el3c90xDndMblkPack (EL3C90X_DEVICE *, EL_DESC_CHAIN *, M_BLK_ID);
LOCAL int    el3c90xDndSet	(EL3C90X_DEVICE *, EL_DESC_CHAIN *, M_BLK * );
LOCAL STATUS el3c90xDndFree (EL3C90X_DEVICE *, EL_DESC_CHAIN *);
LOCAL STATUS el3c90xDndEnqueue (EL3C90X_DEVICE *, EL_DESC_CHAIN *);
LOCAL EL_DESC_CHAIN * el3c90xDndGet (EL3C90X_DEVICE * pDrvCtrl);
LOCAL EL_DESC_CHAIN * el3c90xTxDequeue (EL3C90X_DEVICE * pDrvCtrl);
LOCAL M_BLK_ID el3c90xNextPktFetch (EL3C90X_DEVICE *, EL_SIMPLE_DESC_CHAIN *);
LOCAL EL_SIMPLE_DESC_CHAIN * el3c90xNextUpdFetch (EL3C90X_DEVICE * pDrvCtrl);

LOCAL STATUS el3c90xRxUnStall (EL3C90X_DEVICE * pDrvCtrl);
LOCAL void   el3c90xRxKick (EL3C90X_DEVICE * pDrvCtrl);
LOCAL void   el3c90xIntrRx (EL3C90X_DEVICE * pDrvCtrl);
LOCAL void   el3c90xIntrTx (EL3C90X_DEVICE * pDrvCtrl);
LOCAL void   el3c90xIntrErr (EL3C90X_DEVICE * pDrvCtrl, UINT8 txstat);
LOCAL void   el3c90xStatUpdate (EL3C90X_DEVICE *);
LOCAL void   el3c90xInt (EL3C90X_DEVICE * pDrvCtrl);

LOCAL void   el3c90xMiiSync (EL3C90X_DEVICE *);
LOCAL void   el3c90xMiiSend (EL3C90X_DEVICE *, UINT32, int);
LOCAL int    el3c90xMiiRegRead (EL3C90X_DEVICE *, EL_MII_FRAME *);
LOCAL int    el3c90xMiiRegWrite (EL3C90X_DEVICE *, EL_MII_FRAME *);
LOCAL UINT16 el3c90xPhyRegRead (EL3C90X_DEVICE *, int);
LOCAL void   el3c90xPhyRegWrite (EL3C90X_DEVICE *, int, int);
LOCAL void   el3c90xAutoNegTx (EL3C90X_DEVICE *);
LOCAL void   el3c90xMiiAutoNeg (EL3C90X_DEVICE *);
LOCAL int    el3c90xMiiModeGet (EL3C90X_DEVICE *);
LOCAL void   el3c90xMediaSet (EL3C90X_DEVICE *, int);
LOCAL void   el3c90xMediaCheck (EL3C90X_DEVICE *);
LOCAL int    el3c90xMediaConfig (EL3C90X_DEVICE * pDrvCtrl);

LOCAL int    el3c90xEprmWait (EL3C90X_DEVICE *);
LOCAL int    el3c90xEprmRead (EL3C90X_DEVICE *, caddr_t, int, int, int);

LOCAL void   el3c90xWait (EL3C90X_DEVICE *);
LOCAL void   el3c90xIntrSet (EL3C90X_DEVICE * pDrvCtrl, UINT16 flag);
LOCAL void   el3c90xIntrClr (EL3C90X_DEVICE * pDrvCtrl, UINT16 flag);
LOCAL UCHAR  el3c90xCsrReadByte (EL3C90X_DEVICE *, USHORT, int);
LOCAL USHORT el3c90xCsrReadWord (EL3C90X_DEVICE *, USHORT, int);
LOCAL ULONG  el3c90xCsrReadLong (EL3C90X_DEVICE *, USHORT, int);
LOCAL void   el3c90xCsrWriteByte (EL3C90X_DEVICE *, USHORT, UCHAR, int);
LOCAL void   el3c90xCsrWriteWord (EL3C90X_DEVICE *, USHORT, USHORT, int);
LOCAL void   el3c90xCsrWriteLong (EL3C90X_DEVICE *, USHORT, ULONG, int);

/*
 * Declare our function table.  This is static across all driver
 * instances.
 */

LOCAL NET_FUNCS el3c90xFuncTable =
    {
    (FUNCPTR) el3c90xStart,     /* Function to start the device. */
    (FUNCPTR) el3c90xStop,	/* Function to stop the device. */
    (FUNCPTR) el3c90xUnload,	/* Unloading function for the driver. */
    (FUNCPTR) el3c90xIoctl,	/* Ioctl function for the driver. */
    (FUNCPTR) el3c90xSend,	/* Send function for the driver. */
    (FUNCPTR) el3c90xMCastAdd,	/* Multicast address add function for the */
				/* driver. */
    (FUNCPTR) el3c90xMCastDel,	/* Multicast address delete function for */
				/* the driver. */
    (FUNCPTR) el3c90xMCastGet,	/* Multicast table retrieve function for */
				/* the driver. */
    (FUNCPTR) el3c90xPollSend,	/* Polling send function for the driver. */
    (FUNCPTR) el3c90xPollRcv,	/* Polling receive function for the driver. */
    endEtherAddressForm,  /* Put address info into a packet.  */
    (FUNCPTR) endEtherPacketDataGet,/* Get a pointer to packet data. */
    (FUNCPTR) endEtherPacketAddrGet /* Get packet addresses. */
    };

/******************************************************************************
*
* el3c90xEndLoad - initialize the driver and device
*
* This routine initializes the driver and the device to the operational state.
* All of the device-specific parameters are passed in <initString>, which
* expects a string of the following format:
*
* <unit>:<devMemAddr>:<devIoAddr>:<pciMemBase:<vecnum>:<intLvl>:<memAdrs>
* :<memSize>:<memWidth>:<flags>:<buffMultiplier>
*
* This routine can be called in two modes. If it is called with an empty but
* allocated string, it places the name of this device (that is, "elPci") into 
* the <initString> and returns 0.
*
* If the string is allocated and not empty, the routine attempts to load
* the driver using the values specified in the string.
*
* RETURNS: An END object pointer, or NULL on error, or 0 and the name of the
* device if the <initString> was NULL.
*/

END_OBJ * el3c90xEndLoad
    (
    char * initString		/* String to be parsed by the driver. */
    )
    {
    EL3C90X_DEVICE *	pDrvCtrl = NULL;
    int			speed;

    DRV_LOG (DRV_DEBUG_LOAD, "Loading elPci...\n", 1, 2, 3, 4, 5, 6);

    if (initString == NULL)
        return (NULL);
    
    if (initString [0] == 0)
        {
        bcopy ((char *)EL3C90X_DEV_NAME, initString,
               EL3C90X_DEV_NAME_LEN);
        return (0);
        }

    if ((pDrvCtrl = (EL3C90X_DEVICE *)calloc (sizeof (EL3C90X_DEVICE), 1))
        == NULL)
        return (NULL);
        
    /* parse the init string, filling in the device structure */

    if (el3c90xInitParse (pDrvCtrl, initString) == ERROR)
        goto endLoadFail;

    /* initialize the END and MIB2 parts of the structure */
    
    if (END_OBJ_INIT (&pDrvCtrl->endObj, (DEV_OBJ *)pDrvCtrl,
                      EL3C90X_DEV_NAME,
                      pDrvCtrl->unit, &el3c90xFuncTable,
                      "3COM 3c90X Fast Etherlink Endhanced Network Driver.")
        == ERROR)
        {
        goto endLoadFail;
        }

    /* Reset the adapter. */

    el3c90xReset(pDrvCtrl);

    /* get station address from the EEPROM */

    if (el3c90xEprmRead(pDrvCtrl, &pDrvCtrl->enetAddr[0], EL_EE_OEM_ADR0, 3, 1)
        != OK)
        {
        DRV_LOG (DRV_DEBUG_LOAD, "elPci%d: failed to read station address\n",
                 pDrvCtrl->unit, 2, 3, 4, 5, 6);
        goto endLoadFail;
	}

    /* Perform memory allocation */

    if (el3c90xMemInit (pDrvCtrl) == ERROR)
        goto endLoadFail;

    DRV_LOG (DRV_DEBUG_LOAD, "Malloc done ...\n", 1, 2, 3, 4, 5, 6);
        
    /*
     * Figure out the card type. 3c905B adapters have the
     * 'supportsNoTxLength' bit set in the capabilities
     * word in the EEPROM.
     */

    el3c90xEprmRead(pDrvCtrl, (caddr_t)&pDrvCtrl->devCaps, EL_EE_CAPS, 1, 0);

    if (pDrvCtrl->devCaps & EL_CAPS_NO_TXLENGTH)
        pDrvCtrl->devType = EL_TYPE_905B;
    else
        pDrvCtrl->devType = EL_TYPE_90X;

    speed =  el3c90xMediaConfig (pDrvCtrl);
   
#ifdef INCLUDE_RFC_1213

    /* Old RFC 1213 mib2 interface */

    if (END_MIB_INIT (&pDrvCtrl->endObj, M2_ifType_ethernet_csmacd,
                      &pDrvCtrl->enetAddr[0], 6, ETHERMTU, speed)
        == ERROR)
        goto endLoadFail;

    /* set the flags to indicate readiness */

    END_OBJ_READY (&pDrvCtrl->endObj, IFF_NOTRAILERS | IFF_BROADCAST |
                   IFF_MULTICAST  | IFF_SIMPLEX);

#else
    /* New RFC 2233 mib2 interface */
 
    /* Initialize MIB-II entries (for RFC 2233 ifXTable) */

    pDrvCtrl->endObj.pMib2Tbl = m2IfAlloc(M2_ifType_ethernet_csmacd,
                                          (UINT8*) &pDrvCtrl->enetAddr[0], 6,
                                          ETHERMTU, speed,
                                          EL3C90X_DEV_NAME, pDrvCtrl->unit);
    if (pDrvCtrl->endObj.pMib2Tbl == NULL)
        {
        printf ("%s%d - MIB-II initializations failed\n",
                EL3C90X_DEV_NAME, pDrvCtrl->unit);
        goto endLoadFail;
        }

    /* 
     * Set the RFC2233 flag bit in the END object flags field and
     * install the counter update routines.
     */

    pDrvCtrl->endObj.flags |= END_MIB_2233;

    m2IfPktCountRtnInstall(pDrvCtrl->endObj.pMib2Tbl, m2If8023PacketCount);

    /*
     * Make a copy of the data in mib2Tbl struct as well. We do this
     * mainly for backward compatibility issues. There might be some
     * code that might be referencing the END pointer and might
     * possibly do lookups on the mib2Tbl, which will cause all sorts
     * of problems.
     */

    bcopy ((char *)&pDrvCtrl->endObj.pMib2Tbl->m2Data.mibIfTbl,
                   (char *)&pDrvCtrl->endObj.mib2Tbl, sizeof (M2_INTERFACETBL));

    /* set the flags to indicate readiness */

    END_OBJ_READY (&pDrvCtrl->endObj, IFF_NOTRAILERS | IFF_BROADCAST |
                   IFF_MULTICAST  | IFF_SIMPLEX | END_MIB_2233);

#endif /* INCLUDE_RFC_1213 */

    DRV_LOG (DRV_DEBUG_LOAD, "Done loading elPci...\n", 1, 2, 3, 4, 5, 6);

    return (&pDrvCtrl->endObj);
    
    endLoadFail:
        {
        if (pDrvCtrl != NULL)
            free ((char *)pDrvCtrl);
        }
    
    return ((END_OBJ *)NULL);
    }

/*******************************************************************************
*
* el3c90xInitParse - parse the initialization string
*
* Parse the input string. This routine is called from el3c90xEndLoad() which
* intializes some values in the driver control structure with the values
* passed in the intialization string.
*
* The initialization string format is:
* <unit>:<devMemAddr>:<devIoAddr>:<pciMemBase:<vecNum>:<intLvl>:<memAdrs>
* :<memSize>:<memWidth>:<flags>:<buffMultiplier>
*
* .IP <unit>
* Device unit number, a small integer.
* .IP <devMemAddr>
* Device register base memory address
* .IP <devIoAddr>
* Device register base IO address
* .IP <pciMemBase>
* Base address of PCI memory space
* .IP <vecNum>
* Interrupt vector number.
* .IP <intLvl>
* Interrupt level.
* .IP <memAdrs>
* Memory pool address or NONE.
* .IP <memSize>
* Memory pool size or zero.
* .IP <memWidth>
* Memory system size, 1, 2, or 4 bytes (optional).
* .IP <flags>
* Device specific flags, for future use.
* .IP <buffMultiplier>
* Buffer Multiplier or NONE. If NONE is specified, it defaults to 2
*
* RETURNS: OK, or ERROR if any arguments are invalid.
*/

STATUS el3c90xInitParse
    (
    EL3C90X_DEVICE *	pDrvCtrl,	/* pointer to the control structure */
    char * 		initString	/* initialization string */
    )
    {
    char*       tok;
    char*       pHolder = NULL;
    UINT32      devMemAddr;
    UINT32      devIoAddr;

    DRV_LOG (DRV_DEBUG_LOAD, "Parse starting ...\n", 1, 2, 3, 4, 5, 6);

    /* Parse the initString */

    /* Unit number. */

    tok = strtok_r (initString, ":", &pHolder);
    if (tok == NULL)
        return ERROR;

    pDrvCtrl->unit = atoi (tok);

    DRV_LOG (DRV_DEBUG_LOAD, "Unit : %d ...\n", pDrvCtrl->unit, 2, 3, 4, 5, 6);

    /* devAdrs address. */

    tok = strtok_r (NULL, ":", &pHolder);
    if (tok == NULL)
        return ERROR;
    devMemAddr = (UINT32) strtoul (tok, NULL, 16);

    DRV_LOG (DRV_DEBUG_LOAD, "devMemAddr : 0x%X ...\n", devMemAddr,
             2, 3, 4, 5, 6);

    /* devIoAddrs address */

    tok = strtok_r (NULL, ":", &pHolder);
    if (tok == NULL)
        return ERROR;
    devIoAddr = (UINT32) strtoul (tok, NULL, 16);

    DRV_LOG (DRV_DEBUG_LOAD, "devIoAddr : 0x%X ...\n", devIoAddr,
             2, 3, 4, 5, 6);

    /* always use memory mapped IO if provided, else use io map */
    
    if ((devMemAddr == NONE) && (devIoAddr == NONE))
        {
        DRV_LOG (DRV_DEBUG_LOAD, "No memory or IO base specified ...\n",
                 1, 2, 3, 4, 5, 6);
        return (ERROR);
        }

    else if (devMemAddr != NONE)
        {
        pDrvCtrl->devAdrs = devMemAddr;
        pDrvCtrl->flags   |= EL_MODE_MEM_IO_MAP;
        }
    else
        {
        pDrvCtrl->devAdrs = devIoAddr;
        }
        
    /* PCI memory base address as seen from the CPU */
    
    tok = strtok_r (NULL, ":", &pHolder);
    if (tok == NULL)
        return ERROR;
    pDrvCtrl->pciMemBase = strtoul (tok, NULL, 16);

    DRV_LOG (DRV_DEBUG_LOAD, "Pci : 0x%X ...\n", pDrvCtrl->pciMemBase,
             2, 3, 4, 5, 6);

    /* Interrupt vector. */

    tok = strtok_r (NULL, ":", &pHolder);
    if (tok == NULL)
        return ERROR;
    pDrvCtrl->ivec = atoi (tok);

    DRV_LOG (DRV_DEBUG_LOAD, "ivec : 0x%X ...\n", pDrvCtrl->ivec,
             2, 3, 4, 5, 6);
    /* Interrupt level. */

    tok = strtok_r (NULL, ":", &pHolder);
    if (tok == NULL)
        return ERROR;
    pDrvCtrl->intLevel = atoi (tok);
    DRV_LOG (DRV_DEBUG_LOAD, "ilevel : 0x%X ...\n", pDrvCtrl->intLevel,
             2, 3, 4, 5, 6);

    /* Caller supplied memory address. */

    tok = strtok_r (NULL, ":", &pHolder);
    if (tok == NULL)
        return ERROR;
    pDrvCtrl->memAdrs = (char *)strtoul (tok, NULL, 16);
    DRV_LOG (DRV_DEBUG_LOAD, "memAdrs : 0x%X ...\n", (int)pDrvCtrl->memAdrs,
             2, 3, 4, 5, 6);

    /* Caller supplied memory size. */

    tok = strtok_r (NULL, ":", &pHolder);
    if (tok == NULL)
        return ERROR;
    pDrvCtrl->memSize = strtoul (tok, NULL, 16);
    DRV_LOG (DRV_DEBUG_LOAD, "memSize : 0x%X ...\n", pDrvCtrl->memSize,
             2, 3, 4, 5, 6);

    /* Caller supplied memory width. */

    tok = strtok_r (NULL, ":", &pHolder);
    if (tok == NULL)
        return ERROR;
    pDrvCtrl->memWidth = atoi (tok);
    DRV_LOG (DRV_DEBUG_LOAD, "memWidth : 0x%X ...\n", pDrvCtrl->memWidth,
             2, 3, 4, 5, 6);

    /* caller supplied flags */

    tok = strtok_r (NULL, ":", &pHolder);
    if (tok == NULL)
        return ERROR;

    pDrvCtrl->flags |= strtoul (tok, NULL, 16);
    DRV_LOG (DRV_DEBUG_LOAD, "flags : 0x%X ...\n", pDrvCtrl->flags,
             2, 3, 4, 5, 6);

    /* buffer multiplier */

    tok = strtok_r (NULL, ":", &pHolder);
    if (tok == NULL)
        return ERROR;

    pDrvCtrl->bufMtplr |= strtoul (tok, NULL, 16);
    DRV_LOG (DRV_DEBUG_LOAD, "bufMultiplier : 0x%X ...\n", pDrvCtrl->bufMtplr,
             2, 3, 4, 5, 6);

    return OK;
    }

/*******************************************************************************
*
* el3c90xStart - start the device
*
* This function calls BSP functions to connect interrupts and start the
* device running in interrupt mode.
*
* RETURNS: OK or ERROR
*
*/

LOCAL STATUS el3c90xStart
    (
    void * pEnd	/* device to be started */
    )
    {
    STATUS 		result;
    EL3C90X_DEVICE * 	pDrvCtrl;
    int			flags;

    VOID_TO_DRVCTRL (pEnd, pDrvCtrl);
    
    pDrvCtrl->txBlocked  = FALSE;

    /* perform all initialization memory, ring buffer etc */
    
    el3c90xInit(pDrvCtrl);
    
    flags = END_FLAGS_GET (&pDrvCtrl->endObj);
    flags |= (IFF_UP | IFF_RUNNING);

    END_FLAGS_SET (&pDrvCtrl->endObj, flags);
    
    SYS_INT_CONNECT (pDrvCtrl, el3c90xInt, (int)pDrvCtrl, &result);
    if (result == ERROR)
	return ERROR;

    ENDLOGMSG (("Interrupt connected.\n", 1, 2, 3, 4, 5, 6));

    SYS_INT_ENABLE (pDrvCtrl);

    ENDLOGMSG (("interrupt enabled.\n", 1, 2, 3, 4, 5, 6));

    return (OK);
    }

/*******************************************************************************
*
* el3c90xStop - stop the device
*
* This function calls BSP functions to disconnect interrupts and stop
* the device from operating in interrupt mode.
*
* RETURNS: OK or ERROR.
*/

LOCAL STATUS el3c90xStop
    (
    void * pEnd	/* device to be stopped */
    )
    {
    STATUS 		result = OK;
    EL3C90X_DEVICE * 	pDrvCtrl;

    VOID_TO_DRVCTRL (pEnd, pDrvCtrl);

    END_FLAGS_CLR (&pDrvCtrl->endObj, IFF_UP | IFF_RUNNING);

    el3c90xDevStop (pDrvCtrl);

    SYS_INT_DISCONNECT (pDrvCtrl, el3c90xInt, (int)pDrvCtrl, &result);

    if (result == ERROR)
	{
	ENDLOGMSG (("Could not diconnect interrupt!\n", 1, 2, 3, 4, 5, 6));
	}

    SYS_INT_DISABLE (pDrvCtrl);

    return (result);
    }

/******************************************************************************
*
* el3c90xUnload - unload a driver from the system
*
* This function first brings down the device, and then frees any
* stuff that was allocated by the driver in the load function.
*
* RETURNS: OK or ERROR.
*/

LOCAL STATUS el3c90xUnload
    (
    void * pEnd		/* device to be unloaded */
    )
    {
    EL3C90X_DEVICE * pDrvCtrl;

    VOID_TO_DRVCTRL (pEnd, pDrvCtrl);
    
    END_OBJECT_UNLOAD (&pDrvCtrl->endObj);

    /* Free the shared DMA memory. */

    if (pDrvCtrl->flags & EL_MEM_ALLOC_FLAG)
        {
        /* free the memory allocated for descriptors */
        
        if (pDrvCtrl->pDescMem != NULL)
            cacheDmaFree (pDrvCtrl->pDescMem);

        /* free the memory allocated for clusters */

        if (pDrvCtrl->clDesc.memArea != NULL)
            cacheDmaFree (pDrvCtrl->clDesc.memArea);

        /* Free the memory allocated for mBlks and clBlks */

        if (pDrvCtrl->mClCfg.memArea != NULL)
            free (pDrvCtrl->mClCfg.memArea);
        }

    if (pDrvCtrl->endObj.pNetPool != NULL)
        free (pDrvCtrl->endObj.pNetPool);
    
    return (OK);
    }

/*******************************************************************************
*
* el3c90xIoctl - the driver I/O control routine
*
* Process an ioctl request.
*
* RETURNS: A command specific response, OK or ERROR or EINVAL.
*/

LOCAL int el3c90xIoctl
    (
    void * 	pEnd,           /* device  ptr*/
    int 	cmd,		/* ioctl command code */
    caddr_t 	data		/* command argument */
    )
    {
    int 		error = 0;
    long 		value;
    EL3C90X_DEVICE * 	pDrvCtrl;

    VOID_TO_DRVCTRL (pEnd, pDrvCtrl);

    switch (cmd)
        {
        case EIOCSADDR:
	    if (data == NULL)
		return (EINVAL);
            bcopy ((char *)data, (char *)END_HADDR(&pDrvCtrl->endObj),
		   END_HADDR_LEN(&pDrvCtrl->endObj));
            break;

        case EIOCGADDR:
	    if (data == NULL)
		return (EINVAL);
            bcopy ((char *)END_HADDR(&pDrvCtrl->endObj), (char *)data,
		    END_HADDR_LEN(&pDrvCtrl->endObj));
            break;

        case EIOCSFLAGS:
	    value = (long)data;
	    if (value < 0)
		{
		value = -value;
		value--;
		END_FLAGS_CLR (&pDrvCtrl->endObj, value);
		}
	    else
		{
		END_FLAGS_SET (&pDrvCtrl->endObj, value);
		}

            if (END_FLAGS_GET(&pDrvCtrl->endObj) & IFF_UP)
                {
                el3c90xInit (pDrvCtrl);
		}
            else
                {
                if (END_FLAGS_GET(&pDrvCtrl->endObj) & IFF_RUNNING)
                    el3c90xDevStop (pDrvCtrl);
		}
            error = 0;
            break;

        case EIOCGFLAGS:
	    *(int *)data = END_FLAGS_GET(&pDrvCtrl->endObj);
            break;

	case EIOCPOLLSTART:
	    error = el3c90xPollStart(pDrvCtrl);
	    break;
 
	case EIOCPOLLSTOP:
	    error = el3c90xPollStop(pDrvCtrl);
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
            *(int *)data = EL_MIN_FBUF;
            break;

        case EIOCGMWIDTH:
            if (data == NULL)
                return (EINVAL);
            break;

        case EIOCGHDRLEN:
            if (data == NULL)
                return (EINVAL);
            *(int *)data = EH_SIZE;
            break;

#ifndef INCLUDE_RFC_1213

        /* New RFC 2233 mib2 interface */

        case EIOCGMIB2233:
            if ((data == NULL) || (pDrvCtrl->endObj.pMib2Tbl == NULL))
                error = EINVAL;
            else
                *((M2_ID **)data) = pDrvCtrl->endObj.pMib2Tbl;
            break;

#endif /* INCLUDE_RFC_1213 */

        default:
            error = EINVAL;
        }

    return (error);
    }

/*******************************************************************************
*
* el3c90xSend - the driver send routine
*
* This routine takes a M_BLK_ID sends off the data in the M_BLK_ID.
* The buffer must already have the addressing information properly installed
* in it.  This is done by a higher layer.
*
* RETURNS: OK or END_ERR_BLOCK or ERROR.
*/

LOCAL STATUS el3c90xSend
    (
    void  * 		pEnd,	/* device ptr */
    M_BLK_ID 		pMblk	/* data to send */
    )
    {
    EL3C90X_DEVICE * 	pDrvCtrl;
    EL_DESC_CHAIN *	pTxChain = NULL;

    VOID_TO_DRVCTRL (pEnd, pDrvCtrl);

    /* return if in polled mode */
    
    if (pDrvCtrl->flags & EL_POLLING)
        { 
        netMblkClChainFree (pMblk); /* free the given mBlk chain */
        errno = EINVAL;
        return (ERROR);
        }

    DRV_LOG (DRV_DEBUG_TX, "Send\n", 1, 2, 3, 4, 5, 6);

    END_TX_SEM_TAKE (&pDrvCtrl->endObj, WAIT_FOREVER);

    /* get a free download descriptor */
    
    if ((pTxChain = el3c90xDndGet (pDrvCtrl)) == NULL)
        {
        DRV_LOG (DRV_DEBUG_TX, "Out of TMDs!\n", 1, 2, 3, 4, 5, 6);
        pDrvCtrl->txBlocked = TRUE;
        END_TX_SEM_GIVE (&pDrvCtrl->endObj);
        return (END_ERR_BLOCK);
        }

    /* Pack the data into the descriptor. */

    if (el3c90xDndSet(pDrvCtrl, pTxChain, pMblk) == ERROR)
        {
        netMblkClChainFree (pMblk); /* free the given mBlk chain */
        el3c90xDndFree (pDrvCtrl, pTxChain);
        END_TX_SEM_GIVE (&pDrvCtrl->endObj);
        return (ERROR);
        }
#ifndef INCLUDE_RFC_1213

    /* New RFC 2233 mib2 interface */

    /* RFC 2233 mib2 counter update for outgoing packet */

    if (pDrvCtrl->endObj.pMib2Tbl != NULL)
        {
        pDrvCtrl->endObj.pMib2Tbl->m2PktCountRtn(pDrvCtrl->endObj.pMib2Tbl,
                                                 M2_PACKET_OUT,
                                                 pMblk->mBlkHdr.mData,
                                                 pMblk->mBlkHdr.mLen);
        }
#endif /* INCLUDE_RFC_1213 */

    /* set the interrupt bit for the downloaded packet */

    pTxChain->pDesc->status |= PCI_SWAP (EL_TXSTAT_DL_INTR);

    el3c90xDndEnqueue (pDrvCtrl, pTxChain);

    END_TX_SEM_GIVE (&pDrvCtrl->endObj);

    /* kick the reciever if blocked, parallel tasking */
    el3c90xRxKick (pDrvCtrl);

    return (OK);
    }

/*****************************************************************************
*
* el3c90xMCastAdd - add a multicast address for the device
*
* This routine adds a multicast address to whatever the driver
* is already listening for.  It then resets the address filter.
*
* RETURNS: OK or ERROR.
*/

LOCAL STATUS el3c90xMCastAdd
    (
    void  * 	pEnd,	        /* device pointer */
    char * 	pAddress	/* new address to add */
    )
    {
    int 		error;
    EL3C90X_DEVICE * 	pDrvCtrl;

    VOID_TO_DRVCTRL (pEnd, pDrvCtrl);

    if ((error = etherMultiAdd (&pDrvCtrl->endObj.multiList,
		pAddress)) == ENETRESET)
	el3c90xMcastConfig (pDrvCtrl);

    return (OK);
    }

/*****************************************************************************
*
* el3c90xMCastDel - delete a multicast address for the device
*
* This routine removes a multicast address from whatever the driver
* is listening for.  It then resets the address filter.
*
* RETURNS: OK or ERROR.
*/

LOCAL STATUS el3c90xMCastDel
    (
    void  * 	pEnd,	        /* device pointer */
    char * 	pAddress	/* address to be deleted */
    )
    {
    int 		error;
    EL3C90X_DEVICE * 	pDrvCtrl;

    VOID_TO_DRVCTRL (pEnd, pDrvCtrl);

    if ((error = etherMultiDel (&pDrvCtrl->endObj.multiList,
	     (char *)pAddress)) == ENETRESET)
	el3c90xMcastConfig (pDrvCtrl);

    return (OK);
    }

/*****************************************************************************
*
* el3c90xMCastGet - get the multicast address list for the device
*
* This routine gets the multicast list of whatever the driver
* is already listening for.
*
* RETURNS: OK or ERROR.
*/

LOCAL STATUS el3c90xMCastGet
    (
    void  * 		pEnd,	        /* device pointer */
    MULTI_TABLE * 	pTable		/* address table to be filled in */
    )
    {
    EL3C90X_DEVICE * pDrvCtrl;

    VOID_TO_DRVCTRL (pEnd, pDrvCtrl);

    return (etherMultiGet (&pDrvCtrl->endObj.multiList, pTable));
    }

/*******************************************************************************
*
* el3c90xPollSend - routine to send a packet in polled mode.
*
* This routine is called by a user to try and send a packet on the
* device.
*
* RETURNS: OK upon success.  EAGAIN if device is busy.
*/

LOCAL STATUS el3c90xPollSend
    (
    void  * 		pEnd,	        /* device pointer */
    M_BLK_ID 		pMblk		/* packet to send */
    )
    {
    EL3C90X_DEVICE * 	pDrvCtrl;
    EL_DESC_CHAIN *	pTxChain = NULL;

    VOID_TO_DRVCTRL (pEnd, pDrvCtrl);

    DRV_LOG (DRV_DEBUG_POLL_TX, "PTX b\n", 1, 2, 3, 4, 5, 6);

    /* check if download engine is still busy */

    if (el3c90xCsrReadLong (pDrvCtrl, EL_DOWNLIST_PTR, NONE))
        {
        DRV_LOG (DRV_DEBUG_POLL_TX, "transmitter busy\n", 1, 2, 3, 4, 5, 6);
        
        return (EAGAIN);
        }

    /* check if a download descriptor is available */

    if ((pTxChain =  el3c90xDndGet (pDrvCtrl)) == NULL)
        return (EAGAIN);

    /* Pack the mBlk into the download descriptor. */

    if (el3c90xDndMblkPack (pDrvCtrl, pTxChain, pMblk) == ERROR)
        {
        el3c90xDndFree (pDrvCtrl, pTxChain);
        return (EAGAIN);
        }
#ifndef INCLUDE_RFC_1213

    /* New RFC 2233 mib2 interface */

    /* RFC 2233 mib2 counter update for outgoing packet */

    if (pDrvCtrl->endObj.pMib2Tbl != NULL)
        {
        pDrvCtrl->endObj.pMib2Tbl->m2PktCountRtn(pDrvCtrl->endObj.pMib2Tbl,
                                                 M2_PACKET_OUT,
                                                 pMblk->mBlkHdr.mData,
                                                 pMblk->mBlkHdr.mLen);
        }
#endif /* INCLUDE_RFC_1213 */

    el3c90xCsrWriteLong (pDrvCtrl, EL_DOWNLIST_PTR,
                         (UINT32)EL3C90X_CACHE_VIRT_TO_PHYS (pTxChain->pDesc),
                         NONE);

    /* wait until download has completed */
    
    while (el3c90xCsrReadLong (pDrvCtrl, EL_DOWNLIST_PTR, NONE))
        ;

    el3c90xDndFree (pDrvCtrl, pTxChain);

    DRV_LOG (DRV_DEBUG_POLL_TX, "PTX e\n", 1, 2, 3, 4, 5, 6);

    return (OK);
    }

/*******************************************************************************
*
* el3c90xPollRcv - routine to receive a packet in polled mode.
*
* This routine is called by a user to try and get a packet from the
* device.
*
* RETURNS: EGAIN or OK
*/

LOCAL STATUS el3c90xPollRcv
    (
    void  * 		pEnd,		/* device pointer */
    M_BLK_ID 		pMblk		/* pointer to the mBlk chain */
    )
    {
    EL_SIMPLE_DESC_CHAIN * 	pRxUpd;
    EL3C90X_DEVICE * 		pDrvCtrl;
    M_BLK_ID 			pMblkTemp;	/* pointer to the mBlk */
    int				len;

    DRV_LOG (DRV_DEBUG_POLL_RX, "PRX b\n", 1, 2, 3, 4, 5, 6);

    VOID_TO_DRVCTRL (pEnd, pDrvCtrl);

    el3c90xRxUnStall (pDrvCtrl);

    /* get the next upload descriptor */
    
    if ((pRxUpd = el3c90xNextUpdFetch (pDrvCtrl)) == NULL)
        {
        DRV_LOG (DRV_DEBUG_POLL_RX, "PRX no upd\n", 1, 2, 3, 4, 5, 6);
        return (EAGAIN);
        }

    /* get the mBlk associated with the upd */
    
    if ((pMblkTemp = el3c90xNextPktFetch (pDrvCtrl, pRxUpd)) == NULL)
        return (EAGAIN);

    /* copy the data into the given buffer */
    
    len = netMblkToBufCopy (pMblkTemp, pMblk->mBlkHdr.mData, NULL);

    netMblkClChainFree (pMblkTemp);

    if ((pMblk->mBlkHdr.mLen < len) || (!(pMblk->mBlkHdr.mFlags & M_EXT)))
	{
        DRV_LOG (DRV_DEBUG_POLL_RX, "PRX bad mblk len:%d flags:%d\n",
                 pMblk->mBlkHdr.mLen, pMblk->mBlkHdr.mFlags, 3, 4, 5, 6);
	return (EAGAIN);
	}

    pMblk->mBlkHdr.mLen = len;
    pMblk->mBlkHdr.mFlags |= M_PKTHDR;
    pMblk->mBlkPktHdr.len = len;

#ifndef INCLUDE_RFC_1213

    /* New RFC 2233 mib2 interface */

    /* RFC 2233 mib2 counter update for incoming packet */

    if (pDrvCtrl->endObj.pMib2Tbl != NULL)
        {
        pDrvCtrl->endObj.pMib2Tbl->m2PktCountRtn(pDrvCtrl->endObj.pMib2Tbl,
                                                 M2_PACKET_IN,
                                                 pMblk->mBlkHdr.mData,
                                                 pMblk->mBlkHdr.mLen);
        }
#endif /* INCLUDE_RFC_1213 */

    DRV_LOG (DRV_DEBUG_POLL_RX, "PRX ok\n", 1, 2, 3, 4, 5, 6);

    return (OK);
    }

/*******************************************************************************
*
* el3c90xInit - initialize the device
*
* This routine initializes device and enables the device interrupts. It
* initializes the descriptor rings
*
* RETURNS: N/A
*/

LOCAL void el3c90xInit
    (
    EL3C90X_DEVICE * pDrvCtrl	/* pointer to the device control structure */
    )
    {
    int			ix;
    UINT16		rxFilt = 0;
    UINT16		phyBMCR = 0;
    int			flags;

    /*
     * hack for the 3c905B: the built-in autoneg logic's state
     * gets reset by el3c90xInit() when we don't want it to. Try
     * to preserve it. (For 3c905 cards with real external PHYs,
     * the BMCR register doesn't change, but this doesn't hurt.)
     */
    if (pDrvCtrl->pPhyDevType != NULL)
        phyBMCR = el3c90xPhyRegRead(pDrvCtrl, PHY_BMCR);

    /* cancel pending I/O and free all RX/TX buffers. */

    el3c90xDevStop(pDrvCtrl);

    el3c90xWait(pDrvCtrl);

    /* get the MAC address */

    for (ix = 0; ix < EA_SIZE; ix++)
        {
        el3c90xCsrWriteByte (pDrvCtrl, EL_W2_STATION_ADDR_LO + ix,
                             pDrvCtrl->enetAddr [ix], EL_WIN_2);
	}

    /* clear the station mask */

    for (ix = 0; ix < 3; ix++)
        el3c90xCsrWriteWord (pDrvCtrl, EL_W2_STATION_MASK_LO + (ix * 2), 0,
                             EL_WIN_2);

    /* reset TX and RX */

    el3c90xCsrWriteWord (pDrvCtrl, EL_COMMAND, EL_CMD_RX_RESET, NONE);
    el3c90xWait(pDrvCtrl);
    el3c90xCsrWriteWord (pDrvCtrl, EL_COMMAND, EL_CMD_TX_RESET, NONE);
    el3c90xWait(pDrvCtrl);

    /* init circular upload descriptor list */
    
    if (el3c90xUpdInit(pDrvCtrl) == ENOBUFS)
        {
        DRV_LOG (DRV_DEBUG_LOAD, "elPci%d: initialization failed: no "
               "memory for rx buffers\n", pDrvCtrl->unit, 2, 3, 4, 5, 6);
        el3c90xDevStop(pDrvCtrl);
        return;
	}

    /* init download descriptors list */

    el3c90xDndInit(pDrvCtrl);

    /*
     * set the TX freethresh value.
     * note that this has no effect on 3c905B "cyclone"
     * cards but is required for 3c900/3c905 "boomerang"
     * cards in order to enable the download engine.
     */

    el3c90xCsrWriteByte (pDrvCtrl, EL_TX_FREETHRESH, (EL3C90X_BUFSIZ >> 8),
                         NONE);

    /*
     * If this is a 3c905B, also set the tx reclaim threshold.
     * This helps cut down on the number of tx reclaim errors
     * that could happen on a busy network. The chip multiplies
     * the register value by 16 to obtain the actual threshold
     * in bytes, so we divide by 16 when setting the value here.
     * The existing threshold value can be examined by reading
     * the register at offset 9 in window 5.
     */

    if (pDrvCtrl->devType == EL_TYPE_905B)
        {
        el3c90xCsrWriteWord (pDrvCtrl, EL_COMMAND,
                             (EL_CMD_SET_TX_RECLAIM | (EL3C90X_BUFSIZ >> 4)),
                             NONE);
        }

    /* set RX filter bits. */

    rxFilt = el3c90xCsrReadByte (pDrvCtrl, EL_W5_RX_FILTER, EL_WIN_5);

    /* set the individual bit to receive frames for this host only. */

    rxFilt |= EL_RXFILTER_INDIVIDUAL;

    flags = END_FLAGS_GET (&pDrvCtrl->endObj);

    /* if we want promiscuous mode, set the allframes bit. */

    if (flags & IFF_PROMISC)
        {
        rxFilt |= EL_RXFILTER_ALLFRAMES;
        el3c90xCsrWriteWord (pDrvCtrl, EL_COMMAND,
                             (EL_CMD_RX_SET_FILT | rxFilt), NONE);
	}
    else
        {
        rxFilt &= ~EL_RXFILTER_ALLFRAMES;
        el3c90xCsrWriteWord (pDrvCtrl, EL_COMMAND,
                             (EL_CMD_RX_SET_FILT | rxFilt), NONE);
	}

    /* set capture broadcast bit to capture broadcast frames. */

    if (flags & IFF_BROADCAST)
        {
        rxFilt |= EL_RXFILTER_BROADCAST;
        el3c90xCsrWriteWord (pDrvCtrl, EL_COMMAND,
                             (EL_CMD_RX_SET_FILT | rxFilt), NONE);
	}
    else
        {
        rxFilt &= ~EL_RXFILTER_BROADCAST;
        el3c90xCsrWriteWord (pDrvCtrl, EL_COMMAND,
                             (EL_CMD_RX_SET_FILT | rxFilt), NONE);
	}

    /* set the multicast filter etc */
        
    el3c90xMcastConfig (pDrvCtrl);

    /*
     * load the upload descriptor pointer and start the upload engine
     * Note that we have to do this after any RX resets have completed
     * since the uplist register is cleared by a reset.
     */

    el3c90xCsrWriteWord (pDrvCtrl, EL_COMMAND, EL_CMD_UP_STALL, NONE);
    el3c90xWait(pDrvCtrl);
    el3c90xCsrWriteLong (pDrvCtrl, EL_UPLIST_PTR,
                         (UINT32) EL3C90X_CACHE_VIRT_TO_PHYS
                         (&pDrvCtrl->pDescBlk->rxDescs[0]), NONE);
    el3c90xCsrWriteWord (pDrvCtrl, EL_COMMAND, EL_CMD_UP_UNSTALL, NONE);

    /*
     * if the coax transceiver is on, make sure to enable the DC-DC converter.
     */

    if (pDrvCtrl->xCvr == EL_XCVR_COAX)
        el3c90xCsrWriteWord (pDrvCtrl, EL_COMMAND, EL_CMD_COAX_START, NONE);
    else
        el3c90xCsrWriteWord (pDrvCtrl, EL_COMMAND, EL_CMD_COAX_STOP, NONE);

    /* Clear out the stats counters. */
    el3c90xCsrWriteWord (pDrvCtrl, EL_COMMAND, EL_CMD_STATS_DISABLE, NONE);

    el3c90xStatUpdate(pDrvCtrl);

    el3c90xCsrWriteWord (pDrvCtrl, EL_W4_NET_DIAG,
                         EL_NETDIAG_UPPER_BYTES_ENABLE, EL_WIN_4);
    el3c90xCsrWriteWord (pDrvCtrl, EL_COMMAND, EL_CMD_STATS_ENABLE, NONE);

    /* enable device interrupts */

    el3c90xCsrWriteWord (pDrvCtrl, EL_COMMAND, EL_CMD_INTR_ACK|0xFF, NONE);
    el3c90xCsrWriteWord (pDrvCtrl, EL_COMMAND, EL_CMD_STAT_ENB|EL_INTRS, NONE);
    el3c90xCsrWriteWord (pDrvCtrl, EL_COMMAND, EL_CMD_INTR_ENB|EL_INTRS, NONE);

    /* Set the RX early threshold */

    el3c90xCsrWriteWord (pDrvCtrl, EL_COMMAND,
                         (EL_CMD_RX_SET_THRESH | (EL3C90X_BUFSIZ >>2)), NONE);
    el3c90xCsrWriteWord (pDrvCtrl, EL_DMACTL, EL_DMACTL_UP_RX_EARLY, NONE);

    /* Enable receiver and transmitter. */

    el3c90xCsrWriteWord (pDrvCtrl, EL_COMMAND, EL_CMD_RX_ENABLE, NONE);
    el3c90xCsrWriteWord (pDrvCtrl, EL_COMMAND, EL_CMD_TX_ENABLE, NONE);

    /* Restore state of BMCR */

    if (pDrvCtrl->pPhyDevType != NULL)
        el3c90xPhyRegWrite(pDrvCtrl, PHY_BMCR, phyBMCR);

    return;
    }

/*******************************************************************************
*
* el3c90xMemInit - initialize memory for the device
*
* This function initiailizes memory for the device
*
* RETURNS: OK or ERROR
*
*/

LOCAL STATUS el3c90xMemInit
    (
    EL3C90X_DEVICE * 	pDrvCtrl	/* device to be initialized */
    )
    {
    int			memSize = 0;

    memSize += (sizeof(EL_DESC_BLK) + 16);

    if (pDrvCtrl->bufMtplr == NONE)
        pDrvCtrl->bufMtplr = 2;
    else if (pDrvCtrl->bufMtplr <= 0)
            printf ( "elPci: invalid buffer multiplier\n");
    
    pDrvCtrl->clDesc.clNum  = (EL_UPD_CNT + EL_DND_CNT) * pDrvCtrl->bufMtplr;
    pDrvCtrl->mClCfg.clBlkNum = pDrvCtrl->clDesc.clNum;
    pDrvCtrl->mClCfg.mBlkNum  = pDrvCtrl->mClCfg.clBlkNum * 2;

    /* total memory size for mBlks and clBlks */
    
    pDrvCtrl->mClCfg.memSize =
        (pDrvCtrl->mClCfg.mBlkNum  *  (MSIZE + sizeof (long))) +
        (pDrvCtrl->mClCfg.clBlkNum * (CL_BLK_SZ + sizeof (long)));

    memSize += pDrvCtrl->mClCfg.memSize;

    /* total memory size for all clusters */

    pDrvCtrl->clDesc.clSize  = EL3C90X_BUFSIZ;
    pDrvCtrl->clDesc.memSize =
        (pDrvCtrl->clDesc.clNum * (pDrvCtrl->clDesc.clSize + 8)) + sizeof(int);

    memSize += pDrvCtrl->clDesc.memSize;

    if ((int)pDrvCtrl->memAdrs != NONE)
        {
        /* check if the give memory size if enough */

        if (pDrvCtrl->memSize < memSize)
            {
            printf ( "elPci: memory size too small\n" );
            return (ERROR);
            }

        pDrvCtrl->pDescMem = (char *)pDrvCtrl->memAdrs;
        pDrvCtrl->mClCfg.memArea = (char *)((UINT32)(pDrvCtrl->pDescMem) +
                                            sizeof(EL_DESC_BLK) + 16);
        pDrvCtrl->clDesc.memArea = (pDrvCtrl->mClCfg.memArea +
                                    pDrvCtrl->mClCfg.memSize);

        /* assume pool is cache coherent, copy null structure */

        pDrvCtrl->cacheFuncs = cacheNullFuncs;
        DRV_LOG (DRV_DEBUG_LOAD, "Memory checks out\n", 1, 2, 3, 4, 5, 6);
        }
    else	/* allocate our own memory */
        {
        pDrvCtrl->flags |= EL_MEM_ALLOC_FLAG;

        if (!CACHE_DMA_IS_WRITE_COHERENT ())
            {
            printf ( "elPci: device requires cache coherent memory\n" );
            return (ERROR);
            }
        
        /* allocate memory for upload and download descriptors */

        pDrvCtrl->pDescMem =
            (char *) cacheDmaMalloc (sizeof(EL_DESC_BLK) + 16);

        if (pDrvCtrl->pDescMem == NULL)
            {
            printf ( "elPci: system memory unavailable\n" );
            return (ERROR);
            }

        /* allocate memory for mBlks and clBlks */

        if ((pDrvCtrl->mClCfg.memArea =
             (char *) memalign (sizeof(long), pDrvCtrl->mClCfg.memSize))
            == NULL)
            {
            printf ( "elPci: system memory unavailable\n" );
            goto elMemInitFail;
            }

        /* allocate memory for clusters */
        pDrvCtrl->clDesc.memArea = cacheDmaMalloc (pDrvCtrl->clDesc.memSize);
            
        if (pDrvCtrl->clDesc.memArea == NULL)
            {
            printf ( "elPci: system memory unavailable\n" );
            goto elMemInitFail;
            }

        /* copy the DMA structure */
        pDrvCtrl->cacheFuncs = cacheDmaFuncs;
        }

    pDrvCtrl->pDescBlk = (EL_DESC_BLK *)(((int)pDrvCtrl->pDescMem
                                                  + 0x0f) & ~0x0f);
    bzero ((char*)pDrvCtrl->pDescBlk, sizeof(EL_DESC_BLK));


    /* allocate memory for net pool */
    
    if ((pDrvCtrl->endObj.pNetPool = malloc (sizeof(NET_POOL))) == NULL)
        {
        printf ( "elPci: system memory unavailable\n" );
        goto elMemInitFail;
        }

#ifdef DRV_DEBUG
    pElXlPool = pDrvCtrl->endObj.pNetPool;
#endif
    
    /* initialize the device net pool */
    
    if (netPoolInit (pDrvCtrl->endObj.pNetPool, &pDrvCtrl->mClCfg,
                     &pDrvCtrl->clDesc, 1, NULL) == ERROR)
        {
        DRV_LOG (DRV_DEBUG_LOAD, "Could not init buffering\n",
                 1, 2, 3, 4, 5, 6);
        goto elMemInitFail;
        }
    
    /* Store the cluster pool id as others need it later. */

    pDrvCtrl->pClPoolId = clPoolIdGet (pDrvCtrl->endObj.pNetPool,
                                       EL3C90X_BUFSIZ, FALSE);

    return (OK);

    elMemInitFail:
        {
        if (pDrvCtrl->flags & EL_MEM_ALLOC_FLAG)
            {
            /* free the memory allocated for descriptors */
        
            if (pDrvCtrl->pDescMem != NULL)
                cacheDmaFree (pDrvCtrl->pDescMem);

            /* Free the memory allocated for mBlks and clBlks */

            if (pDrvCtrl->mClCfg.memArea != NULL)
                free (pDrvCtrl->mClCfg.memArea);

            /* free the memory allocated for clusters */

            if (pDrvCtrl->clDesc.memArea != NULL)
                cacheDmaFree (pDrvCtrl->clDesc.memArea);
            }

        if (pDrvCtrl->endObj.pNetPool != NULL)
            free (pDrvCtrl->endObj.pNetPool);

        return (ERROR);
        }
    }


/*******************************************************************************
*
* el3c90xDevStop - stop the device
*
* This function stops the adapter and frees up any M_BLKS allocated to the
* upload and download descriptors.
*
* RETURNS: N/A
*/

LOCAL void el3c90xDevStop
    (
    EL3C90X_DEVICE * 	pDrvCtrl /* pointer to the device control structure */
    )
    {
    register int	ix;

    el3c90xCsrWriteWord (pDrvCtrl, EL_COMMAND, EL_CMD_RX_DISABLE, NONE);
    el3c90xCsrWriteWord (pDrvCtrl, EL_COMMAND, EL_CMD_STATS_DISABLE, NONE);
    el3c90xCsrWriteWord (pDrvCtrl, EL_COMMAND, EL_CMD_INTR_ENB, NONE);
    el3c90xCsrWriteWord (pDrvCtrl, EL_COMMAND, EL_CMD_RX_DISCARD, NONE);

    el3c90xWait(pDrvCtrl);

    el3c90xCsrWriteWord (pDrvCtrl, EL_COMMAND, EL_CMD_TX_DISABLE, NONE);
    el3c90xCsrWriteWord (pDrvCtrl, EL_COMMAND, EL_CMD_COAX_STOP, NONE);
    el3c90xWait(pDrvCtrl);

    el3c90xCsrWriteWord (pDrvCtrl, EL_COMMAND, EL_CMD_RX_RESET, NONE);
    el3c90xWait(pDrvCtrl);

    el3c90xCsrWriteWord (pDrvCtrl, EL_COMMAND, EL_CMD_TX_RESET, NONE);
    el3c90xWait(pDrvCtrl);

    el3c90xCsrWriteWord (pDrvCtrl, EL_COMMAND,
                         (EL_CMD_INTR_ACK | EL_STAT_INTLATCH), NONE);

    /* free the mblks in the upload descriptor lists */
    
    for (ix = 0; ix < EL_UPD_CNT; ix++)
        {
        if (pDrvCtrl->rxTxChain.rxChain [ix].pMblk != NULL)
            {
            netMblkClChainFree(pDrvCtrl->rxTxChain.rxChain [ix].pMblk);
            pDrvCtrl->rxTxChain.rxChain [ix].pMblk = NULL;
            }
	}

    bzero ((char *)&pDrvCtrl->pDescBlk->rxDescs,
           sizeof(pDrvCtrl->pDescBlk->rxDescs));

    /* free the download descriptors */

    for (ix = 0; ix < EL_DND_CNT; ix++)
        {
        if (pDrvCtrl->rxTxChain.txChain [ix].pMblk != NULL)
            {
            netMblkClChainFree(pDrvCtrl->rxTxChain.txChain[ix].pMblk);
            pDrvCtrl->rxTxChain.txChain [ix].pMblk = NULL;
            }
	}

    bzero ((char *)&pDrvCtrl->pDescBlk->txDescs,
           sizeof(pDrvCtrl->pDescBlk->txDescs));

    return;
    }

/*******************************************************************************
*
* el3c90xReset - reset  the device
*
* This function call resets the device completely
*
* RETURNS: N/A
*/

LOCAL void el3c90xReset
    (
    EL3C90X_DEVICE * pDrvCtrl
    )
    {
    /* issue the reset command */
    
    el3c90xCsrWriteWord(pDrvCtrl, EL_COMMAND, EL_CMD_RESET, EL_WIN_0);

    el3c90xWait(pDrvCtrl);	/* wait for the command to complete */

    /* wait for a while */

    SYS_DELAY(1000);
    return;
    }

/*******************************************************************************
*
* el3c90xIntEnable - enable board to cause interrupts
*
* Because the board has maskable status, this routine can simply set the
* mask to all ones.  We set all the bits symbolically; the effect is the
* same.  Note that the interrupt latch is not maskable; if none of the other
* mask bits are set, no interrupts will occur at all.  Only those interrupts
* whose status bits are enabled will actually occur.  Note that the "intMask"
* field in the device control structure is really the status mask.
*
* RETURNS: N/A.
*/

LOCAL void el3c90xIntEnable
    (
    EL3C90X_DEVICE * pDrvCtrl	/* device structure */
    )
    {
    UINT16  status;

    status = el3c90xCsrReadWord (pDrvCtrl, EL_STATUS, NONE);
    status &= 0x00ff;
    el3c90xCsrWriteWord (pDrvCtrl, EL_COMMAND, (EL_CMD_INTR_ACK | status),
                         NONE);
    el3c90xCsrWriteWord (pDrvCtrl, EL_COMMAND, (EL_CMD_INTR_ENB | EL_INTRS),
                         NONE);
    }

/*******************************************************************************
*
* el3c90xIntDisable - prevent board from causing interrupts
*
* This routine simply sets all the interrupt mask bits to zero.
* It is intended for guarding board-critical sections.
*
* RETURNS: N/A.
*/

LOCAL void el3c90xIntDisable
    (
    EL3C90X_DEVICE * pDrvCtrl	/* device structure */
    )
    {
    UINT16  status;

    el3c90xCsrWriteWord (pDrvCtrl, EL_COMMAND, EL_CMD_INTR_ENB, NONE);
    status = el3c90xCsrReadWord (pDrvCtrl, EL_STATUS, NONE);
    status &= 0x00ff;
    el3c90xCsrWriteWord (pDrvCtrl, EL_COMMAND, (EL_CMD_INTR_ACK | status),
                         NONE);
    }

/*******************************************************************************
*
* el3c90xPollStart - start polled mode operations
*
* This function starts polled mode operation.
*
* The device interrupts are disabled, the current mode flag is switched
* to indicate Polled mode and the device is reconfigured.
*
* RETURNS: OK or ERROR.
*/

LOCAL STATUS el3c90xPollStart
    (
    EL3C90X_DEVICE * pDrvCtrl	/* device to be polled */
    )
    {
    int         oldLevel;
    
    oldLevel = intLock ();          /* disable ints during update */

    pDrvCtrl->flags |= EL_POLLING;

    intUnlock (oldLevel);   /* now el3c90xInt won't get confused */

    el3c90xIntDisable (pDrvCtrl);

    ENDLOGMSG (("STARTED\n", 1, 2, 3, 4, 5, 6));

    return (OK);
    }

/*******************************************************************************
*
* el3c90xPollStop - stop polled mode operations
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

LOCAL STATUS el3c90xPollStop
    (
    EL3C90X_DEVICE * pDrvCtrl	/* device structure */
    )
    {
    int         oldLevel;

    oldLevel = intLock ();	/* disable ints during register updates */

    pDrvCtrl->flags &= ~EL_POLLING;

    intUnlock (oldLevel);

    el3c90xIntEnable (pDrvCtrl);

    ENDLOGMSG (("STOPPED\n", 1, 2, 3, 4, 5, 6));

    return (OK);
    }

/*******************************************************************************
*
* el3c90xUpdInit - initialize the upload descriptor list
*
* This function initializes the upload descriptors and allocates M_BLKs for
* them. Note that we arrange the descriptors in a closed ring, so that the
* last descriptor points back to the first.
*
* RETURNS: OK or ENOBUFS
*/

LOCAL int el3c90xUpdInit
    (
    EL3C90X_DEVICE * pDrvCtrl	/* pointer to the device control structure */
    )
    {
    EL_RX_TX_CHAIN *	pRxTxChain;
    EL_DESC_BLK	*	pDescBlk;
    int			ix;

    pRxTxChain = &pDrvCtrl->rxTxChain;
    pDescBlk = pDrvCtrl->pDescBlk;

    for (ix = 0; ix < EL_UPD_CNT; ix++)
        {
        pRxTxChain->rxChain [ix].pDesc =
            (EL_SIMPLE_DESC *)&pDescBlk->rxDescs [ix];
			
        if (el3c90xUpdFill(pDrvCtrl, &pRxTxChain->rxChain [ix]) == ENOBUFS)
            return(ENOBUFS);
        
        if (ix == (EL_UPD_CNT - 1))
            {
            pRxTxChain->rxChain [ix].pNextChain = &pRxTxChain->rxChain [0];
            pDescBlk->rxDescs [ix].nextDesc =
                PCI_SWAP
                (
                (UINT32)EL3C90X_CACHE_VIRT_TO_PHYS (&pDescBlk->rxDescs [0])
                );
            }
        else
            {
            pRxTxChain->rxChain [ix].pNextChain =
                &pRxTxChain->rxChain [ix + 1];
            pDescBlk->rxDescs [ix].nextDesc =
                PCI_SWAP((UINT32)EL3C90X_CACHE_VIRT_TO_PHYS
                         (&pDescBlk->rxDescs [ix + 1]));
            }
	}
    pRxTxChain->pRxHead = &pRxTxChain->rxChain [0];
    return (OK);
    }

/*******************************************************************************
*
* el3c90xDndInit - initialize the download descriptor list
*
* This function initializes the download descriptor list
*
* RETURNS: OK
*/

LOCAL int el3c90xDndInit
    (
    EL3C90X_DEVICE * pDrvCtrl	/* pointer to the device control structure */
    )
    {
    EL_RX_TX_CHAIN *	pRxTxChain;
    EL_DESC_BLK	*	pDescBlk;
    int			ix;

    pRxTxChain = &pDrvCtrl->rxTxChain;
    pDescBlk = pDrvCtrl->pDescBlk;
    
    for (ix = 0; ix < EL_DND_CNT; ix++)
        {
        pRxTxChain->txChain [ix].pDesc = &pDescBlk->txDescs [ix];
        if (ix == (EL_DND_CNT - 1))
            pRxTxChain->txChain [ix].pNextChain = NULL;
        else
            pRxTxChain->txChain[ix].pNextChain = &pRxTxChain->txChain [ix + 1];
	}

    pRxTxChain->pTxFree = &pRxTxChain->txChain [0];
    pRxTxChain->pTxTail = pRxTxChain->pTxHead = NULL;

    return(0);
    }

/*******************************************************************************
*
* el3c90xHashGet - get a hash filter bit position
*
* This routine is taken from the 3Com Etherlink XL manual,
* chapter 10 pg 156. It calculates a CRC of the supplied multicast
* group address and returns the lower 8 bits, which are used
* as the multicast filter position.
* Note: the 3c905B currently only supports a 64-bit hash table,
* which means we really only need 6 bits, but the manual indicates
* that future chip revisions will have a 256-bit hash table,
* hence the routine is set up to calculate 8 bits of position
* info in case we need it some day.
* Note II, The Sequel: _CURRENT_ versions of the 3c905B have a
* 256 bit hash table. This means we have to use all 8 bits regardless.
* On older cards, the upper 2 bits will be ignored. Grrrr....
*
* RETURNS: filter bit position
*/

LOCAL UINT8 el3c90xHashGet
    (
    char * 	addr
    )
    {
    UINT32	crc;
    UINT32	carry;
    int		ix;
    int		jx;
    UINT8	ch;

    /* Compute CRC for the address value. */
    crc = 0xFFFFFFFF; /* initial value */

    for (ix = 0; ix < 6; ix++)
        {
        ch = *(addr + ix);

        for (jx = 0; jx < 8; jx++)
            {
            carry = ((crc & 0x80000000) ? 1 : 0) ^ (ch & 0x01);
            crc <<= 1;
            ch >>= 1;
            if (carry)
                crc = (crc ^ 0x04c11db6) | carry;
            }
        }

    /* return the filter bit position */

    return (crc & 0x000000FF);
    }

/*******************************************************************************
*
* el3c90xFilterSet - set the multicast filter
*
* NICs older than the 3c905B have only one multicast option, which
* is to enable reception of all multicast frames.
*
* RETURNS: N/A
*/

LOCAL void el3c90xFilterSet
    (
    EL3C90X_DEVICE * pDrvCtrl	/* device to be re-configured */
    )
    {
    UINT8		rxFilt;

    rxFilt = el3c90xCsrReadByte(pDrvCtrl, EL_W5_RX_FILTER, EL_WIN_5);

    if (END_FLAGS_GET (&pDrvCtrl->endObj) & IFF_ALLMULTI)
        {
        rxFilt |= EL_RXFILTER_ALLMULTI;
        el3c90xCsrWriteWord (pDrvCtrl, EL_COMMAND,
                             (EL_CMD_RX_SET_FILT | rxFilt), NONE);
        return;
        }

    /* Set up address filter for multicasting. 
     * Multicast bit is set if the address list has one or more entries
     * Multicasting is disabled if the address list is empty
     */

    if (END_MULTI_LST_CNT (&pDrvCtrl->endObj) > 0)
	{
	ENDLOGMSG (("Setting multicast mode on!\n", 1, 2, 3, 4, 5, 6));
        rxFilt |= EL_RXFILTER_ALLMULTI;
	}
    else
	{
	ENDLOGMSG (("Setting multcast mode off!\n", 1, 2, 3, 4, 5, 6));
        rxFilt &= ~EL_RXFILTER_ALLMULTI;
	}

    el3c90xCsrWriteWord (pDrvCtrl, EL_COMMAND,
                         (EL_CMD_RX_SET_FILT | rxFilt), NONE);
    return;
    }

/*******************************************************************************
*
* el3c90xHashFilterSet - set the hash filter for 3c90xB adapters.
*
* This function programs the hash fileter for the 3c905B adapters.
*
* RETURNS: N/A
*/

LOCAL void el3c90xHashFilterSet
    (
    EL3C90X_DEVICE * pDrvCtrl	/* device to be re-configured */
    )
    {
    ETHER_MULTI * 	pCurr;
    UINT8		rxFilt;
    int			ix;
    int			hash = 0;
    int			mcnt = 0;

    rxFilt = el3c90xCsrReadByte(pDrvCtrl, EL_W5_RX_FILTER, EL_WIN_5);

    if (END_FLAGS_GET (&pDrvCtrl->endObj) & IFF_ALLMULTI)
        {
        rxFilt |= EL_RXFILTER_ALLMULTI;
        el3c90xCsrWriteWord (pDrvCtrl, EL_COMMAND,
                             (EL_CMD_RX_SET_FILT | rxFilt), NONE);
        return;
        }
    else
        rxFilt &= ~EL_RXFILTER_ALLMULTI;

    /* first, zot all the existing hash bits */

    for (ix = 0; ix < EL_HASHFILT_SIZE; ix++)
        el3c90xCsrWriteWord (pDrvCtrl, EL_COMMAND, (EL_CMD_RX_SET_HASH | ix),
                             NONE);

    /* walk through the list and recalculate the hash filter */

    pCurr = END_MULTI_LST_FIRST (&pDrvCtrl->endObj);

    while (pCurr != NULL)
	{
        hash = el3c90xHashGet ((UINT8 *)&pCurr->addr);
        el3c90xCsrWriteWord (pDrvCtrl, EL_COMMAND,
                             (EL_CMD_RX_SET_HASH | EL_HASH_SET | hash), NONE);
        mcnt++;
	pCurr = END_MULTI_LST_NEXT(pCurr);
        }
    
    if (mcnt)
        rxFilt |= EL_RXFILTER_MULTIHASH;
    else
        rxFilt &= ~EL_RXFILTER_MULTIHASH;

    el3c90xCsrWriteWord (pDrvCtrl, EL_COMMAND,
                         (EL_CMD_RX_SET_FILT | rxFilt), NONE);
    return;
    }

/******************************************************************************
*
* el3c90xMcastConfig - reconfigure the interface under us.
*
* Reconfigure the interface setting promiscuous mode, and changing the
* multicast interface list.
*
* RETURNS: N/A.
*/

LOCAL void el3c90xMcastConfig
    (
    EL3C90X_DEVICE * pDrvCtrl	/* device to be re-configured */
    )
    {
    if (pDrvCtrl->devType == EL_TYPE_905B)
        el3c90xHashFilterSet (pDrvCtrl);
    else
        el3c90xFilterSet (pDrvCtrl);

    return;
    }

/*******************************************************************************
*
* el3c90xUpdFill - initialize a single upload descriptor
*
* This function allocates an mBlk/clBlk/cluster tuple and initialize the
* upload descriptor.
*
* RETURNS: OK or ERROR.
*/

LOCAL STATUS el3c90xUpdFill
    (
    EL3C90X_DEVICE * 		pDrvCtrl, /* ptr to the dev ctrl structure */
    EL_SIMPLE_DESC_CHAIN * 	pRxUpd	  /* ptr to the descriptor chain */
    )
    {
    M_BLK_ID 	pMblk = NULL;	/* pointer to the mBlk */

    /* get a mBlk/clBlk/cluster tuple */

    if ((pMblk = netTupleGet (pDrvCtrl->endObj.pNetPool, EL3C90X_BUFSIZ,
                              M_DONTWAIT, MT_DATA, FALSE)) == NULL)
        {
        ENDLOGMSG (("Out of M Blocks!\n", 1, 2, 3, 4, 5, 6));
        return (ERROR);
        }

    pRxUpd->pMblk = pMblk;

    pRxUpd->pDesc->descFrag.fragAddr =
        PCI_SWAP((UINT32) EL3C90X_CACHE_VIRT_TO_PHYS (pMblk->mBlkHdr.mData));

    pRxUpd->pDesc->status = 0;
    pRxUpd->pDesc->descFrag.fragLen = PCI_SWAP (CL_SIZE_2048 | EL_LAST_FRAG);

    return (OK);
    }

/*******************************************************************************
*
* el3c90xDndGet - get a free down load descriptor
*
* This function returns a pointer to the descriptor chain which has a free
* down load descriptor.
*
* RETURNS: EL_DESC_CHAIN * or NULL
*/

LOCAL EL_DESC_CHAIN	* el3c90xDndGet
    (
    EL3C90X_DEVICE * pDrvCtrl	/* pointer to the device control structure */
    )
    {
    EL_DESC_CHAIN * 	pFree = NULL;

    if ((pFree = pDrvCtrl->rxTxChain.pTxFree) == NULL)
        return (NULL);

    pDrvCtrl->rxTxChain.pTxFree = pFree->pNextChain;

    pFree->pNextChain = NULL;
    return (pFree);
    }

/*******************************************************************************
*
* el3c90xDndFree - free a down load descriptor
*
* This function returns a the down load descriptor chain to the free list.
*
* RETURNS: OK
*/

LOCAL STATUS el3c90xDndFree
    (
    EL3C90X_DEVICE * 	pDrvCtrl, /* pointer to the device control structure */
    EL_DESC_CHAIN * 	pTxChain
    )
    {
    pTxChain->pMblk = NULL;
    
    pTxChain->pNextChain = pDrvCtrl->rxTxChain.pTxFree;
    pDrvCtrl->rxTxChain.pTxFree = pTxChain;

    return (OK);
    }

/*******************************************************************************
*
* el3c90xTxDequeue - dequeue a transmitted descriptor 
*
* This function dequeues a transmitted descriptor for clean up after
* transmission.
*
* RETURNS: EL_DESC_CHAIN * or NULL
*/

LOCAL EL_DESC_CHAIN	* el3c90xTxDequeue
    (
    EL3C90X_DEVICE * pDrvCtrl	/* pointer to the device control structure */
    )
    {
    EL_DESC_CHAIN * 	pTxFree = NULL;

    if ((pTxFree = pDrvCtrl->rxTxChain.pTxHead) == NULL)
        {
        pDrvCtrl->rxTxChain.pTxTail = NULL;
        return (NULL);
        }
    else
        {

        /* we have to use a different test if devType != EL_TYPE_905B. */
        
        if ((pDrvCtrl->devType == EL_TYPE_905B &&
             !(PCI_SWAP(pTxFree->pDesc->status) & EL_TXSTAT_DL_COMPLETE)) ||
            el3c90xCsrReadLong (pDrvCtrl, EL_DOWNLIST_PTR, NONE))
            {
            if (el3c90xCsrReadLong (pDrvCtrl, EL_DMACTL, NONE) &
                EL_DMACTL_DOWN_STALLED ||
                !el3c90xCsrReadLong(pDrvCtrl, EL_DOWNLIST_PTR, NONE))
                {
                el3c90xCsrWriteLong (pDrvCtrl, EL_DOWNLIST_PTR,
                                     (UINT32) EL3C90X_CACHE_VIRT_TO_PHYS
                                     (pTxFree->pDesc), NONE);
                el3c90xCsrWriteWord (pDrvCtrl, EL_COMMAND, EL_CMD_DOWN_UNSTALL,
                                     NONE);
                }
            return (NULL);
            }
        }

    pDrvCtrl->rxTxChain.pTxHead = pTxFree->pNextChain;
    pTxFree->pNextChain = NULL;
    return (pTxFree);
    }

/*******************************************************************************
*
* el3c90xDndEnqueue - enqueue a down load descriptor 
*
* This function enqueues a down load descriptor 
*
* RETURNS: OK.
*/

LOCAL STATUS el3c90xDndEnqueue
    (
    EL3C90X_DEVICE * 	pDrvCtrl, /* pointer to the device control structure */
    EL_DESC_CHAIN * 	pTxChain
    )
    {
    if (el3c90xCsrReadLong (pDrvCtrl, EL_DOWNLIST_PTR, NONE))
        {
        el3c90xCsrWriteWord (pDrvCtrl, EL_COMMAND, EL_CMD_DOWN_STALL, NONE);

        el3c90xWait(pDrvCtrl);

        /* check once again */
        
        if (el3c90xCsrReadLong (pDrvCtrl, EL_DOWNLIST_PTR, NONE)) 
            {
            /*
             * Place the request for the upload interrupt
             * in the last descriptor in the chain. This way, if
             * we're chaining several packets at once, we'll only
             * get an interupt once for the whole chain rather than
             * once for each packet.
             */

            pDrvCtrl->rxTxChain.pTxTail->pNextChain = pTxChain;
            pDrvCtrl->rxTxChain.pTxTail->pDesc->nextDesc =
                PCI_SWAP((UINT32)EL3C90X_CACHE_VIRT_TO_PHYS(pTxChain->pDesc));
            pDrvCtrl->rxTxChain.pTxTail->pDesc->status &=
                PCI_SWAP(~EL_TXSTAT_DL_INTR);
            pDrvCtrl->rxTxChain.pTxTail = pTxChain;

            el3c90xCsrWriteWord (pDrvCtrl, EL_COMMAND, EL_CMD_DOWN_UNSTALL,
                                 NONE);
            return (OK);
            }
        }

    if (pDrvCtrl->rxTxChain.pTxHead == NULL)
        {
        pDrvCtrl->rxTxChain.pTxHead = pTxChain;
        pDrvCtrl->rxTxChain.pTxTail = pTxChain;
        }
    else
        {
        pDrvCtrl->rxTxChain.pTxTail->pNextChain = pTxChain;
        pDrvCtrl->rxTxChain.pTxTail = pTxChain;
        }

    /* load the down load descriptor */
    el3c90xCsrWriteLong (pDrvCtrl, EL_DOWNLIST_PTR,
                         (UINT32)EL3C90X_CACHE_VIRT_TO_PHYS (pTxChain->pDesc),
                         NONE);
    return (OK);
    }

/*******************************************************************************
*
* el3c90xDndMblkPack - pack a given mBlk into download descriptor
*
* This function packs a given mBlk into download descriptor. This function
* copies the data in a given mBlk into a new mBlk obtained from the pool.
*
* RETURNS: OK or ERROR.
*/

LOCAL STATUS el3c90xDndMblkPack
    (
    EL3C90X_DEVICE * 	pDrvCtrl, /* pointer to the device control structure */
    EL_DESC_CHAIN *	pTxChain,
    M_BLK_ID		pMblk
    )
    {
    int			len = 0;
    EL_FRAG *		pFrag = NULL;
    M_BLK_ID		pMblkTemp = NULL;

    if (pTxChain == NULL || pMblk == NULL)
        return (ERROR);
    
    if ((pMblkTemp = netTupleGet (pDrvCtrl->endObj.pNetPool,
                                  EL3C90X_BUFSIZ,
                                  M_DONTWAIT, MT_DATA, FALSE)) == NULL)
        {
        ENDLOGMSG (("Out of Tx M Blocks!\n", 1, 2, 3, 4, 5, 6));
        return (ERROR);
        }

    len = netMblkToBufCopy (pMblk, pMblkTemp->mBlkHdr.mData, NULL);

    pMblkTemp->mBlkPktHdr.len = pMblkTemp->mBlkHdr.mLen = len;
    
    pFrag = &pTxChain->pDesc->descFrag [0];

    pFrag->fragAddr =
        PCI_SWAP((UINT32)EL3C90X_CACHE_VIRT_TO_PHYS(pMblkTemp->mBlkHdr.mData));
    pFrag->fragLen = PCI_SWAP (len | EL_LAST_FRAG);

    pTxChain->pMblk = pMblkTemp;
    pTxChain->pDesc->status = PCI_SWAP (len);
    pTxChain->pDesc->nextDesc = 0;

    return(OK);
    }

/*******************************************************************************
*
* el3c90xDndSet - set up a download descriptor 
*
* This function seta up a download descriptor by setting fragment addresses
* to the data pointers in the mBlk chain. If the number of fragments
* exceeds 63 then the whole whole mBlk chain is copied into a contiguous
* buffer.
*
* RETURNS: OK or ERROR.
*/

LOCAL int el3c90xDndSet
    (
    EL3C90X_DEVICE * 	pDrvCtrl, /* pointer to the device control structure */
    EL_DESC_CHAIN *	pTxChain,
    M_BLK_ID		pMblkHead
    )
    {
    int			len = 0;
    int			frag = 0;
    EL_FRAG *		pFrag = NULL;
    M_BLK_ID		pMblk;
    M_BLK_ID		pMblkTemp = NULL;

    /*
     * start packing the M_BLKs in this chain into the fragment pointers.
     * stop when we run out of fragments or hit the end of the M_BLK chain.
     */
    pMblk = pMblkHead;

    for (pMblk = pMblkHead, frag = 0; pMblk != NULL; pMblk = pMblk->m_next)
        {
        if (pMblk->mBlkHdr.mLen != 0)
            {
            if (frag == EL_MAXFRAGS)
                break;

            len += pMblk->mBlkHdr.mLen;

            pTxChain->pDesc->descFrag [frag].fragAddr =
                PCI_SWAP ((UINT32)EL3C90X_CACHE_VIRT_TO_PHYS
                          (pMblk->mBlkHdr.mData));

            CACHE_DMA_FLUSH (pMblk->mBlkHdr.mData, pMblk->mBlkHdr.mLen);

            pTxChain->pDesc->descFrag[frag].fragLen =
                PCI_SWAP (pMblk->mBlkHdr.mLen);

            frag++;
            }
        }

    /*
     * Handle special case: we used up all 63 fragments,
     * but we have more M_BLKs left in the chain. Copy the
     * data into an M_BLK cluster. Note that we don't
     * bother clearing the values in the other fragment
     * pointers/counters; it wouldn't gain us anything,
     * and would waste cycles.
     */
    if (pMblk != NULL)
        {
        /* get a mBlk/clBlk/cluster tuple */
        if ((pMblkTemp = netTupleGet (pDrvCtrl->endObj.pNetPool,
                                      EL3C90X_BUFSIZ,
                                      M_DONTWAIT, MT_DATA, FALSE)) == NULL)
            {
            ENDLOGMSG (("Out of Tx M Blocks!\n", 1, 2, 3, 4, 5, 6));
            return (ERROR);
            }

        len = netMblkToBufCopy (pMblkHead, pMblkTemp->mBlkHdr.mData, NULL);

        pMblkTemp->mBlkPktHdr.len = pMblkTemp->mBlkHdr.mLen = len;

        netMblkClChainFree (pMblkHead); /* free the given mBlk chain */

        pMblkHead = pMblkTemp;
        pFrag = &pTxChain->pDesc->descFrag [0];
        pFrag->fragAddr =
            PCI_SWAP
            (
            (UINT32) EL3C90X_CACHE_VIRT_TO_PHYS (pMblkTemp->mBlkHdr.mData)
            );

        pFrag->fragLen = PCI_SWAP (len);
        frag = 1;
	}

    pTxChain->pMblk = pMblkHead;
    pTxChain->pDesc->descFrag [frag - 1].fragLen |=  PCI_SWAP (EL_LAST_FRAG);
    pTxChain->pDesc->status = PCI_SWAP (len);
    pTxChain->pDesc->nextDesc = 0;

    return(OK);
    }

/******************************************************************************
*
* el3c90xNextUpdFetch - fetch the next upload packet descriptor
*
* This routine fetches the next upload packet descriptor
*
* RETURNS: N/A.
*/

LOCAL EL_SIMPLE_DESC_CHAIN * el3c90xNextUpdFetch
    (
    EL3C90X_DEVICE * 	pDrvCtrl /* pointer to the device control structure */
    )
    {
    EL_SIMPLE_DESC_CHAIN * 	pRxChain = NULL;
    UINT16			rxstat;

    if ((rxstat = PCI_SWAP(pDrvCtrl->rxTxChain.pRxHead->pDesc->status)))
        {
        pRxChain = pDrvCtrl->rxTxChain.pRxHead;
        pDrvCtrl->rxTxChain.pRxHead = pRxChain->pNextChain;
        }

    return (pRxChain);
    }

/******************************************************************************
*
* el3c90xNextPktFetch - fetch the next packet buffer
*
* This routine fetches the next packet buffer uploaded and returns
* an mBlk. 
*
* NOMANUAL
*
* RETURNS: M_BLK_ID or NULL
*/

LOCAL M_BLK_ID el3c90xNextPktFetch
    (
    EL3C90X_DEVICE * 		pDrvCtrl, /* pointer to control structure */
    EL_SIMPLE_DESC_CHAIN * 	pRxUpd
    )
    {
    M_BLK_ID 		pMblk;	/* pointer to the mBlk */
    int			len;

    /* get a cluster */
    pMblk = pRxUpd->pMblk;
    
    len = (PCI_SWAP(pRxUpd->pDesc->status) & EL_RXSTAT_LENMASK);

    /* load new buffers into the Upload descriptor */
    if (el3c90xUpdFill (pDrvCtrl, pRxUpd) != OK)
        {
        /* if no buffers available drop the packet */

#ifdef INCLUDE_RFC_1213
       
        /* Old RFC 1213 mib2 interface */

        END_ERR_ADD (&pDrvCtrl->endObj, MIB2_IN_ERRS, +1);
#else 
        /* New RFC 2233 mib2 interface */

        if (pDrvCtrl->endObj.pMib2Tbl != NULL)
            {
            pDrvCtrl->endObj.pMib2Tbl->m2CtrUpdateRtn(pDrvCtrl->endObj.pMib2Tbl,
                                                      M2_ctrId_ifInErrors, 1);
            }
#endif /* INCLUDE_RFC_1213 */

        pRxUpd->pDesc->status = 0;
        return (NULL);
        }

    /* update the mBlk fields */
    pMblk->mBlkHdr.mLen   = len;
    pMblk->mBlkHdr.mFlags |= M_PKTHDR;
    pMblk->mBlkPktHdr.len = len;

    return (pMblk);
    }

/******************************************************************************
*
* el3c90xRxUnStall - unstall the receiver
*
* This routine unstalls the receiver if it the receiver hits an
* end of channel condition.
*
* RETURNS: OK or ERROR
*/

LOCAL STATUS el3c90xRxUnStall
    (
    EL3C90X_DEVICE * pDrvCtrl	/* pointer to the device control structure */
    )
    {
    if (el3c90xCsrReadLong (pDrvCtrl, EL_UPLIST_PTR, NONE) == 0 ||
        (el3c90xCsrReadLong (pDrvCtrl, EL_UPLIST_STATUS, NONE) &
         EL_PKTSTAT_UP_STALLED))
        {
        /* stall the receiver */
        el3c90xCsrWriteWord (pDrvCtrl, EL_COMMAND, EL_CMD_UP_STALL, NONE);

        el3c90xWait(pDrvCtrl);

        el3c90xCsrWriteLong (pDrvCtrl, EL_UPLIST_PTR,
                             (UINT32) EL3C90X_CACHE_VIRT_TO_PHYS
                             (&pDrvCtrl->pDescBlk->rxDescs [0]), NONE);

        pDrvCtrl->rxTxChain.pRxHead = &pDrvCtrl->rxTxChain.rxChain [0];

        /* unstall the receiver */

        el3c90xCsrWriteWord (pDrvCtrl, EL_COMMAND, EL_CMD_UP_UNSTALL, NONE);

        return (OK);
        }

    return (ERROR);
    }

/******************************************************************************
*
* el3c90xRxKick - kicks the receiver if blocked.
*
* This routine kicks the receiver if it is stalled.
* Handle the 'end of channel' condition. When the upload engine hits the
* end of the RX ring, it will stall. This is our cue to flush the RX ring,
* reload the uplist pointer register and unstall the engine.
* With the ThunderLAN chip, you get an interrupt when the receiver hits the end
* of the receive ring, which tells you exactly when you you need to reload
* the ring pointer. Here we have to
* fake it.
* This function reschedules the receive handler if things get
* stalled because of high receive traffic. This routine can also be
* called from the transmit routine after the packet has been queued.
* This will take advantage of the parallel tasking concept which 3COM
* is familiar for.
*
* RETURNS: N/A.
*/

LOCAL void el3c90xRxKick
    (
    EL3C90X_DEVICE * pDrvCtrl	/* pointer to the device control structure */
    )
    {
    /* if the receiver unstalled schedule the receive again */

    if (el3c90xRxUnStall (pDrvCtrl) == OK)
        {
        if (netJobAdd ((FUNCPTR)el3c90xIntrRx, (int) pDrvCtrl, 0, 0, 0, 0)
            != OK)
            logMsg ("elPci: netJobAdd (el3c90xIntrRx) failed\n", 0, 0, 0,
                    0, 0, 0);
        }
    }

/******************************************************************************
*
* el3c90xIntrRx - handle receive interrupts 
*
* This routine handles receive interrupts. 
*
* RETURNS: N/A.
*/

LOCAL void el3c90xIntrRx
    (
    EL3C90X_DEVICE * pDrvCtrl	/* pointer to the device control structure */
    )
    {
    M_BLK_ID 			pMblk;		/* pointer to the mBlk */
    EL_SIMPLE_DESC_CHAIN * 	pRxUpd;
    UINT32			descStatus;

    while ((pRxUpd = el3c90xNextUpdFetch (pDrvCtrl)) != NULL)
        {
        descStatus = PCI_SWAP(pRxUpd->pDesc->status);
        
        /*
         * If an error occurs, update stats, clear the
         * status word and leave the M_BLK cluster in place:
         * it should simply get re-used next time this descriptor
         * comes up in the ring.
         */
        if (descStatus & EL_RXSTAT_UP_ERROR)
            {
#ifdef INCLUDE_RFC_1213

            /* Old RFC 1213 mib2 interface */

            END_ERR_ADD (&pDrvCtrl->endObj, MIB2_IN_ERRS, +1);

#else
            /* New RFC 2233 mib2 interface */

            if (pDrvCtrl->endObj.pMib2Tbl != NULL)
                {
                pDrvCtrl->endObj.pMib2Tbl->m2CtrUpdateRtn(pDrvCtrl->endObj.
                                                                    pMib2Tbl,
                                                          M2_ctrId_ifInErrors,
                                                          1);
                }
#endif /* INCLUDE_RFC_1213 */

            pRxUpd->pDesc->status = 0;
            continue;
            }

        /*
         * If there error bit was not set, the upload complete
         * bit should be set which means we have a valid packet.
         * If not, something truly strange has happened.
         */
        if (!(descStatus & EL_RXSTAT_UP_CMPLT))
            {
            logMsg ("elPci%d: bad receive status -- packet dropped",
                    pDrvCtrl->unit, 0, 0, 0, 0, 0);
#ifdef INCLUDE_RFC_1213

            /* Old RFC 1213 mib2 interface */

            END_ERR_ADD (&pDrvCtrl->endObj, MIB2_IN_ERRS, +1);
#else
            /* New RFC 2233 mib2 interface */
            
            if (pDrvCtrl->endObj.pMib2Tbl != NULL)
                {
                pDrvCtrl->endObj.pMib2Tbl->m2CtrUpdateRtn(pDrvCtrl->endObj.
                                                                    pMib2Tbl,
                                                          M2_ctrId_ifInErrors,
                                                          1);
                }
#endif /* INCLUDE_RFC_1213 */

            pRxUpd->pDesc->status = 0;
            continue;
            }

        if ((pMblk = el3c90xNextPktFetch (pDrvCtrl, pRxUpd)) == NULL)
            continue;

#ifdef INCLUDE_RFC_1213

        /* Old RFC 1213 mib2 interface */
        END_ERR_ADD (&pDrvCtrl->endObj, MIB2_IN_UCAST, +1);

#else
        /* New RFC 2233 mib2 interface */

        /* RFC 2233 mib2 counter update for incoming packet */

        if (pDrvCtrl->endObj.pMib2Tbl != NULL)
            {
            pDrvCtrl->endObj.pMib2Tbl->m2PktCountRtn(pDrvCtrl->endObj.pMib2Tbl,
                                                     M2_PACKET_IN,
                                                     pMblk->mBlkHdr.mData,
                                                     pMblk->mBlkHdr.mLen);
            }
#endif /* INCLUDE_RFC_1213 */

        ENDLOGMSG (("Calling upper layer's recv rtn\n", 1, 2, 3, 4, 5, 6));

        END_RCV_RTN_CALL (&pDrvCtrl->endObj, pMblk);
        }

    /* kick the receiver if stalled */
    
    el3c90xRxKick (pDrvCtrl);

    el3c90xIntrSet (pDrvCtrl, EL_STAT_UP_COMPLETE);
    }

/******************************************************************************
*
* el3c90xIntrTx - handle transmit interrupt
*
* This routine handles transmit interrupt
*
* RETURNS: N/A.
*/

LOCAL void el3c90xIntrTx
    (
    EL3C90X_DEVICE * pDrvCtrl	/* pointer to the device control structure */
    )
    {
    EL_DESC_CHAIN * 	pTxChain;

    END_TX_SEM_TAKE (&pDrvCtrl->endObj, WAIT_FOREVER);

    while ((pTxChain = el3c90xTxDequeue (pDrvCtrl)) != NULL)
        {
        netMblkClChainFree (pTxChain->pMblk);
        el3c90xDndFree (pDrvCtrl, pTxChain);
        }

    END_TX_SEM_GIVE (&pDrvCtrl->endObj);

    el3c90xIntrSet (pDrvCtrl, EL_STAT_DOWN_COMPLETE);

    if (pDrvCtrl->txBlocked)
        {
        pDrvCtrl->txBlocked = FALSE;
        muxTxRestart (&pDrvCtrl->endObj);
        }
    }

/******************************************************************************
*
* el3c90xIntrErr - handle transmit errors
*
* This routine handles transmit errors
*
* RETURNS: N/A.
*/

LOCAL void el3c90xIntrErr
    (
    EL3C90X_DEVICE * 	pDrvCtrl, /* pointer to the device control structure */
    UINT8		txstat
    )
    {
    if (txstat & EL_TXSTATUS_UNDERRUN ||
        txstat & EL_TXSTATUS_JABBER ||
        txstat & EL_TXSTATUS_RECLAIM)
        {
        logMsg ("elPci%d: transmission error: %x\n",
                pDrvCtrl->unit, txstat,0,0,0,0);
        el3c90xCsrWriteWord (pDrvCtrl, EL_COMMAND, EL_CMD_TX_RESET, NONE);
        el3c90xWait(pDrvCtrl);

        if (pDrvCtrl->rxTxChain.pTxHead != NULL)
            {
            el3c90xCsrWriteLong (pDrvCtrl, EL_DOWNLIST_PTR,
                                 (UINT32) EL3C90X_CACHE_VIRT_TO_PHYS
                                 (pDrvCtrl->rxTxChain.pTxHead->pDesc),
                                 NONE);
            }
        /* Remember to set this for the first generation 3c90X chips. */

        el3c90xCsrWriteByte (pDrvCtrl, EL_TX_FREETHRESH,
                             (EL3C90X_BUFSIZ >> 8), NONE);

        if (pDrvCtrl->devType == EL_TYPE_905B)
            {
            el3c90xCsrWriteWord (pDrvCtrl, EL_COMMAND,
                                 (EL_CMD_SET_TX_RECLAIM |
                                  (EL3C90X_BUFSIZ >> 4)), NONE);
            }
        el3c90xCsrWriteWord (pDrvCtrl, EL_COMMAND, EL_CMD_TX_ENABLE, NONE);
        el3c90xCsrWriteWord (pDrvCtrl, EL_COMMAND, EL_CMD_DOWN_UNSTALL, NONE);
        }
    else
        {
        el3c90xCsrWriteWord (pDrvCtrl, EL_COMMAND, EL_CMD_TX_ENABLE, NONE);
        el3c90xCsrWriteWord (pDrvCtrl, EL_COMMAND, EL_CMD_DOWN_UNSTALL, NONE);
        }
#ifdef INCLUDE_RFC_1213

    /* Old RFC 1213 mib2 interface */

    END_ERR_ADD (&pDrvCtrl->endObj, MIB2_OUT_ERRS, +1);

#else
    /* New RFC 2233 mib2 interface */

    if (pDrvCtrl->endObj.pMib2Tbl != NULL)
        {
        pDrvCtrl->endObj.pMib2Tbl->m2CtrUpdateRtn(pDrvCtrl->endObj.pMib2Tbl,
                                                  M2_ctrId_ifOutErrors, 1);
        }
#endif /* INCLUDE_RFC_1213 */

    el3c90xIntrSet (pDrvCtrl, EL_STAT_TX_COMPLETE);
    }

/******************************************************************************
*
* el3c90xStatUpdate - handle the stats overflow interrupt 
*
* This routine handles the stats overflow interrupt 
*
* RETURNS: N/A.
*/

LOCAL void el3c90xStatUpdate
    (
    EL3C90X_DEVICE * pDrvCtrl	/* pointer to the device control structure */
    )
    {
    EL_STATS	elStats;
    UINT8 *	pChar;
    int		ix;

    bzero((char *)&elStats, sizeof(elStats));

    pChar = (UINT8 *)&elStats;

    /* Read all the stats registers. */

    for (ix = 0; ix < 16; ix++)
        *pChar++ = el3c90xCsrReadByte (pDrvCtrl, EL_W6_CARRIER_LOST + ix,
                                       EL_WIN_6);
    /*
     * Boomerang and cyclone chips have an extra stats counter
     * in window 4 (BadSSD). We have to read this too in order
     * to clear out all the stats registers and avoid a statsoflow
     * interrupt.
     */
    el3c90xCsrReadByte (pDrvCtrl, EL_W4_BADSSD, EL_WIN_4);
    return;
    }

/******************************************************************************
*
* el3c90xInt - interrupt handler for the NIC interrupt
*
* This routine is general interrupt handler.
*
* RETURNS: N/A.
*/

LOCAL void el3c90xInt
    (
    EL3C90X_DEVICE * pDrvCtrl	/* pointer to the device control structure */
    )
    {
    UINT16		status;
    UINT16		intClr = 0;
    UINT8		txstat;
    
    ENDLOGMSG (("Got an interrupt!\n", 1, 2, 3, 4, 5, 6));

    /* read interrupt status */

    status = el3c90xCsrReadWord (pDrvCtrl, EL_STATUS, NONE); 

    if ((status & EL_INTRS) == 0)
        return;

    if (status & EL_STAT_UP_COMPLETE)
        {
        if (netJobAdd ((FUNCPTR)el3c90xIntrRx, (int) pDrvCtrl,
                       0, 0, 0, 0) == OK)
            {
            intClr |= EL_STAT_UP_COMPLETE;
            }
        else
            logMsg ("xl: netJobAdd (el3c90xIntrRx) failed\n",
                    0, 0, 0, 0, 0, 0);
        el3c90xCsrWriteWord (pDrvCtrl, EL_COMMAND,
                             (EL_CMD_INTR_ACK | EL_STAT_UP_COMPLETE), NONE);
        }

    if (status & EL_STAT_DOWN_COMPLETE)
        {
        if (netJobAdd ((FUNCPTR)el3c90xIntrTx, (int) pDrvCtrl,
                       0, 0, 0, 0) == OK)
            {
            intClr |= EL_STAT_DOWN_COMPLETE;
            }
        else
            logMsg ("xl: netJobAdd (el3c90xIntrTx) failed\n",
                    0, 0, 0, 0, 0, 0);

        el3c90xCsrWriteWord (pDrvCtrl, EL_COMMAND,
                             (EL_CMD_INTR_ACK | EL_STAT_DOWN_COMPLETE), NONE);
        }

    if (status & EL_STAT_TX_COMPLETE)
        {
        txstat = el3c90xCsrReadByte (pDrvCtrl, EL_TX_STATUS, NONE);

        if (netJobAdd ((FUNCPTR)el3c90xIntrErr, (int) pDrvCtrl,
                       (int)txstat, 0, 0, 0) == OK)
            {
            intClr |= EL_STAT_TX_COMPLETE;
            }
        else
            logMsg ("xl: netJobAdd (el3c90xIntrErr) failed\n",
                    0, 0, 0, 0, 0, 0);
        /*
         * Write an arbitrary byte to the TX_STATUS register
         * to clear this interrupt/error and advance to the next.
         */
        el3c90xCsrWriteByte (pDrvCtrl, EL_TX_STATUS, 0x01, NONE);

        el3c90xCsrWriteWord (pDrvCtrl, EL_COMMAND,
                             (EL_CMD_INTR_ACK | EL_STAT_TX_COMPLETE), NONE);
        }

    if (status & EL_STAT_STATSOFLOW)
        {
        el3c90xStatUpdate(pDrvCtrl);
        }

    if (status & EL_STAT_ADFAIL)
        {
        el3c90xCsrWriteWord (pDrvCtrl, EL_COMMAND, EL_CMD_INTR_ENB, NONE);
        el3c90xReset (pDrvCtrl);

        if (netJobAdd ((FUNCPTR)el3c90xInit, (int) pDrvCtrl,
                       0, 0, 0, 0) != OK)
            logMsg ("xl: netJobAdd (el3c90xInit) failed\n",
                    0, 0, 0, 0, 0, 0);
        el3c90xCsrWriteWord (pDrvCtrl, EL_COMMAND,
                             (EL_CMD_INTR_ACK | EL_STAT_ADFAIL), NONE);

        el3c90xCsrWriteWord (pDrvCtrl, EL_STATUS,
                             (EL_CMD_INTR_ACK | EL_STAT_INTREQ |
                              EL_STAT_INTLATCH), NONE);
        return; 
        }

    /* acknowledge all interrupts */
    el3c90xCsrWriteWord (pDrvCtrl, EL_STATUS,
                         (EL_CMD_INTR_ACK | EL_STAT_INTREQ | EL_STAT_INTLATCH),
                         NONE);

    if (intClr)
        el3c90xIntrClr (pDrvCtrl, intClr);

    return;
    }

/*******************************************************************************
*
* el3c90xMiiSync - sync the PHYs
*
* This function syncs the PHYs by setting data bit and strobing the
* clock 32 times.
*
* RETURNS: N/A
*/

LOCAL void el3c90xMiiSync
    (
    EL3C90X_DEVICE * 	pDrvCtrl
    )
    {
    register int  	ix;

    EL_MII_SET (EL_MII_DIR | EL_MII_DATA);

    for (ix = 0; ix < 32; ix++)
        {
        EL_MII_SET (EL_MII_CLK);
        SYS_DELAY (1);
        EL_MII_CLR (EL_MII_CLK);
        SYS_DELAY (1);
	}

    return;
    }

/*******************************************************************************
*
* el3c90xMiiSend - clock a series of bits through the MII.
*
* This function clocks a series of bits through the MII.
*
* RETURNS: N/A
*/

LOCAL void el3c90xMiiSend
    (
    EL3C90X_DEVICE *	pDrvCtrl,
    UINT32		bits,
    int			cnt
    )
    {
    int			ix;

    EL_MII_CLR (EL_MII_CLK);

    for (ix = (0x1 << (cnt - 1)); ix; ix >>= 1)
        {
        if (bits & ix)
            {
            EL_MII_SET (EL_MII_DATA);
            }
        else
            {
            EL_MII_CLR (EL_MII_DATA);
            }

        SYS_DELAY (1);
        EL_MII_CLR (EL_MII_CLK);
        SYS_DELAY (1);
        EL_MII_SET (EL_MII_CLK);
	}
    }

/*******************************************************************************
*
* el3c90xMiiRegRead - read an PHY register through the MII.
*
* This function reads a PHY register through the MII.
*
* RETURNS: OK or ERROR.
*/

LOCAL int el3c90xMiiRegRead
    (
    EL3C90X_DEVICE *	pDrvCtrl,
    EL_MII_FRAME *	pMiiFrame
    )
	
    {
    int			ix;
    int			ack;

    /* set up frame for RX. */

    pMiiFrame->stDelim = EL_MII_STARTDELIM;
    pMiiFrame->opCode = EL_MII_READOP;
    pMiiFrame->turnAround = 0;
    pMiiFrame->data = 0;

    /* clear PHY_MGMT register */
    
    el3c90xCsrWriteWord (pDrvCtrl, EL_W4_PHY_MGMT, 0, EL_WIN_4);

    /* turn on data xmit.*/

    EL_MII_SET (EL_MII_DIR);

    el3c90xMiiSync (pDrvCtrl);

    /* Send command/address info. */

    el3c90xMiiSend (pDrvCtrl, pMiiFrame->stDelim, 2);
    el3c90xMiiSend (pDrvCtrl, pMiiFrame->opCode, 2);
    el3c90xMiiSend (pDrvCtrl, pMiiFrame->phyAddr, 5);
    el3c90xMiiSend (pDrvCtrl, pMiiFrame->regAddr, 5);

    /* idle bit */
    EL_MII_CLR ((EL_MII_CLK | EL_MII_DATA));
    SYS_DELAY (1);
    EL_MII_SET (EL_MII_CLK);
    SYS_DELAY (1);

    /* turn off xmit. */

    EL_MII_CLR (EL_MII_DIR);

    /* Check for ack */

    EL_MII_CLR (EL_MII_CLK);
    SYS_DELAY (1);
    EL_MII_SET (EL_MII_CLK);
    SYS_DELAY (1);

    ack = (el3c90xCsrReadWord (pDrvCtrl, EL_W4_PHY_MGMT, EL_WIN_4) &
           EL_MII_DATA);

    /*
     * Now try reading data bits. If the ack failed, we still
     * need to clock through 16 cycles to keep the PHY(s) in sync.
     */
    if (ack)
        {
        for(ix = 0; ix < 16; ix++)
            {
            EL_MII_CLR (EL_MII_CLK);
            SYS_DELAY (1);
            EL_MII_SET (EL_MII_CLK);
            SYS_DELAY (1);
            }
        goto fail;
	}

    for (ix = 0x8000; ix; ix >>= 1)
        {
        EL_MII_CLR (EL_MII_CLK);
        SYS_DELAY (1);
        if (!ack)
            {
            if (el3c90xCsrReadWord (pDrvCtrl, EL_W4_PHY_MGMT, EL_WIN_4) &
                EL_MII_DATA)
                pMiiFrame->data |= ix;
            SYS_DELAY (1);
            }
        EL_MII_SET (EL_MII_CLK);
        SYS_DELAY (1);
	}

    fail:
        {
        EL_MII_CLR (EL_MII_CLK);
        SYS_DELAY (1);
        EL_MII_SET (EL_MII_CLK);
        SYS_DELAY (1);
        }

    if (ack)
        return (ERROR);

    return (OK);
    }

/*******************************************************************************
*
* el3c90xMiiRegWrite - write to a PHY register through the MII.
*
* This function writes to a PHY register through the MII.
*
* RETURNS: OK or ERROR.
*/

LOCAL int el3c90xMiiRegWrite
    (
    EL3C90X_DEVICE *	pDrvCtrl,
    EL_MII_FRAME *	pMiiFrame
    )
    {
    /* set up frame for TX. */

    pMiiFrame->stDelim = EL_MII_STARTDELIM;
    pMiiFrame->opCode = EL_MII_WRITEOP;
    pMiiFrame->turnAround = EL_MII_TURNAROUND;
	
    /* turn on data output. */

    EL_MII_SET (EL_MII_DIR);

    el3c90xMiiSync (pDrvCtrl);

    el3c90xMiiSend (pDrvCtrl, pMiiFrame->stDelim, 2);
    el3c90xMiiSend (pDrvCtrl, pMiiFrame->opCode, 2);
    el3c90xMiiSend (pDrvCtrl, pMiiFrame->phyAddr, 5);
    el3c90xMiiSend (pDrvCtrl, pMiiFrame->regAddr, 5);
    el3c90xMiiSend (pDrvCtrl, pMiiFrame->turnAround, 2);
    el3c90xMiiSend (pDrvCtrl, pMiiFrame->data, 16);

    /* idle bit. */

    EL_MII_SET (EL_MII_CLK);
    SYS_DELAY (1);
    EL_MII_CLR (EL_MII_CLK);
    SYS_DELAY (1);

    /* Turn off xmit. */

    EL_MII_CLR (EL_MII_DIR);

    return (OK);
    }

/*******************************************************************************
*
* el3c90xPhyRegRead - read a PHY reg
*
* This function reads a PHY reg
*
* RETURNS: UINT16 value.
*/

LOCAL UINT16 el3c90xPhyRegRead
    (
    EL3C90X_DEVICE *	pDrvCtrl,
    int			reg
    )
    {
    EL_MII_FRAME	miiFrame;

    bzero ((char *)&miiFrame, sizeof(miiFrame));

    miiFrame.phyAddr = pDrvCtrl->devPhyAddr;
    miiFrame.regAddr = reg;
    el3c90xMiiRegRead (pDrvCtrl, &miiFrame);

    return(miiFrame.data);
    }

/*******************************************************************************
*
* el3c90xPhyRegWrite - write a PHY reg
*
* This function writes a PHY reg
*
* RETURNS: N/A
*/

LOCAL void el3c90xPhyRegWrite
    (
    EL3C90X_DEVICE *	pDrvCtrl,
    int			reg,
    int			data
    )
    {
    EL_MII_FRAME	frame;

    bzero((char *)&frame, sizeof(frame));

    frame.phyAddr = pDrvCtrl->devPhyAddr;
    frame.regAddr = reg;
    frame.data = data;

    el3c90xMiiRegWrite(pDrvCtrl, &frame);

    return;
    }

/*******************************************************************************
*
* el3c90xAutoNegTx - initiate an autonegotiation session.
*
* This function initiates an autonegotiation session.
*
* RETURNS: N/A
*/

LOCAL void el3c90xAutoNegTx
    (
    EL3C90X_DEVICE * pDrvCtrl
    )
    {
    UINT16		phyStatus;

    el3c90xPhyRegWrite (pDrvCtrl, PHY_BMCR, PHY_BMCR_RESET);

    SYS_DELAY (500);

    while (el3c90xPhyRegRead (pDrvCtrl, EL_PHY_GENCTL)
           & PHY_BMCR_RESET);

    phyStatus = el3c90xPhyRegRead (pDrvCtrl, PHY_BMCR);
    phyStatus |= (PHY_BMCR_AUTONEGENBL | PHY_BMCR_AUTONEGRSTR);

    el3c90xPhyRegWrite (pDrvCtrl, PHY_BMCR, phyStatus);

    return;
    }

/*******************************************************************************
*
* el3c90xMiiAutoNeg - invoke autonegotiation on a PHY
*
* This function invokes autonegotiation on a PHY. Also used with the
* 3Com internal autoneg logic which is mapped onto the MII.
*
* RETURNS: N/A
*/

LOCAL void el3c90xMiiAutoNeg
    (
    EL3C90X_DEVICE *	pDrvCtrl
    )
    {
    UINT16		phyStatus = 0;
    UINT16		media;
    UINT16		advert;
    UINT16		ability;

    /*
     * The 100baseT4 PHY on the 3c905-T4 has the 'autoneg supported'
     * bit cleared in the status register, but has the 'autoneg enabled'
     * bit set in the control register. This is a contradiction, and
     * I'm not sure how to handle it. If you want to force an attempt
     * to autoneg for 100baseT4 PHYs, #define FORCE_AUTONEG_TFOUR
     * and see what happens.
     */
#ifndef FORCE_AUTONEG_TFOUR
    /*
     * First, see if autoneg is supported. If not, there's
     * no point in continuing.
     */
    phyStatus = el3c90xPhyRegRead (pDrvCtrl, PHY_BMSR);

    if (!(phyStatus & PHY_BMSR_CANAUTONEG))
        {
        DRV_LOG (DRV_DEBUG_LOAD,"elPci%d: autonegotiation not supported\n",
                 pDrvCtrl->unit, 2, 3, 4, 5, 6);

        media = el3c90xPhyRegRead (pDrvCtrl, PHY_BMCR);
        media &= ~PHY_BMCR_SPEEDSEL;
        media &= ~PHY_BMCR_DUPLEX;

        el3c90xPhyRegWrite (pDrvCtrl, PHY_BMCR, media);
        el3c90xCsrWriteByte (pDrvCtrl, EL_W3_MAC_CTRL,
                             (el3c90xCsrReadByte (pDrvCtrl, EL_W3_MAC_CTRL,
                                                  EL_WIN_3) &
                              ~EL_MACCTRL_DUPLEX), EL_WIN_3);
        return;
	}
#endif

    el3c90xAutoNegTx (pDrvCtrl);

    taskDelay (sysClkRateGet() * 2);

    if (el3c90xPhyRegRead (pDrvCtrl, PHY_BMSR) & PHY_BMSR_AUTONEGCOMP)
        {
        DRV_LOG (DRV_DEBUG_LOAD,"elPci%d: autoneg complete, ",
                 pDrvCtrl->unit, 2, 3, 4, 5, 6);
        phyStatus = el3c90xPhyRegRead (pDrvCtrl, PHY_BMSR);
	}
    else
        {
        DRV_LOG (DRV_DEBUG_LOAD,"elPci%d: autoneg not complete, ",
                 pDrvCtrl->unit, 2, 3, 4, 5, 6);
	}

    media = el3c90xPhyRegRead (pDrvCtrl, PHY_BMCR);

    /* Link is good. Report modes and set duplex mode. */
    
    if (el3c90xPhyRegRead (pDrvCtrl, PHY_BMSR) & PHY_BMSR_LINKSTAT)
        {
        DRV_LOG (DRV_DEBUG_LOAD,"link status good ", 1, 2, 3, 4, 5, 6);

        advert = el3c90xPhyRegRead (pDrvCtrl, EL_PHY_ANAR);
        ability = el3c90xPhyRegRead (pDrvCtrl, EL_PHY_LPAR);

        if (advert & PHY_ANAR_100BT4 && ability & PHY_ANAR_100BT4)
            {
            media |= PHY_BMCR_SPEEDSEL;
            media &= ~PHY_BMCR_DUPLEX;
            DRV_LOG (DRV_DEBUG_LOAD,"(100baseT4)\n", 1, 2, 3, 4, 5, 6);
            }
        else if (advert & PHY_ANAR_100BTXFULL && ability & PHY_ANAR_100BTXFULL)
            {
            media |= PHY_BMCR_SPEEDSEL;
            media |= PHY_BMCR_DUPLEX;
            DRV_LOG (DRV_DEBUG_LOAD,"(full-duplex, 100Mbps)\n",
                     1, 2, 3, 4, 5, 6);
            }
        else if (advert & PHY_ANAR_100BTXHALF && ability & PHY_ANAR_100BTXHALF)
            {
            media |= PHY_BMCR_SPEEDSEL;
            media &= ~PHY_BMCR_DUPLEX;
            DRV_LOG (DRV_DEBUG_LOAD,"(half-duplex, 100Mbps)\n",
                     1, 2, 3, 4, 5, 6);
            }
        else if (advert & PHY_ANAR_10BTFULL && ability & PHY_ANAR_10BTFULL)
            {
            media &= ~PHY_BMCR_SPEEDSEL;
            media |= PHY_BMCR_DUPLEX;
            DRV_LOG (DRV_DEBUG_LOAD,"(full-duplex, 10Mbps)\n",
                     1, 2, 3, 4, 5, 6);
            }
        else if (advert & PHY_ANAR_10BTHALF && ability & PHY_ANAR_10BTHALF)
            {
            media &= ~PHY_BMCR_SPEEDSEL;
            media &= ~PHY_BMCR_DUPLEX;
            DRV_LOG (DRV_DEBUG_LOAD,"(half-duplex, 10Mbps)\n",
                     1, 2, 3, 4, 5, 6);
            }

        /* Set ASIC's duplex mode to match the PHY. */
        
        if (media & PHY_BMCR_DUPLEX)
            el3c90xCsrWriteByte (pDrvCtrl, EL_W3_MAC_CTRL, EL_MACCTRL_DUPLEX,
                                 EL_WIN_3);
        else
            el3c90xCsrWriteByte (pDrvCtrl, EL_W3_MAC_CTRL,
                                 (el3c90xCsrReadByte (pDrvCtrl, EL_W3_MAC_CTRL,
                                                      EL_WIN_3) &
                                  ~EL_MACCTRL_DUPLEX), EL_WIN_3);
        el3c90xPhyRegWrite (pDrvCtrl, PHY_BMCR, media);
	}
    else
        {
        DRV_LOG (DRV_DEBUG_LOAD,"no carrier (forcing half-duplex,"
                 "10Mbps)\n", 1, 2, 3, 4, 5, 6);
        media &= ~PHY_BMCR_SPEEDSEL;
        media &= ~PHY_BMCR_DUPLEX;
        el3c90xPhyRegWrite (pDrvCtrl, PHY_BMCR, media);
        el3c90xCsrWriteByte (pDrvCtrl, EL_W3_MAC_CTRL,
                             (el3c90xCsrReadByte (pDrvCtrl, EL_W3_MAC_CTRL,
                                                  EL_WIN_3) &
                              ~EL_MACCTRL_DUPLEX), EL_WIN_3);
        }
    return;
    }

/*******************************************************************************
*
* el3c90xMiiModeGet - get the MII mode
*
* This function gets the MII mode and returns the speed of the physical
* interface. The default speed is 10Mbits/sec.
*
* RETURNS: speed of the physical interface.
*/

LOCAL int el3c90xMiiModeGet
    (
    EL3C90X_DEVICE * pDrvCtrl
    )
    {
    UINT16		bmsr;
    int			speed = 10000000;

    bmsr = el3c90xPhyRegRead (pDrvCtrl, PHY_BMSR);

    DRV_LOG (DRV_DEBUG_LOAD,"elPci%d: PHY status word: %x\n", pDrvCtrl->unit,
             bmsr, 3, 4, 5, 6);

    if (bmsr & PHY_BMSR_10BTHALF)
        {
        DRV_LOG (DRV_DEBUG_LOAD,"elPci%d: 10Mbps half-duplex mode supported\n",
                 pDrvCtrl->unit, 2, 3, 4, 5, 6);
	}

    if (bmsr & PHY_BMSR_10BTFULL)
        {
        DRV_LOG (DRV_DEBUG_LOAD,"elPci%d: 10Mbps full-duplex mode supported\n",
                 pDrvCtrl->unit, 2, 3, 4, 5, 6);
	}

    if (bmsr & PHY_BMSR_100BTXHALF)
        {
        DRV_LOG (DRV_DEBUG_LOAD,"elPci%d:100Mbps half-duplex mode supported\n",
                 pDrvCtrl->unit, 2, 3, 4, 5, 6);
        speed = 100000000;
	}

    if (bmsr & PHY_BMSR_100BTXFULL)
        {
        DRV_LOG (DRV_DEBUG_LOAD,"elPci%d:100Mbps full-duplex mode supported\n",
                 pDrvCtrl->unit, 2, 3, 4, 5, 6);
        speed = 100000000;
	}

    /* Some also support 100BaseT4 */
    if (bmsr & PHY_BMSR_100BT4)
        {
        DRV_LOG (DRV_DEBUG_LOAD,"elPci%d:100baseT4 mode supported\n",
                 pDrvCtrl->unit, 2, 3, 4, 5, 6);
        speed = 100000000;
#ifdef FORCE_AUTONEG_TFOUR
        DRV_LOG (DRV_DEBUG_LOAD,"elPci%d:force on autoneg support for BT4\n",
                 pDrvCtrl->unit, 2, 3, 4, 5, 6);
#endif
	}

    if (bmsr & PHY_BMSR_CANAUTONEG)
        {
        DRV_LOG (DRV_DEBUG_LOAD,"elPci%d: autoneg supported\n",
                 pDrvCtrl->unit, 2, 3, 4, 5, 6);
        speed = 10000000;
	}
    
    return (speed);
    }

/*******************************************************************************
*
* el3c90xMediaSet - set the media access control
*
* This function sets the MAC register.
*
* RETURNS: N/A
*/

LOCAL void el3c90xMediaSet
    (
    EL3C90X_DEVICE * pDrvCtrl,
    int			media
    )
    {
    UINT32		icfg;
    UINT16		mediastat;

    DRV_LOG (DRV_DEBUG_LOAD,"elPci%d: selecting ", pDrvCtrl->unit,
             2, 3, 4, 5, 6);

    mediastat = el3c90xCsrReadWord (pDrvCtrl, EL_W4_MEDIA_STATUS, EL_WIN_4);
    
    icfg = el3c90xCsrReadLong (pDrvCtrl, EL_W3_INTERNAL_CFG, EL_WIN_3);

    if (pDrvCtrl->devMedia & EL_MEDIAOPT_BT)
        {
        if (IFM_SUBTYPE (media) == IFM_10_T)
            {
            DRV_LOG (DRV_DEBUG_LOAD,"10baseT transceiver, ",
                     1, 2, 3, 4, 5, 6);
            pDrvCtrl->xCvr = EL_XCVR_10BT;
            icfg &= ~EL_ICFG_CONNECTOR_MASK;
            icfg |= (EL_XCVR_10BT << EL_ICFG_CONNECTOR_BITS);
            mediastat |= EL_MEDIASTAT_LINKBEAT|
                EL_MEDIASTAT_JABGUARD;
            mediastat &= ~EL_MEDIASTAT_SQEENB;
            }
	}

    if (pDrvCtrl->devMedia & EL_MEDIAOPT_BFX)
        {
        if (IFM_SUBTYPE (media) == IFM_100_FX)
            {
            DRV_LOG (DRV_DEBUG_LOAD,"100baseFX port, ", 1, 2, 3, 4, 5, 6);
            pDrvCtrl->xCvr = EL_XCVR_100BFX;
            icfg &= ~EL_ICFG_CONNECTOR_MASK;
            icfg |= (EL_XCVR_100BFX << EL_ICFG_CONNECTOR_BITS);
            mediastat |= EL_MEDIASTAT_LINKBEAT;
            mediastat &= ~EL_MEDIASTAT_SQEENB;
            }
	}

    if (pDrvCtrl->devMedia & EL_MEDIAOPT_AUI)
        {
        if (IFM_SUBTYPE (media) == IFM_10_5)
            {
            DRV_LOG (DRV_DEBUG_LOAD,"AUI port, ", 1, 2, 3, 4, 5, 6);
            pDrvCtrl->xCvr = EL_XCVR_AUI;
            icfg &= ~EL_ICFG_CONNECTOR_MASK;
            icfg |= (EL_XCVR_AUI << EL_ICFG_CONNECTOR_BITS);
            mediastat &= ~(EL_MEDIASTAT_LINKBEAT|
                           EL_MEDIASTAT_JABGUARD);
            mediastat |= ~EL_MEDIASTAT_SQEENB;
            }
	}

    if (pDrvCtrl->devMedia & EL_MEDIAOPT_BNC)
        {
        if (IFM_SUBTYPE (media) == IFM_10_2)
            {
            DRV_LOG (DRV_DEBUG_LOAD,"BNC port, ", 1, 2, 3, 4, 5, 6);
            pDrvCtrl->xCvr = EL_XCVR_COAX;
            icfg &= ~EL_ICFG_CONNECTOR_MASK;
            icfg |= (EL_XCVR_COAX << EL_ICFG_CONNECTOR_BITS);
            mediastat &= ~(EL_MEDIASTAT_LINKBEAT|
                           EL_MEDIASTAT_JABGUARD|
                           EL_MEDIASTAT_SQEENB);
            }
	}

    if ((media & IFM_GMASK) == IFM_FDX ||
        IFM_SUBTYPE (media) == IFM_100_FX)
        {
        DRV_LOG (DRV_DEBUG_LOAD,"full duplex\n", 1, 2, 3, 4, 5, 6);
        el3c90xCsrWriteByte (pDrvCtrl, EL_W3_MAC_CTRL, EL_MACCTRL_DUPLEX,
                             EL_WIN_3);
	}
    else
        {
        DRV_LOG (DRV_DEBUG_LOAD,"half duplex\n", 1, 2, 3, 4, 5, 6);
        el3c90xCsrWriteByte (pDrvCtrl, EL_W3_MAC_CTRL,
                             (el3c90xCsrReadByte (pDrvCtrl, EL_W3_MAC_CTRL,
                                                  EL_WIN_3) &
                              ~EL_MACCTRL_DUPLEX), EL_WIN_3);
	}

    if (IFM_SUBTYPE(media) == IFM_10_2)
        el3c90xCsrWriteWord (pDrvCtrl, EL_COMMAND, EL_CMD_COAX_START, NONE);
    else
        el3c90xCsrWriteWord (pDrvCtrl, EL_COMMAND, EL_CMD_COAX_STOP, NONE);

    el3c90xCsrWriteLong (pDrvCtrl, EL_W3_INTERNAL_CFG, icfg, EL_WIN_3);
    el3c90xCsrWriteWord (pDrvCtrl, EL_W4_MEDIA_STATUS, mediastat, EL_WIN_4);
    SYS_DELAY(800);

    return;
    }

/*******************************************************************************
*
* el3c90xMediaCheck - check the media on the adapter card.
*
* This function checks for the media on the adapter card.
* This routine is a kludge to work around possible hardware faults
* or manufacturing defects that can cause the media options register
* (or reset options register, as it's called for the first generation
* 3cx90x adapters) to return an incorrect result.
* one Dell Latitude laptop docking station with an integrated 3c905-TX
* doesn't have any of the 'mediaopt' bits set. This causes the
* the attach routine to fail because it doesn't know what media
* to look for. If we find ourselves in this predicament, this routine
* will try to guess the media options values and warn the user of a
* possible manufacturing defect with his adapter/system/whatever.
*
* RETURNS: N/A
*/

LOCAL void el3c90xMediaCheck
    (
    EL3C90X_DEVICE *	pDrvCtrl
    )
    {
    UINT16		devid;

    /*
     * if some of the media options bits are set, assume they are
     * correct. If not, try to figure it out down below.
     */

    if (pDrvCtrl->devMedia & (EL_MEDIAOPT_MASK & ~EL_MEDIAOPT_VCO))
        {
        /*
         * Check the XCVR value. If it's not in the normal range
         * of values, we need to fake it up here.
         */
        if (pDrvCtrl->xCvr <= EL_XCVR_AUTO)
            return;
        else
            {
            DRV_LOG (DRV_DEBUG_LOAD,"elPci%d: bogus xcvr value "
                     "in EEPROM (%x)\n", pDrvCtrl->unit, pDrvCtrl->xCvr,
                     3, 4, 5, 6);
            DRV_LOG (DRV_DEBUG_LOAD,"elPci%d: choosing new default based "
                     "on card type\n", pDrvCtrl->unit, 2, 3, 4, 5, 6);
            }
	}
    else
        {
        DRV_LOG (DRV_DEBUG_LOAD,"elPci%d: WARNING: no media options bits set"
                 "in the media options register!!\n", pDrvCtrl->unit,
                 2, 3, 4, 5, 6);
            
        DRV_LOG (DRV_DEBUG_LOAD,"elPci%d: this could be a manufacturing"
                 "defect in your adapter or system\n",
                 pDrvCtrl->unit, 2, 3, 4, 5, 6);

        DRV_LOG (DRV_DEBUG_LOAD,"elPci%d: attempting to guess media type; you "
                 "should probably consult your vendor\n", pDrvCtrl->unit,
                 2, 3, 4, 5, 6);
	}

    /*
     * Read the device ID from the EEPROM.
     * This is what's loaded into the PCI device ID register, so it has
     * to be correct otherwise we wouldn't have gotten this far.
     */
    
    el3c90xEprmRead (pDrvCtrl, (caddr_t)&devid, EL_EE_PRODID, 1, 0);

    switch (devid) 
        {
	case TC_DEVICEID_BOOMERANG_10BT:	/* 3c900-TP */
	case TC_DEVICEID_CYCLONE_10BT:		/* 3c900B-TP */
            pDrvCtrl->devMedia = EL_MEDIAOPT_BT;
            pDrvCtrl->xCvr = EL_XCVR_10BT;
            DRV_LOG (DRV_DEBUG_LOAD,"elPci%d: guessing 10BaseT transceiver\n",
                 pDrvCtrl->unit, 2, 3, 4, 5, 6);
            break;
            
	case TC_DEVICEID_BOOMERANG_10BT_COMBO:	/* 3c900-COMBO */
	case TC_DEVICEID_CYCLONE_10BT_COMBO:	/* 3c900B-COMBO */
            pDrvCtrl->devMedia = (EL_MEDIAOPT_BT |EL_MEDIAOPT_BNC |
                                  EL_MEDIAOPT_AUI);
            pDrvCtrl->xCvr = EL_XCVR_10BT;
            DRV_LOG (DRV_DEBUG_LOAD,"elPci%d: guessing COMBO (AUI/BNC/TP)\n",
                     pDrvCtrl->unit, 2, 3, 4, 5, 6);
            break;
            
	case TC_DEVICEID_BOOMERANG_10_100BT:	/* 3c905-TX */
            pDrvCtrl->devMedia = EL_MEDIAOPT_MII;
            pDrvCtrl->xCvr = EL_XCVR_MII;
            DRV_LOG (DRV_DEBUG_LOAD,"elPci%d: guessing MII\n",
                     pDrvCtrl->unit, 2, 3, 4, 5, 6);
            break;
            
	case TC_DEVICEID_BOOMERANG_100BT4:	/* 3c905-T4 */
	case TC_DEVICEID_CYCLONE_10_100BT4:	/* 3c905B-T4 */
            pDrvCtrl->devMedia = EL_MEDIAOPT_BT4;
            pDrvCtrl->xCvr = EL_XCVR_MII;
            DRV_LOG (DRV_DEBUG_LOAD,"elPci%d: guessing 100BaseT4/MII\n",
                     pDrvCtrl->unit, 2, 3, 4, 5, 6);
            break;
            
	case TC_DEVICEID_CYCLONE_10_100BT:	/* 3c905B-TX */
	case TC_DEVICEID_CYCLONE_10_100BT_SERV:	/* 3c980-TX */
            pDrvCtrl->devMedia = EL_MEDIAOPT_BTX;
            pDrvCtrl->xCvr = EL_XCVR_AUTO;
            DRV_LOG (DRV_DEBUG_LOAD,"elPci%d: guessing 10/100 internal\n",
                     pDrvCtrl->unit, 2, 3, 4, 5, 6);
            break;

	default:
            DRV_LOG (DRV_DEBUG_LOAD,"elPci%d: unknown device ID: %x -- "
                   "defaulting to 10baseT\n", pDrvCtrl->unit, devid,
                       3, 4, 5, 6);
            pDrvCtrl->devMedia = EL_MEDIAOPT_BT;
            break;
	}
    return;
    }

/*******************************************************************************
*
* el3c90xMediaConfig - configure the media and do auto negotiation
*
* This function configures the media and does the auto negotioation.
*
* RETURNS: speed or 0
*
*/

LOCAL int el3c90xMediaConfig
    (
    EL3C90X_DEVICE * 	pDrvCtrl	/* device control structure */
    )
    {
    EL_DEV_TYPE *	pDevType;
    UINT32		icfg;
    int			media = IFM_ETHER | IFM_100_TX |IFM_FDX;
    UINT16		phyVendorId;
    UINT16		phyDevId;
    UINT16		phyStatus;
    int			speed = 10000000;
    int			ix;

    /*
     * now we have to see what sort of media we have.
     * This includes probing for an MII interace and a
     * possible PHY.
     */
    pDrvCtrl->devMedia = el3c90xCsrReadWord (pDrvCtrl, EL_W3_MEDIA_OPT,
                                             EL_WIN_3);

    DRV_LOG (DRV_DEBUG_LOAD,"elPci%d: media options word: %x\n",
             pDrvCtrl->unit,
             pDrvCtrl->devMedia, 3, 4, 5, 6);

    el3c90xEprmRead (pDrvCtrl, (char *)&pDrvCtrl->xCvr, EL_EE_ICFG_0, 2, 0);

    pDrvCtrl->xCvr &= EL_ICFG_CONNECTOR_MASK;
    pDrvCtrl->xCvr >>= EL_ICFG_CONNECTOR_BITS;

    el3c90xMediaCheck (pDrvCtrl);

    if (pDrvCtrl->devMedia & EL_MEDIAOPT_MII ||
        pDrvCtrl->devMedia & EL_MEDIAOPT_BTX ||
        pDrvCtrl->devMedia & EL_MEDIAOPT_BT4)
        {
        /*
         * in theory we shouldn't need this, but... if this
         * card supports an MII, either an external one or
         * an internal fake one, select it in the internal
         * config register before trying to probe it.
         */

        icfg = el3c90xCsrReadLong (pDrvCtrl, EL_W3_INTERNAL_CFG, EL_WIN_3);
        icfg &= ~EL_ICFG_CONNECTOR_MASK;

        if (pDrvCtrl->devMedia & EL_MEDIAOPT_MII ||
            pDrvCtrl->devMedia & EL_MEDIAOPT_BT4)
            icfg |= (EL_XCVR_MII << EL_ICFG_CONNECTOR_BITS);
        
        if (pDrvCtrl->devMedia & EL_MEDIAOPT_BTX)
            icfg |= (EL_XCVR_AUTO << EL_ICFG_CONNECTOR_BITS);
        
        if (pDrvCtrl->devMedia & EL_MEDIAOPT_BFX)
            icfg |= (EL_XCVR_100BFX << EL_ICFG_CONNECTOR_BITS);
        
        el3c90xCsrWriteLong (pDrvCtrl, EL_W3_INTERNAL_CFG, icfg, EL_WIN_3);

        DRV_LOG (DRV_DEBUG_LOAD,"elPci%d: probing for a PHY\n",
                 pDrvCtrl->unit, 2, 3, 4, 5, 6);

        for (ix = EL_PHYADDR_MIN; ix < EL_PHYADDR_MAX + 1; ix++)
            {
            DRV_LOG (DRV_DEBUG_LOAD,"elPci%d: checking address: %d\n",
                     pDrvCtrl->unit, ix, 3, 4, 5, 6);
            pDrvCtrl->devPhyAddr = ix;
            
            el3c90xPhyRegWrite (pDrvCtrl, EL_PHY_GENCTL, PHY_BMCR_RESET);

            SYS_DELAY(500);

            while (el3c90xPhyRegRead (pDrvCtrl, EL_PHY_GENCTL)
                   & PHY_BMCR_RESET);
            if ((phyStatus = el3c90xPhyRegRead (pDrvCtrl, EL_PHY_GENSTS)))
                break;
            }

        if (phyStatus)
            {
            phyVendorId = el3c90xPhyRegRead (pDrvCtrl, EL_PHY_VENID);
            phyDevId = el3c90xPhyRegRead (pDrvCtrl, EL_PHY_DEVID);

            DRV_LOG (DRV_DEBUG_LOAD,"elPci%d: found PHY at address %d, ",
                     pDrvCtrl->unit, pDrvCtrl->devPhyAddr, 3, 4, 5, 6);

            DRV_LOG (DRV_DEBUG_LOAD,"vendor id: %x device id: %x\n",
                     phyVendorId, phyDevId, 3, 4, 5, 6);

            pDevType = el3c90xPhyTbl;

            while (pDevType->vendorId)
                {
                if (phyVendorId == pDevType->vendorId &&
                    (phyDevId | 0x000F) == pDevType->deviceId)
                    {
                    pDrvCtrl->pPhyDevType = pDevType;
                    break;
                    }
                pDevType++;
                }
            
            if (pDrvCtrl->pPhyDevType == NULL)
                pDrvCtrl->pPhyDevType = &el3c90xPhyTbl [PHY_UNKNOWN];

            DRV_LOG (DRV_DEBUG_LOAD,"elPci%d: PHY type: %s\n",
                     pDrvCtrl->unit, pDrvCtrl->pPhyDevType->devName,
                     3, 4, 5, 6);
            }
        else
            DRV_LOG (DRV_DEBUG_LOAD,"elPci%d: MII without any phy!\n",
                     pDrvCtrl->unit, 2, 3, 4, 5, 6);
	}

    if (pDrvCtrl->devMedia & EL_MEDIAOPT_BT)
        DRV_LOG (DRV_DEBUG_LOAD,"elPci%d: found 10baseT\n",
                 pDrvCtrl->unit, 2, 3, 4, 5, 6);

    if (pDrvCtrl->devMedia & EL_MEDIAOPT_AUI)
        DRV_LOG (DRV_DEBUG_LOAD,"elPci%d: found AUI\n",
                 pDrvCtrl->unit, 2, 3, 4, 5, 6);

    if (pDrvCtrl->devMedia & EL_MEDIAOPT_BNC)
        DRV_LOG (DRV_DEBUG_LOAD,"elPci%d: found BNC\n",
                 pDrvCtrl->unit, 2, 3, 4, 5, 6);
    
    if (pDrvCtrl->devMedia & EL_MEDIAOPT_BTX)
        DRV_LOG (DRV_DEBUG_LOAD,"elPci%d: found 100baseTX\n",
                 pDrvCtrl->unit, 2, 3, 4, 5, 6);
            
    if (pDrvCtrl->devMedia & EL_MEDIAOPT_BFX)
        DRV_LOG (DRV_DEBUG_LOAD,"elPci%d: found 100baseFX\n",
                 pDrvCtrl->unit, 2, 3, 4, 5, 6);

    if ((pDrvCtrl->devMedia & EL_MEDIAOPT_BTX) ||
        (pDrvCtrl->devMedia & EL_MEDIAOPT_BFX))
        speed = 100000000;

    /* if there's an MII, we have to probe its modes separately. */

    if (pDrvCtrl->devMedia & EL_MEDIAOPT_MII ||
        pDrvCtrl->devMedia & EL_MEDIAOPT_BT4)
        {
        DRV_LOG (DRV_DEBUG_LOAD,"elPci%d: found MII\n",
                 pDrvCtrl->unit, 2, 3, 4, 5, 6);
        speed = el3c90xMiiModeGet (pDrvCtrl);
	}

    /* Choose a default media. */

    switch (pDrvCtrl->xCvr)
        {
	case EL_XCVR_10BT:
            media = IFM_ETHER | IFM_10_T;
            el3c90xMediaSet (pDrvCtrl, media);
            break;
            
	case EL_XCVR_AUI:
            media = IFM_ETHER | IFM_10_5;
            el3c90xMediaSet (pDrvCtrl, media);
            break;
            
	case EL_XCVR_COAX:
            media = IFM_ETHER | IFM_10_2;
            el3c90xMediaSet (pDrvCtrl, media);
            break;
            
	case EL_XCVR_AUTO:
            el3c90xMiiAutoNeg (pDrvCtrl);
            break;
            
	case EL_XCVR_100BTX:
	case EL_XCVR_MII:
            el3c90xMiiAutoNeg (pDrvCtrl);
            break;
            
	case EL_XCVR_100BFX:
            media = IFM_ETHER | IFM_100_FX;
            el3c90xMediaSet (pDrvCtrl, media);
            break;

	default:
            DRV_LOG (DRV_DEBUG_LOAD,"elPci%d: unknown XCVR type: %d\n",
                     pDrvCtrl->unit, pDrvCtrl->xCvr, 3, 4, 5, 6);
            /*
             * This will probably be wrong, but it prevents
             * the ifmedia code from panicking.
             */
            media = IFM_ETHER | IFM_10_T;
            break;
	}

    return (speed);
    }

/*******************************************************************************
*
* el3c90xEprmWait - wait for eeprom to get ready 
*
* This function waits for eeprom to get ready 
* EEPROM is slow so give it time to come ready after issuing it a command
* The standard eeprom wait period is 162 usecs.
*
* RETURNS: OK or ERROR.
*/

LOCAL int el3c90xEprmWait
    (
    EL3C90X_DEVICE * 	pDrvCtrl	/* device control structure */
    )
    {
    int			ix;

    for (ix = 0; ix < EL_EE_RETRY_CNT; ix++)
        {
        if (el3c90xCsrReadWord (pDrvCtrl, EL_W0_EE_CMD, EL_WIN_0) & EL_EE_BUSY)
            {
            SYS_DELAY (EL_EE_WAIT);
            }
        else
            break;
	}

    if (ix == EL_EE_RETRY_CNT)
        {
        DRV_LOG (DRV_DEBUG_LOAD, "elPci%d: eeprom failed to come ready\n",
                 pDrvCtrl->unit, 2, 3, 4, 5, 6);
        return (ERROR);
	}

    return (OK);
    }

/*******************************************************************************
*
* el3c90xEprmRead - read a sequence of words from the eeprom 
*
* This function reads a sequence of words from the eeprom.
*
* NOTE:
* The ethernet address stored in the EEPROM is in the network byte order.
*
* RETURNS: OK or ERROR.
*/

LOCAL int el3c90xEprmRead
    (
    EL3C90X_DEVICE * 	pDrvCtrl,	/* device control structure */
    char *		pDest,		/* pointer destination */
    int			off,		/* offset */
    int			cnt,		/* number of words to read */
    int			swap		/* swap or not */
    )
    {
    int			ix;
    UINT16		word = 0;
    UINT16 *		pWord;

    if (el3c90xEprmWait (pDrvCtrl) != OK)
        return (ERROR);

    for (ix = 0; ix < cnt; ix++)
        {
        el3c90xCsrWriteWord (pDrvCtrl, EL_W0_EE_CMD, EL_EE_READ | (off + ix),
                             EL_WIN_0);
        if (el3c90xEprmWait (pDrvCtrl) != OK)
            return (ERROR);

        word = el3c90xCsrReadWord (pDrvCtrl, EL_W0_EE_DATA, EL_WIN_0);
        pWord = (UINT16 *)(pDest + (ix * 2));

        if (swap)
            *pWord = ntohs (word);
        else
            *pWord = PCI_WORD_SWAP(word);
	}

    return (OK);
    }

/*******************************************************************************
*
* el3c90xWait - wait for the command completion
*
* This function waits for the command completion
* It is possible the chip can wedge and the 'command in progress' bit may
* never clear. Hence, we wait only a finite amount of time to avoid getting
* caught in an infinite loop. Normally this delay routine would be a macro,
* but it isn't called during normal operation so we can afford to make it a
* function.
*
* RETURNS: N/A
*/

LOCAL void el3c90xWait
    (
    EL3C90X_DEVICE * 	pDrvCtrl	/* device control structure */
    )
    {
    register int	ix;

    for (ix = 0; ix < EL_TIMEOUT; ix++)
        {
        if (!(el3c90xCsrReadWord (pDrvCtrl, EL_STATUS, NONE) &
              EL_STAT_CMDBUSY))
            break;
	}

    if (ix == EL_TIMEOUT)
        DRV_LOG (DRV_DEBUG_ALL, "elPci%d: command never completed!\n",
                 pDrvCtrl->unit, 2, 3, 4, 5, 6);
    return;
    }

/******************************************************************************
*
* el3c90xIntrSet - set the appropriate interrupt
*
* This routine set the interrupt given in the <flag>
*
* RETURNS: N/A.
*/

LOCAL void el3c90xIntrSet
    (
    EL3C90X_DEVICE * pDrvCtrl,	/* pointer to the device control structure */
    UINT16	     flag
    )
    {
    USHORT		intMask;
    int			intLevel;

    intLevel = intLock ();

    /* set the window */
    SYS_OUT_SHORT (pDrvCtrl, (pDrvCtrl->devAdrs + (UINT32)EL_COMMAND),
                   (EL_CMD_WINSEL | EL_WIN_5));
    
    SYS_IN_SHORT (pDrvCtrl, (pDrvCtrl->devAdrs + (UINT32)EL_W5_STAT_ENB),
                  intMask);

    intMask |= flag;

    /* reenable relevant interrupt conditions */
    SYS_OUT_SHORT (pDrvCtrl, (pDrvCtrl->devAdrs + (UINT32)EL_COMMAND),
                   (EL_CMD_STAT_ENB | intMask));

    intUnlock (intLevel);
    }

/******************************************************************************
*
* el3c90xIntrClr - clear the appropriate interrupt
*
* This routine set the interrupt given in the <flag>
*
* RETURNS: N/A.
*/

LOCAL void el3c90xIntrClr
    (
    EL3C90X_DEVICE * pDrvCtrl,	/* pointer to the device control structure */
    UINT16	     flag
    )
    {
    USHORT		intMask;

    int			intLevel;

    intLevel = intLock ();

    /* set the window */
    SYS_OUT_SHORT (pDrvCtrl, (pDrvCtrl->devAdrs + (UINT32)EL_COMMAND),
                   (EL_CMD_WINSEL | EL_WIN_5));
    
    SYS_IN_SHORT (pDrvCtrl, (pDrvCtrl->devAdrs + (UINT32)EL_W5_STAT_ENB),
                  intMask);

    intMask &= ~(flag);

    /* reenable relevant interrupt conditions */
    SYS_OUT_SHORT (pDrvCtrl, (pDrvCtrl->devAdrs + (UINT32)EL_COMMAND),
                   (EL_CMD_STAT_ENB | intMask));

    intUnlock (intLevel);
    }

/*******************************************************************************
*
* el3c90xCsrReadByte - read a byte from a given window and register
*
* This function reads a byte from a given window and register
*
* RETURNS: UCHAR
*/

LOCAL UCHAR el3c90xCsrReadByte
    (
    EL3C90X_DEVICE * 	pDrvCtrl,	/* device control structure */
    USHORT		reg,		/* register offset */
    int			window		/* window number */
    )
    {
    UCHAR		inByte;

    if (window != NONE)
        {
        /* disable interrupts to prevent race conditions */
        SYS_OUT_SHORT (pDrvCtrl, (pDrvCtrl->devAdrs + (UINT32)EL_COMMAND),
                       EL_CMD_INTR_ENB);

        /* set the window */
	SYS_OUT_SHORT (pDrvCtrl, (pDrvCtrl->devAdrs + (UINT32)EL_COMMAND),
                                  EL_CMD_WINSEL | window);

        /* set the register */
        SYS_IN_BYTE (pDrvCtrl, (pDrvCtrl->devAdrs + (UINT32)reg), inByte);

        /* reenable interrupts */
        SYS_OUT_SHORT (pDrvCtrl, (pDrvCtrl->devAdrs + (UINT32)EL_COMMAND),
                       (EL_CMD_INTR_ENB | EL_INTRS));
        }
    else
        SYS_IN_BYTE (pDrvCtrl, (pDrvCtrl->devAdrs + (UINT32)reg), inByte);

    return (inByte);
    }

/*******************************************************************************
*
* el3c90xCsrReadWord - read a word from a given window and register
*
* This function reads a word from a given window and register
*
* RETURNS: USHORT
*/

LOCAL USHORT el3c90xCsrReadWord
    (
    EL3C90X_DEVICE * 	pDrvCtrl,	/* device control structure */
    USHORT		reg,		/* register offset */
    int			window		/* window number */
    )
    {
    USHORT		inWord;

    if (window != NONE)
        {
        /* disable interrupts to prevent race conditions */
        SYS_OUT_SHORT (pDrvCtrl, (pDrvCtrl->devAdrs + (UINT32)EL_COMMAND),
                       EL_CMD_INTR_ENB);

        /* set the window */
	SYS_OUT_SHORT (pDrvCtrl, (pDrvCtrl->devAdrs + (UINT32)EL_COMMAND),
                                  EL_CMD_WINSEL | window);

        /* set the register */
        SYS_IN_SHORT (pDrvCtrl, (pDrvCtrl->devAdrs + (UINT32)reg), inWord);

        /* reenable interrupts */
        SYS_OUT_SHORT (pDrvCtrl, (pDrvCtrl->devAdrs + (UINT32)EL_COMMAND),
                       (EL_CMD_INTR_ENB | EL_INTRS));
        }
    else
        SYS_IN_SHORT (pDrvCtrl, (pDrvCtrl->devAdrs + (UINT32)reg), inWord);

    return (inWord);
    }

/*******************************************************************************
*
* el3c90xCsrReadLong - read a long word from a given window and register
*
* This function reads a long word from a given window and register
*
* RETURNS: ULONG
*/

LOCAL ULONG el3c90xCsrReadLong
    (
    EL3C90X_DEVICE * 	pDrvCtrl,	/* device control structure */
    USHORT		reg,		/* register offset */
    int			window		/* window number */
    )
    {
    ULONG	inLong;

    if (window != NONE)
        {
        /* disable interrupts to prevent race conditions */
        SYS_OUT_SHORT (pDrvCtrl, (pDrvCtrl->devAdrs + (UINT32)EL_COMMAND),
                       EL_CMD_INTR_ENB);

        /* set the window */
	SYS_OUT_SHORT (pDrvCtrl, (pDrvCtrl->devAdrs + (UINT32)EL_COMMAND),
                                  EL_CMD_WINSEL | window);
        /* set the register */
        SYS_IN_LONG (pDrvCtrl, (pDrvCtrl->devAdrs + (UINT32)reg), inLong);

        /* reenable interrupts */
        SYS_OUT_SHORT (pDrvCtrl, (pDrvCtrl->devAdrs + (UINT32)EL_COMMAND),
                       (EL_CMD_INTR_ENB | EL_INTRS));
        }
    else
        SYS_IN_LONG (pDrvCtrl, (pDrvCtrl->devAdrs + (UINT32)reg), inLong);

    return (inLong);
    }

/*******************************************************************************
*
* el3c90xCsrWriteByte - write a byte to a given window and register
*
* This function writes a byte to a given window and register
*
* RETURNS: N/A
*/

LOCAL void el3c90xCsrWriteByte
    (
    EL3C90X_DEVICE * 	pDrvCtrl,	/* device control structure */
    USHORT		reg,		/* register offset */
    UCHAR		outByte,	/* byte to be written */
    int			window		/* window number */
    )
    {
    if (window != NONE)
        {
        /* disable interrupts to prevent race conditions */
        SYS_OUT_SHORT (pDrvCtrl, (pDrvCtrl->devAdrs + (UINT32)EL_COMMAND),
                       EL_CMD_INTR_ENB);

        /* set the window */
        SYS_OUT_SHORT (pDrvCtrl, (pDrvCtrl->devAdrs + (UINT32)EL_COMMAND),
                       EL_CMD_WINSEL | window);

        /* set the register */
        SYS_OUT_BYTE (pDrvCtrl, (pDrvCtrl->devAdrs + (UINT32)reg), outByte);

        /* reenable interrupts */
        SYS_OUT_SHORT (pDrvCtrl, (pDrvCtrl->devAdrs + (UINT32)EL_COMMAND),
                       (EL_CMD_INTR_ENB | EL_INTRS));
        }
    else
        SYS_OUT_BYTE (pDrvCtrl, (pDrvCtrl->devAdrs + (UINT32)reg), outByte);
    }

/*******************************************************************************
*
* el3c90xCsrWriteWord - write a word to a given window and register
*
* This function writes a word to a given window and register
*
* RETURNS: N/A
*/

LOCAL void el3c90xCsrWriteWord
    (
    EL3C90X_DEVICE * 	pDrvCtrl,	/* device control structure */
    USHORT		reg,		/* register offset */
    USHORT		outWord,	/* word to be written */
    int			window		/* window number */
    )
    {
    if (window != NONE)
        {
        /* disable interrupts to prevent race conditions */
        SYS_OUT_SHORT (pDrvCtrl, (pDrvCtrl->devAdrs + (UINT32)EL_COMMAND),
                       EL_CMD_INTR_ENB);

        /* set the window */
	SYS_OUT_SHORT (pDrvCtrl, (pDrvCtrl->devAdrs + (UINT32)EL_COMMAND),
                                  EL_CMD_WINSEL | window);

        /* set the register */
        SYS_OUT_SHORT (pDrvCtrl, (pDrvCtrl->devAdrs + (UINT32)reg), outWord);

        /* reenable interrupts */
        SYS_OUT_SHORT (pDrvCtrl, (pDrvCtrl->devAdrs + (UINT32)EL_COMMAND),
                       (EL_CMD_INTR_ENB | EL_INTRS));
        }
    else
        SYS_OUT_SHORT (pDrvCtrl, (pDrvCtrl->devAdrs + (UINT32)reg), outWord);
    }

/*******************************************************************************
*
* el3c90xCsrWriteLong - write a long to a given window and register
*
* This function writes a long to a given window and register
*
* RETURNS: N/A
*/

LOCAL void el3c90xCsrWriteLong
    (
    EL3C90X_DEVICE * 	pDrvCtrl,	/* device control structure */
    USHORT		reg,		/* register offset */
    ULONG		outLong,	/* long word to be written */
    int			window		/* window number */
    )
    {
    if (window != NONE)
        {
        /* disable interrupts to prevent race conditions */
        SYS_OUT_SHORT (pDrvCtrl, (pDrvCtrl->devAdrs + (UINT32)EL_COMMAND),
                       EL_CMD_INTR_ENB);

        /* set the window */
	SYS_OUT_SHORT (pDrvCtrl, (pDrvCtrl->devAdrs + (UINT32)EL_COMMAND),
                       EL_CMD_WINSEL | window);

        /* set the register */
        SYS_OUT_LONG (pDrvCtrl, (pDrvCtrl->devAdrs + (UINT32)reg), outLong);

        /* reenable interrupts */
        SYS_OUT_SHORT (pDrvCtrl, (pDrvCtrl->devAdrs + (UINT32)EL_COMMAND),
                       (EL_CMD_INTR_ENB | EL_INTRS));
        }
    else
        SYS_OUT_LONG (pDrvCtrl, (pDrvCtrl->devAdrs + (UINT32)reg), outLong);
    }
