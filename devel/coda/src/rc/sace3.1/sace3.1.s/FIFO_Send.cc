/* -*- C++ -*- */

#include "FIFO_Send.h"
#include "Log_Msg.h"

FIFO_Send::FIFO_Send (void)
{
}

HANDLE
FIFO_Send::open (const char *rendezvous_name, int flags, int perms)
{
  return FIFO::open (rendezvous_name, flags | O_WRONLY, perms);
}

FIFO_Send::FIFO_Send (const char *fifo_name, int flags, int perms)
{
  if (this->FIFO_Send::open (fifo_name, flags, perms) == IPC_SAP::INVALID_HANDLE)
    LM_ERROR ((LOG_ERROR, "%p\n", "FIFO_Send::FIFO_Send"));
}
