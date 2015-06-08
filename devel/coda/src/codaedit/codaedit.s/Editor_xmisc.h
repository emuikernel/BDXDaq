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
 *	CODA Editor X window Misc routines header file
 *	
 * Author:  Jie Chen
 * CEBAF Data Acquisition Group
 *
 * Revision History:
 *   $Log: Editor_xmisc.h,v $
 *   Revision 1.1.1.2  1996/11/05 17:45:21  chen
 *   coda source
 *
 *	  
 */
#ifndef _EDITOR_XMISC_H
#define _EDITOR_XMISC_H

typedef struct _attr_widget{
  Widget    name_widget;
  Widget    host_widget;
  Widget    id_widget;
  Widget    boot_widget;
  Widget    code_widget[3];
  Widget    ok_widget;
  Widget    cancel_widget;
  Widget    script_widget;
  drawComp  *comp;
}AttrWidgets;

#if defined (__STDC__)
extern void popup_resize_selection(XcodaEditorGraph* graph, 
				   drawComp* comp, 
				   Widget draw_area);
extern void popup_ipport_attributes (ipPort* port, 
				     Widget w, 
				     XEvent* event);
extern void popup_comp_attributes (drawComp* comp, 
				   Widget base, 
				   XEvent* event,
				   int type);
extern void popup_ipport_name(ipPort* from_port, 
			      ipPort* to_port, 
			      Widget base);
extern void popup_ebanaName_entry (daqComp* ana, 
				   daqComp* eb, 
				   Arc* new_arc, 
				   int type);
extern void pop_error_message (char* error, Widget base);
#else
extern void popup_resize_selection ();
extern void popup_ipport_attributes ();
extern void popup_comp_attributes ();
extern void popup_ipport_name ();
extern void popup_ebanaName_entry ();
extern void pop_error_message ();
#endif

#endif
