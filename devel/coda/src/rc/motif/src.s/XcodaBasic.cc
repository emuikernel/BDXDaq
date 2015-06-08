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
//	
//	
// Author:  Jie Chen
//       CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: XcodaBasic.cc,v $
//   Revision 1.2  1998/04/08 17:28:49  heyes
//   get in there
//
//   Revision 1.1.1.1  1996/10/10 19:25:00  chen
//   coda motif C++ library
//
//
#include "XcodaBasic.h"
#include <assert.h>
#include <stdio.h>

XcodaBasic::XcodaBasic ( const char *name )
{
#ifdef _TRACE_OBJECTS
  printf("Create XcodaBasic Object \n");
#endif
  _w = NULL;
  assert ( name != NULL );  // Make sure programmers provide name

  _name = new char[strlen(name)+1];
  strcpy (_name, name);
}

XcodaBasic::~XcodaBasic()
{
#ifdef _TRACE_OBJECTS
     printf("Destroy XcodaBasic Object \n");
#endif
    if( _w )
	XtDestroyWidget ( _w );
    delete []_name;
}

void XcodaBasic::manage()
{
    assert ( _w != NULL );
    XtManageChild ( _w );
}

void XcodaBasic::unmanage()
{
    assert ( _w != NULL );
    XtUnmanageChild ( _w );
}
