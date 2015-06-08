/* -*- C++ -*- */

#include "FIFO.h"

#if !defined (__INLINE__)
#include "FIFO.i"
#endif 

FIFO::FIFO (const char *fifo_name, int flags, int perms)
{
  if (this->open (fifo_name, flags, perms) == IPC_SAP::INVALID_HANDLE) 
    LM_ERROR ((LOG_ERROR, "%p\n", "FIFO"));
}

