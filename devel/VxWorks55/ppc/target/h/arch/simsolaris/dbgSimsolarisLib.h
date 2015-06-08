/* dbgSimsparcLib.h - simsparc debugger header */

/* Copyright 1995-2001 Wind River Systems, Inc. */

/*
modification history
-------------------
01c,26nov01,hbh  Added INST_SAV definition.
01b,29dec97,dbt  modified for new breakpoint scheme
01a,07jun95,ism  derived from simsparc
*/

#ifndef __INCdbgSimsparcLibh
#define __INCdbgSimsparcLibh

#ifdef __cplusplus
extern "C" {
#endif

#include "excLib.h"

#define	BREAK_ESF	EXC_INFO
#define	TRACE_ESF	BREAK_ESF		/* in case needed */

#define DBG_BREAK_INST		0x91d02001
#define DBG_NO_SINGLE_STEP	1

#define INST_SAV	0x9de3b000
#define INST_SAV_MASK	0xffeff000

#define INST_CALL       OP_1
#define INST_CALL_MASK  OP
#define JMPL_o7         (OP_2 + RD_o7 + OP3_38)
#define JMPL_o7_MASK    (OP + RD + OP3)

#define I7_OFFSET       0x0f    /* offset from base of stack, in ints */
#define I7_CONTENTS(sp) ((INSTR *)*(sp + I7_OFFSET))

#ifdef __cplusplus
}
#endif

#endif /* __INCdbgSimsparcLibh */
