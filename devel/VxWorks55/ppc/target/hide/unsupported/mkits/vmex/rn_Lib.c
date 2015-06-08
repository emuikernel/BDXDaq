/* rn_Lib.c -  Variable region memory allocation routines */

/* Copyright 1984-1996 Wind River Systems, Inc. */
#include "copyright_wrs.h"

/*
modification history
--------------------
01e,16oct96,jmb  handle ident call before object table initialized
01d,16oct96,jmb  forgot to add auxiliary info struct to object table
01c,14jun96,jmb  comment clean-up
01b,10may96,jmb  added strict-mode global variable.
01a,30apr96,jmb  created
*/

/*****************************************************************************
*
* rn_Lib -  variable-segment memory region routines
*
* These routines are used to manage memory regions containing
* variable-size segments.  These routines are built on top of vxWorks'
* memPartLib.  These routines may be used as in VMEexec(r), but the
* options are ignored.  That's because in vxWorks
*
*  -  cannot wait for an allocation.
*  -  the unit size is a fixed allocation alignment quantum, which depends
*         on the CPU type.
*  -  regions cannot be deleted in vxWorks.
*/

#include "vxWorks.h"
#include "vmexLib.h"
#include "usrLib.h"
#include "memLib.h"
#include "private/memPartLibP.h"

/* Global variables */

IMPORT VMEX_TABLE_NODE vmexRgnArray[];
IMPORT int vmexMaxRgn;
IMPORT BOOL vmexStrictMode;

/* LOCAL variables  */

LOCAL BOOL initialized;
LOCAL VMEX_TABLE *pRgnTbl;		/* regions local name table */
LOCAL char rgnTableName[] = "Memory Regions";

typedef struct
    {
    UINT32 flags;
    UINT32 maxbuf;
    } RGN_AUX;

/*****************************************************************************
* 
* rn_create - create a variable-segment memory region
*
* This function converts a contiguous area of memory into a region
* from which variable-sized segments may be allocated and deallocated
*
* Note:  Although the return value <asiz> is the number of allocatable
*        bytes in the region, this number is correct only if the entire
*        region is allocated with a single call to rn_getseg().  That's
*	 because each segment has a header which is taken from the allocatable
*        area, so the greater the number of segments, the fewer bytes
*        of <asiz> are actually available to the user.
*
* RETURNS: 
* EOK if successful.
* ERR_TINYRN if <length> is a segment header or less.
* ERR_RNADDR if the region address is not a multiple of 8.
* ERR_OBJTFULL if the local object table for memory regions is full
* ERROR if out of memory.
*/

UINT32 rn_create 
    (
    NAME_T(name),         	/* Four character region name */
    void * addr,		/* starting memory address */
    UINT32 length,		/* length of region in bytes*/
    UINT32 unit_size,		/* unit of allocation in bytes */
    UINT32 flags,		/* queueing and deletion flags */
    UINT32 *rnid,		/* region id */
    UINT32 *asiz		/* Number of allocatable bytes in region */
    )
    {
    RGN_AUX * pRgnAux;
    PART_ID partId;		/* vxWorks partition identifier */

    /* Ignoring the following args:   
     *   <unit_size>
     */

    /* Error if region not big enough for at least 1 byte to be allocated */

     if ( length <= 1 + sizeof(BLOCK_HDR))
	 return (ERR_TINYRN);

    /* Check region address
     * vxWorks will round it up if necessary...
     */

    if (vmexStrictMode)
        if ( ( (UINT32) addr % 8) != 0 )   
	    return (ERR_PTADDR);

    if ((partId = memPartCreate (addr, length)) == NULL)
	return(ERROR);

    if ((pRgnAux = (RGN_AUX *) malloc (sizeof (RGN_AUX))) == NULL)
	return (ERROR);

    pRgnAux->flags = flags;
    pRgnAux->maxbuf = memPartFindMax(partId);

    if (!initialized)
	{
        pRgnTbl = vmexTblCreate (rgnTableName, vmexRgnArray, vmexMaxRgn); 
	initialized = TRUE;
	}

    if ( vmexTblAdd (pRgnTbl, NAME_TO_INT(name), (void *) partId, 
        (void *) pRgnAux) == ERROR) 
	{
	free (pRgnAux);
	return (ERR_OBJTFULL);
	}

    *rnid = (UINT32) partId;
    *asiz = pRgnAux->maxbuf - sizeof(BLOCK_HDR);

    return (EOK);
    }

/*****************************************************************************
* 
* rn_delete - delete a variable-segment memory region
*
* This function deletes the memory descriptor structures for a particular
* region.
*
* Note: there is no delete function for vxWorks memory partitions,
* so this function only cleans up the local object table and delete
* the auxiliary information structure.
*
* RETURNS: 
* EOK if successful.
* ERR_OBJID if the region id is incorrect.
* ERR_SEGINUSE if there are any segments in use and the region was
*    created with the PT_NODEL option.
* ERR_OBJDEL if the region is deleted by another process.
* ERROR if some other error.
*/

UINT32 rn_delete 
    (
    UINT32 rnid		/* region id  */
    )
    {
    RGN_AUX * 	pAux;			/* ptr to addition region info */
    PART_ID partId = (PART_ID) rnid;	/* vxWorks partition id */

    /* Check local object table */

    if ( rnid == 0 )           /* Can't delete region 0 */
	return (ERR_OBJID);

    if ((pAux = vmexObjToShadow (pRgnTbl, (void *) rnid)) == (void *) ERROR)
	return (ERR_OBJID);

    if (!(pAux->flags & RN_DEL) )
        if (pAux->maxbuf != memPartFindMax (partId) )
            return (ERR_SEGINUSE);

    /* Remove the region id from the table */
     
    if ( vmexTblRemove (pRgnTbl, (void *) rnid) == ERROR )
	return (ERR_OBJDEL);

    free (pAux);			/* Free auxiliary info struct */

    return (EOK);
    }

/*****************************************************************************
* 
* rn_getseg - get a segment from a variable-block memory region
*
* RETURNS: 
* EOK if successful.
* ERR_OBJID if the region id is incorrect.
* ERR_OBJDEL if the region is deleted before allocation complete.
* ERR_NOSEG if all segment are in use.
* ERR_ZERO if requested <size> is zero.
* ERR_TOOBIG if <size> too big for region.
* ERROR if some other error.
*/
UINT32 rn_getseg 
    (
    UINT32 rnid,		/* region id  */
    UINT32 size,		/* length of segment in bytes */
    UINT32 flags,		/* wait flag */
    UINT32 timeout,		/* wait timeout */
    void ** segaddr  		/* returned segment address */
    )
    {
    void * pAddr;
    MEM_PART_STATS stats;
    RGN_AUX * pAux;
    PART_ID partId = (PART_ID) rnid;

    /*
     *  Check args <rnid> and <size>.  
     *  Ignore args <flags> and <timeout>.  
     */

    if (rnid == 0 )		/* vxWorks system partition not in table */
	partId = memSysPartId;
    else
        if ((pAux = vmexObjToShadowInt (pRgnTbl, (void *) rnid)) == 
	    (void *) ERROR)
	    return (ERR_OBJID);

    if (vmexStrictMode)
        if (size == 0)
	    return (ERR_ZERO);

    /* Allocate memory */

    if ((pAddr = memPartAlloc (partId, size)) == NULL ) 
	{
	if ( memPartInfoGet (partId, &stats) == ERROR )
	    return (ERR_OBJDEL);
        else
	    {
	    if ( stats.numBytesFree == 0 )
		return (ERR_NOSEG);
            else
		return (ERR_TOOBIG);
            }
        }

    *segaddr = pAddr;
    return (EOK);
    }


/*****************************************************************************
* 
* rn_ident - given the name of a variable-block memory region
*            return its numeric identifier.
*
* RETURNS: 
* EOK if successful.
* ERR_OBJNF if named region not found.
*/
UINT32 rn_ident 
    (
    NAME_T(name),		/* 4-character name of region */
    UINT32 *rnid		/* region identifier (returned) */
    )
    {

    /* Get the region ID from the local region object table */
           
    if (!initialized)
	return (ERR_OBJNF);

    *rnid = (UINT32) vmexNameToObj (pRgnTbl, NAME_TO_INT(name));

    /* return the proper error code */

    if(*rnid == ERROR) 
	return (ERR_OBJNF);

    return (EOK);
    }

/*****************************************************************************
* 
* rn_retseg - free a segment from a variable-block memory region
*
* Note:  The vxWorks partition manager only returns the errno,
*        S_memLib_BLOCK_ERROR, so most of the VMEexec error codes
*        that describe why the retseg() failed are not implemented.
*
* RETURNS: 
* EOK if successful.
* ERR_OBJID if <rnid> not not valid.
* ERR_OBJDEL if region is delete while attempting to return a segment.
* ERROR for any other failure.
*/
UINT32 rn_retseg 
    (
    UINT32 rnid,			/* region identifier */
    void * segaddr			/* segment address */
    )
    {
    PART_ID partId = 	(PART_ID) rnid;	/* convert to vxWorks partition id */
    RGN_AUX * 		pAux;

    if ((pAux = vmexObjToShadow (pRgnTbl, (void *) rnid)) == (void *) ERROR)
	return (ERR_OBJID);

    if ( memPartFree (partId, segaddr) == ERROR )
	{
	if (errno == S_objLib_OBJ_ID_ERROR)
	    return (ERR_OBJDEL);
        else
	    return (ERROR);
        }

    return (EOK);
    }
