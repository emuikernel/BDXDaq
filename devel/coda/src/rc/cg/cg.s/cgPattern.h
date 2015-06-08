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
//      2D pattern
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: cgPattern.h,v $
//   Revision 1.1.1.1  1996/10/10 19:38:03  chen
//   CODA OO-graphics library for Xt
//
//
#ifndef _CG_PATTERN_T
#define _CG_PATTERN_T

#ifdef _XWIN_24
class cgPattern
{
public:
  cgPattern(int,int,unsigned long*) ;
  cgPattern() : p(0) {}
  ~cgPattern() ;

  unsigned long* operator [] (int i) { return p + i*c; }
  void operator = (const cgPattern&) ;
  unsigned long
    item(int r,int c) { return (*this)[r%this->r][c%this->c]; }
private:
   unsigned long* p ;
   int r ;                                           // number of rows
   int c ;                                        // number of columns
} ;

#else
class cgPattern
{
public:
  cgPattern(int,int,unsigned char*) ;
  cgPattern() : p(0) {}
  ~cgPattern() ;

  unsigned char* operator [] (int i) { return p + i*c; }
  void operator = (const cgPattern&) ;
  char item(int rr,int cc) { return (*this)[rr%this->r][cc%this->c]; }

private:
  unsigned char* p ;
  int r ;                                           // number of rows
  int c ;                                        // number of columns
} ;
#endif

#endif
