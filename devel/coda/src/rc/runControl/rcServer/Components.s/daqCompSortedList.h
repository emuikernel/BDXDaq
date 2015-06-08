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
//      Sorted list according to priority of daqComponent in
//      descreasing order
//      
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: daqCompSortedList.h,v $
//   Revision 1.1.1.1  1996/10/11 13:39:18  chen
//   run control source
//
//
#ifndef _CODA_DAQCOMP_SORTED_LIST_H
#define _CODA_DAQCOMP_SORTED_LIST_H

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <codaSlist.h>

class daqCompSortedList: public codaSlist
{
public:
  // constructor and destructor
  daqCompSortedList  (void);
  ~daqCompSortedList (void);
  
  // change implementation of add
  virtual void add (void *comp);

private:
  daqCompSortedList (const daqCompSortedList&);
  daqCompSortedList& operator = (const daqCompSortedList&);
};
#endif
