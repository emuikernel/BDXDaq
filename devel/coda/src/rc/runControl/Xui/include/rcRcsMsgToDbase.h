//-----------------------------------------------------------------------------
// Copyright (c) 1994,1995 Southeastern Universities Research Association,
//                         Continuous Electron Beam Accelerator Facility
//
// This software was developed under a United States Government license
// described in the NOTICE file included as part of this distribution.
//
// CEBAF Data Acquisition Group, 12000 Jefferson Ave., Newport News, VA 23606
//       coda@cebaf.gov  Tel: (804) 249-7030     Fax: (804) 249-5800
//-----------------------------------------------------------------------------
//
// Description:
//      RunControl Preference Server Message to Database
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: rcRcsMsgToDbase.h,v $
//   Revision 1.1  1996/10/31 15:58:09  chen
//   server message to database option
//
//
//
#ifndef _RC_RCS_MSG_TODBASE_H
#define _RC_RCS_MSG_TODBASE_H

#include <rcMenuTogComd.h>

class rcRcsMsgToDbase: public rcMenuTogComd
{
public:
  // constructor and destructor
  rcRcsMsgToDbase (char* name, int active,
		   char* acc, char* acc_text,
		   int state,
		   rcClientHandler& handler);
  ~rcRcsMsgToDbase (void);

  // class name
  virtual const char* className (void) const {return "rcRcsMsgToDbase";}

protected:
  // inherited operations
  virtual void doit    (void);
  virtual void undoit  (void);
  // set value callback
  static void rcsMsgToDbaseCbk0 (int status, void* arg, daqNetData* data);
  static void rcsMsgToDbaseCbk1 (int status, void* arg, daqNetData* data);
};
#endif




  
