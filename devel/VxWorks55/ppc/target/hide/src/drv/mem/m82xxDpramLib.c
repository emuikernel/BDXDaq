/* m82xxDpramLib.c - 82xx DPRAM memory allocation driver */

/* Copyright 1984-2001 Wind River Systems, Inc. */

/*
modification history
--------------------
01d,26nov01,rcs   renamed m82xxDpramLib.c and moved to target/src/drv/mem
01c,12oct01,g_h   Cleaning for T2.2
01b,17jul01,g_h   Cleaning and renaming to sys82xxDpramLib.c
01a,16jul01,p_h   Created
*/

/*
DESCRIPTION
This module contains 82xx DPRAM memory allocation driver.

INCLUDES: m82xxDpramLib.h
*/

/* include */

#include "drv/mem/m82xxDpramLib.h"

/* gobals */

/* external */
IMPORT UINT32 vxImmrGet(void); 
/* local */

LOCAL ADDR_RESOURCE addrResPool[ALLOCTABLESIZE];
LOCAL POOL dprMainPool;
LOCAL POOL dprFccPool;

/* forward declaration */

/*******************************************************************************
*
* m82xxDpramAlignedAddr - aligned a given address
*
* This routine aligned a given address.
*
* RETURNS: new aligned address
*
* SEE ALSO: m82xxDpramAllocRange(), m82xxDpramFindFreeEntry(),
*           m82xxDpramAllocateRange(), m82xxDpramReleaseRenage(),
*           m82xxDpramLibInit(), m82xxDpramMalloc(), 
*           m82xxDpramAlignedMalloc(), m82xxDpramFree(),
*           m82xxDpramFccMalloc(), m82xxDpramFccFree()
*/

LOCAL UINT32 m82xxDpramAlignedAddr
    (
    UINT32 addr, 
    UINT32 alignment
    )
    {
    if (alignment) 
	{
	return ((addr + alignment - 1) & ~(alignment - 1));
	}

    return (addr);
    }

/*******************************************************************************
*
* m82xxDpramAllocRange - alloc aligned memeory
*
* This routine alloc aligned memory
*
* RETURNS: OK
*
* SEE ALSO: m82xxDpramAlignedAddr(), m82xxDpramFindFreeEntry(),
*           m82xxDpramAllocateRange(), m82xxDpramReleaseRenage(),
*           m82xxDpramLibInit(), m82xxDpramMalloc(), 
*           m82xxDpramAlignedMalloc(), m82xxDpramFree(),
*           m82xxDpramFccMalloc(), m82xxDpramFccFree()
*/

LOCAL void * m82xxDpramAllocRange
    (
    ADDR_RESOURCE * pNew, 
    UINT32          length, 
    UINT32          alignment, 
    POOL          * pPool
    )
    {
    
    ADDR_RESOURCE * pPrev;
    ADDR_RESOURCE * pCurr;
    UINT32          gapEnd;
    UINT32          gapStart = 0;


    pCurr = pPool->pHead;
    pPrev = 0;

    do
        {
	if (pPrev) 
	    {
	    gapStart = m82xxDpramAlignedAddr (pPrev->start + pPrev->size, alignment);
            }
	else 
	    {
	    gapStart = m82xxDpramAlignedAddr (pPool->start, alignment);
            }

	if (pCurr) 
	    {
	    gapEnd = pCurr->start;
	    }
	else 
	    {
	    gapEnd = pPool->limit;
	    }

        if (gapEnd > gapStart) 
	    {
	    if ((gapEnd - gapStart) >= length) 
	        {
		pNew->start = gapStart;
		pNew->size = length;
		pNew->pNext = pCurr;

		if (pPrev) 
		    {
		    pPrev->pNext = pNew;
		    }

		if (!pPool->pHead) 
		    {
		    pPool->pHead = pNew;
		    }

		return ((void*)(pNew->start));
		}
	    }

            pPrev = pCurr;
            if (pCurr) 
		{
		pCurr = pCurr->pNext;
		}
        } while (pCurr || pPrev);

    return (OK);
    }

/*******************************************************************************
*
* m82xxDpramFindFreeEntry - find first entry in the linked list
*
* RETURNS: pointer to first free entry or 0 if every thing full
*
* SEE ALSO: m82xxDpramAlignedAddr(), m82xxDpramAllocRange(),
*           m82xxDpramAllocateRange(), m82xxDpramReleaseRenage(),
*           m82xxDpramLibInit(), m82xxDpramMalloc(), 
*           m82xxDpramAlignedMalloc(), m82xxDpramFree(),
*           m82xxDpramFccMalloc(), m82xxDpramFccFree()
*/

LOCAL ADDR_RESOURCE * m82xxDpramFindFreeEntry 
    (
    void
    )
    {
    int index;

    for (index=0;index<ALLOCTABLESIZE;index++) 
	{
	if (addrResPool[index].start == 0) 
	    {
	    return &addrResPool[index];
	    }
	}
	
    return (0);
    }

/*******************************************************************************
*
* m82xxDpramAllocateRange - allocate a reange of memory
*
* This routine allocate a reange of memory from the DPRAM.
*
* RETURNS: pointer to allocated memory or 0 if the is no free memory
*
* SEE ALSO: m82xxDpramAlignedAddr(), m82xxDpramAllocRange(),
*           m82xxDpramFindFreeEntry(), m82xxDpramReleaseRenage(),
*           m82xxDpramLibInit(), m82xxDpramMalloc(), 
*           m82xxDpramAlignedMalloc(), m82xxDpramFree(),
*           m82xxDpramFccMalloc(), m82xxDpramFccFree()
*/

LOCAL void * m82xxDpramAllocateRange
    (
    UINT32  requestedLength, 
    UINT32  alignment, 
    POOL  * pPool
    )
    {
    ADDR_RESOURCE * pNew;

    if ((pNew = m82xxDpramFindFreeEntry ()) != NULL)
        { 
	return m82xxDpramAllocRange (pNew, requestedLength, alignment ,pPool);
	}

    return 0;
    }

/*******************************************************************************
*
* m82xxDpramReleaseRenage - release allocated reange of memory
*
* This routine release allocated reange of memory from the DPRAM.
*
* RETURNS: N/A
*
* SEE ALSO: m82xxDpramAlignedAddr(), m82xxDpramAllocRange(),
*           m82xxDpramFindFreeEntry(), m82xxDpramAllocateRange(),
*           m82xxDpramLibInit(), m82xxDpramMalloc(), 
*           m82xxDpramAlignedMalloc(), m82xxDpramFree(),
*           m82xxDpramFccMalloc(), m82xxDpramFccFree()
*/

LOCAL void m82xxDpramReleaseRenage
    (
    UINT32  start, 
    POOL  * pPool
    )
    {
    ADDR_RESOURCE *pCurr, *pPrev;

    pCurr = pPool->pHead;
    pPrev = 0;

    while (pCurr) 
	{
	if (pCurr->start == start) 
            {
	    if (pPrev) 
		{
		pPrev->pNext = pCurr->pNext;
		}
            else 
		{
		pPool->pHead = pCurr->pNext;
		}

            /* clear the released AddrResource structure */
            memset (pCurr,0,sizeof(ADDR_RESOURCE));

            return;
	    }

        pPrev = pCurr;
	pCurr = pCurr->pNext;
	}

    return;
    }

/*******************************************************************************
*
* m82xxDpramLibInit - init DPRAM memory allocation driver
*
* This routine init the DPRAM memory allocation driver.
*
* RETURNS: N/A
*
* SEE ALSO: m82xxDpramAlignedAddr(), m82xxDpramAllocRange(),
*           m82xxDpramFindFreeEntry(), m82xxDpramAllocateRange(),
*           m82xxDpramReleaseRenage(), m82xxDpramMalloc(), 
*           m82xxDpramAlignedMalloc(), m82xxDpramFree(),
*           m82xxDpramFccMalloc(), m82xxDpramFccFree()
*/

void m82xxDpramLibInit
    (
    void
    )
    {
    UINT32 immr = vxImmrGet ();	       /* find the DPRAM start address */

    /* check for uCode */
    UINT32 rccr = (*(UINT32 *)(immr + 0x119C4) & 0xE000) >> 2; 

    /* init the main dpram memory pool */
    dprMainPool.start = immr + rccr + 0x100;
    dprMainPool.limit = immr + 0x4000 -1;
    dprMainPool.pHead = 0;

    /* init the spesial section memory pool (need for FCC in ATM or ETH mode)*/
    dprFccPool.start = immr + 0xb000;
    dprFccPool.limit = immr + 0xc000 - 1;
    dprFccPool.pHead = 0;

    /* clear the AddrResource pool */
    memset (addrResPool,0,sizeof(ADDR_RESOURCE) * ALLOCTABLESIZE);
    }

/*******************************************************************************
*
* m82xxDpramMalloc - allocate memory from DPRAM main pool
*
* This routine allocate memory from DPRAM main pool.
*
* RETURNS: pointer to allocated memory 
*
* SEE ALSO: m82xxDpramAlignedAddr(), m82xxDpramAllocRange(),
*           m82xxDpramFindFreeEntry(), m82xxDpramAllocateRange(),
*           m82xxDpramReleaseRenage(), m82xxDpramLibInit(), 
*           m82xxDpramAlignedMalloc(), m82xxDpramFree(),
*           m82xxDpramFccMalloc(), m82xxDpramFccFree()
*/

void * m82xxDpramMalloc
    (
    size_t length
    )
    {
    return (m82xxDpramAllocateRange (length,0,&dprMainPool));
    }

/*******************************************************************************
*
* m82xxDpramAlignedMalloc - allocate aligned memory from DPRAM main pool
*
* This routine allocate aligned memory from DRPAM main pool.
*
* RETURNS: pointer to allocated aligend memory 
*
* SEE ALSO: m82xxDpramAlignedAddr(), m82xxDpramAllocRange(),
*           m82xxDpramFindFreeEntry(), m82xxDpramAllocateRange(),
*           m82xxDpramReleaseRenage(), m82xxDpramLibInit(), 
*           m82xxDpramMalloc(), m82xxDpramFree(),
*           m82xxDpramFccMalloc(), m82xxDpramFccFree()
*/

void * m82xxDpramAlignedMalloc
    (
    size_t length, 
    size_t alignment
    )
    {
    return (m82xxDpramAllocateRange (length, alignment,&dprMainPool));
    }

/*******************************************************************************
*
* m82xxDpramFree -  free allocated memory from DPRAM main pool
*
* This routine free allocated memory from DPRAM main pool.
*
* RETURNS: N/A
*
* SEE ALSO: m82xxDpramAlignedAddr(), m82xxDpramAllocRange(),
*           m82xxDpramFindFreeEntry(), m82xxDpramAllocateRange(),
*           m82xxDpramReleaseRenage(), m82xxDpramLibInit(), 
*           m82xxDpramMalloc(), m82xxDpramAlignedMalloc(),
*           m82xxDpramFccMalloc(), m82xxDpramFccFree()
*/

void m82xxDpramFree 
    (
    void * addr
    )
    {
    m82xxDpramReleaseRenage ((UINT32)addr,&dprMainPool);
    }

/*******************************************************************************
*
* m82xxDpramFccMalloc -  allocate memory from DPRAM FCC special pool
*
* This routine allocate memory from DPRAM FCC special pool.
*
* RETURNS: pointer to allocated memory
*
* SEE ALSO: m82xxDpramAlignedAddr(), m82xxDpramAllocRange(),
*           m82xxDpramFindFreeEntry(), m82xxDpramAllocateRange(),
*           m82xxDpramReleaseRenage(), m82xxDpramLibInit(), 
*           m82xxDpramMalloc(), m82xxDpramAlignedMalloc(),
*           m82xxDpramFree(), m82xxDpramFccFree()
*/

void * m82xxDpramFccMalloc
    (
    size_t length, 
    size_t alignment
    )
    {
    return (m82xxDpramAllocateRange(length, alignment,&dprFccPool));
    }

/*******************************************************************************
*
* m82xxDpramFccFree -  free allocated memory from DPRAM FCC special pool
*
* This routine free alocated memory from DPRAM FCC special pool.
*
* RETURNS: pointer to allocated memory 
*
* SEE ALSO: m82xxDpramAlignedAddr(), m82xxDpramAllocRange(),
*           m82xxDpramFindFreeEntry(), m82xxDpramAllocateRange(),
*           m82xxDpramReleaseRenage(), m82xxDpramLibInit(), 
*           m82xxDpramMalloc(), m82xxDpramAlignedMalloc(),
*           m82xxDpramFree(), m82xxDpramFccMalloc()
*/

void m82xxDpramFccFree
    (
    void * addr
    )
    {
    m82xxDpramReleaseRenage ((UINT32)addr,&dprFccPool);
    }
