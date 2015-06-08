/* cachePpcLib.h - PowerPC cache library header file */

/* Copyright 1984-1994 Wind River Systems, Inc. */

/*
modification history
--------------------
01a,06nov96,tpr  added CACHE_CMD_XXX macros for PPC860.
*/

#ifndef __INCcachePpcLibh
#define __INCcachePpcLibh

#ifdef __cplusplus
extern "C" {
#endif

#include "vxWorks.h"

#define CACHE_CMD_ENABLE	0x02000000	/* Enable the cache */
#define CACHE_CMD_DISABLE	0x04000000	/* Disable the cache */
#define CACHE_CMD_LOAD_LOCK	0x06000000	/* Load and Lock */
#define CACHE_CMD_UNLOCK_LINE	0x08000000	/* Unlock a cache line */
#define CACHE_CMD_UNLOCK_ALL	0x0a000000	/* Unlock all the cache */
#define CACHE_CMD_INVALIDATE	0x0c000000	/* Invalidate all the cache */

#ifdef __cplusplus
}
#endif

#endif /* __INCcachePpcLibh */
