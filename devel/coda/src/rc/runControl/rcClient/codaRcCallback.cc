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
//      CODA RunControl Server Client Callback
//
// Author:  Jie Chen
//
// Revision History:
//   $Log: codaRcCallback.cc,v $
//   Revision 1.1.1.1  1996/10/11 13:39:32  chen
//   run control source
//
//
#include "codaRcCallback.h"

//==========================================================================
//     Implementation of codaRcCallback
//==========================================================================
codaRcCallback::codaRcCallback (void)
:callback_ (0), userarg_ (0)
{
#ifdef _TRACE_OBJECTS
  printf ("Create codaRcCallback Class Object\n");
#endif
  // empty
}

codaRcCallback::codaRcCallback (rcCallback cbk, void* arg)
:callback_ (cbk), userarg_ (arg)
{
#ifdef _TRACE_OBJECTS
  printf ("Create codaRcCallback Class Object\n");
#endif
  // empty
}

codaRcCallback::codaRcCallback (const codaRcCallback& cbk)
:callback_ (cbk.callback_), userarg_ (cbk.userarg_)
{
#ifdef _TRACE_OBJECTS
  printf ("Create codaRcCallback Class Object\n");
#endif
  // empty
}

codaRcCallback&
codaRcCallback::operator = (const codaRcCallback& cbk)
{
  if (this != &cbk) {
    callback_ = cbk.callback_;
    userarg_ = cbk.userarg_;
  }
  return *this;
}

codaRcCallback::~codaRcCallback (void)
{
#ifdef _TRACE_OBJECTS
  printf ("Delete codaRcCallback Class Object\n");
#endif
  // empty
}

rcCallback
codaRcCallback::callbackFunction (void) const
{
  return callback_;
}

void*
codaRcCallback::userarg (void) const
{
  return userarg_;
}

int
codaRcCallback::operator == (const codaRcCallback& cbk)
{
  if (callback_ == cbk.callback_ &&
      userarg_ == cbk.userarg_)
    return 1;
  else
    return 0;
}

int
codaRcCallback::operator != (const codaRcCallback& cbk)
{
  return ! operator == (cbk);
}

