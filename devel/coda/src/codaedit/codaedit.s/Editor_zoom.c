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
 *	CODA editor Zoom button
 *	
 * Author:  Jie Chen, CEBAF Data Acquisition Group
 *
 * Revision History:
 *   $Log: Editor_zoom.c,v $
 *   Revision 1.1.1.1  1996/08/21 19:36:03  heyes
 *   Imported sources
 *
 *	  
 */

#include <stdio.h>
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <Xm/Xm.h>
#include <Xm/Frame.h>
#include <Xm/Label.h>
#include <Xm/ArrowB.h>
#include <Xm/Form.h>

#include "Editor_graph.h"
#include "Editor_pixmap.h"
#include "Editor_color.h"

/*************************************************************************
 *          Widget XcodaEditorZoomWidget(parent, fg, bg)                 *
 * Description:                                                          *
 *     Create a composite widget which contains a magnifying glass       *
 *     and two Arrow Buttons                                             *
 *     Return containner widget                                          *
 ************************************************************************/
#if defined (__STDC__)
Widget XcodaEditorZoomWidget(Widget parent, Pixel fg, Pixel bg)
#else
Widget XcodaEditorZoomWidget(parent,fg,bg)
Widget parent;
Pixel  fg,bg;
#endif
{
  Widget      zoom_form,zoom_frame,zoom_label,up_arrow,dn_arrow;
  Arg         warg[20];
  int         ac = 0;

  /* Create a frame widget to hold everything */
  zoom_frame = XtCreateManagedWidget("zoom_frame",
				     xmFrameWidgetClass,parent,NULL,0);

  XtSetArg(warg[ac],XmNshadowType,XmSHADOW_OUT);ac++;
  XtSetArg(warg[ac],XmNshadowThickness,2);ac++;
  XtSetArg(warg[ac],XmNbackground,bg); ac++;
  XtSetArg(warg[ac],XmNforeground,fg); ac++;

  XtSetValues(zoom_frame,warg,ac);
  ac = 0;

  XtSetArg(warg[ac],XmNmarginWidth, 0); ac++;
  XtSetArg(warg[ac],XmNmarginHeight, 0); ac++;
  XtSetArg(warg[ac],XmNresizePolicy, XmRESIZE_NONE); ac++;
  zoom_form = XtCreateManagedWidget("zoom_form", xmFormWidgetClass, 
				    zoom_frame, warg, ac);
  ac = 0;

  XtSetArg(warg[ac], XmNlabelType, XmPIXMAP); ac++;
  XtSetArg(warg[ac], XmNlabelPixmap, btn_pixmaps.zoom); ac++;
  XtSetArg(warg[ac], XmNtopAttachment, XmATTACH_FORM); ac++;
  XtSetArg(warg[ac], XmNleftAttachment, XmATTACH_FORM); ac++;
  XtSetArg(warg[ac], XmNbottomAttachment, XmATTACH_FORM); ac++;
  XtSetArg(warg[ac], XmNheight, 48); ac++;
  XtSetArg(warg[ac], XmNmarginHeight, 0); ac++;
  XtSetArg(warg[ac], XmNmarginWidth, 0); ac++;
  zoom_label = XtCreateManagedWidget("zoom_label",xmLabelWidgetClass, 
				     zoom_form, warg, ac);
  ac = 0;

  XtSetArg(warg[ac], XmNarrowDirection,XmARROW_UP); ac++;
  XtSetArg(warg[ac], XmNtopAttachment, XmATTACH_FORM); ac++;
  XtSetArg(warg[ac], XmNleftAttachment,XmATTACH_WIDGET); ac++;
  XtSetArg(warg[ac], XmNleftWidget,  zoom_label); ac++;
  XtSetArg(warg[ac], XmNrightAttachment, XmATTACH_FORM); ac++;
  XtSetArg(warg[ac], XmNwidth,           20); ac++;
  XtSetArg(warg[ac], XmNheight,          22);
  XtSetArg(warg[ac], XmNborderWidth,     0); ac++;
  XtSetArg(warg[ac], XmNshadowThickness, 0); ac++;
  up_arrow = XtCreateManagedWidget("up_arrow", xmArrowButtonWidgetClass,  
				   zoom_form, warg, ac);
  ac = 0;

  XtSetArg(warg[ac], XmNarrowDirection,XmARROW_DOWN); ac++;
  XtSetArg(warg[ac], XmNleftAttachment,XmATTACH_WIDGET); ac++;
  XtSetArg(warg[ac], XmNleftWidget,  zoom_label); ac++;
  XtSetArg(warg[ac], XmNrightAttachment, XmATTACH_FORM); ac++;
  XtSetArg(warg[ac], XmNbottomAttachment, XmATTACH_FORM); ac++;
  XtSetArg(warg[ac], XmNwidth,           20); ac++;
  XtSetArg(warg[ac], XmNheight,          22); ac++;
  XtSetArg(warg[ac], XmNborderWidth,     0); ac++;
  XtSetArg(warg[ac], XmNshadowThickness, 0); ac++;
  dn_arrow = XtCreateManagedWidget("dn_arrow", xmArrowButtonWidgetClass, 
				   zoom_form, warg, ac);
  ac = 0;

  sw_geometry.up_arrow = up_arrow;
  sw_geometry.dn_arrow = dn_arrow;
  return(zoom_frame);
}

/*********************************************************************
 *         void set_zoomscale()                                      *
 * Description:                                                      *
 *    setup zoom scale for drawing area                              *
 *    zomm scale is from 1 to 5                                      *
 ********************************************************************/
#if defined (__STDC__)
void set_zoomscale(Widget w, 
		   XtPointer client_data, 
		   XmArrowButtonCallbackStruct* cbs)
#else
void set_zoomscale(w, client_data, cbs)
     Widget w;
     XtPointer client_data;
     XmArrowButtonCallbackStruct *cbs;
#endif
{
  int type = (int)client_data;
  static int zoomscale = 3;
  
  coda_graph.current_action = NORMAL_ACTION;
  if(type == 0){ /* up arrow */
    if(zoomscale == 6)
      zoomscale = 6;
    else
      zoomscale = zoomscale + 1;
  }
  else{ /*down arrow */
    if(zoomscale == 1)
      zoomscale = 1;
    else
      zoomscale = zoomscale - 1;
  }
  sw_geometry.zoomscale = zoomscale;

  reset_scrollbar_res();
  XClearWindow(xgc.dpy, XtWindow(sw_geometry.draw_area));
  XcodaEditorBgPixmap(sw_geometry.draw_area);
}
   
