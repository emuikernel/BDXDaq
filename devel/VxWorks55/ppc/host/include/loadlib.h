/* loadlib.h - object module loader library header */

/* Copyright 1994-1998 Wind River Systems, Inc. */

/*
modification history
--------------------
01v,28mar02,jn   fix alignment handling for COMMONS (SPR # 74567)
01u,27mar98,pcn  Added a LOAD_NO_DOWNLOAD flag. Redefined LOAD_CORE_FILE flag.
01t,16mar98,dbt  removed loadCpuFamilyNameGet() and loadCpuFamilyGet()
		 definitions. Repaired broken history.
01s,02mar98,pcn  WTX 2: added loadModuleAtFromFileName.
01r,22jan98,c_c  DLLized Target Server Implementation.
		 Removed DYNLK_LIB defines.
01q,30oct96,elp  Added LOAD_LOADED_BY_TGTSVR flag.
01p,01oct96,elp	 Added LOAD_MODULE_INFO_ONLY flag, globals objModuleFormat,
		 targetCpu (SPR# 6775).
01o,06may96,pad  Added prototypes for loadBufferFree(). Changed
		 loadCommonManage() prototype.
01n,15jan96,pad  Added LOAD_OMF_INIT definition and loadFlagsCheck() prototype.
01m,28aug95,pad  Added loadCpuFamilyGet() and loadCpuFamilyNameGet().
		 Removed non-ANSI function declarations.
01l,18kul95,pad  added LOAD_CPLUS_XTOR_AUTO and LOAD_CPLUS_XTOR_MANUAL loader
		 flags.
01k,02may95,pad  modified to match configurable loader scheme.
01j,22mar95,jcf  changed loadCoreFileCheck().
01i,07mar95,pad  added loadLibInit() prototype.
01h,28feb95,pad  applied new error codes. Added loadCoreBuilderGet(),
		 loadCoreBuilderSet() and loadOutputToFile prototypes. Added
		 LOAD_FILE_OUTPUT flag.
01g,17feb95,pad  added prototypes for loadAlignGet() and loadCoreFileCheck().
01f,30jan95,pad  added prototype for loadCommonManage(). Removed prototype of
		 loadCommonMatch(). Changed loadRoutineGet() prototype.
01e,20jan95,jcf  made more portable.
01d,13dec94,p_m  Removed #include "wtxtypes.h".	
01c,01dec94,pad  A bunch of changes after the code review...
01b,28nov94,pad  Cleaned up.
01a,27sep94,pad  based on version 01s of VxWorks.
*/

#ifndef __INCloadLibh
#define __INCloadLibh

#ifdef __cplusplus
extern "C" {
#endif

#include "symLib.h"
#include "module.h"
#include "dynlklib.h"

/* Generic macros handling OMF reader's interface routines */

/* We currently expect to find at least two (2) routines in each relocator
 * library whose name are :
 *
 *  - "<omf><Cpu>SegReloc" for the relocator
 *  - "<omf><Cpu>SegVerify" for the module verifier
 *
 * Another function "<omf><Cpu>SegInit" can be specified. If exported, this 
 * routine will be run after the dynamic link. Its goal is to provide a way
 * to initialize the reloc library.
 */
#define NB_MANDATORY_RELOC_RTNS 2


#define LOAD_OMF_CHECK \
    DYNLK_FUNC_PROTO (STATUS (*) (int, BOOL *), loadDllFv, 0)
#define LOAD_MODULE \
    DYNLK_FUNC_PROTO (STATUS (*) (char *, int, void **, void **, void **, \
                      SYMTAB_ID, MODULE_ID, SEG_INFO *), loadDllFv, 1)
#define LOAD_OMF_INIT \
    DYNLK_FUNC_PROTO (STATUS (*) (), loadDllFv, 2)

/* Special value to let the loader allocate memory for the segments */

#define LOAD_NO_ADDRESS   	((char *) NONE)

/* new load flags */

#define LOAD_NO_SYMBOLS	        0x1	/* No symbols added to the target  */
					/* symbol table                    */
#define LOAD_LOCAL_SYMBOLS	0x2	/* Only internal symbols added to  */
					/* the target symbol table         */
#define LOAD_GLOBAL_SYMBOLS	0x4	/* Only external symbols added to  */
					/* the target symbol table         */
#define LOAD_ALL_SYMBOLS	(LOAD_LOCAL_SYMBOLS | LOAD_GLOBAL_SYMBOLS)
					/* All symbols added to the target */
					/* symbol table                    */
#define LOAD_HIDDEN_MODULE      0x10	/* Don't display module info with  */
					/* moduleShow()                    */
#define LOAD_FULLY_LINKED       0x20	/* No relocation required          */
#define LOAD_NO_DOWNLOAD        0x40    /* No relocation required, nothing */
                                        /* downloaded to the target memory */
#define LOAD_CORE_FILE		(LOAD_FULLY_LINKED | LOAD_NO_DOWNLOAD)
#define LOAD_COMMON_MATCH_NONE	0x100	/* Only alloc, no search (default) */
#define LOAD_COMMON_MATCH_USER	0x200	/* Core file excluded from search  */
#define LOAD_COMMON_MATCH_ALL	0x400	/* Core file included in search    */
#define LOAD_BAL_OPTIM		0x1000	/* set i960 branch-and-link optim. */
#define LOAD_FILE_OUTPUT	0x10000	/* write segments in a file (testing )*/
#define LOAD_CPLUS_XTOR_AUTO	0x100000/* Automatic C++ ctor/dtor strategy */
#define LOAD_CPLUS_XTOR_MANUAL	0x200000/* Manual off C++ ctor/dtor strategy */

/* following flags must be the same than in target/h/loadLib.h */

#define LOAD_MODULE_INFO_ONLY	0x1000000	/* module synchronization */
#define LOAD_LOADED_BY_TGTSVR	0x2000000	/* module synchronization */

/* Command flags for architecture specific functions */

#define TAG_FIELD	1	/* tagentries field in main header (i960) */
#define	ALIGN_FIELD	2	/* s_align field in section headers (i960) */
#define FLAG_FIELD	3	/* n_flags field in symbol entries (i960) */
#define TYPE_FIELD	4	/* n_type field in symbol entries (i960) */
#define PAD_FIELD	5	/* pad2 field in symbol entries (i960) */

/* Some convenient types */

typedef void *      SYM_ADRS;		/* symbol's address */
typedef void **     SYM_ADRS_TBL;	/* table of symbol's addr */
typedef void *      SCN_ADRS;		/* section's address */
typedef void **     SCN_ADRS_TBL;	/* table of section's addr */

/* data structures */

typedef struct
    {
    void *	pAddrText;		/* text segment address on target */
    void *	pAddrData;		/* data segment address on target */
    void *	pAddrBss;		/* bss segment address on target */
    UINT	sizeText;		/* text segment size */
    UINT	sizeProtectedText;	/* protected text segment size */
    UINT	sizeData;		/* data segment size */
    UINT	sizeBss;		/* bss segment size */
    int     	flagsText;		/* text flags for module */
    int		flagsData;		/* data flags for module */
    int		flagsBss;		/* bss flags for module */
    void *	pAdnlInfo;		/* points to additional information */
    } SEG_INFO;

typedef struct
    {
    char *	symName;			/* common symbol's name */
    BOOL	coreSymMatched;		/* TRUE if core syms are included */
    void *	pSymAddrBss;		/* matching bss sym's addr, if any */
    SYM_TYPE	bssSymType;		/* exact type of matching bss symbol */
    void *	pSymAddrData;		/* matching data sym's addr, if any */
    SYM_TYPE	dataSymType;		/* exact type of matching data symbol */
    void *	pSymAddrText;		/* matching text sym's addr, if any */
    SYM_TYPE	textSymType;		/* exact type of matching text symbol */
    } COMMON_INFO;

/* variable declarations */

extern char *   objModuleFormat;
extern int  targetCpu;

/* function declarations */

extern STATUS loadLibInit 
    (
    char *	coreFile, 
    char *	omfName
    );

extern STATUS loadRelocLink
    (
    DYNLK_FUNC	relocDllFv[],		/* array of function descriptions */
    int		routineNb		/* # of routines to link  */
    );

extern MODULE_ID loadModule 
    (
    int		fd,
    int		symFlag
    );

extern MODULE_ID loadModuleAt 
    (
    int		fd, 
    int		symFlag, 
    void **	ppText, 
    void **	ppData,
    void **	ppBss
    );

extern MODULE_ID loadModuleAtFromFileName
    (
    char *      filename,
    char *      fileBufferBase,
    int         loadFlag,
    void **     ppText,
    void **     ppData,
    void **     ppBss
    );

extern STATUS loadSegmentsAllocate 
    (
    SEG_INFO *	pSeg
    );

extern STATUS loadUndefSymAdd 
    (
    MODULE_ID	moduleId, 
    char *	symName
    );

extern STATUS loadCommonManage 
    (
    int		comAreaSize, 
    int         comAlignment,   
    char *	symName,
    SYMTAB_ID	symTbl, 
    SYM_ADRS *	pSymAddr,
    SYM_TYPE *	pSymType, 
    int		loadFlag,
    SEG_INFO *	pSeg, 
    int		group
    );

extern UINT32 loadAlignGet 
    (
    UINT32	alignment, 
    void *	pAddr
    );

extern STATUS loadCoreFileCheck 
    (
    void *	pRemote, 
    void *	pLocal, 
    UINT32	nBytes
    );

extern void loadCoreBuilderSet 
    (
    char *	builder
    );

extern char * loadCoreBuilderGet (void);

extern STATUS loadOutputToFile 
    (
    char *	fileName, 
    SEG_INFO *	pSeg
    );

extern STATUS loadFlagsCheck 
    (
    int		loadFlags, 
    void **	ppText, 
    void **	ppData,
    void **	ppBss
    );

extern void loadBufferFree 
    (
    void **	ppBuf
    );


#ifdef __cplusplus
    }
#endif

#endif /* __INCloadLibh */
