/* demangler.h - WR wrapper around GNU libiberty C++ demangler */

/* Copyright 2001 Wind River Systems, Inc. */

/*
modification history
--------------------
01b,10dec01,sn   move enum defs to demanglerTypes.h
01a,28nov01,sn   wrote
*/

#ifndef __INCdemanglerh
#define __INCdemanglerh

#ifdef __cplusplus
extern "C" {
#endif

/* includes */

/* defines */

/* typedefs */

#include "demanglerTypes.h"

char * demangle
    (
    const char * mangledSymbol,
    DEMANGLER_STYLE style,      /* the scheme used to mangle symbols */
    DEMANGLER_MODE mode         /* how hard to work */
    );

DEMANGLER_STYLE demanglerStyleFromToolFamily
    (
    const char * tool /* "gnu", "diab", ... */
    );
#ifdef __cplusplus
}
#endif

#endif /* __INCdemanglerh */

