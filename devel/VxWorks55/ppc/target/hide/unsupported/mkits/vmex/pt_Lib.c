/* pt_Lib.c -  fixed block memory allocation routines */

/* Copyright 1984-1996 Wind River Systems, Inc. */
#include "copyright_wrs.h"

/*
modification history
--------------------
01d,16oct96,jmb  handle ident call before object table initialized
01c,03jul96,jmb  changed allocation of memory partition to guarantee
		 minimum 8-byte alignment.
01b,13jun96,jmb  comment clean-up
01a,25apr96,jmb  created
*/

/*****************************************************************************
*
* pt_Lib -  fixed-sized memory routines
*
* These routines are used to manage memory partitions containing
* fixed-size buffers.  These routines are build on top of vxWorks'
* malloc -- using them in the vxWorks context will require some
* modification of user code.  The recommended usage is as follows...
*
* - call pt_create with the address arguments (laddr and
* paddr) set to NULL.  vxWorks will then malloc the buffer space from
* the system partition.  
*
* - Alternatively, one can pass a logical address
* to pt_create (the physical address argument is ignored).  If the
* logical address is within the system partition, the memory must be
* malloc'd by the caller before calling pt_create.  
*
* - Another alternative is to pass pt_create a logical address that is 
* off-board, beyond the system partition. It is up to the user to avoid
* creating overlapping partitions or otherwise contaminating the partition.
*/

#include "vxWorks.h"
#include "vmexLib.h"
#include "usrLib.h"
#include "bufLib.h"
#include "intLib.h"
#include "private/memPartLibP.h"

/* Global variables */

IMPORT VMEX_TABLE_NODE vmexPartArray[];
IMPORT int vmexMaxPart;

/* LOCAL variables  */

LOCAL BOOL initialized = FALSE;
LOCAL VMEX_TABLE *pPartTbl;		/* partition local name table */
LOCAL char partTableName[] = "Fixed Buffer Partitions";

typedef struct
    {
    UINT32 flags;
    UINT32 numFreeBufs;
    BOOL allocated;
    } BUF_AUX;

#define FREE_MAGIC      0x753a012b

typedef struct
    {
    char *      next;           /* next free buffer */
    int         type;           /* FREE_MAGIC or ALLOC_MAGIC */
    } BUFFER;

/*****************************************************************************
* 
* pt_create - create a fixed-block memory partition
*
* This function converts a contiguous area of memory into a partition
* from which fixed blocks may be allocated and deallocated at interrupt
* level.
*
* RETURNS: 
* EOK if successful.
* ERR_BUFSIZE if the buffer size <bsize> is too small or not a power of 2.
* ERR_PTADDR if the partition address is not a multiple of 8.
* ERR_TINYPT if the <length> is smaller then <bsize>.
* ERR_OBJTFULL if the local object table for memory partitions is full
* ERROR if out of memory.
*/

UINT32 pt_create 
    (
    NAME_T(name),         	/* Partition name */
    void *paddr,		/* physical address in memory */
    void *laddr,		/* logical address in memory */
    UINT32 length,		/* length of partition */
    UINT32 bsize,		/* buffer size */
    UINT32 flags,		/* addressibility and deletion flags */
    UINT32 *ptid,		/* partition id */
    UINT32 *nbuf		/* Number of buffers in the partition */
    )
    {
    UINT32 i;
    void * partAddr;
    BOOL allocated = FALSE;
    BUF_POOL * pBufPool;
    BUF_AUX  * pBufAux;

    /* Check buffer size */

    if (bsize < 4)   			/* VMEexec requirement */
	return (ERR_BUFSIZE);

    if (bsize < 8) 			/* vxWorks requirement */
	return (ERR_BUFSIZE);

    i = bsize;				/* bsize must be power of 2 */
    while ( !(i & 0x1) )             		/* shift till LSB set */
	i >>= 1;
    if ( i != 1 )				/* must be only bit set */
	return (ERR_BUFSIZE);

    /* Check partition address */

    if (paddr != NULL && laddr != NULL )
	{
	if ( paddr != laddr )
	    return (ERROR);
        else
	    partAddr = paddr;
        }
    else if (paddr == NULL && laddr == NULL)
	{
	if ( memDefaultAlignment < 8 )
	    partAddr = memalign(8,length);
        else
	    partAddr = malloc (length);

	if ( partAddr == NULL)
	    return (ERROR);
        else
	    allocated = TRUE;
        }
    else if (paddr != NULL)
	partAddr = paddr;
    else
	partAddr = laddr;

    if ( ( (UINT32) partAddr % 8) != 0 )
	return (ERR_PTADDR);

    /* Compute the number of buffers */

    *nbuf = length / bsize;

    if (*nbuf == 0 )
	return (ERR_TINYPT);

    /* Allocate a buffer pool descriptor and auxiliary info struct */

    if ((pBufPool = (BUF_POOL *) malloc (sizeof (BUF_POOL))) == NULL)
	{
	if (allocated)
	    free (partAddr);
	return (ERROR);
	}
    if ((pBufAux = (BUF_AUX *) malloc (sizeof (BUF_AUX))) == NULL)
	{
	if (allocated)
	    free (partAddr);
        free (pBufPool);
	return (ERROR);
	}

    /* Initialize the buffer pool */

    bufPoolInit (pBufPool, partAddr, *nbuf, bsize);

    /* Install the partition name and identifier in the local object table */

    pBufAux->allocated = allocated;
    pBufAux->flags = flags;
    pBufAux->numFreeBufs = *nbuf;

    if (!initialized)
	{
        pPartTbl = vmexTblCreate (partTableName, vmexPartArray, vmexMaxPart); 
	initialized = TRUE;
	}

    if ( vmexTblAdd (pPartTbl, NAME_TO_INT(name), (void *) pBufPool, 
        (void *) pBufAux) == ERROR) 
	{
	if (allocated)
	    free (partAddr);
        free (pBufPool);
        free (pBufAux);
	return (ERR_OBJTFULL);
	}

    *ptid = (UINT32) pBufPool;
    return (EOK);
    }

/*****************************************************************************
* 
* pt_delete - delete a fixed-block memory partition
*
* This function converts a contiguous area of memory into a partition
* from which fixed blocks may be allocated and deallocated at interrupt
* level.
*
* RETURNS: 
* EOK if successful.
* ERR_OBJID if the partition id is incorrect.
* ERR_OBJDEL if the partition id already removed from table.
* ERR_BUFINUSE if there are any buffers in use and the partition was
*    created with the PT_NODEL option.
* ERROR if some other error.
*/

UINT32 pt_delete 
    (
    UINT32 ptid		/* partition id  */
    )
    {
    BUF_AUX * 	pAux;
    BUF_POOL * 	pBufPool = (BUF_POOL *) ptid;
    char * 	pBufs;
    int 	intKey;

    /* Check local object table 
     * Need to lock interrupts so deletion isn't interrupted
     * by a call to pt_getbuf or pt_retbuf from interrupt level
     */

    intKey = intLock();			/* LOCK INTERRUPTS */

    if ((pAux = vmexObjToShadowInt (pPartTbl, (void *) ptid)) == (void *) ERROR)
	{
	intUnlock(intKey);		/* UNLOCK INTERRUPTS */
	return (ERR_OBJID);
	}


    if (!(pAux->flags & PT_DEL) )
        if (pAux->numFreeBufs != pBufPool->numBufs)
	    {
	    intUnlock (intKey);		/* UNLOCK INTERRUPTS */
	    return (ERR_BUFINUSE);
	    }

    pBufs = pBufPool->pBufs;

    /* Remove the partition from the table */
     
    if ( vmexTblRemove (pPartTbl, (void *) ptid) == ERROR )
	{
	intUnlock(intKey);		/* UNLOCK INTERRUPTS */
	return (ERR_OBJDEL);
	}

    intUnlock (intKey);			/* UNLOCK INTERRUPTS */

    if (pAux->allocated)
	free (pBufPool->pBufs);

    free (pAux);
    free (pBufPool);

    return (EOK);
    }

/*****************************************************************************
* 
* pt_getbuf - get a buffer from a fixed-block memory partition
*
* RETURNS: 
* EOK if successful.
* ERR_OBJID if the partition id is incorrect.
* ERR_NOBUF if all buffers are in use.
* ERROR if some other error.
*/
UINT32 pt_getbuf 
    (
    UINT32 ptid,		/* partition id  */
    void ** bufaddr
    )
    {
    int intKey;
    BUF_AUX * pAux;

    intKey = intLock();			/* LOCK INTERRUPTS */

    if ((pAux = vmexObjToShadowInt (pPartTbl, (void *) ptid)) == (void *) ERROR)
	{
	intUnlock (intKey);		/* UNLOCK INTERRUPTS */
	return (ERR_OBJID);
	}
 
    if ( pAux->numFreeBufs == 0)
	{
	intUnlock (intKey);		/* UNLOCK INTERRUPTS */
	return (ERR_NOBUF);
	}

    if ((*bufaddr = bufAlloc ((BUF_POOL *) ptid)) == NULL)
	{
	intUnlock (intKey);		/* UNLOCK INTERRUPTS */
	return (ERR_NOBUF);
	}

    pAux->numFreeBufs--;

    intUnlock (intKey);			/* UNLOCK INTERRUPTS */

    return (EOK);
    }


/*****************************************************************************
* 
* pt_ident - given the name of a fixed-block memory partition
*            return its numeric identifier.
*
* RETURNS: 
* EOK if successful.
* ERR_NODENO if <node> not equal 0.
* ERR_OBJNF if named partition not found.
*/
UINT32 pt_ident 
    (
    NAME_T(name),
    UINT32 node,		/* node selector */
    UINT32 *ptid		/* partition identifier (returned) */
    )
    {
    if (node != 0)
	return (ERR_NODENO);

    if (!initialized)
	return (ERR_OBJNF);

    /* Get the partition ID from the local partition object table */
           
    *ptid = (UINT32) vmexNameToObj (pPartTbl, NAME_TO_INT(name));

    /* return the proper error code */

    if(*ptid == ERROR) 
	return (ERR_OBJNF);

    return (EOK);
    }

/*****************************************************************************
* 
* pt_retbuf - free a buffer from a fixed-block memory partition
*
* RETURNS: 
* EOK if successful.
* ERR_OBJID if <ptid> not not valid.
* ERR_BUFFREE if <bufaddr> unallocated.
* ERR_BUFADDR if <bufaddr> is not in the memory partition or not
*             on a buffer size boundary.
*/
UINT32 pt_retbuf 
    (
    UINT32 ptid,			/* partition identifier */
    void * bufaddr
    )
    {
    int 	intKey;
    BUF_POOL * 	pBufPool;
    BUF_AUX * 	pAux;

    intKey = intLock();			/* LOCK INTERRUPTS */

    if ((pAux = vmexObjToShadowInt (pPartTbl, (void *) ptid)) == (void *) ERROR)
	{
	intUnlock (intKey);		/* UNLOCK INTERRUPTS */
	return (ERR_OBJID);
	}

    pBufPool = (BUF_POOL *) ptid;

    if (pBufPool->pFreeBufs == bufaddr)
	{
	intUnlock(intKey);  		/* UNLOCK INTERRUPTS */
	return (ERR_BUFFREE);
	}

    bufFree (pBufPool, bufaddr);

    if (pBufPool->pFreeBufs != bufaddr)
	{
	if (((BUFFER *) bufaddr)->type == FREE_MAGIC)
	    {
	    intUnlock(intKey);  	/* UNLOCK INTERRUPTS */
	    return (ERR_BUFFREE);
	    }
        else
	    {
	    intUnlock(intKey);  	/* UNLOCK INTERRUPTS */
	    return (ERR_BUFADDR);
	    }
        }
    else
	pAux->numFreeBufs++;

    intUnlock(intKey);  		/* UNLOCK INTERRUPTS */
    return (EOK);
    }

/*****************************************************************************
* 
* pt_sgetbuf - get a buffer from a fixed-block memory partition, returning
*              both the logical and physical addresses.
*
* Note:  This routine always returns the same value for logical and
*        physical addresses.
*
* RETURNS: 
* EOK if successful.
* ERR_OBJID if the partition id is incorrect.
* ERR_NOBUF if all buffers are in use.
* ERROR if some other error.
*/
UINT32 pt_sgetbuf 
    (
    UINT32 ptid,		/* partition id  */
    void ** paddr,		/* physical address */
    void ** laddr		/* logical address */
    )
    {
    UINT32 retval;

    if ( (retval = pt_getbuf (ptid, paddr)) == EOK )
	*laddr = *paddr;
    return (retval);
    }
