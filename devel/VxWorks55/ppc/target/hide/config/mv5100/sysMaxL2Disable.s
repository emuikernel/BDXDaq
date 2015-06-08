/* sysMaxL2Disable.s - MPC7400 (MAX) L2 Cache Disable Route */

/* Copyright 2000 Wind River Systems, Inc. */
/* Copyright 2000 Motorola, Inc. All Rights Reserved */

/*
modification history
--------------------
01e,12jun01,dtr  Putting in INCLUDE_ALTIVEC around dssall.
01d,10jun01,dtr  Added sysMaxL2Flush,sysMaxL2InvFunc functions.
01c,09may01,pch  Add .type directives
01b,16aug00,dmw  Wind River coding standards review.
01a,26jul00,dmw  Created
*/

/*
DESCRIPTION
This module contains routines written in assembly language to address a
MAX processor cache errata.  The following is the response from SPS
regarding the problem:

The problem exists on all versions of the MPC7400 (MAX) processor and is 
not expected to be fixed.  A fix is targeted for revision 1.3 of the MPC7410 
(Nitro) processor.

Overview:

The MPC7400 can hang if the L2 is disabled during an outstanding instruction
fetch.

Detailed Description:

The problem centers around the interaction of the instruction cache and the
L2 cache as the L2 cache is disabled.  The scenario is as follows:

  1. An ifetch misses in the icache and allocates a reload table entry
  2. As the instructions return from the BIU they are forwarded around the
     icache and dispatched as well as written to the IRLDQ.
  3. One of these instruction is a mtspr targeting the L2CR.  This
     instruction  disables the L2.
  4. When all beats of data return to the IRLDQ, the IRLT arbitrates to
     reload the L2.  Since the L2 is now disabled, it does not expect reload
     requests from the IRLT.
  5. The unexpected reload request is mishandled by the L2 and passed to the
     BIU as an ifetch miss.

Workaround:
  1. Preload the code that disables the L2 into the instruction cache before
     execution.  This requires the code be structured in such a way that the
     instruction fetch be completed before the mtspr is executed.
*/

#define _ASMLANGUAGE

/* includes */

#include "vxWorks.h"
#include "sysLib.h"
#include "config.h"
#include "regs.h"    
#include "asm.h"
#include "sysCache.h"

/* globals */

   FUNC_EXPORT(sysMaxL2Disable)      /* Max L2 disable routine */
   FUNC_EXPORT(sysMaxL2Flush)        /* Max L2 flush routine */
   FUNC_EXPORT(sysMaxL2InvFunc)      /* Max L2 Cache Inavlidate routine */

   _WRS_TEXT_SEG_START

/*******************************************************************************
*
* sysMaxL2Disable - MAX Disable L2 Cache function
*
* This routine disables the MAX L2 cache by first flushing the L2 using
* the hardware flush bit in SPR 1017, L2CR.  It then turns off the enable
* bit in L2CR.
*
* SYNOPSIS
* \ss
* void sysMaxL2Disable (void)
* \se
*
* RETURNS: None
*/

FUNC_BEGIN(sysMaxL2Disable)

#ifdef INCLUDE_ALTIVEC
   .long 0x7E00066C			   /* Instruction for dssall */
   sync
#endif
   mfspr   r3,L2CR_REG

   /* Hardware flush L2 */

   lis    r4,HI(MPC750_L2CR_HWF)
   ori    r4,r4,LO(MPC750_L2CR_HWF)
   or     r3,r3,r4
   mtspr  L2CR_REG,r3          
   sync
   isync

   /* Disable the L2 cache */

   lis    r4,HI(MPC750_L2CR_E)
   ori    r4,r4,LO(MPC750_L2CR_E)
   mfspr  r3,L2CR_REG
   andc   r3,r3,r4
   sync

   b preFetchL2DisableCode

codeIsPrefetched:
   mtspr  L2CR_REG,r3          
   sync
   isync

   b pastFetchL2DisableCode

preFetchL2DisableCode:
   sync
   isync
   b codeIsPrefetched

pastFetchL2DisableCode:

   /* Return to caller */

   bclr    20,0
FUNC_END(sysMaxL2Disable)

/*******************************************************************************
*
* sysMaxL2Flush - MAX Flush L2 Cache function
*
* This routine disables the MAX L2 cache by stopping all data streams(dssall)
* and then flushing the L2 using the hardware flush bit in SPR 1017, L2CR.
*
* SYNOPSIS
* \ss
* void sysMaxL2Flush (void)
* \se
*
* RETURNS: None
*/

FUNC_BEGIN(sysMaxL2Flush)

#ifdef INCLUDE_ALTIVEC
   .long 0x7E00066C    			/* Instruction for dssall */
   sync
#endif
   mfspr   r3,L2CR_REG

   /* Hardware flush L2 */

   lis    r4,HI(MPC750_L2CR_HWF)
   ori    r4,r4,LO(MPC750_L2CR_HWF)
   or     r3,r3,r4
   mtspr  L2CR_REG,r3          
   sync
   isync

   /* Return to caller */

   bclr    20,0
FUNC_END(sysMaxL2Flush)

/*******************************************************************************
*
* sysMaxL2InvFunc - MAX Invalidate L2 Cache function
*
* This routine invalidates the MAX L2 cache. It stops all data streams then
* disables the cache and finally performs the hw global invalidate.
*
* SYNOPSIS
* \ss
* void sysMaxL2InvFunc (void)
* \se
*  
* RETURNS: None
*/

FUNC_BEGIN(sysMaxL2InvFunc)
    
   /* Instruction for dssall */
#ifdef INCLUDE_ALTIVEC
   .long 0x7E00066C    

    sync        
#endif
    mfspr   r3, L2CR_REG
    andis.  r3, r3, L2CR_DISABLE_MASK_U  /* disable L2 cache*/
    mtspr   L2CR_REG, r3
    sync

    oris    r3, r3, L2CR_GBL_INV_U      /* set global invalidate command */
    mtspr   L2CR_REG, r3
    sync
        
/*  
* Monitoring the L2CR[L2IP] bit to determine when the global 
* invalidation operation is completed.
*/

invalidate_in_progress:
    mfspr   r3, L2CR_REG
    andi.   r3, r3, L2CR_IP             
    bne     invalidate_in_progress


    sync        
    mfspr   r3, L2CR_REG
    rlwinm  r3, r3, 0, 11, 9         /* zero out the invalidate bit*/
    mtspr   L2CR_REG, r3
    sync

    bclr   20,0
FUNC_END(sysMaxL2InvFunc)

