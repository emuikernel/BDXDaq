/* win32ThreadLib.h - win32 thread and mutex library header file */
 
/* Copyright 1998 Wind River Systems, Inc. */
 
/*
modification history
--------------------
01c,05aug98,c_c  Added an API to avoid the creation a default signal handler
                 thread (HPUX only).
01b,22jun98,c_c  Added a threadIdGet routine.
01a,16jan98,jco  written, based on chrisc original file.
*/

/*
DESCRIPTION
This library provides a set of wrapper routines which emulates the Win32 API
used in the target server. The code must be compilable to run either on the
host or the target side.

INTERNAL
This code will be compiled using an ANSI compiler, so no need to worry about 
old C function interfacing like in other parts of VxWorks.
*/
 
#ifndef __INCwin32ThreadLibh
#define __INCwin32ThreadLibh
 
/* includes */
 
#ifdef __cplusplus
extern "C" {
#endif

#ifdef HOST
#include "host.h"
#else
#include "vxWorks.h"
#endif
 
/* typedefs */
 
typedef void *          HANDLE;
typedef unsigned int    DWORD;

/* defines */

#define INFINITE (-1)              /* wait forever */
#define IMMEDIATE (0)              /* no wait */

/* definition of WIN32 thread functions */

#if 0
/* Mutex creation */
 
extern HANDLE CreateMutex
    (
    void * security_attributes,   /* security attributes (not used) */
    BOOL ownership,               /* TRUE created EMPTY, FALSE created FULL */
    char * name                   /* name of the Mutex (not used) */
    );
 
/* Mutex Take function */
 
extern DWORD WaitForSingleObject
    (
    HANDLE Mutex,                 /* Mutex to wait for */
    DWORD  timeout                /* time to wait in milliseconds */
    );
 
/* Mutex Liberation */
 
extern BOOL ReleaseMutex
    (
    HANDLE mutex                  /* Mutex to release */
    );
 
/* Mutex Deletion */
 
extern BOOL CloseHandle
    (
    HANDLE mutex                  /* Mutex to destroy */
    );
 
#endif
#ifdef PARISC_HPUX10
/*
 * No default signal handler thread creation (to be used BEFORE any call to
 * _beginthread ()
 */

extern void noSignalHandlerThreadSet (void);

#endif

/* Thread Creation */
 
extern unsigned long _beginthread
    (
    void (*start_func)(void*),    /* thread starting point */
    unsigned int stack_size,      /* stack size for this thread */
    void * arguments              /* args given to the thread */
    );
 
/* Thread exit function */
 
extern void _endthread (void);

extern DWORD GetCurrentThreadId (void);	/* get the current Thread Id */


#ifdef __cplusplus
}
#endif
 
#endif /* __INCwin32ThreadLibh */
