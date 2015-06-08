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
//      CODA RunControl: run type data structure
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: daqRunTypeStruct.cc,v $
//   Revision 1.2  1996/12/04 18:32:22  chen
//   port to 1.4 on hp and ultrix
//
//   Revision 1.1.1.1  1996/10/11 13:39:30  chen
//   run control source
//
//
#include <stdlib.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <daqArbStructFactory.h>
#include "daqRunTypeStruct.h"

int daqRunTypeStruct::maxNumRunTypes = 100;

daqRunTypeItem::daqRunTypeItem (void)
:runid_ (-1), inuse_ (0), name_ (0), cat_ (0), namelen_ (0), catlen_ (0)
{
#ifdef _TRACE_OBJECTS
  printf ("Create daqRunTypeItem Class Object\n");
#endif
}

daqRunTypeItem::daqRunTypeItem (char* name, long runid, 
				long inuse, char* category)
:runid_ (runid), inuse_ (inuse)
{
#ifdef _TRACE_OBJECTS
  printf ("    Create daqRunTypeItem Class Object\n");
#endif
  long len = ::strlen (name) + 1;
  namelen_ = coda_rc_roundup (len);

  name_ = new char[namelen_];
  ::strcpy (name_, name);


  if (category) {
    len = ::strlen (category) + 1;
    catlen_ = coda_rc_roundup (len);
    cat_  = new char[catlen_];
    ::strcpy (cat_, category);
  }
  else {
    catlen_ = 0;
    cat_ = 0;
  }
}

daqRunTypeItem::daqRunTypeItem (const daqRunTypeItem& ri)
:runid_ (ri.runid_), inuse_ (ri.inuse_), namelen_ (ri.namelen_),
 catlen_ (ri.catlen_)
{
#ifdef _TRACE_OBJECTS
  printf ("    Create daqRunTypeItem Class Object\n");
#endif
  name_ = new char[namelen_];
  ::strcpy (name_, ri.name_);

  if (catlen_ > 0) {
    cat_ = new char[catlen_];
    ::strcpy (cat_, ri.cat_);
  }
  else
    cat_ = 0;
}


daqRunTypeItem::~daqRunTypeItem (void)
{
#ifdef _TRACE_OBJECTS
  printf ("Delete daqRunTypeItem Class Object\n");
#endif
  
  if (name_)
    delete []name_;
  if (cat_)
    delete []cat_;
}

void
daqRunTypeItem::cleanUp (void)
{
  runid_ = -1;
  inuse_ = 0;
  if (name_)
    delete []name_;
  if (cat_)
    delete []cat_;
  name_ = 0;
  cat_ = 0;
  namelen_ = 0;
  catlen_ = 0;

}

long
daqRunTypeItem::runtypeId (void) const
{
  return runid_;
}

long
daqRunTypeItem::inuse (void) const
{
  return inuse_;
}

char*
daqRunTypeItem::name (void) const
{
  return name_;
}

char*
daqRunTypeItem::category (void) const
{
  return cat_;
}

void
daqRunTypeItem::encodeData (void)
{
  runid_ = htonl (runid_);
  inuse_ = htonl (inuse_);
  namelen_ = htonl (namelen_);
  catlen_ = htonl  (catlen_);
}

void
daqRunTypeItem::restoreData (void)
{
  runid_ = ntohl (runid_);
  inuse_ = ntohl (inuse_);
  namelen_ = ntohl (namelen_);
  catlen_ = ntohl  (catlen_);
}

void
daqRunTypeItem::encode (char* buffer, size_t& bufsize)
{
  int i = 0;
  long realsize = sizeof (daqRunTypeItem) - sizeof (daqArbStruct);

  // encode header
  encodeData ();
  ::memcpy (buffer, (void *)&(this->runid_), realsize);
  i += realsize;
  // restore data
  restoreData ();

  if (name_) {
    ::memcpy (&(buffer[i]), (void *)name_, namelen_);
    i += namelen_;
  }
  if (cat_) {
    ::memcpy (&(buffer[i]), (void *)cat_, catlen_);
    i += catlen_;
  }
  
  bufsize = (size_t)i;
}

void
daqRunTypeItem::decode (char* buffer, size_t& size)
{
  int i = 0;

  // clean up old information
  cleanUp ();

  long realsize = sizeof (daqRunTypeItem) - sizeof (daqArbStruct);

  // copy header information
  ::memcpy ((void *)&(this->runid_), buffer, 4*sizeof (long));
  // skip all other elements since they are pointers
  i += realsize;
  // convert to local byte ordering
  restoreData ();

  if (namelen_) {
    name_ = new char[namelen_];
    ::memcpy ((void *)name_, &(buffer[i]), namelen_);
    i += namelen_;
  }
  if (catlen_) {
    cat_ = new char[catlen_];
    ::memcpy ((void *)cat_, &(buffer[i]), catlen_);
    i += catlen_;
  }
  
  size = (size_t) i;
}

size_t
daqRunTypeItem::size (void)
{
  return (sizeof (daqRunTypeItem) + namelen_ + catlen_);
}

//========================================================================
//   Implementation of daqRunTypeStruct Class
//========================================================================
daqRunTypeStruct::daqRunTypeStruct (void)
:daqArbStruct (), id_ (daqArbStructFactory::RUNTYPE_INFO),
 numRunTypes_ (0)
{
#ifdef _TRACE_OBJECTS
  printf ("    Create daqRunTypeStruct Class Object\n");
#endif
  runtypes_ = new daqRunTypeItem*[daqRunTypeStruct::maxNumRunTypes];
}

daqRunTypeStruct::~daqRunTypeStruct (void)
{
#ifdef _TRACE_OBJECTS
  printf ("    Delete daqRunTypeStruct Class Object\n");
#endif  
  for (int i = 0; i < numRunTypes_; i++)
    delete runtypes_[i];
  delete []runtypes_;
}

daqArbStruct*
daqRunTypeStruct::dup (void)
{
  daqRunTypeStruct* tmp = new daqRunTypeStruct ();

  if (numRunTypes_ > 0) {
    for (int i = 0; i < numRunTypes_; i++) 
      tmp->runtypes_[i] = new daqRunTypeItem (*runtypes_[i]);
    tmp->numRunTypes_ = numRunTypes_;
  }
  tmp->id_ = id_;

  return tmp;
}

void
daqRunTypeStruct::cleanUp (void)
{
  if (numRunTypes_ > 0) {
    for (int i = 0; i < numRunTypes_; i++)
      delete runtypes_[i];
  }
  numRunTypes_ = 0;
}

size_t
daqRunTypeStruct::size (void)
{
  size_t s = 0;

  long realsize = sizeof (daqRunTypeStruct) - sizeof (daqArbStruct);
  s += realsize;

  if (numRunTypes_ > 0) {
    for (int i = 0; i < numRunTypes_; i++) 
      s = s + runtypes_[i]->size ();
  }
  return s;
}

long
daqRunTypeStruct::id (void)
{
  return id_;
}

void
daqRunTypeStruct::encodeData (void)
{
  if (numRunTypes_ > 0) {
    for (int i = 0; i < numRunTypes_; i++)
      runtypes_[i]->encodeData ();
  }
  id_ = htonl (id_);
  numRunTypes_ = htonl (numRunTypes_);
}

void
daqRunTypeStruct::restoreData (void)
{
  numRunTypes_ = ntohl (numRunTypes_);
  id_          = ntohl (id_);

  if (numRunTypes_ > 0) {
    for (int i = 0; i < numRunTypes_; i++) 
      runtypes_[i]->restoreData ();
  }
}

void
daqRunTypeStruct::encode (char* buffer, size_t& bufsize)
{
  int i = 0, j = 0;
  int numr = numRunTypes_;

  long realsize = sizeof (daqRunTypeStruct) - sizeof (daqArbStruct);

  // encode header
  encodeData ();
  ::memcpy (buffer, (void *)&(this->id_), realsize);
  i += realsize;

  // copy all run type information
  if (numr > 0) {
    for (j = 0; j < numr; j++) {
      size_t ss = 0;
      runtypes_[j]->encode (&(buffer[i]), ss);
      i += ss;
    }
  }
  // restore data
  restoreData ();
  bufsize = (size_t)i;
}

void
daqRunTypeStruct::decode (char* buffer, size_t size)
{
  int i = 0, j = 0;

  // clean up old information
  cleanUp ();

  long realsize = sizeof (daqRunTypeStruct) - sizeof (daqArbStruct);

  // retrieve header information and skip all others
  ::memcpy ((void *)&(this->id_), buffer, 2*sizeof (long));
  i += realsize;

  // get number of run type and id
  numRunTypes_ = ntohl (numRunTypes_);
  id_ = ntohl (id_);

  if (numRunTypes_ > 0) {
    assert (numRunTypes_ < (daqRunTypeStruct::maxNumRunTypes));

    for (j = 0; j < numRunTypes_; j++) {
      size_t ss = 0;
      runtypes_[j] = new daqRunTypeItem ();
      runtypes_[j]->decode (&(buffer[i]), ss);
      i += ss;
    }
  }
  assert (i == size);
}

void
daqRunTypeStruct::insertRunType (char* run, long runid, 
				 long inuse, char* cat)
{
  if (numRunTypes_ == (daqRunTypeStruct::maxNumRunTypes)) {
    fprintf (stderr, "daqRunTypeStruct Error:: overflow on insert\n");
    return;
  }

  daqRunTypeItem* newitem = new daqRunTypeItem (run, runid, inuse, cat);

  int i = numRunTypes_;
  runtypes_[i++] = newitem;

  numRunTypes_ ++;
}

void
daqRunTypeStruct::dumpAll (void)
{
  printf ("daqRunTypeStruct has id %d num run type %d\n", id_,
	  numRunTypes_);

  if (numRunTypes_ > 0) {
    for (int i = 0; i < numRunTypes_; i++) {
      if (runtypes_[i]->cat_)
	printf ("run type %d has %s %d %d %s\n", i,
		runtypes_[i]->name_, runtypes_[i]->runid_,
		runtypes_[i]->inuse_, runtypes_[i]->cat_);
      else
	printf ("run type %d has %s %d %d\n", i,
		runtypes_[i]->name_, runtypes_[i]->runid_,
		runtypes_[i]->inuse_);
    }
  }
}




