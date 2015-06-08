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
//      2D View Scene
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: cgScene.h,v $
//   Revision 1.1.1.1  1996/10/10 19:38:03  chen
//   CODA OO-graphics library for Xt
//
//
#ifndef _CG_SCENE_T
#define _CG_SCENE_T

#include <cgWinView.h>
#include <cgPattern.h>
#include <cgPrim.h>

class cgScene               // container class for drawable primitives
{
public:
  cgScene  (double xmin, double xmax, double ymin, double ymax,
	    double vxmin, double vxmax, double vymin, double vymax,
	    cgDispDev& disp);
  cgScene  (cgWinView& wview);
  virtual ~cgScene (void);

  void operator += (const cgPrim&) ;   // add primitive to scene
  virtual void draw() const ;          // draw all primitives in scene
  virtual void erase () const;         // erase all
  virtual void solid() const ;         // solid fill all primitives in scene
  void hatch(int) const ;              // hatch all primitives in scene
  void pattern(const cgPattern&) const ;  // pattern all primitives
  cgScene& rot(double) ;                  // 2D rotate all primitives
  cgScene& translate(const cgPoint&) ;    // translate all primitives

  // return an object that contains point 'point'
  cgPrim*  contains (const cgPoint& point);

  // some operations for cgPrim, cgPrim must be non local
  void    addPrim (cgPrim* p);
  // removed item is still valid, caller can free it after the call
  void    removePrim (cgPrim* p);
  
  cgPrim* givprim() const { return prim; }
  const cgWinView& givwv() const { return *wv; } 

  // set display device with and height
  virtual void devWidth  (double width);
  virtual void devHeight (double height);
  // get display device width and height
  virtual double devWidth  (void) const;
  virtual double devHeight (void) const;
  

protected:
  cgPrim*     prim ;
  cgWindow*   window;
  cgViewport* port;
  cgWinView*  wv ;
} ;

#endif
