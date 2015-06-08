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
 *	Implementation of CODA experimental run type/database interface
 *	
 * Author:  Jie Chen
 * CEBAF Data Acquisition Group
 *
 * Revision History:
 *   $Log: Editor_miscinfo.c,v $
 *   Revision 1.1.1.1  1996/08/21 19:36:06  heyes
 *   Imported sources
 *
 *	  
 */
#include "Editor_miscinfo.h"
#include "Editor_layout.h"
#include <Xm/Form.h>
#include <Xm/TextF.h>
#include <Xm/LabelG.h>

#if defined (__STDC__)
Widget XcodaEditorCreateInfoWidgets (Widget formParent)
#else
Widget XcodaEditorCreateInfoWidgets (formParent)
     Widget formParent;
#endif
{
  Arg      arg[20];
  int      ac = 0;
  XmString t;
  Widget   hform;
  Widget   typelabel, namelabel;

  /* create holder form widget */
  hform = XtCreateWidget ("runtypehform", xmFormWidgetClass,
			  formParent, NULL, 0);
  ac = 0;

  /* create database label and database name entry field */
  t = XmStringCreateSimple ("Database");
  XtSetArg (arg[ac], XmNlabelString, t); ac++;
  XtSetArg (arg[ac], XmNalignment, XmALIGNMENT_END); ac++;
  XtSetArg (arg[ac], XmNtopAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNbottomAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNleftAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNleftOffset, 45); ac++;
  namelabel = XtCreateManagedWidget ("namelabel", xmLabelGadgetClass,
				     hform, arg, ac);
  ac = 0;
  XmStringFree (t);

  XtSetArg (arg[ac], XmNtopAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNbottomAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNleftAttachment, XmATTACH_WIDGET); ac++;
  XtSetArg (arg[ac], XmNleftWidget, namelabel); ac++;
  XtSetArg (arg[ac], XmNblinkRate, 0); ac++;
  XtSetArg (arg[ac], XmNmarginHeight, 0); ac++;
  XtSetArg (arg[ac], XmNeditable, FALSE); ac++;
  XtSetArg (arg[ac], XmNcursorPositionVisible, FALSE); ac++;
  manager.dbasename = XtCreateManagedWidget ("dbasenameInput",
					     xmTextFieldWidgetClass,
					     hform, arg, ac);
  ac = 0;

  /* create run type label and run type name */
  XtSetArg (arg[ac], XmNtopAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNbottomAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNrightAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNrightOffset, 15); ac++;
  XtSetArg (arg[ac], XmNblinkRate, 0); ac++;
  XtSetArg (arg[ac], XmNmarginHeight, 0); ac++;
  XtSetArg (arg[ac], XmNcolumns, 15); ac++;
  XtSetArg (arg[ac], XmNeditable, FALSE); ac++;
  XtSetArg (arg[ac], XmNcursorPositionVisible, FALSE); ac++;
  manager.runtype = XtCreateManagedWidget ("runtypeInput",
					   xmTextFieldWidgetClass,
					   hform, arg, ac);
  ac = 0;

  t = XmStringCreateSimple ("Run Type");
  XtSetArg (arg[ac], XmNlabelString, t); ac++;
  XtSetArg (arg[ac], XmNalignment, XmALIGNMENT_END); ac++;
  XtSetArg (arg[ac], XmNtopAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNbottomAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNleftAttachment, XmATTACH_WIDGET); ac++;
  XtSetArg (arg[ac], XmNleftWidget, manager.dbasename); ac++;
  XtSetArg (arg[ac], XmNrightAttachment, XmATTACH_WIDGET); ac++;
  XtSetArg (arg[ac], XmNrightWidget, manager.runtype); ac++;
  typelabel = XtCreateManagedWidget ("typelabel", xmLabelGadgetClass,
				     hform, arg, ac);
  ac = 0;
  XmStringFree (t);

  /* set initial value of expt id and name to empty */
  XmTextFieldSetString (manager.runtype, "");

  XtManageChild (hform);
  return hform;
}

#if defined (__STDC__)
void
XcodaEditorShowDatabaseName (char* name)
#else
void
XcodaEditorShowDatabaseName (name)
     char *name;
#endif
{
  Arg arg[20];
  int ac = 0;

  if (name)
    XmTextFieldSetString (manager.dbasename, name);
  else
    XmTextFieldSetString (manager.dbasename, "");
}

#if defined (__STDC__)
void
XcodaEditorShowConfigName   (char* configName)
#else
void
XcodaEditorShowConfigName   (configName)
     char* configName;
#endif
{
  Arg arg[20];
  int ac = 0;

  if (configName)
    XmTextFieldSetString (manager.runtype, configName);
  else
    XmTextFieldSetString (manager.runtype, "");
}


