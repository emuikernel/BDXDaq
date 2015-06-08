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
//      CODA RunControl Server
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: rcServer.cc,v $
//   Revision 1.20  2000/01/31 20:16:02  rwm
//   If existing rcServer is found, report hostname.
//   Bit o indenting... ;-)
//
//   Revision 1.19  2000/01/11 20:39:42  rwm
//   Added flag to get dalog interface from newer cmlog.h
//
//   Revision 1.18  1999/01/28 17:04:27  rwm
//   include cmlog.h for dalogmsg calls
//
//   Revision 1.17  1999/01/28 14:24:55  heyes
//   add dalogmsg again
//
//   Revision 1.16  1998/11/09 20:40:52  heyes
//   merge with Linux port
//
//   Revision 1.15  1998/11/05 20:11:37  heyes
//   reverse status updating to use UDP, fix other stuff
//
//   Revision 1.14  1998/09/18 15:05:42  heyes
//   no real changes
//
//   Revision 1.13  1998/06/02 19:51:44  heyes
//   fixed rcServer
//
//   Revision 1.12  1997/10/28 13:11:52  heyes
//   fix dplite
//
//   Revision 1.11  1997/10/24 12:58:34  heyes
//   fix dplite bug
//
//   Revision 1.10  1997/10/24 12:23:17  heyes
//   fix dplite bug
//
//   Revision 1.9  1997/10/20 12:45:59  heyes
//   first tag for B
//
//   Revision 1.8  1997/08/26 12:55:59  heyes
//   remove tcl
//
//   Revision 1.7  1997/08/25 15:57:29  heyes
//   use dplite.h
//
//   Revision 1.6  1997/06/13 21:30:42  heyes
//   for marki
//
//   Revision 1.5  1997/05/23 16:45:03  heyes
//   add SESSION env variable, remove coda_activate
//
//   Revision 1.4  1997/01/24 16:36:00  chen
//   change/add Log Component for 1.4
//
//   Revision 1.3  1996/12/04 18:32:40  chen
//   port to 1.4 on hp and ultrix
//
//   Revision 1.2  1996/10/28 14:23:00  heyes
//   communication with ROCs changed
//
//   Revision 1.1.1.1  1996/10/11 13:39:13  chen
//   run control source
//
//

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <rcSvcProtocol.h>
#include <rcServerLocater.h>
#include <rccAcceptor.h>
#include <portHandler.h>

#include <rcMsgReporter.h>
#include <daqActions.h>
#include <daqGlobal.h>

#include <daqSystem.h>
#include <daqSubSystem.h>
#include <netComponent.h>
#include <daqState.h>
#include <daqRun.h>
#include <dbaseReader.h>
#include <rcSignalHandler.h>
#include <rccStdinPipe.h>

#include <rcSvcInfoFinder.h>

#ifdef USE_TK
#include <rcTclInterface.h>
#include <dplite.h>
#endif


#ifdef solaris
extern "C" int gethostname (char*, int);
#endif
extern "C" int get_hostport(char *msqlHost,char *theName,char *theHost,int *thePort,char *result);
sig_atomic_t finished = 0;
Reactor*     glbReactor = 0;
rcMsgReporter *reporter = 0;
static char  expidenv[80];     /* EXPID environment variables */
static char  tcphostenv[80];
static int tcl_interactive = 0;
/* putenv requires static memory location for env argument    */

static void
usage (char* prog)
{
  fprintf (stderr, "Usage: %s [-h(elp)] -d(atabase) dbase -s(ession) session -m(sqld_host) mhost\n", prog);
}

static void
parseOptions (int argc, char** argv, 
	      char* &database, char* &session, char* &msqld)
{
  int i = 1;

  while (i < argc) {
    if (argv[i][0] == '-') {
      if (::strcmp (argv[i], "-h") == 0 ||
	  ::strcmp (argv[i], "-help") == 0) {
	usage (argv[0]);
	exit (0);
      }
      else if (::strcmp (argv[i], "-d") == 0 ||
	       ::strcmp (argv[i], "-database") == 0) {
	i++;
	if (i >= argc || argv[i][0] == '-') {
	  usage (argv[0]) ;
	  ::exit (1);
	}
	database = argv[i];
      }
      else if (::strcmp (argv[i], "-m") == 0 ||
	       ::strcmp (argv[i], "-msqld_host") == 0) {
	i++;
	if (i >= argc || argv[i][0] == '-') {
	  usage (argv[0]) ;
	  ::exit (1);
	}
	msqld = argv[i];
      }
      else if (::strcmp (argv[i], "-s") == 0 ||
	       ::strcmp (argv[i], "-session") == 0) {
	i++;
	if (i >= argc || argv[i][0] == '-') {
	  usage (argv[0]) ;
	  ::exit (1);
	}
	session = argv[i];
      }
      else if (::strcmp (argv[i], "-i") == 0 ||
	       ::strcmp (argv[1], "-interactive") == 0) {
	i++;

	tcl_interactive = 1;

      }
      else {
	usage (argv[0]);
	::exit (1);
      }
    }
    else {
      usage (argv[0]);
      ::exit (1);
    }
    i++;
  }
}

   

main(int argc, char **argv)
{
  char* database = 0;
  char* session = 0;  
  char* msqld = 0;

  /*{
    char host[100],result[100];
    int port;

    get_hostport("alcor","ROC2",host,&port,result);

    printf("host %s port %d result %s\n",host,port,result);
    exit (0);
  }*/
  struct rlimit limits;
  getrlimit(RLIMIT_NOFILE,&limits);
  
  limits.rlim_cur = limits.rlim_max;

  setrlimit(RLIMIT_NOFILE,&limits);

  parseOptions (argc, argv, database, session, msqld);
  
  if (database == 0) {
    database = getenv ("EXPID");
  }

  if (session == 0) {
    session = getenv ("SESSION");
  }

  if (msqld == 0) {
    msqld = getenv ("MYSQL_HOST");
  }

  if (!database || !session) {
    usage (argv[0]);
    ::exit (1);
  }
  ::sprintf (expidenv, "EXPID=%s", database);
  ::putenv (expidenv);


  // First try to find whether there is one server is already running
  char            serverHost[64];
  int             len = sizeof (serverHost);
  unsigned short  serverPort;
  ::gethostname (serverHost, len);

  if (::strcmp (msqld, serverHost) == 0) {
    char* codadb = ::getenv ("CODADB");
  }
  else {  // set MYSQL_HOST env
    char* tcphost = ::getenv ("MYSQL_HOST");
    if (!tcphost) {
      ::sprintf (tcphostenv, "MYSQL_HOST=%s", msqld);
      ::putenv (tcphostenv);      
    }
  }
  
  /* find out whether there is a experiment with the same session
     running somewhere */
  char* host;
  unsigned short udpPort;
  int mst;

  printf("ser3: database >%s<\n",database);fflush(stdout);

  if ((mst = rcSvcInfoFinder::findRcServer (msqld, database, session, 
					    host, udpPort)) == CODA_FATAL) {
    fprintf (stderr, "Communication error with mysql server at %s\n", msqld);
    ::exit (1);

  } else if (mst == CODA_SUCCESS) {

    // there is an entry in the database process table entry for this
    // run control server, but we have to talk to it to determine whether
    // it is really alive or not
    fprintf (stderr, "Warning: RunControl Server: %s may be running already\n",
             session);
    rcServerLocater svcl (DAFINDSERVER, host, session, udpPort, 10);
    unsigned short svcport = 0;

    if (svcl.locatingServer (svcport) == 0) {
      fprintf (stderr, "Error: RunControl Server %s is indeed running on %s.\n",
               session, host);
      delete [] host;
      ::exit (1);

    } else {

      fprintf (stderr, "Info: RunControl Server %s is not running on port %d\n",
               session,udpPort);
      if (rcSvcInfoFinder::removeDeadRcServer (msqld, database, session) 
          != CODA_SUCCESS) {
        fprintf (stderr, "Error: cannot remove dead rcServer from database\n");
      }
    }

    delete [] host;
  }

 
  if (rcSignalHandler::registerSignalHandlers () != CODA_SUCCESS) 
    fprintf (stderr, "Cannot register all signal handler\n");

#ifdef USE_TK
  // initTcl (session);
#endif

  // start new rcServer
  printf("==> start new rcServer: session=>%s< udpPort=%d\n",session,udpPort);
  Reactor          reactor;
  rccAcceptor      rccManager (reactor);
  if (tcl_interactive)
    rccStdinPipe     rccStdin (reactor);

  portHandler brdHandler (reactor, session, udpPort);

  if(brdHandler.open () == -1)
  {
    fprintf(stderr, "Cannot open broadcast Handler\n");
    exit(1);
  }

  if(rccManager.open () == -1)
  {
    fprintf (stderr, "Cannot open client acceptor \n");
    exit(1);
  }
/*#ifdef _CODA_DEBUG*/
  printf ("rcServer client acceptor is at %d port\n",
	  rccManager.port_number ());
/*#endif*/
  brdHandler.acceptorPort(rccManager.port_number());

  if(reactor.register_handler (&rccManager, Event_Handler::READ_MASK) == -1)
  {
    fprintf(stderr, "Cannot register rccManager to reactor\n");
    exit(1);
  }
  if(reactor.register_handler (&brdHandler, Event_Handler::READ_MASK) == -1)
  {
    fprintf(stderr, "Cannot register brdHandler to reactor\n");
    exit(1);
  }
  // setup global reactor pointer
  glbReactor = &reactor;
  // setup global action strings here
  daqActions actions;

#ifdef USE_TK
  // start up hash for DP_cmd
  DP_cmd_init(msqld);
#endif

  daqRun      daqrun (database, session, 0, msqld);
  // update runControl server udp port number to daq run object
  daqrun.udpPort (brdHandler.port_number ());

  // setup message reporter
  rcMsgReporter codaReporter (daqrun);
  // setup global reporter
  reporter = &codaReporter;

  // create database reader
  dbaseReader dbreader (0, daqrun); 

  daqrun.dbaseDecoder (&dbreader);
  daqSystem& system = daqrun.system();

  // pass run to the network manager
  rccManager.setupRun (&daqrun);
  
  brdHandler.register_system(&daqrun);
  printf("INFO: run control up and running !!!\n");

  while(!finished)
  {
    Time_Value tv(10.0);

#ifdef USE_TK
    // tkOneEvent ();
#endif

    reactor.handle_events(tv);
  }

  printf ("Shutting down runcontrol server\n");
  return 0;
}
