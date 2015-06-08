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
//   $Log: cgArc.cc,v $
//   Revision 1.2  1999/12/01 16:41:25  rwm
//   Nix compiler warnings via casts.
//
//   Revision 1.1.1.1  1996/10/10 19:38:05  chen
//   CODA OO-graphics library for Xt
//
//
#include <math.h>

#include <cgLine.h>
#include <cgConst.h>
#include "cgArc.h"

// draw arc by using string line every 4 degrees
const double CG_ARC_DEG_INTERVAL = 4;

cgArc::cgArc (double cx, double cy, double r, double a0, double a1)
:cgConvex (), cen_ (cx, cy), r_ (r), a0_ (a0), a1_ (a1)
{
#ifdef _TRACE_OBJECTS
  printf ("        Create cgArc Class Object\n");
#endif
  // work only within 0-360
  a0_ = a0_ - ((int)(a0_/360.0))*360.0;
  a1_ = a1_ - ((int)(a1_/360.0))*360.0;

  assert (a0_ != a1_);

  if (a0_ > a1_) {
    double temp = a0_;
    a0_ = a1_;
    a1_ = temp;
  }

  // update center position from cgConvex
  cen = cen_;

  // setup all vertices
  setupVertices ();
}

cgArc::cgArc (cgPoint c, double r, double a0, double a1)
:cgConvex (), cen_ (c), r_ (r), a0_ (a0), a1_ (a1)
{
#ifdef _TRACE_OBJECTS
  printf ("        Create cgArc Class Object\n");
#endif
  // work only within 0-360
  a0_ = a0_ - ((int)(a0_/360.0))*360.0;
  a1_ = a1_ - ((int)(a1_/360.0))*360.0;

  assert (a0_ != a1_);

  if (a0_ > a1_) {
    double temp = a0_;
    a0_ = a1_;
    a1_ = temp;
  }

  // update center position from cgConvex
  cen = cen_;

  // setup all vertices
  setupVertices ();
}

cgArc::~cgArc (void)
{
#ifdef _TRACE_OBJECTS
  printf ("        Delete cgArc Class Object\n");
#endif
}

void
cgArc::setupVertices (void)
{
  // create vertex list
  double x1 = r_;
  double y1 = 0.0;

  // rotate to the first point
  double deg = PI/180*a0_;
  
  double x2 = x1*cos (deg) - y1*sin (deg);
  double y2 = x1*sin (deg) + y1*cos (deg);
  // update vertex list
  v = new cgVertex (x2 + cen_.givx (), y2 + cen_.givy(), v);
  vernum = 1;

  // calculate how many points the arc is going to have
  int numVertices = (int) ((a1_ - a0_)/CG_ARC_DEG_INTERVAL);
  
  double su = sin (PI/180*CG_ARC_DEG_INTERVAL);
  double cu = cos (PI/180*CG_ARC_DEG_INTERVAL);

  for (int i = 0; i < numVertices; i++) {
    double x3 = x2*cu - y2*su;
    double y3 = x2*su + y2*cu;
    v = new cgVertex (x3 + cen_.givx (), y3 + cen_.givy (), v);

    x2 = x3;
    y2 = y3;
    vernum ++;
  }
}

cgPrim*
cgArc::copy (void) const
{
  cgArc *arc = new cgArc (cen_, r_, a0_, a1_);
  if (cxt_)
    arc->copyCgCxt (*cxt_);
  return arc;
}

void
cgArc::draw (const cgScene* s) const
{
  assert (cxt_);
  cgVertex *p = 0;

  int i = 0;
  for (p = v; p->givnext (); p = p->givnext()) {
    cgLine tline (*p, *p->givnext ());
    tline.setCgCxt (*cxt_);
    tline.draw(s);
    i++;
  }
}

// list of vertices are in the reverse order of the insertion
// ==========================================================
// device coordinates has Y axis points downward
// CCW in device == CW in window
void
cgArc::update (const cgScene* s)
{
  cgPoint center = cen_;
  // convert center of circle to device coordinates
  s->givwv().win_dev (center);
  // convert radius in to device coordinates
  cgPoint end;
  end.setx (cen_.givx () + r_);
  end.sety (cen_.givy ());
  s->givwv().win_dev (end);
  double radius = end.givx() - center.givx ();

  // create all vertices
  cgVertex *p = v;
  double x1 = radius;
  double y1 = 0.0;

  // rotate to the first point
  double deg = PI/180*a0_ + (vernum - 1)*PI/180*CG_ARC_DEG_INTERVAL;
  
  // rotate CCW in window == rotate CW in device
  double x2 = x1*cos (deg) + y1*sin (deg);
  double y2 = -x1*sin (deg) + y1*cos (deg);
  // device point
  cgPoint devp, winp;
  devp.setx (x2 + center.givx ());
  devp.sety (y2 + center.givy ());
  // convert to window coordinate
  winp = s->givwv().dev_win (devp);
  // update the first vertex
  p->setx (winp.givx());
  p->sety (winp.givy());
  p = p->givnext ();

  double su = sin (PI/180*CG_ARC_DEG_INTERVAL);
  double cu = cos (PI/180*CG_ARC_DEG_INTERVAL);

  for (int i = 0; i < vernum - 1; i++) {
    // rotate CW in window == CCW in device
    double x3 = x2*cu - y2*su;
    double y3 = x2*su + y2*cu;

    devp.setx (x3 + center.givx ());
    devp.sety (y3 + center.givy ());
    cgPoint winp = s->givwv().dev_win (devp);
    // update the first vertex
    p->setx (winp.givx());
    p->sety (winp.givy());
    // get next vertex
    p = p->givnext ();
    x2 = x3;
    y2 = y3;
  }
}
