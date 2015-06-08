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
//      Composite Transitioner Class
//      This Class contains multiple transitioners. 
//      When it execute, all transitioners will be started.
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: compTransitioner.h,v $
//   Revision 1.1.1.1  1996/10/11 13:39:21  chen
//   run control source
//
//
#ifndef _CODA_COMP_TRANSITIONER_H
#define _CODA_COMP_TRANSITIONER_H

#include <stdio.h>
#include <string.h>
#include <assert.h>

#include <codaSlist.h>

class transitioner;

class compTransitioner
{
public:
  // constructor and detructor
  compTransitioner (void);
  ~compTransitioner (void);
  // caller add transioner in a sequence of state transition
  // add a transitioner to the end of the list
  void pendTransitioner (transitioner* tr);
  // add a transitioner to the beginning of the list
  void addTransitioner  (transitioner* tr);
  // execute transition
  void execute (void);
private:
  // deny access of copy and assignment
  compTransitioner (const compTransitioner&);
  compTransitioner& operator = (const compTransitioner& );
  // list of transitioner
  codaSlist tranList_;
  // !!!! order dependent
  codaSlistIterator    trIte_;
};
#endif
