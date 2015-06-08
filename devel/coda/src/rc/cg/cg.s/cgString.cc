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
//      Implementation of 2D character string
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: cgString.cc,v $
//   Revision 1.2  1999/12/01 16:41:25  rwm
//   Nix compiler warnings via casts.
//
//   Revision 1.1.1.1  1996/10/10 19:38:04  chen
//   CODA OO-graphics library for Xt
//
//
#include "cgString.h"
#include <cgScene.h>
#include <cgXdrawing.h>

cgString::cgString (char* str, char* font, int alignment,
		    int yalignment)
:cgPrim (), x_ (0.0), y_ (0.0), alignment_ (alignment), 
 yalignment_ (yalignment)
{
#ifdef _TRACE_OBJECTS
  printf ("    Create cgString Class Object\n");
#endif
  str_ = new char[::strlen (str) + 1];
  ::strcpy (str_, str);
  fontname_ = new char[::strlen (font) + 1];
  ::strcpy (fontname_, font);
}

cgString::cgString (char* str, char* font, double x, double y, 
		    int alignment, int yalignment)
:cgPrim (), x_ (x), y_ (y), alignment_ (alignment), yalignment_ (yalignment)
{
#ifdef _TRACE_OBJECTS
  printf ("    Create cgString Class Object\n");
#endif
  str_ = new char[::strlen (str) + 1];
  ::strcpy (str_, str);
  fontname_ = new char[::strlen (font) + 1];
  ::strcpy (fontname_, font);
}

cgString::~cgString (void)
{
#ifdef _TRACE_OBJECTS
  printf ("    delete cgString Class Object\n");
#endif
  delete []str_;
  delete []fontname_;
}

cgPrim*
cgString::copy (void) const
{
  cgString* newstr = new cgString (str_, fontname_, 
				   x_, y_, alignment_, yalignment_);
  if (cxt_) 
    newstr->copyCgCxt (*cxt_);
  return newstr;
}

void
cgString::createCgCxt (cgDispDev& disp)
{
  cgPrim::createCgCxt (disp);
  cxt_->setFont (fontname_);
}

void
cgString::copyCgCxt (cgContext& cxt)
{
  cgPrim::copyCgCxt (cxt);
  cxt_->setFont (fontname_);
}

void
cgString::setCgCxt (cgContext& cxt)
{
  cgPrim::setCgCxt (cxt);
  cxt_->setFont (fontname_);
}

void
cgString::setx (double x)
{
  x_ = x;
}

void
cgString::sety (double y)
{
  y_ = y;
}

double
cgString::givx (void) const
{
  return x_;
}

double
cgString::givy (void) const
{
  return y_;
}

double
cgString::height (const cgScene* s) const
{
  if (cxt_ !=0 ) {
    int devh = cg_get_text_height (cxt_);
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

double
cgString::width (const cgScene* s) const
{
  if (cxt_ != 0) {
    int devw = cg_get_text_width (cxt_, str_);
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

void
cgString::draw (const cgScene* s) const
{
  assert (cxt_);
  // string position in the window coordination
  double xpos, ypos;
  if (alignment_ == CG_ALIGNMENT_BEGINNING)
    xpos = x_;
  else if (alignment_ == CG_ALIGNMENT_CENTER) 
    xpos = x_ - width (s)/2.0;
  else 
    xpos = x_ - width (s);

  if (yalignment_ == CG_ALIGNMENT_TOP) 
    ypos = y_ - height (s);
  else if (yalignment_ == CG_ALIGNMENT_BOTTOM)
    ypos = y_ + height (s);
  else
    ypos = y_;

  cgPoint winp0 (xpos, ypos);
  // convert string position into the device coordination
  s->givwv().win_dev (winp0);
  // draw string if not clipped
  cg_draw_string (cxt_, str_, (int) winp0.givx(), (int) winp0.givy());
}

cgPrim&
cgString::translate (const cgPoint& point)
{
  x_ += point.givx();
  y_ += point.givy();
  return *this;
}

char*
cgString::string (void) const
{
  return str_;
}
