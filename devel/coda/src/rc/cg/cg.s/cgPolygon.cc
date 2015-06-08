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
//      Implementation of 2D polygon
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: cgPolygon.cc,v $
//   Revision 1.1.1.1  1996/10/10 19:38:03  chen
//   CODA OO-graphics library for Xt
//
//
#include <cgGlist.h>
#include "cgPolygon.h"

cgPolygon::cgPolygon() 
:cgConvex () 
{
  // empty
}

cgPolygon::cgPolygon (cgPoint* points, int num)
:cgConvex (points, num)
{
  // empty
}

cgPolygon::~cgPolygon()
{
  // empty
}

cgPrim*
cgPolygon::copy (void) const
{
  cgPolygon* pl = new cgPolygon ();
  for (cgVertex* p = v; p; p = p->givnext())
    *pl += *p;
  if (cxt_)
    pl->copyCgCxt (*cxt_);  
  return pl;
}

cgElist*
cgPolygon::make_elist(cgVertex* vv) const
{
  return new cgGlist (vv);         // make edgelist for general polygon
}
