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
//      RunControl Command Line Option Class
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: rcComdOption.h,v $
//   Revision 1.3  1998/09/01 18:48:38  heyes
//   satisfy Randy's lust for command line options
//
//   Revision 1.2  1996/10/14 20:13:37  chen
//   add display server messages preference
//
//   Revision 1.1.1.1  1996/10/11 13:39:27  chen
//   run control source
//
//
#ifndef _RC_COMD_OPTION_H
#define _RC_COMD_OPTION_H

#include <stdio.h>
#include <string.h>
#include <assert.h>

class rcComdOption
{
public:
  // use this instead of constructor
  static void             initOptionParser (int argc, char** argv);
  static rcComdOption*    option (void);

  // destructor
  ~rcComdOption (void);

  void parseOptions (void);

  // return all options
  int showAnimation (void) const;

#if defined (_CODA_2_0_T) || defined (_CODA_2_0)
  char* serverHost (void) const;
  void  serverHost (char* host);
  char* session    (void) const;
  void  session    (char* ses);
  char* msqldhost  (void) const;
  void  msqldhost  (char* host);
  char* dbasename  (void) const;
  void  dbasename  (char* name);
  char* codadb     (void) const;
  void  codadb     (char* db);

  int   audio      (void) const;
  void  audioOn    (void);
  void  audioOff   (void);
#endif

  // report message or not
  int   reportMsg    (void) const;
  void  reportMsgOn  (void);
  void  reportMsgOff (void);

  int   startCedit_;
  int   startDbedit_;
  int   autostart_;
  int   startWide_;
  int   noEdit_;
  char* defaultConfig_;

protected:
  // constructor and destructor
  rcComdOption  (int argc, char** argv);

  // option for animation
  int animation_;
#if defined (_CODA_2_0_T) || defined (_CODA_2_0)
  // option for rcServer host
  char*  rcServerHost_;
  // option for msqld host
  char*  msqld_;
  // database name from envionment variable
  char*  dbasename_;
  // local database directory
  char*  codadb_;
  // option for session name
  char* session_;

  // option for audio output
  int   audio_;
#endif
  // option for reporting message
  int   reportMsg_;

  void  usage (char* progname);
  
private:
  char** argv_;
  int    argc_;

  // singlton pattern: allow only one
  static rcComdOption* option_;
};

#endif

