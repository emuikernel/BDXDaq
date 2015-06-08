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
* gtInitLib.h - Header File for : GT Initialization library header
*
* DESCRIPTION:
*       None.
*
* DEPENDENCIES:
*       None.
*
******************************************************************************/

#ifndef __INCgtInitLibh
#define __INCgtInitLibh

/* includes */
#ifndef _ASMLANGUAGE
    #include "gtCore.h"
#endif _ASMLANGUAGE


/* defines  */

/* Use this register to store the detected DIMM configuration */
#define BANK0_SIZE_REG          DMA_CHANNEL0_SOURCE_ADDR
#define BANK1_SIZE_REG          DMA_CHANNEL0_BYTE_COUNT
#define CPU1_STACK_ADRS_REG     DMA_CHANNEL0_DESTINATION_ADDR

/* typedefs */


/* gtInitLib.h API list */
#ifndef _ASMLANGUAGE
    void         gtInit         (void);
    unsigned int gtInitSdramSize(void);
    void         gtInitWindows  (void);
#endif _ASMLANGUAGE

/* Function declaration */
/* IMPORT void frcPermitCPU1ToBoot(void); */

#endif /* __INCgtInitLibh */

