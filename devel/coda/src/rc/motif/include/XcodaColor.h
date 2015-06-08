//-----------------------------------------------------------------------------
// Copyright (c) 1991,1992 Southeastern Universities Research Association,
//                         Continuous Electron Beam Accelerator Facility
//
// This software was developed under a United States Government license
// described in the NOTICE file included as part of this distribution.
//
// CEBAF Data Acquisition Group, 12000 Jefferson Ave., Newport News, VA 23606
// Email: coda@cebaf.gov  Tel: (804) 249-7101  Fax: (804) 249-7363
//-----------------------------------------------------------------------------
// 
// Description:
//	 Default Color Manipulation Class
//       Return Pixel Value from RGB or Char String Using DefaultColorMap
//	
// Author:  Jie Chen
//       CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: XcodaColor.h,v $
//   Revision 1.2  1998/04/08 17:28:51  heyes
//   get in there
//
//   Revision 1.1.1.1  1996/10/10 19:25:00  chen
//   coda motif C++ library
//
//
#ifndef _XCODA_COLOR_H
#define _XCODA_COLOR_H

#include <XcodaApp.h>

class XcodaColor
{
public:
  // constructors and destructor
  XcodaColor  (void);
  ~XcodaColor (void);

  // operations
  static Pixel getPixel (unsigned short red,
			 unsigned short green,
			 unsigned short blue);
  static Pixel getPixel (char *colorName);

protected:
  // return 0 failure
  static int   allocNearestColor (Display *dpy, 
				  Colormap cmap, 
				  XColor* color);
};
#endif

  
