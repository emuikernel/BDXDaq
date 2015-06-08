//-----------------------------------------------------------------------------
// Copyright (c) 1991,1992 Southeastern Universities Research Association,
//                         Continuous Electron Beam Accelerator Facility
//
// This software was developed under a United States Government license
// described in the NOTICE file included as part of this distribution.
//-----------------------------------------------------------------------------
// Description:
//	 codaIntHash: coda hash table keyed by an integer
//                    Open Hash with buckets implemented by single linked lists
//    
//              Note: void *s are stored inside the table. This class
//                    will not manage these pointers. Callers have to
//                    manage these pointers
//
// Author: Jie Chen
// 
// Revision History:
//   $Log: codaIntHash.cc,v $
//   Revision 1.1.1.1  1996/10/11 13:39:31  chen
//   run control source
//
//
//
#include <stdio.h>
#include <assert.h>
#include "codaIntHash.h"

// Use multiplication method. h(k) = [m(kA mod1)]
// A = (sqrt(5) - 1)/ 2 ~ 0.6180339887 suggested by Dr. Knuth
// m is power of 2

const double codaA = 0.6180339887;

//======================================================================
//	class codaIntHash implementation
//======================================================================
codaIntHash::codaIntHash (unsigned int max)
:tablesize(max)
{
#ifdef _TRACE_OBJECTS
  printf ("Create codaIntHash Class Objects\n");
#endif
  // codaSlist has a default constructor, so one can create an array of the list
  buckets = new codaSlist[tablesize];
  assert (buckets);
}

codaIntHash::~codaIntHash (void)
{
#ifdef _TRACE_OBJECTS
  printf ("Delete codaIntHash Class Objects\n");
#endif
  delete []buckets;
  buckets = 0;
}

int 
codaIntHash::isEmpty()
{
  // if any table is non-empty, return 0
  for (int i = 0; i < tablesize; i++)
    if (!buckets[i].isEmpty())
      return 0;

  // all empty
  return 1;
}

void
codaIntHash::deleteAllValues()
{
  // delete all values from a hash table
  // clear the elements from each of teh buckets
  for (int i = 0; i < tablesize; i++)
    buckets[i].deleteAllValues();
}

unsigned int 
codaIntHash::hash(const codaIntKeyItem key) const
{
  // return hashed value of key
  double val = (double)key*codaA - (int)(key*codaA);
  unsigned int slot = (unsigned int)(val*tablesize);
  return slot;
}

void
codaIntHash::add (codaIntKeyItem key, codaHashItem newele)
{
  buckets[hash (key)].add (newele);
}

int
codaIntHash::remove (codaIntKeyItem key, codaHashItem ele)
{
  return (buckets[hash (key)].remove (ele));
}

int
codaIntHash::find (codaIntKeyItem key, codaHashItem ele) const
{
  return buckets[hash (key)].includes(ele);
}

codaSlist&
codaIntHash::bucketRef (codaIntKeyItem key)
{
  return (buckets[hash (key)]);
}

//======================================================================
//	class codaIntHashIterator implementation
//======================================================================
codaIntHashIterator::codaIntHashIterator (codaIntHash& v)
:base(v), currentIndex(0), itr(0)
{
#ifdef _TRACE_OBJECTS
  printf ("Create codaIntHashIterator Class Object \n");
#endif
  // no further initialization
}

codaIntHashIterator::~codaIntHashIterator (void)
{
#ifdef _TRACE_OBJECTS
  printf ("Delete codaIntHashIterator Class Object \n");
#endif
  currentIndex = 0;
  if (itr)
    delete itr;
}

int 
codaIntHashIterator::init (void)
{
  // initialize iterator, 
  // start search with first bucket
  currentIndex = 0; 
  itr = 0;
  return getNextIterator(); 
}

codaHashItem 
codaIntHashIterator::operator() (void)
{
  // return current element
  return (*itr)();
}

int 
codaIntHashIterator::operator ! (void)
{
  // test if there is a current element
  return itr != 0;
}

int 
codaIntHashIterator::operator ++ (void)
{
  // see if current iterator can be advanced
  if (itr && ++(*itr)) 
    return 1;

  // if not, get next iterator
  currentIndex++;
  return getNextIterator();
}

void 
codaIntHashIterator::operator = (codaHashItem val)
{
  // change the current value
  (*itr) = val;
}

int 
codaIntHashIterator::getNextIterator (void)
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
