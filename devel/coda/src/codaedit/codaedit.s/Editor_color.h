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
 *	CODA Editor Color Utilities
 *	
 * Author:  Jie Chen
 * CEBAF Data Acquisition Group
 *
 * Revision History:
 *   $Log: Editor_color.h,v $
 *   Revision 1.1.1.1  1996/08/21 19:36:04  heyes
 *   Imported sources
 *
 *	  
 */
#ifndef _EDITOR_COLOR_H
#define _EDITOR_COLOR_H

#if defined (__STDC__)
extern void  RGB_TO_HSV(int r,int g,int b,
			float* h, float* s,float* v);
extern void  HSV_TO_RGB(int* r,int* g,int* b,
			float h, float s, float v);
extern void  XcodaGetShadowColor(Display* dpy,
				 Drawable win,
				 Pixel bg,
				 Pixel* b_shadow,
				 Pixel* t_shadow);
extern int   XcodaAllocNearestColor(Display* dpy, 
				    Colormap cmap, 
				    XColor* color);
extern Pixel get_pixel (Display* dpy, 
			Colormap cmap, 
			unsigned short r, 
			unsigned short g, 
			unsigned short b);
extern int   XcodaEditorInitColor (Display* dpy, 
				   Colormap cmap, 
				   Pixel* fg_pixel, 
				   Pixel* bg_pixel);
#else
extern void  RGB_TO_HSV ();
extern void  HSV_TO_RGB ();
extern void  XcodaGetShadowColor();
extern int   XcodaAllocNearestColor();
extern Pixel get_pixel ();
extern int   XcodaEditorInitColor ();
#endif

#endif
