/* nicEvbEnd.c - National Semiconductor ST-NIC Chip network interface driver */

/* Copyright 1984-2002 Wind River Systems, Inc. */
#include "copyright_wrs.h"

/*
modification history
--------------------
01j,16may02,wap  Release dmaSem before passing packets up the stack (SPR
                 #77005)
01i,02aug00,stv	 removed netMblkClChainFree() in Pollsend routine (SPR# 32885).
01h,11jun00,ham  removed reference to etherLib.
01g,01dec99,stv  freed mBlk chain before returning ERROR (SPR #28492).
01f,10feb98,sut  added poll mode support
01e,09feb98,sut  added multicast support
01d,08feb98,sut  got working
01c,25jan98,sut  working with initMem
01b,21jan98,sut  investcating descritor
01a,20jan99,sut  from netif/if_nicEvb.c
*/

/*
This module implements the National Semiconductor 83902A ST-NIC Ethernet
network interface driver.

This driver is non-generic and is for use on the IBM EVB403 board.  
The driver must be given several target-specific parameters.  These
parameters, and the mechanisms used to communicate them to the driver, are
detailed below.


BOARD LAYOUT
This device is on-board.  No jumpering diagram is necessary.

EXTERNAL INTERFACE
The only external interface is the nicEvbEndLoad() routine, which expects
the <initString> parameter as input.  This parameter passes in a 
colon-delimited string of the format:

<unit>:<nic_addr>:<int_vector>:<int_level>

The nicEvbEndLoad() function uses strtok() to parse the string.

TARGET-SPECIFIC PARAMETERS
.IP <unit>
A convenient holdover from the former model.  This parameter is used only
in the string name for the driver.
.IP <nic_addr>
Base address for NIC chip
.IP <int_vector>
Configures the NIC device to generate hardware interrupts for various events
within the device. Thus, it contains an interrupt handler routine.  The driver
calls sysIntConnect() to connect its interrupt handler to the interrupt vector.
.IP <int_level>
This parameter is passed to an external support routine, sysLanIntEnable(),
which is described below in "External Support Requirements." This routine 
is called during as part of driver's initialization.  It handles any 
board-specific operations required to allow the servicing of a NIC 
interrupt on targets that use additional interrupt controller devices to 
help organize and service the various interrupt sources.  This parameter 
makes it possible for this driver to avoid all board-specific knowledge of 
such devices.  
.iP "device restart/reset delay"
The global variable nicRestartDelay (UINT32), defined in this file, 
should be initialized in the BSP sysHwInit() routine. nicRestartDelay is 
used only with PowerPC platform and is equal to the number of time base 
increments which makes for 1.6 msec. This corresponds to the delay necessary 
to respect when restarting or resetting the device.
.LP

EXTERNAL SUPPORT REQUIREMENTS
This driver requires several external support functions, defined as macros:
.CS
    SYS_INT_CONNECT(pDrvCtrl, routine, arg)
    SYS_INT_DISCONNECT (pDrvCtrl, routine, arg)
    SYS_INT_ENABLE(pDrvCtrl)
.CE

There are default values in the source code for these macros.  They presume
memory-mapped accesses to the device registers and the normal intConnect(),
and intEnable() BSP functions.  The first argument to each is the device
controller structure. Thus, each has access back to all the device-specific
information.  Having the pointer in the macro facilitates the addition 
of new features to this driver.


SYSTEM RESOURCE USAGE
When implemented, this driver requires the following system resources:

    - one mutual exclusion semaphore
    - one interrupt vector

SEE ALSO: muxLib
*/

/* configurations */

#define	NIC_INSTRUMENT			/* instrument the driver */
#undef	DRV_DEBUG			/* log debug messages */

#if	(CPU_FAMILY == PPC)
#define	NIC_USE_PPCTIMEBASE
#else   /* CPU_FAMILY == PPC */
#undef	NIC_USE_PPCTIMEBASE
#endif	/* CPU_FAMILY == PPC */

/* 
 * NIC_FASTER, if defined, improved the driver throuput, however the driver
 * seems to be less reliable. Left undefined for now.
 */

#undef	NIC_FASTER			/* improve throuput but less stable */


/* includes */

#include "vxWorks.h"

#include "wdLib.h"
#include "iv.h"
#include "vme.h"
#include "net/mbuf.h"
#include "net/unixLib.h"
#include "net/protosw.h"
#include "sys/socket.h"
#include "sys/ioctl.h"
#include "errno.h"
#include "memLib.h"
#include "intLib.h"
#include "net/route.h"
#include "iosLib.h"
#include "errnoLib.h"

#ifdef DRV_DEBUG
#include "logLib.h"
#endif

#include "cacheLib.h"
#include "netLib.h"
#include "stdio.h"
#include "stdlib.h"
#include "sysLib.h"



#include "net/systm.h"
#include "sys/times.h"
#include "net/if_subr.h"
#include "netinet/if_ether.h"

#include "drv/end/nicEvbEnd.h"

#undef	ETHER_MAP_IP_MULTICAST
#include "etherMultiLib.h"
#include "end.h"
#include "endLib.h"

#include "lstLib.h"
#include "semLib.h"


/* defines */

#define NIC_EVB_DEV_NAME	"nicEvb"
#define NIC_EVB_DEV_NAME_LEN	7
#define NIC_EVB_EADR_LEN	6
#define NIC_EVB_SPEED		10000000	/* HELP */
#define NIC_TRANS_TIMEOUT	400		/* times to poll for end of Tx */
#define NIC_CRC_POLY		0x04c11db7

/* debug macros */

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
#define	DRV_DEBUG_MB		0x40000

#ifdef LOCAL
#undef LOCAL
#define LOCAL

NET_POOL nicNetPool;

#endif


/* int	nicEvbDebug = DRV_DEBUG_LOAD | DRV_DEBUG_TX | DRV_DEBUG_RX; */
int	nicEvbDebug = DRV_DEBUG_POLL;

#define DRV_LOG(FLG, X0, X1, X2, X3, X4, X5, X6)				\
	if (nicEvbDebug & FLG)							\
            logMsg(X0, X1, X2, X3, X4, X5, X6);

#define DRV_PRINT(FLG, X0, X1, X2, X3, X4, X5, X6)				\
	if (nicEvbDebug & FLG)							\
            printf(X0, X1, X2, X3, X4, X5, X6);

#else /*DRV_DEBUG*/

#define DRV_LOG(DBG_SW, X0, X1, X2, X3, X4, X5, X6)
#define DRV_PRINT(DBG_SW,X)

#endif /*DRV_DEBUG*/



/*
 * Default macro definitions for BSP interface.
 * These macros can be redefined in a wrapper file, to generate
 * a new module with an optimized interface.
 */


#ifndef SYS_INT_CONNECT
#define SYS_INT_CONNECT(pDrvCtrl,rtn,arg,pResult) \
    {\
    IMPORT STATUS sysIntConnect(); \
    *pResult = intConnect ((VOIDFUNCPTR *)INUM_TO_IVEC (pDrvCtrl->ivec), \
			     rtn, (int)arg); \
    }
#endif /*SYS_INT_CONNECT*/

#ifndef SYS_INT_DISCONNECT
#define SYS_INT_DISCONNECT(pDrvCtrl,rtn,arg,pResult) \
    { \
    *pResult = OK; \
    }
#endif /*SYS_INT_DISCONNECT*/

#ifndef SYS_INT_ENABLE
#define SYS_INT_ENABLE(ivec) \
    { \
    IMPORT void sysLanIntEnable();      \
    sysLanIntEnable (ivec);		\
    }
#endif /*SYS_INT_ENABLE*/


#ifndef SYS_ENET_ADDR_GET
#define SYS_ENET_ADDR_GET(pDrvCtrl, pAddress) \
    { \
    IMPORT sysEnetAddrGet (int, char*);\
    nicEvbEnetAddrGet (pDrvCtrl, pAddress); \
    }
#endif /* SYS_ENET_ADDR_GET */

#define DRV_FLAGS_SET(setBits)                                          \
	(pDrvCtrl->flags |= (setBits))

#define DRV_FLAGS_ISSET(setBits)                                        \
	(pDrvCtrl->flags & (setBits))

#define DRV_FLAGS_CLR(clrBits)                                          \
	(pDrvCtrl->flags &= ~(clrBits))

#define DRV_FLAGS_GET()                                                 \
        (pDrvCtrl->flags)

#define NIC_IS_IN_POLL_MODE()\
	((DRV_FLAGS_GET() & NIC_FLAG_POLL) == NIC_FLAG_POLL)

/* A shortcut for getting the hardware address from the MIB II stuff. */
#define END_HADDR(pEnd)	\
		((pEnd)->mib2Tbl.ifPhysAddress.phyAddress)

#define END_HADDR_LEN(pEnd) \
		((pEnd)->mib2Tbl.ifPhysAddress.addrLength)

#define END_FLAGS_ISSET(pEnd, setBits) \
            ((pEnd)->flags & (setBits))



/* typedefs */

typedef struct rx_hdr
    {
    UINT8		status;		/* status of packet */
    UINT8		nextRxPage;	/* page next pkt starts at */
    UINT8		cntL;		/* low byte of frame length */
    UINT8		cntH;		/* high byte of frame length */
    } NIC_RX_HDR;


#define NIC_RX_HDR_SZ		sizeof(NIC_RX_HDR)


typedef struct rx_frame
    {
    UINT16		pad1;
    NIC_RX_HDR		rxHdr;		/* receive status header */
    struct ether_header enetHdr;	/* ethernet header */
    UINT8		data [ETHERMTU];/* frame data */
    UINT32		fcs;		/* frame check sequence */
    UINT8		refCnt;		/* loaner reference count */
    UINT8		pad2;
    } NIC_RX_FRAME;

/*
 * Macros for read and write descriptors.
 *
 */
#define NIC_FRAME_DATA_ADDR_GET(pFrame) \
	pFrame = ((char *)(pFrame) + sizeof(UINT16) + sizeof(NIC_RX_HDR_SZ));


#define NIC_RBR_LEN	((PSTOP - PSTART) + 1)
#define NIC_BUF_SZ	sizeof(NIC_RX_FRAME)
#define NIC_ETH_CRC_LEN	4

typedef char* NIC_CLUSTER;

typedef struct nic_end_device		/* driver control structure */
    {
    END_OBJ		end;		/* The class we inherit from. */
    int			unit;		/* unit number */
    int			rmdIndex;	/* current RMD index */
    int			rringSize;	/* RMD ring size */
    int 		rringLen;	/* RMD ring length (bytes) */
    UCHAR		enetAddr[NIC_EVB_EADR_LEN];	/* ethernet address */
    BOOL		txBlocked; 		/* transmit flow control */
    BOOL        	txCleaning;    
    NIC_DEVICE*		pNic;		/* address of NIC chip */
    int         	ivec;		/* interrupt vector */
    int			ilevel;		/* interrupt level */
    SEM_ID      	dmaSem;		/* exclusive use of remote DMA */
    UINT8		nextPkt;	/* NIC buf page for next pkt */
    NIC_CLUSTER		pRxCluster;	/* Rx frame memory */
    CL_POOL_ID		pClPoolId;
    NIC_DRV_FLAG	flags;		/* device specific flags */
    } NICEVB_END_DEVICE;


/* globals */

UINT32  nicEvbRestartDelay = 30000;/* number of time base ticks to wait for */
				/* when resetting the chip */

UINT32	nicIntNb = 0;		/* number of receive interrupt */
UINT32	nicTxTimeout = 0;	/* number of transmit time-out */

#ifdef	NIC_INSTRUMENT
UINT32	nicRestartNb = 0;	/* number of restart due to ring overflow */
UINT32	nicLen = 0;		/* lenght of the current received packet */
UINT32	nicHdrStat = 0;		/* status byte of the current received packet */
UINT32	nicNextPage = 0;	/* page pointer to the next received packet */
UINT32	nicCurrentPage = 0;	/* start page of the current packet */
UINT32	nicTxNb = 0;		/* number of transmitted packets */
UINT32	nicInitNb = 0;		/* number of time device is re-initialized */
#endif





/* locals */


/* forward declarations */

LOCAL int nicEvbMblkDataLenGet (M_BLK* pMblk);
LOCAL void nicEvbEnetAddrGet (NICEVB_END_DEVICE* pDrvCtrl, char* pAddress);
LOCAL void nicEvbMARSet (NICEVB_END_DEVICE* pDrvCtrl, UINT8 index, BOOL bSet);
LOCAL void nicEvbAddrFilterSet (NICEVB_END_DEVICE *pDrvCtrl, char* pAddr, BOOL bSet);
LOCAL int nicEvbHashIndex (char* eAddr);

LOCAL void	nicEvbReset (NICEVB_END_DEVICE* pDrvCtrl);
LOCAL void	nicEvbInt (NICEVB_END_DEVICE* pDrvCtrl);
LOCAL void	nicEvbRestart (NICEVB_END_DEVICE *pDrvCtrl, UINT8 cr);
LOCAL void	nicEvbHandleInt (NICEVB_END_DEVICE *pDrvCtrl);
LOCAL BOOL	nicEvbRead (NICEVB_END_DEVICE *pDrvCtrl);
LOCAL NIC_CLUSTER	nicEvbReadFrame (NICEVB_END_DEVICE* pDrvCtrl);
LOCAL STATUS nicEvbToStack (NICEVB_END_DEVICE* pDrvCtrl, NIC_CLUSTER pCluster);


LOCAL void nicEvbConfig (NICEVB_END_DEVICE* pDrvCtrl);


LOCAL STATUS	nicEvbPktBufRead (NICEVB_END_DEVICE *pDrvCtrl, UINT32
                                  nicBufAddr, UINT32 len, char *pData);

LOCAL void	nicEvbWriteCr (NIC_DEVICE *pNic, UINT8 value);
LOCAL void	nicEvbWriteReg (NIC_DEVICE *pNic, volatile UINT8 *pReg,
                                UINT8 value, UINT8 page);

LOCAL UINT8	nicEvbReadReg (NIC_DEVICE *pNic, volatile UINT8 *pReg, char page);
LOCAL STATUS	nicEvbTransmit (NICEVB_END_DEVICE *pDrvCtrl,
                                M_BLK* pMblk, BOOL waitFlag);

LOCAL void	nicEvbWritePort (UINT8 value);
LOCAL STATUS	nicEvbReceive (NICEVB_END_DEVICE *pDrvCtrl,
                               UINT32 nicBufAddr, char *pData, int len);

LOCAL UINT8	nicEvbReadPort (void);
LOCAL void	nicLoanFree (NICEVB_END_DEVICE *pDrvCtrl, NIC_RX_FRAME *pRx);
LOCAL BOOL	nicEvbPagePtrUpdate (NICEVB_END_DEVICE * pDrvCtrl,
                                     NIC_CLUSTER pRxCluster);
LOCAL void	nicEvbResetDelay (void);



LOCAL STATUS nicEvbInitParse (NICEVB_END_DEVICE* pDrvCtrl, char * initString);
LOCAL STATUS nicEvbInitMem (NICEVB_END_DEVICE* pDrvCtrl);

/* END Specific interfaces. */

END_OBJ *	nicEvbEndLoad (char *initString);
LOCAL STATUS	nicEvbUnload (NICEVB_END_DEVICE* pDrvCtrl);
LOCAL STATUS	nicEvbStart (NICEVB_END_DEVICE* pDrvCtrl);
LOCAL STATUS	nicEvbStop (NICEVB_END_DEVICE* pDrvCtrl);
LOCAL int	nicEvbIoctl (NICEVB_END_DEVICE* pDrvCtrl, int cmd, caddr_t data);
LOCAL STATUS	nicEvbSend (NICEVB_END_DEVICE* pDrvCtrl, M_BLK *pMblk);


LOCAL STATUS	nicEvbMCastAddrAdd (NICEVB_END_DEVICE *pDrvCtrl, char* pAddress);
LOCAL STATUS	nicEvbMCastAddrDel (NICEVB_END_DEVICE *pDrvCtrl, char* pAddress);
LOCAL STATUS	nicEvbMCastAddrGet (NICEVB_END_DEVICE *pDrvCtrl, 
                                      MULTI_TABLE *pTable);
LOCAL STATUS	nicEvbPollSend (NICEVB_END_DEVICE *pDrvCtrl, M_BLK *pMblk);
LOCAL STATUS	nicEvbPollReceive (NICEVB_END_DEVICE *pDrvCtrl, M_BLK *pMblk);
LOCAL STATUS	nicEvbPollStart (NICEVB_END_DEVICE *pDrvCtrl);
LOCAL STATUS	nicEvbPollStop (NICEVB_END_DEVICE *pDrvCtrl);


/*
 * Declare our function table.  This is static across all driver
 * instances.
 */
LOCAL NET_FUNCS nicEvbFuncTable =
    {
    (FUNCPTR)nicEvbStart,	/* Function to start the device. */
    (FUNCPTR)nicEvbStop,	/* Function to stop the device. */
    (FUNCPTR)nicEvbUnload,	/* Unloading function for the driver. */
    (FUNCPTR)nicEvbIoctl,	/* Ioctl function for the driver. */
    (FUNCPTR)nicEvbSend,	/* Send function for the driver. */
    (FUNCPTR)nicEvbMCastAddrAdd, /* Multicast address add */
    (FUNCPTR)nicEvbMCastAddrDel, /* Multicast address delete */
    (FUNCPTR)nicEvbMCastAddrGet, /* Multicast table retrieve */
    (FUNCPTR)nicEvbPollSend,	 /* Polling send function for the driver. */
    (FUNCPTR)nicEvbPollReceive, /* Polling receive function for the driver. */
    endEtherAddressForm,        /* Put address info into a packet.  */
    endEtherPacketDataGet,      /* Get a pointer to packet data. */
    endEtherPacketAddrGet       /* Get packet addresses. */
    };


/******************************************************************************
*
* nicEndLoad - initialize the driver and device
*
* This routine initializes the driver and the device to the operational state.
* All of the device-specific parameters are passed in <initString>, which
* expects a string of the following format:
*
* <unit>:<base_addr>:<int_vector>:<int_level>
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

END_OBJ* nicEvbEndLoad
    (
    char* initString		/* string to be parse by the driver */
    )
    {
    NICEVB_END_DEVICE	*pDrvCtrl;

    DRV_LOG (DRV_DEBUG_LOAD, "Loading nic...\n", 1, 2, 3, 4, 5, 6);

    if (initString == NULL)
        return (NULL);
    
    if (initString[0] == NULL)
        {
        bcopy((char *)NIC_EVB_DEV_NAME, initString, NIC_EVB_DEV_NAME_LEN);
        return (0);
        }
    
    /* allocate the device structure */

    pDrvCtrl = (NICEVB_END_DEVICE *)calloc (sizeof (NICEVB_END_DEVICE), 1);
    if (pDrvCtrl == NULL)
	goto errorExit;

    /* init device object */
    pDrvCtrl->pRxCluster = NULL;
    pDrvCtrl->flags = 0x00;    
    

    /* parse the init string, filling in the device structure */

    if (nicEvbInitParse (pDrvCtrl, initString) == ERROR)
	goto errorExit;

    
    /* Have the BSP hand us our address. */

    SYS_ENET_ADDR_GET(pDrvCtrl,(char*)&(pDrvCtrl->enetAddr));

    /* initialize the END and MIB2 parts of the structure */

    if (END_OBJ_INIT (&pDrvCtrl->end, (DEV_OBJ *)pDrvCtrl, NIC_EVB_DEV_NAME,
		    pDrvCtrl->unit, &nicEvbFuncTable,
                      "ST-NIC Enhanced Network Driver") == ERROR
     || END_MIB_INIT (&pDrvCtrl->end, M2_ifType_ethernet_csmacd,
                      &pDrvCtrl->enetAddr[0], NIC_EVB_EADR_LEN, ETHERMTU,
                      NIC_EVB_SPEED)
		    == ERROR)
	goto errorExit;

    /* Perform memory allocation */

    if (nicEvbInitMem (pDrvCtrl) == ERROR)
	goto errorExit;

    /* set the flags to indicate readiness */

    END_OBJ_READY (&pDrvCtrl->end,
                   IFF_NOTRAILERS | IFF_MULTICAST | IFF_BROADCAST);
    
    DRV_LOG (DRV_DEBUG_LOAD, "Done loading nicEvb...\n", 1, 2, 3, 4, 5, 6);

    return (&pDrvCtrl->end);

    errorExit:
        {
        nicEvbUnload (pDrvCtrl);
        return NULL;
        }
    }


/*******************************************************************************
*
* nicEvbInitParse - parse the initialization string
*
* Parse the input string.  Fill in values in the driver control structure.
* The initialization string format is:
* <unit>:<base_adrs>:<vecnum>:<intLvl>
*
* .IP <unit>
* Device unit number, a small integer.
* .IP <base_adrs>
* Base address for NIC device
* .IP <vecNum>
* Interrupt vector number (used with sysIntConnect() ).
* .IP <intLvl>
* Interrupt level.
*
* RETURNS: OK, or ERROR if any arguments are invalid.
*/

STATUS nicEvbInitParse
    (
    NICEVB_END_DEVICE * pDrvCtrl,
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

    /* NIC address. */
    
    tok = strtok_r (NULL, ":", &pHolder);
    if (tok == NULL)
	return ERROR;
    address = strtoul (tok, NULL, 16);
    pDrvCtrl->pNic = (NIC_DEVICE* )address;


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

    DRV_LOG (DRV_DEBUG_LOAD, "Address %p Level %u ivec %d\n",
               pDrvCtrl->pNic, pDrvCtrl->ilevel, pDrvCtrl->ivec, 4, 5, 6);    

    return OK;
    }

/*******************************************************************************
*
* nicEvbInitMem - initialize memory for Lance chip
*
* Using data in the control structure, setup and initialize the memory
* areas needed.  If the memory address is not already specified, then allocate
* cache safe memory.
*
* RETURNS: OK or ERROR.
*/

LOCAL STATUS nicEvbInitMem
    (
    NICEVB_END_DEVICE * pDrvCtrl	/* device to be initialized */
    )
    {
    M_CL_CONFIG		mclBlkConfig;
    CL_DESC		clDesc;		/* cluster description */



    pDrvCtrl->rringSize = NIC_RBR_LEN;	/* size of the whole ring */
    pDrvCtrl->rringLen = pDrvCtrl->rringSize * NIC_BUF_SZ; /* rec ring len */    
    pDrvCtrl->rmdIndex = 0;		/* init recv buf index */
    pDrvCtrl->nextPkt = CURR;		/* reset to initial value */

    /* for remote DMA access */
    
    pDrvCtrl->dmaSem = semMCreate (SEM_Q_PRIORITY);
    
    
    /*
     * Allocate receive buffers from our own private pool.
     */

    /* Initialize net buffer pool for tx/rx buffers */

    bzero ((char *)&mclBlkConfig, sizeof(mclBlkConfig));
    bzero ((char *)&clDesc, sizeof(clDesc));
    

    if ((pDrvCtrl->end.pNetPool = malloc (sizeof(NET_POOL))) == NULL)
        return (ERROR);

    mclBlkConfig.mBlkNum  = pDrvCtrl->rringSize * 8;
    clDesc.clNum	  = pDrvCtrl->rringSize * 8;
    mclBlkConfig.clBlkNum = clDesc.clNum;

    mclBlkConfig.memSize = (mclBlkConfig.mBlkNum * (MSIZE + sizeof (long))) +
                          (mclBlkConfig.clBlkNum * (CL_BLK_SZ + sizeof(long)));

    if ((mclBlkConfig.memArea = (char *) memalign (sizeof(long),
                                                   mclBlkConfig.memSize)) == NULL)
        return (ERROR);
    


    clDesc.clSize  = NIC_BUF_SZ;
    clDesc.memSize = ((clDesc.clNum * (clDesc.clSize + 4)) + 4);


   clDesc.memArea = memalign (sizeof(long), clDesc.memSize);
   pDrvCtrl->pRxCluster = (NIC_CLUSTER)clDesc.memArea;
   

   if (clDesc.memArea == NULL)
        {
        DRV_LOG (DRV_DEBUG_LOAD, "system memory unavailable\n", 1, 2, 3, 4, 5, 6);
        return (ERROR);
        }

    /* Initialize the net buffer pool with transmit buffers */    
    if (netPoolInit(pDrvCtrl->end.pNetPool, &mclBlkConfig,
                    &clDesc, 1, NULL) == ERROR)
        
        {
        DRV_LOG (DRV_DEBUG_LOAD, "Could not init buffering\n",
                 1, 2, 3, 4, 5, 6);
        return (ERROR);
        }
    
#ifdef DRV_DEBUG
    nicNetPool = *pDrvCtrl->end.pNetPool;
#endif
    /* Store the cluster pool id as others need it later. */
    pDrvCtrl->pClPoolId = clPoolIdGet(pDrvCtrl->end.pNetPool, NIC_BUF_SZ, FALSE);


    
    /* Setup the receive ring */
    return OK;
    }


/*******************************************************************************
*
* nicEvbStart - start the device
*
* This function calls BSP functions to connect interrupts and start the
* device running in interrupt mode.
*
* RETURNS: OK or ERROR
*
*/

LOCAL STATUS nicEvbStart
    (
    NICEVB_END_DEVICE *pDrvCtrl
    )
    {
    STATUS result;

    pDrvCtrl->txCleaning = FALSE;
    pDrvCtrl->txBlocked = FALSE;
    
    SYS_INT_CONNECT (pDrvCtrl, nicEvbInt, (int)pDrvCtrl, &result);
    
    if (result == ERROR)
	return ERROR;
    
    DRV_LOG (DRV_DEBUG_LOAD, "Interrupt connected.\n", 1, 2, 3, 4, 5, 6);

    /* reset the device */
    nicEvbReset (pDrvCtrl);

    nicEvbConfig (pDrvCtrl);		/* device config */

    /* mark the interface -- up */
    END_FLAGS_SET (&pDrvCtrl->end, (IFF_UP | IFF_RUNNING));

    
    SYS_INT_ENABLE (pDrvCtrl->ivec);

    
    DRV_LOG (DRV_DEBUG_LOAD, "interrupt enabled.\n", 1, 2, 3, 4, 5, 6);

    return (OK);
    }


/*******************************************************************************
*
* nicEvbConfig - configure the NIC chip and program address
*
* This routine follows the algorythm in the ST-NIC manual for enabling
* a NIC device on an active network.  Essentially, this routine initializes
* the NIC device.
*
* RETURNS: N/A.
*/

LOCAL void nicEvbConfig
    (
    NICEVB_END_DEVICE * pDrvCtrl	/* device to be configured */    
    )
    {
    NIC_DEVICE* pNic = pDrvCtrl->pNic;
    UCHAR*	pEnetAddr = pDrvCtrl->enetAddr;
    
    nicEvbWriteCr (pNic, RPAGE0 | STP | ABORT);

    nicEvbResetDelay ();

    nicEvbWriteReg (pNic, &pNic->Dcr, NOTLS | FIFO8, RPAGE0);

    /* clear remote DMA byte count registers */

    nicEvbWriteReg (pNic, &pNic->Rbcr0, 0, RPAGE0);             
    nicEvbWriteReg (pNic, &pNic->Rbcr1, 0, RPAGE0);

    /* accept broadcast and multicast, but not runt */

    /* MULTI */
    nicEvbWriteReg (pNic, &pNic->Rcr, AB, RPAGE0);    
    nicEvbWriteReg (pNic, &pNic->Rcr, (AB | AM), RPAGE0);
    nicEvbWriteReg (pNic, &pNic->Tcr, MODE1, RPAGE0);	/* int loopback mode */

    nicEvbWriteReg (pNic, &pNic->Pstart, PSTART, RPAGE0);
    nicEvbWriteReg (pNic, &pNic->Pstop, PSTOP, RPAGE0);
    nicEvbWriteReg (pNic, &pNic->Bnry, BNRY, RPAGE0);

    nicEvbWriteReg (pNic, &pNic->Isr, 0xff, RPAGE0);	/* clr pending ints */
    nicEvbWriteReg (pNic, &pNic->Imr, PRXE | OVWE, RPAGE0); /* enable int */

    
    /* set up page 1 registers */

    nicEvbWriteReg (pNic, &pNic->Par0, pEnetAddr [0], RPAGE1);
    nicEvbWriteReg (pNic, &pNic->Par1, pEnetAddr [1], RPAGE1);
    nicEvbWriteReg (pNic, &pNic->Par2, pEnetAddr [2], RPAGE1);
    nicEvbWriteReg (pNic, &pNic->Par3, pEnetAddr [3], RPAGE1);
    nicEvbWriteReg (pNic, &pNic->Par4, pEnetAddr [4], RPAGE1);
    nicEvbWriteReg (pNic, &pNic->Par5, pEnetAddr [5], RPAGE1);


    nicEvbWriteReg (pNic, &pNic->nic_pg1.mar0, 0x00, RPAGE1);
    nicEvbWriteReg (pNic, &pNic->nic_pg1.mar1, 0x00, RPAGE1);
    nicEvbWriteReg (pNic, &pNic->nic_pg1.mar2, 0x00, RPAGE1);
    nicEvbWriteReg (pNic, &pNic->nic_pg1.mar3, 0x00, RPAGE1);
    nicEvbWriteReg (pNic, &pNic->nic_pg1.mar4, 0x00, RPAGE1);
    nicEvbWriteReg (pNic, &pNic->nic_pg1.mar5, 0x00, RPAGE1);
    nicEvbWriteReg (pNic, &pNic->nic_pg1.mar6, 0x00, RPAGE1);
    nicEvbWriteReg (pNic, &pNic->nic_pg1.mar7, 0x00, RPAGE1);
    
    nicEvbWriteReg (pNic, &pNic->Curr, CURR, RPAGE1);

    nicEvbWriteCr (pNic, RPAGE0 | ABORT | STA);		/* back to page 0 */
    nicEvbWriteReg (pNic, &pNic->Tcr, MODE0, RPAGE0);	/* Tx normal mode */
    }



/*******************************************************************************
*
* nicEvbResetDelay - performs the delay required before resetting the chip 
*
* This routine performs a 1.6 ms delay for PowerPC architecture if 
* using the internal PPC time base (the number of time base increment to
* count for 1.6 msec is defined by the value nicRestartDelay which must 
* be set in the BSP), or waits for one system clock tick otherwise.
*
* RETURNS: N/A.
*/
 
LOCAL void nicEvbResetDelay (void)
    {
#ifdef  NIC_USE_PPCTIMEBASE
    UINT32 tbu1, tbu2;
    UINT32 tbl1, tbl2;
#endif

#ifdef  NIC_USE_PPCTIMEBASE
    vxTimeBaseGet (&tbu1, &tbl1);
    while (1)  {
        vxTimeBaseGet (&tbu2, &tbl2);
        if ((tbu2-tbu1) != 0)
            vxTimeBaseGet (&tbu1, &tbl1);
        else
	    if ((tbl2 - tbl1) > nicEvbRestartDelay)
            	break;
        }
#else
    taskDelay (1);                      /* wait at least 1.6 mSec */
#endif
    }


/*******************************************************************************
*
* nicEvbWriteCr - write to the NIC command register
*
* RETURNS: N/A.
*/

LOCAL void nicEvbWriteCr
    (
    NIC_DEVICE		*pNic,
    UINT8		value
    )
    {
    int			level = intLock ();

    do {
        pNic->Cr = value;
        } while	(!((*(volatile UINT8 *)NIC_DMA) & ACC_MASK));

    intUnlock (level);
    }


/*******************************************************************************
*
* nicEvbWriteReg - write to a NIC register
*
* RETURNS: N/A.
*/

LOCAL void nicEvbWriteReg 
    (
    NIC_DEVICE *	pNic,
    volatile UINT8 *	pReg,
    UINT8		value,
    UINT8		page
    )
    {
    volatile UINT8 *	nicDma = (volatile UINT8 *) NIC_DMA;
    UINT8		cr;
    int			level = intLock ();

    do {
	cr = pNic->Cr;
	} while (!((*nicDma) & ACC_MASK));

    if ((cr & PBMASK) != page)
	do {
	    pNic->Cr = (cr & 0x3f) | page;
	    } while (!((*nicDma) & ACC_MASK));

    do {
	*pReg = value;
	} while (!((*nicDma) & ACC_MASK));

    intUnlock (level);
    }

/*******************************************************************************
*
* nicReadReg - read from a NIC register
*
* RETURNS: the register value.
*/

LOCAL UINT8 nicEvbReadReg 
    (
    NIC_DEVICE *	pNic,
    volatile UINT8 *	pReg,
    char		page
    )
    {
    volatile UINT8 *	nicDma = (volatile UINT8 *) NIC_DMA;
    UINT8		cr;
    int			level = intLock ();

    do {
	cr = pNic->Cr;
	} while (!((*nicDma) & ACC_MASK));

    if ((cr & PBMASK) != page)
	do {
	    pNic->Cr = (cr & 0x3f) | page;
	    } while (!((*nicDma) & ACC_MASK));

    do {
	cr = *pReg;
	} while (!((*nicDma) & ACC_MASK));

    intUnlock (level);
    return (cr);
    }


/*******************************************************************************
*
* nicEvbReadPort - read from the DMA port
*
* RETURNS: the DMA port value.
*/

LOCAL UINT8 nicEvbReadPort (void)
    {
    UINT8		value;
#ifndef	NIC_FASTER
    int			level = intLock ();
#endif

    do
	{
	value = *(volatile UINT8 *) NIC_PORT;
        }
    while (!((*(volatile UINT8 *) NIC_DMA) & ACC_MASK));

#ifndef	NIC_FASTER
    intUnlock (level);
#endif
    return (value);
    }


/*******************************************************************************
*
* nicEvbReceive - receive data from the NIC network interface
*
* This routine transfers data from the NIC device ring buffers via the
* remote DMA.
*
* RETURNS: OK, always.
*/

LOCAL STATUS nicEvbReceive 
    (
    NICEVB_END_DEVICE*	pDrvCtrl,    
    UINT32		nicBufAddr, 
    char*		pData, 
    int			len
    )
    {
    NIC_DEVICE *	pNic = pDrvCtrl->pNic;
    int			residual;
    int			dmaCnt;
    int			ix;

#ifdef	NIC_FASTER
    nicEvbWriteReg(pNic, &pNic->Imr, 0, RPAGE0);	/* disable all interrupts */
#endif

    DRV_LOG (DRV_DEBUG_RX, "nicReceive: nicBufAddr:%u pData:%p len:%d\n",
             nicBufAddr, pData, len, 0, 0, 0);        
    
    if ((nicBufAddr + len) < (PSTOP * PACKET_SIZE))
	{

	nicEvbWriteReg (pNic, &pNic->Rbcr0, (len & 0xff), RPAGE0);
	nicEvbWriteReg (pNic, &pNic->Rbcr1, ((len & 0xff00) >> 8), RPAGE0);
	nicEvbWriteReg (pNic, &pNic->Rsar0, (nicBufAddr & 0xff), RPAGE0);
	nicEvbWriteReg (pNic, &pNic->Rsar1, (((nicBufAddr & 0xff00)>> 8)), RPAGE0);
	nicEvbWriteCr (pNic, RREAD);

	for (ix = 0; ix < len; ix ++)
	    *pData++ = nicEvbReadPort ();
	}
    else
	{

	residual = (nicBufAddr + len) - (PSTOP * PACKET_SIZE);
	dmaCnt = len - residual;

	nicEvbWriteReg (pNic, &pNic->Rbcr0, (dmaCnt & 0xff), RPAGE0);
	nicEvbWriteReg (pNic, &pNic->Rbcr1, ((dmaCnt & 0xff00) >> 8), RPAGE0);
	nicEvbWriteReg (pNic, &pNic->Rsar0, (nicBufAddr & 0xff), RPAGE0);
	nicEvbWriteReg (pNic, &pNic->Rsar1, (((nicBufAddr & 0xff00)>> 8)), RPAGE0);
	nicEvbWriteCr (pNic, RREAD);

	for (ix = 0; ix < dmaCnt; ix++)
	    *pData++ = nicEvbReadPort ();

	nicEvbWriteReg (pNic, &pNic->Rbcr0, (residual & 0xff), RPAGE0);
	nicEvbWriteReg (pNic, &pNic->Rbcr1, ((residual & 0xff00) >> 8), RPAGE0);
	nicEvbWriteReg (pNic, &pNic->Rsar0, 0x00, RPAGE0);
	nicEvbWriteReg (pNic, &pNic->Rsar1, PSTART, RPAGE0);
	nicEvbWriteCr (pNic, RREAD);

	for (ix = 0; ix < residual; ix++)
	    *pData++ = nicEvbReadPort ();
	}

#ifdef	NIC_FASTER
    nicEvbWriteReg(pNic, &pNic->Imr, PRXE | OVWE, RPAGE0);	/* re-enable intr. */
#endif
    return (OK);
    } 

/*******************************************************************************
*
* nicEvbPktBufRead - read data from the NIC receive ring buffer
*
* This routine gets exclusive access to the remote DMA, and calls
* nicReceive() to get data from the NIC's receive ring buffer.
*
* RETURNS: OK, or ERROR if obtaining the requested bytes encountered an error.
*/

LOCAL STATUS nicEvbPktBufRead
    (
    NICEVB_END_DEVICE*	pDrvCtrl,
    UINT32		nicBufAddr,
    UINT32		len,
    char*		pData
    )
    {
    STATUS		status = OK;

    
    /* avoid starting DMA if device is down to to fatal error */

    if ((END_FLAGS_GET(&pDrvCtrl->end) & (IFF_UP | IFF_RUNNING)) !=
        (IFF_UP | IFF_RUNNING))
        {
        DRV_LOG (DRV_DEBUG_RX, "nicPktBufRead: NOT UP and RUNNING\n",
                 0, 0, 0, 0, 0, 0);        
        return (ERROR);
        }

    if (!NIC_IS_IN_POLL_MODE())
        {
        if (semTake (pDrvCtrl->dmaSem, 100) == ERROR)	/* get DMA */
            {
            DRV_LOG (DRV_DEBUG_LOAD, "nicPktBufRead: can't obtain dmaSem\n",
                     0, 0, 0, 0, 0, 0);
            semTake (pDrvCtrl->dmaSem, WAIT_FOREVER);
            }
        }

    status = nicEvbReceive (pDrvCtrl, nicBufAddr, pData, len);

    if (!NIC_IS_IN_POLL_MODE())
        semGive(pDrvCtrl->dmaSem);

    return (status);
    }

/*******************************************************************************
*
* nicEvbPagePtrUpdate - updates receive buffer/page pointers
*
* This routine updates the receive buffer/page pointer and the receive
* boundary register (BNRY). The chip is re-initialized if the receive next 
* packet pointer recorded in the current packet header is out of range.
*
* RETURNS: TRUE, or FALSE if next packet pointer is out of range.
*/
 
LOCAL BOOL nicEvbPagePtrUpdate
    (
    NICEVB_END_DEVICE*	pDrvCtrl,
    NIC_CLUSTER		pRxCluster
    )
    {
    NIC_RX_FRAME*	pRx = (NIC_RX_FRAME*)pRxCluster;
    NIC_DEVICE*		pNic = pDrvCtrl->pNic;

    /* update ring buffer/page pointers */

    if ((pRx->rxHdr.nextRxPage < PSTART)||(pRx->rxHdr.nextRxPage >= PSTOP))
        {
        DRV_LOG (DRV_DEBUG_LOAD,
                 "nicPagePtrUpdate: statusHeader=0x%x nextRxPage=%d IntNb=%d\n",
                 pRx->rxHdr.status, pRx->rxHdr.nextRxPage, nicIntNb, 0, 0, 0);

#ifdef	NIC_INSTRUMENT
    	nicInitNb++;
#endif
        /* TODO MID increament error count */
        /* pDrvCtrl->idr.ac_if.if_ierrors++; */

	/* restart the chip - we should never end up here - */

    	/* nicEvbConfig (0); HELP was index 0 */
        nicEvbConfig (pDrvCtrl);
    	pDrvCtrl->nextPkt = CURR;

        /* mark the interface -- up */
        END_FLAGS_SET (&pDrvCtrl->end,
                       (IFF_UP | IFF_RUNNING | IFF_MULTICAST | IFF_BROADCAST));    

        return (FALSE);
        }

    /* updates the Boundary pointer register (BNRY) */

    if ((pDrvCtrl->nextPkt = pRx->rxHdr.nextRxPage) == PSTART)
        nicEvbWriteReg (pNic, &pNic->Bnry, PSTOP - 1, RPAGE0);
    else
        nicEvbWriteReg (pNic, &pNic->Bnry, pDrvCtrl->nextPkt - 1, RPAGE0);

    return (TRUE);
    }

/*******************************************************************************
*
* nicEvToStack - passes received data to stack
*
* This routine grabs a MBuf from the pool, assigns data pointer to pointes the 
* receive data and passes the MBuf to stack.
*
* RETURNS: OK, or ERROR in case of any error.
*/
LOCAL STATUS nicEvbToStack
    (
    NICEVB_END_DEVICE*	pDrvCtrl,	/* the device */
    NIC_CLUSTER		pCluster
    )
    {
    NIC_RX_FRAME*	pRx = (NIC_RX_FRAME*)pCluster;    
    CL_BLK_ID		pClBlk;
    M_BLK_ID		pMblk;
    UINT32		len;
    STATUS		nRetValue = ERROR;

    len = pRx->rxHdr.cntL + (pRx->rxHdr.cntH << 8) - NIC_ETH_CRC_LEN;
    
    if ((pClBlk = netClBlkGet (pDrvCtrl->end.pNetPool, M_DONTWAIT)) == NULL)
        {
        DRV_LOG (DRV_DEBUG_RX, "Out of Cluster Blocks!\n", 1, 2, 3, 4, 5, 6);
        netClFree (pDrvCtrl->end.pNetPool, pCluster);        
        END_ERR_ADD (&pDrvCtrl->end, MIB2_IN_ERRS, +1);
        goto cleanRXD;
        }

    /*
     * OK we've got a spare, let's get an M_BLK_ID and marry it to the
     * one in the ring.
     */

    if ((pMblk = mBlkGet(pDrvCtrl->end.pNetPool, M_DONTWAIT, MT_DATA)) ==
        NULL)
        {
        netClBlkFree (pDrvCtrl->end.pNetPool, pClBlk);
        netClFree (pDrvCtrl->end.pNetPool, pCluster);                
        DRV_LOG (DRV_DEBUG_MB, "Out of M Blocks!\n", 1, 2, 3, 4, 5, 6);
        END_ERR_ADD (&pDrvCtrl->end, MIB2_IN_ERRS, +1);
        goto cleanRXD;
        }

    END_ERR_ADD (&pDrvCtrl->end, MIB2_IN_UCAST, +1);


    /* Join the cluster to the MBlock */

    netClBlkJoin (pClBlk, (char*)pCluster, len, NULL, 0, 0, 0);
    netMblkClJoin (pMblk, pClBlk);

    /* make the packet data coherent */
    /* LN_CACHE_INVALIDATE (pMblk->mBlkHdr.mData, len); */

    /* pMblk->mBlkHdr.mData += pDrvCtrl->offset; */
    NIC_FRAME_DATA_ADDR_GET(pCluster);    
    pMblk->mBlkHdr.mLen = len;
    pMblk->mBlkHdr.mData = pCluster;
    pMblk->mBlkHdr.mFlags |= M_PKTHDR;
    pMblk->mBlkPktHdr.len = len;


    /* Call the upper layer's receive routine. */
    END_RCV_RTN_CALL(&pDrvCtrl->end, pMblk);

    return nRetValue;

cleanRXD:

    return OK;
    }


/*******************************************************************************
*
* nicEvbRead - read a packet off the interface ring buffer
*
* nicRead copies packets from local memory into an mbuf and hands it to
* the next higher layer (IP).
*
* RETURNS: TRUE, or FALSE if the packet reception encountered errors.
*/

LOCAL BOOL nicEvbRead
    (
    NICEVB_END_DEVICE*	pDrvCtrl
    )
    {
    NIC_CLUSTER	pRxCluster;    
    DRV_LOG (DRV_DEBUG_POLL, "Start Read!\n", 1, 2, 3, 4, 5, 6);    

    pRxCluster = (NIC_CLUSTER)nicEvbReadFrame (pDrvCtrl);

    /* Pass the data up to the stack */    
    if (pRxCluster == NULL)
        return FALSE;
    
    nicEvbToStack (pDrvCtrl, pRxCluster);

    return (TRUE);

    }

/*******************************************************************************
*
* nicEvbReadFrame - read a packet off the interface ring buffer into a cluster
*
* Allocates a new cluster from the cluster pool, and reads the frame from the
* device into the cluster.
*
* RETURNS: a cluster or NULL if any error
*/
LOCAL NIC_CLUSTER nicEvbReadFrame
    (
    NICEVB_END_DEVICE*	pDrvCtrl
    )
    {
    NIC_RX_FRAME*	pRx;
    UINT32		len;			/* len of Rx pkt */
    int			cur;
    NIC_CLUSTER		pRetCluster = NULL;
    NIC_DEVICE*		pNic = pDrvCtrl->pNic;	/* NIC registers */
    
    if (!(END_FLAGS_GET(&pDrvCtrl->end) & (IFF_UP | IFF_RUNNING)) ||
        (pDrvCtrl->nextPkt == (cur = nicEvbReadReg (pNic, &pNic->Curr,
                                                    RPAGE1))))
        return (pRetCluster);

    /*
     * OK, there is work to be done.
     * First we copy the NIC receive status header from the NIC buffer
     * into our local area. This is done so that we can obtain the length
     * of the packet before copying out the rest of it. Note that the length
     * field in the NIC header includes the Ethernet header, the data, and
     * the 4 byte FCS field.
     */

    /* Get a cluster */
    if ((pRx = (NIC_RX_FRAME*)netClusterGet(pDrvCtrl->end.pNetPool,
                                           pDrvCtrl->pClPoolId)) == NULL)
        {
        DRV_LOG (DRV_DEBUG_RX, "Cannot loan!\n", 1, 2, 3, 4, 5, 6);
        END_ERR_ADD (&pDrvCtrl->end, MIB2_IN_ERRS, +1);
        return (pRetCluster);
        }
    

    /* read header in cluster*/
    DRV_LOG (DRV_DEBUG_RX, "nicPktBufRead : Reading Header\n", 1, 2, 3, 4, 5, 6);    
    if (nicEvbPktBufRead (pDrvCtrl, pDrvCtrl->nextPkt << 8,
                          (NIC_RX_HDR_SZ), (char *) &pRx->rxHdr) == ERROR)
        {

        DRV_LOG (DRV_DEBUG_RX,
                 "nicRead could not read packet header\n",
                 0, 0, 0, 0, 0, 0);
        return (pRetCluster);
        }

    /* TODO */
    len = pRx->rxHdr.cntL + (pRx->rxHdr.cntH << 8) - NIC_ETH_CRC_LEN;
    
    /* valid frame checks */

    /*
     * NOTE: if the packet's receive status byte indicates an error
     * the packet is discarded and the receive page pointers are updated to
     * point to the next packet.
     */

#ifdef	NIC_INSTRUMENT
    nicLen = len;
    nicHdrStat = pRx->rxHdr.status;
    nicNextPage = pRx->rxHdr.nextRxPage;
    nicCurrentPage = cur;
#endif

    if ((len < 60) || (len > 1514) || ((pRx->rxHdr.status & ~PHY) != PRX))
	{
        /* TODO increament error count */
	/* pDrvCtrl->idr.ac_if.if_ierrors++; */

        DRV_LOG (DRV_DEBUG_LOAD,
                 "nicRead receive error: statusHeader=0x%x nextRxPage=%d \
                           currentPage=%d len=%d IntNb=%d\n",
                 pRx->rxHdr.status, pRx->rxHdr.nextRxPage, cur,len,nicIntNb,
                 0);
        
        END_ERR_ADD (&pDrvCtrl->end, MIB2_IN_ERRS, +1);

	nicEvbPagePtrUpdate (pDrvCtrl, (NIC_CLUSTER)pRx);

        return (pRetCluster);
	}

    /* copy Ethernet packet section of the frame */
    
    DRV_LOG (DRV_DEBUG_RX, "nicPktBufRead : Reading Frame\n",
             1, 2, 3, 4, 5, 6);
    
    if (nicEvbPktBufRead (pDrvCtrl, (pDrvCtrl->nextPkt << 8) + NIC_RX_HDR_SZ,
                          len, (char *) &pRx->enetHdr) == ERROR)
        {

        DRV_LOG (DRV_DEBUG_LOAD, "nicRead: Could not read packet data\n",
                 0, 0, 0, 0, 0, 0);

        END_ERR_ADD (&pDrvCtrl->end, MIB2_IN_ERRS, +1);        

	nicEvbPagePtrUpdate(pDrvCtrl, (NIC_CLUSTER)pRx);
        return (pRetCluster);
        
	}

    /* update ring buffer/page pointers */
    if (!nicEvbPagePtrUpdate (pDrvCtrl, (NIC_CLUSTER)pRx))
	{
	return (pRetCluster);
	}

    END_ERR_ADD (&pDrvCtrl->end, MIB2_IN_UCAST, +1);    

    DRV_LOG (DRV_DEBUG_POLL, "End Read!\n", 1, 2, 3, 4, 5, 6);     

    return (NIC_CLUSTER)pRx;
    }


/*******************************************************************************
*
* nicEvbRestart - restart chip after receive ring buffer overflow
*
* This routine is the task-level handler that deals with a receive DMA
* overflow condition.  It gets access to the remote DMA, cleans up NIC
* registers, empties the receive ring buffers, and then resends any
* packet that was in the midst of transmission when the overflow hit.
*
* RETURNS: N/A.
*/

LOCAL void nicEvbRestart
    (
    NICEVB_END_DEVICE*	pDrvCtrl,
    UINT8		cr
    )
    {
    NIC_DEVICE *	pNic = pDrvCtrl->pNic;	/* NIC registers */
    BOOL		resend = FALSE;



#ifdef	NIC_INSTRUMENT
    nicRestartNb++;

#endif
    nicEvbWriteCr (pNic, STP);


    nicEvbResetDelay ();	/* wait at least 1.6 mSec */


    if(!NIC_IS_IN_POLL_MODE())
        {
        if (semTake(pDrvCtrl->dmaSem, 100) == ERROR)
            {
            DRV_LOG (DRV_DEBUG_LOAD, "nicRestart: can't obtain dmaSem\n",
                     0, 0, 0, 0, 0,0);
            semTake (pDrvCtrl->dmaSem, WAIT_FOREVER);
            }
        }

    nicEvbWriteReg (pNic, &pNic->Rbcr0, 0, RPAGE0);
    nicEvbWriteReg (pNic, &pNic->Rbcr1, 0, RPAGE0);

    if ((cr & TXP) && (!(nicEvbReadReg (pNic, &pNic->Isr, RPAGE0) &
        (TXE | PTX))))
    	resend = TRUE;

    nicEvbWriteReg (pNic, &pNic->Tcr, MODE1, RPAGE0);
    nicEvbWriteCr (pNic, RPAGE0 | ABORT | STA);		/* back to page 0 */

    END_FLAGS_SET (&pDrvCtrl->end, IFF_UP | IFF_RUNNING | IFF_NOTRAILERS);

    /*
     * Release the dmaSem semaphore here since a) it will be re-acquired
     * later if needed and b) we don't want to hold it while sending things
     * up the stack since this may cause a deadlock if an application
     * tries to send something down the stack to us. (The send routine
     * will try to acquire the semaphore while we're still holding it
     * here.)
     */
 
    if(!NIC_IS_IN_POLL_MODE())
        semGive(pDrvCtrl->dmaSem);

    while (nicEvbRead (pDrvCtrl))
	;

    if(!NIC_IS_IN_POLL_MODE())
        {
        if (semTake(pDrvCtrl->dmaSem, 100) == ERROR)
            {
            DRV_LOG (DRV_DEBUG_LOAD, "nicRestart: can't obtain dmaSem\n",
                     0, 0, 0, 0, 0,0);
            semTake (pDrvCtrl->dmaSem, WAIT_FOREVER);
            }
        }

    nicEvbWriteReg(pNic, &pNic->Isr, OVW, RPAGE0);
    nicEvbWriteReg (pNic, &pNic->Tcr, MODE0, RPAGE0);

    if (resend)
        nicEvbWriteCr (pNic, RPAGE0 | STA | TXP | ABORT);

    if(!NIC_IS_IN_POLL_MODE())
        semGive(pDrvCtrl->dmaSem);

    nicEvbWriteReg (pNic, &pNic->Imr, PRXE | OVWE, RPAGE0);
    }


/*******************************************************************************
*
* nicEvbHandleInt - deferred receive interrupt handler
*
* This function handles the received frames from the device.  It runs in the
* context of the netTask, which was triggered by a received packet interrupt.
* Actual processing of the packet is done by calling nicEvbRead().
*
* RETURNS: N/A.
*/

LOCAL void nicEvbHandleInt
    (
    NICEVB_END_DEVICE*	pDrvCtrl    
    )
    {
    /* empties the receive ring buffer of its packets */

    while (nicEvbRead (pDrvCtrl));
    }




/*******************************************************************************
*
* nicIntr - The driver's interrupt handler
*
* This function clears the cause of the device interrupt(s) and then acts
* on the individual possible causes.  The primary goal of this routine is to
* minimize the time spent in it.  This is accomplished by deferring processing
* to the netTask via the netJobAdd() function.
*
* Note that in case the receiver overruns, we promptly mark the interface as
* "down" and leave error handling to task-level.   This is in case netTask
* is in the midst of DMA activity, we must allow it to complete.  The receive
* handler will give up when it discovers the interface is down, which will
* then allow netTask to run our OVW handler.  This provides a nice orderly
* error recovery.
*
* RETURNS: N/A.
*/

LOCAL void nicEvbInt
    (
    NICEVB_END_DEVICE*	pDrvCtrl
    )
    {
    NIC_DEVICE *	pNic = pDrvCtrl->pNic;	/* NIC registers */
    UINT8		isr;				/* copy of ISR */
    UINT8		cr;				/* copy of CR */

#ifdef	DRV_DEBUG
    nicIntNb++;
#endif

    DRV_LOG (DRV_DEBUG_INT, "Inside INT\n", 1, 2, 3, 4, 5, 6);
    
    isr = nicEvbReadReg(pNic, &pNic->Isr, RPAGE0);
    nicEvbWriteReg(pNic, &pNic->Isr, isr, RPAGE0);


    /* handle receiver overrun */

    if  ((isr & OVW) && (END_FLAGS_GET(&pDrvCtrl->end) & (IFF_UP | IFF_RUNNING)))
	{
	cr = nicEvbReadReg (pNic, &pNic->Cr, RPAGE0);

	nicEvbWriteReg(pNic, &pNic->Imr, 0, RPAGE0); /* disable all interrupts */

        /* mark the interface -- down */
        END_FLAGS_CLR (&pDrvCtrl->end, IFF_UP | IFF_RUNNING);
        
	netJobAdd ((FUNCPTR) nicEvbRestart, (int) pDrvCtrl, cr, 0, 0, 0);
        return;
	}

    /* handle packet received */

    if  ((isr & PRX) && (END_FLAGS_GET(&pDrvCtrl->end) & (IFF_UP | IFF_RUNNING)))
	netJobAdd ((FUNCPTR) nicEvbHandleInt, (int) pDrvCtrl, 0, 0, 0, 0);
    }


/*******************************************************************************
*
* nicEvbSend - the driver's actual output routine
*
* This routine accepts outgoing packets from the snd queue, and then 
* gains exclusive access to the DMA (through a mutex semaphore),
* then calls nicEvbTransmit() to send the packet out onto the interface.
*
* RETURNS: OK, or ERROR if the packet could not be transmitted.
*/

LOCAL STATUS nicEvbSend
    (
    NICEVB_END_DEVICE* pDrvCtrl,
    M_BLK* pMblk
    )
    {
    int	status = OK;

    DRV_LOG(DRV_DEBUG_TX, "Begin nicEvbSend pDrvCtrl %p pMblk %p\n", pDrvCtrl,
            pMblk, 0, 0, 0, 0);

    if ((END_FLAGS_GET(&pDrvCtrl->end) & (IFF_UP | IFF_RUNNING)) !=
        (IFF_UP | IFF_RUNNING))
        {
        DRV_LOG(DRV_DEBUG_TX, "Device is NOT UP and RUNNING\n", 0, 0,
                0, 0, 0, 0);
	if(!NIC_IS_IN_POLL_MODE())
        netMblkClChainFree (pMblk); /* free the given mBlk chain */
        errno = EINVAL;
        return (ERROR);
        }

    if(!NIC_IS_IN_POLL_MODE())
        {
        if (semTake (pDrvCtrl->dmaSem, 100) == ERROR)	/* get DMA access */
            {
            DRV_LOG(DRV_DEBUG_TX, "nicTxStartup: can't obtain dmaSem\n",
                      0, 0, 0, 0, 0, 0);
            semTake (pDrvCtrl->dmaSem, WAIT_FOREVER);
            }
        }

    /* update statistics */
    END_ERR_ADD (&pDrvCtrl->end, MIB2_OUT_UCAST, +1);

#ifdef	NIC_INSTRUMENT
    nicTxNb++;
#endif

    /* send packet out over interface */

    if ((status = nicEvbTransmit (pDrvCtrl, pMblk, TRUE)) == ERROR)
        {

        DRV_LOG(DRV_DEBUG_TX, "FAILED nicEvbTransmit\n", 0, 0, 0, 0, 0, 0);
        
        /* update statistics */
        END_ERR_ADD (&pDrvCtrl->end, MIB2_OUT_UCAST, +1);

        /* TODO update error count */
        }

    if(!NIC_IS_IN_POLL_MODE())
	{
        semGive (pDrvCtrl->dmaSem);

    	/* free the Mblk chain */
    
    	netMblkClChainFree (pMblk);
	}

    
    DRV_LOG(DRV_DEBUG_TX, "End nicEvbSend \n", 0, 0, 0, 0, 0, 0);    
    
    return status;    
    }




/*******************************************************************************
*
* nicEvbTransmit - send data over the NIC network interface
*
* This routine transfers data to the NIC device via the remote DMA, and
* then signal for a transmission.
*
* RETURNS: OK, or ERROR if the transmitter signalled an error.
*/

LOCAL STATUS nicEvbTransmit
    (
    NICEVB_END_DEVICE* pDrvCtrl,    
    M_BLK*		pMblk,
    BOOL		waitFlag
    )
    {
    NIC_DEVICE *	pNic = pDrvCtrl->pNic;
    UINT8		cr;
    UINT8		isr;
    UINT8*		pBuf;
    int			status = OK;
    int			count;
    int			ix;
    int			len;
    int			tranxLen;

    /* find the length of the packet */    
    len = nicEvbMblkDataLenGet (pMblk);
    
    /* find the length of the packet */
    tranxLen = max (len, MINPKTSIZE);
    
#ifdef	NIC_FASTER
    nicWriteReg(pNic, &pNic->Imr, 0, RPAGE0);   /* disable all interrupts */
#endif

    nicEvbWriteReg (pNic, &pNic->Rbcr0, 0x10, RPAGE0);
    nicEvbWriteCr (pNic, RREAD);

    nicEvbWriteReg (pNic, &pNic->Rbcr0, (tranxLen & 0x00ff), RPAGE0);
    nicEvbWriteReg (pNic, &pNic->Rbcr1, (tranxLen & 0xff00) >> 8, RPAGE0);
    nicEvbWriteReg (pNic, &pNic->Rsar0, 0x00, RPAGE0);
    nicEvbWriteReg (pNic, &pNic->Rsar1, 0x00, RPAGE0);
    nicEvbWriteCr (pNic, RWRITE);
    
    count = 0;

    while (pMblk != NULL)
        {
        pBuf = (char *)pMblk->mBlkHdr.mData;
        len  = pMblk->mBlkHdr.mLen;

	for (ix = 0 ; ix < len; ix++)
	    { 
	    count++;
	    nicEvbWritePort (*pBuf++);
	    }
        pMblk = pMblk->mBlkHdr.mNext;
        }
    
    for (; count < tranxLen; count ++)		/* pad out if too short */
	nicEvbWritePort (0);

    nicEvbWriteReg (pNic, &pNic->Tpsr, 0x00, RPAGE0);
    nicEvbWriteReg (pNic, &pNic->Tbcr0, (tranxLen & 0x000000ff), RPAGE0);
    nicEvbWriteReg (pNic, &pNic->Tbcr1, ((tranxLen & 0x0000ff00) >> 8), RPAGE0);

    cr = nicEvbReadReg (pNic, &pNic->Cr, RPAGE0);
    nicEvbWriteReg(pNic, &pNic->Isr, (PTX | TXE), RPAGE0);
    nicEvbWriteCr (pNic, TXP | (cr & (RWRITE | RREAD)));	/* start Tx */

    count = 0;
    if (waitFlag)				/* wait for Tx to end ? */
	{
        while (count++ < NIC_TRANS_TIMEOUT)		/* only poll a few times */
	    {
            isr = nicEvbReadReg (pNic, &pNic->Isr, RPAGE0);

	    if (isr & TXE)			/* error encountered */
	        {
                DRV_LOG(DRV_DEBUG_TX, "ERROR in Transmition\n", 
                          0, 0, 0, 0, 0, 0);                                
                status = ERROR;
	        break;
	        }

	    if (isr & PTX)			/* Tx done */
                {
                DRV_LOG(DRV_DEBUG_TX, "Transmit Done\n", 
                          0, 0, 0, 0, 0, 0);                
	        break;
                }
            }

        if (count >= NIC_TRANS_TIMEOUT)		/* error ? */
	    {
            status = ERROR;
#ifdef	NIC_INSTRUMENT
	    nicTxTimeout++;
#endif

            DRV_LOG(DRV_DEBUG_LOAD, "nicTransmit TRANS_TIMEOUT %d\n",
                    nicTxTimeout,0,0,0,0,0);
	    }
        }

#ifdef	NIC_FASTER
    nicEvbWriteReg(pNic, &pNic->Imr, PRXE | OVWE, RPAGE0); /* re-enable intr. */
#endif
    return (status);
    }

/*******************************************************************************
*
* nicEvbMblkDataLenGet - gets the total len of the given Mblk chain packet
*
* RETURNS: length of the packet.
*
*/
LOCAL int nicEvbMblkDataLenGet
    (
    M_BLK* pMblk
    )
    {
    int nPktChainLen = 0;

    while (pMblk != NULL)
        {
        nPktChainLen += pMblk->mBlkHdr.mLen;
        pMblk = pMblk->mBlkHdr.mNext;
        }
        
    return (nPktChainLen);
    }


/*******************************************************************************
*
* nicEnetAddrGet - get the Ethernet address.
*
* Get ethernet address from the BSP.
*
* RETURNS: N/A.
*/

LOCAL void nicEvbEnetAddrGet
    (
    NICEVB_END_DEVICE*	pDrvCtrl,
    char*		pAddress
    )
    {
    UINT8		enetAdrs[6];
    UINT8		count;

    if (sysEnetAddrGet (pDrvCtrl->unit, enetAdrs) != ERROR )
        {
        for (count=0; count<6; count++)
            pAddress[count] = enetAdrs[5-count];
        }
    }


/*******************************************************************************
*
* nicEvbWritePort - write to the DMA port
*
* RETURNS: N/A.
*/

LOCAL void nicEvbWritePort
    (
    UINT8		value
    )
    {
#ifndef	NIC_FASTER
    int			level = intLock ();
#endif

    do 
	{
	*(volatile char *) NIC_PORT = value;
        }
    while (!((*(volatile UINT8 *) NIC_DMA) & ACC_MASK));
#ifndef	NIC_FASTER
    intUnlock (level);
#endif
    }    
    
/*******************************************************************************
*
* nicIoctl - the driver's I/O control routine
*
* Perform device-specific commands.
*
* RETURNS: 0, or EINVAL if the command 'cmd' is not supported.
*/

LOCAL int nicEvbIoctl
    (
    NICEVB_END_DEVICE* pDrvCtrl,
    int cmd,
    caddr_t data    
    )
    {
    int error = 0;

    DRV_LOG (DRV_DEBUG_LOAD, "nicEvbIoctl Command %d\n", cmd, 2, 3, 4, 5, 6);
    
    switch (cmd)
	{
        case EIOCSADDR:
	    if (data == NULL)
		return (EINVAL);
            bcopy ((char *)data, (char *)END_HADDR(&pDrvCtrl->end),
		   END_HADDR_LEN(&pDrvCtrl->end));
            nicEvbConfig (pDrvCtrl);	/* HELP  Will it work? */
            break;

        case EIOCGADDR:
	    if (data == NULL)
		return (EINVAL);
            bcopy ((char *)END_HADDR(&pDrvCtrl->end), (char *)data,
		    END_HADDR_LEN(&pDrvCtrl->end));
            break;

        case EIOCSFLAGS:/* no driver-dependent flags */
            break;

        case EIOCGFLAGS:
	    *(int *)data = END_FLAGS_GET(&pDrvCtrl->end);
            break;

        case EIOCMULTIADD:                   /* move to mux */
            error = nicEvbMCastAddrAdd (pDrvCtrl, (char *)data);
            break;

        case EIOCMULTIDEL:                   /* move to mux */
            error = nicEvbMCastAddrDel (pDrvCtrl, (char *)data);
            break;

        case EIOCMULTIGET:                   /* move to mux */
            error = nicEvbMCastAddrGet (pDrvCtrl, (MULTI_TABLE *)data);
            break;

        case EIOCPOLLSTART:                  /* move to mux */
            error = nicEvbPollStart (pDrvCtrl);
            break;

        case EIOCPOLLSTOP:                   /* move to mux */
            error = nicEvbPollStop (pDrvCtrl);
            break;            

        case EIOCGMIB2:
            if (data == NULL)
                return (EINVAL);
            bcopy((char *)&pDrvCtrl->end.mib2Tbl, (char *)data,
                  sizeof(pDrvCtrl->end.mib2Tbl));
            break;

        default:
            error = EINVAL;
	}
    return (error);
    } 


/*******************************************************************************
*
* nicEvbStop - stop the device
*
* This function calls BSP functions to disconnect interrupts and stop
* the device from operating in interrupt mode.
*
* RETURNS: OK or ERROR
*/

LOCAL STATUS nicEvbStop
    (
    NICEVB_END_DEVICE* pDrvCtrl
    )
    {
    STATUS	result = OK;
    NIC_DEVICE*	pNic = pDrvCtrl->pNic;    

    /* Stop the device. */
    nicEvbWriteReg(pNic, &pNic->Imr, 0, RPAGE0);  /* disable all interrupts */

    SYS_INT_DISCONNECT (pDrvCtrl, nicEvbInt, (int)pDrvCtrl, &result);

    if (result == ERROR)
	{
	DRV_LOG (DRV_DEBUG_LOAD, "Could not diconnect interrupt!\n",
                 1, 2, 3, 4, 5, 6);
	}

    return (result);
    }

/******************************************************************************
*
* nicEvbUnload - unload a driver from the system
*
* This function first brings down the device, and then frees any
* stuff that was allocated by the driver in the load function.
*/

LOCAL STATUS nicEvbUnload
    (
    NICEVB_END_DEVICE* pDrvCtrl
    )
    {
    if (pDrvCtrl != NULL)
        {
        END_OBJECT_UNLOAD (&pDrvCtrl->end);

        /* free cluster memory */
        if (pDrvCtrl->pRxCluster != NULL)
            free (pDrvCtrl->pRxCluster);
        }
    return (OK);
    }



/*******************************************************************************
*
* nicEvbMCastAddrAdd - add a multicast address
*
*
* RETURNS: OK on success, ERROR otherwise.
*/

LOCAL STATUS nicEvbMCastAddrAdd
    (
    NICEVB_END_DEVICE *	pDrvCtrl,
    char *	pAddr
    )
    {
    int retVal;

    DRV_LOG (DRV_DEBUG_IOCTL, "MCastAddrAdd\n", 0, 0, 0, 0, 0, 0);

    retVal = etherMultiAdd (&pDrvCtrl->end.multiList, pAddr);

    if (retVal == ENETRESET)
        nicEvbAddrFilterSet (pDrvCtrl, pAddr, TRUE);

    return OK; /*((retVal == OK) ? OK : ERROR);*/
    }

/*******************************************************************************
*
* nicEvbMCastAddrDel - remove a multicast address
*
*
* RETURNS: OK on success, ERROR otherwise.
*/

LOCAL STATUS nicEvbMCastAddrDel
    (
    NICEVB_END_DEVICE *	pDrvCtrl,
    char *	pAddr
    )
    {
    int retVal;

    DRV_LOG (DRV_DEBUG_IOCTL, "MCastAddrDel\n", 0, 0, 0, 0, 0, 0);

    retVal = etherMultiDel (&pDrvCtrl->end.multiList, pAddr);

    if (retVal == ENETRESET)
        nicEvbAddrFilterSet (pDrvCtrl, pAddr, FALSE);

    return OK; /*((retVal == OK) ? OK : ERROR);*/
    }


/*******************************************************************************
*
* nicEvbMCastAddrGet - retreive current multicast address list
*
*
* RETURNS: OK on success; otherwise ERROR.
*/

LOCAL STATUS nicEvbMCastAddrGet
    (
    NICEVB_END_DEVICE *	pDrvCtrl,
    MULTI_TABLE *pTable
    )
    {
    DRV_LOG (DRV_DEBUG_IOCTL, "MCastAddrGet\n", 0, 0, 0, 0, 0, 0);

    return (etherMultiGet (&pDrvCtrl->end.multiList, pTable));
    }


/*(FIXED)*/

/*******************************************************************************
*
* nicEvbPollStart - starting polling mode
*
* RETURNS: OK, always.
*/

LOCAL STATUS nicEvbPollStart
    (
    NICEVB_END_DEVICE *	pDrvCtrl
    )
    {
    int		intLevel;
    NIC_DEVICE*	pNic = pDrvCtrl->pNic;		/* address of NIC chip */

    DRV_LOG (DRV_DEBUG_POLL, "S ", 0, 0, 0, 0, 0, 0);

    intLevel = intLock();

    /* disable all interrupts */
    nicEvbWriteReg (pNic, &pNic->Imr, 0x00, RPAGE0);
    DRV_FLAGS_SET(NIC_FLAG_POLL);

    intUnlock (intLevel);

    return (OK);    
    }

/*******************************************************************************
*
* nicEvbPollStop - stop polling mode
*
* RETURNS: OK, always.
*/

LOCAL STATUS nicEvbPollStop
    (
    NICEVB_END_DEVICE *	pDrvCtrl
    )
    {
    NIC_DEVICE*	pNic = pDrvCtrl->pNic;		/* address of NIC chip */
    int		intLevel;

    intLevel = intLock();

    nicEvbWriteReg (pNic, &pNic->Imr, PRXE | OVWE, RPAGE0); /* enable int */
    DRV_FLAGS_CLR(NIC_FLAG_POLL);

    DRV_LOG (DRV_DEBUG_POLL, "s", 0, 0, 0, 0, 0, 0);
    
    intUnlock (intLevel);

    return (OK);
    }

/*******************************************************************************
*
* nicEvbPollSend - send a packet in polled mode
*
* RETURNS: OK on success, EAGAIN on failure
*/

LOCAL STATUS nicEvbPollSend
    (
    NICEVB_END_DEVICE*	pDrvCtrl,
    M_BLK*		pMblk
    )
    {
    return (nicEvbSend (pDrvCtrl, pMblk));
    }

/*******************************************************************************
*
* nicEvbPollReceive - get a packet in polled mode
*
* RETURNS: OK on success, EAGAIN on failure.
*/

LOCAL STATUS nicEvbPollReceive
    (
    NICEVB_END_DEVICE*	pDrvCtrl,
    M_BLK*		pMblk
    )
    {
    STATUS	nRetValue = OK;
    NIC_CLUSTER	pCluster;
    int		len;
    
    DRV_LOG (DRV_DEBUG_POLL, "Start Poll Read!\n", 1, 2, 3, 4, 5, 6);
    if ((pCluster = nicEvbReadFrame (pDrvCtrl)) == NULL)
        nRetValue = EAGAIN;
    else
        {
        NIC_RX_FRAME* pRx = (NIC_RX_FRAME*)pCluster;
        
        len = pRx->rxHdr.cntL + (pRx->rxHdr.cntH << 8) - NIC_ETH_CRC_LEN;
    
	pMblk->mBlkHdr.mFlags |= M_PKTHDR; /* set the packet header */
	pMblk->mBlkHdr.mLen = len;	   /* set the data len */
	pMblk->mBlkPktHdr.len = len;	   /* set the total len */

        NIC_FRAME_DATA_ADDR_GET(pCluster);

        bcopy (pCluster, (char *)pMblk->mBlkHdr.mData, len);
        netClFree (pDrvCtrl->end.pNetPool, (char*)pRx);
        
        }
    
    DRV_LOG (DRV_DEBUG_POLL, "End Poll Read!\n", 1, 2, 3, 4, 5, 6);        
    return nRetValue;
    }


/*******************************************************************************
*
* nicEvbHashIndex - compute the hash index for an ethernet address
*
* RETURNS: hash index for an ethernet address.
*/

LOCAL int nicEvbHashIndex
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
                crc ^= NIC_CRC_POLY;
                crc |= 0x1;
                }
            eAddrByte >>= 1;
            }
        }

    /* Just want the 6 most significant bits. */
    
    index = crc >> 26 ;

    return index;
    }
 

/******************************************************************************
*
* nicEvbAddrFilterSet - set the address filter for multicast addresses
*
* This routine goes through all of the multicast addresses on the list
* of addresses (added with the MCastAddrAdd() routine) and sets the
* device's filter correctly.
*
* NOMANUAL
*/

LOCAL void nicEvbAddrFilterSet
    (
    NICEVB_END_DEVICE	*pDrvCtrl,
    char*		pAddr,
    BOOL		bSet
    )
    {

    UINT8		nHashIndex;


    /* get hash index for the address */
    nHashIndex = nicEvbHashIndex (pAddr);

    /* Turn on the corresponding bit in the filter. */
    nicEvbMARSet (pDrvCtrl, nHashIndex, bSet);

    }

/*******************************************************************************
*
* nicEvbMARSet - sets/resets the MAR for the specified hash index
*
* This routine sets/resets the MAR bit for the specified hash index
*
* RETURNS: OK or ERROR.
*/
LOCAL void nicEvbMARSet
    (
    NICEVB_END_DEVICE*	pDrvCtrl,    
    UINT8		index,		/* hash index */
    BOOL		bSet		/* Set/Reset */
    )
    {
    UINT8	nRegOffset	= index;
    UINT8	nBitPosition	= index;
    UINT8	nBitMask = (UINT8)0x01;
    UINT8	nValue;
    char*	pMAR = (char*)pDrvCtrl->pNic;
    
    /* Finde register and bit position */
    nBitPosition = nBitPosition & 0x07;  /* 3 LSB bits */
    nRegOffset >>= 3;		     /* next 3 bits */
    nBitMask <<= nBitPosition;
    pMAR += NIC_IR_MAR0 + nRegOffset;

    /* set the bit in bit array*/
    nValue = nicEvbReadReg (pDrvCtrl->pNic, pMAR, RPAGE1);

    DRV_LOG (DRV_DEBUG_LOAD, "Hash Index:%d MAR Offset:%d value:%d\n",
             index, nRegOffset, nValue, 4, 5, 6);
    
    if (bSet)
        {
        nValue |= nBitMask;	/* set */
        }
    else
        {
        nBitMask = ~nBitMask;	/* reset */
        nValue &= nBitMask;        
        }
    
    nicEvbWriteReg (pDrvCtrl->pNic, pMAR, nValue, RPAGE1);
    
    DRV_LOG (DRV_DEBUG_LOAD, "Hash Index:%d MAR Offset:%d value:%d\n",
             index, nRegOffset, nValue, 4, 5, 6);
    
    }

/* (OLD) */




/*******************************************************************************
*
* nicReset - reset of interface
*
* This routine performs a software reset of the ST-NIC device.
*
* RETURNS: N/A.
*
* NOMANUAL
*/

LOCAL void nicEvbReset
    (
    NICEVB_END_DEVICE*		pDrvCtrl
    )
    {

    NIC_DEVICE *	pNic = pDrvCtrl->pNic;

    nicEvbWriteCr (pNic, STP | ABORT | RPAGE0);

    taskDelay (2);			/* wait at least 1.6 mSec */
    }





