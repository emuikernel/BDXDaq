/* rtrm.c - delete RT-11 files */

static char *copyright = "Copyright 1984-1988, Wind River Systems, Inc.";

/*
modification history
--------------------
01l,12sep88,gae  documentation.
01k,06jun88,dnw  changed rtLib to rt11Lib.
01j,30may88,dnw  changed to v4 names.
01i,08mar88,gae  changed to include <time.h> instead of <sys/time.h> if
		   HOST_IRIS or HOST_HK.
01h,10mar87,gae  added rtFmt (-r) flag.
01g,17feb87,gae  added nentries parameter to rtDevInit().  Placated lint.
01f,06sep86,jlf  documentation.
01e,19sep85,jlf  removed call to rtVolMount, which now automatically happens
		 in rtLib.
01d,10sep85,jlf  added reset function (null) to rtDevInit.
01c,16jul85,jlf  Added include of stdio.h.
01b,05jun85,jlf  changed for new rtDevInit call.
01a,08mar85,rdc  written
*/


/*
SYNOPSIS
rtrm [-b <b/s>] [-s <s/t>] [-t <t/d>] [-r] file1 ... filen

DESCRIPTION
Rtrm removes RT-11 files.  If given a list of files, it will attempt to 
delete each one, even if prior deletes fail.  Each file must have
a proper device prefix - this can be handled by using the csh
construct:  rtrm device/{bar1,bar2,bar3}.

The command line switches -b, -s, and -t set bytes per sector, sectors
per track, and tracks per disk, respectively.  These default to 128,
26, and 77, which are the correct numbers for a single density 8" floppy.
The -r flag turns off the usual RT-11 interleave.

NOTE
You may have to be the superuser to access the disk special
files as required by this program.

EXAMPLES
.CS
	rtrm /dev/flop.sd.8/{file1,file2,file3}
.CE
removes the files file1, file2, file3 from the single-density, 8" floppy
disk in /dev/flop.sd.8.
.CS
	rtrm -b 512 -s 15 /dev/flop.dd.8/file1
.CE
removes the file file1 from the double density 8" RT-11 disk on /dev/flop.dd.8.

SEE ALSO: rtinit(4), rtls(4), rt11Lib(1)
*/


#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>

#if (defined(HOST_HK) || defined(HOST_IRIS))
#include <time.h>
#else
#include <sys/time.h>
#endif (HOST_HK)

#include "vxWorks.h"
#include "rt11ULib.h"


LOCAL int tracksPerDisk	= 77;		/* defaults for single density disks */
LOCAL int secsPerTrack	= 26;
LOCAL int bytesPerSec	= 128;
LOCAL char usage [] =
    "Usage: rtrm [-b <b/s>] [-s <s/t>] [-t <t/d>] [-r] dev/file1 ... dev/filen";


/*******************************************************************************
*
* rtrm - delete rt-11 files 
*/

main (argc, argv)
    int argc;
    char *argv[];

    {
    FAST int argIndex = 1; 	/* used to index through argv */
    char *dirname;		/* pointer to directory name */
    int i;			/* index into dirname */
    static RT_UNIX_DEV rtUDev;	/* rt-11 device descriptor for unix */
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
	    /* Read bytes per sector */

	    if (sscanf (argv [++argIndex], "%d", &bytesPerSec) == 0)
		error (usage, (char *)NULL);

	    argIndex++;
	    }
	else if (strcmp (argv [argIndex], "-b") == 0)
	    {
	    /* turn off RT-11 format */

	    rtFmt = FALSE;

	    argIndex++;
	    }
	else if (*(argv [argIndex]) == '-')
	    /* Invalid switch */

	    error (usage, (char *)NULL);

	else
	    /* No more switches.  The rest of the arguments are file names */

	    break;
	}


    /* initialize rt-11 package */

    rt11Init (2);		/* allow 2 open files */


    for ( ; argIndex <= (argc - 1); argIndex++)	/* for each input file */
	{
	dirname = argv[argIndex];

	/* find last slash in dirname, if any */

	for (i = strlen (dirname) - 1; dirname[i] != '/' && i >= 0; i--);

	if (i <= 0)		/* no device specified */
	    {
	    fprintf (stderr, "%s: must specify device name. \n",dirname);
	    continue;
	    }
    
	dirname[i] = '\0';		/* divide string into directory name
					   and file name */

	if ((rtUDev.fd = open (dirname, UPDATE)) < 0)
	    {
	    fprintf (stderr, "open: can't open %s\n", dirname);
	    continue;
	    }

	rt11DevInit ((RT_VOL_DESC *) &rtUDev, bytesPerSec, secsPerTrack,
		     ((tracksPerDisk - 1) * secsPerTrack), rtFmt,
		     RT_FILES_FOR_2_BLOCK_SEG,
		     rtURdSec, rtUWrtSec, (FUNCPTR) NULL);

	if (rt11Delete ((RT_VOL_DESC *) &rtUDev, &dirname[i + 1]) == ERROR)
	    {
	    fprintf (stderr, "rt11Delete: %s file not found.\n", &dirname[i+1]);
	    }
	
	close(rtUDev.fd);
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

