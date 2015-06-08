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
 *	Coda Editor misc X-window routines
 *	
 * Author:  Jie Chen, CEBAF Data Acquisition Group
 *
 * Revision History:
 *   $Log: Editor_xmisc.c,v $
 *   Revision 1.9  1998/09/15 17:51:06  rwm
 *   Cast warnings aside.
 *
 *   Revision 1.8  1998/06/18 12:28:33  heyes
 *   tidy up a lot of thingscd ../cedit
 *
 *   Revision 1.7  1997/11/25 15:55:43  rwm
 *   Zapped compiler warnings.
 *
 *   Revision 1.6  1997/09/08 15:19:40  heyes
 *   fix dd icon etc
 *
 *   Revision 1.5  1997/08/29 12:25:39  heyes
 *   fixed window positioning
 *
 *   Revision 1.4  1997/08/01 18:37:54  heyes
 *   fixed moving bug
 *
 *   Revision 1.3  1997/06/20 17:00:24  heyes
 *   clean up GUI!
 *
 *   Revision 1.2  1996/12/02 15:23:51  chen
 *   change size of attribute popup for components
 *
 *   Revision 1.1.1.2  1996/11/05 17:45:19  chen
 *   coda source
 *
 *	  
 */
#include <stdio.h>
#include <string.h>
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/Shell.h>
#include <Xm/Xm.h>
#include <Xm/Form.h>
#include <Xm/ArrowB.h>
#include <Xm/Separator.h>
#include <Xm/PushB.h>
#include <Xm/DialogS.h>
#include <Xm/Frame.h>
#include <Xm/TextF.h>
#include <Xm/Label.h>
#include <Xm/SelectioB.h>

#include "Editor_graph.h"
#include "Editor_converter.h"
#include "Editor_xmisc.h"
#include "Editor_drawing.h"
#include "Editor_misc.h"
#include "Editor_syntax_checker.h"
#include "Editor_graph_cmd.h"
#include "Editor_script_dialog.h"




/**************************************************************
 *      void popup_resize_selection(graph, comp, draw_area)   *
 * Description:                                               *
 *     Popup a selection dialog box to select size of         *
 *     component  comp                                        *
 *************************************************************/
#if defined (__STDC__)
static void change_num_ports(Widget w, 
			     Widget value_label, 
			     XmArrowButtonCallbackStruct* cbs)
#else
static void change_num_ports(w, value_label, cbs)
     Widget w;
     Widget value_label;
     XmArrowButtonCallbackStruct *cbs;
#endif
{
  int type;
  int num_ports;
  char   temp[5];
  char   *value;
  Arg    args[5];
  int    ac = 0;
  XmString t;

  XtSetArg(args[ac], XmNlabelString, &t); ac++;
  XtGetValues(value_label, args, ac);
  ac = 0;
  XmStringGetLtoR(t, XmSTRING_DEFAULT_CHARSET,&value);
  sscanf(value,"%d",&num_ports);
  XmStringFree(t);
  
  XtSetArg(args[ac], XmNuserData, &type); ac++;
  XtGetValues(w, args, ac);
  ac = 0;
  if(type == XmARROW_UP){
    if(num_ports < 5)
      num_ports = num_ports + 1;
    else
      num_ports = 5;
    sprintf(temp, "%d", num_ports);
    t = XmStringCreateSimple(temp);
    XtSetArg(args[ac], XmNlabelString, t); ac++;
    XtSetValues(value_label, args, ac); 
    ac = 0;
    XmStringFree(t);
  }
  else{
    if(num_ports > 1){
      num_ports = num_ports - 1;
    }
    else
      num_ports = 1;
    sprintf(temp, "%d", num_ports);
    t = XmStringCreateSimple(temp);
    XtSetArg(args[ac], XmNlabelString, t); ac++;
    XtSetValues(value_label, args, ac); 
    ac = 0;
    XmStringFree(t);
  }
}

#if defined (__STDC__)  
static void popdown_resize_shell (Widget w, 
				  Widget shell, 
				  XmAnyCallbackStruct* cbs)
#else
static void change_num_ports(w, value_label, cbs)
     Widget w, shell;
     XmAnyCallbackStruct *cbs;
#endif
{
  XtDestroyWidget(shell);
}

#if defined (__STDC__)
static void resize_comp (Widget w, drawComp* comp, 
			 XmAnyCallbackStruct* cbs)
#else
static void resize_comp (w, comp, cbs)
     Widget w;
     drawComp *comp;
     XmAnyCallbackStruct *cbs;
#endif
{
  Widget shell = XtParent(XtParent(w));
  Arg    args[5];
  int    num_ports, old_num_ports;
  char   *value;
  Widget value_label;
  XmString t;
  int    i, j, ac = 0;
  ipPort *port;
  Arc    **from_arcs, **to_arcs;
  int    num_from_arcs, num_to_arcs;

  XtSetArg(args[ac], XmNuserData, &value_label); ac++;
  XtGetValues(w, args, ac);
  ac = 0;
  XtSetArg(args[ac], XmNlabelString, &t); ac++;
  XtGetValues(value_label, args, ac);
  ac = 0;
  XmStringGetLtoR(t, XmSTRING_DEFAULT_CHARSET, &value);
  sscanf(value,"%d",&num_ports);
  XmStringFree(t);

/* check whether I have enough space to contain this resizeed component */
  for(i=1; i<num_ports;i++){
    if(isOneCoverAnother(&coda_graph, comp, comp->col, comp->row + i)){
      XBell(xgc.dpy, 0);
      return;
    }
  }

/* erase old component first */  
  (*comp->erase)(comp,xgc.dpy, XtWindow(sw_geometry.draw_area));
  (*comp->erase_name)(comp,xgc.dpy, XtWindow(sw_geometry.draw_area));

  old_num_ports = comp->num_ports;
  comp->num_row = num_ports;
  comp->num_ports = num_ports;

  if(old_num_ports > num_ports){ /* one may have to delete arcs */
    for(i = num_ports; i < old_num_ports; i++){
      port = &(comp->ip_port[i]);
      if(port->ip_addr != NULL)
	(*port->erase_name)(port, xgc.dpy, XtWindow(sw_geometry.draw_area));
      from_arcs = XcodaEditorGetFromArcs(&coda_graph, port, &num_from_arcs);
      if(num_from_arcs != 0){
	for(j=0;j<num_from_arcs; j++){
	  (*from_arcs[j]->erase)(from_arcs[j],xgc.dpy,XtWindow(sw_geometry.draw_area));
	  removeArcFromWholeArcList(&coda_graph, from_arcs[j]);
	  free(from_arcs[j]);
	}
	free(from_arcs);
      }
      to_arcs = XcodaEditorGetToArcs(&coda_graph ,port, &num_to_arcs);
      if(num_to_arcs != 0){
	for(j=0;j<num_to_arcs; j++){
	  (*to_arcs[j]->erase)(to_arcs[j],xgc.dpy,XtWindow(sw_geometry.draw_area));
	  removeArcFromWholeArcList(&coda_graph, to_arcs[j]);
	  free(to_arcs[j]);
	}
	free(to_arcs);
      }
      if(port->ip_addr != NULL)
	free(port->ip_addr);
      port->ip_addr = (char *)NULL;
    }
  }

  if(num_ports > 1){
    for(i=1; i < num_ports; i++){
      port = &(comp->ip_port[i]);
      port->left_x = comp->x;
      port->left_y = comp->y + i*comp->height + 3*comp->height/8.0;
      port->right_x = comp->x + 7*comp->width/8.0;
      port->right_y = port->left_y;
      port->row = comp->row + 1;
      port->col = comp->col;
      port->width = comp->width/8.0;
      port->height = comp->height/4.0;
    }
  }

  if(comp->selected)
    (*comp->high_light)(comp, xgc.dpy, XtWindow(sw_geometry.draw_area));
  else
    (*comp->unhigh_light)(comp, xgc.dpy, XtWindow(sw_geometry.draw_area));
  (*comp->write_name)(comp,xgc.dpy, XtWindow(sw_geometry.draw_area));
  
  XtDestroyWidget(shell);
}

#if defined (__STDC__)
void popup_resize_selection(XcodaEditorGraph* graph, 
			    drawComp* comp, 
			    Widget draw_area)
#else
void popup_resize_selection(graph, comp, draw_area)
     XcodaEditorGraph *graph;
     drawComp         *comp;
     Widget           draw_area;
#endif
{
  Widget d_shell, form, label, frame;
  Widget mid_form,value_label;
  Widget sep, pushb0, pushb1;
  Widget up_arrow, dn_arrow;
  Arg    args[20];
  int    ac = 0;
  char   temp[5];
  XmString  t;
  Position ret_x, ret_y;
  
  XtTranslateCoords(draw_area, comp->x, comp->y, &ret_x, &ret_y);

  XtSetArg(args[ac], XmNx, 100); ac++;
  XtSetArg(args[ac], XmNy, 200); ac++;
  XtSetArg(args[ac], XmNdefaultPosition, False); ac++;
  XtSetArg(args[ac], XmNtitle, "Resize Dialog"); ac++;
  XtSetArg(args[ac], XmNresizePolicy, XmRESIZE_NONE); ac++;
  XtSetArg(args[ac], XmNnoResize, True); ac++;
  form = XmCreateFormDialog(draw_area, "resizeFormDialog",args, ac);
  d_shell = XtParent (form);

  t = XmStringCreateSimple("Number of data Ports");
  XtSetArg(args[ac], XmNlabelString, t); ac++;
  XtSetArg(args[ac], XmNtopAttachment, XmATTACH_FORM); ac++;
  XtSetArg(args[ac], XmNtopOffset, 5); ac++;
  XtSetArg(args[ac], XmNleftAttachment, XmATTACH_FORM); ac++;
  XtSetArg(args[ac], XmNrightAttachment, XmATTACH_FORM); ac++;
  label = XtCreateManagedWidget("resize_label", 
	   xmLabelWidgetClass, form, args, ac);
  ac = 0;
  XmStringFree(t);

  /* Mid form holds all arrow buttons */
  XtSetArg(args[ac], XmNtopAttachment, XmATTACH_WIDGET); ac++;
  XtSetArg(args[ac], XmNtopWidget, label); ac++;
  XtSetArg(args[ac], XmNtopOffset, 5); ac++;
  XtSetArg(args[ac], XmNleftAttachment, XmATTACH_FORM); ac++;
  XtSetArg(args[ac], XmNleftOffset, 2); ac++;
  XtSetArg(args[ac], XmNrightAttachment, XmATTACH_FORM); ac++;
  XtSetArg(args[ac], XmNrightOffset, 2); ac++;
  mid_form = XtCreateManagedWidget("mid_form",
           xmFormWidgetClass, form, args, ac);
  ac = 0;

  XtSetArg(args[ac], XmNtopAttachment, XmATTACH_FORM); ac++;
  XtSetArg(args[ac], XmNtopOffset, 5); ac++;
  XtSetArg(args[ac], XmNleftAttachment, XmATTACH_FORM); ac++;
  XtSetArg(args[ac], XmNleftOffset, 20); ac++;
  XtSetArg(args[ac], XmNborderWidth, 0); ac++;
  XtSetArg(args[ac], XmNarrowDirection, XmARROW_UP); ac++;
  XtSetArg(args[ac], XmNuserData, XmARROW_UP); ac++;
  up_arrow = XtCreateManagedWidget("up_arrow", 
           xmArrowButtonWidgetClass, mid_form, args, ac);
  ac = 0;

  XtSetArg(args[ac], XmNtopAttachment, XmATTACH_WIDGET); ac++;
  XtSetArg(args[ac], XmNtopWidget, up_arrow); ac++;
  XtSetArg(args[ac], XmNtopOffset, 5); ac++;
  XtSetArg(args[ac], XmNleftAttachment, XmATTACH_FORM); ac++;
  XtSetArg(args[ac], XmNleftOffset, 20); ac++;
  XtSetArg(args[ac], XmNbottomAttachment, XmATTACH_FORM); ac++;
  XtSetArg(args[ac], XmNbottomOffset, 5); ac++;
  XtSetArg(args[ac], XmNborderWidth, 0); ac++;
  XtSetArg(args[ac], XmNarrowDirection, XmARROW_DOWN); ac++;
  XtSetArg(args[ac], XmNuserData, XmARROW_DOWN); ac++;
  dn_arrow = XtCreateManagedWidget("dn_arrow", 
           xmArrowButtonWidgetClass, mid_form, args, ac);
  ac = 0;

  XtSetArg(args[ac], XmNtopAttachment, XmATTACH_POSITION); ac++;
  XtSetArg(args[ac], XmNtopPosition, 20); ac++;
  XtSetArg(args[ac], XmNbottomAttachment, XmATTACH_POSITION); ac++;
  XtSetArg(args[ac], XmNbottomPosition, 80); ac++;
  XtSetArg(args[ac], XmNleftAttachment, XmATTACH_POSITION); ac++;
  XtSetArg(args[ac], XmNleftPosition, 40); ac++;
  XtSetArg(args[ac], XmNrightAttachment, XmATTACH_FORM); ac++;
  XtSetArg(args[ac], XmNrightOffset, 10); ac++;
  XtSetArg(args[ac], XmNshadowType, XmSHADOW_IN); ac++;
  XtSetArg(args[ac], XmNshadowThickness, 3); ac++;
  frame = XtCreateManagedWidget("frame",
	    xmFrameWidgetClass, mid_form, args, ac);
  ac = 0;

  sprintf(temp,"%d", comp->num_row);
  t = XmStringCreateSimple(temp);
  XtSetArg(args[ac], XmNlabelString, t); ac++;
  value_label = XtCreateManagedWidget("value_label", xmLabelWidgetClass,
		 frame, args, ac);
  ac = 0;
  XmStringFree(t);

  /* action buttons */
  t = XmStringCreateSimple("Ok");
  XtSetArg(args[ac], XmNleftAttachment, XmATTACH_POSITION); ac++;
  XtSetArg(args[ac], XmNleftPosition,5); ac++;
  XtSetArg(args[ac], XmNrightAttachment, XmATTACH_POSITION); ac++;
  XtSetArg(args[ac], XmNrightPosition, 45); ac++;
  XtSetArg(args[ac], XmNbottomAttachment, XmATTACH_FORM); ac++;
  XtSetArg(args[ac], XmNbottomOffset, 5); ac++;
  XtSetArg(args[ac], XmNlabelString, t); ac++;
  XtSetArg(args[ac], XmNuserData, value_label); ac++;
  pushb0 = XtCreateManagedWidget("pushb0",xmPushButtonWidgetClass,
				 form, args, ac);
  ac = 0;
  XmStringFree(t);

  t = XmStringCreateSimple("Cancel");
  XtSetArg(args[ac], XmNleftAttachment, XmATTACH_POSITION); ac++;
  XtSetArg(args[ac], XmNleftPosition,55); ac++;
  XtSetArg(args[ac], XmNrightAttachment, XmATTACH_POSITION); ac++;
  XtSetArg(args[ac], XmNrightPosition, 95); ac++;
  XtSetArg(args[ac], XmNbottomAttachment, XmATTACH_FORM); ac++;
  XtSetArg(args[ac], XmNbottomOffset, 5); ac++;
  XtSetArg(args[ac], XmNlabelString, t); ac++;
  pushb1 = XtCreateManagedWidget("pushb1",xmPushButtonWidgetClass,
				 form, args, ac);
  ac = 0;
  XmStringFree(t);

  XtSetArg(args[ac], XmNleftAttachment, XmATTACH_FORM); ac++;
  XtSetArg(args[ac], XmNleftOffset, 2); ac++;
  XtSetArg(args[ac], XmNrightAttachment, XmATTACH_FORM); ac++;
  XtSetArg(args[ac], XmNrightOffset, 2); ac++;
  XtSetArg(args[ac], XmNbottomAttachment, XmATTACH_WIDGET); ac++;
  XtSetArg(args[ac], XmNbottomWidget, pushb0); ac++;
  XtSetArg(args[ac], XmNbottomOffset, 5); ac++;
  XtSetArg(args[ac], XmNtopAttachment, XmATTACH_WIDGET); ac++;
  XtSetArg(args[ac], XmNtopWidget, mid_form); ac++;
  XtSetArg(args[ac], XmNtopOffset, 5); ac++;

  sep = XtCreateManagedWidget("sep",
         xmSeparatorWidgetClass, form, args, ac);
  ac = 0;

  XtAddCallback(pushb0, XmNactivateCallback, resize_comp, comp);
  
  XtAddCallback(pushb1, XmNactivateCallback, popdown_resize_shell,
		d_shell);

  XtManageChild(form);

  XtAddCallback(up_arrow, XmNactivateCallback, change_num_ports, value_label);
  XtAddCallback(dn_arrow, XmNactivateCallback, change_num_ports, value_label);
  XtPopup(d_shell, XtGrabNone);
}

/**************************************************************************
 *      void popup_ipport_attributes(port, w, event)                      *
 * Description:                                                           *
 *     popup dialog box to enter a name for this port's ip_addrs          *
 **************************************************************************/
#if defined (__STDC__)
static void popdown_name_entry_shell(Widget w, 
				     Widget shell, 
				     XmAnyCallbackStruct* cbs)
#else
static void popdown_name_entry_shell(w, shell, cbs)
     Widget w, shell;
     XmAnyCallbackStruct *cbs;
#endif
{
  XtPopdown(shell);
}

#if defined (__STDC__)    
static void assign_port_name(Widget w, 
			     ipPort* port, 
			     XmAnyCallbackStruct* cbs)
#else
static void assign_port_name(w, port, cbs)
     Widget w;
     ipPort *port;
     XmAnyCallbackStruct *cbs;
#endif
{
  Widget shell = XtParent(XtParent(w));
  Widget text_w;
  char *name = 0;
  Arg  args[5];
  int  ac = 0;
  int      i;
  daqComp *comp = port->comp_ptr;


  (*port->erase_name)(port, xgc.dpy, XtWindow(sw_geometry.draw_area));
  if(XtIsSubclass(w, xmPushButtonWidgetClass)){
    XtSetArg(args[ac], XmNuserData, &text_w); ac++;
    XtGetValues(w, args, ac); 
    ac = 0;
    name = XmTextFieldGetString(text_w);
  }
  else  /* text_w */
    name = XmTextFieldGetString(w);

  if (!name || !*name){/* empty input, give default to port */
    if(port->ip_addr != NULL){
      free(port->ip_addr);
      port->ip_addr = (char *)NULL;
    }
    if (comp->node_name)
      port->ip_addr = strsave(comp->node_name);
    else
      port->ip_addr = 0;
  }
  else{
    if(port->ip_addr != NULL){
      free(port->ip_addr);
      port->ip_addr = (char *)NULL;
    }
    port->ip_addr = strsave(name);
    XtFree(name);
  }

  XtPopdown(shell);
}

#if defined (__STDC__)
void popup_ipport_attributes (ipPort* port, 
			      Widget w, 
			      XEvent* event)
#else
void popup_ipport_attributes (port, w, event)
     ipPort *port;
     Widget w;
     XEvent *event;
#endif
{
  Widget ipAttrForm;
  Widget text_w, pushb0;
  Widget d_shell;
  Widget top_label, sep, pushb1;
  Arg    args[20];
  int    ac = 0;
  Position ret_x, ret_y;
  char   ip_name[50];
  XmString t = XmStringCreateSimple("Link Type: ");
  
  XtTranslateCoords(w, port->right_x, port->right_y,&ret_x, &ret_y);
  XtSetArg(args[ac], XmNx, 100); ac++;
  XtSetArg(args[ac], XmNy, 200); ac++;
  XtSetArg(args[ac], XmNtitle,"Port name"); ac++;
  XtSetArg(args[ac], XmNdefaultPosition, False); ac++;
  ipAttrForm = XmCreateFormDialog(w, "ipPortInfoDialog",args, ac);
  ac = 0;
  d_shell = XtParent(ipAttrForm);

  XtSetArg(args[ac], XmNlabelString, t); ac++;
  XtSetArg(args[ac], XmNtopAttachment, XmATTACH_FORM); ac++;
  XtSetArg(args[ac], XmNtopOffset, 10); ac++;
  XtSetArg(args[ac], XmNleftAttachment, XmATTACH_FORM); ac++;
  XtSetArg(args[ac], XmNleftOffset, 20); ac++;
  XtSetArg(args[ac], XmNrightAttachment, XmATTACH_FORM); ac++;
  XtSetArg(args[ac], XmNrightOffset, 20); ac++;
  XtSetArg(args[ac], XmNalignment, XmALIGNMENT_BEGINNING); ac++;
  top_label = XtCreateManagedWidget("top_label",xmLabelWidgetClass,
				    ipAttrForm, args, ac);
  ac = 0;
  XmStringFree(t);

  if(port->ip_addr != NULL)
    strcpy(ip_name, port->ip_addr);
  else
    strcpy(ip_name,"");
  XtSetArg(args[ac], XmNvalue, ip_name); ac++;
  XtSetArg(args[ac], XmNleftAttachment, XmATTACH_FORM); ac++;
  XtSetArg(args[ac], XmNleftOffset, 20); ac++;
  XtSetArg(args[ac], XmNrightAttachment, XmATTACH_FORM); ac++;
  XtSetArg(args[ac], XmNrightOffset, 20); ac++;
  text_w = XtCreateManagedWidget("text_f_w", xmTextFieldWidgetClass,
				 ipAttrForm, args, ac);
  XtAddCallback(text_w, XmNactivateCallback, assign_port_name,port);
  ac = 0;

  /* Action buttons */
  XtSetArg(args[ac], XmNleftAttachment, XmATTACH_POSITION); ac++;
  XtSetArg(args[ac], XmNleftPosition, 10); ac++;
  XtSetArg(args[ac], XmNrightAttachment, XmATTACH_POSITION); ac++;    
  XtSetArg(args[ac], XmNrightPosition, 40); ac++;
  XtSetArg(args[ac], XmNbottomAttachment, XmATTACH_FORM); ac++;
  XtSetArg(args[ac], XmNbottomOffset, 10); ac++;
  XtSetArg(args[ac], XmNuserData, text_w); ac++;
  pushb0 = XtCreateManagedWidget("Ok", xmPushButtonWidgetClass,
				 ipAttrForm, args, ac);
  
  XtSetArg(args[ac], XmNleftAttachment, XmATTACH_POSITION); ac++;
  XtSetArg(args[ac], XmNleftPosition, 60); ac++;
  XtSetArg(args[ac], XmNrightAttachment, XmATTACH_POSITION); ac++;    
  XtSetArg(args[ac], XmNrightPosition, 90); ac++;
  XtSetArg(args[ac], XmNbottomAttachment, XmATTACH_FORM); ac++;
  XtSetArg(args[ac], XmNbottomOffset, 10); ac++;
  pushb1 = XtCreateManagedWidget("Cancel", xmPushButtonWidgetClass,
				 ipAttrForm, args, ac);

  /* separator widget */
  XtSetArg(args[ac], XmNleftAttachment, XmATTACH_FORM); ac++;
  XtSetArg(args[ac], XmNleftOffset, 2); ac++;
  XtSetArg(args[ac], XmNrightAttachment, XmATTACH_FORM); ac++;
  XtSetArg(args[ac], XmNrightOffset, 2); ac++;
  XtSetArg(args[ac], XmNbottomAttachment, XmATTACH_WIDGET); ac++;
  XtSetArg(args[ac], XmNbottomWidget, pushb0); ac++;
  XtSetArg(args[ac], XmNbottomOffset, 10); ac++;
  sep = XtCreateManagedWidget("sep",xmSeparatorWidgetClass,
			      ipAttrForm, args, ac);
  ac = 0;
  
  /* Set resources for text window and toplabel */
  XtSetArg(args[ac], XmNbottomAttachment, XmATTACH_WIDGET); ac++;
  XtSetArg(args[ac], XmNbottomWidget, sep); ac++;
  XtSetArg(args[ac], XmNbottomOffset, 10); ac++;
  XtSetValues(text_w, args, ac);
  ac = 0;
  
  XtSetArg(args[ac], XmNbottomAttachment, XmATTACH_WIDGET); ac++;
  XtSetArg(args[ac], XmNbottomWidget, text_w); ac++;
  XtSetArg(args[ac], XmNbottomOffset, 10); ac++;
  XtSetValues(top_label, args, ac);
  ac = 0;
  
  /* add callbacks for two pushbuttons */
  XtAddCallback(pushb0, XmNactivateCallback, assign_port_name, port);
  XtAddCallback(pushb1, XmNactivateCallback, popdown_name_entry_shell,
		d_shell);
  
  XtManageChild(ipAttrForm);
  
  XtPopup(d_shell, XtGrabNone);
}

/********************************************************************/
/************************************************************
 *    static void script_dialog ()                          *
 * Description:                                             *
 *       popup a script dialog box                          *
 ***********************************************************/
#if defined (__STDC__)
static void script_dialog (Widget w, XtPointer data,
			   XmAnyCallbackStruct* cbs)
#else
static void script_dialog (w, data, cbs)
     Widget w;
     XtPointer data;
     XmAnyCallbackStruct* cbs;
#endif
{
  AttrWidgets* widgets = (AttrWidgets *)data;
  popup_script_dialog (widgets->comp, sw_geometry.draw_area, 
		       widgets);
}


/************************************************************
 *    static void popdown_comp_attr_shell()                 *
 * Description:                                             *
 *    popdown component's attributes shell                  *
 ***********************************************************/
#if defined (__STDC__)
static void popdown_comp_attr_shell(Widget w, 
				    AttrWidgets* widgets, 
				    XmAnyCallbackStruct* cbs)
#else
static void popdown_comp_attr_shell(w, widgets, cbs)
     Widget w;
     AttrWidgets *widgets;     
     XmAnyCallbackStruct *cbs;
#endif
{
  Arg    args[10];
  int    ac = 0, type;
  Widget shell = XtParent(XtParent(w));
  drawComp *comp = widgets->comp;

  /* Get type of this dialog */
  XtSetArg (args[ac], XmNuserData, &type); ac++;
  XtGetValues (w, args, ac);
  ac = 0;
  if (type != 0){ /* Existing components dialog */
    XtRemoveGrab(shell);
    XtDestroyWidget(shell);
  }
  else{ /* First popup. Cancel will remove this component from the list */
    removeNodeFromCompList(&coda_graph, comp);
    (*comp->erase)(comp, xgc.dpy, XtWindow(sw_geometry.draw_area)); 
    /* free memories for this component */
    free_daqcomp_res (&(comp->comp));
    free_ipPort_res (comp->ip_port, comp->num_ports);
    free (comp);
    comp = (drawComp *)0;

    XtRemoveGrab(shell);
    XtDestroyWidget(shell);
  }
}

#if defined (__STDC__)
static void setup_comp_attr(Widget w, 
			    AttrWidgets* widgets, 
			    XmAnyCallbackStruct* cbs)
#else
static void setup_comp_attr(w, widgets, cbs)
     Widget w;
     AttrWidgets *widgets;
     XmAnyCallbackStruct *cbs;
#endif
{
  char     *str;
  char     msg[256];
  Widget   shell = XtParent(XtParent(w));
  daqComp  *daq = &(widgets->comp->comp);
  drawComp *comp = widgets->comp;
  ipPort   *port = &(widgets->comp->ip_port[0]);
  int      id_num, i, type;
  Arg      arg[10];
  int      ac = 0;

  /* get type field, check whether this is for adding node or editing node */
  XtSetArg (arg[ac], XmNuserData, &type); ac++;
  XtGetValues (w, arg, ac);
  ac = 0;

  str = XmTextFieldGetString(widgets->name_widget);
  if(!str || !*str){
    pop_error_message("Nothing typed for component name!", w);
    return;
  }
  if(daq->comp_name != NULL){
    (*comp->erase_name)(comp, xgc.dpy, XtWindow(sw_geometry.draw_area));
    free(daq->comp_name);
    daq->comp_name = (char *)0;
  }
  daq->comp_name = strsave(str);
  XtFree(str);
  
  if (type == 0 && !compNameOk (comp, daq->comp_name)){
    sprintf(msg, "Duplicated component name \"%s\"", daq->comp_name);
    free (daq->comp_name);
    daq->comp_name = (char *)0;
    pop_error_message(msg, w);
    return;
  }
  if (daq->type < 11 ){
    (*comp->write_name)(comp, xgc.dpy, XtWindow(sw_geometry.draw_area));
  }
  
  str = XmTextFieldGetString(widgets->host_widget);
  if(!str || !*str){
    pop_error_message("Nothing typed for Ethernet Host !", w);
    return;
  }
  if (!compHostOk (comp, str)){
    sprintf(msg, "Cannot find host machine \"%s\"",str);
    pop_error_message (msg, w);
    return;
  }
  if(daq->node_name != NULL){
    (*comp->erase_hostname)(comp, xgc.dpy, XtWindow(sw_geometry.draw_area));
    free(daq->node_name);
    daq->node_name = (char *)0;
  }
  if (daq->type <11) {
    daq->node_name = strsave(str);
    (*comp->write_hostname)(comp, xgc.dpy, XtWindow(sw_geometry.draw_area));
    if (!type) {/* add new node mode */
      /* add default port name */
      for (i = 0; i < comp->num_ports; i++){
	if (comp->ip_port[i].ip_addr == NULL)
	  comp->ip_port[i].ip_addr = strsave (daq->node_name);
      }
    }
    XtFree(str);
  } else {
    printf("here for %s\n",daq->comp_name);
    daq->node_name = strdup(daq->comp_name);
    comp->num_ports = 1;
    comp->ip_port[0].ip_addr = strdup(daq->comp_name);
  }
  
  str = XmTextFieldGetString(widgets->id_widget);
  if(!str || !*str){
    pop_error_message("Nothing typed for id number !", w);
    return;
  }
  if((sscanf(str, "%d", &id_num)) < 1){
    pop_error_message("Wrong syntax for id number !", w);
    return;
  }
  if(id_num < 0) {
    pop_error_message("Id number must be > 0 !", w);
    return;
  }
  if (!compIdOk (comp, id_num)){
    sprintf(msg,"Duplicated id number %d",id_num);
    pop_error_message (msg, w);
    return;
  }
  daq->id_num = id_num;
  XtFree(str);

  str = XmTextFieldGetString(widgets->boot_widget);
  /* allow empty boot string */
  if(daq->boot_string != NULL)
    free(daq->boot_string);
  if(!str || !*str)
    daq->boot_string = 0;
  else
    daq->boot_string = strsave (str);
  XtFree(str);

  str = XmTextFieldGetString(widgets->code_widget[0]);
  if(daq->type == CODA_ROC){
    if(!str || !*str) {
      pop_error_message("Nothing typed for ROC code !", w);
      return;
    }
  }

  if(daq->code[0] != NULL) {
    free(daq->code[0]);
    daq->code[0] = 0;
  }
  if (str && *str) 
    daq->code[0] = strsave(str);
  else
    daq->code[0] = 0;
  XtFree(str);

  if(daq->code[1] != NULL) {
    free(daq->code[1]);
    daq->code[1] = 0;
  }
  str = XmTextFieldGetString(widgets->code_widget[1]);
  if (str && *str) 
    daq->code[1] = strsave(str);
  else
    daq->code[1] = 0;
  XtFree(str);

  if(daq->code[2] != NULL) {
    free(daq->code[2]);
    daq->code[2] = 0;
  }
  str = XmTextFieldGetString(widgets->code_widget[2]);
  if (str && *str) 
    daq->code[2] = strsave(str);
  else
    daq->code[2] = 0;
  XtFree(str);

  if (type){ /* editing mode */
    updateInfoToIconList(comp);
  }

  XtRemoveGrab(shell);
  XtDestroyWidget(shell);
  /* split coda_ebana into two parts, one eb and one ana */
  if (daq->type == CODA_EBANA)
    SplitEbana (comp);
}

/*********************************************************************
 *       void popup_comp_attributes(comp, base, event, type)         *
 * Description:                                                      *
 *    Popup dialog for setting up component sttributes               *
 *    type == 0 for creation                                         *
 *    type == 1 for later modiofication                              *
 ********************************************************************/
static AttrWidgets atw;
    
#if defined (__STDC__)
void popup_comp_attributes(drawComp* comp, 
			   Widget base, 
			   XEvent* event,
			   int type)
#else
void popup_comp_attributes(comp, base, event,type)
     drawComp *comp;
     Widget   base;
     XEvent   *event;
     int      type;
#endif
{
  Position ret_x, ret_y;
  Arg      args[20];
  int      ac = 0, i;
  ipPort   *port;
  daqComp  *daq;
  Widget   shell, form, sub_form0, sub_form1, sub_form2;
  Widget   sub_form3, sub_form4, sub_form5, sub_form6;
  Widget   left_label,sep;
  Widget   text_w0, text_w1, text_w2, text_w3, text_w4, text_w5, text_w6;
  Widget   label_w0, label_w1, label_w2, label_w3, label_w4, label_w5, label_w6;
  XmString t;
  Dimension wwd;
  char     hname[80];

  /* first get current hostname */
  if (gethostname (hname, sizeof (hname)) != 0)
    strcpy (hname, "unknown");
  

  XtTranslateCoords(base,comp->x + comp->width, comp->y, &ret_x, &ret_y);

  XtSetArg(args[ac], XmNx, 100); ac++;
  XtSetArg(args[ac], XmNy, 200); ac++;
  XtSetArg(args[ac], XmNtitle, "Component Attributes"); ac++;
  XtSetArg(args[ac], XmNdefaultPosition, False); ac++;
  XtSetArg(args[ac], XmNautoUnmanage, False); ac++;
  form = XmCreateFormDialog(base, "attributes_form", args, ac);
  shell = XtParent(form);

  if (comp->comp.type != CODA_EBANA){
    XtSetArg(args[ac], XmNtopAttachment, XmATTACH_FORM); ac++;
    XtSetArg(args[ac], XmNtopOffset, 10); ac++;
    XtSetArg(args[ac], XmNleftAttachment, XmATTACH_FORM); ac++;
    XtSetArg(args[ac], XmNleftOffset, 5); ac++;
    XtSetArg(args[ac], XmNrightAttachment, XmATTACH_FORM); ac++;
    XtSetArg(args[ac], XmNrightOffset, 5); ac++;
    sub_form0 = XtCreateWidget("sub_form", xmFormWidgetClass,
			       form, args, ac);
    ac = 0;
    t = XmStringCreateSimple("Name:");
    XtSetArg(args[ac], XmNtopAttachment, XmATTACH_FORM); ac++;
    XtSetArg(args[ac], XmNbottomAttachment, XmATTACH_FORM); ac++;
    XtSetArg(args[ac], XmNleftAttachment, XmATTACH_FORM); ac++;
    XtSetArg(args[ac], XmNleftOffset, 5); ac++;
    XtSetArg(args[ac], XmNrightAttachment, XmATTACH_POSITION); ac++;
    XtSetArg(args[ac], XmNrightPosition, 20); ac++;
    XtSetArg(args[ac], XmNlabelString, t); ac++;
    XtSetArg(args[ac], XmNalignment, XmALIGNMENT_END); ac++;
    label_w0 = XtCreateManagedWidget("label_w0", xmLabelWidgetClass,
				       sub_form0, args, ac);
    ac = 0;
    XmStringFree(t);

    XtSetArg(args[ac], XmNtopAttachment, XmATTACH_FORM); ac++;
    XtSetArg(args[ac], XmNbottomAttachment, XmATTACH_FORM); ac++;
    XtSetArg(args[ac], XmNleftAttachment, XmATTACH_WIDGET); ac++;
    XtSetArg(args[ac], XmNleftWidget, label_w0); ac++;
    XtSetArg(args[ac], XmNleftOffset, 2); ac++;
    XtSetArg(args[ac], XmNrightAttachment, XmATTACH_FORM); ac++;
    XtSetArg(args[ac], XmNrightOffset, 5); ac++;  
    text_w0 = XtCreateManagedWidget("text_window0", xmTextFieldWidgetClass,
				    sub_form0, args, ac);
    ac = 0;
    XtManageChild(sub_form0);
  }
    
  if (comp->comp.type != CODA_EBANA){
    XtSetArg(args[ac], XmNtopAttachment, XmATTACH_WIDGET); ac++;
    XtSetArg(args[ac], XmNtopWidget, sub_form0); ac++;
  }
  else{
    XtSetArg(args[ac], XmNtopAttachment, XmATTACH_FORM); ac++;
  }
  
  XtSetArg(args[ac], XmNleftAttachment, XmATTACH_FORM); ac++;
  XtSetArg(args[ac], XmNleftOffset, 5); ac++;
  XtSetArg(args[ac], XmNrightAttachment, XmATTACH_FORM); ac++;  
  XtSetArg(args[ac], XmNrightOffset, 5); ac++;
  sub_form1 = XtCreateWidget("sub_form", xmFormWidgetClass,
			     form, args, ac);
  ac = 0;

  t = XmStringCreateSimple("Ethernet Host:");
  XtSetArg(args[ac], XmNtopAttachment, XmATTACH_FORM); ac++;
  XtSetArg(args[ac], XmNbottomAttachment, XmATTACH_FORM); ac++;
  XtSetArg(args[ac], XmNleftAttachment, XmATTACH_FORM); ac++;
  XtSetArg(args[ac], XmNleftOffset, 5); ac++;
  XtSetArg(args[ac], XmNrightAttachment, XmATTACH_POSITION); ac++;
  XtSetArg(args[ac], XmNrightPosition, 20); ac++;
  XtSetArg(args[ac], XmNlabelString, t); ac++;
  XtSetArg(args[ac], XmNalignment, XmALIGNMENT_END); ac++;
  label_w1 = XtCreateManagedWidget("left_label", xmLabelWidgetClass,
				     sub_form1, args, ac);
  ac = 0;
  XmStringFree(t);  
  XtSetArg(args[ac], XmNtopAttachment, XmATTACH_FORM); ac++;
  XtSetArg(args[ac], XmNbottomAttachment, XmATTACH_FORM); ac++;
  XtSetArg(args[ac], XmNleftAttachment, XmATTACH_WIDGET); ac++;
  XtSetArg(args[ac], XmNleftWidget, label_w1); ac++;
  XtSetArg(args[ac], XmNleftOffset, 2); ac++;
  XtSetArg(args[ac], XmNrightAttachment, XmATTACH_FORM); ac++;
  XtSetArg(args[ac], XmNrightOffset, 5); ac++;  
  text_w1 = XtCreateManagedWidget("text_window1", xmTextFieldWidgetClass,
				  sub_form1, args, ac);
  ac = 0;
  XtManageChild(sub_form1);  

  XtSetArg(args[ac], XmNtopAttachment, XmATTACH_WIDGET); ac++;
  XtSetArg(args[ac], XmNtopWidget, sub_form1); ac++;
  XtSetArg(args[ac], XmNleftAttachment, XmATTACH_FORM); ac++;
  XtSetArg(args[ac], XmNleftOffset, 5); ac++;
  XtSetArg(args[ac], XmNrightAttachment, XmATTACH_FORM); ac++;  
  XtSetArg(args[ac], XmNrightOffset, 5); ac++;
  sub_form2 = XtCreateWidget("sub_form", xmFormWidgetClass,
			     form, args, ac);
  ac = 0;
  t = XmStringCreateSimple("Id Number:");
  XtSetArg(args[ac], XmNtopAttachment, XmATTACH_FORM); ac++;
  XtSetArg(args[ac], XmNbottomAttachment, XmATTACH_FORM); ac++;
  XtSetArg(args[ac], XmNleftAttachment, XmATTACH_FORM); ac++;
  XtSetArg(args[ac], XmNleftOffset, 5); ac++;
  XtSetArg(args[ac], XmNrightAttachment, XmATTACH_POSITION); ac++;
  XtSetArg(args[ac], XmNrightPosition, 20); ac++;
  XtSetArg(args[ac], XmNlabelString, t); ac++;
  XtSetArg(args[ac], XmNalignment, XmALIGNMENT_END); ac++;
  label_w2 = XtCreateManagedWidget("left_label", xmLabelWidgetClass,
				     sub_form2, args, ac);
  ac = 0;
  XmStringFree(t);  
  XtSetArg(args[ac], XmNtopAttachment, XmATTACH_FORM); ac++;
  XtSetArg(args[ac], XmNbottomAttachment, XmATTACH_FORM); ac++;
  XtSetArg(args[ac], XmNleftAttachment, XmATTACH_WIDGET); ac++;
  XtSetArg(args[ac], XmNleftWidget, label_w2); ac++;
  XtSetArg(args[ac], XmNleftOffset, 2); ac++;
  XtSetArg(args[ac], XmNrightAttachment, XmATTACH_FORM); ac++;
  XtSetArg(args[ac], XmNrightOffset, 5); ac++;  
  text_w2 = XtCreateManagedWidget("text_window2", xmTextFieldWidgetClass,
				  sub_form2, args, ac);
  ac = 0;
  XtManageChild(sub_form2);  

  XtSetArg(args[ac], XmNtopAttachment, XmATTACH_WIDGET); ac++;
  XtSetArg(args[ac], XmNtopWidget, sub_form2); ac++;
  XtSetArg(args[ac], XmNleftAttachment, XmATTACH_FORM); ac++;
  XtSetArg(args[ac], XmNleftOffset, 5); ac++;
  XtSetArg(args[ac], XmNrightAttachment, XmATTACH_FORM); ac++;  
  XtSetArg(args[ac], XmNrightOffset, 5); ac++;
  sub_form3 = XtCreateWidget("sub_form", xmFormWidgetClass,
			     form, args, ac);
  ac = 0;
  t = XmStringCreateSimple("Booting String:");
  XtSetArg(args[ac], XmNtopAttachment, XmATTACH_FORM); ac++;
  XtSetArg(args[ac], XmNbottomAttachment, XmATTACH_FORM); ac++;
  XtSetArg(args[ac], XmNleftAttachment, XmATTACH_FORM); ac++;
  XtSetArg(args[ac], XmNleftOffset, 5); ac++;
  XtSetArg(args[ac], XmNrightAttachment, XmATTACH_POSITION); ac++;
  XtSetArg(args[ac], XmNrightPosition, 20); ac++;
  XtSetArg(args[ac], XmNlabelString, t); ac++;
  XtSetArg(args[ac], XmNalignment, XmALIGNMENT_END); ac++;
  label_w3 = XtCreateManagedWidget("left_label", xmLabelWidgetClass,
				     sub_form3, args, ac);
  ac = 0;
  XmStringFree(t);  
  XtSetArg(args[ac], XmNtopAttachment, XmATTACH_FORM); ac++;
  XtSetArg(args[ac], XmNbottomAttachment, XmATTACH_FORM); ac++;
  XtSetArg(args[ac], XmNleftAttachment, XmATTACH_WIDGET); ac++;
  XtSetArg(args[ac], XmNleftWidget, label_w3); ac++;
  XtSetArg(args[ac], XmNleftOffset, 2); ac++;
  XtSetArg(args[ac], XmNrightAttachment, XmATTACH_FORM); ac++;
  XtSetArg(args[ac], XmNrightOffset, 5); ac++;  
  text_w3 = XtCreateManagedWidget("text_window3", xmTextFieldWidgetClass,
				  sub_form3, args, ac);
  ac = 0;
  XtManageChild(sub_form3);  

  XtSetArg(args[ac], XmNtopAttachment, XmATTACH_WIDGET); ac++;
  XtSetArg(args[ac], XmNtopWidget, sub_form3); ac++;
  XtSetArg(args[ac], XmNleftAttachment, XmATTACH_FORM); ac++;
  XtSetArg(args[ac], XmNleftOffset, 5); ac++;
  XtSetArg(args[ac], XmNrightAttachment, XmATTACH_FORM); ac++;  
  XtSetArg(args[ac], XmNrightOffset, 5); ac++;
  sub_form4 = XtCreateWidget("sub_form", xmFormWidgetClass,
			     form, args, ac);
  ac = 0;
  t = XmStringCreateSimple("Readout List:");
  XtSetArg(args[ac], XmNtopAttachment, XmATTACH_FORM); ac++;
  XtSetArg(args[ac], XmNbottomAttachment, XmATTACH_FORM); ac++;
  XtSetArg(args[ac], XmNleftAttachment, XmATTACH_FORM); ac++;
  XtSetArg(args[ac], XmNleftOffset, 5); ac++;
  XtSetArg(args[ac], XmNrightAttachment, XmATTACH_POSITION); ac++;
  XtSetArg(args[ac], XmNrightPosition, 20); ac++;
  XtSetArg(args[ac], XmNlabelString, t); ac++;
  XtSetArg(args[ac], XmNalignment, XmALIGNMENT_END); ac++;
  label_w4 = XtCreateManagedWidget("left_label", xmLabelWidgetClass,
				     sub_form4, args, ac);
  ac = 0;
  XmStringFree(t);  
  XtSetArg(args[ac], XmNtopAttachment, XmATTACH_FORM); ac++;
  XtSetArg(args[ac], XmNbottomAttachment, XmATTACH_FORM); ac++;
  XtSetArg(args[ac], XmNleftAttachment, XmATTACH_WIDGET); ac++;
  XtSetArg(args[ac], XmNleftWidget, label_w4); ac++;
  XtSetArg(args[ac], XmNleftOffset, 2); ac++;
  XtSetArg(args[ac], XmNrightAttachment, XmATTACH_FORM); ac++;
  XtSetArg(args[ac], XmNrightOffset, 5); ac++;  
  XtSetArg(args[ac], XmNcolumns, 40); ac++;
  text_w4 = XtCreateManagedWidget("text_window4", xmTextFieldWidgetClass,
				  sub_form4, args, ac);
  ac = 0;
  XtManageChild(sub_form4);  

  XtSetArg(args[ac], XmNtopAttachment, XmATTACH_WIDGET); ac++;
  XtSetArg(args[ac], XmNtopWidget, sub_form4); ac++;
  XtSetArg(args[ac], XmNleftAttachment, XmATTACH_FORM); ac++;
  XtSetArg(args[ac], XmNleftOffset, 5); ac++;
  XtSetArg(args[ac], XmNrightAttachment, XmATTACH_FORM); ac++;  
  XtSetArg(args[ac], XmNrightOffset, 5); ac++;
  sub_form5 = XtCreateWidget("sub_form", xmFormWidgetClass,
			     form, args, ac);
  ac = 0;
  t = XmStringCreateSimple("Readout List:");
  XtSetArg(args[ac], XmNtopAttachment, XmATTACH_FORM); ac++;
  XtSetArg(args[ac], XmNbottomAttachment, XmATTACH_FORM); ac++;
  XtSetArg(args[ac], XmNleftAttachment, XmATTACH_FORM); ac++;
  XtSetArg(args[ac], XmNleftOffset, 5); ac++;
  XtSetArg(args[ac], XmNrightAttachment, XmATTACH_POSITION); ac++;
  XtSetArg(args[ac], XmNrightPosition, 20); ac++;
  XtSetArg(args[ac], XmNlabelString, t); ac++;
  XtSetArg(args[ac], XmNalignment, XmALIGNMENT_END); ac++;
  label_w5 = XtCreateManagedWidget("left_label", xmLabelWidgetClass,
				     sub_form5, args, ac);
  ac = 0;
  XmStringFree(t);  
  XtSetArg(args[ac], XmNtopAttachment, XmATTACH_FORM); ac++;
  XtSetArg(args[ac], XmNbottomAttachment, XmATTACH_FORM); ac++;
  XtSetArg(args[ac], XmNleftAttachment, XmATTACH_WIDGET); ac++;
  XtSetArg(args[ac], XmNleftWidget, label_w5); ac++;
  XtSetArg(args[ac], XmNleftOffset, 2); ac++;
  XtSetArg(args[ac], XmNrightAttachment, XmATTACH_FORM); ac++;
  XtSetArg(args[ac], XmNrightOffset, 5); ac++;  
  text_w5 = XtCreateManagedWidget("text_window5", xmTextFieldWidgetClass,
				  sub_form5, args, ac);
  ac = 0;
  XtManageChild(sub_form5);  

  XtSetArg(args[ac], XmNtopAttachment, XmATTACH_WIDGET); ac++;
  XtSetArg(args[ac], XmNtopWidget, sub_form5); ac++;
  XtSetArg(args[ac], XmNleftAttachment, XmATTACH_FORM); ac++;
  XtSetArg(args[ac], XmNleftOffset, 5); ac++;
  XtSetArg(args[ac], XmNrightAttachment, XmATTACH_FORM); ac++;  
  XtSetArg(args[ac], XmNrightOffset, 5); ac++;
  sub_form6 = XtCreateWidget("sub_form", xmFormWidgetClass,
			     form, args, ac);
  ac = 0;
  t = XmStringCreateSimple("Readout List:");
  XtSetArg(args[ac], XmNtopAttachment, XmATTACH_FORM); ac++;
  XtSetArg(args[ac], XmNbottomAttachment, XmATTACH_FORM); ac++;
  XtSetArg(args[ac], XmNleftAttachment, XmATTACH_FORM); ac++;
  XtSetArg(args[ac], XmNleftOffset, 5); ac++;
  XtSetArg(args[ac], XmNrightAttachment, XmATTACH_POSITION); ac++;
  XtSetArg(args[ac], XmNrightPosition, 20); ac++;
  XtSetArg(args[ac], XmNlabelString, t); ac++;
  XtSetArg(args[ac], XmNalignment, XmALIGNMENT_END); ac++;
  label_w6 = XtCreateManagedWidget("left_label", xmLabelWidgetClass,
				     sub_form6, args, ac);
  ac = 0;
  XmStringFree(t);  
  XtSetArg(args[ac], XmNtopAttachment, XmATTACH_FORM); ac++;
  XtSetArg(args[ac], XmNbottomAttachment, XmATTACH_FORM); ac++;
  XtSetArg(args[ac], XmNleftAttachment, XmATTACH_WIDGET); ac++;
  XtSetArg(args[ac], XmNleftWidget, label_w6); ac++;
  XtSetArg(args[ac], XmNleftOffset, 2); ac++;
  XtSetArg(args[ac], XmNrightAttachment, XmATTACH_FORM); ac++;
  XtSetArg(args[ac], XmNrightOffset, 5); ac++;  
  text_w6 = XtCreateManagedWidget("text_window6", xmTextFieldWidgetClass,
				  sub_form6, args, ac);
  ac = 0;
  XtManageChild(sub_form6);  

  if (comp->comp.type != CODA_EBANA)
    atw.name_widget = text_w0;
  else
    atw.name_widget = 0;

  atw.host_widget = text_w1;
  atw.id_widget = text_w2;
  atw.boot_widget = text_w3;

  atw.code_widget[0] = text_w4;
  atw.code_widget[1] = text_w5;
  atw.code_widget[2] = text_w6;    
  atw.comp = comp;

  XtSetArg(args[ac], XmNtopAttachment, XmATTACH_WIDGET); ac++;
  XtSetArg(args[ac], XmNtopWidget, sub_form6); ac++;
  XtSetArg(args[ac], XmNtopOffset, 10); ac++;
  XtSetArg(args[ac], XmNleftAttachment, XmATTACH_FORM); ac++;
  XtSetArg(args[ac], XmNleftOffset, 2); ac++;
  XtSetArg(args[ac], XmNrightAttachment, XmATTACH_FORM); ac++;
  XtSetArg(args[ac], XmNrightOffset, 2); ac++;
  sep = XtCreateManagedWidget("sep", xmSeparatorWidgetClass,
			      form, args, ac);
  ac = 0;

  t = XmStringCreateSimple("    Ok    ");
  XtSetArg(args[ac], XmNtopAttachment, XmATTACH_NONE); ac++;
  XtSetArg(args[ac], XmNleftAttachment, XmATTACH_POSITION); ac++;
  XtSetArg(args[ac], XmNleftPosition, 10); ac++;
  XtSetArg(args[ac], XmNbottomAttachment, XmATTACH_FORM); ac++;
  XtSetArg(args[ac], XmNbottomOffset, 5); ac++;
  XtSetArg(args[ac], XmNlabelString, t); ac++;
  XtSetArg(args[ac], XmNuserData, type); ac++;
  XtSetArg(args[ac], XmNshowAsDefault, 1); ac++;
  atw.ok_widget = XtCreateManagedWidget("ok_button",xmPushButtonWidgetClass,
					form, args, ac);
  ac = 0;
  XmStringFree(t);

  t = XmStringCreateSimple(" Dismiss ");
  XtSetArg(args[ac], XmNtopAttachment, XmATTACH_NONE); ac++;
  XtSetArg(args[ac], XmNbottomAttachment, XmATTACH_FORM); ac++;
  XtSetArg(args[ac], XmNbottomOffset, 10); ac++;
  XtSetArg(args[ac], XmNlabelString, t); ac++;
  XtSetArg(args[ac], XmNuserData, type); ac++;
  atw.cancel_widget = XtCreateManagedWidget("cancel_button",
					    xmPushButtonWidgetClass,
					    form, args, ac);
  ac = 0;
  XmStringFree(t);
  /* try to center this widget */
  XtSetArg (args[ac], XmNwidth, &wwd); ac++;
  XtGetValues (atw.cancel_widget, args, ac);
  ac = 0;
  XtSetArg (args[ac], XmNleftAttachment, XmATTACH_POSITION); ac++;
  XtSetArg (args[ac], XmNleftPosition, 50); ac++;
  XtSetArg (args[ac], XmNleftOffset, -wwd/2); ac++;
  XtSetValues (atw.cancel_widget, args, ac);
  ac = 0;

  t = XmStringCreateSimple("Scripts...");
  XtSetArg(args[ac], XmNtopAttachment, XmATTACH_NONE); ac++;
  XtSetArg(args[ac], XmNrightAttachment, XmATTACH_POSITION); ac++;
  XtSetArg(args[ac], XmNrightPosition, 90); ac++;
  XtSetArg(args[ac], XmNbottomAttachment, XmATTACH_FORM); ac++;
  XtSetArg(args[ac], XmNbottomOffset, 10); ac++;
  XtSetArg(args[ac], XmNlabelString, t); ac++;
  XtSetArg(args[ac], XmNuserData, type); ac++;
  atw.script_widget = XtCreateManagedWidget("script_button",
					    xmPushButtonWidgetClass,
					    form, args, ac);
  ac = 0;
  XmStringFree(t);
  
  /* Set X window resoruces for separator */
  XtSetArg (args[ac], XmNbottomAttachment, XmATTACH_WIDGET); ac++;
  XtSetArg (args[ac], XmNbottomWidget, atw.ok_widget); ac++;
  XtSetArg (args[ac], XmNbottomOffset, 5); ac++;
  XtSetValues (sep, args, ac);
  ac = 0;
  

  if (comp->comp.type != CODA_EBANA)
    XtAddCallback(text_w0,XmNactivateCallback, XmProcessTraversal,
		  XmTRAVERSE_NEXT_TAB_GROUP);
  XtAddCallback(text_w1,XmNactivateCallback, XmProcessTraversal,
		XmTRAVERSE_NEXT_TAB_GROUP);
  XtAddCallback(text_w2,XmNactivateCallback, XmProcessTraversal,
		XmTRAVERSE_NEXT_TAB_GROUP);
  XtAddCallback(text_w3,XmNactivateCallback, XmProcessTraversal,
		XmTRAVERSE_NEXT_TAB_GROUP);
  XtAddCallback(text_w4,XmNactivateCallback, XmProcessTraversal,
		XmTRAVERSE_NEXT_TAB_GROUP);
  XtAddCallback(text_w5,XmNactivateCallback, XmProcessTraversal,
		XmTRAVERSE_NEXT_TAB_GROUP);
  XtAddCallback(text_w6,XmNactivateCallback, XmProcessTraversal,
		XmTRAVERSE_NEXT_TAB_GROUP);

  XtAddCallback(atw.ok_widget, XmNactivateCallback, setup_comp_attr, &atw);
  XtAddCallback(atw.cancel_widget, XmNactivateCallback, 
		popdown_comp_attr_shell,
		&atw);
  XtAddCallback(atw.script_widget, XmNactivateCallback, script_dialog,
		&atw);
		

/* set up initial values for all attributes */
  for (i = 0; i < 3; i++) {
    if(comp->comp.code[i] != NULL)
      XmTextFieldSetString(atw.code_widget[i], comp->comp.code[i]);
  }

 
  if (comp->comp.type == CODA_ROC) {
    
  if(comp->comp.boot_string != NULL)
    XmTextFieldSetString(text_w3, comp->comp.boot_string);
  else
    XmTextFieldSetString(text_w3, "$CODA_BIN/coda_roc");
  
  } else if (comp->comp.type == CODA_EB) {
    if(comp->comp.boot_string != NULL)
      XmTextFieldSetString(text_w3, comp->comp.boot_string);
    else
      XmTextFieldSetString(text_w3, "$CODA_BIN/coda_eb");

    t = XmStringCreateSimple("Incoming Format");
    ac = 0;
    XtSetArg(args[ac], XmNlabelString, t); ac++;
    XtSetValues (label_w4, args, ac);
    XmStringFree(t);

    if(comp->comp.code[0] != NULL)
      XmTextFieldSetString(text_w4, comp->comp.code[0]);
    else
      XmTextFieldSetString(text_w4, "CODA");

    t = XmStringCreateSimple("Outgoing Format");
    ac = 0;
    XtSetArg(args[ac], XmNlabelString, t); ac++;
    XtSetValues (label_w5, args, ac);
    XmStringFree(t);

    if(comp->comp.code[0] != NULL)
      XmTextFieldSetString(text_w5, comp->comp.code[0]);
    else
      XmTextFieldSetString(text_w5, "CODA");

    XtUnmanageChild(label_w6);
    XtUnmanageChild(text_w6);
  } else if (comp->comp.type == CODA_ER) {
    if(comp->comp.boot_string != NULL)
      XmTextFieldSetString(text_w3, comp->comp.boot_string);
    else
      XmTextFieldSetString(text_w3, "$CODA_BIN/coda_er");
    XtUnmanageChild(label_w4);
    XtUnmanageChild(text_w4);
    XtUnmanageChild(label_w5);
    XtUnmanageChild(text_w5);
    XtUnmanageChild(label_w6);
    XtUnmanageChild(text_w6);
    t = XmStringCreateSimple("Data Format");
    ac = 0;
    XtSetArg(args[ac], XmNlabelString, t); ac++;
    XtSetValues (label_w4, args, ac);
    XmStringFree(t);

    if(comp->comp.code[0] != NULL)
      XmTextFieldSetString(text_w4, comp->comp.code[0]);
    else
      XmTextFieldSetString(text_w4, "CODA");

    
  } else if ((comp->comp.type == CODA_FILE)||(comp->comp.type == CODA_CODAFILE)) {
    char     *temp = (char *)malloc(200);
    if (comp->comp.type == CODA_FILE) {
      sprintf (temp, "file_%d", outputFileNum);

    }

    if (comp->comp.type == CODA_CODAFILE) {
      sprintf (temp, "coda_%d", outputFileNum);
    }

    if(comp->comp.comp_name != NULL)
      XmTextFieldSetString(text_w0, comp->comp.comp_name);
    else
      XmTextFieldSetString(text_w0, temp);
    
    comp->editable = 0;

    if(comp->comp.node_name != NULL)
      XmTextFieldSetString(text_w1, comp->comp.node_name);
    else
      XmTextFieldSetString(text_w1, hname);

    sprintf (temp, "%d", outputFileNum++);
    if(comp->comp.node_name != NULL)
      XmTextFieldSetString(text_w2, (char *) comp->comp.id_num);
    else
      XmTextFieldSetString(text_w2, temp);
    XtUnmanageChild(label_w1);
    XtUnmanageChild(text_w1);
    XtUnmanageChild(label_w2);
    XtUnmanageChild(text_w2);
    XtUnmanageChild(label_w3);
    XtUnmanageChild(text_w3);
    XtUnmanageChild(label_w6);
    XtUnmanageChild(text_w6);

    t = XmStringCreateSimple("Data File Name");
    ac = 0;
    XtSetArg(args[ac], XmNlabelString, t); ac++;
    XtSetValues (label_w4, args, ac);
    XmStringFree(t);
    
    XmTextFieldSetString(text_w4, "test.dat");

    t = XmStringCreateSimple("Data File Format");
    ac = 0;
    XtSetArg(args[ac], XmNlabelString, t); ac++;
    XtSetValues (label_w5, args, ac);
    XmStringFree(t);
    if (comp->comp.type == CODA_FILE)
      XmTextFieldSetString(text_w5, "CODA");
    else 
      XmTextFieldSetString(text_w5, "CODA");

  }
  else if (comp->comp.type == CODA_MON) {
    char     *temp = (char *) malloc(200);
    XtUnmanageChild(label_w1);
    XtUnmanageChild(text_w1);
    XtUnmanageChild(label_w2);
    XtUnmanageChild(text_w2);
    XtUnmanageChild(label_w3);
    XtUnmanageChild(text_w3);
    XtUnmanageChild(label_w4);
    XtUnmanageChild(text_w4);
    XtUnmanageChild(label_w5);
    XtUnmanageChild(text_w5);
    XtUnmanageChild(label_w6);
    XtUnmanageChild(text_w6);
    
    sprintf (temp, "mon_system");
    if(comp->comp.comp_name != NULL)
      XmTextFieldSetString(text_w0, comp->comp.comp_name);
    else
      XmTextFieldSetString(text_w0, temp);

    if(comp->comp.node_name != NULL)
      XmTextFieldSetString(text_w1, comp->comp.node_name);
    else
      XmTextFieldSetString(text_w1, "MON");
    sprintf (temp, "%d", ddNum++);

    if(comp->comp.node_name != NULL)
      XmTextFieldSetString(text_w2, (char *) comp->comp.id_num);
    else
      XmTextFieldSetString(text_w2, temp);
    XmTextFieldSetString(text_w3, "$CODA_BIN/coda_mon");
    
  }
  else if (comp->comp.type == CODA_NONE) {
    char     *temp = (char *) malloc(200);
    XtUnmanageChild(label_w1);
    XtUnmanageChild(text_w1);
    XtUnmanageChild(label_w2);
    XtUnmanageChild(text_w2);
    XtUnmanageChild(label_w3);
    XtUnmanageChild(text_w3);
    XtUnmanageChild(label_w4);
    XtUnmanageChild(text_w4);
    XtUnmanageChild(label_w5);
    XtUnmanageChild(text_w5);
    XtUnmanageChild(label_w6);
    XtUnmanageChild(text_w6);
    
    comp->editable = 0;

    XmTextFieldSetString(text_w0, "none");

    XmTextFieldSetString(text_w1, "none");
    XmTextFieldSetString(text_w2, "1");
    XmTextFieldSetString(text_w3, "N/A");
    
  }
  else if (comp->comp.type == CODA_DEBUG) {
    char     *temp = (char *) malloc(200);
    sprintf (temp, "debugger_%d", debuggerNum);
    /*XtUnmanageChild(label_w0);
      XtUnmanageChild(text_w0);*/
    XtUnmanageChild(label_w1);
    XtUnmanageChild(text_w1);
    XtUnmanageChild(label_w2);
    XtUnmanageChild(text_w2);
    XtUnmanageChild(label_w3);
    XtUnmanageChild(text_w3);
    XtUnmanageChild(label_w4);
    XtUnmanageChild(text_w4);
    XtUnmanageChild(label_w5);
    XtUnmanageChild(text_w5);
    XtUnmanageChild(label_w6);
    XtUnmanageChild(text_w6);
    
    if(comp->comp.comp_name != NULL)
      XmTextFieldSetString(text_w0, comp->comp.comp_name);
    else
      XmTextFieldSetString(text_w0, temp);

    if(comp->comp.node_name != NULL)
      XmTextFieldSetString(text_w1, comp->comp.node_name);
    else
      XmTextFieldSetString(text_w1, "debug");
    
    sprintf (temp, "%d", debuggerNum++);

    if(comp->comp.node_name != NULL)
      XmTextFieldSetString(text_w2, (char *) comp->comp.id_num);
    else
      XmTextFieldSetString(text_w2, temp);
    
    XmTextFieldSetString(text_w3, "N/A");
  }
  
  if(comp->comp.id_num >= 0){
    char     *temp = (char *) malloc(200);
    sprintf(temp,"%d",comp->comp.id_num);
    XmTextFieldSetString(text_w2, temp);
  }
  
  if(comp->comp.node_name != NULL)
    XmTextFieldSetString(text_w1, comp->comp.node_name);

  if(comp->comp.comp_name != NULL)
    XmTextFieldSetString(text_w0, comp->comp.comp_name);

  /* setup sensitivity according to drawComp editable flag */
  if(!comp->editable){
    if (comp->comp.type != CODA_EBANA)
      XmTextFieldSetEditable(text_w0, False);    
  }

  XtManageChild(form);
  XtAddGrab(shell, True, False);
  XtPopup(shell, XtGrabNone);
}


/************************************************************/
typedef struct _name_widget{
  Widget name_widget;
  ipPort *from_port;
  ipPort *to_port;
}NameWidget;

#if defined (__STDC__)
static void setup_port_name(Widget w, 
			    NameWidget* nw, 
			    XmAnyCallbackStruct* cbs)
#else
static void setup_port_name(w, nw, cbs)
     Widget w;
     NameWidget *nw;
     XmAnyCallbackStruct *cbs;
#endif
{
  char *str;
  char msg[80];
  Widget shell = XtParent(XtParent(w));

  str = XmTextFieldGetString(nw->name_widget);
  if(!str || !*str){
    pop_error_message("Empty ip port name !", w);
    return;
  }
  if(nw->from_port->ip_addr == NULL){
    nw->from_port->ip_addr = strsave(str);
    (*nw->from_port->write_name)(nw->from_port, xgc.dpy, 
				 XtWindow(sw_geometry.draw_area));
  }
  else if(nw->to_port->ip_addr == NULL){
    nw->to_port->ip_addr = strsave(str);
    (*nw->to_port->write_name)(nw->to_port, xgc.dpy,
			       XtWindow(sw_geometry.draw_area));
  }
  
  XtRemoveGrab(shell);
  if(nw->to_port->ip_addr == NULL)
    popup_ipport_name(nw->from_port, nw->to_port, XtParent(shell));
  
  XtDestroyWidget(shell);
}

/*************************************************************
 *      void popup_ipport_name(from_port, to_port, base)     *
 * Description:                                              *
 *    popup a dialog to enter from port an to port names     *
 *    if there is no name associated with these ports        * 
 ************************************************************/
#if defined (__STDC__)
void popup_ipport_name(ipPort* from_port, 
		       ipPort* to_port, 
		       Widget base)
#else
void popup_ipport_name(from_port, to_port, base)
     ipPort *from_port;
     ipPort *to_port;
     Widget base;
#endif
{
  Position ret_x, ret_y;
  Widget d_shell, text_w, pushb0;
  Widget form,top_label, sep;
  Arg    args[20];
  int    ac = 0;  
  XmString t = XmStringCreateSimple("Enter Ip Port Name:");
  static NameWidget *nw = 0;

  if (nw){
    free (nw);
    nw = 0;
  }
  nw = (NameWidget *)malloc(sizeof(NameWidget));
  if(nw == NULL){
    fprintf(stderr,"Cannot allocate memory for port name widget");
    exit(1);
  }
  nw->from_port = from_port;
  nw->to_port = to_port;
  if(from_port->ip_addr == NULL && to_port->ip_addr != NULL){
    XtTranslateCoords(base, from_port->right_x, from_port->right_y, 
		      &ret_x, &ret_y);
  }
  else if(from_port->ip_addr != NULL && to_port->ip_addr == NULL){
    XtTranslateCoords(base, to_port->left_x, to_port->left_y, 
		      &ret_x, &ret_y);
  }
  else if(from_port->ip_addr == NULL && to_port->ip_addr == NULL){
    XtTranslateCoords(base, from_port->right_x, from_port->right_y, 
		      &ret_x, &ret_y);
  }
  else
    return;

  XtSetArg(args[ac], XmNx, 100); ac++;
  XtSetArg(args[ac], XmNy, 200); ac++;
  XtSetArg(args[ac], XmNtitle,"IpPort Name"); ac++;
  XtSetArg(args[ac], XmNdefaultPosition, False); ac++;
  XtSetArg(args[ac], XmNresizePolicy, XmRESIZE_NONE); ac++;
  XtSetArg(args[ac], XmNnoResize, True); ac++;
  form = XmCreateFormDialog(base, "ip_form", args, ac);
  ac = 0;
  d_shell = XtParent(form);

  XtSetArg(args[ac], XmNlabelString, t); ac++;
  XtSetArg(args[ac], XmNtopAttachment, XmATTACH_FORM); ac++;
  XtSetArg(args[ac], XmNtopOffset, 10); ac++;
  XtSetArg(args[ac], XmNleftAttachment, XmATTACH_FORM); ac++;
  XtSetArg(args[ac], XmNleftOffset, 20); ac++;
  XtSetArg(args[ac], XmNrightAttachment, XmATTACH_FORM); ac++;
  XtSetArg(args[ac], XmNrightOffset, 20); ac++;
  XtSetArg(args[ac], XmNalignment, XmALIGNMENT_BEGINNING); ac++;
  top_label = XtCreateManagedWidget("top_label",xmLabelWidgetClass,
				    form, args, ac);
  ac = 0;
  XmStringFree(t);

  XtSetArg(args[ac], XmNtopAttachment, XmATTACH_WIDGET); ac++;
  XtSetArg(args[ac], XmNtopWidget, top_label); ac++;
  XtSetArg(args[ac], XmNleftAttachment, XmATTACH_FORM); ac++;
  XtSetArg(args[ac], XmNleftOffset, 20); ac++;
  XtSetArg(args[ac], XmNrightAttachment, XmATTACH_FORM); ac++;
  XtSetArg(args[ac], XmNrightOffset, 20); ac++;
  text_w = XtCreateManagedWidget("text_f_w", xmTextFieldWidgetClass,
				 form, args, ac);
  nw->name_widget = text_w;
  XtAddCallback(text_w, XmNactivateCallback, XmProcessTraversal,
		XmTRAVERSE_NEXT_TAB_GROUP);
  ac = 0;

  XtSetArg(args[ac], XmNtopAttachment, XmATTACH_WIDGET); ac++;
  XtSetArg(args[ac], XmNtopWidget, text_w); ac++;
  XtSetArg(args[ac], XmNtopOffset, 10); ac++;
  XtSetArg(args[ac], XmNleftAttachment, XmATTACH_FORM); ac++;
  XtSetArg(args[ac], XmNleftOffset, 2); ac++;
  XtSetArg(args[ac], XmNrightAttachment, XmATTACH_FORM); ac++;
  XtSetArg(args[ac], XmNrightOffset, 2); ac++;
  sep = XtCreateManagedWidget("sep",xmSeparatorWidgetClass,
			      form, args, ac);
  ac = 0;

  XtSetArg(args[ac], XmNtopAttachment, XmATTACH_WIDGET); ac++;
  XtSetArg(args[ac], XmNtopWidget, sep); ac++;
  XtSetArg(args[ac], XmNtopOffset, 10); ac++;
  XtSetArg(args[ac], XmNleftAttachment, XmATTACH_POSITION); ac++;
  XtSetArg(args[ac], XmNleftPosition, 40); ac++;
  XtSetArg(args[ac], XmNrightAttachment, XmATTACH_POSITION); ac++;    
  XtSetArg(args[ac], XmNrightPosition, 60); ac++;
  XtSetArg(args[ac], XmNbottomAttachment, XmATTACH_FORM); ac++;
  XtSetArg(args[ac], XmNbottomOffset, 10); ac++;
  pushb0 = XtCreateManagedWidget("Ok", xmPushButtonWidgetClass,
				 form, args, ac);

  XtAddCallback(pushb0, XmNactivateCallback, setup_port_name, nw);
  XtManageChild(form);
  
  XtAddGrab(d_shell, True, False);
  XtPopup(d_shell, XtGrabNone);  
}


/********************Dialog Box For EB and ANA component name*/
#if defined (__STDC__)
static void register_ebana_name (Widget w, 
				 XtPointer client_data, 
				 XmSelectionBoxCallbackStruct* cbs)
#else
static void register_ebana_name (w, client_data, cbs)
     Widget w;
     XtPointer client_data;
     XmSelectionBoxCallbackStruct *cbs;
#endif
{
  Arc      *arc = (Arc *)client_data;
  Arg      arg[5];
  int      ac = 0, type;
  ipPort   *from, *to;
  drawComp *draw_comp;
  daqComp  *comp;
  char     *name = 0;
  char     msg[80];

  XmStringGetLtoR (cbs->value, XmSTRING_DEFAULT_CHARSET, &name);
  if (!name || !*name){
    pop_error_message ("Nothing typed", w);
    return;
  }
  XtSetArg (arg[ac], XmNuserData, &type); ac++;
  XtGetValues (w, arg, ac);
  ac = 0;
  if (type == 0){ /* eb name */
    from = arc->from;
    comp = from->comp_ptr;
    comp->comp_name = strsave (name);
    /* draw component can be retrieved */
    draw_comp = findDrawCompFromPort (&coda_graph, from);
    if (!compNameOk (draw_comp, name)){
      sprintf(msg,"Duplicated component name \"%s\"",name);
      free (name);
      free (comp->comp_name);
      comp->comp_name = (char *)0;
      pop_error_message (msg, w);
      return;
    }
    (*draw_comp->write_name)(draw_comp, xgc.dpy, 
			     XtWindow(sw_geometry.draw_area));
    /* remove grab */
    XtRemoveGrab (XtParent(w));
    free (name);
    XtDestroyWidget (XtParent(w));
    popup_ebanaName_entry (arc->to->comp_ptr, arc->from->comp_ptr, arc, 1);
  }
  else{
    to = arc->to;
    comp = to->comp_ptr;
    comp->comp_name = strsave (name);
    /* find draw component from this port */
    draw_comp = findDrawCompFromPort (&coda_graph, to);
    if (!compNameOk (draw_comp, name)){
      sprintf(msg,"Duplicated component name \"%s\"",name);
      free (name);
      free (comp->comp_name);
      comp->comp_name = (char *)0;
      pop_error_message (msg, w);
      return;
    }
    (*draw_comp->write_name)(draw_comp, xgc.dpy, 
			     XtWindow(sw_geometry.draw_area));
    /* remove grab */
    XtRemoveGrab (XtParent(w));
    free (name);
    XtDestroyWidget (XtParent(w));
  }
}

#if defined (__STDC__)    
void popup_ebanaName_entry (daqComp* ana, 
			    daqComp* eb, 
			    Arc* new_arc, 
			    int type)
#else
void popup_ebanaName_entry (ana, eb, new_arc, type)
     daqComp *ana, *eb;
     Arc      *new_arc;
     int      type;
#endif
{
  Widget   dialog, shell;
  Position ret_x, ret_y;
  Arg      args[20];
  int      ac = 0;
  XmString t;

  if (type == 0){ /* first dialog */
    XtTranslateCoords(sw_geometry.draw_area, new_arc->edge_point[0].x,
		      new_arc->edge_point[0].y, &ret_x, &ret_y);
    t = XmStringCreateSimple ("EB Component Name");
  }
  else{
    XtTranslateCoords(sw_geometry.draw_area, new_arc->edge_point[3].x,
		      new_arc->edge_point[3].y, &ret_x, &ret_y);
    t = XmStringCreateSimple ("ANA Component Name");
  }
  XtSetArg(args[ac], XmNx, 100); ac++;
  XtSetArg(args[ac], XmNy, 200); ac++;
  if (type == 0){
    XtSetArg(args[ac], XmNtitle, "EB Name Popup"); ac++;
  }
  else{
    XtSetArg(args[ac], XmNtitle, "ANA Name Popup"); ac++;
  }
  XtSetArg(args[ac], XmNdefaultPosition, False); ac++;
  XtSetArg(args[ac], XmNautoUnmanage, False); ac++;
  XtSetArg(args[ac], XmNuserData, type); ac++;
  XtSetArg(args[ac], XmNselectionLabelString, t); ac++;
  dialog = XmCreatePromptDialog(sw_geometry.draw_area, "name_entry", 
				args, ac);
  ac = 0;
  XmStringFree (t);
  shell = XtParent(dialog);  

  XtSetSensitive (XmSelectionBoxGetChild (dialog, XmDIALOG_HELP_BUTTON),
		  False);
  XtSetSensitive (XmSelectionBoxGetChild (dialog, XmDIALOG_CANCEL_BUTTON),
		  False);
  XtAddCallback (dialog, XmNokCallback, register_ebana_name, 
		 (XtPointer)new_arc);
  XtManageChild (dialog);
  XtAddGrab(shell, True, False);
  XtPopup(shell, XtGrabNone);
}

/********************Error and warning popups*****************/
#if defined (__STDC__)
void pop_error_message(char* error, Widget base)
#else
void pop_error_message(error, base)
     char *error;
     Widget base;
#endif
{
  Widget dialog;
  XmString t = XmStringCreateSimple(error);
  Arg    args[5];
  int    ac = 0;
  
  XtSetArg(args[ac], XmNmessageString, t); ac++;
  XtSetArg(args[ac], XmNtitle,"CODA Error"); ac++;
  dialog = (Widget)XmCreateErrorDialog(base,"error_pop", args, ac);
  ac = 0;
  XtSetSensitive((Widget)XmMessageBoxGetChild(dialog,XmDIALOG_HELP_BUTTON),
		 FALSE);
  XtUnmanageChild((Widget)XmMessageBoxGetChild(dialog, 
					       XmDIALOG_CANCEL_BUTTON));
  XtManageChild(dialog);
  XmStringFree(t);
  XtPopup(XtParent(dialog),XtGrabNone);
}

