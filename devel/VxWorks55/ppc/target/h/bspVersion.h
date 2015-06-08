/* bspVersion.h - VxWorks board support package version information */

/* Copyright 1984-1996 Wind River Systems, Inc. */

/*
This header file is now historical.  New 1.1 BSPs do not refer to this
file, only older 1.0 BSPs.  This file is carried forward only to maintain
backward compatibility for old 1.0 BSPs.

MAKE NO FURTHER EDITS TO THIS FILE. 23sep96,dat.
*/

/*
modification history
--------------------
01i,23sep96,dat	 changed default BSP version back to 1.0. (1.1 BSPs don't
		 use this header file).
01h,26sep95,dat	 added decl of sysBspRev 
01g,21jun95,ms	 changed to version "1.1"
01f,12apr94,caf  changed to version "1.0".
01e,20nov93,caf  changed to version "1.0-beta".
01e,02dec93,pme  pushed to Beta for Am29K.
01d,21jan93,caf  changed to version "1.0".
01c,16oct92,caf  changed from alpha to beta.
01b,22sep92,rrr  added support for c++
01a,08sep92,caf  created.
*/

#ifndef __INCbspVersionh
#define __INCbspVersionh

#ifdef __cplusplus
extern "C" {
#endif

/* defines */

#ifndef BSP_VERSION
#   define	BSP_VERSION	"1.0"	/* for printing */
#   define	BSP_VER_1_0	1	/* for #if or #ifdef testing */
#endif /*BSP_VERSION*/

/* function declarations */

#ifndef _ASMLANGUAGE

#if defined(__STDC__) || defined(__cplusplus)

extern char * bspVersion (void);	/* old 1.0 usage */
extern char * sysBspRev (void);		/* new 1.1 usage */

#else	/* __STDC__ */

extern char * bspVersion ();
extern char * sysBspRev ();

#endif	/* __STDC__ */

#endif /* _ASMLANGUAGE */

#ifdef __cplusplus
}
#endif

#endif /* __INCbspVersionh */
