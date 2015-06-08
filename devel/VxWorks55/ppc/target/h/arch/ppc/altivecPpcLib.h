/* altivecPpcLib.h - PowerPC Altivec coprocessor support library header */

/* Copyright 1984-2001 Wind River Systems, Inc. */
/*
modification history
--------------------
01b,04oct01,yvp  Fix SPR69306: Changed Altivec context pointer from spare4 to
                 pCoprocCtx.
01a,29mar01,pcs  Implement code review suggestions.
*/

#ifndef __INCaltivecPpcLibh
#define __INCaltivecPpcLibh

#ifdef __cplusplus
extern "C" {
#endif

#include "vxWorks.h"

#define VRSAVE_REG                      256  /* VRSAVE Register */

/* Altivec exceptions */    

#define _EXC_ALTIVEC_UNAVAILABLE        0xF20  /* altivec unavailable */
#define _EXC_ALTIVEC_ASSIST             0x1600 /* altivec assist */


/* number of floating-point data registers */

#define	ALTIVEC_NUM_REGS	32

#ifndef	_ASMLANGUAGE

typedef UINT32 VEC_REG[4];              /* 128 bit vector register */

typedef struct altivecContext
    {
    VEC_REG  vrfile[ALTIVEC_NUM_REGS];  /* VRFILE: 32 Vector Registers */
    UINT32   vscr[4];                   /* VSCR: vec status and control reg */
    UINT32   vrsave;                    /* VRSAVE: SPR 256 */
    UINT32   pad1;                      /* pad to 16 byte boundary.    */ 
    UINT32   pad2;  
    UINT32   pad3;  
    } ALTIVEC_CONTEXT;


#define ALTIVECREG_SET ALTIVEC_CONTEXT


#define ALTIVEC_CONTEXT_GET(pTcb)       (ALTIVEC_CONTEXT *)(pTcb->pCoprocCtx)
#define ALTIVEC_CONTEXT_SET(pTcb,X)     (pTcb->pCoprocCtx=(int)(X))

/* variable declarations */
#if 0
extern REG_INDEX altivecRegName[];		/* altivec data register table */
extern REG_INDEX altivecCtlRegName[];	/* altivec control register table */
#endif

extern WIND_TCB *pAltivecTaskIdPrevious;	/* task id for deferred exceptions */
extern FUNCPTR	 altivecCreateHookRtn;	/* arch dependent create hook routine */
extern FUNCPTR	 altivecDisplayHookRtn;	/* arch dependent display routine */

/* function declarations */

#if defined(__STDC__) || defined(__cplusplus)

IMPORT void	altivecArchInit (void);
IMPORT void	altivecArchTaskCreateInit (ALTIVEC_CONTEXT *pFpContext);

#else	/* __STDC__ */

IMPORT void	altivecArchInit ();
IMPORT void	altivecArchTaskCreateInit ();

#endif	/* __STDC__ */


#endif	/* _ASMLANGUAGE */

/* define offsets in ALTIVEC_CONTEXT structure */

#define	AVX		0x0
#define AVX_OFFSET(n)	(AVX + (n)*16)
#define VSCR_OFFSET     (AVX + (ALTIVEC_NUM_REGS)*16)   /* offset == 0x200 */
#define VRSAVE_OFFSET   (VSCR_OFFSET + 16)              /* offset == 0x210 */
#define AVCONTEXT_SIZE	16*(ALTIVEC_NUM_REGS + 2)

#ifdef __cplusplus
}
#endif

#endif /* __INCaltivecPpcLibh */
