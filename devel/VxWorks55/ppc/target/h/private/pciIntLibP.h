/* pciIntLibP.h - PCI Interrupt support private constants header file */

/* Copyright 1984-1998 Wind River Systems, Inc. */

/*
modification history
--------------------
01a,04mar98, tm  derived from pciIntLib.h v01a 
*/

#ifndef __INCpciIntLibPh
#define __INCpciIntLibPh

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _ASMLANGUAGE

typedef struct pciIntRtn
    {
    DL_NODE	node;		/* double link list */
    VOIDFUNCPTR	routine;	/* interrupt handler */
    int		parameter;	/* parameter of the handler */
    } PCI_INT_RTN;

#endif	/* _ASMLANGUAGE */

#ifdef __cplusplus
}
#endif

#endif /* __INCpciIntLibPh */
