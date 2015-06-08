/* bkendlog.h - backend debug log library header */

/* Copyright 1984-1994 Wind River Systems, Inc. */

/*
modification history
--------------------
01b,02mar98,pcn  WTX 2: changed bkendLogInit and bkendLog parameters.
01a,17may95,tpr written, inspired from bkendlib.h version 01f.
          + p_m got rid of u_int.
*/

#ifndef __INCbkendlogh
#define __INCbkendlogh

#ifdef __cplusplus
extern "C" {
#endif

/* function declarations */

extern STATUS   bkendLogInit 
    (
    char *      pLogFileName,           /* File name to save info in */
    UINT32      wdbLogMaxSize           /* Max size for the log file */
    );

extern void     bkendLog 
    (
    u_long      procNum,                /* procedure number to perform */
    UINT32      xmit,                   /* request xmit: TRUE or FALSE */
    char *      args,                   /* Input/Output structure pointer */
    INT32       timeoutNb,              /* timeout number */
    INT32       sequenceNumber,         /* sequence number */
    UINT32      status                  /* request status */
    );

extern void     bkendLogClose (void);

#ifdef __cplusplus
}
#endif

#endif /* __INCbkendlogh */

