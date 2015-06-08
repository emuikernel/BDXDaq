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
//      RunControl Automatic Start Button
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: rcAuto.h,v $
//   Revision 1.1.1.1  1996/10/11 13:39:27  chen
//   run control source
//
//
#ifndef _RC_AUTO_H
#define _RC_AUTO_H

#include <rcComdButton.h>

class rcAuto: public rcComdButton
{
public:
  // constructor and destructor
  rcAuto (Widget parent, rcButtonPanel* panel,
	  rcClientHandler& handler);
  ~rcAuto (void);

  // class name
  virtual const char* className (void) const {return "rcAuto";}

protected:
  // inherited function
  virtual void doit   (void);
  virtual void undoit (void);

  // callback for auto start
  static void autoStartCallback (int status, void* arg, daqNetData* data);
};
#endif


    
    
