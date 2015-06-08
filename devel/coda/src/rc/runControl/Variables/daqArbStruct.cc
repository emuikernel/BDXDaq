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
//      Arbitrary data structure that can be transpored daqNetData
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: daqArbStruct.cc,v $
//   Revision 1.1.1.1  1996/10/11 13:39:30  chen
//   run control source
//
//
#include <stdio.h>
#include <string.h>
#include "daqArbStruct.h"

daqArbStruct::daqArbStruct (void)
{
#ifdef _TRACE_OBJECTS
  printf ("Create daqArbStruct Class Object\n");
#endif
}

daqArbStruct::~daqArbStruct (void)
{
#ifdef _TRACE_OBJECTS
  printf ("Delete daqArbStruct Class Object\n");
#endif
}

daqArbStruct*
daqArbStruct::dup (void)
{
  daqArbStruct* newstr = new daqArbStruct ();
  return newstr;
}

size_t
daqArbStruct::size (void)
{
  return 0;
}

long
daqArbStruct::id (void)
{
  return 0;
}

void
daqArbStruct::encode (char* buffer, size_t &bufsize)
{
  bufsize = 0;
}

void
daqArbStruct::decode (char* buffer, size_t bufsize)
{
  // empty
}

