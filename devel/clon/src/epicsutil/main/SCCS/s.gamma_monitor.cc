h04148
s 00004/00004/00582
d D 1.11 07/10/12 10:20:32 boiarino 12 11
c *** empty log message ***
e
s 00004/00001/00582
d D 1.10 07/10/12 09:58:27 boiarino 11 10
c *** empty log message ***
e
s 00042/00012/00541
d D 1.9 07/03/07 21:14:37 boiarino 10 9
c *** empty log message ***
e
s 00004/00004/00549
d D 1.8 06/11/28 11:04:25 boiarino 9 8
c add 2 new channels, decrease wait intervals
c 
e
s 00002/00002/00551
d D 1.7 05/06/24 21:58:17 wolin 8 7
c Typo
e
s 00005/00003/00548
d D 1.6 05/06/24 21:14:30 wolin 7 6
c Added two new channels
e
s 00002/00002/00549
d D 1.5 05/06/19 22:13:24 fklein 6 5
c add new goni info
e
s 00001/00001/00550
d D 1.4 01/07/17 11:49:38 wolin 5 4
c wait_time 2 secs
e
s 00004/00003/00547
d D 1.3 01/07/17 11:45:33 wolin 4 3
c Seems to work
e
s 00000/00001/00550
d D 1.2 01/07/17 11:21:54 wolin 3 1
c Minor mods
e
s 00000/00000/00000
d R 1.2 01/07/17 11:19:09 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 epicsutil/s/gamma_monitor.cc
e
s 00551/00000/00000
d D 1.1 01/07/17 11:19:08 wolin 1 0
c 
e
u
U
f b 
f e 0
t
T
I 1
// 
//  gamma_monitor
//
//  inserts epics gamma data into data stream periodically
//
//  ejw, 17-jul-2001


// for posix
#define _POSIX_SOURCE_ 1
#define __EXTENSIONS__


// for smartsockets
#include <rtworks/cxxipc.hxx>


// CLAS ipc
#include <clas_ipc_prototypes.h>


// misc
#include <fstream.h>
#include <iomanip.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
D 11
#include <strstream.h>
E 11
I 11

using namespace std;
#include <strstream>

E 11
D 12
#include <macros.h>
E 12

D 12

E 12
// for tcl
extern "C" {
#include <tcl.h>
Tcl_Interp *tclinterp_init(void);
void tclipc_init(Tcl_Interp *interp);
}


// for cdev
#include <cdev.h>
#include <cdevData.h>
#include <cdevDevice.h>
#include <cdevRequestObject.h>
#include <cdevSystem.h>

I 12
#define MIN(a,b)  ( (a) < (b) ? (a) : (b) )
#define MAX(a,b)  ( (a) > (b) ? (a) : (b) )
E 12

// for gamma data
D 10
static const char *gammaname[] = 
{"IPM2C21A","IPM2C21A","IPM2C21A",
 "IPM2C24A","IPM2C24A","IPM2C24A",
 "scaler_dS2b","scaler_dS3b","scaler_dS4b","scaler_dS14b",
D 6
 "coh_edge","gp_photons_x","gp_photons_y"};
E 6
I 6
D 7
 "coh_edge","coh_plane","coh_radiator","gp_photons_x","gp_photons_y"};
E 7
I 7
 "coh_edge","coh_plane","coh_radiator","gp_photons_x","gp_photons_y",
D 8
 "jscaler.GATE0","jscaler.GATE1"};
E 8
I 8
D 9
 "jscaler","jscaler"};
E 9
I 9
 "jscaler","jscaler","HCBMWCALC","HCBMWCALC"};
E 9
E 8
E 7
E 6
static const char *gammaget[] = 
{"get","get XPOS","get YPOS",
 "get","get XPOS","get YPOS",
 "get","get","get","get",
D 6
 "get","get","get"};
E 6
I 6
D 7
 "get","get","get","get","get"};
E 7
I 7
 "get","get","get","get","get",
D 8
 "get","get"};
E 8
I 8
D 9
 "get GATE0","get GATE1"};
E 9
I 9
 "get GATE0","get GATE1","get I","get J"};
E 10
I 10
static const char *gammaname[] = {
  "IPM2C21A",
  "IPM2C21A",
  "IPM2C21A",
  "IPM2C24A",
  "IPM2C24A",
  "IPM2C24A",
  "scaler_dS2b",
  "scaler_dS3b",
  "scaler_dS4b",
  "scaler_dS14b",
  "coh_edge",
  "coh_plane",
  "coh_radiator",
  "gp_photons_x",
  "gp_photons_y",
  "jscaler",
  "jscaler",
  "HCBMWCALC",
  "HCBMWCALC"
};
static const char *gammaget[] = {
  "get",
  "get XPOS",
  "get YPOS",
  "get",
  "get XPOS",
  "get YPOS",
  "get",
  "get",
  "get",
  "get",
  "get",
  "get",
  "get",
  "get",
  "get",
  "get GATE0",
  "get GATE1",
  "get I",
  "get J"
};
E 10
E 9
E 8
E 7
E 6
static cdevRequestObject *gammareq[sizeof(gammaname)/sizeof(char*)];
static float gammaarray[sizeof(gammaname)/sizeof(char*)][9];   // F,8A
static int ngamma = sizeof(gammaname)/sizeof(char*);


// misc variables
static char *application       	 = (char*)"clastest";
char *session          		 = NULL;
char *clon_parms       		 = getenv("CLON_PARMS");
static char *host         	 = getenv("HOST");
static char *unique_id         	 = (char*)"gamma_monitor";
static char dest[132];
static char *epics_gamma_name	 = (char*)"epics/GAMMA_summary.txt" ;
D 4
static double gamma_wait_time    = 0.5;
static double gamma_pend_time    = 0.2;
E 4
I 4
D 5
static double gamma_wait_time    = 0.1;
E 5
I 5
D 9
static double gamma_wait_time    = 2.0;
E 5
static double gamma_pend_time    = 1.9;
E 9
I 9
static double gamma_wait_time    = 1.0/*2.0*/;
static double gamma_pend_time    = 1.0/*1.9*/;
E 9
E 4
static Tcl_Interp *interp;     
static char *init_tcl_script   	 = NULL;
static int nev_to_ipc            = 0;
static int done                	 = 0;
static int no_ipc              	 = 0;
static int no_info             	 = 0;
static int no_file             	 = 0;
static int nepics_read         	 = 0;
static int debug                 = 0;
static int lost_connection       = 0;
static char temp[256];
static char *msql_database    	 = (char*)"clasrun";
D 3
static char buffer[256];
E 3


// file descriptors for services registered with the cdev system object
static int ipcfd;
static cdevUserFdCbkId ipcid;


// other prototypes
void decode_command_line(int argc, char **argv);
void init_tcl(void);
void init_ipc_callbacks(void);
void init_epics(void);
void *gamma_thread(void *param);
I 7
int ipc_mainloop(int opened, int fd, void *arg);
E 7
extern "C" {
void quit_callback(int sig);
void status_poll_callback(T_IPC_MSG msg);
D 7
int ipc_mainloop(int opened, int fd, void *arg);
E 7
int tcl_help(ClientData clientdata, Tcl_Interp *interp, 
		int argc, char **argv);
int tcl_quit(ClientData clientdata, Tcl_Interp *interp, 
		int argc, char **argv);
int get_run_number(const char *msql_database, const char *session);
int insert_msg(const char *name, const char *facility, const char *process, const char *msgclass, 
	       int severity, const char *status, int code, const char *text);
}


// ref to IPC server (connection created later)
TipcSrv &server=TipcSrv::Instance();


// ref to cdev system object
cdevSystem &cdevsys = cdevSystem::defaultSystem ();


//--------------------------------------------------------------------------


main(int argc,char **argv) {


  int status;
  strstream temp2;
  time_t now;
  timespec current  = {0,0};
  timespec lgamma     = {0,0};
  double dels,deln,deltat;


  // synch with stdio
  ios::sync_with_stdio();


  // only print cdev error messages
  cdevsys.setThreshold(CDEV_SEVERITY_ERROR);


  // decode command line...flags may be overridden in Tcl startup script
  decode_command_line(argc,argv);


  // set session name if not specified via env variable or on command line
  if(session==NULL)session=(char*)"clasprod";


  // get event destination
  sprintf(dest,"evt_bosbank/%s",session);


  // get Tcl interp, create tcl commands, link vars, process Tcl startup script, etc.
  interp=tclinterp_init();
  init_tcl();
  if(init_tcl_script!=NULL)Tcl_EvalFile(interp,init_tcl_script);


  // set ipc parameters and connect to ipc system
  if(!TipcInitThreads()) {
    cerr << "Unable to init IPC thread package" << endl;
    exit(EXIT_FAILURE);
  }
  ipc_set_application(application);
  ipc_set_user_status_poll_callback(status_poll_callback);
  ipc_set_quit_callback(quit_callback);
D 4
  status=ipc_init(unique_id,"Epics monitor");
E 4
I 4
  status=ipc_init(unique_id,"Gamma monitor");
E 4
  if(status<0) {
    cerr << "\n?Unable to connect to server...probably duplicate unique id\n"
	 << "   ...check for another gamma_monitor  using ipc_info\n"
	 << "   ...only one connection allowed!" << endl << endl;
    exit(EXIT_FAILURE);
  }


  //  create tcl_request callback
  tclipc_init(interp);


  // set up ipc cdev callbacks
  init_ipc_callbacks();


  // initialize epics callbacks, request objects reinitialized each iteration
  // also read in epics channel list
  init_epics();


  // post startup message
  temp2 << "Process startup:    gamma_monitor starting in " << application << ends;
  status=insert_msg("gamma_monitor","online",unique_id,"status",0,"START",0,temp2.str());
  

  // flush output to log files, etc
  fflush(NULL);


  // main loop
  while (done==0) {

    clock_gettime(CLOCK_REALTIME,&current);
    dels=(double)(current.tv_sec-lgamma.tv_sec);
    deln=(double)(current.tv_nsec-lgamma.tv_nsec)/1.0e9;
    deltat=(deln>0)?(dels+deln):(dels-1.-deln);
    if(deltat>gamma_wait_time) {
      gamma_thread(0);
      clock_gettime(CLOCK_REALTIME,&lgamma);
    }

I 4
    cdevsys.pend(0.05);
E 4
  }


  // post shutdown message
  temp2.seekp(0,ios::beg);
  temp2 << "Process shutdown:  gamma_monitor" << ends;
  status=insert_msg("gamma_monitor","online",unique_id,"status",0,"STOP",0,temp2.str());
  

  // done...clean up
  ipc_close();
  exit(EXIT_SUCCESS);
}
       

//--------------------------------------------------------------------------


void *gamma_thread(void *param) {

  static int first = 0;
  int i;
  float gammaval[sizeof(gammaname)/sizeof(char*)];
  int status[sizeof(gammaname)/sizeof(char*)];
  cdevData result[sizeof(gammaname)/sizeof(char*)];
  cdevGroup group;


  // detach
  if(first==1) {
    for(i=0; i<ngamma; i++)cdevRequestObject::detach(gammareq[i]);
  } else {
    first=1;
  }


  // read gamma channels
  nepics_read++;
  for(i=0; i<ngamma; i++) 
    gammareq[i] = cdevRequestObject::attachPtr((char*)gammaname[i],(char*)gammaget[i]);
  group.start();
  for(i=0; i<ngamma; i++) gammareq[i]->sendNoBlock(NULL,result[i]);
  group.pend(gamma_pend_time);
  group.status(status,ngamma);
  for(int i=0; i<ngamma; i++) {
    if(status[i]==0) {
      gammaval[i] = (float)result[i];
    } else {
      gammaval[i] = -9999.;
    }
  }


  // ship data
  if(no_ipc==0) {
    nev_to_ipc++;
    for(int i=0; i<ngamma; i++) gammaarray[i][0] = gammaval[i];
    TipcMsg msg((T_STR)"evt_bosbank");
    msg.Sender((T_STR)"gamma_monitor");
    msg.Dest(dest);
    msg << (T_STR)"EPIC" << (T_INT4)1 << (T_STR)"(F,8A)" << (T_INT4)9 << (T_INT4)ngamma
	<< (T_INT4)(ngamma*9)
	<< SetSize(ngamma*9) << (T_INT4*)gammaarray;
    server.Send(msg);
    server.Flush();
  }
  
  // dump to file
  if(no_file==0) {
    strstream fn;
    fn << clon_parms<< "/" << epics_gamma_name << ends; 
    ofstream file(fn.str());
    file.setf(ios::left);
    file.setf(ios::showpoint);
    time_t now=time(NULL);
    file << "\n#  epics GAMMA summary created by gamma_monitor on " << ctime(&now) << endl ;
    file << "#     ejw, 14-jul-01"  << endl << endl << endl;
    file << "*epics*" << endl;

    file << endl 
	 << setw(27) << "#Channel name" << setw(20) << "Channel value" << endl;
    file << setw(27) << "#------------" << setw(20) << "-------------" << endl;
    for(int i=0; i<ngamma; i++) {
      if(strlen(gammaget[i])>4) {
	sprintf(temp,"%s.%s",gammaname[i],&(gammaget[i])[4]);
	file << setw(27) << temp << setw(20) << gammaval[i] << endl;
      } else {
	file << setw(27) << gammaname[i] << setw(20) << gammaval[i] << endl;
      }
    }
    file << endl;
    file.close();
  }
  
  
  return((void*)0);
}


//--------------------------------------------------------------------------


void init_ipc_callbacks() {

  ipcfd=server.XtSource();
  cdevsys.addUserFdCallback(ipcfd,ipc_mainloop,(void *)0,ipcid);

  return;
}


//--------------------------------------------------------------------------


void init_epics() {

  char *blanks = (char*)malloc(64);
  char *p1,*p2;


  // fill blanks
  for(int i=0;i<31; i++)blanks[i]=' ';
  blanks[31]='\0';


  // copy gamma channel names
  if(no_ipc==0) {
    int i;
    for(i=0; i<ngamma; i++) strncpy((char *)(&gammaarray[i][1]),blanks,32);
    for(i=0; i<ngamma; i++) {
      if(strlen(gammaget[i])>4) {
	sprintf(temp,"%s.%s",gammaname[i],&(gammaget[i])[4]);
D 12
	strncpy((char *)(&gammaarray[i][1]),temp,min(strlen(temp),31));
E 12
I 12
	strncpy((char *)(&gammaarray[i][1]),temp,MIN(strlen(temp),31));
E 12
      } else {
D 12
	strncpy((char *)(&gammaarray[i][1]),gammaname[i],min(strlen(gammaname[i]),31));
E 12
I 12
	strncpy((char *)(&gammaarray[i][1]),gammaname[i],MIN(strlen(gammaname[i]),31));
E 12
      }
    }
  }

  return;
}


//--------------------------------------------------------------------------


void status_poll_callback(T_IPC_MSG msg) {

  
  TipcMsgAppendStr(msg,(char*)"number of epics reads");
  TipcMsgAppendInt4(msg,nepics_read);
  
  TipcMsgAppendStr(msg,(char*)"nevent sent to ipc");
  TipcMsgAppendInt4(msg,nev_to_ipc);
  
  TipcMsgAppendStr(msg,(char*)"destination");
  TipcMsgAppendStr(msg,dest);
  
  TipcMsgAppendStr(msg,(char*)"no_ipc");
  TipcMsgAppendInt4(msg,no_ipc);

  TipcMsgAppendStr(msg,(char*)"no_info");
  TipcMsgAppendInt4(msg,no_info);

  TipcMsgAppendStr(msg,(char*)"no_file");
  TipcMsgAppendInt4(msg,no_file);

  return;
}


//-------------------------------------------------------------------


void quit_callback(int sig) {

  done=1;

  return;
}


//-------------------------------------------------------------------


// process existing messages in Smartsockets receipt queue
int ipc_mainloop(int opened, int fd, void *arg) {
  
  if(opened==0) {
    cerr << "Cdevsys unable to attach to ipc fd: " << fd << endl;
    return(-1);

  } else {
    server.MainLoop(0.0);
    return (0);
  }
}


//-------------------------------------------------------------------


void init_tcl() {

  // link c and Tcl variables
  Tcl_LinkVar(interp,(char*)"application",    	   (char *)&application,      	TCL_LINK_STRING);
  Tcl_LinkVar(interp,(char*)"unique_id",     	   (char *)&unique_id,          TCL_LINK_STRING);
  Tcl_LinkVar(interp,(char*)"session",     	   (char *)&session,            TCL_LINK_STRING);
  //  Tcl_LinkVar(interp,(char*)"dest",     	   (char *)&dest,               TCL_LINK_STRING|TCL_LINK_READ_ONLY);
  Tcl_LinkVar(interp,(char*)"gamma_wait_time",     (char *)&gamma_wait_time,    TCL_LINK_DOUBLE);
  Tcl_LinkVar(interp,(char*)"gamma_pend_time",     (char *)&gamma_pend_time,    TCL_LINK_DOUBLE);
  Tcl_LinkVar(interp,(char*)"no_ipc",     	   (char *)&no_ipc,             TCL_LINK_INT);
  Tcl_LinkVar(interp,(char*)"no_info",     	   (char *)&no_info,            TCL_LINK_INT);
  Tcl_LinkVar(interp,(char*)"no_file",     	   (char *)&no_file,            TCL_LINK_INT);
  Tcl_LinkVar(interp,(char*)"nepics_read",    	   (char *)&nepics_read,        TCL_LINK_INT|TCL_LINK_READ_ONLY);
  Tcl_LinkVar(interp,(char*)"nev_to_ipc",     	   (char *)&nev_to_ipc,         TCL_LINK_INT|TCL_LINK_READ_ONLY);
  Tcl_LinkVar(interp,(char*)"debug",     	   (char *)&debug,              TCL_LINK_BOOLEAN);


  // create Tcl commands
  Tcl_CreateCommand(interp,(char*)"help",tcl_help,
  		    (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
  Tcl_CreateCommand(interp,(char*)"quit",tcl_quit,
		    (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
  Tcl_CreateCommand(interp,(char*)"stop",tcl_quit,
		    (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
  Tcl_CreateCommand(interp,(char*)"exit",tcl_quit,
		    (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
  return;

}


//--------------------------------------------------------------------------


void decode_command_line(int argc, char**argv) {

  const char *help = "\nusage:\n\n  gamma_monitor [-a application] [-s session] [-u unique_id]\n"
    "        [-t init_tcl_script] [-m msql_database] [-wait gamma_wait_time] [-pend gamma_pend_time]\n"
    "        [-no_ipc] [-no_info] [-no_file] [-debug]\n";


  // loop over all arguments, except the 1st (which is program name)
  int i=1;
  while(i<argc) {
    if(strncasecmp(argv[i],"-h",2)==0) {
      cout << help << endl;
      exit(EXIT_SUCCESS);
    }
    else if (strncasecmp(argv[i],"-no_ipc",7)==0) {
      no_ipc=1;
      i=i+1;
    }
    else if (strncasecmp(argv[i],"-no_info",8)==0) {
      no_info=1;
      i=i+1;
    }
    else if (strncasecmp(argv[i],"-no_file",8)==0) {
      no_file=1;
      i=i+1;
    }
    else if (strncasecmp(argv[i],"-debug",6)==0) {
      debug=1;
      i=i+1;
    }
    else if (strncasecmp(argv[i],"-pend",5)==0) {
      gamma_pend_time= atof(argv[i+1]);
      i=i+2;
    }
    else if (strncasecmp(argv[i],"-wait",5)==0) {
      gamma_wait_time= atof(argv[i+1]);
      i=i+2;
    }
    else if (strncasecmp(argv[i],"-a",2)==0) {
      application=strdup(argv[i+1]);
      i=i+2;
    }
    else if (strncasecmp(argv[i],"-u",2)==0) {
      unique_id=strdup(argv[i+1]);
      i=i+2;
    }
    else if (strncasecmp(argv[i],"-s",2)==0) {
      session=strdup(argv[i+1]);
      i=i+2;
    }
    else if (strncasecmp(argv[i],"-t",2)==0) {
      init_tcl_script=strdup(argv[i+1]);
      i=i+2;
    }
    else if (strncasecmp(argv[i],"-m",2)==0) {
      msql_database=strdup(argv[i+1]);
      i=i+2;
    }
    else if (strncasecmp(argv[i],"-",1)==0) {
      cout << "Unknown command line arg: " << argv[i] << argv[i+1] << endl << endl;
      i=i+1;
    }
  }

  return;
}

  
//----------------------------------------------------------------


int tcl_help(ClientData clientdata, Tcl_Interp *interp,
		int argc, char **argv) {

    const char *help =
    "\nTcl commands available in the gamma_monitor program:\n\n"
    " help                  print this message\n"
    " stop                  stop program\n"
    " quit                  stop program\n"
    " exit                  stop program\n"
    "\n\n Type command that require args with NO args for more information\n"
    "\n";

    Tcl_SetResult(interp,(char*)help,TCL_STATIC);

  return (TCL_OK);

}


//---------------------------------------------------------------------


int tcl_quit(ClientData clientdata, Tcl_Interp *interp,
	     int argc, char **argv) {
  
  done=1;
  
  return (TCL_OK);
}


//---------------------------------------------------------------------
E 1