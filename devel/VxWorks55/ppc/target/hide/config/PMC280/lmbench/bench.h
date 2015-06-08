/*
 * $Id: bench.h,v 1.1.1.7 2003/07/09 15:03:03 anish Exp $
 */
#ifndef _BENCH_H
#define _BENCH_H

#include	    <assert.h>
#include        <ctype.h>
#include        <stdio.h>
#include        <unistd.h>
#include        <stdlib.h>
#include        <fcntl.h>
#include        <signal.h>
#include        <errno.h>
#include        <string.h>
#include        <sys/types.h>
#include        <sys/mman.h>
#include        <sys/stat.h>
#include        <sys/wait.h>
#include        <time.h>
#include        <sys/socket.h>
#ifdef TORNADO_1_0_1 
#include        <streams/un.h>
#endif
#if 0
#include        <sys/resource.h>
#endif

#include	<rpc/rpc.h>
#include	"stats.h"
#include	"Lib_timing.h"

/*
#include	"lib_tcp.h"
#include	"lib_udp.h"
#include	"lib_unix.h"
*/

#undef RUSAGE

#ifdef	DEBUG
#	define		debug(x) fprintf x
#else
#	define		debug(x)
#endif
#ifdef	NO_PORTMAPPER	/* not recently tested */
#define	TCP_XACT	-3962
#define	TCP_CONTROL	-3963
#define	TCP_DATA	-3964
#define	TCP_CONNECT	-3965
#else
#define	TCP_XACT	(u_long)404039	/* XXX - unregistered */
#define	TCP_CONTROL	(u_long)404040	/* XXX - unregistered */
#define	TCP_DATA	(u_long)404041	/* XXX - unregistered */
#define	TCP_CONNECT	(u_long)404042	/* XXX - unregistered */
#define	UDP_XACT 	(u_long)404032	/* XXX - unregistered */
#define	UDP_DATA 	(u_long)404033	/* XXX - unregistered */
#define	VERS		(u_long)1
#endif

#define	UNIX_CONTROL	"/tmp/lmbench.ctl"
#define	UNIX_DATA	"/tmp/lmbench.data"
#define	UNIX_LAT	"/tmp/lmbench.lat"

/*
 * socket send/recv buffer optimizations
 */
#define	SOCKOPT_READ	0x0001
#define	SOCKOPT_WRITE	0x0002
#define	SOCKOPT_RDWR	0x0003
#define	SOCKOPT_PID	0x0004
#define	SOCKOPT_REUSE	0x0008
#define	SOCKOPT_NONE	0

#ifndef SOCKBUF
#define	SOCKBUF		(1024*1024)
#endif

#ifndef	XFERSIZE
#define	XFERSIZE	(64*1024)	/* all bandwidth I/O should use this */
#endif

#ifdef SYS5
#define	bzero(b, len)	memset(b, 0, len)
#define	bcopy(s, d, l)	memcpy(d, s, l)
#define	rindex(s, c)	strrchr(s, c)
#endif
#define	gettime		usecs_spent
#define	streq		!strcmp
#define	ulong		unsigned long

#define	SMALLEST_LINE	32		/* smallest cache line size */
#define	TIME_OPEN2CLOSE

#define	GO_AWAY	signal(SIGALRM, exit); alarm(60 * 60);
#define	REAL_SHORT	   50000
#define	SHORT	 	 1000000
#define	MEDIUM	 	 2000000
#define	LONGER		 7500000	/* for networking data transfers */
#define	ENOUGH		REAL_SHORT

#define	TRIES		11

typedef struct {
	int	N;
	uint64	u[TRIES];
	uint64	n[TRIES];
} result_t;
void    insertinit(result_t *r);
void    insertsort(uint64, uint64, result_t *);
void	save_median();
void	save_minimum();
void	save_results(result_t *r);
void	get_results(result_t *r);


#define	BENCHO(loop_body, overhead_body, enough) { 			\
	int 		__i, __N;					\
	double 		__oh;						\
	result_t 	__overhead, __r;				\
	insertinit(&__overhead); insertinit(&__r);			\
	__N = (enough == 0 || get_enough(enough) <= 100000) ? TRIES : 1;\
	if (enough < LONGER) {loop_body;} /* warm the cache */		\
	for (__i = 0; __i < __N; ++__i) {				\
		BENCH1(overhead_body, enough);				\
		insertsort(gettime(), get_n(), &__overhead);		\
		BENCH1(loop_body, enough);				\
		insertsort(gettime(), get_n(), &__r);			\
	}								\
	for (__i = 0; __i < __r.N; ++__i) {				\
		__oh = __overhead.u[__i] / (double)__overhead.n[__i];	\
		__r.u[__i] -= (uint64)((double)__r.n[__i] * __oh);	\
	}								\
	save_results(&__r);						\
}

#define	BENCH(loop_body, enough) { 					\
	long		__i, __N;					\
	result_t 	__r;						\
	insertinit(&__r);						\
	__N = (enough == 0 || get_enough(enough) <= 100000) ? TRIES : 1;\
	if (enough < LONGER) {loop_body;} /* warm the cache */		\
	for (__i = 0; __i < __N; ++__i) {				\
		BENCH1(loop_body, enough);				\
		insertsort(gettime(), get_n(), &__r);			\
	}								\
	save_results(&__r);						\
}

#define	BENCH1(loop_body, enough) { 					\
	double		__usecs;					\
	BENCH_INNER(loop_body, enough);  				\
	__usecs = gettime();						\
	__usecs -= t_overhead() + get_n() * l_overhead();		\
	lmbench_settime((uint64)__usecs);					\
}
	
#define	BENCH_INNER(loop_body, enough) { 				\
	static long	__iterations = 1;				\
	int		__enough = get_enough(enough);			\
	long		__n;						\
	double		__result = 0.;					\
									\
	while(__result < 0.95 * __enough) {				\
	start(0);							\
	for (__n = __iterations; __n > 0; __n--) {			\
		loop_body;						\
	}								\
	__result = stop(0,0);						\
	if (__result < 0.99 * __enough || __result > 1.2 * __enough) {	\
		if (__result > 150) {					\
			double	tmp = __iterations / __result;		\
			tmp *= 1.1 * __enough;				\
			__iterations = (long)(tmp + 1);			\
		} else							\
			__iterations *= 10;				\
	}								\
	} /* while */							\
	save_n((uint64)__iterations); lmbench_settime((uint64)__result);	\
}

#if 0
	/*  \
	   fprintf(stderr, "\tN=%d u=%lu", __interations, (unsigned long)usecs);	\
	   fflush(stderr);\
	   fprintf(stderr, " c=%.2f\tr=%.2f\n", (double)usecs/__interations,	\
		((double)usecs - t_overhead - __interations * l_overhead)		\
		/ __interations );  			\
	*/ \
#endif

/*
 * Standard timing loop.  Usage:
 *
 *	LOOP_FIRST(N, usecs, time)
 *	<code that you want timed>
 *	LOOP_LAST(N, usecs, time)
 *
 * time is how long you think it should run to be accurate.
 * "N" is a variable that will be set to the number of times it 
 * took to get "usecs" duration.  You then use N & usecs to print
 * out your results.
 * 
 * Notes: 
 *
 * Adjust the amount of time proportional to how
 * far we need to go.  We want time/usecs to be ~1.
 *
 * For systems with low resolution clocks, usecs can
 * be 0 or very close to 0.  We don't know how 
 * much time we spent, it could be anywhere from
 * 1 to 9999 usecs.  We pretend it was 1000 usecs.
 * The 129 value is because some systems bump the
 * timeval each time you call gettimeofday_lmbench() */
#define	LOOP_FIRST(N, usecs, time)			\
	N = 0;						\
	do {						\
		if (!N) {				\
			N = 1;				\
		} else {				\
			double	__adj;			\
			int	__n;			\
			if (usecs <= 129) {		\
				usecs = 1000;		\
			}				\
			__adj = (int)((time * 1.5)/usecs + .9);	\
			__n = N * __adj;			\
			/* printf("\tN=%.2f u=%.2f a=%.2f n=%d\n", \
			    (double)N, (double)usecs, __adj, __n);  \
			*/ \
			N = __n <= N ? N+1 : __n;	\
		}					\
timit:		usecs = N;				\
		start(0);				\
		while (usecs--) {

#define	LOOP_LAST(N, usecs, time)			\
		}					\
		usecs = stop(0,0);			\
	} while (usecs < time);				

#define	OBENCH(test, enough, result) { 				\
	int	__bench_n, __bench_i, __bench_enough;		\
								\
	__bench_enough = get_enough(enough);			\
	test;							\
	LOOP_FIRST(__bench_n, __bench_i, __bench_enough);	\
	test;							\
	LOOP_LAST(__bench_n, __bench_i, __bench_enough);	\
	result = __bench_i;					\
	result -= t_overhead();					\
	result /= __bench_n;					\
	/*							\
	printf("usecs=%d n=%d c=%f r=%f\n", __bench_i, __bench_n, \
	    (double)__bench_i/__bench_n, result);		\
	*/							\
}


/*
 * Generated from msg.x which is included here:

	program XACT_PROG {
	    version XACT_VERS {
		char
		RPC_XACT(char) = 1;
    	} = 1;
	} = 3970;

 * Please do not edit this file.
 * It was generated using rpcgen.
 */

#include <rpc/types.h>

#define XACT_PROG ((u_long)404040)
#define XACT_VERS ((u_long)1)
#define RPC_XACT ((u_long)1)
#define RPC_EXIT ((u_long)2)
extern char *rpc_xact_1();
extern char *client_rpc_xact_1();

/*new function declarations added by vijay*/
int  gettimeofday_lmbench(struct timeval *tp, struct timezone *ptr );
int getpagesize_lmbench(void);
int frcLmbench();
#endif /* _BENCH_H */
