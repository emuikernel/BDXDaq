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
//      X11 window Color Context
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: cgColorCxt.cc,v $
//   Revision 1.2  1998/04/08 18:31:00  heyes
//   new look and feel GUI
//
//   Revision 1.1.1.1  1996/10/10 19:38:05  chen
//   CODA OO-graphics library for Xt
//
//
#include "cgColorCxt.h"

const float MAX_UNSIGNED_SHORT = 65535.0;

cgColorCxt::cgColorCxt (cgDispDev& disp)
:disp_ (disp)
{
#ifdef _TRACE_OBJECTS
  printf ("Create cgColorCxt Class Object\n");
#endif
  XCC xcc = disp_.xcc_;
  colormap_ = XCCGetColormap (xcc);
  visual_ = XCCGetVisual (xcc);
  visualInfo_ = XCCGetVisualInfo (xcc);
  numColors_ = XCCGetNumColors (xcc);
}

cgColorCxt::~cgColorCxt (void)
{
#ifdef _TRACE_OBJECTS
  printf ("Delete cgColorCxt Class Object\n");
#endif
  // empty
}

Pixel
cgColorCxt::pixel (unsigned short r,
		   unsigned short g,
		   unsigned short b)
{
  XCC xcc = disp_.xcc_;
  Boolean failed;
  return XCCGetPixel (xcc, 
		      (unsigned int)r, 
		      (unsigned int)g,
		      (unsigned int)b,
		      &failed);
}

Pixel
cgColorCxt::pixel (char* name)
{
  XColor color;
  color.red = color.green = color.blue = 0;

  if (XParseColor (disp_.display_, colormap_, name, &color) != 0)
    return pixel (color.red, color.green, color.blue);
  else // bad color name
    return 0;
}

void
cgColorCxt::RGB_TO_HSV (unsigned short r,
			unsigned short g,
			unsigned short b,
			float* h,
			float* s,
			float* v)
{
  float max,min;
  float temp;
  float rr,gg,bb;
  float delta;

  rr = r/(MAX_UNSIGNED_SHORT);
  gg = g/(MAX_UNSIGNED_SHORT);
  bb = b/(MAX_UNSIGNED_SHORT);

  max = rr;
  if(gg >= max)
    max = gg;
  if(bb >= max)
    max = bb;

  min = rr;
  if(gg < min)
    min = gg;
  if(bb < min)
    min = bb;
  

  *v = max;
  if(max != 0)
    {
      *s = (max-min)/max;
    }
  else
    *s = 0.0;

  if(*s == 0.0)
    *h = 400.0;
  else
    {
      delta = max - min;
      if(rr == max)
	*h = (gg-bb)/delta;
      else if(gg == max)
	*h = 2 + (bb - rr)/delta;
      else
        *h = 4 + (rr - gg)/delta;

      *h = *h * 60.0;
      if(*h < 0)
        *h = *h + 360;
    }
}

void 
cgColorCxt::HSV_TO_RGB(unsigned short* r,
		       unsigned short* g,
		       unsigned short* b,
		       float h,
		       float s,
		       float v)
{
  float rr,gg,bb;
  int i;
  float f,p,q,t;

  if(s < 0.000001) {
    if(h > 360.0) {
      rr = v;
      gg = v;
      bb = v;
    }
  }
  else {
    if(h == 360.0)
      h = 0.0;
    h = h/60;
    i = (int)floor(h);
    f = h -i;
    p = v*(1-s);
    q = v*(1-(s*f));
    t = v*(1-(s*(1-f)));
    switch(i) {
    case 0:
      rr = v;
      gg = t;
      bb = p;
      break;
    case 1:
      rr = q;
      gg = v;
      bb = p;
      break;
    case 2:
      rr = p;
      gg = v;
      bb = t;
      break;
    case 3:
      rr = p;
      gg = q;
      bb = v;
      break;
    case 4:
      rr = t;
      gg = p;
      bb = v;
      break;
    case 5:
      rr = v;
      gg = p;
      bb = q;
    default:
      break;
    }
  }
  *r = (unsigned short)(rr*MAX_UNSIGNED_SHORT);
  *g = (unsigned short)(gg*MAX_UNSIGNED_SHORT);
  *b = (unsigned short)(bb*MAX_UNSIGNED_SHORT);
}

    
Pixel
cgColorCxt::liteShadePixel (Pixel cl)
{
  XColor color;

  color.pixel = cl;

  XCCQueryColor (disp_.xcc_, &color);
  
  float h, s, v;
  cgColorCxt::RGB_TO_HSV (color.red, color.green, color.blue,
			  &h, &s, &v);
  float ts, th, tv;
  if (s == 0.0) {
    ts = 0.0;
    th = 400.0; // undefined
    if (v == 0.0)
      tv = 0.3;
    else {
      if (v*1.33 <= 1.0)
	tv = v*1.33;
      else
	tv = 1.0;
    }
  }
  else {
    ts = s/2;
    th = h;
    tv = v;
  }

  unsigned short r, g, b;
  cgColorCxt::HSV_TO_RGB (&r, &g, &b, th, ts, tv);
  
  return pixel (r, g, b);
}

Pixel
cgColorCxt::darkShadePixel (Pixel cl)
{
  XColor color;

  color.pixel = cl;

  XCCQueryColor (disp_.xcc_, &color);
  
  float h, s, v;
  cgColorCxt::RGB_TO_HSV (color.red, color.green, color.blue,
			  &h, &s, &v);
  float ts, th, tv;

  if(s == 0.0) {
    ts = 0.0;
    th = 400.0; /*undefined*/
    tv = v/2;
  }
  else {
    ts = s;
    th = h;
    tv = v/2;
  }
  unsigned short r, g, b;
  cgColorCxt::HSV_TO_RGB(&r,&g,&b,th,ts,tv);

  return pixel (r, g, b);
}
