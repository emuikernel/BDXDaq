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
 *	Coda Editor Database File name selection dialog
 *	
 * Author:  Jie Chen
 * CEBAF Data Acquisition Group
 *
 * Revision History:
 *   $Log: Editor_configSel.h,v $
 *   Revision 1.1.1.1  1996/08/21 19:36:06  heyes
 *   Imported sources
 *
 *	  
 */
#ifndef _EDITOR_CONFIG_SEL_H
#define _EDITOR_CONFIG_SEL_H

#include <stdio.h>
#include <string.h>
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include "Editor_database.h"

typedef struct _editor_config_sel_
{
  char*   configs_[EDITOR_MAX_CONFIGS];
  int     numConfigs_;
  int     managed_;
  Widget  w_;
  Widget  option_;
  Widget  ok_;
  Widget  cancel_;
  Widget  pushb[EDITOR_MAX_CONFIGS];
}editorConfigSel;

/************************************************************************
 *          void initConfigSel (Widget parent)                          *
 * Description:                                                         *
 *     Initialize the editorConfigSel structure                         *
 ***********************************************************************/
#if defined (__STDC__)
extern void initConfigSel (Widget parent);
#else
extern void initConfigSel ();
#endif

/************************************************************************
 *          void configSelpopup (void)                                  *
 * Description:                                                         *
 *     Popup database selection dialog                                  *
 ***********************************************************************/
#if defined (__STDC__)
extern void configSelPopup (void);
#else
extern void configSelPopup ();
#endif

/************************************************************************
 *          void configSelpopdown (void)                                *
 * Description:                                                         *
 *     Popdown database selection dialog                                *
 ***********************************************************************/
#if defined (__STDC__)
extern void configSelPopdown (void);
#else
extern void configSelPopdown ();
#endif

#endif

  
