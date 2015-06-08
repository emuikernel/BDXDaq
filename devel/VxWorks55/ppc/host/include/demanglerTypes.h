/* demanglerTypes.h - enums needed for WR demangler functions (can safely be included by GDB) */

/* Copyright 2001 Wind River Systems, Inc. */

/*
modification history
--------------------
01a,10dec01,sn   wrote
*/

#ifndef __INCdemanglerTypesh
#define __INCdemanglerTypesh

#ifdef __cplusplus
extern "C" {
#endif

/* includes */

/* defines */

/* typedefs */

typedef enum demangler_style
    {
    DMGL_STYLE_GNU,
    DMGL_STYLE_DIAB,
    DMGL_STYLE_ARM
    } DEMANGLER_STYLE;

typedef enum demangler_mode
    {
    DMGL_MODE_OFF = 0,            /* function name is not demangled */
    DMGL_MODE_TERSE = 1,          /* just function name, no type information */
    DMGL_MODE_COMPLETE = 2,       /* full demangling */
    } DEMANGLER_MODE;

#ifdef __cplusplus
}
#endif

#endif /* __INCdemanglerTypesh */

