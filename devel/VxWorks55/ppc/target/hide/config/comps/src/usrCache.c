/* usrCache.c - cache configuration file */

/* Copyright 1984-1998 Wind River Systems, Inc. */

/*
modification history
--------------------
01a,02jun98,ms   written
*/

/*
DESCRIPTION
User configurable cache initialization.
*/

/******************************************************************************
*
* usrCacheEnable - configure the processor caches
*/

STATUS usrCacheEnable (void)
    {
#ifdef  USER_I_CACHE_ENABLE
    cacheEnable (INSTRUCTION_CACHE);            /* enable instruction cache */
#endif  /* USER_I_CACHE_ENABLE */

#ifdef  USER_D_CACHE_ENABLE
    cacheEnable (DATA_CACHE);                   /* enable data cache */
#endif  /* USER_D_CACHE_ENABLE */

#if	defined(USER_B_CACHE_ENABLE) && (CPU==MC68060)
    cacheEnable (BRANCH_CACHE);                 /* enable branch cache */
#endif  /* USER_B_CACHE_ENABLE */

    return (OK);
    }

