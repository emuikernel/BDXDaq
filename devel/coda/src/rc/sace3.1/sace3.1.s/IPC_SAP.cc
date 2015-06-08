/* Defines the member functions for the base class of the IPC_SAP
   abstraction. */ 

#include "IPC_SAP.h"

/* Cache for the process ID. */
pid_t IPC_SAP::pid_ = 0;

/* Make the HANDLE_ available for asynchronous I/O (SIGIO),
   urgent data (SIGURG), or non-blocking I/O (ACE_NONBLOCK). */

int
IPC_SAP::enable (int signum) const
{
  /* First-time in initialization. */
  if (IPC_SAP::pid_ == 0)
    IPC_SAP::pid_ = ::getpid ();

  switch (signum)
    {
#if defined (SIGURG)
    case SIGURG:
#if defined (ACE_HAS_SETOWN)
      if (::fcntl (this->handle_, F_SETOWN, IPC_SAP::pid_) < 0)
	return IPC_SAP::INVALID_HANDLE;
#else
      return IPC_SAP::INVALID_HANDLE;
#endif /* ACE_HAS_SETOWN */
      break;
#endif /* SIGURG */
    case SIGIO:
#if defined (ACE_HAS_SETOWN)
      if (::fcntl (this->handle_, F_SETOWN, IPC_SAP::pid_) == IPC_SAP::INVALID_HANDLE)
	return IPC_SAP::INVALID_HANDLE;
      if (ace_set_fl (this->handle_, FASYNC) == IPC_SAP::INVALID_HANDLE)
	return IPC_SAP::INVALID_HANDLE;
#else
      return IPC_SAP::INVALID_HANDLE;
#endif /* ACE_HAS_SETOWN */
      break;
    case ACE_NONBLOCK:
      if (ace_set_fl (this->handle_, ACE_NONBLOCK) == IPC_SAP::INVALID_HANDLE)
	return IPC_SAP::INVALID_HANDLE;
      break;
    default:
      return IPC_SAP::INVALID_HANDLE;
    }
  return 0;
}

/* Restore the IPC_SAPet by turning off synchronous I/O or urgent delivery. */

int
IPC_SAP::disable (int signum) const
{
  switch (signum)
    {
#if defined (SIGURG)
    case SIGURG:
#if defined (ACE_HAS_SETOWN)
      if (::fcntl (this->handle_, F_SETOWN, 0) == IPC_SAP::INVALID_HANDLE)
	return IPC_SAP::INVALID_HANDLE;
#else
      return IPC_SAP::INVALID_HANDLE;
#endif /* ACE_HAS_SETOWN */
      break;
#endif /* SIGURG */
    case SIGIO:
#if defined (ACE_HAS_SETOWN) 
      if (::fcntl (this->handle_, F_SETOWN, 0) == IPC_SAP::INVALID_HANDLE)
	return IPC_SAP::INVALID_HANDLE;
      if (ace_clr_fl (this->handle_, FASYNC) == IPC_SAP::INVALID_HANDLE)
	return IPC_SAP::INVALID_HANDLE;
#else
      return IPC_SAP::INVALID_HANDLE;
#endif /* ACE_HAS_SETOWN */
      break;
    case ACE_NONBLOCK:
      if (ace_clr_fl (this->handle_, ACE_NONBLOCK) == IPC_SAP::INVALID_HANDLE)
	return IPC_SAP::INVALID_HANDLE;
      break;
    default:
      return IPC_SAP::INVALID_HANDLE;
    }
  return 0;
}

