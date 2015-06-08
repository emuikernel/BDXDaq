/* Miscellaneous functions for the SOCK abstraction */

#include "sysincludes.h"

/* Bind socket to an unused port. */

int
ace_bind_port (int fd)
{
  sockaddr_in sin;
  const int   MAX_SHORT   = 65535;
  static int  upper_limit = MAX_SHORT;
  int	      len         = sizeof sin;
  int	      lower_limit = IPPORT_RESERVED;
  int	      round_trip  = upper_limit;

  ::memset ((void *) &sin, 0, sizeof sin);
  sin.sin_family      = AF_INET;
  sin.sin_addr.s_addr = INADDR_ANY;

  for (;;)
    {
      sin.sin_port = htons (upper_limit);

      if (::bind (fd, (sockaddr *) &sin, sizeof sin) >= 0)
	return 0;
      if (errno != EADDRINUSE)
	return -1;
      else
	{
	  upper_limit--;
		  
	  /* Wrap back around when we reach the bottom. */
	  if (upper_limit <= lower_limit)
	    upper_limit = MAX_SHORT;
		  
	  /* See if we have already gone around once! */
	  if (upper_limit == round_trip)
	    {
	      errno = EAGAIN;	
	      return -1;
	    }
	}
    }
}

