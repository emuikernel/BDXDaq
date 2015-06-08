/* module.h - object module library header */

/* Copyright 1992-1996 Wind River Systems, Inc. */

/*
modification history
--------------------
01n,18mar98,jmb  merged Mark Mason patch from HPSIM, 04nov96: add
                 SEGMENT_UNWIND segment type.
01m,02mar98,pcn  WTX 2: added moduleFromFileNameCreate, moduleHostChecksumGet,
                 moduleTargetChecksumGet.
01l,11sep96,pad  added MODULE_GROUP_MAX definition (SPR #7133).
01k,11jun96,pad  changed moduleSegAdd interface and added PowerPC's SDA support.
01j,30jun95,pad  changed moduleIdFigure() prototype. Added moduleIdCheck().
01i,04may95,pad  added MODULE_NAME_MAX macro
01h,02may95,pad  format is now a string instead of an int.
01g,24feb95,pad  applied new error codes.
01f,21feb95,pad  removed OMF_SPEC sub-structure.
01e,08feb95,p_m  added loadFlag in MODULE_INFO.
01d,20jan95,jcf  made more portable.
01c,14dec94,p_m  added moduleUndefSymFirst(), moduleUndefSymNext() prototypes.
01b,01dec94,pad  modified MODULE type (undefined symbols management and
		 addition of OMF_SPEC). Changed moduleCreate() and moduleInit()
		 interface.
01a,03aug94,pad	 based on version 01e of VxWorks.
*/

#ifndef __INCmoduleLibh
#define __INCmoduleLibh

#ifdef __cplusplus
extern "C" {
#endif

#include <limits.h>

#include "dllLib.h"
#include "sllLib.h"
#include "objLib.h"
#include "tgtmem.h"

/* Types of segments */

/* Note - These are derived from a.out format*/

#define SEGMENT_TEXT	2	/* text segment */
#define SEGMENT_DATA	4	/* data segment */
#define SEGMENT_BSS	8	/* bss segment */
#define SEGMENT_SDATA	0x10	/* sdata segment (PowerPC) */
#define SEGMENT_SBSS	0x20	/* sbss segment (PowerPC) */
#define SEGMENT_SDATA2	0x40	/* sdata2 segment (PowerPC) */
#define SEGMENT_SBSS2	0x80	/* sbss2 segment (PowerPC) */
#define SEGMENT_UNWIND	0x100	/* unwind segment (PA-RISC) */

/* segment flag bits */

#define SEG_FREE_MEMORY		1 /* Free memory when deleting this segment */
#define SEG_WRITE_PROTECTION	2 /* Segment's memory is write-protected,
				   * need to call memory managment routines
				   * to unprotect when deleting */
#define SEG_REMOTE		4 /* remote segment */

/* display options */

#define MODDISPLAY_CODESIZE  	1 /* Display size of the code segments */
#define MODDISPLAY_IS_DLL_NODE 	2 /* Display routine is being called with
				   * DLL_NODE instead of MODULE_ID -
				   * used by moduleShow() */

#define MODDISPLAY_ALL  	(MODDISPLAY_CODESIZE) /* Display all possible
						       * information */

/* moduleCheck() options */

#define MODCHECK_NOPRINT	1 /* Don't print module names during check */

#define MODCHECK_TEXT		(SEGMENT_TEXT) /* check text segment */
#define MODCHECK_DATA		(SEGMENT_DATA) /* check data segment */
#define MODCHECK_BSS	       	(SEGMENT_BSS)  /* check bss segment */

/* module status information */

#define MODULE_REPLACED	0x00010000 /* set if the module is loaded twice */

/* module name maximum length */

#define MODULE_NAME_MAX		255

/* max number of module groups */

#define MODULE_GROUP_MAX		65535

typedef struct
    {
    DL_NODE	segmentNode;	/* double-linked list node information*/
    void *	address;	/* segment address */
    int		size;		/* segment size */
    int		type;		/* segment type*/
    int		flags;		/* segment flags */
    u_int	checksum;	/* segment checksum */
    PART_ID	memPartId;	/* ID of memory partition holding the segment */
    } SEGMENT;

typedef SEGMENT *SEGMENT_ID;

typedef struct
    {
    DL_NODE	undefSymNode;	/* double-linked list node information */
    char *	name;		/* name of the undefined symbol */
    } UNDEF_SYM;

typedef UNDEF_SYM * UNDEF_SYM_ID;

typedef struct
    {
    OBJ_CORE		objCore;	/* object management */
    DL_NODE		moduleNode;	/* double-linked list node info */
    char		name [MODULE_NAME_MAX]; /* module name */
    char		path [PATH_MAX]; /* module path */
    int			flags;		/* loadFlags as passed to the loader */
    DL_LIST		segmentList;	/* list of segments */
    char * 		format;		/* object module format */
    int			targetCpu;      /* target cpu identifier */
    UINT16		group;		/* group number */
    DL_LIST		dependencies;	/* list of modules that this depends */
					/* on. XXX PAD - not used for now */
    DL_LIST		referents;	/* list of modules that refer to this */
					/* XXX PAD - not used for now */
    VOIDFUNCPTR	*	 ctors;		/* list of static constructor */
					/* callsets. XXX PAD - not used */
    VOIDFUNCPTR *	dtors;		/* list of static destructor callsets */
					/* XXX PAD - not used for now */
    BOOL		noUndefSym;	/* TRUE if undef symbol list is empty */
    DL_LIST		undefSymList;	/* list of undefined symbol names */
    void *		user1;		/* reserved for use by end-user */
    void *		reserved1;	/* reserved for use by WRS */
    void *		reserved2;	/* reserved for use by WRS */
    void *		reserved3;	/* reserved for use by WRS */
    void *		reserved4;	/* reserved for use by WRS */
    } MODULE;

typedef MODULE *MODULE_ID;

typedef struct
    {
    void *	textAddr;	/* address of text segment */
    void *	dataAddr;	/* address of data segment */
    void *	bssAddr;	/* address of bss segment */
    int 	textSize;	/* size of text segment */
    int 	dataSize;	/* size of data segment */
    int 	bssSize;	/* size of bss segment */
    } MODULE_SEG_INFO;

typedef struct
    {
    char        	name [MODULE_NAME_MAX]; /* module name */
    char          	format[MODULE_NAME_MAX];/* object module format */
    int			group;		 /* group number */
    int			loadFlag;	 /* flags used to load module */
    MODULE_SEG_INFO	segInfo; 	 /* segment info */
    } MODULE_INFO;

/* function declarations */

extern STATUS moduleLibInit (void);

extern MODULE_ID moduleCreate 
    (
    int 	fd, 
    int 	flags
    );

extern MODULE_ID moduleFromFileNameCreate 
    (
    char * 	filename, 
    int 	flags
    );

extern STATUS moduleInit 
    (
    MODULE_ID 	moduleId, 
    char *	name, 
    int 	flags
    );

extern STATUS moduleTerminate 
    (
    MODULE_ID 	moduleId
    );

extern STATUS moduleDelete 
    (
    MODULE_ID 	moduleId
    );

extern MODULE_ID moduleIdFigure 
    (
    char * 	moduleName
    );

extern MODULE_ID moduleIdCheck 
    (
    MODULE_ID 	moduleId
    );

extern STATUS moduleShow 
    (
    char *	moduleNameOrId, 
    int 	options
    );

extern STATUS moduleSegAdd 
    (
    MODULE_ID 	moduleId, 
    int 	type, 
    void *	location,
    int 	length, 
    int 	flags, 
    PART_ID 	memPartId
    );

extern SEGMENT_ID moduleSegGet 
    (
    MODULE_ID 	moduleId
    );

extern SEGMENT_ID moduleSegFirst 
    (
    MODULE_ID 	moduleId
    );

extern SEGMENT_ID moduleSegNext 
    (
    SEGMENT_ID 	segmentId
    );

extern SEGMENT_ID moduleSegEach 
    (
    MODULE_ID 	moduleId, 
    FUNCPTR 	routine,
    int 	userArg
    );

extern STATUS moduleCreateHookAdd 
    (
    FUNCPTR 	moduleCreateHookRtn
    );

extern STATUS moduleCreateHookDelete 
    (
    FUNCPTR 	moduleCreateHookRtn
    );

extern MODULE_ID moduleFindByName 
    (
    char *	moduleName
    );

extern MODULE_ID moduleFindByNameAndPath 
    (
    char *	moduleName, 
    char *	pathName
    );

extern MODULE_ID moduleFindByGroup 
    (
    int 	groupNumber
    );

extern MODULE_ID moduleEach 
    (
    FUNCPTR 	routine, 
    int 	userArg
    );

extern int moduleIdListGet 
    (
    MODULE_ID *	idList, 
    int 	maxModules
    );

extern STATUS moduleInfoGet 
    (
    MODULE_ID 	moduleId, 
    MODULE_INFO *pModuleInfo
    );

extern char * moduleNameGet 
    (
    MODULE_ID 	moduleId
    );

extern int moduleFlagsGet 
    (
    MODULE_ID 	moduleId
    );

extern STATUS moduleUndefSymAdd 
    (
    MODULE_ID 	moduleId, 
    char * 	symName
    );

extern UNDEF_SYM_ID moduleUndefSymFirst 
    (
    MODULE_ID 	moduleId
    );

extern UNDEF_SYM_ID moduleUndefSymNext 
    (
    UNDEF_SYM_ID symbol
    );

extern STATUS moduleCheck 
    (
    int 	options
    );

extern INT32 moduleHostChecksumGet 
    (
    MODULE_ID 	moduleId
    );

extern INT32 moduleTargetChecksumGet 
    (
    MODULE_ID 	moduleId
    );
 
#ifdef __cplusplus
}
#endif

#endif /* __INCmoduleLibh */
