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
//      RunControl Load Database Command
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: rcLoad.h,v $
//   Revision 1.1.1.1  1996/10/11 13:39:26  chen
//   run control source
//
//
#ifndef _RC_LOAD_H
#define _RC_LOAD_H

#include <rcComdButton.h>

class rcLoadDialog;
class rcButtonPanel;
#if defined (_CODA_2_0_T) || defined (_CODA_2_0)
class rcSessionDialog;
#endif

class rcLoad: public rcComdButton
{
public:
  // constructor and destructor
  rcLoad  (Widget parent, rcButtonPanel* panel, rcClientHandler& handler);
  ~rcLoad (void);

  // operations
#if defined (_CODA_2_0_T) || defined (_CODA_2_0)
  void loadRcDbase (char* dbase, char* session);
#else
  void loadRcDbase (char* dbase);
#endif

  // class name
  virtual const char* className (void) const {return "rcLoad";}

protected:
  // inherited operations
  virtual void doit   (void);
  virtual void undoit (void);
  // command callback
  static void loadCallback (int status, void* arg, daqNetData* data);

private:
#if !defined (_CODA_2_0_T) && !defined (_CODA_2_0)
  rcLoadDialog* dialog_;
#endif
};
#endif
