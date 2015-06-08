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
//      RunControl Menu Bar Command (Generic Class)
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: rcMenuComd.h,v $
//   Revision 1.1.1.1  1996/10/11 13:39:26  chen
//   run control source
//
//
#ifndef _RC_MENU_COMD_H
#define _RC_MENU_COMD_H

#include <codaPbtnComd.h>
#include <rcClientHandler.h>

class rcNetStatus;
class rcInfoPanel;
class XcodaErrorDialog;

class rcMenuComd: public codaPbtnComd 
{
public:
  virtual ~rcMenuComd (void);

  // setup pointer for two dynamic panel which have to be stopped before
  // the runcontrol exits
  virtual void   netStatusPanel (rcNetStatus* st);
  virtual void   infoPanel      (rcInfoPanel* panel);
  // widget for all dialogs to be based from
  const Widget          dialogBaseWidget (void);

  // error reporting mechanism
  virtual void reportErrorMsg (char* msg); 

  virtual const char *className (void) const {return "rcMenuComd";}

protected:
  // constructor
  rcMenuComd (char* name, int active,
	      char* acc, char* acc_text, rcClientHandler& handler);

  // netstatus and information panel
  rcNetStatus* netSt_;
  rcInfoPanel* rcipanel_;
  // network handler
  rcClientHandler& netHandler_;

private:
  static XcodaErrorDialog* errDialog_;
};
#endif
