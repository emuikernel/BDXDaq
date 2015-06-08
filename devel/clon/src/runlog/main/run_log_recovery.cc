/* run_log_recovery.cc - recovers following information for
                        session 'session' and run number 'runnum':

$CLON_PARMS/run_files/runfile_<session>_<runnum>.txt - normally written by Event Recorder
$CLON_PARMS/scalers/archive/scalers_<session>_<runnum>.txt - normally written by 'scaler_server' from ET
$CLON_PARMS/run_log/archive/end_<session>_<runnum>.txt - normally written by 'run_log_end'

input raw data files (*.A00, *.A01, etc) assumed to be in '/work/recovery'

input parameter: run number; if runnum is set to '-1', so-called cronjob mode will be used
(will start from current run number minus one, and go backward restoring missing files)

NOTE: program contains hard-coded raid partition names etc - sorry about !!! 

Sergey Boyarinov, Feb 2009
 */

// for posix
#define _POSIX_SOURCE_ 1
#define __EXTENSIONS__
// system stuff

using namespace std;
#include <strstream>

#include <iostream.h>
#include <iomanip.h>
#include <fstream.h>


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <math.h>
#include <time.h>
#include <sys/time.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>
#include <locale.h>
#include <langinfo.h>
#include <stdint.h>
#include <unistd.h>

#include "bos.h"
#include "bosio.h"

#define NBCS 700000
#include "bcs.h"

/*ERROR CODES*/
#define ERR_EXIST      -1
#define ERR_NOFILES    -2
#define ERR_NORAID     -3
#define ERR_EXTENSION  -4
#define ERR_INCOMPLETE -5
#define ERR_FILESIZE   -6
#define ERR_WRITING    -7

static char *session = (char*)"clasprod";

extern "C"
{
  int get_run_number(char *msql_database, char *session);
}
int process_one_run(int run, int need_runfile, int need_end, int need_scalers);

static int
files_compare(char *name1, char *name2)
{
  int len1, len2;
  char *ch1, *ch2, str1[10], str2[10];
  unsigned int num1, num2;

  len1 = strlen(name1);
  len2 = strlen(name2);
  ch1 = (char *) &name1[len1-3];
  ch2 = (char *) &name2[len2-3];
  strcpy(str1,"0x");
  strcpy(str2,"0x");
  strcat(str1,ch1);
  strcat(str2,ch2);
  num1 = (int)strtol((const char *)str1,(char **)NULL, 16);
  num2 = (int)strtol((const char *)str2,(char **)NULL, 16);
  /*
  printf("ch1=%3.3s ch2=%3.3s\n",ch1,ch2);
  printf("str1=%s str2=%s\n",str1,str2);
  printf("num1=%d num2=%d\n",num1,num2);
  */
  if(num1>num2) return (1);
  if(num1<num2) return (-1);

  return(0);
}

/* check if at least one file we suppose to recover are missing; if so - recover */
#define CHECK_LOG_FILE_EXISTENCE \
    need_recover = 0; \
    need_runfile = 0; \
    need_end = 0; \
    need_scalers = 0; \
    sprintf(str1,"%s/run_files/archive/runfile_%s_%s.txt",getenv("CLON_PARMS"),session,chrun); \
    sprintf(str2,"%s/run_files/runfile_%s_%s.txt",getenv("CLON_PARMS"),session,chrun); \
    fd1 = fopen(str1,"r"); \
    fd2 = fopen(str2,"r"); \
    if(fd1!=NULL) \
    { \
      printf("File >%s< exist, do not need to recover\n",str1); \
      fclose(fd1); \
    } \
    else if(fd2!=NULL) \
    { \
      printf("File >%s< exist, do not need to recover\n",str2); \
      fclose(fd2); \
    } \
    else \
    { \
      printf("File >%s< is missing as well, recovering\n",str2); \
      need_runfile = 1; \
      need_recover=1; \
    } \
    sprintf(str1,"%s/run_log/archive/end_%s_%s.txt",getenv("CLON_PARMS"),session,chrun); \
    fd1 = fopen(str1,"r"); \
    if(fd1!=NULL) \
    { \
      printf("File >%s< exist, do not need to recover\n",str1); \
      fclose(fd1); \
    } \
    else \
    { \
      printf("File >%s< is missing, recovering\n",str1); \
      need_end = 1; \
      need_recover=1; \
    } \
    sprintf(str1,"%s/scalers/archive/scalers_%s_%s.txt",getenv("CLON_PARMS"),session,chrun); \
    fd1 = fopen(str1,"r"); \
    if(fd1!=NULL) \
    { \
      printf("File >%s< exist, do not need to recover\n",str1); \
      fclose(fd1); \
    } \
    else \
    { \
      printf("File >%s< is missing, recovering\n",str1); \
      need_scalers = 1; \
      need_recover=1; \
    }


int
main(int argc, char **argv)
{
  int run, ret;
  char str1[1000], str2[1000];
  char chrun[30];
  FILE *fd1, *fd2;
  int need_recover;
  int need_runfile;
  int need_end;
  int need_scalers;
  time_t tloc;

  if(argc != 2)
  {
    printf("\n   Usage: run_log_recovery <runnum>\n\n");
    exit(0);
  }


  time(&tloc);
  printf("\n\nStarting 'run_log_recovery' on %s\n",ctime(&tloc));


  if(!strncmp(argv[1],"-1",2))
  {

startagain:

    run = get_run_number("clasrun","clasprod");
    run --;
    printf("will start from run number=%d\n",run);

prevrun:

    sprintf(chrun,"%06d",run);
    printf("chrun=>%s<\n",chrun);

    CHECK_LOG_FILE_EXISTENCE;

	printf("need_recover=%d\n",need_recover);
    if(need_recover)
    {
      ret = process_one_run(run, need_runfile, need_end, need_scalers);
      if(ret!=0) exit(0);
      goto startagain;
	}
	else
	{
      run --;
      goto prevrun;
	}

  }
  else
  {
    run = atoi(argv[1]);
    printf("will process run number=%d\n",run);
    sprintf(chrun,"%06d",run);
    printf("chrun=>%s<\n",chrun);
    CHECK_LOG_FILE_EXISTENCE;
	printf("need_recover=%d\n",need_recover);
    if(need_recover)
    {
      ret = process_one_run(run, need_runfile, need_end, need_scalers);
	}
  }


  exit(0);
}

int
process_one_run(int run, int need_runfile, int need_end, int need_scalers)
{
  DIR *dd[5];
  DIR *ddd;
  struct dirent *de;
  struct stat stat_result;
  struct stat lstat_result;
  BOSIO *bd;
  FILE *fd;
  int tmp1 = 1;
  float *bcsfl;
  char str[1000], dirname[5][130];
  char dname[300][130], fname[300][80], fullname[300][210], goodraid[4][30], *ch;
  int i,j,iev,iret,nw,ind,ind1,ind2,status,nfiles,len,ifile,ngoodraid,igoodraid,good;
  int nlongs[300],nevents[300],goodraidlen[4];
  int nlongs_total = 0;
  int nevents_total = 0;
  int nerrors = 0;
  int trigstat[12];
  int bor_time = 0;
  int eor_time = 0;
  struct tm *ptm;
  char chrun[30], date_string[30];
  int len1;
  char *ch1, *ch2;
  unsigned int num1, num2;
  char tmp[1000];
  time_t tloc;

  unsigned int head0[8];
  unsigned int tgbi0[5];
  unsigned int hls_0[32];
  unsigned int hls_1[32];
  unsigned int ecs0[96];
  unsigned int scs0[192];
  unsigned int trgs0[96];
  unsigned int trgs1[96];
  unsigned int s1st0[35];
  unsigned int stsn0[32];
  unsigned int tgs0[192];

  unsigned int clock_all;
  unsigned int fcup_all;
  unsigned int clock_active;
  unsigned int fcup_active;
  unsigned int clock_live;
  unsigned int fcup_live;
  unsigned int trig_presc[13],trig_event[13],trig_file[13];

  sprintf(chrun,"%06d",run);
  printf("chrun=>%s<\n",chrun);

  for(i=0; i<13; i++)
  {
    trig_presc[i] = 0;
    trig_event[i] = 0;
    trig_file[i] = 0;
  }

  for(i=0; i<8; i++)
  {
    head0[i] = 0;
  }
  for(i=0; i<5; i++)
  {
    tgbi0[i] = 0;
  }
  for(i=0; i<96; i++)
  {
    ecs0[i] = 0;
    trgs0[i] = 0;
    trgs1[i] = 0;
  }
  for(i=0; i<192; i++)
  {
    scs0[i] = 0;
    tgs0[i] = 0;
  }
  for(i=0; i<35; i++)
  {
    s1st0[i] = 0;
  }
  for(i=0; i<32; i++)
  {
    hls_0[i] = 0;
    hls_1[i] = 0;
    stsn0[i] = 0;
  }



  bcsfl = (float*)bcs_.iw;
  bosInit(bcs_.iw,NBCS);


  /* look for data files in /mnt/raidX/stage_in/ directories and then in
  /work/recovery/ */

  nfiles = 0;

  sprintf(dirname[0],"/mnt/raid0/stage_in/");
  sprintf(dirname[1],"/mnt/raid1/stage_in/");
  sprintf(dirname[2],"/mnt/raid2/stage_in/");
  sprintf(dirname[3],"/mnt/raid3/stage_in/");
  sprintf(dirname[4],"/work/recovery/");

  for(j=0; j<5; j++)
  {
    dd[j] = opendir(dirname[j]);
    if(dd[j] == NULL) continue;
    while((de = readdir(dd[j])) != NULL)
    {
	  /*
      printf("dd[%1d]: file >%s< (len=%d) inode=%d offset=%d len=%d\n",
        j,de->d_name,strlen(de->d_name),de->d_ino,de->d_off,de->d_reclen);
	  */
      len = strlen(de->d_name);
      if(len > 6)
      {
        ch = (char *) &de->d_name[len-4];
        /*printf("ch1=%2.2s\n",ch);*/
        if( !strncmp(ch,".A",2) || !strncmp(ch,".B",2) || !strncmp(ch,".C",2) )
        {
          ch = (char *) &de->d_name[len-10];
          /*printf("ch2=%6.6s\n",ch);*/
          if(!strncmp(ch,chrun,6))
          {
            sprintf(fname[nfiles],"%s",de->d_name);
            sprintf(dname[nfiles],"%s",dirname[j]);
            nfiles ++;
          }
        }
      }
    }
    closedir(dd[j]);
  }

  printf("found %d files\n",nfiles);fflush(stdout);
  if(nfiles==0) return(ERR_NOFILES);








  /* check the status of raid partitions; we'll process only files on active and .temp partitions */

  ngoodraid = 0;
  sprintf(dirname[0],"/ssa/");
  {
    ddd = opendir(dirname[0]);
    if(ddd == NULL)
	{
      printf("ERRRRRRRRRRR\n");/*better say it is not clondaq2 and continue with /work/recovery/*/
      exit(0);
	}
    while((de = readdir(ddd)) != NULL)
    {
	  /*
      printf("ddd: file >%s< (len=%d) inode=%d offset=%d len=%d\n",
        de->d_name,strlen(de->d_name),de->d_ino,de->d_off,de->d_reclen);
	  */
      sprintf(str,"%s%s",dirname[0],de->d_name);
      tmp[0] = '\0';
      readlink(str,tmp,1000);
      len = strlen(tmp);
	  if(len>0)
      {
        printf("ddd: symbolic link >%s< points to >%s< (%d)\n\n",str,tmp,len);
		/*here we have to select raid partitions we want to use; make sure they do not erased during processing*/
        if(!strcmp(str,"/ssa/active"))
		{
          strcpy(goodraid[ngoodraid],tmp);
          goodraidlen[ngoodraid++]=len;
		}
        else if(!strcmp(str,"/ssa/.temp1"))
		{
          strcpy(goodraid[ngoodraid],tmp);
          goodraidlen[ngoodraid++]=len;
		}
        else if(!strcmp(str,"/ssa/.temp2"))
		{
          strcpy(goodraid[ngoodraid],tmp);
          goodraidlen[ngoodraid++]=len;
		}
        else if(!strcmp(str,"/ssa/presilo"))
		{
          strcpy(goodraid[ngoodraid],tmp);
          goodraidlen[ngoodraid++]=len;
		}
      }
    }
    closedir(ddd);
  }

  for(igoodraid=0; igoodraid<ngoodraid; igoodraid++)
  {
    printf("can use raid partition >%s<\n",goodraid[igoodraid]);
  }
  printf("\n");

  /* check if all our files are on good raid partitions */
  for(ifile=0; ifile<nfiles; ifile++)
  {
    sprintf(fullname[ifile],"%s%s",dname[ifile],fname[ifile]);
    good=0;
    for(igoodraid=0; igoodraid<ngoodraid; igoodraid++)
	{
      if(!strncmp(dname[ifile],"/work/recovery/",15))
	  {
        good=1;
        break;
	  }
      else if(!strncmp(dname[ifile],goodraid[igoodraid],goodraidlen[igoodraid]))
	  {
        good=1;
        break;
	  }
    }  
    if(good)
    {
      printf("File >%s< can be processed\n",fullname[ifile]);
    }
	else
	{
      printf("File >%s< cannot be processed, raid partition >%s< cannot be used\n",fullname[ifile],dname[ifile]);
      return(ERR_NORAID);
    }
  }
  

  /* make full names */
  for(ifile=0; ifile<nfiles; ifile++)
  {
    sprintf(fullname[ifile],"%s%s",dname[ifile],fname[ifile]);
    /*printf("unsorted: [%2d] %s\n",ifile,fullname[ifile]);*/
  }

  /* sort files */
  qsort((void *)fullname, nfiles, 210, (int (*) (const void *, const void *))files_compare);

  /* refill dname and fname arrays using sorted list */
  for(ifile=0; ifile<nfiles; ifile++)
  {
    ch = fullname[ifile];
    ch += strlen(fullname[ifile]);
    while(*ch != '/') ch --;
    /*printf("ch >%s<\n",ch);*/
    /*replace '/' by '\0', copy both parts and restore '/'*/
    *ch = '\0';
    strcpy(dname[ifile],fullname[ifile]);
    strcat(dname[ifile],(char *)"/");
    strcpy(fname[ifile],(char *)&ch[1]);
    *ch = '/';
  }

  for(ifile=0; ifile<nfiles; ifile++)
  {
    len1 = strlen(fname[ifile]);
    ch1 = (char *) &fname[ifile][len1-2];
    ch2 = (char *) &fname[ifile][len1-3];
    num1 = (int)strtol((const char *)ch1,(char **)NULL, 10);
    if(*ch2 == 'A') num2=0;
    else if(*ch2 == 'B') num2=1;
    else if(*ch2 == 'C') num2=2;
    else
    {
      printf("Error in file name extension >%s< - exit\n",fname[ifile]);
      return(ERR_EXTENSION);
	}

    printf("sorted: [%2d] %s (%s %s [%1d %2d])\n",ifile,fullname[ifile],dname[ifile],fname[ifile],num2,num1);
    if((num2*100+num1) != ifile)
	{
      printf("ERROR: seems files set is incomplete - exit\n");
      return(ERR_INCOMPLETE);
	}
  }


  for(j=0; j<12; j++) trigstat[j] = 0;
  for(ifile=0; ifile<nfiles; ifile++)
  {
    nlongs[ifile] = nevents[ifile] = 0;
    sprintf(str,"%s%s",dname[ifile],fname[ifile]);
    printf("[%2d] %s (%s)\n",ifile,fname[ifile],str);

    if ( stat(str, &stat_result) == -1 )
	{
      printf("Error getting %s file size - exit\n",str);
      return(ERR_FILESIZE);
	}
    else
	{
      nlongs[ifile] = stat_result.st_size / 4;
      printf("Size of \"%s\" is %ld (%d) bytes\n", str, stat_result.st_size,nlongs[ifile]);
	}

    status = bosOpen(str,(char*)"r",&bd);
    for(iev=0; iev<1000000000; iev++)
    {
      iret = bosRead(bd, bcs_.iw, (char*)"E");
      if(iret==0)
	  {
        nevents[ifile] ++;
        if(!(iev%100000)) printf("===== Event no. %d\n",iev);

        /*************************/
        /*get info from HEAD bank*/
        if((ind1=bosNlink(bcs_.iw,"HEAD",0)) > 0)
        {
          unsigned int *fb, *fbend;
          int crate,slot,channel,edge,data,count,ncol1,nrow1;

          ncol1 = bcs_.iw[ind1-6];
          nrow1 = bcs_.iw[ind1-5];
          nw = nrow1;
	      /*
          printf("ncol=%d nrow=%d nw=%d\n",ncol1,nrow1,nw);
	      */
          fb = (unsigned int *)&bcs_.iw[ind1];
          for(j=0; j<8; j++) head0[j] = fb[j];
          if(bor_time == 0) bor_time = fb[3];
          eor_time = fb[3];
	      /*
          printf("\eor_time=%d\n",eor_time);
	      */

          for(j=0; j<12; j++) trigstat[j] += (fb[7] >> j) & 1;


          if(fb[4] == 10) /*process scaler event*/
		  {
            unsigned int *fb;

            if((ind1=bosNlink(bcs_.iw,"TGBI",0)) > 0)
            {
              fb = (unsigned int *)&bcs_.iw[ind1];
              for(j=0; j<8; j++) tgbi0[j] = fb[j];
		    }
            if((ind1=bosNlink(bcs_.iw,"HLS+",0)) > 0)
            {
              fb = (unsigned int *)&bcs_.iw[ind1];
              for(j=0; j<32; j++) hls_0[j] = fb[j];
		    }
            if((ind1=bosNlink(bcs_.iw,"HLS+",1)) > 0)
            {
              fb = (unsigned int *)&bcs_.iw[ind1];
              for(j=0; j<32; j++) hls_1[j] = fb[j];
		    }
            if((ind1=bosNlink(bcs_.iw,"ECS ",0)) > 0)
            {
              fb = (unsigned int *)&bcs_.iw[ind1];
              for(j=0; j<96; j++) ecs0[j] = fb[j];
		    }
            if((ind1=bosNlink(bcs_.iw,"SCS ",0)) > 0)
            {
              fb = (unsigned int *)&bcs_.iw[ind1];
              for(j=0; j<192; j++) scs0[j] = fb[j];
		    }
            if((ind1=bosNlink(bcs_.iw,"TRGS",0)) > 0)
            {
              fb = (unsigned int *)&bcs_.iw[ind1];
              for(j=0; j<96; j++) trgs0[j] = fb[j];
		    }
            if((ind1=bosNlink(bcs_.iw,"TRGS",1)) > 0)
            {
              fb = (unsigned int *)&bcs_.iw[ind1];
              for(j=0; j<96; j++) trgs1[j] = fb[j];
		    }
            if((ind1=bosNlink(bcs_.iw,"S1ST",0)) > 0)
            {
              fb = (unsigned int *)&bcs_.iw[ind1];
              for(j=0; j<35; j++) s1st0[j] = fb[j];
		    }
            if((ind1=bosNlink(bcs_.iw,"STSN",0)) > 0)
            {
              fb = (unsigned int *)&bcs_.iw[ind1];
              for(j=0; j<32; j++) stsn0[j] = fb[j];
		    }
            if((ind1=bosNlink(bcs_.iw,"TGS ",0)) > 0)
            {
              fb = (unsigned int *)&bcs_.iw[ind1];
              for(j=0; j<192; j++) tgs0[j] = fb[j];
		    }
		  }

	    }


	  }
      else if(iret == -1 || iret > 0)
      {
        printf(" End-of-File flag, iret =%d\n",iret);
        goto a1;
      }
      else if(iret < 0)
      {
        printf(" Error1, iret =%d\n",iret);
        goto a1;
      }
      if(iret != 0)
      {
        printf(" Error2, iret =%d\n",iret);
      }
      bosLdrop(bcs_.iw,"E");
      bosNgarbage(bcs_.iw);
    }

a1:
    bosClose(bd);
  }


  for(ifile=0; ifile<nfiles; ifile++)
  {
    nlongs_total += nlongs[ifile] / 1024;
    nevents_total += nevents[ifile];
  }



  /**************************/
  /* write results to files */
  /**************************/

  /**********/
  /**********/
  if(need_runfile)
  {
  sprintf(str,"%s/run_files/runfile_%s_%s.txt",
    getenv("CLON_PARMS"),session,chrun);
  printf("Opening file >%s<\n",str);
  fd = fopen(str,"r");
  if(fd == NULL)
  {
    fd = fopen(str,"w");
    if(fd != NULL)
	{
      chmod(str,S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH);
      time(&tloc);
      fprintf(fd,"\n*run_log_recovery*\n");
      fprintf(fd,"%s\n",ctime(&tloc));

      fprintf(fd,"\n*BOR*     this is UNIX time\n");
      fprintf(fd,"%12d\n",bor_time);

      fprintf(fd,"\n*FILES*\n");
      fprintf(fd,"#  dir                   filename                 nlong   nevent   nerror\n");
      fprintf(fd,"#  ---                   --------                 -----   ------   ------\n");

	  for(ifile=0; ifile<nfiles; ifile++)
	  {
        fprintf(fd,"%s\t%s\t\t\t%d\t%d\t%d\n",dname[ifile],fname[ifile],nlongs[ifile],nevents[ifile],nerrors);
	  }

      fprintf(fd,"\n*L1STAT*    1       2       3       4       5       6       7       8       9      10      11      12\n");
	  fprintf(fd,"      %7u %7u %7u %7u %7u %7u %7u %7u %7u %7u %7u %7u\n",trigstat[0],trigstat[1],
        trigstat[2],trigstat[3],trigstat[4],trigstat[5],trigstat[6],
		trigstat[7],trigstat[8],trigstat[9],trigstat[10],trigstat[11]);

      fprintf(fd,"\n*EOR*     this is UNIX time\n");
      fprintf(fd,"%12d\n",eor_time);

      fclose(fd);
	}
    else
	{
      printf("Cannot open file >%s< for writing\n",str);
      return(ERR_WRITING);
	}
  }
  else
  {
    printf("File >%s< exist - do nothing\n",str);
    fclose(fd);
  }
  }


  /**********/
  /**********/
  if(need_end)
  {
  sprintf(str,"%s/run_log/archive/end_%s_%s.txt",
    getenv("CLON_PARMS"),session,chrun);
  printf("Opening file >%s<\n",str);
  fd = fopen(str,"r");
  if(fd == NULL)
  {
    fd = fopen(str,"w");
    if(fd != NULL)
	{
      chmod(str,S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH);
      time(&tloc);
      fprintf(fd,"\n*run_log_recovery*\n");
      fprintf(fd,"%s\n",ctime(&tloc));

      fprintf(fd,"\n*SESSION*\n");
      fprintf(fd,"%s\n",session);

      fprintf(fd,"\n*RUN*\n");
      fprintf(fd,"%d\n",run);

      fprintf(fd,"\n*END_DATE*\n");
      ptm = localtime((long *)&eor_time);
      strftime(date_string,25,"%Y-%m-%d %H:%M:%S",ptm);
      fprintf(fd,"%s\n",date_string);

      fprintf(fd,"\n*FILEBASE*\n");
      fprintf(fd,"clas_%s\n",chrun);

      fprintf(fd,"\n*FILES* --- nfile,nlong,nevent,nerror\n");
	  fprintf(fd,"%d %d %d %d\n",nfiles,nlongs_total,nevents_total,nerrors);

      clock_all    = trgs0[0];
      fcup_all     = trgs0[1];
      clock_active = trgs0[16];
      fcup_active  = trgs0[17];
      clock_live   = trgs0[32];
      fcup_live    = trgs0[33];
      fprintf(fd,"\n*SCALERS* --- clock all,active,live; fcup all,active,live\n");
	  fprintf(fd,"%u %u %u\n",clock_all,clock_active,clock_live);
	  fprintf(fd,"%u %u %u\n",fcup_all,fcup_active,fcup_live);

      for(j=0; j<13; j++) trig_presc[j] = trgs0[48+j];
      fprintf(fd,"\n*PRESCALE_COUNT*\n");
      for(j=0; j<13; j++)
	  {
        fprintf(fd,"%u",trig_presc[j]);
        fprintf(fd," ");
      }
      fprintf(fd,"\n");

      for(j=0; j<13; j++) trig_event[j] = s1st0[j];
      fprintf(fd,"\n*EVENT_COUNT*\n");
      for(j=0; j<13; j++)
	  {
        fprintf(fd,"%u",trig_event[j]);
        fprintf(fd," ");
      }
      fprintf(fd,"\n");

      for(j=0; j<12; j++) trig_file[j] = trigstat[j];
      trig_file[12] = nevents_total;
      fprintf(fd,"\n*FILE_COUNT*\n");
      for(j=0; j<13; j++)
	  {
        fprintf(fd,"%u",trig_file[j]);
        fprintf(fd," ");
      }
      fprintf(fd,"\n");


      fclose(fd);
	}
    else
	{
      printf("Cannot open file >%s< for writing\n",str);
      return(ERR_WRITING);
	}
  }
  else
  {
    printf("File >%s< exist - do nothing\n",str);
    fclose(fd);
  }
  }

  /**********/
  /**********/
  if(need_scalers)
  {
  sprintf(str,"%s/scalers/archive/scalers_%s_%s.txt",
    getenv("CLON_PARMS"),session,chrun);
  printf("Opening file >%s<\n",str);
  fd = fopen(str,"r");
  if(fd == NULL)
  {
    fd = fopen(str,"w");
    if(fd != NULL)
	{
      chmod(str,S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH);
      time(&tloc);
      fprintf(fd,"\n*run_log_recovery*\n");
      fprintf(fd,"%s\n",ctime(&tloc));

      fprintf(fd,"\n*HEAD00*\n");
      for(j=0; j<8; j++)
	  {
        fprintf(fd,"%10u",head0[j]);
        fprintf(fd," ");
  	  }
      fprintf(fd,"\n");
      fprintf(fd,"*EOD*\n");


      fprintf(fd,"\n*TGBI00*\n");
      for(j=0; j<5; j++)
	  {
        fprintf(fd,"%10u",tgbi0[j]);
        fprintf(fd," ");
  	  }
      fprintf(fd,"\n");
      fprintf(fd,"*EOD*\n");


      fprintf(fd,"\n*HLS+00*\n");
      for(i=0; i<4; i++)
	  {
        for(j=0; j<8; j++)
		{
          fprintf(fd,"%10u",hls_0[i*8+j]);
          fprintf(fd," ");
		}
        fprintf(fd,"\n");
      }
      fprintf(fd,"*EOD*\n");


      fprintf(fd,"\n*HLS+01*\n");
      for(i=0; i<4; i++)
	  {
        for(j=0; j<8; j++)
		{
          fprintf(fd,"%10u",hls_1[i*8+j]);
          fprintf(fd," ");
		}
        fprintf(fd,"\n");
      }
      fprintf(fd,"*EOD*\n");


      fprintf(fd,"\n*ECS 00*\n");
      for(i=0; i<12; i++)
	  {
        for(j=0; j<8; j++)
		{
          fprintf(fd,"%10u",ecs0[i*8+j]);
          fprintf(fd," ");
		}
        fprintf(fd,"\n");
      }
      fprintf(fd,"*EOD*\n");


      fprintf(fd,"\n*SCS 00*\n");
      for(i=0; i<24; i++)
	  {
        for(j=0; j<8; j++)
		{
          fprintf(fd,"%10u",scs0[i*8+j]);
          fprintf(fd," ");
		}
        fprintf(fd,"\n");
      }
      fprintf(fd,"*EOD*\n");


      fprintf(fd,"\n*TRGS00*\n");
      for(i=0; i<12; i++)
	  {
        for(j=0; j<8; j++)
		{
          fprintf(fd,"%10u",trgs0[i*8+j]);
          fprintf(fd," ");
		}
        fprintf(fd,"\n");
      }
      fprintf(fd,"*EOD*\n");


      fprintf(fd,"\n*TRGS01*\n");
      for(i=0; i<12; i++)
	  {
        for(j=0; j<8; j++)
		{
          fprintf(fd,"%10u",trgs1[i*8+j]);
          fprintf(fd," ");
		}
        fprintf(fd,"\n");
      }
      fprintf(fd,"*EOD*\n");


      fprintf(fd,"\n*S1ST00*\n");
      for(i=0; i<4; i++)
	  {
        for(j=0; j<8; j++)
		{
          fprintf(fd,"%10u",s1st0[i*8+j]);
          fprintf(fd," ");
		}
        fprintf(fd,"\n");
      }
      for(j=32; j<35; j++) fprintf(fd,"%10u",s1st0[j]);
      fprintf(fd," \n");
      fprintf(fd,"*EOD*\n");


      fprintf(fd,"\n*STSN00*\n");
      for(i=0; i<4; i++)
	  {
        for(j=0; j<8; j++)
		{
          fprintf(fd,"%10u",stsn0[i*8+j]);
          fprintf(fd," ");
		}
        fprintf(fd,"\n");
      }
      fprintf(fd,"*EOD*\n");


      fprintf(fd,"\n*TGS 00*\n");
      for(i=0; i<24; i++)
	  {
        for(j=0; j<8; j++)
		{
          fprintf(fd,"%10u",tgs0[i*8+j]);
          fprintf(fd," ");
		}
        fprintf(fd,"\n");
      }
      fprintf(fd,"*EOD*\n");


      fclose(fd);
	}
    else
	{
      printf("Cannot open file >%s< for writing\n",str);
      return(ERR_WRITING);
	}
  }
  else
  {
    printf("File >%s< exist - do nothing\n",str);
    fclose(fd);
  }
  }


  return(0);
}
