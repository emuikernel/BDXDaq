/* sysProgramBootImage.c - VxWorks download and Boot Image Programming */

/* Copyright 2004 Motorola, Inc. All Rights Reserved */

/*
modification history
--------------------
01b,07jun04,cak  Minor adjustment to command syntax strings.
01a,27may04,cak  Initial writing (ported the download portion from 
		 flash.c in the test source base).
*/

/*
DESCRIPTION

This file contains two utilities for programming a boot image into
the boot block of a FLASH bank.  One utility allows you to download
the boot image via the network, and one utility allows you to specify
a locally resident boot image (ie. on disk, tffs, etc.).

The utility to program a file that has been downloaded from a remote host is:

.CS
 programRemoteBootImage
    -n  filename to download
    -d  devicename of FLASH bank to be programmed
    -h  host dotted ip address (optional - boot host as default)
    -g  gateway dotted ip address (optional - boot gateway as default)
    syntax: remoteBootImage "-n<fname>","-d<devname>","-h<host>","-g<gateway>"
.CE

Either RSH, FTP, or TFTP will be used to download the file, based on the
boot parameters.  If the "flags" boot parameter is set to 0x80 (for TFTP)
then TFTP will be used.  Otherwise, if a "password" is specified, then FTP
will be used.  If neither of the previous two conditions are met then RSH 
will be used to download the file.

The utility to program a "local" file is:

.CS
 programLocalBootImage
    -n  filename to open
    -d  devicename of FLASH bank to be programmed
    syntax: localBootImage "-n<fname>","-d<devname>"
.CE

The file name specified must be a locally accessible file, located on
a hard disk, a TFFS drive, or another such local medium.
*/

/* includes */

#include "ioLib.h"
#include "ctype.h"
#include "fioLib.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "config.h"
#include "ftpLib.h" 
#include "tftpLib.h"
#include "remLib.h"
#include "inetLib.h"
#include "routeLib.h"
#include "sysLib.h"

/* defines */

#define RSHD		514		/* rshd service */
#define MAX_FILE_SIZE	0xFFF00		/* 1MB - 0x100  maximum */
#define DIGIT(x) (isdigit(x)? ((x)-'0') : (10+tolower(x)-'a'))

/* locals */

/* forward declarations */

LOCAL char * findOption (char *[], char);
LOCAL STATUS download (char *, char *, char *, char *, char*, int*);
LOCAL STATUS fileOpen (char *, char *, int *);
LOCAL STATUS flashProgram (char *, char *, int *);
STATUS programRemoteBootImage (char *, char *, char *, char *);
STATUS programLocalBootImage (char *, char *);

/* globals */

IMPORT int sysFlags;

/* command syntax strings */

static char *remoteBootImageExplanation = { "\
    -n  filename to download\r\n\
    -d  devicename of FLASH bank to be programmed\r\n\
    -h  host dotted ip address (optional - boot host as default)\r\n\
    -g  gateway dotted ip address (optional - boot gateway as default)\r\n\
    syntax: programRemoteBootImage \"-n<fname>\",\
\"-d<devname>\",\"-h<host>\",\"-g<gateway>\"\r\n\
    " };

static char *localBootImageExplanation = { "\
    -n  filename to open\r\n\
    -d  devicename of FLASH bank to be programmed\r\n\
    syntax: programLocalBootImage \"-n<fname>\",\"-d<devname>\"\r\n\
    " };

/*******************************************************************************
*
* findOption - scan argument list for option
*
* This function's purpose is to scan the argument list for a particular
* switch for input.
*
* RETURNS: pointer to argument if found, NULL otherwise.
*/

LOCAL char *findOption
    (
    char *argv[],	/* argument list */
    char option		/* option switch */
    )
    {
    char *pCurrent;
    char *pTick;
    int i = 0;

    pTick = (char *)strchr(argv[i], '-');
    pCurrent = (char *)strchr(argv[i], option);

    while (argv[i++] != NULL)
           {
           if (pTick && pCurrent)
              {
              if ((pCurrent - pTick) == 1) /* Found it */
                 return (++pCurrent);
              }
           if (argv[i] == NULL)
               return (NULL);
           pTick = (char *)strchr(argv[i], '-');
           pCurrent = (char *)strchr(argv[i], option);
           }
    return (NULL);
    }

/******************************************************************************
*
* download - download a file from a remote machine via the network.
*
* This routine will download a file from a remote machine via the network
* using one of three methods: RSH, FTP, or TFTP.  The method chosen is
* determined by the method with which the VxWorks kernel image was
* loaded onto the target (this is determined by scanning the boot parameters).
*
* RETURNS: OK or ERROR
*/

LOCAL STATUS download 
    (
    char *hostName,		/* name of remote host */
    char *fileName,		/* name of file to be downloaded */
    char *usr,			/* user name */
    char *passwd,		/* password (for FTP) */
    char *downloadBuffer,	/* destination buffer */
    int *downloadSize		/* number of bytes downloaded */
    )
    {
    int fd;	/* file descriptor */
    int errFd;	/* for receiving standard error message from Unix */
    char command[100];
    BOOL bootFtp = (passwd[0] != EOS);
    BOOL bootRsh = FALSE;

#ifdef INCLUDE_TFTP_CLIENT
    if (sysFlags & SYSFLG_TFTP)	/* use tftp to get image */
	{
	printf("(TFTP) ... ");
	if (tftpXfer (hostName, 0, fileName, "get", "binary", &fd, &errFd)
	    == ERROR)
	    return (ERROR);
	}
    else
#endif
	{
	if (bootFtp)
	    {
	    printf("(FTP) ... ");
	    if (ftpXfer (hostName, usr, passwd, "", "RETR %s", "", fileName,
		&errFd, &fd) == ERROR)
		return (ERROR);
	    }
	else
	    {
	    bootRsh = TRUE;
	    sprintf(command, "cat %s", fileName);
	    printf("(RSH) ... ");

	    fd = rcmd (hostName, RSHD, usr, usr, command, &errFd);
	    if (fd == ERROR)
		return (ERROR);
	    }
	}

    /* Read it into memory */

    *downloadSize = fioRead (fd, (char *)downloadBuffer, 0x100000);

    if (*downloadSize == 0)
	{
	printf("File is of size 0x0\n");
	close (fd);
	close (errFd);
	return (ERROR);
	}

    /* ensure file is not too large for boot block */

    if (*downloadSize > MAX_FILE_SIZE)
	{
	printf ("File is too large for boot block\n");
	close (fd);
	close (errFd);
	return (ERROR);
	}

    printf("0x%08X bytes ", *downloadSize);

    if (bootRsh == FALSE)
	{

	/* Empty the Data Socket before close.  PC FTP server hangs otherwise */
 
	while ((read (fd, command, sizeof(command))) > 0);

	if (bootFtp)
	    (void) ftpCommand (errFd, "QUIT",0,0,0,0,0,0);
	}

    close (fd);
    close (errFd);
    return (OK);
    }
   
/******************************************************************************
*
* fileOpen - Open the specified file and retrieve the contents
*
* This routine will open the specified file and place the contents
* into a buffer.
*
* RETURNS: OK if file successfully retrieved, ERROR otherwise.
*/

LOCAL STATUS fileOpen
    (
    char * fileName,	/* file to be opened */
    char * fileBuffer,	/* destination buffer */
    int * fileSize	/* number of bytes */
    )
    {
    int fd;	/* file descriptor */

    /* open the file */

    if ((fd = open(fileName, O_RDONLY, 0)) == ERROR) 
	{
	printf("Failed to open file %s\n", fileName); 
	return (ERROR);
	} 

    /* determine file size */

    if ((*fileSize = lseek(fd, 0, SEEK_END)) == ERROR) 
	{
	printf("lseek(SEEK_END) failed on %s\n", fileName); 
	close(fd); 
	return (ERROR);
	} 

    /* reset file pointer to beginning of file */

    if (lseek(fd, 0, SEEK_SET) != 0)
	{
	printf("lseek failed to return to beginning of file %s\n", fileName);
	close(fd);
	return (ERROR);
	}

    /* check to ensure file is not too big for the boot block */

    if (*fileSize > MAX_FILE_SIZE)
	{
	printf("File is too large for boot block\n");
	close(fd);
	return (ERROR);
	}

    /* read file contents into buffer */

    if (read(fd, fileBuffer, *fileSize) == ERROR)
	{
	printf("Error reading file %s\n", fileName);
	close(fd);
	return(ERROR);
	}

    /* close file */

    close (fd); 
    return (OK);
    }

/******************************************************************************
*
* flashProgram - FLASH the boot image into the specified FLASH bank
*
* This routine will program a boot image into the boot block of the
* specified FLASH bank.
*
* RETURNS: OK if programming was successful, ERROR otherwise.
*/

LOCAL STATUS flashProgram 
    (
    char * deviceName,	/* flash device name */
    char * buffer,	/* buffer containing boot image */
    int * numBytes	/* number of bytes to be programmed */
    )
    {
    int fd;
    int bytes;
    int result = 0;

    printf("Programming %s ...", deviceName);
 
    if ((fd = open (deviceName, O_RDWR, 0644)) == ERROR)
	{
	printf("Unable to open FLASH device\n");
	return (ERROR);
	}

    if (ioctl (fd, FIONREAD, (int)&result) == ERROR)
	{
	printf("Unable to determine FLASH device size\n");
	close (fd);
	return (ERROR);
	}

    if (ioctl (fd, FIOSEEK, (result - MAX_FILE_SIZE)) == ERROR)
	{
	printf("Unable to set offset to boot block in FLASH device\n");
	close (fd);
	return (ERROR);
	}

    bytes = write (fd, buffer, *numBytes);

    if (bytes != *numBytes)
	{
	printf("Bytes written does not equal file size\n");
        close (fd);
	return (ERROR);
	}

    close (fd);
    printf("done.\n");
    return(OK);
    }

/******************************************************************************
*
* programRemoteBootImage - download boot file and program boot bank
* 
* This routine parses command line arguments and creates a route if the
* host and/or gateway arguments are specified.  The command then calls
* download() to download the boot image and calls flashProgram() to program
* the boot image into the boot block of the specified boot bank. 
*
* RETURNS: OK if successful, ERROR otherwise.
*/

STATUS programRemoteBootImage 
    (
    char *arg1,
    char *arg2,
    char *arg3,
    char *arg4
    )
    {
    char *fileName = NULL;
    char *hostIP = NULL;
    char *gatewayIP = NULL;
    char *deviceName = NULL;
    char *arglist[5] = { 0 };
    int count = 0;
    BOOT_PARAMS params;
    char *netadd = NULL;    /* host's network internet address */
    char *downloadBuffer = NULL;
    int *downloadSize = NULL;

    if (arg1)
       arglist[count++] = arg1;
    else
       {
       printf (remoteBootImageExplanation);
       return (OK);
       }
    if (arg2)
       arglist[count++] = arg2;
    if (arg3)
       arglist[count++] = arg3;
    if (arg4)
	arglist[count++] = arg4;

    bootStringToStruct (BOOT_LINE_ADRS, &params);

    fileName = findOption (arglist, 'n');

    if (fileName == NULL)
       {
       printf (remoteBootImageExplanation);
       return (OK);
       }

    strcpy(params.bootFile, fileName);

    gatewayIP = findOption (arglist, 'g');

    if (gatewayIP != NULL)

       strcpy(params.gad, gatewayIP);

    hostIP = findOption (arglist, 'h');

    if (hostIP != NULL)
       {
       strcpy (params.had, hostIP);

       /* add a route to this network */
 
       inet_netof_string (params.had, netadd);
       routeAdd (netadd, params.gad);

       printf("Route Add %s via %s\r\n", netadd, params.gad);
       }

    deviceName = findOption (arglist, 'd');

    if (deviceName == NULL)
       {
       printf (remoteBootImageExplanation);
       return (OK);
       }

    if (downloadBuffer == NULL)
       downloadBuffer = malloc (0x100000);

    /* Display download parameters */

    printf("Downloading %s from %s\n\r", params.bootFile, params.had);

    if (download (params.had, params.bootFile, params.usr,
                  params.passwd, downloadBuffer, downloadSize) != OK)
	{
	printf("Error downloading file\n");
	free (downloadBuffer);
	return (ERROR);
	}

    printf("transferred to 0x%08X. \r\n", (unsigned int)downloadBuffer);

    if (flashProgram(deviceName, downloadBuffer, downloadSize) != OK)
	{
	printf("\nError programming boot image\n");
	free (downloadBuffer);
	return (ERROR);
	}

    free (downloadBuffer);
    return(OK);
    }

/******************************************************************************
*
* programLocalBootImage - program a local file to the boot bank
* 
* This routine parses command line arguments. The command then calls
* fileOpen() to retrieve the boot image and calls flashProgram() to program
* the boot image into the boot block of the specified boot bank. 
*
* RETURNS: OK if successful, ERROR otherwise.
*/

STATUS programLocalBootImage 
    (
    char *arg1,
    char *arg2
    )
    {
    char *fileName = NULL;
    char *deviceName = NULL;
    char *arglist[3] = { 0 };
    int count = 0;
    char *fileBuffer = NULL;
    int *fileSize = NULL;

    if (arg1)
       arglist[count++] = arg1;
    else
       {
       printf (localBootImageExplanation);
       return (OK);
       }
    if (arg2)
       arglist[count++] = arg2;

    fileName = findOption (arglist, 'n');

    if (fileName == NULL)
       {
       printf (localBootImageExplanation);
       return (OK);
       }

    deviceName = findOption (arglist, 'd');

    if (deviceName == NULL)
       {
       printf (localBootImageExplanation);
       return (OK);
       }

    if (fileBuffer == NULL)
       fileBuffer = malloc (MAX_FILE_SIZE);

    if (fileOpen(fileName, fileBuffer, fileSize) != OK)
	{
	printf("\nError opening file\n");
	free (fileBuffer);
	return (ERROR);
	}

    printf("File contents at 0x%08X. \r\n", (unsigned int)fileBuffer);

    if (flashProgram(deviceName, fileBuffer, fileSize) != OK)
	{
	printf("\nError programming boot image\n");
	free (fileBuffer);
	return (ERROR);
	}

    free (fileBuffer);
    return(OK);
    }
