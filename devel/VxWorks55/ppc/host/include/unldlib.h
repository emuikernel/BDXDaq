/* unldlib.h - header for unload library */

/* Copyright 1992-1996 Wind River Systems, Inc. */

/*
modification history
--------------------
01c,07jun96,pad  added MOD_INFO structure type for PowerPC's SDA support.
01b,01mar95,pad  removed old style error codes.
01a,01oct94,jcf  based on version 01f of VxWorks.
*/

#ifndef __INCunldLibh
#define __INCunldLibh

#ifdef __cplusplus
extern "C" {
#endif

/* includes */

#include "module.h"

/* options for unld */

#define UNLD_KEEP_BREAKPOINTS  1 /* don't delete breakpoints from unld() */

/* typedefs */

typedef struct
    {
    UINT16	group;		/* module's group */
    PART_ID	sdaMemPartId;	/* module's SDA mem part id */
    } MOD_INFO;

/* function declarations */

extern STATUS unld (void *name, int options);
extern STATUS unldByNameAndPath (char *name, char *path, int options);
extern STATUS unldByGroup (UINT16 group, int options);
extern STATUS unldByModuleId (MODULE_ID moduleId, int options);
MODULE_ID reld (void * nameOrId, int options);

#ifdef __cplusplus
}
#endif

#endif /* __INCunldLibh */
