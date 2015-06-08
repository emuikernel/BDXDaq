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
//   $Log: codaStrHash.h,v $
//   Revision 1.1.1.1  1996/10/11 13:39:31  chen
//   run control source
//
//
//
#ifndef _CODA_STR_HASH_H
#define _CODA_STR_HASH_H

#include <codaSlist.h>

//======================================================================
//  One simple string hash function
//======================================================================
extern unsigned int codaStrHashFunc (char *str);

typedef char* codaKeyItem;
typedef void* codaHashItem;

//======================================================================
//	class codaStrHash
//		collection of buckets indexed by hashed values
//======================================================================
class codaStrHashIterator;

class codaStrHash
{
public:
  // constructor
  // construct a table with entry max and hash function *f
  // hash function return any integer from 0 to MAX_INT_VALUE
  codaStrHash (unsigned int max, unsigned int (*f)(codaKeyItem));
  // destructor
  virtual ~codaStrHash (void);

  // operations

  // is the table empty: return 1: yes. return 0: no
  virtual int   isEmpty();

  // clear the elements of the set
  virtual void  deleteAllValues();

  // add an element to the collection
  virtual void  add (codaKeyItem key, codaHashItem ele);

  // test to see whether this hash table includes one particular element
  virtual int   find (codaKeyItem key, codaHashItem ele) const;

  // remove an element. return 0: ele is not inside table. return 1: success
  virtual int   remove (codaKeyItem key, codaHashItem ele);

  // return a reference to a particular bucket according to the key
  codaSlist&  bucketRef (codaKeyItem key);

protected:
  friend class codaStrHashIterator;

  // the actual table itself is a vector of buckets
  const unsigned int    tablesize;
  codaSlist*            buckets;

  // convert key into unsigned integer value in range
  unsigned int          hash(const codaKeyItem& key) const;

 private:
  unsigned int          (*hashCode_)(codaKeyItem);
};

//======================================================================
//	class codaStrHashIterator
//		iterator protocol for hash tables
//======================================================================
class codaStrHashIterator
{
public:
  // constructor and destructor
  codaStrHashIterator  (codaStrHash& v);
  ~codaStrHashIterator (void);

  // iterator protocol 
  int             init       (void);
  codaHashItem    operator ()(void);
  int             operator ! (void);
  int             operator ++(void);
  void            operator = (codaHashItem value);

  // new operations
  // remove current item pointed by this cursor
  void            removeCurrent (void);

protected:
  codaStrHash&    base;
  unsigned int    currentIndex;
  // Single iterator within a bucket
  codaSlistIterator*         itr;
  // getNextIterator used to set internal iterator pointer
  // return 1: got it. return 0: no more iterator
  int             getNextIterator (void);
};
#endif
