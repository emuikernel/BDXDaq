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
 *	Coda Editor graphical command routines
 *	
 * Author:  Jie Chen
 * CEBAF Data Acquisition Group
 *
 * Revision History:
 *   $Log: Editor_graph_cmd.h,v $
 *   Revision 1.1.1.1  1996/08/21 19:36:06  heyes
 *   Imported sources
 *
 *	  
 */
#ifndef _EDITOR_GRAPH_CMD_H
#define _EDITOR_GRAPH_CMD_H

#if defined (__STDC__)
extern Widget XcodaEditorDataLinkButtons(Widget parent);
extern void   XcodaEditorResetGraphCmd  (void);
extern void   XcodaEditorExecGraphCmd   (Widget w, 
					 XtPointer client_data, 
					 XmAnyCallbackStruct* cbs);
#else
extern Widget XcodaEditorDataLinkButtons();
extern void   XcodaEditorResetGraphCmd ();
extern void   XcodaEditorExecGraphCmd();
#endif

#endif
