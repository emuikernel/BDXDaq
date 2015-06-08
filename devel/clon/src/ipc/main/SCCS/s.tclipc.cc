h33341
s 00001/00001/00336
d D 1.19 01/02/06 12:54:47 wolin 20 19
c Using cin.getline()
e
s 00055/00036/00282
d D 1.18 00/09/07 14:48:58 wolin 19 18
c RPC not working, many fixes for new CC
e
s 00035/00020/00283
d D 1.17 98/04/23 16:55:30 wolin 18 17
c Added async
c 
e
s 00008/00010/00295
d D 1.16 98/04/15 17:50:36 wolin 17 16
c Bug...code/reply pairs
c 
e
s 00010/00007/00295
d D 1.15 98/04/15 17:46:00 wolin 16 15
c Changed unique name, print all replies
c 
e
s 00001/00001/00301
d D 1.14 98/03/10 15:58:39 wolin 15 14
c Error code now 99
c 
e
s 00001/00001/00301
d D 1.13 98/03/10 13:48:20 wolin 14 13
c Error exit status -99
c 
e
s 00003/00002/00299
d D 1.12 98/03/10 13:44:36 wolin 13 12
c More bombproofing
c 
e
s 00027/00018/00274
d D 1.11 98/03/10 13:37:26 wolin 12 11
c Added exit_status flag
c 
e
s 00010/00006/00282
d D 1.10 98/01/07 09:31:19 wolin 11 10
c Added ipc_close call
c 
e
s 00007/00006/00281
d D 1.9 96/11/13 14:19:37 wolin 10 9
c Minor bug in timeout
c 
e
s 00011/00009/00276
d D 1.8 96/10/22 12:35:05 wolin 9 8
c Minor mods
e
s 00001/00001/00284
d D 1.7 96/10/22 11:23:41 wolin 8 7
c Typo
e
s 00015/00001/00270
d D 1.6 96/10/21 15:59:21 wolin 7 6
c Bombproofed quit,exit,stop
e
s 00001/00001/00270
d D 1.5 96/10/17 12:51:36 wolin 6 5
c Typo
e
s 00007/00002/00264
d D 1.4 96/10/17 11:38:45 wolin 5 4
c Minor bug
e
s 00150/00066/00116
d D 1.3 96/10/17 11:22:22 wolin 4 3
c Major upgrade, handles internal and external commands, help, etc.
e
s 00017/00009/00165
d D 1.2 96/10/11 15:25:39 wolin 3 1
c Checks for non-existant files now
e
s 00000/00000/00000
d R 1.2 96/10/09 17:04:55 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 2 1 ipcutil/s/tclipc.cc
c Name history : 1 0 s/tclipc.cc
e
s 00174/00000/00000
d D 1.1 96/10/09 17:04:54 wolin 1 0
c Tcl IPC shell
e
u
U
f e 0
t
T
I 1
//
//  tclipc
//
//  Presents Tcl IPC "shell" which reads tcl commands and sends them to remote process
//
D 12
//
//  Usage:
//
D 4
//     tclipc [-a application] [-t timeout] destination 
E 4
I 4
//     tclipc [-a application] [-t timeout] [-c command] process
E 4
//
//       default application     = "clastest"
//       default timeout         = 5.0 sec
//
E 12
I 4
//   Use [-c command] to run a single command, then quit
I 12
//   Use [-exit_status] to exit with atoi(result) if result is a number
E 12
E 4
//
I 4
//   commands are sent to remote process unless prefixed with @
//   legal @ commands:
//        @help
//        @file myfile     send myfile to remote process
//        @timeout t       reset timeout
//        @quit            stop program
//        @stop            stop program
//        @exit            stop program
//
E 4
D 7
//  ejw, 9-oct-96
E 7
//
I 7
//   verifies command for "quit", "stop", "exit"
E 7
//
I 7
//
//  ejw, 21-oct-96
//
//
E 7

#define _POSIX_SOURCE 1
#define __EXTENSIONS__


#include <rtworks/cxxipc.hxx>
#include <iostream.h>
#include <fstream.h>
I 4
D 11
#include <clas_ipc.h>
E 11
E 4
#include <clas_ipc_prototypes.h>
D 19
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/filio.h>
I 10
#include <stdio.h>
E 19
I 19
#include <pthread.h>
E 19
E 10

I 11

E 11
D 4
char *dest    	= NULL;
E 4
I 4
D 12
char *process  	= NULL;
E 4
char *appl     	= "clastest";
D 4
char *help    	= "\nusage:\n\n  tclipc [-a application] [-t timeout] destination \n\n";
E 4
I 4
D 5
char *help1    	= "\nusage:\n\n  tclipc [-a application] [-t timeout] [-c command] process_id \n\n";
E 5
I 5
char *help1    	= "\nusage:\n\n  tclipc [-a application] [-t timeout] [-c command] process \n\n";
E 5
E 4
float timeout   = 5.0;
I 4
char *one_cmd   = NULL;
char inbuf[256];
char *help2     = "\n Legal tclipc commands:\n\n"
E 12
I 12
static char *process   = NULL;
D 19
static char *appl      = "clastest";
D 18
static char *help1     = "\nusage:\n\n  tclipc [-a application] [-t timeout] [-c command] "
E 18
I 18
static char *help1     = "\nusage:\n\n  tclipc [-a application] [-t timeout] [-c command] [-async] "
E 19
I 19
static char *appl      = (char*)"clastest";
static char *help1     = (char*)"\nusage:\n\n  tclipc [-a application] [-t timeout] [-c command] [-async] "
E 19
E 18
                      	 "[-exit_status] process \n\n";
static float timeout   = 5.0;
static char *one_cmd   = NULL;
static int exit_status = 0;
I 18
static int async       = 0;
E 18
D 19

E 19
static T_INT4 code;
static T_STR result;
static char inbuf[256];
D 19
static char *help2     = "\n Legal tclipc commands:\n\n"
E 19
I 19
static char *help2     = (char*)"\n Legal tclipc commands:\n\n"
E 19
E 12
                  "    any Tcl command\n"
                  "                  type help to get list of commands implemented in process\n"
                  "                  note:  use setc for fortran strings, NOT set\n" 
                  "    @file myfile  to send Tcl commands in myfile to process\n"
                  "    @timeout t    to reset RPC timeout (float, default 5.0 sec)\n"
                  "    @exit         to exit tclipc\n"
                  "    @quit         to exit tclipc\n"
                  "    @stop         to exit tclipc\n"
                  "\n";
E 4

I 4

I 12
// prototypes
E 12
E 4
void decode_command_line(int argc, char **argv);
I 4
D 10
void process_cmd(char *cmd, float timeout);
E 10
I 10
void process_cmd(char *cmd);
E 10
char *get_message(char *cmd);
E 4
D 19

E 19
I 19
extern "C" {
void *ipc_thread(void *param);
}
E 19
I 11

E 11
// ref to server (connection created later)
TipcSrv &server=TipcSrv::Instance();


//----------------------------------------------------------------------


int main(int argc, char **argv)
{
D 4
  
  T_INT4 code;
  T_STR result;
  struct stat buf;
  char command[1024];
  char *file_contents;
  int count,i;
  char *startcmd;
E 4
I 4
  int count;
  int stat;
I 7
  char ans[2];
I 19
  pthread_t t1;
  int r1=0;
E 19
E 7
E 4


  // decode command line args
  decode_command_line(argc,argv);

I 19
  // synch with c i/o
  ios::sync_with_stdio();

E 19
  // set application
  ipc_set_application(appl);

D 4
  // connect to ipc...unique id is: destination_tclipc
  strcpy(command,dest);
  strcat(command,"_tclipc");
  ipc_init(command,"tclipc");
E 4
I 4
D 11
  // connect to ipc...unique id is: process_tclipc...die if unable to connect
E 11
I 11
  // connect to ipc
I 19
  if(!TipcInitThreads()) {
    cerr << "Unable to init IPC thread package" << endl;
    exit(EXIT_FAILURE);
  }
E 19
E 11
D 16
  strcpy(inbuf,process);
  strcat(inbuf,"_tclipc");
E 16
I 16
  strcpy(inbuf,"tclipc_");
  strcat(inbuf,process);
E 16
D 19
  stat=ipc_init(inbuf,"tclipc");
E 19
I 19
  stat=ipc_init(inbuf,(char*)"tclipc");
E 19
  if(stat!=0){
    cout << "\n?Unable to connect to server...probably duplicate unique id\n"
	 << "   ...check for another tclipc process connected to " << process << " using ipc_info\n"
	 << "   ...only one connection allowed!" << endl << endl;
    exit(0);
  }
  cout << endl;
E 4


D 4
  // first prompt
D 3
  cout << "\ntclipc > " << flush;
E 3
I 3
  cout << "\ntclipc > "<< flush;
E 4
I 4
  // if in one_cmd mode, just execute command and quit
  if(one_cmd!=NULL){
D 10
    process_cmd(one_cmd,timeout);
E 10
I 10
    process_cmd(one_cmd);
E 10
D 12
    exit(0);
E 12
I 12
    if(exit_status==1) {
D 13
      if(isdigit(result[0])) {
E 13
I 13
      if((code==0)&&(isdigit(result[0]))) {
E 13
	exit(atoi(result));
      } else {
D 13
	exit(EXIT_FAILURE);
E 13
I 13
D 14
	exit(-9999);
E 14
I 14
D 15
	exit(-99);
E 15
I 15
	exit(99);
E 15
E 14
E 13
      } 
    } else {
      exit(EXIT_SUCCESS);
    }
E 12
  }
E 4
E 3

I 3

E 3
D 4
  // loop forever checking for user input
  for (;;){
E 4
I 4
  // print startup help message
  cout << "\n\n (type @help for help)" << endl << endl;
E 4

I 4

D 19
  // loop forever checking for user input and handling ipc messages
E 19
I 19
  // init ipc thread
//    thr_setconcurrency(thr_getconcurrency()+2);
//    pthread_create(&t1,NULL,ipc_thread,(void *)&r1);
//    pthread_detach(t1);


  // loop forever checking for user input
E 19
  cout << "\ntclipc> "<< flush;
D 19
  for(;;){
E 19
I 19
  for(;;) {
E 19

D 19
    // check if any user input available
E 4
    ioctl(fileno(stdin),FIONREAD,&count);
E 19
I 19
D 20
    cin >> inbuf;
E 20
I 20
    cin.getline(inbuf,sizeof(inbuf));
E 20
E 19
I 9

E 9
D 11
    if(count>0){
E 11
I 11
D 19
    if(count>0) {
E 11
I 4
      fgets(inbuf,1024,stdin);
      inbuf[count-1]='\0';
I 7
D 9

      // verify input of quit,stop,exit
      if( (strncasecmp(inbuf,"quit",4)==0) || (strncasecmp(inbuf,"exit",4)==0) || 
	  (strncasecmp(inbuf,"stop",4)==0) ){
D 8
	cout << "\n  Are you sure you want the remote procss to quit [y or n]: ";
E 8
I 8
	cout << "\nAre you sure you want the remote procss to quit [y or n]: ";
E 8
	cin >> ans;
	if((ans[0]!='y')&&(ans[0]!='Y')){cout << "\ntclipc> "<< flush; continue;}
E 9
I 9
    
      if(count>1){
	// verify input of quit,stop,exit
	if( (strncasecmp(inbuf,"quit",4)==0) || (strncasecmp(inbuf,"exit",4)==0) || 
	    (strncasecmp(inbuf,"stop",4)==0) ){
	  cout << "\nAre you sure you want the remote procss to quit [y or n]: ";
	  cin >> ans;
	  if((ans[0]!='y')&&(ans[0]!='Y')){cout << "\ntclipc> "<< flush; continue;}
	}
D 10
	process_cmd(inbuf,timeout);
E 10
I 10
	process_cmd(inbuf);
E 10
E 9
      }
D 9

E 7
      process_cmd(inbuf,timeout);
E 9
      cout << "\ntclipc> "<< flush;
E 19
I 19
    // verify input of quit,stop,exit
    if( (strncasecmp(inbuf,"quit",4)==0) || (strncasecmp(inbuf,"exit",4)==0) || 
	(strncasecmp(inbuf,"stop",4)==0) ) {
      cout << "\nAre you sure you want the remote procss to quit [y or n]: ";
      cin >> ans;
      if((ans[0]!='y')&&(ans[0]!='Y')){cout << "\ntclipc> "<< flush; continue;}
E 19
    }
E 4
D 19

D 4
      // get command...note that last char is newline
      fgets(command,1024,stdin);
      command[count-1]='\0';
E 4
I 4
    // check for ipc messages
    ipc_check(0.10);
E 19
I 19
    process_cmd(inbuf);
    ipc_check(0.0);
    cout << "\ntclipc> "<< flush;
E 19
  }
I 19

E 19
}
E 4

D 4
      // gobble up leading whitespace
      for(startcmd=command; (startcmd[0]!='\0')&&((startcmd[0]==' ')||(startcmd[0]=='\t')); startcmd++);
      
      // create message
E 4
I 4

//---------------------------------------------------------------------


I 19
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


E 19
D 10
void process_cmd(char *cmd, float timeout){
E 10
I 10
void process_cmd(char *cmd){
E 10

D 12
  T_INT4 code;
  T_STR result;
E 12
  char *cmdstart;
  char *msg;


  // get pointer to 1st non-blank char
  for(cmdstart=cmd; (cmdstart[0]!='\0')&&((cmdstart[0]==' ')||(cmdstart[0]=='\t')); cmdstart++);


  // handle different message types, internal and external
D 6
  if((strncmp(cmdstart,"@file",5)==0)||(cmdstart[0]!='@')){       // external commmand 
E 6
I 6
  if((strncasecmp(cmdstart,"@file",5)==0)||(cmdstart[0]!='@')){       // external commmand 
E 6

    // get message text
    msg=get_message(cmdstart);

D 16
    // create message, fill destination, fill contents, send, print reply
E 16
I 16
    // create message, fill destination, fill contents, send, print replies
E 16
D 18
    if(msg!=NULL){
E 18
I 18
    if(msg!=NULL) {
E 18

E 4
D 19
      TipcMsg message("tcl_request");
E 19
I 19
      TipcMsg message((char*)"tcl_request");
E 19
D 4
    
      // fill destination
      message.Dest(dest);
      
      // append tcl command...check for redirect to file
      if(startcmd[0]!='@'){
	message << startcmd;
      } else {
D 3
	stat(startcmd+1,&buf);
	ifstream file;
	file.open(startcmd+1);
	file_contents = new char[buf.st_size];
	file.read(file_contents,buf.st_size);
	message << file_contents << '\0';
	file.close();
E 3
I 3
	if(access(startcmd+1,R_OK)==0){
	  stat(startcmd+1,&buf);
	  ifstream file;
	  file.open(startcmd+1);
	  file_contents = new char[buf.st_size];
	  file.read(file_contents,buf.st_size);
	  message << file_contents << '\0';
	  file.close();
	} else {
	  cout << "\n?no such file: " << startcmd+1 << endl;
	  cout << "\ntclipc > "<< flush;
	  continue;
	}
E 3
      }
      
      // send message
E 4
I 4
      message.Dest(process);
      message << msg;
E 4
D 10
      TipcMsg reply=server.SendRpc(message,T_REAL8(timeout));
E 10
I 10
D 18
      TipcMsg reply=server.SendRpc(message,(T_REAL8)timeout);
E 10
D 4
      
      // print results
E 4
D 16
      if(reply){
	reply >> code >> result;
	if(code==0){
	  cout << result << endl;
E 16
I 16
      if(reply) {
D 17
	reply >> code;
	if(code==0) {
	  for(int i=1; i<reply.NumFields(); i++) {
	    reply >> result; cout << result << endl;
E 17
I 17
	for(int i=0; i<reply.NumFields(); i+=2) {
	  reply >> code >> result;
	  if(code==0) {
	    cout << result << endl;
	  } else {
	    cout << "\n?Return code: " << code << endl << result << endl;
E 17
	  }
E 16
D 17
	} else {
I 16
	  reply >> result;
E 16
D 4
	  cout << "\n?Return code: " << code << endl;
	  cout << "?" << result << endl;
E 4
I 4
	  cout << "\n?Return code: " << code << endl << result << endl;
E 4
	}
      } 
      else {
E 17
I 17
	} 
E 18
I 18

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

E 18
      } else {
E 17
D 18
	cout << "\nNo reply...Timed out after " << timeout << " seconds" << endl << endl;
I 13
	code=-1;
E 18
I 18
D 19
	result="0";
E 19
I 19
	result=(char*)"0";
E 19
	server.Send(message);
	server.Flush();
E 18
E 13
      }
I 18

E 18
D 4
      
D 3
      // another prompt
E 3
I 3
      // prompt
E 3
      cout << "\ntclipc > "<< flush;
E 4
I 4
      free(msg);
    }
E 4
I 3

I 4

  } else {                                     // internal command
    if(strncasecmp(cmdstart+1,"help",4)==0){
      cout << help2 << endl;
    }else if(strncasecmp(cmdstart+1,"timeout",7)==0){
D 10
      sscanf(cmdstart+7,"%f",timeout);
E 10
I 10
      sscanf(cmdstart+9,"%f",&timeout);
E 10
    }else if(strncasecmp(cmdstart+1,"quit",4)==0){
D 11
      exit(0);
E 11
I 11
      ipc_close();
      exit(EXIT_SUCCESS);
E 11
    }else if(strncasecmp(cmdstart+1,"stop",4)==0){
D 11
      exit(0);
E 11
I 11
      ipc_close();
      exit(EXIT_SUCCESS);
E 11
    }else if(strncasecmp(cmdstart+1,"exit",4)==0){
D 11
      exit(0);
E 11
I 11
      ipc_close();
      exit(EXIT_SUCCESS);
E 11
    } else {
      cout << "?Unknown command " << cmdstart << endl << endl;
E 4
E 3
    }
D 4
    
    // check for ipc traffic and sleep for a while
    ipc_check(0.10);
E 4
I 4
  }
E 4

I 4
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
E 4
  }

I 4
  return (msg);

E 4
}

I 4

E 4
//---------------------------------------------------------------------


void decode_command_line(int argc, char**argv){


  // error if no command line args
D 4
  if(argc<2){TutOut("%s", help); exit(0); }
E 4
I 4
D 18
  if(argc<2){cout <<  help1 <<endl; exit(0); }
E 18
I 18
  if(argc<2){cout <<  help1 << endl; exit(0);}
E 18
E 4
  
  
D 18
  // decode command line...loop over all arguments, except the 1st (which is program name)
E 18
I 18
  // decode command line
E 18
  int i=1;
  while(i<argc) {
D 18
    if(strncasecmp(argv[i],"-h",2)==0){
E 18
I 18

    if(strncasecmp(argv[i],"-h",2)==0) {
E 18
D 4
      TutOut("%s", help);
E 4
I 4
      cout << help1 << endl;
E 4
      exit(0);
    }
D 18
    else if (strncasecmp(argv[i], "-", 1)!= 0)
E 18
I 18
    else if (strncasecmp(argv[i], "-", 1)!= 0) {
E 18
D 4
      break;   // reached destination field
E 4
I 4
      break;   // reached process name field
E 4
D 18
    
    else if (strncasecmp(argv[i],"-a",2)==0){
E 18
I 18
    }    
    else if (strncasecmp(argv[i],"-async",6)==0) {
      async=1;
      i=i+1;
    }
    else if (strncasecmp(argv[i],"-a",2)==0) {
E 18
D 19
      free(appl);
E 19
I 19
      //      free(appl)
E 19
      appl=strdup(argv[i+1]);
      i=i+2;
    }
    else if (strncasecmp(argv[i],"-t",2)==0) {
      sscanf(argv[i+1],"%f",&timeout);
      i=i+2;
    }
I 4
    else if (strncasecmp(argv[i],"-c",2)==0) {
      one_cmd=strdup(argv[i+1]);
      i=i+2;
    }
I 12
    else if (strncasecmp(argv[i],"-exit_status",12)==0) {
      exit_status=1;
      i=i+1;
    }
E 12
E 4
    else if (strncasecmp(argv[i],"-",1)==0) {
D 4
      TutWarning("Unknown command line arg: %s %s\n\n",argv[i],argv[i+1]);
E 4
I 4
      cout << "Unknown command line arg: " << argv[i] << argv[i+1] << endl << endl;
E 4
      i=i+2;
    }
  }

D 4
  // set destination
  dest=argv[i];
E 4
I 4
  // set process name
D 5
  process=argv[i];
E 5
I 5
  if(i>argc){
    cout << "?no process specified" << endl << endl;
    exit(0);
  } else {
    process=argv[i];
  }
E 5
E 4

}

  
//----------------------------------------------------------------
E 1
