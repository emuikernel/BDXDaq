h25316
s 00233/00185/00567
d D 1.9 10/04/05 12:05:47 boiarino 10 9
c *** empty log message ***
e
s 00004/00096/00748
d D 1.8 07/10/16 23:37:12 boiarino 9 8
c *** empty log message ***
e
s 00004/00000/00840
d D 1.7 07/10/12 14:46:17 boiarino 8 7
c *** empty log message ***
e
s 00005/00001/00835
d D 1.6 06/10/17 16:05:39 boiarino 7 6
c EPICS3.14-related fixes
c 
e
s 00006/00005/00830
d D 1.5 06/03/03 14:25:30 wolin 6 5
c Commented out sscanf of prescale info, getting seg fault and prescale info no longer generated
e
s 00003/00003/00832
d D 1.4 04/11/12 10:39:45 sergpozd 5 4
c Some changes in reading mon_det_p.txt files
e
s 00001/00001/00834
d D 1.3 03/07/11 09:32:33 wolin 4 3
c Typo
e
s 00112/00011/00723
d D 1.2 03/07/10 16:40:47 wolin 3 1
c Working
e
s 00000/00000/00000
d R 1.2 03/07/10 13:26:44 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 epicsutil/s/clas_epics_server.cc
e
s 00734/00000/00000
d D 1.1 03/07/10 13:26:43 wolin 1 0
c 
e
u
U
f b 
f e 0
t
T
I 1
//  clas_epics_server

// CLAS portable ca server cross-posts assorted DAQ/Online info 
//   from ipc and files as epics channels 

D 3

E 3
//  ejw, 3-jul-2003



// for posix
#define _POSIX_SOURCE_ 1
#define __EXTENSIONS__


// for ca 
#include <casdef.h>
#include <fdManager.h>
#include <gdd.h>
D 3
#include <gddApps.h>
E 3


D 3
//  epics channel names, etc.
#include <clas_epics_server.h>


E 3
// epics server class defs
#include <epics_server.h>


I 3
//  epics channel names, etc.
#include <clas_epics_server.h>


E 3
// for smartsockets
#include <rtworks/cxxipc.hxx>


// CLAS ipc
#include <clas_ipc_prototypes.h>


// misc
#include <pthread.h>
I 8
#ifdef SunOS
E 8
#include <thread.h>
I 8
#endif
E 8
#include <iostream.h>
#include <fstream.h>
#include <iomanip.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>


D 9
// for tcl
extern "C" {
#include <tcl.h>
Tcl_Interp *tclinterp_init(void);
void tclipc_init(Tcl_Interp *interp);
}


E 9
// misc variables
static char *application       	    = (char*)"clastest";
D 4
static char *unique_id         	    = (char*)"epics_server";
E 4
I 4
static char *unique_id         	    = (char*)"clas_epics_server";
E 4
static const char *ec_pretrig_file  = "diman/mondat/mon_ec_p.txt";
static const char *cc_pretrig_file  = "diman/mondat/mon_cc_p.txt";
static const char *sc_pretrig_file  = "diman/mondat/mon_sc_p.txt";
static const char *tdc_file         = "discr/archive/discr_clasprod";
D 9
static char *init_tcl_script   	    = NULL;
E 9
static char *clon_parms             = getenv("CLON_PARMS");
D 9
static Tcl_Interp *interp;     	    
E 9
static int ca_pend_time             = 5;
static int data_sleep_time          = 30;
I 3
static int archive_sleep_time       = 60;
static char *archive_file           = (char*)"epics/clas_epics_server.dat";
E 3
static int ipc_pend_time            = 1;
static int callback_count           = 0;
static int done                	    = 0;
static int debug                    = 0;
static int run_number               = 0;
static char temp[4096];
static char filename[256];
static char line[1024];


//  user-defined channel names, units, etc and array of pv's
class myPV;
class myAttrPV;
static myPV *pPV[sizeof(pvNames)/sizeof(char*)];


// other prototypes
void decode_command_line(int argc, char **argv);
D 9
void init_tcl(void);
E 9
I 3
void read_archive();
void read_pv_info(ifstream &i, int *a, int len);
E 3
void epics_server_callback(
			   T_IPC_CONN conn,
			   T_IPC_CONN_PROCESS_CB_DATA data,
			   T_CB_ARG arg);
void get_ec_p(ifstream &f);
void get_cc_p(ifstream &f);
void get_sc_p(ifstream &f);
void get_tdc_info(ifstream &f);
int find_tag_line(ifstream &file, const char *tag, char buffer[], int buflen);
int get_next_line(ifstream &file, char buffer[], int buflen);
extern "C" {
void *ipc_thread(void *param);
void *data_thread(void *param);
I 3
void *archive_thread(void *param);
E 3
void quit_callback(int sig);
void status_poll_callback(T_IPC_MSG msg);
D 9
int tcl_help(ClientData clientdata, Tcl_Interp *interp, 
	       int argc, char **argv);
int tcl_quit(ClientData clientdata, Tcl_Interp *interp, 
	       int argc, char **argv);
E 9
int insert_msg(const char *name, const char *facility, const char *process, const char *msgclass, 
	       int severity, const char *status, int code, const char *text);
}


// ref to IPC server (connection created later)
TipcSrv &server=TipcSrv::Instance();


//--------------------------------------------------------------------------
//--------------------------------------------------------------------------


class myServer : public caServer {
  

public:
    
D 10
  pvExistReturn pvExistTest(const casCtx &ctx, const char *pPVName) {
E 10
I 10
  pvExistReturn pvExistTest(const casCtx &ctx, const char *pPVName)
  {
E 10
    
    int PVLen = strcspn(pPVName,".");

D 10
    for(int i=0; i<sizeof(pvNames)/sizeof(char*); i++) {
E 10
I 10
    for(int i=0; i<sizeof(pvNames)/sizeof(char*); i++)
    {
E 10
      if((PVLen==strlen(pvNames[i]))&&(strncasecmp(pPVName,pvNames[i],PVLen)==0))
	 return pverExistsHere;
    }
    return pverDoesNotExistHere;
  }
  
  
//---------------------------------------------------


D 10
  pvCreateReturn createPV(const casCtx &ctx, const char *pPVName) {
E 10
I 10
  pvCreateReturn createPV(const casCtx &ctx, const char *pPVName)
  {
E 10
    if(debug!=0)cout << "createPV for " << pPVName << endl;

    int PVNameLen      = strlen(pPVName);
    int PVLen          = strcspn(pPVName,".");
    const char *pattr  = pPVName+PVLen+1;
    int lattr          = PVNameLen-PVLen-1;

D 10
    for(int i=0; i<sizeof(pvNames)/sizeof(char*); i++) {
	if((PVLen==strlen(pvNames[i]))&&(strncasecmp(pPVName,pvNames[i],PVLen)==0)) {
	    if( (PVNameLen==PVLen) || ((lattr==3)&&(strncasecmp(pattr,"VAL",3)==0)) ) {
		return(*pPV[i]);
	    } else {
		return(*(new myAttrPV(pPV[i],pattr,lattr)));
E 10
I 10
    for(int i=0; i<sizeof(pvNames)/sizeof(char*); i++)
    {
	  if((PVLen==strlen(pvNames[i]))&&(strncasecmp(pPVName,pvNames[i],PVLen)==0))
      {
	    if( (PVNameLen==PVLen) || ((lattr==3)&&(strncasecmp(pattr,"VAL",3)==0)) )
        {
		  return(*pPV[i]);
E 10
	    }
D 10
	}
E 10
I 10
        else
        {
		  return(*(new myAttrPV(pPV[i],pattr,lattr)));
	    }
	  }
E 10
    }
    return(S_casApp_pvNotFound);
  }
  
    
//---------------------------------------------------
    
    
D 10
    ~myServer() {
E 10
I 10
    ~myServer()
    {
E 10
      if(debug!=0)cout << "myServer destructor" << endl;
      return;
    }
    
};


//---------------------------------------------------------------------------
//---------------------------------------------------------------------------


D 9
main(int argc,char **argv) {


E 9
I 9
main(int argc,char **argv)
{
E 9
  int status;
D 3
  pthread_t t1,t2;
E 3
I 3
  pthread_t t1,t2,t3;
E 3


  // synch with stdio
  ios::sync_with_stdio();


D 9
  // decode command line...flags may be overridden in Tcl startup script
E 9
I 9
  // decode command line...
E 9
  decode_command_line(argc,argv);

D 9

  // get Tcl interp, create tcl commands, link vars, process Tcl startup script, etc.
  interp=tclinterp_init();
  init_tcl();
  if(init_tcl_script!=NULL)Tcl_EvalFile(interp,init_tcl_script);
  

E 9
  // set ipc parameters and connect to ipc system
  if(!TipcInitThreads()) {
    cerr << "Unable to init IPC thread package" << endl;
    exit(EXIT_FAILURE);
  }
  ipc_set_application(application);
  ipc_set_user_status_poll_callback(status_poll_callback);
  ipc_set_quit_callback(quit_callback);
  status=ipc_init(unique_id,"Epics Server");
  if(status<0) {
    cerr << "\n?Unable to connect to server...probably duplicate unique id\n"
	 << "   ...check for another epics_server  using ipc_info\n"
	 << "   ...only one connection allowed!" << endl << endl;
    exit(EXIT_FAILURE);
  }


  //  setup smartsockets callbacks
  server.SubjectSubscribe((T_STR)"/info_server/out/epics_server",TRUE);
  server.SubjectCbCreate((T_STR)"/info_server/out/epics_server",NULL,epics_server_callback,NULL);


D 9
  //  create tcl_request callback
  tclipc_init(interp);


E 9
I 3
  // set epics server debug flag
  setDebug(debug);


  // read pv alarm info from archive file
  read_archive();


E 3
  // create array of pv objects
  for(int i=0; i<sizeof(pvNames)/sizeof(char*); i++) {
      pPV[i] = new myPV(pvNames[i],pvTypes[i],pvUnits[i],pvAlarm[i],pvHIHI[i],pvLOLO[i],
			pvHIGH[i],pvLOW[i],pvHOPR[i],pvLOPR[i],pvDRVH[i],pvDRVL[i],pvPREC[i]);
  }


  // create ca server
  myServer *cas = new myServer();


  // launch threads
I 8
#ifdef SunOS
E 8
D 3
  thr_setconcurrency(thr_getconcurrency()+3);
E 3
I 3
  thr_setconcurrency(thr_getconcurrency()+4);
I 8
#endif
E 8
E 3
  pthread_create(&t1,NULL,data_thread,(void*)NULL);
  pthread_create(&t2,NULL,ipc_thread,(void*)NULL);
I 3
  pthread_create(&t3,NULL,archive_thread,(void*)NULL);
E 3


  // post startup message
  sprintf(temp,"Process startup:    epics_server starting in %s",application);
  status=insert_msg("epics_server","online",unique_id,"status",0,"START",0,temp);
  

  // flush output to log files, etc
  fflush(NULL);


  //  ca server loop
D 10
  while(done==0) {
E 10
I 10
  while(done==0)
  {
E 10

    fileDescriptorManager.process((double)ca_pend_time);

    // check for updates and satisfy monitorOn requests
D 10
    for(int i=0; i<sizeof(pvNames)/sizeof(char*); i++) {
      if((pPV[i]->myMonitor!=0)&&(pPV[i]->myUpdate!=0)) {
D 3
	  if(debug!=0)cout << "(monitorOn response for " << pvNames[i] << ")" << endl;
E 3
I 3
	if(debug!=0)cout << "(monitorOn response for " << pvNames[i] << ")" << endl;
E 3
	gdd *value = new gdd();
E 10
I 10
    for(int i=0; i<sizeof(pvNames)/sizeof(char*); i++)
    {
      if((pPV[i]->myMonitor!=0)&&(pPV[i]->myUpdate!=0))
      {
	    if(debug!=0)cout << "(monitorOn response for " << pvNames[i] << ")" << endl;
	    gdd *value = new gdd();
E 10
I 7

E 7
D 10
	caServer *pCAS = pPV[i]->getCAS();
D 7
	casEventMask select(pCAS->valueEventMask | pCAS->logEventMask);
E 7
I 7
	/*sergey casEventMask select(pCAS->valueEventMask | pCAS->logEventMask);*/
	casEventMask select( pCAS->valueEventMask() | pCAS->logEventMask());
E 10
I 10
	    caServer *pCAS = pPV[i]->getCAS();
	    /*sergey casEventMask select(pCAS->valueEventMask | pCAS->logEventMask);*/
	    casEventMask select( pCAS->valueEventMask() | pCAS->logEventMask());
E 10

D 10

E 7
	pPV[i]->getVAL(*value);
	pPV[i]->postEvent(select,*value);
	pPV[i]->myUpdate=0;
E 10
I 10
	    pPV[i]->getVAL(*value);
	    pPV[i]->postEvent(select,*value);
	    pPV[i]->myUpdate=0;
E 10
      }
    }
  }


  // post shutdown message
  sprintf(temp,"Process shutdown:  epics_server");
  status=insert_msg("epics_server","online",unique_id,"status",0,"STOP",0,temp);
  
  
  // done...clean up
  ipc_close();
  exit(EXIT_SUCCESS);
}
       

//--------------------------------------------------------------------------


I 3
//  reads archive file and fills pvXXXX arrays prior to pv initialization
D 10
void read_archive() {
E 10
I 10
void
read_archive()
{
E 10

  int len;

D 10

    sprintf(temp,"%s/%s",clon_parms,archive_file);
    if(debug!=0)cout << "reading archive file " << temp << endl;
    ifstream file(temp);
    if(!file.is_open()) {
      cerr << "?Unable to open archive file: " << temp << endl;
      return;
    }
E 10
I 10
  sprintf(temp,"%s/%s",clon_parms,archive_file);
  if(debug!=0)cout << "reading archive file " << temp << endl;
  ifstream file(temp);
  if(!file.is_open())
  {
    cerr << "?Unable to open archive file: " << temp << endl;
    return;
  }
E 10
    

D 10
    file >> len;
    if(len!=sizeof(pvNames)/sizeof(char*)) {
      cerr << "?Unable to read archive, lengths disagree, program/archive are: " 
E 10
I 10
  file >> len;
  if(len!=sizeof(pvNames)/sizeof(char*))
  {
    cerr << "?Unable to read archive, lengths disagree, program/archive are: " 
E 10
	   << sizeof(pvNames)/sizeof(char*) << "/" << len << endl;
D 10
      file.close();
      return;
    }
E 10
I 10
    file.close();
    return;
  }
E 10


D 10
    read_pv_info(file,pvAlarm,sizeof(pvNames)/sizeof(char*)); 
    read_pv_info(file,pvHIHI,sizeof(pvNames)/sizeof(char*)); 
    read_pv_info(file,pvLOLO,sizeof(pvNames)/sizeof(char*)); 
    read_pv_info(file,pvHIGH,sizeof(pvNames)/sizeof(char*)); 
    read_pv_info(file,pvLOW,sizeof(pvNames)/sizeof(char*)); 
    read_pv_info(file,pvHOPR,sizeof(pvNames)/sizeof(char*)); 
    read_pv_info(file,pvLOPR,sizeof(pvNames)/sizeof(char*)); 
    read_pv_info(file,pvDRVH,sizeof(pvNames)/sizeof(char*)); 
    read_pv_info(file,pvDRVL,sizeof(pvNames)/sizeof(char*)); 
E 10
I 10
  read_pv_info(file,pvAlarm,sizeof(pvNames)/sizeof(char*)); 
  read_pv_info(file,pvHIHI,sizeof(pvNames)/sizeof(char*)); 
  read_pv_info(file,pvLOLO,sizeof(pvNames)/sizeof(char*)); 
  read_pv_info(file,pvHIGH,sizeof(pvNames)/sizeof(char*)); 
  read_pv_info(file,pvLOW,sizeof(pvNames)/sizeof(char*)); 
  read_pv_info(file,pvHOPR,sizeof(pvNames)/sizeof(char*)); 
  read_pv_info(file,pvLOPR,sizeof(pvNames)/sizeof(char*)); 
  read_pv_info(file,pvDRVH,sizeof(pvNames)/sizeof(char*)); 
  read_pv_info(file,pvDRVL,sizeof(pvNames)/sizeof(char*)); 
E 10

D 10
    file.close();
E 10
I 10
  file.close();
E 10
}


//--------------------------------------------------------------------------


D 10
void read_pv_info(ifstream &i, int *a, int len) {

E 10
I 10
void
read_pv_info(ifstream &i, int *a, int len)
{
E 10
  for(int j=0; j<len; j++) i >> a[j];
D 10

E 10
}


//--------------------------------------------------------------------------


E 3
//  periodically retrieves online info from files
D 10
void *data_thread(void *param) {
E 10
I 10
void *
data_thread(void *param)
{
  ifstream file;
E 10

D 10
    ifstream file;
E 10
I 10
  while(done==0)
  {
E 10

D 10
  while(done==0) {

E 10
    // ec pretrig
    sprintf(filename,"%s/%s",clon_parms,ec_pretrig_file);
    file.open(filename);
D 10
    if(!file.is_open()) {
	cerr << "?unable to open " << filename << endl;
    } else {
	get_ec_p(file);
	file.close();
E 10
I 10
    if(!file.is_open())
    {
	  cerr << "?unable to open " << filename << endl;
E 10
    }
I 10
    else
    {
	  get_ec_p(file);
	  file.close();
    }
E 10


    // cc pretrig
    sprintf(filename,"%s/%s",clon_parms,cc_pretrig_file);
    file.open(filename);
D 10
    if(!file.is_open()) {
	cerr << "?unable to open " << filename << endl;
    } else {
	get_cc_p(file);
	file.close();
E 10
I 10
    if(!file.is_open())
    {
	  cerr << "?unable to open " << filename << endl;
E 10
    }
I 10
    else
    {
	  get_cc_p(file);
	  file.close();
    }
E 10


    // sc pretrig
    sprintf(filename,"%s/%s",clon_parms,sc_pretrig_file);
    file.open(filename);
D 10
    if(!file.is_open()) {
	cerr << "?unable to open " << filename << endl;
    } else {
	get_sc_p(file);
	file.close();
E 10
I 10
    if(!file.is_open())
    {
	  cerr << "?unable to open " << filename << endl;
E 10
    }
I 10
    else
    {
	  get_sc_p(file);
	  file.close();
    }
E 10


    // tdc info
D 10
    if(run_number>0) {
	sprintf(filename,"%s/%s_%06d.txt",clon_parms,tdc_file,run_number);
	file.open(filename);
	if(!file.is_open()) {
E 10
I 10
    if(run_number>0)
    {
	  sprintf(filename,"%s/%s_%06d.txt",clon_parms,tdc_file,run_number);
	  file.open(filename);
	  if(!file.is_open())
      {
E 10
	    cerr << "?unable to open " << filename << endl;
D 10
	} else {
E 10
I 10
	  }
      else
      {
E 10
	    get_tdc_info(file);
	    file.close();
D 10
	}
E 10
I 10
	  }
E 10
    }


    // until next time...
    sleep(data_sleep_time);
  }  

  return((void*)NULL);
}


//--------------------------------------------------------------------------


//  reads ec pretrig info
D 10
void get_ec_p(ifstream &f) {
E 10
I 10
void
get_ec_p(ifstream &f)
{
  int i,j;
  int avg,p[6];
E 10
    
D 10
    int i,j;
    int avg,p[6];
    
E 10

D 10
    // skip some lines
D 5
    for(i=0; i<5; i++)f.getline(line,sizeof(line));
E 5
I 5
    for(i=0; i<6; i++)f.getline(line,sizeof(line));
E 10
I 10
  // skip some lines
  for(i=0; i<6; i++)f.getline(line,sizeof(line));
E 10
E 5

D 10
    // inner,outer,total for hi,lo
    for(i=0; i<6; i++) {
E 10
I 10
  // inner,outer,total for hi,lo
  for(i=0; i<6; i++)
  {
E 10
	avg=0;
D 10
	if(f.is_open()&&f.good()&&!f.eof()&&!f.fail()&&!f.bad()) {
E 10
I 10
	if(f.is_open()&&f.good()&&!f.eof()&&!f.fail()&&!f.bad())
    {
E 10
	    f.getline(line,sizeof(line));
	    sscanf(line,"%d %d %d %d %d %d",&p[0],&p[1],&p[2],&p[3],&p[4],&p[5]);
	    for(j=0; j<6; j++) avg+=p[j]; avg/=6;
	}
	pPV[16+i]->fillPV(avg);
D 10
    }
    
    return;
E 10
I 10
  }

  return;
E 10
}


//--------------------------------------------------------------------------


//  reads cc pretrig info
D 10
void get_cc_p(ifstream &f) {
    
    int i,p[4]={0,0,0,0};
    
E 10
I 10
void
get_cc_p(ifstream &f)
{    
  int i,p[4]={0,0,0,0};
E 10

D 10
    // skip some lines, then read
D 5
    for(i=0; i<3; i++)f.getline(line,sizeof(line));
E 5
I 5
    for(i=0; i<5; i++)f.getline(line,sizeof(line));
E 5
    if(f.is_open()&&f.good()&&!f.eof()&&!f.fail()&&!f.bad()) {
E 10
I 10
  // skip some lines, then read
  for(i=0; i<5; i++)f.getline(line,sizeof(line));
  if(f.is_open()&&f.good()&&!f.eof()&&!f.fail()&&!f.bad())
  {
E 10
	f.getline(line,sizeof(line));
	sscanf(line,"%d %d %d %d",&p[0],&p[1],&p[2],&p[3]);
D 10
    }
    pPV[22]->fillPV((p[0]+p[2])/2);
    pPV[23]->fillPV((p[1]+p[3])/2);
    
    return;
E 10
I 10
  }
  pPV[22]->fillPV((p[0]+p[2])/2);
  pPV[23]->fillPV((p[1]+p[3])/2);

  return;
E 10
}


//--------------------------------------------------------------------------


//  reads sc pretrig info
D 10
void get_sc_p(ifstream &f) {
    
    int i,p[2]={0,0};
    
E 10
I 10
void
get_sc_p(ifstream &f)
{
  int i,p[2]={0,0};
E 10

D 10
    // skip some lines, then read
D 5
    for(i=0; i<3; i++)f.getline(line,sizeof(line));
E 5
I 5
    for(i=0; i<5; i++)f.getline(line,sizeof(line));
E 10
I 10
  // skip some lines, then read
  for(i=0; i<5; i++)f.getline(line,sizeof(line));
E 10
E 5

D 10
    if(f.is_open()&&f.good()&&!f.eof()&&!f.fail()&&!f.bad()) {
E 10
I 10
  if(f.is_open()&&f.good()&&!f.eof()&&!f.fail()&&!f.bad())
  {
E 10
	f.getline(line,sizeof(line));
	sscanf(line,"%d %d",&p[0],&p[1]);
D 10
    }
    pPV[24]->fillPV(p[0]);
    pPV[25]->fillPV(p[1]);
E 10
I 10
  }
  pPV[24]->fillPV(p[0]);
  pPV[25]->fillPV(p[1]);
E 10
    
D 10
    return;
E 10
I 10
  return;
E 10
}


//--------------------------------------------------------------------------


//  reads tdc info
D 10
void get_tdc_info(ifstream &f) {
    
    int i,j,count;
    int t_avg,w_avg,p[5];
    
E 10
I 10
void
get_tdc_info(ifstream &f)
{
  int i,j,count;
  int t_avg,w_avg,p[5];
E 10

D 10
    // ec
    count=0;
    t_avg=0;
    w_avg=0;
    if(find_tag_line(f,"*ec*",line,sizeof(line))==0) {
E 10
I 10
  // ec
  count=0;
  t_avg=0;
  w_avg=0;
  if(find_tag_line(f,"*ec*",line,sizeof(line))==0)
  {
E 10
	f.getline(line,sizeof(line));
D 10
	while(f.is_open()&&f.good()&&!f.eof()&&!f.fail()&&!f.bad()) {
	    f.getline(line,sizeof(line));
	    if(strlen(line)==0)continue;
	    if(strncmp(line,"*",1)==0)break;
	    count++;
	    sscanf(line,"%d %d %d %d %d",&p[0],&p[1],&p[2],&p[3],&p[4]);
	    t_avg+=p[3];
	    w_avg+=p[4];
	}
E 10
I 10
	while(f.is_open()&&f.good()&&!f.eof()&&!f.fail()&&!f.bad())
    {
	  f.getline(line,sizeof(line));
	  if(strlen(line)==0)continue;
	  if(strncmp(line,"*",1)==0)break;
	  count++;
	  sscanf(line,"%d %d %d %d %d",&p[0],&p[1],&p[2],&p[3],&p[4]);
      t_avg+=p[3];
      w_avg+=p[4];
E 10
    }
D 10
    pPV[26]->fillPV((count!=0)?t_avg/count:0);
    pPV[27]->fillPV((count!=0)?w_avg/count:0);
E 10
I 10
  }
  pPV[26]->fillPV((count!=0)?t_avg/count:0);
  pPV[27]->fillPV((count!=0)?w_avg/count:0);
E 10
    

D 10
    // cc
    count=0;
    t_avg=0;
    w_avg=0;
    if(find_tag_line(f,"*cc*",line,sizeof(line))==0) {
E 10
I 10
  // cc
  count=0;
  t_avg=0;
  w_avg=0;
  if(find_tag_line(f,"*cc*",line,sizeof(line))==0)
  {
E 10
	f.getline(line,sizeof(line));
D 10
	while(f.is_open()&&f.good()&&!f.eof()&&!f.fail()&&!f.bad()) {
	    f.getline(line,sizeof(line));
	    if(strlen(line)==0)continue;
	    if(strncmp(line,"*",1)==0)break;
	    count++;
	    sscanf(line,"%d %d %d %d %d",&p[0],&p[1],&p[2],&p[3],&p[4]);
	    t_avg+=p[3];
	    w_avg+=p[4];
E 10
I 10
	while(f.is_open()&&f.good()&&!f.eof()&&!f.fail()&&!f.bad())
    {
	  f.getline(line,sizeof(line));
	  if(strlen(line)==0)continue;
      if(strncmp(line,"*",1)==0)break;
      count++;
      sscanf(line,"%d %d %d %d %d",&p[0],&p[1],&p[2],&p[3],&p[4]);
      t_avg+=p[3];
      w_avg+=p[4];
E 10
	}
D 10
    }
    pPV[28]->fillPV((count!=0)?t_avg/count:0);
    pPV[29]->fillPV((count!=0)?w_avg/count:0);
E 10
I 10
  }
  pPV[28]->fillPV((count!=0)?t_avg/count:0);
  pPV[29]->fillPV((count!=0)?w_avg/count:0);
E 10

    
D 10
    // sc
    count=0;
    t_avg=0;
    w_avg=0;
    if(find_tag_line(f,"*sc*",line,sizeof(line))==0) {
E 10
I 10
  // sc
  count=0;
  t_avg=0;
  w_avg=0;
  if(find_tag_line(f,"*sc*",line,sizeof(line))==0)
  {
E 10
	f.getline(line,sizeof(line));
D 10
	while(f.is_open()&&f.good()&&!f.eof()&&!f.fail()&&!f.bad()) {
	    f.getline(line,sizeof(line));
	    if(strlen(line)==0)continue;
	    if(strncmp(line,"*",1)==0)break;
	    count++;
	    sscanf(line,"%d %d %d %d %d",&p[0],&p[1],&p[2],&p[3],&p[4]);
	    t_avg+=p[3];
	    w_avg+=p[4];
E 10
I 10
	while(f.is_open()&&f.good()&&!f.eof()&&!f.fail()&&!f.bad())
    {
	  f.getline(line,sizeof(line));
	  if(strlen(line)==0)continue;
	  if(strncmp(line,"*",1)==0)break;
	  count++;
	  sscanf(line,"%d %d %d %d %d",&p[0],&p[1],&p[2],&p[3],&p[4]);
	  t_avg+=p[3];
	  w_avg+=p[4];
E 10
	}
D 10
    }
    pPV[30]->fillPV((count!=0)?t_avg/count:0);
    pPV[31]->fillPV((count!=0)?w_avg/count:0);
    
E 10
I 10
  }
  pPV[30]->fillPV((count!=0)?t_avg/count:0);
  pPV[31]->fillPV((count!=0)?w_avg/count:0);
E 10

D 10
    return;
E 10
I 10
  return;
E 10
}


//--------------------------------------------------------------------------


I 3
//  archives epics alarm data
D 10
void *archive_thread(void *param) {

E 10
I 10
void *
archive_thread(void *param)
{
E 10
  int i;
  int l=sizeof(pvNames)/sizeof(char*);

D 10
  while(done==0) {
E 10
I 10
  while(done==0)
  {
E 10
    sleep(archive_sleep_time);

    sprintf(temp,"%s/%s",clon_parms,archive_file);
    if(debug!=0)cout << "archiving to " << temp << endl;
    ofstream file(temp);

    file << l << endl;
    for(i=0; i<l; i++) file << pPV[i]->myAlarm << " ";    file << endl;
    for(i=0; i<l; i++) file << pPV[i]->myHIHI << " ";     file << endl;
    for(i=0; i<l; i++) file << pPV[i]->myLOLO << " ";     file << endl;
    for(i=0; i<l; i++) file << pPV[i]->myHIGH << " ";     file << endl;
    for(i=0; i<l; i++) file << pPV[i]->myLOW  << " ";     file << endl;
    for(i=0; i<l; i++) file << pPV[i]->myHOPR << " ";     file << endl;
    for(i=0; i<l; i++) file << pPV[i]->myLOPR << " ";     file << endl;
    for(i=0; i<l; i++) file << pPV[i]->myDRVH << " ";     file << endl;
    for(i=0; i<l; i++) file << pPV[i]->myDRVL << " ";     file << endl;
    
    file.close();
  }  

  return((void*)NULL);
}


//--------------------------------------------------------------------------


E 3
D 10
//  receives online info from other programs and dispatches to appropriate callbacks
void *ipc_thread(void *param) {

  while(done==0) {
E 10
I 10
//  receives online info from other programs and dispatches
//  to appropriate callbacks
void *
ipc_thread(void *param)
{
  while(done==0)
  {
E 10
    server.MainLoop((double)ipc_pend_time);
  }  
  return((void*)NULL);
}


//--------------------------------------------------------------------------


D 10
void epics_server_callback(T_IPC_CONN conn,
			   T_IPC_CONN_PROCESS_CB_DATA data,
			   T_CB_ARG arg) {
  
E 10
I 10
void
epics_server_callback(T_IPC_CONN conn,
                      T_IPC_CONN_PROCESS_CB_DATA data,
                      T_CB_ARG arg)
{
E 10
  long l;
  long pre[8];
  long long ll;
  double d;
  T_STR s;
  T_INT4 *trgd;
  T_INT4 len;

  TipcMsg msg(data->msg);
  

  callback_count++;


  // run number
  msg.Current(0);  
D 10
  msg >> l; 
E 10
I 10
  msg >> l;
  //printf("clas_epics_server: run1=%d\n",(int)l);
E 10
  pPV[0]->fillPV((int)l);
  run_number=l;


  // event count (ER)
  msg.Current(10);
  msg >> l;
  pPV[1]->fillPV((int)l);


  // event rate (ER)
  msg.Current(11);
  msg >> d;
  pPV[2]->fillPV((int)d);


  // livetime
  msg.Current(14);
  msg.Next(&trgd,&len);
  d=(trgd[19]>0)?(((double)trgd[35])/trgd[19]):0.0;
  pPV[3]->fillPV((int)(100.0*d));
  

  // csr and state
  msg.Current(1);
  msg >> ll;
  pPV[4]->fillPV((unsigned int)(ll&0xffffffff));
  pPV[5]->fillPV((unsigned int)(ll&0x1));


  // trig enable
  msg.Current(4);
  msg >> ll;
  pPV[6]->fillPV((unsigned int)((ll>>1)&0xfff));


  // roc enable
  msg.Current(6);
  msg >> ll;
  pPV[7]->fillPV((unsigned int)(ll&0xffffffff));


I 6
  // *** causing crashes 3-Mar-2006 ejw ***
E 6
  // 8 prescales
D 6
  msg.Current(3);
  msg >> s; 
  sscanf(s,"%ld %ld %ld %ld %ld %ld %ld %ld",
	 &pre[0],&pre[1],&pre[2],&pre[3],&pre[4],&pre[5],&pre[6],&pre[7]);
  for(long i=0; i<8; i++) pPV[8+i]->fillPV((int)pre[i]);
E 6
I 6
  //  msg.Current(3);
  //  msg >> s; 
  //  sscanf(s,"%ld %ld %ld %ld %ld %ld %ld %ld",
  //	 &pre[0],&pre[1],&pre[2],&pre[3],&pre[4],&pre[5],&pre[6],&pre[7]);
  //  for(long i=0; i<8; i++) pPV[8+i]->fillPV((int)pre[i]);
E 6


  // data rate (ER)
  msg.Current(12);
  msg >> d;
  pPV[32]->fillPV((int)d);


  return;
}


//------------------------------------------------------------------


D 10
void status_poll_callback(T_IPC_MSG msg) {

  
E 10
I 10
void
status_poll_callback(T_IPC_MSG msg)
{  
E 10
  TipcMsgAppendStr(msg,(char*)"number of epics channels served");
  TipcMsgAppendInt4(msg,sizeof(pvNames)/sizeof(char*));
  
  TipcMsgAppendStr(msg,(char*)"number of smartsockets callbacks");
  TipcMsgAppendInt4(msg,callback_count);
D 10
  
E 10

  return;
}


//-------------------------------------------------------------------


D 10
void quit_callback(int sig) {

E 10
I 10
void
quit_callback(int sig)
{
E 10
  done=1;

  return;
}


D 9
//-------------------------------------------------------------------


void init_tcl() {

  // link c and Tcl variables
  Tcl_LinkVar(interp,(char*)"application",    	   (char *)&application,      	TCL_LINK_STRING);
  Tcl_LinkVar(interp,(char*)"unique_id",     	   (char *)&unique_id,          TCL_LINK_STRING);
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


E 9
//--------------------------------------------------------------------------


D 10
void decode_command_line(int argc, char**argv) {
E 10
I 10
void
decode_command_line(int argc, char**argv)
{
E 10

  const char *help = "\nusage:\n\n epics_server [-a application] [-u unique_id]\n"
    "              [-ca ca_pend_time] [-ipc ipc_pend_time] [-data data_sleep_time]\n"
D 3
    "              [-t init_tcl_script] [-debug]\n";
E 3
I 3
D 9
    "              [-archive archive_sleep_time] [-t init_tcl_script] [-debug]\n";
E 9
I 9
    "              [-archive archive_sleep_time] [-debug]\n";
E 9
E 3


  // loop over all arguments, except the 1st (which is program name)
  int i=1;
  while(i<argc) {
    if(strncasecmp(argv[i],"-h",2)==0) {
      cout << help << endl;
      exit(EXIT_SUCCESS);
    }
    else if (strncasecmp(argv[i],"-debug",6)==0) {
      debug=1;
      i=i+1;
    }
    else if (strncasecmp(argv[i],"-ca",3)==0) {
      ca_pend_time=atoi(argv[i+1]);
      i=i+2;
    }
    else if (strncasecmp(argv[i],"-ipc",4)==0) {
      ipc_pend_time=atoi(argv[i+1]);
      i=i+2;
    }
D 3
    else if (strncasecmp(argv[i],"-data",4)==0) {
E 3
I 3
    else if (strncasecmp(argv[i],"-data",5)==0) {
E 3
      data_sleep_time=atoi(argv[i+1]);
      i=i+2;
    }
I 3
    else if (strncasecmp(argv[i],"-archive",8)==0) {
      archive_sleep_time=atoi(argv[i+1]);
      i=i+2;
    }
E 3
    else if (strncasecmp(argv[i],"-a",2)==0) {
      application=strdup(argv[i+1]);
      i=i+2;
    }
    else if (strncasecmp(argv[i],"-u",2)==0) {
      unique_id=strdup(argv[i+1]);
      i=i+2;
    }
D 9
    else if (strncasecmp(argv[i],"-t",2)==0) {
      init_tcl_script=strdup(argv[i+1]);
      i=i+2;
    }
E 9
    else if (strncasecmp(argv[i],"-",1)==0) {
      cout << "Unknown command line arg: " << argv[i] << argv[i+1] << endl << endl;
      i=i+1;
    }
  }

  return;
}

D 9
  
//----------------------------------------------------------------


int tcl_help(ClientData clientdata, Tcl_Interp *interp,
		int argc, char **argv) {

    const char *help =
    "\nTcl commands available in the epics_server program:\n\n"
    " help                  print this message\n"
    " stop                  stop program\n"
    " quit                  stop program\n"
    " exit                  stop program\n"
    "\n\n Type command that require args with NO args for more information\n"
    "\n";

    Tcl_SetResult(interp,(char*)help,TCL_STATIC);

  return (TCL_OK);

}


E 9
//---------------------------------------------------------------------
D 9


int tcl_quit(ClientData clientdata, Tcl_Interp *interp,
	     int argc, char **argv) {
  
  done=1;
  
  return (TCL_OK);
}


//---------------------------------------------------------------------
E 9


E 1
