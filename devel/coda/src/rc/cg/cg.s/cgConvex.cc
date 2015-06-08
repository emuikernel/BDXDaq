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
//      Implementation of 2D convex
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: cgConvex.cc,v $
//   Revision 1.2  1997/11/24 22:02:09  rwm
//   Eliminated CC warning by renaming vars.
//
//   Revision 1.1.1.1  1996/10/10 19:38:03  chen
//   CODA OO-graphics library for Xt
//
//
#include <stdio.h>
#include <math.h>

#include <cgElist.h>
#include <cgConst.h>
#include <cgXdrawing.h>
#include "cgConvex.h"

cgConvex::cgConvex (void)
:cgPrim (), v (0), vernum (0), cen (0,0)
{
  // empty
}

cgConvex::cgConvex (cgPoint* points, int num)
:cgPrim (), v (0), vernum (0), cen (0, 0)
{
  for (int i = 0; i < num; i++)
    *this += cgVertex (points[i]);
}

cgConvex::~cgConvex()
{
  delete v;
}

cgPrim*
cgConvex::copy() const
{
  cgConvex* cx = new cgConvex ();
  for (cgVertex* p = v; p; p = p->givnext())
    *cx += *p;
  if (cxt_)
    cx->copyCgCxt (*cxt_);
  return cx;
}

void 
cgConvex::operator = (const cgConvex& a)
{
  delete v;
  v = 0;
  vernum = 0;
  cen.set (cgPoint(0,0)); 
  for (cgVertex* p = a.v; p; p = p->givnext())
    *this += *p;
}     

void 
cgConvex::operator += (const cgVertex& a)
{
  cgVertex* temp = v;             // adds in copy of vertex
  v = a.copy();                      // updates vert_num and centroid
  v->setnext(temp);
  cen = (cen*vernum + *v)/(vernum + 1);
  vernum++;
}

//**************************** draw() ********************************
void cgConvex::draw (const cgScene* scene) const
{
  assert (cxt_);
  if (vernum < 3) return;
  
  cgVertex* clipped = scene->givwv().clipvl(v) ;
  if (!clipped) return;
  
  cgVertex *p = 0;
  for (p = clipped; p; p = p->givnext())
    scene->givwv().win_dev(*p);

  for (p = clipped; p->givnext(); p = p->givnext())
    draw_line(cxt_, (int)(p->givx()),(int)(p->givy()),
	      (int)(p->givnext()->givx()),(int)(p->givnext()->givy()));
  draw_line(cxt_, (int)(p->givx()), (int)(p->givy()),
	    (int)(clipped->givx()), (int)(clipped->givy()));

  delete clipped;
}

//***************************** solid() ******************************
void cgConvex::solid(const cgScene* scene) const
{
  if (vernum < 3) return;
  cgVertex* clipped = scene->givwv().clipvl(v) ;
  if (!clipped) return;

  for (cgVertex* p = clipped; p; p = p->givnext())
    scene->givwv().win_dev(*p);

  cgElist* elist = make_elist(clipped) ;
  assert (cxt_);
  elist->setCgCxt (*cxt_);
  elist->fill(&cgEdge::solidscan);
  delete elist;
  delete clipped;
}

void cgConvex::hatch(const cgScene* scene,int distance) const
{

  if (vernum < 3) return;

  extern void setdist(int) ;
  setdist(distance);
  
  cgVertex* clipped = scene->givwv().clipvl(v) ;
  if (!clipped) return;
  for (cgVertex* p = clipped; p; p = p->givnext())
    scene->givwv().win_dev(*p);

  cgElist* elist = make_elist(clipped);
  assert (cxt_);
  elist->setCgCxt (*cxt_);
  elist->fill(&cgEdge::hatchscan);
  delete elist;
  delete clipped;
}

void cgConvex::pattern(const cgScene* scene,const cgPattern& p) const
{
  if (vernum < 3) return;

  extern void setpattern(const cgPattern&) ;
  setpattern(p);

  cgVertex* clipped = scene->givwv().clipvl(v) ;
  if (!clipped) return;

  for (cgVertex* pp = clipped; pp; pp = pp->givnext())
    scene->givwv().win_dev(*pp);

  cgElist* elist = make_elist(clipped) ;
  assert (cxt_);
  elist->setCgCxt (*cxt_);
  elist->fill(&cgEdge::patternscan);
  delete elist;
  delete clipped;
}

//************************ transformations ***************************
cgPrim& cgConvex::rot(double deg)                    // deg in degrees
{
  cgPoint center(this->cen) ;
  translate(-center);
  deg *= DEG2RAD;
  double cx = cos(deg) ;                       // this is faster than 
  double sx = sin(deg) ;                    // calling cgPoint::rot()
  
  for (cgVertex* p = v; p; p = p->givnext())
    p->set(cgPoint(p->givx()*cx - p->givy()*sx,
		      p->givx()*sx + p->givy()*cx ));
  translate(center);
  return *this;
}

cgPrim& cgConvex::translate(const cgPoint& point)
{
  for (cgVertex* p = v; p; p = p->givnext())
    *p += point;
  cen += point;
  return *this;
}

cgElist* cgConvex::make_elist(cgVertex* vv) const
{
  return new cgElist(vv);
}
