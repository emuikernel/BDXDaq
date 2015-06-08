/* -*- C++ -*- */
/* Defines the member functions for the base class of the IPC_SAP abstraction. */

/* This is the do-nothing constructor.  It does not
   perform a ::socket system call. */

inline
IPC_SAP::IPC_SAP (void): handle_ (IPC_SAP::INVALID_HANDLE)
{
}

/* Used to return the underlying handle_. */

inline HANDLE
IPC_SAP::get_handle (void) const
{
  return this->handle_;
}

/* Used to set the underlying handle_. */

inline void
IPC_SAP::set_handle (HANDLE handle)
{
  this->handle_ = handle;
}

/* Provides access to the ::ioctl system call. */

inline int 
IPC_SAP::control (int cmd, void *arg) const
{
  return ::ioctl (this->handle_, cmd, arg);
}
