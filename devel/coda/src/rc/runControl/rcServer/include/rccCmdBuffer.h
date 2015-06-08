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
//   $Log: rccCmdBuffer.h,v $
//   Revision 1.1.1.1  1996/10/11 13:39:14  chen
//   run control source
//
//
#ifndef _CODA_RCC_CMD_BUF_H
#define _CODA_RCC_CMD_BUF_H

#include <stdio.h>
#include <string.h>
#include <assert.h>

#include <codaSlist.h>
#include <rccIO.h>
#include <rcMsg.h>

class rcCmdBufItem
{
public:
  rcCmdBufItem  (rccIO* chan, rcMsg* msg);
  ~rcCmdBufItem (void);

  rccIO*      channel;
  rcMsg*      cmsg;
};

class rccCmdBuffer
{
public:
  // constructor and destructor
  rccCmdBuffer  (void);
  ~rccCmdBuffer (void);

  // add command and channel information to the end of the list
  void insertCmd (rccIO* chan, rcMsg* msg);
  // access first command
  rcCmdBufItem* firstCmd (void);
  // remove command and channel information from the beginning of the list
  rcCmdBufItem* removeCmd (void);
  // check whether is empty
  int isEmpty (void) const;

private:
  // list holds all items of rcCmdBufItem
  codaDoubleEndedSlist queue_;
};
#endif
