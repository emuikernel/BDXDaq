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
 *     XPM utility header file
 *	
 * Author:  Jie Chen
 * CEBAF Data Acquisition Group
 *
 * Revision History:
 *   $Log: XcodaXpm.h,v $
 *   Revision 1.1.1.1  1996/08/21 19:36:03  heyes
 *   Imported sources
 *
 *	  
 */
#ifndef _XCODA_XPM_H
#define _XCODA_XPM_H

#if defined (__STDC__)
extern Pixmap XcodaCreatePixmapFromXpm (Widget parent,
				        char** data,
				        int    type);
#else
extern Pixmap XcodaCreatePixmapFromXpm ();
#endif

#endif
