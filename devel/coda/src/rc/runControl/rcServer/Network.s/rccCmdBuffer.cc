//-----------------------------------------------------------------------------
// Copyright (c) 1991,1992 Southeastern Universities Research Association,
//                         Continuous Electron Beam Accelerator Facility
//
// This software was developed under a United States Government license
// described in the NOTICE file included as part of this distribution.
//
//-----------------------------------------------------------------------------
//
// Description:
//      CODA RunControl Server Side Command Queue
//
// Author:  Jie Chen
//
// Revision History:
//   $Log: rccCmdBuffer.cc,v $
//   Revision 1.1.1.1  1996/10/11 13:39:14  chen
//   run control source
//
//
#include "rccCmdBuffer.h"

rcCmdBufItem::rcCmdBufItem (rccIO* chan, rcMsg* msg)
:channel (chan), cmsg (msg)
{
#ifdef _TRACE_OBJECTS
  printf ("Create rcCmdBufItem Object\n");
#endif
  // empty
}

rcCmdBufItem::~rcCmdBufItem (void)
{
#ifdef _TRACE_OBJECTS
  printf ("Delete rcCmdBufItem Object\n");
#endif
  // empty, called will find cmsg useful
}

rccCmdBuffer::rccCmdBuffer (void)
:queue_()
{
#ifdef TRACE_OBJECTS
  printf ("Create rccCmdBuffer Class Object\n");
#endif
  //empty
}

rccCmdBuffer::~rccCmdBuffer (void)
{
#ifdef TRACE_OBJECTS
  printf ("Delete rccCmdBuffer Class Object\n");
#endif
  codaSlistIterator ite (queue_);
  rcCmdBufItem *item = 0;
  for (ite.init(); !ite; ++ite) {
    item = (rcCmdBufItem *)ite ();
    delete item->cmsg;
    delete item;
  }
}
  
void
rccCmdBuffer::insertCmd (rccIO *chan, rcMsg* msg)
{
  rcCmdBufItem *item = new rcCmdBufItem (chan, msg);

  queue_.addToEnd ((void *)item);
}

int
rccCmdBuffer::isEmpty (void) const
{
  return queue_.isEmpty ();
}

rcCmdBufItem*
rccCmdBuffer::firstCmd (void)
{
  if (queue_.isEmpty())
    return 0;
  else {
    rcCmdBufItem *item = (rcCmdBufItem *)queue_.firstElement ();
    return item;
  }
}

rcCmdBufItem*
rccCmdBuffer::removeCmd (void)
{
  if (queue_.isEmpty())
    return 0;
  else {
    rcCmdBufItem *item = (rcCmdBufItem *)queue_.firstElement ();
    queue_.removeFirst ();
    return item;
  }
}
