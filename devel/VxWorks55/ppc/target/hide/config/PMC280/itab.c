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
* itab.c - System Initialisation Table
*
* DESCRIPTION:
*       This module contains the recommended initialisation values for chip
*       registers on the board. Most of these registers are written once.
*       The remainder are rewritten as required.
*
* DEPENDENCIES:
*       None.
*
******************************************************************************/

/* includes */
#include "gtCore.h"


/* defines  */


/* typedefs */


/* locals   */

/*
 * NOTE: this initialisation table is used during basic initialisation
 */

struct vrent
{
    unsigned int    reg;
    unsigned int    value;
};

struct vrent reginittab[] =
{     
    /* Activate support for read out of order completion and pipeline */
#ifdef PMC280_DUAL_CPU
     /* Bit[11] is set to 0 for Dual CPU */
    {CPU_CONFIG, 0x06ca20ff },  /* CPU Configuration Register */ 
#else
    {CPU_CONFIG, 0x06ca28ff },  /* CPU Configuration Register */ 
#endif

    {GPP_INTERRUPT_CAUSE, 0}, /* clean the GPP cause register */

#if 0 /*  */
    {COMM_UNIT_ARBITER_CONTROL, 0x11ff0400},

    /* PCI */
    {PCI_0_TIMEOUT_RETRY,   0x00FFFFFF},
    {PCI_1_TIMEOUT_RETRY,   0x00FFFFFF}, 
#endif

    {PCI_0_BASE_ADDR_REG_ENABLE, 0xffffffff}, /* PMC280 */
    {PCI_1_BASE_ADDR_REG_ENABLE, 0xffffffff}, /* PMC280 */

    {0,0} /* terminator */                              
};                                                     
