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
//      CODA database reader. Parse database and create all necessary
//      DAQ components
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: dbaseReader.h,v $
//   Revision 1.2  1996/11/20 15:33:42  chen
//   Fix a bug related to enabling a wrong components
//
//   Revision 1.1.1.1  1996/10/11 13:39:18  chen
//   run control source
//
//
#ifndef _CODA_DBASE_READER_H
#define _CODA_DBASE_READER_H

#include <stdio.h>
#include <string.h>
#include <assert.h>

#include <daqConst.h>
#include <daqRun.h>

// Objects of this class will be stored in the hash table
// upon runtime configuration
class rcNetConfig
{
public:
  // constructor
  rcNetConfig  (char* title, char* config);
  // destructor
  ~rcNetConfig (void);

  // return title
  char* title (void) const;
  // return config information
  char* config (void) const;
  // set config information
  void  config (char* cf);

private:
  char* title_;
  char* config_;

  // deny access to copy and assignment operator
  rcNetConfig (const rcNetConfig& config);
  rcNetConfig& operator = (const rcNetConfig& config);
};

#if defined (_CODA_2_0_T) || defined (_CODA_2_0)
#include "libdb.h"
#define DBASE_MAX_SESSIONS    100
#define DBASE_MAX_CONFIGS     200
#define DBASE_MAX_DBASES      100
#define DBASE_SESSION_TABLE   "sessions"
#define DBASE_PROCESS_TABLE   "process"
#define DBASE_RUNTYPE_TABLE   "runTypes"
#define DBASE_PRIORITY_TABLE  "priority"
#define DBASE_POSITION_TABLE  "_pos"
#define DBASE_OPTION_TABLE    "_option"
#define DBASE_SCRIPT_TABLE    "_script"
#define DBASE_EVENTLIMIT      "eventLimit"
#define DBASE_DATALIMIT       "dataLimit"
#define DBASE_DATAFILE        "dataFile"
#define DBASE_TOKEN_INTERVAL  "tokenInterval"
#endif

class factory;

class dbaseReader
{
public:
  // constructor and destructor
  // provide exptid and run reference
  dbaseReader  (int exptid, daqRun& run);
  ~dbaseReader (void);

  // operations
  // set and retrieve database directory
  void database  (char *dbaseDir);
  char* database (void) const;

#if defined (_CODA_2_0_T) || defined (_CODA_2_0)
  int isDatabaseOpen    (void) const;
  int databaseSelected  (void) const;

  // get all session and session status
  int  getAllSessions (void);
  int  createSession  (char* session);
  // check a session is created
  int sessionCreated   (char* name);
  int  selectSession  (char* session);
  int  giveupSession  (char* session);
  int  getPriorities  (void);
  void setDefaultPriorities (void);

  // check whether a configuration is in use
  int isConfigInUse  (char* config);
  // give up a configuration
  int giveupConfiguration (char* config);

  // get data file name
  void  putDataFileName (char* name);
  char* getDataFileName (void);

  // put token interval value to database
  void  putTokenInterval (int itval);

  // insert runControl information into the database
  int insertRcServerToDbase (void);
  // remove runControl information from the database
  int removeRcServerFromDbase (void);
#endif

  // Following functions return CODA_SUCCESS on success
  // read rcNetwork and rcPriority file and create all components
  int getComponents  (void);
  // read rcRunTypes 
  int getAllRunTypes (void);
  // read rcRunNumber
  int  getRunNumber   (void);
  // update rcRunNumber
  void putRunNumber   (int number);

  // update event/data limit, call these after runtype has been selected
  void putEventLimit  (int evl);
  void putDataLimit   (int dl);

  // read a configuration
  int configure (char* runtype);
  // read an option file
  int parseOptions (char* runtype);
  // get network configuration information
  // return CODA_SUCCESS, found it config info for this component
  // return CODA_ERROR, not found
  int getNetConfigInfo (char *title, char* &config);
  int setNetConfigInfo (char *title, char* config);
  int configured (char *title);

protected:
#if defined (_CODA_2_0_T) || defined (_CODA_2_0)
  // connect to database server
  int connectMysql      (void);
  // list all databases
  int listAllDatabases (void);
  // list all session names and active flags
  int listAllSessions  (void);
  // check a session is active
  int sessionActive    (char* name);

  // mysql database reconnection mechanism : just retry to connect
  // return CODA_SUCCESS: ok and database is selected
  // return CODA_ERROR:   cannot reconnect and database is not selected
  int reconnectMysql     (void);
#endif

  // check whether a component with name 'title' in the hash table
  int compInsideHash    (char* title);

private:
  char*       dbaseDir_;
  int         exptid_;
  daqRun&     run_;
  factory*    compFactory_;
#if defined (_CODA_2_0_T) || defined (_CODA_2_0)
  int         dbaseSock_;
  char*       username_;
  uid_t       uid_;
  gid_t       gid_;
  static int  numRetries_;  // number of retries to connect to mysql server
#endif
  codaStrHash cinfos_;
};
#endif
