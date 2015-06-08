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
//      2D vertex Class
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: cgVertex.h,v $
//   Revision 1.1.1.1  1996/10/10 19:38:03  chen
//   CODA OO-graphics library for Xt
//
//
#ifndef _CG_VERTEX_T
#define _CG_VERTEX_T

#include <cgPoint.h>

class cgEdge ;

class cgVertex : virtual public cgPoint
{
public:
  cgVertex (void) ;
  cgVertex (double,double,cgVertex* = 0) ;
  cgVertex (const cgPoint&) ;
  virtual ~cgVertex (void) ;

  virtual   cgEdge*    make_edge(const cgVertex&) const ;
  virtual   cgVertex*  copy() const ;
  cgVertex* givnext() const { return next; }
  void      setnext (cgVertex* v) { next = v; }

protected:
  cgVertex* next ;
} ;
#endif
