/* usrExtra.c - optionally included modules */

/* Copyright 1992-2002 Wind River Systems, Inc. */

/*
modification history
--------------------
02x,13mar02,sn   SPR 74275 -  added INCLUDE_NO_CPLUS_DEMANGLER
02w,11mar02,mas  updated to use usrSmObj.c configlette (SPR 73371)
02v,11mar02,sn   Wind Foundation classes no longer supported (SPR #73967)
02u,11mar02,sn   removed INCLUDE_GNU_64BIT (now part of INCLUDE_GNU_INTRINSICS)
02t,27feb02,sn   move intrinsics pull-in code to intrinsics.c
02s,18jan02,h_k  removed SH intrinsics.
02r,03jan02,mrs  Fix to conform to ANSI standard for token pasting.
02q,12dec01,sn   Fix TOOL_CONFIG_HEADER
02p,10dec01,jlb  Change UGL to WindML
02o,07dec01,sn   added INCLUDE_CPLUS_DEMANGLER
02n,06nov01,sn   use toolchain specific C++ components
02m,30oct01,sn   added INCLUDE_GNU_INTRINSICS, INCLUDE_DIAB_INTRINSICS
                 removed MIPS intrinsics (obsolete now we're building with Diab)  
02l,03oct01,jkf  added dosFs2 declarations to match components in 10dosfs2.cdf
02k,27sep01,jkf  removed dosFs non-ansi declarations.
02i,10jul01,mem  Updated MIPS intrinsics.
02j,13jul01,kab  Cleanup for merge to mainline
02i,10may01,dtr  Adding in usrAltivec.c.
02h,16nov98,jpd  added use of INCLUDE_MMU_MPU.
02b,30jun00,zl   added dosFs 2.0 related functions
02a,08jun00,csi  Merging dir for vxfusion
01z,03mar00,zl   merged SH support into T2
02c,03apr01,max  introducing AltiVec save/restore entries for vector registers
02b,24jan01,sn   added long long support
02a,11oct00,sn   updated for new C++ components
01y,10mar99,drm  changed WINDMP to VXFUSION to reflect new product name
01x,11feb99,sn   corrected logic that defines INCLUDE_CPLUS_STRING
01w,26jan99,jco  merged from graphics2_0 branch
01v,29may98,bjl  added INCLUDE_CODETEST.
02g,01sep98,drm  adding conditional include of usrWindMP.c
02f,08jul98,cym  removed undef of ANSI_MATH for SIMNT.
02e,03jun98,sn   cleanup up mod history.
                 added new C++ configuration options splitting between
                 basic, Standard Template Library, string class, complex class
		 and iostreams. 
		 distinguished INCLUDE_CPLUS_IOSTREAMS
		 (everything needed by iostream.o) from 
		 INCLUDE_CPLUS_IOSTREAMS_FULL.
		 removed INCLUDE_CPLUS_HEAP and INCLUDE_CPLUS_BOOCH since
		 they are no-ops.
02d,13may98,dbt  removed optional modules included by WDB since the scalable
		 agent has be redesigned.

02c,13jan98,dbt  adding conditional include of usrBreakpoint.c
02b,18dec97,cth  added conditional include of usrWindview.c
02a,19nov97,hdn  added support for TFFS.
01u,03dec96,tpr  removed the reference to __cmpsf2 for the I960 familly.
01t,01nov96,hdn  added support for PCMCIA.
01s,23jul96,hdn  added support for ATA driver.
01r,23jun95,tpr  added #ifdef INCLUDE_WDB
01q,20jun95,srh  make iostreams separately configurable.
01p,14jun95,srh  normalized names of C++ init files.
01o,13jun95,srh  added triggers for optional C++ products.
01n,20may95,jcf  added WDB.
01m,29mar95,kdl  added INCLUDE_GCC_FP; added reference to __fixunssfsi();
		 made MC68060 use same gcc support as MC68040.
01l,22aug94,dzb  added INCLUDE_NETWORK conditional (SPR #1147).
01k,06nov94,tmk  added MC68LC040 support
01j,16jun94,tpr  Added MC68060 cpu support.
01i,25oct93,hdn  added support for floppy and IDE device.
01h,25mar94,jmm  changed C++ to check for either full or min, not both
01g,08apr94,kdl  changed aio_show() reference to aioShow().
01f,28jan94,dvs  changed INCLUDE_POSIX_MEM_MAN to INCLUDE_POSIX_MEM
01e,02dec93,dvs  added POSIX A-I/O show configuration
01d,22nov93,dvs  added INCLUDE_POSIX_SCHED, INCLUDE_POSIX_MMAN, and 
		 INCLUDE_POSIX_FTRUNCATE
01c,23aug93,srh  Added INCLUDE_CPLUS_MIN to the conditions for making
		   sysCplusEnable TRUE.
01b,30oct92,smb  forced include of libfpg.a for 960
01a,18sep92,jcf  written.
*/

/*
DESCRIPTION
This file is used to include extra modules necessary to satisfy the desired
configuration specified by configAll.h and config.h. This file is included
by usrConfig.c.

SEE ALSO: usrDepend.c

NOMANUAL
*/

#ifndef  __INCusrExtrac
#define  __INCusrExtrac 

/*
 * Extra modules to be included in VxWorks.  These modules are
 * not referred in the VxWorks code, so they will not be
 * automatically linked in unless they are defined here explicitly.
 *
 */


extern VOIDFUNCPTR __cmpsf2();

VOIDFUNCPTR usrExtraModules [] =
    {
    usrInit,			/* provided in case of null array */

#if	CPU_FAMILY==MIPS
#ifdef	INCLUDE_RPC
    (VOIDFUNCPTR) xdr_float,
#endif
#endif	/* CPU_FAMILY==MIPS */

#if CPU_FAMILY==MC680X0 && !(CPU==MC68040) && !(CPU==MC68060)
#ifdef  INCLUDE_GCC_FP
    (VOIDFUNCPTR) gccMathInit,
    (VOIDFUNCPTR) gccUssInit,
    (VOIDFUNCPTR) __fixunssfsi,
#endif
#endif  /* CPU_FAMILY==MC680X0 && !(CPU==MC68040) && !(CPU==MC68060  */

#if     (CPU==MC68040) || (CPU==MC68060)
#ifdef  INCLUDE_GCC_FP
    (VOIDFUNCPTR) gccUss040Init,
    (VOIDFUNCPTR) __fixunssfsi,
#endif
#endif  /* (CPU==MC68040) || (CPU==MC68060) */

#ifdef  INCLUDE_NETWORK
#ifdef  INCLUDE_NET_SHOW
    netShowInit,
#endif
#endif  /* INCLUDE_NETWORK */

#ifdef	INCLUDE_TASK_VARS
    (VOIDFUNCPTR) taskVarInit,
#endif

#ifdef  INCLUDE_LOADER
    (VOIDFUNCPTR) loadModule,
#endif

#ifdef  INCLUDE_UNLOADER
    (VOIDFUNCPTR) unld,
#endif

#ifdef INCLUDE_DOSFS              /* usrDosFsOld.c, dosFs1 API wrapper */
    (VOIDFUNCPTR) dosFsDevInit,
#endif /* INCLUDE_DOSFS  */

#ifdef INCLUDE_DOSFS_MAIN         /* dosFsLib (2) */
    (VOIDFUNCPTR) dosFsLibInit,
#endif /* INCLUDE_DOSFS_MAIN  */

#ifdef INCLUDE_DOSFS_FAT          /* dosFs FAT12/16/32 FAT table handler */
    (VOIDFUNCPTR) dosFsFatInit,
#endif /* INCLUDE_DOSFS_FAT */

#ifdef INCLUDE_DOSFS_DIR_VFAT     /* Microsoft VFAT dirent handler */
    (VOIDFUNCPTR) dosVDirLibInit,
#endif /* INCLUDE_DOSFS_DIR_VFAT */

#ifdef INCLUDE_DOSFS_DIR_FIXED    /* 8.3 & VxLongNames directory handler */
    (VOIDFUNCPTR) dosDirOldLibInit,
#endif /* INCLUDE_DOSFS_DIR_FIXED */

#ifdef INCLUDE_DOSFS_FMT          /* dosFs2 file system formatting module */
    (VOIDFUNCPTR) dosFsFmtLibInit,
#endif /* INCLUDE_DOSFS_FMT */

#ifdef INCLUDE_DOSFS_CHKDSK       /* file system integrity checking */
    (VOIDFUNCPTR) dosChkLibInit,
#endif /* INCLUDE_DOSFS_CHKDSK */

#ifdef INCLUDE_CBIO               /* CBIO API module */
    (VOIDFUNCPTR) cbioLibInit, 
#endif /* INCLUDE_CBIO */

#ifdef INCLUDE_DISK_CACHE         /* CBIO API disk caching layer */
    (VOIDFUNCPTR) dcacheDevCreate,
#endif /* INCLUDE_DISK_CACHE */

#ifdef INCLUDE_DISK_PART         /* disk partition handling code, fdisk... */
    (VOIDFUNCPTR) dpartDevCreate,
    (VOIDFUNCPTR) usrFdiskPartRead,
#endif /* INCLUDE_DISK_PART */

#ifdef INCLUDE_DISK_UTIL        /* ls, cd, mkdir, xcopy, etc */
    (VOIDFUNCPTR) ls,
#endif /* INCLUDE_DISK_UTIL */

#ifdef INCLUDE_TAR              /* tar utility */
    (VOIDFUNCPTR) tarExtract,
#endif /* INCLUDE_TAR */

#ifdef INCLUDE_RAM_DISK         /* CBIO API ram disk driver */
    (VOIDFUNCPTR) ramDiskDevCreate,
#endif /* INCLUDE_RAM_DISK */

#ifdef  INCLUDE_POSIX_TIMERS
    (VOIDFUNCPTR) nanosleep,
    (VOIDFUNCPTR) clock_gettime,
#endif

#ifdef  INCLUDE_POSIX_SCHED
    (VOIDFUNCPTR) sched_setparam,
#endif

#ifdef  INCLUDE_POSIX_MEM
    (VOIDFUNCPTR) mlockall,
#endif

#ifdef  INCLUDE_POSIX_FTRUNC
    (VOIDFUNCPTR) ftruncate,
#endif

#if defined(INCLUDE_POSIX_AIO) && defined(INCLUDE_SHOW_ROUTINES)
    (VOIDFUNCPTR) aioShow,
#endif /* defined(INCLUDE_POSIX_AIO) && defined(INCLUDE_SHOW_ROUTINES */

#ifdef  INCLUDE_NETWORK
#ifdef  INCLUDE_TFTP_CLIENT
    (VOIDFUNCPTR) tftpCopy,
#endif
#endif  /* INCLUDE_NETWORK */
    };

#ifdef  INCLUDE_ANSI_ASSERT
#include "../../src/config/assertInit.c"
#endif

#ifdef  INCLUDE_ANSI_CTYPE
#include "../../src/config/ctypeInit.c"
#endif

#ifdef  INCLUDE_ANSI_LOCALE
#include "../../src/config/localeInit.c"
#endif

#ifdef  INCLUDE_ANSI_MATH
#include "../../src/config/mathInit.c"
#endif

#ifdef  INCLUDE_ANSI_STDIO
#include "../../src/config/stdioInit.c"
#endif

#ifdef  INCLUDE_ANSI_STDLIB
#include "../../src/config/stdlibInit.c"
#endif

#ifdef  INCLUDE_ANSI_STRING
#include "../../src/config/stringInit.c"
#endif

#ifdef  INCLUDE_DSP
#include "../../src/config/usrDsp.c"
#endif

#ifdef  INCLUDE_ALTIVEC
/* AltiVec run-time support */
#include "../../src/config/SaveRestVR.c"
#include "../../src/config/usrAltivec.c"
#endif

#ifdef  INCLUDE_ANSI_TIME
#include "../../src/config/timeInit.c"
#endif

#ifdef  INCLUDE_ANSI_5_0
#include "../../src/config/ansi_5_0.c"
#endif

#ifdef	INCLUDE_FD
#include "../../src/config/usrFd.c"
#endif

#ifdef	INCLUDE_IDE
#include "../../src/config/usrIde.c"
#endif

#ifdef	INCLUDE_ATA
#include "../../src/config/usrAta.c"
#endif

#ifdef	INCLUDE_PCMCIA
#include "../../src/config/usrPcmcia.c"
#endif

#ifdef	INCLUDE_TFFS
#include "../../src/config/usrTffs.c"
#endif

#ifdef	INCLUDE_NET_SYM_TBL
#include "../../src/config/usrLoadSym.c"
#endif	/* INCLUDE_NET_SYM_TBL */

#if	defined(INCLUDE_MMU_BASIC) || defined(INCLUDE_MMU_FULL) || \
	defined(INCLUDE_MMU_MPU)
#include "../../src/config/usrMmuInit.c"
#endif	/* defined(INCLUDE_MMU_BASIC, INCLUDE_MMU_FULL, INCLUDE_MMU_MPU) */

#ifdef	INCLUDE_NETWORK
#include "../../src/config/usrNetwork.c"
#endif

#ifdef	INCLUDE_STARTUP_SCRIPT
#include "../../src/config/usrScript.c"
#endif	/* INCLUDE_STARTUP_SCRIPT */

#ifdef	INCLUDE_SCSI
#include "../../src/config/usrScsi.c"
#endif

#ifdef	INCLUDE_SM_OBJ
#include "../../config/comps/src/usrSmObj.c"
#endif

#ifdef	INCLUDE_VXFUSION
#include "../../src/config/usrVxFusion.c"
#endif

#ifdef	INCLUDE_WDB
#include "../../src/config/usrWdb.c"
#endif

#ifdef	INCLUDE_WINDML
#include "../../src/config/usrWindMl.c"
#endif

#ifdef  INCLUDE_JAVA
#include "../../src/config/usrJava.c"
#endif

#ifdef  INCLUDE_HTML
#include "../../src/config/usrHtml.c"
#endif

#ifdef	INCLUDE_ADA
BOOL 		sysAdaEnable = TRUE;	/* TRUE = enable Ada support options */
#else
BOOL 		sysAdaEnable = FALSE;
#endif

#ifdef  INCLUDE_CODETEST
#include "../../src/config/CodeTEST.c"
#endif

#include "../../src/config/intrinsics.c"

/* All this to generate a string we can #include */

#ifndef TOOL_FAMILY
#   define TOOL_FAMILY gnu
#endif

#define TOOL_CONFIG_HDR_STRINGIFY(x)  #x
#define TOOL_CONFIG_HDR2(tc, file) TOOL_CONFIG_HDR_STRINGIFY(cplus##tc##file.c)
#define TOOL_CONFIG_HDR1(tc, file) TOOL_CONFIG_HDR2(tc, file)
#define TOOL_CONFIG_HEADER(file) TOOL_CONFIG_HDR1(TOOL_FAMILY, file)

/* C++ configuration */
#if     defined (INCLUDE_CPLUS_STL)
#ifndef INCLUDE_CPLUS
#define INCLUDE_CPLUS
#endif /* INCLUDE_CPLUS */
#include TOOL_CONFIG_HEADER(Stl)
#endif

#if     defined (INCLUDE_CPLUS_COMPLEX_IO)
#ifndef INCLUDE_CPLUS
#define INCLUDE_CPLUS
#endif /* INCLUDE_CPLUS */
#ifndef INCLUDE_CPLUS_COMPLEX
#define INCLUDE_CPLUS_COMPLEX
#endif /* INCLUDE_CPLUS_COMPLEX */
#include TOOL_CONFIG_HEADER(ComplexIo)
#endif

#if     defined (INCLUDE_CPLUS_COMPLEX)
#ifndef INCLUDE_CPLUS
#define INCLUDE_CPLUS
#endif /* INCLUDE_CPLUS */
#include TOOL_CONFIG_HEADER(Complex)
#endif

#if     defined (INCLUDE_CPLUS_STRING_IO)
#ifndef INCLUDE_CPLUS
#define INCLUDE_CPLUS
#endif /* INCLUDE_CPLUS */
#ifndef INCLUDE_CPLUS_STRING
#define INCLUDE_CPLUS_STRING
#endif /* INCLUDE_CPLUS_STRING */
#include TOOL_CONFIG_HEADER(StringIo)
#endif

#if     defined (INCLUDE_CPLUS_STRING)
#ifndef INCLUDE_CPLUS
#define INCLUDE_CPLUS
#endif /* INCLUDE_CPLUS */
#include TOOL_CONFIG_HEADER(String)
#endif

#if	defined (INCLUDE_CPLUS_IOSTREAMS)||defined(INCLUDE_CPLUS_IOSTREAMS_FULL)
#ifndef INCLUDE_CPLUS
#define INCLUDE_CPLUS
#endif /* INCLUDE_CPLUS */
#include TOOL_CONFIG_HEADER(Ios)
#endif

#if defined(INCLUDE_CPLUS) && \
    (defined(INCLUDE_SHELL) || \
     defined(INCLUDE_SYM_TBL_SHOW) || \
     defined(INCLUDE_DEBUG) || \
     defined(INCLUDE_SPY) || \
     defined(INCLUDE_TIMEX))
#ifndef INCLUDE_CPLUS_DEMANGLER
#define INCLUDE_CPLUS_DEMANGLER
#endif
#endif

#ifdef INCLUDE_NO_CPLUS_DEMANGLER
#undef INCLUDE_CPLUS_DEMANGLER
#endif

#ifdef INCLUDE_CPLUS
#ifndef INCLUDE_CPLUS_LANG
#define INCLUDE_CPLUS_LANG
#endif
#endif

#ifdef INCLUDE_CPLUS_LANG
#ifndef INCLUDE_CPLUS
#define INCLUDE_CPLUS
#endif
#include TOOL_CONFIG_HEADER(Lang)
#endif

#if	defined (INCLUDE_CPLUS) || defined (INCLUDE_CPLUS_MIN)
BOOL 		sysCplusEnable = TRUE;	/* TRUE = enable C++ support */
#else
BOOL 		sysCplusEnable = FALSE;
#endif
/* END C++ configuration */

#if	(defined(INCLUDE_WDB_BP) && defined (INCLUDE_WDB)) || \
	defined (INCLUDE_DEBUG) || defined(INCLUDE_SHELL)
#include "../../src/config/usrBreakpoint.c"
#endif

#if	defined (INCLUDE_WINDVIEW)
#include "../../src/config/usrWindview.c"
#endif

#ifdef  INCLUDE_DELETE_5_0

/*******************************************************************************
*
* delete - delete a file (obsolete routine)
*
* This routine is provided for binary compatibility with VxWorks 5.0
* and earlier versions which define this function.  This interface
* has been removed from VxWorks because "delete" is a reserved word
* in C++.
*
* Source code which uses delete() may be recompiled using a special
* macro in ioLib.h which will translate delete() calls into remove()
* calls.  To enable the macro, define __DELETE_FUNC as TRUE in ioLib.h.
*
* If code is recompiled in this way, it is not necessary to include
* the delete() routine defined here.
*
* RETURNS: OK, or ERROR.
*
* SEE ALSO: remove(), unlink(), ioLib.h
*
* NOMANUAL
*/

STATUS delete 
    (
    const char *filename		/* filename to delete */
    )
    {
    return (remove (filename));
    }

#endif  /* INCLUDE_DELETE_5_0 */

#if	(defined(INCLUDE_FD) || defined(INCLUDE_IDE) || \
	 defined(INCLUDE_ATA) || defined(INCLUDE_PCMCIA) || \
	 defined(INCLUDE_TFFS))
/******************************************************************************
*
* devSplit - split the device name from a full path name
*
* This routine returns the device name from a valid UNIX-style path name
* by copying until two slashes ("/") are detected.  The device name is
* copied into <devName>.
*
* RETURNS: N/A
*
* NOMANUAL
*/

void devSplit
    (
    FAST char *fullFileName,    /* full file name being parsed */
    FAST char *devName          /* result device name */
    )
    {
    FAST int nChars = 0;

    if (fullFileName != NULL)
	{
        char *p0 = fullFileName;
        char *p1 = devName;

        while ((nChars < 2) && (*p0 != EOS))
	    {
            if (*p0 == '/')
                nChars++;

	    *p1++ = *p0++;
            }
        *p1 = EOS;
        }
    else
	{
        (void) strcpy (devName, "");
	}
    }

#endif	/* (defined (INCLUDE_FD) || (IDE) || (ATA) || (PCMCIA) || (TFFS)) */
#endif	/* __INCusrExtrac */
