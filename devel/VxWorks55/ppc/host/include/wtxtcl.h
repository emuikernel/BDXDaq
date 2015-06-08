/* wtxtcl.h - header file for WTX Tcl binding */

/* Copyright 1994-1998 Wind River Systems, Inc. */

/* modification history
-----------------------
02a,25jun98,fle  re-added wtxTclErrorString
		 + removed old C-style declarations
01z,28jun96,c_s  correct prototype of wtxTclHandleRevoke.
01y,02nov95,c_s  added wtxTclBusyFuncSet for busy animation support.
01x,12sep95,c_s  changed prototypes of wtxTclHandle{,Grant,Revoke}; part of 
		   fix to SPR #4891.
01w,21jun95,s_w  correct use of __cplusplus macro - removed trailing '_'
01v,18jun95,c_s  removed wtxIntrQueryFunc, added non-ANSI prototypes, changed
                   wtxTclInit() prototype.
01u,12may95,c_s  upgraded for new architecture.
01t,28apr95,c_s  added wtxTclErrorString.
01s,22mar95,c_s  added prototype for wtxTclContextReset().
01r,13mar95,c_s  relocated Tcl implementation function prototypes to wtinit.c.
01q,10mar95,c_s  added wtxTclContextSet, wtxEndian, tWtxAgentModeSet.
01p,28feb95,p_m  added wtxSymTblInfoGet.
01o,25feb95,jcf  added tWtxTs[Un]Lock.
01n,07feb95,p_m  replaced tWtxMemZero() with tWtxMemSet().
		 added tWtxEventAdd().
		 replaced #include "htools.h" by #include "wtxmsg.h"
01m,19jan95,c_s  added some new routines.
01l,12jan95,c_s  added some new routines.
01k,21dec94,c_s  added wtxMemInfoGet, wtxIntrQueryFunc.
01j,15dec94,c_s  added C++ bracketing; some new functions.
01i,23nov94,c_s  added/reshuffled some extern declarations.
01h,16nov94,c_s  changed tWtxVio{Open,Close} to tWtx{Open,Close}.
01g,14nov94,c_s  added tWtxVio{Read,Write}.
01f,14nov94,c_s  added tWtxContextResume.
01e,14nov94,c_s  reformatted, added some routines, tweaked #includes.
01d,27oct94,c_s  added tWtxSymFind, tWtxSymListGet
01c,260ct94,pme  changed tWtxContextSourceStep to tWtxContextStep().
01b,26oct94,c_s  added some new routines, rearranged.
01a,12oct94,c_s  written
*/

#ifndef __INCwtxtclh
#define __INCwtxtclh

#include "tcl.h"
#include "wtx.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef struct tclMemBlock
    {
    int			size;		/* logical size in bytes of block */
    int			actualSize;	/* amount allocated for block */
    int			endian;		/* endianness of block */
    int 		fill;		/* fill value for gaps */
    unsigned char *	block;		/* the data (on the heap) */
    char *		name;		/* block name (on heap) */
    struct tclMemBlock *next;		/* a list is maintained of all blks */
    } TCL_MEM_BLOCK;

/* functions declaration */

extern int 	wtxTclInit
    (
    Tcl_Interp *	pInterp		/* Tcl interpreter         */
    );

extern HWTX	wtxTclHandle
    (
    Tcl_Interp *	pInterp,	/* Tcl interpreter         */
    char *		name		/* handle name to get back */
    );

extern char * 	wtxTclHandleGrant
    (
    Tcl_Interp *	pInterp,	/* Tcl interpreter */
    HWTX		hWtx		/* wtx handle      */
    );

extern void	wtxTclBusyFuncSet
    (
    Tcl_Interp *	pInterp,	/* Tcl interpreter       */
    void (*busyFunc)			/* busy function handle  */
	(
	void *		funcArg,	/* busy func arguments   */
	int		busyState	/* busy functionsa state */
	),
    void *		funcArg		/* funtion arguments     */
    );

extern char *	wtxTclErrorString
    (
    Tcl_Interp *	pInterp,	/* Tcl interpreter */
    unsigned int	errCode		/* error code      */
    );

extern int 	wtxTclHandleRevoke
    (
    Tcl_Interp *	pInterp,	/* interpreter context */
    HWTX		hWtx		/* WTX handle          */
    );

extern char *	memBlockCreateFromData
    (
    int		endian,			/* true if LE block to be created */
    char *	data,			/* buffer                         */
    int		len			/* length of above                */
    );

#ifdef __cplusplus
}
#endif /* __cplusplus (extern "C") */

#endif /* __INCwtxtclh */
