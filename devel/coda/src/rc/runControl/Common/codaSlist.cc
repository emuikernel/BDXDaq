//-----------------------------------------------------------------------------
// Copyright (c) 1994,1995 Southeastern Universities Research Association,
//                         Continuous Electron Beam Accelerator Facility
//
// This software was developed under a United States Government license
// described in the NOTICE file included as part of this distribution.
//
//-----------------------------------------------------------------------------
// 
// Description:
//	 Single Linked List for void *
//
//       Note: remove and clean operations on the list
//             will only remove link nodes without removal of
//             the content inside the nodes. It is callers' 
//             responsiblity to clean up those contents
//
//       This is unsafe C++ practice, use this list at you own risk
//       
//       Reason for this list: It is very difficult to instantiate
//       a template class in a stand alone shared library
//	
// Author:  Jie Chen
//       CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: codaSlist.cc,v $
//   Revision 1.3  1998/05/28 17:46:46  heyes
//   new GUI look
//
//   Revision 1.2  1997/08/25 16:02:01  heyes
//   fix some display problems
//
//   Revision 1.1.1.1  1996/10/11 13:39:31  chen
//   run control source
//
// Revision 1.1  1995/06/30  15:07:36  chen
// single linked list for void *
//
//
#include "stdio.h"
#include "codaSlist.h"

//======================================================================
//	class codaSlist implementation
//======================================================================
codaSlist::codaSlist (void)
:ptrToFirstLink(0)
{
#ifdef _TRACE_OBJECTS
  printf ("Create codaSlist Class Object\n");
#endif
  // no further initialization
}

codaSlist::codaSlist (const codaSlist & source)
{
#ifdef _TRACE_OBJECTS
  printf ("Create codaSlist Class Object\n");
#endif
  // duplicate elements from source list
  if (source.isEmpty())
    ptrToFirstLink = 0;
  else{
    codaSlistLink * firstLink = source.ptrToFirstLink;
    ptrToFirstLink = firstLink->duplicate();
  }
}

codaSlist::~codaSlist (void)
{
  // empty all elements from the list
#ifdef _TRACE_OBJECTS
  printf ("Delete codaSlist Class Object\n");
#endif
  deleteAllValues();
}

void codaSlist::add (codaSlistItem val)
{
  // add a new value to the front of a linked list
  ptrToFirstLink = new codaSlistLink(val, ptrToFirstLink);
  assert(ptrToFirstLink != 0);
}

int codaSlist::remove (codaSlistItem val)
{
  // remove an element from the list
  // loop to test each element
  codaSlistLink *q = ptrToFirstLink;
  for (codaSlistLink * p = ptrToFirstLink; p; p = p->ptrToNextLink){
    if (val == p->value){
      if (q == p){
	// remove first element
	ptrToFirstLink = p->ptrToNextLink;
	delete p;
      }
      else{
        q->ptrToNextLink = p->ptrToNextLink;
	delete p;
      }
      return 1;
    }
    q = p;
  }
  // not found
  return 0; 
}

void codaSlist::deleteAllValues (void)
{
  // clear all items from the list
  codaSlistLink * next;

  for (codaSlistLink * p = ptrToFirstLink; p != 0; p = next){
    // delete the element pointed to by p
    next = p->ptrToNextLink;
    p->ptrToNextLink = 0;
    delete p;
  }
  
  // mark that the list contains no elements
  ptrToFirstLink = 0;
}

int codaSlist::count (void) const
{
  // count how many items are there 
  codaSlistLink * next;
  int      num = 0;

  for (codaSlistLink *p = ptrToFirstLink; p != 0; p = next){
    next = p->ptrToNextLink;
    num++;
  }
  return num;
}

codaSlist * codaSlist::duplicate (void) const
{
  codaSlist * newlist = new codaSlist;
  assert(newlist != 0);

  // copy list
  if (ptrToFirstLink)
    newlist->ptrToFirstLink = ptrToFirstLink->duplicate();
  
  // return the new list
  return newlist;
}

codaSlistItem
codaSlist::firstElement (void) const
{
  // return first value in list
  assert(ptrToFirstLink != 0);
  return ptrToFirstLink->value;
}

codaSlistItem
codaSlist::lastElement (void) const
{
  codaSlistLink *p;
  assert(ptrToFirstLink != 0);
  // loop through until last element
  for (p = ptrToFirstLink; p->ptrToNextLink; p = p->ptrToNextLink)
    ;
  return p->value;
}

int codaSlist::includes(codaSlistItem v) const
{
  codaSlistLink *p;
  // loop to test each element
  for (p = ptrToFirstLink; p; p = p->ptrToNextLink)
    if (v == p->value)
      return 1;
  
  // not found
  return 0;
}

int codaSlist::isEmpty (void) const
{
  // test to see if the list is empty
  // list is empty if the pointer to the first link is null
  return ptrToFirstLink == 0;
}

void codaSlist::removeFirst (void)
{
  // make sure there is a first element
  assert(ptrToFirstLink != 0);
  
  // save pointer to the removed node
  codaSlistLink * p = ptrToFirstLink;

  // reassign the ptrToFirstLink node
  ptrToFirstLink = p->ptrToNextLink;

  // recover memory used by the first element
  delete p;
}

//======================================================================
//	class slink implementation
//======================================================================
codaSlistLink* 
codaSlistLink::insert(codaSlistItem val)
{
  // insert a new link after current node
  ptrToNextLink = new codaSlistLink(val, ptrToNextLink);

  // check that allocation was successful
  assert(ptrToNextLink != 0);
  return ptrToNextLink;
}

codaSlistLink::codaSlistLink (codaSlistItem val, codaSlistLink * nxt)
:value(val), ptrToNextLink(nxt)
{
  // create and initialize a new link field
}

codaSlistLink* 
codaSlistLink::duplicate (void)
{
  codaSlistLink * newlink;

  // if there is a next field. copy remainder of list
  if (ptrToNextLink != 0)
    newlink = new codaSlistLink(value, ptrToNextLink->duplicate());
  else
    newlink = new codaSlistLink (value, 0);

  // check that allocation was successful
  assert(newlink != 0);
  return newlink;
}

//======================================================================
//	class listIterator implementation
//======================================================================
codaSlistIterator::codaSlistIterator(codaSlist & aList)
:theList(aList)
{
  // create and initialize a new list
  init();
}

int codaSlistIterator::init()
{
  // set the iterator to the first element in the list
  previousLink = 0;
  currentLink = theList.ptrToFirstLink;
  return currentLink != 0;
}

codaSlistItem
codaSlistIterator::operator () (void)
{
  // return value of current element
  // check to see if there is a current element
  assert(currentLink != 0);

  // return value associated with current element
  return currentLink->value;
}

int codaSlistIterator::operator ! (void)
{
  // test for termination of the iterator
  // if current link references a removed value,
  // update current to point to next position
  if (currentLink == 0)
    if (previousLink != 0)
      currentLink = previousLink->ptrToNextLink;

  // now see if currentLink is valid
  return currentLink != 0;
}

int codaSlistIterator::operator ++()
{
  // move current pointer to nect element
  // special processing if current link is deleted
  if (currentLink == 0){
    if (previousLink == 0)
      currentLink = theList.ptrToFirstLink;
    else
      currentLink = previousLink->ptrToNextLink;
  }
  else{
    // advance pointer
    previousLink = currentLink;
    currentLink = currentLink->ptrToNextLink;
  }

  // return true if we have a valid current element
  return currentLink != 0;
}

void codaSlistIterator::operator = (codaSlistItem val)
{
  // modify value of current element
  assert(currentLink != 0);

  // modify value of the current link
  currentLink->value = val;
}

void codaSlistIterator::removeCurrent (void)
{
  // remove the current element from a list
  // make sure there is a current element
  assert(currentLink != 0);

  // case 1, removing first element
  if (previousLink == 0)
    theList.ptrToFirstLink = currentLink->ptrToNextLink;
  
  // case 2, not removing the first element
  else
    previousLink->ptrToNextLink = currentLink->ptrToNextLink;
  
  // delete current node
  delete currentLink;

  // and set current pointer to null
  currentLink = 0;
}

void codaSlistIterator::addBefore(codaSlistItem val)
{
  // a a new element to list before current value
  // case 1, not at start
  if (previousLink)
    previousLink = previousLink->insert(val);

  // case 2, at start of list
  else{
    theList.codaSlist::add(val);
    previousLink = theList.ptrToFirstLink;
    currentLink = previousLink->ptrToNextLink;
  }
}

void codaSlistIterator::addAfter(codaSlistItem val)
{
  // a a new element to list after current value
  // case 1, not at start
  if (currentLink != 0)
    currentLink->insert(val);

  // case 2, at end of list
  else if (previousLink != 0)
    currentLink = previousLink->insert(val);
  
  // case 3, start of list
  else
    theList.codaSlist::add(val);
}

int codaSlistIterator::searchSame(codaSlistItem &val)
{
  // search the list to find out whether we have this element
  // if we do, move cursor to this element return 1
  // if we don't return 0
  init();

  if (currentLink == 0){
  // empty link 
    return 0;
  }
  while (currentLink != 0){
    // advance pointer
    if (currentLink->value == val)
      break;
    previousLink = currentLink;
    currentLink = currentLink->ptrToNextLink;
  }

  // return true if we have a valid current element
  return currentLink != 0;
}

//========================================================================
//          Implementation of codaSlistCursor
//                 Implements of cursor without changing list
//========================================================================
codaSlistCursor::codaSlistCursor(const codaSlist & aList)
:theList(aList)
{
  // create and initialize a new list
  init();
}

int
codaSlistCursor::init()
{
  // set the iterator to the first element in the list
  previousLink = 0;
  currentLink = theList.ptrToFirstLink;
  return currentLink != 0;
}

codaSlistItem
codaSlistCursor::operator () (void)
{
  // return value of current element
  // check to see if there is a current element
  assert(currentLink != 0);

  // return value associated with current element
  return currentLink->value;
}

int codaSlistCursor::operator ! (void)
{
  // test for termination of the iterator
  // if current link references a removed value,
  // update current to point to next position
  if (currentLink == 0)
    if (previousLink != 0)
      currentLink = previousLink->ptrToNextLink;
  
  // now see if currentLink is valid
  return currentLink != 0;
}

int codaSlistCursor::operator ++ (void)
{
  // move current pointer to nect element
  // special processing if current link is deleted
  if (currentLink == 0){
    if (previousLink == 0)
      currentLink = theList.ptrToFirstLink;
    else
      currentLink = previousLink->ptrToNextLink;
  }
  else{
    // advance pointer
    previousLink = currentLink;
    currentLink = currentLink->ptrToNextLink;
  }

  // return true if we have a valid current element
  return currentLink != 0;
}

int codaSlistCursor::searchSame (codaSlistItem &val)
{
  // search the list to find out whether we have this element
  // if we do, move cursor to this element return 1
  // if we don't return 0
  init();

  if (currentLink == 0){
    // empty link 
    return 0;
  }
  while (currentLink != 0){
    // advance pointer
    if (currentLink->value == val)
      break;
    previousLink = currentLink;
    currentLink = currentLink->ptrToNextLink;
  }

  // return true if we have a valid current element
  return currentLink != 0;
}

void codaSlistCursor::operator = (codaSlistItem )
{
  // empty
}


//========================================================================
//          Implementation of doubleEndedList
//========================================================================
codaDoubleEndedSlist::codaDoubleEndedSlist()
:codaSlist()
{
  ptrToLastLink = 0;
}

codaDoubleEndedSlist::codaDoubleEndedSlist(const codaDoubleEndedSlist &v)
:codaSlist(v)
{
  ptrToLastLink = v.ptrToLastLink;
}

void codaDoubleEndedSlist::add(codaSlistItem val)
{
  // add method needs to check one special case when the first element
  // is added to the list
  if (isEmpty()) {
    // call parent method
    codaSlist::add (val);
    ptrToLastLink = ptrToFirstLink;
  }
  else{
    // always add to the head
    codaSlist::add(val);
  }
}

void 
codaDoubleEndedSlist::addToEnd(codaSlistItem val)
{
  //add new value T to the end of the list
  // if there is an end, add to it
  if(ptrToLastLink != 0)
    ptrToLastLink = ptrToLastLink->insert(val);
  else{
    // otherwise just add to the front
    add (val);
  }
}

void
codaDoubleEndedSlist::deleteAllValues()
{
  //delete all values from the list
  codaSlist::deleteAllValues();
  ptrToLastLink = 0;
}

void
codaDoubleEndedSlist::removeFirst()
{
  //remove the first element from the list
  // invoke the parent method
  codaSlist::removeFirst();
  
  // only do something if we removed last element
  if(isEmpty())
    ptrToLastLink = 0;
}

