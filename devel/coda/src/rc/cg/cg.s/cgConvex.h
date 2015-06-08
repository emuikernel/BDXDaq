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
//      2D convex Class header file
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: cgConvex.h,v $
//   Revision 1.1.1.1  1996/10/10 19:38:04  chen
//   CODA OO-graphics library for Xt
//
//
#ifndef _CG_CONVEX_T
#define _CG_CONVEX_T

#include <cgVertex.h>
#include <cgPattern.h>
#include <cgPrim.h>
#include <cgScene.h>
#include <cgElist.h>

class cgConvex : public cgPrim  // convex polygon is a container class
{
public:
  cgConvex  (void) ;
  cgConvex  (cgPoint* points, int num) ;
  ~cgConvex (void) ;

  virtual void draw        (const cgScene*) const ;
  void         operator =  (const cgConvex &);
  void         operator += (const cgVertex&) ;
  virtual void solid       (const cgScene*) const;        // solid fill
  virtual void hatch       (const cgScene*,int) const ;   // crosshatch fill
  virtual void pattern     (const cgScene*,const cgPattern&) const ;
  cgPrim&      rot         (double);         // 2-D rotation about centroid
  cgPrim&      translate   (const cgPoint&) ;
  cgPrim*      copy        (void) const ;

protected:
  cgVertex* v ;
  int vernum ;
  cgPoint cen ;                                       // 2-D centroid

private:
  virtual cgElist* make_elist(cgVertex*) const ;     // make edgelist
  cgConvex  (const cgConvex&) ;
};
#endif
