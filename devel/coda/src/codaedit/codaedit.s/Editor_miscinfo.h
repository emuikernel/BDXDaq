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
 *	CODA editor misc info widgets
 *	
 * Author:  Jie Chen
 * CEBAF Data Acquisition Group
 *
 * Revision History:
 *   $Log: Editor_miscinfo.h,v $
 *   Revision 1.1.1.1  1996/08/21 19:36:06  heyes
 *   Imported sources
 *
 *	  
 */
#ifndef _EDITOR_EXPINFO_H
#define _EDITOR_EXPINFO_H

#include <stdio.h>
#include <string.h>
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>

#if defined (__STDC__)
extern Widget XcodaEditorCreateInfoWidgets (Widget parent);
extern void   XcodaEditorShowDatabaseName  (char* name);
extern void   XcodaEditorShowConfigName    (char* config);
#else
extern Widget XcodaEditorCreateInfoWidgets ();
extern void   XcodaEditorShowDatabaseName  ();
extern void   XcodaEditorShowConfigName    ();
#endif

#endif
