/* pipeDrv.h - header file for pipeDrv.c */

/* Copyright 1984-2001 Wind River Systems, Inc. */

/*
modification history
--------------------
01f,05oct01,dcb  Fix SPR 9434.  Add pipeDevDelete() call.  Code merged back
                 from the AE tree (created for AE under SPR 26204).
01e,22sep92,rrr  added support for c++
01d,04jul92,jcf  cleaned up.
01c,26may92,rrr  the tree shuffle
01b,04oct91,rrr  passed through the ansification filter
		  -fixed #else and #endif
		  -changed copyright notice
01a,05oct90,shl created.
*/

#ifndef __INCpipeDrvh
#define __INCpipeDrvh

#ifdef __cplusplus
extern "C" {
#endif

/* function declarations */

#if defined(__STDC__) || defined(__cplusplus)

extern STATUS 	pipeDevCreate (char *name, int nMessages, int nBytes);
extern STATUS   pipeDevDelete (char *name, BOOL force);
extern STATUS 	pipeDrv (void);

#else	/* __STDC__ */

extern STATUS 	pipeDevCreate ();
extern STATUS   pipeDevDelete ();
extern STATUS 	pipeDrv ();

#endif	/* __STDC__ */

#ifdef __cplusplus
}
#endif

#endif /* __INCpipeDrvh */
