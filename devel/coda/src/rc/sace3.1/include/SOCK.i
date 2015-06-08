/* Open up a socket.  Returns error status... */

/* Provides access to the ::setsockopt system call. */

inline int 
SOCK::set_option (int level, int option, void *optval, int optlen) const
{
  return ::setsockopt (this->get_handle (), level, option, (char *) optval, optlen);
}

/* Provides access to the ::getsockopt system call. */

inline int 
SOCK::get_option (int level, int option, void *optval, int *optlen) const
{
  /*Sergey: return ::getsockopt (this->get_handle (), level, option, (char *) optval, optlen);*/
  return ::getsockopt (this->get_handle (), level, option, (char *) optval, (socklen_t *)optlen);
}

inline 
SOCK::SOCK (void)
{
}

inline int
SOCK::get_local_addr (Addr &sa) const
{
  int len = sa.get_size ();

  /*Sergey: if (::getsockname (this->get_handle (), (sockaddr *) sa.get_addr (), &len) == IPC_SAP::INVALID_HANDLE)*/
  if (::getsockname (this->get_handle (), (sockaddr *) sa.get_addr (), (socklen_t *)&len) == IPC_SAP::INVALID_HANDLE)
    return IPC_SAP::INVALID_HANDLE;
  else
    {
      sa.set_size (len);
      return 0;
    }
}

