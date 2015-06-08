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
 *	Widget Poll Header File
 *	
 * Author:  Jie Chen
 * CEBAF Data Acquisition Group
 *
 * Revision History:
 *   $Log: Editor_widget_poll.h,v $
 *   Revision 1.1.1.1  1996/08/21 19:36:06  heyes
 *   Imported sources
 *
 *	  
 */
#ifndef _EDITOR_WIDGET_POLL_H
#define _EDITOR_WIDGET_POLL_H

#if defined (__STDC__)
extern void init_widget_poll (void);
extern void change_color     (Pixel col, Screen* scr,int type);
extern void saveColor        (XColor* col, int type);
#else
extern void init_widget_poll ();
extern void change_color     ();
extern void saveColor        ();
#endif

#endif
