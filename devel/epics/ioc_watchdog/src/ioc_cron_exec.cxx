
/* ioc_cron_exec.cxx */

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

using namespace std;

static FILE *fp_check;
static char file_ps[300];

int is_process_running(int is_start, char *process_template, char *semkey );

int
main(int argc, char *argv[])
{
  
  printf("argv[0]..[4] >%s< >%s< >%s< >%s< >%s<\n",argv[0],argv[1],argv[2],argv[3],argv[4]);
  
  char SEM_NAME[40]= "hall_b_sc_";
  sem_t *mutex;
  int ret;
  //const char *file_ps="$HOME/.ioc_ps_output.txt";
  //char file_status[300];
  char tmp[PATH_MAX];
  char *get_user;
  char *get_home;
  char tmp_check[100];
  char *myargv[10];

  get_home = getenv("HOME");
  get_user = getenv("USER");

  strcpy(tmp_check, get_home);
  strcat(tmp_check, "/");
  strcat(tmp_check, ".ioc_check_");
  strcat(tmp_check,argv[2]);
  fp_check=fopen(tmp_check,"a+");


  //struct timespec ts;
  //ts.tv_sec=10;
  //ts.tv_nsec=0;

  strcat(SEM_NAME,get_user);
  strcat(SEM_NAME,"_");
  strcat(SEM_NAME,argv[3]);

  strcpy(file_ps, get_home);
  strcat(file_ps,"/");
  strcat(file_ps,".");
  strcat(file_ps,argv[2]);
  // strcpy(file_status,file_ps);
  strcat(file_ps,".txt");
  // strcat(file_status,".txt");
  printf("file_ps=%s sem_name=%s\n", file_ps, SEM_NAME);

  /// sleep(1);
printf("====1\n");
  // corr 08/18/2008
  // system("killall emudcs_watchdog_crontab");
  int is_start=atoi(argv[1]);
  char semkey[100];
  if(is_start)
  {
    if(argc>=6)strcpy(semkey, argv[5]);
    else strcpy(semkey,"");
  }

  if(is_start==2)
  {
    ret=sem_unlink(SEM_NAME);
    if(ret<0)
    {
      perror("unable to kill semaphore");
    }
  }
  mutex = sem_open(SEM_NAME,O_CREAT,0777,1);
  if(mutex == SEM_FAILED)
  {
    perror("unable to create semaphore");
    sem_unlink(SEM_NAME);
    exit(-1);
  }
printf("====1\n");
  ret = 0;
  if(is_start==1)
  {
    ret=sem_trywait(mutex);
  }
  else sem_wait(mutex);
  if(ret<0) return 1;
printf("====2  ret=%d\n",ret);

  if(is_process_running(is_start, argv[2], semkey ))
  {
    printf("process is running - return 1\n");
    if(is_start!=0)sem_post(mutex);
    sem_close(mutex);
    return(1);
  }
  else
  {
    printf("process is not running - will start it\n");
  }

  sem_post(mutex);
  sem_close(mutex);

  sleep(1);

 /* if(is_process_running(argv[3],argv[4] )){system("killall cc");system("killall read_single");return 1;}*/

 //==========================================================================
 //==========================================================================
 //==========================================================================
 //==========================================================================

fprintf(fp_check, "start %s\n", argv[2]);

 for(int i=0;i<10;i++){    //name of program and /0
   myargv[i]= (char *) malloc(600); // alloc 100 bytes
 }

// chdir("");
 
 
 printf("launch \n");

// return 1;

 //------------------------------------------ 
 //------------------------------------------ 
 if(fork()==0){
     printf("launch %s\n", argv[2]);
           int ret; 
           ret= chdir(argv[4]);
printf("launch ret=%d dir=%s \n", ret, argv[4]);
   //            strcpy(myargv[0], "emudcs_exe_wrap");
            strcpy(myargv[0], argv[3]);
            //strcpy(myargv[1], (char *)0;
            myargv[1]=(char *)0;

	    //          execv("/home/dcs/bin/emudcs_exe_wrap",myargv);
         // execv("/usr/local/clas/clas12/R3.14.12.3/epicsB/baseB/src/CAEN_HV/level0/IocShell/cstart",myargv);
strcpy(tmp,"./");
strcat(tmp,argv[3]);
//          execv("./cstart",myargv);
            execv(tmp,myargv);
printf("not started \n");
 }
 wait(0);

  //  sleep(45);

  ////////// } // while(1)

 //------------------------------------------  



  return 1;

}
//========================================================





// ==================
int
is_process_running(int is_start, char *process_template, char *semkey )
{
  FILE *fp;
  char line[PATH_MAX];   
  int isEnabled;
  int ret, retpid;

  //string process_template2_string= string(process_template2);//"big_config";

  char command_ps[PATH_MAX];
  char retline[PATH_MAX];
  char *retline1;

  //   string  process_template2_string=string(process_template2);
  string line_string;

  retline1 = (char *) &retline;

  strcpy(command_ps,"/sbin/pidof ");
  strcat(command_ps,process_template);   
  strcat(command_ps," > ");
  strcat(command_ps, "/dev/null");
  retpid = system(command_ps);  
  printf("execute >%s<\n",command_ps);
  printf("pid=%d\n",retpid);

  if(is_start==1){  /// call from crontab
   if(retpid==0 || retpid==-1) return 1;  /// no start as the process named process_template is running or error in request of this
   else { /// although the process is supposed not running, we do killing just in case (not to have two processes)
 
    fprintf(fp_check, "start_check %s retpid=%d\n", process_template, retpid);

    strcpy(command_ps,"killall ");
    strcat(command_ps,process_template);
    strcat(command_ps," > /dev/null");
    ret=system(command_ps);

   }
 ///  fp_status=fopen(file_status,"r");
 ///  if(fp_status){
 ///   fscanf(fp_status,"%d", &isEnabled);
 ///   fclose(fp_status);
 ///   if(isEnabled==0)return 1; /// no start : start is disabled
 ///  }
  }
  else if(is_start==0){ /// forced stop (call from crontab will be disabled)
 ///  fp_status=fopen(file_status,"w+");
 ///  fprintf(fp_status,"%d\n",0);
 ///  fclose(fp_status);

   strcpy(command_ps,"killall ");
   strcat(command_ps,process_template);
   strcat(command_ps," > /dev/null");
   ret=system(command_ps);
   return 1; /// no start if we stop 

  }
  else if(is_start==2){ /// forced start (restart)  (call from crontab will be enabled)
 ///  fp_status=fopen(file_status,"w+");
 ///  fprintf(fp_status,"%d\n",1);
 ///  fclose(fp_status);

///   if(retpid==0){
   /// although the perocess may not run, we do killing just in case
    strcpy(command_ps,"killall ");
    strcat(command_ps,process_template);
    strcat(command_ps," > /dev/null");
    ret=system(command_ps);
///   }
  }

///--------------------------------------------------------------------------------------

  if(!strcmp(semkey,"")) return 0;  /// if no semkey is used (mpod, old caen, e.g.) we do not remove a semaphore

  strcpy(command_ps,"ipcs -s"); 
  strcat(command_ps," > ");
  strcat(command_ps,file_ps);
  ret=system(command_ps);
  if(ret!=0){printf("ipcs command error \n");return 1;}  /// any error means : no start 
  sleep(1);
  fp=fopen(file_ps,"r");
  if(fp==NULL){printf("file open failure\n");return 1;} /// any error means : no start 

  char *pos, *pos1=0;
  char *pos_found_not_space;
 // char s_id[300];

   while(1){

    retline1= (char *)fgets((char *)line, PATH_MAX, fp);
    if(retline1==NULL)break; /// normal situation at PC restart

    line_string=string((char *)line);
//    printf("%s >>>  \n",line_string.c_str());

    if( (pos=strstr(line, semkey)) ){
     pos1=pos+strlen(semkey);
     pos_found_not_space=0;
     while(1){
      if(*(pos1)!=0x20){
       pos_found_not_space=pos1;
       break;
      }
      pos1++;
     }
     while(1){
      if(*(pos1)==0x20){
       *(pos1)=0;
       strcpy(line,pos_found_not_space);
       break;
      }
      pos1++;
     }
     printf("semid=%s %d\n",line, atoi(line));
     strcpy(command_ps,"ipcrm sem "); 
     strcat(command_ps,line);
     strcat(command_ps," > /dev/null");
     ret=system(command_ps);
     if(ret!=0){printf("ipcrm command error \n");return 1;}  /// any error means : no start 

     break;
    } // if( (pos=strstr(line, semkey)) )
   } // while(1);


   return 0;

}
