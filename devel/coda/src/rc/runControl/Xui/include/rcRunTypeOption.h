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
//      RunControl RunType Option Menu
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: rcRunTypeOption.h,v $
//   Revision 1.1.1.1  1996/10/11 13:39:26  chen
//   run control source
//
//
#ifndef _RC_RUN_TYPE_OPTION_H
#define _RC_RUN_TYPE_OPTION_H

#include <XcodaSimpleOptionMenu.h>
#include <rcClientHandler.h>
#include <rcXuiConsts.h>

class rcRunTypeDialog;

class rcRunTypeOption: public XcodaSimpleOptionMenu
{
public:
  // constructor and destructor
  rcRunTypeOption (Widget parent,
		   char* name, char* title,
		   rcClientHandler& handler,
		   rcRunTypeDialog* rdialog);
  virtual ~rcRunTypeOption (void);

  // function of start/end monitoring runtype
  void startMonitoringRunTypes (void);
  void endMonitoringRunTypes   (void);

  // return current selection of runtype
  char* currentRunType (void);

  // set up all menu entries 
  void  setAllEntries  (void);

  // class name
  virtual const char* className (void) const {return "rcRunTypeOption";}

protected:
  // monitor on and off callback
  static void runTypesCallback (int status, void* arg, daqNetData* data);
#if defined (_CODA_2_0_T) || defined (_CODA_2_0)
  static void rtStatusCallback (int status, void* arg, daqNetData* data);
#endif
  static void offCallback      (int status, void* arg, daqNetData* data);
  
private:
  // network handler
  rcClientHandler& netHandler_;
  // parent run type dialog
  rcRunTypeDialog* dialog_;
  // all runtypes: potential danger: not exceeding 100 runtypes
  char*            runtypes_[RCXUI_MAX_RUNTYPES];
  // current run type selection
  char*            currRunType_;
  int              numRuntypes_;
};
#endif

