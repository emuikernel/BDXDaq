/* dec21x4xEnd.c - END style DEC 21x4x PCI Ethernet network interface driver */

/* Copyright 1998 Wind River Systems, Inc. */

#include "copyright_wrs.h"

/*
modification history
--------------------
02d,29mar99,dat  SPR 26040, changed BIG_ENDIAN issue
02c,29mar99,dat  SPR 26119, fixed .bS/.bE calls
02b,14sep98,kla  modified pci calls for new library.	(SPR 24794)
02a,11sep98,kla  modified from ebsa285 BSP, for pc486 target	(SPR 24794)
01z,13oct98,cn   removed setting CSR6_FD in xxxMediaSelect [SPR# 21683]
		 and updated documentation about user flags.
01y,30sep98,dbt  Adding missing code in poll receive routine.
01x,22sep98,dat  SPR 22325, system mode transition.
01w,21apr98,jpd  added call to dec21x4xChipReset in dec21x4xStop().
01v,17mar98,jgn  fixed problems with this driver on CMA22x cards (SPR #20166)
		 (includes start up fix for 21x40 devices on all platforms)
01u,29jan98,rlp  check interrupt status before acknowledgement (SPR# 9995).
		 renamed macro names containing lower case characters.
01t,08dec97,tam  added _func_dec2114xIntAck.
01s,30oct97,rlp  added support for dec21143.
                 renamed _func_dec21x40MediaSelect to _func_dec2114xMediaSelect.
01r,19oct97,vin  moved swapping of loaned buffer before END_RCV_RTN_CALL
01q,17oct97,vin  removed extra free.
01p,09oct97,vin  delete unwanted frees in PollSend routine
01o,07oct97,vin  fixed multicasting, MIB MTU size to ETHER_MTU
01n,25sep97,gnn  SENS beta feedback fixes
01m,24sep97,vin  added clBlk related calls
01l,02sep97,gnn  fixed a race condition in the txRestart handling.
01k,25aug97,vin  adjusted cluster mem size.
01j,22aug97,vin  fixed initialization of cluster mem area.
01i,22aug97,gnn  fixed a bug in polled mode.
01h,21aug97,vin  added changes for newBuffering API
01g,12aug97,gnn  changes necessitated by MUX/END update.
01f,08aug97,map  fixed muxBufInit for user-specified bufpool.
                 added ilevel arg to sysLanIntDisable().
                 update collision statistics.
                 added more flags to userFlags.
01e,10jun97,map  renamed funcDec21x40MediaSelect to _func_dec21x40MediaSelect.
01d,28may97,kbw  general text edit
            map  added DEC SROM support for media configuration.
01c,23apr97,map  cleaned up; replaced SWAP_SHORT with PCISWAP_SHORT.
01b,16apr97,map  added support for dec21140.
01a,07apr97,map  modified if_dc.c to END style.
*/

/*
  
This module implements a DEC 21x4x PCI Ethernet network interface driver and
supports 21040, 21140 and 21143 versions of the chip.

The DEC 21x4x PCI Ethernet controller is little endian because it interfaces
with a little endian PCI bus.  Although PCI configuration for a device is
handled in the BSP, all other device programming and initialization are handled
in this module.

This driver is designed to be moderately generic. Without modification, it can
operate across the range of architectures and targets supported by VxWorks.
To achieve this, the driver requires a few external support routines as well
as several target-specific parameters.  These parameters, and the mechanisms
used to communicate them to the driver, are detailed below.  If any of the
assumptions stated below are not true for your particular hardware, you need to
modify the driver before it can operate correctly on your hardware.

On 21040, the driver configures the 10BASE-T interface by default, waits for
two seconds, and checks the status of the link. If the link status indicates
failure, AUI interface is configured.

On other versions of the 2114x family, the driver reads media information from
a DEC serial ROM and configures the media. On targets that do not support a
DEC format serial ROM, the driver calls a target-specfic media select routine
using the hook, _func_dec2114xMediaSelect, to configure the media.

The driver supports big-endian or little-endian architectures (as a 
configurable option).  The driver also and contains error recovery code 
that handles known device errata related to DMA activity.

Big endian processors can be connected to the PCI bus through some controllers
which take care of hardware byte swapping. In such cases all the registers
which the chip DMAs to have to be swapped and written to, so that when the
hardware swaps the accesses, the chip would see them correctly. The chip still
has to be programmed to operate in little endian mode as it is on the PCI
bus.  If the cpu board hardware automatically swaps all the accesses to and
from the PCI bus, then input and output byte stream need not be swapped.

BOARD LAYOUT
This device is on-board.  No jumpering diagram is necessary.

EXTERNAL INTERFACE

The driver provides one standard external interface, dec21x4xEndLoad(), which
a takes a string of colon separated parameters. The parameters should be
specified as hexadecimal strings, optionally preceded by "0x" or a minus sign
"-".

Although the parameter string is parsed using strtok_r(), each parameter is
converted from string to binary by a call to strtoul(parameter, NULL, 16).

The format of the parameter string is:

 "<unit number>:<device addr>:<PCI addr>:<ivec>:<ilevel>:<mem base>:\
<mem size>:<user flags>:<offset>"

TARGET-SPECIFIC PARAMETERS


.iP "unit number"

This represents the device instance number relative to this driver. 
I.e. a value of zero represents the first dec21x4x device, a value of 1
represents the second dec21x4x device.

.iP "device addr"

This is the base address at which the hardware device registers are located.

.iP "PCI addr"

This parameter defines the main memory address over the PCI bus. It is used to
translate physical memory address into PCI accessible address.

.iP "ivec"

This is the interrupt vector number of the hardware interrupt generated by
this Ethernet device.  The driver uses intConnect, or pciIntConnect (x86 arch),
to attach an interrupt handler for this interrupt.

.iP "ilevel"

This parameter defines the level of the hardware interrupt.

.iP "mem base"

This parameter specifies the base address of a DMA-able, cache free,
pre-allocated memory region for use as a memory pool for transmit/receive
descriptors and buffers.

If there is no pre-allocated memory available for the driver, this parameter
should be -1 (NONE). In which case, the driver allocates cache safe memory
for its use using cacheDmaAlloc().

.iP "mem size"

The memory size parameter specifies the size of the pre-allocated memory
region. If memory base is specified as NONE (-1), the driver ignores this
parameter.

.iP "user flags"

User flags control the run-time characteristics of the Ethernet
chip. Most flags specify non default CSR0 bit values. Refer to
dec21x4xEnd.h for the bit values of the flags, and to the device hardware
reference manual for details about device capabilities, and CSR 0.

Some of them are worth mentioning:

Full Duplex Mode: When set, the DEC_USR_FD flag allows the device to
work in full duplex mode, as long as the PHY used has this
capability. It is worth noting here that in this operation mode,
the dec21x40 chip ignores the Collision and the Carrier Sense signals.
 
Transmit treshold value: The DEC_USR_THR_XXX flags enable the user to
choose among different threshold values for the transmit FIFO.
Transmission starts when the frame size within the transmit FIFO is
larger than the treshold value. This should be selected taking into
account the actual operating speed of the PHY.  Again, see the device
hardware reference manual for details.
 
.iP "offset"

This parameter defines the offset which is used to solve alignment problem.

.iP "Device Type"

Although the default device type is DEC 21040, specifying the DEC_USR_21140
or DEC_USR_21143 flag bit turns on DEC 21140 or DEC_USR_21143 functionality.

.iP "Ethernet Address"

The Ethernet address is retrieved from standard serial ROM on DEC 21040, DEC
21140 and DEC 21143 devices. If retrieve from ROM fails, the driver calls the
BSP routine, sysDec21x4xEnetAddrGet(). Specifying DEC_USR_XEA flag bit tells
the driver should, by default, retrieve the Ethernet address using
the BSP routine, sysDec21x4xEnetAddrGet().

.iP "Priority RX processing"

The driver programs the chip to process the transmit and receive queues at the
same priority. By specifying DEC_USR_BAR_RX, the device is programmed
to process receives at a higher priority.

.iP "TX poll rate"

By default, the driver sets the Ethernet chip into a non-polling mode. 
In this mode, if the transmit engine is idle, it is kick-started every 
time a packet needs to be transmitted.  Alternately, the chip can be 
programmed to poll for the next available transmit descriptor if 
the transmit engine is in idle state. The poll rate is specified by one 
of DEC_USR_TAP_xxx.

.iP "Cache Alignment"

The DEC_USR_CAL_xxx flags specify the address boundaries for
data burst transfers.

.iP "DMA burst length"    

The DEC_USR_PBL_xxx flags specify the maximum number of long
words in a DMA burst.

.iP "PCI multiple read"

The DEC_USR_RML flag specifies that a device supports PCI
memory-read-multiple.


EXTERNAL SUPPORT REQUIREMENTS

This driver requires four external support functions, and provides a hook
function:

.iP "void sysLanIntEnable (int level)" "" 9 -1

This routine provides a target-specific interface for enabling Ethernet device
interrupts at a specified interrupt level.

.iP "void sysLanIntDisable (void)" "" 9 -1

This routine provides a target-specific interface for disabling Ethernet device
interrupts.

.iP "STATUS sysDec21x4xEnetAddrGet (int unit, char *enetAdrs)" "" 9 -1

This routine provides a target-specific interface for accessing a device
Ethernet address.

.iP "STATUS sysDec21143Init (DRV_CTRL * pDrvCtrl)" "" 9 -1

This routine performs any target-specific initialization
required before the dec21143 device is initialized by the driver.
The driver calls this routine every time it wants to load
the device.  This routine returns OK, or ERROR if it fails.

.iP "FUNCPTR _func_dec2114xMediaSelect" "" 9 -1

This driver provides a default media select routine, when
<_func_dec2114xMediaSelect> is NULL, to read and setup physical media with
configuration information from a Version 3 DEC Serial ROM. Any other media
configuration can be supported by initializing <_func_dec2114xMediaSelect>,
typically in sysHwInit(), to a target-specific media select routine.

A media select routine is typically defined as:

.ne 7
.CS
   STATUS decMediaSelect
       (
       DEC21X4X_DRV_CTRL *	pDrvCtrl,   /@ Driver control @/
       UINT *			pCsr6Val    /@ CSR6 return value @/
       )
       {
           ...
       }
.CE

Parameter <pDrvCtrl> is a pointer to the driver control structure which this
routine may use to access the Ethenet device. The driver control structure
field mediaCount, is initialized to 0xff at startup, while the other media
control fields (mediaDefault, mediaCurrent, and gprModeVal) are initialized to
zero. This routine may use these fields in any manner, however all other
driver control fields should be considered read-only and should not be
modified.

This routine should reset, initialize and select an appropriate media, and
write necessary the CSR6 bits (port select, PCS, SCR, and full duplex) to
memory location pointed to by <pCsr6Val>. The driver will use this value to
program register CSR6. This routine should return OK, and ERROR on failure.

.iP "FUNCPTR _func_dec2114xIntAck" "" 9 -1

This driver does acknowledge the LAN interrupts. However if the board hardware
requires specific interrupt acknowledgement, not provided by this driver,
the BSP should define such a routine and attach it to the driver via
_func_dec2114xIntAck. 

SEE ALSO: ifLib, 
.I "DECchip 21040 Ethernet LAN Controller for PCI."
.I "Digital Semiconductor 21140A PCI Fast Ethernet LAN Controller."
.I "Digital Semiconductor 21143 PCI/CardBus Fast Ethernet LAN Controller."
.I "Using the Digital Semiconductor 21140A with Boot ROM, Serial ROM, 
    and External Register: An Application Note"
.I "Using the Digital Semiconductor 21143 with Boot ROM, Serial ROM, 
    and External Register: An Application Note"
*/

/* includes */

#include "vxWorks.h"
#include "wdLib.h"
#include "iv.h"
#include "vme.h"
#include "errno.h"
#include "memLib.h"
#include "intLib.h"
#include "vxLib.h"
#include "net/mbuf.h"
#include "net/unixLib.h"
#include "net/protosw.h"
#include "sys/socket.h"
#include "sys/ioctl.h"
#include "net/route.h"
#include "iosLib.h"
#include "errnoLib.h"

#include "cacheLib.h"
#include "logLib.h"
#include "netLib.h"
#include "stdio.h"
#include "stdlib.h"
#include "sysLib.h"

#include "etherLib.h"
#include "net/systm.h"
#include "sys/times.h"
#include "net/if_subr.h"

#include "drv/end/dec21x4xEnd.h"

#undef	ETHER_MAP_IP_MULTICAST
#include "etherMultiLib.h"
#include "end.h"
#include "endLib.h"

#include "lstLib.h"
#include "semLib.h"

#undef	DRV_DEBUG
#undef	DRV_DEBUG_LOG

/* defines */

#define LOG_MSG(X0, X1, X2, X3, X4, X5, X6)				\
	if (_func_logMsg != NULL)					\
	    _func_logMsg(X0, X1, X2, X3, X4, X5, X6);

#ifdef	DRV_DEBUG_LOG
#define DRV_DEBUG_OFF		0x0000
#define DRV_DEBUG_RX		0x0001
#define	DRV_DEBUG_TX		0x0002
#define DRV_DEBUG_INT		0x0004
#define	DRV_DEBUG_POLL		(DRV_DEBUG_POLL_RX | DRV_DEBUG_POLL_TX)
#define	DRV_DEBUG_POLL_RX	0x0008
#define	DRV_DEBUG_POLL_TX	0x0010
#define	DRV_DEBUG_LOAD		0x0020
#define	DRV_DEBUG_IOCTL		0x0040
#define DRV_DEBUG_POLL_REDIR	0x10000
#define	DRV_DEBUG_LOG_NVRAM	0x20000

int	decDebug = 0x60;

#define DRV_LOG(FLG, X0, X1, X2, X3, X4, X5, X6)                        \
	if (decDebug & FLG)                                             \
            LOG_MSG(X0, X1, X2, X3, X4, X5, X6);

#else /* DRV_DEBUG_LOG */

#define DRV_LOG(DBG_SW, X0, X1, X2, X3, X4, X5, X6)

#endif /* DRV_DEBUG_LOG */

#ifdef  DRV_DEBUG

int     decTxInts=0;
 
IMPORT	void	decCsrShow (void);
IMPORT	void	decTxDShow(int numTxD); 
IMPORT	void	decRxDShow(int numRxD);
IMPORT	void	decShow(void); 
IMPORT  void	eAdrsDisplay(UINT8 * pAddr);
IMPORT	void	mcAdd (char *eAddr);
IMPORT	void	mcDel (char *eAddr);
IMPORT	void	decSerialRomUpload(void);
IMPORT	int	decReset(void);

#endif /* DRV_DEBUG */

#define END_FLAGS_ISSET(pEnd, setBits)                                  \
            ((pEnd)->flags & (setBits))

#define END_HADDR(pEnd)                                                 \
		((pEnd)->mib2Tbl.ifPhysAddress.phyAddress)

#define END_HADDR_LEN(pEnd)                                             \
		((pEnd)->mib2Tbl.ifPhysAddress.addrLength)

#define DEC_SPEED_10	10000000             /* 10 Mbps */
#define DEC_SPEED_100	100000000            /* 100 Mbps */
#define	DEC_SPEED_DEF	DEC_SPEED_10
#define EADDR_LEN	6                    /* ethernet address length */
#define DEC_PKT_SIZE	(ETHERMTU + ENET_HDR_REAL_SIZ + 6)

#define DELAY(count)	{                                               \
			volatile int cx = 0;                            \
			for (cx = 0; cx < (count); cx++);               \
			}

#define	LOOPS_PER_NS	4
#define NSDELAY(nsec)	{                                               \
			volatile int nx = 0;                            \
			volatile int loop = (int)((nsec)*LOOPS_PER_NS); \
			for (nx = 0; nx < loop; nx++);                  \
			}

#define DEC_BUF_ALLOC_ROUTINE                                           \
	(FUNCPTR) (DRV_FLAGS_ISSET(DEC_MEMOWN) ?                        \
                   cacheDmaMalloc : NULL)

#define DEC_BUF_POOL_ADRS                                               \
	(char *)(DRV_FLAGS_ISSET(DEC_MEMOWN) ?                          \
                  NULL:(pDrvCtrl->txRing + pDrvCtrl->numTds))

/* DRV_CTRL flags access macros */

#define DRV_FLAGS_SET(setBits)                                          \
	(pDrvCtrl->flags |= (setBits))

#define DRV_FLAGS_ISSET(setBits)                                        \
	(pDrvCtrl->flags & (setBits))

#define DRV_FLAGS_CLR(clrBits)                                          \
	(pDrvCtrl->flags &= ~(clrBits))

#define DRV_FLAGS_GET()                                                 \
        (pDrvCtrl->flags)

/* Cache macros */

#define DEC_CACHE_INVALIDATE(address, len)                              \
        CACHE_DRV_INVALIDATE (&pDrvCtrl->cacheFuncs, (address), (len))

#define DEC_CACHE_VIRT_TO_PHYS(address)                                 \
        CACHE_DRV_VIRT_TO_PHYS (&pDrvCtrl->cacheFuncs, (address))

#define DEC_CACHE_PHYS_TO_VIRT(address)                                 \
        CACHE_DRV_PHYS_TO_VIRT (&pDrvCtrl->cacheFuncs, (address))

/* memory to PCI address translation macros */

#define PCI_TO_MEM_PHYS(pciAdrs)                                        \
	(((ULONG)(pciAdrs)) - (pDrvCtrl->pciMemBase))
	
#define MEM_TO_PCI_PHYS(memAdrs)                                        \
	(((ULONG)(memAdrs)) + (pDrvCtrl->pciMemBase))

#define DEC_VIRT_TO_PCI(vAdrs)                                          \
	MEM_TO_PCI_PHYS (DEC_CACHE_VIRT_TO_PHYS (vAdrs))

#define DEC_PCI_TO_VIRT(pciAdrs)                                        \
	DEC_CACHE_PHYS_TO_VIRT (PCI_TO_MEM_PHYS (pciAdrs))

/*
 * CSR access macros
 *
 * To optimize CSR accesses, redefine DEC_CSR_READ and
 * DEC_CSR_WRITE macros in a wrapper file.
 */

#ifndef DEC_CSR_READ
#define DEC_CSR_READ(csr)                                               \
	dec21x4xCsrRead(pDrvCtrl->devAdrs, (csr))
#endif /* DEC_CSR_READ */

#ifndef DEC_CSR_WRITE
#define DEC_CSR_WRITE(csr,val)                                          \
	dec21x4xCsrWrite(pDrvCtrl->devAdrs, (csr), (val))
#endif /* DEC_CSR_WRITE */

#define DEC_CSR_UPDATE(csr,val)                                         \
	DEC_CSR_WRITE((csr), DEC_CSR_READ(csr) | (val))
    
#define DEC_CSR_RESET(csr,val)                                          \
	DEC_CSR_WRITE((csr), DEC_CSR_READ(csr) & ~(val))
    
#define DEC_SROM_CMD_WRITE(adrs,delay)                                  \
	{                                                               \
        DEC_CSR_WRITE(CSR9, CSR9_2114X_SR | CSR9_2114X_WR | (adrs));    \
        NSDELAY (delay);                                                \
        }

#define DEC_SROM_CMD_READ()                                             \
	((DEC_CSR_READ(CSR9) & 0x8) >> 3)


/*
 * Special purpose macros
 * These macros can be redefined in a wrapper file if desired
 */

#ifndef INITIAL_RESET

#define DUMMY_BUFFER_SIZE  60

#define INITIAL_RESET(pDrvCtrl) \
        dec21x4xInitialReset(pDrvCtrl)
#endif

/*
 * Default macro definitions for BSP interface.
 * These macros can be redefined in a wrapper file, to generate
 * a new module with an optimized interface.
 */

/*
 * For X86 arch, the intConnect/Disconnect defaults are pciIntConnect and
 * pciIntDisconnect.  For all others it is just intConnect and a dummy
 * disconnect function.
 * HELP: This needs to be fixed by porting pciIntLib to all architectures.
 */

#if CPU_FAMILY == I80X86
#ifndef SYS_INT_CONNECT
#define SYS_INT_CONNECT(pDrvCtrl,rtn,arg,pResult)                       \
    {                                                                   \
    IMPORT STATUS pciIntConnect();                                      \
    *pResult = pciIntConnect ((VOIDFUNCPTR *)INUM_TO_IVEC (pDrvCtrl->ivec),\
			     (rtn), (int)(arg));                        \
    }
#endif /*SYS_INT_CONNECT*/

#ifndef SYS_INT_DISCONNECT
#define SYS_INT_DISCONNECT(pDrvCtrl,rtn,arg,pResult)                    \
    {                                                                   \
    IMPORT STATUS pciIntDisconnect();                                      \
    *pResult = pciIntDisconnect ((VOIDFUNCPTR *)INUM_TO_IVEC (pDrvCtrl->ivec),\
			     (rtn));		                        \
    }
#endif /*SYS_INT_DISCONNECT*/

#else /* CPU_FAMILY == I80X86 */

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
    *pResult = OK;							\
    }
#endif /*SYS_INT_DISCONNECT*/

#endif /* CPU_FAMILY == I80X86 */

#ifndef SYS_INT_ENABLE
#define SYS_INT_ENABLE(pDrvCtrl)                                        \
    {                                                                   \
    IMPORT STATUS sysLanIntEnable();					\
    sysLanIntEnable(pDrvCtrl->ilevel);					\
    }
#endif /*SYS_INT_ENABLE*/

#ifndef SYS_INT_DISABLE
#define SYS_INT_DISABLE(pDrvCtrl)                                       \
    {                                                                   \
    IMPORT STATUS sysLanIntDisable();					\
    sysLanIntDisable(pDrvCtrl->ilevel);					\
    }
#endif /*SYS_INT_DISABLE*/

#ifndef SYS_DEC21X4X_INIT
#define SYS_DEC21X4X_INIT(pDrvCtrl,pResult)                             \
    {                                                                   \
    IMPORT STATUS sysDec21x4xInit();                                    \
    *pResult = sysDec21x4xInit (pDrvCtrl);	                        \
    }
#endif /* SYS_DEC21X4X_INIT */

#define	END_MIB_SPEED_SET(pEndObj, speed)                               \
	((pEndObj)->mib2Tbl.ifSpeed=speed)

#define NET_BUF_ALLOC()                                                 \
    netClusterGet (pDrvCtrl->endObj.pNetPool, pDrvCtrl->clPoolId)

#define NET_BUF_FREE(pBuf)                                              \
    netClFree (pDrvCtrl->endObj.pNetPool, pBuf)

#define	NET_MBLK_ALLOC()                                                \
    mBlkGet (pDrvCtrl->endObj.pNetPool, M_DONTWAIT, MT_DATA)

#define NET_MBLK_FREE(pMblk)                                            \
    netMblkFree (pDrvCtrl->endObj.pNetPool, (M_BLK_ID)pMblk)

#define	NET_CL_BLK_ALLOC()                                              \
    clBlkGet (pDrvCtrl->endObj.pNetPool, M_DONTWAIT)

#define NET_CL_BLK_FREE(pClblk)                                         \
    clBlkFree (pDrvCtrl->endObj.pNetPool, (CL_BLK_ID)pClBlk)
            
#define NET_MBLK_BUF_FREE(pMblk)                                        \
    netMblkClFree ((M_BLK_ID)pMblk)

#define NET_MBLK_CHAIN_FREE(pMblk)                                      \
    {                                                                   \
    M_BLK *pNext;                                                       \
                                                                        \
    pNext=pMblk;                                                        \
    while (pNext)                                                       \
        pNext=NET_MBLK_BUF_FREE (pNext);                                \
    }
    
#define NET_MBLK_CL_JOIN(pMblk, pClBlk)                              	\
    netMblkClJoin ((pMblk), (pClBlk))

#define NET_CL_BLK_JOIN(pClBlk, pBuf, len)                             	\
    netClBlkJoin ((pClBlk), (pBuf), (len), NULL, 0, 0, 0)
        
#define	DRV_CTRL	DEC21X4X_DRV_CTRL

#ifdef DRV_DEBUG
DRV_CTRL	*pDrvCtrlDbg;
#endif

/* structure sizes */

#define DRV_CTRL_SIZ		sizeof(DRV_CTRL)
#define RD_SIZ			sizeof(DEC_RD)
#define TD_SIZ			sizeof(DEC_TD)

/* locals */

/* Default network buffer configuration */

NET_POOL dec21x4xNetPool; 

/* forward declarations */

LOCAL STATUS	dec21x4xInitParse (DRV_CTRL *pDrvCtrl, char *InitString);
LOCAL STATUS	dec21x4xInitMem (DRV_CTRL *pDrvCtrl);
LOCAL STATUS	dec21x4xIASetup (DRV_CTRL *pDrvCtrl);
LOCAL void 	dec21x4xModeSet (DRV_CTRL *pDrvCtrl);
LOCAL int	dec21x4xHashIndex (char *eAddr);

LOCAL void 	dec21x4xInt (DRV_CTRL *pDrvCtrl);
LOCAL STATUS 	dec21x4xRecv (DRV_CTRL *pDrvCtrl, DEC_RD *rmd);
LOCAL STATUS 	dec21x4xChipReset (DRV_CTRL *pDrvCtrl);
LOCAL void	dec21040AuiTpInit (DRV_CTRL *pDrvCtrl);
LOCAL STATUS 	dec21x4xEnetAddrGet (DRV_CTRL *pDrvCtrl, char *enetAdrs);
LOCAL STATUS 	dec21040EnetAddrGet (DRV_CTRL *pDrvCtrl, char *enetAdrs);
LOCAL STATUS 	dec2114xEnetAddrGet (DRV_CTRL *pDrvCtrl, char *enetAdrs);
LOCAL void	dec21x4xRestart (DRV_CTRL *pDrvCtrl);

LOCAL DEC_TD *	dec21x4xTxDGet (DRV_CTRL *pDrvCtrl);
LOCAL DEC_RD *	dec21x4xRxDGet (DRV_CTRL *pDrvCtrl);
LOCAL void 	dec21x4xTxRingClean (DRV_CTRL *pDrvCtrl);
LOCAL void 	dec21x4xRxIntHandle (DRV_CTRL *pDrvCtrl);

LOCAL USHORT	dec2114xSromWordRead (DRV_CTRL *pDrvCtrl, UCHAR lineCnt);
LOCAL STATUS 	dec21140MediaSelect (DRV_CTRL *pDrvCtrl, UINT *pCsr6Val);
LOCAL STATUS 	dec21143MediaSelect (DRV_CTRL *pDrvCtrl, UINT *pCsr6Val);

LOCAL void      dec21x4xInitialReset (DRV_CTRL *pDrvCtrl);

LOCAL void	dec21x4xCsrWrite (ULONG devAdrs, int reg, ULONG value);
LOCAL ULONG	dec21x4xCsrRead (ULONG devAdrs, int reg);

/* externals */

IMPORT STATUS	sysDec21x4xEnetAddrGet (int unit, char *enetAdrs);

/* globals */

FUNCPTR _func_dec2114xMediaSelect;
VOIDFUNCPTR  _func_dec2114xIntAck;

/* END Specific interfaces. */

END_OBJ *	dec21x4xEndLoad (char *initString);
LOCAL STATUS	dec21x4xUnload (DRV_CTRL *pDrvCtrl);
LOCAL STATUS	dec21x4xStart (DRV_CTRL *pDrvCtrl);
LOCAL STATUS	dec21x4xStop (DRV_CTRL *pDrvCtrl);
LOCAL int	dec21x4xIoctl (DRV_CTRL *pDrvCtrl, int cmd, caddr_t data);
LOCAL STATUS	dec21x4xSend (DRV_CTRL *pDrvCtrl, M_BLK *pMblk);
LOCAL STATUS	dec21x4xMCastAddrAdd (DRV_CTRL *pDrvCtrl, char* pAddress);
LOCAL STATUS	dec21x4xMCastAddrDel (DRV_CTRL *pDrvCtrl, char* pAddress);
LOCAL STATUS	dec21x4xMCastAddrGet (DRV_CTRL *pDrvCtrl, 
                                      MULTI_TABLE *pTable);
LOCAL STATUS	dec21x4xPollSend (DRV_CTRL *pDrvCtrl, M_BLK *pMblk);
LOCAL STATUS	dec21x4xPollReceive (DRV_CTRL *pDrvCtrl, M_BLK *pMblk);
LOCAL STATUS	dec21x4xPollStart (DRV_CTRL *pDrvCtrl);
LOCAL STATUS	dec21x4xPollStop (DRV_CTRL *pDrvCtrl);

/* 
 * Define the device function table.  This is static across all driver
 * instances.
 */

LOCAL NET_FUNCS netFuncs = 
  {
  (FUNCPTR)dec21x4xStart,	/* start func. */                 
  (FUNCPTR)dec21x4xStop,	/* stop func. */
  (FUNCPTR)dec21x4xUnload,	/* unload func. */                
  (FUNCPTR)dec21x4xIoctl,	/* ioctl func. */                 
  (FUNCPTR)dec21x4xSend,	/* send func. */                  
  (FUNCPTR)dec21x4xMCastAddrAdd,/* multicast add func. */         
  (FUNCPTR)dec21x4xMCastAddrDel,/* multicast delete func. */      
  (FUNCPTR)dec21x4xMCastAddrGet,/* multicast get fun. */          
  (FUNCPTR)dec21x4xPollSend,	/* polling send func. */          
  (FUNCPTR)dec21x4xPollReceive,	/* polling receive func. */       
  endEtherAddressForm,		/* put address info into a NET_BUFFER */
  endEtherPacketDataGet,	/* get pointer to data in NET_BUFFER */
  endEtherPacketAddrGet		/* Get packet addresses. */
  };				

/*******************************************************************************
*
* dec21x4xEndLoad - initialize the driver and device
*
* This routine initializes the driver and the device to the operational state.
* All of the device specific parameters are passed in the initString.
*
* This routine can be called in two modes. If it is called with an empty, but
* allocated string then it places the name of this device (i.e. dc) into the
* initString and returns 0.
*
* If the string is allocated then the routine attempts to perform its load
* functionality.
*
* RETURNS: An END object pointer or NULL on error or 0 and the name of the
* device if the initString was NULL.
*/

END_OBJ * dec21x4xEndLoad
    (
    char *	initStr		/* String to be parse by the driver. */
    )
    {
    DRV_CTRL *	pDrvCtrl;
    char	eAdrs[EADDR_LEN];            /* ethernet address */
    int		retVal;
    char	bucket[sizeof(ULONG)];

    DRV_LOG (DRV_DEBUG_LOAD, "Loading dc ...\n", 0, 0, 0, 0, 0, 0);

    if (initStr == NULL)
        return (NULL);
    
    if (initStr[0] == NULL)
        {
        memcpy(initStr, (char *) DRV_NAME, DRV_NAME_LEN);
        return (0);
        }

    /* Allocate a control structure for this device */

    pDrvCtrl = calloc (sizeof(DRV_CTRL), 1);
    if (pDrvCtrl == NULL)
        {
        LOG_MSG ("%s - Failed to allocate control structure\n",
                (int) DRV_NAME, 0, 0, 0, 0, 0); 
        return (NULL);
        }

#ifdef DRV_DEBUG
    pDrvCtrlDbg = pDrvCtrl;
#endif

    pDrvCtrl->flags = NULL;
    pDrvCtrl->txCleaning = FALSE;
    pDrvCtrl->rxHandling = FALSE;
    pDrvCtrl->mediaCount = 0xff;

    /* Parse InitString */

    if (dec21x4xInitParse (pDrvCtrl, initStr) == ERROR)
        {
        LOG_MSG ("%s - Failed to parse initialization parameters\n",
                (int) DRV_NAME, 0, 0, 0, 0, 0);
        return (NULL);
        }

    /* callout to perform adapter init */
 
    SYS_DEC21X4X_INIT (pDrvCtrl, &retVal);
    if (retVal == ERROR)
        {
        LOG_MSG ("%s%d - Failed to perform adapter init \n", 
		(int) DRV_NAME, pDrvCtrl->unit, 0, 0, 0, 0);
        return (NULL);
        }

    /* Prevent transmission hanging up later on */

    INITIAL_RESET(pDrvCtrl);

    /* probe for memory-mapped CSR registers */

    if (vxMemProbe ((char *) pDrvCtrl->devAdrs, VX_READ, 
		sizeof(ULONG), &bucket[0]) != OK)
	{
	LOG_MSG ("%s%d - need MMU mapping for address %#x\n",
		(int)DRV_NAME, pDrvCtrl->unit, (int)pDrvCtrl->devAdrs, 0, 0, 0);
	return (NULL);
	}
 
    /* endObject Initializations */

    if (END_OBJ_INIT (&pDrvCtrl->endObj, (DEV_OBJ*)pDrvCtrl, DRV_NAME,
                      pDrvCtrl->unit, &netFuncs,
                      "dec21x4x Enhanced Network Driver") == ERROR)
        {
        LOG_MSG ("%s%d - Failed to initialize END object\n",
                (int) DRV_NAME, pDrvCtrl->unit, 0, 0, 0, 0);
        return (NULL);
        }

    /* memory Initializations */

    if (dec21x4xInitMem (pDrvCtrl) == ERROR)
        goto error;

    /* get Ethernet Address */

    if (dec21x4xEnetAddrGet (pDrvCtrl, eAdrs) == ERROR)
        {
        LOG_MSG ("%s%d - Failed to read ethernet address\n",
                (int) DRV_NAME, pDrvCtrl->unit, 0, 0, 0, 0);
        goto error;
        }

    DRV_LOG (DRV_DEBUG_LOAD,
             "ENET: %x:%x:%x:%x:%x:%x\n",
             eAdrs[0], eAdrs[1], eAdrs[2], eAdrs[3], eAdrs[4], eAdrs[5]);
    
    /* Initialize MIB-II entries */

    if (END_MIB_INIT (&pDrvCtrl->endObj, M2_ifType_ethernet_csmacd,
                      (UINT8*) eAdrs, EADDR_LEN,
                       ETHERMTU, DEC_SPEED_DEF) == ERROR)
        {
        LOG_MSG ("%s%d - MIB-II initializations failed\n",
                (int) DRV_NAME, pDrvCtrl->unit, 0, 0, 0, 0);
        goto error;
        }

    /* Mark the device ready with default flags */

    END_OBJ_READY (&pDrvCtrl->endObj,
                   IFF_NOTRAILERS | IFF_MULTICAST | IFF_BROADCAST);

    /* Successful return */

    return (&pDrvCtrl->endObj);

    /***** Handle error cases *****/

error:
	{
        dec21x4xUnload (pDrvCtrl);
        return (NULL);
        }
    }

/*******************************************************************************
*
* dec21x4xUnload - unload a driver from the system
*
* This routine deallocates lists, and free allocated memory.
*
* RETURNS: OK, always.
*/

LOCAL STATUS dec21x4xUnload
    (
    DRV_CTRL *	pDrvCtrl
    )
    {

    DRV_LOG (DRV_DEBUG_LOAD, "EndUnload\n", 0, 0, 0, 0, 0, 0);

    /* deallocate lists */

    END_OBJ_UNLOAD (&pDrvCtrl->endObj);

    /* deallocate allocated shared memory */

    if (DRV_FLAGS_ISSET (DEC_MEMOWN)  && pDrvCtrl->memBase)
        cacheDmaFree (pDrvCtrl->memBase);

    return (OK);
    }

/*******************************************************************************
*
* dec21x4xInitParse - parse parameter values from initString
*
* Parse the input string.  Fill in values in the driver control structure.
*
* The initialization string format is:
* "<device addr>:<PCI addr>:<ivec>:<ilevel>:<mem base>:<mem size>:	\
*  <user flags>:<offset>"
*
* .IP <device addr>
* base address of hardware device registers
* .IP <PCI addr>
* main memory address over the PCI bus
* .IP <ivec>
* interrupt vector number
* .IP <ilevel>
* interrupt level
* .IP <mem base>
* base address of a DMA-able, cache free,pre-allocated  memory
* .IP <mem size>
* size of the pre-allocated memory
* .IP <user flags>
* User flags control the run-time characteristics of the chip
* .IP <offset>
* Memory offset for alignment
* .LP
*
* RETURNS: OK or ERROR for invalid arguments.
*/

LOCAL STATUS dec21x4xInitParse
    (
    DRV_CTRL *	pDrvCtrl,
    char *	initString
    )
    {
    char *	tok;		/* an initString token */
    char *	holder=NULL;	/* points to initString fragment beyond tok */

    DRV_LOG (DRV_DEBUG_LOAD, "InitParse: Initstr=%s\n",
             (int)initString, 0, 0, 0, 0, 0);

    tok = strtok_r(initString, ":", &holder);
    if (tok == NULL)
        return ERROR;
    pDrvCtrl->unit = atoi(tok);

    tok=strtok_r(NULL, ":", &holder);
    if (tok == NULL)
        return ERROR;
    pDrvCtrl->devAdrs = strtoul (tok, NULL, 16);
    
    tok=strtok_r(NULL, ":", &holder);
    if (tok == NULL)
        return ERROR;
    pDrvCtrl->pciMemBase = strtoul (tok, NULL, 16);

    tok=strtok_r(NULL, ":", &holder);
    if (tok == NULL)
        return ERROR;
    pDrvCtrl->ivec = strtoul (tok, NULL, 16);

    tok=strtok_r(NULL, ":", &holder);
    if (tok == NULL)
        return ERROR;
    pDrvCtrl->ilevel = strtoul (tok, NULL, 16);
    
    tok=strtok_r(NULL, ":", &holder);
    if (tok == NULL)
        return ERROR;
    pDrvCtrl->memBase = (char *) strtoul (tok, NULL, 16);

    tok=strtok_r(NULL, ":", &holder);
    if (tok == NULL)
        return ERROR;
    pDrvCtrl->memSize = strtoul (tok, NULL, 16);

    tok=strtok_r(NULL, ":", &holder);
    if (tok == NULL)
        return (ERROR);
    pDrvCtrl->usrFlags = strtoul(tok, NULL, 16);

    tok = strtok_r (NULL, ":", &holder);
    if (tok == NULL)
        return ERROR;
    pDrvCtrl->offset = atoi (tok);
 
    /* decode non-register user flags */

    if (pDrvCtrl->usrFlags & DEC_USR_XEA)
        DRV_FLAGS_SET (DEC_BSP_EADRS);
    
    switch (pDrvCtrl->usrFlags & DEC_USR_VER_MSK)
	{
	case DEC_USR_21143 :
            DRV_FLAGS_SET (DEC_21143);
	    break;

	case DEC_USR_21140 :
	    DRV_FLAGS_SET (DEC_21140);
	    break;

	default :
            DRV_FLAGS_SET (DEC_21040);
	    break;
	}

    /* print debug info */

    DRV_LOG (DRV_DEBUG_LOAD,
            "EndLoad: unit=%d devAdrs=%#x ivec=%#x ilevel=%#x "
            "membase=%#x memSize=%#x\n",
            pDrvCtrl->unit, pDrvCtrl->devAdrs, pDrvCtrl->ivec, pDrvCtrl->ilevel,
            (int)pDrvCtrl->memBase, pDrvCtrl->memSize);
    
    DRV_LOG (DRV_DEBUG_LOAD,
             "pciMemBase=%#x flags=%#x usrFlags=%#x offset=%#x\n",
             (int)pDrvCtrl->pciMemBase, pDrvCtrl->flags, pDrvCtrl->usrFlags,
             pDrvCtrl->offset, 0, 0);

    return (OK);
    }

/*******************************************************************************
*
* dec21x4xInitMem - initialize memory
*
* Using data in the control structure, setup and initialize the memory
* areas needed.  If the memory address is not already specified, then allocate
* cache safe memory.
*
* RETURNS: OK or ERROR
*/

LOCAL STATUS dec21x4xInitMem
    (
    DRV_CTRL *	pDrvCtrl
    )
    {
    DEC_RD *	pRxD = pDrvCtrl->rxRing;
    DEC_TD *	pTxD = pDrvCtrl->txRing;
    M_CL_CONFIG	dcMclBlkConfig;
    CL_DESC	clDesc;                      /* cluster description */
    char *	pBuf;
    int		ix;
    int		sz;
    char *	pShMem;
    
    /* Establish size of shared memory region we require */
    
    DRV_LOG (DRV_DEBUG_LOAD, "InitMem\n", 0, 0, 0, 0, 0, 0);

    if ((int)pDrvCtrl->memBase != NONE)  /* specified memory pool */
	{
	sz  = ((pDrvCtrl->memSize - (RD_SIZ + TD_SIZ)) /
               (((2 + NUM_LOAN) * DEC_BUFSIZ) + RD_SIZ + TD_SIZ));
	pDrvCtrl->numRds = max (sz, MIN_RDS);
	pDrvCtrl->numTds = max (sz, MIN_TDS);
	}
    else
        {
        pDrvCtrl->numRds = NUM_RDS_DEF;
        pDrvCtrl->numTds = NUM_TDS_DEF;
        }

    /* Establish a region of shared memory */

    /*
     * OK. We now know how much shared memory we need.  If the caller
     * provides a specific memory region, we check to see if the provided
     * region is large enough for our needs.  If the caller did not
     * provide a specific region, then we attempt to allocate the memory
     * from the system, using the cache aware allocation system call.
     */

    switch ((int)pDrvCtrl->memBase)
        {
        default :       /* caller provided memory */

            sz = ((pDrvCtrl->numRds * (DEC_BUFSIZ + RD_SIZ + 8)) + 4 +
                  (pDrvCtrl->numTds * (DEC_BUFSIZ + TD_SIZ + 8)) + 4 +
                  (NUM_LOAN * (DEC_BUFSIZ + 8)) + 4);

            if ((int) pDrvCtrl->memSize < sz )     /* not enough space */
                {
                LOG_MSG( "%s%d: not enough memory provided\n",
                         (int) DRV_NAME, pDrvCtrl->unit, 0, 0, 0, 0);
                return ( ERROR );
                }
            pShMem = pDrvCtrl->memBase;      /* set the beginning of pool */

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
                LOG_MSG ( "%s%d: device requires cache coherent memory\n",
			(int) DRV_NAME, pDrvCtrl->unit, 0, 0, 0, 0);
                return (ERROR);
                }

            sz = (((pDrvCtrl->numRds + 1) * RD_SIZ) +
                  ((pDrvCtrl->numTds + 1) * TD_SIZ));

            pDrvCtrl->memBase = 
                pShMem = (char *) cacheDmaMalloc ( sz );

            if ((int)pShMem == NULL)
                {
                LOG_MSG ( "%s%d - system memory unavailable\n",
                         (int) DRV_NAME, pDrvCtrl->unit, 0, 0, 0, 0);
                return (ERROR);
                }

            pDrvCtrl->memSize = sz;

            DRV_FLAGS_SET (DEC_MEMOWN);

            /* copy the DMA structure */

            pDrvCtrl->cacheFuncs = cacheDmaFuncs;

            break;
        }

    /* zero the shared memory */

    memset (pShMem, 0, (int) sz);

    /* carve Rx memory structure */

    pRxD =
        pDrvCtrl->rxRing = (DEC_RD *) (((int)pShMem + 0x03) & ~0x03);

    /* carve Tx memory structure */

    pTxD =
        pDrvCtrl->txRing = (DEC_TD *) (pDrvCtrl->rxRing + pDrvCtrl->numRds);

    /* Initialize net buffer pool for tx/rx buffers */

    memset ((char *)&dcMclBlkConfig, 0, sizeof(dcMclBlkConfig));
    memset ((char *)&clDesc, 0, sizeof(clDesc));
    
    dcMclBlkConfig.mBlkNum  = pDrvCtrl->numRds * 4;
    clDesc.clNum 	    = pDrvCtrl->numRds + pDrvCtrl->numTds + NUM_LOAN;
    dcMclBlkConfig.clBlkNum = clDesc.clNum;

    /*
     * mBlk and cluster configuration memory size initialization
     * memory size adjusted to hold the netPool pointer at the head.
     */

    dcMclBlkConfig.memSize = ((dcMclBlkConfig.mBlkNum *
                               (MSIZE + sizeof (long))) +
                              (dcMclBlkConfig.clBlkNum *
                               (CL_BLK_SZ + sizeof (long))));
    
    if ((dcMclBlkConfig.memArea = (char *)memalign(sizeof (long),
                                            dcMclBlkConfig.memSize)) == NULL)
        return (ERROR);

    clDesc.clSize 	= DEC_BUFSIZ;
    clDesc.memSize 	= ((clDesc.clNum * (clDesc.clSize + 4)) + 4);

    if (DRV_FLAGS_ISSET(DEC_MEMOWN))
        {
        clDesc.memArea = (char *) cacheDmaMalloc (clDesc.memSize);
        if ((int)clDesc.memArea == NULL)
            {
            LOG_MSG ( "%s%d - system memory unavailable\n",
                     (int) DRV_NAME, pDrvCtrl->unit, 0, 0, 0, 0);
            return (ERROR);
            }
        }
    else
        clDesc.memArea = (char *) (pDrvCtrl->txRing + pDrvCtrl->numTds);
    
    if ((pDrvCtrl->endObj.pNetPool = malloc (sizeof(NET_POOL))) == NULL)
        return (ERROR);

    /* Initialize the net buffer pool with transmit buffers */

    if (netPoolInit (pDrvCtrl->endObj.pNetPool, &dcMclBlkConfig,
                     &clDesc, 1, NULL) == ERROR)
        {
        LOG_MSG ("%s%d - netPoolInit failed\n", 
		(int) DRV_NAME, pDrvCtrl->unit, 0, 0, 0, 0);
        return (ERROR);
        }

    /* Save the cluster pool id */

    pDrvCtrl->clPoolId = clPoolIdGet (pDrvCtrl->endObj.pNetPool,
                                      DEC_BUFSIZ, FALSE);
    /* Clear all indices */

    pDrvCtrl->rxIndex=0;
    pDrvCtrl->txIndex=0;
    pDrvCtrl->txDiIndex=0;

    /* Setup the receive ring */

    for (ix = 0; ix < pDrvCtrl->numRds; ix++, pRxD++)
        {
        pBuf = (char *) NET_BUF_ALLOC();
        if (pBuf == NULL)
            {
            LOG_MSG ("%s%d - netClusterGet failed\n", 
			(int) DRV_NAME, pDrvCtrl->unit, 0, 0, 0, 0);
            return (ERROR);
            }

	pRxD->rDesc2 = PCISWAP (DEC_VIRT_TO_PCI (pBuf)); /* buffer 1 */
	pRxD->rDesc3 = 0;                    /* no second buffer */

	/* buffer size */

	pRxD->rDesc1 = PCISWAP (RDESC1_RBS1_VAL (DEC_BUFSIZ) | 
                                RDESC1_RBS2_VAL (0));

	if (ix == (pDrvCtrl->numRds - 1))	/* if its is last one */
	    pRxD->rDesc1 |= PCISWAP (RDESC1_RER); /* end of receive ring */

	pRxD->rDesc0 = PCISWAP (RDESC0_OWN);	/* give ownership to chip */
        }


    /* Setup the transmit ring */

    for (ix = 0; ix < pDrvCtrl->numTds; ix++, pTxD++)
        {
        /* empty -- no buffers at this time */
	pTxD->tDesc2 = 0;
	pTxD->tDesc3 = 0;

	pTxD->tDesc1 = PCISWAP ((TDESC1_TBS1_PUT(0) |   /* buffer1 size */
                                 TDESC1_TBS2_PUT(0) |   /* buffer2 size */
                                 TDESC1_IC          | 	/* intrpt on xmit */
                                 TDESC1_LS          |	/* last segment */
                                 TDESC1_FS));		/* first segment */

	if (ix == (pDrvCtrl->numTds - 1))    /* if its is last one */
	    pTxD->tDesc1 |= PCISWAP (TDESC1_TER); /* end of Xmit ring */

	pTxD->tDesc0 = 0;                    /* owner is host */
        }

    /* Flush the write pipe */

    CACHE_PIPE_FLUSH ();

    return (OK);
    }

/*******************************************************************************
*
* dec21x4xStart - start the device
*
* This function initializes the device and calls BSP functions to connect
* interrupts and start the device running in interrupt mode.
*
* The complement of this routine is dec21x4xStop.  Once a unit is reset by
* dec21x4xStop, it may be re-initialized to a running state by this routine.
*
* RETURNS: OK if successful, otherwise ERROR
*/

LOCAL STATUS dec21x4xStart
    (
    DRV_CTRL *	pDrvCtrl
    )
    {
    int		retVal;
    UINT	csr6Val=0;
    UINT	usrFlags = pDrvCtrl->usrFlags;

    DRV_LOG (DRV_DEBUG_LOAD, "Start\n", 0, 0, 0, 0, 0, 0);

    /* Reset the device */

    dec21x4xChipReset (pDrvCtrl);

    /* Select the media */

    if (! DRV_FLAGS_ISSET (DEC_21040))
        {
        if (_func_dec2114xMediaSelect != NULL)
            retVal = (* _func_dec2114xMediaSelect) (pDrvCtrl, &csr6Val);
        else if (DRV_FLAGS_ISSET (DEC_21140))
            retVal = dec21140MediaSelect (pDrvCtrl, &csr6Val);
	else
	    retVal = dec21143MediaSelect (pDrvCtrl, &csr6Val);

        if (retVal == ERROR)
            return (ERROR);
        
        if (csr6Val & CSR6_2114X_PS)
            {
            DEC_CSR_UPDATE (CSR6, CSR6_2114X_PS);
            dec21x4xChipReset (pDrvCtrl);
            csr6Val |= CSR6_2114X_HBD;
            }
        
        csr6Val &= DEC_USR_CSR6_MSK;
        csr6Val |= CSR6_2114X_MB1;

        /* decode CSR6 specific options from userFlags */

        if (usrFlags & DEC_USR_SF)
            csr6Val |= CSR6_2114X_SF;

        if (usrFlags & DEC_USR_THR_MSK)
            csr6Val |= (usrFlags & DEC_USR_THR_MSK) >> DEC_USR_THR_SHF;

        if (usrFlags & DEC_USR_SB)
            csr6Val |= CSR6_SB;

        if (usrFlags & DEC_USR_PB)
            csr6Val |= CSR6_PB;

        if (usrFlags & DEC_USR_SC)
            csr6Val |= CSR6_2114X_SC;

        if (usrFlags & DEC_USR_CA)
            csr6Val |= CSR6_CAE;
        }
    else
        dec21040AuiTpInit (pDrvCtrl);

    /* Write start of receive ring */

    DEC_CSR_WRITE (CSR3, DEC_VIRT_TO_PCI(pDrvCtrl->rxRing));

    /* Write start of transmit ring */

    DEC_CSR_WRITE (CSR4, DEC_VIRT_TO_PCI(pDrvCtrl->txRing));

    /* clear the status register */

    DEC_CSR_WRITE (CSR5, 0xffffffff);

    /* setup CSR6 - start transmitter */

    DEC_CSR_WRITE (CSR6, csr6Val | CSR6_ST);

    /* setup ethernet address and filtering mode */

    dec21x4xIASetup (pDrvCtrl);

    /* set operating mode and start the receiver */

    dec21x4xModeSet (pDrvCtrl);

    /* set up the interrupts */

    DEC_CSR_WRITE (CSR7, (CSR7_NIM |         /* normal interrupt mask */
                          CSR7_RIM |         /* rcv  interrupt mask */
                          CSR7_TIM |         /* xmit interrupt mask */
                          CSR7_TUM |         /* xmit buff unavailble mask */
                          CSR7_AIM |         /* abnormal interrupt mask */
                          CSR7_SEM |         /* system error mask */
                          CSR7_RUM ));       /* rcv buff unavailable mask */

    if (! DRV_FLAGS_ISSET (DEC_21140))
        DEC_CSR_UPDATE (CSR7, CSR7_21X4X_LFM);   /* link fail mask */

    /* Connect the interrupt handler */

    SYS_INT_CONNECT (pDrvCtrl, dec21x4xInt, pDrvCtrl, &retVal);
    if (retVal == ERROR)
        return (ERROR);

    pDrvCtrl->txBlocked = FALSE;

    /* mark the interface -- up */

    END_FLAGS_SET (&pDrvCtrl->endObj, (IFF_UP | IFF_RUNNING));

    /* Enable LAN interrupts */

    SYS_INT_ENABLE (pDrvCtrl);

    return (OK);
    }

/*******************************************************************************
*
* dec21x4xStop - stop the device
*
* This routine marks the interface as down and resets the device.  This
* includes disabling interrupts, stopping the transmitter and receiver.
*
* The complement of this routine is dec21x4xStart.  Once a unit is
* stop in this routine, it may be re-initialized to a running state by
* dec21x4xStart.
*
* RETURNS: OK or ERROR
*/

LOCAL STATUS dec21x4xStop
    (
    DRV_CTRL *	pDrvCtrl
    )
    {
    int		retVal=OK;

    DRV_LOG (DRV_DEBUG_LOAD, "Stop\n", 0, 0, 0, 0, 0, 0);


    /* mark the interface down */

    END_FLAGS_CLR (&pDrvCtrl->endObj, IFF_UP | IFF_RUNNING);


    /* Reset the device */

    dec21x4xChipReset (pDrvCtrl);


    /* Disable LAN interrupts */

    SYS_INT_DISABLE (pDrvCtrl);


    /* disconnect interrupt */

    SYS_INT_DISCONNECT (pDrvCtrl, dec21x4xInt, (int)pDrvCtrl, &retVal);


    return (retVal);
    }

/*******************************************************************************
*
* dec21x4xRestart - restart the device
*
* Restarts the device after cleaning up the transmit and receive queues. This
* routine should be called only after dec21x4xStop().
*
* RETURNS: OK or ERROR
*/

LOCAL void dec21x4xRestart
    (
    DRV_CTRL *	pDrvCtrl
    )
    {
    DEC_TD *	pTxD = pDrvCtrl->txRing;
    FREE_BUF *	pFreeBuf = pDrvCtrl->freeBuf;
    int		count;
    
    LOG_MSG ("%s%d - restarting device.\n",
            (int) DRV_NAME, pDrvCtrl->unit, 0, 0, 0, 0);
    
    /* cleanup the tx and rx queues */

    dec21x4xTxRingClean (pDrvCtrl);
    dec21x4xRxIntHandle (pDrvCtrl);

    /* drop rest of the queued tx packets */

    for (count = pDrvCtrl->numTds; count; count--, pTxD++, pFreeBuf++)
        {
        if (pFreeBuf->pClBuf != NULL)
            {
            NET_BUF_FREE(pFreeBuf->pClBuf);
            pFreeBuf->pClBuf = NULL;
            }

        DEC_CACHE_INVALIDATE (pTxD, TD_SIZ);
        
        if (pTxD->tDesc1 & PCISWAP(TDESC1_SET))
            {
            pTxD->tDesc1 &= PCISWAP(~(TDESC1_SET | TDESC1_FT0));
            pTxD->tDesc0 = 0;
            }
        else if (pTxD->tDesc0 & PCISWAP(TDESC0_OWN|TDESC0_ES))
            {
            END_ERR_ADD (&pDrvCtrl->endObj, MIB2_OUT_ERRS, +1);
            END_ERR_ADD (&pDrvCtrl->endObj, MIB2_OUT_UCAST, -1);
            pTxD->tDesc0 = 0;
            }
        }

    /* Reset indices */

    pDrvCtrl->rxIndex=0;
    pDrvCtrl->txIndex=0;
    pDrvCtrl->txDiIndex=0;

    /* Restart the device */

    dec21x4xStart (pDrvCtrl);
    }

/*******************************************************************************
*
* dec21x4xIoctl - interface ioctl procedure
*
* Process an interface ioctl request.
*
* This routine implements the network interface control functions.
* It handles EIOCSADDR, EIOCGADDR, EIOCSFLAGS, EIOCGFLAGS, EIOCMULTIADD,
* EIOCMULTIDEL, EIOCMULTIGET, EIOCPOLLSTART, EIOCPOLLSTOP, EIOCGMIB2 commands.
*
* RETURNS: OK if successful, otherwise EINVAL.
*/

LOCAL int dec21x4xIoctl
    (
    DRV_CTRL *	pDrvCtrl,
    int		cmd,
    caddr_t	data
    )
    {
    int		error=0;
    long	value;
    int		savedFlags;
    END_OBJ *	pEndObj=&pDrvCtrl->endObj;
    
    DRV_LOG (DRV_DEBUG_IOCTL,
             "Ioctl unit=%#x cmd=%d data=%#x\n",
             pDrvCtrl->unit, cmd, (int)data, 0, 0, 0);

    switch (cmd)
        {
        case EIOCSADDR:
            if (data == NULL)
                error = EINVAL;
            else
                {
                /* Copy and install the new address */

                memcpy ((char *)END_HADDR(pEndObj), (char *)data,
                       END_HADDR_LEN(pEndObj));
                dec21x4xIASetup (pDrvCtrl);
                }

            break;
            
        case EIOCGADDR:                      /* HELP: move to mux */
            if (data == NULL)
                error = EINVAL;
            else
                memcpy ((char *)data, (char *)END_HADDR(pEndObj),
                       END_HADDR_LEN(pEndObj));
#ifdef	DRV_DEBUG
            {
            char *cp = (char *)data;
            DRV_LOG (DRV_DEBUG_IOCTL,
                          "EIOCGADDR: %x:%x:%x:%x:%x:%x\n",
                          cp[0], cp[1], cp[2], cp[3], cp[4], cp[5]);
            }
#endif /*DRV_DEBUG*/
            break;

        case EIOCSFLAGS:
            value = (long) data;
            if (value < 0)
                {
                value = -value;
                value--;
                END_FLAGS_CLR (pEndObj, value);
                }
            else
                END_FLAGS_SET (pEndObj, value);

            /* handle IIF_PROMISC and IFF_ALLMULTI */

            savedFlags = DRV_FLAGS_GET();
            if (END_FLAGS_ISSET (pEndObj, IFF_PROMISC))
                DRV_FLAGS_SET (DEC_PROMISC);
            else
                DRV_FLAGS_CLR (DEC_PROMISC);

            if (END_FLAGS_GET (pEndObj) & (IFF_ALLMULTI | IFF_MULTICAST))
                DRV_FLAGS_SET (DEC_MCAST);
            else
                DRV_FLAGS_CLR (DEC_MCAST);

            DRV_LOG (DRV_DEBUG_IOCTL, 
		"endFlags=%#x savedFlags=%#x newFlags=%#x\n",
		END_FLAGS_GET(pEndObj), savedFlags, DRV_FLAGS_GET(), 0, 0, 0);

            if ((DRV_FLAGS_GET() != savedFlags) &&
		(END_FLAGS_GET(pEndObj) & IFF_UP))
                dec21x4xModeSet (pDrvCtrl);

            break;

        case EIOCGFLAGS:                     /* move to mux */
            DRV_LOG (DRV_DEBUG_IOCTL, "EIOCGFLAGS: 0x%x: 0x%x\n",
                    pEndObj->flags, *(long *)data, 0, 0, 0, 0);

            if (data == NULL)
                error = EINVAL;
            else
                *(long *)data = END_FLAGS_GET(pEndObj);

            break;

        case EIOCMULTIADD:                   /* move to mux */
            error = dec21x4xMCastAddrAdd (pDrvCtrl, (char *)data);
            break;

        case EIOCMULTIDEL:                   /* move to mux */
            error = dec21x4xMCastAddrDel (pDrvCtrl, (char *)data);
            break;

        case EIOCMULTIGET:                   /* move to mux */
            error = dec21x4xMCastAddrGet (pDrvCtrl, (MULTI_TABLE *)data);
            break;

        case EIOCPOLLSTART:                  /* move to mux */
            dec21x4xPollStart (pDrvCtrl);
            break;

        case EIOCPOLLSTOP:                   /* move to mux */
            dec21x4xPollStop (pDrvCtrl);
            break;

        case EIOCGMIB2:                      /* move to mux */
            if (data == NULL)
                error=EINVAL;
            else
                memcpy((char *)data, (char *)&pEndObj->mib2Tbl,
                      sizeof(pEndObj->mib2Tbl));
            break;

        default:
            error = EINVAL;
        }

    return (error);
    }

/*******************************************************************************
*
* dec21x4xModeSet - promiscuous & multicast operation and start receiver
*
* This routine enables promiscuous and multicast mode if corresponding flags 
* are set and starts the receiver.
*
* RETURNS: N/A
*/

LOCAL void dec21x4xModeSet
    (
    DRV_CTRL *	pDrvCtrl
    )
    {
    int	opMode = CSR6_SR;                    /* start receiver */

    DRV_LOG (DRV_DEBUG_IOCTL, "ModeSet\n", 0, 0, 0, 0, 0, 0);

    if (DRV_FLAGS_ISSET (DEC_MCAST))
        opMode |= CSR6_PM;                   /* rx multicast */

    if (DRV_FLAGS_ISSET (DEC_PROMISC))
        opMode |= CSR6_PR;                   /* rx promiscuous */
    
    DEC_CSR_WRITE (CSR6, (DEC_CSR_READ (CSR6) & ~(CSR6_PM|CSR6_PR)) | opMode);

    return;
    }

/*******************************************************************************
*
* dec21x4xHashIndex - compute the hash index for an ethernet address
*
* This routine provides the hash index for an given ethernet address. The bit
* position in the hash table is taken from the CRC32 checksum derived from
* the first 6 bytes.
*
* RETURNS: hash index for an ethernet address.
*/

LOCAL int dec21x4xHashIndex
    (
    char *	eAddr
    )
    {
    UINT8	eAddrByte;
    int		index;                       /* hash index - return value */
    int		byte;                        /* loop - counter */
    int		bit;                         /* loop - counter */
    UINT	crc = 0xffffffff;
    UINT8	msb;

    for (byte=0; byte<6; byte++)
        {
        eAddrByte = eAddr[byte];

        for (bit=0; bit<8; bit++)
            {
            msb = crc >> 31;
            crc <<= 1;
            if (msb ^ (eAddrByte & 0x1))
                {
                crc ^= DEC_CRC_POLY;
                crc |= 0x1;
                }
            eAddrByte >>= 1;
            }
        }

    /*
     * return the upper 9 bits of the CRC in a decreasing order of
     * significance.
     * crc <31..23> as index <0..8>
     */

    index = 0;
    for (bit=0; bit<9; bit++)
        index |= ((crc >> (31-bit)) & 0x1) << bit;

    return index;
    }

/*******************************************************************************
*
* dec21x4xIASetup - set up physical and multicast addresses
*
* This routine sets up a filter frame to filter the physical addresses
* and all the current multicast addresses.
*
* While the first call to this routine during chip initialization requires
* that the receiver be turned off, subsequent calls do not.
* 
* RETURNS: OK on success, ERROR otherwise.
*/

LOCAL STATUS dec21x4xIASetup
    (
    DRV_CTRL *	pDrvCtrl 	/* pointer to device control structure */
    )
    {
    UINT8	ethBcastAdrs[]={ 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };
    UINT8 *	pFltrFrm;
    UINT8 *	pAdrs;
    ETHER_MULTI	* pMCastNode;
    DEC_TD *	pTxD;
    ULONG	csr7Val;
    char *	pBuf;
    int		index;
    int		timeout;

    DRV_LOG (DRV_DEBUG_IOCTL, "IASetup\n", 0, 0, 0, 0, 0, 0);

    /* gain exclusive access to transmitter */

    END_TX_SEM_TAKE (&pDrvCtrl->endObj, WAIT_FOREVER);
    
    /* get a free transmit descriptor */

    pTxD = dec21x4xTxDGet (pDrvCtrl);
    if (pTxD == NULL)
        {
        END_TX_SEM_GIVE (&pDrvCtrl->endObj);
        return (ERROR);
        }

    /* get a buffer */

     pBuf = NET_BUF_ALLOC();
     if (pBuf == NULL)
         {
         DRV_LOG (DRV_DEBUG_LOAD, "netClusterGet failed\n", 0, 0, 0, 0, 0, 0);
         END_TX_SEM_GIVE (&pDrvCtrl->endObj);
         return (ERROR);
         }

     /* align the frame */

     pFltrFrm = (UINT8 *) (((int)pBuf + 0x3) & ~0x3);

     /* clear all entries */

     memset (pFltrFrm, 0, FLTR_FRM_SIZE);

     /* install multicast addresses */

     for (pMCastNode = END_MULTI_LST_FIRST (&pDrvCtrl->endObj);
          pMCastNode != NULL;
          pMCastNode = END_MULTI_LST_NEXT (pMCastNode))
         {
         index = dec21x4xHashIndex (pMCastNode->addr);
         pFltrFrm [DEC_FLT_INDEX(index/8)] |= 1 << (index % 8);
         }

     /* install an ethernet broadcast address */

     index = dec21x4xHashIndex (ethBcastAdrs);
     pFltrFrm [DEC_FLT_INDEX(index/8)] |= 1 << (index % 8);

     /* install the physical address */

     pAdrs = (UINT8 *)END_HADDR (&pDrvCtrl->endObj);

     for (index=0; index<6; index++)
         pFltrFrm [FLTR_FRM_PHY_ADRS_OFF + DEC_FLT_INDEX(index)] = pAdrs[index];

     /* transmit the frame */

     pTxD->tDesc2  = PCISWAP (DEC_VIRT_TO_PCI (pFltrFrm));
     pTxD->tDesc3  = 0;

     /* frame type is SETUP, filtering mode is HASH + 1 perfect */

     pTxD->tDesc1 |= PCISWAP (TDESC1_SET | TDESC1_FT0); 
     pTxD->tDesc1 &= PCISWAP (~TDESC1_TBS1_MSK);
     pTxD->tDesc1 |= PCISWAP (TDESC1_TBS1_PUT(FLTR_FRM_SIZE));
     pTxD->tDesc1 &= PCISWAP (~(TDESC1_LS|TDESC1_FS));

     pTxD->tDesc0  = PCISWAP(TDESC0_OWN);      /* ready for transmit */

    /* Flush the write pipe */

     CACHE_PIPE_FLUSH();

     /* Advance our management index */

     pDrvCtrl->txIndex = (pDrvCtrl->txIndex + 1) % pDrvCtrl->numTds;

     /* mask interrupts */

     csr7Val = DEC_CSR_READ (CSR7);
     DEC_CSR_WRITE (CSR7, 0);

     /* start tx */

     if (DRV_FLAGS_ISSET(DEC_TX_KICKSTART))
         DEC_CSR_WRITE (CSR1, CSR1_TPD);

     /* wait for completion */

     timeout=0xffffff;
     while (timeout && (pTxD->tDesc0 != (~PCISWAP (TDESC0_OWN))))
         timeout--;

     /* restore TXD bits */

     pTxD->tDesc0  = 0;
     pTxD->tDesc1 &= PCISWAP (~(TDESC1_SET | TDESC1_FT0));
     pTxD->tDesc1 |= PCISWAP (TDESC1_LS | TDESC1_FS);
     pTxD->tDesc2  = 0;

     /* restore interrupts */

     DEC_CSR_WRITE (CSR7, csr7Val);

     /* free the buffer */

     NET_BUF_FREE (pBuf);

    /* release exclusive access */

    END_TX_SEM_GIVE (&pDrvCtrl->endObj);

    /* return success */

    return (OK);
    }

/*******************************************************************************
*
* dec21x4xMCastAddrAdd - add a multicast address
*
* This routine adds a multicast address to whatever the driver
* is already listening for.
*
* RETURNS: OK on success, ERROR otherwise.
*/

LOCAL STATUS dec21x4xMCastAddrAdd
    (
    DRV_CTRL *	pDrvCtrl,
    char *	pAddr
    )
    {
    int	retVal;

    DRV_LOG (DRV_DEBUG_IOCTL, "MCastAddrAdd\n", 0, 0, 0, 0, 0, 0);

    /* add a multicast address to the list */

    retVal = etherMultiAdd (&pDrvCtrl->endObj.multiList, pAddr);

    /* reconfigure the device */

    if (retVal == ENETRESET)
        return dec21x4xIASetup (pDrvCtrl);

    return ((retVal == OK) ? OK : ERROR);
    }

/*******************************************************************************
*
* dec21x4xMCastAddrDel - remove a multicast address
*
* This routine deletes a multicast address from the current list of
* multicast addresses.
*
* RETURNS: OK on success and ERROR otherwise.
*/

LOCAL STATUS dec21x4xMCastAddrDel
    (
    DRV_CTRL *	pDrvCtrl,
    char *	pAddr
    )
    {
    int retVal;

    DRV_LOG (DRV_DEBUG_IOCTL, "MCastAddrDel\n", 0, 0, 0, 0, 0, 0);

    /* remove a multicast address from the list */

    retVal = etherMultiDel (&pDrvCtrl->endObj.multiList, pAddr);

    /* reconfigure the device */
 
    if (retVal == ENETRESET)
        return dec21x4xIASetup (pDrvCtrl);

    return ((retVal == OK) ? OK : ERROR);
    }

/*******************************************************************************
*
* dec21x4xMCastAddrGet - retreive current multicast address list
*
* This routine returns the current multicast address list in <pTable>
*
* RETURNS: OK on success; otherwise ERROR.
*/

LOCAL STATUS dec21x4xMCastAddrGet
    (
    DRV_CTRL *		pDrvCtrl,
    MULTI_TABLE *	pTable
    )
    {
    DRV_LOG (DRV_DEBUG_IOCTL, "MCastAddrGet\n", 0, 0, 0, 0, 0, 0);

    return (etherMultiGet (&pDrvCtrl->endObj.multiList, pTable));
    }

/*******************************************************************************
*
* dec21x4xSend - transmit an ethernet packet
*
* This routine() takes a M_BLK_ID and sends off the data in the M_BLK_ID.
* The buffer must already have the addressing information properly installed
* in it. This is done by a higher layer. The last arguments are a free
* routine to be called when the device is done with the buffer and a pointer
* to the argument to pass to the free routine.
*
* muxSend() calls this routine each time it wants to send a packet.
* Errors are detected at interrupt level.
*
* RETURNS: OK on success; and ERROR otherwise.
*/

LOCAL STATUS dec21x4xSend
    (
    DRV_CTRL *	pDrvCtrl,
    M_BLK *	pMblk
    )
    {
    DEC_TD *	pTxD;
    char *	pBuf;
    int		len;
    int         s;

    DRV_LOG (DRV_DEBUG_TX, "S:%#x ", pDrvCtrl->txIndex, 0, 0, 0, 0, 0);

#if 0    
    if (pDrvCtrl->txBlocked)
        return (END_ERR_BLOCK); /* transmitter not ready */
#endif    

    /* Gain exclusive access to transmit */

    END_TX_SEM_TAKE (&pDrvCtrl->endObj, WAIT_FOREVER);

    /* Get the next TXD */

    pTxD = dec21x4xTxDGet (pDrvCtrl);
    pBuf = NET_BUF_ALLOC();

    if ((pTxD == NULL) || (pBuf == NULL))
        {
        END_ERR_ADD (&pDrvCtrl->endObj, MIB2_OUT_ERRS, +1);
        END_TX_SEM_GIVE (&pDrvCtrl->endObj);

        if (pBuf)
            NET_BUF_FREE (pBuf);

        if (!pDrvCtrl->txCleaning && !pDrvCtrl->txBlocked)
            dec21x4xTxRingClean (pDrvCtrl);

        s = intLock();
        pDrvCtrl->txBlocked = TRUE;        /* transmitter not ready */
        intUnlock(s);

        return (END_ERR_BLOCK); /* just return without freeing mBlk chain */
        }

    /* copy and free the MBLK */

    len = netMblkToBufCopy (pMblk, pBuf, NULL);

    /* setup the transmit buffer pointers */

    NET_MBLK_CHAIN_FREE (pMblk);

    /* setup the transmit buffer pointers */

    pTxD->tDesc2 = PCISWAP (DEC_VIRT_TO_PCI (pBuf));
    pTxD->tDesc3 = 0;

    /* setup frame len */

    pTxD->tDesc1 &= PCISWAP (~TDESC1_TBS1_MSK);
    pTxD->tDesc1 |= PCISWAP (TDESC1_TBS1_PUT(len));

    /* transfer ownership to device */

    pTxD->tDesc0  = PCISWAP(TDESC0_OWN);      

    /* Flush the write pipe */

    CACHE_PIPE_FLUSH();

    /* Save the buf info */

    pDrvCtrl->freeBuf[pDrvCtrl->txIndex].pClBuf = pBuf;

    /* Advance our management index */

    pDrvCtrl->txIndex = (pDrvCtrl->txIndex + 1) % pDrvCtrl->numTds;

     /* start tx */

    if (DRV_FLAGS_ISSET(DEC_TX_KICKSTART))
        DEC_CSR_WRITE (CSR1, CSR1_TPD);

    END_TX_SEM_GIVE (&pDrvCtrl->endObj);

    /* update statistics */

    END_ERR_ADD (&pDrvCtrl->endObj, MIB2_OUT_UCAST, +1);

    return (OK);
    }

/*******************************************************************************
*
* dec21x4xRecv - pass a received frame to the upper layer
*
* This routine processes an input frame, then passes it up to the higher
* level in a form it expects.  Buffer loaning, promiscuous mode are all
* supported.  Trailer protocols is not supported.
*
* RETURNS: OK, always.
*/

LOCAL STATUS dec21x4xRecv
    (
    DRV_CTRL *	pDrvCtrl,
    DEC_RD *	pRxD
    )
    {
    END_OBJ *	pEndObj = &pDrvCtrl->endObj;
    M_BLK_ID	pMblk;      /* MBLK to send upstream */
    CL_BLK_ID	pClBlk;	    /* pointer to clBlk */
    char *	pBuf;	    /* A replacement buffer for the current RxD */
    char *	pData;      /* Data pointer for the current RxD */
    char *	pTmp;
    int     	len;	    /* Len of the current data */

    DRV_LOG (DRV_DEBUG_RX, "R", 0, 0, 0, 0, 0, 0); 
    
    /* check for errors */

    if (pRxD->rDesc0 & PCISWAP(RDESC0_ES))
        {
        DRV_LOG (DRV_DEBUG_RX, "- ",0, 0, 0, 0, 0, 0);
        END_ERR_ADD (pEndObj, MIB2_IN_ERRS, +1);
        goto cleanRxD;
        }

    /* Allocate an MBLK, and a replacement buffer */

    pMblk = NET_MBLK_ALLOC();
    pBuf  = NET_BUF_ALLOC();
    pClBlk = NET_CL_BLK_ALLOC(); 

    if ((pMblk == NULL) || (pBuf == NULL) || (pClBlk == NULL))
        {
        END_ERR_ADD (pEndObj, MIB2_IN_ERRS, +1);
        if (pMblk)
            NET_MBLK_FREE (pMblk);
        if (pBuf)
            NET_BUF_FREE (pBuf);
        if (pClBlk)
            NET_CL_BLK_FREE (pClBlk);
        goto cleanRxD;
        }

    /* Get the data pointer and len from the current RxD */

    len   = DEC_FRAME_LEN_GET (PCISWAP (pRxD->rDesc0)) - ETH_CRC_LEN; 
    pData = (char *) DEC_PCI_TO_VIRT (PCISWAP (pRxD->rDesc2));

    /*
     * The code above solves alignment problem when the CPU and the
     * ethernet chip don't accept longword unaligned addresses.
     * 
     * Pb: When the ethernet chip receives a packet from the network,
     * it needs a longword aligned buffer to copy the data. To process the
     * IP packet, MUX layer adds a ENET_HDR_REAL_SIZ (0x14) offset to the
     * data buffer. So the CPU obtains a longword unaligned buffer and
     * a fault execption occurs when it reads "ip_src" and "ip_dst" fields 
     * in the IP structure (ip_src and ip_dst fields are longwords).
     *
     * The problem is solved copying the data in a new buffer in which
     * the IP structure is aligned.
     */  

    if (pDrvCtrl->offset != 0)
	{
	/* exchange buffer addresses */

	pTmp = pData;
	pData = pBuf;
	pBuf = pTmp;

	/* copy data in new unaligned buffer */
 
	pData += pDrvCtrl->offset;
	memcpy(pData, pBuf, len);
        }

    /* Restore orignal buffer address to be able to free it */
 
    pData -= pDrvCtrl->offset; 

    /* Associate the data pointer with the MBLK */

    NET_CL_BLK_JOIN (pClBlk, pData, DEC_BUFSIZ); 
    
    /* Associate the data pointer with the MBLK */

    NET_MBLK_CL_JOIN (pMblk, pClBlk);

    pMblk->mBlkHdr.mData += pDrvCtrl->offset; 
    pMblk->mBlkHdr.mFlags 	|= M_PKTHDR;	/* set the packet header */
    pMblk->mBlkHdr.mLen		= len;		/* set the data len */
    pMblk->mBlkPktHdr.len 	= len;		/* set the total len */

    /* Make cache consistent with memory */

    DEC_CACHE_INVALIDATE (pData, len);

    /* Install the new data buffer */

    pRxD->rDesc2 = PCISWAP (DEC_VIRT_TO_PCI (pBuf));

    /* mark the descriptor ready to receive */

    pRxD->rDesc0 = PCISWAP (RDESC0_OWN);

    /* update statistics */
    
    DRV_LOG (DRV_DEBUG_RX, "+ ", 0, 0, 0, 0, 0, 0);
    END_ERR_ADD (pEndObj, MIB2_IN_UCAST, +1);

    /* advance management index */

    pDrvCtrl->rxIndex = (pDrvCtrl->rxIndex + 1) % pDrvCtrl->numRds;

    /* Flush the write pipe */

    CACHE_PIPE_FLUSH();

    /* send the frame to the upper layer */

    END_RCV_RTN_CALL (pEndObj, pMblk);

    return OK;

cleanRxD:

    /* mark the descriptor ready to receive */

    pRxD->rDesc0 = PCISWAP (RDESC0_OWN);

    /* update statistics */
    
    DRV_LOG (DRV_DEBUG_RX, "+ ", 0, 0, 0, 0, 0, 0);
    END_ERR_ADD (pEndObj, MIB2_IN_UCAST, +1);

    /* Flush the write pipe */

    CACHE_PIPE_FLUSH();

    /* advance management index */

    pDrvCtrl->rxIndex = (pDrvCtrl->rxIndex + 1) % pDrvCtrl->numRds;

    return (OK);
    }

/*******************************************************************************
*
* dec21x4xRxIntHandle - perform receive processing
*
* This routine is called at task level indirectly by the interrupt
* service routine to do any message received processing.
*
* RETURNS: N/A
*/

LOCAL void dec21x4xRxIntHandle
    (
    DRV_CTRL *	pDrvCtrl
    )
    {
    DEC_RD *	pRxD;

    pDrvCtrl->rxHandling = TRUE;
    
    while ((pRxD = dec21x4xRxDGet (pDrvCtrl)))
            dec21x4xRecv (pDrvCtrl, pRxD);

    DEC_CSR_UPDATE (CSR7, CSR7_RIM);     /* turn on Rx interrupts */
    pDrvCtrl->rxHandling = FALSE;
    }

/*******************************************************************************
*
* dec21x4xRxDGet - get a ready receive descriptor
*
* Get next received message descriptor.  Returns NULL if none are
* ready.
*
* RETURNS: a filled receive descriptor, otherwise NULL.
*/

LOCAL DEC_RD * dec21x4xRxDGet
    (
    DRV_CTRL *	pDrvCtrl
    )
    {
    DEC_RD *	pRxD = pDrvCtrl->rxRing + pDrvCtrl->rxIndex;
    
    DEC_CACHE_INVALIDATE (pRxD, RD_SIZ);

    /* check if the descriptor is owned by the chip */

    if (pRxD->rDesc0 & PCISWAP (RDESC0_OWN))
        return (NULL);

    return (pRxD);
    }

/*******************************************************************************
*
* dec21x4xTxDGet - get an available transmit descriptor
*
* Get next transmited message descriptor.  Returns NULL if none are
* ready.
*
* RETURNS: an available transmit descriptor, otherwise NULL.
*/

LOCAL DEC_TD * dec21x4xTxDGet
    (
    DRV_CTRL *	pDrvCtrl
    )
    {
    DEC_TD *	pTxD = pDrvCtrl->txRing + pDrvCtrl->txIndex;

    DEC_CACHE_INVALIDATE (pTxD, TD_SIZ);

    /* check if this descriptor is owned by the chip or is out of bounds */

    if ((pTxD->tDesc0 & PCISWAP(TDESC0_OWN)) ||
        (((pDrvCtrl->txIndex + 1) % pDrvCtrl->numTds) == pDrvCtrl->txDiIndex))
        return (NULL);

    return (pTxD);
    }

/*******************************************************************************
*
* dec21x4xTxRingClean - clean up processed tx descriptors
*
* This routine processes the transmit queue, freeing all transmitted
* descriptors.
*
* RETURNS: N/A
*/

LOCAL void dec21x4xTxRingClean
    (
    DRV_CTRL *	pDrvCtrl
    )
    {
    DEC_TD *	pTxD;
    
    pDrvCtrl->txCleaning = TRUE;
    
    while (pDrvCtrl->txDiIndex != pDrvCtrl->txIndex)
        {
        pTxD = pDrvCtrl->txRing + pDrvCtrl->txDiIndex;
        DEC_CACHE_INVALIDATE (pTxD, TD_SIZ);

	/* check if this descriptor is owned */

        if (pTxD->tDesc0 & PCISWAP(TDESC0_OWN))
            break;

        DRV_LOG (DRV_DEBUG_TX, "Tc:%#x ", pDrvCtrl->txDiIndex, 0, 0, 0, 0, 0);

        /* free the buffer */

        if (pDrvCtrl->freeBuf[pDrvCtrl->txDiIndex].pClBuf != NULL)
            {
            NET_BUF_FREE(pDrvCtrl->freeBuf[pDrvCtrl->txDiIndex].pClBuf);
            pDrvCtrl->freeBuf[pDrvCtrl->txDiIndex].pClBuf = NULL;
            }
        
        pDrvCtrl->txDiIndex =  (pDrvCtrl->txDiIndex + 1) % pDrvCtrl->numTds;

        /* count collisions */
#if 0
        if (! (pTxD->tDesc0 & PCISWAP(TDESC0_EC)))
            END_ERR_ADD (pEndObj, MIB2_COLLISIONS,
                         TDESC0_CC_VAL (PCISWAP (pTxD->tDesc0)));
#endif

        /* clear frame-type of a setup frame and process errors for others */

        if (pTxD->tDesc1 & PCISWAP(TDESC1_SET))
            {
            pTxD->tDesc1 &= PCISWAP(~(TDESC1_SET | TDESC1_FT0));
            }
        else if (pTxD->tDesc0 & PCISWAP(TDESC0_ES))
            {
            END_OBJ	*pEndObj = &pDrvCtrl->endObj;

            END_ERR_ADD (pEndObj, MIB2_OUT_ERRS, +1);
            END_ERR_ADD (pEndObj, MIB2_OUT_UCAST, -1);

            /* count excessive collisions */
#if 0
            if (pTxD->tDesc0 & PCISWAP(TDESC0_EC))
                END_ERR_ADD (pEndObj, MIB2_COLLISIONS, 16);

            /* check for no carrier */

            if (pTxD->tDesc0 & PCISWAP(TDESC0_NC|TDESC0_LO|TDESC0_LF))
                LOG_MSG ("%s%d - no carrier\n",
                        (int) DRV_NAME, pDrvCtrl->unit, 0, 0, 0, 0);
#endif

            /* restart if DMA underflow is detected */

            if (pTxD->tDesc0 & PCISWAP(TDESC0_UF))
                {
                LOG_MSG ("%s%d - fatal DMA underflow\n",
                        (int) DRV_NAME, pDrvCtrl->unit, 0, 0, 0, 0);

                dec21x4xStop (pDrvCtrl);
                pDrvCtrl->txCleaning = FALSE;
                netJobAdd ((FUNCPTR)dec21x4xRestart, (int)pDrvCtrl, 0, 0, 0, 0);
                return;
               }
            }

        pTxD->tDesc0 = 0;                    /* clear errors and stats */
        }

    pDrvCtrl->txCleaning = FALSE;

    return;
    }

/*******************************************************************************
*
* dec21x4xInt - handle device interrupts
*
* This interrupt service routine, reads device interrupt status, acknowledges
* the interrupting events, and schedules receive and transmit processing when
* required.
*
* RETURNS: N/A
*/

LOCAL void dec21x4xInt
    (
    DRV_CTRL *	pDrvCtrl
    )
    {
    ULONG	status;

    /* get the status bits */

    status = DEC_CSR_READ (CSR5);

    /* return on false interrupt */
 
    if ((status & (CSR5_NIS | CSR5_AIS)) == NULL)
        {
        DRV_LOG (DRV_DEBUG_INT, "false intr\n", 0, 0, 0, 0, 0, 0);
        return;
        }
 
    DRV_LOG (DRV_DEBUG_INT, "i=0x%x:", status, 0, 0, 0, 0, 0);

    /* Check for system error */

    if (status & CSR5_SE)
        {
        LOG_MSG ("%s%d - fatal system error\n",
                (int) DRV_NAME, pDrvCtrl->unit, 0, 0, 0, 0);

	/* Acknowledge system error interrupts */

	DEC_CSR_WRITE (CSR5, (CSR5_SE | CSR5_AIS));

        dec21x4xStop (pDrvCtrl);
        netJobAdd ((FUNCPTR)dec21x4xRestart, (int)pDrvCtrl, 0, 0, 0, 0);
        return;
        }

    /* Handle received packets */

    if (status & CSR5_RI)
	{
	DRV_LOG (DRV_DEBUG_INT, "r ", 0, 0, 0, 0, 0, 0);

	if ((! (status & CSR5_RPS)) && (!pDrvCtrl->rxHandling))
            {
            pDrvCtrl->rxHandling = TRUE;

            netJobAdd ((FUNCPTR)dec21x4xRxIntHandle, (int)pDrvCtrl, 0, 0, 0, 0);
        
            /* disable Rx intr; re-enable in dec21x4xRxIntHandle() */

            DEC_CSR_RESET (CSR7, CSR7_RIM);
            }

	/* Acknowledge receive interrupt */

	DEC_CSR_WRITE (CSR5, (CSR5_RI | CSR5_NIS));
	}

#ifdef DRV_DEBUG
    if (status & CSR5_TI)
        decTxInts++;
#endif
    
    /* Cleanup TxRing */

    if (status & CSR5_TI)
	{
        DRV_LOG (DRV_DEBUG_INT, "t ", 0, 0, 0, 0, 0, 0);

	if (!pDrvCtrl->txCleaning)
            {
            pDrvCtrl->txCleaning = TRUE;

            netJobAdd ((FUNCPTR)dec21x4xTxRingClean, (int)pDrvCtrl, 0, 0, 0, 0);
            }

        /* Acknowledge transmit interrupts */

	DEC_CSR_WRITE (CSR5, (CSR5_TI | CSR5_NIS));
	}

    /* Acknowledge all other interrupts - ignore events */

    DEC_CSR_WRITE (CSR5, (status & ~(CSR5_SE | CSR5_RI | CSR5_TI)));

    if (_func_dec2114xIntAck != NULL)
	{

	/*
	 * Call BSP specific interrupt ack. routine, if defined.
	 * This must be called after all the interrupt sources
	 * have been cleared
	 */

	(* _func_dec2114xIntAck) ();
	}


    if (pDrvCtrl->txBlocked)
        {
        pDrvCtrl->txBlocked = FALSE;
        netJobAdd ((FUNCPTR)muxTxRestart, (int)&pDrvCtrl->endObj, 0, 0,
                   0, 0);
        }
    return;
    }

/*******************************************************************************
*
* dec21x4xPollStart - starting polling mode
*
* This routine starts polling mode by disabling ethernet interrupts and
* setting the polling flag in the END_CTRL stucture.
*
* RETURNS: OK, always.
*/

LOCAL STATUS dec21x4xPollStart
    (
    DRV_CTRL *	pDrvCtrl
    )
    {
    int	intLevel;

    DRV_LOG (DRV_DEBUG_POLL, "dec21x4xPollStart\n", 0, 0, 0, 0, 0, 0);

    intLevel = intLock();

    /* mask normal and abnormal interrupts */

    DEC_CSR_RESET (CSR7, CSR7_NIM | CSR7_AIM);

    /* set polling flag */

    DRV_FLAGS_SET (DEC_POLLING);

    intUnlock (intLevel);

    return (OK);
    }

/*******************************************************************************
*
* dec21x4xPollStop - stop polling mode
*
* This routine stops polling mode by enabling ethernet interrupts and
* resetting the polling flag in the END_CTRL structure.
*
* RETURNS: OK, always.
*/

LOCAL STATUS dec21x4xPollStop
    (
    DRV_CTRL *	pDrvCtrl
    )
    {
    int	intLevel;

    DRV_LOG (DRV_DEBUG_POLL, "dec21x4xPollStop\n", 0, 0, 0, 0, 0, 0);

    intLevel = intLock();

    /* restore normal and abnormal interrupts */
 
    DEC_CSR_UPDATE (CSR7, (CSR7_NIM | CSR7_AIM));

    /* clear polling flag */
 
    DRV_FLAGS_CLR (DEC_POLLING);

    intUnlock (intLevel);

    DRV_LOG (DRV_DEBUG_POLL, "s", 0, 0, 0, 0, 0, 0);

    return (OK);
    }

/*******************************************************************************
*
* dec21x4xPollSend - send a packet in polled mode
*
* This routine is called by a user to try and send a packet on the
* device. It sends a packet directly on the network from the caller without
* going through the normal processes of queuing a pacet on an output queue
* and the waiting for the device to decide to transmit it.
*
* If it detects a transmission error, the restart command is issued.
*
* These routine should not call any kernel functions.
*
* RETURNS: OK on success, EAGAIN on failure
*/

LOCAL STATUS dec21x4xPollSend
    (
    DRV_CTRL *	pDrvCtrl,
    M_BLK *	pMblk
    )
    {
    DEC_TD *	pTxD;
    char *	pBuf;
    int		len;

    DRV_LOG (DRV_DEBUG_POLL_TX, "dec21x4xPollSend\n", 0, 0, 0, 0, 0, 0);

    /* Get the next TXD */

    pTxD = dec21x4xTxDGet (pDrvCtrl);
    pBuf = NET_BUF_ALLOC ();

    if ((pTxD == NULL) || (pBuf == NULL))
        {
        dec21x4xTxRingClean (pDrvCtrl);
        if (pBuf)
	    NET_BUF_FREE (pBuf);
        return (EAGAIN);
        }

    /* copy and free the MBLK */

    len = netMblkToBufCopy (pMblk, pBuf, NULL);

    /* setup the transmit buffer pointers */
 
    pTxD->tDesc2 = PCISWAP (DEC_VIRT_TO_PCI (pBuf));
    pTxD->tDesc3 = 0;

    /* setup descriptor fields for tansmit */

    pTxD->tDesc0 = 0;
    pTxD->tDesc1 &= PCISWAP (~TDESC1_TBS1_MSK);
    pTxD->tDesc1 |= PCISWAP (TDESC1_TBS1_PUT(len));

    pTxD->tDesc0 = PCISWAP(TDESC0_OWN);      /* ready for transmit */

    /* Flush the write pipe */

    CACHE_PIPE_FLUSH();

    /* Advance our management index */

    pDrvCtrl->txIndex = (pDrvCtrl->txIndex + 1) % pDrvCtrl->numTds;

     /* start tx */

    if (DRV_FLAGS_ISSET(DEC_TX_KICKSTART))
        DEC_CSR_WRITE (CSR1, CSR1_TPD);

    /* The buffer does not belong to us; Spin until it can be freed */

    while (pTxD->tDesc0 & PCISWAP(TDESC0_OWN));

    NET_BUF_FREE (pBuf);

    /* Try again on transmit errors */

    if (pTxD->tDesc0 & PCISWAP(TDESC0_ES))
        return (EAGAIN);

    /* update statistics */
 
    END_ERR_ADD (&pDrvCtrl->endObj, MIB2_OUT_UCAST, +1);
 
    return (OK);
    }

/*******************************************************************************
*
* dec21x4xPollReceive - get a packet in polled mode
*
* This routine is called by a user to try and get a packet from the
* device. It returns EAGAIN if no packet is available. The caller must
* supply a M_BLK_ID with enough space to contain the receiving packet. If
* enough buffer is not available then EAGAIN is returned.
*
* These routine should not call any kernel functions.
*
* RETURNS: OK on success, EAGAIN on failure.
*/

LOCAL STATUS dec21x4xPollReceive
    (
    DRV_CTRL *	pDrvCtrl,
    M_BLK  *	pMblk
    )
    {
    DEC_RD *	pRxD;
    char *	pRxBuf;
    int		len;
    BOOL	gotOne=FALSE;

    DRV_LOG (DRV_DEBUG_POLL_RX, "dec21x4xPollReceive\n", 0, 0, 0, 0, 0, 0);

    /* Check if the M_BLK has an associated cluster */

    if ((pMblk->mBlkHdr.mFlags & M_EXT) != M_EXT)
        return (EAGAIN);

    while (((pRxD = dec21x4xRxDGet (pDrvCtrl)) != NULL) && !gotOne)
        {

        /* Check if the packet was received OK */

        if (pRxD->rDesc0 & PCISWAP (RDESC0_ES))
            {
            /* Update the error statistics and discard the packet */

            END_ERR_ADD (&pDrvCtrl->endObj, MIB2_IN_ERRS, +1);
            }
        else
            {
            len = DEC_FRAME_LEN_GET (PCISWAP(pRxD->rDesc0)) - ETH_CRC_LEN;

            if (pMblk->mBlkHdr.mLen >= len)
                {
                gotOne = TRUE;

		/* update statistics */

                END_ERR_ADD (&pDrvCtrl->endObj, MIB2_IN_UCAST, +1);

		/* Deal with memory alignment */

		if (((int) pMblk->mBlkHdr.mData & 0x3) == 0)
                    pMblk->mBlkHdr.mData += pDrvCtrl->offset;

		pMblk->mBlkHdr.mFlags |= M_PKTHDR; /* set the packet header */
		pMblk->mBlkHdr.mLen = len;	   /* set the data len */
		pMblk->mBlkPktHdr.len = len;	   /* set the total len */
 
                pRxBuf = (char *) DEC_PCI_TO_VIRT (PCISWAP (pRxD->rDesc2));
                DEC_CACHE_INVALIDATE (pRxBuf, len);
                memcpy ((char *)pMblk->mBlkHdr.mData , pRxBuf, len);
                }
            }

	/* mark the descriptor ready to receive */

        pRxD->rDesc0 = PCISWAP (RDESC0_OWN);

	/* flush the write pipe */

        CACHE_PIPE_FLUSH();

	/* advance management index */

        pDrvCtrl->rxIndex = (pDrvCtrl->rxIndex + 1) % pDrvCtrl->numRds;
        }

    return (gotOne ? OK : EAGAIN);
    }

/*******************************************************************************
*
* dec21x4xCsrRead - read a Command and Status Register
*
* RETURNS: contents of the CSR register.
*/

LOCAL ULONG dec21x4xCsrRead
    (
    ULONG	devAdrs,                     /* device base address */
    int		reg                          /* register to read */
    )
    {
    ULONG *	csrReg;
    ULONG	csrData;
    
    csrReg = (ULONG *)(devAdrs + (reg * DECPCI_REG_OFFSET));
    csrData = *csrReg;

    return (PCISWAP (csrData));
    }

/*******************************************************************************
*
* dec21x4xCsrWrite - write a Command and Status Register
*
* RETURNS: N/A
*/

LOCAL void dec21x4xCsrWrite
    (
    ULONG	devAdrs,
    int		reg,
    ULONG	value
    )
    {
    ULONG *	csrReg;

    csrReg = (ULONG *)(devAdrs + (reg * DECPCI_REG_OFFSET));
    *csrReg = PCISWAP (value);

    return;
    }

/*******************************************************************************
*
* dec21040AuiTpInit - initialize either AUI or 10BASE-T connection
*
* This function configures 10BASE-T interface. If the link pass state is 
* not achieved within two seconds then the AUI interface is initialized.
*
* RETURNS: N/A
*/

LOCAL void dec21040AuiTpInit
    (
    DRV_CTRL *	pDrvCtrl
    )
    {

    /* reset the SIA registers */

    DEC_CSR_WRITE (CSR13, 0);
    DEC_CSR_WRITE (CSR14, 0);
    DEC_CSR_WRITE (CSR15, 0);

    /* configure 10BASE-T */

    DEC_CSR_WRITE (CSR13, CSR13_CAC_CSR);    /* 10BT auto configuration */

    taskDelay (sysClkRateGet() * 2);         /* 2 seconds delay */

    /* configure AUI if 10BASE-T is not connected */

    if (DEC_CSR_READ (CSR12) & (CSR12_21040_LKF | CSR12_21040_NCR))
	{

        /* reset SIA registers */

	DEC_CSR_WRITE (CSR13, 0);            
	DEC_CSR_WRITE (CSR14, 0);
	DEC_CSR_WRITE (CSR15, 0);

	/* AUI auto configuration */

	DEC_CSR_WRITE (CSR13, (CSR13_AUI_TP | CSR13_CAC_CSR));
	}

    return;
    }

/*******************************************************************************
*
* dec21x4xEnetAddrGet - gets the ethernet address
*
* This routine gets the ethernet address by calling the appropriate
* EnetAddrGet() routine.
* 
* RETURNS: OK/ERROR
*
* SEE ALSO: dec20140EnetAddrGet(), dec2114xEnetAddrGet(),
* sysDec21x4xEnetAddrGet()
*/

LOCAL STATUS dec21x4xEnetAddrGet
    (
    DRV_CTRL *	pDrvCtrl,
    char *	enetAdrs
    )
    {
    STATUS retVal=ERROR;

    if (! DRV_FLAGS_ISSET (DEC_BSP_EADRS))
        {
	/* don't use BSP routine to get ethernet addresse */

        if (! DRV_FLAGS_ISSET (DEC_21040))
            retVal=dec2114xEnetAddrGet (pDrvCtrl, enetAdrs);
        else
            retVal=dec21040EnetAddrGet (pDrvCtrl, enetAdrs);
        }

    /* Use BSP routine to get ethernet addresse */
    
    if (retVal == ERROR)
        retVal=sysDec21x4xEnetAddrGet (pDrvCtrl->unit, enetAdrs);

    return (retVal);
    }


/*******************************************************************************
*
* dec21040EnetAddrGet - gets the ethernet address from the ROM register.
*
* This routine gets the ethernet address from the ROM register.
* This routine returns the ethernet address into the pointer supplied to it.
* 
* RETURNS: OK/ERROR
*/

LOCAL STATUS dec21040EnetAddrGet
    (
    DRV_CTRL *	pDrvCtrl,
    char * 	enetAdrs	/* pointer to the ethernet address */ 
    )
    {
    ULONG	csr9Value;		/* register to hold CSR9 */
    BOOL	eRomReady = FALSE;	/* ethernet ROM register state */
    int		ix;			/* index register */
    int		len = EADDR_LEN;

    DEC_CSR_WRITE (CSR9, 0);	/* reset rom pointer */

    while (len > 0)
	{
	for (ix = 0; ix < 10; ix++)	/* try at least 10 times */
	    {
	    if ((csr9Value = DEC_CSR_READ (CSR9)) & CSR9_21040_DNV)
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
            return (ERROR);
	}

    return (OK);
    }

/*******************************************************************************
*
* dec2114xEnetAddrGet - gets the ethernet address from the ROM register 
*
* This routine reads an ethernet address from the serial ROM. It supports
* legacy, ver 1/A, and 3.0 serial ROM formats.
* 
* RETURNS: OK on success, and ERROR if the ethernet address bytes cannot be
* read.
*/

LOCAL STATUS dec2114xEnetAddrGet
    (
    DRV_CTRL *	pDrvCtrl,
    char * 	enetAdrs	/* pointer to the ethernet address */ 
    )
    {
    USHORT	sromData;
    int		adrsOffset;
    int		len;

    /* Check if SROM is programmed. */

    sromData = dec2114xSromWordRead (pDrvCtrl, 0);
    if ( sromData == 0xFFFF ) 
	return (ERROR);

    sromData = dec2114xSromWordRead (pDrvCtrl, 13);

    /*
     * Set MAC address offset from the ROM format.
     *  Legacy ROMs have ethernet address start at offset 0,
     *  while the rest (ver 1/A and 3.0) have it at byte offset 20.
     */

    adrsOffset = (sromData == 0xAA55) ? 0: 10;

    for (len=EADDR_LEN; len; len-=2, adrsOffset++)
        {
        sromData = dec2114xSromWordRead (pDrvCtrl, adrsOffset);

	/* copy in byte order, not word order */

#if (_BYTE_ORDER == _LITTLE_ENDIAN)
        *enetAdrs++ = MSB(sromData);	/* ARM, x86, i960, etc */
        *enetAdrs++ = LSB(sromData);
#else
        *enetAdrs++ = LSB(sromData);
        *enetAdrs++ = MSB(sromData);
#endif
        }

    return (OK);
    }

/*******************************************************************************
*
* dec2114xSromWordRead - read 2 bytes from the serial ROM
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

LOCAL USHORT dec2114xSromWordRead
    (
    DRV_CTRL *	pDrvCtrl,
    UCHAR	lineCnt 	/* Serial ROM line Number */ 
    )
    {
    int		ix;		/* index register */
    int		loop;           /* address loop counter */
    ULONG	adrsBase;
    USHORT	romData;

    /* Is the line offset valid, and if so, how large is it */

    if (lineCnt > DEC2114X_SROM_SIZE)
    	return (ERROR);

    if (lineCnt < 64)
    	{
	loop = 6;
	lineCnt = lineCnt << 2;  /* Prepare lineCnt for processing */
    	}
    else
	loop = 8;

    /* Command the Serial ROM to the correct Line */

    /* Preamble */

    DEC_SROM_CMD_WRITE (0x0, 30);           /* Command 1 */
    DEC_SROM_CMD_WRITE (0x1, 50);           /* Command 2 */
    DEC_SROM_CMD_WRITE (0x3, 250);          /* Command 3 */
    DEC_SROM_CMD_WRITE (0x1, 150);          /* Command 4 */

    /* Command Phase */

    DEC_SROM_CMD_WRITE (0x5, 150);          /* Command 5 */
    DEC_SROM_CMD_WRITE (0x7, 250);          /* Command 6 */
    DEC_SROM_CMD_WRITE (0x5, 250);          /* Command 7 */
    DEC_SROM_CMD_WRITE (0x7, 250);          /* Command 8 */
    DEC_SROM_CMD_WRITE (0x5, 100);          /* Command 9 */
    DEC_SROM_CMD_WRITE (0x1, 150);          /* Command 10 */
    DEC_SROM_CMD_WRITE (0x3, 250);          /* Command 11 */
    DEC_SROM_CMD_WRITE (0x1, 100);          /* Command 12 */
    
    /* Address Phase */

    for (ix=0; ix < loop; ix++)
    	{
	adrsBase = ((lineCnt & 0x80) >> 5); 

	/* Write the command */

        DEC_SROM_CMD_WRITE (adrsBase | 0x1, 150);  /* Command 13 */
        DEC_SROM_CMD_WRITE (adrsBase | 0x3, 250);  /* Command 14 */
        DEC_SROM_CMD_WRITE (adrsBase | 0x1, 100);  /* Command 15 */

	lineCnt = lineCnt<<1;
    	}

    NSDELAY (150);

    /* Data Phase */

    romData = 0;
    for (ix=15; ix >= 0; ix--)
    	{
	/* Write the command */

        DEC_SROM_CMD_WRITE (0x3, 100);      /* Command 16 */

	/* Extract data */

    	romData |= (DEC_SROM_CMD_READ() << ix);
    	NSDELAY (150);                       /* Command 17 */

        DEC_SROM_CMD_WRITE (0x1, 250);      /* Command 18 */
    	}

    /* Finish up command */

    DEC_SROM_CMD_WRITE (0x0, 100);           /* Command 19 */

    return (PCISWAP_SHORT(romData));
    }


/*******************************************************************************
*
* dec21x4xChipReset - Reset the chip and setup CSR0 register
*
* This routine stops the transmitter and receiver, masks all interrupts, then
* resets the ethernet chip. Once the device comes out of the reset state, it
* initializes CSR0 register.
*
* RETURNS: OK, Always. 
*/

LOCAL STATUS dec21x4xChipReset
    (
    DRV_CTRL *	pDrvCtrl	/* pointer to device control structure */
    )
    {
    ULONG	ix;				/* index */
    ULONG	usrFlags=pDrvCtrl->usrFlags;
    ULONG	csr0Val=0x0;
	
    DEC_CSR_WRITE (CSR6, 0);		/* stop rcvr & xmitter */

    DEC_CSR_WRITE (CSR7, 0);		/* mask interrupts */
    
    DEC_CSR_WRITE (CSR0, CSR0_SWR);

    /* Wait Loop, Loop for at least 50 PCI cycles according to chip spec */

    for (ix = 0; ix < 0x1000; ix++);

    /* Decode user setting into CSR0 bits */

    csr0Val |= (usrFlags & DEC_USR_BAR_RX)? 0 : CSR0_BAR;
    csr0Val |= (usrFlags & DEC_USR_BE)? CSR0_BLE : 0;
    csr0Val |= (usrFlags & DEC_USR_TAP_MSK) << DEC_USR_TAP_SHF;
    csr0Val |= (usrFlags & DEC_USR_PBL_MSK) << DEC_USR_PBL_SHF;
    csr0Val |= (usrFlags & DEC_USR_RML)? CSR0_2114X_RML : 0;

    if (usrFlags & DEC_USR_CAL_MSK)
        csr0Val |= (usrFlags & DEC_USR_CAL_MSK) << DEC_USR_CAL_SHF;
    else
        {
        csr0Val &= ~(CSR0_CAL_MSK | CSR0_PBL_MSK);
        csr0Val |= (CSR0_CAL_32 | CSR0_PBL_32);
        }
    
    if (! (usrFlags & DEC_USR_TAP_MSK))
        DRV_FLAGS_SET (DEC_TX_KICKSTART);

    DEC_CSR_WRITE (CSR0, csr0Val);

    for (ix = 0; ix < 0x1000; ix++);		/* loop for some cycles */

    return (OK);
    }

/*******************************************************************************
*
* dec21140MediaInit - Initialize media information
*
* This routine initializes media information for dec21140 chip.
*
* RETURNS: OK or ERROR
*/

LOCAL STATUS dec21140MediaInit
    (
    DRV_CTRL *	pDrvCtrl,
    UCHAR *	pSromData
    )
    {
    USHORT *	pSromWord = (USHORT *)pSromData;
    UCHAR *	pInfoLeaf0;
    UCHAR	ix;
    
    /* read the serial ROM into pSromData */

    for (ix=0; ix<DEC2114X_SROM_WORDS; ix++)
        *pSromWord++ = dec2114xSromWordRead (pDrvCtrl, ix);
    
    /* Initialize the media summary */

    if (pDrvCtrl->mediaCount == 0xFF)
        {
        /* check version */

        if (SROM_VERSION( pSromData ) < DEC2114X_SROM_VERSION_3)
            return (ERROR);

        /* get the leaf offset */

        pInfoLeaf0 = pSromData + SROM_ILEAF0_OFFSET( pSromData );

        pDrvCtrl->gprModeVal = ILEAF_21140_GPR_MODE( pInfoLeaf0 );
        pDrvCtrl->mediaCount = ILEAF_21140_MEDIA_COUNT( pInfoLeaf0 );

        pDrvCtrl->mediaCurrent = 0xFF;
        pDrvCtrl->mediaDefault = 0xFF;

        DRV_LOG (DRV_DEBUG_LOAD, "gpMask=%#x mediaCount = %d\n",
                    pDrvCtrl->gprModeVal, pDrvCtrl->mediaCount, 0, 0, 0, 0);
        }
    return (OK);
    }

/*******************************************************************************
*
* dec21140MediaSelect - select the current media 21140
*
* This routine reads and sets up physical media with configuration
* information from a Version 3 DEC Serial ROM. Any other media configuration
* can be supported by initializing <_func_dec2114xMediaSelect>.
*
* RETURN: OK or ERROR
*/

LOCAL STATUS dec21140MediaSelect
    (
    DRV_CTRL *	pDrvCtrl,
    UINT *	pCsr6Val
    )
    {
    UCHAR	sromData[128];
    UCHAR *	pInfoLeaf0;
    UCHAR *	pInfoBlock;
    UINT8	ix;

    DRV_LOG (DRV_DEBUG_LOAD, " dec21140MediaSelect\n", 0, 0, 0, 0, 0, 0);

    if (dec21140MediaInit (pDrvCtrl, sromData) == ERROR)
        return (ERROR);

    /* Get Info Leaf 0 */

    pInfoLeaf0 = sromData + SROM_ILEAF0_OFFSET (sromData);

    /* Get the current media */

    if ((pDrvCtrl->mediaCurrent == 0) ||
        (pDrvCtrl->mediaCurrent >= pDrvCtrl->mediaCount))
        pDrvCtrl->mediaCurrent = pDrvCtrl->mediaCount - 1;
    else
        pDrvCtrl->mediaCurrent --;

    /* Seek to the correct media Info Block */

    pInfoBlock = ILEAF_21140_INFO_BLK0( pInfoLeaf0);
    for (ix=0; ix<pDrvCtrl->mediaCurrent; ix++)
        {
        if (IBLK_IS_COMPACT (pInfoBlock))
            pInfoBlock += IBLK_COMPACT_SIZE;
        else
            pInfoBlock += IBLK_EXT_SIZE (pInfoBlock) + 1;
        }

    /* Convert ext0 into compact */

    if (IBLK_IS_EXT0 (pInfoBlock))
        pInfoBlock = IBLK_EXT0_TO_COMPACT (pInfoBlock);

    /* Setup the GP port */

    DEC_CSR_WRITE (CSR12, CSR12_21140_GPC | pDrvCtrl->gprModeVal);
    NSDELAY (150);
    
    if (IBLK_IS_COMPACT (pInfoBlock))
        {
        USHORT	compactCmd;

        DRV_LOG ( DRV_DEBUG_LOAD,
                    "COMPACT: mediaCode=%#x gpData=%#x command=%#x\n",
                     IBLK_COMPACT_MCODE(pInfoBlock),
                     IBLK_COMPACT_GPDATA(pInfoBlock),
                     IBLK_COMPACT_CMD(pInfoBlock), 0, 0, 0);

        /* Initialize the PHY interface */

        DEC_CSR_WRITE (CSR12, IBLK_COMPACT_GPDATA(pInfoBlock));
        NSDELAY(150);

        /* Get CSR6 settings */

        compactCmd = IBLK_COMPACT_CMD( pInfoBlock );
        if (compactCmd & COMPACT_CMD_SCR)
            *pCsr6Val |= CSR6_2114X_SCR;
        if (compactCmd & COMPACT_CMD_PCS)
            *pCsr6Val |= CSR6_2114X_PCS;
        if (compactCmd & COMPACT_CMD_PS)
            *pCsr6Val |= CSR6_2114X_PS;
        }
    else
        {
        USHORT	mediaCap;
        UCHAR *	pGpStr;

        DRV_LOG (DRV_DEBUG_LOAD,
                   "EXT1: PHY#=%#x InitLen=%#x resetLen=%#x \n",
                    IBLK_EXT1_PHY(pInfoBlock),
                    IBLK_EXT1_INIT_LEN(pInfoBlock),
                    IBLK_EXT1_RESET_LEN(pInfoBlock), 0, 0, 0);
        DRV_LOG (DRV_DEBUG_LOAD,
                   "mediaCap=%#x autoAd=%#x FDMap=%#x TTMmap=%#x\n",
                    IBLK_EXT1_MEDIA_CAP(pInfoBlock),
                    IBLK_EXT1_AUTO_AD(pInfoBlock),
                    IBLK_EXT1_FD_MAP(pInfoBlock),
                    IBLK_EXT1_TTM_MAP(pInfoBlock), 0, 0);

        /* Reset the media */

        pGpStr = IBLK_EXT1_RESET_STR (pInfoBlock);
        for (ix=IBLK_EXT1_RESET_LEN(pInfoBlock); ix; ix--, pGpStr++)
            DEC_CSR_WRITE (CSR12, *pGpStr);
        NSDELAY(150);
        
        /* Initialize the media */

        pGpStr = IBLK_EXT1_INIT_STR (pInfoBlock);
        for (ix=IBLK_EXT1_INIT_LEN (pInfoBlock); ix; ix--, pGpStr++)
            DEC_CSR_WRITE (CSR12, *pGpStr);
        NSDELAY(150);
        
	/* 
         * Get CSR6 settings:
	 * select full duplex mode only if the user esplicitely 
	 * asks it and the device has that capability.
	 */

        mediaCap = IBLK_EXT1_MEDIA_CAP (pInfoBlock);
	if ((IBLK_EXT1_FD_MAP (pInfoBlock) & mediaCap) &&
	    ((pDrvCtrl->usrFlags & DEC_USR_FD) == DEC_USR_FD))
	    *pCsr6Val |= CSR6_FD;

        if (IBLK_EXT1_TTM_MAP (pInfoBlock) & mediaCap)
            *pCsr6Val |= CSR6_2114X_TTM;

        *pCsr6Val |= CSR6_2114X_PS;
        }
    
    return (OK);
    }

/*******************************************************************************
*
* dec21143MediaInit - Initialize media information
*
* This routine initializes media information for dec21143 chip.
*
* RETURNS: OK or ERROR
*/
 
LOCAL STATUS dec21143MediaInit
    (
    DRV_CTRL *  pDrvCtrl,
    UCHAR *     pSromData
    )
    {
    USHORT *    pSromWord = (USHORT *)pSromData;
    UCHAR *     pInfoLeaf0;
    UCHAR       ix;
   
    /* read the serial ROM into pSromData */
 
    for (ix=0; ix < DEC2114X_SROM_WORDS; ix++)
        *pSromWord++ = dec2114xSromWordRead (pDrvCtrl, ix);
   
    /* Initialize the media summary */
 
    if (pDrvCtrl->mediaCount == 0xFF)
        {
        /* check version */
 
        if (SROM_VERSION( pSromData ) < DEC2114X_SROM_VERSION_3)
            return (ERROR);
 
        /* get the leaf offset */
 
        pInfoLeaf0 = pSromData + SROM_ILEAF0_OFFSET(pSromData);
 
        pDrvCtrl->mediaCount = ILEAF_21143_MEDIA_COUNT(pInfoLeaf0);
 
        pDrvCtrl->mediaCurrent = 0xFF;
        pDrvCtrl->mediaDefault = 0xFF;
 
        DRV_LOG (DRV_DEBUG_LOAD, "mediaCount = %d\n", 
			pDrvCtrl->mediaCount, 0, 0, 0, 0, 0);
        }
    return (OK);
    }
 

/*******************************************************************************
*
* dec21143MediaSelect - select the current media for dec21143
*
* This routine reads and sets up physical media with configuration
* information from a Version 3 DEC Serial ROM. Any other media configuration
* can be supported by initializing <_func_dec2114xMediaSelect>.
*
* RETURN: OK or ERROR
*/

LOCAL STATUS dec21143MediaSelect
    (
    DRV_CTRL *	pDrvCtrl,
    UINT *	pCsr6Val
    )
    {
    UCHAR       sromData[128];
    UCHAR *     pInfoLeaf0;
    UCHAR *     pInfoBlock;
    USHORT	mediaCap;
    UCHAR *	pGpStr;
    UINT	mediaCmd;
    UINT8       ix;


    DRV_LOG (DRV_DEBUG_LOAD, " dec21143MediaSelect\n", 0, 0, 0, 0, 0, 0);

    if (dec21143MediaInit (pDrvCtrl, sromData) == ERROR)
        return (ERROR);
 
    /* Get Info Leaf 0 */
 
    pInfoLeaf0 = sromData + SROM_ILEAF0_OFFSET (sromData);
 
    /* Get the current media */
 
    if ((pDrvCtrl->mediaCurrent == 0) ||
        (pDrvCtrl->mediaCurrent >= pDrvCtrl->mediaCount))
        pDrvCtrl->mediaCurrent = pDrvCtrl->mediaCount - 1;
    else
        pDrvCtrl->mediaCurrent --;
 
    /* Seek to the correct media Info Block */
 
    pInfoBlock = ILEAF_21143_INFO_BLK0( pInfoLeaf0);
    for (ix=0; ix < pDrvCtrl->mediaCurrent; ix++)
        pInfoBlock += IBLK_EXT_SIZE (pInfoBlock) + 1;

    if (IBLK_IS_EXT(pInfoBlock))
	switch (IBLK_EXT_TYPE(pInfoBlock))
	    {

	    case IBLK_IS_EXT2 :		/* Extended format block - type 2 */

		DRV_LOG ( DRV_DEBUG_LOAD, "EXT2: mediaCode=%#x EXT=%#x\n",
			IBLK_EXT2_MCODE(pInfoBlock),
			IBLK_EXT2_EXT(pInfoBlock), 0, 0, 0, 0);

		/* get Media-specific data */

		if (IBLK_EXT2_EXT(pInfoBlock))
		    {
		    DRV_LOG (DRV_DEBUG_LOAD, 
			"CSR13=%#x CSR14=%#x CSR15=%#x\n",
			     IBLK_EXT2_MSD_CSR13(pInfoBlock),
			     IBLK_EXT2_MSD_CSR13(pInfoBlock),
			     IBLK_EXT2_MSD_CSR13(pInfoBlock), 0, 0, 0);

		    DEC_CSR_WRITE (CSR13, IBLK_EXT2_MSD_CSR13(pInfoBlock));
		    DEC_CSR_WRITE (CSR14, IBLK_EXT2_MSD_CSR13(pInfoBlock));
		    DEC_CSR_WRITE (CSR15, IBLK_EXT2_MSD_CSR13(pInfoBlock));
		    }

		/* setup the GP port */

		DRV_LOG ( DRV_DEBUG_LOAD, "GP Ctrl=%#x GP Data=%#x\n",
			IBLK_EXT2_GPC(pInfoBlock),
			IBLK_EXT2_GPD(pInfoBlock), 0, 0, 0, 0);

		DEC_CSR_UPDATE(CSR15, IBLK_EXT2_GPC(pInfoBlock) << 8);
		DEC_CSR_UPDATE(CSR15, IBLK_EXT2_GPD(pInfoBlock) << 8);
		NSDELAY(150);

		break;
		
	    case IBLK_IS_EXT3 :		/* Extended format block - type 3 */

		DRV_LOG (DRV_DEBUG_LOAD,
			"EXT3: PHY#=%#x InitLen=%#x resetLen=%#x \n",
				IBLK_EXT3_PHY(pInfoBlock),
				IBLK_EXT3_INIT_LEN(pInfoBlock),
				IBLK_EXT3_RESET_LEN(pInfoBlock), 0, 0, 0);
		DRV_LOG (DRV_DEBUG_LOAD,
			"mediaCap=%#x autoAd=%#x FDMap=%#x
			TTMmap=%#x MIIci=%#x\n",
				IBLK_EXT3_MEDIA_CAP(pInfoBlock),
				IBLK_EXT3_AUTO_AD(pInfoBlock),
				IBLK_EXT3_FD_MAP(pInfoBlock),
				IBLK_EXT3_TTM_MAP(pInfoBlock),
				IBLK_EXT3_MII_CI(pInfoBlock), 0);
 
		/* Reset the media */
 
		pGpStr = IBLK_EXT3_RESET_STR (pInfoBlock);
		for (ix=IBLK_EXT3_RESET_LEN(pInfoBlock); ix; ix--, pGpStr++)
		    DEC_CSR_WRITE (CSR15, *pGpStr);
		NSDELAY(150);
 
		/* Initialize the media */
 
		pGpStr = IBLK_EXT3_INIT_STR (pInfoBlock);
		for (ix=IBLK_EXT3_INIT_LEN (pInfoBlock); ix; ix--, pGpStr++)
		    DEC_CSR_WRITE (CSR15, *pGpStr);
		NSDELAY(150);
 
		/* 
		 * Get CSR6 settings:
		 * select full duplex mode only if the user esplicitely 
		 * asks it and the device has that capability.
		 */

		mediaCap = IBLK_EXT1_MEDIA_CAP (pInfoBlock);
		if ((IBLK_EXT1_FD_MAP (pInfoBlock) & mediaCap) &&
		    ((pDrvCtrl->usrFlags & DEC_USR_FD) == DEC_USR_FD))
		    *pCsr6Val |= CSR6_FD;

		if (IBLK_EXT3_TTM_MAP (pInfoBlock) & mediaCap)
		    *pCsr6Val |= CSR6_2114X_TTM;
 
		*pCsr6Val |= CSR6_2114X_PS;

		break;

	    case IBLK_IS_EXT4 :		/* Extended format block - type 4 */

		DRV_LOG ( DRV_DEBUG_LOAD, 
			"EXT4: mediaCode=%#x GPCtrl=%#x GPData=%#x Cmd=%#x\n",
				IBLK_EXT4_MCODE(pInfoBlock),
				IBLK_EXT4_GPC(pInfoBlock),
				IBLK_EXT4_GPD(pInfoBlock),
				IBLK_EXT4_CMD(pInfoBlock), 0, 0);

		/* setup the GP port */

		DEC_CSR_UPDATE (CSR15, IBLK_EXT4_GPC(pInfoBlock) << 8);
		DEC_CSR_UPDATE (CSR15, IBLK_EXT4_GPD(pInfoBlock) << 8);
		NSDELAY(150);

		/* Get CSR6 settings */

		mediaCmd = IBLK_EXT4_CMD(pInfoBlock);

		if (mediaCmd & IBLK_EXT4_CMD_PS)
		    *pCsr6Val |= CSR6_2114X_PS;
		if (mediaCmd & IBLK_EXT4_CMD_TTM)
		    *pCsr6Val |= CSR6_2114X_TTM;
		if (mediaCmd & IBLK_EXT4_CMD_PCS)
		    *pCsr6Val |= CSR6_2114X_PCS;
		if (mediaCmd & IBLK_EXT4_CMD_SCR)
		    *pCsr6Val |= CSR6_2114X_SCR;

		break;

	    case IBLK_IS_EXT5 :		/* Extended format block - type 5 */

		DRV_LOG ( DRV_DEBUG_LOAD, "EXT5: Rst Seq length=%#x\n",
				IBLK_EXT5_RESET_LEN(pInfoBlock), 0, 0, 0, 0, 0);

		/* Reset the media */

		/* not implemented */

		break;

	    default :
		return (ERROR);

	    }
    else
	return (ERROR);
 
    return (OK);
    }

/*******************************************************************************
*
* dec21x4xInitialReset - perform an initial reset on the DEC chip
*
* This routine performs an initial reset on the DEC chip having started a
* transmission. This is to prevent subsequent transmissions from hanging
* up.
*
* RETURN: N/A
*/

LOCAL void dec21x4xInitialReset(DRV_CTRL* pDrvCtrl)
    {
    char  dummy_buffer[DUMMY_BUFFER_SIZE];
    ULONG tmd[4];

    /* Fill in the dummy buffer transmit descriptor */

    tmd[TDESC0] = PCISWAP (TDESC0_OWN);
    tmd[TDESC1] = PCISWAP (TDESC1_IC  |
		           TDESC1_LS  |
		           TDESC1_FS  |
		           TDESC1_TER |
		           (DUMMY_BUFFER_SIZE & TDESC1_TBS1_MSK));
    tmd[TDESC2] = PCISWAP ((ULONG) dummy_buffer);
    tmd[TDESC3] = 0;

    /* Start the transmission */

    DEC_CSR_WRITE (CSR4, (ULONG) &tmd);
    DEC_CSR_WRITE (CSR7, 0);
    DEC_CSR_WRITE (CSR6, CSR6_2114X_MB1 |
			 CSR6_2114X_TTM |
			 CSR6_2114X_SF  |
			 CSR6_2114X_PS  |
			 CSR6_ST        |
			 CSR6_OM_INT);
    DEC_CSR_WRITE (CSR1, CSR1_TPD);

    DELAY (0x8000);

    /* Reset the device while transmitting */

    DEC_CSR_WRITE (CSR0, DEC_CSR_READ(CSR0) | CSR0_SWR);
    }

#ifdef DRV_DEBUG

/*******************************************************************************
*
* dec21x4xDebugCsrShow - show CSRs register
*
* This routine shows CSRs registers used for host communication. 
*
* RETURNS: N/A
*
* NOMANUAL
*/
 
void dec21x4xDebugCsrShow (void)
    {
    ULONG devAdrs = pDrvCtrlDbg->devAdrs;
    
    printf ("\n");
    printf ("CSR0\t 0x%x\n", (int) dec21x4xCsrRead(devAdrs, CSR0));
    printf ("CSR1\t 0x%x\n", (int) dec21x4xCsrRead(devAdrs, CSR1));
    printf ("CSR2\t 0x%x\n", (int) dec21x4xCsrRead(devAdrs, CSR2));
    printf ("CSR3\t 0x%x\n", (int) dec21x4xCsrRead(devAdrs, CSR3));
    printf ("CSR4\t 0x%x\n", (int) dec21x4xCsrRead(devAdrs, CSR4));
    printf ("CSR5\t 0x%x\n", (int) dec21x4xCsrRead(devAdrs, CSR5));
    printf ("CSR6\t 0x%x\n", (int) dec21x4xCsrRead(devAdrs, CSR6));
    printf ("CSR7\t 0x%x\n", (int) dec21x4xCsrRead(devAdrs, CSR7));
    printf ("CSR8\t 0x%x\n", (int) dec21x4xCsrRead(devAdrs, CSR8));
    printf ("CSR9\t 0x%x\n", (int) dec21x4xCsrRead(devAdrs, CSR9));
    printf ("CSR10\t 0x%x\n",(int) dec21x4xCsrRead(devAdrs, CSR10));
    printf ("CSR11\t 0x%x\n",(int) dec21x4xCsrRead(devAdrs, CSR11));
    printf ("CSR12\t 0x%x\n",(int) dec21x4xCsrRead(devAdrs, CSR12));
    printf ("CSR13\t 0x%x\n",(int) dec21x4xCsrRead(devAdrs, CSR13));
    printf ("CSR14\t 0x%x\n",(int) dec21x4xCsrRead(devAdrs, CSR14));
    printf ("CSR15\t 0x%x\n",(int) dec21x4xCsrRead(devAdrs, CSR15));
    }

/*******************************************************************************
*
* dec21x4xDebugTxDShow - show the transmit descriptor's informations
*
* This routine shows the informations of a specific transmit descriptor.
*
* RETURNS: N/A
*
* NOMANUAL
*/

void dec21x4xDebugTxDShow
    (
    int numTxD
    )
    {
    DEC_TD	*pTxD = &pDrvCtrlDbg->txRing[numTxD];

    printf ("txd[%d] at 0x%x:\n", numTxD, (int)pTxD);
    printf ("tDesc0=0x%x tDesc1=0x%x tDesc2=0x%x tDesc3=0x%x\n",
            (int) PCISWAP (pTxD->tDesc0), (int) PCISWAP (pTxD->tDesc1),
            (int) PCISWAP (pTxD->tDesc2), (int) PCISWAP (pTxD->tDesc3));
    }

/*******************************************************************************
*
* dec21x4xDebugRxDShow - show the receive descriptor's informations
*
* This routine shows the informations of a specific receive descriptor.
*
* RETURNS: N/A
*
* NOMANUAL
*/

void dec21x4xDebugRxDShow
    (
    int numRxD
    )
    {
    DEC_RD	*pRxD = &pDrvCtrlDbg->rxRing[numRxD];

    printf ("rxd[%d] at 0x%x:\n", numRxD, (int)pRxD);
    printf ("rDesc0=0x%x rDesc1=0x%x rDesc2=0x%x rDesc3=0x%x\n",
            (int) PCISWAP (pRxD->rDesc0), (int) PCISWAP (pRxD->rDesc1),
            (int) PCISWAP (pRxD->rDesc2), (int) PCISWAP (pRxD->rDesc3));
    }

/*******************************************************************************
*
* dec21x4xDebugShow - show the status of the control structure 
*
* This routine shows the current status of the driver control structure.
*
* RETURNS: N/A
*
* NOMANUAL
*/

void dec21x4xDebugShow(void)
    {
    printf ("pDrvCtrl=0x%x txNum=%d txIndex=%d txDiIndex=%d\n",
            (int) pDrvCtrlDbg,
            pDrvCtrlDbg->numTds,
            pDrvCtrlDbg->txIndex,
            pDrvCtrlDbg->txDiIndex);
    
    printf ("rxNum=%d rxIndex=%d\n",
            pDrvCtrlDbg->numRds,
            pDrvCtrlDbg->rxIndex);
    }

/*******************************************************************************
*
* dec21x4xDebugEAdrsDisplay - display the ethernet address
*
* This routine display the physical ethernet address of the board.
*
* RETURNS: N/A
*
* NOMANUAL
*/

void dec21x4xDebugEAdrsDisplay
    (
    UINT8 *pAddr
    )
    {
    printf ("EtherAddr=%x:%x:%x:%x:%x:%x\n",
            pAddr[0], pAddr[1], pAddr[2], pAddr[3], pAddr[4], pAddr[5]);
    }

/*******************************************************************************
*
* dec21x4xDebugMcAdd - Add a multicast address
*
* This routine adds a multicast address to whatever the driver
* is already listening for.
*
* RETURNS: N/A
*
* NOMANUAL
*/

void dec21x4xDebugMcAdd (char *eAddr)
    {
    dec21x4xMCastAddrAdd (pDrvCtrlDbg, eAddr);
    }

/*******************************************************************************
*
* dec21x4xDebugMcDel - delete a multicast address
*
* This routine deletes a multicast address from the current list of
* multicast addresses.
*
* RETURNS: N/A
*
* NOMANUAL
*/

void dec21x4xDebugMcDel (char *eAddr)
    {
    dec21x4xMCastAddrDel (pDrvCtrlDbg, eAddr);
    }

/*******************************************************************************
*
* dec21x4xDebugMcShow - show all multicast addresses
*
* This routine shows all multicast addresses from the current list of
* multicast addresses.
*
* RETURNS: N/A
*
* NOMANUAL
*/

void dec21x4xDebugMcShow (void)
    {
    ETHER_MULTI  *pMCastNode;

    pMCastNode = END_MULTI_LST_FIRST (&pDrvCtrlDbg->endObj);
    while (pMCastNode != NULL)
        {
        dec21x4xDebugEAdrsDisplay (pMCastNode->addr);
        pMCastNode = END_MULTI_LST_NEXT (pMCastNode);
        }
    }

char dec21x4xDebugSerialRom[128];

/*******************************************************************************
*
* dec21x4xDebugSerialRomUpload - load data from the serial rom 
*
* This routine loads the serial rom into an array. 
*
* RETURNS: N/A
*
* NOMANUAL
*/

void dec21x4xDebugSerialRomUpload(void)
    {
    int 	count;
    USHORT 	*pSerialRom = (USHORT *)dec21x4xDebugSerialRom;

    memset (dec21x4xDebugSerialRom, 0, 128);
    for (count=0; count<64; count++)
        *pSerialRom++ = dec2114xSromWordRead (pDrvCtrlDbg, count);
    }

/*******************************************************************************
*
* dec21x4xDebugReset - stop and restart the driver 
*
* This routine stops and restarts the driver
*
* RETURNS: OK always
*
* NOMANUAL
*/

int dec21x4xDebugReset(void)
    {
    dec21x4xStop (pDrvCtrlDbg);
    taskDelay (2);
    netJobAdd ((FUNCPTR)dec21x4xRestart, (int)pDrvCtrlDbg, 0, 0, 0, 0);
    return (OK);
    }

#endif /* DRV_DEBUG */
