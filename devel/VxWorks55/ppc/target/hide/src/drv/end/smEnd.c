/* smEnd.c - END shared memory backplane network interface driver */

/* Copyright 1984-1999 Wind River Systems, Inc. */

/*
modification history
--------------------
01a,20jan1999,mas  written based on if_sm.c version 01k but with no Unix.
*/

/*
DESCRIPTION
This module implements the VxWorks shared memory backplane network
interface enhanced network driver.  

This driver is designed to be moderately generic, operating unmodified
across most targets supported by VxWorks.  To achieve this, the driver must
be given several target-specific parameters, and some external support
routines must be provided.  These parameters are detailed below.

There are no user-callable routines.

This driver is layered between the shared memory packet library and the Mux
modules.  The backplane driver gives CPUs residing on a common backplane 
the ability to communicate using IP (via shared memory).

Macros:

The macro SYS_INT_ENABLE is used to enable the interrupt level for the
end device.  It is called once during initialization.  By default this is
the routine sysLanIntEnable(), defined in the module sysLib.o.

The macro SYS_ENET_ADDR_GET is used to get the ethernet address (MAC)
for the device.  The single argument to this routine is the END_DEVICE
pointer.  By default this routine copies the ethernet address stored in
the global variable sysTemplateEnetAddr into the END_DEVICE structure.

INCLUDES:
end.h endLib.h etherMultiLib.h

INTERNAL

Data flow:

		 -------		
		 | MUX |		| multiplexer: interface
		 -------		| to network drivers
		    |			|
		    |    		|
	      --------------		|
	      |   smEnd	   |		| shared memory
	      --------------		| backplane driver
		    |			|
		    |			|
	       ------------		|
	       | smPktLib |		| shared memory data
	       ------------		| packetizer library
		    |			|
		    |			|
	      -------------		|
	      | smUtilLib |		| H/W abstraction
	      -------------		|
		    |			|
		    |			|
	===========================	| backplane

MUX

Implements the interface between the protocol layer and the data link layer
which contains the driver.

smEnd

Contains the network interface code that is common to all END based
systems.  It is the interface between the network modules and the backplane.
It does the packet processing.

smPktLib

Reformats data between IP packets and shared memory buffer packets.

smUtilLib

Contains routines to abstract the hardware backplane, giving a uniform
interface to any type of backplane.


MUX Interface:

The interfaces into this module from the MUX module follow.  

smEndLoad -  Called by the MUX, the routine initializes and attaches this
shared memory network interface driver to the MUX.  It is the only globally
accessible entry into this driver.  This routine typically gets called twice
per backplane interface (device) and accepts a pointer to a string of
initialization parameters.  The first call to this routine will be made with
an empty string; it signals the routine to return a device name, not to load
and intialize the driver.  The second call will be with a valid parameter
string, signalling that the driver is to be loaded and initialized with the
parameter values in the string.  The shared memory region must have been
setup and initialized (via smPktSetup) prior to calling smEndLoad().
Although initialized, no devices will become active until smEndStart() is
called.

The following routines are all local to this driver but are listed in the
driver entry function table:

smEndUnload -  Called by the MUX, this routine stops all associated devices,
frees driver resources, and prepares this driver to be unloaded.  If required,
calls to smEndStop() will be made to all active devices.

smEndStart -  Called by the MUX, the routine starts this driver and device(s).
The routine activates this driver and its device(s).  The activities performed
are dependent upon the selected mode of operation, interrupt or polled.

smEndStop -  Called by the MUX, the routine stops this driver by inactivating
the driver and its associated device(s).  Upon completion of this routine,
this driver is left in the same state it was just after smEndLoad() execution.

smEndRecv -  This routine is not called from the MUX.  It gets called from
this drivers interrupt service routine (ISR) to process input shared memory
packets.  It then passes them on to the MUX.

smEndSend -  Called by the MUX, this routine sends a packet via shared memory.

smEndPollRecv - Called by the MUX, this routine polls the shared memory region
designated for this CPU to determine if any new packet buffers are available
to be read.  If so, it reads the packet into the supplied mBlk and returns OK
to the MUX.  If the packet is too big for the mBlk or if no packets are
available, EAGAIN is returned.  If the device is not in polled mode, EIO is
returned.

smEndPollSend -  Called by the MUX, this routine does a polled send of one
packet to shared memory.  Because shared memory buffers act as a message queue,
this routine will attempt to put the polled mode packet at the head of the list
of buffers.  If no free buffers are available, the buffer currently appearing
first in the list is overwritten with the packet.

smEndIoctl -  Called by the MUX, the routine accesses the control routines for
this driver.

smEndMCastAddrAdd -  Called by the MUX, this routine adds an address to a
device's multicast address list.

smEndMCastAddrDel -  Called by the MUX, this routine deletes an address from a
device's multicast address list.

smEndMCastAddrGet -  Called by the MUX, this routine gets the multicast address
list maintained for a specified device.

The following routines do not require shared memory specific logic so the
default END library routines are referenced in the function table:

endEtherAddressForm -  Called by the MUX, this routine forms an address by
adding appropriate link-level (shared memory) information to a specified mBlk
in preparation for transmission.

endEtherPacketDataGet -  Called by the MUX, this routine derives the protocol
specific data within a specified mBlk by stripping the link-level (shared
memory) information from it.  The resulting data are copied to another mBlk.

endEtherPacketAddrGet -  Called by the MUX, this routine extracts address
information from one mBlk, ignoring all other data.  Each source and
destination address is written to its own mBlk.  For ethernet packets, this
routine produces two output mBlks (an address pair).  However, for non-ethernet
packets, up to four mBlks (two address pairs) may be produced; two for an
intermediate address pair and two more for the terminal address pair.

.LP

EXTERNAL SUPPORT REQUIREMENTS
The following routines/macros must be provided for this module: 

sysSmBspLoad()	- a routine to dynamically build a parameter string
		  for, and call into, smEndLoad().  This routine is typically
		  contained within the sysSmCfg.c BSP module.

The following routines/macros may be optionally provided for this module: 

.IP SM_PACKET_XFER()
.CS
    void SM_PACKET_XFER (void * source, void * destination, int numBytes);
.CE
This macro transfers data between clusters and SM packets.  It may optionally
be redefined in the BSP.  By default, bcopy() is used:

    #define SM_PACKET_XFER(source,destination,numBytes)  \\
        bcopy((char*)source, (char*)destination, (int)numBytes)


TARGET-SPECIFIC PARAMETERS
These parameters are input to this driver in an ASCII string format, using
colon delimited values, via the sysSmBspLoad() routine in sysSmCfg.c.  Each
parameter have a preselected radix in which they are expected to be read as
shown below.

The sysSmCfg.c module is included in sysLib.c and is compiled with the current
settings for the target-specific parameters defined by the following:

Parameter         Radix Use
----------------- ----- ------------------------------------------------------
SM_BUS_SPACE       16   bus address space; NONE = local, else, a VME AM or PCI
                        space selector   [**not yet supported in smPktLib.c**]
SM_ANCHOR_ADRS     16   shared memory anchor region  @<SM_BUS_SPACE>
SM_MEM_ADRS        16   shared memory (NONE = allocate)  @<SM_BUS_SPACE>
			This value is used by the master CPU when building sm.
SM_MEM_SIZE        16   total shared memory size in bytes
			This value is used by the master CPU when building sm.
SM_TAS_TYPE        10   test-and-set type (SM_TAS_HARD or SM_TAS_SOFT)
SM_CPUS_MAX        10   maximum number of CPUs supported on backplane
			(0 = default number)
SM_MASTER_CPU      10   master CPU#
SM_PKTS_SIZE       10   max number of data bytes per shared memory packet
			(0 = default)
SM_MAX_INPUT_PKTS  10   max number of queued receive packets for this CPU
			(0 = default)
SM_INT_TYPE        10   interrupt method (SM_INT_MAILBOX/_BUS/_NONE)
SM_INT_ARG1        16   1st interrupt argument
SM_INT_ARG2        16   2nd interrupt argument
SM_INT_ARG3        16   3rd interrupt argument
SM_NUM_MBLKS       16   number of mBlks in driver memory pool (if < 16,
			a default value is used)
SM_NUM_CBLKS       16   number of clBlks in driver memory pool (if < 16,
			a default value is used)


DEBUG SUPPORT
This module has a define to enable compilation of debug instrumentation code
called SM_DBG.  There is also a conditionally compiled global called smEndDebug
which is used as a bit array for enabling/disabling debug reporting in various
routines in this module.  This enables selective debugging with little or no
extraneous information from other code sections.

SEE ALSO: muxLib, endLib
.I "Writing an Enhanced Network Driver"

INTERNAL
There are currently certain values stored in what are listed as reserved
locations within various sm structures.  These must be treated with extreme
care so as not to overwrite them:

Structure		Reserved field usage
--------------------	------------------------------------------------------
SM_PKT_MEM_HDR		reserved1 - master's sequential IP address
			reserved2 - sm heartbeat WD timer ID

SM_CPU_DESC		reserved1 - each CPU stores it's non-sequential IP
			address here.  There is one SM_CPU_DESC per CPU in sm.
*/

/* includes */

#include "vxWorks.h"
#include "stdlib.h"
#include "stdio.h"
#include "cacheLib.h"
#include "intLib.h"
#include "net/if_dl.h"
#include "end.h"			/* Common END structures. */
#include "endLib.h"
#include "lstLib.h"			/* Needed to maintain protocol list. */
#include "arpLib.h"
#include "wdLib.h"
#include "iv.h"
#include "semLib.h"
#include "taskLib.h"
#include "etherLib.h"
#include "logLib.h"
#include "netLib.h"
#include "sysLib.h"
#include "errno.h"
#include "errnoLib.h"
#include "memLib.h"
#include "muxLib.h"
#include "etherMultiLib.h"		/* multicast stuff. */
#include "m2Lib.h"

#include "netinet/if_ether.h"
#include "net/if_subr.h"
#include "net/mbuf.h"
#include "net/unixLib.h"
#include "net/protosw.h"
#include "net/systm.h"
#include "net/route.h"

#include "sys/socket.h"
#include "sys/ioctl.h"
#include "sys/times.h"

#include "drv/end/smEnd.h"
#include "smLib.h"
#include "smPktLib.h"
#include "smUtilLib.h"

IMPORT	int endMultiLstCnt (END_OBJ * pEnd);


/* defines */

/*
 * Definitions for the shared memory END State flags field
 *
 * Flag			Use
 * ---------------	------------------------------------------------------
 * S_LOADED		smEnd successfully loaded
 * S_CPU_ATTACHED	local CPU is attached to shared memory
 * S_RUNNING		local CPU active in shared memory
 * S_POLLED_SM		shared memory interface is operating in polled mode
 * S_POLLED_SM_LOCKED	sm interface locked in polled mode, can't enter
 *			interrupt mode
 * S_POLLED_END		MUX interface is operating in polled mode
 * S_RCV_TASK_ACTIVE	interrupt driven receive task is active
 */

#define S_LOADED		0x0001
#define S_CPU_ATTACHED		0x0002
#define S_RUNNING		0x0004
#define S_POLLED_SM		0x0008
#define S_POLLED_SM_LOCKED	0x0010
#define S_POLLED_END		0x0020
#define S_RCV_TASK_ACTIVE	0x0040

/* shared memory END compound states */

#define S_ACTIVE         (S_LOADED | S_CPU_ATTACHED | S_RUNNING)
#define S_POLLED_SM_RDY  (S_ACTIVE | S_POLLED_SM)
#define S_POLLED_END_RDY (S_ACTIVE | S_POLLED_END)
#define S_POLLED_RDY     (S_ACTIVE | S_POLLED_SM | S_POLLED_END)
#define S_INTR_RDY       (S_ACTIVE)

#define MODE_INTR_MASK   (S_ACTIVE | S_POLLED_RDY | S_RCV_TASK_ACTIVE)


/* macros */

/*
 * Default macro definitions for smUtilLib interface.
 * These macros must be redefined when architecture-neutral functional calls
 * exist for: intConnect(), intDisconnect(), intEnable(), intDisable(),
 * sysMailboxDisconnect(), and sysMailboxDisable().
 */

/*
 * XXXmas temporary; someday will have to make a real one based on a
 * functional intDisconnect()!
 */

#define smUtilIntDisconnect(pri,pIsr,isrArg,iType,iArg1,iArg2,iArg3)  OK

/* XXXmas ditto: smUtilLib will someday have real versions of these: */

/* Macro to enable the appropriate interrupt level */

#ifndef   smUtilIntEnable
#define smUtilIntEnable(pSmEndDev) \
    do \
        { \
        if (pSmEndDev->intType == SM_INT_BUS) \
            sysIntEnable (pSmEndDev->intArg1); \
        else if (pSmEndDev->intType != SM_INT_NONE) \
            sysMailboxEnable ((char *)pSmEndDev->intArg2); \
        else \
            taskResume (pollTaskId); \
        } while (0)
#endif /* smUtilIntEnable */

/* Macro to disable the appropriate interrupt level */

#ifndef   smUtilIntDisable
#define smUtilIntDisable(pSmEndDev) \
    do \
        { \
        if (pSmEndDev->intType == SM_INT_BUS) \
            sysIntDisable (pSmEndDev->intArg1); \
        else if (pSmEndDev->intType == SM_INT_NONE) \
            taskSuspend (pollTaskId); \
        } while (0)
#endif /* smUtilIntDisable */

/* A shortcut for getting the hardware address from the MIB II stuff. */

#define END_HW_ADDR(pEnd)	\
		((pEnd)->mib2Tbl.ifPhysAddress.phyAddress)

#define END_HW_ADDR_LEN(pEnd) \
		((pEnd)->mib2Tbl.ifPhysAddress.addrLength)

#define SM_END_PKT_LEN_GET(pPkt)  ((pPkt)->header.nBytes)

#define SM_END_IS_RUNNING  \
    ((END_FLAGS_GET (&pSmEndDev->end) & IFF_RUNNING) != 0)

#define SM_END_IS_LOADED  \
    ((pSmEndDev->flags & S_LOADED) != 0)

#define SM_END_UNLOAD  \
    (pSmEndDev->flags &= ~S_LOADED)

#define SM_END_IS_ACTIVE  \
    ((pSmEndDev->flags & S_ACTIVE) == S_ACTIVE)

#define SM_END_POLL_SM_RDY  \
    ((pSmEndDev->flags & S_POLLED_SM_RDY) == S_POLLED_SM_RDY)

#define MUX_IS_POLLING  \
    ((pSmEndDev->flags & S_POLLED_END_RDY) == S_POLLED_END_RDY)

#define SM_IS_POLLED_LOCKED  \
    ((pSmEndDev->flags & S_POLLED_SM_LOCKED) != 0)

#define SM_END_INTR_RDY  \
    ((pSmEndDev->flags & MODE_INTR_MASK) == S_INTR_RDY)

#define SM_END_STOP  \
    {pSmEndDev->flags &= ~S_RUNNING; \
     END_FLAGS_CLR (&pSmEndDev->end, IFF_RUNNING);}

#define SM_END_START  \
    {pSmEndDev->flags |= S_RUNNING; \
     END_FLAGS_SET (&pSmEndDev->end, IFF_RUNNING);}

#define SM_RCV_TASK_ACTIVE  \
    (pSmEndDev->flags |= S_RCV_TASK_ACTIVE)

#define SM_RCV_TASK_INACTIVE  \
    (pSmEndDev->flags &= ~S_RCV_TASK_ACTIVE)


/* DEBUG MACROS */

#define TUPLE   /* XXXmas */
#define SM_DBG  /* XXXmas */

#ifdef	SM_DBG
#  define SM_DBG_OFF		0x0000
#  define SM_DBG_RX		0x0001
#  define SM_DBG_TX		0x0002
#  define SM_DBG_INT		0x0004
#  define SM_DBG_POLL		(SM_DBG_POLL_RX | SM_DBG_POLL_TX)
#  define SM_DBG_POLL_RX	0x0008
#  define SM_DBG_POLL_TX	0x0010
#  define SM_DBG_LOAD		0x0020
#  define SM_DBG_MEM_INIT	0x0040
#  define SM_DBG_UNLOAD		0x0080
#  define SM_DBG_IOCTL		0x0100
#  define SM_DBG_START		0x0200
#  define SM_DBG_STOP		0x0400
#  define SM_DBG_CFG		0x0800
#  define SM_DBG_RSLV		0x1000

#  define SM_LOG(FLG, X0, X1, X2, X3, X4, X5, X6) \
	if (smEndDebug & FLG)                    \
            {logMsg(X0, X1, X2, X3, X4, X5, X6); taskDelay(3);}

#else /*SM_DBG*/

#  define SM_LOG(FLG, X0, X1, X2, X3, X4, X5, X6)

#endif /*SM_DBG*/


/* typedefs */

/* Allowed State Changes */

typedef enum {SC_NONE, SC_INIT, SC_POLL2INT, SC_INT2POLL} STATE_CHANGE;

/*
 * The definition of the shared memory device control structure
 *
 * typedef struct sm_end_device	/@ SM_END_DEV @/
 *  {
 *  END_OBJ		end;		/@ The class we inherit from. @/
 *  UINT		cookie;		/@ sm device validation cookie @/
 *  unsigned		localCpu;	/@ local cpu number @/
 *  int			unit;		/@ unit number @/
 *  ULONG		busSpace;	/@ bus address space for pAnchor/pmem@/
 *  SM_ANCHOR *		pAnchor;	/@ pointer to shared memory anchor @/
 *  char *		pMem;		/@ pointer to start of shared memory @/
 *  ULONG		memSize;	/@ total shared memory size in bytes @/
 *  unsigned		tasType;	/@ test-and-set type (HARD/SOFT) @/
 *  unsigned		maxCpus;	/@ max #CPUs supported in 'sm' @/
 *  unsigned		masterCpu;	/@ master CPU number @/
 *  ULONG		maxPktBytes;	/@ max #packets in shared memory @/
 *  char *		startAddr;	/@ start of seq addrs (0 = not seq) @/
 *  char *		ipAddr;		/@ non-seq. IP address @/
 *  int			maxPackets;	/@ max #packets CPU can receive @/
 *  int			intType;	/@ interrupt method @/
 *  int			intArg1;	/@ 1st interrupt argument @/
 *  int			intArg2;	/@ 2nd interrupt argument @/
 *  int			intArg3;	/@ 3rd interrupt argument @/
 *  int			ticksPerBeat;	/@ #CPU ticks per heartbeat @/
 *  ULONG		mbNum;		/@ number of mBlks to allocate @/
 *  ULONG		cbNum;		/@ number of clBlks to allocate @/
 *  BOOL		smAlloc;	/@ sm allocated? @/
 *  BOOL		isMaster;	/@ we are master CPU? @/
 *  ULONG		flags;		/@ Our local flags. @/
 *  SM_PKT_MEM_HDR *    pSmPktHdr;	/@ sm packet header @/
 *  CL_POOL_ID		clPoolId;	/@ cluster pool ID @/
 *  M_BLK_ID		tupleId;	/@ receive-ready tuple (mBlk) ID @/
 *  M_BLK_ID		pollQ;		/@ polled tuple queue @/
 *  M_BLK_ID		pollQLast;	/@ polled tuple queue last entry @/
 *  void *		pMclBlkCfg;	/@ mBlk/clBlk config memory @/
 *  void *		pClustMem;	/@ cluster pool memory @/
 *
 *  /@ old SM_SOFTC section @/
 *
 *  struct arpcom	arpcom;		/@ common ethernet structure@/
 *  SM_PKT_DESC		smPktDesc;	/@ shared mem packet desc @/
 *  u_long		masterAddr;	/@ master's IP address @/
 *  } SM_END_DEV;
 */


/* globals */

#ifdef	SM_DBG
int        smEndDebug  = NONE;	/* section debug enable switch */
int        smEndRxInts = 0;	/* number of receive interrupts received */
int        smEndTxInts = 0;	/* number of transmit interrupts generated */
#endif /* SM_DBG */


/* unit -> SM_END_DEV * Table */

SM_END_DEV ** unitTbl = NULL;


/* LOCALS */

/* forward static functions */

LOCAL STATUS smEndParse (SM_END_DEV * pSmEndDev, char * pParamStr);
LOCAL STATUS smEndMemInit (SM_END_DEV * pSmEndDev);
LOCAL STATUS smEndConfig (SM_END_DEV * pSmEndDev, STATE_CHANGE sDelta);
LOCAL void   smEndHwAddrSet (SM_END_DEV * pSmEndDev);
LOCAL void   smEndIsr (SM_END_DEV * pSmEndDev);

LOCAL void   smEndSrvcRcvInt (SM_END_DEV * pSmEndDev);
LOCAL STATUS smEndRecv (SM_END_DEV * pSmEndDev, SM_PKT * pPkt, M_BLK_ID pMblk);
LOCAL void   smEndPollQPut (SM_END_DEV * pSmEndDev, M_BLK_ID mBlkId);
LOCAL int    smEndPollQGet (SM_END_DEV * pSmEndDev, M_BLK_ID mBlkId);
LOCAL void   smEndPollQFree (SM_END_DEV * pSmEndDev);
LOCAL void   smEndPulse (SM_PKT_MEM_HDR * pSmPktHdr);
LOCAL M_BLK_ID smEndTupleGet (SM_END_DEV * pSmEndDev);
LOCAL STATUS smEndTupleChainWalk (M_BLK * pMblk, UINT * pFragNum,
				  UINT16 * pPktType, UINT * pMaxSize);
LOCAL int    smEndAddrResolve (FAST struct arpcom *   pAc,
                               FAST struct rtentry *  pRt,
                               struct mbuf *          pMbuf,
                               FAST struct sockaddr * pDestIP,
                               FAST u_char *          pDestHW);

/* END Specific interfaces */

/* This is the only externally visible interface. */

END_OBJ *    smEndLoad (char* initString);

LOCAL STATUS smEndUnload (void *);
LOCAL STATUS smEndStart  (void *);
LOCAL STATUS smEndStop   (void *);
LOCAL int    smEndIoctl  (void *, int, caddr_t);
LOCAL STATUS smEndSend   (void *, M_BLK_ID);
			  
LOCAL STATUS smEndMCastAddrAdd  (void *, char *);
LOCAL STATUS smEndMCastAddrDel  (void *, char *);
LOCAL STATUS smEndMCastAddrGet  (void *, MULTI_TABLE *);

LOCAL STATUS smEndPollSend  (void *, M_BLK_ID);
LOCAL STATUS smEndPollRecv  (void *, M_BLK_ID);

LOCAL STATUS smEndPollStart (void *);
LOCAL STATUS smEndPollStop  (void *);

#if FALSE	/* XXXmas future enhancement */
LOCAL void smEndAddrFilterSet (void *);
#endif


/*
 * Declare our function entry table.  This is static across all END driver
 * instances.
 */

LOCAL NET_FUNCS smEndFuncTable =
    {
    (FUNCPTR)smEndStart,	/* Function to start the device */
    (FUNCPTR)smEndStop,		/* Function to stop the device */
    (FUNCPTR)smEndUnload,	/* Unloading function of the driver */
    (FUNCPTR)smEndIoctl,	/* Ioctl function of the driver */
    (FUNCPTR)smEndSend,		/* Send function of the driver */
    (FUNCPTR)smEndMCastAddrAdd,	/* Multicast add function of the driver */
    (FUNCPTR)smEndMCastAddrDel,	/* Multicast delete function of the driver */
    (FUNCPTR)smEndMCastAddrGet,	/* Multicast retrieve function of the driver */
    (FUNCPTR)smEndPollSend,	/* Polling send function */
    (FUNCPTR)smEndPollRecv,	/* Polling receive function */
    endEtherAddressForm,	/* put address info into a NET_BUFFER */
    endEtherPacketDataGet, 	/* get pointer to data in NET_BUFFER */
    endEtherPacketAddrGet 	/* Get packet addresses */
    };

LOCAL int  pollTaskId = NONE;

/*
 * This array will only be needed until there are functional intConnect(),
 * intDisconnect(), intEnable() and intDisable() routines for all BSPs.
 */

LOCAL BOOL connected[SM_NUM_INT_TYPES] =
    {FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE};


/******************************************************************************
*
* smEndLoad - attach the sm interface to the MUX, initialize driver and device
*
* This routine attaches an 'sm' Ethernet interface to the network MUX.  This
* routine makes the interface available by allocating and filling in an END_OBJ
* structure, a driver entry table, and a MIB2 interface table.
*
* Calls to this routine evoke different results depending upon the parameter
* string it receives.  If the string is empty, the MUX is requesting that the
* device name be returned, not an intialized END_OBJ pointer.  If the string
* is not empty, a load operation is being requested with initialization being
* done with the parameters parsed from the string.
*
* Upon successful completion of a load operation by this routine, the driver
* will be ready to be started, not active.  The system will start the driver
* when it is ready to accept packets.
*
* The shared memory region will be initialized, via smPktSetup(), during the
* call to this routine if it is executing on the designated master CPU.
* The smEndLoad() routine can be called to load only one device unit at a time.
*
* Input parameters are specified in the form of an ASCII string of colon (:)
* delimited values of the form:
*
* "<unit>:<busSpace>:<pAnchor>:<pMem>:<memSize>:<tasType>:<maxCpus>:
*  <masterCpu>:<maxPktBytes>:<startAddr>:<ipAddr>:<maxInputPkts>:<intType>:
*  <intArg1>:<intArg2>:<intArg3>:<mbNum>:<cbNum>"
*
* The <unit> parameter denotes the logical device unit number assigned by the
* operating system.  Specified using radix 10.
*
* The <busSpace> parameter denotes the bus address space.  NONE = local, else,
* a VME AM or PCI space identifier is required.  This value applies to the
* <pAnchor> and <pMem> parameters.  Specified using radix 16.  [NOT SUPPORTED]
*
* The <pAnchor> parameter is the local address by which the local CPU may
* access the shared memory anchor.  Specified using radix 16.
*
* The <pMem> parameter contains either the local address of shared memory or
* the value NONE (-1), which implies that shared memory is to be allocated
* dynamically.  Specified using radix 16.
*
* The <memSize> parameter is the size, in bytes, of the shared memory region.
* Specified using radix 16.
*
* The <tasType> parameter specifies the test-and-set operation to be used to
* obtain exclusive access to the shared data structures.  It is preferable
* to use a genuine test-and-set instruction, if the hardware permits it.  In
* this case, <tasType> should be SM_TAS_HARD.  If any of the CPUs on the
* backplane network do not support the test-and-set instruction, <tasType>
* should be SM_TAS_SOFT.  Specified using radix 10.
*
* The <maxCpus> parameter specifies the maximum number of CPUs that may
* use the shared memory region.  Specified using radix 10.
*
* The <masterCpu> parameter indicates the shared memory master CPU number.
* Specified in radix 10.
*
* The <maxPktBytes> parameter specifies the size, in bytes, of the data
* buffer in shared memory packets.  This is the largest amount of data
* that may be sent in a single packet.  If this value is not an exact
* multiple of 4 bytes, it will be rounded up to the next multiple of 4.
* If zero, the default size specified in DEFAULT_PKT_SIZE is used.
* Specified using radix 10.
*
* The <startAddr> parameter is only applicable if sequential addressing is
* enabled.  If <startAddr> is non-zero, it specifies the starting address to
* use for sequential addressing on the backplane.  If <startAddr> is zero,
* sequential addressing is disabled and <ipAddr> must be non-zero.  Specified
* using radix 16.
*
* The <ipAddr> parameter is only applicable if sequential addressing is
* not enabled.  If <ipAddr> is non-zero, it specifies the IP address to
* use for this CPU on the backplane.  If <ipAddr> is zero, <startAddr> must
* be non-zero.  Specified using radix 16.
*
* The <maxInputPkts> parameter specifies the maximum number of incoming shared
* memory packets which may be queued to this CPU at one time.  If zero, the
* default value is used.  Specified using radix 10.
*
* The <intType> parameter allows a CPU to announce the method by which it is to
* be notified of input packets which have been queued to it.  Specified using
* radix 10.
*
* The <intArg1>, <intArg2>, and <intArg3> parameters are arguments chosen based
* on, and required by, the interrupt method specified.  They are used to
* generate an interrupt of type <intType>.  Specified using radix 16.
*
* If <mbNum> is non-zero, it specifies the number of mBlks to allocate in the
* driver memory pool.  If <mbNum> is less than 0x10, a default value is used.
* Specified using radix 16.
*
* If <cbNum> is non-zero, it specifies the number of clBlks and, therefore, the
* number of clusters, to allocate in the driver memory pool.  If <cbNum> is
* less than 0x10, a default value is used.  Specified using radix 16.
*
* The number of clBlks is also the number of clusters which will be allocated.
* The clusters allocated in the driver memory pool all have a size of
* SM_END_CLST_SIZ bytes.  This size is defined such that an ethernet address
* plus the largest MTU size can be contained and so that the size as allocated
* by the netPoolInit() routine is an integer multiple of sizeof(int).
*
* XXXmas NOTE: to be added later: multicasting and zero-copy
*
* RETURNS: return values are dependent upon the context implied by the
* parameter string length as shown below.
*
* Length	Return Value
* --------	---------------------------------------------------------------
* 0		OK and device name copied to input string pointer or ERROR if
*		NULL string pointer.
* non-0		END_OBJ * to initialized object or ERROR if bogus string or an
*		internal error occurs.
*
* SEE ALSO: smEndParse()
*/

END_OBJ * smEndLoad
    (
    char * pParamStr	/* pointer to initialization parameter string */
    )
    {
    SM_END_DEV *	pSmEndDev    = NULL;	/* device control pointer */
    SM_ANCHOR *		pAnchor;		/* addr of anchor */
    int                 anchorSet    = ERROR;	/* anchor visible */
    int                 tics = smUtilProcNumGet(); /* sm probe delay period */
    int			temp;			/* temp for vxMemProbe */
    static UCHAR	enetAddr[EADDR_LEN] = {0,0,0,0,0,0}; /* ethernet adrs*/
    static char		devName []   = SM_END_DEV_NAME;
    static int		smEndDevAttr = (IFF_UP | IFF_NOTRAILERS |
                                        IFF_BROADCAST);
#if FALSE	/* XXXmas future enhancement */
    static int		smEndDevAttr = (IFF_UP | IFF_NOTRAILERS |
                                        IFF_BROADCAST | IFF_MULTICAST);
#endif

SM_LOG (SM_DBG_LOAD, "smEndLoad: pParamStr = %#X, len = %d\n",
        (unsigned)pParamStr, strlen (pParamStr), 0, 0, 0, 0);
    /* If parameter string pointer is NULL, just return ERROR */

    if (pParamStr == NULL)
        {
        SM_LOG (SM_DBG_LOAD, "smEndLoad: ERROR: no param string ptr...\n",
                0, 0, 0, 0, 0, 0);
	return ((END_OBJ *)ERROR);
        }

    /*
     * If parameter string is empty, MUX is requesting device name,
     * not a load operation.  Just copy name to pointer and return OK.
     */

    if (*pParamStr == 0)
        {
        strcpy (pParamStr, devName);
        SM_LOG (SM_DBG_LOAD, "smEndLoad: 1st pass: Returning dev name <%s>\n",
                (unsigned)pParamStr, 0, 0, 0, 0, 0);
        return ((END_OBJ *)OK);
        }

    /*
     * load operation requested
     * allocate required memory for structures
     */

    /* allocate the SM_END_DEV structure */

    SM_LOG (SM_DBG_LOAD, "smEndLoad: 2nd pass\n", 0, 0, 0, 0, 0, 0);
    if ((pSmEndDev = (SM_END_DEV *)calloc (1, sizeof (SM_END_DEV))) == NULL)
        {
        SM_LOG (SM_DBG_LOAD, "smEndLoad: can't Aallocate SM_END_DEV\n",
                0, 0, 0, 0, 0, 0);
	return ((END_OBJ *)ERROR);
        }

    /* validate structure */

    pSmEndDev->cookie = SM_END_COOKIE;

    /*
     * Parse the initialization parameter string, extracting all parameters.
     * Parameter values are delimited by spaces.
     */

    SM_LOG (SM_DBG_LOAD, "Parsing string...\n", 0, 0, 0, 0, 0, 0);
    if (smEndParse (pSmEndDev, pParamStr) != ERROR)
        {
        /*
         * The <ticksPerBeat> parameter specifies the frequency of the shared
         * memory anchor's heartbeat.  The frequency is expressed in terms of
         * the number of CPU ticks on the local CPU corresponding to one
         * heartbeat period.
         */

        pSmEndDev->ticksPerBeat = sysClkRateGet ();

        /* if initial mode is polled, lock it */

        if (pSmEndDev->intType == SM_INT_NONE)
            pSmEndDev->flags = S_POLLED_SM_LOCKED;
        else
            pSmEndDev->flags = 0;

        /* If not already, allocate unit table and store our ptr in it */

        if (unitTbl != NULL)
            {
            if ((unitTbl = (SM_END_DEV **)calloc (NSM, sizeof (SM_END_DEV)))
                         == NULL)
                {
                smEndUnload (pSmEndDev);
                return ((END_OBJ *)ERROR);
                }
            }
        unitTbl [pSmEndDev->unit] = pSmEndDev;

        /* If this is the master CPU, determine addressing mode */

        if (pSmEndDev->masterCpu == smUtilProcNumGet ())
            {
            pSmEndDev->isMaster = TRUE;
            if (pSmEndDev->startAddr != NULL)
                {
                SM_LOG (SM_DBG_LOAD,
                        "MASTER: sequential addressing enabled at (0x%x)\n",
                        (unsigned)pSmEndDev->startAddr, 0, 0, 0, 0, 0);
                }
            else
                {
                SM_LOG (SM_DBG_LOAD,
                        "MASTER: sequential addressing disabled\n",
                        0, 0, 0, 0, 0, 0);
                }
            }

        /* Perform device memory allocation/distribution */

        if (smEndMemInit (pSmEndDev) == ERROR)
            {
            smEndUnload (pSmEndDev);
            return ((END_OBJ *)ERROR);
            }

        /* Initialize the shared memory descriptor.  */

        pAnchor = pSmEndDev->pAnchor;
        smPktInit (&pSmEndDev->smPktDesc, pSmEndDev->pAnchor,
                   pSmEndDev->maxPackets, pSmEndDev->ticksPerBeat,
                   pSmEndDev->intType,    pSmEndDev->intArg1,
                   pSmEndDev->intArg2,    pSmEndDev->intArg3);
        pSmEndDev->smPktDesc.hdrLocalAdrs = pSmEndDev->pSmPktHdr;

        /* initialize the END and MIB2 parts of the device structure */
    
        if ((END_OBJ_INIT (&pSmEndDev->end, (DEV_OBJ *)pSmEndDev,
                           SM_END_DEV_NAME, pSmEndDev->unit, &smEndFuncTable,
                           "Shared Memory END Driver") != ERROR) &&
            (END_MIB_INIT (&pSmEndDev->end, M2_ifType_other, enetAddr,
                           EADDR_LEN, SM_END_CLST_SIZ,
                           SM_END_MIB2_SPEED) != ERROR))
            {
            /* configure the device */

            smEndConfig (pSmEndDev, SC_INIT);

            /* XXX
             * The following code is a hack.  The anchor may not even be
             * mapped to the bus yet!  When probing with the next few lines
             * Bus Errors occur on many boards if the slave beats the master
             * to the pool.  The hack here simply avoids the sequential
             * addressing if the slave gets a BERR looking for the anchor.
             * Therefore, reliable use of sequential addressing can only be
             * guaranteed if the slave is stopped during the booting sequence
             * until the master has fully booted.  Towards this end an initial
             * delay in bus probing is introduced based on processor number.
             * If the first probe is unsuccessful, an exponential increase in
             * delay period is used to reduce bus contention on subsequent
             * probes.  This workaround is no worse than receiving BERRs
             * but does reduce bus contention and the number of BERRs.  Note
             * that the master procesor is assumed to be processor #0 (even
             * though it may not be).  Processor #0 has no effective delay
             * period on the first or subsequent bus probes!  This is so the
             * presumed master processor can proceed with minimal delay.
             */

            for (tics <<= 1; (tics < SM_MAX_WAIT) && (anchorSet == ERROR);
                 tics <<= 1)
                {
                smUtilDelay (NULL, tics);
                if ((anchorSet = smUtilMemProbe ((char *)pAnchor, READ, 4,
                                                 (char *)&temp))
                              != OK)
                    continue;
                }

            if (tics >= SM_MAX_WAIT)
                {
                SM_LOG (SM_DBG_LOAD, "smEndLoad: Error: sm probe timed out\n",
                        0, 0, 0, 0, 0, 0);
                smEndUnload (pSmEndDev);
                return ((END_OBJ *)ERROR);
                }

            SM_LOG (SM_DBG_LOAD, "smEndLoad: sm probe worked, tics = %u\n",
                    tics, 0, 0, 0, 0, 0);

            /*
             * IP -> HW Address Resolution
             * Whether or not sequential addressing is enabled, we calculate
             * the HW address.  ARP protocol does not apply.
             */

            muxAddrResFuncAdd (M2_ifType_other, 0x800, smEndAddrResolve);

            /* set the flags to indicate readiness */

            SM_RCV_TASK_INACTIVE;
            END_OBJ_READY (&pSmEndDev->end, smEndDevAttr);
            pSmEndDev->flags |= S_ACTIVE;
            SM_LOG (SM_DBG_LOAD, "Done loading smEnd - flags = %#lx\n",
                    pSmEndDev->flags, 0, 0, 0, 0, 0);

            return (&pSmEndDev->end);
            }
        }

    smEndUnload (pSmEndDev);
    return ((END_OBJ *)ERROR);
    }


/*******************************************************************************
*
* smEndUnload - unload sm device from the system
*
* This routine unloads the device pointed to by <pSmEndDev> from the system.
*
* RETURNS: OK or ERROR.
*
* SEE ALSO: smEndLoad()
*/

LOCAL STATUS smEndUnload
    (
    void * pObj		/* device control pointer */
    )
    {
    unsigned         i;
    unsigned         cnt;
    SM_END_DEV *     pSmEndDev = (SM_END_DEV *)pObj;
    SM_PKT_MEM_HDR * pSmPktHdr = NULL;	/* packet header */

    SM_LOG (SM_DBG_UNLOAD, "smEndUnload: start\n", 0, 0, 0, 0, 0, 0);

    if (pSmEndDev == NULL)
        {
        SM_LOG (SM_DBG_UNLOAD, "smEndUnload error: NULL device pointer\n",
                0, 0, 0, 0, 0, 0);
	return (ERROR);
        }

    /* been there, done that... */

    if (pSmEndDev->cookie != SM_END_COOKIE)
        {
        SM_LOG (SM_DBG_UNLOAD, "smEndUnload warning: dev already unloaded\n",
                0, 0, 0, 0, 0, 0);
	return (OK);
        }

#ifdef	SM_DBG
    if (!SM_END_IS_LOADED)
        {
        SM_LOG (SM_DBG_UNLOAD, "smEndUnload warning: device not loaded\n",
                0, 0, 0, 0, 0, 0);
        }
#endif /* SM_DBG */

    SM_END_UNLOAD;

    /* detach from shared mem */

    if (pSmEndDev->smPktDesc.status == SM_CPU_ATTACHED)
        {
        (void) smPktDetach (&pSmEndDev->smPktDesc, SM_FLUSH);
        }

    /* clear END status flags */

    END_FLAGS_CLR (&pSmEndDev->end, SM_END_ATTR_CLR);
    pSmEndDev->flags = 0;

    /* finished with END object */

    END_OBJECT_UNLOAD (&pSmEndDev->end);

    /* free allocated memory as necessary */

    if (pSmEndDev->tupleId != NULL)
        netMblkClChainFree (pSmEndDev->tupleId);

    if (pSmEndDev->end.pNetPool != NULL)
        netPoolDelete (pSmEndDev->end.pNetPool);

    if (pSmEndDev->pClustMem != NULL)
        cacheDmaFree (pSmEndDev->pClustMem);

    if (pSmEndDev->pMclBlkCfg != NULL)
        free (pSmEndDev->pMclBlkCfg);

    if (pSmEndDev->end.pNetPool != NULL)
        free (pSmEndDev->end.pNetPool);

    /*
     * If this is the master CPU, delete heartbeat watchdog timer
     * and free shared memory if it was allocated.
     */

    if (pSmEndDev->isMaster)
        {
        pSmPktHdr = SM_OFFSET_TO_LOCAL (ntohl(pSmEndDev->pAnchor->smPktHeader),
                                        (int)pSmEndDev->pAnchor,
                                        SM_PKT_MEM_HDR *);
        if (pSmPktHdr->reserved2 != 0)
            {
            wdDelete ((WDOG_ID) ntohl (pSmPktHdr->reserved2));
            }

        if (pSmEndDev->smAlloc)
            cacheDmaFree (pSmEndDev->pMem);
        }

    /* invalidate structure */

    pSmEndDev->cookie = 0;

    /* if this is the last entry in the unit table, free the table */

    for (i = 0, cnt = 0; i < NSM; ++i)
        {
        if (unitTbl[i] == pSmEndDev)
            unitTbl[i] = NULL;
        else
            ++cnt;
        }
    if (cnt == 0)
        free (unitTbl);

    /* free memory for device control structure */

    free (pSmEndDev);
    pSmEndDev = NULL;

    SM_LOG (SM_DBG_UNLOAD, "smEndUnload: Done\n", 0, 0, 0, 0, 0, 0);

    return (OK);
    }


/******************************************************************************
*
* smEndConfig - configure the sm interface to the device
*
* Configure the interface through controlled state changes.
*
* RETURNS: OK or ERROR.
*/

LOCAL STATUS smEndConfig
    (
    SM_END_DEV * pSmEndDev,	/* device to be configured */
    STATE_CHANGE sDelta		/* type of configuration change */
    )
    {
    STATUS      result          = OK;

    /* shutdown device completely if attached and reclaim packets */

    if (pSmEndDev->smPktDesc.status == SM_CPU_ATTACHED)
        {
        (void) smPktDetach (&pSmEndDev->smPktDesc, SM_FLUSH);
        pSmEndDev->smPktDesc.status = 0;
        pSmEndDev->flags &= ~S_CPU_ATTACHED;
        }

    /* Perform requested state change */

    switch (sDelta)
        {
        case SC_INT2POLL:
            if (pollTaskId == NONE)
                {
                if ((result = smUtilIntConnect (LOW_PRIORITY,(FUNCPTR)smEndIsr,
                                                (int)pSmEndDev, SM_INT_NONE,
                                                0, 0, 0))
                            == OK)
                    {
                    connected[SM_INT_NONE] = TRUE;
                    pollTaskId = taskNameToId (SM_POLL_TASK_NAME);
                    }
                }
            else
                {
                result = taskResume (pollTaskId);
                }
            break;

        case SC_POLL2INT:
            if (pollTaskId == NONE)
                {
                errno = EINVAL;
                result = ERROR;
                }
            else
                {
                taskSuspend (pollTaskId);
                if ((!connected[pSmEndDev->intType]) &&
                    (result = smUtilIntConnect (LOW_PRIORITY,
                                                (FUNCPTR)smEndIsr,
                                                (int)pSmEndDev,
                                                pSmEndDev->intType,
                                                pSmEndDev->intArg1,
                                                pSmEndDev->intArg2,
                                                pSmEndDev->intArg3))
                            == OK)
                    {
                    connected[pSmEndDev->intType] = TRUE;
                    }
                }
            break;

        case SC_INIT:    /* set hardware address */
            smEndHwAddrSet (pSmEndDev);
            break;

        case SC_NONE:
            break;

        default:
            SM_LOG (SM_DBG_CFG, "smEndConfig: unknown state change %x\n",
                    sDelta, 0, 0, 0, 0, 0);
            errno  = EINVAL;
            result = ERROR;
        }

#if FALSE	/* XXXmas future enhancement */
    /* set up address filter for multicasting */
	
    if (END_MULTI_LST_CNT(&pSmEndDev->end) > 0)
        {
        smEndAddrFilterSet (pSmEndDev);
        }
#endif

    /* attach to shared memory */

    if (smPktAttach (&pSmEndDev->smPktDesc) == ERROR)
	{
        SM_LOG (SM_DBG_CFG, "smEndConfig: ERROR attaching to sm: 0x%x\n",
                errno, 0, 0, 0, 0, 0);
        smEndUnload (pSmEndDev);	/* ??? is this necessary? */
    	return (ERROR);
	}

    pSmEndDev->flags |= S_CPU_ATTACHED;
    return (result);
    }


#if FALSE	/* XXXmas future enhancement */
/******************************************************************************
*
* smEndAddrFilterSet - set the address filter for multicast addresses
*
* This routine goes through all of the multicast addresses on the list
* of addresses (added with the endAddrAdd() routine) and sets the
* device's filter correctly.
*
* RETURNS: N/A.
*/

void smEndAddrFilterSet
    (
    VOID * pObj		/* device to be updated */
    )
    {
    SM_END_DEV *  pSmEndDev = (SM_END_DEV *)pObj; /* device to be updated */
    ETHER_MULTI * pCurr;

    pCurr = END_MULTI_LST_FIRST (&pSmEndDev->end);

    while (pCurr != NULL)
	{
        /* TODO - set up the multicast list */
        
	pCurr = END_MULTI_LST_NEXT(pCurr);
	}
    
    /* TODO - update the device filter list */
    }
#endif


/******************************************************************************
*
* smEndStart - start the device
*
* This function calls the system functions to connect interrupts and start the
* device running in interrupt mode.
*
* RETURNS: OK or ERROR if this cpu is configured for polled mode only.
*/

LOCAL STATUS smEndStart
    (
    VOID * pObj		/* device ID */
    )
    {
    SM_END_DEV * pSmEndDev = (SM_END_DEV *)pObj; /* device to be started */
    STATUS       result    = OK;

    /* free all queued tuples */

    smEndPollQFree (pSmEndDev);

    /*
     * If not already connected, connect sm interrupt.
     * ++ NOTE: This is necessary until a functional intDisconnect() or
     * smUtilIntDisconnect() routine becomes available.
     */

    if (!connected[pSmEndDev->intType])
        {
        if ((result = smUtilIntConnect (LOW_PRIORITY, (FUNCPTR)smEndIsr,
                                        (int)pSmEndDev, pSmEndDev->intType,
                                        pSmEndDev->intArg1, pSmEndDev->intArg2,
                                        pSmEndDev->intArg3))
                    == OK)
            {
            connected[pSmEndDev->intType] = TRUE;
            if ((pSmEndDev->intType == SM_INT_NONE) && (pollTaskId == NONE))
                {
                pollTaskId = taskNameToId (SM_POLL_TASK_NAME);
                }
            }
        }

    if (result == OK)
        {
        SM_LOG (SM_DBG_START, "smEndStart: interrupt connected\n",
                0, 0, 0, 0, 0, 0);

        /* enable interrupt */

        smUtilIntEnable (pSmEndDev);

        SM_LOG (SM_DBG_START, "smEndStart: interrupt enabled\n",
                0, 0, 0, 0, 0, 0);

        /* start the device: enable interrupts */

        SM_END_START;
        }
    else
        {
        SM_LOG (SM_DBG_START, "smEndStart: ERROR: can't connect interrupt!\n",
                0, 0, 0, 0, 0, 0);
        }

    return (result);
    }


/******************************************************************************
*
* smEndStop - stop the device
*
* This function calls the system functions to disconnect interrupts and stop
* the device from operating in interrupt mode.
*
* RETURNS: OK or ERROR.
*/

LOCAL STATUS smEndStop
    (
    VOID * pObj		/* device to be stopped */
    )
    {
    SM_END_DEV * pSmEndDev = (SM_END_DEV *)pObj; /* device to be stopped */
    STATUS       result;

    /* stop/disable the device. */

    SM_END_STOP;

    smUtilIntDisable (pSmEndDev);  /* XXXmas someday will be a real call */

    /*
     * XXXmas  someday this will have to be a call to a real functional
     * smUtilIntDisconnect() routine.
     */

    if ((result = smUtilIntDisconnect (LOW_PRIORITY, (FUNCPTR)smEndIsr,
                                       (int)pSmEndDev, pSmEndDev->intType,
                                       pSmEndDev->intArg1, pSmEndDev->intArg2,
                                       pSmEndDev->intArg3))
                == ERROR)
	{
	SM_LOG (SM_DBG_STOP, "smEndStop: Can't disconnect interrupt!\n",
		0, 0, 0, 0, 0, 0);
	}
    else
        {
        if (pSmEndDev->intType == SM_INT_BUS)
            connected[SM_INT_BUS] = FALSE;
        SM_LOG (SM_DBG_STOP, "smEndStop: interrupt disconnected\n",
                0, 0, 0, 0, 0, 0);
        }

    return (result);
    }


/******************************************************************************
*
* smEndPollStart - start polled mode operation
*
* This function starts full polled mode operation for both the shared memory
* and END interfaces.
*
* RETURNS: OK or ERROR.
*/

LOCAL STATUS smEndPollStart
    (
    void * pObj
    )
    {
    SM_END_DEV * pSmEndDev = (SM_END_DEV *)pObj; /* device receiving command */
    int          oldLevel;
    STATUS       result = ERROR;

    /* if sm is already in polled mode, just set END polled flag */

    if (SM_END_IS_ACTIVE)
        {
        if (!SM_END_POLL_SM_RDY)
            {
            oldLevel = intLock ();          /* disable ints during update */

            smUtilIntDisable (pSmEndDev);

            pSmEndDev->flags |= S_POLLED_SM;

            intUnlock (oldLevel);   /* now smEndInt won't get confused */

            smEndConfig (pSmEndDev, SC_INT2POLL); /* reconfigure device */
            }

        pSmEndDev->flags |= S_POLLED_END;
        result = OK;
        }
    else
        errno = ENODEV;

    return (result);
    }


/******************************************************************************
*
* smEndPollStop - stop polled mode operation
*
* This function stops polled mode operation for the MUX interface and, if not
* locked in polled mode, changes shared memory mode to interrupt.
*
* RETURNS: OK or ERROR.
*/

LOCAL STATUS smEndPollStop
    (
    void * pObj
    )
    {
    SM_END_DEV * pSmEndDev = (SM_END_DEV *)pObj; /* device receiving command */
    int          oldLevel;
    STATUS       result = ERROR;

    /* if sm is not in polled mode, just clear END polled flag */

    if (SM_END_IS_ACTIVE)
        {
        if (SM_END_POLL_SM_RDY && !SM_IS_POLLED_LOCKED)
            {
            oldLevel = intLock ();          /* disable ints during update */

            smUtilIntEnable (pSmEndDev);

            pSmEndDev->flags &= ~S_POLLED_SM;

            intUnlock (oldLevel);   /* now smEndInt won't get confused */

            smEndConfig (pSmEndDev, SC_POLL2INT); /* reconfigure device */
            }

        pSmEndDev->flags &= ~S_POLLED_END;
        result = OK;
        }
    else
        errno = ENODEV;

    return (result);
    }


/******************************************************************************
*
* smEndIoctl - sm device I/O control routine
*
* This routine is called to perform control operations on the sm device network
* interface.  The implemented commands are:
*
* Command	Data		  Function
* ------------	----------------  -----------------------------------------
* EIOCSADDR	char *		  set sm device address
* EIOCGADDR	char *		  get sm device address
* EIOCSFLAGS	int		  set sm device flags
* EIOCGFLAGS	int		  get sm device flags
* EIOCPOLLSTART	N/A		  start polled operation
* EIOCPOLLSTOP	N/A		  stop polled operation
* EIOCGMIB2	M2_INTERFACETBL * return MIB2 information
* EIOCGFBUF	int		  return minimum First Buffer for chaining
*
* RETURNS: A command specific response, usually OK or an error code:
* EINVAL (invalid argument) or ENOSUP (unsupported Ioctl command).
*/

LOCAL int smEndIoctl
    (
    VOID *  pObj,	/* device receiving command */
    int     cmd,	/* ioctl command code */
    caddr_t data	/* command argument */
    )
    {
    SM_END_DEV * pSmEndDev = (SM_END_DEV *)pObj; /* device receiving command */
    int          error     = OK;
    long         value;

    switch (cmd)
        {
        case EIOCSADDR:		/* set sm interface address */
	    if (data == NULL)
		return (EINVAL);
            bcopy ((char *)data, (char *)SM_END_ADDR_GET (&pSmEndDev->end),
		   SM_END_ADDR_LEN_GET (&pSmEndDev->end));
            break;

        case EIOCGADDR:		/* get sm interface address */
	    if (data == NULL)
		return (EINVAL);
            bcopy ((char *)SM_END_ADDR_GET (&pSmEndDev->end), (char *)data,
		    SM_END_ADDR_LEN_GET (&pSmEndDev->end));
            break;

        case EIOCSFLAGS:	/* set sm END device flags */
	    value = (long)data;
	    if (value < 0)
		{
		value = -(--value);
		END_FLAGS_CLR (&pSmEndDev->end, value);
		}
	    else
		{
		value &= ~IFF_MULTICAST;  /* XXXmas temporary */
		END_FLAGS_SET (&pSmEndDev->end, value);
		}
	    smEndConfig (pSmEndDev, SC_NONE);
            break;

        case EIOCGFLAGS:	/* get sm END device flags */
	    *(int *)data = END_FLAGS_GET(&pSmEndDev->end);
            break;

        case EIOCGMWIDTH:	/* get sm END memory width (#bytes) */
	    *(int *)data = 0;	/* no restrictions on size */
            break;

        case EIOCMULTIGET:	/* get multicast list (XXXmas future) */
	    return (ENOTSUP);
            break;

	case EIOCPOLLSTART:	/* Begin polled operation */
	    smEndPollStart (pSmEndDev);
	    break;

	case EIOCPOLLSTOP:	/* End polled operation */
	    smEndPollStop (pSmEndDev);
	    break;

        case EIOCGMIB2:		/* return MIB information */
            if (data == NULL)
                return (EINVAL);
            bcopy((char *)&pSmEndDev->end.mib2Tbl, (char *)data,
                  sizeof(pSmEndDev->end.mib2Tbl));
            break;

        case EIOCGFBUF:		/* return minimum First Buffer for chaining */
            if (data == NULL)
                return (EINVAL);
            *(int *)data = SM_END_MIN_FBUF;
            break;

        default:
            SM_LOG (SM_DBG_IOCTL, "smEndIoctl: unsupported command: 0x%x\n",
                    cmd, 0, 0, 0, 0, 0);
            error = ENOTSUP;
        }

    return (error);
    }


/******************************************************************************
*
* smEndHwAddrSet - set hardware address
*
* This routine sets the hardware address associated with the 'sm' device
* specified by <pSmEndDev>.  It also stores the non-sequential IP address in
* the associated CPU descriptor for fast address resolution.
*
* The 'sm' device H/W address consists of six bytes.  Depending upon whether
* sequential addressing is selected or not, the last three bytes assume
* different values:
*
* Sequential addressing: { 0x00, 0x02, 0xE2, ip[2], ip[3], ip[4] }
* where ip is the lower three bytes of the IP interface address.
*
* Not sequential addressing: { 0x00, 0x02, 0xE2, 0x00, unit, cpu }
*
* RETURNS: N/A
*/

LOCAL void smEndHwAddrSet
    (
    SM_END_DEV * pSmEndDev 	/* device for which to set address */
    )
    {
    u_char * 		pEaddr;			/* ethernet address */
    u_long		addr;			/* address */
    SM_PKT_MEM_HDR *	pSmPktHdr;		/* packet header */
    FAST SM_CPU_DESC *	pCpuDesc;		/* sm CPU descriptor */
    FAST SM_ANCHOR *	pAnchor = pSmEndDev->pAnchor;
    FAST SM_HDR *	pSmHdr;
    FAST SM_DESC *	pSmDesc = &pSmEndDev->smPktDesc.smDesc;

    pEaddr = etherAddrPtr (pSmEndDev->arpcom.ac_enaddr);

    bzero ((caddr_t)pEaddr, 6);		/* fill in ethernet address */
    pEaddr [0] = 0x00;
    pEaddr [1] = 0x02;
    pEaddr [2] = 0xE2;

    pSmPktHdr = pSmEndDev->smPktDesc.hdrLocalAdrs;
    pSmEndDev->localCpu = pSmDesc->cpuNum;
    SM_LOG (SM_DBG_CFG, "smEndHwAddrSet: rsvrd1 = %#x\n",
            ntohl (pSmPktHdr->reserved1), 0, 0, 0, 0, 0);

    if (pSmPktHdr->reserved1 != 0)		/* sequential addressing */
	{
	/*
	 * The backplane hw address consists of
	 * { 0x00, 0x02, 0xE2, ip[1], ip[2], ip[3] }.
	 * where ip is the lower three bytes of
	 * the IP interface address.
	 */

	pSmEndDev->masterAddr = ntohl (pSmPktHdr->reserved1) & 0x00FFFFFF;
	addr = pSmEndDev->masterAddr + pSmEndDev->localCpu;

	pEaddr [3] = ((addr >> 16) & 0xff);
	pEaddr [4] = ((addr >> 8)  & 0xff);
	pEaddr [5] = (addr & 0xff);
	}
    else
	{
    	/*
    	 * Backplane hw address consists of
     	 * { 0x00, 0x02, 0xE2, 0x00, unit, cpu }.
     	 */

	pEaddr [3] = 0;
	pEaddr [4] = pSmEndDev->unit;
	pEaddr [5] = pSmEndDev->localCpu;

	/*
	 * store the non-sequential IP address for this CPU in the CPU
	 * descriptor for the local CPU in shared memory for all to see
	 */

        pSmHdr = SM_OFFSET_TO_LOCAL (ntohl (pAnchor->smHeader), pSmDesc->base,
                                     SM_HDR *);
        pCpuDesc = SM_OFFSET_TO_LOCAL (ntohl (pSmHdr->cpuTable), pSmDesc->base,
                                       SM_CPU_DESC *);
        pCpuDesc = &(pCpuDesc[pSmDesc->cpuNum]);
        pCpuDesc->reserved1 = (unsigned)pSmEndDev->ipAddr;
	}

    SM_LOG (SM_DBG_LOAD, "smEndHwAddrSet: H/W address = %s\n",
            (int)ether_sprintf (pEaddr), 0, 0, 0, 0, 0);
    }


/******************************************************************************
*
* smEndParse - parse input paramter string and derive parameter values
*
* This routine parses the input parameter string, deriving the values of each
* parameter in the assumed order and radix as shown here:
*
* "<unit>:<busSpace>:<pAnchor>:<pMem>:<memSize>:<tasType>:<maxCpus>:
*  <masterCpu>:<maxPktBytes>:<startAddr>:<ipAddr>:<maxInputPkts>:<intType>:
*  <intArg1>:<intArg2>:<intArg3>:<mbNum>:<cbNum>"
*
* Parameter	Radix	Use
* -------------	-----	-------------------------------------------------------
* unitNumber	 10	device unit number assigned by operating system
* busSpace	 16	bus address space; NONE = local, else, VME AM/PCI space
*			[busSpace is not yet supported in smPktLib.c]
* pAnchor	 16	shared memory anchor region @<busSpace>
* pMem		 16	start of shared memory (NONE = allocate) @<busSpace>
* memSize	 16	total shared memory size in bytes
* tasType	 10	test-and-set type (SM_TAS_HARD or SM_TAS_SOFT)
* maxCpus	 10	maximum number of CPUs supported on backplane
*			(0 = default)
* masterCpu	 10	master CPU#
* maxPktBytes	 10	maximum number of bytes per shared memory packet
*			(0 = default)
* startAddr	 16	start of sequential IP addressing (0 = not sequential)
* ipAddr	 16	non-sequential IP address (0 = sequential)
* maxInputPkts	 10	maximum number of queued receive packets for this CPU
*			(0 = default)
* intType	 10	interrupt method (SM_INT_MAILBOX/BUS/NONE)
* intArg1	 16	1st interrupt argument
* intArg2	 16	2nd interrupt argument
* intArg3	 16	3rd interrupt argument
* mbNum		 16	number of mBlks in driver memory pool (if < 16, a
*			default value is used)
* cbNum		 16	number of clBlks in driver memory pool (if < 16, a
*			default value is used)
*
* The parameter values are delimited by colons and the EOS.
*
* RETURNS: OK or ERROR if bogus string.
*/

LOCAL STATUS smEndParse
    (
    SM_END_DEV * pSmEndDev,	/* pointer to device descriptor */
    char *       pParamStr	/* pointer to parameter string to parse */
    )
    {
    char * tok    = NULL;	/* parameter value (token) pointer */
    char * pLast  = NULL;	/* string position of last parse */

    /* derive unit number */

    if (((tok = strtok_r (pParamStr, ":", &pLast)) == NULL) ||
        ((pSmEndDev->unit = (unsigned) strtoul (tok, NULL, 10)) >= NSM))
        {
        SM_LOG (SM_DBG_LOAD, "missing or illegal unit number: %d\n",
                pSmEndDev->unit, 0, 0, 0, 0, 0);
        return (ERROR);
        }

    /*
     * derive bus address space for shared memory anchor and shared memory
     * addresses; NONE = local bus
     */

    if (((tok = strtok_r (NULL, ":", &pLast)) == NULL) ||
        ((pSmEndDev->busSpace = (ULONG) strtoul (tok, NULL, 16)) == NULL))
        {
        SM_LOG (SM_DBG_LOAD, "Missing or NULL SM bus space\n",
                0, 0, 0, 0, 0, 0);
        return (ERROR);
        }

    /* derive shared memory anchor address; must not be NULL */

    if (((tok = strtok_r (NULL, ":", &pLast)) == NULL) ||
        ((pSmEndDev->pAnchor = (SM_ANCHOR *) strtoul (tok, NULL, 16)) == NULL))
        {
        SM_LOG (SM_DBG_LOAD, "Missing or NULL SM anchor address\n",
                0, 0, 0, 0, 0, 0);
        return (ERROR);
        }

    /* derive pointer to shared memory; NONE (-1) = allocate dynamically */

    if (((tok = strtok_r (NULL, ":", &pLast)) == NULL) ||
        ((pSmEndDev->pMem = (char *) strtoul (tok, NULL, 16)) == NULL))
        {
        SM_LOG (SM_DBG_LOAD, "Missing or NULL SM address\n",
                0, 0, 0, 0, 0, 0);
        return (ERROR);
        }

    /* derive shared memory size in bytes */

    if (((tok = strtok_r (NULL, ":", &pLast)) == NULL) ||
        ((pSmEndDev->memSize = (ULONG) strtoul (tok, NULL, 16)) == 0))
        {
        SM_LOG (SM_DBG_LOAD, "Missing or zero SM size\n",
                0, 0, 0, 0, 0, 0);
        return (ERROR);
        }

    /* derive test-and-set type */

    if (((tok = strtok_r (NULL, ":", &pLast)) == NULL) ||
        ((pSmEndDev->tasType = (unsigned) strtoul (tok, NULL, 10)) >
         SM_TAS_HARD))
        {
        SM_LOG (SM_DBG_LOAD, "Missing or illegal test-and-set type: %d\n",
                pSmEndDev->tasType, 0, 0, 0, 0, 0);
        return (ERROR);
        }

    /* derive maximum number of CPUs supported in shared memory */

    if ((tok = strtok_r (NULL, ":", &pLast)) == NULL)
        {
        SM_LOG (SM_DBG_LOAD, "Missing number of CPUs\n",
                0, 0, 0, 0, 0, 0);
        return (ERROR);
        }
    if ((pSmEndDev->maxCpus = (unsigned) strtoul (tok, NULL, 10)) == 0)
        pSmEndDev->maxCpus = DEFAULT_CPUS_MAX;

    /* derive master CPU number */

    if ((tok = strtok_r (NULL, ":", &pLast)) == NULL)
        {
        SM_LOG (SM_DBG_LOAD, "Missing master CPU number\n",
                0, 0, 0, 0, 0, 0);
        return (ERROR);
        }
    if ((pSmEndDev->masterCpu = (unsigned) strtoul (tok, NULL, 10))
                              >= pSmEndDev->maxCpus)
        {
        SM_LOG (SM_DBG_LOAD, "Master CPU number: %d >= #CPUs (%d)\n",
                pSmEndDev->masterCpu, pSmEndDev->maxCpus, 0, 0, 0, 0);
        return (ERROR);
        }

    /* derive maximum shared memory packet size */

    if ((tok = strtok_r (NULL, ":", &pLast)) == NULL)
        {
        SM_LOG (SM_DBG_LOAD, "Missing max #bytes per packet\n",
                0, 0, 0, 0, 0, 0);
        return (ERROR);
        }
    if ((pSmEndDev->maxPktBytes = (ULONG) strtoul (tok, NULL, 10)) == 0)
        pSmEndDev->maxPktBytes = DEFAULT_PKT_SIZE;

    /* derive shared memory sequential start IP address (0 = not sequential) */

    if (((tok = strtok_r (NULL, ":", &pLast)) == NULL))
        {
        SM_LOG (SM_DBG_LOAD, "Missing SM sequential starting IP address\n",
                0, 0, 0, 0, 0, 0);
        return (ERROR);
        }
    pSmEndDev->startAddr = (char *) strtoul (tok, NULL, 16);

    /* derive shared memory non-sequential IP address (0 = sequential) */

    if (((tok = strtok_r (NULL, ":", &pLast)) == NULL))
        {
        SM_LOG (SM_DBG_LOAD, "Missing SM non-sequential IP address\n",
                0, 0, 0, 0, 0, 0);
        return (ERROR);
        }
    pSmEndDev->ipAddr = (char *) strtoul (tok, NULL, 16);

    /* derive maximum #packets that can be received; must be > 0 */

    if (((tok = strtok_r (NULL, ":", &pLast)) == NULL) ||
        ((pSmEndDev->maxPackets = atoi (tok)) < 0))
        {
        SM_LOG (SM_DBG_LOAD, "Missing or illegal number of packets: %d\n",
                pSmEndDev->maxPackets, 0, 0, 0, 0, 0);
        return (ERROR);
        }
    if (pSmEndDev->maxPackets == 0)
        pSmEndDev->maxPackets = DEFAULT_PKTS_MAX;

    /* derive interrupt method */

    if (((tok = strtok_r (NULL, ":", &pLast)) == NULL) ||
        ((pSmEndDev->intType = (unsigned) strtoul (tok, NULL, 10)) >
         SM_INT_MAILBOX_R4))
        {
        SM_LOG (SM_DBG_LOAD, "Missing or illegal SM interrupt type: %d\n",
                pSmEndDev->intType, 0, 0, 0, 0, 0);
        return (ERROR);
        }

    /* derive interrupt argument 1 */

    if ((tok = strtok_r (NULL, ":", &pLast)) == NULL)
        {
        SM_LOG (SM_DBG_LOAD, "Missing SM interrupt 1st argument\n",
                0, 0, 0, 0, 0, 0);
        return (ERROR);
        }
    pSmEndDev->intArg1 = (int) strtoul (tok, NULL, 16);

    /* derive interrupt argument 2 */

    if ((tok = strtok_r (NULL, ":", &pLast)) == NULL)
        {
        SM_LOG (SM_DBG_LOAD, "Missing SM interrupt 2nd argument\n",
                0, 0, 0, 0, 0, 0);
        return (ERROR);
        }
    pSmEndDev->intArg2 = (int) strtoul (tok, NULL, 16);

    /* derive interrupt argument 3 */

    if ((tok = strtok_r (NULL, ":", &pLast)) == NULL)
        {
        SM_LOG (SM_DBG_LOAD, "Missing SM interrupt 3rd argument\n",
                0, 0, 0, 0, 0, 0);
        return (ERROR);
        }
    pSmEndDev->intArg3 = (int) strtoul (tok, NULL, 16);

    /* derive number of mBlks to allocate */

    if ((tok = strtok_r (NULL, ":", &pLast)) == NULL)
        {
        SM_LOG (SM_DBG_LOAD, "Missing number of mBlks\n",
                0, 0, 0, 0, 0, 0);
        return (ERROR);
        }
    pSmEndDev->mbNum = (uint32_t) strtoul (tok, NULL, 16);
    if (pSmEndDev->mbNum < SM_MIN_MBLK_NUM)
        pSmEndDev->mbNum = SM_DFLT_MBLK_NUM;

    /* derive number of clBlks to allocate */

    if ((tok = strtok_r (NULL, ":", &pLast)) == NULL)
        {
        SM_LOG (SM_DBG_LOAD, "Missing number of cBlks\n",
                0, 0, 0, 0, 0, 0);
        return (ERROR);
        }
    pSmEndDev->cbNum = (uint32_t) strtoul (tok, NULL, 16);
    if (pSmEndDev->cbNum < SM_MIN_CLBLK_NUM)
        pSmEndDev->cbNum = SM_DFLT_CLBLK_NUM;

    return (OK);
    }


/******************************************************************************
*
* smEndMemInit - set up and initialize memory for an sm device
*
* This routine sets up and initializes a net pool for the specified sm device.
*
* RETURNS: OK or ERROR if failure.
*/

LOCAL STATUS smEndMemInit
    (
    SM_END_DEV * pSmEndDev
    )
    {
    SM_PKT_MEM_HDR * pSmPktHdr   = NULL;	/* packet header */
    WDOG_ID          smEndBeatWd = NULL;	/* heartbeat watchdog ID */
    UINT             smPktSize   = 0;
    M_CL_CONFIG      smEndMclBlkConfig = /* network mBlk/clBlk config table */
        {
        /* 
        no. mBlks		no. clBlks	memArea		memSize
        -----------		----------	-------		-------
        */
        0, 			0, 		NULL, 		0
        };
    CL_DESC          smEndClDescTbl [] = /* network cluster pool config table*/
        {
        /* 
        clSize			num		memArea		memSize
        -----------		----		-------		-------
        */
        {0,			0,		NULL,		0}
        }; 
    int              smEndClDescTblNumEnt = (NELEMENTS(smEndClDescTbl));

    /* If this is the master CPU, setup and initialize shared memory */

    if (pSmEndDev->isMaster)
        {
        /* if not already obtained, allocate the shared memory */

        if (pSmEndDev->pMem == (char *)NONE)
            {
            if (!CACHE_DMA_IS_WRITE_COHERENT () ||
                !CACHE_DMA_IS_READ_COHERENT  ()
               )
                {
                SM_LOG (SM_DBG_MEM_INIT,
                        "MASTER: cache coherent buffer not available\n",
                        0, 0, 0, 0, 0, 0);
                smEndUnload (pSmEndDev);
                return (ERROR);
                }

            if ((pSmEndDev->pMem =
                (char *)cacheDmaMalloc (pSmEndDev->memSize))
                == NULL)
                {
                SM_LOG (SM_DBG_MEM_INIT,
                        "MASTER: cache coherent 'sm' allocation error\n",
                        0, 0, 0, 0, 0, 0);
                smEndUnload (pSmEndDev);
                return (ERROR);
                }

            pSmEndDev->smAlloc = TRUE;
            }

        /* else, if shared memory is contiguous with anchor, update info */

        else if (pSmEndDev->pMem == (char *) pSmEndDev->pAnchor)
            {
            pSmEndDev->pMem    += sizeof (SM_ANCHOR);
            pSmEndDev->memSize -= sizeof (SM_ANCHOR);
            }

        SM_LOG (SM_DBG_MEM_INIT,
                "MASTER: anchor = 0x%X, memory @ 0x%X, size = %ld\n",
                (unsigned)pSmEndDev->pAnchor, (unsigned)pSmEndDev->pMem,
                pSmEndDev->memSize, 0, 0, 0);

        /* set up shared memory region */

        SM_LOG (SM_DBG_MEM_INIT, "MASTER: shared memory setup\n",
                0, 0, 0, 0, 0, 0);

        smPktSize = pSmEndDev->maxPktBytes +
                    SM_PACKET_ROUNDUP(pSmEndDev->maxPktBytes);

        SM_LOG (SM_DBG_MEM_INIT, "MASTER: sm packet size = 0x%08.8X\n",
                smPktSize, 0, 0, 0, 0, 0);
        if ((smPktSetup (pSmEndDev->pAnchor, pSmEndDev->pMem,
                         pSmEndDev->memSize, pSmEndDev->tasType,
                         pSmEndDev->maxCpus, smPktSize) == OK) &&
            ((smEndBeatWd = wdCreate ()) != NULL))
            {
            pSmPktHdr = SM_OFFSET_TO_LOCAL (ntohl (pSmEndDev->pAnchor->
                                                   smPktHeader),
                                                   (int)pSmEndDev->pAnchor,
                                                   SM_PKT_MEM_HDR *);
            pSmPktHdr->reserved1  = htonl ((unsigned)pSmEndDev->startAddr);
            pSmPktHdr->reserved2  = htonl ((unsigned)smEndBeatWd);
            pSmEndDev->pSmPktHdr  = pSmPktHdr;
            pSmEndDev->maxPackets = ntohl (pSmPktHdr->freeList.limit);
            SM_LOG (SM_DBG_MEM_INIT,
                    "MASTER: pSmPktHdr = %#x; rsvd1 = %#x; rsvd2 = %#x\n",
                    (unsigned)pSmPktHdr, ntohl (pSmPktHdr->reserved1),
                    ntohl (pSmPktHdr->reserved2), 0, 0, 0);

            /* start heartbeat */

            SM_LOG (SM_DBG_MEM_INIT, "MASTER: starting heartbeat\n",
                    0, 0, 0, 0, 0, 0);
            smEndPulse (pSmPktHdr);
            }
        else
            {
            smEndUnload (pSmEndDev);
            return (ERROR);
            }
        }

    /* allocate and set up END netPool using netBufLib() */
    
    if ((pSmEndDev->end.pNetPool = (NET_POOL_ID)calloc (1, sizeof(NET_POOL)))
                                 == NULL)
        {
        SM_LOG (SM_DBG_MEM_INIT, "smEndMemInit: net pool allocation error\n",
                0, 0, 0, 0, 0, 0);
        smEndUnload (pSmEndDev);
        return (ERROR);
        }

    smEndMclBlkConfig.mBlkNum  = pSmEndDev->mbNum;
    smEndMclBlkConfig.clBlkNum = pSmEndDev->cbNum;
    smEndClDescTbl[0].clNum    = pSmEndDev->cbNum;

    /* Calculate the total memory for all the M-Blks and CL-Blks. */

    smEndMclBlkConfig.memSize = (smEndMclBlkConfig.mBlkNum *
                                 (M_BLK_SZ + M_BLK_OVERHEAD)) +
                                 (smEndMclBlkConfig.clBlkNum * 
                                 (CL_BLK_SZ + CL_BLK_OVERHEAD));

    /* allocate aligned memory for m/clBlks */

    pSmEndDev->pMclBlkCfg = memalign (SM_ALIGN_BOUNDARY,
                                      smEndMclBlkConfig.memSize);
    
    SM_LOG (SM_DBG_MEM_INIT, "smEndMemInit: mBlk/clBlk mem size = 0x%08.8X=\n",
            (UINT)smEndMclBlkConfig.memSize, 0, 0, 0, 0, 0);
    SM_LOG (SM_DBG_MEM_INIT, "  0x%08.8X mBlks, each 0x%08.8X bytes\n",
            (UINT)smEndMclBlkConfig.mBlkNum, (M_BLK_SZ + M_BLK_OVERHEAD),
            0, 0, 0, 0);
    SM_LOG (SM_DBG_MEM_INIT, "  0x%08.8X clBlks, each 0x%08.8X bytes\n",
            (UINT)smEndMclBlkConfig.clBlkNum, (CL_BLK_SZ + CL_BLK_OVERHEAD),
            0, 0, 0, 0);
    SM_LOG (SM_DBG_MEM_INIT, "smEndMemInit: mBlk/clBlk mem area = 0x%08.8X\n",
            (UINT)pSmEndDev->pMclBlkCfg, 0, 0, 0, 0, 0);

    if ((smEndMclBlkConfig.memArea = (char *)pSmEndDev->pMclBlkCfg) == NULL)
        {
        SM_LOG (SM_DBG_MEM_INIT, "smEndMemInit: mBlk/clBlk allocation error\n",
                0, 0, 0, 0, 0, 0);
        smEndUnload (pSmEndDev);
        return (ERROR);
        }
    
    /* Calculate the memory size of all the clusters. */

    smEndClDescTbl[0].clSize  = SM_END_CLST_SIZ;
    smEndClDescTbl[0].memSize = smEndClDescTbl[0].clNum *
                                (SM_END_CLST_SIZ + CL_OVERHEAD);

    /* allocate the memory for the clusters from cache safe memory. */

    pSmEndDev->pClustMem = cacheDmaMalloc (smEndClDescTbl[0].memSize);

    SM_LOG (SM_DBG_MEM_INIT, "smEndMemInit: cluster mem size = 0x%08.8X =\n",
            (UINT)smEndClDescTbl[0].memSize, 0, 0, 0, 0, 0);
    SM_LOG (SM_DBG_MEM_INIT, "  0x%08.8X clusters, each 0x%08.8X bytes\n",
            (UINT)smEndClDescTbl[0].clNum, (UINT)SM_END_CLST_SIZ, 0, 0, 0, 0);
    SM_LOG (SM_DBG_MEM_INIT, "smEndMemInit: cluster mem area = 0x%08.8X\n",
            (UINT)pSmEndDev->pClustMem, 0, 0, 0, 0, 0);

    if ((smEndClDescTbl[0].memArea = (char *)pSmEndDev->pClustMem) == NULL)
        {
        SM_LOG (SM_DBG_MEM_INIT, "smEndMemInit: insufficient system memory\n",
                0, 0, 0, 0, 0, 0);
        smEndUnload (pSmEndDev);
        return (ERROR);
        }

    /* Initialize the memory pool. */

    if (netPoolInit (pSmEndDev->end.pNetPool, &smEndMclBlkConfig,
                     &smEndClDescTbl[0], smEndClDescTblNumEnt, NULL) == ERROR)
        {
        SM_LOG (SM_DBG_MEM_INIT, "smEndMemInit: Can't init net pool: 0x%x\n",
                errno, 0, 0, 0, 0, 0);
        smEndUnload (pSmEndDev);
        return (ERROR);
        }

    /* Get our pool ID for future reference */

    if ((pSmEndDev->clPoolId = netClPoolIdGet (pSmEndDev->end.pNetPool,
                                               SM_END_CLST_SIZ, FALSE)) ==NULL)
        {
        SM_LOG (SM_DBG_MEM_INIT, "smEndMemInit: Can't get cluster pool ID\n",
                0, 0, 0, 0, 0, 0);
        smEndUnload (pSmEndDev);
        return (ERROR);
        }

    /*
     * Prime the tuple pump
     * Try to always have one pre-allocated tuple available for fast receiving.
     */

    if ((pSmEndDev->tupleId = netTupleGet (pSmEndDev->end.pNetPool,
                                          SM_END_CLST_SIZ, M_DONTWAIT,
                                          MT_DATA, FALSE))
                           == NULL)
        {
        SM_LOG (SM_DBG_MEM_INIT, "smEndMemInit: Tuple get failed: 0x%x\n",
                errno, 0, 0, 0, 0, 0);
        smEndUnload (pSmEndDev);
        return (ERROR);
        }

    SM_LOG (SM_DBG_MEM_INIT, "Memory setup complete\n", 0, 0, 0, 0, 0, 0);

    return (OK);
    }


/******************************************************************************
*
* smEndIsr - shared memory interrupt service routine
*
* This routine is the interrupt service routine for shared memory input
* packets.  It processes the interrupt (if appropriate) then notifies the
* MUX of the incomming packet by calling the MUX receive routine.  It is
* enabled/disabled by setting/clearing the S_RUNNING flag bit.
*
* RETURNS: N/A
*/

LOCAL void smEndIsr
    (
    SM_END_DEV * pSmEndDev
    )
    {
    if (pSmEndDev != NULL)
        {
        if (SM_END_INTR_RDY)
            {
            SM_RCV_TASK_ACTIVE;
            netJobAdd ((FUNCPTR)smEndSrvcRcvInt, (int)pSmEndDev, 0, 0, 0, 0);
            }

        /* if bus interrupt, acknowledge it */

        if (pSmEndDev->intType == SM_INT_BUS)
            sysBusIntAck (pSmEndDev->intArg1);
        }

#ifdef	SM_DBG
    if ((smEndDebug & SM_DBG_INT) != 0)
        ++smEndRxInts;
#endif
    }


/******************************************************************************
*
* smEndPulse - continually pulse the shared memory heartbeat
*
* This routine maintains the shared memory heart beat by incrementing 
* the heartbeat count and then re-scheduling itself to run again 
* after a "beat" has passed.
*
* RETURNS: N/A
*/

LOCAL void smEndPulse
    (
    SM_PKT_MEM_HDR * pSmPktHdr	/* pointer to heartbeat */
    )
    {
    smPktBeat (pSmPktHdr);
    (void) wdStart ((WDOG_ID)ntohl (pSmPktHdr->reserved2),
                   (int)sysClkRateGet (), (FUNCPTR)smEndPulse, (int)pSmPktHdr);
    }


/******************************************************************************
*
* smEndTupleGet - get a netBuf tuple
*
* This routine gets a netBuf tuple and returns its ID or NULL if error.
*
* RETURNS: ID of tuple or NULL if error.
*/

LOCAL M_BLK_ID smEndTupleGet
    (
    SM_END_DEV * pSmEndDev
    )
    {
    M_BLK_ID  pMblk = NULL;
#ifndef TUPLE
    char *    pNewCluster;
    CL_BLK_ID pClBlk;
#endif /* TUPLE */

    /*
     * We are implicitly loaning tuples to the MUX here.
     * If fast tuple is available, use it, else, create another.
     */

    if (pSmEndDev->tupleId != NULL)
        {
        pMblk = pSmEndDev->tupleId;
        pSmEndDev->tupleId = NULL;
        }
#ifdef TUPLE
    else if ((pMblk = netTupleGet (pSmEndDev->end.pNetPool, SM_END_CLST_SIZ,
                                   M_DONTWAIT, MT_DATA, FALSE))
                    == NULL)
        {
        SM_LOG (SM_DBG_RX, "smEndTupleGet: Tuple get failed\n",
                0, 0, 0, 0, 0, 0);
        }
#else
    else
        {
        pNewCluster = netClusterGet (pSmEndDev->end.pNetPool,
                                     pSmEndDev->clPoolId);

        if (pNewCluster == NULL)
            {
            SM_LOG (SM_DBG_RX, "smEndRecv: no free clusters\n",
                    0, 0, 0, 0, 0, 0);
            END_ERR_ADD (&pSmEndDev->end, MIB2_IN_ERRS, +1);
            }

        /* Grab a cluster block to marry to the cluster we received. */

        else if ((pClBlk = netClBlkGet (pSmEndDev->end.pNetPool, M_DONTWAIT))
                         == NULL)
            {
            netClFree (pSmEndDev->end.pNetPool, pNewCluster);
            SM_LOG (SM_DBG_RX, "smEndRecv: no free clBlks\n",
                    0, 0, 0, 0, 0, 0);
            END_ERR_ADD (&pSmEndDev->end, MIB2_IN_ERRS, +1);
            }
    
        /*
         * OK we've got a spare, let's get an M_BLK_ID and marry it to the
         * one in the ring.
         */

        else if ((pMblk = mBlkGet (pSmEndDev->end.pNetPool, M_DONTWAIT,
                                   MT_DATA))
                        == NULL)
            {
            netClBlkFree (pSmEndDev->end.pNetPool, pClBlk); 
            netClFree (pSmEndDev->end.pNetPool, pNewCluster);
            SM_LOG (SM_DBG_RX, "smEndRecv: no free mBlks\n", 0, 0, 0, 0, 0, 0);
            END_ERR_ADD (&pSmEndDev->end, MIB2_IN_ERRS, +1);
            }

        else
            {
            /* Join the cluster to the MBlock */

            netClBlkJoin (pClBlk, pNewCluster, SM_END_CLST_SIZ, NULL, 0, 0, 0);
            netMblkClJoin (pMblk, pClBlk);
            }
        }
#endif /* TUPLE */

    return (pMblk);
    }


/******************************************************************************
*
* smEndRecv - process the next incoming packet
*
* Handle one incoming packet.  The packet is checked for errors.
*
* RETURNS: OK or ERROR.
*/

LOCAL STATUS smEndRecv
    (
    SM_END_DEV * pSmEndDev,	/* device structure */
    SM_PKT *     pPkt,		/* packet to process */
    M_BLK_ID     pMblk		/* mBlk to hold packet data */
    )
    {
    int                   len;
    struct ether_header * pEh    = (struct ether_header *)pPkt->data;
    u_char *              pData;		/* packet data to process */

    /* Packet must be checked for errors. */

    if ((len = SM_END_PKT_LEN_GET (pPkt)) < SIZEOF_ETHERHEADER)
        {
        SM_LOG (SM_DBG_RX, "smEndRecv: invalid packet len %d\n", len,
                0, 0, 0, 0, 0);
        END_ERR_ADD (&pSmEndDev->end, MIB2_IN_ERRS, +1);
        return (ERROR);		/* invalid packet size */
        }

    /* debugging aid */

    SM_LOG (SM_DBG_RX, "smEndRecv: [0x%x] len: %d src:%s ",
            ntohs (pEh->ether_type), len,
            (int)ether_sprintf (etherAddrPtr (pEh->ether_shost)), 0, 0, 0);
    SM_LOG (SM_DBG_RX, "dst:%s\n",
            (int)ether_sprintf (etherAddrPtr (pEh->ether_dhost)),
            0, 0, 0, 0, 0);

    /* adjust for enet header */

    len -= SIZEOF_ETHERHEADER;
    pData = (u_char *)(pEh + SIZEOF_ETHERHEADER);

    /* if packet with no data, just return */

    if (len != 0)
        {
        /* copy packet from sm to cluster */

        SM_END_COPY_TO_CLUSTER (pMblk, pData, len);

        /* Add one to our unicast data. */

        END_ERR_ADD (&pSmEndDev->end, MIB2_IN_UCAST, +1);

        /* set up mBlk */

        pMblk->mBlkHdr.mData   = pData;
        pMblk->mBlkHdr.mLen    = len;
        pMblk->mBlkHdr.mFlags |= M_PKTHDR;
        pMblk->mBlkPktHdr.len  = len;

        /* Call the upper layer's receive routine. */

        SM_LOG (SM_DBG_RX, "smEndRecv: calling MUX\n", 0, 0, 0, 0, 0, 0);
        END_RCV_RTN_CALL (&pSmEndDev->end, pMblk);
        }

    return (OK);
    }


/******************************************************************************
*
* smEndSrvcRcvInt - task level interrupt service for receiving sm packets
*
* This routine is called at task level indirectly by the interrupt service
* routine to do any sm receive packet processing.
*
* The double loop is to protect against a race condition where the interrupt
* code see rxHandling as TRUE, but it is then turned off by task code.
* This race is not fatal, but does cause occassional delays until a second
* packet is received and then triggers the netTask to call this routine again.
*
* RETURNS: N/A.
*/

LOCAL void smEndSrvcRcvInt
    (
    SM_END_DEV * pSmEndDev	/* interrupting device */
    )
    {
    FAST STATUS status;
    SM_PKT *    pPkt;		/* shared memory packet */
    M_BLK_ID    pMblk;
    int         cpuNum = pSmEndDev->localCpu;

    while ((pSmEndDev->smPktDesc.status == SM_CPU_ATTACHED) &&
           (smEndInputCount (pSmEndDev, cpuNum) > 0))
        {
        /*
         * If there is at least one more free tuple, get it and proceed.
         * If not, do not perform smPktRecv() because the sm packet would be
         * lost - there is no way to return a received sm packet to its list.
         */

        if (pSmEndDev->tupleId == NULL)
            {
    	    if ((pSmEndDev->tupleId = smEndTupleGet (pSmEndDev)) == NULL)
    	        {
                END_ERR_ADD (&pSmEndDev->end, MIB2_IN_ERRS, +1);
                SM_LOG (SM_DBG_INT, "smEndSrvcRcvInt: no free tuples\n",
                        0, 0, 0, 0, 0, 0);
    	        break;
    	        }
            }

    	if (((status = smPktRecv (&pSmEndDev->smPktDesc, &pPkt)) == OK) &&
    	    (pPkt != NULL))
    	    {
    	    pMblk = pSmEndDev->tupleId;
    	    pSmEndDev->tupleId = NULL;
            smEndRecv (pSmEndDev, pPkt, pMblk);
    	    }

	else if (status == ERROR)
	    {
            END_ERR_ADD (&pSmEndDev->end, MIB2_IN_ERRS, +1);
            SM_LOG (SM_DBG_INT, "smEndSrvcRcvInt: packet receive error\n",
                    0, 0, 0, 0, 0, 0);
	    }

	else
	    break;	/* no more sm packets to receive */
        }

    SM_RCV_TASK_INACTIVE;
    }


/******************************************************************************
*
* smEndTupleChainWalk - walk and gather information on a tuple chain
*
* This routine walks the tuple chain headed by <pMblk>, computes
* the number of fragments it is made of and the maximum data size of all
* fragments, and returns this information to the parameters referenced by
* <pFragNum> and <pMaxSize>.  In addition, it finds out whether the specified
* chain is unicast or multicast, and sets <pPktType> accordingly.
*
* RETURNS: OK, or ERROR in case of invalid mBlk (XXXmas: or multicast).
*/

LOCAL STATUS smEndTupleChainWalk
    (
    M_BLK *  pMblk,	/* pointer to the mBlk */
    UINT  *  pFragNum,	/* number of fragments */
    UINT16 * pPktType,	/* packet type */
    UINT *   pMaxSize	/* size of largest fragment */
    )
    {
    M_BLK *  pCurr = pMblk;	/* the current mBlk */

    if (pMblk == NULL)
        {
        SM_LOG (SM_DBG_TX, "smEndTupleChainWalk: invalid pMblk\n",
                0, 0, 0, 0, 0, 0);
        return (ERROR);
        }
    SM_LOG (SM_DBG_TX,
            "smEndTupleChainWalk: pMblk = %#x; pMblk->mBlkHdr.mNext = %#x\n",
            (unsigned)pMblk, (unsigned)pMblk->mBlkHdr.mNext, 0, 0, 0, 0);
    SM_LOG (SM_DBG_TX,
            "    pMblk data ptr = %#x; pMblk data len = %#x\n",
            (unsigned)pMblk->mBlkHdr.mData, (unsigned)pMblk->mBlkHdr.mLen,
            0, 0, 0, 0);
#ifdef	SM_DBG
    {
    char temp[512] = {0,0,0,0,0,0,0,0};
    char temp2 [8];
    int  i;

    for (i = 0; (i < 255) && (i < pMblk->mBlkHdr.mLen); ++i)
        {
        sprintf (temp2, "%2.2x", (unsigned)pMblk->mBlkHdr.mData[i]);
        strcat (temp, temp2);
        }
    SM_LOG (SM_DBG_TX, "    pMblk data = %s\n", (unsigned)temp, 0, 0, 0, 0, 0);
    taskDelay (60);
    }
#endif /* SM_DBG */

    /* walk this mBlk */

    *pMaxSize = 0;
    *pFragNum = 0;
    while (pCurr != NULL)
	{
	/* keep track of the number of fragments in this mBlk */

        *pFragNum += 1;

	/* record largest fragment size */

	if (pCurr->mBlkHdr.mLen > *pMaxSize)
	    *pMaxSize = pCurr->mBlkHdr.mLen;

	pCurr = ((M_BLK *) pCurr->mBlkHdr.mNext);
	}

    /* set the packet type to multicast or unicast */

    if (pMblk->mBlkHdr.mData[0] & (UINT8) 0x01)
	{
/* XXXmas  Multicast is a future enhancement; ignore for now */
	(*pPktType) = PKT_TYPE_MULTI;
        SM_LOG (SM_DBG_TX, "smEndTupleChainWalk: multicast request ignored\n",
                0, 0, 0, 0, 0, 0);
	}
    else
	{
	(*pPktType) = PKT_TYPE_UNI;
	}

    return (OK);
    }


/*******************************************************************************
*
* smEndSend - sm END send routine
*
* This routine accepts an M_BLK_ID and sends the data in it to the appropriate
* shared memory area.
*
* The buffer must already have the addressing information properly installed
* in it.  This is done by a higher layer.
*
* The last arguments are a free routine to be called when the device is done
* with the buffer and a pointer to the argument to pass to the free routine.
*
* RETURNS: OK or END_ERROR_BLOCK if we are in polled mode or insufficient
* sm buffer resources are available.
*/

LOCAL STATUS smEndSend
    (
    void *    pObj,	/* device ptr */
    M_BLK_ID  mBlkId	/* ID of mBlk (data) to send */
    )
    {
    SM_END_DEV *          pSmEndDev = (SM_END_DEV *)pObj; /* device ptr */
    M_BLK *               pMblk     = mBlkId;
    UINT                  fragNum;		/* number of fragments */
    UINT16                pktType;		/* packet type */
    UINT                  fragSizeMax;		/* max frag size (bytes) */
    unsigned		  destCPU;		/* destination cpu */
    struct ether_header * pEh      = NULL;	/* enet header */
    u_char *              pEaddr;		/* enet address ptr */
    unsigned		  ipAddr;
    SM_PKT *              pPkt     = NULL;	/* packet pointer */
    SM_PKT_INFO           smInfo;		/* sm info */
    
    SM_LOG (SM_DBG_TX, "smEndSend...\n", 0, 0, 0, 0, 0, 0);

    /*
     * Obtain exclusive access to shared memory.  This is necessary because
     * we might have more than one stack transmitting at once.
     */

    if (!(pSmEndDev->flags & SM_END_POLL_SM_RDY))
        {
	END_TX_SEM_TAKE (&pSmEndDev->end, WAIT_FOREVER);
        }

    else
        {
        /* if operating mode is polling, return error */

        SM_LOG (SM_DBG_TX, "smEndSend: polled mode enabled!\n",
                0, 0, 0, 0, 0, 0);
        END_ERR_ADD (&pSmEndDev->end, MIB2_OUT_ERRS, +1);
        return (END_ERR_BLOCK);
        }

    /* walk the mBlk tuple chain */

    if (smEndTupleChainWalk (mBlkId, &fragNum, &pktType, &fragSizeMax) ==ERROR)
        {
        SM_LOG (SM_DBG_TX, "smEndSend: invalid mBlk\n",
                0, 0, 0, 0, 0, 0);
        END_ERR_ADD (&pSmEndDev->end, MIB2_OUT_ERRS, +1);
        END_TX_SEM_GIVE (&pSmEndDev->end);
        errno = EINVAL;
        return (END_ERR_BLOCK);
        }
    SM_LOG (SM_DBG_TX, "smEndSend: walked %d tuples\n", fragNum,
            0, 0, 0, 0, 0);

    /* ensure sufficient sm packet resources for all data */

    if (smPktInfoGet (&pSmEndDev->smPktDesc, &smInfo) == ERROR)
        {
        SM_LOG (SM_DBG_TX, "smEndSend: cpu not attached!\n",
                0, 0, 0, 0, 0, 0);
        END_ERR_ADD (&pSmEndDev->end, MIB2_OUT_ERRS, +1);
        END_TX_SEM_GIVE (&pSmEndDev->end);
        return (END_ERR_BLOCK);
        }
    SM_LOG (SM_DBG_TX, "smEndSend: #free pkts = %d\n", smInfo.freePkts,
            0, 0, 0, 0, 0);

    if ((fragNum     > smInfo.freePkts) ||
        (fragSizeMax > pSmEndDev->smPktDesc.maxPktBytes))
        {
        SM_LOG (SM_DBG_TX, "smEndSend: sm resource shortage: FRAG    PKT\n",
                0, 0, 0, 0, 0, 0);
        SM_LOG (SM_DBG_TX, "                        Number:  %u    %d\n",
                fragNum, smInfo.freePkts, 0, 0, 0, 0);
        SM_LOG (SM_DBG_TX, "                          Size:  %u    %d\n",
                fragSizeMax, pSmEndDev->smPktDesc.maxPktBytes, 0, 0, 0, 0);
        END_ERR_ADD (&pSmEndDev->end, MIB2_OUT_ERRS, +1);
        END_TX_SEM_GIVE (&pSmEndDev->end);
        errno = ENOSPC;
        return (END_ERR_BLOCK);
        }

    /* initiate sm packet transfer */

    for ( ; (fragNum > 0) && (pMblk != NULL);
         --fragNum, pMblk = (M_BLK *)pMblk->mBlkHdr.mNext)
        {
    	if ((smPktFreeGet (&pSmEndDev->smPktDesc, &pPkt) == ERROR) ||
    	    (pPkt == NULL))
            {
            SM_LOG (SM_DBG_TX, "smEndSend: smPktFreeGet error 0x%x\n",
                    errno, 0, 0, 0, 0, 0);
            END_ERR_ADD (&pSmEndDev->end, MIB2_OUT_ERRS, +1);
            continue;
            }

    	/* point to ether address in local cluster data */

    	pEh = (struct ether_header *)pMblk->mBlkHdr.mData;

        /* check for broadcast message */

    	if (bcmp ((caddr_t) etherAddrPtr (pEh->ether_dhost),
		  (caddr_t) etherAddrPtr (etherbroadcastaddr) ,
		  sizeof (pEh->ether_dhost)) == 0)
            {
    	    destCPU  = SM_BROADCAST;	 /* specify broadcast */
            SM_LOG (SM_DBG_TX, "smEndSend: broadcast message\n",
                    0, 0, 0, 0, 0, 0);
            }

    	else
    	    {
	    pEaddr = etherAddrPtr (pEh->ether_dhost);

            /* sequential addressing */

            if (pSmEndDev->masterAddr != 0)
		{
		ipAddr = (pEaddr [3] << 16) | (pEaddr [4] << 8) | pEaddr [5];
		destCPU = ipAddr - pSmEndDev->masterAddr;  /* & 0x00FFFFFF ? */
                SM_LOG (SM_DBG_TX,
                        "smEndSend: seq adrs %#x - mstr %#x -> cpu #%u\n",
                        ipAddr, pSmEndDev->masterAddr, destCPU, 0, 0, 0);
		}
	    else
	        {
		destCPU = pEaddr [5];
                SM_LOG (SM_DBG_TX, "smEndSend: non-seq adrs -> cpu #%u\n",
                        destCPU, 0, 0, 0, 0, 0);
	        }

    	    /* sending to ourself */

            SM_LOG (SM_DBG_TX, "smEndSend: dest CPU = %u\n",
                    destCPU, 0, 0, 0, 0, 0);

    	    if (destCPU == pSmEndDev->localCpu)
    		{
                /*
                 * Call the upper layer's receive routine and
                 * return unused packet.
                 */

                SM_LOG (SM_DBG_TX, "smEndSend: sent to ourselves\n",
                        0, 0, 0, 0, 0, 0);
                END_RCV_RTN_CALL (&pSmEndDev->end, pMblk);
                END_ERR_ADD (&pSmEndDev->end, MIB2_OUT_UCAST, +1);
                smPktFreePut (&pSmEndDev->smPktDesc, pPkt);
		continue;
    		}
    	    }

    	/* copy data to pkt */

    	pEh = (struct ether_header *)pPkt->data;
        SM_END_COPY_FROM_CLUSTER (pMblk, (char *)pEh, pMblk->mBlkHdr.mLen);
    	pPkt->header.nBytes = pMblk->mBlkHdr.mLen;

        /* diagnostics */

        SM_LOG (SM_DBG_TX, "smEndSend: [0x%x] len:%d src:%s\n",
                pEh->ether_type, pMblk->mBlkHdr.mLen,
                (int)ether_sprintf (etherAddrPtr (pEh->ether_shost)), 0, 0, 0);
        SM_LOG (SM_DBG_TX, "dst:%s cpu [%#x]\n",
                (int)ether_sprintf (etherAddrPtr (pEh->ether_dhost)), destCPU,
                0, 0, 0, 0);

        /* now send (announce) packet to destination cpu */

    	if (smPktSend (&pSmEndDev->smPktDesc, pPkt, destCPU) == ERROR)
    	    {
            SM_LOG (SM_DBG_TX, "smEndSend: smPktSend error: 0x%x\n",
                    errno, 0, 0, 0, 0, 0);
            END_ERR_ADD (&pSmEndDev->end, MIB2_OUT_ERRS, +1);

	    /*
	     * need to return shared memory packet on error,
	     * unless it's an incomplete broadcast error.
	     */

    	    if (errno != S_smPktLib_INCOMPLETE_BROADCAST)
    		(void)smPktFreePut (&pSmEndDev->smPktDesc, pPkt);
    	    }

        /* Bump the statistic counter. */

    	else
            END_ERR_ADD (&pSmEndDev->end, MIB2_OUT_UCAST, +1);
        }

    /* relinquish ownership of shared memory */

    END_TX_SEM_GIVE (&pSmEndDev->end);
    
    /* Cleanup.  The driver must free the mBlk(s) now. */

    netMblkClChainFree (mBlkId);

    return (OK);
    }


/******************************************************************************
*
* smEndPollQPut - enqueue a polled mode tuple
*
* This routine queues a tuple sent in polled mode to the local cpu so that
* it may later be returned to an upper layer via a polled receive call.
*
* RETURNS: N/A
*
* SEE ALSO: smEndPollQGet() and smEndPollQFree()
*/

LOCAL void smEndPollQPut
    (
    SM_END_DEV * pSmEndDev,	/* device containing queue */
    M_BLK_ID     mBlkId		/* polled tuple to be queued */
    )
    {
    mBlkId->mBlkHdr.mNext = NULL;

    /* make first entry in queue */

    if (pSmEndDev->pollQ == NULL)
        {
        pSmEndDev->pollQ     = mBlkId;
        pSmEndDev->pollQLast = mBlkId;
        }

    /*
     * append to end of queue
     * mas Note: prioritization could be supported here in the future
     */

    else
        {
        pSmEndDev->pollQLast->mBlkHdr.mNext = mBlkId;
        pSmEndDev->pollQLast = mBlkId;
        }
    }


/******************************************************************************
*
* smEndPollQGet - get next queued tuple
*
* This routine gets the next queued tuple, if any, and copies its cluster to
* the cluster of the receiving tuple specified by <mBlkId>.
*
* RETURNS: OK upon success, NONE if no tuple is available, or EAGAIN if the
* receiving tuple cluster size is too small.
*
* SEE ALSO: smEndPollQPut() and smEndPollQFree()
*/

LOCAL int smEndPollQGet
    (
    SM_END_DEV * pSmEndDev,	/* device containing queue */
    M_BLK_ID     mBlkId		/* tuple to receive queued tuple */
    )
    {
    M_BLK_ID pMblk = NULL;

    /* if no queued tuples, return NONE */

    if (pSmEndDev->pollQ == NULL)
        return (NONE);

    /*
     * if queued tuple has more data than receiving tuple can hold,
     * return EAGAIN
     */

    pMblk = pSmEndDev->pollQ;
    if (pMblk->mBlkHdr.mLen > mBlkId->pClBlk->clSize)
        return (EAGAIN);

    /* copy data from queued tuple to receiving tuple */

    bcopy (pMblk->mBlkHdr.mData, mBlkId->mBlkHdr.mData, pMblk->mBlkHdr.mLen);
    mBlkId->mBlkHdr.mLen   = pMblk->mBlkHdr.mLen;
    mBlkId->mBlkHdr.mFlags = pMblk->mBlkHdr.mFlags;
    mBlkId->mBlkPktHdr.len = pMblk->mBlkPktHdr.len;

    /* remove tuple from queue and free it */

    pSmEndDev->pollQ = pMblk->mBlkHdr.mNext;
    if (pSmEndDev->pollQ == NULL)
        pSmEndDev->pollQLast = NULL;
    netMblkClFree (pMblk);

    return (OK);
    }


/******************************************************************************
*
* smEndPollQFree - free all queued tuples in the specified device
*
* This routine frees all tuples, if any, queued in the specified device.
*
* RETURNS: N/A
*
* SEE ALSO: smEndPollQGet() and smEndPollQPut()
*/

LOCAL void smEndPollQFree
    (
    SM_END_DEV * pSmEndDev	/* device containing queue */
    )
    {
    M_BLK_ID pMblk    = NULL;
    M_BLK_ID pMblkNxt = NULL;

    pMblk = pSmEndDev->pollQ;
    while (pMblk != NULL)
        {
        pMblkNxt = pMblk->mBlkHdr.mNext;
        netMblkClFree (pMblk);
        pMblk = pMblkNxt;
        }
    pSmEndDev->pollQLast = pSmEndDev->pollQ = NULL;
    }


/******************************************************************************
*
* smEndPollRecv - routine to receive a packet in polled mode.
*
* This routine is called by a user to try and get a packet from the device.
*
* RETURNS: OK upon success.  EAGAIN is returned when no packet is available,
* the cpu is not attached to sm, or no free mBlks.  EINVAL is returned if
* either input argument is NULL.  EACCES is returned if device is not in
* polled mode.
*/

LOCAL STATUS smEndPollRecv
    (
    VOID *    pObj,	/* device to be polled */
    M_BLK_ID  pMblk	/* ptr to buffer */
    )
    {
    SM_END_DEV * pSmEndDev = (SM_END_DEV *)pObj; /* device to be polled */
    int          cpuNum    = pSmEndDev->localCpu;
    SM_PKT *     pPkt      = NULL;		/* shared memory packet */
    struct ether_header * pEh;			/* ethernet header */
    u_char *     pData;				/* packet data to process */
    int          qStatus;
    int          len;

    if ((pObj == NULL) || (pMblk == NULL))
        {
        END_ERR_ADD (&pSmEndDev->end, MIB2_IN_ERRS, +1);
        return (EINVAL);
        }

    /* cpu attached and in polled mode? */

    if (!SM_END_POLL_SM_RDY)
        {
        END_ERR_ADD (&pSmEndDev->end, MIB2_IN_ERRS, +1);
        return (EACCES);
        }

    /* return next queued tuple, if any */

    if ((qStatus = smEndPollQGet (pSmEndDev, pMblk)) != NONE)
        {
        if (qStatus == OK)
            {
            /* Add one to our unicast data. */

            END_ERR_ADD (&pSmEndDev->end, MIB2_IN_UCAST, +1);
            }

        return (qStatus);
        }

    /* any sm packets? */

    if (smEndInputCount (pSmEndDev, cpuNum) < 1)
        {
        END_ERR_ADD (&pSmEndDev->end, MIB2_IN_ERRS, +1);
        return (EAGAIN);
        }

    /* get packet address */

    if ((smPktRecv (&pSmEndDev->smPktDesc, &pPkt) != OK) ||
        (pPkt == NULL))
        {
        END_ERR_ADD (&pSmEndDev->end, MIB2_IN_ERRS, +1);
        return (EAGAIN);
        }

    /* Get packet length */

    if ((len = SM_END_PKT_LEN_GET (pPkt)) < SIZEOF_ETHERHEADER)
        {
        END_ERR_ADD (&pSmEndDev->end, MIB2_IN_ERRS, +1);
        return (EAGAIN);		/* invalid packet size */
        }

    /* adjust for enet header */

    pEh = (struct ether_header *)pPkt->data;	/* ethernet header */
    len -= SIZEOF_ETHERHEADER;
    pData = ((u_char *) pEh) + SIZEOF_ETHERHEADER;

    /* Upper layer must provide a valid buffer */

    if ((pMblk->mBlkHdr.mLen < len) || (!(pMblk->mBlkHdr.mFlags & M_EXT)))
	{
	return (EAGAIN);
	}

    /* if packet with no data, just return */

    if (len == 0)
        {
	return (EAGAIN);
        }

    /* set up mBlk */

    pMblk->mBlkHdr.mLen    = len;
    pMblk->mBlkHdr.mFlags |= M_PKTHDR;
    pMblk->mBlkPktHdr.len  = len;

    /* copy packet from sm to tuple cluster and return to MUX */

    SM_END_COPY_TO_CLUSTER (pMblk, pData, len);

    /* Add one to our unicast data. */

    END_ERR_ADD (&pSmEndDev->end, MIB2_IN_UCAST, +1);

    return (OK);
    }


/******************************************************************************
*
* smEndPollSend - routine to send a packet in polled mode.
*
* This routine is called by a user to try and send a packet on the device.
*
* RETURNS: OK upon success.  ERROR is returned and errno is set to:
* EINVAL - if either input argument is NULL.
* EACCES - if device is not in polled mode.
* EAGAIN is returned in all other cases.
*/

LOCAL STATUS smEndPollSend
    (
    void *   pObj,	/* device to be polled */
    M_BLK_ID pMblk	/* packet to send */
    )
    {
    SM_END_DEV *          pSmEndDev = (SM_END_DEV *)pObj; /* device to poll */
    unsigned              len;
    int                   destCPU;		/* destination cpu */
    struct ether_header * pEh      = NULL;	/* enet header */
    u_char *              pEaddr;		/* enet address ptr */
    SM_PKT *              pPkt     = NULL;	/* packet pointer */
    SM_PKT_INFO           smInfo;		/* sm info */

    if ((pObj == NULL) || (pMblk == NULL))
        {
        END_ERR_ADD (&pSmEndDev->end, MIB2_OUT_ERRS, +1);
        errno = EINVAL;
        return (ERROR);
        }

    /* cpu attached and in polled mode? */

    if (!SM_END_POLL_SM_RDY)
        {
        END_ERR_ADD (&pSmEndDev->end, MIB2_OUT_ERRS, +1);
        errno = EACCES;
        return (ERROR);
        }

/* ??? XXXmas is this next step needed? */
    /*
     * Obtain exclusive access to shared memory.  This is necessary because
     * we might have more than one stack transmitting at once.
     */

    END_TX_SEM_TAKE (&pSmEndDev->end, WAIT_FOREVER);

    /* ensure sufficient sm packet resources for one cluster */

    if (smPktInfoGet (&pSmEndDev->smPktDesc, &smInfo) == ERROR)
        {
        END_ERR_ADD (&pSmEndDev->end, MIB2_OUT_ERRS, +1);
        END_TX_SEM_GIVE (&pSmEndDev->end);
        return (EAGAIN);
        }

    if (smInfo.freePkts == 0)
        {
        END_ERR_ADD (&pSmEndDev->end, MIB2_OUT_ERRS, +1);
        END_TX_SEM_GIVE (&pSmEndDev->end);
        return (EAGAIN);
        }

    /* get a free sm packet */

    if ((smPktFreeGet (&pSmEndDev->smPktDesc, &pPkt) == ERROR) ||
        (pPkt == NULL))
        {
        END_ERR_ADD (&pSmEndDev->end, MIB2_OUT_ERRS, +1);
        return (EAGAIN);
        }

    /* point to ether address in local cluster data */

    pEh = (struct ether_header *)pMblk->mBlkHdr.mData;

    /* check for broadcast message */

    if (bcmp ((caddr_t) etherAddrPtr (pEh->ether_dhost),
              (caddr_t) etherAddrPtr (etherbroadcastaddr) ,
              sizeof (pEh->ether_dhost)) == 0)
        destCPU  = SM_BROADCAST;	 /* specify broadcast */

    else
        {
        pEaddr = etherAddrPtr (pEh->ether_dhost);

        /* sequential addressing */

        if (pSmEndDev->masterAddr != 0)
            {
            destCPU = ((pEaddr [3] << 16)|(pEaddr [4] << 8)| pEaddr [5]) -
                      pSmEndDev->masterAddr;
            }
        else
            destCPU = pEaddr [5];

        /* sending to ourself */

        if (destCPU == pSmEndDev->localCpu)
            {
            /*
             * Being in polled mode means we cannot call the upper layer's
             * receive routine.  Instead, the outbound tuple must be queued
             * for polled receiving.  Caveat: tuples may be lost if not all
             * queued tuples are read by the upper layer or interrupt mode
             * is not entered.  Entering interrupt mode frees queued tuples.
             */

            SM_LOG (SM_DBG_TX, "smEndPollSend: sent to ourselves\n",
                    0, 0, 0, 0, 0, 0);
            smEndPollQPut (pSmEndDev, pMblk);
            END_ERR_ADD (&pSmEndDev->end, MIB2_OUT_UCAST, +1);
            END_TX_SEM_GIVE (&pSmEndDev->end);
            return (OK);
            }
        }

    /* copy data from cluster to sm packet */

    pEh = (struct ether_header *)  pPkt->data;
    len = max (ETHERSMALL, pMblk->mBlkHdr.mLen);
    SM_END_COPY_FROM_CLUSTER (pMblk, (char *)pEh, len);
    pPkt->header.nBytes = pMblk->mBlkHdr.mLen;

    /* now send (announce) packet to destination cpu */

    if (smPktSend (&pSmEndDev->smPktDesc, pPkt, destCPU) == ERROR)
        {
        END_ERR_ADD (&pSmEndDev->end, MIB2_OUT_ERRS, +1);

        /*
         * need to return shared memory packet on error,
         * unless it's an incomplete broadcast error.
         */

        if (errno != S_smPktLib_INCOMPLETE_BROADCAST)
            (void)smPktFreePut (&pSmEndDev->smPktDesc, pPkt);

        return (EAGAIN);
        }

    /* Bump the statistic counter. */

    else
        END_ERR_ADD (&pSmEndDev->end, MIB2_OUT_UCAST, +1);

    /* relinquish ownership of shared memory */

    END_TX_SEM_GIVE (&pSmEndDev->end);
    
    /* Cleanup.  The driver must free the tuple now. */

    netMblkClFree (pMblk);

    return (OK);
    }


/******************************************************************************
*
* smEndMCastAddrAdd - add a multicast address for the device
*
* This routine adds a multicast address to whatever the driver
* is already listening for.
* 
* RETURNS: OK or ERROR.
*
* SEE ALSO: smEndMCastAddrDel() smEndMCastAddrGet()
*/

LOCAL STATUS smEndMCastAddrAdd
    (
    void * pObj,	/* cookie pointer */
    char * pAddr	/* address to be added */
    )
    {
    int          retVal    = ERROR;
#if FALSE	/* XXXmas  multicast is a future enhancement */
    SM_END_DEV * pSmEndDev = (SM_END_DEV *)pObj; /* device ptr */

    SM_LOG (SM_DBG_MCAST,
            "smEndMCastAddrAdd addr = 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x\n",
	    (int) (*pAddr + 0), (int) (*pAddr + 1), 
	    (int) (*pAddr + 2), (int) (*pAddr + 3), 
	    (int) (*pAddr + 4), (int) (*pAddr + 5));

    retVal = etherMultiAdd (&pSmEndDev->end.multiList, pAddr);

    if (retVal == ENETRESET)
	{
        pSmEndDev->end.nMulti++;
 
	return (smEndHashTblAdd (pSmEndDev, pAddr));
	}
#else
    errno = ENOTSUP;
#endif

    return ((retVal == OK) ? OK : ERROR);
    }

/******************************************************************************
*
* smEndMCastAddrDel - delete a multicast address for the device
*
* This routine deletes a multicast address from the current list of
* multicast addresses.
* 
* RETURNS: OK or ERROR.
*
* SEE ALSO: smEndMCastAddrAdd() smEndMCastAddrGet()
*/

LOCAL STATUS smEndMCastAddrDel
    (
    void * pObj,	/* cookie pointer */
    char * pAddr	/* address to be deleted */
    )
    {
    int          retVal    = ERROR;
#if FALSE	/* XXXmas  multicast is a future enhancement */
    SM_END_DEV * pSmEndDev = (SM_END_DEV *)pObj; /* device ptr */

    SM_LOG (SM_DBG_MCAST,
            "smEndMCastAddrDel addr = 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x\n",
	    (int) (*pAddr + 0), (int) (*pAddr + 1), 
	    (int) (*pAddr + 2), (int) (*pAddr + 3), 
	    (int) (*pAddr + 4), (int) (*pAddr + 5));

    retVal = etherMultiDel (&pSmEndDev->end.multiList, pAddr);

    if (retVal == ENETRESET)
	{
	pSmEndDev->end.nMulti--;

	/* stop the sm device */

	if (smEndStop (pSmEndDev) != OK)
	    return (ERROR);

	/* populate the hash table */

	retVal = smEndHashTblPopulate (pSmEndDev);

	/* restart the sm device */

	if (smEndStart (pSmEndDev) != OK)
	    return (ERROR);
	}
#else
    errno = ENOTSUP;
#endif

    return ((retVal == OK) ? OK : ERROR);
    }

/******************************************************************************
*
* smEndMCastAddrGet - get the current multicast address list
*
* This routine returns the current multicast address list in <pTable>
*
* RETURNS: OK or ERROR.
*
* SEE ALSO: smEndMCastAddrAdd() smEndMCastAddrDel()
*/

LOCAL STATUS smEndMCastAddrGet
    (
    void *        pObj,		/* cookie pointer */
    MULTI_TABLE * pTable	/* table into which to copy addresses */
    )
    {
#if FALSE	/* XXXmas  multicast is a future enhancement */
    SM_END_DEV * pSmEndDev = (SM_END_DEV *)pObj; /* device ptr */

    SM_LOG (SM_DBG_MCAST, "smEndMCastAddrGet\n", 0, 0, 0, 0, 0, 0);

    return (etherMultiGet (&pSmEndDev->end.multiList, pTable));
#else
    errno = ENOTSUP;
    return (ERROR);
#endif
    }


/*******************************************************************************
*
* smEndAddrResolve - resolve an Ethernet address
*
* This routine converts a destination IP address into a destination
* Ethernet HW address.  If sequential addressing is being used and the
* destination is attached to shared memory, a simple mapping is performed as
* explained in smEndHwAddrSet().  Otherwise, a direct lookup in the sm
* CPU descriptors is performed, matching the specified IP address to an
* attached CPU.
*
* RETURNS: 1 (send packet) if successful, otherwise 0 (pend packet).
*
* SEE ALSO: smEndHwAddrSet()
*
* INTERNAL: This routine is based on arpioctl(SIOCSARP) rev 01g,03sep98.
*/

LOCAL int smEndAddrResolve
    (
    FAST struct arpcom *   pAc,		/* arpcom structure */
    FAST struct rtentry *  pRt,		/* retry entry list */
    struct mbuf *          pMbuf,	/* resolve address for this mbuf */
    FAST struct sockaddr * pDestIP,	/* destination IP address */
    FAST u_char *          pDestHW	/* destination ethernet HW address */
    )
    {
    struct llinfo_arp *  pLa;
    struct sockaddr_dl * pSdl;
    SM_PKT_CPU_DESC *	 pPktCpuDesc;			
    u_long		 dstHostAddr;
    unsigned		 destCpu;
    unsigned		 maxCpus;
    SM_END_DEV *	 pSmEndDev = unitTbl [pAc->ac_if.if_unit];
    FAST SM_CPU_DESC *	 pCpuDesc;		/* sm CPU descriptor */
    FAST SM_ANCHOR *	 pAnchor = pSmEndDev->pAnchor;
    FAST SM_HDR *	 pSmHdr;
    FAST SM_DESC *	 pSmDesc = &pSmEndDev->smPktDesc.smDesc;
    static u_char	 etherbroadcastaddr[6] =
    {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};

    if (pMbuf != NULL)          /* check if mbuf is not null */
        {
        if (pMbuf->m_flags & M_BCAST)
            {       /* broadcast */
            bcopy((caddr_t)etherbroadcastaddr, (caddr_t)pDestHW,
                  sizeof(etherbroadcastaddr));
            SM_LOG (SM_DBG_RSLV, "smEndAddrResolve: broadcast packet\n",
                    0, 0, 0, 0, 0, 0);
            return (1);
            }
        if (pMbuf->m_flags & M_MCAST)
            {       /* multicast */
            ETHER_MAP_IP_MULTICAST(&((struct sockaddr_in *)pDestIP)->sin_addr,
                                   pDestHW);
            SM_LOG (SM_DBG_RSLV, "smEndAddrResolve: multicast packet\n",
                    0, 0, 0, 0, 0, 0);
            return(1);
            }
        }

    /* preliminary lookup */

    if (pRt != NULL)
        pLa = (struct llinfo_arp *)pRt->rt_llinfo;
    else
#ifndef SM_ARP_SUPPORT
        {
        SM_LOG (SM_DBG_RSLV, "smEndAddrResolve: ARP not supported!\n",
                0, 0, 0, 0, 0, 0);
        m_freem(pMbuf);
        return (0);
        }
#else
        {
        if ((pLa = arplookup(((struct sockaddr_in *)pDestIP)->sin_addr.s_addr,
                            1, 0)) != NULL)
            pRt = pLa->la_rt;
        }
#endif /* SM_ARP_SUPPORT */

    if (pLa == NULL || pRt == NULL)
        {
        SM_LOG (SM_DBG_RSLV, "smEndAddrResolve: can't allocate llinfo\n",
                0, 0, 0, 0, 0, 0);
        m_freem(pMbuf);
        return (0);
        }

    /* prepare router table pointer and entry */

    pSdl = (struct sockaddr_dl *)(pRt->rt_gateway);
    pRt->rt_rmx.rmx_expire = 0;	/* make table entry permanent */

    /* if entry already exists, just return it */

    if ((pSdl->sdl_family == AF_LINK) && (pSdl->sdl_alen != 0))
        {
        pRt->rt_flags &= ~RTF_REJECT;
        bcopy ((char *)((caddr_t)(pSdl->sdl_data + pSdl->sdl_nlen)),
               pDestHW, pSdl->sdl_alen);
        return (1);
        }

    /* derive destination CPU IP address */

    dstHostAddr = ((struct sockaddr_in *)pDestIP)->sin_addr.s_addr;
    dstHostAddr &= 0x00ffffff;

    /*
     * If sequential addressing mode, use simple address translation
     * mechanism to resolve HW address.
     */

    if (pSmEndDev->masterAddr != 0)
        {
        /*
         * Check for erroneous input or unattached CPU.
         * If either, free the buffer and return 0.
         */

        destCpu = dstHostAddr - pSmEndDev->masterAddr;
        if ((destCpu < 0) || (destCpu >= pSmEndDev->smPktDesc.smDesc.maxCpus))
            {
            SM_LOG (SM_DBG_RSLV, "smEndAddrResolve: illegal CPU #%u\n",
                    destCpu, 0, 0, 0, 0, 0);
            m_freem (pMbuf);
            return (0);
            }

        /*
         * XXXmas this would reflect current information if the actual sm
         * structure were read rather than a local copy.  Although it has
         * greater overhead and impacts bus traffic, it will allow for Hot
         * Swap of processor boards.  For the future...
         */

        pPktCpuDesc = &((pSmEndDev->smPktDesc.cpuLocalAdrs) [destCpu]);

        if (pPktCpuDesc->status != SM_CPU_ATTACHED)
            {
            SM_LOG (SM_DBG_RSLV, "smEndAddrResolve: CPU #%u not attached\n",
                    destCpu, 0, 0, 0, 0, 0);
            m_freem (pMbuf);
            return (0);
            }

        pDestHW [0] = 0x00;
        pDestHW [1] = 0x02;
        pDestHW [2] = 0xE2;
        pDestHW [3] = (dstHostAddr >> 16) & 0xff;
        pDestHW [4] = (dstHostAddr >> 8) & 0xff;
        pDestHW [5] = dstHostAddr & 0xff;
        }

    /*
     * Non-sequential addressing is used.  Must walk the array of CPU
     * descriptors in sm and find a match to the specified IP address.
     * If no match is found, it as assumed that the destination CPU is
     * not yet attached.  In this case, the buffer is freed and zero is
     * returned.
     */

    else
        {
        /* derive major sm structure pointers */

        pSmHdr   = SM_OFFSET_TO_LOCAL (ntohl (pAnchor->smHeader),
                                       pSmDesc->base,
                                       SM_HDR *);
        pCpuDesc = SM_OFFSET_TO_LOCAL (ntohl (pSmHdr->cpuTable),
                                       pSmDesc->base,
                                       SM_CPU_DESC *);

        /* check each CPU descriptor until IP address match or end of list */

        for (destCpu = 0, maxCpus = pSmHdr->maxCpus;
             destCpu < maxCpus;
             ++destCpu)
            {
            if (pCpuDesc[destCpu].reserved1 == dstHostAddr)
                break;
            }

        if (destCpu >= maxCpus)
            {
            SM_LOG (SM_DBG_RSLV,
                  "smEndAddrResolve: CPU with IP = %u.%u.%u.%u not attached\n",
                    (dstHostAddr >> 24) & 0x000000FF,
                    (dstHostAddr >> 16) & 0x000000FF,
                    (dstHostAddr >>  8) & 0x000000FF,
                    dstHostAddr & 0x000000FF, 0, 0);
            m_freem (pMbuf);
            return (0);
            }

        /* match found; derive sm HW address */

        pDestHW [0] = 0x00;
        pDestHW [1] = 0x02;
        pDestHW [2] = 0xE2;
        pDestHW [3] = 0x00;
        pDestHW [4] = pSmEndDev->unit & 0xff;
        pDestHW [5] = destCpu & 0xff;
        }

    /* enter directly into arp table for efficency */

    pSdl->sdl_alen = EADDR_LEN;   /* sizeof ethernet HW address */
    bcopy (pDestHW, (char *)LLADDR(pSdl), pSdl->sdl_alen); 

    pRt->rt_flags &= ~RTF_REJECT;

    return (1);
    }

