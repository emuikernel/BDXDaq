#ifndef _IOINCL_ 
/* ioincl.h - not for MACII !!!*/

#if defined(MTHREAD_IO) &&  defined(SunOS)
#define _REENTRANT
#include <pthread.h>
#endif
 
#ifdef SunOS
#include <unistd.h>
#endif
 
#include <stdio.h>
#include <errno.h>
 
#ifndef VAXVMS
#include <sys/types.h>
#endif
 
#ifndef SEEK_CUR

/* on some machines (Alliant) these macros are not
   defined in stdio.h   */

#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 2
#endif
/* We keep file pointers in an array in C, rather than passing as ints
 * back to Fortran.  This is implemented for 64-bit Alphas, but it'll
 * work on any machine. */
#ifndef NUNITS
#define NUNITS 40
#endif

extern FILE *filepointer[NUNITS];

/* extern int  *bufpointer[NUNITS] */

 
#ifndef VAXVMS
/* includes needed for Exabyte I/O */
#ifdef AIX
#include <sys/tape.h>
#else
#include <sys/mtio.h>
#endif
#endif
 
#if defined(ALLIANT) || defined(ULTRIX) || defined(ALPHA) || defined(SunOS)
#include <sys/ioctl.h>
#endif

#include "bosio.h"

#define _IOINCL_
#endif
 
