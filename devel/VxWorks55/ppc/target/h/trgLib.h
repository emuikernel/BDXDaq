/* trgLib.h - event triggers header */

/* Copyright 1997 - 1999 Wind River Systems, Inc. */

/*
modification history
--------------------
01d,16mar99,spm  recovered orphaned code from tor1_0_1.sens1_1 (SPR #25770)
01c,16dec98,nps  fix last change(!)
01b,14dec98,nps  added prototype for trgShowInit.
01a,27jan98,pr   written

*/

#ifndef __INCtrglibh
#define __INCtrglibh

#ifdef __cplusplus
extern "C" {
#endif

/* This file contains event triggering elements. */

#define TRIGGER_COND_NO 	0
#define TRIGGER_COND_YES 	1

#define TRIGGER_COND_FUNC 	0
#define TRIGGER_COND_VAR 	1
#define TRIGGER_COND_LIB        2

#define TRG_DISABLE  	        0  
#define TRG_ENABLE   	        1

#define TRIGGER_EQ 	0
#define TRIGGER_NEQ 	1
#define TRIGGER_LSS 	2
#define TRIGGER_LEQ 	3
#define TRIGGER_GRT 	4
#define TRIGGER_GEQ 	5
#define TRIGGER_OR 	6
#define TRIGGER_AND 	7

#define TRG_CTX_ANY      0
#define TRG_CTX_ANY_TASK 1
#define TRG_CTX_TASK     2
#define TRG_CTX_ANY_ISR  3
#define TRG_CTX_ISR      4
#define TRG_CTX_SYSTEM   5

#define TRG_ACT_NONE     0
#define TRG_ACT_WV_START 1
#define TRG_ACT_WV_STOP  2
#define TRG_ACT_FUNC     3

#ifndef _ASMLANGUAGE

#include "private/trgLibP.h"

typedef struct trigger * TRIGGER_ID;

#if defined(__STDC__) || defined(__cplusplus)
extern STATUS   	trgLibInit (void);
extern TRIGGER_ID   	trgAdd (event_t event, int status, int contextType, 
				UINT32 contextId, OBJ_ID objId, int conditional, 
				int condType, int * condEx1, int condOp, int condEx2, 
				BOOL disable, TRIGGER_ID chain, int actionType, 
				FUNCPTR actionFunc, BOOL actionDef, int actionArg);
extern STATUS   	trgDelete (TRIGGER_ID trgId);
extern STATUS   	trgOn (void);
extern void   	        trgOff (void);
extern STATUS   	trgEnable (TRIGGER_ID trgId);
extern STATUS   	trgDisable (TRIGGER_ID trgId);
extern STATUS   	trgChainSet (TRIGGER_ID fromId, TRIGGER_ID toId);
extern STATUS   	trgShow (TRIGGER_ID trgId, int options);
extern void	   	trgShowInit (void);
#else   /* __STDC__ */
extern STATUS   	trgLibInit ();
extern TRIGGER_ID   	trgAdd ();
extern STATUS   	trgDelete ();
extern STATUS   	trgOn ();
extern void     	trgOff ();
extern STATUS   	trgEnable ();
extern STATUS   	trgDisable ();
extern STATUS   	trgChainSet ();
extern STATUS   	trgShow ();
extern void	   	trgShowInit ();
#endif  /* __STDC__ */

#ifdef __cplusplus
}
#endif

#endif /* _ASMLANGUAGE */ 

#endif /* __INCtrglibph*/
