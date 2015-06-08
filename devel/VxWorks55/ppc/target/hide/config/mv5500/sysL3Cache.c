/* sysL3Cache.c - L3 cache library */

/* Copyright 1984-1999 Wind River Systems, Inc. */
/* Copyright 1996-2002 Motorola, Inc., All Rights Reserved. */

/*
modification history
--------------------
01c,21may03,scb Check L3cr against desired program value before enabling.
01b,01apr03,scb Correctly set L3CR_L3CLKEN_MASK in L3 cache control reg.
01a,01oct02,scb Copied from hxeb100 base (ver 01a).
*/

/*
DESCRIPTION

This library provides L3 cache support. 

Support is included for initializing, invalidating, enabling, 
flushing, disabling, locking and unlocking the L3 Cache.  Parameters 
used to configure the L3 controller are gathered from the L3 Cache 
Record in the VPD.
*/

/* includes */

#include "vxWorks.h"
#include "sysCache.h"
#include "sysMotVpd.h"

/* defines */

/* forward declarations */

STATUS sysL3CacheInit (void);
void sysL3CacheFlushDisable (void);
void sysL3CacheInvEnable (void);
void sysL3CacheFlush (void);
void sysL3CacheInv (void);
void sysL3CacheLock (void);
void sysL3CacheUnlock (void);
void sysL3CacheEnable (void);
void sysL3CacheDisable (void);

/* externals */

IMPORT UINT sysL3crGet (void);
IMPORT void sysL3crPut (UINT regVal);

IMPORT VOIDFUNCPTR _pSysL3CacheInvalEnableFunc;
IMPORT VOIDFUNCPTR _pSysL3CacheFlushDisableFunc;

LOCAL VPD_PACKET * pVpdL3CachePacket = NULL;

/******************************************************************************
*
* sysL3CacheInit - initialize the L3 cache 
*
* This routine initializes and enables L3 cache support. 
*
* RETURNS: N/A 
*/ 

STATUS sysL3CacheInit (void)
    {

    /* Extract L3 Cache Configuration from VPD */

    if (pVpdL3CachePacket == NULL)
        {

        /* Extract a VPD L3 Cache Packet */

        if (sysMotVpdPktGet(VPD_PID_L3, 0, &sysVpdPkts[0], &pVpdL3CachePacket)
            == ERROR)
            return (ERROR);
        }

    /* hook up the L3 functions */

    _pSysL3CacheInvalEnableFunc = sysL3CacheInvEnable;
    _pSysL3CacheFlushDisableFunc = sysL3CacheFlushDisable;

    /* Invalidate & Enable the L3 Cache */

    sysL3CacheInvEnable();

    return OK;
    }

/******************************************************************************
*
* sysL3CacheInvEnable - invalidate and enable L3 cache 
*
* This routine invalidates and enables the L3 cache 
*
* RETURNS: N/A 
*/ 

void sysL3CacheInvEnable(void)
    {
    UINT l3crVal = 0x00000000;  /* current L3CR values are not preserved */
    LOCAL UINT l3crSave = 0x00000000;  /* Saved value for L3cr programming */

    /* If L3 cache is disabled, then go ahead and enable it, else do nothing */

    if ((sysL3crGet() != l3crSave) || (l3crSave == 0))
        {

	/*
	 * L3 cache is either not enabled or the programming of the L3
	 * cache control register has been corrupted.  We need to program
	 * the L3 cache control register with the proper mode bits set and
	 * ultimately set the enable bit in this register.  Because we don't
	 * know for sure whether the cache has simply been disabled or if 
	 * some of the L3 cache control mode bit settings have been
	 * compromised, we need to start from a known state.  We will
	 * accomplish this by disabling the L3 cache.  
	 */

	sysL3CacheDisable ();

	/* 
	 * Configure the L3CR Register:
	 *
	 * L3CLK   -> L3 Clock Ratio
	 * L3RT    -> L3 SRAM Type
	 * L3SIZ   -> L3 Size
	 * L3IO    -> L3 Instruction-only mode
	 * L3SPO   -> L3 Sample Point Override
	 * L3CKSP  -> L3 Clock Sample Point
	 * L3PSP   -> L3 P-Clock Sample Point
	 * L3REP   -> L3 Replacement Algorithm
	 * L3NIRCA -> L3 non-integer ratios clock adjustment
	 * L3DO    -> L3 Data-only Mode
	 *
	 * The replacement algorithm and the mode (instruction, data, both)
	 * are user-configurable, via a set of definitions in the board
	 * header file.  All other fields are programmed according to VPD
	 * data.
	 */

	/* L3 Clock Ratio */

	l3crVal |= 
	 (((UINT)pVpdL3CachePacket->data[VPD_L3C_CLK_RATIO]) << L3CR_L3CLK_BIT);

	/* Replacement Algorithm */

	l3crVal |= L3_REP_ALG;

	/* L3 Cache Size */

	l3crVal |=
	 (((UINT)pVpdL3CachePacket->data[VPD_L3C_SIZE]) << L3CR_L3SIZ_BIT);

	/* L3 Cache Mode */

	l3crVal |= L3_MODE;

	/* L3 Sample Point Override */

	l3crVal |=
	 (((UINT)pVpdL3CachePacket->data[VPD_L3C_SMPL_PNT_OVERRIDE]) << 
	  L3CR_L3SPO_BIT);

	/* L3 Clock Sample Point */

	l3crVal |=
	 (((UINT)pVpdL3CachePacket->data[VPD_L3C_CACHE_CLK_SMPL_PNT]) <<
	  L3CR_L3CKSP_BIT);

	/* L3 P-Clock Sample Point */

	l3crVal |=
	 (((UINT)pVpdL3CachePacket->data[VPD_L3C_PROC_CLK_SMPL_PNT]) <<
	  L3CR_L3PSP_BIT);

	/* L3 SRAM Type */

	l3crVal |=
	 (((UINT)pVpdL3CachePacket->data[VPD_L3C_SRAM_TYPE]) <<
	   L3CR_L3RT_BIT);

	/* L3 Non-Integer Ratios Clock Adjustment for SRAM */

	l3crVal |=
	 (((UINT)pVpdL3CachePacket->data[VPD_L3C_SRAM_CLK_CTRL]) <<
	   L3CR_L3NIRCA_BIT);

	/* Write to L3CR Register */

	sysL3crPut(l3crVal);

	/* set L3CR[5] reserved bit to 1 */

	sysL3crPut(sysL3crGet() | L3CR_RES_INIT_MASK);

	/* set L3CR[L3CLKEN] = 1 */

	sysL3crPut(sysL3crGet() | L3CR_L3CLKEN_MASK);

	/* Globally invalidate the L3 Cache */

	sysL3CacheInv();

	/* Clear L3CR[L3CLKEN] */

	sysL3crPut(sysL3crGet() & ~L3CR_L3CLKEN_MASK);

	/* Configure Data and Address Parity */

	l3crVal = sysL3crGet();

	l3crVal |=
	 (((UINT)pVpdL3CachePacket->data[VPD_L3C_DATA_ERROR_DETECT]) <<
	   L3CR_L3PE_BIT);

	l3crVal |=
	 (((UINT)pVpdL3CachePacket->data[VPD_L3C_ADDR_ERROR_DETECT]) <<
	   L3CR_L3APE_BIT);

	sysL3crPut(l3crVal);

	/* Wait 100 processor cycles - 1 microsecond */

	sysUsDelay(1);

	/* Set L3 Cache Enable & L3 Clock Enable */

	sysL3crPut(sysL3crGet() | (L3CR_L3E_MASK | L3CR_L3CLKEN_MASK));

	/* Wait 100 processor cycles - 1 microsecond */

	sysUsDelay(1);

	l3crSave = sysL3crGet ();
	}
    }

/******************************************************************************
*
* sysL3CacheFlushDisable - flush and disable the L3 cache
*
* This routine flushes and disables the L3 cache.
*
* RETURNS: N/A
*/

void sysL3CacheFlushDisable (void)
    {
    UINT l3crVal;

    /* If L3 cache is enabled, go ahead and flush it, otherwise do nothing */

    if ((sysL3crGet() & L3CR_L3E_MASK) == L3CR_L3E_MASK)
        {

        /* save L3CR contents */

        l3crVal = sysL3crGet();

	/* Flush the L3 Cache */

	sysL3CacheFlush();

        /* restore L3CR to original value */

        sysL3crPut(l3crVal);  

	/* Disable the L3 Cache */

	sysL3CacheDisable(); 
        }
    }

/******************************************************************************
*
* sysL3CacheFlush - flush the L3 cache
*
* This routine flushes the L3 cache.
*
* RETURNS: N/A
*/

void sysL3CacheFlush (void)
    {

    /* Lock the L3 Cache */

    sysL3CacheLock();

    /* perform hardware flush be setting HWF bit in L3CR */

    sysL3crPut(sysL3crGet() | L3CR_L3HWF_MASK);

    /* poll L3CR until HWF bit is cleared, flush complete */

    while (sysL3crGet() & L3CR_L3HWF_MASK)   
        ;
    }

/******************************************************************************
*
* sysL3CacheLock - Lock the L3 cache
*
* This routine locks the L3 cache.
*
* RETURNS: N/A
*/

void sysL3CacheLock (void)
    {

    /*
     * Lock the L3cache by setting the L3IO (L3 Instruction-only mode)
     * and L3DO (L3 Data-only mode) bits.
     */

    sysL3crPut(sysL3crGet() | (L3CR_L3IO_MASK | L3CR_L3DO_MASK));
    }

/******************************************************************************
*
* sysL3CacheUnlock - Unlock the L3 cache
*
* This routine unlocks the L3 cache.
*
* RETURNS: N/A
*/

void sysL3CacheUnlock (void)
    {

    /* Unlock the L3 Cache by clearing the L3IO and L3DO bits */

    sysL3crPut(sysL3crGet() & ~(L3CR_L3IO_MASK | L3CR_L3DO_MASK));
    }

/******************************************************************************
*
* sysL3CacheInv - Globally invalidate the L3 cache
*
* This routine globally invalidates the L3 cache.
*
* RETURNS: N/A
*/

void sysL3CacheInv (void)
    {

    /* 
     * Perform Global Invalidation:
     *
     * DSSALL
     * SYNC
     * set L3CR[L3I] = 1
     * poll L3CR[L3I] until clear
     */

    DSSALL;

    SYNC;

    sysL3crPut(sysL3crGet() | L3CR_L3I_MASK);

    while (sysL3crGet() & L3CR_L3I_MASK)
        ;
    }

/******************************************************************************
*
* sysL3CacheEnable - Enable the L3 cache
*
* This routine enables the L3 cache.
*
* RETURNS: N/A
*/

void sysL3CacheEnable (void)
    {

    /* Enable the L3 cache by setting the L3E bit in the L3CR register */

    sysL3crPut(sysL3crGet() | L3CR_L3E_MASK);
    }

/******************************************************************************
*
* sysL3CacheDisable - Disable the L3 cache
*
* This routine disables the L3 cache.
*
* RETURNS: N/A
*/

void sysL3CacheDisable (void)
    {

    /* Disable the L3 Cache by clearing the L3E bit in the L3CR register */

    sysL3crPut(sysL3crGet() & ~L3CR_L3E_MASK);
    }
