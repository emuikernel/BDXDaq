/* iOlicomEnd.c - END style Intel Olicom PCMCIA network interface driver */

/* Copyright 1997-2002 Wind River Systems, Inc. */

#include "copyright_wrs.h"

/*
modification history
--------------------
01q,14jan02,dat  Removing warnings from Diab compiler
01p,20sep01,dat  Removing ANSI errors, for diab compiler
01o,28aug00,stv  corrected the handling of EIOCSFLAGS ioctl (SPR# 29423).
01n,11jun00,ham  removed reference to etherLib.
01m,16nov99,jpd  fixed warnings caused by new definition of NULL.
01l,01dec99,stv  freed mBlk chain before returning ERROR (SPR #28492).
01k,11nov99,cn   removed unnecessary freeing of pDrvCtrl in iOlicomUnload
		 (SPR# 28772).
01j,16nov98,jpd  claim buffers using malloc() not cacheDmaMalloc(). Fix bug
		 parsing CIS. Improved efficiency. Tidied. Fix problem after
		 (hard) PID7T reset.
01i,29mar99,dat  SPR 26119, documentation, usage of .bS/.bE
01h,30sep98,dbt  Adding missing code in poll receive routine.
01g,22sep98,dat  SPR 22325, System mode transition.
01f,14may98,jgn  disable RX stop interrupts, tidy comments (SPR #21202)
01e,13may98,jgn  fixed driver lock up on PID (SPR #21202)
01d,11may98,jpd  fixed incorrect parameters in strtok_r calls (SPR #21192).
01c,09apr98,jpd  made conditionally-compilable version for Brutus or PID.
		 fixed fault in iOlicomRxIntHandle().
01b,24mar98,jpd  modified to work on Brutus, but not yet on both Brutus and PID.
01a,16dec97,rlp	 modified if_oli.c to END style
*/

/*
This module implements the Olicom (Intel 82595TX) network interface
driver.  The physical device is a PCMCIA card.  This driver also houses
code to manage a Vadem PCMCIA Interface controller on the ARM PID
board, which is strictly a subsystem in it's own right.

This network interface driver does not include support for trailer protocols
or data chaining.  However, buffer loaning has been implemented in an effort to
boost performance.

BOARD LAYOUT
The device resides on a PCMCIA card and is soft configured.
No jumpering diagram is necessary.

EXTERNAL INTERFACE
This driver provides the END external interface with the following
exceptions.  The only external interface is the iOlicomEndLoad() routine.
All of the paramters are passed as strings in a colon (:) separated list
to the load function as an initString.  The iOlicomEndLoad() function uses
strtok() to parse the string.

The string contains the target specific parameters like this:

 "<io_baseA>:<attr_baseA>:<mem_baseA>:<io_baseB>:<attr_baseB>:<mem_baseB>: \
  <ctrl_base>:<intVectA>:<intLevelA>:<intVectB>:<intLevelB>: \
  <txBdNum>:<rxBdNum>:<pShMem>:<shMemSize>"

TARGET-SPECIFIC PARAMETERS

.iP "I/O base address A"

This is the first parameter passed to the driver init string. This
parameter indicates the base address of the PCMCIA I/O space for socket
A.

.iP "Attribute base address A"

This is the second parameter passed to the driver init string. This
parameter indicates the base address of the PCMCIA attribute space for
socket A. On the PID board, this should be the offset of the beginning
of the attribute space from the beginning of the memory space.

.iP "Memory base address A"

This is the third parameter passed to the driver init string. This
parameter indicates the base address of the PCMCIA memory space for
socket A.

.iP "I/O base address B"

This is the fourth parameter passed to the driver init string. This
parameter indicates the base address of the PCMCIA I/O space for socket
B.

.iP "Attribute base address B"

This is the fifth parameter passed to the driver init string. This
parameter indicates the base address of the PCMCIA attribute space for
socket B. On the PID board, this should be the offset of the beginning
of the attribute space from the beginning of the memory space.

.iP "Memory base address B"

This is the sixth parameter passed to the driver init string. This
parameter indicates the base address of the PCMCIA memory space for
socket B.

.iP "PCMCIA controller base address"

This is the seventh parameter passed to the driver init string. This
parameter indicates the base address of the Vadem PCMCIA controller.

.iP "interrupt vectors and levels"

These are the eighth, ninth, tenth and eleventh parameters passed to the
driver init string.

The mapping of IRQs generated at the Card/PCMCIA level to interrupt
levels and vectors is system dependent. Furthermore the slot holding
the PCMCIA card is not initially known. The interrupt levels and
vectors for both socket A and socket B must be passed to
iOlicomEndLoad(), allowing the driver to select the required parameters
later.

.iP "number of transmit and receive buffer descriptors"

These are the twelfth and thirteenth parameters passed to the driver
init string.

The number of transmit and receive buffer descriptors (BDs) used is
configurable by the user upon attaching the driver. There must be a
minimum of two transmit and two receive BDs, and there is a maximum of
twenty transmit and twenty receive BDs.  If this parameter is "NULL" a
default value of 16 BDs will be used.

.iP "offset"

This is the fourteenth parameter passed to the driver in the init string.

This parameter defines the offset which is used to solve alignment problem.

.iP "base address of buffer pool"

This is the fifteenth parameter passed to the driver in the init string.

This parameter is used to notify the driver that space for the transmit
and receive buffers need not be allocated, but should be taken from a
private memory space provided by the user at the given address.  The
user should be aware that memory used for buffers must be 4-byte
aligned but need not be non-cacheable. If this parameter is "NONE",
space for buffers will be obtained by calling malloc() in
iOlicomEndLoad().

.iP "mem size of buffer pool"

This is the sixteenth parameter passed to the driver in the init string.

The memory size parameter specifies the size of the pre-allocated memory
region. If memory base is specified as NONE (-1), the driver ignores this
parameter.

.iP "Ethernet address"

This parameter is obtained from the Card Information Structure on the
Olicom PCMCIA card.

.LP

EXTERNAL SUPPORT REQUIREMENTS

This driver requires three external support function:

.iP "void sysLanIntEnable (int level)" "" 9 -1

This routine provides a target-specific interface for enabling Ethernet
device interrupts at a specified interrupt level.  This routine is
called each time that the iOlicomStart() routine is called.

.iP "void sysLanIntDisable (int level)" "" 9 -1

This routine provides a target-specific interface for disabling
Ethernet device interrupts.  The driver calls this routine from the
iOlicomStop() routine each time a unit is disabled.

.iP "void sysBusIntAck(void)" "" 9 -1

This routine acknowledge the interrupt if it's necessary.

.LP

SEE ALSO: muxLib, endLib
.I "Intel 82595TX ISA/PCMCIA High Integration Ethernet Controller User Manual,"
.I "Vadem VG-468 PC Card Socket Controller Data Manual."
*/

/* includes */

#include "vxWorks.h"
#include "iv.h"
#include "taskLib.h"
#include "memLib.h"
#include "errno.h"
#include "errnoLib.h"
#include "intLib.h"
#include "cacheLib.h"
#include "netLib.h"
#include "stdio.h"
#include "stdlib.h"
#include "logLib.h"
#include "sysLib.h"
#include "net/if.h"
#include "netinet/if_ether.h"
#include "drv/end/iOlicomEnd.h"		/* Common defines. */
#include "etherMultiLib.h"		/* multicast stuff. */
#include "end.h"			/* Common END structures. */

#include "endLib.h"
#include "lstLib.h"			/* Needed to maintain protocol list. */


/* defines */

#define RX_BD_SIZ	sizeof(RX_BD)
#define TX_BD_SIZ	sizeof(TX_BD)

#define LOG_MSG(X0, X1, X2, X3, X4, X5, X6)			\
	{							\
	    if (_func_logMsg != NULL)				\
		_func_logMsg(X0, X1, X2, X3, X4, X5, X6);	\
	}

#undef DEBUG

#ifdef  DEBUG
#define LOCAL
#define END_DEBUG_OFF           0x0000
#define END_DEBUG_RX            0x0001
#define END_DEBUG_TX            0x0002
#define END_DEBUG_INT           0x0004
#define END_DEBUG_POLL          (END_DEBUG_POLL_RX | END_DEBUG_POLL_TX)
#define END_DEBUG_POLL_RX       0x0008
#define END_DEBUG_POLL_TX       0x0010
#define END_DEBUG_LOAD          0x0020
#define END_DEBUG_IOCTL         0x0040
#define END_DEBUG_ADDR		0x0080
#define END_DEBUG_ALL_FUNCS     0x0100
#define END_DEBUG_LOAD_FAIL	0x0200
#define END_DEBUG_POLL_REDIR    0x10000
#define END_DEBUG_LOG_NVRAM     0x20000
#define END_DEBUG_TUPLES	0x40000

int     oliDebug = END_DEBUG_LOAD_FAIL;

#define END_LOG_MSG(FLG, X0, X1, X2, X3, X4, X5, X6)    \
        if (oliDebug & FLG)                             \
            LOG_MSG(X0, X1, X2, X3, X4, X5, X6);

#else /* DEBUG */

#define END_LOG_MSG(DBG_SW, X0, X1, X2, X3, X4, X5, X6)

#endif /* DEBUG */

/*
 * Default macro definitions for BSP interface.
 * These macros can be redefined in a wrapper file, to generate
 * a new module with an optimized interface.
 */

#ifndef SYS_INT_CONNECT
#define SYS_INT_CONNECT(pDrvCtrl,rtn,arg,pResult) \
    { \
    IMPORT STATUS intConnect(); \
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
    IMPORT STATUS sysLanIntEnable(); \
    sysLanIntEnable (pDrvCtrl->ilevel); \
    }
#endif /*SYS_INT_ENABLE*/

#ifndef SYS_INT_DISABLE
#define SYS_INT_DISABLE(pDrvCtrl) \
    { \
    IMPORT STATUS sysLanIntDisable(); \
    sysLanIntDisable (pDrvCtrl->ilevel); \
    }
#endif /*SYS_INT_DISABLE*/

#ifndef SYS_BUS_INT_ACK
#define SYS_BUS_INT_ACK(pDrvCtrl) \
    { \
    IMPORT int sysBusIntAck(); \
    sysBusIntAck (pDrvCtrl->ilevel); \
    }
#endif /*SYS_BUS_INT_ACK*/

/* A shortcut for getting the hardware address from the MIB II stuff. */

#define END_HADDR(pEnd)	\
		((pEnd)->mib2Tbl.ifPhysAddress.phyAddress)

#define END_HADDR_LEN(pEnd) \
		((pEnd)->mib2Tbl.ifPhysAddress.addrLength)

#ifndef PCMCIA_NO_VADEM
/* Vadem access macros */

#ifndef OLI_VADEM_READ
#define OLI_VADEM_READ(index) \
	iOlicomVademRead(pDrvCtrl->pcmcia.ctrlBase, index)
#endif /* OLI_VADEM_READ */

#ifndef OLI_VADEM_WRITE
#define OLI_VADEM_WRITE(index,val) \
	iOlicomVademWrite(pDrvCtrl->pcmcia.ctrlBase, index, val)
#endif /* OLI_VADEM_WRITE */

#ifndef OLI_VADEM_UNLOCK
#define OLI_VADEM_UNLOCK() \
	iOlicomVademUnlock(pDrvCtrl->pcmcia.ctrlBase)
#endif /* OLI_VADEM_UNLOCK */
#endif /* !PCMCIA_NO_VADEM */

/* END_DEVICE flags access macros */

#define DRV_FLAGS_SET(setBits)		\
        (pDrvCtrl->flags |= (setBits))

#define DRV_FLAGS_ISSET(setBits)	\
        (pDrvCtrl->flags & (setBits))

#define DRV_FLAGS_CLR(clrBits)		\
        (pDrvCtrl->flags &= ~(clrBits))

#define DRV_FLAGS_GET()			\
        (pDrvCtrl->flags)

#define NET_BUF_ALLOC()		\
    netClusterGet (pDrvCtrl->endObj.pNetPool, pDrvCtrl->clPoolId)

#define NET_BUF_FREE(pBuf)	\
    netClFree (pDrvCtrl->endObj.pNetPool, (UINT8 *)pBuf)

#define NET_MBLK_ALLOC()	\
    mBlkGet (pDrvCtrl->endObj.pNetPool, M_DONTWAIT, MT_DATA)

#define NET_MBLK_FREE(pMblk)	\
    netMblkFree (pDrvCtrl->endObj.pNetPool, (M_BLK_ID)pMblk)

#define NET_CL_BLK_ALLOC()	\
    clBlkGet (pDrvCtrl->endObj.pNetPool, M_DONTWAIT)

#define NET_CL_BLK_FREE(pClblk)	\
    clBlkFree (pDrvCtrl->endObj.pNetPool, (CL_BLK_ID)pClBlk)

#define NET_MBLK_BUF_FREE(pMblk)	\
    netMblkClFree ((M_BLK_ID)pMblk)

#define NET_MBLK_CHAIN_FREE(pMblk)	\
    {					\
    M_BLK *pNext;			\
					\
    pNext=pMblk;			\
    while (pNext)			\
        pNext=NET_MBLK_BUF_FREE (pNext);\
    }

#define NET_MBLK_CL_JOIN(pMblk, pClBlk)	\
    netMblkClJoin ((pMblk), (pClBlk))

#define NET_CL_BLK_JOIN(pClBlk, pBuf, len)	\
    netClBlkJoin ((pClBlk), (pBuf), (len), NULL, 0, 0, 0)

/* globals */

#ifdef DEBUG
    END_DEVICE * pDbgDrvCtrl;
#endif


/* local */

/* forward static functions */

LOCAL void	iOlicomReset (END_DEVICE * pDrvCtrl);
LOCAL void	iOlicomInt (END_DEVICE * pDrvCtrl);
LOCAL void	iOlicomRxIntHandle (END_DEVICE * pDrvCtrl);
LOCAL STATUS	iOlicomRecv (END_DEVICE * pDrvCtrl, RX_BD * pData);
LOCAL void	iOlicomConfig (END_DEVICE * pDrvCtrl);
LOCAL STATUS	iOlicomAddrFilterSet (END_DEVICE * pDrvCtrl);
LOCAL STATUS	iOlicomInit (END_DEVICE * pDrvCtrl);
LOCAL void	iOlicomTxBdQueueClean (END_DEVICE * pDrvCtrl);
LOCAL void	iOlicomTxStart (END_DEVICE * pDrvCtrl);
LOCAL void	iOlicomTxRestart (END_DEVICE * pDrvCtrl);
LOCAL RX_BD *	iOlicomPacketGet (END_DEVICE * pDrvCtrl);
LOCAL TX_BD *	iOlicomTxBdGet (END_DEVICE * pDrvCtrl);

LOCAL STATUS	iOlicomPcmciaSetup (END_DEVICE * pDrvCtrl);
LOCAL void	iOlicomInitialiseSlot (END_DEVICE * pDrvCtrl, UINT socket);
LOCAL void	iOlicomShutdownSlot (END_DEVICE * pDrvCtrl, UINT socket);
LOCAL UINT	iOlicomPcmciaReadTuple (END_DEVICE * pDrvCtrl, UINT offset,
			UCHAR * buffer, UINT buflen);
LOCAL UCHAR *	iOlicomPcmciaFindTuple (END_DEVICE * pDrvCtrl, UINT tuple,
			UINT * offset, UCHAR * buffer, UINT buflen);
LOCAL STATUS	iOlicomReadCardAttributes (END_DEVICE * pDrvCtrl);
LOCAL STATUS	iOlicomCardInsertion (END_DEVICE * pDrvCtrl, UINT socket);
LOCAL STATUS	iOlicomInitialiseCard (END_DEVICE * pDrvCtrl);
LOCAL STATUS	iOlicomMapIOSpace (END_DEVICE * pDrvCtrl, UINT base, UINT range,
			int mapirq);
#ifndef PCMCIA_NO_VADEM
LOCAL void	iOlicomMapMemory (END_DEVICE * pDrvCtrl, UINT socket,
			UINT window, UINT base, UINT range, UINT offset,
			MEMTYPE type);
LOCAL UINT	iOlicomVademRead (char * ctrlBase, UINT index);
LOCAL void	iOlicomVademWrite (char * ctrlBase, UINT index, UINT  value);
LOCAL void	iOlicomVademUnlock (char * ctrlBase);
#endif /* !PCMCIA_NO_VADEM */

/* END Specific interfaces. */

/* These are the only externally visible interfaces. */

END_OBJ * 	iOlicomEndLoad (char * initString);
void		iOlicomIntHandle (END_DEVICE * pDrvCtrl);

LOCAL STATUS	iOlicomStart (END_DEVICE * pDrvCtrl);
LOCAL STATUS	iOlicomStop (END_DEVICE * pDrvCtrl);
LOCAL STATUS	iOlicomUnload ();
LOCAL int	iOlicomIoctl (END_DEVICE * pDrvCtrl, int cmd, char * data);
LOCAL STATUS	iOlicomSend (END_DEVICE * pDrvCtrl, M_BLK * pMblk);

LOCAL STATUS	iOlicomMCastAddrAdd (END_DEVICE * pDrvCtrl, char * pAddress);
LOCAL STATUS	iOlicomMCastAddrDel (END_DEVICE * pDrvCtrl, char * pAddress);
LOCAL STATUS	iOlicomMCastAddrGet (END_DEVICE * pDrvCtrl,
				    MULTI_TABLE * pTable);
LOCAL STATUS	iOlicomPollSend (END_DEVICE * pDrvCtrl, M_BLK * pMblk);
LOCAL STATUS	iOlicomPollReceive (END_DEVICE * pDrvCtrl, M_BLK * pMblk);
LOCAL STATUS	iOlicomPollStart (END_DEVICE * pDrvCtrl);
LOCAL STATUS	iOlicomPollStop (END_DEVICE * pDrvCtrl);

LOCAL STATUS	iOlicomInitParse ();
LOCAL STATUS	iOlicomInitMem ();

/*
 * Declare our function table.  This is static across all driver
 * instances.
 */

LOCAL NET_FUNCS iOlicomFuncTable =
    {
    (FUNCPTR) iOlicomStart,	/* Function to start the device. */
    (FUNCPTR) iOlicomStop,	/* Function to stop the device. */
    (FUNCPTR) iOlicomUnload,	/* Unloading function for the driver. */
    (FUNCPTR) iOlicomIoctl,	/* Ioctl function for the driver. */
    (FUNCPTR) iOlicomSend,	/* Send function for the driver. */
    (FUNCPTR) iOlicomMCastAddrAdd, /* Multicast address add fn for driver */
    (FUNCPTR) iOlicomMCastAddrDel, /* Multicast address delete fn for driver. */
    (FUNCPTR) iOlicomMCastAddrGet, /* Multicast table retrieve fn for driver. */
    (FUNCPTR) iOlicomPollSend,	   /* Polling send function for driver. */
    (FUNCPTR) iOlicomPollReceive,  /* Polling receive function for driver. */
    endEtherAddressForm,/* put address info into a NET_BUFFER */
    endEtherPacketDataGet, /* get pointer to data in NET_BUFFER */
    endEtherPacketAddrGet  /* Get packet addresses. */
    };

/*******************************************************************************
*
* iOlicomEndLoad - initialize the driver and device
*
* This routine initializes the driver and the device to the operational state.
* All of the device specific parameters are passed in the initString.
*
* This routine can be called in two modes. If it is called with an empty, but
* allocated string then it places the name of this device (i.e. oli) into the
* initString and returns 0.
*
* If the string is allocated then the routine attempts to perform its load
* functionality.
*
* RETURNS: An END object pointer or NULL on error or 0 and the name of the
* device if the initString was NULL.
*/

END_OBJ * iOlicomEndLoad
    (
    char *	initString	/* String to be parsed by the driver. */
    )
    {
    END_DEVICE *	pDrvCtrl;

    END_LOG_MSG (END_DEBUG_LOAD, "Loading oli...\n", 0, 0, 0, 0, 0, 0);

    if (initString == NULL)
	{
	END_LOG_MSG (END_DEBUG_LOAD_FAIL, "initString was null\n",
		     0, 0, 0, 0, 0, 0);
        return (NULL);
	}

    if (initString[0] == EOS)
        {
        memcpy(initString, (char *)DRV_NAME, DRV_NAME_LEN);
	END_LOG_MSG (END_DEBUG_LOAD_FAIL, "initString[0] was null\n",
		     0, 0, 0, 0, 0, 0);
        return (0);
        }

    /* allocate the device structure */

    pDrvCtrl = (END_DEVICE *)calloc (sizeof (END_DEVICE), 1);
    if (pDrvCtrl == NULL)
	{
        END_LOG_MSG (END_DEBUG_LOAD_FAIL,
		"%s%d - Failed to allocate control structure\n",
                        DRV_NAME, pDrvCtrl->unit, 0, 0, 0, 0);
	return (NULL);
	}

#ifdef DEBUG
    pDbgDrvCtrl = pDrvCtrl;
#endif

    /* parse the init string, filling in the device structure */

    if (iOlicomInitParse (pDrvCtrl, initString) == ERROR)
	{
        END_LOG_MSG (END_DEBUG_LOAD_FAIL,
                "%s%d - Failed to parse initialization parameters\n",
                        DRV_NAME, pDrvCtrl->unit, 0, 0, 0, 0);
	goto errorExit;
	}

    /* Sanity check on the unit number */

    if (pDrvCtrl->unit < 0)
	{
	END_LOG_MSG (END_DEBUG_LOAD_FAIL, "unit number < 0\n",
		     0, 0, 0, 0, 0, 0);
        goto errorExit;
	}

    /* Check if we are already attached */

    if (pDrvCtrl->endObj.attached == TRUE)
	{
        END_LOG_MSG (END_DEBUG_LOAD_FAIL, "%s - Already attached\n",
                        DRV_NAME, 0, 0, 0, 0, 0);
        goto errorExit;
	}

    /* endObject Initializations */

    if (END_OBJ_INIT (&pDrvCtrl->endObj, (DEV_OBJ*)pDrvCtrl, DRV_NAME,
                      pDrvCtrl->unit, &iOlicomFuncTable,
                      "olicom Enhanced Network Driver") == ERROR)
        {
        END_LOG_MSG (END_DEBUG_LOAD_FAIL,
                "%s%d - Failed to initialize END object\n",
                        DRV_NAME, pDrvCtrl->unit, 0, 0, 0, 0);
        goto errorExit;
        }

    /*
     * Initialise the PCMCIA side of things. I'm assuming here we have
     * complete control over the PCMCIA system, this shouldn't really
     * be done here, rather the whole job should be handled by a
     * separate PCMCIA sub-system module.
     * If there is an Olicom card in one of the slots, this will
     * return TRUE. If no card is found then I just return an error
     * and this attach() call will fail. A better implementation might
     * just mark the interface as down, bringing it up when a card
     * insertion is detected. This will suffice for a first pass though.
     */

    if (iOlicomPcmciaSetup(pDrvCtrl) != OK)
	{
        END_LOG_MSG (END_DEBUG_LOAD_FAIL,
                "%s%d - Failed to initialize the PCMCIA side\n",
                        DRV_NAME, pDrvCtrl->unit, 0, 0, 0, 0);
        goto errorExit;
	}

    /* Initialize MIB-II entries */

    if (END_MIB_INIT (&pDrvCtrl->endObj, M2_ifType_ethernet_csmacd,
                      (UINT8*) pDrvCtrl->enetAddr, EADDR_LEN,
                       ETHERMTU, OLI_SPEED) == ERROR)
        {
        END_LOG_MSG (END_DEBUG_LOAD_FAIL,
                "%s%d - MIB-II initializations failed\n",
                        DRV_NAME, pDrvCtrl->unit, 0, 0, 0, 0);
        goto errorExit;
        }

    /* Perform memory allocation/distribution */

    if (iOlicomInitMem (pDrvCtrl) == ERROR)
	{
	END_LOG_MSG (END_DEBUG_LOAD_FAIL, "initMem() failed\n",
		     0, 0, 0, 0, 0, 0);
	goto errorExit;
	}

    /* set the flags to indicate readiness */

    END_OBJ_READY (&pDrvCtrl->endObj,
	IFF_NOTRAILERS | IFF_MULTICAST | IFF_BROADCAST);

    END_LOG_MSG (END_DEBUG_LOAD, "Done loading oli...", 0, 0, 0, 0, 0, 0);

    /* Successful return */

    return (&pDrvCtrl->endObj);

    /***** Handle error cases *****/

errorExit:

        {
        iOlicomUnload (pDrvCtrl);
	taskDelay(120);
        return (NULL);
        }
    }

/*******************************************************************************
*
* iOlicomInitParse - parse the init string
*
* Parse the input string.  Fill in values in the driver control structure.
*
* The initialization string format is:
* .CS
*  "<nisa_base>:<nisa_pcmcia>:<nisa_pcmem>:<intVectA>:<intLevelA>:      \
*   <intVectB>:<intLevelB>:<txBdNum>:<rxBdNum>:<offset>:<pShMem>:<shMemSize>"
* .CE
*
* .IP <nisa_base>
* Base of NISA space
* .IP <ctrlBase>
* Base address of Vadem chip
* .IP <nisa_pcmem>
* Base address of mapped memory space
* .IP <intVectA>
* Interrupt vector for slot A
* .IP <intLevelA>
* Interrupt level for slot A
* .IP <intVectB>
* Interrupt vector for card B
* .IP <intLevelB>
* Interrupt level for card B
* .IP <txBdNum>
* number of transmit buffer descriptors; NULL = default
* .IP <rxBdNum>
* number of receive buffer descriptors; NULL = default
* .IP <offset>
* Memory offset for alignment.
* .IP <pShMem>
* Address of memory pool; NONE = malloc it
* .IP <shMemSize>
* memory pool size
* .LP
*
* RETURNS: OK or ERROR for invalid arguments.
*/

LOCAL STATUS iOlicomInitParse
    (
    END_DEVICE *	pDrvCtrl,	/* pointer to END_DEVICE structure */
    char *		initString	/* init string */
    )
    {
    char *	tok;
    char *	holder = NULL;

    /* Parse the initString */

    /* Unit number. */

    tok = strtok_r (initString, ":", &holder);
    if (tok == NULL)
	return ERROR;
    pDrvCtrl->unit = atoi (tok);


    /* Base address of I/O space for card A. */

    tok = strtok_r (NULL, ":", &holder);
    if (tok == NULL)
	return ERROR;
    pDrvCtrl->pcmcia.ioBaseA = (char *) strtoul (tok, NULL, 16);


    /* Base address of Attribute Memory space for card A. */

    tok = strtok_r (NULL, ":", &holder);
    if (tok == NULL)
	return ERROR;
    pDrvCtrl->pcmcia.attrBaseA = (char *) strtoul (tok, NULL, 16);


    /* Base address of Common memory space for card A. */

    tok = strtok_r (NULL, ":", &holder);
    if (tok == NULL)
	return ERROR;
    pDrvCtrl->pcmcia.memBaseA = (char *) strtoul (tok, NULL, 16);


    /* Base address of I/O space for card B. */

    tok = strtok_r (NULL, ":", &holder);
    if (tok == NULL)
	return ERROR;
    pDrvCtrl->pcmcia.ioBaseB = (char *) strtoul (tok, NULL, 16);


    /* Base address of Attribute Memory space for card B. */

    tok = strtok_r (NULL, ":", &holder);
    if (tok == NULL)
	return ERROR;
    pDrvCtrl->pcmcia.attrBaseB = (char *) strtoul (tok, NULL, 16);


    /* Base address of Common memory space for card B. */

    tok = strtok_r (NULL, ":", &holder);
    if (tok == NULL)
	return ERROR;
    pDrvCtrl->pcmcia.memBaseB = (char *) strtoul (tok, NULL, 16);


    /* Base of Vadem controller */

    tok = strtok_r (NULL, ":", &holder);
    if (tok == NULL)
	return ERROR;
    pDrvCtrl->pcmcia.ctrlBase = (char *) strtoul (tok, NULL, 16);


    /* Interrupt vector for slot A. */

    tok = strtok_r (NULL, ":", &holder);
    if (tok == NULL)
	return ERROR;
    pDrvCtrl->pcmcia.intVectA = strtoul (tok, NULL, 16);


    /* Interrupt level for slot A. */

    tok = strtok_r (NULL, ":", &holder);
    if (tok == NULL)
	return ERROR;
    pDrvCtrl->pcmcia.intLevelA = strtoul (tok, NULL, 16);


    /* Interrupt vector for slot B. */

    tok = strtok_r (NULL, ":", &holder);
    if (tok == NULL)
	return ERROR;
    pDrvCtrl->pcmcia.intVectB = strtoul (tok, NULL, 16);


    /* Interrupt level for slot B. */

    tok = strtok_r (NULL, ":", &holder);
    if (tok == NULL)
	return ERROR;
    pDrvCtrl->pcmcia.intLevelB = strtoul (tok, NULL, 16);


    /* number of transmit buf descriptors */

    tok = strtok_r(NULL, ":", &holder);
    if (tok == NULL)
        return ERROR;
    pDrvCtrl->txBdNum = strtoul (tok, NULL, 16);


    /* number of receive buf descriptors */

    tok = strtok_r(NULL, ":", &holder);
    if (tok == NULL)
        return ERROR;
    pDrvCtrl->rxBdNum = strtoul (tok, NULL, 16);


    /* Caller supplied alignment offset. */

    tok = strtok_r (NULL, ":", &holder);
    if (tok == NULL)
        return ERROR;
    pDrvCtrl->offset = atoi (tok);


    /* Address of memory pool. */

    tok = strtok_r (NULL, ":", &holder);
    if (tok == NULL)
	return ERROR;
    pDrvCtrl->pShMem = (char *) strtoul (tok, NULL, 16);


    /* memory pool size. */

    tok = strtok_r (NULL, ":", &holder);
    if (tok == NULL)
	return ERROR;
    pDrvCtrl->shMemSize = strtoul (tok, NULL, 16);

    END_LOG_MSG (END_DEBUG_LOAD,
            "EndLoad: unit=%d pShMem=0x%X shMemSize=0x%X ctrlBase=0x%X\n",
	    pDrvCtrl->unit, pDrvCtrl->pShMem, pDrvCtrl->shMemSize,
	    pDrvCtrl->pcmcia.ctrlBase, 0, 0);

    END_LOG_MSG (END_DEBUG_LOAD,
            "ioBaseA=0x%X attrBaseA=0x%X memBaseA=0x%X\n"
            "ioBaseB=0x%X attrBaseB=0x%X memBaseB=0x%X\n",
	     (int)pDrvCtrl->pcmcia.ioBaseA, (int)pDrvCtrl->pcmcia.attrBaseA,
	     (int)pDrvCtrl->pcmcia.memBaseA, (int)pDrvCtrl->pcmcia.ioBaseB,
	     (int)pDrvCtrl->pcmcia.attrBaseB, (int)pDrvCtrl->pcmcia.memBaseB);

    END_LOG_MSG (END_DEBUG_LOAD,
             "intLevelA=%#x intLevelB %#x txBdNum %#x rxBdNum %#x offset=%#x\n",
	      pDrvCtrl->pcmcia.intLevelA, pDrvCtrl->pcmcia.intLevelB,
	      pDrvCtrl->txBdNum, pDrvCtrl->rxBdNum, pDrvCtrl->offset, 0);

    END_LOG_MSG (END_DEBUG_LOAD, "Processed all arguments\n", 0, 0, 0, 0, 0, 0);

    return (OK);
    }

/*******************************************************************************
*
* iOlicomInitMem - initialize memory for the chip
*
* Using data in the control structure, setup and initialize the memory
* areas needed.  If the memory address is not already specified, then allocate
* memory.
*
* RETURNS: OK or ERROR.
*/

LOCAL STATUS iOlicomInitMem
    (
    END_DEVICE *	pDrvCtrl	/* pointer to END_DEVICE structure */
    )
    {
    M_CL_CONFIG oliMclBlkConfig;
    CL_DESC     clDesc;                      /* cluster description */
    void *      pBuf;
    void *      pMemBase;
    int         ix;
    ULONG	size;

    /* Use default number of buffer descriptors if user did not specify */

    if (pDrvCtrl->txBdNum == 0)
        pDrvCtrl->txBdNum = TX_BD_DEFAULT;
    if (pDrvCtrl->rxBdNum == 0)
        pDrvCtrl->rxBdNum = RX_BD_DEFAULT;

    /* must be at least two transmit and receive buffer descriptors */

    pDrvCtrl->txBdNum = max (TX_BD_MIN, pDrvCtrl->txBdNum);
    pDrvCtrl->rxBdNum = max (RX_BD_MIN, pDrvCtrl->rxBdNum);

    /* Establish a region of memory.
     *
     * OK. We now know how much shared memory we need.  If the caller
     * provides a specific memory region, we check to see if the provided
     * region is large enough for our needs.  If the caller did not
     * provide a specific region, then we attempt to allocate the memory
     * from the system.
     */

    if ((int) pDrvCtrl->pShMem != NONE)
	{
	/* caller provided memory */

	size = ((pDrvCtrl->rxBdNum * (OLI_BUFSIZ + RX_BD_SIZ + 8)) + 4 +
		(pDrvCtrl->txBdNum * (OLI_BUFSIZ + TX_BD_SIZ + 8)) + 4 +
		(NUM_LOAN * (OLI_BUFSIZ + 8)) + 4);

	if (pDrvCtrl->shMemSize < size )	/* not enough space */
	    {
	    END_LOG_MSG (END_DEBUG_LOAD, "%s%d - not enough memory provided\n",
			DRV_NAME, pDrvCtrl->unit, 0, 0, 0, 0);
            return ( ERROR );
            }

	pMemBase = pDrvCtrl->pShMem;	/* set the beginning of pool */

        /* copy null structure */

        pDrvCtrl->cacheFuncs = cacheNullFuncs;
	}
    else
        {

	size = (((pDrvCtrl->rxBdNum + 1) * RX_BD_SIZ) +
		((pDrvCtrl->txBdNum + 1) * TX_BD_SIZ));

        /* We must allocate memory for buffer descriptors */

	pDrvCtrl->pShMem =
		pMemBase = (char *) malloc (size);

	if (pMemBase == NULL)
	    {
            END_LOG_MSG (END_DEBUG_LOAD, "%s%d - system memory unavailable\n",
                        DRV_NAME, pDrvCtrl->unit, 0, 0, 0, 0);
	    return (ERROR);
	    }

	pDrvCtrl->shMemSize = size;

        /* copy the DMA structure */

        pDrvCtrl->cacheFuncs = cacheNullFuncs;

	/* Trasmit and receive descriptors allocated by driver */

	DRV_FLAGS_SET(OLI_MEMOWN);
        }

    /* zero the shared memory */

    memset (pMemBase, 0, (int) size);

    /* carve Tx memory structure */

    pDrvCtrl->txBdBase = (TX_BD *) pDrvCtrl->pShMem;

    /* carve Rx memory structure */

    pDrvCtrl->rxBdBase = (RX_BD *) (pDrvCtrl->txBdBase + pDrvCtrl->txBdNum);

    /* Initialize net buffer pool for tx/rx buffers */

    memset ((char *)&oliMclBlkConfig, 0, sizeof(oliMclBlkConfig));
    memset ((char *)&clDesc, 0, sizeof(clDesc));

    oliMclBlkConfig.mBlkNum  = pDrvCtrl->rxBdNum * 4;
    clDesc.clNum            = pDrvCtrl->rxBdNum + pDrvCtrl->txBdNum + NUM_LOAN;
    oliMclBlkConfig.clBlkNum = clDesc.clNum;

    /*
     * mBlk and cluster configuration memory size initialization
     * memory size adjusted to hold the netPool pointer at the head.
     */

    oliMclBlkConfig.memSize = ((oliMclBlkConfig.mBlkNum *
                               (MSIZE + sizeof (long))) +
                              (oliMclBlkConfig.clBlkNum *
                               (CL_BLK_SZ + sizeof (long))));

    if ((oliMclBlkConfig.memArea = (char *)memalign(sizeof (long),
			oliMclBlkConfig.memSize)) == NULL)
        return (ERROR);

    clDesc.clSize       = OLI_BUFSIZ;
    clDesc.memSize      = ((clDesc.clNum * (clDesc.clSize + 4)) + 4);

    if (DRV_FLAGS_ISSET(OLI_MEMOWN))
        {
        clDesc.memArea = malloc (clDesc.memSize);
        if (clDesc.memArea == NULL)
            {
            END_LOG_MSG (END_DEBUG_LOAD, "%s%d - system memory unavailable\n",
                        DRV_NAME, pDrvCtrl->unit, 0, 0, 0, 0);
            return (ERROR);
            }
        }
    else
        clDesc.memArea = (char *) (pDrvCtrl->rxBdBase + pDrvCtrl->rxBdNum);

    if ((pDrvCtrl->endObj.pNetPool = malloc (sizeof(NET_POOL))) == NULL)
        return (ERROR);

    /* Initialize the net buffer pool with transmit buffers */

    if (netPoolInit (pDrvCtrl->endObj.pNetPool, &oliMclBlkConfig,
                     &clDesc, 1, NULL) == ERROR)
        {
        END_LOG_MSG (END_DEBUG_LOAD, "%s%d - netPoolInit failed\n",
        		DRV_NAME, pDrvCtrl->unit, 0, 0, 0, 0);
        return (ERROR);
        }

    /* Save the cluster pool id */

    pDrvCtrl->clPoolId = clPoolIdGet (pDrvCtrl->endObj.pNetPool,
                                      OLI_BUFSIZ, FALSE);

    /* Setup the receive ring */

    for (ix = 0; ix < pDrvCtrl->rxBdNum; ix++)
        {
        pBuf = (char *) NET_BUF_ALLOC();
        if (pBuf == NULL)
            {
            END_LOG_MSG (END_DEBUG_LOAD, "%s%d - netClusterGet failed\n",
            		DRV_NAME, pDrvCtrl->unit, 0, 0, 0, 0);
            return (ERROR);
            }
	
	pBuf = (char *) pBuf + pDrvCtrl->offset;
        pDrvCtrl->rxBdBase[ix].dataPointer = pBuf;
        }

    END_LOG_MSG (END_DEBUG_LOAD, "Memory setup complete\n", 0, 0, 0, 0, 0, 0);

    return (OK);
    }

/*******************************************************************************
*
* iOlicomStart - start the device
*
* This function initializes the device and calls BSP functions to connect
* interrupts and start the device running in interrupt mode.
*
* The complement of this routine is iOlicomStop.  Once a unit is reset by
* iOlicomStop, it may be re-initialized to a running state by this routine.
*
* RETURNS: OK if successful, otherwise ERROR
*/

LOCAL STATUS iOlicomStart
    (
    END_DEVICE *	pDrvCtrl	/* pointer to END_DEVICE structure */
    )
    {
    STATUS	result;
    int		ix;
    ULONG	addrC;

    END_LOG_MSG (END_DEBUG_ALL_FUNCS, "iOlicomStart()\n", 0, 0, 0, 0, 0, 0);

    /* Select bank 0 of the Olicom register set */

    SELECTBANK(pDrvCtrl->pcmcia.oliAddr, 0);

    /* reset the device */

    iOlicomReset (pDrvCtrl);

    /* initialize the device */

    iOlicomInit (pDrvCtrl);

    /* initialize flags */

    DRV_FLAGS_CLR(OLI_POLLING | OLI_TX_CLEANING | OLI_TX_STOP);

    /* initialize receive buffer descriptors */

    for (ix = 0; ix < pDrvCtrl->rxBdNum; ix++)
        pDrvCtrl->rxBdBase[ix].statusMode = RX_BD_EMPTY;

    pDrvCtrl->rxBdNext = 0;

    /* initialize transmit buffer descriptors */

    for (ix = 0; ix < pDrvCtrl->txBdNum; ix++)
        {
	pDrvCtrl->txBdBase[ix].dataPointer =
		(char *)(addrC = (RAM_TX_BASE + (ix * OLI_MAX_XMT)));

        PCMCIA_IO_WRITE(pDrvCtrl->pcmcia.oliAddr + I595_R12, addrC);
        PCMCIA_IO_WRITE(pDrvCtrl->pcmcia.oliAddr + I595_R13, (addrC >> 8));
	WRITE_R14_R15 (TX_BD_READY);
        }

    pDrvCtrl->txBdNext = 0;
    pDrvCtrl->txBdIndexC = 0;

    /* configure promiscuous mode and multicast addresses list */

    iOlicomConfig(pDrvCtrl);

    /* connect interrupt */

    SYS_INT_CONNECT (pDrvCtrl, iOlicomInt, (int)pDrvCtrl, &result);
    if (result == ERROR)
	return ERROR;

    END_LOG_MSG (END_DEBUG_LOAD, "Interrupt connected.\n", 0, 0, 0, 0, 0, 0);


    /* set running & up flags */

    END_FLAGS_SET (&pDrvCtrl->endObj, (IFF_UP | IFF_RUNNING));


    /* enable interrupt */

    SYS_INT_ENABLE (pDrvCtrl);

    END_LOG_MSG (END_DEBUG_LOAD, "Interrupt enabled.\n", 0, 0, 0, 0, 0, 0);


    return (OK);
    }

/*******************************************************************************
*
* iOlicomIntHandle - interrupt service for card interrupts
*
* This routine is called when an interrupt has been detected from the Olicom
* card.
*
* RETURNS: N/A.
*/

void iOlicomIntHandle
    (
    END_DEVICE *	pDrvCtrl	/* pointer to END_DEVICE structure */
    )
    {
    int		cardStat;
    int		intStat;

    /*
     * Read the card configuration state to verify this is a card I/O
     * interrupt.
     */

    cardStat = PCMCIA_ATTR_READ(pDrvCtrl->pcmcia.oliAttribMem + CARDCONFREG1);

    END_LOG_MSG (END_DEBUG_INT, "cardStat (config Reg 1) = 0x%X\n",
	    cardStat, 0, 0, 0, 0, 0);

    /* I/O Interrupt, examine card to find out what */

    if (cardStat & CREG1_INT_IO)
	{
	END_LOG_MSG (END_DEBUG_INT, "I/O interrupt\n",
		     0, 0, 0, 0, 0, 0);

	/*
	 * Read the chip's interrupt status register to find out why
	 * an IRQ was raised.
	 */

	intStat = PCMCIA_IO_READ(pDrvCtrl->pcmcia.oliAddr + I595_R1);

	END_LOG_MSG (END_DEBUG_INT, "intStat (595 Reg 1) = 0x%X\n",
		intStat, 0, 0, 0, 0, 0);

	/*
	 * Decode the interrupt down and take action accordingly. Note
	 * that there may be multiple IRQ sources pending.
	 */

	/* handle receive events */

	if (intStat & BNK0_RX_IT)
	    {
	    /* netTask handles any input packets */

	    if (! DRV_FLAGS_ISSET(OLI_RCV_HANDLING))
		{
		DRV_FLAGS_SET(OLI_RCV_HANDLING);
		(void)netJobAdd ((FUNCPTR)iOlicomRxIntHandle,
			    (int)pDrvCtrl, 0,0,0,0);
		}

	    /* Acknowledge receive interrupt */

	    PCMCIA_IO_WRITE(pDrvCtrl->pcmcia.oliAddr + I595_R1, BNK0_RX_IT);
	    }

	/* handle transmitter events */

	if (intStat & BNK0_TX_IT)
	    {

	    /*
	     * clean the transmit buffer descriptor queue if we have
	     * received a transmit interrupt and if we are not already
	     * cleaning this transmit queue.
	     */

	    if (! DRV_FLAGS_ISSET(OLI_TX_CLEANING))
		{
		DRV_FLAGS_SET(OLI_TX_CLEANING);
		(void)netJobAdd ((FUNCPTR)iOlicomTxBdQueueClean,
			    (int)pDrvCtrl, 0,0,0,0);
		}

	    /* Acknowledge transmit interrupts */

	    PCMCIA_IO_WRITE(pDrvCtrl->pcmcia.oliAddr + I595_R1, BNK0_TX_IT);
	    }

	/* Acknowledge all other interrupts - ignore events */

	PCMCIA_IO_WRITE(pDrvCtrl->pcmcia.oliAddr + I595_R1,
			     ~(BNK0_TX_IT | BNK0_RX_IT));

	if (DRV_FLAGS_ISSET(OLI_TX_STOP))	    /* cause a restart */
	    {
	    DRV_FLAGS_CLR(OLI_TX_STOP);
	    netJobAdd ((FUNCPTR)muxTxRestart, (int)&pDrvCtrl->endObj,
			    0, 0, 0, 0);
	    }
	}

    return;
    }

/*******************************************************************************
*
* iOlicomRxIntHandle - task level interrupt service for input packets
*
* This routine is called at task level indirectly by the interrupt
* service routine to do any message received processing.
*
* RETURNS: N/A.
*/

LOCAL void iOlicomRxIntHandle
    (
    END_DEVICE *	pDrvCtrl	/* pointer to END_DEVICE structure */
    )
    {
    RX_BD *	pRxBd = iOlicomPacketGet (pDrvCtrl);

    END_LOG_MSG (END_DEBUG_RX, ("iOlicomRxIntHandle\n"), 0, 0, 0, 0, 0, 0);

    do
        {
        DRV_FLAGS_SET (OLI_RCV_HANDLING);

	while (pRxBd != NULL)
	    {
	    iOlicomRecv (pDrvCtrl, pRxBd);
	    pRxBd = iOlicomPacketGet (pDrvCtrl);
	    }

        DRV_FLAGS_CLR (OLI_RCV_HANDLING);


	/* check once more after resetting flags */

	pRxBd = iOlicomPacketGet (pDrvCtrl);
        }
    while (pRxBd != NULL);
    }

/*******************************************************************************
*
* iOlicomPacketGet - get next received message
*
* Get next received message.  Returns NULL if none are ready.
*
* RETURNS: ptr to next packet, or NULL if none ready.
*/

LOCAL RX_BD * iOlicomPacketGet
    (
    END_DEVICE *	pDrvCtrl	/* pointer to END_DEVICE structure */
    )
    {
    int			start;
    int			event;
    int			status;
    int			length;
    int			s;
    int			nextStop;
    UINT8 *		pData;
    UINT8 *		pDataEnd;
    RX_BD *		pRxBd = &pDrvCtrl->rxBdBase[pDrvCtrl->rxBdNext];
    volatile UINT8 *	pR14;

    END_LOG_MSG (END_DEBUG_RX, "iOlicomPacketGet oliAddr=0x%X\n",
    		pDrvCtrl->pcmcia.oliAddr, 0, 0, 0, 0, 0);
	
    /*
     * The RCV STOP register points to the last pair of bytes
     * BEFORE the start of the next packet so we must add two
     * bytes to reach the correct address.
     */

    s = intLock();

    start = PCMCIA_IO_READ(pDrvCtrl->pcmcia.oliAddr + I595_R6);
    start |= (PCMCIA_IO_READ(pDrvCtrl->pcmcia.oliAddr + I595_R7) << 8);
    start += 2;

    /* Handle the roll over case */

    if (start > RAM_RX_LIMIT)
	start -= RAM_RX_BASE;

    END_LOG_MSG (END_DEBUG_RX, "start= 0x%X\n", start, 0, 0, 0, 0, 0);

    /* Set up address from where we wish to start reading data */

    PCMCIA_IO_WRITE(pDrvCtrl->pcmcia.oliAddr + I595_R12, start);
    PCMCIA_IO_WRITE(pDrvCtrl->pcmcia.oliAddr + I595_R13, (start >> 8));

    /* The first word describes the state of reception. */

    READ_R14_R15 (event);

    /* The following bit will be set once the packet is complete in memory */

    if (!(event & RCV_EOF))
	{
	intUnlock (s);

	return ((RX_BD *) NULL);
	}

    /* Collect the status of the packet */

    READ_R14_R15 (status);

    /* get next packet pointer */

    READ_R14_R15 (nextStop);

    /* The next stop value is 2 bytes back in the circular buffer */

    nextStop -= 2;

    /* Handle the roll over case */

    if (nextStop < RAM_RX_BASE)
	nextStop += RAM_RX_BASE;

    /* get packet length */

    READ_R14_R15 (length);

    END_LOG_MSG (END_DEBUG_RX, "length= 0x%X\n", length, 0, 0, 0, 0, 0);

    /* Check for errors */

    if (!(status & RCV_OK))
        {
        /* Bump input error packet counter */

        END_ERR_ADD (&pDrvCtrl->endObj, MIB2_IN_ERRS, +1);

	/* Update the STOP register from the next packet pointer */

	PCMCIA_IO_WRITE(pDrvCtrl->pcmcia.oliAddr + I595_R6, nextStop);
	PCMCIA_IO_WRITE(pDrvCtrl->pcmcia.oliAddr + I595_R7, (nextStop >> 8));

	intUnlock (s);

        return ((RX_BD *) NULL);
        }

    /* check if a receive buffer descriptor is available */

    if (!(pRxBd->statusMode & RX_BD_EMPTY))
        {
        /* Bump input error packet counter */

        END_ERR_ADD (&pDrvCtrl->endObj, MIB2_IN_ERRS, +1);

	/* Update the STOP register from the next packet pointer */

	PCMCIA_IO_WRITE(pDrvCtrl->pcmcia.oliAddr + I595_R6, nextStop);
	PCMCIA_IO_WRITE(pDrvCtrl->pcmcia.oliAddr + I595_R7, (nextStop >> 8));
	intUnlock (s);

        return ((RX_BD *) NULL);
        }


    /*
     * We always read an even number of bytes from the controller,
     * so adjust the length if necessary.
     */

    if ((length & ODD_MSK) != 0)
        ++length;

    pRxBd->dataLength = length;

    /*
     * Read the data from the Rx ring buffer.
     *
     * Set up the pointer just before the loop: provokes code generator
     * into producing much better code.
     */

    pR14 = (volatile UINT8 *) (pDrvCtrl->pcmcia.oliAddr + I595_R14);

    for (pData = (UINT8 *)pRxBd->dataPointer, pDataEnd = pData + length;
	 pData != pDataEnd; )
	READ_PTR_R14_R15 (pData, pR14);

    END_LOG_MSG (END_DEBUG_RX, "data = 0x%X 0x%X\n",
		 (pRxBd->dataPointer)[0], (pRxBd->dataPointer)[1], 0, 0, 0, 0);

    /* Update the STOP register from the next packet pointer */

    PCMCIA_IO_WRITE(pDrvCtrl->pcmcia.oliAddr + I595_R6, nextStop);
    PCMCIA_IO_WRITE(pDrvCtrl->pcmcia.oliAddr + I595_R7, (nextStop >> 8));

    /* Update status buffer descriptor */

    pRxBd->statusMode = 0;

    /* incr buffer descriptor count */

    pDrvCtrl->rxBdNext = (pDrvCtrl->rxBdNext + 1) % pDrvCtrl->rxBdNum;

    intUnlock (s);

    return pRxBd;
    }


/*******************************************************************************
*
* iOlicomRecv - process the next incoming packet
*
* This routine processes an input frame, then passes it up to the higher
* level in a form it expects.
*
* RETURNS: OK, always.
*/

LOCAL STATUS iOlicomRecv
    (
    END_DEVICE *	pDrvCtrl,	/* pointer to END_DEVICE structure */
    RX_BD *		pRxBd
    )
    {
    END_OBJ *   pOliObj = &pDrvCtrl->endObj;
    M_BLK_ID    pMblk;      /* MBLK to send upstream */
    CL_BLK_ID   pClBlk;     /* pointer to clBlk */
    char *      pBuf;       /* A replacement buffer for the current RxD */
    char *      pData;      /* Data pointer for the current RxD */
    int         len;        /* Len of the current data */

    END_LOG_MSG (END_DEBUG_RX, "iOlicomRecv\n", 0, 0, 0, 0, 0, 0);

    /* Allocate an MBLK, and a replacement buffer */

    pMblk = NET_MBLK_ALLOC();
    pBuf  = NET_BUF_ALLOC();
    pClBlk = NET_CL_BLK_ALLOC();

    if ((pMblk == NULL) || (pBuf == NULL) || (pClBlk == NULL))
        {
        END_ERR_ADD (&pDrvCtrl->endObj, MIB2_IN_ERRS, +1);
        if (pMblk)
            NET_MBLK_FREE (pMblk);
        if (pBuf)
            NET_BUF_FREE (pBuf);
        if (pClBlk)
            NET_CL_BLK_FREE (pClBlk);
        goto cleanRxBd;
        }

    /* Get the data pointer and len from the current RX_BD */

    len   = pRxBd->dataLength;
    pData = pRxBd->dataPointer;

    pData -= pDrvCtrl->offset;

    /* Associate the data pointer with the CL_BLK */

    NET_CL_BLK_JOIN (pClBlk, pData, OLI_BUFSIZ);

    /* Associate the CL_BLK with the MBLK */

    NET_MBLK_CL_JOIN (pMblk, pClBlk);

    pMblk->mBlkHdr.mData  += pDrvCtrl->offset;
    pMblk->mBlkHdr.mFlags |= M_PKTHDR;	/* set the packet header */
    pMblk->mBlkHdr.mLen    = len;	/* set the data len */
    pMblk->mBlkPktHdr.len  = len;	/* set the total len */

    /* Deal with memory alignment. */

    pBuf += pDrvCtrl->offset;

    /* Install the new data buffer */

    pRxBd->dataPointer = pBuf;

    /* mark the descriptor ready to receive */

    pRxBd->statusMode |= RX_BD_EMPTY;

    /* Bump the statistic counter. */

    END_ERR_ADD (&pDrvCtrl->endObj, MIB2_IN_UCAST, +1);

    /* send up to protocol */

    END_LOG_MSG (END_DEBUG_RX, ("Calling upper layer! \n"), 0, 0, 0, 0, 0, 0);

    END_RCV_RTN_CALL (pOliObj, pMblk);

    return OK;

cleanRxBd:

    /* mark the descriptor ready to receive */

    pRxBd->statusMode |= RX_BD_EMPTY;

    return OK;
    }

/*******************************************************************************
*
* iOlicomSend - the driver send routine
*
* This routine takes a M_BLK_ID sends off the data in the M_BLK_ID.
* The buffer must already have the addressing information properly installed
* in it.  This is done by a higher layer.
*
* RETURNS: OK or ERROR.
*/

LOCAL STATUS iOlicomSend
    (
    END_DEVICE *	pDrvCtrl,	/* pointer to END_DEVICE structure */
    M_BLK *		pMblk
    )
    {
    TX_BD *		pTxBd;
    char *		pBuf;
    UINT8 *		pData;
    UINT8 *		pDataEnd;
    ULONG		addrC;
    int			len;
    int			s;
    volatile UINT8 *	pR14;

    END_LOG_MSG (END_DEBUG_ALL_FUNCS, "iOlicomSend()\n", 0, 0, 0, 0, 0, 0);
    END_LOG_MSG (END_DEBUG_TX, "iOlicomSend()\n", 0, 0, 0, 0, 0, 0);

    if (DRV_FLAGS_ISSET (OLI_POLLING))
        { 
        NET_MBLK_CHAIN_FREE (pMblk); /* free the given mBlk chain */
        errno = EINVAL;
        return (ERROR);
        }

    /*
     * Obtain exclusive access to transmitter.  This is necessary because
     * we might have more than one stack transmitting at once.
     */

    END_TX_SEM_TAKE (&pDrvCtrl->endObj, WAIT_FOREVER);
    END_LOG_MSG (END_DEBUG_TX, "iOlicomSend() got semaphore\n",
		 0, 0, 0, 0, 0, 0);

    /* get a free transmit frame descriptor */

    pTxBd = iOlicomTxBdGet (pDrvCtrl);
    addrC = (ULONG) pTxBd->dataPointer;

    /* get a free buufer */

    pBuf = NET_BUF_ALLOC();

    if ((pTxBd == NULL) || (pBuf == NULL))
        {
        END_ERR_ADD (&pDrvCtrl->endObj, MIB2_OUT_ERRS, +1);
        END_TX_SEM_GIVE (&pDrvCtrl->endObj);

        if (pBuf)
            NET_BUF_FREE (pBuf);

        if (! DRV_FLAGS_ISSET(OLI_TX_CLEANING | OLI_TX_STOP))
	    iOlicomTxBdQueueClean (pDrvCtrl);

	s = intLock();
        DRV_FLAGS_SET(OLI_TX_STOP);        /* transmitter not ready */
	intUnlock (s);

	/* just return without freeing mBlk chain */

        return (END_ERR_BLOCK);
        }

    /* Copy and free the MBLK */

    len = netMblkToBufCopy (pMblk, pBuf, NULL);

    NET_MBLK_CHAIN_FREE (pMblk);

    s = intLock();

    /* Write the transmit buffer base address. */

    PCMCIA_IO_WRITE(pDrvCtrl->pcmcia.oliAddr + I595_R12, addrC);
    PCMCIA_IO_WRITE(pDrvCtrl->pcmcia.oliAddr + I595_R13, (addrC >> 8));


    /* Write the command opcode, and reserved field for the packet */

    WRITE_R14_R15 (CMD_TRANSMIT);


    /* Write 16 bit status field */

    WRITE_R14_R15 (0x00);


    /* Write the chain pointer */

    WRITE_R14_R15 (0x00);


    /* Write the byte count low and high bytes */

    WRITE_R14_R15 (len & ~XMT_CHAIN);


    /*
     * Copy data. Initialise pointer here to provoke the compiler into
     * generating better code
     */

    pR14 = (volatile UINT8 *) (pDrvCtrl->pcmcia.oliAddr + I595_R14);

    for (pData = (UINT8 *)pBuf, pDataEnd = pData + len; pData < pDataEnd ; )
	WRITE_PTR_R14_R15 (pData, pR14);

    /*
     * The Ethernet chip can't receive a transmit command while a
     * TRANSMIT is already in progress. The following test checks if the
     * 82595TX chip is transmitting a packet before issuing another transmit
     * command.
     */

    if (pDrvCtrl->txBdIndexC == pDrvCtrl->txBdNext)
	{
	/* Write the XMT base address register */

	PCMCIA_IO_WRITE(pDrvCtrl->pcmcia.oliAddr + I595_R10, addrC);
	PCMCIA_IO_WRITE(pDrvCtrl->pcmcia.oliAddr + I595_R11, (addrC >> 8));

	/* Issue a XMT command to the 82595TX */

	RUNCMD((UINT)(pDrvCtrl->pcmcia.oliAddr), CMD_TRANSMIT);

	}

    /* Save the buf info */

    pDrvCtrl->freeBuf[pDrvCtrl->txBdNext].pClBuf  = pBuf;

    /* incr BD count */

    pDrvCtrl->txBdNext = (pDrvCtrl->txBdNext + 1) % pDrvCtrl->txBdNum;

    intUnlock(s);

    /* release exclusive access */

    END_TX_SEM_GIVE (&pDrvCtrl->endObj);

    /* Bump the statistic counter. */

    END_ERR_ADD (&pDrvCtrl->endObj, MIB2_OUT_UCAST, +1);

    END_LOG_MSG (END_DEBUG_ALL_FUNCS, "iOlicomSend() exit\n", 0, 0, 0, 0, 0, 0);

    return (OK);

    }

/*******************************************************************************
*
* iOlicomTxBdGet - get an available transmit descriptor
*
* Get next transmited message descriptor.  Returns NULL if none are
* ready.
*
* RETURNS: an available transmit descriptor, otherwise NULL.
*/

LOCAL TX_BD * iOlicomTxBdGet
    (
    END_DEVICE *	pDrvCtrl
    )
    {
    ULONG       addrC;
    int         status;
    TX_BD *	pTxBd = &pDrvCtrl->txBdBase[pDrvCtrl->txBdNext];

    addrC = (ULONG) pTxBd->dataPointer;

    PCMCIA_IO_WRITE(pDrvCtrl->pcmcia.oliAddr + I595_R12, addrC);
    PCMCIA_IO_WRITE(pDrvCtrl->pcmcia.oliAddr + I595_R13, (addrC >> 8));

    READ_R14_R15 (status);

    /* check if a transmit buffer descriptor is available */

    if (!(status & TX_BD_READY) ||
	 ((pDrvCtrl->txBdNext + 1) % pDrvCtrl->txBdNum) == pDrvCtrl->txBdIndexC)
        return (NULL);

    return (pTxBd);
    }

/*******************************************************************************
*
* iOlicomIoctl - the driver I/O control routine
*
* Process an ioctl request.
*
* This routine implements the network interface control functions.
* It handles EIOCSADDR, EIOCGADDR, EIOCSFLAGS, EIOCGFLAGS,
* EIOCPOLLSTART, EIOCPOLLSTOP, EIOCGMIB2, EIOCGFBUF commands.
*
* RETURNS: OK if successful, otherwise EINVAL.
*/

LOCAL int iOlicomIoctl
    (
    END_DEVICE *	pDrvCtrl,	/* pointer to END_DEVICE structure */
    int			cmd,		/* command to process */
    char *		data		/* pointer to data */
    )
    {
    int		error = 0;
    long	value;

    END_LOG_MSG (END_DEBUG_ALL_FUNCS, "iOlicomIoctl()\n", 0, 0, 0, 0, 0, 0);

    switch ((UINT)cmd)
        {
        case EIOCSADDR:
	    if (data == NULL)
		return (EINVAL);
            memcpy ((char *)END_HADDR(&pDrvCtrl->endObj), (char *)data,
		   END_HADDR_LEN(&pDrvCtrl->endObj));
            break;

        case EIOCGADDR:
	    if (data == NULL)
		return (EINVAL);
            memcpy ((char *)data, (char *)END_HADDR(&pDrvCtrl->endObj),
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
	    iOlicomConfig (pDrvCtrl);
	    END_LOG_MSG (END_DEBUG_IOCTL, "endFlags %#x\n",
			END_FLAGS_GET(&pDrvCtrl->endObj), 0, 0, 0, 0 ,0);
            break;

        case EIOCGFLAGS:
	    *(int *)data = END_FLAGS_GET(&pDrvCtrl->endObj);
            break;

	case EIOCPOLLSTART:
	    error = iOlicomPollStart (pDrvCtrl);
	    break;

	case EIOCPOLLSTOP:
	    error = iOlicomPollStop (pDrvCtrl);
	    break;

        case EIOCGMIB2:
            if (data == NULL)
                return (EINVAL);
            memcpy((char *)data, (char *)&pDrvCtrl->endObj.mib2Tbl,
                  sizeof(pDrvCtrl->endObj.mib2Tbl));
            break;
        case EIOCGFBUF:
            if (data == NULL)
                return (EINVAL);
            *(int *)data = OLI_MIN_FBUF;
            break;
        default:
            error = EINVAL;
        }

    return (error);
    }

/******************************************************************************
*
* iOlicomConfig - reconfigure the interface under us.
*
* Reconfigure the interface setting promiscuous mode, and changing the
* multicast interface list.
*
* RETURNS: N/A
*/

LOCAL void iOlicomConfig
    (
    END_DEVICE *	pDrvCtrl	/* pointer to END_DEVICE structure */
    )
    {

    /* Set promiscuous mode if it's asked for. */

    if (END_FLAGS_GET (&pDrvCtrl->endObj) & IFF_PROMISC)
	{
	END_LOG_MSG (END_DEBUG_IOCTL, "Setting promiscuous mode on!\n",
			 0, 0, 0, 0, 0, 0);

        SELECTBANK (pDrvCtrl->pcmcia.oliAddr, 2);
        PCMCIA_IO_WRITE (pDrvCtrl->pcmcia.oliAddr + I595_R2,
		(PCMCIA_IO_READ (pDrvCtrl->pcmcia.oliAddr + I595_R2) |
			BNK2_PCS_EN));
	}
    else
	{
	END_LOG_MSG (END_DEBUG_IOCTL, "Setting promiscuous mode off!\n",
			 0, 0, 0, 0, 0, 0);
	}

    /* setup ethernet address and filtering mode */
	
    iOlicomAddrFilterSet (pDrvCtrl);

    return;
    }

/******************************************************************************
*
* iOlicomAddrFilterSet - set the address filter for multicast addresses
*
* This routine goes through all of the multicast addresses on the list
* of addresses (added with the iOlicomAddrAdd() routine) and sets the
* device's filter correctly.
*
* RETURNS: OK on success, ERROR otherwise.
*/

LOCAL STATUS iOlicomAddrFilterSet
    (
    END_DEVICE *	pDrvCtrl	/* pointer to END_DEVICE structure */
    )
    {
    UINT8 *     pFltrFrm;
    UINT8 *     pData;
    ETHER_MULTI * pMCastNode;
    TX_BD *	pTxBd;
    int         length;
    int         ix;
    int         s;
    int         index = 0;
    ULONG       addrC;

    END_LOG_MSG (END_DEBUG_ADDR, "iOlicomAddrFilterSet \n", 0, 0, 0, 0, 0, 0);

    /* gain exclusive access to transmitter */

    END_TX_SEM_TAKE (&pDrvCtrl->endObj, WAIT_FOREVER);

    /* Select bank 0 of the Olicom register set */

    SELECTBANK(pDrvCtrl->pcmcia.oliAddr, 0);

    /* get a free transmit frame descriptor */

    pTxBd = iOlicomTxBdGet (pDrvCtrl);

    if (pTxBd == NULL)
        {
	END_TX_SEM_GIVE (&pDrvCtrl->endObj);
        return (ERROR);
        }

    addrC = (ULONG) pTxBd->dataPointer;

    /* get a buffer */

     pFltrFrm = (UINT8 *)NET_BUF_ALLOC();
     if (pFltrFrm == NULL)
         {
         END_LOG_MSG (END_DEBUG_LOAD, "netClusterGet failed\n",
					0, 0, 0, 0, 0, 0);
         END_TX_SEM_GIVE (&pDrvCtrl->endObj);
         return (ERROR);
         }

    /* clear all entries */

    memset (pFltrFrm, 0, FLTR_FRM_SIZE);

    /* install multicast addresses */

    pData = pFltrFrm;

    for (pMCastNode = END_MULTI_LST_FIRST (&pDrvCtrl->endObj);
        pMCastNode != NULL;
        pMCastNode = END_MULTI_LST_NEXT (pMCastNode))
        {
        for (ix = 0; ix < EADDR_LEN; ix++)
            *(pData++) = pMCastNode->addr[ix];	
        index++;
        }

    /* Set variables to copy the buffer in */

    length = index * EADDR_LEN;
    pData--;

    s = intLock ();    /* disable ints during update */

    /* Write the transmit buffer base address. */

    PCMCIA_IO_WRITE(pDrvCtrl->pcmcia.oliAddr + I595_R12, addrC);
    PCMCIA_IO_WRITE(pDrvCtrl->pcmcia.oliAddr + I595_R13, (addrC >> 8));

    /* Write the command opcode, and reserved field for the packet */

    WRITE_R14_R15 (CMD_MC_SETUP);


    /* Write 16 bit status field */

    WRITE_R14_R15 (0x00);


    /* Write the chain pointer */

    WRITE_R14_R15 (0x00);

    /* Write the byte count low and high bytes */

    WRITE_R14_R15 (length & ~XMT_CHAIN);


    /* copy data */

    for (; length > 0 ; length -= 2)
        {
        PCMCIA_IO_WRITE((UINT)(pDrvCtrl->pcmcia.oliAddr + I595_R14),
					*pData--);
        PCMCIA_IO_WRITE((UINT)(pDrvCtrl->pcmcia.oliAddr + I595_R15),
					*pData--);
        }

    /* Write the XMT base address register */

    PCMCIA_IO_WRITE(pDrvCtrl->pcmcia.oliAddr + I595_R10, addrC);
    PCMCIA_IO_WRITE(pDrvCtrl->pcmcia.oliAddr + I595_R11, (addrC >> 8));

    /* Issue a MC_SETUP command to the 82595TX */

    RUNCMD((UINT)(pDrvCtrl->pcmcia.oliAddr), CMD_MC_SETUP);

    /* incr BD count */

    pDrvCtrl->txBdNext = (pDrvCtrl->txBdNext + 1) % pDrvCtrl->txBdNum;

    /* Spin until we've sent it. */

    while (!(PCMCIA_IO_READ(pDrvCtrl->pcmcia.oliAddr + I595_R1) & BNK0_EXE_IT));

    /* clear execution interrupt */

    PCMCIA_IO_WRITE(pDrvCtrl->pcmcia.oliAddr + I595_R1, BNK0_EXE_IT);

    /* clean the transmit queue */

    iOlicomTxBdQueueClean(pDrvCtrl);

    intUnlock (s);   /* now iOlicomInt won't get confused */

    /* free the buffer */

    NET_BUF_FREE (pFltrFrm);

    /* release exclusive access */

    END_TX_SEM_GIVE (&pDrvCtrl->endObj);

    END_LOG_MSG (END_DEBUG_ADDR, "iOlicomAddrFilterSet() exit\n",
		 0, 0, 0, 0, 0, 0);

    return (OK);
    }

/*******************************************************************************
*
* iOlicomPollReceive - routine to receive a packet in polled mode.
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

LOCAL STATUS iOlicomPollReceive
    (
    END_DEVICE *	pDrvCtrl,	/* pointer to END_DEVICE structure */
    M_BLK  *		pMblk
    )
    {
    char *		pPacket;
    int			status;
    int			length;
    int			event;
    int			start;
    int			nextStop;
    BOOL		gotOne=FALSE;
    volatile UINT8 *	pR14;

    END_LOG_MSG (END_DEBUG_ALL_FUNCS, "iOlicomPollReceive()\n",
		 0, 0, 0, 0, 0, 0);

    if ((pMblk->mBlkHdr.mFlags & M_EXT) != M_EXT)
        return (EAGAIN);

    /* Read the device status register */

    status = PCMCIA_IO_READ(pDrvCtrl->pcmcia.oliAddr + I595_R1);

    /* If no interrupt then return. */

    if (!(status & BNK0_RX_IT))
	{
	END_LOG_MSG (END_DEBUG_POLL_RX, "iOlicomPollReceive no rint\n",
			 0, 0, 0, 0, 0, 0);
        return (EAGAIN);
	}

    /*
     * The RCV STOP register points to the last pair of bytes
     * BEFORE the start of the next packet so we must add two
     * bytes to reach the correct address.
     */

    start = PCMCIA_IO_READ(pDrvCtrl->pcmcia.oliAddr + I595_R6);
    start |= (PCMCIA_IO_READ(pDrvCtrl->pcmcia.oliAddr + I595_R7) << 8);
    start += 2;

    /* Handle the roll over case */

    if (start > RAM_RX_LIMIT)
	start -= RAM_RX_BASE;

    /* Set up address from where we wish to start reading data */

    PCMCIA_IO_WRITE(pDrvCtrl->pcmcia.oliAddr + I595_R12, start);
    PCMCIA_IO_WRITE(pDrvCtrl->pcmcia.oliAddr + I595_R13, (start >> 8));

    /* The first word describes the state of reception.  */

    READ_R14_R15 (event);

    /* The following bit will be set once the packet is complete in memory. */

    if (event & RCV_EOF)
        {

        /* Collect the status of the packet */

	READ_R14_R15 (status);

	/* get next packet pointer */

	READ_R14_R15 (nextStop);

	/* The next stop value is 2 bytes back in the circular buffer */

	nextStop -= 2;

	/* Handle the roll over case */

	if (nextStop < RAM_RX_BASE)
	    nextStop += RAM_RX_BASE;

        /* get packet length */

	READ_R14_R15 (length);


        /* Check for errors */

        if (!(status & RCV_OK) || ((length - SIZEOF_ETHERHEADER) <= 0))
            {
            /* bump input error packet counter */

            END_ERR_ADD (&pDrvCtrl->endObj, MIB2_IN_ERRS, +1);
            }
        else
            {

            /*
             * We always read an even number of bytes from the controller,
             * so adjust the length if necessary.
             */

            if ((length & ODD_MSK) != 0)
                ++length;

            /* Upper layer provides the buffer. */

            if (pMblk->mBlkHdr.mLen < length)
                goto pollRecvExit;

	    /* Deal with memory aligment */

	    if (((int) pMblk->mBlkHdr.mData & 0x3) == 0)
		pMblk->mBlkHdr.mData += pDrvCtrl->offset;

            pMblk->mBlkHdr.mFlags |= M_PKTHDR;	/* set the packet header */
	    pMblk->mBlkPktHdr.len  = length;	/* set the total len */
	    pMblk->mBlkHdr.mLen    = length;	/* set the data len */


            /* Copy data.
	     *
	     * Set up the pointer just before the loop: provokes code
	     * generator into producing much better code.
	     */

	    pR14 = (volatile UINT8 *) (pDrvCtrl->pcmcia.oliAddr + I595_R14);

            for (pPacket = pMblk->mBlkHdr.mData; length != 0; length -=2)
		READ_PTR_R14_R15 (pPacket, pR14);

	    gotOne = TRUE;

            /* bump input packet counter */

            END_ERR_ADD (&pDrvCtrl->endObj, MIB2_IN_UCAST, +1);
            }

        /* Update the STOP register from the next packet pointer */

	PCMCIA_IO_WRITE(pDrvCtrl->pcmcia.oliAddr + I595_R6, nextStop);
	PCMCIA_IO_WRITE(pDrvCtrl->pcmcia.oliAddr + I595_R7, (nextStop >> 8));
	}

pollRecvExit:

    /* clear receive interrupts. */

    PCMCIA_IO_WRITE(pDrvCtrl->pcmcia.oliAddr + I595_R1, BNK0_RX_IT);

    return (gotOne ? OK : EAGAIN);
    }

/*******************************************************************************
*
* iOlicomPollSend - routine to send a packet in polled mode.
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

LOCAL STATUS iOlicomPollSend
    (
    END_DEVICE *	pDrvCtrl,	/* pointer to END_DEVICE structure */
    M_BLK *		pMblk
    )
    {
    TX_BD *		pTxBd;
    char *		pBuf;
    char *		pData;
    ULONG		addrC;
    int			len;
    int			status;
    volatile UINT8 *	pR14;

    END_LOG_MSG (END_DEBUG_ALL_FUNCS, "iOlicomPollSend()\n",
		 0, 0, 0, 0, 0, 0);

    if (DRV_FLAGS_ISSET(OLI_TX_STOP))
        return (EAGAIN);

    /* get a free transmit frame descriptor */

    pTxBd = iOlicomTxBdGet (pDrvCtrl);
    addrC = (ULONG) pTxBd->dataPointer;

    /* get a free buffer */

    pBuf = NET_BUF_ALLOC ();

    if ((pTxBd == NULL) || (pBuf == NULL))
        {
        iOlicomTxBdQueueClean (pDrvCtrl);

        if (pBuf)
            NET_BUF_FREE (pBuf);

        return (EAGAIN);
        }

   /* copy the MBLK */

    len = netMblkToBufCopy (pMblk, pBuf, NULL);

    /* Write the transmit buffer base address. */

    PCMCIA_IO_WRITE(pDrvCtrl->pcmcia.oliAddr + I595_R12, addrC);
    PCMCIA_IO_WRITE(pDrvCtrl->pcmcia.oliAddr + I595_R13, (addrC >> 8));

    /* Write the command opcode, and reserved field for the packet */

    WRITE_R14_R15 (CMD_TRANSMIT);


    /* Write 16 bit status field */

    WRITE_R14_R15 (0x00);


    /* Write the chain pointer */

    WRITE_R14_R15 (0x00);

    /* Write the byte count low and high bytes */

    WRITE_R14_R15 (len & ~XMT_CHAIN);


    /*
     * Copy data. Initialise pointer here to provoke the compiler into
     * generating better code
     */

    pR14 = (volatile UINT8 *) (pDrvCtrl->pcmcia.oliAddr + I595_R14);

    for (pData = pBuf; len > 0; len -= 2)
        WRITE_PTR_R14_R15 (pData, pR14);

    /* Write the XMT base address register */

    PCMCIA_IO_WRITE(pDrvCtrl->pcmcia.oliAddr + I595_R10, addrC);
    PCMCIA_IO_WRITE(pDrvCtrl->pcmcia.oliAddr + I595_R11, (addrC >> 8));

    /* Issue a XMT command to the 82595TX */

    RUNCMD((UINT)(pDrvCtrl->pcmcia.oliAddr), CMD_TRANSMIT);

    /* incr BD count */

    pDrvCtrl->txBdNext = (pDrvCtrl->txBdNext + 1) % pDrvCtrl->txBdNum;

    /* Bump the statistic counter. */

    END_ERR_ADD (&pDrvCtrl->endObj, MIB2_OUT_UCAST, +1);

    /* Spin until we've sent it. */

    while (!(PCMCIA_IO_READ(pDrvCtrl->pcmcia.oliAddr + I595_R1) & BNK0_TX_IT));

    /* clear transmit interrupt */

    PCMCIA_IO_WRITE(pDrvCtrl->pcmcia.oliAddr + I595_R1, BNK0_TX_IT);

    /* Free the data immediately. */

    NET_BUF_FREE (pBuf);

    /* check for output errors */

    addrC = (ULONG) pTxBd->dataPointer + 2;

    PCMCIA_IO_WRITE(pDrvCtrl->pcmcia.oliAddr + I595_R12, addrC);
    PCMCIA_IO_WRITE(pDrvCtrl->pcmcia.oliAddr + I595_R13, (addrC >> 8));

    READ_R14_R15 (status);

    if (!(status & XMT_OK))
        return (EAGAIN);

    /*
     * we are allowed to do this because transmit queue is empty when we
     * start polling mode.
     */

    pDrvCtrl->txBdIndexC = pDrvCtrl->txBdNext;

    return (OK);
    }

/******************************************************************************
*
* iOlicomMCastAddrAdd - add a multicast address for the device
*
* This routine adds a multicast address to whatever the driver
* is already listening for.  It then resets the address filter.
*
* RETURNS: OK on success, ERROR otherwise.
*/

LOCAL STATUS iOlicomMCastAddrAdd
    (
    END_DEVICE *	pDrvCtrl,	/* pointer to END_DEVICE structure */
    char *		pAddress	/* address to add to the table */
    )
    {
    int	error;

    END_LOG_MSG (END_DEBUG_ALL_FUNCS, "iOlicomMCastAddrAdd\n",
   		 0, 0, 0, 0, 0, 0);

    if ((error = etherMultiAdd (&pDrvCtrl->endObj.multiList,
		pAddress)) == ENETRESET)
	    iOlicomConfig (pDrvCtrl);

    return (OK);
    }

/*****************************************************************************
*
* iOlicomMCastAddrDel - delete a multicast address for the device
*
* This routine removes a multicast address from whatever the driver
* is listening for.  It then resets the address filter.
*
* RETURNS: OK on success, ERROR otherwise.
*/

LOCAL STATUS iOlicomMCastAddrDel
    (
    END_DEVICE *	pDrvCtrl,	/* pointer to END_DEVICE structure */
    char *		pAddress	/* address to delete from the table */
    )
    {
    int	error;

    END_LOG_MSG (END_DEBUG_ALL_FUNCS, "iOlicomMCastAddrDel\n", 0, 0, 0, 0, 0,0);

    if ((error = etherMultiDel (&pDrvCtrl->endObj.multiList,
	     (char *)pAddress)) == ENETRESET)
	    iOlicomConfig (pDrvCtrl);

    return (OK);
    }

/*****************************************************************************
*
* iOlicomMCastAddrGet - get the multicast address list for the device
*
* This routine gets the multicast list of whatever the driver
* is already listening for.
*
* RETURNS: OK on success, ERROR otherwise.
*/

LOCAL STATUS iOlicomMCastAddrGet
    (
    END_DEVICE *	pDrvCtrl,	/* pointer to END_DEVICE structure */
    MULTI_TABLE *	pTable		/* table to fill in with addresses */
    )
    {
    int	error;

    END_LOG_MSG (END_DEBUG_ALL_FUNCS, "iOlicomMCastAddrGet\n", 0, 0, 0, 0, 0,0);

    error = etherMultiGet (&pDrvCtrl->endObj.multiList, pTable);

    return (error);
    }

/*******************************************************************************
*
* iOlicomStop - stop the device
*
* This routine marks the interface as down and resets the device.  This
* includes stopping the transmitter and receiver.
*
* The complement of this routine is iOlicomStart.  Once a unit is
* stopped in this routine, it may be re-initialized to a running state by
* iOlicomStart.
*
* RETURNS: OK or ERROR
*/

LOCAL STATUS iOlicomStop
    (
    END_DEVICE *	pDrvCtrl	/* pointer to END_DEVICE structure */
    )
    {
    STATUS	result = OK;

    END_LOG_MSG (END_DEBUG_ALL_FUNCS, "iOlicomStop()\n", 0, 0, 0, 0, 0, 0);

    /* reset the controller */

    iOlicomReset (pDrvCtrl);


    /* mark the interface -- down */

    END_FLAGS_CLR (&pDrvCtrl->endObj, IFF_UP | IFF_RUNNING);


    /* disconnect interrupt */

    SYS_INT_DISCONNECT (pDrvCtrl, iOlicomInt, (int)pDrvCtrl, &result);

    if (result == ERROR)
	END_LOG_MSG (END_DEBUG_IOCTL, "Could not disconnect interrupt!\n",
			 0, 0, 0, 0, 0, 0);


    /* Disable LAN interrupts */

    SYS_INT_DISABLE (pDrvCtrl);

    return (result);
    }

/******************************************************************************
*
* iOlicomUnload - unload a driver from the system
*
* This function first brings down the device, and then frees any
* stuff that was allocated by the driver in the load function.
*
* RETURNS: OK, always.
*/

LOCAL STATUS iOlicomUnload
    (
    END_DEVICE *	pDrvCtrl	/* pointer to END_DEVICE structure */
    )
    {
    END_LOG_MSG (END_DEBUG_ALL_FUNCS, "iOlicomUnload()\n", 0, 0, 0, 0, 0, 0);

    /* Bring down the device */

    pDrvCtrl->endObj.flags &= ~(IFF_UP);

    /* deallocate lists */

    END_OBJECT_UNLOAD (&pDrvCtrl->endObj);

    /* Free the memory. */

    if (DRV_FLAGS_ISSET (OLI_MEMOWN))
	free ((char *)pDrvCtrl->pShMem);

    return (OK);
    }

/*******************************************************************************
*
* iOlicomPollStart - start polled mode operations
*
* This routine starts polling mode by disabling ethernet interrupts and
* setting the polling flag in the END_DEVICE stucture.
*
* RETURNS: OK, always.
*/

LOCAL STATUS iOlicomPollStart
    (
    END_DEVICE *	pDrvCtrl	/* pointer to END_DEVICE structure */
    )
    {
    int			s;
    int			txBdIndex;
    TX_BD *		pTxBd;
    ULONG		addrC;
    int			status;

    if (pDrvCtrl->flags & OLI_POLLING)
	return (OK);

    s = intLock ();          /* disable ints during update */

    txBdIndex = pDrvCtrl->txBdIndexC;

    /* Select bank 0 of the Olicom register set */

    SELECTBANK(pDrvCtrl->pcmcia.oliAddr, 0);

    while (txBdIndex != pDrvCtrl->txBdNext)
        {
        pTxBd = & pDrvCtrl->txBdBase[txBdIndex];
	addrC = (ULONG) (pTxBd->dataPointer);

        /* Spin until frame buffer is sent */

	PCMCIA_IO_WRITE(pDrvCtrl->pcmcia.oliAddr + I595_R12, addrC);
	PCMCIA_IO_WRITE(pDrvCtrl->pcmcia.oliAddr + I595_R13, (addrC >> 8));
	READ_R14_R15 (status);

        while (!(status & TX_BD_READY))
	    {
	    PCMCIA_IO_WRITE(pDrvCtrl->pcmcia.oliAddr + I595_R12, addrC);
	    PCMCIA_IO_WRITE(pDrvCtrl->pcmcia.oliAddr + I595_R13, (addrC >> 8));
	    READ_R14_R15 (status);
	    }

        /* free the buffer */

        if (pDrvCtrl->freeBuf[txBdIndex].pClBuf != NULL)
            {
            NET_BUF_FREE(pDrvCtrl->freeBuf[txBdIndex].pClBuf);
            pDrvCtrl->freeBuf[txBdIndex].pClBuf = NULL;
            }

        /* increment txBdIndex */

        txBdIndex = (txBdIndex + 1) % pDrvCtrl->txBdNum;

	/* Send a Transmit command if the transmit queue is not empty */

	if (txBdIndex != pDrvCtrl->txBdNext)
	    iOlicomTxStart(pDrvCtrl);
        }

    /*
     * Program featues: not promiscuous, not receive broadcast,
     * do not save CRC, Length Enable is on, Source Address insertion is
     * on, only one individual address, loopback off.
     */

    SELECTBANK(pDrvCtrl->pcmcia.oliAddr, 2);
    PCMCIA_IO_WRITE(pDrvCtrl->pcmcia.oliAddr + I595_R2, (BNK2_SAI_EN |
                        BNK2_RX_CRC | BNK2_BRO_DIS));
    PCMCIA_IO_WRITE(pDrvCtrl->pcmcia.oliAddr + I595_R3,
        PCMCIA_IO_READ(pDrvCtrl->pcmcia.oliAddr + I595_R3) & ~BNK2_TST_EN);

    /* Now, transmit queue is empty. We can enter polling mode. */
    /* mask off the receive and transmit interrupts */

    SELECTBANK(pDrvCtrl->pcmcia.oliAddr, 0);
    PCMCIA_IO_WRITE(pDrvCtrl->pcmcia.oliAddr + I595_R3,
        (PCMCIA_IO_READ(pDrvCtrl->pcmcia.oliAddr + I595_R3) |
		BNK0_ITS_MSK));

    /* Set the polling flag */

    DRV_FLAGS_SET(OLI_POLLING);

    intUnlock (s);   /* now iOlicomInt won't get confused */

    return OK;
    }

/*******************************************************************************
*
* iOlicomPollStop - stop polled mode operations
*
* This routine stops polling mode by enabling ethernet interrupts and
* resetting the polling flag in the END_DEVICE structure.
*
* RETURNS: OK, always.
*/

LOCAL STATUS iOlicomPollStop
    (
    END_DEVICE *	pDrvCtrl	/* pointer to END_DEVICE structure */
    )
    {
    int	s;

    s = intLock ();          /* disable ints during update */

    /*
     * Program featues: not promiscuous, receive broadcast,
     * do not save CRC, Length Enable is on, Source Address insertion is
     * on, only one individual address, loopback off.
     */

    SELECTBANK(pDrvCtrl->pcmcia.oliAddr, 2);
    PCMCIA_IO_WRITE(pDrvCtrl->pcmcia.oliAddr + I595_R2, (BNK2_SAI_EN |
                                BNK2_RX_CRC));
    PCMCIA_IO_WRITE(pDrvCtrl->pcmcia.oliAddr + I595_R3,
        PCMCIA_IO_READ(pDrvCtrl->pcmcia.oliAddr + I595_R3) & ~BNK2_TST_EN);

    /* Rewrite the register to turn on interrupts */

    SELECTBANK(pDrvCtrl->pcmcia.oliAddr, 0);
    PCMCIA_IO_WRITE(pDrvCtrl->pcmcia.oliAddr + I595_R3,
        PCMCIA_IO_READ(pDrvCtrl->pcmcia.oliAddr + I595_R3) &
		~(BNK0_RX_MK | BNK0_TX_MK | BNK0_EXE_MK));

    /* Reset the polling flag */

    DRV_FLAGS_CLR(OLI_POLLING);

    intUnlock (s);   /* now iOlicomInt won't get confused */

    return (OK);
    }

/*******************************************************************************
*
* iOlicomReset - network interface reset routine
*
* This routine resets the 82595TX on the PCMCIA card.The PCMCIA setup
* must have been done already and an I/O window mapped into the target's
* address space.
*
* RETURNS: N/A
*/

LOCAL void iOlicomReset
    (
    END_DEVICE *	pDrvCtrl	/* pointer to END_DEVICE structure */
    )
    {
    END_LOG_MSG (END_DEBUG_LOAD, "iOlicomReset()\n", 0, 0, 0, 0, 0, 0);

    pDrvCtrl->endObj.flags &= ~(IFF_RUNNING);
    PCMCIA_ATTR_WRITE (pDrvCtrl->pcmcia.oliAttribMem + CARDCONFREG0, 0x80);
    }

/*******************************************************************************
*
* iOlicomInit - network interface initialisation routine
*
* This routine marks the interface as down, configures and initialises the
* 82595 on the PCMCIA card.
* The PCMCIA setup must have been done already and an I/O window mapped into
* the PID's address space.
*
* RETURNS: OK if successful, otherwise ERROR.
*/

LOCAL STATUS iOlicomInit
    (
    END_DEVICE *	pDrvCtrl	/* pointer to END_DEVICE structure */
    )
    {
    UCHAR *	cptr;

    pDrvCtrl->endObj.flags &= ~(IFF_UP | IFF_RUNNING);

    /*
     * Bring card out of reset. To be safe employ a
     * delay before enabling I/O.
     */

    END_LOG_MSG(END_DEBUG_LOAD,
		"iOlicomInit(): oliAttribMem = 0x%X oliAddr = 0x%X\n",
		pDrvCtrl->pcmcia.oliAttribMem, pDrvCtrl->pcmcia.oliAddr,
		0, 0, 0, 0);

    PCMCIA_ATTR_WRITE(pDrvCtrl->pcmcia.oliAttribMem + CARDCONFREG0, 0x40);

    taskDelay(sysClkRateGet()/100+1);

    PCMCIA_ATTR_WRITE(pDrvCtrl->pcmcia.oliAttribMem + CARDCONFREG0, 0x41);
    PCMCIA_ATTR_WRITE(pDrvCtrl->pcmcia.oliAttribMem + CARDCONFREG1, 0x20);

    /*
     * Check that the Exec interrupt is set, and that the
     * state is Init Done
     */

    SELECTBANK(pDrvCtrl->pcmcia.oliAddr, 0);

    if ((PCMCIA_IO_READ(pDrvCtrl->pcmcia.oliAddr + I595_R1) &
		 BNK0_ITS_MSK) != BNK0_EXE_IT ||
        (PCMCIA_IO_READ(pDrvCtrl->pcmcia.oliAddr + I595_R0) &
		 OPCODE_MASK) != RESULT_INIT_DONE)
        {

        /*
         * I'm not too sure what to do here, basically the chip has failed to
         * initialise, it may be useful to try and start from scratch, but
         * for now I'm just going to exit. Perhaps it would be better to have
         * this code return an ERROR if things don't work, then at least we
         * can inform something higher up.
         */

	END_LOG_MSG(END_DEBUG_LOAD, "iOlicomInit() problem\n",
		0, 0, 0, 0, 0, 0);
        }

    /* Acknowledge the interrupt */

    PCMCIA_IO_WRITE(pDrvCtrl->pcmcia.oliAddr + I595_R1, BNK0_EXE_IT);


    /* Program transmit and receive ring buffer parameters */

    PCMCIA_IO_WRITE(pDrvCtrl->pcmcia.oliAddr + I595_R4,
				(UCHAR) (RAM_RX_BASE + 2));
    PCMCIA_IO_WRITE(pDrvCtrl->pcmcia.oliAddr + I595_R5,
				(UCHAR) ( (RAM_RX_BASE + 2) >> 8));
    PCMCIA_IO_WRITE(pDrvCtrl->pcmcia.oliAddr + I595_R6,
				(UCHAR) RAM_RX_BASE);
    PCMCIA_IO_WRITE(pDrvCtrl->pcmcia.oliAddr + I595_R7,
				(UCHAR) (RAM_RX_BASE >> 8));
    PCMCIA_IO_WRITE(pDrvCtrl->pcmcia.oliAddr + I595_R10,
				(UCHAR) RAM_TX_BASE);
    PCMCIA_IO_WRITE(pDrvCtrl->pcmcia.oliAddr + I595_R11,
				(UCHAR) (RAM_TX_BASE >> 8));

    SELECTBANK(pDrvCtrl->pcmcia.oliAddr, 1);
    PCMCIA_IO_WRITE(pDrvCtrl->pcmcia.oliAddr + I595_R8,
                    (RAM_RX_BASE >> 8) & OCT_MSK);
    PCMCIA_IO_WRITE(pDrvCtrl->pcmcia.oliAddr + I595_R9,
                    (RAM_RX_LIMIT >> 8) & OCT_MSK);
    PCMCIA_IO_WRITE(pDrvCtrl->pcmcia.oliAddr + I595_R10,
                    (RAM_TX_BASE >> 8) & OCT_MSK);
    PCMCIA_IO_WRITE(pDrvCtrl->pcmcia.oliAddr + I595_R11,
                    (RAM_TX_LIMIT >> 8) & OCT_MSK);


    /* Clear feature selection */

    PCMCIA_IO_WRITE(pDrvCtrl->pcmcia.oliAddr + I595_R7, 0);

    SELECTBANK(pDrvCtrl->pcmcia.oliAddr, 0);

    PCMCIA_IO_WRITE(pDrvCtrl->pcmcia.oliAddr + I595_R8, 0);


    if (BNK0_EXE_STE(PCMCIA_IO_READ( pDrvCtrl->pcmcia.oliAddr + I595_R1)) !=
		BNK0_EXE_IDL)
        {
        /*
         * Not too sure what to do here either, we've got the chip up it seems
         * but the state should now be in an idle state.
         */
	END_LOG_MSG(END_DEBUG_LOAD, "iOlicomInit problem 2\n",
		0, 0, 0, 0, 0, 0);
        }

    /*
     * Select bank two of the chips registers ready to program in the
     * hardware MAC address.
     */

    SELECTBANK(pDrvCtrl->pcmcia.oliAddr, 2);
    cptr = (UCHAR *) pDrvCtrl->enetAddr;

    /* Copy in the MAC address */

    PCMCIA_IO_WRITE(pDrvCtrl->pcmcia.oliAddr + I595_R4, *cptr);
    ++cptr;
    PCMCIA_IO_WRITE(pDrvCtrl->pcmcia.oliAddr + I595_R5, *cptr);
    ++cptr;
    PCMCIA_IO_WRITE(pDrvCtrl->pcmcia.oliAddr + I595_R6, *cptr);
    ++cptr;
    PCMCIA_IO_WRITE(pDrvCtrl->pcmcia.oliAddr + I595_R7, *cptr);
    ++cptr;
    PCMCIA_IO_WRITE(pDrvCtrl->pcmcia.oliAddr + I595_R8, *cptr);
    ++cptr;
    PCMCIA_IO_WRITE(pDrvCtrl->pcmcia.oliAddr + I595_R9, *cptr);


    /* now wait for the IA-SETUP command to finish */

    SELECTBANK(pDrvCtrl->pcmcia.oliAddr, 0);

    while (BNK0_EXE_STE(PCMCIA_IO_READ(pDrvCtrl->pcmcia.oliAddr + I595_R1)) !=
		BNK0_EXE_IDL)
        /* do nothing */
        ;

    /*
     * We do not use Transmit Concurrent Processing,
     * but we do discard bad frames.
     */

    SELECTBANK(pDrvCtrl->pcmcia.oliAddr, 2);
    PCMCIA_IO_WRITE(pDrvCtrl->pcmcia.oliAddr + I595_R1, BNK2_BAD_DSC);

    /*
     * Program featues: not promiscuous, receive broadcast,
     * do not save CRC, Length Enable is on, Source Address insertion is
     * on, individual address enable, loopback off.
     */

    PCMCIA_IO_WRITE(pDrvCtrl->pcmcia.oliAddr + I595_R2, (BNK2_SAI_EN |
                                BNK2_RX_CRC));
    PCMCIA_IO_WRITE(pDrvCtrl->pcmcia.oliAddr + I595_R3,
        PCMCIA_IO_READ(pDrvCtrl->pcmcia.oliAddr + I595_R3) &
				~BNK2_TST_EN);


    /* Enable interrupts */

    SELECTBANK(pDrvCtrl->pcmcia.oliAddr, 1);
    PCMCIA_IO_WRITE(pDrvCtrl->pcmcia.oliAddr + I595_R1, BNK1_TST_EN);
    SELECTBANK(pDrvCtrl->pcmcia.oliAddr, 0);
    PCMCIA_IO_WRITE(pDrvCtrl->pcmcia.oliAddr + I595_R3,
        (PCMCIA_IO_READ(pDrvCtrl->pcmcia.oliAddr + I595_R3) &
	 	~(BNK0_RX_MK | BNK0_TX_MK | BNK0_EXE_MK)));

    RUNCMD(pDrvCtrl->pcmcia.oliAddr, CMD_RESET_TRISTATE);
    RUNCMD(pDrvCtrl->pcmcia.oliAddr, CMD_RCV_ENABLE);

    return OK;
    }

/*******************************************************************************
*
* iOlicomTxBdQueueClean - clean the transmit buffer descriptor queue
*
* This routine cleans the transmit buffer queue.
*
* RETURNS: N/A
*
*/

LOCAL void iOlicomTxBdQueueClean
    (
    END_DEVICE *	pDrvCtrl        /* pointer to END_DEVICE structure */
    )
    {
    TX_BD *	pTxBd;
    ULONG	addrC;
    int		s;
    int		status;

    END_LOG_MSG (END_DEBUG_TX, "iOlicomTxBdQueueClean ...\n", 0, 0, 0, 0, 0, 0);

    s = intLock();

    while (pDrvCtrl->txBdIndexC != pDrvCtrl->txBdNext)
        {
        pTxBd = & pDrvCtrl->txBdBase[pDrvCtrl->txBdIndexC];
	addrC = (ULONG) pTxBd->dataPointer;

        /* read the current Transmit Done flag */

	PCMCIA_IO_WRITE(pDrvCtrl->pcmcia.oliAddr + I595_R12, addrC);
	PCMCIA_IO_WRITE(pDrvCtrl->pcmcia.oliAddr + I595_R13, (addrC >> 8));
	READ_R14_R15 (status);

        if (status & TX_BD_READY)
	    {
	    READ_R14_R15 (status);

            /* check for output errors */

            if (!(status & XMT_OK))
	        {
	        if ((status & XMT_COL_MAX) || (status & XMT_UND_RUN) ||
	    	    (status & XMT_LST_CRS))	/* Restart a transmit command */
		    {
		    (void) netJobAdd ((FUNCPTR) iOlicomTxRestart,
					(int) pDrvCtrl, 0,0,0,0);
		    break;
		    }
	        }
	    }
	else
	    {
	    /* Send a Transmit command if the transmit queue is not empty */

	    iOlicomTxStart(pDrvCtrl);
	    break;
	    }

	/* free the buffer */

        if (pDrvCtrl->freeBuf[pDrvCtrl->txBdIndexC].pClBuf != NULL)
            {
	    NET_BUF_FREE(pDrvCtrl->freeBuf[pDrvCtrl->txBdIndexC].pClBuf);
            pDrvCtrl->freeBuf[pDrvCtrl->txBdIndexC].pClBuf = NULL;
            }

        /* incr txBdIndexC */

        pDrvCtrl->txBdIndexC = (pDrvCtrl->txBdIndexC + 1) % pDrvCtrl->txBdNum;
        }

    DRV_FLAGS_CLR(OLI_TX_CLEANING);

    intUnlock(s);
    }

/*******************************************************************************
*
* iOlicomTxStart - issue a XMT command to the 82595TX
*
* This routine issues a XMT command to the 82595TX.
*
* RETURNS: N/A
*/

LOCAL void iOlicomTxStart
    (
    END_DEVICE *	pDrvCtrl	/* pointer to END_DEVICE structure */
    )
    {
    TX_BD *	pTxBd;
    ULONG		addrC;
    int			s;

    END_LOG_MSG (END_DEBUG_TX, "iOlicomTxStart \n", 0, 0, 0, 0, 0, 0);

    pTxBd = & pDrvCtrl->txBdBase[pDrvCtrl->txBdIndexC];
    addrC = (ULONG) pTxBd->dataPointer;

    /* lock interrupts */

    s = intLock ();

    /* Write the XMT base address register */

    PCMCIA_IO_WRITE(pDrvCtrl->pcmcia.oliAddr + I595_R10, addrC);
    PCMCIA_IO_WRITE(pDrvCtrl->pcmcia.oliAddr + I595_R11, (addrC >> 8));

    /* Issue a XMT command to the 82595TX */

    RUNCMD(pDrvCtrl->pcmcia.oliAddr, CMD_TRANSMIT);

    /* Unlock interrupts */

    intUnlock (s);
    }

/*******************************************************************************
*
* iOlicomTxRestart - issue an other XMT  command to the 82595TX
*
* This routine issues an other XMT command to the 82595Tx.  It is
* executed by netTask (iOlicomTxBdQueueClean() did a netJobAdd).
*
* RETURNS: N/A
*/

LOCAL void iOlicomTxRestart
    (
    END_DEVICE *        pDrvCtrl        /* pointer to END_DEVICE structure */
    )
    {

    END_LOG_MSG (END_DEBUG_TX, "iOlicomTxRestart \n", 0, 0, 0, 0, 0, 0);

    /* update error counter */

    END_ERR_ADD (&pDrvCtrl->endObj, MIB2_OUT_ERRS, +1);
    END_ERR_ADD (&pDrvCtrl->endObj, MIB2_OUT_UCAST, -1);

    /* restart a transmit */

    iOlicomTxStart(pDrvCtrl);
    }

/*******************************************************************************
*
* iOlicomPcmciaReadTuple - read a tuple from the given offset in attribute space
*
* RETURNS: offset to beginning of next tuple, otherwise 0
*/

LOCAL UINT iOlicomPcmciaReadTuple
    (
    END_DEVICE *	pDrvCtrl,	/* pointer to END_DEVICE structure */
    UINT		offset,		/* offset into tuple space */
    UCHAR *		buffer,		/* pointer to the holding buffer */
    UINT		buflen		/* size of the buffer in bytes */
    )
    {
    PCMCIASOCKET *	pcs = &pDrvCtrl->pcmcia.theSocket;
    UINT		tuple_type;
    UINT		tuple_length;
    UINT		base = pcs->pcs_attrbase;
    UCHAR *		cptr = buffer;

    END_LOG_MSG (END_DEBUG_TUPLES,
    "iOlicomPcmciaReadTuple() going to read from (base+offset) (0x%X + 0x%X\n",
		base, offset, 0, 0, 0, 0);

    /* Load the tuple type, check for end of list markers  */

    tuple_type = PCMCIA_ATTR_READ (base + offset);
	
    END_LOG_MSG (END_DEBUG_TUPLES,
		 "iOlicomPcmciaReadTuple(): tuple type = 0x%X\n",
		 tuple_type, 0, 0, 0, 0, 0);

    if ((tuple_type == 0xFF) || (tuple_type == 0x14))
	{
	END_LOG_MSG (END_DEBUG_TUPLES,
				"iOlicomPcmciaReadTuple(): end of tuple list\n",                                0, 0, 0, 0, 0, 0);
	return 0;
	}


    /*
     * We are going to be returning at least a type and a link,
     * so get these into the buffer.
     */

    *(cptr++) = tuple_type;
    offset += 2;

    END_LOG_MSG (END_DEBUG_TUPLES,
    "iOlicomPcmciaReadTuple() going to read from (base+offset) (0x%X + 0x%X\n",
		base, offset, 0, 0, 0, 0);

    tuple_length = PCMCIA_ATTR_READ(base + offset);
    *cptr = tuple_length;
    offset += 2;

    END_LOG_MSG (END_DEBUG_TUPLES,
	"iOlicomPcmciaReadTuple(): tuple length = 0x%X\n",
	tuple_length, 0, 0, 0, 0, 0);

    /* Check for tuple link marking the end of a list */

    if (tuple_length == 0xFF)
        /* End of the list marker */
        return 0;

    /*
     * Copy the tuple data from attribute space into our buffer
     * until we reach the end of the tuple or run out of buffer
     * space (unlikely).
     */

    buflen -= 2;
    if (buflen > tuple_length)
        buflen = tuple_length;

    while (buflen-- > 0)
	{
        *++cptr = PCMCIA_ATTR_READ(base + offset);
        offset += 2;
	}

    /* finished */

    END_LOG_MSG (END_DEBUG_TUPLES,
	"iOlicomPcmciaReadTuple() returning 0x%X\n",
		offset, 0, 0, 0, 0, 0);

    return offset;
    }

/*******************************************************************************
*
* iOlicomPcmciaFindTuple - search a card's attribute space for a given tuple.
*
* RETURNS: Pointer to a buffer holding the tuple data.
*/

LOCAL UCHAR *iOlicomPcmciaFindTuple
    (
    END_DEVICE *	pDrvCtrl,	/* pointer to END_DEVICE structure */
    UINT		tuple,		/* tuple code to find */
    UINT *		offset,		/* offset in attr space */
    UCHAR *		buffer,		/* buffer to hold data */
    UINT		buflen		/* size of buffer */
    )
    {
    UINT	loff = *offset;                /* local offset */

    END_LOG_MSG (END_DEBUG_TUPLES,"iOlicomPcmciaFindTuple(), find tuple 0x%X\n",
		tuple, 0, 0, 0, 0, 0);

    /* guard against potential false positives */

    *buffer = ~tuple;

    /* keep going until the tuple is found, or proven not to exist  */

    do
    {
        loff = iOlicomPcmciaReadTuple(pDrvCtrl, loff, buffer, buflen);
    }
    while (loff != 0 && *buffer != tuple);

    *offset = loff;

#ifdef DEBUG
    if (*buffer != tuple)
	END_LOG_MSG (END_DEBUG_LOAD_FAIL,
		     "PcmciaFindTuple(): tuple 0x%X not found\n",
		     tuple, 0, 0, 0, 0, 0);
#endif

    return (*buffer == tuple) ? buffer : NULL;
    }

/*******************************************************************************
*
* iOlicomReadCardAttributes - read a card's attributes
*
* This is really read card attributes and check it's an Olicom Ethernet card.
* If this call returns OK, the card in the slot is an Olicom and the base
* address of the Ethernet hardware will have been set up in the pDrvCtrl
* structure.
*
* RETURNS: OK if successful, otherwise ERROR.
*/

LOCAL STATUS iOlicomReadCardAttributes
    (
    END_DEVICE *	pDrvCtrl	/* pointer to END_DEVICE structure */
    )
    {
    UCHAR	buffer[256];
    UCHAR *	cptr;
    UINT        offset = 0;
    UINT        iobase;
    UCHAR *	hwaddr = (UCHAR *) pDrvCtrl->enetAddr;

    /*
     * There are four tuples that we need to read from the card:
     *
     * CARD_FUNCID - Should identify the card as an Network Adaptor
     *
     * CARD_FUNCE - Function extension tuple detailing the Ethernet
     * hardware address
     *
     * CARD_CONFIG - Describes card configuration registers
     *
     * CARD_CFT_ENTRY - There are several different copies of this
     * one tuple, which describe the different I/O addresses accepted by
     * the Intel 82595 Controller.
     *
     * These tuples should be in the order listed, so we start searching
     * for the next tuple from where the previous one finished, but we
     * also try again from the beginning if the search failed.
     */

    /* Find FUNCID tuple first */

    if ((cptr = iOlicomPcmciaFindTuple(pDrvCtrl, CARD_FUNCID, &offset,
        buffer, sizeof(buffer))) == NULL)
        return ERROR;

    if (*(cptr + 2) != 0x06)
        return ERROR;

    /* Look for FUNCE tuple */

    if ((cptr = iOlicomPcmciaFindTuple(pDrvCtrl, CARD_FUNCE, &offset,
        buffer, sizeof(buffer))) == NULL)
        {

        /* try again, from the beginning */

        offset = 0;
        if ((cptr = iOlicomPcmciaFindTuple(pDrvCtrl, CARD_FUNCE, &offset,
            buffer, sizeof(buffer))) == NULL)
            return ERROR;
        }

    /*
     * this should be flagged as a node id tuple (byte 2 = 0x04), with
     * a length (byte 3) of 6 bytes
     */

    if (*(cptr + 2) != 0x04 || *(cptr + 3) != 0x06)
        return ERROR;

    /* OK, copy the hardware address */

    memcpy(hwaddr, cptr + 4, 6);

    /* Find CONFIG tuple */

    if ((cptr = iOlicomPcmciaFindTuple(pDrvCtrl, CARD_CONFIG, &offset,
        buffer, sizeof(buffer))) == NULL)
        {

        /* try again, from the beginning */

        offset = 0;
        if ((cptr = iOlicomPcmciaFindTuple(pDrvCtrl, CARD_CONFIG, &offset,
            buffer, sizeof(buffer))) == NULL)
            return ERROR;
        }

    END_LOG_MSG (END_DEBUG_TUPLES,
	"iOlicomReadCardAttributes() CONFIG Regs at 0x%X%X\n",
		*(cptr + 5), *(cptr + 4), 0, 0, 0, 0);


    /*
     * OK, bytes 4 and 5 of the tuple contain the address (in attribute
     * memory) of the card configuration registers (lsb first), byte 6
     * contains the bitmap of which registers are present
     */

    if (*(cptr + 6) != 0x03)
        return ERROR;

    /*
     * tuple 4 - CARD_CFT_ENTRY
     *
     * I must confess to a certain level of ignorance here: using the
     * version of the PCMCIA Metaformat Specification that we have here
     * (November 1995) I cannot fully decode the default Configuration
     * Table Entry supplied by the Olicom Ethernet Card.  However, I
     * *can* understand the I/O Space Descriptions, of which the Olicom
     * card supplies several (because it does not fully decode the I/O
     * address); fortuitously, for all non-default CFTABLE_ENTRY tuples,
     * these I/O Space Descriptions are the first descriptions within
     * the tuple, and therefore easy to find.
     */

    if ((cptr = iOlicomPcmciaFindTuple(pDrvCtrl, CARD_CFT_ENTRY, &offset,
        buffer, sizeof(buffer))) == NULL)
        {

        /* try again, from the beginning */

        offset = 0;
        if ((cptr = iOlicomPcmciaFindTuple(pDrvCtrl, CARD_CFT_ENTRY, &offset,
            buffer, sizeof(buffer))) == NULL)
            return ERROR;
        }

    /* keep going until we get some I/O space mapped */

    for (;;)
        {

        /*
         * don't bother trying if this is the default configuration
         * tuple (identified by bit 6 of byte 2); can't cope with the
         * Interface Description field (flagged by bit 7, byte 2) being
         * present either.
         */

        if (((*(cptr + 2)) & 0xC0) == 0)
            {

            /*
             * Paranoia (i.e. limited implementation) checks:
             *
             * Bit 3 of byte 3 flags an I/O description structure;
             * this should be the lsb set in the byte.
             *
             * Byte 5 describes the I/O range - we can only
             * cope with a specific case (the general case
             * is neither hard, nor worth implementing).
             */

            if ((*(cptr + 3) & 0x0F) == 0x08 && *(cptr + 5) == 0x60)
                {

                /*
                 * OK, we can cope with this tuple: bytes 6 and 7
                 * give the I/O base address, and byte 8 gives the
                 * range (which should be 0x0F)
                 */

                if (*(cptr + 8) != 0x0F)
                    return ERROR;

                /*
                 * pull out the base address, and try to get this
                 * mapped into the PCMCIA controller's I/O space;
                 * note that we also ask for Interrupts to be "enabled"
                 * by the PCMCIA controller, but no interrupt sources
                 * on the card itself are enabled yet.
                 */

                iobase = (*(cptr + 7) << 8) | *(cptr + 6);
                if (iOlicomMapIOSpace(pDrvCtrl, iobase, 0x0F, 1) == OK)
                    {

                    /*
                     * calculate the actual base address of the
                     * chip in I/O space
                     */

		    END_LOG_MSG (END_DEBUG_LOAD,
		    "ReadCardAttributes(), iobase = 0x%X\n",
		    iobase, 0, 0, 0, 0, 0);

		    if (pDrvCtrl->pcmcia.socket == PC_SOCKET_A)
			{
			pDrvCtrl->pcmcia.oliAddr =
					(char *) PCMCIA_CALC_ADDR(iobase)
					+ (UINT) pDrvCtrl->pcmcia.ioBaseA;
			}
		    else
			{
			pDrvCtrl->pcmcia.oliAddr =
					(char *) (PCMCIA_CALC_ADDR(iobase)
					+ (UINT) pDrvCtrl->pcmcia.ioBaseB);

			}
                    return OK;
                    }
                }
            }

            if ((cptr = iOlicomPcmciaFindTuple(pDrvCtrl,
					 CARD_CFT_ENTRY, &offset,
                buffer, sizeof(buffer))) == NULL)
            return ERROR;
        }

    }

/*******************************************************************************
*
* iOlicomInitialiseCard - initialise the card
*
* Do any work necessary to bring a card to a consistent
* state, and implicitly check it is an Olicom ethernet card via the
* iOlicomReadCardAttributes() function.
*
* RETURNS: OK if successful, otherwise ERROR.
*/

LOCAL STATUS iOlicomInitialiseCard
    (
    END_DEVICE *	pDrvCtrl	/* pointer to END_DEVICE structure */
    )
    {
    UCHAR	buffer[256];
    UCHAR *	cptr;
    UINT        offset = 0;

    END_LOG_MSG (END_DEBUG_LOAD, "iOlicomInitialiseCard()\n",
		0, 0, 0, 0, 0, 0);

    if ((cptr = iOlicomPcmciaFindTuple(pDrvCtrl, 0x15,
        &offset, buffer, sizeof(buffer))) != NULL)
        {

        /*
         * An Olicom Ethercom card has been found in the socket. Ascertain
         * the I/O space mapping from the tuple headers and then set up an
         * I/O window for this card.
         */

        if (iOlicomReadCardAttributes(pDrvCtrl) == OK)
	    {
	    END_LOG_MSG (END_DEBUG_LOAD,
		"iOlicomCardInitialiseCard(): found Olicom card\n",
			0, 0, 0, 0, 0, 0);
            return OK;
	    }
	else
	    END_LOG_MSG (END_DEBUG_LOAD_FAIL, "ReadCardAttributes() failed\n",
		 0, 0, 0, 0, 0, 0);
        }

    END_LOG_MSG (END_DEBUG_LOAD_FAIL, "card in slot is not Olicom\n",
		 0, 0, 0, 0, 0, 0);

    iOlicomShutdownSlot(pDrvCtrl, pDrvCtrl->pcmcia.socket);

    return ERROR;
    }

/*******************************************************************************
*
* iOlicomCardInsertion - cope with the insertion of a card
*
* Called when a card is detected in a slot, either by a real insertion
* or from detection during initialisation.
*
* RETURNS: OK if successful, otherwise ERROR.
*/

LOCAL STATUS iOlicomCardInsertion
    (
    END_DEVICE *	pDrvCtrl,	/* pointer to END_DEVICE structure */
    UINT		socket		/* socket ID */
    )
    {
    END_LOG_MSG (END_DEBUG_LOAD, "iOlicomCardInsertion() socket = 0x%X\n",
	socket, 0, 0, 0, 0, 0);

    /*
     * first set up the PCMCIA slot, then try
     * to set up the card itself
     */

    iOlicomInitialiseSlot(pDrvCtrl, socket);

    return iOlicomInitialiseCard(pDrvCtrl);
    }

#ifndef PCMCIA_NO_VADEM
/*******************************************************************************
*
* iOlicomInt - handle controller interrupt
*
* This interrupt service routine, reads device interrupt status, acknowledges
* the interrupting events, and schedules receive and transmit processing when
* required.
*
* RETURNS: N/A.
*/

LOCAL void iOlicomInt
    (
    END_DEVICE *	pDrvCtrl	/* pointer to END_DEVICE structure */
    )
    {
    int		irqType;
    int		cardStat;

    /* On some systems it may be necessary to acknowledge the interrupt. */

    SYS_BUS_INT_ACK(pDrvCtrl);

    /*
     * Handle low latency interrupt processing, such as device errors,
     * output complete, etc. Merely reading the interrupt state acknowledges
     * the request.
     *
     * First, check the PCMCIA status register. This should either be
     * generating a card detect change interrupt (due to card insertion or
     * removal) or a card I/O interrupt due to some event on the inserted
     * card.
     */

    irqType = OLI_VADEM_READ(PCCSTATCHNG + pDrvCtrl->pcmcia.socket);

    if (irqType & GSR_CDC) /* This is a card detect change */
        {
        /*
         * For card detect changes either a card has been plugged in or
         * a card has been removed. These are hooks for later extension and
         * neither event is handled here.
         */

        cardStat = (OLI_VADEM_READ(PCIFSTATUS + pDrvCtrl->pcmcia.socket))
			 & GSR_CD;

        if (cardStat == GSR_CD)
            {
            /* This is a card insertion. */
            }
        else
            {
            /* Yikes! My card doesn't work. */
            }

	END_LOG_MSG (END_DEBUG_INT, "False interrupt.\n", 0, 0, 0, 0, 0, 0);
        return;

        }
    else /* A card state interrupt, examine the card for more */
	iOlicomIntHandle(pDrvCtrl);

    return;
    }

/*******************************************************************************
*
* iOlicomPcmciaSetup - bring up the PCMCIA hardware
*
* This routine examines the two slots and attempts to initialise any
* Olicom card found.
*
* RETURNS: OK if successful, otherwise ERROR
*/

LOCAL STATUS iOlicomPcmciaSetup
    (
    END_DEVICE *	pDrvCtrl	/* pointer to END_DEVICE structure */
    )
    {
    END_LOG_MSG (END_DEBUG_LOAD, "iOlicomPcmciaSetup() - %s%d\n",
			 DRV_NAME, pDrvCtrl->unit, 0, 0, 0, 0);

    /*
     * VG-468 IRQs 4 & 3 are tied to AMBA interrupt lines
     * IRQ_CARDA and IRQ_CARDB respectively, set IRQ
     * steering as appropriate, and enable Card Detect
     * interrupts on both sockets
     */

    OLI_VADEM_WRITE(PCSTATCHNGINT + PC_SOCKET_A, (GSR_IRQ4 | GSR_CDE));
    OLI_VADEM_WRITE(PCSTATCHNGINT + PC_SOCKET_B, (GSR_IRQ3 | GSR_CDE));

    /* clear status registers */

    OLI_VADEM_READ(PCCSTATCHNG + PC_SOCKET_A);
    OLI_VADEM_READ(PCCSTATCHNG + PC_SOCKET_B);

    /*
     * Map the attribute memory of both sockets into ARM
     * address space. Then shutdown the sockets.
     */

    iOlicomMapMemory(pDrvCtrl, PC_SOCKET_A, 0, ATTRIB_MEMORY_A,
                ATTRIBUTE_MEMORY_SLOT, 0, ATTRIBUTE);
    iOlicomShutdownSlot(pDrvCtrl, PC_SOCKET_A);
    iOlicomMapMemory(pDrvCtrl, PC_SOCKET_B, 0, ATTRIB_MEMORY_B,
                ATTRIBUTE_MEMORY_SLOT, 0, ATTRIBUTE);
    iOlicomShutdownSlot(pDrvCtrl, PC_SOCKET_B);

    /*
     * Enable card detect debouncing, and Tri-state
     * unused pins. We need to unlock the VG-468
     * unique registers before this will work
     */

    OLI_VADEM_UNLOCK();

    OLI_VADEM_WRITE(PCCONTROL + PC_SOCKET_A, 0x00);
    OLI_VADEM_WRITE(PCCONTROL + PC_SOCKET_B, 0x00);


    END_LOG_MSG (END_DEBUG_LOAD, "Looking for cards\n", 0, 0, 0, 0, 0, 0);

    /*
     * initialise socket descriptor, first check socket A
     * and then socket B, looking for an Olicom Ethernet
     * card.
     */

    pDrvCtrl->pcmcia.cardPresent = FALSE;
    pDrvCtrl->pcmcia.socket = PC_SOCKET_A;
    pDrvCtrl->pcmcia.theSocket.pcs_attrbase = (UINT)pDrvCtrl->pcmcia.attrBaseA;
    pDrvCtrl->pcmcia.oliAttribMem = pDrvCtrl->pcmcia.attrBaseA;
    pDrvCtrl->pcmcia.theSocket.pcs_attrrange = ATTRIBUTE_MEMORY_SLOT;


    if (((OLI_VADEM_READ(PCIFSTATUS + PC_SOCKET_A) & GSR_CD) == GSR_CD) &&
        ( iOlicomCardInsertion(pDrvCtrl, PC_SOCKET_A) == OK ))
        {
	END_LOG_MSG (END_DEBUG_LOAD, "Found card A\n", 0, 0, 0, 0, 0, 0);

        pDrvCtrl->ilevel = pDrvCtrl->pcmcia.intLevelA;
        pDrvCtrl->ivec = pDrvCtrl->pcmcia.intVectA;
        return OK;
        }

    pDrvCtrl->pcmcia.socket = PC_SOCKET_B;
    pDrvCtrl->pcmcia.theSocket.pcs_attrbase = (UINT)pDrvCtrl->pcmcia.attrBaseB;
    pDrvCtrl->pcmcia.oliAttribMem = pDrvCtrl->pcmcia.attrBaseB;

    if (((OLI_VADEM_READ(PCIFSTATUS + PC_SOCKET_B) & GSR_CD) == GSR_CD) &&
        ( iOlicomCardInsertion(pDrvCtrl, PC_SOCKET_B) == OK ))
        {
	END_LOG_MSG (END_DEBUG_LOAD, "Found card B\n", 0, 0, 0, 0, 0, 0);

        pDrvCtrl->ilevel = pDrvCtrl->pcmcia.intLevelB;
        pDrvCtrl->ivec = pDrvCtrl->pcmcia.intVectB;
        return OK;
        }

    END_LOG_MSG (END_DEBUG_LOAD_FAIL, "Failed to find card in either slot\n",
		 0, 0, 0, 0, 0, 0);
    return ERROR;
    }

/*******************************************************************************
*
* iOlicomInitialiseSlot - initialise PCMCIA slot
*
* Called once a card has been detected in a PCMCIA slot, this function applies
* power to the slot and maps the attribute memory in.
*
* RETURNS: N/A
*/

LOCAL void iOlicomInitialiseSlot
    (
    END_DEVICE *	pDrvCtrl,	/* pointer to END_DEVICE structure */
    UINT		socket		/* socket structure */
    )
    {
    END_LOG_MSG (END_DEBUG_LOAD, "iOlicomInitialiseSlot() socket = 0x%X\n",
		socket, 0, 0, 0, 0, 0);

    /* socket now has a card present */

    pDrvCtrl->pcmcia.cardPresent = TRUE;

    /* Turn on power (use auto-select) */

    OLI_VADEM_WRITE(PCPWRRSTCTL + socket, (GSR_CPE | GSR_PSE));

    /* delay for at least 300 ms */

    taskDelay (((sysClkRateGet()*3)/10) + 1);


    /* then enable output. */

    OLI_VADEM_WRITE(PCPWRRSTCTL + socket, (GSR_CPE | GSR_PSE | GSR_OE));


    taskDelay (1);

    /*
     * reset the card by asserting the reset line, waiting a bit,
     * then taking reset off again
     */

    OLI_VADEM_WRITE(PCINTGCTL + socket, 0x00);

    /* assert reset for at least 10 ms */

    taskDelay ((sysClkRateGet()/100) + 1);

    /* deassert reset */

    OLI_VADEM_WRITE(PCINTGCTL + socket, GSR_DRT);

    /* delay for at least 20 ms */

    taskDelay (((sysClkRateGet()* 2)/100) + 1);

    /* Wait until card is ready */

    while ((OLI_VADEM_READ(PCIFSTATUS + socket) & GSR_RDY) == 0)
        /* do nothing */
        ;

    /* Enable window 0, which is the attribute memory window  */

    OLI_VADEM_WRITE(PCADDWINEN + socket, GSR_WD0);
    }

/*******************************************************************************
*
* iOlicomShutdownSlot - shutdown a PCMCIA slot
*
* This routine is the opposite of iOlicomInitialiseSlot(), it removes power to
* a PCMCIA slot and maps the card out of the processor's address space.
*
* RETURNS: N/A
*/

LOCAL void iOlicomShutdownSlot
    (
    END_DEVICE *	pDrvCtrl,	/* pointer to END_DEVICE structure */
    UINT		socket		/* socket structure */
    )
    {
    /* Disable all I/O interrupts, and reset the card */

    OLI_VADEM_WRITE(PCINTGCTL + socket, 0x00);

    /* Disable all memory windows */

    OLI_VADEM_WRITE(PCADDWINEN + socket, 0x00);

    /* Remove the card's power */

    OLI_VADEM_WRITE(PCPWRRSTCTL + socket, GSR_PSE);
    }

/*******************************************************************************
*
* iOlicomMapMemory - map a section of memory into the processor's address space.
*
* RETURNS: N/A
*/

LOCAL void iOlicomMapMemory
    (
    END_DEVICE *	pDrvCtrl,	/* pointer to END_DEVICE structure */
    UINT		socket,		/* Socket ID*/
    UINT		window,		/* window to use */
    UINT		base,		/* base address */
    UINT		range,		/* mapping range */
    UINT		offset,		/* offset in space */
    MEMTYPE		type		/* Memory type */
    )
    {
    UINT	reg;
    UINT	stop;
    UINT	cardoff;

    /* registers for each window are 8 bytes apart */

    reg = socket + (window << 3);

    /* set start address, 16-bit data path, and no Zero Wait State */

    OLI_VADEM_WRITE(PCSYSA0MSTARTL + reg, (base >> 12) & OCT_MSK);
    OLI_VADEM_WRITE(PCSYSA0MSTARTH + reg, ((base >> 20) & 0x0F) | 0x80);

    /* set stop address, and 3 wait states for 16-bit access */

    stop = base + (range - 1);
    OLI_VADEM_WRITE(PCSYSA0MSTOPL + reg, (stop >> 12) & OCT_MSK);
    OLI_VADEM_WRITE(PCSYSA0MSTOPH + reg, ((stop >> 20) & 0x0F) | 0xC0);

    /* set card offset, no Write Protect, and attribute bit */

    cardoff = offset - base;
    OLI_VADEM_WRITE(PCCMEMA0OFFL + reg, (cardoff >> 12) & OCT_MSK);
    OLI_VADEM_WRITE(PCCMEMA0OFFH + reg, (((cardoff >> 20) & 0x3F) |
			 ((type == ATTRIBUTE) ? 0x40 : 0x00)));
    }

/*******************************************************************************
*
* iOlicomMapIOSpace - map I/O space
*
* Attempt to map I/O space on the PCMCIA card into processor address space.
*
* RETURNS: OK if successful, otherwise ERROR
*/

LOCAL STATUS iOlicomMapIOSpace
    (
    END_DEVICE *	pDrvCtrl,	/* pointer to END_DEVICE structure */
    UINT		base,		/* Base address of IO space */
    UINT		range,		/* Range of address to map */
    int			mapirq		/* Interrupt line to map */
    )
    {
    PCMCIASOCKET *	pcs = &pDrvCtrl->pcmcia.theSocket;
    PCMCIASOCKET *	altpcs;
    UINT		socket = pDrvCtrl->pcmcia.socket;
    UINT		irqline;

    /*
     * As with the Card Detect Interrupts in pcmciaSetup(), we
     * need to route IRQs from sockets A and B to IRQ4 and IRQ3
     * respectively
     */

    if (socket == PC_SOCKET_A)
        {
        irqline = 0x04;
        altpcs = pcs;
        }
    else
        {
        irqline = 0x03;
        altpcs = pcs;
        }

    /* check that I/O ranges do not overlap */

    if ((base >= altpcs->pcs_iobase &&
         base <= altpcs->pcs_iobase + altpcs->pcs_iorange) ||
        (base + range >= altpcs->pcs_iobase &&
         base + range <= altpcs->pcs_iobase + altpcs->pcs_iorange))
        /* Ranges overlap, not legal */
        return ERROR;

    /* OK - program I/O range for window 0 */

    OLI_VADEM_WRITE(PCIOA0STARTL + socket, base & OCT_MSK);
    OLI_VADEM_WRITE(PCIOA0STARTH + socket, (base >> 8) & OCT_MSK);
    OLI_VADEM_WRITE(PCIOA0STOPL + socket, (base + range) & OCT_MSK);
    OLI_VADEM_WRITE(PCIOA0STOPH + socket, ((base + range) >> 8) & OCT_MSK);

    /*
     * it is probably safe to assume that this socket contains
     * an I/O type card, so set the appropriate bit in the
     * control register
     */

    OLI_VADEM_WRITE(PCINTGCTL + socket, 0x60 | ((mapirq) ? irqline : 0));

    /*
     * This is by no means general, but at the moment I don't know how
     * to extract the relevant information, and I'm not sure how to
     * implement a general scheme for it anyway, so for the time being
     * it stays as a hard wired number.
     */

    OLI_VADEM_WRITE(PCIOCTL + socket, 0x00);

    /* enable I/O window 0 */

    OLI_VADEM_WRITE(PCADDWINEN + socket,
		 (OLI_VADEM_READ(PCADDWINEN + socket) | 0x60));

    return OK;
    }


/*
 * Routines for accessing VG-468 controller.
 *
 * The Vadem chip will always shift data down to D[7:0] for reads,
 * but the data register is located at a byte-1 offset, so this shifting
 * of data conflicts with the ARM's behaviour during non-word aligned reads
 * (the ARM will try to shift byte-1 to D[7:0] itself).
 * PC_INDEX, PC_DATA_READ and PC_DATA_WRITE are macros defined in iOlicomEnd.h.
 * They depend on the base address of PCMCIA space, which is why this is
 * passed to all routines and seemingly not used.
 */

/******************************************************************************
*
* iOlicomVademRead - Read a register from the Vadem PCMCIA controller
*
* RETURNS: Value of requested register
*/

LOCAL UINT iOlicomVademRead
    (
    char *	ctrlBase,	/* Base address of Vadem PCMCIA controller */
    UINT        index		/* Index of register to read */
    )
    {
    *PC_INDEX = index;
    return ((*PC_DATA_READ >> 24) & OCT_MSK);
    }

/******************************************************************************
*
* iOlicomVademWrite - Write a register in the Vadem PCMCIA controller
*
* RETURNS: N/A
*/

LOCAL void iOlicomVademWrite
    (
    char *	ctrlBase,	/* Base address of Vadem PCMCIA controller*/
    UINT        index,		/* Index of register to write */
    UINT        value		/* Value to be written */
    )
    {
    *PC_INDEX = index;
    *PC_DATA_WRITE = value;
    }

/*******************************************************************************
*
* iOlicomVademUnlock - Perform Vadem magic 'Unlock' sequence.
*
* To prevent erroneous configuration, the Vadem controller requires this
* magic unlock sequence to be issued before it will respond to any accesses
* made to the Vadem unique register set.
*
* RETURNS: N/A
*/

LOCAL void iOlicomVademUnlock
    (
    char *	ctrlBase	/* Base address of Vadem PCMCIA controller*/
    )
    {
    *PC_INDEX = 0x0E;		/* First 'magic' access */
    *PC_INDEX = 0x37;		/* Second 'magic' access */
    }

#endif /* !PCMCIA_NO_VADEM */

#ifdef DEBUG

/* miscellaneous debug routines */

void drvFlagsIsset (int setBits)
    {
    END_DEVICE* pDrvCtrl = pDbgDrvCtrl;

    logMsg ("flag is %s\n",
			(DRV_FLAGS_ISSET(setBits) ? (int)"set" : (int)"clear"),
			0, 0, 0, 0, 0);
    }

void iOlicomRxState(void)
    {
    END_DEVICE* pDrvCtrl = pDbgDrvCtrl;

    logMsg ("reg 00 = 0x%02x\n", PCMCIA_IO_READ(pDrvCtrl->pcmcia.oliAddr + I595_R0),0,0,0,0,0);
    logMsg ("reg 01 = 0x%02x\n", PCMCIA_IO_READ(pDrvCtrl->pcmcia.oliAddr + I595_R1),0,0,0,0,0);
    logMsg ("reg 02 = 0x%02x\n", PCMCIA_IO_READ(pDrvCtrl->pcmcia.oliAddr + I595_R2),0,0,0,0,0);
    logMsg ("reg 03 = 0x%02x\n", PCMCIA_IO_READ(pDrvCtrl->pcmcia.oliAddr + I595_R3),0,0,0,0,0);
    logMsg ("reg 04 = 0x%02x\n", PCMCIA_IO_READ(pDrvCtrl->pcmcia.oliAddr + I595_R4),0,0,0,0,0);
    logMsg ("reg 05 = 0x%02x\n", PCMCIA_IO_READ(pDrvCtrl->pcmcia.oliAddr + I595_R5),0,0,0,0,0);
    logMsg ("reg 06 = 0x%02x\n", PCMCIA_IO_READ(pDrvCtrl->pcmcia.oliAddr + I595_R6),0,0,0,0,0);
    logMsg ("reg 07 = 0x%02x\n", PCMCIA_IO_READ(pDrvCtrl->pcmcia.oliAddr + I595_R7),0,0,0,0,0);
    logMsg ("reg 08 = 0x%02x\n", PCMCIA_IO_READ(pDrvCtrl->pcmcia.oliAddr + I595_R8),0,0,0,0,0);
    logMsg ("reg 09 = 0x%02x\n", PCMCIA_IO_READ(pDrvCtrl->pcmcia.oliAddr + I595_R9),0,0,0,0,0);
    logMsg ("reg 10 = 0x%02x\n", PCMCIA_IO_READ(pDrvCtrl->pcmcia.oliAddr + I595_R10),0,0,0,0,0);
    logMsg ("reg 11 = 0x%02x\n", PCMCIA_IO_READ(pDrvCtrl->pcmcia.oliAddr + I595_R11),0,0,0,0,0);
    logMsg ("reg 12 = 0x%02x\n", PCMCIA_IO_READ(pDrvCtrl->pcmcia.oliAddr + I595_R12),0,0,0,0,0);
    logMsg ("reg 13 = 0x%02x\n", PCMCIA_IO_READ(pDrvCtrl->pcmcia.oliAddr + I595_R13),0,0,0,0,0);
    logMsg ("reg 14 = 0x%02x\n", PCMCIA_IO_READ(pDrvCtrl->pcmcia.oliAddr + I595_R14),0,0,0,0,0);
    logMsg ("reg 15 = 0x%02x\n", PCMCIA_IO_READ(pDrvCtrl->pcmcia.oliAddr + I595_R15),0,0,0,0,0);
    }

void iOlicomDebugRcv(void)
    {
    END_DEVICE* pDrvCtrl = pDbgDrvCtrl;

    int s;
    int start;
    int status;
    int event;
    int length;

    s = intLock();

    start = PCMCIA_IO_READ(pDrvCtrl->pcmcia.oliAddr + I595_R6);
    start |= (PCMCIA_IO_READ(pDrvCtrl->pcmcia.oliAddr + I595_R7) << 8);
    start += 2;

    logMsg ("start= 0x%04x\n", start, 0, 0, 0, 0, 0);

    /* Set up address from where we wish to start reading data */

    PCMCIA_IO_WRITE(pDrvCtrl->pcmcia.oliAddr + I595_R12, start);
    PCMCIA_IO_WRITE(pDrvCtrl->pcmcia.oliAddr + I595_R13, (start >> 8));

    /* The first word describes the state of reception. */

    READ_R14_R15 (event);

    logMsg ("event= 0x%04x\n", event, 0, 0, 0, 0, 0);
    /* Collect the status of the packet */

    READ_R14_R15 (status);
    logMsg ("status= 0x%04x\n", status, 0, 0, 0, 0, 0);

    /* get next packet pointer */

    READ_R14_R15 (start);
    logMsg ("next= 0x%04x\n", start, 0, 0, 0, 0, 0);

    /* get packet length */

    READ_R14_R15 (length);
    logMsg ("length= 0x%04x\n", length, 0, 0, 0, 0, 0);

    intUnlock(s);
    }

void iOlicomAckInterrupt (int ackValue)
    {
    END_DEVICE* pDrvCtrl = pDbgDrvCtrl;

    PCMCIA_IO_WRITE(pDrvCtrl->pcmcia.oliAddr + I595_R1, ackValue);
    }

void iOlicomClusterNum(void)
    {
    CL_POOL_ID pClId = pDbgDrvCtrl->clPoolId;

    logMsg ("pDrvCtrl = %#x %d %d\n", (int) pDbgDrvCtrl, pClId->clNum,
                                         pClId->clNumFree, 4, 5, 6);
    }

void iOlicomFlagsStatus(void)
    {
    logMsg ("txBdIndexC %#x txBdNext %#x Flags %3x\n", pDbgDrvCtrl->txBdIndexC,
                pDbgDrvCtrl->txBdNext, pDbgDrvCtrl->flags, 0, 0, 0);
    }

void iOlicomTxBdStatus(void)
    {
    TX_BD *     pTxBd;
    ULONG       addrC;
    int         status;
    int         ix;

    for (ix = 0; ix < pDbgDrvCtrl->txBdNum; ix++)
        {
        pTxBd = & pDbgDrvCtrl->txBdBase[ix];
        addrC = (ULONG) pTxBd->dataPointer;

        /* if the data buffer has not been transmitted, don't touch it */

        PCMCIA_IO_WRITE(pDbgDrvCtrl->pcmcia.oliAddr + I595_R12, addrC);
        PCMCIA_IO_WRITE(pDbgDrvCtrl->pcmcia.oliAddr + I595_R13, (addrC >> 8));
        status = PCMCIA_IO_READ(pDbgDrvCtrl->pcmcia.oliAddr + I595_R14);
        status |= (PCMCIA_IO_READ(pDbgDrvCtrl->pcmcia.oliAddr + I595_R15) << 8);

        LOG_MSG ("N %d Addr %#x Status %#x\n", ix, addrC, status, 0, 0, 0);
        }

    }

#endif /* DEBUG */
