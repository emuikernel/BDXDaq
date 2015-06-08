//
//  rc_transition
//
//    Sends out Smartsockets run_control and info_server messages upon run control transitions
//      format is: transition,run,config,etc. (e.g. pause 123 clasrun ITAL_CAL ...)
//
//    If -file flag set, also writes current state to rc status file
//
//  Example:
//
//     to send run_control message "pause" to group "clasprod" in application "clasprod":
//
//           rc_transition -a clasprod -d clasprod pause
//
//
//  Notes:
//     doesn't check command-line syntax vary carefully
//
//
//  ejw, 21-jan-00



#define _POSIX_SOURCE 1
#define __EXTENSIONS__

#include <rtworks/cxxipc.hxx>
#include <stdlib.h>
using namespace std;
#include <strstream>
#include <fstream.h>


char *app  		= "clastest";
char *dest 		= "run_control";
char *session           = NULL;
char *msql_database     = "clasrun";
int rcfile              = 0;
char *rcfile_name       = "%s/run_log/rcstate_%s.txt";
char *transition;
char temp[128];


// prototypes
void decode_command_line(int argc, char **argv);
extern "C"{
int init_msg_types(void);
void get_run_config(char *msql_database, char *session, int *run, char **config);
}


//------------------------------------------------------------------------------------


int main(int argc, char **argv) {
  
  int run;
  char *config;
  char filename[120];


  // decode command line args
  decode_command_line(argc,argv);


  // set session name if not specified via env variable or on command line
  if(session==NULL)session="clasprod";


  // get run and config
  get_run_config(msql_database, session, &run, &config);


  // read Smartsockets license file
  strstream s;
  s << getenv("RTHOME") << "/standard/license.cm" << ends;
  TutCommandParseFile(s.str());


  // set application
  T_OPTION opt=TutOptionLookup("Application");
  if(!TutOptionSetEnum(opt,app)){TutOut("?unable to set application\n");}


  // connect to server 
  TipcSrv &server=TipcSrv::InstanceCreate(T_IPC_SRV_CONN_FULL);


  // create message types
  init_msg_types();


  // run_control message
  TipcMsg message("run_control");
  message.Sender("rc_transition");
  message.Dest(dest);
  message << transition << (T_INT4) run << session << config;
  server.Send(message);


  // info_server message
  TipcMsg msg2("info_server");
  msg2.Sender("rc_transition");
  sprintf(temp,"info_server/in/run_control/%s",session);  
  msg2.Dest(temp);
  sprintf(temp,"run_control/%s",session);  
  msg2 << temp << (T_INT4) run << config << transition;
  server.Send(msg2);


  // flush messages
  server.Flush();

  
  // close connection
  server.Destroy(T_IPC_SRV_CONN_NONE);


  // write state to file
  if(rcfile!=0){
    sprintf(filename,rcfile_name,getenv("CLON_PARMS"),session);
    ofstream file(filename);
    file << transition << " " << run << " " << session << " " << config << endl;
    file.close();
  }
}


//-----------------------------------------------------------------------------


void decode_command_line(int argc, char **argv){

    char *help = "\nusage:\n\n   rc_transition [-a application] [-d destination] [-s session]"
             " [-m msql_database] [-file] transition \n\n\n";
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
      if (strncasecmp(argv[i],"-file",5)==0){
	rcfile=1;
	i=i+1;
      }
      else if (strncasecmp(argv[i],"-a",2)==0){
	app=strdup(argv[i+1]);
	i=i+2;
      }
      else if (strncasecmp(argv[i],"-d",2)==0){
	dest=strdup(argv[i+1]);
	i=i+2;
      }
      else if (strncasecmp(argv[i],"-s",2)==0){
	session=strdup(argv[i+1]);
	i=i+2;
      }
      else if (strncasecmp(argv[i],"-m",2)==0){
	msql_database=strdup(argv[i+1]);
	i=i+2;
      }
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
  
  // get transition
  transition=strdup(argv[i]);

  return;

}  


//-----------------------------------------------------------------------------
