/* wtxregd.h - wtxregd include file */

/* Copyright 1984-1995 Wind River Systems, Inc. */

/*
modification history
--------------------
01n,12sep01,fle  added wtxregdgd structures duplication ond free routines for
                 Win32 hosts
01m,24feb98,fle  destroyed wtxregdRegistryPing() function declaration
01l,28jan98,fle  added wtxregdRegistryPing() function declaration
		 + got rid of old C style declarations
01k,09nov95,p_m  removed un-necessary version defines.
01c,02jun95,p_m  added WIND_REGISTRY_VERSION.
01b,18may95,jcf  changed name to wtxregd.h.
01a,17jan95,jcf  written.
*/

#ifndef __INCwtxregdh
#define __INCwtxregdh	1

#ifdef __cplusplus
extern "C" {
#endif

#include "host.h"
#include "wtxtypes.h"

/* defines */

/* function declarations */

extern WTX_DESC * wtxregdRegister	/* create a descriptor */
    (
    char *	name,			/* service name, NULL if TBD */
    char *	type,			/* service type, NULL if TBD */
    char *	key			/* service key */
    );

extern STATUS wtxregdUnregister		/* remove descriptor from table */
    (
    char *	wpwrName		/* item name to remove */
    );

extern WTX_DESC * wtxregdInfo		/* gives info on an item */
    (
    char *	wpwrName		/* item name to get info on */
    );

extern WTX_DESC_Q * wtxregdInfoQ	/* get info on matching strings */
    (
    char *	namePattern,		/* reg expr to match service name */
    char *	typePattern,		/* reg expr to match service type */
    char *	keyPattern		/* reg expr to match service key */
    );

extern STATUS wtxregdInfoQFree		/* frees an info queue */
    (
    WTX_DESC_Q *	pInfoQ		/* info Q to free */
    );

#ifdef WIN32

WTX_DESC * wtxregdWtxDescDup	/* duplicates a WTX_DESC and members  */
    (
    char *		name,		/* name to duplicate in WTX_DESC      */
    char *		type,		/* type to duplicate in WTX_DESC      */
    char *		key		/* key to duplicate in WTX_DESC       */
    );

STATUS wtxregdWtxDescFree
    (
    WTX_DESC *		pDesc	/* wtx item desc to free */
    );

WTX_DESC_Q * wtxregdInfoQDup		/* duplicates a registry info queue   */
    (
    WTX_DESC_Q *	pDescQIn	/* Info queue to duplicate            */
    );

STATUS wtxregdDupInfoQFree
    (
    WTX_DESC_Q *	pInfoQ			/* pointer on info Q to free */
    );

#endif /* WIN32 */

#ifdef __cplusplus
}
#endif

#endif /* __INCwtxregdh */
