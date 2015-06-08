/* excLibP.h - private exception handling subsystem header file */

/* Copyright 1984-2001 Wind River Systems, Inc. */

/*
modification history
--------------------
01a,13nov01,yvp  created.
*/

#ifndef __INCexcLibPh
#define __INCexcLibPh

#ifdef __cplusplus
extern "C" {
#endif

#ifndef	_ASMLANGUAGE

/* Select extended (32-bit) calls from the vector table to handlers */

extern BOOL excExtendedVectors;

/* function declarations */

#if defined(__STDC__) || defined(__cplusplus)
extern STATUS 	excVecInit (void);
extern STATUS 	excInit (void);
extern void 	excTask (void);
#else
extern STATUS 	excVecInit ();
extern STATUS 	excInit ();
extern void 	excTask ();
#endif	/* __STDC__ */

#endif	/* _ASMLANGUAGE */

#ifdef __cplusplus
}
#endif

#endif /* __INCexcLibPh */
