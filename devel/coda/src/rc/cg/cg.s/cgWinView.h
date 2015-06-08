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
//      Window and View port Class
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: cgWinView.h,v $
//   Revision 1.1.1.1  1996/10/10 19:38:04  chen
//   CODA OO-graphics library for Xt
//
//
#ifndef _CG_WINVIEW_T
#define _CG_WINVIEW_T

#include <cgVertex.h>

class cgLine ;
class cgWinView;

class cgWindow
{
public:

  cgWindow (double xmin,double xmax,double ymin,double ymax) ;

  virtual cgVertex* clipvl(cgVertex*)     const ; // clip vertex list
  virtual cgLine*   clipln(const cgLine&) const ; // clip straight line

  // reset boundary
  virtual void setXmin (double xmin);
  virtual void setXmax (double xmax);
  virtual void setYmin (double ymin);
  virtual void setYmax (double ymax);

protected:
  double l , r , b , t ;                         // world coordinates

private:
  void cltop(double,double) const ;           // for polygon clipping
  void clbot(double,double) const ;
  void clrit(double,double) const ;
  void cllef(double,double) const ;
  char reg_code(double,double) const ;           // for line clipping
  friend class cgWinView;
} ;


class cgViewport
{
public:
  cgViewport (double=0,double=1,double=0,double=1);

  virtual void    devWidth      (double width);
  virtual double  devWidth      (void) const;
  virtual void    devHeight     (double height);
  virtual double  devHeight     (void) const;

protected: 
  double l , r , b , t ;             // normalized device coordinates
  double width_, height_;
  friend class cgWinView;
} ;

class cgWinView
{
public:
  cgWinView (cgWindow& win, cgViewport& port) ;

  cgPoint       dev_win (const cgPoint&) const ;
  virtual void  win_dev (cgPoint&) const ;

  // return window boundary
  virtual double xmin (void) const;
  virtual double xmax (void) const;
  virtual double ymin (void) const;
  virtual double ymax (void) const;

  // set window boundary
  virtual void   setXmin (double xmin);
  virtual void   setXmax (double xmax);
  virtual void   setYmin (double ymin);
  virtual void   setYmax (double ymax);

  // clip operation
  virtual cgVertex* clipvl(cgVertex*)     const ; // clip vertex list
  virtual cgLine*   clipln(const cgLine&) const ; // clip straight line

  virtual double  givd () const { return 0; }
  virtual cgPoint givcen () const { return cgPoint(0,0); }

protected:
  cgWindow&   win_;
  cgViewport& port_;
};
#endif
