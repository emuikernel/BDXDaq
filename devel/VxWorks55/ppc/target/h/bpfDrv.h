/* bpfDrv.h - include file for Berkeley Packet Filter (BPF) I/O interface */

/* Copyright 1999 - 2000 Wind River Systems, Inc. */
#include "copyright_wrs.h"

/*
modification history
--------------------
01b,05apr00,spm  added interface to remove I/O device
01a,24nov99,spm  written
*/

#ifndef __INCbpfDrvh
#define __INCbpfDrvh

#ifdef __cplusplus
extern "C" {
#endif

#include "net/bpf.h"

/* function declarations */

#if defined (__STDC__) || defined(__cplusplus)

extern STATUS bpfDrv (void);
extern STATUS bpfDevCreate (char *pDevName, int numUnits, int bufSize);
extern STATUS bpfDevDelete (char *pDevName);
extern STATUS bpfDrvRemove (void);
#else	/* __STDC__ */

extern STATUS bpfDrv ();
extern STATUS bpfDevCreate ();
extern STATUS bpfDevDelete ();
extern STATUS bpfDrvRemove ();
#endif	/* __STDC__ */

#ifdef __cplusplus
}
#endif

#endif /* __INCbpfDrvh */
