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
 *	Implementation of openDbase DialogBox
 *	
 * Author:  Jie Chen
 * CEBAF Data Acquisition Group
 *
 * Revision History:
 *   $Log: Editor_opendbaseDialog.c,v $
 *   Revision 1.2  1997/07/09 13:03:41  heyes
 *   back to normal
 *
 *   Revision 1.1.1.1  1996/08/21 19:36:07  heyes
 *   Imported sources
 *
 *	  
 */
#include "Editor_opendbaseDialog.h"
#include <Xm/Xm.h>
#include <Xm/MessageB.h>

#include "Editor_dbaseSel.h"
#include "Editor_graph.h"

#if defined (__STDC__)
static void
openDatabase (Widget w, XtPointer data, XmAnyCallbackStruct* cbs)
#else
static void
openDatabase (w, data, cbs)
     Widget w;
     XtPointer data;
     XmAnyCallbackStruct* cbs;
#endif
{
  int type = (int)data;

  delete_everything(w, data, cbs);
  delete_everyicons( );
  XcodaEditorDisableInput ();
  /* set current database and configuration to null */
  XcodaEditorShowDatabaseName (0);
  XcodaEditorShowConfigName   (0);
  /* tell database handler to clean up */
  cleanDatabaseMiscInfo ();  

  if (type == 0)  /* new database */
    XcodaEditorNewDbaseEntry (sw_geometry.draw_area);
  else {          /* select old database */
    initDbaseSel (sw_geometry.draw_area);
    dbaseSelPopup ();
  }
}

#if defined (__STDC__)
static void 
cancelOpenDatabase (Widget w, 
		    XtPointer data, 
		    XmAnyCallbackStruct* cbs)
#else
static void 
cancelOpenDatabase (w, data, cbs)
     Widget w;
     XtPointer data;
     XmAnyCallbackStruct* cbs;
#endif
{
  XtPopdown (XtParent (w));
}

#if defined (__STDC__)
void opendbaseDialogPopup (Widget parent, int type)
#else
void opendbaseDialogPopup (parent, type)
     Widget parent;
     int    type;
#endif
{
  static Widget dialog = 0;
  XmString yes, no, label;
  char     temp[255];
  Arg arg[20];
  int ac = 0;
			  

  if (!dialog) {
    sprintf (temp, "Open a database will erase current drawing\n");
    strcat  (temp, "Are you sure ?");
    label = XmStringCreateLtoR (temp, XmSTRING_DEFAULT_CHARSET);
    yes = XmStringCreateSimple ("Yes");
    no = XmStringCreateSimple  ("No");

    XtSetArg (arg[ac], XmNmessageString, label); ac++;
    XtSetArg (arg[ac], XmNokLabelString, yes); ac++;
    XtSetArg (arg[ac], XmNcancelLabelString, no); ac++;
    dialog = XmCreateQuestionDialog (parent, "Dialog", arg, ac);
    ac = 0;
    XmStringFree (yes);
    XmStringFree (no);
    XmStringFree (label);

    XtAddCallback (dialog, XmNokCallback, openDatabase, (XtPointer)type);
    XtAddCallback (dialog, XmNcancelCallback, cancelOpenDatabase, 
		   (XtPointer)0);

    XtManageChild (dialog);
  }
  XtPopup (XtParent (dialog), XtGrabNone);
}

