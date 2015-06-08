/* fppPpcLib.h - PowerPC floating-point coprocessor support library header */

/* Copyright 1984-1995 Wind River Systems, Inc. */
/*
modification history
--------------------
01c,20apr95,yao  added padding word to fpContext.  added copyright note.
01b,12jan95,caf  fixed to reflect 32 registers of 64 bits each.
01a,XXXXXXX,yao  written.
*/

#ifndef __INCfppPpcLibh
#define __INCfppPpcLibh

#ifdef __cplusplus
extern "C" {
#endif

/* number of floating-point data registers */

#define	FP_NUM_DREGS	32

#ifndef	_ASMLANGUAGE

typedef struct fpContext	/* FP_CONTEXT */
    {
    double fpr[FP_NUM_DREGS];	/* 32 doubles	  : 256 bytes */
    int fpcsr;			/* control/status :   4 bytes */
    int pad;			/* pad to 8 byte boundary     */
    } FP_CONTEXT;		/* TOTAL	  : 260 bytes */

#define FPREG_SET FP_CONTEXT

/* variable declarations */

extern REG_INDEX fpRegName[];		/* f-point data register table */
extern REG_INDEX fpCtlRegName[];	/* f-point control register table */
extern WIND_TCB *pFppTaskIdPrevious;	/* task id for deferred exceptions */
extern FUNCPTR	 fppCreateHookRtn;	/* arch dependent create hook routine */
extern FUNCPTR	 fppDisplayHookRtn;	/* arch dependent display routine */

/* function declarations */

#if defined(__STDC__) || defined(__cplusplus)

IMPORT void	fppArchInit (void);
IMPORT void	fppArchTaskCreateInit (FP_CONTEXT *pFpContext);

#else	/* __STDC__ */

IMPORT void	fppArchInit ();
IMPORT void	fppArchTaskCreateInit ();

#endif	/* __STDC__ */


#endif	/* _ASMLANGUAGE */

/* define offsets in FP_CONTEXT structure */

#define	FPX		0x0
#define FPX_OFFSET(n)	(FPX + (n)*8)
#define	FPCSR_OFFSET	(FPX + (FP_NUM_DREGS)*8)	/* offset == 0x100 */
#define FPCONTEXT_SIZE	8*(FPNUM_DREGS + 1)

#ifdef __cplusplus
}
#endif

#endif /* __INCfppPpcLibh */
