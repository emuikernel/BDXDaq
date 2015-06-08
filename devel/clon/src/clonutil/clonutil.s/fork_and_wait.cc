//  fork_and_wait
//
//  forks process, waits for it to complete
//  kills (-2, SIGINT) child after timeout
//
//  ejw, 19-may-97


// for posix
#define _POSIX_SOURCE_ 1
#define __EXTENSIONS__

#include <iostream.h>
#include <stdio.h>
#include <stdarg.h>
#include <iomanip.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>


// use SIGINT
#define KILLSIGNAL 2


pid_t pid;
extern "C" {
void child_timeout(int sig);
}
int child_killed;


//--------------------------------------------------------------------------


// accepts null-terminated variable length arg list to go with command
extern "C"{
  int fork_and_wait(const char *command, int timeout, ...){

  int status,narg;
  va_list argp;


  // fork child process
  switch (pid=fork()) {
  case -1:
    cerr << "?unable to fork " << command << endl;
    return (-1);


  case 0:
    narg=0;
    va_start(argp,timeout);
    while(va_arg(argp,char *)!=NULL) narg++;                   // count args and create array

    { 
      char **arg_array = new char *[narg+2];
      arg_array[0]=(char*)command;
      va_start(argp,timeout);                                  // fill arg array
      for(int i=0; i<narg; i++) {arg_array[i+1]=va_arg(argp,char*);}
      arg_array[narg+1]=(char *)NULL;
      va_end(argp);
      
      execvp(command,arg_array);
      
      delete arg_array;
    }
    break;


  default:
    child_killed=0;
    if(timeout>0){
      signal(SIGALRM,child_timeout);
      alarm(timeout);
    }

    waitpid(pid,&status,0);
    alarm(0);

    if(child_killed!=0){
      cerr << "?fork_and_wait...killed child \"" << command << "\" after " 
	   << timeout << " seconds" << endl;
      return(-2);
    } else {
      return (status);
    }

  }

}
} // extern "C"


//--------------------------------------------------------------------------


// kills child after timeout
void child_timeout(int sig){

  kill(pid,KILLSIGNAL);
  child_killed=1;

  return;

}

//--------------------------------------------------------------------------
