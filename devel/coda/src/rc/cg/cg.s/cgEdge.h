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
//      2D edge class
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: cgEdge.h,v $
//   Revision 1.1.1.1  1996/10/10 19:38:03  chen
//   CODA OO-graphics library for Xt
//
//
#ifndef _CG_EDGE_T
#define _CG_EDGE_T

#include <cgContext.h>

class cgPoint ;

class cgEdge                                           // polygon edge
{
public:
  cgEdge (const cgPoint&,const cgPoint&) ;
  // edges must be deleted singly
  virtual ~cgEdge (void) {if (cxt_) delete cxt_;} 

  double      givymax ()             { return ymax; }
  double      givymin ()             { return ymin; }
  double      givx ()                { return x; }
  cgEdge*     givnext()              { return next; }
  void        setnext (cgEdge* e) { next = e; }
  cgEdge** givaddrnext()          { return &next; }
  virtual  void update() ;
  virtual  void solidscan(double) ;
  virtual  void hatchscan(double) ;
  virtual  void patternscan(double) ;

  // setup graphics context
  virtual void       setCgCxt (cgContext& cxt) {if (cxt_) delete cxt_;
						cxt_ = new cgContext (cxt);}
  virtual cgContext*  cgCxt (void) {return cxt_;}
   

protected: 
  double  ymax  ,           // larger of the edge's y-values
  ymin  ,                   // smaller of the edge's y-values
  dx    ,                   // negative inverse slope of the edge
  x     ;                   // intersection x-value of edge and scanline

  cgEdge* next;
  cgContext* cxt_;          // graphics context
} ;
#endif
