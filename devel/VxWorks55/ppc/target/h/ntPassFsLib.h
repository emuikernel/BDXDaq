/* ntPassFsLib.h - pass-through file system library header */

/* Copyright 1984-1993 Wind River Systems, Inc. */

/*
modification history
--------------------
01c,07dec99,pai  moved HOST_BINARY to ntPassFsLib.h.
01b,07dec99,pai  fixed SPR 27918 by adding ARCHCVTFLAGS macro and
                 Win32 file control flag values to ntPassFsLib.h.
01a,03oct97,cym  written from passFsLib.h.
*/


#ifndef __INCntPassFsLibh
#define __INCntPassFsLibh


#ifdef __cplusplus
extern "C" {
#endif


#if CPU==SIMNT

/* Win32 API file control options used by open() and creat() */

#define	L_RDONLY        0
#define L_WRONLY        1
#define L_RDWR          2
#define L_ACCMODE       (L_RDONLY | L_WRONLY | L_RDWR)
#define	L_APPEND        0x0008
#define	L_CREAT         0x0100
#define	L_TRUNC         0x0200
#define	L_EXCL          0x0400

/* 
 * L_BINARY should match the O_BINARY value in fcntl.h from the toolchain
 * headers (in the host tree.)  For cygwin b19, it should be 0x10000.  For
 * mingw32 the value should be 0x8000.
 */
#define	L_BINARY        0x8000
#define HOST_BINARY     L_BINARY


/*******************************************************************************
*
* ARCHCVTFLAGS - map Win32 file flags to VxWorks file flags
*
* This macro tests the file mode flags passed in token <x> and, if necessary,
* converts the VxWorks flag to the corresponding Win32 flag.
*
* CAVEATS
* This macro is intended for ntPassFsLib internal use only.
*
* INTERNAL
* If a particular bit position is not tested in the macro, then it is
* effectively filtered out of the resulting value.
*
* VxWorks and Win32 currently all use the same values for most of the file
* mode flags passed to open() and creat().  However, some Win32 file flags
* have different values than those used in the VxWorks API.  Local versions
* of the Win32 modes are used to map VxWorks file mode flags to Win32 file
* mode flags.  This macro and method is identical to what has been done with
* the HP and Sun simulators in simLib.h.  The macro names are conditionally
* compiled by testing CPU==SIMNT just in case this code is combined with
* definitions in simLib.h.
*
* RETURNS:  A flags value converted to something Win32 understands.
*/
#define ARCHCVTFLAGS(x)                                \
    (                                                  \
    ((x) & O_RDONLY     ? L_RDONLY     : 0) |          \
    ((x) & O_WRONLY     ? L_WRONLY     : 0) |          \
    ((x) & O_RDWR       ? L_RDWR       : 0) |          \
    /* L_ACCMODE : already tested above    */          \
    ((x) & O_APPEND     ? L_APPEND     : 0) |          \
    ((x) & O_CREAT      ? L_CREAT      : 0) |          \
    ((x) & O_TRUNC      ? L_TRUNC      : 0) |          \
    ((x) & O_EXCL       ? L_EXCL       : 0) |          \
    ((x) & HOST_BINARY  ? L_BINARY     : 0)            \
    )

#endif /* CPU==SIMNT */


/* Function declarations */

#if defined(__STDC__) || defined(__cplusplus)

extern void *ntPassFsDevInit (char *devName);
extern STATUS ntPassFsInit (int nPassfs);
	      
#else	/* __STDC__ */

extern void *ntPassFsDevInit ();
extern STATUS ntPassFsInit ();

#endif	/* __STDC__ */

#ifdef __cplusplus
}
#endif

#endif /* __INCntPassFsLibh */
