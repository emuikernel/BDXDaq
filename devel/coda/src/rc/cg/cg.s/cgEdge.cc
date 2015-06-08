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
//      Implementation of 2D edge class
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: cgEdge.cc,v $
//   Revision 1.1.1.1  1996/10/10 19:38:04  chen
//   CODA OO-graphics library for Xt
//
//
#include <stdio.h>

#include <math.h>

#include <cgVertex.h>
#include <cgPattern.h>
#include <cgXdrawing.h>

#include "cgEdge.h"

cgEdge::cgEdge(const cgPoint& a,const cgPoint& b)
:ymax (a.givy()), ymin (b.givy()) ,
 dx(-(a.givx() - b.givx())/(ymax - ymin)) ,
 x(a.givx() - dx*(int(ymax + 1) - ymax)) ,
 next(0), cxt_ (0)
{
  // empty
}

void cgEdge::update() { x += dx; }

//******************Solid Scan****************************************
static void sline (cgContext* cxt, short x1,short x2,short scan)
{
  draw_line (cxt, x1, scan, x2, scan);
}

void cgEdge::solidscan(double scan)
{
  assert (cxt_);
  sline (cxt_, short(x),short(next->x),short(scan));
}

//**************************** hatchscan ******************************
static int dist  ,                       // distance between hatchlines
tdist ,
hatch ;

void setdist(int distance)
{
  dist  = distance ;
  tdist = 2*dist ;
}

static void hline (cgContext* cxt, int x1,int x2,int scan)
{
  int term  = (x1-hatch+tdist-1)/tdist*tdist ,
  si    = x1-term+hatch-0.5 < 0 ? -1 : 1 ,
  pos   = term + si*hatch                ,
  step  = dist + si*(dist - 2*hatch)     ;

  while (pos <= x2)
    {
      draw_point (cxt, pos, scan);
      pos += step;
      step = tdist - step;
    }
}

void cgEdge::hatchscan (double scan)
{
//Sergey: add 'double' inside fabs
  hatch = int(dist - fabs( double(int(scan)%tdist - dist) ) );
  assert (cxt_);
  hline(cxt_, int(x),int(next->x),int(scan));
}

//************************** patternscan ******************************
static cgPattern p ;

void setpattern(const  cgPattern& pat)
{
  p = pat;                                       // set global pattern
}


static void pline (cgContext* cxt, int x1,int x2,int scan)
{
  for (int k = x1; k <= x2; k++) {
    cxt->setForeground (p.item(scan, k));
    draw_point (cxt, k, scan);
  }
}

void cgEdge::patternscan (double scan)
{
  assert (cxt_);
  pline(cxt_, int(x+0.5),int(next->x+0.5),int(scan));
}
