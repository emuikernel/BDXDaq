//  run_log_comment
//
//  Uses Tk gui to collect run_log_comment info from operator, then
//    inserts into the database, event stream, and info_server
//
//
// Usage: run_log_comment -a clasprod (from rcscript)
//        run_log_comment -a clasprod -s clasprod -debug 59908 (if recovering and debugging)
//        run_log_comment -a clasprod -s clasprod 59908 (if recovering)
//

//   still to do:
//       new columns
//       get header ctl words, event class correct
//
//
//  ejw, 6-may-97
// vardan  Oct. 11 2005 add read file generated rlComment
// sergey 29-oct-2007 migrate from cdev to ca


// for posix
#define _POSIX_SOURCE_ 1
#define __EXTENSIONS__


// system stuff

using namespace std;
#include <strstream>

#include <iostream.h>
#include <rtworks/cxxipc.hxx>
#include <fstream.h>


// online stuff
extern "C"{
#include <clas_ipc_prototypes.h>
}

#include "epicsutil.h"

// for record segment header
static int nevnt  = 0;
static int nphys  = 0;
static int trig   = 0;


// constants for head bank
static int nvers  = 0;
static int type   = 101;
static int rocst  = 0;
static int evcls  = 0;
static int presc  = 0;


// flags to inhibit event recording, etc.
static int no_dbr  = 0;
static int no_info = 0;
static int no_data = 0;
static int debug   = 0;


// misc vars
static char *application    = (char*)"clastest";
static char *uniq_dgrp      = (char*)"run_log_comment";
static char *id_string      = (char*)"run_log_comment";
static char *dest           = (char*)"dbrouter";
static char *mysql_database  = (char*)"clasrun";
static char session[50]     = "";
static int gmd_time         =  5;
static int filep            = 0;
static char temp[128];

static char *archive_file_name = (char*)"run_log/archive/comment_%s_%06d.txt";
char *date = NULL;
char *runtype = NULL;
char *beamcurrent = NULL;
char *raster = NULL;
char *shiftcrue = NULL;
char *comment = NULL;
char *logbook = NULL;
char *page = NULL;
char *target = NULL;
char *tk_b_ener = NULL;
char *tk_t_cur = NULL;
char *tk_mt_cur = NULL;
char *trig_conf = NULL;
char *chan_conf = NULL;
char *l1_mask = NULL;
char *prescale = NULL; 
char *ec_inner_l = NULL; 
char *ec_inner_h = NULL; 
char *ec_outer_l = NULL; 
char *ec_outer_h = NULL; 
char *ec_total_l = NULL; 
char *ec_total_h = NULL; 
char *sc_h = NULL; 
char *cc_h = NULL; 
char *cc_l = NULL; 
char *photon_trig_conf = NULL; 
char *radiator = NULL; 
char *scaler_config1 = NULL; 
char *scaler_config2 = NULL; 
char *scaler_config3 = NULL; 
char *scaler_config4 = NULL;
char *PS_converter = NULL;
char *collimator1 = NULL;   
char *collimator2 = NULL; 
char *PC_converter = NULL;
char *TAC = NULL;
char *RadPhi = NULL;
char *et_coincidence = NULL;
char *tagger_prescaling = NULL;

// data variables
static int run;
static char *config;
static char entry_date[30]; 



static char *epics_name[] = {
  "beam_energy",
  "torus current",
  "mini current",
  "tagger current",
};

static char *epics_chan[] = {
  "MBSY2C_energy",
  "torus_current",
  "MTIRBCK",
  "TMIRBCK",
};

static float      epics_val[sizeof(epics_chan)/sizeof(char *)];
static int nepics  = sizeof(epics_chan)/sizeof(char *);



// prototypes
void decode_command_line(int argc, char **argv);
void collect_data(strstream &sql_string, strstream &sql_string2);
int read_from_file(strstream &sql_string, strstream &sql_string2);
void get_epics_data(void);
void insert_into_ipc(char *entry, char *entry2);
void insert_into_data(char *entry, char *entry2);
int read_user_data(strstream &filename);
void get_string2(ifstream &file, const char* tag, char *var);
char *env_name(const char *env, const char *name);

void get_string(ifstream &file, const char* tag, char *var);
void get_int(ifstream &file, const char* tag, int array[], int nwd);
void get_long(ifstream &file, const char* tag, unsigned long array[], int nwd);
void get_hex(ifstream &file, const char* tag, unsigned long array[], int nwd);
void get_string3(ifstream &file, const char* tag, char *var);
void get_int2(ifstream &file, const char* tag, int *var);
void get_double2(ifstream &file, const char* tag, double *var);
void convert_date(char *out, char *in);
void double_quotes(char out[], char in[]);
int find_tag_line(ifstream &file, const char* tag, char buffer[], int buflen);
int get_next_line(ifstream &file, char buffer[], int buflen);
char *trim(char *in);


extern "C" {
int get_run_config(const char *mysql_database, const char *session, int *run, char **config);
}


// program start time
static time_t start=time(NULL);


//--------------------------------------------------------------------------
  // open file in $CLON_PARMS/run_log/archive/comment_<session>_<runnumber>.txt

int
main(int argc,char **argv)
{
  int ret;
  strstream sql_string, sql_string2;

  // decode command line
  decode_command_line(argc,argv);

  if(filep==0)
  {
    strstream filename;
    filename << getenv("CLON_PARMS") << "/run_log/current_usr_input.txt" << ends;
    read_user_data(filename);
  }

  // synch with stdio
  ios::sync_with_stdio();

  // no ipc or data in recovery mode
  if(filep!=0)
  {
    no_info=1;
    no_data=1;
  }

  // normal mode
  if(filep==0)
  {
    // set session name if not specified via env variable or on command line
    if(strlen(session)==0) strcpy(session,"clasprod");

    // collect data (returns 2 sql strings:  standard + optional photon string)
    collect_data(sql_string,sql_string2);

    // inserts sql strings into data stream as special database event
    if((no_data==0)&&(debug==0))insert_into_data(sql_string.str(),sql_string2.str());

    // ship data to dbrouter and info server
    if(debug==0)insert_into_ipc(sql_string.str(),sql_string2.str());
  }
  else // recovery mode
  {
    printf("RECOVERY MODE\n");

    for(int i=filep; i<argc; i++)
    {
      // read data from text file and create sql strings
      run = atoi(argv[i]);
      printf("Trying to read archive file for run number %d (>%s<)\n",run,argv[i]);
      ret = read_from_file(sql_string,sql_string2);
      if(ret) continue; // did not get anything

      if(debug==0) insert_into_ipc(sql_string.str(),sql_string2.str());
	}
  }

  // debug...just print sql strings
  if(debug!=0)
  {
    cout << "\nsql string for run " << run << " is:\n\n" << sql_string.str() << endl << endl;
    if(strncasecmp(runtype,"photon",6)==0)
    {
      cout << "\nsql photon string for run " << run << " is:\n\n" << sql_string2.str() << endl;
    }
  }


  // done
  exit(EXIT_SUCCESS);
}
       

//----------------------------------------------------------------


int
read_from_file(strstream &sql, strstream &sql2)
{
  char entry_date[35];
  char target[25];
  char run_type[25];
  double beam_current_request;
  char mini_raster[20];
  char operators[101];
  int logbook_book;
  int logbook_page;
  char ignore_run[2000];
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



  int ii,ind,flen,ret;
  char *p, date1[30], time1[30];
  char line[4096];
  static char filename[256];


  // read run log archive file
  sprintf(filename,archive_file_name,session,run);
  ifstream file(env_name("CLON_PARMS",filename));
  if(!file.is_open())
  {
    printf("File >%s< does not exist - exit\n",filename);
    return(-1);
  }
  else
  {
    printf("Use file >%s<\n",env_name("CLON_PARMS",filename));
  }

  // read file generated by rlComment


  get_string2(file,"DATE",temp);
  convert_date(entry_date,temp);
  get_string2(file,"RUNTYPE",run_type);
  runtype = strdup(run_type);
  get_string2(file,"TARGET",target);
  get_double2(file,"BEAMREQUEST",&beam_current_request);
  get_string2(file,"MINIRASTER",mini_raster);
  get_string2(file,"OPERATORS",temp);
  double_quotes(operators,temp);
  get_int2(file, "LOG_BOOK", &logbook_book);
  get_int2(file, "LOG_PAGE", &logbook_page);
  get_string(file,"*IGNORE*",ignore_run);
  get_string3(file,"*COMMENT*",temp);
  double_quotes(comment,temp);


  // form sql string
  sql << "insert into run_log_comment (run,session_name,entry_date,run_type,target,"
	 << "beam_current_request," 
	 << "mini_raster,operators,logbook_book,logbook_page,ignore_run,comment) values ("
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
     << prime << ignore_run << prime << comma
	 << prime << trim(comment) << prime 
	 << ")" << ends;


  // is this a photon run
  if(strncasecmp(runtype,"photon",6)==0)
  {
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

    sql2 << "insert into run_log_photon (run,session_name,trigger_type,radiator,"
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

  }





  file.close();

  return(0);
}


//--------------------------------------------------------------------------


int
read_user_data(strstream &name)
{
  
  ifstream file(name.str());
 
  int len;
  char buffer[256];

  if (! file.is_open())
  {
    cout << "Error opening file"; exit (1);
  }


   /* Sergey: need something like that 
  while(file.is_open()&&file.good()&&!file.eof()&&!file.fail()&&!file.bad()) {
   */

  while (! file.eof() )
  {
    file.getline (buffer,100);
    len = strlen(buffer);
    cout << len << " " << buffer << endl;

//Sergey: break loop if received zero length string; if 'comment' longer then 'buffer'
// we are looping forever and end_of_file never shows up ...
if(len <= 0)
{
  printf("===> len=%d - break loop\n",len);
  break;
}

    if(strncasecmp(buffer,"DATE",4)==0){file.getline (buffer,100);date = (char *)malloc(strlen(buffer)+1); strcpy(date,buffer); }
    else if(strncasecmp(buffer,"RUNTYPE",7)==0){file.getline (buffer,100);runtype = (char *)malloc(strlen(buffer)+1); strcpy(runtype,buffer);}
    else if(strncasecmp(buffer,"BEAMREQUEST",11)==0){file.getline (buffer,100);beamcurrent = (char *)malloc(strlen(buffer)+1); strcpy(beamcurrent,buffer);}
    else if(strncasecmp(buffer,"MINIRASTER",10)==0){file.getline (buffer,100);raster = (char *)malloc(strlen(buffer)+1); strcpy(raster,buffer);}
    else if(strncasecmp(buffer,"OPERATORS",9)==0){file.getline (buffer,100);shiftcrue = (char *)malloc(strlen(buffer)+1); strcpy(shiftcrue,buffer);}
    else if(strncasecmp(buffer,"COMMENT",7)==0)
    {
      file.getline (buffer,100);
      if(strlen(buffer)==99) printf("===> WARNING: comment line maybe cut !!!\n");
      printf("===> %d >%s<\n",strlen(buffer),buffer);
      comment = (char *)malloc(strlen(buffer)+1);
      strcpy(comment,buffer);
    }
    else if(strncasecmp(buffer,"LOG_BOOK",8)==0){file.getline (buffer,100);logbook = (char *)malloc(strlen(buffer)+1); strcpy(logbook,buffer);}
    else if(strncasecmp(buffer,"LOG_PAGE",8)==0){file.getline (buffer,100);page = (char *)malloc(strlen(buffer)+1); strcpy(page,buffer);}
    else if(strncasecmp(buffer,"TARGET",6)==0){file.getline (buffer,100);target = (char *)malloc(strlen(buffer)+1); strcpy(target,buffer);}
    else if(strncasecmp(buffer,"BEAMENERGY",10)==0){file.getline (buffer,100);tk_b_ener = (char *)malloc(strlen(buffer)+1); strcpy(tk_b_ener,buffer);}
    else if(strncasecmp(buffer,"TORUSCURRENT",12)==0){file.getline (buffer,100);tk_t_cur = (char *)malloc(strlen(buffer)+1); strcpy(tk_t_cur,buffer);}
    else if(strncasecmp(buffer,"MINICURRENT",11)==0){file.getline (buffer,100);tk_mt_cur = (char *)malloc(strlen(buffer)+1); strcpy(tk_mt_cur,buffer);}
    else if(strncasecmp(buffer,"TIGRIS",6)==0){file.getline (buffer,100);trig_conf = (char *)malloc(strlen(buffer)+1); strcpy(trig_conf,buffer);}
    else if(strncasecmp(buffer,"CHANNELCONFIG",13)==0){file.getline (buffer,100);chan_conf = (char *)malloc(strlen(buffer)+1); strcpy(chan_conf,buffer);}
    else if(strncasecmp(buffer,"L1MASK",6)==0){file.getline (buffer,100);l1_mask = (char *)malloc(strlen(buffer)+1); strcpy(l1_mask,buffer);}
    else if(strncasecmp(buffer,"PRESCALE",8)==0){file.getline (buffer,100);prescale = (char *)malloc(strlen(buffer)+1); strcpy(prescale,buffer); }
    else if(strncasecmp(buffer,"ECINNERLO",9)==0){file.getline (buffer,100);ec_inner_l = (char *)malloc(strlen(buffer)+1); strcpy(ec_inner_l,buffer); }
    else if(strncasecmp(buffer,"ECINNERHI",9)==0){file.getline (buffer,100);ec_inner_h = (char *)malloc(strlen(buffer)+1); strcpy(ec_inner_h,buffer); }
    else if(strncasecmp(buffer,"ECOUTERLO",9)==0){file.getline (buffer,100);ec_outer_l = (char *)malloc(strlen(buffer)+1); strcpy(ec_outer_l,buffer); }
    else if(strncasecmp(buffer,"ECOUTERHI",9)==0){file.getline (buffer,100);ec_outer_h = (char *)malloc(strlen(buffer)+1); strcpy(ec_outer_h,buffer); }
    else if(strncasecmp(buffer,"ECTOTALLO",9)==0){file.getline (buffer,100);ec_total_l = (char *)malloc(strlen(buffer)+1); strcpy(ec_total_l,buffer); }
    else if(strncasecmp(buffer,"ECTOTALHI",9)==0){file.getline (buffer,100);ec_total_h = (char *)malloc(strlen(buffer)+1); strcpy(ec_total_h,buffer); }
    else if(strncasecmp(buffer,"SCHI",4)==0){file.getline (buffer,100);sc_h = (char *)malloc(strlen(buffer)+1); strcpy(sc_h,buffer); }
    else if(strncasecmp(buffer,"CCHI",4)==0){file.getline (buffer,100);cc_h = (char *)malloc(strlen(buffer)+1); strcpy(cc_h,buffer); }
    else if(strncasecmp(buffer,"CCLO",4)==0){file.getline (buffer,100);cc_l = (char *)malloc(strlen(buffer)+1); strcpy(cc_l,buffer); }
    else if(strncasecmp(buffer,"PhotonTrigger",13)==0){file.getline (buffer,100);photon_trig_conf = (char *)malloc(strlen(buffer)+1); strcpy(photon_trig_conf,buffer); }
    else if(strncasecmp(buffer,"Radiator",8)==0){file.getline (buffer,100);radiator = (char *)malloc(strlen(buffer)+1); strcpy(radiator,buffer); }
    else if(strncasecmp(buffer,"Scalerconfig1",13)==0){file.getline (buffer,100);scaler_config1 = (char *)malloc(strlen(buffer)+1); strcpy(scaler_config1,buffer); }
    else if(strncasecmp(buffer,"Scalerconfig2",13)==0){file.getline (buffer,100);scaler_config2 = (char *)malloc(strlen(buffer)+1); strcpy(scaler_config2,buffer); }
    else if(strncasecmp(buffer,"Scalerconfig3",13)==0){file.getline (buffer,100);scaler_config3 = (char *)malloc(strlen(buffer)+1); strcpy(scaler_config3,buffer); }
    else if(strncasecmp(buffer,"Scalerconfig4",13)==0){file.getline (buffer,100);scaler_config4 = (char *)malloc(strlen(buffer)+1); strcpy(scaler_config4,buffer);}
    else if(strncasecmp(buffer,"PSconvertor",11)==0){file.getline (buffer,100);PS_converter = (char *)malloc(strlen(buffer)+1); strcpy(PS_converter,buffer);}
    else if(strncasecmp(buffer,"Collimator1",11)==0){file.getline (buffer,100);collimator1 = (char *)malloc(strlen(buffer)+1); strcpy(collimator1,buffer); }
    else if(strncasecmp(buffer,"Collimator2",11)==0){file.getline (buffer,100);collimator2 = (char *)malloc(strlen(buffer)+1); strcpy(collimator2,buffer); }
    else if(strncasecmp(buffer,"PCconvertor",11)==0){file.getline (buffer,100);PC_converter = (char *)malloc(strlen(buffer)+1); strcpy(PC_converter,buffer);}
    else if(strncasecmp(buffer,"TAC",3)==0){file.getline (buffer,100);TAC = (char *)malloc(strlen(buffer)+1); strcpy(TAC,buffer);}
    else if(strncasecmp(buffer,"RadPhi",6)==0){file.getline (buffer,100);RadPhi = (char *)malloc(strlen(buffer)+1); strcpy(RadPhi,buffer);}
    else if(strncasecmp(buffer,"ETcoincidence",13)==0){file.getline (buffer,100);et_coincidence = (char *)malloc(strlen(buffer)+1); strcpy(et_coincidence,buffer);}
    else if(strncasecmp(buffer,"Tagger_prescaling",17)==0){file.getline (buffer,100);tagger_prescaling = (char *)malloc(strlen(buffer)+1); strcpy(tagger_prescaling,buffer);}
    else
    {
      printf("===> unknown command >%s<\n",buffer);
    }    
 }

  return 0;
}   


//----------------------------------------------------------------


void
collect_data(strstream &sql, strstream &sql2)
{
  strstream query;
  struct tm *tstruct;
  const char *prime="'", *comma=",", *end=")";
  int i;


  // get run and config
  get_run_config(mysql_database,session,&run,&config);

  // get epics data
  get_epics_data();

  // create entry date in ingres format
  tstruct=localtime(&start);

  /*sergey strftime(entry_date,25," %d-%b-%Y %H:%M",tstruct);*/
  strftime(entry_date,25," %Y-%m-%d %H:%M:%S",tstruct);
  
  //  construct sql string
  sql.setf(ios::showpoint);
  sql << "insert into run_log_comment ("
    "session_name,run,entry_date,run_type,target,beam_current_request,mini_raster,"
    "operators,comment,logbook_book,logbook_page,ignore_run"
    ") values ("
      << prime << session << prime 
      << comma << run
      << comma << prime << entry_date << prime
      << comma << prime << runtype << prime
      << comma << prime << target << prime
      << comma << atof(beamcurrent)
      << comma << prime << raster << prime
      << comma << prime << shiftcrue << prime
      << comma << prime << comment << prime
      << comma << atoi(logbook)
      << comma << atoi(page)
      << comma << prime << "N" << prime
      << end << ends;


  //  optional photon string
  if(strncasecmp(runtype,"photon",6)==0) {
    sql2 << "insert into run_log_photon ("
      "session_name,run,trigger_type,radiator,scaler1_odd,scaler1_even,scaler2_odd,"
      "scaler2_even,ps_converter,primary_collimator,secondary_collimator,"
      "pair_counter,tac,radphi,et_coincidence,tagger_prescaling) values ("
	 << prime << session << prime 
	 << comma << run
	 << comma << prime << photon_trig_conf << prime
	 << comma << prime << radiator << prime
	 << comma << prime << scaler_config1 << prime
	 << comma << prime << scaler_config2 << prime
	 << comma << prime << scaler_config3 << prime
	 << comma << prime << scaler_config4 << prime
	 << comma << prime << PS_converter << prime
	 << comma << prime << collimator1 << prime
	 << comma << prime << collimator2 << prime
	 << comma << prime << PC_converter << prime
	 << comma << prime << TAC << prime
	 << comma << prime << RadPhi << prime
	 << comma << prime << et_coincidence << prime
	 << comma << prime << tagger_prescaling << prime
	 << end << ends;
  } else {
    sql2 << ends;
  }


  return;
}


//----------------------------------------------------------------
  

void
get_epics_data()
{
  int result;

  // read channels (use CA !)
  for(int i=0; i<nepics; i++)
  {
    epics_val[i]=-1.0;
  }
  result = getepics(nepics, epics_chan, epics_val);

  return;
}


//----------------------------------------------------------------

void
insert_into_ipc(char *entry, char *entry2)
{
  // disable gmd timeout
  T_OPTION opt=TutOptionLookup((T_STR)"Server_Delivery_Timeout");
  TutOptionSetNum(opt,0.0);

  // connect to server
  TipcSrv &server=TipcSrv::Instance();
  dbr_init(uniq_dgrp,application,id_string);


  // dbrouter message
  if(no_dbr==0)
  {
    TipcMsg msg = TipcMsg((T_STR)"dbr_request");
    msg.Dest(dest);
    msg.Sender(uniq_dgrp);
    msg.UserProp(0.0);
    msg.DeliveryMode(T_IPC_DELIVERY_ALL);
    
    // fill and send...check for photon run
    msg << (T_INT4) 1 << entry;
    if(strncasecmp(runtype,"photon",6)==0) {
      msg << (T_INT4) 1 << entry2;
    }
    server.Send(msg);
  }


  // info server message
  if(no_info==0) {
    TipcMsg msg = TipcMsg((T_STR)"info_server");
    msg.Dest((T_STR)"info_server/in/run_log_comment");
    msg.Sender(uniq_dgrp);
    
    // fill and send
    msg << (T_STR)"run_log_comment" 
	<< (T_INT4) run << session << entry_date
	<< runtype
	<< target
	<< (T_REAL8) atof(beamcurrent)
	<< shiftcrue
	<< comment
	<< (T_INT4) atoi(logbook)
	<< (T_INT4) atoi(page)
	<< (T_REAL8) atof(tk_b_ener)
	<< raster 
	<< (T_REAL8) atof(tk_t_cur)
	<< (T_REAL8) atof(tk_mt_cur)
	<< trig_conf 
	<< chan_conf 
	<< l1_mask 
	<< prescale
	<< (T_INT4) atoi(ec_inner_h)
	<< (T_INT4) atoi(ec_inner_l)
	<< (T_INT4) atoi(ec_outer_h)
	<< (T_INT4) atoi(ec_outer_l)
	<< (T_INT4) atoi(ec_total_h)
	<< (T_INT4) atoi(ec_total_l)
	<< (T_INT4) atoi(sc_h)
	<< (T_INT4) atoi(cc_h)
	<< (T_INT4) atoi(cc_l);
    server.Send(msg);


    if(strncasecmp(runtype,"photon",6)==0) {
      TipcMsg msg2 = TipcMsg((T_STR)"info_server");
      msg2.Dest((T_STR)"info_server");
      msg2.Sender(uniq_dgrp);
      msg2 << (T_STR)"run_log_photon" 
	   << (T_INT4) run << session << entry_date
	   << photon_trig_conf
	   << radiator
	   << scaler_config1
	   << scaler_config2
	   << scaler_config3
	   << scaler_config4
	   << PS_converter
	   << collimator1
	   << collimator2
	   << PC_converter
	   << TAC
	   << RadPhi
	   << et_coincidence
	   << tagger_prescaling;
      server.Send(msg2);
    }
  }


  // flush all messages
  server.Flush();
  
  
  // allow gmd to acknowledge receipt
  if(no_dbr==0)dbr_check((double) gmd_time);


  // close ipc connection
  dbr_close();


  return;
}

//----------------------------------------------------------------
  

void
insert_into_data(char *entry, char *entry2)
{
  TipcSrv &server=TipcSrv::Instance();
  dbr_init(uniq_dgrp,application,id_string);


  TipcMsg msg((T_STR)"evt_bosbank");
  msg.Sender((T_STR)"run_log_comment");
  sprintf(temp,"evt_bosbank/%s",session);
  msg.Dest(temp);


  // fill data bank with logbook info
  int buflen=(strlen(entry)+3)/4;
  msg << (T_STR)"RNLG" << (T_INT4)2 << (T_STR)"(A)" << (T_INT4)1 << (T_INT4)1 << (T_INT4)buflen
      << SetSize(buflen) << (T_INT4*)entry;


  // fill data bank with optional photon info
  if(strncasecmp(runtype,"photon",6)==0) {
    buflen=(strlen(entry2)+3)/4;
    msg << (T_STR)"RNLG" << (T_INT4)3 << (T_STR)"(A)" << (T_INT4)1 << (T_INT4)1 << (T_INT4)buflen
	<< SetSize(buflen) << (T_INT4*)entry2;
  }


  server.Send(msg);
  server.Flush();
  dbr_close();

  return;
}

//---------------------------------------------------------------------

void
get_string(ifstream &file, const char* tag, char *var)
{
  char line[1024];
  find_tag_line(file,tag,line,sizeof(line));
  get_next_line(file,line,sizeof(line));
  strcpy(var,line);
  return;
}

//---------------------------------------------------------------------

void
get_double2(ifstream &file, const char* tag, double *var)
{
  char line[1024];
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


void
get_int2(ifstream &file, const char* tag, int *var)
{
  char line[1024];
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


void
get_string2(ifstream &file, const char* tag, char *var)
{
  char line[1024];
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


void
double_quotes(char out[], char in[])
{
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


void
get_string3(ifstream &file, const char* tag, char *var)
{
  char line[1024];
  strcpy(var,"");
  find_tag_line(file,tag,line,sizeof(line));
  for(;;) {
    if(!file.is_open()||!file.good()||file.eof()||file.fail()||file.bad()) return;
    file.getline(line,sizeof(line));
    strcat(var,line);
  }
}


//---------------------------------------------------------------------


void
convert_date(char *out, char *in)
{
  char *p;
  char wkday[5];
  char month[5];
  int day;
  int mon;
  int year;
  char time[10];
  char edt[5];

  strcpy(out,"");
  
  strncpy(wkday,strtok(in," "),3);

  strncpy(month,strtok(NULL," "),3); month[3]='\0';
  if(!strncmp(month,"Jan",3))      mon=1;
  else if(!strncmp(month,"Feb",3)) mon=2;
  else if(!strncmp(month,"Mar",3)) mon=3;
  else if(!strncmp(month,"Apr",3)) mon=4;
  else if(!strncmp(month,"May",3)) mon=5;
  else if(!strncmp(month,"Jun",3)) mon=6;
  else if(!strncmp(month,"Jul",3)) mon=7;
  else if(!strncmp(month,"Aug",3)) mon=8;
  else if(!strncmp(month,"Sep",3)) mon=9;
  else if(!strncmp(month,"Oct",3)) mon=10;
  else if(!strncmp(month,"Nov",3)) mon=11;
  else if(!strncmp(month,"Dec",3)) mon=12;
  else printf("ERROR: unknown month >%s<\n",month);

  day=atoi(strtok(NULL," "));

  strncpy(time,strtok(NULL," "),8); time[8]='\0';

  strncpy(edt,strtok(NULL," "),3);

  year=atoi(strtok(NULL," "));

  /*print time in following form: 2009-05-14 08:05:46*/
  sprintf(out,"%d-%02d-%02d %s",year,mon,day,time);
  return;
}


//---------------------------------------------------------------------


char *
trim(char *in)
{
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


char *
env_name(const char *env, const char *name)
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

//----------------------------------------------------------------


void
decode_command_line(int argc, char **argv)
{

  int i=1;
  const char *help="\nusage:\n\n  run_log_comment [-a application] [-u uniq_dgrp] [-i id_string] [-debug]\n"
               "        [-d destination] [-m mysql_database] [-s session] [-no_dbr]\n"
               "        [-no_info] [-no_data] [-g gmd_time] file1 file2 ...\n\n\n";


  while(i<argc)
  {  
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
    else if (strncasecmp(argv[i],"-no_dbr",7)==0){
      no_dbr=1;
      i=i+1;
    }
    else if (strncasecmp(argv[i],"-no_info",8)==0){
      no_info=1;
      i=i+1;
    }
    else if (strncasecmp(argv[i],"-no_data",8)==0){
      no_data=1;
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
    else if (strncasecmp(argv[i],"-m",2)==0){
      mysql_database=strdup(argv[i+1]);
      i=i+2;
    }
  }
}


//---------------------------------------------------------------------

