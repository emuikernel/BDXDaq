h11162
s 00002/00001/00193
d D 1.14 07/10/11 21:45:03 boiarino 15 14
c *** empty log message ***
e
s 00007/00000/00187
d D 1.13 02/08/16 10:31:21 wolin 14 13
c license.cm
e
s 00000/00004/00187
d D 1.12 02/08/13 13:27:03 wolin 13 12
c Removed TutCommandParseStdLicense()
c 
e
s 00002/00001/00189
d D 1.11 00/01/21 13:16:54 wolin 12 11
c Typo in info_server message
e
s 00026/00013/00164
d D 1.10 00/01/21 13:10:56 wolin 11 10
c Added info_server message
e
s 00001/00001/00176
d D 1.9 00/01/14 12:12:47 wolin 10 9
c No more dd_name
e
s 00003/00015/00174
d D 1.8 98/01/22 12:04:44 wolin 9 8
c Env vars
c 
e
s 00027/00010/00162
d D 1.7 97/06/12 15:02:05 wolin 8 7
c Added file option
e
s 00015/00023/00157
d D 1.6 97/05/20 16:57:14 wolin 7 6
c Now gets run,config from msql
e
s 00004/00000/00176
d D 1.5 97/04/18 13:33:52 wolin 6 5
c Added default session = clasprod
e
s 00041/00010/00135
d D 1.4 97/04/18 13:31:08 wolin 5 4
c Added  run,session,config to message
e
s 00010/00005/00135
d D 1.3 97/03/25 12:31:23 wolin 4 3
c Got rid of message type numbers
e
s 00003/00003/00137
d D 1.2 97/03/04 17:38:35 wolin 3 1
c Default dest now run_control
e
s 00000/00000/00000
d R 1.2 97/03/04 12:15:35 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 2 1 ipcutil/s/rc_transition.cc
c Name history : 1 0 s/rc_transition.cc
e
s 00140/00000/00000
d D 1.1 97/03/04 12:15:34 wolin 1 0
c Broadcasts run_control message upon run control transition
e
u
U
f e 0
t
T
I 1
//
//  rc_transition
//
D 11
//    Sends out Smartsockets run_control message upon run control transitions
E 11
I 11
//    Sends out Smartsockets run_control and info_server messages upon run control transitions
E 11
I 5
//      format is: transition,run,config,etc. (e.g. pause 123 clasrun ITAL_CAL ...)
E 5
//
I 8
//    If -file flag set, also writes current state to rc status file
E 8
//
I 8
D 9
//
//
E 8
//  Usage:
//
D 5
//     rc_transition [-a application] [-d destination] msgfield1 msgfield2 ...
E 5
I 5
D 7
//     rc_transition [-a application] [-d destination] [-s session] [-f current_run_file] msgfield1 msgfield2 ...
E 7
I 7
//     rc_transition [-a application] [-d destination] [-s session]
D 8
//                   [-m msql_database] transition
E 8
I 8
//                   [-m msql_database]  [-file] [-fname rcfile_name] transition
E 8
E 7
E 5
//
D 5
//       default application     = "clastest"
D 3
//       default destination     = "clastest"
E 3
I 3
//       default destination     = "run_control"
E 5
E 3
//
E 9
D 8
//  Important:
//       Message text fields MUST come after ALL command-line args!
//
//
E 8
//  Example:
//
//     to send run_control message "pause" to group "clasprod" in application "clasprod":
//
//           rc_transition -a clasprod -d clasprod pause
//
//
//  Notes:
//     doesn't check command-line syntax vary carefully
I 5
D 7
//     add'l msg fields appear AFTER run,config
E 7
E 5
//
//
D 7
//  ejw, 4-mar-97
E 7
I 7
D 8
//  ejw, 20-may-97
E 8
I 8
D 11
//  ejw, 12-jun-97
E 11
I 11
//  ejw, 21-jan-00
E 11
E 8
E 7



#define _POSIX_SOURCE 1
#define __EXTENSIONS__

#include <rtworks/cxxipc.hxx>
I 5
D 7
#include <fstream.h>
E 7
#include <stdlib.h>
I 14
D 15
#include <strstream.h>
E 15
I 15
using namespace std;
#include <strstream>
E 15
E 14
I 8
#include <fstream.h>
E 8
E 5

D 5
char *app  = "clastest";
D 3
char *dest = "clastest";
E 3
I 3
char *dest = "run_control";
E 3
char *help = "\nusage:\n\n   rc_transition [-a application] [-d destination]  msgfield1 msgfield2 ...\n\n\n";
E 5
I 5

char *app  		= "clastest";
char *dest 		= "run_control";
D 10
char *session           = getenv("DD_NAME");
E 10
I 10
char *session           = NULL;
E 10
D 7
char *current_run_file  = "/usr/local/clas/parms/run_log/current_run_%s.txt";
E 7
I 7
char *msql_database     = "clasrun";
I 8
int rcfile              = 0;
D 9
char *rcfile_name       = "/usr/local/clas/parms/run_log/rcstate_%s.txt";
E 9
I 9
char *rcfile_name       = "%s/run_log/rcstate_%s.txt";
E 9
E 8
E 7
D 11
char *help = "\nusage:\n\n   rc_transition [-a application] [-d destination] [-s session]"
D 7
             " [-f current_run_file]  msgfield1 msgfield2 ...\n\n\n";
E 5
int first_msgfield = 0;
E 7
I 7
D 8
             " [-m msql_database] transition \n\n\n";
E 8
I 8
D 9
             " [-m msql_database] [-file] [-fname rcfile_name] transition \n\n\n";
E 9
I 9
             " [-m msql_database] [-file] transition \n\n\n";
E 11
E 9
E 8
char *transition;
I 11
char temp[128];
E 11
E 7


// prototypes
void decode_command_line(int argc, char **argv);
I 4
extern "C"{
int init_msg_types(void);
I 7
void get_run_config(char *msql_database, char *session, int *run, char **config);
E 7
}
E 4

I 11

E 11
D 4

E 4
//------------------------------------------------------------------------------------


D 11
int main(int argc, char **argv)
{
E 11
I 11
int main(int argc, char **argv) {
E 11
  
I 5
  int run;
D 7
  char config[40];
E 7
I 7
  char *config;
E 7
D 8
  char line[120];
  char space = ' ';
E 8
  char filename[120];


E 5
  // decode command line args
  decode_command_line(argc,argv);

I 5

I 6
  // set session name if not specified via env variable or on command line
  if(session==NULL)session="clasprod";


E 6
D 7
  // read run,config from current run file for this session
  sprintf(filename,current_run_file,session);
  ifstream file(filename);
  file.getline(line,sizeof(line));
  file.close();
  sscanf(line,"%d %s",&run,config);
E 7
I 7
  // get run and config
  get_run_config(msql_database, session, &run, &config);
E 7


I 14
  // read Smartsockets license file
  strstream s;
  s << getenv("RTHOME") << "/standard/license.cm" << ends;
  TutCommandParseFile(s.str());


E 14
E 5
D 13
  // read Smartsockets license file
  TutCommandParseStdLicense();

I 11

E 13
E 11
  // set application
  T_OPTION opt=TutOptionLookup("Application");
  if(!TutOptionSetEnum(opt,app)){TutOut("?unable to set application\n");}

I 11

E 11
  // connect to server 
  TipcSrv &server=TipcSrv::InstanceCreate(T_IPC_SRV_CONN_FULL);

I 11

E 11
D 4
  // create message type
  TipcMt mt("run_control",12,"verbose");
E 4
I 4
  // create message types
  init_msg_types();
E 4

D 4
  // create message
D 3
  TipcMsg message(12,dest,"rt_transition");
E 3
I 3
  TipcMsg message(12,dest,"rc_transition");
E 4
I 4
D 11
  // create message, fill sender and destination
E 11
I 11

  // run_control message
E 11
  TipcMsg message("run_control");
  message.Sender("rc_transition");
  message.Dest(dest);
E 4
E 3
D 11

I 4
D 7

E 7
E 4
D 5
  // build message from remaining fields in command line
    for(int i=first_msgfield; i<argc; i++) {
      message << argv[i];
    }
E 5
I 5
  // build message 
E 11
D 7
  message << argv[first_msgfield] << space << (T_INT4) run << space << session << space << config;
  for(int i=first_msgfield+1; i<argc; i++) {
    message << argv[i];
  }
E 7
I 7
  message << transition << (T_INT4) run << session << config;
E 7
E 5
D 11

  // send and flush message
E 11
  server.Send(message);
I 11


  // info_server message
  TipcMsg msg2("info_server");
  msg2.Sender("rc_transition");
  sprintf(temp,"info_server/in/run_control/%s",session);  
  msg2.Dest(temp);
D 12
  msg2 << "run_control" << (T_INT4) run << config << transition;
E 12
I 12
  sprintf(temp,"run_control/%s",session);  
  msg2 << temp << (T_INT4) run << config << transition;
E 12
  server.Send(msg2);


  // flush messages
E 11
  server.Flush();
I 11

E 11
  
  // close connection
  server.Destroy(T_IPC_SRV_CONN_NONE);

I 11

E 11
I 8
  // write state to file
  if(rcfile!=0){
D 9
    sprintf(filename,rcfile_name,session);
E 9
I 9
    sprintf(filename,rcfile_name,getenv("CLON_PARMS"),session);
E 9
    ofstream file(filename);
    file << transition << " " << run << " " << session << " " << config << endl;
    file.close();
  }
D 11


E 11
E 8
}


//-----------------------------------------------------------------------------


void decode_command_line(int argc, char **argv){

I 11
    char *help = "\nusage:\n\n   rc_transition [-a application] [-d destination] [-s session]"
             " [-m msql_database] [-file] transition \n\n\n";
E 11
  int i=1;


  // print help if no command line args
  if(argc<2){TutOut("%s", help); exit(EXIT_SUCCESS); }

  // stop if no message specified
  if(i>=argc){TutWarning("No message specified\n"); exit(EXIT_FAILURE);}


  // decode command line...loop over all arguments, except the 1st (which is program name)
  while(i<argc) {
    if(strncasecmp(argv[i],"-h",2)==0){
      TutOut("%s", help);
      exit(EXIT_SUCCESS);
    }
    else if (strncasecmp(argv[i], "-", 1)!= 0)
      break;   // reached 1st msg field

    else if (argc!=(i+1)) {
D 8
      if (strncasecmp(argv[i],"-a",2)==0){
E 8
I 8
      if (strncasecmp(argv[i],"-file",5)==0){
	rcfile=1;
	i=i+1;
      }
D 9
      else if (strncasecmp(argv[i],"-fname",6)==0){
	rcfile_name=strdup(argv[i+1]);
	i=i+2;
      }
E 9
      else if (strncasecmp(argv[i],"-a",2)==0){
E 8
	app=strdup(argv[i+1]);
	i=i+2;
      }
      else if (strncasecmp(argv[i],"-d",2)==0){
	dest=strdup(argv[i+1]);
	i=i+2;
      }
I 5
      else if (strncasecmp(argv[i],"-s",2)==0){
	session=strdup(argv[i+1]);
	i=i+2;
      }
D 7
      else if (strncasecmp(argv[i],"-f",2)==0){
	current_run_file=strdup(argv[i+1]);
E 7
I 7
      else if (strncasecmp(argv[i],"-m",2)==0){
	msql_database=strdup(argv[i+1]);
E 7
	i=i+2;
      }
E 5
      else if (strncasecmp(argv[i],"-",1)==0) {
	TutWarning("Unknown command line arg: %s\n\n",argv[i]);
	i=i+1;
      }
    }
    else {
      TutOut("%s", help);
      exit(EXIT_SUCCESS);
    }
  }
  
D 7
  // set pointer to first msg field
  first_msgfield=i;
E 7
I 7
  // get transition
  transition=strdup(argv[i]);
E 7

  return;

}  


//-----------------------------------------------------------------------------
E 1
