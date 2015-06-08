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
 *	Implementation of newConfig Dialog Box
 *	
 * Author:  Jie Chen
 * CEBAF Data Acquisition Group
 *
 * Revision History:
 *   $Log: Editor_newConfig.c,v $
 *   Revision 1.1.1.2  1996/11/05 17:45:28  chen
 *   coda source
 *
 *	  
 */
#include "Editor_newConfig.h"
#include <Xm/Xm.h>
#include <Xm/PushBG.h>
#include <Xm/Form.h>
#include <Xm/LabelG.h>
#include <Xm/SeparatoG.h>
#include <Xm/TextF.h>

#include "Editor_database.h"
#include "Editor_miscinfo.h"
#include "Editor_graph.h"

typedef struct _new_config_widgets {
  Widget w_;
  Widget input_;
  Widget ok_;
  Widget cancel_;
}editorNewConfigDialog;

#if defined (__STDC__)
static void newConfig (Widget w, XtPointer data, XmAnyCallbackStruct* cbs)
#else
static void newConfig (w, data, cbs)
     Widget w;
     XtPointer data;
     XmAnyCallbackStruct* cbs;
#endif
{
  char *config;
  char temp[128];
  editorNewConfigDialog* obj = (editorNewConfigDialog *)data;

  config = XmTextFieldGetString (obj->input_);
  if (!config || !*config) {
    XtFree (config);
    pop_error_message ("New configuration name, please !!!", 
		       sw_geometry.draw_area);
    return;
  }

  if (isConfigCreated (config)) {
    /* reset option dialog information */
    XcodaEditorResetOptionDialog ();
    XtFree (config);
    return;
  }
  if (   (createConfigTable (config)   < 0) 
      || (createPositionTable (config) < 0)
      || (createOptionTable (config)   < 0) 
      || (createScriptTable (config)   < 0) ) {
    sprintf (temp, "Cannot create configuration table %s\n", config);
    XtFree (config);
    pop_error_message (temp, sw_geometry.draw_area);
    return;
  }
  else {
    /* reset option dialog information */
    XcodaEditorResetOptionDialog ();

    selectConfigTable (config);
    XcodaEditorShowConfigName (config);
    XtFree (config);
  }
  XtPopdown (XtParent (obj->w_));
}

#if defined (__STDC__)
static void popdownNewConfigDialog (Widget w,
				    XtPointer data,
				    XmAnyCallbackStruct* cbs)
#else
static void popdownNewConfigDialog (w, data, cbs)
     Widget w;
     XtPointer data;
     XmAnyCallbackStruct* cbs;
#endif
{
  editorNewConfigDialog* obj = (editorNewConfigDialog *)data;  
  XtPopdown (XtParent (obj->w_));
}

static editorNewConfigDialog newConfigDialog;

#if defined (__STDC__)
void
newConfigDialogPopup (Widget parent)
#else
void
newConfigDialogPopup (parent)
     Widget parent;
#endif
{
  Arg args[20];
  int ac = 0;
  XmString t;
  Widget act_form, label, sep;
  static int newConfigDCreated = 0;

  if (!newConfigDCreated) {
    newConfigDCreated = 1;
    newConfigDialog.w_ = XmCreateFormDialog (parent,
					     "New Configuration",
					     NULL, 0);
    
    t = XmStringCreateSimple ("Enter new configuration name: ");
    XtSetArg(args[ac], XmNlabelString, t); ac++;
    XtSetArg(args[ac], XmNalignment, XmALIGNMENT_BEGINNING); ac++;
    XtSetArg(args[ac], XmNtopAttachment, XmATTACH_FORM); ac++;
    XtSetArg(args[ac], XmNtopOffset, 10); ac++;
    XtSetArg(args[ac], XmNleftAttachment, XmATTACH_FORM); ac++;
    XtSetArg(args[ac], XmNleftOffset, 5); ac++;
    XtSetArg(args[ac], XmNrightAttachment, XmATTACH_FORM); ac++;
    XtSetArg(args[ac], XmNrightOffset, 2); ac++;
    label = XtCreateManagedWidget("config_top",xmLabelGadgetClass,
				  newConfigDialog.w_, args, ac);
    ac = 0;
    XmStringFree(t);

    /* create all action buttons */
    XtSetArg(args[ac], XmNleftAttachment, XmATTACH_FORM); ac++;
    XtSetArg(args[ac], XmNleftOffset, 2); ac++;
    XtSetArg(args[ac], XmNrightAttachment, XmATTACH_FORM); ac++;
    XtSetArg(args[ac], XmNrightOffset, 2); ac++;
    XtSetArg(args[ac], XmNbottomAttachment, XmATTACH_FORM); ac++;
    XtSetArg(args[ac], XmNbottomOffset, 2); ac++;
    act_form = XtCreateWidget ("newconfig_act_form", xmFormWidgetClass,
			       newConfigDialog.w_, args, ac);
    ac = 0;
    
    t = XmStringCreateSimple("  Ok  ");
    XtSetArg(args[ac], XmNlabelString, t); ac++;
    XtSetArg(args[ac], XmNtopAttachment, XmATTACH_FORM);ac++;
    XtSetArg(args[ac], XmNleftAttachment, XmATTACH_POSITION); ac++;
    XtSetArg(args[ac], XmNleftPosition, 10); ac++;
    XtSetArg(args[ac], XmNbottomAttachment, XmATTACH_FORM); ac++;
    XtSetArg(args[ac], XmNbottomOffset, 5); ac++;
    XtSetArg(args[ac], XmNshowAsDefault, 1); ac++;
    newConfigDialog.ok_ = XtCreateManagedWidget("ok", xmPushButtonGadgetClass,
						act_form, args, ac);
    /* start_save_file */
    ac = 0;
    XmStringFree(t);

    t = XmStringCreateSimple("Cancel");
    XtSetArg(args[ac], XmNlabelString, t); ac++;
    XtSetArg(args[ac], XmNtopAttachment, XmATTACH_FORM);ac++;
    XtSetArg(args[ac], XmNtopOffset, 5); ac++;
    XtSetArg(args[ac], XmNrightAttachment, XmATTACH_POSITION); ac++;
    XtSetArg(args[ac], XmNrightPosition,90); ac++;
    XtSetArg(args[ac], XmNbottomAttachment, XmATTACH_FORM); ac++;
    XtSetArg(args[ac], XmNbottomOffset, 10); ac++;
    newConfigDialog.cancel_ = XtCreateManagedWidget("cancel", 
						    xmPushButtonGadgetClass,
						    act_form, args, ac);
    ac = 0;
    XmStringFree(t);

    XtSetArg(args[ac], XmNleftAttachment, XmATTACH_FORM); ac++;
    XtSetArg(args[ac], XmNleftOffset, 2); ac++;
    XtSetArg(args[ac], XmNrightAttachment, XmATTACH_FORM); ac++;
    XtSetArg(args[ac], XmNrightOffset, 2); ac++;
    XtSetArg(args[ac], XmNbottomAttachment, XmATTACH_WIDGET); ac++;
    XtSetArg(args[ac], XmNbottomWidget, act_form); ac++;
    XtSetArg(args[ac], XmNbottomOffset, 10); ac++;
    sep = XtCreateManagedWidget("sep", xmSeparatorGadgetClass,
				newConfigDialog.w_, args, ac);
    ac = 0;

    /* text input widget */
    XtSetArg(args[ac], XmNleftAttachment, XmATTACH_FORM); ac++;
    XtSetArg(args[ac], XmNleftOffset, 5); ac++;
    XtSetArg(args[ac], XmNrightAttachment, XmATTACH_FORM); ac++;
    XtSetArg(args[ac], XmNrightOffset, 5); ac++;
    XtSetArg(args[ac], XmNbottomAttachment, XmATTACH_WIDGET); ac++;
    XtSetArg(args[ac], XmNbottomWidget, sep); ac++;
    XtSetArg(args[ac], XmNbottomOffset, 10); ac++;
    newConfigDialog.input_ = XtCreateManagedWidget("text_w", 
						   xmTextFieldWidgetClass,
						   newConfigDialog.w_, 
						   args, ac);
    ac = 0;

    /* set resource for label widget */
    XtSetArg (args[ac], XmNbottomAttachment, XmATTACH_WIDGET); ac++;
    XtSetArg (args[ac], XmNbottomWidget, newConfigDialog.input_); ac++;
    XtSetValues (label, args, ac);
    ac = 0;

    /* set default button */
    XtSetArg (args[ac], XmNdefaultButton, newConfigDialog.ok_); ac++;
    XtSetValues (newConfigDialog.w_, args, ac);
    ac = 0;

    /* add callbacks for all push buttons */
    XtAddCallback (newConfigDialog.ok_, XmNactivateCallback,
		   newConfig, (XtPointer)&(newConfigDialog));
    XtAddCallback (newConfigDialog.cancel_, XmNactivateCallback,
		   popdownNewConfigDialog, (XtPointer)&(newConfigDialog));
    
    XtManageChild(act_form);
    XtManageChild(newConfigDialog.w_);
  }
  XtPopup (XtParent (newConfigDialog.w_), XtGrabNone);
}    
   
