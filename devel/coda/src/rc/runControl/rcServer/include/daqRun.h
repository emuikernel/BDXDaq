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
//      Data Acquisition DAQ Run Class. This Class handles state 
//      transition requests by forwarding them onto one DAQ
//      system object. It also contains some DAQ variables such as
//      event number and so on.
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: daqRun.h,v $
//   Revision 1.7  1999/02/25 14:37:01  rwm
//   Limits defined in daqRunLimits.h
//
//   Revision 1.6  1998/11/10 16:37:01  timmer
//   Linux port
//
//   Revision 1.5  1997/05/16 16:04:10  chen
//   add global script capability
//
//   Revision 1.4  1997/02/03 13:47:32  heyes
//   add ask command
//
//   Revision 1.3  1996/11/04 16:13:49  chen
//   add options for monitoring components
//
//   Revision 1.2  1996/10/31 15:56:09  chen
//   Fixing boot stage bug + reorganize code
//
//   Revision 1.1.1.1  1996/10/11 13:39:18  chen
//   run control source
//
//
#ifndef _CODA_DAQ_RUN_H
#define _CODA_DAQ_RUN_H

#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "daqRunLimits.h"
#include <daqSystem.h>
#include <daqDataManager.h>
#include <daqDataUpdater.h>
#include <daqCompBootStruct.h>
#include <daqRunTypeStruct.h>
#include <daqMonitorStruct.h>
#include <daqScriptSystem.h>
#include <rcsDaqData.h>
#include <rcsRunNumberWriter.h>
#include <rcsEvLimitWriter.h>
#include <rcsDataLimitWriter.h>
#include <rcsUpdateIWriter.h>
#include <netConfig.h>
#include <rccCmdBuffer.h>


class dbaseReader;
class daqRun;
class rccAcceptor;
class daqCompMonitor;

#if defined (_CODA_2_0_T) || defined (_CODA_2_0)
#include <rcsTokenIntervalWriter.h>
#include <rcsLogFileDesWriter.h>

class daqRunType
{
private:
  daqRunType     (void);
  daqRunType     (char* type, int number, int inuse, char* cat = 0);
  ~daqRunType    (void);
  daqRunType     (const daqRunType& type);
  daqRunType&    operator = (const daqRunType& type);

  char* type_;
  int   number_;
  int   inuse_;
  char* cat_;

  friend class daqRun;
};
#else
class daqRunType
{
private:
  daqRunType     (void);
  daqRunType     (char* type, int number);
  ~daqRunType    (void);
  daqRunType     (const daqRunType& type);
  daqRunType&    operator = (const daqRunType& type);

  char* type_;
  int   number_;
  friend class daqRun;
};
#endif

class daqRun
{
public:
  // constructor and destructor
#if defined (_CODA_2_0_T) || defined (_CODA_2_0)
  daqRun  (char* dbasename, char* session, int expid, char* msqlhost);
#else
  daqRun  (char* exptname, int expid);
#endif

  ~daqRun (void);

  // operations
  // abort operation
  virtual void abort (int wanted);
   // autostart for run
  virtual int autostart (void);
  // boot operation
  virtual int boot (void);
  // configuration of new run type
  virtual int configure (char* type);
  // download operation
  virtual int download (void);
  // go operation
  virtual int go (void);
  // pause operation
  virtual int pause (void);
  // prestart operation
  virtual int prestart (void);
  // reset operation
  virtual int reset (void);
  // end operation
  virtual int end (void);
  // terminate operation
  virtual int terminate (void);
  // auto transition
  virtual int autoTransition (int istate, int fstate);
  // cancel transition
  virtual int cancelTransition (void);

  // clean all information
  void cleanAll (void);
  // load database: return CODA_SUCCESS for success
  int loadDatabase (char* direc, char* session = 0);
  // get network configuration information for a component with name 'title'
  // return CODA_SUCCESS: found information
  // return CODA_ERROR: not found
  int  getNetConfigInfo (char* title, char* &config);
  int  configured (char* title);

  // get expetname
  char*   exptname (void) const;  // session name in version 2.0
  // get experiment id
  int     exptid   (void) const;
  // get host name
  char*   hostname (void) const;
  // get control display name
  char*   controlDisplay (void) const;
#if defined (_CODA_2_0_T) || defined (_CODA_2_0)
  char*   msqlhost (void) const;
  // create a new session
  int     createSession (char* session);
  // select a session
  int     selectSession (char* session);
  // change current database name
  void    database (char* dbase);
  char*   database (void) const;

  // token interval value: writeUpdate == 1 means write to database
  void    tokenInterval (int val, int writeUpdate = 1);
  int     tokenInterval (void) const;

  // run control server message to database or not
  void    enableRcsMsgToDbase  (void);
  void    disableRcsMsgToDbase (void);
  int     rcsMsgToDbase        (void) const;
#endif
  
  // runtype name 
  void    runtype     (char* type);
  char*   runtype     (void) const;
  int     runtypeNum  (void) const;
  // run number
  void    runNumber (int number);
  int     runNumber (void) const;
  // increase runnumber by one
  void    increaseRunNumber (void);
  // write to datebase
  void    updateRunNumber (int number);
  void    updateEventLimit (int evl);
  void    updateDataLimit  (int dl);
#if defined (_CODA_2_0_T) || defined (_CODA_2_0)
  // data file descriptor
  void    writeDataFileNameToDbase  (char* file);
  void    setDataFileName           (char* file);

  // update token ineterval number
  void    updateTokenInterval (int interval);
#endif
  // run state status
  void    status    (int st);
  int     status    (void) const;
  // run event number
  void    eventNumber (int evn);
  int     eventNumber (void) const;
  // run data long words
  void    longWords   (int lwd);
  int     longWords   (void) const;
  // set start time
  void    setStartTime (void);
  // set end time
  void    setEndTime   (void);
  // erase end/start time
  void    eraseStartTime (void);
  void    eraseEndTime   (void);

  // show all data
  void    showdata  (void);
  // add runtype
#if defined (_CODA_2_0_T) || defined (_CODA_2_0)
  void    addRunType (char* runtype, int number, int inuse, char* cat = 0);
#else
  void    addRunType (char* runtype, int number);
#endif
  void    setAllRunTypes (void);
  // set all components' name
  void    setAllCompNames (void);
  // set all components' boot info: must be called after configure
  void    setAllCompBootInfo (void);
  // set all components' monitoring parameters: must be called after configure
  void    setAllMonitorParms (void);

  // setup auto increment
  // flag = 1, turn auto increment on, flag = 0, turn auto increment off
  // default behaviour is 'flag = 1'
  void    autoIncrement (int flag);
  int     autoIncrement (void) const;

  // setup online/offline
  void    online   (void);
  void    offline  (void);
  int     isOnline (void) const;

  // event and data limit
  // default event and data limit are 0. Data limit is in the unit of kbyte
  void    eventLimit  (int nevent);
  int     eventLimit  (void) const;
  void    dataLimit   (int dl);
  int     dataLimit   (void) const;
#if defined (_CODA_2_0_T) || defined (_CODA_2_0)
  void    dataFile    (char* file);
  char*   dataFile    (void) const;
  void    dataFile    (int   runNumber);
#endif


  // update interval for remote components (in seconds)
  daqData* updateInterval_; // data should be private but others need access
  void    dataUpdateInterval (int sec);
  int     dataUpdateInterval (void) const;
  void    restartUpdating (int sec);

  // update run control message buffer
  void    updateRunMessage (char *newMessage);

  // setup dynamic variables
  void    createDynamicVars (void);
  void    stopUpdatingDynamicVars (void);
  void    startUpdatingDynamicVars (void);
  // update dynamic variables
  void    updateDynamicVars (void);

  // start/stop monitoring components
  void    startMonitoringComp (void);
  void    endMonitoringComp  (void);
  
  void    removeDynamicVars (void);
  int     numDynamicVars (void) const;
  void    sendDynamicVarsInfo (rccIO* chan);
  void    setStatusVariable (char* title, int st);

  // setup ANA log information
  // must be called after configure step
  void    createAnaLogVars   (void);
  // must be called before system clean up
  void    removeAnaLogVars   (void);
  void    updateAnaLogInfo   (char* ana, char* path);
  void    sendAnaLogVarsInfo (rccIO* chan);
  
  // return and set this server udp port number
  unsigned short udpPort     (void) const;
  void           udpPort     (unsigned short port);


  // access system, data manager and data updater
  daqSystem& system (void);
  daqDataManager& dataManager (void);
  daqDataUpdater& dataUpdater (void);
  
  // access daqScriptSystem
  daqScriptSystem& scriptSystem (void);
  
  // setup database reader pointer
  void dbaseDecoder (dbaseReader* reader);

  // check daqRun is locked or not
  int  locked (void) const;
  void lock   (void);
  void unlock (void);

  //======================================================================
  //     Network related functions
  //======================================================================
  // setup network manager pointer
  void networkManager (rccAcceptor* m);
  // process command from client
  int processCommand (rccIO* chan, int type, rcMsg* msg);
  // get value: value name is inside msg
  int getValue (rccIO* chan, rcMsg* msg);
  // set value: value name is inside msg
  int setValue (rccIO* chan, rcMsg* msg);
  // monitor on a value
  int monitorOnValue (rccIO* chan, rcMsg* cmsg);
  // turn off monitor on a value
  int monitorOffValue (rccIO* chan, rcMsg* cmsg);
  // auto boot info the components in the rcMsg cmsg
  int autoBootInfo    (rccIO* chan, rcMsg* cmsg);
  // monitor option parameters
  int monitorParms    (rccIO* chan, rcMsg* msg);

  // ask component
  int ask_component    (rccIO* chan, rcMsg* msg,char **ref);

  // update client information
  int updateClientInfo  (rccIO* chan, long cbkid = 0);
  // update master information
  void updateMasterInfo  (char* info);
  // update control display information
  void updateControlDispInfo (char* disp);
  // remove all callbacks associated with on client rccIO
  void removeCallbacks (rccIO* chan);
  // network access point for transitioner and others to send result
  void cmdFinalResult (int success);

  // class name
  virtual const char *className (void) const { return "daqRun";}

protected:
  // Create all those daq variables
  void createAllVariables (void);
  void resetAllVariables  (void);
  void deleteAllVariables (void);

  // function before calling configure
  int  preConfigure (char* type);

private:
  // data areas
  // experiment name
  char* exptname_;  // session name
  int   exptid_;
#if defined (_CODA_2_0_T) || defined (_CODA_2_0)
  char* dbasename_;     // database name
  char* msqlhost_;
  char* datafileName_;  // data file name 
#endif
  // DAQ system
  daqSystem system_;
  // DAQ Data Manager
  daqDataManager dataManager_;
  // DAQ script system
  daqScriptSystem scriptSystem_;
  // DAQ data updater
  daqDataUpdater dataUpdater_;
  // monitor for all components
  daqCompMonitor* monitor_;
  // Database Decoder
  dbaseReader* dbreader_;
  // all runtypes
  daqRunType runtypes_[MAX_NUM_RUNTYPES];
  int        numtypes_;
  // command buffer
  rccCmdBuffer cmdBuffer;
  // all daq data variables
  daqData* version_;
  daqData* startTime_;
  daqData* currTime_;
  daqData* endTime_;
  daqData* startTimeBin_;  // integer form
  daqData* currTimeBin_;   // integer form
  daqData* endTimeBin_;    // integer form
  daqData* runNumber_;
  daqData* status_;
  daqData* runType_;
  daqData* runTypeNum_;
  daqData* nevents_;
  daqData* nlongs_;
#if defined (_CODA_2_0_T) || defined (_CODA_2_0)
  daqData* database_;
  daqData* runTypeInfo_;
  daqData* rcsMsgToDbase_;
  daqData* dataFile_;            // real data file name
  daqData* logFileDescriptor_;   // data logging file descriptor
  daqData* tokenInterval_;       // token interval
#endif
  daqData* allRunTypes_;
  daqData* exptName_;
  daqData* exptId_;
  daqData* hostName_;
  daqData* autoIncrement_;
  daqData* eventLimit_;
  daqData* dataLimit_;
  daqData* compnames_;
  daqData* clientList_;
  daqData* master_;
  daqData* controlDisp_;   // where current control display is
  daqData* online_;
  daqData* compBootInfo_;
  daqData* monitorParms_;
  // message buffer
  daqData* runMsg_;

  // dynamic variables
  daqData* dvars_[MAX_NUM_DYN_VARS];
  int      numDvars_;

  // database writer
  daqDataWriter* runNumberWriter_;
  daqDataWriter* updateIWriter_;
  daqDataWriter* evlimitWriter_;
  daqDataWriter* datalimitWriter_;

#if defined (_CODA_2_0_T) || defined (_CODA_2_0)
  daqDataWriter* tokenIWriter_;
  daqDataWriter* logfdesWriter_;
#endif  

  // Lock variable to lock whole system to let one command to finish before
  // next command to come to execute
  int locked_;

  // network acceptor ie. manager
  rccAcceptor* netMan_;

  // rcServer udp port number
  unsigned short port_;

  // deny access to copy and assignment
  daqRun (const daqRun& );
  daqRun& operator = (const daqRun& );
};
#endif
