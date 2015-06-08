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
//      Generic X window drawing Functions
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: cgXdrawing.cc,v $
//   Revision 1.1.1.1  1996/10/10 19:38:06  chen
//   CODA OO-graphics library for Xt
//
//
#include <cgDispDev.h>
#include <cgContext.h>

#include "cgXdrawing.h"

void draw_line (cgContext* cxt, 
		int x1,
		int y1,
		int x2,
		int y2)
{
  if (cxt->xdrawable () != 0) {
    XDrawLine (cxt->xdisplay(), cxt->xdrawable(), cxt->xgc(),
	       x1, y1, x2, y2);
  }
}

void draw_point (cgContext* cxt,
		 int x, int y)
{
  if (cxt->xdrawable () != 0) {
    XDrawPoint (cxt->xdisplay(), cxt->xdrawable(), cxt->xgc(),
		x, y);
  }
}

int cg_get_text_height (cgContext* cxt)
{
  XFontStruct* font = cxt->getFont ();
  if (font) {
   int ht = font->max_bounds.ascent + font->max_bounds.descent;
   // free font structure without unloading the font
   XFreeFontInfo (0, font, 1);
   return ht;
 }
  else
    return 0;
}

int cg_get_text_width (cgContext* cxt, char* str)
{
  XFontStruct *font = cxt->getFont ();
  if (font) {
    int wd = XTextWidth (font, str, strlen (str));
    // free font structure without unloading the font
    XFreeFontInfo (0, font, 1);
    return wd;
  }
  else
    return 0;
}

void cg_draw_string (cgContext* cxt, char* str, int x, int y)
{
  if (cxt->xdrawable () != 0) {
    XFontStruct *font = cxt->getFont ();
    if (font) {
      // find real origin of the character string
      // lbearing < 0
      int xorig = x - font->min_bounds.lbearing;
      int yorig = y - font->max_bounds.descent;
      XDrawString (cxt->xdisplay(), cxt->xdrawable (), cxt->xgc (),
		 xorig, yorig, str, strlen (str));
      // free font structure without unloading the font
      XFreeFontInfo (0, font, 1);
    }
  }
}

void cg_draw_pixmap (cgContext* cxt, Pixmap pix, 
		     Dimension width, Dimension height,
		     int x, int y)
{
  // only works for color monitor:
  // it is safe since this routine will be called after xpm image
  if (cxt->xdrawable () != 0) 
    XCopyArea (cxt->xdisplay (), pix, cxt->xdrawable (),
	       cxt->xgc (), 0, 0, width, height, 
	       x, y - height);
}



