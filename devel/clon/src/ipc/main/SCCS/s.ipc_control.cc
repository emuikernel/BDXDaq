h37649
s 00002/00001/00136
d D 1.7 07/10/11 21:45:29 boiarino 8 7
c *** empty log message ***
e
s 00010/00002/00127
d D 1.6 02/08/16 10:27:30 wolin 7 6
c license.cm
e
s 00000/00003/00129
d D 1.5 02/08/13 13:26:18 wolin 6 5
c Removed TutCommandParseStdLicense()
c 
e
s 00001/00001/00131
d D 1.4 96/08/12 09:56:52 aswin 5 4
c Fixed command line arguments function.
e
s 00003/00003/00129
d D 1.3 96/08/12 09:53:31 aswin 4 3
c Fixed command line arguments function.
e
s 00023/00017/00109
d D 1.2 96/08/09 17:04:10 aswin 3 1
c Fixed command line argument code from core dump.
e
s 00000/00000/00000
d R 1.2 96/07/29 12:56:20 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 2 1 ipcutil/s/ipc_control.cc
c Name history : 1 0 s/ipc_control.cc
e
s 00126/00000/00000
d D 1.1 96/07/29 12:56:19 wolin 1 0
c Sends control message to arbitrary process
e
u
U
f e 0
t
T
I 1
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
I 7
D 8
#include <strstream.h>
E 8
I 8
using namespace std;
#include <strstream>
E 8
#include <iostream.h>
#include <iomanip.h>
E 7


char *app  = "clastest";
char *dest = NULL;
I 3
char *help = "\nusage:\n\n   ipc_control [-a application] [-d destination]  msgfield1 msgfield2 ...\n\n\n";
E 3

D 3

E 3
D 7
int main(int argc, char **argv)
{
E 7
I 7
int main(int argc, char **argv) {
E 7
  
  // error if no command line args
  if(argc<2){
D 3
    TutOut("\nusage:\n\n   ipc_control [-a application] [-d destination] msgfield1 msgfield2 ...\n\n\n"); exit(0); }
E 3
I 3
    TutOut("%s", help); exit(0); }
E 3
  

  // decode command line...loop over all arguments, except the 1st (which is program name)
  int i=1;
  while(i<argc) {
    if(strncasecmp(argv[i],"-h",2)==0){
D 3
      TutOut("\nusage:\n\n   ipc_control [-a application] [-d destination]  msgfield1 msgfield2 ...\n\n\n");
E 3
I 3
      TutOut("%s", help);
E 3
      exit(0);
    }
I 4
D 5
    else if (strncasecmp(agrv[i], "-", 1)!= 0)
E 5
I 5
    else if (strncasecmp(argv[i], "-", 1)!= 0)
E 5
      break;   // reached 1st msg field

E 4
D 3
    else if (strncasecmp(argv[i],"-a",2)==0){
      app=strdup(argv[i+1]);
      i=i+2;
E 3
I 3
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
D 4
      else {
	break;   // reached 1st msg field
      }
E 4
E 3
    }
D 3
    else if (strncasecmp(argv[i],"-d",2)==0){
      dest=strdup(argv[i+1]);
      i=i+2;
    }
    else if (strncasecmp(argv[i],"-",1)==0) {
      TutWarning("Unknown command line arg: %s\n\n",argv[i]);
      i=i+1;
    }
E 3
    else {
D 3
      break;   // reached 1st msg field
E 3
I 3
      TutOut("%s", help);
      exit(0);
E 3
    }
  }
D 3


E 3
I 3
  
  
E 3
  // stop if no message specified
  if(i>=argc){TutWarning("No message specified\n"); exit(0);}

  // default destination is same as application, if application not specified
  if(dest==NULL)dest=strdup(app);

I 7
  // read Smartsockets license file
  strstream s;
  s << getenv("RTHOME") << "/standard/license.cm" << ends;
  TutCommandParseFile(s.str());


E 7
D 6
  // read Smartsockets license file
  TutCommandParseStdLicense();

E 6
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

E 1
