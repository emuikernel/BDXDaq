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
 *	CODA Net Editor New Configuration Dialog 
 *	
 * Author:  Jie Chen
 * CEBAF Data Acquisition Group
 *
 * Revision History:
 *   $Log: Editor_newConfig.h,v $
 *   Revision 1.1.1.1  1996/08/21 19:36:07  heyes
 *   Imported sources
 *
 *	  
 */
#ifndef _EDITOR_NEW_CONFIG_H
#define _EDITOR_NEW_CONFIG_H

#include <stdio.h>
#include <string.h>
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>

/************************************************************************
 *          void newConfigDialogPopup (void)                            *
 * Description:                                                         *
 *     Popup the new configuration dialog box                           *
 ***********************************************************************/
#if defined (__STDC__)
extern void newConfigDialogPopup (Widget parent);
#else
extern void newConfigDialogPopup ();
#endif

#endif


