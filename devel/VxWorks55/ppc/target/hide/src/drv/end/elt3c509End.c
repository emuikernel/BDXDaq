/* elt3c509End.c - END  network interface driver  for 3COM 3C509*/

/* Copyright 1984-2001 Wind River Systems, Inc. */

#include "copyright_wrs.h"

/*
modification history
--------------------
01j,17jun02,dmh  increased delay loop in elt3c509EepromRead SPR# 78830
01i,28aug00,stv  corrected the handling of EIOCSFLAGS ioctl (SPR# 29423).
01h,02aug00,jkf  Added statistics definitions to this file, SPR#26952.
01g,11jun00,ham  removed reference to etherLib.
01f,27jan00,dat  fixed use of NULL, removed warnings
01e,01dec99,stv  freed mBlk chain before returning ERROR (SPR #28492).
01d,29mar99,dat  SPR 26119, documentation, usage of .bS/.bE
01c,10mar99,sbs  added checking for non-zero value of nRxFrames.
01b,06dec98,ms_  remove comment about "assumptions stated below", which we
                 think
is leftover from the template lance driver.
01a,28sep98,snk	 written by mBedded Innovations Inc.
*/

/*
DESCRIPTION
This module implements the 3COM 3C509 EtherLink III Ethernet network 
interface driver.
This driver is designed to be moderately generic.  Thus, it operates 
unmodified across the range of architectures and targets supported by 
VxWorks.  To achieve this, the driver load routine requires an input 
string consisting of several target-specific values.  The driver also 
requires some external support routines.  These target-specific values 
and the external support routines are described below.  

BOARD LAYOUT
This device is on-board.  No jumpering diagram is necessary.

EXTERNAL INTERFACE
The only external interface is the elt3c509Load() routine, which expects
the <initString> parameter as input.  This parameter passes in a 
colon-delimited string of the format:

<unit>:<port>:<intVector>:<intLevel>:<attachementType>:<nRxFrames>

The elt3c509Load() function uses strtok() to parse the string.

TARGET-SPECIFIC PARAMETERS
.IP <unit>
A convenient holdover from the former model.  This parameter is used only
in the string name for the driver.
.IP <intVector>
Configures the ELT device to generate hardware interrupts
for various events within the device. Thus, it contains
an interrupt handler routine.  The driver calls intConnect() to connect
its interrupt handler to the interrupt vector generated as a result of
the ELT interrupt.
.IP <intLevel>
This parameter is passed to an external support routine, sysEltIntEnable(),
which is described below in "External Support Requirements." This routine 
is called during as part of driver's initialization.  It handles any 
board-specific operations required to allow the servicing of a ELT 
interrupt on targets that use additional interrupt controller devices to 
help organize and service the various interrupt sources.  This parameter 
makes it possible for this driver to avoid all board-specific knowledge of 
such devices.  
.IP <attachmentType>
This parameter is used to select the transceiver hardware attachment. 
This is then used by the elt3c509BoardInit() routine to activate the 
selected attachment. elt3c509BoardInit() is called as a part of the driver's
initialization.
.IP <nRxFrames>
This parameter is used as number of receive frames by the driver.

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
    SYS_OUT_WORD_STRING(pDrvCtrl, reg, pData, len)
    SYS_IN_WORD_STRING(pDrvCtrl, reg, pData, len)
    
    sysEltIntEnable(pDrvCtrl->intLevel) 
    sysEltIntDisable(pDrvCtrl->intLevel) 
.CE

There are default values in the source code for these macros.  They presume
IO-mapped accesses to the device registers and the normal intConnect(),
and intEnable() BSP functions.  The first argument to each is the device
controller structure. Thus, each has access back to all the device-specific
information.  Having the pointer in the macro facilitates the addition 
of new features to this driver.

The macros SYS_INT_CONNECT, SYS_INT_DISCONNECT, and SYS_INT_ENABLE allow
the driver to be customized for BSPs that use special versions of these
routines.

The macro SYS_INT_CONNECT is used to connect the interrupt handler to
the appropriate vector.  By default it is the routine intConnect().

The macro SYS_INT_DISCONNECT is used to disconnect the interrupt handler prior
to unloading the module.  By default this is a dummy routine that
returns OK.

The macro SYS_INT_ENABLE is used to enable the interrupt level for the
end device.  It is called once during initialization.  It calls an
external board level routine sysEltIntEnable(). 

The macro SYS_INT_DISABLE is used to disable the interrupt level for the
end device.  It is called during stop.  It calls an
external board level routine sysEltIntDisable(). 

SYSTEM RESOURCE USAGE
When implemented, this driver requires the following system resources:

    - one interrupt vector
    - 9720 bytes of text
    - 88 bytes in the initialized data section (data)
    - 0 bytes of bss

The driver requires 1520 bytes of preallocation for Transmit Buffer and 
1520*nRxFrames of receive buffers. The default value of nRxFrames is 64
therefore total pre-allocation is (64 + 1)*1520.

TUNING HINTS
nRxFrames parameter can be used for tuning no of receive frames to be used
for handling packet receive. More no. of these could help receiving more 
loaning in case of massive reception.

INTERNAL
This driver is END compliant. It supports Multicasting and Poll mode Send and
Receive capabilities. The initialization calls elt3c509BoardInit(), which 
initializes the ELT3C509 chip, and elt3c509MemInit() routine which initializes
memory allocation for Tx and Rx functionality. One mblk chain is allocated for 
storing Tx frame and nRxFrame(64) mblk chainss are allocated for Rx frames.
When a receive interrupt is received, netJobAdd() is called to schedule
elt3c509HandlePktsRcv() routine for handling reception of the packets. This 
routine empties the FIFO in a mblk chain and loops for more packets.
The cluster is then passed up to the Receive routine of the upper layer.

To transmit a packet, contents of mblk are copied into the TxFrame which is
used to fill the FIFO of the chip. Transmit complete interrupt is used to 
check for status of the transmission.

When Polling mode is enabled, both for Tx and Rx together, elt3c509PollRcv()
routine gets the packet out of the FIFO, copies into the mblk. For Tx in 
Poll mode, elt3c509PollSend(0 routine is called by the upper layer, the data
is copied into the TxFrame from the mblk and sent out. The interrupt is 
disabled for transmit complete.

Multicast addresses are handled by setting/resetting the RX_F_MULTICAST bit
in the chip. When an address is added, the bit is set. When an address is 
removed and the address list is empty, the bit is reset.

INCLUDES:
end.h endLib.h etherMultiLib.h elt3c509End.h

SEE ALSO: muxLib, endLib
.I "Writing an Enhanced Network Driver"
*/

/* includes */

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
#include "etherMultiLib.h"		/* multicast stuff. */
#include "end.h"			/* Common END structures. */
#include "endLib.h"
#include "lstLib.h"			/* Needed to maintain protocol list. */
#include "netBufLib.h"
#include "muxLib.h"
#include "m2Lib.h"
#include "drv/end/elt3c509End.h"

/* statistic macro's */

#define collisions      elt3c509Stats[0]
#define crcs            elt3c509Stats[1]
#define aligns          elt3c509Stats[2]
#define rxnobuffers     elt3c509Stats[3]
#define rxoverruns      elt3c509Stats[4]
#define disabled        elt3c509Stats[5]
#define deferring       elt3c509Stats[6]
#define txunderruns     elt3c509Stats[7]
#define aborts          elt3c509Stats[8]
#define outofwindow     elt3c509Stats[9]
#define heartbeats      elt3c509Stats[10]
#define badPacket       elt3c509Stats[11]
#define shortPacket     elt3c509Stats[12]
#define txnoerror       elt3c509Stats[13]
#define rxnoerror       elt3c509Stats[14]
#define txerror         elt3c509Stats[15]
#define rxerror         elt3c509Stats[16]
#define overwrite       elt3c509Stats[17]
#define wrapped         elt3c509Stats[18]
#define interrupts      elt3c509Stats[19]
#define elt3c509_reset  elt3c509Stats[20]
#define strayint        elt3c509Stats[21]
#define multcollisions  elt3c509Stats[22]
#define latecollisions  elt3c509Stats[23]
#define nocarriers      elt3c509Stats[24]
#define jabbers         elt3c509Stats[25]
#define txoverruns      elt3c509Stats[26]
#define rxunderruns     elt3c509Stats[27]
#define rxearly         elt3c509Stats[28]
#ifdef ELT_TIMING       
#define timerUpdates    elt3c509Stats[29]
#define timerOverflow   elt3c509Stats[30]
#define timerInvalid    elt3c509Stats[31]
#define maxRxLatency    elt3c509Stats[32]
#define minRxLatency    elt3c509Stats[33]
#define maxIntLatency   elt3c509Stats[34]
#define taskQRxOuts     elt3c509Stats[35]
#define maxRxTaskQ      elt3c509Stats[36]  
#define taskQTxOuts     elt3c509Stats[37]  
#define maxTxTaskQ      elt3c509Stats[38]  
#endif /* ELT_TIMING */ 

/* Cache macros */

#define ELT3C509_CACHE_INVALIDATE(address, len) \
        CACHE_DRV_INVALIDATE (&pDrvCtrl->cacheFuncs, (address), (len))

#define ELT3C509_CACHE_PHYS_TO_VIRT(address) \
        CACHE_DRV_PHYS_TO_VIRT (&pDrvCtrl->cacheFuncs, (address))

#define ELT3C509_CACHE_VIRT_TO_PHYS(address) \
        CACHE_DRV_VIRT_TO_PHYS (&pDrvCtrl->cacheFuncs, (address))

/*
 * Default macro definitions for BSP interface.
 * These macros can be redefined in a wrapper file, to generate
 * a new module with an optimized interface.
 */

/* Macro to connect interrupt handler to vector */

#ifndef SYS_INT_CONNECT
#   define SYS_INT_CONNECT(pDrvCtrl,rtn,arg,pResult) \
	{ \
	*pResult = intConnect ((VOIDFUNCPTR *)INUM_TO_IVEC (pDrvCtrl->ivec), \
			     rtn, (int)arg); \
	}
#endif

/* Macro to disconnect interrupt handler from vector */

#ifndef SYS_INT_DISCONNECT
#   define SYS_INT_DISCONNECT(pDrvCtrl,rtn,arg,pResult) \
	{ \
	*pResult = OK;  \
	}
#endif

/* Macro to enable the appropriate interrupt level */

#ifndef SYS_INT_ENABLE
#   define SYS_INT_ENABLE(pDrvCtrl) \
	{ \
	IMPORT void sysEltIntEnable(); \
	sysEltIntEnable (pDrvCtrl->intLevel); \
	}
#endif

/* Macro to disable the appropriate interrupt level */

#ifndef SYS_INT_DISABLE
#   define SYS_INT_DISABLE(pDrvCtrl) \
	{ \
	IMPORT void sysEltIntDisable (); \
	sysEltIntDisable (pDrvCtrl->intLevel); \
	}
#endif

/*
 * Macros to do a byte access to the chip. Uses
 * sysOutByte() and sysInByte().
 */

#ifndef SYS_OUT_BYTE
#   define SYS_OUT_BYTE(pDrvCtrl,addr,value) \
	{ \
        sysOutByte((addr), (value)); \
	}
#endif

#ifndef SYS_IN_BYTE
#   define SYS_IN_BYTE(pDrvCtrl,addr,data) \
        { \
        (data) = sysInByte((addr)); \
        }
#endif 
/*
 * Macros to do a word access to the chip. Uses
 * sysOutWord() and sysInWord().
 */

#ifndef SYS_OUT_WORD
#   define SYS_OUT_WORD(pDrvCtrl,addr,value) \
	{ \
        sysOutWord((addr), (value)); \
	}
#endif

#ifndef SYS_IN_WORD
#   define SYS_IN_WORD(pDrvCtrl,addr,data) \
        { \
        (data) = sysInWord((addr)); \
        }
#endif 

#ifndef SYS_IN_WORD_STRING
#define SYS_IN_WORD_STRING(pDrvCtrl, reg, pData, len) \
	{ \
        sysInWordString ((reg), (pData),(len)); \
	}
#endif

#ifndef SYS_OUT_WORD_STRING
#define SYS_OUT_WORD_STRING(pDrvCtrl, reg, pData, len) \
	{ \
        sysOutWordString ((reg),(pData), (len)); \
	}
#endif

	
	
/* A shortcut for getting the hardware address from the MIB II stuff. */

#define END_HADDR(pEnd)	\
		((pEnd)->mib2Tbl.ifPhysAddress.phyAddress)

#define END_HADDR_LEN(pEnd) \
		((pEnd)->mib2Tbl.ifPhysAddress.addrLength)


#define VOID_TO_DRVCTRL(pVoid,pDrvCtrl) ((pDrvCtrl)=(ELT3C509_DEVICE *)(pVoid))

/* externs */
IMPORT int endMultiLstCnt (END_OBJ *);


M_CL_CONFIG elt3c509MclConfig = 	/* network mbuf configuration table */
    {
    /* 
    no. mBlks		no. clBlks	memArea		memSize
    -----------		----------	-------		-------
    */
    0, 			0, 		NULL, 		0
    };

CL_DESC elt3c509ClDescTbl [] = 	/* network cluster pool configuration table */
    {
    /* 
    clusterSize		num		memArea		memSize
    -----------		----		-------		-------
    */
    {ELT3C509_BUFSIZ,	0,		NULL,		0}
    }; 

int elt3c509ClDescTblNumEnt = (NELEMENTS(elt3c509ClDescTbl));

/* DEBUG MACROS */

#undef DRV_DEBUG
#ifdef DRV_DEBUG

int 	elt3c509Debug = 0x0f; 	/* Turn it off initially. */
NET_POOL * pEltPool;

#define ENDLOGMSG(x) \
	if (elt3c509Debug) \
	    { \
	    logMsg x; \
	    }
#else
#define ENDLOGMSG(x)
#endif /* DRV_DEBUG */

/* forward static functions */

      STATUS	elt3c509Parse (ELT3C509_DEVICE *, char *);
LOCAL STATUS	elt3c509Start (void * pEnd);
LOCAL STATUS	elt3c509Stop (void * pEnd);
LOCAL STATUS	elt3c509Unload (void * pEnd);
LOCAL int	elt3c509Ioctl (void * pEnd, int cmd, caddr_t data);
LOCAL STATUS	elt3c509Send (void * pEnd, M_BLK_ID pBuf);
LOCAL STATUS	elt3c509MCastAdd (void * pEnd, char * pAddress);
LOCAL STATUS	elt3c509MCastDel (void * pEnd, char * pAddress);
LOCAL STATUS	elt3c509MCastGet (void * pEnd, MULTI_TABLE * pTable);
LOCAL STATUS	elt3c509PollSend (void * pEnd, M_BLK_ID pBuf);
LOCAL STATUS	elt3c509PollRcv (void * pEnd, M_BLK_ID pBuf);
LOCAL void	elt3c509Reset (ELT3C509_DEVICE * pDrvCtrl);
LOCAL void	elt3c509Int (ELT3C509_DEVICE * pDrvCtrl);
LOCAL void	elt3c509HandlePktsRcv (ELT3C509_DEVICE * pDrvCtrl);
LOCAL STATUS    elt3c509BoardInit (ELT3C509_DEVICE * pDrvCtrl);
LOCAL void	elt3c509Config (ELT3C509_DEVICE * pDrvCtrl);
LOCAL STATUS	elt3c509PollStart (ELT3C509_DEVICE * pDrvCtrl);
LOCAL STATUS	elt3c509PollStop (ELT3C509_DEVICE * pDrvCtrl);
LOCAL STATUS	elt3c509MemInit	(ELT3C509_DEVICE * pDrvCtrl);
LOCAL void	elt3c509IntDisable (ELT3C509_DEVICE * pDrvCtrl);
LOCAL void	elt3c509IntEnable (ELT3C509_DEVICE * pDrvCtrl);
LOCAL void	elt3c509RxStart (ELT3C509_DEVICE * pDrvCtrl);
LOCAL void	elt3c509TxStart (ELT3C509_DEVICE * pDrvCtrl);
LOCAL STATUS	elt3c509Activate (ELT3C509_DEVICE * pDrvCtrl);
LOCAL void	elt3c509IdCommand (int idPort);
LOCAL UINT16	elt3c509IdEepromRead (int address);
LOCAL int	elt3c509EepromRead (int port, int offset);
LOCAL void 	elt3c509IntMaskSet (ELT3C509_DEVICE * pDrvCtrl, int maskBits);
LOCAL void 	elt3c509StatFlush (ELT3C509_DEVICE * pDrvCtrl);

/* LOCALS */

/*
 * Declare our function table.  This is static across all driver
 * instances.
 */

LOCAL NET_FUNCS elt3c509FuncTable =
    {
    (FUNCPTR)elt3c509Start,     /* Function to start the device. */
    (FUNCPTR)elt3c509Stop,	/* Function to stop the device. */
    (FUNCPTR)elt3c509Unload,	/* Unloading function for the driver. */
    (FUNCPTR)elt3c509Ioctl,	/* Ioctl function for the driver. */
    (FUNCPTR)elt3c509Send,	/* Send function for the driver. */
    (FUNCPTR)elt3c509MCastAdd,	/* Multicast address add function for the */
				/* driver. */
    (FUNCPTR)elt3c509MCastDel,	/* Multicast address delete function for */
				/* the driver. */
    (FUNCPTR)elt3c509MCastGet,	/* Multicast table retrieve function for */
				/* the driver. */
    (FUNCPTR)elt3c509PollSend,	/* Polling send function for the driver. */
    (FUNCPTR)elt3c509PollRcv,	/* Polling receive function for the driver. */
    endEtherAddressForm,        /* Put address info into a packet.  */
    endEtherPacketDataGet,      /* Get a pointer to packet data. */
    endEtherPacketAddrGet       /* Get packet addresses. */
    };

/*******************************************************************************
*
* elt3c509Load - initialize the driver and device
*
* This routine initializes the driver and the device to the operational state.
* All of the device-specific parameters are passed in <initString>, which
* expects a string of the following format:
*
* <unit>:<port>:<intVector>:<intLevel>:<attachementType>:<noRxFrames>
*
* This routine can be called in two modes. If it is called with an empty but
* allocated string, it places the name of this device (that is, "elt") into 
* the <initString> and returns 0.
*
* If the string is allocated and not empty, the routine attempts to load
* the driver using the values specified in the string.
*
* RETURNS: An END object pointer, or NULL on error, or 0 and the name of the
* device if the <initString> was NULL.
*/

END_OBJ * elt3c509Load
    (
    char * initString		/* String to be parsed by the driver. */
    )
    {
    ELT3C509_DEVICE * 	pDrvCtrl;
    int   		productID;
    short * 		pAddr;

    ENDLOGMSG (("Loading elt3c...\n", 1, 2, 3, 4, 5, 6));

    if (initString == NULL)
        return (NULL);
    
    if (initString [0] == 0)
        {
        bcopy ((char *)ELT3C509_DEV_NAME, initString, ELT3C509_DEV_NAME_LEN);
        return (0);
        }

    /* allocate the device structure */

    pDrvCtrl = (ELT3C509_DEVICE *)calloc (sizeof (ELT3C509_DEVICE), 1);
    if (pDrvCtrl == NULL)
	goto errorExit;

    /* parse the init string, filling in the device structure */

    if (elt3c509Parse (pDrvCtrl, initString) == ERROR)
	goto errorExit;

    /* initialize the END and MIB2 parts of the structure */

    /*
     * The M2 element must come from m2Lib.h 
     * 
     */
    
    if (END_OBJ_INIT (&pDrvCtrl->endObj, (DEV_OBJ *)pDrvCtrl,
                      ELT3C509_DEV_NAME,
                      pDrvCtrl->unit, &elt3c509FuncTable,
                      "3COM 3c509 EtherLink III Endhanced Network Driver.")
        == ERROR)
        goto errorExit;

    productID = elt3c509EepromRead (pDrvCtrl->port, EE_A_PRODUCT_ID);
    if (productID == ELT_PRODUCTID_3C589)
	{
        pAddr = (short *)pDrvCtrl->enetAddr;
	*pAddr++ = htons (elt3c509EepromRead
                          (pDrvCtrl->port, EE_A_OEM_NODE_0));
	*pAddr++ = htons (elt3c509EepromRead
                          (pDrvCtrl->port, EE_A_OEM_NODE_1));
	*pAddr++ = htons (elt3c509EepromRead
                      (pDrvCtrl->port, EE_A_OEM_NODE_2));

	SYS_OUT_WORD (pDrvCtrl, pDrvCtrl->port + ELT3C509_COMMAND,
                    SELECT_WINDOW | WIN_CONFIG);
	SYS_OUT_WORD (pDrvCtrl, pDrvCtrl->port + ADDRESS_CONFIG,
                    elt3c509EepromRead (pDrvCtrl->port, EE_A_ADDRESS));
	SYS_OUT_WORD (pDrvCtrl, pDrvCtrl->port + RESOURCE_CONFIG,
                    elt3c509EepromRead (pDrvCtrl->port,EE_A_RESOURCE));
	}
    else
        {
        if (elt3c509Activate (pDrvCtrl) != OK)
            return (NULL);
        }

    if (END_MIB_INIT (&pDrvCtrl->endObj, M2_ifType_ethernet_csmacd,
                      &pDrvCtrl->enetAddr[0], 6, ETHERMTU, ELT3C509_SPEED)
         == ERROR)
	goto errorExit;

     
    /* Initialize the Board here */
    
    if (elt3c509BoardInit (pDrvCtrl) == ERROR)
	goto errorExit;

    /* Perform memory allocation/distribution */

    if (elt3c509MemInit (pDrvCtrl) == ERROR)
	goto errorExit;

    
    /* set the flags to indicate readiness */

    END_OBJ_READY (&pDrvCtrl->endObj, IFF_NOTRAILERS | IFF_BROADCAST
                   | IFF_MULTICAST | IFF_UP | IFF_RUNNING);
    
    pDrvCtrl->rxFilter = RX_F_NORMAL;

    /* configure the device */

    elt3c509Config (pDrvCtrl);

    ENDLOGMSG (("Done loading end...", 1, 2, 3, 4, 5, 6));

    return (&pDrvCtrl->endObj);

errorExit:
    if (pDrvCtrl != NULL)
	free ((char *)pDrvCtrl);

    return NULL;
    }

/*******************************************************************************
*
* elt3c509Parse - parse the init string
*
* Parse the input string.  Fill in values in the driver control structure.
*
* The initialization string format is:
* .CS
*     <unit>:<port>:<intVector>:<intLevel>:<attachementType>:<noRxFrames>
* .CE
* .IP <unit>
* Device unit number, a small integer.
* .IP <port>
* base I/O address
* .IP <intVector>
* Interrupt vector number (used with sysIntConnect)
* .IP <intLevel>
* Interrupt level
* .IP <attachmentType>
* type of Ethernet connector
* .IP <nRxFrames>
* no. of Rx Frames in integer format
* .LP
*
* RETURNS: OK or ERROR for invalid arguments.
*/

STATUS elt3c509Parse
    (
    ELT3C509_DEVICE * 	pDrvCtrl,	/* device pointer */
    char * 		initString	/* initialization info string */
    )
    {
    char *	tok;
    char *	holder = NULL;
    
    /* unit number. */

    tok = strtok_r (initString, ":", &holder);
    if (tok == NULL)
	return ERROR;
    pDrvCtrl->unit = atoi (tok);

    /* port number */

    tok = strtok_r (NULL, ":", &holder);
    if (tok == NULL)
	return ERROR;
    pDrvCtrl->port = strtoul (tok, NULL, 16);
    
    /* interrupt vector. */

    tok = strtok_r (NULL, ":", &holder);
    if (tok == NULL)
	return ERROR;
    pDrvCtrl->ivec = strtoul (tok, NULL, 16);

    /* interrupt level. */

    tok = strtok_r (NULL, ":", &holder);
    if (tok == NULL)
	return ERROR;
    pDrvCtrl->intLevel = strtoul (tok, NULL, 16);

    /* attachment Type */

    tok = strtok_r (NULL, ":", &holder);
    if (tok == NULL)
	return ERROR;
    pDrvCtrl->attachment = atoi (tok);

    /* no. of Rx Frames */
    tok = strtok_r (NULL, ":", &holder);
    if (tok == NULL)
        return ERROR;
    pDrvCtrl->nRxFrames = atoi (tok);
    
    ENDLOGMSG (("Processed all arugments\n", 1, 2, 3, 4, 5, 6));

    return OK;
    }

/*******************************************************************************
*
* elt3c509BoardInit - Initialise the device
*
* This function prepares the board for operation
*
* RETURNS: OK or ERROR
*
*/

LOCAL STATUS elt3c509BoardInit
    (
    ELT3C509_DEVICE * pDrvCtrl	/* device to be initialized */
    )
    {
    int 	port;
    int 	index;
    UINT16 	transceiver;
    UINT16 	mediaStatus;

    port = pDrvCtrl->port;

    END_FLAGS_CLR (&pDrvCtrl->endObj, IFF_UP | IFF_RUNNING);

    elt3c509IntDisable (pDrvCtrl);   /* make it OK to change register window */

    SYS_OUT_WORD (pDrvCtrl, port + ELT3C509_COMMAND,
                  (SELECT_WINDOW | WIN_ADDRESS));

    for (index = 0; index < EA_SIZE; index++)
        SYS_OUT_BYTE (pDrvCtrl, port + index, pDrvCtrl->enetAddr [index]);

    /* Select the transceiver hardware (attachment) then do whatever is
     * necessary to activate the selected attachment.
     * A truly complete implementation would probably check to see
     * if the selected attachment were present.
     */

    SYS_OUT_WORD (pDrvCtrl, port + ELT3C509_COMMAND,
                  (SELECT_WINDOW | WIN_CONFIG));
    SYS_IN_WORD (pDrvCtrl, port + ADDRESS_CONFIG, transceiver);
    if (pDrvCtrl->attachment == ATTACHMENT_DEFAULT)
        {
        switch (transceiver & AC_XCVR_MASK)
            {
            case AC_XCVR_TPE:
                pDrvCtrl->attachment = ATTACHMENT_RJ45;
                break;
            case AC_XCVR_BNC:
                pDrvCtrl->attachment = ATTACHMENT_BNC;
                break;
            case AC_XCVR_AUI:
                pDrvCtrl->attachment = ATTACHMENT_AUI;
                break;
            default:    /* there's only one other value; it's "reserved" */
                pDrvCtrl->attachment = ATTACHMENT_AUI;  /* good enough */
                break;
            }
        }

    /* Now set the selected attachment type, even if it was already selected */

    transceiver &= ~AC_XCVR_MASK;
    switch (pDrvCtrl->attachment)
        {
        case ATTACHMENT_RJ45:
            SYS_OUT_WORD (pDrvCtrl, port + ADDRESS_CONFIG,
                          (transceiver | AC_XCVR_TPE));
            SYS_OUT_WORD (pDrvCtrl, port + ELT3C509_COMMAND,
                          (SELECT_WINDOW | WIN_DIAGNOSTIC));
            SYS_IN_BYTE (pDrvCtrl, port + MEDIA_STATUS, mediaStatus);
            SYS_OUT_BYTE (pDrvCtrl, port + MEDIA_STATUS,
                          mediaStatus |
                          MT_S_JABBER_ENABLE |
                          MT_S_LINK_BEAT_ENABLE);
            break;
        case ATTACHMENT_BNC:
            SYS_OUT_WORD (pDrvCtrl, port + ADDRESS_CONFIG,
                          transceiver | AC_XCVR_BNC);
            SYS_OUT_WORD (pDrvCtrl, port + ELT3C509_COMMAND, START_COAX);
            break;
            
        case ATTACHMENT_AUI:
        default:
            SYS_OUT_WORD (pDrvCtrl, port + ADDRESS_CONFIG,
                          transceiver | AC_XCVR_AUI);
            break;
        }

    /*
     * Define the set of status bits that could cause interrupts.
     * There is no interrupt cause indicator separate from the board status.
     * To keep the ISR from seeing status conditions we didn't want to be
     * interrrupted for, we must mask the STATUS off, not the interrupt.
     * This prevents the condition from interrupting and also prevents
     * the ISR from seeing the condition even if it is true.
     * The interrupt mask is set only once to the set of all conditions
     * we might want to be interrupted by; the status mask is set and
     * cleared according to which conditions we want at any particular
     * time.  The intMask field in the control structure
     * is named for its effect; it is really used in the status mask
     * command (3Com calls it the read zero mask).
     */

    /* enable the status bits we currently want to cause interrupts */

    elt3c509IntMaskSet (pDrvCtrl, ADAPTER_FAILURE | TX_COMPLETE |
                        RX_COMPLETE | RX_EARLY | UPDATE_STATS);

    /* enable the collection of statistics */

    SYS_OUT_WORD (pDrvCtrl, port + ELT3C509_COMMAND, STATS_ENABLE);

    /* enable the hardware to generate interrupt requests */

    SYS_OUT_WORD (pDrvCtrl, port + ELT3C509_COMMAND,
                  SELECT_WINDOW | WIN_CONFIG);
    SYS_OUT_WORD (pDrvCtrl, port + CONFIG_CONTROL, CC_ENABLE);

    SYS_OUT_WORD (pDrvCtrl, port + ELT3C509_COMMAND,
                  SELECT_WINDOW | WIN_OPERATING);

    elt3c509IntEnable (pDrvCtrl);

    elt3c509RxStart (pDrvCtrl);
    elt3c509TxStart (pDrvCtrl);
    return (OK);
    }

/*******************************************************************************
*
* elt3c509MemInit - initialize memory for the chip
*
* This routine is highly specific to the device.  
*
* RETURNS: OK or ERROR.
*/

LOCAL STATUS elt3c509MemInit
    (
    ELT3C509_DEVICE * 	pDrvCtrl	/* device to be initialized */
    )
    {
    if ((pDrvCtrl->endObj.pNetPool = malloc (sizeof(NET_POOL))) == NULL)
        return (ERROR);

#ifdef DRV_DEBUG    
    pEltPool = pDrvCtrl->endObj.pNetPool;
#endif    

    /* check if the value of nRxFrames is valid */

    pDrvCtrl->nRxFrames = pDrvCtrl->nRxFrames ? 
                          pDrvCtrl->nRxFrames : DEF_NUM_RX_FRAMES; 

    /* assign static cluster config/desc structures to unit specific structures
     * This is done to enable multiple units to operate.
     */

    pDrvCtrl->endClDesc = elt3c509ClDescTbl [0];
    pDrvCtrl->endClConfig = elt3c509MclConfig;
    
    pDrvCtrl->endClDesc.clNum = pDrvCtrl->nRxFrames  + 1;
    pDrvCtrl->endClConfig.mBlkNum  = (2 * pDrvCtrl->endClDesc.clNum);
    pDrvCtrl->endClConfig.clBlkNum = pDrvCtrl->endClDesc.clNum;

    /* Calculate the total memory for all the M-Blks and CL-Blks. */

    pDrvCtrl->endClConfig.memSize = (pDrvCtrl->endClConfig.mBlkNum *
                                     (MSIZE + sizeof (long))) +
        	                    (pDrvCtrl->endClConfig.clBlkNum *
                                     (CL_BLK_SZ + sizeof(long)));

    if ((pDrvCtrl->endClConfig.memArea =
         (char *) memalign (sizeof(long), pDrvCtrl->endClConfig.memSize))
        == NULL)
        {
        return (ERROR);
        }
    
    /* Calculate the memory size of all the clusters. */

    pDrvCtrl->endClDesc.memSize = ((pDrvCtrl->endClDesc.clNum *
                                    (ELT3C509_BUFSIZ + 8)) +
                                   sizeof(int));

    /* Allocate the memory for the clusters from cache safe memory. */

    pDrvCtrl->endClDesc.memArea =
        (char *) memalign (sizeof(long), pDrvCtrl->endClDesc.memSize);

    if (pDrvCtrl->endClDesc.memArea == NULL)
        {
        ENDLOGMSG(("system memory unavailable\n", 1,2,3,4,5,6));
        return (ERROR);
        }

    pDrvCtrl->cacheFuncs = cacheNullFuncs;

    /* Initialize the memory pool. */

    if (netPoolInit (pDrvCtrl->endObj.pNetPool, &pDrvCtrl->endClConfig,
                     &pDrvCtrl->endClDesc, elt3c509ClDescTblNumEnt, NULL)
        == ERROR)
        {
        ENDLOGMSG (("Could not init buffering\n",1,2,3,4,5,6));
        return (ERROR);
        }

    /* Get Cluster POOL ID here */

    pDrvCtrl->pClPoolId = clPoolIdGet (pDrvCtrl->endObj.pNetPool,
                                       ELT3C509_BUFSIZ, FALSE);

    /* allocate one cluster for the transmit frame */

    pDrvCtrl->pTxCluster = (char *)netClusterGet (pDrvCtrl->endObj.pNetPool,
                                                  pDrvCtrl->pClPoolId);

    ENDLOGMSG (("Memory setup complete\n",1,2,3,4,5,6));

    return OK;
    }

/*******************************************************************************
*
* elt3c509Start - start the device
*
* This function calls BSP functions to connect interrupts and start the
* device running in interrupt mode.
*
* RETURNS: OK or ERROR
*
*/

LOCAL STATUS elt3c509Start
    (
    void * pEnd	/* device to be started */
    )
    {
    STATUS 		result;
    ELT3C509_DEVICE * 	pDrvCtrl;

    VOID_TO_DRVCTRL (pEnd, pDrvCtrl);
    
    SYS_INT_CONNECT (pDrvCtrl, elt3c509Int, (int)pDrvCtrl, &result);
    if (result == ERROR)
	return ERROR;

    ENDLOGMSG (("Interrupt connected.\n", 1, 2, 3, 4, 5, 6));

    SYS_INT_ENABLE (pDrvCtrl);

    ENDLOGMSG (("interrupt enabled.\n", 1, 2, 3, 4, 5, 6));

    return (OK);
    }

/*******************************************************************************
*
* elt3c509Stop - stop the device
*
* This function calls BSP functions to disconnect interrupts and stop
* the device from operating in interrupt mode.
*
* RETURNS: OK or ERROR.
*/

LOCAL STATUS elt3c509Stop
    (
    void * pEnd	/* device to be stopped */
    )
    {
    STATUS 		result = OK;
    ELT3C509_DEVICE * 	pDrvCtrl;

    VOID_TO_DRVCTRL (pEnd, pDrvCtrl);

    elt3c509Reset(pDrvCtrl);

    SYS_INT_DISCONNECT (pDrvCtrl, elt3c509Int, (int)pDrvCtrl, &result);

    if (result == ERROR)
	{
	ENDLOGMSG (("Could not diconnect interrupt!\n", 1, 2, 3, 4, 5, 6));
	}

    SYS_INT_DISABLE (pDrvCtrl);

    END_FLAGS_CLR (&pDrvCtrl->endObj, IFF_UP | IFF_RUNNING);

    return (result);
    }


/******************************************************************************
*
* elt3c509Unload - unload a driver from the system
*
* This function first brings down the device, and then frees any
* stuff that was allocated by the driver in the load function.
*
* RETURNS: OK or ERROR.
*/

LOCAL STATUS elt3c509Unload
    (
    void * pEnd		/* device to be unloaded */
    )
    {
    ELT3C509_DEVICE * pDrvCtrl;

    VOID_TO_DRVCTRL (pEnd, pDrvCtrl);
    
    END_OBJECT_UNLOAD (&pDrvCtrl->endObj);

    if (pDrvCtrl->endClDesc.memArea != NULL)
        free (pDrvCtrl->endClDesc.memArea);

    if (pDrvCtrl->endClConfig.memArea != NULL)
        free ( pDrvCtrl->endClConfig.memArea);
    
    if (pDrvCtrl->endObj.pNetPool != NULL)
        free (pDrvCtrl->endObj.pNetPool);
    
    return (OK);
    }

/*******************************************************************************
*
* elt3c509Ioctl - the driver I/O control routine
*
* Process an ioctl request.
*
* RETURNS: A command specific response, OK or ERROR or EINVAL.
*/

LOCAL int elt3c509Ioctl
    (
    void * 	pEnd,           /* device  ptr*/
    int 	cmd,		/* ioctl command code */
    caddr_t 	data		/* command argument */
    )
    {
    int 		error = 0;
    long 		value;
    ELT3C509_DEVICE * 	pDrvCtrl;

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
	    elt3c509Config (pDrvCtrl);
            break;

        case EIOCGFLAGS:
	    *(int *)data = END_FLAGS_GET(&pDrvCtrl->endObj);
            break;

	case EIOCPOLLSTART:
	    error = elt3c509PollStart(pDrvCtrl);
	    break;
 
	case EIOCPOLLSTOP:
	    error = elt3c509PollStop(pDrvCtrl);
 	    break;

        case EIOCGMIB2:
            if (data == NULL)
                return (EINVAL);
            bcopy((char *)&pDrvCtrl->endObj.mib2Tbl, (char *)data,
                  sizeof(pDrvCtrl->endObj.mib2Tbl));
            break;

        case EIOCGFBUF:
            return (EINVAL); /* no scatter gather supported for the chip */
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

        default:
            error = EINVAL;
        }

    return (error);
    }

/*******************************************************************************
*
* elt3c509Send - the driver send routine
*
* This routine takes a M_BLK_ID sends off the data in the M_BLK_ID.
* The buffer must already have the addressing information properly installed
* in it.  This is done by a higher layer.
*
* RETURNS: OK or END_ERR_BLOCK or ERROR.
*/

LOCAL STATUS elt3c509Send
    (
    void  * 	pEnd,	/* device ptr */
    M_BLK_ID 	pMblk	/* data to send */
    )
    {
    int			len;
    int *		pLenMask;
    char *		pBuf;
    int			intLevel;
    ELT3C509_DEVICE * 	pDrvCtrl;
    UINT16		tempData;

    VOID_TO_DRVCTRL (pEnd, pDrvCtrl);

    /* change this drv flag set */

    if (pDrvCtrl->flags & ELT3C_POLLING)
        { 
        netMblkClChainFree (pMblk); /* free the given mBlk chain */
        errno = EINVAL;
        return (ERROR);
        }
        
    /* check if transmitter ready */

    SYS_IN_WORD (pDrvCtrl, pDrvCtrl->port + TX_FREE_BYTES, tempData);
    if (tempData >= TX_IDLE_COUNT)
        {
        pBuf = pDrvCtrl->pTxCluster; 	/* pointer to the transmit cluster */

        pLenMask = (UINT32 *)pBuf;
        
        /* allow space for the preamble the trasmit buffer is 1520 */

        pBuf += sizeof(UINT32);

        len = netMblkToBufCopy (pMblk, pBuf, NULL);

        /* free the mblk chain */
        
        netMblkClChainFree (pMblk);

        len = max(len, ETHERSMALL);		/* set the packet size */

        *pLenMask = len | TX_F_INTERRUPT;	/* set the preamble */

        len = (len + TX_F_PREAMBLE_SIZE + 3) & TX_F_DWORD_MASK;

        /* place a transmit request */

        SYS_OUT_WORD_STRING (pDrvCtrl, pDrvCtrl->port + DATA_REGISTER, 
                             (short *)pLenMask, len / 2);
        }
    else
        {
        intLevel = intLock();
	pDrvCtrl->txBlocked = TRUE;
        intUnlock (intLevel);
        ENDLOGMSG(("Error in Send, not enough TxFreeBytes\n",1,2,3,4,5,6));
        return (END_ERR_BLOCK);
        }
    
    /* Bump the statistic counter. */

    END_ERR_ADD (&pDrvCtrl->endObj, MIB2_OUT_UCAST, +1);

    ENDLOGMSG (("Send complete\n",1,2,3,4,5,6));
    return (OK);
    }

/*****************************************************************************
*
* elt3c509MCastAdd - add a multicast address for the device
*
* This routine adds a multicast address to whatever the driver
* is already listening for.  It then resets the address filter.
*
* RETURNS: OK or ERROR.
*/

LOCAL STATUS elt3c509MCastAdd
    (
    void  * 	pEnd,	        /* device pointer */
    char * 	pAddress	/* new address to add */
    )
    {
    int 		error;
    ELT3C509_DEVICE * 	pDrvCtrl;

    VOID_TO_DRVCTRL (pEnd, pDrvCtrl);

    if ((error = etherMultiAdd (&pDrvCtrl->endObj.multiList,
		pAddress)) == ENETRESET)
	elt3c509Config (pDrvCtrl);

    return (OK);
    }

/*****************************************************************************
*
* elt3c509MCastDel - delete a multicast address for the device
*
* This routine removes a multicast address from whatever the driver
* is listening for.  It then resets the address filter.
*
* RETURNS: OK or ERROR.
*/

LOCAL STATUS elt3c509MCastDel
    (
    void  * 	pEnd,	        /* device pointer */
    char * 	pAddress	/* address to be deleted */
    )
    {
    int 		error;
    ELT3C509_DEVICE * 	pDrvCtrl;

    VOID_TO_DRVCTRL (pEnd, pDrvCtrl);
    if ((error = etherMultiDel (&pDrvCtrl->endObj.multiList,
	     (char *)pAddress)) == ENETRESET)
	elt3c509Config (pDrvCtrl);

    return (OK);
    }

/*****************************************************************************
*
* elt3c509MCastGet - get the multicast address list for the device
*
* This routine gets the multicast list of whatever the driver
* is already listening for.
*
* RETURNS: OK or ERROR.
*/

LOCAL STATUS elt3c509MCastGet
    (
    void  * 		pEnd,	        /* device pointer */
    MULTI_TABLE * 	pTable		/* address table to be filled in */
    )
    {
    ELT3C509_DEVICE * pDrvCtrl;

    VOID_TO_DRVCTRL (pEnd, pDrvCtrl);
    return (etherMultiGet (&pDrvCtrl->endObj.multiList, pTable));
    }

/*******************************************************************************
*
* elt3c509Int - handle controller interrupt
*
* This routine is called at interrupt level in response to an interrupt from
* the controller.
*
* RETURNS: N/A.
*/

LOCAL void elt3c509Int
    (
    ELT3C509_DEVICE *	pDrvCtrl	/* interrupting device */
    )
    {
    UINT16 	status;
    UINT16 	statusDiag;
    UINT8  	statusTx;
    int    	port;
    BOOL   	needTxStart = FALSE;

    port = pDrvCtrl->port;

    ENDLOGMSG (("Got an interrupt!\n", 1, 2, 3, 4, 5, 6));

#ifdef ELT_TIMING
    {
    int time;
    SYS_IN_BYTE (pDrvCtrl, pDrvCtrl->port + TIMER, time);

    if (time> pDrvCtrl->elt3c509Stat.maxIntLatency)
        pDrvCtrl->elt3c509Stat.maxIntLatency = time; 
    if (pDrvCtrl->interruptTime == -1)
        pDrvCtrl->interruptTime = time;
    else if (pDrvCtrl->interruptTime >= 0)
        pDrvCtrl->interruptTime = -1;
    }
#endif /* ELT_TIMING */

    SYS_IN_BYTE (pDrvCtrl, pDrvCtrl->port + ELT3C509_STATUS, status);
    status &= 0x00ff;

    if ((status & INTERRUPT_LATCH) == 0)
        {
        ++pDrvCtrl->elt3c509Stat.strayint;
        return;
        }
    else
        ++pDrvCtrl->elt3c509Stat.interrupts;


    /* Handle adapter failure first in case other conditions mask it */

    if ((status & ADAPTER_FAILURE) != 0)
        {
        SYS_OUT_WORD (pDrvCtrl, port + ELT3C509_COMMAND,
                      SELECT_WINDOW | WIN_DIAGNOSTIC);
        SYS_IN_WORD (pDrvCtrl, port + FIFO_DIAGNOSTIC, statusDiag);
        SYS_OUT_WORD (pDrvCtrl, port + ELT3C509_COMMAND,
                      SELECT_WINDOW | WIN_OPERATING);

        if ((statusDiag & FD_TX_OVERRUN) != 0)
            {
            ++pDrvCtrl->elt3c509Stat.txoverruns;
            SYS_OUT_WORD (pDrvCtrl, port + ELT3C509_COMMAND, TX_RESET);
            elt3c509TxStart (pDrvCtrl);
            }

        if ((status & FD_RX_UNDERRUN) != 0)
            {
            ++pDrvCtrl->elt3c509Stat.rxunderruns;
            SYS_OUT_WORD (pDrvCtrl, port + ELT3C509_COMMAND, RX_RESET);
            elt3c509RxStart (pDrvCtrl);
            }
        }

    if (status & (RX_COMPLETE | RX_EARLY))
        {
        if ((status & RX_EARLY) != 0)
            ++pDrvCtrl->elt3c509Stat.rxearly;

        if (netJobAdd ((FUNCPTR)elt3c509HandlePktsRcv, (int) pDrvCtrl,
                       0, 0, 0, 0) == OK)
            {
            pDrvCtrl->intMask &= ~(RX_COMPLETE | RX_EARLY);
                
#ifdef ELT_TIMING
            if((int)++pDrvCtrl->elt3c509Stat.taskQRxOuts >
               (int)pDrvCtrl->elt3c509Stat.maxRxTaskQ)
                {
                pDrvCtrl->elt3c509Stat.maxRxTaskQ =
                    pDrvCtrl->elt3c509Stat.taskQRxOuts;
                }
#endif /* ELT_TIMING */
            }
        else
            logMsg ("elt: netJobAdd (elt3c509HandlePktsRcv) failed\n",
                    0, 0, 0, 0, 0, 0);
        }
    
    /* Handle transmitter interrupts */

    if ((status & TX_COMPLETE) != 0)
        {
        SYS_IN_BYTE (pDrvCtrl, port+TX_STATUS, statusTx);
        if ((statusTx & TX_S_COMPLETE) != 0)
            {
            SYS_OUT_BYTE (pDrvCtrl,port + TX_STATUS, 0); /* clear old status */

            /*
             * other errors are tabulated by reading the statistics registers
             */

            if ((statusTx & TX_S_MAX_COLL) != 0)
                END_ERR_ADD (&pDrvCtrl->endObj, MIB2_OUT_ERRS, +16);
            if ((statusTx & TX_S_JABBER) != 0)
                END_ERR_ADD (&pDrvCtrl->endObj, MIB2_OUT_ERRS, +1);

            if ((statusTx & (TX_S_JABBER | TX_S_MAX_COLL | TX_S_UNDERRUN))
                != 0)
                {
                needTxStart = TRUE;                 /* restart transmitter */

                /* packet not sent */

                END_ERR_ADD (&pDrvCtrl->endObj, MIB2_OUT_ERRS, +1);
                END_ERR_ADD (&pDrvCtrl->endObj, MIB2_OUT_UCAST, -1);
                }
            if ((statusTx & (TX_S_JABBER | TX_S_UNDERRUN)) != 0)
                {
                /* Must reset transmitter; this clears the tx FIFO */

                SYS_OUT_WORD (pDrvCtrl, port + ELT3C509_COMMAND, TX_RESET);
                }
            }

        if (needTxStart)                            /* check flag */
            elt3c509TxStart (pDrvCtrl);

        if (pDrvCtrl->txBlocked)
            {
            pDrvCtrl->txBlocked = FALSE;

            /* restart the transmit flow */

            netJobAdd ((FUNCPTR) muxTxRestart, (int) &pDrvCtrl->endObj, 0, 0,
                       0, 0);
#ifdef ELT_TIMING
            if ((int)++pDrvCtrl->elt3c509Stat.taskQTxOuts >
                 (int)pDrvCtrl->elt3c509Stat.maxTxTaskQ)
                pDrvCtrl->elt3c509Stat.maxTxTaskQ =
                    pDrvCtrl->elt3c509Stat.taskQTxOuts;
#endif /* ELT_TIMING */
            }
        }

    /* Handle update statistics interrupt */

    if ((status & UPDATE_STATS) != 0)
        elt3c509StatFlush (pDrvCtrl);

    /* mask and ack the events we've just handled or queued handlers for */

    SYS_OUT_WORD (pDrvCtrl, pDrvCtrl->port + ELT3C509_COMMAND,
                  MASK_STATUS | pDrvCtrl->intMask);
    SYS_OUT_WORD (pDrvCtrl, pDrvCtrl->port + ELT3C509_COMMAND,
                  ACK_INTERRUPT | status);
    sysBusIntAck (pDrvCtrl->intLevel);
    }

/*******************************************************************************
*
* elt3c509HandlePktsRcv - task level interrupt service for input packets
*
* This routine is called at task level indirectly by the interrupt
* service routine to do any message received processing.
*
* RETURNS: N/A.
*/

LOCAL void elt3c509HandlePktsRcv
    (
    ELT3C509_DEVICE * pDrvCtrl	/* interrupting device */
    )
    {
    M_BLK_ID 		pMblk;		/* pointer to the mBlk */
    CL_BLK_ID		pClBlk;		/* pointer to the clBlk */
    char *		pCluster;	/* pointer to the cluster */
    int    		port;		/* IO port to read */
    STATUS      	result = OK;    /* Tracks result of Cluster Alloc */

    port = pDrvCtrl->port;
    
    /* loop for more frames */

    while (TRUE)
        {
        /* get a cluster */

        pCluster = netClusterGet (pDrvCtrl->endObj.pNetPool,
                                  pDrvCtrl->pClPoolId);
        if (pCluster == NULL)
            {
            ENDLOGMSG (("Cannot loan!\n", 1, 2, 3, 4, 5, 6));
            ++pDrvCtrl->elt3c509Stat.rxnobuffers;
            END_ERR_ADD (&pDrvCtrl->endObj, MIB2_IN_ERRS, +1);
	    result = ERROR;
            break;
            }

        /* get a cluster block */

        if ((pClBlk = netClBlkGet (pDrvCtrl->endObj.pNetPool, M_DONTWAIT)) ==
            NULL)
            {
            netClFree (pDrvCtrl->endObj.pNetPool, pCluster);
            ENDLOGMSG (("Out of Cluster Blocks!\n", 1, 2, 3, 4, 5, 6));
            END_ERR_ADD (&pDrvCtrl->endObj, MIB2_IN_ERRS, +1);
	    result = ERROR;
            break;
            }
    
        /* get an mBlk */

        if ((pMblk = mBlkGet (pDrvCtrl->endObj.pNetPool, M_DONTWAIT, MT_DATA))
            == NULL)
            {
            netClBlkFree (pDrvCtrl->endObj.pNetPool, pClBlk); 
            netClFree (pDrvCtrl->endObj.pNetPool, pCluster);
            ENDLOGMSG (("Out of M Blocks!\n", 1, 2, 3, 4, 5, 6));
            END_ERR_ADD (&pDrvCtrl->endObj, MIB2_IN_ERRS, +1);
	    result = ERROR;
            break;
            }

        /* Join the cluster to the MBlock */

        netClBlkJoin (pClBlk, pCluster, ELT3C509_BUFSIZ, NULL, 0, 0, 0);
        netMblkClJoin (pMblk, pClBlk);
        
        if (elt3c509PollRcv ((void *)pDrvCtrl, pMblk) == OK)
            {
            END_ERR_ADD (&pDrvCtrl->endObj, MIB2_IN_UCAST, +1);
            ENDLOGMSG (("Calling upper layer's recv rtn\n", 1, 2, 3, 4, 5, 6));

            /* Call the upper layer's receive routine. */

            END_RCV_RTN_CALL (&pDrvCtrl->endObj, pMblk);
            }
        else
            {
            ENDLOGMSG (("EAGAIN\n", 1, 2, 3, 4, 5, 6));
            netMblkClFree (pMblk);
	    result = ERROR;
            break;
            }
        }
    
    /* Discard the packet in the FIFO here of there was an error */

    if (result == ERROR)
	{
	/* discard packet */
        SYS_OUT_WORD (pDrvCtrl, port + ELT3C509_COMMAND, RX_DISCARD);
	}	
    	/* enable the recv interrupt */

    elt3c509IntMaskSet (pDrvCtrl, (RX_COMPLETE | RX_EARLY));
    }

/*******************************************************************************
*
* elt3c509PollRcv - routine to receive a packet in polled mode.
*
* This routine is called by a user to try and get a packet from the
* device.
*
* RETURNS: EGAIN or OK
*/

LOCAL STATUS elt3c509PollRcv
    (
    void  * 	pEnd,		/* device pointer */
    M_BLK_ID 	pMblk		/* pointer to the mBlk chain */
    )
    {
    UINT16 		statusRx;
    UINT16 		statusRxNew;
    UINT16 		length;
    int    		port;
    int			ix = 0;
    int	     		status = OK;
    ELT3C509_DEVICE * 	pDrvCtrl;

    VOID_TO_DRVCTRL (pEnd, pDrvCtrl);

    port = pDrvCtrl->port;
    
    SYS_IN_WORD (pDrvCtrl, port + RX_STATUS, statusRx);
    
    if (statusRx & RX_S_INCOMPLETE)		/* incomplete */
        {
        if (statusRx == RX_S_INCOMPLETE)	/* no bytes available */
            {
            ENDLOGMSG (("no more packets\n", 1, 2, 3, 4, 5, 6));
            return (EAGAIN);
            }

        while (ix < 10000)
            {
            SYS_IN_WORD (pDrvCtrl, port + RX_STATUS, statusRxNew);
            if (statusRx == statusRxNew)
                {
                break;
                }
            else
                {
                statusRx = statusRxNew;
                ix++;
                }
            }
        
        if (statusRx & RX_S_INCOMPLETE)
            {
            END_ERR_ADD (&pDrvCtrl->endObj, MIB2_IN_ERRS, +1);
            ENDLOGMSG (("status quo\n", 1, 2, 3, 4, 5, 6));
            status = EAGAIN;
            goto PollRcvErrHandle;
            }
        }

    /* Check for errors in completed packet */

    if (statusRx & RX_S_ERROR)
        {
        switch (statusRx & RX_S_CODE_MASK)
            {
            case RX_S_OVERRUN:	/* handled by statistics registers */
                break;
            case RX_S_RUNT:
                ++pDrvCtrl->elt3c509Stat.shortPacket;
                break;
            case RX_S_ALIGN:
                ++pDrvCtrl->elt3c509Stat.aligns;
                break;
            case RX_S_CRC:
                ++pDrvCtrl->elt3c509Stat.crcs;
                break;
            case RX_S_OVERSIZE:
                ++pDrvCtrl->elt3c509Stat.badPacket;
                break;
            default:		/* no other codes are defined */
                break;	
            }
        END_ERR_ADD (&pDrvCtrl->endObj, MIB2_IN_ERRS, +1);
        ENDLOGMSG (("recv error\n", 1, 2, 3, 4, 5, 6));
        status = EAGAIN;
        goto PollRcvErrHandle;
        }


    /* length of the buffer in the FIFO */

    length = statusRx & RX_S_CNT_MASK;

    /* discard packet if received frame is bigger than MAX_FRAME_SIZE */

    if (length > MAX_FRAME_SIZE)
        {
        ENDLOGMSG (("pktlen>MAX_FRAME_SIZE\n", 1, 2, 3, 4, 5, 6));
        END_ERR_ADD (&pDrvCtrl->endObj, MIB2_IN_ERRS, +1);
        status = EAGAIN;
        goto PollRcvErrHandle;
        }

    /* if given mBlk does not have enough space */

    if ((!(pMblk->mBlkHdr.mFlags & M_EXT)) || (pMblk->pClBlk->clSize < length))
	{
        ENDLOGMSG (("PRX bad mblk len:%d flags:%d\n", pMblk->mBlkHdr.mLen,
                   pMblk->mBlkHdr.mFlags, 3, 4, 5, 6));
        END_ERR_ADD (&pDrvCtrl->endObj, MIB2_IN_ERRS, +1);
        status = EAGAIN;
        goto PollRcvErrHandle;
	}

    /* read the packet from the FIFO into the cluster */
    
    SYS_IN_WORD_STRING (pDrvCtrl, port + DATA_REGISTER, 
			(short *) pMblk->mBlkHdr.mData, (length + 1) / 2);

    /* update the mBlk fields */

    pMblk->mBlkHdr.mLen   = length;
    pMblk->mBlkHdr.mFlags |= M_PKTHDR;
    pMblk->mBlkPktHdr.len = length;

    status = OK;
    
    PollRcvErrHandle:
    	{
	/* discard packet */
        SYS_OUT_WORD (pDrvCtrl, port + ELT3C509_COMMAND, RX_DISCARD);

        /* wait for command to finish */

        statusRxNew = ERROR;
        while ((statusRxNew & COMMAND_IN_PROGRESS) != 0)
            {
            SYS_IN_WORD (pDrvCtrl, port + ELT3C509_STATUS, statusRxNew);
            }
        }

    return (status);
    }

/******************************************************************************
*
* elt3c509Config - reconfigure the interface under us.
*
* Reconfigure the interface setting promiscuous mode, and changing the
* multicast interface list.
*
* RETURNS: N/A.
*/

LOCAL void elt3c509Config
    (
    ELT3C509_DEVICE * pDrvCtrl	/* device to be re-configured */
    )
    {
    /* Set promiscuous mode if it's asked for. */

    if (END_FLAGS_GET (&pDrvCtrl->endObj) & IFF_PROMISC)
	{
	ENDLOGMSG (("Setting promiscuous mode on!\n", 1, 2, 3, 4, 5, 6));
        pDrvCtrl->rxFilter |= RX_F_PROMISCUOUS;
        SYS_OUT_WORD(pDrvCtrl, pDrvCtrl->port + ELT3C509_COMMAND,
                     SET_RX_FILTER | pDrvCtrl->rxFilter);
	}
    else
	{
	ENDLOGMSG (("Setting promiscuous mode off!\n", 1, 2, 3, 4, 5, 6));
        pDrvCtrl->rxFilter &= ~RX_F_PROMISCUOUS;
        SYS_OUT_WORD (pDrvCtrl, pDrvCtrl->port + ELT3C509_COMMAND,
                      SET_RX_FILTER | pDrvCtrl->rxFilter);
	}

    /* Set up address filter for multicasting. 
     * Multicast bit is set if the address list has one or more entries
     * Multicasting is disabled if the address list is empty
     */

    if (END_MULTI_LST_CNT (&pDrvCtrl->endObj) > 0 ||
             (END_FLAGS_GET (&pDrvCtrl->endObj) & IFF_MULTICAST))
	{
	ENDLOGMSG (("Setting multicast mode on!\n", 1, 2, 3, 4, 5, 6));
        pDrvCtrl->rxFilter |= RX_F_MULTICAST;
        SYS_OUT_WORD (pDrvCtrl, pDrvCtrl->port + ELT3C509_COMMAND,
                      SET_RX_FILTER | pDrvCtrl->rxFilter);
	}
    else
	{
	ENDLOGMSG (("Setting multcast mode off!\n", 1, 2, 3, 4, 5, 6));
        pDrvCtrl->rxFilter &= ~RX_F_MULTICAST;
        SYS_OUT_WORD (pDrvCtrl, pDrvCtrl->port + ELT3C509_COMMAND,
                      SET_RX_FILTER | pDrvCtrl->rxFilter);
	}

    return;
    }

/*******************************************************************************
*
* elt3c509PollSend - routine to send a packet in polled mode.
*
* This routine is called by a user to try and send a packet on the
* device.
*
* RETURNS: OK upon success.  EAGAIN if device is busy.
*/

LOCAL STATUS elt3c509PollSend
    (
    void  * 	pEnd,	        /* device pointer */
    M_BLK_ID 	pMblk		/* packet to send */
    )
    {
    int			len;
    int *		pLenMask;
    char *		pBuf;
    ELT3C509_DEVICE * 	pDrvCtrl;
    UINT16		txStatus;

    VOID_TO_DRVCTRL (pEnd, pDrvCtrl);

    SYS_IN_WORD (pDrvCtrl, pDrvCtrl->port + TX_FREE_BYTES, txStatus);

    if (txStatus >= TX_IDLE_COUNT)
        {
        pBuf = pDrvCtrl->pTxCluster; 	/* pointer to the transmit cluster */

        pLenMask = (UINT32 *)pBuf;
        
        /* allow space for the preamble the trasmit buffer is 1520 */

        pBuf += sizeof(UINT32);

        len = netMblkToBufCopy(pMblk, pBuf, NULL);

        len = max(len, ETHERSMALL);		/* set the packet size */

        *pLenMask = len | TX_F_INTERRUPT;	/* set the preamble */

        len = (len + TX_F_PREAMBLE_SIZE + 3) & TX_F_DWORD_MASK;

        /* place a transmit request */

        SYS_OUT_WORD_STRING (pDrvCtrl, pDrvCtrl->port + DATA_REGISTER, 
				(short *)pLenMask, len / 2);

        /* read the tranmsit status */
        SYS_IN_BYTE (pDrvCtrl, pDrvCtrl->port + TX_STATUS, txStatus);

        /* wait until tramit is complete */
        while (!(txStatus & TX_S_COMPLETE))
            {
            SYS_IN_BYTE (pDrvCtrl, pDrvCtrl->port + TX_STATUS, txStatus);
            }

	/* clear old status */
        SYS_OUT_BYTE (pDrvCtrl, pDrvCtrl->port + TX_STATUS, 0);
        }
    else
        {
        ENDLOGMSG(("Error in Send, not enough TxFreeBytes\n",1,2,3,4,5,6));
        return (EAGAIN);
        }
    
    /* Bump the statistic counter. */

    END_ERR_ADD (&pDrvCtrl->endObj, MIB2_OUT_UCAST, +1);

    ENDLOGMSG (("Poll Send complete\n",1,2,3,4,5,6));
    return (OK);
    }

/*******************************************************************************
*
* elt3c509PollStart - start polled mode operations
*
* This function starts polled mode operation.
*
* The device interrupts are disabled, the current mode flag is switched
* to indicate Polled mode and the device is reconfigured.
*
* RETURNS: OK or ERROR.
*/

LOCAL STATUS elt3c509PollStart
    (
    ELT3C509_DEVICE * pDrvCtrl	/* device to be polled */
    )
    {
    int         oldLevel;
    
    oldLevel = intLock ();          /* disable ints during update */

    pDrvCtrl->flags |= ELT3C_POLLING;

    intUnlock (oldLevel);   /* now elt3c509Int won't get confused */

    elt3c509IntDisable (pDrvCtrl);

    ENDLOGMSG (("STARTED\n", 1, 2, 3, 4, 5, 6));

    elt3c509Config (pDrvCtrl);	/* reconfigure device */

    return (OK);
    }

/*******************************************************************************
*
* elt3c509PollStop - stop polled mode operations
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

LOCAL STATUS elt3c509PollStop
    (
    ELT3C509_DEVICE * pDrvCtrl	/* device structure */
    )
    {
    int         oldLevel;

    oldLevel = intLock ();	/* disable ints during register updates */

    pDrvCtrl->flags &= ~ELT3C_POLLING;

    intUnlock (oldLevel);

    elt3c509IntEnable (pDrvCtrl);

    elt3c509Config (pDrvCtrl);

    ENDLOGMSG (("STOPPED\n", 1, 2, 3, 4, 5, 6));

    return (OK);
    }


/*******************************************************************************
*
* elt3c509IntEnable - enable board to cause interrupts
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

LOCAL void elt3c509IntEnable
    (
    ELT3C509_DEVICE * pDrvCtrl	/* device structure */
    )
    {
    UINT16  status;

    SYS_IN_BYTE (pDrvCtrl, pDrvCtrl->port + ELT3C509_STATUS, status);
    status &= 0x00ff;
    SYS_OUT_WORD (pDrvCtrl, pDrvCtrl->port + ELT3C509_COMMAND,
                  ACK_INTERRUPT | status);
    SYS_OUT_WORD (pDrvCtrl, pDrvCtrl->port + ELT3C509_COMMAND, MASK_INTERRUPT |
                  ADAPTER_FAILURE | TX_COMPLETE | TX_AVAILABLE | RX_COMPLETE |
                  RX_EARLY | INTERRUPT_REQ | UPDATE_STATS);
    }

/*******************************************************************************
*
* elt3c509IntDisable - prevent board from causing interrupts
*
* This routine simply sets all the interrupt mask bits to zero.
* It is intended for guarding board-critical sections.
*
* RETURNS: N/A.
*/

LOCAL void elt3c509IntDisable
    (
    ELT3C509_DEVICE * pDrvCtrl	/* device structure */
    )
    {
    UINT16  status;

    SYS_OUT_WORD (pDrvCtrl, pDrvCtrl->port + ELT3C509_COMMAND,
                  MASK_INTERRUPT | 0);
    SYS_IN_BYTE (pDrvCtrl, pDrvCtrl->port + ELT3C509_STATUS, status);
    status &= 0x00ff;
    SYS_OUT_WORD (pDrvCtrl, pDrvCtrl->port + ELT3C509_COMMAND,
                  ACK_INTERRUPT | status);
    }

/*******************************************************************************
*
* elt3c509TxStart - turn on board's transmit function
*
* RETURNS: N/A.
*/

LOCAL void elt3c509TxStart
    (
    ELT3C509_DEVICE *  pDrvCtrl	/* device structure */
    )
    {
    SYS_OUT_WORD (pDrvCtrl, pDrvCtrl->port + ELT3C509_COMMAND, TX_ENABLE);
    }

/*******************************************************************************
*
* elt3c509RxStart - enable board to start receiving
*
* RETURNS: N/A.
*/

LOCAL void elt3c509RxStart
    (
    ELT3C509_DEVICE *  pDrvCtrl	/* device structure */
    )
    {
    SYS_OUT_WORD (pDrvCtrl, pDrvCtrl->port + ELT3C509_COMMAND,
                  SET_RX_FILTER | pDrvCtrl->rxFilter);
    SYS_OUT_WORD (pDrvCtrl, pDrvCtrl->port + ELT3C509_COMMAND, RX_ENABLE);
    }

/*******************************************************************************
*
* elt3c509Reset - reset the elt3c509 interface
*
* Mark interface as inactive and reset the adapter.
*
* RETURNS: N/A.
*/

LOCAL void elt3c509Reset
    (
    ELT3C509_DEVICE * pDrvCtrl	/* device structure */
    )
    {
    elt3c509IntDisable (pDrvCtrl);	/* prevent device from interrupting */
    SYS_OUT_WORD (pDrvCtrl, pDrvCtrl->port + ELT3C509_COMMAND, RX_RESET);
    SYS_OUT_WORD (pDrvCtrl, pDrvCtrl->port + ELT3C509_COMMAND, TX_RESET);
    }

/*******************************************************************************
*
* elt3c509StatFlush - flush the board's statistics registers to statistics
* block Called when the board reports that its statistics registers are getting
* full, or when someone wants to see the current statistics (to fully update
* the statistics block).
*
* Note that reading a statistics register zeroes it in the hardware.
* Note also that zeroing all the registers is necessary and sufficient
* to clear the interrupt condition.
*
* Must be called with board or system interrupts disabled.
*
* RETURNS: N/A.
*/

LOCAL void elt3c509StatFlush
    (
    ELT3C509_DEVICE * pDrvCtrl	/* device structure */
    )
    {
    int         port;
    UINT16	tempCounter;

    port = pDrvCtrl->port;

    SYS_OUT_WORD (pDrvCtrl, port + ELT3C509_COMMAND,
                  SELECT_WINDOW | WIN_STATISTICS);
    SYS_OUT_WORD (pDrvCtrl, port + ELT3C509_COMMAND, STATS_DISABLE);

    SYS_IN_BYTE (pDrvCtrl, port+CARRIER_LOSTS, tempCounter);
    pDrvCtrl->elt3c509Stat.nocarriers += tempCounter & 0x0f;

    SYS_IN_BYTE (pDrvCtrl, port+SQE_FAILURES, tempCounter);
    pDrvCtrl->elt3c509Stat.heartbeats += tempCounter & 0x0f;

    SYS_IN_BYTE (pDrvCtrl, port+MULT_COLLISIONS, tempCounter);
    pDrvCtrl->elt3c509Stat.multcollisions += tempCounter & 0x3f;

    SYS_IN_BYTE (pDrvCtrl, port+ONE_COLLISIONS, tempCounter);
    pDrvCtrl->elt3c509Stat.collisions +=  tempCounter & 0x3f;

    SYS_IN_BYTE (pDrvCtrl, port+LATE_COLLISIONS, tempCounter);
    pDrvCtrl->elt3c509Stat.latecollisions += tempCounter;

    SYS_IN_BYTE (pDrvCtrl, port+RECV_OVERRUNS, tempCounter);
    pDrvCtrl->elt3c509Stat.rxoverruns += tempCounter;

    SYS_IN_BYTE (pDrvCtrl, port+GOOD_TRANSMITS, tempCounter);
    pDrvCtrl->elt3c509Stat.txnoerror += tempCounter;

    SYS_IN_BYTE (pDrvCtrl, port+GOOD_RECEIVES, tempCounter);
    pDrvCtrl->elt3c509Stat.rxnoerror += tempCounter;

    SYS_IN_BYTE (pDrvCtrl, port+TX_DEFERRALS, tempCounter);
    pDrvCtrl->elt3c509Stat.deferring += tempCounter;

    /* Must read all the registers to be sure to clear the interrupt */

    SYS_IN_WORD (pDrvCtrl, port + BYTES_RECEIVED, tempCounter);
    SYS_IN_WORD (pDrvCtrl, port + BYTES_TRANSMITTED, tempCounter);

    SYS_OUT_WORD (pDrvCtrl, port + ELT3C509_COMMAND, STATS_ENABLE);
    SYS_OUT_WORD (pDrvCtrl, port + ELT3C509_COMMAND,
                  SELECT_WINDOW | WIN_OPERATING);
    }

/*******************************************************************************
*
* elt3c509IntMaskSet - enable specific status conditions to cause interrupts
*
* Sets bit(s) in the intMask field of the device control structure and in
* the board's "read zero mask" where a one bit enables the corresponding
* status condition to be read and to cause an interrupt.
*
* RETURNS: N/A.
*/

LOCAL void elt3c509IntMaskSet
    (
    ELT3C509_DEVICE * 	pDrvCtrl,	/* device structure */
    int 		maskBits	/* mask bits */
    )
    {
    elt3c509IntDisable (pDrvCtrl);

    pDrvCtrl->intMask |= maskBits;
    SYS_OUT_WORD (pDrvCtrl, pDrvCtrl->port + ELT3C509_COMMAND,
                  MASK_STATUS | pDrvCtrl->intMask);

    elt3c509IntEnable (pDrvCtrl);
    }

/*******************************************************************************
*
* elt3c509Activate - attempt to activate the adapter with given address
*
* The 3Com 3C509 ISA adapter does not enable itself at power-on.  This is
* left to the driver, which presumably knows which board it wants.  This
* we do know, from the port field in the driver control structure.
*
* As a helpful side effect, this routine stores the OEM Ethernet address
* of the selected adapter into the driver control structure.
*
* Note that this procedure will activate only one board of the given I/O
* address; this is presumably designed in by 3Com as a helpful feature.
*
* RETURNS: OK or ERROR.
*/

LOCAL STATUS elt3c509Activate
    (
    ELT3C509_DEVICE * pDrvCtrl
    )
    {
    int 	adapterPort;    /* I/O address of adapter we're to look for */
    int 	selectedPort;   /* I/O address of adapter we've found */
    int 	addressConfig;  /* adapter's address configuration register */
    int 	resourceConfig; /* adapter's resource configuration register */
    char 	nodeAddress [EA_SIZE];
    STATUS 	status = OK; 	/* presume OK, change if there's a problem */

    adapterPort = pDrvCtrl->port;

    elt3c509IdCommand (ID_PORT);             /* wake up all adapters */
    SYS_OUT_BYTE (pDrvCtrl, ID_PORT, ID_SET_TAG);   /* clear all tags */

    /* first see if there's a 3Com 3C5xx board out there at all */

    if (elt3c509IdEepromRead (EE_A_MANUFACTURER) != MANUFACTURER_ID)
        return (ERROR);

    /* Identify adapters one by one until we find the right one;
     * as we eliminate adapters, we tag them so they don't participate
     * in the next round of contention eliminations.  Along the way,
     * as part of the adapter contention process, we read out the
     * station address and resource configuration.
     */

    do
        {
        elt3c509IdCommand (ID_PORT);         /* prepare for contention */

        /* Now read all untagged adapters' addresses from EEPROM
         * a bit at a time.  Tagged adapters ignore the reads therefore
         * won't be found; we find the next untagged adapter.
         */

        * (UINT16 *) &nodeAddress [0] = elt3c509IdEepromRead (EE_A_OEM_NODE_0);
        * (UINT16 *) &nodeAddress [2] = elt3c509IdEepromRead (EE_A_OEM_NODE_1);
        * (UINT16 *) &nodeAddress [4] = elt3c509IdEepromRead (EE_A_OEM_NODE_2);
        resourceConfig = elt3c509IdEepromRead (EE_A_RESOURCE);
        addressConfig = elt3c509IdEepromRead (EE_A_ADDRESS);
        if ((addressConfig & AC_IO_BASE_MASK) == AC_IO_BASE_EISA)
            {

            /* the EISA base address is the last possible one; if we hit
             * this value without finding the adapter we want, we're done.
             */

            status = ERROR;
            break;
            }
        selectedPort = (addressConfig & AC_IO_BASE_MASK) * AC_IO_BASE_FACTOR +
                       AC_IO_BASE_ZERO;

        ENDLOGMSG (("elt: activate: adapter at 0x%04x\n", selectedPort, 2, 3,
                    4, 5, 6));

        /* tag this adapter so if we don't want it it won't contend again */

        SYS_OUT_BYTE (pDrvCtrl, ID_PORT, ID_SET_TAG + 1);
        }
    while (selectedPort != adapterPort);

    if (status != ERROR)
        {
        SYS_OUT_BYTE (pDrvCtrl, ID_PORT, ID_ACTIVATE);
        uswab (nodeAddress, (char *)pDrvCtrl->enetAddr, EA_SIZE);
        }

    return (status);
    }

/*******************************************************************************
*
* elt3c509IdCommand - put all adapters into ID command state
*
* This procedure synchronizes the ID state machines of all installed 3Com
* adapters in preparation for contending among them.
*
* RETURNS: N/A.
*/

LOCAL void elt3c509IdCommand
    (
    int idPort          /* I/O address to use as ID port (1x0 hex) */
    )
    {
    int 	idValue;        /* data read or written to ID port */
    int 	count;

    /* We should guard this routine since the ID procedure touches
     * all unactivated adapters.  In fact the first three writes should
     * be guarded against any possible intervening write to any port
     * 0x100, 0x110, 0x120, ... , 0x1f0.
     */

    SYS_OUT_BYTE (pDrvCtrl, idPort, ID_RESET); /* select the ID port */
    
    /* put adapters in ID-WAIT state */
    
    SYS_OUT_BYTE (pDrvCtrl, idPort, ID_RESET);

    idValue = ID_SEQUENCE_INITIAL;
    for (count = ID_SEQUENCE_LENGTH; count > 0; count--)
        {
        SYS_OUT_BYTE (pDrvCtrl, idPort, idValue);
        idValue <<= 1;
        if ((idValue & ID_CARRY_BIT) != 0)
            idValue ^= ID_POLYNOMIAL;
        }
    }

/*******************************************************************************
*
* elt3c509EepromRead - read one 16-bit adapter EEPROM register.
*
* RETURNS: Value from an offset of the EEPROM.
*/

LOCAL int elt3c509EepromRead
    (
    int 	port,
    int 	offset
    )

    {
    int 	ix;
    UINT16	epromStatus;

    SYS_OUT_WORD (NULL, port + EEPROM_CONTROL, 0x80 + offset);

    /* wait 162 us minimum */

    for (ix = 0; ix < 400; ix++)
	sysDelay ();
    
    SYS_IN_WORD (NULL, port + EEPROM_DATA, epromStatus);
    return (epromStatus);
    }


/*******************************************************************************
*
* elt3c509IdEepromRead - read one 16-bit adapter EEPROM register
*
* Read an EEPROM register bitwise by way of the ID port.  Used for adapter
* contention process and to find out what's in the EEPROM.  Addresses are
* masked to a valid size; invalid addresses are simply truncated.
*
* RETURNS: Value from an offset of Eeprom.
*/

LOCAL UINT16 elt3c509IdEepromRead
    (
    int address                         /* EEPROM register address */
    )
    {
    int 	bitCount;
    UINT16 	value;
    UINT16	tempVal;

    SYS_OUT_BYTE (NULL, ID_PORT, ID_EEPROM_READ | (address & ID_EEPROM_MASK));
    taskDelay (2);                      /* must wait for 162 uS read cycle */
    value = 0;
    for (bitCount = ID_REGISTER_SIZE; bitCount > 0; bitCount--)
        {
        value <<= 1;
        SYS_IN_BYTE (NULL, ID_PORT, tempVal);
	value |= (tempVal & 1);
        }
    return (value);
    }

#ifdef DRV_DEBUG
/*******************************************************************************
*
* elt3c509Show - display statistics for the 3C509 `elt' network interface
*
* This routine displays statistics about the `elt' Ethernet network interface.
* It has two parameters: 
* .iP <unit>
* interface unit; should be 0.
* .iP <zap>
* if 1, all collected statistics are cleared to zero.
* .LP
*
* NOMANUAL
*
* RETURNS: N/A
*/

void elt3c509Show 
    (
    ELT3C509_DEVICE * 	pDrvCtrl,	/* device structure */
    BOOL 		zap    		/* 1 = zero totals */
    )
    {
    FAST int ix;

    LOCAL char *e_message [] = {
        "collisions",
        "crcs",
        "aligns",
        "rx no buffers",
        "rx over-runs",
        "disabled",
        "deferring",
        "tx under-run",
        "aborts",
        "out-of-window",
        "heart-beats",
        "bad-packet",
        "short-packet",
        "tx-no-error",
        "rx-no-error",
        "tx-error",
        "rx-error",
        "over-write",
        "wrapped",
        "interrupts",
        "reset",
        "stray-int",
        "multiple-collisions",
        "late-collisions",
        "no-carriers",
        "jabbers",
        "tx over-run",
        "rx under-run",
#ifdef ELT_TIMING
        "rx early",
        "timer updates",
        "timer overflows",
        "timer invalids",
        "max rx latency",
        "min rx latency",
        "max int latency",
        "current outstanding net rx tasks",
        "max outstanding rx net tasks",
        "current outstanding net tx tasks",
        "max outstanding tx net tasks"
#else
        "rx early"
#endif /* ELT_TIMING */
	};

    /* Disable board interrupts because elt3c509StatFlush()
     * changes reg. window
     */

    elt3c509IntDisable (pDrvCtrl);
    elt3c509StatFlush (pDrvCtrl);            /* get the latest statistics */
    elt3c509IntEnable (pDrvCtrl);

    for (ix = 0; ix < NELEMENTS(e_message); ix++)
        {
        printf ("    %-30.30s  %4d\n", e_message [ix],
                pDrvCtrl->elt3c509Stat.stat [ix]);
        if (zap)
            pDrvCtrl->elt3c509Stat.stat [ix] = 0;
        }
    }
#endif /* DRV_DEBUG */
