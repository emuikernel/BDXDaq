h10337
s 00003/00003/00069
d D 1.2 00/08/29 16:38:16 wolin 3 1
c New CC
e
s 00000/00000/00000
d R 1.2 97/05/20 17:10:34 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 libutil/test/run_config.cc
e
s 00072/00000/00000
d D 1.1 97/05/20 17:10:33 wolin 1 0
c Gets run and config from msql database
e
u
U
f e 0
t
T
I 1
// run_config

//  gets run and config

//  ejw, 20-may-97


#include <iostream.h>
#include <stdlib.h>
#include <string.h>


char *session       = getenv("DD_NAME");
D 3
char *msql_database = "clasrun";
E 3
I 3
char *msql_database = (char *)"clasrun";
E 3


extern "C"{
void get_run_config(char *msql_database, char *session, int *run, char **config);
}
void decode_command_line(int argc, char **argv);


//----------------------------------------------------------------


main(int argc, char **argv){

  int run;
  char *config;

  decode_command_line(argc,argv);
D 3
  if(session==NULL)session="clasprod";
E 3
I 3
  if(session==NULL)session=(char *)"clasprod";
E 3


  // get run status
  get_run_config(msql_database,session,&run,&config);
  cout << "\n Current run,config for session " << session << " are:   " 
       << run << "   " << config << endl << endl;

}


//----------------------------------------------------------------


void decode_command_line(int argc, char **argv)
{

  int i=1;
D 3
  char *help="\nusage:\n\n  run_config [-s session] [-m msql_database] \n\n\n";
E 3
I 3
  char *help=(char *)"\nusage:\n\n  run_config [-s session] [-m msql_database] \n\n\n";
E 3


  while(i<argc) {
    
    if(strncasecmp(argv[i],"-h",2)==0){
      cout << help;
      exit(EXIT_SUCCESS);
    }
    else if (strncasecmp(argv[i],"-s",2)==0){
      session=strdup(argv[i+1]);
      i=i+2;
    }
    else if (strncasecmp(argv[i],"-m",2)==0){
      msql_database=strdup(argv[i+1]);
      i=i+2;
    }
  }
}


/*---------------------------------------------------------------------*/

E 1
