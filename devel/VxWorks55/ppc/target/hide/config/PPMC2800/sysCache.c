/******************************************************************************
*              (c), Copyright, MOTOROLA  INC  - ECC                  *
*                                                                             *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MOTOROLA , INC.        *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT *
* OF MOTOROLA   OR ANY THIRD PARTY. MOTOROLA   RESERVES THE RIGHT AT ITS SOLE DISCRETION*
* TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MOTOROLA .       *
* THIS CODE IS PROVIDED "AS IS". MOTOROLA  MAKES NO WARRANTIES, EXPRESS,*
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.  *
******************************************************************************/

/*================================== INCLUDES ================================*/

/*================================== DEFINES =================================*/
/*
 * some defines for the PowerPC 750 L2 cache controler:
 * The defines are located here, until Windriver provide MPC 750 support
 */
#define	PPC750_L2CR	1017		/* L2 Cache Control Register (SPR)*/

#define	L2CR_L2E	0x80000000	/* enable L2 cache		*/
#define	L2CR_L2PE	0x40000000	/* enable L2 data parity 	*/


#define	L2CR_L2RAM_FT	0x00000000	/* L2 RAM flow trough		*/
#define	L2CR_L2RAM_RES	0x00800000	/* L2 RAM reserved		*/

#define	L2CR_L2DO	0x00400000	/* L2 data only operations	*/
#define	L2CR_L2I	0x00200000	/* L2 globale invalidate	*/
#define	L2CR_L2CTL	0x00100000	/* L2 low power RAM control	*/
#define	L2CR_L2WT	0x00080000	/* L2 write through mode	*/
#define	L2CR_L2TS	0x00040000	/* L2 test support		*/

#define	L2CR_L2IP	0x00000001	/* L2 global invalidate in prog.*/

#ifndef SYNC
#define	SYNC		__asm__("sync")
#endif

#ifndef EIEIO
#define	EIEIO		__asm__("eieio")
#endif
/*================================== TYPEDEFS ================================*/

/*================================== IMPORTS =================================*/
IMPORT UINT32 sysHid1Get(void);
IMPORT UINT32 sysL2crGet(void);
IMPORT void sysL2crPut(UINT32);

/*================================== GLOBALS =================================*/

/*================================== LOCALS ==================================*/
 
/*================================== IMPLEMENTATION ==========================*/

#if	defined(INCLUDE_CACHE_SUPPORT) || defined(DOC)

/* forward declarations */
void	sysL2CacheDisable (void);
void	sysL2CacheFlush (void);
void	sysL2cacheEnable (void);

/******************************************************************************
*
* sysL2CacheInit - BSP specific L2 cache inialization
*
* This routine initializes and enables L2 cache support on the board.
*
* RETURNS: OK, always.
*
* NOMANUAL
*/ 

STATUS sysL2CacheInit (void)
    {
    int 	i, l;


    /* IF cache is already running, disable it first */

    l = intLock();
    
    sysL2CacheDisable ();
    sysL2CacheFlush ();

    sysL2crPut((UINT32)0 |
#ifdef L2_CACHE_DATA_ONLY
	L2CR_L2DO	|	/* enable data only mode */
#else
	0		|	/* disable data only mode */
#endif
#ifdef L2_CACHE_COPY_BACK
	0		|	/* copy back mode */
#else
	L2CR_L2WT	|	/* write through mode */
#endif
	0
	);

    /* wait for at least 640 clock cycles */
    for(i=0; i<1000; i++) EIEIO;

    /* start the L2 cache */
    sysL2crPut(((UINT32)sysL2crGet()) | L2CR_L2E);

    return (OK);
    }

/******************************************************************************
*
* sysL2CacheDisable - disable the 2nd level cache (L2 cache)
*
* This routine disables the L2 cache. It assumes the L2 cache has
* previously been initialized and enabled.
*
* RETURNS: N/A
*
* SEE ALSO: sysL2CacheEnable(), sysL2CacheFlush()
*/ 

void sysL2CacheDisable (void)
    {

    /*
     * Check if L2 cache is enabled. If it is already disabled
     * nothing had to be done.
     */
    if ( (((UINT32)sysL2crGet()) & L2CR_L2E) != L2CR_L2E )
	{
    	return;
	}

    /* Disable the L2 cache */
    sysL2crPut(((UINT32)sysL2crGet()) & (~ L2CR_L2E ));

    EIEIO; SYNC;
    }

/******************************************************************************
*
* sysL2CacheEnable - enable the 2nd level cache (L2 cache)
*
* This routine initializes and enables the L2 cache.
*
* RETURNS: N/A
*
* SEE ALSO: sysL2CacheDisable(), sysL2CacheFlush()
*/

void sysL2CacheEnable (void)
    {
    sysL2CacheInit();
    }

/******************************************************************************
*
* sysL2CacheFlush - flush the 2nd level cache (L2 cache)
*
* This routine flushes and invalidates the L2 cache. It assumes
* that the L2 cache has previously been initialized and is disabled (!).
*
* RETURNS: N/A
*
* SEE ALSO: sysL2CacheEnable(), sysL2CacheDisable()
*/ 

void sysL2CacheFlush (void)
    {
    UINT32	tmp;
    int		l;
    
    /*
     * NOTE:
     * Cache flush will fail if dynamic power management is also enabled.
     * Disable dynamic power management (DPM) if used !!!!
     */


    tmp = (UINT32)sysL2crGet();
    
    if (tmp & L2CR_L2E)
	{
	return;
	}

    l = intLock();
    
    /* start L2 invalidation */
    SYNC;
    sysL2crPut(tmp | L2CR_L2I);
    SYNC;

    /* waiting for L2 invalidation */
    while((((UINT32)sysL2crGet()) & L2CR_L2IP) != 0);

    /* clear invalidation bit */
    sysL2crPut(tmp & (~ L2CR_L2I));

    intUnlock (l);

    }

#if FALSE
#define L1_SIZE	0x8000
#define L1_WAYS	8
#define L2_SIZE	0x80000
#define L2_WAYS	2

UINT8 sysCacheLineFlush	
    (
    UINT8	*adrs
    )
    {
    UINT8	*accAdrs;
    UINT8	 dummy;
    int		 i, j;
    
    for (i = 0; i <= L2_WAYS; i++)
	{
	accAdrs = (UINT8*)((UINT32)adrs & (L2_SIZE-1) + L2_SIZE * i);
	
	for (j = 0; j <= L1_WAYS; j++)
	    {
	    dummy += accAdrs[j * L1_SIZE];
	    }
	}
    
    return dummy;    
    }
#endif

#endif	/* INCLUDE_CACHE_SUPPORT || DOC */


