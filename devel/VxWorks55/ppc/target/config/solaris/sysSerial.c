/* sysSerial.c - solaris & linux serial device initialization */

/* Copyright 1997-2001 Wind River Systems, Inc. */

#include "copyright_wrs.h"

/*
modification history
--------------------
01f,19dec01,jmp  fixed sysSerialPppPoll() to no longer use u_select() with
                 VxWorks' fd_set and FD_SETSIZE, use simReadSelect() instead.
01e,06jun01,jmp  added SIMSPARCSOLARIS support.
01d,22dec00,jmp  switched from SLIP to PPP.
01c,01dec00,jmp  added flush of SLIP serial line receive buffer in
                 sysSerialHwInit2.
01b,21nov00,jmp  replaced L_CNOTTY by L_NOCTTY.
		 fixed typo in INCLUDE_SLIP.
01a,10nov00,hbh	 written based on SIMHPPA version.
*/

/*
DESCRIPTION

This library contains routines for linux and solaris serial device
initialization.
*/

/*
DESCRIPTION
This source file is not usually compiled by itself.  It is #included
from sysLib.c.  The purpose of this source file is to isolate the
serial hardware setup from the bulk of the sysLib.c module.  BSPs using
the same serial devices should be able to use the sysSerial.c from
another BSP with very few modifications.
*/

/* includes */

#include "vxWorks.h"
#include "simLib.h"
#include "stdio.h"
#include "u_Lib.h"
#include "string.h"
#include "iv.h"
#include "intLib.h"
#include "config.h"
#include "sysLib.h"
#include "taskLib.h"
#include "unixSio.h"

/* defines */

#define PPP_OPEN_ERROR_MSG "PPP Error: Unable to open %s.\
	   Please check that PPP is installed and configured for VxSim.\n"
#define PPP_LOCK_ERROR_MSG "PPP Error: %s already in use.\
	   Please check that no other application is using %s\n"

#define PPP_PPPD	1	/* use pppd (linux and solaris >= 2.9)	*/
#define PPP_ASPPP	2	/* use asppp (solaris < 2.9)		*/
#define PPP_UNKNOWN	-1	/* unknown os version, probably unsupported */

/* globals */

UNIX_CHAN unixChan [NUM_TTY];	/* Hardware device structures */

/*
 * Array of pointers to all serial channels configured in system.
 * See sioChanGet(). It is this array that maps channel pointers
 * to standard device names.  The first entry will become "/tyCo/0",
 * the second "/tyCo/1", and so forth.
 *
 * See sysSerialHwInit ().
 */

SIO_CHAN * sysSioChans [NUM_TTY];

#ifdef	INCLUDE_PPP
/* locals */

LOCAL int  pppVer = 0;

/* foward declarations */

LOCAL void sysSerialPppPoll (UNIX_CHAN * pChan);
LOCAL int sysSerialPppVerGet (void);

/* externs */

extern int pppPollPid;
extern int sysPPPOpen ();
#endif	/* INCLUDE_PPP */

/******************************************************************************
*
* sysSerialHwInit - initialize the BSP serial devices to a quiesent state
*
* This routine initializes the BSP serial device descriptors and puts the
* devices in a quiescent state.  It is called from sysHwInit() with
* interrupts locked.  Polled mode serial operations are possible, but not
* interrupt mode.  Interrupt mode is enabled in sysSerialHwInit2().
*
* RETURNS: N/A
*/ 

void sysSerialHwInit (void)
    {
    int ix;
    UNIX_CHAN *pChan = 0;

#ifdef INCLUDE_PPP
    char ptyName [100];

    pppVer = sysSerialPppVerGet ();
#endif

    for (ix = 0; ix < NUM_TTY; ix++)
	{
	pChan = &unixChan[ix];
	sysSioChans[ix] = (SIO_CHAN *) pChan;

#ifdef INCLUDE_PPP
	if (ix == PPP_TTY)
	    {
	    if (pppVer == PPP_PPPD)
		{
		sprintf (ptyName, PPP_PSEUDO_TTY_PATH, sysProcNumGet());

		/* open pseudo device */

		pChan->u_fd = u_open (ptyName, L_RDWR | L_NOCTTY | L_NDELAY,
				      0644);

		if (pChan->u_fd < 0)
		    {
		    u_printf (PPP_OPEN_ERROR_MSG, ptyName);
		    }
		else
		    {
		    /* test if device is already in used by another simulator */

		    if (u_lockf (pChan->u_fd, F_TLOCK, 0) != 0)
			{
			u_printf (PPP_LOCK_ERROR_MSG, ptyName, ptyName);
			u_close (pChan->u_fd);
			}
		    }
		}
	    else if (pppVer == PPP_ASPPP)
		{
		/* open asppp device */

		pChan->u_fd = sysPPPOpen ();
		}
	    else
		{
		u_printf ("sysSerialHwInit: PPP: unsupported OS\n");
		}
	    }
	else
#endif /* INCLUDE_PPP */

	if (ix > 0)
	    {
	    pChan->u_fd = -1;
	    }

    	unixDevInit (pChan);
	}
    }

/******************************************************************************
*
* sysSerialHwInit2 - serial initialization, part 2
*
* This routine usually connects the serial device interrupts, and calls the
* second step driver initializations if any.  It is called from
* sysHwInit2().  Serial device interrupts could not be connected in
* sysSerialHwInit() because the kernel memory allocator was not initialized
* at that point, and intConnect() calls malloc().
*
* RETURNS: N/A
*/ 

void sysSerialHwInit2 (void)
    {
    int ix;
    UNIX_CHAN *pChan;
    static int done = 0;

    if (done)
	return;
    done = 1;
    
    /* enable interrupt mode of operation */
    
    for (ix = 0; ix < NUM_TTY; ix++)
	{
	pChan = (UNIX_CHAN *) sysSioChans[ix];

	if (pChan->created)
	    {
	    (void)intConnect ( (void *) FD_TO_IVEC (pChan->u_fd), unixIntRcv, 
	         (unsigned int) pChan);
	    unixDevInit2 (pChan);

#ifdef	INCLUDE_PPP
	    if (ix == PPP_TTY)
		{
#if (CPU_FAMILY == SIMLINUX)
		char buf[100];

		/* flush serial line receive buffer */

		while (u_read (pChan->u_fd, buf, sizeof(buf)) > 0);
#endif	/* CPU_FAMILY == SIMLINUX */

#if (CPU_FAMILY == SIMSPARCSOLARIS)
		if (pppVer == PPP_PPPD)
		    {
		    sysSerialPppPoll (pChan);
		    }
#endif	/* CPU_FAMILY == SIMSPARCSOLARIS */
		}
#endif
	    }

	/* setup to get SIGIO when I/O ready */
	
	s_fdint (pChan->u_fd, 1);
	}

#if	(CPU==SIMSPARCSOLARIS) && defined(INCLUDE_PPP)
    if (pppVer == PPP_ASPPP)
	{
	u_kill (u_getpid(), 22);	/* need SIGPOLL to start things */
	}
#endif	/* (CPU==SIMSPARCSOLARIS) && defined(INCLUDE_PPP) */
    }

/******************************************************************************
*
* sysSerialChanGet - get the SIO_CHAN device associated with a serial channel
*
* This routine returns a pointer to the SIO_CHAN device associated
* with a specified serial channel.  It is used by usrRoot to obtain 
* pointers for creating all the system serial devices, "/tyCo/x".  It
* is also how the wdb agent locates the proper serial channel when using
* serial communications to the target server.
*
* RETURNS: A pointer to the SIO_CHAN structure for the channel, or ERROR
* if the channel is invalid.
*/

SIO_CHAN * sysSerialChanGet
    (
    int channel         /* serial channel */
    )
    {
    if (channel < 0 || channel >= NELEMENTS (sysSioChans))
	return ((SIO_CHAN *) ERROR);

    return sysSioChans [channel];
    }


/******************************************************************************
*
* sysSerialReset - reset the serial devices to a quiescent state
*
* This routine resets the serial device. It is called by sysToMonitor().
*
* RETURNS: N/A
*
* SEE ALSO: sysToMonitor()
*/

void sysSerialReset (void)
    {
    UNIX_CHAN *pChan;
    int ix;

    for (ix = 0; ix < NUM_TTY; ix++)
	{
	pChan = (UNIX_CHAN *) sysSioChans[ix];
	unixDevInit (pChan);
	}
    }

#ifdef INCLUDE_PPP
/******************************************************************************
*
* sysSerialPppVerGet - get PPP version to use
*
* This routine determines which implementation of PPP must be used on the
* running operating system.
*
* RETURNS: PPP_PPPD for linux, and solaris version upper or equal than 2.9.
*	   PPP_ASPPP for solaris version lower than 2.9.
*/

LOCAL int sysSerialPppVerGet (void)
    {
    static int sysSerialPppVer = 0;

    if (!sysSerialPppVer)
	{
	char sysname [100];
	char nodename [100];
	char release [100];
	char version [100];
	char machine [100];

	s_uname (sysname, nodename, release, version, machine);

	if (strcmp (sysname, "Linux") == 0)
	    {
	    sysSerialPppVer = PPP_PPPD;
	    }
	else if (strcmp (sysname, "SunOS") == 0)
	    {
	    if ((strcmp (release, "5.5.1") == 0) ||
		(strcmp (release, "5.6") == 0) ||
		(strcmp (release, "5.7") == 0) ||
		(strcmp (release, "5.8") == 0))
		{
		sysSerialPppVer = PPP_ASPPP;
		}
	    else
		{
		/* assuming solaris version is 5.9 or higher */

		sysSerialPppVer = PPP_PPPD;
		}
	    }
	else
	    {
	    sysSerialPppVer = PPP_UNKNOWN;
	    }
	}

    return (sysSerialPppVer);
    }

#if (CPU_FAMILY == SIMSPARCSOLARIS)
/*******************************************************************************
*
* sysSerialPppPoll - poll PPP connection for new packets
*
* This routine fork a new vxWorks process that will poll the PPP connection
* for new packets.
* The polling done using a blocking select() on PPP device, and when there
* is something to read, a SIGPOOL signal is sent to the vxWorks process.
*
* RETURNS: N/A
*
* ERRNO: N/A
*
*/

LOCAL void sysSerialPppPoll
    (
    UNIX_CHAN * pChan
    )
    {
    int ppid, i;

    switch (pppPollPid = u_fork ())
	{
	case -1:
	    u_printf ("sysSerialPppPoll Error: Cannot start PPP "
			"polling process, u_fork() failed.\n");
	    u_exit (0);
	    break;

	case 0:
	    /* child process code */

	    /* reset child process signals to their default value */

	    for (i = 0; i <= 45; i++)	/* 45: MAXSIG for solaris */
		u_signal (i, (void *) 1);	/* 1: SIG_IGN */

	    while (TRUE)
		{
		/* get parent process Id (vxWorks PID) */

		ppid = u_getppid ();

		if (ppid == 1)
		    {
		    /*
		     * The parent process Id is 1, this means that it has been
		     * killed, so the poll process job is finished. Bye.
		     */

		    u_exit (0);
		    }

		if (simReadSelect (pChan->u_fd, 0, 100) > 0)
		    {
		    /* notify vxWorks that there is something to read */

		    /*
		     * notify vxWorks that there is something to read, and let
		     * it some time to consume this new packet.
		     */

		    u_kill (ppid, 22);	/* SIGPOLL */
		    u_usleep (10);
		    }
		}

	    break;
	}
    }
#endif	/* CPU_FAMILY == SIMSPARCSOLARIS */
#endif	/* INCLUDE_PPP */
