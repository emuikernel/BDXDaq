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
//	XcodaColor Implementation
//	
// Author:  Jie Chen
//       CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: XcodaColor.cc,v $
//   Revision 1.2  1998/04/08 17:28:51  heyes
//   get in there
//
//   Revision 1.1.1.1  1996/10/10 19:24:55  chen
//   coda motif C++ library
//
//
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include "XcodaColor.h"

XcodaColor::XcodaColor (void)
{
#ifdef _TRACE_OBJECTS
  printf ("Create XcodaColor Class Object\n");
#endif
  // empty
}

XcodaColor::~XcodaColor (void)
{
#ifdef _TRACE_OBJECTS
  printf ("Delete XcodaColor Class Object\n");
#endif
  // empty
}

Pixel
XcodaColor::getPixel (char *colorName)
{
  assert (theApplication);

  Display *dpy = theApplication->display ();
  Screen  *scr = XDefaultScreenOfDisplay (dpy);
  Colormap cmap = DefaultColormapOfScreen (scr);
  
  XColor exacRet, defRet;
  int status = XLookupColor (dpy, cmap, colorName, &exacRet, &defRet);
  if (status != 0){ // success
    return getPixel (defRet.red, defRet.green, defRet.blue);
  }
  else
    return 0;
}

Pixel
XcodaColor::getPixel (unsigned short red,
		      unsigned short green,
		      unsigned short blue)
{
  assert (theApplication);

  Display *dpy = theApplication->display ();
  Screen  *scr = XDefaultScreenOfDisplay (dpy);
  Colormap cmap = DefaultColormapOfScreen (scr);

  XColor color;
  color.red = red;
  color.green = green;
  color.blue = blue;
  color.flags = DoRed | DoGreen | DoBlue;

  if(XAllocColor(dpy, cmap, &color))
    return (color.pixel);
  else{
    int status = XcodaColor::allocNearestColor(dpy, cmap, &color);
    if (status){
      return (color.pixel);
    }
    else
      return 0;
  }
}

int
XcodaColor::allocNearestColor (Display* dpy, 
			       Colormap cmap,
			       XColor*  color)
{
  int            i, dc;
  XColor         *ctab;
  int            rd, gd, bd;
  unsigned short ri, gi, bi;
  int            mdist, close, d, status;

  dc = XCellsOfScreen (XDefaultScreenOfDisplay (dpy));
  ctab = (XColor *)malloc (dc*sizeof(XColor));
  if (ctab == NULL){
    fprintf(stderr,"Cannot allocate memory for XColor object\n");
    exit (1);
  }
  for (i = 0; i < dc; i++)
    ctab[i].pixel = (unsigned long)i;

  XQueryColors (dpy, cmap, ctab, dc);

  mdist = 3*dc*dc + 10;
  close = -1;
  ri = (color->red) >> 8;
  gi = (color->green) >> 8;
  bi = (color->blue) >> 8;
  
  for (i = 0; i < dc; i++){
    rd = ri - (ctab[i].red >> 8);
    gd = gi - (ctab[i].green >> 8);
    bd = bi - (ctab[i].blue >> 8);
    
    d = rd*rd + gd*gd + bd*bd;
    if (d < mdist){
      mdist = d;
      close = i;
    }
  }
  if (close < 0){ /* this cannot happen!!! */
    fprintf(stderr,"Bummer!!!!, Fatal Error!!!!\n");
    exit (1);
  }
  if (XAllocColor(dpy, cmap, &ctab[close])){
    color->red = ctab[close].red;
    color->green = ctab[close].green;
    color->blue = ctab[close].blue;
    color->pixel = ctab[close].pixel;
    status = 1;
  }
  else
    status = 0;

  free (ctab);
  return status;
}
  
  
