/* host.h - header for host application code */

/* Copyright 1993-1998 Wind River Systems, Inc. */

/*
modification history
--------------------
02o,11sep01,fle  added MAX_HOST_NAME
02n,15dec99,dra  Added X86/FreeBSD host port support.
02m,18sep98,c_s  ansi C++ fixes

02l,01jul98,c_c  Added a define for WIN32 DLL exported routines.
02h,09may98,tcy  undef min and max if NOMINMAX is defined
02g,05may98,tcy  added HP-UX10 support; integrated some changes from 
		 tempest branch
02k,20apr98,dbt  use streams pty with HPUX10.
02j,25mar98,fle  added gethostname() declaration for SOLARIS
02i,11mar98,fle  added SVR_LISTEN_BACKLOG define
02h,29jan98,c_c  Corrected a typo for HPUX10.
02g,22jan98,c_c  DLLized Target Server Implementation.
		 Removed CPU types include. Added HP-UX 10 support.
02f,17oct96,bss  added check for external definition of ERROR (SPR# 7297).
02e,22jul96,c_s  rs6000-aix4 host port. Adjust #includes, work around AIX's
           +pad  definitions of BIG_ & LITTLE_ENDIAN, define HOST_TYPE.
                 Redefined malloc as a macro since malloc(0) is not valid
                 for AIX.
02d,17jul96,wmd  added code to undefine ERROR for WIN32 and move its definition
		 (spr# 6897).
02c,16feb96,pad  added CHECK_2_ALIGN macro.
02b,11nov95,sks  fix HOST_TYPE macro to be more specific for a WIN32 host.
02a,19oct95,c_s  guard SIGHANDLER from non-ANSI compilation environments.
01z,05oct95,c_s  HPUX pty fixes.
01y,20sep95,c_s  added #define of HAVE_STREAMS_TERMIO in SUN4_SOLARIS2 case
                 (SPR #4677, #4744).
01x,31aug95,c_s  added typedef SIGHANDLER; changed def of HOST_TYPE for WIN32.
01w,17jun95,p_m  removed space before #defines.
01v,11jun95,c_s  fixed UNPACK_L32.
01u,09jun95,s_w	 put in more WIN32 changes for g_h.
01t,07jun95,c_s  made PACK/UNPACK macros endian-safe.  
01s,30may95,s_w  put in WIN32 changes from Greg.
01r,30may95,p_m  added Solaris-2 and HP-UX  support.
01q,19may95,tpr  added TGT_ADDR_T, TGT_ARG_T and TGT_INT_T type definition.
01p,15may95,s_w  don't define strtoul and callrpc for WIN32
01o,14may95,s_w  added definitions of WTX_ERROR and WTX_OK macros. Also don't
		 define ERROR and OK if already defined. Added declaration
		 of strtol and gethostname for sun4-sunos4 GNU compilation.
01n,04may95,c_s  fixed UNPACK_{16,32} macros.
01m,04may95,pad  suppressed conflicting declaration for gcc 2.6.3
01l,20apr95,p_m  integrated more WIN32 changes.
01k,16apr95,p_m  made HOST_TYPE be sun4-sunos4 instead of sun4 and replaced
		 sun4 with SUN4_SUNOS4.
01j,16mar95,p_m  made basic types be typedef instead of #define.
01i,15mar95,p_m  completed basic types definitions and merged with old
		 vxWorks.h.  made all basic types be #defined instead 
		 of typedef in order to allow external definitions of 
		 basic types.  This is needed on some Microsoft platforms.
01h,13mar95,c_s  added PACK_16 and PACK_32, the analogues of the UNPACK macros.
01g,28feb95,p_m  added WIN32 stuff.
01f,30jan95,tpr  added HOST_TYPE.
01e,20jan95,jcf  made more portable.
01d,15dec94,pad  added some casting to please acc.
01c,07dec94,pad  added several macro definitions (SWAB_16, SWAB_16_IF,
		 UNPACK_16...)
01b,14feb94,c_s  added material for HP-UX port.
01a,27sep93,c_s  written.
*/

#ifndef __INChosth
#define __INChosth

#ifdef	__cplusplus
extern "C" {
#endif	/* __cplusplus */

/* basic types defines */

#ifndef	STATUS_T
#define		STATUS_T	int
#endif
#ifndef BOOL_T
#define 	BOOL_T		int
#endif
#ifndef INT8_T
#define		INT8_T		char
#endif
#ifndef UINT8_T
#define 	UINT8_T		unsigned char
#endif
#ifndef INT16_T
#define		INT16_T		short
#endif
#ifndef UINT16_T
#define		UINT16_T	unsigned short
#endif
#ifndef INT32_T
#define		INT32_T		int
#endif
#ifndef UINT32_T
#define		UINT32_T	unsigned int
#endif
#ifndef UCHAR_T
#define		UCHAR_T		unsigned char
#endif
#ifndef USHORT_T
#define		USHORT_T	unsigned short
#endif
#ifndef UINT_T
#define 	UINT_T		unsigned int
#endif 
#ifndef ULONG_T
#define		ULONG_T		unsigned long
#endif

/* typedefs */

typedef unsigned long	TGT_ADDR_T;	/* target address */
typedef unsigned long	TGT_ARG_T;	/* target function argument */
typedef long		TGT_INT_T;	/* target function integer */

/* Basic type definitions to be used for portability */

typedef STATUS_T	STATUS;		/* return status */
typedef BOOL_T		BOOL;		/* boolean */
typedef INT8_T		INT8;		/* 8 bits signed integer */
typedef UINT8_T		UINT8;		/* 8 bits unsigned integer */
typedef INT16_T		INT16;		/* 16 bits signed integer */
typedef UINT16_T	UINT16;		/* 16 bits unsigned integer */
typedef INT32_T		INT32;		/* 8 bits unsigned integer */
typedef UINT32_T	UINT32;		/* 32 bits unsigned integer */

/* obsolete types */

/* 
 * The 4 types below are needed for backward compatibility with VxWorks
 * code reused in the target server and tools.
 * These types should not be used, instead use the types defined above 
 * (INT8, INT8, etc...)
 */

typedef UCHAR_T		UCHAR;		/* 8 bits unsigned character */
typedef USHORT_T	USHORT;		/* 16 bits unsigned integer */
typedef UINT_T		UINT;		/* 32 bits unsigned integer */
typedef ULONG_T		ULONG;		/* 32 bits unsigned long */

/*
 * Most compilation environments say the prototype of a signal handler
 * is void (*) (int).  But SunOS 4 insists on void (*) (int, ...).
 */

#if defined (__STDC__) || defined (__cplusplus)
#ifdef sun4
typedef void (*SIGHANDLER) (int, ...);
#else
typedef void (*SIGHANDLER) (int);
#endif
#endif /* __STDC__ || __cplusplus */

#ifdef	__cplusplus
typedef int 		(*FUNCPTR) (...);	/* pfunction returning int */
typedef void 		(*VOIDFUNCPTR) (...);	/* pfunction returning void */
typedef double 		(*DBLFUNCPTR) (...);	/* pfunction returning double*/
typedef float 		(*FLTFUNCPTR) (...);	/* pfunction returning float */
#else
typedef int 		(*FUNCPTR) ();		/* pfunction returning int */
typedef void 		(*VOIDFUNCPTR) (); 	/* pfunction returning void */
typedef double 		(*DBLFUNCPTR) ();  	/* pfunction returning double*/
typedef float 		(*FLTFUNCPTR) ();  	/* pfunction returning float */
#endif	/* __cplusplus */

#ifdef	__cplusplus
}
#endif	/* __cplusplus */

/* declarations */

#if defined(SUN4_SUNOS4) && defined(__GNUC__)
#   ifndef __cplusplus

    /* The GNU Ansi compilation environment for SunOS 4 seems to omit 
       the following prototypes from <stdlib.h>, <stdio.h>, etc.; we provide
       them here. */

    extern int 		printf (const char *, ...);
    extern int 		system (const char *);
    extern char *	memchr (const void *, int, int);
    extern void *	memmove (void *, const void *, int);
    extern char *	memset (void *, int, int);
    extern int		fclose (void *);
    extern int		fflush (void *);
    extern int		fprintf (void *, const char *, ...);
    extern void		free (void *);
    extern int		fscanf (void *, const char *, ...);
    extern int		scanf (const char *, ...);
    extern int		sscanf (const char *, const char *, ...);
    extern unsigned long int strtoul(const char *, char **, int);
    extern long int	strtol(const char *, char **, int);
    extern int		tolower (int);
    extern int		toupper (int);
    extern int		vfprintf (void *, const char *, void *);
    extern int		gethostname (char *, int);

#   else /* !__cplusplus */

    extern "C" int callrpc (...);

#   endif /* !__cplusplus */
#else /* SUN4_SUNOS4 && __GNUC__ */

/* Both CenterLine and SparcWorks need definition of strtoul (),
   in the SunOS4 case. */

#   ifndef WIN32
#   ifndef SUN4_SOLARIS2 
#   ifndef RS6000_AIX4
#   ifndef PARISC_HPUX10
#   ifndef __hpux
#       ifdef __cplusplus

        extern "C" unsigned long strtoul (char *, char **, int); 
        extern "C" int callrpc (...);

#       endif /* __cplusplus */
#   endif /* !__hpux */
#   endif /* !SUN4_SOLARIS2 */
#   endif /* !PARISC_HPUX10 */
#   endif /* !RS6000_AIX4 */
#   endif /* !WIN32 */

#endif /* SUN4_SUNOS4 && __GNUC__ */

#if defined (SUN4_SOLARIS2) && defined (__GNUC__)
    extern int		gethostname (char *, int);
#endif /* SUN4_SOLARIS2 && __GNUC__ */


/* includes */

#ifndef WIN32
#include <limits.h>
#include <sys/types.h>
#endif /* WIN32 */

#ifdef RS6000_AIX4
/*
 * <machine.h> on AIX defines BIG_ and LITTLE_ENDIAN, which makes life
 * tough on us.  We prevent the expansion of the content of that file
 * by defining the following macro.
 */
#undef  _H_MACHINE
#define _H_MACHINE 1

/*
 * Unlike probably all the other UNIX flavors in the world (and Windows),
 * AIX just considers (somewhat logically though) malloc(0) as an invalid
 * call. This is not convenient in the loader (at least) where some malloc
 * make use of the result of a calculus as parameter. So we simply allocate
 * 4 bytes when the parameter is null.
 */
#define malloc(x)       (((x) == 0) ? (malloc (4)) : (malloc ((x))))
#endif /* RS6000_AIX4 */

/* definitions */

#define FAST		register	/* should be removed one day */

#undef  BIG_ENDIAN
#define BIG_ENDIAN	0
#undef  LITTLE_ENDIAN
#define LITTLE_ENDIAN	1

#if defined(SUN4_SUNOS4)  || defined(SUN4_SOLARIS2) ||\
    defined(PARISC_HPUX9) || defined (PARISC_HPUX10) || defined(RS6000_AIX4)
#define HOST_BYTE_ORDER		BIG_ENDIAN
#else
#define HOST_BYTE_ORDER		LITTLE_ENDIAN
#endif

#if defined(SUN4_SOLARIS2) || defined(PARISC_HPUX10)
#define HAVE_STREAMS_TERMIO
#define PTY_MULTIPLEX_DEVICE "/dev/ptmx"
#endif /* SUN4_SOLARIS2 */

#if defined (PARISC_HPUX9) || defined (PARISC_HPUX10)
/* the following are used in R5 xterm to deal with HPUX. */
#define PTY_DEVICE_ROOT "/dev/ptym/ptyXX"
#define TTY_DEVICE_ROOT "/dev/pty/ttyXX"
#define PTY_CHAR1 "zyxwvutsrqp"
#define PTY_CHAR2 "fedcba9876543210"
#endif /* PARISC_HPUX[9|10] */

#ifdef RS6000_AIX4
#define HAVE_STREAMS_TERMIO
#define PTY_MULTIPLEX_DEVICE "/dev/ptc"
#endif /* RS6000_AIX4 */

#ifndef NAME_MAX
#define NAME_MAX		_POSIX_NAME_MAX
#endif /* ifndef NAME_MAX */

#ifndef PATH_MAX
#define PATH_MAX		_POSIX_PATH_MAX
#endif /* ifndef PATH_MAX */
#define MAX_FILENAME_LENGTH	(_POSIX_PATH_MAX + 1)

/* memory alignment macros */

#define ROUND_UP(x, align)      (((int) (x) + (align - 1)) & ~(align - 1))
#define ROUND_DOWN(x, align)    ((int)(x) & ~(align - 1))
#define ALIGNED(x, align)       (((int)(x) & (align - 1)) == 0)

#define _ALLOC_ALIGN_SIZE	0x4
#define MEM_ROUND_UP(x)         ROUND_UP(x, _ALLOC_ALIGN_SIZE)
#define MEM_ROUND_DOWN(x)       ROUND_DOWN(x, _ALLOC_ALIGN_SIZE)
#define MEM_ALIGNED(x)          ALIGNED(x, _ALLOC_ALIGN_SIZE)

#if defined SUN4_SUNOS4
#define HOST_TYPE	"sun4-sunos4"
#elif defined SUN4_SOLARIS2
#define HOST_TYPE	"sun4-solaris2"
#elif defined PARISC_HPUX9
#define HOST_TYPE	"parisc-hpux9"
#elif defined PARISC_HPUX10
#define HOST_TYPE	"parisc-hpux10"
#elif defined decmips
#define HOST_TYPE	"decmips"
#elif defined iris
#define HOST_TYPE	"iris"
#elif defined magnum
#define HOST_TYPE	"magnum"
#elif defined RS6000_AIX4
#define HOST_TYPE	"rs6000-aix4"
#elif (defined(_M_IX86) && defined(_WIN32))
#define HOST_TYPE	"x86-win32"
#endif


#define IMPORT		extern
#define LOCAL		static
#ifdef WIN32
#define DLL_EXPORT	__declspec (dllexport)
#define DLL_IMPORT	__declspec (dllimport)
#else
#define DLL_EXPORT
#define DLL_IMPORT
#endif

#if	!defined(NULL)
#define NULL		0
#endif

#if	!defined(EOF) || (EOF!=(-1))
#define EOF		(-1)
#endif

#if	!defined(FALSE) || (FALSE!=0)
#define FALSE		0
#endif

#if	!defined(TRUE) || (TRUE!=1)
#define TRUE		1
#endif

#define NONE		(-1)	/* for times when NULL won't do */
#define EOS		'\0'	/* C string terminator */

/* I/O input, output, error fd's */

#define	STD_IN	0
#define	STD_OUT	1
#define	STD_ERR	2

/* common macros */

#define MSB(x)	(((x) >> 8) & 0xff)	/* most signif byte of 2-byte integer */
#define LSB(x)	((x) & 0xff)		/* least signif byte of 2-byte integer*/

/* swap the MSW with the LSW of a 32 bit integer */
#define WORDSWAP(x) (MSW(x) | (LSW(x) << 16))

#define LLSB(x)	((x) & 0xff)		/* 32bit word byte/word swap macros */
#define LNLSB(x) (((x) >> 8) & 0xff)
#define LNMSB(x) (((x) >> 16) & 0xff)
#define LMSB(x)	 (((x) >> 24) & 0xff)
#define LONGSWAP(x) ((LLSB(x) << 24) | \
		     (LNLSB(x) << 16)| \
		     (LNMSB(x) << 8) | \
		     (LMSB(x)))

/* Swap of two bytes : AB becomes BA */

#define SWAB_16(x)	(((UINT16)((x) & 0x00ff) << 8) | \
			 ((UINT16)((x) & 0xff00) >> 8))

/* Swap of four bytes : ABCD becomes DCBA */

#define SWAB_32(x)	(((UINT32)((x) & 0x000000ff) << 24) | \
			 ((UINT32)((x) & 0x0000ff00) << 8)  | \
			 ((UINT32)((x) & 0x00ff0000) >> 8)  | \
			 ((UINT32)((x) & 0xff000000) >> 24))

/* Swap only if the boolean (b) is true */

#define SWAB_16_IF(b,v)		((v) = ((b) ? SWAB_16 (v) : (v)))
#define SWAB_32_IF(b,v)		((v) = ((b) ? SWAB_32 (v) : (v)))

/*
 * The following macros allow to read 16 bit or 32 bit data whatever address
 * is used. If the address is 4-byte aligned, then the bits are read in one
 * chunk. Otherwise, they are read as a succession of bytes.
 */

#define UCHP(p)		(*(UINT8 *)(p))
#define UCHPO(p,o)	(*(((UINT8 *)p)+(o)))

#define UNPACK_16(adr)   ((HOST_BYTE_ORDER == BIG_ENDIAN) \
                         ? (UNPACK_B16 (adr))            \
			 : (UNPACK_L16 (adr)))

#define UNPACK_B16(adr)	((UINT16)(UCHP(adr) << 8 | UCHPO(adr,1)))
#define UNPACK_L16(adr)	((UINT16)(UCHPO(adr,1) << 8 | UCHP(adr)))

#define PACK_16(adr,val) 	((HOST_BYTE_ORDER == BIG_ENDIAN)	\
                                ? (PACK_B16 (adr,val))			\
                                : (PACK_L16 (adr,val)))

#define PACK_B16(adr,val)	(UCHP(adr)    = ((val)>>8 )&0xff, \
				 UCHPO(adr,1) =  (val)     &0xff)
#define PACK_L16(adr,val)	(UCHPO(adr,1) = ((val)>>8 )&0xff, \
				 UCHP(adr)    =  (val)     &0xff)


#define UNPACK_32(adr)	((HOST_BYTE_ORDER == BIG_ENDIAN) \
                         ? (UNPACK_B32 (adr))            \
			 : (UNPACK_L32 (adr)))

#define UNPACK_B32(adr)	((UINT32)(UCHP(adr)    << 24 |    \
                                  UCHPO(adr,1) << 16 |    \
			          UCHPO(adr,2) << 8  |    \
			          UCHPO(adr,3)))


#define UNPACK_L32(adr)	((UINT32)(UCHPO(adr,3) << 24 |    \
                                  UCHPO(adr,2) << 16 |    \
			          UCHPO(adr,1) << 8  |    \
			          UCHP(adr)))

#define PACK_32(adr,val) 	((HOST_BYTE_ORDER == BIG_ENDIAN)	\
                                ? (PACK_B32 (adr,val))			\
                                : (PACK_L32 (adr,val)))

#define PACK_B32(adr,val)	(UCHP(adr)     = ((val)>>24)&0xff, \
				 UCHPO(adr,1)  = ((val)>>16)&0xff, \
				 UCHPO(adr,2)  = ((val)>>8) &0xff, \
				 UCHPO(adr,3)  =  (val)     &0xff)

#define PACK_L32(adr,val)	(UCHPO(adr,3)  = ((val)>>24)&0xff, \
				 UCHPO(adr,2)  = ((val)>>16)&0xff, \
				 UCHPO(adr,1)  = ((val)>>8) &0xff, \
				 UCHP(adr)     =  (val)     &0xff)

#define OFFSET(structure, member)	/* byte offset of member in structure*/\
		((int) &(((structure *) 0) -> member))

#define MEMBER_SIZE(structure, member)	/* size of a member of a structure */\
		(sizeof (((structure *) 0) -> member))

#define NELEMENTS(array)		/* number of elements in an array */ \
		(sizeof (array) / sizeof ((array) [0]))

#define FOREVER	for (;;)

/* macro to check if alignment is a power of 2 */

#define CHECK_2_ALIGN(x)	(((x) & ((x) - 1)) == 0)

#if !defined(WIN32) || defined(__STDC__) 
#ifndef NOMINMAX
#ifndef max
#define max(x, y)	(((x) < (y)) ? (y) : (x))
#endif /* !max */

#ifndef min
#define min(x, y)	(((x) < (y)) ? (x) : (y))
#endif /* !min */
#endif /* !NOMINMAX */
#endif /* !defined(WIN32) || defined(__STDC__) */

/* SUN4_SOLARIS2 host specific defines and includes */
 
#ifdef SUN4_SOLARIS2
/* all posix systems must define these and wrs uses these values */
#define _POSIX_ARG_MAX          4096
#define _POSIX_CHILD_MAX        6
#define _POSIX_LINK_MAX         8
#define _POSIX_MAX_CANON        255
#define _POSIX_MAX_INPUT        255
#define _POSIX_NAME_MAX         14
#define _POSIX_NGROUPS_MAX      0
#define _POSIX_OPEN_MAX         16
#define _POSIX_PATH_MAX         255
#define _POSIX_PIPE_BUF         512
#endif /* SUN4_SOLARIS2 */

/* WIN32 host specific defines and includes */

#ifdef WIN32
#define WIN32_COMPILER

#include <windows.h>

#include <fcntl.h>
#include <io.h>
#include <stdlib.h>
#include <stddef.h>

#define DATE __DATE__

/* all posix systems must define these and wrs uses these values */

#define	_POSIX_ARG_MAX		4096
#define	_POSIX_CHILD_MAX	6
#define	_POSIX_LINK_MAX		8
#define	_POSIX_MAX_CANON	255
#define	_POSIX_MAX_INPUT	255
#define	_POSIX_NAME_MAX		14
#define	_POSIX_NGROUPS_MAX	0
#define	_POSIX_OPEN_MAX		16
#define	_POSIX_PATH_MAX		255
#define	_POSIX_PIPE_BUF		512

/* defined in sys\param.h on unix */
#define MAXPATHLEN          1024

#ifndef O_RDONLY
#define O_RDONLY _O_RDONLY
#endif
#ifndef O_WRONLY
#define O_WRONLY _O_WRONLY
#endif
#ifndef O_RDWR
#define O_RDWR   _O_RDWR
#endif
#ifndef O_CREAT
#define O_CREAT  _O_CREAT
#endif

/* unistd.h access() define */
#define R_OK     4

#ifndef open
#define open    _open
#endif
#ifndef close 
#define close   _close
#endif
#ifndef popen 
#define popen   _popen
#endif
#ifndef pclose 
#define pclose  _pclose
#endif
#ifndef isascii 
#define isascii __isascii
#endif
#ifndef write 
#define write   _write
#endif
#ifndef read 
#define read    _read
#endif
#ifndef strdup 
#define strdup  _strdup
#endif
#ifndef sleep 
#define sleep 	Sleep
#endif

typedef long  _off_t;                   /* may belong in "sys/types.h" */
#define off_t _off_t

/* define replacements for bzero and bcopy */

#define bzero(a,b) memset(a,'\0',b)
#define bcopy(s,d,c) memmove(d,s,c)

#include <signal.h>           /* MS include for signal values */
#ifndef SIGQUIT
#define SIGQUIT SIGTERM       /* used by kill */
#endif
#ifndef ECONNRESET
#define ECONNRESET 0          /* unknown define set in clnt_tty.c util_lib */
#endif

#undef ERROR

#endif /* ifdef WIN32 */

/* return status values */

#if	!defined(OK)
#define OK		0
#endif

/* 
 * SPR# 7297 - Check if ERROR is externally defined to
 * avoid clash with Microsoft's definition.
 */

#if defined(ERROR)  && (ERROR != -1)
#error "Macro ERROR is defined to bogus value outside of host.h!"
#error "ERROR redefined to (-1)."
#undef ERROR
#endif

#define ERROR (-1)

#define WTX_OK		0
#define WTX_ERROR	(-1)

/* target server max pending connection to socket */

#define SVR_LISTEN_BACKLOG	2

/* host name max length */

#ifdef MAXHOSTNAMELEN
#   define MAX_HOST_NAME	MAXHOSTNAMELEN
#else
#   define MAX_HOST_NAME	64
#endif /* MAXHOSTNAMELEN */

#endif /* __INChosth */
