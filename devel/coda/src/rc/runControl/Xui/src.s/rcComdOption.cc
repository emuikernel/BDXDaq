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
//      Implementation of runControl Command Line Options
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: rcComdOption.cc,v $
//   Revision 1.6  1998/09/01 18:48:37  heyes
//   satisfy Randy's lust for command line options
//
//   Revision 1.5  1998/06/18 12:20:33  heyes
//   new GUI ready I think
//
//   Revision 1.4  1997/06/06 18:51:22  heyes
//   new RC
//
//   Revision 1.3  1996/12/04 18:32:26  chen
//   port to 1.4 on hp and ultrix
//
//   Revision 1.2  1996/10/14 20:13:36  chen
//   add display server messages preference
//
//   Revision 1.1.1.1  1996/10/11 13:39:27  chen
//   run control source
//
//
#include <stdlib.h>
#include "rcComdOption.h"

extern "C" int gethostname (char *, int);

rcComdOption* rcComdOption::option_ = 0;
void
rcComdOption::initOptionParser (int argc, char** argv)
{
  if (rcComdOption::option_)
    delete rcComdOption::option_;

  rcComdOption::option_ = new rcComdOption (argc, argv);
}

rcComdOption*
rcComdOption::option (void)
{
  assert (rcComdOption::option_);
  return rcComdOption::option_;
}
    

#if defined (_CODA_2_0_T) || defined (_CODA_2_0)
rcComdOption::rcComdOption (int argc, char **argv)
:animation_ (0), rcServerHost_ (0), msqld_ (0), dbasename_ (0),
 codadb_ (0), session_ (0), audio_ (0), reportMsg_ (0), 
 startCedit_ (0), startDbedit_ (0), autostart_ (0),
 startWide_ (0), noEdit_(0), argv_ (argv), argc_ (argc)
{
#ifdef _TRACE_OBJECTS
  printf ("Create rcComdOption Class Object\n");
#endif
}

rcComdOption::~rcComdOption (void)
{
#ifdef _TRACE_OBJECTS
  printf ("Delete rcComdOption Class Object\n");
#endif
  if (rcServerHost_)
    delete []rcServerHost_;
  if (msqld_)
    delete []msqld_;
  if (session_)
    delete []session_;
  if (dbasename_)
    delete []dbasename_;
  if (codadb_)
    delete []codadb_;
}

void
rcComdOption::parseOptions (void)
{
  int i = 1;

  while (i < argc_) {
    if (argv_[i][0] == '-') {
      if (::strcmp (argv_[i], "-a") == 0 || 
	  ::strcmp (argv_[i], "-animate") == 0)
	animation_ = 1;
      else if (::strcmp (argv_[i], "-h") == 0 ||
	       ::strcmp (argv_[i], "-help") == 0 ||
	       ::strcmp (argv_[i], "--help") == 0) {
	usage (argv_[0]);
	::exit (1);
      }
      else if (::strcmp (argv_[i], "-r") == 0 ||
	  ::strcmp (argv_[i], "-rcServer") == 0) {
	i++;
	if (i >= argc_ || argv_[i][0] == '-') {
	  usage (argv_[0]) ;
	  ::exit (1);
	}
	rcServerHost_ = new char[::strlen (argv_[i]) + 1];
	::strcpy (rcServerHost_, argv_[i]);
      }
      else if (::strcmp (argv_[i], "-m") == 0 ||
	  ::strcmp (argv_[i], "-msqld") == 0) {
	i++;
	if (i >= argc_ || argv_[i][0] == '-') {
	  usage (argv_[0]) ;
	  ::exit (1);
	}
	msqld_ = new char[::strlen (argv_[i]) + 1];
	::strcpy (msqld_, argv_[i]);
      }
      else if (::strcmp (argv_[i], "-s") == 0 ||
	  ::strcmp (argv_[i], "-session") == 0) {
	i++;
	if (i >= argc_ || argv_[i][0] == '-') {
	  usage (argv_[0]) ;
	  ::exit (1);
	}
	session_ = new char[::strlen (argv_[i]) + 1];
	::strcpy (session_, argv_[i]);
      }
      else if (::strcmp (argv_[i], "-v") == 0 || 
	       ::strcmp (argv_[i], "-verbose") == 0) 
	audio_ = 1;
      else if (::strcmp (argv_[i], "-o") == 0 ||
	       ::strcmp (argv_[i], "-output") == 0)
	reportMsg_ = 1;
      else if (::strcmp (argv_[i], "-c") == 0 ||
	       ::strcmp (argv_[i], "-cedit") == 0)
	startCedit_ = 1;
      else if (::strcmp (argv_[i], "-d") == 0 ||
	       ::strcmp (argv_[i], "-dbedit") == 0)
	startDbedit_ = 1;
      else if (::strcmp (argv_[i], "-A") == 0 ||
	       ::strcmp (argv_[i], "-autostart") == 0)
	autostart_ = 1;
      else if (::strcmp (argv_[i], "-w") == 0 ||
	       ::strcmp (argv_[i], "-wide") == 0)
	startWide_ = 1;
      else if (::strcmp (argv_[i], "-n") == 0 ||
	       ::strcmp (argv_[i], "-noedit") == 0)
	noEdit_ = 1;
      else {
	usage (argv_[0]);
      }
    }
    else {
      usage (argv_[0]);
    }
    i++;
  }
  // check all options, if they are not set, use default values
  if (!rcServerHost_) {
    char            serverHost[64];
    int             len = sizeof (serverHost);
    ::gethostname (serverHost, len);
    rcServerHost_ = new char[::strlen (serverHost) + 1];
    ::strcpy (rcServerHost_, serverHost);
  }
  
  if (!session_) {
    char* mtmp = ::getenv ("SESSION");
    if (mtmp) {
      session_ = new char[::strlen (mtmp) + 1];
      ::strcpy (session_, mtmp);
    }
  }
  
  if (!dbasename_) {
    char* mtmp = ::getenv ("EXPID");
    if (mtmp) {
      dbasename_ = new char[::strlen (mtmp) + 1];
      ::strcpy (dbasename_, mtmp);
    }
  }
  
  if (!msqld_) {
    char* mtmp = ::getenv ("MYSQL_HOST");
    if (!mtmp) {
      char            serverHost[64];
      int             len = sizeof (serverHost);
      ::gethostname (serverHost, len);
      msqld_ = new char[::strlen (serverHost) + 1];
      ::strcpy (msqld_, serverHost);
    }
    else {
      msqld_ = new char[::strlen (mtmp) + 1];
      ::strcpy (msqld_, mtmp);
    }
  }

}

char*
rcComdOption::serverHost (void) const
{
  return rcServerHost_;
}

void
rcComdOption::serverHost (char* host)
{
  if (rcServerHost_)
    delete []rcServerHost_;
  rcServerHost_ = new char[::strlen (host) + 1];
  ::strcpy (rcServerHost_, host);
}

char*
rcComdOption::msqldhost (void) const
{
  return msqld_;
}

void
rcComdOption::msqldhost (char* host)
{
  if (msqld_)
    delete []msqld_;
  msqld_ = new char[::strlen (host) + 1];
  ::strcpy (msqld_, host);
}

char*
rcComdOption::session (void) const
{
  return session_;
}

void
rcComdOption::session (char* ses)
{
  if (session_)
    delete []session_;
  session_ = new char[::strlen (ses) + 1];
  ::strcpy (session_, ses);
}

char*
rcComdOption::dbasename (void) const
{
  return dbasename_;
}

void
rcComdOption::dbasename (char* name)
{
  if (dbasename_)
    delete []dbasename_;
  dbasename_ = new char[::strlen (name) + 1];
  ::strcpy (dbasename_, name);
}

char*
rcComdOption::codadb (void) const
{
  return codadb_;
}

void
rcComdOption::codadb (char* dir)
{
  if (codadb_)
    delete []codadb_;
  codadb_ = 0;

  if (dir) {
    codadb_ = new char[::strlen (dir) + 1];
    ::strcpy (codadb_, dir);
  }
}

int
rcComdOption::audio (void) const
{
  return audio_;
}

void
rcComdOption::audioOn (void)
{
  audio_ = 1;
}

void
rcComdOption::audioOff (void)
{
  audio_ = 0;
}

void
rcComdOption::usage (char* progname)
{
  fprintf (stderr, "Command Options are:\n"
	   "-h, -help --help:      display this text\n"
	   "-a, -animate    :      animate graphics                      \n"
	   "-r, -rcServer <host>:  start server on host \"host\"         \n"
	   "-m, -msqld <host>:     use database daemon on \"host\"       \n"
	   "-s, -session <name>:   use \"name\" as the session name      \n"
	   "-v, -verbose    :      verbose error reporting               \n"
	   "-o, -output     :      local message output                  \n"
	   "-c, -cedit      :      start coda configuration editor       \n"
	   "-d, -dbedit     :      start database editor                 \n"
	   "-A, -autostart  :      start UNIX components when required   \n"
	   "-w, -wide       :      start in full screen mode             \n"
	   "-n, -noedit     :      disable editing in cedit              \n"
	   );
}

#else
rcComdOption::rcComdOption (int argc, char **argv)
:animation_ (0), reportMsg_ (0), argv_ (argv), argc_ (argc)
{
#ifdef _TRACE_OBJECTS
  printf ("Create rcComdOption Class Object\n");
#endif
}

rcComdOption::~rcComdOption (void)
{
#ifdef _TRACE_OBJECTS
  printf ("Delete rcComdOption Class Object\n");
#endif
}

void
rcComdOption::parseOptions (void)
{
  int i = 1;

  while (i < argc_) {
    if (argv_[i][0] == '-') {
      if (::strcmp (argv_[i], "-a") == 0 || 
	  ::strcmp (argv_[i], "-animate") == 0)
	animation_ = 1;
      else if (::strcmp (argv_[i], "-o") == 0 ||
	       ::strcmp (argv_[i], "-output") == 0)
	reportMsg_ = 1;
      else if (::strcmp (argv_[i], "-h") == 0 ||
	       ::strcmp (argv_[i], "-help") == 0) {
	usage (argv_[0]);
	exit (0);
      }
      else {
	usage (argv_[0]);
      }
    }
    else {
      usage (argv_[0]);
    }
    i++;
  }
}

void
rcComdOption::usage (char* progname)
{
  fprintf (stderr, "Usage: %s [-a(nimate)] [-h(elp)] [-o(utputMessage)]\n",
	   progname);
}
#endif



int
rcComdOption::showAnimation (void) const
{
  return animation_;
}

int
rcComdOption::reportMsg (void) const
{
  return reportMsg_;
}

void
rcComdOption::reportMsgOn (void)
{
  reportMsg_ = 1;
}

void
rcComdOption::reportMsgOff (void)
{
  reportMsg_ = 0;
}




	  
  
