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
//	 Single Linked List for pointers void *
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
//   $Log: codaSlist.h,v $
//   Revision 1.1.1.1  1996/10/11 13:39:31  chen
//   run control source
//
// Revision 1.1  1995/06/30  15:08:00  chen
// single linked list for void *
//
//
#ifndef _CODA_SLIST_H
#define _CODA_SLIST_H

#include <assert.h>

typedef void*  codaSlistItem;

//======================================================================
//	class codaGenSlist
//        Single Linked List for void* pointer
//======================================================================
class codaSlistLink;
class codaSlistIterator;
class codaSlistCursor;

class codaSlist
{
public:
  // constructors
  codaSlist (void);
  codaSlist (const codaSlist & source);
  virtual                  ~codaSlist (void);

  // operations

  // add list item to the beginning of the list
  virtual void             add(codaSlistItem value);

  // remove a list item from the list
  // return 0: nothing to remove
  // return 1: remove success
  virtual int              remove (codaSlistItem value);

  // clean up the list. 
  virtual void             deleteAllValues();

  // return first element of the list
  virtual codaSlistItem    firstElement() const;

  // return last element of the list
  virtual codaSlistItem    lastElement() const;

  // duplicate ths whole list
  virtual codaSlist*       duplicate() const;
  
  // check whether this list contains a particular item
  // return 1: yes. return 0: no
  virtual int              includes(codaSlistItem value) const;

  // Is list empty
  // return 1: yes. return 0: no
  virtual int              isEmpty() const;

  // remove first element of the list
  virtual void             removeFirst();

  // return number of elements inside the list
  virtual int              count() const;

 protected:
  // data field
  codaSlistLink*           ptrToFirstLink;

  // friends
  friend class  codaSlistIterator;
  // cannot modify list in anyways
  friend class  codaSlistCursor;
};

//======================================================================
//	class codaSlistLink
//        Single linked list link node
//======================================================================
class codaSlistLink
{
 public:
  // insert a new element following the current value
  codaSlistLink*     insert (codaSlistItem val);

 private:
  // constructor
  codaSlistLink (codaSlistItem linkValue, codaSlistLink * nextPtr);

  // duplicate
  codaSlistLink*     duplicate (void);

  // data areas
  codaSlistItem      value;
  codaSlistLink*     ptrToNextLink;

  // friends
  friend class       codaSlist;
  friend class       codaSlistIterator;
  friend class       codaSlistCursor;
};

//===================================================================
//	class codaSlistIterator
//		implements iterator protocol for linked lists
//		also permits removal and addition of elements
//===================================================================
class codaSlistIterator
{
public:
  // constructor
  codaSlistIterator        (codaSlist& aList);

  // iterator protocol
  virtual int              init (void);
  virtual codaSlistItem    operator () (void);
  virtual int              operator !  (void);
  virtual int              operator ++ (void);
  virtual void             operator =  (codaSlistItem value);

  // new methods specific to list iterators

  // remove current item pointed by the iterator from the list
  void                     removeCurrent(void);
  
  // add an item to the list before the position pointed by the iterator
  void                     addBefore(codaSlistItem  newValue);

  // add an item to the list after the position pointed by the iterator
  void                     addAfter(codaSlistItem   newValue);

  // search an item and move the iterator to that position
  int                      searchSame(codaSlistItem &value);

protected:
  // data areas
  codaSlistLink *    currentLink;
  codaSlistLink *    previousLink;
  codaSlist&         theList;
};

//===================================================================
//	class codaSlistCursor
//		implements cursor protocol for linked lists
//===================================================================
class codaSlistCursor
{
public:
  // constructor
  codaSlistCursor           (const codaSlist& aList);

  // iterator protocol
  virtual int               init        (void);
  virtual codaSlistItem     operator () (void);
  virtual int               operator !  (void);
  virtual int               operator ++ (void);
  virtual void              operator =  (codaSlistItem value);

  int                       searchSame  (codaSlistItem &value);

 protected:
  // data areas
  codaSlistLink *           currentLink;
  codaSlistLink *           previousLink;
  const codaSlist&          theList;
};

//======================================================================
//      class doubleEndedList
//           Not only keeps a pointer to first node
//           but also keeps a pointer to last node
//======================================================================
class codaDoubleEndedSlist: public codaSlist{
 public:
  //constructor
  codaDoubleEndedSlist   (void);
  codaDoubleEndedSlist   (const codaDoubleEndedSlist &v);
  
  // override the following methods from the codaSlist
  virtual void      add             (codaSlistItem value);
  virtual void      deleteAllValues (void);
  virtual void      removeFirst     (void);

  // add new element to the end of the list
  virtual void      addToEnd        (codaSlistItem value);

 protected:
  codaSlistLink    *ptrToLastLink;
};
#endif
