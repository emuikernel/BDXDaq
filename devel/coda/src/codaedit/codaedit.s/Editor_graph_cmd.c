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
 *     Data Link option buttons for CODA Editor
 *	
 * Author:  Jie Chen, CEBAF Data Acquisition Group
 *
 * Revision History:
 *   $Log: Editor_graph_cmd.c,v $
 *   Revision 1.2  1997/07/22 12:16:46  heyes
 *   need to debug on solaris
 *
 *   Revision 1.1.1.1  1996/08/21 19:36:03  heyes
 *   Imported sources
 *
 *	  
 */ 
#include <stdio.h>
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <Xm/Xm.h>
#include <Xm/RowColumn.h>
#include <Xm/Form.h>
#include <Xm/ToggleB.h>
#include <Xm/PushB.h>

#include "Editor_pixmap.h"
#include "Editor_layout.h"
#include "Editor_graph.h"

/* return form  widget */
#if defined (__STDC__)
Widget XcodaEditorDataLinkButtons(Widget parent)
#else
Widget XcodaEditorDataLinkButtons(parent)
     Widget parent;
#endif
{
  Widget form, pb[10];
  Arg    args[20];
  int    ac;

  ac = 0;
  form = XtCreateWidget("link_form",xmFormWidgetClass,
			parent, NULL, 0);

  XtSetArg(args[ac], XmNtopAttachment, XmATTACH_FORM); ac++;
  XtSetArg(args[ac], XmNtopOffset, 5); ac++;
  XtSetArg(args[ac], XmNleftAttachment, XmATTACH_FORM); ac++;
  XtSetArg(args[ac], XmNleftOffset, 5); ac++;
  XtSetArg(args[ac], XmNrightAttachment, XmATTACH_FORM); ac++;
  XtSetArg(args[ac], XmNrightOffset, 5); ac++;
  XtSetArg(args[ac], XmNlabelType, XmPIXMAP); ac++;
  XtSetArg(args[ac], XmNlabelPixmap, btn_pixmaps.eth_input); ac++;
  pb[0] = XtCreateManagedWidget("pb0", xmPushButtonWidgetClass,
				form, args, ac);
  ac = 0;
  manager.eth_btn = pb[0];

  XtSetArg(args[ac], XmNtopAttachment, XmATTACH_WIDGET); ac++;
  XtSetArg(args[ac], XmNtopWidget,     pb[0]); ac++;
  XtSetArg(args[ac], XmNtopOffset, 5); ac++;
  XtSetArg(args[ac], XmNrightAttachment, XmATTACH_FORM); ac++;
  XtSetArg(args[ac], XmNrightOffset, 5); ac++;
  XtSetArg(args[ac], XmNbottomAttachment, XmATTACH_FORM); ac++;
  XtSetArg(args[ac], XmNbottomOffset, 5); ac++;
  XtSetArg(args[ac], XmNleftAttachment, XmATTACH_FORM); ac++;
  XtSetArg(args[ac], XmNleftOffset, 5); ac++;
  XtSetArg(args[ac], XmNlabelType, XmPIXMAP); ac++;
  XtSetArg(args[ac], XmNlabelPixmap, btn_pixmaps.move_node); ac++;
  pb[1] = XtCreateManagedWidget("pb1", xmPushButtonWidgetClass,
				form, args, ac);
  ac = 0;
  manager.mvnode_btn = pb[1];

  /*XtManageChild(form);*/
  return form;
}

/******************************************************************
 *        void XcodaEditorResetGraphCmd()                         *
 * Description:                                                   *
 *     Reset Graph Command to Default                             *
 *****************************************************************/
static int move_node_mode = 0;
static int conn_node_mode = 0;

#if defined (__STDC__)
void XcodaEditorResetGraphCmd (void)
#else
void XcodaEditorResetGraphCmd ()
#endif
{
  Arg arg[10];
  int ac = 0;

  if (move_node_mode){
    coda_graph.current_action = NORMAL_ACTION;
    XUndefineCursor(xgc.dpy, XtWindow(sw_geometry.draw_area));
    move_node_mode = 0;
    XtSetArg(arg[ac], XmNlabelPixmap, btn_pixmaps.move_node); ac++;
    XtSetValues (manager.mvnode_btn, arg, ac);
    ac = 0;
  }
  else if (conn_node_mode){
    coda_graph.current_action = NORMAL_ACTION;
    XUndefineCursor(xgc.dpy, XtWindow(sw_geometry.draw_area));
    conn_node_mode = 0;
    XtSetArg(arg[ac], XmNlabelPixmap, btn_pixmaps.eth_input); ac++;
    XtSetValues (manager.eth_btn, arg, ac);
    ac = 0;
  }
}

/******************************************************************
 *        void XcodaEditorExecGraphCmd()                          *
 * Description:                                                   *
 *     change selected arc direction                              *
 *****************************************************************/
#if defined (__STDC__)
void XcodaEditorExecGraphCmd(Widget w, 
			     XtPointer client_data, 
			     XmAnyCallbackStruct* cbs)
#else
void XcodaEditorExecGraphCmd(w, client_data, cbs)
     Widget w;
     XtPointer client_data;
     XmAnyCallbackStruct *cbs;
#endif
{
  Arc    *selected_arc;
  ipPort *temp;
  int    temp_int;
  XPoint new_edge[4];
  int    i;
  int    type = (int)client_data;
  Arg    arg[10];
  int    ac = 0;

  if (type == IP_CONNECTION){
    selected_arc = XcodaEditorGetSelectedArc(&coda_graph);
    if(selected_arc != NULL){
      (*selected_arc->erase)(selected_arc, xgc.dpy, 
			     XtWindow(sw_geometry.draw_area));
      if(type == selected_arc->type){ /* flip direction */
	temp = selected_arc->from;
	selected_arc->from = selected_arc->to;
	selected_arc->to = temp;
	selected_arc->from_row = temp_int;
	selected_arc->from_row = selected_arc->to_row;
	selected_arc->to_row = temp_int;
	selected_arc->from_col = temp_int;
	selected_arc->from_col = selected_arc->to_col;
	selected_arc->to_col = temp_int;
	update_arc_geometry(selected_arc, selected_arc->from, selected_arc->to);
	(*selected_arc->high_light)(selected_arc, xgc.dpy, 
				    XtWindow(sw_geometry.draw_area));
      }
      else{
	selected_arc->type = type;
	(*selected_arc->high_light)(selected_arc, xgc.dpy, 
				    XtWindow(sw_geometry.draw_area));
      }
    }
    else{ /* conencting two ports */
      if (!conn_node_mode){
	coda_graph.current_action = PRE_CONNECT_IP_PORTS_ACTION;
	XDefineCursor(xgc.dpy, XtWindow(sw_geometry.draw_area),
		      coda_graph.delete_arc_cursor);    
	conn_node_mode = 1;
	XtSetArg(arg[ac], XmNlabelPixmap, btn_pixmaps.eth_input_sel); ac++;
	XtSetValues (w, arg, ac);
	ac = 0;
	if (move_node_mode){
	  move_node_mode = 0;
	  XtSetArg(arg[ac], XmNlabelPixmap, btn_pixmaps.move_node); ac++;
	  XtSetValues (manager.mvnode_btn, arg, ac);
	  ac = 0;
	}
      }
      else{
	coda_graph.current_action = NORMAL_ACTION;
	XUndefineCursor(xgc.dpy, XtWindow(sw_geometry.draw_area));
	conn_node_mode = 0;
	XtSetArg(arg[ac], XmNlabelPixmap, btn_pixmaps.eth_input); ac++;
	XtSetValues (w, arg, ac);
	ac = 0;
      }
    }
  }
  else{ /* move a node */
    if (!move_node_mode){
      coda_graph.current_action = PRE_MOVE_NODE_ACTION;
      XDefineCursor(xgc.dpy, XtWindow(sw_geometry.draw_area),
		    coda_graph.finger_cursor);  
      move_node_mode = 1;
      XtSetArg(arg[ac], XmNlabelPixmap, btn_pixmaps.move_node_sel); ac++;
      XtSetValues (w, arg, ac);
      ac = 0;
      if (conn_node_mode){
	conn_node_mode = 0;
	XtSetArg(arg[ac], XmNlabelPixmap, btn_pixmaps.eth_input); ac++;
	XtSetValues (manager.eth_btn, arg, ac);
	ac = 0;
      }
    }
    else{
      coda_graph.current_action = NORMAL_ACTION;
      XUndefineCursor(xgc.dpy, XtWindow(sw_geometry.draw_area));
      move_node_mode = 0;
      XtSetArg(arg[ac], XmNlabelPixmap, btn_pixmaps.move_node); ac++;
      XtSetValues (w, arg, ac);
      ac = 0;
    }
  }      
}
