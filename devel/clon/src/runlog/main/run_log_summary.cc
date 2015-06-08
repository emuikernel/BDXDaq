//
//  run_log_summary.sc
//
//  generates run summary 
//
//
//   ejw, 27-jun-1997
//   ejw, 6-jun-2006  converted from ingres to mysql


// CC -g -features=no%conststrings -o run_log_summary run_log_summary.cc  -I/usr/include/mysql -L/usr/lib/mysql -lmysqlclient -L/usr/local/clas/devel_new/SunOS_sun4u/lib -lutil -L/usr/local/coda/2.2.1/SunOS/lib -lmsql -lsocket -lnsl


#define _POSIX_SOURCE 1
#define __EXTENSIONS__

using namespace std;
#include <strstream>

#include <fstream.h>
#include <iomanip.h>
#include <stdlib.h>
#include <strings.h>

#include <mysql/mysql.h>

static char *session        = (char*)"clasprod";
static char *msql_database  = "clasrun";
static int run              = 0;
static int runend           = 0;
static char *dbhost         = "clondb1";
static char *dbuser         = "clasrun";
static char *database       = "clasprod";
static MYSQL *dbhandle      = NULL;


// misc variables
static char *filename  = NULL;
static int debug;
static int nbegin;
static int ncomment;
static int nfiles;
static int nend;


// prototypes
void decode_command_line(int argc, char **argv);
void process_run(int &run, ostream &file);
char *get_col_by_name(MYSQL_RES *res, MYSQL_ROW row, char *name);
extern "C" {
int get_run_number(char *msql_database, char *session);
}


//------------------------------------------------------------------------------------


main(int argc, char **argv){


  // synch with c i/o
  ios::sync_with_stdio();


  // decode command line args
  decode_command_line(argc,argv);
  

  // set session name if not specified via env variable or on command line
  if(session==NULL)session="clasprod";

  
  // if no run specified just dump current run
  if(run<=0){
    run=get_run_number(msql_database,session);
    runend=run;
  }


  // set readlock
  //  exec sql set lockmode session where readlock=nolock;  ???


  // connect to database
  dbhandle=mysql_init(NULL);
  if(!mysql_real_connect(dbhandle,dbhost,dbuser,NULL,database,0,NULL,0)) {
    time_t now = time(NULL);
    cerr << "Unable to connect to database (error: " << mysql_errno(dbhandle) 
	 << ", " << mysql_error(dbhandle) << ") on " 
	 << ctime(&now) << endl;
    exit(EXIT_FAILURE);
  }


  // process the runs
  for(int i=run; i<=runend; i++){
    if(filename==NULL){
      process_run(i, cout);
    } else {
      ofstream file(filename);
      if(!file.is_open()){ cerr << "\n?unable to open " << filename << endl << endl; continue;}
      process_run(i, file);
      file.close();
    }
  }


  // done...close database
  mysql_close(dbhandle);
  exit(EXIT_SUCCESS);

}


//------------------------------------------------------------------------------------


void process_run(int &run, ostream &file) {

  strstream sql1,sql2,sql3,sql4;
  MYSQL_ROW bRow,cRow,eRow,fRow;
  MYSQL_RES *bRes,*cRes,*eRes,*fRes;


  // get run_log_begin info
  nbegin=0;
  sql1 << "select * from run_log_begin where session_name=\'" << session << "\' and run=" << run << ends;
  mysql_query(dbhandle, sql1.str());
  bRes = mysql_store_result(dbhandle);
  if(bRes!=NULL) {
    nbegin=mysql_num_rows(bRes);
    bRow = mysql_fetch_row(bRes);
  } else {
    cerr << "Unable to find run_log_begin entry for session: " << session << "  run: " << run << endl;
  }


  // get run_log_comment info
  ncomment=0;
  sql2 << "select * from run_log_comment where session_name=\'" << session << "\' and run=" << run << ends;
  mysql_query(dbhandle, sql2.str());
  cRes = mysql_store_result(dbhandle);
  if(cRes!=NULL) {
    ncomment=mysql_num_rows(cRes);
    cRow = mysql_fetch_row(cRes);
  } else {
    cerr << "Unable to find run_log_begin entry for session: " << session << "  run: " << run << endl;
  }


  // get run_log_end info
  nend=0;
  sql3 << "select * from run_log_end where session_name=\'" << session << "\' and run=" << run << ends;
  mysql_query(dbhandle, sql3.str());
  eRes = mysql_store_result(dbhandle);
  if(eRes!=NULL) {
    nend=mysql_num_rows(eRes);
    eRow = mysql_fetch_row(eRes);
  } else {
    cerr << "Unable to find run_log_end entry for session: " << session << "  run: " << run << endl;
  }


  // got data, print summary
  file.setf(ios::left);
  file << "\n\n\nRun file summary for session:  " << session << "      run:  " << setw(6) << run << endl;
  file << "-------------------------------------------------------" << endl << endl;

  if(nbegin>0) {
    file << setw(25) << "Begin:       " << get_col_by_name(bRes,bRow,"start_date") << endl;
    file << setw(25) << "End:         " << ((nend>0)?get_col_by_name(eRes,eRow,"end_date"):" ") << endl;
    file << setw(25) << "Config:      " << get_col_by_name(bRes,bRow,"configuration") << endl;
    file << setw(25) << "Trig config: " << get_col_by_name(bRes,bRow,"trigger_config") << endl;
    file << setw(25) << "Chan config: " << get_col_by_name(bRes,bRow,"channel_config") << endl << endl;

    // misc files		 
    file << setw(25) << "L1:     " << get_col_by_name(bRes,bRow,"l1_program") << endl;
    file << setw(25) << "TS:     " << get_col_by_name(bRes,bRow,"ts_file") << endl;
    
    // sparsification files
    file << setw(25) << "SC:    " << get_col_by_name(bRes,bRow,"sc_spar") << endl;
    file << setw(25) << "CC:    " << get_col_by_name(bRes,bRow,"cc_spar") << endl;
    file << setw(25) << "EC1:   " << get_col_by_name(bRes,bRow,"ec1_spar") << endl;
    file << setw(25) << "EC2:   " << get_col_by_name(bRes,bRow,"ec2_spar") << endl;
    file << setw(25) << "LAC:   " << get_col_by_name(bRes,bRow,"lac_spar") << endl << endl;
  }

  if(ncomment>0){
    file << setw(25) << "run type:             " << get_col_by_name(cRes,cRow,"run_type") << endl;
    file << setw(25) << "target:               " << get_col_by_name(cRes,cRow,"target") << endl;
    file << setw(25) << "beam current request: " << get_col_by_name(cRes,cRow,"beam_current_request") << endl;
    file << setw(25) << "operators:            " << get_col_by_name(cRes,cRow,"operators") << endl;
    file << setw(25) << "logbook book:         " << get_col_by_name(cRes,cRow,"logbook_book") << endl;
    file << setw(25) << "logbook page:         " << get_col_by_name(cRes,cRow,"logbook_page") << endl << endl;
    file << setw(25) << "comment:              " << get_col_by_name(cRes,cRow,"comment") << endl << endl;
  }      


  // prescales
  file << endl << endl;
  file << "Prescale registers:" << endl;
  file << "------------------" << endl << endl;
  file << "1       2       3       4       5       6       7       8" << endl;
  file << "-       -       -       -       -       -       -       -" << endl;
  if(nbegin>0) {
    file << setw(8) << get_col_by_name(bRes,bRow,"prescale_1");
    file << setw(8) << get_col_by_name(bRes,bRow,"prescale_2");
    file << setw(8) << get_col_by_name(bRes,bRow,"prescale_3");
    file << setw(8) << get_col_by_name(bRes,bRow,"prescale_4");
    file << setw(8) << get_col_by_name(bRes,bRow,"prescale_5");
    file << setw(8) << get_col_by_name(bRes,bRow,"prescale_6");
    file << setw(8) << get_col_by_name(bRes,bRow,"prescale_7");
    file << setw(8) << get_col_by_name(bRes,bRow,"prescale_8");
    file << endl;
  }


  // discriminators
  file << endl << endl;
  file << "   ec inner        ec outer        ec total           cc               sc" << endl;
  file << "  low    high     low    high     low    high     low    high   thresh   width" << endl;
  file << "------- ------- ------- ------- ------- ------- ------- ------- ------- -------" << endl;
  if(nbegin>0) {
    file << "  " 
	 << setw(8) << get_col_by_name(bRes,bRow,"ec_inner_lo") 
	 << setw(8) << get_col_by_name(bRes,bRow,"ec_inner_hi") 
	 << setw(8) << get_col_by_name(bRes,bRow,"ec_outer_lo")
	 << setw(8) << get_col_by_name(bRes,bRow,"ec_outer_hi")
	 << setw(8) << get_col_by_name(bRes,bRow,"ec_total_lo")
	 << setw(8) << get_col_by_name(bRes,bRow,"ec_total_hi")
	 << setw(8) << get_col_by_name(bRes,bRow,"cc_hi")
	 << setw(8) << get_col_by_name(bRes,bRow,"cc_lo")
	 << setw(8) << get_col_by_name(bRes,bRow,"sc_thresh")
	 << setw(8) << get_col_by_name(bRes,bRow,"sc_width");
      file << endl;
  }
  

  // end run
  file << endl << endl;
  file << "End run information" << endl;
  file << "-------------------" << endl << endl;
  if(nend>0){
    file << setw(25) << "end date           " << get_col_by_name(eRes,eRow,"end_date") << endl;
    file << setw(25) << "file base          " << get_col_by_name(eRes,eRow,"filebase") << endl;
    file << setw(25) << "nfile              " << get_col_by_name(eRes,eRow,"nfile") << endl;
    file << setw(25) << "nevent	       	    " << get_col_by_name(eRes,eRow,"nevent") << endl;
    file << setw(25) << "nerror             " << get_col_by_name(eRes,eRow,"nerror") << endl;
    file << setw(25) << "nlong 	       	    " << get_col_by_name(eRes,eRow,"nlong") << endl << endl;
  }


  // data files
  file << endl << endl;
  nfiles=0;
  file << "File                            Nlong     Nevent    Nerror    Location" << endl;
  file << "------------------------------  --------  --------  --------  "
       << "------------------------\n" << endl;

  sql4 << "select * from run_log_files where session_name=\'" << session << "\' and run=" << run << ends;
  mysql_query(dbhandle, sql4.str());
  fRes = mysql_store_result(dbhandle);
  if(fRes!=NULL) {
    while(fRow = mysql_fetch_row(fRes)) {
      nfiles++;
      file << setw(32) << get_col_by_name(fRes,fRow,"filename") << setw(10) << get_col_by_name(fRes,fRow,"nlong") 
	   << setw(10) << get_col_by_name(fRes,fRow,"nevent") << setw(10) << get_col_by_name(fRes,fRow,"nerror")
	   << setw(27) << get_col_by_name(fRes,fRow,"location") << endl;
    }
  } else {
    cerr << "Unable to find run_log_files entries for session: " << session << "  run: " << run << endl;
  }



  // epics data
  if(nbegin>0){
    file << endl << endl;
    file << "Epics data" << endl;
    file << "----------" << endl << endl;
    file << setw(25) << "beam_energy:"        << get_col_by_name(bRes,bRow,"beam_energy") << endl;
    file << setw(25) << "a_slit_position:"    << get_col_by_name(bRes,bRow,"a_slit_position") << endl;
    file << setw(25) << "b_slit_position:"    << get_col_by_name(bRes,bRow,"b_slit_position") << endl;
    file << setw(25) << "c_slit_position:"    << get_col_by_name(bRes,bRow,"c_slit_position") << endl;
    file << setw(25) << "thermionic_gun:"     << get_col_by_name(bRes,bRow,"thermionic_gun") << endl;
    file << setw(25) << "polarized_gun:"      << get_col_by_name(bRes,bRow,"polarized_gun") << endl << endl;
    file << setw(25) << "torus_current:"      << get_col_by_name(bRes,bRow,"torus_current") << endl;
    file << setw(25) << "mini_current:"       << get_col_by_name(bRes,bRow,"mini_current") << endl;
    file << setw(25) << "tagger_current:"     << get_col_by_name(bRes,bRow,"tagger_current") << endl << endl;
    file << setw(25) << "cryo_pressure:"      << get_col_by_name(bRes,bRow,"cryo_pressure") << endl;
    file << setw(25) << "cryo_temperature:"   << get_col_by_name(bRes,bRow,"cryo_temperature") << endl;
    file << setw(25) << "cryo_status:"        << get_col_by_name(bRes,bRow,"cryo_status") << endl << endl;
    file << setw(25) << "upstream_beam_vac:"  << get_col_by_name(bRes,bRow,"upstream_beam_vac") << endl;
    file << setw(25) << "target_vac:"         << get_col_by_name(bRes,bRow,"target_vac") << endl << endl;
  }
  if(nend>0){
    cout << endl << endl;
    file << "Scaler data" << endl;
    file << "-----------" << endl << endl;
    file << setw(25) << "faraday cup        " << get_col_by_name(eRes,eRow,"fcup") << endl;
    file << setw(25) << "faraday cup active " << get_col_by_name(eRes,eRow,"fcup_active") << endl;
    file << setw(25) << "faraday cup live   " << get_col_by_name(eRes,eRow,"fcup_live") << endl;
    file << setw(25) << "clock              " << get_col_by_name(eRes,eRow,"clock") << endl;
    file << setw(25) << "clock active       " << get_col_by_name(eRes,eRow,"clock_active") << endl;
    file << setw(25) << "clock live         " << get_col_by_name(eRes,eRow,"clock_live") << endl;
  }


  // done
  file << endl << endl
       << "==========================================="
       << "===========================================" << endl << endl;
  mysql_free_result(bRes);
  mysql_free_result(cRes);
  mysql_free_result(eRes);
  mysql_free_result(fRes);
  return;

}


//-----------------------------------------------------------------------------


char *get_col_by_name(MYSQL_RES *res, MYSQL_ROW row, char *name) {
  
  MYSQL_FIELD *field;

  int num_fields = mysql_num_fields(res);
  for(int i=0; i<num_fields; i++) {
    field = mysql_fetch_field_direct(res,i);
    if(strcasecmp(field->name,name)==0) {
      return(row[i]);
    }
  }

  // not found
  return(NULL);
}


//-----------------------------------------------------------------------------


void decode_command_line(int argc, char **argv){

   int i=1;
   char *help = "\nUsage:\n\n   run_log_summary [-f filename] [-debug] [-m msql_database]\n"
     "            [-host dbhost] [-user dbuser] [-d database] [-s session] first_run [last_run]\n";


  // decode command line...loop over all arguments, except the 1st (which is program name)
  while(i<argc) {
    if (strncasecmp(argv[i],"-",1)!=0){
      run=atoi(argv[i]);
      if((i+1)<argc)runend=atoi(argv[i+1]); else runend=run;
      return;
    } 
    else if (strncasecmp(argv[i],"-debug",6)==0){
      debug=1;
      i=i+1;
    }
    else if (strncasecmp(argv[i],"-host",5)==0) {
      dbhost=strdup(argv[i+1]);
      i=i+2;
    }
    else if (strncasecmp(argv[i],"-user",5)==0) {
      dbuser=strdup(argv[i+1]);
      i=i+2;
    }
    else if (strncasecmp(argv[i],"-s",2)==0) {
      session=strdup(argv[i+1]);
      i=i+2;
    }
    else if (strncasecmp(argv[i],"-d",2)==0) {
      database=strdup(argv[i+1]);
      i=i+2;
    }
    else if (strncasecmp(argv[i],"-f",2)==0) {
      filename=strdup(argv[i+1]);
      i=i+2;
    }
    else if (strncasecmp(argv[i],"-m",2)==0) {
      msql_database=strdup(argv[i+1]);
      i=i+2;
    }
    else if(strncasecmp(argv[i],"-h",2)==0){
      cout << help << endl;
      exit(EXIT_SUCCESS);
    }
    else{
      cerr << "Unknown arg: " << argv[i] << endl;
      i=i+1;
    }
  }
  
  return;
}  


//-----------------------------------------------------------------------------
