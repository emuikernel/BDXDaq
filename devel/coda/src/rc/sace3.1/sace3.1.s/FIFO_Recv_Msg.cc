/* -*- C++ -*- */

#include "FIFO_Recv_Msg.h"
#include "Log_Msg.h"

/* Note that persistent means "open fifo for writing, as well as reading."
   This ensures that the fifo never gets EOF, even if there aren't 
   any writers at the moment! */

int 
FIFO_Recv_Msg::open (const char *fifo_name, int flags, int perms, int persistent)
{
  return FIFO_Recv::open (fifo_name, flags, perms, persistent);
}

FIFO_Recv_Msg::FIFO_Recv_Msg (void)
{
}

FIFO_Recv_Msg::FIFO_Recv_Msg (const char *fifo_name, int flags, int perms, int persistent)
{
  if (this->FIFO_Recv_Msg::open (fifo_name, flags, perms, 
				 persistent) == IPC_SAP::INVALID_HANDLE)
    LM_ERROR ((LOG_ERROR, "%p\n", "FIFO_Recv_Msg"));
}
