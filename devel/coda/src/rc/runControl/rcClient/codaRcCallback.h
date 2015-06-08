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
//   $Log: codaRcCallback.h,v $
//   Revision 1.1.1.1  1996/10/11 13:39:32  chen
//   run control source
//
//
#ifndef _CODA_RC_CALLBACK_H
#define _CODA_RC_CALLBACK_H

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <daqNetData.h>

typedef void (*rcCallback)(int status, void* arg, daqNetData* data);

class codaRcCallback
{
public:
  codaRcCallback (void);
  codaRcCallback (rcCallback cbk, void* arg);
  codaRcCallback (const codaRcCallback& cbk);
  codaRcCallback& operator = (const codaRcCallback& cbk);
  ~codaRcCallback (void);

  int operator == (const codaRcCallback& cbk);
  int operator != (const codaRcCallback& cbk);

  virtual rcCallback callbackFunction (void) const;
  virtual void* userarg (void) const;

private:
  void* userarg_;
  rcCallback callback_;
};
#endif

