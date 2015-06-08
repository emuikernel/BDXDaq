/* sysL2BackCache.s - L2 cache functions */

/* Copyright 1984-2000 Wind River Systems, Inc. */
/* Copyright 1996-1998 Motorola, Inc. */

/*
modification history
--------------------
01n,25mar02,pcs  Add support for PMC7455.
01m,24jan02,mil  Fixed Diab warning on comments.
01l,24jan02,mil  Changed define of CPU7410 to SP7410.
01k,22jan02,pcs  Merge from Tor2.1
01k,02jan02,pcs  Add L2 cache support for Vger 7450.
01j,12jun01,mil  Added dssall for L2 flush and invalidate.
01i,08may01,pch  Add assembler abstractions (FUNC_EXPORT, FUNC_BEGIN, etc.)
01h,13apr01,mil  Added -DCPU7410 to differentiate from 7400 on L2PM
01g,12apr01,pcs  ADD Workaround for L2 Cache BUG.
Code as received from Mot.
                 mv5100 BSP.
01f,11apr01,mil  Added PM support for 7410
01e,20mar01,kab  auto-merge.
01e,15mar01,kxb  Merge of Sekhar's mods.
01f,22jan01,pcs  Add changes as received from teamF1
01f,15jan01,ksn   doubled the flush size in sysL2BackSWFlush routine.(teamF1)
01e,28oct00,ksn   added routines for harware and software based Flushing
                  (teamF1)        
01d,12sep00,ksn   added support for L2 cache and L2PM + cleanups. (teamF1).	
01c,27oct98,ms_   modified to comply with WRS coding standards
01b,15sep98,My	  Added autosize for 256K backside, and detect MPC740. 
01a,31jul98,My&RGP  Created
*/

/* includes */

#define _ASMLANGUAGE
#include "vxWorks.h"
#include "cacheLib.h"        
#include "config.h"        
#include "sysL2BackCache.h"


	/* globals */

	FUNC_EXPORT(sysL2BackGlobalInv)
	FUNC_EXPORT(sysL2BackDisable)
	FUNC_EXPORT(sysL2BackEnable)
	FUNC_EXPORT(sysL1DcacheEnable)
	FUNC_EXPORT(sysL1DcacheDisable)
        FUNC_EXPORT(sysL2BackInit)
        FUNC_EXPORT(sysL2BackHWFlush)
                
#if defined(INCLUDE_L2PM) && (defined(SP755) || defined(SP7410))
        FUNC_EXPORT(sysL2PMEnable)
#endif /* INCLUDE_L2PM && (SP755 || SP7410) */        
        
   _WRS_TEXT_SEG_START
        	
/*********************************************************************
* sysL2BackGlobalInv - Globally invalidate L2 backside cache 
*
* This function reads the value of the l2cr register, disables
* the cache, before setting the global invalidate bit.  It waits
* for the L2CR[L2IP] bit is clear before returning to the caller.
* 
* RETURNS: N/A

* void sysL2BackGlobalInv
*     (
*     void
*     )

*/

FUNC_BEGIN(sysL2BackGlobalInv)
    
	mfspr	r9, PVR_REG		  /*  read PVR             */
	rlwinm	r9,r9,16,16,31		  /* shift down 16 bits    */
	cmpli	0,0,r9,CPU_TYPE_7450		  /* Vger pvr              */
	beq	cache_inval_L2_MPC745X
	cmpli	0,0,r9,CPU_TYPE_7455      /* 0x8001              */
	beq	cache_inval_L2_MPC745X
	cmpli	0,0,r9,CPU_TYPE_7447      /* 0x8002              */
	beq	cache_inval_L2_MPC745X
	cmpli	0,0,r9,CPU_TYPE_7447A     /* 0x8003              */
	beq	cache_inval_L2_MPC745X

        isync

        sync        
        mfspr   r3, L2CR_REG
        andis.  r3, r3, L2CR_DISABLE_MASK_U  /* disable L2 cache*/
        mtspr   L2CR_REG, r3

#ifdef INCLUDE_ALTIVEC
        .long   0x7e00066c                   /*  dssall          */
                                             /* STOP all data stream. */
#endif

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

        blr

cache_inval_L2_MPC745X:
        isync

        sync        

         mfspr   r3,L2CR_REG
         andis.  r4,r3,0x8000
         beq     inval_L2_Cache_MPC7450_exit

#ifdef INCLUDE_ALTIVEC
        .long   0x7e00066c                   /*  dssall          */
                                             /* STOP all data stream. */
#endif

        sync

        oris    r3, r3, L2CR_GBL_INV_U      /* set global invalidate command */
        mtspr   L2CR_REG, r3
        sync

	isync					# Somerset test card code does this

inval_L2_Cache_MPC7450_poll:
	mfspr	r5,1017
	sync
	mtcrf	0xff,r5			# Is L2CR[L2I/10] still set?
	bc		0xC,10,inval_L2_Cache_MPC7450_poll
	isync					# Somerset test card code does this
inval_L2_Cache_MPC7450_exit:
        blr

FUNC_END(sysL2BackGlobalInv)

/*********************************************************************
*  sysL2BackEnable - Set and enable L2 backside cache
*
* RETURNS: N/A
*
* void sysL2BackEnable
*     (
*     CACHE_TYPE cache         /@ type of L1 cache being enable @/
*     )
*
*/

FUNC_BEGIN(sysL2BackEnable)
        sync
        cmpwi   r3, _DATA_CACHE
        beq     DCACHE
ICACHE:      
#if  defined(USER_I_CACHE_ENABLE) && defined(USER_D_CACHE_ENABLE)
        b       RETOK              
#elif defined(USER_I_CACHE_ENABLE)
        mfspr   r4, L2CR_REG
        ori     r4, r4, L2IO
        mtspr   L2CR_REG, r4
        b       L2Enable             
#else
        b       RETOK
#endif
DCACHE: 
#if  defined(USER_I_CACHE_ENABLE) && defined(USER_D_CACHE_ENABLE)
        b       L2Enable
#elif defined(USER_D_CACHE_ENABLE)
        mfspr   r4, L2CR_REG
        oris    r4, r4, L2DO
        mtspr   L2CR_REG, r4
        b       L2Enable             
#else
        b       RETOK
#endif
L2Enable:                       
        mfspr   r4, L2CR_REG
        oris    r4, r4, L2CR_EN_U
        mtspr   L2CR_REG, r4 
RETOK: 
        sync         
      	blr				   	/* return to caller */
FUNC_END(sysL2BackEnable)

/*********************************************************************
* sysL2BackDisable - Disable the L2 backside cache                            
*
* The value of the l2cr register is read, the enable bit is 
* disabled and written back to the l2cr register.
*
* The following problem exists on all versions of the MPC7400 (MAX) processor and is
* not expected to be fixed.  A fix is targeted for revision 1.3 of the MPC7410
* (Nitro) processor.
*
* The following is the response from SPS
* regarding the problem:
*
* Overview:
*
* The MPC7400 can hang if the L2 is disabled during an outstanding instruction
* fetch.
*
* Detailed Description:
*
* The problem centers around the interaction of the instruction cache and the
* L2 cache as the L2 cache is disabled.  The scenario is as follows:
*
*  1. An ifetch misses in the icache and allocates a reload table entry
*  2. As the instructions return from the BIU they are forwarded around the
*     icache and dispatched as well as written to the IRLDQ.
*  3. One of these instruction is a mtspr targeting the L2CR.  This
*     instruction  disables the L2.
*  4. When all beats of data return to the IRLDQ, the IRLT arbitrates to
*     reload the L2.  Since the L2 is now disabled, it does not expect reload
*     requests from the IRLT.
*  5. The unexpected reload request is mishandled by the L2 and passed to the
*     BIU as an ifetch miss.
*
* Workaround:
*  1. Preload the code that disables the L2 into the instruction cache before
*     execution.  This requires the code be structured in such a way that the
*     instruction fetch be completed before the mtspr is executed.
* RETURNS: N/A
*
* void sysL2BackDisable
*     (
*     void
*     )
*
*/

FUNC_BEGIN(sysL2BackDisable)
	sync
	mfspr	r3, L2CR_REG		   	/* Read L2 control register */
	andis.	r3, r3, L2CR_DISABLE_MASK_U  	/* Disable L2 backside */
        sync

        b  preFetchL2DisableCode

codeIsPrefetched:
	mtspr   L2CR_REG, r3     
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
FUNC_END(sysL2BackDisable)


/*********************************************************************
* sysL2BackHWFlush - Hardware Flush for L2 (only for 74X0)                    
*
* The value of the l2cr register is read, the Harware flush bit is set 
*  and written back to the l2cr register.
* 
* RETURNS: N/A
*
* void sysL2HWFlush
*     (
*     void
*     )
*
*/

FUNC_BEGIN(sysL2BackHWFlush)

	mfspr	r4,PVR_REG			# read PVR 
	srawi	r3,r4,16		# shift down 16 bits

        cmpi	0,0,r3,0x000C 		# Max pvr
        beq	flush_L2_hw
        cmpi	0,0,r3,-32756 #0x800C 		# Nitro pvr
        beq	flush_L2_hw
        cmpi	0,0,r3,-32768 #0x8000  		# Vger pvr
        beq	flush_L2_hw_vger
        cmpi	0,0,r3,-32767 #0x8001  		# Apollo pvr
        beq	flush_L2_hw_vger
        cmpi	0,0,r3,-32766 #0x8002  		# 7447
        beq	flush_L2_hw_vger
        cmpi	0,0,r3,-32765 #0x8003  		# 7447A
        beq	flush_L2_hw_vger

flush_L2_hw:

        isync

#ifdef INCLUDE_ALTIVEC
        .long 0x7e00066c                     /* dssall           */
                                             /* STOP all data stream. */
#endif
        sync
        mfspr   r3, L2CR_REG
        ori     r3, r3, L2CR_HWFLUSH
        mtspr   L2CR_REG, r3     
        sync
      	blr				   	/* return to caller */

/* Flush the L2 for Vger using the hardware assist.  Vger is just
 * slightly different, of course.
 */

flush_L2_hw_vger:
        isync
#ifdef INCLUDE_ALTIVEC
        .long 0x7e00066c                     /* dssall           */
                                             /* STOP all data stream. */
#endif
	sync
        mfspr   r3,L2CR_REG
        sync

	andis.	r4,r3,0x8000		# If L2 is not on, dont
	beq	fL2end		# flush

	ori	r3,r3,0x0800		# Set the HWF bit.

	sync
        mtspr   L2CR_REG,r3
        sync

fl2v_wait:
	mfspr	r3,L2CR_REG
	sync
	andi.	r3,r3,0x0800		# L2HWF still set?
	bne	fl2v_wait
fL2end:
	sync
	isync

        blr                            
FUNC_END(sysL2BackHWFlush)


/*********************************************************************
* sysL2BackSWFlush - Software Flush for L2                     
*
* The software based routine, which loads block of memory to L1, equal 
* to size  of L2 RAM.   
* 
* RETURNS: N/A
*
* void sysL2SWFlush
*     (
*     void
*     )
*
*
        
FUNC_BEGIN(sysL2BackSWFlush)
        sync
        lis    r3, HI((L2RAM_SIZE*2)/L1CACHE_ALIGN_SIZE)
        ori    r3, r3, LO((L2RAM_SIZE * 2)/L1CACHE_ALIGN_SIZE)
        mtspr  CTR, r3
        lis    r4, HI(BUFFER_ADDR)
        ori    r4, r4, LO(BUFFER_ADDR)
        subi   r4,r4,(L1CACHE_ALIGN_SIZE)
        andi.  r5, r5, 0x0
        
FlushCache:
        lbzu   r5, L1CACHE_ALIGN_SIZE(r4)       
        bdnz   FlushCache
        sync
        blr
FUNC_END(sysL2BackSWFlush)

*/
 
/*********************************************************************
*   sysL2BackInit - Initialize L2 Back Cache.
*
*   RETURNS:    N/A
*   void sysL2BackInit
*       (
*       void
*       )
*/

FUNC_BEGIN(sysL2BackInit)  

	mfspr	r9, PVR_REG		     /*	 read PVR             */
	rlwinm	r9,r9,16,16,31		     /* shift down 16 bits    */
	cmpli	0,0,r9,CPU_TYPE_7450		     /* Vger pvr              */
	beq	cache_init_L2_MPC745X
	cmpli	0,0,r9,CPU_TYPE_7455         /* 0x8001              */
	beq	cache_init_L2_MPC745X
	cmpli	0,0,r9,CPU_TYPE_7447         /* 0x8001              */
	beq	cache_init_L2_MPC745X
	cmpli	0,0,r9,CPU_TYPE_7447A        /* 0x8003              */
	beq	cache_init_L2_MPC745X

#if FALSE
        mfspr   r3, L2CR_REG
        andis.  r4, r3, L2CR_EN_U
        bne     l2backenable_done

        mflr    r7                      

        sync
        addi    r3, r0, 0x0
        oris    r3, r3, L2CR_CLK_2
        mtspr   L2CR_REG, r3
        sync

        bl      sysL2BackGlobalInv                 
        sync
        mfspr   r3, L2CR_REG
        oris    r3, r3, L2CR_CFG
        mtspr   L2CR_REG, r3
        sync

        
        isync

        mtlr    r7
#endif /* FALSE */

l2backenable_done:      
        blr

cache_init_L2_MPC745X:

        mfspr   r3, L2CR_REG
        andis.  r4, r3, L2CR_EN_U
        bne     l2backenable_done

        sync

        lis     r5,0x8000
        ori     r5,r5,0x0000
        lis     r4,0x7011
        ori     r4,r4,0x1000

        and     r3,r3,r4
        mtspr   L2CR_REG,r3

        sync

        mflr    r7                      
        bl      sysL2BackGlobalInv                 
        
        isync

        mtlr    r7

        blr
FUNC_END(sysL2BackInit)  



/**********************************************************************
*  sysL1DcacheEnable - Enable the L1 Dcache
*
* RETURNS: N/A

* void sysL1DcacheEnable
*     (
*     void
*     )

*/

FUNC_BEGIN(sysL1DcacheEnable)

        mfspr   r5,HID0_REG     		/* turn on  the D cache. */
        ori     r5,r5,L1_DCACHE_ENABLE    	/* Data cache only! */
        andi.   r6,r5,L1_DCACHE_INV_MASK    	/* clear the invalidate bit */
        mtspr   HID0_REG,r5
        isync
        sync
        mtspr   HID0_REG,r6
        isync
        sync
        blr 
FUNC_END(sysL1DcacheEnable)

/**********************************************************************
*  sysL1DcacheDisable - Disable the L1 Dcache
*
* RETURNS: N/A

* void sysL1DcacheDisable
*     (
*     void
*     )

*/

FUNC_BEGIN(sysL1DcacheDisable)

        mfspr   r5,HID0_REG
        andi.   r5,r5,L1_DCACHE_DISABLE
        mtspr   HID0_REG,r5
        isync
        sync
        blr          
FUNC_END(sysL1DcacheDisable)

