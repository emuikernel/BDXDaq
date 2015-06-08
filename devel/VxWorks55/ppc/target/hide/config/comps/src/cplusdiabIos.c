/* cplusDiabIos.c - VxWorks iostreams class library initialization */

/* Copyright 1995 Wind River Systems, Inc. */

/*
modification history
--------------------
01b,18mar01,sn   INCLUDE_CPLUS_IOSTREAMS_FULL implies INCLUDE_CPLUS_IOSTREAMS
01a,06nov01,sn   wrote
*/

/*
DESCRIPTION
This file is used to include the iostreams classes in the 
VxWorks build. The routines are only included when this file is 
included by usrConfig.c.

NOMANUAL
*/

#ifndef __INCcplusIosc
#define __INCcplusIosc

#ifdef INCLUDE_CPLUS_IOSTREAMS_FULL
#define INCLUDE_CPLUS_IOSTREAMS
#endif

#ifdef INCLUDE_CPLUS_IOSTREAMS
extern char __diab_cplusplus_std_libraryInit;

char * __cplusIosObjs [] =
{
    &__diab_cplusplus_std_libraryInit,
    0
};
#endif

#ifdef INCLUDE_CPLUS_IOSTREAMS_FULL
/* TODO */
#endif

#endif /* __INCcplusIosc */
