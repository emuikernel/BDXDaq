/* fppSimntLib.h - simnt floating-point header */

/* Copyright 1993-2002 Wind River Systems, Inc. */

/*
modification history
--------------------
01b,03may02,jmp  added fppDtoDx() & fppDxtoD().
01a,29apr98,cym  derived from i86
*/

#ifndef __INCfppSimntLibh
#define __INCfppSimntLibh

#ifdef __cplusplus
extern "C" {
#endif

#define FPCR		0x00    /* OFFSET(FP_CONTEXT, fpcr)             */
#define FPSR            0x04    /* OFFSET(FP_CONTEXT, fpsr)             */
#define FPTAG           0x08    /* OFFSET(FP_CONTEXT, fptag)            */
#define IP_OFFSET       0x0c    /* OFFSET(FP_CONTEXT, ipOffset)         */
#define CS_SELECTOR     0x10    /* OFFSET(FP_CONTEXT, csSelector)       */
#define OP_OFFSET       0x14    /* OFFSET(FP_CONTEXT, opOffset)         */
#define OP_SELECTOR     0x18    /* OFFSET(FP_CONTEXT, opSelector)       */


#ifndef _ASMLANGUAGE

/* number of fp registers on coprocessor */
#define FP_NUM_REGS     8

/* maximum size of floating-point coprocessor state frame */
#define FP_STATE_FRAME_SIZE     108

typedef struct  /* DOUBLEX - double extended precision */
    {
    UCHAR f[10];
    } DOUBLEX;

typedef struct fpContext        /* FP_CONTEXT */
    {
    int         fpcr;                           /* control word         :   4 */
    int         fpsr;                           /* status word          :   4 */
    int         fptag;                          /* tag word             :   4 */
    int         ipOffset;                       /* ip offset            :   4 */
    int         csSelector;                     /* cs selector          :   4 */
    int         opOffset;                       /* operand offset       :   4 */
    int         opSelector;                     /* operand selector     :   4 */
    DOUBLEX     fpx[FP_NUM_REGS];               /* 8 extended doubles   :  80 */

    } FP_CONTEXT;                               /*                TOTAL : 108 */

typedef FP_CONTEXT FPREG_SET;

#define FPX_OFFSET(n)   (0x1c + (n)*sizeof(DOUBLEX))

/* variable declarations */

extern REG_INDEX fpRegName[];           /* f-point data register table */
extern REG_INDEX fpCtlRegName[];        /* f-point control register table */
extern FUNCPTR   fppCreateHookRtn;      /* arch dependent create hook routine */
extern FUNCPTR   fppDisplayHookRtn;     /* arch dependent display routine */

/* function declarations */

#if defined(__STDC__) || defined(__cplusplus)

extern	void	fppArchInit (void);
extern	void	fppArchTaskCreateInit (FP_CONTEXT *pFpContext);
extern	STATUS	fppProbe (void);
extern	void	fppDtoDx (DOUBLEX * pDx, double * pDouble);
extern	void	fppDxtoD (double * pDouble, DOUBLEX * pDx);

#else

extern	void	fppArchInit ();
extern	void	fppArchTaskCreateInit ();
extern	STATUS	fppProbe ();
extern	void	fppDtoDx ();
extern	void	fppDxtoD ();

#endif  /* __STDC__ */

#endif  /* _ASMLANGUAGE */

#ifdef __cplusplus
}
#endif

#endif /* __INCfppSimntLibh */
