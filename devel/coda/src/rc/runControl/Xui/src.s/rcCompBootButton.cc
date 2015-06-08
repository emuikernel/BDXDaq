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
//      RunControl Change Boot Option for All Active Components
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: rcCompBootButton.cc,v $
//   Revision 1.5  1999/02/24 20:40:28  rwm
//   Nix compiler warning.
//
//   Revision 1.4  1998/05/28 17:46:52  heyes
//   new GUI look
//
//   Revision 1.3  1998/04/08 18:31:13  heyes
//   new look and feel GUI
//
//   Revision 1.2  1997/09/05 12:03:50  heyes
//   almost final
//
//   Revision 1.1.1.1  1996/10/11 13:39:28  chen
//   run control source
//
//
#include <rcInfoPanel.h>
#include <rcCompBootDialog.h>
#include "rcCompBootButton.h"
#include <daqCompBootStruct.h>

rcCompBootButton::rcCompBootButton (char* name, int active,
				    char* acc, char* acc_text,
				    rcClientHandler& handler)
  :netHandler_ (handler),dialog_ (0)
{
#ifdef _TRACE_OBJECTS
  printf ("         Create rcCompBootButton Class Object\n");
#endif
}

rcCompBootButton::~rcCompBootButton (void)
{
#ifdef _TRACE_OBJECTS
  printf ("         Delete rcCompBootButton Class Object\n");
#endif
}

void
rcCompBootButton::doit (void)
{
  if (!dialog_) {
    //    dialog_ = new rcCompBootDialog (this, "bootdialog",
    //"Boot Option Dialog", netHandler_);
    //dialog_->init ();
    //dialog_->setModal ();
  }
  //dialog_->popup ();
}

void
rcCompBootButton::undoit (void)
{
  // empty
}

void
rcCompBootButton::bootall ()
{
  daqCompBootStruct bootInfo;
  int  numComp = 0;
  char **components = netHandler_.components(numComp);

  for (int i = 0; i < numComp; i++) {
    bootInfo.insertInfo (components[i], 1);
  }

  setCompBootInfo (&bootInfo);
}

void
rcCompBootButton::setCompBootInfo (daqCompBootStruct* info)
{
  // get network handler
  rcClient& client = netHandler_.clientHandler (); 

  daqData data (client.exptname (), "command", (daqArbStruct *)info);

  client.sendCmdCallback (DAAUTOBOOT_INFO, data,
			  (rcCallback)&(rcCompBootButton::compBootInfoCbk),
			  (void *)this);
}

void
rcCompBootButton::compBootInfoCbk (int status, void* arg, daqNetData *)
{

}

