/* random.h - PPP random number generator header */

/* Copyright 1995 Wind River Systems, Inc. */

/*
modification history
--------------------
01a,17jun95,dzb  written.
*/

#ifndef	__INCrandomh
#define	__INCrandomh

#ifdef	__cplusplus
extern "C" {
#endif

/* function declarations */
 
#if defined(__STDC__) || defined(__cplusplus)
  
extern long	random (void);
extern void	srandom (int);

#else	/* __STDC__ */

extern long	random ();
extern void	srandom ();

#endif	/* __STDC__ */

#ifdef	__cplusplus
}
#endif

#endif	/* __INCrandomh */
