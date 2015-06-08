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
//      2d Point Class Implementation
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: cgPoint.cc,v $
//   Revision 1.1.1.1  1996/10/10 19:38:04  chen
//   CODA OO-graphics library for Xt
//
//
#include <math.h>
#include <cgConst.h>
#include "cgPoint.h"

cgPoint::cgPoint (void)
:x (0), y (0)
{
  // empty
}

cgPoint::cgPoint(double xx, double yy)
:x (xx), y (yy)
{
  // empty
}

cgPoint::cgPoint (const cgPoint& p) 
:x (p.x), y (p.y)
{
  // empty
}

cgPoint&
cgPoint::operator = (const cgPoint& p)
{
  if (this != &p) {
    x = p.x;
    y = p.y;
  }
  return *this;
}

cgPoint 
cgPoint::operator + (const cgPoint& a) const
{
  return cgPoint (x+a.x,y+a.y);
}

cgPoint 
cgPoint::operator - (const cgPoint& a) const
{
  return cgPoint(x-a.x,y-a.y);
}

cgPoint
cgPoint::operator *(double d) const
{
  return cgPoint(x*d,y*d);
}
cgPoint 
cgPoint::operator /(double d) const
{
  return cgPoint(x/d,y/d);
}

cgPoint 
operator *(double d,const cgPoint& p)
{
  return p*d;
}

cgPoint& 
cgPoint::operator +=(const cgPoint& a)
{
  x += a.x; y += a.y;
  return *this;
}

cgPoint& 
cgPoint::operator -=(const cgPoint& a)
{
  x -= a.x; y -= a.y;
  return *this;
}

cgPoint
cgPoint::operator - () const
{
  return cgPoint(-x,-y);
}

cgPoint&
cgPoint::rot (double deg)
{
  double c = cos(deg*DEG2RAD);
  double s = sin(deg*DEG2RAD);
  double tx = x*c - y*s ;
  double ty = x*s + y*c;

  x = tx;
  y = ty;

  return *this;
}
