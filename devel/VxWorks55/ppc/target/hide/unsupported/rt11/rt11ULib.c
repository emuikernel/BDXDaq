/* rt11ULib.c - RT-11 file I/O library for UNIX */

/* Copyright 1984,1985,1986,1987,1988,1989 Wind River Systems, Inc. */
extern char copyright_wind_river[]; static char *copyright=copyright_wind_river;

/*
modification history
--------------------
01k,06jun88,dnw  changed rtLib to rt11Lib.
01j,09nov87,ecs  added dummy iosDevFind.
01i,03nov87,ecs  documentation.
01h,20dec86,dnw  changed to not get include files from default directories.
01g,20jul85,jlf  Documentation.
01f,19jun85,jlf  Removed include of stdio.h.
01e,29may85,jlf  changed block i/o routines to sector i/o.
01d,10sep84,jlf  added copyright, comments.  Removed GLOBAL.
01c,01aug84,dnw  changed to use secsPerTrack and bytesPerSec in RT_UNIX_DEV,
		   instead of constants.
01b,11jun84,dnw  changed from read/write sector to read/write block.
		 changed to use RT_UNIX_DEV now in rtULib.h.
01a,24aug83,dnw  extracted from rtLib.c
*/

/*
This library makes it possible to use rt11Lib (1) under UNIX.  It provides the
rdSec and wrtSec routines that rt11Lib (1) uses, and they both go through UNIX.

In order to use these, the fd in the descriptor must refer to a UNIX file
that has been opened.

SEE ALSO: rt11Lib (1)
*/

/* LINTLIBRARY */

#include "vxWorks.h"
#include "ctype.h"
#include "rt11ULib.h"


/***********************************************************************
*
* rtURdSec - read a sector from an RT-11 volume
*
* This routine reads the specified sector from the specified volume.
*
* RETURNS: status (OK, ERROR)
*/

STATUS rtURdSec (pRtUDev, secNum, buffer)
    RT_UNIX_DEV *pRtUDev;	/* pointer to volume descriptor */
    FAST int secNum;		/* number of block to read */
    char *buffer;		/* buffer to receive block */

    {
    FAST RT_VOL_DESC *pRtVol = & (pRtUDev->rtvol);

    /* Add a one track offset to the sector number */

    secNum += pRtVol->vd_secTrack;

    /* read sector */

    lseek (pRtUDev->fd, (long) (secNum * pRtVol->vd_bytesPerSec), 0);

    if (read (pRtUDev->fd, buffer, pRtVol->vd_bytesPerSec) < OK)
	return (ERROR);

    return (OK);
    }
/***********************************************************************
*
* rtUWrtSec - write a sector to an RT-11 volume
*
* This routine writes the specified sector to the specified volume.
*
* RETURNS: status (OK, ERROR)
*/

STATUS rtUWrtSec (pRtUDev, secNum, buffer)
    RT_UNIX_DEV *pRtUDev;	/* pointer to volume descriptor */
    int secNum;			/* number of sector to write */
    char *buffer;		/* buffer to write to block */

    {
    FAST RT_VOL_DESC *pRtVol = & (pRtUDev->rtvol);

    /* Add a one track offset to the sector number */

    secNum += pRtVol->vd_secTrack;

    /* write sector */

    lseek (pRtUDev->fd, (long) (secNum * pRtVol->vd_bytesPerSec), 0);

    if (write (pRtUDev->fd, buffer, pRtVol->vd_bytesPerSec) < OK)
	return (ERROR);

    return (OK);
    }
/*******************************************************************************
*
* iosDevFind - find an I/O device in the device list
*
* This is a dummy version of iosDevFind. It should be in iosULib.c, but that
* doesn't exist.
*
* RETURNS:
*    NULL, always.
*
* ARGSUSED
*/

DEV_HDR *iosDevFind (name, pNameTail)
    char *name;			/* Name of the device */
    char **pNameTail;		/* Where to return ptr to tail of the name */

    {
    return (NULL);
    }
