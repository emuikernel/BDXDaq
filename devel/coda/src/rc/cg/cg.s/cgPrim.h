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
//      Primitive Drawing Shape (Base Class)
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: cgPrim.h,v $
//   Revision 1.1.1.1  1996/10/10 19:38:04  chen
//   CODA OO-graphics library for Xt
//
//
#ifndef _CG_PRIM_T
#define _CG_PRIM_T

#include <cgPattern.h>
#include <cgPoint.h>
#include <cgDispDev.h>
#include <cgContext.h>

class cgScene;

class cgPrim                // base class for all drawable shapes
{
public:
  // destructor
  virtual ~cgPrim (void);

  virtual cgPrim* copy (void) const { return 0; }
  virtual void attach (cgScene* s);
  virtual void detach (cgScene* s);
  virtual void update (const cgScene* )     {}; // change object shape
  virtual void draw  (const cgScene*) const {}
  virtual void erase (const cgScene*) const;
  virtual void solid (const cgScene*) const {}
  virtual void hatch (const cgScene*,int) const {}
  virtual void pattern(const cgScene*,const cgPattern&) const {}
  virtual cgPrim& rot (double) { return *this; }       // 2D-rotation 
  virtual cgPrim& rotx(double) { return *this; }       // 3D-rotation
  virtual cgPrim& roty(double) { return *this; }
  virtual cgPrim& rotz(double) { return *this; }
  virtual cgPrim& translate(const cgPoint&) { return *this; }
  virtual cgPrim& viewtrans(
      const cgPoint&,const cgPoint&,const cgPoint&) { return *this; }

  cgPrim* givnext() { return next; }
  void    setnext(cgPrim* p) { next = p; }

  // set and retrieve all graphics context
  // create new context
  virtual void       createCgCxt (cgDispDev& disp);
  // set context to an existing context
  virtual void       setCgCxt    (cgContext& cxt);
  // create new context that is a copy of existing one
  virtual void       copyCgCxt (cgContext& cxt);
  // return context
  virtual cgContext* cgCxt (void) const;

  // check a single point inside the region of this object.
  // return 0: do not care, return 1: yes
  virtual int        contains (const cgPoint&) {return 0;}

  // this object being selected or action duto button press
  virtual void       select (const cgScene*) {}
  virtual void       action (const cgScene*) {}
  
  // class name
  virtual const char* className (void) const {return "cgPrim";}

protected:
  // deny direct instantiation of cgPrim
  cgPrim (cgPrim* next = 0);

  cgPrim* next ;            // this makes primitives linkable in list
  cgContext* cxt_;

private:
  // deny access
  cgPrim (const cgPrim& p);
  cgPrim& operator = (const cgPrim& p);
} ;
#endif
