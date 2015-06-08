/* connLibP.h - event buffer library header */

/* Copyright 1993 Wind River Systems, Inc. */

/*
modification history
--------------------
01a,10dec93,smb  written.
*/

#ifndef __INCconnlibph
#define __INCconnlibph

#ifdef __cplusplus
extern "C" {
#endif

#if defined(__STDC__) || defined(__cplusplus)

STATUS    connectionInit (void);
void      connectionClose (void);
void      connectionError (void);
void      dataTransfer (char * buffer, size_t bufSize);

#else   /* __STDC__ */

STATUS    connectionInit ();
void      connectionClose ();
void      connectionError ();
void      dataTransfer ();

#endif  /* __STDC__ */

#ifdef __cplusplus
}
#endif

#endif /* __INCconnlibph*/

