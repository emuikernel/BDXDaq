//-----------------------------------------------------------------------------
// Copyright (c) 1991,1992 Southeastern Universities Research Association,
//                         Continuous Electron Beam Accelerator Facility
//
// This software was developed under a United States Government license
// described in the NOTICE file included as part of this distribution.
//
//-----------------------------------------------------------------------------
// 
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
//   $Log: codaIntHash.h,v $
//   Revision 1.1.1.1  1996/10/11 13:39:31  chen
//   run control source
//
//
//     
#ifndef _CODA_INT_HASH_H
#define _CODA_INT_HASH_H

#include <codaSlist.h>

//======================================================================
//  One simple integer hash function
//           This hash function is used in callback hashing
//           It returns integer value between 0 to table size
//======================================================================

typedef long codaIntKeyItem;
typedef void* codaHashItem;
//======================================================================
//	class codaIntHash
//		collection of buckets indexed by hashed values
//======================================================================
class codaIntHashIterator;

class codaIntHash
{
public:
  // constructor, construct a table with entry max 
  codaIntHash (unsigned int max);
  // destructor
  virtual ~codaIntHash (void);

  // operations

  // is the table empty: return 1: yes. return 0: no
  virtual int   isEmpty();

  // clear the elements of the set
  virtual void  deleteAllValues();

  // add an element to the collection
  virtual void  add (codaIntKeyItem key, codaHashItem ele);

  // test to see whether this hash table includes one particular element
  virtual int   find (codaIntKeyItem key, codaHashItem ele) const;

  // remove an element. return 0: ele is not inside table. return 1: success
  virtual int   remove (codaIntKeyItem key, codaHashItem ele);

  // return a reference to a particular bucket according to the key
  codaSlist&  bucketRef (codaIntKeyItem key);

 protected:
  friend class codaIntHashIterator;

  // the actual table itself is a vector of buckets
  const unsigned int    tablesize;
  codaSlist*            buckets;

  // convert key into unsigned integer value in range
  unsigned int          hash(const codaIntKeyItem key) const;

};

//======================================================================
//	class codaIntHashIterator
//		iterator protocol for hash tables
//======================================================================
class codaIntHashIterator
{
public:
  // constructor and destructor
  codaIntHashIterator  (codaIntHash& v);
  ~codaIntHashIterator (void);

  // iterator protocol 
  int             init       (void);
  codaHashItem    operator ()(void);
  int             operator ! (void);
  int             operator ++(void);
  void            operator = (codaHashItem value);

protected:
  codaIntHash&    base;
  unsigned int    currentIndex;
  // Single iterator within a bucket
  codaSlistIterator*         itr;
  // getNextIterator used to set internal iterator pointer
  // return 1: got it. return 0: no more iterator
  int             getNextIterator (void);
};
#endif
