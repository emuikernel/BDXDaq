//
//  run_log_end
//
//  collects and inserts run log end info into database
//  assumes get_eor_scalers already run (looks for file in archive area)
//
//  If optional file list given, makes belated run_log_end entries in "recovery" mode
// 
//  Appends info to file written by ER (for backup), tags ER file
//  In normal mode, also creates info_server message,
//     dies if no ER file
//
//
// Usage: run_log_end -a clasprod (from rcscript)
//        run_log_end -a clasprod -s clasprod -debug *.txt (if recovering and debugging)
//        cd $CLON_PARMS/run_files; run_log_end -a clasprod -s clasprod -debug runfile_clasprod_060067.txt
//        run_log_end -a clasprod -s clasprod *.txt (if recovering)
//
//  Recovery mode:
//    NOTE: only 'clasprod' files will be processed, the rest will be ignored
//
//
//  ejw, 17-mar-98
// Sergey Boyarinov: fix recovery part June 2009


// for posix
#define _POSIX_SOURCE_ 1
#define __EXTENSIONS__


// system stuff

using namespace std;
#include <strstream>

#include <iostream.h>
#include <iomanip.h>
#include <fstream.h>
#include <sys/time.h>


// for ipc
#include <rtworks/cxxipc.hxx>


// online and coda stuff
extern "C"{
#include <clas_ipc_prototypes.h>
}


// flags to inhibit event recording, etc.
static int no_dbr    = 0;
static int no_info   = 0;
static int no_file   = 0;
static int debug     = 0;


// misc variables
static char *application      	 = (char*)"clastest";
static char *uniq_dgrp        	 = (char*)"run_log_end";
static char *id_string        	 = (char*)"run_log_end";
static char *dest             	 = (char*)"dbrouter";
static char *msql_database    	 = (char*)"clasrun";
static char session[50]          = "";
static int gmd_time           	 =  5;
static int filep              	 = 0;
static int force              	 = 0;
static int min_file_age          = 5;   // hours
static time_t now                = time(NULL);

static char *archive_file_name   = (char*)"run_log/archive/end_%s_%06d.txt";
static char *run_file_summary 	 = (char*)"run_files/runfile_%s_%06d.txt";
static char *scaler_eor_name     = (char*)"scalers/archive/eor_%s_%06d.txt";
static char *scaler_file_name    = (char*)"scalers/archive/scalers_%s_%06d.txt";
static char *envrun              = getenv("RUN_NUMBER");

static char line[1024];
static char filename[128];
static char temp[128];


// end data
static int run;
static int current_run;
static char end_date[30];
static char location[80];
static char filebase[80];
static long nfile  = 0;
static long nevent = 0;
static long nlong  = 0;
static long nerror = 0;


// scalers
static unsigned long eor_scalers[64];
static unsigned long clock_all, clock_active, clock_live;
static unsigned long fcup_all, fcup_active, fcup_live;
static unsigned long trig_presc[13], trig_event[13], trig_file[13];


// prototypes
void decode_command_line(int argc, char **argv);
void collect_data(strstream &sql_string, int recover, char *recovery_filename);
void get_scaler_data(void);
void insert_into_ipc(char *sql);
void insert_into_file();
int find_tag_line(ifstream &file, const char *tag, char buffer[], int buflen);
int get_next_line(ifstream &file, char buffer[], int buflen);
char *env_name(const char *env, char *name);
extern "C" {
int get_run_number(const char *msql_database, const char *session);
}


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

  // no ipc or file in recovery mode
  if(filep!=0)
  {
    no_info=1;
    no_file=1;
  }

  // set session name if not specified via env variable or on command line
  if(filep==0)
  {
    if(strlen(session)==0)strcpy(session,"clasprod");
  }
    
  // disable GMD timeout and connect to server
  if(debug==0)
  {
    if(no_dbr==0)
    {
      T_OPTION opt=TutOptionLookup((T_STR)"Server_Delivery_Timeout");
      TutOptionSetNum(opt,0.0);
    }
    dbr_init(uniq_dgrp,application,id_string);
  }


  // normal mode
  if(filep==0)
  {

    // get run number from env var if it exists, otherwise try msql
    if(envrun!=NULL)
    {
      run=atoi(envrun);
    }
    else
    {
      cerr << "?No RUN_NUMBER env var...using msql" << endl;
      run=get_run_number(msql_database,session);
    }

    // collect data in normal mode (returns sql string)
    strstream sql_string;
    collect_data(sql_string,0,NULL);


    // write run end info to file
    if((no_file==0)&&(debug==0))insert_into_file();


    // ship sql string to database router and/or info_server
    if(debug==0)insert_into_ipc(sql_string.str());
    
    
    // debug...just print sql string
    if(debug!=0){
      cout << "\nsql string for normal run " << run << " is:\n\n" << sql_string.str() << endl << endl;
    }


  // recovery mode
  }
  else
  {

    for(int i=filep; i<argc; i++)
    {

      ifstream file(argv[i]);
      if(file.is_open()) {
	
	  // extract run and session from filename
	  strcpy(temp,argv[i]);
	  char *p = strchr(strstr(temp,"runfile_"),'_')+1;
	  *strchr(p,'_')=' ';
	  sscanf(p,"%s %d.txt",session,&run);
	
      if( strncmp(session,"clasprod",8) )
	  {
        /*printf("skip session >%s<\n",session);*/
        continue;
	  }
      else
	  {
        /*printf("run %d, session >%s<\n",run,session)*/;
	  }

	  // skip file if current run and not old enough
	  current_run=get_run_number(msql_database,session);
	  if(find_tag_line(file,"*BOR*",line,sizeof(line))==0)
      {
	    get_next_line(file,line,sizeof(line));
	    int age = atoi(line);
	    if((run==current_run)&&
	       ((now-age)<min_file_age*3600))
        {
	      cout << "skipping file " << argv[i] << " current run file only " << (now-age)/3600 
		   << " hours old" << endl;
	      continue;
	    }
	  }
	
	
	  // don't process if already processed, unless forced
	  if((force==0)&&(find_tag_line(file,"*run_log_end*",line,sizeof(line))==0))
      {
        cout << "skip: already processed" << endl;
	    file.close();
	    continue;
	  }
	

	  // collect data for recovery mode
	  strstream sql_string;
	  collect_data(sql_string,1,argv[i]);


	  // ship sql string to database router
	  if(debug==0)insert_into_ipc(sql_string.str());

      
	  // write run end info to file
	  if((no_file==0)&&(debug==0))insert_into_file();


	  // debug...just print sql string
	  if(debug!=0)
      { 
	    cout << "\nsql string for recovery run " << run << " is:\n\n" << sql_string.str()<<endl<<endl;
	  }

      }
    }
  }


  // allow gmd to acknowledge receipt and close connection
  if(debug==0) {
    if(no_dbr==0)dbr_check((double) gmd_time);
    dbr_close();
  }
  

  // done
  exit(EXIT_SUCCESS);
}
       

//----------------------------------------------------------------


void
collect_data(strstream &sql, int recover, char *recovery_filename)
{
  long nevt,nlng,nerr;
  const char *comma = ",", *prime = "'";
  int i;


  // read ER run file summary info and sum up stats
  nfile  = 0;
  nevent = 0;
  nlong  = 0;
  nerror = 0;
  filebase[0]=NULL;
  if(recover==0)
  {
    sprintf(temp,run_file_summary,session,run);
    strcpy(filename,env_name("CLON_PARMS",temp));
  }
  else
  {
    strcpy(filename,recovery_filename);
  }

  ifstream file(filename);
  if(file.is_open())
  {
    // search for FILES tag...start at beginning if not there (n.b. old files don't have the tag)
    // n.b. count words in kbytes
    if(find_tag_line(file,"*FILES*",line,sizeof(line))!=0)file.seekg(0,ios::beg);
    while(get_next_line(file,line,sizeof(line))==0)
    {
      sscanf(line,"%s %s %d %d %d",location,filebase,&nlng,&nevt,&nerr);
      nfile++;
      nlong+=nlng/1024;
      nevent+=nevt;
      nerror+=nerr;
    }

    // search for L1STAT tag
    if(find_tag_line(file,"*L1STAT*",line,sizeof(line))==0)
    {
      get_next_line(file,line,sizeof(line));
      istrstream l1stat(line,sizeof(line));
      for(i=0; i<12; i++) l1stat >> trig_file[i];
      trig_file[12]=nevent;
    }

    // get end date (default is now if no time in file)
    time_t eortime = start;
    if(find_tag_line(file,"*EOR*",line,sizeof(line))==0)
    {
      get_next_line(file,line,sizeof(line));
      sscanf(line,"%d",&eortime);
    } 
    tm *tstruct = localtime(&eortime);
    /*sergey strftime(end_date, 25, "%d-%b-%Y %H:%M", tstruct);*/
    strftime(end_date,25,"%Y-%m-%d %H:%M:%S",tstruct);

    file.close();

  }
  else
  {
    cerr << "Unable to open " << filename << endl;
    if(recover==0)exit(EXIT_FAILURE);
  }


  // read scalers from archive file
  get_scaler_data();


  // remove extension from last data file name to create filebase (handles abc.def...xyz.a00)
  if(strrchr(filebase,'.')!=NULL)*strrchr(filebase,'.')='\0';


  // create sql string
  sql.setf(ios::showpoint);
  sql << "insert into run_log_end ("
      << "session_name,run,end_date,end_ok,filebase,nfile,nlong,nevent,nerror,"
      << "clock,clock_active,clock_live,fcup,fcup_active,fcup_live";
  for(i=1; i<=12; i++) sql << comma << "trig_presc_bit" << i; 
  sql << comma << "trig_presc_all";
  for(i=1; i<=12; i++) sql << comma << "trig_event_bit" << i; 
  sql << comma << "trig_event_all";
  for(i=1; i<=12; i++) sql << comma << "trig_file_bit" << i; 
  sql << comma << "trig_file_all";

  sql << ") values ("
      << prime << session << prime
      << comma << run
      << comma << prime << end_date << prime;
  if(recover==0) {sql << ",'Y'";} else {sql << ",'N'";}
  sql << comma << prime << filebase << prime
      << comma << nfile << comma << nlong << comma << nevent << comma << nerror
      << comma << (long)clock_all << comma << (long)clock_active << comma << (long)clock_live
      << comma << (long)fcup_all  << comma << (long)fcup_active  << comma << (long)fcup_live;
  
  for(i=0; i<13; i++) sql << comma << (long)trig_presc[i];
  for(i=0; i<13; i++) sql << comma << (long)trig_event[i];
  for(i=0; i<13; i++) sql << comma << (long)trig_file[i];

  sql << ")" << ends;
  

  // append sql string to ER file as backup
  if(debug==0)
  {
    if(recover==0)
    {
      sprintf(temp,run_file_summary,session,run);
      strcpy(filename,env_name("CLON_PARMS",temp));
    }
    else
    {
      strcpy(filename,recovery_filename);
    }
    ofstream ofile(filename,ios::app);
    if(ofile.is_open())
    {
      ofile << "\n\n*run_log_end*" << endl;
      if(recover==0)
      {
	    ofile << "#  Processed in run_log_end normal mode on " << ctime(&start);
      }
      else
      {
	    ofile << "#  Processed in run_log_end recovery mode on " << ctime(&start);
      }
      ofile << sql.str() << endl << endl;
      ofile.close();
    }
  }

  return;
}

//--------------------------------------------------------------------------


void
get_scaler_data(void)
{
  int i,j,k,ind;

  // get scaler data from scaler file
  for(i=0; i<64; i++)eor_scalers[i]=0;
  sprintf(filename,scaler_file_name,session,run);
  ifstream arch_file(env_name("CLON_PARMS",filename));
  if(arch_file.is_open())
  {
      if(find_tag_line(arch_file,"*TRGS00*",line,sizeof(line))==0)
      {
	    for(i=0; i<64; i+=8)
        {
	      get_next_line(arch_file,line,sizeof(line));
	      istrstream arch(line,sizeof(line));
	      for(j=i; j<i+8; j++) arch >> eor_scalers[j];
	    }
      }
      arch_file.close();
      
  }
  else
  {
    sprintf(filename,scaler_file_name,session,run);
    ifstream eor_file(env_name("CLON_PARMS",filename));
    if(eor_file.is_open())
    {
	  if(find_tag_line(eor_file,"*TRGS00*",line,sizeof(line))==0)
      {
	    for(i=0; i<64; i+=8)
        {
		  get_next_line(eor_file,line,sizeof(line));
		  istrstream eor(line,sizeof(line));
		  for(j=i; j<i+8; j++) eor >> eor_scalers[j];
	    }
	  }
	  eor_file.close();
    }
    else
    {
	  cerr << "Unable to open scaler archive or eor file, run = " << run << endl;
    }
  }
  

  // extract clocks and fcups
  clock_all    = eor_scalers[ 0];
  fcup_all     = eor_scalers[ 1];
  clock_active = eor_scalers[16];
  fcup_active  = eor_scalers[17];
  clock_live   = eor_scalers[32];
  fcup_live    = eor_scalers[33];
  for(i=0; i<13; i++) trig_presc[i]=eor_scalers[48+i];
  

  // read software scaler info from scaler archive file (S1ST tag)
  for(i=0; i<13; i++) trig_event[i]=0;
  sprintf(filename,scaler_file_name,session,run);
  ifstream file(env_name("CLON_PARMS",filename));
  if(file.is_open()) {
    if(find_tag_line(file,"*S1ST00*",line,sizeof(line))==0) {
      get_next_line(file,line,sizeof(line));
      k=0; ind=0; for(i=0; i<8; i++) {k+=ind; sscanf(&line[k],"%d %n",&trig_event[i],&ind);}
      get_next_line(file,line,sizeof(line));
      k=0; ind=0; for(i=8; i<13; i++) {k+=ind; sscanf(&line[k],"%d %n",&trig_event[i],&ind);}
    }
    file.close();
  } else {
    cerr << "Unable to open scaler file " << filename << endl;
  }


  return;
}


//---------------------------------------------------------------------------


void
insert_into_ipc(char *sql)
{
  if(sql==NULL)return;


  // dbr message
  if(no_dbr==0) {

    TipcMsg dbr = TipcMsg((T_STR)"dbr_request");
    dbr.Dest(dest);
    dbr.Sender(uniq_dgrp);
    dbr.UserProp(0.0);
    dbr.DeliveryMode(T_IPC_DELIVERY_ALL);
    
    // fill and send message 
    dbr << (T_INT4) 1 << sql;
    server.Send(dbr);
  }


  // info server message
  if(no_info==0) {

    TipcMsg info = TipcMsg((T_STR)"info_server");
    info.Dest((T_STR)"info_server/in/run_log_end");
    info.Sender(uniq_dgrp);
    
    // fill and send msg
    info << (T_STR)"run_log_end"
	 << session << (T_INT4) run << end_date 
	 << filebase << (T_INT4) nfile << (T_INT4) nlong << (T_INT4) nevent << (T_INT4) nerror
	 << (T_INT4) clock_all << (T_INT4) clock_active << (T_INT4) clock_live
	 << (T_INT4) fcup_all << (T_INT4) fcup_active  << (T_INT4) fcup_live;
    server.Send(info);
  }


  // flush messages
  server.Flush();


  return;
}

//----------------------------------------------------------------
  

void
insert_into_file()
{
  int i;


  sprintf(filename,archive_file_name,session,run);
  ofstream file(env_name("CLON_PARMS",filename));
  if(!file.is_open()) return;


  file << "\n*SESSION*\n"  << session << endl;
  file << "\n*RUN*\n"      << run << endl;
  file << "\n*END_DATE*\n" << end_date << endl;
  file << "\n*FILEBASE*\n" << filebase << endl;
  file << "\n*FILES* --- nfile,nlong,nevent,nerror" << endl;
  file << nfile << " " << nlong << " " << nevent << " " << nerror << endl;
  file << "\n*SCALERS* --- clock all,active,live; fcup all,active,live" << endl;
  file << clock_all << " " << clock_active << " " << clock_live << endl;
  file << fcup_all  << " " << fcup_active  << " " << fcup_live  << endl;

  file << "\n*PRESCALE_COUNT*" << endl;
  for(i=0; i<13; i++) file << trig_presc[i] << " ";
  file << endl;

  file << "\n*EVENT_COUNT*\n";
  for(i=0; i<13; i++) file << trig_event[i] << " ";
  file << endl;

  file << "\n*FILE_COUNT*\n";
  for(i=0; i<13; i++) file << trig_file[i] << " ";
  file << endl;

  file.close();

  return;
}


//----------------------------------------------------------------
  

void
decode_command_line(int argc, char **argv)
{
  int i=1;
  const char *help="\nusage:\n\n  run_log_end [-a application] [-u uniq_dgrp] [-i id_string] [-debug]\n"
               "        [-d destination] [-m msql_database]]\n"
               "        [-no_dbr] [-no_info] [-no_file] [-mf min_file_age]\n"
               "        [-s session] [-g gmd_time] file1 file2 ...\n\n\n";

  while(i<argc) {
    
    if(strncasecmp(argv[i],"-h",2)==0){
      printf(help);
      exit(EXIT_SUCCESS);
    }
    else if (strncasecmp(argv[i],"-",1)!=0){
      filep=i;
      return;
    }
    else if (strncasecmp(argv[i],"-debug",6)==0){
      debug=1;
      i=i+1;
    }
    else if (strncasecmp(argv[i],"-force",6)==0){
      force=1;
      i=i+1;
    }
    else if (strncasecmp(argv[i],"-no_dbr",7)==0){
      no_dbr=1;
      i=i+1;
    }
    else if (strncasecmp(argv[i],"-no_info",8)==0){
      no_info=1;
      i=i+1;
    }
    else if (strncasecmp(argv[i],"-no_file",8)==0){
      no_file=1;
      i=i+1;
    }
    else if (strncasecmp(argv[i],"-mf",3)==0){
      min_file_age=atoi(argv[i+1]);
      i=i+2;
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
    else if (strncasecmp(argv[i],"-m",2)==0){
      msql_database=strdup(argv[i+1]);
      i=i+2;
    }
  }
}


//---------------------------------------------------------------------


char *
env_name(const char *env, char *name)
{
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


//---------------------------------------------------------------------
