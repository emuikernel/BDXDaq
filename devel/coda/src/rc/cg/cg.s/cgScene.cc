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
//      Implementation of 2D view scene
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: cgScene.cc,v $
//   Revision 1.1.1.1  1996/10/10 19:38:04  chen
//   CODA OO-graphics library for Xt
//
//
#include <stdio.h>

#include "cgScene.h"

cgScene::cgScene (double xmin, double xmax, double ymin, double ymax,
		  double vxmin, double vxmax, double vymin, double vymax,
		  cgDispDev& disp)
:prim (0)
{
  window = new cgWindow (xmin, xmax, ymin, ymax);
  port = new cgViewport (vxmin, vxmax, vymin, vymax);
  port->devWidth (disp.width ());
  port->devHeight (disp.height ());
  wv = new cgWinView (*window, *port);
}

cgScene::cgScene (cgWinView& wview)
:window (0), port (0), wv (&wview), prim (0)
{
  // empty
}

cgScene::~cgScene (void)
{
  if (prim)
    delete prim;
  delete wv;
  if (port)
    delete port;
  if (window)
    delete window;
}

void
cgScene::operator += (const cgPrim& p)
{
   cgPrim* temp = p.copy() ;
   temp->setnext(prim);
   prim = temp;
}

void
cgScene::addPrim (cgPrim* p)
{
  p->setnext (prim);
  prim = p;
}

void
cgScene::removePrim (cgPrim* rem)
{
  int found = 0;
  cgPrim* p;
  cgPrim* q;

  if (rem == prim) { // header
    prim = prim->givnext ();
  }
  else {
    q = prim;
    p = prim->givnext ();
    while (p) {
      if (p == rem) {
	q->setnext (p->givnext());
	break;
      }
      q = p;
      p = p->givnext ();
    }
  }
}

void cgScene::draw () const
{
   for (cgPrim* p = prim; p; p = p->givnext()) {
     // some of the objects wants to change before drawing
     p->update (this);
     p->draw (this);
   }
}

void cgScene::erase () const
{
   for (cgPrim* p = prim; p; p = p->givnext())
     p->erase (this);
}

void cgScene::solid() const
{
   for (cgPrim* p = prim; p; p = p->givnext())
      p->solid(this);
}

void cgScene::hatch(int dist) const
{
  for (cgPrim* p = prim; p; p = p->givnext())
    p->hatch(this,dist);
}

void cgScene::pattern(const cgPattern& pat) const
{
  for (cgPrim* p = prim; p; p = p->givnext())
    p->pattern(this,pat);
}

cgScene& cgScene::rot(double x)                        // x in degrees
{
   for (cgPrim* p = prim; p; p = p->givnext())
     p->rot(x);

   return *this;
}

cgScene& cgScene::translate(const cgPoint& point)
{
   for (cgPrim* p = prim; p; p = p->givnext())
      p->translate(point);
   return *this;
}

void
cgScene::devWidth (double width)
{
  if (port) 
    port->devWidth (width);
}

void
cgScene::devHeight (double height)
{
  if (port)
    port->devHeight (height);
}

double
cgScene::devWidth (void) const
{
  if (port)
    return port->devWidth ();
  else
    return 0;
}

double
cgScene::devHeight (void) const
{
  if (port)
    return port->devHeight ();
  else
    return 0;
}

cgPrim*
cgScene::contains (const cgPoint& point)
{
  for (cgPrim* p = prim; p; p = p->givnext())
    if (p->contains (point))
      return p;
  return 0;
}

