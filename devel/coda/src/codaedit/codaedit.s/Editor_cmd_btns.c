/*----------------------------------------------------------------------------- * copyright (c) 1991,1992 Southeastern Universities Research Association,
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
 *	availabel command pushbuttons for config editor
 *	
 * Author:  Jie Chen, CEBAF Data Acquisition Group
 *
 * Revision History:
 *   $Log: Editor_cmd_btns.c,v $
 *   Revision 1.4  1997/09/08 15:19:10  heyes
 *   fix dd icon etc
 *
 *   Revision 1.3  1997/08/29 12:25:26  heyes
 *   fixed window positioning
 *
 *   Revision 1.2  1997/06/20 16:59:46  heyes
 *   clean up GUI!
 *
 *   Revision 1.1.1.2  1996/11/05 17:45:04  chen
 *   coda source
 *
 *	  
 */
#include <stdio.h>
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <Xm/Xm.h>
#include <Xm/Label.h>
#include <Xm/PushB.h>
#include <Xm/Form.h>
#include <Xm/CascadeB.h>

#include "Editor_pixmap.h"
#include "Editor_layout.h"
#include "Editor_graph.h"

#define EDITOR_MAX_NEW_TYPES 20

/* static variables for user defined types */
static Widget ed_new_types[EDITOR_MAX_NEW_TYPES];
static char   ed_new_type_names[EDITOR_MAX_NEW_TYPES][80];
static int    ed_new_numtypes = 0;

#if defined (__STDC__)
static Widget XcodaConfigRocButton(Widget parent)
#else
static Widget XcodaConfigRocButton(parent)
     Widget parent;
#endif
{
  Widget pb;
  Arg    args[5];

  XtSetArg(args[0], XmNlabelType,  XmPIXMAP);
  XtSetArg(args[1], XmNlabelPixmap,btn_pixmaps.roc_btn);
  pb = XtCreateManagedWidget("roc_button", xmPushButtonWidgetClass,
		      parent, args, 2);
  manager.roc_btn = pb;
  return pb;
}

#if defined (__STDC__)
static Widget XcodaConfigEbButton(Widget parent)
#else
static Widget XcodaConfigEbButton(parent)
     Widget parent;
#endif
{
  Widget pb;
  Arg    args[5];

  XtSetArg(args[0], XmNlabelType,  XmPIXMAP);
  XtSetArg(args[1], XmNlabelPixmap,btn_pixmaps.eb_btn);
  pb = XtCreateManagedWidget("eb_button", xmPushButtonWidgetClass,
		      parent, args, 2);
  manager.eb_btn = pb;
  return pb;
}

#if defined (__STDC__)
static Widget XcodaConfigERButton(Widget parent)
#else
static Widget XcodaConfigERButton(parent)
     Widget parent;
#endif
{
  Widget pb;
  Arg    args[5];

  XtSetArg(args[0], XmNlabelType,  XmPIXMAP);
  XtSetArg(args[1], XmNlabelPixmap,btn_pixmaps.er_btn);
  pb = XtCreateManagedWidget("eb_button", xmPushButtonWidgetClass,
		      parent, args, 2);
  manager.er_btn = pb;
  return pb;
}

#if defined (__STDC__)
static Widget XcodaConfigFIButton(Widget parent)
#else
static Widget XcodaConfigFIButton(parent)
     Widget parent;
#endif
{
  Widget pb;
  Arg    args[5];

  XtSetArg(args[0], XmNlabelType,  XmPIXMAP);
  XtSetArg(args[1], XmNlabelPixmap,btn_pixmaps.fi_btn);
  pb = XtCreateManagedWidget("eb_button", xmPushButtonWidgetClass,
		      parent, args, 2);
  manager.fi_btn = pb;
  return pb;
}

#if defined (__STDC__)
static Widget XcodaConfigCFIButton(Widget parent)
#else
static Widget XcodaConfigCFIButton(parent)
     Widget parent;
#endif
{
  Widget pb;
  Arg    args[5];

  XtSetArg(args[0], XmNlabelType,  XmPIXMAP);
  XtSetArg(args[1], XmNlabelPixmap,btn_pixmaps.cfi_btn);
  pb = XtCreateManagedWidget("eb_button", xmPushButtonWidgetClass,
		      parent, args, 2);
  manager.cfi_btn = pb;
  return pb;
}


#if defined (__STDC__)
static Widget XcodaConfigDBGButton(Widget parent)
#else
static Widget XcodaConfigDBGButton(parent)
     Widget parent;
#endif
{
  Widget pb;
  Arg    args[5];

  XtSetArg(args[0], XmNlabelType,  XmPIXMAP);
  XtSetArg(args[1], XmNlabelPixmap,btn_pixmaps.dbg_btn);
  pb = XtCreateManagedWidget("eb_button", xmPushButtonWidgetClass,
		      parent, args, 2);
  manager.dbg_btn = pb;
  return pb;
}

#if defined (__STDC__)
static Widget XcodaConfigDDButton(Widget parent)
#else
static Widget XcodaConfigDDButton(parent)
     Widget parent;
#endif
{
  Widget pb;
  Arg    args[5];

  XtSetArg(args[0], XmNlabelType,  XmPIXMAP);
  XtSetArg(args[1], XmNlabelPixmap,btn_pixmaps.dd_btn);
  pb = XtCreateManagedWidget("dd_button", xmPushButtonWidgetClass,
		      parent, args, 2);
  manager.dd_btn = pb;
  return pb;
}

#if defined (__STDC__)
static Widget XcodaConfigNoneButton(Widget parent)
#else
static Widget XcodaConfigNoneButton(parent)
     Widget parent;
#endif
{
  Widget pb;
  Arg    args[5];

  XtSetArg(args[0], XmNlabelType,  XmPIXMAP);
  XtSetArg(args[1], XmNlabelPixmap,btn_pixmaps.trash);
  pb = XtCreateManagedWidget("none_button", xmPushButtonWidgetClass,
		      parent, args, 2);
  manager.none_btn = pb;
  return pb;
}

#if defined (__STDC__)
static void newTypeSelected (Widget w, XtPointer data, 
			     XmAnyCallbackStruct *cbs)
#else
static void newTypeSelected (w, data, cbs)
     Widget w;
     XtPointer data;
     XmAnyCallbackStruct *cbs;
#endif
{
  int index = (int)data;

  printf ("index is %d\n", index);
}

#if defined (__STDC__)
static Widget XcodaConfigTrigButton(Widget parent)
#else
static Widget XcodaConfigTrigButton(parent)
     Widget parent;
#endif
{
  Widget pb;
  Arg    args[5];

  XtSetArg(args[0], XmNlabelType,  XmPIXMAP);
  XtSetArg(args[1], XmNlabelPixmap,btn_pixmaps.trig_btn);
  pb = XtCreateManagedWidget("roc_button", xmPushButtonWidgetClass,
		      parent, args, 2);
  manager.trig_btn = pb;
  return pb;
}

/* add new type to the type list */
#if defined (__STDC__)
void XcodaEditorAddNewType (char* type)
#else
void XcodaEditorAddNewType (type)
     char* type;
#endif
{
  Arg arg[10];
  int ac = 0;
  XmString t;

  if (ed_new_numtypes >= EDITOR_MAX_NEW_TYPES) {
    fprintf (stderr, "Fatal: Maximum number of new defined types is %d\n",
	     EDITOR_MAX_NEW_TYPES);
    return;
  }

  t = XmStringCreateSimple (type);
  XtSetArg (arg[ac], XmNlabelString, t); ac++;
  XtSetValues (ed_new_types[ed_new_numtypes], arg, ac); ac++;
  ac = 0;
  XmStringFree (t);

  strncpy (ed_new_type_names[ed_new_numtypes], type, 
	   sizeof (ed_new_type_names[ed_new_numtypes]));
  ed_new_numtypes++;
}

/* return form widget which serves as a container of all command buttons*/
#if defined (__STDC__)
Widget XcodaEditorNewButtons(Widget parent)
#else
Widget XcodaEditorNewButtons(parent)
     Widget parent;
#endif
{
  Widget form;
  Widget pushb[20];
  Arg    args[20];
  int    ac;

  ac = 0;
  form = XtCreateWidget("cmd_form",xmFormWidgetClass,parent, NULL, 0);
  ac = 0;

  pushb[0] = XcodaConfigTrigButton(form);
  pushb[1] = XcodaConfigRocButton(form);
  pushb[2] = XcodaConfigEbButton(form);
  pushb[3] = XcodaConfigERButton(form);
  pushb[4] = XcodaConfigFIButton(form);
  pushb[5] = XcodaConfigCFIButton(form);
  pushb[6] = XcodaConfigDBGButton(form);
  pushb[7] = XcodaConfigDDButton(form);
  pushb[8] = XcodaConfigNoneButton(form);


  XtSetArg(args[ac], XmNtopAttachment, XmATTACH_FORM); ac++;
  XtSetArg(args[ac], XmNleftAttachment, XmATTACH_FORM); ac++;
  XtSetArg(args[ac], XmNrightAttachment, XmATTACH_FORM); ac++;
  XtSetValues(pushb[0], args, ac);
  ac = 0;

  XtSetArg(args[ac], XmNtopAttachment, XmATTACH_WIDGET); ac++;
  XtSetArg(args[ac], XmNtopWidget, pushb[0]); ac++;
  XtSetArg(args[ac], XmNleftAttachment, XmATTACH_FORM); ac++;
  XtSetArg(args[ac], XmNrightAttachment, XmATTACH_FORM); ac++;
  XtSetValues(pushb[1], args, ac);
  ac = 0;

  XtSetArg(args[ac], XmNtopAttachment, XmATTACH_WIDGET); ac++;
  XtSetArg(args[ac], XmNtopWidget, pushb[1]); ac++;
  XtSetArg(args[ac], XmNleftAttachment, XmATTACH_FORM); ac++;
  XtSetArg(args[ac], XmNrightAttachment, XmATTACH_FORM); ac++;
  XtSetValues(pushb[2], args, ac);
  ac = 0;

  XtSetArg(args[ac], XmNtopAttachment, XmATTACH_WIDGET); ac++;
  XtSetArg(args[ac], XmNtopWidget, pushb[2]); ac++;
  XtSetArg(args[ac], XmNleftAttachment, XmATTACH_FORM); ac++;
  XtSetArg(args[ac], XmNrightAttachment, XmATTACH_FORM); ac++;
  XtSetValues(pushb[3], args, ac);
  ac = 0;

  XtSetArg(args[ac], XmNtopAttachment, XmATTACH_WIDGET); ac++;
  XtSetArg(args[ac], XmNtopWidget, pushb[3]); ac++;
  XtSetArg(args[ac], XmNleftAttachment, XmATTACH_FORM); ac++;
  XtSetArg(args[ac], XmNrightAttachment, XmATTACH_FORM); ac++;
  XtSetValues(pushb[4], args, ac);
  ac = 0;

  XtSetArg(args[ac], XmNtopAttachment, XmATTACH_WIDGET); ac++;
  XtSetArg(args[ac], XmNtopWidget, pushb[4]); ac++;
  XtSetArg(args[ac], XmNleftAttachment, XmATTACH_FORM); ac++;
  XtSetArg(args[ac], XmNrightAttachment, XmATTACH_FORM); ac++;
  XtSetValues(pushb[5], args, ac);
  ac = 0;

  XtSetArg(args[ac], XmNtopAttachment, XmATTACH_WIDGET); ac++;
  XtSetArg(args[ac], XmNtopWidget, pushb[5]); ac++;
  XtSetArg(args[ac], XmNleftAttachment, XmATTACH_FORM); ac++;
  XtSetArg(args[ac], XmNrightAttachment, XmATTACH_FORM); ac++;
  XtSetValues(pushb[6], args, ac);
  ac = 0;

  XtSetArg(args[ac], XmNtopAttachment, XmATTACH_WIDGET); ac++;
  XtSetArg(args[ac], XmNtopWidget, pushb[6]); ac++;
  XtSetArg(args[ac], XmNleftAttachment, XmATTACH_FORM); ac++;
  XtSetArg(args[ac], XmNrightAttachment, XmATTACH_FORM); ac++;
  XtSetValues(pushb[7], args, ac);
  ac = 0;

  XtSetArg(args[ac], XmNtopAttachment, XmATTACH_WIDGET); ac++;
  XtSetArg(args[ac], XmNtopWidget, pushb[7]); ac++;
  XtSetArg(args[ac], XmNbottomAttachment, XmATTACH_FORM); ac++;
  XtSetArg(args[ac], XmNleftAttachment, XmATTACH_FORM); ac++;
  XtSetArg(args[ac], XmNrightAttachment, XmATTACH_FORM); ac++;
  XtSetValues(pushb[8], args, ac);
  ac = 0;

  XtManageChild(form);
  
  return form;
}





