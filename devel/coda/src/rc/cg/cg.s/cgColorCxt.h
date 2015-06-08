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
//      X window color context
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: cgColorCxt.h,v $
//   Revision 1.1.1.1  1996/10/10 19:38:06  chen
//   CODA OO-graphics library for Xt
//
//
#ifndef _CG_COLOR_CXT_H
#define _CG_COLOR_CXT_H

#include <stdio.h>
#include <string.h>
#include <X11/Xlib.h>
#include <X11/Xlibint.h>
#include <X11/Xatom.h>
#include <math.h>

#include <cgDispDev.h>

class cgColorCxt
{
public:
  // constructor and destructor
  cgColorCxt (cgDispDev& dispDev);
  virtual    ~cgColorCxt (void);

  // pixel value by RGB
  virtual Pixel pixel  (unsigned short r,
			unsigned short g,
			unsigned short b);
  // pixel value by name
  virtual Pixel pixel  (char* name);
  
  // lite shade of a pixel color
  virtual Pixel liteShadePixel       (Pixel color);
  // dark shade of a pixel color
  virtual Pixel darkShadePixel       (Pixel color);

protected:
  static void RGB_TO_HSV (unsigned short r,
			  unsigned short g,
			  unsigned short b,
			  float* h,
			  float* s,
			  float* v);
  static void HSV_TO_RGB (unsigned short* r,
			  unsigned short* g,
			  unsigned short* b,
			  float h,
			  float s,
			  float v);

private:
  cgDispDev&         disp_;
  Colormap           colormap_;
  Visual             *visual_;
  XVisualInfo*       visualInfo_;
  int                numColors_;
};
#endif
  
  
