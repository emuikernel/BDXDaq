/******************************************************************************
*              (c), Copyright, MOTOROLA  INC  - ECC                  *
*                                                                             *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MOTOROLA , INC.        *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT *
* OF MOTOROLA  OR ANY THIRD PARTY. MOTOROLA  RESERVES THE RIGHT AT ITS SOLE DISCRETION*
* TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MOTOROLA .       *
* THIS CODE IS PROVIDED "AS IS". MOTOROLA  MAKES NO WARRANTIES, EXPRESS,*
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.  *
******************************************************************************/



#ifdef PMC280_DUAL_CPU
/*
 * Test stubs for Doorbell interrupts and Inter-CPU communication
 */
#include "vxDbIntCtrl.h"
#include "gtSmp.h"
#include "semLib.h"
#include "vxWorks.h"
#include "stdio.h"
#include "taskLib.h"
#include "string.h"

#undef DEBUG

/*
 * This is how this works:
 *
 * A task, say tDbTask is created on CPU0, which initialises Doorbell 
 * interrupts on CPU0. This fills up some portion of the shared memory
 * region with some data and calculates the checksum. It then signals 
 * CPU1 through Doorbell (that the data is ready) and waits for the signal
 * from CPU1 that the work is completed. It then fills up the same region
 * with some other data and so on...
 *
 * A task, say tDbTask is created on CPU1, which waits for a signal 
 * from CPU0, processes data (calculates checksum) and stores the checksum 
 * in a pre-defined location and signals CPU0 that its work is done, and waits 
 * for more work from CPU0 and so on...
 *
 * In the current setup, the shared memory for applications begins at:
 * PHY_CPU0_APP_SHMEM_BOT. Since there is right now *no* mechanism to 
 * allocate memory (no shared memory management routines), the applications
 * using shared memory are responsible for ensuring that their memory
 * allocation does not clash with other shared memory applications.
 */ 
unsigned int frcSharedMemoryBase;
unsigned int frcSharedMemorySize;
unsigned int *frcCheckSumLocation;
static unsigned int frcCount = 0;
SEM_ID frcSharedSem;

void frcDbAppIntHandler(int no);

static unsigned int frcCheckSum(unsigned int *start, unsigned int size)
{
    unsigned int checksum = 0;
    while(size > 0)
    {
        checksum +=  *start;
        start++;
        size--;
    }
    return checksum;
}

void frcDbTestInit(unsigned int db)
{
    /* Initialize Doorbell interrupt module */
    frcDbIntCtrlInit();

    /* Attach all the handlers */
    frcDbIntConnect(db, frcDbAppIntHandler, 0, 1);

    /* Enable all Doorbell interrupts */
    frcDbIntEnable(db);
}

void frcDbTestTaskEntry(unsigned int membase, unsigned int memsize, int iter)
{
    unsigned int db = 0, checksum = 0;

    /* Create semaphore */
    frcSharedSem = semBCreate(SEM_Q_FIFO, SEM_EMPTY);

    frcSharedMemoryBase = membase;
    frcSharedMemorySize = memsize;

    frcCheckSumLocation = (unsigned int *) (frcSharedMemoryBase + frcSharedMemorySize);

    /* Initialise the Doorbell interrupt sub-system */
    frcDbTestInit(db);

    while(iter >= 0)
    {
        iter--;
        if(frcWhoAmI())
        {
            /* Wait for signal that work is has come */
            semTake(frcSharedSem, WAIT_FOREVER);

            /* Calculate checksum */ 
            checksum = frcCheckSum((unsigned int *)frcSharedMemoryBase, 
                                   frcSharedMemorySize);

#ifdef DEBUG
            printf("[%d] %08x\n", frcCount, checksum);
#endif
            
            /* Put checksum at frcCheckSumLocation */
            *frcCheckSumLocation = checksum;

            /* Signal the other CPU to send new data */
            frcDbIntSend(db);

            frcCount++;
        }
        else
        {
            if(frcCount != 0)
            {
                /* Wait for signal that work is done */
                semTake(frcSharedSem, WAIT_FOREVER);

                /* Verify checksum */ 
                if(checksum != *frcCheckSumLocation)
                    printf("Shared memory application failed!\n");
                else
                    printf("Shared memory application passed!\n");
            }
        
            memset((void *)frcSharedMemoryBase, frcCount, frcSharedMemorySize);

            /* Calculate checksum */ 
            checksum = frcCheckSum((unsigned int *)frcSharedMemoryBase, 
                                   frcSharedMemorySize);
#ifdef DEBUG
            printf("[%d] %08x\n", frcCount, checksum);
#endif

            /* Signal the other CPU to work on this data */
            frcDbIntSend(db);

            frcCount++;
        }
    }
}

void frcDbAppIntHandler(int no)
{
#ifdef DEBUG
    logMsg("\nfrcDbAppIntHandler[%d]: Entering...",no,0,0,0,0,0);
#endif

    /* Give semaphore */
    semGive(frcSharedSem);

#ifdef DEBUG
    logMsg("\nfrcDbAppIntHandler[%d]: Exiting...",no,0,0,0,0,0);
#endif
}

void frcDbTestStart(unsigned int membase, unsigned int memsize, int iter)
{
    /* Spawn Doorbell Test task */
    if(taskSpawn("tDbTask", 100, 0, 4000, (FUNCPTR) frcDbTestTaskEntry, 
                  membase, memsize, iter, 0, 0, 0, 0, 0, 0, 0) == ERROR)
    {
        printf("Unable to spawn tDbTestTask.\n");
        return;
    }
}
#endif /* PMC280_DUAL_CPU */
