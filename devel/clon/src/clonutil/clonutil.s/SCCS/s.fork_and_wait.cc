h04852
s 00001/00001/00108
d D 1.9 00/10/26 12:56:11 wolin 10 9
c const
e
s 00001/00001/00108
d D 1.8 00/10/26 12:52:13 wolin 9 8
c const 
e
s 00003/00000/00106
d D 1.7 00/06/21 14:49:41 wolin 8 7
c Minor mods for 5.0 compiler
e
s 00024/00003/00082
d D 1.6 97/10/22 17:25:11 wolin 7 6
c Strange usage of arg_array by execvp, but I think it works now!
c 
e
s 00006/00002/00079
d D 1.5 97/08/22 14:40:03 wolin 6 5
c Now using SIGINT to kill child process
c 
e
s 00003/00001/00078
d D 1.4 97/07/15 11:24:26 wolin 5 4
c Forgot to reset child_killed
c 
e
s 00001/00000/00078
d D 1.3 97/07/08 16:19:45 wolin 4 3
c Reset alarm after waitpid
e
s 00032/00004/00046
d D 1.2 97/06/25 15:40:30 wolin 3 1
c Added timeout
e
s 00000/00000/00000
d R 1.2 97/05/19 15:01:50 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 2 1 libutil/s/fork_and_wait.cc
c Name history : 1 0 s/fork_and_wait.cc
e
s 00050/00000/00000
d D 1.1 97/05/19 15:01:49 wolin 1 0
c Forks process to execute command, then waits for it to finish
e
u
U
f e 0
t
T
I 1
//  fork_and_wait
//
//  forks process, waits for it to complete
I 3
D 6
//  kills (-9) child after timeout
E 6
I 6
//  kills (-2, SIGINT) child after timeout
E 6
E 3
//
//  ejw, 19-may-97


// for posix
#define _POSIX_SOURCE_ 1
#define __EXTENSIONS__

I 8
#include <iostream.h>
E 8
#include <stdio.h>
I 7
#include <stdarg.h>
E 7
#include <iomanip.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
I 3
#include <signal.h>
E 3


I 6
// use SIGINT
#define KILLSIGNAL 2


E 6
I 3
pid_t pid;
I 8
extern "C" {
E 8
void child_timeout(int sig);
I 8
}
E 8
D 5
int child_killed=0;
E 5
I 5
int child_killed;
E 5
E 3

I 3

E 3
//--------------------------------------------------------------------------


I 7
// accepts null-terminated variable length arg list to go with command
E 7
extern "C"{
D 3
  int fork_and_wait(char *command){
E 3
I 3
D 7
  int fork_and_wait(char *command, int timeout=0){
E 7
I 7
D 9
  int fork_and_wait(char *command, int timeout, ...){
E 9
I 9
  int fork_and_wait(const char *command, int timeout, ...){
E 9
E 7
E 3

D 3
  pid_t pid;
E 3
D 7
  int status;
E 7
I 7
  int status,narg;
  va_list argp;
E 7

I 5

E 5
D 3
  // fork process to read prescale factors and write to file
E 3
I 3
  // fork child process
E 3
  switch (pid=fork()) {
  case -1:
    cerr << "?unable to fork " << command << endl;
    return (-1);

I 7

E 7
  case 0:
D 7
    execlp(command,NULL,(char *)0);
E 7
I 7
    narg=0;
    va_start(argp,timeout);
    while(va_arg(argp,char *)!=NULL) narg++;                   // count args and create array

    { 
      char **arg_array = new char *[narg+2];
D 10
      arg_array[0]=command;
E 10
I 10
      arg_array[0]=(char*)command;
E 10
      va_start(argp,timeout);                                  // fill arg array
      for(int i=0; i<narg; i++) {arg_array[i+1]=va_arg(argp,char*);}
      arg_array[narg+1]=(char *)NULL;
      va_end(argp);
      
      execvp(command,arg_array);
      
      delete arg_array;
    }
E 7
    break;

I 7

E 7
  default:
I 5
    child_killed=0;
E 5
I 3
    if(timeout>0){
      signal(SIGALRM,child_timeout);
      alarm(timeout);
    }

E 3
    waitpid(pid,&status,0);
I 4
    alarm(0);
E 4
D 3
    return (0);
E 3
I 3

    if(child_killed!=0){
      cerr << "?fork_and_wait...killed child \"" << command << "\" after " 
	   << timeout << " seconds" << endl;
      return(-2);
    } else {
      return (status);
    }
I 7

E 7
E 3
  }

}
} // extern "C"


//--------------------------------------------------------------------------


I 3
// kills child after timeout
void child_timeout(int sig){

D 6
  kill(pid,9);
E 6
I 6
  kill(pid,KILLSIGNAL);
E 6
  child_killed=1;

  return;

}

//--------------------------------------------------------------------------
E 3
E 1
