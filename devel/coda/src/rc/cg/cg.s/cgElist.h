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
//      Edge List Class
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: cgElist.h,v $
//   Revision 1.1.1.1  1996/10/10 19:38:03  chen
//   CODA OO-graphics library for Xt
//
//
#ifndef _CG_ELIST_T
#define _CG_ELIST_T

#include <cgVertex.h>
#include <cgEdge.h>

#include <cgContext.h>

class cgElist                  // edge list for filling convex polygon
{
public:
  cgElist  (cgVertex *);
  cgElist  (void) ;
  ~cgElist (void) {}
  
  // setup context for all edges
  virtual void        setCgCxt (cgContext& cxt);

  virtual void fill (void (cgEdge::*)(double)) ;

protected:
  void insert_edge    (cgVertex&, cgVertex&);
  void include        (void);
  void exclude        (void);
  virtual void swap   (void);
  virtual void update (void);

  cgEdge* head;              // beginning of edge list
  cgEdge* rite;              // points beyond rightmost cutting edge
  double scan ;
  double scan_decrement ;
} ;
#endif
