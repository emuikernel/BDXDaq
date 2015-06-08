/* -*- C++ -*- */
/* Abstract base class for UNIX FIFOs (a.k.a. "named-pipes"). */

#include "Log_Msg.h"

INLINE HANDLE
FIFO::open (const char *r, int flags, int perms)
{
  ::strncpy (this->rendezvous_, r, MAXPATHLEN);

  if ((flags & O_CREAT) != 0 
      && ::mkfifo (this->rendezvous_, perms) == -1 
      && !(errno == EEXIST))
    return IPC_SAP::INVALID_HANDLE;

  this->set_handle (::open (this->rendezvous_, flags));
  return this->get_handle ();
}

INLINE int 
FIFO::get_local_addr (const char *&r) const
{
  r = this->rendezvous_;
  return 0;
}

INLINE
FIFO::FIFO (void)
{
}

INLINE int
FIFO::close (void)
{
  return ::close (this->get_handle ());
}

INLINE int
FIFO::remove (void)
{
  int result = this->close ();
  return ::unlink (this->rendezvous_) == -1 || result == -1 ? -1 : 0;
}

