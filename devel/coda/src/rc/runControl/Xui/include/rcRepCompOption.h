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
//      RunControl Reporting Components Option Menu
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: rcRepCompOption.h,v $
//   Revision 1.1.1.1  1996/10/11 13:39:25  chen
//   run control source
//
//
#ifndef _RC_REP_COMP_OPTION_H
#define _RC_REP_COMP_OPTION_H

#include <XcodaSimpleOptionMenu.h>
#include <rcClientHandler.h>
#include <rcXuiConsts.h>

class rcRunDInfoPanel;

class rcRepCompOption: public XcodaSimpleOptionMenu
{
public:
  // constructor and destructor
  rcRepCompOption (Widget parent, char* name, char* title,
		   rcClientHandler& handler,
		   rcRunDInfoPanel* dpanel);
  virtual ~rcRepCompOption (void);

  // redefine manage and unmanage
  virtual void unmanage (void);
  virtual void manage   (void);

  // return current selection
  char* currentComponent (void) const;

  // class name
  virtual const char* className (void) const {return "rcRepCompOption";}

protected:
  virtual void doit (void);
  virtual void entriesChangedAction (void);
  // monitor on/off callback
  static void compCallback (int status, void* arg, daqNetData* data);
  static void offCallback  (int status, void* arg, daqNetData* data);

private:
  // network handler
  rcClientHandler& netHandler_;
  // parent dynamic information panel
  rcRunDInfoPanel* dpanel_;
  // all components: potentail danger: not exceeding 100 components
  char*            components_[RCXUI_MAX_COMPONENTS];
  int              numComp_;
};
#endif
