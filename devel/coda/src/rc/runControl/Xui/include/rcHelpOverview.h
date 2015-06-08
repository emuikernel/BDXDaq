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
//      RunControl Help Overview Command
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: rcHelpOverview.h,v $
//   Revision 1.2  1996/12/04 18:32:28  chen
//   port to 1.4 on hp and ultrix
//
//   Revision 1.1.1.1  1996/10/11 13:39:23  chen
//   run control source
//
//
#ifndef _RC_HELP_OVERVIEW_H
#define _RC_HELP_OVERVIEW_H

#include <codaCbtnComd.h>
#include <rcClientHandler.h>

class rcInfoPanel;
class rcHelpContext;

class rcHelpOverview: public codaCbtnComd
{
public:
  // constructor and destructor
  rcHelpOverview (char* name, int active);
  virtual ~rcHelpOverview (void);

  // return base widget for dialog
  const Widget dialogBaseWidget (void);

  // setup information panel
  void infoPanel (rcInfoPanel* panel);

  // setup all help context
  void setHelpContext (void);

  // configuration for different state
  void config (int state);

  // class name
  virtual const char *className (void) const {return "rcHelpOverview";}

private:
  // run control information panel
  rcInfoPanel* infoPanel_;

  // all those real help command
#if defined (_CODA_2_0_T) || defined (_CODA_2_0)
  rcHelpContext* connect_;
  rcHelpContext* disconnect_;
  rcHelpContext* close_;
  rcHelpContext* exit_;
  rcHelpContext* master_;
  rcHelpContext* config_;
  rcHelpContext* download_;
  rcHelpContext* reset_;
  rcHelpContext* evlimit_;
  rcHelpContext* runnum_;
  rcHelpContext* repbug_;
#else
  rcHelpContext* connect_;
  rcHelpContext* disconnect_;
  rcHelpContext* close_;
  rcHelpContext* exit_;
  rcHelpContext* load_;  
  rcHelpContext* master_;
  rcHelpContext* config_;
  rcHelpContext* download_;
  rcHelpContext* reset_;
  rcHelpContext* evlimit_;
  rcHelpContext* runnum_;
  rcHelpContext* script_;  
  rcHelpContext* repbug_;
#endif  
};
#endif
