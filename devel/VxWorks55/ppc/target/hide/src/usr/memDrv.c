/* memDrv.c - pseudo memory device driver */

/* Copyright 1984-1999 Wind River Systems, Inc. */
#include "copyright_wrs.h"

/*
modification history
--------------------
01o,12mar99,p_m  fixed SPR# 20018 by documenting lseek and open interaction.
01n,22feb99,wsl  minor doc cleanup
01m,13aug98,rlp  added memDevDelete function.
01l,16jul98,ics  added support for multiple files, directory searches
01l,16jan98,sjw  clean up error reporting to improve diagnostics
01k,16jan98,sjw  add support for FIONREAD and FIOSTATGET
01l,07jan98,ics  allow multiple simultaneous reads of the same file: SPR 20180
01k,11apr97,ics  allow a read of more than the remaining size to succeed
01j,16jan95,rhp  improve ANSI conformance in declarations of user routines
01i,21oct92,jdi  removed mangen SECTION designation.
01h,02oct92,srh  added ioctl(FIOGETFL) to return file's open mode
01g,18jul92,smb  Changed errno.h to errnoLib.h.
01f,04jul92,jcf  scalable/ANSI/cleanup effort.
01e,26may92,rrr  the tree shuffle
01d,04oct91,rrr  passed through the ansification filter
		  -changed READ, WRITE and UPDATE to O_RDONLY O_WRONLY and O_RDWR
		  -changed copyright notice
01c,11apr91,jdi  documentation cleanup; doc review by dnw.
01b,17apr90,jcf  lint.
01a,13jun89,gae  written.
*/

/*
DESCRIPTION
This driver allows the I/O system to access memory directly as a
pseudo-I/O device.  Memory location and size are specified when the
device is created.  This feature is useful when data must be preserved
between boots of VxWorks or when sharing data between CPUs.

Additionally, it can be used to build some files into a VxWorks binary
image (having first converted them to data arrays in C source files,
using a utility such as memdrvbuild), and then mount them in the
filesystem; this is a simple way of delivering some non-changing files
with VxWorks.  For example, a system with an integrated web server may
use this technique to build some HTML and associated content files
into VxWorks.

memDrv can be used to simply provide a high-level method of reading
and writing bytes in absolute memory locations through I/O calls.  It
can also be used to implement a simple, essentially read-only
filesystem (exsisting files can be rewritten within their existing
sizes); directory searches and a limited set of IOCTL calls (including
stat()) are supported.

USER-CALLABLE ROUTINES
Most of the routines in this driver are accessible only through the
I/O system.  Four routines, however, can be called directly: memDrv()
to initialize the driver, memDevCreate() and memDevCreateDir() to
create devices, and memDevDelete() to delete devices.

Before using the driver, it must be initialized by calling memDrv().
This routine should be called only once, before any reads, writes, or
memDevCreate() calls.  It may be called from usrRoot() in usrConfig.c
or at some later point.

IOCTL FUNCTIONS
The dosFs file system supports the following ioctl() functions.  The
functions listed are defined in the header ioLib.h.  Unless stated
otherwise, the file descriptor used for these functions may be any file
descriptor which is opened to a file or directory on the volume or to 
the volume itself.

.iP "FIOGETFL"
Copies to <flags> the open mode flags of the file (O_RDONLY,
O_WRONLY, O_RDWR):
.CS
    int flags;
    status = ioctl (fd, FIOGETFL, &flags);
.CE
.iP "FIOSEEK"
Sets the current byte offset in the file to the position specified by
<newOffset>:
.CS
    status = ioctl (fd, FIOSEEK, newOffset);
.CE
The FIOSEEK offset is always relative to the beginning of the file.  The
offset, if any, given at open time by using pseudo-file name is overridden.
.iP "FIOWHERE"
Returns the current byte position in the file.  This is the byte offset of
the next byte to be read or written.  It takes no additional argument:
.CS
    position = ioctl (fd, FIOWHERE, 0);
.CE
.iP "FIONREAD"
Copies to <unreadCount> the number of unread bytes in the file:
.CS
    int unreadCount;
    status = ioctl (fd, FIONREAD, &unreadCount);
.CE
.iP "FIOREADDIR"
Reads the next directory entry.  The argument <dirStruct> is a DIR
directory descriptor.  Normally, the readdir() routine is used to read a
directory, rather than using the FIOREADDIR function directly.  See dirLib.
.CS
    DIR dirStruct;
    fd = open ("directory", O_RDONLY);
    status = ioctl (fd, FIOREADDIR, &dirStruct);
.CE
.iP "FIOFSTATGET"
Gets file status information (directory entry data).  The argument
<statStruct> is a pointer to a stat structure that is filled with data
describing the specified file.  File inode numbers, user and group
IDs, and times are not supported (returned as 0).

Normally, the stat() or fstat() routine is used to obtain file
information, rather than using the FIOFSTATGET function directly.  See
dirLib.
.CS
    struct stat statStruct;
    fd = open ("file", O_RDONLY);
    status = ioctl (fd, FIOFSTATGET, &statStruct);
.CE
.LP
Any other ioctl() function codes will return error status.

SEE ALSO:
.pG "I/O System"

LINTLIBRARY
*/

#include "vxWorks.h"
#include "ioLib.h"
#include "iosLib.h"
#include "stat.h"
#include "dirent.h"
#include "memLib.h"
#include "errnoLib.h"
#include "string.h"
#include "stdlib.h"
#include "stdio.h"
#include "memDrv.h"


typedef struct		  /* MEM_DEV - memory device descriptor */
    {
    DEV_HDR devHdr;
    MEM_DRV_DIRENTRY dir; /* Contents of this memDrv device */
    int allowOffset;	  /* Allow files to be opened with an offset. */
    } MEM_DEV;


typedef struct		  /* MEM_FILE_DESC - memory file descriptor */
    {
    MEM_DEV *pDevice;	  /* The memory device of this file */
    MEM_DRV_DIRENTRY *pDir;/* Directory entry for this file */
    int offset;		  /* current position */
    int mode;		  /* O_RDONLY, O_WRONLY, O_RDWR */
    } MEM_FILE_DESC;


LOCAL int memDrvNum;	  /* driver number of memory driver */


/* forward declarations */

LOCAL MEM_DRV_DIRENTRY *memFindFile ();
LOCAL MEM_FILE_DESC *memOpen ();
LOCAL STATUS memFileStatGet ();
LOCAL int memRead ();
LOCAL int memWrite ();
LOCAL int memClose ();
LOCAL STATUS memIoctl ();


/*******************************************************************************
*
* memDrv - install a memory driver
*
* This routine initializes the memory driver.  It must be called first,
* before any other routine in the driver.
*
* RETURNS: OK, or ERROR if the I/O system cannot install the driver.
*/

STATUS memDrv (void)

    {
    if (memDrvNum > 0)
	return (OK);	/* driver already installed */

    memDrvNum = iosDrvInstall ((FUNCPTR) memOpen, (FUNCPTR) NULL,
			       (FUNCPTR) memOpen, memClose,
			       memRead, memWrite, memIoctl);

    return (memDrvNum == ERROR ? ERROR : OK);
    }
/*******************************************************************************
*
* memDevCreate - create a memory device
*
* This routine creates a memory device containing a single file.
* Memory for the device is simply an absolute memory location
* beginning at <base>.  The <length> parameter indicates the size of
* memory.
*
* For example, to create the device "/mem/cpu0/", a device for accessing
* the entire memory of the local processor, the proper call would be:
* .CS
*     memDevCreate ("/mem/cpu0/", 0, sysMemTop())
* .CE
* The device is created with the specified name, start location, and size.
*
* To open a file descriptor to the memory, use open().  Specify a
* pseudo-file name of the byte offset desired, or open the "raw" file at the
* beginning and specify a position to seek to.  For example, the following
* call to open() allows memory to be read starting at decimal offset 1000.
* .CS
*     -> fd = open ("/mem/cpu0/1000", O_RDONLY, 0)
* .CE
* Pseudo-file name offsets are scanned with "%d".
*
* CAVEAT
* The FIOSEEK operation overrides the offset given via the pseudo-file name
* at open time.
*
* EXAMPLE
* Consider a system configured with two CPUs in the backplane and a separate
* dual-ported memory board, each with 1 megabyte of memory.  The first CPU
* is mapped at VMEbus address 0x00400000 (4 Meg.), the second at bus
* address 0x00800000 (8 Meg.), the dual-ported memory board at 0x00c00000
* (12 Meg.).  Three devices can be created on each CPU as follows.  On
* processor 0:
* .CS
*     -> memDevCreate ("/mem/local/", 0, sysMemTop())
*     ...
*     -> memDevCreate ("/mem/cpu1/", 0x00800000, 0x00100000)
*     ...
*     -> memDevCreate ("/mem/share/", 0x00c00000, 0x00100000)
* .CE
* On processor 1:
* .CS
*     -> memDevCreate ("/mem/local/", 0, sysMemTop())
*     ...
*     -> memDevCreate ("/mem/cpu0/", 0x00400000, 0x00100000)
*     ...
*     -> memDevCreate ("/mem/share/", 0x00c00000, 0x00100000)
* .CE
* Processor 0 has a local disk.  Data or an object module needs to be
* passed from processor 0 to processor 1.  To accomplish this, processor 0
* first calls:
* .CS
*     -> copy </disk1/module.o >/mem/share/0
* .CE
* Processor 1 can then be given the load command:
* .CS
*     -> ld </mem/share/0
* .CE
*
* RETURNS: OK, or ERROR if memory is insufficient or the I/O system cannot add
*          the device.
*
* ERRNO: S_ioLib_NO_DRIVER
*
*/

STATUS memDevCreate 
    (
    char * name,		/* device name		    */
    char * base,		/* where to start in memory */
    int    length		/* number of bytes	    */
    )
    {
    STATUS status;
    FAST MEM_DEV *pMemDv;

    if (memDrvNum < 1)
        {
        errnoSet (S_ioLib_NO_DRIVER);
        return (ERROR);
        }

    if ((pMemDv = (MEM_DEV *) calloc (1, sizeof (MEM_DEV))) == NULL)
	return (ERROR);

    pMemDv->dir.name  = "";
    pMemDv->dir.base  = base;
    pMemDv->dir.pDir  = NULL;
    pMemDv->dir.length = length;
    pMemDv->allowOffset = 1;

    /*
     * XXX
     * Specify byte,word,long accesses.
     * Semaphore read/write? */

    status = iosDevAdd ((DEV_HDR *) pMemDv, name, memDrvNum);

    if (status == ERROR)
	free ((char *) pMemDv);

    return (status);
    }

/*******************************************************************************
*
* memDevCreateDir - create a memory device for multiple files
*
* This routine creates a memory device for a collection of files
* organised into directories.  The given array of directory entry
* records describes a number of files, some of which may be directories,
* represented by their own directory entry arrays.  The structure may
* be arbitrarily deep.  This effectively allows a filesystem to
* be created and installed in VxWorks, for essentially read-only use.
* The filesystem structure can be created on the host using the
* memdrvbuild utility.
*
* Note that the array supplied is not copied; a reference to it is
* kept.  This array should not be modified after being passed to
* memDevCreateDir.
*
* RETURNS:  OK, or ERROR if memory is insufficient or the I/O system cannot
*           add the device.
*
* ERRNO: S_ioLib_NO_DRIVER
*
*/

STATUS memDevCreateDir
    (
    char * name,		/* device name		    */
    MEM_DRV_DIRENTRY * files,	/* array of dir. entries - not copied */
    int    numFiles		/* number of entries	    */
    )
    {
    STATUS status;
    FAST MEM_DEV *pMemDv;

    if (memDrvNum < 1)
        {
        errnoSet (S_ioLib_NO_DRIVER);
        return (ERROR);
        }

    if ((pMemDv = (MEM_DEV *) calloc (1, sizeof (MEM_DEV))) == NULL)
	return (ERROR);

    pMemDv->dir.name  = "";
    pMemDv->dir.base  = NULL;
    pMemDv->dir.pDir  = files;
    pMemDv->dir.length = numFiles;

    /*
     * Let's not allow the "offset" notation for opening files, at
     * least for now, when we have "real" subdirectories.
     */
    pMemDv->allowOffset = 0;

    /*
     * XXX
     * Specify byte,word,long accesses.
     * Semaphore read/write?
     */

    status = iosDevAdd ((DEV_HDR *) pMemDv, name, memDrvNum);

    if (status == ERROR)
	free ((char *) pMemDv);

    return (status);
    }

/*******************************************************************************
*
* memDevDelete - delete a memory device
*
* This routine deletes a memory device containing a single file or a
* collection of files. The device is deleted with it own name.
*
* For example, to delete the device created by memDevCreate ("/mem/cpu0/", 0,
* sysMemTop()), the proper call would be:
* .CS
* 	memDevDelete ("/mem/cpu0/");
* .CE
*  
* RETURNS: OK, or ERROR if the device doesn't exist.
*/

STATUS memDevDelete
    (
    char * name			/* device name */
    )
    {
    DEV_HDR * pDevHdr;

    /* get the device pointer corresponding to the device name */
 
    if ((pDevHdr = iosDevFind (name, NULL)) == NULL)
        return (ERROR);

    /* delete the device from the I/O system */

    iosDevDelete (pDevHdr);

    /* free the device pointer */

    free ((MEM_DEV *) pDevHdr);

    return (OK);
    }

/*******************************************************************************
*
* memFindFile - find a memory file by name
*
* RETURNS: The file descriptor number, or ERROR if the name is not a
*  valid number.
*/

LOCAL MEM_DRV_DIRENTRY *memFindFile
    (
    MEM_DEV *pMemDv,		/* Device we are looking in. */
    char *name,			/* name of file to find */
    MEM_DRV_DIRENTRY *pDir,	/* pointer to memory device descriptor */
    int *pOffset			/* returned file pOffset */
    )
    {
    MEM_DRV_DIRENTRY *pFile = NULL;

    *pOffset = 0;

    /* Protect against NULL. */
    if (name == NULL)
	name = "";
    while (*name == '/')
	++name;

    if (strcmp (pDir->name, name) == 0)
	{
	pFile = pDir;
	}
    else if (strncmp (pDir->name, name, strlen (pDir->name)) == 0)
	{
	int index;

	name += strlen (pDir->name);
	if (pDir->pDir != NULL)
	    {
	    /* Search for the referenced file in this directory. */
	    for (index = 0; index < pDir->length; ++index)
		{
		pFile = memFindFile (pMemDv, name, &pDir->pDir[index], pOffset);
		if (pFile != NULL) break;
		}
	    }
	else if (pMemDv->allowOffset)
	    {
	    int off = 0;

	    while (*name == '/')
		++name;
	    if (sscanf (name, "%d", &off) == 1)
		{
		pFile = pDir;
		*pOffset = off;
		}
	    }
	}

    return pFile;
    }
/*******************************************************************************
*
* memOpen - open a memory file
*
* RETURNS: The file descriptor number, or ERROR if the name is not a
*          valid number.
*
* ERRNO: EINVAL
*
*/

LOCAL MEM_FILE_DESC *memOpen
    (
    MEM_DEV *pMemDv,	/* pointer to memory device descriptor */
    char *name,		/* name of file to open (a number) */
    int mode		/* access mode (O_RDONLY,O_WRONLY,O_RDWR) */
    )
    {
    MEM_DRV_DIRENTRY *pFile = NULL;
    MEM_FILE_DESC *pMemFd = NULL;
    int offset = 0;
    int isDir = 0;

    pFile = memFindFile (pMemDv, name, &pMemDv->dir, &offset);

    if (pFile != NULL)
	{
	/* Directories open for read only. */
	isDir = (pFile->pDir != NULL);
	if (!isDir || mode == O_RDONLY)
	    {
	    /* Get a free file descriptor */
	    pMemFd = (MEM_FILE_DESC *) calloc (1, sizeof (MEM_FILE_DESC));
	    if (pMemFd != NULL)
		{
		pMemFd->pDevice = pMemDv;
		pMemFd->pDir = pFile;
		pMemFd->offset = offset;
		pMemFd->mode   = mode;
		}
	    }
	}

    if (pMemFd != NULL)
	return pMemFd;
    else
	{
	errnoSet (EINVAL);
	return (MEM_FILE_DESC *) ERROR;
	}
    }

/*******************************************************************************
*
* memFileStatGet - get file status (directory entry data)
*
* This routine is called via an ioctl() call, using the FIOFSTATGET
* function code.  The passed stat structure is filled, using data
* obtained from the directory entry which describes the file.
*
* RETURNS: ERROR or OK
*
* ERRNO: EINVAL
*
*/

LOCAL STATUS memFileStatGet
    (
    MEM_FILE_DESC * pfd,	/* pointer to file descriptor */
    struct stat	* pStat		/* structure to fill with data */
    )
    {
    MEM_DEV *pMemDv = pfd->pDevice;	/* pointer to device */
    MEM_DRV_DIRENTRY *pDir = pfd->pDir;	/* pointer to file info */
    int isDir = 0;

    if (pStat == NULL || pDir == NULL)
	{
	errnoSet (EINVAL);
	return ERROR;
	}

    bzero ((char *) pStat, sizeof (struct stat)); /* zero out stat struct */
    isDir = (pDir->pDir != NULL);

    /* Fill stat structure */

    pStat->st_dev     = (ULONG) pMemDv;		/* device ID = DEV_HDR addr */
    pStat->st_ino     = 0;			/* no file serial number */
    pStat->st_nlink   = 1;			/* always only one link */
    pStat->st_uid     = 0;			/* no user ID */
    pStat->st_gid     = 0;			/* no group ID */
    pStat->st_rdev    = 0;			/* no special device ID */
    pStat->st_size    = isDir ? 0 : pDir->length; /* file size, in bytes */
    pStat->st_atime   = 0;			/* no last-access time */
    pStat->st_mtime   = 0;			/* no last-modified time */
    pStat->st_ctime   = 0;			/* no last-change time */
    pStat->st_attrib  = 0;			/* file attribute byte */
    
    /*
     * Set file access permissions to be read/write by all; for
     * directories, add exec (search).
     */
    pStat->st_mode |= (S_IRUSR | S_IRGRP | S_IROTH | 
		       S_IWUSR | S_IWGRP | S_IWOTH);
    if (isDir)
	pStat->st_mode |= (S_IXUSR | S_IXGRP | S_IXOTH);

    /* Is it a file or directory? */
    if (isDir)
	pStat->st_mode |= S_IFDIR;		/*  it is a directory */
    else
	pStat->st_mode |= S_IFREG;		/*  it is a regular file */

    return (OK);
    }
/*******************************************************************************
*
* memRead - read from a memory file
*
* RETURNS: ERROR if the file open mode is O_WRONLY; otherwise, the number of
*          bytes read (may be less than the amount requested), or 0 if the
*          file is at EOF.
*
* ERRNO: EINVAL, EISDIR
*
*/

LOCAL int memRead
    (
    MEM_FILE_DESC *pfd,	/* file descriptor of file to read */
    char *buffer,	/* buffer to receive data */
    int maxbytes	/* max bytes to read in to buffer */
    )
    {
    MEM_DRV_DIRENTRY *pDir = pfd->pDir;	/* pointer to file info */
    int lLeft;

    /* Fail if the mode is invalid, or if it's a directory. */
    if (pfd->mode == O_WRONLY)
	{
	errnoSet (EINVAL);
	return (ERROR);
	}
    if (pDir == NULL || pDir->pDir != NULL)
	{
	errnoSet (EISDIR);
	return (ERROR);
	}

    /* calculate #bytes left to read */
    lLeft = pDir->length - pfd->offset;
    if (lLeft < 0) lLeft = 0;

    /* limit maxbytes to minimum of bytes left or buffer length */
    if (maxbytes > lLeft) maxbytes = lLeft;

    if (maxbytes > 0)
        {
        bcopy (pDir->base + pfd->offset, buffer, maxbytes);
        pfd->offset += maxbytes;
        }

    return (maxbytes);
    }
/*******************************************************************************
*
* memWrite - write to a memory file
*
* RETURNS: The number of bytes written, or ERROR if past the end of memory or
*          is O_RDONLY only.
*
* ERRNO: EINVAL, EISDIR
*
*/

LOCAL int memWrite
    (
    MEM_FILE_DESC *pfd,	/* file descriptor of file to write */
    char *buffer,	/* buffer to be written */
    int nbytes		/* number of bytes to write from buffer */
    )
    {
    MEM_DRV_DIRENTRY *pDir = pfd->pDir;	/* pointer to file info */
    int lLeft;

    /* Fail if the mode is invalid, or if it's a directory. */
    if (pfd->mode == O_RDONLY)
	{
	errnoSet (EINVAL);
	return (ERROR);
	}
    if (pDir == NULL || pDir->pDir != NULL)
	{
	errnoSet (EISDIR);
	return (ERROR);
	}

    /* calculate #bytes left to write */
    lLeft = pDir->length - pfd->offset;
    if (lLeft < 0) lLeft = 0;

    /* If write is too large, truncate it */
    if (nbytes > lLeft) nbytes = lLeft;

    if (nbytes > 0)
        {
	bcopy (buffer, pDir->base + pfd->offset, nbytes);
	pfd->offset += nbytes;
        }

    return (nbytes);
    }
/*******************************************************************************
*
* memIoctl - do device specific control function
*
* Only the FIONREAD, FIOSEEK, FIOWHERE, FIOGETFL, FIOFSTATGET, and
* FIOREADDIR options are supported.
*
* RETURNS: OK, or ERROR if seeking passed the end of memory.
*
* ERRNO: EINVAL, S_ioLib_UNKNOWN_REQUEST
*
*/

LOCAL STATUS memIoctl (pfd, function, arg)
    FAST MEM_FILE_DESC *pfd;	/* descriptor to control */
    FAST int function;		/* function code */
    int	arg;			/* some argument */

    {
    MEM_DRV_DIRENTRY *pDir = pfd->pDir;	/* pointer to file info */
    DIR *dirp;
    int isDir = 0;
    int status = OK;

    if (pDir == NULL)
	{
	errnoSet (EINVAL);
	status = ERROR;
	}
    else
	{
	isDir = (pDir->pDir != NULL);
	switch (function)
	    {
	    case FIONREAD:
		if (isDir)
		    {
		    errnoSet (EINVAL);
		    status = ERROR;
		    }
		else
		    {
		    *((int *) arg) = pDir->length - pfd->offset;
		    }
		break;

	    case FIOSEEK:
		if (isDir || arg > pDir->length)
		    {
		    errnoSet (EINVAL);
		    status = ERROR;
		    }
		else
		    {
		    pfd->offset = arg;
		    }
		break;

	    case FIOWHERE:
		if (isDir)
		    status = 0;
		else
		    status = pfd->offset;
		break;

	    case FIOGETFL:
		*((int *) arg) = pfd->mode;
		break;

	    case FIOFSTATGET:
		status = memFileStatGet (pfd, (struct stat *) arg);
		break;

	    case FIOREADDIR:
		/*
		 * The index of the directory entry required is passed in via
		 * dirp->dd_cookie.  We're supposed to return the name in
		 * dirp->dd_dirent.
		 */
		dirp = (DIR *) arg;
		if (!isDir || dirp->dd_cookie >= pDir->length)
		    {
		    errnoSet (EINVAL);
		    status = ERROR;
		    }
		else
		    {
		    strcpy (dirp->dd_dirent.d_name, pDir->pDir[dirp->dd_cookie].name);
		    ++dirp->dd_cookie;
		    }
		break;

	    default:
		errnoSet (S_ioLib_UNKNOWN_REQUEST);
		status = ERROR;
		break;
	    }
	}

    return (status);
    }
/*******************************************************************************
*
* memClose - close a memory file
*
* RETURNS: OK, or ERROR if file couldn't be flushed or entry couldn't 
*  be found.
*/

LOCAL STATUS memClose (pfd)
    MEM_FILE_DESC *pfd;	/* file descriptor of file to close */

    {
    free (pfd);
    return OK;
    }
