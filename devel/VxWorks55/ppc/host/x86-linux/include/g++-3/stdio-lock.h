/* stdio-lock.h - multithreaded iostreams header file */

/* Copyright 1998 Wind River Systems, Inc. */

/* 
modification history
--------------------
01d,04may99,sn  make safe for 'C'
01c,05may98,sn  cleaned up to use the new class vxw_mutex
01b,27apr98,sn  corrected behaviour of _IO_lock_init and _IO_lock_fini
01a,02mar98,sn	wrote
*/

/* This file defines what is needed for VxWorks multithread safe iostreams.
 * This version of iostreams is actually thread safe at the object level,
 * meaning that each stream mutation operation is either already atomic
 * or composed of atomic operations each of which leave the stream
 * is a sensible state. In particular individual "insertions" into
 * a stream are atomic.

 * In the interests of efficiency, our implementation has strayed somewhat 
 * from the existing multithreaded GNU libio framework.
 */

#ifndef __INCstdio_lockh
#define __INCstdio_lockh

#include <vxwMutex.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct vxw_mutex_t _IO_lock_t;
#define _IO_LOCK_T _IO_lock_t *

/* In the GNU scheme of things cin/cout/cerr are initialized statically
 * in a way that simulates construction. In particular a pointer to a lock
 * object (of type _IO_lock_t) is stored. The lock object itself is 
 * initialized at runtime when
 * constructors for static objects are called, via the magic of a line like
 * _IO_lock_t mylock = _IO_lock_initializer;
 * Note that (given the definition below) this is an alternative syntax for
 * _IO_lock_t mylock;
 */
#ifdef __cplusplus
#define _IO_lock_initializer _IO_lock_t ()
#else
/* This isn't quite as bogus as it looks: the underlying
 * implementation for _IO_lock_t (vxw_mutex_t) has a
 * no-op constructor.
 */
#define _IO_lock_initializer {0}
#endif
 
/* The existing multithreaded architecture has the concept of
 * cleanup regions. The idea is we save away the address of a function
 * the kernel should call in case we get deleted. In practice this
 * is used exclusively to avoid a task being deleted while it is
 * holding a streambuf's mutex. Since we use VxWorks deletion safe
 * mutexes we really do not need this protection. Unfortunately
 * there does not seem to be any way of making the _IO_cleanup_region
 * macros do the cleanup work for us (unlocking the mutex) without
 * incurring all sorts of unecessary overhead so we set them to no-ops. 
 * The cleanup work must be done explicitly in any particular case: for example
 *
 * _IO_cleanup_region_start (_IO_funlockfile, fp)
 * ... do some stuff ...
 * _IO_cleanup_region_end (_doit)
 *
 * should be immediately followed by the lines.
 *
 * #ifdef _IO_VXW_THREADS
 *  if (_doit) _IO_funlockfile (fp);
 * #endif
 *
 */

#define _IO_cleanup_region_start(_fct, _fp)  
#define _IO_cleanup_region_end(_doit) 


/* locking primitives */
/* _IO_lock_init and _IO_lock_fini are not used by iostreams proper
 * but are needed by the "stdio like environment", described in
 * iostdio.h, which is used to test iostreams. They take the
 * place of C++ constructors and destructors in C code.
 */
#define _IO_lock_init vxw_mutex_init
#define _IO_lock_fini vxw_mutex_fini
#define _IO_lock_lock vxw_mutex_lock
#define _IO_lock_unlock vxw_mutex_unlock

/* These take an _IO_FILE * */
#define _IO_flockfile(_fp) _IO_lock_lock (*((_fp) -> _lock))
#define _IO_funlockfile(_fp) _IO_lock_unlock (*((_fp) -> _lock))
#define _IO_ftrylockfile(_fp) /* not used */

#ifdef __cplusplus
}
#endif

#endif /* __INCstdio_lockh */
