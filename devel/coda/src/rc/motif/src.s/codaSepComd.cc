//-----------------------------------------------------------------------------
// Copyright (c) 1991,1992 Southeastern Universities Research Association,
//                         Continuous Electron Beam Accelerator Facility
//
// This software was developed under a United States Government license
// described in the NOTICE file included as part of this distribution.
//
// CEBAF Data Acquisition Group, 12000 Jefferson Ave., Newport News, VA 23606
// Email: coda@cebaf.gov  Tel: (804) 249-7101  Fax: (804) 249-7363
//-----------------------------------------------------------------------------
// 
// Description:
//	 Separator Widget class for menubar only
//	
// Author:  Jie Chen
//       CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: codaSepComd.cc,v $
//   Revision 1.2  1998/04/08 17:29:53  heyes
//   get in there
//
//   Revision 1.1.1.1  1996/10/10 19:24:57  chen
//   coda motif C++ library
//
//
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include <XcodaSepInterface.h>
#include <codaSepComd.h>

class codaComdXInterface;

codaSepComd::codaSepComd(char *name, int active):codaComd(name, active)
{
#ifdef _TRACE_OBJECTS
  printf("Create codaSepComd object \n");
#endif
  sep = 0;
  _hasUndo = 0;
}

void codaSepComd::createXInterface(Widget parent)
{
  sep = new XcodaSepInterface (parent, this);
  // creation part alreay register itself to command object
  sep->init();
}

codaSepComd::~codaSepComd()
{
#ifdef _TRACE_OBJECTS
  printf("Destroy codaSepComd object \n");
#endif
  if (sep != NULL)
    delete sep;
}

void codaSepComd::doit()
{
  //empty
}

void codaSepComd::undoit()
{
  // empty
}

