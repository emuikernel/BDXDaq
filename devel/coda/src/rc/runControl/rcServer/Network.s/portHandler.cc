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
//      CODA RunControl Service Port Handler
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: portHandler.cc,v $
//   Revision 1.5  1999/07/28 19:21:30  rwm
//   Added comment about checking for status message from Linux and indented code.
//
//   Revision 1.4  1998/11/24 15:00:14  timmer
//   add byte swap to handle_input
//
//   Revision 1.3  1998/11/05 20:11:56  heyes
//   reverse status updating to use UDP, fix other stuff
//
//   Revision 1.2  1996/12/04 18:32:51  chen
//   port to 1.4 on hp and ultrix
//
//   Revision 1.1.1.1  1996/10/11 13:39:14  chen
//   run control source
//
//

#include <stdio.h>
#include <string.h>
#include "portHandler.h"
#include <daqConst.h>
#include <daqState.h>
#include "daqRun.h"
#include "daqSystem.h"
#include "daqComponent.h"

/* sergey: to be able to send messages to runControl log window */
#include <rcMsgReporter.h>

portHandler::portHandler (Reactor& r, char* exptname, int exptid)
:reactor_ (r), daqrun_ (0), brdcastListener_ (), acceptorPort_ (0), exptid_ (exptid)
{
#ifdef _TRACE_OBEJCTS
	printf ("Create portHandler Class Object\n");
#endif
	::strncpy (exptname_, exptname, MAX_STRING_LEN);
}

portHandler::~portHandler (void)
{
	handle_close (-1, Event_Handler::READ_MASK);
}

int
portHandler::open (int async)
{
#if defined (_CODA_2_0_T) || defined (_CODA_2_0)
	INET_Addr    brdAddr ((unsigned short)exptid_);

	if(brdcastListener_.open (brdAddr /*sap_any*/) == -1)
    {
      printf("portHandler::open error 1\n");
	  return(-1);
	}
#else
	int port;

	char *tmp = ::getenv ("CODA_RC_PORT");
	if (tmp)
		sscanf (tmp, "%d", &port);
	else 
		port = RC_PORT + exptid_;

	INET_Addr    brdAddr ((unsigned short)port);

	if (brdcastListener_.open (brdAddr) == -1)
		return -1;
#endif
	else if (async && brdcastListener_.enable (SIGIO) == -1)
	{
      printf("portHandler::open error 2\n");
	  return -1;
	}
	else
	{
      printf("portHandler::open return 0\n");
	  return 0;
	}
}

int
portHandler::register_system(daqRun *daqrun)
{
	daqrun_ = daqrun;
	return 0;
}

#if defined (_CODA_2_0_T) || defined (_CODA_2_0)
unsigned short
portHandler::port_number (void)
{
	INET_Addr addr;

	int status = brdcastListener_.get_local_addr (addr);
	printf("Listening for UDP on port %d\n",ntohs (addr.get_port_number ()));

	return ntohs (addr.get_port_number ());
}
#endif

void
portHandler::acceptorPort (unsigned short portnum)
{
	acceptorPort_ = portnum;
}

int
portHandler::findRcSvcReply (INET_Addr& reqAddr)
{
	int status;
	unsigned short tmp = htons (acceptorPort_);
	status = brdcastListener_.send (&tmp, sizeof (tmp), reqAddr);
	if(status == -1)
    {
	  printf("portHandler::findRcSvcReply: Cannot send back broadcast reply to the client\n");
	}
	if (status > 0)
		return 0;
	else
		return -1;
}

int
portHandler::handle_close (int, Reactor_Mask)
{
	return brdcastListener_.close ();
}

int
portHandler::get_handle (void) const
{
	return brdcastListener_.get_handle ();
}


/* Sergey: main UDP listener !!! */

int
portHandler::handle_input (int)
{
  INET_Addr sa;
  int       n;
  int      type;
  char      exptname[MAX_STRING_LEN];
  char      brdrecvbuf[128];
  daqSystem& system = daqrun_->system();
  daqComponent *comp;
/*
printf("!!! portHandler::handle_input reached\n");
*/
  if ((n = brdcastListener_.recv (brdrecvbuf, sizeof (brdrecvbuf), sa)) == -1)
  {
	//printf("portHandler: empty ...\n");
    return(-1);
  }
  else
  {
/*
printf("portHandler: received: >%s< (0x%02x 0x%02x 0x%02x 0x%02x)\n",
&(brdrecvbuf[0]),brdrecvbuf[0],brdrecvbuf[1],brdrecvbuf[2],brdrecvbuf[3]);fflush(stdout);
*/
    memcpy (&type, brdrecvbuf, sizeof (int));
    int i = sizeof (int);
    daqDataManager& dataManager	= daqrun_->dataManager();

    type = ntohl (type);
#ifdef DEBUG_MSGS
    printf("portHandler: received message: type 0x%08x (%4.4s) content >%s< (>%s< %d)\n",
    type,&type,&(brdrecvbuf[i]),&(brdrecvbuf[0]),i);
#endif
    switch (type)
    {
    case DAFINDSERVER:
      memcpy (exptname, &(brdrecvbuf[i]), MAX_STRING_LEN);
      if(::strcmp (exptname, exptname_) == 0) return(findRcSvcReply(sa));
      break;

      // Look for the string "sta:" or ":ats" for linux and other byte disordered OSes... HACK!
      // this is a status message.
    case 0x7374613a:
    case 0x3a617473:
      char compName[256];
      char status[256];
      int nev, nlong;
      float evrate, nlongrate;

      sscanf(&(brdrecvbuf[i]),"%s %s %d %f %d %f",compName,status,&nev,&evrate,&nlong,&nlongrate);
      if(system.has(compName, comp) == CODA_SUCCESS)
      {
        //printf("comp is %08x \n", comp);
        if(comp)
        {
          int theState;
          //printf("portHandler: found %s nevents is %d state %d\n",comp->title(),daqrun_->eventNumber(),comp->state());
          //printf("portHandler: requested status is %s\n",status);




          /************************************/
		  /* Sergey: get 'status' from DB !!! */
          /************************************
          {
            printf("portHandler: befor >%s< >%s<\n",compName,status);
            printf("portHandler: after >%s< >%s<\n",compName,status);
		  }
          ************************************/
          /************************************/
          /************************************/




          if(::strcmp      (status, "dormant") == 0)     theState = CODA_DORMANT;
          else if(::strcmp (status, "booting") == 0)     theState = CODA_BOOTING;
          else if(::strcmp (status, "booted") == 0)      theState = CODA_BOOTED;
          else if(::strcmp (status, "configuring") == 0) theState = CODA_CONFIGURING;
          else if(::strcmp (status, "configured") == 0)  theState = CODA_CONFIGURED;
          else if(::strcmp (status, "downloading") == 0) theState = CODA_DOWNLOADING;
          else if(::strcmp (status, "downloaded") == 0)  theState = CODA_DOWNLOADED;
          else if(::strcmp (status, "prestarting") == 0) theState = CODA_PRESTARTING;
          else if(::strcmp (status, "paused") == 0)      theState = CODA_PAUSED;
          else if(::strcmp (status, "activating") == 0)  theState = CODA_ACTIVATING;
          else if(::strcmp (status, "active") == 0)      theState = CODA_ACTIVE;
          else if(::strcmp (status, "ending") == 0)      theState = CODA_ENDING;
          else if(::strcmp (status, "prestarted") == 0)  theState = CODA_PRESTARTED;
          else if(::strcmp (status, "resetting") == 0)   theState = CODA_RESETTING;
          else printf("portHandler: ERROR: unknown transaction !!!\n");

          comp->setState(theState);

          daqData* serverData = 0;
          if(dataManager.findData (compName, "nevents", serverData) == CODA_SUCCESS)
          {
            *serverData = nev;
          }

          if(daqrun_->status() == DA_ACTIVE)
          {   
            if(dataManager.findData (compName, "nlongs", serverData) == CODA_SUCCESS)
            {
              *serverData = nlong;
            }
            if(dataManager.findData (compName, "erate", serverData) == CODA_SUCCESS)
            {
              *serverData = evrate;
            }
            if(dataManager.findData (compName, "drate", serverData) == CODA_SUCCESS)
            {
              *serverData = nlongrate;
            }
          }
		  /*
          printf("portHandler: %s state set to %d\n",compName,comp->state());
		  */
        }
      }
      break;



      /*sergey*/
      // Look for the string "ts2:" or ":2st" for linux and other byte disordered OSes... HACK!
      // this is a message from Trigger Supervisor TS2
    case 0x7473323a:
    case 0x3a327374:
      int livetime, tb[12];
	  /*
      printf("received 'ts2:' message >%s<\n",&(brdrecvbuf[i]));
      fflush(stdout);
	  */
      sscanf(&(brdrecvbuf[i]),"%d %d %d %d %d %d %d %d %d %d %d %d %d",&livetime,
        &tb[0],&tb[1],&tb[2],&tb[3],&tb[4],&tb[5],&tb[6],&tb[7],&tb[8],&tb[9],&tb[10],&tb[11]);
	  /*
      printf("scaned\n");
      fflush(stdout);
	  */

	  /*
      printf("portHandler: livetime=%d, trigbits= %d %d %d %d %d %d %d %d %d %d %d %d\n",livetime,
        tb[0],tb[1],tb[2],tb[3],tb[4],tb[5],tb[6],tb[7],tb[8],tb[9],tb[10],tb[11]);
      fflush(stdout);
	  */

          if(daqrun_->status() == DA_ACTIVE)
          {
	  /*
printf("portHandler: sending ..\n");fflush(stdout);
	  */
            daqData* serverData = 0;
            if(dataManager.findData (compName, "livetime", serverData) == CODA_SUCCESS)
            {
              *serverData = livetime;
	  /*
printf("portHandler: sent %d\n",livetime);fflush(stdout);
	  */
            }               
		  }

      break;







      /*sergey*/
      // Look for the string "err:" or ":rre" for linux and other byte disordered OSes... HACK!
      // this is an error message from components
    case 0x6572723a:
    case 0x3a727265:
	  /*
      printf("received 'err:' message >%s<\n",&(brdrecvbuf[i]));
      fflush(stdout);
	  */
      /* ?? */
      reporter->cmsglog (CMSGLOG_ERROR,"%s\n",&(brdrecvbuf[i]));
      break;




      /*sergey*/
      // Look for the string "wrn:" or ":nrw" for linux and other byte disordered OSes... HACK!
      // this is an info message from components
    case 0x77726e3a:
    case 0x3a6e7277:
	  /*
      printf("received 'wrn:' message >%s<\n",&(brdrecvbuf[i]));
      fflush(stdout);
	  */
      /* ?? */
      reporter->cmsglog (CMSGLOG_WARN,"%s\n",&(brdrecvbuf[i]));
      break;




      /*sergey*/
      // Look for the string "inf:" or ":fni" for linux and other byte disordered OSes... HACK!
      // this is an info message from components
    case 0x696e663a:
    case 0x3a666e69:
	  /*
      printf("received 'inf:' message >%s<\n",&(brdrecvbuf[i]));
      fflush(stdout);
	  */
      /* ?? */
      reporter->cmsglog (CMSGLOG_INFO,"%s\n",&(brdrecvbuf[i]));
      break;









    default:
      printf ("portHandler: ERROR: unknown type=0x%08x, msg>%s<\n",
        type,brdrecvbuf);
      fflush(stdout);
      break;
    }
  }
  return(0);
}

    
  

