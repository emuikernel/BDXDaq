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
//      CODA RunControl Client Handler For X Window
//      First to Create and Last to Destroy
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: rcClientHandler.h,v $
//   Revision 1.3  1996/11/04 16:14:54  chen
//   add options for monitoring components
//
//   Revision 1.2  1996/10/31 15:57:29  chen
//   Add server message to database option
//
//   Revision 1.1.1.1  1996/10/11 13:39:23  chen
//   run control source
//
//
#ifndef _RC_CLIENT_HANDLER_H
#define _RC_CLIENT_HANDLER_H

#include <stdio.h>
#include <string.h>
#include <assert.h>

#include <codaSlist.h>
#include <XcodaInput.h>
#include <rcSvcProtocol.h>
#include <rcMsg.h>
#include <rcClient.h>
#include <rcXuiConsts.h>

class rcPanel;
class daqCompBootStruct;
class daqMonitorStruct;

class rcClientHandler: public XcodaInput
{
public:
  rcClientHandler (Widget parent);
  rcClientHandler (XtAppContext context);

  virtual ~rcClientHandler (void);

  // cached server run status or connection status
  int     status     (void) const;
  int     connected  (void) const;
  // experiment name
  char*   exptname   (void) const;

  // connect to remoter server and monitor status
  // return CODA_SUCCESS on success
#if defined (_CODA_2_0_T) || defined (_CODA_2_0)
  int     connect    (char* database, char* exptname, char* msqld = 0);
#else
  int     connect    (char* host, char* exptname, int exptid);
  int     connect    (char* host, char* exptname, int exptid,
		      rcUpdateFunc func, void* arg);
#endif

  // return how many clients are connected to the rc server
  int     numConnectedClients (void) const;
  char**  connectedClients (int& num);

#if defined (_CODA_2_0) || defined (_CODA_2_0_T)
  // return how many database and each database name
  int     numDatabases        (void) const;
  char**  databases           (int& num);

  // return session information
  int     numSessions         (void) const;
  char**  sessions            (int& num);
  int*    sessionActive       (int& num);

  // return data log file
  char*   datalogFile         (void) const;

  // return data log descriptor
  char*   logFileDescriptor   (void) const;

  // return token inetrval value
  int     tokenInterval       (void) const;
  // token interval changed, update all panels
  void    setTokenInterval    (int val);
  // state of server msg to database changed, update all panels
  void    setRcsMsgToDbaseFlag (int state);
#endif

  // return how many active components
  int     numComponents       (void) const;
  char**  components          (int& num);

  // get components auto boot flag info
  long    compBootInfo        (char** &comps, long* &autoboot);

  // get monitor parameters
  long    monitorParms        (char** &comps, long* &monitored,
			       long& autoend, long& interval);
  
  // disconnnect from remote server
  void    disconnect (void);
  void    killServer (void);

  // request/giveup mastership
  int    requestMastership (void);
  int    giveupMastership  (void);

  // register all rcPanel 
  int     addPanel     (rcPanel* panel);
  int     removePanel  (rcPanel* panel);

  // when status changed to change appearance on all panels
  void    status        (int st);
  // when the mastership has been changed, change appearance on all panels
  void    mastershipType (int type);
  // when the online option has been changed, change appearance on all panels
  void    setOnlineOption (int option);
  // when update interval has been changed, change apperance on all panels
  void    setUpdateInterval (int interval);
  // return whether all ANA has real logging files
  int     anaLogToRealFiles (void);

#if !defined (_CODA_2_0_T) && !defined (_CODA_2_0)
  // return number of ANAs with list of daqNetData pointers
  codaSlist& allAnas (void);
#endif
  
  // give direct handle of rcClient
  rcClient& clientHandler (void);

protected:
  virtual void input_callback (void);
  // monitor on callback for status
  static void  statusCallback (int status, void *arg, daqNetData* data);
  // monitor mastership
  static void  mastershipCallback (int status, void* arg, daqNetData* data);
  // monitor callback on online/offline option
  static void  onlineCallback     (int status, void* arg, daqNetData* data);
  // monitor callback on update interval
  static void  updateIntervalCbk  (int status, void* arg, daqNetData* data);
  // monitor on number of clients connections
  static void  clientsCallback (int status, void* arg, daqNetData* data);
  // requestMaster and giveup master callback
  static void  reqMastershipCbk   (int status, void* arg, daqNetData* data);
  static void  giveupMastershipCbk(int status, void* arg, daqNetData* data);
#if defined  (_CODA_2_0_T) || defined (_CODA_2_0)
  // moinitor on all databases
  static void  dbaseCallback      (int status, void* arg, daqNetData* data);
  // moinitor on all sessions
  static void  sessionsCallback   (int status, void* arg, daqNetData* data);
  static void  sessionStatusCallback (int status, void* arg, daqNetData* data);

  // monitor on log file descriptor
  static void  logFileDesCallback (int status, void* arg, daqNetData* data);

  // monitor token interval
  static void  tokenIntervalCallback (int status, void* arg, daqNetData* data);

  // monitor on rcs msg logging to a database
  static void  rcsMsgToDbaseCbk      (int status, void* arg, daqNetData* data);
#endif
  // monitor on component auto boot information
  static void  cabootinfoCallback (int status, void* arg, daqNetData* data);

  // monitor on options of runcontrol monitoring components
  static void  monitorParmsCallback  (int status, void* arg, daqNetData* data);


  // Monitor on callback for all components
  static void componentsCallback  (int status, void* arg, daqNetData* data);
  static void offCallback         (int status, void* arg, daqNetData* data);

  // ana log callback
  static void anaLogCallback      (int status, void* arg, daqNetData* data);

  // when any changes happen to a ANA components, tell everyone attached
  // to this handler
  void        anaLogChanged  (daqNetData* info, int added);


#if !defined (_CODA_2_0_T) && !defined (_CODA_2_0)
  void        monitorDataLogFiles (void);  
#endif

  // clients connection changed
  void        clientsConnectionChanged (void);
  
  // disconnect callback
  static void  discCallback   (int status, void *arg, daqNetData* data);

private:
  // cached value of runcontrol server status
  int      status_;
  // master value
  int      master_;
  // all active components
  char*    components_[RCXUI_MAX_COMPONENTS];
  int      numComps_;
  // all connected clients
  char*    clients_[RCXUI_MAX_CLIENTS];
  int      numClients_;

  // all databases
  char*    dbases_[RCXUI_MAX_DATABASES];  // not used in coda 1_4
  int      numDbases_;

  // all sessions
  char*    sessions_[RCXUI_MAX_SESSIONS];  // not used in coda 1_4
  int      sessionActive_[RCXUI_MAX_SESSIONS];
  int      numSessions_;

  // all components auto boot information
  daqCompBootStruct* caboot_;

  // all options for monitoring
  daqMonitorStruct*  monParms_;

#if defined (_CODA_2_0_T) || defined (_CODA_2_0)
  char* datalogFile_;
  char* logFileDes_;
  int   tokenIVal_;
#else
  // all ANA components with daqNetData*
  codaSlist anas_;
#endif
  // network handler to runcontrol server
  rcClient handler_;
  // list for all rcPanel
  codaSlist panels_;
};
#endif
