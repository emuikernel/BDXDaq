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
//      Message Reporter Class Implementation
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: rcMsgReporter.cc,v $
//   Revision 1.13  1999/02/17 18:11:27  rwm
//   AUTOEND behaviour.
//
//   Revision 1.12  1999/02/04 16:24:21  rwm
//   Some reformatting and debug_printf.
//
//   Revision 1.11  1999/01/28 14:24:57  heyes
//   add dalogmsg again
//
//   Revision 1.10  1998/06/02 19:51:47  heyes
//   fixed rcServer
//
//   Revision 1.9  1997/07/22 19:39:05  heyes
//   cleaned up lots of things
//
//   Revision 1.8  1997/06/17 19:29:15  heyes
//   for larry
//
//   Revision 1.7  1997/05/09 14:03:17  heyes
//   add cmlog to rcServer
//
//   Revision 1.6  1996/12/04 18:32:47  chen
//   port to 1.4 on hp and ultrix
//
//   Revision 1.5  1996/10/31 15:56:11  chen
//   Fixing boot stage bug + reorganize code
//
//   Revision 1.4  1996/10/28 14:23:05  heyes
//   communication with ROCs changed
//
//   Revision 1.3  1996/10/22 17:17:19  chen
//   fix bugs of boot stage different state among components
//
//   Revision 1.2  1996/10/14 20:02:50  heyes
//   changed message system
//
//   Revision 1.1.1.1  1996/10/11 13:39:19  chen
//   run control source
//
//
#include "rcMsgReporter.h"
#include <rcSvcProtocol.h>

#ifdef USE_TK
#include "rcTclInterface.h"
#include <dplite.h>
#endif

#define _CODA_REPORT_MSG_LOCAL

#undef WRITE_LOG_FILE /*sergey */

rcMsgReporter::rcMsgReporter (daqRun& run)
:level_ (REPORTING_NORMAL), run_ (run)
{
#ifdef _TRACE_OBJECTS
  printf ("Create msgReporter Class Object\n");
#endif
  reporter = this;
  
#ifdef _CODA_USE_THREADS
  pthread_mutex_init (&am_, 0);
#endif

#ifdef WRITE_LOG_FILE
  fd_log = fopen("$CLON_LOG/run_log/rcServer_<expid>.log","a+");
#endif
}

rcMsgReporter::~rcMsgReporter (void)
{
#ifdef _TRACE_OBJECTS
  printf ("Delete msgReporter Class Object\n");
#endif
  reporter = 0;

#ifdef _CODA_USE_THREADS
  pthread_mutex_destroy (&am_);
#endif
}

void
rcMsgReporter::reportingLevel (int level)
{
  rcMsgReporterLocker locker (this);

  if (level < REPORTING_NONE)
    level_ = REPORTING_NONE;
  else if (level > REPORTING_TERSE)
    level_ = REPORTING_TERSE;
  else
    level_ = level;
}

int
rcMsgReporter::reportingLevel (void) const
{
  return level_;
}

void
rcMsgReporter::reportMessage (char *format, ...)
{
  va_list argp;
  
  if (level_ == REPORTING_NONE) return;
  
  // writes to daqrun string buffer
  char buffer[1024];
  va_start (argp, format);
  int status = vsprintf (buffer,format,argp);
  va_end (argp);

  rcMsgReporterLocker locker (this);  // lock run_ object
  run_.updateRunMessage (buffer);
#ifdef _CODA_REPORT_MSG_LOCAL
  printf ("%s\n",buffer);
#endif
  
}

void
rcMsgReporter::cmsglog (int msg_class,char *format, ...)
{
  va_list argp;
  char tag[20];

#ifdef _CODA_REPORT_MSG_LOCAL
  //printf (MSG_DBG, "Top of cmsglog.");
  //printf (MSG_DBG, "msg_class = %d.\n", msg_class);
  //printf (MSG_DBG, "level_ = %d.\n", level_);
#endif

  if (msg_class > 3) {
    return;
  }

  if (level_ == REPORTING_NONE) {
    return;
  }

  // writes to daqrun string buffer
  char buffer[1024];
  va_start (argp, format);

  switch (msg_class) {
  case CMSGLOG_SEVERE:
    strcpy(tag,"SEVERE");
    break;
  case CMSGLOG_ERROR:
    strcpy(tag,"ERROR");
    break;
  case CMSGLOG_WARN:
    strcpy(tag,"WARN");
    break;
  case CMSGLOG_INFO:
    strcpy(tag,"INFO");
    break;
  case CMSGLOG_INFO1:
    strcpy(tag,"INFO1");
    break;
  case CMSGLOG_DEBUG1:
    strcpy(tag,"DEBUG1");
    break;
  case CMSGLOG_DEBUG2:
    strcpy(tag,"DEBUG2");
    break;
  } 
  
  vsprintf (buffer,format,argp);
  va_end (argp);

  {
    char temp[1000];
    sprintf(temp,"%-7s: %s",tag,buffer);
    rcMsgReporterLocker locker (this);  // lock run_ object
    run_.updateRunMessage (temp);
  }

  if (buffer[strlen(buffer) - 1] == '\n')
    buffer[strlen(buffer) - 1] = 0;

#if defined (_CODA_2_0_T) || defined (_CODA_2_0)
  if (run_.rcsMsgToDbase ()) {
    if (msg_class < CMSGLOG_INFO1) {

#ifdef _CODA_REPORT_MSG_LOCAL
      //printf(MSG_DBG, "XXXXXXXXXXXXXXXXX calling daLogMsg with: \n");
      //printf(MSG_DBG, "  tag = %s, buffer = %s\n", tag, buffer);
#endif
      //daLogMsg(tag,buffer);
    }
  }
#endif

#ifdef _CODA_REPORT_MSG_LOCAL
  //printf (MSG_DBG,"\"%s\"\n",buffer);
#endif
}

#ifdef _CODA_USE_THREADS
void
rcMsgReporter::lock (void)
{
  pthread_mutex_lock (&am_);
}

void
rcMsgReporter::unlock (void)
{
  pthread_mutex_unlock (&am_);
}

rcMsgReporterLocker::rcMsgReporterLocker (rcMsgReporter* reporter)
:reporter_ (reporter)
{
  reporter_->lock ();
}

rcMsgReporterLocker::~rcMsgReporterLocker (void)
{
  reporter_->unlock ();
}
#else
rcMsgReporterLocker::rcMsgReporterLocker (rcMsgReporter* reporter)
:reporter_ (reporter)
{
  //empty
}

rcMsgReporterLocker::~rcMsgReporterLocker (void)
{
  // empty
}
#endif



    
