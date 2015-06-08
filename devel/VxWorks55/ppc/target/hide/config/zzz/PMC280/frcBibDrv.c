
/* frcBibDrv.c - Board Information Block Driver */

/*
################################################################################
#
#  Project:	The Board Information Block (BIB)
#
#  Portability Status:
#		FULLY PORTABLE
#
################################################################################
#
#		Test Development Group
#		MOTOROLA GmbH
#		Embedded Communications Computing
#		Lilienthalstr. 15
#	D-85579	Neubiberg/Muenchen
#		Germany
#
#	Remarks: Note! This material is proprietary of MOTOROLA GmbH and may
#	only be viewed under the terms of a signed non-disclosure agreement,
#	or a source code license agreement!
#	The information in this document is subject to change without notice
#	and should not be construed as a commitment by MOTOROLA.
#	Neither MOTOROLA, nor the authors assume any responsibility for
#	the use or reliability of this document or the described software.
#
#	Copyright (C) 1999-2004, MOTOROLA  GmbH.
#	Copyright (C) 2004-2006, MOTOROLA GmbH. All rights reserved.
#
################################################################################

modification history:
=====================
02a,17may06,rgr	 Reworked driver to support BIB 2.1.  This uses a different
		 directory structure, i.e. a linked list which allows to store
		 the same type of BIB element more than once (e.g. VPD data).
01q,11aug04,rgr	 Be sure to clear <errno> if BIB was written properly.  This is
		 necessary, because an underlying routine may have set <errno>.
01p,21jun04,fxs	 Removed calls to strerror_r to avoid link problems when 
		 statSymTbl is missing.
01o,16jun04,rgr	 Data is now verified by frcBibWrite() after writing.  In case
		 of an error, the routine tries to recover the data, i.e. the
		 complete BIB image is written from the internal buffer to the
		 physical memory device.
		 Added bibVerify() and frcBibErrnoGet(), updated documentation.
01n,12mar03,fxs	 Moved old driver API to frcBibDrvCompat.c, except
		 bibRead and bibWrite to prevent link problems for applications
		 that used these routines,
02m,04jun02,rgr  Moved inclusion of header file <stdio.h> to "frcBibDrv.h".
02l,27may02,rgr  Implemented new API, all routines begin with frcBibXxx now.
		 Added frcBibDrvShow() routine.
02k,17may02,rgr  Corrected address calculation to the next BIB directory.
		 Removed DEFS_PATH, because it cannot be handled by the
		 Microsoft compiler.  Use option "-I<dir>" instead.
		 Use macro IMPORT_FRC_MOD_NUM_BASE to import or define the
		 global variable <frcModNumBase>, removed PowerBoot define.
02j,16oct01,akn  Redeclared include statement of 'defs.h' to avoid linkage
		 problems
		 Added a PowerBoot define to declare a variable in a PowerBoot-
		 style
02i,05jan01,fxs  Declared frcModNumBase to avoid linkage problems.
02h,08may00,rage Ported to MS-DOS, fixed type casts.
02g,01mar00,rage Changed documentation format for "refgen" tool.
02f,15feb00,fxs	 Implemented callout routines for memory allocation
02e,05jan00,rage Function bibAttach(): Clear error number if handle was
		 not found, which is correct in this case.
02d,09mar99,rage Corrected function bibWrite(): The last byte of a buffer
		 was not written to the image.
02c,05mar99,rage Added some type casts to satisfy the Microsoft compiler.
		 Removed support for raw BIB, because it was not
		 applicable in this way.
		 Check also for 0x00 as the end of block list identifier.
		 Implemented maximum range for block offset.
01c,02feb99,rage Corrected typos, disabled debugging.
		 Divided define RAW_BIB into RAW_BIB_READ and
		 RAW_BIB_WRITE to be able to use the driver for the
		 "upd_bib" tool, which reads a Raw BIB and writes a
		 final BIB.
01b,25jan99,rage Added setting of error codes, bibRdWr() and bibWrite()
		 routine.  If RAW_BIB is defined, the checksum for a
		 "raw BIB" is expected, respective calculated.
		 Corrected function bibRead().
01a,20jan99,rage Initial version, supports BIB version 2.0.

DESCRIPTION:
This is a generic driver which provides access to all elements of a Board
Information Block (BIB).  The Board Information Block is a data structure,
used to store hardware related information in a unique and compact
way.  The Board Information Block is stored in a non-volatile memory,
usually a serial I2C-bus EEPROM, called ID-ROM, which is on the board.
For a complete description of the BIB, see
'The Board Information Block (BIB) Version 2.1',
http://compass.mot.com/doc/197464052/\nl
\nl

INITIALIZATION
Before data can be read from or written to a BIB, the ID-ROM device must
be attached by calling frcBibAttach().  This routine expects a <handle>
and a structure which specifies the interface to the device:
\its
\sh BIB_INTF
\ite
\ss
typedef struct
{
    /@ interface routines to access this BIB device @/
    STATUS (*'bibReadIntfRtn') ('BIB_HDL' 'handle',  UINT32 'offset',
			      UINT32  'byteCnt', void  *'pDstBuf');
    STATUS (*'bibWriteIntfRtn')('BIB_HDL' 'handle',  UINT32 'offset',
			      UINT32  'byteCnt', void  *'pSrcBuf');

    /@ optional local memory pool for this BIB @/
    void    *'pMemPool';	/@ start address of local memory pool	@/
    size_t   'memPoolSize';	/@ pool size in bytes			@/
} BIB_INTF;
\se

The elements of the structure in detail:
\is
\i 'bibReadIntfRtn'
This function pointer specifies the BIB read interface routine for the
respective BIB device.  For a description of the function parameters see
'Arguments of the Interface Routines'.

\i 'bibWriteIntfRtn'
This function pointer specifies the BIB write interface routine for the
respective BIB device.  For a description of the function parameters see
'Arguments of the Interface Routines'.

\i 'pMemPool'
To specify a local memory pool for this BIB handle, set <pMemPool> to the
start address of the pool.  Element 'memPoolSize' must also be set.  If
the memory allocation routines 'bibMallocRtn'() and 'bibFreeRtn'() should
be used instead of a local memory pool, set <pMemPool> to 'NULL'.
For default, these routines refer to malloc(), resp. free().

\i 'memPoolSize'
If 'pMemPool' is used to define a local memory pool, this element must
contain the pool size in bytes.  If 'pMemPool' is 'NULL', it is not used.
The amount of memory which is required depends on the size of the BIB
to be attached.  In a typical system with a large ID-ROM, the maximum
amount of memory that needs to be provided is about 512 bytes.
\ie
\nl

\its
\sh Arguments of the Interface Routines
\ite
The BIB interface routines 'bibReadIntfRtn'() and 'bibWriteIntfRtn'() use
the following arguments:
\is
\i 'handle'
The BIB device handle specifies the memory device to access.  This is usually
an I2C-bus EEPROM (ID-ROM).  The handle is not defined by the driver, but
directly passed from the application to the respective interface routine.
If an individual set of interface routines has been defined for each BIB
device, this parameter may be discarded.

\i 'offset'
Byte offset within the memory device where to start reading or writing.

\i 'byteCnt'
Byte count, number of bytes to read or write.

\i 'pDstBuf'
Destination buffer, specifies where to store the data which was read.

\i 'pSrcBuf'
Source buffer, specifies where to get the write data from.
\ie
The interface routines return a status code.  It is OK if the operation was
successful, or ERROR if an invalid parameter was passed to the routine, e.g.
an 'offset' value which is out of range, or if the read/write operation failed.

READING DATA FROM A BIB
After frcBibAttach() has been called to attach a handle to a BIB device,
data can be read from the respective BIB via frcBibElemRead().
Data is exchanged as a complete BIB element structure, it is not possible
to read or write single bytes of a BIB image.  The first byte of a BIB
structure always contains the size of the structure minus one, i.e. the
number of bytes following.

WRITING DATA TO A BIB
Normally this driver is only used to read data from a BIB.  Nevertheless
the frcBibElemWrite() routine provides the ability to write element structures
within an already existing BIB image for update purposes.  Be very careful
with this feature!

CUSTOMIZING THE BIB DRIVER
There are several macros and global variables which can be changed to
customize this driver.

\is
\i 'IMPORT_ERRNO'
This macro is used to import the error number variable.  Most operating
systems use the global variable <errno>, therefore the macro is defined to:
\cs
    #define IMPORT_ERRNO      extern int errno
\ce

\i 'SET_ERRNO'
Macro to set an error code.  The error codes of this driver are described in
section 'ERROR CODES'.  Usually the global variable <errno> is set, therefore
the macro is defined to:
\cs
    #define SET_ERRNO(x)      errno = (x)
\ce

\i 'GET_ERRNO'
This macro returns the current error code.  Usually the global variable
<errno> holds the current error code, therefore the macro is defined to:
\cs
    #define GET_ERRNO         errno
\ce

\i 'IMPORT_FRC_MOD_NUM_BASE'
The global variable <frcModNumBase> is used to define the base value of
all MOTOROLA specific error numbers.  This variable is usually defined
in several modules, therefore the macro defines the variable:
\cs
    #define IMPORT_FRC_MOD_NUM_BASE     int frcModNumBase
\ce
However, some linkers have a problem with multiple defined (common) variables.
In this case the macro can be set to a real import functionality:
\cs
    #define IMPORT_FRC_MOD_NUM_BASE     extern int frcModNumBase
\ce

\i 'BIB_HDL'
This macro specifies the data type of all BIB handles.  This implementation
uses type 'UINT32' for default.  It is also possible to use another data type,
e.g. a pointer to a structure.

\i 'BIB_DRV_MALLOC'
This macro specifies the default memory allocation routine, which is usally
malloc().  See also the global function pointer 'bibMallocRtn'.

\i 'BIB_DRV_FREE'
This macro specifies the default memory free routine, which is usally
free().  See also the global function pointer 'bibFreeRtn'.

\i 'bibMallocRtn'
This function pointer refers to the memory allocation routine used by this
driver.  Initially this pointer is set to the define 'BIB_DRV_MALLOC', which
is usually set to the malloc() routine.  The pointer can be modified to
refer to another function, if the operating system does not support this
routine, or it can not be used, e.g. in an early stage of booting.
However it is also possible to specify a local memory pool for a BIB handle,
see element 'pMemPool' in data structure 'BIB_INTF'.\nl
The prototype of a memory allocation routine is:
\cs
    void   *yourMalloc (size_t size);
\ce
See also function pointer 'bibFreeRtn'.

\i 'bibFreeRtn'
This function pointer refers to the memory free routine used by this driver.
Initially this pointer is set to the define 'BIB_DRV_FREE', which is usually
set to the free() routine.  The pointer can be modified to refer to another
function, see also 'bibMallocRtn'.\nl
The prototype of a memory allocation routine is:
\cs
    void    yourFree (void *ptr);
\ce

\i 'frcBibShowRtn'
When frcBibDrvShow() is called with an information level above 1, and this
function pointer has previously been set to a BIB show routine, this will
be called.\nl
The prototype of a BIB show routine is:
\cs
    STATUS    yourBibShow (void *pImage, size_t imgSize,
                           BOOL verbose, FILE  *output);
\ce
Parameter <pImage> is the address of the BIB image which should be displayed.
It is set to the driver\'s internal cache buffer.  The <imgSize> parameter
contains the number of bytes of the BIB image.  The <verbose> flag is set
TRUE if frcBibDrvShow() is called with an <infoLvl> greater than 2, otherwise
it is FALSE.  The <output> is not used here, it is set to 'NULL' to specify
"standard out".

Usually the application sets this function pointer to the frcBibShow() routine,
which has to be included by the application.
\cs
    frcBibShowRtn = frcBibShow;
\ce
\ie

ERROR CODES
When the driver returns ERROR, the cause of the error can be determined by
reading the error code from the error number variable, usually <errno>.  If the
error occured within the driver, the code is one of the following, described
below.  If a BIB interface routine reports an error, the error variable is
usually set by an underlying module and is not changed by the BIB driver.

The following table shows the error codes used by this driver.  The
corresponding defines begin with S_frcBibDrv_XXX and can be found in
the header file "'frcBibDrv.h'".

\ts
Error code define | Description
------------------|------------------------------------------------------------
INVALID_PARAMETER   | An invalid function argument was passed to the driver.
ALREADY_ATTACHED    | The handle of the specified BIB is already attached.
UNKNOWN_HANDLE      | This handle is currently unknown, first call \
frcBibAttach().
NO_BIB_FOUND        | There exists no BIB image in the specified memory device.
BIB_INVALID         | The BIB image has no valid identifier.
INVALID_DIR         | A BIB directory is not located within the BIB image.
INVALID_DIR_ENTRY   | A directory entry tells an offset value, which is \
located outside of the BIB image.
CHKSUM_ERROR        | The checksum of the BIB image is not correct.
ELEMENT_NOT_FOUND   | The requested element could not be found in the BIB.
BUFFER_TOO_SMALL    | The specified buffer is too small.
ELEMENT_TOO_LARGE   | The BIB element structure that should be written is \
larger than the size provided by the BIB in the memory device.
NOT_ENOUGH_MEMORY   | This error code is set whenever the driver cannot \
allocate enough memory from its local pool 'pMemPool', or via the \
bibMallocRtn(), resp. malloc().
READ_INTF_ERROR     | If the 'bibReadIntfRtn'() returns ERROR, but does not \
set any error code, this code is used by the driver to signal that a read \
operation has failed.  Normally the underlying layer, e.g. the 'frcI2cBus' \
library sets an error code.
WRITE_INTF_ERROR    | If the 'bibWriteIntfRtn'() returns ERROR, but does not \
set any error code, this code is used by the driver to signal that a write \
operation has failed.  Normally the underlying layer, e.g. the 'frcI2cBus' \
library sets an error code.
DATA_VERIFY_ERROR   | If a BIB element was written, the complete BIB image \
is verified.  If the data in the memory device is not identical to the \
internal buffer, a verify error is set and a data recovery process is \
initiated.
DATA_RECOVERY_ERROR | As already described, a data recovery process is \
initiated after a DATA_VERIFY_ERROR, i.e. the complete BIB image is written \
from the internal buffer to the memory device.  If this process also fails, \
a DATA_RECOVERY_ERROR is set.
\te

BACKWARD COMPATIBILITY
The API of this driver has been changed with version 02l.  To guarantee
backward compatibility, a new module frcBibDrvCompat.c is available which 
provides the old routines.

'<However, it is strictly recommended only to use the new driver API !!>'

SEE ALSO
File frcBibDrv.h, and 'The Board Information Block (BIB) Version 2.1',
http://compass.mot.com/doc/197464052/\nl
*/


#ifdef	CPU				/* is defined by the VxWorks Makefile */
#include "vxWorks.h"
#include "frcModNum.h"		/* module number for error codes */
#include "frcBib.h"	/* BIB structure and elements */
#include "frcBibDrv.h"	/* header file for this driver */
#else
#include "defs.h"			/* common definitions */
#include "frcBib.h"			/* BIB struture and elements */
#include "frcBibDrv.h"			/* header file for this driver */
#endif

#include "stdlib.h"
#include "string.h"
#include "ctype.h"

/*=============================== DEFINES ==============================*/

#define	DEBUG
#undef 	DEBUG			/* deactivate debugging */

#define	BIB_MAX_OFFSET		(32L * 1024L)	/* 32K is the limit	*/

/*=============================== TYPEDEFS =============================*/
typedef struct hdlDesc
    {
	struct hdlDesc	*pNextDesc;	/* ptr to the next descriptor	*/
	BIB_HDL	 hdl;		/* external handle for this descriptor	*/
	UINT32	 devOffs;	/* byte offset within (EEPROM) device	*/
	UINT32	 bibLength;	/* length of the BIB image in bytes	*/
	BIB_V20	*pBIB;		/* pointer to BIB in memory (cache)	*/
	int	 prevErrno;	/* previous error number		*/

	int	 origErrno;	/* original cause of error		*/

	BIB_INTF bibIntf;	/* BIB interface structure		*/
    } HDL_DESC;			/* handle descriptor */

/*=============================== IMPORTS ==============================*/

IMPORT_ERRNO;			/* import the error number variable */
IMPORT_FRC_MOD_NUM_BASE;	/* import/define variable frcModNumBase */
IMPORT STATUS	frcBibShow
    (
    void	*pImage,	/* pointer to BIB image			*/
    size_t	 imgSize,	/* image size in bytes			*/
    BOOL	 verbose,	/* TRUE enables verbose display mode	*/
    FILE	*output		/* output stream - NULL = stdout	*/
    );
/*=============================== GLOBALS ==============================*/

GLOBAL	void	*(*bibMallocRtn)(size_t size) = NULL;
GLOBAL	void	(*bibFreeRtn)	(void *ptr) = NULL;
GLOBAL	STATUS	(*frcBibShowRtn)(void *pImage, size_t imgSize,
				 BOOL verbose, FILE  *output) = NULL;

/*=============================== LOCALS ===============================*/
LOCAL STATUS	 bibRdWr	(BIB_HDL handle, int element, int index,
				 void   *pBuf,   UINT32 bufSize,
				 int     writeAccess);
LOCAL STATUS	 bibVerify	(HDL_DESC *pDesc);
LOCAL HDL_DESC	*hdlSearch	(BIB_HDL handle);
LOCAL HDL_DESC	*hdlCreate	(BIB_HDL handle, BIB_INTF *pIntf);
LOCAL STATUS	 hdlRemove	(BIB_HDL handle);
LOCAL void	*bibMalloc	(BIB_INTF *pIntf, size_t size);
LOCAL void	 bibFree	(BIB_INTF *pIntf, void  *ptr);

LOCAL HDL_DESC	*pRootDesc = NULL;	/* root pointer of descriptors	*/


/*******************************************************************************
*
* frcBibAttach - attach a BIB device
*
* This routine must be called once to attach a <handle> to a BIB device,
* usually an I2C-bus EEPROM (ID-ROM).  The handle is used as a unique
* identifier for the internal driver structures, and it is passed to the
* interface routines 'bibReadIntfRtn'() and 'bibWriteIntfRtn'(), which are
* specified in the structure 'BIB_INTF', referenced by <pIntf>.\nl
* As described in section "3.1 Data Block Structure" of
* 'The Board Information Block (BIB) Version 2.1' document, more than one
* data block can be stored in the same ID-ROM by building a linked list.
* The driver searches this linked list of data blocks for a valid BIB image,
* verifies its checksum and copies the BIB image into an internal memory
* buffer.
*
* The routine returns ERROR in case of insufficient memory, if the device
* cannot be read, no BIB is found, or the BIB image is not valid.
*
* RETURNS: OK or ERROR.
*/

GLOBAL STATUS	 frcBibAttach
(
    BIB_HDL	 handle,	/* usually specifies an EEPROM		*/
    BIB_INTF	*pIntf		/* ptr to interface structure		*/
)
{
UINT8		 buffer[BIB_HEADER_SIZE];  /* buffer for data block header */
BIB_V20		*pBIB;		/* pointer to BIB basic structure	*/
FAST UINT32	 i;		/* fast counter variable		*/
FAST UINT8	*pData;		/* fast pointer variable		*/
UINT32		 offset;	/* byte offset within the BIB device	*/
UINT32		 length;	/* length of the BIB image in bytes	*/
UINT32		 chkSum;	/* checksum over the BIB image		*/
HDL_DESC	*pDesc;		/* pointer to current descriptor	*/
BIB_INTF	 bibIntf;	/* local BIB interface structure	*/


    /* be sure that the module number base is set */

    if (frcModNumBase == 0)
	frcModNumBase = M_frc;	/* set to default */

    /* Check whether environment provided a special malloc/free */

    if (bibMallocRtn == NULL)
	bibMallocRtn = BIB_DRV_MALLOC;

    if (bibFreeRtn == NULL)
	bibFreeRtn = BIB_DRV_FREE;

    /* first check if a descriptor already exists for <handle> */

    pDesc = hdlSearch (handle);
    if (pDesc != NULL)
    {
	SET_ERRNO (S_frcBibDrv_ALREADY_ATTACHED);
	pDesc->prevErrno = S_frcBibDrv_ALREADY_ATTACHED;
	return (ERROR);				/* BIB already attached */
    }

    /* handle does not exist, which is OK - clear error number */

    SET_ERRNO (OK);

    /* check interface parameters */

    if (pIntf == NULL)
    {
	SET_ERRNO (S_frcBibDrv_INVALID_PARAMETER);
	return (ERROR);
    }

    if (pIntf->bibReadIntfRtn == NULL  ||  pIntf->bibWriteIntfRtn == NULL)
    {
	SET_ERRNO (S_frcBibDrv_INVALID_PARAMETER);
	return (ERROR);
    }

    bibIntf = *pIntf;

    /* search the BIB structure in the specified device, e.g. an EEPROM */

    offset = 0;					/* start at offset 0 */
    pBIB   = (BIB_V20 *) buffer;

    while (1)
    {
	/* read the header of the data block */

	if (bibIntf.bibReadIntfRtn (handle, offset,
				    BIB_HEADER_SIZE, buffer) != OK)
	{
	    /* see if interface routine has set an error code */

	    if (GET_ERRNO == OK)
		SET_ERRNO (S_frcBibDrv_READ_INTF_ERROR);

	    return (ERROR);
	}

	/* check if this block is a BIB stucture */

	if (pBIB->BlockId == BIB20_VERSION	/* BIB 2.0 */
	||  pBIB->BlockId == BIB_VERSION)	/* BIB 2.1 */
	    break;				/* found BIB */

	/* check for end of block list */

	if ((pBIB->BlockId    == 0xff)
	||  (pBIB->LengthCode == 0xff)  ||  (pBIB->LengthCode == 0x00))
	{
	    SET_ERRNO (S_frcBibDrv_NO_BIB_FOUND);
	    return (ERROR);			/* reached end of block list */
	}

	/* data block is no BIB, check the next block */

	offset += (pBIB->LengthCode * BIB_LENGTH_UNIT);

	/* check for maximum <offset> */

	if (offset > BIB_MAX_OFFSET)
	{
	    SET_ERRNO (S_frcBibDrv_NO_BIB_FOUND);
	    return (ERROR);			/* BIB not found in ID-ROM */
	}
    }

    /* seems to be a BIB - check identifier */

    if (pBIB->BlockId == BIB20_VERSION)		/* BIB 2.0 */
    {
	if (strcmp (pBIB->ForceIdentifier, BIB20_FORCE_ID) != 0)
	{
	    SET_ERRNO (S_frcBibDrv_BIB_INVALID);
	    return (ERROR);				/* no valid BIB */
	}
    }
    else					/* BIB 2.1 */
    {
	if (strcmp (((BIB_V21 *)pBIB)->Motorola, BIB_ID_STR) != 0)
	{
	    SET_ERRNO (S_frcBibDrv_BIB_INVALID);
	    return (ERROR);				/* no valid BIB */
	}
    }

    /* identifier is OK - allocate memory for the BIB structure */

    length = pBIB->LengthCode * BIB_LENGTH_UNIT;

    pBIB = bibMalloc (&bibIntf, (size_t) length);

	
    if (pBIB == NULL)
	return (ERROR);				/* not enough memory */

    /* read complete BIB into memory buffer */

    if (bibIntf.bibReadIntfRtn (handle, offset, length, pBIB) != OK)
    {
	/* see if interface routine has set an error code */

	if (GET_ERRNO == OK)
	    SET_ERRNO (S_frcBibDrv_READ_INTF_ERROR);

	bibFree (&bibIntf, pBIB);
	return (ERROR);
    }

    /* verify the checksum of the block */

    pData = (UINT8 *) pBIB;

    for (chkSum = 0, i = 0;  i < length - 2;  i++)
	chkSum += *(pData++);			/* add data byte to checksum */

    if ((chkSum & 0xffff) != (UINT32) BIB_NUM16_GET (pData))
    {
	bibFree (&bibIntf, pBIB);
	SET_ERRNO (S_frcBibDrv_CHKSUM_ERROR);
	return (ERROR);				/* checksum error */
    }

    /* create a new descriptor for the handle */

    if ((pDesc = hdlCreate (handle, &bibIntf)) == NULL)
    {
	bibFree (&bibIntf, pBIB);
	return (ERROR);				/* not enough memory */
    }

    /* fill descriptor with the required information */

    pDesc->devOffs	= offset;
	
    pDesc->bibLength	= length;
		
    pDesc->pBIB		= pBIB;
	

    return (OK);
}

/*******************************************************************************
*
* frcBibDetach - detach a BIB device
*
* This routine removes the specified <handle> from the internal descriptor
* list and frees all related memory.
*
* RETURNS: OK or ERROR.
*/

GLOBAL STATUS	 frcBibDetach
(
    BIB_HDL	 handle		/* usually specifies an EEPROM		*/
)
{
HDL_DESC	*pDesc;		/* pointer to current descriptor	*/


    /* first check if a descriptor exists for <handle> */

    if ((pDesc = hdlSearch (handle)) == NULL)
	return (ERROR);				/* unknown handle */

    /* free BIB memory buffer */

    bibFree (&(pDesc->bibIntf), pDesc->pBIB);

    /* remove descriptor from the list */

    return (hdlRemove (handle));
}

/*******************************************************************************
*
* frcBibElemRead - read an element from BIB
*
* This routine reads the specified <element> from the BIB, which is referred
* to by <handle>, and copies the complete BIB element structure into the
* destination buffer at address <pDstBuf>.  The first byte of the buffer
* always contains the "Size" field, which tells the number of bytes that
* follow.  The routine returns ERROR if the element cannot be found in the
* BIB directory, or the corresponding element structure does not exist in
* the BIB.
*
* The <element> parameter specifies the identifier of the BIB element which
* should be read from the BIB.  For a complete list of all BIB elements, see
* structure BIB_ELEM_NAME in file "'frcBIB.h'".  With the introduction of
* BIB Version 2.1 it is possible to include the same type of element more than
* once, therefore the parameter <index> was added to be able to access the n-th
* element.  Usually <index> is set to 0.
*
* NOTE:
* The frcBibAttach() routine must be called once, before frcBibElemRead() can
* be used.
*
* RETURNS: OK or ERROR.
*/

GLOBAL STATUS	 frcBibElemRead
(
    BIB_HDL	 handle,	/* usually specifies an EEPROM		*/
    int		 element,	/* BIB element to read			*/
    int		 index,		/* element index (0..n), usually 0	*/
    void	*pDstBuf,	/* destination buffer address		*/
    UINT32	 bufSize	/* size of the buffer in bytes		*/
)
{
    return (bibRdWr (handle, element, index, pDstBuf, bufSize, 0));
}

/*******************************************************************************
*
* frcBibElemWrite - write an element to a BIB
*
* This routine writes the specified <element> to the BIB, which is referred
* by <handle>.  It copies the complete BIB element structure from the source
* buffer at address <pSrcBuf> to the BIB.  The first byte of the buffer
* always must contain the <Size> field, which tells the number of bytes that
* follow.  Only elements which already exist in the BIB image can be written,
* i.e. "updated".  This driver cannot create new elements in a BIB image!
* The routine returns ERROR if the element cannot be found in the BIB
* directory, or the corresponding element structure does not exist in
* the BIB.
*
* The <element> parameter specifies the identifier of the BIB element which
* should be written to the BIB.  For a complete list of all BIB elements, see
* structure BIB_ELEM_NAME in file "'frcBIB.h'".  With the introduction of
* BIB Version 2.1 it is possible to include the same type of element more than
* once, therefore the parameter <index> was added to be able to access the n-th
* element.  Usually <index> is set to 0.
*
* After an element has been written to the physical memory device, e.g. an
* EEPROM, the complete BIB image is read back and verified against the internal
* buffer.  If the image does not match, a 'verify error' is set and a data
* recovery procedure is started which writes the complete BIB to the memory
* device again.  If this also fails, a 'data recovery error' is set.  Use
* function frcBibErrnoGet() to get the underlying errors in case of a fault.
* The following table describes the possible error conditions:
*
* \ts
* <errno>        | previous error | original error | Description
* ---------------|----------------|----------------|---------------------------
* <set by driver>|WRITE_INTF_ERROR| <set by driver>| Write failed, but can \
recover data
* VERIFY_ERROR   | VERIFY_ERROR   | <set by driver>| Verify (read) failed, \
but can recover
* VERIFY_ERROR   | VERIFY_ERROR   | VERIFY_ERROR   | Verify (compare) failed, \
can recover
* RECOVERY_ERROR | RECOVERY_ERROR | <set by driver>| Recover (write) failed
* RECOVERY_ERROR | VERIFY_ERROR   | <set by driver>| Recover (verify read) \
failed
* RECOVERY_ERROR | VERIFY_ERROR   | VERIFY_ERROR   | Recover (verify compare) \
failed
* \te
*
* The term <set by driver> means that the error code is set by the underlying
* driver which is called via the 'bibReadIntfRtn'() and 'bibWriteIntfRtn'()
* routines, e.g. by the 'frcI2cBus' library.
*
* NOTE:
* The frcBibAttach() routine must be called once, before frcBibElemWrite()
* can be used.  This driver cannot create new elements in a BIB image!
* The size of the element structure in the BIB image cannot be increased.
*
* RETURNS: OK or ERROR.
*
* SEE ALSO: frcBibErrnoGet()
*/

GLOBAL STATUS	 frcBibElemWrite
(
    BIB_HDL	 handle,	/* usually specifies an EEPROM		*/
    int		 element,	/* BIB element to write			*/
    int		 index,		/* element index (0..n)			*/
    void	*pSrcBuf	/* source buffer address		*/
)
{
    return (bibRdWr (handle, element, index, pSrcBuf,
	    *((UINT8 *)pSrcBuf) + 1, 1));
}

/*******************************************************************************
*
* bibRdWr - read/write element from/to BIB
*
* This internal routine performs the read or write access to the BIB.  First
* the specified <element> is searched in the directory to get its offset
* within the BIB.  Then the access is done according to the <writeAccess>
* flag.
*
* RETURNS: OK or ERROR.
*/

LOCAL STATUS	 bibRdWr
(
    BIB_HDL	 handle,	/* usually specifies an EEPROM		*/
    int		 element,	/* BIB element to read or write		*/
    int		 index,		/* access the <n>th occurrence of <element> */
    void	*pBuf,		/* source or destination buffer address	*/
    UINT32	 bufSize,	/* size of the buffer in bytes		*/
    int		 writeAccess	/* 1=write, 0=read access to BIB	*/
)
{
HDL_DESC	*pDesc;		/* pointer to current descriptor	*/
BIB_DIR		*pDir;		/* pointer to directory structure	*/
FAST UINT8	*pData;		/* pointer to BIB element		*/
FAST UINT32	 i;		/* fast counter variable		*/


#if 0 
def	DEBUG
    printf ("\n bibRdWr(handle=0x%lx, element=%d, pBuf=%p, bufSize=0x%lx,"
	    " writeAccess=%d)\n", (unsigned long)handle, element, pBuf,
	    (unsigned long)bufSize, writeAccess);
#endif

    SET_ERRNO (OK);				/* clear previous error	*/

    /* check if a descriptor exists for <handle> */
/*printf ("\n bibRdWr=%x,%x,%x,%x",pBuf->Size,pBuf->Data[0],pBuf->Data[1],pBuf->Data[2]);*/
    if ((pDesc = hdlSearch (handle)) == NULL)
	return (ERROR);				/* unknown handle */

    /* check parameters */

    if ((element < 0)  ||  (index < 0)  ||  (pBuf == NULL)  ||  (bufSize == 0))
    {
	SET_ERRNO (S_frcBibDrv_INVALID_PARAMETER);
	pDesc->prevErrno = S_frcBibDrv_INVALID_PARAMETER;
	return (ERROR);
    }

    /* see if BIB 2.0 or BIB 2.1 */
    if (pDesc->pBIB->BlockId == BIB20_VERSION)		/* ==== BIB 2.0 ==== */
    {
	/* check <index> */

	if (index != 0)
	{
	    /* BIB 2.0 does only support one element per type */

	    SET_ERRNO (S_frcBibDrv_INVALID_PARAMETER);
	    pDesc->prevErrno = S_frcBibDrv_INVALID_PARAMETER;
	    return (ERROR);
	}

	/* set pointer to the first BIB directory */

	pDir = &pDesc->pBIB->Directory;		/* pointer to 1st directory */
	i    = 0;

	/* search <element> in the directory list */

	while (1)
	{
	    /* update the directory entry counter */

	    i += pDir->dirEntryCnt;

#if 0 
def	DEBUG
	    printf ("bibRdWr(): element =%3d, total cnt =%3ld, "
		    "dirEntryCnt =%3d\n",
		    element, i, pDir->dirEntryCnt);
#endif

	    /* see if this directory structure contains <element> */

	    if ((UINT32) element < i)
		break;				/* found the right directory */

	    /* check if there is another directory in the list */

	    if (pDir->offsNextDir == 0)
	    {
		SET_ERRNO (S_frcBibDrv_ELEMENT_NOT_FOUND);
		pDesc->prevErrno = S_frcBibDrv_ELEMENT_NOT_FOUND;
		return (ERROR);			/* <element> not found */
	    }

	    /* set pointer to the next directory */

	    pDir = (BIB_DIR *)((UINT8 *)&(pDir->offsNextDir)
				+ pDir->offsNextDir);

	    /* check if directory is located within the BIB image */

	    if ((UINT32)((UINT8 *)pDir - (UINT8 *)pDesc->pBIB)
		>  pDesc->bibLength - 3)
	    {
		SET_ERRNO (S_frcBibDrv_INVALID_DIR);
		pDesc->prevErrno = S_frcBibDrv_INVALID_DIR;
		return (ERROR);		/* fatal error: invalid BIB directory */
	    }
	} /* while (1) */

	/* calculate entry within current directory */

	i = pDir->dirEntryCnt - i + element;

#if 0 
def	DEBUG
	printf ("bibRdWr(): element =%3d, dirEntryCnt =%3d, index = %ld\n",
		element, pDir->dirEntryCnt, i);
#endif

	/* check if directory entry is located within the BIB image */

	pData = (UINT8 *)&pDir->offsDirEntry[i];   /* ptr to directory entry */

	if ((UINT32)(pData - (UINT8 *)pDesc->pBIB)  >  pDesc->bibLength - 3)
	{
	    SET_ERRNO (S_frcBibDrv_INVALID_DIR);
	    pDesc->prevErrno = S_frcBibDrv_INVALID_DIR;
	    return (ERROR);		/* fatal error: invalid BIB directory */
	}

	/* get offset to BIB element from the directory */

	i = pDir->offsDirEntry[i];

#if 0 
def	DEBUG
	printf ("bibRdWr(): element =%3d, offset = %ld\n", element, i);
#endif

	if (i == 0)
	{
	    SET_ERRNO (S_frcBibDrv_ELEMENT_NOT_FOUND);
	    pDesc->prevErrno = S_frcBibDrv_ELEMENT_NOT_FOUND;
	    return (ERROR);		/* <element> does not exist */
	}

	pData += i;

    } /* end BIB 2.0 */
    else						/* ==== BIB 2.1 ==== */
    {
	/* set data pointer to the begin of the internal linked list */

	pData = ((BIB_V21 *)(pDesc->pBIB))->dataArea;

	while (1)
	{
	    i = *(pData++);		/* get Id of current BIB element */

#if 0 
def	DEBUG
	    printf ("bibRdWr(): element =%3d, index =%2d, curr.element =%3ld\n",
		    element, index, i);
#endif
	    if ((int)i == element)
	    {
		if (index == 0)
		    break;		/* found the requested element	*/

		index--;	/* search another element of the same type */
	    }

	    if (i == 0xFF)
	    {
		SET_ERRNO (S_frcBibDrv_ELEMENT_NOT_FOUND);
		pDesc->prevErrno = S_frcBibDrv_ELEMENT_NOT_FOUND;
		return (ERROR);		/* <element> does not exist */
	    }

	    /* check size field */

	    if (pData[0] == 0)
	    {
		SET_ERRNO (S_frcBibDrv_INVALID_DIR_ENTRY);
		pDesc->prevErrno = S_frcBibDrv_INVALID_DIR_ENTRY;
		return (ERROR);		/* fatal error: invalid dir entry */
	    }

	    pData += (pData[0] + 1);		/* update data pointer	*/

	    /* check if BIB element structure is located within the BIB image */

	    if ((UINT32)(pData - (UINT8 *)pDesc->pBIB)  >  pDesc->bibLength - 3)
	    {
		SET_ERRNO (S_frcBibDrv_INVALID_DIR_ENTRY);
		pDesc->prevErrno = S_frcBibDrv_INVALID_DIR_ENTRY;
		return (ERROR);		/* fatal error: invalid dir entry */
	    }

	} /* while (1) */

    } /* end BIB 2.1 */

    /* check if BIB element structure is located within the BIB image */

    if ((UINT32)(pData - (UINT8 *)pDesc->pBIB)  >  pDesc->bibLength - 3)
    {
	SET_ERRNO (S_frcBibDrv_INVALID_DIR_ENTRY);
	pDesc->prevErrno = S_frcBibDrv_INVALID_DIR_ENTRY;
	return (ERROR);		/* fatal error: invalid dir entry */
    }

    /* get size of this BIB element structure */

    i = pData[0] + 1;			/* first byte is the size - 1 */

#if 0 
def	DEBUG
    printf ("bibRdWr(): element =%3d, structure size = %ld\n", element, i);
#endif

    /* access the BIB element structure */

    if (writeAccess)			/* BIB write access */
    {
    UINT32	 chkSum;		/* checksum over the BIB image	*/

	/*
	 * Check the size of the BIB element which should be written.
	 * We must not overwrite any other data, therefore the size of
	 * the element structure within the BIB image cannot be increased!
	 * It is possible to decrease the size of an existing element, but
	 * this is then taken as the "new" size and cannot be made larger
	 * again.
	 */
	if (i < bufSize)
	{
	    SET_ERRNO (S_frcBibDrv_ELEMENT_TOO_LARGE);
	    pDesc->prevErrno = S_frcBibDrv_ELEMENT_TOO_LARGE;
	    return (ERROR);		/* element is too large */
	}

	/* clear whole element structure in memory */

	memset (pData, 0x00, (size_t) i);

	/* copy data to BIB image in memory */

	memcpy (pData, pBuf, (size_t) bufSize);

	/* save address and size of the BIB element in memory */

	pBuf    = pData;		/* save data pointer		*/
	bufSize = i;			/* save number of bytes to write */

	/* recalculate checksum of the BIB image */

	pData = (UINT8 *) pDesc->pBIB;

	for (chkSum = 0, i = 0;  i < pDesc->bibLength - 2;  i++)
	    chkSum += *(pData++);		/* add data byte to checksum */

#if 0 
def	DEBUG
	printf ("bibRdWr(): element =%3d, new checksum = 0x%04lX\n",
		element, chkSum);
#endif

	/* update checksum in memory */

	BIB_NUM16_SET (pData, chkSum);

	/* write data and checksum to the device */

	if (pDesc->bibIntf.bibWriteIntfRtn (handle,
			pDesc->devOffs + ((UINT8 *)pBuf - (UINT8 *)pDesc->pBIB),
			bufSize, pBuf) != OK
	||  pDesc->bibIntf.bibWriteIntfRtn (handle,
			pDesc->devOffs + (pData - (UINT8 *)pDesc->pBIB),
			2, pData) != OK)
	{
	    /* see if interface routine has set an error code */
		
	    if (GET_ERRNO == OK)
		SET_ERRNO (S_frcBibDrv_WRITE_INTF_ERROR);

	    pDesc->prevErrno = S_frcBibDrv_WRITE_INTF_ERROR;
	    pDesc->origErrno = GET_ERRNO;
		
	}
	else
	{
	    /* no error - verify if image was written properly to the device */
		
  if (bibVerify (pDesc) == OK)
		SET_ERRNO (OK);		/* be sure to clear <errno> if all OK */
	}

	/* check for any errors, try to recover if required */
	
	if (GET_ERRNO != OK)
	{
	int	errnoSave;

	    /*
	     * To be sure that the bibReadIntfRtn() does not violate the
	     * maximum number of bytes that may be written at once for
	     * some I2C-bus EEPROMs, e.g. a 24C02, we write the data
	     * byte by byte.
	     */
	    errnoSave = GET_ERRNO;		/* save original error	*/
	    SET_ERRNO (OK);			/* clear previous error	*/

	    for (i = 0;  i < pDesc->bibLength;  i++)
	    {
		if (pDesc->bibIntf.bibWriteIntfRtn (handle, pDesc->devOffs + i,
					    1, (UINT8 *)pDesc->pBIB + i) != OK)
		{
		    /* see if interface routine has set an error code */

		    if (GET_ERRNO == OK)
			SET_ERRNO (S_frcBibDrv_WRITE_INTF_ERROR);

		    pDesc->prevErrno = S_frcBibDrv_DATA_RECOVERY_ERROR;
		    pDesc->origErrno = GET_ERRNO;
		    SET_ERRNO (S_frcBibDrv_DATA_RECOVERY_ERROR);

		    return (ERROR);	/* no hope to recover from here	*/
		}
		
	    }
		

	    SET_ERRNO (errnoSave);		/* restore original error */

	    /* image is re-written - verify it again */

	    if (bibVerify (pDesc) != OK)
		SET_ERRNO (S_frcBibDrv_DATA_RECOVERY_ERROR);

	    return (ERROR);
	}
    }
    else				/* BIB read access */
    {
	/* check the size of the destination buffer */

	if (i > bufSize)
	{
	    SET_ERRNO (S_frcBibDrv_BUFFER_TOO_SMALL);
	    pDesc->prevErrno = S_frcBibDrv_BUFFER_TOO_SMALL;
	    return (ERROR);		/* buffer is too small */
	}

	/*
	 * Clear the whole memory buffer, because data of 0x00 is default
	 * for all BIB flags, size and date fields.
	 */
	memset (pBuf, 0x00, (size_t) bufSize);

	/* copy out the BIB element - including its <Size> field */

	memcpy (pBuf, pData, (size_t) i);
    }

    return (OK);
}

/*******************************************************************************
*
* bibVerify - verify BIB image in memory with device data
*
* This routine compares the BIB image in local memory (our BIB cache) with the
* data in the device, e.g. an EEPROM.
*
* RETURNS: OK or ERROR.
*/

LOCAL STATUS	 bibVerify
(
    HDL_DESC	*pDesc		/* pointer to descriptor		*/
)
{
UINT8		 buffer[BIB_LENGTH_UNIT];	/* data verify buffer	*/
FAST UINT32	 i;		/* fast counter variable		*/
int		 errnoSave;


    /* check parameter */

    if (pDesc == NULL)
	return (ERROR);

    errnoSave = GET_ERRNO;			/* save original error	*/
    SET_ERRNO (OK);				/* clear previous error	*/

    /* read and compare data in portions of 16 bytes */

    for (i = 0;  i < pDesc->bibLength;  i += BIB_LENGTH_UNIT)
    {
    
	 
      
	if (pDesc->bibIntf.bibReadIntfRtn (pDesc->hdl, pDesc->devOffs + i,
					   BIB_LENGTH_UNIT, buffer) != OK)
	{
	    if (GET_ERRNO == OK)
		SET_ERRNO (S_frcBibDrv_READ_INTF_ERROR);

	    pDesc->prevErrno = S_frcBibDrv_DATA_VERIFY_ERROR;
	    pDesc->origErrno = GET_ERRNO;
	    SET_ERRNO (S_frcBibDrv_DATA_VERIFY_ERROR);
		

	    return (ERROR);
	}

	if (memcmp (buffer, (UINT8 *)pDesc->pBIB + i, BIB_LENGTH_UNIT) != 0)
	{
	 
	  	
	    pDesc->prevErrno = S_frcBibDrv_DATA_VERIFY_ERROR;
	    pDesc->origErrno = S_frcBibDrv_DATA_VERIFY_ERROR;
	    SET_ERRNO (S_frcBibDrv_DATA_VERIFY_ERROR);
		

	    return (ERROR);
	}
    }

    SET_ERRNO (errnoSave);			/* restore original error */

    return (OK);
}

/*******************************************************************************
*
* frcBibErrnoGet - get previous and original error codes
*
* This routine allows an application to get the underlying error conditions
* in case of a faulty BIB read or write operation.  It gets the previous and
* original error numbers which are associated with <handle>, and stores them
* into the integer variables addressed by <pPrevErrno>, respective
* <pOrigErrno>.  These error codes may give you some information about the
* real cause of a fault.  See frcBibElemWrite() for a description of possible
* error conditions.
*
* RETURNS: OK, or ERROR if invalid handle.
*
* SEE ALSO: frcBibDrvShow(), frcBibElemWrite()
*/

GLOBAL STATUS	 frcBibErrnoGet
(
    BIB_HDL	 handle,	/* usually specifies an EEPROM		*/
    int		*pPrevErrno,	/* variable to store previous error	*/
    int		*pOrigErrno	/* variable to store original error	*/
)
{
HDL_DESC	*pDesc;		/* pointer to current descriptor	*/


    /* first check if a descriptor exists for <handle> */

    if ((pDesc = hdlSearch (handle)) == NULL)
	return (ERROR);				/* unknown handle */

    /* print error numbers, or store them into the specified variables */

    if (pPrevErrno == NULL  ||  pOrigErrno == NULL)
    {
	printf ("Previous Error: 0x%08lX\n"
		"Original Error: 0x%08lX\n",
		(long) (pDesc->prevErrno),
		(long) (pDesc->origErrno));
    }
    else
    {
	*pPrevErrno = pDesc->prevErrno;
	*pOrigErrno = pDesc->origErrno;
    }

    return (OK);
}

/*******************************************************************************
*
* frcBibDrvShow - show BIB driver information
*
* This show routine displays all attached BIB handles and the associated
* information, e.g. the previous error code, whether a local memory pool
* is used, and the address of the BIB interface routines.  If <infoLvl>
* is higher than 1 and an external show routine has been defined via the
* global function pointer 'frcBibShowRtn', the contents of the BIB itself
* is also displayed.
*
* RETURNS: OK or ERROR.
*
* SEE ALSO: frcBibErrnoGet()
*/
GLOBAL STATUS	 frcBibDrvShow
(
    int		 infoLvl	/* level of information (0 to 3)	*/
)
{
HDL_DESC	*pDesc;		/* pointer to current descriptor	*/

 frcBibShowRtn=frcBibShow;

    if (pRootDesc == NULL)
    {
	printf ("There is no BIB attached!\n");
	return (ERROR);
    }

    /* display all handles and the associated information */

    printf ("Attached BIB handles and associated information\n"
	    "===============================================\n");

    for (pDesc = pRootDesc;  pDesc != NULL;  pDesc = pDesc->pNextDesc)
    {
	printf ("\nHdl 0x%08lX  -  ", (long) (pDesc->hdl));

	if (pDesc->prevErrno == OK)
	{
	    printf ("No Error\n");

	    /* underline the handle number */

	    if (infoLvl >= 1)
		printf ("--------------");
	}
	else
	{

	    printf ("Previous Error: 0x%08lX\n"
		    "%14s     "
		    "Original Error: 0x%08lX\n",
		    (long) (pDesc->prevErrno),
		    (infoLvl >= 1 ? "--------------" : ""),
		    (long) (pDesc->origErrno));
	}

	/* info level 1 */

	if (infoLvl >= 1)
	{
	    printf ("\n");

	    /* see if a local memory pool is used */

	    if (pDesc->bibIntf.pMemPool != NULL)
	    {
		printf ("    Using local memory pool at %p, still %ld bytes\n",
			pDesc->bibIntf.pMemPool,
			(long) (pDesc->bibIntf.memPoolSize));
	    }
	    else
	    {
		printf ("    Using memory allocation/free routines\n");
	    }

	    /* display BIB read/write interface routines */

	    printf ("    BIB interface routines: read=%p, write=%p\n",
		    pDesc->bibIntf.bibReadIntfRtn,
		    pDesc->bibIntf.bibWriteIntfRtn);

	    /* information about the BIB image */

	    printf ("    The BIB image is 0x%lX (%ld) bytes large\n",
		    (long) (pDesc->bibLength), (long) (pDesc->bibLength));

	    printf ("    The cache buffer is located at address %p\n",
		    pDesc->pBIB);
	}

	/* info level 2/3 */

	if (infoLvl >= 2)
	{
	    if (frcBibShowRtn != NULL)
	    {
		if (infoLvl > 2)
		    printf ("\n");

		if (frcBibShowRtn (pDesc->pBIB, (size_t)(pDesc->bibLength),
				   (BOOL)(infoLvl > 2 ? TRUE : FALSE), NULL))
		    printf ("\n*** ERROR IN BIB IMAGE ***\n");
	    }
	}
    }

    printf ("\n");

    return (OK);
}
/*******************************************************************************
*
* hdlSearch - search the descriptor of the specified handle
*
* This routine searches the descriptor of the specified BIB handle.  There
* is a linked list of descriptors - one per handle.  The routine returns the
* address of the corresponding descriptor.  If no descriptor can be found
* for the specified <handle>, NULL is returned.
*
* RETURNS: Descriptor, or NULL if <handle> does not exist.
*/

LOCAL HDL_DESC	*hdlSearch
(
    BIB_HDL	 handle		/* external handle (must be unique)	*/
)
{
HDL_DESC	*pDesc;		/* pointer to current descriptor	*/


    pDesc = pRootDesc;			/* start with the root descriptor */

    /* search <handle> in the descriptor list */

    while (pDesc != NULL)
    {
	/* check if descriptor belongs to <handle> */

	if (pDesc->hdl == handle)
	    break;			/* descriptor found, return it */

	/* set pointer to the next descriptor */

	pDesc = pDesc->pNextDesc;
    }

    if (pDesc == NULL)
	SET_ERRNO (S_frcBibDrv_UNKNOWN_HANDLE);

    return (pDesc);			/* return descriptor or NULL	*/
}

/*******************************************************************************
*
* hdlCreate - create a descriptor for a new handle
*
* This routine creates a descriptor for a new BIB handle.  The handle must be
* defined external and must be unique.  If there already exists a descriptor
* for the specified <handle>, NULL is returned to indicate an error.
* Otherwise a new descriptor is created and added to the linked list.  The
* routine then returns the address of the new descriptor.
* In case of an error, e.g. if there is not enough memory, NULL is returned.
*
* RETURNS: New descriptor, or NULL if error.
*/

LOCAL HDL_DESC	*hdlCreate
(
    BIB_HDL	 handle,	/* external handle (must be unique)	*/
    BIB_INTF	*pIntf		/* ptr to interface structure		*/
)
{
HDL_DESC	*pDesc;		/* pointer to current descriptor	*/


    /* first check if descriptor already exists */

    pDesc = hdlSearch (handle);
    if (pDesc != NULL)
    {
	SET_ERRNO (S_frcBibDrv_ALREADY_ATTACHED);
	pDesc->prevErrno = S_frcBibDrv_ALREADY_ATTACHED;
	return (NULL);			/* do not allow duplicate handles */
    }

    /* handle does not exist, which is OK - clear error number */

    SET_ERRNO (OK);

    /* allocate memory for a new descriptor */

    pDesc = (HDL_DESC *) bibMalloc (pIntf, sizeof (HDL_DESC));
    if (pDesc == NULL)
	return (NULL);			/* not enough memory error */

    /* insert descriptor at the beginning of the list */

    pDesc->pNextDesc = pRootDesc;
    pRootDesc = pDesc;			/* new begin of the list */

    /* store <handle> into descriptor */

    pDesc->hdl = handle;

    /* copy interface structure to descriptor */

    pDesc->bibIntf = *pIntf;

    return (pDesc);			/* return new descriptor */
}

/*******************************************************************************
*
* hdlRemove - remove the descriptor of the specified handle
*
* This routine removes the descriptor of the specified BIB handle.  It first
* searches the descriptor in the linked list.  If it cannot be found, ERROR
* is returned.  Otherwise the descriptor is removed from the list and its
* memory is deallocated.
*
* RETURNS: OK or ERROR.
*/

LOCAL STATUS	 hdlRemove
(
    BIB_HDL	 handle		/* external handle (must be unique)	*/
)
{
HDL_DESC	*pCurrDesc;	/* pointer to current descriptor	*/
HDL_DESC	*pPrevDesc;	/* pointer to previous descriptor	*/


    pCurrDesc = pRootDesc;		/* start with the root descriptor */
    pPrevDesc = NULL;			/* no previous descriptor yet	*/

    /* search <handle> in the descriptor list */

    while (pCurrDesc != NULL)
    {
	/* check if descriptor belongs to <handle> */

	if (pCurrDesc->hdl == handle)
	    break;			/* descriptor found, return it */

	/* set pointer to the next descriptor, save previous */

	pPrevDesc = pCurrDesc;
	pCurrDesc = pCurrDesc->pNextDesc;
    }

    if (pCurrDesc == NULL)
    {
	SET_ERRNO (S_frcBibDrv_UNKNOWN_HANDLE);
	return (ERROR);			/* handle not found, error */
    }

    /* remove the descriptor from the list */

    if (pPrevDesc == NULL)		/* first descriptor in the list */
	pRootDesc = pCurrDesc->pNextDesc;
    else
	pPrevDesc->pNextDesc = pCurrDesc->pNextDesc;

    /* deallocate descriptor memory */

    bibFree (&(pCurrDesc->bibIntf), pCurrDesc);

    return (OK);
}

/*******************************************************************************
*
* bibMalloc - allocate memory
*
* This is a driver-specific memory allocation routine.  It allocates <size>
* bytes of memory from the <pIntf> specific local memory pool, or via the
* bibMallocRtn(), respective malloc().
* The routine returns a pointer to memory block, or NULL in case of an error,
* e.g. if not enough memory is available.
*
* RETURNS: Pointer to memory block, or NULL if error.
*/

LOCAL void	*bibMalloc
(
    BIB_INTF	*pIntf,		/* ptr to interface structure		*/
    size_t	 size		/* number of bytes to allocate		*/
)
{
FAST UINT8	*pMemBlock;	/* fast pointer to memory block		*/


    /* check if <pIntf> is specified */

    if (pIntf == NULL)
	pMemBlock = NULL;			/* use bibMallocRtn()	*/
    else
	pMemBlock = pIntf->pMemPool;		/* evtl. use mem. pool	*/

    /* allocate memory */

    if (pMemBlock == NULL)
    {
	/* no local memory pool available - use bibMallocRtn() */

	pMemBlock = bibMallocRtn (size);
	if (pMemBlock == NULL)
	{
	    SET_ERRNO (S_frcBibDrv_NOT_ENOUGH_MEMORY);
	    return (NULL);
	}
    }
    else
    {
	/* use local memory pool */

	if (size > pIntf->memPoolSize)
	{
	    SET_ERRNO (S_frcBibDrv_NOT_ENOUGH_MEMORY);
	    return (NULL);
	}

	pIntf->pMemPool = pMemBlock + size;	/* remaining pool	*/
	pIntf->memPoolSize -= size;		/* decrease pool size	*/
    }

    /* initialize the new memory block with 0 */

    memset (pMemBlock, 0x00, (size_t) size);

    return ((void *) pMemBlock);
}

/*******************************************************************************
*
* bibFree - free memory
*
* This is a driver-specific memory free routine.  If a local memory pool is
* specified via interface structure <pIntf>, the routine just returns, because
* blocks of a local memory pool need not to be freed.  If no local memory pool
* exists, bibFreeRtn() is called, which calls an application specific free
* routine, or free().
*
* RETURNS: N/A.
*/

LOCAL void	 bibFree
(
    BIB_INTF	*pIntf,		/* ptr to interface structure		*/
    void	*ptr		/* memory block to free			*/
)
{
    /* check if <pIntf> is specified */

    if (pIntf != NULL)
	if (pIntf->pMemPool != NULL)
	    return;			/* nothing to be done		*/

    /* no local memory pool - call standard free routine */

    bibFreeRtn (ptr);
}

/*******************************************************************************
*
* frcBibRead - read an element from BIB (OBSOLETE)
*
* This routine calls frcBibElemRead() with an <index> of 0.
*
* NOTE:
* This routine is obsolete and exists only for backward compatibility!
* Use frcBibElemRead() to read data from a BIB device.
*
* RETURNS: OK or ERROR.
*/

GLOBAL STATUS	 frcBibRead
(
    BIB_HDL	 handle,	/* usually specifies an EEPROM		*/
    int		 element,	/* BIB element to read			*/
    void	*pDstBuf,	/* destination buffer address		*/
    UINT32	 bufSize	/* size of the buffer in bytes		*/
)
{
    return (frcBibElemRead (handle, element, 0, pDstBuf, bufSize));
}

/*******************************************************************************
*
* frcBibWrite - write an element to BIB (OBSOLETE)
*
* This routine calls frcBibElemWrite() with an <index> of 0.
*
* NOTE:
* This routine is obsolete and exists only for backward compatibility!
* Use frcBibElemRead() to write data to a BIB device.
*
* RETURNS: OK or ERROR.
*/

GLOBAL STATUS	 frcBibWrite
(
    BIB_HDL	 handle,	/* usually specifies an EEPROM		*/
    int		 element,	/* BIB element to read			*/
    void	*pSrcBuf	/* source buffer address		*/
)
{
    return (frcBibElemWrite (handle, element, 0, pSrcBuf));
}

/*******************************************************************************
*
* bibRead - read element from BIB (OBSOLETE)
*
* This routine calls frcBibElemRead() with an <index> of 0.
*
* NOTE:
* This routine is obsolete and exists only for backward compatibility!
* Use frcBibElemRead() to read data from a BIB device.
*
* NOMANUAL
*
* RETURNS: OK or ERROR.
*/

GLOBAL STATUS	 bibRead
(
    BIB_HDL	 handle,	/* usually specifies an EEPROM		*/
    int		 element,	/* BIB element to read			*/
    void	*pDstBuf,	/* destination buffer address		*/
    UINT32	 bufSize	/* size of the buffer in bytes		*/
)
{
    return (frcBibElemRead (handle, element, 0, pDstBuf, bufSize));
}

/*******************************************************************************
*
* bibWrite - write element to BIB
*
* This routine calls frcBibElemWrite() with an <index> of 0.
*
* NOTE:
* This routine is obsolete and exists only for backward compatibility!
* Use frcBibElemRead() to write data to a BIB device.
*
* NOMANUAL
*
* RETURNS: OK or ERROR.
*/

GLOBAL STATUS	 bibWrite
(
    BIB_HDL	 handle,	/* usually specifies an EEPROM		*/
    int		 element,	/* BIB element to write			*/
    void	*pSrcBuf	/* source buffer address		*/
)
{
    return (frcBibElemWrite (handle, element, 0, pSrcBuf));
}
