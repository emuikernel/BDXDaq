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
//      CODA Server Message Reporter
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: rcMsgReporter.h,v $
//   Revision 1.3  1999/02/17 18:11:27  rwm
//   AUTOEND behaviour.
//
//   Revision 1.2  1996/10/14 20:02:51  heyes
//   changed message system
//
//   Revision 1.1.1.1  1996/10/11 13:39:19  chen
//   run control source
//
//
#ifndef _CODA_RCMSG_REPORTER_H
#define _CODA_RCMSG_REPORTER_H

#include <stdio.h>
#include <stdarg.h>
#include <daqRun.h>

#ifdef _CODA_USE_THREADS
#include <pthread.h>
#endif

#define CMSGLOG_SEVERE -1
#define CMSGLOG_ERROR  1 
#define CMSGLOG_WARN   2
#define CMSGLOG_INFO   3
#define CMSGLOG_INFO1  4
#define CMSGLOG_DEBUG1 5
#define CMSGLOG_DEBUG2 6
#define CMSGLOG_DEBUG3 7
#define CMSGLOG_DEBUG4 8
#define CMSGLOG_DEBUG5 9

class rcMsgReporterLocker;

class rcMsgReporter
{
public:
  // constructor
  rcMsgReporter   (daqRun& run);
  ~rcMsgReporter  (void);
  
  // reporting level
  void reportingLevel (int level);   
  int  reportingLevel (void) const;
  // user call this to report message
  void reportMessage (char *format, ...);
  void cmsglog (int msg_class, char *format, ...);

private:
  // reporting level
  int level_;
  daqRun& run_;
#ifdef _CODA_USE_THREADS
  pthread_mutex_t am_;
  void            lock (void);
  void            unlock (void);
#endif
  friend class rcMsgReporterLocker;
};

extern rcMsgReporter* reporter;

class rcMsgReporterLocker
{
public:
  rcMsgReporterLocker (rcMsgReporter* reporter);
  ~rcMsgReporterLocker (void);

private:
  rcMsgReporter* reporter_;

  // deny access to copy and assignment
  rcMsgReporterLocker (const rcMsgReporterLocker& );
  rcMsgReporterLocker& operator = (const rcMsgReporterLocker& );
};
#endif


  
