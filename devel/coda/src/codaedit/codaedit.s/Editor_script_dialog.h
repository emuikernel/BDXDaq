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
 *	CodaEditor Script Input Dialog
 *	
 * Author:  Jie Chen
 * CEBAF Data Acquisition Group
 *
 * Revision History:
 *   $Log: Editor_script_dialog.h,v $
 *   Revision 1.1.1.1  1996/11/05 17:45:32  chen
 *   coda source
 *
 *	  
 */
#ifndef _EDITOR_SCRIPT_DIALOG_H
#define _EDITOR_SCRIPT_DIALOG_H

#include <stdio.h>
#include <string.h>
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include "Editor_graph.h"
#include "Editor_xmisc.h"


#if defined (__STDC__)
/* popup a script dialog box, with component comp, parent widget */
/* and a cancel button on the parent dialog box which cannot be  */
/* canceled after a user clicks on ok button of the script dialog*/
extern void popup_script_dialog (drawComp* comp,
				 Widget    parent,
				 AttrWidgets* atw);
#else
extern void popup_script_dialog ();
#endif

#endif

  
