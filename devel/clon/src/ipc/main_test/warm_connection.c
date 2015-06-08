/*
*  warm_connection
* 
*  creates or destroys a warm connection to a server
*
*  ejw, 18-nov-96
*
*/


#define _POSIX_SOURCE 1
#define __EXTENSIONS__

#include <rtworks/ipc.h>
#include <stdlib.h>
#include <signal.h>


char *application = "clastest";
char *unique_id   = "dbrouter";
int warm          = 1;
int sleep_time    = 0;


void decode_command_line(int argc, char **argv);



/*------------------------------------------------------------------*/


int main(argc, argv)
int argc;
char **argv;
{
  T_OPTION opt;
  int err;

  /* decode command line */
  decode_command_line(argc,argv);
  
  /* sleep if requested */
  if(sleep_time>0)sleep(sleep_time);

  /* set application */
  opt=TutOptionLookup("Application");
  err=TutOptionSetEnum(opt,application);

  /* set unique_id */
  opt=TutOptionLookup("Unique_Datagroup");
  err=TutOptionSetEnum(opt,unique_id);

  /* set disconnect mode */
  opt=TutOptionLookup("Server_Disconnect_Mode");
  if(warm==1){
    err=TutOptionSetEnum(opt,"warm");
  } else {
    err=TutOptionSetEnum(opt,"gmd_failure");
  }

  /* connect to server */
  err=TipcSrvCreate(T_IPC_SRV_CONN_FULL);

  /* disconnect with proper mode */
  if(warm==1){
    err=TipcSrvDestroy(T_IPC_SRV_CONN_WARM);
  } 
  else {
    err=TipcSrvDestroy(T_IPC_SRV_CONN_NONE);
  }

  exit(EXIT_SUCCESS);

}


/*------------------------------------------------------------------*/


void decode_command_line(int argc, char **argv)
{
  char *help ="\nusage:\n\n  warm_connection [-a application] [-u unique_id] [-s sleep_time] [-none]\n\n\n";
  int i=1;

  if(argc<2){
    printf("%s", help);
    exit(EXIT_SUCCESS);
  }

  while(i<argc) {
    if(strncasecmp(argv[i],"-h",2)==0){
      printf("%s", help);
      exit(EXIT_SUCCESS);
    }
    else if (strncasecmp(argv[i],"-none",5)==0){
      warm=0;
      i=i+1;
    }
    else if (strncasecmp(argv[i],"-a",2)==0){
      application=strdup(argv[i+1]);
      i=i+2;
    }
    else if (strncasecmp(argv[i],"-u",2)==0){
      unique_id=strdup(argv[i+1]);
      i=i+2;
    }
    else if (strncasecmp(argv[i],"-s",2)==0){
      sleep_time=atoi(argv[i+1]);
      i=i+2;
    }
  }

}


/*-------------------------------------------------------------------*/


