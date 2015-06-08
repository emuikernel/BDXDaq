//-----------------------------------------------------------------------------
// Copyright (c) 1991,1992 Southeastern Universities Research Association,
//                         Continuous Electron Beam Accelerator Facility
//
// This software was developed under a United States Government license
// described in the NOTICE file included as part of this distribution.
//
//-----------------------------------------------------------------------------
//
// Description:
//      CODA RunControl Server Client Interface
//
// Author:  Jie Chen
//
// Revision History:
//   $Log: rcClient.h,v $
//   Revision 1.5  1997/09/05 12:03:57  heyes
//   almost final
//
//   Revision 1.4  1997/08/25 16:02:06  heyes
//   fix some display problems
//
//   Revision 1.3  1996/11/04 16:14:58  chen
//   add options for monitoring components
//
//   Revision 1.2  1996/10/31 18:09:28  chen
//   server message to database as an option
//
//   Revision 1.1.1.1  1996/10/11 13:39:32  chen
//   run control source
//
//
#ifndef _CODA_RCCLIENT_H
#define _CODA_RCCLIENT_H

#include <stdio.h>
#include <string.h>
#include <assert.h>

#include <rcSvcProtocol.h>

#include <Event_Handler.h>
#include <Reactor.h>
#include <INET_Addr.h>
#include <SOCK_Connector.h>
#include <SOCK_Stream.h>
#include <rcServerLocater.h>

#include <codaRcCallback.h>
#include <codaIntHash.h>
#include <rccDaqData.h>
#include <daqDataManager.h>
#include <rcMsg.h>
#include <codaSlist.h>

// default client side timeout for synchronous call
const double CODA_RC_TIMEOUT = 4.0;

class rcClientLocker;

class rcClient: public Event_Handler
{
public:
  // constructor and destructor
  rcClient  (void);
  ~rcClient (void);

  // client interface
#if defined (_CODA_2_0_T) || defined (_CODA_2_0)
  //=======================================================================
  // int connect (char* database, char* exptname)
  //     Try to establish connection to server with name exptname
  //     database name 'database'
  //     msql host may be infulenced by the env MSQL_TCP_PORT
  //     return CODA_SUCCESS for success
  //     return anything else for failure
  //=======================================================================
  int connect (char* database, char* exptname, char* msql = 0);
#endif


  //=======================================================================
  // int connect (char* hostname, char* exptname, int exptid);
  //     Try to establish connection to server
  //     return CODA_SUCCESS for success
  //     return anything else for failure
  //     note: CODA_RC_PORT and EXPID environment variables
  //           can effect seaching process
  //=======================================================================
  int connect (char* hostname, char* exptname, int exptid);

  //=======================================================================
  // int connect (char* hostname, char* exptname, int exptid,
  //              rcUpdateFunc func, void* arg);
  //     Try to establish connection to server
  //     with external updating function func and argument arg
  //     return CODA_SUCCESS for success
  //     return anything else for failure
  //     note: CODA_RC_PORT and EXPID environment variables
  //           can effect seaching process
  //=======================================================================
  int connect (char* hostname, char* exptname, int exptid,
	       rcUpdateFunc func, void* arg);
  

  //=======================================================================
  // int connected (void)
  //     return 1: connected
  //     return 0: not connected
  //=======================================================================
  int connected (void) const;

  //=======================================================================
  // int disconnect (void)
  //     return CODA_SUCCESS: for successful disconnection
  //     return anything else: for failure
  //=======================================================================
  int disconnect (void);

  //======================================================================
  // void killServer (void)
  //     kill the runControl Server
  //======================================================================
  void killServer (void);

  //=======================================================================
  // int disconnectCallback (rcCallback callback, void* arg)
  //      return CODA_SUCCESS: for registering disconnect callback
  //      return anything else: for failure
  //======================================================================
  int disconnectCallback (rcCallback callback, void* arg);

  //=======================================================================
  // int getFd (void) const
  //     return file descriptor of socket connection
  //=======================================================================
  int getFd (void) const;

  //=======================================================================
  // int pendIO (double seconds)
  //     Wait until outstanding event occurs
  //     seconds = 0.0 polling
  //     return CODA_SUCCESS: success
  //     return CODA_WARNING: timeout
  //     return CODA_ERROR: network error
  //=======================================================================
  int pendIO (double seconds);

  //=======================================================================
  // int pendIO (void)
  //     Wait forever for an event
  //     return CODA_SUCCESS: success
  //     return CODA_ERROR: network error
  //=======================================================================
  int pendIO (void);

  //=======================================================================
  // int sendCmdCbk (int cmd, daqData& data, rcCallback callback, void* arg)
  //     Send a command and callback to server
  //     return CODA_SUCCESS: success
  //     return CODA_ERROR: network error
  //=======================================================================
  int sendCmdCallback (int cmd, daqData& data,
		       rcCallback callback, void* arg);

  //=======================================================================
  // int getValueCallback (char* name, char* attr,
  //                       rcCallback callback, void* arg)
  //     send command get value with callback
  //     return CODA_SUCCESS: success
  //     return CODA_ERROR: network error
  //=======================================================================
  int getValueCallback (char* compname,
			char* attribute,
			rcCallback callback,
			void* arg);

  //=======================================================================
  // int setValueCallback (daqData& data,
  //                       rcCallback callback, void* arg)
  //     send command set value with callback
  //     return CODA_SUCCESS: success
  //     return CODA_ERROR: network error
  //=======================================================================
  int setValueCallback (daqData& data,
			rcCallback callback,
			void* arg);

  //=======================================================================
  // int monitorOnCallback (char* compname, char* attribute,
  //                       rcCallback callback, void* arg)
  //     monitor on a value with a callback
  //     return CODA_SUCCESS: success
  //     return CODA_ERROR: network error
  //=======================================================================
  int monitorOnCallback (char* compname, char* attribute,
			 rcCallback callback,
			 void* arg);

  //=======================================================================
  // int monitorOffCallback (char* compname, char* attribute,
  //                         rcCallback moncallback, void* monarg,
  //                         rcCallback callback, void* arg)
  //     monitor value off for moncallback and monarg
  //     If moncallback and monarg appeared more than once, they all
  //     will be removed
  //     return CODA_SUCCESS: removal monitor callback succefully
  //     return CODA_ERROR: failed
  //======================================================================
  int monitorOffCallback (char* compname, char* attribute,
			  rcCallback moncallback, void* monarg,
			  rcCallback callback, void* arg);

  //=======================================================================
  //     char *exptname (void) const
  //                 Return Experiment name
  //=======================================================================
  char* exptname (void) const;

  //======================================================================
  //     Some functions useful for implementation of CDEV interface
  //======================================================================
  daqData* data (char* compname, char* attribute);

protected:
  // deny access to assignment and copy
  rcClient (const rcClient&);
  rcClient& operator = (const rcClient&);

  static void attr2Callback (int status, void* arg, daqNetData* data);
  static void offCallback (int status, void* arg, daqNetData* data);

  // inherited functions from event_handler
  int get_handle (void) const;
  int handle_input (int fd);
  int handle_close (int, Reactor_Mask);
  int handle_timeout (const Time_Value& tv, const void* arg);

  // create all variables
  void createAllVariables (void);
  void deleteAllVariables (void);

  // some IO functions
  int getValCbkFromServer    (rcMsg& cmsg);
  int setValCbkFromServer    (rcMsg& cmsg);
  int monitoredValFromServer (rcMsg& cmsg);
  int monitorOffFromServer   (rcMsg& cmsg);
  int commandCbkFromServer   (rcMsg& cmsg);
  int addDynamicVars         (rcMsg& cmsg);
  int removeDynamicVars      (rcMsg& cmsg);
  int addAnaLogVars          (rcMsg& cmsg);
  int removeAnaLogVars       (rcMsg& cmsg);
  int sendClientInfo         (void);

  // call all disconnect callbacks
  void callAllDiscCbks       (void);

  // some debug information
  int  numCmdCbks      (void);
  void removeCallbacks (void);

private:
  // reactor which handles everything
       Reactor reactor_;
  // real network connection
  SOCK_Stream toServer_;
  SOCK_Connector conServer_;
  // data areas
  // experiment name
  char* exptname_;
  // user name
  char* username_;
  // process id
  int   pid_;
  // xwindow display
  char* disp_;
  // flag to see whether this client information is registered to the server
  int  infoRegistered_;
  // connection flags
  int connected_;
  // vars deleted flag
  int varDeleted_;
  // disconnect callback list
  codaSlist discCbkList_;
  // table for all command callbacks
  codaIntHash cmdCbkTable_;
  // table for all data variables
  daqDataManager dataManager_;
  // all data variables
  daqData* version_;
  daqData* startTime_;
  daqData* startTimeBin_;
  daqData* endTime_;
  daqData* endTimeBin_;
  daqData* currTime_;
  daqData* currTimeBin_;
  daqData* runNumber_;
  daqData* status_;
  daqData* nevents_;
  daqData* nlongs_;
  daqData* runType_;
  daqData* runTypeNum_;
#if defined (_CODA_2_0_T) || defined (_CODA_2_0)
  daqData* database_;
  daqData* dataFile_;
  daqData* rcsMsgToDbase_;
  daqData* logFileDescriptor_;
  daqData* tokenInterval_;
#endif
  daqData* allRunTypes_;
  daqData* exptName_;
  daqData* exptId_;
  daqData* hostName_;
  daqData* autoIncrement_;
  daqData* eventLimit_;
  daqData* dataLimit_;
  daqData* updateInterval_;  
  daqData* compnames_;
  daqData* clientList_;
  daqData* master_;
  daqData* controlDisp_;
  daqData* online_;
  daqData* compBootInfo_;
  daqData* monitorParms_;
  // message buffer
  daqData* runMsg_;
  // static function for register client information callback
  static void regInfoCbk (int status, void* arg, daqNetData* data);

  // lock for client object to prevent it from being called recursively
  int  lock_;
  void lock   (void);
  void unlock (void);

  // friend class 
  friend class rcClientLocker;
};

class rcClientLocker
{
public:
  // constructor and destructor
  rcClientLocker  (rcClient* client);
  ~rcClientLocker (void);

private:
  rcClient* client_;
  // deny access to assignment and copy operations
  rcClientLocker (const rcClientLocker& );
  rcClientLocker& operator = (const rcClientLocker& );
};

#endif
