h53557
s 00000/00003/00118
d D 1.4 02/08/13 13:28:46 wolin 5 4
c Removed TutCommandParseStdLicense
c   
c 
e
s 00010/00001/00111
d D 1.3 97/09/08 16:45:51 wolin 4 3
c Added sleep option
c 
e
s 00015/00006/00097
d D 1.2 96/11/18 12:10:07 wolin 3 1
c Apparently need to set disc mode explicitely
e
s 00000/00000/00000
d R 1.2 96/11/18 11:54:54 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 2 1 libipc/test/warm_connection.c
c Name history : 1 0 test/warm_connection.c
e
s 00103/00000/00000
d D 1.1 96/11/18 11:54:53 wolin 1 0
c Creates or destroys a warm connection
e
u
U
f e 0
t
T
I 1
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
I 4
int sleep_time    = 0;
E 4

I 4

E 4
void decode_command_line(int argc, char **argv);



/*------------------------------------------------------------------*/


int main(argc, argv)
int argc;
char **argv;
{
  T_OPTION opt;
I 3
  int err;
E 3

  /* decode command line */
  decode_command_line(argc,argv);
  
I 4
  /* sleep if requested */
  if(sleep_time>0)sleep(sleep_time);

E 4
D 5
  /* read startup file */
D 3
  TutCommandParseStdLicense();
E 3
I 3
  err=TutCommandParseStdLicense();
E 3
  
E 5
  /* set application */
  opt=TutOptionLookup("Application");
D 3
  TutOptionSetEnum(opt,application);
E 3
I 3
  err=TutOptionSetEnum(opt,application);
E 3

  /* set unique_id */
  opt=TutOptionLookup("Unique_Datagroup");
D 3
  TutOptionSetEnum(opt,unique_id);
E 3
I 3
  err=TutOptionSetEnum(opt,unique_id);
E 3

I 3
  /* set disconnect mode */
  opt=TutOptionLookup("Server_Disconnect_Mode");
  if(warm==1){
    err=TutOptionSetEnum(opt,"warm");
  } else {
    err=TutOptionSetEnum(opt,"gmd_failure");
  }

E 3
  /* connect to server */
D 3
  TipcSrvCreate(T_IPC_SRV_CONN_FULL);
E 3
I 3
  err=TipcSrvCreate(T_IPC_SRV_CONN_FULL);
E 3

  /* disconnect with proper mode */
  if(warm==1){
D 3
    TipcSrvDestroy(T_IPC_SRV_CONN_WARM);
E 3
I 3
    err=TipcSrvDestroy(T_IPC_SRV_CONN_WARM);
E 3
  } 
  else {
D 3
    TipcSrvDestroy(T_IPC_SRV_CONN_NONE);
E 3
I 3
    err=TipcSrvDestroy(T_IPC_SRV_CONN_NONE);
E 3
  }

  exit(EXIT_SUCCESS);

}


/*------------------------------------------------------------------*/


void decode_command_line(int argc, char **argv)
{
D 4
  char *help ="\nusage:\n\n  warm_connection [-a application] [-u unique_id] [-none]\n\n\n";
E 4
I 4
  char *help ="\nusage:\n\n  warm_connection [-a application] [-u unique_id] [-s sleep_time] [-none]\n\n\n";
E 4
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
I 4
    else if (strncasecmp(argv[i],"-s",2)==0){
      sleep_time=atoi(argv[i+1]);
      i=i+2;
    }
E 4
  }

}


/*-------------------------------------------------------------------*/


E 1
