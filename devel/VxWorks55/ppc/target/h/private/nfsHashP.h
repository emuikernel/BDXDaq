/* nfsHashP.h - private nfs hash  header file */

/* Copyright 1992-1996 Wind River Systems, Inc. */

/*
modification history
--------------------
01a,15nov98,rjc  created.
*/

#ifndef __INCnfsHashPh
#define __INCnfsHashPh

#ifdef __cplusplus
extern "C" {
#endif

#if defined(__STDC__) || defined(__cplusplus)

extern void nfsHashTblSet (char *, int, u_long );
extern void nfsHashTblUnset (char *, int);
extern STATUS nfsFhLkup (NFS_FILE_HANDLE * ,  char *);
extern int nfsNmLkupIns (int, char *);

#else	/* __STDC__ */

extern void nfsHashTblSet ();
extern void nfsHashTblUnset ();
extern STATUS nfsFhLkup ();
extern int nfsNmLkupIns ();

#endif	/* __STDC__ */

#ifdef __cplusplus
}
#endif

#endif /* __INCnfsHashPh */
