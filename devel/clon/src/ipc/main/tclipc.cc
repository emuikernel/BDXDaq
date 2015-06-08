//
//  tclipc
//
//  Presents Tcl IPC "shell" which reads tcl commands and sends them to remote process
//
//   Use [-c command] to run a single command, then quit
//   Use [-exit_status] to exit with atoi(result) if result is a number
//
//   commands are sent to remote process unless prefixed with @
//   legal @ commands:
//        @help
//        @file myfile     send myfile to remote process
//        @timeout t       reset timeout
//        @quit            stop program
//        @stop            stop program
//        @exit            stop program
//
//
//   verifies command for "quit", "stop", "exit"
//
//
//  ejw, 21-oct-96
//
//

#define _POSIX_SOURCE 1
#define __EXTENSIONS__


#include <rtworks/cxxipc.hxx>
#include <iostream.h>
#include <fstream.h>
#include <clas_ipc_prototypes.h>
#include <pthread.h>


static char *process   = NULL;
static char *appl      = (char*)"clastest";
static char *help1     = (char*)"\nusage:\n\n  tclipc [-a application] [-t timeout] [-c command] [-async] "
                      	 "[-exit_status] process \n\n";
static float timeout   = 5.0;
static char *one_cmd   = NULL;
static int exit_status = 0;
static int async       = 0;
static T_INT4 code;
static T_STR result;
static char inbuf[256];
static char *help2     = (char*)"\n Legal tclipc commands:\n\n"
                  "    any Tcl command\n"
                  "                  type help to get list of commands implemented in process\n"
                  "                  note:  use setc for fortran strings, NOT set\n" 
                  "    @file myfile  to send Tcl commands in myfile to process\n"
                  "    @timeout t    to reset RPC timeout (float, default 5.0 sec)\n"
                  "    @exit         to exit tclipc\n"
                  "    @quit         to exit tclipc\n"
                  "    @stop         to exit tclipc\n"
                  "\n";


// prototypes
void decode_command_line(int argc, char **argv);
void process_cmd(char *cmd);
char *get_message(char *cmd);
extern "C" {
void *ipc_thread(void *param);
}

// ref to server (connection created later)
TipcSrv &server=TipcSrv::Instance();


//----------------------------------------------------------------------


int main(int argc, char **argv)
{
  int count;
  int stat;
  char ans[2];
  pthread_t t1;
  int r1=0;


  // decode command line args
  decode_command_line(argc,argv);

  // synch with c i/o
  ios::sync_with_stdio();

  // set application
  ipc_set_application(appl);

  // connect to ipc
  if(!TipcInitThreads()) {
    cerr << "Unable to init IPC thread package" << endl;
    exit(EXIT_FAILURE);
  }
  strcpy(inbuf,"tclipc_");
  strcat(inbuf,process);
  stat=ipc_init(inbuf,(char*)"tclipc");
  if(stat!=0){
    cout << "\n?Unable to connect to server...probably duplicate unique id\n"
	 << "   ...check for another tclipc process connected to " << process << " using ipc_info\n"
	 << "   ...only one connection allowed!" << endl << endl;
    exit(0);
  }
  cout << endl;


  // if in one_cmd mode, just execute command and quit
  if(one_cmd!=NULL){
    process_cmd(one_cmd);
    if(exit_status==1) {
      if((code==0)&&(isdigit(result[0]))) {
	exit(atoi(result));
      } else {
	exit(99);
      } 
    } else {
      exit(EXIT_SUCCESS);
    }
  }


  // print startup help message
  cout << "\n\n (type @help for help)" << endl << endl;


  // init ipc thread
//    thr_setconcurrency(thr_getconcurrency()+2);
//    pthread_create(&t1,NULL,ipc_thread,(void *)&r1);
//    pthread_detach(t1);


  // loop forever checking for user input
  cout << "\ntclipc> "<< flush;
  for(;;) {

    cin.getline(inbuf,sizeof(inbuf));

    // verify input of quit,stop,exit
    if( (strncasecmp(inbuf,"quit",4)==0) || (strncasecmp(inbuf,"exit",4)==0) || 
	(strncasecmp(inbuf,"stop",4)==0) ) {
      cout << "\nAre you sure you want the remote procss to quit [y or n]: ";
      cin >> ans;
      if((ans[0]!='y')&&(ans[0]!='Y')){cout << "\ntclipc> "<< flush; continue;}
    }
    process_cmd(inbuf);
    ipc_check(0.0);
    cout << "\ntclipc> "<< flush;
  }

}


//---------------------------------------------------------------------


extern "C" {
void *ipc_thread(void *param) {

  // check for ipc messages
  while(1==1) {
    ipc_check(1.0);
  }

  return (void *)0;
}
}
  

//-------------------------------------------------------------------


void process_cmd(char *cmd){

  char *cmdstart;
  char *msg;


  // get pointer to 1st non-blank char
  for(cmdstart=cmd; (cmdstart[0]!='\0')&&((cmdstart[0]==' ')||(cmdstart[0]=='\t')); cmdstart++);


  // handle different message types, internal and external
  if((strncasecmp(cmdstart,"@file",5)==0)||(cmdstart[0]!='@')){       // external commmand 

    // get message text
    msg=get_message(cmdstart);

    // create message, fill destination, fill contents, send, print replies
    if(msg!=NULL) {

      TipcMsg message((char*)"tcl_request");
      message.Dest(process);
      message << msg;

      if(async==0) {
	TipcMsg reply=server.SendRpc(message,(T_REAL8)timeout);
	if(reply) {
	  for(int i=0; i<reply.NumFields(); i+=2) {
	    reply >> code >> result;
	    if(code==0) {
	      cout << result << endl;
	    } else {
	      cout << "\n?Return code: " << code << endl << result << endl;
	    }
	  } 
	} else {
	  cout << "\nNo reply...Timed out after " << timeout << " seconds" << endl << endl;
	  code=-1;
	}

      } else {
	result=(char*)"0";
	server.Send(message);
	server.Flush();
      }

      free(msg);
    }


  } else {                                     // internal command
    if(strncasecmp(cmdstart+1,"help",4)==0){
      cout << help2 << endl;
    }else if(strncasecmp(cmdstart+1,"timeout",7)==0){
      sscanf(cmdstart+9,"%f",&timeout);
    }else if(strncasecmp(cmdstart+1,"quit",4)==0){
      ipc_close();
      exit(EXIT_SUCCESS);
    }else if(strncasecmp(cmdstart+1,"stop",4)==0){
      ipc_close();
      exit(EXIT_SUCCESS);
    }else if(strncasecmp(cmdstart+1,"exit",4)==0){
      ipc_close();
      exit(EXIT_SUCCESS);
    } else {
      cout << "?Unknown command " << cmdstart << endl << endl;
    }
  }

}


//---------------------------------------------------------------------


char *get_message(char *cmd){

  struct stat buf;
  char *msg=NULL;
  char *filename;
  
  if(cmd[0]!='@'){                             // just return cmd
    msg=strdup(cmd);
    
  } else {                                     // get contents of file
    for(filename=cmd+5; (filename[0]!='\0')&&((filename[0]==' ')||(filename[0]=='\t')); filename++);
    if(access(filename,R_OK)==0){
      stat(filename,&buf);
      ifstream file;
      file.open(filename);
      msg = (char *) malloc(buf.st_size+1);
      file.read(msg,buf.st_size);
      file.close();
      msg[buf.st_size]='\0';
    } else {
      cout << "\n?no such file: " << filename << endl;
    }
  }

  return (msg);

}


//---------------------------------------------------------------------


void decode_command_line(int argc, char**argv){


  // error if no command line args
  if(argc<2){cout <<  help1 << endl; exit(0);}
  
  
  // decode command line
  int i=1;
  while(i<argc) {

    if(strncasecmp(argv[i],"-h",2)==0) {
      cout << help1 << endl;
      exit(0);
    }
    else if (strncasecmp(argv[i], "-", 1)!= 0) {
      break;   // reached process name field
    }    
    else if (strncasecmp(argv[i],"-async",6)==0) {
      async=1;
      i=i+1;
    }
    else if (strncasecmp(argv[i],"-a",2)==0) {
      //      free(appl)
      appl=strdup(argv[i+1]);
      i=i+2;
    }
    else if (strncasecmp(argv[i],"-t",2)==0) {
      sscanf(argv[i+1],"%f",&timeout);
      i=i+2;
    }
    else if (strncasecmp(argv[i],"-c",2)==0) {
      one_cmd=strdup(argv[i+1]);
      i=i+2;
    }
    else if (strncasecmp(argv[i],"-exit_status",12)==0) {
      exit_status=1;
      i=i+1;
    }
    else if (strncasecmp(argv[i],"-",1)==0) {
      cout << "Unknown command line arg: " << argv[i] << argv[i+1] << endl << endl;
      i=i+2;
    }
  }

  // set process name
  if(i>argc){
    cout << "?no process specified" << endl << endl;
    exit(0);
  } else {
    process=argv[i];
  }

}

  
//----------------------------------------------------------------
