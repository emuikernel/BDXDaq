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
 *	CODA Editor Icon box routines
 *	
 * Author:  Jie Chen
 * CEBAF Data Acquisition Group
 *
 * Revision History:
 *   $Log: Editor_icon_box.h,v $
 *   Revision 1.1.1.1  1996/08/21 19:36:06  heyes
 *   Imported sources
 *
 *	  
 */
#ifndef _EDITOR_ICON_BOX_H
#define _EDITOR_ICON_BOX_H

#if defined (__STDC__)
extern Widget XcodaEditorIconBox (Widget parent);
extern void   XcodaEditorCreateIconsFromRcnet(rcNetComp** list, 
					      int num_comps);
extern void   XcodaEditorInsertCompToIcons(XcodaEditorGraph* graph);
extern void   delete_everyicons(void);
extern int    isEmptyIconList(void);
extern int    nameOkWithIconList (char* name);
extern int    idOkWithIconList (char* name, int id, int type);
extern void   updateInfoToIconList(drawComp* comp);
extern void   compressIconList(void);
#else
extern Widget XcodaEditorIconBox ();
extern void   XcodaEditorCreateIconsFromRcnet ();
extern void   XcodaEditorInsertCompToIcons();
extern void   delete_everyicons();
extern int    isEmptyIconList();
extern int    nameOkWithIconList ();
extern int    idOkWithIconList ();
extern void   updateInfoToIconList();
extern void   compressIconList();
#endif

#endif
