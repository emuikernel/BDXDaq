/* sysCache.c - secondary (L2) cache library for the Falcon */

/* Copyright 1984-1997 Wind River Systems, Inc. */
/* Copyright 1996 Motorola, Inc. */

/*
modification history
--------------------
01e,21sep98,fle  doc : fixed a problem with sysL2LookasideCacheSize declaration
01d,14apr98,ms_  merged Motorola mv2700 support
01c,16oct97,scb  added MPC750 (Arthur) support.
01b,02jan97,wlf  doc: cleanup.
01a,14sep96,rhk  written.
*/

/*
DESCRIPTION

This library provides L2 cache support, and is derived from
code supplied by Motorola.

The L2 cache should always be set up in writethrough mode.  If the
L2 cache jumper is present on the MVME2600 series board, it should be 
removed.

The driver only sets or clears one bit at a time in the System
External Cache Control register, per the Genesis 2 programming
guide.

The flush bit in the System External Cache Control register should
not be used.

*/

/* includes */

#include "vxWorks.h"
#include "mv2600.h"

#if defined(INCLUDE_CACHE_SUPPORT) && defined(INCLUDE_CACHE_L2)

/* forward declarations */

STATUS           sysL2CacheInit (void);
LOCAL STATUS     sysL2LookasideCacheInit (void);
LOCAL STATUS     sysL2InlineCacheInit (void);

LOCAL void       sysL2LookasideCacheEnable (void);
LOCAL void       sysL2InlineCacheEnable (void);

void             sysL2CacheDisable (void);
LOCAL void       sysL2LookasideCacheDisable (void);
LOCAL void       sysL2InlineCacheDisable (void);

LOCAL UINT       sysL2LookasideCacheSize (void);
LOCAL UINT       sysL2InlineCacheSize (void);
LOCAL UINT	 sysL2InlineCacheSpd (void);
LOCAL int	 nop (int);

/* externals */

IMPORT UINT sysL2crGet (void);
IMPORT void sysL2crPut (UINT regVal);
IMPORT UINT sysGetMpuSpd (void);

LOCAL UINT gen2ConvTable[] =
    {
    0x0, 0x1, 0x2, 0x4, 0x5, 0x6, 0x0, 0x0, 0x0,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0
    };

LOCAL float gen2FreqTable[] =
    {
    1.0, 1.0, 1.5, 2.0, 2.5, 3.0, 1.0, 1.0,
    1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0
    };

/******************************************************************************
*
* sysL2CacheInit - initialize the L2 cache 
*
* This routine initializes and enables L2 cache support.  
*
* RETURNS: OK, or ERROR if cache is not present or not supported.
*
*/ 

STATUS sysL2CacheInit (void)
    {

    STATUS status = OK;
    /* check for presence of L2 cache */

    if (sysL2LookasideCacheSize() != 0)
	status |= sysL2LookasideCacheInit();

    if (sysL2InlineCacheSize() != 0)
	status |= sysL2InlineCacheInit();

    return (status);
    }

/******************************************************************************
*
* sysL2LookasideCacheInit - initialize the L2 lookaside cache 
*
* This routine initializes and enables L2 lookaside cache support.
*
* RETURNS: OK, or ERROR if cache is not present or not supported.
*
*/ 

LOCAL STATUS sysL2LookasideCacheInit(void)
    {

    /* check for presence of L2 cache */

    if (sysL2LookasideCacheSize() == 0)
        return(ERROR);		/* No L2 cache to init */

    /* disable the L2 cache, and invalidate the tags */
    sysL2LookasideCacheDisable ();

    /* Enable the L2 cache(s) */
    sysL2LookasideCacheEnable();

    return (OK);
    }

/******************************************************************************
*
* sysL2InlineCacheInit - initialize the L2 in-line cache.
*
* This routine initializes and enables L2 in-line cache support.
*
* RETURNS: OK, or ERROR if cache is not present or not supported.
*
*/ 

LOCAL STATUS sysL2InlineCacheInit(void)
    {
    UINT l2CacheSize;
    UINT l2crVal;
    UINT i;

    /* check for presence of L2 cache */

    if ((l2CacheSize = sysL2InlineCacheSize()) == 0)
        return(ERROR);		/* No L2 cache to init */

    /* Initialize RAM type, output hold, differential clock */

    switch ( (*MV2600_MCR) & MV2600_MCR_L2TYPE_MSK )
	{
	case MV2600_MCR_L2TYPE_LWP:   /* Late Write, with Parity */
	case MV2600_MCR_L2TYPE_LWNP:  /* Late Write, without Parity */
	    l2crVal = 0x01814000;     /* L2PE  = No parity enable      
				       * L2RAM = Pipe reg reg sync burst
				       * L2OH  = 1.0ns
				       * L2DF  = Diff clock
				       */
	    break;
	case MV2600_MCR_L2TYPE_BP:    /* Burst with parity */
	case MV2600_MCR_L2TYPE_BNP:   /* Burst, without Parity */
	    l2crVal = 0x01000000;     /* L2PE  = No parity enable      
				       * L2RAM = Flow thru reg buf sync burst
				       * L2OH  = 0.5ns
				       * L2DF  = Nondiff clock
				       */
	    break;
	default:
	    l2crVal = 0;
	    break;
	}

    /* Initialize the L2 clock ratio */

    i = (*MV2600_MCR & MV2600_MCR_L2PLL_MSK) >> 8;
    l2crVal |= gen2ConvTable[i] << 25;

    if (sysL2InlineCacheSpd() < 100)
        l2crVal |= MPC750_L2CR_SL;

    /* Set the Cache size */

    switch (l2CacheSize)
	{
	case 0x00040000:
	    l2crVal |= MPC750_L2CR_256K;
	    break;
	case 0x00080000:
	    l2crVal |= MPC750_L2CR_512K;
	    break;
	case 0x00100000:
	    l2crVal |= MPC750_L2CR_1024K;
	    break;
	}

    /* Write out the L2 cache control register */

    sysL2crPut(l2crVal);

    /* Invalidate the L2 cache */

    l2crVal |= MPC750_L2CR_I;
    sysL2crPut(l2crVal);
    while (sysL2crGet() & MPC750_L2CR_IP)
	;
    l2crVal &= ~MPC750_L2CR_I;
    sysL2crPut(l2crVal);

    /* Enable the L2 cache */
    sysL2InlineCacheEnable();

    return (OK);
    }

/******************************************************************************
*
* sysL2LookasideCacheEnable - enable the L2 lookaside cache
* 
* RETURNS: void
*
*/ 

LOCAL void sysL2LookasideCacheEnable (void)
    {
    unsigned char regVal;
 
    if (sysL2LookasideCacheSize() == 0)
	return;                   /* Return if no L2 Lookaside cache present */

    /* make sure L2 is already disabled */
    regVal = ((*MV2600_SXCCR) & L2_ENABLE);
    if (regVal == 0)
        *MV2600_SXCCR |= L2_ENABLE;
    }

/******************************************************************************
*
* sysL2InlineCacheEnable - enable the L2 in-line cache
* 
* RETURNS: void
*
*/ 

LOCAL void sysL2InlineCacheEnable (void)
    {
    UINT l2crVal;
 
    if (sysL2InlineCacheSize() == 0)
	return;                   /* Return if no L2 Lookaside cache present */

    /* make sure L2 is already disabled */
    if ( ((l2crVal = sysL2crGet()) & MPC750_L2CR_E) == 0)
	{
        l2crVal |= MPC750_L2CR_E;
	sysL2crPut(l2crVal);
	}
    }

/******************************************************************************
*
* sysL2CacheDisable - enable the L2 cache(s)
* 
* RETURNS: void
*
*/ 

void sysL2CacheDisable (void)
    {
    UINT cacheSize;

    if ((cacheSize = sysL2LookasideCacheSize()) != 0)
	{
	sysL2LookasideCacheDisable();
	}

    if ((cacheSize = sysL2InlineCacheSize()) != 0)
	{
	sysL2InlineCacheDisable();
	}
    }

/******************************************************************************
*
* sysL2LookasideCacheDisable - disable the L2 lookaside cache
*
* This routine disables the L2 lookaside cache if it was previously 
* initialized using sysL2CacheInit().  Calling this routine invalidates 
* the L2 tag bits.
*
* RETURNS: N/A
*
*/ 

LOCAL void sysL2LookasideCacheDisable (void)
    {
    unsigned char	regVal;
    int		  	temp;
    int			i;

    if (sysL2LookasideCacheSize() == 0)
	return;                          /* No cache to disable */

    /* Invalidate all L2 cache tags, clear the SXC_RST bit */

    regVal = *MV2600_SXCCR;
    regVal &= (~L2_RESET);
    *MV2600_SXCCR = regVal;

    for (i=0; i<=10; i++)
        temp = nop ( i );
 
    /* reset the SXC_RST bit in the cache control reg. */
    *MV2600_SXCCR |= L2_RESET;

    /* disable the L2 cache */
    regVal = *MV2600_SXCCR;
    regVal &= (~L2_DISABLE);
    *MV2600_SXCCR = regVal;
    }

/******************************************************************************
*
* sysL2InlineCacheDisable - disable the L2 in-line cache
*
* If the in-line cache is enabled, this routine disables it via the following
* steps:
*
* .IP "1."
* Flush by reading one word from each cache line within a cache
* enabled buffer.  Note that this buffer should be twice the size of
* the L2 cache to override the pseudo-LRU replacement algorithm.
* .IP "2."
* Turn of the cache enable bit in the L2CR.
*
* RETURNS: N/A
*
*/ 

LOCAL void sysL2InlineCacheDisable (void)
    {
    ULONG tmp;
    ULONG l2crVal;
    int i;

    if ((l2crVal = sysL2crGet()) & MPC750_L2CR_E)
	{
	if (vxHid0Get() & _PPC_HID0_DCE)
	    {
	    for (i = (2 * sysL2InlineCacheSize()); i >= 0; i -= 32 )
                    tmp = *((ULONG *)0 + i);
	    }
	sysL2crPut(l2crVal & ~MPC750_L2CR_E);
	}
    }

/******************************************************************************
*
* sysL2LookasideCacheSize - Determine size of the L2 lookaside cache.
*
* This function returns the size of the L2 lookaside cache.  If no L2
* lookaside cache is present, the size returned is zero so this function's
* return value can be used like a boolean to determine if the L2 lookaside
* cache is actually present.
*
* RETURNS: Size of L2 lookaside cache or zero if lookaside cache not present.
*
* NOTE: The L2 lookaside cache is sometimes referred to as the "glance" cache.
*/ 

LOCAL UINT sysL2LookasideCacheSize()
    {
    STATUS size;
    switch ( *MV2600_CCR & MV2600_CCR_SYSXC_MSK )
	{
        case MV2600_CCR_SYSXC_1024:
	    size = 0x100000;	    /* 1MB cache present */
	    break;
        case MV2600_CCR_SYSXC_512:     
	    size = 0x80000;         /* 512K cache present */
	    break;
        case MV2600_CCR_SYSXC_256:
	    size = 0x40000;         /* 256K cache present */
            break; 	           
        default:
            size = 0 ;              /* L2 lookaside cache is not present */
	    break;
	}
    return(size);
    }

/******************************************************************************
*
* sysL2InlineCacheSize - Determine size of the L2 in-line cache.
*
* This function returns the size of the L2 in-line cache.  If no L2
* in-line cache is present, the size returned is zero so this function's
* return value can be used like a boolean to determine if the L2 in-line
* cache is actually present.
*
* RETURNS: Size of L2 in-line cache or zero if in-line cache not present.
*
* NOTE: Don't confuse the L2 in-line cache with the other possible type
* of cache, referred to as the "glance" or "lookaside" cache.
*/ 

LOCAL UINT sysL2InlineCacheSize()
    {
    STATUS size;

    if (CPU_TYPE != CPU_TYPE_750)
	return(0);	/* Not an Arthur chip so no in-line L2 cache */

    switch ( *MV2600_CCR & MV2600_CCR_P0STAT_MSK )
        {
        case MV2600_CCR_P0STAT_256:
 	    size = 0x40000;
 	    break;
        case MV2600_CCR_P0STAT_512:
 	    size = 0x80000;
 	    break;
        case MV2600_CCR_P0STAT_1024:
 	    size = 0x100000;
 	    break;
        default:
 	    size = 0;
 	    break;
        }
    return(size);
    }

/******************************************************************************
*
* sysL2InlineCacheSpd - Determine size of the L2 in-line cache.
*
* This function returns the size of the L2 in-line cache.  If no L2
* in-line cache is present, the size returned is zero so this function's
* return value can be used like a boolean to determine if the L2 in-line
* cache is actually present.
*
* RETURNS: Size of L2 in-line cache or zero if in-line cache not present.
*
* NOTE: Don't confuse the L2 in-line cache with the other possible type
* of cache, referred to as the "glance" or "lookaside" cache.
*/ 

LOCAL UINT sysL2InlineCacheSpd(void)
   {
   UINT l2Spd;
   UINT i;

   if (sysL2InlineCacheSize() == 0)
      return(0);
   
   i = (*MV2600_MCR & MV2600_MCR_L2PLL_MSK) >> 8;

   l2Spd = (UINT)((sysGetMpuSpd() / gen2FreqTable[i]) + 0.5);

   return(l2Spd);
   }

/******************************************************************************
*
* nop - no op
*
* NOMANUAL
*/ 

LOCAL int nop
    (
    int dummy
    )
    {
    return (dummy + 1);
    }

#endif	/* INCLUDE_CACHE_SUPPORT && INCLUDE_CACHE_L2 */
