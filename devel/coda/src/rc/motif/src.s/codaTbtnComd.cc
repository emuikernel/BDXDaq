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
//	 CODA Toggle Button Command class
//	
// Author:  Jie Chen
//       CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: codaTbtnComd.cc,v $
//   Revision 1.2  1998/04/08 17:29:55  heyes
//   get in there
//
//   Revision 1.1.1.1  1996/10/10 19:24:57  chen
//   coda motif C++ library
//
//
#include <stdio.h>
#include <codaTbtnComd.h>
#include <codaComdList.h>
#include <XcodaTbtnInterface.h>

class XcodaComdXInterface;

codaTbtnComd::codaTbtnComd (char *name, int active, int st)
:codaComd(name, active), initState(st)
{
#ifdef _TRACE_OBJECTS
  printf("Create codaTbtnComd Object \n");
#endif
  tb = NULL;
  _acc = 0;
  _acc_text = 0;
  _notoggle = 0;
}

codaTbtnComd::codaTbtnComd (char *name, int active, int notoggle, int st)
:codaComd(name, active), _notoggle(notoggle), initState(st)
{
#ifdef _TRACE_OBJECTS
  printf("Create codaTbtnComd Object \n");
#endif
  tb = NULL;
  _acc = 0;
  _acc_text = 0;
}

codaTbtnComd::codaTbtnComd (char *name, 
			    int active, 
			    char *acc,
			    char *acc_text,
			    int st)
:codaComd(name, active), initState(st)
{
#ifdef _TRACE_OBJECTS
  printf("Create codaTbtnComd Object \n");
#endif
  tb = NULL;
  _acc = new char[::strlen(acc) + 1];
  ::strcpy(_acc, acc);
  _acc_text = new char[::strlen(acc_text) + 1];
  ::strcpy(_acc_text, acc_text);
  _notoggle = 0;
}

void codaTbtnComd::createXInterface(Widget parent)
{
  if(_acc && _acc_text)
    tb = new XcodaTbtnInterface (parent, this, initState, _acc, _acc_text);
  else
    tb = new XcodaTbtnInterface(parent, this, _notoggle, initState);
  // creation part already did registration to command object
  tb->init();
}

codaTbtnComd::~codaTbtnComd()
{
#ifdef _TRACE_OBJECTS
  printf("Destroy codaTbtnComd Object \n");
#endif
  if (tb != NULL)
    delete tb;
  if(_acc)
    delete []_acc;
  if(_acc_text)
    delete []_acc_text;
}

int codaTbtnComd::state()
{
  if (tb == NULL)
    return -1;
  else{
    return tb->state();
  }
}

void codaTbtnComd::setState(int st)
{
  if(tb == NULL)
    return;
  else{
    tb->setState(st);
  }
}

      
