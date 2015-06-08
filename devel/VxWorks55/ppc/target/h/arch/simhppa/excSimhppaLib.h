/* excSimhppaLib.h - simhppa exception library header */

/* Copyright 1993-1994 Wind River Systems, Inc. */

/*
modification history
--------------------
01d,08jun94,ms   saved more state during exception/breakpoint
01c,10jan94,gae  added external declaration for excExcepHook
01b,26jan94,gae  rearranged EXC_INFO -- same as "simsparc".
01a,11aug93,gae  from rrr.
*/

#ifndef __INCexcSimhppaLibh
#define __INCexcSimhppaLibh

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _ASMLANGUAGE


/* variable declarations */

extern FUNCPTR  excExcepHook;   /* add'l rtn to call when exceptions occur */


#define	ESF	EXC_INFO

typedef struct
    {
    int valid;
    int pc;
    int npc;
    int ps;
    int sig;
    int code;
    int addr;
    int ptcb;
    int gr[32];
    int fr[32];
    int sr[8];
    int cr11;
    int state;
    int addr1;
    int addr2;
    int pad[4];
    } EXC_INFO;

#ifdef __cplusplus
}
#endif

#endif	/* _ASMLANGUAGE */

#endif /* __INCexcSimhppaLibh */
