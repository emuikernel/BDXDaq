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
//      Implmentation of rcInfoRunPanel Class
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: rcInfoRunPanel.cc,v $
//   Revision 1.5  1998/04/08 18:31:21  heyes
//   new look and feel GUI
//
//   Revision 1.4  1997/06/16 12:26:46  heyes
//   add dbedit and so on
//
//   Revision 1.3  1997/06/06 18:51:26  heyes
//   new RC
//
//   Revision 1.2  1997/05/26 12:27:46  heyes
//   embed tk in RC GUI
//
//   Revision 1.1.1.1  1996/10/11 13:39:25  chen
//   run control source
//
//
#include <Xm/Form.h>
#include <Xm/Frame.h>
#include <Xm/LabelG.h>
#include <Xm/Label.h>
#include <Xm/TextF.h>
#include <daqNetData.h>
//#include <rcRunStatusPanel.h>
#include <rcRunCInfoPanel.h>
#include <rcRunSInfoPanel.h>
#include <rcRunDInfoPanel.h>
#include <rcRunTypeDialog.h>
#include "rcInfoRunPanel.h"
#include <rcMenuWindow.h>

rcInfoRunPanel::rcInfoRunPanel (Widget parent, char* name, 
				rcClientHandler& handler)
:XcodaUi (name), parent_ (parent), netHandler_ (handler)
{
#ifdef _TRACE_OBJECTS
  printf ("              Create rcInfoRunPanel Class Object\n");
#endif
#if defined (_CODA_2_0_T) || defined (_CODA_2_0)
  datafile_ = 0;
#endif
}

rcInfoRunPanel::~rcInfoRunPanel (void)
{
#ifdef _TRACE_OBJECTS
  printf ("              Delete rcInfoRunPanel Class Object\n");
#endif
  // delete rcRunStatusPanel since it has no destroy handler
  //delete statusPanel_;
}

void
rcInfoRunPanel::init (void)
{
  Arg arg[20];
  int ac = 0;

  /* sergey: place new objects in 'runInfoPanel' ?? (see rcInfoPanel.cc) */

  XtSetArg (arg[ac], XmNshadowType, XmSHADOW_ETCHED_IN); ac++;
  _w = XtCreateWidget ("runInfoPanel", xmFrameWidgetClass,
		       parent_, arg, ac);
  ac = 0; 
  XtSetArg (arg[ac], XmNshadowThickness, 0); ac++;
  Widget form = XtCreateWidget ("runPanelForm", xmFormWidgetClass,
				_w, arg, ac);

  // create run type dialog first
  runTypeDialog_ = new rcRunTypeDialog (form, "runTypeDialog",
					"Run Type Configuration",
					netHandler_);
  runTypeDialog_->init ();
  runTypeDialog_->setModal ();


  // setup x resources for all
  ac = 0;
  cinfoPanel_ = new rcRunCInfoPanel (form, "criticalInfoPanel", netHandler_);
  cinfoPanel_->init ();
  XtSetArg (arg[ac], XmNtopAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNleftAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNrightAttachment, XmATTACH_FORM); ac++;
  XtSetValues (cinfoPanel_->baseWidget(), arg, ac);

  // First put netStatus and simple information on top row
  
  // Create frame sessStatFrame to hold form topForm
  ac = 0;
 
  XtSetArg (arg[ac], XmNtopAttachment, XmATTACH_WIDGET); ac++;
  XtSetArg (arg[ac], XmNtopWidget, cinfoPanel_->baseWidget()); ac++;
  XtSetArg (arg[ac], XmNleftAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNrightAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNshadowType, XmSHADOW_ETCHED_IN); ac++;
  Widget sessStatFrame = XtCreateWidget ("runPanelsessStatFrame",
				    xmFrameWidgetClass,
				    form, arg, ac);
  
  // create frame title
  ac = 0;
  XmString t;
  t = XmStringCreateSimple ("Session status");
  XtSetArg (arg[ac], XmNlabelString, t); ac++;
  XtSetArg (arg[ac], XmNchildType, XmFRAME_TITLE_CHILD); ac++;
  Widget rilabel = XtCreateWidget ("runstatusLabel",
					 xmLabelGadgetClass,
					 sessStatFrame, arg, ac);
  XtManageChild (rilabel);
  XmStringFree (t);
  
    // Create form topForm inside frame
  Widget topForm = XtCreateWidget ("runPanelTopForm", xmFormWidgetClass,
				   sessStatFrame, NULL, 0);
				   
  // Create statusPanel and sinfoPanel as children of topForm
  //statusPanel_ = new rcRunStatusPanel (topForm, "netStatus", netHandler_);
  sinfoPanel_ = new rcRunSInfoPanel (topForm, "simpleInfoPanel", netHandler_);
  sinfoPanel_->init ();

  
  // Create and position at bottom of topForm a
  // simple label showing the current data file name.
  ac = 0;
  XtSetArg (arg[ac], XmNtopAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNleftAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNrightAttachment, XmATTACH_FORM); ac++;
  // XtSetArg (arg[ac], XmNrightAttachment, XmATTACH_POSITION); ac++;
  // XtSetArg (arg[ac], XmNrightPosition, 40); ac++;
  XtSetArg (arg[ac], XmNshadowType, XmSHADOW_ETCHED_OUT); ac++;
  Widget fnframe = XtCreateWidget ("datafn", xmFrameWidgetClass,
		       topForm, arg, ac);
  t = XmStringCreateSimple ("Data file name");

  ac = 0;
  XtSetArg (arg[ac], XmNlabelString, t); ac++;
  XtSetArg (arg[ac], XmNchildType, XmFRAME_TITLE_CHILD); ac++;
  Widget datalabel = XtCreateManagedWidget ("datafilelabel", xmLabelGadgetClass,
				     fnframe, arg, ac);
  XmStringFree (t);

  ac = 0;
  XtSetArg (arg[ac], XmNrecomputeSize, FALSE); ac++;
  XtSetArg (arg[ac], XmNeditable, False); ac++;
  XtSetArg (arg[ac], XmNheight, 20); ac++;
  XtSetArg (arg[ac], XmNmarginHeight, 0); ac++;
  XtSetArg (arg[ac], XmNmarginWidth, 0); ac++;
  XtSetArg (arg[ac], XmNblinkRate, 0); ac++;
  t = XmStringCreateSimple ("Unknown");
  XtSetArg (arg[ac], XmNlabelString, t); ac++;

  datafile_ = XtCreateManagedWidget ("datafilename", xmLabelWidgetClass,
				     fnframe, arg, ac);
  XmStringFree (t);

  // Position status panel, 
  ac = 0;
  XtSetArg (arg[ac], XmNtopAttachment, XmATTACH_WIDGET); ac++;
  XtSetArg (arg[ac], XmNtopWidget, fnframe); ac++;
  XtSetArg (arg[ac], XmNleftAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNrightAttachment, XmATTACH_POSITION); ac++;
  XtSetArg (arg[ac], XmNrightPosition, 40); ac++;
  //XtSetArg (arg[ac], XmNheight, 100); ac++;
  XtSetArg (arg[ac], XmNbottomAttachment, XmATTACH_FORM); ac++;
  //XtSetArg (arg[ac], XmNbottomAttachment, XmATTACH_WIDGET); ac++;
  //XtSetArg (arg[ac], XmNbottomWidget, fnframe); ac++;
  //XtSetValues (statusPanel_->baseWidget(), arg, ac);
  statusPanel_ = XtCreateManagedWidget ("statuspanel", xmFrameWidgetClass,
				     topForm, arg, ac);

  ac = 0;
  XtSetArg (arg[ac], XmNtopAttachment, XmATTACH_WIDGET); ac++;
  XtSetArg (arg[ac], XmNtopWidget, fnframe); ac++;
  XtSetArg (arg[ac], XmNleftAttachment, XmATTACH_POSITION); ac++;
  XtSetArg (arg[ac], XmNleftPosition, 40); ac++;
  XtSetArg (arg[ac], XmNrightAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNbottomAttachment, XmATTACH_FORM); ac++;
  XtSetValues (sinfoPanel_->baseWidget(), arg, ac);



  // create the bottom panel
  ac = 0;
  dinfoPanel_ = new rcRunDInfoPanel (form, "dynInfoPanel", netHandler_, statusPanel_);
  dinfoPanel_->init ();



  /* sergey: !!! if commented out, 'Run Progress' is not shown, but statustic is big !!!*/
  XtSetArg (arg[ac], XmNtopAttachment, XmATTACH_WIDGET); ac++;



  XtSetArg (arg[ac], XmNtopWidget, sessStatFrame); ac++;
  XtSetArg (arg[ac], XmNleftAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNrightAttachment, XmATTACH_FORM); ac++;
  //XtSetArg (arg[ac], XmNheight, 100); ac++;
  XtSetArg (arg[ac], XmNbottomAttachment, XmATTACH_FORM); ac++;
  XtSetValues (dinfoPanel_->baseWidget(), arg, ac);
  ac = 0;




  // install destroy handler
  installDestroyHandler ();

  // manage all widgets except top widget
  XtManageChild (datalabel);
  XtManageChild (fnframe);
  XtManageChild (topForm);
  XtManageChild (sessStatFrame);
  XtManageChild (form);
}

void
rcInfoRunPanel::config (int st)
{
  sinfoPanel_->config (st);
  //statusPanel_->config (st);
  dinfoPanel_->config (st);
}

void
rcInfoRunPanel::anaLogChanged (daqNetData* info, int added)
{
  //statusPanel_->anaLogChanged (info, added);
#if defined (_CODA_2_0_T) || defined (_CODA_2_0)
  if (datafile_ != 0) 
    updateDataFileLabel ();
#endif
}

void
rcInfoRunPanel::manage (void)
{
  XcodaUi::manage ();
  cinfoPanel_->manage ();
  sinfoPanel_->manage ();
  dinfoPanel_->manage ();
  runTypeDialog_->startMonitoringRunTypes ();

#if defined (_CODA_2_0_T) || defined (_CODA_2_0)
  // check whether a data file name already presented, so update
  // file name on the label
  if (datafile_ != 0)
    updateDataFileLabel ();
#endif
}

void
rcInfoRunPanel::unmanage (void)
{
  cinfoPanel_->unmanage ();
  sinfoPanel_->unmanage ();
  dinfoPanel_->unmanage ();
  runTypeDialog_->endMonitoringRunTypes ();
  XcodaUi::unmanage ();
}

void
rcInfoRunPanel::stop (void)
{
  //statusPanel_->endDataTaking ();
  dinfoPanel_->endDataTaking ();
}

rcRunTypeDialog*
rcInfoRunPanel::runTypeDialog (void)
{
  return runTypeDialog_;
}

void
rcInfoRunPanel::zoomOnEventInfo (void)
{
  dinfoPanel_->zoomOnEventInfo ();
}

void
rcInfoRunPanel::popupRateDisplay (rcMenuWindow *menW)
{
  dinfoPanel_->popupRateDisplay (menW);
}

#if defined (_CODA_2_0_T) || defined (_CODA_2_0)
void
rcInfoRunPanel::updateDataFileLabel (void)
{
  Arg arg[10];
  int ac = 0;
  XmString t;
  
  char *filename = netHandler_.datalogFile ();
  if (filename && ::strcmp (filename, "unknown") != 0) {
    char *s = 0;
    //    if ((s = ::strrchr (filename, '/')) != 0) {
    //  s++;
    //  if (::strlen (s) > 0) 
    //	t = XmStringCreateSimple (s);
    // else
    //t = XmStringCreateSimple ("   ");
    //    }
    //else
      t = XmStringCreateSimple (filename);
  }
  else
    t = XmStringCreateSimple ("   ");
  
  XtSetArg (arg[ac], XmNlabelString, t); ac++;
  XtSetValues (datafile_, arg, ac);
  ac = 0;
  XmStringFree (t);
}
#endif

