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
 *     CODA Editor manager routines
 *	
 * Author:  Chip Watson & Jie Chen, CEBAF Data Acquisition Group
 *
 * Revision History:
 *   $Log: Editor_manager.c,v $
 *   Revision 1.1.1.2  1996/11/05 17:45:13  chen
 *   coda source
 *
 *	  
 */
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <Xm/Xm.h>

#include "Editor_layout.h"

#if defined (__STDC__)
void XcodaEditorDisableInput (void)
#else
void XcodaEditorDisableInput ()
#endif
{
  XtSetSensitive (manager.newconfig, FALSE); 
  XtSetSensitive (manager.openconfig, FALSE); 
  XtSetSensitive (manager.removeconfig, FALSE); 
  XtSetSensitive (manager.saveconfig, FALSE); 
  XtSetSensitive (manager.saveasconfig, FALSE); 
  XtSetSensitive (manager.option, FALSE);   
}

#if defined (__STDC__)
void
XcodaEditorEnableInput (void)
#else
void
XcodaEditorEnableInput ()
#endif
{
  XtSetSensitive (manager.newconfig, TRUE); 
  XtSetSensitive (manager.openconfig, TRUE); 
  XtSetSensitive (manager.removeconfig, TRUE); 
  XtSetSensitive (manager.saveconfig, TRUE); 
  XtSetSensitive (manager.saveasconfig, TRUE); 
  XtSetSensitive (manager.option, TRUE); 
}

#if defined (__STDC__)
void
XcodaEditorDisableEditing (void)
#else
void
XcodaEditorDisableEditing ()
#endif
{
  XtSetSensitive (manager.delete_comp_menu, FALSE);
  XtSetSensitive (manager.delete_arc_menu, FALSE);
  XtSetSensitive (manager.delete_allarc_menu, FALSE);
  XtSetSensitive (manager.delete_all_menu, FALSE);
  XtSetSensitive (manager.undo_menu, FALSE);
  XtSetSensitive (manager.resize_comp_menu, FALSE);
  XtSetSensitive (manager.connect2cols_menu, FALSE);
  XtSetSensitive (manager.redraw_menu, FALSE);

  /* popup menu buttons */
  XtSetSensitive (manager.pop_delete_comp_menu, FALSE);
  XtSetSensitive (manager.pop_delete_arc_menu, FALSE);
  XtSetSensitive (manager.pop_delete_allarc_menu, FALSE);
  XtSetSensitive (manager.pop_delete_all_menu, FALSE);
  XtSetSensitive (manager.pop_undo_menu, FALSE);
  XtSetSensitive (manager.pop_resize_comp_menu, FALSE);
  XtSetSensitive (manager.pop_connect2cols_menu, FALSE);
  XtSetSensitive (manager.pop_redraw_menu, FALSE);
}

#if defined (__STDC__)
void
XcodaEditorEnableEditing (void)
#else
void
XcodaEditorEnableEditing ()
#endif
{
  XtSetSensitive (manager.delete_comp_menu, TRUE);
  XtSetSensitive (manager.delete_arc_menu, TRUE);
  XtSetSensitive (manager.delete_allarc_menu, TRUE);
  XtSetSensitive (manager.delete_all_menu, TRUE);
  XtSetSensitive (manager.undo_menu, TRUE);
  XtSetSensitive (manager.resize_comp_menu, TRUE);
  XtSetSensitive (manager.connect2cols_menu, TRUE);
  XtSetSensitive (manager.redraw_menu, TRUE);

  /* popup menu buttons */
  XtSetSensitive (manager.pop_delete_comp_menu, TRUE);
  XtSetSensitive (manager.pop_delete_arc_menu, TRUE);
  XtSetSensitive (manager.pop_delete_allarc_menu, TRUE);
  XtSetSensitive (manager.pop_delete_all_menu, TRUE);
  XtSetSensitive (manager.pop_undo_menu, TRUE);
  XtSetSensitive (manager.pop_resize_comp_menu, TRUE);
  XtSetSensitive (manager.pop_connect2cols_menu, TRUE);
  XtSetSensitive (manager.pop_redraw_menu, TRUE);
}
