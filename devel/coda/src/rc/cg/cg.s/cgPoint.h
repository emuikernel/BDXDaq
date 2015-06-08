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
//      2d point class
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: cgPoint.h,v $
//   Revision 1.1.1.1  1996/10/10 19:38:03  chen
//   CODA OO-graphics library for Xt
//
//
#ifndef _CG_POINT_T
#define _CG_POINT_T

class cgPoint         // point in 2D space
{
public:
   cgPoint (void) ;
   cgPoint (double,double) ;
   cgPoint (const cgPoint&) ;
   cgPoint& operator = (const cgPoint&);

   cgPoint   operator + (const cgPoint&) const ;
   cgPoint   operator - (const cgPoint&) const ;
   cgPoint   operator * (double) const ;
   cgPoint   operator / (double) const ;
   cgPoint&  operator+= (const cgPoint&) ;

   cgPoint&  operator-= (const cgPoint&) ;
   cgPoint   operator - () const ;

   double givx() const { return x;}
   double givy() const { return y;}
   virtual double givz() const { return 0; }
   virtual void set(const cgPoint& p) { x = p.x; y = p.y; }
   void setx(double xx) { cgPoint::x = xx;}
   void sety(double yy) { cgPoint::y = yy;}
   cgPoint& rot(double) ;

protected: 
   double x , y ;
} ;

cgPoint operator *(double,const cgPoint&) ;
#endif
