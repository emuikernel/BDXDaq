/* rtinit.c - initialize directory of RT-11 volume */

static char *copyright = "Copyright 1984-1988, Wind River Systems, Inc.";

/*
modification history
--------------------
01o,12sep88,gae  documentation.
01n,06jun88,dnw  changed rtLib to rt11Lib.
01m,30may88,dnw  changed to v4 names.
01l,26mar87,gae  replaced rtVolInit() with rtOpen() and rtIoctl().
01k,10mar87,gae  added rtFmt (-r) flag.
01j,17feb87,gae  added nentries parameter to rtDevInit().  Placated lint.
01i,10sep85,jlf  added reset function (null) to rtDevInit.
01h,22jul85,jlf  Documentation.
01g,05jun85,jlf  Changed for new read/write sector, rather than block, and
		 new rtDevInit calling sequence.
01f,19sep84,jlf  Added copyright and cleaned up comments.
01e,03aug84,jlf  Changed to single character options.
01d,01aug84,dnw&jlf  removed references to single density disk constants.
		 Added options for different disks.
01c,10jun84,dnw  changed to use new rtLib.
01b,12aug83,dnw  changed to new calling sequence to rtInitVol.
01a,21Feb83,dnw  written
*/

/*
SYNOPSIS
rtinit  [-b <b/s>] [-s <s/t>] [-t <t/d>] [-r] <special>

DESCRIPTION
This program initializes the directory of an RT-11 format disk.
<special> is the special file for the disk drive containing the
RT-11 disk. The options -b, -s, and -t are used to set bytes per
sector, sectors per track, and tracks per disk.  They default to
128, 26, and 77 respectively, which are the right numbers for
a single density 8" floppy.  The -r flag turns off the usual RT-11
interleave.

NOTE
You must have read/write access to the disk device referred to.

EXAMPLES
.CS
	rtinit /dev/flop.sd.8
.CE
initializes the single density 8" diskette on /dev/flop.sd.8 to be an
empty RT-11 disk.
.CS
	rtinit -b 512 -s 15 /dev/flop.dd.8
.CE
initializes the double density 8" diskette on /dev/flop.dd.8 to be an
empty RT-11 disk.
.CS
	rtinit -b 512 -s 9 -t 35 /dev/flop.dd.5
.CE
initializes the double density 5 1/4" diskette on /dev/flop.dd.8 to be an
empty RT-11 disk.

DEFICIENCIES
You can't initialize a real double density disk that will work on a DEC
machine.  Single density 8" will work fine.

SEE ALSO: "Cross Development", rtls(4), rtcp(4), rt11Lib(1)
*/

#include <stdio.h>

#include "vxWorks.h"
#include "rt11ULib.h"

LOCAL char usage [] =
    "Usage: rtinit [-b <b/s>] [-s <s/t>] [-t <t/d>] [-r] <device>";


/*******************************************************************************
*
* rtinit - initialize directory of RT-11 volume
*/

main (argc, argv)
    int argc;
    char *argv [];

    {
    RT_UNIX_DEV rtUDev;			/* RT-11 device descriptor for unix */
    RT_FILE_DESC *pFd;
    int tracksPerDisk	= 77;		/* defaults for single density disks */
    int secsPerTrack	= 26;
    int bytesPerSec	= 128;
    int argIndex 	= 1;
    BOOL rtFmt = TRUE;

    if (argc < 2)
	error (usage, (char *)NULL);

    /* Crack the arguments */

    while (argIndex < (argc - 1))
	{
	if (strcmp (argv [argIndex], "-t") == 0)
	    {
	    /* Read tracks per disk */

	    if (sscanf (argv [++argIndex], "%d", &tracksPerDisk) == 0)
		error (usage, (char *)NULL);

	    argIndex++;
	    }
	else if (strcmp (argv [argIndex], "-s") == 0)
	    {
	    /* Read sectors per track */

	    if (sscanf (argv [++argIndex], "%d", &secsPerTrack) == 0)
		error (usage, (char *)NULL);

	    argIndex++;
	    }
	else if (strcmp (argv [argIndex], "-b") == 0)
	    {
	    /* Read bytes per sector per sector */

	    if (sscanf (argv [++argIndex], "%d", &bytesPerSec) == 0)
		error (usage, (char *)NULL);

	    argIndex++;
	    }
	else if (strcmp (argv [argIndex], "-r") == 0)
	    {
	    /* turn off RT-11 format */

	    rtFmt = FALSE;

	    argIndex++;
	    }
	else
	    error (usage, (char *)NULL);
	}
    /* Make sure there is exactly one argument left */

    if (argIndex != (argc - 1))
	error (usage, (char *)NULL);

    if ((rtUDev.fd = open (argv [argIndex], WRITE)) < 0)
	error ("rtinit: can't open %s", argv [argIndex]);

    rt11Init (1);

    rt11DevInit ((RT_VOL_DESC *) &rtUDev, bytesPerSec, secsPerTrack,
       	         ((tracksPerDisk - 1) * secsPerTrack), rtFmt, 
	         RT_FILES_FOR_2_BLOCK_SEG,
	         rtURdSec, rtUWrtSec, (FUNCPTR) NULL);


    /* open "raw" device */

    pFd = rt11Open ((RT_VOL_DESC *) &rtUDev, "", UPDATE);

    if (pFd == NULL)
	error ("rtinit: can't open device?", (char *)NULL);

    if (rt11Ioctl (pFd, FIODISKINIT, NULL) != OK)
	error ("rtinit: cannot initialize RT-11 volume", (char *)NULL);

    exit (OK);
    }
/*******************************************************************************
*
* error - print error message and die
*
* This routine prints an error message on the standard error output and
* aborts the program with the error status ERROR.  The error message is
* output with the specified format with the single specified argument.
*/

LOCAL VOID error (format, arg)
    char *format;	/* format of error message */
    char *arg;		/* argument supplied to format (arbitrary type!) */

    {
    fprintf (stderr, format, arg);
    fprintf (stderr, "\n");
    exit (ERROR);
    }
