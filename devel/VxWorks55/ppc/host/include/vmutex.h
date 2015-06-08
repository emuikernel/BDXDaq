/* vmutex.h - mutual exclusion header */

/* Copyright 1995-1998 Wind River Systems, Inc. */

/*
modification history
--------------------
01b,23apr98,pcn  Added an API for counting semaphores.
01a,20jan95,jcf  based on VxWorks semaphores.
*/

#ifndef __INCvmutexh
#define __INCvmutexh

#include "host.h"

/* semaphore options */

#define SEM_Q_MASK		0x3	/* q-type mask */
#define SEM_Q_FIFO		0x0	/* first in first out queue */
#define SEM_Q_PRIORITY		0x1	/* priority sorted queue */
#define SEM_DELETE_SAFE		0x4	/* owner delete safe (vmutex opt.) */
#define SEM_INVERSION_SAFE	0x8	/* no priority inversion (vmutex opt.) */

#define NO_WAIT			(0)
#define WAIT_FOREVER		(-1)

/* binary semaphore initial state */

typedef enum		/* SEM_B_STATE */
    {
    SEM_EMPTY,			/* 0: semaphore not available */
    SEM_FULL			/* 1: semaphore available */
    } SEM_B_STATE;

/* types */

typedef struct semaphore /* SEMAPHORE */
    {
    int objCore;
    } SEMAPHORE;

typedef struct semaphore *SEM_ID;

/* function declarations */

#if defined(__STDC__) || defined(__cplusplus)

extern SEM_ID 	semMCreate (int options);
extern SEM_ID   semCCreate (int options, int initialCount);
extern STATUS 	semTerminate (SEM_ID semId);
extern STATUS 	semMInit (SEMAPHORE *pSem, int options);
extern STATUS   semCInit (SEMAPHORE *pSemaphore, int options, int initialCount);
extern STATUS 	semTake (SEM_ID semId, int timeout);
extern STATUS 	semGive (SEM_ID semId);

#else	/* __STDC__ */

extern SEM_ID 	semMCreate ();
extern SEM_ID   semCCreate ();
extern STATUS 	semTerminate ();
extern STATUS 	semMInit ();
extern STATUS   semCInit ();
extern STATUS 	semTake ();
extern STATUS 	semGive ();

#endif	/* __STDC__ */

#endif	/* __INCvmutexh */
