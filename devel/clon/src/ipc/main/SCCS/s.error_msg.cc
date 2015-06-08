h58983
s 00000/00008/00140
d D 1.10 02/10/02 15:07:09 wolin 11 10
c Removed thread stuff, not needed
e
s 00010/00002/00138
d D 1.9 02/09/19 14:03:23 wolin 10 9
c Init thread package
e
s 00001/00001/00139
d D 1.8 99/06/01 16:03:50 wolin 9 8
c Default is clasprod
c 
e
s 00047/00012/00093
d D 1.7 99/05/18 15:05:48 wolin 8 7
c ipc version now working
c 
e
s 00002/00002/00103
d D 1.6 97/05/23 11:30:37 wolin 7 6
c Now using text tag instead of message tag
e
s 00002/00002/00103
d D 1.5 97/04/28 15:10:39 wolin 6 5
c Typo
c 
e
s 00006/00006/00099
d D 1.4 97/04/28 14:07:16 wolin 5 4
c Added name
c 
e
s 00005/00005/00100
d D 1.3 97/04/24 15:21:09 wolin 4 3
c Added new status field, modified severity
e
s 00002/00002/00103
d D 1.2 97/03/27 16:15:59 wolin 3 1
c New insert_msg has name arg
e
s 00000/00000/00000
d R 1.2 97/03/25 09:58:28 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 2 1 ipcutil/s/error_msg.cc
c Name history : 1 0 s/error_msg.cc
e
s 00105/00000/00000
d D 1.1 97/03/25 09:58:27 wolin 1 0
c Inserts message into coda message logging system via command line interface
e
u
U
f e 0
t
T
I 1
D 8
//
E 8
//  error_msg
//
D 8
//   Sends error message to CODA message facility using command line interface
E 8
I 8
//   sends error message using insert_msg (ipc version)
E 8
//
//  Usage:
//
D 5
//   error_msg facility process msgclass severity code message
E 5
I 5
D 6
//   error_msg name facility process msgclass severity code message
E 6
I 6
D 7
//   error_msg name facility process msgclass severity status code message
E 7
I 7
//   error_msg name facility process msgclass severity status code text
E 7
E 6
E 5
//
//
D 5
//  ejw, 24-mar-97
E 5
I 5
D 8
//  ejw, 28-apr-97
E 8
I 8
//  ejw, 28-may-99
E 8
E 5


// for posix
#define _POSIX_SOURCE_ 1
#define __EXTENSIONS__


// system stuff
#include <stdlib.h>
#include <strings.h>
#include <iostream.h>
I 10
D 11
#include <pthread.h>
E 11
E 10


I 8
// for smartsockets
#include <rtworks/cxxipc.hxx>


// for CLAS ipc
#include <clas_ipc_prototypes.h>


E 8
// misc variables
D 5
static int argcount = 6;    // number of command line args required 
E 5
I 5
D 6
static int argcount = 7;    // number of command line args required 
E 6
I 6
D 8
static int argcount = 8;    // number of command line args required 
E 8
I 8
D 9
static char *application = "clastest";
E 9
I 9
D 10
static char *application = "clasprod";
E 10
I 10
static char *application = (char*)"clasprod";
E 10
E 9
static int argcount      = 8;           // number of command line args required 
E 8
E 6
E 5
static int dataptr;
I 8
static int debug         = 0;
E 8


// prototypes
void decode_command_line(int argc, char **argv);
extern "C"{
D 3
int insert_msg(char *facility, char *process, char *msgclass, char *severity, 
E 3
I 3
D 4
int insert_msg(char *name, char *facility, char *process, char *msgclass, char *severity, 
E 3
		   int code, char *message);
E 4
I 4
int insert_msg(char *name, char *facility, char *process, char *msgclass, 
D 7
	       int severity, char *status, int code, char *message);
E 7
I 7
	       int severity, char *status, int code, char *text);
E 7
E 4
}


//----------------------------------------------------------------------


D 8
int main(int argc, char **argv){
E 8
I 8
int main(int argc, char **argv) {
E 8

  int status;


  // decode command line (global dataptr = pointer to first data field on command line)
  decode_command_line(argc,argv);


I 8
  // init ipc
  TutSetOutputFunc(dbr_output_dummy);
I 10
D 11
  thr_setconcurrency(thr_getconcurrency()+4);
  if(!TipcInitThreads()) {
    cerr << "Unable to init IPC thread package" << endl;
    exit(EXIT_FAILURE);
  }


E 11
E 10
  dbr_init(NULL,application,"error_msg");


E 8
  // insert message 
D 3
  status=insert_msg(argv[dataptr+0],argv[dataptr+1],argv[dataptr+2],argv[dataptr+3],
E 3
I 3
D 4
  status=insert_msg(argv[0],argv[dataptr+0],argv[dataptr+1],argv[dataptr+2],argv[dataptr+3],
E 3
			atoi(argv[dataptr+4]),argv[dataptr+5]);
E 4
I 4
D 5
  status=insert_msg(argv[0],argv[dataptr+0],argv[dataptr+1],argv[dataptr+2],
		    atoi(argv[dataptr+3]), argv[dataptr+4], atoi(argv[dataptr+5]),argv[dataptr+6]);
E 5
I 5
D 8
  status=insert_msg(argv[dataptr+0],argv[dataptr+1],argv[dataptr+2],argv[dataptr+3],
		    atoi(argv[dataptr+4]), argv[dataptr+5], atoi(argv[dataptr+6]),argv[dataptr+7]);
E 8
I 8
  if(debug==0) {
    status=insert_msg(argv[dataptr+0],argv[dataptr+1],
		      argv[dataptr+2],argv[dataptr+3],
		      atoi(argv[dataptr+4]),argv[dataptr+5],
		      atoi(argv[dataptr+6]),argv[dataptr+7]);
  } else {
    cout << argv[dataptr+0]       << "  " << argv[dataptr+1] << "  " 
	 << argv[dataptr+2]       << "  " << argv[dataptr+3] << "  "
	 << atoi(argv[dataptr+4]) << "  " << argv[dataptr+5] << "  "
	 << atoi(argv[dataptr+6]) << "  " << argv[dataptr+7] << endl;
  }

E 8
E 5
E 4

  // check for error
D 8
  if(status!=0){
E 8
I 8
  if(status!=0) {
E 8
    cerr << "?unable to insert error message, status is: " << status << endl;
    exit(EXIT_FAILURE);
  }
  

  // done
I 8
  dbr_close();
E 8
  exit(EXIT_SUCCESS);

}


//----------------------------------------------------------------------


void decode_command_line(int argc, char **argv){

D 4
  char *help = "\nusage:\n\n   error_msg facility process class severity code text";
E 4
I 4
D 5
  char *help = "\nusage:\n\n   error_msg facility process class severity status code text";
E 5
I 5
D 8
  char *help = "\nusage:\n\n   error_msg name facility process class severity status code text";
E 8
I 8
D 10
  char *help = "\nusage:\n\n   error_msg [-a application] name facility process class severity status code text";
E 10
I 10
  const char *help = "\nusage:\n\n   error_msg [-a application] name facility process class severity status code text";
E 10
E 8
E 5
E 4

  int i=1;
  while(i<argc) {
    if(strncasecmp(argv[i],"-h",2)==0){
      cout << help << endl << endl;
      exit(EXIT_SUCCESS);

D 8
    } else if (strncasecmp(argv[i],"-", 1) != 0){
E 8
I 8
    } else if(strncasecmp(argv[i],"-a",2)==0) {
      application=strdup(argv[i+1]);
      i=i+2;

    } else if(strncasecmp(argv[i],"-debug",6)==0) {
      debug=1;
      i=i+1;

    } else if (strncasecmp(argv[i],"-", 1) != 0) {
E 8
      break;  // reached msg field
D 8
    }
    else {
E 8
I 8

    }  else {
E 8
      cout << help << endl << endl; 
      exit(EXIT_SUCCESS);
    } 
  }
  

  // error if no command line args
  if(argc<2){ cout << help << endl << endl; exit(EXIT_SUCCESS);}
  
I 8

E 8
  // set pointer to 1st data field on command line
  dataptr=i;
I 8

E 8

  // error if not enough args
  if((argc-dataptr)<argcount){cerr << "\n   ?too few arguments...use -h flag for more info" << endl << endl; exit(EXIT_FAILURE);}

  return;
}


//----------------------------------------------------------------------


E 1
