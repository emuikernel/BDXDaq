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
//      RunControl Server all signal handler
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: rcSignalHandler.cc,v $
//   Revision 1.6  1997/08/26 12:56:00  heyes
//   remove tcl
//
//   Revision 1.5  1997/07/22 19:38:51  heyes
//   cleaned up lots of things
//
//   Revision 1.4  1996/12/04 18:32:41  chen
//   port to 1.4 on hp and ultrix
//
//   Revision 1.3  1996/10/22 17:17:16  chen
//   fix bugs of boot stage different state among components
//
//   Revision 1.2  1996/10/14 13:29:40  heyes
//   fix timeouts
//
//   Revision 1.1.1.1  1996/10/11 13:39:14  chen
//   run control source
//
//
#include <codaConst.h>
#include <sys/types.h>
#include <time.h>
#include "rcSignalHandler.h"

#define RC_DUMP_FILE ".rcerror"

int
rcSignalHandler::registerSignalHandlers (void)
{
  struct sigaction act, oact;

  act.sa_handler = &(rcSignalHandler::signalFunc);
  sigemptyset (&act.sa_mask);
  act.sa_flags = 0;
#ifdef SA_RESTART
  act.sa_flags |= SA_RESTART;
#endif
  /*
  if (sigaction (SIGINT, &act, &oact) < 0)
    return CODA_ERROR;

  if (sigaction (SIGQUIT, &act, &oact) < 0)
    return CODA_ERROR;

  if (sigaction (SIGILL, &act, &oact) < 0)
    return CODA_ERROR;

  if (sigaction (SIGFPE, &act, &oact) < 0)
    return CODA_ERROR;

  if (sigaction (SIGBUS, &act, &oact) < 0)
    return CODA_ERROR;

  if (sigaction (SIGSEGV, &act, &oact) < 0)
    return CODA_ERROR; 

  //  if (sigaction (SIGSYS, &act, &oact) < 0)
  //  return CODA_ERROR;
  
#ifndef _CODA_2_0_T
  // coda version 2 dpshell handles sigpipe signal already
  if (sigaction (SIGPIPE, &act, &oact) < 0)
    return CODA_ERROR;
#endif

  if (sigaction (SIGTERM, &act, &oact) < 0)
    return CODA_ERROR;

  if (sigaction (SIGTSTP, &act, &oact) < 0)
    return CODA_ERROR;

  if (sigaction (SIGCONT, &act, &oact) < 0)
    return CODA_ERROR;
    */
  return CODA_SUCCESS;
}

void
rcSignalHandler::signalFunc (int signo)
{
  // get system time
  time_t currt = ::time (0);
  char*  timestr = ::ctime (&currt);

#ifdef solaris
  char signame[SIG2STR_MAX];
  if (sig2str (signo, signame) == -1)
    sprintf (signame, "unknown");
#else
  char signame[128];
  sprintf (signame, "%d", signo);
#endif

  char fullpath[128];
  char* homedir = ::getenv ("HOME");
  if (homedir != 0) {
    ::strcpy (fullpath, homedir);
    ::strcat (fullpath, "/");
    ::strcat (fullpath, RC_DUMP_FILE);
  }
  else
    sprintf (fullpath, "/tmp/.rcerror");

  FILE *fd = fopen (fullpath, "a+");
  if (fd != 0) {
    fprintf (fd, "Interrupted by %s signal at %s \n", signame, timestr);
    fclose (fd);
  }
  else
    fprintf (stderr, "Interrupted by %s signal at %s \n", signame,
	     timestr);

  if (signo == SIGSEGV || signo == SIGBUS) {
    fprintf (stderr, "Fatal: Caught signal %s, Quit.......\n", signame);
    ::exit (1);
  }
  else if (signo != SIGPIPE && signo != SIGTSTP &&
	   signo != SIGCONT ) {
    fprintf (stderr, "Interrupted by %s signal at %s \n", signame,
	     timestr);
    finished = 1; // exit from runcontrol
  }
}


  
