/* mutexPxLibP.h - kernel mutex and condition header file */

/* Copyright 1993-2001 Wind River Systems, Inc. */

/*
modification history
--------------------
01b,24sep01,gls  removed definition and use of __P() macro (SPR #28330)
01a,18feb93,rrr  written.
*/

#ifndef __INCmutexPxLibPh
#define __INCmutexPxLibPh

#include "qLib.h"

typedef struct
    {
    int		m_owner;
    Q_HEAD	m_queue;
    } mutex_t;

typedef struct
    {
    mutex_t	*c_mutex;
    Q_HEAD	c_queue;
    } cond_t;

struct timespec;

extern void mutex_init (mutex_t *, void *);
extern void mutex_destroy (mutex_t *);

extern void cond_init (cond_t *, void *);
extern void cond_destroy (cond_t *);

extern void mutex_lock (mutex_t *);
extern void mutex_unlock (mutex_t *);

extern void cond_signal (cond_t *);
extern int cond_timedwait (cond_t *, mutex_t *, const struct timespec *);

#endif /* __INCmutexPxLibPh */
