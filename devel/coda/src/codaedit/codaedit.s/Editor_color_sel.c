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
 *	 Color Selector
 *	
 * Author:  Jie Chen, CEBAF Data Acquisition Group
 *
 * Revision History:
 *   $Log: Editor_color_sel.c,v $
 *   Revision 1.1.1.1  1996/08/21 19:36:02  heyes
 *   Imported sources
 *
 *
 */

#include <stdio.h>
#include <X11/Xatom.h>
#include <X11/Intrinsic.h>
#include <Xm/Xm.h>
#include <Xm/RowColumn.h>
#include <Xm/Scale.h>
#include <Xm/Form.h>
#include <Xm/Frame.h>
#include <Xm/TextF.h>
#include <Xm/PushB.h>
#include <Xm/Label.h>
#include <Xm/DrawingA.h>
#include <Xm/ToggleB.h>

#include "Editor_color.h"
#include "Editor_widget_poll.h"

/******************************************************************************
 *         Toggle Buttons for foreground or background color                  *
 *****************************************************************************/
typedef struct _fg_bg_toggles
{
#if defined (__STDC__)
  /* constructor              */
  void (*construct)(struct _fg_bg_toggles* toggles,
		    Widget parent);
#else
  void (*construct)();           /* constructor              */
#endif
  Widget fg_toggle, bg_toggle;   /* internal toggle widgests */
  Widget _w;                     /* container                */
  int    type;                   /* type = 0, foreground, 1 background */
#if defined (__STDC__)
  /* callback routine for toggle button */
  void   (*fg_toggled)(Widget w,
		       XtPointer data,
		       XmToggleButtonCallbackStruct* cbs);
  void   (*bg_toggled)(Widget w,
		       XtPointer data,
		       XmToggleButtonCallbackStruct* cbs);
#else
  /* callback routine for toggle button */
  void   (*fg_toggled)();
  void   (*bg_toggled)();
#endif
}typeSel;

static typeSel typeToggles;

#if defined (__STDC__)
static void fgToggleCallback(Widget w, 
			     XtPointer client_data, 
			     XmToggleButtonCallbackStruct* cbs)
#else
static void fgToggleCallback(w, client_data, cbs)
     Widget w;
     XtPointer client_data;
     XmToggleButtonCallbackStruct *cbs;
#endif
{
  Arg     arg[10];
  int     ac = 0;
  typeSel *toggles = (typeSel *)client_data;

  if (cbs->set){      /* foreground Toggle set */
    toggles->type = 0;
    XtSetArg (arg[ac], XmNset, False); ac++;
    XtSetValues (toggles->bg_toggle, arg, ac);
    ac = 0;
  }
  else{
    toggles->type = 1;
    XtSetArg (arg[ac], XmNset, True); ac++;
    XtSetValues (toggles->bg_toggle, arg, ac);
    ac = 0;
  }
}

#if defined (__STDC__)
static void bgToggleCallback(Widget w, 
			     XtPointer client_data, 
			     XmToggleButtonCallbackStruct* cbs)
#else
static void bgToggleCallback(w, client_data, cbs)
     Widget w;
     XtPointer client_data;
     XmToggleButtonCallbackStruct *cbs;
#endif
{
  Arg     arg[10];
  int     ac = 0;
  typeSel *toggles = (typeSel *)client_data;

  if (cbs->set){      /* background toggle set */
    toggles->type = 1;
    XtSetArg (arg[ac], XmNset, False); ac++;
    XtSetValues (toggles->fg_toggle, arg, ac);
    ac = 0;
  }
  else{
    toggles->type = 0;
    XtSetArg (arg[ac], XmNset, True); ac++;
    XtSetValues (toggles->fg_toggle, arg, ac);
    ac = 0;
  }
}

#if defined (__STDC__)
static void constructToggles(typeSel* toggles, Widget parent)
#else
static void constructToggles(toggles, parent)
     typeSel *toggles;
     Widget  parent;
#endif
{
  int      ac = 0;
  Arg      arg[10];
  XmString t;

  toggles->_w = XtCreateWidget ("toggle_form", xmFormWidgetClass,
				parent, NULL, 0);
  t = XmStringCreateSimple ("Foreground");
  XtSetArg (arg[ac], XmNlabelString, t); ac++;
  XtSetArg (arg[ac], XmNset, False); ac++;
  XtSetArg (arg[ac], XmNindicatorType, XmONE_OF_MANY); ac++;
  XtSetArg (arg[ac], XmNtopAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNbottomAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNleftAttachment, XmATTACH_FORM); ac++;
  toggles->fg_toggle = XtCreateManagedWidget ("fg_toggles",
					      xmToggleButtonWidgetClass,
					      toggles->_w,
					      arg, ac);
  ac = 0;
  XmStringFree (t);

  t = XmStringCreateSimple ("Background");
  XtSetArg (arg[ac], XmNlabelString, t); ac++;
  XtSetArg (arg[ac], XmNset, True); ac++;
  XtSetArg (arg[ac], XmNindicatorType, XmONE_OF_MANY); ac++;
  XtSetArg (arg[ac], XmNtopAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNbottomAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNrightAttachment, XmATTACH_FORM); ac++;
  toggles->bg_toggle = XtCreateManagedWidget ("bg_toggles",
					      xmToggleButtonWidgetClass, 
					      toggles->_w,
					      arg, ac);
  ac = 0;
  XmStringFree (t);

  toggles->fg_toggled = fgToggleCallback;
  toggles->bg_toggled = bgToggleCallback;

  XtAddCallback (toggles->fg_toggle, XmNvalueChangedCallback,
		 toggles->fg_toggled, (XtPointer)toggles);
  XtAddCallback (toggles->bg_toggle, XmNvalueChangedCallback,
		 toggles->bg_toggled, (XtPointer)toggles);

  toggles->type = 1;
  XtManageChild (toggles->_w);
}


/*****************************************************************************
 *             Related routines for RGB labels                               *
 ****************************************************************************/
typedef struct _rgb_labels
{
#if defined (__STDC__)
  /* constructor            */
  void (*construct)(struct _rgb_labels* label,
		    Widget parent);
#else
  void (*construct)();               /* constructor            */
#endif
  Widget r, g, b;                    /* internal label widgets */
  Widget _w;                         /* container widget       */
#if defined (__STDC__)
  /* update function        */
  void (*updateR) (struct _rgb_labels* label,
		   int value);
  void (*updateB) (struct _rgb_labels* label,
		   int value);
  void (*updateG) (struct _rgb_labels* label,
		   int value);
#else
  void (*updateR)();                 /* update function        */
  void (*updateB)();
  void (*updateG)();
#endif
}rgbLabels;

static rgbLabels      rgb_labels;

#if defined (__STDC__)
static void update_redlabel (rgbLabels* labels, int value)
#else
static void update_redlabel (labels, value)
     rgbLabels *labels;
     int       value;
#endif
{
  char temp[10];
  
  sprintf(temp, "%3.3d", value);
  XmTextFieldSetString (labels->r, temp);
}

#if defined (__STDC__)
static void update_greenlabel (rgbLabels* labels, int value)
#else
static void update_greenlabel (labels, value)
     rgbLabels *labels;
     int       value;
#endif
{
  char temp[10];
  
  sprintf(temp, "%3.3d", value);
  XmTextFieldSetString (labels->g, temp);
}

#if defined (__STDC__)  
static void update_bluelabel (rgbLabels* labels, int value)
#else
static void update_bluelabel (labels, value)
     rgbLabels *labels;
     int       value;
#endif
{
  char temp[10];
  
  sprintf(temp, "%3.3d", value);
  XmTextFieldSetString (labels->b, temp);
}

#if defined (__STDC__)
static void constructRGBlabels(rgbLabels* labels, Widget parent)
#else
static void constructRGBlabels(labels, parent)
     rgbLabels *labels;
     Widget parent;
#endif
{
  int ac = 0;
  Arg arg[10];
  Widget   label1, label2, label3;

  XtSetArg (arg[ac], XmNorientation, XmHORIZONTAL); ac++;
  XtSetArg (arg[ac], XmNpacking, XmPACK_COLUMN); ac++;
  XtSetArg (arg[ac], XmNnumColumns, 3); ac++;
  XtSetArg (arg[ac], XmNentryAlignment, XmALIGNMENT_END); ac++;
  XtSetArg (arg[ac], XmNadjustLast, False) ;ac++;
  labels->_w = XtCreateWidget ("rgbViewers", xmRowColumnWidgetClass,
			      parent, arg, ac);
  ac = 0;

  XtSetArg (arg[ac], XmNcolumns, 5); ac++;
  XtSetArg (arg[ac], XmNeditable, False); ac++;
  XtSetArg (arg[ac], XmNcursorPositionVisible, False); ac++;

  label1 = XtCreateManagedWidget ("Red:", xmLabelWidgetClass, 
				  labels->_w, NULL, 0);
  labels->r = XtCreateManagedWidget ("redField", xmTextFieldWidgetClass, 
				     labels->_w, arg, ac);

  label2 = XtCreateManagedWidget ("Green:", xmLabelWidgetClass, 
				  labels->_w, NULL, 0);
  labels->g = XtCreateManagedWidget ("greenField", xmTextFieldWidgetClass, 
				     labels->_w, arg, ac);

  label3 = XtCreateManagedWidget ("Blue:", xmLabelWidgetClass, 
				  labels->_w, NULL, 0);
  labels->b = XtCreateManagedWidget ("blueField", xmTextFieldWidgetClass, 
				     labels->_w, arg, ac);

  XmTextFieldSetString (labels->r, "000");
  XmTextFieldSetString (labels->g, "000");
  XmTextFieldSetString (labels->b, "000");

  labels->updateR = update_redlabel;
  labels->updateG = update_greenlabel;
  labels->updateB = update_bluelabel;

  XtManageChild (labels->_w);
}


/*****************************************************************************
 *             Related routines for HSV Labels                               *
 ****************************************************************************/
typedef struct _hsv_labels
{
#if defined (__STDC__)
  void (*construct) (struct _hsv_labels* labels, Widget parent);
#else
  void (*construct)();
#endif
  Widget h, s, v;
  Widget _w;
#if defined (__STDC__)
  void (*updateHSV) (struct _hsv_labels* labels, int r, int g, int b);
#else
  void (*updateHSV) ();
#endif
}hsvLabels;  

static hsvLabels hsv_labels;

#if defined (__STDC__)
static void update_hsv_values (hsvLabels* labels, int r, int g, int b)
#else
static void update_hsv_values (labels, r, g, b)
     hsvLabels *labels;
     int r, g, b;
#endif
{
  char temp[10];
  float  hue, value, sat;

  RGB_TO_HSV(r, g, b, &hue, &value, &sat);

  sprintf(temp, "%3.3d", (int)hue);
  XmTextFieldSetString (labels->h, temp);

  sprintf(temp, "%3.3d", (int)(value*100));
  XmTextFieldSetString (labels->v, temp);

  sprintf(temp, "%3.3d", (int)(sat*100));
  XmTextFieldSetString (labels->s, temp);
}

#if defined (__STDC__)
static void constructHSVlabels (hsvLabels* labels, Widget parent)
#else
static void constructHSVlabels (labels, parent)
     hsvLabels *labels;
     Widget parent;
#endif
{
  int ac = 0;
  Arg arg[10];
  Widget label1, label2, label3;

  XtSetArg (arg[ac], XmNorientation, XmHORIZONTAL); ac++;
  XtSetArg (arg[ac], XmNpacking, XmPACK_COLUMN); ac++;
  XtSetArg (arg[ac], XmNnumColumns, 3); ac++;
  XtSetArg (arg[ac], XmNentryAlignment, XmALIGNMENT_END); ac++;
  XtSetArg (arg[ac], XmNadjustLast, False) ;ac++;
  labels->_w = XtCreateWidget ("rgbViewers", xmRowColumnWidgetClass,
			      parent, arg, ac);
  ac = 0;

  XtSetArg (arg[ac], XmNcolumns, 5); ac++;
  XtSetArg (arg[ac], XmNeditable, False); ac++;
  XtSetArg (arg[ac], XmNcursorPositionVisible, False); ac++;
  
  label1 = XtCreateManagedWidget ("Hue:", xmLabelWidgetClass, 
				  labels->_w, NULL, 0);
  labels->h = XtCreateManagedWidget ("hField", xmTextFieldWidgetClass, 
				     labels->_w, arg, ac);

  label2 = XtCreateManagedWidget ("Saturation:", xmLabelWidgetClass, 
				  labels->_w, NULL, 0);
  labels->s = XtCreateManagedWidget ("sField", xmTextFieldWidgetClass, 
				     labels->_w, arg, ac);

  label3 = XtCreateManagedWidget ("Value:", xmLabelWidgetClass, 
				  labels->_w, NULL, 0);
  labels->v = XtCreateManagedWidget ("vField", xmTextFieldWidgetClass, 
				     labels->_w, arg, ac);

  XmTextFieldSetString (labels->h,"000");
  XmTextFieldSetString (labels->s,"000");
  XmTextFieldSetString (labels->v,"000");

  labels->updateHSV = update_hsv_values;

  XtManageChild (labels->_w);
}  

/*****************************************************************************
 *             Related routines for Color Displayer                          *
 ****************************************************************************/
typedef struct _color_disp
{
#if defined (__STDC__)
  void   (*construct) (struct _color_disp* disp, Widget parent);
#else
  void   (*construct) ();
#endif
  Widget  _w, _view;

#if defined (__STDC__)
  void   (*update) (struct _color_disp* viewer, int r, int g, int b);
#else
  void   (*update) ();
#endif

  XColor color;
}colorViewer;

static colorViewer color_viewer;

#if defined (__STDC__)
static void update_color_disp (colorViewer* viewer, int r, int g, int b)
#else
static void update_color_disp (viewer, r, g, b)
     colorViewer *viewer;
     int r, g, b;
#endif
{
  Arg arg[5];
  int ac = 0;
  int status;
  Pixel bg;

  if (viewer->_view ){
    viewer->color.red = r*256;
    viewer->color.green = g*256;
    viewer->color.blue = b*256;
    viewer->color.flags = DoRed | DoBlue | DoGreen;
 
    if (XAllocColor (XtDisplay (viewer->_view),
		     DefaultColormapOfScreen (XtScreen (viewer->_view)),
		     &(viewer->color)))
      bg = viewer->color.pixel;
    else{
      status = XcodaAllocNearestColor (XtDisplay (viewer->_view),
				       DefaultColormapOfScreen (XtScreen (viewer->_view)),
				       &(viewer->color));
      if (status)
	bg = viewer->color.pixel;
      else
	bg = 0;
    }
    XtSetArg (arg[ac], XmNbackground, bg); ac++;
    XtSetValues (viewer->_view, arg, ac);
    ac = 0;
  }
}

#if defined (__STDC__)
static void constructColorViewer(colorViewer* viewer, Widget parent)
#else
static void constructColorViewer(viewer, parent)
     colorViewer *viewer;
     Widget      parent;
#endif
{
  int   status;
  Arg   arg[10];
  int   ac = 0;
  Pixel bg;

  viewer->_w = XtCreateWidget ("frame", xmFrameWidgetClass, parent, NULL, 0);

  XtSetArg (arg[ac], XmNbackground, 0); ac++;
  XtSetArg (arg[ac], XmNwidth, 30); ac++;
  viewer->_view = XtCreateManagedWidget ("colorViwer",
					 xmDrawingAreaWidgetClass,
					 viewer->_w, arg, ac);
  ac = 0;

  viewer->update = update_color_disp;

  XtManageChild (viewer->_w);
}



/*****************************************************************************
 *             Related routines with RGB Controllers                         *
 ****************************************************************************/
typedef struct _rgb_controllers
{
#if defined (__STDC__)
  void  (*construct) (struct _rgb_controllers* ctrls, Widget parent);
  void  (*set_r)     (Widget w, XtPointer data, XmScaleCallbackStruct *cbs);
  void  (*set_g)     (Widget w, XtPointer data, XmScaleCallbackStruct *cbs);
  void  (*set_b)     (Widget w, XtPointer data, XmScaleCallbackStruct *cbs);
#else
  void  (*construct)();
  void  (*set_r)();
  void  (*set_g)();
  void  (*set_b)();
#endif

  Widget r, g, b;
  Widget _w;
  int    _red, _green, _blue;
  rgbLabels *rgbViewers;
  hsvLabels *hsvViewers;
  colorViewer *viewer;
}rgbControllers;

static rgbControllers rgbCtrls;

#if defined (__STDC__)
static void set_red(Widget w, XtPointer client_data, 
		    XmScaleCallbackStruct* cbs)
#else
static void set_red(w, client_data, cbs)
     Widget w;
     XtPointer client_data;
     XmScaleCallbackStruct *cbs;
#endif
{
  rgbControllers *obj = (rgbControllers *)client_data;
  obj->_red = cbs->value;
  obj->rgbViewers->updateR (obj->rgbViewers, cbs->value);
  obj->hsvViewers->updateHSV (obj->hsvViewers, cbs->value, obj->_green, obj->_blue);
  obj->viewer->update (obj->viewer, obj->_red, obj->_green, obj->_blue);
}

#if defined (__STDC__)
static void set_green(Widget w, 
		      XtPointer client_data,
		      XmScaleCallbackStruct* cbs)
#else
static void set_green(w, client_data, cbs)
     Widget w;
     XtPointer client_data;
     XmScaleCallbackStruct *cbs;
#endif
{
  rgbControllers *obj = (rgbControllers *)client_data;
  obj->_green = cbs->value;
  obj->rgbViewers->updateG (obj->rgbViewers, cbs->value);
  obj->hsvViewers->updateHSV (obj->hsvViewers, obj->_red, cbs->value, obj->_blue);
  obj->viewer->update (obj->viewer, obj->_red, obj->_green, obj->_blue);
}


#if defined (__STDC__)
static void set_blue(Widget w, 
		     XtPointer client_data, 
		     XmScaleCallbackStruct* cbs)
#else
static void set_blue(w, client_data, cbs)
     Widget w;
     XtPointer client_data;
     XmScaleCallbackStruct *cbs;
#endif
{
  rgbControllers *obj = (rgbControllers *)client_data;
  obj->_blue = cbs->value;
  obj->rgbViewers->updateB (obj->rgbViewers, cbs->value);
  obj->hsvViewers->updateHSV (obj->hsvViewers, obj->_red, obj->_green, cbs->value);
  obj->viewer->update (obj->viewer, obj->_red, obj->_green, obj->_blue);
}

#if defined (__STDC__)
static void constructRGBControllers(rgbControllers* ctrllers, 
				    Widget parent)
#else
static void constructRGBControllers(ctrllers, parent)
     rgbControllers *ctrllers;
     Widget parent;
#endif
{
  Arg arg[10];
  int ac = 0;
  
  XtSetArg (arg[ac], XmNnumColumns, 1); ac++;
  XtSetArg (arg[ac], XmNpacking, XmPACK_COLUMN); ac++;
  XtSetArg (arg[ac], XmNorientation, XmVERTICAL); ac++;
  ctrllers->_w = XtCreateWidget ("rgbControllers",
				 xmRowColumnWidgetClass,
				 parent, arg, ac);
  ac = 0;

  XtSetArg (arg[ac], XmNminimum, 0); ac++;
  XtSetArg (arg[ac], XmNmaximum, 255); ac++;
  XtSetArg (arg[ac], XmNorientation, XmHORIZONTAL); ac++;
  ctrllers->r = XtCreateManagedWidget ("scale_red",
				       xmScaleWidgetClass,
				       ctrllers->_w, arg, ac);

  ctrllers->g = XtCreateManagedWidget ("scale_green",
				       xmScaleWidgetClass,
				       ctrllers->_w, arg, ac);

  ctrllers->b = XtCreateManagedWidget ("scale_blue",
				       xmScaleWidgetClass,
				       ctrllers->_w, arg, ac);
  ac = 0;

  ctrllers->set_r = set_red;
  ctrllers->set_g = set_green;
  ctrllers->set_b = set_blue;

  ctrllers->rgbViewers = &rgb_labels;
  ctrllers->hsvViewers = &hsv_labels;
  ctrllers->viewer = &color_viewer;

  XtAddCallback (ctrllers->r, XmNvalueChangedCallback,
		 ctrllers->set_r, (XtPointer)ctrllers);
  XtAddCallback (ctrllers->g, XmNvalueChangedCallback,
		 ctrllers->set_g, (XtPointer)ctrllers);
  XtAddCallback (ctrllers->b, XmNvalueChangedCallback,
		 ctrllers->set_b, (XtPointer)ctrllers);
  
  XtManageChild (ctrllers->_w);
}

/******************************************************************************
 *         Popup the color selector                                           *
 *****************************************************************************/
static Widget form = 0;  /* check to see dialog created or not */

#if defined (__STDC__)
static void popdown_color_selector(Widget w, 
				   XtPointer client_data,
				   XmAnyCallbackStruct* cbs)
#else
static void popdown_color_selector(w, client_data, cbs)
     Widget w;
     XtPointer client_data;
     XmAnyCallbackStruct *cbs;
#endif
{
  XtUnmanageChild (form);
  XtPopdown (XtParent(form));
}

#if defined (__STDC__)
static void select_color (Widget w, 
			  XtPointer client_data, 
			  XmAnyCallbackStruct* cbs)
#else
static void select_color (w, client_data, cbs)
     Widget w;
     XtPointer client_data;
     XmAnyCallbackStruct *cbs;
#endif
{
  colorViewer *obj = (colorViewer *)client_data;
  change_color(obj->color.pixel, XtScreen(w), typeToggles.type);
  saveColor (&(obj->color), typeToggles.type);
}

#if defined (__STDC__)
void popup_color_selector(Widget w, 
			  XtPointer client_data, 
			  XmAnyCallbackStruct* cbs)
#else
void popup_color_selector(w, client_data, cbs)
     Widget w;
     XtPointer client_data;
     XmAnyCallbackStruct *cbs;
#endif
{
  Widget pb0, pb1;
  Widget t_form, act_form;
  Arg    arg[20];
  int    ac = 0;
  XmString t;
  int    ncells = XCellsOfScreen (XtScreen(w));

  if (ncells <= 2){
    pop_error_message ("You are using a b/w display", w);
    return;
  }
  if (!form){
    typeToggles.construct = constructToggles;
    rgb_labels.construct = constructRGBlabels;
    hsv_labels.construct = constructHSVlabels;
    color_viewer.construct = constructColorViewer;
    rgbCtrls.construct = constructRGBControllers;

    XtSetArg (arg[ac], XmNtitle, "Color Picker"); ac++;
    form = XmCreateFormDialog (w, "color_form", arg, ac);
    ac = 0;
    
    XtSetArg (arg[ac], XmNtopAttachment, XmATTACH_FORM); ac++;
    XtSetArg (arg[ac], XmNleftAttachment, XmATTACH_FORM); ac++;
    XtSetArg (arg[ac], XmNleftOffset, 2); ac++;
    XtSetArg (arg[ac], XmNrightAttachment, XmATTACH_FORM); ac++;
    XtSetArg (arg[ac], XmNrightOffset, 2); ac++;
    t_form = XtCreateWidget ("top_form", xmFormWidgetClass,
			     form, arg, ac);
    ac = 0;

    /* create passive objects */
    typeToggles.construct (&typeToggles, form);
    rgb_labels.construct (&rgb_labels, t_form);
    hsv_labels.construct (&hsv_labels, t_form);
    color_viewer.construct (&color_viewer, t_form);

    XtSetArg (arg[ac], XmNtopAttachment, XmATTACH_FORM); ac++;
    XtSetArg (arg[ac], XmNtopOffset, 10); ac++;
    XtSetArg (arg[ac], XmNbottomAttachment, XmATTACH_FORM); ac++;
    XtSetArg (arg[ac], XmNrightAttachment, XmATTACH_FORM); ac++;
    XtSetArg (arg[ac], XmNrightOffset, 10); ac++;
    XtSetValues (hsv_labels._w, arg, ac); 
    ac = 0;    

    XtSetArg (arg[ac], XmNtopAttachment, XmATTACH_FORM); ac++;
    XtSetArg (arg[ac], XmNtopOffset, 10); ac++;
    XtSetArg (arg[ac], XmNbottomAttachment, XmATTACH_FORM); ac++;
    XtSetArg (arg[ac], XmNrightAttachment, XmATTACH_WIDGET); ac++;
    XtSetArg (arg[ac], XmNrightWidget, hsv_labels._w); ac++;
    XtSetArg (arg[ac], XmNrightOffset, 10); ac++;
    XtSetValues (rgb_labels._w, arg, ac); 
    ac = 0;

    XtSetArg (arg[ac], XmNtopAttachment, XmATTACH_FORM); ac++;
    XtSetArg (arg[ac], XmNtopOffset, 10); ac++;
    XtSetArg (arg[ac], XmNleftAttachment, XmATTACH_FORM); ac++;
    XtSetArg (arg[ac], XmNleftOffset, 10); ac++;
    XtSetArg (arg[ac], XmNbottomAttachment, XmATTACH_FORM); ac++;
    XtSetArg (arg[ac], XmNrightAttachment, XmATTACH_WIDGET); ac++;
    XtSetArg (arg[ac], XmNrightWidget, rgb_labels._w); ac++;
    XtSetArg (arg[ac], XmNrightOffset, 10); ac++;
    XtSetValues (color_viewer._w, arg, ac); 
    ac = 0;

    /* Create Bottom action buttons */
    XtSetArg (arg[ac], XmNbottomAttachment, XmATTACH_FORM); ac++;
    XtSetArg (arg[ac], XmNbottomOffset, 10); ac++;
    XtSetArg (arg[ac], XmNleftAttachment, XmATTACH_FORM); ac++;
    XtSetArg (arg[ac], XmNleftOffset, 2); ac++;
    XtSetArg (arg[ac], XmNrightAttachment, XmATTACH_FORM); ac++;
    XtSetArg (arg[ac], XmNrightOffset, 2); ac++;
    act_form = XtCreateWidget ("act_form", xmFormWidgetClass,
			       form, arg, ac);
    ac = 0;

    t = XmStringCreateSimple (" Apply ");
    XtSetArg (arg[ac], XmNlabelString, t); ac++;
    XtSetArg (arg[ac], XmNtopAttachment, XmATTACH_FORM); ac++;
    XtSetArg (arg[ac], XmNbottomAttachment, XmATTACH_FORM); ac++;
    XtSetArg (arg[ac], XmNleftAttachment, XmATTACH_FORM); ac++;
    XtSetArg (arg[ac], XmNleftOffset, 10); ac++;
    pb0 = XtCreateManagedWidget ("ok_button", xmPushButtonWidgetClass,
				 act_form, arg, ac);
    ac = 0;
    XmStringFree (t);

    t = XmStringCreateSimple ("Dismiss");
    XtSetArg (arg[ac], XmNlabelString, t); ac++;
    XtSetArg (arg[ac], XmNtopAttachment, XmATTACH_FORM); ac++;
    XtSetArg (arg[ac], XmNbottomAttachment, XmATTACH_FORM); ac++;
    XtSetArg (arg[ac], XmNrightAttachment, XmATTACH_FORM); ac++;
    XtSetArg (arg[ac], XmNrightOffset, 10); ac++;
    pb1 = XtCreateManagedWidget ("dismiss_button", xmPushButtonWidgetClass,
				 act_form, arg, ac);
    ac = 0;
    XmStringFree (t);

    XtSetArg (arg[ac], XmNdefaultButton, pb0); ac++;
    XtSetValues (form, arg, ac);
    ac = 0;

    /* Create RGB Controllers */
    rgbCtrls.construct (&rgbCtrls, form); 
    rgbCtrls._red = 0;
    rgbCtrls._blue = 0;
    rgbCtrls._green = 0;

    XtSetArg (arg[ac], XmNleftAttachment, XmATTACH_FORM); ac++;
    XtSetArg (arg[ac], XmNleftOffset, 20); ac++;
    XtSetArg (arg[ac], XmNrightAttachment, XmATTACH_FORM); ac++;
    XtSetArg (arg[ac], XmNrightOffset, 20); ac++;
    XtSetArg (arg[ac], XmNbottomAttachment, XmATTACH_WIDGET); ac++;
    XtSetArg (arg[ac], XmNbottomWidget, act_form); ac++;
    XtSetArg (arg[ac], XmNbottomOffset, 20); ac++;
    XtSetValues (typeToggles._w, arg, ac); 
    ac = 0;
    
    XtSetArg (arg[ac], XmNleftAttachment, XmATTACH_FORM); ac++;
    XtSetArg (arg[ac], XmNleftOffset, 20); ac++;
    XtSetArg (arg[ac], XmNrightAttachment, XmATTACH_FORM); ac++;
    XtSetArg (arg[ac], XmNrightOffset, 20); ac++;
    XtSetArg (arg[ac], XmNbottomAttachment, XmATTACH_WIDGET); ac++;
    XtSetArg (arg[ac], XmNbottomWidget, typeToggles._w); ac++;
    XtSetArg (arg[ac], XmNbottomOffset, 10); ac++;
    XtSetValues (rgbCtrls._w, arg, ac);
    ac = 0;

    /* setup resources */
    XtSetArg (arg[ac], XmNbottomAttachment, XmATTACH_WIDGET); ac++;
    XtSetArg (arg[ac], XmNbottomWidget, rgbCtrls._w); ac++;
    XtSetArg (arg[ac], XmNbottomOffset, 20); ac++;
    XtSetValues (t_form, arg, ac);
    ac = 0;

    XtAddCallback (pb0, XmNactivateCallback, select_color, (XtPointer)&color_viewer);
    XtAddCallback (pb1, XmNactivateCallback, popdown_color_selector,
		   NULL);

    XtManageChild (t_form);
    XtManageChild (act_form);
  }
  XtManageChild (form);
  XtPopup (XtParent(form), XtGrabNone);
}

