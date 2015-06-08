//-----------------------------------------------------------------------------
// Copyright (c) 1994,1995 Southeastern Universities Research Association,
//                         Continuous Electron Beam Accelerator Facility
//
// This software was developed under a United States Government license
// described in the NOTICE file included as part of this distribution.
//
// CEBAF Data Acquisition Group, 12000 Jefferson Ave., Newport News, VA 23606
//       coda@cebaf.gov  Tel: (804) 249-7030     Fax: (804) 249-5800
//-----------------------------------------------------------------------------
//
// Description:
//      Generic X window drawing routines
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: cgXdrawing.h,v $
//   Revision 1.1.1.1  1996/10/10 19:38:06  chen
//   CODA OO-graphics library for Xt
//
//
#ifndef _CG_X_DRAWING_H
#define _CG_X_DRAWING_H

#include <cgContext.h>

extern void draw_line  (cgContext* cxt,
			int x1, int y1,
			int x2, int y2);
extern void draw_point (cgContext* cxt,
			int x, int y);
extern void cg_draw_string (cgContext* cxt,
			    char* str, int x, int y);
extern int  cg_get_text_height (cgContext* cxt);
extern int  cg_get_text_width  (cgContext* cxt, char* str);
extern void cg_draw_pixmap     (cgContext* cxt, Pixmap pix,
				Dimension width, Dimension height,
				int x, int y);

#endif
