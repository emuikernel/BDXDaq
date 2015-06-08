/* cplusgnuComplex.c - force-link of C++ complex type  */

/* Copyright 1998 Wind River Systems, Inc. */

/*
modification history
--------------------
01e,06nov01,sn   use new prefixed names
01d,21mar01,sn   include ldcomplex.o
01c,24jan01,sn   use wrapper symbols
01b,11oct00,sn   regenerated
01a,03Jun98,sn   wrote
*/

/*
DESCRIPTION
This file is used to include support for the C++ complex type.
 
NOMANUAL
*/

#ifndef __INCcplusComplexc
#define __INCcplusComplexc
extern char ___x_gnu_dcomplex_o;
extern char ___x_gnu_ldcomplex_o;
extern char ___x_gnu_fcomplex_o;

char * __cplusComplexObjs [] =
{

    &___x_gnu_dcomplex_o,
    &___x_gnu_ldcomplex_o,
    &___x_gnu_fcomplex_o,
    0
};
#endif /* __INCcplusComplexc */
