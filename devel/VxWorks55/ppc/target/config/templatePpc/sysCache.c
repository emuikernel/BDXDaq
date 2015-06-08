/* sysCache.c - secondary (L2) cache library for the Falcon */

/* Copyright 1984-2002 Wind River Systems, Inc. */

/*
TODO -	Remove the template modification history and begin a new history
	starting with version 01a and growing the history upward with
	each revision.

modification history
--------------------
01b,17jul02,dat  remove obsolete information
01a,08jul97,dat  written
*/

/*
DESCRIPTION

This library provides L2 cache support.

The L2 cache should always be set up in writethrough mode.

The driver only sets or clears one bit at a time in the System
External Cache Control register, per the Genesis 2 programming
guide.

The flush bit in the System External Cache Control register should
not be used.

*/

/* includes */

#include "vxWorks.h"
#include "config.h"

#if defined(INCLUDE_CACHE_SUPPORT) && defined(INCLUDE_CACHE_L2)

/******************************************************************************
*
* sysL2CacheInit - initialize the L2 cache 
*
* This routine initializes and enables L2 cache support.  It should be
* called at startup, before sysL2CacheEnable(), to avoid invalidating the L2
* tags.
*
* RETURNS: OK, or ERROR if cache is not present or not supported.
*
* SEE ALSO: sysL2CacheDisable(), sysL2CacheEnable()
*
*/ 

STATUS sysL2CacheInit (void)
    {

    /* TODO - quick exit if L2 cache is not present */

    sysL2CacheDisable ();	/* disable and invalidate all tags */

    /* TODO - configure the cache control reg. for normal L2 cache operation */

    return (OK);
    }

/******************************************************************************
*
* sysL2CacheDisable - disable the L2 cache
*
* This routine disables the L2 cache if it was previously initialized using 
* sysL2CacheInit().  Calling this routine invalidates the L2 tag bits.
*
* RETURNS: N/A
*
* SEE ALSO: sysL2CacheEnable(), sysL2CacheInit()
*/ 

void sysL2CacheDisable (void)
    {
    unsigned char	regVal;
    int		  	temp;
    int			i;

    /* TODO - Invalidate all L2 cache tags */

    /* TODO - disable the L2 cache */
    }

/******************************************************************************
*
* sysL2CacheEnable - enable the L2 cache
* 
* This routine enables the L2 cache by calling sysL2CacheInit().  It checks
* for the presence of an L2 cache and ensures that the cache is disabled.
*
* RETURNS: N/A
*
* SEE ALSO: sysL2CacheDisable(), sysL2CacheInit()
*/ 

void sysL2CacheEnable (void)
    {
    /* TODO - enable cache, if present */
    }

#endif	/* INCLUDE_CACHE_SUPPORT */
