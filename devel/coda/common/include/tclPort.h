/*
 * tclPort.h --
 *
 *	This header file handles porting issues that occur because
 *	of differences between systems.  It reads in UNIX-related
 *	header files and sets up UNIX-related macros for Tcl's UNIX
 *	core.  It should be the only file that contains #ifdefs to
 *	handle different flavors of UNIX.  This file sets up the
 *	union of all UNIX-related things needed by any of the Tcl
 *	core files.  This file depends on configuration #defines such
 *	as NO_DIRENT_H that are set up by the "configure" script.
 *
 *	Much of the material in this file was originally contributed
 *	by Karl Lehenbauer, Mark Diekhans and Peter da Silva.
 *
 * Copyright (c) 1991-1994 The Regents of the University of California.
 * Copyright (c) 1994 Sun Microsystems, Inc.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 * @(#) tclPort.h 1.6 95/05/11 10:46:38
 */

#ifndef _TCLPORT
#define _TCLPORT

#include <errno.h>
#include <fcntl.h>

#ifndef TCL_VW
#  include <pwd.h>
#  include <signal.h>
#  include <sys/param.h>
#  include <sys/types.h>
#endif /* !TCL_VW */

#ifdef USE_DIRENT2_H
#   include "compat/dirent2.h"
#else
#   ifdef NO_DIRENT_H
#	include "compat/dirent.h"
#   else
#	include <dirent.h>
#   endif
#endif

#ifndef TCL_VW
#  include <sys/file.h>
#  include <sys/stat.h>
#  ifndef NO_SYS_TIME_H
#     include <sys/time.h>
#  else
#     include <time.h>
#  endif
#  ifndef NO_SYS_WAIT_H
#     include <sys/wait.h>
#  endif
#  ifdef HAVE_UNISTD_H
#     include <unistd.h>
#  else
#     include "compat/unistd.h"
#  endif
#endif /* !TCL_VW */

/*
 * Not all systems declare the errno variable in errno.h. so this
 * file does it explicitly.  The list of system error messages also
 * isn't generally declared in a header file anywhere.
 */

extern int errno;

#ifndef TCL_VW
/*
 * The type of the status returned by wait varies from UNIX system
 * to UNIX system.  The macro below defines it:
 */
 
# ifdef AIX
#   define WAIT_STATUS_TYPE pid_t
# else
#   ifndef NO_UNION_WAIT
#      define WAIT_STATUS_TYPE union wait
#   else
#      define WAIT_STATUS_TYPE int
#   endif
# endif
#else
# include <stat.h>
# undef S_IFLNK		/* don't have lstat(), readlink() */
#endif /* !TCL_VW */

/*
 * Supply definitions for macros to query wait status, if not already
 * defined in header files above.
 */

#ifndef WIFEXITED
#   define WIFEXITED(stat)  (((*((int *) &(stat))) & 0xff) == 0)
#endif

#ifndef WEXITSTATUS
#   define WEXITSTATUS(stat) (((*((int *) &(stat))) >> 8) & 0xff)
#endif

#ifndef WIFSIGNALED
#   define WIFSIGNALED(stat) (((*((int *) &(stat)))) && ((*((int *) &(stat))) == ((*((int *) &(stat))) & 0x00ff)))
#endif

#ifndef WTERMSIG
#   define WTERMSIG(stat)    ((*((int *) &(stat))) & 0x7f)
#endif

#ifndef WIFSTOPPED
#   define WIFSTOPPED(stat)  (((*((int *) &(stat))) & 0xff) == 0177)
#endif

#ifndef WSTOPSIG
#   define WSTOPSIG(stat)    (((*((int *) &(stat))) >> 8) & 0xff)
#endif

/*
 * Define constants for waitpid() system call if they aren't defined
 * by a system header file.
 */

#ifndef WNOHANG
#   define WNOHANG 1
#endif
#ifndef WUNTRACED
#   define WUNTRACED 2
#endif

/*
 * Supply macros for seek offsets, if they're not already provided by
 * an include file.
 */

#ifndef SEEK_SET
#   define SEEK_SET 0
#endif

#ifndef SEEK_CUR
#   define SEEK_CUR 1
#endif

#ifndef SEEK_END
#   define SEEK_END 2
#endif

/*
 * The stuff below is needed by the "time" command.  If this
 * system has no gettimeofday call, then must use times and the
 * CLK_TCK #define (from sys/param.h) to compute elapsed time.
 * Unfortunately, some systems only have HZ and no CLK_TCK, and
 * some might not even have HZ.
 */

#ifdef NO_GETTOD
#   ifdef TCL_VW
      struct tms {int foo;}; 	/* we don't need a real definition */
#     include <time.h>
#   else
#     include <sys/times.h>
#     include <sys/param.h>
#   endif /* TCL_VW */
#   ifndef CLK_TCK
#       ifdef HZ
#           define CLK_TCK HZ
#       else
#           define CLK_TCK 60
#       endif
#   endif
#else
#   ifdef HAVE_BSDGETTIMEOFDAY
#	define gettimeofday BSDgettimeofday
#   endif
#endif

#ifdef GETTOD_NOT_DECLARED
EXTERN int		gettimeofday _ANSI_ARGS_((struct timeval *tp,
			    struct timezone *tzp));
#endif

/*
 * Define access mode constants if they aren't already defined.
 */

#ifndef F_OK
#    define F_OK 00
#endif
#ifndef X_OK
#    define X_OK 01
#endif
#ifndef W_OK
#    define W_OK 02
#endif
#ifndef R_OK
#    define R_OK 04
#endif

/*
 * Define FD_CLOEEXEC (the close-on-exec flag bit) if it isn't
 * already defined.
 */

#ifndef FD_CLOEXEC
#   define FD_CLOEXEC 1
#endif

/*
 * On systems without symbolic links (i.e. S_IFLNK isn't defined)
 * define "lstat" to use "stat" instead.
 */

#ifndef S_IFLNK
#   define lstat stat
#endif

/*
 * Define macros to query file type bits, if they're not already
 * defined.
 */

#ifndef S_ISREG
#   ifdef S_IFREG
#       define S_ISREG(m) (((m) & S_IFMT) == S_IFREG)
#   else
#       define S_ISREG(m) 0
#   endif
# endif
#ifndef S_ISDIR
#   ifdef S_IFDIR
#       define S_ISDIR(m) (((m) & S_IFMT) == S_IFDIR)
#   else
#       define S_ISDIR(m) 0
#   endif
# endif
#ifndef S_ISCHR
#   ifdef S_IFCHR
#       define S_ISCHR(m) (((m) & S_IFMT) == S_IFCHR)
#   else
#       define S_ISCHR(m) 0
#   endif
# endif
#ifndef S_ISBLK
#   ifdef S_IFBLK
#       define S_ISBLK(m) (((m) & S_IFMT) == S_IFBLK)
#   else
#       define S_ISBLK(m) 0
#   endif
# endif
#ifndef S_ISFIFO
#   ifdef S_IFIFO
#       define S_ISFIFO(m) (((m) & S_IFMT) == S_IFIFO)
#   else
#       define S_ISFIFO(m) 0
#   endif
# endif
#ifndef S_ISLNK
#   ifdef S_IFLNK
#       define S_ISLNK(m) (((m) & S_IFMT) == S_IFLNK)
#   else
#       define S_ISLNK(m) 0
#   endif
# endif
#ifndef S_ISSOCK
#   ifdef S_IFSOCK
#       define S_ISSOCK(m) (((m) & S_IFMT) == S_IFSOCK)
#   else
#       define S_ISSOCK(m) 0
#   endif
# endif

/*
 * Make sure that MAXPATHLEN is defined.
 */

#ifndef MAXPATHLEN
#   ifdef PATH_MAX
#       define MAXPATHLEN PATH_MAX
#   else
#       define MAXPATHLEN 2048
#   endif
#endif

/*
 * Make sure that L_tmpnam is defined.
 */

#ifndef L_tmpnam
#   define L_tmpnam 100
#endif

/*
 * Substitute Tcl's own versions for several system calls.  The
 * Tcl versions retry automatically if interrupted by signals.
 * (see tclUnixUtil.c).
 */

#define open(a,b,c) TclOpen(a,b,c)
#define read(a,b,c) TclRead(a,b,c)
#define waitpid(a,b,c) TclWaitpid(a,b,c)
#define write(a,b,c) TclWrite(a,b,c)
EXTERN int	TclOpen _ANSI_ARGS_((char *path, int oflag, int mode));
EXTERN int	TclRead _ANSI_ARGS_((int fd, VOID *buf, size_t numBytes));
EXTERN int	TclWaitpid _ANSI_ARGS_((pid_t pid, int *statPtr, int options));
EXTERN int	TclWrite _ANSI_ARGS_((int fd, VOID *buf, size_t numBytes));

/*
 * Variables provided by the C library:
 */

#if defined(_sgi) || defined(__sgi)
#define environ _environ
#endif
extern char **environ;

#ifdef TCL_VW
/* vxWorks' chdir() implementation is limimted to absolute pathnames.
 * But they've got ioDefPathCat() which (as far as I can tell) implements
 * chdir() just fine.
 */
#define chdir(x)        ioDefPathCat(x)

/* Reentrant strerror-type function */
extern char *vxStrError    _ANSI_ARGS_((int errno, char *buf));
#endif /* TCL_VW */


#endif /* _TCLPORT */
