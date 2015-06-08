/* configNet.h - network configuration header */

/* Copyright 1984-2002 Wind River Systems, Inc. */

/*
TODO -	Remove the template modification history and begin a new history
	starting with version 01a and growing the history upward with
	each revision.

modification history
--------------------
01e,28may02,dat  exclude driver if NETWORK not defined, 77135
01d,18apr02,rhe  Added C++ Protection
01c,13dec00,pai  corrected endDevTbl[] definition (SPR #62994) and specified
                 the return type in the END_LOAD_FUNC declaration.
01b,21jul98,db   dec21x40End replaced with template END driver. 
01a,07jul97,dat  written (from mv2603/configNet.h,ver 01b)
*/
 
#ifndef INCnetConfigh
#define INCnetConfigh

#ifdef __cplusplus
extern "C" {
#endif

#ifdef INCLUDE_NETWORK /* ignore everything if NETWORK not included */

#include "vxWorks.h"
#include "end.h"

/* This template presumes the template END driver */

#define END_LOAD_FUNC	templateLoad
#define END_BUFF_LOAN   1

/* <devAdrs>:<PCIadrs>:<ivec>:<ilevel>:<memBase>:<memSize>:<userFlags> */

#define	END_LOAD_STRING	"0x81020000:0x80000000:0x12:0x12:-1:0:0x80000000"

IMPORT END_OBJ * END_LOAD_FUNC ();

END_TBL_ENTRY endDevTbl [] =
    {
    { 0, END_LOAD_FUNC, END_LOAD_STRING, END_BUFF_LOAN, NULL},
    { 0, END_TBL_END, NULL, 0, NULL},
    };

#ifdef __cplusplus
}
#endif

#endif /* INCLUDE_NETWORK */
#endif /* INCnetConfigh */
