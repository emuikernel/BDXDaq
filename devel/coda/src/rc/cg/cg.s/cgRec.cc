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
//      Implementation of 2D rectangle
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: cgRec.cc,v $
//   Revision 1.1.1.1  1996/10/10 19:38:04  chen
//   CODA OO-graphics library for Xt
//
//
#include "cgRec.h"

cgRec::cgRec (void)
:cgPolygon (), bleft_ (), width_ (0.0), height_ (0.0)
{
  // empty
}

cgRec::cgRec (double blx, double bly, double width, double height)
:cgPolygon (), bleft_ (blx, bly), width_ (width), height_ (height)
{
  // from bottom left 
  *this += bleft_;
  // to top left
  cgPoint tleft (blx, bly + height_);
  *this += tleft;
  cgPoint tright (blx + width_, bly + height_);
  *this += tright;
  cgPoint bright (blx + width, bly);
  *this += bright;
}

cgRec::cgRec (const cgPoint& bl, double width, double height)
:cgPolygon (), bleft_ (bl), width_ (width), height_ (height)
{
  // from bottom left 
  *this += bleft_;
  // to top left
  cgPoint tleft (bleft_.givx (), bleft_.givy () + height_);
  *this += tleft;
  cgPoint tright (bleft_.givx () + width_, bleft_.givy () + height_);
  *this += tright;
  cgPoint bright (bleft_.givx () + width, bleft_.givy ());
  *this += bright;
}
  
cgRec::~cgRec (void)
{
  // empty
}

cgPoint
cgRec::givbl (void) const
{
  return bleft_;
}

double
cgRec::width (void) const
{
  return width_;
}

double
cgRec::height (void) const
{
  return height_;
}

void
cgRec::setbl (const cgPoint& cgbl)
{
  double deltax = cgbl.givx() - bleft_.givx ();
  bleft_ = cgbl;
  cgPoint point (deltax, 0.0);
  translate (point);
}

void
cgRec::width (double w)
{
  width_ = w;
}

void
cgRec::height (double h)
{
  height_ = h;
}
