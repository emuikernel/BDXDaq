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
//      2D polygon class
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: cgPolygon.h,v $
//   Revision 1.1.1.1  1996/10/10 19:38:03  chen
//   CODA OO-graphics library for Xt
//
//
#ifndef _CG_POLYGON_T
#define _CG_POLYGON_T

#include <cgPattern.h>
#include <cgConvex.h>
#include <cgScene.h>
#include <cgElist.h>

class cgPolygon : public cgConvex      // polygon is a container class
{
public:
  cgPolygon  (void) ;
  cgPolygon  (cgPoint* p, int num);
  ~cgPolygon (void) ;

  cgPrim*    copy (void) const ;

private:
  cgElist* make_elist(cgVertex*) const ;           // return a glist!

  cgPolygon  (const cgPolygon&);
} ;
#endif
