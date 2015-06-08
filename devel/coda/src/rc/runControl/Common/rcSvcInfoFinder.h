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
//      RunControl Server Information Finder (CODA 2.0 Only)
//      Tightly coupled with mysql database
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: rcSvcInfoFinder.h,v $
//   Revision 1.1.1.1  1996/10/11 13:39:31  chen
//   run control source
//
//
#ifndef _RC_SVC_INFO_FINDER_H
#define _RC_SVC_INFO_FINDER_H

#if defined (_CODA_2_0) || defined (_CODA_2_0_T)

// CODADB and EXPID environment variables may be needed
// mysql host is not needed to specify
// MYSQL_HOST env will set the host

#include <stdio.h>
#include <string.h>
#include <assert.h>

class rcSvcInfoFinder
{
public:
  // go out to find out the server information
  // return CODA_SUCCESS if found.
  // return CODA_FATAL if database error
  // return CODA_ERROR not found
  // callers have to free host
  static int findRcServer (const char* dbhost, const char* database,
               const char* session,
			   char* &host, unsigned short& port);

  // remove some old rcServer information due to crashed rcServer
  static int removeDeadRcServer (char* dbhost, char* database, char* session);

private:
  static char* processTableName;
};

#endif

#endif
