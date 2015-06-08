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
//      RunControl Option Menu For Automatic Booting Components
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: rcCompBootButton.h,v $
//   Revision 1.3  1998/04/08 18:31:13  heyes
//   new look and feel GUI
//
//   Revision 1.2  1997/09/05 12:03:51  heyes
//   almost final
//
//   Revision 1.1.1.1  1996/10/11 13:39:28  chen
//   run control source
//
//
#ifndef _RC_COMP_BOOT_BUTTON_H
#define _RC_COMP_BOOT_BUTTON_H

#include <rcClientHandler.h>

class rcCompBootDialog;
class daqCompBootStruct;

class rcCompBootButton
{
public:
  rcCompBootButton (char* name, int active,
		    char* acc, char* acc_text,
		    rcClientHandler& handler);
  virtual ~rcCompBootButton (void);

  // operation
  void    setCompBootInfo (daqCompBootStruct* info);

  // class name
  virtual const char* className (void) const {return "rcCompBootButton";}
  void bootall ();
  virtual void doit (void);
protected:

  virtual void undoit (void);

  // setup boot info callbacks
  static void compBootInfoCbk (int status, void* arg, daqNetData* data);

private:
  rcClientHandler& netHandler_;
  rcCompBootDialog* dialog_;
};

#endif
