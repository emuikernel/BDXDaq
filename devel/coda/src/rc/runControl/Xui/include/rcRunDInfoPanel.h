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
//      RunControl Dynamic Information (Data Rate + Event Rate)
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: rcRunDInfoPanel.h,v $
//   Revision 1.5  1998/11/24 13:57:57  heyes
//   check in for Carl...
//
//   Revision 1.4  1998/11/05 20:12:22  heyes
//   reverse status updating to use UDP, fix other stuff
//
//   Revision 1.3  1998/04/08 18:31:31  heyes
//   new look and feel GUI
//
//   Revision 1.2  1997/07/08 14:59:16  heyes
//   deep trouble
//
//   Revision 1.1.1.1  1996/10/11 13:39:25  chen
//   run control source
//
//
#ifndef _RC_RUN_DINFO_PANEL_H
#define _RC_RUN_DINFO_PANEL_H

#include <stdio.h>
#include <string.h>
#include <assert.h>

#include <XcodaUi.h>
#include <rcClientHandler.h>
#include <rcMenuWindow.h>

#define RC_CACHE_SIZE 10

class rcRepCompOption;
class rcRunDInfoZoomPanel;
class rcRateDisplay;

class rcRunDInfoPanel: public XcodaUi
{
public:
  // constructor and destructor
  rcRunDInfoPanel (Widget parent, char* name, rcClientHandler& handler, Widget statusPanel_);
  virtual ~rcRunDInfoPanel (void);

  // Initialize all widgets
  void init (void);

  // redefine manage and unmanage
  virtual void manage (void);
  virtual void unmanage (void);

  // start/stop monitoring information according to component name
  void         startMonitoringInfo (char* component);
  void         stopMonitoringInfo  (char* component);

  // end of dynamic event information updating
  void         endDataTaking (void);

  // popup zoomed display of the event information
  void         zoomOnEventInfo (void);

  // popup event/data rate display
  void         popupRateDisplay (rcMenuWindow *menW);

  // config this panel according to the state
  void         config (int state);

  // class name
  virtual const char* className (void) const {return "rcRunDInfoPanel";}

protected:
  // all those callbacks
  static void updateCallback (int status, void* arg, daqNetData* data);
  static void attr5Callback (int status, void* arg, daqNetData* data);
  static void attr4Callback (int status, void* arg, daqNetData* data);
  static void attr3Callback (int status, void* arg, daqNetData* data);
  static void attr2Callback (int status, void* arg, daqNetData* data);
  static void attr1Callback (int status, void* arg, daqNetData* data);
  static void attr0Callback (int status, void* arg, daqNetData* data);
  static void timeCallback (int status, void* arg, daqNetData* data);
  static void startTimeCallback (int status, void* arg, daqNetData* data);
  static void endTimeCallback (int status, void* arg, daqNetData* data);
  static void offCallback   (int status, void* arg, daqNetData* data);

  // update event and data rate
  void        updateEventRate (void);
  void        updateDataRate  (void); 

private:
  // network handler to runcontrol server
  rcClientHandler& netHandler_;
  // all widgets
  Widget parent_;
  Widget statusPanel_;
  // Event number widget
  Widget eventNumber_;
  // Integrated event rate
  Widget iEvRate_;
  // Differential event rate
  Widget dEvRate_;
  // Integrated data rate
  Widget iDataRate_;
  // Differential data rate
  Widget dDataRate_;
  // option menu for reporting component
  rcRepCompOption* compOption_;
  // event/data rate display
  rcRateDisplay*       ratePanel_;
  // flag to tell whether one has done monitor on or not
  int              monitorOn_;
  // number of events
  unsigned long    numEvents_;
  // number of k bytes seen so far
  double           numKbytes_;
  // starting time of this experiment
  long             startTime_;
  // current time
  long             time_;
  // end time of this experiment
  long             endTime_;
  // buffers to hold past time/ev number
  long             pastTime_[RC_CACHE_SIZE];
  long             pastEvn_[RC_CACHE_SIZE];
  // buffers to hold past time/data number
  long             pastTimeB_[RC_CACHE_SIZE];
  double           pastDataB_[RC_CACHE_SIZE];
  // event number counter
  int              evc_;
  // data rate counter
  int              drc_;
};
#endif

    
