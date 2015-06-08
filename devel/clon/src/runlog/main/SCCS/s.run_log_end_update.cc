h56212
s 00245/00152/00205
d D 1.4 09/05/29 11:09:32 boiarino 5 4
c *** empty log message ***
e
s 00001/00001/00356
d D 1.3 01/01/03 11:13:08 wolin 4 3
c is_open()
e
s 00001/00001/00356
d D 1.2 00/10/27 10:02:33 wolin 3 1
c const
e
s 00000/00000/00000
d R 1.2 98/03/02 14:48:04 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 run_log/s/run_log_end_update.cc
e
s 00357/00000/00000
d D 1.1 98/03/02 14:48:03 wolin 1 0
c 
e
u
U
f b 
f e 0
t
T
I 1
//
//  run_log_end_update
//
D 5
// updates run log end info in ingres database
E 5
I 5
// updates run log end info in database
E 5
//
I 5
// usage: run_log_end_update -a clasprod <runnumber>
//
E 5
//  ejw, 2-mar-98


// for posix
#define _POSIX_SOURCE_ 1
#define __EXTENSIONS__


// system stuff
I 5

using namespace std;
#include <strstream>
E 5
#include <iostream.h>
#include <iomanip.h>
#include <fstream.h>
D 5
#include <strstream.h>
E 5
I 5

#include <stdio.h>
/*
#include <string.h>
#include <stdlib.h>
E 5
#include <sys/time.h>
I 5
*/
E 5

D 5

E 5
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
D 5
static char *application      	 = "clastest";
static char *uniq_dgrp        	 = "run_log_end_update";
static char *id_string        	 = "run_log_end_update";
static char *dest             	 = "dbrouter";
static char *session          	 =  getenv("DD_NAME");
static int gmd_time           	 =  5;
static int filep              	 = 0;
static int force              	 = 0;

static char *archive_file_name   = "parms/run_log/archive/end_%s_%06d.txt";
static char *run_file_summary 	 = "parms/run_files/runfile_%s_%06d.txt";
static char *scaler_archive_name = "parms/scalers/archive/scal%06d.txt";

static char line[1024];
E 5
I 5
static char *application      	 = (char*)"clastest";
static char *uniq_dgrp        	 = (char*)"run_log_end_update";
static char *id_string        	 = (char*)"run_log_end_update";
static char *dest             	 = (char*)"dbrouter";
static char session[50]          = "";
static int gmd_time           	 = 5;
static int nruns              	 = 0;
static char line[2048];
E 5
static char filename[128];
static char temp[128];


// end data
D 5
static int run;
E 5
static char end_date[30];
D 5
static char location[80];
static char filebase[80];
static long nfile  = 0;
static long nevent = 0;
static long nlong  = 0;
static long nerror = 0;


// scalers
static unsigned long eor_scalers[64];
E 5
I 5
static char location[256];
static char filebase[256];
E 5
static unsigned long clock_all, clock_active, clock_live;
static unsigned long fcup_all, fcup_active, fcup_live;
static unsigned long trig_presc[13], trig_event[13], trig_file[13];


// prototypes
void decode_command_line(int argc, char **argv);
D 5
void collect_recovery_data(char *name_in, strstream &sql_string);
void insert_into_ipc(char *sql);
E 5
I 5
void collect_recovery_data(char *runnum);
void insert_into_database(char *sql);
E 5
D 3
int find_tag_line(ifstream &file, char *tag, char buffer[], int buflen);
E 3
I 3
int find_tag_line(ifstream &file, const char *tag, char buffer[], int buflen);
E 3
int get_next_line(ifstream &file, char buffer[], int buflen);
D 5
char *env_name(char *env, char *name);
extern "C" {
void DP_cmd_init(char *msql_tcp_host);
void DP_cmd(char *roc, char *cmd, char *buf, int timeout);
}
E 5


// program start time
static time_t start=time(NULL);


// ref to server (connection created later)
TipcSrv &server=TipcSrv::Instance();



//--------------------------------------------------------------------------

I 5
int
main(int argc,char **argv)
{
E 5

D 5
main(int argc,char **argv){


E 5
  // decode command line
  decode_command_line(argc,argv);


  // set session name if not specified via env variable or on command line
D 5
  if(session==NULL)session="clasprod";
E 5
I 5
  if(strlen(session)==0) strcpy(session,"clasprod");
E 5


  // disable GMD timeout and connect to server
D 5
  if(debug==0) {
    if(no_dbr==0) {
E 5
I 5
  if(debug==0)
  {
    if(no_dbr==0)
    {
E 5
      T_OPTION opt=TutOptionLookup("Server_Delivery_Timeout");
      TutOptionSetNum(opt,0.0);
    }
    dbr_init(uniq_dgrp,application,id_string);
  }

  // recovery mode
D 5
  for(int i=filep; i<argc; i++){
    
E 5
I 5
  for(int i=nruns; i<argc; i++)
  {  
E 5
    // collect data for recovery
D 5
    strstream sql_string;
    collect_recovery_data(argv[i],sql_string);
    
    // ship sql string to database router
    if(debug==0)insert_into_ipc(sql_string.str());
    
    // debug...just print sql string
    if(debug!=0){ 
      cout << "\nsql string for recovery run " << run << " is:\n\n" << sql_string.str()<<endl<<endl;
    }
E 5
I 5
    printf("processing run # %s\n",argv[i]);
    collect_recovery_data(argv[i]);
E 5
  }


  // allow gmd to acknowledge receipt and close connection
D 5
  if(debug==0) {
    if(no_dbr==0)dbr_check((double) gmd_time);
E 5
I 5
  if(debug==0)
  {
    if(no_dbr==0) dbr_check((double) gmd_time);
E 5
    dbr_close();
  }
  

  // done
  exit(EXIT_SUCCESS);
}
       

//----------------------------------------------------------------


D 5
void collect_recovery_data(char *name_in, strstream &sql){


E 5
I 5
void
collect_recovery_data(char *runnum)
{
  FILE *fd;
  static char sql[2000];
E 5
  char session_name[50];
D 5
  long nevt,nlng,nerr;
E 5
  char *comma = ",", *prime = "'";
D 5
  int i;
  char *p;
E 5
I 5
  int i, nfile, nlong, nevent, nerror;
  char *p, name_in[256];
  char tmp[1024], chrun[20], end_date[30];
  int clock, clock_active, clock_live, fcup, fcup_active, fcup_live;
  int i1[13], i2[13], i3[13];
E 5

I 5
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
E 5

I 5
  // extract session from filename
  strcpy(temp,name_in);
  p = strchr(strstr(temp,"runfile_"),'_')+1;
  *strchr(p,'_')=' ';
  sscanf(p,"%s",session_name);
  printf("use session >%s<\n",session_name);
E 5

D 5
  // read ER run file summary info and sum up stats
E 5
  ifstream file(name_in);
D 4
  if(!file.bad()) {
E 4
I 4
D 5
  if(file.is_open()) {
E 5
I 5
  if(file.is_open())
  {
    if(find_tag_line(file,"*FILES*",line,sizeof(line))==0)
	{
      while(get_next_line(file,line,sizeof(line))==0)
      {
        printf("tag *FILES* >%s<\n",line);
        sscanf(line,"%s %s %d %d %d",location,filebase,&nlong,&nevent,&nerror);
E 5
E 4

D 5
    // don't process if already processed, unless forced
    if((force==0)&&(find_tag_line(file,"*run_log_end*",line,sizeof(line))==0)) {
      file.close();
      return;
E 5
I 5
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
E 5
    }
D 5
    
    // process file
    filebase[0]=NULL;
E 5
I 5
  }
  else
  {
    printf("Use file >%s<\n",name_in);
    fclose(fd);
  }
E 5

D 5
    // search for FILES tag...start at beginning if not there (n.b. old files don't have the tag)
    nfile  = 0;
    nevent = 0;
    nlong  = 0;
    nerror = 0;
    if(find_tag_line(file,"*FILES*",line,sizeof(line))!=0)file.seekg(0,ios::beg);
    while(get_next_line(file,line,sizeof(line))==0){
      sscanf(line,"%s %s %d %d %d",location,filebase,&nlng,&nevt,&nerr);
      nfile++;
      nlong+=nlng;
      nevent+=nevt;
      nerror+=nerr;
E 5
I 5
  ifstream file2(name_in);
  if(file2.is_open())
  {
    if(find_tag_line(file2,"*SESSION*",line,sizeof(line))==0)
    {
      get_next_line(file2,line,sizeof(line));
      printf("tag *SESSION* >%s<\n",line);
      strncpy(session_name,line,sizeof(session_name));
E 5
    }

I 5
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
E 5
    // search for L1STAT tag
D 5
    if(find_tag_line(file,"*L1STAT*",line,sizeof(line))==0) {
E 5
I 5
    if(find_tag_line(file,"*L1STAT*",line,sizeof(line))==0)
    {
E 5
      get_next_line(file,line,sizeof(line));
I 5
      printf("tag *L1STAT* >%s<\n",line);
E 5
      istrstream l1stat(line,sizeof(line));
      for(i=0; i<12; i++) l1stat >> trig_file[i];
      trig_file[12]=nevent;
    }

    // form end date using EOR tag in file, or current time if EOR tag not in file
D 5
    if(find_tag_line(file,"*EOR*",line,sizeof(line))==0) {
E 5
I 5
    if(find_tag_line(file,"*EOR*",line,sizeof(line))==0)
    {
E 5
      get_next_line(file,line,sizeof(line));
I 5
      printf("tag *EOR* >%s<\n",line);
E 5
      time_t eor=atoi(line);
      tm *tstruct = localtime(&eor);
D 5
      strftime(end_date, 25, "%d-%b-%Y %H:%M", tstruct);
    } else {
E 5
I 5
      strftime(end_date,25,"%Y-%m-%d %H:%M:%S",tstruct);
    }
    else
    {
E 5
      tm *tstruct = localtime(&start);
D 5
      strftime(end_date, 25, "%d-%b-%Y %H:%M", tstruct);
E 5
I 5
      strftime(end_date,25,"%Y-%m-%d %H:%M:%S",tstruct);
E 5
    }
I 5
	*/

E 5
  
D 5
    file.close();
E 5
I 5
    file2.close();
E 5
  }


D 5
  // extract run and session from filename
  strcpy(temp,name_in);
  p = strchr(strstr(temp,"runfile_"),'_')+1;
  *strchr(p,'_')=' ';
  sscanf(p,"%s %d.txt",session_name,&run);
E 5

D 5

  // remove extension from last data file name to create filebase (handles abc.def...xyz.a00)
  if(strrchr(filebase,'.')!=NULL)*strrchr(filebase,'.')='\0';


E 5
  // create sql string
D 5
  sql.setf(ios::showpoint);
  sql << "update ingres.run_log_end set "
      << "nfile=" << nfile << ", nlong=" << nlong << ", nevent=" << nevent << ", nerror=" << nerror
      << " where run = " << run << ends;
  
E 5
I 5
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
E 5

D 5
  // append sql string to ER file as backup if found file data
  if(debug==0) {
    ofstream ofile(name_in,ios::app);
    ofile << "\n\n*run_log_end_update*" << endl;
    ofile << "#  Processed in run_log_end_update recovery mode on " << ctime(&start);
    ofile << sql.str() << endl << endl;
    ofile.close();
  }
E 5
I 5
  if(debug==0) insert_into_database(sql);
  else cout << "\nsql string for recovery run " << runnum << " is:\n\n" << sql << endl << endl;
E 5

  return;
}



//--------------------------------------------------------------------------


D 5
void insert_into_ipc(char *sql){
E 5
I 5
void
insert_into_database(char *sql)
{
E 5

I 5
  if(sql==NULL) return;
E 5

D 5
  if(sql==NULL)return;


E 5
  // dbr message
D 5
  if(no_dbr==0) {

    TipcMsg dbr = TipcMsg("dbr_request");
E 5
I 5
  if(no_dbr==0)
  {
    TipcMsg dbr = TipcMsg((T_STR)"dbr_request");
E 5
    dbr.Dest(dest);
    dbr.Sender(uniq_dgrp);
    dbr.UserProp(0.0);
    dbr.DeliveryMode(T_IPC_DELIVERY_ALL);
    
    // fill and send message 
    dbr << (T_INT4) 1 << sql;
    server.Send(dbr);
I 5
    printf("dbr message sent\n");
E 5
  }

D 5

E 5
  // flush messages
  server.Flush();

D 5

E 5
  return;
}

//----------------------------------------------------------------
  

D 5
void decode_command_line(int argc, char **argv) {

E 5
I 5
void
decode_command_line(int argc, char **argv)
{
E 5
  int i=1;
D 5
  char *help="\nusage:\n\n  run_log_end [-a application] [-u uniq_dgrp] [-i id_string] [-debug]\n"
               "        [-d destination] \n"
               "        [-no_dbr] \n"
               "        [-s session] [-g gmd_time] file1 file2 ...\n\n\n";
E 5
I 5
  char *help="\nusage:\n\n run_log_end_update [-a application] [-u uniq_dgrp] [-i id_string] [-debug]\n"
               "        [-d destination] [-no_dbr] \n"
               "        [-s session] [-g gmd_time] run1_number run2_number ...\n\n\n";
E 5


  while(i<argc) {
    
    if(strncasecmp(argv[i],"-h",2)==0){
      printf(help);
      exit(EXIT_SUCCESS);
    }
    else if (strncasecmp(argv[i],"-",1)!=0){
D 5
      filep=i;
E 5
I 5
      nruns=i;
E 5
      return;
    }
    else if (strncasecmp(argv[i],"-debug",6)==0){
      debug=1;
      i=i+1;
    }
D 5
    else if (strncasecmp(argv[i],"-force",6)==0){
      force=1;
      i=i+1;
    }
E 5
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
D 5
      session=strdup(argv[i+1]);
E 5
I 5
      strcpy(session,argv[i+1]);
E 5
      i=i+2;
    }
  }
}

D 5

//---------------------------------------------------------------------


char *env_name(char *env, char *name) {

  static char bigname[200];
  char *e=getenv(env);

  if(e!=NULL) {
    strcpy(bigname,e);
    strcat(bigname,"/");
    strcat(bigname,name);
  } else {
    strcpy(bigname,name);
  }
  
  return(bigname);
}

E 5

//---------------------------------------------------------------------
E 1
