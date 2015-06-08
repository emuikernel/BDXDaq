//
//  rlb_recover.cc
//
//  collects and inserts run log begin and comment info into database
//
//  ejw, 1-jul-98
//  ejw, 14-jun-2006 switched to mySQL from ingres


// CC -g -features=no%conststrings -o rlb_recover rlb_recover.cc   -I/usr/include/mysql -L/usr/lib/mysql -lmysqlclient -L/usr/local/clas/devel_new/SunOS_sun4u/lib -lutil -L/usr/local/coda/2.2.1/SunOS/lib -lmsql -lsocket -lnsl


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


// counts
static int nentry_begin    = 0;
static int nentry_comment  = 0;


// flags to inhibit event recording, etc.
static int debug     = 0;

static char *begin_run_file   = (char*)"run_log/archive/begin_%s_%06d.txt";
static char *comment_file     = (char*)"run_log/archive/comment_%s_%06d.txt";

static char line[1024];
static char bfilename[128];
static char cfilename[128];
static char temp[128];


// prototypes
void decode_command_line(int argc, char **argv);
void collect_begin_data(void);
void collect_comment_data(void);
void insert_into_db(char *sql);
void get_string(ifstream &file, const char* tag, char *var);
void get_int(ifstream &file, const char* tag, int array[], int nwd);
void get_long(ifstream &file, const char* tag, unsigned long array[], int nwd);
void get_hex(ifstream &file, const char* tag, unsigned long array[], int nwd);
void get_string2(ifstream &file, const char* tag, char *var);
void get_string3(ifstream &file, const char* tag, char *var);
void get_int2(ifstream &file, const char* tag, int *var);
void get_double2(ifstream &file, const char* tag, double *var);
void convert_date(char *out, char *in);
void double_quotes(char out[], char in[]);
int find_tag_line(ifstream &file, const char* tag, char buffer[], int buflen);
int get_next_line(ifstream &file, char buffer[], int buflen);
char *env_name(const char *env, char *name);
char *trim(char *in);


// program start time
static time_t start=time(NULL);


//--------------------------------------------------------------------------


main(int argc,char **argv){

  strstream sql1,sql2,sql3,sql4;
  MYSQL_ROW bRow,cRow,eRow,fRow;
  MYSQL_RES *bRes,*cRes,*eRes,*fRes;


  // synch with c i/o
  ios::sync_with_stdio();


  // decode command line
  decode_command_line(argc,argv);


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


    // check if begin data already in database
    nentry_begin=0;
    sql1 << "select run from run_log_begin where session_name=\'" << session << "\' and run=" << run;
    mysql_query(dbhandle,sql1.str());
    bRes = mysql_store_result(dbhandle);
    if(bRes!=NULL) nentry_begin++;
    mysql_free_result(bRes);


    // check if comment data already in database
    nentry_comment=0;
    sql1 << "select run from run_log_comment where session_name=\'" << session << "\' and run=" << run;
    mysql_query(dbhandle,sql1.str());
    cRes = mysql_store_result(dbhandle);
    if(cRes!=NULL) nentry_comment++;
    mysql_free_result(cRes);


    // get file names
    sprintf(temp,begin_run_file,session,run);
    sprintf(bfilename,"%s",env_name("CLON_PARMS",temp));
    sprintf(temp,comment_file,session,run);
    sprintf(cfilename,"%s",env_name("CLON_PARMS",temp));


    // make begin entries if needed
    if(nentry_begin<=0) {
      collect_begin_data();
    } else {
      cout << "Run " << run << "   begin entry exists" << endl;
    }


    // make comment entry if needed
    if(nentry_comment<=0) {
      collect_comment_data();
    } else {
      cout << "Run " << run << " comment entry exists" << endl;
    }


  }


  // disconnect from database
  mysql_close(dbhandle);


  // done
  exit(EXIT_SUCCESS);
}


//----------------------------------------------------------------


void collect_begin_data() {

  int i;
  char comma          = ',';
  char prime          = '\'';
  char config[30];
  char startdate[30];
  int prescale[8];
  char ts_file[100], l1_prog[120], trigger_config[80], channel_config[80];
  char sc_spar[100], cc_spar[100], ec1_spar[100], ec2_spar[100], lac_spar[100];
  int ecdiscr[6],ccdiscr[2],scdiscr[2];
  unsigned long ts_reg[4],ts_timer[5];

  int nepics=0;
  char db_name[100][30];
  float epics_val[100];

  int npolepics=0;
  char polar_db_name[100][30];
  float polar_epics_val[100];
  
  
  // open file
  ifstream file(bfilename);
  if(file.is_open()) {


    // read fields
    get_string(file,"*STARTDATE*",startdate);
    get_string(file,"*CONFIG*",config);
    get_string(file,"*TRIGGER_CONFIG",trigger_config);
    get_string(file,"*CHANNEL_CONFIG",channel_config);
    get_string(file,"*TS_FILE*",ts_file);
    get_string(file,"*L1_PROG*",l1_prog);
    get_string(file,"*SCSPAR*",sc_spar);
    get_string(file,"*CCSPAR*",cc_spar);
    get_string(file,"*EC1SPAR*",ec1_spar);
    get_string(file,"*EC2SPAR*",ec2_spar);
    get_string(file,"*LACSPAR*",lac_spar);

    get_int(file,"*ECDISCR*",ecdiscr,6);
    get_int(file,"*CCDISCR*",ccdiscr,2);
    get_int(file,"*SCDISCR*",scdiscr,2);

    get_hex(file,"*TS_REG*",ts_reg,4);
    get_long(file,"*TS_TIMERS*",ts_timer,5);
    get_int(file,"*TS_PRESCALE*",prescale,8);

    nepics=0;
    find_tag_line(file,"*EPICS*",line,sizeof(line));
    while((nepics<100)&&(get_next_line(file,line,sizeof(line))==0)) {
      sscanf(line,"%f %s",&epics_val[nepics],&db_name[nepics][0]);
      nepics++;
    }

    npolepics=0;
    find_tag_line(file,"*POLAR*",line,sizeof(line));
    while((npolepics<100)&&(get_next_line(file,line,sizeof(line))==0)) {
      sscanf(line,"%f %s",&polar_epics_val[npolepics],&polar_db_name[npolepics][0]);
      npolepics++;
    }



    // form rlb sql string -----------------------------------------
    strstream sql;
    sql << "insert into run_log_begin ("
	<< "run,start_date,session_name,configuration,trigger_config,channel_config,"
	<< "ts_file,l1_program,ts_csr,ts_control,ts_roc_enable,ts_synch,"
	<< "ts_timer_1,ts_timer_2,ts_timer_3,ts_timer_4,ts_timer_5,"
	<< "sc_spar,cc_spar,ec1_spar,ec2_spar,lac_spar,"
	<< "ec_inner_hi,ec_inner_lo,ec_outer_hi,ec_outer_lo,ec_total_hi,ec_total_lo,"
	<< "cc_hi,cc_lo,sc_thresh,sc_width";

    for (i=0; i<sizeof(prescale)/sizeof(int); i++){
      sql << comma << "prescale_" << i+1;
    }

    for (i=0; i<nepics; i++){
      sql << comma << trim(db_name[i]);
    }

    sql << ") values ("
	<< run
	<< comma	<< prime << trim(startdate) 	       << prime
	<< comma	<< prime << trim(session)   	       << prime
	<< comma	<< prime << trim(config)    	       << prime
	<< comma	<< prime << trim(trigger_config)       << prime
	<< comma	<< prime << trim(channel_config)       << prime   
	<< comma	<< prime << trim(ts_file)              << prime
	<< comma	<< prime << trim(l1_prog)              << prime
	<< comma << (long)ts_reg[0] << comma << (long)ts_reg[1]
	<< comma << (long)ts_reg[2] << comma << (long)ts_reg[3];

    for (i=0; i<5; i++) {
      sql << comma << (long)ts_timer[i];
    }

    sql << comma << prime	<< trim(sc_spar)     << prime 
	<< comma << prime	<< trim(cc_spar)     << prime 
	<< comma << prime	<< trim(ec1_spar)    << prime 
	<< comma << prime	<< trim(ec2_spar)    << prime 
	<< comma << prime	<< trim(lac_spar)    << prime 
	<< comma << ecdiscr[0] << comma << ecdiscr[1]
	<< comma << ecdiscr[2] << comma << ecdiscr[3]
	<< comma << ecdiscr[4] << comma << ecdiscr[5]
	<< comma << ccdiscr[0] << comma << ccdiscr[1]
	<< comma << scdiscr[0] << comma << scdiscr[1];
      
    for (i=0; i<sizeof(prescale)/sizeof(int); i++) {
      sql << comma << prescale[i];
    }

    for (i=0; i<nepics; i++){
      sql << comma << epics_val[i];
    }
    
    sql << ")" << ends;
    

    // form rlb sql string -----------------------------------------
    strstream sqlp;
    sqlp << "insert into run_log_polar ("
	<< "run,session_name";

    for (i=0; i<npolepics; i++){
      sqlp << comma << trim(polar_db_name[i]);
    }

    sqlp << ") values ("
	<< run
	<< comma << prime << trim(session) << prime;

    for (i=0; i<npolepics; i++){
      sqlp << comma << polar_epics_val[i];
    }
    
    sqlp << ")" << ends;
    


    // dump string if in debug mode
    if(debug==1){
      cout << endl << "rlb for run " << run << endl;
      cout << sql.str() << endl;
      cout << endl << endl << "rlp for run " << run << endl;
      cout << sqlp.str() << endl;
    }
    

    // insert into database
    cout << "making rlb entry for run " << run << endl;
    insert_into_db(sql.str());
    cout << "making rlp entry for run " << run << endl;
    insert_into_db(sqlp.str());
    

    // close file
    file.close();


  } else {
    cerr << "Begin unable to open " << bfilename << endl;
  }
  
  return;
}


//----------------------------------------------------------------


void collect_comment_data() {

  char entry_date[35];
  char run_type[25];
  char target[25];
  double beam_current_request;
  char mini_raster[20];
  char operators[101];
  int logbook_book;
  int logbook_page;
  char comment[2000];

  char trigger_type[121];
  char radiator[20];
  char scaler1_odd[20];
  char scaler1_even[20];
  char scaler2_odd[20];
  char scaler2_even[20];
  char ps_converter[20];
  char primary_collimator[20];
  char secondary_collimator[20];
  char pair_counter[20];
  char tac[20];
  char radphi[20];
  char et_coincidence[20];
  char tagger_prescaling[35];


  char comma    = ',';
  char prime    = '\'';
  int i;
  char temp[2000];
  char *p1, *p2;


  // open file
  ifstream file(cfilename);
  if(file.is_open()) {

    get_string2(file,"DATE",temp);
    convert_date(entry_date,temp);
    get_string2(file,"RUNTYPE",run_type);
    get_string2(file,"TARGET",target);
    get_double2(file,"BEAMREQUEST",&beam_current_request);
    get_string2(file,"MINIRASTER",mini_raster);
    get_string2(file,"OPERATORS",temp);
    double_quotes(operators,temp);
    get_int2(file, "LOG_BOOK", &logbook_book);
    get_int2(file, "LOG_PAGE", &logbook_page);
    get_string3(file,"*COMMENT*",temp);
    double_quotes(comment,temp);


    // form sql string
    strstream csql;
    csql << "insert into run_log_comment (run,session_name,entry_date,run_type,target,"
	 << "beam_current_request," 
	 << "mini_raster,operators,logbook_book,logbook_page,comment) values ("
         << run << comma
	 << prime << trim(session) << prime << comma
	 << prime << trim(entry_date) << prime << comma
	 << prime << trim(run_type) << prime << comma
	 << prime << trim(target) << prime << comma
	 << beam_current_request << comma
	 << prime << trim(mini_raster) << prime << comma
	 << prime << trim(operators) << prime << comma
         << logbook_book << comma
         << logbook_page << comma
	 << prime << trim(comment) << prime 
	 << ")" << ends;


    // dump string if in debug mode
    if(debug==1) {
      cout << endl << "Comment for run " << run << endl;
      cout << csql.str() << endl;
    }
    

    // insert into database
    cout << "making comment entry for run " << run << endl;
    insert_into_db(csql.str());
    

    // is this a photon run
    if(strncasecmp(run_type,"photon",6)==0) {

      get_string2(file,"Photon Trigger",trigger_type);
      get_string2(file,"Radiator",radiator);
      get_string2(file,"Scaler Gate",temp);
      p1=temp;
      *(p2=strchr(p1,' '))='\0'; strcpy(scaler1_odd,p1);  p1=p2+1;
      *(p2=strchr(p1,' '))='\0'; strcpy(scaler1_even,p1); p1=p2+1;
      *(p2=strchr(p1,' '))='\0'; strcpy(scaler2_odd,p1);  p1=p2+1;
      strcpy(scaler2_even,p1);

      get_string2(file,"PS_convertor",ps_converter);    // typo in comment file!
      get_string2(file,"Collimator1",primary_collimator);
      get_string2(file,"Collimator2",secondary_collimator);
      get_string2(file,"PC",pair_counter);
      get_string2(file,"TAC",tac);
      get_string2(file,"RadPhi",radphi);
      get_string2(file,"ET_coincidence",et_coincidence);
      get_string2(file,"Tagger_prescaling",tagger_prescaling);


      strstream psql;
      psql << "insert into run_log_photon (run,session_name,trigger_type,radiator,"
	   << "scaler1_odd,scaler1_even,scaler2_odd,scaler2_even,ps_converter,"
	   << "primary_collimator,secondary_collimator,pair_counter,tac,radphi,et_coincidence,"
	   << "tagger_prescaling) values ("
	   << run << comma
	   << prime << trim(session) << prime << comma
	   << prime << trim(trigger_type) << prime << comma
	   << prime << trim(radiator) << prime << comma
	   << prime << trim(scaler1_odd) << prime << comma
	   << prime << trim(scaler1_even) << prime << comma
	   << prime << trim(scaler2_odd) << prime << comma
	   << prime << trim(scaler2_even) << prime << comma
	   << prime << trim(ps_converter) << prime << comma
	   << prime << trim(primary_collimator) << prime << comma
	   << prime << trim(secondary_collimator) << prime << comma
	   << prime << trim(pair_counter) << prime << comma
	   << prime << trim(tac) << prime << comma
	   << prime << trim(radphi) << prime << comma
	   << prime << trim(et_coincidence) << prime << comma
	   << prime << trim(tagger_prescaling) << prime
	   << ")" << ends;

      // dump string if in debug mode
      if(debug==1) {
	cout << endl << "Photon for run " << run << endl;
	cout << psql.str() << endl;
      }
    

      // insert into database
      cout << "making photon entry for run " << run << endl;
      insert_into_db(psql.str());
    }


    // close file
    file.close();


  } else {
    cerr << "Begin unable to open " << bfilename << endl;
  }
  
  return;
}


//----------------------------------------------------------------


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
  char *help="\nusage:\n\n  rlb_recover [-s session] [-host dbhost] [-user dbuser] [-d database]\n"
    "              [-debug] runbeg [runend]\n\n";


  while(i<argc) {
    
    if(strncasecmp(argv[i],"-h",2)==0){
      printf(help);
      exit(EXIT_SUCCESS);
    }
    else if (strncasecmp(argv[i],"-",1)!=0){
      runbeg=atoi(argv[i]);
      if(argc>=i+2) runend=atoi(argv[i+1]); else runend=runbeg;
      return;
    }
    else if (strncasecmp(argv[i],"-host",5)==0) {
      dbhost=strdup(argv[i+1]);
      i=i+2;
    }
    else if (strncasecmp(argv[i],"-user",5)==0) {
      dbuser=strdup(argv[i+1]);
      i=i+2;
    }
    else if (strncasecmp(argv[i],"-debug",6)==0){
      debug=1;
      i=i+1;
    }
    else if (strncasecmp(argv[i],"-d",2)==0) {
      database=strdup(argv[i+1]);
      i=i+2;
    }
    else if (strncasecmp(argv[i],"-s",2)==0){
      session=strdup(argv[i+1]);
      i=i+2;
    }
  }
}


//---------------------------------------------------------------------


char *env_name(const char *env, char *name) {

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


void get_string(ifstream &file, const char* tag, char *var) {

  find_tag_line(file,tag,line,sizeof(line));
  get_next_line(file,line,sizeof(line));
  strcpy(var,line);
  return;

}


//---------------------------------------------------------------------


void get_int(ifstream &file, const char* tag, int array[], int nwd) {

  int k=0; int ind=0; int i=0;

  find_tag_line(file,tag,line,sizeof(line));
  get_next_line(file,line,sizeof(line));
  for(i=0; i<nwd; i++) {k+=ind; sscanf(&line[k],"%d %n",&array[i],&ind);}

  return;
}


//---------------------------------------------------------------------


void get_long(ifstream &file, const char* tag, unsigned long array[], int nwd) {

  int k=0; int ind=0; int i=0;

  find_tag_line(file,tag,line,sizeof(line));
  get_next_line(file,line,sizeof(line));
  for(i=0; i<nwd; i++) {k+=ind; sscanf(&line[k],"%d %n",&array[i],&ind);}

  return;
}


//---------------------------------------------------------------------


void get_hex(ifstream &file, const char* tag, unsigned long array[], int nwd) {

  int k=0; int ind=0; int i=0;
  char *p;

  find_tag_line(file,tag,line,sizeof(line));
  get_next_line(file,line,sizeof(line));
  p=line;
  for(i=0; i<nwd; i++) {p=strchr(p,'x')+1; sscanf(p,"%x %n",&array[i],&ind); p+=ind; }

  return;
}


//---------------------------------------------------------------------


void get_string2(ifstream &file, const char* tag, char *var) {

  file.setf(ios::skipws);
  file.seekg(0,ios::beg);

  for(;;) {
    if(!file.is_open()||!file.good()||file.eof()||file.fail()||file.bad()) return;
    file.getline(line,sizeof(line));
    if(strncasecmp(line+strspn(line," \t\n\r\b"),tag,strlen(tag))==0) {
      strcpy(var,strstr(line," - ")+3);
      return;
    }
  }

  return;
}


//---------------------------------------------------------------------


void get_double2(ifstream &file, const char* tag, double *var) {

  file.setf(ios::skipws);
  file.seekg(0,ios::beg);

  for(;;) {
    if(!file.is_open()||!file.good()||file.eof()||file.fail()||file.bad()) return;
    file.getline(line,sizeof(line));
    if(strncasecmp(line+strspn(line," \t\n\r\b"),tag,strlen(tag))==0) {
      *var = atof(strstr(line," - ")+3);
      return;
    }
  }

  return;
}


//---------------------------------------------------------------------


void get_int2(ifstream &file, const char* tag, int *var) {

  file.setf(ios::skipws);
  file.seekg(0,ios::beg);

  for(;;) {
    if(!file.is_open()||!file.good()||file.eof()||file.fail()||file.bad()) return;
    file.getline(line,sizeof(line));
    if(strncasecmp(line+strspn(line," \t\n\r\b"),tag,strlen(tag))==0) {
      *var = atoi(strstr(line," - ")+3);
      return;
    }
  }

  return;
}


//---------------------------------------------------------------------


void double_quotes(char out[], char in[]) {

  int i,p;

  p=0;
  for(i=0; i<strlen(in); i++) {
    if(in[i]!='\'') {
      out[p]=in[i];
      p++;
    } else {
      out[p]='\'';
      out[p+1]='\'';
      p+=2;
    }
  }
  out[p]='\0';
}


//---------------------------------------------------------------------


void get_string3(ifstream &file, const char* tag, char *var) {

  strcpy(var,"");
  find_tag_line(file,tag,line,sizeof(line));
  for(;;) {
    if(!file.is_open()||!file.good()||file.eof()||file.fail()||file.bad()) return;
    file.getline(line,sizeof(line));
    strcat(var,line);
  }
}


//---------------------------------------------------------------------


void convert_date(char *out, char *in) {

  char *p;
  char wkday[5];
  char month[5];
  int day;
  int year;
  char time[10];
  char edt[5];

  strcpy(out,"");
  
  strncpy(wkday,strtok(in," "),3);
  strncpy(month,strtok(NULL," "),3); month[3]='\0';
  day=atoi(strtok(NULL," "));
  strncpy(time,strtok(NULL," "),8); time[8]='\0';
  strncpy(edt,strtok(NULL," "),3);
  year=atoi(strtok(NULL," "));

  sprintf(out,"%d-%s-%d %s",day,month,year,time);
  return;
}


//---------------------------------------------------------------------


char *trim(char *in) {

  char *p=in+strspn(in," \t\n\r\b");
  for(int i=strlen(p)-1; i>0; i--) {
    if((p[i]==' ')||(p[i]=='\t')||(p[i]=='\n')||(p[i]=='\r')||(p[i]=='\b')) {
      p[i]='\0';
    } else {
      break;
    }
  }
  //  p[strcspn(p," \t\n\r\b")]='\0';
  return(p);

}


//---------------------------------------------------------------------
