/* pciIntLib.h - PCI Interrupt support public constants header file */

/* Copyright 1984-1998 Wind River Systems, Inc. */

/*
modification history
--------------------
01b,04mar98, tm  moved pciIntRtn structure to private header file
01a,24feb98, tm  derived from pciIomapLib.h v01a from ebsa285 BSP
*/

#ifndef __INCpciIntLibh
#define __INCpciIntLibh

#ifdef __cplusplus
extern "C" {
#endif

#ifndef PCI_IRQ_LINES
#define PCI_IRQ_LINES   32
#endif /* PCI_IRQ_LINES */

#ifndef _ASMLANGUAGE

#if defined(__STDC__) || defined(__cplusplus)

STATUS pciIntLibInit	(void);
STATUS pciIntConnect	(VOIDFUNCPTR *vector, VOIDFUNCPTR routine,
			 int parameter);
STATUS pciIntDisconnect	(VOIDFUNCPTR *vector, VOIDFUNCPTR routine);

#else	/* __STDC__ */

STATUS pciIntLibInit	();
STATUS pciIntConnect	();
STATUS pciIntDisconnect	();

#endif	/* __STDC__ */

#endif	/* _ASMLANGUAGE */

#ifdef __cplusplus
}
#endif

#endif /* __INCpciIntLibh */
