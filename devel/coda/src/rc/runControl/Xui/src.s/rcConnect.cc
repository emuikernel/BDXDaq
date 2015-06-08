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
//      Implementation of rcConnect Button
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: rcConnect.cc,v $
//   Revision 1.8  1998/05/28 17:46:54  heyes
//   new GUI look
//
//   Revision 1.7  1997/10/23 13:00:54  heyes
//   fix Alt/W
//
//   Revision 1.6  1997/10/15 16:08:27  heyes
//   embed dbedit, ddmon and codaedit
//
//   Revision 1.5  1997/09/05 12:03:51  heyes
//   almost final
//
//   Revision 1.4  1997/08/20 18:38:30  heyes
//   fix up for SunOS
//
//   Revision 1.3  1997/08/01 18:38:10  heyes
//   nobody will believe this!
//
//   Revision 1.2  1997/07/30 14:32:47  heyes
//   add more xpm support
//
//   Revision 1.1.1.1  1996/10/11 13:39:24  chen
//   run control source
//
//
#include <rcNetStatus.h>
#include <rcButtonPanel.h>
#include <rcComdOption.h>
#include <rcAudioOutput.h>
#include "rcConnect.h"
#ifdef USE_CREG
#include <codaRegistry.h>
#endif
#include "pixmaps/startup.xpm"

#define RC_CONNECT_NAME "Connect"
#define RC_CONNECT_MSG "Connect to a Server"


#if defined (_CODA_2_0_T) || defined (_CODA_2_0)
int rcConnect::maxCount_ = 100;   // 25 seconds

rcConnect::rcConnect (Widget parent, rcButtonPanel* panel, 
		      rcClientHandler& handler)
:rcXpmComdButton (parent, RC_CONNECT_NAME, NULL, RC_CONNECT_MSG, panel, handler), 
 dialog_(0), armed_ (0), autoTimerId_ (0), timeoutCount_ (0)
{
#ifdef _TRACE_OBJECTS
  printf ("               Create rcConnect Class Object\n");
#endif
  appContext_ = XtWidgetToApplicationContext (parent);

}

rcConnect::~rcConnect (void)
{
#ifdef _TRACE_OBJECTS
  printf ("               Delete rcConnect Class Object\n");
#endif
  // empty
  // dialog box will be destroyed by Xt destroy mechanism

  if (armed_)
    XtRemoveTimeOut (autoTimerId_);
  armed_ = 0;
}
#else
rcConnect::rcConnect (Widget parent, rcButtonPanel* panel, 
		      rcClientHandler& handler)
:rcComdButton (parent, RC_CONNECT_NAME, RC_CONNECT_MSG, panel, handler), 
 dialog_(0)
{
#ifdef _TRACE_OBJECTS
  printf ("               Create rcConnect Class Object\n");
#endif
  // empty
}

rcConnect::~rcConnect (void)
{
#ifdef _TRACE_OBJECTS
  printf ("               Delete rcConnect Class Object\n");
#endif
  // empty
  // dialog box will be destroyed by Xt destroy mechanism
}
#endif

void
rcConnect::doit (void)
{
  rcComdOption* option = rcComdOption::option ();

  if (option->dbasename () && option->session () && option->msqldhost ()) {
    if (connect ( ) != CODA_SUCCESS) {
      startRcServer ();
      return;
    }
    return;
  }

  if (dialog_ == 0) {
    dialog_ = new rcConnectDialog (this, "connectDialog",
				   "Open RunControl Server", netHandler_);
    dialog_->init ();
  }
  rcAudio ("Connect to run control server");
  dialog_->popup ();
}

void
rcConnect::configureCallback (int status, void* arg, daqNetData* data)
{
  rcConnect* obj = (rcConnect *)arg;

  if (status != CODA_SUCCESS)
    obj->reportErrorMsg ("Configuring a run failed !!!");
}


void
rcConnect::undoit (void)
{
  // empty
}

#if defined (_CODA_2_0_T) || defined (_CODA_2_0)
int
rcConnect::connect (void)
{
  rcComdOption* option = rcComdOption::option ();

  int status = netHandler_.connect (option->dbasename (),
			      option->session (),
			      option->msqldhost ());
  if (status != CODA_SUCCESS)
    return status;

  printf("rcConnect::connect reached\n");

  if(::getenv("DEFAULT_RUN") != 0)
  {
    rcClient& client = netHandler_.clientHandler ();

    // insert database name + session name into data object
    daqData data ("RCS", "command", "unkown");
    char* temp[2];
    temp[0] = new char[::strlen (option->dbasename ()) + 1];
    ::strcpy (temp[0], option->dbasename ());
    temp[1] = new char[::strlen (option->session ()) + 1];
    ::strcpy (temp[1], option->session ());

    data.assignData (temp, 2); //sergey ???

    // free memory
    delete []temp[0]; 
    delete []temp[1];
    
    int status = client.sendCmdCallback (DALOADDBASE, data, 
					 (rcCallback)&(rcConnect::configureCallback), 
					 (void *)this);
    if (status != CODA_SUCCESS)
    {
      reportErrorMsg ("Cannot send load database command");
      rcAudio ("can not send load database command");
    }

    daqData data2 ("RCS", "command", ::getenv("DEFAULT_RUN"));

    if (client.sendCmdCallback (DACONFIGURE, data2,
				(rcCallback)&(rcConnect::configureCallback),
				(void *)this) != CODA_SUCCESS)
    {
      reportErrorMsg ("Cannot communication with the RunControl Server\n");
	}

    if (::getenv("DEFAULT_RUN"))
    {
      char cmd[100];
      sprintf(cmd,"c:%s",::getenv("DEFAULT_RUN"));

printf("CEDIT 1\n");
#ifdef USE_CREG
      coda_send(XtDisplay(this->baseWidget()),"CEDIT",cmd);
#endif
    }

    //EditorSelectConfig(::getenv("DEFAULT_RUN"));
  }

  printf("rcConnect::connect done\n");

  return status;
}

void
rcConnect::startRcServer (void)
{
  rcComdOption* option = rcComdOption::option ();

  if (connect () != CODA_SUCCESS) { //try to create rcServer
    char msg[256];
    ::sprintf(msg,"$CODA_BIN/rcServer -m %s -d %s -s %s &\n",
	      option->msqldhost(),
	      option->dbasename (),
	      option->session ());

    int errcode = system(msg);
    if (errcode == 0) {
      timeoutCount_ = 0;
      startTimer ();
    }
    else 
      reportErrorMsg ("Starting rcServer failed due to script error");
  }
}

void
rcConnect::startTimer (void)
{
  if (!armed_) {
    armed_ = 1;
    timeoutCount_ = 0;
    // start net status window
    //stWin_->startShowingStatus ();

    // add timer
    autoTimerId_ = XtAppAddTimeOut (appContext_, 1000,
		    (XtTimerCallbackProc)&(rcConnect::autoTimerCallback),
		    (XtPointer)this);
  }
}

void
rcConnect::endTimer (void)
{
  if (armed_)
    XtRemoveTimeOut (autoTimerId_);
  armed_ = 0;
  autoTimerId_ = 0;

  // stop net status window too
  stWin_->stopShowingStatus ();
}

void
rcConnect::autoTimerCallback (XtPointer data, XtIntervalId *id)
{
  rcConnect* obj = (rcConnect *)data;

  if (obj->connect () != CODA_SUCCESS) {
    obj->timeoutCount_ ++;
    if (obj->timeoutCount_ >= rcConnect::maxCount_) {
      obj->endTimer ();
      obj->reportErrorMsg ("Cannot connect to the RunControl server");
      rcAudio ("can not connect to run control server");
    }
    else {
      obj->stWin_->showingInProgress ();
      obj->autoTimerId_ = XtAppAddTimeOut (obj->appContext_, 250,
		   (XtTimerCallbackProc)&(rcConnect::autoTimerCallback),
		   (XtPointer)obj);
    }
  }
  else 
    obj->endTimer ();
}
#endif

    
