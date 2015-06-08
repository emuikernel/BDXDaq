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
//	Command Lisr header file
//	
// Author:  Jie Chen
//       CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: codaComdList.h,v $
//   Revision 1.2  1998/04/08 17:29:51  heyes
//   get in there
//
//   Revision 1.1.1.1  1996/10/10 19:24:59  chen
//   coda motif C++ library
//
//
#ifndef _CODA_COMD_LIST
#define _CODA_COMD_LIST

class codaComd;

class codaComdList {

public:
    
  codaComdList();           // Construct an empty list
  virtual ~codaComdList();  // Destroys list, but not objects in list
    
  void add ( codaComd * );  // Add a single Cmd object to list
    
  codaComd **contents() { return _contents; } // Return the list
  int size() { return _numElements; }         // Return list size
  codaComd *operator[]( int );                // Return an element of the list

private:
    
  codaComd **_contents;    // The list of objects
  int      _numElements;   // Current size of list
    
};
#endif
