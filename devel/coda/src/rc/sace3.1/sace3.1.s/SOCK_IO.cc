#include "SOCK_IO.h"

/* Send N char *ptrs and int lengths.  Note that the char *'s 
   precede the ints (basically, an varargs version of writev). 
   The count N is the *total* number of trailing arguments, 
   *not* a couple of the number of tuple pairs! */

ssize_t
SOCK_IO::send (size_t n, ...) const
{

  va_list argp;  
  int	  total_tuples = n / 2;
  ssize_t result;
#if defined (ACE_HAS_ALLOCA)
  iovec   *iovp = (iovec *) alloca (total_tuples);
#else
  iovec	  *iovp = new iovec[total_tuples];
#endif /* !defined (ACE_HAS_ALLOCA) */

  va_start (argp, n);

  for (size_t i = 0; i < total_tuples; i++)
    {
      iovp[i].iov_base = va_arg (argp, char *);
      iovp[i].iov_len  = va_arg (argp, int);
    }

  result = ::writev (this->get_handle (), iovp, total_tuples);
#if !defined (ACE_HAS_ALLOCA)
  delete iovp;
#endif /* !defined (ACE_HAS_ALLOCA) */
  va_end (argp);
  return result;
}

/* This is basically an interface to ::readv, that doesn't
   use the struct iovec explicitly.  The ... can be passed
   as an arbitrary number of (char *ptr, int len) tuples.
   However, the count N is the *total* number of trailing
   arguments, *not* a couple of the number of tuple pairs! */

ssize_t
SOCK_IO::recv (size_t n, ...) const
{

  va_list argp;  
  int	  total_tuples = n / 2;
  ssize_t result;
#if defined (ACE_HAS_ALLOCA)
  iovec   *iovp = (iovec *) alloca (total_tuples);
#else
  iovec	  *iovp = new iovec[total_tuples];
#endif /* !defined (ACE_HAS_ALLOCA) */

  va_start (argp, n);

  for (size_t i = 0; i < total_tuples; i++)
    {
      iovp[i].iov_base = va_arg (argp, char *);
      iovp[i].iov_len  = va_arg (argp, int);
    }

  result = ::readv (this->get_handle (), iovp, total_tuples);
#if !defined (ACE_HAS_ALLOCA)
  delete iovp;
#endif /* !defined (ACE_HAS_ALLOCA) */
  va_end (argp);
  return result;
}

