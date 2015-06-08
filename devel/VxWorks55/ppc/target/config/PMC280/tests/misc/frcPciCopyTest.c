#ifdef CONFIG_PMC280

#include "vxWorks.h"
#include "stdio.h"
#include "taskLib.h"

/*
 * This module defines all the functions necessary to do copy
 * from the DDR on PMC260  (through PCI) into DDR on PMC260 (through PCI)
 * and verifies the data integrity through a checksum.
 *
 * Note: This is a plain copy and *not* DMA copy.
 *
 * Test Logic:
 *
 * Test Setup:
 * 
 * Test Procedure:
 *
 */
#undef DEBUG
extern unsigned int frcCheckSum(unsigned int*, unsigned int);
void *memcpy();
void *memset();
void free();

void frcPciCopyTestTaskEntry(unsigned int srcAddr,
                             unsigned int dstAddr, 
                             unsigned int xferSize,
                             unsigned int dmaData,
                             unsigned int loopCnt)
{
    unsigned int srcChecksum = 0, dstChecksum = 0, loopEnd = 0;
    unsigned int frcCount = 0, srcLocal = 0, dstLocal = 0;;

    printf("srcAddr = %08x, dstAddr = %08x, xferSize = %08x, dmaData = %08x, loopCnt = %08x\n", srcAddr, dstAddr, xferSize, dmaData, loopCnt);

    /*
     * If srcAddr or dstAddr is 0x0, then the source or destination
     * of the transfer is local memory. We will need to allocate space
     * for it.
     */
   
    if(srcAddr == 0x0)
    {
        srcLocal = 1;
#if FALSE
        srcAddr = (char *) cacheDmaMalloc(xferSize);
#endif
        srcAddr =(unsigned int) (char *) malloc(xferSize);
        if((char*)srcAddr == NULL)
        {
            printf("ERROR: Unable to allocate memory for test\n");
            return;
        }
    }

    /* Fill data at the source */
    memset((void *)srcAddr, dmaData, xferSize);

    if(dstAddr == 0x0)
    {
        dstLocal = 1;
#if FALSE
        dstAddr = (char *) cacheDmaMalloc(xferSize);
#endif
        dstAddr = (unsigned int)(char *) malloc(xferSize);
        if(dstAddr == 0)
        {  
            printf("ERROR: Unable to allocate memory for test\n");
            return;
        }
    }

    /* Clean up destination */
    memset((void *)dstAddr, 0x0, xferSize);

    if(loopCnt == -1) loopEnd = 1;
    else loopEnd = loopCnt;

    while(loopEnd)
    {
        /* Give the other tasks chance to run! */
        taskDelay(10);

        if(frcCount != 0)
        {
            dstChecksum = frcCheckSum((unsigned int *)dstAddr, (xferSize/4));
#ifdef DEBUG
            printf("[%d] dst = %08x\n", frcCount, dstChecksum);
#endif
            if(srcChecksum != dstChecksum)
            {
                printf("Copy Error: Checksum src = 0x%08x dst = %08x\n",
                        srcChecksum, dstChecksum);
            }
            else
                printf("frcPciCopy: Loop = %08x -- Pass\n", frcCount);

            /* Clean up destination */
            memset((void *)dstAddr, 0x0, xferSize);
        }

        /* Calculate checksum */
        srcChecksum = frcCheckSum((unsigned int *)srcAddr, (xferSize/4));
#ifdef DEBUG
        printf("[%d] src = %08x\n", frcCount, srcChecksum);
#endif

        memcpy(dstAddr, srcAddr, xferSize);

        frcCount++;

        /* Reset frcCount */
        if(frcCount == 0x0FFFFFFF) frcCount = 1;

        if(loopCnt != -1)
            loopEnd--;
    }

    if(srcLocal) free((void *)srcAddr);
    if(dstLocal) free((void *)dstAddr);
}

/*
 * Entry point for test -- To be called from shell.
 *
 * Note: Set up the following before running this test from shell.
 *
 *     . Internal Register Space BAR in PCI Configuration Space of Fn. 0
 *       (makes it possible to access internal register space of GT64260
 *        from PCI)
 *     . Set up the following from PMC280:
 *         . CS[0] BAR Size in GT64260 Internal Register space
 *         . CS[0] BAR in PCI Configuration Space of Fn. 0
 *           (after writing 0xFFFFFFFF to it and getting the size)
 *         . CS[0] Remap register in GT64260 Internal Register space
 */
void frcPciCopyTestStart(unsigned int dstAddr, 
                         unsigned int srcAddr, 
                         unsigned int xferSize,
                         unsigned int dmaData,
                         unsigned int loopCnt)
{
    /* Spawn PciCopy Test task */
    if(taskSpawn("tMemCopyTask", 100, 0, 4000, 
                  (FUNCPTR) frcPciCopyTestTaskEntry,
                  dstAddr, srcAddr, xferSize, dmaData, loopCnt,
                  0, 0, 0, 0, 0) == ERROR)
    {
        printf("Unable to spawn tMemCopyTask.\n");
        return;
    }
}
#endif /* CONFIG_PMC280 */
