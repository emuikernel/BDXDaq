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
 *	Implementation of Editor_dbaseSel (removal)
 *	
 * Author:  Jie Chen
 * CEBAF Data Acquisition Group
 *
 * Revision History:
 *   $Log: Editor_rmdbaseSel.c,v $
 *   Revision 1.1.1.1  1996/08/21 19:36:07  heyes
 *   Imported sources
 *
 *	  
 */
#include "Editor_rmdbaseSel.h"
#include <Xm/Xm.h>
#include <Xm/PushBG.h>
#include <Xm/Form.h>
#include <Xm/RowColumn.h>
#include <Xm/SeparatoG.h>
#include <Xm/LabelG.h>

#include "Editor_graph.h"

static editorRmDbaseSel  rmdbaseSel;

static Widget createOptionMenu (Widget parent)
{
  Arg arg[20];
  int ac = 0;
  XmString t;
  Widget menu;
  Widget option;
  int    i = 0;

  menu = XmCreatePulldownMenu (parent, "optionPullDown", NULL, 0);

  t = XmStringCreateSimple ("Database Name ");
  XtSetArg (arg[ac], XmNlabelString, t); ac++;
  XtSetArg (arg[ac], XmNsubMenuId, menu); ac++;
  option = XmCreateOptionMenu (parent, "databaseOption", arg, ac);
  ac = 0;
  XmStringFree (t);

  t = XmStringCreateSimple ("              ");
  XtSetArg (arg[ac], XmNlabelString, t); ac++;
  for (i = 0; i < EDITOR_MAX_DATABASES; i++) {
    rmdbaseSel.pushb[i] = XtCreateWidget ("optionButton", 
					 xmPushButtonGadgetClass,
					 menu, arg, ac);
    if (i == 0)
      XtManageChild (rmdbaseSel.pushb[i]);
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

  editorRmDbaseSel* sel = (editorRmDbaseSel *)data;

  /* popdown dialog first */
  removeDbaseSelPopdown ();

  /* get current selection */
  XtSetArg (arg[ac], XmNmenuHistory, &curr); ac++;
  XtGetValues (sel->option_, arg, ac);
  ac = 0;
  
  for (i = 0; i < sel->numDbases_; i++) {
    if (curr == sel->pushb[i])
      break;
  }
  currdbase = sel->dbasenames_[i];

  if (removeDatabase (currdbase) < 0) {
    sprintf (temp, "Cannot remove database %s", currdbase);
    pop_error_message (temp, sw_geometry.draw_area);
    return;
  }
  else {
    if (databaseSelected ()) {
      if (strcmp (currdbase, currentDatabase ()) == 0) {
	 delete_everything(w, data, cbs);
	 delete_everyicons( );
	 XcodaEditorDisableInput ();
	 /* set current database and configuration to null */
	 XcodaEditorShowDatabaseName (0);
	 XcodaEditorShowConfigName   (0);
	 /* tell database handler to clean up */
	 cleanDatabaseMiscInfo ();
      }
    }
  }
}

static void
dbaseSelCancel (Widget w, XtPointer data, XmAnyCallbackStruct* cbs)
{
  removeDbaseSelPopdown ();
}
  
void
initRemoveDbaseSel (Widget parent)
{
  Arg arg[20];
  int ac = 0;
  XmString t;
  char temp[128];
  Widget actionForm, sep, label;
  static int inited = 0;
  
  if (inited) 
    return;

  rmdbaseSel.numDbases_ = 0;
  rmdbaseSel.managed_ = 0;
  rmdbaseSel.w_ = 0;
  rmdbaseSel.option_ = 0;
  rmdbaseSel.ok_ = 0;
  rmdbaseSel.cancel_ = 0;

  /* create all widgets */
  XtSetArg (arg[ac], XmNtitle, "Database selection"); ac++;
  XtSetArg (arg[ac], XmNautoUnmanage, FALSE); ac++;
  rmdbaseSel.w_ = XmCreateFormDialog (parent, "editorDBaseSel",
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
			       xmFormWidgetClass, rmdbaseSel.w_,
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
  rmdbaseSel.ok_ = XtCreateManagedWidget ("runTypeOk", xmPushButtonGadgetClass,
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
  rmdbaseSel.cancel_ = XtCreateManagedWidget ("cancel", xmPushButtonGadgetClass,
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
  sep = XtCreateManagedWidget ("sep", xmSeparatorGadgetClass, rmdbaseSel.w_,
			       arg, ac);
  ac = 0;
					  
  

  /* create option menu */
  rmdbaseSel.option_ = createOptionMenu (rmdbaseSel.w_);
  XtSetArg (arg[ac], XmNleftAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNleftOffset, 10); ac++;
  XtSetArg (arg[ac], XmNbottomAttachment, XmATTACH_WIDGET); ac++;
  XtSetArg (arg[ac], XmNbottomWidget, sep); ac++;
  XtSetArg (arg[ac], XmNbottomOffset, 10); ac++;
  XtSetValues (rmdbaseSel.option_, arg, ac);
  ac = 0;

  /* add callbacks */
  XtAddCallback (rmdbaseSel.ok_, XmNactivateCallback, dbaseSelOk,
		 (XtPointer)&(rmdbaseSel));
  XtAddCallback (rmdbaseSel.cancel_, XmNactivateCallback, dbaseSelCancel,
		 (XtPointer)&(rmdbaseSel));

  /* create top label */
  sprintf (temp, "Removing a database will destroy all stored data\n");
  strcat  (temp, "Are you sure?");
  t = XmStringCreateLtoR (temp, XmSTRING_DEFAULT_CHARSET);
  XtSetArg (arg[ac], XmNtopAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNtopOffset, 10); ac++;
  XtSetArg (arg[ac], XmNleftAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNleftOffset, 2); ac++;
  XtSetArg (arg[ac], XmNrightAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNrightOffset, 2); ac++;
  XtSetArg (arg[ac], XmNbottomAttachment, XmATTACH_WIDGET); ac++;
  XtSetArg (arg[ac], XmNbottomWidget, rmdbaseSel.option_); ac++;
  XtSetArg (arg[ac], XmNbottomOffset, 10); ac++;
  XtSetArg (arg[ac], XmNlabelString, t); ac++;
  
  label = XtCreateManagedWidget ("rmdbaseSelLabel", xmLabelGadgetClass,
				 rmdbaseSel.w_, arg, ac);
  ac = 0;
  XmStringFree (t);


  XtManageChild (actionForm);
  XtManageChild (rmdbaseSel.option_);
  
  /* set default button */
  XtSetArg (arg[ac], XmNdefaultButton, rmdbaseSel.ok_); ac++;
  XtSetValues (rmdbaseSel.w_, arg, ac);
  ac = 0;

  /* set inited flag */
  inited = 1;
}

void
removeDbaseSelPopup (void)
{
  int  status;
  int  i;
  Arg  arg[10];
  int  ac = 0;
  XmString t;

  if (rmdbaseSel.managed_) 
    removeDbaseSelPopdown ();
  
  /* remove old database information */
  for (i = 0; i < rmdbaseSel.numDbases_; i++)
    free (rmdbaseSel.dbasenames_[i]);

  /* get all database names */
  status = listAllDatabases (rmdbaseSel.dbasenames_, &(rmdbaseSel.numDbases_));

  if (status == 0) {
    if (rmdbaseSel.numDbases_ > 0) {
      for (i = 0; i < rmdbaseSel.numDbases_; i++) {
	ac = 0;
	t = XmStringCreateSimple (rmdbaseSel.dbasenames_[i]);
	XtSetArg (arg[ac], XmNlabelString, t); ac++;
	XtSetValues (rmdbaseSel.pushb[i], arg, ac);
	ac = 0;
	XmStringFree (t);
	XtManageChild (rmdbaseSel.pushb[i]);
      }
    }
    else {
      t = XmStringCreateSimple ("no database");
      XtSetArg (arg[ac], XmNlabelString, t); ac++;
      XtSetValues (rmdbaseSel.pushb[0], arg, ac);
      ac = 0;
      XmStringFree (t);
      /* disable ok button */
      XtSetSensitive (rmdbaseSel.ok_, FALSE);
    }
  }
  else
    rmdbaseSel.numDbases_ = 0;
      
  XtManageChild (rmdbaseSel.w_);
  XtPopup (XtParent (rmdbaseSel.w_), XtGrabNone);
  rmdbaseSel.managed_ = 1;
}

void
removeDbaseSelPopdown (void)
{
  int i = 0;

  if (rmdbaseSel.managed_) 
    XtPopdown (XtParent (rmdbaseSel.w_));
  rmdbaseSel.managed_ = 0;
  for (i = 0; i < EDITOR_MAX_DATABASES; i++)
    XtUnmanageChild (rmdbaseSel.pushb[i]);
  /* reset ok button sensitivity */
  XtSetSensitive (rmdbaseSel.ok_, TRUE);
}


	       

