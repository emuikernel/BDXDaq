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
//      Implementation of rcRunDInfoPanel Class
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: rcRunDInfoPanel.cc,v $
//   Revision 1.13  1998/11/24 13:57:56  heyes
//   check in for Carl...
//
//   Revision 1.12  1998/11/05 20:12:21  heyes
//   reverse status updating to use UDP, fix other stuff
//
//   Revision 1.11  1998/09/17 19:18:36  rwm
//   Only print if _CODA_DEBUG is set.
//
//   Revision 1.10  1998/09/01 18:48:42  heyes
//   satisfy Randy's lust for command line options
//
//   Revision 1.9  1998/06/18 12:20:41  heyes
//   new GUI ready I think
//
//   Revision 1.8  1998/05/28 17:47:04  heyes
//   new GUI look
//
//   Revision 1.7  1998/04/08 18:31:30  heyes
//   new look and feel GUI
//
//   Revision 1.6  1997/10/15 16:08:30  heyes
//   embed dbedit, ddmon and codaedit
//
//   Revision 1.5  1997/09/05 12:03:54  heyes
//   almost final
//
//   Revision 1.4  1997/07/08 14:59:14  heyes
//   deep trouble
//
//   Revision 1.3  1997/06/06 18:51:30  heyes
//   new RC
//
//   Revision 1.2  1997/05/26 12:27:48  heyes
//   embed tk in RC GUI
//
//   Revision 1.1.1.1  1996/10/11 13:39:25  chen
//   run control source
//
//
#include <Xm/Frame.h>
#include <Xm/Label.h>
#include <Xm/LabelG.h>
#include <Xm/Form.h>
#include <Xm/SeparatoG.h>
 
#include <rcRepCompOption.h>
#include <rcRateDisplay.h>
#ifdef USE_CREG
#include <codaRegistry.h>
#endif
#include "rcRunDInfoPanel.h"
#include <rcMenuWindow.h>

rcRunDInfoPanel::rcRunDInfoPanel (Widget parent, char* name,
				  rcClientHandler& handler,Widget statusPanel)
:XcodaUi (name), parent_ (parent), netHandler_ (handler), statusPanel_ (statusPanel),
 ratePanel_ (0), monitorOn_ (0),
 startTime_ (0), time_ (0), endTime_ (0), evc_ (0), drc_ (0)
{
#ifdef _TRACE_OBJECTS
  printf ("              Create rcRunDInfoPanel Class Object\n");
#endif
  // empty
}

rcRunDInfoPanel::~rcRunDInfoPanel (void)
{
#ifdef _TRACE_OBJECTS
  printf ("              Delete rcRunDInfoPanel Class Object\n");
#endif
}

void
rcRunDInfoPanel::init (void)
{
  Arg arg[20];
  int ac = 0;
  XmString t;
  XtSetArg (arg[ac], XmNshadowType, XmSHADOW_ETCHED_IN); ac++;
  _w = XtCreateWidget (_name, xmFrameWidgetClass, parent_,
		       arg, ac);
  ac = 0;

  // create top form
  Widget dframe = XtCreateWidget ("dInfoPanelFrame", xmFrameWidgetClass,
				_w, NULL, 0);
  XtManageChild (dframe);
  
  ac = 0;
  t = XmStringCreateSimple ("Run progress");
  XtSetArg (arg[ac], XmNlabelString, t); ac++;
  XtSetArg (arg[ac], XmNchildType, XmFRAME_TITLE_CHILD); ac++;
  Widget rplabel = XtCreateWidget ("runsprogressLabel",
					 xmLabelWidgetClass,
					 dframe, arg, ac);
  XtManageChild (rplabel);
  XmStringFree (t);
  
  Widget form = XtCreateWidget ("dInfoPanelForm", xmFormWidgetClass,
				dframe, NULL, 0);
  
  ac = 0;

  XtSetArg (arg[ac], XmNtopAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNleftAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNrightAttachment, XmATTACH_FORM); ac++;
  Widget evnbFrame = XtCreateWidget ("evnbFrame", xmFrameWidgetClass,
				form, arg, ac);
				
  XtManageChild (evnbFrame); 
  ac = 0; 
  XtSetArg (arg[ac], XmNshadowThickness, 0); ac++;
  XtSetArg (arg[ac], XmNfractionBase, 2); ac++;
  Widget evnbForm = XtCreateWidget ("dInfoPanelForm", xmFormWidgetClass,
	            evnbFrame, arg, ac);
  XtManageChild (evnbForm); 

  ac = 0;
  XtSetArg (arg[ac], XmNtopAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNleftAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNbottomAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNshadowType, XmSHADOW_ETCHED_OUT); ac++;
  XtSetArg (arg[ac], XmNrightAttachment, XmATTACH_POSITION ); ac++;
  XtSetArg (arg[ac], XmNrightPosition, 1); ac++;
  Widget evnbFrame2 = XtCreateWidget ("evnbFrame2", xmFrameWidgetClass,
				evnbForm, arg, ac);
				
  XtManageChild (evnbFrame2); 
  ac = 0;
  t = XmStringCreateSimple ("Events this run");
  XtSetArg (arg[ac], XmNlabelString, t); ac++;
  XtSetArg (arg[ac], XmNchildType, XmFRAME_TITLE_CHILD); ac++;
  Widget evnlabel = XtCreateManagedWidget ("evNumLabel",
					   xmLabelWidgetClass,
					   evnbFrame2, arg, ac);
  XtManageChild (evnlabel);
  XmStringFree (t);
  
  ac = 0;
  t = XmStringCreateSimple ("                 0");
  //XtSetArg (arg[ac], XmNwidth, 40); ac++;
  XtSetArg (arg[ac], XmNlabelString, t); ac++;
  XtSetArg (arg[ac], XmNrecomputeSize, FALSE); ac++;
  XtSetArg (arg[ac], XmNshadowType, XmSHADOW_ETCHED_OUT); ac++;
  XtSetArg (arg[ac], XmNalignment, XmALIGNMENT_END); ac++;
  eventNumber_ = XtCreateManagedWidget ("eventNumberG",
					xmLabelWidgetClass,
					evnbFrame2, arg, ac);
  XtManageChild (eventNumber_);
  ac = 0;
  XmStringFree (t);

  ac = 0;

  // This little widget is the pull down menu of all possible sources
  // from which the event number can be read.

  compOption_ = new rcRepCompOption (evnbForm, "compOption",
				     "Read From:", netHandler_, this);
  compOption_->init ();
  XtSetArg (arg[ac], XmNtopOffset, 10); ac++;
  XtSetArg (arg[ac], XmNbottomOffset, 10); ac++;
  XtSetArg (arg[ac], XmNtopAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNbottomAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNrightAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNleftAttachment, XmATTACH_POSITION ); ac++;
  XtSetArg (arg[ac], XmNleftPosition, 1); ac++;
  //XtSetArg (arg[ac], XmNleftWidget, evnbFrame2); ac++;
  XtSetValues (compOption_->baseWidget(), arg, ac);
  XtManageChild ( compOption_->baseWidget());
  // create a seperator
  ac = 0;
  XtSetArg (arg[ac], XmNtopAttachment, XmATTACH_WIDGET); ac++;
  XtSetArg (arg[ac], XmNtopWidget, evnbFrame); ac++;
  XtSetArg (arg[ac], XmNtopOffset, 2); ac++;
  XtSetArg (arg[ac], XmNleftAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNrightAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNrightOffset, 2); ac++;
  Widget sep = XtCreateManagedWidget ("dInfoPanelSep", xmSeparatorGadgetClass,
			       form, arg, ac);
  
  ac = 0;
 
  // create real time 2D plot for differential event rate
  // XtSetArg (arg[ac], XmNtopAttachment, XmATTACH_WIDGET); ac++;
  // XtSetArg (arg[ac], XmNtopWidget, sep); ac++;
  // XtSetArg (arg[ac], XmNleftAttachment, XmATTACH_FORM); ac++;
  // XtSetArg (arg[ac], XmNbottomAttachment, XmATTACH_FORM); ac++;
  // XtSetArg (arg[ac], XmNshadowType, XmSHADOW_ETCHED_OUT); ac++;
  // Widget iEvDispFrame = XtCreateManagedWidget ("iEvDispFrame", xmFrameWidgetClass,
  //	   				       form, arg, ac);

  ac = 0;


  /* ??? */
  //evRateDisp_ = new rc2DRtDisp (iEvDispFrame, "diffEvRateDisp",
  //				0.0, -10.0, 500.0,
  //				120, 60);

  
  // Create a row for event rate and data rate title label
  XtSetArg (arg[ac], XmNtopAttachment, XmATTACH_WIDGET); ac++;
  XtSetArg (arg[ac], XmNtopWidget, sep); ac++;
  XtSetArg (arg[ac], XmNtopOffset, 2); ac++;
  XtSetArg (arg[ac], XmNleftAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNrightAttachment, XmATTACH_FORM); ac++;
  Widget subform1 = XtCreateWidget ("dpanelSubF1", xmFormWidgetClass,
				    form, arg, ac);

  ac = 0;
  
  
  t = XmStringCreateSimple ("Rates");
  XtSetArg (arg[ac], XmNtopAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNbottomAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNleftAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNrightAttachment, XmATTACH_POSITION); ac++;
  XtSetArg (arg[ac], XmNrightPosition, 30); ac++;
  XtSetArg (arg[ac], XmNlabelString, t); ac++;
  Widget evrlabel1 = XtCreateManagedWidget ("evRates", xmLabelWidgetClass,
					   subform1, arg, ac);
  ac = 0;
  XmStringFree (t);

  t = XmStringCreateSimple ("Events/S");
  XtSetArg (arg[ac], XmNtopAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNbottomAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNleftAttachment, XmATTACH_POSITION); ac++;
  XtSetArg (arg[ac], XmNleftPosition, 30); ac++;
  XtSetArg (arg[ac], XmNrightAttachment, XmATTACH_POSITION); ac++;
  XtSetArg (arg[ac], XmNrightPosition, 60); ac++;
  XtSetArg (arg[ac], XmNlabelString, t); ac++;
  Widget evrlabel = XtCreateManagedWidget ("evRateLabel", xmLabelWidgetClass,
					   subform1, arg, ac);
  ac = 0;
  XmStringFree (t);


  t = XmStringCreateSimple ("Rate (KB/S)");
  XtSetArg (arg[ac], XmNtopAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNbottomAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNleftAttachment, XmATTACH_POSITION); ac++;
  XtSetArg (arg[ac], XmNleftPosition, 60); ac++;
  XtSetArg (arg[ac], XmNrightAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNlabelString, t); ac++;
  Widget drlabel = XtCreateManagedWidget ("dataRateLabel", xmLabelWidgetClass,
					  subform1, arg, ac);
  ac = 0;
  XmStringFree (t);

  // create another separator
  XtSetArg (arg[ac], XmNtopAttachment, XmATTACH_WIDGET); ac++;
  XtSetArg (arg[ac], XmNtopWidget, subform1); ac++;
  XtSetArg (arg[ac], XmNtopOffset, 2); ac++;
  XtSetArg (arg[ac], XmNleftAttachment, XmATTACH_POSITION); ac++;
  XtSetArg (arg[ac], XmNleftPosition,30); ac++;
  XtSetArg (arg[ac], XmNrightAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNrightOffset, 2); ac++;
  sep = XtCreateManagedWidget ("dInfoPanelSep", xmSeparatorGadgetClass,
			       form, arg, ac);
  ac = 0;


  /* sergey: statistic graphic is here ? */

  // create a form widget which holds everything for data/event rate number
  XtSetArg (arg[ac], XmNtopAttachment, XmATTACH_WIDGET); ac++;
  XtSetArg (arg[ac], XmNtopWidget, sep); ac++;
  XtSetArg (arg[ac], XmNtopOffset, 3/*3*/); ac++;
  XtSetArg (arg[ac], XmNleftAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNrightAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNbottomAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNbottomOffset, 3/*3*/); ac++;
  Widget botform = XtCreateWidget ("dpanelBotForm", xmFormWidgetClass,
				   form, arg, ac);
  ac = 0;    


  // create integrated data/event rate label
  XtSetArg (arg[ac], XmNtopAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNleftAttachment, XmATTACH_POSITION); ac++;
  XtSetArg (arg[ac], XmNleftPosition, 0); ac++;
  XtSetArg (arg[ac], XmNrightAttachment, XmATTACH_POSITION); ac++;
  XtSetArg (arg[ac], XmNrightPosition, 30); ac++;
  XtSetArg (arg[ac], XmNbottomAttachment, XmATTACH_FORM); ac++;
  Widget labelform2 = XtCreateWidget ("dpanelLabelF2", xmFormWidgetClass,
				      botform, arg, ac);
  ac = 0;  

  t = XmStringCreateSimple ("Integrated");
  XtSetArg (arg[ac], XmNtopAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNbottomAttachment, XmATTACH_POSITION); ac++;
  XtSetArg (arg[ac], XmNbottomPosition, 49); ac++;
  XtSetArg (arg[ac], XmNleftAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNleftOffset, 5); ac++;
  XtSetArg (arg[ac], XmNrightAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNlabelString, t); ac++;
  XtSetArg (arg[ac], XmNalignment, XmALIGNMENT_BEGINNING); ac++;
  Widget ilabel = XtCreateManagedWidget ("rcDpanelIlabel",
					 xmLabelWidgetClass,
					 labelform2, arg, ac);
  ac = 0;
  XmStringFree (t);

  t = XmStringCreateSimple ("Differential");
  XtSetArg (arg[ac], XmNtopAttachment, XmATTACH_POSITION); ac++;
  XtSetArg (arg[ac], XmNtopPosition, 51); ac++;
  XtSetArg (arg[ac], XmNbottomAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNleftAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNleftOffset, 5); ac++;
  XtSetArg (arg[ac], XmNrightAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNlabelString, t); ac++;
  XtSetArg (arg[ac], XmNalignment, XmALIGNMENT_BEGINNING); ac++;
  Widget dlabel = XtCreateManagedWidget ("rcDpaneldlabel",
					 xmLabelWidgetClass,
					 labelform2, arg, ac);
  ac = 0;
  XmStringFree (t);


  /* !!!??? */
  // create a form widget holding all displays
  XtSetArg (arg[ac], XmNtopAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNleftAttachment, XmATTACH_POSITION); ac++;
  XtSetArg (arg[ac], XmNleftPosition, 30); ac++;
  XtSetArg (arg[ac], XmNrightAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNbottomAttachment, XmATTACH_FORM); ac++;
  Widget dataform3 = XtCreateWidget ("dpanelSubF3", xmFormWidgetClass,
				     botform, arg, ac);
  ac = 0;  


  XtSetArg (arg[ac], XmNtopAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNbottomAttachment, XmATTACH_POSITION); ac++;
  XtSetArg (arg[ac], XmNbottomPosition, 49); ac++;
  XtSetArg (arg[ac], XmNleftAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNrightAttachment, XmATTACH_POSITION); ac++;
  XtSetArg (arg[ac], XmNrightPosition, 49); ac++;
  XtSetArg (arg[ac], XmNshadowType, XmSHADOW_ETCHED_OUT); ac++;
  Widget iEvRateFrame = XtCreateManagedWidget ("iEvRate", xmFrameWidgetClass,
					       dataform3, arg, ac);
  ac = 0;


  /* Sergey (on Jie Chen advice 2-nov-2007): to specify the font for the
  integrated/differential event/data rate it must be described in
  'main/runcontrol.cc' with 'iEvRateG' etc tags; following lines was added
  to the 'main/runcontrol.cc':

  "runcontrol*.iEvRateG.fontList:                -adobe-helvetica-medium-r-*-*-20-*-*-*-*-*-*-*",
  "runcontrol*.iDataRateG.fontList:              -adobe-helvetica-medium-r-*-*-20-*-*-*-*-*-*-*",
  "runcontrol*.dEvRateG.fontList:                -adobe-helvetica-medium-r-*-*-20-*-*-*-*-*-*-*",
  "runcontrol*.dDataRateG.fontList:              -adobe-helvetica-medium-r-*-*-20-*-*-*-*-*-*-*",

  and font if BIG now !!! The size of window where numbers are displayed is defined by
  choosing font, there is no explicit window size settings

  DID NOT HELPED !!!
  */
  t = XmStringCreateSimple ("   0    ");
  XtSetArg (arg[ac], XmNlabelString, t); ac++;
  //XtSetArg (arg[ac], XmNrecomputeSize, FALSE); ac++;
  XtSetArg (arg[ac], XmNshadowType, XmSHADOW_ETCHED_OUT); ac++;
  iEvRate_ = XtCreateManagedWidget ("iEvRateG", xmLabelWidgetClass,
				    iEvRateFrame, arg, ac);
  ac = 0;
  XmStringFree (t);

  XtSetArg (arg[ac], XmNtopAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNbottomAttachment, XmATTACH_POSITION); ac++;
  XtSetArg (arg[ac], XmNbottomPosition, 49); ac++;
  XtSetArg (arg[ac], XmNleftAttachment, XmATTACH_POSITION); ac++;
  XtSetArg (arg[ac], XmNleftPosition, 51); ac++;
  XtSetArg (arg[ac], XmNrightAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNrightOffset, 5); ac++;
  Widget iDataRateFrame = XtCreateManagedWidget ("iDataRate", 
						 xmFrameWidgetClass,
						 dataform3, arg, ac);
  ac = 0;

  t = XmStringCreateSimple ("   0.0   ");
  XtSetArg (arg[ac], XmNlabelString, t); ac++;
  //XtSetArg (arg[ac], XmNrecomputeSize, FALSE); ac++;
  iDataRate_ = XtCreateManagedWidget ("iDataRateG", xmLabelWidgetClass,
				      iDataRateFrame, arg, ac);
  ac = 0;
  XmStringFree (t);

  
  XtSetArg (arg[ac], XmNtopAttachment, XmATTACH_POSITION); ac++;
  XtSetArg (arg[ac], XmNtopPosition, 51); ac++;
  XtSetArg (arg[ac], XmNbottomAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNleftAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNrightAttachment, XmATTACH_POSITION); ac++;
  XtSetArg (arg[ac], XmNrightPosition, 49); ac++;
  XtSetArg (arg[ac], XmNshadowType, XmSHADOW_ETCHED_OUT); ac++;
  Widget dEvRateFrame = XtCreateManagedWidget ("dEvRate", xmFrameWidgetClass,
					       dataform3, arg, ac);
  ac = 0;

  t = XmStringCreateSimple ("   0    ");
  XtSetArg (arg[ac], XmNlabelString, t); ac++;
  //XtSetArg (arg[ac], XmNrecomputeSize, FALSE); ac++;
  dEvRate_ = XtCreateManagedWidget ("dEvRateG", xmLabelWidgetClass,
				    dEvRateFrame, arg, ac);
  ac = 0;
  XmStringFree (t);

  XtSetArg (arg[ac], XmNtopAttachment, XmATTACH_POSITION); ac++;
  XtSetArg (arg[ac], XmNtopPosition, 51); ac++;
  XtSetArg (arg[ac], XmNbottomAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNleftAttachment, XmATTACH_POSITION); ac++;
  XtSetArg (arg[ac], XmNleftPosition, 51); ac++;
  XtSetArg (arg[ac], XmNrightAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNrightOffset, 5); ac++;

  Widget dDataRateFrame = XtCreateManagedWidget ("dDataRate", 
						 xmFrameWidgetClass,
						 dataform3, arg, ac);
  ac = 0;

  t = XmStringCreateSimple ("   0.0   ");
  XtSetArg (arg[ac], XmNlabelString, t); ac++;
  //XtSetArg (arg[ac], XmNrecomputeSize, FALSE); ac++;
  XtSetArg (arg[ac], XmNshadowType, XmSHADOW_ETCHED_OUT); ac++;
  dDataRate_ = XtCreateManagedWidget ("dDataRateG", xmLabelWidgetClass,
				      dDataRateFrame, arg, ac);
  ac = 0;
  XmStringFree (t);


  /* sergey: end of statistics gui */







  // install destroy handler
  installDestroyHandler ();

  // manage all
  XtManageChild (subform1);
  XtManageChild (labelform2);
  XtManageChild (dataform3);
  XtManageChild (botform);
  XtManageChild (form);
  XtManageChild (_w);
}

  
void
rcRunDInfoPanel::manage (void)
{
  XcodaUi::manage ();
  compOption_->manage ();
  // start monitoring on time and start time
  // get network handler first
  rcClient& client = netHandler_.clientHandler ();

  if (client.monitorOnCallback (client.exptname(), "timeBin",
		(rcCallback)&(rcRunDInfoPanel::timeCallback),
		(void *)this) != CODA_SUCCESS) 
    fprintf (stderr, "Cannot register monitor on timeBin\n");

  if (client.monitorOnCallback (client.exptname(), "startTimeBin",
		(rcCallback)&(rcRunDInfoPanel::startTimeCallback),
		(void *)this) != CODA_SUCCESS) 
    fprintf (stderr, "Cannot register monitor on startTimeBin\n");

  if (client.monitorOnCallback (client.exptname(), "endTimeBin",
		(rcCallback)&(rcRunDInfoPanel::endTimeCallback),
		(void *)this) != CODA_SUCCESS) 
    fprintf (stderr, "Cannot register monitor on endTimeBin\n");
  if (client.monitorOnCallback (client.exptname (), "updateInterval",
		(rcCallback)&(rcRunDInfoPanel::updateCallback),
		(void *)this) != CODA_SUCCESS) {
    fprintf (stderr, "Cannot register monitor on %s %s\n", 
	     client.exptname(), "updateInterval");
  }
}

void
rcRunDInfoPanel::unmanage (void)
{
  compOption_->unmanage ();  
  XcodaUi::unmanage ();
}

void
rcRunDInfoPanel::config (int state)
{

  if (state >= DA_PAUSED) {
    if (startTime_ != 0 && endTime_ == 0) {
      if (!monitorOn_) {
	ratePanel_->manage ();
	//evRateDisp_->reset ();
	startMonitoringInfo (compOption_->currentComponent() );
      }
    }
    else {
      if (monitorOn_)
	stopMonitoringInfo (compOption_->currentComponent ());
    }
  }
  else {
    ratePanel_->unmanage ();
    //if (ratePanel_ && ratePanel_->isMapped ())
    //ratePanel_->cleanDisplay ();
  }
}

void
rcRunDInfoPanel::endDataTaking (void)
{
  if (monitorOn_) 
    stopMonitoringInfo (compOption_->currentComponent ());
}

void
rcRunDInfoPanel::startMonitoringInfo (char* compname)
{
  int err = 0;
  // get network handler first
  rcClient& client = netHandler_.clientHandler ();

#ifdef _CODA_DEBUG
  printf("start monitoring %s\n",compname);
#endif

  if (client.monitorOnCallback (compname, DYN_ATTR0,
		(rcCallback)&(rcRunDInfoPanel::attr0Callback),
		(void *)this) != CODA_SUCCESS) {
    fprintf (stderr, "Cannot register monitor on %s %s\n", 
	     compname, DYN_ATTR0);
    err = 1;
  }
  if (client.monitorOnCallback (compname, DYN_ATTR1,
		(rcCallback)&(rcRunDInfoPanel::attr1Callback),
		(void *)this) != CODA_SUCCESS) {
    fprintf (stderr, "Cannot register monitor on %s %s\n", 
	     compname, DYN_ATTR1);
    err = 1;
  }
  if (client.monitorOnCallback (compname, DYN_ATTR2,
		(rcCallback)&(rcRunDInfoPanel::attr2Callback),
		(void *)this) != CODA_SUCCESS) {
    fprintf (stderr, "Cannot register monitor on %s %s\n", 
	     compname, DYN_ATTR2);
    err = 1;
  }
  if (client.monitorOnCallback (compname, DYN_ATTR3,
		(rcCallback)&(rcRunDInfoPanel::attr3Callback),
		(void *)this) != CODA_SUCCESS) {
    fprintf (stderr, "Cannot register monitor on %s %s\n", 
	     compname, DYN_ATTR3);
    err = 1;
  }
  if (client.monitorOnCallback (compname, DYN_ATTR4,
		(rcCallback)&(rcRunDInfoPanel::attr4Callback),
		(void *)this) != CODA_SUCCESS) {
    fprintf (stderr, "Cannot register monitor on %s %s\n", 
	     compname, DYN_ATTR4);
    err = 1;
  }


  /*sergey*/
  if (client.monitorOnCallback (compname, DYN_ATTR5,
		(rcCallback)&(rcRunDInfoPanel::attr5Callback),
		(void *)this) != CODA_SUCCESS) {
    fprintf (stderr, "Cannot register monitor on %s %s\n", 
	     compname, DYN_ATTR5);
    err = 1;
  }


  if (!err) {
    monitorOn_ = 1;
    // reset counters to zero
    evc_ = drc_ = 0;
    // display event rate
    ratePanel_->cleanDisplay ();
    ratePanel_->manage ();
	//printf("reset display!!!\n");
	}
}

void
rcRunDInfoPanel::stopMonitoringInfo (char* compname)
{
  rcClient& client = netHandler_.clientHandler ();
  
#ifdef _CODA_DEBUG
  printf("try to stop monitoring %s\n",compname);
#endif
  if (monitorOn_) {
#ifdef _CODA_DEBUG
    printf("stop monitoring %s\n",compname);
#endif

    if (client.monitorOffCallback (compname, DYN_ATTR0,
				   (rcCallback)&(rcRunDInfoPanel::attr0Callback),
				   (void *)this,
				   (rcCallback)&(rcRunDInfoPanel::offCallback),
				   (void *)this) != CODA_SUCCESS)
      fprintf (stderr, "Cannot unregister monitor on callback on %s %s\n", 
	       compname,DYN_ATTR0);
    if (client.monitorOffCallback (compname, DYN_ATTR1,
				   (rcCallback)&(rcRunDInfoPanel::attr1Callback),
				   (void *)this,
		   (rcCallback)&(rcRunDInfoPanel::offCallback),
				   (void *)this) != CODA_SUCCESS)
      fprintf (stderr, "Cannot unregister monitor on callback on %s %s\n", 
	       compname,DYN_ATTR1);
    if (client.monitorOffCallback (compname, DYN_ATTR2,
				   (rcCallback)&(rcRunDInfoPanel::attr2Callback),
				   (void *)this,
		   (rcCallback)&(rcRunDInfoPanel::offCallback),
				   (void *)this) != CODA_SUCCESS)
      fprintf (stderr, "Cannot unregister monitor on callback on %s %s\n", 
	       compname,DYN_ATTR1);
    if (client.monitorOffCallback (compname, DYN_ATTR3,
				   (rcCallback)&(rcRunDInfoPanel::attr3Callback),
				   (void *)this,
				   (rcCallback)&(rcRunDInfoPanel::offCallback),
				   (void *)this) != CODA_SUCCESS)
      fprintf (stderr, "Cannot unregister monitor on callback on %s %s\n", 
	       compname,DYN_ATTR3);
    if (client.monitorOffCallback (compname, DYN_ATTR4,
				   (rcCallback)&(rcRunDInfoPanel::attr4Callback),
				   (void *)this,
				   (rcCallback)&(rcRunDInfoPanel::offCallback),
				   (void *)this) != CODA_SUCCESS)
      fprintf (stderr, "Cannot unregister monitor on callback on %s %s\n", 
	       compname,DYN_ATTR4);



    /*sergey*/
    if (client.monitorOffCallback (compname, DYN_ATTR5,
				   (rcCallback)&(rcRunDInfoPanel::attr5Callback),
				   (void *)this,
				   (rcCallback)&(rcRunDInfoPanel::offCallback),
				   (void *)this) != CODA_SUCCESS)
      fprintf (stderr, "Cannot unregister monitor on callback on %s %s\n", 
	       compname,DYN_ATTR5);



    monitorOn_ = 0;
  }
}

void
rcRunDInfoPanel::attr2Callback (int status, void* arg, daqNetData* data)
{
  rcRunDInfoPanel *obj = (rcRunDInfoPanel *)arg;

  if(obj->monitorOn_)
  {
    if(status == CODA_SUCCESS)
    {
      int tevn = (int)(*data);

      //setCompState(data->name(),tevn);
      {
        char cmd[100];
        sprintf(cmd,"s:%d %s",tevn,data->name());
printf("CEDIT 3\n");
#ifdef USE_CREG
        coda_send(XtDisplay(obj->baseWidget()),"CEDIT",cmd);
#endif
      }
    }
  }
}

void
rcRunDInfoPanel::attr1Callback (int status, void* arg, daqNetData* data)
{
  rcRunDInfoPanel *obj = (rcRunDInfoPanel *)arg;

  if (obj->monitorOn_) {
    if (status == CODA_SUCCESS) {
      Arg arg[10];
      int ac = 0;

      XmString t = XmStringCreateSimple ((char *)(*data));
      XtSetArg (arg[ac], XmNlabelString, t); ac++;
      XtSetValues (obj->eventNumber_, arg, ac);
      ac = 0;
      XmStringFree (t);

      int tevn = (int)(*data);
      obj->numEvents_ = (unsigned long)tevn;
      
    }
  }
}

void
rcRunDInfoPanel::attr3Callback (int status, void* arg, daqNetData* data)
{
	rcRunDInfoPanel *obj = (rcRunDInfoPanel *)arg;

	if (obj->monitorOn_) {
		if (status == CODA_SUCCESS) {

			// update to window
			Arg arg[10];
			int ac = 0;
			XmString t;
			char tempi[50], tempd[50];

			// integrated event rate
			sprintf (tempi, "%-20.4lf", (float) (*data));

			t = XmStringCreateSimple (tempi);
			XtSetArg (arg[ac], XmNlabelString, t); ac++;
			XtSetValues (obj->dEvRate_, arg, ac);
			ac = 0;
			XmStringFree (t);  

			// update rate graph display if it is mapped
			//if (ratePanel_ && ratePanel_->isMapped ())
                        obj->updateEventRate();
			obj->ratePanel_->addEvRateData ((double) (*data));

		}
	}
}

void
rcRunDInfoPanel::attr4Callback (int status, void* arg, daqNetData* data)
{
  rcRunDInfoPanel *obj = (rcRunDInfoPanel *)arg;

	if (obj->monitorOn_) {
		if (status == CODA_SUCCESS) {
			// update to window
			Arg arg[10];
			int ac = 0;
			XmString t;
			char temp[50];

			// integrated event rate
			sprintf (temp, "%20.4lf", ((float) (*data))/1000.0);
			t = XmStringCreateSimple (temp);
			XtSetArg (arg[ac], XmNlabelString, t); ac++;
			XtSetValues (obj->dDataRate_, arg, ac);
			ac = 0;
			XmStringFree (t); 

			// update rate graph display if it is mapped
			//if (ratePanel_ && ratePanel_->isMapped ())
                        obj->updateDataRate();
			obj->ratePanel_->addDataRateData (((float) (*data))/1000.0);

		}
	}
}



/*sergey*/
void
rcRunDInfoPanel::attr5Callback (int status, void* arg, daqNetData* data)
{
  rcRunDInfoPanel *obj = (rcRunDInfoPanel *)arg;

	if (obj->monitorOn_) {
		if (status == CODA_SUCCESS) {
			// update to window
			Arg arg[10];
			int ac = 0;

			/*
printf("attr5Callback: got livetime=%d\n",(*data));
			*/

			obj->ratePanel_->addLivetimeData ((float)(*data));
		}
	}
}




void
rcRunDInfoPanel::attr0Callback (int status, void* arg, daqNetData* data)
{
  rcRunDInfoPanel *obj = (rcRunDInfoPanel *)arg;

  if (obj->monitorOn_) {    
    if (status == CODA_SUCCESS) {
      int temp = (int)(*data);
      obj->numKbytes_ = ((unsigned long)temp)/256.0;
    }
  }
}

void
rcRunDInfoPanel::updateCallback (int status, void* arg, daqNetData* data)
{
  rcRunDInfoPanel *obj = (rcRunDInfoPanel *)arg;

  if (status == CODA_SUCCESS) {
    printf("rcRunDInfoPanel::updateCallback: update interval = %d\n",
      (int) (*data));

	/*
    *data = 5;
    printf("sergey: set update interval = %d\n",(int) (*data));
	*/

    obj->ratePanel_->setUpdateRate((int)(*data));
    if (obj->monitorOn_) {
      obj->stopMonitoringInfo (obj->compOption_->currentComponent ());
      obj->startMonitoringInfo (obj->compOption_->currentComponent() );
    }
  }
 

}

void
rcRunDInfoPanel::timeCallback (int status, void* arg, daqNetData* data)
{
  rcRunDInfoPanel *obj = (rcRunDInfoPanel *)arg;

  if (status == CODA_SUCCESS) 
    obj->time_ = (long)(*data);
}

void
rcRunDInfoPanel::startTimeCallback (int status, void* arg, daqNetData* data)
{
  rcRunDInfoPanel *obj = (rcRunDInfoPanel *)arg;

  if (status == CODA_SUCCESS) {
    obj->startTime_ = (long)(*data);
  }
}

void
rcRunDInfoPanel::endTimeCallback (int status, void* arg, daqNetData* data)
{
  rcRunDInfoPanel *obj = (rcRunDInfoPanel *)arg;

  if (status == CODA_SUCCESS) {
    obj->endTime_ = (long)(*data);
  }
}

void
rcRunDInfoPanel::offCallback (int status, void* arg, daqNetData* data)
{
  if (status != CODA_SUCCESS)
    printf ("monitor off failed\n");
}

// integrated event rate counter window (not histogram !)
void
rcRunDInfoPanel::updateEventRate (void)
{
  double irate = 0.0;
  if (evc_ < RC_CACHE_SIZE) {
    pastTime_[evc_]= time_;
    pastEvn_[evc_++] = numEvents_;
    if (time_ > startTime_) {
      irate = numEvents_/(double)(time_- startTime_);
    }
  }
  else {
    irate = numEvents_/(double)(time_- startTime_);
    // remember old values
    pastEvn_[evc_ - RC_CACHE_SIZE] = numEvents_;
    pastTime_[evc_ - RC_CACHE_SIZE] = time_;
    evc_++;
    if (evc_ >= 2*RC_CACHE_SIZE)
      evc_ = RC_CACHE_SIZE;
  }
  
  // update to window
  Arg arg[10];
  int ac = 0;
  XmString t;
  char tempi[50], tempd[50];

  // integrated event rate
  sprintf (tempi, "%-20.4lf", irate);
  t = XmStringCreateSimple (tempi);
  XtSetArg (arg[ac], XmNlabelString, t); ac++;
  XtSetValues (iEvRate_, arg, ac);
  ac = 0; 
  XmStringFree (t);

}

// integrated data rate counter window (not histogram !)
void
rcRunDInfoPanel::updateDataRate (void)
{
  double irate = 0.0;

  if (drc_ < RC_CACHE_SIZE) {
    pastTimeB_[drc_]= time_;
    pastDataB_[drc_] = numKbytes_;
    drc_ ++;
    if (time_ > startTime_) {
      irate = numKbytes_/(double)(time_- startTime_);
    }
  }
  else {
    irate = numKbytes_/(double)(time_- startTime_);
      (double)(time_ - pastTimeB_[drc_ - RC_CACHE_SIZE]);
    // remember old values
    pastDataB_[drc_ - RC_CACHE_SIZE] = numKbytes_;
    pastTimeB_[drc_ - RC_CACHE_SIZE] = time_;
    drc_++;
    if (drc_ >= 2*RC_CACHE_SIZE)
      drc_ = RC_CACHE_SIZE;
  }
  
  // update to window
  Arg arg[10];
  int ac = 0;
  XmString t;
  char temp[50];

  // integrated event rate
  sprintf (temp, "%20.4lf", irate);
  t = XmStringCreateSimple (temp);
  XtSetArg (arg[ac], XmNlabelString, t); ac++;
  XtSetValues (iDataRate_, arg, ac);
  ac = 0;
  XmStringFree (t);

}

void
rcRunDInfoPanel::zoomOnEventInfo (void)
{

}

void
rcRunDInfoPanel::popupRateDisplay (rcMenuWindow *menW)
{
  if (!ratePanel_) {

    Widget stattab = menW->createTabFrame("Statistics",0);
  
    ratePanel_ = new rcRateDisplay (stattab, netHandler_,statusPanel_,"rcRateDisplay",
				    "Event/Data Rate Display",
				    60);
    ratePanel_->init (menW);
  }

  ratePanel_->popup ();
}

				    
  

