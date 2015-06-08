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
//      2D glist class
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: cgGlist.h,v $
//   Revision 1.1.1.1  1996/10/10 19:38:03  chen
//   CODA OO-graphics library for Xt
//
//
#ifndef _CG_GLIST_T
#define _CG_GLIST_T

#include <cgElist.h>

// edge list for filling general polygon
class cgGlist : public cgElist 
{
public:
  cgGlist  (cgVertex* v);
  ~cgGlist (void) ;

  void fill(void (cgEdge::*)(double))  ;          // is redefined
protected:
  void swap()   ;                                   // is redefined
  void update() ;                                    // is redefined
} ;
#endif
