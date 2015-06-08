/* tclSerial.win32.c - tclserial interface using WIN32 COMM API */

/* Copyright 1997-2002, Wind River Systems, Inc. */

/*
modification history
--------------------
01d,10jun02,dat  pass device name straight thru, SPR 78580
01c,08sep97,dat  changed formatting
01b,23jul97,wmd  added signal handling to abort the process if a
		 termination signal arrives.  Removed unused functions.
01a,08apr97,wmd  written.
*/

/*
This module implements the tclserial package using the WIN32 communications
API.
*/

#include <windows.h>
#include <winbase.h>
#include <winnt.h>
#include <assert.h>
#include <stdio.h>
#include "tclserial.h"

/* Forward declarations */

static int	win32CharRead (HANDLE, char *);
static void	win32ReadTimeoutSet (HANDLE hCom, int timeout, int nbytes);
static int	win32OutputFlush (HANDLE hcom);
static int	win32InputFlush (HANDLE hcom);

/* globals */

static OVERLAPPED	ovrlapped_s;
static DWORD		evtMask;
static DCB		savedCommState;
static HANDLE		savedHCom;

/*******************************************************************************
*
* tclSerSigHandler -  WIN32 signal handler
*
* Catches any termination signal from the system and exits the program.
*
* RETURNS: N/A
*
* NOMANUAL
*/

BOOL _stdcall tclSerSigHandler
    (
    DWORD signal
    )
    {
    switch (signal)
	{
	case CTRL_C_EVENT:
	case CTRL_BREAK_EVENT:
	case CTRL_CLOSE_EVENT:
	case CTRL_LOGOFF_EVENT:
	case CTRL_SHUTDOWN_EVENT:
	    SetCommState (savedHCom, &savedCommState);
	    CloseHandle (savedHCom);
	    exit (1);

	default:                /* we don't handle anything else! */
	    return FALSE;
	}
    }

 		
/*****************************************************************************
*
* win32ReadTimeoutSet - set timeout for number of bytes to be read
*
* Set the read timeout for nbytes to be read
*
* RETURNS: N/A
*/

static void win32ReadTimeoutSet
    (
    HANDLE hCom,
    int timeout,
    int nbytes
    )
    {
    COMMTIMEOUTS timeouts;
	
    GetCommTimeouts (hCom, (LPCOMMTIMEOUTS)&timeouts);
    timeouts.ReadTotalTimeoutMultiplier = MULTIPLIER(timeout, nbytes);
    SetCommTimeouts (hCom, (LPCOMMTIMEOUTS)&timeouts);
    }

/*****************************************************************************
*
* tclSerialOpen - function which opens a serial communications channel,
*
* The channel name is used and if opened without error, the 
* fd field of the serial control block is set to the comm handle.
*
* RETURNS: File handle if successful, -1 for failure
*
*/

HANDLE tclSerialOpen
    (
    char* devName,
    int baudRate,
    int dataBits,
    int stopBits,
    char * parity
    )
    {
    DCB dcb;
    HANDLE hCom;
    DWORD dwError;
    BOOL fSuccess;
    COMMTIMEOUTS timeouts;

    hCom = CreateFile (devName,
                       GENERIC_READ | GENERIC_WRITE,
                       0,	  /* share: Must be zero for comm resouces */
                       NULL, 		  /* no security attrs */
                       OPEN_EXISTING,  	  /* must use for comm devices */
#ifdef NOT_OVERLAPPED
                       NULL,
#else
                       FILE_FLAG_OVERLAPPED,
#endif
                       (void*)NULL);	/* hTemplate */
    
    if (hCom == (HANDLE)INVALID_HANDLE_VALUE)
        {
        dwError = GetLastError();
        dbprintf ("CreateFile failed, err = ", dwError);

        /* so whatelse ?? */

        return  (HANDLE) -1;
        }
  
	savedHCom = hCom;		/* save the handle to com port */
	
#ifndef NOT_OVERLAPPED
    ovrlapped_s.Offset = 0;
    ovrlapped_s.OffsetHigh = 0;
    ovrlapped_s.hEvent = CreateEvent(NULL,	/* security attr. */
                                     TRUE,	/* man reset event ??? */
                                     FALSE,	/*  signaled */
                                     NULL);	/* not named */
  
    assert (ovrlapped_s.hEvent);
  
    if (ResetEvent (ovrlapped_s.hEvent) == FALSE)
        dbprintf ("ResetEvent failed, err = ", GetLastError());
  
    if ((fSuccess = SetCommMask (hCom, EV_RXCHAR | EV_TXEMPTY)) == FALSE)
	{
        dbprintf ("SetCommMask failed, err =\n",GetLastError());
        return (HANDLE) -1;
	}

#endif /* NOT_OVERLAPPED */

    /* get the current configuration */

    if ((fSuccess = GetCommState (hCom, &dcb)) == FALSE)
        {
        dbprintf ("GetCommState failed, err = ", GetLastError());
        return (HANDLE) -1;
        }

    savedCommState = dcb;

    /*
     * fill in DCB with: baud = baudRate (9600 is the default), 8 data bits, 
     * no parity, 1 stop bit
     */

    if (dataBits < 4 || dataBits > 8)
	{
	dbprintf ("dataBits %d is invalid!\n", dataBits);
	return (HANDLE) -1;
	}
    dcb.BaudRate = baudRate;
    dcb.ByteSize = dataBits;
    dcb.Parity = NOPARITY;
    dcb.StopBits = ONESTOPBIT;
    dcb.fOutxDsrFlow = FALSE;
    dcb.fDtrControl = DTR_CONTROL_DISABLE;
    dcb.fDsrSensitivity = FALSE;
    dcb.fRtsControl = RTS_CONTROL_DISABLE;

    switch (stopBits)
	{
	case 1:
	    dcb.StopBits = ONESTOPBIT;
	    break;
	case 2:
	    dcb.StopBits = TWOSTOPBITS;
	    break;
	default:
	    dcb.StopBits = ONE5STOPBITS;
	}

    if (strcmp (parity, "even") == 0)
	dcb.Parity = EVENPARITY;
    else if (strcmp (parity, "odd") == 0)
	dcb.Parity = ODDPARITY;
    else if (strcmp (parity, "mark") == 0)
	dcb.Parity = MARKPARITY;
	
    if ((fSuccess = SetCommState (hCom, &dcb)) == FALSE)
        {
        dbprintf ("SetCommState failed, err = ", GetLastError);
        return (HANDLE) -1;
        }
  
    GetCommTimeouts (hCom, &timeouts);
    timeouts.ReadIntervalTimeout = 0;
    timeouts.ReadTotalTimeoutMultiplier = MULTIPLIER(TIMEOUT_VALUE, 1);
    timeouts.ReadTotalTimeoutConstant = 0;
    timeouts.WriteTotalTimeoutMultiplier = 0;
    timeouts.WriteTotalTimeoutConstant = 0;
    SetCommTimeouts (hCom, &timeouts);

    SetConsoleCtrlHandler(tclSerSigHandler, TRUE);
    return hCom;
    }

/*****************************************************************************
*
* tclSerialClose - close the handle for the communications resource
*
* Closes the communications port for the handle that was passed.
*
* RETURNS: 0 upon success, -1 for failure.
*/

int tclSerialClose
    (
    HANDLE hcom
    )
    {
    int fSuccess;

    win32OutputFlush (hcom);
    win32InputFlush (hcom);

    if ((fSuccess = SetCommState (hcom, &savedCommState)) == FALSE)
        {
        dbprintf ("SetCommState failed, err = ", GetLastError);
        return -1;
        }
    
    return (CloseHandle (hcom) ? 0 : -1);
    }

/*****************************************************************************
*
* tclSerialFlush - flush the console input buffer
*
* RETURNS: 0 if successful, -1 otherwise.
*/

int tclSerialFlush
    (
    HANDLE hcom
    )
    {
    int fSuccess;

    if ((fSuccess = win32InputFlush (hcom)) == FALSE)
        {
        dbprintf ("SetCommState failed, err = ", GetLastError);
        return -1;
        }
    return 0;    
    }

/*****************************************************************************
*
* win32CharRead - read a chanacter from the serial communications device
*
* RETURNS: 1 if read succeeded, -1 if failure, or SERIAL_TIMEOUT_FAILURE
* if timeout
*/

static int win32CharRead
    (
    HANDLE hcom,
    char * inchar
    )
    {
    int  rc;
    int  nmRead = 0;
    int  bytesRead = 0;
    
#ifdef NOT_OVERLAPPED
    rc = ReadFile (hcom, inchar, 1, &bytesRead, NULL);
#else
    ovrlapped_s.Offset = 0;
    ovrlapped_s.OffsetHigh = 0;
    rc = ReadFile (hcom, inchar, 1, &bytesRead, &ovrlapped_s);
    
    if (!rc)
	{
	switch (GetLastError())
	    {
	    case ERROR_HANDLE_EOF:
                printf ("win32CharRead: got EOF, do something??\n");
                break;
	    case ERROR_IO_PENDING:
                /* wait until we get input from the monitor */
                rc = GetOverlappedResult (hcom, &ovrlapped_s, &nmRead, TRUE);
                if (nmRead == 0)
                    {
                    dbprintf ("win32CharRead: read 0 bytes, err = ",
			    GetLastError());
                    return SERIAL_TIMEOUT_FAILURE;
                    }
                bytesRead = nmRead;
                break;
            default:
                dbprintf ("win32CharRead: err = ", GetLastError());
                return -1;
	    }
	}
#endif

    if (bytesRead == 1)
        {
        if (dbg_on)
            {
            printf ("%c", (unsigned char) *inchar);
            }	
        
        return (1);
	}
    else
      return -1;
    }


/*****************************************************************************
*
* tclSerialReceive  - read len bytes from the serial communications device
*
* It returns len of bytes read if successful and pattern is matched.
* else returns 0 if timeout, and -1 if inbuf buffer is too small.
* in all cases, the buffer BUF contains the characters that are read.
*
* RETURNS: number of bytes read, or 0 if timeout, else -1 if input buffer
* is too small.
*/

int tclSerialReceive
    (
    HANDLE hcom,
    char *buf,
    int len,
    int timeout,
    char * pattern
    )
    {
    int	    rc;
    int	    bytesRead = 0;
    char    inChar;
    BOOL    patternMatch = FALSE;
    int     patternIndex = 0;
    int     patternLen = strlen (pattern);

    /* set the timeout value */

    win32ReadTimeoutSet (hcom, timeout, 1);

    while (bytesRead < len)
	{
        rc = win32CharRead (hcom, &inChar);
        if (rc == 1)
            {
            *buf++ = inChar;
            bytesRead++;
    
	    /* check to see if pattern is matched */

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
	    }
        else	/* timeout has occurred, return 0 */
	    {
	    *buf = '\0';
            return 0;
	    }
	}

    if (dbg_on)
	printf ("\n");
    
    /* see if we overran the buffer */

    if ((bytesRead >= len) && (patternIndex != patternLen))
        return -1;

    *buf = '\0';

    return bytesRead;
    }


/*****************************************************************************
*
* win32TtyStateGet - get the tty state of the device
*
* The function returns a pointer to a DCB structure which is 
* allocated in this function.  The pointer will be freed
* when a called to win32TtyStateSet() is made.  If
* not the caller must explicitly free the structure.
*
* RETURNS: TRUE if the function succeeds, else FALSE.
*/

serial_ttystate win32TtyStateGet
    (
    HANDLE hcom
    )
    {
    DCB *state;

    state = (DCB *) malloc (sizeof *state);
    if (GetCommState (hcom, state) == TRUE)
  	return (serial_ttystate) state;
    else
  	return 0;
    }

/*****************************************************************************
*
* win32TtyStateSet - sets the tty state 
*
* Sets the tty state of the device hcome to the values set in 
* ttystate.  The ttystate must be a pointer retruned by win32TtyStateGet().
*
* RETURNS: 0 if successful, -1 if failure.
*/

int win32TtyStateSet 
    (
    HANDLE hcom,
    serial_ttystate *ttystate
    )
    {
    DCB * state = (DCB *)ttystate;
    BOOL status = SetCommState (hcom, state);

    free (ttystate);
    return status;
    }

/*****************************************************************************
*
* win32TtyStatePrint - prints the state of the com device
*	
* RETURNS: N/A.
*/

void win32TtyStatePrint
    (
    HANDLE hcom,
    serial_ttystate ttystate
    )
    {
    DCB *state = (DCB *)ttystate;

    printf ("Communicatinos device state (DCB structure contents):\n"); 
    printf ("DWORD DCBlength = %d\n", state->DCBlength);
    printf ("DWORD BaudRate = %d\n", state->BaudRate);
    printf ("DWORD fBinary = %d\n", state->fBinary);
    printf ("DWORD fParity = %d\n", state->fParity);
    printf ("DWORD fOutxCtsFlow = %d\n", state->fOutxCtsFlow);
    printf ("DWORD fOutxDsrFlow = %d\n", state->fOutxDsrFlow);
    printf ("DWORD fDrrControl = %d\n", state->fDtrControl);
    printf ("DWORD fDsrSensitivity = %d\n", state->fDsrSensitivity);
    printf ("DWORD fTXContinueOnXoff = %d\n", state->fTXContinueOnXoff);
    printf ("DWORD fOutX = %d\n", state->fOutX);
    printf ("DWORD fInX = %d\n", state->fInX);
    printf ("DWORD fErrorChar = %d\n", state->fErrorChar);
    printf ("DWORD fAbortOnError = %d\n", state->fAbortOnError);
    printf ("DWORD fDummy2 = %d\n", state->fDummy2);
    printf ("DWORD wReserved = %d\n", state->wReserved);
    printf ("DWORD XomLim = %d\n", state->XonLim);
    printf ("DWORD XoffLim = %d\n", state->XoffLim);
    printf ("DWORD ByteSize = %d\n", state->ByteSize);
    printf ("DWORD StopBits = %d\n", state->StopBits);
    printf ("DWORD XonChar = %c\n", state->XonChar);
    printf ("DWORD XoffChar = %c\n", state->XoffChar);
    printf ("DWORD ErrorChar = %c\n", state->ErrorChar);
    printf ("DWORD EofChar = %c\n", state->EofChar);
    printf ("DWORD EvtChar = %c\n", state->EvtChar);
    printf ("\n");
    }

/*****************************************************************************
*
* tclSerialSend - write len chars from str to the port serial_t
*
* RETURNS: number of bytes written
*/

int tclSerialSend
    (
    HANDLE hcom,
    char * str,
    int len
    )
    {
    BOOL status;
    int	 bytes_written;
    int  evnt_mask = 0;
    int  bytesWritten = 0;
    
#ifdef NOT_OVERLAPPED
    status = WriteFile (hcom, str, len, &bytes_written, NULL);
    return bytes_written;
#else
    ResetEvent (ovrlapped_s.hEvent);
    ovrlapped_s.Offset = 0;
    ovrlapped_s.OffsetHigh = 0;
    status = WriteFile (hcom, str, len, &bytes_written, &ovrlapped_s);
    
    if (dbg_on)
	{
        int jx, ix;
        
        printf ("\ntclSerialSend: length = %d\n", len);
        for (ix = 0; ix < len;)
	    {
            for (jx = 0; (jx < DBG_LINE_CNT) && (ix < len) ; jx++, ix++)
		printf ("%2.2x ", (unsigned char) str[ix]);
            printf ("\n");
	    }
	printf ("\n");
	}	
    
    if (status == FALSE || (bytes_written != len))
	{
        switch (GetLastError ())
	    {
	    case ERROR_IO_PENDING:

		/* wait until done */

		bytesWritten = bytes_written;
		status = GetOverlappedResult (hcom, &ovrlapped_s,
						&bytes_written, TRUE);
		bytesWritten += bytes_written;
		while (status == FALSE)
		    {
		    status = GetOverlappedResult (hcom, &ovrlapped_s,
						&bytes_written, TRUE);
		    bytesWritten  += bytes_written;
		    printf ("tclSerialSend : len to write = %d, bytes written "
			    "= %d failed error = %d\n", len, bytesWritten,
			    GetLastError () );
		    }
		return len;		  
            
	    default:
		return bytes_written;
	    }
	}
    else
	return bytes_written;
#endif
    }


/*****************************************************************************
*
* win32BaudrateSet - set the baudrate for the serial device
*
* Sets the baudrate for the serial device represented by hcom.
*
* RETURNS: 0 if successful, else -1.
*/

int win32BaudrateSet
    (
    HANDLE hcom,
    int rate
    )
    {
    DCB* tty_state = (DCB*) win32TtyStateGet (hcom);

    tty_state->BaudRate = rate;
    return (win32TtyStateSet (hcom, (serial_ttystate*) tty_state));
    }

/*****************************************************************************
*
* win32OutputFlush - flush pending output
*
*/

static int win32OutputFlush
    (
    HANDLE hcom
    )
    {
    return (FlushFileBuffers (hcom));
    }


/*****************************************************************************
*
* win32InputFlush - flush pending input buffer, the data is lost
*
* RETURNS: TRUE if successful, FALSE otherwise
*/

static int win32InputFlush
    (
    HANDLE hcom
    )
    {
    return (PurgeComm (hcom, PURGE_RXCLEAR));
    }
