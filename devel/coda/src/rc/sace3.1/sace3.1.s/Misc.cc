#include "sysincludes.h"

// There must be a better way to do this...
#ifdef Linux
#ifdef RLIMIT_OFILE
#   define RLIMIT_NOFILE RLIMIT_OFILE
#else
#   define RLIMIT_NOFILE 200
#endif
#endif

/* Make the current process a UNIX daemon.  This is based 
   on Stevens code from APUE! */

int
ace_daemonize (void)
{
  pid_t pid;

  if ((pid = ::fork ()) == -1)
    return -1;
  else if (pid != 0)
    ::exit (0);			/* parent exits */

  /* child continues */
  ::setsid (); /* become session leader */

  ::chdir ("/");		/* change working directory */

  ::umask (0);			/* clear our file mode creation mask */
  return 0;
}

ssize_t
ace_send_n (HANDLE handle, const void *buf, size_t len)
{
  size_t bytes_written;
  int	 n;

  for (bytes_written = 0; bytes_written < len; bytes_written += n)
    if ((n = ::write (handle, (const char *) buf + bytes_written, 
		      len - bytes_written)) == -1)
      return -1;

  return bytes_written;
}

ssize_t
ace_send_n (HANDLE handle, const void *buf, size_t len, int flags)
{
  size_t bytes_written;
  int	 n;

  for (bytes_written = 0; bytes_written < len; bytes_written += n)
    if ((n = ::send (handle, (const char *) buf + bytes_written, 
		     len - bytes_written, flags)) == -1)
      return -1;

  return bytes_written;
}

ssize_t
ace_recv_n (HANDLE handle, void *buf, size_t len)
{
  size_t bytes_read;
  int	 n;

  for (bytes_read = 0; bytes_read < len; bytes_read += n)
    if ((n = ::read (handle, (char *) buf + bytes_read, 
		     len - bytes_read)) == -1)
      return -1;
    else if (n == 0)
      break;

  return bytes_read;      
}

ssize_t
ace_recv_n (HANDLE handle, void *buf, size_t len, int flags)
{
  size_t bytes_read;
  int	 n;

  for (bytes_read = 0; bytes_read < len; bytes_read += n)
    if ((n = ::recv (handle, (char *) buf + bytes_read, 
		     len - bytes_read, flags)) == -1)
      return -1;
    else if (n == 0)
      break;

  return bytes_read;      
}

/* Set the number of currently open handles in the process.

   If NEW_LIMIT == -1 set the limit to the maximum allowable.
   Otherwise, set it to be the value of NEW_LIMIT.

   Note the insanity that the maximum limit appears
   to be 200 open files, but NOFILE and FD_SETSIZE are
   both set to 256!!! */

int 
ace_set_handle_limit (int new_limit)
{
  struct rlimit rl;

#if defined (RLIMIT_NOFILE)
  if (::getrlimit (RLIMIT_NOFILE, &rl) == -1)
    return -1;
#else
  rl.rlim_cur = NOFILES_MAX;
#endif /* RLIMIT_NOFILE */

  if (new_limit < 0 || new_limit > rl.rlim_max)
    rl.rlim_cur = rl.rlim_max;
  else
    rl.rlim_cur = new_limit;

  return ::setrlimit (RLIMIT_NOFILE, &rl);
}

/* Flags are file status flags to turn on */

int
ace_set_fl (HANDLE fd, int flags)
{
  int val;

  if ((val = ::fcntl (fd, F_GETFL, 0)) == -1)
    return -1;

  val |= flags; /* turn on flags */

  if (::fcntl (fd, F_SETFL, val) == -1)
    return -1;
  else
    return 0;
}

/* Flags are the file status flags to turn off */

int
ace_clr_fl (HANDLE fd, int flags)
{
  int val;

  if ((val = ::fcntl (fd, F_GETFL, 0)) == -1)
    return -1;

  val &= ~flags; /* turn flags off */

  if (::fcntl (fd, F_SETFL, val) == -1)
    return -1;
  else
    return 0;
}

/* The following code is courtesy of Rich Salz and may
   be useful for odd systems that lack writev(). */

#if defined (ACE_NEEDS_WRITEV)
/*  $Revision: 1.1.1.1 $
**
*/
#include <stdio.h>
#include <sys/types.h>
#include <sys/uio.h>

/* Hacks since you don't have INN's header file. */
#define POINTER	char*
#define SIZE_T	int

/* Keep writing until everything has been written or we get an error. */
int
ace_xwrite(HANDLE fd, char *p, int i)
{
  int c;

  for ( ; i; p += c, i -= c)
    if ((c = ::write (fd, (POINTER) p, (SIZE_T) i)) <= 0)
      return -1;
  return 0;
}
/* "Fake" writev for sites without it. */
int
writev (HANDLE fd, struct iovec *vp, int vpcount)
{
  int count;

  for (count = 0; --vpcount >= 0; count += vp->iov_len, vp++)
    if (ace_xwrite (fd, vp->iov_base, vp->iov_len) < 0)
      return -1;
  return count;
}
#endif /* ACE_NEEDS_WRITEV */
