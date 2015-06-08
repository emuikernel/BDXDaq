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
 *	Coda Editor Color selection header files
 *	
 * Author:  Jie Chen
 * CEBAF Data Acquisition Group
 *
 * Revision History:
 *   $Log: Editor_color_sel.h,v $
 *   Revision 1.1.1.1  1996/08/21 19:36:04  heyes
 *   Imported sources
 *
 *	  
 */
#ifndef _EDITOR_COLOR_SEL_H
#define _EDITOR_COLOR_SEL_H

#if defined (__STDC__)
extern void popup_color_selector(Widget w, 
				 XtPointer client_data, 
				 XmAnyCallbackStruct* cbs);
#else
extern void popup_color_selector();
#endif

#endif
