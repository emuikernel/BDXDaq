//-----------------------------------------------------------------------------
// Copyright (c) 1994,1995 Southeastern Universities Research Association,
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
//	 codaStrHash: coda hash table keyed by a variable length string
//                    Open Hash with buckets implemented by single linked lists
//    
//              Note: void *s are stored inside the table. This class
//                    will not manage these pointers. Callers have to
//                    manage these pointers
// 
//              Note: this is unsafe C++ practice. Use at your own risk
//              
//            Reason: It is so difficult to use a template class inside
//                    a shared library. (Cfront based C++ compiler cannot
//                    instantiate a template class during compilation time
//	
// Author:  Jie Chen
//       CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: codaStrHash.cc,v $
//   Revision 1.2  1998/05/28 17:46:46  heyes
//   new GUI look
//
//   Revision 1.1.1.1  1996/10/11 13:39:31  chen
//   run control source
//
//
//
#include <assert.h>
#include <stdio.h>
#include "codaStrHash.h"

//======================================================================
//      A simple string hash fucntion for codaDevice and caChannel
//           Its implementation will be changed for release 1.1
//======================================================================
unsigned int codaStrHashFunc (char *src)
{
  unsigned int hash = 0, g;

  for (int i = 0; src[i] != '\0'; i++){
    hash = (hash << 4) + src[i];
    // assume 32 bit integer
    if (g = hash & 0xf0000000){
      hash ^= g >> 24;
      hash ^= g;
    }
  }
  return hash;
}


//======================================================================
//	class codaStrHash implementation
//======================================================================
codaStrHash::codaStrHash (unsigned int max, unsigned int (*f)(codaKeyItem))
:tablesize(max), hashCode_ (f)
{
#ifdef _TRACE_OBJECTS
  printf ("Create codaStrHash Class Objects\n");
#endif
  // codaSlist has a default constructor, so one can create an array of the list
  buckets = new codaSlist[tablesize];
  assert (buckets);
}

codaStrHash::~codaStrHash (void)
{
#ifdef _TRACE_OBJECTS
  printf ("Delete codaStrHash Class Objects\n");
#endif
  delete []buckets;
  buckets = 0;
}

int 
codaStrHash::isEmpty()
{
  // if any table is non-empty, return 0
  for (int i = 0; i < tablesize; i++)
    if (!buckets[i].isEmpty())
      return 0;

  // all empty
  return 1;
}

void
codaStrHash::deleteAllValues()
{
  // delete all values from a hash table
  // clear the elements from each of teh buckets
  for (int i = 0; i < tablesize; i++)
    buckets[i].deleteAllValues();
}

unsigned int 
codaStrHash::hash(const codaKeyItem& key) const
{
  // return hashed value of key
  return ((*hashCode_)(key) % tablesize);
}

void
codaStrHash::add (codaKeyItem key, codaHashItem newele)
{
  buckets[hash (key)].add (newele);
}

int
codaStrHash::remove (codaKeyItem key, codaHashItem ele)
{
  return (buckets[hash (key)].remove (ele));
}

int
codaStrHash::find (codaKeyItem key, codaHashItem ele) const
{
  return buckets[hash (key)].includes(ele);
}

codaSlist&
codaStrHash::bucketRef (codaKeyItem key)
{
  return (buckets[hash (key)]);
}

//======================================================================
//	class codaStrHashIterator implementation
//======================================================================
codaStrHashIterator::codaStrHashIterator (codaStrHash& v)
:base(v), currentIndex(0), itr(0)
{
#ifdef _TRACE_OBJECTS
  printf ("Create codaStrHashIterator Class Object \n");
#endif
  // no further initialization
}

codaStrHashIterator::~codaStrHashIterator (void)
{
#ifdef _TRACE_OBJECTS
  printf ("Delete codaStrHashIterator Class Object \n");
#endif
  currentIndex = 0;
  if (itr)
    delete itr;
}

int 
codaStrHashIterator::init (void)
{
  // initialize iterator, 
  // start search with first bucket
  currentIndex = 0; 
  itr = 0;
  return getNextIterator(); 
}

codaHashItem 
codaStrHashIterator::operator() (void)
{
  // return current element
  return (*itr)();
}

int 
codaStrHashIterator::operator ! (void)
{
  // test if there is a current element
  return itr != 0;
}

int 
codaStrHashIterator::operator ++ (void)
{
  // see if current iterator can be advanced
  if (itr && ++(*itr)) 
    return 1;

  // if not, get next iterator
  currentIndex++;
  return getNextIterator();
}

void 
codaStrHashIterator::operator = (codaHashItem val)
{
  // change the current value
  (*itr) = val;
}

void
codaStrHashIterator::removeCurrent (void)
{
  if (itr) 
    itr->removeCurrent ();
}

int 
codaStrHashIterator::getNextIterator (void)
{
  // if there is an old iterator, delete it
  if (itr != 0) 
    delete itr;

  // now search for a new one
  for (; currentIndex < base.tablesize; currentIndex++){
    // generate a new iterator at the current point
    itr = new codaSlistIterator (base.buckets[currentIndex]);
    assert(itr != 0);

    // if it has at least one element, we're done
    if (itr->init())
      return 1;

    // otherwise delete it, try again
    delete itr;
  }
  // all out of iterators, can quit
  itr = 0;
  return 0;
}
