/* -*- C++ -*- */
/* Here are all the declarations that are needed throughout the Reactor library. */

#if !defined (ACE_SYSINCLUDES_H)
#define ACE_SYSINCLUDES_H

// This file should be a link to the platform/compiler-specific 
// configuration file (e.g., config-sunos5-sunc++-4.x.h).
#include "config.h"



/* Sergey: on SunOS 'thread_key_t' defined in thread.h */
#ifdef SunOS
#include <thread.h>
#endif
#ifdef Linux
#include <thread_db.h>
#endif



#if defined (ACE_MT_SAFE)

#if !defined (_REENTRANT)
#define _REENTRANT
#endif /* _REENTRANT */

#define MT(X) X
#else
#define MT(X) 
#endif /* ACE_MT_SAFE */

#if defined (NDEBUG)
#define DB(X)
#else
#define DB(X) X
#endif /* NDEBUG */

#if !defined (ACE_HAS_THREAD_T) 
//typedef int thread_t;     E.Wolin, D.Abbott, 14-may-01
#endif /* !ACE_HAS_THREAD_T */

// The following is necessary since many cfront-based C++ compilers
// don't support typedef'd types inside of classes used as formal
// template arguments... ;-(.  Luckily, using the C++ preprocessor
// I can hide most of this nastiness!

#if defined (ACE_HAS_TEMPLATE_TYPEDEFS)
#define ACE_SYNCH_1 class SYNCH
#define ACE_SYNCH_2 SYNCH
#define S_MUTEX SYNCH::MUTEX
#define S_CONDITION SYNCH::CONDITION
#else /* TEMPLATES are broken (must be a cfront-based compiler...) */
#define ACE_SYNCH_1 class SYNCH_MUTEX, class SYNCH_CONDITION
#define ACE_SYNCH_2 SYNCH_MUTEX, SYNCH_CONDITION
#define S_MUTEX SYNCH_MUTEX
#define S_CONDITION SYNCH_CONDITION
#endif /* ACE_TEMPLATE_TYPEDEFS */

#if !defined (ACE_HAS_SYS_SIGLIST)
#if !defined (_sys_siglist)
#define _sys_siglist sis_siglist
#endif /* !defined (sys_siglist) */
extern char **_sys_siglist;
#endif /* !ACE_HAS_SYS_SIGLIST */

#if !defined (ACE_HAS_STRERROR)
#define strerror(err) sys_errlist[err]
#endif /* !ACE_HAS_STERROR */

#if !defined (ACE_HAS_TLI_PROTOTYPES)
#include "tli.h"
#endif /* !ACE_HAS_TLI_PROTOTYPES */

#if !defined (ACE_HAS_SIG_ATOMIC_T)
typedef int sig_atomic_t;
#endif /* !ACE_HAS_SIG_ATOMIC_T */

#if !defined (ACE_HAS_SSIZE_T)
typedef int ssize_t;
#endif /* ACE_HAS_SSIZE_T */

#if !defined (ACE_HAS_CPLUSPLUS_HEADERS)
#include <libc.h>
#include <osfcn.h>
#endif /* ACE_HAS_CPLUSPLUS_HEADERS */

#if !defined (ACE_HAS_RTLD_LAZY_V)
#define RTLD_LAZY 1
#endif /* !ACE_HAS_RTLD_LAZY_V */

#if defined (ACE_HAS_PTHREADS)
#include <pthread.h>
#include "pthreads_map.h"
#endif /* ACE_HAS_PTHREADS */

#if defined (ACE_HAS_THREADS) && !defined (ACE_MT_SAFE) || !defined (ACE_HAS_THREADS)
//#define thr_self() 1     E.Wolin, D.Abbott, 14-may-01
#endif 

#if defined (ACE_HAS_SYSENT_H)
#include <sysent.h>
#endif /* ACE_HAS_SYSENT_H_*/

#if defined (ACE_HAS_SIGINFO_T)
#include <siginfo.h>
#include <ucontext.h>
#endif /* ACE_HAS_SIGINFO_T */

#if defined (ACE_HAS_POLL)
#include <poll.h>
#endif /* ACE_HAS_POLL */

#if defined (ACE_HAS_SVR4_POLL)
#define ACE_USE_POLL_IMPLEMENTATION
#endif /* ACE_HAS_SVR4_POLL */

#if defined (ACE_HAS_TIUSER_H)
#include <tiuser.h> 
#endif /* ACE_HAS_TIUSER_H */

#if defined (ACE_HAS_NO_T_ERRNO)
extern int t_errno;
#endif /* ACE_HAS_NO_T_ERRNO */

#if defined (ACE_HAS_ALLOCA)
#include <alloca.h>
#endif /* ACE_HAS_ALLOCA */

#if defined (ACE_HAS_STREAMS)
#include <stropts.h>
#endif /* ACE_HAS_STREAMS */

#if defined (ACE_HAS_SVR4_DYNAMIC_LINKING)
#include <dlfcn.h>
#endif /* ACE_HAS_SVR4_DYNAMIC_LINKING */

#if defined (ACE_HAS_STHREADS)
#include <synch.h>
#include <thread.h>
#endif /* ACE_HAS_STHREADS */

#if defined (ACE_HAS_TIMOD_H)
#include <sys/timod.h>
#elif defined (ACE_HAS_OSF_TIMOD_H)
#include <tli/timod.h>
#endif /* ACE_HAS_TIMOD_H */

#if defined (ACE_HAS_PROC_FS)
#include <sys/procfs.h>
#endif /* ACE_HAS_PROC_FD */

#if defined (ACE_HAS_SYS_FILIO_H)
#include <sys/filio.h>
#endif /* ACE_HAS_SYS_FILIO_H */

#if defined (ACE_HAS_POSIX_NONBLOCK)
#define ACE_NONBLOCK O_NONBLOCK
#else
#define ACE_NONBLOCK O_NDELAY
#endif /* ACE_HAS_POSIX_NONBLOCK */

#if defined (ACE_HAS_SVR4_GETTIMEOFDAY)
extern "C" int gettimeofday (struct timeval *tp, void * = 0);
#endif /* ACE_HAS_SVR4_GETTIMEOFDAY */

#if defined (ACE_HAS_CONSISTENT_SIGNAL_PROTOTYPES)
/* Prototypes for both signal() and struct sigaction are consistent. */
typedef void (*SignalHandler)(int);
typedef void (*SignalHandlerV)(int);
#elif defined (ACE_HAS_SVR4_SIGNAL_T) 
/* SVR4 Signals are inconsistent (e.g., see struct sigaction). */
typedef void (*SignalHandler)(int);
typedef void (*SignalHandlerV)(void);
#else /* This is necessary for some older broken version of cfront */
#define SignalHandler SIG_PF
typedef void (*SignalHandlerV)(...);
#endif /* ACE_HAS_CONSISTENT_SIGNAL_PROTOTYPES */

/* These prototypes are chronically lacking from many versions of UNIX. */
extern "C" int t_getname (int, struct netbuf *, int);
extern "C" int isastream (int);

/* A couple useful inline functions for checking whether bits are
   enabled or disabled. */

inline int  BIT_ENABLED (unsigned long word, int bit = 1) { return (word & bit) != 0; }
inline int  BIT_DISABLED (unsigned long word, int bit = 1) { return (word & bit) == 0; }
inline void SET_BITS (unsigned long &word, int bits) { word |= bits; }
inline void CLR_BITS (unsigned long &word, int bits) { word &= ~bits; }
inline void SET_BITS (short &word, int bits) { word |= bits; }
inline void CLR_BITS (short &word, int bits) { word &= ~bits; }

/* I don't like putting all the system headers in one place, but this
   gets really messy if it is spread all throughout the program... */

#if defined (ACE_HAS_SOCKIO_H)
#include <sys/sockio.h>
#endif 
#include <stddef.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/param.h>
#include <sys/file.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/uio.h>
#include <sys/mman.h>
#include <sys/un.h>
#include <sys/resource.h>
#include <sys/ipc.h>
#include <sys/sem.h>

// OSF1 has problems with sys/msg.h and C++...
#if !defined (__osf__)
#include <sys/msg.h>
#else
#include "msg_hack.h"
#endif /* !defined __osf__ */

#include <sys/wait.h>
#include <rpc/rpc.h>
#include <limits.h>
#include <sys/shm.h>
#include <sys/utsname.h>
#include <net/if.h>
#include <netinet/in.h>
#include <stdio.h>
#include <signal.h>

#ifdef Darwin
#include <sys/malloc.h>
#else
#include <malloc.h>
#endif

#include <string.h>
#include <iostream.h>
#include <assert.h>
#include <memory.h>
#include <stdarg.h>
#include <ctype.h>
#include <fcntl.h>
#include <errno.h>
#include <stdlib.h>
#include <pwd.h>
#include <dirent.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>

#if defined (IRIX4)
extern "C"
{
  int writev(int d, struct iovec *iov, int iovcnt);
  int readv(int d, struct iovec *iov, int iovcnt);
}
#endif /* IRIX4 */

#if 0
extern "C"
{
  int madvise (const caddr_t addr, size_t len, int advice);
  int poll (struct pollfd *, unsigned long, int);
  int sigfillset(sigset_t *set);
  int sigemptyset(sigset_t *set);
#if defined (__GNUG__)
  int sigaction(int sig, const struct sigaction *act, struct sigaction *oact);
  int sigprocmask(int how, const sigset_t *set, sigset_t *oset);
#else
  int sigaction(int sig, struct sigaction *act, struct sigaction *oact);
  int sigprocmask(int how, sigset_t *set, sigset_t *oset);
  int mkfifo(char *path, unsigned short mode);
#endif /* __GNUG__ */
}

extern int t_errno;
#define LOCALNAME 0
#define REMOTENAME 1
#include <mon/openprom.h>
#include <mon/sunromvec.h>
#endif /* SunOS 4 */

#if defined (__GNUG__)
#if defined (ultrix)
extern "C"{
int  ioctl (int d, int request, void *argp);
void bzero (char *b1, int length);
int  writev (int fd, struct iovec *iov, int ioveclen);
int  readv (int fd, struct iovec *iov, int ioveclen);
int  recv (int s, char *buf, int len, int flags);
int  recvfrom (int s, char *buf, int len, int flags, struct sockaddr *from,
	       int *fromlen);
int  send (int s, const char *msg, int len, int flags);
int  sendto (int s, const char *msg, int len, int flag, struct sockaddr *to, 
	     int tolen);
int  sendmsg (int s, struct msghdr msg[], int flags);
int  recvmsg (int s, struct msghdr msg[], int flags);
int  setsockopt (int s, int level, int optname, char *optval, int optlen);
int  getsockopt (int s, int level, int optname, char *optval, int *optlen);
int  getsockname (int s, struct sockaddr *name, int *namelen);
int  shutdown (int s, int how);
caddr_t mmap (caddr_t addr, size_t len, int prot, int flags, int fd, 
	      off_t off);
caddr_t munmap (caddr_t addr, size_t len);
int     ftruncate (int fd, int length);
int     madvise (caddr_t addr, size_t len, int behaviour);
char    *sbrk (int incr);
int     getrlimit (int resource, struct rlimit *rlp);
int     setrlimit (int resource, struct rlimit *rlp);
int     select (int nfds, fd_set *, fd_set *, fd_set *, struct timeval *);
int     getpeername (int s, struct sockaddr *name, int *namelen);
int     connect (int s, struct sockaddr *name, int namelen);
int     bind (int s, struct sockaddr *name, int namelen);
char    *strdup (char *s);
int     socket (int af, int type, int protocol);
int     accept (int s, struct sockaddr *addr, int *addrlen);
int     listen (int s, int backlog);
};
#endif
#endif /* __GNUG__ */

/* Create some useful typedefs */
typedef void *(*THR_FUNC)(void *);
typedef const char **SYS_SIGLIST;

/* Provide compatibility with Windows NT. */
typedef int HANDLE;

/* Miscellaneous C functions from $WRAPPER_ROOT/libsrc/Misc/Misc.C */
#include "Misc.h"

#endif /* _SYSINCLUDES_H */
