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
 *	Menu header file
 *	
 * Author:  Jie Chen
 * CEBAF Data Acquisition Group
 *
 * Revision History:
 *   $Log: Editor_menu.h,v $
 *   Revision 1.2  1997/06/16 12:23:58  heyes
 *   various fixes and nicities!
 *
 *   Revision 1.1.1.2  1996/11/05 17:45:23  chen
 *   coda source
 *
 *	  
 */
#ifndef _EDITOR_MENU_H
#define _EDITOR_MENU_H

#if defined (__STDC__)
extern void  XcodaEditorCreateMenus    (Widget menu_bar, int withExit);
extern void  XcodaEditorDeleteAllArcs  (Widget w, 
					XtPointer client_data, 
					XmAnyCallbackStruct* cbs);
extern void  XcodaEditorDeleteAll      (Widget w, 
					XtPointer client_data, 
					XmAnyCallbackStruct* cbs);
extern void  XcodaEditorNewConfigCbk   (Widget w, 
					XtPointer clientData, 
					XmAnyCallbackStruct* cbs);
extern void  XcodaEditorOpenConfigCbk  (Widget w, 
					XtPointer clientData, 
					XmAnyCallbackStruct* cbs);
extern void  XcodaEditorRemoveConfigCbk(Widget w, 
					XtPointer clientData, 
					XmAnyCallbackStruct* cbs);
extern void  XcodaEditorSaveConfig     (Widget w, 
					XtPointer client_data, 
					XmAnyCallbackStruct* cbs);
extern void  XcodaEditorSaveDefault    (Widget w, 
					XtPointer client_data, 
					XmAnyCallbackStruct* cbs);
extern void  XcodaEditorResetOptionDialog (void);
extern void  XcodaEditorSaveConfigOption (char* runtype);
extern void  XcodaEditorConfigOption   (Widget w, XtPointer client_data,
					XmAnyCallbackStruct* cbs);
extern void  XcodaEditorNewDbaseEntry  (Widget w);
extern void  XcodaEditorNewDbaseCbk    (Widget w, 
					XtPointer client_data, 
					XmAnyCallbackStruct* cbs);
extern void  XcodaEditorOpenDbaseCbk   (Widget w, 
					XtPointer client_data, 
					XmAnyCallbackStruct* cbs);
extern void  XcodaEditorRemoveDbaseCbk (Widget w, 
					XtPointer client_data, 
					XmAnyCallbackStruct* cbs);
extern void  XcodaEditorCleanDbase     (Widget w, 
					XtPointer client_data, 
					XmAnyCallbackStruct* cbs);
extern void  XcodaEditorExitMenuCbk    (Widget w, 
					XtPointer client_data, 
					XmAnyCallbackStruct* cbs);
#else
extern void  XcodaEditorCreateMenus    ();
extern void  XcodaEditorDeleteAllArcs  ();
extern void  XcodaEditorDeleteAll      ();
extern void  XcodaEditorNewConfigCbk   ();
extern void  XcodaEditorOpenConfigCbk  ();
extern void  XcodaEditorRemoveConfigCbk();
extern void  XcodaEditorSaveConfig     ();
extern void  XcodaEditorSaveDefault    ();
extern void  XcodaEditorResetOptionDialog ();
extern void  XcodaEditorSaveConfigOption ();
extern void  XcodaEditorConfigOption   ();
extern void  XcodaEditorNewDbaseEntry  ();
extern void  XcodaEditorNewDbaseCbk    ();
extern void  XcodaEditorOpenDbaseCbk   ();
extern void  XcodaEditorRemoveDbaseCbk();
extern void  XcodaEditorCleanDbase     ();
extern void  XcodaEditorExitMenuCbk    ();
#endif

#endif
