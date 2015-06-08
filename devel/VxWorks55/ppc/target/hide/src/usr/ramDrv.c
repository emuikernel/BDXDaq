/* ramDrv.c - RAM disk driver */

/* Copyright 1984-1995 Wind River Systems, Inc. */
#include "copyright_wrs.h"

/*
modification history
--------------------
02m,11dec96,dgp  doc: fix SPR #7596, add INCLUDE FILE ramDrv.h
02l,16jan95,rhp  ANSIfy declaration of ramDrv().
02k,20jan93,jdi  documentation cleanup.
02j,21oct92,jdi  removed mangen SECTION designation.
02i,18jul92,smb  Changed errno.h to errnoLib.h.
02h,04jul92,jcf  scalable/ANSI/cleanup effort.
02g,26may92,rrr  the tree shuffle
02f,04oct91,rrr  passed through the ansification filter
		  -changed READ, WRITE and UPDATE to O_RDONLY O_WRONLY O_RDWR
		  -changed copyright notice
02e,11apr91,jdi  documentation cleanup; doc review by kdl.
02d,19oct90,kdl  changed "msdos" references to "dosFs"; changed "sector"
		 to "block" throughout.
02c,30jul90,dnw  coerced malloc to (char*) to eliminate warning
02b,12jul90,kdl  added bd_statusChk field to BLK_DEV.
02a,15may90,kdl  documentation.
01z,04may90,kdl  added BLK_DEV fields (bd_mode, bd_readyChanged);
		 added FIODISKFORMAT.
01y,23mar90,kdl  changed BLK_DEV field names (blkIo.h).
01x,05mar90,kdl  changed to support multiple file systems.
01w,21feb90,dab  added DEFAULT_DISK_SIZE for ramMkfs().
01v,08jul88,jcf  lint.
01u,06jun88,dnw  changed rtLib to rt11Lib.
01t,30may88,dnw  changed to v4 names.
01s,06nov87,ecs  documentation.
01r,04aug87,gae  made ramClose() return STATUS.
		 Changed "static" stuff in ramDrv().
01q,25mar87,jlf  documentation
01p,13mar87,gae  documentation and fixed/improved version 01o.
01o,11mar87,gae  changed raw device open to not use "-" any more.
		 made ramMkfs() return STATUS not (char *).
		 made ramDrv() be a NOP on successive calls.
01n,17feb87,gae  fixed rtDevInit() call.
01m,04feb87,llk  changed call to rtDevInit.  Specifies number of files.
01l,21dec86,dnw  changed to not get include files from default directories.
01k,05sep86,jlf  documentation
01j,15aug86,ecs  renamed ramIoctrl to ramIoctl, changed to call rtIoctl.
01i,02jul86,jlf  oops.
01h,02jul86,jlf  documentation.
01g,03mar86,jlf  changed ioctrl calls to ioctl.
01f,11nov85,jlf  made ramOpen, ramCreate, and ramDelete do a rtReadyChange.
		   The is a kludge so when multiple CPU's share a RAM
		   device, the directory will be reread when any of them
		   try to create, delete, or open a file.
01e,19sep85,jlf  got rid of checkMount, and removed member `mounted' from
		 dev descriptor.
01d,12sep85,jlf  updated calling sequence to rtDevInit.
01c,14aug85,dnw  added ramMkfs.
01b,21jul85,jlf  documentation
01a,24may85,jlf  written, by modifying v03k of fd208Drv.c.
*/

/*
This driver emulates a disk driver, but actually keeps all data in memory.
The memory location and size are specified when the "disk" is created.
The RAM disk feature is useful when data must be preserved between boots of
VxWorks or when sharing data between CPUs.

USER-CALLABLE ROUTINES
Most of the routines in this driver are accessible only through the I/O system.
Two routines, however, can be called directly by the user.  The first, ramDrv(),
provides no real function except to parallel the initialization function found
in true disk device drivers.  A call to ramDrv() is not required to use the RAM
disk driver.  However, the second routine, ramDevCreate(), must be called 
directly to create RAM disk devices.

Once the device has been created, it must be associated with a name and
file system (dosFs, rt11Fs, or rawFs).  This is accomplished by passing
the value returned by ramDevCreate(), a pointer to a block device
structure, to the file system's device initialization routine or
make-file-system routine.  See the manual entry ramDevCreate() for a
more detailed discussion.

IOCTL FUNCTIONS
The RAM driver is called in response to ioctl() codes in the same manner
as a normal disk driver.  When the file system is unable to handle a
specific ioctl() request, it is passed to the ramDrv driver.  Although
there is no physical device to be controlled, ramDrv does handle a
FIODISKFORMAT request, which always returns OK.  All other ioctl()
requests return an error and set the task's `errno' to S_ioLib_UNKNOWN_REQUEST.

INCLUDE FILE
ramDrv.h

SEE ALSO: dosFsDevInit(), dosFsMkfs(), rt11FsDevInit(), rt11FsMkfs(),
rawFsDevInit(),
.pG "I/O System, Local File Systems"

LINTLIBRARY
*/

#include "vxWorks.h"
#include "blkIo.h"
#include "ioLib.h"
#include "iosLib.h"
#include "memLib.h"
#include "errnoLib.h"
#include "string.h"
#include "stdlib.h"
#include "stdio.h"


#define DEFAULT_DISK_SIZE  (min (memFindMax()/2, 51200))
#define DEFAULT_SEC_SIZE   512


typedef struct		/* RAM_DEV - RAM disk device descriptor */
    {
    BLK_DEV	 ram_blkdev;	/* generic block device structure */
    char	 *ram_addr;	/* memory location of the RAM disk */
    int		 ram_blkOffset;	/* block offset of this device from ram_addr */
    } RAM_DEV;



/* FORWARD DECLARATIONS */

LOCAL STATUS ramIoctl ();
LOCAL STATUS ramBlkRd ();
LOCAL STATUS ramBlkWrt ();


/*******************************************************************************
*
* ramDrv - prepare a RAM disk driver for use (optional)
*
* This routine performs no real function, except to provide compatibility
* with earlier versions of ramDrv and to parallel the initialization
* function found in true disk device drivers.  It also is used in
* usrConfig.c to link in the RAM disk driver when building
* VxWorks.  Otherwise, there is no need to call this routine before using
* the RAM disk driver.
*
* RETURNS: OK, always.
*/

STATUS ramDrv (void)

    {
    return (OK);			/* no initialization required */
    }

/*******************************************************************************
*
* ramDevCreate - create a RAM disk device
*
* This routine creates a RAM disk device.
*
* Memory for the RAM disk can be pre-allocated separately; if so, the
* <ramAddr> parameter should be the address of the pre-allocated device
* memory.  Or, memory can be automatically allocated with malloc() by
* setting <ramAddr> to zero.
*
* The <bytesPerBlk> parameter specifies the size of each logical block
* on the RAM disk.  If <bytesPerBlk> is zero, 512 is used.
*
* The <blksPerTrack> parameter specifies the number of blocks on each
* logical track of the RAM disk.  If <blksPerTrack> is zero, the count of
* blocks per track is set to <nBlocks> (i.e., the disk is defined
* as having only one track).
*
* The <nBlocks> parameter specifies the size of the disk, in blocks.
* If <nBlocks> is zero, a default size is used.  The default is calculated
* using a total disk size of either 51,200 bytes or one-half of the size
* of the largest memory area available, whichever is less.  This default
* disk size is then divided by <bytesPerBlk> to determine the number
* of blocks.
*
* The <blkOffset> parameter specifies an offset, in blocks, from the start
* of the device to be used when writing or reading the RAM disk.  This
* offset is added to the block numbers passed by the file system during
* disk accesses.  (VxWorks file systems always use block numbers beginning
* at zero for the start of a device.)  This offset value is typically useful
* only if a specific address is given for <ramAddr>.  Normally, <blkOffset>
* is 0.
*
* FILE SYSTEMS:
* Once the device has been created, it must be associated with a name and a
* file system (dosFs, rt11Fs, or rawFs).  This is accomplished using the
* file system's device initialization routine or make-file-system routine,
* e.g., dosFsDevInit() or dosFsMkfs().  The ramDevCreate() call returns a
* pointer to a block device structure (BLK_DEV).  This structure contains
* fields that describe the physical properties of a disk device and specify
* the addresses of routines within the ramDrv driver.  The BLK_DEV structure
* address must be passed to the desired file system (dosFs, rt11Fs or rawFs)
* via the file system's device initialization or make-file-system routine.
* Only then is a name and file system associated with the device, making it
* available for use.
*
* EXAMPLE:
* In the following example, a 200-Kbyte RAM disk is created with
* automatically allocated memory, 512-byte blocks, a single track, and no
* block offset.  The device is then initialized for use with dosFs and assigned
* the name "DEV1:":
* .CS
*     BLK_DEV *pBlkDev;
*     DOS_VOL_DESC *pVolDesc;
*
*     pBlkDev = ramDevCreate (0,  512,  400,  400,  0);
*     pVolDesc = dosFsMkfs ("DEV1:", pBlkDev);
* .CE
* The dosFsMkfs() routine calls dosFsDevInit() with default parameters and
* initializes the file system on the disk by calling ioctl() with the
* \%FIODISKINIT function.
*
* If the RAM disk memory already contains a disk image created elsewhere,
* the first argument to ramDevCreate() should be the address in memory, and
* the formatting parameters -- <bytesPerBlk>, <blksPerTrack>, <nBlocks>, and
* <blkOffset> -- must be identical to those used when the image was
* created.  For example:
* .CS
*     pBlkDev = ramDevCreate (0xc0000, 512, 400, 400, 0);
*     pVolDesc = dosFsDevInit ("DEV1:", pBlkDev, NULL);
* .CE
* In this case, dosFsDevInit() must be used instead of dosFsMkfs(), because
* the file system already exists on the disk and should not be
* re-initialized.  This procedure is useful if a RAM disk is to be created
* at the same address used in a previous boot of VxWorks.  The contents of
* the RAM disk will then be preserved.
*
* These same procedures apply when creating a RAM disk with rt11Fs using
* rt11FsDevInit() and rt11FsMkfs(), or creating a RAM disk with rawFs using
* rawFsDevInit().
*
* RETURNS:
* A pointer to a block device structure (BLK_DEV) or NULL if memory cannot
* be allocated for the device structure or for the RAM disk.
*
* SEE ALSO: dosFsMkfs(), dosFsDevInit(), rt11FsDevInit(), rt11FsMkfs(),
* rawFsDevInit()
*/

BLK_DEV *ramDevCreate
    (
    char	*ramAddr,	/* where it is in memory (0 = malloc) */
    FAST int	bytesPerBlk,	/* number of bytes per block */
    int		blksPerTrack,	/* number of blocks per track */
    int		nBlocks,	/* number of blocks on this device */
    int		blkOffset	/* no. of blks to skip at start of device */
    )
					 

    {
    FAST RAM_DEV	*pRamDev;	/* ptr to created RAM_DEV struct */
    FAST BLK_DEV	*pBlkDev;	/* ptr to BLK_DEV struct in RAM_DEV */


    /* Set up defaults for any values not specified */

    if (bytesPerBlk == 0)
	bytesPerBlk = DEFAULT_SEC_SIZE;

    if (nBlocks == 0)
	nBlocks = DEFAULT_DISK_SIZE / bytesPerBlk;

    if (blksPerTrack == 0)
	blksPerTrack = nBlocks;


    /* Allocate a RAM_DEV structure for device */

    pRamDev = (RAM_DEV *) malloc (sizeof (RAM_DEV));
    if (pRamDev == NULL)
	return (NULL);					/* no memory */


    /* Initialize BLK_DEV structure (in RAM_DEV) */

    pBlkDev = &pRamDev->ram_blkdev;

    pBlkDev->bd_nBlocks      = nBlocks;		/* number of blocks */
    pBlkDev->bd_bytesPerBlk  = bytesPerBlk;	/* bytes per block */
    pBlkDev->bd_blksPerTrack = blksPerTrack;	/* blocks per track */

    pBlkDev->bd_nHeads       = 1;		/* one "head" */
    pBlkDev->bd_removable    = FALSE;		/* not removable */
    pBlkDev->bd_retry	     = 1;		/* retry count */
    pBlkDev->bd_mode	     = O_RDWR;		/* initial mode for device */
    pBlkDev->bd_readyChanged = TRUE;		/* new ready status */

    pBlkDev->bd_blkRd	     = ramBlkRd;	/* read block function */
    pBlkDev->bd_blkWrt	     = ramBlkWrt;	/* write block function */
    pBlkDev->bd_ioctl	     = ramIoctl;	/* ioctl function */
    pBlkDev->bd_reset	     = NULL;		/* no reset function */
    pBlkDev->bd_statusChk    = NULL;		/* no check-status function */


    /* Initialize remainder of device struct */

    if (ramAddr == NULL)				/* allocate mem if 0 */
	{
	pRamDev->ram_addr = (char *) malloc ((UINT) (bytesPerBlk * nBlocks));

	if (pRamDev->ram_addr == NULL)
	    {
	    free ((char *) pRamDev);
	    return (NULL);				/* no memory */
	    }
	}
    else
	pRamDev->ram_addr = ramAddr;


    pRamDev->ram_blkOffset = blkOffset;			/* block offset */


    return (&pRamDev->ram_blkdev);
    }

/*******************************************************************************
*
* ramIoctl - do device specific control function
*
* This routine is called when the file system cannot handle an ioctl()
* function.
*
* The FIODISKFORMAT function always returns OK, since a RAM disk does
* not require formatting.  All other requests return ERROR.
*
* RETURNS:  OK or ERROR.
*
* ERRNO: S_ioLib_UNKNOWN_REQUEST
*
*/

LOCAL STATUS ramIoctl (pRamDev, function, arg)
    RAM_DEV	*pRamDev;		/* device structure pointer */
    int		function;		/* function code */
    int		arg;			/* some argument */

    {
    FAST int	status;			/* returned status value */


    switch (function)
	{
	case FIODISKFORMAT:
	    status = OK;
	    break;

	default:
	    errnoSet (S_ioLib_UNKNOWN_REQUEST);
	    status = ERROR;
	}

    return (status);
    }

/*******************************************************************************
*
* ramBlkRd - read one or more blocks from a RAM disk volume
*
* This routine reads one or more blocks from the specified volume,
* starting with the specified block number.  The byte offset is
* calculated and the RAM disk data is copied to the specified buffer.
*
* If any block offset was specified during ramDevCreate(), it is added
* to <startBlk> before the transfer takes place.
*
* RETURNS: OK, always.
*/

LOCAL STATUS ramBlkRd (pRamDev, startBlk, numBlks, pBuffer)
    FAST RAM_DEV	*pRamDev;	/* pointer to device desriptor */
    int			startBlk;	/* starting block number to read */
    int			numBlks;	/* number of blocks to read */
    char		*pBuffer;	/* pointer to buffer to receive data */

    {
    FAST int		bytesPerBlk;	/* number of bytes per block */


    bytesPerBlk = pRamDev->ram_blkdev.bd_bytesPerBlk;

    /* Add in the block offset */

    startBlk += pRamDev->ram_blkOffset;

    /* Read the block(s) */

    bcopy ((pRamDev->ram_addr + (startBlk * bytesPerBlk)), pBuffer,
	   bytesPerBlk * numBlks);

    return (OK);
    }

/*******************************************************************************
*
* ramBlkWrt - write one or more blocks to a RAM disk volume
*
* This routine writes one or more blocks to the specified volume,
* starting with the specified block number.  The byte offset is
* calculated and the buffer data is copied to the RAM disk.
*
* If any block offset was specified during ramDevCreate(), it is added
* to <startBlk> before the transfer takes place.
*
* RETURNS: OK, always.
*/

LOCAL STATUS ramBlkWrt (pRamDev, startBlk, numBlks, pBuffer)
    FAST RAM_DEV	*pRamDev;	/* pointer to device desriptor */
    int			startBlk;	/* starting block number to write */
    int			numBlks;	/* number of blocks to write */
    char		*pBuffer;	/* pointer to buffer of data to write */

    {
    FAST int		bytesPerBlk;	/* number of bytes per block */


    bytesPerBlk = pRamDev->ram_blkdev.bd_bytesPerBlk;

    /* Add in the block offset */

    startBlk += pRamDev->ram_blkOffset;

    /* Write the block(s) */

    bcopy (pBuffer, (pRamDev->ram_addr + (startBlk * bytesPerBlk)),
	   bytesPerBlk * numBlks);

    return (OK);
    }
