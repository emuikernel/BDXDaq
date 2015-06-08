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
//      Implementation of reporting components option menu
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: rcRepCompOption.cc,v $
//   Revision 1.5  1999/02/25 14:27:11  rwm
//   Fixed typo in comment.
//
//   Revision 1.4  1998/11/05 20:12:20  heyes
//   reverse status updating to use UDP, fix other stuff
//
//   Revision 1.3  1998/09/17 19:18:33  rwm
//   Only print if _CODA_DEBUG is set.
//
//   Revision 1.2  1998/05/28 17:47:03  heyes
//   new GUI look
//
//   Revision 1.1.1.1  1996/10/11 13:39:26  chen
//   run control source
//
//
#include <stdio.h>
#include <string.h>
#include <rcRunDInfoPanel.h>
#include "rcRepCompOption.h"

rcRepCompOption::rcRepCompOption (Widget parent, char* name,
				  char* title, rcClientHandler& handler,
				  rcRunDInfoPanel* panel)
:XcodaSimpleOptionMenu (parent, name, title), netHandler_ (handler),
 dpanel_ (panel), numComp_ (0)
{
#ifdef _TRACE_OBJECTS
  printf ("                   Create rcRepCompOption Class Object\n");
#endif
}

rcRepCompOption::~rcRepCompOption (void)
{
#ifdef _TRACE_OBJECTS
  printf ("                   Delete rcRepCompOption Class Object\n");
#endif
  for (int i = 0; i < numComp_; i++)
    delete []components_[i];
}

void
rcRepCompOption::manage (void)
{
  XcodaSimpleOptionMenu::manage ();
  // add monitor on callbacks for active components
  rcClient& client = netHandler_.clientHandler ();
  if (client.monitorOnCallback (client.exptname (), "components",
                                (rcCallback)&(rcRepCompOption::compCallback),
                                (void *)this) != CODA_SUCCESS)
    fprintf (stderr, "Cannot register monitor on components callback\n");
}

void
rcRepCompOption::unmanage (void)
{
  XcodaSimpleOptionMenu::unmanage ();
  // add monitor off callback
  rcClient& client = netHandler_.clientHandler ();
  if (client.monitorOffCallback (client.exptname (), "components",
				 (rcCallback)&(rcRepCompOption::compCallback),
				 (void *)this,
				 (rcCallback)&(rcRepCompOption::offCallback),
				 (void *)this) != CODA_SUCCESS) {
#ifdef _CODA_DEBUG
    fprintf (stderr, "Cannot register monitor off components callback\n");
#endif
  }
}

void
rcRepCompOption::compCallback (int status, void* arg, daqNetData* data)
{
  rcRepCompOption* obj = (rcRepCompOption *)arg;

  if (status == CODA_SUCCESS) {
    // do not expect more than RCXUI_MAX_COMPONENTS components
    int count = RCXUI_MAX_COMPONENTS;
    
    // delete old result
    for (int i = 0; i < obj->numComp_; i++)
      delete obj->components_[i];
    obj->numComp_ = 0;

    if (data->getData (obj->components_, count) != CODA_ERROR) {
      obj->removeAll ();
      obj->numComp_ = count;
      obj->addEntries (obj->components_, count);
    }
  }
}

void
rcRepCompOption::offCallback (int status, void *, daqNetData* data)
{
  if (status != CODA_SUCCESS)
    printf ("monitor off failed\n");
}

void
rcRepCompOption::doit (void)
{
  assert (currentSel_ < numComp_);

  if (netHandler_.status () >= DA_DOWNLOADED ) {
    if (currentSel_ != prevSel_) {
      dpanel_->stopMonitoringInfo (components_[prevSel_]);
      dpanel_->startMonitoringInfo (components_[currentSel_]);
    }
  }
}

void
rcRepCompOption::entriesChangedAction (void)
{
  if (netHandler_.status () >= DA_PAUSED ) 
    dpanel_->startMonitoringInfo (components_[currentSel_]);
}

char*
rcRepCompOption::currentComponent (void) const
{
  assert (currentSel_ < numComp_);
  
  if (::strcmp (components_[currentSel_], "unknown") == 0)
    return 0;
  else
    return components_[currentSel_];
}
