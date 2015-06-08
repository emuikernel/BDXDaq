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
 *	Implementation of Editor Script Dialog Box
 *	
 * Author:  Jie Chen
 * CEBAF Data Acquisition Group
 *
 * Revision History:
 *   $Log: Editor_script_dialog.c,v $
 *   Revision 1.1.1.1  1996/11/05 17:45:32  chen
 *   coda source
 *
 *	  
 */
#include "Editor.h"
#include "Editor_script_dialog.h"
#include "Editor_misc.h"
#include <Xm/Form.h>
#include <Xm/LabelG.h>
#include <Xm/TextF.h>
#include <XmTabs.h>

#define EDITOR_NUM_STATES 7
#define EDITOR_MAX_NUM_SCRIPTS 6
#define EDITOR_TAB_HT          30
#define EDITOR_EXTRA_TABS      2

static char* codaStates[]=
{
  {"boot"},
  {"configure"},
  {"download"},
  {"prestart"},
  {"go"},
  {"pause"},
  {"end"}
};

typedef struct _script_widgets
{
  Widget text[EDITOR_MAX_NUM_SCRIPTS];
  Widget tab;
  Widget shell;
  char*  cache[EDITOR_NUM_STATES][EDITOR_MAX_NUM_SCRIPTS];
  drawComp* comp;
}scriptWidget;

static int    firstTime = 1;
static scriptWidget swidgets;
static int    curr = 1;

static void
#if defined (__STDC__)
update_scripts (drawComp* comp)
#else
update_scripts (comp)
     drawComp* comp;
#endif
{
  int created = 0;
  int i, j;
  codaScript* p;

  if (comp->scripts)
    freeCodaScriptList (comp->scripts);
  comp->scripts = 0;

  for (i = 0; i < EDITOR_NUM_STATES; i++) {
    for (j = 0; j < EDITOR_MAX_NUM_SCRIPTS; j++) {
      if (swidgets.cache[i][j]) {
	if (!created) {
	  comp->scripts = newCodaScript (codaStates[i], swidgets.cache[i][j]);
	  created = 1;
	}
	else {
	  p = newCodaScript (codaStates[i], swidgets.cache[i][j]);
	  addCodaScriptToList (comp->scripts, p);
	  /*	  addScriptToList (comp->scripts,
			   codaStates[i], swidgets.cache[i][j]);*/
	}
      }
    }
  }
  /* free all cached information */
  for (i = 0; i < EDITOR_NUM_STATES; i++) {
    for (j = 0; j < EDITOR_MAX_NUM_SCRIPTS; j++) {
      if (swidgets.cache[i][j]) 
	free (swidgets.cache[i][j]);
      swidgets.cache[i][j] = 0;
    }
  }
}
 

static void
#if defined (__STDC__)
reset_tab_resources (drawComp* comp)
#else
reset_tab_resources (comp)
     drawComp* comp;
#endif
{
  Arg arg[20];
  int ac = 0;
  int i, j;
  codaScript* p;
  char* tmp;
  curr = 1;
  

  XtSetArg (arg[ac], XtNlefttabs, curr); ac++;
  XtSetArg (arg[ac], XtNrighttabs, 
	    EDITOR_NUM_STATES + EDITOR_EXTRA_TABS - curr - 1); 
  ac++;
  XtSetValues (swidgets.tab, arg, ac);
  ac = 0;

  /* clean out old script information */
  for (i = 0; i < EDITOR_NUM_STATES; i++) {
    for (j = 0; j < EDITOR_MAX_NUM_SCRIPTS; j++) {
      if (swidgets.cache[i][j])
	free (swidgets.cache[i][j]);
      swidgets.cache[i][j] = 0;
    }
  }
  for (j = 0; j < EDITOR_MAX_NUM_SCRIPTS; j++) 
    XmTextFieldSetString (swidgets.text[j], "");
  
  /* fill all script cache with scripts of the component */
  if (comp->scripts != 0) {
    for (p = comp->scripts; p != 0; p = p->next) {
      for (i = 0; i < EDITOR_NUM_STATES; i++) {
	if (strcasecmp (p->state, codaStates[i]) == 0)
	  break;
      }
      if (i >= EDITOR_NUM_STATES) {
	fprintf (stderr, "Fatal Error: Shit happens\n");
	exit (1);
      }
      for (j = 0; j < EDITOR_MAX_NUM_SCRIPTS; j++) {
	if (swidgets.cache[i][j] == 0) {
	  swidgets.cache[i][j] = strsave (p->script);
	  break;
	}
      }
    }
  }
  /* update information of the text field */
  for (j = 0; j < EDITOR_MAX_NUM_SCRIPTS; j++) {
    if (swidgets.cache[curr - 1][j] != 0) 
      XmTextFieldSetString (swidgets.text[j], swidgets.cache[curr - 1][j]);
  }
}   
  

static void
#if defined (__STDC__)
handle_tab (Widget w, XtPointer data, XtPointer calldata)
#else
handle_tab (w, data, calldata)
     Widget w;
     XtPointer data;
     XtPointer calldata;
#endif
{
  Arg arg[20];
  int ac = 0;
  int state, i, j, tab;
  char* tmp;
  AttrWidgets* atws = (AttrWidgets *)data;

  /* get tab data */
  tab = (int)calldata;

  if (tab == 0)
    return;

  if (curr >= 1 && curr < EDITOR_NUM_STATES + EDITOR_EXTRA_TABS - 1) {
    state = curr - 1;

    /* put old information in the text filed into the cache */
    for (j = 0; j < EDITOR_MAX_NUM_SCRIPTS; j++) {
      if (swidgets.cache[state][j] != 0)
	free (swidgets.cache[state][j]);
      swidgets.cache[state][j] = 0;
      
      tmp = XmTextFieldGetString (swidgets.text[j]);
      if (tmp && *tmp) 
	swidgets.cache[state][j] = strsave (tmp);
      if (tmp)
	XtFree (tmp);
    }
    
    /* clear out old text information */
    for (j = 0; j < EDITOR_MAX_NUM_SCRIPTS; j++) 
      XmTextFieldSetString (swidgets.text[j], "");
  }

  
  /* get new tab value */
  curr += tab;

  XtSetArg (arg[ac], XtNlefttabs, curr); ac++;
  XtSetArg (arg[ac], XtNrighttabs, 
	    EDITOR_NUM_STATES + EDITOR_EXTRA_TABS - curr - 1); 
  ac++;
  XtSetValues (swidgets.tab, arg, ac);
  ac = 0;

  if (curr == 0) { /* Ok button, parent dialog cannot do cancel */
    XtSetSensitive (atws->cancel_widget, False);
    XtRemoveGrab (swidgets.shell);
    XtPopdown (swidgets.shell);
    /* update all scripts */
    update_scripts (swidgets.comp);
  }
  else if (curr == EDITOR_NUM_STATES + EDITOR_EXTRA_TABS - 1) {
    XtRemoveGrab (swidgets.shell);
    XtPopdown (swidgets.shell);
  }
  else {
    state = curr - 1;
    /* update text field with all cached script value */
    for (j = 0; j < EDITOR_MAX_NUM_SCRIPTS; j++) {
      if (swidgets.cache[curr - 1][j] != 0) 
	XmTextFieldSetString (swidgets.text[j], swidgets.cache[curr - 1][j]);
    }
  }
}


void
#if defined (__STDC__)
popup_script_dialog (drawComp* comp, Widget parent, AttrWidgets* atws)
#else
popup_script_dialog (comp, parent, atws)
     drawComp* comp;
     Widget    parent;
     AttrWidgets* atws;
#endif
{
  Arg      arg[20];  
  int      ac = 0;
  int      i, j;
  Widget   form, subform;
  XmString t;
  /* labels used in the tab widget */
  char*    labels[EDITOR_NUM_STATES + EDITOR_EXTRA_TABS];

  /* keep track of component information */
  swidgets.comp = comp;

  if (firstTime) {
    firstTime = 0;
    
    /* clean out all cache for all scripts */
    for (i = 0; i < EDITOR_NUM_STATES; i++) {
      for (j = 0; j < EDITOR_MAX_NUM_SCRIPTS; j++)
	swidgets.cache[i][j] = 0;
    }
    
    form = XmCreateFormDialog (parent, "scriptDialog", NULL, 0);
    
    swidgets.shell = XtParent (form);
  
    j = 1;
    for (i = 0; i < EDITOR_NUM_STATES; i++) 
      labels[j++] = strsave (codaStates[i]);

    /* I know extra tabs = 2 */
    labels[j++] = strsave ("Dismiss");
    labels[0] = strsave ("  Ok   ");

    XtSetArg (arg[ac], XtNlabels, labels); ac++;
    XtSetArg (arg[ac], XtNlefttabs, 1); ac++;
    XtSetArg (arg[ac], XtNrighttabs, EDITOR_NUM_STATES); ac++;
    XtSetArg (arg[ac], XtNorientation, XfwfUpTabs); ac++;
    XtSetArg (arg[ac], XmNtopAttachment, XmATTACH_FORM); ac++;
    XtSetArg (arg[ac], XmNtopOffset, 5); ac++;
    XtSetArg (arg[ac], XmNleftAttachment, XmATTACH_FORM); ac++;
    XtSetArg (arg[ac], XmNleftOffset, 5); ac++;
    XtSetArg (arg[ac], XmNrightAttachment, XmATTACH_FORM); ac++;
    XtSetArg (arg[ac], XmNrightOffset, 5); ac++;
    XtSetArg (arg[ac], XmNbottomAttachment, XmATTACH_OPPOSITE_FORM); ac++;
    XtSetArg (arg[ac], XmNbottomOffset, (-1)*EDITOR_TAB_HT); ac++;
    swidgets.tab = XtCreateManagedWidget ("scriptTab", xmTabsWidgetClass,
					  form, arg, ac);
    ac = 0;

    XtSetArg (arg[ac], XmNtopAttachment, XmATTACH_WIDGET); ac++;
    XtSetArg (arg[ac], XmNtopWidget, swidgets.tab); ac++;
    XtSetArg (arg[ac], XmNleftAttachment, XmATTACH_FORM); ac++;
    XtSetArg (arg[ac], XmNleftOffset, 5); ac++;
    XtSetArg (arg[ac], XmNrightAttachment, XmATTACH_FORM); ac++;
    XtSetArg (arg[ac], XmNrightOffset, 5); ac++;
    XtSetArg (arg[ac], XmNbottomAttachment, XmATTACH_FORM); ac++;
    XtSetArg (arg[ac], XmNbottomOffset, 5); ac++;
    subform = XtCreateWidget ("subform", xmFormWidgetClass,
			      form, arg, ac);
    ac = 0;
      
    /* create all text input fields */
    for (j = 0; j < EDITOR_MAX_NUM_SCRIPTS; j++) {
      if (j == 0) {
	XtSetArg (arg[ac], XmNtopAttachment, XmATTACH_FORM); ac++;
      }
      else {
	XtSetArg (arg[ac], XmNtopAttachment, XmATTACH_WIDGET); ac++;
	XtSetArg (arg[ac], XmNtopWidget, swidgets.text[j-1]); ac++;
      }
      XtSetArg (arg[ac], XmNtopOffset, 2); ac++;
      XtSetArg (arg[ac], XmNleftAttachment, XmATTACH_FORM); ac++;
      XtSetArg (arg[ac], XmNleftOffset, 2); ac++;
      XtSetArg (arg[ac], XmNrightAttachment, XmATTACH_FORM); ac++;
      XtSetArg (arg[ac], XmNrightOffset, 2); ac++;
      if (j == EDITOR_MAX_NUM_SCRIPTS - 1) {
	XtSetArg (arg[ac], XmNbottomAttachment, XmATTACH_FORM); ac++;
	XtSetArg (arg[ac], XmNbottomOffset, 2); ac++;
      }
      XtSetArg (arg[ac], XmNcolumns, 80); ac++;
      XtSetArg (arg[ac], XmNeditable, True); ac++;
      swidgets.text[j] = XtCreateManagedWidget ("scriptInput",
						xmTextFieldWidgetClass,
						subform,
						arg, ac);
      ac = 0;
    }
    XtManageChild (subform);
    /* add all callbacks */
    XtAddCallback (swidgets.tab, XtNactivateCallback, handle_tab, 
		   (XtPointer)atws);
    XtManageChild (form);
  }

  /* reset tab to boot initial state */
  reset_tab_resources (comp);
  /* popup the whole dialog          */
  XtAddGrab (swidgets.shell, True, False);
  XtPopup (swidgets.shell, XtGrabNone);
}
