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
//      Implementation of Edge list
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: cgElist.cc,v $
//   Revision 1.1.1.1  1996/10/10 19:38:04  chen
//   CODA OO-graphics library for Xt
//
//
#include <stdio.h>
#include "cgElist.h"

cgElist::cgElist()
:head (0), scan_decrement (1)
{
  // empty
}

cgElist::cgElist (cgVertex* v)
:head (0), scan_decrement (1)
{
  cgVertex *p = 0;
  for (p = v; p->givnext(); p = p->givnext())
    insert_edge(*p,*p->givnext());
  insert_edge(*p,*v);               // edge from last to first vertex

  rite = head;
  scan = int(head->givymax() + 1);
}

// make edge from vertices and put in edge list
void cgElist::insert_edge(cgVertex& a,cgVertex& b)
{            
  cgEdge* edge = a.make_edge(b) ;

  if (!edge) return;

  cgEdge** cur = &head ;                      // find insertion place
  while (*cur && (*cur)->givymax() > edge->givymax())
    cur = (*cur)->givaddrnext();                          // advance

  edge->setnext(*cur);                        // then insert new edge
  *cur = edge;
}

void cgElist::include()            // include edges whose ymax >= scan
{
  while (rite && rite->givymax() >= scan)
    rite = rite->givnext();
}

void cgElist::exclude()            // exclude edges whose ymin >= scan
{
  cgEdge **cur = &head ;

  while (*cur != rite)
    if ((*cur)->givymin() >= scan)
      {
	cgEdge* e = *cur;
	*cur = (*cur)->givnext();                           // remove
	delete e;
      }
    else
      cur = (*cur)->givaddrnext();                       // advance
}

void cgElist::update()                           // increment x-values
{
  if (head)
    {
      head->update();
      head->givnext()->update();
    }
}

void cgElist::swap()                                // swap edges on x
{
  if (head && head->givx() > head->givnext()->givx())
    {
      cgEdge* temp = head;
      head = head->givnext();
      head->setnext(temp);
      temp->setnext(rite); 
    }
}

void
cgElist::setCgCxt (cgContext& cxt)
{
  cgEdge *p;
  
  for (p = head; p != 0; p = p->givnext ())
    p->setCgCxt (cxt);
}

void cgElist::fill(void (cgEdge::*f)(double))
{
  while (scan -= scan_decrement ,
	 include() ,
	 exclude() ,
	 update()  ,
	 swap()    ,
	 head      )                     // stop when edge list empty
    (head->*f)(scan);
}
