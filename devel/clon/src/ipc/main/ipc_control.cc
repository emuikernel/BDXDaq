//
//  ipc_control
//
//  Sends out Smartsockets control message using command-line interface
//
//
//  Usage:
//
//     ipc_control [-a application] [-d destination]  msgfield1 msgfield2 ...
//
//       default application     = "clastest"
//       default destination     = same as application
//
//
//   Important:
//
//     Message text fields MUST come after ALL command-line args!
//
//
//
//  Examples:
//
//     1. to send control message "mymsg" to process "myproc" in application "myapp":
//
//           ipc_control -a myapp -d mydest mymsg
//
//     2. to send a "quit" message to all processes in the "clastest" application:
//
//           ipc_control quit
//
//
//  Notes:
//
//     doesn't check command-line syntax vary carefully
//
//
//  To link:
//
//      rtlink -cxx -g -o ipc_control ipc_control.cc
//
//
//  ejw, 30-may-96
//
//

#define _POSIX_SOURCE 1
#define __EXTENSIONS__

#include <rtworks/cxxipc.hxx>
using namespace std;
#include <strstream>
#include <iostream.h>
#include <iomanip.h>


char *app  = "clastest";
char *dest = NULL;
char *help = "\nusage:\n\n   ipc_control [-a application] [-d destination]  msgfield1 msgfield2 ...\n\n\n";

int main(int argc, char **argv) {
  
  // error if no command line args
  if(argc<2){
    TutOut("%s", help); exit(0); }
  

  // decode command line...loop over all arguments, except the 1st (which is program name)
  int i=1;
  while(i<argc) {
    if(strncasecmp(argv[i],"-h",2)==0){
      TutOut("%s", help);
      exit(0);
    }
    else if (strncasecmp(argv[i], "-", 1)!= 0)
      break;   // reached 1st msg field

    else if (argc!=(i+1)) {
      if (strncasecmp(argv[i],"-a",2)==0){
	app=strdup(argv[i+1]);
	i=i+2;
      }
      else if (strncasecmp(argv[i],"-d",2)==0){
	dest=strdup(argv[i+1]);
	i=i+2;
      }
      else if (strncasecmp(argv[i],"-",1)==0) {
	TutWarning("Unknown command line arg: %s\n\n",argv[i]);
	i=i+1;
      }
    }
    else {
      TutOut("%s", help);
      exit(0);
    }
  }
  
  
  // stop if no message specified
  if(i>=argc){TutWarning("No message specified\n"); exit(0);}

  // default destination is same as application, if application not specified
  if(dest==NULL)dest=strdup(app);

  // read Smartsockets license file
  strstream s;
  s << getenv("RTHOME") << "/standard/license.cm" << ends;
  TutCommandParseFile(s.str());


  // set application
  T_OPTION opt=TutOptionLookup("Application");
  if(!TutOptionSetEnum(opt,app)){TutOut("?unable to set application\n");}

  // connect to server 
  TipcSrv &server=TipcSrv::InstanceCreate(T_IPC_SRV_CONN_FULL);

  // create message
  TipcMsg message(T_MT_CONTROL);

  // fill destination
  message.Dest(dest);

  // build message from remaining fields in command line
    for(int j=i; j<argc; j++) {
      message << argv[j];
    }

  // send message
  server.Send(message);

  // flush message
  server.Flush();
  
  // close connection
  server.Destroy(T_IPC_SRV_CONN_NONE);

}

