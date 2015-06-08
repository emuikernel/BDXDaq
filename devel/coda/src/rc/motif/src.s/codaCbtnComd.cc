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
//	 CODA Cascade Button Command class
//	
// Author:  Jie Chen
//       CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: codaCbtnComd.cc,v $
//   Revision 1.2  1998/04/08 17:29:49  heyes
//   get in there
//
//   Revision 1.1.1.1  1996/10/10 19:24:56  chen
//   coda motif C++ library
//
//
#include <stdio.h>
#include <codaCbtnComd.h>
#include <codaComdList.h>
#include <XcodaCbtnInterface.h>

class codaComdXInterface;

codaCbtnComd::codaCbtnComd (char *name, int active):codaComd(name, active)
{
#ifdef _TRACE_OBJECTS
  printf("Create codaCbtnComd Objects \n");
#endif
  _comd_list = NULL;
  cb = NULL;
}

void codaCbtnComd::doit()
{
  //empty no need to provide any function
}

void codaCbtnComd::undoit()
{
  //empty
}

void codaCbtnComd::registerCommand(codaComd *cmd)
{
  if (_comd_list == NULL)
    _comd_list = new codaComdList();
  _comd_list->add (cmd);
}

void codaCbtnComd::createXInterface(Widget parent)
{
  cb = new XcodaCbtnInterface(parent, this);

  // Creation of cb already did register itself to command object
  cb->init();
  if (_comd_list != NULL)
    cb->addCommands (_comd_list);
}

codaCbtnComd::~codaCbtnComd()
{
#ifdef _TRACE_OBJECTS
  printf("Remove codaCbtnComd Objects \n");
#endif
  if (_comd_list != NULL)
    delete _comd_list;
  if (cb != NULL)
    delete cb;
}
