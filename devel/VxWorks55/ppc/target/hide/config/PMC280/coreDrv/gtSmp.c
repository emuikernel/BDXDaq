/******************************************************************************
*              (c), Copyright, FORCE COMPUTERS INDIA Limited                  *
*                                                                             *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF FORCE COMPUTERS, INC.        *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT *
* OF FORCE OR ANY THIRD PARTY. FORCE RESERVES THE RIGHT AT ITS SOLE DISCRETION*
* TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO FORCE COMPUTERS.       *
* THIS CODE IS PROVIDED "AS IS". FORCE COMPUTERS MAKES NO WARRANTIES, EXPRESS,*
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.  *
******************************************************************************/


#ifdef PMC280_DUAL_CPU
/*******************************************************************************
* gtSMP.c - SMP functions and definitions
*
* DESCRIPTION:
*     This file gives the user a complete interface to the SMP functionality of
*     MV64360.
*
* DEPENDENCIES:
*       None.
*
*******************************************************************************/

/* includes */
#include "gt64360r.h"
#include "gtSmp.h"
#include "vxWorks.h"
#include "gtSysGal.h"

/* externs */
extern unsigned int gtInternalRegBaseAddr;

int frcMv64360semTake(unsigned int sem_reg_no, unsigned int timeout)
{
    unsigned int sem_register_offset, data;

    sem_register_offset = SMP_SEMAPHOR0 + (sem_reg_no * 8);
    GT_REG_READ(sem_register_offset, &data);

    if(data != frcWhoAmI())
    {
        /* Semaphore is held by somebody else */
        if(timeout == MV64360_SMP_NO_WAIT)
            return ERROR;
        else if(timeout == MV64360_SMP_WAIT_FOREVER)
        {
            /* Spin here */
            while(GT_REG_READ(sem_register_offset, &data) != frcWhoAmI());
            return OK;
        }
        else return ERROR;
    }
    else
    {
        /* Nobody has the semaphore, its ours */
        return OK;
    }
}

int frcMv64360semGive(unsigned int sem_reg_no)
{
    unsigned int sem_register_offset, data;

    sem_register_offset = SMP_SEMAPHOR0 + (sem_reg_no * 8);
    GT_REG_READ(sem_register_offset, &data);

    if(data != frcWhoAmI())
    {
        /* We don't own it, so can't unlock it */
        return ERROR;
    }
    else
    {
        /* Unlock it */
        GT_REG_WRITE(sem_register_offset, 0xFF);
        return OK;
    }
}

#endif /* PMC280_DUAL_CPU */
