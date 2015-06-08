/* rtls.c - list directory of RT-11 volume */

static char *copyright = "Copyright 1984-1988, Wind River Systems, Inc.";

/*
modification history
--------------------
01r,12sep88,gae  documentation.
01q,06jun88,dnw  changed rtLib to rt11Lib.
01p,30may88,dnw  changed to v4 names.
01o,26mar87,gae  replaced rtDirEntry() with rtOpen() and rtIoctl().
01n,10mar87,gae  added rtFmt (-r) flag.
01m,17feb87,gae  added nentries parameter to rtDevInit().  Placated lint.
01l,19sep85,jlf  removed call to rtVolMount, which now automatically happens
		 in rtLib.
01k,10sep85,jlf  added reset function (null) to rtDevInit.
01j,22jul85,jlf  documentation.
01i,05jun85,jlf  Changed for new rtDevInit calling seq.
01h,19sep84,jlf  Added copyright and cleaned up comments
01g,10sep84,dnw  Changed to use rtDirEntry instead of rtGetEntry.
		 Changed to print in same format as target "ls".
01f,03aug84,jlf  Changed to single character options.
01e,01aug84,dnw & jlf  removed references to single density disk constants.
		 Added options for different disk formats.
01d,12jul84,ecs  fixed printEntry to handle date right.
01c,10jun84,dnw  changed to use new rtLib.
01b,12aug83,dnw  changed to use cleaned-up interface to rtLib.
		 improved error reporting and changed to exit w/ERROR
		   in all error cases.
01a,20Feb83,dnw  written
*/

/*
SYNOPSIS
rtls  [-b <b/s>] [-s <s/t>] [-t <t/d>] [-r] <special>

DESCRIPTION
This program lists the directory of an RT-11 disk.
<special> is the special file for the disk drive containing the
RT-11 disk.  The files are listed in the format of standard ls with
the long (-l) option.  Much of the information is dummy stuff to
conform to the ls format.  The meaningful info is the file size,
date, and name.  Empty slots on the disk are displayed with name
"(EMPTY)".

The options -b, -s, and -t set bytes per sector, sectors per track,
and tracks per disk, respectively.
These default to 128, 26, and 77, which are the correct
numbers for a single density 8" floppy.  The -r flag turns off the usual
RT-11 interleave.

NOTE
In general you have to be the superuser to access the disk special
files as required by this program.

EXAMPLES
.CS
	rtls /dev/flop.sd.8
.CE
lists the directory of the single density 8" RT-11 diskette on /dev/flop.sd.8.
.CS
	rtls -b 512 -s 15 /dev/flop.dd.8
.CE
lists the directory of the double density 8" RT-11 disk on /dev/flop.dd.8.
.CS
	rtls -b 512 -s 9 -t 35 /dev/flop.dd.5
.CE
lists the directory of the double density 5 1/4" RT-11 disk on /dev/flop.dd.5.

DEFICIENCIES
The only listing format is the ls long (-l) format.  This should
be an option and other ls formats should be supported.  Also the
files are listed in the order they appear on the disk rather than
sorted alphabetically.

You can't read a double density disk produced by a DEC machine, since
it can't be described in the options available.

SEE ALSO: "Cross Development", rtinit(4), rtcp(4), rt11Lib(1)
*/

#include <stdio.h>

#include "vxWorks.h"
#include "rt11ULib.h"

LOCAL char usage [] =
    "Usage: rtls [-b <b/s>] [-s <s/t>] [-t <t/d>] [-r] <device>";


/*******************************************************************************
*
* rtls - list directory of RT-11 volume
*/

main (argc, argv)
    int argc;
    char *argv[];

    {
    RT_UNIX_DEV rtUDev;			/* RT-11 device descriptor for unix */
    RT_FILE_DESC *pFd;
    REQ_DIR_ENTRY rde;
    int tracksPerDisk	= 77;		/* defaults for single density disks */
    int secsPerTrack	= 26;
    int bytesPerSec	= 128;
    FAST int argIndex	= 1;		/* used to index through argv array */
    BOOL rtFmt = TRUE;

    static char *month[] = {"   ", "Jan", "Feb", "Mar", "Apr", "May", "Jun",
			    "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

    /* Crack the arguments */

    if (argc < 2)
	error (usage, (char *)NULL);

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
	    /* turn of RT-11 format */

	    rtFmt = FALSE;

	    argIndex++;
	    }
	else
	    error (usage, (char *)NULL);
	}

    /* Make sure there is exactly one argument left */

    if (argIndex != (argc - 1))
	error (usage, (char *)NULL);


    /* initialize RT-11 stuff to read the disk */

    if ((rtUDev.fd = open (argv [argIndex], READ)) < 0)
	error ("rtls: can't open %s", argv [argIndex]);

    rt11Init (1);

    rt11DevInit ((RT_VOL_DESC *) &rtUDev, bytesPerSec, secsPerTrack,
       	         ((tracksPerDisk - 1) * secsPerTrack), rtFmt,  
	         RT_FILES_FOR_2_BLOCK_SEG,
	         rtURdSec, rtUWrtSec, (FUNCPTR) NULL);

    /* open "raw" device */

    pFd = rt11Open ((RT_VOL_DESC *) &rtUDev, "", READ);

    if (pFd == NULL)
	error ("rtls: can't open device?", (char *)NULL);

    /* print each entry in directory */

    printf (" size     date     name\n");
    printf (" ----     ----     ----\n");

    for (rde.entryNum = 0;
	 rt11Ioctl (pFd, FIODIRENTRY, &rde) == OK;
	 ++rde.entryNum)
	{
	if (rde.name[0] == EOS)
	    printf ("%6d             (EMPTY)\n", rde.nChars);
	else
	    printf ("%6d %s %2d %4d %s\n", rde.nChars, month[rde.month],
		   rde.day, rde.year, rde.name);
	}

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
