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
//	Coda Xpm Pushbutton Command header file
//	
// Author:  Jie Chen
//       CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: codaXpmpbtnComd.cc,v $
//   Revision 1.4  1998/06/18 12:21:53  heyes
//   new GUI ready I think
//
//   Revision 1.3  1998/04/08 17:29:56  heyes
//   get in there
//
//   Revision 1.2  1997/07/30 14:12:21  heyes
//   add more xpm support
//
//   Revision 1.1.1.1  1996/10/10 19:24:57  chen
//   coda motif C++ library
//
//
#include <stdio.h>
#include <string.h>
#include <codaXpmpbtnComd.h>
#include <XcodaXpmpbtnInterface.h>

codaXpmpbtnComd::codaXpmpbtnComd(char *name, char **data, int active)
:codaPbtnComd(name, active)
{
#ifdef _TRACE_OBJECTS
  printf("Create codaXpmpbtnComd Object \n");
#endif
  _name = name;
  _xpm_data = data;
  _bgsymbol = 0;
}

codaXpmpbtnComd::codaXpmpbtnComd(char *name, 
				 char **data, 
				 int active, 
				 char *symbol)
:codaPbtnComd(name, active)
{
#ifdef _TRACE_OBJECTS
  printf("Create codaXpmpbtnComd Object \n");
#endif
  _name = name;
  _xpm_data = data;
  _bgsymbol = new char[::strlen(symbol) + 1];
  ::strcpy (_bgsymbol, symbol);
}

void codaXpmpbtnComd::createXInterface(Widget parent)
{
  if (_bgsymbol)
    pb = new XcodaXpmpbtnInterface(parent, _name, _xpm_data, this, _bgsymbol);
  else
    pb = new XcodaXpmpbtnInterface(parent, _name, _xpm_data, this);
  // creation already register itself to command object
  pb->init();
}

codaXpmpbtnComd::~codaXpmpbtnComd()
{
#ifdef _TRACE_OBJECTS
  printf("Create codaXpmpbtnComd Object \n");
#endif
  if (_bgsymbol)
    delete []_bgsymbol;
  // no need to free _xpm_data compiler allocated space
}


