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
//      RunControl Critical Information Panel
//      containing experiment name run type and hostname
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: rcRunCInfoPanel.h,v $
//   Revision 1.1.1.1  1996/10/11 13:39:25  chen
//   run control source
//
//
#ifndef _RC_RUN_CINFO_PANEL_H
#define _RC_RUN_CINFO_PANEL_H

#include <stdio.h>
#include <string.h>
#include <assert.h>

#include <XcodaUi.h>
#include <rcClientHandler.h>

class rcRunCInfoPanel: public XcodaUi
{
public:
  // constructor and destructor
  rcRunCInfoPanel (Widget parent, char* name, rcClientHandler& handler);
  virtual ~rcRunCInfoPanel (void);

  // initialize all widgets
  void     init (void);
  // redefine manage
  virtual void manage   (void);
  virtual void unmanage (void);

  // class name
  virtual const char* className (void) const {return "rcRunCInfoPanel";}

protected:
  // clock time callback + off time callback
  static void timeCallback (int status, void *arg, daqNetData* data);
  // expriment name callback
  static void exptnameCallback (int status, void* arg, daqNetData* data);
  // experiment runtype callback
  static void runtypeCallback (int status, void* arg, daqNetData* data);
  // server host name callback
  static void hostnameCallback (int status, void* arg, daqNetData* data);
#if defined (_CODA_2_0) || defined (_CODA_2_0_T)
  // server database name callback
  static void databaseCallback (int status, void* arg, daqNetData* data);
#else
  // experiment id callback
  static void exptidCallback (int status, void* arg, daqNetData* data);
#endif
  static void offCallback  (int status, void *arg, daqNetData* data);

private:
  // network handler to server
  rcClientHandler& netHandler_;
  // all widgets
  Widget parent_;
  // experiment name
  Widget exptname_;
  // run type
  Widget runType_;
  // host name
  Widget hostname_;
  // time
  Widget time_;
#if defined (_CODA_2_0_T) || defined (_CODA_2_0)
  // database name
  Widget database_;
#else
  // exptid
  Widget exptid_;
#endif
};
#endif
