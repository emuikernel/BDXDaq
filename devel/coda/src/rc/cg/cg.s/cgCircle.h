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
//      2D circle Class
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: cgCircle.h,v $
//   Revision 1.1.1.1  1996/10/10 19:38:03  chen
//   CODA OO-graphics library for Xt
//
//
#ifndef _CG_CIRCLE_T
#define _CG_CIRCLE_T

#include <cgConvex.h>
#include <cgScene.h>

class cgCircle : public cgConvex
{                                  // center is the convex's centroid
public:
  cgCircle (cgPoint center,double r);

  cgPrim* copy  (void) const ;
  void    update (const cgScene* s);
  void    draw  (const cgScene* s) const;   // circle is drawn like a line

  cgPrim& rot   (double) { return *this; }

protected:
  // update all vertex coordinates from device coordinates
  // since a circle must be a circle on a device
  void updateVertices (const cgScene* s);

private:
  double r ;                                               // radius
  cgPoint c;

  cgCircle (const cgCircle&);
};
#endif
