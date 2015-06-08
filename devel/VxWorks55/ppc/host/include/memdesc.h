/* memdesc.h - red-black memDesc library header */

/* Copyright 1984-1995 Wind River Systems, Inc. */

/*
modification history
--------------------
01d,20apr95,p_m  integrated more WIN32 changes.
01c,15mar95,p_m  changed #include "vxWorks.h" to #include "host.h".
01b,01mar95,pad  removed include file vwModNum.h
01a,19jan94,jcf  created.
*/

#ifndef __INCmemDescLibh
#define __INCmemDescLibh

#ifdef __cplusplus
extern "C" {
#endif

#include "host.h"

/* type definitions */

/* HIDDEN */

typedef struct mem_desc
    {
    struct mem_desc *	leftChild;
    struct mem_desc *	rightChild;
    BOOL		isRed;
    void *		blockBase;
    void *		blockEnd;
    void *		blockCousin;
    } MEM_DESC;

typedef struct
    {
    MEM_DESC root;
    } MEM_DESC_HEAD;

/* END_HIDDEN */

/* function declarations */

#if defined(__STDC__) || defined(__cplusplus) || defined(WIN32_COMPILER)

extern MEM_DESC_HEAD *	memDescHeadCreate (void);
extern STATUS 		memDescHeadDelete (MEM_DESC_HEAD *pMemDescHead);
extern STATUS 		memDescHeadInit (MEM_DESC_HEAD *pMemDescHead);
extern STATUS 		memDescHeadTerminate (MEM_DESC_HEAD *pMemDescHead);
extern MEM_DESC *	memDescCreate (MEM_DESC_HEAD *pMemDescHead,
				       void *blockBase, void *blockEnd,
				       void *blockCousin);
extern STATUS 		memDescInit (MEM_DESC_HEAD *pMemDescHead, 
				     MEM_DESC *pMemDesc, void *blockBase, 
				     void *blockEnd, void *blockCousin);
extern STATUS 		memDescPut (MEM_DESC_HEAD *pMemDescHead,
				    MEM_DESC *pMemDesc);
extern STATUS 		memDescDelete (MEM_DESC_HEAD *pMemDescHead,
				       MEM_DESC *pMemDesc);
extern STATUS 		memDescTerminate (MEM_DESC_HEAD *pMemDescHead,
				          MEM_DESC *pMemDesc);
extern STATUS 		memDescRemove (MEM_DESC_HEAD *pMemDescHead,void *pAddr);
extern MEM_DESC	*	memDescFind (MEM_DESC_HEAD *pMemDescHead, void *ptr,
				     MEM_DESC **ppMemDesc);
extern STATUS 		memDescMerge (MEM_DESC_HEAD *pMemDescHead,
				      MEM_DESC *mergeRoot);
extern MEM_DESC *	memDescEach (MEM_DESC_HEAD *pMemDescHead, 
			             FUNCPTR routine, int routineArg);
extern void 		memDescShow (MEM_DESC_HEAD *pMemDesc, int format);

#else	/* __STDC__ */

extern MEM_DESC_HEAD *	memDescHeadCreate ();
extern STATUS 		memDescHeadDelete ();
extern STATUS 		memDescHeadInit ();
extern STATUS 		memDescHeadTerminate ();
extern STATUS 		memDescCreate ();
extern STATUS 		memDescInit ();
extern STATUS 		memDescDelete ();
extern STATUS 		memDescTerminate ();
extern MEM_DESC	*	memDescFind ();
extern MEM_DESC *	memDescEach ();
extern void 		memDescShow ();

#endif	/* __STDC__ */

#ifdef __cplusplus
}
#endif

#endif /* __INCmemDescLibh */
