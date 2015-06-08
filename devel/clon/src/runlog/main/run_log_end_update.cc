//
//  run_log_end_update
//
// updates run log end info in database
//
// usage: run_log_end_update -a clasprod <runnumber>
//
//  ejw, 2-mar-98


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
/*
#include <string.h>
#include <stdlib.h>
#include <sys/time.h>
*/

// for ipc
#include <rtworks/cxxipc.hxx>


// online and coda stuff
extern "C"{
#include <clas_ipc_prototypes.h>
}


// flags to inhibit event recording, etc.
static int no_dbr    = 0;
static int debug     = 0;


// misc variables
static char *application      	 = (char*)"clastest";
static char *uniq_dgrp        	 = (char*)"run_log_end_update";
static char *id_string        	 = (char*)"run_log_end_update";
static char *dest             	 = (char*)"dbrouter";
static char session[50]          = "";
static int gmd_time           	 = 5;
static int nruns              	 = 0;
static char line[2048];
static char filename[128];
static char temp[128];


// end data
static char end_date[30];
static char location[256];
static char filebase[256];
static unsigned long clock_all, clock_active, clock_live;
static unsigned long fcup_all, fcup_active, fcup_live;
static unsigned long trig_presc[13], trig_event[13], trig_file[13];


// prototypes
void decode_command_line(int argc, char **argv);
void collect_recovery_data(char *runnum);
void insert_into_database(char *sql);
int find_tag_line(ifstream &file, const char *tag, char buffer[], int buflen);
int get_next_line(ifstream &file, char buffer[], int buflen);


// program start time
static time_t start=time(NULL);


// ref to server (connection created later)
TipcSrv &server=TipcSrv::Instance();



//--------------------------------------------------------------------------

int
main(int argc,char **argv)
{

  // decode command line
  decode_command_line(argc,argv);


  // set session name if not specified via env variable or on command line
  if(strlen(session)==0) strcpy(session,"clasprod");


  // disable GMD timeout and connect to server
  if(debug==0)
  {
    if(no_dbr==0)
    {
      T_OPTION opt=TutOptionLookup("Server_Delivery_Timeout");
      TutOptionSetNum(opt,0.0);
    }
    dbr_init(uniq_dgrp,application,id_string);
  }

  // recovery mode
  for(int i=nruns; i<argc; i++)
  {  
    // collect data for recovery
    printf("processing run # %s\n",argv[i]);
    collect_recovery_data(argv[i]);
  }


  // allow gmd to acknowledge receipt and close connection
  if(debug==0)
  {
    if(no_dbr==0) dbr_check((double) gmd_time);
    dbr_close();
  }
  

  // done
  exit(EXIT_SUCCESS);
}
       

//----------------------------------------------------------------


void
collect_recovery_data(char *runnum)
{
  FILE *fd;
  static char sql[2000];
  char session_name[50];
  char *comma = ",", *prime = "'";
  int i, nfile, nlong, nevent, nerror;
  char *p, name_in[256];
  char tmp[1024], chrun[20], end_date[30];
  int clock, clock_active, clock_live, fcup, fcup_active, fcup_live;
  int i1[13], i2[13], i3[13];

  // process ER run file summary file
  sprintf(name_in,"%s/run_files/runfile_%s_0%s.txt",getenv("CLON_PARMS"),session,runnum);
  fd = fopen(name_in,"r");
  if(fd == NULL)
  {
    printf("File >%s< does not exist, check for restored one\n",name_in);
    sprintf(name_in,"%s/run_files/runfile_%s_0%s.restored.txt",getenv("CLON_PARMS"),session,runnum);
    fd = fopen(name_in,"r");
    if(fd == NULL)
    {
      printf("File >%s< does not exist - exit\n",name_in);
      exit(0);
	}
    else
	{
      printf("Use file >%s<\n",name_in);
      fclose(fd);
    }
  }
  else
  {
    printf("Use file >%s<\n",name_in);
    fclose(fd);
  }

  // extract session from filename
  strcpy(temp,name_in);
  p = strchr(strstr(temp,"runfile_"),'_')+1;
  *strchr(p,'_')=' ';
  sscanf(p,"%s",session_name);
  printf("use session >%s<\n",session_name);

  ifstream file(name_in);
  if(file.is_open())
  {
    if(find_tag_line(file,"*FILES*",line,sizeof(line))==0)
	{
      while(get_next_line(file,line,sizeof(line))==0)
      {
        printf("tag *FILES* >%s<\n",line);
        sscanf(line,"%s %s %d %d %d",location,filebase,&nlong,&nevent,&nerror);

        // create sql string
        sprintf(sql,"INSERT INTO run_log_files (session_name,run,location,filename,nlong,nevent,nerror) VALUES ('%s',%s,'%s','%s',%d,%d,%d);",
          session_name,runnum,location,filebase,nlong,nevent,nerror);

        if(debug==0) insert_into_database(sql);
        else cout << "\nsql string for recovery run " << runnum << " is:\n\n" << sql << endl << endl;
      }
	}

    file.close();
  }


  // process end_session_runnum file
  sprintf(name_in,"%s/run_log/archive/end_%s_0%s.txt",getenv("CLON_PARMS"),session,runnum);


  fd = fopen(name_in,"r");
  if(fd == NULL)
  {
    printf("File >%s< does not exist, check for restored one\n",name_in);
    sprintf(name_in,"%s/run_log/archive/end_%s_0%s.restored.txt",getenv("CLON_PARMS"),session,runnum);
    fd = fopen(name_in,"r");
    if(fd == NULL)
    {
      printf("File >%s< does not exist - exit\n",name_in);
      exit(0);
	}
    else
	{
      printf("Use file >%s<\n",name_in);
      fclose(fd);
    }
  }
  else
  {
    printf("Use file >%s<\n",name_in);
    fclose(fd);
  }

  ifstream file2(name_in);
  if(file2.is_open())
  {
    if(find_tag_line(file2,"*SESSION*",line,sizeof(line))==0)
    {
      get_next_line(file2,line,sizeof(line));
      printf("tag *SESSION* >%s<\n",line);
      strncpy(session_name,line,sizeof(session_name));
    }

    if(find_tag_line(file2,"*RUN*",line,sizeof(line))==0)
    {
      get_next_line(file2,line,sizeof(line));
      printf("tag *RUN* >%s<\n",line);
      strncpy(chrun,line,sizeof(chrun));
    }
	
    if(find_tag_line(file2,"*END_DATE*",line,sizeof(line))==0)
    {
      get_next_line(file2,line,sizeof(line));
      printf("tag *END_DATE* >%s<\n",line);
      strncpy(end_date,line,sizeof(end_date));
    }

    if(find_tag_line(file2,"*FILEBASE*",line,sizeof(line))==0)
    {
      get_next_line(file2,line,sizeof(line));
      printf("tag *FILEBASE* >%s<\n",line);
      strncpy(filebase,line,sizeof(filebase));
    }

    if(find_tag_line(file2,"*FILES*",line,sizeof(line))==0)
    {
      get_next_line(file2,line,sizeof(line));
      printf("tag *FILES* >%s<\n",line);
      sscanf(line,"%d %d %d %d",&nfile,&nlong,&nevent,&nerror);
    }

    if(find_tag_line(file2,"*SCALERS*",line,sizeof(line))==0)
    {
      get_next_line(file2,line,sizeof(line));
      printf("tag *SCALERS* >%s<\n",line);
      sscanf(line,"%d %d %d",&clock,&clock_active,&clock_live);
      get_next_line(file2,line,sizeof(line));
      printf("tag *SCALERS* >%s<\n",line);
      sscanf(line,"%d %d %d",&fcup,&fcup_active,&fcup_live);
    }

    if(find_tag_line(file2,"*PRESCALE_COUNT*",line,sizeof(line))==0)
    {
      get_next_line(file2,line,sizeof(line));
      printf("tag *PRESCALE_COUNT* >%s<\n",line);
      sscanf(line,"%d %d %d %d %d %d %d %d %d %d %d %d %d",
        &i1[0],&i1[1],&i1[2],&i1[3],&i1[4],&i1[5],&i1[6],&i1[7],&i1[8],&i1[9],&i1[10],&i1[11],&i1[12]);
	}

    if(find_tag_line(file2,"*EVENT_COUNT*",line,sizeof(line))==0)
    {
      get_next_line(file2,line,sizeof(line));
      printf("tag *EVENT_COUNT* >%s<\n",line);
      sscanf(line,"%d %d %d %d %d %d %d %d %d %d %d %d %d",
        &i2[0],&i2[1],&i2[2],&i2[3],&i2[4],&i2[5],&i2[6],&i2[7],&i2[8],&i2[9],&i2[10],&i2[11],&i2[12]);
	}

    if(find_tag_line(file2,"*FILE_COUNT*",line,sizeof(line))==0)
    {
      get_next_line(file2,line,sizeof(line));
      printf("tag *FILE_COUNT* >%s<\n",line);
      sscanf(line,"%d %d %d %d %d %d %d %d %d %d %d %d %d",
        &i3[0],&i3[1],&i3[2],&i3[3],&i3[4],&i3[5],&i3[6],&i3[7],&i3[8],&i3[9],&i3[10],&i3[11],&i3[12]);
	}



	/*
    // search for L1STAT tag
    if(find_tag_line(file,"*L1STAT*",line,sizeof(line))==0)
    {
      get_next_line(file,line,sizeof(line));
      printf("tag *L1STAT* >%s<\n",line);
      istrstream l1stat(line,sizeof(line));
      for(i=0; i<12; i++) l1stat >> trig_file[i];
      trig_file[12]=nevent;
    }

    // form end date using EOR tag in file, or current time if EOR tag not in file
    if(find_tag_line(file,"*EOR*",line,sizeof(line))==0)
    {
      get_next_line(file,line,sizeof(line));
      printf("tag *EOR* >%s<\n",line);
      time_t eor=atoi(line);
      tm *tstruct = localtime(&eor);
      strftime(end_date,25,"%Y-%m-%d %H:%M:%S",tstruct);
    }
    else
    {
      tm *tstruct = localtime(&start);
      strftime(end_date,25,"%Y-%m-%d %H:%M:%S",tstruct);
    }
	*/

  
    file2.close();
  }



  // create sql string
  sprintf(sql,"INSERT INTO run_log_end (session_name,run,end_date,end_ok,filebase,nfile,nevent,nerror,nlong,"
    "fcup,fcup_active,fcup_live,clock,clock_active,clock_live,"
    "trig_presc_bit1,trig_presc_bit2,trig_presc_bit3,trig_presc_bit4,trig_presc_bit5,trig_presc_bit6,"
    "trig_presc_bit7,trig_presc_bit8,trig_presc_bit9,trig_presc_bit10,trig_presc_bit11,trig_presc_bit12,trig_presc_all,"
    "trig_event_bit1,trig_event_bit2,trig_event_bit3,trig_event_bit4,trig_event_bit5,trig_event_bit6,"
    "trig_event_bit7,trig_event_bit8,trig_event_bit9,trig_event_bit10,trig_event_bit11,trig_event_bit12,trig_event_all,"
    "trig_file_bit1,trig_file_bit2,trig_file_bit3,trig_file_bit4,trig_file_bit5,trig_file_bit6,"
    "trig_file_bit7,trig_file_bit8,trig_file_bit9,trig_file_bit10,trig_file_bit11,trig_file_bit12,trig_file_all) VALUES"
    " ('%s',%s,'%s','%s','%s',%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,"
    "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d);",
    session_name,chrun,end_date,"N",filebase,nfile,nevent,nerror,nlong,
    fcup,fcup_active,fcup_live,clock,clock_active,clock_live,
	i1[0],i1[1],i1[2],i1[3],i1[4],i1[5],i1[6],i1[7],i1[8],i1[9],i1[10],i1[11],i1[12],
	i2[0],i2[1],i2[2],i2[3],i2[4],i2[5],i2[6],i2[7],i2[8],i2[9],i2[10],i2[11],i2[12],
	i3[0],i3[1],i3[2],i3[3],i3[4],i3[5],i3[6],i3[7],i3[8],i3[9],i3[10],i3[11],i3[12]);

  if(debug==0) insert_into_database(sql);
  else cout << "\nsql string for recovery run " << runnum << " is:\n\n" << sql << endl << endl;

  return;
}



//--------------------------------------------------------------------------


void
insert_into_database(char *sql)
{

  if(sql==NULL) return;

  // dbr message
  if(no_dbr==0)
  {
    TipcMsg dbr = TipcMsg((T_STR)"dbr_request");
    dbr.Dest(dest);
    dbr.Sender(uniq_dgrp);
    dbr.UserProp(0.0);
    dbr.DeliveryMode(T_IPC_DELIVERY_ALL);
    
    // fill and send message 
    dbr << (T_INT4) 1 << sql;
    server.Send(dbr);
    printf("dbr message sent\n");
  }

  // flush messages
  server.Flush();

  return;
}

//----------------------------------------------------------------
  

void
decode_command_line(int argc, char **argv)
{
  int i=1;
  char *help="\nusage:\n\n run_log_end_update [-a application] [-u uniq_dgrp] [-i id_string] [-debug]\n"
               "        [-d destination] [-no_dbr] \n"
               "        [-s session] [-g gmd_time] run1_number run2_number ...\n\n\n";


  while(i<argc) {
    
    if(strncasecmp(argv[i],"-h",2)==0){
      printf(help);
      exit(EXIT_SUCCESS);
    }
    else if (strncasecmp(argv[i],"-",1)!=0){
      nruns=i;
      return;
    }
    else if (strncasecmp(argv[i],"-debug",6)==0){
      debug=1;
      i=i+1;
    }
    else if (strncasecmp(argv[i],"-no_dbr",7)==0){
      no_dbr=1;
      i=i+1;
    }
    else if (strncasecmp(argv[i],"-a",2)==0){
      application=strdup(argv[i+1]);
      i=i+2;
    }
    else if (strncasecmp(argv[i],"-u",2)==0){
      uniq_dgrp=strdup(argv[i+1]);
      i=i+2;
    }
    else if (strncasecmp(argv[i],"-i",2)==0){
      id_string=strdup(argv[i+1]);
      i=i+2;
    }
    else if (strncasecmp(argv[i],"-d",2)==0){
      dest=strdup(argv[i+1]);
      i=i+2;
    }
    else if (strncasecmp(argv[i],"-g",2)==0){
      gmd_time=atoi(argv[i+1]);
      i=i+2;
    }
    else if (strncasecmp(argv[i],"-s",2)==0){
      strcpy(session,argv[i+1]);
      i=i+2;
    }
  }
}


//---------------------------------------------------------------------
