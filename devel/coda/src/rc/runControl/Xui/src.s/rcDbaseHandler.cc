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
//      Implementation of RunControl Client Database Handler
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: rcDbaseHandler.cc,v $
//   Revision 1.4  2000/08/21 18:13:04  abbottd
//   Sun 5.0 C++ compiler fix
//
//   Revision 1.3  1999/11/29 21:26:31  rwm
//   Fix typo.
//
//   Revision 1.2  1999/10/26 19:23:54  rwm
//   Fixed misspelling of _TRACE_OBEJECTS.
//
//   Revision 1.1.1.1  1996/10/11 13:39:27  chen
//   run control source
//
//
#if defined (_CODA_2_0_T) || defined (_CODA_2_0)

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include <codaConst.h>
#include <rcDbaseHandler.h>

rcDbaseHandler* rcDbaseHandler::handler_ = 0;

rcDbaseHandler*
rcDbaseHandler::dbaseHandler (void)
{
  if (rcDbaseHandler::handler_ == 0)
    rcDbaseHandler::handler_ = new rcDbaseHandler ();
  return rcDbaseHandler::handler_;
}

rcDbaseHandler::rcDbaseHandler (void)
:dbaseSock_ (NULL), dbaseDir_ (0), numDbases_ (0),
 numSessions_ (0)
{
  int i;
#ifdef _TRACE_OBJECTS
  printf ("Create rcDbaseHandler Class Object\n");
#endif
  for (i = 0; i < RCXUI_MAX_SESSIONS; i++)
    sessionActive_[i] = 0;
}

rcDbaseHandler::~rcDbaseHandler (void)
{
  int i;
#ifdef _TRACE_OBJECTS
  printf ("Delete rcDbaseHandler Class Object\n");
#endif
  if (dbaseSock_ != NULL) 
    ::mysql_close (dbaseSock_);
  dbaseSock_ = NULL;

  if (dbaseDir_)
    delete []dbaseDir_;

  for (i = 0; i < numSessions_; i++)
    delete []sessions_;
  
  for (i = 0; i < numDbases_; i++)
    delete []dbases_;
}

int
rcDbaseHandler::connect (char* host)
{
  if (dbaseSock_ != NULL)
    ::mysql_close (dbaseSock_);
  
  dbaseSock_ = ::dbConnect (host, getenv("EXPID"));
  if (dbaseSock_ == NULL) {
    fprintf (stderr, "Cannot connect to mysql host at %s\n", host);
    ::exit (1);
    return CODA_ERROR;
  }
  return CODA_SUCCESS;
}

int
rcDbaseHandler::close (void)
{
  if (dbaseSock_ != NULL)
    ::mysql_close (dbaseSock_);
  dbaseSock_ = NULL;
  return CODA_SUCCESS;
}

int
rcDbaseHandler::connected (void) const
{
  if (dbaseSock_ == NULL)
    return 0;
  return 1;
}

int
rcDbaseHandler::listAllDatabases (void)
{
  int i;
  MYSQL_RES *res = 0;
  MYSQL_ROW    row;

  // first clean up the old result
  for (i = 0; i < numDbases_; i++)
    delete []dbases_[i];
  numDbases_ = 0;

  if (dbaseSock_ == NULL)
    return CODA_ERROR;

  res = mysql_list_dbs(dbaseSock_, NULL);
  if (!res)
    return CODA_ERROR;

  i = 0;
  while ((row = ::mysql_fetch_row (res))) {
    dbases_[i] = new char[::strlen (row[0]) + 1];
    ::strcpy (dbases_[i], row[0]);
    i++;
    if (i >= RCXUI_MAX_DATABASES)
      break;
  }
  numDbases_ = i;
  
  // free database query result
  ::mysql_free_result (res);

  return CODA_SUCCESS;
}

int
rcDbaseHandler::database (char* dbase)
{
  if (dbaseDir_)
    delete []dbaseDir_;
  dbaseDir_ = 0;

  if (::mysql_select_db (dbaseSock_, dbase) >= 0) {
    dbaseDir_ = new char[::strlen (dbase) + 1];
    ::strcpy (dbaseDir_, dbase);
  }
  else
    return CODA_ERROR;
  return CODA_SUCCESS;
}

char*
rcDbaseHandler::database (void) const
{
  return dbaseDir_;
}

char**
rcDbaseHandler::allDatabases (int& numDbases)
{
  numDbases = numDbases_;
  return dbases_;
}

char**
rcDbaseHandler::allSessions (int& numS)
{
  numS = numSessions_;
  return sessions_;
}

int*
rcDbaseHandler::sessionStatus (int& numS)
{
  numS = numSessions_;
  return sessionActive_;
}

int
rcDbaseHandler::listAllSessions (void)
{
  int i;
  char qstring[1024];
  MYSQL_RES *res = 0;
  MYSQL_ROW    row;

  // clean up old result
  for (i = 0; i < numSessions_; i++)
    delete[] sessions_[i];
  numSessions_ = 0;

  if (dbaseSock_ == NULL)
    return CODA_ERROR;

  ::sprintf (qstring, "select * from %s", DBASE_SESSION_TABLE);
  if (mysql_query (dbaseSock_, qstring) != 0) {
#ifdef _CODA_DEBUG
    printf ("list all sessions error: %s\n", mysql_error(dbaseSock_));
#endif
    return CODA_ERROR;
  }
  res = mysql_store_result(dbaseSock_);
  if (!res) {
#ifdef _CODA_DEBUG
    printf ("List all sessions error: %s\n", mysql_error(dbaseSock_));
#endif
    return CODA_ERROR;
  }
  i = 0;
  while ((row = mysql_fetch_row(res))) {
    sessions_[i] = new char[::strlen(row[0]) + 1];
    ::strcpy (sessions_[i], row[0]);
    if (::strcmp (row[3], "yes") == 0) {
      sessionActive_[i] = 1;
    }
    else 
      sessionActive_[i] = 0;
    i++;
    if (i >= RCXUI_MAX_SESSIONS)
      break;
  }
  numSessions_ = i;

  // free result
  ::mysql_free_result(res);

  return CODA_SUCCESS;
}
#endif
