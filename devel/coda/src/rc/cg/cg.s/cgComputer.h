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
//      Simple 2D Graphical Representation of a computer
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: cgComputer.h,v $
//   Revision 1.1.1.1  1996/10/10 19:38:04  chen
//   CODA OO-graphics library for Xt
//
//
#ifndef _CG_COMPUTER_H
#define _CG_COMPUTER_H

#include <stdio.h>
#include <string.h>
#include <assert.h>

#include <cgPrim.h>
#include <cgRec.h>
#include <cgLine.h>

class cgComputer: public cgPrim
{
public:
  // constructor and destructor
  // construct a computer by bottom left x and y + total width = height
  cgComputer  (double blx, double bly, double width);
  virtual ~cgComputer (void);

  // inherited operations
  virtual cgPrim*    copy         (void) const;
  virtual void       draw         (const cgScene* s) const;
  virtual void       erase        (const cgScene* s) const;
  virtual cgPrim&    translate    (const cgPoint& p);
  virtual void       createCgCxt  (cgDispDev& disp);
  virtual void       setCgCxt     (cgContext& cxt);
  virtual void       copyCgCxt    (cgContext& cxt);

  // set pixel for computer body color
  void               setComputerColor (Pixel color);
  void               setComputerColor (char* name);
  // set foreground color
  void               setComputerFgColor (Pixel color);
  void               setComputerFgColor (char* name);

  // class name
  virtual const char* className (void) const {return "cgComputer";}

protected:
  // creation of all internal components
  void createInternalPrims (void);
  // propagate cgContext to all components
  void propagateCgCxt      (void);

  // real internal components
  cgRec*     monitor_;
  cgRec*     screen_;
  cgRec*     adjuster_;
  cgRec*     supporter_;
  cgRec*     console_;
  cgLine*    discDr_;
  cgLine**   ducts_;

  // Coordinates
  double x_, y_;
  double width_, height_;
  // Pixel for computer body color
  Pixel color_;
  // Pixel for computer foreground color
  Pixel fg_;

private:
  // deny access to copy and assignement operations


  //Sergey cgCompueter (const cgComputer& comp);
  void cgCompueter (const cgComputer& comp);


  cgComputer& operator = (const cgComputer& comp);
};
#endif


  
