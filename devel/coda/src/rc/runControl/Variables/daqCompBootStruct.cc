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
//      CODA run control: component auto boot values
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: daqCompBootStruct.cc,v $
//   Revision 1.3  1997/08/25 16:02:02  heyes
//   fix some display problems
//
//   Revision 1.2  1996/12/04 18:32:21  chen
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
#include "daqCompBootStruct.h"

int daqCompBootStruct::maxNumComps = 100;
int daqCompBootStruct::maxCompNameLen = 80;

daqCompBootStruct::daqCompBootStruct (void)
:daqArbStruct (), id_ (daqArbStructFactory::BOOT_INFO), numComponents_ (0)
{
#ifdef _TRACE_OBJECTS
  printf ("    Create daqCompBootStruct Class Object\n");
#endif
  autoboot_ = new long[daqCompBootStruct::maxNumComps];
  components_ = new char*[daqCompBootStruct::maxNumComps];

  for (int i = 0; i < daqCompBootStruct::maxNumComps; i++) {
    autoboot_[i] = 0;
    components_[i] = 0;
  }
}

daqCompBootStruct::~daqCompBootStruct (void)
{
#ifdef _TRACE_OBJECTS
  printf ("    Delete daqCompBootStruct Clas Object\n");
#endif

  for (int i = 0; i < numComponents_; i++)
    delete []components_[i];

  delete []autoboot_;
  delete []components_;
}

daqArbStruct*
daqCompBootStruct::dup (void)
{
  daqCompBootStruct* tmp = new daqCompBootStruct ();

  if (numComponents_ > 0) {
    for (int i = 0; i < numComponents_; i++) {
      tmp->autoboot_[i] = autoboot_[i];
      tmp->components_[i] = new char[daqCompBootStruct::maxCompNameLen];
      ::strcpy (tmp->components_[i], components_[i]);
    }
    tmp->numComponents_ = numComponents_;
  }

  tmp->id_ = id_;

  return tmp;
}

void
daqCompBootStruct::insertInfo (char* component, int autoboot)
{
  if (numComponents_ == (daqCompBootStruct::maxNumComps)) 
    fprintf (stderr, "daqCompBootStruct Error: overflow on insert\n");

  int i = numComponents_;
  components_[i] = new char[daqCompBootStruct::maxCompNameLen];
  ::strcpy (components_[i], component);
  autoboot_[i] = autoboot;

  numComponents_ ++;
}

long
daqCompBootStruct::compBootInfo (char** &components, long* &autoboot)
{
  if (numComponents_ > 0) {
    components = components_;
    autoboot = autoboot_;
  }
  else {
    components = 0;
    autoboot = 0;
  }
  return numComponents_;
}

void
daqCompBootStruct::cleanUp (void)
{
  if (numComponents_ > 0) {
    for (int i = 0; i < numComponents_; i++)
      delete []components_[i];
  }
  numComponents_ = 0;
}    

void
daqCompBootStruct::encodeData (void)
{
  if (numComponents_ > 0) {
    for (int i = 0; i < numComponents_; i++)
      autoboot_[i] = htonl (autoboot_[i]);
  }
  numComponents_ = htonl (numComponents_);
  id_ = htonl (id_);
}

void
daqCompBootStruct::restoreData (void)
{
  numComponents_ = ntohl (numComponents_);
  id_ = ntohl (id_);

  if (numComponents_ > 0) {
    for (int i = 0; i < numComponents_; i++)
      autoboot_[i] = ntohl (autoboot_[i]);
  }
}

void
daqCompBootStruct::encode (char* buffer, size_t& bufsize)
{
  int    i = 0, j = 0;
  int    numComps = numComponents_;

  long realsize = sizeof (daqCompBootStruct) - sizeof (daqArbStruct);


  // encode information data
  encodeData ();
  ::memcpy (buffer, (void *)&(this->id_), realsize);
  i += realsize;

  // copy autoboot to the buffer
  ::memcpy (&(buffer[i]), (void *)autoboot_, numComps*sizeof (long));
  i += numComps*sizeof (long);

  // copy all components to the buffer
  for (j = 0; j < numComps; j++) {
    ::memcpy (&(buffer[i]), (void *)components_[j], 
    daqCompBootStruct::maxCompNameLen);
    i = i + daqCompBootStruct::maxCompNameLen;
  }
  // restore data
  restoreData ();

  bufsize = (size_t)i;
}

void
daqCompBootStruct::decode (char* buffer, size_t size)
{
  int i = 0;
  int j = 0;
  
  // clean up old information
  cleanUp ();

  long realsize = sizeof (daqCompBootStruct) - sizeof (daqArbStruct);
  
  // copy header information
  ::memcpy ((void *)&(this->id_), buffer, 2*sizeof (long));
  // skip all other elements
  i += realsize;
  // get number of components value
  numComponents_ = ntohl (numComponents_);
  // get id number
  id_ = ntohl (id_);

  if (numComponents_) {
    // make sure number of components < maximum number of components
    assert (numComponents_ < (daqCompBootStruct::maxNumComps));

    // copy auto boot information and convert to native byte order
    ::memcpy ((void *)autoboot_, &(buffer[i]), numComponents_*sizeof (long));

    for (j = 0; j < numComponents_; j++) 
      autoboot_[j] = ntohl (autoboot_[j]);

    i += numComponents_*sizeof (long);

    // copy components name info
    for (j = 0; j < numComponents_; j++) {
      components_[j] = new char[(daqCompBootStruct::maxCompNameLen)];
      ::memcpy ((void *)components_[j], &(buffer[i]),
      daqCompBootStruct::maxCompNameLen);
      i = i + daqCompBootStruct::maxCompNameLen;
    }
  }
  
  assert (i == size);
}

size_t
daqCompBootStruct::size (void)
{
  size_t s = 0;

  long realsize = sizeof (daqCompBootStruct) - sizeof (daqArbStruct);

  s += realsize;

  if (numComponents_ > 0) {
    s = s + sizeof (long)*numComponents_;
    s = s + numComponents_*(daqCompBootStruct::maxCompNameLen);
  }
  return s;
}

long
daqCompBootStruct::id (void)
{
  return id_;
}

void
daqCompBootStruct::dumpAll (void)
{
  for (int i = 0; i < numComponents_; i++) {
    printf ("Component %s boot info %d\n", components_[i],
	    autoboot_[i]);
  }
}



  
  
