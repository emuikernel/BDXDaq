/*-----------------------------------------------------------------------------
 * Copyright (c) 1991,1992 Southeastern Universities Research Association,
 *                         Continuous Electron Beam Accelerator Facility
 *
 * This software was developed under a United States Government license
 * described in the NOTICE file included as part of this distribution.
 *
 * CEBAF Data Acquisition Group, 12000 Jefferson Ave., Newport News, VA 23606
 * Email: coda@cebaf.gov  Tel: (804) 249-7101  Fax: (804) 249-7363
 *-----------------------------------------------------------------------------
 * 
 * Description:
 *	Implementation of Editor_dbaseSel
 *	
 * Author:  Jie Chen
 * CEBAF Data Acquisition Group
 *
 * Revision History:
 *   $Log: Editor_dbaseSel.c,v $
 *   Revision 1.2  1997/06/16 12:23:41  heyes
 *   various fixes and nicities!
 *
 *   Revision 1.1.1.1  1996/08/21 19:36:06  heyes
 *   Imported sources
 *
 *	  
 */
#include "Editor_dbaseSel.h"
#include <Xm/Xm.h>
#include <Xm/PushBG.h>
#include <Xm/Form.h>
#include <Xm/RowColumn.h>
#include <Xm/SeparatoG.h>

static editorDbaseSel  idbaseSel;

static Widget
createOptionMenu (Widget parent)
{
  Arg arg[20];
  int ac = 0;
  XmString t;
  Widget menu;
  Widget option;
  int    i = 0;

  menu = XmCreatePulldownMenu (parent, "optionPullDown", NULL, 0);

  t = XmStringCreateSimple ("RCDATABASE");
  XtSetArg (arg[ac], XmNlabelString, t); ac++;
  XtSetArg (arg[ac], XmNsubMenuId, menu); ac++;
  option = XmCreateOptionMenu (parent, "databaseOption", arg, ac);
  ac = 0;
  XmStringFree (t);

  t = XmStringCreateSimple ("              ");
  XtSetArg (arg[ac], XmNlabelString, t); ac++;
  for (i = 0; i < EDITOR_MAX_DATABASES; i++) {
    idbaseSel.pushb[i] = XtCreateWidget ("optionButton", 
					 xmPushButtonGadgetClass,
					 menu, arg, ac);
    if (i == 0)
      XtManageChild (idbaseSel.pushb[i]);
  }
  ac = 0;
  XmStringFree (t);
  
  return option;
}

static void
dbaseSelOk (Widget w, XtPointer data, XmAnyCallbackStruct* cbs)
{
  int  i = 0;
  char* currdbase;
  Arg arg[20];
  int ac = 0;
  Widget curr;
  char  temp[128];
  rcNetComp* daq_list[MAX_NUM_COMPS];
  int        num_comps;

  editorDbaseSel* sel = (editorDbaseSel *)data;

  /* popdown dialog first */
  dbaseSelPopdown ();

  
  /* get current selection */
  XtSetArg (arg[ac], XmNmenuHistory, &curr); ac++;
  XtGetValues (sel->option_, arg, ac);
  ac = 0;
  
  for (i = 0; i < sel->numDbases_; i++) {
    if (curr == sel->pushb[i])
      break;
  }
  currdbase = sel->dbasenames_[i];

  /* tell database handler */
  if (selectDatabase (currdbase) < 0) {
    XcodaEditorShowDatabaseName (0);
    sprintf (temp, "Cannot select database %s", currdbase);
    pop_error_message (temp, w);
    return;
  }
  else {
    XcodaEditorShowDatabaseName (currdbase);
    XcodaEditorEnableInput ();
    if (constructRcnetComps (daq_list, &num_comps) == 0) 
      XcodaEditorCreateIconsFromRcnet(daq_list, num_comps);
  }
}

void
EditorSelectExp (Widget w, char *exp)
{
  int  i = 0;
  char* currdbase;
  Arg arg[20];
  int ac = 0;
  Widget curr;
  char  temp[128];
  rcNetComp* daq_list[MAX_NUM_COMPS];
  int        num_comps;

  currdbase = exp;

  /* tell database handler */
  if (selectDatabase (currdbase) < 0) {
    XcodaEditorShowDatabaseName (0);
    sprintf (temp, "Cannot select database %s", currdbase);
    pop_error_message (temp, w);
    return;
  }
  else {
    XcodaEditorShowDatabaseName (currdbase);
    XcodaEditorEnableInput ();
    if (constructRcnetComps (daq_list, &num_comps) == 0) 
      XcodaEditorCreateIconsFromRcnet(daq_list, num_comps);
  }
}

static void
dbaseSelCancel (Widget w, XtPointer data, XmAnyCallbackStruct* cbs)
{
  dbaseSelPopdown ();
}

void
initDbaseSel (Widget parent)
{
  Arg arg[20];
  int ac = 0;
  XmString t;
  Widget actionForm, sep;
  static int inited = 0;
  
  if (inited) 
    return;

  idbaseSel.numDbases_ = 0;
  idbaseSel.managed_ = 0;
  idbaseSel.w_ = 0;
  idbaseSel.option_ = 0;
  idbaseSel.ok_ = 0;
  idbaseSel.cancel_ = 0;

  /* create all widgets */
  XtSetArg (arg[ac], XmNtitle, "Database selection"); ac++;
  XtSetArg (arg[ac], XmNautoUnmanage, FALSE); ac++;
  idbaseSel.w_ = XmCreateFormDialog (parent, "editorDBaseSel",
				     arg, ac);
  ac = 0;

  /* create action form */
  XtSetArg (arg[ac], XmNbottomAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNbottomOffset, 5); ac++;
  XtSetArg (arg[ac], XmNleftAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNleftOffset, 2); ac++;
  XtSetArg (arg[ac], XmNrightAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNrightOffset, 2); ac++;
  XtSetArg (arg[ac], XmNwidth, 300); ac++;
  actionForm = XtCreateWidget ("runTypeActionForm",
			       xmFormWidgetClass, idbaseSel.w_,
			       arg, ac);
  ac = 0;

  /* create push buttons */
  t = XmStringCreateSimple ("   Ok   ");
  XtSetArg (arg[ac], XmNtopAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNbottomAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNleftAttachment, XmATTACH_POSITION); ac++;
  XtSetArg (arg[ac], XmNleftPosition, 10); ac++;
  XtSetArg (arg[ac], XmNlabelString, t); ac++;
  XtSetArg (arg[ac], XmNshowAsDefault, 1); ac++;
  idbaseSel.ok_ = XtCreateManagedWidget ("runTypeOk", xmPushButtonGadgetClass,
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
  idbaseSel.cancel_ = XtCreateManagedWidget ("cancel", xmPushButtonGadgetClass,
					     actionForm, arg, ac);
  ac = 0;
  XmStringFree (t);

  /* create a separator */
  XtSetArg (arg[ac], XmNleftAttachment, XmATTACH_FORM); ac++; 
  XtSetArg (arg[ac], XmNleftOffset, 5); ac++;
  XtSetArg (arg[ac], XmNrightAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNrightOffset, 5); ac++;
  XtSetArg (arg[ac], XmNbottomAttachment, XmATTACH_WIDGET); ac++;
  XtSetArg (arg[ac], XmNbottomWidget, actionForm); ac++;
  XtSetArg (arg[ac], XmNbottomOffset, 10); ac++;
  XtSetArg (arg[ac], XmNorientation, XmHORIZONTAL); ac++;
  sep = XtCreateManagedWidget ("sep", xmSeparatorGadgetClass, idbaseSel.w_,
			       arg, ac);
  ac = 0;
					  
  

  /* create optin menu */
  idbaseSel.option_ = createOptionMenu (idbaseSel.w_);
  XtSetArg (arg[ac], XmNtopAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNtopOffset, 20); ac++;
  XtSetArg (arg[ac], XmNleftAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNleftOffset, 10); ac++;
  XtSetArg (arg[ac], XmNbottomAttachment, XmATTACH_WIDGET); ac++;
  XtSetArg (arg[ac], XmNbottomWidget, sep); ac++;
  XtSetArg (arg[ac], XmNbottomOffset, 10); ac++;
  XtSetValues (idbaseSel.option_, arg, ac);
  ac = 0;

  /* add callbacks */
  XtAddCallback (idbaseSel.ok_, XmNactivateCallback, dbaseSelOk,
		 (XtPointer)&(idbaseSel));
  XtAddCallback (idbaseSel.cancel_, XmNactivateCallback, dbaseSelCancel,
		 (XtPointer)&(idbaseSel));


  XtManageChild (actionForm);
  XtManageChild (idbaseSel.option_);
  
  /* set default button */
  XtSetArg (arg[ac], XmNdefaultButton, idbaseSel.ok_); ac++;
  XtSetValues (idbaseSel.w_, arg, ac);
  ac = 0;

  /* set inited flag */
  inited = 1;
}

void
dbaseSelPopup (void)
{
  int  status;
  int  i;
  Arg  arg[10];
  int  ac = 0;
  XmString t;

  if (idbaseSel.managed_) 
    dbaseSelPopdown ();
  
  /* remove old database information */
  for (i = 0; i < idbaseSel.numDbases_; i++)
    free (idbaseSel.dbasenames_[i]);

  /* get all database names */
  status = listAllDatabases (idbaseSel.dbasenames_, &(idbaseSel.numDbases_));

  if (status == 0) {
    if (idbaseSel.numDbases_ > 0) {
      for (i = 0; i < idbaseSel.numDbases_; i++) {
	ac = 0;
	t = XmStringCreateSimple (idbaseSel.dbasenames_[i]);
	XtSetArg (arg[ac], XmNlabelString, t); ac++;
	XtSetValues (idbaseSel.pushb[i], arg, ac);
	ac = 0;
	XmStringFree (t);
	XtManageChild (idbaseSel.pushb[i]);
      }
    }
    else {
      t = XmStringCreateSimple ("no database");
      XtSetArg (arg[ac], XmNlabelString, t); ac++;
      XtSetValues (idbaseSel.pushb[0], arg, ac);
      ac = 0;
      XmStringFree (t);
      /* disable ok button */
      XtSetSensitive (idbaseSel.ok_, FALSE);
    }
  }
  else
    idbaseSel.numDbases_ = 0;
      
  XtManageChild (idbaseSel.w_);
  XtPopup (XtParent (idbaseSel.w_), XtGrabNone);
  idbaseSel.managed_ = 1;
}

void
dbaseSelPopdown (void)
{
  int i = 0;

  if (idbaseSel.managed_) 
    XtPopdown (XtParent (idbaseSel.w_));
  idbaseSel.managed_ = 0;
  for (i = 0; i < EDITOR_MAX_DATABASES; i++)
    XtUnmanageChild (idbaseSel.pushb[i]);
  /* reset ok button sensitivity */
  XtSetSensitive (idbaseSel.ok_, TRUE);
}


	       

