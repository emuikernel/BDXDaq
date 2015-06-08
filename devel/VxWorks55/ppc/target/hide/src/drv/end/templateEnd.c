/* templateEnd.c - template Enhanced Network Driver (END) */

/* Copyright 1984-2001 Wind River Systems, Inc. */

/*
TODO -	Remove the template modification history and begin a new history
	starting with version 01a and growing the history upward with
	each revision.

modification history
--------------------
01y,14jun02,dat  Fixing diab warnings
01x,15may02,dat  Update comments about polled entry points
01w,03nov01,dat  buffer/cache updates
01v,24sep01,pai  merge from sustaining branch to Veloce (SPR# 70553).
01u,06jun01,pai  templateLoad corrected for two-pass algorithm (SPR# 67836)
01t,09mar01,dat  64473, MuxLib was free'ing the END_OBJ structure. Fixed the
		 load function for driver name response.
01s,29sep00,dat  removing PHYS_TO_VIRT translation
01r,20sep00,dat  SPR 32058, allow for alternate intConnect rtn
01q,28aug00,stv	 corrected the handling of EIOCSFLAGS ioctl (SPR# 29423).
01p,02aug00,stv	 removed netMblkClChainFree() in Pollsend routine (SPR# 32885).
01o,11jun00,ham  removed reference to etherLib.
01n,27jan00,dat  fixed use of NULL
01m,12dec00,pai  fixed SYS_ENET_ADDR_GET and SYS_INT_ENABLE definitions (SPR
                 #62969).
01l,12dec00,pai  fixed various build errors (SPR #62962).
01k,12dec00,pai  merged in changes from HIT August Drivers Release.
01j,12oct99,dat  SPR 28492, fixed templateSend.
01i,29mar99,dat  documentation, SPR 26119. fixed .bS/.bE usage
01h,28feb99,pul  changed the prototype for END NET functions, to have END_OBJ
		 reference as the first argument rather than device object,
		 fix for SPR#24285
01g,17feb99,dat  documentation, removed beta warning
01f,29sep98,dat  Fixed problem in PollRx relating to SPR 22325.
01e,28sep98,dat  SPR 22325, system mode transition, plus fixed warnings
		 and template is now compilable (SPR 22204).
01d,17jul98,db   changed "holder" in templateParse from char** to char *.
		 fixed references to "holder" in templateParse(spr #21464).
01c,15oct97,gnn  revised to reflect the latest API changes.
01b,05may97,dat  added TODO's for documentation and macros.
01a,04feb97,gnn	 written.
*/

/*
DESCRIPTION

TODO - Describe the chip being used completely, even if it provides more
features than ethernet.

TODO - Describe the device's full ethernet capabilities, even if this driver
doesn't or can't utilize all of them.  Describe the features that the driver
does implement and any restrictions on their use.

TODO - Describe all macros that can be used to customize this driver.  All
accesses to chip registers should be done through redefineable macros.
In this example driver the macros TEMPLATE_OUT_SHORT and TEMPLATE_IN_SHORT
are sample macros to read/write data to a mock device.  If a device
communicates through formatted control blocks in shared memory, the
accesses to those control blocks should also be through redefinable
macros.

TODO - The following information describes the procedure an end user would
follow to integrate this new END device into a new BSP.  The procedure 
needs to be well documented.

This driver is easily customized for a BSP by modifying global pointers
to routines.  The routine pointers are declared below.  The code below
indicates the default values for each routine as well.  By modifying
these global pointer values, the BSP can change the behaviour of the driver.

.CS
    IMPORT STATUS (*templateIntConnectRtn) \
            (int level, FUNCTPR pFunc, int arg);
    IMPORT STATUS (*templateIntDisconnectRtn) \
            (int level, FUNCTPR pFunc, int arg);
    IMPORT STATUS (*templateIntEnableRtn) (int level);
    IMPORT STATUS (*templateEnetAddrGetRtn)(int unit, char *pResult);
    IMPORT STATUS (*templateOutShortRtn)(UINT addr, UINT value);
    IMPORT STATUS (*templateInShortRtn)(UINT addr, USHORT *pData);
    
    templateIntConnectRtn = intConnect; /@ must not be NULL @/
    templateIntDisconnectRtn = NULL;
    templateIntEnableRtn = NULL;
    templateEndAddrGetRtn = NULL;
    templateOutShortRtn = NULL;
    templateInShortRtn = NULL;
.CE

Excecpt for templateIntConnectRtn and templateIntEnableRtn, a NULL value
will result in the driver taking a default action.  For the int disconnect
function the default action is to do nothing at all.  For the short in and out
routines, the default is to assume memory mapped device registers and to
access them directly.  The default ethernet address get routine
provides an invalid ethernet address of all zeros (0:0:0:0:0:0).
If the BSP is willing to accept these default values no action at all
is needed.  To change the default value, the BSP should create an appropriate
routine and set the address into the global value before first use.  This
would normally be done at function sysHwInit2() time.

For Tornado 3.0 you need to pay attention to virtual physical address
translations which are important.  Use the cache lib macros to to
proper VIRT_TO_PHYS translation as part of generating the physical DMA
address for the device.  Avoid the use of PHYS_TO_VIRT translation as
it can be very time consuming.  If at all possible, the driver should
cache the virtual address of each data buffer used for DMA.

Prior to VxWorks AE 1.1, the muxLib function muxDevUnload() did a free
of the actual END_OBJ structure that was malloc'd during the driver
load routine.  Starting with VxWorks AE 1.1, this behaviour can be
changed.  If the second argument to END_OBJ_INIT points to the END_OBJ
then muxLib will free it during muxDevUnload.  If not, then muxDevUnload
will not free the allocated space.  Under this situation, it is assumed
that the driver unload routine has free'd the space itself.  This preserves
backward compatibility with older drivers that always specified the second
argument to END_OBJ_INIT() as a pointer to the END_OBJ structure.  This
template has been changed to use the new behaviour instead.

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
#include "netinet/if_ether.h"

#include "sys/socket.h"
#include "sys/ioctl.h"
#include "sys/times.h"

IMPORT	int endMultiLstCnt (END_OBJ* pEnd);

/* defines */

#define	DRV_NAME	"template"
#define DRV_NAME_LEN	(sizeof(DRV_NAME) + 1)

/* Configuration items */

#define END_BUFSIZ      (ETHERMTU + SIZEOF_ETHERHEADER + 6)
#define EH_SIZE		(14)
#define END_SPEED_10M	10000000	/* 10Mbs */
#define END_SPEED_100M	100000000	/* 100Mbs */
#define END_SPEED       END_SPEED_10M

/* Cache macros */

#define END_CACHE_INVALIDATE(address, len) \
        CACHE_DRV_INVALIDATE (pDrvCtrl->pCacheFuncs, (address), (len))

#define END_CACHE_VIRT_TO_PHYS(address) \
        CACHE_DRV_VIRT_TO_PHYS (pDrvCtrl->pCacheFuncs, (address))

/*
 * Default macro definitions for BSP interface.
 * These macros can be redefined in a wrapper file, to generate
 * a new module with an optimized interface.
 */

/* Macro to connect interrupt handler to vector */

#ifndef SYS_INT_CONNECT
    FUNCPTR	templateIntConnectRtn = intConnect;
#   define SYS_INT_CONNECT(pDrvCtrl,rtn,arg,pResult) \
	do { \
	*pResult = (*templateIntConnectRtn) ((VOIDFUNCPTR *) \
			INUM_TO_IVEC (pDrvCtrl->ivec), rtn, (int)arg); \
	} while (0)
#endif

/* Macro to disconnect interrupt handler from vector */

#ifndef SYS_INT_DISCONNECT
    STATUS	(*templateIntDisconnectRtn) \
			(int level, FUNCPTR rtn, int arg) = NULL;
#   define SYS_INT_DISCONNECT(pDrvCtrl,rtn,arg,pResult) \
	do { \
	if (templateIntDisconnectRtn != NULL) \
	    *pResult = (*templateIntDisconnectRtn)(pDrvCtrl->ivec, \
					rtn, (int)arg); \
	else \
	    *pResult = ERROR; \
	} while (0)
#endif


/* Macro to enable the appropriate interrupt level */

#ifndef SYS_INT_ENABLE
    STATUS	(*templateIntEnableRtn)(int level) = NULL;
#   define SYS_INT_ENABLE(pDrvCtrl) \
	do { \
	    if (templateIntEnableRtn != NULL) \
		(*templateIntEnableRtn) (pDrvCtrl->ilevel); \
	} while (0)
#endif

/* Macro to get the ethernet address from the BSP */

#ifndef SYS_ENET_ADDR_GET
    STATUS (*templateEnetAddrGetRtn)(int unit, char * pResult) = NULL;
#   define SYS_ENET_ADDR_GET(pDevice) \
	do { \
	if (templateEnetAddrGetRtn == NULL) \
	    bzero ((char *)&pDevice->enetAddr, 6); \
	else \
	    (*templateEnetAddrGetRtn)(pDevice->unit, \
					(char *)&pDevice->enetAddr); \
	} while (0)
#endif

/*
 * Macros to do a short (UINT16) access to the chip. Default
 * assumes a normal memory mapped device.
 */

#ifndef TEMPLATE_OUT_SHORT
    STATUS (*templateOutShortRtn)(UINT addr, UINT value) = NULL;
#   define TEMPLATE_OUT_SHORT(pDrvCtrl,addr,value) \
	do { \
	    if (templateOutShortRtn == NULL) \
		(*(USHORT *)addr = value) \
	    else \
		(*templateOutShortRtn)((UINT)addr, (UINT)value) \
	} while (0)
#endif

#ifndef TEMPLATE_IN_SHORT
    STATUS (*templateInShortRtn)(UINT addr, USHORT *pData) = NULL;
#   define TEMPLATE_IN_SHORT(pDrvCtrl,addr,pData) \
	do { \
	    if (templateInShortRtn == NULL) \
		(*pData = *addr) \
	    else \
		(*templateInShortRtn)((UINT)addr, pData) \
	} while (0)
#endif


/* A shortcut for getting the hardware address from the MIB II stuff. */

#define END_HADDR(pEnd)	\
		((pEnd)->mib2Tbl.ifPhysAddress.phyAddress)

#define END_HADDR_LEN(pEnd) \
		((pEnd)->mib2Tbl.ifPhysAddress.addrLength)


/* typedefs */

typedef struct
    {
    int len;
    char * pData;	/* data virtual address */
    char * pPhys;	/* dma physical address */
    } PKT;	/* A dummy DMA data packet */

#define TEMPLATE_PKT_LEN_GET(pPkt) (((PKT *)pPkt)->len)
#define TEMPLATE_PKT_VIRT_GET(pPkt) (((PKT *)pPkt)->pData)

typedef struct rfd
    {
    PKT *  pPkt;
    struct rfd * next;
    } RFD;	/* dummy rx frame descriptor */

typedef struct free_args
    {
    void* arg1;
    void* arg2;
    } FREE_ARGS;

/* The definition of the driver control structure */

typedef struct end_device
    {
    END_OBJ     end;			/* The class we inherit from. */
    int		unit;			/* unit number */
    int         ivec;			/* interrupt vector */
    int         ilevel;			/* interrupt level */
    char*       pShMem;			/* real ptr to shared memory */
    long	flags;			/* Our local flags. */
    UCHAR	enetAddr[6];		/* ethernet address */
    CACHE_FUNCS* pCacheFuncs;		/* cache function pointers */
    FUNCPTR     freeRtn[128];		/* Array of free routines. */
    struct free_args    freeData[128];  /* Array of free arguments */
					/* the free routines. */
    CL_POOL_ID	pClPoolId;		/* cluster pool */
    BOOL	rxHandling;		/* rcv task is scheduled */
    } END_DEVICE;

/*
 * This will only work if there is only a single unit, for multiple
 * unit device drivers these should be integrated into the END_DEVICE
 * structure.
 */

M_CL_CONFIG templateMclBlkConfig = 	/* network mbuf configuration table */
    {
    /*
    no. mBlks		no. clBlks	memArea		memSize
    -----------		----------	-------		-------
    */
    0, 			0, 		NULL, 		0
    };

CL_DESC templateClDescTbl [] = 	/* network cluster pool configuration table */
    {
    /*
    clusterSize			num	memArea		memSize
    -----------			----	-------		-------
    */
    {ETHERMTU + EH_SIZE + 2,	0,	NULL,		0}
    };

int templateClDescTblNumEnt = (NELEMENTS(templateClDescTbl));

/* Definitions for the flags field */

#define TEMPLATE_PROMISCUOUS	0x1
#define TEMPLATE_POLLING	0x2

/* Status register bits, returned by templateStatusRead() */

#define TEMPLATE_RINT		0x1	/* Rx interrupt pending */
#define TEMPLATE_TINT		0x2	/* Tx interrupt pending */
#define TEMPLATE_RXON		0x4	/* Rx on (enabled) */
#define TEMPLATE_VALID_INT	0x3	/* Any valid interrupt pending */

#define TEMPLATE_MIN_FBUF	(1536)	/* min first buffer size */

/* DEBUG MACROS */

#ifdef DEBUG
#   define LOGMSG(x,a,b,c,d,e,f) \
	if (endDebug) \
	    { \
	    logMsg (x,a,b,c,d,e,f); \
	    }
#else
#   define LOGMSG(x,a,b,c,d,e,f)
#endif /* ENDDEBUG */

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

int	templateDebug = 0x00;
int     templateTxInts=0;

#define DRV_LOG(FLG, X0, X1, X2, X3, X4, X5, X6)                        \
	if (templateDebug & FLG)                                        \
            logMsg(X0, X1, X2, X3, X4, X5, X6);

#define DRV_PRINT(FLG,X)                                                \
	if (templateDebug & FLG) printf X;

#else /*DRV_DEBUG*/

#define DRV_LOG(DBG_SW, X0, X1, X2, X3, X4, X5, X6)
#define DRV_PRINT(DBG_SW,X)

#endif /*DRV_DEBUG*/

/* LOCALS */

/* forward static functions */

LOCAL void	templateReset	(END_DEVICE *pDrvCtrl);
LOCAL void	templateInt	(END_DEVICE *pDrvCtrl);
LOCAL void	templateHandleRcvInt (END_DEVICE *pDrvCtrl);
LOCAL STATUS	templateRecv	(END_DEVICE *pDrvCtrl, PKT* pData);
LOCAL void	templateConfig	(END_DEVICE *pDrvCtrl);
LOCAL UINT	templateStatusRead (END_DEVICE *pDrvCtrl);

/* END Specific interfaces. */

/* This is the only externally visible interface. */

END_OBJ* 	templateLoad (char* initString);

LOCAL STATUS	templateStart	(END_DEVICE* pDrvCtrl);
LOCAL STATUS	templateStop	(END_DEVICE* pDrvCtrl);
LOCAL int	templateIoctl   (END_DEVICE* pDrvCtrl, int cmd, caddr_t data);
LOCAL STATUS	templateUnload	(END_DEVICE* pDrvCtrl);
LOCAL STATUS	templateSend	(END_DEVICE* pDrvCtrl, M_BLK_ID pBuf);

LOCAL STATUS	templateMCastAdd (END_DEVICE* pDrvCtrl, char* pAddress);
LOCAL STATUS	templateMCastDel (END_DEVICE* pDrvCtrl, char* pAddress);
LOCAL STATUS	templateMCastGet (END_DEVICE* pDrvCtrl,
				    MULTI_TABLE* pTable);
LOCAL STATUS	templatePollStart (END_DEVICE* pDrvCtrl);
LOCAL STATUS	templatePollStop (END_DEVICE* pDrvCtrl);
LOCAL STATUS	templatePollSend (END_DEVICE* pDrvCtrl, M_BLK_ID pBuf);
LOCAL STATUS	templatePollRcv (END_DEVICE* pDrvCtrl, M_BLK_ID pBuf);
LOCAL void	templateAddrFilterSet(END_DEVICE *pDrvCtrl);

LOCAL STATUS	templateParse	();
LOCAL STATUS	templateMemInit	();

/*
 * Declare our function table.  This is static across all device
 * instances.
 */

LOCAL NET_FUNCS templateFuncTable =
    {
    (FUNCPTR) templateStart,		/* Function to start the device. */
    (FUNCPTR) templateStop,		/* Function to stop the device. */
    (FUNCPTR) templateUnload,		/* Unloading function for the driver. */
    (FUNCPTR) templateIoctl,		/* Ioctl function for the driver. */
    (FUNCPTR) templateSend,		/* Send function for the driver. */
    (FUNCPTR) templateMCastAdd,		/* Multicast add function for the */
					/* driver. */
    (FUNCPTR) templateMCastDel,		/* Multicast delete function for */
					/* the driver. */
    (FUNCPTR) templateMCastGet,		/* Multicast retrieve function for */
					/* the driver. */
    (FUNCPTR) templatePollSend,		/* Polling send function */
    (FUNCPTR) templatePollRcv,		/* Polling receive function */
    endEtherAddressForm,		/* put address info into a NET_BUFFER */
    endEtherPacketDataGet,	 	/* get pointer to data in NET_BUFFER */
    endEtherPacketAddrGet  		/* Get packet addresses. */
    };

/*******************************************************************************
*
* templateLoad - initialize the driver and device
*
* This routine initializes the driver and the device to the operational state.
* All of the device specific parameters are passed in the initString.
*
* The string contains the target specific parameters like this:
*
* "register addr:int vector:int level:shmem addr:shmem size:shmem width"
*
* This routine can be called in two modes.  If it is called with an empty but
* allocated string, it places the name of this device into the <initString>
* and returns 0.
*
* If the string is allocated and not empty, the routine attempts to load
* the driver using the values specified in the string.
*
* RETURNS: An END object pointer, or NULL on error, or 0 and the name of the
* device if the <initString> was empty.
*/

END_OBJ* templateLoad
    (
    char* initString		/* String to be parsed by the driver. */
    )
    {
    END_DEVICE 	*pDrvCtrl;

    DRV_LOG (DRV_DEBUG_LOAD, "Loading template...\n", 1, 2, 3, 4, 5, 6);

    if (initString == NULL)
        {
        DRV_LOG (DRV_DEBUG_LOAD, "templateLoad: NULL initStr\r\n",
		0,0,0,0,0,0);
        return NULL;
        }
    
    if (initString[0] == EOS)
        {
        bcopy ((char *)DRV_NAME, initString, DRV_NAME_LEN);
        return NULL;
        }

    /* else initString is not blank, pass two ... */

    /* allocate the device structure */

    pDrvCtrl = (END_DEVICE *)calloc (sizeof (END_DEVICE), 1);
    if (pDrvCtrl == NULL)
	goto errorExit;

    /* parse the init string, filling in the device structure */

    if (templateParse (pDrvCtrl, initString) == ERROR)
	goto errorExit;

    /* Ask the BSP for the ethernet address. */

    SYS_ENET_ADDR_GET(pDrvCtrl);

    /* initialize the END and MIB2 parts of the structure */

    /*
     * The M2 element must come from m2Lib.h
     * This template is set up for a DIX type ethernet device.
     */

    if (END_OBJ_INIT (&pDrvCtrl->end, NULL, "template",
                      pDrvCtrl->unit, &templateFuncTable,
                      "END Template Driver.") == ERROR
     || END_MIB_INIT (&pDrvCtrl->end, M2_ifType_ethernet_csmacd,
                      &pDrvCtrl->enetAddr[0], 6, END_BUFSIZ,
                      END_SPEED)
		    == ERROR)
	goto errorExit;

    /* Perform memory allocation/distribution */

    if (templateMemInit (pDrvCtrl) == ERROR)
	goto errorExit;

    /* reset and reconfigure the device */

    templateReset (pDrvCtrl);
    templateConfig (pDrvCtrl);

    /* set the flags to indicate readiness */

    END_OBJ_READY (&pDrvCtrl->end,
		    IFF_NOTRAILERS | IFF_BROADCAST | IFF_MULTICAST);

    DRV_LOG (DRV_DEBUG_LOAD, "Done loading Template...", 1, 2, 3, 4, 5, 6);

    return (&pDrvCtrl->end);

errorExit:
    if (pDrvCtrl != NULL)
	free ((char *)pDrvCtrl);

    return NULL;
    }

/*******************************************************************************
*
* templateParse - parse the init string
*
* Parse the input string.  Fill in values in the driver control structure.
*
* The muxLib.o module automatically prepends the unit number to the user's
* initialization string from the BSP (configNet.h).
*
* .IP <unit>
* Device unit number, a small integer.
* .IP <vecNum>
* Interrupt vector number
* .IP <intLvl>
* Interrupt level
* .LP
*
* RETURNS: OK or ERROR for invalid arguments.
*/

LOCAL STATUS templateParse
    (
    END_DEVICE * pDrvCtrl,	/* device pointer */
    char * initString		/* information string */
    )
    {
    char*	tok;
    char*	pHolder = NULL;

    /* Parse the initString */

    /* Unit number. (from muxLib.o) */

    tok = strtok_r (initString, ":", &pHolder);
    if (tok == NULL)
	return ERROR;
    pDrvCtrl->unit = atoi (tok);

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

    DRV_LOG (DRV_DEBUG_LOAD, "Processed all arugments\n", 1, 2, 3, 4, 5, 6);

    return OK;
    }

/*******************************************************************************
*
* templateMemInit - initialize memory for the chip
*
* This routine is highly specific to the device.
*
* Design choices available:
*
* Use default system buffers, or create device specific buffer pools.
*
* Use contiguous buffers for device frame descriptors and the data, or
* use descriptor buffers separate from the data buffers.
*
* Use the same buffering scheme for Rx and Tx, or each side uses it's
* own buffering scheme.
*
* RETURNS: OK or ERROR.
*/

LOCAL STATUS templateMemInit
    (
    END_DEVICE * pDrvCtrl	/* device to be initialized */
    )
    {
    int count = 0;

    /* TODO - allocate and initialize any shared memory areas */

    /*
     * This is how we would set up an END netPool using netBufLib(1).
     * This code is pretty generic.
     */

    if ((pDrvCtrl->end.pNetPool = malloc (sizeof(NET_POOL))) == NULL)
        return (ERROR);

    /*
     * Note, Separating the descriptor from the data buffer can be
     * advantageous for architectures that need cached data buffers but
     * don't have snooping units to make the caches fully coherent.
     * It is a disadvantage for architectures that do have snooping, becuase
     * they need to do twice the number of netBufLib operations for each
     * data frame.  This template driver assumes the descriptor and data
     * buffer are contiguous.
     */

    /* number of driver Descriptor/data buffers */

    templateClDescTbl[0].clNum = 16;

    /* Setup mbuf/cluster block pool with more mbufs than clBlks */

    templateMclBlkConfig.clBlkNum = templateClDescTbl[0].clNum;
    templateMclBlkConfig.mBlkNum = templateMclBlkConfig.clBlkNum * 4;

    /* Calculate the total memory for all the M-Blks and CL-Blks. */

    templateMclBlkConfig.memSize = (templateMclBlkConfig.mBlkNum *
				    (MSIZE + sizeof (long))) +
			      (templateMclBlkConfig.clBlkNum *
				    (CL_BLK_SZ + sizeof(long)));

    /* allocate mbuf/Cluster blocks from normal memory */

    if ((templateMclBlkConfig.memArea = (char *) memalign (sizeof(long),
                                                  templateMclBlkConfig.memSize))
        == NULL)
        return (ERROR);

    /* Round cluster size up to a multiple of a cache line */

    templateClDescTbl[0].clSize =
		ROUND_UP(templateClDescTbl[0].clSize,_CACHE_ALIGN_SIZE);

    /* Calculate the memory size of all the clusters. */

    templateClDescTbl[0].memSize = (templateClDescTbl[0].clNum
				    * templateClDescTbl[0].clSize);

    /* Allocate the memory for the clusters from cache safe memory. */

    templateClDescTbl[0].memArea =
        (char *) cacheDmaMalloc (templateClDescTbl[0].memSize);

    /* cacheDmaMalloc memory is assumed to be cache line aligned ! */

    /* use cacheDmaFuncs for cacheDmaMalloc memory */

    pDrvCtrl->pCacheFuncs = &cacheDmaFuncs;

    if (templateClDescTbl[0].memArea == NULL)
        {
        DRV_LOG (DRV_DEBUG_LOAD, "system memory unavailable\n",
		1, 2, 3, 4, 5, 6);
        return (ERROR);
        }

    /* Initialize the memory pool. */

    if (netPoolInit(pDrvCtrl->end.pNetPool, &templateMclBlkConfig,
                    &templateClDescTbl[0], templateClDescTblNumEnt,
		    NULL) == ERROR)
        {
        DRV_LOG (DRV_DEBUG_LOAD, "Could not init buffering\n",
		1, 2, 3, 4, 5, 6);
        return (ERROR);
        }

    /*
     * If you need clusters to store received packets into then get them
     * here ahead of time.  This template driver only uses the cluster pool
     * for receiving.  Here it takes all the clusters out of the pool and
     * attaches them to the device.  The device will fill them with incoming
     * packets and trigger an interrupt.  This will schedule the
     * templateHandleRcvInt routine.  It will unlink the cluster and send
     * it to the stack.  When the stack is done with the data, the cluster
     * is freed and returned to the cluster pool to be used by the device
     * again.
     */

    if ((pDrvCtrl->pClPoolId =
		netClPoolIdGet (pDrvCtrl->end.pNetPool, sizeof (RFD), FALSE))
        == NULL)
        return (ERROR);

    while (count < templateClDescTbl[0].clNum)
        {
	char * pTempBuf;

        if ((pTempBuf = (char *)netClusterGet(pDrvCtrl->end.pNetPool,
                                              pDrvCtrl->pClPoolId))
            == NULL)
            {
            DRV_LOG (DRV_DEBUG_LOAD, "Could not get a buffer\n",
                     1, 2, 3, 4, 5, 6);
            return (ERROR);
            }

        /* TODO - Store the pointer in some appropriate structure. */

        }

    DRV_LOG (DRV_DEBUG_LOAD, "Memory setup complete\n", 1, 2, 3, 4, 5, 6);

    return OK;
    }

/*******************************************************************************
*
* templateStart - start the device
*
* This function calls BSP functions to connect interrupts and start the
* device running in interrupt mode.
*
* RETURNS: OK or ERROR
*
*/

LOCAL STATUS templateStart
    (
    END_DEVICE * pDrvCtrl	/* device ID */
    )
    {
    STATUS result;

    SYS_INT_CONNECT (pDrvCtrl, templateInt, (int)pDrvCtrl, &result);
    if (result == ERROR)
	return ERROR;

    DRV_LOG (DRV_DEBUG_LOAD, "Interrupt connected.\n", 1, 2, 3, 4, 5, 6);

    SYS_INT_ENABLE (pDrvCtrl);

    DRV_LOG (DRV_DEBUG_LOAD, "interrupt enabled.\n", 1, 2, 3, 4, 5, 6);

    END_FLAGS_SET (&pDrvCtrl->end, IFF_UP | IFF_RUNNING);

    /* TODO  - start the device, enabling interrupts */

    return (OK);
    }


/*******************************************************************************
*
* templateInt - handle controller interrupt
*
* This routine is called at interrupt level in response to an interrupt from
* the controller.
*
* RETURNS: N/A.
*/

LOCAL void templateInt
    (
    END_DEVICE  *pDrvCtrl	/* interrupting device */
    )
    {
    UCHAR stat;

    DRV_LOG (DRV_DEBUG_INT, "Got an interrupt!\n", 1, 2, 3, 4, 5, 6);

    /* Read the device status register */

    stat = templateStatusRead (pDrvCtrl);

    /* If false interrupt, return. */

    if (!(stat & TEMPLATE_VALID_INT)) /* test for valid interrupt */
	{
        return;	/* return immediately, no error message */
	}

    /*
     * enable interrupts, clear receive and/or transmit interrupts, and clear
     * any errors that may be set.
     */

    /* TODO - Check for errors */

    /* Have netTask handle any input packets */

    if ((stat & TEMPLATE_RINT) && (stat & TEMPLATE_RXON))
        {
        if (!(pDrvCtrl->rxHandling))
            {
            pDrvCtrl->rxHandling = TRUE;
            netJobAdd ((FUNCPTR)templateHandleRcvInt, (int)pDrvCtrl,
                       0,0,0,0);
            }
        }

    /* TODO - handle transmit interrupts */
    }


/*******************************************************************************
*
* templatePacketGet - get next received message
*
* Get next received message.  Returns NULL if none are
* ready.
*
* RETURNS: ptr to next packet, or NULL if none ready.
*/

LOCAL PKT* templatePacketGet
    (
    END_DEVICE  *pDrvCtrl	/* device structure */
    )
    {
    /*
     * TODO - get next received packet. Packet address must be a valid
     * virtual address, not a physical address.
     */

    return (PKT *)NULL;
    }


/*******************************************************************************
*
* templateRecv - process the next incoming packet
*
* Handle one incoming packet.  The packet is checked for errors.
*
* RETURNS: N/A.
*/

LOCAL STATUS templateRecv
    (
    END_DEVICE *pDrvCtrl,	/* device structure */
    PKT* pPkt			/* packet to process */
    )
    {
    int         len = 0;
    M_BLK_ID 	pMblk;
    char*       pCluster = NULL;
    char*       pNewCluster;
    CL_BLK_ID	pClBlk;


    /* Add one to our unicast data. */

    END_ERR_ADD (&pDrvCtrl->end, MIB2_IN_UCAST, +1);

    /*
     * We implicitly are loaning here, if copying is necessary this
     * step may be skipped, but the data must be copied before being
     * passed up to the protocols.
     */

    pNewCluster = netClusterGet (pDrvCtrl->end.pNetPool, pDrvCtrl->pClPoolId);

    if (pNewCluster == NULL)
        {
	DRV_LOG (DRV_DEBUG_RX, "Cannot loan!\n", 1, 2, 3, 4, 5, 6);
	END_ERR_ADD (&pDrvCtrl->end, MIB2_IN_ERRS, +1);
	goto cleanRXD;
        }

    /* Grab a cluster block to marry to the cluster we received. */

    if ((pClBlk = netClBlkGet (pDrvCtrl->end.pNetPool, M_DONTWAIT)) == NULL)
        {
        netClFree (pDrvCtrl->end.pNetPool, (UCHAR *)pNewCluster);
	DRV_LOG (DRV_DEBUG_RX, "Out of Cluster Blocks!\n", 1, 2, 3, 4, 5, 6);
	END_ERR_ADD (&pDrvCtrl->end, MIB2_IN_ERRS, +1);
	goto cleanRXD;
        }

    if ((pMblk = mBlkGet (pDrvCtrl->end.pNetPool, M_DONTWAIT, MT_DATA)) == NULL)
        {
        netClBlkFree (pDrvCtrl->end.pNetPool, pClBlk);
        netClFree (pDrvCtrl->end.pNetPool, (UCHAR *)pNewCluster);
	DRV_LOG (DRV_DEBUG_RX, "Out of M Blocks!\n", 1, 2, 3, 4, 5, 6);
	END_ERR_ADD (&pDrvCtrl->end, MIB2_IN_ERRS, +1);
	goto cleanRXD;
        }

    /* Join the cluster to the MBlock */

    netClBlkJoin (pClBlk, pCluster, len, NULL, 0, 0, 0);
    netMblkClJoin (pMblk, pClBlk);

    /* TODO - Invalidate any RFD dma buffers */

    /* TODO - Packet must be checked for errors. */

    END_ERR_ADD (&pDrvCtrl->end, MIB2_IN_UCAST, +1);
    len = TEMPLATE_PKT_LEN_GET (pPkt);
    pCluster = TEMPLATE_PKT_VIRT_GET (pPkt);

    pMblk->mBlkHdr.mLen = len;
    pMblk->mBlkHdr.mFlags |= M_PKTHDR;
    pMblk->mBlkPktHdr.len = len;

    /* make the packet data coherent */

    END_CACHE_INVALIDATE (pMblk->mBlkHdr.mData, len);

    DRV_LOG (DRV_DEBUG_RX, "Calling upper layer!\n", 1, 2, 3, 4, 5, 6);

    /* TODO - Done with processing, clean up and pass it up. */

    /* Call the upper layer's receive routine. */

    END_RCV_RTN_CALL(&pDrvCtrl->end, pMblk);

cleanRXD:

    return (OK);
    }

/*******************************************************************************
*
* templateHandleRcvInt - task level interrupt service for input packets
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

LOCAL void templateHandleRcvInt
    (
    END_DEVICE *pDrvCtrl	/* interrupting device */
    )
    {
    PKT* pPkt;

    do
        {
        pDrvCtrl->rxHandling = TRUE;

        while ((pPkt = templatePacketGet (pDrvCtrl)) != NULL)
            templateRecv (pDrvCtrl, pPkt);

        pDrvCtrl->rxHandling = FALSE;
        }
    while (templatePacketGet (pDrvCtrl) != NULL);
    }

/*******************************************************************************
*
* templateSend - the driver send routine
*
* This routine takes a M_BLK_ID sends off the data in the M_BLK_ID.
* The buffer must already have the addressing information properly installed
* in it.  This is done by a higher layer.  The last arguments are a free
* routine to be called when the device is done with the buffer and a pointer
* to the argument to pass to the free routine.
*
* RETURNS: OK, ERROR, or END_ERR_BLOCK.
*/

LOCAL STATUS templateSend
    (
    END_DEVICE * pDrvCtrl,	/* device ptr */
    M_BLK_ID     pMblk		/* data to send */
    )
    {
    int         oldLevel = 0;
    BOOL        freeNow = TRUE;

    /*
     * Obtain exclusive access to transmitter.  This is necessary because
     * we might have more than one stack transmitting at once.
     */

    if (!(pDrvCtrl->flags & TEMPLATE_POLLING))
	END_TX_SEM_TAKE (&pDrvCtrl->end, WAIT_FOREVER);

    /* TODO - Flush the data buffer(s), if it might be cached */

    /* TODO - If necessary, get a Tx Frame Descriptor (TFD) */

    /*
     * TODO - If resources are not available,
     * release the semaphore and return END_ERR_BLOCK.
     * Do not free packet
     */

    /* TODO - Translate buffer virtual address to a remove bus physical addr */

    /* place a transmit request */

    if (!(pDrvCtrl->flags & TEMPLATE_POLLING))
        oldLevel = intLock ();	/* protect templateInt */

    /* TODO - initiate device transmit, FLUSH TFD */

    /* Advance our management index(es) */

    if (!(pDrvCtrl->flags & TEMPLATE_POLLING))
	END_TX_SEM_GIVE (&pDrvCtrl->end);

    if (!(pDrvCtrl->flags & TEMPLATE_POLLING))
        intUnlock (oldLevel);

    /* Bump the statistics counters. */

    END_ERR_ADD (&pDrvCtrl->end, MIB2_OUT_UCAST, +1);

    /*
     * Cleanup.  If the driver copied the data from the mblks to a different
     * buffer, then free the mblks now.  Otherwise, free the mblk chain
     * after the device is finished with the TFD.
     */

    if (freeNow)
        netMblkClChainFree (pMblk);

    return (OK);
    }

/*******************************************************************************
*
* templateIoctl - the driver I/O control routine
*
* Process an ioctl request.
*
* RETURNS: A command specific response, usually OK or ERROR.
*/

LOCAL int templateIoctl
    (
    END_DEVICE * pDrvCtrl,	/* device receiving command */
    int cmd,			/* ioctl command code */
    caddr_t data		/* command argument */
    )
    {
    int error = 0;
    long value;

    switch ((unsigned)cmd)
        {
        case EIOCSADDR:		/* set MAC address */
	    if (data == NULL)
		return (EINVAL);
            bcopy ((char *)data, (char *)END_HADDR(&pDrvCtrl->end),
		   END_HADDR_LEN(&pDrvCtrl->end));
            break;

        case EIOCGADDR:		/* get MAC address */
	    if (data == NULL)
		return (EINVAL);
            bcopy ((char *)END_HADDR(&pDrvCtrl->end), (char *)data,
		    END_HADDR_LEN(&pDrvCtrl->end));
            break;

        case EIOCSFLAGS:	/* set (or clear) flags */
	    value = (long)data;
	    if (value < 0)
		{
		value = -value;
		value--;
		END_FLAGS_CLR (&pDrvCtrl->end, value);
		}
	    else
		{
		END_FLAGS_SET (&pDrvCtrl->end, value);
		}
	    templateConfig (pDrvCtrl);
            break;

        case EIOCGFLAGS:	/* get flags */
	    *(int *)data = END_FLAGS_GET(&pDrvCtrl->end);
            break;

	case EIOCPOLLSTART:	/* Begin polled operation */
	    templatePollStart (pDrvCtrl);
	    break;

	case EIOCPOLLSTOP:	/* End polled operation */
	    templatePollStop (pDrvCtrl);
	    break;

        case EIOCGMIB2233:
        case EIOCGMIB2:		/* return MIB information */
            if (data == NULL)
                return (EINVAL);
            bcopy((char *)&pDrvCtrl->end.mib2Tbl, (char *)data,
                  sizeof(pDrvCtrl->end.mib2Tbl));
            break;

        case EIOCGFBUF:		/* return minimum First Buffer for chaining */
            if (data == NULL)
                return (EINVAL);
            *(int *)data = TEMPLATE_MIN_FBUF;
            break;

        case EIOCGHDRLEN:
            if (data == NULL)
                return (EINVAL);
            *(int *)data = EH_SIZE;
            break;

        default:		/* unknown request */
            error = EINVAL;
        }

    return (error);
    }

/******************************************************************************
*
* templateConfig - reconfigure the interface under us.
*
* Reconfigure the interface setting promiscuous mode, and changing the
* multicast interface list.
*
* RETURNS: N/A.
*/

LOCAL void templateConfig
    (
    END_DEVICE *pDrvCtrl	/* device to be re-configured */
    )
    {

    /* Set promiscuous mode if it's asked for. */

    if (END_FLAGS_GET(&pDrvCtrl->end) & IFF_PROMISC)
	{
	DRV_LOG (DRV_DEBUG_IOCTL, "Setting promiscuous mode on!\n",
		1, 2, 3, 4, 5, 6);
	}
    else
	{
	DRV_LOG (DRV_DEBUG_IOCTL, "Setting promiscuous mode off!\n",
		1, 2, 3, 4, 5, 6);
	}

    /* Set up address filter for multicasting. */

    if (END_MULTI_LST_CNT(&pDrvCtrl->end) > 0)
	{
	templateAddrFilterSet (pDrvCtrl);
	}

    /* TODO - shutdown device completely */

    /* TODO - reset all device counters/pointers, etc. */

    /* TODO - initialise the hardware according to flags */

    return;
    }

/******************************************************************************
*
* templateAddrFilterSet - set the address filter for multicast addresses
*
* This routine goes through all of the multicast addresses on the list
* of addresses (added with the endAddrAdd() routine) and sets the
* device's filter correctly.
*
* RETURNS: N/A.
*/

LOCAL void templateAddrFilterSet
    (
    END_DEVICE *pDrvCtrl	/* device to be updated */
    )
    {
    ETHER_MULTI* pCurr;

    pCurr = END_MULTI_LST_FIRST (&pDrvCtrl->end);

    while (pCurr != NULL)
	{
        /* TODO - set up the multicast list */

	pCurr = END_MULTI_LST_NEXT(pCurr);
	}

    /* TODO - update the device filter list */
    }

/*******************************************************************************
*
* templatePollRcv - routine to receive a packet in polled mode.
*
* Polled mode operation takes place without any kernel or other OS
* services available.  Use extreme care to insure that this code does not
* call any kernel services.  Polled mode is only for WDB system mode use.
* Kernel services, semaphores, tasks, etc, are not available during WDB
* system mode.
*
* The WDB agent polls the device constantly looking for new data.  Typically
* the device has a ring of RFDs to receive incoming packets.  This routine
* examines the ring for any new data and copies it to the provided mblk.
* The concern here is to keep the device supplied with empty buffers at all
* time.
*
* RETURNS: OK upon success.  EAGAIN is returned when no packet is available.
* A return of ERROR indicates a hardware fault or no support for polled mode
* at all.
*/

LOCAL STATUS templatePollRcv
    (
    END_DEVICE * pDrvCtrl,	/* device to be polled */
    M_BLK_ID      pMblk		/* ptr to buffer */
    )
    {
    u_short stat;
    char* pPacket;
    int len = 0;

    /* TODO - If no packet is available return immediately */

    stat = templateStatusRead (pDrvCtrl);
    if (!(stat & TEMPLATE_RINT))
	{
        return (EAGAIN);
	}

    /* Upper layer must provide a valid buffer. */

    if ((pMblk->mBlkHdr.mLen < len) || (!(pMblk->mBlkHdr.mFlags & M_EXT)))
	{
	return (EAGAIN);
	}

    /* TODO - Invalidate any inbound RFD DMA buffers */

    /* TODO - clear any status bits that may be set. */

    /* TODO - Check packet and device for errors */

    /* Get packet and  length from device buffer/descriptor */

    pPacket = NULL;	/* DUMMY CODE */
    len = 64;		/* DUMMY CODE */

    /* TODO - Invalidate any inbound data DMA buffers */

    /* TODO - Process device packet into net buffer */

    bcopy (pPacket, pMblk->m_data, len);
    pMblk->mBlkHdr.mFlags |= M_PKTHDR;	/* set the packet header */
    pMblk->mBlkHdr.mLen = len;		/* set the data len */
    pMblk->mBlkPktHdr.len = len;	/* set the total len */

    /*
     * TODO - Done with this packet, clean up device. If needed
     * setup a new RFD/buffer for incoming packets
     */

    return (OK);
    }

/*******************************************************************************
*
* templatePollSend - routine to send a packet in polled mode.
*
* Polled mode operation takes place without any kernel or other OS
* services available.  Use extreme care to insure that this code does not
* call any kernel services.  Polled mode is only for WDB system mode use.
* Kernel services, semaphores, tasks, etc, are not available during WDB
* system mode.
*
* A typical implementation is to set aside a fixed buffer for polled send
* operation.  Copy the mblk data to the buffer and pass the fixed buffer
* to the device.  Performance is not a consideration for polled operations.
*
* An alternate implementation is a synchronous one.  The routine accepts
* user data but does not return until the data has actually been sent.  If an
* error occurs, the routine returns EAGAIN and the user will retry the request.
*
* If the device returns OK, then the data has been sent and the user may free
* the associated mblk.  The driver never frees the mblk in polled mode.
* The calling routine will free the mblk upon success.
*
* RETURNS: OK upon success.  EAGAIN if device is busy or no resources.
* A return of ERROR indicates a hardware fault or no support for polled mode
* at all.
*/

LOCAL STATUS templatePollSend
    (
    END_DEVICE* 	pDrvCtrl,	/* device to be polled */
    M_BLK_ID    pMblk	/* packet to send */
    )
    {
    int         len;
    u_short	stat;
    static	char txBuff [END_BUFSIZ];

    /* TODO - test to see if tx is busy */

    stat = templateStatusRead (pDrvCtrl);	/* dummy code */
    if ((stat & TEMPLATE_TINT) == 0)
	return ((STATUS) EAGAIN);

    /* TODO - Copy data from mblk */

    len = netMblkToBufCopy (pMblk, &txBuff[0], NULL);
    len = max (ETHERSMALL, len);

    /* TODO - If necessary get a TFD */

    /* TODO - Setup the TFD, with bus physical addresses, Flush it */

    /* TODO - Handoff TFD/data to device */

    return (OK);
    }

/*****************************************************************************
*
* templateMCastAdd - add a multicast address for the device
*
* This routine adds a multicast address to whatever the driver
* is already listening for.  It then resets the address filter.
*
* RETURNS: OK or ERROR.
*/

LOCAL STATUS templateMCastAdd
    (
    END_DEVICE *pDrvCtrl,		/* device pointer */
    char* pAddress	/* new address to add */
    )
    {
    int error;

    if ((error = etherMultiAdd (&pDrvCtrl->end.multiList,
		pAddress)) == ENETRESET)
	templateConfig (pDrvCtrl);

    return (OK);
    }

/*****************************************************************************
*
* templateMCastDel - delete a multicast address for the device
*
* This routine removes a multicast address from whatever the driver
* is listening for.  It then resets the address filter.
*
* RETURNS: OK or ERROR.
*/

LOCAL STATUS templateMCastDel
    (
    END_DEVICE *pDrvCtrl,		/* device pointer */
    char* pAddress		/* address to be deleted */
    )
    {
    int error;

    if ((error = etherMultiDel (&pDrvCtrl->end.multiList,
	     (char *)pAddress)) == ENETRESET)
	templateConfig (pDrvCtrl);

    return (OK);
    }

/*****************************************************************************
*
* templateMCastGet - get the multicast address list for the device
*
* This routine gets the multicast list of whatever the driver
* is already listening for.
*
* RETURNS: OK or ERROR.
*/

LOCAL STATUS templateMCastGet
    (
    END_DEVICE *pDrvCtrl,			/* device pointer */
    MULTI_TABLE* pTable		/* address table to be filled in */
    )
    {
    return (etherMultiGet (&pDrvCtrl->end.multiList, pTable));
    }

/*******************************************************************************
*
* templateStop - stop the device
*
* This function calls BSP functions to disconnect interrupts and stop
* the device from operating in interrupt mode.
*
* RETURNS: OK or ERROR.
*/

LOCAL STATUS templateStop
    (
    END_DEVICE *pDrvCtrl	/* device to be stopped */
    )
    {
    STATUS result = OK;

    END_FLAGS_CLR (&pDrvCtrl->end, IFF_UP | IFF_RUNNING);

    /* TODO - stop/disable the device. */

    SYS_INT_DISCONNECT (pDrvCtrl, (FUNCPTR)templateInt, (int)pDrvCtrl, &result);
    if (result == ERROR)
	{
	DRV_LOG (DRV_DEBUG_LOAD, "Could not disconnect interrupt!\n",
		1, 2, 3, 4, 5, 6);
	}

    return (result);
    }

/******************************************************************************
*
* templateUnload - unload a driver from the system
*
* This function first brings down the device, and then frees any
* stuff that was allocated by the driver in the load function.
*
* RETURNS: OK or ERROR.
*/

LOCAL STATUS templateUnload
    (
    END_DEVICE* pDrvCtrl	/* device to be unloaded */
    )
    {
    END_OBJECT_UNLOAD (&pDrvCtrl->end);

    /* TODO - Free any special allocated memory */

    /* New: free the END_OBJ structure allocated during templateLoad() */

    cfree ((char *)pDrvCtrl);

    return (OK);
    }


/*******************************************************************************
*
* templatePollStart - start polled mode operations
*
* RETURNS: OK or ERROR.
*/

LOCAL STATUS templatePollStart
    (
    END_DEVICE * pDrvCtrl	/* device to be polled */
    )
    {
    int         oldLevel;

    oldLevel = intLock ();          /* disable ints during update */

    /* TODO - turn off interrupts */

    pDrvCtrl->flags |= TEMPLATE_POLLING;

    intUnlock (oldLevel);   /* now templateInt won't get confused */

    DRV_LOG (DRV_DEBUG_POLL, "STARTED\n", 1, 2, 3, 4, 5, 6);

    templateConfig (pDrvCtrl);	/* reconfigure device */

    return (OK);
    }

/*******************************************************************************
*
* templatePollStop - stop polled mode operations
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

LOCAL STATUS templatePollStop
    (
    END_DEVICE * pDrvCtrl	/* device to be polled */
    )
    {
    int         oldLevel;

    oldLevel = intLock ();	/* disable ints during register updates */

    /* TODO - re-enable interrupts */

    pDrvCtrl->flags &= ~TEMPLATE_POLLING;

    intUnlock (oldLevel);

    templateConfig (pDrvCtrl);

    DRV_LOG (DRV_DEBUG_POLL, "STOPPED\n", 1, 2, 3, 4, 5, 6);

    return (OK);
    }

/*******************************************************************************
*
* templateReset - reset device
*
* RETURNS: N/A.
*/

LOCAL void templateReset
    (
    END_DEVICE *pDrvCtrl
    )
    {
    /* TODO - reset the controller */
    }

/*******************************************************************************
*
* templateStatusRead - get current device state/status
*
* RETURNS: status bits.
*/

LOCAL UINT templateStatusRead
    (
    END_DEVICE *pDrvCtrl
    )
    {
    /* TODO - read and return status bits/register */
    return (0);
    }
