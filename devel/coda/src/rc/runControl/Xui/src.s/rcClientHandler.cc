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
//      Implementation of rcClientHandler Class
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: rcClientHandler.cc,v $
//   Revision 1.9  2000/08/21 18:27:35  abbottd
//   Sun 5.0 C++ compiler fix
//
//   Revision 1.8  1999/10/26 19:23:54  rwm
//   Fixed misspelling of _TRACE_OBEJECTS.
//
//   Revision 1.7  1998/06/18 12:20:31  heyes
//   new GUI ready I think
//
//   Revision 1.6  1998/05/28 17:46:52  heyes
//   new GUI look
//
//   Revision 1.5  1997/10/15 16:08:25  heyes
//   embed dbedit, ddmon and codaedit
//
//   Revision 1.4  1997/09/05 12:03:49  heyes
//   almost final
//
//   Revision 1.3  1996/11/04 16:14:53  chen
//   add options for monitoring components
//
//   Revision 1.2  1996/10/31 15:57:27  chen
//   Add server message to database option
//
//   Revision 1.1.1.1  1996/10/11 13:39:23  chen
//   run control source
//
//
#include <rcPanel.h>
#include <daqCompBootStruct.h>
#include <daqMonitorStruct.h>
#include <rcAudioOutput.h>
#include "rcClientHandler.h"

#if defined (_CODA_2_0_T) || defined (_CODA_2_0)
rcClientHandler::rcClientHandler (Widget parent)
:XcodaInput (parent), status_ (DA_NOT_CONNECTED), master_ (0), 
 numComps_ (0), numClients_ (0), numDbases_ (0), numSessions_ (0),
 caboot_ (0), monParms_ (0), datalogFile_ (0), logFileDes_ (0), tokenIVal_ (0),
 handler_ (), panels_ ()
{
#ifdef _TRACE_OBJECTS
  printf ("    Create rcClientHandler Class Object\n");
#endif
}

rcClientHandler::rcClientHandler (XtAppContext context)
:XcodaInput (context), status_ (DA_NOT_CONNECTED), master_ (0),
 numComps_ (0), numClients_ (0), numDbases_ (0), numSessions_ (0),
 caboot_ (0), monParms_ (0), datalogFile_ (0), logFileDes_ (0), tokenIVal_ (0),
 handler_ (), panels_ ()
{
#ifdef _TRACE_OBJECTS
  printf ("    Create rcClientHandler Class Object\n");
#endif
}
#else
rcClientHandler::rcClientHandler (Widget parent)
:XcodaInput (parent), status_ (DA_NOT_CONNECTED), master_ (0), 
 numComps_ (0), numClients_ (0), numDbases_ (0), numSessions_ (0),
 caboot_ (0), monParms_ (0), anas_ (), handler_ (), panels_ ()
{
#ifdef _TRACE_OBJECTS
  printf ("    Create rcClientHandler Class Object\n");
#endif
}

rcClientHandler::rcClientHandler (XtAppContext context)
:XcodaInput (context), status_ (DA_NOT_CONNECTED), master_ (0),
 numComps_ (0), numClients_ (0), numDbases_ (0), numSessions_ (0),
 caboot_ (0), monParms_ (0), anas_ (), handler_ (), panels_ ()
{
#ifdef _TRACE_OBJECTS
  printf ("    Create rcClientHandler Class Object\n");
#endif
}
#endif


rcClientHandler::~rcClientHandler (void)
{
  int i;
#ifdef _TRACE_OBJECTS
  printf ("    Create rcClientHandler Class Object\n");
#endif
  if (handler_.connected ()) {
    handler_.disconnect ();
    removeInput ();
  }
  // remove all components
  for (i = 0; i < numComps_; i++)
    delete []components_[i];

#if defined (_CODA_2_0_T) || defined (_CODA_2_0)
  if (datalogFile_)
    delete []datalogFile_;
  if (logFileDes_)
    delete []logFileDes_;
#else
  // remove all ANA components information
  codaSlistIterator ite (anas_);
  daqNetData* data = 0;

  for (ite.init (); !ite; ++ite) {
    data = (daqNetData *) ite ();
    delete data;
  }
  anas_.deleteAllValues ();
#endif

  // let individual panel to remove itself from the list
  for (i = 0; i < numDbases_; i++)
    delete []dbases_[i];
  numDbases_ = 0;

  for (i = 0; i < numSessions_; i++)
    delete []sessions_[i];
  numSessions_ = 0;

  // remove component autoboot flag info
  if (caboot_)
    delete caboot_;

  // remove monitoring options
  if (monParms_)
    delete monParms_;
}

int
rcClientHandler::status (void) const
{
  return status_;
}

int
rcClientHandler::connected (void) const
{
  return handler_.connected ();
}

#if defined (_CODA_2_0_T) || defined (_CODA_2_0)
int
rcClientHandler::connect (char* database, char* exptname, char* msqld)
{
  int status = handler_.connect (database, exptname, msqld);
  if (status == CODA_SUCCESS) {
    addInput (handler_.getFd (), (XtPointer)XtInputReadMask);
    if (handler_.monitorOnCallback (exptname, "status", 
		    (rcCallback)&(rcClientHandler::statusCallback),
		    (void *)this) != CODA_SUCCESS)
      fprintf (stderr, "Cannot monitor on %s status\n", exptname);
    if (handler_.monitorOnCallback (exptname, "master",
			(rcCallback)&(rcClientHandler::mastershipCallback),
			(void *)this) != CODA_SUCCESS)
      fprintf (stderr, "Cannot monitor on %s master\n", exptname);
    if (handler_.monitorOnCallback (exptname, "online",
				(rcCallback)&(rcClientHandler::onlineCallback),
				(void *)this) != CODA_SUCCESS)
      fprintf (stderr, "Cannot monitor on %s online\n", exptname);
    if (handler_.monitorOnCallback (exptname, "updateInterval",
			(rcCallback)&(rcClientHandler::updateIntervalCbk),
			(void *)this) != CODA_SUCCESS)
      fprintf (stderr, "Cannot monitor on %s updateInterval\n", exptname);
    if (handler_.monitorOnCallback (exptname, "clientList",
			(rcCallback)&(rcClientHandler::clientsCallback),
			(void *)this) != CODA_SUCCESS)
      fprintf (stderr, "Cannot monitor on %s clientList\n", exptname);

    if (handler_.monitorOnCallback (exptname, "components",
			(rcCallback)&(rcClientHandler::componentsCallback),
			(void *)this) != CODA_SUCCESS)
      fprintf (stderr, "Cannot monitor on %s components\n", exptname);
    /* if (handler_.monitorOnCallback (exptname, "allDatabases",
				(rcCallback)&(rcClientHandler::dbaseCallback),
				(void *)this) != CODA_SUCCESS)
      fprintf (stderr, "Cannot monitor on %s allDatabases\n", exptname);    
    if (handler_.monitorOnCallback (exptname, "allSessions",
		(rcCallback)&(rcClientHandler::sessionsCallback),
		(void *)this) != CODA_SUCCESS)
      fprintf (stderr, "Cannot monitor on %s allSessions\n", exptname);
    if (handler_.monitorOnCallback (exptname, "sessionStatus",
		(rcCallback)&(rcClientHandler::sessionStatusCallback),
		(void *)this) != CODA_SUCCESS)
       fprintf (stderr, "Cannot monitor on %s sessionStatus\n", exptname); */
    if (handler_.monitorOnCallback (exptname, "compBootInfo",
		(rcCallback)&(rcClientHandler::cabootinfoCallback),
		(void *)this) != CODA_SUCCESS)
       fprintf (stderr, "Cannot monitor on %s compBootInfo\n", exptname);

    if (handler_.monitorOnCallback (exptname, "monitorParms",
		(rcCallback)&(rcClientHandler::monitorParmsCallback),
		(void *)this) != CODA_SUCCESS)
       fprintf (stderr, "Cannot monitor on %s monitorParms\n", exptname);

#if defined (_CODA_2_0_T) || defined (_CODA_2_0)
    if (handler_.monitorOnCallback (exptname, "dataFile",
	    (rcCallback)&(rcClientHandler::anaLogCallback),
	    (void *)this) != CODA_SUCCESS)
       fprintf (stderr, "Cannot monitor on %s dataFile\n", exptname);
    if (handler_.monitorOnCallback (exptname, "logFileDescriptor",
	    (rcCallback)&(rcClientHandler::logFileDesCallback),
	    (void *)this) != CODA_SUCCESS)
       fprintf (stderr, "Cannot monitor on %s logFileDescriptor\n", exptname);
    if (handler_.monitorOnCallback (exptname, "tokenInterval",
	    (rcCallback)&(rcClientHandler::tokenIntervalCallback),
	    (void *)this) != CODA_SUCCESS)
       fprintf (stderr, "Cannot monitor on %s tolenInterval\n", exptname);
    if (handler_.monitorOnCallback (exptname, "rcsMsgToDbase",
	    (rcCallback)&(rcClientHandler::rcsMsgToDbaseCbk),
	    (void *)this) != CODA_SUCCESS)
       fprintf (stderr, "Cannot monitor on %s rcsMsgToDbase\n", exptname);

#endif

    handler_.disconnectCallback ((rcCallback)&(rcClientHandler::discCallback),
				 (void *)this);
  }

  return status;
}
#else
int
rcClientHandler::connect (char* hostname, char* exptname, int exptid)
{
  int status = handler_.connect (hostname, exptname, exptid);
  if (status == CODA_SUCCESS) {
    addInput (handler_.getFd (), (XtPointer)XtInputReadMask);
    handler_.monitorOnCallback (exptname, "status", 
				(rcCallback)&(rcClientHandler::statusCallback),
				(void *)this);
    handler_.monitorOnCallback (exptname, "master",
			(rcCallback)&(rcClientHandler::mastershipCallback),
			(void *)this);
    handler_.monitorOnCallback (exptname, "online",
				(rcCallback)&(rcClientHandler::onlineCallback),
				(void *)this);
    handler_.monitorOnCallback (exptname, "updateInterval",
			(rcCallback)&(rcClientHandler::updateIntervalCbk),
			(void *)this);
    handler_.monitorOnCallback (exptname, "clientList",
			(rcCallback)&(rcClientHandler::clientsCallback),
			(void *)this);
    handler_.monitorOnCallback (exptname, "components",
			(rcCallback)&(rcClientHandler::componentsCallback),
			(void *)this);
    handler_.monitorOnCallback (exptname, "compBootInfo",
		(rcCallback)&(rcClientHandler::cabootinfoCallback),
		(void *)this);
    handler_.monitorOnCallback (exptname, "monitorParms",
		(rcCallback)&(rcClientHandler::monitorParmsCallback),
		(void *)this);

    handler_.disconnectCallback ((rcCallback)&(rcClientHandler::discCallback),
				(void *)this);
  }
  return status;
}


int
rcClientHandler::connect (char* hostname, char* exptname, int exptid,
			  rcUpdateFunc func, void* arg)
{
  int status = handler_.connect (hostname, exptname, exptid, func, arg);
  if (status == CODA_SUCCESS) {
    addInput (handler_.getFd (), (XtPointer)XtInputReadMask);
    handler_.monitorOnCallback (exptname, "status", 
				(rcCallback)&(rcClientHandler::statusCallback),
				(void *)this);
    handler_.monitorOnCallback (exptname, "master",
			(rcCallback)&(rcClientHandler::mastershipCallback),
			(void *)this);
    handler_.monitorOnCallback (exptname, "online",
				(rcCallback)&(rcClientHandler::onlineCallback),
				(void *)this);
    handler_.monitorOnCallback (exptname, "updateInterval",
			(rcCallback)&(rcClientHandler::updateIntervalCbk),
			(void *)this);
    handler_.monitorOnCallback (exptname, "clientList",
			(rcCallback)&(rcClientHandler::clientsCallback),
			(void *)this);
    handler_.monitorOnCallback (exptname, "components",
			(rcCallback)&(rcClientHandler::componentsCallback),
			(void *)this);
    handler_.monitorOnCallback (exptname, "compBootInfo",
		(rcCallback)&(rcClientHandler::cabootinfoCallback),
		(void *)this);
    handler_.monitorOnCallback (exptname, "monitorParms",
		(rcCallback)&(rcClientHandler::monitorParmsCallback),
		(void *)this);

    handler_.disconnectCallback ((rcCallback)&(rcClientHandler::discCallback),
				(void *)this);
  }
  return status;
}
#endif

void
rcClientHandler::disconnect (void)
{
  if (handler_.disconnect () == CODA_SUCCESS) {
    status_ = DA_NOT_CONNECTED;
    status (DA_NOT_CONNECTED);
  }
}

void
rcClientHandler::killServer (void)
{
  handler_.killServer ();
  status_ = DA_NOT_CONNECTED;
}

char*
rcClientHandler::exptname (void) const
{
  return handler_.exptname ();
}

rcClient&
rcClientHandler::clientHandler (void)
{
  return handler_;
}

void
rcClientHandler::input_callback (void)
{
  handler_.pendIO (0.0);
}

int
rcClientHandler::addPanel (rcPanel* panel)
{
  if (panels_.includes ((void *)panel))
    return CODA_ERROR;
  else {
    panels_.add ((void *)panel);
    return CODA_SUCCESS;
  }
}

int
rcClientHandler::removePanel (rcPanel* panel)
{
  if (!panels_.includes ((void *)panel))
    return CODA_ERROR;  
  else {
    panels_.remove ((void *)panel);
    return CODA_SUCCESS;
  }
}

void
rcClientHandler::statusCallback (int st, void* arg, daqNetData* data)
{
  rcClientHandler* obj = (rcClientHandler *)arg;
  if (st == CODA_SUCCESS) 
    obj->status_ = (int)(*data);
  else 
    obj->status_ = DA_NOT_CONNECTED;
  obj->status (obj->status_);
}

void
rcClientHandler::mastershipCallback (int status, void* arg, daqNetData* data)
{
  rcClientHandler *obj = (rcClientHandler *)arg;
  if (status == CODA_SUCCESS) {
    char *master = (char *)(*data);
    if (::strcmp (master, "unknown") == 0)
      obj->mastershipType (CODA_NOMASTER);
    else {
      // master_ value should be set by request/giveup mastership
      if (obj->master_)
	obj->mastershipType (CODA_ISMASTER);
      else
	obj->mastershipType (CODA_LOCKED);
    }
  }
}

void
rcClientHandler::onlineCallback (int status, void* arg, daqNetData* data)
{
  rcClientHandler *obj = (rcClientHandler *)arg;
  if (status == CODA_SUCCESS) {
    int online = (int)(*data);
    obj->setOnlineOption (online);
  }
}

void
rcClientHandler::updateIntervalCbk (int status, void* arg, daqNetData* data)
{
  rcClientHandler *obj = (rcClientHandler *)arg;
  if (status == CODA_SUCCESS) {
    int interval = (int)(*data);
    obj->setUpdateInterval (interval);
  }
}

#if defined (_CODA_2_0_T) || defined (_CODA_2_0)
void
rcClientHandler::dbaseCallback (int status, void* arg, daqNetData* data)
{
  rcClientHandler* obj = (rcClientHandler *)arg;

  // remove old information
  for (int i = 0; i < obj->numDbases_; i++)
    delete [](obj->dbases_[i]);
  obj->numDbases_ = 0;

  if (status == CODA_SUCCESS) {
    // do not expect more than 100 databases
    int count = RCXUI_MAX_DATABASES;
    
    if (data->getData (obj->dbases_, count) != CODA_ERROR) 
      obj->numDbases_ = count;
  }
  else 
    obj->numDbases_ = 0;
}

void
rcClientHandler::sessionsCallback (int status, void* arg, daqNetData* data)
{
  rcClientHandler* obj = (rcClientHandler *)arg;

  // remove old information
  for (int i = 0; i < obj->numSessions_; i++) 
    delete [](obj->sessions_[i]);
  obj->numSessions_ = 0;

  if (status == CODA_SUCCESS) {
    // do not expect more than sessions
    int count = RCXUI_MAX_SESSIONS;
    
    if (data->getData (obj->sessions_, count) != CODA_ERROR) 
      obj->numSessions_ = count;
  }
}

void
rcClientHandler::sessionStatusCallback (int status, void* arg, daqNetData* data)
{
  rcClientHandler* obj = (rcClientHandler *)arg;

  // remove old information
  for (int i = 0; i < RCXUI_MAX_SESSIONS; i++)
    obj->sessionActive_[i] = 0;

  if (status == CODA_SUCCESS) {
    // do not expect more than 100 databases
    int count = RCXUI_MAX_SESSIONS;
    
    if (data->getData (obj->sessionActive_, count) == CODA_ERROR) 
      obj->numSessions_ = 0;
  }
}
#endif

void
rcClientHandler::componentsCallback (int status, void* arg, daqNetData* data)
{
  rcClientHandler* obj = (rcClientHandler *)arg;

  // remove old information
  for (int i = 0; i < obj->numComps_; i++)
    delete [](obj->components_[i]);
  obj->numComps_ = 0;

  if (status == CODA_SUCCESS) {
    // do not expect more than 100 components
    int count = RCXUI_MAX_COMPONENTS;
    
    if (data->getData (obj->components_, count) != CODA_ERROR) {
      obj->numComps_ = count;
#if !defined (_CODA_2_0_T) && !defined (_CODA_2_0)
      obj->monitorDataLogFiles ();
#endif
    }
  }
  else 
    obj->numComps_ = 0;
}

void
rcClientHandler::cabootinfoCallback (int status, void* arg, daqNetData* data)
{
  rcClientHandler* obj = (rcClientHandler *)arg;

  if (obj->caboot_) {
    delete obj->caboot_;
    obj->caboot_ = 0;
  }

  daqArbStruct* tmp = (daqArbStruct *)(*data);
  // since I know the struct type, it is safe to do the following
  obj->caboot_ = (daqCompBootStruct *)tmp;

  codaSlistIterator ite (obj->panels_);
  rcPanel* panel = 0;
  
  for (ite.init(); !ite; ++ite) {
    panel = (rcPanel *) ite ();
    panel->configBoot();
  }
 
}

void
rcClientHandler::monitorParmsCallback (int status, void* arg, daqNetData* data)
{
  rcClientHandler* obj = (rcClientHandler *)arg;
  if (obj->monParms_) {
    delete obj->monParms_;
    obj->monParms_ = 0;
  }

  daqArbStruct* tmp = (daqArbStruct *)(*data);
  // since I know the struct type, it is safe to do the following
  obj->monParms_ = (daqMonitorStruct *)tmp;

  codaSlistIterator ite (obj->panels_);
  rcPanel* panel = 0;
  
  for (ite.init(); !ite; ++ite) {
    panel = (rcPanel *) ite ();
    panel->configMonParms();
  }
}
  

void
rcClientHandler::clientsCallback (int status, void* arg, daqNetData* data)
{
  rcClientHandler* obj = (rcClientHandler *)arg;

  // remove old information
  for (int i = 0; i < obj->numClients_; i++)
    delete [](obj->clients_[i]);
  obj->numClients_ = 0;

  if (status == CODA_SUCCESS) {
    // do not expect more than 100 components
    int count = RCXUI_MAX_CLIENTS;
    
    if (data->getData (obj->clients_, count) != CODA_ERROR) {
      obj->numClients_ = count;
      obj->clientsConnectionChanged ();
    }
  }
  else 
    obj->numClients_ = 0;
}

#if defined (_CODA_2_0_T) || defined (_CODA_2_0)
void
rcClientHandler::anaLogCallback (int status, void* arg, daqNetData* data)
{
  rcClientHandler* obj = (rcClientHandler *)arg;

  if (status == CODA_SUCCESS) {
    // update log file here
    if (obj->datalogFile_)
      delete []obj->datalogFile_;
    char* file = (char *)(*data);
    obj->datalogFile_ = new char[::strlen (file) + 1];
    ::strcpy (obj->datalogFile_, file);
    
    if (::strcmp (file, "unknown") != 0) 
      obj->anaLogChanged (0, 1);
    else
      obj->anaLogChanged (0, 0);
  }
  else  // disconnection, so remove data
    obj->anaLogChanged (0, 0);
}

char*
rcClientHandler::datalogFile (void) const
{
  return datalogFile_;
}

char*
rcClientHandler::logFileDescriptor (void) const
{
  return logFileDes_;
}

int
rcClientHandler::tokenInterval (void) const
{
  return tokenIVal_;
}

void
rcClientHandler::logFileDesCallback (int status, void* arg, daqNetData* data)
{
  rcClientHandler* obj = (rcClientHandler *)arg;

  if (obj->logFileDes_)
    delete []obj->logFileDes_;
  obj->logFileDes_ = 0;
  
  if (status == CODA_SUCCESS) {
    char *des = (char *)(*data);
    obj->logFileDes_ = new char[::strlen (des) + 1];
    ::strcpy (obj->logFileDes_, des);
  }
}

void
rcClientHandler::tokenIntervalCallback (int status, void* arg, 
					daqNetData* data)
{
  rcClientHandler* obj = (rcClientHandler *)arg;

  if (status == CODA_SUCCESS) {
    obj->tokenIVal_ = (int)(*data);
    obj->setTokenInterval (obj->tokenIVal_);
  }
}

void
rcClientHandler::rcsMsgToDbaseCbk (int status, void* arg,
				   daqNetData* data)
{
  rcClientHandler* obj = (rcClientHandler *)arg;

  if (status == CODA_SUCCESS) 
    obj->setRcsMsgToDbaseFlag ((int) (*data));
}
#else

void
rcClientHandler::monitorDataLogFiles (void)
{
  // add monitor on callbacks for active components
  for (int i = 0; i < numComps_; i++) {
    if (handler_.monitorOnCallback (components_[i], DYN_ANA_LOG,
			    (rcCallback)&(rcClientHandler::anaLogCallback),
			    (void *)this) != CODA_SUCCESS) {
#ifdef _CODA_DEBUG
      printf ("Cannot monitor on %s.%s\n",components_[i], DYN_ANA_LOG);
#endif
    }
    else {
#ifdef _CODA_DEBUG
      printf ("Monitor on %s.%s success\n",components_[i], DYN_ANA_LOG);
#endif
    }
  }
}

void
rcClientHandler::anaLogCallback (int status, void* arg, daqNetData* data)
{
  rcClientHandler* obj = (rcClientHandler *)arg;
  int found = 0;
  daqNetData *tdata = 0;
  codaSlistIterator ite (obj->anas_);

  if (status == CODA_SUCCESS) {
    // check whether the ANA log file exits or not
    if (::strcmp ((char *)(*data), "unknown") != 0) {
      for (ite.init (); !ite; ++ite) {
	tdata = (daqNetData *) ite ();
	if (::strcmp (data->name (), tdata->name ()) == 0) {
	  // reassigne value
	  *tdata = *data;
	  found = 1;
	  obj->anaLogChanged (tdata, 1);
	  break;
	}
      }
      if (!found) {
	tdata = new daqNetData (*data);
	obj->anas_.add ((void *)tdata);
	obj->anaLogChanged (tdata, 1);
      }
    }
  }
  else { // disconnection, so remove data
    for (ite.init (); !ite; ++ite) {
      tdata = (daqNetData *)ite ();
      if (::strcmp (data->name (), tdata->name ()) == 0) {
	found = 1;
	break;
      }
    }
    if (found) {
      obj->anas_.remove ((void *)tdata);
      obj->anaLogChanged (tdata, 0);
      delete tdata;
    }
  }
}

codaSlist& 
rcClientHandler::allAnas (void)
{
  return anas_;
}
#endif

void
rcClientHandler::anaLogChanged (daqNetData* info, int added)
{
  codaSlistIterator ite (panels_);
  rcPanel* panel = 0;

  for (ite.init(); !ite; ++ite) {
    panel = (rcPanel *) ite ();
    panel-> anaLogChanged (info, added);
  }
}

void
rcClientHandler::discCallback (int status, void* arg, daqNetData* )
{
  rcClientHandler *obj = (rcClientHandler *)arg;

  if (status == CODA_SUCCESS) {
    if (obj->status_ != DA_NOT_CONNECTED) {
      obj->status_ = DA_NOT_CONNECTED;
      obj->status (obj->status_);
    }
    obj->removeInput ();
  }
  rcAudio ("Disconnected from server");
}

void
rcClientHandler::reqMastershipCbk (int status, void* arg, daqNetData* data)
{
  rcClientHandler *obj = (rcClientHandler *)arg;

  if (status == CODA_SUCCESS) 
    obj->master_ = 1;
  else
    obj->master_ = 0;
}

void
rcClientHandler::giveupMastershipCbk (int status, void* arg, daqNetData* data)
{
  rcClientHandler *obj = (rcClientHandler *)arg;

  if (status == CODA_SUCCESS) 
    obj->master_ = 0;
  else
    obj->master_ = 1;
}

int
rcClientHandler::requestMastership (void)
{
  daqData data (handler_.exptname(), "command", (int)DABECOMEMASTER);
  if (handler_.sendCmdCallback (DABECOMEMASTER, data,
			(rcCallback)&(rcClientHandler::reqMastershipCbk),
			(void *)this) != CODA_SUCCESS) {
    fprintf (stderr, "Cannot send command DABECOMEMASTER to the server\n");
    return CODA_ERROR;
  }
  else {
    master_ = 1;
    return CODA_SUCCESS;
  }
}

int
rcClientHandler::giveupMastership (void)
{
  daqData data (handler_.exptname(), "command", (int)DACANCELMASTER);
  if (handler_.sendCmdCallback (DACANCELMASTER, data,
			(rcCallback)&(rcClientHandler::giveupMastershipCbk),
			(void *)this) != CODA_SUCCESS) {
    fprintf (stderr, "Cannot send command DACANCELMASTER to the server\n");
    return CODA_ERROR;
  }
  else {
    master_ = 0;
    return CODA_SUCCESS;
  }
}
  

void
rcClientHandler::status (int st)
{
  codaSlistIterator ite (panels_);
  rcPanel* panel = 0;

  for (ite.init(); !ite; ++ite) {
    panel = (rcPanel *) ite ();
    panel->config (st);
  }
}

void
rcClientHandler::mastershipType (int type)
{
  codaSlistIterator ite (panels_);
  rcPanel* panel = 0;

  for (ite.init(); !ite; ++ite) {
    panel = (rcPanel *) ite ();
    panel-> configMastership (type);
  }
}

void
rcClientHandler::setOnlineOption (int online)
{
  codaSlistIterator ite (panels_);
  rcPanel* panel = 0;

  for (ite.init(); !ite; ++ite) {
    panel = (rcPanel *) ite ();
    panel-> configOnlineOption (online);
  }
}

void
rcClientHandler::setUpdateInterval (int interval)
{
  codaSlistIterator ite (panels_);
  rcPanel* panel = 0;

  for (ite.init(); !ite; ++ite) {
    panel = (rcPanel *) ite ();
    panel-> configUpdateInterval (interval);
  }
}

int
rcClientHandler::anaLogToRealFiles (void)
{
#if defined (_CODA_2_0_T) || defined (_CODA_2_0)
  if (datalogFile_ && ::strcmp (datalogFile_, "unknown") != 0)
    return 1;
  return 0;
#else
  codaSlistIterator ite (anas_);
  daqNetData* data = 0;

  for (ite.init(); !ite; ++ite) {
    data = (daqNetData *) ite ();
    if (::strcmp ((char *)(*data), "NOLOG") != 0 &&
	::strcmp ((char *)(*data), "unknown") != 0)
      return 1;
  }
  return 0;
#endif
}

void
rcClientHandler::clientsConnectionChanged (void)
{
  codaSlistIterator ite (panels_);
  rcPanel* panel = 0;

  for (ite.init(); !ite; ++ite) {
    panel = (rcPanel *) ite ();
    panel->clientsConnectionConfig (clients_, numClients_);
  }
}

int
rcClientHandler::numConnectedClients (void) const
{
  return numClients_;
}

char**
rcClientHandler::connectedClients (int& num)
{
  num = numClients_;
  return clients_;
}

#if defined (_CODA_2_0) || defined (_CODA_2_0_T)
int
rcClientHandler::numDatabases (void) const
{
  return numDbases_;
}

char**
rcClientHandler::databases (int& num)
{
  num = numDbases_;
  return dbases_;
}

int
rcClientHandler::numSessions (void) const
{
  return numSessions_;
}

char**
rcClientHandler::sessions (int& num)
{
  num = numSessions_;
  return sessions_;
}

int*
rcClientHandler::sessionActive (int& num)
{
  num = numSessions_;
  return sessionActive_;
}

void
rcClientHandler::setTokenInterval (int interval)
{
  codaSlistIterator ite (panels_);
  rcPanel* panel = 0;

  for (ite.init(); !ite; ++ite) {
    panel = (rcPanel *) ite ();
    panel-> configTokenInterval (interval);
  }
}

void
rcClientHandler::setRcsMsgToDbaseFlag (int state)
{
  codaSlistIterator ite (panels_);
  rcPanel* panel = 0;

  for (ite.init(); !ite; ++ite) {
    panel = (rcPanel *) ite ();
    panel-> configRcsMsgToDbase (state);
  }
}
#endif


int
rcClientHandler::numComponents (void) const
{
  return numComps_;
}

char**
rcClientHandler::components (int& num)
{
  num = numComps_;
  return components_;
}

long
rcClientHandler::compBootInfo (char** &comps, long* &autoboot)
{
  if (caboot_)
    return caboot_->compBootInfo (comps, autoboot);
  else {
    comps = 0;
    autoboot = 0;
    return 0;
  }
}

long
rcClientHandler::monitorParms (char** &comps, long* &monitored,
			       long& autoend, long& interval)
{
  if (monParms_) {
    return monParms_->monitorParms (comps, monitored, autoend, interval);
  } else {
    comps = 0;
    monitored = 0;
    autoend = 1;
    interval = 10;
    return 0;
  }
}



