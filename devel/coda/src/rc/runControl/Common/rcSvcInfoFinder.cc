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
//      Implementation of rcSvcInfoFinder Class
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: rcSvcInfoFinder.cc,v $
//   Revision 1.2  1997/09/05 12:03:44  heyes
//   almost final
//
//   Revision 1.1.1.1  1996/10/11 13:39:31  chen
//   run control source
//
//

#include <codaConst.h>
#include <stdlib.h>
#include "rcSvcInfoFinder.h"
#include "libdb.h"

char* rcSvcInfoFinder::processTableName = (char *)"process";

int
rcSvcInfoFinder::findRcServer (const char* dbhost, const char* database,
                   const char* session,
			       char* &host, unsigned short& port)
{
  MYSQL *dbaseSock;
  MYSQL_RES *res = 0;
  MYSQL_ROW    row;		
  char qstring[256];

  printf("ser111: database=%d (%d)\n",database,dbhost);fflush(stdout);
  printf("ser111: database >%s<\n",database);fflush(stdout);
  dbaseSock = ::dbConnect(dbhost, database);
  printf("ser112: database=%d (%d)\n",database,dbhost);fflush(stdout);
  printf("ser112: database >%s<\n",database);fflush(stdout);
  if (dbaseSock == NULL) {
      fprintf (stderr, "Cannot connect to mysql server\n");
      return CODA_FATAL;
  }

  if (::mysql_select_db(dbaseSock, database) < 0) {
    fprintf (stderr, "Cannot select database %s\n", database);
    ::mysql_close(dbaseSock);
    return CODA_FATAL;
  }

  sprintf (qstring, "select * from %s where name = '%s'",
	   rcSvcInfoFinder::processTableName, session);

  if (::mysql_query (dbaseSock, qstring) != 0) {
    fprintf (stderr, "Get information for rcServer %s error: %s\n",
	     session, mysql_error(dbaseSock));
    ::mysql_close(dbaseSock);
    return CODA_ERROR;
  }

  res = mysql_store_result(dbaseSock);
  if (!res) {
    ::mysql_close(dbaseSock);
    return CODA_ERROR;
  }

  row = ::mysql_fetch_row(res);
  if (row) {
    host = new char[strlen (row[4]) + 1];
    ::strcpy (host, row[4]);
    int tport = atoi (row[5]);
    port = (unsigned short)tport;

    ::mysql_free_result (res);

    ::mysql_close(dbaseSock);
    return CODA_SUCCESS;
  }
  ::mysql_free_result (res);

  ::mysql_close(dbaseSock);
  return CODA_ERROR;
}

int
rcSvcInfoFinder::removeDeadRcServer (char* dbhost, 
				     char* database, 
				     char* session)
{
  MYSQL *dbaseSock;
  char qstring[256];

  dbaseSock = dbConnect(dbhost, database);
  printf("ser2: database >%s<\n",database);fflush(stdout);
  if (dbaseSock == NULL) {
      fprintf (stderr, "Cannot connect to mysql server\n");
      return CODA_FATAL;
  }

  if (::mysql_select_db(dbaseSock, database) < 0) {
    fprintf (stderr, "Cannot select database %s\n", database);
    return CODA_FATAL;
  }
  
  sprintf (qstring, "delete from %s where name = '%s'",
	   rcSvcInfoFinder::processTableName, session);
  if (::mysql_query (dbaseSock, qstring) != 0) {
    fprintf (stderr, "Delete dead rcServer %s info error: %s\n",
	     session, mysql_error(dbaseSock));
    return CODA_ERROR;
  }
  ::mysql_close(dbaseSock);
  return CODA_SUCCESS;
}

  
    
