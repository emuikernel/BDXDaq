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
//      RunControl Connect Command
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: rcConnect.h,v $
//   Revision 1.3  1997/09/05 12:03:52  heyes
//   almost final
//
//   Revision 1.2  1997/07/30 14:32:48  heyes
//   add more xpm support
//
//   Revision 1.1.1.1  1996/10/11 13:39:24  chen
//   run control source
//
//
#ifndef _RC_CONNECT_H
#define _RC_CONNECT_H

#include <stdio.h>
#include <string.h>
#include <assert.h>

#include <rcXpmComdButton.h>
#include <rcConnectDialog.h>
#include <rcClientHandler.h>

class rcButtonPanel;

class rcConnect: public rcXpmComdButton
{
public:
  rcConnect  (Widget parent, rcButtonPanel* panel, rcClientHandler& handler);
  ~rcConnect (void);

  // class name
  virtual const char* className (void) const {return "rcConnect";}
  
protected:
  // inherited functions
  virtual void doit   (void);
  virtual void undoit (void);

#if defined (_CODA_2_0) || defined (_CODA_2_0_T)
  int  connect       (void);
  void startRcServer (void);
  
  void startTimer    (void);
  void endTimer      (void);
  static void autoTimerCallback (XtPointer, XtIntervalId *);
  static void configureCallback (int,void*, daqNetData*);
#endif

private:

#if defined (_CODA_2_0_T) || defined (_CODA_2_0)
  // data area
  int          armed_;
  XtIntervalId autoTimerId_;
  XtAppContext appContext_;
  int          timeoutCount_;
  static int   maxCount_;
#endif
  // friend class
  friend class rcConnectDialog;
  // dialog box assoicated with connect
  rcConnectDialog* dialog_;
};
#endif

