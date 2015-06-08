/* wvBufferP.h - generic buffer definition for buffers used with windview */

/* Copyright 1997 Wind River Systems, Inc. */

/*
modification history
--------------------
01e,06may98,cth  added nBytesRtn to BUFFER_DESC
01d,02feb98,nps  Allow inclusion in (68k) assembler source.
01c,18dec97,cth  renamed from buffer.h to wvBufferP.h, added flushRtn, comments
01b,21nov97,nps  added objCore to the start of BUFFER_DESC.
01a,16nov97,cth  written.
*/

/*
This file contains the definition of a structure that should make up the
core of any buffer id that can be passed to wvLib.  Windview uses a
pointer to this type of structure as a buffer id and as a way of 
accessing the buffer's routines.  See wvLib for a detailed description
of how the BUFFER_DESC is used.
*/
 
#ifndef __INCwvbufferph
#define __INCwvbufferph


#ifdef __cplusplus
extern "C" {
#endif

#include "private/semLibP.h"


/* typedefs */

#ifndef _ASMLANGUAGE

typedef struct bufferDesc	/* BUFFER_DESC */
    {
    OBJ_CORE    objCore;            /* buffer object core */
    INT32       (*readReserveRtn)();/* request zero-copy read of buffer */
    STATUS      (*readCommitRtn)(); /* commit after zero-copy read of buffer */
    UINT8 *     (*writeRtn)(); 	    /* write (with reserve) to buffer */
    INT32	(*flushRtn)();	    /* flush routine of buffer */
    INT32	(*nBytesRtn)();	    /* number readable bytes in buffer */
    SEMAPHORE	threshXSem;	    /* sem given by buf when thresh crossed */
    INT32	threshold;	    /* arbitrary size in bytes set by buffer */
    } BUFFER_DESC;

typedef BUFFER_DESC *BUFFER_ID;

#endif /* _ASMLANGUAGE */

#ifdef __cplusplus
}
#endif

#endif /* __INCwvbufferph*/
