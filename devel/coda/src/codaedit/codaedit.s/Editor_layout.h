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
 *	CODA Editor Xwindow layout header file
 *	
 * Author: Jie Chen, CEBAF Data Acquisition Group
 *
 * Revision History:
 *   $Log: Editor_layout.h,v $
 *   Revision 1.3  1997/09/08 15:19:35  heyes
 *   fix dd icon etc
 *
 *   Revision 1.2  1997/06/20 17:00:15  heyes
 *   clean up GUI!
 *
 *   Revision 1.1.1.2  1996/11/05 17:45:13  chen
 *   coda source
 *
 *	  
 */
#ifndef EDITOR_LAYOUT_H
#define EDITOR_LAYOUT_H

typedef struct _manager{
  /* add node cmd */
  Widget trig_btn;
  Widget roc_btn;
  Widget eb_btn;
  Widget er_btn;
  Widget fi_btn;
  Widget cfi_btn;
  Widget dbg_btn;
  Widget dd_btn;
  Widget none_btn;
  Widget typemenu_btn;
  /* graph cmd */
  Widget eth_btn;
  Widget mvnode_btn;
  /* edit options */
  Widget delete_comp_menu;
  Widget delete_arc_menu;
  Widget delete_allarc_menu;
  Widget delete_all_menu;
  Widget undo_menu;
  Widget resize_comp_menu;
  Widget connect2cols_menu;
  Widget redraw_menu;
  /* popup edit menu */
  Widget pop_delete_comp_menu;
  Widget pop_delete_arc_menu;
  Widget pop_delete_allarc_menu;
  Widget pop_delete_all_menu;
  Widget pop_undo_menu;
  Widget pop_resize_comp_menu;
  Widget pop_connect2cols_menu;
  Widget pop_redraw_menu;
  /* options */
  Widget rmvcomp_menu;
  Widget color_menu;
  /* icon holder */
  Widget icon_holder;
  /* file menus  */
  Widget newdbase;
  Widget opendbase;
  Widget removedbase;
  Widget exit_menu;
  /* config menus */
  Widget newconfig;
  Widget openconfig;
  Widget removeconfig;
  Widget saveconfig;
  Widget saveasconfig;
  Widget option;
  /* experiment run type nad dbase name */
  Widget dbasename;
  Widget runtype;
}Manager;

extern Manager manager;

#if defined (__STDC__)
extern void XcodaEditorDisableInput   (void);
extern void XcodaEditorEnableInput    (void);
extern void XcodaEditorDisableEditing (void);
extern void XcodaEditorEnableEditing  (void);
#else
extern void XcodaEditorDisableInput   ();
extern void XcodaEditorEnableInput    ();
extern void XcodaEditorDisableEditing ();
extern void XcodaEditorEnableEditing  ();
#endif

#endif
  
