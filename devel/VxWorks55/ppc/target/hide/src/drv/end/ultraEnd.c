/* ultraEnd.c - SMC Ultra Elite END network interface driver */

/* Copyright 1984-2002 Wind River Systems, Inc. */
#include "copyright_wrs.h"

/*
modification history
--------------------
01h,14jan02,dat  (more) Removing warnings from Diab compiler
01g,21sep01,dat  Removing ANSI errors for diab compiler
01f,28aug00,stv  corrected the handling of EIOCSFLAGS ioctl (SPR# 29423).
01e,02aug00,stv	 removed netMblkClChainFree() in Pollsend routine (SPR# 32885).
01d,11jun00,ham  removed reference to etherLib.
01c,27jan00,dat  fixed use of NULL, removed warnings
01b,09mar99,sbs  changed SYS_INT_ENABLE and SYS_INT_DISABLE to use
                 device specific BSP routines. 
01a,06jan99,dra	 written.
*/

/*
DESCRIPTION
This module implements the SMC Elite Ultra Ethernt network interface driver.

This driver supports single transmission and multiple reception.  The
Current register is a write pointer to the ring.  The Bound register
is a read pointer from the ring.  This driver gets the Current
register at the interrupt level and sets the Bound register at the
task level.  The interrupt is only masked during configuration or in
polled mode.

CONFIGURATION
The W1 jumper should be set in the position of "Software Configuration".
The defined I/O address in config.h must match the one stored in EEROM. 
The RAM address, the RAM size, and the IRQ level are defined in config.h. 
IRQ levels 2,3,5,7,10,11,15 are supported.

EXTERNAL SUPPORT REQUIREMENTS
This driver requires several external support functions, defined as macros:
.CS
    SYS_INT_CONNECT(pDrvCtrl, routine, arg)
    SYS_INT_DISCONNECT (pDrvCtrl, routine, arg)
    SYS_INT_ENABLE(pDrvCtrl)
    SYS_INT_DISABLE(pDrvCtrl)
    SYS_IN_BYTE(pDrvCtrl, reg, pData)
    SYS_OUT_BYTE(pDrvCtrl, reg, pData)
.CE

These macros allow the driver to be customized for BSPs that use
special versions of these routines.

The macro SYS_INT_CONNECT is used to connect the interrupt handler to
the appropriate vector.  By default it is the routine intConnect().

The macro SYS_INT_DISCONNECT is used to disconnect the interrupt handler prior
to unloading the module.  By default this is a dummy routine that
returns OK.

The macro SYS_INT_ENABLE is used to enable the interrupt level for the
end device.  It is called once during initialization. It calls an
external board level routine sysUltraIntEnable().

The macro SYS_INT_DISABLE is used to disable the interrupt level for the
end device.  It is called once during shutdown. It calls an
external board level routine sysUltraIntDisable().

The macros SYS_IN_BYTE and SYS_OUT_BYTE are used for accessing the
ultra device.  The default macros map these operations onto
sysInByte() and sysOutByte().

INCLUDES:
end.h endLib.h etherMultiLib.h

SEE ALSO: muxLib, endLib
.I "Writing an Enhanced Network Driver"
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
#include "drv/end/ultraEnd.h"

/* defines */

/* Configuration items */

#define ULTRA_SPEED_10M		10000000	/* 10Mbs */
#define ULTRA_SPEED		ULTRA_SPEED_10M

/* Definitions for the flags field */

#define ULTRA_POLLING			0x01
#define ULTRA_TX_IN_PROGRESS		0x02
#define ULTRA_TX_BLOCKED		0x04
#define ULTRA_RCV_HANDLING_FLAG		0x08

/* Debug macros */

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
#define	DRV_DEBUG_ERR		0x0080

#define DRV_LOG(FLG, X0, X1, X2, X3, X4, X5, X6)		\
    do								\
        {							\
	if (ultraDebug & (FLG))					\
	    logMsg (X0, X1, X2, X3, X4, X5, X6);		\
	}							\
    while (0)

#define DRV_PRINT(FLG, X)					\
    do								\
        {							\
	if (ultraDebug & (FLG))					\
	    printf X;						\
	}							\
    while (0)

#else /* DRV_DEBUG */

#define DRV_LOG(FLG, X0, X1, X2, X3, X4, X5, X6)	do {} while (0)
#define DRV_PRINT(FLG, X)				do {} while (0)

#endif /* DRV_DEBUG */

/* Cache macros */

#define ULTRA_CACHE_INVALIDATE(address, len) \
        CACHE_DRV_INVALIDATE (&pDrvCtrl->cacheFuncs, (address), (len))

/*
 * Default macro definitions for BSP interface.  These macros can be
 * redefined in a wrapper file, to generate a new module with an
 * optimized interface.
 */

/* Macro to connect interrupt handler to vector */

#ifndef SYS_INT_CONNECT
#define SYS_INT_CONNECT(pDrvCtrl,rtn,arg,pResult) \
    { \
    IMPORT STATUS sysIntConnect(); \
    *(pResult) = intConnect ((VOIDFUNCPTR *)INUM_TO_IVEC ((pDrvCtrl)->ivec), \
			   rtn, (int)(arg)); \
    }
#endif

/* Macro to disconnect interrupt handler from vector */

LOCAL VOID dummyIsr (void) { }

#ifndef SYS_INT_DISCONNECT
#define SYS_INT_DISCONNECT(pDrvCtrl,rtn,arg,pResult) \
    { \
    IMPORT STATUS intConnect(); \
    *(pResult) = intConnect ((VOIDFUNCPTR *)INUM_TO_IVEC ((pDrvCtrl)->ivec), \
			     dummyIsr, (int)(arg)); \
    }
#endif

/* Macro to enable the appropriate interrupt level */

#ifndef SYS_INT_ENABLE
#define SYS_INT_ENABLE(pDrvCtrl) \
	{ \
        IMPORT void sysUltraIntEnable(); \
	sysUltraIntEnable ((pDrvCtrl)->intLevel); \
	}
#endif

/* Macro to disable the appropriate interrupt level */

#ifndef SYS_INT_DISABLE
#define SYS_INT_DISABLE(pDrvCtrl) \
	{ \
        IMPORT void sysUltraIntDisable(); \
	sysUltraIntDisable ((pDrvCtrl)->intLevel); \
	}
#endif

/*
 * Macros to do a byte access to the chip.  Default assumes an
 * I/O mapped device accessed in the x86 fashion.
 */

#ifndef SYS_OUT_BYTE
#define SYS_OUT_BYTE(pDrvCtrl, addr, value) \
	 (sysOutByte ((pDrvCtrl)->ioAddr+(addr), value))
#endif

#ifndef SYS_IN_BYTE
#define SYS_IN_BYTE(pDrvCtrl, addr, pData) \
	 (*(pData) = sysInByte ((pDrvCtrl)->ioAddr+(addr)))
#endif

/* A shortcut for getting the hardware address from the MIB II stuff. */

#define END_HADDR(pEnd)	\
		((pEnd)->mib2Tbl.ifPhysAddress.phyAddress)

#define END_HADDR_LEN(pEnd) \
		((pEnd)->mib2Tbl.ifPhysAddress.addrLength)

/* typedefs */

/* The definition of the driver control structure */

typedef struct ultra_device
    {
    END_OBJ     endObj;			/* The class we inherit from. */
    END_ERR	lastError;		/* Last error passed to muxError */
    int		unit;			/* unit number */
    int         ivec;                   /* interrupt vector */
    int         ilevel;			/* interrupt level */
    int		ioAddr;			/* address of ultra's shared memory */
    int		memAddr;		/* address of ultra's shared memory */
    int		memSize;		/* size of ultra's shared memory */
    int		config;			/* device config (TP, BNC) */
    int		offset;			/* packet data offset */
    UINT	transmitPage[2];	/* start of TX packets */
    UINT	transmitCnt;		/* number of TX packets sent */
    UCHAR	current;		/* location of current RX packet */
    UCHAR	nextPacket;		/* location of next RX packet */
    UCHAR	istat;			/* current interrupt status */
    UCHAR	tstat;			/* current TX status */
    UCHAR	rstat;			/* current RX status */
    UCHAR	uppByteCnt;		/* last upper byte count */
    UCHAR	intLevel;		/* interrupt level from table */
    ULONG	flags;			/* Our local flags. */
    UCHAR	enetAddr[6];		/* ethernet address */
    UCHAR	mcastFilter[8];		/* multicast address mask */
    CACHE_FUNCS cacheFuncs;             /* cache function pointers */
    CL_POOL_ID	clPoolId;		/* cluster pool */
    } ULTRA_DEVICE;

/* globals */

#ifdef	DRV_DEBUG
int ultraDebug = 0x00;
#endif	/* DRV_DEBUG */

/* locals */

LOCAL IRQ_TABLE irqTable [] = 
    {
    { 2, 0x15},    { 2, 0x15},    { 2, 0x15},    { 3, 0x19},
    { 3, 0x19},    { 5, 0x1d},    { 5, 0x1d},    { 7, 0x51},
    { 7, 0x51},    { 7, 0x51},    {10, 0x55},    {11, 0x59},
    {11, 0x59},    {11, 0x59},    {11, 0x59},    {15, 0x5d}
    };

/* forward declarations */

IMPORT	int endMultiLstCnt (END_OBJ* pEnd);

LOCAL void	ultraInt	(ULTRA_DEVICE *pDrvCtrl);
LOCAL STATUS	ultraRecv	(ULTRA_DEVICE *pDrvCtrl);
LOCAL void	ultraConfig	(ULTRA_DEVICE *pDrvCtrl);
LOCAL int	ultraPacketGet  (ULTRA_DEVICE *pDrvCtrl, char *pDst);
LOCAL void	ultraAddrFilterSet (ULTRA_DEVICE *pDrvCtrl);
LOCAL void	ultraHandleRcvInt  (ULTRA_DEVICE *pDrvCtrl);

LOCAL STATUS	ultraParse	(ULTRA_DEVICE* pDrvCtrl, char* initString);
LOCAL STATUS	ultraMemInit	(ULTRA_DEVICE* pDrvCtrl, int clNum);
LOCAL STATUS	ultraPollStart	(ULTRA_DEVICE* pDrvCtrl);
LOCAL STATUS	ultraPollStop	(ULTRA_DEVICE* pDrvCtrl);
LOCAL void	ultraReset	(ULTRA_DEVICE* pDrvCtrl);

/* Ultra Specific interfaces. */

LOCAL STATUS	ultraStart	(void* pObj);
LOCAL STATUS	ultraStop	(void* pObj);
LOCAL STATUS	ultraUnload	(void* pObj);
LOCAL int	ultraIoctl	(void* pObj, int cmd, caddr_t data);
LOCAL STATUS	ultraSend	(void* pObj, M_BLK_ID pBuf);
			  
LOCAL STATUS	ultraMCastAdd	(void* pObj, char* pAddress);
LOCAL STATUS	ultraMCastDel	(void* pObj, char* pAddress);
LOCAL STATUS	ultraMCastGet	(void* pObj, MULTI_TABLE* pTable);
LOCAL STATUS	ultraPollSend	(void* pObj, M_BLK_ID pBuf);
LOCAL STATUS	ultraPollRcv	(void* pObj, M_BLK_ID pBuf);

/* This is the only externally visible interface. */

END_OBJ* 	ultraLoad	(char* initString);

/*
 * Declare our function table.  This is static across all driver
 * instances.
 */

LOCAL NET_FUNCS ultraFuncTable =
    {
    (FUNCPTR)ultraStart,	/* Function to start the device. */
    (FUNCPTR)ultraStop,		/* Function to stop the device. */
    (FUNCPTR)ultraUnload,	/* Unloading function for the driver. */
    (FUNCPTR)ultraIoctl,	/* Ioctl function for the driver. */
    (FUNCPTR)ultraSend,		/* Send function for the driver. */
    (FUNCPTR)ultraMCastAdd,	/* Multicast add function for the driver */
    (FUNCPTR)ultraMCastDel,	/* Multicast delete function for the driver */
    (FUNCPTR)ultraMCastGet,	/* Multicast get function for the driver */
    (FUNCPTR)ultraPollSend,	/* Polling send function */
    (FUNCPTR)ultraPollRcv,	/* Polling receive function */
    endEtherAddressForm,	/* put address info into a NET_BUFFER */
    endEtherPacketDataGet, 	/* get pointer to data in NET_BUFFER */
    endEtherPacketAddrGet 	/* Get packet addresses. */
    };

/*******************************************************************************
*
* ultraLoad - initialize the driver and device
*
* This routine initializes the driver and the device to the operational state.
* All of the device-specific parameters are passed in <initString>, which
* expects a string of the following format:
*
* <unit>:<ioAddr>:<memAddr>:<vecNum>:<intLvl>:<config>:<offset>"
*
* This routine can be called in two modes. If it is called with an empty but
* allocated string, it places the name of this device (that is, "ultra") into 
* the <initString> and returns 0.
*
* If the string is allocated and not empty, the routine attempts to load
* the driver using the values specified in the string.
*
* RETURNS: An END object pointer, or NULL on error, or 0 and the name of the
* device if the <initString> was NULL.
*/

END_OBJ* ultraLoad
    (
    char *	initString	/* String to be parsed by the driver. */
    )
    {
    ULTRA_DEVICE *	pDrvCtrl;
    
    if (initString == NULL)
        return (NULL);
    
    if (initString[0] == EOS)
        {
        bcopy ((char *)DEV_NAME, initString, DEV_NAME_LEN);
        return (NULL);
        }
    
    DRV_LOG (DRV_DEBUG_LOAD, "ultraLoad: loading\n",
	     (int) DEV_NAME, 2, 3, 4, 5, 6);

    /* allocate the device structure */
            
    pDrvCtrl = (ULTRA_DEVICE *)calloc (sizeof (ULTRA_DEVICE), 1);
    if (pDrvCtrl == NULL)
	goto errorExit;

    /* parse the init string, filling in the device structure */

    if (ultraParse (pDrvCtrl, initString) == ERROR)
	{
	logMsg ("ultraLoad: bad initialization string\n",
		1, 2, 3, 4, 5, 6);
	goto errorExit;
	}

    DRV_LOG (DRV_DEBUG_LOAD,
	     "ultraLoad: unit=%d ivec=%x ilevel=%x ioAddr=%x memAddr=%x config=%x\n",
	     pDrvCtrl->unit,
	     pDrvCtrl->ivec, pDrvCtrl->ilevel,
	     pDrvCtrl->ioAddr,  pDrvCtrl->memAddr,
	     pDrvCtrl->config);

    /* Setup the rest of pDrvCtrl */

    pDrvCtrl->intLevel		= irqTable [pDrvCtrl->ilevel].irq;
    pDrvCtrl->transmitPage[0]	= ULTRA_TSTART0;
    pDrvCtrl->transmitPage[1]	= ULTRA_TSTART1;
    pDrvCtrl->transmitCnt	= 0;
    pDrvCtrl->memSize		= ULTRA_RAMSIZE;
    pDrvCtrl->cacheFuncs	= cacheDmaFuncs;

    /* Reset the device */

    ultraReset (pDrvCtrl);

    /* Get ethernet address from card */

    SYS_OUT_BYTE (pDrvCtrl, LAN_CMD, CMD_PS0 | CMD_STP);
    SYS_IN_BYTE (pDrvCtrl, CTRL_LAN0, &pDrvCtrl->enetAddr[0]);
    SYS_IN_BYTE (pDrvCtrl, CTRL_LAN1, &pDrvCtrl->enetAddr[1]);
    SYS_IN_BYTE (pDrvCtrl, CTRL_LAN2, &pDrvCtrl->enetAddr[2]);
    SYS_IN_BYTE (pDrvCtrl, CTRL_LAN3, &pDrvCtrl->enetAddr[3]);
    SYS_IN_BYTE (pDrvCtrl, CTRL_LAN4, &pDrvCtrl->enetAddr[4]);
    SYS_IN_BYTE (pDrvCtrl, CTRL_LAN5, &pDrvCtrl->enetAddr[5]);
    SYS_OUT_BYTE (pDrvCtrl, LAN_CMD, CMD_STP);

    /* initialize the END and MIB2 parts of the structure */

    if (END_OBJ_INIT (&pDrvCtrl->endObj, (DEV_OBJ *)pDrvCtrl, "ultra",
                      pDrvCtrl->unit, &ultraFuncTable,
                      "SMC Ultra Elite Enhanced Network Driver.") == ERROR)
	goto errorExit;

    if (END_MIB_INIT (&pDrvCtrl->endObj, M2_ifType_ethernet_csmacd,
                      &pDrvCtrl->enetAddr[0], 6, ETHERMTU,
		      ULTRA_SPEED) == ERROR)
	goto errorExit;

    /* Perform memory allocation */

    if (ultraMemInit (pDrvCtrl, 128) == ERROR)
	{
#if (CPU_FAMILY==I80X86)
	/* Probably the bootrom case, try to get by with fewer clusters. */
	if (ultraMemInit (pDrvCtrl, 8) == ERROR)
	    goto errorExit;
#else /* (CPU_FAMILY==I80X86) */
	goto errorExit;
#endif /* (CPU_FAMILY==I80X86) */
	}

    /* Reconfigure the device */

    ultraConfig (pDrvCtrl);

    DRV_LOG (DRV_DEBUG_LOAD, "ultraLoad: done\n",
	     1, 2, 3, 4, 5, 6);

    /* Set the flags to indicate readiness */

    END_OBJ_READY (&pDrvCtrl->endObj,
		    IFF_UP | IFF_RUNNING | IFF_NOTRAILERS | IFF_BROADCAST
		    | IFF_MULTICAST | IFF_SIMPLEX);
    
    return (&pDrvCtrl->endObj);

errorExit:
    DRV_LOG (DRV_DEBUG_LOAD, "ultraLoad: errorExit\n",
	     1, 2, 3, 4, 5, 6);

    if (pDrvCtrl != NULL)
	free ((char *)pDrvCtrl);

    return (NULL);
    }
   
/*******************************************************************************
*
* ultraParse - parse the init string
*
* Parse the input string.  Fill in values in the driver control structure.
* The initialization string format is:
* <unit>:<ioAddr>:<memAddr>:<vecNum>:<intLvl>:<config>:<offset>"
*
* .IP <unit>
* Device unit number, a small integer.
* .IP <ioAddr>
* I/O address
* .IP <memAddr>
* Memory address, assumed to be 16k bytes in length.
* .IP <vecNum>
* Interrupt vector number (used with sysIntConnect()).
* .IP <intLvl>
* Interrupt level.
* .IP <config>
* Ultra config (0: RJ45 + AUI(Thick)  1: RJ45 + BNC(Thin)).
* .IP <offset>
* Memory offset for alignment.
*
* RETURNS: OK, or ERROR if any arguments are invalid.
*/

LOCAL STATUS ultraParse
    (
    ULTRA_DEVICE *	pDrvCtrl,	/* device pointer */
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
    pDrvCtrl->unit = atoi (tok);

    /* I/O address. */
    
    tok = strtok_r (NULL, ":", &pHolder);
    if (tok == NULL)
	return (ERROR);
    pDrvCtrl->ioAddr = strtoul (tok, NULL, 16);

    /* Memory address. */
    
    tok = strtok_r (NULL, ":", &pHolder);
    if (tok == NULL)
	return (ERROR);
    pDrvCtrl->memAddr = strtoul (tok, NULL, 16);

    /* Interrupt vector. */

    tok = strtok_r (NULL, ":", &pHolder);
    if (tok == NULL)
	return (ERROR);
    pDrvCtrl->ivec = strtoul (tok, NULL, 16);

    /* Interrupt level. */

    tok = strtok_r (NULL, ":", &pHolder);
    if (tok == NULL)
	return (ERROR);
    pDrvCtrl->ilevel = strtoul (tok, NULL, 16);

    /* config */
    
    tok = strtok_r (NULL, ":", &pHolder);
    if (tok == NULL)
	return (ERROR);
    pDrvCtrl->config = strtoul (tok, NULL, 16);

    /* offset */
    
    tok = strtok_r (NULL, ":", &pHolder);
    if (tok == NULL)
	return (ERROR);
    pDrvCtrl->offset = strtoul (tok, NULL, 16);
    
    return (OK);
    }

/*******************************************************************************
*
* ultraMemInit - initialize memory for the chip
*
* Using data in the control structure, setup and initialize the memory
* areas needed.  If the memory address is not already specified, then allocate
* cache safe memory.
*
* RETURNS: OK or ERROR.
*/

LOCAL STATUS ultraMemInit
    (
    ULTRA_DEVICE *	pDrvCtrl,	/* device to be initialized */
    int			clNum		/* number of clusters to allocate */
    )
    {
    CL_DESC	clDesc;                      /* cluster description */
    M_CL_CONFIG	ultraMclBlkConfig;

    bzero ((char *)&ultraMclBlkConfig, sizeof(ultraMclBlkConfig));
    bzero ((char *)&clDesc, sizeof(clDesc));
    
    pDrvCtrl->endObj.pNetPool = (NET_POOL_ID) malloc (sizeof(NET_POOL));
    if (pDrvCtrl->endObj.pNetPool == NULL)
        return (ERROR);

    clDesc.clNum   = clNum;
    clDesc.clSize  = MEM_ROUND_UP(ULTRA_BUFSIZ + pDrvCtrl->offset);
    clDesc.memSize = ((clDesc.clNum * (clDesc.clSize + 4)) + 4);

    ultraMclBlkConfig.mBlkNum = clDesc.clNum * 2;
    ultraMclBlkConfig.clBlkNum = clDesc.clNum;

    /*
     * mBlk and cluster configuration memory size initialization
     * memory size adjusted to hold the netPool pointer at the head.
     */
    ultraMclBlkConfig.memSize =
      (ultraMclBlkConfig.mBlkNum * (M_BLK_SZ + sizeof (long)))
      + (ultraMclBlkConfig.clBlkNum * (CL_BLK_SZ + sizeof (long)));
    ultraMclBlkConfig.memArea = (char *) memalign(sizeof (long),
						  ultraMclBlkConfig.memSize);
    if (ultraMclBlkConfig.memArea == NULL)
	{
	free (pDrvCtrl->endObj.pNetPool);
	pDrvCtrl->endObj.pNetPool = NULL;
	return (ERROR);
	}
    
    clDesc.memArea = (char *) malloc (clDesc.memSize);
    if (clDesc.memArea == NULL)
	{
	free (ultraMclBlkConfig.memArea);
	free (pDrvCtrl->endObj.pNetPool);
	pDrvCtrl->endObj.pNetPool = NULL;
	return (ERROR);
	}

    /* Initialize the net buffer pool with transmit buffers */
    if (netPoolInit (pDrvCtrl->endObj.pNetPool, &ultraMclBlkConfig,
		     &clDesc, 1, NULL) == ERROR)
	{
	free (clDesc.memArea);
	free (ultraMclBlkConfig.memArea);
	free (pDrvCtrl->endObj.pNetPool);
	pDrvCtrl->endObj.pNetPool = NULL;
	return (ERROR);
	}

    pDrvCtrl->clPoolId = clPoolIdGet (pDrvCtrl->endObj.pNetPool,
				      ULTRA_BUFSIZ + pDrvCtrl->offset, FALSE);
    DRV_LOG (DRV_DEBUG_LOAD, "%s%d: ultraMemInit: %d clusters\n",
	     (int) DEV_NAME, pDrvCtrl->unit, clNum, 4, 5, 6);

    return (OK);
    }

/******************************************************************************
*
* ultraReset - reset the ultra device
*
* Stop and reset the Ultra chip.
*
* RETURNS: N/A
*/

LOCAL void ultraReset
    (
    ULTRA_DEVICE *	pDrvCtrl
    )
    {     
    int tmp;

    DRV_LOG (DRV_DEBUG_LOAD, "%s%d: ultraReset\n",
	     (int)DEV_NAME, pDrvCtrl->unit,
	     3, 4, 5, 6);

    SYS_OUT_BYTE (pDrvCtrl, LAN_INTMASK, 0x00);

    SYS_OUT_BYTE (pDrvCtrl, LAN_CMD, CMD_STP);
    do
	SYS_IN_BYTE (pDrvCtrl, LAN_INTSTAT, &tmp);
    while ((tmp & ISTAT_RST) != ISTAT_RST);

    SYS_OUT_BYTE (pDrvCtrl, CTRL_CON, CON_RESET);
    SYS_OUT_BYTE (pDrvCtrl, CTRL_CON, 0x00);
    SYS_OUT_BYTE (pDrvCtrl, CTRL_HARD, 0x08);
    taskDelay (sysClkRateGet () >> 2);
    SYS_OUT_BYTE (pDrvCtrl, CTRL_CON, CON_MENABLE);

    SYS_OUT_BYTE (pDrvCtrl, LAN_INTSTAT, 0xff);

    DRV_LOG (DRV_DEBUG_LOAD, "%s%d: ultraReset done\n",
	     (int)DEV_NAME, pDrvCtrl->unit,
	     3, 4, 5, 6);
    }

/******************************************************************************
*
* ultraStart - start the device
*
* This function calls BSP functions to connect interrupts and start the
* device running in interrupt mode.
*
* RETURNS: OK or ERROR
*
*/

LOCAL STATUS ultraStart
    (
    void *	pObj		/* device ID */
    )
    {
    STATUS result;
    ULTRA_DEVICE * pDrvCtrl = pObj;
    
    DRV_LOG (DRV_DEBUG_LOAD, "%s%d: ultraStart\n",
	     (int) DEV_NAME, pDrvCtrl->unit, 3, 4, 5, 6);
    
    SYS_INT_CONNECT (pDrvCtrl, ultraInt, (int)pDrvCtrl, &result);
    if (result == ERROR)
	{
	DRV_LOG (DRV_DEBUG_LOAD,
		 "%s%d: ultraStart: could not attach interrupt\n",
		 (int) DEV_NAME, pDrvCtrl->unit, 3, 4, 5, 6);
	return (ERROR);
	}

    SYS_INT_ENABLE (pDrvCtrl);

    /* start the device */

    SYS_OUT_BYTE (pDrvCtrl, LAN_CMD, CMD_STA);

    /* initialize Transmit Configuration Register */

    SYS_OUT_BYTE (pDrvCtrl, LAN_TCON, 0x00);

    /* enable interrupts */
      
    SYS_OUT_BYTE (pDrvCtrl, LAN_INTMASK, 0x00);
    SYS_OUT_BYTE (pDrvCtrl, CTRL_INT, INT_ENABLE);
    SYS_OUT_BYTE (pDrvCtrl, LAN_INTMASK,
		  IM_OVWE | IM_TXEE | IM_PTXE | IM_PRXE);
    return (OK);
    }

/******************************************************************************
*
* ultraStop - stop the device
*
* This function calls BSP functions to disconnect interrupts and stop
* the device from operating in interrupt mode.
*
* RETURNS: OK or ERROR.
*/

LOCAL STATUS ultraStop
    (
    void *	pObj		/* device to be stopped */
    )
    {     
    int		tmp;
    int		oldLevel;
    STATUS	result = OK;
    ULTRA_DEVICE * pDrvCtrl = pObj;

    DRV_LOG (DRV_DEBUG_LOAD, "%s%d: ultraStop\n",
	     (int)DEV_NAME, pDrvCtrl->unit, 3, 4, 5, 6);

    /* Stop/disable the device. */

    oldLevel = intLock ();
    SYS_OUT_BYTE (pDrvCtrl, LAN_INTMASK, 0x00);

    SYS_OUT_BYTE (pDrvCtrl, LAN_CMD, CMD_STP);
    do
	SYS_IN_BYTE (pDrvCtrl, LAN_INTSTAT, &tmp);
    while ((tmp & ISTAT_RST) != ISTAT_RST);

    SYS_OUT_BYTE (pDrvCtrl, LAN_RCON, 0x00);
    SYS_OUT_BYTE (pDrvCtrl, LAN_TCON, TCON_LB1);

    SYS_OUT_BYTE (pDrvCtrl, LAN_INTSTAT, 0xff);

    SYS_INT_DISABLE (pDrvCtrl);

    intUnlock (oldLevel);
    
    /* Remove the ISR */
    
    SYS_INT_DISCONNECT (pDrvCtrl, ultraInt, (int)pDrvCtrl, &result);

    if (result == ERROR)
	{
	DRV_LOG (DRV_DEBUG_LOAD, "Could not disconnect interrupt!\n",
		1, 2, 3, 4, 5, 6);
	}

    return (result);
    }

/******************************************************************************
*
* ultraConfig - reconfigure the interface under us.
*
* Reconfigure the interface setting promiscuous mode, and changing the
* multicast interface list.
*
* RETURNS: N/A.
*/

LOCAL void ultraConfig
    (
    ULTRA_DEVICE *	pDrvCtrl	/* device to be re-configured */
    )
    {
    int		i;
    int		tmp;
    UCHAR	rcon;
    UCHAR	memAddr;
    UCHAR	intLevel;
    UCHAR	ioAddr;
    UCHAR	eeromGcon;

    DRV_LOG (DRV_DEBUG_LOAD, "%s%d: ultraConfig: flags=%x\n",
	     (int) DEV_NAME, pDrvCtrl->unit, pDrvCtrl->flags, 4, 5, 6);
      
    /* Mask interrupts */

    SYS_OUT_BYTE (pDrvCtrl, LAN_INTMASK, 0x00);

    /* program Command Register for page 0, stop device */
    
    SYS_OUT_BYTE (pDrvCtrl, LAN_CMD, CMD_STP);
    do
	SYS_IN_BYTE (pDrvCtrl, LAN_INTSTAT, &tmp);
    while ((tmp & ISTAT_RST) != ISTAT_RST);
    
    /* get values set by EEROM */

    SYS_OUT_BYTE (pDrvCtrl, CTRL_HARD, 0x80);
    SYS_IN_BYTE (pDrvCtrl, CTRL_GCON, &eeromGcon);

    /* IO address, Memory address, Interrupt request */

    memAddr = (((pDrvCtrl->memAddr & 0x1e000) >> 13)
	       | ((pDrvCtrl->memAddr & 0x20000) >> 10));
    intLevel = (((pDrvCtrl->intLevel & 0x03) << 2)
		| ((pDrvCtrl->intLevel & 0x04) << 6));
    ioAddr = (((pDrvCtrl->ioAddr & 0xe000) >> 8)
	      | ((pDrvCtrl->ioAddr & 0x01e0) >> 4));
    SYS_OUT_BYTE (pDrvCtrl, CTRL_HARD, 0x80);
    SYS_OUT_BYTE (pDrvCtrl, CTRL_IOADDR, ioAddr);
    SYS_OUT_BYTE (pDrvCtrl, CTRL_MEMADDR, memAddr | 0x10);
    if (pDrvCtrl->config == 1)
        SYS_OUT_BYTE (pDrvCtrl, CTRL_GCON,
		      irqTable [pDrvCtrl->intLevel].reg | (eeromGcon & 0x20));
    else if (pDrvCtrl->config == 2)
        SYS_OUT_BYTE (pDrvCtrl, CTRL_GCON,
		      (irqTable [pDrvCtrl->intLevel].reg | 0x02
		       | (eeromGcon & 0x20)));
    else
        SYS_OUT_BYTE (pDrvCtrl, CTRL_GCON,
		      irqTable [pDrvCtrl->intLevel].reg | (eeromGcon & 0x22));

    /* set FINE16 bit in BIO register to get finer res for M16CS decode */

    SYS_IN_BYTE (pDrvCtrl, CTRL_BIO, &tmp); 
    SYS_OUT_BYTE (pDrvCtrl, CTRL_BIO, tmp | BIO_FINE16);
    SYS_OUT_BYTE (pDrvCtrl, CTRL_HARD, 0x00);

    /* 16 bit enable */

    SYS_OUT_BYTE (pDrvCtrl, CTRL_BIOS, BIOS_M16EN);

    /* initialize Data Configuration Register */

    SYS_OUT_BYTE (pDrvCtrl, LAN_DCON, DCON_BSIZE1 | DCON_BUS16);

    /* initialize Receive Configuration Register */
   
    ultraAddrFilterSet (pDrvCtrl);

    /* always accept broadcast packets */
    rcon = RCON_BROAD;
    if (END_MULTI_LST_CNT(&pDrvCtrl->endObj) > 0)
	rcon |= RCON_GROUP;
    if (END_FLAGS_GET(&pDrvCtrl->endObj) & IFF_PROMISC)
	rcon |= (RCON_PROM | RCON_GROUP);
    if (END_FLAGS_GET(&pDrvCtrl->endObj) & (IFF_PROMISC|IFF_ALLMULTI))
	{
	/* receive all multicast packets */
	rcon |= RCON_GROUP;
	/* Set multicast hashing array to all 1's */
	for (i = 0; i < 8; ++i)
	    pDrvCtrl->mcastFilter[i] = 0xff;
	}
    SYS_OUT_BYTE (pDrvCtrl, LAN_RCON, rcon);

    if (rcon & RCON_GROUP)
	DRV_LOG (DRV_DEBUG_IOCTL, "%s%d: Setting multicast mode on!\n",
		 (int) DEV_NAME, pDrvCtrl->unit, 3, 4, 5, 6);
    if (rcon & RCON_PROM)
	DRV_LOG (DRV_DEBUG_IOCTL, "%s%d: Setting promiscuous mode on!\n",
		 (int) DEV_NAME, pDrvCtrl->unit, 3, 4, 5, 6);

    /* place the Ultra in external LOOPBACK mode */

    SYS_OUT_BYTE (pDrvCtrl, LAN_TCON, TCON_LB1);

    /* initialize Receive Buffer Ring */

    SYS_OUT_BYTE (pDrvCtrl, LAN_RSTART, ULTRA_PSTART);
    SYS_OUT_BYTE (pDrvCtrl, LAN_RSTOP, ULTRA_PSTOP);
    SYS_OUT_BYTE (pDrvCtrl, LAN_BOUND, ULTRA_PSTART);

    /* clear Interrupt Status Register */
 
    SYS_OUT_BYTE (pDrvCtrl, LAN_INTSTAT, 0xff);

    /* initialize Interrupt Mask Register */

    SYS_OUT_BYTE (pDrvCtrl, LAN_INTMASK, 0x00);

    /* Set command register for page 1 */

    SYS_OUT_BYTE (pDrvCtrl, LAN_CMD, CMD_PS0 | CMD_STP);

    /* Program ethernet address (page 1) */

    SYS_OUT_BYTE (pDrvCtrl, LAN_STA0, pDrvCtrl->enetAddr[0]);
    SYS_OUT_BYTE (pDrvCtrl, LAN_STA1, pDrvCtrl->enetAddr[1]);
    SYS_OUT_BYTE (pDrvCtrl, LAN_STA2, pDrvCtrl->enetAddr[2]);
    SYS_OUT_BYTE (pDrvCtrl, LAN_STA3, pDrvCtrl->enetAddr[3]);
    SYS_OUT_BYTE (pDrvCtrl, LAN_STA4, pDrvCtrl->enetAddr[4]);
    SYS_OUT_BYTE (pDrvCtrl, LAN_STA5, pDrvCtrl->enetAddr[5]);

    /* Program multicast mask (page 1) */

    SYS_OUT_BYTE (pDrvCtrl, LAN_MAR0, pDrvCtrl->mcastFilter[0]);
    SYS_OUT_BYTE (pDrvCtrl, LAN_MAR1, pDrvCtrl->mcastFilter[1]);
    SYS_OUT_BYTE (pDrvCtrl, LAN_MAR2, pDrvCtrl->mcastFilter[2]);
    SYS_OUT_BYTE (pDrvCtrl, LAN_MAR3, pDrvCtrl->mcastFilter[3]);
    SYS_OUT_BYTE (pDrvCtrl, LAN_MAR4, pDrvCtrl->mcastFilter[4]);
    SYS_OUT_BYTE (pDrvCtrl, LAN_MAR5, pDrvCtrl->mcastFilter[5]);
    SYS_OUT_BYTE (pDrvCtrl, LAN_MAR6, pDrvCtrl->mcastFilter[6]);
    SYS_OUT_BYTE (pDrvCtrl, LAN_MAR7, pDrvCtrl->mcastFilter[7]);

    /* Initialize current page pointer (page 1) */

    SYS_OUT_BYTE (pDrvCtrl, LAN_CURR, ULTRA_PSTART + 1);
    pDrvCtrl->nextPacket = ULTRA_PSTART + 1;

    /* Switch to page 2 registers */

    SYS_OUT_BYTE (pDrvCtrl, LAN_CMD, CMD_PS1 | CMD_STP);

    /* Program wait states (0) */

    SYS_OUT_BYTE (pDrvCtrl, LAN_ENH, 0x00);

    /* put the Ultra in START mode */

    SYS_OUT_BYTE (pDrvCtrl, LAN_CMD, CMD_STA);

    /* Take interface out of loopback */

    SYS_OUT_BYTE (pDrvCtrl, LAN_TCON, 0x00);

    if (!(pDrvCtrl->flags & ULTRA_POLLING))
	{
	SYS_OUT_BYTE (pDrvCtrl, CTRL_INT, INT_ENABLE);
	SYS_OUT_BYTE (pDrvCtrl, LAN_INTMASK,
		      IM_OVWE | IM_TXEE | IM_PTXE | IM_PRXE);
	}
    }

/******************************************************************************
*
* ultraInt - handle controller interrupt
*
* This routine is called at interrupt level in response to an interrupt from
* the controller.
*
* RETURNS: N/A.
*/

LOCAL void ultraInt
    (
    ULTRA_DEVICE *	pDrvCtrl	/* interrupting device */
    )
    {
    UCHAR istat;
    
    /* Acknowledge interrupt, get Transmit/Receive status */

    pDrvCtrl->istat = 0;
    do
	{
	SYS_IN_BYTE (pDrvCtrl, LAN_INTSTAT, &istat);
	SYS_OUT_BYTE (pDrvCtrl, LAN_INTSTAT, istat);
	pDrvCtrl->istat |= istat;
	}
    while (istat != 0);
    SYS_IN_BYTE (pDrvCtrl, LAN_TSTAT, &pDrvCtrl->tstat);
    SYS_IN_BYTE (pDrvCtrl, LAN_RSTAT, &pDrvCtrl->rstat);

    DRV_LOG (DRV_DEBUG_INT,
	     "%s%d: istat %x tstat %x rstat %x flags %x\n",
	     (int) DEV_NAME, pDrvCtrl->unit,
	     pDrvCtrl->istat, pDrvCtrl->tstat,
	     pDrvCtrl->rstat, pDrvCtrl->flags);

    /* Receive-error */

    if (pDrvCtrl->istat & ISTAT_RXE)
	END_ERR_ADD (&pDrvCtrl->endObj, MIB2_IN_ERRS, +1);

    /* Receive, Overwrite */

    if (pDrvCtrl->istat & (ISTAT_PRX | ISTAT_OVW))
	{
	if (pDrvCtrl->istat & ISTAT_OVW)
	    {
	    END_ERR_ADD (&pDrvCtrl->endObj, MIB2_IN_ERRS, +1);
	    DRV_LOG (DRV_DEBUG_RX | DRV_DEBUG_INT | DRV_DEBUG_ERR,
		     "%s%d: overwrite error\n",
		     (int) DEV_NAME, pDrvCtrl->unit, 3, 4, 5, 6);
	    }
	if (!(pDrvCtrl->flags & ULTRA_RCV_HANDLING_FLAG))
	    {
	    if (netJobAdd ((FUNCPTR) ultraHandleRcvInt,
			   (int) pDrvCtrl, 0, 0, 0, 0) == ERROR)
		{
		/* Error - will have to wait until next RX interrupt recv'd */
		logMsg ("%s%d: failed to queue ultraHandleRcvInt\n",
			(int) DEV_NAME, pDrvCtrl->unit, 3, 4, 5, 6);
		/* failed netJobAdd causes panic, stop device */
		SYS_OUT_BYTE (pDrvCtrl, LAN_CMD, CMD_STP);
		return;
		}
	    pDrvCtrl->flags |= ULTRA_RCV_HANDLING_FLAG;
	    SYS_OUT_BYTE (pDrvCtrl, LAN_CMD, CMD_PS0);
	    SYS_IN_BYTE (pDrvCtrl, LAN_CURR, &pDrvCtrl->current);
	    SYS_OUT_BYTE (pDrvCtrl, LAN_CMD, 0);
	    DRV_LOG (DRV_DEBUG_RX | DRV_DEBUG_INT,
		     "%s%d: queued ultraHandleRcvInt: curr=%x next=%x\n",
		     (int) DEV_NAME, pDrvCtrl->unit,
		     pDrvCtrl->current, pDrvCtrl->nextPacket, 5, 6);
	    }
	}

    /* Transmit-error, Transmit */

    if (pDrvCtrl->istat & (ISTAT_TXE | ISTAT_PTX))
	{
	if (pDrvCtrl->istat & ISTAT_TXE)
	    END_ERR_ADD (&pDrvCtrl->endObj, MIB2_OUT_UCAST, -1);
	if ((pDrvCtrl->istat & ISTAT_TXE)
	    || (pDrvCtrl->tstat & (TSTAT_ABORT | TSTAT_UNDER | TSTAT_CDH
				   | TSTAT_OWC | TSTAT_PTX)))
	    END_ERR_ADD (&pDrvCtrl->endObj, MIB2_OUT_ERRS, +1);
	pDrvCtrl->flags &= ~ULTRA_TX_IN_PROGRESS;
	if (pDrvCtrl->flags & ULTRA_TX_BLOCKED)
	    {
	    DRV_LOG (DRV_DEBUG_TX | DRV_DEBUG_INT,
		     "%s%d: queuing muxTxRestart\n",
		     (int) DEV_NAME, pDrvCtrl->unit, 3, 4, 5, 6);
	    if (netJobAdd ((FUNCPTR)muxTxRestart,
			   (int) &pDrvCtrl->endObj, 0, 0, 0, 0) == ERROR)
		{
		/* Error - can't queue, transmitter is hung. */
		logMsg ("%s%d: failed to queue muxTxRestart\n",
			(int) DEV_NAME, pDrvCtrl->unit, 3, 4, 5, 6);
		/* failed netJobAdd causes panic, stop device */
		SYS_OUT_BYTE (pDrvCtrl, LAN_CMD, CMD_STP);
		return;
		}
	    pDrvCtrl->flags &= ~ULTRA_TX_BLOCKED;
	    }
	}
    }

/******************************************************************************
*
* ultraPacketGet - get next received message
*
* Get next received packet into the supplied buffer.
*
* RETURNS: length of the next packet, or a negative number if none available.
*/

LOCAL int ultraPacketGet
    (
    ULTRA_DEVICE *	pDrvCtrl,
    char *		pDst
    )
    {
    int		len;
    char *	pSrc;
    int		wrapSize = 0;
    UINT	packetSize;
    int		oldLevel;
    ULTRA_HEADER * pH;

    if (pDrvCtrl->nextPacket == pDrvCtrl->current)
	return (0);

    /* Invalidate cache and get pointer to header */

    ULTRA_CACHE_INVALIDATE ((UCHAR *)(pDrvCtrl->memAddr + (ULTRA_PSTART << 8)),
			    (pDrvCtrl->memSize - (ULTRA_PSTART << 8)));
    pH = (ULTRA_HEADER *)(pDrvCtrl->memAddr + (pDrvCtrl->nextPacket << 8));

    DRV_LOG (DRV_DEBUG_RX, "%s%d: ultraPacketGet: %02x/%d/%02x/%02x\n",
	     (int) DEV_NAME, pDrvCtrl->unit,
	     pH->next, (((UINT)pH->uppByteCnt << 8) + pH->lowByteCnt),
	     pDrvCtrl->nextPacket, pDrvCtrl->uppByteCnt);

    if ((pH->next < ULTRA_PSTART) || (pH->next >= ULTRA_PSTOP))
	{
	pH->next = pDrvCtrl->nextPacket + pDrvCtrl->uppByteCnt + 1;
	DRV_LOG (DRV_DEBUG_RX | DRV_DEBUG_ERR,
		 "%s%d: ultraPacketGet: fix pH->next (%x)\n",
		 (int) DEV_NAME, pDrvCtrl->unit, pH->next, 4, 5, 6);
	if (pH->next >= ULTRA_PSTOP)
	    pH->next = ULTRA_PSTART + (pH->next - ULTRA_PSTOP);
	}

    if (pH->rstat & RSTAT_PRX)
	{
	/* 3Com says this status marks a packet bit-shifted in memory;
	 * the data cannot be trusted but the NIC header is OK.
	 */
	if (pH->rstat & (RSTAT_DFR | RSTAT_DIS))
	    {
	    DRV_LOG (DRV_DEBUG_RX | DRV_DEBUG_ERR,
		     "%s%d: ultraPacketGet: bit-shifted packet\n",
		     (int) DEV_NAME, pDrvCtrl->unit, 3, 4, 5, 6);
	    packetSize = -2;
	    goto doneGet;
	    }
	}
    else
	{
	DRV_LOG (DRV_DEBUG_RX | DRV_DEBUG_ERR,
		 "%s%d: ultraPacketGet: corrupted packet\n",
		 (int) DEV_NAME, pDrvCtrl->unit, 3, 4, 5, 6);
	packetSize = -2;
	goto doneGet;
	}

    /* Compute packet size, excluding Ultra header */

    pSrc = ((char *)pH) + sizeof(ULTRA_HEADER);
    packetSize = (((UINT)pH->uppByteCnt << 8) + pH->lowByteCnt
		  - sizeof(ULTRA_HEADER));
    if ((packetSize < ULTRA_MIN_SIZE) || (packetSize > ULTRA_MAX_SIZE))
	{
	END_ERR_ADD (&pDrvCtrl->endObj, MIB2_IN_ERRS, +1);
	DRV_LOG (DRV_DEBUG_RX | DRV_DEBUG_ERR,
		 "%s%d: ultraPacketGet: bad size packet %d\n",
		 (int) DEV_NAME, pDrvCtrl->unit, packetSize, 4, 5, 6);
	packetSize = -2;
	goto doneGet;
	}

    /* copy separated frame to network buffer */

    len = packetSize;
    if ((pSrc+packetSize) > (char *)(pDrvCtrl->memAddr + (ULTRA_PSTOP << 8)))
	{
	wrapSize = (char *)(pDrvCtrl->memAddr + (ULTRA_PSTOP << 8)) - pSrc;
	bcopy (pSrc, pDst, wrapSize);
	len -= wrapSize;
	pSrc = (char *) (pDrvCtrl->memAddr + (ULTRA_PSTART << 8));
	pDst += wrapSize;
	}
    bcopy (pSrc, pDst, len);
    
    DRV_LOG (DRV_DEBUG_RX, "%s%d: ultraPacketGet: c/n=%x/%x len/next=%d/%x\n",
	     (int) DEV_NAME, pDrvCtrl->unit,
	     pDrvCtrl->current, pDrvCtrl->nextPacket,
	     (((UINT)pH->uppByteCnt << 8) + pH->lowByteCnt), pH->next);
 
doneGet:
    /* Mark packet as received. */

    oldLevel = intLock ();
    pDrvCtrl->nextPacket = pH->next;
    pDrvCtrl->uppByteCnt = pH->uppByteCnt;

    if (pDrvCtrl->nextPacket != ULTRA_PSTART)
	SYS_OUT_BYTE (pDrvCtrl, LAN_BOUND, pDrvCtrl->nextPacket - 1);
    else
	SYS_OUT_BYTE (pDrvCtrl, LAN_BOUND, ULTRA_PSTOP - 1);
    intUnlock (oldLevel);

    return (packetSize);
    }

/******************************************************************************
*
* ultraRecv - process the next incoming packet.
*
* Handle one incoming packet.  The packet is checked for errors.
*
* RETURNS: OK or ERROR.
*/

LOCAL STATUS ultraRecv
    (
    ULTRA_DEVICE *	pDrvCtrl	/* interrupting device */
    )
    {
    int		len;
    char *	pBuf;
    CL_BLK_ID	pClBlk;
    M_BLK_ID	pMblk;      /* MBLK to send upstream */

    /* Allocate an MBLK, and a replacement buffer */
    pBuf = netClusterGet (pDrvCtrl->endObj.pNetPool, pDrvCtrl->clPoolId);
    if (!pBuf)
	{
	DRV_LOG (DRV_DEBUG_RX, "%s%d: ultraRecv: Out of clusters!\n",
		 (int) DEV_NAME, pDrvCtrl->unit, 0, 0, 0, 0);
	pDrvCtrl->lastError.errCode = END_ERR_WARN;
	pDrvCtrl->lastError.pMesg = "out of clusters";
	muxError (&pDrvCtrl->endObj, &pDrvCtrl->lastError);
	return (ERROR);
	}

    /* Read in offset packet so IP header is long-aligned */

    len = ultraPacketGet (pDrvCtrl, pBuf + pDrvCtrl->offset);
    if (len <= 0)
	{
	DRV_LOG (DRV_DEBUG_RX, "%s%d: ultraRecv: bad packet (%d)\n",
		 (int) DEV_NAME, pDrvCtrl->unit, len, 0, 0, 0);
	END_ERR_ADD (&pDrvCtrl->endObj, MIB2_IN_ERRS, +1);
	netClFree (pDrvCtrl->endObj.pNetPool, (UCHAR *) pBuf);
	return (OK);
	}

    pMblk = mBlkGet (pDrvCtrl->endObj.pNetPool, M_DONTWAIT, MT_DATA);
    if (!pMblk)
	{
	DRV_LOG (DRV_DEBUG_RX, "%s%d: ultraRecv: out of M blocks\n",
		 (int) DEV_NAME, pDrvCtrl->unit, 0, 0, 0, 0);
	pDrvCtrl->lastError.errCode = END_ERR_WARN;
	pDrvCtrl->lastError.pMesg = "out of M blocks.";
	muxError (&pDrvCtrl->endObj, &pDrvCtrl->lastError);
	END_ERR_ADD (&pDrvCtrl->endObj, MIB2_IN_ERRS, +1);
	netClFree (pDrvCtrl->endObj.pNetPool, (UCHAR *) pBuf);
	return (OK);
	}
    pClBlk = netClBlkGet (pDrvCtrl->endObj.pNetPool, M_DONTWAIT);
    if (!pClBlk)
	{
	DRV_LOG (DRV_DEBUG_RX, "%s%d: ultraRecv: out of CL blocks\n",
		 (int) DEV_NAME, pDrvCtrl->unit, 0, 0, 0, 0);
	pDrvCtrl->lastError.errCode = END_ERR_WARN;
	pDrvCtrl->lastError.pMesg = "out of CL blocks.";
	muxError (&pDrvCtrl->endObj, &pDrvCtrl->lastError);
	netMblkFree (pDrvCtrl->endObj.pNetPool, (M_BLK_ID)pMblk);
	netClFree (pDrvCtrl->endObj.pNetPool, (UCHAR *) pBuf);
	return (OK);
	}

    /* Associate the data pointer with the MBLK */
    netClBlkJoin (pClBlk, pBuf, ULTRA_BUFSIZ, NULL, 0, 0, 0);
    
    /* Associate the data pointer with the MBLK */
    netMblkClJoin (pMblk, pClBlk);

    pMblk->mBlkHdr.mFlags |= M_PKTHDR;
    pMblk->mBlkHdr.mLen    = len;
    pMblk->mBlkPktHdr.len  = len;
    pMblk->mBlkHdr.mData  += pDrvCtrl->offset;	/* match ultraPacketGet */

    /* Call the upper layer's receive routine. */

    END_RCV_RTN_CALL (&pDrvCtrl->endObj, pMblk);

    /* Record received packet */

    END_ERR_ADD (&pDrvCtrl->endObj, MIB2_IN_UCAST, +1);

    return (OK);
    }

/******************************************************************************
*
* ultraHandleRcvInt - task level interrupt service for input packets
*
* This routine is called at task level indirectly by the interrupt
* service routine to do any message received processing.
*
* RETURNS: N/A.
*/

LOCAL void ultraHandleRcvInt
    (
    ULTRA_DEVICE *	pDrvCtrl	/* interrupting device */
    )
    {
    DRV_LOG (DRV_DEBUG_RX,
	     "%s%d: ultraHandleRcvInt: begin, flags=%x curr=%x next=%x\n",
	     (int) DEV_NAME, pDrvCtrl->unit,
	     pDrvCtrl->flags, pDrvCtrl->current, pDrvCtrl->nextPacket, 6);

    while (TRUE)
	{
	int oldLevel;
    
	/* Refresh the current pointer, and check if done. */

	oldLevel = intLock ();
	SYS_OUT_BYTE (pDrvCtrl, LAN_CMD, CMD_PS0);
	SYS_IN_BYTE (pDrvCtrl, LAN_CURR, &pDrvCtrl->current);
	SYS_OUT_BYTE (pDrvCtrl, LAN_CMD, 0);
	if (pDrvCtrl->nextPacket == pDrvCtrl->current)
	    {
	    /* all packets consumed. */
	    pDrvCtrl->flags &= ~ULTRA_RCV_HANDLING_FLAG;
	    intUnlock (oldLevel);
	    break;
	    }
	intUnlock (oldLevel);
	if (ultraRecv (pDrvCtrl) == ERROR)
	    {
	    /*
	     * ultraRecv only errors if out of clusters or the like.  We
	     * need to exit this loop and let the netTask run for a bit
	     * in order for it to clean up its memory.
	     */
	    oldLevel = intLock ();
	    pDrvCtrl->flags &= ~ULTRA_RCV_HANDLING_FLAG;
	    intUnlock (oldLevel);
	    break;
	    }
	}

    DRV_LOG (DRV_DEBUG_RX,
	     "%s%d: ultraHandleRcvInt: done, flags=%x curr=%x next=%x\n",
	     (int) DEV_NAME, pDrvCtrl->unit,
	     pDrvCtrl->flags, pDrvCtrl->current, pDrvCtrl->nextPacket, 6);
    }

/******************************************************************************
*
* ultraSend - the driver send routine
*
* This routine takes a M_BLK_ID sends off the data in the M_BLK_ID.
* The buffer must already have the addressing information properly installed
* in it.  This is done by a higher layer.  The last arguments are a free
* routine to be called when the device is done with the buffer and a pointer
* to the argument to pass to the free routine.  
*
* RETURNS: OK or ERROR.
*/

LOCAL STATUS ultraSend
    (
    void *	pObj,		/* device ptr */
    M_BLK_ID	pMblk		/* data to send */
    )
    {
    int		oldLevel;
    int		len;
    UINT	transmitPage;
    char *	pBuf;
    ULTRA_DEVICE * pDrvCtrl = (ULTRA_DEVICE *) pObj;
       
    /*
     * Obtain exclusive access to transmitter.  This is necessary because
     * we might have more than one stack transmitting at once.
     */

    if (!(pDrvCtrl->flags & ULTRA_POLLING))
	END_TX_SEM_TAKE (&pDrvCtrl->endObj, WAIT_FOREVER);

    if (pDrvCtrl->flags & ULTRA_TX_IN_PROGRESS)
	{
	int cnt;

	/*
	 * Wait up to 1 second. Testing on a slow 486 pc platform this
	 * loop never waited more than 2 ticks at 60/sec.
	 */
	cnt = sysClkRateGet ();
	while ((pDrvCtrl->flags & ULTRA_TX_IN_PROGRESS) && (cnt-- > 0))
	    taskDelay (1);
	DRV_LOG (DRV_DEBUG_TX,
		 "%s%d: ultraSend: TX_IN_PROGRESS: waited %d ticks\n",
		 (int) DEV_NAME, pDrvCtrl->unit,
		 sysClkRateGet() - cnt, 0, 0, 0);
	}

    /* If TX still in progress, note blocked and return ERROR */
    oldLevel = intLock ();
    if (pDrvCtrl->flags & ULTRA_TX_IN_PROGRESS)
	{
	DRV_LOG (DRV_DEBUG_TX, "%s%d: ultraSend: TX_IN_PROGRESS (flags=%x)\n",
		 (int) DEV_NAME, pDrvCtrl->unit, pDrvCtrl->flags, 4, 5, 6);
	pDrvCtrl->flags |= ULTRA_TX_BLOCKED;
	if (!(pDrvCtrl->flags & ULTRA_POLLING))
	    END_TX_SEM_GIVE (&pDrvCtrl->endObj);
	intUnlock (oldLevel);
        return (END_ERR_BLOCK);
	}
    intUnlock (oldLevel);

    /* Copy and free the MBLK */

    transmitPage = pDrvCtrl->transmitPage [pDrvCtrl->transmitCnt & 1];
    pBuf = (char *)(pDrvCtrl->memAddr + (transmitPage << 8));

    len = netMblkToBufCopy (pMblk, pBuf, NULL);
    len = max (len, ETHERSMALL);
    netMblkClChainFree (pMblk);

    DRV_LOG (DRV_DEBUG_TX, "%s%d: ultraSend, packet %d, len=%d\n",
	     (int) DEV_NAME, pDrvCtrl->unit, pDrvCtrl->transmitCnt, len, 5, 6);
    
    /* Flush the write pipe */

    CACHE_PIPE_FLUSH ();
   
    /* Transfer to the device and mark TX in progress. */

    oldLevel = intLock ();
    SYS_OUT_BYTE (pDrvCtrl, LAN_TSTART, transmitPage);
    SYS_OUT_BYTE (pDrvCtrl, LAN_TCNTH, len >> 8);
    SYS_OUT_BYTE (pDrvCtrl, LAN_TCNTL, len & 0xff);
    SYS_OUT_BYTE (pDrvCtrl, LAN_CMD, CMD_TXP);
    pDrvCtrl->transmitCnt++;
    pDrvCtrl->flags |= ULTRA_TX_IN_PROGRESS;
    intUnlock (oldLevel);

    /* update statistics */

    END_ERR_ADD (&pDrvCtrl->endObj, MIB2_OUT_UCAST, +1);

    /* Release semaphore */

    if (!(pDrvCtrl->flags & ULTRA_POLLING))
	END_TX_SEM_GIVE (&pDrvCtrl->endObj);

    return (OK);
    }
  
/******************************************************************************
*
* ultraIoctl - the driver I/O control routine
*
* Process an ioctl request.
*
* RETURNS: A command specific response, usually OK or ERROR.
*/

LOCAL int ultraIoctl
    (
    void *	pObj,		/* device receiving command */
    int		cmd,		/* ioctl command code */
    caddr_t	data		/* command argument */
    )
    {
    long	value;
    int		error = 0;
    ULTRA_DEVICE * pDrvCtrl = pObj;	/* device receiving command */
    
    switch ((UINT)cmd)
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
	    ultraConfig (pDrvCtrl);
            break;

        case EIOCGFLAGS:
	    *(int *)data = END_FLAGS_GET(&pDrvCtrl->endObj);
            break;

	case EIOCPOLLSTART:	/* Begin polled operation */
	    ultraPollStart (pDrvCtrl);
	    break;

	case EIOCPOLLSTOP:	/* Ultra polled operation */
	    ultraPollStop (pDrvCtrl);
	    break;

        case EIOCGMIB2:		/* return MIB information */
            if (data == NULL)
                return (EINVAL);
            bcopy ((char *)&pDrvCtrl->endObj.mib2Tbl, (char *)data,
		   sizeof(pDrvCtrl->endObj.mib2Tbl));
            break;

        case EIOCGFBUF:		/* return minimum First Buffer for chaining */
            if (data == NULL)
                return (EINVAL);
            *(int *)data = ULTRA_MIN_FBUF;
	    break;

        case EIOCGHDRLEN:
            if (data == NULL)
                return (EINVAL);
            *(int *)data = SIZEOF_ETHERHEADER;
            break;

        default:
            error = EINVAL;
        }

    return (error);
    }

/******************************************************************************
*
* ultraAddrFilterSet - set the address filter for multicast addresses
*
* This routine goes through all of the multicast addresses on the list
* of addresses (added with the ultraMCastAdd() routine) and sets the
* device's filter correctly.
*
* RETURNS: N/A.
*/

LOCAL void ultraAddrFilterSet
    (
    ULTRA_DEVICE *	pDrvCtrl		/* device pointer */
    )
    {          
    int		len;
    int		count;
    u_char	c;
    u_char *	pCp;
    u_long	crc;
    ETHER_MULTI * pCurr;

    pDrvCtrl->mcastFilter[0] = 0x00;
    pDrvCtrl->mcastFilter[1] = 0x00;
    pDrvCtrl->mcastFilter[2] = 0x00;
    pDrvCtrl->mcastFilter[3] = 0x00;
    pDrvCtrl->mcastFilter[4] = 0x00;
    pDrvCtrl->mcastFilter[5] = 0x00;
    pDrvCtrl->mcastFilter[6] = 0x00;
    pDrvCtrl->mcastFilter[7] = 0x00;

    for (pCurr = END_MULTI_LST_FIRST (&pDrvCtrl->endObj);
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

/*****************************************************************************
*
* ultraMCastAdd - add a multicast address for the device
*
* This routine adds a multicast address to whatever the driver
* is already listening for.  It then resets the address filter.
*
* RETURNS: OK or ERROR.
*/

LOCAL STATUS ultraMCastAdd
    (
    void *	pObj,		/* device pointer */
    char *	pAddress	/* new address to add */
    )
    {     
    ULTRA_DEVICE * pDrvCtrl = pObj;	/* device pointer */

    DRV_LOG (DRV_DEBUG_LOAD, "%s%d: ultraMCastAdd\n",
	     (int) DEV_NAME, pDrvCtrl->unit, 3, 4, 5, 6);

    if (etherMultiAdd (&pDrvCtrl->endObj.multiList, pAddress) == ENETRESET)
	ultraConfig (pDrvCtrl);

    return (OK);
    }

/*****************************************************************************
*
* ultraMCastDel - delete a multicast address for the device
*
* This routine removes a multicast address from whatever the driver
* is listening for.  It then resets the address filter.
*
* RETURNS: OK or ERROR.
*/

LOCAL STATUS ultraMCastDel
    (
    void *	pObj,		/* device pointer */
    char *	pAddress	/* address to be deleted */
    )
    {
    ULTRA_DEVICE * pDrvCtrl = pObj;	/* device pointer */

    DRV_LOG (DRV_DEBUG_LOAD, "%s%d: ultraMCastDel\n",
	     (int) DEV_NAME, pDrvCtrl->unit, 3, 4, 5, 6);

    if (etherMultiDel (&pDrvCtrl->endObj.multiList, pAddress) == ENETRESET)
	ultraConfig (pDrvCtrl);

    return (OK);
    }

/*****************************************************************************
*
* ultraMCastGet - get the multicast address list for the device
*
* This routine gets the multicast list of whatever the driver
* is already listening for.
*
* RETURNS: OK or ERROR.
*/

LOCAL STATUS ultraMCastGet
    (
    void *		pObj,		/* device pointer */
    MULTI_TABLE	*	pTable		/* address table to be filled in */
    )
    {     
    ULTRA_DEVICE * pDrvCtrl = pObj;	/* device pointer */

    return (etherMultiGet (&pDrvCtrl->endObj.multiList, pTable));
    }

/******************************************************************************
*
* ultraUnload - unload a driver from the system
*
* This function first brings down the device, and then frees any
* stuff that was allocated by the driver in the load function.
*
* RETURNS: OK or ERROR.
*/

LOCAL STATUS ultraUnload
    (
    void *	pObj		/* device to be unloaded */
    )
    {     
    ULTRA_DEVICE * pDrvCtrl = pObj;	/* device to be unloaded */

    DRV_LOG (DRV_DEBUG_LOAD, "%s%d: ultraUnload\n",
	     (int) DEV_NAME, pDrvCtrl->unit,
	     3, 4, 5, 6);

    END_OBJECT_UNLOAD (&pDrvCtrl->endObj);

    return (OK);
    }


/******************************************************************************
*
* ultraPollStart - start polled mode operations
*
* RETURNS: OK or ERROR.
*/

LOCAL STATUS ultraPollStart
    (
    ULTRA_DEVICE *	pDrvCtrl
    )
    {     
    int oldLevel;

    DRV_LOG (DRV_DEBUG_POLL, "%s%d: ultraPollStart\n",
	     (int) DEV_NAME, pDrvCtrl->unit, 3, 4, 5, 6);

    oldLevel = intLock ();
    SYS_OUT_BYTE (pDrvCtrl, LAN_INTMASK, 0x00);
    pDrvCtrl->flags |= ULTRA_POLLING;
    intUnlock (oldLevel);

    return (OK);
    }

/******************************************************************************
*
* ultraPollStop - stop polled mode operations
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

LOCAL STATUS ultraPollStop
    (
    ULTRA_DEVICE *	pDrvCtrl
    )
    {     
    int	oldLevel;

    DRV_LOG (DRV_DEBUG_POLL, "%s%d: ultraPollStop\n",
	     (int) DEV_NAME, pDrvCtrl->unit, 3, 4, 5, 6);

    oldLevel = intLock ();
    SYS_OUT_BYTE (pDrvCtrl, LAN_INTMASK,
		  IM_OVWE | IM_TXEE | IM_PTXE | IM_PRXE);
    pDrvCtrl->flags &= ~ULTRA_POLLING;
    intUnlock (oldLevel);

    return (OK);
    }

/******************************************************************************
*
* ultraPollRcv - routine to receive a packet in polled mode.
*
* This routine is called by a user to try and get a packet from the
* device.
*
* RETURNS: OK upon success.  EAGAIN is returned when no packet is available.
*/

LOCAL STATUS ultraPollRcv
    (
    void *	pObj,		/* device to be polled */
    M_BLK_ID	pMblk		/* ptr to buffer */
    )
    {
    int len;
    ULTRA_DEVICE * pDrvCtrl = pObj;	/* device to be polled */

    /* Don't call DRV_LOG until past the busy-wait */

    /* If no packet is available, return immediately */

    SYS_OUT_BYTE (pDrvCtrl, LAN_CMD, CMD_PS0);
    SYS_IN_BYTE (pDrvCtrl, LAN_CURR, &pDrvCtrl->current);
    SYS_OUT_BYTE (pDrvCtrl, LAN_CMD, 0);
    if (pDrvCtrl->nextPacket == pDrvCtrl->current)
        return (EAGAIN);

    DRV_LOG (DRV_DEBUG_POLL_RX, "%s%d: ultraPollRcv\n",
	     (int) DEV_NAME, pDrvCtrl->unit, 3, 4, 5, 6);

    /* Upper layer must provide a valid buffer. */

    if ((pMblk->mBlkHdr.mLen < ULTRA_MAX_SIZE)
	|| !(pMblk->mBlkHdr.mFlags & M_EXT))
	{
        DRV_LOG (DRV_DEBUG_POLL_RX,
		 "%s%d: ultraPollRecv: bad mblk, mLen=%d mFlags=%x\n",
		 (int) DEV_NAME, pDrvCtrl->unit,
		 pMblk->mBlkHdr.mLen, pMblk->mBlkHdr.mFlags, 5, 6);
	return (EAGAIN);
	}

    /* Process device packet into net buffer */

    pMblk->mBlkHdr.mData += pDrvCtrl->offset;
    len = ultraPacketGet (pDrvCtrl, pMblk->mBlkHdr.mData);

    pMblk->mBlkHdr.mFlags |= M_PKTHDR;	/* set the packet header */
    pMblk->mBlkHdr.mLen = len;		/* set the data len */
    pMblk->mBlkPktHdr.len = len;	/* set the total len */

    DRV_LOG (DRV_DEBUG_POLL_RX, "%s%d: ultraPollRcv done, len=%d\n",
	     (int) DEV_NAME, pDrvCtrl->unit, len, 4, 5, 6);

    return (OK);
    }

/******************************************************************************
*
* ultraPollSend - routine to send a packet in polled mode.
*
* This routine is called by a user to try and send a packet on the
* device.  As this is a single-transmit device, we actually wait
* for the previous transmit to complete, otherwise parts of the
* packet may be dropped.
*
* RETURNS: OK upon success.  EAGAIN if device is busy.
*/

LOCAL STATUS ultraPollSend
    (
    void *	pObj,		/* device to be polled */
    M_BLK_ID	pMblk		/* packet to send */
    )
    {
    UCHAR	cmdStat;
    int		len;
    UINT	transmitPage;
    char *	pBuf;
    ULTRA_DEVICE * pDrvCtrl = pObj;

    DRV_LOG (DRV_DEBUG_POLL_TX, "%s%d: ultraPollSend\n",
	     (int) DEV_NAME, pDrvCtrl->unit, 3, 4, 5, 6);

    /* Wait for the last transmit to complete */

    do
	SYS_IN_BYTE (pDrvCtrl, LAN_CMD, &cmdStat);
    while (cmdStat & CMD_TXP);

    /* Process the net buffer into a device transmit packet */

    transmitPage = pDrvCtrl->transmitPage [pDrvCtrl->transmitCnt & 1];
    pBuf = (char *)(pDrvCtrl->memAddr + (transmitPage << 8));
    len = netMblkToBufCopy (pMblk, pBuf, NULL);
    len = max (len, ETHERSMALL);

    DRV_LOG (DRV_DEBUG_POLL_TX, "%s%d: ultraPollSend: len=%d\n",
	     (int) DEV_NAME, pDrvCtrl->unit, len, 4, 5, 6);

    /* Flush the write pipe */

    CACHE_PIPE_FLUSH ();
   
    /* Transfer to the device */

    SYS_OUT_BYTE (pDrvCtrl, LAN_TSTART, transmitPage);
    SYS_OUT_BYTE (pDrvCtrl, LAN_TCNTH, len >> 8);
    SYS_OUT_BYTE (pDrvCtrl, LAN_TCNTL, len & 0xff);
    SYS_OUT_BYTE (pDrvCtrl, LAN_CMD, CMD_TXP);
    pDrvCtrl->transmitCnt++;

    /* Bump the statistic counter. */

    END_ERR_ADD (&pDrvCtrl->endObj, MIB2_OUT_UCAST, +1);

    DRV_LOG (DRV_DEBUG_POLL_TX, "%s%d: ultraPollSend: done\n",
	     (int) DEV_NAME, pDrvCtrl->unit, 3, 4, 5, 6);

    return (OK);
    }
