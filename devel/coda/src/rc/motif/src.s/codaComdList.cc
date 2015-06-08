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
//	Command List Header File
//	
// Author:  Jie Chen
//       CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: codaComdList.cc,v $
//   Revision 1.2  1998/04/08 17:29:51  heyes
//   get in there
//
//   Revision 1.1.1.1  1996/10/10 19:24:56  chen
//   coda motif C++ library
//
//
#include <stdio.h>
#include <codaComdList.h>

class codaComd;

codaComdList::codaComdList()
{
#ifdef _TRACE_OBJECTS
  printf("Create codaComdList object \n");
#endif
  // The list is initially empty
    
  _contents    = 0;
  _numElements = 0;
}

codaComdList::~codaComdList()
{
#ifdef _TRACE_OBJECTS
  printf("Delete codaComdList object \n");
#endif
  // free the list but not the object it contains
    
  delete []_contents;
}

void codaComdList::add ( codaComd *cmd )
{
    int i;
    codaComd **newList;
    
    // Allocate a list large enough for one more element
    
    newList = new codaComd*[_numElements + 1];
    
    // Copy the contents of the previous list to
    // the new list
    
    for( i = 0; i < _numElements; i++)
	newList[i] = _contents[i];
    
    // Free the old list
    
    delete []_contents;
    
    // Make the new list the current list
    
    _contents =  newList;
    
    // Add the command to the list and update the list size.
    
    _contents[_numElements] = cmd;
    
    _numElements++;
}

codaComd *codaComdList::operator[] ( int index )
{
    // Return the indexed element
    
    return _contents[index];
}
