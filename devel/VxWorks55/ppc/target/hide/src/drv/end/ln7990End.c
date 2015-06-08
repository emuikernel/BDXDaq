/* ln7990End.c - END style AMD 7990 LANCE Ethernet network interface driver */

/* Copyright 1984-2002 Wind River Systems, Inc. */
#include "copyright_wrs.h"

/*
modification history
--------------------
02x,16jul02,rcs  aligned pRing and pTring before initializing fields. SPR#77322
02w,14jan02,dat  Removing warnings from Diab compiler
02v,11jun00,ham  removed reference to etherLib.
02u,27jan00,dat  fixed use of NULL
02t,04mar99,jkf  removed "+ 2" when setting pTring. added "+ 1" and 
                 "* TMD_SIZ" to lnClDescTbl[0].memArea, ln7990InitMem(),
                 for boards that hand over a memory pool SPR#22628.  
                 changed ln7990ScrubTRing() to handle a receive packet.
                 fixed the endMultiLstCnt implicit declaration warning.
02s,06oct98,n_s  correct error codes in ln7990Int.
02r,30sep98,n_s  removed check for RINT in PollReceive.
02q,23sep98,n_s  fixed ln7990Int to limit netJobAdd calls and handle overload
                 cases.  spr # 22381.
02p,22sep98,n_s  fixed ln7990Int to call Restart from task level.
02o,21sep98,n_s  corrected handling of txSem in ln7990Send ().
02n,22sep98,dat  SPR 22325, System mode transition.
02m,17jul98,db   changed "holder" in ln7990InitParse from char ** to char *.
		 fixed references to "holder" in ln7990InitParse(spr #21464).
02l,12dec97,kbw  made man page edits
02k,11dec97,gnn  removed IFF_SCAT and IFF_LOAN.
02j,08dec97,gnn  END code review fixes
02i,25nov97,gnn  fixed spr#9620 polled mode receive
02h,27oct97,vin  fixed the offset problem in xxRecv() call.
02g,19oct97,vin	 moved swapping of loaned buffer before END_RCV_RTN_CALL
02f,17oct97,vin  removed extra free.
02e,07oct97,vin  MIB MTU size to ETHER_MTU
02d,03oct97,gnn  added example error upcall code
02c,25sep97,gnn  fixed SPARC issues and cleaned up memory problems.
02b,03sep97,gnn  fixed a crashing bug under heavy load
02a,31aug97,vin  reimplemented txBlocked logic, moved the txBlocked variable 
		 into the drvCtrl structure.
01z,25aug97,gnn  changes due to new netPoolInit routine.
01y,22aug97,gnn  rewrote and fixed polled mode
01x,19aug97,gnn  changes due to new buffering scheme.
01w,12aug97,gnn  changes necessitated by MUX/END update.
01v,02jun97,gnn  implemented new Memory Width ioctl.
01u,20may97,spm  corrected memory alignment, added a real lnRestart function 
01t,17apr97,gnn  added offset load parameter.
01s,24feb97,gnn  Removed an unecessary step in the interrupt routine.
                 Added code to properly pass back NET_BUFFER fields.
01r,05jan97,gnn  Turned IFF_SCAT off by default.
01q,03feb97,gnn  Changed the way in which muxBufAlloc is used.
                 Added speed as an argument to MIB stuff.
01p,22jan97,gnn  Turned IFF_SCAT off by default.
01o,22jan97,gnn  Changed code to get the enet address from the BSP.
01n,21jan97,gnn  Added code to handle send side free routines.
                 Added code to handle scatter/gather interface.
                 Added code to better handle polled mode.
                 Removed code that is common to all ENDs into endLib.c.
01m,06jan97,gnn  Fixed compilation errors and removed more non-driver stuff.
01l,23dec96,dat  added macros, added csr3B, enetAddr to init string,
01k,02dec96,gnn  forgot a break in the Ioctl.
01j,27nov96,gnn  added MIB 2 Ioctl.
01i,19nov96,gnn  Added IFF_BROADCAST to the flags.
01h,13nov96,dat  Restructured to isolate end/mib routines
01g,22oct96,gnn  Name changes to follow coding standards.
                 static functions changed to LOCAL.
01f,22oct96,gnn  Removed all netVectors and replaced them with netBuffers.
                 Did a significant code cleanup in preparation for going to
                 a 32bit world with a 24 bit driver.
01e,23sep96,gnn	 Cleanup of code.  Changed pointers to look like WRS
		 pointers and the like.
		 Made is so that the ln7990Config routine works
		 correctly to fill in the transmit and receive
		 descriptors' memory fields.
		 This version works with system level debugging or
		 buffer loaning but not with both.
01d,28aug96,gnn	 Fixed argument parsing for pre-allocated memory.
		 Added more log messages to the load sequence.
01c,15aug96,gnn	 Name changes to follow our coding standard.
01b,14aug96,gnn	 Worked the documentation and other stuff over to
		 integrate into the new scheme of network drivers.
01a,22apr96,gnn	 Copied if_ln driver and worked into the END scheme.
*/

/*
This module implements the Advanced Micro Devices Am7990 LANCE Ethernet network
interface driver.  The driver can be configured to support big-endian or
little-endian architectures, and it contains error recovery code to handle 
known device errata related to DMA activity.

This driver is designed to be moderately generic.  Thus, it operates 
unmodified across the range of architectures and targets supported by 
VxWorks.  To achieve this, the driver load routine requires an input 
string consisting of several target-specific values.  The driver also 
requires some external support routines.  These target-specific values 
and the external support routines are described below.  If any of the 
assumptions stated below are not true for your particular hardware, this 
driver might not function correctly with that hardware.

BOARD LAYOUT
This device is on-board.  No jumpering diagram is necessary.

EXTERNAL INTERFACE
The only external interface is the ln7990EndLoad() routine, which expects
the <initString> parameter as input.  This parameter passes in a 
colon-delimited string of the format:

<unit>:<CSR_reg_addr>:<RAP_reg_addr>:<int_vector>:<int_level>:<shmem_addr>:<shmem_size>:<shmem_width>:<offset>:<csr3B>

The ln7990EndLoad() function uses strtok() to parse the string.

TARGET-SPECIFIC PARAMETERS
.IP <unit>
A convenient holdover from the former model.  This parameter is used only
in the string name for the driver.
.IP <CSR_register_addr>
Tells the driver where to find the CSR register.
.IP <RAP_register_addr>
Tells the driver where to find the RAP register.
.IP <int_vector>
Configures the LANCE device to generate hardware interrupts
for various events within the device. Thus, it contains
an interrupt handler routine.  The driver calls sysIntConnect() to connect
its interrupt handler to the interrupt vector generated as a result of
the LANCE interrupt.
.IP <int_level>
This parameter is passed to an external support routine, sysLanIntEnable(),
which is described below in "External Support Requirements." This routine 
is called during as part of driver's initialization.  It handles any 
board-specific operations required to allow the servicing of a LANCE 
interrupt on targets that use additional interrupt controller devices to 
help organize and service the various interrupt sources.  This parameter 
makes it possible for this driver to avoid all board-specific knowledge of 
such devices.  
.IP <shmem_addr>
The LANCE device is a DMA type of device and typically shares access to
some region of memory with the CPU.  This driver is designed for systems
that directly share memory between the CPU and the LANCE.  It
assumes that this shared memory is directly available to it
without any arbitration or timing concerns.

This parameter can be used to specify an explicit memory region for use
by the LANCE.  This should be done on hardware that restricts the LANCE
to a particular memory region.  The constant NONE can be used to indicate
that there are no memory limitations, in which case, the driver
attempts to allocate the shared memory from the system space.
.IP <shmem_size>
Use this parameter to explicitly limit the amount of shared memory (bytes) 
that this driver uses.  Use "NONE" to indicate that there is no specific 
size limitation.  This parameter is used only if a specific memory region 
is provided to the driver.
.IP <shmem_width>
Some target hardware that restricts the shared memory region to a
specific location also restricts the access width to this region by
the CPU.  On such targets, performing an access of an invalid width
causes a bus error.  Use this parameter to specify the number of bytes 
on which data must be aligned if it is to be used by the driver during 
access to the shared memory.  Use "NONE" to indicate that there are no 
restrictions.  The support for this mechanism is not robust. Thus, its 
current implementation might not work on all targets requiring these 
restrictions.
.IP <offset>
Specifies the memory alignment offset.
.IP <csr3B>
Specifies the value that is placed into LANCE control register #3. This
value determines the bus mode of the device and thus allows the support of
big-endian and little-endian architectures.  The default value supports
Motorola-type buses.  Normally this value is 0x4.  For SPARC CPUs, it is
normally set to 0x7 to add the ACON and BCON control bits.  For more
information on this register and the bus mode of the LANCE controller, see
.I "Advanced Micro Devices Local Area Network Controller Am7990 (LANCE)."
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
When implemented, this driver requires the following system resources:

    - one interrupt vector
    - 68 bytes in the initialized data section (data) /@HELP@/
    - 0 bytes of bss /@HELP@/

The above data and BSS requirements are for the MC68020 architecture
and can vary for other architectures.  Code size (text) varies greatly 
between architectures and is therefore not quoted here.

If the driver is not given a specific region of memory using the ln7990EndLoad()
routine, then it calls cacheDmaMalloc() to allocate the memory to be shared
with the LANCE.  The size requested is 80,542 bytes.  If a memory region
is provided to the driver, the size of this region is adjustable to suit
user needs.

The LANCE can only be operated if the shared memory region is write-coherent
with the data cache.  The driver cannot maintain cache coherency for data 
that is written by the driver. That is because members within the shared 
structures are asynchronously modified by both the driver and the device, 
and these members might share the same cache line.

SEE ALSO: muxLib,
.I "Advanced Micro Devices Local Area Network Controller Am7990 (LANCE)"
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
#include "drv/end/ln7990EndCommon.h"	/* Common defines. */
#include "drv/end/ln7990End.h"		/* device description header */
#include "etherMultiLib.h"		/* multicast stuff. */
#include "end.h"			/* Common END structures. */
#include "netBufLib.h"
#include "muxLib.h"

#undef END_MACROS

#include "endLib.h"
#include "lstLib.h"			/* Needed to maintain protocol list. */

/***** LOCAL DEFINITIONS *****/

#define DELAY(count)	{                                               \
			volatile int cx = 0;                            \
			for (cx = 0; cx < (count); cx++);               \
			}

/* Configuration items */

#define LN_BUFSIZ      (ETHERMTU + SIZEOF_ETHERHEADER + 6)
#define LN_RMD_RLEN     5       /* ring size as a power of 2 -- 32 RMD's */
#define LN_TMD_TLEN     5       /* same for transmit ring    -- 32 TMD's */
#define LN_SPEED        10000000

/* Naming items */
#define LN_DEV_NAME "ln"
#define LN_DEV_NAME_LEN 3

/*
 * If LN_KICKSTART_TX is TRUE the transmitter is kick-started to force a
 * read of the transmit descriptors, otherwise the internal polling (1.6msec)
 * will initiate a read of the descriptors.  This should be FALSE is there
 * is any chance of memory latency or chip accesses detaining the LANCE DMA,
 * which results in a transmitter UFLO error.  This can be changed with the
 * global lnKickStartTx below.
 */

#define LN_KICKSTART_TX TRUE

/* Cache macros */

#define LN_CACHE_INVALIDATE(address, len) \
        CACHE_DRV_INVALIDATE (&pDrvCtrl->cacheFuncs, (address), (len))

#define LN_CACHE_PHYS_TO_VIRT(address) \
        CACHE_DRV_PHYS_TO_VIRT (&pDrvCtrl->cacheFuncs, (address))

#define LN_CACHE_VIRT_TO_PHYS(address) \
        CACHE_DRV_VIRT_TO_PHYS (&pDrvCtrl->cacheFuncs, (address))

/*
 * Default macro definitions for BSP interface.
 * These macros can be redefined in a wrapper file, to generate
 * a new module with an optimized interface.
 */

#ifndef SYS_INT_CONNECT
#define SYS_INT_CONNECT(pDrvCtrl,rtn,arg,pResult) \
    { \
    IMPORT STATUS sysIntConnect(); \
    *pResult = intConnect ((VOIDFUNCPTR *)INUM_TO_IVEC (pDrvCtrl->ivec), \
			     rtn, (int)arg); \
    }
#endif /*SYS_INT_CONNECT*/

#ifndef SYS_INT_DISCONNECT
#define SYS_INT_DISCONNECT(pDrvCtrl,rtn,arg,pResult) \
    { \
    *pResult = OK; /* HELP: need a real routine */ \
    }
#endif /*SYS_INT_DISCONNECT*/

#ifndef SYS_INT_ENABLE
#define SYS_INT_ENABLE(pDrvCtrl) \
    { \
    IMPORT void sysLanIntEnable(); \
    sysLanIntEnable (pDrvCtrl->ilevel); \
    }
#endif /*SYS_INT_ENABLE*/

#ifndef SYS_OUT_SHORT
#define SYS_OUT_SHORT(pDrvCtrl,addr,value) \
    { \
    *(USHORT *)addr = value; \
    }
#endif /*SYS_OUT_SHORT*/

#ifndef SYS_IN_SHORT
#define SYS_IN_SHORT(pDrvCtrl,addr,pData) \
    (*(USHORT *)pData = *(USHORT *)addr)
#endif /*SYS_IN_SHORT*/

#ifndef SYS_ENET_ADDR_GET
#define SYS_ENET_ADDR_GET(pAddress) \
    { \
    IMPORT unsigned char lnEnetAddr[]; \
    bcopy ((char *)lnEnetAddr, (char *)(pAddress), 6); \
    }
#endif /* SYS_ENET_ADDR_GET */

/* A shortcut for getting the hardware address from the MIB II stuff. */
#define END_HADDR(pEnd)	\
		((pEnd)->mib2Tbl.ifPhysAddress.phyAddress)

#define END_HADDR_LEN(pEnd) \
		((pEnd)->mib2Tbl.ifPhysAddress.addrLength)

#define END_FLAGS_ISSET(pEnd, setBits)                                  \
            ((pEnd)->flags & (setBits))

typedef struct free_args
    {
    void* arg1;
    void* arg2;
    } FREE_ARGS;
        
/* The definition of the driver control structure */

typedef struct ln_device
    {
    END_OBJ     end;			/* The class we inherit from. */
    ln_ib       *ib;			/* ptr to Initialization Block */
    int		unit;			/* unit number */
    int		rmdIndex;		/* current RMD index */
    int		rringSize;		/* RMD ring size */
    int 	rringLen; 		/* RMD ring length (bytes) */
    ln_rmd*	pRring;			/* RMD ring start */
    int		tmdIndex;		/* current TMD index */
    int		tmdIndexC;		/* current TMD index */
    int		tringSize;		/* TMD ring size */
    int 	tringLen;               /* TMD ring length (bytes) */
    ln_tmd*	pTring;			/* TMD ring start */
    int         ivec;                   /* interrupt vector */
    int         ilevel;                 /* interrupt level */
    u_short*	pCsr;              	/* device register CSR */
    u_short*	pRap;              	/* device register RAP */
    char*       pShMem;                	/* real ptr to shared memory */
    char*       memBase;               	/* LANCE memory pool base */
    char*       memAdrs;               	/* LANCE memory pool base */
    int         memSize;               	/* LANCE memory pool size */
    int         memWidth;               /* width of data port */
    int         offset;
    int         csr0Errs;               /* count of csr0 errors */
    long	flags;			/* Our local flags. */
    UCHAR	enetAddr[6];		/* ethernet address */
    USHORT	csr3B;
    CACHE_FUNCS cacheFuncs;             /* cache function pointers */
    BOOL	txBlocked; 		/* transmit flow control */
    BOOL        txCleaning;
    FUNCPTR     freeRtn[128];           /* Array of free routines. */
    struct free_args    freeData[128];          /* Array of free arguments */
                                        /* the free routines. */
    CL_POOL_ID  pClPoolId;
    END_ERR     lastError;              /* Last error passed to muxError */
    BOOL        errorHandling;          /* task level error handling */
    u_short     errorStat;              /* error status */
    } LN7990END_DEVICE;

/* Definitions for the flags field */

#define LS_PROMISCUOUS_FLAG     0x1
#define LS_MEM_ALLOC_FLAG       0x2
#define LS_PAD_USED_FLAG        0x4
#define LS_RCV_HANDLING_FLAG    0x8
#define LS_POLLING		0x20

/* network buffers configuration */

M_CL_CONFIG lnMclConfig = 	/* mBlk configuration table */
    {
    0, 0, NULL, 0
    };

CL_DESC lnClDescTbl [] = 	/* network cluster pool configuration table */
    {
    /* 
    clusterSize	num		memArea		memSize
    -----------	----		-------		-------
    */
    {LN_BUFSIZ,		0,	NULL,		0}
    }; 

int lnClDescTblNumEnt = (NELEMENTS(lnClDescTbl));

/***** DEBUG MACROS *****/

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
#define DRV_DEBUG_POLL_REDIR	0x10000
#define	DRV_DEBUG_LOG_NVRAM	0x20000

#ifdef LOCAL
#undef LOCAL
#endif /* LOCAL */

#define LOCAL ;

int	ln7990Debug = DRV_DEBUG_OFF; /* Turn it off initially. */
#include "nvLogLib.h"

NET_POOL ln7990NetPool;

#define DRV_LOG(FLG, X0, X1, X2, X3, X4, X5, X6)                        \
	if ((ln7990Debug & FLG)&&(FLG & DRV_DEBUG_POLL))                \
            nvLogMsg(X0, X1, X2, X3, X4, X5, X6);                       \
        else if (ln7990Debug & FLG)                                     \
            logMsg(X0, X1, X2, X3, X4, X5, X6);

#define DRV_PRINT(FLG,X)                                                \
	if (ln7990Debug & FLG) printf X;

#else /*DRV_DEBUG*/

#define DRV_LOG(DBG_SW, X0, X1, X2, X3, X4, X5, X6)
#define DRV_PRINT(DBG_SW,X)

#endif /*DRV_DEBUG*/

/***** LOCALS *****/

LOCAL int lnTsize = LN_TMD_TLEN;    /* deflt xmit ring size as power of 2 */
LOCAL int lnRsize = LN_RMD_RLEN;    /* deflt recv ring size as power of 2 */
LOCAL BOOL lnKickStartTx = LN_KICKSTART_TX;

/* forward static functions */

LOCAL void	ln7990Reset (LN7990END_DEVICE *pDrvCtrl);
LOCAL void	ln7990Int (LN7990END_DEVICE *pDrvCtrl);
LOCAL void	ln7990HandleRecvInt (LN7990END_DEVICE *pDrvCtrl);
LOCAL STATUS	ln7990Recv (LN7990END_DEVICE *pDrvCtrl, ln_rmd *rmd);
LOCAL ln_rmd *	ln7990FullRMDGet (LN7990END_DEVICE *pDrvCtrl);
LOCAL u_short	ln7990Csr0Read (LN7990END_DEVICE * pDrvCtrl);
LOCAL void	ln7990CsrWrite (LN7990END_DEVICE * pDrvCtrl, int reg,
				u_short value);
LOCAL void	ln7990Restart (LN7990END_DEVICE *pDrvCtrl);
LOCAL STATUS 	ln7990RestartSetup (LN7990END_DEVICE *pDrvCtrl);
LOCAL void	ln7990Config (LN7990END_DEVICE *pDrvCtrl);
LOCAL void	ln7990AddrFilterSet (LN7990END_DEVICE *pDrvCtrl);
LOCAL void	ln7990ScrubTRing (LN7990END_DEVICE* pDrvCtrl);
LOCAL void      ln7990HandleError (LN7990END_DEVICE  *pDrvCtrl);

/* END Specific interfaces. */

/* This is the only externally visible interface. */
END_OBJ* 	ln7990EndLoad (char* initString);

LOCAL STATUS	ln7990Start (LN7990END_DEVICE* pDrvCtrl);
LOCAL STATUS	ln7990Stop (LN7990END_DEVICE* pDrvCtrl);
LOCAL STATUS	ln7990Unload (LN7990END_DEVICE* pDrvCtrl);
LOCAL int	ln7990Ioctl (LN7990END_DEVICE* pDrvCtrl, int cmd, caddr_t data);
LOCAL STATUS	ln7990Send (LN7990END_DEVICE* pDrvCtrl, M_BLK_ID pBuf);
			  
LOCAL STATUS	ln7990MCastAddrAdd (LN7990END_DEVICE* pDrvCtrl, char* pAddress);
LOCAL STATUS	ln7990MCastAddrDel (LN7990END_DEVICE* pDrvCtrl, char* pAddress);
LOCAL STATUS	ln7990MCastAddrGet (LN7990END_DEVICE* pDrvCtrl,
				    MULTI_TABLE* pTable);
LOCAL STATUS	ln7990PollSend (LN7990END_DEVICE* pDrvCtrl, M_BLK_ID pBuf);
LOCAL STATUS	ln7990PollReceive (LN7990END_DEVICE* pDrvCtrl, M_BLK_ID pBuf);
LOCAL STATUS	ln7990PollStart (LN7990END_DEVICE* pDrvCtrl);
LOCAL STATUS	ln7990PollStop (LN7990END_DEVICE* pDrvCtrl);

LOCAL STATUS	ln7990InitParse ();
LOCAL STATUS	ln7990InitMem ();

/*
 * Declare our function table.  This is static across all driver
 * instances.
 */
LOCAL NET_FUNCS lnFuncTable =
    {
    (FUNCPTR)ln7990Start,	/* Function to start the device. */
    (FUNCPTR)ln7990Stop,	/* Function to stop the device. */
    (FUNCPTR)ln7990Unload,	/* Unloading function for the driver. */
    (FUNCPTR)ln7990Ioctl,	/* Ioctl function for the driver. */
    (FUNCPTR)ln7990Send,	/* Send function for the driver. */
    (FUNCPTR)ln7990MCastAddrAdd,/* Multicast address add */
    (FUNCPTR)ln7990MCastAddrDel,/* Multicast address delete */
    (FUNCPTR)ln7990MCastAddrGet,/* Multicast table retrieve */
    (FUNCPTR)ln7990PollSend,	/* Polling send function for the driver. */
    (FUNCPTR)ln7990PollReceive,	/* Polling receive function for the driver. */
    endEtherAddressForm,        /* Put address info into a packet.  */
    endEtherPacketDataGet,      /* Get a pointer to packet data. */
    endEtherPacketAddrGet       /* Get packet addresses. */
    };

/* external IMPORT's */

#ifndef END_MACROS
IMPORT int endMultiLstCnt (END_OBJ *);
#endif


/******************************************************************************
*
* ln7990EndLoad - initialize the driver and device
*
* This routine initializes the driver and the device to the operational state.
* All of the device-specific parameters are passed in <initString>, which
* expects a string of the following format:
*
* <unit>:<CSR_reg_addr>:<RAP_reg_addr>:<int_vector>:<int_level>:<shmem_addr>:
* <shmem_size>:<shmem_width>
*
* This routine can be called in two modes. If it is called with an empty but
* allocated string, it places the name of this device (that is, "ln") into 
* the <initString> and returns 0.
*
* If the string is allocated and not empty, the routine attempts to load
* the driver using the values specified in the string.
*
* RETURNS: An END object pointer, or NULL on error, or 0 and the name of the
* device if the <initString> was NULL.
*/

END_OBJ* ln7990EndLoad
    (
    char* initString		/* string to be parse by the driver */
    )
    {
    LN7990END_DEVICE 	*pDrvCtrl;

    DRV_LOG (DRV_DEBUG_LOAD, "Loading ln...\n", 1, 2, 3, 4, 5, 6);

    if (initString == NULL)
        return (NULL);
    
    if (initString[0] == 0)
        {
        bcopy((char *)LN_DEV_NAME, initString, LN_DEV_NAME_LEN);
        return (0);
        }
    
    /* allocate the device structure */

    pDrvCtrl = (LN7990END_DEVICE *)calloc (sizeof (LN7990END_DEVICE), 1);
    if (pDrvCtrl == NULL)
	goto errorExit;

    /* parse the init string, filling in the device structure */

    if (ln7990InitParse (pDrvCtrl, initString) == ERROR)
	goto errorExit;

    /* Have the BSP hand us our address. */

    SYS_ENET_ADDR_GET(&(pDrvCtrl->enetAddr));

    /* initialize the END and MIB2 parts of the structure */

    if (END_OBJ_INIT (&pDrvCtrl->end, (DEV_OBJ *)pDrvCtrl, LN_DEV_NAME,
		    pDrvCtrl->unit, &lnFuncTable,
                      "AMD 7990 Lance Enhanced Network Driver") == ERROR
     || END_MIB_INIT (&pDrvCtrl->end, M2_ifType_ethernet_csmacd,
                      &pDrvCtrl->enetAddr[0], 6, ETHERMTU,
                      LN_SPEED)
		    == ERROR)
	goto errorExit;

    /* Perform memory allocation */

    if (ln7990InitMem (pDrvCtrl) == ERROR)
	goto errorExit;

    /* Perform memory distribution and reset and reconfigure the device */

    if (ln7990RestartSetup (pDrvCtrl) == ERROR)
        goto errorExit;

    /* set the flags to indicate readiness */

    END_OBJ_READY (&pDrvCtrl->end,
		    IFF_UP | IFF_RUNNING | IFF_NOTRAILERS | IFF_BROADCAST
		    | IFF_MULTICAST | IFF_SIMPLEX);
    
    DRV_LOG (DRV_DEBUG_LOAD, "Done loading ln7990...\n", 1, 2, 3, 4, 5, 6);

    return (&pDrvCtrl->end);

errorExit:
    if (pDrvCtrl != NULL)
	free ((char *)pDrvCtrl);

    return NULL;
    }

/*******************************************************************************
*
* ln7990InitParse - parse the initialization string
*
* Parse the input string.  Fill in values in the driver control structure.
* The initialization string format is:
* <unit>:<csrAdr>:<rapAdr>:<vecnum>:<intLvl>:<memAdrs>:<memSize>:<memWidth>:
* <offset>:<csr3B>
*
* .IP <unit>
* Device unit number, a small integer.
* .IP <csrAdr>
* Address of CSR0 register.
* .IP <rapAdr>
* Address of RAP register.
* .IP <vecNum>
* Interrupt vector number (used with sysIntConnect() ).
* .IP <intLvl>
* Interrupt level.
* .IP <memAdrs>
* Memory pool address or NONE.
* .IP <memSize>
* Memory pool size or zero.
* .IP <memWidth>
* Memory system size, 1, 2, or 4 bytes (optional).
* .IP <offset>
* Memory offset for alignment.
* .IP <csr3B>
* CSR register 3B control value, normally 0x4 or 0x7.
*
* RETURNS: OK, or ERROR if any arguments are invalid.
*/

LOCAL STATUS ln7990InitParse
    (
    LN7990END_DEVICE * pDrvCtrl,
    char * initString
    )
    {
    char *	tok;
    char *	pHolder = NULL;
    long	address;
    
    /* Parse the initString */

    /* Unit number. */

    tok = strtok_r (initString, ":", &pHolder);
    if (tok == NULL)
	return ERROR;
    pDrvCtrl->unit = atoi (tok);

    /* CSR address. */
    
    tok = strtok_r (NULL, ":", &pHolder);
    if (tok == NULL)
	return ERROR;
    address = strtoul (tok, NULL, 16);
    pDrvCtrl->pCsr = (UINT16 *)address;

    /* RAP address. */

    tok = strtok_r (NULL, ":", &pHolder);
    if (tok == NULL)
	return ERROR;
    address = strtoul (tok, NULL, 16);
    pDrvCtrl->pRap = (UINT16 *)address;

    /* Interrupt vector. */

    tok = strtok_r (NULL, ":", &pHolder);
    if (tok == NULL)
	return ERROR;
    pDrvCtrl->ivec = atoi (tok);

    /* Interrupt level. */
    tok = strtok_r (NULL, ":", &pHolder);
    if (tok == NULL)
	return ERROR;
    pDrvCtrl->ilevel = atoi (tok);

    /* Caller supplied memory address. */
    tok = strtok_r (NULL, ":", &pHolder);
    if (tok == NULL)
	return ERROR;
    pDrvCtrl->memAdrs = (char *)strtoul (tok, NULL, 16);

    /* Caller supplied memory size. */

    tok = strtok_r (NULL, ":", &pHolder);
    if (tok == NULL)
	return ERROR;
    pDrvCtrl->memSize = strtoul (tok, NULL, 16);

    /* Caller supplied memory width. */

    tok = strtok_r (NULL, ":", &pHolder);
    if (tok == NULL)
	return ERROR;
    pDrvCtrl->memWidth = atoi (tok);

    /* Caller supplied alignment offset. */
    tok = strtok_r (NULL, ":", &pHolder);
    if (tok == NULL)
	return ERROR;
    pDrvCtrl->offset = atoi (tok);

    /* Do we use csr3B? */

    tok = strtok_r (NULL, ":", &pHolder);
    if (tok == NULL)
	return ERROR;
    pDrvCtrl->csr3B = atoi (tok);

    DRV_LOG (DRV_DEBUG_LOAD, "Processed all arugments\n", 1, 2, 3, 4, 5, 6);

    return OK;
    }

/*******************************************************************************
*
* ln7990InitMem - initialize memory for Lance chip
*
* Using data in the control structure, setup and initialize the memory
* areas needed.  If the memory address is not already specified, then allocate
* cache safe memory.
*
* RETURNS: OK or ERROR.
*/

LOCAL STATUS ln7990InitMem
    (
    LN7990END_DEVICE * pDrvCtrl	/* device to be initialized */
    )
    {
    UINT	sz;           	/* temporary size holder */
    int 	ix;
    ln_rmd * 	pRmd;
    void * 	pTemp;
    char* 	pTempBuf;

    /***** Establish size of shared memory region we require *****/

    if ((int) pDrvCtrl->memAdrs != NONE)  /* specified memory pool */
        {
        /*
         * With a specified memory pool we want to maximize
         * lnRsize and lnTsize
         */

        sz = (pDrvCtrl->memSize - (RMD_SIZ + TMD_SIZ + sizeof (ln_ib)))
               / ((2 * LN_BUFSIZ) + RMD_SIZ + TMD_SIZ);

        sz >>= 1;               /* adjust for roundoff */

        for (lnRsize = 0; sz != 0; lnRsize++, sz >>= 1)
            ;

        lnTsize = lnRsize;      /* lnTsize = lnRsize for convenience */
        }

    /* limit ring sizes to reasonable values */

    lnRsize = max (lnRsize, 2);		/* 4 Rx buffers is reasonable min */
    lnRsize = min (lnRsize, 7);		/* 128 Rx buffers is max for chip */
    lnTsize = max (lnTsize, 2);		/* 4 Tx buffers is reasonable min */
    lnTsize = min (lnTsize, 7);		/* 128 Tx buffers is max for chip */

    /* Add it all up */

    sz = (((1 << lnRsize) + 1) * RMD_SIZ) +
        (((1 << lnTsize) + 1) * TMD_SIZ) +  IB_SIZ + 28;


    /***** Establish a region of shared memory *****/

    /* OK. We now know how much shared memory we need.  If the caller
     * provides a specific memory region, we check to see if the provided
     * region is large enough for our needs.  If the caller did not
     * provide a specific region, then we attempt to allocate the memory
     * from the system, using the cache aware allocation system call.
     */

    switch ((int) pDrvCtrl->memAdrs)
        {
        default :       /* caller provided memory */
            if (pDrvCtrl->memSize < sz)     /* not enough space */
                {
		DRV_LOG (DRV_DEBUG_LOAD, "ln7990: not enough memory provided\n"
                         "ln7990: need %ul got %d\n",
                         pDrvCtrl->memSize, sz, 3, 4, 5, 6);
                return (ERROR);
                }

	    /* set the beginning of pool */

            pDrvCtrl->pShMem = pDrvCtrl->memAdrs;

            /* assume pool is cache coherent, copy null structure */

            pDrvCtrl->cacheFuncs = cacheNullFuncs;
	    DRV_LOG (DRV_DEBUG_LOAD, "Memory checks out\n", 1, 2, 3, 4, 5, 6);
            break;

        case NONE :     /* get our own memory */

            /* Because the structures that are shared between the device
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
                printf ("ln: device requires cache coherent memory\n");
                return (ERROR);
                }

            pDrvCtrl->pShMem = (char *) cacheDmaMalloc (sz);

            if (pDrvCtrl->pShMem == NULL)
                {
                printf ("ln: system memory unavailable\n");
                return (ERROR);
                }

            /* copy the DMA structure */

            pDrvCtrl->cacheFuncs = cacheDmaFuncs;

            break;
        }

    /*                        Turkey Carving
     *                        --------------
     *
     *                          LOW MEMORY
     *
     *             |-------------------------------------|
     *             |       The initialization block      |
     *             |         (sizeof (ln_ib))            |
     *             |-------------------------------------|
     *             |         The Rx descriptors          |
     *             | ((1 << lnRsize) + 1)*sizeof (ln_rmd)|
     *             |-------------------------------------|
     *             |         The Tx descriptors          |
     *             | ((1 << lnTsize) + 1)*sizeof (ln_tmd)|
     *             |-------------------------------------|
     */

    /* align */
    pDrvCtrl->pShMem = (char *) ( ( (int)pDrvCtrl->pShMem + 3) & ~3);

    /* Save some things */

    pDrvCtrl->memBase  = (char *)((ULONG)pDrvCtrl->pShMem & 0xff000000);

    if ((int) pDrvCtrl->memAdrs == NONE)
        pDrvCtrl->flags |= LS_MEM_ALLOC_FLAG;

    /* first let's clear memory */

    bzero ((char *) pDrvCtrl->pShMem, (int) sz); /* HELP: deal with memWidth */

    /* setup Rx memory pointers */

    pDrvCtrl->pRring = (ln_rmd *) ((((int)pDrvCtrl->pShMem + IB_SIZ) + 7) &~7);
    pDrvCtrl->rringLen  = lnRsize;
    pDrvCtrl->rringSize = 1 << lnRsize;
    pDrvCtrl->rmdIndex	= 0;

    /* setup Tx memory pointers. */

    pDrvCtrl->pTring = (ln_tmd *) ((int)pDrvCtrl->pRring +
			((1 << lnRsize) + 1) * RMD_SIZ);

    pDrvCtrl->tringSize = 1 << lnTsize;
    pDrvCtrl->tringLen  = lnTsize;
    pDrvCtrl->tmdIndex	= 0;
    pDrvCtrl->tmdIndexC	= 0;

    /* Set up the structures to allow us to free data after sending it. */
    
    for (ix = 0; ix < pDrvCtrl->rringSize; ix++)
        {
        pDrvCtrl->freeRtn[ix] = NULL;
        pDrvCtrl->freeData[ix].arg1 = NULL;
        pDrvCtrl->freeData[ix].arg2 = NULL;
        }

    /*
     * Allocate receive buffers from our own private pool.
     */

    if ((pDrvCtrl->end.pNetPool = malloc (sizeof(NET_POOL))) == NULL)
        return (ERROR);

    lnMclConfig.mBlkNum = pDrvCtrl->rringSize * 2;
    lnClDescTbl[0].clNum = pDrvCtrl->rringSize *2;
    lnMclConfig.clBlkNum = lnClDescTbl[0].clNum;

    lnMclConfig.memSize = (lnMclConfig.mBlkNum * (MSIZE + sizeof (long))) +
                          (lnMclConfig.clBlkNum * (CL_BLK_SZ + sizeof(long)));

    if ((lnMclConfig.memArea = (char *) memalign (sizeof(long),
                                                  lnMclConfig.memSize))
        == NULL)
        return (ERROR);
    

    lnClDescTbl[0].clNum = pDrvCtrl->rringSize *2;
    
    lnClDescTbl[0].memSize = (lnClDescTbl[0].clNum * (LN_BUFSIZ + 8))
        + sizeof(int);

    if ((int) pDrvCtrl->memAdrs != NONE) /* Do we hand over our own memory? */
        {
        lnClDescTbl[0].memArea = 
                (char *)(pDrvCtrl->pTring + (((1 << lnTsize) + 1) * TMD_SIZ));
        }
    else
        {
        lnClDescTbl[0].memArea =
            (char *) cacheDmaMalloc (lnClDescTbl[0].memSize);
        if (lnClDescTbl[0].memArea == NULL)
            {
            DRV_LOG(DRV_DEBUG_LOAD,
                    "system memory unavailable\n", 1, 2, 3, 4, 5, 6);
            return (ERROR);
            }
        }
    
    if (netPoolInit(pDrvCtrl->end.pNetPool, &lnMclConfig,
                    &lnClDescTbl[0], lnClDescTblNumEnt, NULL) == ERROR)
        {
        DRV_LOG (DRV_DEBUG_LOAD, "Could not init buffering\n",
                 1, 2, 3, 4, 5, 6);
        return (ERROR);
        }
#ifdef DRV_DEBUG
    ln7990NetPool = *pDrvCtrl->end.pNetPool;
#endif
    /* Store the cluster pool id as others need it later. */
    pDrvCtrl->pClPoolId = clPoolIdGet(pDrvCtrl->end.pNetPool,
                                      LN_BUFSIZ, FALSE);

    pRmd = pDrvCtrl->pRring;
    for (ix = 0; ix < pDrvCtrl->rringSize; ix++, pRmd++)
        {
        if ((pTempBuf = (char *)netClusterGet(pDrvCtrl->end.pNetPool,
                                              pDrvCtrl->pClPoolId))
            == NULL)
            {
            DRV_LOG (DRV_DEBUG_LOAD, "Could not get a buffer\n",
                     1, 2, 3, 4, 5, 6);
            return (ERROR);
            }
        pTempBuf += pDrvCtrl->offset;
        LN_RMD_BUF_TO_ADDR (pRmd, pTemp, pTempBuf);
        }

    return OK;
    }

/*******************************************************************************
*
* ln7990Start - start the device
*
* This function calls BSP functions to connect interrupts and start the
* device running in interrupt mode.
*
* RETURNS: OK or ERROR
*
*/

LOCAL STATUS ln7990Start
    (
    LN7990END_DEVICE *pDrvCtrl
    )
    {
    STATUS result;

    pDrvCtrl->txCleaning = FALSE;
    pDrvCtrl->txBlocked = FALSE;
    SYS_INT_CONNECT (pDrvCtrl, ln7990Int, (int)pDrvCtrl, &result);
    if (result == ERROR)
	return ERROR;

    DRV_LOG (DRV_DEBUG_LOAD, "Interrupt connected.\n", 1, 2, 3, 4, 5, 6);

    SYS_INT_ENABLE (pDrvCtrl);

    DRV_LOG (DRV_DEBUG_LOAD, "interrupt enabled.\n", 1, 2, 3, 4, 5, 6);

    return (OK);
    }

/*******************************************************************************
*
* ln7990HandleError - handle controller interrupt errors
*
* This routine is called to process controller errors at a task level that 
* were detecected in interrupt context.
*
* RETURNS: N/A.
*/

LOCAL void ln7990HandleError
    (
    LN7990END_DEVICE  *pDrvCtrl
    )
    {
    u_short stat;

    pDrvCtrl->errorHandling = FALSE;
    stat = pDrvCtrl->errorStat;
    pDrvCtrl->errorStat &= ~stat;
    
    if (stat & lncsr_MERR)
	{
	pDrvCtrl->lastError.errCode = END_ERR_RESET;
	pDrvCtrl->lastError.pMesg = "Memory error.";
	muxError (&pDrvCtrl->end, &pDrvCtrl->lastError);
	}

    if (stat & lncsr_BABL)
	{
	pDrvCtrl->lastError.errCode = END_ERR_WARN;
	pDrvCtrl->lastError.pMesg = "Babbling";
	muxError (&pDrvCtrl->end, &pDrvCtrl->lastError);
	}

    if (stat & lncsr_MISS)
	{
	pDrvCtrl->lastError.errCode = END_ERR_WARN;
	pDrvCtrl->lastError.pMesg = "Missing";
	muxError (&pDrvCtrl->end, &pDrvCtrl->lastError);
	}
    
    return;
    }

    

/*******************************************************************************
*
* ln7990Int - handle controller interrupt
*
* This routine is called at interrupt level in response to an interrupt from
* the controller.
*
* RETURNS: N/A.
*/

LOCAL void ln7990Int
    (
    LN7990END_DEVICE  *pDrvCtrl
    )
    {
    u_short        stat;
    ln_rmd         *pRmd;

   /* Read the device status register */

    stat = ln7990Csr0Read (pDrvCtrl);

    DRV_LOG (DRV_DEBUG_INT, "i=0x%x:\n", stat, 2, 3, 4, 5, 6);

    /* If false interrupt, return. */

    if (!(stat & lncsr_INTR))
	{
	DRV_LOG (DRV_DEBUG_INT, "False interrupt.\n", 1, 2, 3, 4, 5, 6);
        return;
	}

    /*
     * enable interrupts, clear receive and/or transmit interrupts,
     * and clear any errors that may be set.
     */
    ln7990CsrWrite (pDrvCtrl, 0, ((stat &
	    (lncsr_BABL|lncsr_CERR|lncsr_MISS|lncsr_MERR|
	     lncsr_RINT|lncsr_TINT|lncsr_IDON)) | lncsr_INEA));

    /* Check for errors */

    if (stat & (lncsr_BABL | lncsr_MISS | lncsr_MERR))
        {
        ++pDrvCtrl->csr0Errs;

	END_ERR_ADD (&pDrvCtrl->end, MIB2_IN_ERRS, +1);

	if (stat & lncsr_BABL)
	    {
	    pDrvCtrl->errorStat |= lncsr_BABL;
	    if (!pDrvCtrl->errorHandling)
		{
		if (netJobAdd ((FUNCPTR) ln7990HandleError, (int) pDrvCtrl
			       , 2, 3, 4, 5) == OK)
		    {
		    pDrvCtrl->errorHandling = TRUE;
		    }
		}
	    DRV_LOG (DRV_DEBUG_INT, "Babbling\n", 1, 2, 3, 4, 5, 6);
	    }
	if (stat & lncsr_MISS)
	    {
	    pDrvCtrl->errorStat |= lncsr_MISS;
	    if (!pDrvCtrl->errorHandling)
		{
		if (netJobAdd ((FUNCPTR) ln7990HandleError, (int) pDrvCtrl
			       , 2, 3, 4, 5) == OK)
		    {
		    pDrvCtrl->errorHandling = TRUE;
		    }
		}
	    DRV_LOG (DRV_DEBUG_INT, "Missing\n", 1, 2, 3, 4, 5, 6);
	    }
        
	/* restart chip on fatal error */

        if (stat & lncsr_MERR)        /* memory error */
            {
	    pDrvCtrl->errorStat |= lncsr_MERR;
	    if (!pDrvCtrl->errorHandling)
		{
		if (netJobAdd ((FUNCPTR) ln7990HandleError, (int) pDrvCtrl
			       , 2, 3, 4, 5) == OK)
		    {
		    pDrvCtrl->errorHandling = TRUE;
		    }
		}
	    DRV_LOG (DRV_DEBUG_INT, "Memory error, restarting.\n",
                     1, 2, 3, 4, 5, 6);
            END_FLAGS_CLR (&pDrvCtrl->end, (IFF_UP | IFF_RUNNING));

	    /* Only reset device if Restart will occur */

	    if (netJobAdd ((FUNCPTR) ln7990Restart
			   , (int) pDrvCtrl, 2, 3, 4, 5) == OK)
		{
		ln7990Reset (pDrvCtrl);
		}
	    return;
	    }
        }

    /* Have netTask handle any input packets */

    pRmd =  pDrvCtrl->pRring + pDrvCtrl->rmdIndex;
    LN_CACHE_INVALIDATE (pRmd, RMD_SIZ);

    if (((stat & lncsr_RINT) && (stat & lncsr_RXON)) ||
	LN_RMD_OWNED (pRmd) == 0)
        {
        DRV_LOG (DRV_DEBUG_INT, "r ", 1, 2, 3, 4, 5, 6);
        if (!(pDrvCtrl->flags & LS_RCV_HANDLING_FLAG))
            {
            if (netJobAdd ((FUNCPTR)ln7990HandleRecvInt, (int)pDrvCtrl,
                              0,0,0,0) == OK)
		{
		pDrvCtrl->flags |= LS_RCV_HANDLING_FLAG;
		}
	    }
        }

    /*
     * Did LANCE update any of the TMD's?
     * If not then don't bother continuing with transmitter stuff
     */

    if (!(stat & lncsr_TINT))
	{
        return;
	}


    DRV_LOG (DRV_DEBUG_INT, "t ", 1, 2, 3, 4, 5, 6);
    
    if (!pDrvCtrl->txCleaning)
        {
        pDrvCtrl->txCleaning = TRUE;
        netJobAdd ((FUNCPTR)ln7990ScrubTRing, (int) pDrvCtrl, 0, 0, 0, 0);
        }
    if (pDrvCtrl->txBlocked)	/* cause a restart */
        {
        netJobAdd ((FUNCPTR)muxTxRestart, (int) &pDrvCtrl->end, 0, 0, 0, 0);
        pDrvCtrl->txBlocked = FALSE;
        }

    /* Flush the write pipe */

    CACHE_PIPE_FLUSH ();
    }


/*******************************************************************************
*
* ln7990HandleRecvInt - task level interrupt service for input packets
*
* This routine is called at task level indirectly by the interrupt
* service routine to do any message received processing.
*
* RETURNS: N/A.
*/

LOCAL void ln7990HandleRecvInt
    (
    LN7990END_DEVICE *pDrvCtrl
    )
    {
    ln_rmd *pRmd;

    do
        {
        pDrvCtrl->flags |= LS_RCV_HANDLING_FLAG;

        while ((pRmd = ln7990FullRMDGet (pDrvCtrl)) != NULL)
            ln7990Recv (pDrvCtrl, pRmd);

        /*
         * There is a RACE right here.  The ISR could add a receive packet
         * and check the boolean below, and decide to exit.  Thus the
         * packet could be dropped if we don't double check before we
         * return.
         */

        pDrvCtrl->flags &= ~LS_RCV_HANDLING_FLAG;
        }
    while (ln7990FullRMDGet (pDrvCtrl) != NULL);
    /* this double check solves the RACE */

    }

/*******************************************************************************
*
* ln7990FullRMDGet - get next received message RMD
*
* Get next recevied message descriptor.  Returns NULL if none are
* ready.
*
* RETURNS: ptr to next Rx desc to process, or NULL if none ready.
*/

LOCAL ln_rmd *ln7990FullRMDGet
    (
    LN7990END_DEVICE  *pDrvCtrl
    )
    {
    ln_rmd   *pRmd;

    pRmd = pDrvCtrl->pRring + pDrvCtrl->rmdIndex;  /* form ptr to Rx desc */

    LN_CACHE_INVALIDATE (pRmd, RMD_SIZ);

    if (LN_RMD_OWNED (pRmd) == 0)
        return (pRmd);
    else
        return ((ln_rmd *) NULL);
    }


/*******************************************************************************
*
* ln7990Recv - process the next incoming packet
*
* Handle one incoming packet.  The packet is checked for errors.
*
* RETURNS: N/A.
*/

LOCAL STATUS ln7990Recv
    (
    LN7990END_DEVICE *pDrvCtrl,
    ln_rmd   *pRmd
    )
    {
    int         len;
    u_long	phys;
    M_BLK_ID 	pMblk;
    char*       pCluster;
    char*       pNewCluster;
    char*       pTemp;
    CL_BLK_ID	pClBlk;
    /* Packet must be checked for errors. */

    /* If error flag */
    /* OR if packet is not completely in one buffer */
    if  (LN_RMD_ERR (pRmd))
        {
	DRV_LOG (DRV_DEBUG_RX, "RMD error!\n", 1, 2, 3, 4, 5, 6);
	END_ERR_ADD (&pDrvCtrl->end, MIB2_IN_ERRS, +1);
	goto cleanRXD;
        }

    /* If we cannot get a buffer to loan then bail out. */
    pNewCluster = netClusterGet(pDrvCtrl->end.pNetPool, pDrvCtrl->pClPoolId);

    if (pNewCluster == NULL)
        {
	DRV_LOG (DRV_DEBUG_RX, "Cannot loan!\n", 1, 2, 3, 4, 5, 6);
	END_ERR_ADD (&pDrvCtrl->end, MIB2_IN_ERRS, +1);
	goto cleanRXD;
        }

    if ((pClBlk = netClBlkGet (pDrvCtrl->end.pNetPool, M_DONTWAIT)) == NULL)
        {
        netClFree (pDrvCtrl->end.pNetPool, pNewCluster);
	DRV_LOG (DRV_DEBUG_RX, "Out of Cluster Blocks!\n", 1, 2, 3, 4, 5, 6);
	END_ERR_ADD (&pDrvCtrl->end, MIB2_IN_ERRS, +1);
	goto cleanRXD;
        }
    
    /*
     * OK we've got a spare, let's get an M_BLK_ID and marry it to the
     * one in the ring.
     */
    if ((pMblk = mBlkGet(pDrvCtrl->end.pNetPool, M_DONTWAIT, MT_DATA)) == NULL)
        {
        netClBlkFree (pDrvCtrl->end.pNetPool, pClBlk); 
        netClFree (pDrvCtrl->end.pNetPool, pNewCluster);
	DRV_LOG (DRV_DEBUG_RX, "Out of M Blocks!\n", 1, 2, 3, 4, 5, 6);
	END_ERR_ADD (&pDrvCtrl->end, MIB2_IN_ERRS, +1);
	goto cleanRXD;
        }


    END_ERR_ADD (&pDrvCtrl->end, MIB2_IN_UCAST, +1);
    len = LN_PKT_LEN_GET (pRmd);           		/* get packet length */

    /* Get pointer to packet */

    LN_RMD_TO_ADDR (pDrvCtrl->memBase , pRmd, phys);

    pCluster = LN_CACHE_PHYS_TO_VIRT (phys);

    pCluster -= pDrvCtrl->offset;
    
    /* Join the cluster to the MBlock */
    netClBlkJoin (pClBlk, pCluster, len, NULL, 0, 0, 0);
    netMblkClJoin (pMblk, pClBlk);

    /* make the packet data coherent */
    LN_CACHE_INVALIDATE (pMblk->mBlkHdr.mData, len);

    pMblk->mBlkHdr.mData += pDrvCtrl->offset;
    pMblk->mBlkHdr.mLen = len;
    pMblk->mBlkHdr.mFlags |= M_PKTHDR;
    pMblk->mBlkPktHdr.len = len;


    DRV_LOG (DRV_DEBUG_RX, "Calling upper layer!\n", 1, 2, 3, 4, 5, 6);

    /* Deal with memory alignment. */

    pNewCluster += pDrvCtrl->offset;

    LN_RMD_BUF_TO_ADDR (pRmd, pTemp, pNewCluster);

    LN_CLEAN_RXD (pRmd);

    /* Flush the write pipe */

    CACHE_PIPE_FLUSH ();

    /* Advance our management index */

    pDrvCtrl->rmdIndex = (pDrvCtrl->rmdIndex + 1) & (pDrvCtrl->rringSize - 1);

    /* Call the upper layer's receive routine. */

    END_RCV_RTN_CALL(&pDrvCtrl->end, pMblk);

    return (OK);

cleanRXD:

    LN_CLEAN_RXD (pRmd);

    /* Flush the write pipe */

    CACHE_PIPE_FLUSH ();

    /* Advance our management index */

    pDrvCtrl->rmdIndex = (pDrvCtrl->rmdIndex + 1) & (pDrvCtrl->rringSize - 1);

    return (OK);
    }

/*******************************************************************************
*
* ln7990Send - the driver send routine
*
* This routine takes a M_BLK_ID sends off the data in the M_BLK_ID.
* The buffer must already have the addressing information properly installed
* in it.  This is done by a higher layer.  The last arguments are a free
* routine to be called when the device is done with the buffer and a pointer
* to the argument to pass to the free routine.  
*
* RETURNS: OK or ERROR.
*/

LOCAL STATUS ln7990Send
    (
    LN7990END_DEVICE *pDrvCtrl,	/* device ptr */
    M_BLK_ID pMblk	/* data to send */
    )
    {
    ln_tmd*	pTmd;
    int         len = 0;
    void*       pTemp;
    char*       pBuf;
    char*       pOrig;
    int         s;
    /*
     * Obtain exclusive access to transmitter.  This is necessary because
     * we might have more than one stack transmitting at once.
     */

    if (!(pDrvCtrl->flags & LS_POLLING))
	END_TX_SEM_TAKE (&pDrvCtrl->end, WAIT_FOREVER);

    /*
     * Here we handle both simple and vector transmit in the same place.
     *
     */

#if 0
    {
    M_BLK_ID    pChunk;
    int         count;

    pChunk = pMblk;
    for (count = 0; pChunk != NULL && (pChunk->mBlkHdr.mData != 0);
         pChunk = pChunk->m_next, count++)
        {

        if (pChunk->mBlkHdr.mLen <= 0)
            {
            netMblkClFree(pChunk);
            continue;
            }

        pTmd = pDrvCtrl->pTring + pDrvCtrl->tmdIndex;
        
        LN_CACHE_INVALIDATE (pTmd, TMD_SIZ);
        
        if ((pTmd->tbuf_stat & lntmd1_OWN) ||
            (((pDrvCtrl->tmdIndex + 1) & (pDrvCtrl->tringSize - 1))
             == pDrvCtrl->tmdIndexC))
            {
            if (!(pDrvCtrl->flags & LS_POLLING))
                END_TX_SEM_GIVE (&pDrvCtrl->end);
            /* Are we still on the first chunk? */
            if (count == 0)
                netMblkClChainFree(pMblk);
            else /* Have to iteratively clean up. */
                {
                
                }
            DRV_LOG (DRV_DEBUG_TX, "Out of TMDs!\n", 1, 2, 3, 4, 5, 6);
            return (END_ERR_BLOCK);
            }
        len = pChunk->mBlkHdr.mLen;
        LN_TMD_BUF_TO_ADDR(pTmd, pTemp, pChunk->mBlkHdr.mData);
        pTmd->tbuf_err = 0;         /* clear buffer error status */
        pTmd->tbuf_bcnt = -len;     /* negative message byte count */
        
        if (count == 0)
            pTmd->tbuf_stat |=  lntmd1_STP;
        
        pTmd->tbuf_stat |= lntmd1_OWN;

        pDrvCtrl->freeRtn[pDrvCtrl->tmdIndex] = (FUNCPTR)netMblkClFree;
        pDrvCtrl->freeData[pDrvCtrl->tmdIndex].arg1 = pChunk;
        pDrvCtrl->freeData[pDrvCtrl->tmdIndex].arg2 = NULL;

        /* Advance our management index */
        pDrvCtrl->tmdIndex = (pDrvCtrl->tmdIndex + 1) & (pDrvCtrl->tringSize - 1);
        
        }
    
    /* On the final tmd we set the END bit as well as OWN. */
    pTmd->tbuf_stat |= lntmd1_OWN | lntmd1_ENP;

    /* If we only have one TMD for the packet then make sure of size. */
    if (count == 1)
        {
        len = max (len, ETHERSMALL);
        pTmd->tbuf_bcnt = -len;
        }
    
    }
#else
    pTmd = pDrvCtrl->pTring + pDrvCtrl->tmdIndex;
    
    LN_CACHE_INVALIDATE (pTmd, TMD_SIZ);
    
    if ((pTmd->tbuf_stat & lntmd1_OWN) ||
        (((pDrvCtrl->tmdIndex + 1) & (pDrvCtrl->tringSize - 1))
         == pDrvCtrl->tmdIndexC))
        {
        if (!(pDrvCtrl->flags & LS_POLLING))
            END_TX_SEM_GIVE (&pDrvCtrl->end);
        /* Are we still on the first chunk? */
        DRV_LOG (DRV_DEBUG_TX, "Out of TMDs!\n", 1, 2, 3, 4, 5, 6);
        s = intLock();
	pDrvCtrl->txBlocked = TRUE;
        intUnlock(s);
        return (END_ERR_BLOCK);
        }

    DRV_LOG (DRV_DEBUG_TX, "before cluster get %d %d\n",
             (int) pDrvCtrl->end.pNetPool,
             (int) pDrvCtrl->pClPoolId, 3, 4, 5, 6);
    pOrig = pBuf = netClusterGet(pDrvCtrl->end.pNetPool, pDrvCtrl->pClPoolId);
    DRV_LOG (DRV_DEBUG_TX, "after cluster get\n", 1, 2, 3, 4, 5, 6);

    if (pBuf == NULL)
        {
	if (!(pDrvCtrl->flags & LS_POLLING))
	    END_TX_SEM_GIVE (&pDrvCtrl->end);
        netMblkClChainFree(pMblk);
        return (ERROR);
        }

    pBuf += pDrvCtrl->offset;
    
    len = netMblkToBufCopy (pMblk, pBuf, NULL);
    netMblkClChainFree(pMblk); 
    LN_TMD_BUF_TO_ADDR(pTmd, pTemp, pBuf);
    len = max (len, ETHERSMALL);
    pTmd->tbuf_err = 0;         /* clear buffer error status */
    pTmd->tbuf_bcnt = -len;     /* negative message byte count */
        
    pTmd->tbuf_stat |=  lntmd1_STP | lntmd1_OWN | lntmd1_ENP;
    
    pDrvCtrl->freeRtn[pDrvCtrl->tmdIndex] = (FUNCPTR)netClFree;
    pDrvCtrl->freeData[pDrvCtrl->tmdIndex].arg1 = pDrvCtrl->end.pNetPool;
    pDrvCtrl->freeData[pDrvCtrl->tmdIndex].arg2 = pOrig;

    /* Advance our management index */
    pDrvCtrl->tmdIndex = (pDrvCtrl->tmdIndex + 1) & (pDrvCtrl->tringSize - 1);
#endif
    /* Flush the write pipe */
    
    CACHE_PIPE_FLUSH ();
    
    if (lnKickStartTx)
        {
        if (!(pDrvCtrl->flags & LS_POLLING))
	    ln7990CsrWrite (pDrvCtrl, 0, (lncsr_INEA | lncsr_TDMD));
	else
	    ln7990CsrWrite (pDrvCtrl, 0, lncsr_TDMD);
	}
    
    if (!(pDrvCtrl->flags & LS_POLLING))
	END_TX_SEM_GIVE (&pDrvCtrl->end);
    
    /* Bump the statistic counter. */
    END_ERR_ADD (&pDrvCtrl->end, MIB2_OUT_UCAST, +1);

    return (OK);

    }

/*******************************************************************************
*
* ln7990Ioctl - the driver I/O control routine
*
* Process an ioctl request.
*/

LOCAL int ln7990Ioctl
    (
    LN7990END_DEVICE *pDrvCtrl,
    int cmd,
    caddr_t data
    )
    {
    int error = 0;
    long value;

    switch ((UINT)cmd)
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
            bcopy ((char *)END_HADDR(&pDrvCtrl->end), (char *)data,
		    END_HADDR_LEN(&pDrvCtrl->end));
            break;

        case EIOCSFLAGS:
	    value = (long)data;
	    if (value < 0)
		{
		value = -value;
		value--;		/* HELP: WHY ??? */
		END_FLAGS_CLR (&pDrvCtrl->end, value);
		}
	    else
		{
		END_FLAGS_SET (&pDrvCtrl->end, value);
		}
	    ln7990Config (pDrvCtrl);
            break;
        case EIOCGFLAGS:
	    *(int *)data = END_FLAGS_GET(&pDrvCtrl->end);
            break;

	case EIOCPOLLSTART:
	    error = ln7990PollStart (pDrvCtrl);
	    break;

	case EIOCPOLLSTOP:
	    error = ln7990PollStop (pDrvCtrl);
	    break;

        case EIOCGMIB2:
            if (data == NULL)
                return (EINVAL);
            bcopy((char *)&pDrvCtrl->end.mib2Tbl, (char *)data,
                  sizeof(pDrvCtrl->end.mib2Tbl));
            break;
        case EIOCGFBUF:
            if (data == NULL)
                return (EINVAL);
            *(int *)data = LN_MIN_FBUF;
            break;
        case EIOCGMWIDTH:
            if (data == NULL)
                return (EINVAL);
            *(int *)data = pDrvCtrl->memWidth;
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
* ln7990Reset - hardware reset of chip (stop it)
*/

LOCAL void ln7990Reset
    (
    LN7990END_DEVICE  *pDrvCtrl
    )
    {
    ln7990CsrWrite (pDrvCtrl, 0, lncsr_STOP);
    }

/*******************************************************************************
*
* ln7990Csr0Read - read CSR 0 register
*
* Assumes normal operation, where the RAP register has been left selecting
* CSR0.
*/

LOCAL u_short ln7990Csr0Read
    (
    LN7990END_DEVICE * pDrvCtrl		/* driver control */
    )
    {
    USHORT result;

    SYS_IN_SHORT (pDrvCtrl, pDrvCtrl->pCsr, &result);

    return (result);		/* get contents of CSR */
    }

/*******************************************************************************
*
* ln7990CsrWrite - select and write a CSR register
*
*/

LOCAL void ln7990CsrWrite
    (
    LN7990END_DEVICE * pDrvCtrl,	/* driver control */
    int reg,				/* register to select */
    u_short value			/* value to write */
    )
    {
    int level;

    level = intLock ();
    SYS_OUT_SHORT (pDrvCtrl, pDrvCtrl->pRap, (u_short)reg);
    SYS_OUT_SHORT (pDrvCtrl, pDrvCtrl->pCsr, value);
    intUnlock (level);
    }

/*******************************************************************************
*
* ln7990RestartSetup - setup memory descriptors and turn on chip
*
* Initializes all the shared memory structures and turns on the chip.
*/

LOCAL STATUS ln7990RestartSetup
    (
    LN7990END_DEVICE *pDrvCtrl
    )
    {
    int 	rsize;
    int 	tsize;
    void * 	pTemp;

    /* reset the device */

    ln7990Reset (pDrvCtrl);

    /* setup Rx buffer descriptors  */

    rsize = pDrvCtrl->rringLen;

    /* 
     * setup Tx buffer descriptors  - 
     */

    tsize = pDrvCtrl->tringLen;

    /* setup the initialization block */

    pDrvCtrl->ib = (ln_ib *)pDrvCtrl->pShMem;

    /* HELP */
    swab ((char *) END_HADDR(&pDrvCtrl->end), pDrvCtrl->ib->lnIBPadr, 6);

    CACHE_PIPE_FLUSH ();

    pTemp = LN_CACHE_VIRT_TO_PHYS (pDrvCtrl->pRring);   /* point to Rx ring */
    LN_ADDR_TO_IB_RMD (pTemp, pDrvCtrl->ib, rsize);

    pTemp = LN_CACHE_VIRT_TO_PHYS (pDrvCtrl->pTring);   /* point to Tx ring */
    LN_ADDR_TO_IB_TMD (pTemp, pDrvCtrl->ib, tsize);

    DRV_LOG (DRV_DEBUG_LOAD, "Memory setup complete\n", 1, 2, 3, 4, 5, 6);

    /* reconfigure the device */

    ln7990Config (pDrvCtrl);

    return (OK); 
    }
     
/*******************************************************************************
*
* ln7990Restart - restart the device after a fatal error
*
* This routine takes care of all the messy details of a restart.  The device
* is reset and re-initialized.  The driver state is re-synchronized.
*/

LOCAL void ln7990Restart
    (
    LN7990END_DEVICE *pDrvCtrl
    )
    {
    ln7990Reset (pDrvCtrl);

    ln7990RestartSetup (pDrvCtrl);

    /* set the flags to indicate readiness */


    END_OBJ_READY (&pDrvCtrl->end,
                    IFF_UP | IFF_RUNNING | IFF_NOTRAILERS | IFF_BROADCAST
                    | IFF_MULTICAST);
    }

/******************************************************************************
*
* ln7990Config - reconfigure the interface under us.
*
* Reconfigure the interface setting promiscuous mode, and changing the
* multicast interface list.
*
* NOMANUAL
*/

LOCAL void ln7990Config
    (
    LN7990END_DEVICE *pDrvCtrl
    )
    {
    u_short stat;
    void* pTemp;
    int timeoutCount = 0;
    ln_rmd *pRmd;
    ln_tmd *pTmd;
    int ix;

    /* Set promiscuous mode if it's asked for. */

    if (END_FLAGS_GET(&pDrvCtrl->end) & IFF_PROMISC)
        {
        DRV_LOG (DRV_DEBUG_LOAD, "Setting promiscuous mode on!\n",
                 1, 2, 3, 4, 5, 6);
        
        /* chip will be in promiscuous mode */
        
        pDrvCtrl->ib->lnIBMode = 0x8000;
        }
    else
        {
        DRV_LOG (DRV_DEBUG_LOAD, "Setting promiscuous mode off!\n",
                 1, 2, 3, 4, 5, 6);
        pDrvCtrl->ib->lnIBMode = 0; /* chip will be in normal receive mode */
        }
    
    CACHE_PIPE_FLUSH ();

    ln7990CsrWrite (pDrvCtrl, 0, lncsr_STOP);     /* set the stop bit */

    /* Set up address filter for multicasting. */
	
    if (END_MULTI_LST_CNT(&pDrvCtrl->end) > 0)
	{
	ln7990AddrFilterSet (pDrvCtrl);
	}

    ln7990CsrWrite (pDrvCtrl, 3, pDrvCtrl->csr3B);

    pRmd = pDrvCtrl->pRring;
    for (ix = 0; ix < pDrvCtrl->rringSize; ix++, pRmd++)
        {
        pRmd->rbuf_bcnt = -(LN_BUFSIZ);		/* neg of buffer byte count */
        LN_CLEAN_RXD (pRmd);
        }
    pDrvCtrl->rmdIndex = 0;
    
    pTmd = pDrvCtrl->pTring;
    for (ix = 0; ix < pDrvCtrl->tringSize; ix++, pTmd++)
        {
        pTmd->tbuf_bcnt = 0;			/* no message byte count yet */
        pTmd->tbuf_err = 0;			/* no error status yet */
        LN_TMD_CLR_ERR (pTmd);
        }
    
    pDrvCtrl->tmdIndex = 0;
    pDrvCtrl->tmdIndexC = 0;
    
    /* Point the device to the initialization block */

    pTemp = LN_CACHE_VIRT_TO_PHYS (pDrvCtrl->ib);
    ln7990CsrWrite (pDrvCtrl, 2,
		       (u_short)(((u_long)pTemp >> 16) & 0x000000ff));
    ln7990CsrWrite (pDrvCtrl, 1, (u_long) pTemp);
    ln7990CsrWrite (pDrvCtrl, 0, lncsr_INIT);    /* init chip (read IB) */

    /* hang until Initialization DONe, ERRor, or timeout */

    while (((stat = ln7990Csr0Read (pDrvCtrl)) &
	    (lncsr_IDON | lncsr_ERR)) == 0)
        {
        if (timeoutCount++ > 0x10000)
	    break;
	DELAY (1000 * timeoutCount);
        }

    DRV_LOG (DRV_DEBUG_LOAD, "Timeoutcount %d\n", timeoutCount,
             2, 3, 4, 5, 6);

    /* log chip initialization failure */

    if ((stat & lncsr_ERR) || (timeoutCount >= 0x10000))
        {
	DRV_LOG (DRV_DEBUG_LOAD, "%s: Device initialization failed\n",
                 (int)END_DEV_NAME(&pDrvCtrl->end), 0,0,0,0,0);
        return;
        }

    if (!(pDrvCtrl->flags & LS_POLLING))
	{
	ln7990CsrWrite (pDrvCtrl, 0, lncsr_INEA | lncsr_STRT);
	}
    else
	{
	ln7990CsrWrite (pDrvCtrl, 0, lncsr_STRT);
	}
    }

/******************************************************************************
*
* ln7990AddrFilterSet - set the address filter for multicast addresses
*
* This routine goes through all of the multicast addresses on the list
* of addresses (added with the ln7990AddrAdd() routine) and sets the
* device's filter correctly.
*
* NOMANUAL
*/

LOCAL void ln7990AddrFilterSet
    (
    LN7990END_DEVICE *pDrvCtrl
    )
    {

    ETHER_MULTI* pCurr;
    ln_ib *pIb;
    u_char *pCp;
    u_char c;
    u_long crc;
    int len;
    int count;

    pIb = pDrvCtrl->ib;

    LN_ADDRF_CLEAR (pIb);

    pCurr = END_MULTI_LST_FIRST (&pDrvCtrl->end);

    while (pCurr != NULL)
	{
	/*
	 * One would think, given the AM7990 document's polynomial
	 * of 0x04c11db6, that this should be 0x6db88320 (the bit
	 * reversal of the AMD value), but that is not right.  See
	 * the BASIC listing: bit 0 (our bit 31) must then be set.
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

	LN_ADDRF_SET (pIb, crc);

	pCurr = END_MULTI_LST_NEXT(pCurr);
	}
    }

/*******************************************************************************
*
* ln7990PollReceive - routine to receive a packet in polled mode.
*
* This routine is called by a user to try and get a packet from the
* device.
*/

LOCAL STATUS ln7990PollReceive
    (
    LN7990END_DEVICE *pDrvCtrl,
    M_BLK_ID pMblk
    )
    {
    ln_rmd *pRmd;
    u_short stat;
    u_long phys;
    char* pPacket;
    int len;

    DRV_LOG (DRV_DEBUG_POLL_RX, "PRX b\n", 1, 2, 3, 4, 5, 6);

    /* Read the device status register */

    stat = ln7990Csr0Read (pDrvCtrl);

    /* Check for errors */

    if (stat & (lncsr_BABL | lncsr_MISS | lncsr_MERR))
        {
        ++pDrvCtrl->csr0Errs;
        DRV_LOG (DRV_DEBUG_POLL_RX, "PRX bad error\n", 1, 2, 3, 4, 5, 6);
	END_ERR_ADD (&pDrvCtrl->end, MIB2_IN_ERRS, +1);

        /* restart chip on fatal error */

        if (stat & lncsr_MERR)        /* memory error */
            {
	    END_FLAGS_CLR (&pDrvCtrl->end, (IFF_UP | IFF_RUNNING));
            DRV_LOG (DRV_DEBUG_POLL_RX, "PRX restart\n", 1, 2, 3, 4, 5, 6);
            ln7990Restart (pDrvCtrl);
	    return (EAGAIN);
            }
        }

    /*
     * clear receive interrupts, and clear any errors that may be set.
     */

    ln7990CsrWrite (pDrvCtrl, 0, ((stat &
            (lncsr_BABL|lncsr_CERR|lncsr_MISS|lncsr_MERR|
             lncsr_RINT))));


    /* Packet must be checked for errors. */

    pRmd = ln7990FullRMDGet (pDrvCtrl);

    if (pRmd == NULL)
	{
        DRV_LOG (DRV_DEBUG_POLL_RX, "PRX no rmd\n", 1, 2, 3, 4, 5, 6);
	return (EAGAIN);
	}

    if  (LN_RMD_ERR (pRmd))
	{
	END_ERR_ADD (&pDrvCtrl->end, MIB2_IN_ERRS, +1);
        DRV_LOG (DRV_DEBUG_POLL_RX, "PRX bad rmd\n", 1, 2, 3, 4, 5, 6);
	goto cleanRXD;
	}

    END_ERR_ADD (&pDrvCtrl->end, MIB2_IN_UCAST, +1);
    len = LN_PKT_LEN_GET (pRmd);		/* get packet length */

    /* Get pointer to packet */

    LN_RMD_TO_ADDR (pDrvCtrl->memBase, pRmd, phys);
    pPacket = (char *) LN_CACHE_PHYS_TO_VIRT (phys);
    LN_CACHE_INVALIDATE (pPacket, len);   /* make the packet coherent */

    /* Upper layer provides the buffer. */

    if ((pMblk->mBlkHdr.mLen < len) || (!(pMblk->mBlkHdr.mFlags & M_EXT)))
	{
        DRV_LOG (DRV_DEBUG_POLL_RX, "PRX bad mblk len:%d flags:%d\n",
                 pMblk->mBlkHdr.mLen, pMblk->mBlkHdr.mFlags, 3, 4, 5, 6);
	return (EAGAIN);
	}

    pMblk->mBlkHdr.mData += pDrvCtrl->offset;
    bcopy (pPacket, pMblk->mBlkHdr.mData, len);
    pMblk->mBlkHdr.mLen = len;
    pMblk->mBlkHdr.mFlags |= M_PKTHDR;
    pMblk->mBlkPktHdr.len = len;

    /* Done with descriptor, clean up and give it to the device. */
    cleanRXD:
    LN_CLEAN_RXD (pRmd);

    /* Flush the write pipe */

    CACHE_PIPE_FLUSH ();

    /* Advance our management index */

    pDrvCtrl->rmdIndex = (pDrvCtrl->rmdIndex + 1) & (pDrvCtrl->rringSize - 1);

    DRV_LOG (DRV_DEBUG_POLL_RX, "PRX ok\n", 1, 2, 3, 4, 5, 6);
    return (OK);


    }

/*******************************************************************************
*
* ln7990PollSend - routine to send a packet in polled mode.
*
* This routine is called by a user to try and send a packet on the
* device.
*/

LOCAL STATUS ln7990PollSend
    (
    LN7990END_DEVICE* pDrvCtrl,
    M_BLK_ID pMblk
    )
    {
    ln_tmd*	pTmd;
    void*       pTemp;
    int         len;
    int         oldLevel;
    u_short	stat;
    char*       pBuf = NULL;
    char*       pOrig = NULL;
    
    DRV_LOG (DRV_DEBUG_POLL_TX, "PTX b\n", 1, 2, 3, 4, 5, 6);

    /* See if next TXD is available */

    pTmd = pDrvCtrl->pTring + pDrvCtrl->tmdIndex;

    LN_CACHE_INVALIDATE (pTmd, TMD_SIZ);

    if ((pTmd->tbuf_stat & lntmd1_OWN) || (((pDrvCtrl->tmdIndex + 1) &
        (pDrvCtrl->tringSize - 1)) == pDrvCtrl->tmdIndexC))
        {
        DRV_LOG (DRV_DEBUG_POLL_TX, "Out of tmds.\n", 1, 2, 3, 4, 5, 6);
        if (!pDrvCtrl->txCleaning)
            netJobAdd ((FUNCPTR)ln7990ScrubTRing, (int) pDrvCtrl, 0, 0, 0, 0);
       return (EAGAIN);
        }

    /*
     * If we don't have alignment issues then we can transmit
     * directly from the M_BLK otherwise we have to copy.
     */
    
    if ((pDrvCtrl->offset == 0) && (pMblk->mBlkHdr.mNext == NULL))
        {
        len = max (ETHERSMALL, pMblk->m_len);
        LN_TMD_BUF_TO_ADDR(pTmd, pTemp, pMblk->m_data);
        DRV_LOG (DRV_DEBUG_POLL_TX, "offset == 0.\n", 1, 2, 3, 4, 5, 6);
        }
    else
        {
        DRV_LOG (DRV_DEBUG_POLL_TX, "offset != 0.\n", 1, 2, 3, 4, 5, 6);
        pOrig = pBuf = netClusterGet(pDrvCtrl->end.pNetPool,
                                     pDrvCtrl->pClPoolId);
        if (pBuf == NULL)
            {
            return (EAGAIN);
            }
        
        pBuf += pDrvCtrl->offset;
        
        len = netMblkToBufCopy (pMblk, pBuf, NULL);
        LN_TMD_BUF_TO_ADDR(pTmd, pTemp, pBuf);
        }

    len = max (len, ETHERSMALL);

    /* place a transmit request */

    oldLevel = intLock ();          /* disable ints during update */

    pTmd->tbuf_err = 0;                     /* clear buffer error status */
    pTmd->tbuf_bcnt = -len;                  /* negative message byte count */
    pTmd->tbuf_stat |= lntmd1_OWN | lntmd1_STP | lntmd1_ENP;

    /* Flush the write pipe */

    CACHE_PIPE_FLUSH ();

    intUnlock (oldLevel);   /* now ln7990Int won't get confused */

    /* Advance our management index */

    pDrvCtrl->tmdIndex = (pDrvCtrl->tmdIndex + 1) & (pDrvCtrl->tringSize - 1);

    /* kick start the transmitter, if selected */

    if (lnKickStartTx)
	ln7990CsrWrite (pDrvCtrl, 0, lncsr_TDMD);

    /* Bump the statistic counter. */

    END_ERR_ADD (&pDrvCtrl->end, MIB2_OUT_UCAST, +1);

    /* Flush the write pipe */

    CACHE_PIPE_FLUSH ();

    /* Read the device status register */

    stat = ln7990Csr0Read (pDrvCtrl);

    /*
     * Spin until we think we've sent it.  SInce we're single threaded
     * now it must be us who talked.
     */
    while (!(stat & lncsr_TINT))
        {
        stat = ln7990Csr0Read (pDrvCtrl);
        }

    /*
     * We now believe taht this frame has been transmitted.  SInce we
     * may have allocated/copied it we need to free it an then shout
     * hey nonny hey.
     */
    if (pOrig != NULL)
        {
        DRV_LOG (DRV_DEBUG_POLL_TX, "pOrig != NULL\n", 1, 2, 3, 4, 5, 6);
        netClFree(pDrvCtrl->end.pNetPool, pOrig);
        }
    
    if (!pDrvCtrl->txCleaning)
        {
        pDrvCtrl->txCleaning = TRUE;
        ln7990ScrubTRing (pDrvCtrl);
        }
    
    DRV_LOG (DRV_DEBUG_POLL_TX, "PTX e\n", 1, 2, 3, 4, 5, 6);

    return (OK);
    }

/*****************************************************************************
*
* ln7990MCastAddrAdd - add a multicast address for the device
*
* This routine adds a multicast address to whatever the driver
* is already listening for.  It then resets the address filter.
*/

LOCAL STATUS ln7990MCastAddrAdd
    (
    LN7990END_DEVICE *pDrvCtrl,
    char* pAddress
    )
    {

    int error;

    if ((error = etherMultiAdd (&pDrvCtrl->end.multiList,
		pAddress)) == ENETRESET)
	    ln7990Config (pDrvCtrl);

    return (OK);
    }

/*****************************************************************************
*
* ln7990MCastAddrDel - delete a multicast address for the device
*
* This routine removes a multicast address from whatever the driver
* is listening for.  It then resets the address filter.
*/

LOCAL STATUS ln7990MCastAddrDel
    (
    LN7990END_DEVICE* pDrvCtrl,
    char* pAddress
    )
    {

    int error;

    if ((error = etherMultiDel (&pDrvCtrl->end.multiList,
	     (char *)pAddress)) == ENETRESET)
	    ln7990Config (pDrvCtrl);

    return (OK);
    }

/*****************************************************************************
*
* ln7990MCastAddrGet - get the multicast address list for the device
*
* This routine gets the multicast list of whatever the driver
* is already listening for.
*/

LOCAL STATUS ln7990MCastAddrGet
    (
    LN7990END_DEVICE* pDrvCtrl,
    MULTI_TABLE* pTable
    )
    {

    int error;

    error = etherMultiGet (&pDrvCtrl->end.multiList, pTable);

    return (error);
    }

/*******************************************************************************
*
* ln7990Stop - stop the device
*
* This function calls BSP functions to disconnect interrupts and stop
* the device from operating in interrupt mode.
*
* RETURNS: OK or ERROR
*/

LOCAL STATUS ln7990Stop
    (
    LN7990END_DEVICE *pDrvCtrl
    )
    {
    STATUS result = OK;

    /* Stop the device. */

    ln7990CsrWrite (pDrvCtrl, 0, lncsr_STOP);     	/* set the stop bit */

    SYS_INT_DISCONNECT (pDrvCtrl, ln7990Int, (int)pDrvCtrl, &result);

    if (result == ERROR)
	{
	DRV_LOG (DRV_DEBUG_LOAD, "Could not diconnect interrupt!\n",
                 1, 2, 3, 4, 5, 6);
	}

    return (result);

    }

/******************************************************************************
*
* ln7990Unload - unload a driver from the system
*
* This function first brings down the device, and then frees any
* stuff that was allocated by the driver in the load function.
*/

LOCAL STATUS ln7990Unload
    (
    LN7990END_DEVICE* pDrvCtrl
    )
    {
    END_OBJECT_UNLOAD (&pDrvCtrl->end);

    /* Free the shared DMA memory. */
    if (pDrvCtrl->flags & LS_MEM_ALLOC_FLAG)
	cacheDmaFree (pDrvCtrl->pShMem);

    return (OK);
    }


/*******************************************************************************
*
* ln7990PollStart - start polled mode operations
*
* RETURNS: OK or ERROR.
*/

LOCAL STATUS ln7990PollStart
    (
    LN7990END_DEVICE* pDrvCtrl
    )

    {
    u_short        stat;
    int         oldLevel;

    /* Read the device status register */

    oldLevel = intLock ();          /* disable ints during update */

    stat = ln7990Csr0Read (pDrvCtrl);

    /* Rewrite the register minus the INEA bit to turn off interrupts */

    ln7990CsrWrite (pDrvCtrl, 0, ((stat &
		    (lncsr_BABL|lncsr_CERR|lncsr_MISS|lncsr_MERR|
		    lncsr_RINT|lncsr_TINT))));

    pDrvCtrl->flags |= LS_POLLING;

    intUnlock (oldLevel);   /* now ln7990Int won't get confused */

    DRV_LOG (DRV_DEBUG_POLL, "STARTED\n", 1, 2, 3, 4, 5, 6);

    ln7990Config (pDrvCtrl);	/* reconfigure device */ 

    return (OK);
    }

/*******************************************************************************
*
* ln7990PollStop - stop polled mode operations
*
* RETURNS: OK or ERROR.
*/

LOCAL STATUS ln7990PollStop
    (
    LN7990END_DEVICE* pDrvCtrl
    )
    {

    u_short        stat;
    int         oldLevel;

    oldLevel = intLock ();          /* disable ints during update */

    stat = ln7990Csr0Read (pDrvCtrl);

    /* Rewrite the register minus the INEA bit to turn off interrupts */

    ln7990CsrWrite (pDrvCtrl, 0, ((stat &
            (lncsr_BABL|lncsr_CERR|lncsr_MISS|lncsr_MERR|
		lncsr_RINT|lncsr_TINT)) | lncsr_INEA));


    pDrvCtrl->flags &= ~LS_POLLING;

    intUnlock (oldLevel);   /* now ln7990Int won't get confused */

    DRV_LOG (DRV_DEBUG_POLL, "STOPPED\n", 1, 2, 3, 4, 5, 6);

    ln7990Config (pDrvCtrl);	/* reconfigure device */
    
    return (OK);
    }


/******************************************************************************
*
* ln7990ScrubTRing - clean the transmit ring
*
* RETURNS: N/A
*
*/

LOCAL void ln7990ScrubTRing
    (
    LN7990END_DEVICE* pDrvCtrl
    )
    {

    int         oldLevel;
    ln_tmd* pTmd;
    ln_rmd* pRmd;
    
    pDrvCtrl->txCleaning = TRUE;

    while (pDrvCtrl->tmdIndexC != pDrvCtrl->tmdIndex)
        {

        /* Handle one recv packet if exists */
	if ((pRmd = ln7990FullRMDGet (pDrvCtrl)) != NULL)
	    {
            LN_CACHE_INVALIDATE (pRmd, RMD_SIZ);
            ln7990Recv (pDrvCtrl, pRmd);
            }

        /* disposal has not caught up */
        pTmd = pDrvCtrl->pTring + pDrvCtrl->tmdIndexC;

        /* if the buffer is still owned by LANCE, don't touch it */

        LN_CACHE_INVALIDATE (pTmd, TMD_SIZ);

        if (SHMEM_RD (&pTmd->tbuf_stat) & TMD_OWN)
            break;

        oldLevel = intLock();

        if (pDrvCtrl->freeRtn[pDrvCtrl->tmdIndexC] != NULL)
            {
            pDrvCtrl->freeRtn[pDrvCtrl->tmdIndexC]
                (pDrvCtrl->freeData[pDrvCtrl->tmdIndexC].arg1,
                 pDrvCtrl->freeData[pDrvCtrl->tmdIndexC].arg2); 
            pDrvCtrl->freeRtn[pDrvCtrl->tmdIndexC] = NULL;
            pDrvCtrl->freeData[pDrvCtrl->tmdIndexC].arg1 = NULL; 
            pDrvCtrl->freeData[pDrvCtrl->tmdIndexC].arg2 = NULL; 
            }

        intUnlock(oldLevel);
        
        /* now bump the tmd disposal index pointer around the ring */

        pDrvCtrl->tmdIndexC = (pDrvCtrl->tmdIndexC + 1) &
			      (pDrvCtrl->tringSize - 1);

        /*
         * lntmd1_ERR is an "OR" of LCOL, LCAR, UFLO or RTRY.
         * Note that BUFF is not indicated in lntmd1_ERR.
         * We should therefore check both lntmd1_ERR and lntmd3_BUFF
         * here for error conditions.
         */

        if ((SHMEM_RD (&pTmd->tbuf_stat) & lntmd1_ERR) ||
	    (SHMEM_RD (&pTmd->tbuf_err) & lntmd3_BUFF))
            {
	    END_ERR_ADD (&pDrvCtrl->end, MIB2_OUT_ERRS, +1);
	    END_ERR_ADD (&pDrvCtrl->end, MIB2_OUT_UCAST, -1);

            /*
	     * If error was due to excess collisions, bump the collision
             * counter.  The LANCE does not keep an individual counter of
             * collisions, so in this driver, the collision statistic is not
             * an accurate count of total collisions.
             */

	    /* assume DRTY bit not set */
            if (SHMEM_RD (&pTmd->tbuf_err) & lntmd3_RTRY)
		END_ERR_ADD (&pDrvCtrl->end, MIB2_OUT_ERRS, +1);

            /* check for no carrier */

            if (SHMEM_RD (&pTmd->tbuf_err) & TMD_LCAR)
		{
		DRV_LOG (DRV_DEBUG_TX, "%s: no carrier\n",
                         (int)END_DEV_NAME(&pDrvCtrl->end), 0,0,0,0,0);
		}

            /*
	     * Restart chip on fatal errors.
             * The following code handles the situation where the transmitter
             * shuts down due to an underflow error.  This is a situation that
             * will occur if the DMA cannot keep up with the transmitter.
             * It will occur if the LANCE is being held off from DMA access
             * for too long or due to significant memory latency.  DRAM
             * refresh or slow memory could influence this.  Many
             * implementations use a dedicated LANCE buffer.  This can be
             * static RAM to eliminate refresh conflicts; or dual-port RAM
             * so that the LANCE can have free run of this memory during its
             * DMA transfers.
             */

            if ((SHMEM_RD (&pTmd->tbuf_err) & lntmd3_BUFF) ||
		(SHMEM_RD (&pTmd->tbuf_err) & TMD_UFLO))
                {
                pDrvCtrl->end.flags &= ~(IFF_UP | IFF_RUNNING);
		ln7990Restart (pDrvCtrl);
                }
            }

	LN_TMD_CLR_ERR (pTmd);
        }
    pDrvCtrl->txCleaning = FALSE;
    }

