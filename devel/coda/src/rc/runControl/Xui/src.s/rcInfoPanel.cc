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
//      Implementation of RunControl Information Panel
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: rcInfoPanel.cc,v $
//   Revision 1.6  1998/06/18 12:20:37  heyes
//   new GUI ready I think
//
//   Revision 1.5  1998/04/08 18:31:20  heyes
//   new look and feel GUI
//
//   Revision 1.4  1997/07/18 16:54:45  heyes
//   new GUI
//
//   Revision 1.3  1997/06/06 18:51:25  heyes
//   new RC
//
//   Revision 1.2  1996/10/14 20:13:38  chen
//   add display server messages preference
//
//   Revision 1.1.1.1  1996/10/11 13:39:24  chen
//   run control source
//
//
#include <daqNetData.h>
#include <rcInfoInitPanel.h>
#include <rcInfoRunPanel.h>
#include <rcRunTypeDialog.h>
#include "rcInfoPanel.h"
#include <rcMenuWindow.h>

extern "C" void loadAndOrJump(char *file, char *loc, Boolean store);

rcInfoPanel::rcInfoPanel (Widget parent,
			  char* name,
			  rcClientHandler& handler)
:XcodaUi (name), rcPanel (), parent_ (parent), initPanel_ (0), runPanel_ (0),
 netHandler_ (handler)
{
#ifdef _TRACE_OBJECTS
  printf ("              Create rcInfoPanel Class Object\n");
#endif
  // register this panel to network handler
  netHandler_.addPanel (this);
}

rcInfoPanel::~rcInfoPanel (void)
{
#ifdef _TRACE_OBJECTS
  printf ("              Delete rcInfoPanel Class Object\n");
#endif
  netHandler_.removePanel (this);
  // sinec initPanel has no destroy callback associated with it,
  // we have to delete it explicitly
  delete initPanel_;
}

void
rcInfoPanel::init (void)
{
  Arg arg[20];
  int ac = 0;

  _w = XtCreateWidget ("infoForm", xmFormWidgetClass, parent_, arg, ac);
  // handle destroy callback
  installDestroyHandler ();


  /* sergey: main run control panel (will be resized in runcontrol.cc, so ignore numbers here) */
  // create init panel


  //initPanel_ = new rcInfoInitPanel (_w, "initInfoPanel", 460, 550);
  initPanel_ = new rcInfoInitPanel (_w, "initInfoPanel", 0, 0);


  XtSetArg (arg[ac], XmNtopAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNbottomAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNleftAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNrightAttachment, XmATTACH_FORM); ac++;
  XtSetValues (initPanel_->baseWidget(), arg, ac);
  ac = 0;

  // create run info panel
  /* all run control objects (except log messages) will be placed into it (?) */
  runPanel_ = new rcInfoRunPanel (_w, "runInfoPanel", netHandler_);
  runPanel_->init ();
  XtSetArg (arg[ac], XmNtopAttachment, XmATTACH_FORM); ac++;

  XtSetArg (arg[ac], XmNbottomAttachment, XmATTACH_FORM); ac++;


  XtSetArg (arg[ac], XmNleftAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNrightAttachment, XmATTACH_FORM); ac++;
  XtSetValues (runPanel_->baseWidget(), arg, ac);
  ac = 0;

  manage ();
 
  initPanel_->manage();

}

void
rcInfoPanel::config (int status)
{
  static int rimanaged = 0;

  char *dollar_coda, file[1024];
  
  dollar_coda = getenv("CODA");
  
  if (status == DA_NOT_CONNECTED) {

    runPanel_->unmanage ();
    initPanel_->manage();
    if (dollar_coda) {
      sprintf(file,"%s/common/html/rc/Notice.html", dollar_coda);
    }
    loadAndOrJump(file,NULL,1);
    rimanaged = 0;
  } else {
    if (!rimanaged) {

      initPanel_->unmanage();
      runPanel_->manage ();

      if (dollar_coda) {
	sprintf(file,"%s/common/html/rc/contextHelp.html", dollar_coda);
      }
      loadAndOrJump(file,NULL,1);
      rimanaged = 1;
    }
    runPanel_->config (status);
  }
}

void
rcInfoPanel::anaLogChanged (daqNetData* info, int added)
{
  runPanel_->anaLogChanged (info, added);
}

void
rcInfoPanel::stop (void)
{
  initPanel_->stop ();
  runPanel_->stop ();
}

void
rcInfoPanel::readyToExit (void)
{
  initPanel_->stop ();
  runPanel_->stop ();
}

rcRunTypeDialog*
rcInfoPanel::runTypeDialog (void)
{
  return runPanel_->runTypeDialog ();
}

void
rcInfoPanel::zoomOnEventInfo (void)
{
  runPanel_->zoomOnEventInfo ();
}

void
rcInfoPanel::popupRateDisplay (rcMenuWindow *menW)
{
  runPanel_->popupRateDisplay (menW);
}

