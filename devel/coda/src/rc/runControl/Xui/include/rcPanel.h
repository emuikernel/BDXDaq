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
//      Abstract Class of runControl Panel that has response to status change
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: rcPanel.h,v $
//   Revision 1.3  1998/05/28 17:47:01  heyes
//   new GUI look
//
//   Revision 1.2  1996/10/31 15:57:32  chen
//   Add server message to database option
//
//   Revision 1.1.1.1  1996/10/11 13:39:24  chen
//   run control source
//
//
#ifndef _RC_PANEL_H
#define _RC_PANEL_H

#include <stdio.h>
#include <string.h>
#include <assert.h>

class daqNetData;

class rcPanel
{
public:
  // destructor
  virtual ~rcPanel (void);

  // panel response to status change
  virtual void config (int status) = 0;

  // panel response to mastership change
  virtual void configMastership (int type);

  // panel response to online option changes
  virtual void configOnlineOption (int option);

  // panel response to update interval changes
  virtual void configUpdateInterval (int interval);

#if defined (_CODA_2_0_T) || defined (_CODA_2_0)
  // panel response to rcs message to interval changes
  virtual void configRcsMsgToDbase  (int yes);
  // panel response to token interval changes
  virtual void configTokenInterval  (int interval);
#endif
  
  // panel response to ANA log file changes
  // second argument added = 1  ==> new or updated log file
  // second arrument added = 0  ==> deleted ANA
  virtual void anaLogChanged (daqNetData* info, int added);

  // number of clients connected to the runcontrol server configuration.
  // do not free memory assocaited with each clients[i].
  virtual void clientsConnectionConfig (char** clients, int numClients);

  virtual void configBoot();
  virtual void configMonParms();
protected:
  // constructor to deny direct access
  rcPanel (void);
};
#endif
