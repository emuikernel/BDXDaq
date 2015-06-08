/******************************************************************************
*              (c), Copyright, MOTOROLA  INC  - ECC                  *
*                                                                             *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MOTOROLA , INC.        *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT *
* OF MOTOROLA  OR ANY THIRD PARTY. MOTOROLA  RESERVES THE RIGHT AT ITS SOLE DISCRETION*
* TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MOTOROLA .       *
* THIS CODE IS PROVIDED "AS IS". MOTOROLA  MAKES NO WARRANTIES, EXPRESS,*
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.  *
*******************************************************************************

 *
 * Test stubs for IDMA engines of MV64360.
 */
 #ifdef PMC280_DUAL_CPU 
 
#include "vxDmaIntCtrl.h"
#include "gtDma.h"
#include "gtSram.h"
#include "semLib.h"
#include "vxWorks.h"
#include "stdio.h"
#include "taskLib.h"
#include "gtSmp.h"
#include "cacheLib.h"
#include "config.h"
#include "userRsrvMem.h"
#include <string.h>

#undef DEBUG
#undef GPP_DEBUG

#define ALIGN_DRAM	10
#define ALIGN_SRAM      10

void frcDmaCompleteAppIntHandler(int no);

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
#define DMA_CPU1_SRAM_BASE   (INTERNAL_SRAM_ADRS + (INTERNAL_SRAM_SIZE/2))
#define DMA_CPU0_SRAM_SIZE   (INTERNAL_SRAM_SIZE/2)
#define DMA_CPU1_SRAM_SIZE   (INTERNAL_SRAM_SIZE/2)

/* PCI */
/*
 * It is assumed that memory on the PCI target (which is either PMC260
 * or PMC270) has its SDRAM opened for access from PCI. However since
 * we do not know what memory  *not* used by the target, we 
 * cannot use *any* address for IDMA. Since memory from 0x0 to 0x2000
 * is reserved for exceptions, we will assume that 0x2000 onwards till
 * 0x80000 can be used for IDMA. 
 */

#define GUARD                0x2000
#define DMA_CPU0_PCI_BASE    (PCI0_MEM0_BASE_ADRS)
#define DMA_CPU0_PCI_SIZE    0x02000000

static unsigned int frcDram_base;
static unsigned int frcDram_size;

static unsigned int frcSram_base;
static unsigned int frcSram_size;

static unsigned int frcPci_base;
static unsigned int frcPci_size;

unsigned int frcCount = 0;
static SEM_ID frcDmaSem;

/*
 * This is how this works:
 *
 * A task, say tIdmaTask on any CPU copies data from DRAM/SRMA/PCI to one of
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
        printf("[0x%08x] %08x\n", (int) start, (int *)*start);
#endif
        start++;
        size--;
    }
    return checksum;
}

LOCAL unsigned int frcAlignedMalloc(unsigned int size, unsigned int alignment)
{
    unsigned int blkBase, alignmentMask = 0;

    /* Add aligment bytes to the block size for aligment space */
    blkBase = (unsigned int) cacheDmaMalloc(size + (1 << alignment));
    if(blkBase == (unsigned int) NULL)
    {
        printf("Unable to allocate memory\n");
        return ERROR;
    }

#ifdef DEBUG
    printf("blkBase = 0x%08x\n", (int)blkBase);
#endif

    /* If malloc is already aligned */
    if(blkBase % (1 << alignment) != 0)
    {
        /* Mask for getting the address low bits for checking the aligment */
        alignmentMask = ~(0xFFFFFFFF << alignment);
#ifdef DEBUG
        printf("alignmentMask = 0x%08x\n", (int)alignmentMask);
#endif

        /* Add offset to the block base to complete the aligment */
        blkBase += (1 << alignment) - (alignmentMask & blkBase);
    }

    return blkBase;
}

unsigned int frcIdmaTestInit(DMA_ENGINE dmaEngine, char srcIf, char tgtIf)
{
    unsigned int compCause;
#ifdef GPP_DEBUG
    unsigned int data;
#endif /* GPP_DEBUG */

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

    /* Setup appropriate decode windows for IDMA engine */
    if(!frcWhoAmI())
    {
        frcDram_base = DMA_CPU0_DRAM_BASE;
        frcDram_size = DMA_CPU0_DRAM_SIZE;

        frcSram_base = DMA_CPU0_SRAM_BASE;
        frcSram_size = DMA_CPU0_SRAM_SIZE;

        frcPci_base = DMA_CPU0_PCI_BASE;
        frcPci_size = DMA_CPU0_PCI_SIZE;
    }

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
            if(!frcWhoAmI())
            {
                gtDmaSetMemorySpace(DMA_WINDOW0, DMA_DRAM_CS0, 0,
                                    frcDram_base, frcDram_size);
            }
            break;
        case DMA_SRC_SRAM:
            if(!frcWhoAmI())
            {
                gtDmaSetMemorySpace(DMA_WINDOW0, DMA_SRAM, 0,
                                    frcSram_base, frcSram_size);
            }
            break;
        case DMA_SRC_PCI:
            if(!frcWhoAmI())
            {
                gtDmaSetMemorySpace(DMA_WINDOW0, DMA_PCI_0|BIT11|BIT12, 0,
                                    frcPci_base, frcPci_size);
            }
            break;
        default:
            printf("Undefined source type\n");
            return ERROR;
    }

    switch(tgtIf)
    {
        case DMA_TGT_DRAM:
            if(!frcWhoAmI())
            {
                if(srcIf != DMA_SRC_DRAM)
                    gtDmaSetMemorySpace(DMA_WINDOW1, DMA_DRAM_CS0, 0,
                                        frcDram_base, frcDram_size);
            }
            break;

        case DMA_TGT_SRAM:
            if(!frcWhoAmI())
            {
                if(srcIf != DMA_SRC_SRAM)
                    gtDmaSetMemorySpace(DMA_WINDOW1, DMA_SRAM, 0,
                                        frcSram_base, frcSram_size);
            }
            break;

        case DMA_TGT_PCI:
            if(!frcWhoAmI())
            {
                if(srcIf != DMA_SRC_PCI)
                    gtDmaSetMemorySpace(DMA_WINDOW1, DMA_PCI_0|BIT11|BIT12, 0,
                                        frcPci_base, frcPci_size);
            }
            break;
        default:
            printf("Undefined destination type\n");
            return ERROR;
    }

    /* Attach DMA completion handler */
    frcDmaIntConnect(compCause, frcDmaCompleteAppIntHandler, 0, 1);

    /* Enable DMA completion interrupt */
    frcDmaIntEnable(compCause);

#ifdef GPP_DEBUG
    if(frcWhoAmI())
    {
        frcMv64360semTake(GPP_SEM, MV64360_SMP_WAIT_FOREVER);

        /* Make GPP[10] as Output pin */
        GT_REG_READ(GPP_IO_CONTROL, &data);
        data |= (0x1 << 10);
        GT_REG_WRITE(GPP_IO_CONTROL, data);

        frcMv64360semGive(GPP_SEM);
    }
    else
    {
        frcMv64360semTake(GPP_SEM, MV64360_SMP_WAIT_FOREVER);

        /* Make GPP[2] as Output pin */
        GT_REG_READ(GPP_IO_CONTROL, &data);
        data |= (0x1 << 2);
        GT_REG_WRITE(GPP_IO_CONTROL, data);

        frcMv64360semGive(GPP_SEM);
    }
#endif /* GPP_DEBUG */

    return OK;
}

void frcDmaTestTaskEntry(DMA_ENGINE dmaEngine, unsigned int xferSize, 
                         char srcIf, char tgtIf, char dmaData, 
                         unsigned int dmaDtl, int loopCnt)
{
    unsigned int srcChecksum = 0, dstChecksum = 0, loopEnd = 0;
    unsigned int *srcMembase = 0, *dstMembase = 0;

    printf("dmaEngine = %d, xferSize = 0x%08x, srcIf = %d tgtIf = %d 
            dmaData = %08x dmaDtl = %08x\n", dmaEngine, (int)xferSize, srcIf, 
            tgtIf, dmaData, (int)dmaDtl);

    /* Create semaphore */
    frcDmaSem = semBCreate(SEM_Q_FIFO, SEM_EMPTY);

    /* Initialise DMA engine for tests */
    if(frcIdmaTestInit(dmaEngine, srcIf, tgtIf) == ERROR)
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
    else loopEnd = loopCnt + 1;

    frcCount=0;

    while(loopEnd)
    {
        if(frcCount != 0)
        {
            /* Wait for signal that work is done */
            semTake(frcDmaSem, WAIT_FOREVER);
           
            dstChecksum = frcCheckSum((unsigned int *)dstMembase, (xferSize/4));
#ifdef DEBUG
            printf("[%d] dst = %08x\n", frcCount, dstChecksum);
#endif
            if(srcChecksum != dstChecksum)
            {
                printf("DMA Error: Checksum src = 0x%08x dst = %08x\n", 
                        srcChecksum, dstChecksum);
            }
            else
                printf("DMA test [%d] passed.\n",frcCount);

            /* Clean up destination */
            memset((void *)dstMembase, 0x0, xferSize);
        }

        /* Calculate checksum */
        srcChecksum = frcCheckSum((unsigned int *)srcMembase, (xferSize/4));
#ifdef DEBUG
        printf("[%d] src = %08x\n", frcCount, srcChecksum);
#endif

        /* Setup for transfer */
        gtDmaTransfer(dmaEngine, (unsigned int) srcMembase, 
                     (unsigned int) dstMembase, xferSize,
                     DMA_BLOCK_TRANSFER_MODE|dmaDtl, srcIf, tgtIf, 
                     (DMA_RECORD *)NULL); 

        frcCount++;

        /* Reset frcCount */
        if(frcCount == 0x0FFFFFFF) frcCount = 1;

        if(loopCnt != -1)
           loopEnd--;
    }
}

void frcDmaCompleteAppIntHandler(int no)
{
#ifdef GPP_DEBUG
    unsigned int data = 0;
#endif /* GPP_DEBUG */

#ifdef DEBUG
    logMsg("\nfrcDmaCompleteAppIntHandler[%d]: Entering...",no,0,0,0,0,0);
#endif

#ifdef GPP_DEBUG
    frcMv64360semTake(GPP_SEM, MV64360_SMP_WAIT_FOREVER);

    GT_REG_READ(GPP_VALUE, &data);
    if(frcWhoAmI())
    {
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
    }
    else
    {
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
    }

    frcMv64360semGive(GPP_SEM);
#endif /* GPP_DEBUG */

    /* Give semaphore */
    semGive(frcDmaSem);

#ifdef DEBUG
    logMsg("\nfrcDmaCompleteAppIntHandler[%d]: Exiting...",no,0,0,0,0,0);
#endif
}

void frcDmaTestStart(DMA_ENGINE dmaEngine, unsigned int xferSize, 
                     char srcIf, char tgtIf, char dmaData, 
                     unsigned int dmaDtl, int loopCnt)
{
    /* Spawn DMA Test task */
    if(taskSpawn("tDmaTask", 100, 0, 4000, (FUNCPTR) frcDmaTestTaskEntry,
                  dmaEngine, xferSize, srcIf, tgtIf, dmaData, dmaDtl, loopCnt, 
                  0, 0, 0) == ERROR)
    {
        printf("Unable to spawn tDmaTask.\n");
        return;
    }
}

#endif /*PMC280_DUAL_CPU */
