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
//      Data Acquisition System Class Implementation
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: daqSystem.cc,v $
//   Revision 1.7  1999/07/28 19:17:39  rwm
//   Bunch of hacks to support Pause script button for Hall B.
//
//   Revision 1.6  1999/02/25 14:37:02  rwm
//   Limits defined in daqRunLimits.h
//
//   Revision 1.5  1998/11/09 20:40:54  heyes
//   merge with Linux port
//
//   Revision 1.4  1998/11/05 20:11:45  heyes
//   reverse status updating to use UDP, fix other stuff
//
//   Revision 1.2  1997/06/17 19:29:14  heyes
//   for larry
//
//   Revision 1.1.1.1  1996/10/11 13:39:18  chen
//   run control source
//
//
#include <daqComponent.h>
#include <daqScriptComp.h>
#include <daqSubSystem.h>
#include <activater.h>
#include <booter.h>
#include <configurer.h>
#include <downloader.h>
#include <ender.h>
#include <pauser.h>
#include <prestarter.h>
#include <resetter.h>
#include <terminater.h>
#include <verifier.h>
#include <compTransitioner.h>
#include <transitionGraph.h>
#include <daqRun.h>

#include "daqSystem.h"

const int COMP_TABLE_SIZE = 113;

daqSystem::daqSystem (void)
:daqTarget ("codaDaqSystem"), components_(COMP_TABLE_SIZE, codaStrHashFunc),
 subsystems_ (), subsysLocked_ (0), activeBeforePaused_ (0), run_ (0)
{
#ifdef _TRACE_OBJECTS
	printf ("    Create daqSystem Class Object\n");
#endif
	// initialize all those transitioners
	activater_ = new activater (this);
	booter_ = new booter (this);
	configurer_ = new configurer (this);
	downloader_ = new downloader (this);
	ender_ = new ender (this);
	pauser_ = new pauser (this);
	prestarter_ = new prestarter (this);
	resetter_ = new resetter (this);
	terminater_ = new terminater (this);
	verifier_ = new verifier (this);
	// initialize current transitioner to 0
	currTransitioner_ = 0;
	// create state transition graph
	stateGraph_ = new transitionGraph (this);
}

daqSystem::~daqSystem (void)
{
#ifdef _TRACE_OBJECTS
	printf ("    Delete daqSystem Class Object\n");
#endif
	// remove all subsystems from the list
	subsysLocked_ = 1;
	{
		codaSlistIterator ite (subsystems_);
		daqSubSystem* subsys = 0;

		for (ite.init (); !ite; ++ite) {
			subsys = (daqSubSystem *)ite ();
			delete subsys;
		}
	}
	subsysLocked_ = 0;
	// remove state transition graph
	delete stateGraph_;
	// subsystems will delete all their components 
	// delete all transitioners
	delete activater_;
	delete booter_;
	delete configurer_;
	delete downloader_;
	delete ender_;
	delete pauser_;
	delete prestarter_;
	delete resetter_;
	delete terminater_;
	delete verifier_;

	currTransitioner_ = 0;
}

void
daqSystem::abort (int wanted)
{
	codaSlistIterator ite (subsystems_);
	daqSubSystem* subsys = 0;

	for (ite.init (); !ite; ++ite) {
		subsys = (daqSubSystem *)ite ();
		subsys->abort (wanted);
	}
	setState (wanted);
}

int
daqSystem::addComponent (daqComponent* comp)
{
	daqComponent* tcomp = 0;
	if (has (comp->title (), tcomp) != CODA_SUCCESS) {
		components_.add (comp->title(), (void *)comp);
		return CODA_SUCCESS;
	}
	return CODA_ERROR;
}

int
daqSystem::removeComponent (daqComponent* comp)
{
	daqComponent* tcomp = 0;
	if (!has (comp->title (), tcomp) == CODA_SUCCESS){
		components_.remove (comp->title(), (void *)comp);
		return CODA_SUCCESS;
	}
	return CODA_ERROR;
}

int
daqSystem::removeComponent (char *title)
{
	codaSlist& list = components_.bucketRef (title);
	codaSlistIterator ite (list);
	daqComponent* comp = 0;

	int found = 0;
	for (ite.init (); !ite; ++ite) {
		comp = (daqComponent *)ite ();
		if (::strcmp (comp->title (), title) == 0) {
			ite.removeCurrent ();
			return CODA_SUCCESS;
		}
	}
	return CODA_ERROR;
}

int
daqSystem::allComponents (daqComponent* cs[], int bufsize)
{
	int count = 0;
	codaStrHashIterator ite (components_);
	daqComponent *comp = 0;

	// First time is for the ER

	for (ite.init (); !ite; ++ite) {
		comp = (daqComponent *)ite ();
		daqScriptComp *scomp = (daqScriptComp *)comp;
		daqSubSystem& subsys = scomp->subSystem ();

		if ((strstr (comp->title (), CODA_USER_SCRIPT) == 0) &&
		 (strcmp (subsys.title (), "ER") == 0)) {
			cs[count++] = comp;
		}
		if (count >= bufsize)
			return -1;
	}
	// Second is for EB
	for (ite.init (); !ite; ++ite) {
		comp = (daqComponent *)ite ();
		daqScriptComp *scomp = (daqScriptComp *)comp;
		daqSubSystem& subsys = scomp->subSystem ();
	   if ((strstr (comp->title (), CODA_USER_SCRIPT) == 0) && 
	   (strcmp (subsys.title (), "EB") == 0)) {
			cs[count++] = comp;
		}
		if (count >= bufsize)
			return -1;
	}
	// Third is for everything else
	for (ite.init (); !ite; ++ite) {
		comp = (daqComponent *)ite ();
		daqScriptComp *scomp = (daqScriptComp *)comp;
		daqSubSystem& subsys = scomp->subSystem ();
		if ((strstr (comp->title (), CODA_USER_SCRIPT) == 0)	&& 
			(strcmp (subsys.title (), "ER") != 0) &&
			(strcmp (subsys.title (), "EB") != 0))
			cs[count++] = comp;
		if (count >= bufsize)
			return -1;
	}

	return count;
}

int
daqSystem::allComponents (char* cs[], int bufsize)
{
	int count = 0;
	int ix, counttemp = 0;
	daqComponent *comp = 0;
	// bug bug, only MAX_NUM_COMPONENTS components
	daqComponent* cstemp[MAX_NUM_COMPONENTS];	 

	counttemp = allComponents ( cstemp, MAX_NUM_COMPONENTS );

	for (ix=0; ix<counttemp; ix++) {
		comp = cstemp[ix];
		cs[count] = new char[::strlen (comp->title()) + 1];
		::strcpy (cs[count++], comp->title());

		if (count >= bufsize)
			return -1;
	}
	return count;
}

int
daqSystem::allEnabledComponents (daqComponent* cs[], int bufsize)
{
	int count = 0;
	int ix, counttemp = 0;
	daqComponent *comp = 0;
	daqComponent* cstemp[MAX_NUM_COMPONENTS];	 // bug bug, only MAX_NUM_COMPONENTS components

	counttemp = allComponents ( cstemp, MAX_NUM_COMPONENTS );

	for (ix=0; ix<counttemp; ix++) {
		comp = cstemp[ix];
		if (comp->enabled ())
			cs[count++] = comp;

		if (count >= bufsize)
			return -1;
	}
	return count;
}

int
daqSystem::allEnabledComponents (char* cs[], int bufsize)
{
	int count = 0;
	int ix, counttemp = 0;
	daqComponent *comp = 0;
	daqComponent* cstemp[MAX_NUM_COMPONENTS];	 // bug bug, only MAX_NUM_COMPONENTS components

	counttemp = allComponents ( cstemp, MAX_NUM_COMPONENTS );

	for (ix=0; ix<counttemp; ix++) {
		comp = cstemp[ix];
		if (comp->enabled ()) {
			cs[count] = new char[::strlen (comp->title()) + 1];
			::strcpy (cs[count++], comp->title());

		}
		if (count >= bufsize)
			return -1;
	}
	return count;
}

void
daqSystem::disableAllComponents (void)
{
	codaStrHashIterator ite (components_);
	daqComponent* comp = 0;

	for (ite.init (); !ite; ++ite) {
		comp = (daqComponent *)ite();
		comp->disable ();
	}
}

void
daqSystem::enableAllComponents (void)
{
	codaStrHashIterator ite (components_);
	daqComponent* comp = 0;

	for (ite.init (); !ite; ++ite) {
		comp = (daqComponent *)ite();
		comp->enable ();
	}
}

void
daqSystem::removeAllScriptComp (void)
{
	codaStrHashIterator ite (components_);
	daqComponent* comp = 0;

	for (ite.init (); !ite; ++ite) {
		comp = (daqComponent *)ite ();
		if (strstr (comp->title (), CODA_USER_SCRIPT) != 0) {
			// now this is a daqScriptComp			 
			daqScriptComp *scomp = (daqScriptComp *)comp;
			daqSubSystem& subsys = scomp->subSystem ();
			subsys.removeComponent (comp);
			// remove this from system hash table
			ite.removeCurrent ();
			delete comp;
		}
	}

}

int
daqSystem::addSubSystem (daqSubSystem* subsys)
{
	if (subsystems_.includes (subsys->title ()))
		return CODA_WARNING;
	subsystems_.add ((void *)subsys);
	return CODA_SUCCESS;
}

int
daqSystem::removeSubSystem (daqSubSystem* subsys)
{
	if (subsystems_.remove ((void *)subsys))
		return CODA_SUCCESS;
	else
		return CODA_ERROR;
}

int
daqSystem::has (daqComponent *comp)
{
	if (components_.find (comp->title (), (void *)comp))
		return CODA_SUCCESS;
	return CODA_ERROR;
}

int
daqSystem::has (char* title, daqComponent* &comp)
{
	codaSlist& list = components_.bucketRef (title);
	codaSlistIterator ite (list);
	daqComponent* tcomp = 0;

	for (ite.init (); !ite; ++ite) {
		tcomp = (daqComponent *)ite ();
		if (::strcmp (title, tcomp->title ()) == 0) {
			comp = tcomp;
			return CODA_SUCCESS;
		}
	}
	comp = 0;
	return CODA_ERROR;
}

int
daqSystem::locateSystem (char *className, daqSubSystem* &sys)
{
	// subsystem's className == subsystem's title
	codaSlistIterator ite (subsystems_);
	daqSubSystem* subsys = 0;

	for (ite.init (); !ite; ++ite) {
		subsys = (daqSubSystem *)ite ();
		if (::strcmp (className, subsys->title()) == 0) {
			sys = subsys;
			return CODA_SUCCESS;
		}
	}
	sys = 0;
	return CODA_ERROR;
}

void
daqSystem::cleanAll (void)
{
	components_.deleteAllValues ();

	// remove all subsystems from the list
	subsysLocked_ = 1;
	{
		codaSlistIterator ite (subsystems_);
		daqSubSystem* subsys = 0;

		for (ite.init (); !ite; ++ite) {
			subsys = (daqSubSystem *)ite ();
			delete subsys;
		}
	}
	subsysLocked_ = 0;
	subsystems_.deleteAllValues ();
	// no need to delete all these transitioners which are only related
	// to subsystems_
}  

int
daqSystem::autostart (void)
{
	compTransitioner autoTransitioner;

	autoTransitioner.pendTransitioner (prestarter_);
	autoTransitioner.pendTransitioner (activater_);  
	autoTransitioner.execute ();
	return CODA_SUCCESS;
}

int
daqSystem::boot (void)
{
	booter_->execute ();
	return CODA_SUCCESS;
}

int
daqSystem::terminate (void)
{
	terminater_->execute ();
	return CODA_SUCCESS;
}

int
daqSystem::configure (void)
{
	codaSlistIterator ite (subsystems_);
	daqSubSystem* subsys = 0;

	for (ite.init(); !ite; ++ite) {
		subsys = (daqSubSystem *) ite ();
		// check configuration information to do disable/enable a sub system
		subsys->enableRun ();
	}
	configurer_->execute ();
	return CODA_SUCCESS;
}

int
daqSystem::download (void)
{
	autoTransition (DA_DORMANT, DA_DOWNLOADED);
	return CODA_SUCCESS;
}

int
daqSystem::go (void)
{
	activater_->execute ();
	return CODA_SUCCESS;
}

int
daqSystem::pause (void)
{
  pauser_->execute ();
  return CODA_SUCCESS;  
}

int
daqSystem::resume (void)
{
  // RWM bugbug hack to get rcServer to just call a script to resume.
  // resumer_->execute ();
  int status = 0;
  printf("Calling rcResume script.\n");
  //  status = system("");
  return CODA_SUCCESS;  
}

int
daqSystem::prestart (void)
{
	prestarter_->execute ();
	return CODA_SUCCESS;
}

int
daqSystem::end (void)
{
	ender_->execute ();
	return CODA_SUCCESS;
}

int
daqSystem::verify (void)
{
	verifier_->execute ();
	return CODA_SUCCESS;
}

int
daqSystem::reset (void)
{
	resetter_->execute ();
	return CODA_SUCCESS;  
}

int
daqSystem::autoTransition (int istate, int fstate)
{
	int status = CODA_SUCCESS;

	if (istate == fstate)
		status = CODA_ERROR;
	if (istate < CODA_LOW_STATE || istate > CODA_HIGH_STATE)
		status = CODA_ERROR;
	if (fstate < CODA_LOW_STATE || fstate > CODA_HIGH_STATE)
		status = CODA_ERROR;

	if (status == CODA_SUCCESS) {
		graphNode *source;
		graphNode *dest;
		if (stateGraph_->DFS (istate, fstate, source, dest) == CODA_SUCCESS) {
			compTransitioner tran;
			graphNode *node = dest;
			while (node != source) {
				transitioner *tr = (transitioner *)node->linkToParent ();
				// add one transitioner to the beginning of the list
				tran.addTransitioner (tr);
				node = node->parent ();
			}
			tran.execute ();
		}
		else {
			status = CODA_ERROR;
		}
	}
	if (status != CODA_SUCCESS) // send command result back to daq run
		run_->cmdFinalResult (status);
	return status;
}

int
daqSystem::cancelTransition (void)
{
	if (currTransitioner_) {
		currTransitioner_->cancel (CODA_IGNORED);
		return CODA_SUCCESS;
	}
	return CODA_ERROR;
}

void
daqSystem::currTransitioner (transitioner* tran)
{
	currTransitioner_ = tran;
}

transitioner*
daqSystem::currTransitioner (void) const
{
	return currTransitioner_;
}

void
daqSystem::setState (int newst)
{
  daqTarget::setState(newst);
  if(prevState_ == CODA_ACTIVE && newst == CODA_PAUSED)
    activeBeforePaused_ = 1;
  else
    activeBeforePaused_ = 0;    
  run_->status(newst);
}
  
void
daqSystem::setOverrideState (int wanted)
{
  codaSlistIterator ite (subsystems_);
  daqSubSystem* subsys = 0;

  for(ite.init(); !ite; ++ite)
  {
    subsys = (daqSubSystem *) ite ();
    subsys->setOverrideState (wanted);
  }
  overrideState_ = wanted;
}

int
daqSystem::status (void) const
{
  return status_;
}

int
daqSystem::checkState (void)
{
	status_ = CODA_SUCCESS;
	int targetState = 0;
	int someItems = 0;
	int inconsistent = 0;
	int res;

	codaSlistIterator ite (subsystems_);
	daqSubSystem* subsys = 0;

	for (ite.init(); !ite; ++ite) {
		subsys = (daqSubSystem *)ite ();
		if (subsys->enabled () ) {
			if (!someItems) {
				targetState = subsys->state ();
				someItems = 1;
			}
			else {
				if (subsys->state () != targetState)
					inconsistent = 1;
			}
		}
	}
	if (!someItems)
		res = state_;
	else if (!inconsistent) {
		state_ = targetState;
		res = targetState;
	}
	else {
		status_ = CODA_ERROR;
		res = state_;
	}
	// update daqRun status
	run_->status (res);
	return res;
}

daqRun*
daqSystem::run (void) 
{
	return run_;
}

void
daqSystem::run (daqRun* r)
{
	run_ = r;
}

int
daqSystem::pauseActiveLoop (void) const
{
	return activeBeforePaused_;
}



