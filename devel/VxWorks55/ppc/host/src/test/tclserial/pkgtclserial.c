/* pkgTclSerial.c - tclSerial package */

/* Copyright 1997, Wind River Systems, Inc. */

/* 
modification history 
--------------------
01c,08sep97,dat  added copyright
01b,20aug97,wmd  conditionalized compilation for hpux hosts for the dbg_on
		 variable.
01a,20mar97,wmd  created.
*/

/*
This file contains a simple Tcl package  that is intended
for opening a serial device, reading and writing from and to it.
*/


#ifdef WIN32
#include <windows.h>
#endif
#include "tcl.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tclserial.h"

/* Prototypes for procedures defined later in this file: */

static int	PkgTcl_SerialOpen (ClientData clientData,
		    Tcl_Interp *interp, int argc, char **argv);
static int	PkgTcl_SerialClose (ClientData clientData,
		    Tcl_Interp *interp, int argc, char **argv);
static int	PkgTcl_SerialSend (ClientData clientData,
		    Tcl_Interp *interp, int argc, char **argv);
static int	PkgTcl_SerialReceive (ClientData clientData,
		    Tcl_Interp *interp, int argc, char **argv);
static int      PkgTcl_SerialFlush (ClientData clientData,
		    Tcl_Interp *interp, int argc, char **argv);

int dbg_on = 0;

/**********************************************************************
*
* PkgTcl_SerialOpen - open serial port
*
* Opens a serial port or device on the host.
*
* Input arguments are:
*   device name
*   baud rate
*   number of data bits
*   number of stop bits
*   parity (even, odd, none)
*
* RETURNS:
* Returns TCL_OK (0) if successful, else TCL_ERROR (-1).
*/

static int PkgTcl_SerialOpen
    (
    ClientData dummy,			/* Not used. */
    Tcl_Interp *interp,			/* Current interpreter. */
    int argc,				/* Number of arguments. */
    char **argv				/* Argument strings. */
    )
    {
    HANDLE fd;
    int baud;
    int dataBits;
    int stopBits;

    if (argc != 6) 
        {
	Tcl_AppendResult(interp, "wrong # args: should be \"", argv[0],
		" device, baudRate, timeout\"", (char *) NULL);
	return TCL_ERROR;
        }

    PRINT_ARGS(argc, argv)

    /*
     * argv[0] holds the command line "devName"
     * argv[1] holds the device name
     * argv[2] holds the baud rate
     * argv[3] holds the number of data bits
     * argv[4] holds the number of stop bits
     * argv[5] holds the parity ("even", "odd", "none")
     */

    if (Tcl_GetInt (interp, argv[2], &baud) != TCL_OK)
        return TCL_ERROR;
    if (Tcl_GetInt (interp, argv[3], &dataBits) != TCL_OK)
        return TCL_ERROR;
    if (Tcl_GetInt (interp, argv[4], &stopBits) != TCL_OK)
        return TCL_ERROR;
    if ((fd = tclSerialOpen (argv[1], baud, dataBits, stopBits, argv[5]))
	    != (HANDLE)-1)
        {
	sprintf (interp->result, "%d", fd);
	}
    else
	{
	sprintf (interp->result, "%d", -1);
	}
    return TCL_OK;
    }

/***********************************************************************
*
* PkgTcl_SerialClose - close the serial device
*
* Closes the serial device named.
*
* RETURNS:
* Returns TCL_OK (0) if successful, else TCL_ERROR (-1).
*/

static int PkgTcl_SerialClose
    (
    ClientData dummy,			/* Not used. */
    Tcl_Interp *interp,			/* Current interpreter. */
    int argc,				/* Number of arguments. */
    char **argv				/* Argument strings. */
    )
    {
    HANDLE fd;

    if (argc != 2)
	{
	Tcl_AppendResult(interp, "wrong # args: should be \"", argv[0],
		" fd\"", (char *) NULL);
	return TCL_ERROR;
	}

    PRINT_ARGS(argc, argv)

    if (Tcl_GetInt (interp, argv[1], (int *)&fd) != TCL_OK)
        return TCL_ERROR;
    
    sprintf (interp->result, "%d", tclSerialClose (fd));

    return TCL_OK;
    }

/********************************************************************
*
* PkgTcl_SerialSend - send data to a serial device
*
* Sends a stream of Number of bytes to the device that's named by the
* device descriptor that was recevied from the SerialOpen command.
*
* RETURNS:
* Returns TCL_OK (0) if successful, else TCL_ERROR (-1).
*/

static int PkgTcl_SerialSend
    (
    ClientData dummy,			/* Not used. */
    Tcl_Interp *interp,			/* Current interpreter. */
    int argc,				/* Number of arguments. */
    char **argv				/* Argument strings. */
    )
    {
    HANDLE fd;
    char * outBuf;
    int nBytes;
  
    if (argc != 4)
	{
	Tcl_AppendResult(interp, "wrong # args: should be \"", argv[0],
		" fd, buffer, nbytes\"", (char *) NULL);
	return TCL_ERROR;
	}

    PRINT_ARGS(argc, argv)

    /*
     * argv[0] holds the file command line
     * argv[1] holds the device descriptor
     * argv[2] holds the command to send
     * argv[3] holds the number of bytes to write
     */

    if (Tcl_GetInt (interp, argv[1], (int *)&fd) != TCL_OK)
        return TCL_ERROR;
    if (Tcl_GetInt (interp, argv[3], &nBytes) != TCL_OK)
        return TCL_ERROR;
    
    outBuf = (char *)malloc (nBytes+1);
    strcpy (outBuf, argv[2]);
    outBuf[nBytes] = '\0';

    sprintf (interp->result, "%d", tclSerialSend (fd, outBuf, nBytes));
    free (outBuf);
    return TCL_OK;
    }

/*************************************************************************
*
* PkgTcl_SerialReceive -
*
* Receives from the device named, a stream of bytes until the desired pattern
* is received, or the timeout specified is reached.
*
* Input parameters are:
*   device descriptor received from the SerialOpen command
*   the size of buffer to allocate
*   the timeout value to end the read
*   the string pattern to match which ends the read
*
* RETURNS:
* Returns TCL_OK (0) if successful, else TCL_ERROR (-1).
* The result buffer contains a list { retval, buffer}
* where the retval is bytes received for success, 0 for timeout, 
* and -1 if the input buffer is overrun.
*/

static int
PkgTcl_SerialReceive
    (
    ClientData dummy,			/* Not used. */
    Tcl_Interp *interp,			/* Current interpreter. */
    int argc,				/* Number of arguments. */
    char **argv				/* Argument strings. */
    )
    {
    HANDLE    fd = 0;
    int    nBytes = 0;
    int    timeout = 0;
    char * pattern = NULL;
    int    retval = 0;
    char * inbuf = NULL;
    static char strval [10];

    if (argc != 5)
	{
	Tcl_AppendResult(interp, "wrong # args: should be \"", argv[0],
		    " fd, bufsize, timeout, pattern\"", (char *) NULL);
	return TCL_ERROR;
	}

    /*
     * argv[0] holds the file command line
     * argv[1] holds the device descriptor
     * argv[2] holds the size of buffer to allocate
     * argv[3] holds the timeout value to end the read
     * argv[4] holds the pattern to match to end the read
     */

    PRINT_ARGS(argc, argv)

    if (Tcl_GetInt (interp, argv[1], (int *)&fd) != TCL_OK)
        return TCL_ERROR;
    if (Tcl_GetInt (interp, argv[2], &nBytes) != TCL_OK)
        return TCL_ERROR;
    if (Tcl_GetInt (interp, argv[3], &timeout) != TCL_OK)
        return TCL_ERROR;

    pattern = malloc (strlen (argv[4]) + 1);
    strcpy (pattern, argv[4]);

    inbuf = malloc (nBytes);
    retval = tclSerialReceive (fd, inbuf, nBytes, timeout, pattern);
    sprintf (strval, "%d", retval);
    Tcl_AppendElement (interp, strval);
    Tcl_AppendElement (interp, inbuf);
    free (inbuf);
    return TCL_OK;
    }

/***********************************************************************
*
* PkgTcl_SerialFlush - flush the serial buffer
*
* Flush the input buffer and device buffer of the named device
*
* Input:  Device descriptor that was received from the SerialOpen command
*
* RETURNS:
* Returns TCL_OK (0) if successful, else TCL_ERROR (-1).
*/

static int PkgTcl_SerialFlush
    (
    ClientData dummy,			/* Not used. */
    Tcl_Interp *interp,			/* Current interpreter. */
    int argc,				/* Number of arguments. */
    char **argv 			/* Argument strings. */
    )
    {
    HANDLE fd;

    if (argc != 2)	
	{
	Tcl_AppendResult (interp, "wrong # args: should be \"", argv[0],
			" fd\"", (char *) NULL);
	return TCL_ERROR;
	}

    PRINT_ARGS(argc, argv)

    if (Tcl_GetInt (interp, argv[1], (int *)&fd) != TCL_OK)
        return TCL_ERROR;
    
    sprintf (interp->result, "%d", tclSerialFlush (fd));

    return TCL_OK;
    }

/***********************************************************************
*
* Pkgtclserial_Init - init interpreter with tclserial commands
*
* This is a package initialization procedure, which is called
* by Tcl when this package is to be added to an interpreter.
*
* RETURNS:
* Returns TCL_OK (0) if successful, else TCL_ERROR (-1).
*/

#ifdef WIN32
__declspec(dllexport)
#endif

int Pkgtclserial_Init
    (
    Tcl_Interp *interp		/* Interpreter in which the package is */
				/* to be made available. */
    )
    {
    int code;

    code = Tcl_PkgProvide (interp, "PkgTclSerial", "1.0");
    if (code != TCL_OK)
	{
        return code;
	}

    Tcl_CreateCommand (interp, "tclSerOpen", PkgTcl_SerialOpen,
		    (ClientData) 0, (Tcl_CmdDeleteProc *) NULL);
    Tcl_CreateCommand (interp, "tclSerClose", PkgTcl_SerialClose,
		    (ClientData) 0, (Tcl_CmdDeleteProc *) NULL);
    Tcl_CreateCommand (interp, "tclSerSend", PkgTcl_SerialSend,
		    (ClientData) 0, (Tcl_CmdDeleteProc *) NULL);
    Tcl_CreateCommand (interp, "tclSerReceive", PkgTcl_SerialReceive,
		    (ClientData) 0, (Tcl_CmdDeleteProc *) NULL);
    Tcl_CreateCommand (interp, "tclSerFlush", PkgTcl_SerialFlush,
		    (ClientData) 0, (Tcl_CmdDeleteProc *) NULL);

#if HOST!=parisc_hpux9 
    Tcl_LinkVar (interp, "debug_on", (char *) &dbg_on, TCL_LINK_INT);
#endif

    return TCL_OK;
    }

