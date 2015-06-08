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
//      RunControl event/data rate display header file
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: rcRateDisplay.h,v $
//   Revision 1.4  1998/11/24 13:57:55  heyes
//   check in for Carl...
//
//   Revision 1.3  1998/05/28 17:47:03  heyes
//   new GUI look
//
//   Revision 1.2  1998/04/08 18:31:27  heyes
//   new look and feel GUI
//
//   Revision 1.1.1.1  1996/10/11 13:39:28  chen
//   run control source
//
//
#ifndef _RC_RATE_DISPLAY_H
#define _RC_RATE_DISPLAY_H

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <XcodaUi.h>
#include <rcClientHandler.h>
#include <rcMenuWindow.h>
#include <Xm/Scale.h>
class rcRateDisplay : public XcodaUi
{
public:
  // constructor and destructor
  rcRateDisplay  (Widget parent, rcClientHandler& handler, Widget status, char* name, char* title,
		  int    buffersize = 100);
  ~rcRateDisplay (void);

  void init (rcMenuWindow *menW);
  // operations
  // add event rate data to the display
  void addEvRateData (double data);
  // add data rate data to the display
  void addDataRateData (double data);

  /*sergey*/
  void addLivetimeData (double data);

  void addRatioData (double data);
  // redefine popup
  void popup (void);

  // reset display to initial display
  void         cleanDisplay (void);

  void sendChanged (char *what,int newval);
  void sendMonitorInfo (daqMonitorStruct* info);
  void manage();
  void unmanage();
  void setUpdateRate(int rate);

protected:

  // reset the plots
  void         resetDisplay       (void);
  
  // callback function for dismiss button
  static void dismissCallback (Widget w, XtPointer data,
			       XmAnyCallbackStruct* cbs);

  static void changedValueCallback (Widget w, XtPointer data,
			       XmScaleCallbackStruct* cbs);

  static void setCallback (int status, void* arg, daqNetData* );

  static void offCallback (int status, void *, daqNetData* data);

  static void compCallback (int status, void* arg, daqNetData* data);

private:
  char *title_;
  // network handler
  rcClientHandler& netHandler_;
  // parent widget
  Widget parent_;  
  Widget statusPanel_;
  // graph widgets
  Widget evrate_;
  Widget datarate_;
  Widget ratio_;
  Widget other_;
  Widget rates_;
  Widget toggles_[100];
  Widget toggles2_[100];
  // list id for above widgets
  int    evlist_;
  int    datalist_;
  int    ratiolist_;

  int    lvlist_; //sergey

  int    revlist_;
  int    rdatalist_;
  int    rratiolist_;
  int    updateRate_;

  // x and y value for event rate
  double  *evx_;
  double  *evy_;
  // x and y values for data rate
  double  *dx_;
  double  *dy_;

  // x and y values for ratio
  double  *rx_;
  double  *ry_;

  //sergey: x and y values for livetime
  double  *lvx_;
  double  *lvy_;

  // number of data in the buffer
  int     nume_;
  int     numd_;
  int     numl_; //sergey
  int     numr_;
  // number of times the evrate/data rate exceeds the buffer
  int     etime_;
  int     dtime_;
  int     rtime_;
  // size of buffer
  int     size_;

  //sergey: variables used to averate rates
  time_t time0, time1;
  int ibin0, ibin1;
  double evy_average;

  // number of widgets to map
  int      num_;
  // name of components
  char*    names_[RCXUI_MAX_COMPONENTS];

};

#endif


