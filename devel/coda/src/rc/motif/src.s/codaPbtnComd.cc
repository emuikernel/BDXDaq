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
//	 CODA Pushbutton Command class
//	
// Author:  Jie Chen
//       CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: codaPbtnComd.cc,v $
//   Revision 1.4  1998/04/08 17:29:52  heyes
//   get in there
//
//   Revision 1.3  1997/07/30 15:32:46  heyes
//   clean for Solaris
//
//   Revision 1.2  1997/06/06 19:00:45  heyes
//   new RC
//
//   Revision 1.1.1.1  1996/10/10 19:24:57  chen
//   coda motif C++ library
//
//
#include <stdio.h>
#include <codaPbtnComd.h>
#include <codaComdList.h>
#include <XcodaPbtnInterface.h>

class codaComdXInterface;

codaPbtnComd::codaPbtnComd (char *name, int active):codaComd(name, active)
{
#ifdef _TRACE_OBJECTS
  printf("Create codaPbtnComd object \n");
#endif
  pb = NULL;
  _acc = 0;
  _acc_text = 0;
}

codaPbtnComd::codaPbtnComd (char *name, 
			    int active,
			    char *acc,
			    char *acc_text)
:codaComd(name, active)
{
#ifdef _TRACE_OBJECTS
  printf("Create codaPbtnComd object \n");
#endif
  pb = NULL;
  _acc = new char[::strlen(acc) + 1];
  ::strcpy(_acc, acc);
  _acc_text = new char[::strlen(acc_text) + 1];
  ::strcpy(_acc_text, acc_text);
}

void codaPbtnComd::defaultButton()  
{
  //printf("pb is %08x\n",pb);
}

void codaPbtnComd::createXInterface(Widget parent)
{
  if(!_acc  && !_acc_text)
    pb = new XcodaPbtnInterface(parent, this);
  else
    pb = new XcodaPbtnInterface(parent, this, _acc, _acc_text);
  // creation part already register itself to command object
  pb->init();
}

codaPbtnComd::~codaPbtnComd()
{
#ifdef _TRACE_OBJECTS
  printf("Destroy codaPbtnComd object \n");
#endif
  if (pb != NULL)
    delete pb;
  if(_acc)
    delete []_acc;
  if(_acc_text)
    delete []_acc_text;
}
