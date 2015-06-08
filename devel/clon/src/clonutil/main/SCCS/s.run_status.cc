h51632
s 00003/00003/00109
d D 1.13 01/04/02 15:00:32 wolin 15 14
c New compiler
e
s 00001/00001/00111
d D 1.12 01/04/02 14:59:35 wolin 14 13
c Bombproofed .eof()
e
s 00001/00001/00111
d D 1.11 01/01/03 10:50:35 wolin 13 12
c is_open()
e
s 00007/00001/00105
d D 1.10 00/10/25 13:40:11 wolin 12 10
c get_run_status still doesn't work
e
s 00003/00003/00103
d D 1.9.1.1 00/08/29 16:37:49 wolin 11 10
c Mods for new CC
e
s 00013/00005/00093
d D 1.9 99/01/26 10:48:53 wolin 10 9
c Getting config from database
c 
e
s 00022/00001/00076
d D 1.8 98/05/21 10:27:07 wolin 9 8
c Added check of msql database
c 
e
s 00023/00030/00054
d D 1.7 98/05/21 10:14:43 wolin 8 7
c Now just reads file
c 
e
s 00004/00005/00080
d D 1.6 97/06/23 17:31:59 wolin 7 6
c Minor improvements
e
s 00004/00003/00081
d D 1.5 97/06/23 17:29:18 wolin 6 5
c Minor improvements
e
s 00015/00004/00069
d D 1.4 97/06/23 13:39:09 wolin 5 4
c run_status also prints run and config
e
s 00012/00001/00061
d D 1.3 97/06/23 11:46:23 wolin 4 3
c Improved output message
e
s 00030/00004/00032
d D 1.2 97/05/20 10:35:42 wolin 3 1
c Uses new cmlog,cdev; other minor mods
e
s 00000/00000/00000
d R 1.2 97/04/09 09:45:06 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 libutil/test/run_status.cc
e
s 00036/00000/00000
d D 1.1 97/04/09 09:45:05 wolin 1 0
c Gets run status
e
u
U
f e 0
t
T
I 1
// run_status

//  gets run status

//  ejw, 9-apr-97


#include <iostream.h>
I 8
#include <fstream.h>
#include <stdio.h>
E 8
#include <stdlib.h>
#include <string.h>


D 5
char *session = getenv("DD_NAME");
E 5
I 5
char *session        = getenv("DD_NAME");
I 9
D 11
D 15
char *msql_database  = "clasrun";
E 15
I 15
char *msql_database  = (char*)"clasrun";
E 15
E 11
I 11
char *msql_database  = (char *)"clasrun";
E 11
E 9
D 8
char *msql_database  = "clasrun";
E 8
E 5

I 9
extern "C" {
int get_run_number(char *msql_database, char *session);
I 10
void get_run_config(char *msql_database, char *session, int *run, char **config);
I 12
int get_run_status(char *session);
E 12
E 10
}

E 9
D 8

extern "C"{
int get_run_status(char *session);
I 5
void get_run_config(char *msql_database, char *session, int *run, char **config);
E 5
}
E 8
I 3
void decode_command_line(int argc, char **argv);
E 3


//----------------------------------------------------------------

D 8
main(int argc, char **argv){
E 8

D 8
  int status;
I 4
  char *astatus;
I 5
  char *config;
E 8
I 8
main(int argc, char **argv) {

  char line[200];
  char state[32];
  char fsession[32];
D 10
  char config[32];
E 10
I 10
  char *config;
  char fconfig[32];
E 10
E 8
  int run;
I 9
  int msql_run;
I 12
  int stat;
E 12
E 9
E 5
E 4

I 8

E 8
D 3
  // check for session on command line
  if (strncasecmp(argv[1],"-s",2)==0){
    session=strdup(argv[2]);
  }
E 3
I 3
  decode_command_line(argc,argv);
I 8

E 8
I 4
D 11
D 15
  if(session==NULL)session="clasprod";
E 15
I 15
  if(session==NULL)session=(char*)"clasprod";
E 15
E 11
I 11
  if(session==NULL)session=(char *)"clasprod";
E 11
E 4
E 3

I 9

I 12
//    // try get_run_status
//    stat=get_run_status(session);
//    cout << "stat: " << stat << endl;
  

E 12
D 10
  // get run from database
  msql_run=get_run_number(msql_database,session);
E 10
I 10
  // get run,config from database
  get_run_config(msql_database,session,&msql_run,&config);
D 12

E 12
E 10


  // get info from rcstate file
E 9
D 5
  // get run status
E 5
I 5
D 8
  // get run status, etc.
E 5
  status=get_run_status(session);
I 5
  get_run_config(msql_database,session,&run,&config);
E 8
I 8
  sprintf(line,"%s/run_log/rcstate_%s.txt",getenv("CLON_PARMS"),session);
  ifstream file(line);
D 13
  if(file.bad()||file.eof()) {
E 13
I 13
D 14
  if(!file.is_open()||file.eof()) {
E 14
I 14
  if(!file.is_open()||!file.good()||file.eof()||file.fail()||file.bad()) {
E 14
E 13
    cout << "\n?Unable to read " << line << endl << endl;
    exit(EXIT_FAILURE);
  }
E 8
E 5
D 4
  cout << "\n Current run status is: " << status << endl << endl;
E 4

I 4
D 8
  if     (status== 2)astatus="boot";
  else if(status== 4)astatus="config";
  else if(status== 6)astatus="download";
  else if(status== 8)astatus="prestart or pause";
  else if(status==11)astatus="go";
  else if(status==12)astatus="end";
  else if(status== 0)astatus="unknown";
E 8
I 8
  file.getline(line,sizeof(line));
D 10
  sscanf(line,"%s %d %s %s",state,&run,fsession,config);
E 10
I 10
  sscanf(line,"%s %d %s %s",state,&run,fsession,fconfig);
E 10
E 8

D 5
  cout << "\n Current run status is:  " << astatus 
E 5
I 5
D 6
  cout << "\n Current run: " << run 
       << "   config is:  " << config
       << "   status is:  " << astatus 
E 6
I 6
D 7
  cout << "\n Current session: " << session
       << "   run: " << run 
       << "   config:  " << config
       << "   status:  " << astatus 
E 6
E 5
       << "  (" << status << ")" << endl << endl;
E 7
I 7
D 8
  cout << "\n Info for session " << session << ":" << endl << endl
E 8
I 8
  cout << "\n Info for session " << session << " (" << fsession << "):"  << endl << endl
E 8
       << "     current run: " << run 
       << "     config:  " << config
D 8
       << "     status:  " << astatus  << "  (" << status << ")" << endl << endl;
E 8
I 8
       << "     state:  " << state << endl << endl;
I 9

  if(msql_run!=run) {
D 10
    cout << "?msql database run is " << msql_run << ", disagrees with rcstate file!" << endl << endl;
E 10
I 10
    cout << "?msql database run is " << msql_run << ", disagrees with rcstate file: " 
	 << run << endl << endl;
  }
  if(strcmp(config,fconfig)!=0) {
    cout << "?msql database config is " << config << ", disagrees with rcstate file: "
	 << fconfig << endl << endl;
E 10
  }
  
E 9
E 8
E 7
E 4
}
I 3


//----------------------------------------------------------------


D 8
void decode_command_line(int argc, char **argv)
{
E 8
I 8
void decode_command_line(int argc, char **argv) {
E 8

  int i=1;
D 5
  char *help="\nusage:\n\n  run_status [-s session]\n\n\n";
E 5
I 5
D 8
  char *help="\nusage:\n\n  run_status [-s session] [-m msql_database]\n\n\n";
E 8
I 8
D 9
  char *help="\nusage:\n\n  run_status [-s session] \n\n\n";
E 9
I 9
D 11
D 15
  char *help="\nusage:\n\n  run_status [-s session] [-m msql_database]\n\n\n";
E 15
I 15
  const char *help="\nusage:\n\n  run_status [-s session] [-m msql_database]\n\n\n";
E 15
E 11
I 11
  char *help=(char *)"\nusage:\n\n  run_status [-s session] [-m msql_database]\n\n\n";
E 11
E 9
E 8
E 5


  while(i<argc) {
    
    if(strncasecmp(argv[i],"-h",2)==0){
      cout << help;
      exit(EXIT_SUCCESS);
    }
    else if (strncasecmp(argv[i],"-s",2)==0){
      session=strdup(argv[i+1]);
      i=i+2;
    }
I 9
    else if (strncasecmp(argv[i],"-m",2)==0){
      msql_database=strdup(argv[i+1]);
      i=i+2;
    }
E 9
I 5
D 8
    else if (strncasecmp(argv[i],"-m",2)==0){
      msql_database=strdup(argv[i+1]);
      i=i+2;
    }
E 8
E 5
  }
}


D 8
/*---------------------------------------------------------------------*/
E 8
I 8
//--------------------------------------------------------------------
E 8

E 3
E 1
