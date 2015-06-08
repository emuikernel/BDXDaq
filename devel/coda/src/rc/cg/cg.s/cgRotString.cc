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
//      Implementation of 2D rotated character string
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: cgRotString.cc,v $
//   Revision 1.1.1.1  1996/10/10 19:38:05  chen
//   CODA OO-graphics library for Xt
//
//
#include "cgRotString.h"
#include <cgScene.h>
#include <cgXdrawing.h>
#include <rotated.h>

cgRotString::cgRotString (char* str, char* font, 
			  float angle, float mag,int alignment)
:cgPrim (), x_ (0.0), y_ (0.0),
 angle_ (angle), mag_ (mag),
 alignment_ (alignment)
{
#ifdef _TRACE_OBJECTS
  printf ("    Create cgRotString Class Object\n");
#endif
  str_ = new char[::strlen (str) + 1];
  ::strcpy (str_, str);
  fontname_ = new char[::strlen (font) + 1];
  ::strcpy (fontname_, font);
}

cgRotString::cgRotString (char* str, char* font, double x, double y, 
			  float angle, float mag,
			  int alignment)
:cgPrim (), x_ (x), y_ (y), 
 angle_ (angle), mag_ (mag), 
 alignment_ (alignment)
{
#ifdef _TRACE_OBJECTS
  printf ("    Create cgRotString Class Object\n");
#endif
  str_ = new char[::strlen (str) + 1];
  ::strcpy (str_, str);
  fontname_ = new char[::strlen (font) + 1];
  ::strcpy (fontname_, font);
}

cgRotString::~cgRotString (void)
{
#ifdef _TRACE_OBJECTS
  printf ("    delete cgRotString Class Object\n");
#endif
  delete []str_;
  delete []fontname_;
}

cgPrim*
cgRotString::copy (void) const
{
  cgRotString* newstr = new cgRotString (str_, fontname_, 
					 x_, y_, angle_,
					 mag_, alignment_);
  if (cxt_) 
    newstr->copyCgCxt (*cxt_);
  return newstr;
}

void
cgRotString::createCgCxt (cgDispDev& disp)
{
  cgPrim::createCgCxt (disp);
  cxt_->setFont (fontname_);
}

void
cgRotString::copyCgCxt (cgContext& cxt)
{
  cgPrim::copyCgCxt (cxt);
  cxt_->setFont (fontname_);
}

void
cgRotString::setCgCxt (cgContext& cxt)
{
  cgPrim::setCgCxt (cxt);
  cxt_->setFont (fontname_);
}

void
cgRotString::setx (double x)
{
  x_ = x;
}

void
cgRotString::sety (double y)
{
  y_ = y;
}

double
cgRotString::givx (const cgScene* s) const
{
  if (cxt_ != 0) {
    XPoint* xp = 0;
    XFontStruct* font = cxt_->getFont ();
    Display* disp = cxt_->xdisplay ();

    if (mag_ != 1.0) 
      XRotSetMagnification(mag_);
    cgPoint winp (x_, y_);
    // convert string position into the device coordination
    s->givwv().win_dev (winp);

    xp = XRotTextExtents (disp, font, angle_, 
			  (int)winp.givx(), (int)winp.givy(), 
			  str_, alignment_);
    // free font structure without unloading this font
    XFreeFontInfo (0, font, 1);
    if (xp) {
      int x = xp[0].x;
      // arbitrarily assign begining pont at 0, 0
      cgPoint devp1 (x, 0.0);
      cgPoint winp1 = s->givwv().dev_win (devp1);
      // remember X widnow inverse y direction
      return winp1.givx ();
    }
  }
  return 0.0;
}

double
cgRotString::givy (const cgScene* s) const
{
  if (cxt_ != 0) {
    XPoint* xp = 0;
    XFontStruct* font = cxt_->getFont ();
    Display* disp = cxt_->xdisplay ();

    if (mag_ != 1.0) 
      XRotSetMagnification(mag_);

    cgPoint winp (x_, y_);
    // convert string position into the device coordination
    s->givwv().win_dev (winp);

    xp = XRotTextExtents (disp, font, angle_, 
			  (int)winp.givx (), (int)winp.givy (), 
			  str_, alignment_);
    // free font structure without unloading this font
    XFreeFontInfo (0, font, 1);

    if (xp) {
      int y = xp[0].y;
      // arbitrarily assign begining piont at 0, 0
      cgPoint devp1 (0.0, y);
      cgPoint winp1 = s->givwv().dev_win (devp1);
      // remember X widnow inverse y direction
      return winp1.givy ();
    }
  }
  return 0.0;
}

double
cgRotString::height (const cgScene* s) const
{
  if (cxt_ !=0 ) {
    XPoint* xp = 0;
    XFontStruct* font = cxt_->getFont ();
    Display* disp = cxt_->xdisplay ();

    if (mag_ != 1.0) 
      XRotSetMagnification(mag_);

    cgPoint winp (x_, y_);
    // convert string position into the device coordination
    s->givwv().win_dev (winp);

    xp = XRotTextExtents (disp, font, angle_, 
			  (int)winp.givx(), (int)winp.givy(),
			  str_, alignment_);
    // free font structure without unloading this font
    XFreeFontInfo (0, font, 1);

    if (xp) {
      int devh = xp[0].y - xp[1].y;
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
  }
  else
    return 0.0;
}

double
cgRotString::width (const cgScene* s) const
{
  if (cxt_ != 0) {
    XPoint* xp = 0;
    XFontStruct* font = cxt_->getFont ();
    Display* disp = cxt_->xdisplay ();

    if (mag_ != 1.0) 
      XRotSetMagnification(mag_);

    cgPoint winp (x_, y_);
    // convert string position into the device coordination
    s->givwv().win_dev (winp);

    xp = XRotTextExtents (disp, font, angle_, 
			  (int)winp.givx(), (int)winp.givy(),
			  str_, alignment_);
    // free font structure without unloading this font
    XFreeFontInfo (0, font, 1);

    if (xp) {
      int devw = xp[3].x - xp[0].x;

      if (devw == 0)
	return 0.0;
      // arbitrarily assign begining pont at 0, 0
      cgPoint devp0;
      cgPoint devp1 (devw, 0.0);
      cgPoint winp0 = s->givwv().dev_win (devp0);
      cgPoint winp1 = s->givwv().dev_win (devp1);
      return winp1.givx () - winp0.givx ();    
    }
  }
  else
    return 0.0;
}

void
cgRotString::draw (const cgScene* s) const
{
  assert (cxt_);
  cgPoint winp0 (x_, y_);
  // convert string position into the device coordination
  s->givwv().win_dev (winp0);

  XFontStruct* font = cxt_->getFont ();

  if (font) {
    if (mag_ != 1.0)
      XRotSetMagnification (mag_);

    XRotDrawString (cxt_->xdisplay(), font, angle_,
		    cxt_->xdrawable (), cxt_->xgc (),
		    (int)winp0.givx(), (int)winp0.givy (),
		    str_);

    // free font info without unloading font 
    XFreeFontInfo (0, font, 1);
  }
}

cgPrim&
cgRotString::translate (const cgPoint& point)
{
  x_ += point.givx();
  y_ += point.givy();
  return *this;
}

char*
cgRotString::string (void) const
{
  return str_;
}

void
cgRotString::string (const cgScene*s, char* str)
{
  if (cxt_->xdrawable ())
    erase (s);

  delete []str_;

  str_ = new char[::strlen (str) + 1];
  ::strcpy (str_, str);

  if (cxt_->xdrawable ())
    draw (s);
}

