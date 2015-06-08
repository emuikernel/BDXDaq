/* tickLib.h - header file for tickLib.c */

/* Copyright 1984-2002 Wind River Systems, Inc. */

/*
modification history
--------------------
02c,22may02,jgn  removed vxTicks and vxAbsTicks declarations - SPR #70255
02b,22sep92,rrr  added support for c++
02a,04jul92,jcf  cleaned up.
01c,26may92,rrr  the tree shuffle
01b,04oct91,rrr  passed through the ansification filter
		  -fixed #else and #endif
		  -changed VOID to void
		  -changed copyright notice
01a,05oct90,shl created.
*/

#ifndef __INCtickLibh
#define __INCtickLibh

#ifdef __cplusplus
extern "C" {
#endif

/* function declarations */

#if defined(__STDC__) || defined(__cplusplus)

extern void 	tickAnnounce (void);
extern void 	tickSet (ULONG ticks);
extern ULONG 	tickGet (void);

#else

extern void 	tickAnnounce ();
extern void 	tickSet ();
extern ULONG 	tickGet ();

#endif	/* __STDC__ */

#ifdef __cplusplus
}
#endif

#endif /* __INCtickLibh */
