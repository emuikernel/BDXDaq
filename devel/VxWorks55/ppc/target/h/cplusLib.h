/* cplusLib.h - VxWorks C++ support */

/* Copyright 1992,1993 Wind River Systems, Inc. */

/*
modification history
--------------------
01c,07dec01,sn   added demangler declarations
01b,26apr93,srh  added cplus?torsLink declarations.
01a,01jan93,srh  written.
*/

#ifndef __INCcplusLibh
#define __INCcplusLibh

#include "private/cplusLibP.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum demangler_style
    {
    DMGL_STYLE_GNU,
    DMGL_STYLE_DIAB,
    DMGL_STYLE_ARM
    } DEMANGLER_STYLE;

/* function declarations */

#if defined(__STDC__) || defined(__cplusplus)

void cplusCtors (const char * moduleName);
void cplusDtors (const char * moduleName);
void cplusCtorsLink ();
void cplusDtorsLink ();
void cplusDemanglerSet (int mode);
void cplusDemanglerStyleSet (DEMANGLER_STYLE style);
void cplusXtorSet (int strategy);

#else   /* __STDC__ */

void cplusCtors ();
void cplusDtors ();
void cplusCtorsLink ();
void cplusDtorsLink ();
void cplusDemanglerSet ();
void cplusXtorSet ();

#endif  /* __STDC__ */

#ifdef __cplusplus
}
#endif

#endif /* __INCcplusLibh */
