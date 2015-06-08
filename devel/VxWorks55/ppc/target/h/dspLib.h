/* dspLib.h - dsp coprocessor support library header */

/* Copyright 1984-1998 Wind River Systems, Inc. */

/*
modification history
--------------------
01b,08aug98,kab  fixed default typedef of DSP_CONTEXT.  Cleanup after review.
01a,22jul98,mem  written.
*/

#ifndef __INCdspLibh
#define __INCdspLibh

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _ASMLANGUAGE
#include "taskLib.h"
#endif	/* _ASMLANGUAGE */

#if	(CPU_FAMILY==SH)
#include "arch/sh/dspShLib.h"
#else
#define DSPREG_SET	DSP_CONTEXT
typedef ULONG		DSP_CONTEXT;		/* in case of allocation */
#endif	/* CPU_FAMILY==SH */

#ifndef _ASMLANGUAGE

/* variable declarations */

IMPORT REG_INDEX dspRegName[];		/* dsp data register table */
IMPORT REG_INDEX dspCtlRegName[];	/* dsp control register table */
IMPORT WIND_TCB *pDspTaskIdPrevious;	/* task id for deferred exceptions */
IMPORT FUNCPTR	 dspCreateHookRtn;	/* arch dependent create hook routine */
IMPORT FUNCPTR	 dspDisplayHookRtn;	/* arch dependent display routine */

/* function declarations */

#if defined(__STDC__) || defined(__cplusplus)

/* 
 * The interface is set in os/dspLib.c through dspInit().  Some of
 * the functions are actually implemented in architecture dependent
 * files.
 */
IMPORT void 	dspInit (void);
IMPORT void	dspShowInit (void);
IMPORT void 	dspTaskRegsShow (int task);
IMPORT STATUS 	dspTaskRegsGet (int task, DSPREG_SET *pDspRegSet);
IMPORT STATUS 	dspTaskRegsSet (int task, DSPREG_SET *pDspRegSet);
IMPORT STATUS 	dspProbe (void);
IMPORT void 	dspRestore (DSP_CONTEXT *pDspContext);
IMPORT void 	dspSave (DSP_CONTEXT *pDspContext);
IMPORT void	dspRegsToCtx (DSPREG_SET *pDspRegSet, DSP_CONTEXT *pDspContext);
IMPORT void	dspCtxToRegs (DSP_CONTEXT *pDspContext, DSPREG_SET *pDspRegSet);

#else

IMPORT void 	dspInit ();
IMPORT void	dspShowInit ();
IMPORT void 	dspTaskRegsShow ();
IMPORT STATUS 	dspTaskRegsGet ();
IMPORT STATUS 	dspTaskRegsSet ();
IMPORT STATUS 	dspProbe ();
IMPORT void 	dspRestore ();
IMPORT void 	dspSave ();
IMPORT void	dspRegsToCtx ();
IMPORT void	dspCtxToRegs ();

#endif	/* __STDC__ */
#endif	/* _ASMLANGUAGE */

#ifdef __cplusplus
}
#endif

#endif /* __INCdspLibh */
