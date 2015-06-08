//  clas_epics_server

// CLAS portable ca server cross-posts assorted DAQ/Online info 
//   from ipc and files as epics channels 

//  ejw, 3-jul-2003



// for posix
#define _POSIX_SOURCE_ 1
#define __EXTENSIONS__


// for ca 
#include <casdef.h>
#include <fdManager.h>
#include <gdd.h>


// epics server class defs
#include <epics_server.h>


//  epics channel names, etc.
#include <clas_epics_server.h>


// for smartsockets
#include <rtworks/cxxipc.hxx>


// CLAS ipc
#include <clas_ipc_prototypes.h>


// misc
#include <pthread.h>
#ifdef SunOS
#include <thread.h>
#endif
#include <iostream.h>
#include <fstream.h>
#include <iomanip.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>


// misc variables
static char *application       	    = (char*)"clastest";
static char *unique_id         	    = (char*)"clas_epics_server";
static const char *ec_pretrig_file  = "diman/mondat/mon_ec_p.txt";
static const char *cc_pretrig_file  = "diman/mondat/mon_cc_p.txt";
static const char *sc_pretrig_file  = "diman/mondat/mon_sc_p.txt";
static const char *tdc_file         = "discr/archive/discr_clasprod";
static char *clon_parms             = getenv("CLON_PARMS");
static int ca_pend_time             = 5;
static int data_sleep_time          = 30;
static int archive_sleep_time       = 60;
static char *archive_file           = (char*)"epics/clas_epics_server.dat";
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
void read_archive();
void read_pv_info(ifstream &i, int *a, int len);
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
void *archive_thread(void *param);
void quit_callback(int sig);
void status_poll_callback(T_IPC_MSG msg);
int insert_msg(const char *name, const char *facility, const char *process, const char *msgclass, 
	       int severity, const char *status, int code, const char *text);
}


// ref to IPC server (connection created later)
TipcSrv &server=TipcSrv::Instance();


//--------------------------------------------------------------------------
//--------------------------------------------------------------------------


class myServer : public caServer {
  

public:
    
  pvExistReturn pvExistTest(const casCtx &ctx, const char *pPVName)
  {
    
    int PVLen = strcspn(pPVName,".");

    for(int i=0; i<sizeof(pvNames)/sizeof(char*); i++)
    {
      if((PVLen==strlen(pvNames[i]))&&(strncasecmp(pPVName,pvNames[i],PVLen)==0))
	 return pverExistsHere;
    }
    return pverDoesNotExistHere;
  }
  
  
//---------------------------------------------------


  pvCreateReturn createPV(const casCtx &ctx, const char *pPVName)
  {
    if(debug!=0)cout << "createPV for " << pPVName << endl;

    int PVNameLen      = strlen(pPVName);
    int PVLen          = strcspn(pPVName,".");
    const char *pattr  = pPVName+PVLen+1;
    int lattr          = PVNameLen-PVLen-1;

    for(int i=0; i<sizeof(pvNames)/sizeof(char*); i++)
    {
	  if((PVLen==strlen(pvNames[i]))&&(strncasecmp(pPVName,pvNames[i],PVLen)==0))
      {
	    if( (PVNameLen==PVLen) || ((lattr==3)&&(strncasecmp(pattr,"VAL",3)==0)) )
        {
		  return(*pPV[i]);
	    }
        else
        {
		  return(*(new myAttrPV(pPV[i],pattr,lattr)));
	    }
	  }
    }
    return(S_casApp_pvNotFound);
  }
  
    
//---------------------------------------------------
    
    
    ~myServer()
    {
      if(debug!=0)cout << "myServer destructor" << endl;
      return;
    }
    
};


//---------------------------------------------------------------------------
//---------------------------------------------------------------------------


main(int argc,char **argv)
{
  int status;
  pthread_t t1,t2,t3;


  // synch with stdio
  ios::sync_with_stdio();


  // decode command line...
  decode_command_line(argc,argv);

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


  // set epics server debug flag
  setDebug(debug);


  // read pv alarm info from archive file
  read_archive();


  // create array of pv objects
  for(int i=0; i<sizeof(pvNames)/sizeof(char*); i++) {
      pPV[i] = new myPV(pvNames[i],pvTypes[i],pvUnits[i],pvAlarm[i],pvHIHI[i],pvLOLO[i],
			pvHIGH[i],pvLOW[i],pvHOPR[i],pvLOPR[i],pvDRVH[i],pvDRVL[i],pvPREC[i]);
  }


  // create ca server
  myServer *cas = new myServer();


  // launch threads
#ifdef SunOS
  thr_setconcurrency(thr_getconcurrency()+4);
#endif
  pthread_create(&t1,NULL,data_thread,(void*)NULL);
  pthread_create(&t2,NULL,ipc_thread,(void*)NULL);
  pthread_create(&t3,NULL,archive_thread,(void*)NULL);


  // post startup message
  sprintf(temp,"Process startup:    epics_server starting in %s",application);
  status=insert_msg("epics_server","online",unique_id,"status",0,"START",0,temp);
  

  // flush output to log files, etc
  fflush(NULL);


  //  ca server loop
  while(done==0)
  {

    fileDescriptorManager.process((double)ca_pend_time);

    // check for updates and satisfy monitorOn requests
    for(int i=0; i<sizeof(pvNames)/sizeof(char*); i++)
    {
      if((pPV[i]->myMonitor!=0)&&(pPV[i]->myUpdate!=0))
      {
	    if(debug!=0)cout << "(monitorOn response for " << pvNames[i] << ")" << endl;
	    gdd *value = new gdd();

	    caServer *pCAS = pPV[i]->getCAS();
	    /*sergey casEventMask select(pCAS->valueEventMask | pCAS->logEventMask);*/
	    casEventMask select( pCAS->valueEventMask() | pCAS->logEventMask());

	    pPV[i]->getVAL(*value);
	    pPV[i]->postEvent(select,*value);
	    pPV[i]->myUpdate=0;
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


//  reads archive file and fills pvXXXX arrays prior to pv initialization
void
read_archive()
{

  int len;

  sprintf(temp,"%s/%s",clon_parms,archive_file);
  if(debug!=0)cout << "reading archive file " << temp << endl;
  ifstream file(temp);
  if(!file.is_open())
  {
    cerr << "?Unable to open archive file: " << temp << endl;
    return;
  }
    

  file >> len;
  if(len!=sizeof(pvNames)/sizeof(char*))
  {
    cerr << "?Unable to read archive, lengths disagree, program/archive are: " 
	   << sizeof(pvNames)/sizeof(char*) << "/" << len << endl;
    file.close();
    return;
  }


  read_pv_info(file,pvAlarm,sizeof(pvNames)/sizeof(char*)); 
  read_pv_info(file,pvHIHI,sizeof(pvNames)/sizeof(char*)); 
  read_pv_info(file,pvLOLO,sizeof(pvNames)/sizeof(char*)); 
  read_pv_info(file,pvHIGH,sizeof(pvNames)/sizeof(char*)); 
  read_pv_info(file,pvLOW,sizeof(pvNames)/sizeof(char*)); 
  read_pv_info(file,pvHOPR,sizeof(pvNames)/sizeof(char*)); 
  read_pv_info(file,pvLOPR,sizeof(pvNames)/sizeof(char*)); 
  read_pv_info(file,pvDRVH,sizeof(pvNames)/sizeof(char*)); 
  read_pv_info(file,pvDRVL,sizeof(pvNames)/sizeof(char*)); 

  file.close();
}


//--------------------------------------------------------------------------


void
read_pv_info(ifstream &i, int *a, int len)
{
  for(int j=0; j<len; j++) i >> a[j];
}


//--------------------------------------------------------------------------


//  periodically retrieves online info from files
void *
data_thread(void *param)
{
  ifstream file;

  while(done==0)
  {

    // ec pretrig
    sprintf(filename,"%s/%s",clon_parms,ec_pretrig_file);
    file.open(filename);
    if(!file.is_open())
    {
	  cerr << "?unable to open " << filename << endl;
    }
    else
    {
	  get_ec_p(file);
	  file.close();
    }


    // cc pretrig
    sprintf(filename,"%s/%s",clon_parms,cc_pretrig_file);
    file.open(filename);
    if(!file.is_open())
    {
	  cerr << "?unable to open " << filename << endl;
    }
    else
    {
	  get_cc_p(file);
	  file.close();
    }


    // sc pretrig
    sprintf(filename,"%s/%s",clon_parms,sc_pretrig_file);
    file.open(filename);
    if(!file.is_open())
    {
	  cerr << "?unable to open " << filename << endl;
    }
    else
    {
	  get_sc_p(file);
	  file.close();
    }


    // tdc info
    if(run_number>0)
    {
	  sprintf(filename,"%s/%s_%06d.txt",clon_parms,tdc_file,run_number);
	  file.open(filename);
	  if(!file.is_open())
      {
	    cerr << "?unable to open " << filename << endl;
	  }
      else
      {
	    get_tdc_info(file);
	    file.close();
	  }
    }


    // until next time...
    sleep(data_sleep_time);
  }  

  return((void*)NULL);
}


//--------------------------------------------------------------------------


//  reads ec pretrig info
void
get_ec_p(ifstream &f)
{
  int i,j;
  int avg,p[6];
    

  // skip some lines
  for(i=0; i<6; i++)f.getline(line,sizeof(line));

  // inner,outer,total for hi,lo
  for(i=0; i<6; i++)
  {
	avg=0;
	if(f.is_open()&&f.good()&&!f.eof()&&!f.fail()&&!f.bad())
    {
	    f.getline(line,sizeof(line));
	    sscanf(line,"%d %d %d %d %d %d",&p[0],&p[1],&p[2],&p[3],&p[4],&p[5]);
	    for(j=0; j<6; j++) avg+=p[j]; avg/=6;
	}
	pPV[16+i]->fillPV(avg);
  }

  return;
}


//--------------------------------------------------------------------------


//  reads cc pretrig info
void
get_cc_p(ifstream &f)
{    
  int i,p[4]={0,0,0,0};

  // skip some lines, then read
  for(i=0; i<5; i++)f.getline(line,sizeof(line));
  if(f.is_open()&&f.good()&&!f.eof()&&!f.fail()&&!f.bad())
  {
	f.getline(line,sizeof(line));
	sscanf(line,"%d %d %d %d",&p[0],&p[1],&p[2],&p[3]);
  }
  pPV[22]->fillPV((p[0]+p[2])/2);
  pPV[23]->fillPV((p[1]+p[3])/2);

  return;
}


//--------------------------------------------------------------------------


//  reads sc pretrig info
void
get_sc_p(ifstream &f)
{
  int i,p[2]={0,0};

  // skip some lines, then read
  for(i=0; i<5; i++)f.getline(line,sizeof(line));

  if(f.is_open()&&f.good()&&!f.eof()&&!f.fail()&&!f.bad())
  {
	f.getline(line,sizeof(line));
	sscanf(line,"%d %d",&p[0],&p[1]);
  }
  pPV[24]->fillPV(p[0]);
  pPV[25]->fillPV(p[1]);
    
  return;
}


//--------------------------------------------------------------------------


//  reads tdc info
void
get_tdc_info(ifstream &f)
{
  int i,j,count;
  int t_avg,w_avg,p[5];

  // ec
  count=0;
  t_avg=0;
  w_avg=0;
  if(find_tag_line(f,"*ec*",line,sizeof(line))==0)
  {
	f.getline(line,sizeof(line));
	while(f.is_open()&&f.good()&&!f.eof()&&!f.fail()&&!f.bad())
    {
	  f.getline(line,sizeof(line));
	  if(strlen(line)==0)continue;
	  if(strncmp(line,"*",1)==0)break;
	  count++;
	  sscanf(line,"%d %d %d %d %d",&p[0],&p[1],&p[2],&p[3],&p[4]);
      t_avg+=p[3];
      w_avg+=p[4];
    }
  }
  pPV[26]->fillPV((count!=0)?t_avg/count:0);
  pPV[27]->fillPV((count!=0)?w_avg/count:0);
    

  // cc
  count=0;
  t_avg=0;
  w_avg=0;
  if(find_tag_line(f,"*cc*",line,sizeof(line))==0)
  {
	f.getline(line,sizeof(line));
	while(f.is_open()&&f.good()&&!f.eof()&&!f.fail()&&!f.bad())
    {
	  f.getline(line,sizeof(line));
	  if(strlen(line)==0)continue;
      if(strncmp(line,"*",1)==0)break;
      count++;
      sscanf(line,"%d %d %d %d %d",&p[0],&p[1],&p[2],&p[3],&p[4]);
      t_avg+=p[3];
      w_avg+=p[4];
	}
  }
  pPV[28]->fillPV((count!=0)?t_avg/count:0);
  pPV[29]->fillPV((count!=0)?w_avg/count:0);

    
  // sc
  count=0;
  t_avg=0;
  w_avg=0;
  if(find_tag_line(f,"*sc*",line,sizeof(line))==0)
  {
	f.getline(line,sizeof(line));
	while(f.is_open()&&f.good()&&!f.eof()&&!f.fail()&&!f.bad())
    {
	  f.getline(line,sizeof(line));
	  if(strlen(line)==0)continue;
	  if(strncmp(line,"*",1)==0)break;
	  count++;
	  sscanf(line,"%d %d %d %d %d",&p[0],&p[1],&p[2],&p[3],&p[4]);
	  t_avg+=p[3];
	  w_avg+=p[4];
	}
  }
  pPV[30]->fillPV((count!=0)?t_avg/count:0);
  pPV[31]->fillPV((count!=0)?w_avg/count:0);

  return;
}


//--------------------------------------------------------------------------


//  archives epics alarm data
void *
archive_thread(void *param)
{
  int i;
  int l=sizeof(pvNames)/sizeof(char*);

  while(done==0)
  {
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


//  receives online info from other programs and dispatches
//  to appropriate callbacks
void *
ipc_thread(void *param)
{
  while(done==0)
  {
    server.MainLoop((double)ipc_pend_time);
  }  
  return((void*)NULL);
}


//--------------------------------------------------------------------------


void
epics_server_callback(T_IPC_CONN conn,
                      T_IPC_CONN_PROCESS_CB_DATA data,
                      T_CB_ARG arg)
{
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
  msg >> l;
  //printf("clas_epics_server: run1=%d\n",(int)l);
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


  // *** causing crashes 3-Mar-2006 ejw ***
  // 8 prescales
  //  msg.Current(3);
  //  msg >> s; 
  //  sscanf(s,"%ld %ld %ld %ld %ld %ld %ld %ld",
  //	 &pre[0],&pre[1],&pre[2],&pre[3],&pre[4],&pre[5],&pre[6],&pre[7]);
  //  for(long i=0; i<8; i++) pPV[8+i]->fillPV((int)pre[i]);


  // data rate (ER)
  msg.Current(12);
  msg >> d;
  pPV[32]->fillPV((int)d);


  return;
}


//------------------------------------------------------------------


void
status_poll_callback(T_IPC_MSG msg)
{  
  TipcMsgAppendStr(msg,(char*)"number of epics channels served");
  TipcMsgAppendInt4(msg,sizeof(pvNames)/sizeof(char*));
  
  TipcMsgAppendStr(msg,(char*)"number of smartsockets callbacks");
  TipcMsgAppendInt4(msg,callback_count);

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


void
decode_command_line(int argc, char**argv)
{

  const char *help = "\nusage:\n\n epics_server [-a application] [-u unique_id]\n"
    "              [-ca ca_pend_time] [-ipc ipc_pend_time] [-data data_sleep_time]\n"
    "              [-archive archive_sleep_time] [-debug]\n";


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
    else if (strncasecmp(argv[i],"-data",5)==0) {
      data_sleep_time=atoi(argv[i+1]);
      i=i+2;
    }
    else if (strncasecmp(argv[i],"-archive",8)==0) {
      archive_sleep_time=atoi(argv[i+1]);
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
    else if (strncasecmp(argv[i],"-",1)==0) {
      cout << "Unknown command line arg: " << argv[i] << argv[i+1] << endl << endl;
      i=i+1;
    }
  }

  return;
}

//---------------------------------------------------------------------


