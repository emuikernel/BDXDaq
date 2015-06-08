// run_number

//  prints run number

//  ejw, 20-may-97


#include <iostream.h>
#include <stdlib.h>
#include <string.h>


char *session       = getenv("DD_NAME");
char *msql_database = (char *)"clasrun";


extern "C"{
void get_run_config(char *msql_database, char *session, int *run, char **config);
}
void decode_command_line(int argc, char **argv);


//----------------------------------------------------------------


main(int argc, char **argv){

  int run;
  char *config;

  decode_command_line(argc,argv);
  if(session==NULL)session=(char *)"clasprod";


  // get run number
  get_run_config(msql_database,session,&run,&config);
  cout << run << endl;

}


//----------------------------------------------------------------


void decode_command_line(int argc, char **argv)
{

  int i=1;
  const char *help="\nusage:\n\n  run_number [-s session] [-m msql_database] \n\n\n";


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

