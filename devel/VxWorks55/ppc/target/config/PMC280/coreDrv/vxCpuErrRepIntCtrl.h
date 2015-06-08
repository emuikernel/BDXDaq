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

*******************************************************************************
* vxCpuErrRepIntCtrl.h - Header File for : 
* 		CPU Error Report interrupt controller driver
*
* DESCRIPTION:
*       None.
*
* DEPENDENCIES:
*       None.
*
******************************************************************************/

#ifndef __INCvxCpuErrRepIntCtrlh
#define __INCvxCpuErrRepIntCtrlh

/* includes */
#include "gtIntControl.h"  

  
/* defines  */

       
/* typedefs */

/* CPU_ERR_CAUSE is an enumerator that moves between 0 and 31. */
/* This enumerator describes the cause bits positions in the   */
/* CPU Error Report cause register (offset 0x140).             */             

typedef enum cpuErrCause
{
    /* CPU Error Eeport Interrupt cause register */

    CPU_ERR_CAUSE_START = -1            , /* Cause Start boundry */
    CPU_ADDR_RANGE_ERR                  ,  
    CPU_ADDR_PARITY_ERR                 ,
    CPU_TRANSFER_TYPE_VIOLATION_ERR     ,
    CPU_ACCESS_PROTECTED_REGION_ERR     ,
    CPU_WRITE_TO_PROTECTED_REGION_ERR   ,
    CPU_READ_CACHE_PROTECTED_REGION_ERR ,
    CPU_BAD_WRITE_DATA_PARITY_DETECT_ERR,
    CPU_BAD_READ_DATA_PARITY_DETECT_ERR ,
    CPU_ERR_RSRVD_L08                   ,
    CPU_ERR_RSRVD_L09                   ,
    CPU_ERR_RSRVD_L10                   ,
    CPU_ERR_RSRVD_L11                   ,
    CPU_ERR_RSRVD_L12                   ,
    CPU_ERR_RSRVD_L13                   ,
    CPU_ERR_RSRVD_L14                   ,
    CPU_ERR_RSRVD_L15                   ,
    CPU_ERR_RSRVD_L16                   ,
    CPU_ERR_RSRVD_L17                   ,
    CPU_ERR_RSRVD_L18                   ,
    CPU_ERR_RSRVD_L19                   ,
    CPU_ERR_RSRVD_L20                   ,
    CPU_ERR_RSRVD_L21                   ,
    CPU_ERR_RSRVD_L22                   ,
    CPU_ERR_RSRVD_L23                   ,
    CPU_ERR_RSRVD_L24                   ,
    CPU_ERR_RSRVD_L25                   ,
    CPU_ERR_RSRVD_L26                   ,
    CPU_ERR_RSRVD_L27                   ,
    CPU_ERR_RSRVD_L28                   ,   
    CPU_ERR_RSRVD_L29                   ,     
    CPU_ERR_RSRVD_L30                   ,
    CPU_ERR_RSRVD_L31                   ,
    CPU_ERR_CAUSE_END   /* Cause End boundry */
} CPU_ERR_CAUSE;


/* Cpu Error Report Interrupt Controller's API */ 
void    vxCpuErrRepIntCtrlInit (void); 
STATUS  vxCpuErrRepIntConnect  (CPU_ERR_CAUSE cause, VOIDFUNCPTR routine,
                                int parameter, int prio);
STATUS  vxCpuErrRepIntEnable   (CPU_ERR_CAUSE cause);
STATUS  vxCpuErrRepIntDisable  (CPU_ERR_CAUSE cause);

#endif   /* __INCvxCpuErrRepIntCtrlh */

