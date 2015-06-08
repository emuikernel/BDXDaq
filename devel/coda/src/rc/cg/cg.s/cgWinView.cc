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
//      Implementation of window and view port
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: cgWinView.cc,v $
//   Revision 1.1.1.1  1996/10/10 19:38:04  chen
//   CODA OO-graphics library for Xt
//
//
#include <stdio.h>
#include <stdlib.h>

#include <cgPoint.h>
#include <cgLine.h>
#include "cgWinView.h"

//====================================================================
//      Implementaion of cgWindow
//====================================================================
cgWindow::cgWindow (double ll,double rr,double bb,double tt) 
:l(ll) , r(rr) , b(bb) , t(tt)
{
  if (l == r || b == t) {
    printf("impossible window size\n");
    exit(0);
  }
}

void
cgWindow::setXmin (double xmin)
{
  l = xmin;
}

void
cgWindow::setXmax (double xmax)
{
  r = xmax;
}

void
cgWindow::setYmin (double ymin)
{
  b = ymin;
}

void
cgWindow::setYmax (double ymax)
{
  t = ymax;
}


cgViewport::cgViewport (double ll,double rr,double bb,double tt)
:l(ll) , r(rr) , b(bb) , t(tt), width_ (0), height_ (0)
{
  if (  l <  0 || r >  1 || b < 0 || t > 1
      || r <= l || t <= b) {
    printf("impossible viewport size\n");
    exit(0);
  }
}


void
cgViewport::devWidth (double width)
{
  width_ = width;
}

double
cgViewport::devWidth (void) const
{
  return width_;
}

double
cgViewport::devHeight (void) const
{
  return height_;
}

void
cgViewport::devHeight (double height)
{
  height_ = height;
}

cgWinView::cgWinView (cgWindow& w, cgViewport& v)
:win_ (w), port_ (v)
{
  // empty
}

// device-viewport-window transformation
cgPoint 
cgWinView::dev_win (const cgPoint& p) const
{
  double newx =  (double(p.givx())/port_.width_ - port_.l)
    /(port_.r - port_.l) * (win_.r - win_.l) + win_.l;

  double newy =  (1 - double(p.givy())/port_.height_ - port_.b)
    /(port_.t - port_.b) * (win_.t - win_.b) + win_.b;
  return cgPoint (newx, newy);
}

// window-viewport-device transformation
void 
cgWinView::win_dev (cgPoint& p) const
{  
  // new X coordinate 
  double newx = (p.givx () - win_.l)/(win_.r - win_.l)*(port_.r - port_.l)
    + port_.l;
  newx = newx*port_.width_;

  // new Y coordinate
  double newy = 1- ((p.givy() - win_.b)/(win_.t - win_.b)*(port_.t - port_.b)
		    + port_.b);
  newy = newy*port_.height_;

  p.set(cgPoint(newx, newy));
}



//*************** Liang-Barsky straight line clipping ****************
#if 0
// returns 0 if line is rejected
// positive value if not rejected
static double find_t (double p,
		      double q,
		      double& t1,
		      double& t2 ) 
{
  if (p) {
    double r = q/p ;
    if (p < 0)
      return t2 < r ? 0 : t1 < r ? t1 = r : 1 ;
    else 
      return r < t1 ? 0 : r < t2 ? t2 = r : 1 ;
  }
  else 
    return q < 0 ? 0 : 1 ;
}

// Liang-Barsky straight line clipping
cgLine* 
cgWindow::clipln (const cgLine& line) const
{
  double x1 = line.giva().givx() ,
  y1 = line.giva().givy() ,
  x2 = line.givb().givx() ,
  y2 = line.givb().givy() ;

  double t1 = 0 ,
  t2 = 1 ,
  dx = x2 - x1 ,
  dy = y2 - y1 ;

  if (!find_t(-dx,x1-l,t1,t2)) return 0;                   // left  
  if (!find_t( dx,r-x1,t1,t2)) return 0;                   // right  
  if (!find_t(-dy,y1-b,t1,t2)) return 0;                   // bottom 
  if (!find_t( dy,t-y1,t1,t2)) return 0;                   // top   

  if (t1 > 0) {                         // line not outside the window
    x1 += t1*dx;                         // line is clipped at entry
    y1 += t1*dy;
  }
  if (t2 < 1) {
    x2 -= (1-t2)*dx;                      // line is clipped at exit
    y2 -= (1-t2)*dy;
  }
  return new cgLine (x1,y1,x2,y2);
}
#endif

#if 1 
//************* Cohen-Sutherland straight line clipping **************
char 
cgWindow::reg_code (double x,double y) const // region code of x,y 
{
  return (((x<l)<<1 | (r<x))<<1 | (y<b))<<1 | t<y;
}

cgLine* 
cgWindow::clipln (const cgLine& line) const 
{ 
  // region code of point x1,y1
  char rcode1;
  // region code of point x2,y2
  char  rcode2;
  // region code to work with 
  char  rcode;

  double x , y ;
  
  double x1 = line.giva().givx() ,       // endpoints of clipped line
  y1 = line.giva().givy() ,
  x2 = line.givb().givx() ,
  y2 = line.givb().givy() ;

  rcode1 = reg_code(x1,y1);
  rcode2 = reg_code(x2,y2);
  while ((rcode1 | rcode2)                      // trivial acceptance
	 && !(rcode1 & rcode2))                      // trivial rejection
    {
      rcode = rcode1 != 0 ? rcode1 : rcode2;
      if (rcode & 8)                           // bit 3 in region code
	{
	  x = l;
	  y = y1 + (y2-y1)*(l-x1)/(x2-x1);
	}
      else if (rcode & 4)                      // bit 2 in region code
	{
	  x = r;
	  y = y1 + (y2-y1)*(r-x1)/(x2-x1);
	}
      if (rcode & 2)                           // bit 1 in region code
	{
	  x = x1 + (x2-x1)*(b-y1)/(y2-y1);
	  y = b;
	}
      else if (rcode & 1)                      // bit 0 in region code
	{
         x = x1 + (x2-x1)*(t-y1)/(y2-y1);
         y = t;
      }
      if (rcode == rcode1)                  // clipped point was x1,y1
	{
	  x1 = x; y1 = y;
	  rcode1 = reg_code(x1,y1);
	}
      else                                  // clipped point was x2,y2
	{
	  x2 = x; y2 = y;
	  rcode2 = reg_code(x2,y2);
	}
    }
  return  (rcode1 & rcode2) ? 0 : new cgLine (x1,y1,x2,y2);
}
#endif


//********************** vertex list clipping ************************
static cgVertex* v ;
static cgVertex* cur ;

void 
cgWindow::cltop (double x,double y) const           // clip vs top
{
  static double xl , yl ;

  if (y  <= t && t <  yl                     // edge crosses boundary
      ||  yl <  t && t <= y ) 
    {
      cgVertex* temp = v;
      v = cur->copy();              // create vertex copy of same type
      v->setx((x-xl)*(t-y)/(y-yl)+x);
      v->sety(t);
      v->setnext(temp);
   }

   if (y < t || yl == t && y == t) 
   {
      cgVertex* temp = v;
      v = cur->copy();              // create vertex copy of same type
      v->setx(x);
      v->sety(y);
      v->setnext(temp);
   }
   xl = x; yl = y;                                // update last point
}

void
cgWindow::clbot (double x,double y) const        // clip vs bottom
{
   static double xl , yl ;

   if (yl <  b && b <= y                      // edge crosses boundary
       ||  y  <= b && b <  yl) 
     cltop((x-xl)*(b-y)/(y-yl)+x,b);

   if (b < y ||  yl == b && y == b) 
     cltop(x,y);
   xl = x; yl = y;                                // update last point
}

void 
cgWindow::clrit(double x,double y) const         // clip vs right
{
   static double xl , yl ;

   if (x  <= r && r <  xl                     // edge crosses boundary
       ||  xl <  r && r <= x ) 
     clbot(r,(y-yl)*(r-x)/(x-xl)+y);
   if (x < r || xl == r && x == r) 
      clbot(x,y);
   xl = x; yl = y;                                // update last point
}

void 
cgWindow::cllef(double x,double y) const          // clip vs left
{
  static double xl , yl ;

  if (xl <  l && l <= x                      // edge crosses boundary
      ||  x  <= l && l <  xl) 
    clrit(l,(y-yl)*(l-x)/(x-xl)+y);
  if (l < x || xl == l && x == l) 
    clrit(x,y);
  xl = x; yl = y;                                // update last point
}

cgVertex*
cgWindow::clipvl (cgVertex* v) const  
{
  ::v = 0;
  for(cur = v; cur; cur = cur->givnext())
    cllef(cur->givx(),cur->givy());
  delete ::v;
  
  ::v = 0;
  for(cur = v; cur; cur = cur->givnext())
    cllef(cur->givx(),cur->givy());
  
  return ::v;
}

cgVertex*
cgWinView::clipvl (cgVertex* v) const
{
  return win_.clipvl (v);
}

cgLine*
cgWinView::clipln (const cgLine& l) const
{
  return win_.clipln (l);
}

double
cgWinView::xmin (void) const
{
  return win_.l;
}

double
cgWinView::xmax (void) const
{
  return win_.r;
}

double
cgWinView::ymin (void) const
{
  return win_.b;
}

double
cgWinView::ymax (void) const
{
  return win_.t;
}

void
cgWinView::setXmin (double xmin)
{
  win_.setXmin (xmin);
}

void
cgWinView::setXmax (double xmax)
{
  win_.setXmax (xmax);
}

void
cgWinView::setYmin (double ymin)
{
  win_.setYmin (ymin);
}

void
cgWinView::setYmax (double ymax)
{
  win_.setYmax (ymax);
}
