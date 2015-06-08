/* sysMv64360Dma.c - Support for the MV64360 DMA Controller */

/* Copyright 2004 Motorola, Inc., All Rights Reserved */

/*
modification history
--------------------
01e,03feb04,cak  Added a DMA window show routine.
01d,17dec03,cak  BSP update.
01c,24oct02,cak  Renamed the file sysMv64360Dma.c to follow RTSS convention
		 for C files, and also renamed routines to sysMv64360...
		 Modifications were also made based on bugs that were found
		 during testing of the HXEB100.
01b,17oct02,cak  Corrected programming error in mv64360DmaStart routine. 
01a,29May02,efb  New. Base on harrierDma.c features (01b,21jan02,mcp820).
*/

/*
DESCRIPTION

MV64360 DMA driver functions are provided to support 
initialization, configuration, start, pause, resume, abort and 
status retrieval of DMA operations on a per channel basis. 
Both block mode and chain mode are supported.

The DMA initialization routine must be called once before any other DMA
routine is invoked. Failure to do so will result in status error returns
from all other DMA functions. This initialization is handled by sysHwInit2 
whenever INCLUDE_MV64360_DMA is defined in config.h. 

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
supplied in the DMA descriptor. The starting address for either or both
source and destination may be optionally held (no increment) during the 
transfer.

The user interrupt handler, if provided, must conform to the rules for 
standard VxWorks interrupt service routines. It is called with a current
channel parameter during handling of both normal completion and error 
interrupts. Since this routine is called by the driver interrupt handlers,
it is not responsible for clearing interrupts. Interrupts are cleared after
the user routine returns to the driver's handler.

.CS
  sysMv64360DmaArbiterSet 
  sysMv64360DmaAbort
  sysMv64360DmaInit
  sysMv64360DmaPause
  sysMv64360DmaResume 
  sysMv64360DmaStart 
  sysMv64360DmaStatus
.CE

*/

/* includes */

#include "vxWorks.h"
#include "config.h"
#include "mv64360Dma.h"
#include "mv64360Smc.h"

/* defines */

#define IDMA_REG_SIZE sizeof(UINT32)
#define IDMA_CHAN_IS_ACTIVE(_X_) (((_X_) & IDMA_CNTL_CHAN_ACTIVE_MASK) == \
                                  IDMA_CNTL_CHAN_ACTIVE_MASK)

/* typedefs */

/*
 * The following structure defines the control elements for a given DMA
 * channel. The register address offsets are used to conveniently access a
 * particular channel's DMA descriptor and control registers.
 */

typedef struct mv64360DmaDrvCntl
    {
    UINT32  byteCountRegOff;    /* offset of DMA byte count register */
    UINT32  sourceAddrRegOff;   /* offset of DMA source address register */
    UINT32  destinAddrRegOff;   /* offset of DMA destin address register */
    UINT32  nextDescPtrRegOff;  /* offset of the next desc ptr register */
    UINT32  currDescPtrRegOff;  /* offset of the current desc ptr register */
    UINT32  chanCntlLowRegOff;  /* offset of the channel control register */
    UINT32  chanCntlHighRegOff; /* offset of the channel control register */
    UINT32  chanAccessProtOff;  /* offset of window access control register */
    HANDLER userHandler;        /* user supplied notification routine */
    SEM_ID  semDrvId;           /* driver access semaphore */
    SEM_ID  semIntId;           /* interrupt interface semaphore */
    STATUS  dmaStatus;          /* DMA completion status */
    UINT32  errSel;             /* DMA error select code */
    UINT32  errAddr;            /* address causing the error */
    } IDMA_DRV_CNTL;

/* globals */

/* locals */

LOCAL IDMA_DRV_CNTL mv64360DmaCntl[IDMA_CHANNEL_COUNT];
LOCAL BOOL isInitialized = FALSE;

/* forward declarations */

#ifdef INCLUDE_SHOW_ROUTINES
    void sysMv64360DmaWindowsShow ();
#endif /* INCLUDE_SHOW_ROUTINES */

/* external references */

IMPORT void sysDelay (void);

/******************************************************************************
*
* sysMv64360InitAddrWindows - Initialize address decoding windows.
*
* This routine initializes the DMA address decoding windows to 
* cover the system address map. DRAM addresses, PCI I/O and PCI memory 
* addresses on both PCI buses are mapped for DMA access. 
* A decoding window is configured for each DRAM bank that is enabled.
* All DRAM windows are configured for write back (WB) coherency.
* DRAM bank base addresses and sizes are obtained from the
* corresponding CPU interface registers, then programmed into
* DMA base address and size registers.  PCI windows are configured for
* non-byte swapping transfers. All configured windows are enabled upon return.
*
* Note that base addresses and sizes are read from previously configured
* CPU IF registers. The base address values from these registers are 20 bits
* wide. The upper four address bits are ignored. It is not expected to have
* any addresses high enough to use these ignored bits. Only the next (upper) 
* 16 address bits are used since all DMA windows must be 64K aligned.
*
* RETURNS: N/A
*/

LOCAL void sysMv64360InitAddrWindows (void)
    {
    UINT32 dmaBaseEnable = 0;
    UINT32 baseAddrEnable = 0;
    UINT32 baseAddr = 0;
    UINT32 size = 0;
    UINT32 baseAddrVal = 0;

    baseAddrEnable = MV64360_READ32 (MV64360_REG_BASE, 
                     CPUIF_BASE_ADDR_ENABLE);
        
    /* Configure window for DRAM CS0 */

    if (!(baseAddrEnable & CPUIF_BASE_ADDR_ENABLE_CS_0_MASK)) 
        {
        baseAddr = MV64360_READ32 (MV64360_REG_BASE, 
                   CPUIF_CS0_BASE_ADDR) << IDMA_CPUIF_ADDR_SHIFT_INDEX;

        size = MV64360_READ32 (MV64360_REG_BASE, CPUIF_CS0_SIZE) <<
               IDMA_WINDOW_SIZE_BIT;

        baseAddrVal = IDMA_BASE_ADDR_TARGET_DRAM   |
                      IDMA_BASE_ADDR_DRAM_BANK_CS0 |
                      IDMA_BASE_ADDR_COHERENCY_WB  |
                      (baseAddr & IDMA_BASE_ADDR_BASE_MASK);

        MV64360_WRITE32_PUSH (MV64360_REG_BASE, IDMA_BASE_ADDR_REG0,
                              baseAddrVal);
        MV64360_WRITE32_PUSH (MV64360_REG_BASE, IDMA_SIZE_REG0, 
                              size & IDMA_WINDOW_SIZE_MASK);

        /* Enable Window 0 */

        dmaBaseEnable = MV64360_READ32 (MV64360_REG_BASE,
                        IDMA_BASE_ADDR_ENABLE);

        MV64360_WRITE32_PUSH (MV64360_REG_BASE, IDMA_BASE_ADDR_ENABLE,
                             ~IDMA_BASE_ADDR_ENABLE_WIN0 & dmaBaseEnable);
        }

    /* Configure window for DRAM CS1 */

    if (!(baseAddrEnable & CPUIF_BASE_ADDR_ENABLE_CS_1_MASK)) 
        {
        baseAddr = MV64360_READ32 (MV64360_REG_BASE, 
                   CPUIF_CS1_BASE_ADDR) << IDMA_CPUIF_ADDR_SHIFT_INDEX;

        size = MV64360_READ32 (MV64360_REG_BASE, CPUIF_CS1_SIZE) <<
               IDMA_WINDOW_SIZE_BIT;

        baseAddrVal = IDMA_BASE_ADDR_TARGET_DRAM   |
                      IDMA_BASE_ADDR_DRAM_BANK_CS1 |
                      IDMA_BASE_ADDR_COHERENCY_WB  |
                      (baseAddr & IDMA_BASE_ADDR_BASE_MASK);

        MV64360_WRITE32_PUSH (MV64360_REG_BASE, IDMA_BASE_ADDR_REG1,
                              baseAddrVal);
        MV64360_WRITE32_PUSH (MV64360_REG_BASE, IDMA_SIZE_REG1, 
                              size & IDMA_WINDOW_SIZE_MASK);

        /* Enable Window 1 */

        dmaBaseEnable = MV64360_READ32 (MV64360_REG_BASE,
                        IDMA_BASE_ADDR_ENABLE);

        MV64360_WRITE32_PUSH (MV64360_REG_BASE, IDMA_BASE_ADDR_ENABLE,
                             ~IDMA_BASE_ADDR_ENABLE_WIN1 & dmaBaseEnable);
        }

    /* Configure window for DRAM CS2 */

    if (!(baseAddrEnable & CPUIF_BASE_ADDR_ENABLE_CS_2_MASK)) 
        {
        baseAddr = MV64360_READ32 (MV64360_REG_BASE, 
                   CPUIF_CS2_BASE_ADDR) << IDMA_CPUIF_ADDR_SHIFT_INDEX;

        size = MV64360_READ32 (MV64360_REG_BASE, CPUIF_CS2_SIZE) <<
               IDMA_WINDOW_SIZE_BIT;

        baseAddrVal = IDMA_BASE_ADDR_TARGET_DRAM   |
                      IDMA_BASE_ADDR_DRAM_BANK_CS2 |
                      IDMA_BASE_ADDR_COHERENCY_WB  |
                      (baseAddr & IDMA_BASE_ADDR_BASE_MASK);

        MV64360_WRITE32_PUSH (MV64360_REG_BASE, IDMA_BASE_ADDR_REG2,
                              baseAddrVal);
        MV64360_WRITE32_PUSH (MV64360_REG_BASE, IDMA_SIZE_REG2, 
                              size & IDMA_WINDOW_SIZE_MASK);

        /* Enable Window 2 */

        dmaBaseEnable = MV64360_READ32 (MV64360_REG_BASE,
                        IDMA_BASE_ADDR_ENABLE);

        MV64360_WRITE32_PUSH (MV64360_REG_BASE, IDMA_BASE_ADDR_ENABLE,
                             ~IDMA_BASE_ADDR_ENABLE_WIN2 & dmaBaseEnable);
        }

    /* Configure window for DRAM CS3 */

    if (!(baseAddrEnable & CPUIF_BASE_ADDR_ENABLE_CS_3_MASK)) 
        {
        baseAddr = MV64360_READ32 (MV64360_REG_BASE, 
                   CPUIF_CS3_BASE_ADDR) << IDMA_CPUIF_ADDR_SHIFT_INDEX;

        size = MV64360_READ32 (MV64360_REG_BASE, CPUIF_CS3_SIZE) <<
               IDMA_WINDOW_SIZE_BIT;

        baseAddrVal = IDMA_BASE_ADDR_TARGET_DRAM   |
                      IDMA_BASE_ADDR_DRAM_BANK_CS3 |
                      IDMA_BASE_ADDR_COHERENCY_WB  |
                      (baseAddr & IDMA_BASE_ADDR_BASE_MASK);

        MV64360_WRITE32_PUSH (MV64360_REG_BASE, IDMA_BASE_ADDR_REG3,
                              baseAddrVal);
        MV64360_WRITE32_PUSH (MV64360_REG_BASE, IDMA_SIZE_REG3, 
                              size & IDMA_WINDOW_SIZE_MASK);

        /* Enable Window 3 */

        dmaBaseEnable = MV64360_READ32 (MV64360_REG_BASE,
                        IDMA_BASE_ADDR_ENABLE);

        MV64360_WRITE32_PUSH (MV64360_REG_BASE, IDMA_BASE_ADDR_ENABLE,
                             ~IDMA_BASE_ADDR_ENABLE_WIN3 & dmaBaseEnable);
        }

    /* Configure window for PCI Bus 0 I/O */

    baseAddr = MV64360_READ32 (MV64360_REG_BASE, 
               CPUIF_PCI0_IO_BASE_ADDR) << IDMA_CPUIF_ADDR_SHIFT_INDEX;

    size = MV64360_READ32 (MV64360_REG_BASE, CPUIF_PCI0_IO_SIZE) <<
           IDMA_WINDOW_SIZE_BIT;

#ifdef IDMA_SNOOP_ON
    baseAddrVal = IDMA_BASE_ADDR_TARGET_PCI0 | 
                  IDMA_BASE_ADDR_PCI_NO_SWAP |
                  IDMA_BASE_ADDR_PCI_REQ64_SIZE |
                  (baseAddr & IDMA_BASE_ADDR_BASE_MASK);
#else
    baseAddrVal = IDMA_BASE_ADDR_TARGET_PCI0 | 
                  IDMA_BASE_ADDR_PCI_NO_SWAP |
                  IDMA_BASE_ADDR_PCI_REQ64_SIZE |
		  IDMA_BASE_ADDR_PCIX_NO_SNOOP |
                  (baseAddr & IDMA_BASE_ADDR_BASE_MASK);
#endif

    MV64360_WRITE32_PUSH (MV64360_REG_BASE, IDMA_BASE_ADDR_REG4,
                          baseAddrVal);
    MV64360_WRITE32_PUSH (MV64360_REG_BASE, IDMA_SIZE_REG4, 
                          size & IDMA_WINDOW_SIZE_MASK);

    /* Configure window for PCI Bus 1 I/O */

    baseAddr = MV64360_READ32 (MV64360_REG_BASE, 
               CPUIF_PCI1_IO_BASE_ADDR) << IDMA_CPUIF_ADDR_SHIFT_INDEX;

    size = MV64360_READ32 (MV64360_REG_BASE, CPUIF_PCI1_IO_SIZE) <<
           IDMA_WINDOW_SIZE_BIT;

#ifdef IDMA_SNOOP_ON
    baseAddrVal = IDMA_BASE_ADDR_TARGET_PCI1 | 
                  IDMA_BASE_ADDR_PCI_NO_SWAP |
                  IDMA_BASE_ADDR_PCI_REQ64_SIZE |
                  (baseAddr & IDMA_BASE_ADDR_BASE_MASK);
#else
    baseAddrVal = IDMA_BASE_ADDR_TARGET_PCI1 | 
                  IDMA_BASE_ADDR_PCI_NO_SWAP |
                  IDMA_BASE_ADDR_PCI_REQ64_SIZE |
		  IDMA_BASE_ADDR_PCIX_NO_SNOOP |
                  (baseAddr & IDMA_BASE_ADDR_BASE_MASK);
#endif

    MV64360_WRITE32_PUSH (MV64360_REG_BASE, IDMA_BASE_ADDR_REG5,
                          baseAddrVal);
    MV64360_WRITE32_PUSH (MV64360_REG_BASE, IDMA_SIZE_REG5, 
                          size & IDMA_WINDOW_SIZE_MASK);

    /* Configure window for PCI Bus 0 memory */

    baseAddr = MV64360_READ32 (MV64360_REG_BASE, 
               CPUIF_PCI0_MEM0_BASE_ADDR) << IDMA_CPUIF_ADDR_SHIFT_INDEX;

    size = MV64360_READ32 (MV64360_REG_BASE, CPUIF_PCI0_MEM0_SIZE) <<
           IDMA_WINDOW_SIZE_BIT;

    baseAddrVal = IDMA_BASE_ADDR_TARGET_PCI0 | 
                  IDMA_BASE_ADDR_PCI_NO_SWAP |
                  IDMA_BASE_ADDR_PCI_MEM_SEL |
                  IDMA_BASE_ADDR_PCI_REQ64_SIZE |
                  (baseAddr & IDMA_BASE_ADDR_BASE_MASK);

    MV64360_WRITE32_PUSH (MV64360_REG_BASE, IDMA_BASE_ADDR_REG6,
                          baseAddrVal);
    MV64360_WRITE32_PUSH (MV64360_REG_BASE, IDMA_SIZE_REG6, 
                          size & IDMA_WINDOW_SIZE_MASK);

    /* Configure window for PCI Bus 1 memory */

    baseAddr = MV64360_READ32 (MV64360_REG_BASE, 
               CPUIF_PCI1_MEM0_BASE_ADDR) << IDMA_CPUIF_ADDR_SHIFT_INDEX;

    size = MV64360_READ32 (MV64360_REG_BASE, CPUIF_PCI1_MEM0_SIZE) <<
           IDMA_WINDOW_SIZE_BIT;

    baseAddrVal = IDMA_BASE_ADDR_TARGET_PCI1 | 
                  IDMA_BASE_ADDR_PCI_NO_SWAP |
                  IDMA_BASE_ADDR_PCI_MEM_SEL |
                  IDMA_BASE_ADDR_PCI_REQ64_SIZE |
                  (baseAddr & IDMA_BASE_ADDR_BASE_MASK);

    MV64360_WRITE32_PUSH (MV64360_REG_BASE, IDMA_BASE_ADDR_REG7,
                          baseAddrVal);
    MV64360_WRITE32_PUSH (MV64360_REG_BASE, IDMA_SIZE_REG7, 
                          size & IDMA_WINDOW_SIZE_MASK);

    /* Enable Windows 4,5,6,7 */

    dmaBaseEnable = MV64360_READ32 (MV64360_REG_BASE,
                    IDMA_BASE_ADDR_ENABLE);

    MV64360_WRITE32_PUSH (MV64360_REG_BASE, IDMA_BASE_ADDR_ENABLE,
                          dmaBaseEnable & ~(IDMA_BASE_ADDR_ENABLE_WIN4 |
                          IDMA_BASE_ADDR_ENABLE_WIN5 |
                          IDMA_BASE_ADDR_ENABLE_WIN6 |
                          IDMA_BASE_ADDR_ENABLE_WIN7));
    }

/**************************************************************************
*
* sysMv64360DmaHandleInt - Handle a normal completion interrupt.
*
* This function handles a normal DMA completion interrupt for a particular
* channel. The interrupt is verified to have occurred and the channel
* status set to OK to indicate successful completion. The semaphore is
* given to the DMA start routine if userHandler is set to WAIT_FOREVER.
* Otherwise if the userHandler is non-null, the userHandler routine is
* invoked with the channel number. The interrupt is cleared by writing a
* zero to the appropriate channel completion bit in the interrupt cause
* register. Other channel's bits are written as ones to preserve their 
* state (writing a one has no effect).
*
* RETURNS: N/A
*/

LOCAL void sysMv64360DmaHandleInt 
    (
    UINT32   chan		/* Interrupting channel */
    )
    {
    IDMA_DRV_CNTL *dmaChanCntl = NULL; 
    UINT32 intCause = 0xFFFFFFFF;
    UINT32 completionMask = 0;
    UINT32 errorMask = 0;
    UINT32 errorCode = 0;

    dmaChanCntl = &mv64360DmaCntl[chan]; 
    completionMask = IDMA_INT_CAUSE_DMA_COMPL_MASK << 
                     (IDMA_INT_CAUSE_CHAN1_BIT * chan);

    errorMask = IDMA_INT_CAUSE_CHAN_ERR_MASK <<
                (IDMA_INT_CAUSE_CHAN1_BIT * chan);

    errorCode = MV64360_READ32 (MV64360_REG_BASE,
                IDMA_INTERRUPT_CAUSE) & errorMask;

    /* Check channel to be sure interrupt actually occurred */

    if ((MV64360_READ32 (MV64360_REG_BASE, IDMA_INTERRUPT_CAUSE) & 
        completionMask) != 0)
        {
        if (errorCode == 0)
            {
            dmaChanCntl->dmaStatus = OK;
            }

        /*
         * If the user handler for the channel is set to WAIT_FOREVER
         * then give the semaphore to the waiting DMA start. If the user
         * handler is not WAIT_FOREVER, but non-null, invoke the user
         * routine with the channel number. If the handler is null
         * continue directly to clear the interrupt.
         */ 

        if ((int)(dmaChanCntl->userHandler) == WAIT_FOREVER)
            {    
	    semGive (dmaChanCntl->semIntId);
            }
        else if (dmaChanCntl->userHandler != NULL)
            {
            (dmaChanCntl->userHandler) (chan);
            }

        /*
         * Write out cleared interrupt field to clear the completion 
         * interrupt for the given channel only.
         */

        intCause &= ~completionMask;
        MV64360_WRITE32_PUSH (MV64360_REG_BASE, IDMA_INTERRUPT_CAUSE,
            intCause);
        }
    }

/**************************************************************************
*
* sysMv64360DmaHandleErrInt - Handle an error interrupt.
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
* the interrupts are cleared by writing zeros to the all of the 
* interrupting channels' error bits in the interrupt cause register. Other 
* channel's bits are written as ones to preserve their state (writing a 
* one has no effect).
*
* RETURNS: N/A
*/

LOCAL void sysMv64360DmaHandleErrInt (void)
    {
    UINT32 chan = 0;
    IDMA_DRV_CNTL *dmaChanCntl = NULL; 
    UINT32 errorMask = 0;
    UINT32 errorCode = 0;
    UINT32 errorChan = 0;
    UINT32 errorCause = 0xFFFFFFFF;

    /* Check each channel error interrupt group */

    for (chan = 0; chan < IDMA_CHANNEL_COUNT; ++chan)
        {
        dmaChanCntl = &mv64360DmaCntl[chan]; 

        errorMask = IDMA_INT_CAUSE_CHAN_MASK << 
            (IDMA_INT_CAUSE_CHAN1_BIT * chan);

        errorCode = MV64360_READ32 (MV64360_REG_BASE, 
                    IDMA_INTERRUPT_CAUSE) & errorMask;

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
            dmaChanCntl->dmaStatus = ERROR;
            dmaChanCntl->errSel = MV64360_READ32 (MV64360_REG_BASE,
                                  IDMA_ERR_SELECT);

            errorChan = (dmaChanCntl->errSel & IDMA_ERROR_SEL_CHAN_MASK) >> 
                IDMA_ERROR_SEL_CHAN_BIT;

            if (chan == errorChan)
                {
                dmaChanCntl->errAddr = MV64360_READ32 (MV64360_REG_BASE, 
                    IDMA_ERR_ADDR);
                }
            else
                {
                dmaChanCntl->errAddr = 0;
                }

            /* Clear the error interrupt field */
 
            errorCause &= ~errorMask;

            if ((int)(dmaChanCntl->userHandler) == WAIT_FOREVER)
                {    
                semGive (dmaChanCntl->semIntId);
                }
            else if (dmaChanCntl->userHandler != NULL)
                {
                (dmaChanCntl->userHandler) (chan);
                }
            }
        }

    /* Write out cleared interrupt fields to clear the interrupts */

    MV64360_WRITE32_PUSH (MV64360_REG_BASE, IDMA_INTERRUPT_CAUSE,
        errorCause);
    }

/******************************************************************************
*
* sysMv64360DmaInit - Initialize DMA and attach DMA Interrupt Handler.
*
* This function configures the DMA driver for all available DMA channels.
* The driver tracks information on each channel independently. Each channel
* control structure is set up to allow easy indexing by channel from the 
* driver routines. Register defaults are also initialized.
*
* Eight address decoding windows are also configured and enabled
* via this routine. Four are used to cover the DRAM addresses, one per bank.
* If a bank is not enabled, then no window is configured for it.
* Two are used for PCI I/O addresses on each PCI bus. Two are used for PCI 
* memory addresses on each PCI bus. An interrupt handler is connected to
* each DMA completion interrupt number. A different handler is connected
* to the DMA error interrupt. This routine must be called once for DMA 
* capability to be properly initialized before any other driver routine 
* is called. 
*
* RETURNS: OK if initialization succeeded or ERROR if an error occurred.
*/

STATUS sysMv64360DmaInit (void)
    {
    UINT32 chan = 0;
    IDMA_DRV_CNTL *dmaChanCntl = NULL; 
    UINT32 dmaCntlVal = 0;
    STATUS status = OK;
    UINT32 arbSliceDefault[] = { 0, 1, 2, 3, 0, 1, 2, 3, 
                                 0, 1, 2, 3, 0, 1, 2, 3 };

    /* Set all unused registers to defaults */

    MV64360_WRITE32_PUSH (MV64360_REG_BASE, IDMA_HDRS_RETARG_CTRL, 0x00);
    MV64360_WRITE32_PUSH (MV64360_REG_BASE, IDMA_HDR_RETARG_BASE, 0x00);
    MV64360_WRITE32_PUSH (MV64360_REG_BASE, IDMA_CROSSBAR_TIMEOUT, 
                          IDMA_CROSSBAR_TIMEOUT_PRESET |
                          IDMA_CROSSBAR_TIMEOUT_DISABLE);
    
    /* Ensure all interrupts are cleared and windows are disabled */

    MV64360_WRITE32_PUSH (MV64360_REG_BASE, IDMA_INTERRUPT_CAUSE, 0x00);
    MV64360_WRITE32_PUSH (MV64360_REG_BASE, IDMA_INTERRUPT_MASK, 0x00);
    MV64360_WRITE32_PUSH (MV64360_REG_BASE, IDMA_BASE_ADDR_ENABLE,
                          IDMA_BASE_ADDR_ENABLE_WIN_MASK);

    dmaCntlVal = IDMA_DST_BURST_LIMIT |
                 IDMA_SRC_BURST_LIMIT |
                 IDMA_CNTL_BLOCK_MODE |
                 IDMA_CNTL_NON_CHAIN_MODE |
		 IDMA_CNTL_INTERRUPT_MODE |
                 IDMA_CNTL_END_OF_XFER_HALT |
                 IDMA_CNTL_DESC_16MB;

    /* Initialize the DMA control array for all channels */

    while (chan < IDMA_CHANNEL_COUNT && status == OK)
        {
        dmaChanCntl = &mv64360DmaCntl[chan];

        dmaChanCntl->byteCountRegOff = 
            IDMA_BYTE_COUNT_CHAN0 + (chan * IDMA_REG_SIZE);   

        dmaChanCntl->sourceAddrRegOff = 
            IDMA_SRC_ADDR_CHAN0 + (chan * IDMA_REG_SIZE);   

        dmaChanCntl->destinAddrRegOff = 
            IDMA_DEST_ADDR_CHAN0 + (chan * IDMA_REG_SIZE);   

        dmaChanCntl->nextDescPtrRegOff = 
            IDMA_NEXT_DESC_PTR_CHAN0 + (chan * IDMA_REG_SIZE);   

        dmaChanCntl->currDescPtrRegOff =
            IDMA_CURR_DESC_PTR_CHAN0 + (chan * IDMA_REG_SIZE);   

        dmaChanCntl->chanCntlLowRegOff = 
            IDMA_CHAN0_CTRL_LOW + (chan * IDMA_REG_SIZE);   

        dmaChanCntl->chanCntlHighRegOff = 
            IDMA_CHAN0_CTRL_HI + (chan * IDMA_REG_SIZE);   

        dmaChanCntl->chanAccessProtOff = 
            IDMA_CHAN0_ACCESS_PROTECT + (chan * IDMA_REG_SIZE);

        dmaChanCntl->userHandler = NULL;

        dmaChanCntl->semDrvId = semBCreate (SEM_Q_PRIORITY, SEM_FULL);
        dmaChanCntl->semIntId = semBCreate (SEM_Q_FIFO, SEM_EMPTY);

        /* Clear the error and status information */

        dmaChanCntl->dmaStatus = ERROR;
        dmaChanCntl->errSel = 0;
        dmaChanCntl->errAddr = 0;

        MV64360_READ32 (MV64360_REG_BASE, IDMA_ERR_ADDR);
        MV64360_READ32 (MV64360_REG_BASE, IDMA_ERR_SELECT);
        MV64360_WRITE32_PUSH (MV64360_REG_BASE, IDMA_ERR_ADDR, 0);
        MV64360_WRITE32_PUSH (MV64360_REG_BASE, IDMA_ERR_SELECT, 0);
            
        /* Initialize the fixed control settings and defaults */

        MV64360_WRITE32_PUSH (MV64360_REG_BASE,
                              dmaChanCntl->chanCntlLowRegOff,
                              dmaCntlVal);

        MV64360_WRITE32_PUSH (MV64360_REG_BASE,
                              dmaChanCntl->chanCntlHighRegOff,
                              0x00);

        MV64360_WRITE32_PUSH (MV64360_REG_BASE,
                              dmaChanCntl->chanAccessProtOff,
                              IDMA_CHAN_ACCESS_FULL_WIN_ALL);

        MV64360_WRITE32_PUSH (MV64360_REG_BASE,
                              dmaChanCntl->byteCountRegOff,
                              0); 

        MV64360_WRITE32_PUSH (MV64360_REG_BASE,
                              dmaChanCntl->sourceAddrRegOff, 
                              0x00);

        MV64360_WRITE32_PUSH (MV64360_REG_BASE,
                              dmaChanCntl->destinAddrRegOff,
                              0x00);

        MV64360_WRITE32_PUSH (MV64360_REG_BASE,
                              dmaChanCntl->currDescPtrRegOff,
                              0x00);

        MV64360_WRITE32_PUSH (MV64360_REG_BASE,
                              dmaChanCntl->nextDescPtrRegOff,
                              0x00);

        MV64360_WRITE32_PUSH (MV64360_REG_BASE,
                              IDMA_HI_ADDR_REMAP0 + (chan * IDMA_REG_SIZE),
                              0x00);

        /* Connect the same interrupt handler to each interrupt number */

        status = intConnect (INUM_TO_IVEC (IDMA_CHAN0_COMPLETION_INT + chan),
            sysMv64360DmaHandleInt, (int)chan);

        if (status == OK)
             {
             intEnable (IDMA_CHAN0_COMPLETION_INT + chan);
             }

        ++chan;
        }

    /*
     * If status is okay, connect the DMA error interrupt and initialize
     * the DMA base address decoding windows and arbiter.
     */

    if (status == OK)
        {
        status = intConnect (INUM_TO_IVEC (IDMA_ERROR_INT), 
            sysMv64360DmaHandleErrInt, 0);
        
        if (status == OK)
            {
            intEnable (IDMA_ERROR_INT);

            sysMv64360InitAddrWindows (); 
            sysMv64360DmaArbiterSet (arbSliceDefault); 
            }
        }

    isInitialized = (status == OK);
    return (status);
    }

/******************************************************************************
*
* sysMv64360DmaStart - Configure and start the DMA controller.
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
* the done interrupt which will return the contents of the DMA status register
* (userHandler == WAIT_FOREVER).  The second is to provide a routine for the
* DMA interrupt handler to call upon completion which must follow the rules
* for executing inside an ISR by not calling waits, prints, etc.
* (userHander == user routine).  The third is to have DMA start return
* immediately so the user task can proceed with its execution and then call
* sysMv64360DmaStatus later to get the results (userHandler == 0).
*
* RETURNS: 
* OK is returned if the channel started successfully.
* ERROR is returned if the driver is not initialized, or the 
* channel is invalid, or the dmaDesc reference is null, or the
* channel is already active and busy.
*/

STATUS sysMv64360DmaStart
    (
    UINT32           chan,      /* DMA channel to start */
    IDMA_DESCRIPTOR *dmaDesc,   /* Byte Count, Source, Destination, Next Desc */
    IDMA_ATTRIBUTES *dmaAttrib,	/* Configuration options */
    IDMA_STATUS     *dmaStatus	/* DMA status output */
    )
    {
    IDMA_DRV_CNTL *dmaChanCntl = NULL;
    UINT32 cntlReg = 0;
    STATUS status = OK;

    if (isInitialized && chan < IDMA_CHANNEL_COUNT && dmaDesc != NULL)
        {
        dmaChanCntl = &mv64360DmaCntl[chan]; 

        semTake (dmaChanCntl->semDrvId, WAIT_FOREVER);
    
        /* Make sure channel is not active and byte count is legal */

        cntlReg = MV64360_READ32 (MV64360_REG_BASE, 
                  dmaChanCntl->chanCntlLowRegOff);

        if (!IDMA_CHAN_IS_ACTIVE (cntlReg) && 
            dmaDesc->byteCount <= IDMA_DESC_16MB_BYTE_COUNT_MASK)
            {
     
            /* Set the configuration options */

            dmaChanCntl->userHandler = NULL;
            cntlReg &= ~IDMA_CNTL_HOLD_SRC_ADDR;
            cntlReg &= ~IDMA_CNTL_HOLD_DEST_ADDR;

            if (dmaAttrib != NULL)
                {
                dmaChanCntl->userHandler = dmaAttrib->userHandler;

                if (dmaAttrib->holdSourceAddr)
                    {
                    cntlReg |= IDMA_CNTL_HOLD_SRC_ADDR;
                    }
                if (dmaAttrib->holdDestAddr)
                    {
                    cntlReg |= IDMA_CNTL_HOLD_DEST_ADDR;
                    }
                }
 
            /* Set the descriptor registers */
    
            MV64360_WRITE32_PUSH (MV64360_REG_BASE,
                dmaChanCntl->byteCountRegOff, dmaDesc->byteCount &
                IDMA_DESC_16MB_BYTE_COUNT_MASK); 

            MV64360_WRITE32_PUSH (MV64360_REG_BASE,
                dmaChanCntl->sourceAddrRegOff, dmaDesc->sourceAddr);

            MV64360_WRITE32_PUSH (MV64360_REG_BASE,
                dmaChanCntl->destinAddrRegOff, dmaDesc->destAddr);

            MV64360_WRITE32_PUSH (MV64360_REG_BASE,
                dmaChanCntl->nextDescPtrRegOff, dmaDesc->nextDescPtr);

            MV64360_WRITE32_PUSH (MV64360_REG_BASE,
                dmaChanCntl->currDescPtrRegOff, (UINT32)dmaDesc);

            /*
             * Reset error state. Status is error until successful 
             * completion.
             */

            dmaChanCntl->dmaStatus = ERROR;
            dmaChanCntl->errSel = 0;
            dmaChanCntl->errAddr = 0;

            /* Unmask/enable all of the given channel's interrupts */

            MV64360_WRITE32_PUSH (MV64360_REG_BASE, IDMA_INTERRUPT_MASK, 
                MV64360_READ32 (MV64360_REG_BASE, IDMA_INTERRUPT_MASK) |
                (IDMA_INT_MASK_CHAN_MASK << 
                (IDMA_INT_MASK_CHAN1_BIT * chan)));

            /* Clear the chainMode bit if in chain mode */

            if (dmaDesc->nextDescPtr)
                cntlReg &= IDMA_CNTL_CHAIN_MODE;

            /* Start the transfer */

            cntlReg |= IDMA_CNTL_CHAN_ENABLE;

            MV64360_WRITE32_PUSH (MV64360_REG_BASE,
                dmaChanCntl->chanCntlLowRegOff, cntlReg);

            if ((int)dmaChanCntl->userHandler == WAIT_FOREVER)
                {
                semTake (dmaChanCntl->semIntId, WAIT_FOREVER);
            
                if (dmaStatus != NULL)
                    {
                    sysMv64360DmaStatus (chan, dmaStatus);
                    }
                }
            }
        else
            {
            status = ERROR;
            }

        /* Release the device instance semaphore. */

        semGive (dmaChanCntl->semDrvId);
        }
    else
        {
        status = ERROR;
        }

    return (status);
    }

/******************************************************************************
*
* sysMv64360DmaStatus - Read and return DMA status.
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

STATUS sysMv64360DmaStatus
    (
    UINT32       chan,		/* Channel to get status */
    IDMA_STATUS *dmaStatus      /* Current status of channel */
    )
    {
    IDMA_DRV_CNTL *dmaChanCntl = NULL;
    STATUS status = OK;

    if (isInitialized && chan < IDMA_CHANNEL_COUNT && dmaStatus != NULL)
        {
        dmaChanCntl = &mv64360DmaCntl[chan]; 

        dmaStatus->dmaStatus = dmaChanCntl->dmaStatus;
        dmaStatus->dmaErrorCode = dmaChanCntl->errSel;
        dmaStatus->dmaErrorAddr = dmaChanCntl->errAddr;
 
        dmaStatus->curSourceAddr = MV64360_READ32 (MV64360_REG_BASE, 
                                   dmaChanCntl->sourceAddrRegOff);
        dmaStatus->curDestinAddr = MV64360_READ32 (MV64360_REG_BASE, 
                                   dmaChanCntl->destinAddrRegOff);
        dmaStatus->curNextDesc = MV64360_READ32 (MV64360_REG_BASE, 
                                 dmaChanCntl->nextDescPtrRegOff);
        dmaStatus->chanCntlLow = MV64360_READ32 (MV64360_REG_BASE, 
                                 dmaChanCntl->chanCntlLowRegOff);
        }
    else
        {
        status = ERROR;
        }

    return (status);
    }

/******************************************************************************
*
* sysMv64360DmaAbort - Initiate an abort of the current DMA operation.
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

STATUS sysMv64360DmaAbort
    (
    UINT32   chan		/* The active DMA channel to abort */
    )
    {
    IDMA_DRV_CNTL *dmaChanCntl = NULL;
    UINT32 cntlReg = 0;
    STATUS status = OK;
    UINT32 msWait = 10;         /* millisecond wait for abort time */

    if (isInitialized && chan < IDMA_CHANNEL_COUNT)
        {
        dmaChanCntl = &mv64360DmaCntl[chan];

        semTake (dmaChanCntl->semDrvId, WAIT_FOREVER);

        cntlReg = MV64360_READ32 (MV64360_REG_BASE,
                  dmaChanCntl->chanCntlLowRegOff);

        if ((cntlReg & IDMA_CNTL_CHAN_ACTIVE_MASK) ==
            IDMA_CNTL_CHAN_ACTIVE_MASK)
            {
            cntlReg |= IDMA_CNTL_CHAN_ABORT;

            MV64360_WRITE32_PUSH (MV64360_REG_BASE,
                dmaChanCntl->chanCntlLowRegOff, cntlReg);

            /* Wait for abort to complete */

            while ((MV64360_READ32 (MV64360_REG_BASE,
                dmaChanCntl->chanCntlLowRegOff) & IDMA_CNTL_CHAN_ABORT) &&
                msWait > 0)
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

        semGive (dmaChanCntl->semDrvId);
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
* sysMv64360DmaPause - Initiate a pause of the current DMA operation.
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

STATUS sysMv64360DmaPause
    (
    UINT32   chan		/* Channel to be paused */
    )
    {
    IDMA_DRV_CNTL *dmaChanCntl = NULL;
    UINT32 cntlReg = 0;
    STATUS status = OK;
    UINT32 msWait = 10;         /* millisecond wait for abort time */

    if (isInitialized && chan < IDMA_CHANNEL_COUNT)
        {
        dmaChanCntl = &mv64360DmaCntl[chan];

        semTake (dmaChanCntl->semDrvId, WAIT_FOREVER);

        cntlReg = MV64360_READ32 (MV64360_REG_BASE,
                  dmaChanCntl->chanCntlLowRegOff);

        /* If the channel is active, pause the channel */

        if (IDMA_CHAN_IS_ACTIVE (cntlReg))
            {
            cntlReg &= ~IDMA_CNTL_CHAN_ENABLE;

            MV64360_WRITE32_PUSH (MV64360_REG_BASE,
                dmaChanCntl->chanCntlLowRegOff, cntlReg);

            /* Wait for channel to pause */

            while (IDMA_CHAN_IS_ACTIVE (MV64360_READ32 (MV64360_REG_BASE,
                dmaChanCntl->chanCntlLowRegOff)) &&
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

        semGive (dmaChanCntl->semDrvId);
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
* sysMv64360DmaResume - Resume a previously paused DMA operation.
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

STATUS sysMv64360DmaResume
    (
    UINT32   chan		/* Channel to be resumed */
    )
    {
    IDMA_DRV_CNTL *dmaChanCntl = NULL;
    UINT32 cntlReg = 0;
    UINT32 byteCount = 0;
    STATUS status = OK;

    if (isInitialized && chan < IDMA_CHANNEL_COUNT)
        {
        dmaChanCntl = &mv64360DmaCntl[chan]; 
 
        semTake (dmaChanCntl->semDrvId, WAIT_FOREVER);
 
        cntlReg = MV64360_READ32 (MV64360_REG_BASE, 
                  dmaChanCntl->chanCntlLowRegOff);

        byteCount = MV64360_READ32 (MV64360_REG_BASE, 
                  dmaChanCntl->byteCountRegOff);

        /* If the channel is not active and bytes remain for transfer, 
           resume the channel */

        if (!IDMA_CHAN_IS_ACTIVE (cntlReg) && byteCount > 0)
            {
            cntlReg |= IDMA_CNTL_CHAN_ENABLE;

            MV64360_WRITE32_PUSH (MV64360_REG_BASE,
                dmaChanCntl->chanCntlLowRegOff, cntlReg);
            }
        else
            {
            status = ERROR;
            }

        semGive (dmaChanCntl->semDrvId);
        }
    else
        {
        status = ERROR;
        }

    return (status);
    }

/******************************************************************************
*
* sysMv64360DmaArbiterSet - Custom program channel arbitration weighting.
*
* This function accepts an array of IDMA_ARBITER_SLICE_COUNT elements. Each
* element represents an arbiter time slice. The value of each element is a
* DMA channel number assigned to the slice. The default arbiter configuration
* provides equal weighting to all channels at reset. The default arbSlice
* array of channel numbers looks like this:
*
* arbSliceDefault[] = { 0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3 }
*
* Replacing channel numbers provides a custom weighting. Those channels
* that occupy more slices receive proportionately more running time when other
* channels are active simultaneously. A null array reference will have no 
* effect and results in an ERROR return status. If a channel number is not 
* a valid channel an ERROR status is returned with no action taken.
*
* RETURNS: 
* OK when arbiter has been successfully programmed.
* Error if arbSlice array is null or any channel is invalid. 
*/

STATUS sysMv64360DmaArbiterSet
    (
    UINT32   arbSlice[]		/* 16 slice array of channel numbers */
    )
    {
    UINT32 slice = 0;
    UINT32 arbCntl = 0;
    STATUS status = OK;
 
    if (arbSlice != NULL)
        {

        /* Validate the channel number in each entry slice */

        while (status == OK && slice < IDMA_ARBITER_SLICE_COUNT)
            {
            if (arbSlice[slice] < IDMA_CHANNEL_COUNT)
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
            for (slice = 0; slice < IDMA_ARBITER_SLICE_COUNT; ++slice)
                {
                arbCntl |= (arbSlice[slice] & IDMA_ARBITER_SLICE_MASK) <<
                           (slice * IDMA_ARBITER_1_BIT);
                }

            MV64360_WRITE32_PUSH (MV64360_REG_BASE, IDMA_ARB_CTRL, arbCntl);
            }
        }
    else
        {
        status = ERROR;
        }
     
    return (status);
    }

#ifdef INCLUDE_SHOW_ROUTINES
/******************************************************************************
*
* sysMv64360DmaWindowsShow - Display DMA Window Configuration
*
* This routines displays the address window configuration in the Disco II
* DMA controller.
*
* RETURNS: N/A
*/

void sysMv64360DmaWindowsShow ()
    {
    UINT32 dmaBaseEnable = 0;
    UINT32 baseAddrReg = 0;
    UINT32 baseAddr = 0;
    UINT32 sizeReg = 0;
    UINT32 size = 0;
    UINT32 remapReg = 0;
    UINT32 remap = 0;
    int x;

    dmaBaseEnable = MV64360_READ32 (MV64360_REG_BASE, IDMA_BASE_ADDR_ENABLE);
    baseAddrReg = IDMA_BASE_ADDR_REG0;
    sizeReg = IDMA_SIZE_REG0;
    remapReg = IDMA_HI_ADDR_REMAP0;
    printf("Window  Base-Address,Size  Remap    Attributes\n\r");

    for (x = 0; x < 8; x++)
        {

    if (~dmaBaseEnable & (1 << x))
        {
    baseAddr = MV64360_READ32 (MV64360_REG_BASE, baseAddrReg);
        size = MV64360_READ32 (MV64360_REG_BASE, sizeReg);
        size = size >> IDMA_WINDOW_SIZE_BIT;
        size = MV64360_SIZE_FIELD_INV(size);
        printf("%d       ",x);
        printf("%08x,%08x  ", baseAddr & 0xFFFF0000, size);
        if (x < 4)
            {
            remap = MV64360_READ32 (MV64360_REG_BASE, remapReg + x*4);
            printf("%08x ",remap);
            }
        else
            printf("None     ");
        switch (baseAddr & 0x000000F)
            {
            case IDMA_BASE_ADDR_TARGET_DRAM:
            printf("DRAM\n");
            switch ((baseAddr & 0x00000F00) >> 8)
                {
                case 0xe:
                   printf("%36sCS[0]\n"," ");
                   break;
                case 0xd:
                   printf("%36sCS[1]\n"," ");
                   break;
                case 0xb:
                   printf("%36sCS[2]\n"," ");
                   break;
                case 0x7:
                   printf("%36sCS[3]\n"," ");
                   break;
                default:
                   printf("%36sUnknown CS\n"," ");
                   break;
                }
            switch ((baseAddr & 0x00003000) >> 12)
                {
                case 0x0:
                   printf("%36sNo cache coherency\n"," ");
                   break;
                case 0x1:
                   printf("%36sWT cache coherency\n"," ");
                   break;
                case 0x2:
                   printf("%36sWB cache coherency\n"," ");
                   break;
                default:
                   printf("%36sUnknown cache coherency\n"," ");
                   break;
                }
            break;
            case IDMA_BASE_ADDR_TARGET_DEVICE:
            printf("Device\n");
            switch ((baseAddr & 0x00001F00) >> 8)
                {
                case 0x1e:
                   printf("%36sDevCS[0]\n"," ");
                   break;
                case 0x1d:
                   printf("%36sDevCS[1]\n"," ");
                   break;
                case 0x1b:
                   printf("%36sDevCS[2]\n"," ");
                   break;
                case 0x17:
                   printf("%36sDevCS[3]\n"," ");
                   break;
                case 0x0f:
                   printf("%36sBootCS\n"," ");
                   break;
                default:
                   printf("%36sUnknown Device Bank\n"," ");
                   break;
                }
            break;
            case IDMA_BASE_ADDR_TARGET_SRAM:
            switch ((baseAddr & 0x00000800) >> 11)
                {
                case 0x0:
                   printf("SRAM\n");
                   break;
                case 0x1:
                   printf("60x bus\n");
                   break;
                }
            break;
            case IDMA_BASE_ADDR_TARGET_PCI0:
            printf("PCI 0\n");

            switch ((baseAddr & 0x00000300) >> 8)
                {
                case 0x0:
                   printf("%36sByte Swap\n"," ");
                   break;
                case 0x1:
                   printf("%36sNo Swap\n"," ");
                   break;
                case 0x2:
                   printf("%36sByte and Word Swap\n"," ");
                   break;
                case 0x3:
                   printf("%36sWord Swap\n"," ");
                   break;
                default:
                   printf("%36sUnknown Swap Type\n"," ");
                   break;
                }
            switch ((baseAddr & 0x00000400) >> 10)
                {
                case 0x0:
                   printf("%36sPCI-X NS Not Asserted\n"," ");
                   break;
                case 0x1:
                   printf("%36sPCI-X NS Asserted\n"," ");
                   break;
                default:
                   printf("%36sUnknown PCI-X NS Attribute\n"," ");
                   break;
                }
            switch ((baseAddr & 0x00000800) >> 11)
                {
                case 0x0:
                   printf("%36sPCI I/O space\n"," ");
                   break;
                case 0x1:
                   printf("%36sPCI memory space\n"," ");
                   break;
                default:
                   printf("%36sUnknown PCI space\n"," ");
                   break;
                }
            switch ((baseAddr & 0x00001000) >> 12)
                {
                case 0x0:
                   printf("%36sAlways Assert REQ64#\n"," ");
                   break;
                case 0x1:
                   printf("%36sREQ64# by Data Size\n"," ");
                   break;
                }
            break;
            case IDMA_BASE_ADDR_TARGET_PCI1:
            printf("PCI 1\n");
            switch ((baseAddr & 0x00000300) >> 8)
                {
                case 0x0:
                   printf("%36sByte Swap\n"," ");
                   break;
                case 0x1:
                   printf("%36sNo Swap\n"," ");
                   break;
                case 0x2:
                   printf("%36sByte and Word Swap\n"," ");
                   break;
                case 0x3:
                   printf("%36sWord Swap\n"," ");
                   break;
                default:
                   printf("%36sUnknown Swap Type\n"," ");
                   break;
                }
            switch ((baseAddr & 0x00000400) >> 10)
                {
                case 0x0:
                   printf("%36sPCI-X NS Not Asserted\n"," ");
                   break;
                case 0x1:
                   printf("%36sPCI-X NS Asserted\n"," ");
                   break;
                default:
                   printf("%36sUnknown PCI-X NS Attribute\n"," ");
                   break;
                }
            switch ((baseAddr & 0x00000800) >> 11)
                {
                case 0x0:
                   printf("%36sPCI I/O Space\n"," ");
                   break;
                case 0x1:
                   printf("%36sPCI Memory Space\n"," ");
                   break;
                default:
                   printf("%36sUnknown PCI Space\n"," ");
                   break;
                }
            switch ((baseAddr & 0x00001000) >> 12)
                {
                case 0x0:
                   printf("%36sAlways Assert REQ64#\n"," ");
                   break;
                case 0x1:
                   printf("%36sREQ64# by Data Size\n"," ");
                   break;
                }
            break;
        }

        }
        baseAddrReg += 8;  /* advance to next window */
        sizeReg += 8;

        } /* for loop */
    }
#endif /* INCLUDE_SHOW_ROUTINES */
