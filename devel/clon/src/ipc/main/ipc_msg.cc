//
//  ipc_msg
//
//  Sends out arbitrary Smartsockets message
//
//  ejw, 22-jun-99


#define _POSIX_SOURCE 1
#define __EXTENSIONS__


#include <rtworks/cxxipc.hxx>
#include <fstream.h>
using namespace std;
#include <strstream>
#include <iostream.h>
#include <iomanip.h>
#include <stdio.h>
#include <time.h>


char *project  	= (char*)"clastest";
char *msgtype 	= NULL;
char *dest    	= NULL;
char *sender  	= NULL;
char *uniq_name = NULL;
int gmd       	= 0;
int debug     	= 0;
int ind;
char buffer[1024];


void decode_command_line(int argc, char **argv);
extern "C" {
void init_msg_types(void);
void ipc_output_dummy(T_STR str, va_list arg);
}


//------------------------------------------------------------------------


int main(int argc, char **argv) {
  

  int i,j,len;


  // synch with c i/o
  ios::sync_with_stdio();


  // decode command line args
  decode_command_line(argc,argv);


  // check args
  if(dest==NULL){TutWarning((T_STR)"?no destination\n\n"); exit(EXIT_FAILURE);}
  if(msgtype==NULL){TutWarning((T_STR)"?no msgtype\n\n"); exit(EXIT_FAILURE);}
  if(sender==NULL)sender=(char*)"ipc_msg";
  if((gmd>0)&&(uniq_name==NULL)){TutWarning((T_STR)"?no unique name for gmd\n\n"); exit(EXIT_FAILURE);}


  // inhibit output
  if(debug==0)TutSetOutputFunc(ipc_output_dummy);


  // set projext
  T_OPTION opt=TutOptionLookup((T_STR)"Project");
  if(!TutOptionSetEnum(opt,project)){cerr << "?unable to set project\n" << endl; exit(EXIT_FAILURE);}


  // setup gmd if requested
  if(gmd>0) {
    opt=TutOptionLookup((T_STR)"Unique_Subject");
    TutOptionSetEnum(opt,uniq_name);
    opt=TutOptionLookup((T_STR)"Server_Disconnect_Mode");
    TutOptionSetEnum(opt,(T_STR)"warm");
    opt=TutOptionLookup((T_STR)"Server_Delivery_Timeout");
    TutOptionSetNum(opt,0.0);
  }


  // connect to server 
  TipcSrv &server=TipcSrv::InstanceCreate(T_IPC_SRV_CONN_FULL);


  // destroy gmd failure callbacks, if gmd requested
  if(gmd>0) {
    T_IPC_MT mt=TipcMtLookupByNum(T_MT_GMD_FAILURE);
    T_CB cb=TipcSrvProcessCbLookup(mt,TipcCbConnProcessGmdFailure, NULL);
    TutCbDestroy(cb);
    cb=TipcSrvProcessCbLookup(mt,TipcCbSrvProcessGmdFailure, NULL);
    TutCbDestroy(cb);
  }


  // init standard msg types
  init_msg_types();


  // create message
  TipcMsg message(msgtype);
  if(!message) {
    cerr << "unable to create message type: " << msgtype << endl;
    exit(EXIT_FAILURE);
  }


  // sender and destination
  message.Sender(sender);
  message.Dest(dest);


  // set delivery mode for gmd
  if(gmd>0) message.DeliveryMode(T_IPC_DELIVERY_ALL);


  // append args
  for(i=ind; i<argc; i++) {

    if(strncasecmp(argv[i],"-unixtime",9)==0) {
      message << (T_INT4)time(NULL);

    } else if(strncasecmp(argv[i],"-i4a",4)==0) {
      len=atoi(argv[i+1]);
      int *i4a = new int[len];
      for(j=0; j<len; j++) i4a[j]=atoi(argv[i+j+2]);
      message << SetSize(len) << (T_INT4*)i4a;
      delete [] i4a;
      i+=j;
      i++;

    } else if(strncasecmp(argv[i],"-r4a",4)==0) {
      len=atoi(argv[i+1]);
      float *r4a = new float[len];
      for(j=0; j<len; j++) r4a[j]=atof(argv[i+j+2]);
      message << SetSize(len) << (T_REAL4*)r4a;
      delete [] r4a;
      i+=j;
      i++;

    } else if(strncasecmp(argv[i],"-r8a",4)==0) {
      len=atoi(argv[i+1]);
      double *r8a = new double [len];
      for(j=0; j<len; j++) r8a[j]=(double)atof(argv[i+j+2]);
      message << SetSize(len) << (T_REAL8*)r8a;
      delete [] r8a;
      i+=j;
      i++;

    } else if (strncasecmp(argv[i],"-i4",3)==0) {
      message << (T_INT4)atoi(argv[i+1]);
      i++;

    } else if (strncasecmp(argv[i],"-r4",3)==0) {
      message << (T_REAL4)atof(argv[i+1]);
      i++;

    } else if (strncasecmp(argv[i],"-r8",3)==0) {
      message << (T_REAL8)atof(argv[i+1]);
      i++;

    } else if (strncasecmp(argv[i],"-str",4)==0) {
      message << argv[i+1];
      i++;

    } else if (strncasecmp(argv[i],"-file",5)==0) {
      ifstream file(argv[i+1]);
      if(!file.is_open())TutWarning((T_STR)"?file doesn't exist\n\n"),exit(EXIT_FAILURE);
      strstream str;
      for(;;) {
	if(!file.is_open()||!file.good()||file.eof()||file.fail()||file.bad()) break;
	file.getline(buffer,sizeof(buffer));
	str << buffer << endl;
      }
      message << str.str();
      file.close();
      i++;
    }  
  }



  //  dump message
  if(debug==1) {
    cout << endl;
    message.Print(TutOut);
    cout << endl;
  }


  // send and flush, wait if gmd
  server.Send(message,TRUE);
  server.Flush();
  if(gmd>0)server.MainLoop(T_REAL8(gmd));

  
  // close connection
  server.Destroy(T_IPC_SRV_CONN_NONE);
}


//------------------------------------------------------------------------


void decode_command_line(int argc, char **argv) {

  const char *help    = "\nusage:\n\n   ipc_msg [-a project] [-u uniq_name] [-sender sender]\n"
    "           [-dest dest] [-type msgtype] [-gmd time] [-debug]\n"
    "           [-i4 int4] [-r4 real4] [-r8 real8] [-str \"string\"] [-file filename]\n"
    "           [-i4a len i4_array[len]] [-r4a len r4_array[len]] [-r8 len real8[len]]\n";
    "           [-unixtime] \n\n";
    
  int i=1;


  if(argc<4)TutOut((T_STR)"%s", help),exit(EXIT_SUCCESS);


  while(i<argc) {
    if(strncasecmp(argv[i],"-h",2)==0) {
      TutOut((T_STR)"%s", help);
      exit(EXIT_SUCCESS);
    }
    else if (strncasecmp(argv[i],"-debug",6)==0) {
      debug=1;
      i=i+1;
    }
    else if (strncasecmp(argv[i],"-sender",7)==0) {
      sender=strdup(argv[i+1]);
      i=i+2;
    }
    else if (strncasecmp(argv[i],"-dest",5)==0) {
      dest=strdup(argv[i+1]);
      i=i+2;
    }
    else if (strncasecmp(argv[i],"-type",5)==0) {
      msgtype=strdup(argv[i+1]);
      i=i+2;
    }
    else if (strncasecmp(argv[i],"-gmd",4)==0) {
      gmd=atoi(argv[i+1]);
      i=i+2;
    }
    else if (strncasecmp(argv[i],"-a",2)==0) {
      project=strdup(argv[i+1]);
      i=i+2;
    }
    else if (strncasecmp(argv[i],"-u",2)==0) {
      uniq_name=strdup(argv[i+1]);
      i=i+2;
    }
    else if (strncasecmp(argv[i],"-",1)==0) {
      ind=i;
      return;
    }
    else {
      TutOut((T_STR)"%s", help);
      exit(EXIT_FAILURE);
    }
  }

  return;
}
  
//------------------------------------------------------------------------
  
