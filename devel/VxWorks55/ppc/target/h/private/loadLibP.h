/* loadLibP.h - object module loader private library header */

/* Copyright 2002 Wind River Systems, Inc. */

/*
modification history
--------------------
01a,16may02,fmk  written
*/

#ifndef __INCloadLibPh
#define __INCloadLibPh

#ifdef __cplusplus
extern "C" {
#endif

#include "moduleLib.h"
#include "loadLib.h"

/* type definitions */

typedef void *  SYM_ADRS;               /* symbol's address */

typedef struct
    {
    char *	symName;                /* common symbol's name */
    BOOL	vxWorksSymMatched;	/* TRUE if core syms are included */
    void *	pSymAddrBss;		/* matching bss sym's addr, if any */
    SYM_TYPE	bssSymType;		/* exact type of matching bss symbol */
    void *	pSymAddrData;		/* matching data sym's addr, if any */
    SYM_TYPE	dataSymType;		/* exact type of matching data symbol */
    } COMMON_INFO;

/* function declarations */

extern MODULE_ID loadModuleAtSym (int fd, int symFlag, char **ppText,
				  char **ppData, char **ppBss,
				  SYMTAB_ID symTbl);
extern MODULE_ID loadModuleGet (char *fileName, int format, int *symFlag);
extern void addSegNames (int fd, char *pText, char *pData, char *pBss,
			 SYMTAB_ID symTbl, UINT16 group);
extern STATUS loadSegmentsAllocate (SEG_INFO *pSeg);
extern STATUS loadCommonManage (int comAreaSize, int comAlignment,   
                                char *symName, SYMTAB_ID symTbl,
                                SYM_ADRS *pSymAddr, SYM_TYPE *pSymType, 
                                int loadFlag, SEG_INFO *pSeg, 
                                int group);
extern STATUS loadCommonMatch (COMMON_INFO * pCommInfo, SYMTAB_ID symTblId);

#ifdef __cplusplus
}
#endif

#endif /* __INCloadLibPh */







