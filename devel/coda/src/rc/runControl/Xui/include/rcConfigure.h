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
//      RunControl Configure Button
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: rcConfigure.h,v $
//   Revision 1.3  1997/07/30 15:32:27  heyes
//   clean for Solaris
//
//   Revision 1.2  1997/07/30 14:32:46  heyes
//   add more xpm support
//
//   Revision 1.1.1.1  1996/10/11 13:39:26  chen
//   run control source
//
//
#ifndef _RC_CONFIGURE_H
#define _RC_CONFIGURE_H

#include <rcComdButton.h>

class rcButtonPanel;

class rcConfigure: public rcComdButton
{
public:
  // constructor and destructor
  rcConfigure  (Widget parent, rcButtonPanel* panel, rcClientHandler& handler);
  ~rcConfigure (void);

  // class name
  virtual const char* className (void) const {return "rcConfigure";}
  virtual void doit   (void);

protected:
  // inherited operations
  virtual void undoit (void);

#if defined (_CODA_2_0_T) || defined (_CODA_2_0)
  // load database
  void   loadRcDbase (char *dbase, char* session);
  static void loadRcDbaseCbk (int status, void* arg, daqNetData* data);
#endif
};

extern "C" void doitCbk(rcConfigure *);

#endif
