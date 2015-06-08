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
//      Implementation of general glist
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: cgGlist.cc,v $
//   Revision 1.1.1.1  1996/10/10 19:38:04  chen
//   CODA OO-graphics library for Xt
//
//
#include <cgEdge.h>
#include "cgGlist.h"

cgGlist::cgGlist (cgVertex* v)
:cgElist(v)
{
  // empty
}

// edges are destructed singly
cgGlist::~cgGlist (void) 
{
  // empty
}

// increment x-values
void cgGlist::update (void)                           
{
  for (cgEdge* cur = head; cur != rite; cur = cur->givnext())
    cur->update();    // add dx to x         
}

// sort edges on x
void cgGlist::swap()
{
  if (head != rite) {
    cgEdge  *tem ,                  //  temporary edge
    **cur ,                         // current edge
    **ins = &head ;                 // edge to be inserted
      
    while (*ins != rite) {
      cur = &head;
      while ((*cur)->givx() < (*ins)->givx())
	cur = (*cur)->givaddrnext();
      if (cur != ins)
	tem   = (*ins)->givnext(),

      (*ins)->setnext(*cur),
      *cur  = *ins,
      *ins  = tem;
      else
	ins = (*ins)->givaddrnext();
    }
  }
}

void cgGlist::fill(void (cgEdge::*f)(double))
{
  while (scan -= scan_decrement ,
	 include() ,
	 exclude() ,
	 update()  ,
	 swap()    ,
	 head      )                     // stop when edge list empty

    for (cgEdge* p = head; p != rite; p = p->givnext()->givnext())
      (p->*f)(scan);
}
