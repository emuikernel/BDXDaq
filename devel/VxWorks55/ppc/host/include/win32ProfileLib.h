/* win32Profilelib.h - win32 wrapping library for UNIX (init file parser)*/

/* Copyright 1998 Wind River Systems, Inc. */

/*
modification history
--------------------
01a,22jan98,c_c  written.

*/

#ifndef __INCwin32Profilelibh
#define __INCwin32Profilelibh

#include "host.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Parameter file parsing functions */

extern unsigned int GetPrivateProfileString
    (
    char *	sectionName,	/* section name where to find the param */
    char *	parameterName,	/* param name */
    char *	defaultString,	/* default value if param not found */
    char *	destBuffer,	/* where to store the value */
    unsigned int bufSize,	/* size of this buffer */
    char *	fileName	/* paramter file name */
    );

extern unsigned int GetPrivateProfileInt
    (
    char *	sectionName,	/* section name where to find the param */
    char *	parameterName,	/* param name */
    int		defaultValue,	/* default value if param not found */
    char *	fileName	/* paramter file name */
    );

#ifdef __cplusplus
}
#endif

#endif /* __INCwin32Profilelibh */
