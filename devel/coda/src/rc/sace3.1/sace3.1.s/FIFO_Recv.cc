/* -*- C++ -*- */

#include "FIFO_Recv.h"
#include "Log_Msg.h"

int 
FIFO_Recv::close (void)
{
  int result = FIFO::close ();
  if (this->aux_handle_ != IPC_SAP::INVALID_HANDLE)
    return ::close (this->aux_handle_);
  else
    return result;
}

/* Note that persistent means "open fifo for writing, as well as reading."
   This ensures that the fifo never gets EOF, even if there aren't 
   any writers at the moment! */

HANDLE
FIFO_Recv::open (const char *fifo_name, int flags, int perms, int persistent)
{
  if (FIFO::open (fifo_name, ACE_NONBLOCK | flags, perms) == IPC_SAP::INVALID_HANDLE)
    return IPC_SAP::INVALID_HANDLE;
  else if (this->disable (ACE_NONBLOCK) == IPC_SAP::INVALID_HANDLE)
    return IPC_SAP::INVALID_HANDLE;
  else if (persistent && (this->aux_handle_ = 
			  ::open (fifo_name, O_WRONLY) == IPC_SAP::INVALID_HANDLE))
    return IPC_SAP::INVALID_HANDLE;
  else
    return this->get_handle ();
}

FIFO_Recv::FIFO_Recv (void): aux_handle_ (IPC_SAP::INVALID_HANDLE)
{
}

FIFO_Recv::FIFO_Recv (const char *fifo_name, int flags, int perms, int persistent)
{
  if (this->FIFO_Recv::open (fifo_name, flags, perms, persistent) == IPC_SAP::INVALID_HANDLE)
    LM_ERROR ((LOG_ERROR, "%p\n", "FIFO_Recv"));
}

