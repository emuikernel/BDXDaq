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

*******************************************************************************
* vxDmaIntCtrl.h - Header File for : DMA interrupt controller driver
*
* DESCRIPTION:
*       None.
*
* DEPENDENCIES:
*       None.
*
******************************************************************************/

#ifndef __INCvxDmaIntCtrlh
#define __INCvxDmaIntCtrlh

/* includes */
#include "gtIntControl.h"


/* defines  */


/* typedefs */

/* DMA_CAUSE is an enumerator that moves between 0 and 31. This enumerator */
/* describes the cause bits positions in the DMA cause  registers.         */

typedef enum _dmaCause
{
    /* DMA channels 0-3 Interrupt cause register (0x8c0) */
    DMA_CAUSE_START = -1, /* Cause Start boundry */
    DMA_CHAN0_COMP      , /* Channel0 DMA Completion.                        */
    DMA_CHAN0_ADDR_MISS , /* Channel0 Address Miss / Failed address decoding.*/
    DMA_CHAN0_ACC_PROT  , /* Channel0 Access Protect Violation               */
    DMA_CHAN0_WR_PROT   , /* Channel0 Write Protect Violation                */
    DMA_CHAN0_OWN       , /* Channel0 Descriptor Ownership Violation         */
    DMA_RSRVD_L05       ,
    DMA_RSRVD_L06       ,
    DMA_RSRVD_L07       ,
    DMA_CHAN1_COMP      , /* Channel1 DMA Completion.                        */
    DMA_CHAN1_ADDR_MISS , /* Channel1 Address Miss / Failed address decoding.*/
    DMA_CHAN1_ACC_PROT  , /* Channel1 Access Protect Violation               */
    DMA_CHAN1_WR_PROT   , /* Channel1 Write Protect Violation                */
    DMA_CHAN1_OWN       , /* Channel1 Descriptor Ownership Violation         */
    DMA_RSRVD_L13       ,
    DMA_RSRVD_L14       ,
    DMA_RSRVD_L15       ,
    DMA_CHAN2_COMP      , /* Channel2 DMA Completion.                        */
    DMA_CHAN2_ADDR_MISS , /* Channel2 Address Miss / Failed address decoding.*/
    DMA_CHAN2_ACC_PROT  , /* Channel2 Access Protect Violation               */
    DMA_CHAN2_WR_PROT   , /* Channel2 Write Protect Violation                */
    DMA_CHAN2_OWN       , /* Channel2 Descriptor Ownership Violation         */
    DMA_CHAN2_L21       ,
    DMA_RSRVD_L22       ,
    DMA_RSRVD_L23       ,
    DMA_CHAN3_COMP      , /* Channel3 DMA Completion.                        */
    DMA_CHAN3_ADDR_MISS , /* Channel3 Address Miss / Failed address decoding.*/
    DMA_CHAN3_ACC_PROT  , /* Channel3 Access Protect Violation               */
    DMA_CHAN3_WR_PROT   , /* Channel3 Write Protect Violation                */
    DMA_CHAN3_OWN       , /* Channel3 Descriptor Ownership Violation         */
    DMA_RSRVD_L29       ,
    DMA_RSRVD_L30       ,
    DMA_RSRVD_L31       ,
    DMA_CAUSE_END   /* Cause End boundry */
} DMA_CAUSE;

        
/* vxDmaIntCtrl.h API list */
void    frcDmaIntCtrlInit (void); 
STATUS  frcDmaIntConnect  (DMA_CAUSE cause, VOIDFUNCPTR routine,
                          int parameter, int prio);
STATUS  frcDmaIntEnable   (DMA_CAUSE cause);
STATUS  frcDmaIntDisable  (DMA_CAUSE cause);

#endif /* __INCvxDmaIntCtrlh */

