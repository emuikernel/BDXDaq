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
//      Implementation of 2D pattern
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: cgPattern.cc,v $
//   Revision 1.1.1.1  1996/10/10 19:38:04  chen
//   CODA OO-graphics library for Xt
//
//
#include "cgPattern.h"

#ifdef _XWIN_24
cgPattern::cgPattern(int r,int c,unsigned long* pat)
:r(r) , c(c) , p(new unsigned long[r*c])
{
  for (int i = 0;i < r*c; i++)
    p[i] = pat[i];
}

cgPattern::~cgPattern() 
{delete p;}

void cgPattern::operator = (const cgPattern& pat)
{
  delete p;
  r = pat.r;
  c = pat.c;
  p = new unsigned long[r*c];
  for (int i = 0;i < r*c; i++)
    p[i] = pat.p[i];
}

#else
#include <string.h>
cgPattern::cgPattern(int rr,int cc,unsigned char* pat) 
: r (rr) , c (cc) , p((unsigned char*)strcpy(new char[r*c],(char *)pat)) {}

cgPattern::~cgPattern() {delete p;}

void cgPattern::operator = (const cgPattern& pat)
{
  if (this != &pat) {
    delete p;
    r = pat.r;
    c = pat.c;
    p = (unsigned char*)strcpy(new char[r*c],(char *)pat.p);
  }
}

#endif
