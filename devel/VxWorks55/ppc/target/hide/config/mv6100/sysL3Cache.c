/* sysL3Cache.c - L3 cache library */

/* Copyright 1984-1999 Wind River Systems, Inc. */
/* Copyright 1996-2004 Motorola, Inc., All Rights Reserved. */

/*
modification history
--------------------
01f,07dec04,cak  SPR 102861 - remove sysL3CacheInvEnable from L3CacheInit.
01e,08jun04,cak  Added a needed SYNC instruction to sysL3CacheFlush() as
		 well as a call to sysL3CacheUnlock().  And, modified
		 sysL3CacheLock() to ensure the cache is locked before
		 exiting the routine.
01d,29mar04,cak  Added support for the L3 Cache Output Hold Control
		 Register (L3OHCR - SPR 1000). 
01c,26feb04,cak  Fixed diab compiler warnings.
01b,17feb04,cak  Modified to support errata #15 of MPC7457CE 
		 Rev.5, 12/2003. 
01a,17dec03,cak  Ported from ver 01e, mcpm905/sysL3Cache.c.
*/

/*
DESCRIPTION

This library provides L3 cache support. 

Support is included for initializing, invalidating, enabling, 
flushing, disabling, locking and unlocking the L3 Cache.  Parameters 
used to configure the L3 controller are gathered from the L3 Cache 
Record in the VPD.

Support is also included for configuring the L3 cache as private
memory.
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
#ifdef INCLUDE_CACHE_L3_PM
    STATUS sysL3CachePmEnable (UINT32);
    LOCAL void sysL3CachePmBaseAddrSet (UINT32);
    LOCAL STATUS sysL3CachePmScrub (UINT32, UINT32);
    LOCAL void sysL3CacheZeroFlush (UINT32);
    void sysL3CachePmDisable (void);
#endif /* INCLUDE_CACHE_L3_PM */

/* externals */

IMPORT UINT sysL3crGet (void);
IMPORT void sysL3crPut (UINT regVal);
IMPORT UINT sysL3pmGet (void);
IMPORT void sysL3pmPut (UINT regVal);
IMPORT void sysL3ohcrPut (UINT regVal);

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
    UINT l3crVal = 0x00000000; /* current L3CR values are not preserved */
    LOCAL UINT l3crSave = 0x00000000;  /* Saved value for L3cr programming */
    UINT l3crPmen = 0x0;
    UINT l3crPmSize = 0x0;
    UINT l3ClkRatio = 0x0;
    UINT l3SamplePoint = 0x0;
    UINT cpuType = 0x0;
    UINT cpuRev = 0x0;

    /* If L3 cache is disabled, then go ahead and enable it, else do nothing */

    if ((sysL3crGet() != l3crSave) || (l3crSave == 0))
        {

	/* Need to capture state of private memory enable and size bits */

	l3crPmen = (sysL3crGet() & L3CR_PMEN_MASK);
	l3crPmSize = (sysL3crGet() & L3CR_PMSIZ_MASK);

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
	 * Configure the L3 Cache Output Hold Control Register (L3OHCR) 
	 * The L3 Cache Output Hold Control Register is a register
	 * that is specific to the MPC7457 processor.
	 */

	sysL3ohcrPut((L3OHCR_L3AOH << L3OHCR_L3AOH_BIT) |
		     (L3OHCR_L3CLK0_OH << L3OHCR_L3CLK0_OH_BIT) |
		     (L3OHCR_L3CLK1_OH << L3OHCR_L3CLK1_OH_BIT) |
		     (L3OHCR_L3DOH0 << L3OHCR_L3DOH0_BIT) |
		     (L3OHCR_L3DOH8 << L3OHCR_L3DOH8_BIT) |
		     (L3OHCR_L3DOH16 << L3OHCR_L3DOH16_BIT) |
		     (L3OHCR_L3DOH24 << L3OHCR_L3DOH24_BIT) |
		     (L3OHCR_L3DOH32 << L3OHCR_L3DOH32_BIT) |
		     (L3OHCR_L3DOH40 << L3OHCR_L3DOH40_BIT) |
		     (L3OHCR_L3DOH48 << L3OHCR_L3DOH48_BIT) |
		     (L3OHCR_L3DOH56 << L3OHCR_L3DOH56_BIT));
	
	/* 
	 * Configure the L3CR Register:
	 *
	 * L3CLK     -> L3 Clock Ratio
	 * L3RT      -> L3 SRAM Type
	 * L3SIZ     -> L3 Size
	 * L3IO      -> L3 Instruction-only mode
	 * L3CLKEXT  -> L3 Clock Ratio Extension
	 * L3CKSPEXT -> L3 Clock Sample Point Extension
	 * L3SPO     -> L3 Sample Point Override
	 * L3CKSP    -> L3 Clock Sample Point
	 * L3PSP     -> L3 P-Clock Sample Point
	 * L3REP     -> L3 Replacement Algorithm
	 * L3NIRCA   -> L3 non-integer ratios clock adjustment
	 * L3DO      -> L3 Data-only Mode
	 *
	 * The replacement algorithm and the mode (instruction, data, both)
	 * are user-configurable, via a set of definitions in the board
	 * header file.  All other fields are programmed according to VPD
	 * data.
	 */

	/* L3 Clock Ratio and Extension */

	l3ClkRatio = 
	 (((UINT)pVpdL3CachePacket->data[VPD_L3C_CLK_RATIO]) << L3CR_L3CLK_BIT);

	l3crVal |= l3ClkRatio & L3CR_L3CLK_MASK;

	if (l3ClkRatio & 0x04000000)
	    {
	    l3crVal |= (1 << L3CR_L3CLKEXT_BIT);
	    } 

	/* Replacement Algorithm */

	l3crVal |= L3_REP_ALG;

	/* L3 Cache Size */

	if (l3crPmen == 0x0)
	    l3crVal |=
	     (((UINT)pVpdL3CachePacket->data[VPD_L3C_SIZE]) << L3CR_L3SIZ_BIT);
	else
	    {
	    if (l3crPmSize == 0x0)
		{
	        if (((UINT)pVpdL3CachePacket->data[VPD_L3C_SIZE]) == 0x0)
		    l3crVal |= (L3CR_PMSIZ_1MB << L3CR_PMSIZ_BIT);
	        else
		    {
		    l3crVal |= (L3CR_PMSIZ_1MB << L3CR_PMSIZ_BIT);
		    l3crVal |= (L3CR_L3SIZ_1MB << L3CR_L3SIZ_BIT);
		    }
		}
	    else
		{
	        if (((UINT)pVpdL3CachePacket->data[VPD_L3C_SIZE]) == 0x0)
		    l3crVal |= (L3CR_PMSIZ_1MB << L3CR_PMSIZ_BIT);
	        else
		    {
		    l3crVal |= (L3CR_PMSIZ_2MB << L3CR_PMSIZ_BIT);
		    }
		}
	    }

	/* 
	 * L3 Cache Mode 
	 *
	 * Due to errata #15 "Six outstanding miss requests may
	 * stall the processor" in MPC7457CE Rev.5, 12/2003, the 
	 * L3 cache mode configuration is dependent on the size
	 * of the L3 cache that is to be utilized as cache - rather
	 * than private memory.  This errata applies to the 7455/7457
	 * processor: revisions 0100 and 0101.
	 *
	 * If, at this point in the initialization of the L3 cache
	 * it is determined that the full 2MB may potentially be 
	 * configured as cache, then one of two things needs to be done:
	 * (1) the cache, if left at a full 2MB, must be configured
	 *     for instruction-only mode
	 * (2) the size of the cache can be reduced to 1MB, and
	 *     the mode can be determined by user setting (L3_MODE)
	 * Which of these two options is chosen is also 
	 * user-configurable, via the ERRATA_WORKAROUND_2MB_IONLY 
	 * definition in config.h.  If defined then option (1) will 
	 * be chosen, otherwise option (2) will be chosen. 
	 */

	cpuType = CPU_TYPE;
	cpuRev = CPU_REV;

	if ((cpuType == CPU_TYPE_74X7) && 
	    ((cpuRev == CPU_7457_REV_1_0) || (cpuRev == CPU_7457_REV_1_1)))
	    {

	    /* for the MPC7455,57 revision 0100 and 0101 processors */

	    if ((l3crVal & (1 << L3CR_L3SIZ_BIT)) == (1 << L3CR_L3SIZ_BIT))
	        {

		/* If 2MB specified via setting of L3CR_L3SIZ_BIT in L3CR */

#if (defined ERRATA_WORKAROUND_2MB_IONLY)
	        l3crVal |= L3_MODE_I_ONLY;         /* Instruction-Only */
#else
	        l3crVal &= ~(1 << L3CR_L3SIZ_BIT); /* Reduce size to 1MB */
	        l3crVal |= L3_MODE;		   /* mode is user specified */
#endif /* ERRATA_WORKAROUND_2MB_IONLY */
	        }
	    else
	        l3crVal |= L3_MODE;
	    }
	else
	    l3crVal |= L3_MODE;  /* errata does not apply to this processor */

	/* L3 Sample Point Override */

	l3crVal |=
	 (((UINT)pVpdL3CachePacket->data[VPD_L3C_SMPL_PNT_OVERRIDE]) << 
	  L3CR_L3SPO_BIT);

	/* L3 Clock Sample Point and Sample Point Extension */

	l3SamplePoint = 
	 (((UINT)pVpdL3CachePacket->data[VPD_L3C_CACHE_CLK_SMPL_PNT]) <<
          L3CR_L3CKSP_BIT);

	l3crVal |= l3SamplePoint & L3CR_L3CKSP_MASK;

	if (l3SamplePoint & 0x00040000)
	    {
	    l3crVal |= (1 << L3CR_L3CLKSPEXT_BIT);
	    }

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

	if (l3crPmen == 0x0)
	    sysL3crPut(sysL3crGet() | (L3CR_L3E_MASK | L3CR_L3CLKEN_MASK));
	else
	    {
	    if (l3crPmSize == 0x0)
		{
	        if (((UINT)pVpdL3CachePacket->data[VPD_L3C_SIZE]) == 0x0)
		    sysL3crPut(sysL3crGet() | L3CR_L3CLKEN_MASK | l3crPmen); 
	        else
		    {
		    sysL3crPut(sysL3crGet() | l3crPmen |
			       (L3CR_L3E_MASK | L3CR_L3CLKEN_MASK)); 
		    }
		}
	    else
		{
		sysL3crPut(sysL3crGet() | L3CR_L3CLKEN_MASK | l3crPmen);
		}
	    }

	/* Wait 100 processor cycles - 1 microsecond */

	sysUsDelay(1);

        l3crSave = sysL3crGet ();
	}
    }

#ifdef INCLUDE_CACHE_L3_PM
/******************************************************************************
*
* sysL3CacheZeroFlush - Perform a zero-flush of the L3 cache private memory
*
* This routine performs a zero-flush of the L3 cache private memory.
*
* RETURNS: N/A
*/

LOCAL void sysL3CacheZeroFlush
    (
    UINT32 addr         /* cache line address to zero-flush */
    )
    {
    WRS_ASM ("dcbz 0,3");
    WRS_ASM ("sync;isync");
    WRS_ASM ("dcbf 0,3");
    WRS_ASM ("sync;isync");
    }

/******************************************************************************
*
* sysL3CachePmScrub - Scrub the L3 Private Memory.
*
* This routine scrubs the L3 private memory to remove any latent parity
* errors.
*
* RETURNS: N/A
*/

LOCAL STATUS sysL3CachePmScrub 
    (
    UINT32 addr,	/* start address of private memory */
    UINT32 size		/* size of private memory */
    )
    {
    UINT32 pmAddr = addr;
    UINT32 i;

    /* Zero-flush the cache */

    for (i = 0; i < size ; i += _CACHE_ALIGN_SIZE)
        {
        pmAddr = addr + i;
        sysL3CacheZeroFlush (pmAddr);     /* call dcbz dcbf routine */
        }
    return (OK);
    }

/******************************************************************************
*
* sysL3CachePmBaseAddrSet - Program the L3 Private Memory Address Register
*
* This routine programs the L3 Private Memory Address Register (L3PM).
*
* RETURNS: N/A
*/

LOCAL void sysL3CachePmBaseAddrSet 
    (
    UINT32 addr		/* start address of private memory */
    )
    {

    /* Program the L3 Private Memory Address Register */

    sysL3pmPut(addr >> 4);

    }

/*****************************************************************************
*
* sysL3CachePmEnable - Configure the L3 cache as private memory
*
* This routine configures the L3 cache, all or part, as private memory.
* Keep in mind when calling this routine that the size can only be 1MB
* or 2MB.
*
* RETURNS: OK, or ERROR if unable to configure the L3 srams as private memory
*/

STATUS sysL3CachePmEnable
    (
    UINT32 size		/* size of private memory */
    )
    {
    UINT32 pmSize = 0x0;
    UINT32 cacheSize = 0x0;
    UINT32 l3crVal = 0x0;

    /* Ensure a valid size for the private memory, as well as L3 cache */

#if (! defined INCLUDE_CACHE_L3)

    /* Extract L3 Cache Configuration from VPD */

    if (pVpdL3CachePacket == NULL)
        {

        /* Extract a VPD L3 Cache Packet */

        if (sysMotVpdPktGet(VPD_PID_L3, 0, &sysVpdPkts[0], &pVpdL3CachePacket)
            == ERROR)
            return (ERROR);
        }

    /* Entire L3 must be configured as private memory */

    if (((UINT)pVpdL3CachePacket->data[VPD_L3C_SIZE]) == 0x0)
	{
	pmSize = SIZE_1MB;
	cacheSize = 0x0;
	}
    else
	{
	pmSize = SIZE_2MB;
	cacheSize = 0x0;
	}
#else
    if (size == SIZE_1MB)
	{
	if (((UINT)pVpdL3CachePacket->data[VPD_L3C_SIZE]) == 0x0)
	    {
	    pmSize = size;
	    cacheSize = 0x0;
	    }
	else
	    {
	    pmSize = size;
	    cacheSize = SIZE_1MB;
	    }
	}
    else   /* user requests 2MB, or any value other than 1MB */ 
	{
	if (((UINT)pVpdL3CachePacket->data[VPD_L3C_SIZE]) == 0x0)
	    {
	    pmSize = SIZE_1MB;
	    cacheSize = 0x0;
	    }
	else
	    {
	    pmSize = size;
	    cacheSize = 0x0;
	    }
	}
#endif /* INCLUDE_CACHE_L3 */

    if (pmSize != size)
	{

	/* 
	 * The user has requested a private memory size incompatible 
	 * with the current cache configuration.
	 */

	return (ERROR);
	}

#if (! defined INCLUDE_CACHE_L3)

    /* 
     * The L3 Cache has not yet been properly configured, 
     * do this now.  The VPD will be read and the L3CR register
     * will be properly programmed.  This will also temporarily
     * enable the L3 SRAMs as cache, before we reconfigure for
     * private memory.
     */

    sysL3CacheInvEnable(); 
#endif /* INCLUDE_CACHE_L3 */

    /* Disable and Flush the L3 Cache */

    sysL3CacheFlushDisable();

    /* Retrieve Current L3CR Settings */

    l3crVal = sysL3crGet ();

    /* Program the L3 Private Memory Base Address Register */

    sysL3CachePmBaseAddrSet(INCLUDE_CACHE_L3_PM_BASE);

    /* 
     * Preserve the following L3CR parameters:
     *
     * L3 Clock Ratio
     * Replacement Algorithm
     * L3 Sample Point Override
     * L3 Clock Sample Point
     * L3 P-Clock Sample Point
     * L3 SRAM Type
     * L3 Non-Integer Ratios Clock Adjustment for SRAM
     */

    l3crVal &= (L3CR_L3CLK_MASK | L3CR_L3REP_MASK | L3CR_L3SPO_MASK | 
		L3CR_L3CKSP_MASK | L3CR_L3PSP_MASK | L3CR_L3RT_MASK | 
		L3CR_L3NIRCA_MASK | L3CR_L3CLKEXT_MASK | 
		L3CR_L3CKSPEXT_MASK); 

    /*
     * L3 Cache Mode
     *
     * The cache mode will be set according to user-preference, as
     * specified with L3_MODE in config.h.  Since at least part, if 
     * not all, of the L3 cache will be configured as private memory,
     * the mode can be data-only, instruction-only, or both data and
     * instruction.
     */

    l3crVal |= L3_MODE;

    /* Set L3 cache size to 1MB, this is valid for all configurations */

    l3crVal |= (L3CR_PMSIZ_1MB << L3CR_PMSIZ_BIT); 

    /* set the Private Memory size */

    if (pmSize == SIZE_1MB)
	l3crVal |= (L3CR_PMSIZ_1MB << L3CR_PMSIZ_BIT);
    else if (pmSize == SIZE_2MB)
	l3crVal |= (L3CR_PMSIZ_2MB << L3CR_PMSIZ_BIT);

    /* write to L3CR register */

    sysL3crPut(l3crVal);

    /* set L3CR[5] reserved bit to 1 */

    sysL3crPut(sysL3crGet() | L3CR_RES_INIT_MASK);

    /* set L3CR[L3CLKEN] = 1 */

    sysL3crPut(sysL3crGet() | L3CR_L3CLKEN_MASK);

    /* Globally invalidate the L3 Cache */

    sysL3CacheInv();

    /* Clear L3CR[L3CLKEN] */

    sysL3crPut(sysL3crGet() & ~L3CR_L3CLKEN_MASK);

    /* Wait 100 processor cycles - 1 microsecond */

    sysUsDelay(1);

    /* Configure Data and Address Parity & set L3CR[L3CLKEN] = 1 */

    l3crVal = sysL3crGet();

    l3crVal |=
     (((UINT)pVpdL3CachePacket->data[VPD_L3C_DATA_ERROR_DETECT]) <<
       L3CR_L3PE_BIT);

    l3crVal |=
     (((UINT)pVpdL3CachePacket->data[VPD_L3C_ADDR_ERROR_DETECT]) <<
       L3CR_L3APE_BIT);

    l3crVal |= L3CR_L3CLKEN_MASK;

    sysL3crPut(l3crVal);

    /* Wait 100 processor cycles - 1 microsecond */

    sysUsDelay(1);

    /* Set L3 Cache Enable, Clock Enable, Private Memory Enable */

#if (! defined INCLUDE_CACHE_L3)

    sysL3crPut(sysL3crGet() | L3CR_L3CLKEN_MASK | L3CR_PMEN_MASK);

#else
    if (cacheSize != 0x0)
	sysL3crPut(sysL3crGet() | (L3CR_L3E_MASK | L3CR_L3CLKEN_MASK |
                   L3CR_PMEN_MASK));
    else
	sysL3crPut(sysL3crGet() | L3CR_L3CLKEN_MASK | L3CR_PMEN_MASK);
#endif /* INCLUDE_CACHE_L3 */

    /* Wait 100 processor cycles - 1 microsecond */

    sysUsDelay(1);

    /* Scrub the Private Memory */

    sysL3CachePmScrub(INCLUDE_CACHE_L3_PM_BASE, pmSize);

    return (OK);
    } 

/******************************************************************************
*
* sysL3CachePmDisable - Disable the L3 private memory
*
* This routine disables the L3 private memory
*
* RETURNS: N/A
*/

void sysL3CachePmDisable (void)
    {

    /* Disable the L3 private memory by clearing the PMEN bit in L3CR */

    sysL3crPut(sysL3crGet() & ~L3CR_PMEN_MASK);
    sysL3CachePmBaseAddrSet(0x0);
    }
#endif /* INCLUDE_CACHE_L3_PM */

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

    /* clear the store queues in the memory subsystem */

    SYNC;

    /* unlock the L3 cache */

    sysL3CacheUnlock();
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

    /* ensure cache is locked before exiting */

    while (!(sysL3crGet() & (L3CR_L3IO_MASK | L3CR_L3DO_MASK)))
        ;
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
