/* visionTmdDrv.c - Wind River Low-Level visionTMD IO subsystem */

/* Copyright 1988-2002 Wind River Systems Inc. */

/*
modification history
--------------------
01j,13jul02,tcr  Added support for the 440.
01g,21dec01,g_h  Rename to visionTmdDrv.c
01f,29nov01,g_h  Cleaning for T2.2
01e,12nov01,g_h  Adding ColdFire.
01d,05may01,g_h  rename to visionTMDrv.c and cleaning
01c,11apr01,rnr  Turned into a LOW-LEVEL IO subsystem called form vDriver
01b,07feb01,g_h  renaming module name and cleaning
01a,07may97,est  Adapted from memdrv.c
*/

/*
DESCRIPTION
This driver is a LOW-LEVEL I/O subsystem module which is utilized by the
generic WR wdbVisionDrv driver. This subsystem performs basic BYTE level I/O
through the JTAG/BDM interface.

USER-CALLABLE ROUTINES
None

All routines which are callable from this module can be done so only
from the vDriver interface. Only "visionTMDLowLevelIOInit" is declared as
as GLOBAL entry point.

SEE ALSO:
.pG "I/O System"
*/

#include "vxWorks.h"
#include "ioLib.h"
#include "iosLib.h"
#include "memLib.h"
#include "cacheLib.h"
#include "errnoLib.h"
#include "string.h"
#include "stdlib.h"
#include "stdio.h"
#include "taskLib.h"
#include "drv/wdb/vision/wdbVisionDrvIF.h"
#include "drv/wdb/vision/visionTmdDrv.h"

/* typedefs */

typedef struct tmdDataControl 
    {
    TMD_DESCRIPTOR *pSpace;
    TMD_DESCRIPTOR *pTxDesc;
    TMD_DESCRIPTOR *pRxDesc; 
    } TMD_DATA_CONTROL;

/* locals data */

LOCAL TMD_DATA_CONTROL  control;

LOCAL FUNCPTR tmdSignalTxReady   = NULL;
LOCAL FUNCPTR tmdSignalDescReady = NULL;
LOCAL FUNCPTR tmdSignalRxReady   = NULL;

/* globals data */

volatile unsigned long txDesc; /* global copies for rx/tx */
volatile unsigned long rxDesc; /* emulator descriptors    */

/* externals */

#if (CPU == PPC604)
IMPORT void tmdPPC604SignalTxReady (void);
IMPORT void tmdPPC604SignalDescReady (void);
IMPORT void tmdPPC604SignalRxReady (void);
#elif (CPU == PPC405)
IMPORT void tmdPPC405SignalTxReady (void);
IMPORT void tmdPPC405SignalDescReady (void);
IMPORT void tmdPPC405SignalRxReady (void);
#elif (CPU == PPC440)
IMPORT void tmdPPC440SignalTxReady (void);
IMPORT void tmdPPC440SignalDescReady (void);
IMPORT void tmdPPC440SignalRxReady (void);
#elif ((CPU == PPC603) || (CPU == PPC860))
IMPORT void tmdPPCSignalTxReady (void);
IMPORT void tmdPPCSignalDescReady (void);
IMPORT void tmdPPCSignalRxReady (void);
#elif (CPU == MIPS32)
IMPORT void tmdMIPS32SignalTxReady (void);
IMPORT void tmdMIPS32SignalDescReady (void);
IMPORT void tmdMIPS32SignalRxReady (void);
#elif (CPU == ARMARCH4)
IMPORT void tmdARMARCH4SignalTxReady (void);
IMPORT void tmdARMARCH4SignalDescReady (void);
IMPORT void tmdARMARCH4SignalRxReady (void);
#elif (CPU == XSCALE)
IMPORT void tmdXSCALESignalTxReady (void);
IMPORT void tmdXSCALESignalDescReady (void);
IMPORT void tmdXSCALESignalRxReady (void);
#elif ((CPU == SH7700) || (CPU == SH7750) || (CPU == SH7600))
IMPORT void tmdSH7xxxSignalTxReady (void);
IMPORT void tmdSH7xxxSignalDescReady (void);
IMPORT void tmdSH7xxxSignalRxReady (void);
#elif (CPU == MCF5200)
IMPORT void tmdMCF5200SignalTxReady (void);
IMPORT void tmdMCF5200SignalDescReady (void);
IMPORT void tmdMCF5200SignalRxReady (void);
#else
#error "This CPU is not supported !!!"
#endif

/* forward declarations */

VDR_ULONG tmdLowLevelIOInit (V_DRIVER_INTERFACE *pInterfaceData);

LOCAL VDR_ULONG tmdOpen (VDR_PDATA prv);
LOCAL VDR_ULONG tmpClose (VDR_PDATA prv);
LOCAL VDR_ULONG tmdReadStatus (VDR_PDATA prv ,VDR_ULONG *pStatus);
LOCAL VDR_ULONG tmdWriteStatus (VDR_PDATA prv ,VDR_ULONG *pStatus);

LOCAL VDR_ULONG tmdWrite (VDR_PDATA prv ,VDR_UCHAR *pData ,VDR_ULONG size);

LOCAL VDR_ULONG tmdRead (VDR_PDATA prv  ,VDR_UCHAR *pData, 
                         VDR_ULONG size ,VDR_ULONG *rcvBytes);

/***************************************************************************
*
* tmdLowLevelIOInit - initialization entry point for the subsystem
*
* This routine sets up the return parameter space with the entry 
* point names/addresses and any private data the I/O subsystem
* wants to be persistent between the two drivers.
*
* RETURNS: OK, or ERROR if the I/O system cannot install the driver.
*/

VDR_ULONG tmdLowLevelIOInit 
    (
    V_DRIVER_INTERFACE * pInterfaceData 
    ) 
    {

    /*
     *  Setup the return space to indicate we have success in our
     *  initialization and can continue onward.
     */

    pInterfaceData->openFunc = (VDR_OPEN_PTR) tmdOpen;
    pInterfaceData->closeFunc = (VDR_CLOSE_PTR) tmpClose;
    pInterfaceData->readStatusFunc = (VDR_READ_STATUS_PTR) tmdReadStatus;
    pInterfaceData->writeStatusFunc = (VDR_WRITE_STATUS_PTR) tmdWriteStatus;
    pInterfaceData->readFunc = (VDR_READ_PTR) tmdRead;
    pInterfaceData->writeFunc = (VDR_WRITE_PTR) tmdWrite;
    pInterfaceData->privateData = (VDR_PDATA) &control;

	/* Initialization of the signal routines */

    #if (CPU == PPC604)
    tmdSignalTxReady = (FUNCPTR)tmdPPC604SignalTxReady;
    tmdSignalDescReady = (FUNCPTR)tmdPPC604SignalDescReady;
    tmdSignalRxReady = (FUNCPTR)tmdPPC604SignalRxReady;
    #elif (CPU == PPC405)
    tmdSignalTxReady = (FUNCPTR)tmdPPC405SignalTxReady;
    tmdSignalDescReady = (FUNCPTR)tmdPPC405SignalDescReady;
    tmdSignalRxReady = (FUNCPTR)tmdPPC405SignalRxReady;
    #elif (CPU == PPC440)
    tmdSignalTxReady = (FUNCPTR)tmdPPC440SignalTxReady;
    tmdSignalDescReady = (FUNCPTR)tmdPPC440SignalDescReady;
    tmdSignalRxReady = (FUNCPTR)tmdPPC440SignalRxReady;
    #elif ((CPU == PPC603) || (CPU == PPC860))
    tmdSignalTxReady = (FUNCPTR)tmdPPCSignalTxReady;
    tmdSignalDescReady = (FUNCPTR)tmdPPCSignalDescReady;
    tmdSignalRxReady = (FUNCPTR)tmdPPCSignalRxReady;
    #elif (CPU == MIPS32)
    tmdSignalTxReady = (FUNCPTR)tmdMIPS32SignalTxReady;
    tmdSignalDescReady = (FUNCPTR)tmdMIPS32SignalDescReady;
    tmdSignalRxReady = (FUNCPTR)tmdMIPS32SignalRxReady;
    #elif (CPU == ARMARCH4)
    tmdSignalTxReady = (FUNCPTR)tmdARMARCH4SignalTxReady;
    tmdSignalDescReady = (FUNCPTR)tmdARMARCH4SignalDescReady;
    tmdSignalRxReady = (FUNCPTR)tmdARMARCH4SignalRxReady;
    #elif (CPU == XSCALE)
    tmdSignalTxReady = (FUNCPTR)tmdXSCALESignalTxReady;
    tmdSignalDescReady = (FUNCPTR)tmdXSCALESignalDescReady;
    tmdSignalRxReady = (FUNCPTR)tmdXSCALESignalRxReady;
    #elif ((CPU == SH7700) || (CPU == SH7750) || (CPU == SH7600))
    tmdSignalTxReady = (FUNCPTR)tmdSH7xxxSignalTxReady;
    tmdSignalDescReady = (FUNCPTR)tmdSH7xxxSignalDescReady;
    tmdSignalRxReady = (FUNCPTR)tmdSH7xxxSignalRxReady;
    #elif (CPU == MCF5200)
    tmdSignalTxReady = (FUNCPTR)tmdMCF5200SignalTxReady;
    tmdSignalDescReady = (FUNCPTR)tmdMCF5200SignalDescReady;
    tmdSignalRxReady = (FUNCPTR)tmdMCF5200SignalRxReady;
    #else
    #error "This CPU is not supported !!!"
    #endif

    return( VDR_SUCCESS );
    }

/***************************************************************************
*
* tmdOpen - open a connection 
*
* This routine open connection between the TMD runtime said and the Emulator.
*
* RETURNS: VDR_SUCCESS or VDR_FAILURE
*/

LOCAL VDR_ULONG tmdOpen
    ( 
    VDR_PDATA prv  /* private data */
    )
    {
    TMD_DATA_CONTROL *pCtrl = (TMD_DATA_CONTROL*)prv;

    /* Create the device descriptor space */

    pCtrl->pSpace = (TMD_DESCRIPTOR*)cacheDmaMalloc (2 * sizeof(TMD_DESCRIPTOR));

    if (pCtrl->pSpace == (TMD_DESCRIPTOR*)NULL)
        {
        return(VDR_FAILURE);
        }

    pCtrl->pTxDesc = pCtrl->pSpace;
    pCtrl->pRxDesc = pCtrl->pSpace + 1;

    /* Initialize descriptor values */

    pCtrl->pTxDesc->status = TX_BUF_EMPTY;
    pCtrl->pTxDesc->count  = 0;
    pCtrl->pRxDesc->status = RX_BUF_EMPTY;
    pCtrl->pRxDesc->count  = 0;

    /* take address of descriptors (globals for in line asm() routine) */ 

    txDesc = (volatile)((unsigned long)pCtrl->pTxDesc);
    rxDesc = (volatile)((unsigned long)pCtrl->pRxDesc);

    /*
     *  Send a background "signal" to the Emulator to inform it 
     *  where it can find the buffer descriptors.
     */

    ((tmdSignalDescReady)());

    return(VDR_SUCCESS);
    }

/***************************************************************************
*
* tmdCLose - closes the TMD connection
*
* This routine close the connection between the TMD and the Emulator.
*
* RETURNS: VDR_SUCCESS or VDR_FAILURE
*/

LOCAL VDR_ULONG tmpClose
    ( 
    VDR_PDATA prv  /* private data */
    )
    {
    TMD_DATA_CONTROL *pCtrl = (TMD_DATA_CONTROL*)prv;

    /* Deallocate the associated with the XMIT/RECV buffers */

    if (cacheDmaFree ((void*)pCtrl->pSpace) == OK)
        {
        return(VDR_SUCCESS);
        }
    else
        {
        return(VDR_FAILURE);
        }
    }

/***************************************************************************
*
* tmdReadStatus - determines if data is present to be read.
*
* This routine determines if data is present to be read.
*
* RETURNS: VDR_SUCCESS or VDR_FAILURE
*/

LOCAL VDR_ULONG tmdReadStatus
    ( 
    VDR_PDATA  prv,     /* private data    */
    VDR_ULONG *pStatus  /* returned status */
    )
    {
    VDR_ULONG retVal = VDR_SUCCESS;

    TMD_DATA_CONTROL *pCtrl = (TMD_DATA_CONTROL*)prv;

    /* Check if the ctrl pointer is correct */

    if (pCtrl != NULL)
	{
        if (pCtrl->pRxDesc->status == RX_BUF_EMPTY)
            {
	    *pStatus = VDR_DATA_NONE;
            }
        else
            {
	    *pStatus = VDR_DATA_FOUND;
            }
        }
    else
        {
        retVal = VDR_FAILURE;
        }

    return (retVal);
    }

/***************************************************************************
*
* tmdWriteStatus - determines if data is present to be write.
*
* This routine determines
* RETURNS: VDR_SUCCESS or VDR_FAILURE
*/

LOCAL VDR_ULONG  tmdWriteStatus
    ( 
    VDR_PDATA  prv,     /* private data    */
    VDR_ULONG *pStatus  /* returned status */
    )
    {
    VDR_ULONG retVal = VDR_SUCCESS;

    TMD_DATA_CONTROL *pCtrl = (TMD_DATA_CONTROL*)prv;

    /* Check if the ctrl pointer is correct */

    if (pCtrl != NULL)
	{
	if (pCtrl->pTxDesc->status == TX_BUF_EMPTY)
            {
	    *pStatus = VDR_WRITE_COMPLETE;
            }
        else
            {
            *pStatus = VDR_WRITE_PENDING;
            }
	}
    else /* NULL pointer */  
        {
        retVal = VDR_FAILURE;
        }

    return(retVal);
    }

/***************************************************************************
*
* tmdRead - read data from the receive buffer.
*
* This routine read data from the receive buffer.
*
* RETURNS: VDR_SUCCESS or FAILURE and the number of bytes read
*/

LOCAL VDR_ULONG tmdRead
    ( 
    VDR_PDATA  prv,      /* Private data             */
    VDR_UCHAR *pData,    /* return data space        */
    VDR_ULONG  size,     /* Max size of return space */
    VDR_ULONG *pRcvBytes /* Number of bytes returned */
    )
    {
    TMD_DATA_CONTROL *pCtrl = (TMD_DATA_CONTROL*)prv;
    VDR_ULONG         status;

    /* First see if there is any data there */

    if (tmdReadStatus(prv ,&status) != VDR_SUCCESS)
        {
        *pRcvBytes = 0;

        return(VDR_FAILURE);
        }

    if (status == VDR_DATA_NONE)
        {
        *pRcvBytes = 0;

        return(VDR_SUCCESS);
        }

    /* There is data go retrieve it */

    MEMCOPY (pData , &pCtrl->pRxDesc->buf[0] ,pCtrl->pRxDesc->count);

    *pRcvBytes = pCtrl->pRxDesc->count;

    /* Now indicate to the firmware that we have retrieved the data */

    pCtrl->pRxDesc->count  = 0;
    pCtrl->pRxDesc->status = RX_BUF_EMPTY;

    ((tmdSignalRxReady)());

    return (VDR_SUCCESS);
    }

/***************************************************************************
*
* tmdWrite - write data to the TMD channel
*
* This routine write data to the TMD channel.
*
* RETURNS: VDR_SUCCESS or VDR_FAILURE
*/

LOCAL VDR_ULONG tmdWrite
    ( 
    VDR_PDATA  prv,   /* Private data        */
    VDR_UCHAR *pData, /* Data to be written  */
    VDR_ULONG  size   /* # of bytes to write */
    )
    {
    TMD_DATA_CONTROL *pCtrl = (TMD_DATA_CONTROL*)prv;

    /* 
     *   Move the data into the transmit space , setup the count and
     *   indicate the data is there.
     */
 
    MEMCOPY ((char*)&pCtrl->pTxDesc->buf[0] ,pData ,size);

    pCtrl->pTxDesc->count  = size;
    pCtrl->pTxDesc->status = TX_BUF_NOT_EMPTY;

    /* Tell the emulation that there is data to be retrieved */

    ((tmdSignalTxReady)());

    return (VDR_SUCCESS);
    }
