h59418
s 00002/00001/00267
d D 1.14 07/10/11 21:45:17 boiarino 15 14
c *** empty log message ***
e
s 00028/00024/00240
d D 1.13 02/10/02 15:04:45 wolin 14 13
c Added -unixtime, other minor mods
e
s 00009/00000/00255
d D 1.12 02/09/19 16:13:25 wolin 13 12
c Added thread pkg init
e
s 00000/00004/00255
d D 1.11 02/08/13 13:26:41 wolin 12 11
c Removed TutCommandParseStdLicense()
c 
e
s 00016/00016/00243
d D 1.10 01/04/02 14:56:51 wolin 11 10
c New compiler...some problems in smartsockets prototypes
e
s 00001/00001/00258
d D 1.9 01/04/02 14:50:44 wolin 10 9
c Bombproofed .eof()
e
s 00001/00001/00258
d D 1.8 01/01/03 10:48:25 wolin 9 8
c is_open()
e
s 00049/00012/00210
d D 1.7 00/09/26 14:26:53 wolin 8 7
c Added gmd
e
s 00016/00001/00206
d D 1.6 00/09/26 14:12:22 wolin 7 6
c Added -file option
e
s 00046/00013/00161
d D 1.5 99/12/21 12:53:02 wolin 6 5
c Added i4a,r4a,r8a
e
s 00006/00000/00168
d D 1.4 99/11/15 14:14:44 wolin 5 4
c Added ipc_output_dummy
e
s 00059/00017/00109
d D 1.3 99/11/10 17:05:23 wolin 4 3
c Now handles arbitrary message
e
s 00000/00002/00126
d D 1.2 99/06/22 12:22:07 wolin 3 1
c Removed debug stuff
c 
e
s 00000/00000/00000
d R 1.2 99/06/22 12:08:54 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 ipcutil/s/ipc_msg.cc
e
s 00128/00000/00000
d D 1.1 99/06/22 12:08:53 wolin 1 0
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
//  ipc_msg
//
//  Sends out arbitrary Smartsockets message
//
//  ejw, 22-jun-99


#define _POSIX_SOURCE 1
#define __EXTENSIONS__


#include <rtworks/cxxipc.hxx>
I 7
#include <fstream.h>
D 15
#include <strstream.h>
E 15
I 15
using namespace std;
#include <strstream>
E 15
E 7
#include <iostream.h>
#include <iomanip.h>
I 5
#include <stdio.h>
I 13
D 14
#include <pthread.h>
E 14
I 14
#include <time.h>
E 14
E 13
E 5


D 4
char *msgtype = NULL;
char *msgarg  = NULL;
E 4
D 8
char *app     = "clastest";
I 4
char *msgtype = NULL;
E 4
char *dest    = NULL;
D 4
char *help    = "\nusage:\n\n   ipc_msg [-a project] [-d dest] [-t msgtype] [-arg msgarg]\n\n";
E 4
I 4
char *sender  = NULL;
int debug     = 0;
E 8
I 8
D 11
char *app     	= "clastest";
E 11
I 11
D 14
char *app     	= (char*)"clastest";
E 14
I 14
char *project  	= (char*)"clastest";
E 14
E 11
char *msgtype 	= NULL;
char *dest    	= NULL;
char *sender  	= NULL;
char *uniq_name = NULL;
int gmd       	= 0;
int debug     	= 0;
E 8
int ind;
I 7
char buffer[1024];
E 7
E 4


void decode_command_line(int argc, char **argv);
extern "C" {
void init_msg_types(void);
I 5
void ipc_output_dummy(T_STR str, va_list arg);
E 5
}


//------------------------------------------------------------------------


int main(int argc, char **argv) {
  

I 6
  int i,j,len;


E 6
I 4
  // synch with c i/o
  ios::sync_with_stdio();


E 4
  // decode command line args
  decode_command_line(argc,argv);
I 5


D 6
  // inhibit output
  TutSetOutputFunc(ipc_output_dummy);
E 5


E 6
D 4
  // default destination is same as application, if application not specified
  if(dest==NULL)dest=strdup(app);
E 4
I 4
  // check args
D 11
  if(dest==NULL)TutWarning("?no destination\n\n"),exit(0);
D 6
  if(msgtype==NULL)TutWarning("?no msgtype\n\n"),exit(0);
E 6
I 6
  if(msgtype==NULL)TutWarning("?no msgtype\n\n") ,exit(0);
E 6
  if(sender==NULL)sender="ipc_msg";
I 8
  if((gmd>0)&&(uniq_name==NULL))TutWarning("?no unique name for gmd\n\n") ,exit(0);
E 11
I 11
D 14
  if(dest==NULL)TutWarning((T_STR)"?no destination\n\n"),exit(0);
  if(msgtype==NULL)TutWarning((T_STR)"?no msgtype\n\n") ,exit(0);
E 14
I 14
  if(dest==NULL){TutWarning((T_STR)"?no destination\n\n"); exit(EXIT_FAILURE);}
  if(msgtype==NULL){TutWarning((T_STR)"?no msgtype\n\n"); exit(EXIT_FAILURE);}
E 14
  if(sender==NULL)sender=(char*)"ipc_msg";
D 14
  if((gmd>0)&&(uniq_name==NULL))TutWarning((T_STR)"?no unique name for gmd\n\n") ,exit(0);
E 14
I 14
  if((gmd>0)&&(uniq_name==NULL)){TutWarning((T_STR)"?no unique name for gmd\n\n"); exit(EXIT_FAILURE);}
E 14
E 11
E 8
E 4


I 6
  // inhibit output
  if(debug==0)TutSetOutputFunc(ipc_output_dummy);


I 13
D 14
  // init ipc thread package
  thr_setconcurrency(thr_getconcurrency()+4);
  if(!TipcInitThreads()) {
    cerr << "Unable to init IPC thread package" << endl;
    exit(EXIT_FAILURE);
  }


E 13
E 6
D 12
  // read Smartsockets license file
  TutCommandParseStdLicense();


E 12
  // set application
E 14
I 14
  // set projext
E 14
D 8
  T_OPTION opt=TutOptionLookup("Application");
D 6
  if(!TutOptionSetEnum(opt,app)){TutOut("?unable to set application\n");}
E 6
I 6
  if(!TutOptionSetEnum(opt,app)){cerr << "?unable to set application\n" << endl;}
E 8
I 8
D 11
  T_OPTION opt=TutOptionLookup("Project");
E 11
I 11
  T_OPTION opt=TutOptionLookup((T_STR)"Project");
E 11
D 14
  if(!TutOptionSetEnum(opt,app)){cerr << "?unable to set project\n" << endl;}
E 14
I 14
  if(!TutOptionSetEnum(opt,project)){cerr << "?unable to set project\n" << endl; exit(EXIT_FAILURE);}
E 14


  // setup gmd if requested
  if(gmd>0) {
D 11
    opt=TutOptionLookup("Unique_Subject");
E 11
I 11
    opt=TutOptionLookup((T_STR)"Unique_Subject");
E 11
    TutOptionSetEnum(opt,uniq_name);
D 11
    opt=TutOptionLookup("Server_Disconnect_Mode");
    TutOptionSetEnum(opt,"warm");
    opt=TutOptionLookup("Server_Delivery_Timeout");
E 11
I 11
    opt=TutOptionLookup((T_STR)"Server_Disconnect_Mode");
    TutOptionSetEnum(opt,(T_STR)"warm");
    opt=TutOptionLookup((T_STR)"Server_Delivery_Timeout");
E 11
    TutOptionSetNum(opt,0.0);
  }
E 8
E 6


  // connect to server 
  TipcSrv &server=TipcSrv::InstanceCreate(T_IPC_SRV_CONN_FULL);


I 8
  // destroy gmd failure callbacks, if gmd requested
  if(gmd>0) {
    T_IPC_MT mt=TipcMtLookupByNum(T_MT_GMD_FAILURE);
    T_CB cb=TipcSrvProcessCbLookup(mt,TipcCbConnProcessGmdFailure, NULL);
    TutCbDestroy(cb);
    cb=TipcSrvProcessCbLookup(mt,TipcCbSrvProcessGmdFailure, NULL);
    TutCbDestroy(cb);
  }


E 8
  // init standard msg types
  init_msg_types();


  // create message
  TipcMsg message(msgtype);
  if(!message) {
    cerr << "unable to create message type: " << msgtype << endl;
D 14
    exit(0);
E 14
I 14
    exit(EXIT_FAILURE);
E 14
  }

I 4

  // sender and destination
  message.Sender(sender);
E 4
  message.Dest(dest);
D 4
  message << msgarg;
  
E 4
I 4


I 8
  // set delivery mode for gmd
  if(gmd>0) message.DeliveryMode(T_IPC_DELIVERY_ALL);


E 8
  // append args
D 6
  for(int i=ind; i<argc; i+=2) {
    if(strncasecmp(argv[i],"-i4",3)==0) {
E 6
I 6
D 14
  for(i=ind; i<argc; i+=2) {
E 14
I 14
  for(i=ind; i<argc; i++) {
E 14

D 14
    if(strncasecmp(argv[i],"-i4a",4)==0) {
E 14
I 14
    if(strncasecmp(argv[i],"-unixtime",9)==0) {
      message << (T_INT4)time(NULL);

    } else if(strncasecmp(argv[i],"-i4a",4)==0) {
E 14
      len=atoi(argv[i+1]);
      int *i4a = new int[len];
      for(j=0; j<len; j++) i4a[j]=atoi(argv[i+j+2]);
      message << SetSize(len) << (T_INT4*)i4a;
      delete [] i4a;
      i+=j;
I 14
      i++;
E 14

    } else if(strncasecmp(argv[i],"-r4a",4)==0) {
      len=atoi(argv[i+1]);
      float *r4a = new float[len];
      for(j=0; j<len; j++) r4a[j]=atof(argv[i+j+2]);
      message << SetSize(len) << (T_REAL4*)r4a;
      delete [] r4a;
      i+=j;
I 14
      i++;
E 14

    } else if(strncasecmp(argv[i],"-r8a",4)==0) {
      len=atoi(argv[i+1]);
      double *r8a = new double [len];
      for(j=0; j<len; j++) r8a[j]=(double)atof(argv[i+j+2]);
      message << SetSize(len) << (T_REAL8*)r8a;
      delete [] r8a;
      i+=j;
I 14
      i++;
E 14

    } else if (strncasecmp(argv[i],"-i4",3)==0) {
E 6
      message << (T_INT4)atoi(argv[i+1]);
I 14
      i++;
E 14
I 6

E 6
    } else if (strncasecmp(argv[i],"-r4",3)==0) {
      message << (T_REAL4)atof(argv[i+1]);
I 14
      i++;
E 14
I 6

E 6
    } else if (strncasecmp(argv[i],"-r8",3)==0) {
      message << (T_REAL8)atof(argv[i+1]);
I 14
      i++;
E 14
I 6

E 6
    } else if (strncasecmp(argv[i],"-str",4)==0) {
      message << argv[i+1];
I 14
      i++;
E 14
I 7

    } else if (strncasecmp(argv[i],"-file",5)==0) {
      ifstream file(argv[i+1]);
D 9
      if(file.bad())TutWarning("?file doesn't exist\n\n"),exit(0);
E 9
I 9
D 11
      if(!file.is_open())TutWarning("?file doesn't exist\n\n"),exit(0);
E 11
I 11
D 14
      if(!file.is_open())TutWarning((T_STR)"?file doesn't exist\n\n"),exit(0);
E 14
I 14
      if(!file.is_open())TutWarning((T_STR)"?file doesn't exist\n\n"),exit(EXIT_FAILURE);
E 14
E 11
E 9
      strstream str;
      for(;;) {
D 10
	if(file.eof()) break;
E 10
I 10
D 11
	if(!file.is_open()||!file.good()||file.eof()||file.fail()||file.bad()) break
E 11
I 11
	if(!file.is_open()||!file.good()||file.eof()||file.fail()||file.bad()) break;
E 11
E 10
	file.getline(buffer,sizeof(buffer));
	str << buffer << endl;
      }
      message << str.str();
      file.close();
I 14
      i++;
E 14
E 7
D 6
    }
  }  
E 6
I 6
    }  
  }

E 6


  //  dump message
  if(debug==1) {
    cout << endl;
    message.Print(TutOut);
    cout << endl;
  }

E 4
D 3
  message.Print(TutOut);

E 3

D 8
  // send and flush
E 8
I 8
  // send and flush, wait if gmd
E 8
  server.Send(message,TRUE);
  server.Flush();
I 8
  if(gmd>0)server.MainLoop(T_REAL8(gmd));
E 8

  
  // close connection
  server.Destroy(T_IPC_SRV_CONN_NONE);
D 8

E 8
}


//------------------------------------------------------------------------


void decode_command_line(int argc, char **argv) {

I 4
D 6
  char *help    = "\nusage:\n\n   ipc_msg [-a project] [-sender sender] [-dest dest] [-type msgtype] \n"
    "             [-i4 int4] [-r4 real4] [-r8 real8] [-str \"string\"] [-debug]\n\n";
E 4

E 6
I 6
D 8
  char *help    = "\nusage:\n\n   ipc_msg [-a project] [-sender sender] [-dest dest] [-type msgtype]  [-debug]\n"
D 7
    "             [-i4 int4] [-r4 real4] [-r8 real8] [-str \"string\"] \n"
E 7
I 7
    "             [-i4 int4] [-r4 real4] [-r8 real8] [-str \"string\"] [-file filename]\n"
E 7
    "             [-i4a len i4_array[len]] [-r4a len r4_array[len]] [-r8 len real8[len]]\n\n";
E 8
I 8
D 11
  char *help    = "\nusage:\n\n   ipc_msg [-a project] [-u uniq_name] [-sender sender]\n"
E 11
I 11
  const char *help    = "\nusage:\n\n   ipc_msg [-a project] [-u uniq_name] [-sender sender]\n"
E 11
    "           [-dest dest] [-type msgtype] [-gmd time] [-debug]\n"
    "           [-i4 int4] [-r4 real4] [-r8 real8] [-str \"string\"] [-file filename]\n"
D 14
    "           [-i4a len i4_array[len]] [-r4a len r4_array[len]] [-r8 len real8[len]]\n\n";
E 14
I 14
    "           [-i4a len i4_array[len]] [-r4a len r4_array[len]] [-r8 len real8[len]]\n";
    "           [-unixtime] \n\n";
E 14
E 8
    
E 6
  int i=1;
I 4


D 11
  if(argc<4)TutOut("%s", help),exit(0);
E 11
I 11
D 14
  if(argc<4)TutOut((T_STR)"%s", help),exit(0);
E 14
I 14
  if(argc<4)TutOut((T_STR)"%s", help),exit(EXIT_SUCCESS);
E 14
E 11


E 4
  while(i<argc) {
D 4
    if(strncasecmp(argv[i],"-h",2)==0){
E 4
I 4
    if(strncasecmp(argv[i],"-h",2)==0) {
E 4
D 11
      TutOut("%s", help);
E 11
I 11
      TutOut((T_STR)"%s", help);
E 11
D 14
      exit(0);
E 14
I 14
      exit(EXIT_SUCCESS);
E 14
    }
D 4
    else if (strncasecmp(argv[i],"-arg",4)==0){
      msgarg=strdup(argv[i+1]);
      i=i+2;
E 4
I 4
    else if (strncasecmp(argv[i],"-debug",6)==0) {
      debug=1;
      i=i+1;
E 4
    }
D 4
    else if (strncasecmp(argv[i],"-a",2)==0){
      app=strdup(argv[i+1]);
E 4
I 4
    else if (strncasecmp(argv[i],"-sender",7)==0) {
      sender=strdup(argv[i+1]);
E 4
      i=i+2;
    }
D 4
    else if (strncasecmp(argv[i],"-d",2)==0){
E 4
I 4
    else if (strncasecmp(argv[i],"-dest",5)==0) {
E 4
      dest=strdup(argv[i+1]);
      i=i+2;
    }
D 4
    else if (strncasecmp(argv[i],"-t",2)==0){
E 4
I 4
    else if (strncasecmp(argv[i],"-type",5)==0) {
E 4
      msgtype=strdup(argv[i+1]);
      i=i+2;
    }
I 8
    else if (strncasecmp(argv[i],"-gmd",4)==0) {
      gmd=atoi(argv[i+1]);
      i=i+2;
    }
E 8
I 4
    else if (strncasecmp(argv[i],"-a",2)==0) {
D 14
      app=strdup(argv[i+1]);
E 14
I 14
      project=strdup(argv[i+1]);
E 14
I 8
      i=i+2;
    }
    else if (strncasecmp(argv[i],"-u",2)==0) {
      uniq_name=strdup(argv[i+1]);
E 8
      i=i+2;
    }
E 4
    else if (strncasecmp(argv[i],"-",1)==0) {
D 4
      TutWarning("Unknown command line arg: %s\n\n",argv[i]);
      i=i+1;
E 4
I 4
      ind=i;
      return;
E 4
    }
    else {
D 11
      TutOut("%s", help);
E 11
I 11
      TutOut((T_STR)"%s", help);
E 11
D 14
      exit(0);
E 14
I 14
      exit(EXIT_FAILURE);
E 14
    }
  }

  return;
}
  
//------------------------------------------------------------------------
  
E 1
