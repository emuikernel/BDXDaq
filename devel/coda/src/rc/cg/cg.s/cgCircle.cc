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
//      Implementation of 2D circle
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: cgCircle.cc,v $
//   Revision 1.1.1.1  1996/10/10 19:38:03  chen
//   CODA OO-graphics library for Xt
//
//
#include <math.h>

#include <cgLine.h>
#include <cgConst.h>
#include "cgCircle.h"


cgCircle::cgCircle (cgPoint cc, double rr)
:cgConvex (), r (rr), c (cc)
{
  double su = sin(2*PI/90);
  double cu = cos(2*PI/90);

  double x1 = r ;
  double y1 = 0 ;
  cen = c;
  v = new cgVertex (x1+c.givx(),y1+c.givy(),v);
  vernum = 1;

  for (int i = 1; i < 90; i++)             // no use of += operator!
    {
      double x2 = x1*cu - y1*su;
      double y2 = x1*su + y1*cu;
      v = new cgVertex (x2+c.givx(),y2+c.givy(),v);

      x1 = x2;
      y1 = y2;
      vernum ++;
    }
}

cgPrim* 
cgCircle::copy (void) const
{
  cgCircle* ci = new cgCircle (c, r);
  if (cxt_)
    ci->copyCgCxt (*cxt_);
  return ci;
}

void
cgCircle::draw (const cgScene* scene) const
{
  assert (cxt_);

  cgVertex *p;
  for (p = v; p->givnext(); p = p->givnext()) {  
    cgLine tline (*p, *p->givnext ());
    tline.setCgCxt (*cxt_);
    tline.draw(scene);
  }
  cgLine tl (*p, *v);
  tl.setCgCxt (*cxt_);
  tl.draw(scene);
}

// update all vertices because the circle must be a circle on the 
// display device. device has Y axis pointing downward.
// list of vertices has reverse order 
void
cgCircle::update (const cgScene* s)
{
  cgPoint center = c;
  // convert center of circle to device coordinates
  s->givwv().win_dev (center);
  // convert radius in to device coordinates
  cgPoint end;
  end.setx (c.givx () + r);
  end.sety (c.givy ());
  s->givwv().win_dev (end);
  double radius = end.givx() - center.givx ();

  // create all vertics
  double su = sin(2*PI/90);
  double cu = cos(2*PI/90);
  
  double x1 = radius ;
  double y1 = 0 ;

  cgVertex *p = v;
  cgPoint devp;
  for (int i = 1; i < 90; i++)             // no use of += operator!
    {
      // CCW in window == CW in device
      double x2 = x1*cu + y1*su;
      double y2 = -x1*su + y1*cu;

      devp.setx (x2 + center.givx ());
      devp.sety (y2 + center.givy ());
      // convert to window coordinates
      cgPoint winp = s->givwv().dev_win (devp);
      // update vertex x and y coordinates
      p->setx (winp.givx());
      p->sety (winp.givy());
      p = p->givnext ();
      // remeber the previous points
      x1 = x2;
      y1 = y2;
    }
}
