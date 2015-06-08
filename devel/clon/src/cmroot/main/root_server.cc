//
// root_server.cc - CLAS Monitoring ROOT Server: takes histograms from
//                  the ET system and produces ROOT file; separate files
//                  created for different runs
//
// first release: Sergey Boyarinov 16-nov-2007
//

// for smartsockets
#include <rtworks/cxxipc.hxx>

// CLAS ipc
#include <clas_ipc_prototypes.h>

// misc
using namespace std;
#include <strstream>

#include <pthread.h>
#include <fstream.h>
#include <iomanip.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// ROOT includes
#include "TFile.h"
#include "TH1.h"
#include "TH2.h"
#include "TProfile.h"
#include "TNtuple.h"
#include "TRandom.h"
#include "TUnixSystem.h"

// cmroot includes
#include "CMDaq.h"
#include "CMRootServer.h"

// misc variables
static char *application     = (char*)"clastest";
char *session          		 = NULL;
char *clon_parms       		 = getenv("CLON_PARMS");
static char *host         	 = getenv("HOST");
static char *unique_id       = (char*)"root_server";

// globals
static int ipc_pend_time         = 1;
static int done                	 = 0;
static int debug                 = 0;

// prototypes
void decode_command_line(int argc, char **argv);
extern "C" {
void *ipc_thread(void *param);
void quit_callback(int sig);
void status_poll_callback(T_IPC_MSG msg);
//int get_run_number(const char *msql_database, const char *session);
int insert_msg(const char *name, const char *facility, const char *process, const char *msgclass, 
	       int severity, const char *status, int code, const char *text);
}

// ref to IPC server (connection created later)
TipcSrv &server=TipcSrv::Instance();


//_____________________________________________________________________________
int
main(int argc,char **argv)
{
  Int_t status, ret;
  pthread_t t1;
  strstream temp2;


  /* smartsockets stuff */
  // synch with stdio
  ios::sync_with_stdio();
  // decode command line...
  decode_command_line(argc,argv);
  // set session name if not specified via env variable or on command line
  if(session==NULL) session=(char*)"clasprod";
  // set ipc parameters and connect to ipc system
  if(!TipcInitThreads())
  {
    cerr << "Unable to init IPC thread package" << endl;
    exit(EXIT_FAILURE);
  }
  ipc_set_application(application);
  ipc_set_user_status_poll_callback(status_poll_callback);
  ipc_set_quit_callback(quit_callback);
  status=ipc_init(unique_id,"ROOT server");
  if(status<0)
  {
    cerr << "\n?Unable to connect to server...probably duplicate unique id\n"
	 << "   ...check for another epics_monitor  using ipc_info\n"
	 << "   ...only one connection allowed!" << endl << endl;
    exit(EXIT_FAILURE);
  }
  // launch thread(s)
#ifdef SunOS
  thr_setconcurrency(thr_getconcurrency()+4);
#endif
  pthread_create(&t1,NULL,ipc_thread,(void*)NULL);
  // post startup message
  temp2 << "Process startup: root_server starting in " << application << ends;
  status=insert_msg("root_server","online",unique_id,"status",0,"START",0,temp2.str());
  // flush output to log files, etc
  fflush(NULL);
  /* smartsockets stuff */





  // argument can be ET system name or BOS file name
  
  CMDaq *daq = new CMDaq("/tmp/et_sys_clasprod","ROOTSRV");
  
  CMRootServer *rootserver = new CMRootServer("./scripts/hist_server.conf", NULL);

  printf("1\n");
  daq->Download();
  printf("2\n");
  daq->Prestart();
  printf("3\n");
  status = daq->Start();
  printf("4\n");


  while(done==0)
  {
    for(Int_t i=0; i<1000; i++)
    {
      daq->Loop();
    }

    // update file
    rootserver->Update();
  }

  printf("5\n");
  daq->Stop();
  printf("6\n");


  /* smartsockets */
  // post shutdown message
  temp2.seekp(0,ios::beg);
  temp2 << "Process shutdown: root_server" << ends;
  status=insert_msg("root_server","online",unique_id,"status",0,"STOP",0,temp2.str());
  // done...clean up
  ipc_close();
  /* smartsockets */

  exit(0);
}

/***************************************************/
/**************** UTILITY functions ****************/
/***************************************************/

//--------------------------------------------------------------------------
void
status_poll_callback(T_IPC_MSG msg)
{  
  /*
  TipcMsgAppendStr(msg,(char*)"number of epics reads");
  TipcMsgAppendInt4(msg,nepics_read);
  
  TipcMsgAppendStr(msg,(char*)"nevent sent to ipc");
  TipcMsgAppendInt4(msg,nev_to_ipc);
  
  TipcMsgAppendStr(msg,(char*)"destination");
  TipcMsgAppendStr(msg,dest);
  
  TipcMsgAppendStr(msg,(char*)"channel_wait_time");
  TipcMsgAppendInt4(msg,channel_wait_time);

  TipcMsgAppendStr(msg,(char*)"scaler_wait_time");
  TipcMsgAppendInt4(msg,scaler_wait_time);

  TipcMsgAppendStr(msg,(char*)"gamma_wait_time");
  TipcMsgAppendReal4(msg,gamma_wait_time);

  TipcMsgAppendStr(msg,(char*)"ipc_pend_time");
  TipcMsgAppendInt4(msg,ipc_pend_time);

  TipcMsgAppendStr(msg,(char*)"no_ipc");
  TipcMsgAppendInt4(msg,no_ipc);

  TipcMsgAppendStr(msg,(char*)"no_info");
  TipcMsgAppendInt4(msg,no_info);

  TipcMsgAppendStr(msg,(char*)"no_file");
  TipcMsgAppendInt4(msg,no_file);
  */

  TipcMsgAppendStr(msg,(char*)"");
  TipcMsgAppendStr(msg,(char*)"");

  return;
}

//-------------------------------------------------------------------
void
quit_callback(int sig)
{
  done=1;

  return;
}

//--------------------------------------------------------------------------


//  receives online info from other programs and dispatches to appropriate callbacks
void *
ipc_thread(void *param)
{
  while(done==0)
  {
    /*printf("ipc_thread is running ..\n");*/
    server.MainLoop((double)ipc_pend_time);
  }
  printf("ipc_thread exited\n");

  return((void*)NULL);
}


//-------------------------------------------------------------------

void
decode_command_line(int argc, char**argv)
{
  const char *help = "\nusage:\n\n  root_server [-a application] [-s session] [-debug]\n";

  // loop over all arguments, except the 1st (which is program name)
  int i=1;
  while(i<argc)
  {
    if(strncasecmp(argv[i],"-h",2)==0)
    {
      cout << help << endl;
      exit(EXIT_SUCCESS);
    }
    else if (strncasecmp(argv[i],"-a",2)==0) {
      application=strdup(argv[i+1]);
      i=i+2;
    }
    else if (strncasecmp(argv[i],"-s",2)==0) {
      session=strdup(argv[i+1]);
      i=i+2;
    }
    else if (strncasecmp(argv[i],"-debug",6)==0)
    {
      debug=1;
      i=i+1;
    }
    else if (strncasecmp(argv[i],"-",1)==0)
    {
      cout << "Unknown command line arg: " << argv[i] << argv[i+1] << endl << endl;
      i=i+1;
    }
  }

  return;
}

