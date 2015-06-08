/* cplusDiabLang.c - force-link of C++ compiler support  */

/* Copyright 2000 Wind River Systems, Inc. */

/*
modification history
--------------------
01b,06feb02,sn   added libimfp.a C++ objects
01a,06nov01,sn   wrote
*/

/*
DESCRIPTION
This file pulls in C++ language support.
 
NOMANUAL
*/

#ifndef __INCcplusLangC
#define __INCcplusLangC
extern char __dplusplus_intrinsicsInit;
extern char __diab_intrinsics_impfp_cplusInit;

char * __cplusLangObjs [] =
{
    &__dplusplus_intrinsicsInit,
    &__diab_intrinsics_impfp_cplusInit,
    0
};
#endif /* __INCcplusLangC */
