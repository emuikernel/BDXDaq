/* sysCache.c - secondary (L2) cache library for the Hawk SMC */

/* Copyright 1984-1998 Wind River Systems, Inc. */
/* Copyright 1996,1997,1998,1999 Motorola, Inc., All Rights Reserved. */

/*
modification history
--------------------
01h,29jan99,rhv  Removed call to sysVpdInit in sysL2CacheInfoGet (already
                 done in sysHwInit).
01g,28jan99,rhv  Changed sysMsgSpit to sysDebugMsg.
01f,20jan99 mdp  Code cleanup.  Added defines for hardcoded values.
01e,13jan99 mdp	 Removed obsolete forward declaration to nop routine.
01d,13jan99 mdp	 Fixed inline cache speed routine to use VPD data.
01c,13jan99 mdp  Removed obsolete support for look aside L2 cache.
01b,05jan99 mdp	 Support for VPD-based configuration of L2 cache.
01a,15dec98 mdp  Written (from version 01d of mv2304 bsp).
*/

/*
DESCRIPTION

This library provides L2 cache support, and is derived from
code supplied by Motorola.

Support is included for initializing/enabling the L2 Cache on boot and
enabling/disabling the L2 Cache with system calls.  Only Inline caches are
supported since there are no VPD-based boards with a look-aside cache.
Parameters used to configured the L2 controller are gathered from the L2
Cache Record in the VPD.

*/

/* includes */

#include "vxWorks.h"
#include "mv2400.h"
#include "sysCache.h"

#if defined(INCLUDE_CACHE_SUPPORT) && defined(INCLUDE_CACHE_L2)

/* forward declarations */

STATUS           sysL2CacheInit (void);
LOCAL STATUS     sysL2InlineCacheInit (void);

void             sysL2CacheEnable (void);
LOCAL void       sysL2InlineCacheEnable (void);

void             sysL2CacheDisable (void);
LOCAL void       sysL2InlineCacheDisable (void);

LOCAL STATUS     sysL2CacheInfoGet(UCHAR, UCHAR *);
LOCAL UINT       sysL2InlineCacheSize (void);
LOCAL UINT	 sysL2InlineCacheSpd (void);

/* externals */

IMPORT UINT sysL2crGet (void);
IMPORT void sysL2crPut (UINT regVal);
IMPORT UINT sysGetMpuSpd (void);

LOCAL UINT boardConvTable[] =
    {
    0x0, 0x1, 0x2, 0x4, 0x5, 0x6, 0x0, 0x0, 0x0,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0
    };

LOCAL float boardFreqTable[] =
    {
    10, 10, 15, 20, 25, 30, 10, 10,
    10, 10, 10, 10, 10, 10, 10, 10
    };

LOCAL VPD_PACKET * pVpdL2CachePacket = NULL;

/******************************************************************************
*
* sysL2CacheInfoGet - Get L2 Cache Info from the VPD.
*
* This routine parses the VPD data structure for a L2 cache record and returns
* field information in the record based on the L2CachePacketField that is
* passed to it. 
*
* RETURNS: OK, or ERROR if VPD L2 cache record missing or corrupt.
*
* SEE ALSO: sysL2InlineCacheInit(), sysL2GetCacheSize()
*
*/ 
LOCAL STATUS sysL2CacheInfoGet(UCHAR L2CachePacketFieldId,
			       UCHAR * pL2CachePacketField)
    {
      UINT i, fieldSize = 0;
      BOOL fieldError = FALSE;

      if (pVpdL2CachePacket == NULL)
        {

        /* Extract a VPD L2 Cache Packet */
        if (sysVpdPktGet(VPD_PID_L2, 0, &sysVpdPkts[0], &pVpdL2CachePacket)
            == ERROR)
          return (ERROR);
        }

      /* Determine field length and if the field has a valid parameter */
      switch (L2CachePacketFieldId)
        {
	case L2C_MID:
	case L2C_DID:
	    fieldSize = 2;
	    break;

	case L2C_DDW:
	case L2C_NOD:
	case L2C_NOC:
	case L2C_CW:
	case L2C_ASSOCIATE:
	    fieldSize = 1;
	    break;

	case L2C_TYPE:
	    if (pVpdL2CachePacket->data[L2CachePacketFieldId] > L2C_TYPE_INLINE)
		fieldError = TRUE;
	    else
		fieldSize = 1;
	    break;

	case L2C_OPERATIONMODE:
	    if (pVpdL2CachePacket->data[L2CachePacketFieldId] > L2C_OM_WB)
		fieldError = TRUE;
	    else
		fieldSize = 1;
	    break;

	case L2C_ERROR_DETECT:
	    if (pVpdL2CachePacket->data[L2CachePacketFieldId] > L2C_ED_ECC)
		fieldError = TRUE;
	    else
		fieldSize = 1;
	    break;

        case L2C_SIZE:
            if (pVpdL2CachePacket->data[L2CachePacketFieldId] > L2C_SIZE_4M)
                fieldError = TRUE;
            else
                fieldSize = 1;
            break;

        case L2C_TYPE_BACKSIDE:
            if (pVpdL2CachePacket->data[L2CachePacketFieldId] > L2C_TYPE_BS_BNP)
                fieldError = TRUE;
            else
                fieldSize = 1;
            break;

        case L2C_RATIO_BACKSIDE:
	    if (pVpdL2CachePacket->data[L2CachePacketFieldId] > L2C_RATIO_3_1)
	    	fieldError = TRUE;
	    else
	 	fieldSize = 1;	
            break;

        default:
            fieldError= TRUE;
	    break;
        }

	if (!fieldError)
	   {
	   if (fieldSize == 1)
               pL2CachePacketField[0] =
		 pVpdL2CachePacket->data[L2CachePacketFieldId]; 
	   else
      	       for (i=0; i<fieldSize; i++)
        	   pL2CachePacketField[i] =
		     pVpdL2CachePacket->data[L2CachePacketFieldId+i];

	   return OK;
	   }
	else
	   return ERROR;

    }

/******************************************************************************
*
* sysL2CacheInit - initialize the L2 cache 
*
* This routine initializes and enables L2 cache(s) support. 
*
* RETURNS: OK, or ERROR if cache is not present or not supported.
*
* SEE ALSO: sysL2CacheInit(), sysL2CacheEnable(), sysL2CacheDisable()
*
*/ 

STATUS sysL2CacheInit (void)
    {

    STATUS status = OK;

    /* check for presence of L2 cache */

    if (sysL2InlineCacheSize() != 0)
	status |= sysL2InlineCacheInit();

    return (status);

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
    UCHAR l2CacheType, l2CacheRatio;

    /* check for presence of L2 cache */

    if ((l2CacheSize = sysL2InlineCacheSize()) == 0)
        return(ERROR);		/* No L2 cache to init */

    /* Initialize RAM type, output hold, differential clock */

    if (sysL2CacheInfoGet(L2C_TYPE_BACKSIDE, &l2CacheType) == ERROR)
        {
	sysDebugMsg ("L2 cache record missing or corrupt.  L2 Cache Disabled.",
                     CONTINUE_EXECUTION);
	return(ERROR);		/* VPD Error */
        }

    switch ( l2CacheType )
	{
	case L2C_TYPE_BS_LWP:	      /* Late Write, with Parity */
	case L2C_TYPE_BS_LWNP:        /* Late Write, without Parity */

	    /*
	       L2PE  = No parity enable
	       L2RAM = Pipe reg-reg sync late-write
	       L2OH  = 1.0ns
	       L2DF  = Diff clock
	    */      

	    l2crVal = MPC750_L2CR_RAM_PLTWR |
		      MPC750_L2CR_OH_1NS |
		      MPC750_L2CR_DF;
	    break;

	case L2C_TYPE_BS_BP:          /* Burst with parity */
	case L2C_TYPE_BS_BNP:         /* Burst, without Parity */

	    /*
	       L2PE  = No parity enable
	       L2RAM = Pipe reg-reg sync burst
	       L2OH  = 0.5ns
	       L2DF  = Nondiff clock
	    */

	    l2crVal = MPC750_L2CR_RAM_PBRST;      
	    break;

	default:
	    l2crVal = 0;
	    break;
	}

    /* Initialize the L2 clock ratio */
    
    if (sysL2CacheInfoGet(L2C_RATIO_BACKSIDE, &l2CacheRatio) == ERROR)
        {
	sysDebugMsg("L2 cache record missing or corrupt.  L2 cache disabled.",
                    CONTINUE_EXECUTION);
	return (ERROR);
	}

    l2crVal |= boardConvTable[l2CacheRatio] << 25;

    if (sysL2InlineCacheSpd() < 100)
        l2crVal |= MPC750_L2CR_SL;

    /* Set the Cache size */

    switch (l2CacheSize)
	{
	case L2C_MEM_256K:
	    l2crVal |= MPC750_L2CR_256K;
	    break;
	case L2C_MEM_512K:
	    l2crVal |= MPC750_L2CR_512K;
	    break;
	case L2C_MEM_1M:
	    l2crVal |= MPC750_L2CR_1024K;
	    break;
	case L2C_MEM_2M:
	    l2crVal |= MPC750_L2CR_2048K;
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
* sysL2InlineCacheEnable - enable the L2 in-line cache
* 
* RETURNS: N/A
*
*/ 

LOCAL void sysL2InlineCacheEnable (void)
    {
    UINT l2crVal;
 
    if (sysL2InlineCacheSize() == 0)
	return;                   /* Return if no L2 Inline cache present */

    /* make sure L2 is already disabled */
    if ( ((l2crVal = sysL2crGet()) & MPC750_L2CR_E) == 0)
	{
        l2crVal |= MPC750_L2CR_E;
	sysL2crPut(l2crVal);
	}
    }

/******************************************************************************
*
* sysL2CacheDisable - disable the L2 cache(s)
* 
* This routine disables the L2 cache(s) if either or both were
* previously initialized using sysL2CacheInit().  Calling this 
* routine invalidates the L2 tag bits.
*
* RETURNS: N/A
*
* SEE ALSO: sysL2CacheInit(), sysL2CacheEnable(), sysL2CacheDisable()
*
*/ 

void sysL2CacheDisable (void)
    {
    UINT cacheSize;

    if ((cacheSize = sysL2InlineCacheSize()) != 0)
	sysL2InlineCacheDisable();

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
* Turn off the cache enable bit in the L2CR.
* .IP "3."
* Invalidate the L2 cache contents.
*
* RETURNS: N/A
*
*/ 

LOCAL void sysL2InlineCacheDisable (void)
    {
    register ULONG tmp;
    register ULONG l2crVal;
    register int i;

    if ((l2crVal = sysL2crGet()) & MPC750_L2CR_E)
	{
	if (vxHid0Get() & _PPC_HID0_DCE)
	    {
	    /* flush the inline L2 cache */
	    for (i = 2*sysL2InlineCacheSize() - 32; i >= 0; i -= 32 )
		tmp = *(ULONG *)(RAM_LOW_ADRS + i);
	    }

	/* disable the inline L2 cache */
	l2crVal &= ~MPC750_L2CR_E;
	sysL2crPut(l2crVal);

	/* invalidate the inline L2 cache */
	sysL2crPut(l2crVal | MPC750_L2CR_I);
	while (sysL2crGet() & MPC750_L2CR_IP) {}
	sysL2crPut(l2crVal);
	}
    }

/******************************************************************************
*
* sysL2InlineCacheSize() - Determine size of the L2 in-line cache.
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
    UCHAR l2CacheSize;

    if (CPU_TYPE != CPU_TYPE_750 && CPU_TYPE != CPU_TYPE_MAX)
	return(0);	/* Not an Arthur/Max chip so no in-line L2 cache */

    if (sysL2CacheInfoGet(L2C_SIZE, &l2CacheSize) == ERROR)
	{
	sysDebugMsg ("L2 cache record missing or corrupt.  L2 cache disabled.",
                     CONTINUE_EXECUTION);
	return (0);
	}

    switch ( l2CacheSize )
        {
        case L2C_SIZE_256K:
 	    size = L2C_MEM_256K;
 	    break;
        case L2C_SIZE_512K:
 	    size = L2C_MEM_512K;
 	    break;
        case L2C_SIZE_1M:
 	    size = L2C_MEM_1M;
 	    break;
        case L2C_SIZE_2M:
 	    size = L2C_MEM_2M;
 	    break;
	case L2C_SIZE_4M:
	    size = L2C_MEM_4M;
            break;
        default:
 	    size = L2C_MEM_NONE;
 	    break;
        }
    return(size);
    }

/******************************************************************************
*
* sysL2InlineCacheSpd() - Determine speed of the L2 in-line cache.
*
* This function returns the speed of the L2 in-line cache.  If no L2
* in-line cache is present, the speed returned is zero so this function's
* return value can be used like a boolean to determine if the L2 in-line
* cache is actually present.
*
* RETURNS: Speed of L2 in-line cache or zero if in-line cache not present.
*
* NOTE: Don't confuse the L2 in-line cache with the other possible type
* of cache, referred to as the "glance" or "lookaside" cache.
*/ 

LOCAL UINT sysL2InlineCacheSpd(void)
   {
   UINT l2Spd;
   UCHAR l2CacheRatio;

   if (sysL2InlineCacheSize() == 0)
      return(0);
   
   if (sysL2CacheInfoGet(L2C_RATIO_BACKSIDE, &l2CacheRatio) == ERROR)
        {
        sysDebugMsg ("L2 cache record missing or corrupt.  L2 Cache Disabled.",
                     CONTINUE_EXECUTION);
        return(ERROR);          /* VPD Error */
        }

   l2Spd = (UINT)((sysGetMpuSpd() / boardFreqTable[l2CacheRatio]) * 10);

   return(l2Spd);
   }

/******************************************************************************
*
* sysL2CacheEnable - enable the L2 cache(s)
* 
* This routine enables the L2 cache(s).
* It checks for the presence of either L2 cache 
* and ensures that the cache(s) is(are) disabled.
*
* RETURNS: N/A
*
* SEE ALSO: sysL2CacheInit(), sysL2CacheEnable(), sysL2CacheDisable()
*
*/ 

void sysL2CacheEnable (void)
    {
    UINT cacheSize;

    if ((cacheSize = sysL2InlineCacheSize()) != 0)
	sysL2InlineCacheEnable();
    }

#endif	/* INCLUDE_CACHE_SUPPORT && INCLUDE_CACHE_L2 */
