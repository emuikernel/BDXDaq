h27401
s 00005/00000/00366
d D 1.5 09/06/05 11:54:18 boiarino 5 4
c *** empty log message ***
e
s 00014/00009/00352
d D 1.4 09/06/05 11:48:03 boiarino 4 3
c *** empty log message ***
e
s 00109/00033/00252
d D 1.3 09/06/05 11:12:24 boiarino 3 2
c *** empty log message ***
e
s 00037/00009/00248
d D 1.2 09/05/20 09:59:41 boiarino 2 1
c add run_log_files check
c 
e
s 00257/00000/00000
d D 1.1 07/10/12 10:15:21 boiarino 1 0
c 
e
u
U
f b 
f e 0
t
T
I 1
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
D 3
static MYSQL *dbhandle             = NULL;
E 3
I 3
static MYSQL *dbhandle   = NULL;
E 3
static MYSQL_ROW row;
static MYSQL_RES *res;


static char *session     = "clasprod";
static int begin_run     = 0;
static int end_run       = 999999;
static int run;


D 3
// counts
E 3
I 3
// counts for database info
E 3
static short rlb[1000000];
static short rlc[1000000];
static short rle[1000000];
I 2
static short rlf[1000000];
E 2

I 3
// counts for log files
static short rlbf[1000000];
static short rlcf[1000000];
static short rlef[1000000];
static short rlff[1000000];
static short rlfff[1000000];
I 4
static short rlsf[1000000];
E 4
E 3

I 3

E 3
// misc variables
static int no_mult      = 0;
static int no_miss      = 0;
static int all_runs     = 0;
static int last_run     = 0;


// prototypes
void decode_command_line(int argc, char **argv);
I 3
extern "C" {
int get_run_number(const char *, const char *);
}
E 3

D 3

E 3
//-----------------------------------------------------------------------------

D 3

main(int argc, char **argv) {

E 3
I 3
int
main(int argc, char **argv)
{
D 4
  FILE *fd0, *fd1, *fd2, *fd3, *fd4;
  char str0[1000], str1[1000], str2[1000], str3[1000], str4[1000];
E 4
I 4
  FILE *fd0, *fd1, *fd2, *fd3, *fd4, *fd5;
  char str0[1000], str1[1000], str2[1000], str3[1000], str4[1000], str5[1000];
E 4
E 3
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
D 2
  for(i=0; i<1000000; i++) {
E 2
I 2
  for(i=0; i<1000000; i++)
  {
E 2
    rlb[i]=0;
    rlc[i]=0;
    rle[i]=0;
I 2
    rlf[i]=0;
I 3
    rlbf[i]=0;
    rlcf[i]=0;
    rlef[i]=0;
    rlff[i]=0;
    rlfff[i]=0;
I 4
    rlsf[i]=0;
E 4
E 3
E 2
  }


I 3

E 3
  // loop over rlb
  strstream sql1;
  sql1 << "select run from run_log_begin where session_name=\'" << session << "\' and run>=" <<
    begin_run << " and run<=" << end_run << ends;
  mysql_query(dbhandle, sql1.str());
  res = mysql_use_result(dbhandle);
D 3
  if(res!=NULL) {
    while(row = mysql_fetch_row(res)){
E 3
I 3
  if(res!=NULL)
  {
    while(row = mysql_fetch_row(res))
    {
E 3
      run=atoi(row[0]);
      rlb[run]++;
      last_run=MAX(last_run,run);
    }
D 3
  } else {
E 3
I 3
  }
  else
  {
E 3
    cerr << "?unable to read run_log_begin table" << endl;
  }
  mysql_free_result(res);


  // loop over rlc
  strstream sql2;
  sql2 << "select run from run_log_comment where session_name=\'" << session << 
    "\' and run>=" << begin_run << " and run<=" << end_run << ends;
  mysql_query(dbhandle, sql2.str());
  res = mysql_use_result(dbhandle);
D 3
  if(res!=NULL) {
    while(row = mysql_fetch_row(res)){
E 3
I 3
  if(res!=NULL)
  {
    while(row = mysql_fetch_row(res))
    {
E 3
      run=atoi(row[0]);
      rlc[run]++;
      last_run=MAX(last_run,run);
    }
D 3
  } else {
E 3
I 3
  }
  else
  {
E 3
    cerr << "?unable to read run_log_comment table" << endl;
  }
  mysql_free_result(res);


  // loop over rle
  strstream sql3;
  sql3 << "select run from run_log_end where session_name=\'" << session << "\' and run>=" <<
    begin_run << " and run<=" << end_run << ends;
  mysql_query(dbhandle, sql3.str());
  res = mysql_use_result(dbhandle);
D 3
  if(res!=NULL) {
    while(row = mysql_fetch_row(res)){
E 3
I 3
  if(res!=NULL)
  {
    while(row = mysql_fetch_row(res))
    {
E 3
      run=atoi(row[0]);
      rle[run]++;
      last_run=MAX(last_run,run);
    }
D 3
  } else {
E 3
I 3
  }
  else
  {
E 3
    cerr << "?unable to read run_log_end table" << endl;
  }
  mysql_free_result(res);


I 2
  // loop over rlf
  strstream sql4;
  sql4 << "select run from run_log_files where session_name=\'" << session << "\' and run>=" <<
    begin_run << " and run<=" << end_run << ends;
  mysql_query(dbhandle, sql4.str());
  res = mysql_use_result(dbhandle);
D 3
  if(res!=NULL) {
    while(row = mysql_fetch_row(res)){
E 3
I 3
  if(res!=NULL)
  {
    while(row = mysql_fetch_row(res))
    {
E 3
      run=atoi(row[0]);
      rlf[run]++;
      last_run=MAX(last_run,run);
    }
D 3
  } else {
E 3
I 3
  }
  else
  {
E 3
    cerr << "?unable to read run_log_files table" << endl;
  }
  mysql_free_result(res);


I 3

  // loop over log files
  for(run=begin_run; run<=end_run; run++)
  {
    sprintf(str0,"%s/run_log/archive/begin_%s_%06d.txt",getenv("CLON_PARMS"),session,run);
    sprintf(str1,"%s/run_log/archive/comment_%s_%06d.txt",getenv("CLON_PARMS"),session,run);
    sprintf(str2,"%s/run_log/archive/end_%s_%06d.txt",getenv("CLON_PARMS"),session,run);
    sprintf(str3,"%s/run_files/archive/runfile_%s_%06d.txt",getenv("CLON_PARMS"),session,run);
    sprintf(str4,"%s/run_files/runfile_%s_%06d.txt",getenv("CLON_PARMS"),session,run);
I 4
    sprintf(str5,"%s/scalers/archive/scalers_%s_%06d.txt",getenv("CLON_PARMS"),session,run);
E 4
    fd0 = fopen(str0,"r");
    fd1 = fopen(str1,"r");
    fd2 = fopen(str2,"r");
    fd3 = fopen(str3,"r");
    fd4 = fopen(str4,"r");
I 4
    fd5 = fopen(str5,"r");
E 4
    if(fd0!=NULL) {rlbf[run]++;fclose(fd0);}
    if(fd1!=NULL) {rlcf[run]++;fclose(fd1);}
    if(fd2!=NULL) {rlef[run]++;fclose(fd2);}
    if(fd3!=NULL) {rlff[run]++;fclose(fd3);}
    if(fd4!=NULL) {rlfff[run]++;fclose(fd4);}
I 4
    if(fd5!=NULL) {rlsf[run]++;fclose(fd5);}
E 4
  }




I 5
  cout << "NOTES:" << endl;
  cout << "  '+' or '-' stands for the existence of the corresponding *.txt file" << endl;
  cout << "  '(+)' or '(-)' corresponds to run_file which was not moved to archive area yet" << endl;
  cout << "  '[+]' or '[-]' corresponds to the existence of the 'scalers' *.txt files" << endl;

E 5
E 3
E 2
  // compare
D 2
  for(i=begin_run; i<=last_run; i++) {
    if( (rlb[i]>0)||(rlc[i]>0)||(rle[i]>0) ) {
E 2
I 2
  for(i=begin_run; i<=last_run; i++)
  {
    if( (rlb[i]>0)||(rlc[i]>0)||(rle[i]>0)||(rlf[i]>0) )
    {
E 2

      // multiple entries
D 2
      if(no_mult==0) {
E 2
I 2
      if(no_mult==0)
      {
E 2
	if(rlb[i]>1)cout << "run " << setw(6) << i << " has " << rlb[i] << " begin   entries" << endl;
	if(rlc[i]>1)cout << "run " << setw(6) << i << " has " << rlc[i] << " comment entries" << endl;
	if(rle[i]>1)cout << "run " << setw(6) << i << " has " << rle[i] << " end     entries" << endl;
I 2
	/*if(rlf[i]>1)cout << "run " << setw(6) << i << " has " << rlf[i] << " files   entries" << endl;*/
E 2
      }

      // missing entries
D 2
      if(no_miss==0) {
	if( (rlb[i]<=0)||(rlc[i]<=0)||(rle[i]<=0) ) {
E 2
I 2
      if(no_miss==0)
      {
	if( (rlb[i]<=0)||(rlc[i]<=0)||(rle[i]<=0)||(rlf[i]<=0) )
    {
E 2
	  cout << "run " << setw(6) << i << " has:   ";
D 3
	  if(rlb[i]<=0)cout << "x  "; else cout << "B  ";
	  if(rlc[i]<=0)cout << "x  "; else cout << "C  ";
D 2
	  if(rle[i]<=0)cout << "x"; else cout << "E";
E 2
I 2
	  if(rle[i]<=0)cout << "x  "; else cout << "E  ";
	  if(rlf[i]<=0)cout << "x"; else cout << "F";
E 2
	  cout << "      (missing)" << endl;
	} else if (all_runs==1) {
D 2
	  cout << "run " << setw(6) << i << " has:   B  C  E      (ok)" << endl;
E 2
I 2
	  cout << "run " << setw(6) << i << " has:   B  C  E  F      (ok)" << endl;
E 3
I 3
	  if(rlb[i]<=0) cout << "x"; else cout << "B";
        if(rlbf[i]<=0) cout << "-  "; else cout << "+  ";
	  if(rlc[i]<=0) cout << "x"; else cout << "C";
        if(rlcf[i]<=0) cout << "-  "; else cout << "+  ";
	  if(rle[i]<=0) cout << "x"; else cout << "E";
        if(rlef[i]<=0) cout << "-  "; else cout << "+  ";
	  if(rlf[i]<=0) cout << "x"; else cout << "F";
        if(rlff[i]<=0) cout << "-"; else cout << "+";
        if(rlfff[i]<=0) cout << "(-)  "; else cout << "(+)  ";
I 4
        if(rlsf[i]<=0) cout << "[-]  "; else cout << "[+]  ";
E 4
	  cout << "  (missing)" << endl;
E 3
E 2
	}
I 3
    else if(all_runs==1)
    {
D 4
	  cout << "run " << setw(6) << i << " has:   B   C   E   F        (ok)" << endl;
E 4
I 4
	  cout << "run " << setw(6) << i << " has:   B   C   E   F             (ok)" << endl;
E 4
	}
E 3
      }
D 3

      
    } else if ((all_runs==1)&&(no_miss==0)) {
D 2
      cout << "run " << setw(6) << i << " has:   x  x  x      (no entry)" << endl;
E 2
I 2
      cout << "run " << setw(6) << i << " has:   x  x  x  x      (no entry)" << endl;
E 3
I 3
   
E 3
E 2
    }
I 3
    else if((all_runs==1)&&(no_miss==0))
    {
D 4
      cout << "run " << setw(6) << i << " has:   x   x   x   x        (no entry)" << endl;
E 4
I 4
      cout << "run " << setw(6) << i << " has:   x   x   x   x             (no entry)" << endl;
E 4
    }
E 3

  }


  // close database
  mysql_close(dbhandle);
  exit(EXIT_SUCCESS);
}


//-----------------------------------------------------------------------------


D 3
void decode_command_line(int argc, char **argv) {

E 3
I 3
D 4
void decode_command_line(int argc, char **argv)
E 4
I 4
void
decode_command_line(int argc, char **argv)
E 4
{
E 3
   int i=1;
   char *help = "\nUsage:\n\n   run_checker [-s session] [-host dbhost] [-user dbuser] [-d database]\n"
     "               [-no_mult] [-no_miss] [-all_runs] begin_run end_run\n";

D 4

E 4
  // decode command line...loop over all arguments, except the 1st (which is program name)
D 4
  while(i<argc) {
E 4
I 4
  while(i<argc)
  {
E 4
D 3
    if (strncasecmp(argv[i],"-",1)!=0) {
E 3
I 3
    if (strncasecmp(argv[i],"-",1)!=0)
    {
E 3
      begin_run=atoi(argv[i]);
D 3
      if((i+1)<argc)end_run=atoi(argv[i+1]),last_run=end_run;
E 3
I 3
      if((i+1)<argc)
      {
        end_run=atoi(argv[i+1]);
D 4
printf("1\n");
E 4
	  }
      else
	  {
        end_run=get_run_number(dbuser,session);
D 4
printf("2\n");
E 4
	  }
      last_run=end_run;
E 3
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
E 1
