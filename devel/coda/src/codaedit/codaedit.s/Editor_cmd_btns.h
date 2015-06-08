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
 *	All CODA Editor Command Buttons 
 *	
 * Author:  Jie Chen
 * CEBAF Data Acquisition Group
 *
 * Revision History:
 *   $Log: Editor_cmd_btns.h,v $
 *   Revision 1.1.1.2  1996/11/05 17:45:25  chen
 *   coda source
 *
 *	  
 */
#ifndef _EDITOR_CMD_BUTTONS_H
#define _EDITOR_CMD_BUTTONS_H

#if defined (__STDC__)
extern Widget XcodaEditorNewButtons(Widget parent);
extern void   XcodaEditorAddNewType (char* newtype);
extern void   XcodaEditorResetType  (void);
#else
extern Widget XcodaEditorNewButtons();
extern void   XcodaEditorAddNewType();
extern void   XcodaEditorResetType ();
#endif

#endif
