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
//      2D Simple Arc + 2 lines connecting to center
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: cgPieArc.h,v $
//   Revision 1.1.1.1  1996/10/10 19:38:05  chen
//   CODA OO-graphics library for Xt
//
//
#ifndef _CG_PIE_ARC_H
#define _CG_PIE_ARC_H

#include <stdio.h>
#include <string.h>
#include <assert.h>

#include <cgConvex.h>
#include <cgScene.h>

class cgPieArc: public cgConvex
{
public:
  // constructor and destructor

  // construct an arc by center, radius and beginning and ending angle
  // angle is denoted by counter clock wise from x-axis
  // angle is in the unit of degree
  cgPieArc (double x, double y, double r, double bangle, double eangle);
  cgPieArc (cgPoint center, double r, double bangle, double eangle);
  virtual ~cgPieArc (void);

  // inherited operations
  cgPrim*    copy (void) const;
  void       update (const cgScene* s);
  void       draw (const cgScene* s) const;

protected:
  // realy set up all vertex list
  void setupVertices (void);
  

private:
  double    r_;
  cgPoint   cen_;
  double    a0_, a1_;

  // deny access to copy and assignment operator
  cgPieArc (const cgPieArc& arc);
  cgPieArc& operator = (const cgPieArc& arc);
};
#endif

  
  
  
