/* vxwMutex.h - simple locking macros for multithread safe iostreams and STL*/

/* Copyright 1998 Wind River Systems, Inc. */

/* 
modification history
--------------------
01i,16jun01,yvp use brackets with "extern C++" to avoid compiler error.
01h,09nov99,sn  prevent locks from being destroyed twice
01g,25feb99,sn  wrap the struct defn in extern "C++"
01f,24feb99,sn  make sure sem is initialized to zero
01e,09nov98,sn  ensure that initialization occurs whether we use
                the macros (C) or the methods (C++)
01d,29oct98,sn  lazily initialize locks on first use
01c,04sep98,sn  added void return type to lock and unlock methods 
01b,01sep98,sn  vxw_mutex_... macros are now defined for both C and C++
01a,01may98,sn	wrote
*/

#ifndef __INCvxwMutexh
#define __INCvxwMutexh

#include <semLib.h>

#ifdef __cplusplus
extern "C" {
#endif

/* There are C files that include this header. We must set everything
 * up so that vxw_mutex_t looks like an ordinary struct if
 * we're compiling C and a full blown class if its C++.
 */

/* The following macros take the place of member functions in C code
 */

#define vxw_mutex_init(_name) (_name).sem = semMCreate (SEM_Q_PRIORITY | \
	                                                SEM_DELETE_SAFE | \
	                                                SEM_INVERSION_SAFE)
  /*
   * Apparently the iostreams code will sometimes destroy a lock
   * explicitly (via vxw_mutex_fini) and implictly (through the
   * vxw_mutex destructor). We ensure deletion is only performed
   * once.
   */

#define vxw_mutex_fini(_name) do \
    { \
        if ((_name).sem != 0) \
            { \
            semTake ((_name).sem, WAIT_FOREVER); \
            semDelete ((_name).sem); \
            (_name).sem = 0 ; \
            } \
    } while (0)

#define vxw_mutex_lock(_name) do \
        { \
	/* initialize on first use: this works around a compiler bug that \
         * may allow a global lock to be taken before its constructor is \
         * called ... */ \
        if ((_name).sem == 0) \
            { \
            vxw_mutex_init(_name); \
            } \
        semTake ((_name).sem, WAIT_FOREVER); \
	} while (0)

#define vxw_mutex_unlock(_name) do \
        { \
        if ((_name).sem == 0) \
            { \
            vxw_mutex_init(_name); \
            } \
        semGive ((_name).sem); \
	} while (0)

#ifdef __cplusplus
extern "C++" {
#endif

struct vxw_mutex_t
    {
    SEM_ID sem;
#ifdef __cplusplus
    vxw_mutex_t () : sem (0) {/* NO-OP for now*/}
    ~vxw_mutex_t () {vxw_mutex_fini(*this);}

    void lock () {vxw_mutex_lock(*this);}
    void unlock () {vxw_mutex_unlock(*this);}

#endif
    };


#ifdef __cplusplus
}
}
#endif


#endif /* __INCvxwMutexh */
