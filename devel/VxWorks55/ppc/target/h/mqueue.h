/* mqueue.h - POSIX message queue library header */

/* Copyright 1984-1999 Wind River Systems, Inc. */

/*
modification history
--------------------
01e,22mar99,elg  Erroneous Fix for SPR 20532 (SPR 25989).
01d,12mar99,elg  include private header to get mqPxLibInit() (SPR 20532).
01c,19aug96,dbt  added define of MQ_PRIORITY_MAX (SPR #7039).
		 updated copyright.
01b,05jan94,kdl	 added modhist, added include of sigevent.h, general cleanup.
01a,01dec93,rrr  written.
*/

#ifndef __INCmqueueh
#define __INCmqueueh


#ifdef __cplusplus
extern "C" {
#endif

/* includes */

#include "sigevent.h"

/* defines */

#define MQ_PRIORITY_MAX		31	/* max priority */

/* type defs */

struct mq_attr
    {
    size_t	mq_maxmsg;
    size_t	mq_msgsize;
    unsigned	mq_flags;
    size_t	mq_curmsgs;
    };

struct mq_des;

typedef struct mq_des *mqd_t;


/* function declarations */

#if defined(__STDC__) || defined(__cplusplus)

extern mqd_t mq_open (const char *, int, ...);
extern int mq_close (mqd_t);
extern int mq_unlink (const char *);
extern int mq_send (mqd_t, const void *, size_t, int);
extern int mq_receive (mqd_t, void *, size_t, int *);
extern int mq_notify (mqd_t, const struct sigevent *);
extern int mq_setattr (mqd_t, const struct mq_attr *, struct mq_attr *);
extern int mq_getattr (mqd_t, struct mq_attr *);
extern int mqPxLibInit (int);

#else	/* __STDC__ */

extern mqd_t mq_open ();
extern int mq_close ();
extern int mq_unlink ();
extern int mq_send ();
extern int mq_receive ();
extern int mq_notify ();
extern int mq_setattr ();
extern int mq_getattr ();
extern int mqPxLibInit ();

#endif	/* __STDC__ */


#ifdef __cplusplus
}
#endif

#endif	/* INCmqueue.h */
