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
//      Implementation of rcs message to database preference
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: rcRcsMsgToDbase.cc,v $
//   Revision 1.1  1996/10/31 15:58:14  chen
//   server message to database option
//
//
//
#include "rcRcsMsgToDbase.h"

rcRcsMsgToDbase::rcRcsMsgToDbase (char* name, int active,
				  char* acc, char* acc_text,
				  int state,
				  rcClientHandler& handler)
:rcMenuTogComd (name, active, acc, acc_text, state, handler)
{
#ifdef _TRACE_OBJECTS
  printf ("               Create rcRcsMsgToDbase class object\n");
#endif
}

rcRcsMsgToDbase::~rcRcsMsgToDbase (void)
{
#ifdef _TRACE_OBJECTS
  printf ("               Delete rcRcsMsgToDbase class object\n");
#endif
}

void
rcRcsMsgToDbase::doit (void)
{
  rcClient& client = netHandler_.clientHandler ();
  if (state () > 0) {  // after button pressed
    daqData data (netHandler_.exptname (), "rcsMsgToDbase", 1);
    client.setValueCallback (data, 
			     &(rcRcsMsgToDbase::rcsMsgToDbaseCbk0),
			     (void *)this);
  }
  else {
    daqData data (netHandler_.exptname (), "rcsMsgToDbase", 0);
    client.setValueCallback (data, 
			     &(rcRcsMsgToDbase::rcsMsgToDbaseCbk1),
			     (void *)this);
  }
}

void
rcRcsMsgToDbase::undoit (void)
{
  // empty
}

void
rcRcsMsgToDbase::rcsMsgToDbaseCbk0 (int status, void* arg, daqNetData* data)
{
  rcRcsMsgToDbase* obj = (rcRcsMsgToDbase *)arg;
  if (status != CODA_SUCCESS) 
    obj->setState (0);
}

void
rcRcsMsgToDbase::rcsMsgToDbaseCbk1 (int status, void* arg, daqNetData* data)
{
  rcRcsMsgToDbase* obj = (rcRcsMsgToDbase *)arg;
  if (status != CODA_SUCCESS) 
    obj->setState (1);
}

