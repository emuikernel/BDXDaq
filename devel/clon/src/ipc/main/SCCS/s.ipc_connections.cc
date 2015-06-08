h46177
s 00002/00001/00138
d D 1.6 07/10/11 21:45:52 boiarino 7 6
c *** empty log message ***
e
s 00007/00000/00132
d D 1.5 02/08/16 10:23:09 wolin 6 5
c license.cm
e
s 00000/00004/00132
d D 1.4 02/08/13 13:25:52 wolin 5 4
c Removed TutCommandParseStdLicense()
c 
e
s 00001/00001/00135
d D 1.3 00/10/27 10:08:14 wolin 4 3
c Format mods
e
s 00001/00000/00135
d D 1.2 00/08/30 14:21:24 wolin 3 1
c New CC
e
s 00000/00000/00000
d R 1.2 98/06/08 15:48:44 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 ipcutil/s/ipc_connections.cc
e
s 00135/00000/00000
d D 1.1 98/06/08 15:48:43 wolin 1 0
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
//  ipc_connections
//
//  lists ipc client connections
//
//
//     ipc_connections  [-a application]
//
//       default application     = "clastest"
//
//  ejw, 15-may-98




#define _POSIX_SOURCE 1
#define __EXTENSIONS__


#include <rtworks/cxxipc.hxx>
#include <stdio.h>
I 6
D 7
#include <strstream.h>
E 7
I 7
using namespace std;
#include <strstream>
E 7
E 6
I 3
#include <iostream.h>
E 3
#include <iomanip.h>
#include <clas_ipc_prototypes.h>


// misc vars
char *app  = "clastest";
char *help = "\nusage:\n\n   ipc_connections [-a application] \n\n\n";


// prototypes
void decode_command_line(int argc, char **argv);


//-------------------------------------------------------------------------


int main(int argc, char **argv) {

  T_STR *names;
  T_STR *info;
  T_STR *serv;
  T_INT4 nclient;

  
  // decode command line
  decode_command_line(argc,argv);
  

  // inhibit output
  //  TutSetOutputFunc(ipc_output_dummy);


I 6
  // read Smartsockets license file
  strstream s;
  s << getenv("RTHOME") << "/standard/license.cm" << ends;
  TutCommandParseFile(s.str());


E 6
D 5
  // read Smartsockets license file
  TutCommandParseStdLicense();


E 5
  // set application
  T_OPTION opt=TutOptionLookup("Application");
  if(!TutOptionSetEnum(opt,app)){TutOut("?unable to set application\n");}


  // connect to server 
  TipcSrv &server=TipcSrv::InstanceCreate(T_IPC_SRV_CONN_FULL);


  // reenable output
  TutSetOutputFunc(NULL);


  // poll
  TipcMon mon;
  mon.ClientNamesPoll();


  // get result
  TipcMsg msg = server.Next(1.0);

  
  // unpack msg
  msg >> names >> GetSize(&nclient) >> info >> GetSize(&nclient) >> serv >> GetSize(&nclient);


  // dump results
  for(int i=0; i<nclient; i++) {
    *strchr(serv[i]+2,'_')='\0';
D 4
    cout << setw(30) << names[i]+1 << "  " << setw(20) << serv[i]+2 << endl;
E 4
I 4
    cout << setw(45) << names[i]+1 << "  " << setw(15) << serv[i]+2 << endl;
E 4
  }


  exit(EXIT_SUCCESS);
}


//-------------------------------------------------------------------------


void decode_command_line(int argc, char **argv) {


  int i=1;
  while(i<argc) {
    if(strncasecmp(argv[i],"-h",2)==0) {
      TutOut("%s", help);
      exit(EXIT_SUCCESS);
    }
    else if (strncasecmp(argv[i],"-a",2)==0){
      app=strdup(argv[i+1]);
      i=i+2;
    }
    else if (strncasecmp(argv[i],"-",1)==0) {
      TutWarning("Unknown command line arg: %s\n\n",argv[i]);
      i=i+1;
    }
    else {
      TutOut("%s", help);
      exit(EXIT_SUCCESS);
    }
  }

  return;
}


//-------------------------------------------------------------------------


void dummy(T_STR str, va_list arg) {

  return;

}


//-------------------------------------------------------------------
E 1
