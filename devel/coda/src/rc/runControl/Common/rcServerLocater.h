//-----------------------------------------------------------------------------
// Copyright (c) 1994,1995 Southeastern Universities Research Association,
//                         Continuous Electron Beam Accelerator Facility
//
// This software was developed under a United States Government license
// described in the NOTICE file included as part of this distribution.
//
// CEBAF Data Acquisition Group, 12000 Jefferson Ave., Newport News, VA 23606
// Email: coda@cebaf.gov  Tel: (804) 249-7101  Fax: (804) 249-7363
//-----------------------------------------------------------------------------
//
// Description:
//       CODA RunControl Service Service locater
//
// Author:  Jie Chen
//       CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: rcServerLocater.h,v $
//   Revision 1.1.1.1  1996/10/11 13:39:31  chen
//   run control source
//
//
#ifndef _CODA_RC_SVC_LOCATER
#define _CODA_RC_SVC_LOCATER

#include <rcSvcProtocol.h>

#include "rcSvcLocaterRep.h"

// Some environment variables may be used to determine host, name and ID
#define CODA_RCHOST_ENV "CODA_HOSTNAME"
#define CODA_RCNAME_ENV "CODA_EXPTNAME"
#define CODA_RCID_ENV   "EXPID"

typedef int (*rcUpdateFunc)(void *);

class rcServerLocater
{
public:
  // constructor and destructor
  // caller provide request protocol, experiment name and number of retries
  rcServerLocater  (int req, char *hostname, 
		    char *exptname, 
		    int exptid,
		    int numRetries);

  rcServerLocater  (int req, char* hostname, char* exptname,
		    unsigned short port,
		    int      numRetries);

  ~rcServerLocater (void);
  // return master server host name and requested port number
  // called provide buffer and buffer size for hostName
  // return 0 for success
  int locatingServer (unsigned short& port);

  // register external updating function
  void registerUpdateFunc (rcUpdateFunc func, void* arg);

private:
  unsigned short brdPort_;
  int            numRetries_;
  int           req_;
  char           exptname_[MAX_STRING_LEN];
  char           serverHost_[MAX_HOSTNAME_LEN];
  // update external function
  rcUpdateFunc   ufunc_;
  void*          uarg_;
};

#endif
