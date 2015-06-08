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
 *	Coda Editor Syntax checker header file
 *	
 * Author:  Jie Chen
 * CEBAF Data Acquisition Group
 *
 * Revision History:
 *   $Log: Editor_syntax_checker.h,v $
 *   Revision 1.1.1.1  1996/08/21 19:36:05  heyes
 *   Imported sources
 *
 *	  
 */
#ifndef _EDITOR_SYNTAX_CHECKER_H
#define _EDITOR_SYNTAX_CHECKER_H

#if defined (__STDC__)
extern int compNameOk (drawComp* comp, char* name);
extern int compHostOk (drawComp* comp, char* host);
extern int compIdOk   (drawComp* comp, int id);
#else
extern int compNameOk ();
extern int compHostOk ();
extern int compIdOk   ();
#endif

#endif
