/* if_dc.c - DEC 21x4x Ethernet LAN network interface driver */

/* Copyright 1984-1998 Wind River Systems, Inc. */
/* Copyright 1996-1998 Motorola, Inc., All Rights Reserved */

#include "copyright_wrs.h"

/*
modification history
--------------------
01m,07apr98,dat  fixed man page generation.
01l,26nov97,map  added support for SENS stack.
01k,21nov97,map  update if_flags when DC_* flags are set [SPR# 9873]
01j,30oct97,map  Merged media-select changes from mtx603/dec21140/dec21140.c
                 (ver 01s). Added macros for BSP interface and register access.
                 Added MII interface routines.
01i,27oct97,map  Merged from mv2603/dec21140/dec21140.c (ver 01r) [SPR# 8176]
01h,24jun97,map  Fixed memory leaks. Added dropped frame counts [SPR# 8826]
01m,15dec97,map  use BSD macro to define BSD4[34]_DRIVER flag.
01l,26nov97,map  added support for SENS stack.
01k,21nov97,map  update if_flags when DC_* flags are set [SPR# 9873]
01j,30oct97,map  Merged media-select changes from mtx603/dec21140/dec21140.c
                 (ver 01s). Added macros for BSP interface and register access.
                 Added MII interface routines.
01i,27oct97,map  Merged from mv2603/dec21140/dec21140.c (ver 01r) [SPR# 8176]
01h,24jun97,map  Fixed memory leaks. Added dropped frame counts [SPR# 8826]
01g,06nov96,dgp  doc: final formatting
01f,01jul96,tam  moved code from dcInt to dcHandleRecvInt to reduce the amount
		 of code executed at interrupt level (spr #6070).
01e,19jun96,tam  added output etherhook to dcOutput (spr #6695).
01d,05sep95,vin  added filter Frame setup, added PROMISCOUS MODE to ioctl,
		 changed all ln references to dc, added flag DC_MULTICAST_FLAG.
01c,05apr95,vin  added AUI/10BASE-T dynamic configuration support. 
01b,31mar95,caf  changed name from "dcPci" to "dc".
01a,03mar95,vin	 written.
*/

/* 

This module implements an ethernet interface driver for the DEC 21x4x family,
and currently supports the following variants -- 21040, 21140, and 21140A.

The DEC 21x4x PCI Ethernet controllers are inherently little-endian since they
are designed for a little-endian PCI bus. While the 21040 only supports a
10Mps interface, other members of this family are dual-speed devices which
support both 10 and 100 Mbps.

This driver is designed to be moderately generic, operating unmodified across
the range of architectures and targets supported by VxWorks; and on multiple
versions of the dec21x4x family. To achieve this, the driver takes several
parameters, and external support routines which are detailed below. Also
stated below are assumptions made by the driver of the hardware, and if any of
these assumptions are not true for your hardware, the driver will probably not
function correctly.

This driver supports up to 4 ethernet units per CPU, and can be
configured for either big-endian or little-endian architectures.  It
contains error-recovery code to handle known device errata related to DMA
activity.

On a dec21040, this driver configures the 10BASE-T interface by default and
waits for two seconds to check the status of the link. If the link status is
"fail," it then configures the AUI interface.

The dec21140, and dec21140A devices support both 10 and 100Mbps and also a
variety of MII and non-MII PHY interfaces. This driver reads a DEC version 2.0
SROM device for PHY initialization information, and automatically configures
an apropriate active PHY media.

BOARD LAYOUT

This device is on-board.  No jumpering diagram is necessary.

EXTERNAL INTERFACE

This driver provides the standard external interface with the following
exceptions.  All initialization is performed within the attach routine;
there is no separate initialization routine.  Therefore, in the global interface
structure, the function pointer to the initialization routine is NULL.

The only user-callable routine is dcattach(), which publishes the `dc'
interface and initializes the driver and device.

TARGET-SPECIFIC PARAMETERS

.iP "bus mode"

This parameter is a global variable that can be modified at run-time.

The LAN control register #0 determines the bus mode of the device,
allowing the support of big-endian and little-endian architectures.
This parameter, defined as "ULONG dcCSR0Bmr", is the value that will
be placed into device control register #0. The default is mode is little
endian.

For information about changing this parameter, see the manual

.I "DEC  Local Area Network Controller DEC21040 or DEC21140 for PCI."

.iP "base address of device registers"

This parameter is passed to the driver by dcattach().

.iP "interrupt vector"

This parameter is passed to the driver by dcattach().

This driver configures the device to generate hardware interrupts
for various events within the device; thus it contains
an interrupt handler routine.  The driver calls intConnect() to connect 
its interrupt handler to the interrupt vector generated as a result of 
the device interrupt.

.iP "interrupt level"

This parameter is passed to the driver by dcattach().

Some targets use additional interrupt controller devices to help organize and
service the various interrupt sources.  This driver avoids all board-specific
knowledge of such devices.  During the driver's initialization, the external
routine sysLanIntEnable() is called to perform any board-specific operations
required to allow the servicing of a device interrupt.  For a description of
sysLanIntEnable(), see "External Support Requirements" below.

This parameter is passed to the external routine.

.iP "shared memory address"

This parameter is passed to the driver by dcattach().

The DEC 21x4x device is a DMA type of device and typically shares access to
some region of memory with the CPU.  This driver is designed for systems that
directly share memory between the CPU and the DEC 21x4x.  It assumes that this
shared memory is directly available to it without any arbitration or timing
concerns.

This parameter can be used to specify an explicit memory region for use by the
DEC 21x4x device.  This should be done on hardware that restricts the DEC
21x4x device to a particular memory region.  The constant NONE can be used to
indicate that there are no memory limitations, in which case, the driver
attempts to allocate the shared memory from the system space.

.iP "shared memory size"

This parameter is passed to the driver by dcattach().

This parameter can be used to explicitly limit the amount of shared
memory (bytes) this driver will use.  The constant NONE can be used to
indicate no specific size limitation.  This parameter is used only if
a specific memory region is provided to the driver.

.iP "shared memory width"

This parameter is passed to the driver by dcattach().

Some target hardware that restricts the shared memory region to a
specific location also restricts the access width to this region by
the CPU.  On these targets, performing an access of an invalid width
will cause a bus error.

This parameter can be used to specify the number of bytes of access
width to be used by the driver during access to the shared memory.
The constant NONE can be used to indicate no restrictions.

Current internal support for this mechanism is not robust; implementation 
may not work on all targets requiring these restrictions.

.iP "shared memory buffer size"

This parameter is passed to the driver by dcattach().

The driver and DEC 21x4x device exchange network data in buffers.  This
parameter permits the size of these individual buffers to be limited.
A value of zero indicates that the default buffer size should be used.
The default buffer size is large enough to hold a maximum-size Ethernet
packet.

.iP "pci Memory base"

This parameter is passed to the driver by dcattach(). This parameter
gives the base address of the main memory on the PCI bus. 

.iP "dcOpMode"

This parameter is passed to the driver by dcattach(). This parameter
gives the mode of initialization of the device. The mode flags for both
the DEC21040 and DEC21140 interfaces are listed below.  

DC_PROMISCUOUS_FLAG     0x01 
DC_MULTICAST_FLAG	0x02

The mode flags specific to the DEC21140 interface are listed below.

DC_100_MB_FLAG          0x04
DC_21140_FLAG           0x08
DC_SCRAMBLER_FLAG       0x10
DC_PCS_FLAG             0x20
DC_PS_FLAG              0x40

DC_FULLDUPLEX_FLAG      0x10

Loopback mode flags
DC_ILOOPB_FLAG          0x100 
DC_ELOOPB_FLAG          0x200
DC_HBE_FLAG	        0x400

.iP "Ethernet address"

This is obtained by the driver by reading an ethernet ROM register or the DEC
serial ROM.

.LP

EXTERNAL SUPPORT REQUIREMENTS

This driver requires one external support function:

.iP "void sysLanIntEnable (int level)" "" 9 -1

This routine provides a target-specific enable of the interrupt for the DEC
21x4x device.  Typically, this involves interrupt controller hardware, either
internal or external to the CPU.

This routine is called once via the macro SYS_INT_ENABLE().
.LP

INTERNAL

This driver utilizes macros to access registers and certain BSP
functionalities. These macros may be redefined in a wrapper file to generate a
new driver module with a customized interface.

.iP "SYS_INT_CONNECT"

This macro is used to associate an interrupt handler to the device interrupt
vector pDrvCtrl->ivec. An OK/ERROR status is returned via parameter `pResult'.

.iP "SYS_INT_DISCONNECT"

Currently, this macro is not used by the driver, but exists for future
implementation and will be used to disconnect an interrupt handler from the
device interrupt vector. An OK/ERROR status is returned via parameter, `pResult'.

.iP "SYS_INT_ENABLE"

This macro is used by the driver to enable device interrupts. Typically, this
macro should call a BSP routine that will enable interrupts from this device.

.iP "SYS_INT_DISABLE"

This macro is called by the driver to disable device interrupts.

.iP "DC_CSR_READ"

This macro is used by the driver to read the device command status 
registers. If not redefined, this macro calls the inbuilt driver function
dcCsrRead().

.iP "DC_CSR_WRITE"

This macro is used by the driver to write to a device command status
register. By default this macro calls the inbuilt driver funtion dcCsrWrite().

.LP


SEE ALSO: ifLib, 

.I "DECchip 21040 or 21140 Ethernet LAN Controller for PCI."

*/

#include "vxWorks.h"
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
#include "cacheLib.h"
#include "sys/ioctl.h"
#include "etherLib.h"

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
#include "semLib.h"

#include "drv/netif/if_dc.h"                 /* device description header */

/* defines */

#if (BSD == 44)
#  define	BSD44_DRIVER
#else
#  define	BSD43_DRIVER
#endif /* (BSD == 44) */

#ifdef BSD43_DRIVER
typedef struct enet_hdr
    {
    char dst [6];
    char src [6];
    USHORT type;
    } ENET_HDR;
#endif /* BSD43_DRIVER */

#define ENET_HDR_SIZ        sizeof(ENET_HDR)
#define ENET_HDR_REAL_SIZ   14

#define DC_BUFSIZ      (ETHERMTU + SIZEOF_ETHERHEADER + 6)
#define MAX_UNITS       4	/* maximum units supported */
#define DC_L_POOL	0x10	/* number of Rx loaner buffers in pool */
#define LOOP_PER_NS 4

#define DELAY(count)	{                                               \
			volatile int cx = 0;                            \
			for (cx = 0; cx < (count); cx++);               \
			}

#ifndef NSDELAY
#define NSDELAY(nsec)	{                                               \
			volatile int nx = 0;                            \
			volatile int loop = (int)(nsec*LOOP_PER_NS);    \
			for (nx = 0; nx < loop; nx++);                  \
			}
#endif /* NSDELAY */

#define DEC21140(x)	(x & DC_21140_FLAG)

/*
 * If DC_KICKSTART_TX is TRUE the transmitter is kick-started to force a read
 * of the transmit descriptors, otherwise the internal polling (1.6msec) will
 * initiate a read of the descriptors.  This should be FALSE is there is any
 * chance of memory latency or chip accesses detaining the DEC 21x4x DMA,
 * which results in a transmitter UFLO error.  This can be changed with the
 * global dcKickStartTx below.
 */

#define DC_KICKSTART_TX TRUE

/* Cache macros */

#define DC_CACHE_INVALIDATE(address, len)                               \
        CACHE_DRV_INVALIDATE (&pDrvCtrl->cacheFuncs, (address), (len))

#define DC_CACHE_VIRT_TO_PHYS(address)                                  \
        CACHE_DRV_VIRT_TO_PHYS (&pDrvCtrl->cacheFuncs, (address))

#define DC_CACHE_PHYS_TO_VIRT(address)                                  \
        CACHE_DRV_PHYS_TO_VIRT (&pDrvCtrl->cacheFuncs, (address))

/*
 * Default macro definitions for BSP interface.
 * These macros can be redefined in a wrapper file, to generate
 * a new module with an optimized interface.
 */
#ifndef SYS_INT_CONNECT
#define	SYS_INT_CONNECT(pDrvCtrl, rtn, arg, pResult)                    \
    {                                                                   \
    *pResult = intConnect((VOIDFUNCPTR *)INUM_TO_IVEC (pDrvCtrl->ivec), \
                          (rtn), (int) (arg));                          \
    }
#endif /* SYS_INT_CONNECT */

#ifndef SYS_INT_DISCONNECT
#define SYS_INT_DISCONNECT(pDrvCtrl, rtn, arg, pResult)                 \
    {                                                                   \
    *pResult = OK;                                                      \
    }
#endif /* SYS_INT_DISCONNECT */
    
#ifndef SYS_INT_ENABLE
#define	SYS_INT_ENABLE(pDrvCtrl)                                        \
    {                                                                   \
    IMPORT void sysLanIntEnable(int level);                             \
    sysLanIntEnable (pDrvCtrl->ilevel);                                 \
    }
#endif /* SYS_INT_ENABLE */

#ifndef SYS_INT_DISABLE
#define SYS_INT_DISABLE(pDrvCtrl)                                       \
    {                                                                   \
    IMPORT void sysLanIntDisable(int level);                            \
    sysLanIntDisable (pDrvCtrl->ilevel);                                \
    }
#endif /* SYS_INT_DISABLE */
    
/* memory to PCI address translation macros */

#define PCI_TO_MEM_PHYS(pciAdrs)                                        \
	((pciAdrs) - (pDrvCtrl->pciMemBase))
	
#define MEM_TO_PCI_PHYS(memAdrs)                                        \
	((memAdrs) + (pDrvCtrl->pciMemBase))

#define DC_TD_INDEX_NEXT(pDrvCtrl)                                      \
	(((pDrvCtrl)->txIndex + 1) % (pDrvCtrl)->dcNumTds)

#define DC_TD_DINDEX_NEXT(pDrvCtrl)                                     \
	(((pDrvCtrl)->txDiIndex + 1) % (pDrvCtrl)->dcNumTds)

/*
 * Default macro definitions for device register accesses:
 * These macros can be redefined in a wrapper file, to generate
 * a new module with an optimized interface.
 */
#ifndef DC_CSR_READ
#define DC_CSR_READ(devAdrs, csrNum)                                    \
    	dcCsrRead ((devAdrs), (csrNum))
#endif /* DC_CSR_READ */

#ifndef DC_CSR_WRITE
#define DC_CSR_WRITE(devAdrs, csrNum, csrVal)                           \
    	dcCsrWrite((devAdrs), (csrNum), (csrVal))
#endif /* DC_CSR_WRITE */

#define DC_CSR_UPDATE(devAdrs, csrNum, csrBits)                         \
    	DC_CSR_WRITE((devAdrs), (csrNum),                               \
                     DC_CSR_READ((devAdrs), (csrNum)) | (csrBits))

#define	DC_CSR_RESET(devAdrs, csrNum, csrBits)                          \
    	DC_CSR_WRITE((devAdrs), (csrNum),                               \
                     DC_CSR_READ((devAdrs), (csrNum)) & ~(csrBits))
    
#define DC_INT_ENABLE(devAdrs, X)                                       \
	DC_CSR_UPDATE ((devAdrs), CSR7, (X))

#define DC_INT_DISABLE(devAdrs, X)                                      \
    	DC_CSR_RESET ((devAdrs), CSR7, (X))

#define	DC_SROM_WRITE(devAdrs, adrs, delay)                             \
    	{                                                               \
        DC_CSR_WRITE ((devAdrs), CSR9, (adrs) | CSR9_SR | CSR9_WR);     \
        NSDELAY(delay);                                                 \
        }

#define	DC_SROM_READ(devAdrs)                                           \
	((DC_CSR_READ ((devAdrs), CSR9) & 0x08) >> 3)

/* MII read/write access macros */
    
#define	DC_MII_BIT_READ(devAdrs, pBData)                                        \
    	{                                                                       \
        DC_CSR_WRITE ((devAdrs), CSR9, CSR9_MII_RD | CSR9_RD);                  \
        NSDELAY (100);                                                          \
        DC_CSR_WRITE ((devAdrs), CSR9, CSR9_MII_RD | CSR9_RD | CSR9_MDC);       \
        NSDELAY (100);                                                          \
        *(pBData) |= CSR9_MII_DBIT_RD (DC_CSR_READ ((devAdrs), CSR9));          \
    	}

#define	DC_MII_BIT_WRITE(devAdrs, data)                                 \
    	{                                                               \
        DC_CSR_WRITE ((devAdrs), CSR9, CSR9_MII_DBIT_WR(data) |         \
                      CSR9_MII_WR | CSR9_WR);                           \
        NSDELAY (100);                                                  \
        DC_CSR_WRITE ((devAdrs), CSR9, CSR9_MII_DBIT_WR(data) |         \
                      CSR9_MII_WR | CSR9_WR | CSR9_MDC);                \
        NSDELAY (100);                                                  \
        }

#define	DC_MII_RTRISTATE(devAdrs)                                       \
	{                                                               \
        int retVal;                                                     \
        DC_MII_BIT_READ ((devAdrs), &retVal);                           \
        }

#define	DC_MII_WTRISTATE(devAdrs)                                       \
	{                                                               \
        DC_MII_BIT_WRITE((devAdrs), 0x1);                               \
        DC_MII_BIT_WRITE((devAdrs), 0x0);                               \
	}

#define DC_MII_WRITE(devAdrs, data, bitCount)                           \
    	{                                                               \
        int i=(bitCount);                                               \
                                                                        \
        while (i--)                                                     \
            DC_MII_BIT_WRITE ((devAdrs), ((data) >> i) & 0x1);          \
        }

#define	DC_MII_READ(devAdrs, pData, bitCount)                           \
    	{                                                               \
        int i=(bitCount);                                               \
                                                                        \
        while (i--)                                                     \
            {                                                           \
            *(pData) <<= 1;                                             \
            DC_MII_BIT_READ ((devAdrs), (pData));                       \
            }                                                           \
        }

#define BUILD_CLUSTER(pDrvCtrl, pEnetHdr, pData, len)                           \
	build_cluster ((pData), (len), &(pDrvCtrl)->idr.ac_if, MC_LANCE,        \
                       (pDrvCtrl)->pRefCnt[(pDrvCtrl)->nLoanRx - 1],            \
                       (FUNCPTR) dcLoanFree, (int)(pDrvCtrl),                   \
                       (int) (pEnetHdr),                                        \
                       (int)(pDrvCtrl)->pRefCnt[(pDrvCtrl)->nLoanRx - 1])

#ifdef BSD43_DRIVER
#define	DC_START_OUTPUT(pDrvCtrl)                                        \
    	dcStartOutput (pDrvCtrl->idr.ac_if.if_unit)
#else
#define	DC_START_OUTPUT(pDrvCtrl)                                        \
    	dcStartOutput (pDrvCtrl)
#endif
    
/* Typedefs for external structures that are not typedef'd in their .h files */

typedef struct mbuf MBUF;
typedef struct arpcom IDR;                  /* Interface Data Record wrapper */
typedef struct ifnet IFNET;                 /* real Interface Data Record */
typedef struct sockaddr SOCK;

#ifdef DC_DEBUG
typedef struct /* dc_stats */
    {
    /* Transmit Error Stats */
    int		txJbrTmo;
    int		txLostCarrier;
    int		txNoCarrier;
    int		txLateCollision;
    int		txExsCollision;
    int		txHeartBeatFail;
    int		txLinkFail;
    int		txUnderflow;
    int		txDeferred;
    int		txRetries;     
    int		txDropped;     
    int		txFlushCalled;
    int		txFlushNeeded;
    int		txFlushDone;   
    int		txQsizeMax;    
    int		txQsizeCur;    

    /* Receive Error Stats */
    int		rxFiltrErr;  
    int		rxDescErr;   
    int		rxRuntFrm;   
    int		rxTooLong;   
    int		rxCollision; 
    int		rxDribbleBit;
    int		rxCrcErr;    
    int		rxOverflow;  
    int		rxMiiErr;    
    int		rxMissed;
    int		rxLatency;
    int		rxStart;
    int		rxStop;
    } DC_STATS;
#endif /* DC_DEBUG */

/* The definition of the driver control structure */

typedef struct /* drv_ctrl */
    {
    IDR		idr;			/* Interface Data Record */
    int		dcNumRds;		/* RMD ring size */
    int		rxIndex;                /* index into RMD ring */
    DC_RDE *	rxRing;			/* RMD ring */
    int		dcNumTds;		/* TMD ring size */
    int		txIndex;		/* index into TMD ring */
    int		txDiIndex;		/* disposal index into TMD ring */
    DC_TDE *	txRing;			/* TMD ring */
    BOOL        attached;               /* indicates unit is attached */
    BOOL	txFlushScheduled;
    int		txCount;
    SEM_ID      TxSem;                  /* transmitter semaphore */
    ULONG	dcOpMode;		/* mode of operation */
    int         ivec;                   /* interrupt vector */
    int         ilevel;                 /* interrupt level */
    ULONG	devAdrs;                /* device structure address */
    ULONG	pciMemBase;		/* memory base as seen from PCI*/
    int         memWidth;               /* width of data port */
    CACHE_FUNCS cacheFuncs;             /* cache function pointers */
    int		nLoanRx;		/* number of Rx buffers left to loan */
    char	*lPool[DC_L_POOL];	/* receive loaner pool ptrs */
    UINT8	*pRefCnt[DC_L_POOL];	/* stack of reference count pointers */
    UINT8	refCnt[DC_L_POOL];	/* actual reference count values */
    ULONG *	pFltrFrm;		/* pointer to setup filter frame */
    MediaBlocksType dcMediaBlocks;      /* SROM Media Data */
#ifdef DC_DEBUG
    DC_STATS	errStats;
#endif /* DC_DEBUG */
    } DRV_CTRL;

#define DRV_CTRL_SIZ		sizeof(DRV_CTRL)
#define RMD_SIZ			sizeof(DC_RDE)
#define TMD_SIZ			sizeof(DC_TDE)
#define DC_FLUSH_RETRIES	5
#define MAX_TX_RETRIES		2

/* globals */

#ifdef DC_DEBUG
int	dcCsrShow (int unit);
int	dcViewRom (ULONG devAdrs, UCHAR lineCnt, int cnt);
#endif /* DC_DEBUG */

ULONG	dcCSR0Bmr	= 0;
BOOL 	dcKickStartTx 	= DC_KICKSTART_TX;

/* locals */

LOCAL DRV_CTRL  drvCtrl [MAX_UNITS];	/* array of driver control structs */
LOCAL int	dcNumRds  = NUM_RDS;	/* number of ring descriptors */
LOCAL int	dcNumTds  = NUM_TDS;	/* number of xmit descriptors */
LOCAL int 	dcLPool = DC_L_POOL;

/* forward static functions */

LOCAL void 	dcReset (int unit);
LOCAL void 	dcInt (DRV_CTRL *pDrvCtrl);
LOCAL void 	dcHandleRecvInt (DRV_CTRL *pDrvCtrl);
LOCAL void	dcTxRingClean (DRV_CTRL *pDrvCtrl);
LOCAL STATUS 	dcRecv (DRV_CTRL *pDrvCtrl, DC_RDE *rmd);
LOCAL void	dcTxFlush (DRV_CTRL *pDrvCtrl);
#ifdef BSD43_DRIVER
LOCAL int 	dcOutput (IDR *ifp, MBUF *m0, SOCK *dst);
LOCAL void	dcStartOutput (int unit);
#else
LOCAL void	dcStartOutput (DRV_CTRL *pDrvCtrl);
#endif /* BSD43_DRIVER */
LOCAL int 	dcIoctl (IDR *ifp, int cmd, caddr_t data);
LOCAL int 	dcChipReset (DRV_CTRL *pDrvCtrl);
LOCAL DC_RDE *	dcGetFullRMD (DRV_CTRL *pDrvCtrl);
LOCAL void	dc21040AuiTpInit (ULONG devAdrs);
LOCAL void	dc21140AuiMiiInit (DRV_CTRL *pDrvCtrl, 
                                   ULONG devAdrs, UINT dcCSR6);
LOCAL void      dcSelectMedia (DRV_CTRL *pDrvCtrl);
LOCAL void 	dcCsrWrite (ULONG devAdrs, int reg, ULONG value);
LOCAL ULONG 	dcCsrRead (ULONG devAdrs, int reg);
LOCAL void 	dcRestart (int unit);
LOCAL STATUS 	dcEnetAddrGet (ULONG devAdrs, char * enetAdrs, int len);
LOCAL STATUS 	dc21140EnetAddrGet (ULONG devAdrs, char * enetAdrs, int len);
LOCAL void 	dcLoanFree (DRV_CTRL *pDrvCtrl, char *pRxBuf, UINT8 *pRef);
LOCAL int	dcFltrFrmSetup (DRV_CTRL * pDrvCtrl, char * pPhysAdrsTbl, 
				int tblLen);
LOCAL int	dcFltrFrmXmit (DRV_CTRL *pDrvCtrl, char * pPhysAdrsTbl,
			       int tblLen);
LOCAL USHORT	dcMiiPhyRead (DRV_CTRL *pDrvCrl, UINT phyAdrs, UINT phyReg);
LOCAL void	dcMiiPhyWrite (DRV_CTRL *pDrvCtrl, UINT phyAdrs,
                               UINT phyReg, USHORT data);
LOCAL USHORT	dcReadRom (ULONG devAdrs, UCHAR lineCnt);



/*******************************************************************************
*
* dcInit - device initialization sequence
*
* RETURNS: N/A.
*/

LOCAL void dcInit
    (
    ULONG devAdrs
    )
    {
    char dummy_buffer[60];
    DC_TDE tmd;

    tmd.tDesc0 = 0x80000000;
    tmd.tDesc1 = 0xe200003c;
    tmd.tDesc2 = (ULONG)dummy_buffer;
    tmd.tDesc3 = 0;

    /* start xmit dummy packet, this will never go out to the physical bus */

    DC_CSR_WRITE (devAdrs, CSR4, (ULONG)(&tmd));
    DC_CSR_WRITE (devAdrs, CSR7, 0);
    DC_CSR_WRITE (devAdrs, CSR6, 0x2642400);
    DC_CSR_WRITE (devAdrs, CSR1, CSR1_TPD);      /* xmit poll demand */
    DELAY(0x8000);

    /*
     * Reset the device while xmitting, it ensures the device will not hang
     * up in its 1st xmit later on
     */

    DC_CSR_UPDATE (devAdrs, CSR0, CSR0_SWR);
    }

/*******************************************************************************
*
* dcattach - publish the `dc' network interface.
*
* This routine publishes the `dc' interface by filling in a network interface
* record and adding this record to the system list.  This routine also
* initializes the driver and the device to the operational state.
*
* The <unit> parameter is used to specify the device unit to initialize.
*
* The <devAdrs> is used to specify the I/O address base of the device.
*
* The <ivec> parameter is used to specify the interrupt vector associated
* with the device interrupt.
*
* The <ilevel> parater is used to specify the level of the interrupt which
* the device would use.
*
* The <memAdrs> parameter can be used to specify the location of the
* memory that will be shared between the driver and the device.  The value
* NONE is used to indicate that the driver should obtain the memory.
*
* The <memSize> parameter is valid only if the <memAdrs> parameter is not
* set to NONE, in which case <memSize> indicates the size of the
* provided memory region.
*
* The <memWidth> parameter sets the memory pool's data port width (in bytes);
* if it is NONE, any data width is used.
*
* The <pciMemBase> parameter defines the main memory base as seen from PCI bus.
* 
* The <dcOpMode> parameter defines the mode in which the device should be
* operational.
*
* BUGS
*
* To zero out DEC 21x4x data structures, this routine uses bzero(), which
* ignores the <memWidth> specification and uses any size data access to write
* to memory.
*
* RETURNS: OK or ERROR.
*/

STATUS dcattach
    (
    int		unit,		/* unit number */
    ULONG	devAdrs,	/* device I/O address */
    int		ivec,		/* interrupt vector */
    int		ilevel,		/* interrupt level */
    char *	memAdrs,	/* address of memory pool (-1 = malloc it) */
    ULONG	memSize,	/* only used if memory pool is NOT malloc()'d */
    int		memWidth,	/* byte-width of data (-1 = any width) */
    ULONG	pciMemBase,	/* main memory base as seen from PCI bus */
    int		dcOpMode	/* mode of operation */
    )
    {
    DRV_CTRL *		pDrvCtrl;	/* pointer to drvctrl */
    unsigned int	sz;		/* temporary size holder */
    char *		pShMem;		/* start of the device memory pool */
    char *		buf;		/* temp buffer pointer */
    void *		pTemp;		/* temp pointer */
    DC_RDE *		rmd;		/* pointer to rcv descriptor */
    DC_TDE *		tmd;		/* pointer to xmit descriptor */
    int			ix;		/* counter */
    ULONG		uTemp;		/* temporary storage */
    int			retVal;
   
    /* Sanity check the unit number */

    if (unit < 0 || unit >= MAX_UNITS)
        return (ERROR);

    uTemp = 0;

    /* Ensure single invocation per system life */

    pDrvCtrl = & drvCtrl [unit];

    if (pDrvCtrl->attached)
        return (OK);

    pDrvCtrl->ivec	= ivec;			/* interrupt vector */
    pDrvCtrl->ilevel	= ilevel;		/* interrupt level */
    pDrvCtrl->devAdrs	= devAdrs;		/* device I/O address */
    pDrvCtrl->pciMemBase  = pciMemBase;		/* pci memory base */
    pDrvCtrl->memWidth 	= memWidth;		/* memory width */
    pDrvCtrl->dcOpMode	= (dcOpMode == NONE) ? 0 : dcOpMode; /* operation mode */

    /* Publish the interface data record */

#ifdef BSD43_DRIVER
    ether_attach (
		  & pDrvCtrl->idr.ac_if,
		  unit,
		  "dc",
		  (FUNCPTR) NULL,
		  (FUNCPTR) dcIoctl,
		  (FUNCPTR) dcOutput,
		  (FUNCPTR) dcReset
		 );
#else
    ether_attach (
                 (IFNET *) &pDrvCtrl->idr,
                 unit,
                 "dc",
                 (FUNCPTR) NULL,
                 (FUNCPTR) dcIoctl,
                 (FUNCPTR) ether_output,
                 (FUNCPTR) dcReset
                 );

     pDrvCtrl->idr.ac_if.if_start = (FUNCPTR) dcStartOutput;
#endif /* BSD43_DRIVER */

#ifdef DC_DEBUG
    bzero ((char *)&pDrvCtrl->errStats, sizeof (DC_STATS));
#endif /* DC_DEBUG */

    dcInit(pDrvCtrl->devAdrs);

    /* Create the transmit semaphore. */

    if ((pDrvCtrl->TxSem = semMCreate (SEM_Q_PRIORITY | SEM_DELETE_SAFE |
				       SEM_INVERSION_SAFE)) == NULL)
        {
        printf ("dc: error creating transmitter semaphore\n");
        return (ERROR);
        }

    /* Establish size of shared memory region we require */

    if ((int) memAdrs != NONE)  /* specified memory pool */
	{
	sz  = ((memSize - FLTR_FRM_SIZE + RMD_SIZ + TMD_SIZ) /
               ((3 * DC_BUFSIZ) + RMD_SIZ + TMD_SIZ));
	dcNumRds = max (sz, MIN_RDS);
	dcNumTds = max (sz, MIN_TDS);
        dcLPool = min (dcNumRds, DC_L_POOL);
	}

    /* add it all up - allow for alignment adjustment */

    sz = ((dcNumRds + 1) * RMD_SIZ) + (dcNumRds * DC_BUFSIZ) +
	 ((dcNumTds + 1) * TMD_SIZ) + (dcNumTds * DC_BUFSIZ) + 
	 (DC_BUFSIZ * dcLPool) + FLTR_FRM_SIZE;

    /* Establish a region of shared memory */

    /*
     * OK. We now know how much shared memory we need.  If the caller
     * provides a specific memory region, we check to see if the provided
     * region is large enough for our needs.  If the caller did not
     * provide a specific region, then we attempt to allocate the memory
     * from the system, using the cache aware allocation system call.
     */

    switch ((int) memAdrs)
        {
        default :       /* caller provided memory */
            if (memSize < sz)     /* not enough space */
                {
                printf ("dc: not enough memory provided\n");
                return (ERROR);
                }
            else
                dcNumRds += (memSize - sz) / (DC_BUFSIZ + TMD_SIZ);

            pShMem = memAdrs;             /* set the beginning of pool */

            /* assume pool is cache coherent, copy null structure */

            pDrvCtrl->cacheFuncs = cacheNullFuncs;

            break;

        case NONE :     /* get our own memory */

            /*
	     * Because the structures that are shared between the device
             * and the driver may share cache lines, the possibility exists
             * that the driver could flush a cache line for a structure and
             * wipe out an asynchronous change by the device to a neighboring
             * structure. Therefore, this driver cannot operate with memory
             * that is not write coherent.  We check for the availability of
             * such memory here, and abort if the system did not give us what
             * we need.
             */

            if (!CACHE_DMA_IS_WRITE_COHERENT ())
                {
                printf ("dc: device requires cache coherent memory\n");
                return (ERROR);
                }

            pShMem = (char *) cacheDmaMalloc (sz);

            if ((int)pShMem == NULL)
                {
                printf ("dc: system memory unavailable\n");
                return (ERROR);
                }

            /* copy the DMA structure */

            pDrvCtrl->cacheFuncs = cacheDmaFuncs;

            break;
        }

    SYS_INT_CONNECT(pDrvCtrl, dcInt, pDrvCtrl, &retVal);
    if (retVal == ERROR)
        return (ERROR);

    /*           Turkey Carving
     *           --------------
     *
     *             LOW MEMORY
     * |-------------------------------------|
     * |         The Rx descriptors          |
     * | (dcNumRds * sizeof (DC_RDE))        |
     * |-------------------------------------|
     * |          The receive buffers        |
     * |       (DC_BUFSIZ * dcNumRds)        |
     * |-------------------------------------|
     * |         The Rx loaner pool          |
     * |        (DC_BUFSIZ * dcLPool)        |
     * |-------------------------------------|
     * |         The Tx descriptors          |
     * | (dcNumTds *  sizeof (DC_TDE))       |
     * |-------------------------------------|
     * |           The transmit buffers      |
     * |       (DC_BUFSIZ * dcNumTds)        |
     * |-------------------------------------|
     * |           Setup Filter Frame        |
     * |           (FLTR_FRM_SIZE)           |
     * |-------------------------------------|
     */

    /* zero the shared memory */

    bzero ( (char *) pShMem, (int) sz );

    /* carve Rx memory structure */

    pDrvCtrl->rxRing = (DC_RDE *) (((int)pShMem + 0x03) & ~0x03); /* wd align */
    pDrvCtrl->dcNumRds = dcNumRds;		/* receive ring size */
    pDrvCtrl->rxIndex = 0;

    /* Set up the Rx descriptors */

    rmd = pDrvCtrl->rxRing;                      /* receive ring */
    buf = (char *)(rmd + pDrvCtrl->dcNumRds);
    for (ix = 0; ix < pDrvCtrl->dcNumRds; ix++, rmd++, buf += DC_BUFSIZ)
        {
        pTemp = DC_CACHE_VIRT_TO_PHYS (buf);	/* convert to PCI  phys addr */
	pTemp = (void *)(MEM_TO_PCI_PHYS((ULONG)pTemp));
	rmd->rDesc2 = PCISWAP((ULONG)pTemp);	/* buffer address 1 */
	rmd->rDesc3 = 0;			/* no second buffer */

	/* buffer size */

	rmd->rDesc1 = PCISWAP(RDESC1_RBS1_VAL(DC_BUFSIZ) | 
			      RDESC1_RBS2_VAL(0));

	if (ix == (pDrvCtrl->dcNumRds - 1))	/* if its is last one */
    	    rmd->rDesc1 |= PCISWAP(RDESC1_RER);	/* end of receive ring */

	rmd->rDesc0 = PCISWAP(RDESC0_OWN);	/* give ownership to lance */
        }

    /* setup Rx loaner pool */

    pDrvCtrl->nLoanRx = dcLPool;
    for (ix = 0; ix < dcLPool; ix++, buf += DC_BUFSIZ)
	{
        pDrvCtrl->lPool[ix] = buf;
        pDrvCtrl->refCnt[ix] = 0;
        pDrvCtrl->pRefCnt[ix] = & pDrvCtrl->refCnt[ix];
	}

    /* carve Tx memory structure */

    pDrvCtrl->txRing = (DC_TDE *) (((int)buf + 0x03) & ~0x03); /* word align */
    pDrvCtrl->dcNumTds = dcNumTds;	/* transmit ring size */
    pDrvCtrl->txIndex = 0;
    pDrvCtrl->txDiIndex = 0;

    /* Setup the Tx descriptors */

    tmd = pDrvCtrl->txRing;                 /* transmit ring */
    buf = (char *)(tmd + pDrvCtrl->dcNumTds);
    for (ix = 0; ix < pDrvCtrl->dcNumTds; ix++, tmd++, buf += DC_BUFSIZ)
        {
        pTemp = DC_CACHE_VIRT_TO_PHYS (buf);	/* convert to PCI phys addr */
	pTemp = (void *)(MEM_TO_PCI_PHYS((ULONG)pTemp));
	tmd->tDesc2 = PCISWAP((ULONG)pTemp);	/* buffer address 1 */
	tmd->tDesc3 = 0;			/* no second buffer */

	tmd->tDesc1 = PCISWAP((TDESC1_TBS1_PUT(0) |
			       TDESC1_TBS2_PUT(0) |
			       TDESC1_IC	  | 	/* intrpt on xmit */
			       TDESC1_LS          |	/* last segment */
			       TDESC1_FS));		/* first segment */

	if (ix == (pDrvCtrl->dcNumTds - 1))	/* if its is last one */
	    tmd->tDesc1 |= PCISWAP(TDESC1_TER);	/* end of Xmit ring */

	tmd->tDesc0 = 0 ;			/* clear status */
        }

    /* carve Setup Filter Frame buffer */
    
    pDrvCtrl->pFltrFrm = (ULONG *)(((int)buf + 0x03) & ~0x03); /* word align */
    buf += FLTR_FRM_SIZE;

    /* Flush the write pipe */

    CACHE_PIPE_FLUSH ();

    /* Device Initialization */

    dcChipReset (pDrvCtrl);			/* reset the chip */

    pTemp = DC_CACHE_VIRT_TO_PHYS (pDrvCtrl->rxRing);
    pTemp = (void *)(MEM_TO_PCI_PHYS((ULONG)pTemp));

    DC_CSR_WRITE (devAdrs, CSR3, ((ULONG)pTemp));	/* recv descriptor */

    pTemp = DC_CACHE_VIRT_TO_PHYS (pDrvCtrl->txRing);
    pTemp = (void *)(MEM_TO_PCI_PHYS((ULONG)pTemp));

    DC_CSR_WRITE (devAdrs, CSR4, ((ULONG)pTemp));	/* xmit descriptor */

    /* get the ethernet address */

    if (!DEC21140(pDrvCtrl->dcOpMode))
    	{
        if (dcEnetAddrGet (devAdrs, (char *) pDrvCtrl->idr.ac_enaddr, 6) != OK)
	    logMsg ("dc: error reading ethernet rom\n", 0,0,0,0,0,0);
    	}
    else
    	{
        if (dc21140EnetAddrGet (devAdrs, (char *) pDrvCtrl->idr.ac_enaddr, 6) 
	    != OK)
	    logMsg ("dc21140: error reading ethernet rom\n", 0,0,0,0,0,0);
    	}

    if (!DEC21140(pDrvCtrl->dcOpMode))
    	{
        dc21040AuiTpInit (devAdrs);		/* configure AUI or 10BASE-T */
	DC_CSR_WRITE (devAdrs, CSR6, 0x0);
        pDrvCtrl->dcMediaBlocks.DontSwitch = 1;
    	}
    else
    	{
	DC_CSR_WRITE (devAdrs, CSR6, 0x0);
        dc21140AuiMiiInit (pDrvCtrl, devAdrs, uTemp);
    	}

    if ((pDrvCtrl->dcOpMode & DC_FULLDUPLEX_FLAG) && 
	(pDrvCtrl->dcOpMode != NONE))
        DC_CSR_UPDATE (devAdrs, CSR6, CSR6_FD);

    /* clear the status register */

    DC_CSR_WRITE (devAdrs, CSR5, 0xffffffff);

    /* set the operating mode to start Xmitter only */

    DC_CSR_UPDATE (devAdrs, CSR6, (CSR6_ST | CSR6_BIT25));

    if ((pDrvCtrl->dcOpMode & DC_ILOOPB_FLAG) &&
	(pDrvCtrl->dcOpMode != NONE))
        DC_CSR_UPDATE (devAdrs, CSR6, CSR6_OM_ILB);
    else if ((pDrvCtrl->dcOpMode & DC_ELOOPB_FLAG) &&
             (pDrvCtrl->dcOpMode != NONE))
        DC_CSR_UPDATE (devAdrs, CSR6, CSR6_OM_ELB);

    /* Xmit a Filter Setup Frame */

    dcFltrFrmXmit (pDrvCtrl, (char *) pDrvCtrl->idr.ac_enaddr, 1);

    /* start the receiver with multicast and promiscuous mode, if needed */

    DC_CSR_UPDATE (devAdrs, CSR6,
                   (CSR6_SR | 
                    ((pDrvCtrl->dcOpMode & DC_MULTICAST_FLAG) ? CSR6_PM: 0) |
                    ((pDrvCtrl->dcOpMode & DC_PROMISCUOUS_FLAG) ? CSR6_PR: 0)));

    /* set up the interrupts */

    if (!DEC21140(pDrvCtrl->dcOpMode))
    	DC_CSR_WRITE (devAdrs, CSR7, (
			       CSR7_NIM |	/* normal interrupt mask */
			       CSR7_AIM |	/* abnormal interrupt mask */
			       CSR7_SEM |	/* system error mask */
			       CSR7_RIM |	/* rcv  interrupt mask */
			       CSR7_RUM	|	/* rcv buff unavailable mask */
			       CSR7_TUM 	/* xmit buff unavailble mask */
			       ));
    else
    	DC_CSR_WRITE (devAdrs, CSR7, (
			       CSR7_NIM |	/* normal interrupt mask */
			       CSR7_AIM |	/* abnormal interrupt mask */
			       CSR7_SEM |	/* system error mask */
			       CSR7_RIM |	/* rcv  interrupt mask */
			       CSR7_RUM	|	/* rcv buff unavailable mask */
			       CSR7_TUM 	/* xmit buff unavailble mask */
			       ));

    SYS_INT_ENABLE (pDrvCtrl);

    pDrvCtrl->idr.ac_if.if_flags |= ((IFF_UP | IFF_RUNNING | IFF_NOTRAILERS) | 
                       ((dcOpMode & DC_PROMISCUOUS_FLAG) ? IFF_PROMISC : 0) | 
                       ((dcOpMode & DC_MULTICAST_FLAG) ? IFF_ALLMULTI : 0));

    /* Set our flag */

    pDrvCtrl->attached = TRUE;
    pDrvCtrl->txFlushScheduled = FALSE;

    return (OK);
    }

/*******************************************************************************
*
* dcReset - reset the interface
*
* Mark interface as inactive & reset the chip
*/

LOCAL void dcReset
    (
    int unit
    )
    {
    DRV_CTRL *pDrvCtrl = & drvCtrl [unit];

    pDrvCtrl->idr.ac_if.if_flags = 0;
    dcChipReset (pDrvCtrl);                           /* reset device */
    }

/*******************************************************************************
*
* dcInt - handle controller interrupt
*
* This routine is called at interrupt level in response to an interrupt from
* the controller.
*/

LOCAL void dcInt
    (
    DRV_CTRL *	pDrvCtrl	/* pointer to device control struct */
    )
    {
    ULONG	stat;		/* status register */
    ULONG	devAdrs;	/* device address */

    devAdrs = pDrvCtrl->devAdrs;

    /* Read the device status register */

    stat = DC_CSR_READ (devAdrs, CSR5);

    /* clear the interrupts */

    DC_CSR_WRITE (devAdrs, CSR5, stat);
    stat &= DC_CSR_READ (devAdrs, CSR7);

    /* If false interrupt, return. */

    if ( ! (stat & (CSR5_NIS | CSR5_AIS)) ) 
        return;

    /* Check for system error */

    if (stat & CSR5_SE)
        {
        ++pDrvCtrl->idr.ac_if.if_ierrors;
    
        /* restart chip on fatal error */

        pDrvCtrl->idr.ac_if.if_flags &= ~(IFF_UP | IFF_RUNNING);
    
        netJobAdd ((FUNCPTR)dcRestart, pDrvCtrl->idr.ac_if.if_unit, 0,0,0,0);
        }
    
    /* Have netTask handle any input packets */

    if (stat & (CSR5_RI | CSR5_RU))
        {
        DC_INT_DISABLE (devAdrs, CSR7_RIM);
        netJobAdd ((FUNCPTR)dcHandleRecvInt, (int)pDrvCtrl, 0, 0, 0, 0);
        }

    if ((stat & CSR5_TU) && pDrvCtrl->idr.ac_if.if_snd.ifq_len &&
        (! pDrvCtrl->txFlushScheduled))
        {
        pDrvCtrl->txFlushScheduled = TRUE;
        netJobAdd ((FUNCPTR) dcTxFlush, (int) pDrvCtrl, 0, 0, 0, 0);
        }

    return;
    }

/*******************************************************************************
*
* dcHandleRecvInt - task level interrupt service for input packets
*
* This routine is called at task level indirectly by the interrupt
* service routine to do any message received processing.
*/

LOCAL void dcHandleRecvInt
    (
    DRV_CTRL *	pDrvCtrl	/* pointer to device control struct */
    )
    {
    DC_RDE *	rmd;		/* pointer to Rx ring descriptor */

    while ((rmd = dcGetFullRMD (pDrvCtrl)) != NULL)
        dcRecv (pDrvCtrl, rmd);

    DC_INT_ENABLE (pDrvCtrl->devAdrs, CSR7_RIM);

    while ((rmd = dcGetFullRMD (pDrvCtrl)) != NULL)
        dcRecv (pDrvCtrl, rmd);

#ifdef DC_DEBUG
    pDrvCtrl->errStats.rxMissed += (DC_CSR_READ (pDrvCtrl->devAdrs, CSR8) &
                                    (CSR8_MFO | CSR8_MFC_MSK));
#endif /* DC_DEBUG */

    return;
    }


/*******************************************************************************
*
* dcTxRingClean - cleans up the transmit ring
*
* This routine is called at task level in the context of dcOutput() or
* tNetTask by dcTxFlush() to collect statistics, and mark "free" all
* descriptors consumed by the device.
*/

LOCAL void dcTxRingClean
    (
    DRV_CTRL *pDrvCtrl
    )
    {
    DC_TDE *    tmd;            /* pointer to Xmit ring descriptor */

    while (pDrvCtrl->txDiIndex != pDrvCtrl->txIndex)
        {
        /* disposal has not caught up */

        tmd = pDrvCtrl->txRing + pDrvCtrl->txDiIndex;

        /* if the buffer is still owned by device, don't touch it */

        DC_CACHE_INVALIDATE (tmd, TMD_SIZ);
        if (tmd->tDesc0 & PCISWAP(TDESC0_OWN))
            break;

        /* now bump the tmd disposal index pointer around the ring */

        pDrvCtrl->txDiIndex = (pDrvCtrl->txDiIndex + 1) % pDrvCtrl->dcNumTds;
        pDrvCtrl->txCount--;

#ifdef DC_DEBUG
        {
        UINT		tdesc0 = PCISWAP(tmd->tDesc0);
        DC_STATS *	pErrStats = &pDrvCtrl->errStats;

        if (tdesc0 & TDESC0_TO)
            pErrStats->txJbrTmo++;

        if (tdesc0 & TDESC0_LO)
            pErrStats->txLostCarrier++;

        if (tdesc0 & TDESC0_NC)
            pErrStats->txNoCarrier++;

        if (tdesc0 & TDESC0_LC)
            pErrStats->txLateCollision++;

        if (tdesc0 & TDESC0_HF)
            pErrStats->txHeartBeatFail++;

        if (tdesc0 & TDESC0_LF)
            pErrStats->txLinkFail++;

        if (tdesc0 & TDESC0_UF)
            pErrStats->txUnderflow++;

        if (tdesc0 & TDESC0_DE)
            pErrStats->txDeferred++;
        }
#endif /* DC_DEBUG */

        /*
         * TDESC0_ES is an "OR" of LC, NC, UF, EC.
         * here for error conditions.
         */

        if (tmd->tDesc0 & PCISWAP(TDESC0_ES)) 
	    {
            /* check for no carrier */

#if 0  /* HELP */
            if ((tmd->tDesc0 & PCISWAP(TDESC0_NC | TDESC0_LO | TDESC0_LF)) &&
                (pDrvCtrl->dcMediaBlocks.DontSwitch == 0)) 
		{
		dcSelectMedia (pDrvCtrl);
		return;
		}
            pDrvCtrl->dcMediaBlocks.DontSwitch = 1;
#endif

            pDrvCtrl->idr.ac_if.if_oerrors++;     /* output error */
            pDrvCtrl->idr.ac_if.if_opackets--;

            /*
	     * If error was due to excess collisions, bump the collision
             * counter.
             */

            if (tmd->tDesc0 & PCISWAP(TDESC0_EC)) 
		pDrvCtrl->idr.ac_if.if_collisions += 16;

	    /* bump the collision counter if late collision */

	    if (tmd->tDesc0 & PCISWAP(TDESC0_LC)) 
		pDrvCtrl->idr.ac_if.if_collisions++;


	    /* check for no carrier */
#if 0 /* HELP */
	    if (tmd->tDesc0 & PCISWAP(TDESC0_NC | TDESC0_LO | TDESC0_LF)) 
		{
		logMsg ("dc%d: no carrier\n",
			pDrvCtrl->idr.ac_if.if_unit, 0,0,0,0,0);
		dcSelectMedia (pDrvCtrl);
		}
#endif

            /*
	     * Restart chip on fatal errors.
             * The following code handles the situation where the transmitter
             * shuts down due to an underflow error.  This is a situation that
             * will occur if the DMA cannot keep up with the transmitter.
             * It will occur if the device is being held off from DMA access
             * for too long or due to significant memory latency.  DRAM
             * refresh or slow memory could influence this.  Many
             * implementation use a dedicated device buffer.  This can be
             * static RAM to eliminate refresh conflicts; or dual-port RAM
             * so that the device can have free run of this memory during its
             * DMA transfers.
             */

            if (tmd->tDesc0 & PCISWAP(TDESC0_UF))
                {
                pDrvCtrl->idr.ac_if.if_flags &= ~(IFF_UP | IFF_RUNNING);
                netJobAdd ((FUNCPTR)dcRestart, pDrvCtrl->idr.ac_if.if_unit,
                           0,0,0,0 );
                return;
                }
            }
	tmd->tDesc0 = 0;	/* clear all error & stat stuff */
        }

    /* Flush the write pipe */

    CACHE_PIPE_FLUSH ();
    }

/*******************************************************************************
*
* dcGetFullRMD - get next received message RMD
*
* Returns ptr to next Rx desc to process, or NULL if none ready.
*/

LOCAL DC_RDE * dcGetFullRMD
    (
    DRV_CTRL *	pDrvCtrl	/* pointer to device control struct */
    )
    {
    DC_RDE *	rmd;		/* pointer to Rx ring descriptor */

    /* Refuse to do anything if flags are down */

    if  (
        (pDrvCtrl->idr.ac_if.if_flags & (IFF_UP | IFF_RUNNING) ) !=
        (IFF_UP | IFF_RUNNING)
        )
        return ((DC_RDE *) NULL);


    rmd = pDrvCtrl->rxRing + pDrvCtrl->rxIndex;       /* form ptr to Rx desc */

    DC_CACHE_INVALIDATE (rmd, RMD_SIZ);

    if ((rmd->rDesc0 & PCISWAP(RDESC0_OWN)) == 0)
        return (rmd);
    else
        return ((DC_RDE *) NULL);
    }

/*******************************************************************************
*
* dcRecv - process the next incoming packet
*
*/

LOCAL STATUS dcRecv
    (
    DRV_CTRL *	pDrvCtrl,	/* pointer to device control struct */
    DC_RDE *	rmd		/* pointer to Rx ring descriptor */
    )
    {
    ENET_HDR *	pEnetHdr;	/* pointer to ethernet header */
    MBUF *	pMbuf = NULL;	/* pointer to mbuf */
    u_long	pPhys;
    UCHAR *	pData;		/* pointer to data */
    int		len;		/* length */
    USHORT	ether_type;	/* ether packet type */
#ifdef DC_DEBUG
    UINT32	rdesc0;		/* desc0 status word */
#endif /* DC_DEBUG */


#ifdef DC_DEBUG
    rdesc0 = rmd->rDesc0;

    if (rdesc0 & PCISWAP(0x40000000))
        pDrvCtrl->errStats.rxFiltrErr++;
    
    if (rdesc0 & PCISWAP(0x00004000))
        pDrvCtrl->errStats.rxDescErr++;
        
    if  (rdesc0 & PCISWAP(RDESC0_RF))
        pDrvCtrl->errStats.rxRuntFrm++;

    if  (rdesc0 & PCISWAP(RDESC0_TL))
        pDrvCtrl->errStats.rxTooLong++;

    if  (rdesc0 & PCISWAP(RDESC0_CS))
        pDrvCtrl->errStats.rxCollision++;

    if  (rdesc0 & PCISWAP(0x00000008))
        pDrvCtrl->errStats.rxMiiErr++;

    if  (rdesc0 & PCISWAP(RDESC0_DB))
        pDrvCtrl->errStats.rxDribbleBit++;

    if  (rdesc0 & PCISWAP(RDESC0_CE))
        pDrvCtrl->errStats.rxCrcErr++;
#endif /* DC_DEBUG */

    /* Discard if we have errors */
    if  (rmd->rDesc0 & PCISWAP(RDESC0_ES))
        {
        ++pDrvCtrl->idr.ac_if.if_ierrors;       /* bump error stat */
        goto cleanRXD;
        }

    /* Process clean packets */
    ++pDrvCtrl->idr.ac_if.if_ipackets;		/* bump statistic */

    len = RDESC0_FL_GET(PCISWAP(rmd->rDesc0));	/* frame length */

    len -= 4; 	/* Frame length includes CRC in it so subtract it */

    /* Get pointer to packet */

    pEnetHdr = DC_CACHE_PHYS_TO_VIRT(PCI_TO_MEM_PHYS(PCISWAP(rmd->rDesc2)));

    DC_CACHE_INVALIDATE (pEnetHdr, len);   /* make the packet data coherent */

    /* call input hook if any */

    if ((etherInputHookRtn == NULL) || ((*etherInputHookRtn)
       (& pDrvCtrl->idr.ac_if, (char *) pEnetHdr, len)) == 0)
        {
        /* Adjust length to size of data only */

        len -= SIZEOF_ETHERHEADER;

        /* Get pointer to packet data */

        pData = ((u_char *) pEnetHdr) + SIZEOF_ETHERHEADER;

        ether_type = ntohs ( pEnetHdr->type );

        /* OK to loan out buffer ? -> build an mbuf cluster */
    
	if ((pDrvCtrl->nLoanRx > 0) && (USE_CLUSTER (len))
	    && (pMbuf = BUILD_CLUSTER (pDrvCtrl, pEnetHdr, pData, len)))
	    {
            pPhys = (u_long) DC_CACHE_VIRT_TO_PHYS(MEM_TO_PCI_PHYS(
		    (pDrvCtrl->lPool[--pDrvCtrl->nLoanRx])));
	    rmd->rDesc2 = PCISWAP(pPhys);
	    }
        else
            {
            if ((pMbuf = bcopy_to_mbufs (pData, len, 0, & pDrvCtrl->idr.ac_if,
                pDrvCtrl->memWidth)) == NULL)
		{
                ++pDrvCtrl->idr.ac_if.if_ierrors;	/* bump error stat */
                goto cleanRXD;
		}
            }

        /* send on up... */

        do_protocol_with_type (ether_type, pMbuf, &pDrvCtrl->idr, len);
        }

    /* Done with descriptor, clean up and give it to the device. */

cleanRXD:

    /* clear status bits and give ownership to device */

    rmd->rDesc0 = PCISWAP(RDESC0_OWN);

    /* Flush the write pipe */

    CACHE_PIPE_FLUSH ();

    /* Advance our management index */

    pDrvCtrl->rxIndex = (pDrvCtrl->rxIndex + 1) % pDrvCtrl->dcNumRds;

    return (OK);
    }

#ifdef BSD43_DRIVER
/*******************************************************************************
*
* dcOutput - driver output routine
*
* This routine is called by the stack to transmit an mbuf.
* This routine is called by the stack to transmit an mbuf.
*/

LOCAL int dcOutput
    (
    IDR  *	pIDR,		/* pointer to interface data record */
    MBUF *	pMbuf,		/* pointer to mbuf */
    SOCK *	pDest		/* pointer to destination sock */
    )
    {
    int 	unit = ((IFNET *)pIDR)->if_unit;
    DRV_CTRL *	pDrvCtrl = &drvCtrl[unit];
    int		errorVal;

    errorVal =  ether_output ((IFNET *)pIDR, pMbuf, pDest,
                               (FUNCPTR) dcStartOutput, pIDR);

    if ((errorVal == ENOBUFS) && !pDrvCtrl->txFlushScheduled)
        {
#ifdef DC_DEBUG
        pDrvCtrl->errStats.txDropped++;
#endif /* DC_DEBUG */
        dcTxFlush (pDrvCtrl);
        }
    
    return (errorVal);
    }
#endif /* BSD43_DRIVER */

/*******************************************************************************
*
* dcTxFlush - flush enqueued transmit buffers
*
*/

LOCAL void dcTxFlush
    (
    DRV_CTRL *	pDrvCtrl
    )
    {
    int		retry;
    int		delayCount;

#ifdef DC_DEBUG
    pDrvCtrl->errStats.txFlushCalled++;
    if (pDrvCtrl->idr.ac_if.if_snd.ifq_len)
        pDrvCtrl->errStats.txFlushNeeded++;
#endif /* DC_DEBUG */

    retry = DC_FLUSH_RETRIES;
    while (pDrvCtrl->idr.ac_if.if_snd.ifq_len && retry)
        {
        dcTxRingClean (pDrvCtrl);

        if (DC_TD_INDEX_NEXT(pDrvCtrl) != pDrvCtrl->txDiIndex)
            DC_START_OUTPUT (pDrvCtrl);
        else
            {
            for (delayCount=1000; delayCount--;)
                ;
            retry--;
            }
        }

#ifdef DC_DEBUG
    if (pDrvCtrl->idr.ac_if.if_snd.ifq_len == 0)
        pDrvCtrl->errStats.txFlushDone++;
#endif /* DC_DEBUG */

    pDrvCtrl->txFlushScheduled = FALSE;
    }
    
/*******************************************************************************
*
* dcStartOutput - send transmit buffers
*
*/

LOCAL void dcStartOutput
    (
#   ifdef BSD43_DRIVER
    int unit
#   else
    DRV_CTRL *	pDrvCtrl
#   endif /* BSD43_DRIVER */
    )
    {
#   ifdef BSD43_DRIVER    
    DRV_CTRL *	pDrvCtrl = &drvCtrl[unit];
#   endif /* BSD43_DRIVER */    

    DC_TDE *	pTxD;
    MBUF *	pMbuf;
    char * 	pTxdBuf;
    int		bufLen;
    int		retries;

#ifdef DC_DEBUG
    pDrvCtrl->errStats.txQsizeCur=pDrvCtrl->idr.ac_if.if_snd.ifq_len;
#endif /* DC_DEBUG */
    retries = MAX_TX_RETRIES;
    while (pDrvCtrl->idr.ac_if.if_snd.ifq_len && retries)
        {
        pTxD = pDrvCtrl->txRing + pDrvCtrl->txIndex;

        if  (((pTxD->tDesc0 & PCISWAP(TDESC0_OWN)) != 0) ||
             (DC_TD_INDEX_NEXT (pDrvCtrl) == pDrvCtrl->txDiIndex))
            {
            dcTxRingClean (pDrvCtrl);
#ifdef DC_DEBUG
            pDrvCtrl->errStats.txRetries++;
#endif /* DC_DEBUG */
            retries--;
            continue;
            }
        retries = MAX_TX_RETRIES;
        
        /* Get pointer to transmit buffer */

        pTxdBuf = (char *)DC_CACHE_PHYS_TO_VIRT(PCI_TO_MEM_PHYS(
        			PCISWAP(pTxD->tDesc2)));

        IF_DEQUEUE (&pDrvCtrl->idr.ac_if.if_snd, pMbuf);

        /* copy stuff */

        bcopy_from_mbufs (pTxdBuf, pMbuf, bufLen, pDrvCtrl->memWidth);

        pTxD->tDesc0  = 0;		/* clear buffer error status */
        pTxD->tDesc1 &= PCISWAP (~TDESC1_TBS1_MSK);
        pTxD->tDesc1 |= PCISWAP (TDESC1_TBS1_PUT(max (ETHERSMALL, bufLen)));
        pTxD->tDesc0  = PCISWAP (TDESC0_OWN);  /* give ownership to device */

        CACHE_PIPE_FLUSH ();		/* Flush the write pipe */

        /* Advance our management index */

        pDrvCtrl->txIndex = DC_TD_INDEX_NEXT (pDrvCtrl);

        /* Kickstart transmit */
    
        if (dcKickStartTx)
            DC_CSR_WRITE (pDrvCtrl->devAdrs, CSR1, CSR1_TPD);

        pDrvCtrl->txCount++;
        }

    /* Scrub the Tx ring */
    if (pDrvCtrl->txCount > (pDrvCtrl->dcNumTds/3))
        dcTxRingClean (pDrvCtrl);

#ifdef DC_DEBUG
    if (pDrvCtrl->errStats.txQsizeCur > pDrvCtrl->errStats.txQsizeMax)
        pDrvCtrl->errStats.txQsizeMax = pDrvCtrl->errStats.txQsizeCur;
#endif /* DC_DEBUG */
    }


/*******************************************************************************
*
* dcIoctl - the driver I/O control routine
*
* Process an ioctl request.
*/

LOCAL int dcIoctl
    (
    IDR  *	ifp,		/* pointer interface data record */
    int		cmd,		/* command */
    caddr_t	data		/* data */
    )
    {
    int 	error = 0;
    DRV_CTRL *  pDrvCtrl = (DRV_CTRL *)ifp;	/* pointer to device */

    switch (cmd)
        {
        case SIOCSIFADDR:
            ifp->ac_ipaddr = IA_SIN (data)->sin_addr;
            break;

        case SIOCSIFFLAGS:
            /* No further work to be done */
            break;

        case IFF_PROMISC:	/* set device in promiscuous mode */
	    DC_CSR_UPDATE (pDrvCtrl->devAdrs, CSR6, CSR6_PR);
	    break;

        default:
            error = EINVAL;
        }

    return (error);
    }

/*******************************************************************************
*
* dcChipReset - hardware reset of chip (stop it)
*/

LOCAL int dcChipReset
    (
    DRV_CTRL *	pDrvCtrl	/* pointer to device control structure */
    )
    {
    ULONG  	devAdrs = pDrvCtrl->devAdrs;	/* device base address */
    ULONG	ix;				/* index */
	
    DC_CSR_WRITE (devAdrs, CSR6, 0);		/* stop rcvr & xmitter */

    DC_CSR_WRITE (devAdrs, CSR7, 0);		/* mask interrupts */
    
    DC_CSR_WRITE (devAdrs, CSR0, CSR0_SWR);

    /* Wait Loop, Loop for at least 50 PCI cycles according to chip spec */

    for (ix = 0; ix < 0x1000; ix++)
	;

    /* any additional bus mode | give xmit & rcv process equal priority */
    DC_CSR_WRITE (devAdrs, CSR0, (dcCSR0Bmr | CSR0_BAR | CSR0_RML |
                                CSR0_CAL_32 | CSR0_PBL_32));

    for (ix = 0; ix < 0x1000; ix++)		/* loop for some cycles */
	;

    return (OK);
    }

/*******************************************************************************
*
* dc21040AuiTpInit - initialize either AUI or 10BASE-T connection
*
* This function configures 10BASE-T interface. If the link pass state is 
* not achieved within two seconds then the AUI interface is initialized.
*/

LOCAL void dc21040AuiTpInit
    (
    ULONG       devAdrs        /* device base I/O address */
    )
    {

    /* reset the SIA registers */

    DC_CSR_WRITE (devAdrs, CSR13, 0);
    DC_CSR_WRITE (devAdrs, CSR14, 0);
    DC_CSR_WRITE (devAdrs, CSR15, 0);

    /* configure for the 10BASE-T */

    DC_CSR_WRITE (devAdrs, CSR13, CSR13_CAC_CSR);	/* 10BT auto configuration */

    taskDelay (sysClkRateGet() * 2);		/* 2 second delay */

    if (DC_CSR_READ (devAdrs, CSR12) & (CSR12_LKF | CSR12_NCR))
	{
	/* 10BASE-T not connected initialize interface for AUI */

	DC_CSR_WRITE (devAdrs, CSR13, 0);		/* reset SIA registers */
	DC_CSR_WRITE (devAdrs, CSR14, 0);
	DC_CSR_WRITE (devAdrs, CSR15, 0);

	/* AUI auto configuration */

	DC_CSR_WRITE (devAdrs, CSR13, (CSR13_AUI_TP | CSR13_CAC_CSR));
	}
    }

/*******************************************************************************
*
* dc21140SROMVersionGet - gets the SROM format version
*
* RETURNS: 0 - Undefined/Unknown; 1 - Version 1/A; 3 - Version 3.0
*/

LOCAL UCHAR dc21140SROMVersionGet
    (
    ULONG devAdrs
    )
    {
    union SROMData
	{
        char    Char[6];        /* temporary 6 x Char */
        USHORT  Short[2];       /* temporary USHORT */
        ULONG   Long;           /* temporay ULONG */
       	} SROMData;

    /* Check if SROM is programmed. */

    SROMData.Short[0] = dcReadRom (devAdrs, 0);
    if (SROMData.Short[0] == 0xFFFF) 
	{
        return (0);
    	}
    SROMData.Short[0] = dcReadRom (devAdrs, 9);
    switch (SROMData.Char[1]) 	
	{
        case 1:
            return (1);
        case 3:
            return (3);
        default:
            break;
    	}
    return (0);
    }

/*******************************************************************************
*
* dcSetDefaultMediaTypes - initialize Media info for blank SROMs.
*
*
*/

LOCAL void dcSetDefaultMediaTypes
    (
    DRV_CTRL *pDrvCtrl
    ) 
    {
    pDrvCtrl->dcMediaBlocks.MediaFound = 3;
    pDrvCtrl->dcMediaBlocks.ActiveMedia = 0;
    pDrvCtrl->dcMediaBlocks.GPMask = 0x60;

    /* AUI */

    pDrvCtrl->dcMediaBlocks.MediaArray[0].ValCSR6 = CSR6_BIT25;
    pDrvCtrl->dcMediaBlocks.MediaArray[0].GPCount = 0;

    /* 10BaseT */

    pDrvCtrl->dcMediaBlocks.MediaArray[1].ValCSR6 = CSR6_PS | CSR6_BIT25;
    pDrvCtrl->dcMediaBlocks.MediaArray[1].GPCount = 1;
    pDrvCtrl->dcMediaBlocks.MediaArray[2].GPValue[0] = 0x40;

    /* 100BaseT4 */

    pDrvCtrl->dcMediaBlocks.MediaArray[2].ValCSR6 = CSR6_PS | CSR6_BIT25;
    pDrvCtrl->dcMediaBlocks.MediaArray[2].GPCount = 1;
    pDrvCtrl->dcMediaBlocks.MediaArray[2].GPValue[0] = 0x40;
    }

/*******************************************************************************
*
* dcDoCompactInfo - initialize Compact Media info for Version 3.0 SROMs.
*
*/

LOCAL void dcDoCompactInfo 
    (
    DRV_CTRL *pDrvCtrl, 
    CompactFormat *pCompactData, 
    UINT *pCSR6, UINT MediaUnit
    )
    {
    pDrvCtrl->dcMediaBlocks.MediaArray[MediaUnit].GPCount = 1;
    pDrvCtrl->dcMediaBlocks.MediaArray[MediaUnit].GPValue[0] = 
    	pCompactData->GPPortData;

    if (pCompactData->Command & 1)
	*pCSR6 |= CSR6_PS;

    if (pCompactData->Command & 10)
	*pCSR6 |= CSR6_TTM;

    if (pCompactData->Command & 20)
	*pCSR6 |= CSR6_PCS;

    if (pCompactData->Command & 40)
	*pCSR6 |= CSR6_SCR;

    return;
    }

/*******************************************************************************
*
* dc21140AuiMiiInit - initialize either AUI or MII connection
*
* This function configures 21140.  The SROM is used to set the chipset
* (DEC21140/PHY) if the SROM is configured.  The settings are tried and
* if there is no errors (link fail, no carrier, ...) the configuration
* is used.  If an error occurs, the next media info leaf is tried, etc.
* If the SROM is not programmed, or contains invalid data, a "generic"
* TP setting is used/tried and, if failure, the AUI port is used.
*/

LOCAL void dc21140AuiMiiInit
    (
    DRV_CTRL *pDrvCtrl,
    ULONG devAdrs, 
    UINT dcCSR6
    )
    {
    union SROMData 
	{
        char    Char[6];        /* temporary 6 x Char */
        USHORT  Short[2];       /* temporary USHORT */
        ULONG   Long;           /* temporay ULONG */
    	} SROMData;

    UINT BlockCount = 0, 
         GPRLength  = 0,
         OldCSR6    = 0, 
         ii, jj;
    UCHAR *pTmp;
    InfoLeaf *dcInfoLeaf;
    InfoBlock *pInfoBlock;
    UCHAR SROMBuffer [128];

    pDrvCtrl->dcMediaBlocks.DontSwitch = 0;
    dcCSR6 = dcCSR6 | CSR6_BIT25;
    OldCSR6 = dcCSR6;

    /* Get SROM version */

    switch (dc21140SROMVersionGet (devAdrs)) 
	{

	/* Valid Version 3 format was found, try info leafs. */

	case 3: 
	    for ( ii = 0; ii < 64; ii++ ) 
		{
		SROMData.Short[0] = dcReadRom (devAdrs, ii);
		SROMBuffer [ii*2] = SROMData.Char[1];
		SROMBuffer [(ii*2)+1] = SROMData.Char[0];
		}
	    dcInfoLeaf = (InfoLeaf *)(&(SROMBuffer[SROMBuffer[27]])); 
	    BlockCount = dcInfoLeaf->BlockCount;
	    pInfoBlock = &dcInfoLeaf->dcInfoBlock;
	    pDrvCtrl->dcMediaBlocks.MediaFound = BlockCount;
	    pDrvCtrl->dcMediaBlocks.GPMask = dcInfoLeaf->GPControl;  
	    for (ii = 0; ii < BlockCount; ii++) 
		{
		pTmp =  (UCHAR *)pInfoBlock;
		if ( !(pInfoBlock->Compact.MediaCode & 0x80) ) 
		    {

		    /* Compact Format */

                    pDrvCtrl->dcMediaBlocks.MediaArray[0].GPCount = 0;
		    dcDoCompactInfo(pDrvCtrl, &(pInfoBlock->Compact), 
				    &dcCSR6, ii);
		    pDrvCtrl->dcMediaBlocks.MediaArray[ii].ValCSR6 = dcCSR6;
		    pTmp = &(pTmp[4]);
		    }
		else 
		    {

		    /* Extended Format */

		    if (pInfoBlock->Extended0.Type) 
			{

			/* Block Type 1 */

			dcCSR6 = dcCSR6 | CSR6_BIT25 | CSR6_PS;

			/* Get General Purpose Data. */

			GPRLength = pInfoBlock->Extended1.BlockData[1];
			pDrvCtrl->dcMediaBlocks.MediaArray[ii].GPCount =
							GPRLength;
			for (jj = 0; jj < GPRLength; jj++) 
			    {
			    pDrvCtrl->dcMediaBlocks.MediaArray[ii].GPValue[jj] =
			    	pInfoBlock->Extended1.BlockData[jj+2];
			    }
			pDrvCtrl->dcMediaBlocks.MediaArray[ii].GPResetLen = 
				pInfoBlock->Extended1.BlockData[GPRLength+2];
			for (jj = 0; 
			     jj < pDrvCtrl->dcMediaBlocks.MediaArray[ii].
					GPResetLen; jj++) 
			    {
			    pDrvCtrl->dcMediaBlocks.MediaArray[ii].
			    	GPResetValue[jj] = 
			    	pInfoBlock->Extended1.
			    		BlockData[GPRLength+jj+3];
			    }
			}
		    else 
			{

			/* Block Type 0 */

			dcCSR6 = dcCSR6 | CSR6_TTM | CSR6_BIT25;
			dcDoCompactInfo (pDrvCtrl,
					(CompactFormat *)
					    (pInfoBlock->Extended1.BlockData), 
					&dcCSR6, ii);
			}
			pDrvCtrl->dcMediaBlocks.MediaArray[ii].ValCSR6 = dcCSR6;
			pTmp = &(pTmp[(pInfoBlock->Extended0.Length & ~0x80)
					+ 1]);
		    }

		pDrvCtrl->dcMediaBlocks.ActiveMedia = 0;
		dcCSR6 = OldCSR6;
		pInfoBlock = (InfoBlock *)pTmp;
		}

	    pDrvCtrl->dcMediaBlocks.ActiveMedia = BlockCount - 1;
	    dcSelectMedia (pDrvCtrl);
	    break;

	default:

	    /* Invalid format was found, try MII then AUI. */

	    dcSetDefaultMediaTypes (pDrvCtrl);
	    pDrvCtrl->dcMediaBlocks.ActiveMedia = 1;
	    dcSelectMedia (pDrvCtrl);
	    break;
	}
    return;
    }

/*******************************************************************************
*
* dcMiiPhyRead - read a PHY device register via MII
*
* RETURNS: the contents of a PHY device register.
*/

LOCAL USHORT dcMiiPhyRead
    (
    DRV_CTRL	*pDrvCtrl,
    UINT	phyAdrs,                     /* PHY address to access */
    UINT	phyReg                       /* PHY register to read */
    )
    {
    USHORT retVal=0;
    
    /* Write 34-bit preamble */
    DC_MII_WRITE (pDrvCtrl->devAdrs, MII_PREAMBLE, 32);
    DC_MII_WRITE (pDrvCtrl->devAdrs, MII_PREAMBLE, 2);

    /* start of frame + op-code nibble */
    DC_MII_WRITE (pDrvCtrl->devAdrs, MII_SOF | MII_RD, 4);

    /* device address */
    DC_MII_WRITE (pDrvCtrl->devAdrs, phyAdrs, 5);
    DC_MII_WRITE (pDrvCtrl->devAdrs, phyReg, 5);

    /* turn around */
    DC_MII_RTRISTATE (pDrvCtrl->devAdrs);

    /* read data */
    DC_MII_READ (pDrvCtrl->devAdrs, &retVal, 16);

    return (retVal);
    }

/*******************************************************************************
*
* dcMiiPhyWrite - write to a PHY device register via MII
*
* RETURNS: none
*/

LOCAL void dcMiiPhyWrite
    (
    DRV_CTRL	*pDrvCtrl,
    UINT	phyAdrs,                     /* PHY address to access */
    UINT	phyReg,                      /* PHY register to write */
    USHORT	data                         /* Data to write */
    )
    {
    /* write 34-bit preamble */
    DC_MII_WRITE (pDrvCtrl->devAdrs, MII_PREAMBLE, 32);
    DC_MII_WRITE (pDrvCtrl->devAdrs, MII_PREAMBLE, 2);

    /* start of frame + op-code nibble */
    DC_MII_WRITE (pDrvCtrl->devAdrs, MII_SOF | MII_WR, 4);

    /* device address */
    DC_MII_WRITE (pDrvCtrl->devAdrs, phyAdrs, 5);
    DC_MII_WRITE (pDrvCtrl->devAdrs, phyReg, 5);

    /* turn around */
    DC_MII_WTRISTATE (pDrvCtrl->devAdrs);

    /* write data */
    DC_MII_WRITE (pDrvCtrl->devAdrs, data, 16);
    }

/*******************************************************************************
*
* dcSelectMedia - select the next media type supported.
*
* RETURNS: none
*/

LOCAL void dcSelectMedia 
    (
    DRV_CTRL *pDrvCtrl
    )
    {
    UINT	csr6val;
    UINT	csr7val;
    UINT	jj;
    UINT	ix;
    UINT	phyAddr;
    USHORT	miiData;
    UINT	linkTry = 0;

    if (pDrvCtrl->dcMediaBlocks.DontSwitch) 
    	{
    	return;
    	}

    csr6val = DC_CSR_READ (pDrvCtrl->devAdrs, CSR6);
    csr7val = DC_CSR_READ (pDrvCtrl->devAdrs, CSR7);	/* save CSR7 */

    /* Stop Receive/Transmit Processes */

    DC_CSR_WRITE (pDrvCtrl->devAdrs, CSR6, (~(CSR6_ST|CSR6_SR) & csr6val)); 
    taskDelay (sysClkRateGet() * 1);

    if(pDrvCtrl->dcMediaBlocks.MediaArray[pDrvCtrl->
	dcMediaBlocks.ActiveMedia].GPCount) 
	{
	for (jj=0; jj < pDrvCtrl->dcMediaBlocks.
	    MediaArray[pDrvCtrl->dcMediaBlocks.ActiveMedia].GPResetLen; jj++) 
	    {
	    DC_CSR_WRITE (pDrvCtrl->devAdrs, CSR12,
			pDrvCtrl->dcMediaBlocks.
			MediaArray[pDrvCtrl->dcMediaBlocks.ActiveMedia].
			GPResetValue[jj]);
	    DC_CSR_WRITE (pDrvCtrl->devAdrs, CSR12, 
			pDrvCtrl->dcMediaBlocks.GPMask | 0x100);
	    for (ix = 0; ix < 0x1000; ix++)
		;
	    }
	for (jj=0; jj < pDrvCtrl->dcMediaBlocks.
	    MediaArray[pDrvCtrl->dcMediaBlocks.
	    ActiveMedia].GPCount; jj++) 
	    {
	    DC_CSR_WRITE (pDrvCtrl->devAdrs, CSR12,
			pDrvCtrl->dcMediaBlocks.
			MediaArray[pDrvCtrl->
			dcMediaBlocks.
			ActiveMedia].
			GPValue [jj]);
	    DC_CSR_WRITE (pDrvCtrl->devAdrs, CSR12, 
			pDrvCtrl->dcMediaBlocks.GPMask | 0x100);
	    for (ix = 0; ix < 0x1000; ix++)
		;
	    }
        
            /* Find first PHY attached to DEC21140 */

            for (phyAddr = 0; phyAddr < DC_MAX_PHY; phyAddr++) 
                {
                miiData = dcMiiPhyRead(pDrvCtrl, phyAddr, MII_PHY_ID0);
                if (miiData != 0xFFFF) /* Found PHY */
                    break;
                }

            if (miiData != 0xFFFF)
                {
                linkTry=0;
                miiData=0;

                while ( !(miiData & MII_PHY_SR_LNK) &&
                        (linkTry < DC_MAX_LINK_TOUT))
                    {
                    taskDelay (sysClkRateGet() * 1);	/* 1 second delay */
                    miiData = dcMiiPhyRead(pDrvCtrl, phyAddr, MII_PHY_SR);
                    linkTry++;
                    }

                if (linkTry >= DC_MAX_LINK_TOUT)
                    {
                    /* Setup next media to try */
                    
                    if ((pDrvCtrl->dcMediaBlocks.ActiveMedia >= 
	                pDrvCtrl->dcMediaBlocks.MediaFound) ||
	                (pDrvCtrl->dcMediaBlocks.ActiveMedia == 0))
                        {
                        /*
                         * This is the last media supported.
                         * Leave chip programmed this way.
                         */
	                pDrvCtrl->dcMediaBlocks.DontSwitch = 1;
                        }
                    else
                        {
                        /* Attempt next media type */
                        
                        pDrvCtrl->dcMediaBlocks.ActiveMedia -= 1;
		        dcSelectMedia (pDrvCtrl);
                        return;
                        }
                    }
                }
            else /* don't know PHY type, delay 2 seconds */
                {
                taskDelay (sysClkRateGet() * 2);	/* 2 second delay */
                }
         }

    /*
     * Set Capture Effect Enable and Transmit Threshold to 160 by
     * default.
     */

    DC_CSR_WRITE (pDrvCtrl->devAdrs, CSR6, pDrvCtrl->dcMediaBlocks.
		MediaArray[pDrvCtrl->dcMediaBlocks.ActiveMedia].ValCSR6 | 
		CSR6_THR_160 | CSR6_BIT25 | CSR6_CAE | 0x80000000);

    DC_CSR_WRITE (pDrvCtrl->devAdrs, CSR0, 
        DC_CSR_READ (pDrvCtrl->devAdrs, CSR0) | CSR0_SWR);

    taskDelay (sysClkRateGet() * 1);		/* 1 second delay */

    DC_CSR_WRITE (pDrvCtrl->devAdrs, CSR0, 
        CSR0_BAR | CSR0_RML | CSR0_CAL_32 | CSR0_PBL_32);

    /*
     * Set Capture Effect Enable and Transmit Threshold to 160 by
     * default.
     */

    DC_CSR_WRITE (pDrvCtrl->devAdrs, CSR6, pDrvCtrl->dcMediaBlocks.
		MediaArray[pDrvCtrl->dcMediaBlocks.ActiveMedia].ValCSR6 | 
		CSR6_THR_160 | CSR6_BIT25 | CSR6_CAE | 0x80000000);

    DC_CSR_WRITE (pDrvCtrl->devAdrs, CSR7, csr7val); 	/* restore CSR7 */

    /* Restore transmit and receiver process */

    csr6val &= (CSR6_SR | CSR6_ST);
    csr6val |= DC_CSR_READ (pDrvCtrl->devAdrs, CSR6);
    DC_CSR_WRITE (pDrvCtrl->devAdrs, CSR6, csr6val);

    /* Setup next media to try */

    if ((pDrvCtrl->dcMediaBlocks.ActiveMedia >= 
	pDrvCtrl->dcMediaBlocks.MediaFound) ||
	(pDrvCtrl->dcMediaBlocks.ActiveMedia == 0))
	pDrvCtrl->dcMediaBlocks.DontSwitch = 1;

    pDrvCtrl->dcMediaBlocks.ActiveMedia -= 1;

    return;
    }

/*******************************************************************************
*
* dcCsrWrite - select and write a CSR register
*
*/

LOCAL void dcCsrWrite
    (
    ULONG 	devAdrs,		/* device address base */
    int 	reg,			/* register to select */
    ULONG 	value			/* value to write */
    )
    {
    ULONG *	csrReg;

    csrReg = (ULONG *)(devAdrs + (reg * DECPCI_REG_OFFSET));

    /* write val to CSR */

    *(csrReg) = PCISWAP(value);
    }

/*******************************************************************************
*
* dcCsrRead - select and read a CSR register
*
*/

LOCAL ULONG dcCsrRead
    (
    ULONG	devAdrs,		/* device address base */
    int		reg			/* register to select */
    )
    {
    ULONG *	csrReg;			/* csr register */
    ULONG	csrData;		/* data in csr register */

    csrReg = (ULONG *)(devAdrs + (reg * DECPCI_REG_OFFSET));

    csrData = *csrReg;

    /* get contents of CSR */

    return (PCISWAP(csrData));
    }

/*******************************************************************************
*
* dcRestart - restart the device after a fatal error
*
* This routine takes care of all the messy details of a restart.  The device
* is reset and re-initialized.  The driver state is re-synchronized.
*/

LOCAL void dcRestart
    (
    int 	unit		/* unit to restart */
    )
    {
    ULONG	status;		/* status register */	
    int		ix;		/* index variable */
    DC_RDE *	rmd;		/* pointer to receive descriptor */
    DC_TDE *	tmd;		/* pointer to Xmit descriptor */
    DRV_CTRL *	pDrvCtrl = & drvCtrl [unit];

    rmd = pDrvCtrl->rxRing;      /* receive ring */
    tmd = pDrvCtrl->txRing;      /* transmit ring */

    pDrvCtrl->idr.ac_if.if_flags &= ~(IFF_UP | IFF_RUNNING );
    logMsg ("dc%d -- Restarted\n", unit,0,0,0,0,0);

    status = DC_CSR_READ (pDrvCtrl->devAdrs, CSR6);
    DC_CSR_WRITE (pDrvCtrl->devAdrs, CSR6, 0);	/* reset mode register */

    for (ix = 0; ix < pDrvCtrl->dcNumRds; ix++, rmd++)
        {
	/* buffer size */

	rmd->rDesc1 = PCISWAP(RDESC1_RBS1_VAL(DC_BUFSIZ) | 
			      RDESC1_RBS2_VAL(0));

	if (ix == (pDrvCtrl->dcNumRds - 1)) /* if its is last one */
	rmd->rDesc1 |= PCISWAP(RDESC1_RER); /* end of receive ring */

	rmd->rDesc0 = PCISWAP(RDESC0_OWN); /* give ownership to lance */
        }

    for (ix = 0; ix < pDrvCtrl->dcNumTds; ix++, tmd++)
        {
	tmd->tDesc1 = PCISWAP((TDESC1_TBS1_PUT(0) |
			       TDESC1_TBS2_PUT(0) |
			       TDESC1_IC	  | 	/* intrpt on xmit */
			       TDESC1_LS          |	/* last segment */
			       TDESC1_FS));		/* first segment */

	if (ix == (pDrvCtrl->dcNumTds - 1))	/* if its is last one */
	    tmd->tDesc1 |= PCISWAP(TDESC1_TER);	/* end of Xmit ring */

	tmd->tDesc0 = 0 ;			/* clear status */
        }

    /* clear the status register */

    DC_CSR_WRITE (pDrvCtrl->devAdrs, CSR5, 0xffffffff);

    DC_CSR_WRITE (pDrvCtrl->devAdrs, CSR6, status);	/* restore mode */

    pDrvCtrl->idr.ac_if.if_flags |= (IFF_UP | IFF_RUNNING | IFF_NOTRAILERS);
    }

/*******************************************************************************
*
* dcLoanFree - return the given buffer to loaner pool
*
* This routine returns <pRxBuf> to the pool of available loaner buffers.
* It also returns <pRef> to the pool of available loaner reference counters,
* then zeroes the reference count.
*
* RETURNS: N/A
*/
 
LOCAL void dcLoanFree
    (
    DRV_CTRL *	pDrvCtrl,	/* pointer to device control structure */
    char *	pRxBuf,		/* pointer to receive buffer to free */
    UINT8 *	pRef		/* pointer to reference count */
    )
    {
    /* return loaned buffer to pool */
 
    pDrvCtrl->lPool[pDrvCtrl->nLoanRx] = pRxBuf;
 
    /* return loaned reference count to pool */
 
    pDrvCtrl->pRefCnt[pDrvCtrl->nLoanRx++] = pRef;

    /* reset reference count - should have been done from above, but... */
 
    *pRef = 0;
    }

/*******************************************************************************
*
* dcEnetAddrGet - gets the ethernet address from the ROM register.
*
* This routine gets the ethernet address from the ROM register.
* This routine returns the ethernet address into the pointer supplied to it.
* 
* RETURNS: OK/ERROR
*/

LOCAL STATUS dcEnetAddrGet
    (
    ULONG	devAdrs,	/* device base I/O address */
    char * 	enetAdrs,	/* pointer to the ethernet address */ 
    int		len		/* number of bytes to read */
    )
    {
    FAST ULONG	csr9Value;		/* register to hold CSR9 */
    int		ix;			/* index register */
    BOOL	eRomReady = FALSE;	/* ethernet ROM register state */

    DC_CSR_WRITE (devAdrs, CSR9, 0);	/* reset rom pointer */

    while (len > 0)
	{
	for (ix = 0; ix < 10; ix++)	/* try at least 10 times */
	    {
	    if ((csr9Value = DC_CSR_READ (devAdrs, CSR9)) & CSR9_040_DNVAL)
		{
		eRomReady = FALSE;
	    	DELAY(500);
		}
	    else
		{
		*enetAdrs++ = (UCHAR) csr9Value;
		len--;
		eRomReady = TRUE;
		break;
		}
  	    }
        
        if (!eRomReady)
            break;
	}

    if (!eRomReady)
    	return (ERROR);
    else
	return (OK);
    }

/*******************************************************************************
*
* dc21140EnetAddrGet - gets the ethernet address from the ROM register 
*
* This routine gets the last two bytes of the ethernet address from the ROM
* register and concantenates this value to the predefined values for DEC PMCs..
* This routine returns the ethernet address into the pointer supplied to it.
*
* The procedure could be expanded in the future to allow for modification of
* Ethernet addresses in the serial ROM.  It will have to be modified to
* accomodate the differences in the onboard 100Mb/s ethernet on future 
* Motorola products.
* 
* RETURNS: OK/ERROR
*/

LOCAL STATUS dc21140EnetAddrGet
    (
    ULONG	devAdrs,	/* device base I/O address */
    char * 	enetAdrs,	/* pointer to the ethernet address */ 
    int		len		/* number of bytes to read */
    )
    {
    int		i;		/* index register */
    int		ix;		/* index register */
    int		tmpInt;		/* temporary int */
    union SROMData
	{
	char	Char[6];	/* temporary 6 x Char */
	USHORT	Short[2];	/* temporary USHORT */
	ULONG   Long;		/* temporay ULONG */
	} SROMData;

    /* Check if SROM is programmed. */

    SROMData.Short[0] = dcReadRom (devAdrs, 0);
    if (SROMData.Short[0] == 0xFFFF) 
	{
	/* SROM is unprogrammed, get MAC address from NVRAM. */

	sysNvRamGet ((char *)enetAdrs, 6, ((int) ((char *)(0x1f2c)) - 256));
	return (OK);
	}
    else 
	{
	/* Look for Legacy Format in ROM. */

	SROMData.Short[0] = dcReadRom (devAdrs, 13);
	if (SROMData.Short[0] == 0xAA55) 
	    {

	    /*
	     * Read from location 0 in ROM.  MAC address appears at
	     * location 0 in Legacy SROM formats.
	     */

	    tmpInt = 0;
	    }
	else 
	    {

	    /*
	     * Read from location 20 in ROM.  MAC address appears at
	     * location 20 (10 byte pairs) in both Version 1/A and 3.0 
	     * SROM formats.
	     */

	    tmpInt = DEC_PMC_POS;
	    }
	for (i = tmpInt, ix = 0; i<(3+tmpInt); i++, ix++) 
	    {
	    if ((UINT)(SROMData.Short[0] = dcReadRom (devAdrs, i)) == ERROR) 
		{
		return (ERROR);
		}
	    enetAdrs[ix++] = SROMData.Char[1];
	    enetAdrs[ix] = SROMData.Char[0];
	    }
	}
    tmpInt = -1;
    return (OK);
    }

/*******************************************************************************
*
* dcFltrFrmXmit - Transmit the setup filter frame.
*
* This routine transmits the setup filter frame.
* The setup frame is not transmitted actually over the wire. The setup frame
* which is transmitted is 192 bytes. The tranmitter should be in an on state
* before this function is called. This call has been coded so that the 
* setup frame can be transmitted after the chip is done with the 
* intialization taking into consideration for adding multicast support.
* 
* RETURNS: OK/ERROR
*/

LOCAL int dcFltrFrmXmit
    (
    DRV_CTRL *	pDrvCtrl, 	/* pointer to device control structure */
    char * 	pPhysAdrsTbl, 	/* pointer to physical address table */ 
    int 	tblLen		/* size of the physical address table */
    )
    {
    DC_TDE *	tmd;		/* pointer to Tx ring descriptor */
    ULONG * 	pBuff;		/* pointer to the Xmit buffer */
    int		status = OK;	/* intialize status as OK */
    ULONG	csr7Val;	/* value in CSR7 */

    semTake (pDrvCtrl->TxSem, WAIT_FOREVER);

    if (dcFltrFrmSetup (pDrvCtrl, pPhysAdrsTbl, tblLen) != OK)
	{
	status = ERROR;		/* not able to set up filter frame */
	goto setupDone;		/* set up done */
	}

    /* See if next TXD is available */

    tmd = pDrvCtrl->txRing + pDrvCtrl->txIndex;

    DC_CACHE_INVALIDATE (tmd, TMD_SIZ);

    if  (
        ((tmd->tDesc0 & PCISWAP(TDESC0_OWN)) != 0) ||
        (
        ((pDrvCtrl->txIndex + 1) % pDrvCtrl->dcNumTds) == pDrvCtrl->txDiIndex
	)
        )
	{
	status = ERROR;		/* not able to set up filter frame */
	goto setupDone;		/* set up done */
	}

    /* Get pointer to transmit buffer */

    pBuff = DC_CACHE_PHYS_TO_VIRT(PCI_TO_MEM_PHYS(PCISWAP(tmd->tDesc2)));

    /* copy into Xmit buffer */

    bcopyLongs ((char *)pDrvCtrl->pFltrFrm, (char *)pBuff, 
		FLTR_FRM_SIZE_ULONGS);		

    tmd->tDesc0 = 0;				/* clear buffer error status */
    tmd->tDesc1 |= PCISWAP(TDESC1_SET);		/* frame type as set up */
    tmd->tDesc1 &= PCISWAP(~TDESC1_TBS1_MSK);
    tmd->tDesc1 |= PCISWAP(TDESC1_TBS1_PUT(FLTR_FRM_SIZE));
    tmd->tDesc0 = PCISWAP(TDESC0_OWN); 		/* give ownership to device */

    CACHE_PIPE_FLUSH ();			/* Flush the write pipe */

    /* Advance our management index */

    pDrvCtrl->txIndex = (pDrvCtrl->txIndex + 1) % pDrvCtrl->dcNumTds;
    pDrvCtrl->txDiIndex = (pDrvCtrl->txDiIndex + 1) % pDrvCtrl->dcNumTds;

    csr7Val = DC_CSR_READ (pDrvCtrl->devAdrs, CSR7);

    DC_CSR_WRITE (pDrvCtrl->devAdrs, CSR7, 0);	/* mask interrupts */
    
    if (dcKickStartTx)
    	DC_CSR_WRITE (pDrvCtrl->devAdrs, CSR1, CSR1_TPD);   /* xmit poll demand */

    /* wait for the own bit to change */

    while (tmd->tDesc0 & PCISWAP (TDESC0_OWN))
	/* do nothing */ ;

    tmd->tDesc0 = 0;				/* clear status bits */
    tmd->tDesc1 &= PCISWAP(~TDESC1_SET);

    DC_CSR_WRITE (pDrvCtrl->devAdrs, CSR7, csr7Val); /* restore value */
	
setupDone:

    /* Release exclusive access. */

    semGive (pDrvCtrl->TxSem);

    return (status);
    }

/*******************************************************************************
*
* dcFltrFrmSetup - set up the filter frame.
*
* This routine sets up the filter frame to filter the physical addresses
* on the incoming frames. The setup filter frame buffer is 192 bytes. This
* setup frame needs to be transmitted before transmitting and receiving 
* any frames. 
* 
* RETURNS: OK/ERROR
*/

LOCAL int dcFltrFrmSetup
    (
    DRV_CTRL *	pDrvCtrl, 	/* pointer to device control structure */
    char * 	pPhysAdrsTbl, 	/* pointer to physical address table */ 
    int 	tblLen		/* size of the physical address table */
    )
    {
    int		ix;		/* index variable */
    int		jx; 		/* inner index variable */
    ULONG *	pFltrFrm;	/* pointer to the filter frame */
    USHORT *	pPhysAddrs;	/* pointer to the physical addresses */

    if (tblLen > FLTR_FRM_ADRS_NUM)
	return (ERROR);

    pFltrFrm = pDrvCtrl->pFltrFrm;

    for (ix = 0; ix < FLTR_FRM_SIZE_ULONGS; ix++)
	*pFltrFrm++ = FLTR_FRM_DEF_ADRS;

    pFltrFrm = pDrvCtrl->pFltrFrm;
    pPhysAddrs = (USHORT *) (pPhysAdrsTbl);

    for (ix = 0; ix < tblLen; ix++)
	{
	for (jx = 0; jx < (FLTR_FRM_ADRS_SIZE/sizeof (USHORT)); jx++)
	    {
#if (_BYTE_ORDER == _BIG_ENDIAN)
	    *pFltrFrm =  (*pPhysAddrs << 16);
#else
	    *pFltrFrm = ((LSB(*pPhysAddrs) << 8) | MSB(*pPhysAddrs));
#endif
	    pPhysAddrs++;
	    pFltrFrm++;
	    }
	}
    return (OK);
    }

/*******************************************************************************
*
* dcReadRom - reads an specified entry in the Serial ROM 
*
* This routine uses the line number passed to the function and returns
* the two bytes of information that is associated with it.  This will later
* be used by the dc21140GetEthernetAdr function.  It can also be used to 
* review the ROM contents itself.
*
* The function must first send some initial bit paterns to the CSR9 which 
* contains the Serial ROM Control bits.  Then the line index into the ROM
* will be evaluated bit by bit to program the ROM.  The 2 bytes of data
* will be extracted and processed into a normal pair of bytes. 
* 
* RETURNS: Short Value in ROM/ERROR
*/

LOCAL USHORT dcReadRom
    (
    ULONG	devAdrs,	/* device base I/O address */
    UCHAR	lineCnt 	/* Serial ROM line Number */ 
    )
    {
    int		ix;		/* index register */
    int		intCnt;		/* address loop count */
    USHORT	tmpShort;
    ULONG	tmpLong; 

    /* Is the line offset valid, and if so, how large is it */

    if (lineCnt > SROM_SIZE)
    	{
	logMsg ("dcReadRom FAILED, bad lineCnt\n",0,0,0,0,0,0);
    	return (ERROR);
    	}
    if (lineCnt < 64)
    	{
	intCnt = 6;
	lineCnt = lineCnt << 2;  /* Prepare lineCnt for processing */
    	}
    else
	intCnt = 8;

    /* Command the Serial ROM to the correct Line */

    /* Preamble of Command */

    DC_SROM_WRITE (devAdrs, 0x00000000, 30);	/* Command 1 */
    DC_SROM_WRITE (devAdrs, 0x00000001, 50);	/* Command 2 */
    DC_SROM_WRITE (devAdrs, 0x00000003, 250);	/* Command 3 */
    DC_SROM_WRITE (devAdrs, 0x00000001, 100);	/* Command 4 */

    /* Command Phase */

    DC_SROM_WRITE (devAdrs, 0x00000005, 150);	/* Command 5 */
    DC_SROM_WRITE (devAdrs, 0x00000007, 250);	/* Command 6 */
    DC_SROM_WRITE (devAdrs, 0x00000005, 250);	/* Command 7 */
    DC_SROM_WRITE (devAdrs, 0x00000007, 250);	/* Command 8 */
    DC_SROM_WRITE (devAdrs, 0x00000005, 100);	/* Command 9 */
    DC_SROM_WRITE (devAdrs, 0x00000001, 150);	/* Command 10 */
    DC_SROM_WRITE (devAdrs, 0x00000003, 250);	/* Command 11 */
    DC_SROM_WRITE (devAdrs, 0x00000001, 100);	/* Command 12 */
    
    /* Address Phase */

    for (ix = 0; ix < intCnt; ix++)
    	{
	tmpLong = (lineCnt & 0x80) >> 5; /* Extract and move bit to bit 3) */

	/* Write the command */

    	DC_SROM_WRITE (devAdrs, tmpLong | 0x00000001, 150);  /* Command 13 */
    	DC_SROM_WRITE (devAdrs, tmpLong | 0x00000003, 250);  /* Command 14 */
    	DC_SROM_WRITE (devAdrs, tmpLong | 0x00000001, 100);  /* Command 15 */

	lineCnt <<= 1;  /* Adjust significant address bit */
    	}

    NSDELAY (150);

    /* Data Phase */

    tmpShort =0;
    for (ix = 15; ix >= 0; ix--)
    	{
	/* Write the command */

    	DC_SROM_WRITE (devAdrs, 0x00000003, 100);		/* Command 16 */

	/* Extract the data */

    	tmpShort |= DC_SROM_READ (devAdrs) << ix; 		/* Command 17 */
    	NSDELAY (150);
        
    	DC_SROM_WRITE (devAdrs, 0x00000001, 250);		/* Command 18 */
    	}

    /* Finish up command */

    DC_SROM_WRITE (devAdrs, 0x00000000, 100);	/* Command 19 */

    return (tmpShort);
    }

/*******************************************************************************
* 
* dcReadAllRom - read entire serial rom
*
* Function to read all of serial rom and store the data in the
* data structure passed to the function.  The count value will
* indicate how much of the serial rom to read.  The routine with also
* swap the the bytes as the come in. 
*
* RETURNS: N/A.
*/

void dcReadAllRom
    (
    ULONG	devAdrs,	/* device base I/O address */
    UCHAR	*buffer,	/* destination bufferr */ 
    int		cnt             /* Amount to extract in bytes */
    )
    {

    USHORT total, index;
    union tmp_type {
        USHORT Short;
    	char   Byte [2];	/* temporary 2 by char */
    } tmp;

    total = 0;
    for (index = 0, total = 0; total < cnt / 2; index += 2, total++)
    	{
	tmp.Short = dcReadRom (devAdrs, total);
	buffer[index] = tmp.Byte[1];
	buffer[index+1] = tmp.Byte[0];
    	}
    }

#ifdef DC_DEBUG

/*******************************************************************************
*
* dcViewRom - display lines of serial ROM for dec21140
*
* RETURNS: Number of bytes displayed.
*/

int dcViewRom
    (
    ULONG	devAdrs,	/* device base I/O address */
    UCHAR	lineCnt,	/* Serial ROM line Number */ 
    int		cnt             /* Amount to display */
    )
    {

    USHORT total;
    union tmp_type 
    	{
        USHORT Short;
    	char   Byte [2];	/* temporary 2 by char */
    	} tmp;

    if (((lineCnt / 2) < 0) || ((lineCnt / 2) > (UCHAR) SROM_SIZE))
	return (ERROR);

    total = 0;
    while (((lineCnt / 2) < SROM_SIZE) && (cnt > 0))
    	{
	if (!(total % 16))
	    printf ("\nValues for line %2d =>0x  ", total);
	tmp.Short = dcReadRom (devAdrs, lineCnt);
	printf ("%4x ",tmp.Short);
	lineCnt++ ;
	total += 2;
	cnt -= 2;
	}	
    printf ("\n");
    return (total);
    }


/*******************************************************************************
*
* dcCsrShow - display dec 21040/21140 status registers 0 thru 15
*
* Display the 16 registers of the DEC 21140 device on the console.
* Each register is printed in hexadecimal format.
*
* RETURNS: N/A.
*/

int dcCsrShow
    (
    int unit
    )
    {
    DRV_CTRL	*pDrvCtrl;
    int		devAdrs;

    if (unit > MAX_UNITS)
        return -1;

    pDrvCtrl = &drvCtrl[unit];

    if (!pDrvCtrl->attached)
        return -2;

    devAdrs=pDrvCtrl->devAdrs;
    
    printf ("dc%d Configuration Registers: \n", unit);
    printf (" 0=0x%x, 1=0x%x, 2=0x%x, 3=0x%x\n",
            (int) DC_CSR_READ (devAdrs, CSR0),
            (int) DC_CSR_READ (devAdrs, CSR1),
            (int) DC_CSR_READ (devAdrs, CSR2),
            (int) DC_CSR_READ (devAdrs, CSR3));
    printf (" 4=0x%x, 5=0x%x, 6=0x%x, 7=0x%x\n",
            (int) DC_CSR_READ (devAdrs, CSR4),
            (int) DC_CSR_READ (devAdrs, CSR5),
            (int) DC_CSR_READ (devAdrs, CSR6),
            (int) DC_CSR_READ (devAdrs, CSR7));
    printf (" 8=0x%x, 9=0x%x, 10=0x%x, 11=0x%x\n",
            (int) DC_CSR_READ (devAdrs, CSR8),
            (int) DC_CSR_READ (devAdrs, CSR9),
            (int) DC_CSR_READ (devAdrs, CSR10),
            (int) DC_CSR_READ (devAdrs, CSR11));
    printf (" 12=0x%x, 13=0x%x, 14=0x%x, 15=0x%x\n",
            (int) DC_CSR_READ (devAdrs, CSR12),
            (int) DC_CSR_READ (devAdrs, CSR13),
            (int) DC_CSR_READ (devAdrs, CSR14),
            (int) DC_CSR_READ (devAdrs, CSR15));
    return 0;
    }

int dcShow
    (
    UINT unit
    )
    {
    DRV_CTRL *pDrvCtrl;
    DC_STATS *pErrStats;

    if (unit > MAX_UNITS)
        return -1;

    pDrvCtrl = &drvCtrl[unit];

    if (!pDrvCtrl->attached)
        return -2;

    pErrStats=&pDrvCtrl->errStats;

    printf ("dc%d: pDrvCtrl=0x%x\n", unit, (int)pDrvCtrl);
    printf (" ivec=%d ilevel=%d devAdrs=0x%x pciMemBase=0x%x memWidth=%d\n",
            (int) pDrvCtrl->ivec,
            (int) pDrvCtrl->ilevel,
            (int) pDrvCtrl->devAdrs,
            (int) pDrvCtrl->pciMemBase,
            (int) pDrvCtrl->memWidth);
    printf (" nTDs=%d nRDs=%d rxIndex=%d txIndex=%d nLoanRx=%d\n",
            (int) pDrvCtrl->dcNumTds,
            (int) pDrvCtrl->dcNumRds,
            (int) pDrvCtrl->rxIndex,
            (int) pDrvCtrl->txIndex,
            (int) pDrvCtrl->nLoanRx);
    printf (" ipackets=%d, opackets=%d ierrors=%d oerrors=%d collisions=%d\n",
            (int) pDrvCtrl->idr.ac_if.if_ipackets,
            (int) pDrvCtrl->idr.ac_if.if_opackets,
            (int) pDrvCtrl->idr.ac_if.if_ierrors,
            (int) pDrvCtrl->idr.ac_if.if_oerrors,
            (int) pDrvCtrl->idr.ac_if.if_collisions);

    printf (" len=%d maxQlen=%d drops=%d txIndex=%d txDiIndex=%d\n",
            pDrvCtrl->idr.ac_if.if_snd.ifq_len,
            pDrvCtrl->idr.ac_if.if_snd.ifq_maxlen,
            pDrvCtrl->idr.ac_if.if_snd.ifq_drops,
            pDrvCtrl->txIndex,
            pDrvCtrl->txDiIndex);

    printf (" txstats: jbr=%d lostCarr=%d noCarr=%d lateColl=%d XsColl=%d\n",
            pErrStats->txJbrTmo,
            pErrStats->txLostCarrier,
            pErrStats->txNoCarrier,
            pErrStats->txLateCollision,
            pErrStats->txExsCollision);
    
    printf ("          hbFail=%d lnkF=%d uflo=%d defr=%d retries=%d\n",
            pErrStats->txHeartBeatFail,
            pErrStats->txLinkFail,
            pErrStats->txUnderflow,
            pErrStats->txDeferred,
            pErrStats->txRetries);

    printf ("          drops=%d fl=%d fl_need=%d fl_done=%d QMax=%d QCur=%d\n",
            pErrStats->txDropped,     
            pErrStats->txFlushCalled,
            pErrStats->txFlushNeeded,
            pErrStats->txFlushDone, 
            pErrStats->txQsizeMax,   
            pErrStats->txQsizeCur);    
    
    printf (" rxstats: fltErr=%d DescErr=%d Runt=%d tooLng=%d Coll=%d\n",
            pErrStats->rxFiltrErr, 
            pErrStats->rxDescErr,  
            pErrStats->rxRuntFrm,   
            pErrStats->rxTooLong,   
            pErrStats->rxCollision);

    printf ("          dribl=%d crc=%d ovflo=%d miiErr=%d miss=%d rxlatn=%d\n",
            pErrStats->rxDribbleBit,
            pErrStats->rxCrcErr,
            pErrStats->rxOverflow,
            pErrStats->rxMiiErr,
            pErrStats->rxMissed,
            pErrStats->rxLatency);    

            
    return 0;
    }

void phyProbe
    (
    DRV_CTRL	*pDrvCtrl
    )
    {
    int		phyAdrs;

    for (phyAdrs=0; phyAdrs < DC_MAX_PHY; phyAdrs++)
        {
        printf ("%d = 0x%x ", phyAdrs,
                dcMiiPhyRead (pDrvCtrl, phyAdrs, MII_PHY_ID0));
        }
    }

void phyShow
    (
    DRV_CTRL	*pDrvCtrl,
    int		phyNum
    )
    {
    int reg;

    printf ("CR=0x%x SR=0x%x IDO=0x%x ID1=0x%x\n",
            dcMiiPhyRead (pDrvCtrl, phyNum, MII_PHY_CR),
            dcMiiPhyRead (pDrvCtrl, phyNum, MII_PHY_SR),
            dcMiiPhyRead (pDrvCtrl, phyNum, MII_PHY_ID0),
            dcMiiPhyRead (pDrvCtrl, phyNum, MII_PHY_ID1));
    printf ("ANA=0x%x ANLPA=0x%x ANE=0x%x ANP=0x%x\n",
            dcMiiPhyRead (pDrvCtrl, phyNum, MII_PHY_ANA),
            dcMiiPhyRead (pDrvCtrl, phyNum, MII_PHY_ANLPA),
            dcMiiPhyRead (pDrvCtrl, phyNum, MII_PHY_ANE),
            dcMiiPhyRead (pDrvCtrl, phyNum, MII_PHY_ANP));
    }

#endif /* DC_DEBUG */

