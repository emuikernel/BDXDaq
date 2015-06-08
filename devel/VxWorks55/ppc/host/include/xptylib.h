/* xptylib.h - allocates pseudo-terminals; binds to unix commands or xterms */

/* Copyright 1992-1994 Wind River Systems, Inc. */

/* modification history
-----------------------
02h,28jun96,c_s  tweak includes for rs6000-aix4.
02g,19oct95,c_s  made ptySetEcho(), ptySetCanonical() external.
02f,08aug95,p_m  added ptySetRaw() prototype (SPR# 4628).
02e,15mar95,c_s  don't include <unistd.h> if GNUC; 2.6.0 has inconsistencies.
02d,09nov94,pme  added display parameter to ptyBindToXterm().
02c,21oct94,c_s  revamped, improved, renamed.  Interface narrowed to ptyBind
		   and ptyBindToXterm; job control in ptyBound processes now
		   handled correctly.
02b,17jun94,c_s  untweaked.  Changed interfaces to ptyBind to admit an 
		   argument vector.
02a,14dec93,gae  tweaked for the simulator.
01h,19aug93,c_s  Added ptySetNonBlocking (); mod history corrected.
01g,18aug93,c_s  Adjusted Ultrix port.
01f,17aug93,c_s  Added ptySetCanonical ().
01e,02aug93,c_s  Added #include of <sys/stropts.h> in the svr4 streams case.
01d,18sep92,c_s  Ported to Ultrix.
01c,16sep92,c_s  Ported to IRIX.  Adjusted port to HP-UX.
01b,10sep92,maf  conditionally include <sys/ioctl.h> or <sys/filio.h> based
		   on presence of HP-UX predefined macro "hpux."
01a,09jul92,c_s  Adapted from PtyConnection.h (in C++).
*/

#ifndef	INCxptylibh
#define	INCxptylibh

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

#if	defined(hpux) || defined(__hpux)
#define	_INCLUDE_XOPEN_SOURCE
#define	_INCLUDE_HPUX_SOURCE
#define	_INCLUDE_POSIX_SOURCE
#define	_INCLUDE_AES_SOURCE
#endif

#if defined(RS6000_AIX4) || defined(RS6000_AIX3)
#undef malloc
#endif

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/signal.h>
#include <fcntl.h>
#include <errno.h>
#ifndef RS6000_AIX4
#include <sys/file.h>
#endif /* RS6000_AIX4 */

#if	defined(__GNUC__)
/* GCC 2.6.0 has trouble with this: FIXME */
/* #include <unistd.h> */
#endif
#if	defined(hpux) || defined(__hpux)
#ifndef hpux
#define	hpux
#endif
#include <sys/ioctl.h>
#include <sys/termios.h>
#define	TCSETS	TCSETATTR
#define	TCGETS	TCGETATTR
#else
#if	defined(ultrix)
#include <sys/termios.h>
#include <sys/ioctl.h>
#define	TCSETS	TCSETA
#define	TCGETS	TCGETA
#else
#if	defined(sgi)
#include <sys/ioctl.h>
#include <sys/termio.h>
#define termios termio
#define TCSETS TCSETA
#define TCGETS TCGETA
#else
#if	defined(_AIX)
#include <sys/ioctl.h>
#include <termio.h>
#include <unistd.h>
#else
#if	defined(HAVE_STREAMS_TERMIO)
#include <sys/conf.h>
#include <sys/stropts.h>
#endif
#ifndef CENTERLINE_CLPP
#include <termios.h>
#endif /* CENTERLINE_CLPP */
#include <sys/filio.h>
#ifndef __GNUC__
/* GCC 2.6.0 has trouble with this FIXME */
#include <unistd.h>
#endif /* !__GNUC__ */
#endif
#endif
#endif
#endif

int  ptyBind	    (int *masterFd, char *pgm, char *argv [], int detach);
int  ptyBindToXterm (int *masterFd, char *title, char *display, int detach);
void ptySetRaw      (int fd);
void ptySetCanonical(int fd);
void ptySetEcho     (int fd, int echo);

#if defined(__cplusplus)
} /* extern "C" */
#endif /* __cplusplus */

#endif	/* INCxptylibh */
