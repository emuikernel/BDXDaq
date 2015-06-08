/* fppSimsparcLib.h - simsparc floating-point header */

/* Copyright 1993-95 Wind River Systems, Inc. */
/*
modification history
--------------------
01a,07jun95,ism  derived from simsparc
*/

#ifndef __INCfppSimsparcLibh
#define __INCfppSimsparcLibh

#ifdef __cplusplus
extern "C" {
#endif

#include "vxWorks.h"

#define  FP_NUM_DREGS    16              /* Floating-Point Data Registers */

/* equates for fppState, fpstate */

#define UNKNOWN          0x00           /* default */
#define FPU_TESTED       0x01           /* set by fppProbe */
#define FPU_TESTED_TRUE  0x02           /* set by fppProbe */
#define FPU_TESTED_FALSE 0x00           /* set by fppProbe */
#define FPU_AVAILABLE    (FPU_TESTED + FPU_TESTED_TRUE)
#define NO_FPU_AVAILABLE (FPU_TESTED + FPU_TESTED_FALSE)

#ifndef _ASMLANGUAGE

typedef struct fpContext                /* Floating-Point Context */
    {
    double fpd[FP_NUM_DREGS];	/* Data Registers */
    unsigned int fsr;		/* Status Register */
    } FP_CONTEXT;

#define FPREG_SET	FP_CONTEXT

/* variable declarations */

extern REG_INDEX fpRegName[];		/* f-point data register table */
extern REG_INDEX fpCtlRegName[];	/* f-point control register table */
extern WIND_TCB *pFppTaskIdPrevious;	/* task id for deferred exceptions */
extern FUNCPTR	 fppCreateHookRtn;	/* arch dependent create hook routine */
extern FUNCPTR	 fppDisplayHookRtn;	/* arch dependent display routine */


/* function declarations */

#if defined(__STDC__) || defined(__cplusplus)

extern void	fppArchInit (void);
extern void	fppArchTaskCreateInit (FP_CONTEXT *pFpContext);
extern void	fsrShow (UINT fsrValue);
extern void     fppFlushInit (void);
extern UINT     fppQueueInit (void);
extern STATUS   fppProbeSup (void);
extern void     fppNan (FP_CONTEXT *pFpContext, ULONG fppFsr);

#else

extern void	fppArchInit ();
extern void	fppArchTaskCreateInit ();
extern void	fsrShow ();
extern void     fppFlushInit ();
extern UINT     fppQueueInit ();
extern STATUS   fppProbeSup ();
extern void     fppNan ();

#endif	/* __STDC__ */

#endif	/* _ASMLANGUAGE */

#define FP_DATA         0x0           		/* Data Registers */
#define FPD(n)          (FP_DATA + (4 * (n)))
#define FP_QUEUE        (FPD(FP_NUM_DREGS*2))	/* Q Registers (dword aligned)*/
#define FSR             (FP_QUEUE + 4)		/* Status Register */

#ifdef __cplusplus
}
#endif

#endif /* __INCfppSimsparcLibh */
