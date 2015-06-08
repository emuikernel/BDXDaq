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
//      RunControl Menu Bar Toggle Command (Generic Class)
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: rcMenuTogComd.h,v $
//   Revision 1.1.1.1  1996/10/11 13:39:27  chen
//   run control source
//
//
#ifndef _RC_MENU_TOG_COMD_H
#define _RC_MENU_TOG_COMD_H

#include <codaTbtnComd.h>
#include <rcClientHandler.h>

class rcNetStatus;
class rcInfoPanel;
class XcodaErrorDialog;

class rcMenuTogComd: public codaTbtnComd 
{
public:
  virtual ~rcMenuTogComd (void);

  // setup pointer for two dynamic panel which have to be stopped before
  // the runcontrol exits
  virtual void   netStatusPanel (rcNetStatus* st);
  virtual void   infoPanel      (rcInfoPanel* panel);
  // widget for all dialogs to be based from
  const Widget   dialogBaseWidget (void);

  // reporting error message
  virtual void reportErrorMsg (char* msg); 

  virtual const char *className (void) const {return "rcMenuTogComd";}

protected:
  // constructor
  rcMenuTogComd (char* name, int active,
		 char* acc, char* acc_text, int state,
		 rcClientHandler& handler);
  // netstatus and information panel
  rcNetStatus* netSt_;
  rcInfoPanel* rcipanel_;
  // network handler
  rcClientHandler& netHandler_;

private:
  static XcodaErrorDialog* errDialog_;
};
#endif
