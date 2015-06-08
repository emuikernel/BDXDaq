/* cplusgnuLang.c - force-link of C++ compiler support  */

/* Copyright 2000 Wind River Systems, Inc. */

/*
modification history
--------------------
01c,06nov01,sn   use new prefixed names
01b,24jan01,sn   use wrapper symbols
01a,11oct00,sn   wrote
*/

/*
DESCRIPTION
This file pulls in C++ compiler (libgcc) support.
 
NOMANUAL
*/

#ifndef __INCcplusLibgccC
#define __INCcplusLibgccC
extern char ___x_gnu___dummy_o;
extern char ___x_gnu__eh_o;
extern char ___x_gnu_cmathi_o;
extern char ___x_gnu_cstdlibi_o;
extern char ___x_gnu_tinfo_o;
extern char ___x_gnu_tinfo2_o;
extern char ___x_gnu_opnew_o;
extern char ___x_gnu_opvnew_o;
extern char ___x_gnu_opnewnt_o;
extern char ___x_gnu_opvnewnt_o;
extern char ___x_gnu_opdel_o;
extern char ___x_gnu_opvdel_o;
extern char ___x_gnu_opdelnt_o;
extern char ___x_gnu_opvdelnt_o;
extern char ___x_gnu_new_o;
extern char ___x_gnu_exception_o;

char * __cplusLibgccObjs [] =
{
    &___x_gnu___dummy_o,
    &___x_gnu__eh_o,
    &___x_gnu_cmathi_o,
    &___x_gnu_cstdlibi_o,
    &___x_gnu_tinfo_o,
    &___x_gnu_tinfo2_o,
    &___x_gnu_opnew_o,
    &___x_gnu_opvnew_o,
    &___x_gnu_opnewnt_o,
    &___x_gnu_opvnewnt_o,
    &___x_gnu_opdel_o,
    &___x_gnu_opvdel_o,
    &___x_gnu_opdelnt_o,
    &___x_gnu_opvdelnt_o,
    &___x_gnu_new_o,
    &___x_gnu_exception_o,
    0
};
#endif /* __INCcplusLibgccC */
