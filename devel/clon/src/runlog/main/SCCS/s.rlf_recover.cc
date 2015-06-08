h09844
s 00656/00000/00000
d D 1.1 08/06/25 16:09:06 boiarino 1 0
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
//  rlf_recover.scc
//
//  collects and inserts run log files and end info into database
//  assumes get_eor_scalers already run (looks for file in archive area)
//
//  Appends info to file written by ER (for backup), tags ER file
//
//  ejw, 24-jun-98
//  ejw, 14-jun-2006 switched to mySQL


// CC -g -features=no%conststrings -o rlf_recover rlf_recover.cc   -I/usr/include/mysql -L/usr/lib/mysql -lmysqlclient -L/usr/local/clas/devel_new/SunOS_sun4u/lib -lutil -L/usr/local/coda/2.2.1/SunOS/lib -lmsql -lsocket -lnsl


// for posix
#define _POSIX_SOURCE_ 1
#define __EXTENSIONS__


// system stuff

using namespace std;
#include <strstream>

#include <stdio.h>
#include <iostream.h>
#include <iomanip.h>
#include <fstream.h>
#include <sys/time.h>
#include <strings.h>
#include <stdlib.h>


// mysql include files
#include <mysql/mysql.h>


static char *dbhost      = "clondb1";
static char *dbuser      = "clasrun";
static char *database    = (char*)"clasprod";
static MYSQL *dbhandle   = NULL;
static MYSQL_ROW row;
static MYSQL_RES *res;

static char *session     = "clastest";
static int run;
static int dbrun;
static int runbeg;
static int runend;


// flags to inhibit event recording, etc.
static int no_file    = 0;
static int update_end = 0;
static int debug      = 0;

static char *run_file_summary 	 = "run_files/archive/runfile_%s_%06d.txt";
static char *scaler_eor_name     = "scalers/archive/eor_%s_%06d.txt";
static char *scaler_file_name    = "scalers/archive/scalers_%s_%06d.txt";

static char line[1024];
static char filename[128];
static char scfilename[128];
static char arfilename[128];
static char temp[128];


// files data
static int nentry_files;


// end data
static int nentry_end;
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
void collect_file_data(void);
void collect_end_data(void);
void get_scaler_data(void);
void insert_into_db(char *sql);
int find_tag_line(ifstream &file, const char *tag, char buffer[], int buflen);
int get_next_line(ifstream &file, char buffer[], int buflen);
char *env_name(char *env, char *name);


// program start time
static time_t start=time(NULL);


//--------------------------------------------------------------------------


main(int argc,char **argv){

  strstream sql1,sql2,sql3,sql4;
  MYSQL_ROW bRow,cRow,eRow,fRow;
  MYSQL_RES *bRes,*cRes,*eRes,*fRes;
  int numRows;

  // synch with c i/o
  ios::sync_with_stdio();


  // decode command line
  decode_command_line(argc,argv);


  // get session name
  if(session==NULL) session=strdup("clasprod");


  // set readlock
  //  exec sql set lockmode session where readlock=nolock;


  // connect to database
  dbhandle=mysql_init(NULL);
  if(!mysql_real_connect(dbhandle,dbhost,dbuser,NULL,database,0,NULL,0)) {
    time_t now = time(NULL);
    cerr << "Unable to connect to database (error: " << mysql_errno(dbhandle) 
	 << ", " << mysql_error(dbhandle) << ") on " 
	 << ctime(&now) << endl;
    exit(EXIT_FAILURE);
  }


  // loop over runs
  for(run=runbeg; run<=runend; run++) {


    // check if files data already in database
    nentry_files=0;
    sql1 << "select run from run_log_files where session_name=\'" << session << "\' and run=" << run;

    if( (mysql_query(dbhandle,sql1.str())) != 0)
	{
      printf("ERROR in mysql_query >%s<\n",sql1.str());
      exit(EXIT_FAILURE);
    }

    if( !(fRes = mysql_store_result(dbhandle)) )
	{
      printf("ERROR1 in mysql_store_result\n");
      exit(EXIT_FAILURE);
	}
	else
	{
      numRows = mysql_num_rows(fRes);
      if(numRows == 0)
      {
        printf("INFO1: no rows found\n");
	  }
      else
      {
        nentry_files++;
        printf("set nentry_files=%d\n",nentry_files);
	  }
    }

    mysql_free_result(fRes);


    
    // check if end data already in database
    nentry_end=0;
    sql2 << "select run from run_log_end where session_name=\'" << session << "\' and run=" << run;

    if( (mysql_query(dbhandle,sql2.str())) != 0)
	{
      printf("ERROR in mysql_query >%s<\n",sql2.str());
      exit(EXIT_FAILURE);
    }

    if( !(eRes = mysql_store_result(dbhandle)) )
	{
      printf("ERROR2 in mysql_store_result\n");
      exit(EXIT_FAILURE);
	}
	else
	{
      numRows = mysql_num_rows(eRes);
      if(numRows == 0)
      {
        printf("INFO2: no rows found\n");
	  }
      else
      {
        nentry_end++;
        printf("set nentry_end=%d\n",nentry_end);
	  }
	}

    mysql_free_result(eRes);

    
    // get runfile name 
    sprintf(temp,run_file_summary,session,run);
    sprintf(filename,"%s",env_name("CLON_PARMS",temp));


    // make file entries if needed
    if(nentry_files<=0)
    {
      collect_file_data();
    }
    else
    {
      cout << "Run " << run << " file entries exist" << endl;
    }


    // make end entry if needed
    if((update_end==1)||(nentry_end<=0))
    {
      collect_end_data();
    }
    else
    {
      cout << "Run " << run << " end entry exists" << endl;
    }

  }


  // disconnect from database
  mysql_close(dbhandle);


  // done
  exit(EXIT_SUCCESS);
}


//----------------------------------------------------------------


void collect_file_data() {

  int nf = 0;
  int nlong, nevent, nerror;
  char loc[80], fname[80];
  static char comma          = ',';
  static char quote          = '\'';
  static char *preamble      = "insert into run_log_files(run,session_name,location,filename,"
    "nlong,nevent,nerror) values (";


  // open file
  ifstream file(filename);
  if(file.is_open()) {

    // search for FILES tag...start at beginning if not there (n.b. old files don't have the tag)
    if(find_tag_line(file,"*FILES*",line,sizeof(line))!=0)file.seekg(0,ios::beg);
    
    
    // loop over all entries, send dbr message for each
    while(get_next_line(file,line,sizeof(line))==0) {
      nf++;
      sscanf(line,"%s %s %d %d %d",loc,fname,&nlong,&nevent,&nerror);
      
      // form sql string
      strstream sql;
      sql << preamble
	  << run << comma 
	  << quote << session << quote << comma                
	  << quote << loc << quote << comma                
	  << quote << fname << quote << comma
	  << nlong/1024 << comma 
	  << nevent << comma 
	  << nerror << ")" << ends;
      
      
      // dump string if in debug mode
      if(debug==1){
	if(nf==1)cout << endl << "Run " << run << endl;
	cout << sql.str() << endl;
      }
      
      // ship to dbrouter
      insert_into_db(sql.str());
      
    }

    if(nf>0) {
      if(debug==0) {
	cout << "Run " << run << " formed files entry" << endl;
      } else {
	cout << endl;
      }
    } else {
      cout << "Run " << run << " has no files" << endl;
    }


  } else {
    cerr << "Files unable to open " << filename << endl;
  }
  
  return;
}


//----------------------------------------------------------------


void collect_end_data() {


  long nevt,nlng,nerr;
  char *comma = ",", *prime = "'";
  int i;
  strstream sql;


  // reset
  nfile  = 0;
  nevent = 0;
  nlong  = 0;
  nerror = 0;
  filebase[0]=NULL;
  for(i=0; i<64; i++)eor_scalers[i]=0;
  for(i=0; i<13; i++) trig_presc[i]=0;
  for(i=0; i<13; i++) trig_event[i]=0;
  for(i=0; i<13; i++) trig_file[i]=0;


  // read ER run file summary info and sum up stats
  ifstream file(filename);
  if(file.is_open()) {

    // search for FILES tag...start at beginning if not there (n.b. old files don't have the tag)
    // n.b. count words in kbytes
    if(find_tag_line(file,"*FILES*",line,sizeof(line))!=0)file.seekg(0,ios::beg);
    while(get_next_line(file,line,sizeof(line))==0) {
      sscanf(line,"%s %s %d %d %d",location,filebase,&nlng,&nevt,&nerr);
      nfile++;
      nlong+=nlng/1024;
      nevent+=nevt;
      nerror+=nerr;
    }

    // search for L1STAT tag
    if(find_tag_line(file,"*L1STAT*",line,sizeof(line))==0) {
      get_next_line(file,line,sizeof(line));
      istrstream l1stat(line,sizeof(line));
      for(i=0; i<12; i++) l1stat >> trig_file[i];
      trig_file[12]=nevent;
    }

    // get end date (default is now if no time in file)
    time_t eortime = start;
    if(find_tag_line(file,"*EOR*",line,sizeof(line))==0) {
      get_next_line(file,line,sizeof(line));
      sscanf(line,"%d",&eortime);
    } 
    tm *tstruct = localtime(&eortime);

    /*sergey strftime(end_date, 25, "%d-%b-%Y %H:%M", tstruct);*/
    strftime(end_date,25,"%Y-%m-%d %H:%M:%S",tstruct);

    file.close();
  } else {
    cerr << "End unable to open " << filename << endl;
  }
  
  
  // read scalers from archive file
  get_scaler_data();
  
  
  // remove extension from last data file name to create filebase (handles abc.def...xyz.a00)
  if(strrchr(filebase,'.')!=NULL)*strrchr(filebase,'.')='\0';


  // insert or update
  sql.setf(ios::showpoint);

  if(update_end==0) {
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
	<< comma << prime << end_date << prime 
	<< comma << prime << "N" << prime;
    sql << comma << prime << filebase << prime
	<< comma << nfile << comma << nlong << comma << nevent << comma << nerror
	<< comma << (long)clock_all << comma << (long)clock_active << comma << (long)clock_live
	<< comma << (long)fcup_all  << comma << (long)fcup_active  << comma << (long)fcup_live;
    
    for(i=0; i<13; i++) sql << comma << (long)trig_presc[i];
    for(i=0; i<13; i++) sql << comma << (long)trig_event[i];
    for(i=0; i<13; i++) sql << comma << (long)trig_file[i];
    
    sql << ")" << ends;


  } else {

    sql << "update run_log_end set "
        << "end_date=" << prime << end_date << prime << comma
        << "filebase=" << prime << filebase << prime << comma
	<< "nfile=" << nfile << comma 
	<< "nlong=" << nlong << comma 
	<< "nevent=" << nevent << comma 
	<< "nerror=" << nerror << comma
	<< "clock=" << (long)clock_all << comma 
	<< "clock_active=" << (long)clock_active << comma 
	<< "clock_live=" << (long)clock_live << comma
	<< "fcup=" << (long)fcup_all  << comma 
	<< "fcup_active=" << (long)fcup_active  << comma 
	<< "fcup_live=" << (long)fcup_live;

    for(i=0; i<12; i++) sql << comma << "trig_presc_bit" << i+1 << "=" << (long)trig_presc[i];
    sql << comma << "trig_presc_all=" << trig_presc[12];
    for(i=0; i<12; i++) sql << comma << "trig_event_bit" << i+1 << "=" << (long)trig_event[i];
    sql << comma << "trig_event_all=" << trig_event[12];
    for(i=0; i<12; i++) sql << comma << "trig_file_bit" << i+1 << "=" << (long)trig_file[i];
    sql << comma << "trig_file_all=" << trig_file[12];

    sql << " where " 
	<< "session_name=" << prime << session << prime
	<< " and run=" << run << ends;
  }


  // append sql string to ER file as backup
  if(debug==0) {
    if(no_file==0) {
      ofstream ofile(filename,ios::app);
      if(ofile.is_open()) {
	ofile << "\n\n*rlf_recover*" << endl;
	ofile << "#  Processed by rlf_recover on " << ctime(&start);
	ofile << sql.str() << endl << endl;
	ofile.close();
      }
    }
  }


  // ship sql string to database router
  insert_into_db(sql.str());

  
  // debug...just print sql string
  if(debug!=0) { 
    cout << "Run " << run << endl << sql.str() << endl << endl;
  } else {
    if(update_end==0) {
      cout << "Run " << run << " formed end entry" << endl;
    } else {
      cout << "Run " << run << " updated end entry" << endl;
    }
  }

  return;
}

//--------------------------------------------------------------------------


void get_scaler_data(void){

  int i,j,k,ind;


  // get scaler data from archive file...if unable try eor file
  sprintf(scfilename,scaler_file_name,session,run);
  ifstream arch_file(env_name("CLON_PARMS",scfilename));
  if(arch_file.is_open()) {
    if(find_tag_line(arch_file,"*TRGS00*",line,sizeof(line))==0) {
      for(i=0; i<64; i+=8) {
	get_next_line(arch_file,line,sizeof(line));
	istrstream arch(line,sizeof(line));
	for(j=i; j<i+8; j++) arch >> eor_scalers[j];
      }
    }
    arch_file.close();

  } else {
      sprintf(scfilename,scaler_eor_name,session,run);
      ifstream eor_file(env_name("CLON_PARMS",scfilename));
      if(eor_file.is_open()) {
	  if(find_tag_line(eor_file,"*TRGS*",line,sizeof(line))==0) {
	      for(i=0; i<64; i+=8) {
		  get_next_line(eor_file,line,sizeof(line));
		  istrstream eor(line,sizeof(line));
		  for(j=i; j<i+8; j++) eor >> eor_scalers[j];
	      }
	  }
	  eor_file.close();
      } else {
	  cerr << "Unable to open scaler archive or eor files, run = " << run << endl;
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
  

  // read software scaler info from archive file (S1ST tag)
  sprintf(scfilename,scaler_file_name,session,run);
  ifstream file(env_name("CLON_PARMS",scfilename));
  if(file.is_open()) {
    if(find_tag_line(file,"*S1ST00*",line,sizeof(line))==0) {
      get_next_line(file,line,sizeof(line));
      k=0; ind=0; for(i=0; i<8; i++) {k+=ind; sscanf(&line[k],"%d %n",&trig_event[i],&ind);}
      get_next_line(file,line,sizeof(line));
      k=0; ind=0; for(i=8; i<13; i++) {k+=ind; sscanf(&line[k],"%d %n",&trig_event[i],&ind);}
    }
  } else {
    cerr << "Unable to open scaler file " << scfilename << endl;
  }


  return;
}


//---------------------------------------------------------------------------


void insert_into_db(char *sql) {


  if(debug==1)return;
  if(sql==NULL)return;

  mysql_query(dbhandle,sql);
  unsigned int error = mysql_errno(dbhandle);
  if(error!=0) {
    cerr << "Error: " << error << "for: " << endl << sql << endl 
	 << mysql_error(dbhandle) << endl;
  }

  return;
}

//----------------------------------------------------------------
  

void decode_command_line(int argc, char **argv) {

  int i=1;
  char *help="\nusage:\n\n  rlf_recover [-s session] [-host dbhost] [-user dbuser] [-d database] [-u]\n"
             "              [-debug] [-no_file] runbeg [runend]\n\n";


  while(i<argc) {
    
    if(strncasecmp(argv[i],"-help",5)==0)
    {
      printf(help);
      exit(EXIT_SUCCESS);
    }
    else if (strncasecmp(argv[i],"-",1)!=0)
    {
      runbeg=atoi(argv[i]);
      if(argc>=i+2) runend=atoi(argv[i+1]); else runend=runbeg;
      return;
    }
    else if (strncasecmp(argv[i],"-host",5)==0)
    {
      dbhost=strdup(argv[i+1]);
      i=i+2;
    }
    else if (strncasecmp(argv[i],"-user",5)==0)
    {
      dbuser=strdup(argv[i+1]);
      i=i+2;
    }
    else if (strncasecmp(argv[i],"-debug",6)==0)
    {
      debug=1;
      i=i+1;
    }
    else if (strncasecmp(argv[i],"-no_file",8)==0)
    {
      no_file=1;
      i=i+1;
    }
    else if (strncasecmp(argv[i],"-d",2)==0)
    {
      database=strdup(argv[i+1]);
      i=i+2;
    }
    else if (strncasecmp(argv[i],"-s",2)==0)
    {
      session=strdup(argv[i+1]);
      i=i+2;
    }
    else if (strncasecmp(argv[i],"-u",2)==0)
    {
      update_end=1;
      i=i+1;
    }
    else
	{
      printf("ERROR: unknown flag >%s<\n",argv[i]);
	}
  }
}


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


//---------------------------------------------------------------------
E 1
