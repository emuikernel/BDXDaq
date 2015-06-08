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
//      cg Image of XPM format
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: cgXpmImage.cc,v $
//   Revision 1.2  1999/12/01 16:41:26  rwm
//   Nix compiler warnings via casts.
//
//   Revision 1.1.1.1  1996/10/10 19:38:04  chen
//   CODA OO-graphics library for Xt
//
//
#include <cgScene.h>
#include <cgXdrawing.h>
#include <cgRec.h>
#include "cgXpmImage.h"


#define CG_DEFAULT_FONT "-*-fixed-medium-*-*-*-*-100-*-*-*-*-*-*"

cgXpmImage::cgXpmImage (char* name, char** pix, char* bgsymbol, int alignment)
:cgPrim (), x_ (0.0), y_ (0.0), alignment_ (alignment)
{
#ifdef _TRACE_OBJECTS
  printf ("    Create cgXpmImage Class Object\n");
#endif
  if (bgsymbol == 0) 
    xpm_ = new cgXpm (pix);
  else
    xpm_ = new cgXpm (pix, bgsymbol);
  string_ = new cgString (name, CG_DEFAULT_FONT, alignment_);
};

cgXpmImage::cgXpmImage (char* name, double x, double y,
			char** pix, char* bgsymbol, int alignment)
:cgPrim (), x_ (x), y_ (y), alignment_ (alignment)
{
#ifdef _TRACE_OBJECTS
  printf ("    Create cgXpmImage Class Object\n");
#endif
  if (bgsymbol == 0) 
    xpm_ = new cgXpm (pix);
  else
    xpm_ = new cgXpm (pix, bgsymbol);
  string_ = new cgString (name, CG_DEFAULT_FONT, x, y, alignment_);
};

cgXpmImage::~cgXpmImage (void)
{
#ifdef _TRACE_OBJECTS
  printf ("    Delete cgXpmImage Class Object\n");
#endif
  delete xpm_;
  delete string_;
}

void
cgXpmImage::init (void)
{
  assert (cxt_);
  xpm_->dispDev (&(cxt_->dispDev ()) );
  // realy create xpm image
  xpm_->getPixmap ();
}

cgPrim*
cgXpmImage::copy (void) const
{
  cgXpmImage *newimage = new cgXpmImage (string_->string (), x_, y_,
					 xpm_->xpmData (), xpm_->bgSymbol (),
					 alignment_);
  if (cxt_)
    newimage->copyCgCxt (*cxt_);
  return newimage;
}

void
cgXpmImage::draw (const cgScene* s) const
{
  assert (cxt_);

  // pixmap position in the window coordination system
  double xpos, ypos;
  if (alignment_ == CG_ALIGNMENT_BEGINNING)
    xpos = x_;
  else if (alignment_ == CG_ALIGNMENT_CENTER)
    xpos = x_ - width (s)/2.0;
  else
    xpos = x_ - width (s);
  ypos = y_;

  cgPoint winp0 (xpos, ypos);
  // convert this position into the device coordination system
  s->givwv().win_dev (winp0);
  // draw xpm
  if (xpm_->getPixmap () != 0)
    cg_draw_pixmap (cxt_, xpm_->getPixmap(), 
		    xpm_->width(), xpm_->height(), 
		    (int) winp0.givx(), (int) winp0.givy());
  else
    string_->draw (s);
}

void
cgXpmImage::erase (const cgScene* s) const
{
  if (xpm_->getPixmap () == 0)
    string_->erase (s);
  else {
    Pixel fg, bg;
    cxt_->getBackground (bg);
    cxt_->getForeground (fg);
    cxt_->setForeground (bg);
    
    // figure out botton left coordinate of erase rectangle
    double xpos, ypos;
    if (alignment_ == CG_ALIGNMENT_BEGINNING)
      xpos = x_;
    else if (alignment_ == CG_ALIGNMENT_CENTER)
      xpos = x_ - width (s)/2.0;
    else
      xpos = x_ - width (s);
    ypos = y_;

    cgRec rec (xpos, ypos, width (s), height (s));

    rec.setCgCxt (*cxt_);
    rec.draw (s);
    rec.solid (s);
    cxt_->setForeground (fg);    
  }
}

cgPrim&
cgXpmImage::translate (const cgPoint& point)
{
  if (xpm_->getPixmap () == 0)
    string_->translate (point);
  x_ += point.givx ();
  y_ += point.givy ();

  return *this;
}

void
cgXpmImage::setx (double x)
{
  string_->setx (x);
  x_ = x;
}

void
cgXpmImage::sety (double y)
{
  string_->sety (y);
  y_ = y;
}

double
cgXpmImage::givx (void) const
{
  return x_;
}

double
cgXpmImage::givy (void) const
{
  return y_;
}

double
cgXpmImage::width (const cgScene* s) const
{
  if (cxt_ != 0) {
    int devw = xpm_->width ();
    if (devw == 0)
      return 0.0;
    // arbitrarily assign begining pont at 0, 0
    cgPoint devp0;
    cgPoint devp1 (devw, 0.0);
    cgPoint winp0 = s->givwv().dev_win (devp0);
    cgPoint winp1 = s->givwv().dev_win (devp1);
    return winp1.givx () - winp0.givx ();    
  }
  else
    return 0.0;
}

double
cgXpmImage::height (const cgScene* s) const
{
  if (cxt_ !=0 ) {
    int devh = xpm_->height ();
    if (devh == 0)
      return 0.0;
    // arbitrarily assign begining pont at 0, 0
    cgPoint devp0;
    cgPoint devp1 (0.0, devh);
    cgPoint winp0 = s->givwv().dev_win (devp0);
    cgPoint winp1 = s->givwv().dev_win (devp1);
    // remember X widnow inverse y direction
    return -winp1.givy () + winp0.givy ();
  }
  else
    return 0.0;
}

void
cgXpmImage::createCgCxt (cgDispDev& disp)
{
  cgPrim::createCgCxt (disp);
  string_->setCgCxt (*cxt_);
}

void
cgXpmImage::copyCgCxt (cgContext& cxt)
{
  cgPrim::copyCgCxt (cxt);
  string_->setCgCxt (*cxt_);
}

void
cgXpmImage::setCgCxt (cgContext& cxt)
{
  cgPrim::setCgCxt (cxt);
  string_->setCgCxt (*cxt_);
}


				     
