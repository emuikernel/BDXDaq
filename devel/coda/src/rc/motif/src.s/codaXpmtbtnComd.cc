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
//	Coda Xpm Toggle Button Command header file
//	
// Author:  Jie Chen
//       CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: codaXpmtbtnComd.cc,v $
//   Revision 1.2  1998/04/08 17:29:57  heyes
//   get in there
//
//   Revision 1.1.1.1  1996/10/10 19:24:57  chen
//   coda motif C++ library
//
//
#include <stdio.h>
#include <string.h>
#include <codaXpmtbtnComd.h>
#include <XcodaXpmtbtnInterface.h>

codaXpmtbtnComd::codaXpmtbtnComd(char *name, char **data, 
				 int active, int state)
:codaTbtnComd(name, active, state)
{
#ifdef _TRACE_OBJECTS
  printf("Create codaXpmtbtnComd Object \n");
#endif
  _initState = state;
  _xpm_data = data;
  _bgsymbol = 0;
  _sel_xpm = 0;
}

codaXpmtbtnComd::codaXpmtbtnComd(char *name, char **data, 
				 int active, int notoggle, int state)
:codaTbtnComd(name, active, notoggle, state)
{
#ifdef _TRACE_OBJECTS
  printf("Create codaXpmtbtnComd Object \n");
#endif
  _initState = state;
  _notoggle = notoggle;
  _xpm_data = data;
  _bgsymbol = 0;
  _sel_xpm = 0;
}

codaXpmtbtnComd::codaXpmtbtnComd(char *name, 
				 char **data, 
				 int active, 
				 char *symbol,
				 int  notoggle,
				 int state)
:codaTbtnComd(name, active, notoggle, state)
{
#ifdef _TRACE_OBJECTS
  printf("Create codaXpmtbtnComd Object \n");
#endif
  _initState = state;
  _notoggle = notoggle;
  _xpm_data = data;
  _sel_xpm = 0;
  _bgsymbol = new char[::strlen(symbol) + 1];
  ::strcpy (_bgsymbol, symbol);
}

codaXpmtbtnComd::codaXpmtbtnComd(char *name, 
				 char **data, 
				 char **sel_data,
				 int  active, 
				 char *symbol,
				 int  state)
:codaTbtnComd(name, active, 1, state)
{
#ifdef _TRACE_OBJECTS
  printf("Create codaXpmtbtnComd Object \n");
#endif
  _initState = state;
  _notoggle = 1;
  _xpm_data = data;
  _sel_xpm = sel_data;
  _bgsymbol = new char[::strlen(symbol) + 1];
  ::strcpy (_bgsymbol, symbol);
}

void codaXpmtbtnComd::createXInterface(Widget parent)
{
  if (_bgsymbol)
    tb = new XcodaXpmtbtnInterface(parent, _xpm_data, _sel_xpm, this, 
				   _notoggle, _initState, _bgsymbol);
  else
    tb = new XcodaXpmtbtnInterface(parent, _xpm_data, _sel_xpm, this, 
				   _notoggle, _initState);
  // creation already register itself to command object
  tb->init();
}

codaXpmtbtnComd::~codaXpmtbtnComd()
{
#ifdef _TRACE_OBJECTS
  printf("Create codaXpmtbtnComd Object \n");
#endif
  if (_bgsymbol)
    delete []_bgsymbol;
  // no need to free _xpm_data compiler allocated space
}

