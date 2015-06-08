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
 *	Coda Editor Database Remove File name selection dialog
 *	
 * Author:  Jie Chen
 * CEBAF Data Acquisition Group
 *
 * Revision History:
 *   $Log: Editor_rmdbaseSel.h,v $
 *   Revision 1.1.1.1  1996/08/21 19:36:07  heyes
 *   Imported sources
 *
 *	  
 */
#ifndef _EDITOR_RMDBASE_SEL_H
#define _EDITOR_RMDBASE_SEL_H

#include <stdio.h>
#include <string.h>
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include "Editor_database.h"

typedef struct _editor_rmdbase_sel_
{
  char*   dbasenames_[EDITOR_MAX_DATABASES];
  int     numDbases_;
  int     managed_;
  Widget  w_;
  Widget  option_;
  Widget  ok_;
  Widget  cancel_;
  Widget  pushb[EDITOR_MAX_DATABASES];
}editorRmDbaseSel;

/************************************************************************
 *          void initRemovemDbaseSel (Widget parent)                    *
 * Description:                                                         *
 *     Initialize the editorRmDbaseSel structure                        *
 ***********************************************************************/
extern void initRemoveDbaseSel (Widget parent);

/************************************************************************
 *          void removeDbaseSelpopup (void)                             *
 * Description:                                                         *
 *     Popup database selection dialog                                  *
 ***********************************************************************/
extern void removeDbaseSelPopup (void);

/************************************************************************
 *          void removeDbaseSelpopdown (void)                           *
 * Description:                                                         *
 *     Popdown database selection dialog                                *
 ***********************************************************************/
extern void removeDbaseSelPopdown (void);

#endif

  
