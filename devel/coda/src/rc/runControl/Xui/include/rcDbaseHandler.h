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
//      RunControl Client Side Database Handler 
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: rcDbaseHandler.h,v $
//   Revision 1.1.1.1  1996/10/11 13:39:28  chen
//   run control source
//
//
#ifndef _RC_DBASE_HANDLER_H
#define _RC_DBASE_HANDLER_H

#if defined (_CODA_2_0_T) || defined (_CODA_2_0)
#include <rcXuiConsts.h>
#include "libdb.h"

#define DBASE_SESSION_TABLE   "sessions"
#define DBASE_PROCESS_TABLE   "process"
#define DBASE_RUNTYPE_TABLE   "runTypes"
#define DBASE_PRIORITY_TABLE  "priority"
#define DBASE_POSITION_TABLE  "_pos"
#define DBASE_OPTION_TABLE    "_option"
#define DBASE_SCRIPT_TABLE    "_script"

class rcDbaseHandler
{
public:
  // use this instead of constructor
  static rcDbaseHandler* dbaseHandler (void);

  ~rcDbaseHandler   (void);

  // operations
  int   connect     (char* host);
  int   close       (void);
  int   connected   (void) const;

  // select a database
  int   database    (char* dbase);
  char* database    (void) const;

  // get all database
  int    listAllDatabases (void);

  // get all sessions
  int    listAllSessions  (void);

  // get all databases
  char** allDatabases     (int& numdbases);
  // get all sessions 
  char** allSessions      (int& numSessions);
  int*   sessionStatus    (int& numSessions);

protected:
  // constructor and destructor
  rcDbaseHandler    (void);

private:
  MYSQL* dbaseSock_;
  char*  dbaseDir_;
  char*  dbases_[RCXUI_MAX_DATABASES];
  int    numDbases_;
  char*  sessions_[RCXUI_MAX_SESSIONS];
  int    sessionActive_[RCXUI_MAX_SESSIONS];
  int    numSessions_;

  // static instannce: only copy
  static rcDbaseHandler* handler_;
};
#endif

#endif



