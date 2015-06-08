/* tclSerial.c  - serial i/o interface for Tcl */

/* Copyright 1984-1997 Wind River Systems, Inc. */

/* 
modification history
--------------------
01c,08sep97,dat  reformatted code, added copyright
01b,21jul97,wmd  added signal handling code.
01a,08apr97,wmd  written.
*/

/*
This module implements the tclserial package for Tcl.  This allows
TCL code access to serial i/o channels with a host-independent interface.
*/

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/time.h>
#include <signal.h>

#ifdef RS6000_AIX4
#   include <sys/termio.h>
#endif

#include "host.h"

#ifdef WIN32
#   include "windll.h"
#endif

#include "tclserial.h"

/* TTY specific stuff */

static struct termios saveModes;  /* saved attributes of the port or device */
static void (*old_handler) ();
static int savedFd;
static BOOL  timesUp = FALSE;

struct baudtable
    {
    int baudConst;
    int	rate;
    };

static struct baudtable baudTable[] =
    {
    {B150,     150},
    {B300,     300},
    {B600,     600},
    {B1200,   1200},
    {B1800,   1800},
    {B2400,   2400},
    {B4800,   4800},
    {B9600,   9600},
    {B19200, 19200},
    {B38400, 38400},
    {0,	     0}
    };

/******************************************************************************
*
* tclSerSigHandler - handle signals 
*
* RETURNS: N/A.
*/

void tclSerSigHandler
    (
    int sig
    )
    {
    tcsetattr (savedFd, TCSAFLUSH, &saveModes);
    exit (1);
    }


/******************************************************************************
*
* tclSer_send - write data from buf out the serial device fd.
*
* RETURNS : if successful, the number of bytes sent, otherwise, 0
*/ 

int tclSerialSend
    (
    int		fd,
    char *	buf,
    int		nBytes
    )
    {
    int		bytesSent = 0;

    DEBUG_PRINT ("tclSerialSend: number of bytes to send is %d\n",
                     nBytes);
    DEBUG_PRINT ("tclSerialSend: buf contains %s\n", buf);
	
    /* write out the data */

    bytesSent = write (fd, buf, nBytes);

    if (bytesSent < nBytes)
	{
	DEBUG_PRINT ("tclSer_send: didn't send all bytes, byteSent = %d", 
                     bytesSent);
	return (0);
	}

    return (nBytes);
    }

/******************************************************************************
*
* tclRcvTimeoutHdlr - 
*
*/

void tclRcvTimeoutHdlr
    (
    int disp
    )
    {
    timesUp = TRUE;
    }


/******************************************************************************
*
* tclSerialreiver - get input over a serial line
*
* Reads characters until the input pattern desired is received, or the
* the timeout value is exceeded, or the input buffer is full.
*
* RETURNS:
* number of bytes received if successful receipt of characters and the 
*               pattern is found.
*          0 if timeout occurs without receipt of the pattern.
*          -1 if input buffer too small.
*/ 

int tclSerialReceive
    (
    int		fd,                      /* file descriptor or handle */
    char *	buf,                     /* input buffer */
    int		nBytes,                  /* size of buffer */
    int         timeout,                 /* time out value to use */
    char *      pattern                  /* character pattern to match */
    )
    {
    char	inChar;
    int		bytesRead;
    BOOL        patternMatch = FALSE;
    int         patternIndex = 0;
    int         patternLen = strlen (pattern);
    struct itimerval timerval;

    timerval.it_interval.tv_usec = 0;
    timerval.it_interval.tv_sec = 0;
    timerval.it_value.tv_usec = 0;
    timerval.it_value.tv_sec = timeout;

    /* start a timer to exit input */

    if (signal (SIGALRM, tclRcvTimeoutHdlr) == SIG_ERR)
        {
        printf ("signal (): Failed to set a signal trap\n");
        exit (1);
        }

    setitimer (ITIMER_REAL, &timerval, (struct itimerval *) 0);

    DEBUG_PRINT ("tclSerialReceive: set the timer for %d seconds\n", timeout);

    timesUp = FALSE;    /* reset the timer boolean */

    DEBUG_PRINT ("tclSerialReceive: nBytes is %d\n", nBytes);

    if (dbg_on)
        {
        printf ("\ntclSerialReceive: receive len = %d\n", nBytes);
        } 

    for (bytesRead = 0; bytesRead < nBytes; bytesRead++)
	{
        
	if (read (fd, &inChar, 1) < 1)
            {
            if (dbg_on)
                printf ("\n");
#ifdef RS6000_AIX4
            if (errno == ENOENT || errno == EACCES || errno > 100)
                return (0);
            printf (" \nRead failure: errno is %d\n", errno);
#endif
            DEBUG_PRINT ("tclSerReceive: read received an error, errno is %d\n",
                         errno);
            }

        if (dbg_on)
            printf ("%c", inChar);

        *buf++ = inChar;

        /* check to see if we find a pattern */
        if (patternMatch)
            {
            if (inChar == pattern [patternIndex++])
                {
                if (patternIndex == patternLen)
                    break;
                }
            else
                {
                patternIndex = 0;
                patternMatch = FALSE;
                }
            }
        else
            {
            if (inChar == pattern [0])
                {
                patternIndex++;
                patternMatch = TRUE;
                }
            }

	/* check to see if timer has timed out */

        if (timesUp)
	    {
            *buf = '\0';
            return 0;
            }
	}

    if (dbg_on)
        printf ("\n");
    
    /* see if we overran the buffer */

    if ((bytesRead >= nBytes) && (patternIndex != patternLen))
        return -1;

    *buf = '\0';

    return (bytesRead);
    }

/******************************************************************************
*
* tclSer_BaudConstGet - convert baud rate to baud constant value
*
* This routine maps a baud rate numberic value to the proper termio constant
* representing that value.
*
* RETURNS
* Returns the baud constant value, or -1 if baud rate not in table.
*/ 

int tclSerialBaudConstGet
    (
    int buadRate
    )
    {
    int ix;

    for (ix = 0; baudTable[ix].rate != (int)NULL; ix++)
	{
	if (baudTable[ix].rate == buadRate)
	    return (baudTable[ix].baudConst);
	}

    return (-1);
    }

/******************************************************************************
*
* tclSerialOpen - open and initialize a serial device.
*
* This routine uses POSIX defined routines in order to:
*	open the device.
*	verify it is a tty device.
*	set the device hardware options to 8 data bits, 1 stop bit, no partity
*	set the specified baud rate.
*
* RETURNS: a file handle, or -1 on error.
*/ 

int tclSerialOpen
    (
    char *	    devName,
    int		    baudRate,
    int             dataBits,
    int             stopBits,
    char *          parity
    )
    {
    int 		fd = 0;		/* tty file handle */
    struct timeval      wait;
    struct termios	termIo;		/* terminal I/O options */
    int			ix;		/* counter */
    char *		pName = NULL;	/* device name pointer */

    /* We will try each of the following paths when trying to open the tty */

    static char *prefix[] = {"", "/dev/", "/dev/tty", NULL};

    wait.tv_sec = 10;
    wait.tv_usec = 0;

    /*
     * Attempt to open the serial device, appending the default pathname
     * prefixes if the device can't be opened under the name supplied.
     */

    for (ix = 0; prefix[ix] != NULL; ix++)
    	{
    	pName = (char *) malloc (strlen (prefix[ix]) + strlen (devName) + 1);
    	sprintf (pName,"%s%s", prefix[ix], devName);

#ifndef RS6000_AIX4
	if ((fd = open (pName, O_RDWR, 0)) < 0)
#else
	if ((fd = open (pName, O_RDWR | O_NDELAY, 0)) < 0)
#endif /* RS6000_AIX4 */
	    free (pName);
	else 
	    break;
	}

#ifdef RS6000_AIX4
    {
    int rc, flags;
    struct termio termio;
			 
    flags = fcntl (fd, F_GETFL, NULL);
    flags &= O_NONBLOCK;
    fcntl (fd, F_SETFL, flags);
    ioctl (fd, TCGETA, &termio);

    /* change input mode flags */

    termio.c_iflag &= ~BRKINT;
    termio.c_iflag &= ~ICRNL;

    /* change to non-canonical */

    termio.c_cc[VMIN] = 1;
    termio.c_cc[VTIME] = 0;
    termio.c_lflag &= ~ICANON;
 
    /* change to CLOCAL */

    termio.c_cflag |= CLOCAL;
    ioctl (fd, TCSETA, &termio);
    }
#endif /* RS6000_AIX */

    free (pName);

    if (fd < 0)
	return (-1);

    /* get current attributes */
    
    if (tcgetattr (fd, &termIo) == -1)
	{
	close (fd);
	return (-1);
	}

    savedFd = fd;  	/* save the file descriptor */
    termIo.c_cc[VERASE] = '\010';

    /* save the attribute */
    saveModes = termIo;

    /* configure for 8 data bits, 1 stop bit, no partity */

    termIo.c_iflag	= 0;			/* no input procesing */
    termIo.c_oflag	= 0;			/* no output procesing */
    termIo.c_cflag	= CREAD | CS8 | CLOCAL;	/* 8 data, 1 stop bits */
    termIo.c_lflag	= 0;			/* no local processing */
    termIo.c_cc[VMIN]	= 0;			/*  */
    termIo.c_cc[VTIME]	= wait.tv_sec * 10;	/* time out value */

    /* configure for baud rate for input and output */

    cfsetispeed (&termIo, tclSerialBaudConstGet (baudRate));
    cfsetospeed (&termIo, tclSerialBaudConstGet (baudRate));

    /* set the number of data bits */
    if (dataBits != 8)
        {
        termIo.c_cflag &= ~CS8;
        switch (dataBits)
            {
            case 5:
              termIo.c_cflag |= CS5;
              break;
            case 6:
              termIo.c_cflag |= CS6;
              break;
            case 7:
              termIo.c_cflag |= CS7;
              break;
            default:
              termIo.c_cflag |= CS8;   /* default is 8 bits */
            }
        }
          
    /* set the number of stop bits */
    if (stopBits != 1)
        termIo.c_cflag |= CSTOPB;

    /* set the parity */
    if (strcmp (parity, "even") == 0)
        {
        termIo.c_cflag |= PARENB;
        }
    else if (strcmp (parity, "odd") == 0)
        {
        termIo.c_cflag |= PARENB;
        termIo.c_cflag |= PARODD;
        }

    /* set the attributes */

    tcsetattr (fd, TCSAFLUSH, &termIo);

    /* signal handling */

    old_handler = signal (SIGINT, &tclSerSigHandler);

    /* all done */

    return (fd);
    }

/******************************************************************************
*
* tclSerialClose - close the serial channel
*
* Restores the save termios modes and close the serial device
*
* RETURNS:  0 if successful, else -1 on failure
*/

int tclSerialClose (int fd)
    {
    if (tcsetattr (fd, TCSAFLUSH, &saveModes) < 0)
        {
        perror ("tcsetattr");
        return -1;
        }
    signal (SIGINT, old_handler);
    return (close (fd));
    }

/******************************************************************************
*
* tclSerialFlush - flush input and output buffers
*
* This command flushes data received but not read and data written but not
* transmitted to the file descriptor.
*
* RETURNS:  0 if successful, else -1 on failure
*/

int tclSerialFlush (int fd)
    {
    return (tcflush (fd, TCIOFLUSH));
    }
