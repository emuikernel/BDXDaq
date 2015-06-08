/* symTbl.c - standalone symbol tables wrapper */

/* CREATED BY /vobs/wpwr/host/src/hutils/makeSymTbl.tcl
 *  WITH ARGS simso tmp.o symTbl.c
 *         ON Tue Oct 01 00:14:46 PDT 2002
 */

#include "vxWorks.h"
#include "symbol.h"

/* VxSim: undefine READ and WRITE which are macros from VxWorks.h */

#undef READ
#undef WRITE

IMPORT int ABORT ();
IMPORT int ACCESS ();
IMPORT int ALARM ();
IMPORT int ALTZONE ();
IMPORT int ASCTIME ();
IMPORT int ASCTIME_R ();
IMPORT int ATEXIT ();
IMPORT int ATOI ();
IMPORT int BCOPY ();
IMPORT int BRK ();
IMPORT int BZERO ();
IMPORT int CALLOC ();
IMPORT int CHARMAP;
IMPORT int CLOSE ();
IMPORT int CLOSEDIR ();
IMPORT int CLOSELOG ();
IMPORT int CLOSE_FD ();
IMPORT int COND_BROADCAST ();
IMPORT int COND_DESTROY ();
IMPORT int COND_INIT ();
IMPORT int COND_SIGNAL ();
IMPORT int COND_TIMEDWAIT ();
IMPORT int COND_WAIT ();
IMPORT int CREAT ();
IMPORT int CTIME ();
IMPORT int CTIME_R ();
IMPORT int DAYLIGHT ();
IMPORT int DECIMAL_TO_DOUBLE ();
IMPORT int DECIMAL_TO_EXTENDED ();
IMPORT int DECIMAL_TO_QUADRUPLE ();
IMPORT int DECIMAL_TO_SINGLE ();
IMPORT int DIFFTIME ();
IMPORT int DOUBLE_TO_DECIMAL ();
IMPORT int DUP2 ();
IMPORT int ECONVERT ();
IMPORT int ENVIRON ();
IMPORT int ERRNO;
IMPORT int EXECL ();
IMPORT int EXECLP ();
IMPORT int EXECV ();
IMPORT int EXECVE ();
IMPORT int EXECVP ();
IMPORT int EXIT ();
IMPORT int FCHMOD ();
IMPORT int FCLOSE ();
IMPORT int FCNTL ();
IMPORT int FCONVERT ();
IMPORT int FDOPEN ();
IMPORT int FEOF ();
IMPORT int FEOF_UNLOCKED ();
IMPORT int FERROR ();
IMPORT int FERROR_UNLOCKED ();
IMPORT int FFLUSH ();
IMPORT int FFS ();
IMPORT int FGETS ();
IMPORT int FILENO ();
IMPORT int FILENO_UNLOCKED ();
IMPORT int FILE_TO_DECIMAL ();
IMPORT int FLOCKFILE ();
IMPORT int FOPEN ();
IMPORT int FORK ();
IMPORT int FORK1 ();
IMPORT int FPGETMASK ();
IMPORT int FPRINTF ();
IMPORT int FPUTS ();
IMPORT int FREAD ();
IMPORT int FREE ();
IMPORT int FREOPEN ();
IMPORT int FSCANF ();
IMPORT int FSEEK ();
IMPORT int FSTAT ();
IMPORT int FSYNC ();
IMPORT int FTELL ();
IMPORT int FTRYLOCKFILE ();
IMPORT int FUNLOCKFILE ();
IMPORT int FWRITE ();
IMPORT int GETC ();
IMPORT int GETCWD ();
IMPORT int GETC_UNLOCKED ();
IMPORT int GETDENTS ();
IMPORT int GETEGID ();
IMPORT int GETENV ();
IMPORT int GETEUID ();
IMPORT int GETGID ();
IMPORT int GETGROUPS ();
IMPORT int GETMNTANY ();
IMPORT int GETMNTENT ();
IMPORT int GETMSG ();
IMPORT int GETPGID ();
IMPORT int GETPGRP ();
IMPORT int GETPID ();
IMPORT int GETPPID ();
IMPORT int GETRLIMIT ();
IMPORT int GETSID ();
IMPORT int GETTIMEOFDAY ();
IMPORT int GETUID ();
IMPORT int GETWIDTH ();
IMPORT int GMTIME ();
IMPORT int GMTIME_R ();
IMPORT int HASMNTOPT ();
IMPORT int IEEE_HANDLERS;
IMPORT int IOCTL ();
IMPORT int ISALNUM ();
IMPORT int ISALPHA ();
IMPORT int ISASCII ();
IMPORT int ISATTY ();
IMPORT int ISCNTRL ();
IMPORT int ISDIGIT ();
IMPORT int ISGRAPH ();
IMPORT int ISLOWER ();
IMPORT int ISPRINT ();
IMPORT int ISPUNCT ();
IMPORT int ISSPACE ();
IMPORT int ISUPPER ();
IMPORT int ISXDIGIT ();
IMPORT int KILL ();
IMPORT int LOCALECONV ();
IMPORT int LOCALTIME ();
IMPORT int LOCALTIME_R ();
IMPORT int LOCKF ();
IMPORT int LONGJMP ();
IMPORT int LSEEK ();
IMPORT int LSTAT ();
IMPORT int MAKECONTEXT ();
IMPORT int MALLOC ();
IMPORT int MBLEN ();
IMPORT int MBSTOWCS ();
IMPORT int MBTOWC ();
IMPORT int MEMCCPY ();
IMPORT int MEMCHR ();
IMPORT int MEMCMP ();
IMPORT int MEMCNTL ();
IMPORT int MEMCPY ();
IMPORT int MEMMOVE ();
IMPORT int MEMSET ();
IMPORT int MKDIR ();
IMPORT int MKFIFO ();
IMPORT int MKNOD ();
IMPORT int MKTEMP ();
IMPORT int MKTIME ();
IMPORT int MMAP ();
IMPORT int MNTOPT ();
IMPORT int MSYNC ();
IMPORT int MUNMAP ();
IMPORT int MUTEX_DESTROY ();
IMPORT int MUTEX_HELD ();
IMPORT int MUTEX_INIT ();
IMPORT int MUTEX_LOCK ();
IMPORT int MUTEX_TRYLOCK ();
IMPORT int MUTEX_UNLOCK ();
IMPORT int NSS_DEFAULT_FINDERS ();
IMPORT int NSS_DELETE ();
IMPORT int NSS_ENDENT ();
IMPORT int NSS_GETENT ();
IMPORT int NSS_SEARCH ();
IMPORT int NSS_SETENT ();
IMPORT int OPEN ();
IMPORT int OPENDIR ();
IMPORT int OPENLOG ();
IMPORT int PATHCONF ();
IMPORT int PAUSE ();
IMPORT int PERROR ();
IMPORT int PIPE ();
IMPORT int POLL ();
IMPORT int PRINTF ();
IMPORT int PTHREAD_CONDATTR_DESTROY ();
IMPORT int PTHREAD_CONDATTR_GETPSHARED ();
IMPORT int PTHREAD_CONDATTR_INIT ();
IMPORT int PTHREAD_CONDATTR_SETPSHARED ();
IMPORT int PTHREAD_COND_BROADCAST ();
IMPORT int PTHREAD_COND_DESTROY ();
IMPORT int PTHREAD_COND_INIT ();
IMPORT int PTHREAD_COND_SIGNAL ();
IMPORT int PTHREAD_COND_TIMEDWAIT ();
IMPORT int PTHREAD_COND_WAIT ();
IMPORT int PTHREAD_MUTEXATTR_DESTROY ();
IMPORT int PTHREAD_MUTEXATTR_GETPRIOCEILING ();
IMPORT int PTHREAD_MUTEXATTR_GETPROTOCOL ();
IMPORT int PTHREAD_MUTEXATTR_GETPSHARED ();
IMPORT int PTHREAD_MUTEXATTR_INIT ();
IMPORT int PTHREAD_MUTEXATTR_SETPRIOCEILING ();
IMPORT int PTHREAD_MUTEXATTR_SETPROTOCOL ();
IMPORT int PTHREAD_MUTEXATTR_SETPSHARED ();
IMPORT int PTHREAD_MUTEX_DESTROY ();
IMPORT int PTHREAD_MUTEX_GETPRIOCEILING ();
IMPORT int PTHREAD_MUTEX_INIT ();
IMPORT int PTHREAD_MUTEX_LOCK ();
IMPORT int PTHREAD_MUTEX_SETPRIOCEILING ();
IMPORT int PTHREAD_MUTEX_TRYLOCK ();
IMPORT int PTHREAD_MUTEX_UNLOCK ();
IMPORT int PUTC ();
IMPORT int PUTC_UNLOCKED ();
IMPORT int PUTMSG ();
IMPORT int PUTPMSG ();
IMPORT int QECONVERT ();
IMPORT int QFCONVERT ();
IMPORT int QGCONVERT ();
IMPORT int QSORT ();
IMPORT int QUADRUPLE_TO_DECIMAL ();
IMPORT int RAISE ();
IMPORT int READ ();
IMPORT int READDIR ();
IMPORT int READV ();
IMPORT int REALLOC ();
IMPORT int REMOVE ();
IMPORT int RENAME ();
IMPORT int REWIND ();
IMPORT int RMDIR ();
IMPORT int RWLOCK_INIT ();
IMPORT int RW_RDLOCK ();
IMPORT int RW_READ_HELD ();
IMPORT int RW_TRYRDLOCK ();
IMPORT int RW_TRYWRLOCK ();
IMPORT int RW_UNLOCK ();
IMPORT int RW_WRITE_HELD ();
IMPORT int RW_WRLOCK ();
IMPORT int SBRK ();
IMPORT int SCANF ();
IMPORT int SCRWIDTH ();
IMPORT int SELECT ();
IMPORT int SEMA_HELD ();
IMPORT int SEMA_INIT ();
IMPORT int SEMA_POST ();
IMPORT int SEMA_TRYWAIT ();
IMPORT int SEMA_WAIT ();
IMPORT int SEMCTL ();
IMPORT int SEMGET ();
IMPORT int SEMOP ();
IMPORT int SETCONTEXT ();
IMPORT int SETITIMER ();
IMPORT int SETJMP ();
IMPORT int SETLOCALE ();
IMPORT int SETLOGMASK ();
IMPORT int SETPGID ();
IMPORT int SETPGRP ();
IMPORT int SETSID ();
IMPORT int SHMAT ();
IMPORT int SHMCTL ();
IMPORT int SHMDT ();
IMPORT int SHMGET ();
IMPORT int SIGACTION ();
IMPORT int SIGADDSET ();
IMPORT int SIGALTSTACK ();
IMPORT int SIGDELSET ();
IMPORT int SIGEMPTYSET ();
IMPORT int SIGFILLSET ();
IMPORT int SIGHOLD ();
IMPORT int SIGIGNORE ();
IMPORT int SIGISMEMBER ();
IMPORT int SIGLONGJMP ();
IMPORT int SIGNAL ();
IMPORT int SIGPAUSE ();
IMPORT int SIGPENDING ();
IMPORT int SIGPROCMASK ();
IMPORT int SIGRELSE ();
IMPORT int SIGSET ();
IMPORT int SIGSETJMP ();
IMPORT int SIGSUSPEND ();
IMPORT int SIGWAIT ();
IMPORT int SLEEP ();
IMPORT int SPRINTF ();
IMPORT int SSCANF ();
IMPORT int STAT ();
IMPORT int STRCASECMP ();
IMPORT int STRCAT ();
IMPORT int STRCHR ();
IMPORT int STRCMP ();
IMPORT int STRCPY ();
IMPORT int STRCSPN ();
IMPORT int STRDUP ();
IMPORT int STRERROR ();
IMPORT int STRLEN ();
IMPORT int STRNCASECMP ();
IMPORT int STRNCAT ();
IMPORT int STRNCMP ();
IMPORT int STRNCPY ();
IMPORT int STRPBRK ();
IMPORT int STRRCHR ();
IMPORT int STRSPN ();
IMPORT int STRSTR ();
IMPORT int STRTOK ();
IMPORT int STRTOK_R ();
IMPORT int STRTOL ();
IMPORT int SYSCALL ();
IMPORT int SYSCONF ();
IMPORT int SYSINFO ();
IMPORT int SYSLOG ();
IMPORT int SYSTEM ();
IMPORT int SYS_ERRLIST ();
IMPORT int SYS_NERR ();
IMPORT int TCDRAIN ();
IMPORT int TCGETPGRP ();
IMPORT int TCGETSID ();
IMPORT int TEMPNAM ();
IMPORT int THR_CONTINUE ();
IMPORT int THR_CREATE ();
IMPORT int THR_ERRNOP ();
IMPORT int THR_EXIT ();
IMPORT int THR_GETCONCURRENCY ();
IMPORT int THR_GETPRIO ();
IMPORT int THR_GETSPECIFIC ();
IMPORT int THR_JOIN ();
IMPORT int THR_KEYCREATE ();
IMPORT int THR_KILL ();
IMPORT int THR_MAIN ();
IMPORT int THR_MIN_STACK ();
IMPORT int THR_SELF ();
IMPORT int THR_SETCONCURRENCY ();
IMPORT int THR_SETPRIO ();
IMPORT int THR_SETSPECIFIC ();
IMPORT int THR_SIGSETMASK ();
IMPORT int THR_STKSEGMENT ();
IMPORT int THR_SUSPEND ();
IMPORT int THR_YIELD ();
IMPORT int TIME ();
IMPORT int TIMEZONE ();
IMPORT int TOASCII ();
IMPORT int TOLOWER ();
IMPORT int TZNAME ();
IMPORT int TZSET ();
IMPORT int ULIMIT ();
IMPORT int UNAME ();
IMPORT int UNGETC ();
IMPORT int UNLINK ();
IMPORT int USLEEP ();
IMPORT int VFORK ();
IMPORT int VSYSLOG ();
IMPORT int WAIT ();
IMPORT int WAITID ();
IMPORT int WAITPID ();
IMPORT int WCSTOMBS ();
IMPORT int WCTOMB ();
IMPORT int WRITE ();
IMPORT int WRITEV ();
IMPORT int _ACCESS ();
IMPORT int _ALARM ();
IMPORT int _ALTZONE;
IMPORT int _ASCTIME_R ();
IMPORT int _ASSERT ();
IMPORT int _BRK ();
IMPORT int _BRK_UNLOCKED ();
IMPORT int _BUFENDTAB;
IMPORT int _BUFSYNC ();
IMPORT int _CERROR ();
IMPORT int _CERROR64 ();
IMPORT int _CHECK_THREADED ();
IMPORT int _CLEANUP ();
IMPORT int _CLOSE ();
IMPORT int _CLOSEDIR ();
IMPORT int _CLOSELOG ();
IMPORT int _COND_BROADCAST ();
IMPORT int _COND_DESTROY ();
IMPORT int _COND_INIT ();
IMPORT int _COND_SIGNAL ();
IMPORT int _COND_TIMEDWAIT ();
IMPORT int _COND_WAIT ();
IMPORT int _CREAT ();
IMPORT int _CSWIDTH;
IMPORT int _CTIME_R ();
IMPORT int _CTYPE ();
IMPORT int _CUR_LOCALE;
IMPORT int _DAYLIGHT;
IMPORT int _DECIMAL_TO_DOUBLE ();
IMPORT int _DECIMAL_TO_EXTENDED ();
IMPORT int _DECIMAL_TO_QUADRUPLE ();
IMPORT int _DECIMAL_TO_SINGLE ();
IMPORT int _DGETTEXT ();
IMPORT int _DIRENT_LOCK;
IMPORT int _DOPRNT ();
IMPORT int _DOSCAN ();
IMPORT int _DOUBLE_TO_DECIMAL ();
IMPORT int _DUP2 ();
IMPORT int _ECONVERT ();
IMPORT int _ENVIRON;
IMPORT int _EUCWIDTH;
IMPORT int _EXECL ();
IMPORT int _EXECLP ();
IMPORT int _EXECV ();
IMPORT int _EXECVE ();
IMPORT int _EXECVP ();
IMPORT int _EXITHANDLE ();
IMPORT int _FCHMOD ();
IMPORT int _FCNTL ();
IMPORT int _FCONVERT ();
IMPORT int _FDOPEN ();
IMPORT int _FEOF_UNLOCKED ();
IMPORT int _FERROR_UNLOCKED ();
IMPORT int _FFLUSH_U ();
IMPORT int _FFS ();
IMPORT int _FILBUF ();
IMPORT int _FILENO ();
IMPORT int _FILENO_UNLOCKED ();
IMPORT int _FILE_TO_DECIMAL ();
IMPORT int _FINDBUF ();
IMPORT int _FINDIOP ();
IMPORT int _FLOCKFILE ();
IMPORT int _FLOCKGET ();
IMPORT int _FLOCKREL ();
IMPORT int _FLSBUF ();
IMPORT int _FLUSHLBF ();
IMPORT int _FORK ();
IMPORT int _FORK1 ();
IMPORT int _FPGETMASK ();
IMPORT int _FPRINTF ();
IMPORT int _FP_ADD ();
IMPORT int _FP_COMPARE ();
IMPORT int _FP_CURRENT_DIRECTION;
IMPORT int _FP_CURRENT_EXCEPTIONS;
IMPORT int _FP_CURRENT_PRECISION;
IMPORT int _FP_PACK ();
IMPORT int _FP_SUB ();
IMPORT int _FP_UNPACK ();
IMPORT int _FREE_UNLOCKED ();
IMPORT int _FSTAT ();
IMPORT int _FSYNC ();
IMPORT int _FTRYLOCKFILE ();
IMPORT int _FULLOCALE ();
IMPORT int _FUNLOCKFILE ();
IMPORT int _FWRITE_UNLOCKED ();
IMPORT int _GETARG ();
IMPORT int _GETCWD ();
IMPORT int _GETC_UNLOCKED ();
IMPORT int _GETDENTS ();
IMPORT int _GETEGID ();
IMPORT int _GETEUID ();
IMPORT int _GETGID ();
IMPORT int _GETGROUPS ();
IMPORT int _GETMNTANY ();
IMPORT int _GETMNTENT ();
IMPORT int _GETMSG ();
IMPORT int _GETPGID ();
IMPORT int _GETPGRP ();
IMPORT int _GETPID ();
IMPORT int _GETPPID ();
IMPORT int _GETRLIMIT ();
IMPORT int _GETSID ();
IMPORT int _GETSP ();
IMPORT int _GETTIMEOFDAY ();
IMPORT int _GETUID ();
IMPORT int _GMTIME_R ();
IMPORT int _HASMNTOPT ();
IMPORT int _IOB ();
IMPORT int _IOCTL ();
IMPORT int _ISASCII ();
IMPORT int _ISATTY ();
IMPORT int _KILL ();
IMPORT int _LASTBUF;
IMPORT int _LFLAG;
IMPORT int _LIBC_ALARM ();
IMPORT int _LIBC_CHILD_ATFORK ();
IMPORT int _LIBC_CLOSE ();
IMPORT int _LIBC_CREAT ();
IMPORT int _LIBC_FCNTL ();
IMPORT int _LIBC_FORK ();
IMPORT int _LIBC_FORK1 ();
IMPORT int _LIBC_FSYNC ();
IMPORT int _LIBC_KILL ();
IMPORT int _LIBC_MSYNC ();
IMPORT int _LIBC_NANOSLEEP ();
IMPORT int _LIBC_OPEN ();
IMPORT int _LIBC_PARENT_ATFORK ();
IMPORT int _LIBC_PAUSE ();
IMPORT int _LIBC_PREPARE_ATFORK ();
IMPORT int _LIBC_READ ();
IMPORT int _LIBC_SETITIMER ();
IMPORT int _LIBC_SET_THREADED ();
IMPORT int _LIBC_SIGACTION ();
IMPORT int _LIBC_SIGLONGJMP ();
IMPORT int _LIBC_SIGPENDING ();
IMPORT int _LIBC_SIGPROCMASK ();
IMPORT int _LIBC_SIGSUSPEND ();
IMPORT int _LIBC_SIGTIMEDWAIT ();
IMPORT int _LIBC_SIGWAIT ();
IMPORT int _LIBC_SLEEP ();
IMPORT int _LIBC_TCDRAIN ();
IMPORT int _LIBC_THREADS_INTERFACE ();
IMPORT int _LIBC_UNSET_THREADED ();
IMPORT int _LIBC_WAIT ();
IMPORT int _LIBC_WAITPID ();
IMPORT int _LIBC_WRITE ();
IMPORT int _LOCALECONV_NOLOCK ();
IMPORT int _LOCALE_LOCK;
IMPORT int _LOCALTIME_R ();
IMPORT int _LOCKF ();
IMPORT int _LOCKTAB;
IMPORT int _LOC_FILENAME;
IMPORT int _LSEEK ();
IMPORT int _LSTAT ();
IMPORT int _LTZSET ();
IMPORT int _MAKECONTEXT ();
IMPORT int _MBLEN ();
IMPORT int _MBSTOWCS ();
IMPORT int _MBTOWC ();
IMPORT int _MEMCCPY ();
IMPORT int _MEMCMP ();
IMPORT int _MEMCNTL ();
IMPORT int _MEMCPY ();
IMPORT int _MEMMOVE ();
IMPORT int _MEMSET ();
IMPORT int _MKARGLST ();
IMPORT int _MKDIR ();
IMPORT int _MKFIFO ();
IMPORT int _MKNOD ();
IMPORT int _MKTEMP ();
IMPORT int _MMAP ();
IMPORT int _MSYNC ();
IMPORT int _MUNMAP ();
IMPORT int _MUTEX_DESTROY ();
IMPORT int _MUTEX_HELD ();
IMPORT int _MUTEX_INIT ();
IMPORT int _MUTEX_TRYLOCK ();
IMPORT int _NATIVELOC ();
IMPORT int _NSS_DEFAULT_FINDERS;
IMPORT int _NSS_DELETE ();
IMPORT int _NSS_ENDENT ();
IMPORT int _NSS_GETENT ();
IMPORT int _NSS_NETDB_ALIASES ();
IMPORT int _NSS_SEARCH ();
IMPORT int _NSS_SETENT ();
IMPORT int _NSS_SRC_STATE_DESTR ();
IMPORT int _NSS_STATUS_VEC ();
IMPORT int _NSS_XBYY_BUF_ALLOC ();
IMPORT int _NSS_XBYY_BUF_FREE ();
IMPORT int _NSS_XBYY_FGETS ();
IMPORT int _NSW_GETONECONFIG ();
IMPORT int _NUMERIC;
IMPORT int _OPEN ();
IMPORT int _OPENDIR ();
IMPORT int _OPENLOG ();
IMPORT int _PATHCONF ();
IMPORT int _PAUSE ();
IMPORT int _PCMASK;
IMPORT int _PIPE ();
IMPORT int _POLL ();
IMPORT int _PTHREAD_CONDATTR_DESTROY ();
IMPORT int _PTHREAD_CONDATTR_GETPSHARED ();
IMPORT int _PTHREAD_CONDATTR_INIT ();
IMPORT int _PTHREAD_CONDATTR_SETPSHARED ();
IMPORT int _PTHREAD_COND_BROADCAST ();
IMPORT int _PTHREAD_COND_DESTROY ();
IMPORT int _PTHREAD_COND_INIT ();
IMPORT int _PTHREAD_COND_SIGNAL ();
IMPORT int _PTHREAD_COND_TIMEDWAIT ();
IMPORT int _PTHREAD_COND_WAIT ();
IMPORT int _PTHREAD_MUTEXATTR_DESTROY ();
IMPORT int _PTHREAD_MUTEXATTR_GETPRIOCEILING ();
IMPORT int _PTHREAD_MUTEXATTR_GETPROTOCOL ();
IMPORT int _PTHREAD_MUTEXATTR_GETPSHARED ();
IMPORT int _PTHREAD_MUTEXATTR_INIT ();
IMPORT int _PTHREAD_MUTEXATTR_SETPRIOCEILING ();
IMPORT int _PTHREAD_MUTEXATTR_SETPROTOCOL ();
IMPORT int _PTHREAD_MUTEXATTR_SETPSHARED ();
IMPORT int _PTHREAD_MUTEX_DESTROY ();
IMPORT int _PTHREAD_MUTEX_GETPRIOCEILING ();
IMPORT int _PTHREAD_MUTEX_INIT ();
IMPORT int _PTHREAD_MUTEX_LOCK ();
IMPORT int _PTHREAD_MUTEX_SETPRIOCEILING ();
IMPORT int _PTHREAD_MUTEX_TRYLOCK ();
IMPORT int _PTHREAD_MUTEX_UNLOCK ();
IMPORT int _PUTC_UNLOCKED ();
IMPORT int _PUTMSG ();
IMPORT int _PUTPMSG ();
IMPORT int _QECONVERT ();
IMPORT int _QFCONVERT ();
IMPORT int _QGCONVERT ();
IMPORT int _QGETRD ();
IMPORT int _QGETRP ();
IMPORT int _QSWAPRD ();
IMPORT int _QSWAPRP ();
IMPORT int _QUADRUPLE_TO_DECIMAL ();
IMPORT int _Q_QTOLL ();
IMPORT int _Q_QTOULL ();
IMPORT int _Q_add ();
IMPORT int _Q_cmp ();
IMPORT int _Q_cmpe ();
IMPORT int _Q_dtoq ();
IMPORT int _Q_feq ();
IMPORT int _Q_fge ();
IMPORT int _Q_fgt ();
IMPORT int _Q_fle ();
IMPORT int _Q_flt ();
IMPORT int _Q_fne ();
IMPORT int _Q_itoq ();
IMPORT int _Q_qtoi ();
IMPORT int _Q_qtou ();
IMPORT int _Q_set_exception ();
IMPORT int _Q_sub ();
IMPORT int _READ ();
IMPORT int _READDIR ();
IMPORT int _READV ();
IMPORT int _REALBUFEND ();
IMPORT int _REALLOCK ();
IMPORT int _RENAME ();
IMPORT int _REWIND_UNLOCKED ();
IMPORT int _RMDIR ();
IMPORT int _RMUTEX_TRYLOCK ();
IMPORT int _RWLOCK_INIT ();
IMPORT int _RW_RDLOCK ();
IMPORT int _RW_READ_HELD ();
IMPORT int _RW_TRYRDLOCK ();
IMPORT int _RW_TRYWRLOCK ();
IMPORT int _RW_UNLOCK ();
IMPORT int _RW_WRITE_HELD ();
IMPORT int _RW_WRLOCK ();
IMPORT int _Randseed;
IMPORT int _SBRK ();
IMPORT int _SBRK_UNLOCKED ();
IMPORT int _SELECT ();
IMPORT int _SEMA_HELD ();
IMPORT int _SEMA_INIT ();
IMPORT int _SEMA_POST ();
IMPORT int _SEMA_TRYWAIT ();
IMPORT int _SEMA_WAIT ();
IMPORT int _SEMCTL ();
IMPORT int _SEMGET ();
IMPORT int _SEMOP ();
IMPORT int _SETBUFEND ();
IMPORT int _SETCONTEXT ();
IMPORT int _SETITIMER ();
IMPORT int _SETLOCALE ();
IMPORT int _SETLOGMASK ();
IMPORT int _SETPGID ();
IMPORT int _SETPGRP ();
IMPORT int _SETSID ();
IMPORT int _SET_TAB ();
IMPORT int _SHMAT ();
IMPORT int _SHMCTL ();
IMPORT int _SHMDT ();
IMPORT int _SHMGET ();
IMPORT int _SIBUF;
IMPORT int _SIGACTION ();
IMPORT int _SIGADDSET ();
IMPORT int _SIGALTSTACK ();
IMPORT int _SIGDELSET ();
IMPORT int _SIGEMPTYSET ();
IMPORT int _SIGFILLSET ();
IMPORT int _SIGHOLD ();
IMPORT int _SIGIGNORE ();
IMPORT int _SIGISMEMBER ();
IMPORT int _SIGLONGJMP ();
IMPORT int _SIGNAL ();
IMPORT int _SIGPAUSE ();
IMPORT int _SIGPENDING ();
IMPORT int _SIGPROCMASK ();
IMPORT int _SIGRELSE ();
IMPORT int _SIGSET ();
IMPORT int _SIGSETJMP ();
IMPORT int _SIGSUSPEND ();
IMPORT int _SIGUHANDLER;
IMPORT int _SIGWAIT ();
IMPORT int _SIMPLE_SCRWIDTH ();
IMPORT int _SLEEP ();
IMPORT int _SMBUF;
IMPORT int _SOBUF;
IMPORT int _SPLIT_DOUBLE_19 ();
IMPORT int _SPLIT_DOUBLE_3 ();
IMPORT int _SPLIT_DOUBLE_35 ();
IMPORT int _SPLIT_DOUBLE_51 ();
IMPORT int _SPLIT_DOUBLE_52 ();
IMPORT int _SPLIT_DOUBLE_M1 ();
IMPORT int _SPLIT_SHORTEN ();
IMPORT int _STAT ();
IMPORT int _STRDUP ();
IMPORT int _STRERROR ();
IMPORT int _STRTOK_R ();
IMPORT int _SYSCALL ();
IMPORT int _SYSCONF ();
IMPORT int _SYSCONFIG ();
IMPORT int _SYSINFO ();
IMPORT int _SYSLOG ();
IMPORT int _SYS_ERRLIST;
IMPORT int _SYS_ERRS;
IMPORT int _SYS_INDEX;
IMPORT int _SYS_NERR;
IMPORT int _SYS_NUM_ERR;
IMPORT int _TCDRAIN ();
IMPORT int _TCGETPGRP ();
IMPORT int _TCGETSID ();
IMPORT int _TEMPNAM ();
IMPORT int _THR_CONTINUE ();
IMPORT int _THR_CREATE ();
IMPORT int _THR_ERRNOP ();
IMPORT int _THR_EXIT ();
IMPORT int _THR_GETCONCURRENCY ();
IMPORT int _THR_GETPRIO ();
IMPORT int _THR_GETSPECIFIC ();
IMPORT int _THR_GET_DIRECTION ();
IMPORT int _THR_GET_EXCEPTIONS ();
IMPORT int _THR_GET_INF_READ ();
IMPORT int _THR_GET_INF_WRITTEN ();
IMPORT int _THR_GET_NAN_READ ();
IMPORT int _THR_GET_NAN_WRITTEN ();
IMPORT int _THR_GET_PRECISION ();
IMPORT int _THR_JOIN ();
IMPORT int _THR_KEYCREATE ();
IMPORT int _THR_KILL ();
IMPORT int _THR_LIBTHREAD ();
IMPORT int _THR_MAIN ();
IMPORT int _THR_MIN_STACK ();
IMPORT int _THR_SELF ();
IMPORT int _THR_SETCONCURRENCY ();
IMPORT int _THR_SETPRIO ();
IMPORT int _THR_SETSPECIFIC ();
IMPORT int _THR_SIGSETMASK ();
IMPORT int _THR_STKSEGMENT ();
IMPORT int _THR_SUSPEND ();
IMPORT int _THR_YIELD ();
IMPORT int _TIME ();
IMPORT int _TIMEZONE;
IMPORT int _TIME_LOCK;
IMPORT int _TOASCII ();
IMPORT int _TOLOWER ();
IMPORT int _TOUPPER ();
IMPORT int _TSDALLOC ();
IMPORT int _TZNAME;
IMPORT int _TZSET ();
IMPORT int _TZ_GMT;
IMPORT int _TZ_SPACES;
IMPORT int _ULIMIT ();
IMPORT int _UNAME ();
IMPORT int _UNGETC_UNLOCKED ();
IMPORT int _UNLINK ();
IMPORT int _UNPACKED_TO_DECIMAL_TWO ();
IMPORT int _VFORK ();
IMPORT int _VSYSLOG ();
IMPORT int _WAIT ();
IMPORT int _WAITID ();
IMPORT int _WAITPID ();
IMPORT int _WCPTR;
IMPORT int _WCSTOMBS ();
IMPORT int _WCTOMB ();
IMPORT int _WRITE ();
IMPORT int _WRITEV ();
IMPORT int _WRTCHK ();
IMPORT int _XFLSBUF ();
IMPORT int _XGETWIDTH ();
IMPORT int __ARINT_SET_N ();
IMPORT int __ASSERT ();
IMPORT int __BASE_CONVERSION_ABORT ();
IMPORT int __BASE_CONVERSION_SET_EXCEPTION ();
IMPORT int __BASE_CONVERSION_WRITE_ONLY_DOUBLE;
IMPORT int __BIG_BINARY_TO_BIG_DECIMAL ();
IMPORT int __BIG_BINARY_TO_UNPACKED ();
IMPORT int __BIG_DECIMAL_TO_BIG_BINARY ();
IMPORT int __BIG_FLOAT_TIMES_POWER ();
IMPORT int __BINARY_TO_DECIMAL_FRACTION ();
IMPORT int __BINARY_TO_DECIMAL_INTEGER ();
IMPORT int __BUILTIN_ALLOCA ();
IMPORT int __CARRY_IN_B10000 ();
IMPORT int __CARRY_PROPAGATE_TEN ();
IMPORT int __CARRY_PROPAGATE_TWO ();
IMPORT int __CLASS_DOUBLE ();
IMPORT int __CLASS_EXTENDED ();
IMPORT int __CLASS_QUADRUPLE ();
IMPORT int __CLASS_SINGLE ();
IMPORT int __CLOCK_GETRES ();
IMPORT int __CLOCK_GETTIME ();
IMPORT int __CLOCK_SETTIME ();
IMPORT int __COPY_BIG_FLOAT_DIGITS ();
IMPORT int __CTYPE;
IMPORT int __DAYTAB;
IMPORT int __DECIMAL_ROUND ();
IMPORT int __DECIMAL_TO_BINARY_FRACTION ();
IMPORT int __DECIMAL_TO_BINARY_INTEGER ();
IMPORT int __DECIMAL_TO_UNPACKED ();
IMPORT int __DIGITS_TO_DOUBLE ();
IMPORT int __DIV64 ();
IMPORT int __DOSCAN_U ();
IMPORT int __DOUBLE_TO_DIGITS ();
IMPORT int __ENVIRON_LOCK;
IMPORT int __FDSYNC ();
IMPORT int __FILBUF ();
IMPORT int __FIRST_LINK;
IMPORT int __FLSBUF ();
IMPORT int __FOUR_DIGITS_QUICK ();
IMPORT int __FOUR_DIGITS_QUICK_TABLE;
IMPORT int __FPU_ADD3WC ();
IMPORT int __FPU_CMPLI ();
IMPORT int __FPU_ERROR_NAN ();
IMPORT int __FPU_NEG2WC ();
IMPORT int __FPU_NORMALIZE ();
IMPORT int __FPU_RIGHTSHIFT ();
IMPORT int __FPU_SET_EXCEPTION ();
IMPORT int __FPU_SUB3WC ();
IMPORT int __FP_LEFTSHIFT ();
IMPORT int __FP_NORMALIZE ();
IMPORT int __FP_RIGHTSHIFT ();
IMPORT int __FP_SET_EXCEPTION ();
IMPORT int __FRACTIONSTRING_TO_BIG_DECIMAL ();
IMPORT int __FREE_BIG_FLOAT ();
IMPORT int __GETCONTEXT ();
IMPORT int __GET_IEEE_FLAGS ();
IMPORT int __INFNANSTRING ();
IMPORT int __INF_READ;
IMPORT int __INF_WRITTEN;
IMPORT int __INRANGE_DOUBLE ();
IMPORT int __INRANGE_QUADEX ();
IMPORT int __INRANGE_SINGLE ();
IMPORT int __INTEGERSTRING_TO_BIG_DECIMAL ();
IMPORT int __IOB;
IMPORT int __K_DOUBLE_TO_DECIMAL ();
IMPORT int __K_GCONVERT ();
IMPORT int __K_QUADRUPLE_TO_DECIMAL ();
IMPORT int __LEFT_SHIFT_BASE_TEN ();
IMPORT int __LEFT_SHIFT_BASE_TWO ();
IMPORT int __LIBM_LIB_VERSION;
IMPORT int __LONGQUOREM10000 ();
IMPORT int __LSHIFT_B10000 ();
IMPORT int __LYDAY_TO_MONTH;
IMPORT int __MAJOR ();
IMPORT int __MAKEDEV ();
IMPORT int __MALLOC_LOCK;
IMPORT int __MINOR ();
IMPORT int __MONTH_SIZE;
IMPORT int __MON_LENGTHS;
IMPORT int __MT_SIGPENDING ();
IMPORT int __MUL64 ();
IMPORT int __MULTIPLY_BASE_TEN ();
IMPORT int __MULTIPLY_BASE_TEN_BY_TWO ();
IMPORT int __MULTIPLY_BASE_TEN_VECTOR ();
IMPORT int __MULTIPLY_BASE_TWO ();
IMPORT int __MULTIPLY_BASE_TWO_VECTOR ();
IMPORT int __MUL_10000SHORT ();
IMPORT int __MUL_65536SHORT ();
IMPORT int __NANOSLEEP ();
IMPORT int __NAN_READ;
IMPORT int __NAN_WRITTEN;
IMPORT int __NSW_EXTENDED_ACTION ();
IMPORT int __NSW_FREECONFIG ();
IMPORT int __NSW_GETCONFIG ();
IMPORT int __PACK_DOUBLE ();
IMPORT int __PACK_EXTENDED ();
IMPORT int __PACK_QUADRUPLE ();
IMPORT int __PACK_SINGLE ();
IMPORT int __POSIX_ASCTIME_R ();
IMPORT int __POSIX_CTIME_R ();
IMPORT int __PRODC_B10000 ();
IMPORT int __PROD_10000_B65536 ();
IMPORT int __QUOREM ();
IMPORT int __QUOREM10000 ();
IMPORT int __REM64 ();
IMPORT int __RIGHT_SHIFT_BASE_TWO ();
IMPORT int __SBRK_LOCK;
IMPORT int __SET_IEEE_FLAGS ();
IMPORT int __SIGACTION ();
IMPORT int __SIGFILLSET ();
IMPORT int __SIGQUEUE ();
IMPORT int __SIGTIMEDWAIT ();
IMPORT int __SNPRINTF ();
IMPORT int __TBL_10_BIG_DIGITS;
IMPORT int __TBL_10_BIG_START;
IMPORT int __TBL_10_HUGE_DIGITS;
IMPORT int __TBL_10_HUGE_START;
IMPORT int __TBL_10_SMALL_DIGITS;
IMPORT int __TBL_10_SMALL_START;
IMPORT int __TBL_2_BIG_DIGITS;
IMPORT int __TBL_2_BIG_START;
IMPORT int __TBL_2_HUGE_DIGITS;
IMPORT int __TBL_2_HUGE_START;
IMPORT int __TBL_2_SMALL_DIGITS;
IMPORT int __TBL_2_SMALL_START;
IMPORT int __TBL_BASELG;
IMPORT int __TBL_NTENS;
IMPORT int __TBL_TENS;
IMPORT int __THREADED;
IMPORT int __TIMER_CREATE ();
IMPORT int __TIMER_DELETE ();
IMPORT int __TIMER_GETOVERRUN ();
IMPORT int __TIMER_GETTIME ();
IMPORT int __TIMER_SETTIME ();
IMPORT int __UDIV64 ();
IMPORT int __UMAC ();
IMPORT int __UMUL64 ();
IMPORT int __UNPACKDOUBLE ();
IMPORT int __UNPACKLLONG ();
IMPORT int __UNPACKSINGLE ();
IMPORT int __UNPACK_DOUBLE ();
IMPORT int __UNPACK_DOUBLE_TWO ();
IMPORT int __UNPACK_EXTENDED ();
IMPORT int __UNPACK_QUADRUPLE_TWO ();
IMPORT int __UNPACK_SINGLE ();
IMPORT int __UREM64 ();
IMPORT int __UTX_LOCK;
IMPORT int __VSNPRINTF ();
IMPORT int __WELL_HIDDEN_DGETTEXT ();
IMPORT int __XPG4;
IMPORT int __YDAY_TO_MONTH;
IMPORT int __YEAR_LENGTHS;
IMPORT int ___ERRNO ();
IMPORT int ___MUL_65536_N ();
IMPORT int ___x_gnu___gcc_bcmp_o;
IMPORT int ___x_gnu__ashldi3_o;
IMPORT int ___x_gnu__ashrdi3_o;
IMPORT int ___x_gnu__bb_o;
IMPORT int ___x_gnu__clear_cache_o;
IMPORT int ___x_gnu__cmpdi2_o;
IMPORT int ___x_gnu__divdi3_o;
IMPORT int ___x_gnu__eprintf_o;
IMPORT int ___x_gnu__ffsdi2_o;
IMPORT int ___x_gnu__fixdfdi_o;
IMPORT int ___x_gnu__fixsfdi_o;
IMPORT int ___x_gnu__fixtfdi_o;
IMPORT int ___x_gnu__fixunsdfdi_o;
IMPORT int ___x_gnu__fixunsdfsi_o;
IMPORT int ___x_gnu__fixunssfdi_o;
IMPORT int ___x_gnu__fixunssfsi_o;
IMPORT int ___x_gnu__fixunstfdi_o;
IMPORT int ___x_gnu__fixunsxfdi_o;
IMPORT int ___x_gnu__fixunsxfsi_o;
IMPORT int ___x_gnu__fixxfdi_o;
IMPORT int ___x_gnu__floatdidf_o;
IMPORT int ___x_gnu__floatdisf_o;
IMPORT int ___x_gnu__floatditf_o;
IMPORT int ___x_gnu__floatdixf_o;
IMPORT int ___x_gnu__lshrdi3_o;
IMPORT int ___x_gnu__moddi3_o;
IMPORT int ___x_gnu__muldi3_o;
IMPORT int ___x_gnu__negdi2_o;
IMPORT int ___x_gnu__shtab_o;
IMPORT int ___x_gnu__trampoline_o;
IMPORT int ___x_gnu__ucmpdi2_o;
IMPORT int ___x_gnu__udiv_w_sdiv_o;
IMPORT int ___x_gnu__udivdi3_o;
IMPORT int ___x_gnu__udivmoddi4_o;
IMPORT int ___x_gnu__umoddi3_o;
IMPORT int __ashldi3 ();
IMPORT int __ashrdi3 ();
IMPORT int __assert ();
IMPORT int __cacheTextUpdate ();
IMPORT int __clear_cache ();
IMPORT int __clocale;
IMPORT int __cmpdi2 ();
IMPORT int __common_intrinsicsInit ();
IMPORT int __costate;
IMPORT int __cplusLoadObjFiles;
IMPORT int __cplusLoad_o;
IMPORT int __cplusUsr_o;
IMPORT int __cplusXtors_o;
IMPORT int __ctype;
IMPORT int __daysSinceEpoch ();
IMPORT int __div ();
IMPORT int __divdi3 ();
IMPORT int __dtoll ();
IMPORT int __dtoull ();
IMPORT int __errno ();
IMPORT int __exp10 ();
IMPORT int __ffsdi2 ();
IMPORT int __fixdfdi ();
IMPORT int __fixsfdi ();
IMPORT int __fixtfdi ();
IMPORT int __fixunsdfdi ();
IMPORT int __fixunsdfsi ();
IMPORT int __fixunssfdi ();
IMPORT int __fixunssfsi ();
IMPORT int __fixunstfdi ();
IMPORT int __floatdidf ();
IMPORT int __floatdisf ();
IMPORT int __floatditf ();
IMPORT int __ftoll ();
IMPORT int __ftoull ();
IMPORT int __gcc_bcmp ();
IMPORT int __gcc_intrinsicsInit ();
IMPORT int __getDstInfo ();
IMPORT int __getTime ();
IMPORT int __getZoneInfo ();
IMPORT int __includeGnuIntrinsics;
IMPORT int __julday ();
IMPORT int __locale;
IMPORT int __loctime;
IMPORT int __lshrdi3 ();
IMPORT int __main ();
IMPORT int __moddi3 ();
IMPORT int __mul ();
IMPORT int __muldi3 ();
IMPORT int __negdi2 ();
IMPORT int __rem ();
IMPORT int __sclose ();
IMPORT int __sflags ();
IMPORT int __sflush ();
IMPORT int __sfvwrite ();
IMPORT int __shtab;
IMPORT int __sigCtxLoad ();
IMPORT int __sigCtxSave ();
IMPORT int __smakebuf ();
IMPORT int __sread ();
IMPORT int __srefill ();
IMPORT int __srget ();
IMPORT int __sseek ();
IMPORT int __stderr ();
IMPORT int __stdin ();
IMPORT int __stdout ();
IMPORT int __stret4 ();
IMPORT int __stret8 ();
IMPORT int __strxfrm ();
IMPORT int __swbuf ();
IMPORT int __swrite ();
IMPORT int __swsetup ();
IMPORT int __uc2Reg ();
IMPORT int __ucmpdi2 ();
IMPORT int __udiv ();
IMPORT int __udiv_w_sdiv ();
IMPORT int __udivdi3 ();
IMPORT int __udivmoddi4 ();
IMPORT int __umoddi3 ();
IMPORT int __umul ();
IMPORT int __urem ();
IMPORT int __wdbEventListIsEmpty;
IMPORT int __wdbEvtptDeleteAll;
IMPORT int _archHelp_msg;
IMPORT int _clockRealtime;
IMPORT int _dbgArchInit ();
IMPORT int _dbgDsmInstRtn;
IMPORT int _dbgFuncCallCheck ();
IMPORT int _dbgInstSizeGet ();
IMPORT int _dbgRetAdrsGet ();
IMPORT int _dbgTaskPCGet ();
IMPORT int _dbgTaskPCSet ();
IMPORT int _exit ();
IMPORT int _func_bdall;
IMPORT int _func_dbgHostNotify;
IMPORT int _func_dbgTargetNotify;
IMPORT int _func_dspMregsHook;
IMPORT int _func_dspRegsListHook;
IMPORT int _func_dspTaskRegsShow;
IMPORT int _func_evtLogM0;
IMPORT int _func_evtLogM1;
IMPORT int _func_evtLogM2;
IMPORT int _func_evtLogM3;
IMPORT int _func_evtLogO;
IMPORT int _func_evtLogOIntLock;
IMPORT int _func_evtLogPoint;
IMPORT int _func_evtLogReserveTaskName;
IMPORT int _func_evtLogString;
IMPORT int _func_evtLogT0;
IMPORT int _func_evtLogT0_noInt;
IMPORT int _func_evtLogT1;
IMPORT int _func_evtLogT1_noTS;
IMPORT int _func_evtLogTSched;
IMPORT int _func_excBaseHook;
IMPORT int _func_excInfoShow;
IMPORT int _func_excIntHook;
IMPORT int _func_excJobAdd;
IMPORT int _func_excPanicHook;
IMPORT int _func_fclose;
IMPORT int _func_fppTaskRegsShow;
IMPORT int _func_ftpLs;
IMPORT int _func_ioTaskStdSet;
IMPORT int _func_logMsg;
IMPORT int _func_memalign;
IMPORT int _func_netLsByName;
IMPORT int _func_printErr;
IMPORT int _func_pthread_setcanceltype;
IMPORT int _func_remCurIdGet;
IMPORT int _func_remCurIdSet;
IMPORT int _func_selPtyAdd;
IMPORT int _func_selPtyDelete;
IMPORT int _func_selTyAdd;
IMPORT int _func_selTyDelete;
IMPORT int _func_selWakeupAll;
IMPORT int _func_selWakeupListInit;
IMPORT int _func_selWakeupListTerm;
IMPORT int _func_sigExcKill;
IMPORT int _func_sigTimeoutRecalc;
IMPORT int _func_sigprocmask;
IMPORT int _func_smObjObjShow;
IMPORT int _func_spy;
IMPORT int _func_spyClkStart;
IMPORT int _func_spyClkStop;
IMPORT int _func_spyReport;
IMPORT int _func_spyStop;
IMPORT int _func_spyTask;
IMPORT int _func_sseTaskRegsShow;
IMPORT int _func_symFindByValue;
IMPORT int _func_symFindByValueAndType;
IMPORT int _func_symFindSymbol;
IMPORT int _func_symNameGet;
IMPORT int _func_symTypeGet;
IMPORT int _func_symValueGet;
IMPORT int _func_taskCreateHookAdd;
IMPORT int _func_taskDeleteHookAdd;
IMPORT int _func_taskRegsShowRtn;
IMPORT int _func_tmrConnect;
IMPORT int _func_tmrDisable;
IMPORT int _func_tmrEnable;
IMPORT int _func_tmrFreq;
IMPORT int _func_tmrPeriod;
IMPORT int _func_tmrStamp;
IMPORT int _func_tmrStampLock;
IMPORT int _func_trap;
IMPORT int _func_trgCheck;
IMPORT int _func_valloc;
IMPORT int _func_vxMemProbeHook;
IMPORT int _func_wdbIsNowExternal;
IMPORT int _func_wvNetAddressFilterTest;
IMPORT int _func_wvNetPortFilterTest;
IMPORT int _lib_version ();
IMPORT int _mutex_lock ();
IMPORT int _mutex_unlock ();
IMPORT int _pSigQueueFreeHead;
IMPORT int _procNumWasSet;
IMPORT int _setjmpSetup ();
IMPORT int _sigCtxLoad ();
IMPORT int _sigCtxRtnValSet ();
IMPORT int _sigCtxSave ();
IMPORT int _sigCtxSetup ();
IMPORT int _sigCtxStackEnd ();
IMPORT int _sigfaulttable;
IMPORT int _sysInit ();
IMPORT int _taskUcShow ();
IMPORT int _wdbTaskBpAdd;
IMPORT int _wdbTaskBpTrap;
IMPORT int _wdbTaskCont;
IMPORT int _wdbTaskStep;
IMPORT int _yReg ();
IMPORT int abort ();
IMPORT int abs ();
IMPORT int acos ();
IMPORT int activeQHead;
IMPORT int addSegNames ();
IMPORT int asctime ();
IMPORT int asctime_r ();
IMPORT int asin ();
IMPORT int assertFiles;
IMPORT int atan ();
IMPORT int atan2 ();
IMPORT int atexit ();
IMPORT int atof ();
IMPORT int atoi ();
IMPORT int atol ();
IMPORT int attrib ();
IMPORT int b ();
IMPORT int bcmp ();
IMPORT int bcopy ();
IMPORT int bcopyBytes ();
IMPORT int bcopyLongs ();
IMPORT int bcopyWords ();
IMPORT int bd ();
IMPORT int bdall ();
IMPORT int bfill ();
IMPORT int bfillBytes ();
IMPORT int binaryOptionsTsfsDrv;
IMPORT int binvert ();
IMPORT int bootBpAnchorExtract ();
IMPORT int bootChange ();
IMPORT int bootLeaseExtract ();
IMPORT int bootNetmaskExtract ();
IMPORT int bootParamsErrorPrint ();
IMPORT int bootParamsPrompt ();
IMPORT int bootParamsShow ();
IMPORT int bootScanNum ();
IMPORT int bootStringToStruct ();
IMPORT int bootStructToString ();
IMPORT int bpFreeList;
IMPORT int bpList;
IMPORT int bsearch ();
IMPORT int bswap ();
IMPORT int bufAlloc ();
IMPORT int bufFree ();
IMPORT int bufPoolInit ();
IMPORT int bzero ();
IMPORT int c ();
IMPORT int cacheArchLibInit ();
IMPORT int cacheClear ();
IMPORT int cacheDataEnabled;
IMPORT int cacheDataMode;
IMPORT int cacheDisable ();
IMPORT int cacheDmaFree ();
IMPORT int cacheDmaFreeRtn;
IMPORT int cacheDmaFuncs;
IMPORT int cacheDmaMalloc ();
IMPORT int cacheDmaMallocRtn;
IMPORT int cacheDrvFlush ();
IMPORT int cacheDrvInvalidate ();
IMPORT int cacheDrvPhysToVirt ();
IMPORT int cacheDrvVirtToPhys ();
IMPORT int cacheEnable ();
IMPORT int cacheFlush ();
IMPORT int cacheFuncsSet ();
IMPORT int cacheInvalidate ();
IMPORT int cacheLib;
IMPORT int cacheLibInit ();
IMPORT int cacheLock ();
IMPORT int cacheMmuAvailable;
IMPORT int cacheNullFuncs;
IMPORT int cachePipeFlush ();
IMPORT int cacheTextUpdate ();
IMPORT int cacheUnlock ();
IMPORT int cacheUserFuncs;
IMPORT int calloc ();
IMPORT int cd ();
IMPORT int ceil ();
IMPORT int cfree ();
IMPORT int changeReg ();
IMPORT int chdir ();
IMPORT int checkStack ();
IMPORT int checksum ();
IMPORT int chkdsk ();
IMPORT int classClassId;
IMPORT int classCreate ();
IMPORT int classDestroy ();
IMPORT int classInit ();
IMPORT int classInstConnect ();
IMPORT int classInstrument ();
IMPORT int classLibInit ();
IMPORT int classMemPartIdSet ();
IMPORT int classShow ();
IMPORT int classShowConnect ();
IMPORT int classShowInit ();
IMPORT int clearerr ();
IMPORT int clock ();
IMPORT int clockLibInit ();
IMPORT int clock_getres ();
IMPORT int clock_gettime ();
IMPORT int clock_setres ();
IMPORT int clock_settime ();
IMPORT int clock_show ();
IMPORT int close ();
IMPORT int closedir ();
IMPORT int consoleFd;
IMPORT int consoleName;
IMPORT int copy ();
IMPORT int copyStreams ();
IMPORT int copyright_wind_river;
IMPORT int copysign ();
IMPORT int cos ();
IMPORT int cosh ();
IMPORT int cp ();
IMPORT int cplusCallCtors ();
IMPORT int cplusCallDtors ();
IMPORT int cplusCtors ();
IMPORT int cplusCtorsLink ();
IMPORT int cplusDemangle ();
IMPORT int cplusDemangleFunc;
IMPORT int cplusDemanglerMode;
IMPORT int cplusDemanglerSet ();
IMPORT int cplusDemanglerStyle;
IMPORT int cplusDemanglerStyleSet ();
IMPORT int cplusDtors ();
IMPORT int cplusDtorsLink ();
IMPORT int cplusLoadFixup ();
IMPORT int cplusMatchMangled ();
IMPORT int cplusUnloadFixup ();
IMPORT int cplusXtorSet ();
IMPORT int cplusXtorStrategy;
IMPORT int creat ();
IMPORT int creationDate;
IMPORT int cret ();
IMPORT int ctime ();
IMPORT int ctime_r ();
IMPORT int ctypeFiles;
IMPORT int d ();
IMPORT int dbgHelp ();
IMPORT int dbgInit ();
IMPORT int dbgLockUnbreakable;
IMPORT int dbgPrintCall ();
IMPORT int dbgPrintDsp;
IMPORT int dbgPrintFpp;
IMPORT int dbgPrintSimd;
IMPORT int dbgSafeUnbreakable;
IMPORT int dbgTaskBpBreakpoint ();
IMPORT int dbgTaskBpHooksInstall ();
IMPORT int dbgTaskBpTrace ();
IMPORT int dbgTaskBpTrap ();
IMPORT int dbgTaskCont ();
IMPORT int dbgTaskStep ();
IMPORT int dbgUnbreakableOld;
IMPORT int devs ();
IMPORT int difftime ();
IMPORT int dirList ();
IMPORT int diskFormat ();
IMPORT int diskInit ();
IMPORT int div ();
IMPORT int div_r ();
IMPORT int dlclose ();
IMPORT int dlerror ();
IMPORT int dllAdd ();
IMPORT int dllCount ();
IMPORT int dllCreate ();
IMPORT int dllDelete ();
IMPORT int dllEach ();
IMPORT int dllGet ();
IMPORT int dllInit ();
IMPORT int dllInsert ();
IMPORT int dllRemove ();
IMPORT int dllTerminate ();
IMPORT int dlopen ();
IMPORT int dlsym ();
IMPORT int drem ();
IMPORT int drvTable;
IMPORT int dsmData ();
IMPORT int dsmInst ();
IMPORT int dsmNbytes ();
IMPORT int e ();
IMPORT int envLibInit ();
IMPORT int envPrivateCreate ();
IMPORT int envPrivateDestroy ();
IMPORT int envShow ();
IMPORT int errno;
IMPORT int errnoGet ();
IMPORT int errnoOfTaskGet ();
IMPORT int errnoOfTaskSet ();
IMPORT int errnoSet ();
IMPORT int eventClear ();
IMPORT int eventEvtRtn;
IMPORT int eventInit ();
IMPORT int eventLibInit ();
IMPORT int eventReceive ();
IMPORT int eventRsrcSend ();
IMPORT int eventRsrcShow ();
IMPORT int eventSend ();
IMPORT int eventStart ();
IMPORT int eventTaskShow ();
IMPORT int eventTerminate ();
IMPORT int evtAction;
IMPORT int evtBufferBind ();
IMPORT int evtBufferId;
IMPORT int evtLogFuncBind ();
IMPORT int evtLogM0 ();
IMPORT int evtLogM1 ();
IMPORT int evtLogM2 ();
IMPORT int evtLogM3 ();
IMPORT int evtLogO ();
IMPORT int evtLogOInt ();
IMPORT int evtLogPoint ();
IMPORT int evtLogString ();
IMPORT int evtLogT0 ();
IMPORT int evtLogT0_noInt ();
IMPORT int evtLogT1 ();
IMPORT int evtLogTasks ();
IMPORT int evtObjLogFuncBind ();
IMPORT int evtsched ();
IMPORT int excExcHandle ();
IMPORT int excExcepHook;
IMPORT int excHookAdd ();
IMPORT int excInit ();
IMPORT int excIntHandle ();
IMPORT int excJobAdd ();
IMPORT int excMsgQId;
IMPORT int excShowInit ();
IMPORT int excTask ();
IMPORT int excTaskId;
IMPORT int excTaskOptions;
IMPORT int excTaskPriority;
IMPORT int excTaskStackSize;
IMPORT int excVecInit ();
IMPORT int execute ();
IMPORT int exit ();
IMPORT int exp ();
IMPORT int exp__E ();
IMPORT int expm1 ();
IMPORT int fabs ();
IMPORT int fclose ();
IMPORT int fdTable;
IMPORT int fdopen ();
IMPORT int fdprintf ();
IMPORT int feof ();
IMPORT int ferror ();
IMPORT int fflush ();
IMPORT int ffsLsb ();
IMPORT int ffsLsbTbl;
IMPORT int ffsMsb ();
IMPORT int ffsMsbTbl;
IMPORT int fgetc ();
IMPORT int fgetpos ();
IMPORT int fgets ();
IMPORT int fieldSzIncludeSign;
IMPORT int fileUpPathDefaultPerm;
IMPORT int fileUploadPathClose ();
IMPORT int fileUploadPathCreate ();
IMPORT int fileUploadPathLibInit ();
IMPORT int fileUploadPathWrite ();
IMPORT int fileno ();
IMPORT int finite ();
IMPORT int fioFltInstall ();
IMPORT int fioFormatV ();
IMPORT int fioLibInit ();
IMPORT int fioRdString ();
IMPORT int fioRead ();
IMPORT int fioScanV ();
IMPORT int floatInit ();
IMPORT int floor ();
IMPORT int fmod ();
IMPORT int fopen ();
IMPORT int fpClassId;
IMPORT int fpCtlRegName;
IMPORT int fpRegName;
IMPORT int fpTypeGet ();
IMPORT int fppArchInit ();
IMPORT int fppArchTaskCreateInit ();
IMPORT int fppCreateHookRtn;
IMPORT int fppCtxToRegs ();
IMPORT int fppDisplayHookRtn;
IMPORT int fppFsrDefault;
IMPORT int fppInit ();
IMPORT int fppNan ();
IMPORT int fppProbe ();
IMPORT int fppRegsToCtx ();
IMPORT int fppRestore ();
IMPORT int fppSave ();
IMPORT int fppShowInit ();
IMPORT int fppTaskRegsCFmt;
IMPORT int fppTaskRegsDFmt;
IMPORT int fppTaskRegsGet ();
IMPORT int fppTaskRegsSet ();
IMPORT int fppTaskRegsShow ();
IMPORT int fprintf ();
IMPORT int fputc ();
IMPORT int fputs ();
IMPORT int fread ();
IMPORT int free ();
IMPORT int freopen ();
IMPORT int frexp ();
IMPORT int fscanf ();
IMPORT int fseek ();
IMPORT int fsetpos ();
IMPORT int fstat ();
IMPORT int fstatfs ();
IMPORT int ftell ();
IMPORT int ftplDebug;
IMPORT int fwrite ();
IMPORT int g0 ();
IMPORT int g1 ();
IMPORT int g2 ();
IMPORT int g3 ();
IMPORT int g4 ();
IMPORT int g5 ();
IMPORT int g6 ();
IMPORT int g7 ();
IMPORT int getc ();
IMPORT int getchar ();
IMPORT int getcwd ();
IMPORT int getenv ();
IMPORT int gets ();
IMPORT int getw ();
IMPORT int getwd ();
IMPORT int gmtime ();
IMPORT int gmtime_r ();
IMPORT int h ();
IMPORT int hashClassId;
IMPORT int hashFuncIterScale ();
IMPORT int hashFuncModulo ();
IMPORT int hashFuncMultiply ();
IMPORT int hashKeyCmp ();
IMPORT int hashKeyStrCmp ();
IMPORT int hashLibInit ();
IMPORT int hashTblCreate ();
IMPORT int hashTblDelete ();
IMPORT int hashTblDestroy ();
IMPORT int hashTblEach ();
IMPORT int hashTblFind ();
IMPORT int hashTblInit ();
IMPORT int hashTblPut ();
IMPORT int hashTblRemove ();
IMPORT int hashTblTerminate ();
IMPORT int help ();
IMPORT int i ();
IMPORT int i0 ();
IMPORT int i1 ();
IMPORT int i2 ();
IMPORT int i3 ();
IMPORT int i4 ();
IMPORT int i5 ();
IMPORT int i6 ();
IMPORT int i7 ();
IMPORT int index ();
IMPORT int intCatch ();
IMPORT int intCnt;
IMPORT int intConnect ();
IMPORT int intContext ();
IMPORT int intCount ();
IMPORT int intDisable ();
IMPORT int intEnable ();
IMPORT int intHandlerCreate ();
IMPORT int intLevelSet ();
IMPORT int intLock ();
IMPORT int intLockLevelGet ();
IMPORT int intLockLevelSet ();
IMPORT int intLockMask;
IMPORT int intRegsLock ();
IMPORT int intRegsUnlock ();
IMPORT int intRestrict ();
IMPORT int intRun ();
IMPORT int intUnlock ();
IMPORT int intUnlockMask;
IMPORT int intVecBaseGet ();
IMPORT int intVecBaseSet ();
IMPORT int intVecGet ();
IMPORT int intVecSet ();
IMPORT int intVecTable;
IMPORT int intVecTableWriteProtect ();
IMPORT int ioDefDevGet ();
IMPORT int ioDefDirGet ();
IMPORT int ioDefPath;
IMPORT int ioDefPathCat ();
IMPORT int ioDefPathGet ();
IMPORT int ioDefPathSet ();
IMPORT int ioFullFileNameGet ();
IMPORT int ioGlobalStdGet ();
IMPORT int ioGlobalStdSet ();
IMPORT int ioHelp ();
IMPORT int ioMaxLinkLevels;
IMPORT int ioTaskStdGet ();
IMPORT int ioTaskStdSet ();
IMPORT int ioctl ();
IMPORT int iosClose ();
IMPORT int iosCreate ();
IMPORT int iosDelete ();
IMPORT int iosDevAdd ();
IMPORT int iosDevDelete ();
IMPORT int iosDevFind ();
IMPORT int iosDevShow ();
IMPORT int iosDrvInstall ();
IMPORT int iosDrvRemove ();
IMPORT int iosDrvShow ();
IMPORT int iosDvList;
IMPORT int iosFdDevFind ();
IMPORT int iosFdFree ();
IMPORT int iosFdFreeHookRtn;
IMPORT int iosFdNew ();
IMPORT int iosFdNewHookRtn;
IMPORT int iosFdSet ();
IMPORT int iosFdShow ();
IMPORT int iosFdValue ();
IMPORT int iosInit ();
IMPORT int iosIoctl ();
IMPORT int iosLibInitialized;
IMPORT int iosNextDevGet ();
IMPORT int iosOpen ();
IMPORT int iosRead ();
IMPORT int iosShowInit ();
IMPORT int iosWrite ();
IMPORT int isTaskNew ();
IMPORT int isalnum ();
IMPORT int isalpha ();
IMPORT int isatty ();
IMPORT int iscntrl ();
IMPORT int isdigit ();
IMPORT int isgraph ();
IMPORT int islower ();
IMPORT int isprint ();
IMPORT int ispunct ();
IMPORT int isspace ();
IMPORT int isupper ();
IMPORT int isxdigit ();
IMPORT int kernelInit ();
IMPORT int kernelIsIdle;
IMPORT int kernelState;
IMPORT int kernelTimeSlice ();
IMPORT int kernelVersion ();
IMPORT int kill ();
IMPORT int l ();
IMPORT int l0 ();
IMPORT int l1 ();
IMPORT int l2 ();
IMPORT int l3 ();
IMPORT int l4 ();
IMPORT int l5 ();
IMPORT int l6 ();
IMPORT int l7 ();
IMPORT int labs ();
IMPORT int ld ();
IMPORT int ldCommonMatchAll;
IMPORT int ldexp ();
IMPORT int ldiv ();
IMPORT int ldiv_r ();
IMPORT int ledClose ();
IMPORT int ledControl ();
IMPORT int ledId;
IMPORT int ledOpen ();
IMPORT int ledRead ();
IMPORT int lexActions ();
IMPORT int lexClass;
IMPORT int lexNclasses;
IMPORT int lexStateTable;
IMPORT int linkedCtorsInitialized;
IMPORT int lkAddr ();
IMPORT int lkup ();
IMPORT int ll ();
IMPORT int llr ();
IMPORT int loadCommonManage ();
IMPORT int loadCommonMatch ();
IMPORT int loadModule ();
IMPORT int loadModuleAt ();
IMPORT int loadModuleAtSym ();
IMPORT int loadModuleGet ();
IMPORT int loadRoutine;
IMPORT int loadSegmentsAllocate ();
IMPORT int localToGlobalOffset;
IMPORT int localeFiles;
IMPORT int localeconv ();
IMPORT int localtime ();
IMPORT int localtime_r ();
IMPORT int log ();
IMPORT int log10 ();
IMPORT int logFdAdd ();
IMPORT int logFdDelete ();
IMPORT int logFdFromRlogin;
IMPORT int logFdSet ();
IMPORT int logInit ();
IMPORT int logMsg ();
IMPORT int logShow ();
IMPORT int logTask ();
IMPORT int logTaskId;
IMPORT int logTaskOptions;
IMPORT int logTaskPriority;
IMPORT int logTaskStackSize;
IMPORT int log__L ();
IMPORT int logb ();
IMPORT int logout ();
IMPORT int longjmp ();
IMPORT int ls ();
IMPORT int lseek ();
IMPORT int lsr ();
IMPORT int lstAdd ();
IMPORT int lstConcat ();
IMPORT int lstCount ();
IMPORT int lstDelete ();
IMPORT int lstExtract ();
IMPORT int lstFind ();
IMPORT int lstFirst ();
IMPORT int lstFree ();
IMPORT int lstGet ();
IMPORT int lstInit ();
IMPORT int lstInsert ();
IMPORT int lstLast ();
IMPORT int lstLibInit ();
IMPORT int lstNStep ();
IMPORT int lstNext ();
IMPORT int lstNth ();
IMPORT int lstPrevious ();
IMPORT int m ();
IMPORT int mRegs ();
IMPORT int main ();
IMPORT int malloc ();
IMPORT int mathFiles;
IMPORT int mathHardInit ();
IMPORT int maxDrivers;
IMPORT int maxFiles;
IMPORT int mblen ();
IMPORT int mbstowcs ();
IMPORT int mbtowc ();
IMPORT int memAddToPool ();
IMPORT int memDefaultAlignment;
IMPORT int memFindMax ();
IMPORT int memInit ();
IMPORT int memLibInit ();
IMPORT int memOptionsSet ();
IMPORT int memPartAddToPool ();
IMPORT int memPartAlignedAlloc ();
IMPORT int memPartAlloc ();
IMPORT int memPartAllocErrorRtn;
IMPORT int memPartBlockErrorRtn;
IMPORT int memPartBlockIsValid ();
IMPORT int memPartClassId;
IMPORT int memPartCreate ();
IMPORT int memPartFindMax ();
IMPORT int memPartFree ();
IMPORT int memPartInfoGet ();
IMPORT int memPartInit ();
IMPORT int memPartInstClassId;
IMPORT int memPartLibInit ();
IMPORT int memPartOptionsDefault;
IMPORT int memPartOptionsSet ();
IMPORT int memPartRealloc ();
IMPORT int memPartSemInitRtn;
IMPORT int memPartShow ();
IMPORT int memShow ();
IMPORT int memShowInit ();
IMPORT int memSysPartId;
IMPORT int memalign ();
IMPORT int memchr ();
IMPORT int memcmp ();
IMPORT int memcpy ();
IMPORT int memmove ();
IMPORT int memset ();
IMPORT int mkdir ();
IMPORT int mktime ();
IMPORT int modf ();
IMPORT int moduleCheck ();
IMPORT int moduleClassId;
IMPORT int moduleCreate ();
IMPORT int moduleCreateHookAdd ();
IMPORT int moduleCreateHookDelete ();
IMPORT int moduleDelete ();
IMPORT int moduleEach ();
IMPORT int moduleFindByGroup ();
IMPORT int moduleFindByName ();
IMPORT int moduleFindByNameAndPath ();
IMPORT int moduleFlagsGet ();
IMPORT int moduleIdFigure ();
IMPORT int moduleIdListGet ();
IMPORT int moduleInfoGet ();
IMPORT int moduleInit ();
IMPORT int moduleLibInit ();
IMPORT int moduleNameGet ();
IMPORT int moduleSegAdd ();
IMPORT int moduleSegEach ();
IMPORT int moduleSegFirst ();
IMPORT int moduleSegGet ();
IMPORT int moduleSegNext ();
IMPORT int moduleShow ();
IMPORT int moduleTerminate ();
IMPORT int msgQClassId;
IMPORT int msgQCreate ();
IMPORT int msgQDelete ();
IMPORT int msgQDistInfoGetRtn;
IMPORT int msgQDistNumMsgsRtn;
IMPORT int msgQDistReceiveRtn;
IMPORT int msgQDistSendRtn;
IMPORT int msgQDistShowRtn;
IMPORT int msgQEvLibInit ();
IMPORT int msgQEvStart ();
IMPORT int msgQEvStop ();
IMPORT int msgQInfoGet ();
IMPORT int msgQInit ();
IMPORT int msgQInstClassId;
IMPORT int msgQLibInit ();
IMPORT int msgQNumMsgs ();
IMPORT int msgQReceive ();
IMPORT int msgQSend ();
IMPORT int msgQShow ();
IMPORT int msgQShowInit ();
IMPORT int msgQSmInfoGetRtn;
IMPORT int msgQSmNumMsgsRtn;
IMPORT int msgQSmReceiveRtn;
IMPORT int msgQSmSendRtn;
IMPORT int msgQSmShowRtn;
IMPORT int msgQTerminate ();
IMPORT int mutexOptionsIosLib;
IMPORT int mutexOptionsLogLib;
IMPORT int mutexOptionsMemLib;
IMPORT int mutexOptionsSelectLib;
IMPORT int mutexOptionsSymLib;
IMPORT int mutexOptionsTsfsDrv;
IMPORT int mutexOptionsTyLib;
IMPORT int mv ();
IMPORT int namelessPrefix;
IMPORT int netHelp ();
IMPORT int npc ();
IMPORT int o0 ();
IMPORT int o1 ();
IMPORT int o2 ();
IMPORT int o3 ();
IMPORT int o4 ();
IMPORT int o5 ();
IMPORT int o6 ();
IMPORT int o7 ();
IMPORT int objAlloc ();
IMPORT int objAllocExtra ();
IMPORT int objCoreInit ();
IMPORT int objCoreTerminate ();
IMPORT int objFree ();
IMPORT int objShow ();
IMPORT int open ();
IMPORT int opendir ();
IMPORT int pFppTaskIdPrevious;
IMPORT int pJobPool;
IMPORT int pRootMemStart;
IMPORT int pTaskLastDspTcb;
IMPORT int pTaskLastFpTcb;
IMPORT int pWdbMemRegions;
IMPORT int pWdbRtIf;
IMPORT int pWvNetEventMap;
IMPORT int passFsDevInit ();
IMPORT int passFsInit ();
IMPORT int passFsUnixDirPerm;
IMPORT int passFsUnixFilePerm;
IMPORT int passVolume;
IMPORT int pathBuild ();
IMPORT int pathCat ();
IMPORT int pathCondense ();
IMPORT int pathLastName ();
IMPORT int pathLastNamePtr ();
IMPORT int pathParse ();
IMPORT int pathSplit ();
IMPORT int pause ();
IMPORT int pc ();
IMPORT int period ();
IMPORT int periodRun ();
IMPORT int perror ();
IMPORT int pipeDevCreate ();
IMPORT int pipeDevDelete ();
IMPORT int pipeDrv ();
IMPORT int pipeMsgQOptions;
IMPORT int pmPartId;
IMPORT int pow ();
IMPORT int pow_p ();
IMPORT int ppGlobalEnviron;
IMPORT int printErr ();
IMPORT int printErrno ();
IMPORT int printExc ();
IMPORT int printLogo ();
IMPORT int printf ();
IMPORT int psr ();
IMPORT int putc ();
IMPORT int putchar ();
IMPORT int putenv ();
IMPORT int puts ();
IMPORT int putw ();
IMPORT int pwd ();
IMPORT int qAdvance ();
IMPORT int qCalibrate ();
IMPORT int qCreate ();
IMPORT int qDelete ();
IMPORT int qEach ();
IMPORT int qFifoClassId;
IMPORT int qFifoCreate ();
IMPORT int qFifoDelete ();
IMPORT int qFifoEach ();
IMPORT int qFifoGet ();
IMPORT int qFifoInfo ();
IMPORT int qFifoInit ();
IMPORT int qFifoPut ();
IMPORT int qFifoRemove ();
IMPORT int qFirst ();
IMPORT int qGet ();
IMPORT int qGetExpired ();
IMPORT int qInfo ();
IMPORT int qInit ();
IMPORT int qJobClassId;
IMPORT int qJobCreate ();
IMPORT int qJobDelete ();
IMPORT int qJobEach ();
IMPORT int qJobGet ();
IMPORT int qJobInfo ();
IMPORT int qJobInit ();
IMPORT int qJobPut ();
IMPORT int qJobTerminate ();
IMPORT int qKey ();
IMPORT int qPriBMapClassId;
IMPORT int qPriBMapCreate ();
IMPORT int qPriBMapDelete ();
IMPORT int qPriBMapEach ();
IMPORT int qPriBMapGet ();
IMPORT int qPriBMapInfo ();
IMPORT int qPriBMapInit ();
IMPORT int qPriBMapKey ();
IMPORT int qPriBMapListCreate ();
IMPORT int qPriBMapListDelete ();
IMPORT int qPriBMapPut ();
IMPORT int qPriBMapRemove ();
IMPORT int qPriBMapResort ();
IMPORT int qPriListAdvance ();
IMPORT int qPriListCalibrate ();
IMPORT int qPriListClassId;
IMPORT int qPriListCreate ();
IMPORT int qPriListDelete ();
IMPORT int qPriListEach ();
IMPORT int qPriListFromTailClassId;
IMPORT int qPriListGet ();
IMPORT int qPriListGetExpired ();
IMPORT int qPriListInfo ();
IMPORT int qPriListInit ();
IMPORT int qPriListKey ();
IMPORT int qPriListPut ();
IMPORT int qPriListPutFromTail ();
IMPORT int qPriListRemove ();
IMPORT int qPriListResort ();
IMPORT int qPriListTerminate ();
IMPORT int qPut ();
IMPORT int qRemove ();
IMPORT int qResort ();
IMPORT int qTerminate ();
IMPORT int qsort ();
IMPORT int rBuffClass;
IMPORT int rBuffClassId;
IMPORT int rBuffCreate ();
IMPORT int rBuffDestroy ();
IMPORT int rBuffFlush ();
IMPORT int rBuffInfoGet ();
IMPORT int rBuffLibInit ();
IMPORT int rBuffNBytes ();
IMPORT int rBuffRead ();
IMPORT int rBuffReadCommit ();
IMPORT int rBuffReadReserve ();
IMPORT int rBuffReset ();
IMPORT int rBuffSetFd ();
IMPORT int rBuffShow ();
IMPORT int rBuffShowInit ();
IMPORT int rBuffShowRtn;
IMPORT int rBuffUpload ();
IMPORT int rBuffVerify ();
IMPORT int rBuffWrite ();
IMPORT int raise ();
IMPORT int rand ();
IMPORT int read ();
IMPORT int readdir ();
IMPORT int readv ();
IMPORT int readyQBMap;
IMPORT int readyQHead;
IMPORT int realloc ();
IMPORT int reboot ();
IMPORT int rebootHookAdd ();
IMPORT int redirInFd;
IMPORT int redirOutFd;
IMPORT int remove ();
IMPORT int rename ();
IMPORT int repeat ();
IMPORT int repeatRun ();
IMPORT int reschedule ();
IMPORT int restartTaskName;
IMPORT int restartTaskOptions;
IMPORT int restartTaskPriority;
IMPORT int restartTaskStackSize;
IMPORT int rewind ();
IMPORT int rewinddir ();
IMPORT int rindex ();
IMPORT int rm ();
IMPORT int rmdir ();
IMPORT int rngBufGet ();
IMPORT int rngBufPut ();
IMPORT int rngCreate ();
IMPORT int rngDelete ();
IMPORT int rngFlush ();
IMPORT int rngFreeBytes ();
IMPORT int rngIsEmpty ();
IMPORT int rngIsFull ();
IMPORT int rngMoveAhead ();
IMPORT int rngNBytes ();
IMPORT int rngPutAhead ();
IMPORT int rootMemNBytes;
IMPORT int rootTaskId;
IMPORT int roundRobinOn;
IMPORT int roundRobinSlice;
IMPORT int runtimeName;
IMPORT int runtimeVersion;
IMPORT int s ();
IMPORT int s_dosTimeHook ();
IMPORT int s_fdint ();
IMPORT int s_init ();
IMPORT int s_sigcatch ();
IMPORT int s_sigcatchUser;
IMPORT int s_uname ();
IMPORT int s_userGet ();
IMPORT int scalb ();
IMPORT int scanCharSet ();
IMPORT int scanField ();
IMPORT int scanf ();
IMPORT int selNodeAdd ();
IMPORT int selNodeDelete ();
IMPORT int selTaskDeleteHookAdd ();
IMPORT int selWakeup ();
IMPORT int selWakeupAll ();
IMPORT int selWakeupListInit ();
IMPORT int selWakeupListLen ();
IMPORT int selWakeupListTerm ();
IMPORT int selWakeupType ();
IMPORT int select ();
IMPORT int selectInit ();
IMPORT int semBCoreInit ();
IMPORT int semBCreate ();
IMPORT int semBGive ();
IMPORT int semBGiveDefer ();
IMPORT int semBInit ();
IMPORT int semBLibInit ();
IMPORT int semBTake ();
IMPORT int semCCoreInit ();
IMPORT int semCCreate ();
IMPORT int semCGive ();
IMPORT int semCGiveDefer ();
IMPORT int semCInit ();
IMPORT int semCLibInit ();
IMPORT int semCTake ();
IMPORT int semClass;
IMPORT int semClassId;
IMPORT int semDelete ();
IMPORT int semDestroy ();
IMPORT int semEvIsFreeTbl;
IMPORT int semEvLibInit ();
IMPORT int semEvStart ();
IMPORT int semEvStop ();
IMPORT int semFlush ();
IMPORT int semFlushDefer ();
IMPORT int semFlushDeferTbl;
IMPORT int semFlushTbl;
IMPORT int semGive ();
IMPORT int semGiveDefer ();
IMPORT int semGiveDeferTbl;
IMPORT int semGiveTbl;
IMPORT int semInfo ();
IMPORT int semInstClass;
IMPORT int semInstClassId;
IMPORT int semIntRestrict ();
IMPORT int semInvalid ();
IMPORT int semLibInit ();
IMPORT int semMCoreInit ();
IMPORT int semMCreate ();
IMPORT int semMGive ();
IMPORT int semMGiveForce ();
IMPORT int semMGiveKern ();
IMPORT int semMGiveKernWork;
IMPORT int semMInit ();
IMPORT int semMLibInit ();
IMPORT int semMPendQPut ();
IMPORT int semMTake ();
IMPORT int semQFlush ();
IMPORT int semQFlushDefer ();
IMPORT int semQInit ();
IMPORT int semShow ();
IMPORT int semShowInit ();
IMPORT int semSmInfoRtn;
IMPORT int semSmShowRtn;
IMPORT int semTake ();
IMPORT int semTakeTbl;
IMPORT int semTerminate ();
IMPORT int seqConnect ();
IMPORT int seqDisable ();
IMPORT int seqEnable ();
IMPORT int seqFreq ();
IMPORT int seqPeriod ();
IMPORT int seqStamp ();
IMPORT int seqStampLock ();
IMPORT int setbuf ();
IMPORT int setbuffer ();
IMPORT int setjmp ();
IMPORT int setlinebuf ();
IMPORT int setlocale ();
IMPORT int setvbuf ();
IMPORT int shell ();
IMPORT int shellHistSize;
IMPORT int shellHistory ();
IMPORT int shellInit ();
IMPORT int shellIsRemoteConnectedGet ();
IMPORT int shellIsRemoteConnectedSet ();
IMPORT int shellLock ();
IMPORT int shellLogin ();
IMPORT int shellLoginInstall ();
IMPORT int shellLogout ();
IMPORT int shellLogoutInstall ();
IMPORT int shellOrigStdSet ();
IMPORT int shellPromptSet ();
IMPORT int shellRestart ();
IMPORT int shellScriptAbort ();
IMPORT int shellTaskId;
IMPORT int shellTaskName;
IMPORT int shellTaskOptions;
IMPORT int shellTaskPriority;
IMPORT int shellTaskStackSize;
IMPORT int show ();
IMPORT int sigEvtRtn;
IMPORT int sigInit ();
IMPORT int sigPendDestroy ();
IMPORT int sigPendInit ();
IMPORT int sigPendKill ();
IMPORT int sigaction ();
IMPORT int sigaddset ();
IMPORT int sigblock ();
IMPORT int sigdelset ();
IMPORT int sigemptyset ();
IMPORT int sigfillset ();
IMPORT int sigismember ();
IMPORT int signal ();
IMPORT int sigpending ();
IMPORT int sigprocmask ();
IMPORT int sigqueue ();
IMPORT int sigqueueInit ();
IMPORT int sigreturn ();
IMPORT int sigsetmask ();
IMPORT int sigsuspend ();
IMPORT int sigtimedwait ();
IMPORT int sigvec ();
IMPORT int sigwait ();
IMPORT int sigwaitinfo ();
IMPORT int simBlockSigs;
IMPORT int simReadSelect ();
IMPORT int sin ();
IMPORT int sinh ();
IMPORT int sllCount ();
IMPORT SL_LIST *sllCreate ();
IMPORT int sllDelete ();
IMPORT SL_NODE *sllEach ();
IMPORT SL_NODE *sllGet ();
IMPORT int sllInit ();
IMPORT SL_NODE *sllPrevious ();
IMPORT void sllPutAtHead ();
IMPORT void sllPutAtTail ();
IMPORT void sllRemove ();
IMPORT int sllTerminate ();
IMPORT int smMemPartAddToPoolRtn;
IMPORT int smMemPartAllocRtn;
IMPORT int smMemPartFindMaxRtn;
IMPORT int smMemPartFreeRtn;
IMPORT int smMemPartOptionsSetRtn;
IMPORT int smMemPartReallocRtn;
IMPORT int smMemPartShowRtn;
IMPORT int smObjPoolMinusOne;
IMPORT int smObjTaskDeleteFailRtn;
IMPORT int smObjTcbFreeFailRtn;
IMPORT int smObjTcbFreeRtn;
IMPORT int so ();
IMPORT int sp ();
IMPORT int spTaskOptions;
IMPORT int spTaskPriority;
IMPORT int spTaskStackSize;
IMPORT int sprintf ();
IMPORT int spy ();
IMPORT int spyClkStart ();
IMPORT int spyClkStop ();
IMPORT int spyHelp ();
IMPORT int spyReport ();
IMPORT int spyStop ();
IMPORT int spyTask ();
IMPORT int sqrt ();
IMPORT int srand ();
IMPORT int sscanf ();
IMPORT int standAloneSymTbl;
IMPORT int stat ();
IMPORT int statSymTbl;
IMPORT int statfs ();
IMPORT int stdioFiles;
IMPORT int stdioFp ();
IMPORT int stdioFpCreate ();
IMPORT int stdioFpDestroy ();
IMPORT int stdioInit ();
IMPORT int stdioShow ();
IMPORT int stdioShowInit ();
IMPORT int stdlibFiles;
IMPORT int strcat ();
IMPORT int strchr ();
IMPORT int strcmp ();
IMPORT int strcoll ();
IMPORT int strcpy ();
IMPORT int strcspn ();
IMPORT int strerror ();
IMPORT int strerror_r ();
IMPORT int strftime ();
IMPORT int stringFiles;
IMPORT int strlen ();
IMPORT int strncat ();
IMPORT int strncmp ();
IMPORT int strncpy ();
IMPORT int strpbrk ();
IMPORT int strrchr ();
IMPORT int strspn ();
IMPORT int strstr ();
IMPORT int strtod ();
IMPORT int strtok ();
IMPORT int strtok_r ();
IMPORT int strtol ();
IMPORT int strtoul ();
IMPORT int strxfrm ();
IMPORT int substrcmp ();
IMPORT int swab ();
IMPORT int symAdd ();
IMPORT int symAlloc ();
IMPORT int symByCNameFind ();
IMPORT int symByValueAndTypeFind ();
IMPORT int symByValueFind ();
IMPORT int symEach ();
IMPORT int symFindByCName ();
IMPORT int symFindByName ();
IMPORT int symFindByNameAndType ();
IMPORT int symFindByValue ();
IMPORT int symFindByValueAndType ();
IMPORT int symFindSymbol ();
IMPORT int symFree ();
IMPORT int symGroupDefault;
IMPORT int symInit ();
IMPORT int symLibInit ();
IMPORT int symLkupPgSz;
IMPORT int symName ();
IMPORT int symNameGet ();
IMPORT int symRemove ();
IMPORT int symSAdd ();
IMPORT int symShow ();
IMPORT int symShowInit ();
IMPORT int symTblAdd ();
IMPORT int symTblClassId;
IMPORT int symTblCreate ();
IMPORT int symTblDelete ();
IMPORT int symTblDestroy ();
IMPORT int symTblInit ();
IMPORT int symTblRemove ();
IMPORT int symTblTerminate ();
IMPORT int symTypeGet ();
IMPORT int symValueGet ();
IMPORT int syncLoadRtn;
IMPORT int syncSymAddRtn;
IMPORT int syncSymRemoveRtn;
IMPORT int sysAdaEnable;
IMPORT int sysAuxClkConnect ();
IMPORT int sysAuxClkDisable ();
IMPORT int sysAuxClkEnable ();
IMPORT int sysAuxClkRateGet ();
IMPORT int sysAuxClkRateSet ();
IMPORT int sysBootFile;
IMPORT int sysBootHost;
IMPORT int sysBootLine;
IMPORT int sysBootLineAdrs;
IMPORT int sysBootLineMagic;
IMPORT int sysBootLineMagicAdrs;
IMPORT int sysBootParams;
IMPORT int sysBspRev ();
IMPORT int sysBusIntAck ();
IMPORT int sysBusIntGen ();
IMPORT int sysBusTas ();
IMPORT int sysBusToLocalAdrs ();
IMPORT int sysCacheLibInit;
IMPORT int sysClkConnect ();
IMPORT int sysClkDisable ();
IMPORT int sysClkEnable ();
IMPORT int sysClkRateGet ();
IMPORT int sysClkRateSet ();
IMPORT int sysCplusEnable;
IMPORT int sysCpu;
IMPORT int sysExcMsg;
IMPORT int sysExcMsgAdrs;
IMPORT int sysFlags;
IMPORT int sysHardSqrt;
IMPORT int sysHwInit ();
IMPORT int sysHwInit2 ();
IMPORT int sysIntDisable ();
IMPORT int sysIntEnable ();
IMPORT int sysIntLvlDisableRtn;
IMPORT int sysIntLvlEnableRtn;
IMPORT int sysLocalToBusAdrs ();
IMPORT int sysMailboxConnect ();
IMPORT int sysMailboxEnable ();
IMPORT int sysMemBaseAdrs;
IMPORT int sysMemSize;
IMPORT int sysMemTop ();
IMPORT int sysModel ();
IMPORT int sysNvRamGet ();
IMPORT int sysNvRamSet ();
IMPORT int sysPhysMemTop ();
IMPORT int sysProcNum;
IMPORT int sysProcNumGet ();
IMPORT int sysProcNumSet ();
IMPORT int sysSerialChanGet ();
IMPORT int sysSerialHwInit ();
IMPORT int sysSerialHwInit2 ();
IMPORT int sysSerialReset ();
IMPORT int sysShmid;
IMPORT int sysSioChans;
IMPORT int sysSmAddr;
IMPORT int sysStartType;
IMPORT int sysSymTbl;
IMPORT int sysToMonitor ();
IMPORT int system ();
IMPORT int tan ();
IMPORT int tanh ();
IMPORT int taskActivate ();
IMPORT int taskArgsGet ();
IMPORT int taskArgsSet ();
IMPORT int taskBpHook;
IMPORT int taskBpHookSet ();
IMPORT int taskClassId;
IMPORT int taskCreat ();
IMPORT int taskCreateHookAdd ();
IMPORT int taskCreateHookDelete ();
IMPORT int taskCreateHookShow ();
IMPORT int taskCreateTable;
IMPORT int taskDelay ();
IMPORT int taskDelete ();
IMPORT int taskDeleteForce ();
IMPORT int taskDeleteHookAdd ();
IMPORT int taskDeleteHookDelete ();
IMPORT int taskDeleteHookShow ();
IMPORT int taskDeleteTable;
IMPORT int taskDestroy ();
IMPORT int taskHookInit ();
IMPORT int taskHookShowInit ();
IMPORT int taskIdCurrent;
IMPORT int taskIdDefault ();
IMPORT int taskIdFigure ();
IMPORT int taskIdListGet ();
IMPORT int taskIdListSort ();
IMPORT int taskIdSelf ();
IMPORT int taskIdVerify ();
IMPORT int taskInfoGet ();
IMPORT int taskInit ();
IMPORT int taskInstClassId;
IMPORT int taskIsReady ();
IMPORT int taskIsSuspended ();
IMPORT int taskLibInit ();
IMPORT int taskLock ();
IMPORT int taskName ();
IMPORT int taskNameToId ();
IMPORT int taskOptionsGet ();
IMPORT int taskOptionsSet ();
IMPORT int taskOptionsString ();
IMPORT int taskPriRangeCheck;
IMPORT int taskPriorityGet ();
IMPORT int taskPrioritySet ();
IMPORT int taskRegName;
IMPORT int taskRegsFmt;
IMPORT int taskRegsGet ();
IMPORT int taskRegsInit ();
IMPORT int taskRegsSet ();
IMPORT int taskRegsShow ();
IMPORT int taskRestart ();
IMPORT int taskResume ();
IMPORT int taskRtnValueSet ();
IMPORT int taskSafe ();
IMPORT int taskShow ();
IMPORT int taskShowInit ();
IMPORT int taskSpawn ();
IMPORT int taskStackAllot ();
IMPORT int taskStatusString ();
IMPORT int taskSuspend ();
IMPORT int taskSwapHookAdd ();
IMPORT int taskSwapHookAttach ();
IMPORT int taskSwapHookDelete ();
IMPORT int taskSwapHookDetach ();
IMPORT int taskSwapHookShow ();
IMPORT int taskSwapReference;
IMPORT int taskSwapTable;
IMPORT int taskSwitchHookAdd ();
IMPORT int taskSwitchHookDelete ();
IMPORT int taskSwitchHookShow ();
IMPORT int taskSwitchTable;
IMPORT int taskTcb ();
IMPORT int taskTerminate ();
IMPORT int taskUndelay ();
IMPORT int taskUnlock ();
IMPORT int taskUnsafe ();
IMPORT int taskVarAdd ();
IMPORT int taskVarDelete ();
IMPORT int taskVarGet ();
IMPORT int taskVarInfo ();
IMPORT int taskVarInit ();
IMPORT int taskVarSet ();
IMPORT int td ();
IMPORT int ti ();
IMPORT int tick64Get ();
IMPORT int tick64Set ();
IMPORT int tickAnnounce ();
IMPORT int tickGet ();
IMPORT int tickQHead;
IMPORT int tickSet ();
IMPORT int time ();
IMPORT int timeFiles;
IMPORT int timex ();
IMPORT int timexClear ();
IMPORT int timexFunc ();
IMPORT int timexHelp ();
IMPORT int timexInit ();
IMPORT int timexN ();
IMPORT int timexPost ();
IMPORT int timexPre ();
IMPORT int timexShow ();
IMPORT int tmpfile ();
IMPORT int tmpnam ();
IMPORT int tolower ();
IMPORT int toupper ();
IMPORT int tr ();
IMPORT int trcDefaultArgs;
IMPORT int trcStack ();
IMPORT int trgActionDefMsgQId;
IMPORT int trgActionDefPerform ();
IMPORT int trgActionDefStart ();
IMPORT int trgActionPerform ();
IMPORT int trgAdd ();
IMPORT int trgAddTcl ();
IMPORT int trgChainSet ();
IMPORT int trgCheck ();
IMPORT int trgClass;
IMPORT int trgClassId;
IMPORT int trgClassList;
IMPORT int trgCnt;
IMPORT int trgCondTest ();
IMPORT int trgContextMatch ();
IMPORT int trgDefSem;
IMPORT int trgDelete ();
IMPORT int trgDisable ();
IMPORT int trgEnable ();
IMPORT int trgEvent ();
IMPORT int trgEvtClass;
IMPORT int trgEvtToIndex ();
IMPORT int trgInit ();
IMPORT int trgLibInit ();
IMPORT int trgList;
IMPORT int trgNone;
IMPORT int trgOff ();
IMPORT int trgOn ();
IMPORT int trgShow ();
IMPORT int trgShowInit ();
IMPORT int trgWorkQ;
IMPORT int trgWorkQFullNotify;
IMPORT int trgWorkQReader;
IMPORT int trgWorkQReset ();
IMPORT int trgWorkQWriter;
IMPORT int ts ();
IMPORT int tsfsUploadPathClose ();
IMPORT int tsfsUploadPathCreate ();
IMPORT int tsfsUploadPathLibInit ();
IMPORT int tsfsUploadPathWrite ();
IMPORT int tt ();
IMPORT int ttyDevCreate ();
IMPORT int ttyDrv ();
IMPORT int tyAbortFuncSet ();
IMPORT int tyAbortSet ();
IMPORT int tyBackspaceChar;
IMPORT int tyBackspaceSet ();
IMPORT int tyCoDv;
IMPORT int tyDeleteLineChar;
IMPORT int tyDeleteLineSet ();
IMPORT int tyDevInit ();
IMPORT int tyDevRemove ();
IMPORT int tyEOFSet ();
IMPORT int tyEofChar;
IMPORT int tyIRd ();
IMPORT int tyITx ();
IMPORT int tyIoctl ();
IMPORT int tyMonitorTrapSet ();
IMPORT int tyRead ();
IMPORT int tyWrite ();
IMPORT int u_argvsave;
IMPORT int u_close ();
IMPORT int u_closedir ();
IMPORT int u_connect ();
IMPORT int u_cwd;
IMPORT int u_dup2 ();
IMPORT int u_errno ();
IMPORT int u_execl ();
IMPORT int u_execlp ();
IMPORT int u_execvp ();
IMPORT int u_exit ();
IMPORT int u_fcntl ();
IMPORT int u_fork ();
IMPORT int u_free ();
IMPORT int u_fstat ();
IMPORT int u_getcwd ();
IMPORT int u_getenv ();
IMPORT int u_getgid ();
IMPORT int u_gethostbyname ();
IMPORT int u_gethostname ();
IMPORT int u_getpgrp ();
IMPORT int u_getpid ();
IMPORT int u_getppid ();
IMPORT int u_getuid ();
IMPORT int u_getwd ();
IMPORT int u_hostip;
IMPORT int u_hostname;
IMPORT int u_ioctl ();
IMPORT int u_isatty ();
IMPORT int u_kill ();
IMPORT int u_localtime ();
IMPORT int u_lockf ();
IMPORT int u_lseek ();
IMPORT int u_makecontext ();
IMPORT int u_malloc ();
IMPORT int u_mkdir ();
IMPORT int u_mkfifo ();
IMPORT int u_open ();
IMPORT int u_opendir ();
IMPORT int u_printf ();
IMPORT int u_progname;
IMPORT int u_read ();
IMPORT int u_readdir ();
IMPORT int u_rename ();
IMPORT int u_rmdir ();
IMPORT int u_select ();
IMPORT int u_setitimer ();
IMPORT int u_setsid ();
IMPORT int u_shmat ();
IMPORT int u_shmctl ();
IMPORT int u_shmget ();
IMPORT int u_sigaction ();
IMPORT int u_sigaddset ();
IMPORT int u_sigaltstack ();
IMPORT int u_sigdelset ();
IMPORT int u_sigemptyset ();
IMPORT int u_sigfillset ();
IMPORT int u_sigismember ();
IMPORT int u_signal ();
IMPORT int u_sigprocmask ();
IMPORT int u_sigsuspend ();
IMPORT int u_sigsuspendflag;
IMPORT int u_socket ();
IMPORT int u_stat ();
IMPORT int u_system ();
IMPORT int u_tcgetpgrp ();
IMPORT int u_tcsetpgrp ();
IMPORT int u_time ();
IMPORT int u_uname ();
IMPORT int u_unlink ();
IMPORT int u_username;
IMPORT int u_usleep ();
IMPORT int u_wait ();
IMPORT int u_write ();
IMPORT int udpCommIfInit ();
IMPORT int udpRcv ();
IMPORT int ulDebug;
IMPORT int ungetc ();
IMPORT int unixChan;
IMPORT int unixDevInit ();
IMPORT int unixDevInit2 ();
IMPORT int unixIntRcv ();
IMPORT int unlink ();
IMPORT int usrBreakpointSet ();
IMPORT int usrClock ();
IMPORT int usrExtraModules;
IMPORT int usrInit ();
IMPORT int usrKernelInit ();
IMPORT int usrRoot ();
IMPORT int uswab ();
IMPORT int utime ();
IMPORT int valloc ();
IMPORT int version ();
IMPORT int vfdprintf ();
IMPORT int vfprintf ();
IMPORT int vmLibInfo;
IMPORT int vprintf ();
IMPORT int vsprintf ();
IMPORT int vxAbsTicks;
IMPORT int vxEventPendQ;
IMPORT int vxIntStackBase;
IMPORT int vxIntStackEnd;
IMPORT int vxMemArchProbe ();
IMPORT int vxMemProbe ();
IMPORT int vxTas ();
IMPORT int vxTaskEntry ();
IMPORT int vxTicks;
IMPORT int vxWorksVersion;
IMPORT int vxsim_bootdev;
IMPORT int vxsim_cwd;
IMPORT int vxsim_hostname;
IMPORT int vxsim_ip_name;
IMPORT int wcstombs ();
IMPORT int wctomb ();
IMPORT int wdCancel ();
IMPORT int wdClassId;
IMPORT int wdCreate ();
IMPORT int wdDelete ();
IMPORT int wdDestroy ();
IMPORT int wdInit ();
IMPORT int wdInstClassId;
IMPORT int wdLibInit ();
IMPORT int wdShow ();
IMPORT int wdShowInit ();
IMPORT int wdStart ();
IMPORT int wdTerminate ();
IMPORT int wdTick ();
IMPORT int wdbBpInstall ();
IMPORT int wdbCksum ();
IMPORT int wdbCommMtu;
IMPORT int wdbConfig ();
IMPORT int wdbConnectLibInit ();
IMPORT int wdbCtxCreate ();
IMPORT int wdbCtxExitLibInit ();
IMPORT int wdbCtxExitNotifyHook ();
IMPORT int wdbCtxLibInit ();
IMPORT int wdbCtxResume ();
IMPORT int wdbCtxStartLibInit ();
IMPORT int wdbDbgArchInit ();
IMPORT int wdbDbgBpFind ();
IMPORT int wdbDbgBpGet ();
IMPORT int wdbDbgBpListInit ();
IMPORT int wdbDbgBpRemove ();
IMPORT int wdbDbgBpRemoveAll ();
IMPORT int wdbDbgGetNpc ();
IMPORT int wdbDbgTraceModeClear ();
IMPORT int wdbDbgTraceModeSet ();
IMPORT int wdbDbgTrap ();
IMPORT int wdbDirectCallLibInit ();
IMPORT int wdbE ();
IMPORT int wdbEventDeq ();
IMPORT int wdbEventLibInit ();
IMPORT int wdbEventNodeInit ();
IMPORT int wdbEventPost ();
IMPORT int wdbEvtptClassConnect ();
IMPORT int wdbEvtptLibInit ();
IMPORT int wdbExcLibInit ();
IMPORT int wdbExternEnterHook ();
IMPORT int wdbExternExitHook ();
IMPORT int wdbExternInit ();
IMPORT int wdbExternRegSetObjAdd ();
IMPORT int wdbExternRegsGet ();
IMPORT int wdbExternRegsSet ();
IMPORT int wdbExternSystemRegs;
IMPORT int wdbFpLibInit ();
IMPORT int wdbFppGet ();
IMPORT int wdbFppRestore ();
IMPORT int wdbFppSave ();
IMPORT int wdbFppSet ();
IMPORT int wdbFuncCallLibInit ();
IMPORT int wdbGopherLibInit ();
IMPORT int wdbGopherLock;
IMPORT int wdbInfoGet ();
IMPORT int wdbInstallCommIf ();
IMPORT int wdbInstallRtIf ();
IMPORT int wdbIsNowExternal ();
IMPORT int wdbIsNowTasking ();
IMPORT int wdbMbufAlloc ();
IMPORT int wdbMbufFree ();
IMPORT int wdbMemCoreLibInit ();
IMPORT int wdbMemLibInit ();
IMPORT int wdbMemTest ();
IMPORT int wdbModeSet ();
IMPORT int wdbNotifyHost ();
IMPORT int wdbNumMemRegions;
IMPORT int wdbOneShot;
IMPORT int wdbPipePktDevInit ();
IMPORT int wdbRegsLibInit ();
IMPORT int wdbResumeSystem ();
IMPORT int wdbRpcGetArgs ();
IMPORT int wdbRpcNotifyConnect ();
IMPORT int wdbRpcNotifyHost ();
IMPORT int wdbRpcRcv ();
IMPORT int wdbRpcReply ();
IMPORT int wdbRpcReplyErr ();
IMPORT int wdbRpcResendReply ();
IMPORT int wdbRpcXportInit ();
IMPORT int wdbRunsExternal ();
IMPORT int wdbRunsTasking ();
IMPORT int wdbSp ();
IMPORT int wdbSuspendSystem ();
IMPORT int wdbSuspendSystemHere ();
IMPORT int wdbSvcAdd ();
IMPORT int wdbSvcDispatch ();
IMPORT int wdbSvcDsaSvcRemove ();
IMPORT int wdbSvcHookAdd ();
IMPORT int wdbSvcLibInit ();
IMPORT int wdbSysBpLibInit ();
IMPORT int wdbSystemSuspend ();
IMPORT int wdbTargetIsConnected ();
IMPORT int wdbTask ();
IMPORT int wdbTaskBpLibInit ();
IMPORT int wdbTaskId;
IMPORT int wdbTaskInit ();
IMPORT int wdbToolName;
IMPORT int wdbTsfsDefaultDirPerm;
IMPORT int wdbTsfsDrv ();
IMPORT int wdbUserEvtLibInit ();
IMPORT int wdbUserEvtPost ();
IMPORT int wdbVioChannelRegister ();
IMPORT int wdbVioChannelUnregister ();
IMPORT int wdbVioDrv ();
IMPORT int wdbVioLibInit ();
IMPORT int wim ();
IMPORT int windDelay ();
IMPORT int windDelete ();
IMPORT int windExit ();
IMPORT int windIntStackSet ();
IMPORT int windLoadContext ();
IMPORT int windPendQFlush ();
IMPORT int windPendQGet ();
IMPORT int windPendQPut ();
IMPORT int windPendQRemove ();
IMPORT int windPendQTerminate ();
IMPORT int windPriNormalSet ();
IMPORT int windPrioritySet ();
IMPORT int windReadyQPut ();
IMPORT int windReadyQRemove ();
IMPORT int windResume ();
IMPORT int windSemDelete ();
IMPORT int windSpawn ();
IMPORT int windSuspend ();
IMPORT int windTickAnnounce ();
IMPORT int windUndelay ();
IMPORT int windWdCancel ();
IMPORT int windWdStart ();
IMPORT int windviewConfig ();
IMPORT int workQAdd0 ();
IMPORT int workQAdd1 ();
IMPORT int workQAdd2 ();
IMPORT int workQDoWork ();
IMPORT int workQInit ();
IMPORT int workQIsEmpty;
IMPORT int workQPanic ();
IMPORT int workQReadIx;
IMPORT int workQWriteIx;
IMPORT int write ();
IMPORT int writev ();
IMPORT int wvBufId;
IMPORT int wvDefaultBufMax;
IMPORT int wvDefaultBufMin;
IMPORT int wvDefaultBufOptions;
IMPORT int wvDefaultBufSize;
IMPORT int wvDefaultBufThresh;
IMPORT int wvDefaultRBuffParams;
IMPORT int wvEvent ();
IMPORT int wvEventInst ();
IMPORT int wvEvtBufferFullNotify;
IMPORT int wvEvtBufferGet ();
IMPORT int wvEvtClass;
IMPORT int wvEvtClassClear ();
IMPORT int wvEvtClassClearAll ();
IMPORT int wvEvtClassGet ();
IMPORT int wvEvtClassSet ();
IMPORT int wvEvtLogInit ();
IMPORT int wvEvtLogStart ();
IMPORT int wvEvtLogStop ();
IMPORT int wvInstIsOn;
IMPORT int wvLibInit ();
IMPORT int wvLibInit2 ();
IMPORT int wvLogHeader;
IMPORT int wvLogHeaderCreate ();
IMPORT int wvLogHeaderUpload ();
IMPORT int wvObjInst ();
IMPORT int wvObjInstModeSet ();
IMPORT int wvObjIsEnabled;
IMPORT int wvOff ();
IMPORT int wvOn ();
IMPORT int wvRBuffErrorHandler ();
IMPORT int wvRBuffMgr ();
IMPORT int wvRBuffMgrId;
IMPORT int wvRBuffMgrPrioritySet ();
IMPORT int wvSigInst ();
IMPORT int wvTaskNamesBufAdd ();
IMPORT int wvTaskNamesPreserve ();
IMPORT int wvTaskNamesUpload ();
IMPORT int wvTmrRegister ();
IMPORT int wvUpPathId;
IMPORT int wvUpTaskId;
IMPORT int wvUploadMaxAttempts;
IMPORT int wvUploadRetryBackoff;
IMPORT int wvUploadStart ();
IMPORT int wvUploadStop ();
IMPORT int wvUploadTaskConfig ();
IMPORT int wvUploadTaskOptions;
IMPORT int wvUploadTaskPriority;
IMPORT int wvUploadTaskStackSize;
IMPORT int xattrib ();
IMPORT int xcopy ();
IMPORT int xdelete ();
IMPORT int xdrCksum ();
IMPORT int xdr_ARRAY ();
IMPORT int xdr_CHECKSUM ();
IMPORT int xdr_TGT_ADDR_T ();
IMPORT int xdr_TGT_INT_T ();
IMPORT int xdr_UINT32 ();
IMPORT int xdr_WDB_AGENT_INFO ();
IMPORT int xdr_WDB_CALL_RETURN_INFO ();
IMPORT int xdr_WDB_CTX ();
IMPORT int xdr_WDB_CTX_CREATE_DESC ();
IMPORT int xdr_WDB_CTX_STEP_DESC ();
IMPORT int xdr_WDB_EVTPT_ADD_DESC ();
IMPORT int xdr_WDB_EVTPT_DEL_DESC ();
IMPORT int xdr_WDB_EVT_DATA ();
IMPORT int xdr_WDB_EVT_INFO ();
IMPORT int xdr_WDB_MEM_REGION ();
IMPORT int xdr_WDB_MEM_SCAN_DESC ();
IMPORT int xdr_WDB_MEM_XFER ();
IMPORT int xdr_WDB_OPQ_DATA_T ();
IMPORT int xdr_WDB_PARAM_WRAPPER ();
IMPORT int xdr_WDB_REG_READ_DESC ();
IMPORT int xdr_WDB_REG_WRITE_DESC ();
IMPORT int xdr_WDB_REPLY_WRAPPER ();
IMPORT int xdr_WDB_RT_INFO ();
IMPORT int xdr_WDB_STRING_T ();
IMPORT int xdr_WDB_TGT_INFO ();
IMPORT int xdr_bool ();
IMPORT int xdr_bytes ();
IMPORT int xdr_char ();
IMPORT int xdr_double ();
IMPORT int xdr_enum ();
IMPORT int xdr_float ();
IMPORT int xdr_floatInclude ();
IMPORT int xdr_free ();
IMPORT int xdr_int ();
IMPORT int xdr_long ();
IMPORT int xdr_netobj ();
IMPORT int xdr_opaque ();
IMPORT int xdr_short ();
IMPORT int xdr_string ();
IMPORT int xdr_u_char ();
IMPORT int xdr_u_int ();
IMPORT int xdr_u_long ();
IMPORT int xdr_u_short ();
IMPORT int xdr_union ();
IMPORT int xdr_void ();
IMPORT int xdr_wrapstring ();
IMPORT int xdrmem_create ();
IMPORT int y ();
IMPORT int yy_yys;
IMPORT int yy_yyv;
IMPORT int yychar;
IMPORT int yydebug;
IMPORT int yyerrflag;
IMPORT int yylval;
IMPORT int yynerrs;
IMPORT int yyparse ();
IMPORT int yyps;
IMPORT int yypv;
IMPORT int yys;
IMPORT int yystart ();
IMPORT int yystate;
IMPORT int yytmp;
IMPORT int yyv;
IMPORT int yyval;

SYMBOL standTbl [2782] =
    {
        {{NULL}, "ABORT", (char*) ABORT, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "ACCESS", (char*) ACCESS, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "ALARM", (char*) ALARM, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "ALTZONE", (char*) ALTZONE, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "ASCTIME", (char*) ASCTIME, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "ASCTIME_R", (char*) ASCTIME_R, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "ATEXIT", (char*) ATEXIT, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "ATOI", (char*) ATOI, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "BCOPY", (char*) BCOPY, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "BRK", (char*) BRK, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "BZERO", (char*) BZERO, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "CALLOC", (char*) CALLOC, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "CHARMAP", (char*) &CHARMAP, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "CLOSE", (char*) CLOSE, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "CLOSEDIR", (char*) CLOSEDIR, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "CLOSELOG", (char*) CLOSELOG, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "CLOSE_FD", (char*) CLOSE_FD, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "COND_BROADCAST", (char*) COND_BROADCAST, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "COND_DESTROY", (char*) COND_DESTROY, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "COND_INIT", (char*) COND_INIT, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "COND_SIGNAL", (char*) COND_SIGNAL, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "COND_TIMEDWAIT", (char*) COND_TIMEDWAIT, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "COND_WAIT", (char*) COND_WAIT, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "CREAT", (char*) CREAT, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "CTIME", (char*) CTIME, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "CTIME_R", (char*) CTIME_R, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "DAYLIGHT", (char*) DAYLIGHT, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "DECIMAL_TO_DOUBLE", (char*) DECIMAL_TO_DOUBLE, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "DECIMAL_TO_EXTENDED", (char*) DECIMAL_TO_EXTENDED, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "DECIMAL_TO_QUADRUPLE", (char*) DECIMAL_TO_QUADRUPLE, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "DECIMAL_TO_SINGLE", (char*) DECIMAL_TO_SINGLE, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "DIFFTIME", (char*) DIFFTIME, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "DOUBLE_TO_DECIMAL", (char*) DOUBLE_TO_DECIMAL, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "DUP2", (char*) DUP2, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "ECONVERT", (char*) ECONVERT, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "ENVIRON", (char*) ENVIRON, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "ERRNO", (char*) &ERRNO, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "EXECL", (char*) EXECL, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "EXECLP", (char*) EXECLP, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "EXECV", (char*) EXECV, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "EXECVE", (char*) EXECVE, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "EXECVP", (char*) EXECVP, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "EXIT", (char*) EXIT, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "FCHMOD", (char*) FCHMOD, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "FCLOSE", (char*) FCLOSE, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "FCNTL", (char*) FCNTL, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "FCONVERT", (char*) FCONVERT, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "FDOPEN", (char*) FDOPEN, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "FEOF", (char*) FEOF, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "FEOF_UNLOCKED", (char*) FEOF_UNLOCKED, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "FERROR", (char*) FERROR, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "FERROR_UNLOCKED", (char*) FERROR_UNLOCKED, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "FFLUSH", (char*) FFLUSH, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "FFS", (char*) FFS, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "FGETS", (char*) FGETS, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "FILENO", (char*) FILENO, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "FILENO_UNLOCKED", (char*) FILENO_UNLOCKED, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "FILE_TO_DECIMAL", (char*) FILE_TO_DECIMAL, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "FLOCKFILE", (char*) FLOCKFILE, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "FOPEN", (char*) FOPEN, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "FORK", (char*) FORK, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "FORK1", (char*) FORK1, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "FPGETMASK", (char*) FPGETMASK, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "FPRINTF", (char*) FPRINTF, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "FPUTS", (char*) FPUTS, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "FREAD", (char*) FREAD, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "FREE", (char*) FREE, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "FREOPEN", (char*) FREOPEN, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "FSCANF", (char*) FSCANF, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "FSEEK", (char*) FSEEK, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "FSTAT", (char*) FSTAT, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "FSYNC", (char*) FSYNC, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "FTELL", (char*) FTELL, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "FTRYLOCKFILE", (char*) FTRYLOCKFILE, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "FUNLOCKFILE", (char*) FUNLOCKFILE, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "FWRITE", (char*) FWRITE, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "GETC", (char*) GETC, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "GETCWD", (char*) GETCWD, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "GETC_UNLOCKED", (char*) GETC_UNLOCKED, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "GETDENTS", (char*) GETDENTS, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "GETEGID", (char*) GETEGID, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "GETENV", (char*) GETENV, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "GETEUID", (char*) GETEUID, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "GETGID", (char*) GETGID, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "GETGROUPS", (char*) GETGROUPS, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "GETMNTANY", (char*) GETMNTANY, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "GETMNTENT", (char*) GETMNTENT, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "GETMSG", (char*) GETMSG, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "GETPGID", (char*) GETPGID, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "GETPGRP", (char*) GETPGRP, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "GETPID", (char*) GETPID, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "GETPPID", (char*) GETPPID, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "GETRLIMIT", (char*) GETRLIMIT, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "GETSID", (char*) GETSID, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "GETTIMEOFDAY", (char*) GETTIMEOFDAY, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "GETUID", (char*) GETUID, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "GETWIDTH", (char*) GETWIDTH, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "GMTIME", (char*) GMTIME, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "GMTIME_R", (char*) GMTIME_R, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "HASMNTOPT", (char*) HASMNTOPT, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "IEEE_HANDLERS", (char*) &IEEE_HANDLERS, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "IOCTL", (char*) IOCTL, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "ISALNUM", (char*) ISALNUM, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "ISALPHA", (char*) ISALPHA, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "ISASCII", (char*) ISASCII, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "ISATTY", (char*) ISATTY, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "ISCNTRL", (char*) ISCNTRL, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "ISDIGIT", (char*) ISDIGIT, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "ISGRAPH", (char*) ISGRAPH, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "ISLOWER", (char*) ISLOWER, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "ISPRINT", (char*) ISPRINT, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "ISPUNCT", (char*) ISPUNCT, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "ISSPACE", (char*) ISSPACE, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "ISUPPER", (char*) ISUPPER, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "ISXDIGIT", (char*) ISXDIGIT, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "KILL", (char*) KILL, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "LOCALECONV", (char*) LOCALECONV, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "LOCALTIME", (char*) LOCALTIME, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "LOCALTIME_R", (char*) LOCALTIME_R, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "LOCKF", (char*) LOCKF, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "LONGJMP", (char*) LONGJMP, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "LSEEK", (char*) LSEEK, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "LSTAT", (char*) LSTAT, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "MAKECONTEXT", (char*) MAKECONTEXT, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "MALLOC", (char*) MALLOC, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "MBLEN", (char*) MBLEN, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "MBSTOWCS", (char*) MBSTOWCS, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "MBTOWC", (char*) MBTOWC, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "MEMCCPY", (char*) MEMCCPY, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "MEMCHR", (char*) MEMCHR, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "MEMCMP", (char*) MEMCMP, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "MEMCNTL", (char*) MEMCNTL, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "MEMCPY", (char*) MEMCPY, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "MEMMOVE", (char*) MEMMOVE, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "MEMSET", (char*) MEMSET, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "MKDIR", (char*) MKDIR, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "MKFIFO", (char*) MKFIFO, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "MKNOD", (char*) MKNOD, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "MKTEMP", (char*) MKTEMP, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "MKTIME", (char*) MKTIME, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "MMAP", (char*) MMAP, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "MNTOPT", (char*) MNTOPT, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "MSYNC", (char*) MSYNC, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "MUNMAP", (char*) MUNMAP, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "MUTEX_DESTROY", (char*) MUTEX_DESTROY, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "MUTEX_HELD", (char*) MUTEX_HELD, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "MUTEX_INIT", (char*) MUTEX_INIT, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "MUTEX_LOCK", (char*) MUTEX_LOCK, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "MUTEX_TRYLOCK", (char*) MUTEX_TRYLOCK, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "MUTEX_UNLOCK", (char*) MUTEX_UNLOCK, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "NSS_DEFAULT_FINDERS", (char*) NSS_DEFAULT_FINDERS, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "NSS_DELETE", (char*) NSS_DELETE, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "NSS_ENDENT", (char*) NSS_ENDENT, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "NSS_GETENT", (char*) NSS_GETENT, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "NSS_SEARCH", (char*) NSS_SEARCH, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "NSS_SETENT", (char*) NSS_SETENT, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "OPEN", (char*) OPEN, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "OPENDIR", (char*) OPENDIR, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "OPENLOG", (char*) OPENLOG, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "PATHCONF", (char*) PATHCONF, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "PAUSE", (char*) PAUSE, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "PERROR", (char*) PERROR, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "PIPE", (char*) PIPE, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "POLL", (char*) POLL, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "PRINTF", (char*) PRINTF, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "PTHREAD_CONDATTR_DESTROY", (char*) PTHREAD_CONDATTR_DESTROY, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "PTHREAD_CONDATTR_GETPSHARED", (char*) PTHREAD_CONDATTR_GETPSHARED, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "PTHREAD_CONDATTR_INIT", (char*) PTHREAD_CONDATTR_INIT, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "PTHREAD_CONDATTR_SETPSHARED", (char*) PTHREAD_CONDATTR_SETPSHARED, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "PTHREAD_COND_BROADCAST", (char*) PTHREAD_COND_BROADCAST, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "PTHREAD_COND_DESTROY", (char*) PTHREAD_COND_DESTROY, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "PTHREAD_COND_INIT", (char*) PTHREAD_COND_INIT, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "PTHREAD_COND_SIGNAL", (char*) PTHREAD_COND_SIGNAL, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "PTHREAD_COND_TIMEDWAIT", (char*) PTHREAD_COND_TIMEDWAIT, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "PTHREAD_COND_WAIT", (char*) PTHREAD_COND_WAIT, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "PTHREAD_MUTEXATTR_DESTROY", (char*) PTHREAD_MUTEXATTR_DESTROY, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "PTHREAD_MUTEXATTR_GETPRIOCEILING", (char*) PTHREAD_MUTEXATTR_GETPRIOCEILING, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "PTHREAD_MUTEXATTR_GETPROTOCOL", (char*) PTHREAD_MUTEXATTR_GETPROTOCOL, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "PTHREAD_MUTEXATTR_GETPSHARED", (char*) PTHREAD_MUTEXATTR_GETPSHARED, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "PTHREAD_MUTEXATTR_INIT", (char*) PTHREAD_MUTEXATTR_INIT, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "PTHREAD_MUTEXATTR_SETPRIOCEILING", (char*) PTHREAD_MUTEXATTR_SETPRIOCEILING, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "PTHREAD_MUTEXATTR_SETPROTOCOL", (char*) PTHREAD_MUTEXATTR_SETPROTOCOL, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "PTHREAD_MUTEXATTR_SETPSHARED", (char*) PTHREAD_MUTEXATTR_SETPSHARED, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "PTHREAD_MUTEX_DESTROY", (char*) PTHREAD_MUTEX_DESTROY, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "PTHREAD_MUTEX_GETPRIOCEILING", (char*) PTHREAD_MUTEX_GETPRIOCEILING, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "PTHREAD_MUTEX_INIT", (char*) PTHREAD_MUTEX_INIT, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "PTHREAD_MUTEX_LOCK", (char*) PTHREAD_MUTEX_LOCK, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "PTHREAD_MUTEX_SETPRIOCEILING", (char*) PTHREAD_MUTEX_SETPRIOCEILING, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "PTHREAD_MUTEX_TRYLOCK", (char*) PTHREAD_MUTEX_TRYLOCK, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "PTHREAD_MUTEX_UNLOCK", (char*) PTHREAD_MUTEX_UNLOCK, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "PUTC", (char*) PUTC, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "PUTC_UNLOCKED", (char*) PUTC_UNLOCKED, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "PUTMSG", (char*) PUTMSG, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "PUTPMSG", (char*) PUTPMSG, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "QECONVERT", (char*) QECONVERT, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "QFCONVERT", (char*) QFCONVERT, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "QGCONVERT", (char*) QGCONVERT, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "QSORT", (char*) QSORT, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "QUADRUPLE_TO_DECIMAL", (char*) QUADRUPLE_TO_DECIMAL, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "RAISE", (char*) RAISE, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "READ", (char*) READ, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "READDIR", (char*) READDIR, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "READV", (char*) READV, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "REALLOC", (char*) REALLOC, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "REMOVE", (char*) REMOVE, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "RENAME", (char*) RENAME, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "REWIND", (char*) REWIND, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "RMDIR", (char*) RMDIR, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "RWLOCK_INIT", (char*) RWLOCK_INIT, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "RW_RDLOCK", (char*) RW_RDLOCK, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "RW_READ_HELD", (char*) RW_READ_HELD, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "RW_TRYRDLOCK", (char*) RW_TRYRDLOCK, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "RW_TRYWRLOCK", (char*) RW_TRYWRLOCK, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "RW_UNLOCK", (char*) RW_UNLOCK, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "RW_WRITE_HELD", (char*) RW_WRITE_HELD, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "RW_WRLOCK", (char*) RW_WRLOCK, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "SBRK", (char*) SBRK, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "SCANF", (char*) SCANF, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "SCRWIDTH", (char*) SCRWIDTH, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "SELECT", (char*) SELECT, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "SEMA_HELD", (char*) SEMA_HELD, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "SEMA_INIT", (char*) SEMA_INIT, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "SEMA_POST", (char*) SEMA_POST, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "SEMA_TRYWAIT", (char*) SEMA_TRYWAIT, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "SEMA_WAIT", (char*) SEMA_WAIT, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "SEMCTL", (char*) SEMCTL, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "SEMGET", (char*) SEMGET, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "SEMOP", (char*) SEMOP, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "SETCONTEXT", (char*) SETCONTEXT, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "SETITIMER", (char*) SETITIMER, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "SETJMP", (char*) SETJMP, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "SETLOCALE", (char*) SETLOCALE, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "SETLOGMASK", (char*) SETLOGMASK, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "SETPGID", (char*) SETPGID, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "SETPGRP", (char*) SETPGRP, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "SETSID", (char*) SETSID, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "SHMAT", (char*) SHMAT, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "SHMCTL", (char*) SHMCTL, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "SHMDT", (char*) SHMDT, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "SHMGET", (char*) SHMGET, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "SIGACTION", (char*) SIGACTION, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "SIGADDSET", (char*) SIGADDSET, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "SIGALTSTACK", (char*) SIGALTSTACK, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "SIGDELSET", (char*) SIGDELSET, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "SIGEMPTYSET", (char*) SIGEMPTYSET, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "SIGFILLSET", (char*) SIGFILLSET, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "SIGHOLD", (char*) SIGHOLD, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "SIGIGNORE", (char*) SIGIGNORE, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "SIGISMEMBER", (char*) SIGISMEMBER, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "SIGLONGJMP", (char*) SIGLONGJMP, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "SIGNAL", (char*) SIGNAL, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "SIGPAUSE", (char*) SIGPAUSE, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "SIGPENDING", (char*) SIGPENDING, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "SIGPROCMASK", (char*) SIGPROCMASK, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "SIGRELSE", (char*) SIGRELSE, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "SIGSET", (char*) SIGSET, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "SIGSETJMP", (char*) SIGSETJMP, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "SIGSUSPEND", (char*) SIGSUSPEND, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "SIGWAIT", (char*) SIGWAIT, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "SLEEP", (char*) SLEEP, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "SPRINTF", (char*) SPRINTF, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "SSCANF", (char*) SSCANF, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "STAT", (char*) STAT, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "STRCASECMP", (char*) STRCASECMP, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "STRCAT", (char*) STRCAT, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "STRCHR", (char*) STRCHR, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "STRCMP", (char*) STRCMP, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "STRCPY", (char*) STRCPY, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "STRCSPN", (char*) STRCSPN, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "STRDUP", (char*) STRDUP, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "STRERROR", (char*) STRERROR, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "STRLEN", (char*) STRLEN, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "STRNCASECMP", (char*) STRNCASECMP, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "STRNCAT", (char*) STRNCAT, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "STRNCMP", (char*) STRNCMP, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "STRNCPY", (char*) STRNCPY, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "STRPBRK", (char*) STRPBRK, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "STRRCHR", (char*) STRRCHR, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "STRSPN", (char*) STRSPN, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "STRSTR", (char*) STRSTR, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "STRTOK", (char*) STRTOK, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "STRTOK_R", (char*) STRTOK_R, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "STRTOL", (char*) STRTOL, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "SYSCALL", (char*) SYSCALL, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "SYSCONF", (char*) SYSCONF, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "SYSINFO", (char*) SYSINFO, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "SYSLOG", (char*) SYSLOG, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "SYSTEM", (char*) SYSTEM, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "SYS_ERRLIST", (char*) SYS_ERRLIST, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "SYS_NERR", (char*) SYS_NERR, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "TCDRAIN", (char*) TCDRAIN, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "TCGETPGRP", (char*) TCGETPGRP, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "TCGETSID", (char*) TCGETSID, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "TEMPNAM", (char*) TEMPNAM, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "THR_CONTINUE", (char*) THR_CONTINUE, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "THR_CREATE", (char*) THR_CREATE, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "THR_ERRNOP", (char*) THR_ERRNOP, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "THR_EXIT", (char*) THR_EXIT, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "THR_GETCONCURRENCY", (char*) THR_GETCONCURRENCY, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "THR_GETPRIO", (char*) THR_GETPRIO, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "THR_GETSPECIFIC", (char*) THR_GETSPECIFIC, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "THR_JOIN", (char*) THR_JOIN, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "THR_KEYCREATE", (char*) THR_KEYCREATE, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "THR_KILL", (char*) THR_KILL, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "THR_MAIN", (char*) THR_MAIN, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "THR_MIN_STACK", (char*) THR_MIN_STACK, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "THR_SELF", (char*) THR_SELF, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "THR_SETCONCURRENCY", (char*) THR_SETCONCURRENCY, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "THR_SETPRIO", (char*) THR_SETPRIO, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "THR_SETSPECIFIC", (char*) THR_SETSPECIFIC, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "THR_SIGSETMASK", (char*) THR_SIGSETMASK, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "THR_STKSEGMENT", (char*) THR_STKSEGMENT, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "THR_SUSPEND", (char*) THR_SUSPEND, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "THR_YIELD", (char*) THR_YIELD, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "TIME", (char*) TIME, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "TIMEZONE", (char*) TIMEZONE, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "TOASCII", (char*) TOASCII, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "TOLOWER", (char*) TOLOWER, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "TZNAME", (char*) TZNAME, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "TZSET", (char*) TZSET, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "ULIMIT", (char*) ULIMIT, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "UNAME", (char*) UNAME, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "UNGETC", (char*) UNGETC, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "UNLINK", (char*) UNLINK, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "USLEEP", (char*) USLEEP, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "VFORK", (char*) VFORK, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "VSYSLOG", (char*) VSYSLOG, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "WAIT", (char*) WAIT, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "WAITID", (char*) WAITID, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "WAITPID", (char*) WAITPID, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "WCSTOMBS", (char*) WCSTOMBS, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "WCTOMB", (char*) WCTOMB, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "WRITE", (char*) WRITE, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "WRITEV", (char*) WRITEV, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_ACCESS", (char*) _ACCESS, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_ALARM", (char*) _ALARM, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_ALTZONE", (char*) &_ALTZONE, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "_ASCTIME_R", (char*) _ASCTIME_R, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_ASSERT", (char*) _ASSERT, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_BRK", (char*) _BRK, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_BRK_UNLOCKED", (char*) _BRK_UNLOCKED, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_BUFENDTAB", (char*) &_BUFENDTAB, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "_BUFSYNC", (char*) _BUFSYNC, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_CERROR", (char*) _CERROR, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_CERROR64", (char*) _CERROR64, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_CHECK_THREADED", (char*) _CHECK_THREADED, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_CLEANUP", (char*) _CLEANUP, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_CLOSE", (char*) _CLOSE, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_CLOSEDIR", (char*) _CLOSEDIR, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_CLOSELOG", (char*) _CLOSELOG, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_COND_BROADCAST", (char*) _COND_BROADCAST, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_COND_DESTROY", (char*) _COND_DESTROY, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_COND_INIT", (char*) _COND_INIT, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_COND_SIGNAL", (char*) _COND_SIGNAL, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_COND_TIMEDWAIT", (char*) _COND_TIMEDWAIT, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_COND_WAIT", (char*) _COND_WAIT, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_CREAT", (char*) _CREAT, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_CSWIDTH", (char*) &_CSWIDTH, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "_CTIME_R", (char*) _CTIME_R, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_CTYPE", (char*) _CTYPE, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_CUR_LOCALE", (char*) &_CUR_LOCALE, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "_DAYLIGHT", (char*) &_DAYLIGHT, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "_DECIMAL_TO_DOUBLE", (char*) _DECIMAL_TO_DOUBLE, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_DECIMAL_TO_EXTENDED", (char*) _DECIMAL_TO_EXTENDED, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_DECIMAL_TO_QUADRUPLE", (char*) _DECIMAL_TO_QUADRUPLE, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_DECIMAL_TO_SINGLE", (char*) _DECIMAL_TO_SINGLE, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_DGETTEXT", (char*) _DGETTEXT, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_DIRENT_LOCK", (char*) &_DIRENT_LOCK, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "_DOPRNT", (char*) _DOPRNT, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_DOSCAN", (char*) _DOSCAN, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_DOUBLE_TO_DECIMAL", (char*) _DOUBLE_TO_DECIMAL, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_DUP2", (char*) _DUP2, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_ECONVERT", (char*) _ECONVERT, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_ENVIRON", (char*) &_ENVIRON, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "_EUCWIDTH", (char*) &_EUCWIDTH, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "_EXECL", (char*) _EXECL, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_EXECLP", (char*) _EXECLP, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_EXECV", (char*) _EXECV, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_EXECVE", (char*) _EXECVE, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_EXECVP", (char*) _EXECVP, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_EXITHANDLE", (char*) _EXITHANDLE, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_FCHMOD", (char*) _FCHMOD, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_FCNTL", (char*) _FCNTL, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_FCONVERT", (char*) _FCONVERT, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_FDOPEN", (char*) _FDOPEN, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_FEOF_UNLOCKED", (char*) _FEOF_UNLOCKED, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_FERROR_UNLOCKED", (char*) _FERROR_UNLOCKED, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_FFLUSH_U", (char*) _FFLUSH_U, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_FFS", (char*) _FFS, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_FILBUF", (char*) _FILBUF, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_FILENO", (char*) _FILENO, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_FILENO_UNLOCKED", (char*) _FILENO_UNLOCKED, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_FILE_TO_DECIMAL", (char*) _FILE_TO_DECIMAL, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_FINDBUF", (char*) _FINDBUF, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_FINDIOP", (char*) _FINDIOP, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_FLOCKFILE", (char*) _FLOCKFILE, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_FLOCKGET", (char*) _FLOCKGET, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_FLOCKREL", (char*) _FLOCKREL, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_FLSBUF", (char*) _FLSBUF, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_FLUSHLBF", (char*) _FLUSHLBF, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_FORK", (char*) _FORK, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_FORK1", (char*) _FORK1, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_FPGETMASK", (char*) _FPGETMASK, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_FPRINTF", (char*) _FPRINTF, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_FP_ADD", (char*) _FP_ADD, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_FP_COMPARE", (char*) _FP_COMPARE, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_FP_CURRENT_DIRECTION", (char*) &_FP_CURRENT_DIRECTION, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "_FP_CURRENT_EXCEPTIONS", (char*) &_FP_CURRENT_EXCEPTIONS, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "_FP_CURRENT_PRECISION", (char*) &_FP_CURRENT_PRECISION, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "_FP_PACK", (char*) _FP_PACK, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_FP_SUB", (char*) _FP_SUB, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_FP_UNPACK", (char*) _FP_UNPACK, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_FREE_UNLOCKED", (char*) _FREE_UNLOCKED, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_FSTAT", (char*) _FSTAT, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_FSYNC", (char*) _FSYNC, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_FTRYLOCKFILE", (char*) _FTRYLOCKFILE, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_FULLOCALE", (char*) _FULLOCALE, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_FUNLOCKFILE", (char*) _FUNLOCKFILE, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_FWRITE_UNLOCKED", (char*) _FWRITE_UNLOCKED, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_GETARG", (char*) _GETARG, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_GETCWD", (char*) _GETCWD, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_GETC_UNLOCKED", (char*) _GETC_UNLOCKED, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_GETDENTS", (char*) _GETDENTS, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_GETEGID", (char*) _GETEGID, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_GETEUID", (char*) _GETEUID, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_GETGID", (char*) _GETGID, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_GETGROUPS", (char*) _GETGROUPS, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_GETMNTANY", (char*) _GETMNTANY, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_GETMNTENT", (char*) _GETMNTENT, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_GETMSG", (char*) _GETMSG, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_GETPGID", (char*) _GETPGID, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_GETPGRP", (char*) _GETPGRP, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_GETPID", (char*) _GETPID, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_GETPPID", (char*) _GETPPID, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_GETRLIMIT", (char*) _GETRLIMIT, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_GETSID", (char*) _GETSID, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_GETSP", (char*) _GETSP, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_GETTIMEOFDAY", (char*) _GETTIMEOFDAY, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_GETUID", (char*) _GETUID, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_GMTIME_R", (char*) _GMTIME_R, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_HASMNTOPT", (char*) _HASMNTOPT, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_IOB", (char*) _IOB, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_IOCTL", (char*) _IOCTL, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_ISASCII", (char*) _ISASCII, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_ISATTY", (char*) _ISATTY, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_KILL", (char*) _KILL, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_LASTBUF", (char*) &_LASTBUF, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "_LFLAG", (char*) &_LFLAG, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "_LIBC_ALARM", (char*) _LIBC_ALARM, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_LIBC_CHILD_ATFORK", (char*) _LIBC_CHILD_ATFORK, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_LIBC_CLOSE", (char*) _LIBC_CLOSE, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_LIBC_CREAT", (char*) _LIBC_CREAT, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_LIBC_FCNTL", (char*) _LIBC_FCNTL, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_LIBC_FORK", (char*) _LIBC_FORK, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_LIBC_FORK1", (char*) _LIBC_FORK1, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_LIBC_FSYNC", (char*) _LIBC_FSYNC, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_LIBC_KILL", (char*) _LIBC_KILL, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_LIBC_MSYNC", (char*) _LIBC_MSYNC, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_LIBC_NANOSLEEP", (char*) _LIBC_NANOSLEEP, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_LIBC_OPEN", (char*) _LIBC_OPEN, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_LIBC_PARENT_ATFORK", (char*) _LIBC_PARENT_ATFORK, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_LIBC_PAUSE", (char*) _LIBC_PAUSE, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_LIBC_PREPARE_ATFORK", (char*) _LIBC_PREPARE_ATFORK, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_LIBC_READ", (char*) _LIBC_READ, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_LIBC_SETITIMER", (char*) _LIBC_SETITIMER, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_LIBC_SET_THREADED", (char*) _LIBC_SET_THREADED, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_LIBC_SIGACTION", (char*) _LIBC_SIGACTION, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_LIBC_SIGLONGJMP", (char*) _LIBC_SIGLONGJMP, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_LIBC_SIGPENDING", (char*) _LIBC_SIGPENDING, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_LIBC_SIGPROCMASK", (char*) _LIBC_SIGPROCMASK, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_LIBC_SIGSUSPEND", (char*) _LIBC_SIGSUSPEND, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_LIBC_SIGTIMEDWAIT", (char*) _LIBC_SIGTIMEDWAIT, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_LIBC_SIGWAIT", (char*) _LIBC_SIGWAIT, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_LIBC_SLEEP", (char*) _LIBC_SLEEP, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_LIBC_TCDRAIN", (char*) _LIBC_TCDRAIN, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_LIBC_THREADS_INTERFACE", (char*) _LIBC_THREADS_INTERFACE, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_LIBC_UNSET_THREADED", (char*) _LIBC_UNSET_THREADED, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_LIBC_WAIT", (char*) _LIBC_WAIT, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_LIBC_WAITPID", (char*) _LIBC_WAITPID, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_LIBC_WRITE", (char*) _LIBC_WRITE, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_LOCALECONV_NOLOCK", (char*) _LOCALECONV_NOLOCK, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_LOCALE_LOCK", (char*) &_LOCALE_LOCK, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "_LOCALTIME_R", (char*) _LOCALTIME_R, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_LOCKF", (char*) _LOCKF, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_LOCKTAB", (char*) &_LOCKTAB, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "_LOC_FILENAME", (char*) &_LOC_FILENAME, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "_LSEEK", (char*) _LSEEK, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_LSTAT", (char*) _LSTAT, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_LTZSET", (char*) _LTZSET, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_MAKECONTEXT", (char*) _MAKECONTEXT, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_MBLEN", (char*) _MBLEN, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_MBSTOWCS", (char*) _MBSTOWCS, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_MBTOWC", (char*) _MBTOWC, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_MEMCCPY", (char*) _MEMCCPY, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_MEMCMP", (char*) _MEMCMP, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_MEMCNTL", (char*) _MEMCNTL, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_MEMCPY", (char*) _MEMCPY, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_MEMMOVE", (char*) _MEMMOVE, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_MEMSET", (char*) _MEMSET, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_MKARGLST", (char*) _MKARGLST, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_MKDIR", (char*) _MKDIR, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_MKFIFO", (char*) _MKFIFO, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_MKNOD", (char*) _MKNOD, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_MKTEMP", (char*) _MKTEMP, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_MMAP", (char*) _MMAP, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_MSYNC", (char*) _MSYNC, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_MUNMAP", (char*) _MUNMAP, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_MUTEX_DESTROY", (char*) _MUTEX_DESTROY, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_MUTEX_HELD", (char*) _MUTEX_HELD, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_MUTEX_INIT", (char*) _MUTEX_INIT, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_MUTEX_TRYLOCK", (char*) _MUTEX_TRYLOCK, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_NATIVELOC", (char*) _NATIVELOC, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_NSS_DEFAULT_FINDERS", (char*) &_NSS_DEFAULT_FINDERS, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "_NSS_DELETE", (char*) _NSS_DELETE, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_NSS_ENDENT", (char*) _NSS_ENDENT, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_NSS_GETENT", (char*) _NSS_GETENT, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_NSS_NETDB_ALIASES", (char*) _NSS_NETDB_ALIASES, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_NSS_SEARCH", (char*) _NSS_SEARCH, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_NSS_SETENT", (char*) _NSS_SETENT, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_NSS_SRC_STATE_DESTR", (char*) _NSS_SRC_STATE_DESTR, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_NSS_STATUS_VEC", (char*) _NSS_STATUS_VEC, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_NSS_XBYY_BUF_ALLOC", (char*) _NSS_XBYY_BUF_ALLOC, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_NSS_XBYY_BUF_FREE", (char*) _NSS_XBYY_BUF_FREE, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_NSS_XBYY_FGETS", (char*) _NSS_XBYY_FGETS, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_NSW_GETONECONFIG", (char*) _NSW_GETONECONFIG, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_NUMERIC", (char*) &_NUMERIC, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "_OPEN", (char*) _OPEN, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_OPENDIR", (char*) _OPENDIR, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_OPENLOG", (char*) _OPENLOG, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_PATHCONF", (char*) _PATHCONF, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_PAUSE", (char*) _PAUSE, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_PCMASK", (char*) &_PCMASK, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "_PIPE", (char*) _PIPE, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_POLL", (char*) _POLL, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_PTHREAD_CONDATTR_DESTROY", (char*) _PTHREAD_CONDATTR_DESTROY, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_PTHREAD_CONDATTR_GETPSHARED", (char*) _PTHREAD_CONDATTR_GETPSHARED, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_PTHREAD_CONDATTR_INIT", (char*) _PTHREAD_CONDATTR_INIT, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_PTHREAD_CONDATTR_SETPSHARED", (char*) _PTHREAD_CONDATTR_SETPSHARED, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_PTHREAD_COND_BROADCAST", (char*) _PTHREAD_COND_BROADCAST, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_PTHREAD_COND_DESTROY", (char*) _PTHREAD_COND_DESTROY, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_PTHREAD_COND_INIT", (char*) _PTHREAD_COND_INIT, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_PTHREAD_COND_SIGNAL", (char*) _PTHREAD_COND_SIGNAL, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_PTHREAD_COND_TIMEDWAIT", (char*) _PTHREAD_COND_TIMEDWAIT, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_PTHREAD_COND_WAIT", (char*) _PTHREAD_COND_WAIT, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_PTHREAD_MUTEXATTR_DESTROY", (char*) _PTHREAD_MUTEXATTR_DESTROY, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_PTHREAD_MUTEXATTR_GETPRIOCEILING", (char*) _PTHREAD_MUTEXATTR_GETPRIOCEILING, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_PTHREAD_MUTEXATTR_GETPROTOCOL", (char*) _PTHREAD_MUTEXATTR_GETPROTOCOL, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_PTHREAD_MUTEXATTR_GETPSHARED", (char*) _PTHREAD_MUTEXATTR_GETPSHARED, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_PTHREAD_MUTEXATTR_INIT", (char*) _PTHREAD_MUTEXATTR_INIT, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_PTHREAD_MUTEXATTR_SETPRIOCEILING", (char*) _PTHREAD_MUTEXATTR_SETPRIOCEILING, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_PTHREAD_MUTEXATTR_SETPROTOCOL", (char*) _PTHREAD_MUTEXATTR_SETPROTOCOL, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_PTHREAD_MUTEXATTR_SETPSHARED", (char*) _PTHREAD_MUTEXATTR_SETPSHARED, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_PTHREAD_MUTEX_DESTROY", (char*) _PTHREAD_MUTEX_DESTROY, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_PTHREAD_MUTEX_GETPRIOCEILING", (char*) _PTHREAD_MUTEX_GETPRIOCEILING, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_PTHREAD_MUTEX_INIT", (char*) _PTHREAD_MUTEX_INIT, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_PTHREAD_MUTEX_LOCK", (char*) _PTHREAD_MUTEX_LOCK, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_PTHREAD_MUTEX_SETPRIOCEILING", (char*) _PTHREAD_MUTEX_SETPRIOCEILING, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_PTHREAD_MUTEX_TRYLOCK", (char*) _PTHREAD_MUTEX_TRYLOCK, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_PTHREAD_MUTEX_UNLOCK", (char*) _PTHREAD_MUTEX_UNLOCK, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_PUTC_UNLOCKED", (char*) _PUTC_UNLOCKED, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_PUTMSG", (char*) _PUTMSG, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_PUTPMSG", (char*) _PUTPMSG, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_QECONVERT", (char*) _QECONVERT, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_QFCONVERT", (char*) _QFCONVERT, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_QGCONVERT", (char*) _QGCONVERT, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_QGETRD", (char*) _QGETRD, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_QGETRP", (char*) _QGETRP, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_QSWAPRD", (char*) _QSWAPRD, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_QSWAPRP", (char*) _QSWAPRP, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_QUADRUPLE_TO_DECIMAL", (char*) _QUADRUPLE_TO_DECIMAL, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_Q_QTOLL", (char*) _Q_QTOLL, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_Q_QTOULL", (char*) _Q_QTOULL, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_Q_add", (char*) _Q_add, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_Q_cmp", (char*) _Q_cmp, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_Q_cmpe", (char*) _Q_cmpe, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_Q_dtoq", (char*) _Q_dtoq, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_Q_feq", (char*) _Q_feq, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_Q_fge", (char*) _Q_fge, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_Q_fgt", (char*) _Q_fgt, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_Q_fle", (char*) _Q_fle, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_Q_flt", (char*) _Q_flt, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_Q_fne", (char*) _Q_fne, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_Q_itoq", (char*) _Q_itoq, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_Q_qtoi", (char*) _Q_qtoi, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_Q_qtou", (char*) _Q_qtou, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_Q_set_exception", (char*) _Q_set_exception, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_Q_sub", (char*) _Q_sub, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_READ", (char*) _READ, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_READDIR", (char*) _READDIR, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_READV", (char*) _READV, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_REALBUFEND", (char*) _REALBUFEND, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_REALLOCK", (char*) _REALLOCK, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_RENAME", (char*) _RENAME, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_REWIND_UNLOCKED", (char*) _REWIND_UNLOCKED, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_RMDIR", (char*) _RMDIR, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_RMUTEX_TRYLOCK", (char*) _RMUTEX_TRYLOCK, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_RWLOCK_INIT", (char*) _RWLOCK_INIT, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_RW_RDLOCK", (char*) _RW_RDLOCK, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_RW_READ_HELD", (char*) _RW_READ_HELD, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_RW_TRYRDLOCK", (char*) _RW_TRYRDLOCK, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_RW_TRYWRLOCK", (char*) _RW_TRYWRLOCK, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_RW_UNLOCK", (char*) _RW_UNLOCK, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_RW_WRITE_HELD", (char*) _RW_WRITE_HELD, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_RW_WRLOCK", (char*) _RW_WRLOCK, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_Randseed", (char*) &_Randseed, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "_SBRK", (char*) _SBRK, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_SBRK_UNLOCKED", (char*) _SBRK_UNLOCKED, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_SELECT", (char*) _SELECT, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_SEMA_HELD", (char*) _SEMA_HELD, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_SEMA_INIT", (char*) _SEMA_INIT, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_SEMA_POST", (char*) _SEMA_POST, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_SEMA_TRYWAIT", (char*) _SEMA_TRYWAIT, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_SEMA_WAIT", (char*) _SEMA_WAIT, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_SEMCTL", (char*) _SEMCTL, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_SEMGET", (char*) _SEMGET, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_SEMOP", (char*) _SEMOP, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_SETBUFEND", (char*) _SETBUFEND, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_SETCONTEXT", (char*) _SETCONTEXT, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_SETITIMER", (char*) _SETITIMER, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_SETLOCALE", (char*) _SETLOCALE, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_SETLOGMASK", (char*) _SETLOGMASK, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_SETPGID", (char*) _SETPGID, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_SETPGRP", (char*) _SETPGRP, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_SETSID", (char*) _SETSID, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_SET_TAB", (char*) _SET_TAB, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_SHMAT", (char*) _SHMAT, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_SHMCTL", (char*) _SHMCTL, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_SHMDT", (char*) _SHMDT, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_SHMGET", (char*) _SHMGET, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_SIBUF", (char*) &_SIBUF, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "_SIGACTION", (char*) _SIGACTION, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_SIGADDSET", (char*) _SIGADDSET, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_SIGALTSTACK", (char*) _SIGALTSTACK, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_SIGDELSET", (char*) _SIGDELSET, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_SIGEMPTYSET", (char*) _SIGEMPTYSET, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_SIGFILLSET", (char*) _SIGFILLSET, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_SIGHOLD", (char*) _SIGHOLD, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_SIGIGNORE", (char*) _SIGIGNORE, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_SIGISMEMBER", (char*) _SIGISMEMBER, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_SIGLONGJMP", (char*) _SIGLONGJMP, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_SIGNAL", (char*) _SIGNAL, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_SIGPAUSE", (char*) _SIGPAUSE, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_SIGPENDING", (char*) _SIGPENDING, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_SIGPROCMASK", (char*) _SIGPROCMASK, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_SIGRELSE", (char*) _SIGRELSE, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_SIGSET", (char*) _SIGSET, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_SIGSETJMP", (char*) _SIGSETJMP, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_SIGSUSPEND", (char*) _SIGSUSPEND, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_SIGUHANDLER", (char*) &_SIGUHANDLER, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "_SIGWAIT", (char*) _SIGWAIT, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_SIMPLE_SCRWIDTH", (char*) _SIMPLE_SCRWIDTH, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_SLEEP", (char*) _SLEEP, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_SMBUF", (char*) &_SMBUF, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "_SOBUF", (char*) &_SOBUF, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "_SPLIT_DOUBLE_19", (char*) _SPLIT_DOUBLE_19, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_SPLIT_DOUBLE_3", (char*) _SPLIT_DOUBLE_3, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_SPLIT_DOUBLE_35", (char*) _SPLIT_DOUBLE_35, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_SPLIT_DOUBLE_51", (char*) _SPLIT_DOUBLE_51, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_SPLIT_DOUBLE_52", (char*) _SPLIT_DOUBLE_52, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_SPLIT_DOUBLE_M1", (char*) _SPLIT_DOUBLE_M1, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_SPLIT_SHORTEN", (char*) _SPLIT_SHORTEN, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_STAT", (char*) _STAT, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_STRDUP", (char*) _STRDUP, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_STRERROR", (char*) _STRERROR, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_STRTOK_R", (char*) _STRTOK_R, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_SYSCALL", (char*) _SYSCALL, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_SYSCONF", (char*) _SYSCONF, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_SYSCONFIG", (char*) _SYSCONFIG, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_SYSINFO", (char*) _SYSINFO, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_SYSLOG", (char*) _SYSLOG, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_SYS_ERRLIST", (char*) &_SYS_ERRLIST, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "_SYS_ERRS", (char*) &_SYS_ERRS, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "_SYS_INDEX", (char*) &_SYS_INDEX, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "_SYS_NERR", (char*) &_SYS_NERR, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "_SYS_NUM_ERR", (char*) &_SYS_NUM_ERR, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "_TCDRAIN", (char*) _TCDRAIN, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_TCGETPGRP", (char*) _TCGETPGRP, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_TCGETSID", (char*) _TCGETSID, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_TEMPNAM", (char*) _TEMPNAM, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_THR_CONTINUE", (char*) _THR_CONTINUE, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_THR_CREATE", (char*) _THR_CREATE, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_THR_ERRNOP", (char*) _THR_ERRNOP, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_THR_EXIT", (char*) _THR_EXIT, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_THR_GETCONCURRENCY", (char*) _THR_GETCONCURRENCY, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_THR_GETPRIO", (char*) _THR_GETPRIO, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_THR_GETSPECIFIC", (char*) _THR_GETSPECIFIC, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_THR_GET_DIRECTION", (char*) _THR_GET_DIRECTION, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_THR_GET_EXCEPTIONS", (char*) _THR_GET_EXCEPTIONS, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_THR_GET_INF_READ", (char*) _THR_GET_INF_READ, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_THR_GET_INF_WRITTEN", (char*) _THR_GET_INF_WRITTEN, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_THR_GET_NAN_READ", (char*) _THR_GET_NAN_READ, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_THR_GET_NAN_WRITTEN", (char*) _THR_GET_NAN_WRITTEN, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_THR_GET_PRECISION", (char*) _THR_GET_PRECISION, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_THR_JOIN", (char*) _THR_JOIN, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_THR_KEYCREATE", (char*) _THR_KEYCREATE, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_THR_KILL", (char*) _THR_KILL, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_THR_LIBTHREAD", (char*) _THR_LIBTHREAD, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_THR_MAIN", (char*) _THR_MAIN, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_THR_MIN_STACK", (char*) _THR_MIN_STACK, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_THR_SELF", (char*) _THR_SELF, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_THR_SETCONCURRENCY", (char*) _THR_SETCONCURRENCY, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_THR_SETPRIO", (char*) _THR_SETPRIO, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_THR_SETSPECIFIC", (char*) _THR_SETSPECIFIC, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_THR_SIGSETMASK", (char*) _THR_SIGSETMASK, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_THR_STKSEGMENT", (char*) _THR_STKSEGMENT, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_THR_SUSPEND", (char*) _THR_SUSPEND, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_THR_YIELD", (char*) _THR_YIELD, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_TIME", (char*) _TIME, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_TIMEZONE", (char*) &_TIMEZONE, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "_TIME_LOCK", (char*) &_TIME_LOCK, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "_TOASCII", (char*) _TOASCII, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_TOLOWER", (char*) _TOLOWER, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_TOUPPER", (char*) _TOUPPER, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_TSDALLOC", (char*) _TSDALLOC, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_TZNAME", (char*) &_TZNAME, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "_TZSET", (char*) _TZSET, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_TZ_GMT", (char*) &_TZ_GMT, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "_TZ_SPACES", (char*) &_TZ_SPACES, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "_ULIMIT", (char*) _ULIMIT, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_UNAME", (char*) _UNAME, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_UNGETC_UNLOCKED", (char*) _UNGETC_UNLOCKED, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_UNLINK", (char*) _UNLINK, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_UNPACKED_TO_DECIMAL_TWO", (char*) _UNPACKED_TO_DECIMAL_TWO, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_VFORK", (char*) _VFORK, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_VSYSLOG", (char*) _VSYSLOG, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_WAIT", (char*) _WAIT, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_WAITID", (char*) _WAITID, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_WAITPID", (char*) _WAITPID, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_WCPTR", (char*) &_WCPTR, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "_WCSTOMBS", (char*) _WCSTOMBS, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_WCTOMB", (char*) _WCTOMB, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_WRITE", (char*) _WRITE, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_WRITEV", (char*) _WRITEV, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_WRTCHK", (char*) _WRTCHK, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_XFLSBUF", (char*) _XFLSBUF, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_XGETWIDTH", (char*) _XGETWIDTH, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__ARINT_SET_N", (char*) __ARINT_SET_N, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__ASSERT", (char*) __ASSERT, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__BASE_CONVERSION_ABORT", (char*) __BASE_CONVERSION_ABORT, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__BASE_CONVERSION_SET_EXCEPTION", (char*) __BASE_CONVERSION_SET_EXCEPTION, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__BASE_CONVERSION_WRITE_ONLY_DOUBLE", (char*) &__BASE_CONVERSION_WRITE_ONLY_DOUBLE, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "__BIG_BINARY_TO_BIG_DECIMAL", (char*) __BIG_BINARY_TO_BIG_DECIMAL, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__BIG_BINARY_TO_UNPACKED", (char*) __BIG_BINARY_TO_UNPACKED, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__BIG_DECIMAL_TO_BIG_BINARY", (char*) __BIG_DECIMAL_TO_BIG_BINARY, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__BIG_FLOAT_TIMES_POWER", (char*) __BIG_FLOAT_TIMES_POWER, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__BINARY_TO_DECIMAL_FRACTION", (char*) __BINARY_TO_DECIMAL_FRACTION, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__BINARY_TO_DECIMAL_INTEGER", (char*) __BINARY_TO_DECIMAL_INTEGER, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__BUILTIN_ALLOCA", (char*) __BUILTIN_ALLOCA, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__CARRY_IN_B10000", (char*) __CARRY_IN_B10000, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__CARRY_PROPAGATE_TEN", (char*) __CARRY_PROPAGATE_TEN, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__CARRY_PROPAGATE_TWO", (char*) __CARRY_PROPAGATE_TWO, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__CLASS_DOUBLE", (char*) __CLASS_DOUBLE, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__CLASS_EXTENDED", (char*) __CLASS_EXTENDED, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__CLASS_QUADRUPLE", (char*) __CLASS_QUADRUPLE, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__CLASS_SINGLE", (char*) __CLASS_SINGLE, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__CLOCK_GETRES", (char*) __CLOCK_GETRES, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__CLOCK_GETTIME", (char*) __CLOCK_GETTIME, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__CLOCK_SETTIME", (char*) __CLOCK_SETTIME, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__COPY_BIG_FLOAT_DIGITS", (char*) __COPY_BIG_FLOAT_DIGITS, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__CTYPE", (char*) &__CTYPE, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "__DAYTAB", (char*) &__DAYTAB, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "__DECIMAL_ROUND", (char*) __DECIMAL_ROUND, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__DECIMAL_TO_BINARY_FRACTION", (char*) __DECIMAL_TO_BINARY_FRACTION, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__DECIMAL_TO_BINARY_INTEGER", (char*) __DECIMAL_TO_BINARY_INTEGER, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__DECIMAL_TO_UNPACKED", (char*) __DECIMAL_TO_UNPACKED, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__DIGITS_TO_DOUBLE", (char*) __DIGITS_TO_DOUBLE, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__DIV64", (char*) __DIV64, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__DOSCAN_U", (char*) __DOSCAN_U, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__DOUBLE_TO_DIGITS", (char*) __DOUBLE_TO_DIGITS, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__ENVIRON_LOCK", (char*) &__ENVIRON_LOCK, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "__FDSYNC", (char*) __FDSYNC, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__FILBUF", (char*) __FILBUF, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__FIRST_LINK", (char*) &__FIRST_LINK, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "__FLSBUF", (char*) __FLSBUF, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__FOUR_DIGITS_QUICK", (char*) __FOUR_DIGITS_QUICK, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__FOUR_DIGITS_QUICK_TABLE", (char*) &__FOUR_DIGITS_QUICK_TABLE, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "__FPU_ADD3WC", (char*) __FPU_ADD3WC, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__FPU_CMPLI", (char*) __FPU_CMPLI, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__FPU_ERROR_NAN", (char*) __FPU_ERROR_NAN, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__FPU_NEG2WC", (char*) __FPU_NEG2WC, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__FPU_NORMALIZE", (char*) __FPU_NORMALIZE, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__FPU_RIGHTSHIFT", (char*) __FPU_RIGHTSHIFT, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__FPU_SET_EXCEPTION", (char*) __FPU_SET_EXCEPTION, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__FPU_SUB3WC", (char*) __FPU_SUB3WC, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__FP_LEFTSHIFT", (char*) __FP_LEFTSHIFT, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__FP_NORMALIZE", (char*) __FP_NORMALIZE, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__FP_RIGHTSHIFT", (char*) __FP_RIGHTSHIFT, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__FP_SET_EXCEPTION", (char*) __FP_SET_EXCEPTION, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__FRACTIONSTRING_TO_BIG_DECIMAL", (char*) __FRACTIONSTRING_TO_BIG_DECIMAL, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__FREE_BIG_FLOAT", (char*) __FREE_BIG_FLOAT, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__GETCONTEXT", (char*) __GETCONTEXT, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__GET_IEEE_FLAGS", (char*) __GET_IEEE_FLAGS, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__INFNANSTRING", (char*) __INFNANSTRING, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__INF_READ", (char*) &__INF_READ, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "__INF_WRITTEN", (char*) &__INF_WRITTEN, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "__INRANGE_DOUBLE", (char*) __INRANGE_DOUBLE, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__INRANGE_QUADEX", (char*) __INRANGE_QUADEX, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__INRANGE_SINGLE", (char*) __INRANGE_SINGLE, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__INTEGERSTRING_TO_BIG_DECIMAL", (char*) __INTEGERSTRING_TO_BIG_DECIMAL, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__IOB", (char*) &__IOB, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "__K_DOUBLE_TO_DECIMAL", (char*) __K_DOUBLE_TO_DECIMAL, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__K_GCONVERT", (char*) __K_GCONVERT, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__K_QUADRUPLE_TO_DECIMAL", (char*) __K_QUADRUPLE_TO_DECIMAL, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__LEFT_SHIFT_BASE_TEN", (char*) __LEFT_SHIFT_BASE_TEN, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__LEFT_SHIFT_BASE_TWO", (char*) __LEFT_SHIFT_BASE_TWO, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__LIBM_LIB_VERSION", (char*) &__LIBM_LIB_VERSION, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "__LONGQUOREM10000", (char*) __LONGQUOREM10000, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__LSHIFT_B10000", (char*) __LSHIFT_B10000, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__LYDAY_TO_MONTH", (char*) &__LYDAY_TO_MONTH, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "__MAJOR", (char*) __MAJOR, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__MAKEDEV", (char*) __MAKEDEV, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__MALLOC_LOCK", (char*) &__MALLOC_LOCK, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "__MINOR", (char*) __MINOR, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__MONTH_SIZE", (char*) &__MONTH_SIZE, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "__MON_LENGTHS", (char*) &__MON_LENGTHS, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "__MT_SIGPENDING", (char*) __MT_SIGPENDING, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__MUL64", (char*) __MUL64, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__MULTIPLY_BASE_TEN", (char*) __MULTIPLY_BASE_TEN, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__MULTIPLY_BASE_TEN_BY_TWO", (char*) __MULTIPLY_BASE_TEN_BY_TWO, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__MULTIPLY_BASE_TEN_VECTOR", (char*) __MULTIPLY_BASE_TEN_VECTOR, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__MULTIPLY_BASE_TWO", (char*) __MULTIPLY_BASE_TWO, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__MULTIPLY_BASE_TWO_VECTOR", (char*) __MULTIPLY_BASE_TWO_VECTOR, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__MUL_10000SHORT", (char*) __MUL_10000SHORT, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__MUL_65536SHORT", (char*) __MUL_65536SHORT, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__NANOSLEEP", (char*) __NANOSLEEP, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__NAN_READ", (char*) &__NAN_READ, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "__NAN_WRITTEN", (char*) &__NAN_WRITTEN, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "__NSW_EXTENDED_ACTION", (char*) __NSW_EXTENDED_ACTION, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__NSW_FREECONFIG", (char*) __NSW_FREECONFIG, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__NSW_GETCONFIG", (char*) __NSW_GETCONFIG, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__PACK_DOUBLE", (char*) __PACK_DOUBLE, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__PACK_EXTENDED", (char*) __PACK_EXTENDED, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__PACK_QUADRUPLE", (char*) __PACK_QUADRUPLE, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__PACK_SINGLE", (char*) __PACK_SINGLE, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__POSIX_ASCTIME_R", (char*) __POSIX_ASCTIME_R, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__POSIX_CTIME_R", (char*) __POSIX_CTIME_R, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__PRODC_B10000", (char*) __PRODC_B10000, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__PROD_10000_B65536", (char*) __PROD_10000_B65536, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__QUOREM", (char*) __QUOREM, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__QUOREM10000", (char*) __QUOREM10000, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__REM64", (char*) __REM64, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__RIGHT_SHIFT_BASE_TWO", (char*) __RIGHT_SHIFT_BASE_TWO, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__SBRK_LOCK", (char*) &__SBRK_LOCK, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "__SET_IEEE_FLAGS", (char*) __SET_IEEE_FLAGS, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__SIGACTION", (char*) __SIGACTION, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__SIGFILLSET", (char*) __SIGFILLSET, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__SIGQUEUE", (char*) __SIGQUEUE, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__SIGTIMEDWAIT", (char*) __SIGTIMEDWAIT, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__SNPRINTF", (char*) __SNPRINTF, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__TBL_10_BIG_DIGITS", (char*) &__TBL_10_BIG_DIGITS, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "__TBL_10_BIG_START", (char*) &__TBL_10_BIG_START, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "__TBL_10_HUGE_DIGITS", (char*) &__TBL_10_HUGE_DIGITS, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "__TBL_10_HUGE_START", (char*) &__TBL_10_HUGE_START, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "__TBL_10_SMALL_DIGITS", (char*) &__TBL_10_SMALL_DIGITS, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "__TBL_10_SMALL_START", (char*) &__TBL_10_SMALL_START, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "__TBL_2_BIG_DIGITS", (char*) &__TBL_2_BIG_DIGITS, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "__TBL_2_BIG_START", (char*) &__TBL_2_BIG_START, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "__TBL_2_HUGE_DIGITS", (char*) &__TBL_2_HUGE_DIGITS, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "__TBL_2_HUGE_START", (char*) &__TBL_2_HUGE_START, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "__TBL_2_SMALL_DIGITS", (char*) &__TBL_2_SMALL_DIGITS, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "__TBL_2_SMALL_START", (char*) &__TBL_2_SMALL_START, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "__TBL_BASELG", (char*) &__TBL_BASELG, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "__TBL_NTENS", (char*) &__TBL_NTENS, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "__TBL_TENS", (char*) &__TBL_TENS, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "__THREADED", (char*) &__THREADED, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "__TIMER_CREATE", (char*) __TIMER_CREATE, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__TIMER_DELETE", (char*) __TIMER_DELETE, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__TIMER_GETOVERRUN", (char*) __TIMER_GETOVERRUN, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__TIMER_GETTIME", (char*) __TIMER_GETTIME, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__TIMER_SETTIME", (char*) __TIMER_SETTIME, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__UDIV64", (char*) __UDIV64, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__UMAC", (char*) __UMAC, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__UMUL64", (char*) __UMUL64, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__UNPACKDOUBLE", (char*) __UNPACKDOUBLE, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__UNPACKLLONG", (char*) __UNPACKLLONG, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__UNPACKSINGLE", (char*) __UNPACKSINGLE, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__UNPACK_DOUBLE", (char*) __UNPACK_DOUBLE, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__UNPACK_DOUBLE_TWO", (char*) __UNPACK_DOUBLE_TWO, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__UNPACK_EXTENDED", (char*) __UNPACK_EXTENDED, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__UNPACK_QUADRUPLE_TWO", (char*) __UNPACK_QUADRUPLE_TWO, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__UNPACK_SINGLE", (char*) __UNPACK_SINGLE, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__UREM64", (char*) __UREM64, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__UTX_LOCK", (char*) &__UTX_LOCK, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "__VSNPRINTF", (char*) __VSNPRINTF, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__WELL_HIDDEN_DGETTEXT", (char*) __WELL_HIDDEN_DGETTEXT, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__XPG4", (char*) &__XPG4, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "__YDAY_TO_MONTH", (char*) &__YDAY_TO_MONTH, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "__YEAR_LENGTHS", (char*) &__YEAR_LENGTHS, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "___ERRNO", (char*) ___ERRNO, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "___MUL_65536_N", (char*) ___MUL_65536_N, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "___x_gnu___gcc_bcmp_o", (char*) &___x_gnu___gcc_bcmp_o, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "___x_gnu__ashldi3_o", (char*) &___x_gnu__ashldi3_o, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "___x_gnu__ashrdi3_o", (char*) &___x_gnu__ashrdi3_o, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "___x_gnu__bb_o", (char*) &___x_gnu__bb_o, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "___x_gnu__clear_cache_o", (char*) &___x_gnu__clear_cache_o, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "___x_gnu__cmpdi2_o", (char*) &___x_gnu__cmpdi2_o, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "___x_gnu__divdi3_o", (char*) &___x_gnu__divdi3_o, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "___x_gnu__eprintf_o", (char*) &___x_gnu__eprintf_o, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "___x_gnu__ffsdi2_o", (char*) &___x_gnu__ffsdi2_o, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "___x_gnu__fixdfdi_o", (char*) &___x_gnu__fixdfdi_o, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "___x_gnu__fixsfdi_o", (char*) &___x_gnu__fixsfdi_o, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "___x_gnu__fixtfdi_o", (char*) &___x_gnu__fixtfdi_o, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "___x_gnu__fixunsdfdi_o", (char*) &___x_gnu__fixunsdfdi_o, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "___x_gnu__fixunsdfsi_o", (char*) &___x_gnu__fixunsdfsi_o, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "___x_gnu__fixunssfdi_o", (char*) &___x_gnu__fixunssfdi_o, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "___x_gnu__fixunssfsi_o", (char*) &___x_gnu__fixunssfsi_o, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "___x_gnu__fixunstfdi_o", (char*) &___x_gnu__fixunstfdi_o, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "___x_gnu__fixunsxfdi_o", (char*) &___x_gnu__fixunsxfdi_o, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "___x_gnu__fixunsxfsi_o", (char*) &___x_gnu__fixunsxfsi_o, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "___x_gnu__fixxfdi_o", (char*) &___x_gnu__fixxfdi_o, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "___x_gnu__floatdidf_o", (char*) &___x_gnu__floatdidf_o, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "___x_gnu__floatdisf_o", (char*) &___x_gnu__floatdisf_o, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "___x_gnu__floatditf_o", (char*) &___x_gnu__floatditf_o, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "___x_gnu__floatdixf_o", (char*) &___x_gnu__floatdixf_o, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "___x_gnu__lshrdi3_o", (char*) &___x_gnu__lshrdi3_o, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "___x_gnu__moddi3_o", (char*) &___x_gnu__moddi3_o, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "___x_gnu__muldi3_o", (char*) &___x_gnu__muldi3_o, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "___x_gnu__negdi2_o", (char*) &___x_gnu__negdi2_o, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "___x_gnu__shtab_o", (char*) &___x_gnu__shtab_o, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "___x_gnu__trampoline_o", (char*) &___x_gnu__trampoline_o, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "___x_gnu__ucmpdi2_o", (char*) &___x_gnu__ucmpdi2_o, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "___x_gnu__udiv_w_sdiv_o", (char*) &___x_gnu__udiv_w_sdiv_o, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "___x_gnu__udivdi3_o", (char*) &___x_gnu__udivdi3_o, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "___x_gnu__udivmoddi4_o", (char*) &___x_gnu__udivmoddi4_o, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "___x_gnu__umoddi3_o", (char*) &___x_gnu__umoddi3_o, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "__ashldi3", (char*) __ashldi3, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__ashrdi3", (char*) __ashrdi3, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__assert", (char*) __assert, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__cacheTextUpdate", (char*) __cacheTextUpdate, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__clear_cache", (char*) __clear_cache, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__clocale", (char*) &__clocale, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "__cmpdi2", (char*) __cmpdi2, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__common_intrinsicsInit", (char*) __common_intrinsicsInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__costate", (char*) &__costate, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "__cplusLoadObjFiles", (char*) &__cplusLoadObjFiles, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "__cplusLoad_o", (char*) &__cplusLoad_o, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "__cplusUsr_o", (char*) &__cplusUsr_o, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "__cplusXtors_o", (char*) &__cplusXtors_o, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "__ctype", (char*) &__ctype, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "__daysSinceEpoch", (char*) __daysSinceEpoch, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__div", (char*) __div, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__divdi3", (char*) __divdi3, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__dtoll", (char*) __dtoll, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__dtoull", (char*) __dtoull, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__errno", (char*) __errno, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__exp10", (char*) __exp10, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__ffsdi2", (char*) __ffsdi2, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__fixdfdi", (char*) __fixdfdi, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__fixsfdi", (char*) __fixsfdi, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__fixtfdi", (char*) __fixtfdi, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__fixunsdfdi", (char*) __fixunsdfdi, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__fixunsdfsi", (char*) __fixunsdfsi, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__fixunssfdi", (char*) __fixunssfdi, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__fixunssfsi", (char*) __fixunssfsi, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__fixunstfdi", (char*) __fixunstfdi, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__floatdidf", (char*) __floatdidf, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__floatdisf", (char*) __floatdisf, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__floatditf", (char*) __floatditf, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__ftoll", (char*) __ftoll, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__ftoull", (char*) __ftoull, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__gcc_bcmp", (char*) __gcc_bcmp, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__gcc_intrinsicsInit", (char*) __gcc_intrinsicsInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__getDstInfo", (char*) __getDstInfo, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__getTime", (char*) __getTime, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__getZoneInfo", (char*) __getZoneInfo, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__includeGnuIntrinsics", (char*) &__includeGnuIntrinsics, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "__julday", (char*) __julday, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__locale", (char*) &__locale, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "__loctime", (char*) &__loctime, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "__lshrdi3", (char*) __lshrdi3, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__main", (char*) __main, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__moddi3", (char*) __moddi3, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__mul", (char*) __mul, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__muldi3", (char*) __muldi3, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__negdi2", (char*) __negdi2, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__rem", (char*) __rem, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__sclose", (char*) __sclose, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__sflags", (char*) __sflags, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__sflush", (char*) __sflush, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__sfvwrite", (char*) __sfvwrite, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__shtab", (char*) &__shtab, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "__sigCtxLoad", (char*) __sigCtxLoad, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__sigCtxSave", (char*) __sigCtxSave, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__smakebuf", (char*) __smakebuf, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__sread", (char*) __sread, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__srefill", (char*) __srefill, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__srget", (char*) __srget, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__sseek", (char*) __sseek, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__stderr", (char*) __stderr, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__stdin", (char*) __stdin, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__stdout", (char*) __stdout, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__stret4", (char*) __stret4, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__stret8", (char*) __stret8, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__strxfrm", (char*) __strxfrm, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__swbuf", (char*) __swbuf, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__swrite", (char*) __swrite, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__swsetup", (char*) __swsetup, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__uc2Reg", (char*) __uc2Reg, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__ucmpdi2", (char*) __ucmpdi2, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__udiv", (char*) __udiv, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__udiv_w_sdiv", (char*) __udiv_w_sdiv, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__udivdi3", (char*) __udivdi3, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__udivmoddi4", (char*) __udivmoddi4, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__umoddi3", (char*) __umoddi3, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__umul", (char*) __umul, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__urem", (char*) __urem, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "__wdbEventListIsEmpty", (char*) &__wdbEventListIsEmpty, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "__wdbEvtptDeleteAll", (char*) &__wdbEvtptDeleteAll, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "_archHelp_msg", (char*) &_archHelp_msg, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "_clockRealtime", (char*) &_clockRealtime, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "_dbgArchInit", (char*) _dbgArchInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_dbgDsmInstRtn", (char*) &_dbgDsmInstRtn, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "_dbgFuncCallCheck", (char*) _dbgFuncCallCheck, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_dbgInstSizeGet", (char*) _dbgInstSizeGet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_dbgRetAdrsGet", (char*) _dbgRetAdrsGet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_dbgTaskPCGet", (char*) _dbgTaskPCGet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_dbgTaskPCSet", (char*) _dbgTaskPCSet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_exit", (char*) _exit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_func_bdall", (char*) &_func_bdall, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "_func_dbgHostNotify", (char*) &_func_dbgHostNotify, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "_func_dbgTargetNotify", (char*) &_func_dbgTargetNotify, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "_func_dspMregsHook", (char*) &_func_dspMregsHook, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "_func_dspRegsListHook", (char*) &_func_dspRegsListHook, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "_func_dspTaskRegsShow", (char*) &_func_dspTaskRegsShow, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "_func_evtLogM0", (char*) &_func_evtLogM0, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "_func_evtLogM1", (char*) &_func_evtLogM1, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "_func_evtLogM2", (char*) &_func_evtLogM2, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "_func_evtLogM3", (char*) &_func_evtLogM3, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "_func_evtLogO", (char*) &_func_evtLogO, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "_func_evtLogOIntLock", (char*) &_func_evtLogOIntLock, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "_func_evtLogPoint", (char*) &_func_evtLogPoint, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "_func_evtLogReserveTaskName", (char*) &_func_evtLogReserveTaskName, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "_func_evtLogString", (char*) &_func_evtLogString, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "_func_evtLogT0", (char*) &_func_evtLogT0, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "_func_evtLogT0_noInt", (char*) &_func_evtLogT0_noInt, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "_func_evtLogT1", (char*) &_func_evtLogT1, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "_func_evtLogT1_noTS", (char*) &_func_evtLogT1_noTS, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "_func_evtLogTSched", (char*) &_func_evtLogTSched, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "_func_excBaseHook", (char*) &_func_excBaseHook, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "_func_excInfoShow", (char*) &_func_excInfoShow, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "_func_excIntHook", (char*) &_func_excIntHook, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "_func_excJobAdd", (char*) &_func_excJobAdd, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "_func_excPanicHook", (char*) &_func_excPanicHook, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "_func_fclose", (char*) &_func_fclose, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "_func_fppTaskRegsShow", (char*) &_func_fppTaskRegsShow, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "_func_ftpLs", (char*) &_func_ftpLs, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "_func_ioTaskStdSet", (char*) &_func_ioTaskStdSet, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "_func_logMsg", (char*) &_func_logMsg, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "_func_memalign", (char*) &_func_memalign, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "_func_netLsByName", (char*) &_func_netLsByName, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "_func_printErr", (char*) &_func_printErr, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "_func_pthread_setcanceltype", (char*) &_func_pthread_setcanceltype, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "_func_remCurIdGet", (char*) &_func_remCurIdGet, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "_func_remCurIdSet", (char*) &_func_remCurIdSet, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "_func_selPtyAdd", (char*) &_func_selPtyAdd, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "_func_selPtyDelete", (char*) &_func_selPtyDelete, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "_func_selTyAdd", (char*) &_func_selTyAdd, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "_func_selTyDelete", (char*) &_func_selTyDelete, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "_func_selWakeupAll", (char*) &_func_selWakeupAll, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "_func_selWakeupListInit", (char*) &_func_selWakeupListInit, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "_func_selWakeupListTerm", (char*) &_func_selWakeupListTerm, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "_func_sigExcKill", (char*) &_func_sigExcKill, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "_func_sigTimeoutRecalc", (char*) &_func_sigTimeoutRecalc, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "_func_sigprocmask", (char*) &_func_sigprocmask, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "_func_smObjObjShow", (char*) &_func_smObjObjShow, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "_func_spy", (char*) &_func_spy, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "_func_spyClkStart", (char*) &_func_spyClkStart, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "_func_spyClkStop", (char*) &_func_spyClkStop, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "_func_spyReport", (char*) &_func_spyReport, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "_func_spyStop", (char*) &_func_spyStop, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "_func_spyTask", (char*) &_func_spyTask, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "_func_sseTaskRegsShow", (char*) &_func_sseTaskRegsShow, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "_func_symFindByValue", (char*) &_func_symFindByValue, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "_func_symFindByValueAndType", (char*) &_func_symFindByValueAndType, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "_func_symFindSymbol", (char*) &_func_symFindSymbol, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "_func_symNameGet", (char*) &_func_symNameGet, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "_func_symTypeGet", (char*) &_func_symTypeGet, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "_func_symValueGet", (char*) &_func_symValueGet, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "_func_taskCreateHookAdd", (char*) &_func_taskCreateHookAdd, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "_func_taskDeleteHookAdd", (char*) &_func_taskDeleteHookAdd, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "_func_taskRegsShowRtn", (char*) &_func_taskRegsShowRtn, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "_func_tmrConnect", (char*) &_func_tmrConnect, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "_func_tmrDisable", (char*) &_func_tmrDisable, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "_func_tmrEnable", (char*) &_func_tmrEnable, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "_func_tmrFreq", (char*) &_func_tmrFreq, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "_func_tmrPeriod", (char*) &_func_tmrPeriod, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "_func_tmrStamp", (char*) &_func_tmrStamp, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "_func_tmrStampLock", (char*) &_func_tmrStampLock, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "_func_trap", (char*) &_func_trap, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "_func_trgCheck", (char*) &_func_trgCheck, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "_func_valloc", (char*) &_func_valloc, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "_func_vxMemProbeHook", (char*) &_func_vxMemProbeHook, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "_func_wdbIsNowExternal", (char*) &_func_wdbIsNowExternal, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "_func_wvNetAddressFilterTest", (char*) &_func_wvNetAddressFilterTest, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "_func_wvNetPortFilterTest", (char*) &_func_wvNetPortFilterTest, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "_lib_version", (char*) _lib_version, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_mutex_lock", (char*) _mutex_lock, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_mutex_unlock", (char*) _mutex_unlock, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_pSigQueueFreeHead", (char*) &_pSigQueueFreeHead, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "_procNumWasSet", (char*) &_procNumWasSet, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "_setjmpSetup", (char*) _setjmpSetup, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_sigCtxLoad", (char*) _sigCtxLoad, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_sigCtxRtnValSet", (char*) _sigCtxRtnValSet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_sigCtxSave", (char*) _sigCtxSave, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_sigCtxSetup", (char*) _sigCtxSetup, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_sigCtxStackEnd", (char*) _sigCtxStackEnd, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_sigfaulttable", (char*) &_sigfaulttable, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "_sysInit", (char*) _sysInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_taskUcShow", (char*) _taskUcShow, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "_wdbTaskBpAdd", (char*) &_wdbTaskBpAdd, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "_wdbTaskBpTrap", (char*) &_wdbTaskBpTrap, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "_wdbTaskCont", (char*) &_wdbTaskCont, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "_wdbTaskStep", (char*) &_wdbTaskStep, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "_yReg", (char*) _yReg, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "abort", (char*) abort, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "abs", (char*) abs, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "acos", (char*) acos, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "activeQHead", (char*) &activeQHead, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "addSegNames", (char*) addSegNames, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "asctime", (char*) asctime, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "asctime_r", (char*) asctime_r, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "asin", (char*) asin, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "assertFiles", (char*) &assertFiles, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "atan", (char*) atan, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "atan2", (char*) atan2, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "atexit", (char*) atexit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "atof", (char*) atof, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "atoi", (char*) atoi, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "atol", (char*) atol, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "attrib", (char*) attrib, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "b", (char*) b, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "bcmp", (char*) bcmp, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "bcopy", (char*) bcopy, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "bcopyBytes", (char*) bcopyBytes, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "bcopyLongs", (char*) bcopyLongs, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "bcopyWords", (char*) bcopyWords, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "bd", (char*) bd, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "bdall", (char*) bdall, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "bfill", (char*) bfill, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "bfillBytes", (char*) bfillBytes, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "binaryOptionsTsfsDrv", (char*) &binaryOptionsTsfsDrv, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "binvert", (char*) binvert, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "bootBpAnchorExtract", (char*) bootBpAnchorExtract, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "bootChange", (char*) bootChange, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "bootLeaseExtract", (char*) bootLeaseExtract, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "bootNetmaskExtract", (char*) bootNetmaskExtract, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "bootParamsErrorPrint", (char*) bootParamsErrorPrint, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "bootParamsPrompt", (char*) bootParamsPrompt, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "bootParamsShow", (char*) bootParamsShow, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "bootScanNum", (char*) bootScanNum, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "bootStringToStruct", (char*) bootStringToStruct, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "bootStructToString", (char*) bootStructToString, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "bpFreeList", (char*) &bpFreeList, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "bpList", (char*) &bpList, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "bsearch", (char*) bsearch, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "bswap", (char*) bswap, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "bufAlloc", (char*) bufAlloc, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "bufFree", (char*) bufFree, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "bufPoolInit", (char*) bufPoolInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "bzero", (char*) bzero, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "c", (char*) c, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "cacheArchLibInit", (char*) cacheArchLibInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "cacheClear", (char*) cacheClear, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "cacheDataEnabled", (char*) &cacheDataEnabled, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "cacheDataMode", (char*) &cacheDataMode, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "cacheDisable", (char*) cacheDisable, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "cacheDmaFree", (char*) cacheDmaFree, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "cacheDmaFreeRtn", (char*) &cacheDmaFreeRtn, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "cacheDmaFuncs", (char*) &cacheDmaFuncs, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "cacheDmaMalloc", (char*) cacheDmaMalloc, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "cacheDmaMallocRtn", (char*) &cacheDmaMallocRtn, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "cacheDrvFlush", (char*) cacheDrvFlush, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "cacheDrvInvalidate", (char*) cacheDrvInvalidate, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "cacheDrvPhysToVirt", (char*) cacheDrvPhysToVirt, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "cacheDrvVirtToPhys", (char*) cacheDrvVirtToPhys, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "cacheEnable", (char*) cacheEnable, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "cacheFlush", (char*) cacheFlush, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "cacheFuncsSet", (char*) cacheFuncsSet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "cacheInvalidate", (char*) cacheInvalidate, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "cacheLib", (char*) &cacheLib, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "cacheLibInit", (char*) cacheLibInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "cacheLock", (char*) cacheLock, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "cacheMmuAvailable", (char*) &cacheMmuAvailable, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "cacheNullFuncs", (char*) &cacheNullFuncs, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "cachePipeFlush", (char*) cachePipeFlush, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "cacheTextUpdate", (char*) cacheTextUpdate, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "cacheUnlock", (char*) cacheUnlock, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "cacheUserFuncs", (char*) &cacheUserFuncs, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "calloc", (char*) calloc, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "cd", (char*) cd, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "ceil", (char*) ceil, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "cfree", (char*) cfree, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "changeReg", (char*) changeReg, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "chdir", (char*) chdir, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "checkStack", (char*) checkStack, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "checksum", (char*) checksum, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "chkdsk", (char*) chkdsk, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "classClassId", (char*) &classClassId, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "classCreate", (char*) classCreate, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "classDestroy", (char*) classDestroy, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "classInit", (char*) classInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "classInstConnect", (char*) classInstConnect, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "classInstrument", (char*) classInstrument, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "classLibInit", (char*) classLibInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "classMemPartIdSet", (char*) classMemPartIdSet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "classShow", (char*) classShow, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "classShowConnect", (char*) classShowConnect, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "classShowInit", (char*) classShowInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "clearerr", (char*) clearerr, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "clock", (char*) clock, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "clockLibInit", (char*) clockLibInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "clock_getres", (char*) clock_getres, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "clock_gettime", (char*) clock_gettime, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "clock_setres", (char*) clock_setres, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "clock_settime", (char*) clock_settime, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "clock_show", (char*) clock_show, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "close", (char*) close, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "closedir", (char*) closedir, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "consoleFd", (char*) &consoleFd, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "consoleName", (char*) &consoleName, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "copy", (char*) copy, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "copyStreams", (char*) copyStreams, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "copyright_wind_river", (char*) &copyright_wind_river, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "copysign", (char*) copysign, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "cos", (char*) cos, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "cosh", (char*) cosh, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "cp", (char*) cp, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "cplusCallCtors", (char*) cplusCallCtors, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "cplusCallDtors", (char*) cplusCallDtors, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "cplusCtors", (char*) cplusCtors, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "cplusCtorsLink", (char*) cplusCtorsLink, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "cplusDemangle", (char*) cplusDemangle, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "cplusDemangleFunc", (char*) &cplusDemangleFunc, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "cplusDemanglerMode", (char*) &cplusDemanglerMode, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "cplusDemanglerSet", (char*) cplusDemanglerSet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "cplusDemanglerStyle", (char*) &cplusDemanglerStyle, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "cplusDemanglerStyleSet", (char*) cplusDemanglerStyleSet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "cplusDtors", (char*) cplusDtors, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "cplusDtorsLink", (char*) cplusDtorsLink, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "cplusLoadFixup", (char*) cplusLoadFixup, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "cplusMatchMangled", (char*) cplusMatchMangled, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "cplusUnloadFixup", (char*) cplusUnloadFixup, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "cplusXtorSet", (char*) cplusXtorSet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "cplusXtorStrategy", (char*) &cplusXtorStrategy, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "creat", (char*) creat, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "creationDate", (char*) &creationDate, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "cret", (char*) cret, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "ctime", (char*) ctime, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "ctime_r", (char*) ctime_r, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "ctypeFiles", (char*) &ctypeFiles, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "d", (char*) d, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "dbgHelp", (char*) dbgHelp, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "dbgInit", (char*) dbgInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "dbgLockUnbreakable", (char*) &dbgLockUnbreakable, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "dbgPrintCall", (char*) dbgPrintCall, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "dbgPrintDsp", (char*) &dbgPrintDsp, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "dbgPrintFpp", (char*) &dbgPrintFpp, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "dbgPrintSimd", (char*) &dbgPrintSimd, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "dbgSafeUnbreakable", (char*) &dbgSafeUnbreakable, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "dbgTaskBpBreakpoint", (char*) dbgTaskBpBreakpoint, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "dbgTaskBpHooksInstall", (char*) dbgTaskBpHooksInstall, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "dbgTaskBpTrace", (char*) dbgTaskBpTrace, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "dbgTaskBpTrap", (char*) dbgTaskBpTrap, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "dbgTaskCont", (char*) dbgTaskCont, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "dbgTaskStep", (char*) dbgTaskStep, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "dbgUnbreakableOld", (char*) &dbgUnbreakableOld, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "devs", (char*) devs, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "difftime", (char*) difftime, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "dirList", (char*) dirList, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "diskFormat", (char*) diskFormat, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "diskInit", (char*) diskInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "div", (char*) div, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "div_r", (char*) div_r, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "dlclose", (char*) dlclose, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "dlerror", (char*) dlerror, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "dllAdd", (char*) dllAdd, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "dllCount", (char*) dllCount, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "dllCreate", (char*) dllCreate, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "dllDelete", (char*) dllDelete, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "dllEach", (char*) dllEach, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "dllGet", (char*) dllGet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "dllInit", (char*) dllInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "dllInsert", (char*) dllInsert, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "dllRemove", (char*) dllRemove, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "dllTerminate", (char*) dllTerminate, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "dlopen", (char*) dlopen, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "dlsym", (char*) dlsym, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "drem", (char*) drem, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "drvTable", (char*) &drvTable, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "dsmData", (char*) dsmData, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "dsmInst", (char*) dsmInst, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "dsmNbytes", (char*) dsmNbytes, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "e", (char*) e, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "envLibInit", (char*) envLibInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "envPrivateCreate", (char*) envPrivateCreate, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "envPrivateDestroy", (char*) envPrivateDestroy, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "envShow", (char*) envShow, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "errno", (char*) &errno, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "errnoGet", (char*) errnoGet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "errnoOfTaskGet", (char*) errnoOfTaskGet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "errnoOfTaskSet", (char*) errnoOfTaskSet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "errnoSet", (char*) errnoSet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "eventClear", (char*) eventClear, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "eventEvtRtn", (char*) &eventEvtRtn, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "eventInit", (char*) eventInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "eventLibInit", (char*) eventLibInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "eventReceive", (char*) eventReceive, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "eventRsrcSend", (char*) eventRsrcSend, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "eventRsrcShow", (char*) eventRsrcShow, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "eventSend", (char*) eventSend, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "eventStart", (char*) eventStart, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "eventTaskShow", (char*) eventTaskShow, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "eventTerminate", (char*) eventTerminate, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "evtAction", (char*) &evtAction, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "evtBufferBind", (char*) evtBufferBind, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "evtBufferId", (char*) &evtBufferId, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "evtLogFuncBind", (char*) evtLogFuncBind, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "evtLogM0", (char*) evtLogM0, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "evtLogM1", (char*) evtLogM1, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "evtLogM2", (char*) evtLogM2, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "evtLogM3", (char*) evtLogM3, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "evtLogO", (char*) evtLogO, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "evtLogOInt", (char*) evtLogOInt, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "evtLogPoint", (char*) evtLogPoint, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "evtLogString", (char*) evtLogString, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "evtLogT0", (char*) evtLogT0, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "evtLogT0_noInt", (char*) evtLogT0_noInt, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "evtLogT1", (char*) evtLogT1, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "evtLogTasks", (char*) evtLogTasks, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "evtObjLogFuncBind", (char*) evtObjLogFuncBind, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "evtsched", (char*) evtsched, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "excExcHandle", (char*) excExcHandle, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "excExcepHook", (char*) &excExcepHook, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "excHookAdd", (char*) excHookAdd, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "excInit", (char*) excInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "excIntHandle", (char*) excIntHandle, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "excJobAdd", (char*) excJobAdd, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "excMsgQId", (char*) &excMsgQId, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "excShowInit", (char*) excShowInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "excTask", (char*) excTask, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "excTaskId", (char*) &excTaskId, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "excTaskOptions", (char*) &excTaskOptions, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "excTaskPriority", (char*) &excTaskPriority, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "excTaskStackSize", (char*) &excTaskStackSize, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "excVecInit", (char*) excVecInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "execute", (char*) execute, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "exit", (char*) exit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "exp", (char*) exp, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "exp__E", (char*) exp__E, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "expm1", (char*) expm1, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "fabs", (char*) fabs, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "fclose", (char*) fclose, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "fdTable", (char*) &fdTable, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "fdopen", (char*) fdopen, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "fdprintf", (char*) fdprintf, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "feof", (char*) feof, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "ferror", (char*) ferror, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "fflush", (char*) fflush, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "ffsLsb", (char*) ffsLsb, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "ffsLsbTbl", (char*) &ffsLsbTbl, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "ffsMsb", (char*) ffsMsb, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "ffsMsbTbl", (char*) &ffsMsbTbl, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "fgetc", (char*) fgetc, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "fgetpos", (char*) fgetpos, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "fgets", (char*) fgets, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "fieldSzIncludeSign", (char*) &fieldSzIncludeSign, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "fileUpPathDefaultPerm", (char*) &fileUpPathDefaultPerm, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "fileUploadPathClose", (char*) fileUploadPathClose, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "fileUploadPathCreate", (char*) fileUploadPathCreate, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "fileUploadPathLibInit", (char*) fileUploadPathLibInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "fileUploadPathWrite", (char*) fileUploadPathWrite, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "fileno", (char*) fileno, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "finite", (char*) finite, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "fioFltInstall", (char*) fioFltInstall, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "fioFormatV", (char*) fioFormatV, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "fioLibInit", (char*) fioLibInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "fioRdString", (char*) fioRdString, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "fioRead", (char*) fioRead, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "fioScanV", (char*) fioScanV, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "floatInit", (char*) floatInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "floor", (char*) floor, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "fmod", (char*) fmod, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "fopen", (char*) fopen, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "fpClassId", (char*) &fpClassId, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "fpCtlRegName", (char*) &fpCtlRegName, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "fpRegName", (char*) &fpRegName, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "fpTypeGet", (char*) fpTypeGet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "fppArchInit", (char*) fppArchInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "fppArchTaskCreateInit", (char*) fppArchTaskCreateInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "fppCreateHookRtn", (char*) &fppCreateHookRtn, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "fppCtxToRegs", (char*) fppCtxToRegs, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "fppDisplayHookRtn", (char*) &fppDisplayHookRtn, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "fppFsrDefault", (char*) &fppFsrDefault, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "fppInit", (char*) fppInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "fppNan", (char*) fppNan, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "fppProbe", (char*) fppProbe, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "fppRegsToCtx", (char*) fppRegsToCtx, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "fppRestore", (char*) fppRestore, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "fppSave", (char*) fppSave, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "fppShowInit", (char*) fppShowInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "fppTaskRegsCFmt", (char*) &fppTaskRegsCFmt, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "fppTaskRegsDFmt", (char*) &fppTaskRegsDFmt, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "fppTaskRegsGet", (char*) fppTaskRegsGet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "fppTaskRegsSet", (char*) fppTaskRegsSet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "fppTaskRegsShow", (char*) fppTaskRegsShow, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "fprintf", (char*) fprintf, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "fputc", (char*) fputc, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "fputs", (char*) fputs, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "fread", (char*) fread, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "free", (char*) free, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "freopen", (char*) freopen, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "frexp", (char*) frexp, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "fscanf", (char*) fscanf, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "fseek", (char*) fseek, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "fsetpos", (char*) fsetpos, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "fstat", (char*) fstat, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "fstatfs", (char*) fstatfs, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "ftell", (char*) ftell, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "ftplDebug", (char*) &ftplDebug, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "fwrite", (char*) fwrite, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "g0", (char*) g0, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "g1", (char*) g1, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "g2", (char*) g2, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "g3", (char*) g3, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "g4", (char*) g4, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "g5", (char*) g5, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "g6", (char*) g6, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "g7", (char*) g7, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "getc", (char*) getc, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "getchar", (char*) getchar, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "getcwd", (char*) getcwd, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "getenv", (char*) getenv, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "gets", (char*) gets, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "getw", (char*) getw, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "getwd", (char*) getwd, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "gmtime", (char*) gmtime, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "gmtime_r", (char*) gmtime_r, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "h", (char*) h, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "hashClassId", (char*) &hashClassId, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "hashFuncIterScale", (char*) hashFuncIterScale, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "hashFuncModulo", (char*) hashFuncModulo, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "hashFuncMultiply", (char*) hashFuncMultiply, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "hashKeyCmp", (char*) hashKeyCmp, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "hashKeyStrCmp", (char*) hashKeyStrCmp, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "hashLibInit", (char*) hashLibInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "hashTblCreate", (char*) hashTblCreate, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "hashTblDelete", (char*) hashTblDelete, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "hashTblDestroy", (char*) hashTblDestroy, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "hashTblEach", (char*) hashTblEach, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "hashTblFind", (char*) hashTblFind, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "hashTblInit", (char*) hashTblInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "hashTblPut", (char*) hashTblPut, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "hashTblRemove", (char*) hashTblRemove, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "hashTblTerminate", (char*) hashTblTerminate, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "help", (char*) help, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "i", (char*) i, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "i0", (char*) i0, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "i1", (char*) i1, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "i2", (char*) i2, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "i3", (char*) i3, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "i4", (char*) i4, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "i5", (char*) i5, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "i6", (char*) i6, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "i7", (char*) i7, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "index", (char*) index, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "intCatch", (char*) intCatch, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "intCnt", (char*) &intCnt, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "intConnect", (char*) intConnect, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "intContext", (char*) intContext, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "intCount", (char*) intCount, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "intDisable", (char*) intDisable, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "intEnable", (char*) intEnable, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "intHandlerCreate", (char*) intHandlerCreate, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "intLevelSet", (char*) intLevelSet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "intLock", (char*) intLock, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "intLockLevelGet", (char*) intLockLevelGet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "intLockLevelSet", (char*) intLockLevelSet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "intLockMask", (char*) &intLockMask, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "intRegsLock", (char*) intRegsLock, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "intRegsUnlock", (char*) intRegsUnlock, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "intRestrict", (char*) intRestrict, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "intRun", (char*) intRun, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "intUnlock", (char*) intUnlock, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "intUnlockMask", (char*) &intUnlockMask, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "intVecBaseGet", (char*) intVecBaseGet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "intVecBaseSet", (char*) intVecBaseSet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "intVecGet", (char*) intVecGet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "intVecSet", (char*) intVecSet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "intVecTable", (char*) &intVecTable, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "intVecTableWriteProtect", (char*) intVecTableWriteProtect, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "ioDefDevGet", (char*) ioDefDevGet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "ioDefDirGet", (char*) ioDefDirGet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "ioDefPath", (char*) &ioDefPath, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "ioDefPathCat", (char*) ioDefPathCat, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "ioDefPathGet", (char*) ioDefPathGet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "ioDefPathSet", (char*) ioDefPathSet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "ioFullFileNameGet", (char*) ioFullFileNameGet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "ioGlobalStdGet", (char*) ioGlobalStdGet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "ioGlobalStdSet", (char*) ioGlobalStdSet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "ioHelp", (char*) ioHelp, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "ioMaxLinkLevels", (char*) &ioMaxLinkLevels, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "ioTaskStdGet", (char*) ioTaskStdGet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "ioTaskStdSet", (char*) ioTaskStdSet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "ioctl", (char*) ioctl, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "iosClose", (char*) iosClose, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "iosCreate", (char*) iosCreate, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "iosDelete", (char*) iosDelete, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "iosDevAdd", (char*) iosDevAdd, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "iosDevDelete", (char*) iosDevDelete, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "iosDevFind", (char*) iosDevFind, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "iosDevShow", (char*) iosDevShow, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "iosDrvInstall", (char*) iosDrvInstall, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "iosDrvRemove", (char*) iosDrvRemove, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "iosDrvShow", (char*) iosDrvShow, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "iosDvList", (char*) &iosDvList, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "iosFdDevFind", (char*) iosFdDevFind, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "iosFdFree", (char*) iosFdFree, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "iosFdFreeHookRtn", (char*) &iosFdFreeHookRtn, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "iosFdNew", (char*) iosFdNew, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "iosFdNewHookRtn", (char*) &iosFdNewHookRtn, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "iosFdSet", (char*) iosFdSet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "iosFdShow", (char*) iosFdShow, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "iosFdValue", (char*) iosFdValue, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "iosInit", (char*) iosInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "iosIoctl", (char*) iosIoctl, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "iosLibInitialized", (char*) &iosLibInitialized, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "iosNextDevGet", (char*) iosNextDevGet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "iosOpen", (char*) iosOpen, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "iosRead", (char*) iosRead, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "iosShowInit", (char*) iosShowInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "iosWrite", (char*) iosWrite, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "isTaskNew", (char*) isTaskNew, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "isalnum", (char*) isalnum, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "isalpha", (char*) isalpha, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "isatty", (char*) isatty, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "iscntrl", (char*) iscntrl, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "isdigit", (char*) isdigit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "isgraph", (char*) isgraph, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "islower", (char*) islower, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "isprint", (char*) isprint, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "ispunct", (char*) ispunct, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "isspace", (char*) isspace, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "isupper", (char*) isupper, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "isxdigit", (char*) isxdigit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "kernelInit", (char*) kernelInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "kernelIsIdle", (char*) &kernelIsIdle, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "kernelState", (char*) &kernelState, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "kernelTimeSlice", (char*) kernelTimeSlice, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "kernelVersion", (char*) kernelVersion, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "kill", (char*) kill, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "l", (char*) l, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "l0", (char*) l0, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "l1", (char*) l1, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "l2", (char*) l2, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "l3", (char*) l3, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "l4", (char*) l4, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "l5", (char*) l5, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "l6", (char*) l6, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "l7", (char*) l7, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "labs", (char*) labs, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "ld", (char*) ld, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "ldCommonMatchAll", (char*) &ldCommonMatchAll, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "ldexp", (char*) ldexp, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "ldiv", (char*) ldiv, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "ldiv_r", (char*) ldiv_r, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "ledClose", (char*) ledClose, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "ledControl", (char*) ledControl, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "ledId", (char*) &ledId, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "ledOpen", (char*) ledOpen, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "ledRead", (char*) ledRead, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "lexActions", (char*) lexActions, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "lexClass", (char*) &lexClass, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "lexNclasses", (char*) &lexNclasses, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "lexStateTable", (char*) &lexStateTable, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "linkedCtorsInitialized", (char*) &linkedCtorsInitialized, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "lkAddr", (char*) lkAddr, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "lkup", (char*) lkup, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "ll", (char*) ll, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "llr", (char*) llr, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "loadCommonManage", (char*) loadCommonManage, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "loadCommonMatch", (char*) loadCommonMatch, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "loadModule", (char*) loadModule, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "loadModuleAt", (char*) loadModuleAt, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "loadModuleAtSym", (char*) loadModuleAtSym, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "loadModuleGet", (char*) loadModuleGet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "loadRoutine", (char*) &loadRoutine, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "loadSegmentsAllocate", (char*) loadSegmentsAllocate, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "localToGlobalOffset", (char*) &localToGlobalOffset, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "localeFiles", (char*) &localeFiles, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "localeconv", (char*) localeconv, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "localtime", (char*) localtime, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "localtime_r", (char*) localtime_r, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "log", (char*) log, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "log10", (char*) log10, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "logFdAdd", (char*) logFdAdd, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "logFdDelete", (char*) logFdDelete, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "logFdFromRlogin", (char*) &logFdFromRlogin, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "logFdSet", (char*) logFdSet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "logInit", (char*) logInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "logMsg", (char*) logMsg, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "logShow", (char*) logShow, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "logTask", (char*) logTask, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "logTaskId", (char*) &logTaskId, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "logTaskOptions", (char*) &logTaskOptions, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "logTaskPriority", (char*) &logTaskPriority, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "logTaskStackSize", (char*) &logTaskStackSize, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "log__L", (char*) log__L, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "logb", (char*) logb, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "logout", (char*) logout, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "longjmp", (char*) longjmp, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "ls", (char*) ls, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "lseek", (char*) lseek, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "lsr", (char*) lsr, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "lstAdd", (char*) lstAdd, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "lstConcat", (char*) lstConcat, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "lstCount", (char*) lstCount, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "lstDelete", (char*) lstDelete, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "lstExtract", (char*) lstExtract, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "lstFind", (char*) lstFind, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "lstFirst", (char*) lstFirst, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "lstFree", (char*) lstFree, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "lstGet", (char*) lstGet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "lstInit", (char*) lstInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "lstInsert", (char*) lstInsert, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "lstLast", (char*) lstLast, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "lstLibInit", (char*) lstLibInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "lstNStep", (char*) lstNStep, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "lstNext", (char*) lstNext, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "lstNth", (char*) lstNth, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "lstPrevious", (char*) lstPrevious, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "m", (char*) m, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "mRegs", (char*) mRegs, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "main", (char*) main, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "malloc", (char*) malloc, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "mathFiles", (char*) &mathFiles, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "mathHardInit", (char*) mathHardInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "maxDrivers", (char*) &maxDrivers, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "maxFiles", (char*) &maxFiles, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "mblen", (char*) mblen, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "mbstowcs", (char*) mbstowcs, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "mbtowc", (char*) mbtowc, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "memAddToPool", (char*) memAddToPool, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "memDefaultAlignment", (char*) &memDefaultAlignment, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "memFindMax", (char*) memFindMax, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "memInit", (char*) memInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "memLibInit", (char*) memLibInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "memOptionsSet", (char*) memOptionsSet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "memPartAddToPool", (char*) memPartAddToPool, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "memPartAlignedAlloc", (char*) memPartAlignedAlloc, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "memPartAlloc", (char*) memPartAlloc, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "memPartAllocErrorRtn", (char*) &memPartAllocErrorRtn, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "memPartBlockErrorRtn", (char*) &memPartBlockErrorRtn, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "memPartBlockIsValid", (char*) memPartBlockIsValid, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "memPartClassId", (char*) &memPartClassId, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "memPartCreate", (char*) memPartCreate, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "memPartFindMax", (char*) memPartFindMax, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "memPartFree", (char*) memPartFree, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "memPartInfoGet", (char*) memPartInfoGet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "memPartInit", (char*) memPartInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "memPartInstClassId", (char*) &memPartInstClassId, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "memPartLibInit", (char*) memPartLibInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "memPartOptionsDefault", (char*) &memPartOptionsDefault, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "memPartOptionsSet", (char*) memPartOptionsSet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "memPartRealloc", (char*) memPartRealloc, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "memPartSemInitRtn", (char*) &memPartSemInitRtn, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "memPartShow", (char*) memPartShow, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "memShow", (char*) memShow, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "memShowInit", (char*) memShowInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "memSysPartId", (char*) &memSysPartId, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "memalign", (char*) memalign, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "memchr", (char*) memchr, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "memcmp", (char*) memcmp, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "memcpy", (char*) memcpy, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "memmove", (char*) memmove, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "memset", (char*) memset, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "mkdir", (char*) mkdir, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "mktime", (char*) mktime, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "modf", (char*) modf, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "moduleCheck", (char*) moduleCheck, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "moduleClassId", (char*) &moduleClassId, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "moduleCreate", (char*) moduleCreate, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "moduleCreateHookAdd", (char*) moduleCreateHookAdd, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "moduleCreateHookDelete", (char*) moduleCreateHookDelete, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "moduleDelete", (char*) moduleDelete, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "moduleEach", (char*) moduleEach, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "moduleFindByGroup", (char*) moduleFindByGroup, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "moduleFindByName", (char*) moduleFindByName, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "moduleFindByNameAndPath", (char*) moduleFindByNameAndPath, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "moduleFlagsGet", (char*) moduleFlagsGet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "moduleIdFigure", (char*) moduleIdFigure, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "moduleIdListGet", (char*) moduleIdListGet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "moduleInfoGet", (char*) moduleInfoGet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "moduleInit", (char*) moduleInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "moduleLibInit", (char*) moduleLibInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "moduleNameGet", (char*) moduleNameGet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "moduleSegAdd", (char*) moduleSegAdd, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "moduleSegEach", (char*) moduleSegEach, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "moduleSegFirst", (char*) moduleSegFirst, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "moduleSegGet", (char*) moduleSegGet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "moduleSegNext", (char*) moduleSegNext, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "moduleShow", (char*) moduleShow, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "moduleTerminate", (char*) moduleTerminate, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "msgQClassId", (char*) &msgQClassId, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "msgQCreate", (char*) msgQCreate, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "msgQDelete", (char*) msgQDelete, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "msgQDistInfoGetRtn", (char*) &msgQDistInfoGetRtn, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "msgQDistNumMsgsRtn", (char*) &msgQDistNumMsgsRtn, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "msgQDistReceiveRtn", (char*) &msgQDistReceiveRtn, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "msgQDistSendRtn", (char*) &msgQDistSendRtn, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "msgQDistShowRtn", (char*) &msgQDistShowRtn, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "msgQEvLibInit", (char*) msgQEvLibInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "msgQEvStart", (char*) msgQEvStart, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "msgQEvStop", (char*) msgQEvStop, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "msgQInfoGet", (char*) msgQInfoGet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "msgQInit", (char*) msgQInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "msgQInstClassId", (char*) &msgQInstClassId, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "msgQLibInit", (char*) msgQLibInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "msgQNumMsgs", (char*) msgQNumMsgs, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "msgQReceive", (char*) msgQReceive, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "msgQSend", (char*) msgQSend, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "msgQShow", (char*) msgQShow, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "msgQShowInit", (char*) msgQShowInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "msgQSmInfoGetRtn", (char*) &msgQSmInfoGetRtn, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "msgQSmNumMsgsRtn", (char*) &msgQSmNumMsgsRtn, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "msgQSmReceiveRtn", (char*) &msgQSmReceiveRtn, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "msgQSmSendRtn", (char*) &msgQSmSendRtn, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "msgQSmShowRtn", (char*) &msgQSmShowRtn, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "msgQTerminate", (char*) msgQTerminate, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "mutexOptionsIosLib", (char*) &mutexOptionsIosLib, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "mutexOptionsLogLib", (char*) &mutexOptionsLogLib, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "mutexOptionsMemLib", (char*) &mutexOptionsMemLib, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "mutexOptionsSelectLib", (char*) &mutexOptionsSelectLib, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "mutexOptionsSymLib", (char*) &mutexOptionsSymLib, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "mutexOptionsTsfsDrv", (char*) &mutexOptionsTsfsDrv, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "mutexOptionsTyLib", (char*) &mutexOptionsTyLib, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "mv", (char*) mv, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "namelessPrefix", (char*) &namelessPrefix, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "netHelp", (char*) netHelp, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "npc", (char*) npc, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "o0", (char*) o0, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "o1", (char*) o1, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "o2", (char*) o2, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "o3", (char*) o3, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "o4", (char*) o4, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "o5", (char*) o5, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "o6", (char*) o6, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "o7", (char*) o7, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "objAlloc", (char*) objAlloc, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "objAllocExtra", (char*) objAllocExtra, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "objCoreInit", (char*) objCoreInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "objCoreTerminate", (char*) objCoreTerminate, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "objFree", (char*) objFree, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "objShow", (char*) objShow, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "open", (char*) open, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "opendir", (char*) opendir, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "pFppTaskIdPrevious", (char*) &pFppTaskIdPrevious, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "pJobPool", (char*) &pJobPool, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "pRootMemStart", (char*) &pRootMemStart, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "pTaskLastDspTcb", (char*) &pTaskLastDspTcb, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "pTaskLastFpTcb", (char*) &pTaskLastFpTcb, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "pWdbMemRegions", (char*) &pWdbMemRegions, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "pWdbRtIf", (char*) &pWdbRtIf, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "pWvNetEventMap", (char*) &pWvNetEventMap, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "passFsDevInit", (char*) passFsDevInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "passFsInit", (char*) passFsInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "passFsUnixDirPerm", (char*) &passFsUnixDirPerm, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "passFsUnixFilePerm", (char*) &passFsUnixFilePerm, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "passVolume", (char*) &passVolume, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "pathBuild", (char*) pathBuild, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "pathCat", (char*) pathCat, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "pathCondense", (char*) pathCondense, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "pathLastName", (char*) pathLastName, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "pathLastNamePtr", (char*) pathLastNamePtr, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "pathParse", (char*) pathParse, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "pathSplit", (char*) pathSplit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "pause", (char*) pause, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "pc", (char*) pc, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "period", (char*) period, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "periodRun", (char*) periodRun, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "perror", (char*) perror, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "pipeDevCreate", (char*) pipeDevCreate, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "pipeDevDelete", (char*) pipeDevDelete, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "pipeDrv", (char*) pipeDrv, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "pipeMsgQOptions", (char*) &pipeMsgQOptions, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "pmPartId", (char*) &pmPartId, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "pow", (char*) pow, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "pow_p", (char*) pow_p, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "ppGlobalEnviron", (char*) &ppGlobalEnviron, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "printErr", (char*) printErr, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "printErrno", (char*) printErrno, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "printExc", (char*) printExc, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "printLogo", (char*) printLogo, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "printf", (char*) printf, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "psr", (char*) psr, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "putc", (char*) putc, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "putchar", (char*) putchar, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "putenv", (char*) putenv, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "puts", (char*) puts, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "putw", (char*) putw, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "pwd", (char*) pwd, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "qAdvance", (char*) qAdvance, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "qCalibrate", (char*) qCalibrate, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "qCreate", (char*) qCreate, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "qDelete", (char*) qDelete, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "qEach", (char*) qEach, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "qFifoClassId", (char*) &qFifoClassId, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "qFifoCreate", (char*) qFifoCreate, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "qFifoDelete", (char*) qFifoDelete, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "qFifoEach", (char*) qFifoEach, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "qFifoGet", (char*) qFifoGet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "qFifoInfo", (char*) qFifoInfo, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "qFifoInit", (char*) qFifoInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "qFifoPut", (char*) qFifoPut, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "qFifoRemove", (char*) qFifoRemove, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "qFirst", (char*) qFirst, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "qGet", (char*) qGet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "qGetExpired", (char*) qGetExpired, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "qInfo", (char*) qInfo, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "qInit", (char*) qInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "qJobClassId", (char*) &qJobClassId, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "qJobCreate", (char*) qJobCreate, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "qJobDelete", (char*) qJobDelete, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "qJobEach", (char*) qJobEach, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "qJobGet", (char*) qJobGet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "qJobInfo", (char*) qJobInfo, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "qJobInit", (char*) qJobInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "qJobPut", (char*) qJobPut, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "qJobTerminate", (char*) qJobTerminate, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "qKey", (char*) qKey, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "qPriBMapClassId", (char*) &qPriBMapClassId, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "qPriBMapCreate", (char*) qPriBMapCreate, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "qPriBMapDelete", (char*) qPriBMapDelete, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "qPriBMapEach", (char*) qPriBMapEach, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "qPriBMapGet", (char*) qPriBMapGet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "qPriBMapInfo", (char*) qPriBMapInfo, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "qPriBMapInit", (char*) qPriBMapInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "qPriBMapKey", (char*) qPriBMapKey, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "qPriBMapListCreate", (char*) qPriBMapListCreate, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "qPriBMapListDelete", (char*) qPriBMapListDelete, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "qPriBMapPut", (char*) qPriBMapPut, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "qPriBMapRemove", (char*) qPriBMapRemove, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "qPriBMapResort", (char*) qPriBMapResort, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "qPriListAdvance", (char*) qPriListAdvance, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "qPriListCalibrate", (char*) qPriListCalibrate, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "qPriListClassId", (char*) &qPriListClassId, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "qPriListCreate", (char*) qPriListCreate, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "qPriListDelete", (char*) qPriListDelete, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "qPriListEach", (char*) qPriListEach, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "qPriListFromTailClassId", (char*) &qPriListFromTailClassId, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "qPriListGet", (char*) qPriListGet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "qPriListGetExpired", (char*) qPriListGetExpired, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "qPriListInfo", (char*) qPriListInfo, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "qPriListInit", (char*) qPriListInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "qPriListKey", (char*) qPriListKey, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "qPriListPut", (char*) qPriListPut, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "qPriListPutFromTail", (char*) qPriListPutFromTail, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "qPriListRemove", (char*) qPriListRemove, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "qPriListResort", (char*) qPriListResort, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "qPriListTerminate", (char*) qPriListTerminate, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "qPut", (char*) qPut, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "qRemove", (char*) qRemove, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "qResort", (char*) qResort, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "qTerminate", (char*) qTerminate, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "qsort", (char*) qsort, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "rBuffClass", (char*) &rBuffClass, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "rBuffClassId", (char*) &rBuffClassId, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "rBuffCreate", (char*) rBuffCreate, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "rBuffDestroy", (char*) rBuffDestroy, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "rBuffFlush", (char*) rBuffFlush, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "rBuffInfoGet", (char*) rBuffInfoGet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "rBuffLibInit", (char*) rBuffLibInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "rBuffNBytes", (char*) rBuffNBytes, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "rBuffRead", (char*) rBuffRead, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "rBuffReadCommit", (char*) rBuffReadCommit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "rBuffReadReserve", (char*) rBuffReadReserve, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "rBuffReset", (char*) rBuffReset, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "rBuffSetFd", (char*) rBuffSetFd, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "rBuffShow", (char*) rBuffShow, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "rBuffShowInit", (char*) rBuffShowInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "rBuffShowRtn", (char*) &rBuffShowRtn, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "rBuffUpload", (char*) rBuffUpload, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "rBuffVerify", (char*) rBuffVerify, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "rBuffWrite", (char*) rBuffWrite, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "raise", (char*) raise, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "rand", (char*) rand, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "read", (char*) read, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "readdir", (char*) readdir, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "readv", (char*) readv, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "readyQBMap", (char*) &readyQBMap, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "readyQHead", (char*) &readyQHead, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "realloc", (char*) realloc, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "reboot", (char*) reboot, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "rebootHookAdd", (char*) rebootHookAdd, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "redirInFd", (char*) &redirInFd, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "redirOutFd", (char*) &redirOutFd, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "remove", (char*) remove, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "rename", (char*) rename, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "repeat", (char*) repeat, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "repeatRun", (char*) repeatRun, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "reschedule", (char*) reschedule, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "restartTaskName", (char*) &restartTaskName, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "restartTaskOptions", (char*) &restartTaskOptions, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "restartTaskPriority", (char*) &restartTaskPriority, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "restartTaskStackSize", (char*) &restartTaskStackSize, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "rewind", (char*) rewind, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "rewinddir", (char*) rewinddir, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "rindex", (char*) rindex, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "rm", (char*) rm, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "rmdir", (char*) rmdir, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "rngBufGet", (char*) rngBufGet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "rngBufPut", (char*) rngBufPut, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "rngCreate", (char*) rngCreate, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "rngDelete", (char*) rngDelete, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "rngFlush", (char*) rngFlush, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "rngFreeBytes", (char*) rngFreeBytes, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "rngIsEmpty", (char*) rngIsEmpty, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "rngIsFull", (char*) rngIsFull, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "rngMoveAhead", (char*) rngMoveAhead, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "rngNBytes", (char*) rngNBytes, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "rngPutAhead", (char*) rngPutAhead, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "rootMemNBytes", (char*) &rootMemNBytes, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "rootTaskId", (char*) &rootTaskId, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "roundRobinOn", (char*) &roundRobinOn, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "roundRobinSlice", (char*) &roundRobinSlice, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "runtimeName", (char*) &runtimeName, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "runtimeVersion", (char*) &runtimeVersion, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "s", (char*) s, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "s_dosTimeHook", (char*) s_dosTimeHook, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "s_fdint", (char*) s_fdint, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "s_init", (char*) s_init, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "s_sigcatch", (char*) s_sigcatch, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "s_sigcatchUser", (char*) &s_sigcatchUser, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "s_uname", (char*) s_uname, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "s_userGet", (char*) s_userGet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "scalb", (char*) scalb, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "scanCharSet", (char*) scanCharSet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "scanField", (char*) scanField, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "scanf", (char*) scanf, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "selNodeAdd", (char*) selNodeAdd, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "selNodeDelete", (char*) selNodeDelete, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "selTaskDeleteHookAdd", (char*) selTaskDeleteHookAdd, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "selWakeup", (char*) selWakeup, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "selWakeupAll", (char*) selWakeupAll, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "selWakeupListInit", (char*) selWakeupListInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "selWakeupListLen", (char*) selWakeupListLen, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "selWakeupListTerm", (char*) selWakeupListTerm, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "selWakeupType", (char*) selWakeupType, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "select", (char*) select, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "selectInit", (char*) selectInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "semBCoreInit", (char*) semBCoreInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "semBCreate", (char*) semBCreate, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "semBGive", (char*) semBGive, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "semBGiveDefer", (char*) semBGiveDefer, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "semBInit", (char*) semBInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "semBLibInit", (char*) semBLibInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "semBTake", (char*) semBTake, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "semCCoreInit", (char*) semCCoreInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "semCCreate", (char*) semCCreate, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "semCGive", (char*) semCGive, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "semCGiveDefer", (char*) semCGiveDefer, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "semCInit", (char*) semCInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "semCLibInit", (char*) semCLibInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "semCTake", (char*) semCTake, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "semClass", (char*) &semClass, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "semClassId", (char*) &semClassId, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "semDelete", (char*) semDelete, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "semDestroy", (char*) semDestroy, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "semEvIsFreeTbl", (char*) &semEvIsFreeTbl, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "semEvLibInit", (char*) semEvLibInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "semEvStart", (char*) semEvStart, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "semEvStop", (char*) semEvStop, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "semFlush", (char*) semFlush, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "semFlushDefer", (char*) semFlushDefer, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "semFlushDeferTbl", (char*) &semFlushDeferTbl, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "semFlushTbl", (char*) &semFlushTbl, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "semGive", (char*) semGive, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "semGiveDefer", (char*) semGiveDefer, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "semGiveDeferTbl", (char*) &semGiveDeferTbl, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "semGiveTbl", (char*) &semGiveTbl, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "semInfo", (char*) semInfo, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "semInstClass", (char*) &semInstClass, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "semInstClassId", (char*) &semInstClassId, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "semIntRestrict", (char*) semIntRestrict, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "semInvalid", (char*) semInvalid, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "semLibInit", (char*) semLibInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "semMCoreInit", (char*) semMCoreInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "semMCreate", (char*) semMCreate, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "semMGive", (char*) semMGive, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "semMGiveForce", (char*) semMGiveForce, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "semMGiveKern", (char*) semMGiveKern, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "semMGiveKernWork", (char*) &semMGiveKernWork, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "semMInit", (char*) semMInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "semMLibInit", (char*) semMLibInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "semMPendQPut", (char*) semMPendQPut, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "semMTake", (char*) semMTake, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "semQFlush", (char*) semQFlush, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "semQFlushDefer", (char*) semQFlushDefer, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "semQInit", (char*) semQInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "semShow", (char*) semShow, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "semShowInit", (char*) semShowInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "semSmInfoRtn", (char*) &semSmInfoRtn, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "semSmShowRtn", (char*) &semSmShowRtn, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "semTake", (char*) semTake, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "semTakeTbl", (char*) &semTakeTbl, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "semTerminate", (char*) semTerminate, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "seqConnect", (char*) seqConnect, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "seqDisable", (char*) seqDisable, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "seqEnable", (char*) seqEnable, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "seqFreq", (char*) seqFreq, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "seqPeriod", (char*) seqPeriod, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "seqStamp", (char*) seqStamp, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "seqStampLock", (char*) seqStampLock, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "setbuf", (char*) setbuf, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "setbuffer", (char*) setbuffer, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "setjmp", (char*) setjmp, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "setlinebuf", (char*) setlinebuf, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "setlocale", (char*) setlocale, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "setvbuf", (char*) setvbuf, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "shell", (char*) shell, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "shellHistSize", (char*) &shellHistSize, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "shellHistory", (char*) shellHistory, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "shellInit", (char*) shellInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "shellIsRemoteConnectedGet", (char*) shellIsRemoteConnectedGet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "shellIsRemoteConnectedSet", (char*) shellIsRemoteConnectedSet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "shellLock", (char*) shellLock, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "shellLogin", (char*) shellLogin, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "shellLoginInstall", (char*) shellLoginInstall, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "shellLogout", (char*) shellLogout, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "shellLogoutInstall", (char*) shellLogoutInstall, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "shellOrigStdSet", (char*) shellOrigStdSet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "shellPromptSet", (char*) shellPromptSet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "shellRestart", (char*) shellRestart, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "shellScriptAbort", (char*) shellScriptAbort, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "shellTaskId", (char*) &shellTaskId, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "shellTaskName", (char*) &shellTaskName, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "shellTaskOptions", (char*) &shellTaskOptions, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "shellTaskPriority", (char*) &shellTaskPriority, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "shellTaskStackSize", (char*) &shellTaskStackSize, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "show", (char*) show, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sigEvtRtn", (char*) &sigEvtRtn, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "sigInit", (char*) sigInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sigPendDestroy", (char*) sigPendDestroy, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sigPendInit", (char*) sigPendInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sigPendKill", (char*) sigPendKill, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sigaction", (char*) sigaction, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sigaddset", (char*) sigaddset, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sigblock", (char*) sigblock, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sigdelset", (char*) sigdelset, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sigemptyset", (char*) sigemptyset, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sigfillset", (char*) sigfillset, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sigismember", (char*) sigismember, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "signal", (char*) signal, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sigpending", (char*) sigpending, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sigprocmask", (char*) sigprocmask, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sigqueue", (char*) sigqueue, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sigqueueInit", (char*) sigqueueInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sigreturn", (char*) sigreturn, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sigsetmask", (char*) sigsetmask, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sigsuspend", (char*) sigsuspend, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sigtimedwait", (char*) sigtimedwait, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sigvec", (char*) sigvec, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sigwait", (char*) sigwait, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sigwaitinfo", (char*) sigwaitinfo, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "simBlockSigs", (char*) &simBlockSigs, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "simReadSelect", (char*) simReadSelect, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sin", (char*) sin, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sinh", (char*) sinh, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sllCount", (char*) sllCount, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sllCreate", (char*) sllCreate, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sllDelete", (char*) sllDelete, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sllEach", (char*) sllEach, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sllGet", (char*) sllGet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sllInit", (char*) sllInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sllPrevious", (char*) sllPrevious, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sllPutAtHead", (char*) sllPutAtHead, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sllPutAtTail", (char*) sllPutAtTail, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sllRemove", (char*) sllRemove, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sllTerminate", (char*) sllTerminate, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "smMemPartAddToPoolRtn", (char*) &smMemPartAddToPoolRtn, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "smMemPartAllocRtn", (char*) &smMemPartAllocRtn, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "smMemPartFindMaxRtn", (char*) &smMemPartFindMaxRtn, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "smMemPartFreeRtn", (char*) &smMemPartFreeRtn, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "smMemPartOptionsSetRtn", (char*) &smMemPartOptionsSetRtn, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "smMemPartReallocRtn", (char*) &smMemPartReallocRtn, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "smMemPartShowRtn", (char*) &smMemPartShowRtn, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "smObjPoolMinusOne", (char*) &smObjPoolMinusOne, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "smObjTaskDeleteFailRtn", (char*) &smObjTaskDeleteFailRtn, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "smObjTcbFreeFailRtn", (char*) &smObjTcbFreeFailRtn, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "smObjTcbFreeRtn", (char*) &smObjTcbFreeRtn, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "so", (char*) so, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sp", (char*) sp, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "spTaskOptions", (char*) &spTaskOptions, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "spTaskPriority", (char*) &spTaskPriority, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "spTaskStackSize", (char*) &spTaskStackSize, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "sprintf", (char*) sprintf, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "spy", (char*) spy, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "spyClkStart", (char*) spyClkStart, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "spyClkStop", (char*) spyClkStop, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "spyHelp", (char*) spyHelp, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "spyReport", (char*) spyReport, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "spyStop", (char*) spyStop, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "spyTask", (char*) spyTask, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sqrt", (char*) sqrt, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "srand", (char*) srand, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sscanf", (char*) sscanf, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "standAloneSymTbl", (char*) &standAloneSymTbl, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "stat", (char*) stat, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "statSymTbl", (char*) &statSymTbl, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "statfs", (char*) statfs, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "stdioFiles", (char*) &stdioFiles, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "stdioFp", (char*) stdioFp, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "stdioFpCreate", (char*) stdioFpCreate, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "stdioFpDestroy", (char*) stdioFpDestroy, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "stdioInit", (char*) stdioInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "stdioShow", (char*) stdioShow, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "stdioShowInit", (char*) stdioShowInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "stdlibFiles", (char*) &stdlibFiles, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "strcat", (char*) strcat, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "strchr", (char*) strchr, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "strcmp", (char*) strcmp, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "strcoll", (char*) strcoll, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "strcpy", (char*) strcpy, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "strcspn", (char*) strcspn, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "strerror", (char*) strerror, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "strerror_r", (char*) strerror_r, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "strftime", (char*) strftime, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "stringFiles", (char*) &stringFiles, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "strlen", (char*) strlen, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "strncat", (char*) strncat, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "strncmp", (char*) strncmp, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "strncpy", (char*) strncpy, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "strpbrk", (char*) strpbrk, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "strrchr", (char*) strrchr, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "strspn", (char*) strspn, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "strstr", (char*) strstr, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "strtod", (char*) strtod, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "strtok", (char*) strtok, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "strtok_r", (char*) strtok_r, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "strtol", (char*) strtol, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "strtoul", (char*) strtoul, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "strxfrm", (char*) strxfrm, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "substrcmp", (char*) substrcmp, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "swab", (char*) swab, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "symAdd", (char*) symAdd, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "symAlloc", (char*) symAlloc, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "symByCNameFind", (char*) symByCNameFind, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "symByValueAndTypeFind", (char*) symByValueAndTypeFind, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "symByValueFind", (char*) symByValueFind, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "symEach", (char*) symEach, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "symFindByCName", (char*) symFindByCName, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "symFindByName", (char*) symFindByName, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "symFindByNameAndType", (char*) symFindByNameAndType, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "symFindByValue", (char*) symFindByValue, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "symFindByValueAndType", (char*) symFindByValueAndType, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "symFindSymbol", (char*) symFindSymbol, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "symFree", (char*) symFree, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "symGroupDefault", (char*) &symGroupDefault, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "symInit", (char*) symInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "symLibInit", (char*) symLibInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "symLkupPgSz", (char*) &symLkupPgSz, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "symName", (char*) symName, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "symNameGet", (char*) symNameGet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "symRemove", (char*) symRemove, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "symSAdd", (char*) symSAdd, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "symShow", (char*) symShow, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "symShowInit", (char*) symShowInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "symTblAdd", (char*) symTblAdd, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "symTblClassId", (char*) &symTblClassId, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "symTblCreate", (char*) symTblCreate, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "symTblDelete", (char*) symTblDelete, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "symTblDestroy", (char*) symTblDestroy, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "symTblInit", (char*) symTblInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "symTblRemove", (char*) symTblRemove, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "symTblTerminate", (char*) symTblTerminate, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "symTypeGet", (char*) symTypeGet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "symValueGet", (char*) symValueGet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "syncLoadRtn", (char*) &syncLoadRtn, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "syncSymAddRtn", (char*) &syncSymAddRtn, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "syncSymRemoveRtn", (char*) &syncSymRemoveRtn, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "sysAdaEnable", (char*) &sysAdaEnable, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "sysAuxClkConnect", (char*) sysAuxClkConnect, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sysAuxClkDisable", (char*) sysAuxClkDisable, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sysAuxClkEnable", (char*) sysAuxClkEnable, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sysAuxClkRateGet", (char*) sysAuxClkRateGet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sysAuxClkRateSet", (char*) sysAuxClkRateSet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sysBootFile", (char*) &sysBootFile, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "sysBootHost", (char*) &sysBootHost, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "sysBootLine", (char*) &sysBootLine, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "sysBootLineAdrs", (char*) &sysBootLineAdrs, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "sysBootLineMagic", (char*) &sysBootLineMagic, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "sysBootLineMagicAdrs", (char*) &sysBootLineMagicAdrs, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "sysBootParams", (char*) &sysBootParams, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "sysBspRev", (char*) sysBspRev, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sysBusIntAck", (char*) sysBusIntAck, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sysBusIntGen", (char*) sysBusIntGen, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sysBusTas", (char*) sysBusTas, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sysBusToLocalAdrs", (char*) sysBusToLocalAdrs, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sysCacheLibInit", (char*) &sysCacheLibInit, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "sysClkConnect", (char*) sysClkConnect, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sysClkDisable", (char*) sysClkDisable, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sysClkEnable", (char*) sysClkEnable, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sysClkRateGet", (char*) sysClkRateGet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sysClkRateSet", (char*) sysClkRateSet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sysCplusEnable", (char*) &sysCplusEnable, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "sysCpu", (char*) &sysCpu, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "sysExcMsg", (char*) &sysExcMsg, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "sysExcMsgAdrs", (char*) &sysExcMsgAdrs, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "sysFlags", (char*) &sysFlags, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "sysHardSqrt", (char*) &sysHardSqrt, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "sysHwInit", (char*) sysHwInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sysHwInit2", (char*) sysHwInit2, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sysIntDisable", (char*) sysIntDisable, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sysIntEnable", (char*) sysIntEnable, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sysIntLvlDisableRtn", (char*) &sysIntLvlDisableRtn, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "sysIntLvlEnableRtn", (char*) &sysIntLvlEnableRtn, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "sysLocalToBusAdrs", (char*) sysLocalToBusAdrs, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sysMailboxConnect", (char*) sysMailboxConnect, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sysMailboxEnable", (char*) sysMailboxEnable, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sysMemBaseAdrs", (char*) &sysMemBaseAdrs, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "sysMemSize", (char*) &sysMemSize, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "sysMemTop", (char*) sysMemTop, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sysModel", (char*) sysModel, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sysNvRamGet", (char*) sysNvRamGet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sysNvRamSet", (char*) sysNvRamSet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sysPhysMemTop", (char*) sysPhysMemTop, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sysProcNum", (char*) &sysProcNum, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "sysProcNumGet", (char*) sysProcNumGet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sysProcNumSet", (char*) sysProcNumSet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sysSerialChanGet", (char*) sysSerialChanGet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sysSerialHwInit", (char*) sysSerialHwInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sysSerialHwInit2", (char*) sysSerialHwInit2, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sysSerialReset", (char*) sysSerialReset, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "sysShmid", (char*) &sysShmid, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "sysSioChans", (char*) &sysSioChans, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "sysSmAddr", (char*) &sysSmAddr, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "sysStartType", (char*) &sysStartType, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "sysSymTbl", (char*) &sysSymTbl, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "sysToMonitor", (char*) sysToMonitor, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "system", (char*) system, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "tan", (char*) tan, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "tanh", (char*) tanh, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "taskActivate", (char*) taskActivate, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "taskArgsGet", (char*) taskArgsGet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "taskArgsSet", (char*) taskArgsSet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "taskBpHook", (char*) &taskBpHook, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "taskBpHookSet", (char*) taskBpHookSet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "taskClassId", (char*) &taskClassId, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "taskCreat", (char*) taskCreat, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "taskCreateHookAdd", (char*) taskCreateHookAdd, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "taskCreateHookDelete", (char*) taskCreateHookDelete, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "taskCreateHookShow", (char*) taskCreateHookShow, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "taskCreateTable", (char*) &taskCreateTable, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "taskDelay", (char*) taskDelay, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "taskDelete", (char*) taskDelete, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "taskDeleteForce", (char*) taskDeleteForce, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "taskDeleteHookAdd", (char*) taskDeleteHookAdd, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "taskDeleteHookDelete", (char*) taskDeleteHookDelete, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "taskDeleteHookShow", (char*) taskDeleteHookShow, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "taskDeleteTable", (char*) &taskDeleteTable, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "taskDestroy", (char*) taskDestroy, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "taskHookInit", (char*) taskHookInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "taskHookShowInit", (char*) taskHookShowInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "taskIdCurrent", (char*) &taskIdCurrent, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "taskIdDefault", (char*) taskIdDefault, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "taskIdFigure", (char*) taskIdFigure, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "taskIdListGet", (char*) taskIdListGet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "taskIdListSort", (char*) taskIdListSort, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "taskIdSelf", (char*) taskIdSelf, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "taskIdVerify", (char*) taskIdVerify, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "taskInfoGet", (char*) taskInfoGet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "taskInit", (char*) taskInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "taskInstClassId", (char*) &taskInstClassId, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "taskIsReady", (char*) taskIsReady, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "taskIsSuspended", (char*) taskIsSuspended, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "taskLibInit", (char*) taskLibInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "taskLock", (char*) taskLock, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "taskName", (char*) taskName, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "taskNameToId", (char*) taskNameToId, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "taskOptionsGet", (char*) taskOptionsGet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "taskOptionsSet", (char*) taskOptionsSet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "taskOptionsString", (char*) taskOptionsString, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "taskPriRangeCheck", (char*) &taskPriRangeCheck, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "taskPriorityGet", (char*) taskPriorityGet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "taskPrioritySet", (char*) taskPrioritySet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "taskRegName", (char*) &taskRegName, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "taskRegsFmt", (char*) &taskRegsFmt, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "taskRegsGet", (char*) taskRegsGet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "taskRegsInit", (char*) taskRegsInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "taskRegsSet", (char*) taskRegsSet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "taskRegsShow", (char*) taskRegsShow, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "taskRestart", (char*) taskRestart, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "taskResume", (char*) taskResume, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "taskRtnValueSet", (char*) taskRtnValueSet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "taskSafe", (char*) taskSafe, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "taskShow", (char*) taskShow, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "taskShowInit", (char*) taskShowInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "taskSpawn", (char*) taskSpawn, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "taskStackAllot", (char*) taskStackAllot, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "taskStatusString", (char*) taskStatusString, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "taskSuspend", (char*) taskSuspend, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "taskSwapHookAdd", (char*) taskSwapHookAdd, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "taskSwapHookAttach", (char*) taskSwapHookAttach, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "taskSwapHookDelete", (char*) taskSwapHookDelete, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "taskSwapHookDetach", (char*) taskSwapHookDetach, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "taskSwapHookShow", (char*) taskSwapHookShow, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "taskSwapReference", (char*) &taskSwapReference, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "taskSwapTable", (char*) &taskSwapTable, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "taskSwitchHookAdd", (char*) taskSwitchHookAdd, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "taskSwitchHookDelete", (char*) taskSwitchHookDelete, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "taskSwitchHookShow", (char*) taskSwitchHookShow, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "taskSwitchTable", (char*) &taskSwitchTable, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "taskTcb", (char*) taskTcb, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "taskTerminate", (char*) taskTerminate, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "taskUndelay", (char*) taskUndelay, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "taskUnlock", (char*) taskUnlock, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "taskUnsafe", (char*) taskUnsafe, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "taskVarAdd", (char*) taskVarAdd, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "taskVarDelete", (char*) taskVarDelete, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "taskVarGet", (char*) taskVarGet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "taskVarInfo", (char*) taskVarInfo, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "taskVarInit", (char*) taskVarInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "taskVarSet", (char*) taskVarSet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "td", (char*) td, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "ti", (char*) ti, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "tick64Get", (char*) tick64Get, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "tick64Set", (char*) tick64Set, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "tickAnnounce", (char*) tickAnnounce, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "tickGet", (char*) tickGet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "tickQHead", (char*) &tickQHead, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "tickSet", (char*) tickSet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "time", (char*) time, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "timeFiles", (char*) &timeFiles, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "timex", (char*) timex, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "timexClear", (char*) timexClear, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "timexFunc", (char*) timexFunc, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "timexHelp", (char*) timexHelp, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "timexInit", (char*) timexInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "timexN", (char*) timexN, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "timexPost", (char*) timexPost, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "timexPre", (char*) timexPre, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "timexShow", (char*) timexShow, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "tmpfile", (char*) tmpfile, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "tmpnam", (char*) tmpnam, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "tolower", (char*) tolower, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "toupper", (char*) toupper, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "tr", (char*) tr, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "trcDefaultArgs", (char*) &trcDefaultArgs, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "trcStack", (char*) trcStack, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "trgActionDefMsgQId", (char*) &trgActionDefMsgQId, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "trgActionDefPerform", (char*) trgActionDefPerform, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "trgActionDefStart", (char*) trgActionDefStart, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "trgActionPerform", (char*) trgActionPerform, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "trgAdd", (char*) trgAdd, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "trgAddTcl", (char*) trgAddTcl, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "trgChainSet", (char*) trgChainSet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "trgCheck", (char*) trgCheck, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "trgClass", (char*) &trgClass, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "trgClassId", (char*) &trgClassId, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "trgClassList", (char*) &trgClassList, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "trgCnt", (char*) &trgCnt, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "trgCondTest", (char*) trgCondTest, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "trgContextMatch", (char*) trgContextMatch, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "trgDefSem", (char*) &trgDefSem, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "trgDelete", (char*) trgDelete, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "trgDisable", (char*) trgDisable, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "trgEnable", (char*) trgEnable, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "trgEvent", (char*) trgEvent, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "trgEvtClass", (char*) &trgEvtClass, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "trgEvtToIndex", (char*) trgEvtToIndex, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "trgInit", (char*) trgInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "trgLibInit", (char*) trgLibInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "trgList", (char*) &trgList, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "trgNone", (char*) &trgNone, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "trgOff", (char*) trgOff, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "trgOn", (char*) trgOn, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "trgShow", (char*) trgShow, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "trgShowInit", (char*) trgShowInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "trgWorkQ", (char*) &trgWorkQ, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "trgWorkQFullNotify", (char*) &trgWorkQFullNotify, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "trgWorkQReader", (char*) &trgWorkQReader, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "trgWorkQReset", (char*) trgWorkQReset, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "trgWorkQWriter", (char*) &trgWorkQWriter, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "ts", (char*) ts, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "tsfsUploadPathClose", (char*) tsfsUploadPathClose, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "tsfsUploadPathCreate", (char*) tsfsUploadPathCreate, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "tsfsUploadPathLibInit", (char*) tsfsUploadPathLibInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "tsfsUploadPathWrite", (char*) tsfsUploadPathWrite, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "tt", (char*) tt, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "ttyDevCreate", (char*) ttyDevCreate, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "ttyDrv", (char*) ttyDrv, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "tyAbortFuncSet", (char*) tyAbortFuncSet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "tyAbortSet", (char*) tyAbortSet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "tyBackspaceChar", (char*) &tyBackspaceChar, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "tyBackspaceSet", (char*) tyBackspaceSet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "tyCoDv", (char*) &tyCoDv, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "tyDeleteLineChar", (char*) &tyDeleteLineChar, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "tyDeleteLineSet", (char*) tyDeleteLineSet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "tyDevInit", (char*) tyDevInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "tyDevRemove", (char*) tyDevRemove, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "tyEOFSet", (char*) tyEOFSet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "tyEofChar", (char*) &tyEofChar, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "tyIRd", (char*) tyIRd, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "tyITx", (char*) tyITx, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "tyIoctl", (char*) tyIoctl, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "tyMonitorTrapSet", (char*) tyMonitorTrapSet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "tyRead", (char*) tyRead, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "tyWrite", (char*) tyWrite, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "u_argvsave", (char*) &u_argvsave, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "u_close", (char*) u_close, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "u_closedir", (char*) u_closedir, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "u_connect", (char*) u_connect, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "u_cwd", (char*) &u_cwd, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "u_dup2", (char*) u_dup2, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "u_errno", (char*) u_errno, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "u_execl", (char*) u_execl, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "u_execlp", (char*) u_execlp, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "u_execvp", (char*) u_execvp, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "u_exit", (char*) u_exit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "u_fcntl", (char*) u_fcntl, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "u_fork", (char*) u_fork, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "u_free", (char*) u_free, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "u_fstat", (char*) u_fstat, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "u_getcwd", (char*) u_getcwd, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "u_getenv", (char*) u_getenv, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "u_getgid", (char*) u_getgid, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "u_gethostbyname", (char*) u_gethostbyname, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "u_gethostname", (char*) u_gethostname, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "u_getpgrp", (char*) u_getpgrp, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "u_getpid", (char*) u_getpid, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "u_getppid", (char*) u_getppid, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "u_getuid", (char*) u_getuid, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "u_getwd", (char*) u_getwd, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "u_hostip", (char*) &u_hostip, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "u_hostname", (char*) &u_hostname, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "u_ioctl", (char*) u_ioctl, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "u_isatty", (char*) u_isatty, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "u_kill", (char*) u_kill, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "u_localtime", (char*) u_localtime, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "u_lockf", (char*) u_lockf, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "u_lseek", (char*) u_lseek, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "u_makecontext", (char*) u_makecontext, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "u_malloc", (char*) u_malloc, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "u_mkdir", (char*) u_mkdir, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "u_mkfifo", (char*) u_mkfifo, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "u_open", (char*) u_open, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "u_opendir", (char*) u_opendir, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "u_printf", (char*) u_printf, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "u_progname", (char*) &u_progname, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "u_read", (char*) u_read, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "u_readdir", (char*) u_readdir, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "u_rename", (char*) u_rename, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "u_rmdir", (char*) u_rmdir, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "u_select", (char*) u_select, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "u_setitimer", (char*) u_setitimer, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "u_setsid", (char*) u_setsid, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "u_shmat", (char*) u_shmat, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "u_shmctl", (char*) u_shmctl, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "u_shmget", (char*) u_shmget, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "u_sigaction", (char*) u_sigaction, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "u_sigaddset", (char*) u_sigaddset, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "u_sigaltstack", (char*) u_sigaltstack, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "u_sigdelset", (char*) u_sigdelset, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "u_sigemptyset", (char*) u_sigemptyset, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "u_sigfillset", (char*) u_sigfillset, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "u_sigismember", (char*) u_sigismember, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "u_signal", (char*) u_signal, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "u_sigprocmask", (char*) u_sigprocmask, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "u_sigsuspend", (char*) u_sigsuspend, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "u_sigsuspendflag", (char*) &u_sigsuspendflag, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "u_socket", (char*) u_socket, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "u_stat", (char*) u_stat, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "u_system", (char*) u_system, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "u_tcgetpgrp", (char*) u_tcgetpgrp, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "u_tcsetpgrp", (char*) u_tcsetpgrp, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "u_time", (char*) u_time, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "u_uname", (char*) u_uname, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "u_unlink", (char*) u_unlink, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "u_username", (char*) &u_username, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "u_usleep", (char*) u_usleep, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "u_wait", (char*) u_wait, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "u_write", (char*) u_write, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "udpCommIfInit", (char*) udpCommIfInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "udpRcv", (char*) udpRcv, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "ulDebug", (char*) &ulDebug, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "ungetc", (char*) ungetc, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "unixChan", (char*) &unixChan, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "unixDevInit", (char*) unixDevInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "unixDevInit2", (char*) unixDevInit2, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "unixIntRcv", (char*) unixIntRcv, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "unlink", (char*) unlink, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "usrBreakpointSet", (char*) usrBreakpointSet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "usrClock", (char*) usrClock, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "usrExtraModules", (char*) &usrExtraModules, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "usrInit", (char*) usrInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "usrKernelInit", (char*) usrKernelInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "usrRoot", (char*) usrRoot, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "uswab", (char*) uswab, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "utime", (char*) utime, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "valloc", (char*) valloc, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "version", (char*) version, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "vfdprintf", (char*) vfdprintf, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "vfprintf", (char*) vfprintf, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "vmLibInfo", (char*) &vmLibInfo, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "vprintf", (char*) vprintf, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "vsprintf", (char*) vsprintf, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "vxAbsTicks", (char*) &vxAbsTicks, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "vxEventPendQ", (char*) &vxEventPendQ, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "vxIntStackBase", (char*) &vxIntStackBase, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "vxIntStackEnd", (char*) &vxIntStackEnd, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "vxMemArchProbe", (char*) vxMemArchProbe, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "vxMemProbe", (char*) vxMemProbe, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "vxTas", (char*) vxTas, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "vxTaskEntry", (char*) vxTaskEntry, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "vxTicks", (char*) &vxTicks, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "vxWorksVersion", (char*) &vxWorksVersion, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "vxsim_bootdev", (char*) &vxsim_bootdev, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "vxsim_cwd", (char*) &vxsim_cwd, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "vxsim_hostname", (char*) &vxsim_hostname, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "vxsim_ip_name", (char*) &vxsim_ip_name, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "wcstombs", (char*) wcstombs, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wctomb", (char*) wctomb, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wdCancel", (char*) wdCancel, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wdClassId", (char*) &wdClassId, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "wdCreate", (char*) wdCreate, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wdDelete", (char*) wdDelete, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wdDestroy", (char*) wdDestroy, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wdInit", (char*) wdInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wdInstClassId", (char*) &wdInstClassId, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "wdLibInit", (char*) wdLibInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wdShow", (char*) wdShow, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wdShowInit", (char*) wdShowInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wdStart", (char*) wdStart, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wdTerminate", (char*) wdTerminate, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wdTick", (char*) wdTick, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wdbBpInstall", (char*) wdbBpInstall, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wdbCksum", (char*) wdbCksum, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wdbCommMtu", (char*) &wdbCommMtu, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "wdbConfig", (char*) wdbConfig, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wdbConnectLibInit", (char*) wdbConnectLibInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wdbCtxCreate", (char*) wdbCtxCreate, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wdbCtxExitLibInit", (char*) wdbCtxExitLibInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wdbCtxExitNotifyHook", (char*) wdbCtxExitNotifyHook, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wdbCtxLibInit", (char*) wdbCtxLibInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wdbCtxResume", (char*) wdbCtxResume, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wdbCtxStartLibInit", (char*) wdbCtxStartLibInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wdbDbgArchInit", (char*) wdbDbgArchInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wdbDbgBpFind", (char*) wdbDbgBpFind, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wdbDbgBpGet", (char*) wdbDbgBpGet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wdbDbgBpListInit", (char*) wdbDbgBpListInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wdbDbgBpRemove", (char*) wdbDbgBpRemove, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wdbDbgBpRemoveAll", (char*) wdbDbgBpRemoveAll, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wdbDbgGetNpc", (char*) wdbDbgGetNpc, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wdbDbgTraceModeClear", (char*) wdbDbgTraceModeClear, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wdbDbgTraceModeSet", (char*) wdbDbgTraceModeSet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wdbDbgTrap", (char*) wdbDbgTrap, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wdbDirectCallLibInit", (char*) wdbDirectCallLibInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wdbE", (char*) wdbE, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wdbEventDeq", (char*) wdbEventDeq, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wdbEventLibInit", (char*) wdbEventLibInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wdbEventNodeInit", (char*) wdbEventNodeInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wdbEventPost", (char*) wdbEventPost, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wdbEvtptClassConnect", (char*) wdbEvtptClassConnect, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wdbEvtptLibInit", (char*) wdbEvtptLibInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wdbExcLibInit", (char*) wdbExcLibInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wdbExternEnterHook", (char*) wdbExternEnterHook, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wdbExternExitHook", (char*) wdbExternExitHook, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wdbExternInit", (char*) wdbExternInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wdbExternRegSetObjAdd", (char*) wdbExternRegSetObjAdd, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wdbExternRegsGet", (char*) wdbExternRegsGet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wdbExternRegsSet", (char*) wdbExternRegsSet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wdbExternSystemRegs", (char*) &wdbExternSystemRegs, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "wdbFpLibInit", (char*) wdbFpLibInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wdbFppGet", (char*) wdbFppGet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wdbFppRestore", (char*) wdbFppRestore, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wdbFppSave", (char*) wdbFppSave, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wdbFppSet", (char*) wdbFppSet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wdbFuncCallLibInit", (char*) wdbFuncCallLibInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wdbGopherLibInit", (char*) wdbGopherLibInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wdbGopherLock", (char*) &wdbGopherLock, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "wdbInfoGet", (char*) wdbInfoGet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wdbInstallCommIf", (char*) wdbInstallCommIf, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wdbInstallRtIf", (char*) wdbInstallRtIf, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wdbIsNowExternal", (char*) wdbIsNowExternal, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wdbIsNowTasking", (char*) wdbIsNowTasking, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wdbMbufAlloc", (char*) wdbMbufAlloc, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wdbMbufFree", (char*) wdbMbufFree, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wdbMemCoreLibInit", (char*) wdbMemCoreLibInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wdbMemLibInit", (char*) wdbMemLibInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wdbMemTest", (char*) wdbMemTest, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wdbModeSet", (char*) wdbModeSet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wdbNotifyHost", (char*) wdbNotifyHost, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wdbNumMemRegions", (char*) &wdbNumMemRegions, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "wdbOneShot", (char*) &wdbOneShot, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "wdbPipePktDevInit", (char*) wdbPipePktDevInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wdbRegsLibInit", (char*) wdbRegsLibInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wdbResumeSystem", (char*) wdbResumeSystem, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wdbRpcGetArgs", (char*) wdbRpcGetArgs, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wdbRpcNotifyConnect", (char*) wdbRpcNotifyConnect, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wdbRpcNotifyHost", (char*) wdbRpcNotifyHost, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wdbRpcRcv", (char*) wdbRpcRcv, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wdbRpcReply", (char*) wdbRpcReply, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wdbRpcReplyErr", (char*) wdbRpcReplyErr, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wdbRpcResendReply", (char*) wdbRpcResendReply, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wdbRpcXportInit", (char*) wdbRpcXportInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wdbRunsExternal", (char*) wdbRunsExternal, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wdbRunsTasking", (char*) wdbRunsTasking, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wdbSp", (char*) wdbSp, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wdbSuspendSystem", (char*) wdbSuspendSystem, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wdbSuspendSystemHere", (char*) wdbSuspendSystemHere, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wdbSvcAdd", (char*) wdbSvcAdd, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wdbSvcDispatch", (char*) wdbSvcDispatch, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wdbSvcDsaSvcRemove", (char*) wdbSvcDsaSvcRemove, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wdbSvcHookAdd", (char*) wdbSvcHookAdd, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wdbSvcLibInit", (char*) wdbSvcLibInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wdbSysBpLibInit", (char*) wdbSysBpLibInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wdbSystemSuspend", (char*) wdbSystemSuspend, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wdbTargetIsConnected", (char*) wdbTargetIsConnected, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wdbTask", (char*) wdbTask, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wdbTaskBpLibInit", (char*) wdbTaskBpLibInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wdbTaskId", (char*) &wdbTaskId, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "wdbTaskInit", (char*) wdbTaskInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wdbToolName", (char*) &wdbToolName, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "wdbTsfsDefaultDirPerm", (char*) &wdbTsfsDefaultDirPerm, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "wdbTsfsDrv", (char*) wdbTsfsDrv, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wdbUserEvtLibInit", (char*) wdbUserEvtLibInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wdbUserEvtPost", (char*) wdbUserEvtPost, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wdbVioChannelRegister", (char*) wdbVioChannelRegister, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wdbVioChannelUnregister", (char*) wdbVioChannelUnregister, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wdbVioDrv", (char*) wdbVioDrv, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wdbVioLibInit", (char*) wdbVioLibInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wim", (char*) wim, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "windDelay", (char*) windDelay, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "windDelete", (char*) windDelete, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "windExit", (char*) windExit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "windIntStackSet", (char*) windIntStackSet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "windLoadContext", (char*) windLoadContext, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "windPendQFlush", (char*) windPendQFlush, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "windPendQGet", (char*) windPendQGet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "windPendQPut", (char*) windPendQPut, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "windPendQRemove", (char*) windPendQRemove, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "windPendQTerminate", (char*) windPendQTerminate, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "windPriNormalSet", (char*) windPriNormalSet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "windPrioritySet", (char*) windPrioritySet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "windReadyQPut", (char*) windReadyQPut, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "windReadyQRemove", (char*) windReadyQRemove, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "windResume", (char*) windResume, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "windSemDelete", (char*) windSemDelete, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "windSpawn", (char*) windSpawn, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "windSuspend", (char*) windSuspend, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "windTickAnnounce", (char*) windTickAnnounce, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "windUndelay", (char*) windUndelay, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "windWdCancel", (char*) windWdCancel, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "windWdStart", (char*) windWdStart, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "windviewConfig", (char*) windviewConfig, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "workQAdd0", (char*) workQAdd0, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "workQAdd1", (char*) workQAdd1, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "workQAdd2", (char*) workQAdd2, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "workQDoWork", (char*) workQDoWork, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "workQInit", (char*) workQInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "workQIsEmpty", (char*) &workQIsEmpty, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "workQPanic", (char*) workQPanic, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "workQReadIx", (char*) &workQReadIx, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "workQWriteIx", (char*) &workQWriteIx, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "write", (char*) write, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "writev", (char*) writev, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wvBufId", (char*) &wvBufId, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "wvDefaultBufMax", (char*) &wvDefaultBufMax, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "wvDefaultBufMin", (char*) &wvDefaultBufMin, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "wvDefaultBufOptions", (char*) &wvDefaultBufOptions, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "wvDefaultBufSize", (char*) &wvDefaultBufSize, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "wvDefaultBufThresh", (char*) &wvDefaultBufThresh, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "wvDefaultRBuffParams", (char*) &wvDefaultRBuffParams, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "wvEvent", (char*) wvEvent, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wvEventInst", (char*) wvEventInst, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wvEvtBufferFullNotify", (char*) &wvEvtBufferFullNotify, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "wvEvtBufferGet", (char*) wvEvtBufferGet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wvEvtClass", (char*) &wvEvtClass, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "wvEvtClassClear", (char*) wvEvtClassClear, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wvEvtClassClearAll", (char*) wvEvtClassClearAll, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wvEvtClassGet", (char*) wvEvtClassGet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wvEvtClassSet", (char*) wvEvtClassSet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wvEvtLogInit", (char*) wvEvtLogInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wvEvtLogStart", (char*) wvEvtLogStart, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wvEvtLogStop", (char*) wvEvtLogStop, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wvInstIsOn", (char*) &wvInstIsOn, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "wvLibInit", (char*) wvLibInit, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wvLibInit2", (char*) wvLibInit2, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wvLogHeader", (char*) &wvLogHeader, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "wvLogHeaderCreate", (char*) wvLogHeaderCreate, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wvLogHeaderUpload", (char*) wvLogHeaderUpload, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wvObjInst", (char*) wvObjInst, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wvObjInstModeSet", (char*) wvObjInstModeSet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wvObjIsEnabled", (char*) &wvObjIsEnabled, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "wvOff", (char*) wvOff, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wvOn", (char*) wvOn, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wvRBuffErrorHandler", (char*) wvRBuffErrorHandler, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wvRBuffMgr", (char*) wvRBuffMgr, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wvRBuffMgrId", (char*) &wvRBuffMgrId, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "wvRBuffMgrPrioritySet", (char*) wvRBuffMgrPrioritySet, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wvSigInst", (char*) wvSigInst, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wvTaskNamesBufAdd", (char*) wvTaskNamesBufAdd, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wvTaskNamesPreserve", (char*) wvTaskNamesPreserve, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wvTaskNamesUpload", (char*) wvTaskNamesUpload, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wvTmrRegister", (char*) wvTmrRegister, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wvUpPathId", (char*) &wvUpPathId, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "wvUpTaskId", (char*) &wvUpTaskId, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "wvUploadMaxAttempts", (char*) &wvUploadMaxAttempts, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "wvUploadRetryBackoff", (char*) &wvUploadRetryBackoff, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "wvUploadStart", (char*) wvUploadStart, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wvUploadStop", (char*) wvUploadStop, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wvUploadTaskConfig", (char*) wvUploadTaskConfig, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "wvUploadTaskOptions", (char*) &wvUploadTaskOptions, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "wvUploadTaskPriority", (char*) &wvUploadTaskPriority, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "wvUploadTaskStackSize", (char*) &wvUploadTaskStackSize, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "xattrib", (char*) xattrib, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "xcopy", (char*) xcopy, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "xdelete", (char*) xdelete, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "xdrCksum", (char*) xdrCksum, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "xdr_ARRAY", (char*) xdr_ARRAY, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "xdr_CHECKSUM", (char*) xdr_CHECKSUM, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "xdr_TGT_ADDR_T", (char*) xdr_TGT_ADDR_T, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "xdr_TGT_INT_T", (char*) xdr_TGT_INT_T, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "xdr_UINT32", (char*) xdr_UINT32, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "xdr_WDB_AGENT_INFO", (char*) xdr_WDB_AGENT_INFO, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "xdr_WDB_CALL_RETURN_INFO", (char*) xdr_WDB_CALL_RETURN_INFO, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "xdr_WDB_CTX", (char*) xdr_WDB_CTX, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "xdr_WDB_CTX_CREATE_DESC", (char*) xdr_WDB_CTX_CREATE_DESC, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "xdr_WDB_CTX_STEP_DESC", (char*) xdr_WDB_CTX_STEP_DESC, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "xdr_WDB_EVTPT_ADD_DESC", (char*) xdr_WDB_EVTPT_ADD_DESC, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "xdr_WDB_EVTPT_DEL_DESC", (char*) xdr_WDB_EVTPT_DEL_DESC, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "xdr_WDB_EVT_DATA", (char*) xdr_WDB_EVT_DATA, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "xdr_WDB_EVT_INFO", (char*) xdr_WDB_EVT_INFO, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "xdr_WDB_MEM_REGION", (char*) xdr_WDB_MEM_REGION, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "xdr_WDB_MEM_SCAN_DESC", (char*) xdr_WDB_MEM_SCAN_DESC, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "xdr_WDB_MEM_XFER", (char*) xdr_WDB_MEM_XFER, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "xdr_WDB_OPQ_DATA_T", (char*) xdr_WDB_OPQ_DATA_T, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "xdr_WDB_PARAM_WRAPPER", (char*) xdr_WDB_PARAM_WRAPPER, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "xdr_WDB_REG_READ_DESC", (char*) xdr_WDB_REG_READ_DESC, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "xdr_WDB_REG_WRITE_DESC", (char*) xdr_WDB_REG_WRITE_DESC, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "xdr_WDB_REPLY_WRAPPER", (char*) xdr_WDB_REPLY_WRAPPER, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "xdr_WDB_RT_INFO", (char*) xdr_WDB_RT_INFO, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "xdr_WDB_STRING_T", (char*) xdr_WDB_STRING_T, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "xdr_WDB_TGT_INFO", (char*) xdr_WDB_TGT_INFO, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "xdr_bool", (char*) xdr_bool, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "xdr_bytes", (char*) xdr_bytes, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "xdr_char", (char*) xdr_char, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "xdr_double", (char*) xdr_double, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "xdr_enum", (char*) xdr_enum, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "xdr_float", (char*) xdr_float, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "xdr_floatInclude", (char*) xdr_floatInclude, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "xdr_free", (char*) xdr_free, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "xdr_int", (char*) xdr_int, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "xdr_long", (char*) xdr_long, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "xdr_netobj", (char*) xdr_netobj, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "xdr_opaque", (char*) xdr_opaque, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "xdr_short", (char*) xdr_short, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "xdr_string", (char*) xdr_string, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "xdr_u_char", (char*) xdr_u_char, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "xdr_u_int", (char*) xdr_u_int, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "xdr_u_long", (char*) xdr_u_long, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "xdr_u_short", (char*) xdr_u_short, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "xdr_union", (char*) xdr_union, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "xdr_void", (char*) xdr_void, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "xdr_wrapstring", (char*) xdr_wrapstring, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "xdrmem_create", (char*) xdrmem_create, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "y", (char*) y, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "yy_yys", (char*) &yy_yys, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "yy_yyv", (char*) &yy_yyv, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "yychar", (char*) &yychar, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "yydebug", (char*) &yydebug, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "yyerrflag", (char*) &yyerrflag, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "yylval", (char*) &yylval, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "yynerrs", (char*) &yynerrs, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "yyparse", (char*) yyparse, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "yyps", (char*) &yyps, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "yypv", (char*) &yypv, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "yys", (char*) &yys, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "yystart", (char*) yystart, 0, SYM_GLOBAL | SYM_TEXT},
        {{NULL}, "yystate", (char*) &yystate, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "yytmp", (char*) &yytmp, 0, SYM_GLOBAL | SYM_BSS},
        {{NULL}, "yyv", (char*) &yyv, 0, SYM_GLOBAL | SYM_DATA},
        {{NULL}, "yyval", (char*) &yyval, 0, SYM_GLOBAL | SYM_BSS},
    };


ULONG standTblSize = 2782;
