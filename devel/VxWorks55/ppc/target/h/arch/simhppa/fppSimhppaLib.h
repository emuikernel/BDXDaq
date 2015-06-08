/* fppSimhppaLib.h - simhppa floating-point header */

/* Copyright 1996 Wind River Systems, Inc. */
/*
modification history
--------------------
02b,25oct96,mem  fixed FP_DATA definition.
02a,14jan96,kab  created from fppSimsparcLib.h w/ hp mods.
01c,22sep95,ms   added dummyQueuePtr field to FP_CONTEXT (SPR #4796).
01b,30jul93,gae  fixed fpContext to have double regs.
01a,05jun93,rrr  derived.
*/

#ifndef __INCfppSimhppaLibh
#define __INCfppSimhppaLibh

#ifdef __cplusplus
extern "C" {
#endif

#include "vxWorks.h"

#define  FP_NUM_DREGS    28              /* Floating-Point Data Registers */

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
    union
	{
        unsigned int  fps[8];   /* status reg; exception regs; 
				   undefined if read/written as ints */
	double        fpd[4];   /* ditto; */
	} fpe;
    double fr[FP_NUM_DREGS];	/* Data Registers */
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

#else

extern void	fppArchInit ();
extern void	fppArchTaskCreateInit ();

#endif	/* __STDC__ */

#endif	/* _ASMLANGUAGE */

#define FSR_OFFSET(n)   (4*(n))           /* status & exception regs */
#define FP_DATA         FSR_OFFSET(8)     /* Data Registers */
#define FPD_OFFSET(n)   (FP_DATA + (8 * (n)))
#define FPREG_SET_SIZE  (32*8)            /* used in fppALib.s */

#define fpsr   fpe.fps[0]          /* status reg */
#define fpe0   fpe.fps[1]          /* exception reg */
#define fpe1   fpe.fps[2]          /* exception reg */
#define fpe2   fpe.fps[3]          /* exception reg */
#define fpe3   fpe.fps[4]          /* exception reg */
#define fpe4   fpe.fps[5]          /* exception reg */
#define fpe5   fpe.fps[6]          /* exception reg */
#define fpe6   fpe.fps[7]          /* exception reg */

#ifdef __cplusplus
}
#endif

#endif /* __INCfppSimsparcLibh */
