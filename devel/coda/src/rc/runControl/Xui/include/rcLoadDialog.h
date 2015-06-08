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
//      RunControl Load Database Dialog
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: rcLoadDialog.h,v $
//   Revision 1.1.1.1  1996/10/11 13:39:26  chen
//   run control source
//
//
#ifndef _RC_LOAD_DIALOG_H
#define _RC_LOAD_DIALOG_H

#include <stdio.h>
#include <string.h>
#include <assert.h>

#include <XcodaStringPromptDialog.h>
#include <XcodaFormDialog.h>

class rcLoad;

#if defined (_CODA_2_0_T) || defined (_CODA_2_0)
class rcDbaseOption;

class rcLoadDialog: public XcodaFormDialog
{
public:
  // constructor and destructor
  rcLoadDialog  (rcLoad* load, char* name, char* title);
  ~rcLoadDialog (void);

  // redefine popup
  void popup (void);

  // class name
  virtual const char* className (void) const {return "rcLoadDialog";}

protected:
  // redefine inherited ok
  virtual void ok (void);
  virtual void cancel (void);
  // create form children
  void         createFormChildren (void);

  // push button callback
  static void okCallback     (Widget w, XtPointer data, XmAnyCallbackStruct* );
  static void cancelCallback (Widget w, XtPointer data, XmAnyCallbackStruct* );

private:
  // command button
  rcLoad* load_;
  // option menu
  rcDbaseOption* option_;
};
#else
class rcLoadDialog: public XcodaStringPromptDialog
{
public:
  // constructor and destructor
  rcLoadDialog  (rcLoad* load, char* name, char* title);
  ~rcLoadDialog (void);

  // redefine popup
  void popup (void);

  // class name
  virtual const char* className (void) const {return "rcLoadDialog";}

protected:
  // redefine inherited ok
  virtual void ok (void);
  virtual void cancel (void);

private:
  // command button
  rcLoad* load_;
};
#endif

#endif
