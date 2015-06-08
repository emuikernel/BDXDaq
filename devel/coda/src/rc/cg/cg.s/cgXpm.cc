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
//   $Log: cgXpm.cc,v $
//   Revision 1.3  2000/01/28 16:29:22  rwm
//   include should be X11/xpm.h
//
//   Revision 1.2  1997/08/25 16:01:51  heyes
//   fix some display problems
//
//   Revision 1.1.1.1  1996/10/10 19:38:06  chen
//   CODA OO-graphics library for Xt
//
//
#include <X11/xpm.h>
#include "cgXpm.h"

cgXpm::cgXpm (char **pix)
:disp_ (0), pix_ (pix), pixmap_ (0), bgsymbol_ (0),
 width_ (0), height_ (0)
{
#ifdef _TRACE_OBJECTS
  printf("    Create cgXpm Class Object \n");
#endif
  // get pixmap width and height
  int wd, ht, token0, token1;
  if (sscanf (pix_[0], "%d %d %d %d", &wd, &ht, &token0, &token1) != 4) {
    fprintf (stderr, "Fatal Error: Invalid pixmap file\n");
  }
  else {
    width_ = wd;
    height_ = ht;
  }
}

cgXpm::cgXpm (char **pix, char *symbol)
:disp_ (0), pix_ (pix), pixmap_ (0), width_ (0), height_ (0)
{
#ifdef _TRACE_OBJECTS
  printf("    Create cgXpm Class Object \n");
#endif
  bgsymbol_ = new char[::strlen(symbol) + 1];
  ::strcpy (bgsymbol_, symbol);
  // get width and height
  int wd, ht, token0, token1;
  if (sscanf (pix_[0], "%d %d %d %d", &wd, &ht, &token0, &token1) != 4) {
    fprintf (stderr, "Fatal Error: Invalid pixmap file\n");
  }
  else {
    width_ = wd;
    height_ = ht;
  }
}

cgXpm::~cgXpm (void)
{
#ifdef _TRACE_OBJECTS
  printf("     Destroy cgXpm Object \n");
#endif
  if (bgsymbol_)
    delete []bgsymbol_;
  if (pixmap_ != 0) {
    XFreePixmap(xdisplay_, pixmap_);
  }
  pixmap_ = 0;
}

Pixmap
cgXpm::getPixmap (void)
{
  XpmAttributes  attr;
  int            err, numsymbols = 0;
  unsigned long  pixmap_ret, pixmap_mask;
  unsigned long  valuemask = 0;
  Arg            arg[10];
  int            ac = 0;
  XpmColorSymbol col_symbol[5];
  
  assert (disp_);
  if (disp_->drawable () != 0) {
    if (!pixmap_) {
      if(bgsymbol_) {
	ac = 0;
	col_symbol[numsymbols].name = bgsymbol_;
	col_symbol[numsymbols].value = (char *)0;
	col_symbol[numsymbols++].pixel = disp_->bg ();
	
	attr.colormap = disp_->cmap ();
	attr.depth = disp_->depth ();
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
	col_symbol[numsymbols++].pixel = disp_->bg ();

	attr.colormap = disp_->cmap ();
	attr.depth = disp_->depth ();
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
      err = XpmCreatePixmapFromData (disp_->display (), disp_->drawable (), 
				     pix_, &pixmap_ret,
				     &pixmap_mask,
				     &attr);
      if (err != XpmSuccess){
	pixmap_ret = 0;
	pixmap_ = 0;
      }
      else{
	pixmap_ = pixmap_ret;
	width_ = attr.width;
	height_ = attr.height;
      }
    }
  }
  return pixmap_;
}

void
cgXpm::dispDev (cgDispDev* dev)
{
  disp_ = dev;
  xdisplay_ = disp_->display ();
}

unsigned int
cgXpm::width (void) const
{
  return width_;
}

unsigned int
cgXpm::height (void) const
{
  return height_;
}

char**
cgXpm::xpmData (void) const
{
  return pix_;
}

char*
cgXpm::bgSymbol (void) const
{
  return bgsymbol_;
}

