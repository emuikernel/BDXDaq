/* ttyutil.h - tty header */

/* Copyright 1993-1998 Wind River Systems, Inc. */

/*
modification history
--------------------
01h,21jan98,c_c  Modified include section (for WIN32).
01g,28jun96,c_s  don't include sys/file.h on rs6000-aix4.
01f,30may95,p_m  removed #include <sys/ioctl.h>.
01e,30may95,p_m  added Solaris-2 and HP-UX  support.
01d,25apr95,c_s  removed "winsize" from ttyutil.h.
01c,15mar95,p_m  changed #include "vxWorks.h" to #include "host.h".
01b,25feb95,p_m  took care of WIN32 platform.
01a,24sep93,jcf  written.
*/

#ifndef __INCttyLibh
#define __INCttyLibh

#ifdef __cplusplus
extern "C" {
#endif

#ifndef WIN32
#include <sys/types.h>
#include <termios.h>
#include <sys/file.h>

#ifndef RS6000_AIX4
#include <sys/file.h>
#endif /* RS6000_AIX4 */

#endif /* WIN32*/
#include "host.h"

/* types */

#ifdef WIN32
typedef int TTY_MODE;
#else
typedef struct
    {
    struct termios      termios;
    } TTY_MODE;
#endif

/* function declarations */

extern STATUS   ttyModeGet
    (
    int fd,
    TTY_MODE *pTtyMode
    );

extern STATUS   ttyModeSet
    (
    int fd,
    TTY_MODE *pTtyMode
    );

extern STATUS   ttyStdSet
    (
    int fdStdIn,
    int fdStdOut,
    int fdStdErr
    );

extern STATUS   ttyStdGet
    (
    int *pFdStdIn,
    int *pFdStdOut,
    int *pFdStdErr
    );

#ifdef __cplusplus
}
#endif

#endif /* __INCttyLibh */
