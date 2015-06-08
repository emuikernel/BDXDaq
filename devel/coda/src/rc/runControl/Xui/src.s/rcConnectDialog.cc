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
//      Implementation of connection dialog box
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: rcConnectDialog.cc,v $
//   Revision 1.10  1998/09/01 18:48:39  heyes
//   satisfy Randy's lust for command line options
//
//   Revision 1.9  1998/06/18 12:20:35  heyes
//   new GUI ready I think
//
//   Revision 1.8  1998/05/28 17:46:56  heyes
//   new GUI look
//
//   Revision 1.7  1997/10/15 16:08:27  heyes
//   embed dbedit, ddmon and codaedit
//
//   Revision 1.6  1997/08/01 18:38:12  heyes
//   nobody will believe this!
//
//   Revision 1.5  1997/07/30 14:32:49  heyes
//   add more xpm support
//
//   Revision 1.4  1997/07/08 15:22:43  heyes
//   add doTk to connect dialog
//
//   Revision 1.3  1997/06/16 12:26:45  heyes
//   add dbedit and so on
//
//   Revision 1.2  1997/06/06 18:51:23  heyes
//   new RC
//
//   Revision 1.1.1.1  1996/10/11 13:39:25  chen
//   run control source
//
//
#include <Xm/LabelG.h>
#include <Xm/PushBG.h>
#include <Xm/TextF.h>
#include <Xm/SeparatoG.h>
#include <Xm/Form.h>
#include <rcConnect.h>
#include <rcNetStatus.h>
#include <rcComdOption.h>
#include <rcDbaseHandler.h>
#include <rcAudioOutput.h>
#ifdef USE_CREG
#include <codaRegistry.h>
#endif
#include "rcConnectDialog.h"

rcConnectDialog::rcConnectDialog (rcConnect* connect,
				  char* name, 
				  char* title,
				  rcClientHandler& handler)
:XcodaFormDialog (connect->dialogBaseWidget(), name, title), 
 connect_ (connect), netHandler_ (handler), 
 firstTime_ (1), errDialog_ (0)
{
#ifdef _TRACE_OBJECTS
  printf ("                   Create rcConnectDialog Class Object\n");
#endif
  appContext_ = XtWidgetToApplicationContext (connect_->baseWidget ());
}

rcConnectDialog::~rcConnectDialog (void)
{
#ifdef _TRACE_OBJECTS
  printf ("                   Delete rcConnectDialog Class Object\n");
#endif
  // empty
}

#if defined (_CODA_2_0_T) || defined (_CODA_2_0)
void
rcConnectDialog::createFormChildren (void)
{
  Arg arg[20];
  int ac = 0;

  ac = 0;  

  // create label first
  XmString t = XmStringCreateSimple ("Start or locate rcServer.");
  XtSetArg (arg[ac], XmNtopAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNtopOffset, 2); ac++;
  XtSetArg (arg[ac], XmNleftAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNleftOffset, 2); ac++;
  XtSetArg (arg[ac], XmNrightAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNrightOffset, 2); ac++;
  XtSetArg (arg[ac], XmNlabelString, t); ac++;
  XtSetArg (arg[ac], XmNwidth, 60); ac++;
  Widget label = XtCreateManagedWidget ("connectDialogLabel",
					xmLabelGadgetClass,
					_w, arg, ac);
  ac = 0;
  XmStringFree (t);

  // create separator
  XtSetArg (arg[ac], XmNtopAttachment, XmATTACH_WIDGET); ac++;
  XtSetArg (arg[ac], XmNtopWidget, label); ac++;
  XtSetArg (arg[ac], XmNtopOffset, 3); ac++;
  XtSetArg (arg[ac], XmNleftAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNleftOffset, 2); ac++;
  XtSetArg (arg[ac], XmNrightAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNrightOffset, 2); ac++;
  Widget sep = XtCreateManagedWidget ("connectDialogSep",
				      xmSeparatorGadgetClass,
				      _w, arg, ac);
  ac = 0;

  // create bottom action from + sep
  XtSetArg (arg[ac], XmNleftAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNleftOffset, 2); ac++;
  XtSetArg (arg[ac], XmNrightAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNrightOffset, 2); ac++;
  XtSetArg (arg[ac], XmNbottomAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNbottomOffset, 3); ac++;
  Widget actionForm = XtCreateWidget ("connectDialogActionForm",
				      xmFormWidgetClass,
				      _w, arg, ac);
  XtManageChild (actionForm);
  // create all command buttons
  rcXpmComdButton *open = new rcXpmComdButton(actionForm,"Connect",NULL,"connect",NULL,netHandler_);
  rcXpmComdButton *spawn = new rcXpmComdButton(actionForm,"New",NULL,"spawn an rcServer",NULL,netHandler_);
  rcXpmComdButton *cancel = new rcXpmComdButton(actionForm,"Cancel",NULL,"cancel dialog",NULL,netHandler_);

  open->init();
  spawn->init();
  cancel->init();

  open_   = open->baseWidget();
  spawn_  = spawn->baseWidget();
  cancel_ = cancel->baseWidget();

  ac = 0;
  // try to center this widget
  Dimension spwd = 0;
  Dimension opwd = 0;
  Dimension cawd = 0;
  XtSetArg    (arg[ac], XmNwidth, &opwd); ac++;
  XtGetValues (open_, arg, ac); 
  ac = 0;
  XtSetArg    (arg[ac], XmNwidth, &spwd); ac++;
  XtGetValues (spawn_, arg, ac); 
  ac = 0;
  XtSetArg    (arg[ac], XmNwidth, &cawd); ac++;
  XtGetValues (cancel_, arg, ac); 
  ac = 0;
  XtSetArg    (arg[ac], XmNwidth, opwd+cawd+spwd+10); ac++;
  XtSetValues (actionForm, arg, ac);

  ac = 0;
  XtSetArg (arg[ac], XmNtopAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNbottomAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNleftAttachment, XmATTACH_POSITION); ac++;
  XtSetArg (arg[ac], XmNleftPosition, 2); ac++;
  XtSetValues (open_, arg, ac);
  
  ac = 0;
  XtSetArg    (arg[ac], XmNtopAttachment, XmATTACH_FORM); ac++;
  XtSetArg    (arg[ac], XmNbottomAttachment, XmATTACH_FORM); ac++;
  XtSetArg    (arg[ac], XmNleftAttachment, XmATTACH_POSITION); ac++;
  XtSetArg    (arg[ac], XmNleftPosition, 50); ac++;
  XtSetArg    (arg[ac], XmNleftOffset, -spwd/2); ac++;
  XtSetValues (spawn_, arg, ac);
  ac = 0;
  XtSetArg (arg[ac], XmNtopAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNbottomAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNrightAttachment, XmATTACH_POSITION); ac++;
  XtSetArg (arg[ac], XmNrightPosition, 98); ac++;
  XtSetValues (cancel_, arg, ac);
  ac = 0;
  
  // create another separator
  XtSetArg (arg[ac], XmNleftAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNleftOffset, 2); ac++;
  XtSetArg (arg[ac], XmNrightAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNrightOffset, 2); ac++;
  XtSetArg (arg[ac], XmNbottomAttachment, XmATTACH_WIDGET); ac++;
  XtSetArg (arg[ac], XmNbottomWidget, actionForm); ac++;
  XtSetArg (arg[ac], XmNbottomOffset, 3); ac++;
  Widget sep1 = XtCreateManagedWidget ("connectDialogSep",
				       xmSeparatorGadgetClass,
				       _w, arg, ac);
  ac = 0;

  // create form widget containing all text fields
  XtSetArg (arg[ac], XmNtopAttachment, XmATTACH_WIDGET); ac++;
  XtSetArg (arg[ac], XmNtopWidget, sep); ac++;
  XtSetArg (arg[ac], XmNtopOffset, 3); ac++;
  XtSetArg (arg[ac], XmNleftAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNleftOffset, 3); ac++;
  XtSetArg (arg[ac], XmNrightAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNrightOffset, 3); ac++;
  XtSetArg (arg[ac], XmNbottomAttachment, XmATTACH_WIDGET); ac++;
  XtSetArg (arg[ac], XmNbottomWidget, sep1); ac++;
  XtSetArg (arg[ac], XmNbottomOffset, 3); ac++;
  Widget tform = XtCreateManagedWidget ("connectDialogTform",
					xmFormWidgetClass,
					_w, arg, ac);  
  ac = 0;

  // first row hostname
  XtSetArg (arg[ac], XmNleftAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNrightAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNtopAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNtopOffset, 3); ac++;
  XtSetArg (arg[ac], XmNbottomAttachment, XmATTACH_POSITION); ac++;
  XtSetArg (arg[ac], XmNbottomPosition, 33); ac++;
  Widget tsubf0 = XtCreateWidget ("tsubf0", xmFormWidgetClass,
				  tform, arg, ac);
  ac = 0;

  // create 2nd row containing session + database name
  XtSetArg (arg[ac], XmNleftAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNrightAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNtopAttachment, XmATTACH_WIDGET); ac++;
  XtSetArg (arg[ac], XmNtopWidget, tsubf0); ac++;
  Widget tsubf1 = XtCreateWidget ("tsubf1", xmFormWidgetClass,
				  tform, arg, ac);  ac = 0;
  
  ac = 0;

   // create 3rd row containing session + database name
  XtSetArg (arg[ac], XmNleftAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNrightAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNbottomAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNtopAttachment, XmATTACH_WIDGET); ac++;
  XtSetArg (arg[ac], XmNtopWidget, tsubf1); ac++;
  Widget tsubf2 = XtCreateWidget ("tsubf2", xmFormWidgetClass,
				  tform, arg, ac);

  ac = 0;

  t = XmStringCreateSimple ("rcServer host");
  XtSetArg (arg[ac], XmNleftAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNtopAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNbottomAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNrightAttachment, XmATTACH_POSITION); ac++;
  XtSetArg (arg[ac], XmNrightPosition, 30); ac++;
  XtSetArg (arg[ac], XmNlabelString, t); ac++;
  XtSetArg (arg[ac], XmNalignment, XmALIGNMENT_BEGINNING); ac++;
  Widget hostLabel = XtCreateManagedWidget ("hostlabel", xmLabelGadgetClass,
					    tsubf0, arg, ac);
  ac = 0;
  XmStringFree (t);

  //XtSetArg (arg[ac], XmNrightAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNleftAttachment, XmATTACH_WIDGET); ac++;
  XtSetArg (arg[ac], XmNleftWidget, hostLabel); ac++;
  XtSetArg (arg[ac], XmNleftOffset, 5); ac++;
  XtSetArg (arg[ac], XmNrightAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNtopAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNbottomAttachment, XmATTACH_FORM); ac++;
  hostname_ = XtCreateManagedWidget ("hostDialog", xmTextFieldWidgetClass,
				     tsubf0, arg, ac);
 
  ac = 0;
  
  t = XmStringCreateSimple ("Experiment");
  XtSetArg (arg[ac], XmNleftAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNtopAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNbottomAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNrightAttachment, XmATTACH_POSITION); ac++;
  XtSetArg (arg[ac], XmNrightPosition, 30); ac++;
  XtSetArg (arg[ac], XmNlabelString, t); ac++;
  XtSetArg (arg[ac], XmNalignment, XmALIGNMENT_BEGINNING); ac++;
  Widget nameLabel = XtCreateManagedWidget ("namelabel", xmLabelGadgetClass,
					    tsubf1, arg, ac);
  ac = 0;
  XmStringFree (t);

  XtSetArg (arg[ac], XmNrightAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNleftAttachment, XmATTACH_WIDGET); ac++;
  XtSetArg (arg[ac], XmNleftWidget, nameLabel); ac++;
  XtSetArg (arg[ac], XmNleftOffset, 5); ac++;
  XtSetArg (arg[ac], XmNtopAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNbottomAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNeditable, TRUE); ac++;
  exptid_ = XtCreateManagedWidget ("databaseN", xmComboBoxWidgetClass,
				   tsubf1, arg, ac);
  ac = 0;

  t = XmStringCreateSimple ("Session");
  XtSetArg (arg[ac], XmNleftAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNtopAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNbottomAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNrightAttachment, XmATTACH_POSITION); ac++;
  XtSetArg (arg[ac], XmNrightPosition, 30); ac++;
  XtSetArg (arg[ac], XmNlabelString, t); ac++;
  XtSetArg (arg[ac], XmNalignment, XmALIGNMENT_BEGINNING); ac++;
  Widget idLabel = XtCreateManagedWidget ("idlabel", xmLabelGadgetClass,
					  tsubf2, arg, ac);
  ac = 0;
  XmStringFree (t);

  XtSetArg (arg[ac], XmNrightAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNleftAttachment, XmATTACH_WIDGET); ac++;
  XtSetArg (arg[ac], XmNleftWidget, idLabel); ac++;
  XtSetArg (arg[ac], XmNleftOffset, 5); ac++;
  XtSetArg (arg[ac], XmNtopAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNbottomAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNeditable, TRUE); ac++;
  exptname_ = XtCreateManagedWidget ("idDialog", xmComboBoxWidgetClass,
				     tsubf2, arg, ac);

  // add callbacks
  XtAddCallback (cancel_, XmNactivateCallback,
		 (XtCallbackProc)&(rcConnectDialog::cancelCallback),
		 (XtPointer)this);
  XtAddCallback (open_, XmNactivateCallback,
		 (XtCallbackProc)&(rcConnectDialog::openCallback),
		 (XtPointer)this);
  XtAddCallback (spawn_, XmNactivateCallback,
		 (XtCallbackProc)&(rcConnectDialog::spawnCallback),
		 (XtPointer)this);

  // add callback for ComboBox of Database Selection
  XtAddCallback (exptid_, XmNdropDownCallback,
		 (XtCallbackProc)&(rcConnectDialog::dbaseDropDownCbk),
		 (XtPointer)this);
  XtAddCallback (exptname_, XmNdropDownCallback,
		 (XtCallbackProc)&(rcConnectDialog::sessionDropDownCbk),
		 (XtPointer)this);

  // manage all widgets
  XtManageChild (tsubf0);
  XtManageChild (tsubf1);
  XtManageChild (tsubf2);
  XtManageChild (tform);
  
  // set default button open
  defaultButton (open_);
}


void
rcConnectDialog::popup (void)
{
  XcodaFormDialog::popup ();
  rcComdOption* option = rcComdOption::option ();
  rcDbaseHandler* handler = rcDbaseHandler::dbaseHandler ();

  XmTextFieldSetString (hostname_, option->serverHost ());
  // use database handler to find all databases and sessions
  if (option->session())
    XmComboBoxSetString (exptname_, option->session ());
  else
    XmComboBoxSetString (exptname_, "");

  if (option->dbasename())
    XmComboBoxSetString (exptid_, option->dbasename ());
  else
    XmComboBoxSetString (exptid_, "");
}

extern int doTk;

int
rcConnectDialog::storeRcServerInfo (void)
{
  char *hostname = 0;
  char *session = 0;
  char *dbasename = 0;
  char *codadb = 0;

  // get option object
  rcComdOption* option = rcComdOption::option ();


  hostname = XmTextFieldGetString (hostname_);
  if (!hostname || !*hostname) {
    XBell (XtDisplay (_w), 50);
    return CODA_ERROR;
  }
  session = XmComboBoxGetString (exptname_);
  if (!session || !*session) {
    XBell (XtDisplay (_w), 50);
    return CODA_ERROR;
  }
  dbasename = XmComboBoxGetString (exptid_);
  if (!dbasename || !*dbasename) {
    XBell (XtDisplay (_w), 50);
    return CODA_ERROR;
  }

  // redefine option according to the dialogs
  option->serverHost (hostname); XtFree (hostname); 
  option->session (session);     XtFree (session);
  option->dbasename (dbasename);

  {
    char cmd[100];

    sprintf(cmd,"e:%s",dbasename);

printf("CEDIT 2\n");
#ifdef USE_CREG
    coda_send(XtDisplay(_w),"CEDIT",cmd);
#endif
  }
  XtFree (dbasename);

  if (codadb) {
    option->codadb (codadb);
    XtFree (codadb);
  }
  else
    option->codadb (0);

  return CODA_SUCCESS;
}  

void
rcConnectDialog::connect (void)
{

  if (storeRcServerInfo () != CODA_SUCCESS)
    return;

  // info panel will notify all related window about status of remote run
  // control server
  popdown ();


  if (connect_->connect ( ) != CODA_SUCCESS) {
    reportError ("Cannot connect to the RunControl Server");
    rcAudio ("can not connect to run control server");
  }
}

void
rcConnectDialog::dbaseDropDownCbk (Widget w, XtPointer data,
				   XmComboBoxDropDownCallbackStruct* cbs)
{
  rcConnectDialog* dialog = (rcConnectDialog *)data;
  
  // get database handler
  rcDbaseHandler* dbaseHandler = rcDbaseHandler::dbaseHandler ();

  // get option object
  rcComdOption* option = rcComdOption::option ();

  char* msqldhost =  option->msqldhost ();

  int i = 0;
  XmString t;

  if (cbs->reason == XmCR_SHOW_LIST) {
    // try to connect to database
    if (dbaseHandler->connect (msqldhost) != CODA_SUCCESS) {
      dialog->reportError ("Cannot connect to msql database server");
      rcAudio ("can not connect to msql database server");
      return;
    }
    // first to remove all old items
    XmComboBoxDeleteAllItems (dialog->exptid_);
    // get all databases from dbase handler
    if (dbaseHandler->listAllDatabases () != CODA_SUCCESS) {
      dialog->reportError ("Listing all databases from msql database failed");
      return;
    }
    int numdbases = 0;
    char **dbases = dbaseHandler->allDatabases (numdbases);

    if (numdbases <= 0) {
      dialog->reportError ("No databases have been found");
      return;
    }
    
    for (i = 0; i < numdbases; i++) {
      t = XmStringCreateSimple (dbases[i]);
      XmComboBoxAddItemUnselected (dialog->exptid_, t, 0);
      XmStringFree (t);
    }
  }
  else {
    // disconenct database handler from the database server
    dbaseHandler->close ();
  }
}

void
rcConnectDialog::sessionDropDownCbk (Widget w, XtPointer data,
				     XmComboBoxDropDownCallbackStruct* cbs)
{
  rcConnectDialog* dialog = (rcConnectDialog *)data;
  
  // get database handler
  rcDbaseHandler* dbaseHandler = rcDbaseHandler::dbaseHandler ();

  // get option object
  rcComdOption* option = rcComdOption::option ();

  char* msqldhost =  option->msqldhost ();


  int i = 0;
  XmString t;

  if (cbs->reason == XmCR_SHOW_LIST) {
    // try to connect to database
    if (dbaseHandler->connect (msqldhost) != CODA_SUCCESS) {
      dialog->reportError ("Cannot connect to msql database server");
      return;
    }
    // select database selected from the list
    char* dbase = XmComboBoxGetString (dialog->exptid_);
    if (!dbase || !*dbase){
      dialog->reportError ("Fatal: Cannot find selected database");
      XtFree (dbase);
      return;
    }
    dbaseHandler->database (dbase);

    // first to remove all old items
    XmComboBoxDeleteAllItems (dialog->exptname_);
    // get all databases from dbase handler
    if (dbaseHandler->listAllSessions () != CODA_SUCCESS) {
      dialog->reportError ("Listing all sessions from msql database failed");
      return;
    }
    int numSessions = 0;
    char **sessions = dbaseHandler->allSessions (numSessions);
    int* sessionStatus = dbaseHandler->sessionStatus (numSessions);
    
    for (i = 0; i < numSessions; i++) {
      t = XmStringCreateSimple (sessions[i]);
      XmComboBoxAddItemUnselected (dialog->exptname_, t, 0);
      XmStringFree (t);
    }
  }
  else {
    // disconenct database handler from the database server
    dbaseHandler->close ();
  }
}

void
rcConnectDialog::spawnCallback (Widget w, XtPointer data,
				XmAnyCallbackStruct* cbs)
{
  rcConnectDialog* dialog = (rcConnectDialog *)data;
  rcComdOption* option = rcComdOption::option ();

  if (dialog->storeRcServerInfo() != CODA_SUCCESS)
    return;

  dialog->popdown ();

  dialog->connect_->startRcServer ();
}

#else
void
rcConnectDialog::createFormChildren (void)
{
  Arg arg[20];
  int ac = 0;

  // create label first
  XmString t = XmStringCreateSimple ("RunControl Server Information");
  XtSetArg (arg[ac], XmNtopAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNtopOffset, 2); ac++;
  XtSetArg (arg[ac], XmNleftAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNleftOffset, 2); ac++;
  XtSetArg (arg[ac], XmNrightAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNrightOffset, 2); ac++;
  XtSetArg (arg[ac], XmNlabelString, t); ac++;
  Widget label = XtCreateManagedWidget ("connectDialogLabel",
					xmLabelGadgetClass,
					_w, arg, ac);
  ac = 0;
  XmStringFree (t);

  // create separator
  XtSetArg (arg[ac], XmNtopAttachment, XmATTACH_WIDGET); ac++;
  XtSetArg (arg[ac], XmNtopWidget, label); ac++;
  XtSetArg (arg[ac], XmNtopOffset, 3); ac++;
  XtSetArg (arg[ac], XmNleftAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNleftOffset, 2); ac++;
  XtSetArg (arg[ac], XmNrightAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNrightOffset, 2); ac++;
  Widget sep = XtCreateManagedWidget ("connectDialogSep",
				      xmSeparatorGadgetClass,
				      _w, arg, ac);
  ac = 0;

  // create bottom action from + sep
  XtSetArg (arg[ac], XmNleftAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNleftOffset, 2); ac++;
  XtSetArg (arg[ac], XmNrightAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNrightOffset, 2); ac++;
  XtSetArg (arg[ac], XmNbottomAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNbottomOffset, 3); ac++;
  Widget actionForm = XtCreateWidget ("connectDialogActionForm",
				      xmFormWidgetClass,
				      _w, arg, ac);
  ac = 0;

  // create all command buttons
  t = XmStringCreateSimple ("  Open  ");
  XtSetArg (arg[ac], XmNtopAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNbottomAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNleftAttachment, XmATTACH_POSITION); ac++;
  XtSetArg (arg[ac], XmNleftPosition, 10); ac++;
  XtSetArg (arg[ac], XmNlabelString, t); ac++;
  XtSetArg (arg[ac], XmNshowAsDefault, 1); ac++;
  open_ = XtCreateManagedWidget ("open", xmPushButtonGadgetClass,
				 actionForm, arg, ac);
  ac = 0;
  XmStringFree (t);

  t = XmStringCreateSimple (" Cancel ");
  XtSetArg (arg[ac], XmNtopAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNtopOffset, 5); ac++;
  XtSetArg (arg[ac], XmNbottomAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNbottomOffset, 5); ac++;
  XtSetArg (arg[ac], XmNrightAttachment, XmATTACH_POSITION); ac++;
  XtSetArg (arg[ac], XmNrightPosition, 90); ac++;
  XtSetArg (arg[ac], XmNlabelString, t); ac++;
  cancel_ = XtCreateManagedWidget ("cancel", xmPushButtonGadgetClass,
				   actionForm, arg, ac);
  ac = 0;
  XmStringFree (t);
  
  // create another separator
  XtSetArg (arg[ac], XmNleftAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNleftOffset, 2); ac++;
  XtSetArg (arg[ac], XmNrightAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNrightOffset, 2); ac++;
  XtSetArg (arg[ac], XmNbottomAttachment, XmATTACH_WIDGET); ac++;
  XtSetArg (arg[ac], XmNbottomWidget, actionForm); ac++;
  XtSetArg (arg[ac], XmNbottomOffset, 3); ac++;
  Widget sep1 = XtCreateManagedWidget ("connectDialogSep",
				       xmSeparatorGadgetClass,
				       _w, arg, ac);
  ac = 0;

  // create form widget containing all text fields
  XtSetArg (arg[ac], XmNtopAttachment, XmATTACH_WIDGET); ac++;
  XtSetArg (arg[ac], XmNtopWidget, sep); ac++;
  XtSetArg (arg[ac], XmNtopOffset, 3); ac++;
  XtSetArg (arg[ac], XmNleftAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNleftOffset, 3); ac++;
  XtSetArg (arg[ac], XmNrightAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNrightOffset, 3); ac++;
  XtSetArg (arg[ac], XmNbottomAttachment, XmATTACH_WIDGET); ac++;
  XtSetArg (arg[ac], XmNbottomWidget, sep1); ac++;
  XtSetArg (arg[ac], XmNbottomOffset, 3); ac++;
  Widget tform = XtCreateManagedWidget ("connectDialogTform",
					xmFormWidgetClass,
					_w, arg, ac);  
  ac = 0;

  // first row hostname
  XtSetArg (arg[ac], XmNleftAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNrightAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNtopAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNtopOffset, 3); ac++;
  XtSetArg (arg[ac], XmNbottomAttachment, XmATTACH_POSITION); ac++;
  XtSetArg (arg[ac], XmNbottomPosition, 49); ac++;
  Widget tsubf0 = XtCreateWidget ("tsubf0", xmFormWidgetClass,
				  tform, arg, ac);
  ac = 0;
  
  t = XmStringCreateSimple ("Host:");
  XtSetArg (arg[ac], XmNleftAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNtopAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNbottomAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNrightAttachment, XmATTACH_POSITION); ac++;
  XtSetArg (arg[ac], XmNrightPosition, 20); ac++;
  XtSetArg (arg[ac], XmNlabelString, t); ac++;
  XtSetArg (arg[ac], XmNalignment, XmALIGNMENT_END); ac++;
  Widget hostLabel = XtCreateManagedWidget ("hostlabel", xmLabelGadgetClass,
					    tsubf0, arg, ac);
  ac = 0;
  XmStringFree (t);

  XtSetArg (arg[ac], XmNrightAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNleftAttachment, XmATTACH_WIDGET); ac++;
  XtSetArg (arg[ac], XmNleftWidget, hostLabel); ac++;
  XtSetArg (arg[ac], XmNleftOffset, 5); ac++;
  XtSetArg (arg[ac], XmNtopAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNbottomAttachment, XmATTACH_FORM); ac++;
  hostname_ = XtCreateManagedWidget ("hostDialog", xmTextFieldWidgetClass,
				     tsubf0, arg, ac);
  ac = 0;

  // create 2nd row containing experiment name + exptid
  XtSetArg (arg[ac], XmNleftAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNrightAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNtopAttachment, XmATTACH_POSITION); ac++;
  XtSetArg (arg[ac], XmNtopPosition, 51); ac++;
  XtSetArg (arg[ac], XmNbottomAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNbottomOffset, 3); ac++;
  Widget tsubf1 = XtCreateWidget ("tsubf1", xmFormWidgetClass,
				  tform, arg, ac);
  ac = 0;
  
  t = XmStringCreateSimple ("Name:");
  XtSetArg (arg[ac], XmNleftAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNtopAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNbottomAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNrightAttachment, XmATTACH_POSITION); ac++;
  XtSetArg (arg[ac], XmNrightPosition, 20); ac++;
  XtSetArg (arg[ac], XmNlabelString, t); ac++;
  XtSetArg (arg[ac], XmNalignment, XmALIGNMENT_END); ac++;
  Widget nameLabel = XtCreateManagedWidget ("namelabel", xmLabelGadgetClass,
					    tsubf1, arg, ac);
  ac = 0;
  XmStringFree (t);

  XtSetArg (arg[ac], XmNleftAttachment, XmATTACH_WIDGET); ac++;
  XtSetArg (arg[ac], XmNleftWidget, nameLabel); ac++;
  XtSetArg (arg[ac], XmNleftOffset, 5); ac++;
  XtSetArg (arg[ac], XmNrightAttachment, XmATTACH_POSITION); ac++;
  XtSetArg (arg[ac], XmNrightPosition, 49); ac++;
  XtSetArg (arg[ac], XmNtopAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNbottomAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNcolumns, 10); ac++;
  exptname_ = XtCreateManagedWidget ("nameDialog", xmTextFieldWidgetClass,
				     tsubf1, arg, ac);
  ac = 0;  

  
  t = XmStringCreateSimple ("ID:");
  XtSetArg (arg[ac], XmNleftAttachment, XmATTACH_WIDGET); ac++;
  XtSetArg (arg[ac], XmNleftWidget, exptname_); ac++;
  XtSetArg (arg[ac], XmNrightAttachment, XmATTACH_POSITION); ac++;
  XtSetArg (arg[ac], XmNrightPosition, 80); ac++;
  XtSetArg (arg[ac], XmNtopAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNbottomAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNlabelString, t); ac++;
  XtSetArg (arg[ac], XmNalignment, XmALIGNMENT_END); ac++;
  Widget idLabel = XtCreateManagedWidget ("idlabel", xmLabelGadgetClass,
					  tsubf1, arg, ac);
  ac = 0;
  XmStringFree (t);

  XtSetArg (arg[ac], XmNrightAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNleftAttachment, XmATTACH_WIDGET); ac++;
  XtSetArg (arg[ac], XmNleftWidget, idLabel); ac++;
  XtSetArg (arg[ac], XmNleftOffset, 5); ac++;
  XtSetArg (arg[ac], XmNtopAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNbottomAttachment, XmATTACH_FORM); ac++;
  exptid_ = XtCreateManagedWidget ("idDialog", xmTextFieldWidgetClass,
				   tsubf1, arg, ac);
  ac = 0;

  // add callbacks
  XtAddCallback (cancel_, XmNactivateCallback,
		 (XtCallbackProc)&(rcConnectDialog::cancelCallback),
		 (XtPointer)this);
  XtAddCallback (open_, XmNactivateCallback,
		 (XtCallbackProc)&(rcConnectDialog::openCallback),
		 (XtPointer)this);

  // manage all widgets
  XtManageChild (tsubf0);
  XtManageChild (tsubf1);
  XtManageChild (tform);
  XtManageChild (actionForm);
  
  // set default button open
  defaultButton (open_);
}


void
rcConnectDialog::popup (void)
{
  XcodaFormDialog::popup ();
  if (firstTime_) {
    char* hostname = ::getenv (CODA_RCHOST_ENV);
    if (hostname)
      XmTextFieldSetString (hostname_, hostname);
    char* exptname = ::getenv (CODA_RCNAME_ENV);
    if (exptname)
      XmTextFieldSetString (exptname_, exptname);
    char* id = ::getenv (CODA_RCID_ENV);
    if (id)
      XmTextFieldSetString (exptid_, id);
    firstTime_ = 0;
  }
}



void
rcConnectDialog::connect (void)
{
  char *hostname = 0;
  char *exptname = 0;
  char *exptid = 0;

  printf("rcConnectDialog::connect reached\n");

  hostname = XmTextFieldGetString (hostname_);
  if (!hostname || !*hostname) {
    XBell (XtDisplay (_w), 50);
    return;
  }
  exptname = XmTextFieldGetString (exptname_);
  if (!exptname || !*exptname) {
    XBell (XtDisplay (_w), 50);
    return;
  }
  exptid = XmTextFieldGetString (exptid_);
  int   realid = 0;
  if (!exptid || !*exptid) {
    XBell (XtDisplay (_w), 50);
    return;
  }
  else {
    char tid[80];
    ::strcpy (tid, exptid);
    int status = ::sscanf (tid, "%d", &realid);
    if (status < 1) {
      reportError ("Expriment Id must be a non-negative integer");
      return;
    }
  }
  // info panel will notify all related window about status of remote run
  // control server
  popdown ();


  // start network connection
  // if first connection attempt is not successful, we start a timer
  // to try several times. 
  connect_->stWin_->startShowingStatus ();
  if(netHandler_.connect (hostname, exptname, realid,
			   &(rcConnectDialog::updateCallback),
			   (void *)this) != CODA_SUCCESS) 
  {
    reportError ("Cannot connect to the runControl Server");
  }
  else
  {
    printf("rcConnectDialog::connect: connected to the runControl Server\n");
  }
  connect_->stWin_->stopShowingStatus (); 
  XtFree (hostname);
  XtFree (exptname);
  XtFree (exptid);
}

#endif


void
rcConnectDialog::cancelCallback (Widget, XtPointer clientData,
				 XmAnyCallbackStruct* )
{
  rcConnectDialog* dialog = (rcConnectDialog *)clientData;
  
  dialog->popdown ();
}

  
void
rcConnectDialog::openCallback (Widget, XtPointer clientData,
			       XmAnyCallbackStruct *)
{
  rcConnectDialog *dialog = (rcConnectDialog *)clientData;
  
  dialog->connect ();
}

int
rcConnectDialog::updateCallback (void* data)
{
  rcConnectDialog *dialog = (rcConnectDialog *)data;
  Display* display = XtDisplay (dialog->_w);
  
  rcConnectDialog::updateDisplay (display);

  dialog->connect_->stWin_->showingInProgress ();  

  return 0;
}   

void
rcConnectDialog::updateDisplay (Display* disp)
{
  XEvent ev;

  /* First process all available events ... */
  while ( XCheckMaskEvent(disp, 
			  ExposureMask | ButtonPressMask | ButtonReleaseMask,
			  &ev))
    XtDispatchEvent(&ev);
    
  /* Flush all buffers */
  XSync(disp,False);
    
  /* Process remaining events ... */
  while ( XCheckMaskEvent(disp, 
			  ExposureMask | ButtonPressMask | ButtonReleaseMask,
			  &ev))
    XtDispatchEvent(&ev);
}  

void
rcConnectDialog::reportError (char* error)
{
  if (!errDialog_) {
    errDialog_ = new XcodaErrorDialog (_w, "connect error",
				       "connection error");
    errDialog_->init ();
  }
  if (errDialog_->isMapped ())
    errDialog_->popdown ();
  errDialog_->setMessage (error);
  errDialog_->popup ();
}    
