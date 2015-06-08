/* vmlib.h - header for architecture independent mmu interface */

/* Copyright 1994-1998 Wind River Systems, Inc. */

/*
modification history
--------------------
01c,21jan98,c_c  DLLized Target Server Implementation.
		 Added routines to hide globals Variables.
01b,07feb95,p_m  simplified vmLibInfo and removed unnecessary defines.
01a,02feb94,jcf  written from v1j of vmLib.h.
*/

#ifndef __INCvmLibh
#define __INCvmLibh

#ifdef __cplusplus
extern "C" {
#endif

/* Target MMU related informations  */

typedef struct
    {
    BOOL protectTextSegs;	/* TRUE == protext text segments */
    int	 pageSize;		/* MMU page size */	
    } VM_LIB_INFO;


extern STATUS tgtVmInit (void);
 
extern int  tgtVmPageSizeGet(void);	 /* returns the target page size */
 
extern BOOL tgtVmPageSizeProtected(void);/* tells if text protection is ON */
 

#ifdef __cplusplus
}
#endif

#endif /* __INCvmLibh */
