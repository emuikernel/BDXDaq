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
//      Implementation of Configure Button
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: rcConfigure.cc,v $
//   Revision 1.6  1998/06/18 12:20:35  heyes
//   new GUI ready I think
//
//   Revision 1.5  1997/12/04 14:05:49  heyes
//   stuff for Dieter, eye candy!
//
//   Revision 1.4  1997/10/15 16:08:25  heyes
//   embed dbedit, ddmon and codaedit
//
//   Revision 1.3  1997/07/30 15:32:27  heyes
//   clean for Solaris
//
//   Revision 1.2  1997/07/30 14:32:45  heyes
//   add more xpm support
//
//   Revision 1.1.1.1  1996/10/11 13:39:24  chen
//   run control source
//
//
#include <rcInfoPanel.h>
#include <rcRunTypeDialog.h>
#include <rcButtonPanel.h>
#include <rcComdOption.h>
#include <rcAudioOutput.h>
#include "rcConfigure.h"

#define RC_CONFIGURE_NAME "Configure"
#define RC_CONFIGURE_MSG  "Configure an experiment"

rcConfigure::rcConfigure (Widget parent, rcButtonPanel* panel, 
			  rcClientHandler& handler)
:rcComdButton (parent, RC_CONFIGURE_NAME, RC_CONFIGURE_MSG, panel, handler,"B")
{
#ifdef _TRACE_OBJECTS 
  printf ("                   Create rcConfigure Class Object\n");
#endif
  // empty
}

rcConfigure::~rcConfigure (void)
{
#ifdef _TRACE_OBJECTS
  printf ("                   Delete rcConfigure Class Object\n");
#endif
  // empty
}

void doitCbk(rcConfigure *obj)
{
  obj->doit();
}

void
rcConfigure::doit (void)
{
  assert (infoPanel_);

#if defined (_CODA_2_0_T) || defined (_CODA_2_0)
  rcComdOption* option = rcComdOption::option ();
  loadRcDbase (option->dbasename (), option->session ());
#else
  infoPanel_->runTypeDialog()->popup ();
#endif
  rcAudio ("configure a run");
}

void
rcConfigure::undoit (void)
{
  // empty
}

#if defined (_CODA_2_0_T) || defined (_CODA_2_0)
void
rcConfigure::loadRcDbase (char *dbase, char* session)
{
  // get network handler
  rcClient& client = netHandler_.clientHandler ();

  daqData data ("RCS", "command", "unknown");
  // insert database name + session name into data object
  char* temp[2];
  temp[0] = new char[::strlen (dbase) + 1];
  ::strcpy (temp[0], dbase);
  temp[1] = new char[::strlen (session) + 1];
  ::strcpy (temp[1], session);
  data.assignData (temp, 2);
  // free memory
  delete []temp[0]; 
  delete []temp[1];
  
  int status = client.sendCmdCallback (DALOADDBASE, data, 
	       (rcCallback)&(rcConfigure::loadRcDbaseCbk), 
	       (void *)this);
  if (status != CODA_SUCCESS) {
    reportErrorMsg ("Cannot send load database command");
    rcAudio ("can not send load database command");
  }
}

void
rcConfigure::loadRcDbaseCbk (int status, void* arg, daqNetData* data)
{
  rcConfigure* obj = (rcConfigure *)arg;
  if (status != CODA_SUCCESS && status != CODA_IGNORED) {
    obj->reportErrorMsg ("rcConfigure::loadRcDbaseCbk: Loading database failed !!!");
    rcAudio ("rcConfigure::loadRcDbaseCbk: loading database failed");
  }
  else 
    obj->infoPanel_->runTypeDialog()->popup ();
}
#endif
