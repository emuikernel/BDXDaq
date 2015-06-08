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
//      Gauge Class 
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: cgGauge.h,v $
//   Revision 1.1.1.1  1996/10/10 19:38:04  chen
//   CODA OO-graphics library for Xt
//
//
#ifndef _CG_GAUGE_H
#define _CG_GAUGE_H

#include <cgRec.h>

class cgGauge: public cgPrim
{
public:
  // constructor and destructor
  cgGauge (double xmin, double xmax, double ymin, double ymax,
	   int showBorder = 1);
  virtual ~cgGauge (void);

  virtual cgPrim*     copy         (void) const;
  virtual void        draw         (const cgScene* s) const;
  virtual void        erase        (const cgScene* s) const;

  // operations
  void    setValue   (const cgScene*s, double x);
  double  getValue   (void) const;
  void    unmapIndicator (const cgScene* s);
  void    mapIndicator   (const cgScene* s);

  virtual void        createCgCxt  (cgDispDev& disp);
  virtual void        copyCgCxt    (cgContext& cxt);
  virtual void        setCgCxt     (cgContext& cxt);

protected:
  // create all internal stuff
  void createInternalPrims (void);

private:

  cgRec*       holder_;
  cgRec*       mover_;
  double       xmin_, xmax_, ymin_, ymax_;
  double       value_;
  // flag to tell whether indicator is mapped or not
  int          mapped_;
  int          showBorder_;
};
#endif
