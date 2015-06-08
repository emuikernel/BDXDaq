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
//      Implementation of dbaseReader
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: dbaseReader.cc,v $
//   Revision 2.0  1999/07/28 18:56:47  rwm
//   Configuration in use is not an ERROR but INFO.
//
//   Revision 1.12  1998/06/02 19:51:46  heyes
//   fixed rcServer
//
//   Revision 1.11  1997/12/15 16:14:42  abbottd
//   Changes to allow global transition scripts to work
//
//   Revision 1.10  1997/07/22 19:39:01  heyes
//   cleaned up lots of things
//
//   Revision 1.9  1997/06/14 12:36:46  heyes
//   spurious } delete during editing removed
//
//   Revision 1.8  1997/06/13 21:30:46  heyes
//   for marki
//
//   Revision 1.7  1997/05/23 16:45:06  heyes
//   add SESSION env variable, remove coda_activate
//
//   Revision 1.6  1997/05/16 16:04:10  chen
//   add global script capability
//
//   Revision 1.5  1997/01/24 16:36:03  chen
//   change/add Log Component for 1.4
//
//   Revision 1.4  1996/12/04 18:32:44  chen
//   port to 1.4 on hp and ultrix
//
//   Revision 1.3  1996/11/20 15:33:40  chen
//   Fix a bug related to enabling a wrong components
//
//   Revision 1.2  1996/10/14 20:02:48  heyes
//   changed message system
//
//   Revision 1.1.1.1  1996/10/11 13:39:18  chen
//   run control source
//
//
#include <ctype.h>
#include <factory.h>
#include <rcMsgReporter.h>
#include <daqConst.h>
#include <daqActions.h>
#include "dbaseReader.h"
#include <codaSlist.h>
#include <codaStrHash.h>
#include <rcSvcInfoFinder.h>
/*sergey
#include <dplite.h>
*/

const int CODA_CONFIG_HASH_SIZE = 97;


const int FILENAME_MAX_LEN = 1024;
const int BUFFER_SIZE = 1024;


//======================================================================
//       Implementation of rcNetConfig class
//======================================================================
rcNetConfig::rcNetConfig (char* title, char* config)
{
  //printf (OBJECT_DBG,"Create rcNetConfig Class Object\n");
  title_ = new char[::strlen (title) + 1];
  ::strcpy (title_, title);

  if (config) {
    config_ = new char[::strlen (config) + 1];
    ::strcpy (config_, config);
  }
  else
    config_ = 0;
}

rcNetConfig::~rcNetConfig (void)
{
  //printf (OBJECT_DBG,"Delete rcNetConfig Class Object\n");
  delete []title_;
  if (config_)
    delete []config_;
}

char* 
rcNetConfig::title (void) const
{
  return title_;
}

char* 
rcNetConfig::config (void) const
{
  return config_;
}

void
rcNetConfig::config (char* cf)
{
  if (config_)
    delete []config_;

  config_ = new char[::strlen (cf) + 1];
  ::strcpy (config_, cf);
}

//=====================================================================
//    Implementation of dbaseReader Class
//=====================================================================
#if defined (_CODA_2_0_T) || defined (_CODA_2_0)
int dbaseReader::numRetries_ = 20;
#endif

static int emptyLine (char *line)
{
  char *p = line;

  while (*p != '\0' && *p != '\n') {
    if (isgraph (*p))
      return 0;
    p++;
  }
  return 1;
}

// expand environment variable in the beginning of the filename
static char* expandFilename (char *filename)
{
  char *p = 0;
  if (filename[0] != '$') { // no need to expand
    p = new char[::strlen (filename) + 1];
    ::strcpy (p, filename);
  }
  else {
    char *slash = 0;
    if ((slash = ::strchr (filename, '/')) == 0) {
      char *e = ::getenv (&filename[1]);
      if (e) {
	p = new char[::strlen (e) + 1];
	::strcpy (p, e);
      }
    }
    else {
      char env[128]; // environment vairable
      // retrieve environment variable
      char* q = env;
      char* cur = &filename[1];
     while (*cur != *slash) {
	*q = *cur;
	q++; cur++;
      }
      *q = '\0';
      q = ::getenv (env);
      if (q) {
	char expanded[128];
	::strcpy (expanded, q);
	::strcat (expanded, slash);
	p = new char[::strlen (expanded) + 1];
	::strcpy (p, expanded);
      }
      else {
	p = new char[::strlen (slash) + 1];
	::strcpy (p, slash);
      }	
    }
  }
  return p;
}

static void trimBootString (char *boot)
{
  char *p = boot;

  while (*p != '\0' && *p != '\n')
    p++;
  if (*p == '\n')
    *p = '\0';
}


// convert string to lower case
static void toLower (char *str)
{
  char *p = str;
  while (*p != '\0') {
    *p = tolower (*p);
    p++;
  }
}

#if defined (_CODA_2_0_T) || defined (_CODA_2_0)    
dbaseReader::dbaseReader (int exptid, daqRun& run)
:dbaseDir_ (0), exptid_ (exptid), run_(run), dbaseSock_ (NULL),
 cinfos_ (CODA_CONFIG_HASH_SIZE, codaStrHashFunc)
{
  //printf (OBJECT_DBG,"Create dbaseReader Class Object\n");
  // create component factory
  compFactory_ = new factory (exptid, run_.system());

  // get user information
  struct passwd* pwsd = 0;
  if ((pwsd = getpwuid (getuid()) ) != 0) {
    username_ = new char[::strlen(pwsd->pw_name) + 1];
    ::strcpy (username_,pwsd->pw_name);
  }
  else {
    char buff[80];
    ::sprintf (buff, "userid_%d",getuid ());
    username_ = new char[::strlen (buff) + 1];
    ::strcpy (username_, buff);
  }
  uid_ = ::getuid ();
  gid_ = ::getgid ();

  // connect to a mysql server
  if (connectMysql () == CODA_SUCCESS) {
    if (listAllDatabases () == CODA_SUCCESS)
      reporter->cmsglog (CMSGLOG_INFO1,"Connected to mysql server on %s\n",
			       run_.msqlhost());
    else
      reporter->cmsglog (CMSGLOG_ERROR,"Can't talk to mysql server on %s\n",
			       run_.msqlhost());
  }
  else {
    reporter->cmsglog (CMSGLOG_ERROR,"Can't talk to mysql server on %s\n",
			     run_.msqlhost());
    fprintf (stderr, "Failed to connect to mysql server on %s\n",
	     run_.msqlhost());
    ::exit (1);
  }

  // select a database
  database (run_.database ());
  // insert run control information to the database
  insertRcServerToDbase ();
}

dbaseReader::~dbaseReader (void)
{
  //printf (OBJECT_DBG,"Delete dbaseReader Class Object\n");
  // remove information from the database
  removeRcServerFromDbase ();
 
  // first give up session name
  giveupSession (run_.exptname ());
  // give up current config name if possible
  giveupConfiguration (run_.runtype ());

  // then close the socket to mysql server
  if (dbaseSock_ != NULL) {
    ::mysql_close (dbaseSock_);
  }
  dbaseSock_ = NULL;
  if (dbaseDir_)
    delete []dbaseDir_;
  dbaseDir_ = 0;

  if (username_)
    delete []username_;

  delete compFactory_;

  // clean up the hash table 
  codaStrHashIterator ite (cinfos_);
  rcNetConfig* cf = 0;

  for (ite.init(); !ite; ++ite) {
    cf = (rcNetConfig *)ite ();
    delete cf;
  }
  cinfos_.deleteAllValues ();
}

int
dbaseReader::connectMysql (void)
{

  //printf (DBASE_DBG,"connect mysql server\n");

  if (dbaseSock_ == NULL)
  {
    if (::strcmp(run_.msqlhost(),"localhost"))
    {
      dbaseSock_ = ::dbConnect (run_.msqlhost(), getenv("EXPID"));
    }
    else
    {
      dbaseSock_ = ::dbConnect ("localhost", getenv("EXPID"));
    }
    if (dbaseSock_ == NULL)
    {
      fprintf (stderr, "Failed to connect to mysql server on %s\n", 
	       run_.msqlhost());
      reporter->cmsglog (CMSGLOG_ERROR,"Can't talk to mysql server on %s\n", 
			       run_.msqlhost());
      return CODA_ERROR;
    }
    else
	{
      reporter->cmsglog (CMSGLOG_INFO1,"Connected to mysql server on %s\n", 
			       run_.msqlhost());
	}
  }
  return CODA_SUCCESS;
}



int
dbaseReader::reconnectMysql (void)
{
  // check whether the mysql server is indeed went away
  /*sergey: if had following error, will try to reconnect, otherwise return error*/
  if (strcmp(mysql_error(dbaseSock_),"MySQL server has gone away") !=0 )
  {
    fprintf(stderr,"dbaseReader::reconnectMysql: >%s< - do nothing\n",
      mysql_error(dbaseSock_));
    return(CODA_ERROR);
  }
  else
  {
    fprintf(stderr,"dbaseReader::reconnectMysql: >%s< - will try to re-connect\n",
      mysql_error(dbaseSock_));
  }

  int i = 0;
  while (i < dbaseReader::numRetries_)
  {
    if (::strcmp(run_.msqlhost(),"localhost"))
    {
      dbaseSock_ = ::dbConnect (run_.msqlhost(), getenv("EXPID"));
    }
    else
    {
      dbaseSock_ = ::dbConnect ("localhost", getenv("EXPID"));
    }

    if (dbaseSock_ < 0)
    {
      fprintf (stderr, "Can't talk to mysql server on %s : %s\n", 
	       run_.msqlhost(), mysql_error(dbaseSock_));

      reporter->cmsglog (CMSGLOG_ERROR,"Failed to re-connect to mysql server on %s\n", 
			       run_.msqlhost());
    }
    else
    {
      reporter->cmsglog (CMSGLOG_INFO1,"Re-connected to mysql server on %s\n", 
			       run_.msqlhost());
      if (dbaseDir_)
      { // database already selected 
	    if (::mysql_select_db(dbaseSock_, dbaseDir_) < 0)
        {
	      fprintf (stderr, "Cannot select database %s\n", dbaseDir_);
	      reporter->cmsglog (CMSGLOG_ERROR,"Cannot select database %s\n",
				   dbaseDir_);
	      delete []dbaseDir_;
	      dbaseDir_ = 0;
	      return CODA_ERROR;
	    }
	    return CODA_SUCCESS;
      }
    }
    i++;
    ::sleep (1);
  }
  fprintf (stderr, "Cannot re-connect to mysql server.., I give up\n");
  ::exit (1);
}



int
dbaseReader::isDatabaseOpen (void) const
{
  if (dbaseSock_ != NULL)
    return 1;
  return 0;
}

int
dbaseReader::databaseSelected (void) const
{
  if (dbaseDir_)
    return 1;
  return 0;
}

int
dbaseReader::insertRcServerToDbase (void)
{
  char qstring[256];
  char valstr [128];

  sprintf (qstring, "insert into %s\n",	DBASE_PROCESS_TABLE);
  sprintf (valstr, "values ('%s', 0, 'rcServer', 'RCS', '%s', %d, 'dormant', 0, 'yes', 'no')", run_.exptname(), run_.hostname(), run_.udpPort ());
  strcat  (qstring, valstr);

  //printf (DBASE_DBG,"qstring for insert rcServer info is:  \n");
  //printf (DBASE_DBG,"%s \n", qstring);

  if (::mysql_query (dbaseSock_, qstring) != 0) {
	//printf (DBASE_DBG,"insert rcServer into database error: %s\n", mysql_error(dbaseSock_));

    if (reconnectMysql () == CODA_ERROR) {
      reporter->cmsglog (CMSGLOG_ERROR,"Can't add myself to process table: %s\n", mysql_error(dbaseSock_));
      return CODA_ERROR;
    }
    if (::mysql_query (dbaseSock_, qstring) != 0) {
      reporter->cmsglog (CMSGLOG_ERROR,"Can't add myself to process table: %s\n", mysql_error(dbaseSock_));
      return CODA_ERROR;
    }

  }
  return CODA_SUCCESS;
}

int
dbaseReader::removeRcServerFromDbase (void)
{
  char qstring[256];
  char valstr [128];
  sprintf (qstring, "delete from %s\n",	DBASE_PROCESS_TABLE);
  sprintf (valstr, "where name = '%s'", run_.exptname() );
  strcat  (qstring, valstr);

  if (::mysql_query (dbaseSock_, qstring) != 0) {
    //printf (DBASE_DBG,"remove rcServer from database error: %s\n", mysql_error(dbaseSock_));

    if (reconnectMysql () == CODA_ERROR) {
      fprintf (stderr, "Can't remove myself from process table: %s\n", 
	       mysql_error(dbaseSock_));
      return CODA_ERROR;
    }
    if (::mysql_query (dbaseSock_, qstring) != 0) {
      fprintf (stderr, "Can't remove myself from process table: %s\n", 
	       mysql_error(dbaseSock_));
      return CODA_ERROR;
    }
  }
  return CODA_SUCCESS;
}

int
dbaseReader::listAllDatabases (void)
{
  char* dbases[DBASE_MAX_DBASES];
  int   numDbases = 0;
  MYSQL_RES *res;
  MYSQL_ROW row;

  res = mysql_list_dbs(dbaseSock_, NULL);
  if (!res) {
    //printf (DBASE_DBG,"List all database error: %s\n", mysql_error(dbaseSock_));
    reporter->cmsglog (CMSGLOG_ERROR,"Can't read list experiments from database:  %s\n", mysql_error(dbaseSock_));

    // reconnect if possible
    if (reconnectMysql () == CODA_ERROR)
      return CODA_ERROR;
    else {
      res = mysql_list_dbs(dbaseSock_, NULL);
      if (!res)
	return CODA_ERROR;
    }
  }

  int i = 0;
  while ((row = mysql_fetch_row (res))) {
    dbases[i] = new char[::strlen (row[0]) + 1];
#ifdef _RC_VERBOSE
    reporter->cmsglog (CMSGLOG_INFO1,"Found database %s .......\n",row[0]);
#endif
    ::strcpy (dbases[i], row[0]);
    i++;
  }
  numDbases = i;
  // free memory
  ::mysql_free_result(res);

  for (i = 0; i < numDbases; i++) 
    delete []dbases[i];
  return CODA_SUCCESS;
}
  


void
dbaseReader::database (char* path)
{
  if (path != 0) {
    if (::mysql_select_db(dbaseSock_, path) < 0) {
      fprintf (stderr, "Cannot select experiment %s: %s\n",
	       path, mysql_error(dbaseSock_));
      reporter->cmsglog (CMSGLOG_ERROR,"Can't select experiment %s: %s\n",
			       path, mysql_error(dbaseSock_)); 
	
      // reconnect to mysql server if it is the case of server crashed
      if (reconnectMysql () == CODA_ERROR) 
	return;
      if(::mysql_select_db(dbaseSock_, path) < 0) {
	fprintf (stderr, "Can't select database %s: %s\n",
		 path, mysql_error(dbaseSock_));
	reporter->cmsglog (CMSGLOG_ERROR,"Can't select database %s: %s\n",
				 path, mysql_error(dbaseSock_)); 
      }
      else {
	if (dbaseDir_)
	  delete []dbaseDir_;
	dbaseDir_ = new char[::strlen (path) + 1];
	::strcpy (dbaseDir_, path);
	// update database variable to new name
	run_.database (dbaseDir_);
    
      }
    }
    else {
      if (dbaseDir_)
	delete []dbaseDir_;
      dbaseDir_ = new char[::strlen (path) + 1];
      ::strcpy (dbaseDir_, path);
      // update database variable to new name
      run_.database (dbaseDir_);
      
    }
  }
}



int
dbaseReader::listAllSessions (void)
{
  char* sessions[DBASE_MAX_SESSIONS];
  int  sessionsActive[DBASE_MAX_SESSIONS];
  int  numSessions;
  char qstring[1024];
  MYSQL_RES *res = 0;
  MYSQL_ROW    row;

  ::sprintf (qstring, "select * from %s", DBASE_SESSION_TABLE);

  if (mysql_query (dbaseSock_, qstring) != 0) {
	//printf (DBASE_DBG,"list all sessions error: %s\n", mysql_error(dbaseSock_));

    if (reconnectMysql () == CODA_ERROR) {
      reporter->cmsglog (CMSGLOG_ERROR,"Can't read sessions table: %s\n", mysql_error(dbaseSock_));
      return CODA_ERROR;
    }
    if (::mysql_query (dbaseSock_, qstring) != 0) {
      reporter->cmsglog (CMSGLOG_ERROR,"Can't read sessions table: %s\n", mysql_error(dbaseSock_));
      return CODA_ERROR;
    }
  }

  res = mysql_store_result(dbaseSock_);
  if (!res) {
    //printf (DBASE_DBG,"List all sessions error: %s\n", mysql_error(dbaseSock_));

    reporter->cmsglog (CMSGLOG_ERROR,"Can't read sessions table: %s\n", mysql_error(dbaseSock_));
    return CODA_ERROR;
  }
  int i = 0;
  while ((row = mysql_fetch_row (res))) {
    sessions[i] = new char[::strlen(row[0]) + 1];
    ::strcpy (sessions[i], row[0]);
    if (::strcmp (row[3], "yes") == 0) {
      reporter->cmsglog (CMSGLOG_INFO1,"Found an active session %s\n", row[0]);
      sessionsActive[i] = 1;
    }
    else {
      reporter->cmsglog (CMSGLOG_INFO1,"Found an dormant session %s\n", row[0]);
      sessionsActive[i] = 0;
    }
    i++;
  }
  numSessions = i;

  // free result
  ::mysql_free_result (res);

  for (i = 0; i < numSessions; i++)
    delete []sessions[i];

  return CODA_SUCCESS;
}

int
dbaseReader::getAllSessions (void)
{
  if (!isDatabaseOpen ()) {
    reporter->cmsglog (CMSGLOG_ERROR,"Can't read sessions table: mysql server is not connected\n");
    return CODA_ERROR;
  }
  
  if (!databaseSelected ()) {
    reporter->cmsglog (CMSGLOG_ERROR,"Can't read sessions table: experiment is not selected\n");
    return CODA_ERROR;
  }

  reporter->cmsglog (CMSGLOG_INFO1,"Check experimental seesion table ......\n");
  return listAllSessions ();
}

int
dbaseReader::sessionCreated (char* name)
{
  char qstring[1024];
  MYSQL_RES *res = 0;
  MYSQL_ROW    row;

  ::sprintf (qstring, "select * from %s", DBASE_SESSION_TABLE);

  if (mysql_query (dbaseSock_, qstring) != 0) {
#ifdef _CODA_DEBUG
    printf ("list all sessions error: %s\n", mysql_error(dbaseSock_));
#endif
    reporter->cmsglog (CMSGLOG_ERROR,"Can't read sessions table: %s\n", mysql_error(dbaseSock_));
    
    // reconnect to mysql server if it is the case
    if (reconnectMysql () == CODA_ERROR) 
      return CODA_ERROR;
    if (mysql_query (dbaseSock_, qstring) != 0) {
      reporter->cmsglog (CMSGLOG_ERROR,"Can't read sessions table: %s\n", mysql_error(dbaseSock_));
      return CODA_ERROR;
    }
  }

  res = mysql_store_result(dbaseSock_);
  if (!res) {
#ifdef _CODA_DEBUG
    printf ("List all sessions error: %s\n", mysql_error(dbaseSock_));
#endif
    reporter->cmsglog (CMSGLOG_ERROR,"Can't read sessions table: %s\n", mysql_error(dbaseSock_));
    return CODA_ERROR;
  }
  while ((row = mysql_fetch_row (res))) {
    if (::strcmp (row[0], name) == 0) {
      reporter->cmsglog (CMSGLOG_INFO1,"Session %s already exists in db\n", row[0]);
      // free result
      ::mysql_free_result (res);
      return 1;
    }
  }
  // free result
  ::mysql_free_result (res);
  return 0;
}

int
dbaseReader::sessionActive (char* name)
{
  char qstring[1024];
  MYSQL_RES *res = 0;
  MYSQL_ROW    row;

  ::sprintf (qstring, "select * from %s", DBASE_SESSION_TABLE);

  if (mysql_query (dbaseSock_, qstring) != 0) {
#ifdef _CODA_DEBUG
    printf ("list all sessions error: %s\n", mysql_error(dbaseSock_));
#endif
    reporter->cmsglog (CMSGLOG_ERROR,"Can't read sessions table: %s\n", mysql_error(dbaseSock_));
    
    if (reconnectMysql () == CODA_ERROR)
      return CODA_ERROR;
    
    if (::mysql_query (dbaseSock_, qstring) != 0) {
      reporter->cmsglog (CMSGLOG_ERROR,"Can't read sessions table: %s\n", mysql_error(dbaseSock_));
      return CODA_ERROR;
    }
  }

  res = mysql_store_result(dbaseSock_);
  if (!res) {
#ifdef _CODA_DEBUG
    printf ("List all sessions error: %s\n", mysql_error(dbaseSock_));
#endif
    reporter->cmsglog (CMSGLOG_ERROR,"Can't read sessions table: %s\n", mysql_error(dbaseSock_));
    return CODA_ERROR;
  }
  while ((row = mysql_fetch_row (res))) {
    if (::strcmp (row[0], name) == 0 && ::strcmp (row[3], "yes") == 0) {
      reporter->cmsglog (CMSGLOG_INFO,"Session %s is already in use by %s\n", row[0],row[2]);
      // free result
      ::mysql_free_result (res);
      return 1;
    }
  }
  // free result
  ::mysql_free_result (res);
  return 0;
}

int
dbaseReader::createSession (char* name)
{
  if (sessionCreated (name)) {
    reporter->cmsglog (CMSGLOG_WARN,"Cannot create session %s: already exists\n",
			     name);
    return CODA_ERROR;
  }
  reporter->cmsglog (CMSGLOG_INFO1,"Create new session %s", name);

  // construct user information with hostname uname uid gid
  char userinfo[160];
  sprintf (userinfo, "%s %s %d %d", run_.hostname (), username_, uid_, gid_);

  char qstring[256];
  char vstring[128];
  sprintf (qstring, "insert into %s\n", DBASE_SESSION_TABLE);
  sprintf (vstring, "values ('%s', %d, '%s', 'yes', '%s_msg', 'RunControl', %d, '')",
	   name, exptid_, userinfo, name, 0);
  strcat  (qstring, vstring);

  if (mysql_query (dbaseSock_, qstring) != 0) {
#ifdef _CODA_DEBUG
    printf ("Create new session %s error: %s\n",name, mysql_error(dbaseSock_));
#endif
    reporter->cmsglog (CMSGLOG_ERROR,"Can't create session %s: %s\n",name, mysql_error(dbaseSock_));
    
    if (reconnectMysql () == CODA_ERROR) 
      return CODA_ERROR;
    if (mysql_query (dbaseSock_, qstring) != 0) {
      reporter->cmsglog (CMSGLOG_ERROR,"Can't create session %s: %s\n",
			       name, mysql_error(dbaseSock_));
      return CODA_ERROR;
    }
  }

  reporter->cmsglog (CMSGLOG_INFO1,"Finished\n");

  // get all components
  if (getComponents () != CODA_SUCCESS)
    return CODA_ERROR;
  // get all default priorities
  if (getPriorities () != CODA_SUCCESS)
    setDefaultPriorities ();
  // get all runtypes
  if (getAllRunTypes () != CODA_SUCCESS)
    return CODA_ERROR;
  // get run number
  if (getRunNumber () != CODA_SUCCESS)
    return CODA_ERROR;
  return CODA_SUCCESS;
}
    
int
dbaseReader::selectSession (char* name)
{
  if (!sessionCreated (name)) {
    reporter->cmsglog (CMSGLOG_INFO1,"This session: %s has not yet been created\n", name);
    return CODA_ERROR;
  }
  if (sessionActive (name)) {
    reporter->cmsglog (CMSGLOG_WARN,"Session %s is currently in use\n", name);
    return CODA_ERROR;
  }

  // construct user information with hostname uname uid gid
  char userinfo[160];
  sprintf (userinfo, "%s %s %d %d", run_.hostname (), username_, uid_, gid_);

#ifdef DEBUG_MSGS
  printf("userinfo->%s<-\n",userinfo);fflush(stdout);
#endif

  char qstring[256];
  sprintf (qstring, "update %s set inuse = 'yes', owner = '%s' where name = '%s'", 
	   DBASE_SESSION_TABLE, userinfo, name);

#ifdef DEBUG_MSGS
  printf("mysql_query->%s<-\n",qstring);fflush(stdout);
#endif

  if (mysql_query (dbaseSock_, qstring) != 0) {
#ifdef _CODA_DEBUG
    printf ("update session error: %s\n", mysql_error(dbaseSock_));
#endif
    reporter->cmsglog (CMSGLOG_ERROR,"Can't read session %s\n", name);

    if (reconnectMysql () == CODA_ERROR)
      return CODA_ERROR;
    if (mysql_query (dbaseSock_, qstring) != 0) {
      reporter->cmsglog (CMSGLOG_ERROR,"Can't read session %s\n", name);
      return CODA_ERROR;
    }
  }
  reporter->cmsglog (CMSGLOG_INFO1,"Selecting session %s succeeded\n", name);

  // get all components and all run types and so on
  if (getComponents () != CODA_SUCCESS)
    return CODA_ERROR;
  // get all priorities
  if (getPriorities () != CODA_SUCCESS)
    setDefaultPriorities ();
  // get all runtypes
  if (getAllRunTypes () != CODA_SUCCESS)
    return CODA_ERROR;
  // get run number
  if (getRunNumber () != CODA_SUCCESS)
    return CODA_ERROR;
  return CODA_SUCCESS;
}

int
dbaseReader::giveupSession (char* session)
{
  // Initially no session has been selected
  if (::strcmp (session, "unknown") == 0)
    return CODA_SUCCESS;

  char qstring[256];
  sprintf (qstring, "update %s set inuse = 'no', config = '' where name = '%s'", 
	   DBASE_SESSION_TABLE, session);

  if (mysql_query (dbaseSock_, qstring) != 0) {
#ifdef _CODA_DEBUG
    printf ("giving up session error: %s\n", mysql_error(dbaseSock_));
#endif
    reporter->cmsglog (CMSGLOG_ERROR,"Can't release session %s:\n", session);

    if (reconnectMysql () == CODA_ERROR)
      return CODA_ERROR;
    if (mysql_query (dbaseSock_, qstring) != 0) {
      reporter->cmsglog (CMSGLOG_ERROR,"Can't release session %s\n", session);
      return CODA_ERROR;
    }
  }
  return CODA_SUCCESS;
}

int
dbaseReader::getComponents (void)
{
  char qstring[1024];

  reporter->cmsglog (CMSGLOG_INFO1,"Parsing process table ....\n");

  sprintf (qstring, "select * from %s", DBASE_PROCESS_TABLE);

  if (::mysql_query (dbaseSock_, qstring) != 0) {
#ifdef _CODA_DEBUG
    printf ("list all processes error: %s\n",mysql_error(dbaseSock_));
#endif
    reporter->cmsglog (CMSGLOG_ERROR,"Can't read process table: %s\n", mysql_error(dbaseSock_));

    if (reconnectMysql () == CODA_ERROR)
      return CODA_ERROR;
    if (::mysql_query (dbaseSock_, qstring) != 0) {
      reporter->cmsglog (CMSGLOG_ERROR,"Can't read process table: %s\n", mysql_error(dbaseSock_));
      return CODA_ERROR;
    }
  }

  MYSQL_RES* res = mysql_store_result(dbaseSock_);
  if (!res) {
#ifdef _CODA_DEBUG
    printf ("list all process error: %s\n", mysql_error(dbaseSock_));
#endif
    reporter->cmsglog (CMSGLOG_ERROR,"Can't read process table: %s\n", mysql_error(dbaseSock_));
    return CODA_ERROR;
  }
  // get every row of the process table
  MYSQL_ROW row;
  while ((row = mysql_fetch_row (res))) {
    if (::strcasecmp (row[3], "RCS") != 0) {
      compFactory_->createComponent (row[0], atoi (row[1]), row[3], row[4],
				     row[2]);
      reporter->cmsglog (CMSGLOG_INFO1,"Creating component %s succeeded\n",row[0]);
    }
  }
  mysql_free_result (res);

  return CODA_SUCCESS;
}

int
dbaseReader::getPriorities (void)
{
  char qstring[64];

  reporter->cmsglog (CMSGLOG_INFO1,"Parsing priority table ......\n");
  ::sprintf (qstring, "select * from %s", DBASE_PRIORITY_TABLE);

  if (::mysql_query (dbaseSock_, qstring) != 0) {
#ifdef _CODA_DEBUG
    printf ("list all priorities error: %s\n", mysql_error(dbaseSock_));
#endif
    reporter->cmsglog (CMSGLOG_ERROR,"Can't read priority table: %s\n", mysql_error(dbaseSock_));

    if (reconnectMysql () == CODA_ERROR)
      return CODA_ERROR;
    if (::mysql_query (dbaseSock_, qstring ) != 0 ) {
      reporter->cmsglog (CMSGLOG_ERROR,"Can't read priority table: %s\n", mysql_error(dbaseSock_));
      return CODA_ERROR;
    }
  }

  MYSQL_RES *res = mysql_store_result(dbaseSock_);
  if (!res) {
#ifdef _CODA_DEBUG
    printf ("list all priorities error: %s\n", mysql_error(dbaseSock_));
#endif
    reporter->cmsglog (CMSGLOG_ERROR,"Can't read priority table: %s\n", mysql_error(dbaseSock_));
    return CODA_ERROR;
  }
  
  // get every row of the priority table
  MYSQL_ROW row;
  while ((row = mysql_fetch_row (res))) {
    compFactory_->subSystemPriority (row[0], atoi (row[1]));
    reporter->cmsglog (CMSGLOG_INFO1,"Subsystem %s has default priority of %s\n",
			     row[0], row[1]);
  }
  mysql_free_result (res);

  return CODA_SUCCESS;
}

void
dbaseReader::setDefaultPriorities (void)
{
  compFactory_->subSystemPriority ("ROC", 11);
  reporter->cmsglog (CMSGLOG_INFO1,"Subsystem ROC has default priority of %d\n", 11);
  compFactory_->subSystemPriority ("EB",  15);
  reporter->cmsglog (CMSGLOG_INFO1,"Subsystem EB has default priority of %d\n", 15);
  compFactory_->subSystemPriority ("ANA", 19);
  reporter->cmsglog (CMSGLOG_INFO1,"Subsystem ANA has default priority of %d\n", 19);
  compFactory_->subSystemPriority ("ER",  23);
  reporter->cmsglog (CMSGLOG_INFO1,"Subsystem ER has default priority of %d\n", 23);
  compFactory_->subSystemPriority ("LOG", 27);
  reporter->cmsglog (CMSGLOG_INFO1,"Subsystem LOG has default priority of %d\n", 27);
  compFactory_->subSystemPriority ("TS",  -27);
  reporter->cmsglog (CMSGLOG_INFO1,"Subsystem TS has default priority of %d\n", -27);
}

int 
dbaseReader::getAllRunTypes (void)
{
  char qstring[256];

  reporter->cmsglog (CMSGLOG_INFO1,"Parsing runtype table......\n");
  ::sprintf (qstring, "select * from %s", DBASE_RUNTYPE_TABLE);

  if (::mysql_query (dbaseSock_, qstring) != 0) {
#ifdef _CODA_DEBUG
    printf ("List all runtype error: %s\n", mysql_error(dbaseSock_));
#endif
    reporter->cmsglog (CMSGLOG_ERROR,"Can't read run types: %s\n", mysql_error(dbaseSock_));

    if (reconnectMysql () == CODA_ERROR)
      return CODA_ERROR;
    if (::mysql_query (dbaseSock_, qstring) != 0) {
      reporter->cmsglog (CMSGLOG_ERROR,"Can't read run types: %s\n", mysql_error(dbaseSock_));
      return CODA_ERROR;
    }
  }

  MYSQL_RES* res = mysql_store_result(dbaseSock_);
  if (!res) {
#ifdef _CODA_DEBUG
    printf ("List all runtype error: %s\n", mysql_error(dbaseSock_));
#endif
    reporter->cmsglog (CMSGLOG_ERROR,"Can't read run types: %s\n", mysql_error(dbaseSock_));
    return CODA_ERROR;
  }
  // get every row in the run type table
  MYSQL_ROW row;
  while ((row = mysql_fetch_row (res))) {
    if (::strcasecmp (row[2], "no") == 0) {
      reporter->cmsglog (CMSGLOG_INFO1,"Available runtype %s : %s has been added\n",
			       row[0], row[1]);
      if (!row[3] || !*row[3])
	run_.addRunType (row[0], atoi (row[1]), 0, 0);
      else
	run_.addRunType (row[0], atoi (row[1]), 0, row[3]);
    }
    else {
      reporter->cmsglog (CMSGLOG_INFO1,"Runtype %s : %s in use has been added\n",
			       row[0], row[1]);
      if (!row[3] || !*row[3])
	run_.addRunType (row[0], atoi (row[1]), 1, 0);
      else
	run_.addRunType (row[0], atoi (row[1]), 1, row[3]);
    }
  }
  mysql_free_result (res);

  // let daq run object setup all run types
  run_.setAllRunTypes ();

  return CODA_SUCCESS;
}

    

// call this routine after session has been selected
int
dbaseReader::getRunNumber   (void)
{
  char qstring[256];

  ::sprintf (qstring, "select runNumber from %s where name = '%s'",
	     DBASE_SESSION_TABLE, run_.exptname());

  if (::mysql_query (dbaseSock_, qstring) != 0) {
#ifdef _CODA_DEBUG
    printf ("Select runNumber error: %s\n", mysql_error(dbaseSock_));
#endif
    reporter->cmsglog (CMSGLOG_ERROR,"Can't read run number: %s\n", mysql_error(dbaseSock_));

    if (reconnectMysql () == CODA_ERROR) 
      return CODA_ERROR;
    if (::mysql_query (dbaseSock_, qstring) != 0) {
      reporter->cmsglog (CMSGLOG_ERROR,"Can't read run number: %s\n", mysql_error(dbaseSock_));
      return CODA_ERROR;
    }
  }

  MYSQL_RES* res = mysql_store_result(dbaseSock_);
  if (!res) {
#ifdef _CODA_DEBUG
    printf ("Select runNumber error: %s\n", mysql_error(dbaseSock_));
#endif
    reporter->cmsglog (CMSGLOG_ERROR,"Can't read run number: %s\n", mysql_error(dbaseSock_));
    return CODA_ERROR;
  }
  MYSQL_ROW row = mysql_fetch_row (res);

  assert (row[0]);

  run_.runNumber (atoi (row[0]));
  reporter->cmsglog (CMSGLOG_INFO,"Run number is %s\n", row[0]);
  ::mysql_free_result (res);

  return CODA_SUCCESS;
}

// call this routine after session has been selected
void 
dbaseReader::putRunNumber   (int number)
{
  char qstring[256];

  ::sprintf (qstring, "update %s set runNumber = %d where name = '%s'",
	     DBASE_SESSION_TABLE, number, run_.exptname());

  if (::mysql_query (dbaseSock_, qstring) != 0) {
#ifdef _CODA_DEBUG
    printf ("update runNumber error: %s\n", mysql_error(dbaseSock_));
#endif
    reporter->cmsglog (CMSGLOG_ERROR,"Can't write runNumber: %s\n", mysql_error(dbaseSock_));
    
    if (reconnectMysql () == CODA_SUCCESS) {
      if (::mysql_query (dbaseSock_, qstring) != 0) 
	reporter->cmsglog (CMSGLOG_ERROR,"Can't write runNumber: %s\n", mysql_error(dbaseSock_));
    }


/*sergey*/
printf("ERROR updating run number in database - exit\n");
exit(0);


  }
  reporter->cmsglog (CMSGLOG_INFO,"runNumber is now: %d\n", number);

}

// call this routine after session has been selected
void 
dbaseReader::putEventLimit   (int evlimit)
{
  char qstring[256];

  if (strcmp(run_.runtype (), "unknown") == 0) return;

  ::sprintf (qstring, "update %s_option set value = '%d' where name = '%s'",
	     run_.runtype (), evlimit, DBASE_EVENTLIMIT);

  if (::mysql_query (dbaseSock_, qstring) != 0) {
#ifdef _CODA_DEBUG
    printf ("update event limit error: %s\n", mysql_error(dbaseSock_));
#endif
    reporter->cmsglog (CMSGLOG_ERROR,"Can't write eventLimit: %s\n", mysql_error(dbaseSock_));
    
    if (reconnectMysql () == CODA_SUCCESS) {
      if (::mysql_query (dbaseSock_, qstring) != 0) 
	reporter->cmsglog (CMSGLOG_ERROR,"Can't write eventLimit: %s\n", mysql_error(dbaseSock_));
    }
  }
}

void 
dbaseReader::putDataLimit   (int dlimit)
{
  char qstring[256];
  if (strcmp(run_.runtype (), "unknown") == 0) return;

  ::sprintf (qstring, "update %s_option set value = '%d' where name = '%s'",
	     run_.runtype (), dlimit, DBASE_DATALIMIT);

  if (::mysql_query (dbaseSock_, qstring) != 0) {
#ifdef _CODA_DEBUG
    printf ("update data limit error: %s\n", mysql_error(dbaseSock_));
#endif
    reporter->cmsglog (CMSGLOG_ERROR,"Update  data limit error: %s\n", mysql_error(dbaseSock_));
    
    if (reconnectMysql () == CODA_SUCCESS) {
      if (::mysql_query (dbaseSock_, qstring) != 0) 
	reporter->cmsglog (CMSGLOG_ERROR,"Update dataLimit error: %s\n", mysql_error(dbaseSock_));
    }
  }
}

// call this routine after session has been selected
void 
dbaseReader::putDataFileName (char* name)
{
  char qstring[256];
  
  if (strcmp(run_.runtype (), "unknown") == 0) return;
  
  ::sprintf (qstring, "update %s_option set value = '%s' where name = '%s'",
	     run_.runtype (), name, DBASE_DATAFILE);

  if (::mysql_query (dbaseSock_, qstring) != 0) {
#ifdef _CODA_DEBUG
    printf ("update dataFile error: %s\n", mysql_error(dbaseSock_));
#endif
    reporter->cmsglog (CMSGLOG_ERROR,"Update dataFile error: %s\n", mysql_error(dbaseSock_));
    
    if (reconnectMysql () == CODA_SUCCESS) {
      if (::mysql_query (dbaseSock_, qstring) != 0) 
	reporter->cmsglog (CMSGLOG_ERROR,"Update dataFile error: %s\n", mysql_error(dbaseSock_));
    }
  }
}

void 
dbaseReader::putTokenInterval   (int itval)
{
  char qstring[256];

  if (strcmp(run_.runtype (), "unknown") == 0) return;

  ::sprintf (qstring, "update %s_option set value = '%d' where name = '%s'",
	     run_.runtype (), itval, DBASE_TOKEN_INTERVAL);

  if (::mysql_query (dbaseSock_, qstring) != 0) {
#ifdef _CODA_DEBUG
    printf ("update token interval error: %s\n", mysql_error(dbaseSock_));
#endif
    reporter->cmsglog (CMSGLOG_ERROR,"Update  token interval error: %s\n", mysql_error(dbaseSock_));
    
    if (reconnectMysql () == CODA_SUCCESS) {
      if (::mysql_query (dbaseSock_, qstring) != 0) 
	reporter->cmsglog (CMSGLOG_ERROR,"Update token interval error: %s\n", mysql_error(dbaseSock_));
    }
  }
}

int
dbaseReader::isConfigInUse (char* runtype)
{
  char qstring[1024];
  MYSQL_RES *res = 0;
  MYSQL_ROW    row;

  ::sprintf (qstring, "select * from %s", DBASE_RUNTYPE_TABLE);

  if (mysql_query (dbaseSock_, qstring) != 0) {
#ifdef _CODA_DEBUG
    printf ("list all runtypes error: %s\n", mysql_error(dbaseSock_));
#endif
    reporter->cmsglog (CMSGLOG_ERROR,"Database query error: %s\n", mysql_error(dbaseSock_));
    
    if (reconnectMysql () == CODA_ERROR)
      return CODA_ERROR;
    
    if (::mysql_query (dbaseSock_, qstring) != 0) {
      reporter->cmsglog (CMSGLOG_ERROR,"Database query error: %s\n", mysql_error(dbaseSock_));
      return CODA_ERROR;
    }
  }

  res = mysql_store_result(dbaseSock_);
  if (!res) {
#ifdef _CODA_DEBUG
    printf ("List all runtypes error: %s\n", mysql_error(dbaseSock_));
#endif
    reporter->cmsglog (CMSGLOG_ERROR,"Database query error: %s\n", mysql_error(dbaseSock_));
    return CODA_ERROR;
  }
  while ((row = mysql_fetch_row (res))) {
    if (::strcmp (row[0], runtype) == 0 && ::strcmp (row[2], "yes") == 0) {
      reporter->cmsglog (CMSGLOG_INFO,"Configuration %s is already in use.\n", row[0]);
      // free result
      ::mysql_free_result (res);
      // GHGH don't support inuse flag yet... 
      return 1;
    }
  }
  // free result
  ::mysql_free_result (res);
  return 0;
}

int 
dbaseReader::configure (char* runtype)
{
  // first clean out old configuration information
  static int ft = 1;
  static char oldtype[80];

  if (ft == 1) {
    ::strncpy (oldtype, runtype, sizeof (oldtype));
    ft = 0;
  }
  else {
    giveupConfiguration (oldtype);
    ::strncpy (oldtype, runtype, sizeof (oldtype));
  }	

  // clean old hashed information
  codaStrHashIterator ite (cinfos_);
  rcNetConfig* cf = 0;
  for (ite.init(); !ite; ++ite) {
    cf = (rcNetConfig *)ite ();
    delete cf;
  }
  cinfos_.deleteAllValues ();

  // check whether this run type is active due to other server
  if (isConfigInUse (runtype)) {
    //printf(DBASE_DBG,"inuse flag was set, but ignore for now\n");
    //return CODA_ERROR;
  }
  // get new information from the database
  int status = CODA_SUCCESS;

  reporter->cmsglog (CMSGLOG_INFO1,"Parsing configuration table %s ......",
			   runtype);

  char qstring[256];
  ::sprintf (qstring, "select * from %s", runtype);

  if (::mysql_query (dbaseSock_, qstring) != 0) {
#ifdef _CODA_DEBUG
    printf ("Select from %s configutation table error: %s\n", runtype, mysql_error(dbaseSock_));
#endif
    reporter->cmsglog (CMSGLOG_ERROR,"Select from %s configutation table error: %s\n", 
			     runtype, mysql_error(dbaseSock_));

    if (reconnectMysql () == CODA_ERROR)
      return CODA_ERROR;
    if (::mysql_query (dbaseSock_, qstring) != 0) {
      reporter->cmsglog (CMSGLOG_ERROR,"Select from %s configutation table error: %s\n", 
			       runtype, mysql_error(dbaseSock_));
      return CODA_ERROR;
    }
  }

  MYSQL_RES *res = mysql_store_result(dbaseSock_);
  if (!res) {
#ifdef _CODA_DEBUG
    printf ("Select from %s configutation table error: %s\n", runtype, mysql_error(dbaseSock_));
#endif
    reporter->cmsglog (CMSGLOG_ERROR,"Select from %s configutation table error: %s\n", 
			     runtype, mysql_error(dbaseSock_));
    return CODA_ERROR;
  }
  // get every row of the table
  MYSQL_ROW row;
  reporter->cmsglog (CMSGLOG_INFO,"Parsing %s configuration\n", runtype);
  while ((row = mysql_fetch_row (res))) {
    if (!compInsideHash (row[0])) {
      rcNetConfig* cf = 0;
      if (emptyLine (row[1])) 
	cf = new rcNetConfig (row[0], 0);
      else 
	cf = new rcNetConfig (row[0], row[1]);

      cinfos_.add (row[0], (void *)cf);
      reporter->cmsglog (CMSGLOG_INFO,"   activate component %s\n",row[0]);
    }
    else 
      reporter->cmsglog (CMSGLOG_WARN,"Duplicated entry %s\n", row[0]);
  }
  reporter->cmsglog (CMSGLOG_INFO,"Parsing %s finished\n", runtype);

  // free memory
  mysql_free_result (res);

  // set this run type inuse flag to yes
  sprintf (qstring, "update %s set inuse = 'yes' where name = '%s'", 
	   DBASE_RUNTYPE_TABLE, runtype);

  if (mysql_query (dbaseSock_, qstring) != 0) {
#ifdef _CODA_DEBUG
    printf ("update runtype error: %s\n", mysql_error(dbaseSock_));
#endif
    reporter->cmsglog (CMSGLOG_ERROR,"Selecting configuration %s failed\n", runtype);


    if (reconnectMysql () == CODA_ERROR)
      return CODA_ERROR;
    if (::mysql_query (dbaseSock_, qstring) != 0) {
      reporter->cmsglog (CMSGLOG_ERROR,"Selecting configuration %s failed\n", runtype);
      return CODA_ERROR;
    }
  }
  reporter->cmsglog (CMSGLOG_INFO1,"Selecting configuration %s succeeded\n", runtype);

  // set config field inside the session table to run type 'runtype'
  sprintf (qstring, "update %s set config = '%s' where name = '%s'", 
	   DBASE_SESSION_TABLE, runtype, run_.exptname ());

  if (mysql_query (dbaseSock_, qstring) != 0) {
#ifdef _CODA_DEBUG
    printf ("update session config error: %s\n", mysql_error(dbaseSock_));
#endif
    reporter->cmsglog (CMSGLOG_ERROR,"updating configuration %s failed\n", runtype);


    if (reconnectMysql () == CODA_ERROR)
      return CODA_ERROR;
    if (::mysql_query (dbaseSock_, qstring) != 0) {
      reporter->cmsglog (CMSGLOG_ERROR,"updating configuration %s failed\n", runtype);
      return CODA_ERROR;
    }
  }
  reporter->cmsglog (CMSGLOG_INFO1,"Updating configuration %s succeeded\n", runtype); 

  // set daq run run type name
  run_.runtype (runtype);

  return CODA_SUCCESS;
}

int
dbaseReader::giveupConfiguration (char* config)
{
  // Initially no configuration has been selected
  if (::strcmp (config, "unknown") == 0)
    return CODA_ERROR;

  char qstring[256];
  sprintf (qstring, "update %s set inuse = 'no' where name = '%s'", 
	   DBASE_RUNTYPE_TABLE, config);

  if (mysql_query (dbaseSock_, qstring) != 0) {
#ifdef _CODA_DEBUG
    printf ("giving up configuration error: %s\n", mysql_error(dbaseSock_));
#endif
    reporter->cmsglog (CMSGLOG_ERROR,"Giveup configuration %s failed\n", config);

    if (reconnectMysql () == CODA_ERROR)
      return CODA_ERROR;
    if (::mysql_query (dbaseSock_, qstring) != 0) {
      reporter->cmsglog (CMSGLOG_ERROR,"Giveup configuration %s failed\n", config);
      return CODA_ERROR;
    }
  }
  return CODA_SUCCESS;
}

int 
dbaseReader::parseOptions (char* runtype)
{
  // first clean out all old script components
  daqSystem& sys = run_.system ();
  sys.removeAllScriptComp ();

  // clean out script system
  daqScriptSystem& ssys = run_.scriptSystem ();
  ssys.cleanup ();

  char qstring[256];

  reporter->cmsglog (CMSGLOG_INFO1,"Parsing script table ......\n");
  ::sprintf (qstring, "select * from %s%s", runtype, DBASE_SCRIPT_TABLE);

  if (::mysql_query (dbaseSock_, qstring) != 0) {
#ifdef _CODA_DEBUG
    printf ("list all scripts error: %s\n", mysql_error(dbaseSock_));
#endif
    reporter->cmsglog (CMSGLOG_ERROR,"List all scripts error: %s\n", mysql_error(dbaseSock_));

    if (reconnectMysql () == CODA_ERROR)
      return CODA_ERROR;
    if (::mysql_query (dbaseSock_, qstring ) != 0 ) {
      reporter->cmsglog (CMSGLOG_ERROR,"List all scripts error: %s\n", mysql_error(dbaseSock_));
      return CODA_SUCCESS;
    }
  }

  MYSQL_RES *res = mysql_store_result(dbaseSock_);
  if (!res) {
#ifdef _CODA_DEBUG
    printf ("list all scripts error: %s\n", mysql_error(dbaseSock_));
#endif
    reporter->cmsglog (CMSGLOG_ERROR,"List all scripts error: %s\n", mysql_error(dbaseSock_));
    return CODA_SUCCESS;
  }
  
  // get every row of the priority table
  MYSQL_ROW row;
  while ((row = mysql_fetch_row (res))) {
    daqComponent *comp = 0;
    if (sys.has (row[0], comp) == CODA_SUCCESS) {
      daqComponent *scomp = 0;
      scomp = compFactory_->createComponent (comp, row[1], row[2]);    
      if (scomp == 0)
	reporter->cmsglog (CMSGLOG_ERROR,"Wrong action specification: %s\n",
				 row[1]);
      else
	reporter->cmsglog (CMSGLOG_INFO1,"%s user script component created\n",
				 scomp->title () );
    }
  }
  mysql_free_result (res);

  // parse option table
  int action;
  reporter->cmsglog (CMSGLOG_INFO1,"Parsing option table ......\n");
  ::sprintf (qstring, "select * from %s%s", runtype, DBASE_OPTION_TABLE);

  if (::mysql_query (dbaseSock_, qstring) != 0) {
#ifdef _CODA_DEBUG
    printf ("list all options error: %s\n", mysql_error(dbaseSock_));
#endif
    reporter->cmsglog (CMSGLOG_ERROR,"List all options error: %s\n", mysql_error(dbaseSock_));

    if (reconnectMysql () == CODA_ERROR)
      return CODA_ERROR;
    if (::mysql_query (dbaseSock_, qstring ) != 0 ) {
      reporter->cmsglog (CMSGLOG_ERROR,"List all options error: %s\n", mysql_error(dbaseSock_));
      return CODA_SUCCESS;
    }
  }

  res = mysql_store_result(dbaseSock_);
  if (!res) {
#ifdef _CODA_DEBUG
    printf ("list all options error: %s\n", mysql_error(dbaseSock_));
#endif
    reporter->cmsglog (CMSGLOG_ERROR,"List all options error: %s\n", mysql_error(dbaseSock_));
    return CODA_SUCCESS;
  }
  
  // get every row of the options table
  while ((row = mysql_fetch_row (res))) {
    //printf(DBASE_DBG,"Options Table Row %s",row[0]);
    if (::strcmp (row[0], DBASE_EVENTLIMIT) == 0) {
      int eventl;
      //printf(DBASE_DBG," Limit = %s\n",row[1]);
      if (::sscanf (row[1], "%d", &eventl) >= 1) {
	run_.eventLimit (eventl);
	if (eventl != 0) {
	  reporter->cmsglog (CMSGLOG_INFO1,"Event limit to %d\n", eventl);
	} else {
	  reporter->cmsglog (CMSGLOG_WARN,"No event count limit\n", eventl);
	}
      }
    }
    else if (::strcmp (row[0], DBASE_DATALIMIT) == 0) {
      int dl;
      //printf(DBASE_DBG," Limit = %s\n",row[1]);
      if (::sscanf (row[1], "%d", &dl) >= 1) {
	run_.dataLimit (dl);
	if (dl != 0) {
	  reporter->cmsglog (CMSGLOG_INFO1,"Data limit %d Kbytes\n", dl);
	} else {
	  reporter->cmsglog (CMSGLOG_WARN,"No data count limit\n", dl);
	}
      }
    }
    else if (::strcmp (row[0], DBASE_DATAFILE) == 0) {
      char filename[128];
      //printf(DBASE_DBG," File = %s\n",row[1]);
      if (::sscanf (row[1], "%s", filename) >= 1) {
	run_.setDataFileName (filename);
	reporter->cmsglog (CMSGLOG_INFO1,"Data file name %s \n", filename);
      }
    }
    else if (::strcmp (row[0], DBASE_TOKEN_INTERVAL) == 0) {
      int titval;
      //printf(DBASE_DBG," Token = %s\n",row[1]);
      if (::sscanf (row[1], "%d", &titval) >= 1) {
	run_.tokenInterval (titval, 0);
	reporter->cmsglog (CMSGLOG_INFO1,"Token interval %d \n", titval);
      }
    }
    else if ((action = codaDaqActions->action (row[0])) != CODA_ERROR) {
      //printf(DBASE_DBG," Action = %d Script = %s\n",action,row[1]);
      reporter->cmsglog (CMSGLOG_INFO,"Insert global %s transition script %s \n",
			 row[0], row[1]);
      ssys.addScript (action, row[1]);
    }
    else {
      //printf(DBASE_DBG," Value = %s\n",row[1]);
    }
  }
  mysql_free_result (res);

  return CODA_SUCCESS;
}
#else // coda_2_0 && coda_2_0_T
dbaseReader::dbaseReader (int exptid,
			  daqRun& run)
:dbaseDir_ (0), exptid_ (exptid), run_(run),
 cinfos_ (CODA_CONFIG_HASH_SIZE, codaStrHashFunc)
{
#ifdef _TRACE_OBJECTS
  printf ("Create dbaseReader Class Object\n");
#endif
  compFactory_ = new factory (exptid, run_.system());
}

dbaseReader::~dbaseReader (void)
{
#ifdef _TRACE_OBJECTS
  printf ("Delete dbaseReader Class Object\n");
#endif
  if (dbaseDir_)
    delete []dbaseDir_;
  delete compFactory_;
  codaStrHashIterator ite (cinfos_);
  rcNetConfig* cf = 0;

  for (ite.init(); !ite; ++ite) {
    cf = (rcNetConfig *)ite ();
    delete cf;
  }
}

int
dbaseReader::getComponents (void)
{
  int status = CODA_SUCCESS;
  char fullname [FILENAME_MAX_LEN];

  if (!dbaseDir_)
    return CODA_ERROR;

  // get rcNetwork filename
  ::strcpy (fullname, dbaseDir_);
  ::strcat (fullname, "/");
  ::strcat (fullname, CODA_NETWORK);
  FILE *fd = fopen (fullname, "r");  

  char line[BUFFER_SIZE];
  if (fd != 0) {
    reporter->cmsglog (CMSGLOG_INFO1,"Parsing rcNetwork file.....\n");
    while (!feof (fd)) {
      line[0] = '\0';
      fgets (line, sizeof (line), fd);
      if (line[0] != '!' && !emptyLine (line)) { // ignore comments
	char title[40], type[32], node[64], boot[128], bootpre[128];
	int  number;
	int status = sscanf (line, "%s %d %s %s %s",
			     title, &number, type, node, bootpre);
	// ignore LOG component which is not an real daqComponent
	if (status == 5) {
	  // get remaining of the line as boot script
	  ::strncpy (boot, ::strstr (line, bootpre), sizeof (boot));
	  // trim the bootString
	  trimBootString (boot);

	  if (::strcmp (node, "$NODE") == 0) {
	    char *nodenv = ::getenv ("NODE");
	    if (nodenv) {
	      ::strncpy (node, nodenv, sizeof (node));
	      compFactory_->createComponent (title, number, type,
					     node, boot);
	      reporter->cmsglog (CMSGLOG_INFO1,"Success: Component %s created\n",title);
	    }
	    else
	      reporter->cmsglog (CMSGLOG_ERROR,"Error: $NODE undefined \n");
	  }
	  else {
	    compFactory_->createComponent (title, number, type,
					   node, boot);
	    reporter->cmsglog (CMSGLOG_INFO1,"Success: Component %s created\n",title);
	  }
	}
	else if (status == 4) { // no boot script
	  reporter->cmsglog (CMSGLOG_WARN,"Warning: Component %s has no boot script\n", title);
	  compFactory_->createComponent (title, number, type, node, 0);
	}
	else
	  reporter->cmsglog (CMSGLOG_ERROR,"rcNetwork format error\n");
      }
    }
    reporter->cmsglog (CMSGLOG_INFO1,"Parsing rcNetwork file finished\n");
    fclose (fd);
  }
  else {
    reporter->cmsglog (CMSGLOG_ERROR,"no rcNetwork file\n");
    status = CODA_ERROR;
  }
  // Add priority information to all subsystems
  // get rcPriority information

  ::strcpy (fullname, dbaseDir_);
  ::strcat (fullname, "/");
  ::strcat (fullname, CODA_RUN_PRIORITY);
  fd = fopen (fullname, "r");  
  
  if (fd != 0) {
    reporter->cmsglog (CMSGLOG_INFO1,"Parsing rcPriority file......\n");
    while (!feof (fd)) {
      line[0] = '\0';
      fgets (line, sizeof (line), fd);
      if (line[0] != '!' && !emptyLine (line)) { // ignore comments
	char type[40];
	int  priority;
	int status = sscanf (line, "%s %d", type, &priority);
	if (status == 2) {
	  compFactory_->subSystemPriority (type, priority);
	  reporter->cmsglog (CMSGLOG_INFO1,"Success: subSystem %s has default priority %d\n",
				   type, priority);
	}
	else {
	  reporter->cmsglog (CMSGLOG_ERROR,"Error: Format Error\n");
	  status = CODA_ERROR;
	}
      }
    }
    fclose (fd);
  }
  else {
    reporter->cmsglog (CMSGLOG_ERROR,"No rcPriority File\n");
    status = CODA_ERROR;
  }
  return status;
}
  
int
dbaseReader::getAllRunTypes (void)
{
  int status = CODA_SUCCESS;
  char fullname [FILENAME_MAX_LEN];

  if (!dbaseDir_)
    return CODA_ERROR;

  // get rcRunTypes filename
  ::strcpy (fullname, dbaseDir_);
  ::strcat (fullname, "/");
  ::strcat (fullname, CODA_RUN_TYPES);
  FILE *fd = fopen (fullname, "r");  

  char line[BUFFER_SIZE];
  if (fd != 0) {
    reporter->cmsglog (CMSGLOG_INFO1,"Parsing rcRunTypes File......\n");
    while (!feof (fd)) {
      line[0] = '\0';
      fgets (line, sizeof (line), fd);
      if (line[0] != '!' && !emptyLine (line)) { // ignore comments  
	char runtype[80];
	int  runtypenum;
	int status = ::sscanf (line, "%s %d", runtype, &runtypenum);
	if (status == 2){
	  toLower (runtype);
	  run_.addRunType (runtype, runtypenum);
	  reporter->cmsglog (CMSGLOG_INFO1,"Runtype %s with type number %d has been added\n",
				   runtype, runtypenum);
	}
	else {
	  reporter->cmsglog (CMSGLOG_ERROR,"Format error\n");
	  status = CODA_WARNING;
	}
      }
    }
    fclose (fd);
    reporter->cmsglog (CMSGLOG_INFO1,"Parsing rcRunTypes file finished\n");
    // set whole run types for daqRun
    run_.setAllRunTypes ();
  }
  else {
    reporter->cmsglog (CMSGLOG_ERROR,"no rcRunTypes file\n");
    status = CODA_ERROR;
  }
  return status;
}

int
dbaseReader::getRunNumber (void)
{
  int status = CODA_SUCCESS;
  char fullname [FILENAME_MAX_LEN];

  if (!dbaseDir_)
    return 0;

  // get rcRunTypes filename
  ::strcpy (fullname, dbaseDir_);
  ::strcat (fullname, "/");
  ::strcat (fullname, CODA_RUN_NUMBER);
  FILE *fd = fopen (fullname, "r");  

  char line[BUFFER_SIZE];
  if (fd != 0) {
    reporter->cmsglog (CMSGLOG_INFO1,"Parsing rcRunNumber File......\n");
    while (!feof (fd)) {
      line[0] = '\0';
      fgets (line, sizeof (line), fd);
      if (line[0] != '!' && !emptyLine (line)) { // ignore comments  
	int number;
	int status = ::sscanf (line, "%d", &number);
	if (status == 1){
	  run_.runNumber (number);
	  reporter->cmsglog (CMSGLOG_INFO1,"Experiment run number is %d",
				   number);
	}
	else {
	  reporter->cmsglog (CMSGLOG_ERROR,"Format error\n");
	  status = CODA_WARNING;
	}
      }
    }
    fclose (fd);
    reporter->cmsglog (CMSGLOG_INFO1,"Parsing rcRunNumber file finished\n");
  }
  else {
    reporter->cmsglog (CMSGLOG_ERROR,"no rcRunNumber file\n");
    status = CODA_ERROR;
  }
  return status;
}

void
dbaseReader::putRunNumber (int number)
{
  char fullname [FILENAME_MAX_LEN];

  if (!dbaseDir_)
    return;

  // get rcRunTypes filename
  ::strcpy (fullname, dbaseDir_);
  ::strcat (fullname, "/");
  ::strcat (fullname, CODA_RUN_NUMBER);

  FILE *fd = fopen (fullname, "w");  

  if (fd != 0) {
    fprintf (fd, "%d", number);
    fclose (fd);
  }
}

void
dbaseReader::putDataLimit (int dl)
{
  char fullname [FILENAME_MAX_LEN];
  // internal memory buffer for option file
  char mbuffer  [40][256];  
  int  found = 0;
  int  i = 0;

  if (!dbaseDir_)
    return;

  // get rcRunTypes filename
  ::strcpy (fullname, dbaseDir_);
  ::strcat (fullname, "/");
  ::strcat (fullname, run_.runtype ());
  ::strcat (fullname, CODA_OPTIONS);
  FILE *fd = fopen (fullname, "r");  

  if (fd != 0) { 
    while (!feof (fd)) {
      
      if (i >= 40) {
	reporter->cmsglog (CMSGLOG_ERROR,
			   "Option file has too many lines\n");
	fclose (fd);
	return;
      }
      
      mbuffer[i][0] = '\0';
      fgets (mbuffer[i], sizeof (mbuffer[i]), fd);
      if (mbuffer[i][0] != '!' && !emptyLine (mbuffer[i])) { // ignore comments
	char title[40], action[80], script[128];
	int num = 0;

	// user specify script components
	if ((num = sscanf (mbuffer[i], "%s %s %s", title, action, script)) == 2) {
	  if (::strcmp (title, "dataLimit") == 0) {
	    found = 1;
	    sprintf (mbuffer[i], "dataLimit %d", dl);
	    reporter->cmsglog (CMSGLOG_INFO1,
	       "Data limit is set to %d (kbytes) in the option file\n", dl);
	  }
	}
      }
      i++;
    }
    fclose (fd);
  }

  // write new content to the same file
  fd = fopen (fullname, "w");    
  int   j;
  if (fd != 0) {
    for (j = 0; j < i; j++)
      fprintf (fd, "%s", mbuffer[j]);
    if (!found) {
      fprintf (fd, "dataLimit %d", dl);
      reporter->cmsglog (CMSGLOG_INFO1,
			 "Data limit is set to %d (kbytes) in the option file\n", dl);
    }
    fclose (fd);
  }
}

void
dbaseReader::putEventLimit (int el)
{
  char fullname [FILENAME_MAX_LEN];
  // internal memory buffer for option file
  char mbuffer  [40][256];  
  int  found = 0;
  int  i = 0;

  if (!dbaseDir_)
    return;

  // get rcRunTypes filename
  ::strcpy (fullname, dbaseDir_);
  ::strcat (fullname, "/");
  ::strcat (fullname, run_.runtype ());
  ::strcat (fullname, CODA_OPTIONS);
  FILE *fd = fopen (fullname, "r");  

  if (fd != 0) { 
    while (!feof (fd)) {
      
      if (i >= 40) {
	reporter->cmsglog (CMSGLOG_ERROR,
			   "Option file has too many lines\n");
	fclose (fd);
      }
      
      mbuffer[i][0] = '\0';
      fgets (mbuffer[i], sizeof (mbuffer[i]), fd);
      if (mbuffer[i][0] != '!' && !emptyLine (mbuffer[i])) { // ignore comments
	char title[40], action[80], script[128];
	int num = 0;

	// user specify script components
	if ((num = sscanf (mbuffer[i], "%s %s %s", title, action, script)) == 2) {
	  if (::strcmp (title, "eventLimit") == 0) {
	    found = 1;
	    sprintf (mbuffer[i], "eventLimit %d", el);
	    reporter->cmsglog (CMSGLOG_INFO1,
			       "Event limit is set to %d in the option file\n",
			       el);
	  }
	}
      }
      i++;
    }
    fclose (fd);
  }

  // write new content to the same file
  fd = fopen (fullname, "w");    
  int   j;
  if (fd != 0) {
    for (j = 0; j < i; j++)
      fprintf (fd, "%s", mbuffer[j]);
    if (!found) {
      fprintf (fd, "eventLimit %d", el);
      reporter->cmsglog (CMSGLOG_INFO1,
			 "Event limit is set to %d in the option file\n",
			 el);
    }
    fclose (fd);
  }
}
  

int
dbaseReader::configure (char* runtype)
{
  // clean up hashed information
  codaStrHashIterator ite (cinfos_);
  rcNetConfig* cf = 0;
  for (ite.init(); !ite; ++ite) {
    cf = (rcNetConfig *)ite ();
    delete cf;
  }
  cinfos_.deleteAllValues ();

  int status = CODA_SUCCESS;
  char fullname [FILENAME_MAX_LEN];

  // get rcRunTypes filename
  ::strcpy (fullname, dbaseDir_);
  ::strcat (fullname, "/");
  ::strcat (fullname, runtype);
  ::strcat (fullname, CODA_CONFIG);
  FILE *fd = fopen (fullname, "r");  

  if (fd != 0) { 
    reporter->cmsglog (CMSGLOG_INFO1,"Parsing configuration file %s.config ......",
			     runtype);
    char line[128];
    while (!feof (fd)) {
      line[0] = '\0';
      fgets (line, sizeof (line), fd);
      if (line[0] != '!' && !emptyLine (line)) { // ignore comments
	char title[40], config[128];
	if (sscanf (line, "%s %s", title, config) == 1) {
	  rcNetConfig* cf = 0;
	  if (!compInsideHash (title) ) {
#ifdef __ultrix
	    cf = new rcNetConfig (title, "unknown");
#else
	    cf = new rcNetConfig (title, 0);
#endif
	    cinfos_.add (title, (void *)cf);
	    reporter->cmsglog (CMSGLOG_INFO1,"Activate component %s\n",title);
	  }
	  else 
	    reporter->cmsglog (CMSGLOG_WARN,"Duplicated entry %s\n",title);
	}
	else {
	  rcNetConfig* cf = 0;
	  if (!compInsideHash (title)) {
	    char *nstr = expandFilename (config);
	    cf = new rcNetConfig (title, nstr);
	    cinfos_.add (title, (void *)cf);
	    reporter->cmsglog (CMSGLOG_INFO1,"Activate component %s\n",title);
	  }
	  else
	    reporter->cmsglog (CMSGLOG_ERROR,"Duplicated entry %s\n",title);
	}
      }
    }
    fclose (fd);
    reporter->cmsglog (CMSGLOG_INFO1,"Parsing %s.config file finished\n",runtype);
    run_.runtype (runtype);
    // create or update all ANA log file information
    run_.createAnaLogVars ();
  }
  else {
    reporter->cmsglog (CMSGLOG_ERROR,"no configuration file %s.config\n",
			     runtype);
    status = CODA_ERROR;
  }
  return status;
}

int
dbaseReader::parseOptions (char* runtype)
{
  // first clean out all old script components
  daqSystem& sys = run_.system ();
  sys.removeAllScriptComp ();

  // clean out script system
  daqScriptSystem& ssys = run_.scriptSystem ();
  ssys.cleanup ();

  int status = CODA_SUCCESS;
  char fullname [FILENAME_MAX_LEN];
  int  saction;

  if (!dbaseDir_)
    return CODA_ERROR;

  // get rcRunTypes filename
  ::strcpy (fullname, dbaseDir_);
  ::strcat (fullname, "/");
  ::strcat (fullname, runtype);
  ::strcat (fullname, CODA_OPTIONS);
  FILE *fd = fopen (fullname, "r");  

  if (fd != 0) { 
    reporter->cmsglog (CMSGLOG_INFO1,"Parsing option file %s%s ......",
		       runtype, CODA_OPTIONS);
    char line[256];
    while (!feof (fd)) {
      line[0] = '\0';
      fgets (line, sizeof (line), fd);
      if (line[0] != '!' && !emptyLine (line)) { // ignore comments
	char title[40], action[80], script[128];
	int num = 0;

	// user specify script components
	if ((num = sscanf (line, "%s %s %s", title, action, script)) == 3) {
	  if (::strcasecmp (title, "system") == 0) { // success script
	    if ((saction = codaDaqAction->action (action)) != CODA_ERROR) {
	      // set script as the remnant of this line
	      strcpy (script, ::strstr (line, script));
	      ssys.addScript (saction, script);
	    }
	  }
	  else {
	    daqComponent *comp = 0;
	    if (sys.has (title, comp) == CODA_SUCCESS) {
	      daqComponent *scomp = 0;
	      // set script as the remnant of this line
	      strcpy (script, ::strstr (line, script));
	      scomp = compFactory_->createComponent (comp, action, script);
	      if (scomp == 0)
		reporter->cmsglog (CMSGLOG_WARN,"Wrong action specification: %s\n",
				   action);
	      else
		reporter->cmsglog (CMSGLOG_INFO1,"%s user script component created",
				   scomp->title () );
	    }
	    else
	      reporter->cmsglog (CMSGLOG_ERROR,"Cannot find component %s\n",
				 title);
	  }
	}
	else if (num == 2) {
	  if (::strcmp (title, "runNumber") == 0) {
	    if (::strcmp (action, "increment") == 0) {
	      reporter->cmsglog (CMSGLOG_WARN,"Automatic run number increment enabled\n");
	      run_.autoIncrement (1);
	    }
	    else {
	      run_.autoIncrement (0);
	      reporter->cmsglog (CMSGLOG_WARN,"Automatic run number increment disabled\n");
	    }
	  }
	  else if (::strcmp (title, "eventLimit") == 0) {
	    int limit = 0;
	    if (::sscanf (action, "%d", &limit) == 1) {
	      run_.eventLimit (limit);
	      if (limit) {
		reporter->cmsglog (CMSGLOG_WARN,"Event limit is set to %d\n", limit);
	      } else {
		reporter->cmsglog (CMSGLOG_WARN,"No limit on event number\n", limit);
	      }	       
	    }   
	  }
	  else if (::strcmp (title, "dataLimit") == 0) {
	    int limit = 0;
	    // data limit in kBytes
	    if (::sscanf (action, "%d", &limit) == 1) {
	      run_.dataLimit (limit);
	      if (limit) {
		reporter->cmsglog (CMSGLOG_WARN,"Data limit is set to %d (kbytes)\n", limit);
	      } else {
		reporter->cmsglog (CMSGLOG_WARN,"No limit on data count\n", limit);
	      }
	    }
	  }
	  else if (::strcmp (title, "updateInterval") == 0) {
	    int interval = 0;
	    if (::sscanf (action, "%d", &interval) == 1) {
	      run_.dataUpdateInterval (interval);
	      reporter->cmsglog (CMSGLOG_WARN,"Data updating interval is %d (sec)\n", 
				       interval);
	    }
	  }
	}
	else
	  reporter->cmsglog (CMSGLOG_ERROR,"Wrong format in %s%s file\n",
				   runtype, CODA_OPTIONS);
      }
    }
    fclose (fd);
    reporter->cmsglog (CMSGLOG_INFO1,"Parsing %s%s file finished\n",
			     runtype, CODA_OPTIONS);
  }
  else {
    reporter->cmsglog (CMSGLOG_ERROR,"no options file %s%s\n",
			     runtype, CODA_OPTIONS);
    status = CODA_ERROR;
  }
  return status;
}


void
dbaseReader::database (char* path)
{
  if (dbaseDir_)
    delete []dbaseDir_;
  dbaseDir_ = new char[::strlen (path) + 1];
  ::strcpy (dbaseDir_, path);
}
#endif

char* 
dbaseReader::database (void) const
{
  return dbaseDir_;
}

int
dbaseReader::configured (char *title)
{
  // make sure all script components are enabled
  if (::strstr (title, CODA_USER_SCRIPT) != 0)
    return CODA_SUCCESS;

  // find out which config info belongs to this component 'title'
  codaSlist& nlist = cinfos_.bucketRef (title);
  if (nlist.isEmpty ())
    return CODA_ERROR;
  else {
    codaSlistIterator ite (nlist);
    rcNetConfig* cf = 0;

    for (ite.init (); !ite; ++ite) {
      cf = (rcNetConfig *) ite ();
      if (::strcmp (cf->title (), title) == 0)
	return CODA_SUCCESS;
    }
  }
  return CODA_ERROR;
}

int
dbaseReader::getNetConfigInfo (char *title, char* &config)
{
  // find out which config info that belongs to this component 'title'
  codaSlist& nlist = cinfos_.bucketRef (title);
  if (nlist.isEmpty ()) {
    config = 0;
    return CODA_ERROR;
  }
  else {
    codaSlistIterator ite (nlist);
    rcNetConfig* cf = 0;

    for (ite.init(); !ite; ++ite) {
      cf = (rcNetConfig *)ite ();
      if (::strcmp (title, cf->title ()) == 0) {
	config = cf->config ();
	return CODA_SUCCESS;
      }
    }
  }
  return CODA_ERROR;
}

int
dbaseReader::setNetConfigInfo (char* title, char* config)
{
  codaSlist& nlist = cinfos_.bucketRef (title);

  if (nlist.isEmpty ())
    return CODA_ERROR;
  else {
    codaSlistIterator ite (nlist);
    rcNetConfig* cf = 0;
    
    for (ite.init (); !ite; ++ite) {
      cf = (rcNetConfig *) ite ();
      if (::strcmp (title, cf->title ()) == 0) {
	cf->config (config);
	return CODA_SUCCESS;
      }
    }
  }
  return CODA_ERROR;
}

int
dbaseReader::compInsideHash (char* title)
{
  int found = 0;

  codaSlist& nlist = cinfos_.bucketRef (title);
  if (nlist.isEmpty ())
    return 0;

  codaSlistIterator ite (nlist);
  rcNetConfig* cf = 0;

  for (ite.init (); !ite; ++ite) {
    cf = (rcNetConfig *) ite ();
    if (::strcmp (cf->title (), title) == 0) 
      return 1;
  }
  return 0;
}
