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
//      run control server/client exchange messages
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: rcMsg.cc,v $
//   Revision 1.1.1.1  1996/10/11 13:39:31  chen
//   run control source
//
//
#include "rcMsg.h"

//============================================================================
//               Implementation of rcMsg
//============================================================================
rcMsg::rcMsg (long type, daqNetData& data, long reqId)
:type_ (type), data_ (data), arg_ (reqId), unused_ (0)
{
#ifdef _TRACE_OBJECTS
  printf ("Create rcMsg Class Object\n");
#endif
  size_ = data.size();
}

rcMsg::rcMsg (const rcMsg& msg)
:type_ (msg.type_), data_ (msg.data_), arg_ (msg.arg_), size_ (msg.size_),
 unused_ (0)
{
#ifdef _TRACE_OBJECTS
  printf ("Create rcMsg Class Object\n");
#endif
}

rcMsg&
rcMsg::operator = (const rcMsg& msg)
{
  if (this != &msg) {
    type_ = msg.type_;
    arg_ = msg.arg_;
    size_ = msg.size_;
    unused_ = 0;
    data_ = msg.data_;
  }
  return *this;
}

rcMsg::~rcMsg (void)
{
#ifdef _TRACE_OBJECTS
  printf ("Delete rcMsg Class Object\n");
#endif
}

long
rcMsg::type (void) const
{
  return type_;
}

long
rcMsg::dataSize (void) const
{
  return size_;
}

long
rcMsg::reqId (void) const
{
  return arg_;
}

void
rcMsg::encode (void)
{
  // data_ of daqNetData will be performed separatly
  type_ = htonl (type_);
  arg_  = htonl (arg_);
  size_ = htonl (size_);
  unused_ = htonl (unused_);
}

void
rcMsg::decode (void)
{
  // data_ of daqNetData will be performed separatly
  type_ = ntohl (type_);
  arg_  = ntohl (arg_);
  size_ = ntohl (size_);
  unused_ = ntohl (unused_);
}

rcMsg::operator daqNetData& (void)
{
  return data_;
}

int
operator << (SOCK_Stream& out, rcMsg& msg)
{
  char *buffer = 0;
  long bufsize = 0;
  int  n = 0;
  iovec iovp[2];

  // Note: data_ stays the same
  encodeNetData (msg.data_, buffer, bufsize);
  // encode header
  msg.encode ();

  iovp[0].iov_base = (char *)&msg;
  iovp[0].iov_len = RCMSG_PREDATA_SIZE;

  iovp[1].iov_base = buffer;
  iovp[1].iov_len = bufsize;
  n = out.send (&(iovp[0]), (size_t)2);
  // free buffer memory
  delete []buffer;
  return n;
}

int
operator >> (SOCK_Stream& in, rcMsg& msg)
{
  int size = 0;
  int n = in.recv_n (&msg, RCMSG_PREDATA_SIZE);
  if (n != RCMSG_PREDATA_SIZE){
#ifdef _CODA_DEBUG
    printf ("Receiving error for >> operator\n");
#endif
    return -1;
  }
  size += n;
  // decode the header
  msg.decode ();

  char *buffer = new char[msg.size_];
  n = in.recv_n (buffer, msg.size_);
  if (n != msg.size_) {
#ifdef _CODA_DEBUG
    printf ("Receiving error for >> operator in retrieving netData\n");
#endif
    return -1;
  }
  decodeNetData (msg.data_, buffer, msg.size_);
  size += n;
  delete []buffer;
  return size;
}

int
operator << (int out, rcMsg& msg)
{
  char *buffer = 0;
  long bufsize = 0;
  int  n = 0;
  iovec iovp[2];

  // Note: data_ stays the same
  encodeNetData (msg.data_, buffer, bufsize);
  // encode header
  msg.encode ();

  iovp[0].iov_base = (char *)&msg;
  iovp[0].iov_len = RCMSG_PREDATA_SIZE;

  iovp[1].iov_base = buffer;
  iovp[1].iov_len = bufsize;
  n = ::writev (out, &(iovp[0]), (size_t)2);
  // free buffer memory
  delete []buffer;
  return n;
}

int
operator >> (int in, rcMsg& msg)
{
  int size = 0;
  int n = read (in, &msg, RCMSG_PREDATA_SIZE);
  if (n != RCMSG_PREDATA_SIZE){
#ifdef _CODA_DEBUG
    printf ("Receiving error for >> operator\n");
#endif
    return -1;
  }
  size += n;
  // decode the header
  msg.decode ();

  char *buffer = new char[msg.size_];
  n = read (in, buffer, msg.size_);
  if (n != msg.size_) {
#ifdef _CODA_DEBUG
    printf ("Receiving error for >> operator in retrieving netData\n");
#endif
    return -1;
  }
  decodeNetData (msg.data_, buffer, msg.size_);
  size += n;
  delete []buffer;
  return size;
}
