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
//	 XPM Supporting routines
//	
// Author:  Jie Chen
//       CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: XcodaXpm.cc,v $
//   Revision 1.4  2000/01/28 16:42:28  rwm
//   include should be X11/xpm.h
//
//   Revision 1.3  1998/04/08 17:29:42  heyes
//   get in there
//
//   Revision 1.2  1997/07/30 14:12:16  heyes
//   add more xpm support
//
//   Revision 1.1.1.1  1996/10/10 19:24:56  chen
//   coda motif C++ library
//
//
#include <stdio.h>
#include <string.h>
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <Xm/Xm.h>
#include <X11/xpm.h>
#include "XcodaXpm.h"

XcodaXpm::XcodaXpm(Widget parent, char **pix)
{
#ifdef _TRACE_OBJECTS
  printf("    Create XcodaXpm Class Object \n");
#endif
  _pixmap = 0;
  _parent = parent;
  _display = XtDisplay (parent);
  _scr = XDefaultScreenOfDisplay (_display);
  _win = XDefaultRootWindow (_display);
  _cmap = DefaultColormapOfScreen (_scr);
  _depth = DefaultDepthOfScreen (_scr);  
  _pix = pix;
  _bgsymbol = 0;
}

XcodaXpm::XcodaXpm(Widget parent, char **pix, char *symbol)
{
#ifdef _TRACE_OBJECTS
  printf("    Create XcodaXpm Class Object \n");
#endif
  _pixmap = 0;
  _parent = parent;
  _display = XtDisplay (parent);
  _scr = XDefaultScreenOfDisplay (_display);
  _win = XDefaultRootWindow (_display);
  _cmap = DefaultColormapOfScreen (_scr);
  _depth = DefaultDepthOfScreen (_scr);  
  _pix = pix;
  _bgsymbol = new char[::strlen(symbol) + 1];
  ::strcpy (_bgsymbol, symbol);
}

XcodaXpm::~XcodaXpm()
{
#ifdef _TARCE_OBJECTS
  printf("     Destroy XcodaXpm Object \n");
#endif
  if (_bgsymbol)
    delete []_bgsymbol;

  if(_pixmap)
    XFreePixmap(_display, _pixmap);
  _pixmap = 0;
}

Pixmap XcodaXpm::createPixmapFromXpm()
{
  XpmAttributes  attr;
  int            err, numsymbols = 0;
  unsigned long  pixmap_ret, pixmap_mask;
  unsigned long  valuemask = 0;
  Arg            arg[10];
  int            ac = 0;
  XpmColorSymbol col_symbol[5];
  Pixel          bg;  

  XtSetArg (arg[ac], XmNbackground, &bg); ac++;
  XtGetValues (_parent, arg, ac);
  ac = 0;


  if(_bgsymbol){
    col_symbol[numsymbols].name = _bgsymbol;
    col_symbol[numsymbols].value = (char *)NULL;
    col_symbol[numsymbols++].pixel = bg;

    attr.colormap = _cmap;
    attr.depth = _depth;
    attr.colorsymbols = col_symbol;
    attr.valuemask = valuemask;
    attr.numsymbols = numsymbols;
    attr.closeness = 40000;

    attr.valuemask |= XpmReturnPixels;
    attr.valuemask |= XpmColormap;
    attr.valuemask |= XpmColorSymbols;
    attr.valuemask |= XpmDepth;
    attr.valuemask |= XpmCloseness;
  }
 else{
    col_symbol[numsymbols].name = NULL;
    col_symbol[numsymbols].value = "none";
    col_symbol[numsymbols++].pixel = bg;

    attr.colormap = _cmap;
    attr.depth = _depth;
    attr.colorsymbols = col_symbol;
    attr.numsymbols = numsymbols;
    attr.closeness = 40000;
    attr.valuemask = valuemask;


    attr.valuemask |= XpmReturnPixels;
    attr.valuemask |= XpmColormap;
    attr.valuemask |= XpmColorSymbols;
    attr.valuemask |= XpmDepth;
    attr.valuemask |= XpmCloseness;
  }
  err = XpmCreatePixmapFromData (_display, _win, _pix, &pixmap_ret,
				 &pixmap_mask,
				 &attr);
  if (err != XpmSuccess){
    pixmap_ret = 0;
    _pixmap = 0;
  }
  else{
    _pixmap = pixmap_ret;
  }
  return _pixmap;
}

Pixmap
XcodaXpm::getPixmap (void)
{
  return _pixmap;
}
