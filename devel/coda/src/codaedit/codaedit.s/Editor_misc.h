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
 *	CODA Editor Misc Routines
 *	
 * Author:  Jie Chen
 * CEBAF Data Acquisition Group
 *
 * Revision History:
 *   $Log: Editor_misc.h,v $
 *   Revision 1.1.1.2  1996/11/05 17:45:24  chen
 *   coda source
 *
 *	  
 */
#ifndef _EDITOR_MISC_H
#define _EDITOR_MISC_H
#include "Editor.h"

#if defined (__STDC__)
extern char*       strsave (char* s);
extern int         isEmptyLine (char* s);
extern codaScript* newCodaScriptList (void);
extern codaScript* newCodaScript (char* state, char* script);
extern void        addCodaScriptToList (codaScript* list, codaScript* item);
extern int         addScriptToList (codaScript* list, char* state, 
				    char* script);
extern codaScript* duplicateCodaScriptList (codaScript* list);
extern void        freeCodaScript  (codaScript* item);
extern void        freeCodaScriptList (codaScript* list);
#else
extern char*       strsave ();
extern int         isEmptyLine ();
extern codaScript* newCodaScriptList ();
extern codaScript* newCodaScript ();
extern void        addCodaScriptToList ();
extern int         addScriptToList ();
extern codaScript* duplicateCodaScriptList ();
extern void        freeCodaScript  ();
extern void        freeCodaScriptList ();
#endif

#endif
