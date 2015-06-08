/* altivecLib.h - ALTIVEC coprocessor support library header */

/* Copyright 1984-1997 Wind River Systems, Inc. */

/*
modification history
--------------------

*/

#ifndef __INCaltivecLibh
#define __INCaltivecLibh

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _ASMLANGUAGE
#include "taskLib.h"
#endif	/* _ASMLANGUAGE */

#if     CPU_FAMILY==PPC
#include "arch/ppc/altivecPpcLib.h"
#endif  /* CPU_FAMILY==PPC */


/* function declarations */

#ifndef _ASMLANGUAGE

#if defined(__STDC__) || defined(__cplusplus)

extern void 	altivecInit (void);
extern void	altivecShowInit (void);
extern void 	altivecTaskRegsShow (int task);
extern STATUS 	altivecTaskRegsGet (int task, ALTIVECREG_SET *pAltivecRegSet);
extern STATUS 	altivecTaskRegsSet (int task, ALTIVECREG_SET *pAltivecRegSet);
extern STATUS 	altivecProbe (void);
extern void 	altivecRestore (ALTIVEC_CONTEXT *pAltivecContext);
extern void 	altivecSave (ALTIVEC_CONTEXT *pAltivecContext);
extern void	altivecRegsToCtx (ALTIVECREG_SET *pAltivecRegSet, ALTIVEC_CONTEXT *pAltivecContext);
extern void	altivecCtxToRegs (ALTIVEC_CONTEXT *pAltivecContext, ALTIVECREG_SET *pAltivecRegSet);

#else

extern void 	altivecInit ();
extern void	altivecShowInit ();
extern void 	altivecTaskRegsShow ();
extern STATUS 	altivecTaskRegsGet ();
extern STATUS 	altivecTaskRegsSet ();
extern STATUS 	altivecProbe ();
extern void 	altivecRestore ();
extern void 	altivecSave ();
extern void	altivecRegsToCtx ();
extern void	altivecCtxToRegs ();

#endif	/* __STDC__ */
#endif	/* _ASMLANGUAGE */

#ifdef __cplusplus
}
#endif

#endif /* __INCaltivecLibh */
