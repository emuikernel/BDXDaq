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
//      Implmentation of 2D vertex class
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: cgVertex.cc,v $
//   Revision 1.1.1.1  1996/10/10 19:38:04  chen
//   CODA OO-graphics library for Xt
//
//
#include <cgEdge.h>
#include <cgWinView.h>
#include <cgLine.h>
#include "cgVertex.h"

cgVertex::cgVertex() : cgPoint() , next(0) {}

cgVertex::cgVertex(double xx, double yy, cgVertex* nxt)
:cgPoint(xx,yy) , next(nxt) {}

cgVertex::cgVertex(const cgPoint& p)
:cgPoint(p) , next(0) {}

cgVertex::~cgVertex()
{
   delete next;
}

cgVertex* cgVertex::copy() const
{
  return new cgVertex(*this);
}

cgEdge* cgVertex::make_edge(const cgVertex& v) const
{
  if ((int)y == (int)v.y)                    // all y-values are >= 0
    return 0;
  return y > v.y 
    ?   new cgEdge(*this,v)
      :   new cgEdge(v,*this);
}
