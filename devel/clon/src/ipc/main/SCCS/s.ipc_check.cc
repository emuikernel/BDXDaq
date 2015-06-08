h05105
s 00014/00006/00215
d D 1.7 06/11/08 14:17:28 wolin 8 7
c Added -names_only
c 
e
s 00047/00022/00174
d D 1.6 01/10/12 16:26:27 wolin 7 6
c Added broadcast
e
s 00011/00002/00185
d D 1.5 01/10/02 12:33:00 wolin 6 5
c Added unique_name
c 
e
s 00034/00019/00153
d D 1.4 01/10/01 12:00:07 wolin 5 4
c Added -verbose
e
s 00007/00001/00165
d D 1.3 01/09/28 16:15:41 wolin 4 3
c Accept leading /
e
s 00008/00010/00158
d D 1.2 01/09/28 15:39:04 wolin 3 1
c Exit immediately if result received
e
s 00000/00000/00000
d R 1.2 01/09/28 15:35:22 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 ipcutil/s/ipc_check.cc
e
s 00168/00000/00000
d D 1.1 01/09/28 15:35:21 wolin 1 0
c 
e
u
U
f b 
f e 0
t
T
I 1
// ipc_check

D 5
// EXIT_SUCCESS if ipc process running, EXIT_FAILURE otherwise
E 5
I 5
// normal mode:   EXIT_SUCCESS if ipc process running, EXIT_FAILURE otherwise
// verbose mode:  prints ipc name of all processes that respond
E 5

// E.Wolin, 28-sep-01



#define _POSIX_SOURCE 1
#define __EXTENSIONS__


// smartsockets
#include <rtworks/cxxipc.hxx>


// system 
#include <iostream.h>
#include <stdlib.h>


// CLAS ipc
#include <clas_ipc_prototypes.h>


// misc variables
static char *application = (char*)"clastest";
static char *destination = NULL;
I 6
static char *unique_name = NULL;
E 6
static int wait_time     = 4;
I 8
static int names_only    = 0;
E 8
D 5
static int got_it        = 0;
E 5
I 5
static int verbose       = 0;
I 7
static int broadcast     = 0;
E 7
E 5
static int debug         = 0;
I 5
static int rec_count     = 0;
E 5

I 7
#define MAX_REPLY 256
static T_STR reply[MAX_REPLY];

E 7

// prototypes
void receive_status_result(
			   T_IPC_CONN conn,
			   T_IPC_CONN_PROCESS_CB_DATA data,
			   T_CB_ARG arg);
void decode_command_line(int argc, char **argv);


// ref to IPC server (connection created later)
TipcSrv &server=TipcSrv::Instance();


/*------------------------------------------------------------------*/


int main(int argc, char **argv) {

  char *s;


  // decode command line
  decode_command_line(argc,argv);

  
  // check destination
  if(destination==NULL) {
    cerr << "?no destination" << endl;
    exit(EXIT_FAILURE);
  }
  

  //  connect to server
D 7
  TutSetOutputFunc(ipc_output_dummy);
E 7
I 7
  if(debug==0)TutSetOutputFunc(ipc_output_dummy);
E 7
  ipc_set_application(application);
D 6
  ipc_init((char*)"",(char*)"ipc_check");
E 6
I 6
  if(unique_name==NULL) {
    ipc_init((char*)"",(char*)"ipc_check");
  } else {
    ipc_init(unique_name,(char*)"ipc_check");
  }
E 6
  

  // subscribe to status_poll_result message group
  ipc_get_status_poll_group(&s);
  server.SubjectSubscribe(s,TRUE);
  

  //  define callback to receive status_poll_result
  TipcMt mt((T_STR)"status_poll_result");
  server.ProcessCbCreate(mt,receive_status_result,NULL);

  
  // send out status poll request
  TipcMsg msg(TipcMtLookupByNum(T_MT_CONTROL));
  msg.Dest(destination);
  msg << (T_STR)"status_poll";
  server.Send(msg,TRUE);
  server.Flush();
  

D 3
  // wait for result
E 3
I 3
  // wait for result, exit if received
E 3
  server.MainLoop((double)wait_time);
  

D 3
  // check if received result
  if(got_it!=0) {
    if(debug!=0)cout << "\n  received reply" << endl << endl;
    exit(EXIT_SUCCESS);
  } else {
    if(debug!=0)cout << "\n  ?did NOT receive reply" << endl << endl;
    exit(EXIT_FAILURE);
  }
E 3
I 3
D 5
  // did not receive result
  if(debug!=0)cout << "\n  ?did NOT receive reply" << endl << endl;
  exit(EXIT_FAILURE);
E 5
I 5
D 7
  // exit properly
  if((verbose==1)&&(rec_count>0)) {
    if(debug!=0)cout << "\n  received " << rec_count << " replies" << endl << endl;
    exit(EXIT_SUCCESS);
E 7
I 7
  // broadcast message
  if((broadcast!=0)&&(rec_count>0)) {
      TipcMsg msg((T_STR)"info_server");
      msg.Dest((T_STR)"info_server/in/ipc_check");
      msg << (T_STR)"ipc_check";
      for(int i=0; i<rec_count; i++) msg << reply[i];
      server.Send(msg,TRUE);
      server.Flush();
      if(debug!=0)msg.Print(TutOut);
  }
  
E 7

I 7
  // exit properly
  if(rec_count>0) {
D 8
      if((debug!=0)||(verbose!=0))
	  cout << "\n  received " << rec_count << " replies" << endl << endl;
E 8
I 8
    if((debug!=0)||(verbose!=0)) {
      if(names_only==0)cout << "\n  received " << rec_count << " replies" << endl << endl;
E 8
      exit(EXIT_SUCCESS);
D 8
      
E 8
I 8
    }
    
E 8
E 7
  } else {
D 7
    if(debug!=0)cout << "\n  ?did NOT receive reply" << endl << endl;
    exit(EXIT_FAILURE);
E 7
I 7
D 8
      if((debug!=0)||(verbose!=0))cout << "\n  ?did NOT receive reply" << endl << endl;
      exit(EXIT_FAILURE);
E 8
I 8
    if((debug!=0)||(verbose!=0)) {
      if(names_only==0)cout << "\n  ?did NOT receive reply" << endl << endl;
    }
    exit(EXIT_FAILURE);
E 8
E 7
  }
E 5
E 3
}


//--------------------------------------------------------------


void receive_status_result(
			 T_IPC_CONN conn,
			 T_IPC_CONN_PROCESS_CB_DATA data,
			 T_CB_ARG arg) {

  TipcMsg msg=data->msg;
  char *s=strdup(msg.Sender());


D 5
  if(debug!=0) {
    cout << "received result from " << s << endl;
  }
E 5
I 5
  rec_count++;
  if(debug!=0) {cout << "received reply from " << s << endl;}
E 5

D 4
  // check sender...ignore leading '/'
E 4
I 4
D 5
  // check sender (full name)
  if(strcasecmp(destination,s)==0) {
    if(debug!=0)cout << "\n  received reply" << endl << endl;
    exit(EXIT_SUCCESS);
  }
    
  // check again...ignore leading '/'
E 4
  while(s[0]=='/')s++;
D 3
  if(strcasecmp(destination,s)==0)got_it=1;
E 3
I 3
  if(strcasecmp(destination,s)==0) {
    if(debug!=0)cout << "\n  received reply" << endl << endl;
E 5
I 5

D 7
  if(verbose!=0) {
    while(s[0]=='/')s++;
    cout << s << endl;
    return;
  
  } else if(strcasecmp(destination,s)==0) {
E 5
    exit(EXIT_SUCCESS);
I 5

  } else {
    while(s[0]=='/')s++;
    if(strcasecmp(destination,s)==0) {
      if(debug!=0)cout << "\n  received reply" << endl << endl;
      exit(EXIT_SUCCESS);
    }
E 7
I 7
  if((verbose==0)&&(broadcast==0)) {
      if(strcasecmp(destination,s)==0)exit(EXIT_SUCCESS);
      while(s[0]=='/')s++;
      if(strcasecmp(destination,s)==0)exit(EXIT_SUCCESS);
      return;
E 7
E 5
  }
I 7


  // trim leading slash
  while(s[0]=='/')s++;


  // save names for broadcast
  if((broadcast!=0)&&(rec_count<=MAX_REPLY)) reply[rec_count-1]=s;


  // print names for verbose
  if(verbose!=0) cout << s << endl;
E 7
E 3
    
I 7

E 7
  return;
}


//--------------------------------------------------------------


void decode_command_line(int argc, char **argv) {

  const char *help ="\nusage:\n\n   ipc_check [-a application] [-d destination] "
D 5
    "[-w wait_time] \n\n\n";
E 5
I 5
D 6
    "[-w wait_time] [-verbose] \n\n\n";
E 6
I 6
D 7
    "[-u unique_name] [-w wait_time] [-verbose] \n\n\n";
E 7
I 7
D 8
    "[-u unique_name] [-w wait_time] [-verbose] [-broadcast]\n\n\n";
E 8
I 8
    "[-u unique_name] [-w wait_time] [-verbose] [-names_only] [-broadcast]\n\n\n";
E 8
E 7
E 6
E 5
  int i=1;

  while(i<argc) {

    if(strncasecmp(argv[i],"-h",2)==0) {
      printf("%s", help);
      exit(EXIT_SUCCESS);
      
    } else  if (strncasecmp(argv[i],"-debug",6)==0) {
      debug=1;
I 5
      i=i+1;

I 8
    } else  if (strncasecmp(argv[i],"-names_only",11)==0) {
      names_only=1;
      i=i+1;

E 8
    } else  if (strncasecmp(argv[i],"-verbose",8)==0) {
      verbose=1;
I 7
      i=i+1;

    } else  if (strncasecmp(argv[i],"-broadcast",10)==0) {
      broadcast=1;
E 7
E 5
      i=i+1;

    } else if (strncasecmp(argv[i],"-a",2)==0) {
      application=strdup(argv[i+1]);
      i=i+2;

    } else if (strncasecmp(argv[i],"-d",2)==0) {
      destination=strdup(argv[i+1]);
I 6
      i=i+2;

    } else if (strncasecmp(argv[i],"-u",2)==0) {
      unique_name=strdup(argv[i+1]);
E 6
      i=i+2;

    } else if (strncasecmp(argv[i],"-w",5)==0) {
      wait_time=atoi(argv[i+1]);
      i=i+2;

    }  else {
      printf("Unknown command line arg: %s\n",argv[i]);
      exit(EXIT_FAILURE);
    }

  }
}


// ---------------------------------------------------------------
E 1
