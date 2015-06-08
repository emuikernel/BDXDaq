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
//      Simple VME crate
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: cgVmeCrate.h,v $
//   Revision 1.1.1.1  1996/10/10 19:38:04  chen
//   CODA OO-graphics library for Xt
//
//
#ifndef _CG_VME_CRATE_H
#define _CG_VME_CRATE_H

#include <stdio.h>
#include <string.h>
#include <assert.h>

#include <cgPrim.h>
#include <cgRec.h>
#include <cgPolygon.h>
#include <cgCircle.h>
#include <cgLine.h>

class cgVmeCrate: public cgPrim
{
public:
  // constructor and destructor
  // constructed by bottom left coordinate and height (width = 2* height)
  // and number of slots
  cgVmeCrate          (double blx, double bly, 
		       double height,int numSlots);
  virtual ~cgVmeCrate (void);

  // inherited operations
  virtual cgPrim* copy    (void) const;
  virtual void    draw    (const cgScene* s) const;
  virtual void    erase   (const cgScene* s) const;
  virtual cgPrim& translate (const cgPoint& point);
  virtual void    createCgCxt (cgDispDev& disp);
  virtual void    setCgCxt    (cgContext& cxt);
  virtual void    copyCgCxt   (cgContext& cxt);

  // set crate body color
  void    setCrateColor       (Pixel color);
  void    setCrateColor       (char* name);
  // set foreground color for crate
  void    setCrateFgColor     (Pixel color);
  void    setCrateFgColor     (char* name);

  // class name
  virtual const char* className (void) const {return "cgVmeCrate";}

protected:
  // create all internal objects
  void createInternalPrims (void);
  // propagate cgContext to all components
  void propagateCgCxt (void);
  // real internal presentation
  cgRec*       frame_;
  cgRec*       fan_;
  cgRec*       switch_;
  cgLine**     lines_;
  int          numSlots_;
  cgCircle**   lights_;
  int          numLights_;
  // coordinates
  double      blx_, bly_;
  double      width_, height_;
  // crate background color
  Pixel       crateColor_;
  // crate foregound color
  Pixel       crateFg_;

private:
  // deny access to copy and assignment
  cgVmeCrate (const cgVmeCrate& crate);
  cgVmeCrate& operator = (const cgVmeCrate& crate);
};
#endif
