/* motCpmEnd.c - END style Motorola MC68EN360/MPC800 network interface driver */

/* Copyright 1997-2002 Wind River Systems, Inc. */
#include "copyright_wrs.h"

/*
modification history
--------------------
02e,11apr02,rcs  increased clusters, mBlks, clBlks and ring size (SPR# 75471)
02d,01aug00,stv	 allocated END_CTRL structure from heap (SPR# 32775).	
02c,20jun00,stv	 fixed the setting of maxd1 & maxd2 registers (SPR# 31197).  
02b,11jun00,ham  removed reference to etherLib.
02a,26may00,stv  fixed the packet data length in motCpmEndPollReceive() routine,
		 (SPR 31218).
01z,27jan00,dat  fixed use of NULL
01y,01dec99,stv  freed mBlk chain before returning ERROR (SPR #28492).
01x,29mar99,dat  SPR 26119, documentation, usage of .bS/.bE
01w,26nov98,dbt  removed splimp() and splx() calls in motCmpEndIoctl
		 (SPR #23528). Added a missing netMblkClChainFree() call.
01v,22sep98,dat  added Didier's fix for system-mode transitions. (SPR 22325)
		 removed warnings.
01u,09sep98,n_s  removed promiscous mode filter from motCpmEndPollReceive (). 
                 spr 21143.
01t,28aug98,dat  chg'd all names from motCmp to motCpm.
01s,21aug98,n_s  removed promiscous mode filter from motCmpRecv (). spr 21143.
01r,30jun98,cn   changed include file motCmpEnd.h to motCpmEnd.h
01q,22jun98,cn   added support for PPC800-series Ethernet Controller.
01p,11dec97,kbw  making man page edits
01o,08dec97,gnn  END code review fixes.
01n,19oct97,vin	 moved swapping of loaned buffer before END_RCV_RTN_CALL
01m,17oct97,vin  removed extra free.
01l,07oct97,vin  MTU size to ETHER_MTU.
01k,03oct97,gnn  fixed SPR 8988, memory leak
01j,25sep97,gnn  SENS beta feedback fixes
01i,24sep97,vin  added clBlk related calls
01h,03sep97,gnn  fixed a crashing bug under heavy load
01g,25aug97,gnn  changes due to new netPool routines.
01f,22aug97,gnn  changes due to new buffering scheme.
01e,12aug97,gnn  changes necessitated by MUX/END update.
01d,15may97,map  fixed intr event ack (scce) handling (SPR# 8580).
01c,18apr97,gnn  fixed a bug in the receive code.
01b,09apr97,gnn  updated buffer handling code to follow new MUX level
                 requirements.
01a,15feb97,dbt	 modified if_qu.c to END style.	
*/

/*
This module implements the Motorola MC68EN360 QUICC as well as the MPC821 and
MPC860 Power-QUICC Ethernet Enhanced network interface driver.

All the above mentioned microprocessors feature a number of Serial 
Communication Controllers (SCC) that support different serial protocols
including IEEE 802.3 and Ethernet CSMA-CD. As a result, when the Ethernet
mode of a SCC is selected, by properly programming its general Mode Register
(GSMR), they can implement the full set of media access control and channel
interface functions those protocol require. However, while the MC68EN360 
QUICC and the MPC860 Power-QUICC support up to four SCCs per unit, the 
MPC821 only includes two on-chip SCCs.

This driver is designed to support the Ethernet mode of a SCC residing on the
CPM processor core, no matter which among the MC68EN360 QUICC or any of the 
PPC800 Series. In fact, the major differences among these processors, as far
as the driver is concerned, are to be found in the mapping of the internal 
Dual-Port RAM. The driver is generic in the sense that it does not care
which SCC is being used. In addition, it poses no constraint on the number 
of individual units that may be used per board. However, this number should
be specified in the bsp through the macro MAX_SCC_CHANNELS. The default value
for this macro in the driver is 4.

To achieve these goals, the driver requires several target-specific values
provided as an input string to the load routine. It also requires some 
external support routines.  These target-specific values and the external 
support routines are described below.

This network interface driver does not include support for trailer protocols
or data chaining.  However, buffer loaning has been implemented in an effort 
to boost performance. 

This driver maintains cache coherency by allocating buffer space using 
the cacheDmaMalloc() routine.  This is provided for boards whose host
processor use data cache space, e.g. the MPC800 Series. Altough the 
MC68EN360 does not have cache memory, it may be used in a particular 
configuration: 'MC68EN360 in 040 companion mode' where that is attached
to processors that may cache memory. However, due to a lack of suitable 
hardware, the multiple unit support and '040 companion mode support have 
not been tested.

BOARD LAYOUT
This device is on-chip.  No jumpering diagram is necessary.

EXTERNAL INTERFACE
This driver provides the standard END external interface.  The only external 
interface is the motCpmEndLoad() routine.  The parameters are passed 
into the motCpmEndLoad() function as a single colon-delimited string.
The motCpmEndLoad() function uses strtok() to parse the string, which it 
expects to be of the following format:

<unit>:<motCpmAddr>:<ivec>:<sccNum>:<txBdNum>:<rxBdNum>:
<txBdBase>: <rxBdBase>:<bufBase> 

TARGET-SPECIFIC PARAMETERS

.IP <unit>
A convenient holdover from the former model.  This parameter is used only
in the string name for the driver.

.IP <motCpmAddr>
Indicates the address at which the host processor presents its internal 
memory (also known as the dual ported RAM base address). With this address, 
and the SCC number (see below), the driver is able to compute the location 
of the SCC parameter RAM and the SCC register map, and, ultimately, to 
program the SCC for proper operations. This parameter should point to the 
internal memory of the processor where the SCC physically resides. This 
location might not necessarily be the Dual-Port RAM of the microprocessor 
configured as master on the target board.

.IP <ivec>
This driver configures the host processor to generate hardware interrupts
for various events within the device. The interrupt-vector offset
parameter is used to connect the driver's ISR to the interrupt through
a call to the VxWorks system function intConnect().

.IP <sccNum>
This driver is written to support multiple individual device units.
Thus, the multiple units supported by this driver can reside on different 
chips or on different SCCs within a single host processor. This parameter 
is used to explicitly state which SCC is being used (SCC1 is most commonly 
used, thus this parameter most often equals "1").

.IP "<txBdNum> and <rxBdNum>"
Specify the number of transmit and receive buffer descriptors (BDs). 
Each buffer descriptor resides in 8 bytes of the processor's dual-ported 
RAM space, and each one points to a 1520 byte buffer in regular RAM.  
There must be a minimum of two transmit and two receive BDs. There is 
no maximum, although more than a certain amount does not speed up the 
driver and wastes valuable dual-ported RAM space. If any of these parameters
is "NULL", a default value of "32" BDs is used.

.IP "<txBdBase> and <rxBdBase>"
Indicate the base location of the transmit and receive buffer descriptors 
(BDs). They are offsets, in bytes, from the base address of the host
processor's internal memory (see above). Each BD takes up 8 bytes of
dual-ported RAM, and it is the user's responsibility to ensure that all
specified BDs fit within dual-ported RAM. This includes any other
BDs the target board might be using, including other SCCs, SMCs, and the
SPI device. There is no default for these parameters.  They must be
provided by the user.

.IP <bufBase> 
Tells the driver that space for the transmit and receive buffers need not 
be allocated but should be taken from a cache-coherent private memory space 
provided by the user at the given address.  The user should be aware that 
memory used for buffers must be 4-byte aligned and non-cacheable.  All the 
buffers must fit in the given memory space.  No checking is performed.  
This includes all transmit and receive buffers (see above).  Each buffer 
is 1520 bytes.  If this parameter is "NONE", space for buffers is obtained 
by calling cacheDmaMalloc() in motCpmEndLoad().
.LP

EXTERNAL SUPPORT REQUIREMENTS
This driver requires three external support functions:
.IP sysXxxEnetEnable()
.LP
This is either sys360EnetEnable() or sysCpmEnetEnable(), based on the 
actual host processor being used. See below for the actual prototypes.
This routine is expected to handle any target-specific functions needed 
to enable the Ethernet controller. These functions typically include 
enabling the Transmit Enable signal (TENA) and connecting the transmit 
and receive clocks to the SCC. This routine is expected to return OK on 
success, or ERROR. The driver calls this routine, once per unit, from the 
motCpmEndLoad() routine.
.IP sysXxxEnetDisable()
.LP
This is either sys360EnetDisable() or sysCpmEnetDisable(), based on the 
actual host processor being used. See below for the actual prototypes.
This routine is expected to handle any target-specific functions required 
to disable the Ethernet controller. This usually involves disabling the 
Transmit Enable (TENA) signal.  This routine is expected to return OK on 
success, or ERROR. The driver calls this routine from the motCpmEndStop() 
routine each time a unit is disabled.
.IP sysXxxEnetAddrGet()
.LP
This is either sys360EnetAddrGet() or sysCpmEnetAddrGet(), based on the 
actual host processor being used. See below for the actual prototypes.
The driver expects this routine to provide the six-byte Ethernet hardware 
address that is used by this unit.  This routine must copy the six-byte 
address to the space provided by <addr>.  This routine is expected to 
return OK on success, or ERROR.  The driver calls this routine, once per 
unit, from the motCpmEndLoad() routine.
.LP
In the case of the CPU32, the prototypes of the above mentioned support
routines are as follows:
.CS
    STATUS sys360EnetEnable (int unit, UINT32 regBase)
    void sys360EnetDisable (int unit, UINT32 regBase)
    STATUS sys360EnetAddrGet (int unit, u_char * addr)
.CE
.LP
In the case of the PPC860, the prototypes of the above mentioned support
routines are as follows:
.CS
    STATUS sysCpmEnetEnable (int unit)
    void sysCpmEnetDisable (int unit)
    STATUS sysCpmEnetAddrGet (int unit, UINT8 * addr)
.CE
.LP

SYSTEM RESOURCE USAGE
When implemented, this driver requires the following system resources:

    - one mutual exclusion semaphore
    - one interrupt vector
    - 0 bytes in the initialized data section (data)
    - 1272 bytes in the uninitialized data section (BSS)
 
The data and BSS sections are quoted for the CPU32 architecture and could
vary for other architectures.  The code size (text) varies greatly between
architectures, and is therefore not quoted here.

If the driver allocates the memory to share with the Ethernet device unit,
it does so by calling the cacheDmaMalloc() routine.  For the default case
of 32 transmit buffers, 32 receive buffers, and 16 loaner buffers (this is not
configurable), the total size requested is 121,600 bytes.  If a non-cacheable 
memory region is provided by the user, the size of this region should be this 
amount, unless the user has specified a different number of transmit or 
receive BDs.

This driver can operate only if this memory region is non-cacheable
or if the hardware implements bus snooping.  The driver cannot maintain
cache coherency for the device because the buffers are asynchronously
modified by both the driver and the device, and these fields might share 
the same cache line.  Additionally, the chip's dual-ported RAM must be
declared as non-cacheable memory where applicable (for example, when attached
to a 68040 processor). For more information, see the
.I "Motorola MC68EN360 User's Manual",
.I "Motorola MPC860 User's Manual",
.I "Motorola MPC821 User's Manual"

INTERNAL
This driver contains conditional compilation switch DEBUG.
If defined, adds debug output routines.  Output is further
selectable at run-time via the motCpmDebug global variable.
*/

/* includes */

#include "vxWorks.h"
#include "iv.h"
#include "taskLib.h"
#include "memLib.h"
#include "ioctl.h"
#include "net/mbuf.h"
#include "net/protosw.h"
#include "socket.h"
#include "errno.h"
#include "errnoLib.h"
#include "net/unixLib.h"
#include "net/route.h"
#include "net/if_subr.h"
#include "cacheLib.h"
#include "stdio.h"
#include "intLib.h"
#include "logLib.h"
#include "netLib.h"
#include "iosLib.h"
#include "drv/netif/netifDev.h"

#include "drv/end/motCpmEnd.h"

#include "net/if.h"
#include "netinet/in.h"
#include "netinet/in_systm.h"
#include "netinet/in_var.h"
#include "netinet/ip.h"
#include "netinet/if_ether.h"
#include "etherMultiLib.h"
#include "end.h"
#include "semLib.h"

#undef END_MACROS

#include "endLib.h"
#include "lstLib.h"
#include "netBufLib.h"

/* defines */

#define ENET_ADDR_SIZE	0x6	/* size of Ethernet src/dest addresses */
#define TX_BD_MIN	2	/* minimum number of Tx buffer descriptors */
#define TX_BD_MAX	128	/* maximum number of Tx buffer descriptors */
#define RX_BD_MIN	2	/* minimum number of Rx buffer descriptors */
#define TX_BD_DEFAULT	0x40	/* default number of Tx buffer descriptors */
#define RX_BD_DEFAULT	0x40	/* default number of Rx buffer descriptors */
#define FRAME_MAX	0x05ee	/* maximum frame size */
#define FRAME_MAX_AL	0x05f0	/* maximum frame size, 4 byte alligned */
#define FRAME_MIN	0x0040	/* minimum frame size */
#define LENGTH_MIN_FBUF	9	/* min. size of the first buffer in a frame */
#define SPEED		10000000/* ethernet speed */
#define L_POOL		0x80	/* number of Rx loaner buffers in pool */

/* A shortcut for getting the hardware address from the MIB II stuff. */

#define END_HADDR(pEnd) \
		((pEnd)->mib2Tbl.ifPhysAddress.phyAddress)

#define END_HADDR_LEN(pEnd) \
		((pEnd)->mib2Tbl.ifPhysAddress.addrLength)

#ifndef SYS_INT_CONNECT
#define SYS_INT_CONNECT(pFunc, arg, pRet)                         	    \
{                                                                           \
*pRet = (intConnect) (INUM_TO_IVEC (pDrvCtrl->ivec),			    \
		      (pFunc), (int) (arg));                         	    \
}
#endif /* SYS_INT_CONNECT */

#define MOT_CPM_DEBUG	/* debug support */

int motCpmDebug = 0;

#ifdef MOT_CPM_DEBUG
#define MOTCPMLOGMSG(x) \
	if (motCpmDebug) \
	    { \
	    logMsg x; \
	    }
#else
#define MOTCPMLOGMSG(x)
#endif /* MOT_CPM_DEBUG */

/* typedefs */

/* Ethernet header */

typedef struct free_args
    {
    void* arg1;
    void* arg2;
    } FREE_ARGS;

typedef struct end_ctrl	/* END_CTRL */
    {
    END_OBJ		endObject;	/* base class */
    UINT32		regBase;	/* register/DPR base address */
    SCC_ETHER_DEV	ether;		/* ETHERNET SCC device */
    int			txBdIndexC;	/* current tx buffer descriptor index */
    UINT32		bufBase;	/* address of memory pool; */
					/* NONE = malloc it */
    int			unit;		/* unit number */
    int			ivec;		/* interrupt vector */
    BOOL		polling;	/* polling mode */
    BOOL		txCleaning;	/* cleaning queue */
    u_char *		txBuf;		/* transmit buffer */
    u_char *		rxBuf;		/* receive buffer */
    u_char *            txPoolBuf;      /* pool send buffer */
    BOOL		txStop;		/* emergency stop output */
    FUNCPTR		freeRtn[TX_BD_MAX];	/* Array of free routines. */
    FREE_ARGS		freeData[TX_BD_MAX];	/* Array of free arguments */
    CL_POOL_ID  	pClPoolId;      /* for the free routines. */
    BOOL		txBlocked;	/* flag for implementing flow control */
    } END_CTRL;

typedef struct {
    int		unit;		/* unit number */
    UINT32 	motCpmAddr;	/* base address of processor internal mem */
    int		ivec;		/* interrupt vector */
    int		sccNum;		/* Serial Communication Controler number */
    int		txBdNum;	/* number of transmit buffer descriptors */
    int		rxBdNum;	/* number of receive buffer descriptors */
    UINT32	txBdBase;	/* transmit buffer descriptor offset */
    UINT32	rxBdBase;	/* receive buffer descriptor offset */
    UINT32	bufBase;	/* address of memory pool; NONE = malloc it */
    } END_PARM;

/* globals */

/* locals */

M_CL_CONFIG motMclBlkConfig = 	/* network mbuf configuration table */
    {
    /* 
    no. mBlks		no. clBlks	memArea		memSize
    -----------		----------	-------		-------
    */
    0, 			0, 		NULL, 		0
    };

CL_DESC motClDescTbl [] = 	/* network cluster pool configuration table */
    {
    /* 
    clusterSize		num		memArea		memSize
    -----------		----		-------		-------
    */
    {FRAME_MAX_AL,	0,		NULL,		0}
    }; 

int motClDescTblNumEnt = (NELEMENTS(motClDescTbl));

NET_POOL motCpmNetPool;

/* forward declarations */

#ifdef __STDC__

LOCAL STATUS    motCpmInitParse (END_PARM *pEndParm, char *initString);
LOCAL STATUS    motCpmInitMem (END_CTRL *pDrvCtrl);
LOCAL void	motCpmRecv (END_CTRL *pDrvCtrl, SCC_BUF * pRxBd);
LOCAL void      motCpmIntr (END_CTRL *pDrvCtrl);
LOCAL void 	motCpmReset (END_CTRL * pDrvCtrl);
LOCAL void 	motCpmMCastFilterSet (END_CTRL * pDrvCtrl, char * pAddress);
LOCAL void 	motCpmMCastConfig (END_CTRL * pDrvCtrl);
LOCAL void 	motCpmTxRestart (END_CTRL * pDrvCtrl);
LOCAL void 	motCpmHandleInt (END_CTRL * pDrvCtrl);
LOCAL void 	motCpmCleanTxBdQueue (END_CTRL * pDrvCtrl);
LOCAL STATUS 	motCpmRestart (END_CTRL * pDrvCtrl);
LOCAL void	motCpmCleanRxBd (END_CTRL * pDrvCtrl, SCC_BUF * pRxBd);


/* END Specific interfaces. */

END_OBJ *       motCpmEndLoad (char *initString);
LOCAL STATUS    motCpmEndStart(END_CTRL *pDrvCtrl);
LOCAL int       motCpmEndIoctl (END_CTRL *pDrvCtrl, int cmd, caddr_t data);
LOCAL STATUS    motCpmEndSend (END_CTRL *pDrvCtrl, M_BLK_ID pBuf);
LOCAL STATUS    motCpmEndMCastAddrAdd (END_CTRL *pDrvCtrl, char* pAddress);
LOCAL STATUS    motCpmEndMCastAddrDel (END_CTRL *pDrvCtrl, char* pAddress);
LOCAL STATUS    motCpmEndMCastAddrGet (END_CTRL *pDrvCtrl,
					MULTI_TABLE *pTable);
LOCAL STATUS    motCpmEndPollSend (END_CTRL *pDrvCtrl, M_BLK_ID pBuf);
LOCAL STATUS    motCpmEndPollReceive (END_CTRL *pDrvCtrl, M_BLK_ID pBuf);
LOCAL STATUS    motCpmEndPollStart (END_CTRL *pDrvCtrl);
LOCAL STATUS    motCpmEndPollStop (END_CTRL *pDrvCtrl);
LOCAL STATUS 	motCpmEndUnload (END_CTRL *pDrvCtrl);
LOCAL STATUS 	motCpmEndStop (END_CTRL *pDrvCtrl);

#else  /* __STDC__ */

LOCAL STATUS    motCpmInitParse ();
LOCAL STATUS    motCpmInitMem ();
LOCAL void	motCpmRecv ();
LOCAL void      motCpmIntr ();
LOCAL void 	motCpmReset ();
LOCAL void 	motCpmMCastFilterSet ();
LOCAL void 	motCpmMCastConfig ();
LOCAL void 	motCpmTxRestart ();
LOCAL void 	motCpmHandleInt ();
LOCAL void 	motCpmCleanTxBdQueue ();
LOCAL STATUS 	motCpmRestart ();
LOCAL void	motCpmCleanRxBd ();

/* END Specific interfaces. */

END_OBJ *       motCpmEndLoad ();
LOCAL STATUS    motCpmEndStart ();
LOCAL int       motCpmEndIoctl ();
LOCAL STATUS    motCpmEndSend ();
LOCAL STATUS    motCpmEndMCastAddrAdd ();
LOCAL STATUS    motCpmEndMCastAddrDel ();
LOCAL STATUS    motCpmEndMCastAddrGet ();
LOCAL STATUS    motCpmEndPollSend ();
LOCAL STATUS    motCpmEndPollReceive ();
LOCAL STATUS    motCpmEndPollStart ();
LOCAL STATUS    motCpmEndPollStop ();
LOCAL STATUS 	motCpmEndUnload ();
LOCAL STATUS 	motCpmEndStop ();

#endif  /* __STDC__ */

/*
 * Define the device function table.  This is static across all driver
 * instances.
 */

LOCAL NET_FUNCS netFuncs =
    {
    (FUNCPTR)motCpmEndStart,		/* start func. */
    (FUNCPTR)motCpmEndStop,		/* stop func. */
    (FUNCPTR)motCpmEndUnload,		/* unload func. */
    (FUNCPTR)motCpmEndIoctl,		/* ioctl func. */
    (FUNCPTR)motCpmEndSend,		/* send func. */
    (FUNCPTR)motCpmEndMCastAddrAdd,	/* multicast add func. */
    (FUNCPTR)motCpmEndMCastAddrDel,	/* multicast delete func. */
    (FUNCPTR)motCpmEndMCastAddrGet,	/* multicast get fun. */
    (FUNCPTR)motCpmEndPollSend,		/* polling send func. */
    (FUNCPTR)motCpmEndPollReceive,	/* polling receive func.  */
    endEtherAddressForm,          	/* Put address info into a packet.  */
    endEtherPacketDataGet,        	/* Get a pointer to packet data. */
    endEtherPacketAddrGet         	/* Get packet addresses. */
    };

/*******************************************************************************
*
* motCpmEndLoad - initialize the driver and device
*
* This routine initializes the driver and the device to the operational state.
* All of the device specific parameters are passed in the <initString>, which
* is of the following format:
*
* <unit>:<motCpmAddr>:<ivec>:<sccNum>:<txBdNum>:<rxBdNum>:<txBdBase>:<rxBdBase>:<bufBase> 
* 
* The parameters of this string are individually described in the 'motCpmEnd' 
* man page.
*
* The SCC shares a region of memory with the driver.  The caller of 
* this routine can specify the address of a non-cacheable memory region with
* <bufBase>.  Or, if this parameter is "NONE", the driver obtains this
* memory region by making calls to cacheDmaMalloc().  Non-cacheable memory 
* space is important whenever the host processor uses cache memory.
* This is also the case when the MC68EN360 is operating in companion
* mode and is attached to a processor with cache memory.
*
* After non-cacheable memory is obtained, this routine divides up the
* memory between the various buffer descriptors (BDs).  The number
* of BDs can be specified by <txBdNum> and <rxBdNum>, or if "NULL", a
* default value of 32 BDs will be used.  An additional number of buffers are
* reserved as receive loaner buffers.  The number of loaner buffers is
* a default number of 16.
*
* The user must specify the location of the transmit and receive BDs in
* the processor's dual ported RAM.  <txBdBase> and <rxBdBase> give the
* offsets from <motCpmAddr> for the base of the BD rings.  Each BD uses 
* 8 bytes. Care must be taken so that the specified locations for Ethernet 
* BDs do not conflict with other dual ported RAM structures.
*
* Multiple individual device units are supported by this driver.  Device
* units can reside on different chips, or could be on different SCCs
* within a single processor. The <sccNum> parameter is used to explicitly
* state which SCC is being used. SCC1 is most commonly used, thus this
* parameter most often equals "1".
*
* Before this routine returns, it connects up the interrupt vector <ivec>.
*
* RETURNS: An END object pointer or NULL on error.
*
* SEE ALSO:
* .I "Motorola MC68EN360 User's Manual",
* .I "Motorola MPC860 User's Manual",
* .I "Motorola MPC821 User's Manual"
*/

END_OBJ *motCpmEndLoad
    (
    char *      initString      /* parameter string */
    )
    {
    END_PARM    endParm;        /* parameters from initString */
    END_CTRL    *pDrvCtrl;	/* pointer to END_CTRL structure */
    int         scc;
    UINT32	motCpmAddr;
    u_char	enetAddr[ENET_ADDR_SIZE];
    int         retVal;

    MOTCPMLOGMSG(("motCpmEndLoad \n", 0, 0, 0, 0, 0, 0));

    if (initString == NULL)
        return (NULL);
    
    if (initString[0] == 0)
        {
        bcopy((char *)MOT_DEV_NAME, initString, MOT_DEV_NAME_LEN);
        return (0);
        }

    /* Parse InitString */

    if (motCpmInitParse (&endParm, initString) == ERROR)
	return (NULL);

    /* Sanity check -- unit number */

    if (endParm.unit < 0 || endParm.unit >= MAX_SCC_CHANNELS)
	return (NULL);

    /* Sanity check -- scc number */

    if ((endParm.sccNum < 1) || (endParm.sccNum > 4))
	return (NULL);

    if (endParm.txBdBase == endParm.rxBdBase)
	return (NULL);

    /* allocate the device structure */

    pDrvCtrl = (END_CTRL *)calloc (sizeof(END_CTRL), 1);

    if (pDrvCtrl == NULL)
	{
	MOTCPMLOGMSG(("motCpmEndLoad : Failed to allocate control structure\n", 
		       0, 0, 0, 0, 0, 0));
	return (NULL);
	}

    /* Check if we are already attached */

    if (pDrvCtrl->endObject.attached == TRUE)
	return (&pDrvCtrl->endObject);

    /* use default number of buffer descriptors if user did not specify */

    if (endParm.txBdNum == 0)
	endParm.txBdNum = TX_BD_DEFAULT;
    if (endParm.rxBdNum == 0)
    	endParm.rxBdNum = RX_BD_DEFAULT;

    /* must be at least two transmit and receive buffer descriptors */

    endParm.txBdNum = max (TX_BD_MIN, endParm.txBdNum);
    endParm.rxBdNum = max (RX_BD_MIN, endParm.rxBdNum);

    /* Initialize parameters */

    pDrvCtrl->regBase		= endParm.motCpmAddr;
    pDrvCtrl->bufBase		= endParm.bufBase;
    pDrvCtrl->unit		= endParm.unit;
    pDrvCtrl->ivec		= endParm.ivec;
    pDrvCtrl->ether.sccNum	= endParm.sccNum;
    pDrvCtrl->ether.txBdNum	= endParm.txBdNum;
    pDrvCtrl->ether.rxBdNum	= endParm.rxBdNum;
    pDrvCtrl->ether.txBdBase	= (SCC_BUF *) (endParm.motCpmAddr + 
						(endParm.txBdBase & 0xffff));
    pDrvCtrl->ether.rxBdBase	= (SCC_BUF *) (endParm.motCpmAddr + 
						(endParm.rxBdBase & 0xffff));
    pDrvCtrl->ether.txBufSize	= FRAME_MAX_AL;
    pDrvCtrl->ether.rxBufSize	= FRAME_MAX_AL;
    pDrvCtrl->txBdIndexC	= 0;

    /* derive SCC dependent variables */

    scc = pDrvCtrl->ether.sccNum - 1;

    motCpmAddr = pDrvCtrl->regBase;

    pDrvCtrl->ether.pScc    = (SCC *) ((UINT32) CPM_DPR_SCC1(motCpmAddr) +
				       (scc * 0x100));
    pDrvCtrl->ether.pSccReg = (SCC_REG *) 
				((UINT32) CPM_GSMR_L1(motCpmAddr) + (
				 scc * 0x20));
    pDrvCtrl->ether.intMask = CPM_CIMR_SCC4 << (3 - scc);

    /* endObject initializations */

    if (END_OBJ_INIT (&pDrvCtrl->endObject, (void *)pDrvCtrl, MOT_DEV_NAME,
                      pDrvCtrl->unit, &netFuncs,
                      END_OBJ_STRING) == ERROR)
	return (NULL);

    /* memory initialization */

    if (motCpmInitMem (pDrvCtrl) == ERROR)
	return (NULL);

    /* reset the chip */

    motCpmReset (pDrvCtrl);

    /* connect the interrupt handler motCpmIntr() */

    SYS_INT_CONNECT ((VOIDFUNCPTR) motCpmIntr, 
		     (int) pDrvCtrl, &retVal);
    if (retVal == ERROR)
        return (NULL);
 
    /* Get our enet addr */

    SYS_ENET_ADDR_GET (enetAddr);

    MOTCPMLOGMSG(("motCpmEndLoad ADDR: 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x \n", 
		(int) enetAddr[0], (int) enetAddr[1], (int) enetAddr[2], 
		(int) enetAddr[3], (int) enetAddr[4], (int) enetAddr[5]));

    /* Initialize MIB2 entries */

    if (END_MIB_INIT (&pDrvCtrl->endObject, M2_ifType_ethernet_csmacd,
                      enetAddr, 6, ETHERMTU,
			SPEED) == ERROR)
	return (NULL);

    /* Mark the device ready */
    /* IFF_SCAT is not defined by default */

    END_OBJ_READY (&pDrvCtrl->endObject,
		 IFF_NOTRAILERS | IFF_BROADCAST | IFF_MULTICAST);

    /* Successful return */

    return (&pDrvCtrl->endObject);
    }

/*******************************************************************************
*
* motCpmInitParse - parse parameter values from initString
*
* Parse the input string.  Fill in values in the driver control structure.
*
* The initialization string format is:
* .CS
*    "unit:motCpmAddr:ivec:sccNum:txBdNum:rxBdNum:txBdBase:rxBdBase:bufBase"
* .CE
*
* .IP <unit>
* Device unit number, a small integer.
* .IP <motCpmAddr>
* base address of processor internal mem 
* .IP <ivec>
* Interrupt vector number
* .IP <sccNum>
* SCC number used.
* .IP <txBdNum>
* number of transmit buffer descriptors; NULL = default	
* .IP <rxBdNum>
* number of receive buffer descriptors; NULL = default
* .IP <txBdBase>
* transmit buffer descriptor offset
* .IP <rxBdBase>
* receive buffer descriptor offset
* .IP <bufBase>
* address of memory pool; NONE = malloc it
* .LP
*
* RETURNS: OK or ERROR for invalid arguments.
*/

LOCAL STATUS motCpmInitParse
    (
    END_PARM    *pEndParm,	/* structure to fill in with parameters */
    char        *initString	/* init string */
    )
    {
    char *      tok;            /* an initString token */
    char *	holder=NULL;    /* points to initString fragment beyond tok */
    char * 	initStringBuff;

    initStringBuff = malloc(strlen(initString) + 1);
    if (initStringBuff == NULL)
        return (ERROR);

    strcpy (initStringBuff, initString);

    tok = strtok_r(initStringBuff, ":", &holder);
    if (tok == NULL)
        {
        free(initStringBuff);
	return ERROR;
        }
    pEndParm->unit = atoi(tok);

    tok = strtok_r(NULL, ":", &holder);
    if (tok == NULL)
        {
        free(initStringBuff);
	return ERROR;
        }
    pEndParm->motCpmAddr = strtoul (tok, NULL, 16);

    tok = strtok_r(NULL, ":", &holder);
    if (tok == NULL)
        {
        free(initStringBuff);
	return ERROR;
        }
    pEndParm->ivec = strtoul (tok, NULL, 16);

    tok = strtok_r(NULL, ":", &holder);
    if (tok == NULL)
        {
        free(initStringBuff);
	return ERROR;
        }
    pEndParm->sccNum = atoi (tok);

    tok = strtok_r(NULL, ":", &holder);
    if (tok == NULL)
        {
        free(initStringBuff);
	return ERROR;
        }
    pEndParm->txBdNum = strtoul (tok, NULL, 16);

    tok = strtok_r(NULL, ":", &holder);
    if (tok == NULL)
        {
        free(initStringBuff);
	return ERROR;
        }
    pEndParm->rxBdNum = strtoul (tok, NULL, 16);

    tok = strtok_r(NULL, ":", &holder);
    if (tok == NULL)
        {
        free(initStringBuff);
	return ERROR;
        }
    pEndParm->txBdBase = strtoul (tok, NULL, 16);

    tok = strtok_r(NULL, ":", &holder);
    if (tok == NULL)
        {
        free(initStringBuff);
	return ERROR;
        }
    pEndParm->rxBdBase = strtoul (tok, NULL, 16);

    tok = strtok_r(NULL, ":", &holder);
    if (tok == NULL)
        {
        free(initStringBuff);
	return ERROR;
        }
    pEndParm->bufBase = strtoul (tok, NULL, 16);

    MOTCPMLOGMSG(("unit = %d, motCpmAddr = 0x%x, ivec = 0x%x, sccNum = 0x%x\n",
		pEndParm->unit, pEndParm->motCpmAddr, pEndParm->ivec, 
		pEndParm->sccNum, 0, 0));
    MOTCPMLOGMSG(("txBdNum = 0x%x, rxBdNum = 0x%x, txBdBase = 0x%x\n",
		pEndParm->txBdNum, pEndParm->rxBdNum, pEndParm->txBdBase, 
		0, 0, 0));
    MOTCPMLOGMSG(("rxBdBase  = 0x%x, bufBase = %d\n", pEndParm->rxBdBase, 
		pEndParm->bufBase, 0, 0, 0, 0));

    free(initStringBuff);
    return OK;
    }

/*******************************************************************************
*
* motCpmInitMem - initialize memory 
*
* Using data in the control structure, setup and initialize the memory
* areas needed.  If the memory address is not already specified, then allocate
* cache safe memory.
*
* RETURNS: OK or ERROR.
*
*/

LOCAL STATUS motCpmInitMem
    (
    END_CTRL *	pDrvCtrl	/* pointer to END_CTRL structure */
    )
    {
    int		counter;
    FUNCPTR 	allocFunc;
    void *	pData;
    char*	pTmpBuf;
    int		numBuf;

    /* Set up the structures to allow us to free data after sending it. */

    for (counter = 0; counter < pDrvCtrl->ether.txBdNum; counter++)
	{
	pDrvCtrl->freeRtn[counter] = NULL;
	pDrvCtrl->freeData[counter].arg1 = NULL;
	pDrvCtrl->freeData[counter].arg2 = NULL;
	}

    if (pDrvCtrl->bufBase == NONE)
	{
	/* We must allocate memory for buffer descriptors */

	pData = NULL;
	allocFunc = (FUNCPTR) cacheDmaMalloc;
	}
    else
	{
	/* Memory for buffer descriptors is already allocated */

	pData = (void *) pDrvCtrl->bufBase;
	allocFunc = NULL;
	}

    /* 
     * calculate the number of buffers we need :
     * receive buffers + transmit buffers + loaner buffers (= L_POOL)
     */

    numBuf = pDrvCtrl->ether.rxBdNum + pDrvCtrl->ether.txBdNum + L_POOL;

    /* Use the MUX's memory manager to get our buffers. */

    pDrvCtrl->endObject.pNetPool = &motCpmNetPool;

    motClDescTbl[0].clNum 	= numBuf;
    motClDescTbl[0].memSize 	= (numBuf * (FRAME_MAX_AL + sizeof (long)));

    motMclBlkConfig.mBlkNum 	= 5 * numBuf;
    motMclBlkConfig.clBlkNum 	= 5 * numBuf;
    motMclBlkConfig.memSize 	= ((motMclBlkConfig.mBlkNum *
                                    (MSIZE + sizeof (long))) +
                                   (motMclBlkConfig.clBlkNum *
                                    (CL_BLK_SZ + sizeof (long))));

    if ((motMclBlkConfig.memArea = (char *) memalign (sizeof (long),
                                                      motMclBlkConfig.memSize))
        == NULL)
        return (ERROR);

    motClDescTbl[0].memArea = (char *) cacheDmaMalloc(motClDescTbl[0].memSize);
    if (motClDescTbl[0].memArea == NULL)
        {
        MOTCPMLOGMSG (( "system memory unavailable\n", 1, 2, 3, 4, 5, 6));
        return (ERROR);
        }

    if (netPoolInit(pDrvCtrl->endObject.pNetPool, &motMclBlkConfig,
                    &motClDescTbl[0], motClDescTblNumEnt, NULL) == ERROR)
                    
        {
        MOTCPMLOGMSG(("Could not init buffering\n", 1, 2, 3, 4, 5, 6));
        return (ERROR);
        }
    /* Store the cluster pool id as others need it later. */
    pDrvCtrl->pClPoolId = clPoolIdGet(pDrvCtrl->endObject.pNetPool,
                                      FRAME_MAX_AL, FALSE);
    for (counter = 0; counter < pDrvCtrl->ether.rxBdNum; counter++)
	{
        if ((pTmpBuf = (char *)netClusterGet(pDrvCtrl->endObject.pNetPool,
                                             pDrvCtrl->pClPoolId)) == NULL)
            {
            return (ERROR);
            }

        pDrvCtrl->ether.rxBdBase[counter].dataPointer = (u_char *) pTmpBuf;
	}
   
    pDrvCtrl->txPoolBuf = netClusterGet (pDrvCtrl->endObject.pNetPool, 
                                         pDrvCtrl->pClPoolId);

 
    return (OK);
    }

/*******************************************************************************
*
* motCpmEndStart - start the device 
*
* This function initializes the device and calls BSP functions to connect
* interrupts and start the device running in interrupt mode.
*
* The complement of this routine is motCpmEndStop().  Once a unit is reset by
* motCpmEndStop(), it may be re-initialized to a running state by this routine.
* 
* RETURNS: OK if successful, otherwise ERROR.
*/

LOCAL STATUS motCpmEndStart
    (
    END_CTRL *	pDrvCtrl	/* pointer to END_CTRL structure */
    )
    {
    int			counter;
    u_char *		enetAddr;
    ETHER_MULTI *	pCurr;

    MOTCPMLOGMSG(("motCpmEndStart \n", 0, 0, 0, 0, 0, 0));

    /* initialize flag(s) */

    pDrvCtrl->polling = FALSE;
    pDrvCtrl->txStop = FALSE;
    pDrvCtrl->txCleaning = FALSE;
    pDrvCtrl->txBlocked = FALSE;

    /* set up transmit buffer descriptors */

    pDrvCtrl->ether.pScc->param.tbase = (UINT16)
	((UINT32) pDrvCtrl->ether.txBdBase & 0xffff);
    pDrvCtrl->ether.pScc->param.tbptr = (UINT16)
	((UINT32) pDrvCtrl->ether.txBdBase & 0xffff);

    pDrvCtrl->ether.txBdNext  = 0;

    /* initialize each transmit buffer descriptor */
     
    for (counter = 0; counter < pDrvCtrl->ether.txBdNum; counter++)
        pDrvCtrl->ether.txBdBase[counter].statusMode = SCC_ETHER_TX_BD_I |
                                                       SCC_ETHER_TX_BD_PAD |
                                                       SCC_ETHER_TX_BD_L |
                                                       SCC_ETHER_TX_BD_TC;

    /* set the last BD to wrap to the first */

    pDrvCtrl->ether.txBdBase[(counter - 1)].statusMode |= SCC_ETHER_TX_BD_W;

    /* set up receive buffer descriptors */

    pDrvCtrl->ether.pScc->param.rbase = (UINT16)
	((UINT32) pDrvCtrl->ether.rxBdBase & 0xffff);
    pDrvCtrl->ether.pScc->param.rbptr = (UINT16)
	((UINT32) pDrvCtrl->ether.rxBdBase & 0xffff);

    pDrvCtrl->ether.rxBdNext  = 0;

    /* initialize each receive buffer descriptor */

    for (counter = 0; counter < pDrvCtrl->ether.rxBdNum; counter++)
        pDrvCtrl->ether.rxBdBase[counter].statusMode = SCC_ETHER_RX_BD_I |
        					       SCC_ETHER_RX_BD_E;

    /* set the last BD to wrap to the first */

    pDrvCtrl->ether.rxBdBase[(counter - 1)].statusMode |= SCC_ETHER_RX_BD_W;

    /* set SCC attributes to Ethernet mode */
 
    pDrvCtrl->ether.pSccReg->gsmrl    = SCC_GSMRL_ETHERNET | SCC_GSMRL_TPP_10 |
                                        SCC_GSMRL_TPL_48   | SCC_GSMRL_TCI;
    pDrvCtrl->ether.pSccReg->gsmrh    = 0x0;

    pDrvCtrl->ether.pSccReg->psmr     = SCC_ETHER_PSMR_CRC |
					SCC_ETHER_PSMR_NIB_22;

    if (END_FLAGS_GET(&pDrvCtrl->endObject) & IFF_PROMISC)
        pDrvCtrl->ether.pSccReg->psmr |= SCC_ETHER_PSMR_PRO;
 
    pDrvCtrl->ether.pSccReg->dsr      = 0xd555;
    pDrvCtrl->ether.pScc->param.rfcr  = 0x18;      /* supervisor data access */
    pDrvCtrl->ether.pScc->param.tfcr  = 0x18;      /* supervisor data access */
    pDrvCtrl->ether.pScc->param.mrblr = FRAME_MAX_AL;  /* max rx buffer size */

    /* initialize parameter the SCC RAM */
 
    ((SCC_ETHER_PROTO *)pDrvCtrl->ether.pScc->prot)->c_pres	= 0xffffffff;
    ((SCC_ETHER_PROTO *)pDrvCtrl->ether.pScc->prot)->c_mask	= 0xdebb20e3;
    ((SCC_ETHER_PROTO *)pDrvCtrl->ether.pScc->prot)->crcec	= 0x00000000;
    ((SCC_ETHER_PROTO *)pDrvCtrl->ether.pScc->prot)->alec	= 0x00000000;
    ((SCC_ETHER_PROTO *)pDrvCtrl->ether.pScc->prot)->disfc	= 0x00000000;
    ((SCC_ETHER_PROTO *)pDrvCtrl->ether.pScc->prot)->pads	= 0x8888;
    ((SCC_ETHER_PROTO *)pDrvCtrl->ether.pScc->prot)->ret_lim	= 0x000f;
    ((SCC_ETHER_PROTO *)pDrvCtrl->ether.pScc->prot)->mflr	= FRAME_MAX;
    ((SCC_ETHER_PROTO *)pDrvCtrl->ether.pScc->prot)->minflr	= FRAME_MIN;
    ((SCC_ETHER_PROTO *)pDrvCtrl->ether.pScc->prot)->maxd1	= FRAME_MAX_AL;
    ((SCC_ETHER_PROTO *)pDrvCtrl->ether.pScc->prot)->maxd2	= FRAME_MAX_AL;
    ((SCC_ETHER_PROTO *)pDrvCtrl->ether.pScc->prot)->gaddr1	= 0x0000;
    ((SCC_ETHER_PROTO *)pDrvCtrl->ether.pScc->prot)->gaddr2	= 0x0000;
    ((SCC_ETHER_PROTO *)pDrvCtrl->ether.pScc->prot)->gaddr3	= 0x0000;
    ((SCC_ETHER_PROTO *)pDrvCtrl->ether.pScc->prot)->gaddr4	= 0x0000;
    ((SCC_ETHER_PROTO *)pDrvCtrl->ether.pScc->prot)->p_per	= 0x0000;
    ((SCC_ETHER_PROTO *)pDrvCtrl->ether.pScc->prot)->iaddr1	= 0x0000;
    ((SCC_ETHER_PROTO *)pDrvCtrl->ether.pScc->prot)->iaddr2	= 0x0000;
    ((SCC_ETHER_PROTO *)pDrvCtrl->ether.pScc->prot)->iaddr3	= 0x0000;
    ((SCC_ETHER_PROTO *)pDrvCtrl->ether.pScc->prot)->iaddr4	= 0x0000;
    ((SCC_ETHER_PROTO *)pDrvCtrl->ether.pScc->prot)->taddr_h	= 0x0000;
    ((SCC_ETHER_PROTO *)pDrvCtrl->ether.pScc->prot)->taddr_m	= 0x0000;
    ((SCC_ETHER_PROTO *)pDrvCtrl->ether.pScc->prot)->taddr_l	= 0x0000;

    /* set the hardware Ethernet address of the board */

    enetAddr = END_HADDR(&pDrvCtrl->endObject);

    ((SCC_ETHER_PROTO *)pDrvCtrl->ether.pScc->prot)->paddr1_h =
        (enetAddr[5] << 8) + enetAddr[4];
    ((SCC_ETHER_PROTO *)pDrvCtrl->ether.pScc->prot)->paddr1_m =
        (enetAddr[3] << 8) + enetAddr[2];
    ((SCC_ETHER_PROTO *)pDrvCtrl->ether.pScc->prot)->paddr1_l =
        (enetAddr[1] << 8) + enetAddr[0];

    /* Set the multicast addresses */

    for (pCurr = END_MULTI_LST_FIRST(&pDrvCtrl->endObject); pCurr != NULL;
			pCurr = (ETHER_MULTI *)lstNext(&pCurr->node))
	{
	/* add multicast address */

	motCpmMCastFilterSet (pDrvCtrl, pCurr->addr);
	}

    /* enable Ethernet interrupts */

    pDrvCtrl->ether.pSccReg->scce = 0xffff;		/* clr events */

    /* allow receive and transmit errors interrupts */

    pDrvCtrl->ether.pSccReg->sccm = SCC_ETHER_SCCX_RXF | SCC_ETHER_SCCX_TXE |
				    SCC_ETHER_SCCX_TXB;

    *CPM_CIMR(pDrvCtrl->regBase) |= pDrvCtrl->ether.intMask;

    /* call the BSP to do any other initialization (e.g., connecting clocks) */

    SYS_ENET_ENABLE;

    /* raise the interface flags - mark the device as up */

    END_FLAGS_SET (&pDrvCtrl->endObject, IFF_UP | IFF_RUNNING);

    /* enable the transmitter */

    pDrvCtrl->ether.pSccReg->gsmrl |= SCC_GSMRL_ENT;

    /* issue the restart transmitter command to the CP */

    while (*CPM_CPCR(pDrvCtrl->regBase) & CPM_CR_FLG);
    *CPM_CPCR(pDrvCtrl->regBase) = ((pDrvCtrl->ether.sccNum - 1) << 6) |
                                      CPM_CR_SCC_RESTART | CPM_CR_FLG;
    while (*CPM_CPCR(pDrvCtrl->regBase) & CPM_CR_FLG);

    /* enable the receiver */

    pDrvCtrl->ether.pSccReg->gsmrl |= SCC_GSMRL_ENR;

    return (OK);
    }

/*******************************************************************************
*
* motCpmEndStop - stop the device 
*
* This routine marks the interface as down and resets the device.  This
* includes disabling interrupts, stopping the transmitter and receiver,
* and calling the bsp-specific LAN disable routine to do any target 
* specific disabling.
*
* The complement of this routine is motCpmEndStart().  Once a unit is 
* stop in this routine, it may be re-initialized to a running state by 
* motCpmEndStart().
*
* RETURNS: OK or ERROR.
*/

LOCAL STATUS motCpmEndStop
    (
    END_CTRL *	pDrvCtrl	/* pointer to END_CTRL structure */
    )
    {
    /* mark the driver as down */

    END_FLAGS_CLR (&pDrvCtrl->endObject, IFF_UP | IFF_RUNNING);

    /* Reset the device */

    motCpmReset (pDrvCtrl);

    return (OK);
    }

/******************************************************************************
*
* motCpmEndUnload - unload a driver from the system
*
* This function first brings down the device, and then frees any
* stuff that was allocated by the driver in the load function.
*
* RETURN : OK or ERROR
*/

LOCAL STATUS motCpmEndUnload
    (
    END_CTRL	*pDrvCtrl	/* pointer to END_CTRL structure */
    )
    {
    END_OBJECT_UNLOAD (&pDrvCtrl->endObject);

    return (OK);
    }

/*******************************************************************************
*
* motCpmEndSend - output packet to network interface device
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
* RETURNS: OK or ERROR.
*/

LOCAL STATUS motCpmEndSend
    (
    END_CTRL	*pDrvCtrl,	/* pointer to END_CTRL structure */
    M_BLK_ID    pMblk	/* Data to send */
    )
    {
    int			length;
    SCC_BUF *		pTxBd;
    u_char *		pad;
    char *	        pBuf;
    int 		oldLevel;
    int                 s;
    
    MOTCPMLOGMSG(("motCpmEndSend \n", 0, 0, 0, 0, 0, 0));

    if (pDrvCtrl->txBlocked)
        return (END_ERR_BLOCK);
    
    if (pDrvCtrl->polling)
	{ 
        netMblkClChainFree (pMblk); /* free the given mBlk chain */
        errno = EINVAL;
        return (ERROR);
        }

    /* gain exclusive access to the transmitter */

    END_TX_SEM_TAKE (&pDrvCtrl->endObject, WAIT_FOREVER);

    /* get a free transmit frame descriptor */
    
    pTxBd = & pDrvCtrl->ether.txBdBase[pDrvCtrl->ether.txBdNext];
    
    /* check if a transmit buffer descriptor is available */
    
    if ((pTxBd->statusMode & SCC_ETHER_TX_BD_R) || 
        (((pDrvCtrl->ether.txBdNext + 1) % pDrvCtrl->ether.txBdNum) 
         == pDrvCtrl->txBdIndexC))
        {
        END_TX_SEM_GIVE (&pDrvCtrl->endObject);
        s = intLock();
        pDrvCtrl->txBlocked = TRUE;
        intUnlock(s);
        return (END_ERR_BLOCK);
        }
    
    /* fill the transmit frame descriptor */
    
    pBuf = netClusterGet(pDrvCtrl->endObject.pNetPool, pDrvCtrl->pClPoolId);

    if (pBuf == NULL)
        {
        END_TX_SEM_GIVE (&pDrvCtrl->endObject);
	netMblkClChainFree(pMblk);
        return (ERROR);
        }

    length = netMblkToBufCopy (pMblk, (char *)pBuf, NULL);
    netMblkClChainFree(pMblk); 

    pTxBd->dataPointer = (u_char *) pBuf;
    
    /* padding mechanism in Rev A is buggy - do in software */
    
    if (length < FRAME_MIN)
        {
        pad = pTxBd->dataPointer + length;
        for (; length != FRAME_MIN; length++, pad++)
            *pad = 0x88;
        }
    
    pTxBd->dataLength = length;
    
    oldLevel = intLock ();	/* disable ints during update */
    
    if (pTxBd->statusMode & SCC_ETHER_TX_BD_W)
        pTxBd->statusMode = SCC_ETHER_TX_BD_I | SCC_ETHER_TX_BD_PAD |
            SCC_ETHER_TX_BD_L | SCC_ETHER_TX_BD_TC  |
            SCC_ETHER_TX_BD_W | SCC_ETHER_TX_BD_R;
    else
        pTxBd->statusMode = SCC_ETHER_TX_BD_I | SCC_ETHER_TX_BD_PAD |
            SCC_ETHER_TX_BD_L | SCC_ETHER_TX_BD_TC  |
            SCC_ETHER_TX_BD_R;
    
    pDrvCtrl->freeRtn[pDrvCtrl->ether.txBdNext] = (FUNCPTR) netClFree;
    pDrvCtrl->freeData[pDrvCtrl->ether.txBdNext].arg1 =
        pDrvCtrl->endObject.pNetPool;
    pDrvCtrl->freeData[pDrvCtrl->ether.txBdNext].arg2 = pBuf;

    /* incr BD count */
    
    pDrvCtrl->ether.txBdNext = (pDrvCtrl->ether.txBdNext + 1) %
        pDrvCtrl->ether.txBdNum;
    
    /* Unlock interrupts */
    
    intUnlock (oldLevel);

    /* release semaphore */

    END_TX_SEM_GIVE (&pDrvCtrl->endObject);

    /* Bump the statistic counter. */

    END_ERR_ADD (&pDrvCtrl->endObject, MIB2_OUT_UCAST, +1);

    return (OK);
    }

/*******************************************************************************
*
* motCpmEndIoctl - network interface control routine
*
* This routine implements the network interface control functions.
* It handles EIOCSIFADDR, EIOCGADDR, EIOCSFLAGS, EIOCGFLAGS,
* EIOCPOLLSTART, EIOCPOLLSTOP, EIOCGMIB2 and EIOCGFBUF commands.
*
* RETURNS: OK if successful, otherwise EINVAL.
*/

LOCAL int motCpmEndIoctl
    (
    END_CTRL *	pDrvCtrl,	/* pointer to END_CTRL structure */
    int		cmd,		/* command to process */
    caddr_t	data		/* pointer to data */ 
    )
    {
    int         error   = 0;            /* error value */
    long 	value;
    END_OBJ *	pEndObj	= &pDrvCtrl->endObject;

    MOTCPMLOGMSG(("motCpmEndIoctl with command = 0x%x \n", cmd, 0, 0, 0, 0, 0));

    switch (cmd)
        {
	case EIOCSADDR:
	    if (data == NULL)
	    	error = EINVAL;
	    bcopy ((char *)data, (char *)END_HADDR(pEndObj),
			END_HADDR_LEN(pEndObj));
	    break;

        case EIOCGADDR:
	    if (data == NULL)
	    	error = EINVAL;
	     else
	     	bcopy ((char *)END_HADDR(pEndObj), (char *)data,
		 	END_HADDR_LEN(pEndObj));
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

	    /* set promisc bit off flags */

	    if (END_FLAGS_GET(pEndObj) & IFF_PROMISC)
		pDrvCtrl->ether.pSccReg->psmr |= SCC_ETHER_PSMR_PRO;
	    else
		pDrvCtrl->ether.pSccReg->psmr &= ~SCC_ETHER_PSMR_PRO;
	    break;

	case EIOCGFLAGS:
	    if (data == NULL)
		error = EINVAL;
	    else
	    	*(int *)data = END_FLAGS_GET(pEndObj);
	    break;

	case EIOCPOLLSTART:
	    error = motCpmEndPollStart (pDrvCtrl);
	    break;

	case EIOCPOLLSTOP:
	    error = motCpmEndPollStop (pDrvCtrl);
	    break;

	case EIOCGMIB2:
	    if (data == NULL)
	    	error = EINVAL;
	    else
		bcopy((char *)&pDrvCtrl->endObject.mib2Tbl, (char *)data,
			sizeof(pDrvCtrl->endObject.mib2Tbl));
	    break;

        case EIOCGFBUF:
	    if (data == NULL)
		error =  EINVAL;
	    else
		*(int *)data = LENGTH_MIN_FBUF;
	    break;

        default:
            error = EINVAL;
        }

    return (error);
    }

/*******************************************************************************
*
* motCpmEndMCastAddrAdd - add a multicast address for the device
*
* This routine adds a multicast address to whatever the driver
* is already listening for. 
*
* To add an address in the processor group address hash filter, we use
* the SET GROUP ADDRESS command. This command can be executed at any 
* time, regadless of whether the Ethernet channel is enabled.
*
* RETURNS : OK or ERROR
*/

LOCAL STATUS motCpmEndMCastAddrAdd
    (
    END_CTRL *  pDrvCtrl,	/* pointer to END_CTRL structure */
    char *      pAddr		/* Address to add to the table. */
    )
    {
    STATUS error;

    MOTCPMLOGMSG(("motCpmEndMCastAddrAdd %x:%x:%x:%x:%x:%x \n", 
		 pAddr[5], pAddr[4], pAddr[3], 
		 pAddr[2], pAddr[1], pAddr[0]));

    error = etherMultiAdd (&pDrvCtrl->endObject.multiList, pAddr);

    if (error == ENETRESET)
	{
	pDrvCtrl->endObject.nMulti++;

	/* Set the multicast address */

	motCpmMCastFilterSet (pDrvCtrl, pAddr);

	error = OK;
	}

    return ((error == OK) ? OK : ERROR);
    }

/*******************************************************************************
*
* motCpmEndMCastAddrDel - delete a multicast address for the device
*
* This routine deletes a multicast address from the current list of
* multicast addresses.
*
* RETURNS : OK or ERROR
*/

LOCAL STATUS motCpmEndMCastAddrDel
    (
    END_CTRL *  pDrvCtrl,	/* pointer to END_CTRL structure */
    char *      pAddr		/* Address to delete from the table. */
    )
    {
    STATUS error;

    MOTCPMLOGMSG(("motCpmEndMCastDel %x:%x:%x:%x:%x:%x \n", 
		 pAddr[5], pAddr[4], pAddr[3], 
		 pAddr[2], pAddr[1], pAddr[0]));

    error = etherMultiDel (&pDrvCtrl->endObject.multiList, pAddr);

    if (error == ENETRESET)
	{
	pDrvCtrl->endObject.nMulti--;
	motCpmMCastConfig (pDrvCtrl);
	error = OK;
	}

    return ((error == OK) ? OK : ERROR);
    }

/*******************************************************************************
*
* motCpmEndMCastAddrGet - get the current multicast address list
*
* This routine returns the current multicast address list in <pTable>
*
*/

LOCAL STATUS motCpmEndMCastAddrGet
    (
    END_CTRL *  pDrvCtrl,	/* pointer to END_CTRL structure */
    MULTI_TABLE *pTable		/* table to fill in with addresses */
    )
    {
    MOTCPMLOGMSG(("motCpmEndMCastAddrGet \n", 0, 0, 0, 0, 0, 0));

    return (etherMultiGet (&pDrvCtrl->endObject.multiList, pTable));
    }

/*******************************************************************************
*
* motCpmEndPollStart - start polling mode
*
* This routine starts polling mode by disabling ethernet interrupts and
* setting the polling flag in the END_CTRL stucture.
*
* It is necessary to empty transmit queue before entering polling mode
* because M_BLK_ID free routine used in interrupt mode could be unusable 
* in this mode (could use kernel calls). 
*
* RETURNS: OK or ERROR if already in polling mode.
*/

LOCAL STATUS motCpmEndPollStart
    (
    END_CTRL    *pDrvCtrl	/* pointer to END_CTRL structure */
    )
    {
    int 	intLevel;
    int		txBdIndex;
    SCC_BUF *	pTxBd;

    MOTCPMLOGMSG(("motCpmEndPollStart \n", 0, 0, 0, 0, 0, 0));

    /* Lock interrupts */

    intLevel = intLock();

    /* clean transmit queue */

    txBdIndex = pDrvCtrl->txBdIndexC;

    while (txBdIndex != pDrvCtrl->ether.txBdNext)
	{
	pTxBd = & pDrvCtrl->ether.txBdBase[txBdIndex];

	/* Spin until frame buffer is sent */

	while (pTxBd->statusMode & SCC_ETHER_TX_BD_R)
	    ;

	/* Check for transmit errors */

	if (pTxBd->statusMode & (SCC_ETHER_TX_BD_RL | SCC_ETHER_TX_BD_UN |
 				SCC_ETHER_TX_BD_CSL | SCC_ETHER_TX_BD_LC))
	    {
	    /* An error has occured, restart the transmitter */

	    pDrvCtrl->txStop = TRUE;

	    motCpmTxRestart (pDrvCtrl);
	    }

	/* increment txBdIndex */

	txBdIndex = (txBdIndex + 1) % pDrvCtrl->ether.txBdNum;
	}
    
    /* free all transmit buffer and update transmit queue */

    motCpmCleanTxBdQueue (pDrvCtrl);

    /* Now, transmit queue is empty. We can enter polling mode. */
    /* mask off the receive and transmit interrupts */
    
    pDrvCtrl->ether.pSccReg->sccm = 0;

    /* Set the polling flag */

    pDrvCtrl->polling = TRUE;

    /* Unlock interrupts */

    intUnlock (intLevel);

    return (OK);
    }

/*******************************************************************************
*
* motCpmEndPollStop - stop polling mode
*
* This routine stops polling mode by enabling ethernet interrupts and
* resetting the polling flag in the END_CTRL structure.
*
* RETURNS: OK always
*/

LOCAL STATUS motCpmEndPollStop
    (
    END_CTRL    *pDrvCtrl	/* pointer to END_CTRL structure */
    )
    {
    int         intLevel;

    MOTCPMLOGMSG(("motCpmEndPollStop \n", 0, 0, 0, 0, 0, 0));

    /* lock interrupt  */

    intLevel = intLock();

    /* reset the SCC's interrupt status bit */

    *CPM_CISR(pDrvCtrl->regBase) = pDrvCtrl->ether.intMask;

    /* enable this SCC's interrupt */

    *CPM_CIMR(pDrvCtrl->regBase) |= pDrvCtrl->ether.intMask;

    /* reset the status bits */

    pDrvCtrl->ether.pSccReg->scce = 0xffff;

    /* enables the receive and transmit interrupts */

    pDrvCtrl->ether.pSccReg->sccm = SCC_ETHER_SCCX_RXF | SCC_ETHER_SCCX_TXE |
				    SCC_ETHER_SCCX_TXB;

    /* reset the polling flag */

    pDrvCtrl->polling = FALSE;

    /* unlock interrupt  */

    intUnlock (intLevel);

    return (OK);
    }

/******************************************************************************
*
* motCpmEndPollSend - transmit a packet in polled mode
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
* RETURNS: OK or EAGAIN
*/

LOCAL STATUS motCpmEndPollSend
    (
    END_CTRL    *pDrvCtrl,	/* pointer to END_CTRL structure */
    M_BLK_ID    pMblk	/* data to send */
    )
    {
    int		length;
    SCC_BUF *	pTxBd;
    u_char *	pad;
    char *	pBuf;

    MOTCPMLOGMSG(("motCpmEndPollSend \n", 0, 0, 0, 0, 0, 0));

    if (pDrvCtrl->txStop)
	return (ERROR);

    /* get a free transmit frame descriptor */

    pTxBd = & pDrvCtrl->ether.txBdBase[pDrvCtrl->ether.txBdNext];

    /* check if a transmit buffer descriptor is available */

    if ((pTxBd->statusMode & SCC_ETHER_TX_BD_R) ||
	(((pDrvCtrl->ether.txBdNext + 1) % pDrvCtrl->ether.txBdNum)
			== pDrvCtrl->txBdIndexC))
	{
	return (EAGAIN);
	}

    /* fill the transmit frame descriptor */
    
    pBuf = pDrvCtrl->txPoolBuf;   
 
    length = netMblkToBufCopy (pMblk, (char *)pBuf, NULL);

    pTxBd->dataPointer = (u_char *) pBuf;

    /* padding mechanism in Rev A is buggy - do in software */

    if (length < FRAME_MIN)
	{
	pad = pTxBd->dataPointer + length;
	for (; length != FRAME_MIN; length++, pad++)
	    *pad = 0x88;
	}

    pTxBd->dataLength = length;

    if (pTxBd->statusMode & SCC_ETHER_TX_BD_W)
	pTxBd->statusMode = SCC_ETHER_TX_BD_I | SCC_ETHER_TX_BD_PAD |
			    SCC_ETHER_TX_BD_L | SCC_ETHER_TX_BD_TC  |
			    SCC_ETHER_TX_BD_W | SCC_ETHER_TX_BD_R;
    else
	pTxBd->statusMode = SCC_ETHER_TX_BD_I | SCC_ETHER_TX_BD_PAD |
			    SCC_ETHER_TX_BD_L | SCC_ETHER_TX_BD_TC  |
			    SCC_ETHER_TX_BD_R;

    /* incr BD count */

    pDrvCtrl->ether.txBdNext = (pDrvCtrl->ether.txBdNext + 1) %
				pDrvCtrl->ether.txBdNum;

    /* Bump the statistic counter. */

    END_ERR_ADD (&pDrvCtrl->endObject, MIB2_OUT_UCAST, +1);

    /*
     * Spin until we've sent it.
     */

    while (pTxBd->statusMode & SCC_ETHER_TX_BD_R)
	;

    if (pTxBd->statusMode & (SCC_ETHER_TX_BD_RL | SCC_ETHER_TX_BD_UN |
			    SCC_ETHER_TX_BD_CSL | SCC_ETHER_TX_BD_LC))
	{
	/* An error has occured, restart the transmitter */

	pDrvCtrl->txStop = TRUE;

	motCpmTxRestart (pDrvCtrl);
	}

    /* 
     * we are allow to do this because transmit queue is empty when we
     * start polling mode.
     */

    pDrvCtrl->txBdIndexC = pDrvCtrl->ether.txBdNext;

    return (OK);
    }

/*******************************************************************************
*
* motCpmEndPollReceive - receive a packet in polled mode
*
* This routine is called by a user to try and get a packet from the
* device. It returns EAGAIN if no packet is available. The caller must
* supply a M_BLK_ID with enough space to contain the receiving packet. If
* enough buffer is not available then EAGAIN is returned.
*
* These routine should not call any kernel functions.
*
* RETURNS: OK or EAGAIN
*/

LOCAL STATUS motCpmEndPollReceive
    (
    END_CTRL    *pDrvCtrl,	/* pointer to END_CTRL structure */
    M_BLK_ID    pMblk
    )
    {
    SCC_BUF *	pRxBd = & pDrvCtrl->ether.rxBdBase[pDrvCtrl->ether.rxBdNext];
    int		length;
    int		status = EAGAIN;

    MOTCPMLOGMSG(("motCpmEndPollReceive \n", 0, 0, 0, 0, 0, 0));

    /* if we have not received packets, leave immediatly */

    if (pRxBd->statusMode & SCC_ETHER_RX_BD_E)
	return (EAGAIN);

    /* check packets for errors */

    if (((pRxBd->statusMode & (SCC_ETHER_RX_BD_F  | SCC_ETHER_RX_BD_L))
			   == (SCC_ETHER_RX_BD_F  | SCC_ETHER_RX_BD_L))
       && !(pRxBd->statusMode & (SCC_ETHER_RX_BD_CL |
				SCC_ETHER_RX_BD_OV | SCC_ETHER_RX_BD_CR |
				SCC_ETHER_RX_BD_SH | SCC_ETHER_RX_BD_NO |
				SCC_ETHER_RX_BD_LG)))
	{
	/* adjust length to data only */

	length = pRxBd->dataLength - 4;

	if ((length - SIZEOF_ETHERHEADER) <= 0)
	    {
	    /* bump input error packet counter */

	    END_ERR_ADD (&pDrvCtrl->endObject, MIB2_IN_ERRS, +1);
	    goto cleanRxBd;
	    }

	/* 
	 * Upper layer provides the buffer.
         * If buffer is not large enough, we return.
	 */

	/* copy data */

        if ((pMblk->mBlkHdr.mLen < length) ||
            (!(pMblk->mBlkHdr.mFlags & M_EXT)))
            {
            goto cleanRxBd;
            }
        
	bcopy ((char *) pRxBd->dataPointer, (char *)pMblk->mBlkHdr.mData,
               length);
	pMblk->mBlkHdr.mLen = length;
	pMblk->mBlkPktHdr.len = length;
	pMblk->mBlkHdr.mFlags |= M_PKTHDR;

	/* bump input packet counter */

	END_ERR_ADD (&pDrvCtrl->endObject, MIB2_IN_UCAST, +1);

	status = OK;
	}
    else
	{
	/* bump input error packet counter */

	END_ERR_ADD (&pDrvCtrl->endObject, MIB2_IN_ERRS, +1);
	}

cleanRxBd:
    motCpmCleanRxBd (pDrvCtrl, pRxBd);	/* reset buffer descriptor as empty */

    return (status);
    }

/*******************************************************************************
*
* motCpmIntr - network interface interrupt handler
*
* This routine gets called at interrupt level. It handles work that 
* requires minimal processing. Interrupt processing that is more 
* extensive gets handled at task level. The network task, netTask(), is 
* provided for this function. Routines get added to the netTask() work 
* queue via the netJobAdd() command.
*
* RETURNS: N/A
*/

LOCAL void motCpmIntr
    (
    END_CTRL *	pDrvCtrl	/* pointer to END_CTRL structure */
    )
    {
    BOOL rxHandle = FALSE;
    BOOL txbHandle = FALSE;
    
    /* check for spurious interrupt -> initialized ? */

    if (!pDrvCtrl->endObject.attached)
	{
        pDrvCtrl->ether.pSccReg->scce = 0xffff;

        *CPM_CISR(pDrvCtrl->regBase) = pDrvCtrl->ether.intMask;
	return;
	}

    /* handle receive events */

    if ((pDrvCtrl->ether.pSccReg->sccm & SCC_ETHER_SCCX_RXF) &&
        (pDrvCtrl->ether.pSccReg->scce & SCC_ETHER_SCCX_RXF))
	{
        (void) netJobAdd ((FUNCPTR) motCpmHandleInt, (int) pDrvCtrl, 
			  0, 0, 0, 0); 

	/* turn off receive interrupts for now - motCpmHandleIt turns back on */

        pDrvCtrl->ether.pSccReg->sccm &= ~SCC_ETHER_SCCX_RXF;
	rxHandle = TRUE;
        }

    /* check for output errors */

    if (pDrvCtrl->ether.pSccReg->scce & SCC_ETHER_SCCX_TXE)
	{
	/* clean the transmit buffer descriptor queue */
	/* NOTE: HBC error not supported -> always RESTART Tx here */

        (void) netJobAdd ((FUNCPTR) motCpmTxRestart, (int) pDrvCtrl, 
			  0, 0, 0, 0);
	pDrvCtrl->txStop = TRUE;
	}

    /* handle transmitter events - BD full condition -> ever happen ? */

    if ((pDrvCtrl->ether.pSccReg->sccm & SCC_ETHER_SCCX_TXB) &&
        (pDrvCtrl->ether.pSccReg->scce & SCC_ETHER_SCCX_TXB))
	{
	txbHandle = TRUE;
	}

    /* check for input busy condition */

    if (pDrvCtrl->ether.pSccReg->scce & SCC_ETHER_SCCX_BSY)
        pDrvCtrl->ether.pSccReg->scce = SCC_ETHER_SCCX_BSY; 

    /* acknowledge all other interrupts - ignore events */

    pDrvCtrl->ether.pSccReg->scce = (pDrvCtrl->ether.pSccReg->scce &
				    ~(SCC_ETHER_SCCX_RXF |
				      SCC_ETHER_SCCX_TXE |
				      SCC_ETHER_SCCX_TXB |
				      SCC_ETHER_SCCX_BSY));

    /* 
     * clean the transmit buffer descriptor queue if we have 
     * received a transmit interrupt and if we are not already
     * cleaning this transmit queue.
     */

    if ((pDrvCtrl->txStop || txbHandle) && !pDrvCtrl->txCleaning)
        {
    	motCpmCleanTxBdQueue (pDrvCtrl);
        if (pDrvCtrl->txBlocked)
            {
            pDrvCtrl->txBlocked = FALSE;
            (void) netJobAdd ((FUNCPTR) muxTxRestart,
                              (int)&pDrvCtrl->endObject,
                              0, 0, 0, 0);
            }
        }

    /* acknowledge interrupts */

    if (rxHandle)
        pDrvCtrl->ether.pSccReg->scce = SCC_ETHER_SCCX_RXF; 
    if (pDrvCtrl->txStop)
        pDrvCtrl->ether.pSccReg->scce = SCC_ETHER_SCCX_TXE; 
    if (txbHandle)
        pDrvCtrl->ether.pSccReg->scce = SCC_ETHER_SCCX_TXB; 

    *CPM_CISR(pDrvCtrl->regBase) = pDrvCtrl->ether.intMask;

    }

/*******************************************************************************
*
* motCpmMCastFilterSet - set the group addres filter for a multicast addresse.
*
* To add an address in the processor group address hash filter, we use
* the SET GROUP ADDRESS command. This command can be executed at any 
* time, regadless of whether the Ethernet channel is enabled.
*
* RETURNS : N/A 
*
*/

LOCAL void motCpmMCastFilterSet
    (
    END_CTRL *  pDrvCtrl,	/* pointer to END_CTRL structure */
    char *      pAddress        /* Address to delete from the table. */
    )
    {
    MOTCPMLOGMSG(("motCpmMCastFilterSet \n", 0, 0, 0, 0, 0, 0));

    /* add multicast address */

    ((SCC_ETHER_PROTO *)pDrvCtrl->ether.pScc->prot)->taddr_h =
					(pAddress[5] << 8) + pAddress[4];
    ((SCC_ETHER_PROTO *)pDrvCtrl->ether.pScc->prot)->taddr_m =
					(pAddress[3] << 8) + pAddress[2];
    ((SCC_ETHER_PROTO *)pDrvCtrl->ether.pScc->prot)->taddr_l =
					(pAddress[1] << 8) + pAddress[0];

    /* issue the set group address command to the CP */

    while (*CPM_CPCR(pDrvCtrl->regBase) & CPM_CR_FLG);

    *CPM_CPCR(pDrvCtrl->regBase) = ((pDrvCtrl->ether.sccNum - 1) << 6) |
				       CPM_CR_SCC_SET_GROUP |
				       CPM_CR_FLG;

    while (*CPM_CPCR(pDrvCtrl->regBase) & CPM_CR_FLG);
    }

/*******************************************************************************
*
* motCpmMCastConfig - reconfigure the interface under us.
*
* Reconfigure the interface changing the multicast interface list.
*
* In order to delete an address from the hash tabke, the Ethernet channel
* should be disabled, the hash table registers should be cleared, and 
* the SET GROUP ADDRESS command must be executed for the remaining 
* desired addresses. This is required because the hash table may have mapped
* multiple addresses to the same hash table bit.
*
* RETURNS : N/A 
*/

LOCAL void motCpmMCastConfig
    (
    END_CTRL *  pDrvCtrl	/* pointer to END_CTRL structure */
    )
    {
    ETHER_MULTI *	pCurr;

    MOTCPMLOGMSG(("motCpmMCastConfig \n",0,0,0,0,0,0));

    /* disable the ethernet channel */

    motCpmReset (pDrvCtrl);

    /* clear hash table group registers */

    ((SCC_ETHER_PROTO *)pDrvCtrl->ether.pScc->prot)->gaddr1     = 0x0000;
    ((SCC_ETHER_PROTO *)pDrvCtrl->ether.pScc->prot)->gaddr2     = 0x0000;
    ((SCC_ETHER_PROTO *)pDrvCtrl->ether.pScc->prot)->gaddr3     = 0x0000;
    ((SCC_ETHER_PROTO *)pDrvCtrl->ether.pScc->prot)->gaddr4     = 0x0000;

    /* restore remaining addresses */

    for (pCurr = END_MULTI_LST_FIRST(&pDrvCtrl->endObject); pCurr != NULL;
			pCurr = (ETHER_MULTI *)lstNext(&pCurr->node))
	{
	/* add multicast address */
	motCpmMCastFilterSet (pDrvCtrl, pCurr->addr);
	}

    /* restart the ethernet channel */

    motCpmRestart (pDrvCtrl);
    }

/*******************************************************************************
*
* motCpmRestart - network interface restart routine
*
* This routine restarts the device.  This includes enabling interrupts, 
* starting the transmitter and receiver, and calling the bsp-specific
* LAN enable routine to do any target specific enabling.
*
* This routine follows the instructions in MC68EN360/MPC821 Users's Manual :
* "Disabling the SCCs on the Fly"
*
* The complement of this routine is motCpmReset().  Once a unit is reset by
* motCpmReset(), it may be re-initialized to a running state by this routine.
*
* RETURNS: N/A
*/

LOCAL STATUS motCpmRestart
    (
    END_CTRL * 	pDrvCtrl	/* pointer to END_CTRL structure */
    )
    {
    /* enable Ethernet interrupts */

    *CPM_CIMR(pDrvCtrl->regBase) |= pDrvCtrl->ether.intMask;

    /* call the BSP to do any other initialization (e.g., connecting clocks) */

    SYS_ENET_ENABLE;

    /* enable the transmitter */

    pDrvCtrl->ether.pSccReg->gsmrl |= SCC_GSMRL_ENT;

    /* issue the restart transmitter command to the CP */

    while (*CPM_CPCR(pDrvCtrl->regBase) & CPM_CR_FLG);
    *CPM_CPCR(pDrvCtrl->regBase) = ((pDrvCtrl->ether.sccNum - 1) << 6) |
				      CPM_CR_SCC_RESTART |
				      CPM_CR_FLG;
    while (*CPM_CPCR(pDrvCtrl->regBase) & CPM_CR_FLG);

    /* issue the enter hunt mode command to the CP */

    while (*CPM_CPCR(pDrvCtrl->regBase) & CPM_CR_FLG);
    *CPM_CPCR(pDrvCtrl->regBase) = ((pDrvCtrl->ether.sccNum - 1) << 6) |
				      CPM_CR_SCC_HUNT |
				      CPM_CR_FLG;
    while (*CPM_CPCR(pDrvCtrl->regBase) & CPM_CR_FLG);

    /* enable the receiver */

    pDrvCtrl->ether.pSccReg->gsmrl |= SCC_GSMRL_ENR;

    return (OK);
    }

/*******************************************************************************
*
* motCpmReset - network interface reset routine
*
* This routine resets the device.  This includes disabling interrupts, 
* stopping the transmitter and receiver, and calling the bsp-specific
* LAN disable routine to do any target specific disabling.
*
* This routine follows the instructions in MC68EN360/MPC821 Users's Manual :
* "Disabling the SCCs on the Fly"
*
* The complements of this routine are motCpmEndStart() and motCpmRestart().  
* Once a unit is reset in this routine, it may be re-started with parameters
* reinitialized with motCpmEndStart() or re-started with current parameteres 
* with  motCpmRestart().
*
* RETURNS: N/A
*/

LOCAL void motCpmReset
    (
    END_CTRL * 	pDrvCtrl	/* pointer to END_CTRL structure */
    )
    {
    int counter = 0xffff;

    /* disable the SCC interrupts */

    *CPM_CIMR(pDrvCtrl->regBase) &= ~pDrvCtrl->ether.intMask;

    /* issue the CP graceful stop command to the transmitter if necessary */

    if (pDrvCtrl->ether.pSccReg->gsmrl & SCC_GSMRL_ENT)
	{
        while (*CPM_CPCR(pDrvCtrl->regBase) & CPM_CR_FLG);
        *CPM_CPCR(pDrvCtrl->regBase) = ((pDrvCtrl->ether.sccNum - 1) << 6) |
                                          CPM_CR_SCC_GRSTOP | CPM_CR_FLG;
        while (*CPM_CPCR(pDrvCtrl->regBase) & CPM_CR_FLG);

        /* wait for graceful stop to register */

        while ((counter--) && (!(pDrvCtrl->ether.pSccReg->scce &
			         SCC_ETHER_SCCX_GRA)));
	}

    /* disable the SCC receiver and transmitter */

    pDrvCtrl->ether.pSccReg->gsmrl &= ~(SCC_GSMRL_ENR | SCC_GSMRL_ENT);

    /* call the BSP to do any other disabling (e.g., *TENA) */

    SYS_ENET_DISABLE;

    }

/*******************************************************************************
*
* motCpmTxRestart - issue RESTART Tx command to the CP
*
* This routine issues a RESTART transmitter command to the CP.  It is
* executed by netTask (motCpmIntr() did a netJobAdd).  motCpmIntr() cannot do
* a RESTART directly because the CPM flag must be taken before a command
* can be written.  
*
* RETURNS: N/A
*/

LOCAL void motCpmTxRestart
    (
    END_CTRL *	pDrvCtrl	/* pointer to END_CTRL structure */
    )
    {

    MOTCPMLOGMSG(("motCpmTxRestart \n", 0, 0, 0, 0, 0, 0));

    /* update error counter */

    END_ERR_ADD (&pDrvCtrl->endObject, MIB2_OUT_ERRS, +1);
    END_ERR_ADD (&pDrvCtrl->endObject, MIB2_OUT_UCAST, -1);

    /* restart transmitter */

    while (*CPM_CPCR(pDrvCtrl->regBase) & CPM_CR_FLG);
    *CPM_CPCR(pDrvCtrl->regBase) = ((pDrvCtrl->ether.sccNum - 1) << 6) |
                                      CPM_CR_SCC_RESTART | CPM_CR_FLG;
    while (*CPM_CPCR(pDrvCtrl->regBase) & CPM_CR_FLG);

    pDrvCtrl->txStop = FALSE;
    }

/*******************************************************************************
*
* motCpmHandleInt - task-level interrupt handler
*
* This is the task-level interrupt handler, which is called from 
* netTask(). motCpmIntr() gets input frames from the device and then calls 
* motCpmIntr() to process each frame. motCpmIntr() only gets called if no error
* stats were reported in the buffer descriptor.  Data chaining is not
* supported.
*
* This routine should be called with SCC receive interrupts masked so that
* more netJobAdds of this routine are not performed by motCpmIntr().
* Receive interrupts are turned back on by this routine before exiting.
*
* RETURNS: N/A
*/

LOCAL void motCpmHandleInt
    (
    END_CTRL *	pDrvCtrl	/* pointer to END_CTRL structure */
    )
    {
    SCC_BUF *	pRxBd = & pDrvCtrl->ether.rxBdBase[pDrvCtrl->ether.rxBdNext];

    while (!(pRxBd->statusMode & SCC_ETHER_RX_BD_E))
        {
        /* data chaining is not supported  - check all error conditions */

        if (((pRxBd->statusMode & (SCC_ETHER_RX_BD_F  | SCC_ETHER_RX_BD_L))
	                       == (SCC_ETHER_RX_BD_F  | SCC_ETHER_RX_BD_L))
	                      && !(pRxBd->statusMode & (SCC_ETHER_RX_BD_CL |
		                   SCC_ETHER_RX_BD_OV | SCC_ETHER_RX_BD_CR |
				   SCC_ETHER_RX_BD_SH | SCC_ETHER_RX_BD_NO |
				   SCC_ETHER_RX_BD_LG)))
            motCpmRecv (pDrvCtrl, pRxBd);
        else
	    {
            END_ERR_ADD (&pDrvCtrl->endObject, MIB2_IN_ERRS, +1);

	    /* reset buffer descriptor as empty */

	    motCpmCleanRxBd (pDrvCtrl, pRxBd);
	    }

        /* update receive buffer descriptor pointer */

        pRxBd = & pDrvCtrl->ether.rxBdBase[pDrvCtrl->ether.rxBdNext];

        /* clear Rx events */

        pDrvCtrl->ether.pSccReg->scce = SCC_ETHER_SCCX_RXF;
        }

    /* re-enable Rx interrupts */

    pDrvCtrl->ether.pSccReg->sccm |= SCC_ETHER_SCCX_RXF;
    }

/*******************************************************************************
*
* motCpmRecv - process an input frame
*
* This routine processes an input frame, then passes it up to the higher 
* level in a form it expects.  Buffer loaning, promiscuous mode are all 
* supported.  Trailer protocols is not supported.
*
* RETURNS: N/A
*/

LOCAL void motCpmRecv
    (
    END_CTRL *	pDrvCtrl,	/* pointer to END_CTRL structure */
    SCC_BUF *	pRxBd		/* receive buffer descriptor */
    )
    { 
    M_BLK_ID	pMblk;
    CL_BLK_ID	pClBlk;
    char *	pNewCluster;

    if (((pRxBd->dataLength - 4) - SIZEOF_ETHERHEADER) <= 0)
	{ 
        END_ERR_ADD (&pDrvCtrl->endObject, MIB2_IN_ERRS, +1);
	motCpmCleanRxBd (pDrvCtrl, pRxBd);/* reset buffer descriptor as empty */
	return;
	}

    pNewCluster = netClusterGet (pDrvCtrl->endObject.pNetPool,
                                 pDrvCtrl->pClPoolId);

    if (pNewCluster == NULL)
        {
        END_ERR_ADD (&pDrvCtrl->endObject, MIB2_IN_ERRS, +1);
	motCpmCleanRxBd (pDrvCtrl, pRxBd);/* reset buffer descriptor as empty */
	return;
        }

    if ((pMblk = mBlkGet(pDrvCtrl->endObject.pNetPool,
                         M_DONTWAIT, MT_DATA)) == NULL)
        {
        netClFree (pDrvCtrl->endObject.pNetPool, (UCHAR *)pNewCluster);
	END_ERR_ADD (&pDrvCtrl->endObject, MIB2_IN_ERRS, +1);
	motCpmCleanRxBd (pDrvCtrl, pRxBd);/* reset buffer descriptor as empty */
        return;
        }

    if ((pClBlk = clBlkGet (pDrvCtrl->endObject.pNetPool, M_DONTWAIT))
        == NULL)
        {
        netMblkFree (pDrvCtrl->endObject.pNetPool, pMblk); 
        netClFree (pDrvCtrl->endObject.pNetPool, (UCHAR *)pNewCluster);
	END_ERR_ADD (&pDrvCtrl->endObject, MIB2_IN_ERRS, +1);
	motCpmCleanRxBd (pDrvCtrl, pRxBd);/* reset buffer descriptor as empty */
        return;
        }

    /* join the cluster block to the data buffer */

    netClBlkJoin (pClBlk, (char *) pRxBd->dataPointer, pRxBd->dataLength - 4,
                  NULL, 0, 0, 0);

    /* join the mBlk and cluster Block */

    netMblkClJoin (pMblk, pClBlk);

    /* intialize the mBlk */

    pMblk->mBlkHdr.mLen 	= pRxBd->dataLength - 4;
    pMblk->mBlkHdr.mFlags 	|= M_PKTHDR;
    pMblk->mBlkPktHdr.len 	= pMblk->mBlkHdr.mLen;

    /* update receive buffer descriptor data pointer */

    pRxBd->dataPointer = (UCHAR *) pNewCluster;

    /* 
     * Reset buffer descriptor as empty
     * It is neccessary to do this before sending up the packet to the
     * protocol in case we switch to system mode. 
     */

    motCpmCleanRxBd (pDrvCtrl, pRxBd);

    /* send up to protocol */

    END_RCV_RTN_CALL (&pDrvCtrl->endObject, pMblk);

    /* bump input packet counter */

    END_ERR_ADD (&pDrvCtrl->endObject, MIB2_IN_UCAST, +1);
    }

/*******************************************************************************
*
* motCpmCleanRxBd - reset receive buffer descriptor as empty
*
* This routine resets receive buffer descriptor as empty
*
* RETURNS: N/A
*/

LOCAL void motCpmCleanRxBd
    (
    END_CTRL *	pDrvCtrl,	/* pointer to END_CTRL structure */
    SCC_BUF *	pRxBd		/* receive buffer descriptor */
    )
    {
    /* reset buffer descriptor as empty */

    if (pRxBd->statusMode & SCC_ETHER_RX_BD_W)
	pRxBd->statusMode = SCC_ETHER_RX_BD_E | SCC_ETHER_RX_BD_I |
			    SCC_ETHER_RX_BD_W;
    else
	pRxBd->statusMode = SCC_ETHER_RX_BD_E | SCC_ETHER_RX_BD_I;

    /* incr BD count */

    pDrvCtrl->ether.rxBdNext = (pDrvCtrl->ether.rxBdNext + 1) %
				pDrvCtrl->ether.rxBdNum;
    }

/*******************************************************************************
*
* motCpmCleanTxBdQueue - clean the transmit buffer descriptor queue
*
* This routine cleans the transmit buffer queue. It doesn't look for 
* transmission errors, this is done at interrupt level in motCpmIntr.
*
* RETURNS: N/A
*
*/

LOCAL void motCpmCleanTxBdQueue
    (
    END_CTRL * 	pDrvCtrl	/* pointer to END_CTRL structure */
    )
    {
    SCC_BUF * 	pTxBd;
    int		oldLevel;

    MOTCPMLOGMSG(("motCpmCleanTxBdQueue \n", 0, 0, 0, 0, 0, 0));

    pDrvCtrl->txCleaning = TRUE;

    while (pDrvCtrl->txBdIndexC != pDrvCtrl->ether.txBdNext)
	{
	pTxBd = & pDrvCtrl->ether.txBdBase[pDrvCtrl->txBdIndexC];

	/* if the data buffer has not been transmitted, don't touch it */

	if (pTxBd->statusMode & SCC_ETHER_TX_BD_R)
	    break;

	oldLevel = intLock();

	if (pDrvCtrl->freeRtn[pDrvCtrl->txBdIndexC] != NULL)
	    {
	    pDrvCtrl->freeRtn[pDrvCtrl->txBdIndexC]
		    (pDrvCtrl->freeData[pDrvCtrl->txBdIndexC].arg1,
		     pDrvCtrl->freeData[pDrvCtrl->txBdIndexC].arg2);
	    pDrvCtrl->freeRtn[pDrvCtrl->txBdIndexC] = NULL;
	    pDrvCtrl->freeData[pDrvCtrl->txBdIndexC].arg1 = NULL;
	    pDrvCtrl->freeData[pDrvCtrl->txBdIndexC].arg2 = NULL;
	    }

	intUnlock(oldLevel);

	/* incr txBdIndexC */

	pDrvCtrl->txBdIndexC = (pDrvCtrl->txBdIndexC + 1) %
			       pDrvCtrl->ether.txBdNum;
	}

    pDrvCtrl->txCleaning = FALSE;
    }

/* END OF FILE */
