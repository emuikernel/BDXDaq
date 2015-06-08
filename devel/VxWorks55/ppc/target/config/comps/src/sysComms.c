/* sysComms.c - common block symbols needed by many VxWorks librarys */

/* Copyright 1984-1998 Wind River Systems, Inc. */

/*
modification history
--------------------
01a,03jun98,ms   updated
*/

/*
DESCRIPTION
System common block symbol definitions.
*/

#include "symLib.h"
#include "bootLib.h"

/* globals */

int             consoleFd = NONE;	/* fd of initial console device      */
SYMTAB_ID       statSymTbl;		/* error status symbol table id      */
SYMTAB_ID       sysSymTbl;		/* symbol table id                   */
BOOT_PARAMS     sysBootParams;		/* parameters from boot line         */
int             sysStartType;		/* type of boot (WARM, COLD, etc)    */
BOOL		sysCplusEnable;		/* no cplus support by default       */
BOOL		sysAdaEnable;		/* no ada support by default         */
int		sysFlags;               /* boot flags                        */
char		sysBootHost [BOOT_FIELD_LEN]; /* name of boot host           */
char		sysBootFile [BOOT_FIELD_LEN]; /* name of boot file           */

