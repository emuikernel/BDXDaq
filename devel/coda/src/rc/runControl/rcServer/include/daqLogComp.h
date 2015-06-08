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
//      CODA 1.4 User Message log component
//
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: daqLogComp.h,v $
//   Revision 1.1  1997/01/24 16:36:51  chen
//   add log component for 1.4
//
//
//
#ifndef _CODA_DAQ_LOG_COMP_H
#define _CODA_DAQ_LOG_COMP_H

#if !defined (_CODA_2_0_T) && !defined (_CODA_2_0)

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>

#include <daqSubSystem.h>
#include <netComponent.h>

class daqLogComp: public netComponent
{
public:
  // constructor and destructor
  daqLogComp  (char* title, int number, int expid, char* type, 
	       char* node, char* bootString, daqSubSystem& subsys);

  virtual ~daqLogComp (void);

  // inherited operatiions
  virtual int  boot      (void);
  virtual int  terminate (void);
  virtual int  status    (void) const;

  virtual int  configure (void);
  virtual int  download  (void);
  virtual int  go        (void);
  virtual int  pause     (void);
  virtual int  prestart  (void);
  virtual int  end       (void);
  virtual int  verify    (void);
  virtual int  reset     (void);

  virtual int  state     (void);
  virtual int  state2    (void);

  // wait for net connection
  virtual int  waitForConnection (void);

  // timer function
  virtual void timerCallback     (void);

  // return daqSubsystem reference
  daqSubSystem& subSystem (void) const;

  virtual const char* className (void) const {return "daqLogComp";}

private:
  static char* zapmessage;
  static char* resetmessage;
  static char* initmessage;

  // connection to a msg logger (only one is allowed)
  static int   msglogger;
  // deny access to copy and assignment operations
  daqLogComp (const daqLogComp&);
  daqLogComp& operator = (const daqLogComp& );
};

#endif

#endif
