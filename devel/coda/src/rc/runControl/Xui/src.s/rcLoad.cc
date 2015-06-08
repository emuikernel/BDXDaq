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
//      Implementation of RunControl Load Database Command
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: rcLoad.cc,v $
//   Revision 1.3  1998/06/18 12:20:38  heyes
//   new GUI ready I think
//
//   Revision 1.2  1996/12/04 18:32:29  chen
//   port to 1.4 on hp and ultrix
//
//   Revision 1.1.1.1  1996/10/11 13:39:26  chen
//   run control source
//
//
#include <rcLoadDialog.h>
#include <rcButtonPanel.h>
#include <rcComdOption.h>
#include "rcLoad.h"

#define RC_LOAD_NAME "   Load   "
#define RC_LOAD_MSG  "Load database to the Server"

#if defined (_CODA_2_0_T) || defined (_CODA_2_0)
rcLoad::rcLoad (Widget parent, rcButtonPanel* panel, rcClientHandler& handler)
:rcComdButton (parent, RC_LOAD_NAME, RC_LOAD_MSG, panel, handler,"B")
{
#ifdef _TRACE_OBJECTS
  printf ("              Create rcLoad Class Object\n");
#endif
  // empty
}
#else
rcLoad::rcLoad (Widget parent, rcButtonPanel* panel, rcClientHandler& handler)
:rcComdButton (parent, RC_LOAD_NAME, RC_LOAD_MSG, panel, handler), 
 dialog_ (0)
{
#ifdef _TRACE_OBJECTS
  printf ("              Create rcLoad Class Object\n");
#endif
  // empty
}
#endif

rcLoad::~rcLoad (void)
{
#ifdef _TRACE_OBJECTS
  printf ("              Delete rcLoad Class Object\n");
#endif
  // empty
  // all dialogs will be removed by Xt Destroy mechanism
}

void
rcLoad::doit (void)
{
#if defined (_CODA_2_0_T) || defined (_CODA_2_0)
  rcComdOption* option = rcComdOption::option ();
  loadRcDbase (option->dbasename (), option->session ());
#else
  if (dialog_ == 0) {
    dialog_ = new rcLoadDialog (this, "rcDbaseDialog", "RCDATABASE Entry");
    dialog_->init ();
    dialog_->setMessage ("RCDATABASE:");
    dialog_->setModal ();
  }
  dialog_->popup ();
#endif
}

void
rcLoad::undoit (void)
{
  // empty
}

#if defined (_CODA_2_0_T) || defined (_CODA_2_0)
void
rcLoad::loadRcDbase (char *dbase, char* session)
{
  // get network handler
  rcClient& client = netHandler_.clientHandler ();

  daqData data ("RCS", "command", "unkown");
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
				       (rcCallback)&(rcLoad::loadCallback), 
				       (void *)this);
  if (status != CODA_SUCCESS) 
    reportErrorMsg ("Cannot send load database command");
}
#else
void
rcLoad::loadRcDbase (char *dbase)
{
  // get network handler
  rcClient& client = netHandler_.clientHandler ();

  daqData data ("RCS", "command", "unkown");
  // insert database name + session name into data object
  char* temp[2];
  temp[0] = new char[::strlen (dbase) + 1];
  ::strcpy (temp[0], dbase);
  temp[1] = new char[::strlen ("unknown") + 1];
  ::strcpy (temp[1], "unknown");
  data.assignData (temp, 2);
  // free memory
  delete []temp[0]; 
  delete []temp[1];
  
  int status = client.sendCmdCallback (DALOADDBASE, data, 
				       (rcCallback)&(rcLoad::loadCallback), 
				       (void *)this);
  if (status != CODA_SUCCESS) 
    reportErrorMsg ("Cannot send load database command");
}
#endif

void
rcLoad::loadCallback (int status, void* arg, daqNetData* data)
{
  rcLoad* obj = (rcLoad *)arg;
  if (status != CODA_SUCCESS && status != CODA_IGNORED)
    obj->reportErrorMsg ("Loading database failed !!!");
}
