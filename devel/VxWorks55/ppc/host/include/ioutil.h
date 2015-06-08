/* ioutil.h - io utility header */

/* Copyright 1984-1995 Wind River Systems, Inc. */

/*
modification history
--------------------
01b,20jan95.jcf  made more portable.
01a,24sep93,jcf  written.
*/

#ifndef __INCioutilh
#define __INCioutilh

#ifdef __cplusplus
extern "C" {
#endif

/* function declarations */

#if defined(__STDC__) || defined(__cplusplus)

extern int ioWrite (int fd, char *ptr, int nbytes);
extern int ioRead (int fd, char *buffer, int maxbytes);
extern int ioRdString (int fd, char string[], int maxbytes);
extern int ioNullFd (void);

#else	/* __STDC__ */

extern int ioWrite ();
extern int ioRead ();
extern int ioRdString ();
extern int ioNullFd ();

#endif	/* __STDC__ */

#ifdef __cplusplus
}
#endif

#endif /* __INCioutilh */
