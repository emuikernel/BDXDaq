/* rtcp.c - copy RT-11 and/or UNIX files */

static char *copyright = "Copyright 1984-1988, Wind River Systems, Inc.";

/*
modification history
--------------------
01s,12sep88,gae  documentation.
01r,06jun88,dnw  changed rtLib to rt11Lib.
01q,30may88,dnw  changed to v4 names.
01p,08mar88,gae  changed to include <time.h> instead of <sys/time.h> if
		   HOST_IRIS, too.
01o,19feb88,ecs  changed to include <time.h> instead of <sys/time.h> if HOST_HK.
01n,10mar87,gae  added rtFmt (-r) flag.
01m,17feb87,gae  added nentries parameter to rtDevInit().  Placated lint.
01l,19sep85,jlf  removed call to rtVolMount, which now automatically happens
		 in rtLib.
01k,10sep85,jlf  added reset function (null) to rtDevInit.
01j,16jul85,jlf  Added include of stdio.h.  Added local define of STDOUT.
                 documentation.
01i,24may85,jlf  Added -o and -i switches.  Changed for new rtDevInit call.
01h,19sep84,jlf  Added copyright and cleaned up comments
01g,03aug84,jlf  Changed to single character option names.
01h,01aug84,dnw&jlf  removed references to single density disk constants.
		 Added options for different disk formats
01g,09jul84,ecs  removed reference to rfd_entry_num in samefile.
		 added call to rtSetDate.
01f,10jun84,dnw  changed to use new rtLib.
01e,12aug83,dnw  changed to use cleaned-up interface to rtLib.
		 improved error reporting and changed to exit w/ERROR
		   in all error cases.
01d,22apr83,dnw  fixed buildname to concatenate only last simple name to
		   directory name.
01c,15apr83,ecs  disallow copying file onto itself
01b,14apr83,ecs  pass ufd to rtOpen & rtCreate
01a,13apr83,ecs  written
*/


/*
SYNOPSIS
rtcp [-b <b/s>] [-s <s/t>] [-t <t/d>] [-i r|u] [-r] file

rtcp [-b <b/s>] [-s <s/t>] [-t <t/d>] [-i r|u] [-o r|u] [-r] file1 file2

rtcp [-b <b/s>] [-s <s/t>] [-t <t/d>] [-i r|u] [-r] file1 directory

rtcp [-b <b/s>] [-s <s/t>] [-t <t/d>] [-i r|u] [-r] file1 ... directory

DESCRIPTION
This program copies RT-11 or UNIX files onto RT-11 or UNIX files.
With one argument, the file is copied to the standard output.
With two arguments, file1 is copied onto file2; or into directory, retaining
original name.
With three or more arguments, the last argument is assumed to be a directory,
and the files are copied into the directory, retaining their original names.

By default, rtcp assumes that any "normal" UNIX file is, indeed, a UNIX
file, and that any special block-structured device file is an RT-11 device.
The file types of the input and output files can be set explicitly with
the -o and -i switch.  The -r flag turns off the usual rt11 interleave.

The options -b, -s, and -t define the number of bytes per sector, sectors
per track, and tracks per disk on the RT-11 device.  These default to
128, 26, and 77 respectively, which are the right numbers for normal
single density 8" floppy.

Rtcp refuses to copy a UNIX file onto itself.

EXAMPLES
.CS
    rtcp /dev/rt11/gronk.xxx
.CE
copies gronk.xxx on RT-11 volume on /dev/rt11 to stdout.
.CS
    rtcp gronk.c /dev/rt11/honk.c
.CE
copies gronk.c in current directory onto honk.c on RT-11 volume on /dev/rt11.
.CS
    rtcp /dev/rt11/gronk.c /dev/rt11/honk.c
.CE
copies gronk.c on RT-11 volume on /dev/rt11 onto honk.c on same volume.
.CS
    rtcp *.c /dev/rt11
.CE
copies all .c files in the current directory into RT-11 volume on /dev/rt11, with
their original filenames.
.CS
    rtcp -b 512 -s 15 /dev/fd.dd.8/gronk.c
.CE
copies gronk.c from a double density 8" floppy in RT-11 format to stdout.
.CS
    rtcp -b 512 -s 9 -t 35 /dev/fd.dd.5/gronk.c
.CE
copies gronk.c from a double density 5 1/4" floppy in RT-11 format to stdout.
.CS
    rtcp dave.c jerry.c
.CE
copies UNIX file dave.c to UNIX file jerry.c, exactly like UNIX's 'cp'.
.CS
    rtcp -o r dave.c jerry
.CE
copies UNIX file dave.c to file jerry, which is an image of an RT-11 device.

DEFICIENCIES AND WEIRDNESSES
Since expansion of filenames (such as "*.c" to "gronk.c honk.c zonk.c") is done
by the shell, and the shell doesn't know how to find filenames in /dev/rt11,
all RT-11 file names must be fully typed out. A fully functional rtcp should be
able to handle a wild card mechanism, although the shell prevents the use of
the character '*' as that mechanism.

Rtcp allows copying of an RT-11 file onto itself, with destructive consequences.

You can't read or write a real RT-11 double density disk that is usable on
a DEC machine, since there's not enough options to describe such a disk.
Single density works fine, though.

SEE ALSO: "Cross Development", rtinit(4), rtls(4), rt11Lib(1)
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
#include "strLib.h"


IMPORT struct tm *localtime ();


#define UNIX	0
#define RT11	1
#define EITHER  2
#define PMODE	0644		/* R/W for owner, R for others */
#define STDOUT	1

LOCAL int tracksPerDisk	= 77;		/* defaults for single density disks */
LOCAL int secsPerTrack	= 26;
LOCAL int bytesPerSec	= 128;
LOCAL BOOL rtFmt = TRUE;		/* usual rt11 interleave */

LOCAL char usage [] =
"Usage: rtcp [-b <b/s>] [-s <s/t>] [-t <t/d>] [-i u|r] [-o u|r] [-r] <from> [<from> ...] <to>";


/*******************************************************************************
*
* rtcp - copy RT-11 and/or UNIX files
*/

main (argc, argv)
    int argc;
    char *argv[];

    {
    char *of_name;	/* output file name, possibly directory  */
    char this_of[80];	/* name of current output file */
    int of_type;	/* output file type, RT11 or UNIX */
    int of_fd;		/* output file descriptor */
    int if_type;	/* input file type */
    int if_fd;		/* input file descriptor */
    int lastInfile;	/* Index in argv of the last input file name */
    char buf[BUFSIZ];	/* buffer for interfile copying */
    FAST int argIndex = 1; /* Used to index through argv */
    int nbytes;		/* number of bytes obtained in superread */
    int inType = EITHER;  /* File type to which input file will be forced */
    int outType = EITHER; /* File type to which output file will be forced */

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

	else if (strcmp (argv [argIndex], "-i") == 0)
	    {
	    /* Read input file type */

	    if (*(argv [++argIndex]) == 'u')
		inType = UNIX;
	    else if (*(argv [argIndex]) == 'r')
		inType = RT11;
	    else
		error (usage, (char *)NULL);

	    argIndex++;
	    }

	else if (strcmp (argv [argIndex], "-o") == 0)
	    {
	    /* Read output file type */

	    if (*(argv [++argIndex]) == 'u')
		outType = UNIX;
	    else if (*(argv [argIndex]) == 'r')
		outType = RT11;
	    else
		error (usage, (char *)NULL);

	    argIndex++;
	    }

	else if (strcmp (argv [argIndex], "-r") == 0)
	    {
	    /* turn off usual RT-11 format */

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
    if (argIndex == argc)

	/* No file name */

	error (usage, (char *)NULL);

    else if (argIndex == argc - 1)
	{
	/* There's only one file name given. Copy input file to stdout */

	of_name = NULL;
	lastInfile = argc - 1;
	}
    else
	{
	of_name = argv[argc - 1];
	lastInfile = argc - 2;
	}

    setDate ();

    for ( ; argIndex <= lastInfile; argIndex++)	/* for each input file */
	{
	if ((if_fd = superopen (argv [argIndex], inType, &if_type, READ)) < 0)
	    error ("rtcp: can't superopen %s", argv [argIndex]);

	if (of_name == NULL)	/* no outfile specified */
	    {
	    of_type = UNIX;
	    of_fd = STDOUT;	/* send to standard output */
	    }
	else
	    {
	    /* construct outfile name, possibly by appending infile
	       name to outfile directory */

	    buildname (this_of, of_name, argv[argIndex]);

	    if ((of_fd = superopen (this_of, outType, &of_type, WRITE)) < 0)
		error ("rtcp: can't superopen %s", this_of);
	    }

	if (samefile (if_type, if_fd, of_type, of_fd))
	    error ("rtcp: can't copy file onto itself");


	/* supercopy infile to outfile */

	while ((nbytes = superread (if_fd, if_type, buf, BUFSIZ)) != 0)
	    {
	    if (nbytes < 0)
		error ("rtcp: read error on %s", argv[argIndex]);

	    if (superwrite (of_fd, of_type, buf, nbytes) != nbytes)
		error ("rtcp: write error on %s", this_of);
	    }

	superclose (if_fd, if_type);
	superclose (of_fd, of_type);
	}

    exit (OK);
    }
/*******************************************************************************
*
* buildname - construct outfile name
*
* If dirin is a directory or RT-11 volume, nameout gets the last simple name
* of namein appended to dirin. Otherwise nameout gets dirin.
*/

LOCAL VOID buildname (nameout, dirin, namein)
    char *nameout;	/* receives constructed file name */
    char *dirin;	/* directory name (may be file name) */
    char *namein;	/* file name which may be appended to dirin */

    {
    struct stat buf;		/* buffer for file status */
    char *simple_namein;	/* last simple name in namein */

    strcpy (nameout, dirin);	/* start building filename */

    if (stat (dirin, &buf) == ERROR)	/* file doesn't exist */
	return;			/* assume it's a complete filename */

    if ((buf.st_mode & S_IFMT) == S_IFREG)	/* regular file */
	return;				/* it's a complete filename */

    /* dirin is a directory or special;
     * find and append last simple name of namein */

    if ((simple_namein = rindex (namein, '/')) != 0)

	strcat (nameout, simple_namein);
    else
	{
	strcat (nameout, "/");
	strcat (nameout, namein);
	}
    }
/*******************************************************************************
*
* samefile - determine whether 2 files are the same file
*
* This routine only tests for identity between 2 unix files, it does not
* test for identity between 2 rt11 files.
*
* RETURNS: (TRUE | FALSE)
*/

LOCAL BOOL samefile (itype, ifd, otype, ofd)
    int itype;		/* input file type (UNIX or RT11) */
    int ifd;		/* input file descriptor */
    int otype;		/* output file type (UNIX or RT11) */
    int ofd;		/* output file descriptor */

    {
    struct stat ibuf, obuf;	/* status buffers */

    if ((itype == UNIX) && (otype == UNIX))
	{
	fstat (ifd, &ibuf);	/* get infile status */
	fstat (ofd, &obuf);	/* get outfile status */

	return (ibuf.st_ino == obuf.st_ino);
	}

    if ((itype == RT11) && (otype == RT11))
	{
	return (bcmp (&((RT_FILE_DESC *)ifd)->rfd_dir_entry.de_name,
			&((RT_FILE_DESC *)ofd)->rfd_dir_entry.de_name,
			sizeof (RT_NAME)) == 0);
	}

    return (FALSE);
    }
/*******************************************************************************
*
* setDate - get current date, let rt11 stuff know
*/

LOCAL VOID setDate ()

    {
    long chronus = time (0);			/* seconds since 01jan70 GMT */
    struct tm *tmptr = localtime (&chronus);	/* interpret it */

    /* convey this important info to rt11land */

    rt11DateSet (tmptr->tm_year, tmptr->tm_mon + 1, tmptr->tm_mday);
    }
/*******************************************************************************
*
* superopen - open unix or rt11 file
*
* Returns unix or rt11 file descriptor.
* If dfltType == EITHER, superopen decides whether the file is a unix or
* rt11 file. If dfltType == UNIX or RT11, superopen uses that file type.
* Integer pointed at by type gets the file type actually used (UNIX or RT11).
*/

LOCAL int superopen (name, dfltType, type, mode)
    char *name;
    int dfltType;
    int *type;
    int mode;

    {
    char dirname[80];		/* buffer for directory name */
    struct stat buf;
    int i;			/* index into dirname */
    static RT_UNIX_DEV rtUDev;	/* rt-11 device descriptor for unix */
    static BOOL rtinit = FALSE;	/* flag that rt11 communication has been
				   initialized */

    strcpy (dirname, name);

    /* find last slash in dirname, if any */

    for (i = strlen (dirname) - 1; dirname[i] != '/' && i >= 0; i--);

    if ((i <= 0) && (dfltType != RT11))
	{
	/* either no slash found, or directory is "/", and its a unix file */

	*type = UNIX;

	if (mode == READ)
	    return (open (name, READ));		/* open unix file */
	else
	    return (creat (name, PMODE));	/* create unix file */
	}

    
    dirname[i] = '\0';		/* divide string into directory name
				   and file name */
    stat (dirname, &buf);

    if (((buf.st_mode & S_IFMT) == S_IFBLK) || (dfltType == RT11))

	{
	/* Either it's a block special file, or we're being forced to treat
	   it as an RT11 file anyway. */

	*type = RT11;

	if (!rtinit)
	    {
	    /* initialize rt-11 package */

	    if ((rtUDev.fd = open (dirname, UPDATE)) < 0)
		{
		fprintf (stderr, "superopen: can't open %s\n", dirname);
		return (ERROR);
		}

	    rt11Init (2);		/* allow 2 open files */

	    rt11DevInit ((RT_VOL_DESC *) &rtUDev, bytesPerSec, secsPerTrack,
       		         ((tracksPerDisk - 1) * secsPerTrack), rtFmt,
		         RT_FILES_FOR_2_BLOCK_SEG,
		         rtURdSec, rtUWrtSec, (FUNCPTR) NULL);

	    rtinit = TRUE;
	    }

	if (mode == READ)
	    return ((int) rt11Open ((RT_VOL_DESC *)&rtUDev, &dirname[i+1], READ));
	else
	    return ((int) rt11Create ((RT_VOL_DESC *)&rtUDev, &dirname[i+1], 
									WRITE));
	}

    else
	{
	*type = UNIX;

	if (mode == READ)
	    return (open (name, READ));
	else
	    return (creat (name, PMODE));
	}
    }
/*******************************************************************************
*
* superread - read unix or rt11 file
*/

LOCAL int superread (fd, type, buf, nbytes)
    int fd;			/* file descriptor */
    int type;			/* file type, UNIX or RT11 */
    char *buf;			/* where to send characters */
    int nbytes;			/* max chars to send */

    {
    if (type == UNIX)
	return (read (fd, buf, nbytes));
    else
	return (rt11Read ((RT_FILE_DESC *) fd, buf, nbytes));
    }
/*******************************************************************************
*
* superwrite - write unix or rt11 file
*/

LOCAL int superwrite (fd, type, buf, nbytes)
    int fd;			/* file descriptor */
    int type;			/* file type, UNIX or RT11 */
    char *buf;			/* where to get characters */
    int nbytes;			/* number of chars to send */

    {
    if (type == UNIX)
	return (write (fd, buf, nbytes));
    else
	return (rt11Write ((RT_FILE_DESC *) fd, buf, nbytes));
    }
/*******************************************************************************
*
* superclose - close unix or rt11 file
*/

LOCAL VOID superclose (fd, type)
    int fd;			/* file descriptor */
    int type;			/* file type, UNIX or RT11 */

    {
    if (type == UNIX)
	close (fd);
    else
	rt11Close ((RT_FILE_DESC *) fd);
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
