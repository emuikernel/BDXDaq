/* dynlklib.h - header of generic interface for Dynamically Linked Libraries */

/* Copyright 1995 Wind River Systems, Inc. */

/*
modification history
--------------------
01a,08feb95,c_s  written
	  + pad
	  + tpr
*/

#ifndef __INCdynlklibh
#define __INCdynlklibh

#ifdef __cplusplus
extern "C" {
#endif


/* type definitions */

typedef struct dynlkFuncVec
    {
    char *	name;	/* routine's name */
    void	(*func)();
    } DYNLK_FUNC;

/* Macro definitions */

/*
 * This macro takes a DYNLK_FUNC vector, an index within it, and the prototype
 * of the function, to return an expression that represents a function call
 */

#define DYNLK_FUNC_PROTO(proto,pVec,ix) \
    ((proto) pVec[ix].func)

/* function declarations */

#if defined(__STDC__) || defined(__cplusplus)

extern int dynlkFvBind (char * dllName, DYNLK_FUNC * pVec, int funcNb,
			void ** ppDllHandle);
extern STATUS dynlkFvTerminate (void * pDllHandle, DYNLK_FUNC * pVec,
				int funcNb);

#else	/* __STDC__ */

extern int dynlkFvBind ();
extern STATUS dynlkFvTerminate ();

#endif	/* __STDC__ */
#ifdef __cplusplus
}
#endif

#endif /* __INCdynlklibh */
