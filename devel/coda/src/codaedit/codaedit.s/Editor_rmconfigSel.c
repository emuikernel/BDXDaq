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
 *	Implementation of Editor_configSel
 *	
 * Author:  Jie Chen
 * CEBAF Data Acquisition Group
 *
 * Revision History:
 *   $Log: Editor_rmconfigSel.c,v $
 *   Revision 1.1.1.2  1996/11/05 17:45:29  chen
 *   coda source
 *
 *	  
 */
#include "Editor_rmconfigSel.h"
#include <Xm/Xm.h>
#include <Xm/PushBG.h>
#include <Xm/Form.h>
#include <Xm/RowColumn.h>
#include <Xm/SeparatoG.h>
#include <Xm/LabelG.h>

#include "Editor.h"
#include "Editor_graph.h"

static editorRmConfigSel  rmconfigSel;

#if defined (__STDC__)
static void
removeConfigSelPopdown (void)
#else
static void
removeConfigSelPopdown ()
#endif
{
  int i = 0;

  if (rmconfigSel.managed_) 
    XtPopdown (XtParent (rmconfigSel.w_));
  rmconfigSel.managed_ = 0;
  for (i = 0; i < EDITOR_MAX_DATABASES; i++)
    XtUnmanageChild (rmconfigSel.pushb[i]);
  /* reset ok button sensitivity */
  XtSetSensitive (rmconfigSel.ok_, TRUE);
}


#if defined (__STDC__)
static Widget 
createOptionMenu (Widget parent)
#else
static Widget 
createOptionMenu (parent)
     Widget parent;
#endif
{
  Arg arg[20];
  int ac = 0;
  XmString t;
  Widget menu;
  Widget option;
  int    i = 0;

  menu = XmCreatePulldownMenu (parent, "optionPullDown", NULL, 0);

  t = XmStringCreateSimple ("Run Type ");
  XtSetArg (arg[ac], XmNlabelString, t); ac++;
  XtSetArg (arg[ac], XmNsubMenuId, menu); ac++;
  option = XmCreateOptionMenu (parent, "configurationOption", arg, ac);
  ac = 0;
  XmStringFree (t);

  t = XmStringCreateSimple ("              ");
  XtSetArg (arg[ac], XmNlabelString, t); ac++;
  for (i = 0; i < EDITOR_MAX_CONFIGS; i++) {
    rmconfigSel.pushb[i] = XtCreateWidget ("optionButton", 
					 xmPushButtonGadgetClass,
					 menu, arg, ac);
    if (i == 0)
      XtManageChild (rmconfigSel.pushb[i]);
  }
  ac = 0;
  XmStringFree (t);
  
  return option;
}

#if defined (__STDC__)
static void
rmconfigSelOk (Widget w, XtPointer data, XmAnyCallbackStruct* cbs)
#else
static void
rmconfigSelOk (w, data, cbs)
     Widget w;
     XtPointer data;
     XmAnyCallbackStruct* cbs;
#endif
{
  int  i = 0;
  char* currconfig;
  Arg arg[20];
  int ac = 0;
  Widget curr;
  char  temp[128];
  rcNetComp* daq_list[MAX_NUM_COMPS];
  int        num_comps;
  ConfigInfo* configs[EDITOR_MAX_CONFIGS];
  int        num_configs;

  editorRmConfigSel* sel = (editorRmConfigSel *)data;

  /* popdown dialog first */
  removeConfigSelPopdown ();

  
  /* get current selection */
  XtSetArg (arg[ac], XmNmenuHistory, &curr); ac++;
  XtGetValues (sel->option_, arg, ac);
  ac = 0;
  
  for (i = 0; i < sel->numConfigs_; i++) {
    if (curr == sel->pushb[i])
      break;
  }
  currconfig = sel->configs_[i];

  if (currentConfigTable ()) {
    if (strcmp (currconfig, currentConfigTable ()) == 0)
      delete_everything (w, data, cbs);
    /* reset selected config table */
    removeMiscConfigInfo ();
    XcodaEditorShowConfigName (0);
  }
  
  if (removeConfigTable (currconfig) < 0) {
    sprintf (temp, "Cannot remove configuration: %s", currconfig);
    pop_error_message (temp, sw_geometry.draw_area);
  }
}

#if defined (__STDC__)
static void
rmconfigSelCancel (Widget w, XtPointer data, XmAnyCallbackStruct* cbs)
#else
static void
rmconfigSelCancel (w, data, cbs)
     Widget w;
     XtPointer data;
     XmAnyCallbackStruct* cbs;
#endif
{
  removeConfigSelPopdown ();
}
  
#if defined (__STDC__)
void
initRemoveConfigSel (Widget parent)
#else
void
initRemoveConfigSel (parent)
     Widget parent;
#endif
{
  Arg arg[20];
  int ac = 0;
  XmString t;
  char temp[128];
  Widget actionForm, sep, label;
  static int removeConfigSelInited = 0;
  
  if (removeConfigSelInited) 
    return;

  rmconfigSel.numConfigs_ = 0;
  rmconfigSel.managed_ = 0;
  rmconfigSel.w_ = 0;
  rmconfigSel.option_ = 0;
  rmconfigSel.ok_ = 0;
  rmconfigSel.cancel_ = 0;

  /* create all widgets */
  XtSetArg (arg[ac], XmNtitle, "Run Type Selection"); ac++;
  XtSetArg (arg[ac], XmNautoUnmanage, FALSE); ac++;
  rmconfigSel.w_ = XmCreateFormDialog (parent, "editorRmConfigSel",
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
			       xmFormWidgetClass, rmconfigSel.w_,
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
  rmconfigSel.ok_ = XtCreateManagedWidget ("runTypeOk", xmPushButtonGadgetClass,
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
  rmconfigSel.cancel_ = XtCreateManagedWidget ("cancel", xmPushButtonGadgetClass,
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
  sep = XtCreateManagedWidget ("sep", xmSeparatorGadgetClass, rmconfigSel.w_,
			       arg, ac);
  ac = 0;
					  
  

  /* create optin menu */
  rmconfigSel.option_ = createOptionMenu (rmconfigSel.w_);
  XtSetArg (arg[ac], XmNleftAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNleftOffset, 10); ac++;
  XtSetArg (arg[ac], XmNbottomAttachment, XmATTACH_WIDGET); ac++;
  XtSetArg (arg[ac], XmNbottomWidget, sep); ac++;
  XtSetArg (arg[ac], XmNbottomOffset, 10); ac++;
  XtSetValues (rmconfigSel.option_, arg, ac);
  ac = 0;


  sprintf (temp, "Removing a configuration will destroy all stored data\n");
  strcat  (temp, "Are you sure ?");
  t = XmStringCreateLtoR (temp, XmSTRING_DEFAULT_CHARSET);
  XtSetArg (arg[ac], XmNlabelString, t); ac++;
  XtSetArg (arg[ac], XmNtopAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNtopOffset, 20); ac++;
  XtSetArg (arg[ac], XmNleftAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNleftOffset, 10); ac++;
  XtSetArg (arg[ac], XmNrightAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNrightOffset, 10); ac++;
  XtSetArg (arg[ac], XmNbottomAttachment, XmATTACH_WIDGET); ac++;
  XtSetArg (arg[ac], XmNbottomWidget, rmconfigSel.option_); ac++;
  XtSetArg (arg[ac], XmNbottomOffset, 10); ac++;
  label = XtCreateManagedWidget ("rmConfigSellabel", xmLabelGadgetClass,
				 rmconfigSel.w_, arg, ac);
  ac = 0;
  XmStringFree (t);


  /* add callbacks */
  XtAddCallback (rmconfigSel.ok_, XmNactivateCallback, rmconfigSelOk,
		 (XtPointer)&(rmconfigSel));
  XtAddCallback (rmconfigSel.cancel_, XmNactivateCallback, rmconfigSelCancel,
		 (XtPointer)&(rmconfigSel));


  XtManageChild (actionForm);
  XtManageChild (rmconfigSel.option_);
  
  /* set default button */
  XtSetArg (arg[ac], XmNdefaultButton, rmconfigSel.ok_); ac++;
  XtSetValues (rmconfigSel.w_, arg, ac);
  ac = 0;

  /* set inited flag */
  removeConfigSelInited = 1;
}

#if defined (__STDC__)
void
removeConfigSelPopup (void)
#else
void
removeConfigSelPopup ()
#endif
{
  int  status;
  int  i;
  Arg  arg[10];
  int  ac = 0;
  XmString t;

  if (rmconfigSel.managed_) 
    removeConfigSelPopdown ();
  
  /* remove old database information */
  for (i = 0; i < rmconfigSel.numConfigs_; i++)
    free (rmconfigSel.configs_[i]);

  /* get all database names */
  status = listAllConfigs (rmconfigSel.configs_, &(rmconfigSel.numConfigs_));

  if (status == 0) {
    if (rmconfigSel.numConfigs_ > 0) {
      for (i = 0; i < rmconfigSel.numConfigs_; i++) {
	ac = 0;
	t = XmStringCreateSimple (rmconfigSel.configs_[i]);
	XtSetArg (arg[ac], XmNlabelString, t); ac++;
	XtSetValues (rmconfigSel.pushb[i], arg, ac);
	ac = 0;
	XmStringFree (t);
	XtManageChild (rmconfigSel.pushb[i]);
      }
    }
    else {
      t = XmStringCreateSimple ("no runtypes");
      XtSetArg (arg[ac], XmNlabelString, t); ac++;
      XtSetValues (rmconfigSel.pushb[0], arg, ac);
      ac = 0;
      XmStringFree (t);
      /* disable ok button */
      XtSetSensitive (rmconfigSel.ok_, FALSE);
    }
  }
  else
    rmconfigSel.numConfigs_ = 0;
      
  XtManageChild (rmconfigSel.w_);
  XtPopup (XtParent (rmconfigSel.w_), XtGrabNone);
  rmconfigSel.managed_ = 1;
}

