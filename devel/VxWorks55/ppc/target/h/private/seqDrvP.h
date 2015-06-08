/* seqDrvP.h - event buffer library header */

/* Copyright 1993 Wind River Systems, Inc. */

/*
modification history
--------------------
01a,10dec93,smb  written.
*/

#ifndef __INCseqdrvph
#define __INCseqdrvph


#ifdef __cplusplus
extern "C" {
#endif

#if defined(__STDC__) || defined(__cplusplus)

extern UINT32   seqStamp (void);
extern UINT32   seqStampLock (void);
extern STATUS 	seqEnable (void);
extern STATUS 	seqDisable (void);
extern STATUS 	seqConnect ( FUNCPTR dummy, int dummyArg );
extern UINT32   seqPeriod (void);
extern UINT32   seqFreq (void);

#else   /* __STDC__ */

extern UINT32   seqStamp ();
extern UINT32   seqStampLock ();
extern STATUS 	seqEnable ();
extern STATUS 	seqDisable ();
extern STATUS 	seqConnect ();
extern UINT32   seqPeriod ();
extern UINT32   seqFreq ();

#endif  /* __STDC__ */

#ifdef __cplusplus
}
#endif

#endif /* __INCseqdrvph */

