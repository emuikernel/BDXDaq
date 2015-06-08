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
//      2D Simple Arc
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: cgArc.h,v $
//   Revision 1.1.1.1  1996/10/10 19:38:04  chen
//   CODA OO-graphics library for Xt
//
//
#ifndef _CG_ARC_H
#define _CG_ARC_H

#include <stdio.h>
#include <string.h>
#include <assert.h>

#include <cgConvex.h>
#include <cgScene.h>

class cgArc: public cgConvex
{
public:
  // constructor and destructor

  // construct an arc by center, radius and beginning and ending angle
  // angle is denoted by counter clock wise from x-axis
  // angle is in the unit of degree
  cgArc (double x, double y, double r, double bangle, double eangle);
  cgArc (cgPoint center, double r, double bangle, double eangle);
  virtual ~cgArc (void);

  // inherited operations
  cgPrim*    copy       (void) const;
  void       update     (const cgScene* s);
  void       draw       (const cgScene* s) const;

  // class name
  virtual const char* className (void) const {return "cgArc";}

protected:
  // setup all vertices
  void       setupVertices (void);

private:
  double    r_;
  cgPoint   cen_;
  double    a0_, a1_;

  // deny access to copy and assignment operator
  cgArc (const cgArc& arc);
  cgArc& operator = (const cgArc& arc);
};
#endif

  
  
  
