// 
//  epics_monitor.cc
//
//  inserts epics data into data stream periodically
//  broadcasts data to info_server
//  automatically reconnects if cdev channel drops out
//
//  ejw, 12-oct-2000
//
/* Sergey: on October 16, 2007 original cdev-based Elliott Wolin's code was changed
 to CA-based; catools/camonitor.c was used as an prototype, some parts of it were
 copied and modified */

#include <stdio.h>
#include <epicsStdlib.h>
#include <string.h>

#include <cadef.h>
#include <epicsGetopt.h>

#include "tool_lib_1.h"


#define VALSTRLEN 128    /* Length of value in a form of string */

#define VALID_DOUBLE_DIGITS 18  /* Max usable precision for a double */


/*sergey: reqElems cannot be global variable, otherwise first channel will set it to
non-zero value, and 'connection_handler' will not take right value from nElems */
#define REQELEMS 0
/*static unsigned long reqElems = 0;*/


static unsigned long eventMask = DBE_VALUE | DBE_ALARM;   /* Event mask used */
static int floatAsString = 0;                             /* Flag: fetch floats as string */
static int nConn = 0;                                     /* Number of connected PVs */


// for posix
#define _POSIX_SOURCE_ 1
#define __EXTENSIONS__


#define MAX_EPICS 250
#define MAX_CHANNELS_LENGTH 1000


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


#define MIN(a,b)  ( (a) < (b) ? (a) : (b) )
#define MAX(a,b)  ( (a) > (b) ? (a) : (b) )


// holds epics channel names, etc.
struct epics_struct
{
  char *name;
  char *chan;
  char *get;
};
static epics_struct epics[MAX_EPICS];
static epics_struct scalers[MAX_EPICS];
static epics_struct gammas[MAX_EPICS];


// for channel data
static int    nepics = sizeof(epics)/sizeof(epics_struct);
static float  bosarray[sizeof(epics)/sizeof(epics_struct)][9];   // F,8A
static float  epics_val[sizeof(epics)/sizeof(epics_struct)];
static char   epics_valstr[sizeof(epics)/sizeof(epics_struct)][VALSTRLEN];

// for scaler data
static int    nscalers = sizeof(epics)/sizeof(epics_struct);
static int    scalers_nval[sizeof(epics)/sizeof(epics_struct)];
static float  scalers_val[sizeof(epics)/sizeof(epics_struct)][MAX_CHANNELS_LENGTH];
static char   scalers_valstr[sizeof(epics)/sizeof(epics_struct)][VALSTRLEN*MAX_CHANNELS_LENGTH];

// for gamma data
static int    ngamma = sizeof(epics)/sizeof(char*);
static float  gammaarray[sizeof(epics)/sizeof(epics_struct)][9];   // F,8A
static float  gamma_val[sizeof(epics)/sizeof(epics_struct)];
static char   gamma_valstr[sizeof(epics)/sizeof(epics_struct)][VALSTRLEN];


// for scaler data
#define MAXSCALER 1000

// misc variables
static char *application       	 = (char*)"clastest";
char *session          		 = NULL;
char *clon_parms       		 = getenv("CLON_PARMS");
static char *host         	 = getenv("HOST");
static char *unique_id         	 = (char*)"epics_monitor";
static char dest[132];

static char *epics_config_name	       = (char*)"epics/epics_monitor.cfg";
static char *epics_channel_name	       = (char*)"epics/EPIC_summary.txt";
static int channel_wait_time   	       = 20;

static char *epics_scaler_config_name  = (char*)"epics/epics_scalers_monitor.cfg";
static int scaler_wait_time   	       = 3;

static char *epics_gamma_config_name   = (char*)"epics/epics_gamma_monitor.cfg";
static char *epics_gamma_name	       = (char*)"epics/GAMMA_summary.txt" ;
static double gamma_wait_time          = 2.0;

static int ipc_pend_time         = 1;
static time_t last_channel       = 0;
static time_t last_scaler        = 0;
static time_t last_gamma         = 0;
static int nev_to_ipc            = 0;
static int done                	 = 0;
static int dump                	 = 0;
static int no_ipc              	 = 0;
static int no_info             	 = 0;
static int no_file             	 = 0;
static int no_scalers         	 = 0;
static int nepics_read         	 = 0;
static int ngamma_read         	 = 0;
static int debug                 = 0;
static int lost_connection       = 0;
static char temp[256];
static char *msql_database    	 = (char*)"clasrun";
static float *bpm1,*bpm3;
static char buffer[256];


// other prototypes
void decode_command_line(int argc, char **argv);
int  init_epics(void);
void unpack_scaler_data(const char* bank, const int nrecord,
                        size_t nscaler, float *scalers, const char* channel);
void *channel_thread(void *param);
void *scaler_thread(void *param);
void *gamma_thread(void *param);
int find_tag_line(ifstream &file, const char *tag, char buffer[], int buflen);
int get_next_line(ifstream &file, char buffer[], int buflen);
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

//--------------------------------------------------------------------------

int
main(int argc,char **argv)
{
  int status;
  pthread_t t1;
  strstream temp2;
  time_t now;
  /* precise timing for gamma */
  timespec current  = {0,0};
  timespec lgamma     = {0,0};
  double dels,deln,deltat;


  // synch with stdio
  ios::sync_with_stdio();

  // decode command line...
  decode_command_line(argc,argv);


  // set session name if not specified via env variable or on command line
  if(session==NULL) session=(char*)"clasprod";


  // get event destination
  sprintf(dest,"evt_bosbank/%s",session);


  // set ipc parameters and connect to ipc system
  if(!TipcInitThreads()) {
    cerr << "Unable to init IPC thread package" << endl;
    exit(EXIT_FAILURE);
  }
  ipc_set_application(application);
  ipc_set_user_status_poll_callback(status_poll_callback);
  ipc_set_quit_callback(quit_callback);
  status=ipc_init(unique_id,"Epics monitor");
  if(status<0) {
    cerr << "\n?Unable to connect to server...probably duplicate unique id\n"
	 << "   ...check for another epics_monitor  using ipc_info\n"
	 << "   ...only one connection allowed!" << endl << endl;
    exit(EXIT_FAILURE);
  }


  /* EPICS stuff */

  for(int i=0; i<nepics; i++)
  {
    epics_val[i]=-9999.;
  }

  for(int i=0; i<nscalers; i++)
  {
    scalers_nval[i] = 0;
    for(int j=0; j<MAX_CHANNELS_LENGTH; j++)
      scalers_val[i][j]=-9999.;
  }

  for(int i=0; i<ngamma; i++)
  {
    gamma_val[i]=-9999.;
  }

  // initialize epics callbacks, request objects reinitialized each iteration
  // also read in epics channel list
  init_epics();

  /* EPICS stuff */


  // launch thread(s)
#ifdef SunOS
  thr_setconcurrency(thr_getconcurrency()+4);
#endif
  pthread_create(&t1,NULL,ipc_thread,(void*)NULL);


  // post startup message
  temp2 << "Process startup:    epics_monitor starting in " << application << ends;
  status=insert_msg("epics_monitor","online",unique_id,"status",0,"START",0,temp2.str());
  

  // flush output to log files, etc
  fflush(NULL);


  // launch threads for channels, and scalers
  // debug...thread stuff not working yet with cdev and ca
  while (done==0)
  {
    /* channels */
    now=time(NULL);
    if((now-last_channel)>channel_wait_time)
    {
      /*printf("call channel_thread\n");*/
      channel_thread(0);
      last_channel=now;
    }

    /* scalers */
    if(no_scalers==0)
    {
      now=time(NULL);
      if((now-last_scaler)>scaler_wait_time)
      {
        /*printf("call scaler_thread\n");*/
		scaler_thread(0);
	    last_scaler=now;
      }
    }

    /* use precise clock */
    clock_gettime(CLOCK_REALTIME,&current);
    dels=(double)(current.tv_sec-lgamma.tv_sec);
    deln=(double)(current.tv_nsec-lgamma.tv_nsec)/1.0e9;
    deltat=(deln>0)?(dels+deln):(dels-1.-deln);
    if(deltat>gamma_wait_time)
    {
      /*printf("call gamma_thread\n");*/
      gamma_thread(0);
      clock_gettime(CLOCK_REALTIME,&lgamma);
    }





    /* Read and print data forever */
    ca_pend_event(0.05/*0*/);
  }

  /* Shut down Channel Access */
  ca_context_destroy();

  // post shutdown message
  temp2.seekp(0,ios::beg);
  temp2 << "Process shutdown:  epics_monitor" << ends;
  status=insert_msg("epics_monitor","online",unique_id,"status",0,"STOP",0,temp2.str());
  
  // done...clean up
  ipc_close();
  exit(EXIT_SUCCESS);
}
       

//--------------------------------------------------------------------------


void *
channel_thread(void *param)
{

  nepics_read++;
  
  // ship data
  if(no_ipc==0)
  {
    nev_to_ipc++;
    for(int i=0; i<nepics; i++) bosarray[i][0] = epics_val[i];
    TipcMsg msg((T_STR)"evt_bosbank");
    msg.Sender((T_STR)"epics_monitor");
    msg.Dest(dest);
    msg << (T_STR)"EPIC" << (T_INT4)0 << (T_STR)"(F,8A)" << (T_INT4)9 << (T_INT4)nepics 
	<< (T_INT4)(nepics*9)
	<< SetSize(nepics*9) << (T_INT4*)bosarray;
    server.Send(msg);
    server.Flush();
  }
  
  
  // ship to info_server
  if(no_info==0)
  {
    TipcMsg message((T_STR)"info_server");
    message.Sender((T_STR)"epics_monitor");
    message.Dest((T_STR)"info_server/in/epics_monitor");
    message << (T_STR)"epics_monitor";
    for(int i=0; i<nepics; i++)
    {
      double tval[1] = {(double)epics_val[i]};
      if(strlen(epics[i].get)>4)
      {
	    sprintf(temp,"%s.%s",epics[i].chan,&(epics[i].get)[4]);
	    message << (T_STR) temp << SetSize(1) << tval << Check;
      }
      else
      {
	    message << (T_STR) epics[i].chan << SetSize(1) << tval << Check;
      }
    }
    server.Send(message);
    server.Flush();
  }
  
  
  // dump to file
  if(no_file==0)
  {
    strstream fn;
    fn << clon_parms<< "/" << epics_channel_name << ends; 
    ofstream file(fn.str());
    file.setf(ios::left);
    file.setf(ios::showpoint);
    time_t now=time(NULL);
    file << "\n#  epics EPIC summary created by epics_monitor on " << ctime(&now) << endl ;
    file << "#     xxx, 17-Oct-07"  << endl << endl << endl;
    file << "*epics*" << endl;
    file << endl 
	 << setw(27) << "#Channel name" << setw(20) << "Channel value" 
	 << "Channel description" << endl;
    file << setw(27) << "#------------" << setw(20) << "-------------" 
	 << "-------------------" << endl;
    for(int i=0; i<nepics; i++) {
      if(strlen(epics[i].get)>4) {
	sprintf(temp,"%s.%s",epics[i].chan,&(epics[i].get)[4]);
	file << setw(27) << temp << setw(20) << epics_val[i] 
	     << epics[i].name << endl;
      } else {
	file << setw(27) << epics[i].chan << setw(20) << epics_val[i] 
	     << epics[i].name << endl;
      }
    }
    file << endl;
    file.close();
  }
  
  
  // dump to screen
  if(dump!=0)
  {
    cout.setf(ios::left);
    cout.setf(ios::showpoint);
    cout << endl;
    for(int i=0; i<nepics; i++)
    {
      cout << setw(45) << epics[i].name << "    " << epics_val[i] << endl;
    }
    cout << endl;
  }
  
  return((void*)0);
}


//--------------------------------------------------------------------------


void *
scaler_thread(void *param)
{
  /*
  unpack_scaler_data("TESC",0,scalers_nval[0],scalers_val[0],"jscaler.SCALER_0");
  unpack_scaler_data("GP_X",0,scalers_nval[1],scalers_val[1],"gp_x");
  unpack_scaler_data("GP_Y",0,scalers_nval[2],scalers_val[2],"gp_y");
  unpack_scaler_data("TESC",1,scalers_nval[3],scalers_val[3],"jscaler.SCALER_1");
  unpack_scaler_data("TLSC",0,scalers_nval[4],scalers_val[4],"tag_t_left.VAL");
  unpack_scaler_data("TRSC",0,scalers_nval[5],scalers_val[5],"tag_t_right.VAL");
  */
  unpack_scaler_data("TESC",0,scalers_nval[0],scalers_val[0],"tgp_x.VAL");
  unpack_scaler_data("TESC",1,scalers_nval[1],scalers_val[1],"tgp_y.VAL");
  unpack_scaler_data("TESC",2,scalers_nval[2],scalers_val[2],"pgp_x");
  unpack_scaler_data("TESC",3,scalers_nval[3],scalers_val[3],"pgp_y");
  unpack_scaler_data("TESC",4,scalers_nval[4],scalers_val[4],"tpe_cal_scalers");
  /*unpack_scaler_data("TESC",5,scalers_nval[5],scalers_val[5],"tpecal_trig");*/

  return((void*)0);
}


//--------------------------------------------------------------------------


void *
gamma_thread(void *param)
{
  ngamma_read ++;

  // ship data
  if(no_ipc==0)
  {
    nev_to_ipc++;
    for(int i=0; i<ngamma; i++) gammaarray[i][0] = gamma_val[i];
    TipcMsg msg((T_STR)"evt_bosbank");
    msg.Sender((T_STR)"gamma_monitor"); /* ??? should we use 'epics_monitor' */
    msg.Dest(dest);
    msg << (T_STR)"EPIC" << (T_INT4)1 << (T_STR)"(F,8A)" << (T_INT4)9 << (T_INT4)ngamma
	<< (T_INT4)(ngamma*9)
	<< SetSize(ngamma*9) << (T_INT4*)gammaarray;
    server.Send(msg);
    server.Flush();
  }
  
  // dump to file
  if(no_file==0)
  {
    strstream fn;
    fn << clon_parms<< "/" << epics_gamma_name << ends; 
    ofstream file(fn.str());
    file.setf(ios::left);
    file.setf(ios::showpoint);
    time_t now=time(NULL);
    file << "\n#  epics GAMMA summary created by epics_monitor on " << ctime(&now) << endl ;
    file << "#     xxx, 17-Oct-07"  << endl << endl << endl;
    file << "*epics*" << endl;

    file << endl 
	 << setw(27) << "#Channel name" << setw(20) << "Channel value" << endl;
    file << setw(27) << "#------------" << setw(20) << "-------------" << endl;
    for(int i=0; i<ngamma; i++) {
      if(strlen(gammas[i].get)>4) {
	sprintf(temp,"%s.%s",gammas[i].name,&(gammas[i].get)[4]);
	file << setw(27) << temp << setw(20) << gamma_val[i] << endl;
      } else {
	file << setw(27) << gammas[i].name << setw(20) << gamma_val[i] << endl;
      }
    }
    file << endl;
    file.close();
  }
  
  // dump to screen
  if(dump!=0)
  {
    cout.setf(ios::left);
    cout.setf(ios::showpoint);
    cout << endl;
    for(int i=0; i<ngamma; i++)
    {
      cout << setw(45) << gammas[i].name << "    " << gamma_val[i] << endl;
    }
    cout << endl;
  }
  
  return((void*)0);
}


//--------------------------------------------------------------------------

void
unpack_scaler_data(const char* bank, const int nrecord,
                   size_t nscaler, float *scalers, const char* channel)
{
  int i,j;

  // unpack data
  if(nscaler>0)
  {
    // ship data
    if(no_ipc==0)
    {
      nev_to_ipc++;
      TipcMsg msg((T_STR)"evt_bosbank");
      msg.Sender((T_STR)"epics_monitor");
      msg.Dest(dest);
      msg << (T_STR)bank << (T_INT4)nrecord << (T_STR)"(F)" << (T_INT4)1 
	  << (T_INT4)nscaler << (T_INT4)nscaler
	  << SetSize(nscaler) << (T_INT4*)scalers;
      server.Send(msg);
      server.Flush();
    }
    
    
    // dump to file
    if(no_file==0)
    {
      if(nrecord==0)
        sprintf(temp,"%s/epics/%s_summary.txt",clon_parms,bank);
      else
        sprintf(temp,"%s/epics/%s_%1d_summary.txt",clon_parms,bank,nrecord);

	  /*printf("temp >%s< ( >%s< >%s< >%1d<)\n",temp,clon_parms,bank,nrecord);*/
      ofstream file(temp);
      file.setf(ios::left);
      file.setf(ios::showpoint);
      time_t now=time(NULL);
      file << "\n#  epics " << bank << " summary created by epics_monitor on " 
	   << ctime(&now) << endl ;
      file << "#     ejw, 29-aug-01"  << endl << endl << endl;
      file << "*epics_scalers* -- channel = \"" << channel << "\",  nscaler = " << nscaler << endl;
      for(i=0; i<nscaler; i+=8)
      {
	    for(j=i; j<i+8; j++)
        {
	      file << setw(11) << scalers[j] << " ";
	    }
	    file<< endl;
      }
      file << endl;
      file.close();
    }
    
    
    // dump to screen
    if(dump!=0)
    {
      cout.setf(ios::left);
      cout.setf(ios::showpoint);
      cout << endl;
      for(i=0; i<nscaler; i+=8)
      {
	    for(j=i; j<i+8; j++)
        {
	      cout << scalers[j] << "  ";
	    }
	    cout << endl;
      }
      cout << endl;
    }    
  }

  return;
}


//--------------------------------------------------------------------------


/*******************************************/
/*******************************************/
/****************** from $EPICS_BASE/src/catools/camonitor.c *************************/


/*+**************************************************************************
 *
 * Function:	event_handler
 *
 * Description:	CA event_handler for request type callback
 * 		Allocates the dbr structure and copies the data
 *
 * Arg(s) In:	args  -  event handler args (see CA manual)
 *
 **************************************************************************-*/

/* 'pv' defined in tool_lib.h
   'evargs' defined in cadef.h */
/* called if channel is changed */
void
event_handler (evargs args)
{
  pv *pvv;
  char *valstr;
  float valfloatarray[MAX_CHANNELS_LENGTH];
  int index1, index2, nelems = 0;

  int *valintarray;
  valintarray = (int *)valfloatarray;

  /* args.usr contains pointer to the 'ppv' from ca_create_subscription() call
  (see connection_handler) */
  pvv = (pv *)args.usr;

  pvv->status = args.status;
  if (args.status == ECA_NORMAL)
  {
    pvv->dbrType = args.type;


/*sergey
if(pvv->index==206)
{
  printf("index=%d nelems=%d %d, count=%d\n",pvv->index,pvv->reqElems,pvv->nElems,args.count);
}
*/
	/*
printf("[%3d] ------------------------------> count=%d (from %d %d)\n",
pvv->index,dbr_size_n(args.type, args.count),args.type, args.count);
	*/

    memcpy(pvv->value, args.dbr, dbr_size_n(args.type, args.count)); /*normally 24*/

    nelems = MIN(MAX_CHANNELS_LENGTH,pvv->nElems);
    if(nelems != pvv->nElems)
    {
      printf("event_handler: ERROR: pvv->nElems=%d != nelems=%d\n",pvv->nElems,nelems);
	}
    valstr = print_time_val_sts(pvv, nelems, valfloatarray);
  }
  else
  {
    printf("event_handler: ERROR: args.status != ECA_NORMAL\n");
    return;
  }
  /*
  printf("===>%s< %f\n",valstr,valfloatarray[0],valfloatarray[1],valfloatarray[2]);
  */

  index1 = pvv->index - nepics;
  index2 = pvv->index - nepics - nscalers;
  if(pvv->index < nepics)
  {
    strncpy((char *)epics_valstr[pvv->index],valstr,VALSTRLEN-1); /* not in use yet .. */
    epics_val[pvv->index] = valfloatarray[0];
  }
  else if(index1 < nscalers)
  {
	/*
if(index1==0) printf("scaler pvv: index=%d len=%d vals=%f %f %f %f %f %f\n",index1,nelems,
valfloatarray[0],valfloatarray[1],valfloatarray[2],
valfloatarray[3],valfloatarray[4],valfloatarray[5]);
	*/
	
    scalers_nval[index1] = nelems;
    for(int i=0; i<nelems; i++)
    {
      scalers_val[index1][i] = valfloatarray[i];
	}
  }
  else if(index2 < ngamma)
  {
    /*printf("gamma pvv: index=%d\n",index2);*/
    strncpy((char *)gamma_valstr[index2],valstr,VALSTRLEN-1); /* not in use yet .. */
    gamma_val[index2] = valfloatarray[0];
  }
  else
  {
    printf("event_handler: ERROR: unknown index=%d\n",pvv->index);
  }

}

/*+**************************************************************************
 *
 * Function:	connection_handler
 *
 * Description:	CA connection_handler 
 *
 * Arg(s) In:	args  -  connection_handler_args (see CA manual)
 *
 **************************************************************************-*/
void
connection_handler(struct connection_handler_args args)
{
  pv *ppv = (pv *) ca_puser(args.chid);
  char *valstr;
  float valfloatarray[MAX_CHANNELS_LENGTH];
  unsigned long reqElems = REQELEMS;

/* sergey: enforce all ENUM types to be reported as integers rather then strings */
enumAsNr = 1;

  if(args.op == CA_OP_CONN_UP)
  {
    int dbrType;

    /* Set up pv structure */
    /* ------------------- */

    /* Get natural type and array count */
    ppv->nElems  = ca_element_count(ppv->ch_id);
    ppv->dbfType = ca_field_type(ppv->ch_id);

    /* Set up value structures */
    dbrType = dbf_type_to_DBR_TIME(ppv->dbfType); /* Use native type */
    if(dbr_type_is_ENUM(dbrType))                 /* Enums honour -n option */
    {
      if(enumAsNr) dbrType = DBR_TIME_INT;
      else         dbrType = DBR_TIME_STRING;
    }

    else if(floatAsString &&
                 (dbr_type_is_FLOAT(dbrType) || dbr_type_is_DOUBLE(dbrType)))
    {
      dbrType = DBR_TIME_STRING;
    }

/*sergey
if(!strcmp(ppv->name,"gp_x")) printf("connection_handler: >%s< set reqElems to %d (nElems=%d)\n",
ppv->name,reqElems,ppv->nElems);
*/

    /* Adjust array count */
    if (reqElems == 0 || ppv->nElems < reqElems)
	{
      reqElems = ppv->nElems;
	}

    /* Remember dbrType and reqElems */
    ppv->dbrType  = dbrType;
    ppv->reqElems = reqElems;

    ppv->onceConnected = 1;
    nConn++;

    /* Issue CA request */
    /* ---------------- */
/* 'event_handler' WILL BE CALLED IF CHANNEL VALUE IS CHANGED */
    /* install monitor once with first connect */
    if ( ! ppv->value )
    {
      /* Allocate value structure */
      ppv->value = calloc(1, dbr_size_n(dbrType, reqElems));           
      if ( ppv->value )
      {
        ppv->status = ca_create_subscription(dbrType,
                                             reqElems,
                                             ppv->ch_id,
                                             eventMask,
                                             event_handler,
                                             (void*)ppv,
                                             NULL);
        if ( ppv->status != ECA_NORMAL )
        {
          free ( ppv->value );
        }
      }

    }
  }
  else if ( args.op == CA_OP_CONN_DOWN )
  {
    nConn--;
    ppv->status = ECA_DISCONN;
    printf("DISCONNECTED\n");
    valstr = print_time_val_sts(ppv, ppv->reqElems, valfloatarray);
  }
}


/*******************************************/
/*******************************************/


/* main() calls it to setup EPICS callbacks */
int
init_epics()
{
  int i;
  char *blanks = (char*) malloc(64);
  char *p1,*p2;


  /****************************************/
  /* read and process channel config file */

  // fill blanks
  for(i=0; i<31; i++) blanks[i]=' ';
  blanks[31]='\0';

  // open channel config file
  strstream fn;
  fn << clon_parms<< "/" << epics_config_name << ends; 
  ifstream file(fn.str());
  if(!file.is_open())
  {
    cerr << "?unable to open " << fn << endl;
    exit(EXIT_FAILURE);
  }
  
  // find beginning of channel list
  if(find_tag_line(file,"*CHANNELS*",buffer,sizeof(buffer))!=0)
  {
    cerr << "Can't find *CHANNELS* tag in " << fn.str() << endl;
    exit(EXIT_FAILURE);
  }

  // read in all channels
  nepics=0;
  while (get_next_line(file,buffer,sizeof(buffer))==0)
  {
    nepics++;

    p1=strchr(buffer,'"')+1;  p2=strchr(p1,'"');  
    strncpy(temp,p1,p2-p1);   temp[p2-p1]=NULL;
    epics[nepics-1].name = strdup(temp);

    p1=strchr(p2+1,'"')+1;    p2=strchr(p1,'"');  
    strncpy(temp,p1,p2-p1);   temp[p2-p1]=NULL;
    epics[nepics-1].chan = strdup(temp);

    p1=strchr(p2+1,'"')+1;    p2=strchr(p1,'"');  
    strncpy(temp,p1,p2-p1);   temp[p2-p1]=NULL;
    epics[nepics-1].get = strdup(temp);

    if(nepics>=MAX_EPICS)
    {
      cerr << "Too many epics channels in " << fn << ", excess ignored" << endl;
      break;
    }
  }

  // copy epics channel names (31 char max) into special bos array...32nd char is NULL
  // concatenate get string
  if(no_ipc==0)
  {
    int i;
    for(i=0; i<nepics; i++) strncpy((char *)(&bosarray[i][1]),blanks,32);
    for(i=0; i<nepics; i++)
    {
      if(strlen(epics[i].get)>4)
      {
	    sprintf(temp,"%s.%s",epics[i].chan,&(epics[i].get)[4]);
	    strncpy((char *)(&bosarray[i][1]),temp,MIN(strlen(temp),31));
      }
      else
      {
	    strncpy((char *)(&bosarray[i][1]),epics[i].chan,MIN(strlen(epics[i].chan),31));
      }
    }
  }

  /*print*/
  printf("\nepics %3d channels =====================\n",nepics);
  for(i=0; i<nepics; i++)
  {
    printf("[%3d] name=>%s<\t\tchan=>%s<\t\tget=>%s<\n",i,
      epics[i].name,epics[i].chan,epics[i].get);
  }
  printf("======================================\n");







  /***************************************/
  /* read and process scaler config file */

  // open channel config file
  strstream fns;
  fns << clon_parms<< "/" << epics_scaler_config_name << ends; 
  ifstream files(fns.str());
  if(!files.is_open())
  {
    cerr << "?unable to open " << fns << endl;
    exit(EXIT_FAILURE);
  }
  
  // find beginning of channel list
  if(find_tag_line(files,"*CHANNELS*",buffer,sizeof(buffer))!=0)
  {
    cerr << "Can't find *CHANNELS* tag in " << fns.str() << endl;
    exit(EXIT_FAILURE);
  }

  // read in all channels
  nscalers=0;
  while (get_next_line(files,buffer,sizeof(buffer))==0)
  {
    nscalers++;

    p1=strchr(buffer,'"')+1;  p2=strchr(p1,'"');  
    strncpy(temp,p1,p2-p1);   temp[p2-p1]=NULL;
    scalers[nscalers-1].name = strdup(temp);

    p1=strchr(p2+1,'"')+1;    p2=strchr(p1,'"');  
    strncpy(temp,p1,p2-p1);   temp[p2-p1]=NULL;
    scalers[nscalers-1].chan = strdup(temp);

    p1=strchr(p2+1,'"')+1;    p2=strchr(p1,'"');  
    strncpy(temp,p1,p2-p1);   temp[p2-p1]=NULL;
    scalers[nscalers-1].get = strdup(temp);

    if(nscalers>=MAX_EPICS)
    {
      cerr << "Too many scaler channels in " << fns << ", excess ignored" << endl;
      break;
    }
  }

  /*print*/
  printf("\nscaler %3d channels =====================\n",nscalers);
  for(i=0; i<nscalers; i++)
  {
    printf("[%3d] name=>%s<\t\tchan=>%s<\t\tget=>%s<\n",i,
      scalers[i].name,scalers[i].chan,scalers[i].get);
  }
  printf("=======================================\n");



  /**************************************/
  /* read and process gamma config file */

  // fill blanks
  for(i=0; i<31; i++) blanks[i]=' ';
  blanks[31]='\0';

  // open channel config file
  strstream fng;
  fng << clon_parms<< "/" << epics_gamma_config_name << ends; 
  ifstream fileg(fng.str());
  if(!fileg.is_open())
  {
    cerr << "?unable to open " << fng << endl;
    exit(EXIT_FAILURE);
  }
  
  // find beginning of channel list
  if(find_tag_line(fileg,"*CHANNELS*",buffer,sizeof(buffer))!=0)
  {
    cerr << "Can't find *CHANNELS* tag in " << fng.str() << endl;
    exit(EXIT_FAILURE);
  }

  // read in all channels
  ngamma=0;
  while (get_next_line(fileg,buffer,sizeof(buffer))==0)
  {
    ngamma++;

    p1=strchr(buffer,'"')+1;  p2=strchr(p1,'"');  
    strncpy(temp,p1,p2-p1);   temp[p2-p1]=NULL;
    gammas[ngamma-1].name = strdup(temp);

    p1=strchr(p2+1,'"')+1;    p2=strchr(p1,'"');  
    strncpy(temp,p1,p2-p1);   temp[p2-p1]=NULL;
    gammas[ngamma-1].chan = strdup(temp);

    p1=strchr(p2+1,'"')+1;    p2=strchr(p1,'"');  
    strncpy(temp,p1,p2-p1);   temp[p2-p1]=NULL;
    gammas[ngamma-1].get = strdup(temp);

    if(ngamma>=MAX_EPICS)
    {
      cerr << "Too many gamma channels in " << fng << ", excess ignored" << endl;
      break;
    }
  }

  // copy gamma channel names (31 char max) into special bos array...32nd char is NULL
  // concatenate get string
  if(no_ipc==0)
  {
    int i;
    for(i=0; i<ngamma; i++) strncpy((char *)(&gammaarray[i][1]),blanks,32);
    for(i=0; i<ngamma; i++)
    {
      if(strlen(gammas[i].get)>4)
      {
	    sprintf(temp,"%s.%s",gammas[i].chan,&(gammas[i].get)[4]);
	    strncpy((char *)(&gammaarray[i][1]),temp,MIN(strlen(temp),31));
      }
      else
      {
	    strncpy((char *)(&gammaarray[i][1]),gammas[i].chan,MIN(strlen(gammas[i].chan),31));
      }
    }
  }

  /*print*/
  printf("\ngamma %3d channels =====================\n",ngamma);
  for(i=0; i<ngamma; i++)
  {
    printf("[%3d] name=>%s<\t\tchan=>%s<\t\tget=>%s<\n",i,
      gammas[i].name,gammas[i].chan,gammas[i].get);
  }
  printf("======================================\n");




  /***************************************/
  /***************************************/









  /* ?????
  // create channel callback objects once only
  for(i=0; i<nepics; i++)
  {
    cb[i]    = new cdevCallback(channel_callback_func,(void*)i);
    if(strcmp(epics[i].name,"bpm_1_i")==0) bpm1=&(epics_val[i]); // for helicity file
    if(strcmp(epics[i].name,"bpm_3_i")==0) bpm3=&(epics_val[i]);
  }
  */



  /*****************************************************/
  /* setting callbacks in case of channel value change */
  {
    int returncode = 0;
    int n, nn;
    int result;    /* CA result */
    int nPvs;      /* Number of PVs */
    pv *pvs = 0;   /* Array of PV structures */
    char *valstr;
    float valfloatarray[MAX_CHANNELS_LENGTH];

    /* check if we have at lease one pv */
    nPvs = nepics + nscalers + ngamma;
    if(nPvs < 1)
    {
      fprintf(stderr, "epics_monitor: No pv names specified\n");
      return(1);
    }

    /* Start up Channel Access */
    result = ca_context_create(ca_disable_preemptive_callback);
    if(result != ECA_NORMAL)
    {
      fprintf(stderr, "epics_monitor: CA error %s occurred while trying "
              "to start channel access '%s'.\n", ca_message(result), pvs[n].name);
      return(1);
    }

    /* Allocate PV structure array */
    pvs = (pv *)calloc(nPvs, sizeof(pv));
    if (!pvs)
    {
      fprintf(stderr, "Memory allocation for channel structures failed.\n");
      return(1);
    }

    /* Connect channels; we'll put all 'epics' and 'scalers' and whatever
    into one 'pvs' array with through enumeration; event_handler will check
    '.index' and recognize if it is 'epics' or 'scalers' etc */

    /* Copy PV names from epics[].name and assign index starting from 0 */
    nn = 0;
    for(n=0; n<nepics; n++)
    {
      pvs[nn].name = epics[n].chan;
      pvs[nn].index = nn;
      nn ++;
    }
    /* same for scalers */
    for(n=0; n<nscalers; n++)
    {
      pvs[nn].name = scalers[n].chan;
      pvs[nn].index = nn;
      nn ++;
    }
    /* same for gamma */
    for(n=0; n<ngamma; n++)
    {
      pvs[nn].name = gammas[n].chan;
      pvs[nn].index = nn;
      nn ++;
    }

    /* Create CA connections */
    returncode = create_pvs(pvs, nPvs, connection_handler);
    if(returncode)
    {
      return(returncode);
    }

    /* Check for channels that didn't connect */
    ca_pend_event(caTimeout);
    for(n=0; n<nPvs; n++)
    {
      if(!pvs[n].onceConnected)
	  {
        /*printf("NOT CONNECTED\n");*/
        valstr = print_time_val_sts(&pvs[n], pvs[n].reqElems, valfloatarray);
      }
    }


  }

  return(0);
}



/********************************************************************/
/**************** UTILITY functions (no epics calls) ****************/
/********************************************************************/

//--------------------------------------------------------------------------
void
status_poll_callback(T_IPC_MSG msg)
{  
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

  TipcMsgAppendStr(msg,(char*)"");
  TipcMsgAppendStr(msg,(char*)"");

  /* ??? need same for gamma ??? */
  for(int i=0; i<nepics; i++) {
    TipcMsgAppendStr(msg,(char*)epics[i].name);
    TipcMsgAppendReal4(msg,epics_val[i]);
  }

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
  const char *help = "\nusage:\n\n  epics_monitor [-a application] [-s session] [-u unique_id]\n"
    "        [-cw channel_wait_time] [-sw scaler_wait_time] [-gw gamma_wait_time]\n"
    "        [-ipc ipc_pend_time] [-m msql_database]\n"
    "        [-no_ipc] [-no_info] [-no_file] [-no_scalers] [-dump] [-debug]\n";


  // loop over all arguments, except the 1st (which is program name)
  int i=1;
  while(i<argc) {
    if(strncasecmp(argv[i],"-h",2)==0) {
      cout << help << endl;
      exit(EXIT_SUCCESS);
    }
    else if (strncasecmp(argv[i],"-no_scalers",11)==0) {
      no_scalers=1;
      i=i+1;
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
    else if (strncasecmp(argv[i],"-dump",5)==0) {
      dump=1;
      i=i+1;
    }
    else if (strncasecmp(argv[i],"-debug",6)==0) {
      debug=1;
      i=i+1;
    }
    else if (strncasecmp(argv[i],"-ipc",3)==0) {
      ipc_pend_time=atoi(argv[i+1]);
      i=i+2;
    }
    else if (strncasecmp(argv[i],"-cw",3)==0) {
      channel_wait_time=atoi(argv[i+1]);
      i=i+2;
    }
    else if (strncasecmp(argv[i],"-sw",3)==0) {
      scaler_wait_time=atoi(argv[i+1]);
      i=i+2;
    }
    else if (strncasecmp(argv[i],"-gw",3)==0) {
      gamma_wait_time=atof(argv[i+1]);
      i=i+2;
    }
    else if (strncasecmp(argv[i],"-a",2)==0) {
      application=strdup(argv[i+1]);
      i=i+2;
    }
    else if (strncasecmp(argv[i],"-s",2)==0) {
      session=strdup(argv[i+1]);
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

/*
CAC TCP socket shutdown error was Transport endpoint is not connected
ipc_thread exited

ipc_close called...closing connection at Wed Nov  7 14:04:10 2007



IPC atexit handler called at Wed Nov  7 14:04:10 2007


IGT0I00BIASET                  *** Not connected (PV not found)
PSPECIRBCK                     *** Not connected (PV not found)
harp.DRBV                      *** Not connected (PV not found)
bom_sc_sum                     *** Not connected (PV not found)
hallbptdt                      *** Not connected (PV not found)
hallbpttgt                     *** Not connected (PV not found)
hallbptenc                     *** Not connected (PV not found)
hallbptlinenc                  *** Not connected (PV not found)
hallbptindex                   *** Not connected (PV not found)
hallbptcur                     *** Not connected (PV not found)
hallbpteio                     *** Not connected (PV not found)
hallbptcal                     *** Not connected (PV not found)
hallbptpol                     *** Not connected (PV not found)
LL8240                         *** Not connected (PV not found)
ITC502_2_T1                    *** Not connected (PV not found)
LongCellCharge                 *** Not connected (PV not found)
ShortCellCharge                *** Not connected (PV not found)
FI_He3_moles                   *** Not connected (PV not found)



*/
