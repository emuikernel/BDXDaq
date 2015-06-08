//
//  event_db_dump
//
//  dumps info from database 
//
//  For help, type:  event_db_dump -h
//
//  Note:  not particulary bombproof yet...
//
//   ejw, 13-feb-1998


// CC -g -features=no%conststrings -o event_db_dump event_db_dump.cc  -I/usr/include/mysql -L/usr/lib/mysql -lmysqlclient 


#define _POSIX_SOURCE 1
#define __EXTENSIONS__


// system includes
#include <iostream.h>
#include <iomanip.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>

using namespace std;
#include <strstream>


// ingres include files
#include <mysql/mysql.h>


// local variables used by ingres
static int runbeg        = 0;
static int runend        = 0;
static char *dbhost      = "clondb1";
static char *dbuser      = "clasrun";
static char *database    = "clasprod";
static MYSQL *dbhandle   = NULL;
static MYSQL_ROW row;
static MYSQL_RES *res;


// misc variables
static int debug       	      = 0;   


// prototypes
void decode_command_line(int argc, char **argv);
char *get_col_by_name(MYSQL_RES *res, MYSQL_ROW row, char *name);



//------------------------------------------------------------------------------------


main(int argc, char **argv){


  int run;
  float beam_energy;
  float beam_current_request;
  float torus_current;
  float mini_current;
  int ec_total_hi;
  int ec_total_lo;
  int cc_hi;
  int cc_lo;
  char target[30];
  char l1_program[200];
  char errmsg[400];
  short nul[9];
  
  int nfound;
  char *p;


  // synch with c i/o
  ios::sync_with_stdio();


  // decode command line args
  decode_command_line(argc,argv);
  if(runbeg<=0) {
    cerr << "\n   No run specified" << endl << endl;
    exit(EXIT_FAILURE);
  }
  

  // set readlock
  //  exec sql set lockmode session where readlock=nolock; ???


  // connect to database
  dbhandle=mysql_init(NULL);
  if(!mysql_real_connect(dbhandle,dbhost,dbuser,NULL,database,0,NULL,0)) {
    time_t now = time(NULL);
    cerr << "Unable to connect to database (error: " << mysql_errno(dbhandle) 
	 << ", " << mysql_error(dbhandle) << ") on " 
	 << ctime(&now) << endl;
    exit(EXIT_FAILURE);
  }


  // index
  cout << endl << endl
       << "                Event Database Dump" << endl
       << "                -------------------" << endl << endl
       << "   Run " << " Energy " << "Current " << "    Target "
       << "   Torus " << "   Mini " 
       << "EC_hi " << "EC_lo " << "CC_hi " << "CC_lo " << "Trigger" << endl
       << "   --- " << " ------ " << "------- " << "    ------ "
       << "  ------ " << "   ---- " 
       << "----- " << "----- " << "----- " << "----- " << "-------"
       << endl;
  

  // get run data
  strstream sql;
  sql << "select b.run,beam_energy,torus_current,mini_current,beam_current_request,target,"
      << "ec_total_hi,ec_total_lo,cc_hi,cc_lo,l1_program"
      << " from run_log_begin b, run_log_comment c"
      << " where (b.run=c.run and b.run>=" << runbeg << " and b.run<=" << runend << ")"
      << " order by b.run" << ends;
  
  mysql_query(dbhandle, sql.str());
  res = mysql_use_result(dbhandle);
  if(res!=NULL) {
    while(row = mysql_fetch_row(res)) {

      // skip leading dir name
      char *l1p = get_col_by_name(res,row,"l1_program");
      char *p=strstr(l1p,"/home/trigger/");
      if(p==NULL)p=l1p; else p+=14;
      
      cout << setw(6) 	<< get_col_by_name(res,row,"run") << " "
	   << setw(7) 	<< get_col_by_name(res,row,"beam_energy") << " " 
	   << setw(7) 	<< get_col_by_name(res,row,"beam_current_request") << " " 
	   << setw(10)  << get_col_by_name(res,row,"target") << " " 
	   << setw(7) 	<< setprecision(4) << get_col_by_name(res,row,"torus_current") << " " 
	   << setw(7) 	<< get_col_by_name(res,row,"mini_current") << " " 
	   << setw(5) 	<< get_col_by_name(res,row,"ec_total_hi") << " " 
	   << setw(5) 	<< get_col_by_name(res,row,"ec_total_lo") << " " 
	   << setw(5) 	<< get_col_by_name(res,row,"cc_hi") << " " 
	   << setw(5) 	<< get_col_by_name(res,row,"cc_lo") << " " 
	   << p << " " 
	   << endl;
    }
  } else {
    cerr << "?unable to read run_log tables" << endl;
  }
  mysql_free_result(res);


  // done...close database
  mysql_close(dbhandle);
  exit(EXIT_SUCCESS);
  
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
   char *help = "\nUsage:\n\n   event_db_dump [-host dbhost] [-user dbuser] [-d database] [-debug]"
                " first_run [last_run]\n";


  // decode command line...loop over all arguments, except the 1st (which is program name)
  while(i<argc) {
    if (strncasecmp(argv[i],"-",1)!=0){
      runbeg=atoi(argv[i]);
      if((i+1)<argc)runend=atoi(argv[i+1]); else runend=runbeg;
      return;
    } 
    else if (strncasecmp(argv[i],"-debug",6)==0) {
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
    else if (strncasecmp(argv[i],"-d",2)==0) {
      database=strdup(argv[i+1]);
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
