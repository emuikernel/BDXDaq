/* sysMv64260Dma.c - Support for the MV64260 DMA Controller */

/* Copyright 2002 Motorola, Inc., All Rights Reserved */

/*
modification history
--------------------
01c,05jun03,yyz  Added support for IDMA chain mode.
01b,02jun02,scb  DMA tuning plus tie source and dest burst limit to 
		 <config.h> file.
01a,08oct02,efb  New. Base on harrierDma.c features (01b,21jan02,mcp820).
*/

/*
DESCRIPTION

MV64260 DMA driver functions are provided to support 
initialization, configuration, start, pause, resume, abort and 
status retrieval of DMA operations on a per channel basis. 
Both block mode and chain mode are supported. 

The DMA initialization routine must be called once before any other DMA
routine is invoked. Failure to do so will result in status error returns
from all other DMA functions. This initialization is handled by sysHwInit2 
whenever INCLUDE_MV64260_DMA is defined in config.h. 

Next the arbiter set routine may be invoked if a custom channel priority
weighting is desired. It is not necessary to program the DMA channel 
arbiter unless something other than the default weighting is needed. The
default is balanced equally across all channels.

To start DMA for a particular channel, invoke the start routine with
the desired addresses, byte count and transfer attributes. These 
parameters are specified via separate descriptor and attribute 
structures. 

DMA is supported from DRAM to DRAM, DRAM to PCI, PCI to DRAM, and PCI to 
PCI by appropriate selection of the source and destination addresses 
supplied in the DMA descriptor. 

The source address and/or destation address may be optionally specified
to not increment (i.e. hold) throughout the transfer. In this mode, the
source and destation address starting points must be aligned to the 
burst limit. Even though the addresses are held, a full burst count of bytes
will always be transferred. The burst limit is specified in the 
<board>A.h file.

The user interrupt handler, if provided, must conform to the rules for 
standard VxWorks interrupt service routines. It is called with a current
channel parameter during handling of both normal completion and error 
interrupts. Since this routine is called by the driver interrupt handlers,
the user routine is not responsible for clearing interrupts. Interrupts 
are cleared after the user routine returns to the driver's handler.

.CS
  sysMv64260DmaArbiterSet 
  sysMv64260DmaAbort
  sysMv64260DmaInit
  sysMv64260DmaPause
  sysMv64260DmaResume 
  sysMv64260DmaStart 
  sysMv64260DmaStatus
.CE

*/

/* includes */

#include "vxWorks.h"
#include "config.h"
#include "mv64260Dma.h"

/* defines */

#define IDMA_REG_SZ sizeof(UINT32)
#define IDMA_CHAN_IS_ACTIVE(_X_) (((_X_) & IDMA_CTRL_CHAN_ACTIVE_MASK) == \
                                  IDMA_CTRL_CHAN_ACTIVE_MASK)

/* typedefs */

/*
 * The following private structure defines the control elements for a 
 * given DMA channel. The register address offsets are used to 
 * conveniently access a particular channel's DMA descriptor and 
 * control registers. 
 */

typedef struct mv64260DmaDrvCtrl
    {
    UINT32  byteCountRegOff;     /* offset of DMA byte count register */
    UINT32  srcAddrRegOff;       /* offset of DMA source address register */
    UINT32  srcPciHiAddrRegOff;  /* offset of DMA source address register */
    UINT32  destAddrRegOff;      /* offset of DMA dest address register */
    UINT32  destPciHiAddrRegOff; /* offset of DMA source address register */
    UINT32  nextDescPtrRegOff;   /* offset of the next desc ptr register */
    UINT32  nextPciHiAddrRegOff; /* offset of DMA source address register */
    UINT32  currDescPtrRegOff;   /* offset of the current desc ptr register */
    UINT32  chanCtrlLowRegOff;   /* offset of the channel control register */
    UINT32  chanCtrlHiRegOff;    /* offset of the channel control register */
    HANDLER userHandler;         /* user supplied notification routine */
    SEM_ID  semDrvId;            /* driver access semaphore */
    SEM_ID  semIntId;            /* interrupt interface semaphore */
    STATUS  dmaStatus;           /* DMA completion status */
    UINT32  errSel;              /* DMA error select code */
    UINT32  errAddr;             /* address causing the error */
    } IDMA_DRV_CTRL;

/* globals */

/* locals */

LOCAL IDMA_DRV_CTRL mv64260DmaCtrl[IDMA_CHAN_CNT];
LOCAL BOOL isInitialized = FALSE;

/* forward declarations */

/* external references */

IMPORT void sysDelay (void);





/*** SERGEY ***/

/* copied from sysMv64260InitAddrWindows() and modified */

LOCAL void
sysMv64260InitAddrWindows (void)
{
  return;
}

/*** SERGEY ***/







/**************************************************************************
*
* mv64260DmaHandleInt - Handle a normal completion interrupt.
*
* This function handles a normal DMA completion interrupt for a particular
* channel pair. The channel number passed in is always the lower channel
* number of the pair (0, 2, 4, or 6). The interrupt is verified to have 
* occurred for each channel of the pair sequentially. The channel
* status is set to OK to indicate successful completion. The semaphore is
* given to the DMA start routine if userHandler is set to WAIT_FOREVER.
* Otherwise if the userHandler is non-null, the userHandler routine is
* invoked with the channel number. The interrupt is cleared by writing a
* zero to the appropriate channel completion bit in the interrupt cause
* register. Other channel's bits are written as ones to preserve their 
* state (writing a one has no effect).
*
* RETURNS: N/A
*/

LOCAL void mv64260DmaHandleInt 
    (
    UINT32   channel		/* Interrupting channel */
    )
    {
    IDMA_DRV_CTRL *dmaChanCtrl = NULL; 
    UINT32 intCause = 0xFFFFFFFF;
    UINT32 completionMask = 0;
    UINT32 chan = 0;
    UINT32 chanGroup = 0;
    UINT32 chanShift = 0;
    UINT32 errorMask = 0;
    UINT32 errorCode = 0;
    UINT32 intCauseAddrOffset = 0;
 
    /* Check two channels since there are two channels per interrupt */

    chanGroup = channel < IDMA_CHAN_GRP_SZ ? 0 : 1;

    for (chan = channel; chan <= channel + 1; ++chan)
        { 
        dmaChanCtrl = &mv64260DmaCtrl[chan]; 

        chanShift = IDMA_INT_CAUSE_CHAN1_5_BIT * 
                    (chan - (chanGroup * IDMA_CHAN_GRP_SZ));

        completionMask = IDMA_INT_CAUSE_DMA_COMPL_MASK << chanShift;

        errorMask = IDMA_INT_CAUSE_CHAN_ERR_MASK << chanShift;

        intCauseAddrOffset = IDMA_INT_CAUSE_CHAN0_3 + 
                             (chanGroup * IDMA_CHAN_GRP_INTERVAL);

        errorCode = MV64260_READ32 (MV64260_REG_BASE, 
                                    intCauseAddrOffset) & errorMask;

        /* Check channel to be sure interrupt actually occurred */

        if ((MV64260_READ32 (MV64260_REG_BASE, intCauseAddrOffset) & 
            completionMask) != 0)
            {
            if (errorCode == 0)
                {
                dmaChanCtrl->dmaStatus = OK;
                }

            /*
             * If the user handler for the channel is set to WAIT_FOREVER
             * then give the semaphore to the waiting DMA start. If the user
             * handler is not WAIT_FOREVER, but non-null, invoke the user
             * routine with the channel number. If the handler is null
             * continue directly to clear the interrupt.
             */ 

            if ((int)(dmaChanCtrl->userHandler) == WAIT_FOREVER)
                {    
	        semGive (dmaChanCtrl->semIntId);
                }
            else if (dmaChanCtrl->userHandler != NULL)
                {
                (dmaChanCtrl->userHandler) (chan);
                }

            /*
             * Write out cleared interrupt field to clear the completion 
             * interrupt for the given channel only.
             */

            intCause &= ~completionMask;
            MV64260_WRITE32_PUSH (MV64260_REG_BASE, intCauseAddrOffset,
                                  intCause);
            }
        }
    }

/**************************************************************************
*
* mv64260DmaHandleErrInt - Handle an error interrupt.
*
* This function handles a DMA error interrupt. Since the interrupt is shared
* across all channels, all channels must be checked for an error status by
* examining the error bits in the interrupt cause register. If the error
* code is not zero, dmaStatus is set to ERROR and the code is saved for 
* that channel. If the error select register matches the error code, the 
* error address is also saved for that channel. The semaphore is then 
* given to the waiting start routine if userHandler is set to 
* WAIT_FOREVER. Otherwise if the userHandler is non-null, the userHandler
* routine is invoked with the channel number. After checking all channels,
* the interrupts are cleared by writing zeros to all of the 
* interrupting channels' error bits in the interrupt cause register. Other 
* channels' bits are written as ones to preserve their state (writing a 
* one has no effect).
*
* RETURNS: N/A
*/

LOCAL void mv64260DmaHandleErrInt (void)
    {
    UINT32 chan = 0;
    IDMA_DRV_CTRL *dmaChanCtrl = NULL; 
    UINT32 errorMask = 0;
    UINT32 errorCode = 0;
    UINT32 errorChan = 0;
    UINT32 errorCause = 0xFFFFFFFF;
    UINT32 intCauseAddrOffset = 0;
    UINT32 errOffset = 0;
    UINT32 chanGroup = 0;
    UINT32 chanShift = 0;

    /* Check each channel error interrupt group */

    for (chan = 0; chan < IDMA_CHAN_CNT; ++chan)
        {
        dmaChanCtrl = &mv64260DmaCtrl[chan]; 
        errorCause = 0xFFFFFFFF;

        chanGroup = chan < IDMA_CHAN_GRP_SZ ? 0 : 1;

        chanShift = IDMA_INT_CAUSE_CHAN1_5_BIT * 
                    (chan - (chanGroup * IDMA_CHAN_GRP_SZ));

        intCauseAddrOffset = IDMA_INT_CAUSE_CHAN0_3 + 
                             (chanGroup * IDMA_CHAN_GRP_INTERVAL);

        errorMask = IDMA_INT_CAUSE_CHAN_MASK << chanShift;

        errorCode = MV64260_READ32 (MV64260_REG_BASE, 
                                    intCauseAddrOffset) & errorMask;

        /*
         * If the error code is not zero, record the error status for the
         * channel. Save the error address if one is available for the
         * given error on the interrupting channel. Bits 3 & 4 of the 
         * error select code contain the erroring channel number. This
         * should match the channel in the interrupt cause register for
         * the error address to be meaningful. 
         */

        if (errorCode != 0)
            {
            errOffset = IDMA_ERR_SEL_CHAN0_3 + 
                        (chanGroup * IDMA_CHAN_GRP_INTERVAL);

            dmaChanCtrl->dmaStatus = ERROR;
            dmaChanCtrl->errSel = MV64260_READ32 (MV64260_REG_BASE,
                                                  errOffset);

            errorChan = ((dmaChanCtrl->errSel & IDMA_ERR_SEL_CHAN_MASK) >> 
                IDMA_ERR_SEL_CHAN_BIT) + (chanGroup * IDMA_CHAN_GRP_SZ);

            if (chan == errorChan)
                {
                errOffset = IDMA_ERR_ADDR_CHAN0_3 + 
                            (chanGroup * IDMA_CHAN_GRP_INTERVAL);

                dmaChanCtrl->errAddr = MV64260_READ32 (MV64260_REG_BASE, 
                                                       errOffset);
                }
            else
                {
                dmaChanCtrl->errAddr = 0;
                }

            /* Clear the error interrupt field */
 
            errorCause &= ~errorMask;

            if ((int)(dmaChanCtrl->userHandler) == WAIT_FOREVER)
                {    
                semGive (dmaChanCtrl->semIntId);
                }
            else if (dmaChanCtrl->userHandler != NULL)
                {
                (dmaChanCtrl->userHandler) (chan);
                }

            /* Write out cleared interrupt fields to clear the interrupts */

            MV64260_WRITE32_PUSH (MV64260_REG_BASE, intCauseAddrOffset, 
                                  errorCause);
            }
        }
    }

/******************************************************************************
*
* sysMv64260DmaInit - Initialize DMA and attach DMA Interrupt Handler.
*
* This function configures the DMA driver for all available DMA channels.
* The driver tracks information on each channel independently. Each channel
* control structure is set up to allow easy indexing by channel from the 
* driver routines. Register defaults are also initialized.
*
* An interrupt handler is connected to each even DMA completion interrupt 
* number. A different handler is connected to the DMA error interrupt. This 
* routine must be called once for DMA capability to be properly initialized 
* before any other driver routine is called. 
*
* RETURNS: OK if initialization succeeded or ERROR if an error occurred.
*/

STATUS
sysMv64260DmaInit (void)
{
  UINT32 chan = 0;
  UINT32 chanGroup = 0;
  UINT32 chanOffset = 0;
  UINT32 chanRegOffset = 0;
  IDMA_DRV_CTRL *dmaChanCtrl = NULL; 
  STATUS status = OK;
  UINT32 arbSliceDefault[] = { 0, 1, 2, 3, 0, 1, 2, 3, 
                               0, 1, 2, 3, 0, 1, 2, 3 };
  char txt[100];

  /* Set all unused registers to defaults */

  MV64260_WRITE32_PUSH (MV64260_REG_BASE, IDMA_XBAR_TMOUT_CHAN0_3, 
                        IDMA_XBAR_TMOUT_PRESET |
                        IDMA_XBAR_TMOUT_DISABLE);
    
  MV64260_WRITE32_PUSH (MV64260_REG_BASE, IDMA_XBAR_TMOUT_CHAN4_7, 
                        IDMA_XBAR_TMOUT_PRESET |
                        IDMA_XBAR_TMOUT_DISABLE);
    
  /* Ensure all interrupts are cleared */

  MV64260_WRITE32_PUSH (MV64260_REG_BASE, IDMA_INT_CAUSE_CHAN0_3, 0x00);
  MV64260_WRITE32_PUSH (MV64260_REG_BASE, IDMA_INT_MASK_CHAN0_3, 0x00);
  MV64260_WRITE32_PUSH (MV64260_REG_BASE, IDMA_INT_CAUSE_CHAN4_7, 0x00);
  MV64260_WRITE32_PUSH (MV64260_REG_BASE, IDMA_INT_MASK_CHAN4_7, 0x00);

  /* Initialize the DMA control array for all channels */

  for(chan = 0; chan < IDMA_CHAN_CNT; ++chan)
  {
    dmaChanCtrl = &mv64260DmaCtrl[chan];

    /* Set up the register offsets per channel for easy access later. */
     
    chanGroup = chan < IDMA_CHAN_GRP_SZ ? 0 : 1;
    chanOffset = (chan - (chanGroup * IDMA_CHAN_GRP_SZ)) * IDMA_REG_SZ;

    chanRegOffset = (chanGroup * IDMA_CHAN_GRP_INTERVAL) + chanOffset;

    dmaChanCtrl->byteCountRegOff = IDMA_BYTE_CNT_CHAN0 + chanRegOffset;

    dmaChanCtrl->srcAddrRegOff = IDMA_SRC_ADDR_CHAN0 + chanRegOffset;

    dmaChanCtrl->srcPciHiAddrRegOff = IDMA_SRC_HI_PCI_ADDR_CHAN0 + 
                                          chanRegOffset;

    dmaChanCtrl->destAddrRegOff = IDMA_DEST_ADDR_CHAN0 + chanRegOffset;

    dmaChanCtrl->destPciHiAddrRegOff = IDMA_DEST_HI_PCI_ADDR_CHAN0 + 
                                           chanRegOffset;

    dmaChanCtrl->nextDescPtrRegOff = IDMA_NXT_DESC_PTR_CHAN0 + 
                                         chanRegOffset;

    dmaChanCtrl->nextPciHiAddrRegOff = IDMA_NXT_DESC_HI_PCI_ADDR_CHAN0 +
                                           chanRegOffset;

    dmaChanCtrl->currDescPtrRegOff = IDMA_CURR_DESC_PTR_CHAN0 + 
                                         chanRegOffset;

    dmaChanCtrl->chanCtrlLowRegOff = IDMA_CTRL_LO_CHAN0 + chanRegOffset;
    dmaChanCtrl->chanCtrlHiRegOff = IDMA_CTRL_HI_CHAN0 + chanRegOffset;

    dmaChanCtrl->userHandler = NULL;

    dmaChanCtrl->semDrvId = semBCreate (SEM_Q_PRIORITY, SEM_FULL);
    dmaChanCtrl->semIntId = semBCreate (SEM_Q_FIFO, SEM_EMPTY);

    /* Clear the error and status information */

    dmaChanCtrl->dmaStatus = ERROR;
    dmaChanCtrl->errSel = 0;
    dmaChanCtrl->errAddr = 0;

    MV64260_READ32 (MV64260_REG_BASE, IDMA_ERR_ADDR_CHAN0_3);
    MV64260_READ32 (MV64260_REG_BASE, IDMA_ERR_ADDR_CHAN4_7);
    MV64260_READ32 (MV64260_REG_BASE, IDMA_ERR_SEL_CHAN0_3);
    MV64260_READ32 (MV64260_REG_BASE, IDMA_ERR_SEL_CHAN4_7);
    MV64260_WRITE32_PUSH (MV64260_REG_BASE, IDMA_ERR_ADDR_CHAN0_3, 0);
    MV64260_WRITE32_PUSH (MV64260_REG_BASE, IDMA_ERR_ADDR_CHAN4_7, 0);
    MV64260_WRITE32_PUSH (MV64260_REG_BASE, IDMA_ERR_SEL_CHAN0_3, 0);
    MV64260_WRITE32_PUSH (MV64260_REG_BASE, IDMA_ERR_SEL_CHAN4_7, 0);
            
    /* Initialize the fixed control settings and defaults */

    MV64260_WRITE32_PUSH (MV64260_REG_BASE, 
                          dmaChanCtrl->chanCtrlLowRegOff, 
                          IDMA_DST_BURST_LIMIT |
                          IDMA_SRC_BURST_LIMIT |
                          IDMA_CTRL_BLOCK_MODE |
                          IDMA_CTRL_NON_CHAIN_MODE |
						  IDMA_CTRL_INT_MODE |
                          IDMA_CTRL_DESC_16MB);

#ifdef IDMA_SNOOP_ON
        MV64260_WRITE32_PUSH (MV64260_REG_BASE,
                              dmaChanCtrl->chanCtrlHiRegOff, 
                              IDMA_CTRL_SRC_SNOOP_ENB_MASK |
                              IDMA_CTRL_DEST_SNOOP_ENB_MASK |
                              IDMA_CTRL_NXT_SNOOP_ENB_MASK);
#else
        MV64260_WRITE32_PUSH (MV64260_REG_BASE,
                              dmaChanCtrl->chanCtrlHiRegOff, 0);
#endif /* IDMA_SNOOP_ON */

        MV64260_WRITE32_PUSH (MV64260_REG_BASE,
                              dmaChanCtrl->byteCountRegOff, 0); 

        MV64260_WRITE32_PUSH (MV64260_REG_BASE,
                              dmaChanCtrl->srcAddrRegOff, 0x00);

        MV64260_WRITE32_PUSH (MV64260_REG_BASE,
                              dmaChanCtrl->srcPciHiAddrRegOff, 0x00);

        MV64260_WRITE32_PUSH (MV64260_REG_BASE,
                              dmaChanCtrl->destAddrRegOff, 0x00);

        MV64260_WRITE32_PUSH (MV64260_REG_BASE,
                              dmaChanCtrl->destPciHiAddrRegOff, 0x00);

        MV64260_WRITE32_PUSH (MV64260_REG_BASE,
                              dmaChanCtrl->currDescPtrRegOff, 0x00);

        MV64260_WRITE32_PUSH (MV64260_REG_BASE,
                              dmaChanCtrl->nextPciHiAddrRegOff, 0x00);

        MV64260_WRITE32_PUSH (MV64260_REG_BASE,
                              dmaChanCtrl->nextDescPtrRegOff, 0x00);
        }

    /*
     * Connect the same interrupt handler to each interrupt number.
     * There are four main interrupts for eight channels, so only four
     * connections are made. The interrupt handler will process
     * consecutive channel pairs. 
     */

    for (chan = 0; chan < IDMA_CHAN_CNT; chan += 2)
        {
        status = intConnect (INUM_TO_IVEC (IDMA_CHAN0_1_COMPLETION_INT + 
                             (chan / 2)), mv64260DmaHandleInt, (int)chan);

        if (status == OK)
            {
            intEnable (IDMA_CHAN0_1_COMPLETION_INT + (chan / 2));
            }
        }

    /*
     * If status is okay, initialize the channel arbiters and connect 
     * the DMA error interrupt. 
     */

    if (status == OK)
    {
      status = intConnect (INUM_TO_IVEC (IDMA_ERR_INT), 
                           mv64260DmaHandleErrInt, 0);
        
      if (status == OK)
      {
        intEnable (IDMA_ERR_INT);

        /* Sergey: add following function in an attempt to make DMA work */
        sysMv64260InitAddrWindows (); 

        sysMv64260DmaArbiterSet (0, arbSliceDefault); 
        sysMv64260DmaArbiterSet (1, arbSliceDefault); 
      }
    }

    isInitialized = (status == OK);
	/*
sprintf(txt,"DmaInit: status=%d\r\n ",status);
sysDebugMsg(txt,CONTINUE_EXECUTION);
	*/
    return (status);
}

/******************************************************************************
*
* sysMv64260DmaStart - Configure and start the DMA controller.
*
* This function sets up the DMA controller for a block mode or chain mode 
* DMA transfer. The user must fill out the DMA descriptor and DMA attribute 
* parameters according to device specifications. 
*
* Transfer options are set according to dmaAttrib. The descriptor registers 
* are programmed according dmaDesc. Interrupts for the given channel are 
* enabled and the transfer is initiated. 
*
* Both block mode and chain mode are supported.  To invoke chain mode, 
* nextDescPtr of dmaDesc must have a nonzero value.  Otherwise, if nextDescPtr 
* of dmaDesc is zero, the function sets up the DMA transfer in block mode.
*
* The user has three options for being notified of the completion of a DMA
* transaction depending on the contents of the userHandler field in the
* dmaAttrib parameter. The first is to have the DMA start routine wait for
* the done interrupt. The start routine then returns with DMA status
* (dmaDesc.userHandler == WAIT_FOREVER).  The second option is to provide a
* user routine for the DMA interrupt handler to call upon completion 
* (dmaDesc.userHandler == user routine). The routine must follow the rules 
* for executing inside an ISR by not calling waits, prints, etc. The 
* sysDmaStart routine returns immediately so the user task can proceed with its
* execution. The third option is for no user interrupt handler, but still
* allows sysDmaStart to return immediately (dmaDesc.userHandler == 0). In
* the later two cases sysDmaStatus can be called later to get the DMA results.
*
* Status is returned by sysDmaStart when the dmaStatus parameter is not
* NULL and when using dmaDesc.userHandler == WAIT_FOREVER or by calling
* sysDmaStatus directly.  In any case, the status should be interpreted 
* using the appropriate device status structure defined in <device>Dma.h.
*
* RETURNS: 
* OK is returned if the channel started successfully.
* ERROR is returned if the driver is not initialized, or the 
* channel is invalid, or the dmaDesc reference is null, or the
* channel is already active and busy.
*/

STATUS
sysMv64260DmaStart(
    UINT32           chan,      /* DMA channel to start */
    IDMA_DESCRIPTOR *dmaDesc,   /* Byte Count, Source, Destination, Next Desc */
    IDMA_ATTRIBUTES *dmaAttrib,	/* Configuration options */
    IDMA_STATUS     *dmaStatus	/* DMA status output */
    )
{
  IDMA_DRV_CTRL *dmaChanCtrl = NULL;
  UINT32 cntlReg = 0;
  UINT32 intMaskAddrOffset = 0;
  UINT32 chanGroup = 0;
  UINT32 chanShift = 0;
  STATUS status = OK;

  if(isInitialized && chan < IDMA_CHAN_CNT && dmaDesc != NULL)
  {
    dmaChanCtrl = &mv64260DmaCtrl[chan]; 

    semTake (dmaChanCtrl->semDrvId, WAIT_FOREVER);
    
    /* Make sure channel is not active and byte count is legal */
    cntlReg = MV64260_READ32 (MV64260_REG_BASE, 
                                  dmaChanCtrl->chanCtrlLowRegOff);

    if (!IDMA_CHAN_IS_ACTIVE (cntlReg) && 
        dmaDesc->byteCount <= IDMA_DESC_16MB_BYTE_CNT_MASK)
    {
     
      /* Set the configuration options */
      dmaChanCtrl->userHandler = NULL;
      cntlReg &= ~IDMA_CTRL_HOLD_SRC_ADDR;
      cntlReg &= ~IDMA_CTRL_HOLD_DEST_ADDR;

      if(dmaAttrib != NULL)
      {
        dmaChanCtrl->userHandler = dmaAttrib->userHandler;

        if(dmaAttrib->holdSourceAddr)
        {
          cntlReg |= IDMA_CTRL_HOLD_SRC_ADDR;
        }
        if(dmaAttrib->holdDestAddr)
        {
          cntlReg |= IDMA_CTRL_HOLD_DEST_ADDR;
        }
      }
 
      /* Set the descriptor registers */
      MV64260_WRITE32_PUSH (MV64260_REG_BASE,
                            dmaChanCtrl->byteCountRegOff, 
                            dmaDesc->byteCount &
                            IDMA_DESC_16MB_BYTE_CNT_MASK); 

      MV64260_WRITE32_PUSH (MV64260_REG_BASE,
                            dmaChanCtrl->srcAddrRegOff, 
                            dmaDesc->sourceAddr);

      MV64260_WRITE32_PUSH (MV64260_REG_BASE,
                            dmaChanCtrl->destAddrRegOff, 
                            dmaDesc->destAddr);

      MV64260_WRITE32_PUSH (MV64260_REG_BASE,
                            dmaChanCtrl->nextDescPtrRegOff, 
				            dmaDesc->nextDescPtr);

      MV64260_WRITE32_PUSH (MV64260_REG_BASE,
                            dmaChanCtrl->currDescPtrRegOff, 
                            (UINT32)dmaDesc);

      /*
       * Reset error state. Status is error until successful 
       * completion.
       */
      dmaChanCtrl->dmaStatus = ERROR;
      dmaChanCtrl->errSel = 0;
      dmaChanCtrl->errAddr = 0;

      /* Unmask/enable all of the given channel's interrupts */
      chanGroup = chan < IDMA_CHAN_GRP_SZ ? 0 : 1;

      chanShift = IDMA_INT_CAUSE_CHAN1_5_BIT * 
                  (chan - (chanGroup * IDMA_CHAN_GRP_SZ));

      intMaskAddrOffset = IDMA_INT_MASK_CHAN0_3 +
                          (chanGroup * IDMA_CHAN_GRP_INTERVAL);

      MV64260_WRITE32_PUSH (MV64260_REG_BASE, intMaskAddrOffset, 
                            MV64260_READ32 (MV64260_REG_BASE, 
                                            intMaskAddrOffset) |
                            (IDMA_INT_MASK_CHAN_MASK << chanShift));

      /* Clear the chainMode bit if in chain mode */
      if(dmaDesc->nextDescPtr) cntlReg &= IDMA_CTRL_CHAIN_MODE;

      /* Start the transfer */
      cntlReg |= IDMA_CTRL_CHAN_ENB;
/*
printf("000: 0x%08x (adr=0x%08x)\n",
cntlReg,MV64260_REG_BASE+dmaChanCtrl->chanCtrlLowRegOff);
cntlReg = 0x80001f2c;
cntlReg = 0x80001eeb;
*/
/*
printf("111: 0x%08x\n",cntlReg);
*/
      MV64260_WRITE32_PUSH (MV64260_REG_BASE,
                            dmaChanCtrl->chanCtrlLowRegOff, cntlReg);
/*
{
  UINT32 tst;
  tst = *(UINT32 *)0xf1000840;
printf("222: 0x%08x\n",tst);
}
*/
      if((int)dmaChanCtrl->userHandler == WAIT_FOREVER)
      {
        semTake (dmaChanCtrl->semIntId, WAIT_FOREVER);            
        if(dmaStatus != NULL)
        {
          sysMv64260DmaStatus (chan, dmaStatus);
        }

				/*
cntlReg = 0x80000f2c;
MV64260_WRITE32_PUSH (MV64260_REG_BASE,
                      dmaChanCtrl->chanCtrlLowRegOff, cntlReg);

printf("222: 0x%08x\n",cntlReg);
printf("dmaStatus-> %d %d\n",dmaStatus->dmaStatus,dmaStatus->dmaErrorCode);
				*/
      }
    }
    else
    {
      /* Channel already active or byte count to large */
      status = ERROR;
    }

    /* Release the device instance semaphore. */
    semGive (dmaChanCtrl->semDrvId);
  }
  else
  {
    /* Driver not initialized or invalid parameter */
    status = ERROR;
  }

  return(status);
}

/******************************************************************************
*
* sysMv64260DmaStatus - Read and return DMA status.
*
* For the given channel, provide the following DMA status information:
*
* The saved DMA Error Code Value;
* The saved DMA Error Address associated with the error code;
* DMA Current Source Address Register;
* DMA Current Destination Address Register;
* DMA Current Next Descriptor Register;
* DMA Channel Control Register;
*
* The status information is placed into a DMA status
* structure referenced by dmaStatus along with a DMA status (dmaStatus)
* summary of either OK or ERROR. 
*
* If status is obtained while DMA is in progress, the status
* summary will be ERROR with a zero error code and null error address.
* If DMA status is OK, the DMA transfer completed successfully. If status
* is ERROR with a non-zero error code, the transfer stopped due to the 
* indicated error. If available, the offending address for the error 
* is also returned.
*
* If the DMA is in chain mode, and the current descriptor is not the last
* one in the linked descriptor list, the Current Next Descriptor Register
* will contain a nonzero value indicating the starting address of the 
* next descriptor.  Otherwise, if the DMA is in block mode or if the
* current descriptor is the last descriptor of the linked list, the Current
* Next Descriptor Register will contain zero.
*
* RETURNS: 
* OK is returned if the dmaStatus is valid.
* ERROR is returned if the driver is not initialized, or the 
* channel is invalid or the dmaStatus reference is null. The 
* dmaStatus contents will not be valid.
*/

STATUS sysMv64260DmaStatus
    (
    UINT32       chan,		/* Channel to get status */
    IDMA_STATUS *dmaStatus      /* Current status of channel */
    )
    {
    IDMA_DRV_CTRL *dmaChanCtrl = NULL;
    STATUS status = OK;

    if (isInitialized && chan < IDMA_CHAN_CNT && dmaStatus != NULL)
        {
        dmaChanCtrl = &mv64260DmaCtrl[chan]; 

        dmaStatus->dmaStatus = dmaChanCtrl->dmaStatus;
        dmaStatus->dmaErrorCode = dmaChanCtrl->errSel;
        dmaStatus->dmaErrorAddr = dmaChanCtrl->errAddr;
 
        dmaStatus->curSourceAddr = 
            MV64260_READ32 (MV64260_REG_BASE, dmaChanCtrl->srcAddrRegOff);
        dmaStatus->curDestAddr = 
            MV64260_READ32 (MV64260_REG_BASE, dmaChanCtrl->destAddrRegOff);
        dmaStatus->curNextDesc = 
            MV64260_READ32 (MV64260_REG_BASE, dmaChanCtrl->nextDescPtrRegOff);
        dmaStatus->chanCtrlLow = 
            MV64260_READ32 (MV64260_REG_BASE, dmaChanCtrl->chanCtrlLowRegOff);
        dmaStatus->chanCtrlHigh = 
            MV64260_READ32 (MV64260_REG_BASE, dmaChanCtrl->chanCtrlHiRegOff);
        }
    else
        {
 
        /* Driver not initialized or invalid parameter */

        status = ERROR;
        }

    return (status);
    }

/******************************************************************************
*
* sysMv64260DmaAbort - Initiate an abort of the current DMA operation.
*
* For the given valid channel, if the channel is active, the channel's
* abort bit is set in its control register. The function waits for the 
* abort to complete before returning.
*
* RETURNS: 
* OK is returned if abort succeeded.
* ERROR is returned if the driver is not initialized or the 
* channel number is invalid or should the abort fail.
*/

STATUS sysMv64260DmaAbort
    (
    UINT32   chan		/* The active DMA channel to abort */
    )
    {
    IDMA_DRV_CTRL *dmaChanCtrl = NULL;
    UINT32 cntlReg = 0;
    STATUS status = OK;
    UINT32 msWait = 10;         /* millisecond wait for abort time */

    if (isInitialized && chan < IDMA_CHAN_CNT)
        {
        dmaChanCtrl = &mv64260DmaCtrl[chan]; 
 
        semTake (dmaChanCtrl->semDrvId, WAIT_FOREVER);

        cntlReg = MV64260_READ32 (MV64260_REG_BASE, 
                                  dmaChanCtrl->chanCtrlLowRegOff);
        
        if ((cntlReg & IDMA_CTRL_CHAN_ACTIVE_MASK) ==
            IDMA_CTRL_CHAN_ACTIVE_MASK)
            {
            cntlReg |= IDMA_CTRL_CHAN_ABORT;

            MV64260_WRITE32_PUSH (MV64260_REG_BASE,
                                  dmaChanCtrl->chanCtrlLowRegOff, cntlReg);

            /* Wait for abort to complete */

            while ((MV64260_READ32 (MV64260_REG_BASE,
                                    dmaChanCtrl->chanCtrlLowRegOff) & 
                   IDMA_CTRL_CHAN_ABORT) && msWait > 0)
                {
                sysDelay ();                
                --msWait;
                }

            if (msWait == 0)
                {
                
                /*
                 * Channel became inactive prior to abort command or 
                 * abort could not complete.
                 */

                status = ERROR;
                }
            }
        else
            {
            
            /* Channel not active */

            status = ERROR;
            }

        semGive (dmaChanCtrl->semDrvId);
        }
    else       
        {
 
        /* Driver not initialized or invalid parameter */

        status = ERROR;
        }

    return (status);
    }
  
/******************************************************************************
*
* sysMv64260DmaPause - Initiate a pause of the current DMA operation.
*
* For the given valid channel, if the channel is busy, reset the activate
* bit in the channel's control register to pause the channel. Monitor the 
* channel active bit in the channel's control register until it resets. The
* channel will pause after completing the current transfer burst in progress.
*
* RETURNS: 
* OK is returned if pause succeeded.
* ERROR is returned if the driver is not initialized, or the channel
* is invalid, or the channel was not active.
*/

STATUS sysMv64260DmaPause
    (
    UINT32 chan		/* Channel to be paused */
    )
    {
    IDMA_DRV_CTRL *dmaChanCtrl = NULL;
    UINT32 cntlReg = 0;
    STATUS status = OK;
    UINT32 msWait = 10;         /* millisecond wait for abort time */

    if (isInitialized && chan < IDMA_CHAN_CNT)
        {
        dmaChanCtrl = &mv64260DmaCtrl[chan]; 
 
        semTake (dmaChanCtrl->semDrvId, WAIT_FOREVER);
 
        cntlReg = MV64260_READ32 (MV64260_REG_BASE, 
                                  dmaChanCtrl->chanCtrlLowRegOff);

        /* If the channel is active, pause the channel */

        if (IDMA_CHAN_IS_ACTIVE (cntlReg))
            {
            cntlReg &= ~IDMA_CTRL_CHAN_ENB;

            MV64260_WRITE32_PUSH (MV64260_REG_BASE,
                                  dmaChanCtrl->chanCtrlLowRegOff, cntlReg);

            /* Wait for channel to pause */

            while (IDMA_CHAN_IS_ACTIVE (
                   MV64260_READ32 (MV64260_REG_BASE,
                                   dmaChanCtrl->chanCtrlLowRegOff)) &&
                   msWait > 0)
                {
                sysDelay ();                
                --msWait;
                }

            if (msWait == 0)
                {
                
                /* Channel did not become inactive */

                status = ERROR;
                }
            }
        else
            {
  
            /* Channel already stopped */

            status = ERROR;
            }

        semGive (dmaChanCtrl->semDrvId);
        }
    else
        {
 
        /* Driver not initialized or invalid parameter */

        status = ERROR;
        }

    return (status);
    }

/******************************************************************************
*
* sysMv64260DmaResume - Resume a previously paused DMA operation.
*
* For a given valid channel, verify that the channel is not active and that
* there are bytes remaining to transfer (a non-zero byte count). If so, write
* the channel activate bit out to the channel's control register. The 
* channel will continue the transfer.
*
* RETURNS: 
* OK is returned if the resume succeeded.
* ERROR is returned if the driver is not initialized, or the channel
* is invalid, or the channel is not paused (i.e. inactive with a 
* a non-zero byte count descriptor register).
*/

STATUS sysMv64260DmaResume
    (
    UINT32 chan          /* Channel to be resumed */
    )
    {
    IDMA_DRV_CTRL *dmaChanCtrl = NULL;
    UINT32 cntlReg = 0;
    UINT32 byteCount = 0;
    STATUS status = OK;

    if (isInitialized && chan < IDMA_CHAN_CNT)
        {
        dmaChanCtrl = &mv64260DmaCtrl[chan]; 
 
        semTake (dmaChanCtrl->semDrvId, WAIT_FOREVER);
 
        cntlReg = MV64260_READ32 (MV64260_REG_BASE, 
                                  dmaChanCtrl->chanCtrlLowRegOff);

        byteCount = MV64260_READ32 (MV64260_REG_BASE, 
                                    dmaChanCtrl->byteCountRegOff);

        /* 
         * If the channel is not active and bytes remain for transfer, 
         * resume the channel 
         */

        if (!IDMA_CHAN_IS_ACTIVE (cntlReg) && byteCount > 0)
            {
            cntlReg |= IDMA_CTRL_CHAN_ENB;

            MV64260_WRITE32_PUSH (MV64260_REG_BASE,
                                  dmaChanCtrl->chanCtrlLowRegOff, cntlReg);
            }
        else
            {

            /* Channel already active or no bytes to transfer */

            status = ERROR;
            }

        semGive (dmaChanCtrl->semDrvId);
        }
    else
        {
 
        /* Driver not initialized or invalid parameter */

        status = ERROR;
        }

    return (status);
    }

/******************************************************************************
*
* sysMv64260DmaArbiterSet - Custom program channel arbitration weighting.
*
* This function accepts an array of IDMA_ARB_SLICE_CNT elements. Each
* element represents an arbiter time slice. The value of each element is a
* DMA channel number assigned to the slice. The default arbiter configuration
* provides equal weighting to all channels at reset. The default arbSlice
* array of channel numbers looks like this:
*
* arbSliceDefault[] = { 0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3 }
*
* Replacing channel numbers provides a custom weighting. Those channels
* that occupy more slices receive proportionately more running time when other
* channels are active simultaneously. There are two arbiters, each handling
* four channels (i.e. channels 0-3 and 4-7). For both arbiters, channels
* are referred to by the numbers 0 through 3, so the slice array is specified
* the same way for each.
*
* A null array reference will have no  effect and results in an ERROR 
* return status. If a channel number is not a valid channel an ERROR 
* status is returned with no action taken.
*
* RETURNS: 
* OK when arbiter has been successfully programmed.
* Error if arbSlice array is null or any channel is invalid. 
*/

STATUS sysMv64260DmaArbiterSet
    (
    UINT32   arbId,             /* Which arbiter to set, 0 or 1. */
    UINT32   arbSlice[]		/* 16 slice array of channel numbers */
    )
    {
    UINT32 slice = 0;
    UINT32 arbCtrl = 0;
    UINT32 arbAddrOffset = 0;
    STATUS status = OK;
 
    if (arbSlice != NULL && arbId < IDMA_ARB_CNT)
        {
        arbAddrOffset = IDMA_ARB_CTRL_CHAN0_3 + 
                        (arbId * IDMA_CHAN_GRP_INTERVAL);

        /* Validate the channel number in each entry slice */

        while (status == OK && slice < IDMA_ARB_SLICE_CNT)
            {
            if (arbSlice[slice] < IDMA_CHAN_GRP_SZ)
                {
                ++slice;
                }
            else
                {
                status = ERROR;
                }
            }
            
        /* Program the arbiter */

        if (status == OK)
            {
            for (slice = 0; slice < IDMA_ARB_SLICE_CNT; ++slice)
                {
                arbCtrl |= (arbSlice[slice] & IDMA_ARB_SLICE_MASK) <<
                           (slice * IDMA_ARB_1_BIT);
                }

            MV64260_WRITE32_PUSH (MV64260_REG_BASE, arbAddrOffset, arbCtrl);
            }
        }
    else
        {
 
        /* Invalid parameter */

        status = ERROR;
        }
     
    return (status);
    }
