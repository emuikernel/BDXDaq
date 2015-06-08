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
//      Implementation of rcRunCInfoPanel Class
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: rcRunCInfoPanel.cc,v $
//   Revision 1.8  1998/09/17 19:18:34  rwm
//   Only print if _CODA_DEBUG is set.
//
//   Revision 1.7  1998/05/28 17:47:04  heyes
//   new GUI look
//
//   Revision 1.6  1998/04/08 18:31:29  heyes
//   new look and feel GUI
//
//   Revision 1.5  1997/12/04 14:05:50  heyes
//   stuff for Dieter, eye candy!
//
//   Revision 1.4  1997/08/21 19:28:45  heyes
//   fix borders and color problems
//
//   Revision 1.3  1997/06/16 12:26:49  heyes
//   add dbedit and so on
//
//   Revision 1.2  1997/06/06 18:51:29  heyes
//   new RC
//
//   Revision 1.1.1.1  1996/10/11 13:39:25  chen
//   run control source
//
//
#include <Xm/Label.h>
#include <Xm/LabelG.h>
#include <Xm/Frame.h>
#include <Xm/Form.h>
#include <Xm/SeparatoG.h>
#include <Xm/RowColumn.h>

#include "rcRunCInfoPanel.h"

rcRunCInfoPanel::rcRunCInfoPanel (Widget parent, char* name,
				  rcClientHandler& handler)
:XcodaUi (name), parent_ (parent), netHandler_ (handler)
{
#ifdef _TRACE_OBJECTS
  printf ("              Create rcRunCInfoPanel Class Object\n");
#endif
  // empty
}

rcRunCInfoPanel::~rcRunCInfoPanel (void)
{
#ifdef _TRACE_OBJECTS
  printf ("              Delete rcRunCInfoPanel Class Object\n");
#endif
  // empty
  // all widgets will be destroyed by Xt mechanism
}

void
rcRunCInfoPanel::init (void)
{
  Arg arg[20];
  Widget children[20];
  int ac = 0;
  XtSetArg (arg[ac], XmNshadowThickness, 2); ac++;
  XtSetArg (arg[ac], XmNshadowType, XmSHADOW_ETCHED_IN); ac++;
  Widget frame1 =_w = XtCreateWidget (_name, xmFrameWidgetClass, parent_, arg, ac);

  ac = 0;
  //XtSetArg (arg[ac], XmNfractionBase, 1); ac++;
  Widget form = XtCreateWidget ("runcinfo", xmFormWidgetClass,
				frame1, arg, ac);
  // create frame title
  ac = 0;
  XmString t = XmStringCreateSimple ("Static parameters");
  XtSetArg (arg[ac], XmNlabelString, t); ac++;
  XtSetArg (arg[ac], XmNchildType, XmFRAME_TITLE_CHILD); ac++;
  Widget rilabel = XtCreateWidget ("runinfoLabel",
					 xmLabelWidgetClass,
					 frame1, arg, ac);
  XmStringFree (t);

  ac = 0;
  children[ac++] = form;
  children[ac++] = rilabel;
  XtManageChildren(children, ac);

  // create 1st row of information
  ac = 0;
  XtSetArg (arg[ac], XmNfractionBase, 4); ac++;
  XtSetArg (arg[ac], XmNtopAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNleftAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNrightAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNbottomAttachment, XmATTACH_FORM); ac++;
  //XtSetArg (arg[ac], XmNbottomPosition, 1); ac++;
  Widget sform = XtCreateWidget ("cinfoSubForm", xmFormWidgetClass,
				 form, arg, ac );

  // create experiment name 

  ac = 0;
  XtSetArg (arg[ac], XmNshadowThickness, 2); ac++;
  XtSetArg (arg[ac], XmNtopAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNbottomAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNleftAttachment, XmATTACH_POSITION); ac++;
  XtSetArg (arg[ac], XmNleftPosition, 0); ac++;
  XtSetArg (arg[ac], XmNrightAttachment, XmATTACH_POSITION); ac++;
  XtSetArg (arg[ac], XmNrightPosition, 1); ac++;
  XtSetArg (arg[ac], XmNshadowType, XmSHADOW_ETCHED_OUT); ac++;
  Widget frame2 = XmCreateFrame (sform, "frame2", arg, ac );
  ac = 0;
  t = XmStringCreateSimple ("Database");
  XtSetArg (arg[ac], XmNlabelString, t); ac++;
  XtSetArg (arg[ac], XmNshadowType, XmSHADOW_OUT); ac++;
  XtSetArg (arg[ac], XmNchildType, XmFRAME_TITLE_CHILD); ac++;
  Widget dlabel = XtCreateManagedWidget ("databaseLabel",
					 xmLabelWidgetClass,
					 frame2, arg, ac);
  XmStringFree (t);

  ac = 0;
  t = XmStringCreateSimple ("");
  XtSetArg (arg[ac], XmNlabelString, t); ac++;
  XtSetArg (arg[ac], XmNheight, 20); ac++;
  XtSetArg (arg[ac], XmNrecomputeSize, FALSE); ac++;
  XtSetArg (arg[ac], XmNalignment, XmALIGNMENT_CENTER); ac++;
  database_ = XtCreateManagedWidget ("database",
				     xmLabelWidgetClass,
				     frame2, arg, ac);
  XmStringFree (t);
  
  // Create Session title
  ac = 0;
  XtSetArg (arg[ac], XmNshadowThickness, 2); ac++;
  XtSetArg (arg[ac], XmNshadowType, XmSHADOW_ETCHED_OUT); ac++;
  XtSetArg (arg[ac], XmNtopAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNbottomAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNleftAttachment, XmATTACH_POSITION); ac++;
  XtSetArg (arg[ac], XmNleftPosition, 1); ac++;
  XtSetArg (arg[ac], XmNrightAttachment, XmATTACH_POSITION); ac++;
  XtSetArg (arg[ac], XmNrightPosition, 2); ac++;
  Widget frame3 = XmCreateFrame (sform, "frame3", arg, ac );
  ac = 0;
#if defined (_CODA_2_0_T) || defined (_CODA_2_0)
  t = XmStringCreateSimple ("Session");
#else
  t = XmStringCreateSimple ("Name");
#endif
  XtSetArg (arg[ac], XmNlabelString, t); ac++;
  XtSetArg (arg[ac], XmNchildType, XmFRAME_TITLE_CHILD); ac++;
  Widget enlabel = XtCreateManagedWidget ("expnameLabel",
					  xmLabelWidgetClass,
					  frame3, arg, ac);
  XmStringFree (t);

  // Session name
  ac = 0;
  t = XmStringCreateSimple ("");
  XtSetArg (arg[ac], XmNrecomputeSize, FALSE); ac++;
  XtSetArg (arg[ac], XmNlabelString, t); ac++;
  exptname_ = XtCreateManagedWidget ("exptname",
				     xmLabelWidgetClass,
				     frame3, arg, ac);
  XmStringFree (t);  

  // create label for experiment run type
  ac = 0;
  XtSetArg (arg[ac], XmNtopAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNbottomAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNleftAttachment, XmATTACH_POSITION); ac++;
  XtSetArg (arg[ac], XmNleftPosition, 2); ac++;
  XtSetArg (arg[ac], XmNrightAttachment, XmATTACH_POSITION); ac++;
  XtSetArg (arg[ac], XmNrightPosition, 3); ac++;
  XtSetArg (arg[ac], XmNshadowThickness, 2); ac++;
  XtSetArg (arg[ac], XmNshadowType, XmSHADOW_ETCHED_OUT); ac++;
  Widget frame4 = XmCreateFrame (sform, "frame4", arg, ac );
  ac = 0;
  t = XmStringCreateSimple ("Configuration");
  XtSetArg (arg[ac], XmNlabelString, t); ac++;
  XtSetArg (arg[ac], XmNchildType, XmFRAME_TITLE_CHILD); ac++;
  Widget rtlabel = XtCreateManagedWidget ("runTypeLabel",
					  xmLabelWidgetClass,
					  frame4, arg, ac);

  XmStringFree (t);
  // Run name
  ac = 0;
  t = XmStringCreateSimple ("");
  XtSetArg (arg[ac], XmNlabelString, t); ac++;
  XtSetArg (arg[ac], XmNrecomputeSize, FALSE); ac++;
  runType_ = XtCreateManagedWidget ("runType",
				    xmLabelWidgetClass,
				    frame4, arg, ac);
  XmStringFree (t);

  // create run control server host info
  ac = 0;
  XtSetArg (arg[ac], XmNtopAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNbottomAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNleftAttachment, XmATTACH_POSITION); ac++;
  XtSetArg (arg[ac], XmNleftPosition, 3); ac++;
  XtSetArg (arg[ac], XmNrightAttachment, XmATTACH_POSITION); ac++;
  XtSetArg (arg[ac], XmNrightPosition, 4); ac++;
  XtSetArg (arg[ac], XmNshadowThickness, 2); ac++;
  XtSetArg (arg[ac], XmNshadowType, XmSHADOW_ETCHED_OUT); ac++;
  Widget frame5 = XmCreateFrame (sform, "frame5", arg, ac );
  ac = 0;
  t = XmStringCreateSimple ("rcServer");
  XtSetArg (arg[ac], XmNlabelString, t); ac++;
  XtSetArg (arg[ac], XmNchildType, XmFRAME_TITLE_CHILD); ac++;
  Widget hlabel = XtCreateManagedWidget ("hostLabel",
					 xmLabelWidgetClass,
					 frame5, arg, ac);
  XmStringFree (t);
  
  ac = 0;
  t = XmStringCreateSimple ("");
  XtSetArg (arg[ac], XmNlabelString, t); ac++;
  XtSetArg (arg[ac], XmNrecomputeSize, FALSE); ac++;
  hostname_ = XtCreateManagedWidget ("hostname",
				     xmLabelWidgetClass,
				     frame5, arg, ac);
  XmStringFree (t);

  // create 2nd row to handle current time
  //ac = 0;
  //XtSetArg (arg[ac], XmNleftAttachment, XmATTACH_FORM); ac++;
  //XtSetArg (arg[ac], XmNrightAttachment, XmATTACH_FORM); ac++;
  //XtSetArg (arg[ac], XmNtopAttachment, XmATTACH_POSITION); ac++;
  //XtSetArg (arg[ac], XmNtopPosition, 1); ac++;
  //XtSetArg (arg[ac], XmNtopWidget, sform); ac++;
  //XtSetArg (arg[ac], XmNbottomAttachment, XmATTACH_FORM); ac++;
  //XtSetArg (arg[ac], XmNshadowThickness, 2); ac++;
  //XtSetArg (arg[ac], XmNshadowType, XmSHADOW_ETCHED_IN); ac++;
  //Widget frame6 = XmCreateFrame ( form, "frame6", arg, ac );

  //ac = 0;
  //t = XmStringCreateSimple ("Current time");
  //XtSetArg (arg[ac], XmNlabelString, t); ac++;
  //XtSetArg (arg[ac], XmNchildType, XmFRAME_TITLE_CHILD); ac++;
  //Widget timetlabel = XtCreateWidget ("runTimeLabel",
  //					 xmLabelWidgetClass,
  //					 frame6, arg, ac);
  //XmStringFree (t);
  // create time stamp
  //ac = 0;
  //t = XmStringCreateSimple ("   Thu Feb 8 13:02:26 1996   ");
  //XtSetArg (arg[ac], XmNlabelString, t); ac++;
  //XtSetArg (arg[ac], XmNrecomputeSize, FALSE); ac++;
  //XtSetArg (arg[ac], XmNborderWidth, 4); ac++;
  //time_ = XtCreateManagedWidget ("timeG",
  //				 xmLabelWidgetClass,
  //  			         frame6, arg, ac);
  //XmStringFree (t);

  ac = 0;

  // install destroy handler
  installDestroyHandler ();
  
  // manage all widget
  XtManageChild (sform);

  XtManageChild (form);
  XtManageChild (frame1);
  XtManageChild (frame2);
  XtManageChild (frame3);
  XtManageChild (frame4);
  XtManageChild (frame5);
  // XtManageChild (frame6);
  // XtManageChild (timetlabel);

  XtManageChild (_w);
}

void
rcRunCInfoPanel::manage (void)
{
  XcodaUi::manage ();
  // add all monitor on callbacks
  rcClient& client = netHandler_.clientHandler ();
  //if (client.monitorOnCallback (client.exptname (), "time",
  //				(rcCallback)&(rcRunCInfoPanel::timeCallback),
  //				(void *)this) != CODA_SUCCESS) 
  // fprintf (stderr, "Cannot register monitor on time callback\n");
  if (client.monitorOnCallback (client.exptname (), "exptName",
				(rcCallback)&(rcRunCInfoPanel::exptnameCallback),
				(void *)this) != CODA_SUCCESS) {
#ifdef _CODA_DEBUG
    fprintf (stderr, "Cannot register monitor on exptname callback\n");
#endif
  }

  if (client.monitorOnCallback (client.exptname (), "runType",
				(rcCallback)&(rcRunCInfoPanel::runtypeCallback),
				(void *)this) != CODA_SUCCESS) {
#ifdef _CODA_DEBUG
    fprintf (stderr, "Cannot register monitor on exptid callback\n");
#endif
  }

  if (client.monitorOnCallback (client.exptname (), "hostName",
				(rcCallback)&(rcRunCInfoPanel::hostnameCallback),
				(void *)this) != CODA_SUCCESS) {
#ifdef _CODA_DEBUG
    fprintf (stderr, "Cannot register monitor on hostname callback\n");
#endif
  }
#if defined (_CODA_2_0_T) || defined (_CODA_2_0)
  if (client.monitorOnCallback (client.exptname (), "database",
				(rcCallback)&(rcRunCInfoPanel::databaseCallback),
				(void *)this) != CODA_SUCCESS) {
#ifdef _CODA_DEBUG
    fprintf (stderr, "Cannot register monitor on database callback\n");
#endif
  }
#else
  if (client.monitorOnCallback (client.exptname (), "exptId",
				(rcCallback)&(rcRunCInfoPanel::exptidCallback),
				(void *)this) != CODA_SUCCESS) {
#ifdef _CODA_DEBUG
    fprintf (stderr, "Cannot register monitor on exptid callback\n");
#endif
  }
#endif
}

void
rcRunCInfoPanel::unmanage (void)
{
  // remove all callbacks
  rcClient& client = netHandler_.clientHandler ();
  if (client.monitorOffCallback (client.exptname (), "time",
				 (rcCallback)&(rcRunCInfoPanel::timeCallback),
				 (void *)this,
				 (rcCallback)&(rcRunCInfoPanel::offCallback),
				 (void *)this) != CODA_SUCCESS) {
#ifdef _CODA_DEBUG
    fprintf (stderr, "Cannot register monitor off time callback\n"); 
#endif
  }
  if (client.monitorOffCallback (client.exptname (), "exptName",
				 (rcCallback)&(rcRunCInfoPanel::exptnameCallback),
				 (void *)this,
				 (rcCallback)&(rcRunCInfoPanel::offCallback),
				 (void *)this) != CODA_SUCCESS) {
#ifdef _CODA_DEBUG
    fprintf (stderr, "Cannot register monitor off exptname callback\n"); 
#endif
  }
  if (client.monitorOffCallback (client.exptname (), "runType",
				 (rcCallback)&(rcRunCInfoPanel::runtypeCallback),
				 (void *)this,
				 (rcCallback)&(rcRunCInfoPanel::offCallback),
				 (void *)this) != CODA_SUCCESS) {
#ifdef _CODA_DEBUG
    fprintf (stderr, "Cannot register monitor off runType callback\n"); 
#endif
  }
  if (client.monitorOffCallback (client.exptname (), "hostName",
				 (rcCallback)&(rcRunCInfoPanel::hostnameCallback),
				 (void *)this,
				 (rcCallback)&(rcRunCInfoPanel::offCallback),
				 (void *)this) != CODA_SUCCESS) {
#ifdef _CODA_DEBUG
    fprintf (stderr, "Cannot register monitor off hostname callback\n"); 
#endif
  }
#if defined (_CODA_2_0_T) || defined (_CODA_2_0)
  if (client.monitorOffCallback (client.exptname (), "database",
				 (rcCallback)&(rcRunCInfoPanel::databaseCallback),
				 (void *)this,
				 (rcCallback)&(rcRunCInfoPanel::offCallback),
				 (void *)this) != CODA_SUCCESS) {
#else
  if (client.monitorOffCallback (client.exptname (), "exptId",
				 (rcCallback)&(rcRunCInfoPanel::exptidCallback),
				 (void *)this,
				 (rcCallback)&(rcRunCInfoPanel::offCallback),
				 (void *)this) != CODA_SUCCESS) {
#ifdef _CODA_DEBUG
    fprintf (stderr, "Cannot register monitor off exptid callback\n"); 
#endif
#endif
  }
  XcodaUi::unmanage ();
}

void
rcRunCInfoPanel::timeCallback (int status, void *arg, daqNetData* data)
{
  rcRunCInfoPanel *obj = (rcRunCInfoPanel *)arg;
  if (status == CODA_SUCCESS) {
    Arg arg[10];
    int ac = 0;
    
    XmString t = XmStringCreateSimple ((char *)(*data));
    XtSetArg (arg[ac], XmNlabelString, t); ac++;
    XtSetValues (obj->time_, arg, ac);
    ac = 0;
    XmStringFree (t);
  }
}

void
rcRunCInfoPanel::exptnameCallback (int status, void *arg, daqNetData* data)
{
  rcRunCInfoPanel *obj = (rcRunCInfoPanel *)arg;
  if (status == CODA_SUCCESS) {
    Arg arg[10];
    int ac = 0;
    
    XmString t = XmStringCreateSimple ((char *)(*data));
    XtSetArg (arg[ac], XmNlabelString, t); ac++;
    XtSetValues (obj->exptname_, arg, ac);
    ac = 0;
    XmStringFree (t);
  }
}

void
rcRunCInfoPanel::runtypeCallback (int status, void *arg, daqNetData* data)
{
  rcRunCInfoPanel *obj = (rcRunCInfoPanel *)arg;
  if (status == CODA_SUCCESS) {
    Arg arg[10];
    int ac = 0;
    
    XmString t = XmStringCreateSimple ((char *)(*data));
    XtSetArg (arg[ac], XmNlabelString, t); ac++;
    XtSetValues (obj->runType_, arg, ac);
    ac = 0;
    XmStringFree (t);
  }
}

void
rcRunCInfoPanel::hostnameCallback (int status, void *arg, daqNetData* data)
{
  rcRunCInfoPanel *obj = (rcRunCInfoPanel *)arg;
  if (status == CODA_SUCCESS) {
    Arg arg[10];
    int ac = 0;
    
    XmString t = XmStringCreateSimple ((char *)(*data));
    XtSetArg (arg[ac], XmNlabelString, t); ac++;
    XtSetValues (obj->hostname_, arg, ac);
    ac = 0;
    XmStringFree (t);
  }
}

#if defined (_CODA_2_0_T) || defined (_CODA_2_0)
void
rcRunCInfoPanel::databaseCallback (int status, void *arg, daqNetData* data)
{
  rcRunCInfoPanel *obj = (rcRunCInfoPanel *)arg;
  if (status == CODA_SUCCESS) {
    Arg arg[10];
    int ac = 0;
    
    XmString t = XmStringCreateSimple ((char *)(*data));
    XtSetArg (arg[ac], XmNlabelString, t); ac++;
    XtSetValues (obj->database_, arg, ac);
    ac = 0;
    XmStringFree (t);
  }
}
#else

void
rcRunCInfoPanel::exptidCallback (int status, void *arg, daqNetData* data)
{
  rcRunCInfoPanel *obj = (rcRunCInfoPanel *)arg;
  if (status == CODA_SUCCESS) {
    Arg arg[10];
    int ac = 0;
    
    XmString t = XmStringCreateSimple ((char *)(*data));
    XtSetArg (arg[ac], XmNlabelString, t); ac++;
    XtSetValues (obj->exptid_, arg, ac);
    ac = 0;
    XmStringFree (t);
  }
}
#endif

void
rcRunCInfoPanel::offCallback (int status, void *arg, daqNetData* data)
{
  rcRunCInfoPanel *obj = (rcRunCInfoPanel *)arg;
  if (status != CODA_SUCCESS) 
    printf ("monitor off failed\n");
}




