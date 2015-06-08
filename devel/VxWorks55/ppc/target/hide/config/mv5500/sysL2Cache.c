/* sysL2Cache.c - L2 cache library */

/* Copyright 1984-1999 Wind River Systems, Inc. */
/* Copyright 1996-2002 Motorola, Inc., All Rights Reserved. */

/*
modification history
--------------------
01c,14jul03,cak Modified invalidate routine to only reenable the L2 cache
		if it was enabled at the beginning of the routine.
01b,21oct03,scb Check L2cr against desired program value before enabling.
01a,01oct02,scb Copied from hxeb100 base (ver 01a).
*/

/*
DESCRIPTION

This library provides L2 cache support.

Support is included for initializing/enabling, invalidating, flushing,
and disabling the L2 Cache. Routines are based on the algorithms described
on pages 3-52 to 3-55 of the MPC7450 RISC Microprocessor Family User's 
Manual. 
*/

/* includes */

#include "vxWorks.h"
#include "sysCache.h"

/* defines */

/* forward declarations */

void sysL2CacheInit (void);
void sysL2CacheEnable (void);
void sysL2CacheDisable (void);
void sysL2CacheFlush (void);
void sysL2CacheInv (void);
void sysL2CacheLock (void);
void sysL2CacheUnlock (void);

/* externals */

IMPORT UINT sysL2crGet (void);
IMPORT void sysL2crPut (UINT regVal);
IMPORT UINT sysMsscr0Get (void);
IMPORT void sysMsscr0Put (UINT regVal);
#ifdef INCLUDE_CACHE_L3
IMPORT void sysL3CacheDisable (void);
#endif

IMPORT VOIDFUNCPTR _pSysL2CacheInvFunc;
IMPORT VOIDFUNCPTR _pSysL2CacheEnable;
IMPORT VOIDFUNCPTR _pSysL2CacheFlush;
IMPORT VOIDFUNCPTR _pSysL2CacheDisable;

/******************************************************************************
*
* sysL2CacheInit - Initialize L2 cache function pointers and enable L2 cache.
*
* This routine initializes the L2 cache function pointers as defined in
* cacheArchLib.c and enables the L2 cache. 
*
* RETURNS: N/A 
*/ 

void sysL2CacheInit(void)
    {

    /* hook up the L2 functions */

    _pSysL2CacheEnable  = sysL2CacheEnable;
    _pSysL2CacheFlush   = sysL2CacheFlush;
    _pSysL2CacheDisable = sysL2CacheDisable;
    _pSysL2CacheInvFunc = sysL2CacheInv;

    /* Enable the L2 Cache */

    sysL2CacheEnable();
    }

/******************************************************************************
*
* sysL2CacheEnable - Initialize and enable the L2 cache.
*
* This routine initializes and enables the L2 cache. 
*
* RETURNS: N/A 
*/ 

void sysL2CacheEnable(void)
    {
    UINT l2crVal = 0x00000000; /* Current L2CR settings are not preserved */
    UINT msscr0Val; /* MSSCR0 settings, other than PFE, are preserved */
    LOCAL UINT l2crSave = 0x00000000;  /* Saved value for L2cr programming */

    /* If L2 cache is disabled, then go ahead and enable it, else do nothing */

    if ((sysL2crGet() != l2crSave) || (l2crSave == 0))
	{

	/*
	 * L2 cache is either not enabled or the programming of the L2
	 * cache control register has been corrupted.  We need to program
	 * the L2 cache control register with the proper mode bits set and
	 * ultimately set the enable bit in this register.  Because we don't
	 * know for sure whether the cache has simply been disabled or if 
	 * some of the L2 cache control mode bit settings have been
	 * compromised, we need to start from a known state.  We will
	 * accomplish this by disabling the L2 cache.  To make sure we
	 * avoid corruption between the L2 cache and the L3 cache we must
	 * disable the L3 cache first.
	 */

#ifdef INCLUDE_CACHE_L3
	sysL3CacheDisable ();
#endif
	sysL2CacheDisable ();

	/* globally invalidate the L2 cache by setting the L2I bit in L2CR */

	sysL2crPut(sysL2crGet() | L2CR_L2I_MASK);	

	/* poll L2CR until the L2I bit is cleared, invalidation complete */

	while (sysL2crGet() & L2CR_L2I_MASK)		
	    ;                                      

	/*
	 * Configure the L2CR and MSSCR0 Register 
	 *
	 * Enable/Disable L2 Data Parity Checking
	 * Configure the mode (Instruction-only, Data-only, or both)
	 * Configure the replacement algorithm (default, or secondary)
	 * Configure the number of prefetch engines (none, 1, 2, or 3)
	 *
	 * The definitions used to configure the L2CR and MSSCR0 registers 
	 * are located in the board header file, in order to allow the user 
	 * to modify the default configuration.
	 *
	 * The defaults are to enable parity checking, 1 prefetch engine,
	 * allow both instructions and data, and to use the default replacement
	 * algorithm.
	 */

#ifdef ENABLE_L2_DATA_PARITY

	/* Enable L2 Data Parity Checking */

	l2crVal |= L2CR_PE_MASK;

#endif /* ENABLE_L2_DATA_PARITY */

	/* Configure the Mode */

	l2crVal |= L2_MODE;

	/* Configure the Replacement Algorithm */

	l2crVal |= L2_REP_ALG;

	sysL2crPut(l2crVal);		/* Write L2 Cache Control Reg */

	/* The number of prefetch engines is configured in MSSCR0 */

	msscr0Val = (sysMsscr0Get() & ~MSSCR0_L2PFE_MASK); /* clear PFE bits */

	/* Configure the number of prefetch engines */

	msscr0Val |= MSSCR0_L2_PREFETCH_ENGINES;

	sysMsscr0Put(msscr0Val);			/* Write MSSCR0 Reg */

	/* enable the L2 cache by setting the L2E bit in the L2CR register */

	sysL2crPut(sysL2crGet() | L2CR_L2E_MASK);

	/*
	 * Save the properly programmed value to test agains the next time
	 * we are called.
	 */

	l2crSave = sysL2crGet();
	}
    }

/******************************************************************************
*
* sysL2CacheFlush - flush the L2 cache
* 
* This routine flushes the L2 cache.
*
* RETURNS: N/A
*/ 

void sysL2CacheFlush (void)
    {
    UINT l2crVal;
    UINT msscr0Val;

    /* If L2 cache is enabled, go ahead and flush it, otherwise do nothing */

    if ((sysL2crGet() & L2CR_L2E_MASK) == L2CR_L2E_MASK)
	{

	/* save L2CR and MSSCR0 contents */

	l2crVal = sysL2crGet();
	msscr0Val = sysMsscr0Get();

	/* disable prefetching by clearing PFE bits in MSSCR0 */

	sysMsscr0Put(msscr0Val & ~MSSCR0_L2PFE_MASK);	

	/* Lock the L2 Cache */

	sysL2CacheLock();

	/* perform hardware flush be setting HWF bit in L2CR */

	sysL2crPut(sysL2crGet() | L2CR_HWF_MASK);	

	/* poll L2CR until HWF bit is cleared, flush complete */

	while (sysL2crGet() & L2CR_HWF_MASK)	
	    ;                              
  
	/* restore L2CR and MSSCR0 to original values */

	sysL2crPut(l2crVal);	
	sysMsscr0Put(msscr0Val);
	}
    }

/******************************************************************************
*
* sysL2CacheInv - Invalidate the L2 cache
* 
* This routine invalidates the L2 cache.
*
* RETURNS: N/A
*/ 

void sysL2CacheInv (void)
    {
    BOOL l2enable;

    /* capture state of L2E bit in L2CR register */

    if ((sysL2crGet() & L2CR_L2E_MASK) == L2CR_L2E_MASK)
	l2enable = TRUE;
    else
	l2enable = FALSE;

    /* dssall to cancel any pending data stream touch instructions */

    DSSALL;

    /* sync to finish any pending store operations */

    SYNC;

    /* clear L2E bit in L2CR register - disable L2 cache */

    sysL2crPut(sysL2crGet() & ~L2CR_L2E_MASK);	

    /* set L2I bit in L2CR - global invalidate */

    sysL2crPut(sysL2crGet() | L2CR_L2I_MASK);

    /* poll L2CR until L2I bit is cleared, invalidate complete */

    while (sysL2crGet() & L2CR_L2I_MASK)		
        ;                                      

    /* restore state of L2E bit in L2CR register */

    if (l2enable)
        sysL2crPut(sysL2crGet() | L2CR_L2E_MASK);
    }

/******************************************************************************
*
* sysL2CacheDisable - disable the L2 cache
* 
* This routine disables the L2 cache.
*
* RETURNS: N/A
*/ 

void sysL2CacheDisable (void)
    {

    /* If L2 cache is enabled, go ahead and disable it, otherwise do nothing */

    if ((sysL2crGet() & L2CR_L2E_MASK) == L2CR_L2E_MASK)
	{

	/* Flush the L2 cache */

	sysL2CacheFlush();

	/* Disable L2 cache by clearing L2E bit in L2CR register */

	sysL2crPut(sysL2crGet() & ~L2CR_L2E_MASK);	

	/* Ensure that prefetching is disabled in MSSCR0 */

	sysMsscr0Put(sysMsscr0Get() & ~MSSCR0_L2PFE_MASK);
	}
    }

/******************************************************************************
*
* sysL2CacheLock - Lock the L2 cache
* 
* This routine locks the L2 cache.
*
* RETURNS: N/A
*/ 

void sysL2CacheLock (void)
    {

    /*
     * Lock the L2 cache by setting the L2IO (L2 Instruction-only mode)
     * and L2DO (L2 Data-only mode) bits.
     */

    sysL2crPut(sysL2crGet() | (L2CR_L2IO_MASK | L2CR_L2DO_MASK));
    }

/******************************************************************************
*
* sysL2CacheUnlock - Unlock the L2 cache
* 
* This routine unlocks the L2 cache.
*
* RETURNS: N/A
*/ 

void sysL2CacheUnlock (void)
    {

    /* Unlock the L2 cache by clearing L2IO and L2DO in the L2CR register */

    sysL2crPut(sysL2crGet() & ~(L2CR_L2IO_MASK | L2CR_L2DO_MASK));
    }
