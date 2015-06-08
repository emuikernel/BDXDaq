/* -*- C++ -*- */
/* An abstraction for mapping the SunOS threads to POSIX threads.*/

#if defined (__osf__) && !defined (ACE_PTHREADS_MAP)
#define ACE_PTHREADS_MAP
//
//  Threads mapping definitions
#define thread_t        pthread_t
#define thread_key_t    pthread_key_t
//#define thr_self	pthread_self
#define thr_keycreate   pthread_keycreate
#define thr_getspecific pthread_getspecific
#define thr_setspecific pthread_setspecific
#define USYNC_THREAD 	MUTEX_NONRECURSIVE_NP
#define USYNC_PROCESS 	MUTEX_NONRECURSIVE_NP
#define THR_DETACHED    0	// ?? ignore in most places 
#define THR_BOUND    	1	// ?? ignore in most places
#define THR_NEW_LWP	2	// ?? ignore in most places
//  Mutex mapping definitions
#define mutex_t         pthread_mutex_t
#define mutex_init	pthread_mutex_init
#define mutex_destroy	pthread_mutex_destroy
#define mutex_lock	pthread_mutex_lock
#define mutex_trylock	pthread_mutex_trylock
#define mutex_unlock    pthread_mutex_unlock

//  Condition mapping definitions
#define cond_t          pthread_cond_t
#define cond_init	pthread_cond_init
#define cond_destroy	pthread_cond_destroy
#define cond_wait	pthread_cond_wait
#define cond_timedwait  pthread_cond_timedwait
#define cond_signal	pthread_cond_signal
#define cond_broadcast	pthread_cond_broadcast

//  Miscellanious mapping definitions
#define timestruc_t 	struct timespec

#endif /* ACE_PTHREADS_MAP */
