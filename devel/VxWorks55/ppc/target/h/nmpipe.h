/* NmPipe.h - Named pipe routines */

/* Copyright 1984-1997 Wind River Systems, Inc. */

/*
modification history
--------------------
01a,24oct97, pdn  written
*/

#ifndef __INCNmPipeh
#define __INCNmPipeh

#ifdef __cplusplus
extern "C" {
#endif

/* defines */

#define DEF_PIPE_NAME "\\\\.\\pipe\\simnt" /* Default pipe name */
#define OVERLAPPED_IO TRUE
#define PIPE_BUF_SIZE 1048576 /* 1 MByte */

/* function declarations */

HANDLE pipeCreate (void);
HANDLE pipeConnect (void);
int pipeWrite (HANDLE hPipe, char *pBuf, int nByte, int nTimeOut);
int pipeRead (HANDLE hPipe, char *pBuf, int nByte, int nTimeOut);
BOOL pipeCheck (HANDLE hPipe);

#ifdef __cplusplus
}
#endif

#endif /* __INCNmPipeh */
