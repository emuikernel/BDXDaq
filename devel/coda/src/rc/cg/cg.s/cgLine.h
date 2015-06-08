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
//      2D Line Class Header file
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: cgLine.h,v $
//   Revision 1.1.1.1  1996/10/10 19:38:04  chen
//   CODA OO-graphics library for Xt
//
//
#ifndef CG_LINE_T
#define CG_LINE_T
#include <cgPrim.h>
#include <cgPoint.h>
#include <cgScene.h>

class cgLine: public cgPrim   // 2D line
{
public:
  cgLine (void) ;
  cgLine (double,double,double,double) ;
  cgLine (const cgPoint&, const cgPoint&) ;
  virtual ~cgLine (void);

  cgPrim* copy  (void) const ;
  void    draw  (const cgScene*) const ;
  cgPrim& rot  (double) ;
  cgPrim& translate(const cgPoint&) ;
  cgPoint giva() const { return a; }
  cgPoint givb() const { return b; }
  void    setEndPoints (const cgPoint& start, const cgPoint& end);
  void    setEndPoints (double x0, double y0, double x1, double y1);

  // operation on using clipping algorothim
  void    useClippingAlg (void);
  void    withoutClippingAlg (void);

  // class name
  virtual const char* className (void) const {return "cgLine";}

private:
  cgLine (const cgLine &) ;                        // X(X&) constructor

  cgPoint a , b ;                                  // line end points
  cgPoint cen ;                                    // centroid

  // flag to tell whether to use clip algorithm
  // default is not to use
  int     useClipAlg_;
} ;
#endif
