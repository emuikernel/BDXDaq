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
//      Implementation of run type option menu
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: rcRunTypeOption.cc,v $
//   Revision 1.8  2000/08/21 18:27:58  abbottd
//   Sun 5.0 C++ compiler fix
//
//   Revision 1.7  1998/09/17 19:18:38  rwm
//   Only print if _CODA_DEBUG is set.
//
//   Revision 1.6  1998/06/18 12:20:43  heyes
//   new GUI ready I think
//
//   Revision 1.5  1997/10/15 16:08:32  heyes
//   embed dbedit, ddmon and codaedit
//
//   Revision 1.4  1997/07/08 15:22:44  heyes
//   add doTk to connect dialog
//
//   Revision 1.3  1997/06/16 13:22:54  heyes
//   clear graph
//
//   Revision 1.2  1997/06/16 12:26:52  heyes
//   add dbedit and so on
//
//   Revision 1.1.1.1  1996/10/11 13:39:26  chen
//   run control source
//
//
#include <stdio.h>
#include <string.h>
#include <rcRunTypeDialog.h>
#ifdef USE_CREG
#include <codaRegistry.h>
#endif
#include "rcRunTypeOption.h"

rcRunTypeOption::rcRunTypeOption (Widget parent,
				  char* name,
				  char* title,
				  rcClientHandler& handler,
  				  rcRunTypeDialog* dialog)
:XcodaSimpleOptionMenu (parent, name, title), netHandler_ (handler),
 dialog_ (dialog), numRuntypes_ (0), currRunType_ (0)
{
#ifdef _TRACE_OBJECTS
  printf ("                       Create rcRunTypeOption Class Object\n");
#endif
  // empty
}

rcRunTypeOption::~rcRunTypeOption (void)
{
#ifdef _TRACE_OBJECTS
  printf ("                       Delete rcRunTypeOption Class Object\n");
#endif
  if (currRunType_)
    delete []currRunType_;

  for (int i = 0; i < numRuntypes_; i++)
    delete []runtypes_[i];
}

void
rcRunTypeOption::startMonitoringRunTypes (void)
{
  rcClient& client = netHandler_.clientHandler ();
  if (client.monitorOnCallback (client.exptname (), "allRunTypes",
				(rcCallback)&(rcRunTypeOption::runTypesCallback),
				(void *)this) != CODA_SUCCESS) {
#ifdef _CODA_DEBUG
    fprintf (stderr, "Cannot register monitor on allRunTypes callback\n");
#endif
  }
}

void
rcRunTypeOption::endMonitoringRunTypes (void)
{
  rcClient& client = netHandler_.clientHandler ();
  if (client.monitorOffCallback (client.exptname (), "allRunTypes",
				 (rcCallback)&(rcRunTypeOption::runTypesCallback),
				 (void *)this,
				 (rcCallback)&(rcRunTypeOption::offCallback),
				 (void *)this) != CODA_SUCCESS) {
#ifdef _CODA_DEBUG
    fprintf (stderr, "Cannot register monitor off allRunTypes callback\n");
#endif
  }
}

void
rcRunTypeOption::runTypesCallback (int status, void* arg, daqNetData* data)
{
  rcRunTypeOption* obj = (rcRunTypeOption *)arg;

  if (status == CODA_SUCCESS) {
    // do not expecet more than 100 run types
    int count = RCXUI_MAX_RUNTYPES;

    // delete old run types
    for (int i = 0; i < obj->numRuntypes_; i++)
      delete obj->runtypes_[i];
    obj->numRuntypes_ = 0;

    if (data->getData (obj->runtypes_, count) != CODA_ERROR) 
      obj->numRuntypes_ = count;
  }
}

void
rcRunTypeOption::offCallback (int status, void* , daqNetData* data)
{
  if (status != CODA_SUCCESS) {
#ifdef _CODA_DEBUG
    printf ("monitor off failed\n");
#endif
  }
}

void
rcRunTypeOption::setAllEntries (void)
{
  int i;
  // clean up old entries and add new ones
  removeAll  ();

  if (currRunType_) {
    for (i = 0; i < numRuntypes_; i++) {
      if (::strcmp (currRunType_, runtypes_[i]) == 0) 
	break;
    }
    if (i < numRuntypes_)
      addEntries (runtypes_, numRuntypes_, i);
    else
      addEntries (runtypes_, numRuntypes_);
  }
  else
    addEntries (runtypes_, numRuntypes_);
}

extern int doTk;
char*
rcRunTypeOption::currentRunType (void)
{
  assert (currentSel_ < numRuntypes_);

  // remove old information
  if (currRunType_)
    delete []currRunType_;
  currRunType_ = 0;

  if (::strcmp (runtypes_[currentSel_], "unknown") == 0)
    return 0;
  else {
    currRunType_ = new char[::strlen (runtypes_[currentSel_]) + 1];
    ::strcpy (currRunType_, runtypes_[currentSel_]);

    {
      char cmd[100];
      sprintf(cmd,"c:%s",currRunType_);
printf("CEDIT 4: >%s<\n",cmd);
#ifdef USE_CREG
      coda_send(XtDisplay(this->baseWidget()),"CEDIT",cmd);
#endif
    }
    
    return runtypes_[currentSel_];
  }
}
