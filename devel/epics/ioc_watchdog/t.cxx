
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <limits.h>

#include "string"
#include <semaphore.h>

//char SEM_NAME[]= "hall_b_sc_old";

using namespace std;
/*
sem_t *mutex;

//const char *file_ps="$HOME/.ioc_ps_output.txt";

char file_ps[300];
char file_status[300];

int is_process_running(int is_start, char *process_template, char *semkey );
*/
int main(int argc, char *argv[]){

 //====================================================




char *myargv[10];

 

 //==========================================================================
 //==========================================================================
 //==========================================================================
 //==========================================================================

 for(int i=0;i<10;i++){    //name of program and /0
   myargv[i]= (char *) malloc(600); // alloc 100 bytes
 }

// chdir("");
 
 
 //printf("launch \n");

// return 1;


//FILE *fp=fopen("/misc/home/sytnik/ioc_watchdog/t2.txt","w+");

// fprintf(fp,"launch 222 \n");

char tmp[100];
//char *tmp1
strcpy(tmp,getenv("DIM_DNS_NODE"));
// fprintf(fp,"launch 223 \n");
 //------------------------------------------ 
 //------------------------------------------ 
 if(fork()==0){
///printf("uuuuu \n");
FILE *fp=fopen("/misc/home/sytnik/ioc_watchdog/t2.txt","w+");
 fprintf(fp,"launch 224 \n");
     fprintf(fp,"launch %s  dim=%s\n", argv[0], tmp);
           int ret; 
///printf("uuuuu1 %p  \n", fp);
fflush(fp);
fclose(fp);
           ret= chdir("/misc/clas/clas12/R3.14.12.3/epicsB/baseB/src/CAEN_HV/level0/IocShell");
//printf("launch ret=%d dir=%s \n", ret, argv[4]);
   //            strcpy(myargv[0], "emudcs_exe_wrap");
            strcpy(myargv[0], argv[0]);
            //strcpy(myargv[1], (char *)0;
            myargv[1]=(char *)0;

	    //          execv("/home/dcs/bin/emudcs_exe_wrap",myargv);
         // execv("/usr/local/clas/clas12/R3.14.12.3/epicsB/baseB/src/CAEN_HV/level0/IocShell/cstart",myargv);
          execv("./cstart",myargv);
///printf("not started \n");

 }

 wait(0);

  //  sleep(45);

  ////////// } // while(1)

 //------------------------------------------  



  return 1;

}
//========================================================

