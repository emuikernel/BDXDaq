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
//      2D Line Implementation
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: cgLine.cc,v $
//   Revision 1.2  1997/11/24 22:02:10  rwm
//   Eliminated CC warning by renaming vars.
//
//   Revision 1.1.1.1  1996/10/10 19:38:04  chen
//   CODA OO-graphics library for Xt
//
//
#include <math.h>
#include <stdio.h>

#include <cgXdrawing.h>
#include "cgLine.h"

cgLine::cgLine (void)
:cgPrim (), a (0,0), b (0,0), cen (0,0), useClipAlg_ (0)
{
#ifdef _TRACE_OBJECTS
  printf ("    Create cgLine Class Object\n");
#endif
  // empty
}       

cgLine::cgLine (double x1,double y1,double x2,double y2)
:cgPrim (), a (x1,y1), b (x2,y2), cen ((a+b)/2),useClipAlg_ (0)
{
#ifdef _TRACE_OBJECTS
  printf ("    Create cgLine Class Object\n");
#endif
  // empty
}

cgLine::cgLine (const cgPoint& aa, const cgPoint& bb)
:cgPrim (), a (aa), b (bb), cen ((aa+bb)/2),useClipAlg_ (0)
{
#ifdef _TRACE_OBJECTS
  printf ("    Create cgLine Class Object\n");
#endif
  // empty
}

cgLine::~cgLine (void)
{
#ifdef _TRACE_OBJECTS
  printf ("    Delete cgLine Class Object\n");
#endif
}


cgPrim* 
cgLine::copy() const                          // copy function
{
  cgLine* l = new cgLine (a, b);
  if (cxt_)
    l->copyCgCxt (*cxt_);
  if (useClipAlg_)
    l->useClippingAlg ();
  return l;
}

void
cgLine::draw (const cgScene* scene) const
{
  if (useClipAlg_) {
    cgLine* l = scene->givwv().clipln(*this) ;    // 2-D clip

    if (l) {
      scene->givwv().win_dev(l->a);      // 2-D transform
      scene->givwv().win_dev(l->b);

      assert (cxt_);
      draw_line (cxt_, (int)l->a.givx(),(int)l->a.givy(),
		 (int)l->b.givx(),(int)l->b.givy());
      delete l;
    }
  }
  else {
    cgPoint ta = a;
    cgPoint tb = b;

    scene->givwv().win_dev(ta);      // 2-D transform
    scene->givwv().win_dev(tb);

    assert (cxt_);
    draw_line (cxt_, (int)ta.givx(),(int)ta.givy(),
	       (int)tb.givx(),(int)tb.givy());
  }
}

void
cgLine::setEndPoints (const cgPoint& start, const cgPoint& end)
{
  a = start;
  b = end;
  cgPoint temp (a + b/2);
  cen = temp;
}

void
cgLine::setEndPoints (double x0, double y0, double x1, double y1)
{
  a.setx (x0);
  a.sety (y0);
  b.setx (x1);
  b.sety (y1);
  cgPoint temp (a + b/2);
  cen = temp;
}

// deg in degrees
cgPrim&
cgLine::rot(double deg)                          
{
  cgPoint center(this->cen) ;
  
  translate(-center);
  a.rot(deg);
  b.rot(deg);
  translate(center);
  return *this;
}

cgPrim&
cgLine::translate(const cgPoint& p)
{
  a += p;
  b += p;
  cen += p;
  return *this;
}

void
cgLine::useClippingAlg (void)
{
  useClipAlg_ = 1;
}

void
cgLine::withoutClippingAlg (void)
{
  useClipAlg_ = 0;
}

