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
 *	NetEditor Open New Database Dialog
 *	
 * Author:  Jie Chen
 * CEBAF Data Acquisition Group
 *
 * Revision History:
 *   $Log: Editor_opendbaseDialog.h,v $
 *   Revision 1.1.1.1  1996/08/21 19:36:07  heyes
 *   Imported sources
 *
 *	  
 */
#ifndef _EDITOR_OPENDBASE_DIALOG_H
#define _EDITOR_OPENDBASE_DIALOG_H

#include <stdio.h>
#include <string.h>
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>

/**************************************************************************
 *      void opendbaseDialogPopup (Widget parent, int type)               *
 * Description:                                                           *
 *     Popup a warning dialog for opening a new database                  *
 *     type = 0: new database, type = 1: select database                  *
 *************************************************************************/
#if defined (__STDC__)
extern void opendbaseDialogPopup (Widget parent, int type);
#else
extern void opendbaseDialogPopup ();
#endif

#endif
