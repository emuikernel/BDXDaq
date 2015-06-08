// ipc_check

// normal mode:   EXIT_SUCCESS if ipc process running, EXIT_FAILURE otherwise
// verbose mode:  prints ipc name of all processes that respond

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
static char *unique_name = NULL;
static int wait_time     = 4;
static int names_only    = 0;
static int verbose       = 0;
static int broadcast     = 0;
static int debug         = 0;
static int rec_count     = 0;

#define MAX_REPLY 256
static T_STR reply[MAX_REPLY];


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
  if(debug==0)TutSetOutputFunc(ipc_output_dummy);
  ipc_set_application(application);
  if(unique_name==NULL) {
    ipc_init((char*)"",(char*)"ipc_check");
  } else {
    ipc_init(unique_name,(char*)"ipc_check");
  }
  

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
  

  // wait for result, exit if received
  server.MainLoop((double)wait_time);
  

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
  

  // exit properly
  if(rec_count>0) {
    if((debug!=0)||(verbose!=0)) {
      if(names_only==0)cout << "\n  received " << rec_count << " replies" << endl << endl;
      exit(EXIT_SUCCESS);
    }
    
  } else {
    if((debug!=0)||(verbose!=0)) {
      if(names_only==0)cout << "\n  ?did NOT receive reply" << endl << endl;
    }
    exit(EXIT_FAILURE);
  }
}


//--------------------------------------------------------------


void receive_status_result(
			 T_IPC_CONN conn,
			 T_IPC_CONN_PROCESS_CB_DATA data,
			 T_CB_ARG arg) {

  TipcMsg msg=data->msg;
  char *s=strdup(msg.Sender());


  rec_count++;
  if(debug!=0) {cout << "received reply from " << s << endl;}


  if((verbose==0)&&(broadcast==0)) {
      if(strcasecmp(destination,s)==0)exit(EXIT_SUCCESS);
      while(s[0]=='/')s++;
      if(strcasecmp(destination,s)==0)exit(EXIT_SUCCESS);
      return;
  }


  // trim leading slash
  while(s[0]=='/')s++;


  // save names for broadcast
  if((broadcast!=0)&&(rec_count<=MAX_REPLY)) reply[rec_count-1]=s;


  // print names for verbose
  if(verbose!=0) cout << s << endl;
    

  return;
}


//--------------------------------------------------------------


void decode_command_line(int argc, char **argv) {

  const char *help ="\nusage:\n\n   ipc_check [-a application] [-d destination] "
    "[-u unique_name] [-w wait_time] [-verbose] [-names_only] [-broadcast]\n\n\n";
  int i=1;

  while(i<argc) {

    if(strncasecmp(argv[i],"-h",2)==0) {
      printf("%s", help);
      exit(EXIT_SUCCESS);
      
    } else  if (strncasecmp(argv[i],"-debug",6)==0) {
      debug=1;
      i=i+1;

    } else  if (strncasecmp(argv[i],"-names_only",11)==0) {
      names_only=1;
      i=i+1;

    } else  if (strncasecmp(argv[i],"-verbose",8)==0) {
      verbose=1;
      i=i+1;

    } else  if (strncasecmp(argv[i],"-broadcast",10)==0) {
      broadcast=1;
      i=i+1;

    } else if (strncasecmp(argv[i],"-a",2)==0) {
      application=strdup(argv[i+1]);
      i=i+2;

    } else if (strncasecmp(argv[i],"-d",2)==0) {
      destination=strdup(argv[i+1]);
      i=i+2;

    } else if (strncasecmp(argv[i],"-u",2)==0) {
      unique_name=strdup(argv[i+1]);
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
