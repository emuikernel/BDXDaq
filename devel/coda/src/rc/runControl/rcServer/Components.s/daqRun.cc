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
//      DAQ Run Class Implementation
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: daqRun.cc,v $
//   Revision 1.25  1999/12/10 21:22:15  rwm
//   int i at function scope.
//
//   Revision 1.24  1999/11/29 16:13:42  rwm
//   Cast to quiet compiler warnings.
//
//   Revision 1.23  1999/11/22 16:46:49  rwm
//   Fix scoping warning
//
//   Revision 1.22  1999/02/25 14:36:59  rwm
//   Limits defined in daqRunLimits.h
//
//   Revision 1.21  1999/02/04 16:22:32  rwm
//   Turn on cmlog message logging by default.
//
//   Revision 1.20  1999/02/02 19:02:11  heyes
//   AUTOEND feature
//
//   Revision 1.19  1998/11/05 20:11:42  heyes
//   reverse status updating to use UDP, fix other stuff
//
//   Revision 1.18  1998/09/18 15:05:43  heyes
//   no real changes
//
//   Revision 1.17  1998/06/02 19:51:45  heyes
//   fixed rcServer
//
//   Revision 1.16  1998/05/28 17:47:15  heyes
//   new GUI look
//
//   Revision 1.15  1997/08/25 15:57:32  heyes
//   use dplite.h
//
//   Revision 1.14  1997/07/22 19:38:54  heyes
//   cleaned up lots of things
//
//   Revision 1.13  1997/06/13 21:30:44  heyes
//   for marki
//
//   Revision 1.12  1997/05/23 16:45:05  heyes
//   add SESSION env variable, remove coda_activate
//
//   Revision 1.11  1997/05/16 16:04:08  chen
//   add global script capability
//
//   Revision 1.8  1997/02/18 17:07:23  chen
//   fix a minor memory leaks
//
//   Revision 1.7  1997/02/03 13:45:58  heyes
//   add ask command
//
//   Revision 1.6  1997/01/24 16:36:02  chen
//   change/add Log Component for 1.4
//
//   Revision 1.5  1996/12/04 18:32:42  chen
//   port to 1.4 on hp and ultrix
//
//   Revision 1.4  1996/11/05 17:37:48  chen
//   bootable flag is added to daqComponent
//
//   Revision 1.3  1996/11/04 16:13:48  chen
//   add options for monitoring components
//
//   Revision 1.2  1996/10/31 15:56:07  chen
//   Fixing boot stage bug + reorganize code
//
//   Revision 1.1.1.1  1996/10/11 13:39:18  chen
//   run control source
//
//
#include <dbaseReader.h>
#include <daqDataUpdateFunc.h>
#include <rcMsg.h>
#include <rccAcceptor.h>
#include <daqSubSystem.h>
#include <rcsAnaLogVarWriter.h>
#include <rcsNumEvTrigger.h>
#include <rcsNumLongTrigger.h>
#include <daqCompMonitor.h>
#include "daqRun.h"
#include "codaCompClnt.h"
#include <rcMsgReporter.h>

#ifdef solaris
extern "C" int gethostname (char*, int);
#endif

//============================================================================
//         Implementation of class runType
//============================================================================
#if defined (_CODA_2_0_T) || defined (_CODA_2_0)
daqRunType::daqRunType (void)
:type_ (0), number_ (-1), inuse_ (0), cat_ (0)
{
#ifdef _TRACE_OBJECTS
	printf ("Create daqRunType Class Object\n");
#endif
	// empty
}

daqRunType::daqRunType (char* type, int number, int inuse, char* cat)
:number_ (number), inuse_ (inuse)
{
#ifdef _TRACE_OBJECTS
	printf ("Create daqRunType Class Object\n");
#endif
	type_ = new char[::strlen (type) + 1];
	::strcpy (type_, type);

	if (cat) {
		cat_ = new char[::strlen (cat) + 1];
		::strcpy (cat_, cat);
	}
	else
		cat_ = 0;
}

daqRunType::~daqRunType (void)
{
#ifdef _TRACE_OBJECTS
	printf ("Delete daqRunType Class Object\n");
#endif
	if (type_)
		delete []type_;
	if (cat_)
		delete []cat_;
}

daqRunType::daqRunType (const daqRunType& type)
:number_ (type.number_), inuse_ (type.inuse_)
{
#ifdef _TRACE_OBJECTS
	printf ("Create daqRunType Class Object\n");
#endif
	if (type.type_) {
		type_ = new char[::strlen (type.type_) + 1];
		::strcpy (type_, type.type_);
	}
	else
		type_ = 0;

	if (type.cat_) {
		cat_ = new char[::strlen (type.cat_) + 1];
		::strcpy (cat_, type.cat_);
	}
	else
		cat_ = 0;
}

daqRunType&
daqRunType::operator = (const daqRunType& type)
{
	if (this != &type) {
		if (type_)
			delete []type_;
		if (cat_)
			delete []cat_;

		number_ = type.number_;
		inuse_ = type.inuse_;

		if (type.type_) {
			type_ = new char[::strlen (type.type_) + 1];
			::strcpy (type_, type.type_);
		}
		else
			type_ = 0;

		if (type.cat_) {
			cat_ = new char[::strlen (type.cat_) + 1];
			::strcpy (cat_, type.cat_);
		}
		else
			cat_ = 0;
	}
	return *this;
}
#else
daqRunType::daqRunType (void)
:type_ (0), number_ (-1)
{
#ifdef _TRACE_OBJECTS
	printf ("Create daqRunType Class Object\n");
#endif
	// empty
}

daqRunType::daqRunType (char* type, int number)
:number_ (number)
{
#ifdef _TRACE_OBJECTS
	printf ("Create daqRunType Class Object\n");
#endif
	type_ = new char[::strlen (type) + 1];
	::strcpy (type_, type);
}

daqRunType::~daqRunType (void)
{
#ifdef _TRACE_OBJECTS
	printf ("Delete daqRunType Class Object\n");
#endif
	if (type_)
		delete []type_;
}

daqRunType::daqRunType (const daqRunType& type)
:number_ (type.number_)
{
#ifdef _TRACE_OBJECTS
	printf ("Create daqRunType Class Object\n");
#endif
	if (type.type_) {
		type_ = new char[::strlen (type.type_) + 1];
		::strcpy (type_, type.type_);
	}
	else
		type_ = 0;
}

daqRunType&
daqRunType::operator = (const daqRunType& type)
{
	if (this != &type) {
		if (type_)
			delete []type_;
		number_ = type.number_;
		if (type.type_) {
			type_ = new char[::strlen (type.type_) + 1];
			::strcpy (type_, type.type_);
		}
		else
			type_ = 0;
	}
	return *this;
}
#endif

//==========================================================================
//           Implementation of class daqRun
//==========================================================================
#if defined (_CODA_2_0_T) || defined (_CODA_2_0)
daqRun::daqRun (char* dbase, char* ename, int expid, char* msqlhost)
:system_ (), exptid_ (expid), datafileName_ (0), 
 dataManager_ (), dataUpdater_ (), scriptSystem_ (), dbreader_ (0),
 numtypes_ (0), numDvars_ (0), locked_ (0), cmdBuffer (), netMan_ (0),
 port_ (0)
{
#ifdef _TRACE_OBJECTS
	printf ("    Create daqRun Class Objects\n");
#endif
	dbasename_ = new char[::strlen (dbase) + 1];
	::strcpy (dbasename_, dbase);

	exptname_ = new char[::strlen (ename) + 1];
	::strcpy (exptname_, ename);

	// setup database msql daemon host
	if (msqlhost) {
		msqlhost_ = new char[::strlen (msqlhost) + 1];
		::strcpy (msqlhost_, msqlhost);
	}

	// connect to msqld if possible.

	// create components monitor
	monitor_ = new daqCompMonitor (*this);

	// send pointer of this daqRun which is the only one to daqSystem
	system_.run (this);
	// Create all daq data associated with run
	createAllVariables ();
	// set data manager pointer inside updater
	dataUpdater_.dataManager (&dataManager_);
}
#else
daqRun::daqRun (char* ename, int expid)
:system_ (), exptid_ (expid), dataManager_ (), dataUpdater_ (), 
 scriptSystem_ (), dbreader_ (0),
 numtypes_ (0), numDvars_ (0), locked_ (0), cmdBuffer (), netMan_ (0),
 port_ (0)
{
#ifdef _TRACE_OBJECTS
	printf ("    Create daqRun Class Objects\n");
#endif
	// setup experiment name
	exptname_ = new char[::strlen (ename) + 1];
	::strcpy (exptname_, ename);

	// create components monitor
	monitor_ = new daqCompMonitor (*this);

	// send pointer of this daqRun which is the only one to daqSystem
	system_.run (this);
	// Create all daq data associated with run
	createAllVariables ();
	// set data manager pointer inside updater
	dataUpdater_.dataManager (&dataManager_);
}
#endif

daqRun::~daqRun (void)
{
#ifdef _TRACE_OBJECTS
	printf ("    Delete daqRun Class Objects\n");
#endif
	delete []exptname_;

#if defined (_CODA_2_0_T) || defined (_CODA_2_0)
	delete []msqlhost_;
	delete []dbasename_;
	if (datafileName_)
		delete []datafileName_;
#endif

	// remove monitor
	delete monitor_;

	// don't free dbreader
	deleteAllVariables ();

	// all writers will be deleted inside the data variables
}

void
daqRun::abort (int wanted)
{
	system_.abort (wanted);
}

int
daqRun::autostart (void)
{
	return system_.autostart ();
}

int
daqRun::boot (void)
{
	return system_.boot ();
}

int
daqRun::preConfigure (char *type)
{
	// remove all dynamic variables
	removeDynamicVars ();
	// reset all components to initial state
	system_.disableAllComponents ();
	// numtypes_ = 0;
	if (dbreader_->configure (type) != CODA_SUCCESS)
		return CODA_ERROR;
	dbreader_->parseOptions (type);
	return CODA_SUCCESS;
}

int
daqRun::configure (char *type)
{
	system_.configure ();
	// setup all enabled components name
	setAllCompNames ();
	// setup all components' auto boot information
	setAllCompBootInfo ();
	// setup all components' monitored parameters
	setAllMonitorParms ();
	// do not setup all dynamic variables since they are
	// not avialable until downloaded

	// free char of type
	delete []type;
	return CODA_SUCCESS;
}

int
daqRun::download (void)
{
	return system_.download ();
}

int
daqRun::go (void)
{
	return system_.go ();
}

int
daqRun::pause (void)
{
	return system_.pause ();
}

int
daqRun::prestart (void)
{
	return system_.prestart ();
}

int
daqRun::reset (void)
{
	return system_.reset ();
}

int
daqRun::end (void)
{
	return system_.end ();
}

int
daqRun::terminate (void)
{
	return system_.terminate ();
}

int
daqRun::autoTransition (int istate, int fstate)
{
	return system_.autoTransition (istate, fstate);
}

int
daqRun::cancelTransition (void)
{
	return system_.cancelTransition ();
}

daqSystem&
daqRun::system (void)
{
	return system_;
}

daqDataManager&
daqRun::dataManager (void)
{
	return dataManager_;
}

daqDataUpdater&
daqRun::dataUpdater (void)
{
	return dataUpdater_;
}

daqScriptSystem&
daqRun::scriptSystem (void)
{
	return scriptSystem_;
}

void
daqRun::createAllVariables (void)
{
	char serverHost[64];
	int  len = sizeof (serverHost);
	if (::gethostname (serverHost, len) != 0)
		::strcpy (serverHost, "unknown");

	version_ = new rcsDaqData (exptname_, "version", CODA_VERSION);
	startTime_ = new rcsDaqData (exptname_, "startTime", "      ");
	startTimeBin_ = new rcsDaqData (exptname_, "startTimeBin", 0);
	currTime_ = new rcsDaqData  (exptname_, "currentTime", "       ");
	currTimeBin_ = new rcsDaqData (exptname_, "timeBin", 0);
	endTime_ = new rcsDaqData (exptname_, "endTime", "      ");
	endTimeBin_ = new rcsDaqData (exptname_, "endTimeBin", 0);
	runNumber_ = new rcsDaqData (exptname_, "runNumber", 0);
	status_ = new rcsDaqData (exptname_, "status", CODA_DORMANT);
	nevents_ = new rcsDaqData (exptname_, "nevents", 0);
	nlongs_ =  new rcsDaqData (exptname_, "nlongs", 0);
#if defined (_CODA_2_0_T) || defined (_CODA_2_0)
	database_ = new rcsDaqData (exptname_, "database", dbasename_);
	dataFile_ = new rcsDaqData (exptname_, "dataFile", "unknown");
	rcsMsgToDbase_ = new rcsDaqData (exptname_, "rcsMsgToDbase", 1);  
	tokenInterval_ = new rcsDaqData (exptname_, "tokenInterval", 0);
	logFileDescriptor_ = new rcsDaqData (exptname_, "logFileDescriptor",
		"unknown");

	daqRunTypeStruct runtypeInfo;
	runTypeInfo_ = new rcsDaqData   (exptname_, "runTypeInfo", &runtypeInfo);
#endif
	allRunTypes_ = new rcsDaqData (exptname_, "allRunTypes", "unknown");
	runType_ = new rcsDaqData(exptname_, "runType", "unknown");
	runTypeNum_ = new rcsDaqData (exptname_, "runTypeNum", 0);
	exptName_ = new rcsDaqData (exptname_, "exptName", exptname_);
	exptId_ = new rcsDaqData (exptname_, "exptId", exptid_);
	hostName_ = new rcsDaqData (exptname_, "hostName", serverHost);
	autoIncrement_ = new rcsDaqData (exptname_, "autoIncrement", 1);
	eventLimit_ = new rcsDaqData (exptname_, "eventLimit", 0);
	dataLimit_ = new rcsDaqData (exptname_, "dataLimit", 0);
	updateInterval_ = new rcsDaqData (exptname_, "updateInterval", 2);
	compnames_ = new rcsDaqData (exptname_, "components","unknown");
	clientList_ = new rcsDaqData (exptname_, "clientList", "unknown");
	master_ = new rcsDaqData (exptname_, "master", "unknown");
	controlDisp_ = new rcsDaqData (exptname_, "controlDisplay", "unknown");
	online_ = new rcsDaqData (exptname_, "online", 1);
	runMsg_ = new rcsDaqData (exptname_, "runMessage","");

	// create data structure bootinfo which has all information
	// about auto boot flag information of components
	daqCompBootStruct bootInfo;
	compBootInfo_ = new rcsDaqData (exptname_, "compBootInfo", &bootInfo);

	// create daq run monitoring parameters structure
	daqMonitorStruct monitorInfo;
	monitorParms_ = new rcsDaqData (exptname_, "monitorParms", &monitorInfo);


	// create assocaited data base writer
	// this writer will be freed from assocaited data
	runNumberWriter_ = new rcsRunNumberWriter (this);
	runNumber_->writer (runNumberWriter_);
	runNumber_->enableWrite ();
	// create associated writer
	// this writer will be freed from associated data
	// this writer will handle stop and restarting updating mechanism
	updateIWriter_ = new rcsUpdateIWriter (this);
	updateInterval_->writer (updateIWriter_);
	// create eventlimit/datalimit writer
	evlimitWriter_ = new rcsEvLimitWriter (this);
	eventLimit_->writer (evlimitWriter_);
	datalimitWriter_ = new rcsDataLimitWriter (this);
	dataLimit_->writer (datalimitWriter_);
	// enable write
	updateInterval_->enableWrite ();
	autoIncrement_->enableWrite ();
	eventLimit_->enableWrite ();
	dataLimit_->enableWrite ();
	online_->enableWrite ();
	runType_->enableWrite ();

	currTimeBin_->connect (dataManager_);
	currTime_->connect (dataManager_);
	startTime_->connect (dataManager_);
	startTimeBin_->connect (dataManager_);
	endTime_->connect (dataManager_);
	endTimeBin_->connect (dataManager_);
	version_->connect (dataManager_);
	runNumber_->connect (dataManager_);
	status_->connect (dataManager_);
	nevents_->connect (dataManager_);
	nlongs_->connect (dataManager_);
#if defined (_CODA_2_0_T) || defined (_CODA_2_0)
	database_->connect (dataManager_);
	dataFile_->connect (dataManager_);
	rcsMsgToDbase_->connect (dataManager_);
	runTypeInfo_->connect (dataManager_);

	logFileDescriptor_->connect (dataManager_);
	logFileDescriptor_->enableWrite ();

	tokenInterval_->connect (dataManager_);
	tokenInterval_->enableWrite ();

	rcsMsgToDbase_->enableWrite ();

		logfdesWriter_ = new rcsLogFileDesWriter (this);
	logFileDescriptor_->writer (logfdesWriter_);  

	tokenIWriter_ = new rcsTokenIntervalWriter (this);
	tokenInterval_->writer (tokenIWriter_);
#endif
	allRunTypes_->connect (dataManager_);
	runType_->connect (dataManager_);
	runTypeNum_->connect (dataManager_);
	exptName_->connect (dataManager_);
	exptId_->connect (dataManager_);
	hostName_->connect (dataManager_);
	autoIncrement_->connect (dataManager_);
	eventLimit_->connect (dataManager_);
	dataLimit_->connect (dataManager_);
	updateInterval_->connect (dataManager_);
	compnames_->connect (dataManager_);
	clientList_->connect (dataManager_);
	master_->connect (dataManager_);
	controlDisp_->connect (dataManager_);
	online_->connect (dataManager_);
	compBootInfo_->connect (dataManager_);
	monitorParms_->connect (dataManager_);
	runMsg_->connect (dataManager_);

	int updateI = (int)(*updateInterval_);
	currTime_->activate ();
	currTimeBin_->activate ();
	currTime_->updater (serverTime);
	currTimeBin_->updater (serverTimeBin);
	startTime_->updater (serverTime); 
	startTimeBin_->updater (serverTimeBin);
	endTime_->updater (serverTime); 
	endTimeBin_->updater (serverTimeBin); 
	startTime_->keepHistory ();
	startTimeBin_->keepHistory ();
	endTime_->keepHistory ();
	endTimeBin_->keepHistory ();

	dataUpdater_.updateInterval (1000*updateI);
	dataUpdater_.startUpdating ();
}

void
daqRun::resetAllVariables (void)
{
	*startTime_ = "     ";
	*endTime_ = "     ";
	*startTimeBin_ = 0;
	*endTimeBin_ = 0;
	*status_ = CODA_DORMANT;
	*nevents_ = 0;
	*nlongs_ = 0;
	*allRunTypes_ = "unknown";
	*runType_ = "unknown";
	*runTypeNum_ =  0;
	*autoIncrement_ = 1;
	*eventLimit_ = 0;
	*dataLimit_ = 0;
	*compnames_ = "unknown";
	*runMsg_ = "";
#if defined (_CODA_2_0) || defined (_CODA_2_0_T)
	*dataFile_ = "unknown";
	*tokenInterval_ = 0;
	*logFileDescriptor_ = "unknown";
	if (datafileName_)
		delete []datafileName_;
	datafileName_ = 0;
#endif

	daqCompBootStruct tmp;				   
	*compBootInfo_ = &tmp; // clean up the boot information

	// clean up the monitoring parameters
	// without change the autoend and monitoring timer interval
	daqMonitorStruct mtmp;
	if (monitor_->autoend ())
		mtmp.enableAutoEnd ();
	else
		mtmp.disableAutoEnd ();
	mtmp.timerInterval (monitor_->timerInterval ());
	*monitorParms_ = &mtmp;

#if defined (_CODA_2_0) || defined (_CODA_2_0_T)
	// reset session name to no use
		dbreader_->giveupSession (exptname_);
	// for coda 2 exptname = session
#endif

	// reset all dynamic variables
	if (numDvars_ != 0) {
		for (int i = 0; i < numDvars_; i++) {
			// stop getting numbers from components
			dvars_[i]->disableTrigger ();
			dvars_[i]->deactivate ();
		}
	}
}

void
daqRun::createDynamicVars (void)
{
  // assume no more than MAX_NUM_DYN_VARS components at a time.
  char *cs[MAX_NUM_DYN_VARS];
  char *newComp[MAX_NUM_DYN_VARS]; // really new components
  int bufsize = MAX_NUM_DYN_VARS;
  int count = 0;
  int newcount = 0;
  daqData* serverData = 0;
  int i;

  numDvars_ = 0;

  count = system_.allEnabledComponents (cs, bufsize);
  if (count != 0) {
    for (i = 0; i < count; i++) {
      if (dataManager_.findData (cs[i], DYN_ATTR0, serverData) == CODA_SUCCESS){
	dvars_[numDvars_++] = serverData;
	serverData->notifyChannels ();
      }
      else {
	daqData* ndata = new rcsDaqData (cs[i], DYN_ATTR0, 0);
	ndata->connect (dataManager_);
	// ndata->updater (compAttr);
	// create trigger for nlong, insert into the new data
	rcsNumLongTrigger* trigger = new rcsNumLongTrigger (this);
	ndata->trigger (trigger);

	dvars_[numDvars_++] = ndata;
      }
      if (dataManager_.findData (cs[i], DYN_ATTR1, serverData) == CODA_SUCCESS){
	dvars_[numDvars_++] = serverData;
	serverData->notifyChannels ();
      }
      else {
	daqData* edata = new rcsDaqData (cs[i], DYN_ATTR1, 0);
	edata->connect (dataManager_);
	//edata->updater (compAttr);
	// create trigger for nevents, insert into the new data
	rcsNumEvTrigger* trigger = new rcsNumEvTrigger (this);
	edata->trigger (trigger);

	dvars_[numDvars_++] = edata;
      }
      if (dataManager_.findData (cs[i], DYN_ATTR2, serverData) == CODA_SUCCESS){
	dvars_[numDvars_++] = serverData;
	serverData->notifyChannels ();
	// no need to copy the content of cs[i], client side will handle adding
	// dynamic variable appropriatly
	newComp[newcount++] = cs[i];
      }
      else {
	daqData* sdata = new rcsDaqData (cs[i], DYN_ATTR2, CODA_DORMANT);
	sdata->connect (dataManager_);
	dvars_[numDvars_++] = sdata;
	// no need to copy the content of cs[i]
	newComp[newcount++] = cs[i];
      }
      if (dataManager_.findData (cs[i], DYN_ATTR3, serverData) == CODA_SUCCESS){
	dvars_[numDvars_++] = serverData;
	serverData->notifyChannels ();
	// no need to copy the content of cs[i], client side will handle adding
	// dynamic variable appropriatly
	newComp[newcount++] = cs[i];
      }
      else {
	daqData* sdata = new rcsDaqData (cs[i], DYN_ATTR3, 0.0);
	sdata->connect (dataManager_);
	dvars_[numDvars_++] = sdata;
	// no need to copy the content of cs[i]
	newComp[newcount++] = cs[i];
      }

      if (dataManager_.findData (cs[i], DYN_ATTR4, serverData) == CODA_SUCCESS){
	dvars_[numDvars_++] = serverData;
	serverData->notifyChannels ();
	// no need to copy the content of cs[i], client side will handle adding
	// dynamic variable appropriatly
	newComp[newcount++] = cs[i];
      }
      else {
	daqData* sdata = new rcsDaqData (cs[i], DYN_ATTR4, 0.0);
	sdata->connect (dataManager_);
	dvars_[numDvars_++] = sdata;
	// no need to copy the content of cs[i]
	newComp[newcount++] = cs[i];
      }


      /*sergey*/
      if (dataManager_.findData (cs[i], DYN_ATTR5, serverData) == CODA_SUCCESS){
	dvars_[numDvars_++] = serverData;
	serverData->notifyChannels ();
	// no need to copy the content of cs[i], client side will handle adding
	// dynamic variable appropriatly
	newComp[newcount++] = cs[i];
      }
      else {
	daqData* sdata = new rcsDaqData (cs[i], DYN_ATTR5, 0.0);
	sdata->connect (dataManager_);
	dvars_[numDvars_++] = sdata;
	// no need to copy the content of cs[i]
	newComp[newcount++] = cs[i];
      }



    }

    if (newcount > 0) {
      // tell all clients all dynamic variables' name
      // clients may already have them, 
      // but the clients will handle these correctly
      daqNetData data (exptname_, "command", newComp, newcount);
      netMan_->sendToAllClients ((int)DAADD_VARS, data, 0);
    }
    // free memory of cs
    for (i = 0; i < count; i++) {
      delete []cs[i];
    }
  }
}

void
daqRun::setStatusVariable (char* comp, int st)
{
	daqData* serverData = 0;

	if (dataManager_.findData (comp, DYN_ATTR2, serverData) == CODA_SUCCESS) 
		*serverData = st;
}

void
daqRun::stopUpdatingDynamicVars (void)
{
	for (int i = 0; i < numDvars_; i++) {
		dvars_[i]->disableTrigger ();
		dvars_[i]->deactivate ();
	}
}

void
daqRun::startUpdatingDynamicVars (void)
{
	if (isOnline ()) {
		for (int i = 0; i < numDvars_; i++) {
			if (dvars_[i]->monitored ()) {
#ifdef _CODA_DEBUG
				printf ("dynamic variables %s %s\n",dvars_[i]->compname(),
					dvars_[i]->attrname ());      
#endif
				dvars_[i]->enableTrigger ();
				dvars_[i]->activate ();
			}
		}
	}
}

void
daqRun::startMonitoringComp (void)
{
	monitor_->startMonitoringComp ();
}

void
daqRun::endMonitoringComp (void)
{
	monitor_->endMonitoringComp ();
	monitor_->enableDisconnectedComp ();
}

void
daqRun::updateDynamicVars (void)
{
	if (isOnline ()) {
		for (int i = 0; i < numDvars_; i++) 
			if (dvars_[i]->monitored ()) {
#ifdef _CODA_DEBUG
				printf ("Force to update dynamic variables %s %s\n",
					dvars_[i]->compname(),
					dvars_[i]->attrname ());      
#endif
				if (dvars_[i]->active ())
					dvars_[i]->update ();
			}
	}
}  

void
daqRun::removeDynamicVars (void) {
  int i;

  if (numDvars_ != 0) {
    // first stop updating
    for (i = 0; i < numDvars_; i++) {
      dvars_[i]->disableTrigger ();
      dvars_[i]->deactivate ();
    }
    
    char *cs[MAX_NUM_DYN_VARS];
    for (i = 0; i < numDvars_; i++) 
      cs[i] = dvars_[i]->compname ();
    
    // remove client side
    daqNetData data (exptname_, "command", cs, numDvars_);
    netMan_->sendToAllClients ((int)DAREMOVE_VARS, data, 0);
    
    // reset dynamic variables to zero without destroying them
    numDvars_ = 0;
  }
}

int
daqRun::numDynamicVars (void) const
{
	return numDvars_;
}

void
daqRun::createAnaLogVars (void)
{
#if defined (_CODA_2_0_T) || defined (_CODA_2_0)
	return;
#else
	// no way a run control system has more than MAX_NUM_DYN_VARS ANAs
	char *anas[MAX_NUM_DYN_VARS];
	int  numNewAnas = 0;

	daqSubSystem* anasys = 0;

	if (system_.locateSystem (CODA_ANA_CLASS, anasys) == CODA_SUCCESS) {
		codaSlistIterator ite (anasys->compList());
		daqComponent* comp = 0;
		char* log = 0;
		daqData* logdata = 0;

		for (ite.init (); !ite; ++ite) {
			comp = (daqComponent *) ite();
			getNetConfigInfo (comp->title(), log);
			if (dataManager_.findData (comp->title(), DYN_ANA_LOG, logdata) 
				== CODA_SUCCESS) {
				if (log != 0) {
					*logdata = log;
					logdata->notifyChannels ();
				}
				else {
					*logdata = "unkown";
					logdata->notifyChannels ();	  
				}
			}
			else {
				daqData *ldata = 0;
				daqDataWriter *lwriter = 0;
				if (log != 0) 
					ldata = new rcsDaqData (comp->title(), DYN_ANA_LOG, log);
				else
					ldata = new rcsDaqData (comp->title(), DYN_ANA_LOG, "unknown");
				lwriter = new rcsAnaLogVarWriter (this);
				ldata->connect (dataManager_);
				ldata->enableWrite ();
				// hook up the writer for this variable
				ldata->writer (lwriter);

					anas[numNewAnas++] = comp->title();
			}
		} 
		if (numNewAnas > 0) {
			// tell all clients all ANA components with log information
			daqNetData data (exptname_, "command", anas, numNewAnas);
			netMan_->sendToAllClients ((int)DAADD_ANALOG_VARS, data, 0);
		} 
	}
#endif
}

void
daqRun::removeAnaLogVars (void)
{
#if defined (_CODA_2_0_T) || defined (_CODA_2_0)
	return;
#else
	// no way a run control system has more than MAX_NUM_DYN_VARS ANAs
	char *anas[MAX_NUM_DYN_VARS];
	int  numAnas = 0;

	daqSubSystem* anasys = 0;

	if (system_.locateSystem (CODA_ANA_CLASS, anasys) == CODA_SUCCESS) {
		codaSlistIterator ite (anasys->compList());
		daqComponent* comp = 0;
		daqData* logdata = 0;

		for (ite.init (); !ite; ++ite) {
			comp = (daqComponent *)ite ();
			if (dataManager_.findData (comp->title(), DYN_ANA_LOG, logdata) 
				== CODA_SUCCESS) 
				anas[numAnas++] = comp->title();
		}
		if (numAnas > 0) {
			// tell all clients all ANA components with log information
			daqNetData data (exptname_, "command", anas, numAnas);
			netMan_->sendToAllClients ((int)DAREMOVE_ANALOG_VARS, data, 0);
		}
	}
#endif
}

void
daqRun::updateAnaLogInfo (char* ana, char* logpath)
{
#if defined (_CODA_2_0_T) || defined (_CODA_2_0)
	return;
#else
	dbreader_->setNetConfigInfo (ana, logpath);
#endif
}

void
daqRun::sendAnaLogVarsInfo (rccIO *chan)
{
#if defined (_CODA_2_0_T) || defined (_CODA_2_0)
	return;
#else
	// no way a run control system has more than MAX_NUM_DYN_VARS ANAs
	char *anas[MAX_NUM_DYN_VARS];
	int  numNewAnas = 0;

	daqSubSystem* anasys = 0;

	if (system_.locateSystem (CODA_ANA_CLASS, anasys) == CODA_SUCCESS) {
		codaSlistIterator ite (anasys->compList());
		daqComponent* comp = 0;
		char* log = 0;
		daqData* logdata = 0;

		for (ite.init (); !ite; ++ite) {
			comp = (daqComponent *)ite ();
			getNetConfigInfo (comp->title(), log);
			if (dataManager_.findData (comp->title(), DYN_ANA_LOG, logdata) 
				== CODA_SUCCESS) {
				if (log != 0) {
					*logdata = log;
					logdata->notifyChannels ();
				}
				else {
					*logdata = "unkown";
					logdata->notifyChannels ();	  
				}
			}
			else {
				daqData *ldata = 0;
				if (log != 0) 
					ldata = new rcsDaqData (comp->title(), DYN_ANA_LOG, log);
				else
					ldata = new rcsDaqData (comp->title(), DYN_ANA_LOG, "unknown");
				ldata->connect (dataManager_);
			}
			anas[numNewAnas++] = comp->title();
		}
		if (numNewAnas > 0) {
			// tell all clients all ANA components with log information
			daqNetData data (exptname_, "command", anas, numNewAnas);
			chan->sendResult ((int)DAADD_ANALOG_VARS, data, 0);
		}
	}
#endif
}
  
void
daqRun::deleteAllVariables (void)
{
	dataUpdater_.stopUpdating ();
	dataManager_.freeAll ();
	dataManager_.cleanAll ();
}

char*
daqRun::exptname (void) const
{
	static char en[128];
	::strncpy (en, (char *)(*exptName_), sizeof (en));
	return en;
}

int
daqRun::exptid (void) const
{
	return (int)(*exptId_);
}

char*
daqRun::hostname (void) const
{
	static char host[80];
	int len = sizeof (host);

	if(::gethostname (host, len) != 0)
		::strcpy (host, "unknown");

	return host;
}

char*
daqRun::controlDisplay (void) const
{
	static char disp[128];

	::strncpy (disp, (char *)(*controlDisp_), sizeof (disp));
	return disp;
}

#if defined (_CODA_2_0_T) || defined (_CODA_2_0)
char*
daqRun::msqlhost (void) const
{
	return msqlhost_;
}

void
daqRun::database (char* dbase)
{
	*database_ = dbase;
}

char*
daqRun::database (void) const
{
	static char dn[128];
	::strncpy (dn, (char *)(*database_), sizeof (dn));
	return dn;
}

int
daqRun::createSession (char* session)
{
	return dbreader_->createSession (session);
}

int
daqRun::selectSession (char* session)
{
	return dbreader_->selectSession (session);
}

void
daqRun::tokenInterval (int itval, int writeUpdate)
{
	// if not write to database, disable the write
	if (!writeUpdate) 
		tokenInterval_->disableWrite ();

	*tokenInterval_ = itval;

	if (!writeUpdate) 
		tokenInterval_->enableWrite ();  
}
#endif

void
daqRun::runtype (char *name)
{
	*runType_ = name;
	for (int i = 0; i < numtypes_; i++) {
		if (::strcmp (name, runtypes_[i].type_) == 0) {
			*runTypeNum_ = runtypes_[i].number_;
			break;
		}
	}
}

char*
daqRun::runtype (void) const
{
	static char rn[128];
	strncpy (rn, (char *)(*runType_), sizeof (rn));
	return rn;
}

int
daqRun::runtypeNum (void) const
{
	return (int)(*runTypeNum_);
}

void
daqRun::autoIncrement (int flag)
{
	if (flag >= 1) 
		*autoIncrement_ = 1;
	else 
		*autoIncrement_ = 0;
}

int
daqRun::autoIncrement (void) const
{
	return (int)(*autoIncrement_);
}

void
daqRun::online (void)
{
	*online_ = 1;
}

void
daqRun::offline (void)
{
	*online_ = 0;
}

int
daqRun::isOnline (void) const
{
	return (int)(*online_);
}

void
daqRun::eventLimit (int nevent)
{
	*eventLimit_ = nevent;
}

void
daqRun::dataLimit (int dl)
{
	*dataLimit_ = dl;
}

int
daqRun::eventLimit (void) const
{
	return (int)(*eventLimit_);
}

int
daqRun::dataLimit (void) const
{
	return (int) (*dataLimit_);
}

#if defined (_CODA_2_0_T) || defined (_CODA_2_0)
void
daqRun::dataFile (char *file)
{
	*dataFile_ = file;
}

char*
daqRun::dataFile (void) const
{
	static char ff[256];
	::strncpy (ff, (char *)(*dataFile_), sizeof (ff));
	return ff;
}

void
daqRun::dataFile (int number)
{
	// if datafile name is not empty, update dataFile variable
	if (datafileName_) {
		char realfilename[128];
		sprintf (realfilename, datafileName_, number);
		dataFile (realfilename);
	}
}

void
daqRun::writeDataFileNameToDbase (char* file)
{
	dbreader_->putDataFileName (file);

	if (datafileName_)
		delete []datafileName_;
	datafileName_ = 0;

	if (file) {
		datafileName_ = new char[::strlen (file) + 1];
		::strcpy (datafileName_, file);
		dataFile (runNumber ());
	}
	else 
		dataFile ("unknown");
}

void
daqRun::setDataFileName (char* name)
{
	if (datafileName_)
		delete []datafileName_;
	datafileName_ = 0;

	if (name) {
		datafileName_ = new char[::strlen (name) + 1];
		::strcpy (datafileName_, name);
		dataFile (runNumber ());

		// prevent writing back the same thing to the database
		logFileDescriptor_->disableWrite ();
		*logFileDescriptor_ = name;
		logFileDescriptor_->enableWrite ();
	}
	else {
		logFileDescriptor_->disableWrite ();
		*logFileDescriptor_ = "unknown";
		logFileDescriptor_->enableWrite ();
	}
}

void
daqRun::updateTokenInterval (int itval)
{
	dbreader_->putTokenInterval (itval);
}

void
daqRun::enableRcsMsgToDbase (void)
{
	*rcsMsgToDbase_ = 1;
}

void
daqRun::disableRcsMsgToDbase (void)
{
	*rcsMsgToDbase_ = 0;
}

int
daqRun::rcsMsgToDbase (void) const
{
	return (int) (*rcsMsgToDbase_);
}
#endif

void
daqRun::runNumber (int num)
{
	*runNumber_ = num;

#if defined (_CODA_2_0_T) || defined (_CODA_2_0)
	dataFile (num);
#endif
}

void
daqRun::dataUpdateInterval (int sec)
{
	// first stop updating
	dataUpdater_.stopUpdating ();
	// set new update interval
	*updateInterval_ = sec;
	// start new updating
	dataUpdater_.updateInterval (1000*sec);
	dataUpdater_.startUpdating ();  
}

void
daqRun::restartUpdating (int sec)
{
	// first stop updating
	dataUpdater_.stopUpdating ();
	// start new updating
	dataUpdater_.updateInterval (1000*sec);
	dataUpdater_.startUpdating ();  
}

int
daqRun::dataUpdateInterval (void) const
{
	return (int)(*updateInterval_);
}

int
daqRun::runNumber (void) const
{
	return (int)(*runNumber_);
}

void
daqRun::increaseRunNumber (void)
{
	// increment the run number
	int oldNum = (int)(*runNumber_);
	oldNum++;
	*runNumber_ = oldNum;

#if defined (_CODA_2_0_T) || defined (_CODA_2_0)
	dataFile (oldNum);
#endif
}  

void
daqRun::updateRunNumber (int number)
{
	dbreader_->putRunNumber (number);
}

void
daqRun::updateEventLimit (int evl)
{
	dbreader_->putEventLimit (evl);
}

void
daqRun::updateDataLimit (int dl)
{
	dbreader_->putDataLimit (dl);
}

void
daqRun::status (int st)
{
	*status_ = st;
}

int
daqRun::status (void) const
{
	return (int)(*status_);
}

void
daqRun::eventNumber (int en)
{
	*nevents_ = en;
}

int
daqRun::eventNumber (void) const
{
	return (int)(*nevents_);
}

void
daqRun::longWords (int lwd)
{
	*nlongs_ = lwd;
}

int
daqRun::longWords (void) const
{
	return (int)(*nlongs_);
}

void
daqRun::setStartTime (void)
{
	// update startTime
	startTime_->update ();
	startTimeBin_->update ();
}

void
daqRun::setEndTime (void)
{
	// update endTime
	endTime_->update ();
	endTimeBin_->update ();
}

void
daqRun::eraseStartTime (void)
{
	*startTime_ = "            ";
	*startTimeBin_ = 0;
}

void
daqRun::eraseEndTime (void)
{
	*endTime_ = "             ";
	*endTimeBin_ = 0;
}

#if defined (_CODA_2_0_T) || defined (_CODA_2_0)
void
daqRun::addRunType (char* runtype, int number, int inuse, char* cat)
{
	daqRunType rt (runtype, number, inuse, cat);

	runtypes_[numtypes_++] = rt;
}

void
daqRun::setAllRunTypes (void)
{
	int i = 0;

	if (numtypes_ > 1) {
		char **temp = new char*[numtypes_];
		for (i = 0; i < numtypes_; i++) 
			temp[i] = runtypes_[i].type_;
		allRunTypes_->assignData (temp, numtypes_);
		delete []temp;
	}
	else if (numtypes_ == 1) 
		*allRunTypes_ = runtypes_[0].type_;


	// update run type information structure
	daqRunTypeStruct runtypeInfo;

	for (i = 0; i < numtypes_; i++) 
		runtypeInfo.insertRunType (runtypes_[i].type_, runtypes_[i].number_,
			runtypes_[i].inuse_, runtypes_[i].cat_);
	*runTypeInfo_ = (daqArbStruct *)&runtypeInfo;

	daqArbStruct* bs = (daqArbStruct *)(*runTypeInfo_);
	daqRunTypeStruct* rs = (daqRunTypeStruct *)bs;
}
#else
void
daqRun::addRunType (char* runtype, int number)
{
	daqRunType rt (runtype, number);

	runtypes_[numtypes_++] = rt;
}

void
daqRun::setAllRunTypes (void)
{
	if (numtypes_ > 1) {
		char **temp = new char*[numtypes_];
		for (int i = 0; i < numtypes_; i++) 
			temp[i] = runtypes_[i].type_;
		allRunTypes_->assignData (temp, numtypes_);
		delete []temp;
	}
	else if (numtypes_ == 1) {
		*allRunTypes_ = runtypes_[0].type_;
	}
}
#endif


void
daqRun::setAllCompNames (void)
{
	// assume no more than MAX_NUM_DYN_VARS components at a time
	char *cs[MAX_NUM_DYN_VARS];
	int bufsize = MAX_NUM_DYN_VARS;
	int count = 0;

	count = system_.allEnabledComponents (cs, bufsize);
	if (count != 0) {
		compnames_->assignData (cs, count);
		for (int i = 0; i < count; i++)
			delete []cs[i];
	}
	else
		*compnames_ = "unknown";
}

void
daqRun::setAllCompBootInfo (void)
{
	// assume no more than MAX_NUM_DYN_VARS components at a time
	daqComponent* comp[MAX_NUM_DYN_VARS];
	int bufsize = MAX_NUM_DYN_VARS;
	int count = 0;
	daqCompBootStruct bootInfo;

	count = system_.allEnabledComponents (comp, bufsize);
	if (count != 0) {
		for (int i = 0; i < count; i++) {
			if (comp[i]->bootable () && comp[i]->autoBoot ())
				bootInfo.insertInfo (comp[i]->title (), 1);
			else
				bootInfo.insertInfo (comp[i]->title (), 0);
		}
	}
	*compBootInfo_ = &bootInfo;
}

void
daqRun::setAllMonitorParms (void)
{
	// assume no more than MAX_NUM_DYN_VARS components at a time
	daqComponent* comp[MAX_NUM_DYN_VARS];
	int bufsize = MAX_NUM_DYN_VARS;
	int count = 0;
	daqMonitorStruct monInfo;

	count = system_.allEnabledComponents (comp, bufsize);
	if (count != 0) {
		for (int i = 0; i < count; i++) {
			if (comp[i]->monitored ())
				monInfo.insertInfo (comp[i]->title (), 1);
			else
				monInfo.insertInfo (comp[i]->title (), 0);
		}
	}
	if (monitor_->autoend ())
		monInfo.enableAutoEnd ();
	else
		monInfo.disableAutoEnd ();
	monInfo.timerInterval (monitor_->timerInterval ());

	*monitorParms_ = &monInfo;
}
  
void
daqRun::updateRunMessage (char *newMessage)
{
	*runMsg_ = newMessage;
}
    
void
daqRun::dbaseDecoder (dbaseReader* reader)
{
	dbreader_ = reader;
}

void
daqRun::cleanAll (void)
{
	numtypes_ = 0;
	// reset all variables
	resetAllVariables ();
	// remove all dynamic variables
	removeDynamicVars ();
#if !defined (_CODA_2_0) && !defined (_CODA_2_0_T)
	// remove all ANA log information variables
		removeAnaLogVars ();
#endif
	// last to clean out information which may be used by the previous call
	system_.cleanAll ();
}

int
daqRun::loadDatabase (char* direc, char* session)
{

	cleanAll ();

	if (dbreader_) {
#if defined (_CODA_2_0_T) || defined (_CODA_2_0)
		dbreader_->database (direc);
		if (dbreader_->sessionCreated (session))
			return dbreader_->selectSession (session);
		else
			return dbreader_->createSession (session);
#else
		dbreader_->database (direc);
		if (dbreader_->getComponents () != CODA_SUCCESS)
			return CODA_ERROR;
		if (dbreader_->getAllRunTypes () != CODA_SUCCESS)
			return CODA_ERROR;
		if (dbreader_->getRunNumber () != CODA_SUCCESS)
			return CODA_ERROR;
		// create all ana log information, must be called after all
		// system and components information have been set up
		createAnaLogVars ();
#endif
		return CODA_SUCCESS;
	}
	else
		return CODA_ERROR;
}

int
daqRun::getNetConfigInfo (char* title, char* &config)
{
	return dbreader_->getNetConfigInfo (title, config);
}

int
daqRun::configured (char* title)
{
	return dbreader_->configured (title);
}

void
daqRun::showdata (void)
{
	dataManager_.dumpAll ();
}

int
daqRun::locked (void) const
{
	return locked_;
}

void
daqRun::lock (void)
{
	locked_ = 1;
}

void
daqRun::unlock (void)
{
	locked_ = 0;
}

unsigned short
daqRun::udpPort (void) const
{
	return port_;
}

void
daqRun::udpPort (unsigned short port)
{
	port_ = port;
}

//==========================================================================
//       Network related functions
//==========================================================================
void
daqRun::networkManager (rccAcceptor* m)
{
	netMan_ = m;
}

void
daqRun::cmdFinalResult (int success)
{
	// This routine assumes every call to daqRun processCommand
	// has to insert something into the comdBuffer ahead of time.
	// Invariant: number of commands == number of commands in the buffer
	// --jie chen 6/17/96
#ifndef _CENTERLINE
	assert (locked_);               // must be locked at this moment
	assert (!cmdBuffer.isEmpty ()); // command buffer cannot be empty
#endif
	// get first command from queue
	rcCmdBufItem *item = cmdBuffer.removeCmd ();
	rccIO* chan = item->channel;
	if (chan != 0) { // real network call
		daqNetData res (exptname_,"command",success);
		// send result back to client
		chan->sendResult (item->cmsg->type(), res, item->cmsg->reqId ());
		// free memory of item and message
		delete item->cmsg;
	}
	delete item;
	// unlock the system
	unlock ();

	// check whether there are more commands pending, if yes, process them
	if (!cmdBuffer.isEmpty ()) {
		item = cmdBuffer.removeCmd ();
		int type = item->cmsg->type ();
		rcMsg *msg = item->cmsg;
		rccIO* chan = item->channel;
		delete item;
		processCommand (chan, type, msg);
	}
}

int
daqRun::processCommand (rccIO* chan, int command, rcMsg* cmsg)
{
	// chan and cmsg could null if this routine is called from
	// local instead of from remote sites

	int status = CODA_SUCCESS;
	int sendCallback = 1;
	unsigned long cmd;
	daqNetData *res = 0;

	if (locked_) {
		cmdBuffer.insertCmd (chan, cmsg);
		return 0;
	}
	// lock the command process
	lock ();

	cmd = command;

	switch (cmd) {
	case DALOADDBASE:
		{
			char *sdata[2];
			int   count = 2;
			status = CODA_ERROR;

			daqNetData ndata = (daqNetData)(*cmsg);
			if (ndata.getData (sdata, count) == CODA_SUCCESS)
				status = loadDatabase (sdata[0], sdata[1]);
			res = new daqNetData (exptname_,"command",status);
			// free memory
			delete []sdata[0]; delete []sdata[1];
		}
		break;
	case DASESSION:
		{
#if defined (_CODA_2_0_T) || defined (_CODA_2_0)
			daqNetData ndata = (daqNetData)(*cmsg);
			char *session = new char[::strlen ((char *)ndata) + 1];
			::strcpy (session, (char *)ndata);
			status = selectSession (session);
			res = new daqNetData (exptname_, "command", status);
			delete []session;
#endif
		}
		break;
	case DACREATESES:
		{
#if defined (_CODA_2_0_T) || defined (_CODA_2_0)
			daqNetData ndata = (daqNetData)(*cmsg);
			char *session = new char[::strlen ((char *)ndata) + 1];
			::strcpy (session, (char *)ndata);
			status = createSession (session);
			res = new daqNetData (exptname_, "command", status);
			delete []session;
#endif
		}
		break;
	case DACONFIGURE:
		{
			daqNetData ndata = (daqNetData)(*cmsg);

			// char convertion operator only return a pointer to the buffer
			// so I have to make a copy
			char *runtype = new char[::strlen((char *)ndata) + 1];
			::strcpy (runtype, (char *)ndata);

			if (preConfigure (runtype) != CODA_SUCCESS) {
				delete []runtype;
				res = new daqNetData (exptname_, "command", CODA_ERROR);
			}
			else {
				// configure also behaves like state transition, see note below
				cmdBuffer.insertCmd (chan, cmsg);
				status = configure (runtype);
				return 0;
			}
		}
		break;
	case DACHANGE_STATE:
		{
			daqNetData ndata = (daqNetData)(*cmsg);
			// Important note:
			// state transition will be finished by transitioner
			// command and it's information has to be in the buffer
			cmdBuffer.insertCmd (chan, cmsg);

			int state[2];
			int count = 2;
			ndata.getData (state, count);
#ifdef _CODA_DEBUG
			printf ("initial state is %d final state is %d\n",state[0], state[1]);
#endif
			status = autoTransition (state[0], state[1]);
			return 0;
		}
		break;
	case DADOWNLOAD:
		cmdBuffer.insertCmd (chan, cmsg);
		status = download ();
		return 0;
		break;
	case DAPRESTART:
		cmdBuffer.insertCmd (chan, cmsg);
		status = prestart ();
		return 0;
		break;
	case DAGO:
		cmdBuffer.insertCmd (chan, cmsg);
		status = go ();
		return 0;
		break;
	case DAEND:
		cmdBuffer.insertCmd (chan, cmsg);
		status = end ();
		return 0;
		break;
	case DAPAUSE:
		cmdBuffer.insertCmd (chan, cmsg);
		status = pause ();
		return 0;
		break;
	case DAABORT:
		abort (CODA_CONFIGURED);
		res = new daqNetData(exptname_,"command",status);  
		break;
	case DATERMINATE:
		cmdBuffer.insertCmd (chan, cmsg);
		status = reset ();
		return 0;
		break;
	case DARESET:
		cmdBuffer.insertCmd (chan, cmsg);
		status = reset ();
		return 0;
		break;
	case DATEST:
		{
			for (int i = 0; i < 10; i++)
				sleep (1);
		}
		res = new daqNetData(exptname_,"command",status);
		break;
	case DADISCONNECT:
		// close this socket
		status = CODA_ERROR;
		sendCallback = 0;
		break;
	case DAZAP: 
		// if there is only one client connected to this server, kill the server
		// else do nothing

		if (netMan_->numberClients () == 1) {
			// kill all remote process first
			cmdBuffer.insertCmd (chan, cmsg);
#if defined (_CODA_2_0_T) || defined (_CODA_2_0)
			printf("call terminate from daqRun\n");
			terminate ();
			// remove this server information from the database
			dbreader_->removeRcServerFromDbase ();

			// give up session if holding one
			dbreader_->giveupSession (exptname_); 

			// give up configuration if holding one
			dbreader_->giveupConfiguration ((char *)(*runType_));
#else
			terminate ();
#endif

			// finally exit
			::exit (0);
		}
		sendCallback = 0;
		break;
	default:
		status = CODA_ERROR;
		res = new daqNetData(exptname_,"command",status);
		break;
	}
	if (sendCallback == 1) {
		chan->sendResult (command, *res, cmsg->reqId ());
		unlock ();
		delete cmsg;
		delete res;
	}
	else {
		unlock ();
		delete cmsg;
	}  

	// check if command buffer is empty, if not, processing
	if (!cmdBuffer.isEmpty ()) {
		lock ();
		rcCmdBufItem *item = cmdBuffer.removeCmd ();
		int type = item->cmsg->type ();
		rcMsg* msg = item->cmsg;
		rccIO* chan = item->channel;
		delete item;
		status = processCommand (chan, type, msg);
	} 
	return 0;   // ACE expects  0 for success
}

int
daqRun::getValue (rccIO* chan, rcMsg *cmsg)
{
	daqNetData& ndata = (daqNetData &)(*cmsg); // client request data
	daqData* data = 0; // server side data
	dataManager_.findData (ndata.name(), ndata.attribute(), data);

	// client side has variables having the same names as the server side
	// client will never send over a variable which is not inside data manager
#ifndef _CENTERLINE
	assert (data);
#endif
	// get new value
	if (!data->historyKept ())
		data->update ();

	daqNetData& sdata = (daqNetData &)(*data);
#ifdef _CODA_DEBUG
	printf ("Result at server is %s %s is %s\n",sdata.name(), sdata.attribute(),
		(char *)sdata);
#endif
	chan->sendResult ((int)DAGET_VAL, sdata, cmsg->reqId ());
	delete cmsg;
	return 0;
}
  
int
daqRun::setValue (rccIO* chan, rcMsg* cmsg)
{
	daqNetData& ndata = (daqNetData &)(*cmsg); // client request data
	daqData* data = 0; // server side data
	// client side will never send a data which is not inside data manager
	dataManager_.findData (ndata.name(), ndata.attribute(), data);
#ifndef _CENTERLINE
	assert (data);
#endif

	if (!data->locked ()) { // data is not locked
		// assign new value
		data->lock ();
		*data = ndata;

#ifdef _CODA_DEBUG
		printf ("assignment at server is %s %s is %s\n",data->compname(), 
			data->attrname(),(char *)(*data));
#endif
		// send new value of data back to client
		chan->sendResult ((int)DASET_VAL, (daqNetData &)(*data), cmsg->reqId ());
		delete cmsg;
		data->unlock ();

		// check set callback list and get the first set command
		// and execute this set command
		codaSlist &slist = data->setCbkList ();
		if (!slist.isEmpty ()) {
			rcCmdBufItem* item = (rcCmdBufItem *)slist.firstElement ();
			slist.removeFirst ();
			setValue (item->channel, item->cmsg);
		}
	}
	else { // data is locked
		rcCmdBufItem *newch = new rcCmdBufItem (chan, cmsg);
		data->registerSetCbk ((void *)newch);
	}
	return 0;
}
  
int
daqRun::monitorOnValue (rccIO* chan, rcMsg* cmsg)
{
	daqNetData& ndata = (daqNetData &)(*cmsg); // client request data
	daqData* data = 0; // server side data
	dataManager_.findData (ndata.name(), ndata.attribute(), data);
#ifndef _CENTERLINE
	assert (data);
#endif

	int dynamic = 0;
	int updateI = this->dataUpdateInterval();

	// get latest data value
	// if the data wants to keep its history
	if (!data->historyKept()) 
		data->update ();

	// check whether this data is one of the dynamic variables
	// if yes, check whether it is active or not. If it is not active,
	// turn it into active state
	for (int i = 0; i < numDvars_; i++) {
		// just compare the pointer, data manager and dvars_ share
		// pointers of rcsDaqData
		if (data == dvars_[i]) {
			if (isOnline () ) {
				// if this is online i.e. to get info from remote
				data->enableTrigger ();
				data->activate ();    // processes
			}
			if (strcmp(dvars_[i]->attrname(),"nevents") == 0) {
			   printf("monitor on ************ %s set %s to %d\n", dvars_[i]->compname(), dvars_[i]->attrname(),updateI);
			   codaDaReport(dvars_[i]->compname(),updateI);
			}
			dynamic = 1;
			break;
		}
	}

	// if this data is not a dynamic variable, we need to activate it
	// if it has updater associated with itself
	if (!dynamic && data->updater () && !data->historyKept ())
		data->activate ();
	
	data->enableWrite();

	daqNetData& sdata = (daqNetData &)(*data);
	// register monitor on channel to this data
	rcCmdBufItem *newch = new rcCmdBufItem (chan, cmsg);
	// potential dangerous, data must be rcsData --Nov-14-95
	data->monitorOn ((void *)newch);

	chan->sendResult ((int)DAMONITOR_VAL_ON, sdata, cmsg->reqId ());
	return 0;
}

int
daqRun::monitorOffValue (rccIO* chan, rcMsg* cmsg)
{
	daqNetData& ndata = (daqNetData &)(*cmsg); // client request data
	// cmsg has monitorOff callback id -- nov 29 1995

	daqData* data = 0; // server side data
	dataManager_.findData (ndata.name(), ndata.attribute(), data);
#ifndef _CENTERLINE
	assert (data);
#endif

	daqNetData& sdata = (daqNetData &)(*data);

	// get monitor on callback id which was originally registered by 
	// monitorOnValue --jie chen Nov 29, 1995
	long cbkid = (long)ndata;
	// server side data are all rcsDaqData, so it is safe to do the following
	rcsDaqData *rcsdata = (rcsDaqData *)data;

	if (strcmp(data->attrname(),DYN_ATTR0) == 0) {
		printf("Monitor Off************ %s set %s to %d\n",data->compname(),data->attrname(),5);
		codaDaReport(data->compname(),5);	
	}

    // if this is online i.e. to get info from remote
	data->disableTrigger ();
    data->disableWrite();
	data->deactivate ();    // processes

	data->monitorOff ((void *)chan);

	// cmsg of rcMsg inside monitor callback is freed inside removeMonCallback
	if (rcsdata->removeMonCallback (cbkid) == CODA_SUCCESS) {
		chan->sendResult ((int)DAMONITOR_VAL_OFF, ndata, cmsg->reqId ());
		delete cmsg;
	}
	else {
		ndata = (long)0;
		chan->sendResult ((int)DAMONITOR_VAL_OFF, ndata, cmsg->reqId ());
	}
	return 0;
}

int
daqRun::autoBootInfo (rccIO* chan, rcMsg* cmsg)
{
	daqNetData& ndata = (daqNetData &)(*cmsg); // client request data

	// get real data structure
	daqArbStruct* tmp = (daqArbStruct *)ndata;
	daqCompBootStruct* bootInfo = (daqCompBootStruct *)tmp;

	// get internal information
	long numcomp = 0;
	char** names = 0;
	long* autoboot = 0;
	numcomp = bootInfo->compBootInfo (names, autoboot);

	daqComponent* comp = 0;
	for (int i = 0; i < numcomp; i++) {
		if (system_.has (names[i], comp) == CODA_SUCCESS) {
			if (comp->bootable () && autoboot[i] > 0)
				comp->enableAutoBoot ();
			else
				comp->disableAutoBoot ();
		}
	}

	daqNetData sdata (ndata.name (), ndata.attribute (), CODA_SUCCESS);
	chan->sendResult ((int)DAAUTOBOOT_INFO, sdata, cmsg->reqId ());

	// free memeory
	delete cmsg;
	delete bootInfo;

	// set value to compbootinfo variable
	setAllCompBootInfo ();

	return 0;
}

int
daqRun::monitorParms (rccIO* chan, rcMsg* cmsg)
{
	daqNetData& ndata = (daqNetData &)(*cmsg); // client request data

	// get real data structure
	daqArbStruct* tmp = (daqArbStruct *)ndata;
	daqMonitorStruct* monInfo = (daqMonitorStruct *)tmp;


	// get internal information
	long numcomp = 0;
	char** names = 0;
	long* monitored = 0;
	long  autoend = 0;
	long  interval = 0;
	numcomp = monInfo->monitorParms (names, monitored, autoend, interval);

		daqComponent* comp = 0;
	for (int i = 0; i < numcomp; i++) {
		if (system_.has (names[i], comp) == CODA_SUCCESS) {
			if (monitored[i] > 0)
				comp->monitorOn ();
			else
				comp->monitorOff();
		}
	}
	if (autoend > 0) 
		monitor_->enableAutoEnd ();
	else
		monitor_->disableAutoEnd ();
	monitor_->timerInterval (interval);

	daqNetData sdata (ndata.name (), ndata.attribute (), CODA_SUCCESS);
	chan->sendResult ((int)DAMONITOR_PARM, sdata, cmsg->reqId ());

	// set value to monitor info variable
	*monitorParms_ = monInfo;

	// free memeory
	delete cmsg;
	delete monInfo;

	return 0;
}

int
daqRun::ask_component (rccIO* chan, rcMsg* cmsg,char **result)
{
	daqNetData& ndata = (daqNetData &)(*cmsg); // client request data

	// get real data structure
	char* tmp = (char *)ndata;
	int status;

	status = codaAskComponent (tmp,result);

	return status;
}

int
daqRun::updateClientInfo (rccIO* chan, long cbkid)
{
	// Since there is no non block connect, so there will be
	// no situation that two clients register information at
	// the same time at least for single threaded case
	// So no locking mechanism applied here --

	int numClients = netMan_->numberClients ();
	char **clientInfos = new char*[numClients];

	codaSlistIterator ite (netMan_->clientList () );
	rccIO* ch = 0;
	int i = 0;

	for (ite.init(); !ite; ++ite) {
		ch = (rccIO *)ite ();
		clientInfos[i] = ch->clientInfo ();
		i++;
	}

	// set new value to daqData 
	daqData* data = 0;
	if (dataManager_.findData (exptname_, "clientList", data) == CODA_SUCCESS) {
		if (numClients == 0) {
			char temp[80];
			sprintf (temp, "unknown");
			*data = temp;
		}
		else
			data->assignData (clientInfos, numClients);
	}

	// free memory of clientInfos
	delete []clientInfos;
	if (cbkid != 0) { // need send information back to client
		daqNetData res (exptname_, "command", CODA_SUCCESS);
		chan->sendResult (DAREG_CLIENT_INFO, res, cbkid);
	}
	return 0; // ACE expects 0 for success
}

void
daqRun::updateMasterInfo (char* master)
{
	*master_ = master;
}

void
daqRun::updateControlDispInfo (char* disp)
{
	if (disp)
		*controlDisp_ = disp;
	else {
		// netwirk manager must be non zero
		assert (netMan_);

		codaSlist& clist = netMan_->clientList ();
		codaSlistIterator ite (clist);

		// get first client
		rccIO* client = 0;

		for (ite.init(); !ite; ++ite) {
			client = (rccIO *) ite ();
		}
		if (client == 0) // no more client connection
			*controlDisp_ = "unknown:0.0";
		else
			*controlDisp_ = client->clientXDisplay ();
	}
}


void
daqRun::removeCallbacks (rccIO* chan)
{
	daqDataManagerIterator ite (dataManager_);
	rcsDaqData* data = 0;

	// first get every data variable
	for (ite.init(); !ite; ++ite) {
		data = (rcsDaqData *)ite ();
		data->removeGetCallbacks (chan);
		data->removeSetCallbacks (chan);
		data->removeMonCallbacks (chan);
	}
}

void
daqRun::sendDynamicVarsInfo (rccIO* chan)
{
	if (numDvars_ != 0) {
		char* cs[MAX_NUM_DYN_VARS];

		int i = 0, j = 0;
		while (i < numDvars_) {
			cs[j++] = dvars_[i]->compname();
			// each name has two attributes
			i += NUM_ATTR_DYN;
		}
		daqNetData data(exptname_,"command", cs, j);
		chan->sendResult (DAADD_VARS, data, 0);
	}
}


