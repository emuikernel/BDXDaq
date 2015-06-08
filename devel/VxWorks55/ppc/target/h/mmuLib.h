/* mmuLib.h - mmuLib header file */

/* Copyright 1984-1999 Wind River Systems, Inc. */

/*
modification history
--------------------
01m,20jan03,jb   Resolving SPR 79707
01l,01mar00,frf  Add SH4 support for T2.
01k,28aug98,st   added prototype for mmuSh7750LibInit.
01j,15nov96,wt   added prototype for mmuSh7700LibInit.
02b,20jan99,cdp  removed support for old ARM libraries.
01l,12nov98,cdp  removed prototype mmuLibInit() for new ARM architectures.
01k,09apr98,hdn  added PentiumPro support.
01j,19sep95,tpr  added prototype mmuPpcLibInit() for PowerPC.
01i,24nov93,hdn  added prototype mmuLibInit() for I80X86.
01i,02dec93,pme  added Am29K family support.
01h,01oct92,jcf  rolled out 01g to remove warnings.
01g,27sep92,kdl  deleted prototypes for mmu1eLibInit, mmu30LibInit, mmu40LibInit
01f,22sep92,rrr  added support for c++
01e,22sep92,rdc  added support for sun1e.
01d,17sep92,rdc  added prototypes and errors for sun1e.
01c,30jul92,rdc  added prototypes for mmu30LibInit and mmu40LibInit.
01b,28jul92,rdc  removed superfluous function prototypes.
01a,08jul92,rdc  written.
*/

#ifndef __INCmmuLibh
#define __INCmmuLibh

#ifdef __cplusplus
extern "C" {
#endif

#define	S_mmuLib_INVALID_PAGE_SIZE		(M_mmuLib | 1)
#define	S_mmuLib_NO_DESCRIPTOR			(M_mmuLib | 2)
#define	S_mmuLib_INVALID_DESCRIPTOR		(M_mmuLib | 3)

/* sun4 specific codes */

#define	S_mmuLib_OUT_OF_PMEGS			(M_mmuLib | 5)

/* virtual addresses must be in the first or last 1/2 Gibabyte of
 * virtual address space
 */

#define	S_mmuLib_VIRT_ADDR_OUT_OF_BOUNDS	(M_mmuLib | 6)

typedef struct mmuTransTblStruct * MMU_TRANS_TBL_ID;

#if defined(__STDC__) || defined(__cplusplus)

STATUS mmu30LibInit (int pageSize);
STATUS mmu40LibInit (int pageSize);
STATUS mmu1eLibInit (int pageSize);
#if (CPU_FAMILY != ARM)
STATUS mmuLibInit (int pageSize);
#endif
STATUS mmuPro32LibInit (int pageSize);
STATUS mmuPro36LibInit (int pageSize);
STATUS mmuAm29kLibInit (int pageSize);
STATUS mmuPpcLibInit (int mmuType, int * pPhysMemDesc, int elementNb, int *);
STATUS mmuSh7750LibInit (int pageSize);
STATUS mmuSh7700LibInit (int pageSize);

#else	/* __STDC__ */

STATUS mmu30LibInit ();
STATUS mmu40LibInit ();
STATUS mmu1eLibInit ();
#if (CPU_FAMILY != ARM)
STATUS mmuLibInit ();
#endif
STATUS mmuPro32LibInit ();
STATUS mmuPro36LibInit ();
STATUS mmuAm29kLibInit ();
STATUS mmuPpcLibInit ();
STATUS mmuSh7750LibInit ();
STATUS mmuSh7700LibInit ();

#endif	/* __STDC__ */

#ifdef __cplusplus
}
#endif

#endif /* __INCmmuLibh */
