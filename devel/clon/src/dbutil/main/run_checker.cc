//  run_checker.scc

//  checks for inconsistencies in run database

//  ejw, 12-may-98
//  ejw, 6-jun-06, upgraded to use mysql instead of ingres

// CC -g -features=no%conststrings -o run_checker run_checker.cc -I/usr/include/mysql -L/usr/lib/mysql -lmysqlclient 



#define _POSIX_SOURCE 1
#define __EXTENSIONS__

using namespace std;


// system includes

using namespace std;
#include <strstream>

#include <iomanip.h>
#include <fstream.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>


// mysql includes
#include <mysql/mysql.h>

#define MAX(a,b)  ( (a) > (b) ? (a) : (b) )

// mysql variables
static char *dbhost      = "clondb1";
static char *dbuser      = "clasrun";
static char *database    = "clasprod";
static MYSQL *dbhandle   = NULL;
static MYSQL_ROW row;
static MYSQL_RES *res;


static char *session     = "clasprod";
static int begin_run     = 0;
static int end_run       = 999999;
static int run;


// counts for database info
static short rlb[1000000];
static short rlc[1000000];
static short rle[1000000];
static short rlf[1000000];

// counts for log files
static short rlbf[1000000];
static short rlcf[1000000];
static short rlef[1000000];
static short rlff[1000000];
static short rlfff[1000000];
static short rlsf[1000000];


// misc variables
static int no_mult      = 0;
static int no_miss      = 0;
static int all_runs     = 0;
static int last_run     = 0;


// prototypes
void decode_command_line(int argc, char **argv);
extern "C" {
int get_run_number(const char *, const char *);
}

//-----------------------------------------------------------------------------

int
main(int argc, char **argv)
{
  FILE *fd0, *fd1, *fd2, *fd3, *fd4, *fd5;
  char str0[1000], str1[1000], str2[1000], str3[1000], str4[1000], str5[1000];
  int i;


  // synch with c i/o
  ios::sync_with_stdio();


  // decode command line args
  decode_command_line(argc,argv);
  	
	
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


  // clear counters
  for(i=0; i<1000000; i++)
  {
    rlb[i]=0;
    rlc[i]=0;
    rle[i]=0;
    rlf[i]=0;
    rlbf[i]=0;
    rlcf[i]=0;
    rlef[i]=0;
    rlff[i]=0;
    rlfff[i]=0;
    rlsf[i]=0;
  }



  // loop over rlb
  strstream sql1;
  sql1 << "select run from run_log_begin where session_name=\'" << session << "\' and run>=" <<
    begin_run << " and run<=" << end_run << ends;
  mysql_query(dbhandle, sql1.str());
  res = mysql_use_result(dbhandle);
  if(res!=NULL)
  {
    while(row = mysql_fetch_row(res))
    {
      run=atoi(row[0]);
      rlb[run]++;
      last_run=MAX(last_run,run);
    }
  }
  else
  {
    cerr << "?unable to read run_log_begin table" << endl;
  }
  mysql_free_result(res);


  // loop over rlc
  strstream sql2;
  sql2 << "select run from run_log_comment where session_name=\'" << session << 
    "\' and run>=" << begin_run << " and run<=" << end_run << ends;
  mysql_query(dbhandle, sql2.str());
  res = mysql_use_result(dbhandle);
  if(res!=NULL)
  {
    while(row = mysql_fetch_row(res))
    {
      run=atoi(row[0]);
      rlc[run]++;
      last_run=MAX(last_run,run);
    }
  }
  else
  {
    cerr << "?unable to read run_log_comment table" << endl;
  }
  mysql_free_result(res);


  // loop over rle
  strstream sql3;
  sql3 << "select run from run_log_end where session_name=\'" << session << "\' and run>=" <<
    begin_run << " and run<=" << end_run << ends;
  mysql_query(dbhandle, sql3.str());
  res = mysql_use_result(dbhandle);
  if(res!=NULL)
  {
    while(row = mysql_fetch_row(res))
    {
      run=atoi(row[0]);
      rle[run]++;
      last_run=MAX(last_run,run);
    }
  }
  else
  {
    cerr << "?unable to read run_log_end table" << endl;
  }
  mysql_free_result(res);


  // loop over rlf
  strstream sql4;
  sql4 << "select run from run_log_files where session_name=\'" << session << "\' and run>=" <<
    begin_run << " and run<=" << end_run << ends;
  mysql_query(dbhandle, sql4.str());
  res = mysql_use_result(dbhandle);
  if(res!=NULL)
  {
    while(row = mysql_fetch_row(res))
    {
      run=atoi(row[0]);
      rlf[run]++;
      last_run=MAX(last_run,run);
    }
  }
  else
  {
    cerr << "?unable to read run_log_files table" << endl;
  }
  mysql_free_result(res);



  // loop over log files
  for(run=begin_run; run<=end_run; run++)
  {
    sprintf(str0,"%s/run_log/archive/begin_%s_%06d.txt",getenv("CLON_PARMS"),session,run);
    sprintf(str1,"%s/run_log/archive/comment_%s_%06d.txt",getenv("CLON_PARMS"),session,run);
    sprintf(str2,"%s/run_log/archive/end_%s_%06d.txt",getenv("CLON_PARMS"),session,run);
    sprintf(str3,"%s/run_files/archive/runfile_%s_%06d.txt",getenv("CLON_PARMS"),session,run);
    sprintf(str4,"%s/run_files/runfile_%s_%06d.txt",getenv("CLON_PARMS"),session,run);
    sprintf(str5,"%s/scalers/archive/scalers_%s_%06d.txt",getenv("CLON_PARMS"),session,run);
    fd0 = fopen(str0,"r");
    fd1 = fopen(str1,"r");
    fd2 = fopen(str2,"r");
    fd3 = fopen(str3,"r");
    fd4 = fopen(str4,"r");
    fd5 = fopen(str5,"r");
    if(fd0!=NULL) {rlbf[run]++;fclose(fd0);}
    if(fd1!=NULL) {rlcf[run]++;fclose(fd1);}
    if(fd2!=NULL) {rlef[run]++;fclose(fd2);}
    if(fd3!=NULL) {rlff[run]++;fclose(fd3);}
    if(fd4!=NULL) {rlfff[run]++;fclose(fd4);}
    if(fd5!=NULL) {rlsf[run]++;fclose(fd5);}
  }




  cout << "NOTES:" << endl;
  cout << "  '+' or '-' stands for the existence of the corresponding *.txt file" << endl;
  cout << "  '(+)' or '(-)' corresponds to run_file which was not moved to archive area yet" << endl;
  cout << "  '[+]' or '[-]' corresponds to the existence of the 'scalers' *.txt files" << endl;

  // compare
  for(i=begin_run; i<=last_run; i++)
  {
    if( (rlb[i]>0)||(rlc[i]>0)||(rle[i]>0)||(rlf[i]>0) )
    {

      // multiple entries
      if(no_mult==0)
      {
	if(rlb[i]>1)cout << "run " << setw(6) << i << " has " << rlb[i] << " begin   entries" << endl;
	if(rlc[i]>1)cout << "run " << setw(6) << i << " has " << rlc[i] << " comment entries" << endl;
	if(rle[i]>1)cout << "run " << setw(6) << i << " has " << rle[i] << " end     entries" << endl;
	/*if(rlf[i]>1)cout << "run " << setw(6) << i << " has " << rlf[i] << " files   entries" << endl;*/
      }

      // missing entries
      if(no_miss==0)
      {
	if( (rlb[i]<=0)||(rlc[i]<=0)||(rle[i]<=0)||(rlf[i]<=0) )
    {
	  cout << "run " << setw(6) << i << " has:   ";
	  if(rlb[i]<=0) cout << "x"; else cout << "B";
        if(rlbf[i]<=0) cout << "-  "; else cout << "+  ";
	  if(rlc[i]<=0) cout << "x"; else cout << "C";
        if(rlcf[i]<=0) cout << "-  "; else cout << "+  ";
	  if(rle[i]<=0) cout << "x"; else cout << "E";
        if(rlef[i]<=0) cout << "-  "; else cout << "+  ";
	  if(rlf[i]<=0) cout << "x"; else cout << "F";
        if(rlff[i]<=0) cout << "-"; else cout << "+";
        if(rlfff[i]<=0) cout << "(-)  "; else cout << "(+)  ";
        if(rlsf[i]<=0) cout << "[-]  "; else cout << "[+]  ";
	  cout << "  (missing)" << endl;
	}
    else if(all_runs==1)
    {
	  cout << "run " << setw(6) << i << " has:   B   C   E   F             (ok)" << endl;
	}
      }
   
    }
    else if((all_runs==1)&&(no_miss==0))
    {
      cout << "run " << setw(6) << i << " has:   x   x   x   x             (no entry)" << endl;
    }

  }


  // close database
  mysql_close(dbhandle);
  exit(EXIT_SUCCESS);
}


//-----------------------------------------------------------------------------


void
decode_command_line(int argc, char **argv)
{
   int i=1;
   char *help = "\nUsage:\n\n   run_checker [-s session] [-host dbhost] [-user dbuser] [-d database]\n"
     "               [-no_mult] [-no_miss] [-all_runs] begin_run end_run\n";

  // decode command line...loop over all arguments, except the 1st (which is program name)
  while(i<argc)
  {
    if (strncasecmp(argv[i],"-",1)!=0)
    {
      begin_run=atoi(argv[i]);
      if((i+1)<argc)
      {
        end_run=atoi(argv[i+1]);
	  }
      else
	  {
        end_run=get_run_number(dbuser,session);
	  }
      last_run=end_run;
      return;
    } 
    else if (strncasecmp(argv[i],"-no_mult",8)==0) {
      no_mult=1;
      i=i+1;
    }
    else if (strncasecmp(argv[i],"-no_miss",8)==0) {
      no_miss =1;
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
    else if (strncasecmp(argv[i],"-all_runs",9)==0) {
      all_runs=1;
      i=i+1;
    }
    else if (strncasecmp(argv[i],"-s",2)==0) {
      session=strdup(argv[i+1]);
      i=i+2;
    }
    else if (strncasecmp(argv[i],"-d",2)==0) {
      database=strdup(argv[i+1]);
      i=i+2;
    }
    else if(strncasecmp(argv[i],"-h",2)==0) {
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
