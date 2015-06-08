/* mpc107Dma.c - MPC107 DMA Interface support */

/* Copyright 1996-2000 Wind River Systems, Inc. */
#include "copyright_wrs.h"

/*
modification history
--------------------
01b,11sep00,rcs correct headerfile paths
01a,18jun00,bri written
*/

/*
DESCRIPTION
This module contains routines for the DMA interface of MPC107 .

The DMA controller transfers blocks of data independent of the local processor
or PCI hosts. Data movement occurs on the PCI and/or memory bus. The MPC107
has two DMA channels, each with a 64-byte queue to facilitate the gathering
and sending of data.Both the local processor and PCI masters can initiate a
DMA transfer.

Some of the features of the MPC107 DMA unit are:

Two DMA channels and both channels accessible by processor core
and remote PCI masters.

Misaligned transfer capability.

Chaining mode (including scatter gathering) and Direct mode.

Interrupt on completed segment, chain, and error conditions.

Four DMA transfer types:
Local memory to local memory,PCI memory to PCI memory,
PCI memory to local memory, and  Local memory to PCI memory.

This module provides support for "chained" (periodic and non periodic )DMA
and "direct" DMA  transfers on both DMA channels .

All the four DMA transfer types are supported by this module (local memory to
local memory,local memory to PCI mmeory, PCI memory to local memory and
PCI to PCI memory) .

The descriptors for the chained DMA are built in the local memory .

.SH INITIALIZATION
This driver is initialized from the BSP, usually as part of sysHwInit().
The routine mpc107DmaInit() should be called to initialize the  driver .
*/

/* includes */

#include "vxWorks.h"
#include "semLib.h"
#include "sysLib.h"
#include "drv/multi/mpc107.h"
#include "drv/dma/mpc107Dma.h"
#include "drv/intrCtl/mpc107Epic.h"
#include "errno.h"
#include "errnoLib.h"
#include "cacheLib.h"
#include "intLib.h"
#include "memLib.h"
#include "stdlib.h"


/* static file scope locals */

MPC107_DMA_DESCRIPTOR 	*pFirstDescriptorCh0 = NULL; /* Pointer to DMA */
                                                     /* descriptor */
MPC107_DMA_DESCRIPTOR	*pFirstDescriptorCh1 = NULL; /* Pointer to DMA */
                                                     /* descriptor */
BOOL 		chainedDma0FirstTime = FALSE;/* Flag for chained DMA CH0 */
BOOL 		chainedDma1FirstTime = FALSE;/* Flag for chained DMA CH1 */
ULONG 		dmaBuildDescrInProgCh0 = 0;  /* Flag for building descriptors */
ULONG 		dmaBuildDescrInProgCh1 = 0;  /* Flag for building descriptors */
SEM_ID 		semIdDirectDmaCh0;           /* Semaphore for direct DMA CH0 */
SEM_ID 		semIdDirectDmaCh1;           /* Semaphore for direct DMA CH1 */
SEM_ID 		semIdChainDmaCh0;            /* Semaphore for chained DMA CH0 */
SEM_ID 		semIdChainDmaCh1;            /* Semaphore for chained DMA CH1 */

/* forward Declarations */

void 		mpc107DmaCh0Int (void); /* ISR for channel 0 */
void 		mpc107DmaCh1Int (void); /* ISR for channel 1 */
LOCAL STATUS	mpc107DmaChainStart (UINT32 channelNumber);
LOCAL STATUS	mpc107DmaPeriodicStart (UINT32 channelNumber,UINT32 timePeriod);


/***************************************************************************
*
* mpc107DmaInit - initialize DMA  controller
*
* The DMA interface is initialized to be in "Direct DMA transfer" mode and
* DMA  interrupts(End of Transfer Interrupt and Error Interrupt) are
* disabled .
* This routine also attaches the interrupt handlers of the DMA interface.
*
* This routine should be called only once, during hardware initialization,
* before using the DMA interface.
*
* RETURNS: N/A
*/

void mpc107DmaInit (void)
    {

    /*
     * Initialize the  DMA Channel Zero for the following :
     * Error Interrupt Disabled,
     * End of Transfer Interrupt Disabled
     * Direct Mode DMA transfer
     */

    MPC107EUMBBARWRITE(MPC107_DMA_DMR0, MPC107_DMA_DMR0_DEFAULT);

    /*
     * Initialize the  DMA Channel One for the following :
     * Error Interrupt Disabled,
     * End of Transfer Interrupt Disabled
     * Direct Mode DMA transfer
     */

    MPC107EUMBBARWRITE(MPC107_DMA_DMR1, MPC107_DMA_DMR1_DEFAULT);

    /*
     * Semaphore used for mutual exclusion in Direct DMA transfer
     * of Channel Zero
     */

    semIdDirectDmaCh0 = semBCreate ((INT32)SEM_FULL, SEM_Q_PRIORITY);

    /*
     * Semaphore used for mutual exclusion in Direct DMA transfer
     * of Channel One
     */

    semIdDirectDmaCh1 = semBCreate ((INT32)SEM_FULL, SEM_Q_PRIORITY);

    /*
     * Semaphore used for mutual exclusion in Chained DMA transfer
     * of Channel Zero
     */

    semIdChainDmaCh0  = semBCreate ((INT32)SEM_FULL, SEM_Q_PRIORITY);

    /*
     * Semaphore used for mutual exclusion in Chained DMA transfer
     * of Channel One
     */

    semIdChainDmaCh1  = semBCreate ((INT32)SEM_FULL, SEM_Q_PRIORITY);


    /* The ISR handlers should be hooked here */

    intConnect ((VOIDFUNCPTR*)MPC107_EPIC_DMA0_INT_VECT,
                                 (VOIDFUNCPTR)mpc107DmaCh0Int, 0);

    intConnect ((VOIDFUNCPTR*)MPC107_EPIC_DMA1_INT_VECT,
                                 (VOIDFUNCPTR)mpc107DmaCh1Int, 0);

    }


/***************************************************************************
*
* mpc107DmaStartDirect - start  "Direct" DMA transfer
*
* This routine is used for starting "Direct" mode of DMA transfer."Direct"
* mode of DMA transfer can be done on both the DMA channels .All the registers
* of the corresponding channel are initialized to perform "Direct" DMA transfer.
*
* RETURNS:  OK, or ERROR if channel is configured for Periodic DMA or
* <numberOfBytes> is zero or channel specified by <channelNumber> is
* not valid .
*/

STATUS mpc107DmaStartDirect
    (
    UINT32 	channelNumber,      /* Channel Number */
    UINT32 	transferType,       /* Transfer Type */
    UINT32 	sourceAddress,      /* Source Address */
    UINT32 	destinationAddress, /* Destinantion Address */
    UINT32 	numberOfBytes       /* Number of Bytes */
    )
    {

    UINT32 statusOfDmaReg = 0;
    UINT32 dataReadDmaReg = 0;

    if (channelNumber == MPC107_DMA_CHANNEL0) /* channel 0 */
        {

        /* Reading of DMA Mode Register */

        statusOfDmaReg = MPC107EUMBBARREAD(MPC107_DMA_DMR0);

        if ( (!(statusOfDmaReg & MPC107_DMA_DMR_CTM) &&
               (statusOfDmaReg & MPC107_DMA_DMR_PDE)) != 0)
            {

            /* This channel is configured for Periodic DMA */

            errnoSet (EBUSY);
            return (ERROR);

            }

        if (numberOfBytes != 0) /* number of bytes is non-zero */
            {

            semTake (semIdDirectDmaCh0, WAIT_FOREVER); /* Take the semaphore */

            /* waiting for the channel to be free */

            while (MPC107EUMBBARREAD(MPC107_DMA_DSR0) & MPC107_DMA_DSR_CB);

            /* Program the Source Address Register */

            MPC107EUMBBARWRITE(MPC107_DMA_SAR0, sourceAddress);

            /* Program the Destination Address Register */

            MPC107EUMBBARWRITE(MPC107_DMA_DAR0, destinationAddress);

            /* Program the number of bytes in the Byte Count Register */

            MPC107EUMBBARWRITE(MPC107_DMA_BCR0, numberOfBytes);

            /* Program the Current Descriptor Address Register 0 */

            dataReadDmaReg =  (MPC107EUMBBARREAD(MPC107_DMA_CDAR0) &
                               MPC107_DMA_CDAR_ADDR_MASK) |
                              (transferType << MPC107_DMA_CDAR_CTT_SHIFT) ;

            MPC107EUMBBARWRITE(MPC107_DMA_CDAR0, dataReadDmaReg);

            /* Program the DMA Mode Register 0 to start DMA */

            dataReadDmaReg = MPC107EUMBBARREAD(MPC107_DMA_DMR0) &
                           !((MPC107_DMA_DMR_CS) | MPC107_DMA_DMR_CTM) ;

            /*
             * Clear the Channel Start Bit  and  the Channel Transfer Mode
             * to be Direct DMA
             */

            MPC107EUMBBARWRITE(MPC107_DMA_DMR0, dataReadDmaReg);

            /* Set the Channel Start Bit to start DMA transfer */

            MPC107EUMBBARWRITE(MPC107_DMA_DMR0, dataReadDmaReg |
                               MPC107_DMA_DMR_CS);

            semGive (semIdDirectDmaCh0); /* release the semaphore */

            }
        else
            {

            errnoSet (EINVAL);
            return (ERROR);

            }

        }
    else if (channelNumber == MPC107_DMA_CHANNEL1) /* channel 1 */
        {

        statusOfDmaReg = MPC107EUMBBARREAD(MPC107_DMA_DMR1);

        if ( (!(statusOfDmaReg & MPC107_DMA_DMR_CTM) &&
               (statusOfDmaReg & MPC107_DMA_DMR_PDE)) != 0)
            {

            /* This channel is configured for Periodic DMA */

            errnoSet (EBUSY);
            return (ERROR);

            }

        if (numberOfBytes != 0) /* If number of bytes is not zero */
            {

            semTake (semIdDirectDmaCh1, WAIT_FOREVER); /* Take the semaphore */

            /* Polling to see when the channel is free */

            while (MPC107EUMBBARREAD(MPC107_DMA_DSR1) & MPC107_DMA_DSR_CB);

            /* Program the Source Address Register */

            MPC107EUMBBARWRITE(MPC107_DMA_SAR1, sourceAddress);

            /* Program the Destination Address Register */

            MPC107EUMBBARWRITE(MPC107_DMA_DAR1, destinationAddress);

            /* Program the number of bytes in the Byte Count Register */

            MPC107EUMBBARWRITE(MPC107_DMA_BCR1, numberOfBytes);

            /* Program the Current Descriptor Address Register 1 */

            dataReadDmaReg =  MPC107EUMBBARREAD(MPC107_DMA_CDAR1) &
                                (MPC107_DMA_CDAR_ADDR_MASK |
                                (transferType << MPC107_DMA_CDAR_CTT_SHIFT));

            MPC107EUMBBARWRITE(MPC107_DMA_CDAR1, dataReadDmaReg);

            /* Program the DMA Mode Register 1 to start DMA */

            dataReadDmaReg = MPC107EUMBBARREAD(MPC107_DMA_DMR1) &
                           !((MPC107_DMA_DMR_CS) | MPC107_DMA_DMR_CTM) ;

            /*
             * Clear the Channel Start Bit  and  the Channel Transfer Mode
             * to be Direct DMA
             */

            MPC107EUMBBARWRITE(MPC107_DMA_DMR1, dataReadDmaReg);

            /* Set the Channel Start Bit to start DMA transfer */

            MPC107EUMBBARWRITE(MPC107_DMA_DMR1, dataReadDmaReg
                              | MPC107_DMA_DMR_CS);

            semGive (semIdDirectDmaCh1); /* Release the semaphore */

            }
        else
            {

            errnoSet (EINVAL);
            return (ERROR); /* Number of bytes is Zero */

            }


        }
    else
        {

        errnoSet (EINVAL);
        return (ERROR) ; /* Undefined Channel Number */

        }

    return (OK) ;

    }


/***************************************************************************
*
* mpc107DmaChainStart  -  start Chained (non-periodic) DMA Transfer
*
* NOTES
*
* This routine  initializes the specified DMA channel for a "chained"
* (non-periodic) DMA transfer.This routine is called in the routine
* mpc107DmaChainedStart() after the "DMA descriptors" are built using the
* routine  mpc107DmaBuildDescriptor().
*
* RETURNS: OK, or ERROR if channel specified by <channelNumber> is not
* a valid channel or the channel is configured for "periodic" DMA .
*/

LOCAL STATUS mpc107DmaChainStart
    (
    UINT32 	channelNumber  /* channel number */
    )
    {

    UINT32 	statusOfDmaReg;
    UINT32 	dataReadDmaReg;

    if (channelNumber == MPC107_DMA_CHANNEL0) /* channel 0 */
        {

        statusOfDmaReg = MPC107EUMBBARREAD(MPC107_DMA_DMR0);

        if ( (!(statusOfDmaReg & MPC107_DMA_DMR_CTM) &&
               (statusOfDmaReg & MPC107_DMA_DMR_PDE)) != 0 )
            {

            /* This channel is configured for Periodic DMA */

            errnoSet (EBUSY);
            return (ERROR);

            }
        else if ((statusOfDmaReg & MPC107_DMA_DMR_CTM) != 0)
            {

            /* This channel is configured for Direct DMA */

            /* Waiting for the channel to be free */

            while (MPC107EUMBBARREAD(MPC107_DMA_DSR0) & MPC107_DMA_DSR_CB);

            /* Program the Current Descriptor Address Register 0 */

            MPC107EUMBBARWRITE(MPC107_DMA_CDAR0, (UINT32)pFirstDescriptorCh0);

            /* Program the DMA Mode Register 0 to start DMA */

            dataReadDmaReg = MPC107EUMBBARREAD(MPC107_DMA_DMR0) &
                           !(MPC107_DMA_DMR_CS) & !(MPC107_DMA_DMR_CTM);

            /*
             * Clear the Channel Start Bit  and  the Channel Transfer Mode
             * to be Chained  DMA
             */

            MPC107EUMBBARWRITE(MPC107_DMA_DMR0, dataReadDmaReg);

            /* Set the Channel Start Bit to start DMA transfer */

            MPC107EUMBBARWRITE(MPC107_DMA_DMR0, dataReadDmaReg |
                               MPC107_DMA_DMR_CS);

            }

        else if ((statusOfDmaReg & MPC107_DMA_DMR_CTM) == 0)
            {

            /* This channel is configured for Chained DMA */

            /* If Chained DMA is already in progress  */

            if ((MPC107EUMBBARREAD(MPC107_DMA_DSR0) & MPC107_DMA_DSR_CB) != 0)
                {


                /*
                 * Set the channel continue bit to in DMA Mode Register 0
                 * to start DMA
                 */

                dataReadDmaReg = MPC107EUMBBARREAD(MPC107_DMA_DMR0);

                MPC107EUMBBARWRITE(MPC107_DMA_DMR0, dataReadDmaReg |
                                   MPC107_DMA_DMR_CC);

                }

            /*
             * If chained DMA is not in progress  and the descriptors are
             * built afresh
             */

            else if ((chainedDma0FirstTime))
                {

                /* Program the Current Descriptor Address Register 0 */

                MPC107EUMBBARWRITE(MPC107_DMA_CDAR0,
                                  (UINT32)pFirstDescriptorCh0);

                /* Program the DMA Mode Register 0 to start DMA */

                dataReadDmaReg = MPC107EUMBBARREAD(MPC107_DMA_DMR0) &
                                                 !(MPC107_DMA_DMR_CS) &
                                                 !(MPC107_DMA_DMR_CTM);

                /*
                 * Clear the Channel Start Bit  and  the Channel Transfer Mode
                 * to be Chained  DMA
                 */

                MPC107EUMBBARWRITE(MPC107_DMA_DMR0, dataReadDmaReg);

                /* Set the Channel Start Bit to start DMA transfer */

                MPC107EUMBBARWRITE(MPC107_DMA_DMR0, dataReadDmaReg |
                                  MPC107_DMA_DMR_CS);

                }

            }

        }

    else if (channelNumber == MPC107_DMA_CHANNEL1) /* channel 1 */
        {

        statusOfDmaReg = MPC107EUMBBARREAD(MPC107_DMA_DMR1);

        if ( (!(statusOfDmaReg & MPC107_DMA_DMR_CTM) &&
              (statusOfDmaReg & MPC107_DMA_DMR_PDE)) != 0)
            {

            /* This channel is configured for Periodic DMA */

            errnoSet (EBUSY);
            return (ERROR);

            }
        else if ((statusOfDmaReg & MPC107_DMA_DMR_CTM) != 0)
            {

            /* This channel is configured for Direct DMA */

            /* Waiting for the channel to be free */

            while (MPC107EUMBBARREAD(MPC107_DMA_DSR0) & MPC107_DMA_DSR_CB);

            /* Program the Current Descriptor Address Register 1 */

            MPC107EUMBBARWRITE(MPC107_DMA_CDAR1, (UINT32)pFirstDescriptorCh1);

            /* Program the DMA Mode Register 1 to start DMA */

            dataReadDmaReg = MPC107EUMBBARREAD(MPC107_DMA_DMR1) &
                           !(MPC107_DMA_DMR_CS) & !(MPC107_DMA_DMR_CTM);

            /*
             * Clear the Channel Start Bit  and  the Channel Transfer Mode
             * to be Chained  DMA
             */

            MPC107EUMBBARWRITE(MPC107_DMA_DMR1, dataReadDmaReg);

            /* Set the Channel Start Bit to start DMA transfer */

            MPC107EUMBBARWRITE(MPC107_DMA_DMR1, dataReadDmaReg |
                               MPC107_DMA_DMR_CS);

            }

        else if ((statusOfDmaReg & MPC107_DMA_DMR_CTM) == 0)
            {

            /* This channel is configured for Chained DMA */

            /* if Chained DMA is already in progress  */

            if ((MPC107EUMBBARREAD(MPC107_DMA_DSR1) & MPC107_DMA_DSR_CB) != 0)
                {


               /*
                * Set the channel continue bit to in DMA Mode Register 1
                * to start DMA
                */

                dataReadDmaReg = MPC107EUMBBARREAD(MPC107_DMA_DMR1);

                MPC107EUMBBARWRITE(MPC107_DMA_DMR1, dataReadDmaReg |
                                   MPC107_DMA_DMR_CC);


                }

            /*
             * if chained DMA is not in progress  and the descriptors are
             * built afresh
             */

            else if ((chainedDma1FirstTime))
                {

                /* Program the Current Descriptor Address Register 1 */

                MPC107EUMBBARWRITE(MPC107_DMA_CDAR1,
                                  (UINT32)pFirstDescriptorCh1);

                /* Program the DMA Mode Register 1 to start DMA */

                dataReadDmaReg = MPC107EUMBBARREAD(MPC107_DMA_DMR1) &
                                                 !(MPC107_DMA_DMR_CS) &
                                                 !(MPC107_DMA_DMR_CTM);

                /*
                 * Clear the Channel Start Bit  and  the Channel Transfer Mode
                 * to be Chained  DMA
                 */

                MPC107EUMBBARWRITE(MPC107_DMA_DMR1, dataReadDmaReg);

                /* Set the Channel Start Bit to start DMA transfer */

                MPC107EUMBBARWRITE(MPC107_DMA_DMR1, dataReadDmaReg |
                                   MPC107_DMA_DMR_CS);


                }
            }

        }

    else
        {
        errnoSet (EINVAL);
        return (ERROR);
        }

    return (OK);

    }


/***************************************************************************
*
* mpc107DmaPeriodicStart -  start Periodic Chained DMA
*
* This routine initializes the specified  DMA channel for a  "periodic"
* DMA transfer.
* This routine is called in the routine mpc107DmaChainedStart() after the
* "DMA descriptors" are built using the routine  mpc107DmaBuildDescriptor().
* All the descriptors for a "periodic" DMA should be built before starting
* the chained DMA transfer. Descriptors  cannot be added while a periodic
* DMA transfer is taking place.
*
* RETURNS: OK, or ERROR if the channel specified by <channelNumber> is not a
* valid channel or the  channel is already configured for "periodic" DMA .
*/

LOCAL STATUS mpc107DmaPeriodicStart
    (
    UINT32	channelNumber,  /* DMA Channel Number */
    UINT32	timePeriod      /* Time period */
    )
    {

    UINT32 	tempDataRead;
    STATUS 	status;

    if (channelNumber == MPC107_DMA_CHANNEL0) /* channel 0 */
        {

        /* Disable interrupt for the timer */

        tempDataRead = MPC107EUMBBARREAD(MPC107_EPIC_TM2_VEC_REG) |
                                          MPC107_EPIC_TM_VECREG_INTMASK;

        /* Program the count based on time period  and Clear  Count Inhibit */

        MPC107EUMBBARWRITE(MPC107_EPIC_TM2_BASE_COUNT_REG,
                           timePeriod & ! (MPC107_EPIC_TM_BASE_COUNT_CI));


        /* Start Periodic DMA transfer and Set the periodic DMA Enable Bit */

        status = mpc107DmaChainStart (channelNumber);

        if (status == OK)
            {

            tempDataRead = MPC107EUMBBARREAD(MPC107_DMA_DMR0) |
                                             MPC107_DMA_DMR_PDE;

            MPC107EUMBBARWRITE(MPC107_DMA_DMR0, tempDataRead);
            }

        return (status);

        }
    else if (channelNumber == MPC107_DMA_CHANNEL1) /* channel 1 */
        {

        /* Disable interrupt for the timer */

        tempDataRead = MPC107EUMBBARREAD(MPC107_EPIC_TM3_VEC_REG) |
                                          MPC107_EPIC_TM_VECREG_INTMASK;

        /* Program the count based on time period  and Clear  Count Inhibit */

        MPC107EUMBBARWRITE(MPC107_EPIC_TM3_BASE_COUNT_REG,
                           timePeriod & ! (MPC107_EPIC_TM_BASE_COUNT_CI));

        /* Start Periodic DMA transfer and Set the periodic DMA Enable Bit */

        status = mpc107DmaChainStart (channelNumber);

        if (status == OK)
            {
            tempDataRead = MPC107EUMBBARREAD(MPC107_DMA_DMR1) |
                                             MPC107_DMA_DMR_PDE;

            MPC107EUMBBARWRITE(MPC107_DMA_DMR1, tempDataRead);

            }

        return (status);

        }
    else
        {
        errnoSet (EINVAL);
        return (ERROR);
        }

    return (OK);
    }


/***************************************************************************
*
* mpc107DmaChainedStart - start  a  "Chained" (Periodic or Non Periodic) DMA
*
* This routine is used for starting a "chained" (Periodic or Non Periodic)
* DMA  transfer.
*
* The <timePeriod> specified should be in microseconds. If a zero <timePeriod>
* is specified, a "chained" DMA transfer is started. If a nonzero <timePeriod>
* is specified, a "periodic" chained DMA transfer is started. The <timePeriod>
* should not be less than the time it takes for the DMA transfer for periodic
* DMA transfers.
* This routine should be called after the "DMA descriptors" are built using
* the routine  mpc107DmaBuildDescriptor().All the descriptors for a "periodic"
* DMA should be built before starting the chained DMA transfer. Descriptors
* cannot be added while a periodic DMA transfer is taking place.
* The routine  mpc107DmaStopPeriodic()  has to be used to stop the on-going
* periodic DMA  before a new periodic DMA can be started with a new set of
* descriptors.
*
* RETURNS: OK, or ERROR if channel specified by <channelNumber> is not a valid
* channel or the channel is already configured for "periodic" DMA .
*/

STATUS mpc107DmaChainedStart
    (
    UINT32 	channelNumber, /* DMA Channel Number  */
    UINT32 	timePeriod     /* time period for repetition for a periodic */
                               /* DMA transfer, zero for chained DMA. */
                               /* Time period is specified in  multiples  */
                               /* of Microseconds */
    )
    {

    if ((channelNumber == MPC107_DMA_CHANNEL0) ||
        (channelNumber == MPC107_DMA_CHANNEL1))

        {
        if (timePeriod == NULL) /* If time period specified is zero */
            {

            /* Start Chained DMA */

            return (mpc107DmaChainStart (channelNumber));

            }
        else
            {

            /* Start Periodic DMA */

            return (mpc107DmaPeriodicStart (channelNumber, timePeriod));

            }

        }
    else
        {

        errnoSet (EINVAL);
        return (ERROR);

        }

   }

/***************************************************************************
*
* mpc107DmaPeriodicStop -  stop Periodic DMA
*
* This routine is used for stopping "periodic" chained  DMA transfer for the
* specified DMA channel.
*
* RETURNS: OK, or ERROR if specified <channelNumber> is not a valid channel.
*/

STATUS mpc107DmaPeriodicStop
    (
    UINT32 	channelNumber  	/* Channel Number  */
    )
    {

    MPC107_DMA_DESCRIPTOR *pTempDescriptor;
    MPC107_DMA_DESCRIPTOR *pTempFirstDescriptor;
    UINT32 dataReadDmaReg;

    if (channelNumber == MPC107_DMA_CHANNEL0)
        {

        /* Set  Count Inhibit to Inhibit Timer */

        MPC107EUMBBARWRITE(MPC107_EPIC_TM2_BASE_COUNT_REG,
                           MPC107_EPIC_TM_BASE_COUNT_CI);

        /* Disable Periodic DMA by Clearing  the periodic DMA Enable Bit */

        dataReadDmaReg = MPC107EUMBBARREAD(MPC107_DMA_DMR0) &
                                          !(MPC107_DMA_DMR_PDE);

        MPC107EUMBBARWRITE(MPC107_DMA_DMR0, dataReadDmaReg);

        /* Release all the Descriptors */

        pTempFirstDescriptor = pFirstDescriptorCh0;


        /* If Descriptors Already Exist */

        if (((UINT32)pTempFirstDescriptor & MPC107_DMA_NDAR_ADDR_MASK) != 0)
            {

            while (((UINT32)pTempFirstDescriptor &
                                      MPC107_DMA_NDAR_ADDR_MASK ) != NULL )
                {

                pTempDescriptor = pTempFirstDescriptor;

                pTempFirstDescriptor = pTempFirstDescriptor
                                      -> pNextDescriptorAddress;

                free ((void *)((UINT32)pTempDescriptor & 
                               MPC107_DMA_NDAR_ADDR_MASK));

                }
            }

            pFirstDescriptorCh0 = NULL;

            chainedDma0FirstTime = FALSE;

        }

    else if (channelNumber == MPC107_DMA_CHANNEL1) /* channel 1 */
        {

        /* Set  Count Inhibit to Inhibit Timer */

        MPC107EUMBBARWRITE(MPC107_EPIC_TM3_BASE_COUNT_REG,
                           MPC107_EPIC_TM_BASE_COUNT_CI);

        /* Disable Periodic DMA by Clearing  the periodic DMA Enable Bit */

        dataReadDmaReg = MPC107EUMBBARREAD(MPC107_DMA_DMR1) &
                                          !(MPC107_DMA_DMR_PDE);

        MPC107EUMBBARWRITE(MPC107_DMA_DMR1, dataReadDmaReg);

        /* Release all the Descriptors */

        pTempFirstDescriptor = pFirstDescriptorCh1;

        /* Descriptors Already Exist */

        if (((UINT32)pTempFirstDescriptor & MPC107_DMA_NDAR_ADDR_MASK) != 0)
            {

            while (((UINT32)pTempFirstDescriptor &
                                          MPC107_DMA_NDAR_ADDR_MASK) != NULL)
                {

                pTempDescriptor = pTempFirstDescriptor;


                pTempFirstDescriptor = pTempFirstDescriptor
                                      -> pNextDescriptorAddress;

                free ((void *)((UINT32)pTempDescriptor & 
                               MPC107_DMA_NDAR_ADDR_MASK));

                }

            }

            pFirstDescriptorCh1 = NULL;

            chainedDma1FirstTime = FALSE;

        }

    else
        {
        errnoSet (EINVAL);
        return (ERROR);
        }

    return (OK);

    }


/***************************************************************************
*
* mpc107DmaBuildDecsriptor - build  DMA  descriptors
*
* This routine is used for creation of a DMA descriptor required
* for chained (periodic or non-periodic) DMA transfers.
* Before a chained DMA transfer can be started, a set of descriptors can
* be setup by calling this routine as many number of times as required
* with the appropriate parameters. The routine mpc107DmaChainedStart should be
* called after the descriptors are built.
*
* If a periodic DMA is in progress, then the descriptor cannot be built and
* the routine returns an error. If periodic DMA is not in progress then the
* descriptor is formed. A periodic chained DMA with a new set of descriptors
* can only be started by stopping the chained periodic DMA in progress using
* the routine mpc107DmaPeriodicStop.
*
* RETURNS:  OK, or ERROR if channel is configured for Periodic DMA or
* <numberOfBytes> is zero or channel specified by <channelNumber> is
* not valid .
*/

STATUS mpc107DmaBuildDecsriptor
    (
    UINT32 	channelNumber,	     /* Channel Number  */
    UCHAR *	pSourceAddress,      /* Pointer to  DMA start Address  */
    UCHAR *	pDestinationAddress, /* Pointer to DMA destination address */
    UINT32	transferType,        /* specifies whether the transfer */
                                     /* is "PCI to PCI"  */
                                     /* or "Memory to Memory" */
                                     /* or "Memory to PCI" */
                                     /* or  "Memory to Memory" */
    UINT32 numberOfBytes             /* Number of bytes to be sent by DMA */
    )
    {

    MPC107_DMA_DESCRIPTOR *	pTempDescriptor;
    MPC107_DMA_DESCRIPTOR *	pTempFirstDescriptor;
    ULONG 		statusOfDmaReg;


    if (numberOfBytes == 0) /* number of bytes is zero */
        {

        /* if the number of bytes is zero return an error */

        errnoSet (EINVAL);
        return (ERROR);

        }

    if (channelNumber == MPC107_DMA_CHANNEL0) /* channel 0 */
        {
        statusOfDmaReg = MPC107EUMBBARREAD(MPC107_DMA_DMR0);

        if ((!(statusOfDmaReg & MPC107_DMA_DMR_CTM) &&
              (statusOfDmaReg & MPC107_DMA_DMR_PDE)) != 0)
            {

            /* This channel is configured for Periodic DMA */

            errnoSet (EBUSY);
            return (ERROR);

            }


        semTake (semIdChainDmaCh0, WAIT_FOREVER);

        /* If Direct DMA is configured */

        if ((statusOfDmaReg & MPC107_DMA_DMR_CTM) != 0)
            {

            /* Polling to see when the channel is free */

            while (MPC107EUMBBARREAD(MPC107_DMA_DSR0) & MPC107_DMA_DSR_CB);

            }

        /* Flag set to indicate that the descriptors are  being built */

        dmaBuildDescrInProgCh0 = 1;

        pTempFirstDescriptor = pFirstDescriptorCh0;

        /* if Descriptors Already Exist */

        if (((UINT32)pTempFirstDescriptor & MPC107_DMA_NDAR_ADDR_MASK) != 0)
            {

            while (((UINT32)pTempFirstDescriptor &
                    MPC107_DMA_NDAR_ADDR_MASK) != NULL )
                {

                pTempDescriptor = pTempFirstDescriptor;

                /*
                 * Disable End of Segment Interrupt &
                 * Disable End of Transfer Interrupt
                 * For each descriptor except the last one
                 */

                pTempFirstDescriptor -> pNextDescriptorAddress = 
                   (MPC107_DMA_DESCRIPTOR *)
                      ((UINT32)pTempFirstDescriptor -> pNextDescriptorAddress &
                       (!((UINT32)MPC107_DMA_NDAR_EOTD)  &
                        !((UINT32)MPC107_DMA_NDAR_NDEOSIE)));

                pTempFirstDescriptor = pTempFirstDescriptor
                                    -> pNextDescriptorAddress;

                }


            /* Next Descriptor */

            pTempDescriptor  -> pNextDescriptorAddress  =
                (MPC107_DMA_DESCRIPTOR *) memalign(MPC107_DMA_MEM_ALIGN,
                sizeof(MPC107_DMA_DESCRIPTOR));

            /* Flush the data cache */

            cacheFlush (DATA_CACHE, pTempDescriptor  -> pNextDescriptorAddress,
                       sizeof(MPC107_DMA_DESCRIPTOR));

            /* Source  Address for the Next Descriptor */

            pTempDescriptor  -> pNextDescriptorAddress
                             -> pSourceAddress  = (UCHAR *)pSourceAddress;

            /* Destinantion  Address for the Next Descriptor */

            pTempDescriptor  -> pNextDescriptorAddress
                             -> pDestinationAddress  =
                               (UCHAR *)pDestinationAddress;


            /* Number of bytes  for the Next Descriptor */

            pTempDescriptor  -> pNextDescriptorAddress
                             -> numberOfBytes  = numberOfBytes;

            /*
             * Disable End of Segment Interrupt &
             * Enable End of Transfer Interrupt
             */

            pTempDescriptor  -> pNextDescriptorAddress
                             -> pNextDescriptorAddress
                             = (MPC107_DMA_DESCRIPTOR *) MPC107_DMA_NDAR_EOTD;

            }
        else            /* Descriptors donot exist */
            {

            /* First  Descriptor */

            pFirstDescriptorCh0 = (MPC107_DMA_DESCRIPTOR *)
            memalign(MPC107_DMA_MEM_ALIGN, sizeof(MPC107_DMA_DESCRIPTOR));

            /* Flush the data cache */

            cacheFlush (DATA_CACHE, pFirstDescriptorCh0,
                        sizeof(MPC107_DMA_DESCRIPTOR));

            /* Source  Address for the Current Descriptor */

            pFirstDescriptorCh0 -> pSourceAddress  = (UCHAR *)pSourceAddress;

            /* Destinantion Address for the Current Descriptor */

            pFirstDescriptorCh0 -> pDestinationAddress =
                                   (UCHAR *) pDestinationAddress;

            /* Number of Bytes  for the current Descriptor */

            pFirstDescriptorCh0  -> numberOfBytes  = numberOfBytes;

            /*
             * Disable End of Segment Interrupt &
             * Enable End of Transfer Interrupt
             */

            pFirstDescriptorCh0  -> pNextDescriptorAddress =
            (MPC107_DMA_DESCRIPTOR *)MPC107_DMA_NDAR_EOTD;

            /*
             * Set a Global Flag to indicate that the descriptors are being
             * built afresh again or are being built for the first time
             */

            chainedDma0FirstTime = TRUE;


            }

        /* Flag cleared to indicate that the descriptors have been  built  */

        dmaBuildDescrInProgCh0 = 0;

        semGive (semIdChainDmaCh0);

        }

    else if (channelNumber == MPC107_DMA_CHANNEL1) /* channel 1 */
        {

        statusOfDmaReg = MPC107EUMBBARREAD(MPC107_DMA_DMR1);

        if ((!(statusOfDmaReg & MPC107_DMA_DMR_CTM) &&
              (statusOfDmaReg & MPC107_DMA_DMR_PDE)) != 0)
            {

            /* This channel is configured for Periodic DMA */

            errnoSet(EBUSY);
            return (ERROR);

            }

        semTake (semIdChainDmaCh1, WAIT_FOREVER);

        /* if Direct DMA is configured */

        if ((statusOfDmaReg & MPC107_DMA_DMR_CTM) != 0)
            {

            /* Polling to see when the channel is free */

            while (MPC107EUMBBARREAD(MPC107_DMA_DSR1) & MPC107_DMA_DSR_CB);

            }


        /* Flag set to indicate that the  descriptors are  being built */

        dmaBuildDescrInProgCh1 = 1;

        pTempFirstDescriptor =  pFirstDescriptorCh1;

        /* If Descriptors Already Exist */

        if (((UINT32)pTempFirstDescriptor & MPC107_DMA_NDAR_ADDR_MASK) != 0)
            {

            while (((UINT32)pTempFirstDescriptor &
                                  MPC107_DMA_NDAR_ADDR_MASK) != NULL )
                {

                pTempDescriptor = pTempFirstDescriptor;

                /*
                 * Disable End of Segment Interrupt &
                 * Disable End of Transfer Interrupt
                 * For each descriptor except the last one
                 */

                pTempFirstDescriptor -> pNextDescriptorAddress = 
                   (MPC107_DMA_DESCRIPTOR *)
                      ((UINT32)pTempFirstDescriptor -> pNextDescriptorAddress &
                       (!(UINT32)MPC107_DMA_NDAR_EOTD &
                        (UINT32)MPC107_DMA_NDAR_NDEOSIE));

                pTempFirstDescriptor = pTempFirstDescriptor ->
                                       pNextDescriptorAddress;

                }


            /* Next Descriptor */

            pTempDescriptor  -> pNextDescriptorAddress
                               = (MPC107_DMA_DESCRIPTOR *)memalign
                                 (MPC107_DMA_MEM_ALIGN,
                                 sizeof(MPC107_DMA_DESCRIPTOR));

            /* Flush the data cache */

            cacheFlush (DATA_CACHE, pTempDescriptor  -> pNextDescriptorAddress,
                       sizeof(MPC107_DMA_DESCRIPTOR));

            /* Source  Address for the Next Descriptor */

            pTempDescriptor  -> pNextDescriptorAddress
                             -> pSourceAddress
                             = (UCHAR*)pSourceAddress;

            /* Destinantion  Address for the Next Descriptor */

            pTempDescriptor  -> pNextDescriptorAddress
                             -> pDestinationAddress
                             = (UCHAR*) pDestinationAddress;

            /* Number of bytes  for the Next Descriptor */

            pTempDescriptor  -> pNextDescriptorAddress
                             -> numberOfBytes  = numberOfBytes;

            /*
             * Disable End of Segment Interrupt &
             * Enable End of Transfer Interrupt
             */

            pTempDescriptor  -> pNextDescriptorAddress
                             -> pNextDescriptorAddress
                             = (MPC107_DMA_DESCRIPTOR *)MPC107_DMA_NDAR_EOTD;


            }
        else            /* Descriptors donot exist */
            {

            /* First  Descriptor */

            pFirstDescriptorCh1 = memalign(MPC107_DMA_MEM_ALIGN,
                                  sizeof(MPC107_DMA_DESCRIPTOR));

            /* Flush the data cache */

            cacheFlush (DATA_CACHE, pFirstDescriptorCh1,
                                    sizeof(MPC107_DMA_DESCRIPTOR));

            /* Source  Address for the Current Descriptor */

            pFirstDescriptorCh1 -> pSourceAddress  =
                                   (UCHAR *)pSourceAddress;

            /* Destinantion Address for the Current Descriptor */

            pFirstDescriptorCh1 -> pDestinationAddress =
                                   (UCHAR *)pDestinationAddress;

            /* Number of Bytes  for the current Descriptor */

            pFirstDescriptorCh1  -> numberOfBytes  = numberOfBytes;

            /*
             * Disable End of Segment Interrupt &
             * Enable End of Transfer Interrupt
             */

            pFirstDescriptorCh1  -> pNextDescriptorAddress =
                (MPC107_DMA_DESCRIPTOR*)MPC107_DMA_NDAR_EOTD;

            /*
             * Set a Global Flag to indicate that the descriptors are being
             * built afresh again or are being built for the first time
             */

            chainedDma1FirstTime = TRUE;

            }

        /* Flag cleared to indicate that the  descriptors have been  built */

        dmaBuildDescrInProgCh1 = 0;

        semGive (semIdChainDmaCh1);

        }
    else
        {
        errnoSet (EINVAL);
        return (ERROR);
        }

    return (OK);

    }

/***************************************************************************
*
* mpc107DmaStatus  -  query DMA status and DMA Transfer Mode
*
* This  routine purpose is to query the  specified DMA channel for DMA
* transfer status and DMA transfer mode.
*
* RETURNS :
* This routine returns one of the following :
* MPC107_DMA_PERIODIC_CH_BUSY  or  MPC107_DMA_PERIODIC_CH_FREE or
* MPC107_DMA_CHAIN_CH_BUSY  or  MPC107_DMA_CHAIN_CH_FREE or
* MPC107_DMA_DIRECT_CH_BUSY  or   MPC107_DMA_CHAIN_CH_FREE or
* MPC107_DMA_UNDEF_CH
*/

INT32 mpc107DmaStatus
    (
    ULONG 	channel           /* Channel Number */
    )
    {
    ULONG 	dmaModeRegRead;   /* Data read from the DMA Mode register */
    UINT32 	dmaStatusReg;     /* DMA status Register */
    UINT32 	dmaModeReg;       /* DMA mode Register */

    if (channel == MPC107_DMA_CHANNEL0) /*  Channel 0 */
        {

        dmaStatusReg = (UINT32)MPC107_DMA_DSR0; /* Status Register of Ch0 */

        dmaModeReg = (UINT32)MPC107_DMA_DMR0;   /* Mode Register of Ch0 */

        }
    else if (channel == MPC107_DMA_CHANNEL1) /* Channel 1 */
        {

        dmaStatusReg = (UINT32)MPC107_DMA_DSR1; /* Status Register of Ch1 */

        dmaModeReg = (UINT32)MPC107_DMA_DMR1;    /* Mode Register of Ch1 */

        }
    else
        return (MPC107_DMA_UNDEF_CH); /* If the Channel is  undefined */



    dmaModeRegRead = MPC107EUMBBARREAD(dmaModeReg);


    /* if Chained DMA is configured */

    if ((dmaModeRegRead & MPC107_DMA_DMR_CTM) == 0 )
        {

        /* if Periodic  DMA is configured */

        if ((dmaModeRegRead & MPC107_DMA_DMR_PDE) != 0)
            {

            if ((MPC107EUMBBARREAD(dmaStatusReg) & MPC107_DMA_DSR_CB) != 0)
                {

                /*
                 * Periodic Chained DMA is configured
                 * and Channel Is Busy
                 */

                return (MPC107_DMA_PERIODIC_CH_BUSY);

                }
            else
                {

                /*
                 * Periodic Chained DMA is configured
                 * and Channel Is Not Busy (FREE)
                 */

                return (MPC107_DMA_PERIODIC_CH_FREE);
                }

            }

        else
            {

            if ((MPC107EUMBBARREAD(dmaStatusReg) & MPC107_DMA_DSR_CB) != 0)
                {

                /* Chained DMA is configured  and Channel Is Busy  */

                return (MPC107_DMA_CHAIN_CH_BUSY);

                }
            else
                {

                /*
                 * Chained DMA is configured and
                 * Channel Is Not Busy (FREE)
                 */

                return (MPC107_DMA_CHAIN_CH_FREE);
                }

            }

        }

        /* if Direct DMA is configured */

    else if ((dmaModeRegRead & MPC107_DMA_DMR_CTM) != 0)
        {

        if ((MPC107EUMBBARREAD(dmaStatusReg) & MPC107_DMA_DSR_CB) != 0)
            {

            /* Direct  DMA is configured  and Channel Is Busy  */

            return (MPC107_DMA_DIRECT_CH_BUSY);

            }
        else
            {

            /*
             * Direct  DMA is configured and
             * Channel Is Not Busy (FREE)
             */

            return (MPC107_DMA_DIRECT_CH_FREE);

            }

        }

    return (OK);

    }


/***************************************************************************
*
* mpc107DmaCh0Int -  ISR Handler for DMA channel 0
*
* This routine services the interrupts of DMA channel 0.
*
* RETURNS: N/A
*/

void mpc107DmaCh0Int (void)
    {
    MPC107_DMA_DESCRIPTOR *	pTempDescriptor;
    MPC107_DMA_DESCRIPTOR *	pTempFirstDescriptor;
    ULONG 		dataReadDmaReg;
    ULONG 		dataReadTemp;


    /* Read the status Register of DMA Channel 0 */

    dataReadDmaReg = MPC107EUMBBARREAD(MPC107_DMA_DSR0);


    /* if End of Chain / Direct Transfer Interrupt received */

    if ((dataReadDmaReg & MPC107_DMA_DSR_EOCAI) != 0)
        {

        dataReadTemp = MPC107EUMBBARREAD(MPC107_DMA_DMR0);

        /* if End of Interrupt Enabled */

        if ((dataReadTemp & MPC107_DMA_DMR_EOTIE) != 0)
            {

            if ((!(dataReadDmaReg & MPC107_DMA_DMR_CTM) &&
                  (dataReadDmaReg & MPC107_DMA_DMR_PDE)) != 0)
                {

                /* This channel is configured for Periodic DMA */



                }
            else if ( !(dataReadDmaReg & MPC107_DMA_DMR_CTM) &&
                      !(dmaBuildDescrInProgCh0))
                {

                /* This channel is configured for Chained DMA */


                /* Release all the Descriptors */

                pTempFirstDescriptor = pFirstDescriptorCh0;

                /* Descriptors Already Exist */

                if (((UINT32)pTempFirstDescriptor &
                     MPC107_DMA_NDAR_ADDR_MASK) != 0)
                    {


                    while (((UINT32)pTempFirstDescriptor  &
                            MPC107_DMA_NDAR_ADDR_MASK) != NULL )
                        {

                        pTempDescriptor = pTempFirstDescriptor;


                        pTempFirstDescriptor = pTempFirstDescriptor
                                             -> pNextDescriptorAddress;

                        free ((void *)((UINT32)pTempDescriptor &
                               MPC107_DMA_NDAR_ADDR_MASK));

                        }
                    }

                    pFirstDescriptorCh0 = NULL;

                    chainedDma0FirstTime = FALSE; /* Reset the flag */

                }

            }

        }


    /* if End of Segment Transfer Interrupt received */

    if ((dataReadDmaReg & MPC107_DMA_DSR_EOSI) != 0)
        {

        dataReadTemp = MPC107EUMBBARREAD(MPC107_DMA_CDAR0) &
                                         MPC107_DMA_CDAR_EOSIE;

        /* if End Of Segment Interrupt is Enabled  */

        if (dataReadTemp != 0)
            {

            }

        }


    /* If PCI Error  Interrupt received */

    if ((dataReadDmaReg & MPC107_DMA_DSR_PE) != 0)
        {

        dataReadTemp = MPC107EUMBBARREAD(MPC107_DMA_DMR0) &
                                         MPC107_DMA_DMR_EIE;

        /* If Error  Interrupts are  Enabled  */

        if (dataReadTemp != 0 )
            {

            }

        }


    /* If Local Memory Error   Interrupt received */

    if ((dataReadDmaReg & MPC107_DMA_DSR_LME) != 0)
        {

        dataReadTemp = MPC107EUMBBARREAD(MPC107_DMA_DMR0) &
                                         MPC107_DMA_DMR_EIE;

        /* If Error  Interrupts are  Enabled  */

        if (dataReadTemp != 0)
            {

            }

        }

    }


/***************************************************************************
*
* mpc107DmaCh1Int -  ISR Handler for DMA channel 1
*
* This routine services the interrupts of DMA channel 1.
*
* RETURNS: N/A
*/

void mpc107DmaCh1Int (void)
    {
    MPC107_DMA_DESCRIPTOR *	pTempDescriptor;
    MPC107_DMA_DESCRIPTOR *	pTempFirstDescriptor;
    ULONG 		dataReadDmaReg;
    ULONG 		dataReadTemp;

    /* Read the status Register of DMA Channel 1 */

    dataReadDmaReg = MPC107EUMBBARREAD(MPC107_DMA_DSR1);


    /* If End of Chain / Direct Transfer Interrupt received */

    if (dataReadDmaReg & MPC107_DMA_DSR_EOCAI)
        {

        dataReadTemp = MPC107EUMBBARREAD(MPC107_DMA_DMR1);

        /* If End of Interrupt Enabled */

        if ( dataReadTemp & MPC107_DMA_DMR_EOTIE )
            {

            if ( !( dataReadDmaReg & MPC107_DMA_DMR_CTM) &&
                  ( dataReadDmaReg & MPC107_DMA_DMR_PDE))
                {

                /* This channel is configured for Periodic DMA */


                }
            else if ( !( dataReadDmaReg & MPC107_DMA_DMR_CTM)  &&
                      !(dmaBuildDescrInProgCh1))
                {

                /* This channel is configured for Chained DMA */


                /* Release all the Descriptors */

                pTempFirstDescriptor = pFirstDescriptorCh1;

                /* Descriptors Already Exist */

                if (((UINT32)pTempFirstDescriptor & MPC107_DMA_NDAR_ADDR_MASK))
                    {


                    while (((UINT32)pTempFirstDescriptor  &
                            MPC107_DMA_NDAR_ADDR_MASK) != NULL )
                        {

                        pTempDescriptor = pTempFirstDescriptor;

                        pTempFirstDescriptor = pTempFirstDescriptor
                                        -> pNextDescriptorAddress;

                        free ((void *) ((UINT32)pTempDescriptor &
                                         MPC107_DMA_NDAR_ADDR_MASK));

                        }
                    }

                    pFirstDescriptorCh1 = NULL;

                    chainedDma1FirstTime = FALSE; /* Reset the flag */


                }


            }

        }


    /* If End of Segment Transfer Interrupt received */

    if (dataReadDmaReg & MPC107_DMA_DSR_EOSI)
        {

        dataReadTemp = MPC107EUMBBARREAD(MPC107_DMA_CDAR1) &
                                         MPC107_DMA_CDAR_EOSIE;

        /* If End Of Segment Interrupt is Enabled  */

        if (dataReadTemp)
            {

            }

        }


    /* If PCI Error  Interrupt received */

    if (dataReadDmaReg & MPC107_DMA_DSR_PE)
        {

        dataReadTemp = MPC107EUMBBARREAD(MPC107_DMA_DMR1) &
                                         MPC107_DMA_DMR_EIE;

        /* If Error  Interrupts are  Enabled  */

        if (dataReadTemp)
            {

            }

        }


    /* If Local Memory Error   Interrupt received */

    if (dataReadDmaReg & MPC107_DMA_DSR_LME)
        {

        dataReadTemp = MPC107EUMBBARREAD(MPC107_DMA_DMR1) &
                                         MPC107_DMA_DMR_EIE;

        /* If Error  Interrupts are  Enabled  */

        if (dataReadTemp)
            {

            }

        }

    }


