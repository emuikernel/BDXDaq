/******************************************************************************
*              (c), Copyright, FORCE COMPUTERS INDIA Limited                  *
*                                                                             *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF FORCE COMPUTERS, INC.        *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT *
* OF FORCE OR ANY THIRD PARTY. FORCE RESERVES THE RIGHT AT ITS SOLE DISCRETION*
* TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO FORCE COMPUTERS.       *
* THIS CODE IS PROVIDED "AS IS". FORCE COMPUTERS MAKES NO WARRANTIES, EXPRESS,*
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.  *
*******************************************************************************

 *
 * Test stubs for IDMA engines of MV64360.
 *
 * Use this specifically when you want to exercise two IDMA engines from
 * the same CPU. It creates two tasks, each owning an IDMA engine and doing
 * the tests infinitely. taskDelay() is used by each of these tasks to
 * re-schedule the other DMA task so that both tasks get a chance to run.
 */
#include "cacheLib.h"




#include "semLib.h"
#include "vxWorks.h"
#include "stdio.h"
#include "taskLib.h"
#include "./../../coreDrv/gtSmp.h"

#include "./../../pmc280.h"
#include "./../../coreDrv/vxDmaIntCtrl.h"
#include "./../../coreDrv/gtCore.h"
#include "./../../coreDrv/gtDma.h"
#include "./../../coreDrv/gtSram.h"

#undef DEBUG
#define GPP_DEBUG

#define ALIGN_DRAM		10
#define ALIGN_SRAM		10

void frcDmaCompleteAppIntHandler0(int no);
void frcDmaCompleteAppIntHandler1(int no);
void frcDmaTestTaskEntry0(DMA_ENGINE, unsigned int, char, 
                          char, char, unsigned int, int);
void frcDmaTestTaskEntry1(DMA_ENGINE, unsigned int, char, 
                          char, char, unsigned int, int);

void memset(void *, unsigned int, unsigned int);

/* 
 * These addresses are not dependent on the board's address map although
 * it would help to have it same as the board's address map (esp. since
 * malloc and gtSramMalloc() would return addresses that conform to the
 * board's memory map.
 */

/* DRAM */
#define DMA_CPU0_DRAM_BASE   0x00000000
#define DMA_CPU0_DRAM_SIZE   0x20000000

/* SRAM */
#define DMA_CPU0_SRAM_BASE   INTERNAL_SRAM_ADRS
#define DMA_CPU0_SRAM_SIZE   (INTERNAL_SRAM_SIZE/2)

/* PCI */
#define GUARD                0x4000
#define PCI_RESERVED_SPACE   0x4000
#define DMA_CPU0_PCI_BASE    (PCI0_MEM0_BASE_ADRS)
#define DMA_CPU0_PCI_SIZE    0x02000000

static unsigned int frcDram_base;
static unsigned int frcDram_size;

static unsigned int frcSram_base;
static unsigned int frcSram_size;

static unsigned int frcPci_base;
static unsigned int frcPci_size;

static SEM_ID frcDmaSem0, frcDmaSem1;

/*
 * This is how this works:
 *
 * A task, say tIdmaTask on the CPU copies data from DRAM/SRAM/PCI to one of the
 * the following destinations:
 *    . DRAM
 *    . SRAM
 *    . PCI
 * After initiating the transfer, it waits for the completion signal from
 * interrupt handler. It then computes the checksum at the destination to
 * verify that the transfer is indeed complete without any errors.
 */ 
static unsigned int frcCheckSum(unsigned int *start, unsigned int size)
{
    unsigned int checksum = 0;
    while(size > 0)
    {
        checksum +=  *start;
#ifdef DEBUG
        printf("[0x%08x] %08x\n", (int) start, (int *) *start);
#endif
        start++;
        size--;
    }
    return checksum;
}

unsigned int frcAlignedMalloc(unsigned int size, unsigned int alignment)
{
    unsigned int blkBase, alignmentMask = 0;

    /* Add aligment bytes to the block size for aligment space */
    blkBase = (unsigned int) cacheDmaMalloc(size + (1 << alignment));

    if(blkBase ==(unsigned int) NULL)
    {
        printf("Unable to allocate memory\n");
        return ERROR;
    }

#ifdef DEBUG
    printf("blkBase = 0x%08x\n", (int) blkBase);
#endif

    /* If malloc is already aligned */
    if(blkBase % (1 << alignment) != 0)
    {
        /* Mask for getting the address low bits for checking the aligment */
        alignmentMask = ~(0xFFFFFFFF << alignment);
#ifdef DEBUG
        printf("alignmentMask = 0x%08x\n", (int) alignmentMask);
#endif

        /* Add offset to the block base to complete the aligment */
        blkBase += (1 << alignment) - (alignmentMask & blkBase);
    }

    return blkBase;
}

unsigned int frcIdmaTestInit0(DMA_ENGINE dmaEngine, char srcIf, char tgtIf)
{
    unsigned int compCause;
#ifdef GPP_DEBUG
    unsigned int data;
#endif /* GPP_DEBUG */
    printf("Entering frcIdmaTestInit0...\n");

    /* Initialize IDMA interrupt module */
    frcDmaIntCtrlInit();

    switch(dmaEngine)
    {
        case DMA_ENG_0:
            compCause = DMA_CHAN0_COMP;
            break;
        case DMA_ENG_1:
            compCause = DMA_CHAN1_COMP;
            break;
        case DMA_ENG_2:
            compCause = DMA_CHAN2_COMP;
            break;
        case DMA_ENG_3:
            compCause = DMA_CHAN3_COMP;
            break;
        default:
            printf("Non existent DMA engine\n");
            return ERROR;
    }

    frcDram_base = DMA_CPU0_DRAM_BASE;
    frcDram_size = DMA_CPU0_DRAM_SIZE;

    frcSram_base = DMA_CPU0_SRAM_BASE;
    frcSram_size = DMA_CPU0_SRAM_SIZE;

    frcPci_base = DMA_CPU0_PCI_BASE;
    frcPci_size = DMA_CPU0_PCI_SIZE;

    /*
     * Note:
     *     Although "memSpaceTarget" and "memSpaceAttr" are two
     *     separate arguments, we will use "memSpaceTarget" for both
     *     and use "memSpaceAttr" to pass anything not taken care
     *     by "memSpaceTarget".
     *
     *     For example: For DRAM/SRAM target "memSpaceAttr" is 0x0 
     *     as nothing additional is necessary while for a PCI target
     *     we may need some extra bits to be set, and hence will use
     *     "memSpaceAttr".
     *
     * The 8 DMA Windows are also shared between the two CPUs
     * with DMA_WINDOW0 to DMA_WINDOW3 configured by CPU0 and
     * DMA_WINDOW4 to DMA_WINDOW7 configured by CPU1.
     */
    switch(srcIf)
    {
        case DMA_SRC_DRAM:
            gtDmaSetMemorySpace(DMA_WINDOW0, DMA_DRAM_CS0, 0,
                                frcDram_base, frcDram_size);
            break;
        case DMA_SRC_SRAM:
            gtDmaSetMemorySpace(DMA_WINDOW0, DMA_SRAM, 0,
                                frcSram_base, frcSram_size);
            break;
        case DMA_SRC_PCI:
            gtDmaSetMemorySpace(DMA_WINDOW0, DMA_PCI_0|BIT11|BIT12, 0,
                                frcPci_base, frcPci_size);
            break;
        default:
            printf("Undefined source type\n");
            return ERROR;
    }

    switch(tgtIf)
    {
        case DMA_TGT_DRAM:
            /* 
             * Need to map the targets only if the source has not been 
             * mapped.
             */
            if(srcIf != DMA_SRC_DRAM)
                gtDmaSetMemorySpace(DMA_WINDOW1, DMA_DRAM_CS0, 0,
                                    frcDram_base, frcDram_size);
            break;

        case DMA_TGT_SRAM:
            if(srcIf != DMA_SRC_SRAM)
                gtDmaSetMemorySpace(DMA_WINDOW1, DMA_SRAM, 0,
                                    frcSram_base, frcSram_size);
            break;

        case DMA_TGT_PCI:
            if(srcIf != DMA_SRC_PCI)
                gtDmaSetMemorySpace(DMA_WINDOW1, DMA_PCI_0|BIT11|BIT12, 0,
                                    frcPci_base, frcPci_size);
            break;
        default:
            printf("Undefined destination type\n");
            return ERROR;
    }

    /* Attach DMA completion handler */
    frcDmaIntConnect(compCause, frcDmaCompleteAppIntHandler0, 0, 1);

    /* Enable DMA completion interrupt */
    frcDmaIntEnable(compCause);

#ifdef GPP_DEBUG

    /* Make GPP[2] as Output pin */
    GT_REG_READ(GPP_IO_CONTROL, &data);
    data |= (0x1 << 2);
    GT_REG_WRITE(GPP_IO_CONTROL, data);

#endif /* GPP_DEBUG */

    return OK;
}

unsigned int frcIdmaTestInit1(DMA_ENGINE dmaEngine, char srcIf, char tgtIf)
{
    unsigned int compCause;
#ifdef GPP_DEBUG
    unsigned int data;
#endif /* GPP_DEBUG */
    printf("Entering frcIdmaTestInit1...\n");

    /* Initialize IDMA interrupt module */
    frcDmaIntCtrlInit();

    switch(dmaEngine)
    {
        case DMA_ENG_0:
            compCause = DMA_CHAN0_COMP;
            break;
        case DMA_ENG_1:
            compCause = DMA_CHAN1_COMP;
            break;
        case DMA_ENG_2:
            compCause = DMA_CHAN2_COMP;
            break;
        case DMA_ENG_3:
            compCause = DMA_CHAN3_COMP;
            break;
        default:
            printf("Non existent DMA engine\n");
            return ERROR;
    }

    frcDram_base = DMA_CPU0_DRAM_BASE;
    frcDram_size = DMA_CPU0_DRAM_SIZE;

    frcSram_base = DMA_CPU0_SRAM_BASE;
    frcSram_size = DMA_CPU0_SRAM_SIZE;

    frcPci_base = DMA_CPU0_PCI_BASE;
    frcPci_size = DMA_CPU0_PCI_SIZE;

    /*
     * Note:
     *     Although "memSpaceTarget" and "memSpaceAttr" are two
     *     separate arguments, we will use "memSpaceTarget" for both
     *     and use "memSpaceAttr" to pass anything not taken care
     *     by "memSpaceTarget".
     *
     *     For example: For DRAM/SRAM target "memSpaceAttr" is 0x0 
     *     as nothing additional is necessary while for a PCI target
     *     we may need some extra bits to be set, and hence will use
     *     "memSpaceAttr".
     *
     * The 8 DMA Windows are also shared between the two CPUs
     * with DMA_WINDOW0 to DMA_WINDOW3 configured by CPU0 and
     * DMA_WINDOW4 to DMA_WINDOW7 configured by CPU1.
     */
    switch(srcIf)
    {
        case DMA_SRC_DRAM:
            gtDmaSetMemorySpace(DMA_WINDOW0, DMA_DRAM_CS0, 0,
                                frcDram_base, frcDram_size);
            break;
        case DMA_SRC_SRAM:
            gtDmaSetMemorySpace(DMA_WINDOW0, DMA_SRAM, 0,
                                frcSram_base, frcSram_size);
            break;
        case DMA_SRC_PCI:
            gtDmaSetMemorySpace(DMA_WINDOW0, DMA_PCI_0|BIT11|BIT12, 0,
                                frcPci_base, frcPci_size);
            break;
        default:
            printf("Undefined source type\n");
            return ERROR;
    }

    switch(tgtIf)
    {
        case DMA_TGT_DRAM:
            /* 
             * Need to map the targets only if the source has not been 
             * mapped.
             */
            if(srcIf != DMA_SRC_DRAM)
                gtDmaSetMemorySpace(DMA_WINDOW1, DMA_DRAM_CS0, 0,
                                    frcDram_base, frcDram_size);
            break;

        case DMA_TGT_SRAM:
            if(srcIf != DMA_SRC_SRAM)
                gtDmaSetMemorySpace(DMA_WINDOW1, DMA_SRAM, 0,
                                    frcSram_base, frcSram_size);
            break;

        case DMA_TGT_PCI:
            if(srcIf != DMA_SRC_PCI)
                gtDmaSetMemorySpace(DMA_WINDOW1, DMA_PCI_0|BIT11|BIT12, 0,
                                    frcPci_base, frcPci_size);
            break;
        default:
            printf("Undefined destination type\n");
            return ERROR;
    }

    /* Attach DMA completion handler */
    frcDmaIntConnect(compCause, frcDmaCompleteAppIntHandler1, 0, 1);

    /* Enable DMA completion interrupt */
    frcDmaIntEnable(compCause);

#ifdef GPP_DEBUG

    /* Make GPP[2] as Output pin */
    GT_REG_READ(GPP_IO_CONTROL, &data);
    data |= (0x1 << 2);
    GT_REG_WRITE(GPP_IO_CONTROL, data);

#endif /* GPP_DEBUG */

    return OK;
}
void frcDmaTestTaskEntry0(DMA_ENGINE dmaEngine, unsigned int xferSize, 
                          char srcIf, char tgtIf, char dmaData,
                          unsigned int dmaDtl, int loopCnt)
{
    unsigned srcChecksum = 0, dstChecksum = 0, loopEnd = 0;
    unsigned int *srcMembase = 0, *dstMembase = 0;
    unsigned int frcCount0 = 0;

    printf("dmaEngine = %d, xferSize = 0x%08x, srcIf = %d tgtIf = %d 
            dmaData = %08x dmaDtl = %08x\n", dmaEngine, xferSize, srcIf, 
            tgtIf, dmaData, dmaDtl);

    /* Create semaphore */
    frcDmaSem0 = semBCreate(SEM_Q_FIFO, SEM_EMPTY);
    if(frcDmaSem0 == NULL)
    {
        printf("Unable to create semaphore\n");
    }

    /* Initialise DMA engine for tests */
    if(frcIdmaTestInit0(dmaEngine, srcIf, tgtIf) == ERROR)
    {
        printf("Unable to initialize IDMA engine\n");
        return;
    }

    /* Need to allocate memory through malloc only if source is DRAM */
    if(srcIf == DMA_SRC_DRAM)
    {
        srcMembase = (unsigned int *) frcAlignedMalloc(xferSize, ALIGN_DRAM);
        if(srcMembase == NULL)
        {
            printf("Unable to allocate memory for source buffer\n");
            return;
        }
    }
    else if(srcIf == DMA_SRC_SRAM)
    {
        srcMembase = (unsigned int *)gtSramMallocAligned(xferSize, ALIGN_SRAM);
        if(srcMembase == NULL)
        {
            printf("Unable to allocate memory for source buffer\n");
            return;
        }
    }
    else if(srcIf == DMA_SRC_PCI)
    {
        srcMembase = (unsigned int *) (DMA_CPU0_PCI_BASE + GUARD);
    }
    else
        printf("Undefined source type\n");

    printf("srcMembase = 0x%08x\n", (int) srcMembase);

    /* Need to allocate memory through malloc only if destination is DRAM */
    if(tgtIf == DMA_TGT_DRAM)
    {
        dstMembase = (unsigned int *) frcAlignedMalloc(xferSize, ALIGN_DRAM);
        if(dstMembase == NULL)
        {
            printf("Unable to allocate memory for destination buffer\n");
            return;
        }
    }
    else if(tgtIf == DMA_TGT_SRAM)
    {
        dstMembase = (unsigned int *)gtSramMallocAligned(xferSize, ALIGN_SRAM);
        if(dstMembase == NULL)
        {
            printf("Unable to allocate memory for destination buffer\n");
            return;
        }
    }
    else if(tgtIf == DMA_TGT_PCI)
    {
        if(srcIf == DMA_SRC_PCI)
            dstMembase = (unsigned int *) (srcMembase + xferSize);
        else
            dstMembase = (unsigned int *) (DMA_CPU0_PCI_BASE + GUARD);
    }
    else
        printf("Undefined destination type\n");

    printf("dstMembase = 0x%08x\n", (int) dstMembase);

    /* Fill data at the source */
    memset((void *)srcMembase, dmaData, xferSize);

    /* Clean up destination */
    memset((void *)dstMembase, 0x0, xferSize);
  
    if(loopCnt == -1) loopEnd = 1;
    else loopEnd = loopCnt;

    while(loopEnd)
    {
        /* Give the other DMA tasks chance to run! */
        taskDelay(10);

#ifdef DEBUG
        printf("Passed TaskDelay\n");
#endif        
        if(frcCount0 != 0)
        {
            /* Wait for signal that work is done */
            semTake(frcDmaSem0, WAIT_FOREVER);
#ifdef DEBUG
	printf("Got Smaphore after interrupt assertion on DMA Completion\n");
#endif
            /*
             * If the destination is SRAM, we need to force a
             * invalidation before it is read for checksumming.
             *
             * We don't have to bother if DRAM is a source interface
             * since we are using cacheDmaMalloc() for allocation.
             */
            if(tgtIf == DMA_TGT_SRAM)
            {
                cacheInvalidate(DATA_CACHE, dstMembase, xferSize);
            }
            
            dstChecksum = frcCheckSum((unsigned int *)dstMembase, (xferSize/4));
#ifdef DEBUG
            printf("[%d] dst = %08x\n", frcCount0, dstChecksum);
#endif
            if(srcChecksum != dstChecksum)
            {
                printf("DMA Error: Checksum src = 0x%08x dst = %08x\n", 
                        srcChecksum, dstChecksum);
            }
            else
                printf("Thread 0: DMA test[%d] passed\n",frcCount0);

            /* Clean up destination */
            memset((void *)dstMembase, 0x0, xferSize);
        }

        /* Calculate checksum */
        srcChecksum = frcCheckSum((unsigned int *)srcMembase, (xferSize/4));
#ifdef DEBUG
        printf("[%d] src = %08x\n", frcCount0, srcChecksum);
#endif

        /* Setup for transfer */
        if((srcIf != DMA_SRC_PCI) && (tgtIf != DMA_TGT_PCI))
        {
            gtDmaTransfer(dmaEngine, (unsigned int) srcMembase, 
                          (unsigned int) dstMembase, xferSize,
                          DMA_BLOCK_TRANSFER_MODE| dmaDtl, srcIf, tgtIf, 
                         (DMA_RECORD *)NULL); 
        }
        else if((srcIf == DMA_SRC_PCI) && (tgtIf != DMA_TGT_PCI))
        {
            gtDmaTransfer(dmaEngine, (unsigned int) srcMembase, 
                          (unsigned int) dstMembase, xferSize,
                          DMA_BLOCK_TRANSFER_MODE|
#if FALSE
                          DMA_SOURCE_ADDR_IN_PCI0|
#endif
                          dmaDtl, srcIf, tgtIf, 
                          (DMA_RECORD *)NULL); 
        }
        else if((srcIf != DMA_SRC_PCI) && (tgtIf == DMA_TGT_PCI))
        {
            gtDmaTransfer(dmaEngine, (unsigned int) srcMembase, 
                          (unsigned int) dstMembase, xferSize,
                          DMA_BLOCK_TRANSFER_MODE|
#if FALSE
                          DMA_DEST_ADDR_IN_PCI0|
#endif
                          dmaDtl, srcIf, tgtIf, 
                          (DMA_RECORD *)NULL); 
        }
        else if((srcIf == DMA_SRC_PCI) && (tgtIf == DMA_TGT_PCI))
        {
            gtDmaTransfer(dmaEngine, (unsigned int) srcMembase, 
                          (unsigned int)dstMembase, xferSize,
                          DMA_BLOCK_TRANSFER_MODE|
#if FALSE
                          DMA_SOURCE_ADDR_IN_PCI0|
                          DMA_DEST_ADDR_IN_PCI0|
#endif
                          dmaDtl, srcIf, tgtIf, 
                         (DMA_RECORD *)NULL); 
        }
        else
        {
            printf("Unknown srcIf and tgtIf\n");
            return;
        }

        frcCount0++;

        /* Reset frcCount0 */
        if(frcCount0 == 0x0FFFFFFF) frcCount0 = 1;

        if(loopCnt != -1) 
            loopEnd--;
    }
}

void frcDmaTestTaskEntry1(DMA_ENGINE dmaEngine, unsigned int xferSize, 
                          char srcIf, char tgtIf, char dmaData,
                          unsigned int dmaDtl, int loopCnt)
{
    unsigned srcChecksum = 0, dstChecksum = 0, loopEnd = 0;
    unsigned int *srcMembase = 0, *dstMembase = 0,frcCount1 = 0;
    
    printf("dmaEngine = %d, xferSize = 0x%08x, srcIf = %d tgtIf = %d 
            dmaData = %08x dmaDtl = %08x\n", dmaEngine, xferSize, srcIf, 
            tgtIf, dmaData, dmaDtl);

    /* Create semaphore */
    frcDmaSem1 = semBCreate(SEM_Q_FIFO, SEM_EMPTY);
    if(frcDmaSem1 == NULL)
    {
        printf("Unable to create semaphore\n");
        return;
    }

    /* Initialise DMA engine for tests */
    if(frcIdmaTestInit1(dmaEngine, srcIf, tgtIf) == ERROR)
    {
        printf("Unable to initialize IDMA engine\n");
        return;
    }

    /* Need to allocate memory through malloc only if source is DRAM */
    if(srcIf == DMA_SRC_DRAM)
    {
        srcMembase = (unsigned int *) frcAlignedMalloc(xferSize, ALIGN_DRAM);
        if(srcMembase == NULL)
        {
            printf("Unable to allocate memory for source buffer\n");
            return;
        }
    }
    else if(srcIf == DMA_SRC_SRAM)
    {
        srcMembase = (unsigned int *)gtSramMallocAligned(xferSize, ALIGN_SRAM);
        if(srcMembase == NULL)
        {
            printf("Unable to allocate memory for source buffer\n");
            return;
        }
    }
    else if(srcIf == DMA_SRC_PCI)
    {
        srcMembase = (unsigned int *) (DMA_CPU0_PCI_BASE + GUARD + PCI_RESERVED_SPACE);
    }
    else
        printf("Undefined source type\n");

    printf("srcMembase = 0x%08x\n", (int) srcMembase);

    /* Need to allocate memory through malloc only if destination is DRAM */
    if(tgtIf == DMA_TGT_DRAM)
    {
        dstMembase = (unsigned int *) frcAlignedMalloc(xferSize, ALIGN_DRAM);
        if(dstMembase == NULL)
        {
            printf("Unable to allocate memory for destination buffer\n");
            return;
        }
    }
    else if(tgtIf == DMA_TGT_SRAM)
    {
        dstMembase = (unsigned int *)gtSramMallocAligned(xferSize, ALIGN_SRAM);
        if(dstMembase == NULL)
        {
            printf("Unable to allocate memory for destination buffer\n");
            return;
        }
    }
    else if(tgtIf == DMA_TGT_PCI)
    {
        if(srcIf == DMA_SRC_PCI)
            dstMembase = (unsigned int *) (srcMembase + xferSize);
        else
            dstMembase = (unsigned int *) (DMA_CPU0_PCI_BASE + GUARD + PCI_RESERVED_SPACE);
    }
    else
        printf("Undefined destination type\n");

    printf("dstMembase = 0x%08x\n", (int) dstMembase);


    /* Fill data at the source */
    memset((void *)srcMembase, dmaData, xferSize);

    /* Clean up destination */
    memset((void *)dstMembase, 0x0, xferSize);

    if(loopCnt == -1) loopEnd = 1;
    else loopEnd = loopCnt;

    while(loopEnd)
    {
        /* Give the other DMA tasks chance to run! */
        taskDelay(10);

#ifdef DEBUG
        printf("Passed TaskDelay\n");
#endif        
        if(frcCount1 != 0)
        {
            /* Wait for signal that work is done */
            semTake(frcDmaSem1, WAIT_FOREVER);

#ifdef DEBUG
        printf("Semaphore Given after Dma interrupt\n");
#endif        
            /*
             * If the destination is SRAM, we need to force a
             * invalidation before it is read for checksumming.
             *
             * We don't have to bother if DRAM is a source interface
             * since we are using cacheDmaMalloc() for allocation.
             */
            if(tgtIf == DMA_TGT_SRAM)
            {
                cacheInvalidate(DATA_CACHE, dstMembase, xferSize);
            }
            
            dstChecksum = frcCheckSum((unsigned int *)dstMembase, (xferSize/4));
#ifdef DEBUG
            printf("[%d] dst = %08x\n", frcCount1, dstChecksum);
#endif
            if(srcChecksum != dstChecksum)
            {
                printf("DMA Error: Checksum src = 0x%08x dst = %08x\n", 
                        srcChecksum, dstChecksum);
            }
            else
                printf("Thread 1: DMA test[%d] passed\n",frcCount1);

#if FALSE
            /* Clean up destination */
            memset((void *)dstMembase, 0x0, xferSize);
#endif
        }

        /* Calculate checksum */
        srcChecksum = frcCheckSum((unsigned int *)srcMembase, (xferSize/4));
#ifdef DEBUG
        printf("[%d] src = %08x\n", frcCount1, srcChecksum);
#endif

        /* Setup for transfer */
        gtDmaTransfer(dmaEngine, (unsigned int)srcMembase, 
                     (unsigned int) dstMembase, xferSize,
                     DMA_BLOCK_TRANSFER_MODE|dmaDtl, srcIf, tgtIf, 
                     (DMA_RECORD *)NULL); 

        frcCount1++;

        /* Reset frcCount1 */
        if(frcCount1 == 0x0FFFFFFF) frcCount1 = 1;

        if(loopCnt != -1) 
            loopEnd--;
    }
}

void frcDmaCompleteAppIntHandler0(int no)
{
#ifdef GPP_DEBUG
    unsigned int data = 0;
#endif /* GPP_DEBUG */

    int sysIntOldConfig;
    sysIntOldConfig = intLock();

#ifdef DEBUG
    logMsg("\nfrcDmaCompleteAppIntHandler0[%d]: Entering...",no,0,0,0,0,0);
#endif

#ifdef GPP_DEBUG

    GT_REG_READ(GPP_VALUE, &data);

    /* GPP[2] */
    if(data & 0x00000004)
    {
        /* Pulse low */
        data &= 0xFFFFFFFB;
        GT_REG_WRITE(GPP_VALUE, data);
    }
    else
    {
        /* Pulse high */
        data |= (0x1 << 2);
        GT_REG_WRITE(GPP_VALUE, data);
    }

#endif /* GPP_DEBUG */

    /* Give semaphore */
    semGive(frcDmaSem0);

#ifdef DEBUG
    logMsg("\nfrcDmaCompleteAppIntHandler0[%d]: Exiting...",no,0,0,0,0,0);
#endif

   intUnlock(sysIntOldConfig);
}

void frcDmaCompleteAppIntHandler1(int no)
{
#ifdef GPP_DEBUG
    unsigned int data = 0;
#endif /* GPP_DEBUG */

    int sysIntOldConfig;
    sysIntOldConfig = intLock();

#ifdef DEBUG
    logMsg("\nfrcDmaCompleteAppIntHandler1[%d]: Entering...",no,0,0,0,0,0);
#endif

#ifdef GPP_DEBUG
    GT_REG_READ(GPP_VALUE, &data);

    /* GPP[10] */
    if(data & 0x00000400)
    {
        /* Pulse low */
        data &= 0xFFFFFBFF;
        GT_REG_WRITE(GPP_VALUE, data);
    }
    else
    {
        /* Pulse high */
        data |= (0x1 << 10);
        GT_REG_WRITE(GPP_VALUE, data);
    }
#endif /* GPP_DEBUG */

    /* Give semaphore */
    semGive(frcDmaSem1);

#ifdef DEBUG
    logMsg("\nfrcDmaCompleteAppIntHandler1[%d]: Exiting...",no,0,0,0,0,0);
#endif

   intUnlock(sysIntOldConfig);
}

void frcDmaTestStart0(DMA_ENGINE dmaEngine, unsigned int xferSize, 
                     char srcIf, char tgtIf, char dmaData,
                     unsigned int dmaDtl, int loopCnt)
{
    /* Spawn DMA Test task */
    if(taskSpawn("tDmaTask0", 100, 0, 4000, (FUNCPTR) frcDmaTestTaskEntry0,
                  dmaEngine, xferSize, srcIf, tgtIf, dmaData, dmaDtl, loopCnt, 
                  0, 0, 0) == ERROR)
    {
        printf("Unable to spawn tDmaTask0.\n");
        return;
    }
}

void frcDmaTestStart1(DMA_ENGINE dmaEngine, unsigned int xferSize, 
                     char srcIf, char tgtIf, char dmaData,
                     unsigned int dmaDtl, int loopCnt)
{
    /* Spawn DMA Test task */
    if(taskSpawn("tDmaTask1", 100, 0, 4000, (FUNCPTR) frcDmaTestTaskEntry1,
                  dmaEngine, xferSize, srcIf, tgtIf, dmaData, dmaDtl, loopCnt, 
                  0, 0, 0) == ERROR)
    {
        printf("Unable to spawn tDmaTask1.\n");
        return;
    }
}
