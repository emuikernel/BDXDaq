/* excSimsparcLib.h - simsparc exception library header */

/* Copyright 1993-1995 Wind River Systems, Inc. */

/*
modification history
--------------------
01a,07jun95,ism  derived from simsparc
*/

#ifndef __INCexcSimsparcLibh
#define __INCexcSimsparcLibh

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _ASMLANGUAGE

/* variable declarations */

extern FUNCPTR  excExcepHook;   /* add'l rtn to call when exceptions occur */

typedef struct
    {
    int valid;
    int pc;
    int	npc;
    int	ps;
    int	sig;
    int	code;
    int	addr;
    int	ptcb;
    } EXC_INFO;

#endif  /* _ASMLANGUAGE */

#ifdef __cplusplus
}
#endif

#endif /* __INCexcSimsparcLibh */
