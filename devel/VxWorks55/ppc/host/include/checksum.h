/* checksum.h - checksum protocol header file */

/* Copyright 1984-1995 Wind River Systems, Inc. */

/*
modification history
--------------------
01a,17jan95,jcf  written.
*/

#ifndef __INCchecksumh
#define __INCchecksumh	1

#ifdef __cplusplus
extern "C" {
#endif


/* function declarations */

#if defined(__STDC__) || defined(__cplusplus)

extern unsigned short checksum (unsigned short *pAddr, int len);

#else	/* __STDC__ */

extern unsigned short checksum ();

#endif	/* __STDC__ */

#ifdef __cplusplus
}
#endif

#endif /* __INCchecksumh */
