//  error_msg
//
//   sends error message using insert_msg (ipc version)
//
//  Usage:
//
//   error_msg name facility process msgclass severity status code text
//
//
//  ejw, 28-may-99


// for posix
#define _POSIX_SOURCE_ 1
#define __EXTENSIONS__


// system stuff
#include <stdlib.h>
#include <strings.h>
#include <iostream.h>


// for smartsockets
#include <rtworks/cxxipc.hxx>


// for CLAS ipc
#include <clas_ipc_prototypes.h>


// misc variables
static char *application = (char*)"clasprod";
static int argcount      = 8;           // number of command line args required 
static int dataptr;
static int debug         = 0;


// prototypes
void decode_command_line(int argc, char **argv);
extern "C"{
int insert_msg(char *name, char *facility, char *process, char *msgclass, 
	       int severity, char *status, int code, char *text);
}


//----------------------------------------------------------------------


int main(int argc, char **argv) {

  int status;


  // decode command line (global dataptr = pointer to first data field on command line)
  decode_command_line(argc,argv);


  // init ipc
  TutSetOutputFunc(dbr_output_dummy);
  dbr_init(NULL,application,"error_msg");


  // insert message 
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


  // check for error
  if(status!=0) {
    cerr << "?unable to insert error message, status is: " << status << endl;
    exit(EXIT_FAILURE);
  }
  

  // done
  dbr_close();
  exit(EXIT_SUCCESS);

}


//----------------------------------------------------------------------


void decode_command_line(int argc, char **argv){

  const char *help = "\nusage:\n\n   error_msg [-a application] name facility process class severity status code text";

  int i=1;
  while(i<argc) {
    if(strncasecmp(argv[i],"-h",2)==0){
      cout << help << endl << endl;
      exit(EXIT_SUCCESS);

    } else if(strncasecmp(argv[i],"-a",2)==0) {
      application=strdup(argv[i+1]);
      i=i+2;

    } else if(strncasecmp(argv[i],"-debug",6)==0) {
      debug=1;
      i=i+1;

    } else if (strncasecmp(argv[i],"-", 1) != 0) {
      break;  // reached msg field

    }  else {
      cout << help << endl << endl; 
      exit(EXIT_SUCCESS);
    } 
  }
  

  // error if no command line args
  if(argc<2){ cout << help << endl << endl; exit(EXIT_SUCCESS);}
  

  // set pointer to 1st data field on command line
  dataptr=i;


  // error if not enough args
  if((argc-dataptr)<argcount){cerr << "\n   ?too few arguments...use -h flag for more info" << endl << endl; exit(EXIT_FAILURE);}

  return;
}


//----------------------------------------------------------------------


