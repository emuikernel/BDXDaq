
#define RUN_LOG_TPE

//  run_log_begin
//
//  collects and inserts run log begin info into database, datastream, info_server,
//     and file
//
// Usage: run_log_begin -a clasprod (from rcscript)
//        run_log_begin -a clasprod -debug (debugging)
//        run_log_begin -a clasprod -s clasprod -debug 59908 (if recovering and debugging)
//        run_log_begin -a clasprod -s clasprod 59908 (if recovering)
//
//   still to do:
//       tagger trigger words
//
//  ejw, 21-sep-2000
// sergey 29-oct-2007 migrate from cdev to ca
//   also 'epics_val' and 'polar_epics_val' are 'float' now, they were 'double' - is it Ok ?????
// sergey may 2009 added recovery mode
//
//

// for posix
#define _POSIX_SOURCE_ 1
#define __EXTENSIONS__


// system stuff

using namespace std;
#include <strstream>

#include <iostream.h>
#include <iomanip.h>
#include <fstream.h>
#include <sys/types.h>
#include <unistd.h>


// for ipc
#include <rtworks/cxxipc.hxx>


// online and coda stuff
extern "C" {
#include <clas_ipc_prototypes.h>
}

#include "epicsutil.h"


// flags to inhibit event recording, etc.
static int no_dbr    = 0;
static int no_info   = 0;
static int no_data   = 0;
static int no_file   = 0;
static int debug     = 0;


// control params
static char *application       	   = (char*)"clastest";
static char *uniq_subj         	   = (char*)"run_log_begin";
static char *id_string         	   = (char*)"run_log_begin";
static char *msql_database     	   = (char*)"clasrun";
static char session[50]          = "";
static int gmd_time                =  3;
static int filep               	 = 0;


// file names
static char *archive_file_name 	   = (char*)"run_log/archive/begin_%s_%06d.txt";
static char *pretrig_file_name 	   = (char*)"pretrigger/archive/pretrig_%s_%06d.txt";
static char *current_trigger_name  = (char*)"trigger/current_trig_config.txt";
static char *current_channel_name  = (char*)"channel/current_channel_config.txt";
static char *current_tsfile_name   = (char*)"ts/ts_file.txt";
static char *current_l1prog_name   = (char*)"ts/Current.Config";
static char *sc_spar_name          = (char*)"pedman/archive/sc1.ped";
static char *cc_spar_name          = (char*)"pedman/archive/cc1.ped";
static char *ec1_spar_name         = (char*)"pedman/archive/ec1.ped";
static char *ec2_spar_name         = (char*)"pedman/archive/ec2.ped";
static char *lac_spar_name         = (char*)"pedman/archive/lac1.ped";


//  run start data
int run;
static char config[30] = "";
static char *configin;
static char startdate[30];
static unsigned long ts_csr=0,ts_control=0,ts_roc_enable=0,ts_synch=0,ts_timer[5];
static int prescale[8];
static char ts_file[100], l1_prog[120], trigger_config[80], channel_config[80];
static char sc_spar[100], cc_spar[100], ec1_spar[100], ec2_spar[100], lac_spar[100];
static int ec_inner_hi=0,ec_inner_lo=0,ec_outer_hi=0,ec_outer_lo=0,ec_total_hi=0,ec_total_lo=0;
static int cc_hi=0,cc_lo=0;
static int sc_thresh=0,sc_width=0;
static unsigned long ts_reg[128];


// run_log_begin epics channel names, etc.
static char *db_name[] = {
  "beam_energy",
  /*"thermionic_gun",*/
  "polarized_gun",
  "a_slit_position",
  "b_slit_position",
  "c_slit_position",
  /*"radiator_position",*/
  "torus_current",
  "mini_current",
  "mini_voltage",
  "tagger_current",
  "tagger_voltage",

  /*"cryo_pressure",*/ /*timeout or zero*/
  /*"cryo_temperature",*/ /*timeout or zero*/
  /*"cryo_status",*/ /*timeout or zero*/

  "upstream_beam_vac",
  "target_vac",
  "halo_up_up",
  "halo_up_down",
  "halo_up_left",
  "halo_up_right", 
  "halo_down_up",
  "halo_down_down",
  "halo_down_left",
  "halo_down_right", 
  "bpm_1_x",
  "bpm_1_y",
  "bpm_1_i",
  "bpm_2_x",
  "bpm_2_y",
  "bpm_2_i", 
  "bpm_3_x",
  "bpm_3_y",
  "bpm_3_i",

  /*since g9frost*/
  "helicity_clock",
  "helicity_t_settle",
  "helicity_t_stable",
  "helicity_delay",
  "helicity_pattern",
  "helicity_frequency",
  "helicity_wien_angle_zy",
  "helicity_wien_angle_yx",
  "helicity_wien_angle_zx",
  "helicity_half_wave_plate",
  /*"nmr_polarization_lf",*/
  /*"magnet_current_lf",*/
  "cohbrems_edge",
  "cohbrems_plane",
  "cohbrems_radiator"
};

static char *epics_chan[] = {
  "MBSY2C_energy",
  /*"IGT0I00BIASET", does not exist*/
  "IGL1I00DAC2",
  "SMRPOSA",
  "SMRPOSB",
  "SMRPOSC",
  /*"harp", does not exist*/
  "torus_current",
  "MTIRBCK",
  "MTVRBCK",
  "TMIRBCK",
  "TMVRBCK",

  /*"B_cryotarget_pressure",*/ /*timeout or zero*/
  /*"B_cryotarget_temperature",*/ /*timeout or zero*/
  /*"B_cryotarget_status",*/ /*timeout or zero*/

  "VCG2C24",
  "VCG2H01",
  "scalerS2o",
  "scalerS3o",
  "scalerS4o",
  "scalerS5o", 
  "scalerS6o",
  "scalerS7o",
  "scalerS8o",
  "scalerS9o", 
  "IPM2H01.XPOS",
  "IPM2H01.YPOS",
  "IPM2H01",
  "IPM2C24A.XPOS",
  "IPM2C24A.YPOS",
  "IPM2C24A",
  "IPM2C21A.XPOS",
  "IPM2C21A.YPOS",
  "IPM2C21A",

  /*since g9frost*/
  "HELCLOCKd",
  "HELTSETTLEd",
  "HELTSTABLEd",
  "HELDELAYd",
  "HELPATTERNd",
  "HELFREQ",
  "VWienAngle",
  "Phi_FG",
  "HWienAngle",
  "IGL1I00DI24_24M",
  /*"frost_Pol_LF",*/
  /*"frost_Amps_LF",*/
  "coh_edge",
  "coh_plane",
  "coh_radiator"
};

static int ncallback = 0;
static int nepics    = sizeof(epics_chan)/sizeof(char *);
static float epics_val[sizeof(epics_chan)/sizeof(char *)];


// run_log_polar epics channel names
static char *polar_db_name[] = {
  "raster_status",
  "raster_set_x",
  "raster_set_y",
  "raster_set_x_offset",
  "raster_set_y_offset",
  "raster_x_offset",
  "raster_y_offset",
  "raster_rate",
  "raster_x_size",
  "raster_y_size",
  "half_wave_plate",
  /*"wien_angle",*/
  "pair_sync_rate",
  "beam_polarization",
  "beam_polarization_err",
  "pos_helicity_rate",
  "helicity_flip_scheme"
  /*
  "pt_banjo_he_level",
  "pt_minicup_he_level",
  "pt_polarization",
  "pt_polarization_err",
  "pt_calib_const",
  "pt_loc_index",
  "pt_encoded",
  "pt_current",
  "pt_temperature",
*/
};

static char *polar_epics_chan[] = {
  "raster_status_ttl.RVAL",
  "RASTSETPATTERNX",
  "RASTSETPATTERNY",
  "RASTSETXOFFSET",
  "RASTSETYOFFSET",
  "RASTXOFFSET",
  "RASTYOFFSET",
  "RASTCYCLETIME",
  "RASTPATSIZEX",
  "RASTPATSIZEY",
  "IGL1I00OD16_16.RVAL",
  /*"HallProbe2_val",*/
  "scaler_dS5b",
  "beam_polarization",
  "beam_polarization_error",
  "scaler_dS6b",
  "IGL1I00DIOFHRD"
  /*
  "LL8240",
  "LL8245",
  "hallbptpol",
  "unknown",
  "hallbptcal",
  "hallbptindex",
  "hallbptenc",
  "hallbptcur",
  "hallbpttemp",
  */
};

static int npolcallback = 0;
static int npolepics    = sizeof(polar_epics_chan)/sizeof(char *);
static float polar_epics_val[sizeof(polar_epics_chan)/sizeof(char *)];


#ifdef _RTPC_RUN_LOG_
// run_log_rtpc epics channel names
static char *rtpc_db_name[] = {
  "solenoid_current",
  "rtpc_drft_pres",		
  "rtpc_tagt_pres",		
  "rtpc_wrk_gas_flow",	
  "rtpc_qch_gas_flow",	
  "rtpc_he_bag_flow",	
  "rtpc_hv_gem",		
  "rtpc_hv_cath",		
};

static char *rtpc_epics_chan[] = {
  "MSL2H01M",
  "mks1_c1_val_chk",
  "HPI86BT44", 
  "mks1_c3_val_chk",
  "mks1_c2_val_chk", 
  "mks1_c4_val_chk",
  "rtpc_hv_gem",
  "rtpc_hv_cath",
};

static int nrtpccallback = 0;
static int nrtpcepics    = sizeof(rtpc_epics_chan)/sizeof(char *);
static float rtpc_epics_val[sizeof(rtpc_epics_chan)/sizeof(char *)];
#endif


#ifdef RUN_LOG_TPE
static char *rtpc_db_name[] = {
  "pair_spec_current",
  "pair_spec_probe",
  "frascati_magnet_field",
  "frascati_magnet_current",
  "sfm_average_x",
  "sfm_average_y",
  "convertor_position",
  "radiator",
  "collimator",
  "beam_left_half_blocker",
  "beam_right_half_blocker",
  "beam_left_lead_door",
  "beam_right_lead_door",
  "beam_left_door_open",
  "beam_right_door_open",
  "beam_left_half_door",
  "beam_right_half_door",
  "beam_left_door_fully_extended",
  "beam_right_door_fully_extended"
};

static char *rtpc_epics_chan[] = {
  "PSPECIRBCK",
  "Hac_PS_LS450:FLD_DATA",
  "Hac_FRST2_LS450:FLD_DATA",
  "hallb_sf_xy560_0_14",
  "tgp_avg_x",
  "tgp_avg_y",
  "harp_2h00.RBV",
  "radiator_long.RBV",
  "collimator.RBV",
  "eg2_target_f_cmd",
  "eg2_target_b_cmd",
  "eg2_target_c_cmd",
  "eg2_target_d_cmd",
  "eg2_target_c_status",
  "eg2_target_c_switch",
  "eg2_target_b_status",
  "eg2_target_d_switch",
  "eg2_target_a_status",
  "eg2_target_d_status"
};

static int nrtpccallback = 0;
static int nrtpcepics    = sizeof(rtpc_epics_chan)/sizeof(char *);
static float rtpc_epics_val[sizeof(rtpc_epics_chan)/sizeof(char *)];
#endif


// misc
static char filename[256];
//static char line[4096];


// prototypes
void decode_command_line(int argc, char **argv);
void collect_data(void);
int read_from_file(void);
#ifdef _RTPC_RUN_LOG_
void create_sql(strstream &rlb_string, strstream &rlp_string, strstream &rlr_string);
#else
#ifdef RUN_LOG_TPE
void create_sql(strstream &rlb_string, strstream &rlp_string, strstream &rlr_string);
#else
void create_sql(strstream &rlb_string, strstream &rlp_string);
#endif
#endif

void get_epics_data(void);
void get_polar_epics_data(void);
#ifdef _RTPC_RUN_LOG_
void get_rtpc_epics_data(void);
#endif
#ifdef RUN_LOG_TPE
void get_tpe_epics_data(void);
#endif
void insert_into_ipc(void);
void insert_into_database(const char *entry);
void insert_into_datastream(const char *entry, int banknum);
void insert_into_file();
int find_tag_line(ifstream &file, const char *tag, char buffer[], int buflen);
int get_next_line(ifstream &file, char buffer[], int buflen);
char *env_name(const char *env, const char *name);

char *name_only(char *x);
extern "C" {
void get_run_config(const char *msql_database, const char *session, int *run, char **config);
int tcpClientCmd(char *roc, char *cmd, char *buf);
}


// program start time
static time_t start=time(NULL);


// ipc connection
TipcSrv &server=TipcSrv::Instance();


//--------------------------------------------------------------------------


main(int argc,char **argv)
{
  strstream rlb_string, rlp_string, rlr_string;
  int ret;


  // decode command line
  decode_command_line(argc,argv);


  // no ipc or file in recovery mode
  if(filep!=0)
  {
    no_info=1;
    no_file=1;
  }

  // set session name if not specified via env variable or on command line
  if(filep==0) {
    if(strlen(session)==0) strcpy(session,"clasprod");
  }

printf("0\n");fflush(stdout);

  // normal mode
  if(filep==0)
  {

    // collect data and create sql strings
    collect_data();

#ifdef _RTPC_RUN_LOG_
    create_sql(rlb_string,rlp_string,rlr_string);
#else
#ifdef RUN_LOG_TPE
    create_sql(rlb_string,rlp_string,rlr_string);
#else
    create_sql(rlb_string,rlp_string);
#endif
#endif

    // make entries
    if(debug==0) {

      // write rlb,rlp to file
      insert_into_file();

      // connect to server
      dbr_init(uniq_subj,application,id_string);

      // insert into data stream
      insert_into_datastream(rlb_string.str(),1);
      insert_into_datastream(rlp_string.str(),4); //polar
#ifdef _RTPC_RUN_LOG_
      insert_into_datastream(rlr_string.str(),8); //rtpc
#endif
#ifdef RUN_LOG_TPE
      insert_into_datastream(rlr_string.str(),8);
#endif

      // ship to info_server
      insert_into_ipc();

      // ship to database router
      insert_into_database(rlb_string.str());
      insert_into_database(rlp_string.str()); //polar
#ifdef _RTPC_RUN_LOG_
      insert_into_database(rlr_string.str()); //rtpc
#endif
#ifdef RUN_LOG_TPE
      insert_into_database(rlr_string.str());
#endif

      // close ipc connection
      dbr_close();


    } else {

      // just print sql strings
      cout << "\nrlb for run " << run << " is:\n\n" << rlb_string.str()
	   << endl << endl;

	  // polar
      cout << "\nrlp for run " << run << " is:\n\n" << rlp_string.str()
	   << endl << endl;

#ifdef _RTPC_RUN_LOG_
	  // rtpc
      cout << "\nrlr for run " << run << " is:\n\n" << rlr_string.str()
	   << endl << endl;
#endif
#ifdef RUN_LOG_TPE
      cout << "\nrlr for run " << run << " is:\n\n" << rlr_string.str()
	   << endl << endl;
#endif
    }

  // recovery mode
  } else {

    printf("RECOVERY MODE\n");

    for(int i=filep; i<argc; i++)
    {
      // read data from text file and create sql strings
      run = atoi(argv[i]);
      printf("Trying to read archive file for run number %d (>%s<)\n",run,argv[i]);
      ret = read_from_file();
      if(ret) continue; // did not get anything

#ifdef _RTPC_RUN_LOG_
      create_sql(rlb_string,rlp_string,rlr_string);
#else
#ifdef RUN_LOG_TPE
      create_sql(rlb_string,rlp_string,rlr_string);
#else
      create_sql(rlb_string,rlp_string);
#endif
#endif

      // make entries
      if(debug==0) {

        // connect to server
        dbr_init(uniq_subj,application,id_string);
	
        // ship to database router
        insert_into_database(rlb_string.str());
        insert_into_database(rlp_string.str()); //polar
#ifdef _RTPC_RUN_LOG_
        insert_into_database(rlr_string.str()); //rtpc
#endif
#ifdef RUN_LOG_TPE
        insert_into_database(rlr_string.str());
#endif
        // close ipc connection
        dbr_close();
      }
      else
	{
	  // just print sql strings
	  cout << "\nrlb for run " << run << " is:\n\n" << rlb_string.str()
	       << endl << endl;
	  
	  // polar
	  cout << "\nrlp for run " << run << " is:\n\n" << rlp_string.str()
	       << endl << endl;
	  
#ifdef _RTPC_RUN_LOG_
        // rtpc
	  cout << "\nrlr for run " << run << " is:\n\n" << rlr_string.str()
	       << endl << endl;
#endif
#ifdef RUN_LOG_TPE
	  cout << "\nrlr for run " << run << " is:\n\n" << rlr_string.str()
	       << endl << endl;
#endif
	}
    }
  }

  // done
  exit(EXIT_SUCCESS);

}
       

//----------------------------------------------------------------


void
collect_data(void)
{
  int i,ind,flen,ret;
  char *p;
  char line[4096];
  

  // get all epics data
printf("1\n");fflush(stdout);
  get_epics_data();
printf("2\n");fflush(stdout);
  get_polar_epics_data();
printf("3\n");fflush(stdout);
#ifdef _RTPC_RUN_LOG_
  get_rtpc_epics_data();
#endif
#ifdef RUN_LOG_TPE
  get_tpe_epics_data();
#endif
  // get run number and config
  get_run_config(msql_database,session,&run,&configin);
printf("4\n");fflush(stdout);
  if(strlen(session)==0) strcpy(session,"No_session!");
  if(strlen(configin)==0) strcpy(config,"No_configuration!");
  else strcpy(config,configin);

  // form start date
  tm *tstruct = localtime(&start);

  /*sergey strftime(startdate, 25, "%d-%b-%Y %H:%M", tstruct);*/
  strftime(startdate,25,"%Y-%m-%d %H:%M:%S",tstruct);
  

  // get l1 program file name
  ifstream l1prog(env_name("CLON_PARMS",current_l1prog_name));
  if(l1prog.is_open()) {
    l1prog.getline(l1_prog,sizeof(l1_prog));
    l1prog.close();
  }
  
  
  // get ts file name
  ifstream tsfile(env_name("CLON_PARMS",current_tsfile_name));
  if(tsfile.is_open()) {
    tsfile.getline(ts_file,sizeof(ts_file));
    tsfile.close();
  }


  // get trigger config file name
  ifstream trigfile(env_name("CLON_PARMS",current_trigger_name));
  if(trigfile.is_open()) {
    trigfile.getline(trigger_config,sizeof(trigger_config));
    trigfile.close();
  }


  // get channel config file name
  ifstream chanfile(env_name("CLON_PARMS",current_channel_name));
  if(chanfile.is_open()) {
    chanfile.getline(channel_config,sizeof(channel_config));
    chanfile.close();
  }


  // get TS register data
  ret=tcpClientCmd((char*)"clastrig2",(char*)"ts_reg()",(char*)line);
  if(ret<0)
  {
    printf("tcpClientCmd returns %d - skip clastrig2 request\n",ret);
  }
  else
  {
    p=line;
    for(i=0; i<128; i++)
    {
      p=strchr(p,'x')+1;
      sscanf(p,"%x %n",&ts_reg[i],&ind);
	  //printf("[%3d] 0x%08x %d\n",i,ts_reg[i],ind);
      p+=ind;
    }
  
    ts_csr=ts_reg[0];
    ts_control=ts_reg[1]&0xffff;
    ts_roc_enable=ts_reg[2];
    ts_synch=ts_reg[3]&0xffff;
    for(i=0; i<4; i++) ts_timer[i]=ts_reg[16+i]&0xffff;
    ts_timer[4]=ts_reg[20]&0xff;
    for(i=0; i<4; i++) prescale[i]=ts_reg[ 8+i]&0xffffff;
    for(i=4; i<8; i++) prescale[i]=ts_reg[ 8+i]&0xffff;
  }

 
  // read sparsification threshold links 
  flen=readlink(env_name("CLON_PARMS",sc_spar_name),sc_spar,sizeof(sc_spar));
  sc_spar[flen]=NULL;
  flen=readlink(env_name("CLON_PARMS",cc_spar_name),cc_spar,sizeof(cc_spar));
  cc_spar[flen]=NULL;
  flen=readlink(env_name("CLON_PARMS",ec1_spar_name),ec1_spar,sizeof(ec1_spar));
  ec1_spar[flen]=NULL;
  flen=readlink(env_name("CLON_PARMS",ec2_spar_name),ec2_spar,sizeof(ec2_spar));
  ec2_spar[flen]=NULL;
  flen=readlink(env_name("CLON_PARMS",lac_spar_name),lac_spar,sizeof(lac_spar));
  lac_spar[flen]=NULL;
  
  
  // read pretrig summary data
  sprintf(filename,pretrig_file_name,session,run);
  ifstream pretrigfile(env_name("CLON_PARMS",filename));
  if(pretrigfile.is_open()) {
    
    if(find_tag_line(pretrigfile,"*EC_Mean*",line,sizeof(line))==0) {
      if(get_next_line(pretrigfile,line,sizeof(line))==0) {
	istrstream ec(line,sizeof(line));
	ec >> ec_inner_hi >> ec_outer_hi >> ec_total_hi >> ec_inner_lo >> ec_outer_lo >> ec_total_lo;
      }
    }
      
    if(find_tag_line(pretrigfile,"*CC_Mean*",line,sizeof(line))==0) {
      if(get_next_line(pretrigfile,line,sizeof(line))==0) {
	istrstream cc(line,sizeof(line));
	cc >> cc_hi >> cc_lo;
      }
    }
    
    if(find_tag_line(pretrigfile,"*SC_MeanThr*",line,sizeof(line))==0) {
      if(get_next_line(pretrigfile,line,sizeof(line))==0) {
	sscanf(line,"%d",&sc_thresh);
      }
    }
    
    if(find_tag_line(pretrigfile,"*SC_MeanWid*",line,sizeof(line))==0) {
      if(get_next_line(pretrigfile,line,sizeof(line))==0) {
	sscanf(line,"%d",&sc_width);
      }
    }
    
    pretrigfile.close();
  } 
 
  return;
}

//----------------------------------------------------------------


int
read_from_file(void)
{
  int ii,ind,flen,ret;
  char *p, date1[30], time1[30];
  char line[4096];

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

  if(find_tag_line(file,"*RUN*",line,sizeof(line))==0)
  {
    if(get_next_line(file,line,sizeof(line))==0)
    sscanf(line,"%d",&run);
  }

  if(find_tag_line(file,"*STARTDATE*",line,sizeof(line))==0)
  {
    if(get_next_line(file,line,sizeof(line))==0)
    sscanf(line,"%s %s",date1,time1);
    strcpy(startdate,date1);
    strcat(startdate," ");
    strcat(startdate,time1);
  }

  if(find_tag_line(file,"*SESSION*",line,sizeof(line))==0)
  {
    if(get_next_line(file,line,sizeof(line))==0)
    sscanf(line,"%s",session);
  }

  if(find_tag_line(file,"*CONFIG*",line,sizeof(line))==0)
  {
    if(get_next_line(file,line,sizeof(line))==0)
      sscanf(line,"%s",config);
    else
      strcpy(config,"");
  }

  if(find_tag_line(file,"*TRIGGER_CONFIG*",line,sizeof(line))==0)
  {
    if(get_next_line(file,line,sizeof(line))==0)
    sscanf(line,"%s",trigger_config);
  }

  if(find_tag_line(file,"*CHANNEL_CONFIG*",line,sizeof(line))==0)
  {
    if(get_next_line(file,line,sizeof(line))==0)
    sscanf(line,"%s",channel_config);
  }

  if(find_tag_line(file,"*TS_FILE*",line,sizeof(line))==0)
  {
    if(get_next_line(file,line,sizeof(line))==0)
    sscanf(line,"%s",ts_file);
  }

  if(find_tag_line(file,"*L1_PROG*",line,sizeof(line))==0)
  {
    if(get_next_line(file,line,sizeof(line))==0)
    sscanf(line,"%s",l1_prog);
  }

  if(find_tag_line(file,"*SCSPAR*",line,sizeof(line))==0)
  {
    if(get_next_line(file,line,sizeof(line))==0)
    sscanf(line,"%s",sc_spar);
  }

  if(find_tag_line(file,"*CCSPAR*",line,sizeof(line))==0)
  {
    if(get_next_line(file,line,sizeof(line))==0)
    sscanf(line,"%s",cc_spar);
  }

  if(find_tag_line(file,"*EC1SPAR*",line,sizeof(line))==0)
  {
    if(get_next_line(file,line,sizeof(line))==0)
    sscanf(line,"%s",ec1_spar);
  }

  if(find_tag_line(file,"*EC2SPAR*",line,sizeof(line))==0)
  {
    if(get_next_line(file,line,sizeof(line))==0)
    sscanf(line,"%s",ec2_spar);
  }

  if(find_tag_line(file,"*LACSPAR*",line,sizeof(line))==0)
  {
    if(get_next_line(file,line,sizeof(line))==0)
    sscanf(line,"%s",lac_spar);
  }
    
  if(find_tag_line(file,"*ECDISCR*",line,sizeof(line))==0)
  {
    if(get_next_line(file,line,sizeof(line))==0)
    sscanf(line,"%d %d %d %d %d %d",
      &ec_inner_hi,&ec_inner_lo,&ec_outer_hi,&ec_outer_lo,&ec_total_hi,&ec_total_lo);
  }
    
  if(find_tag_line(file,"*CCDISCR*",line,sizeof(line))==0)
  {
    if(get_next_line(file,line,sizeof(line))==0)
    sscanf(line,"%d %d",&cc_hi,&cc_lo);
  }

  if(find_tag_line(file,"*SCDISCR*",line,sizeof(line))==0)
  {
    if(get_next_line(file,line,sizeof(line))==0)
    sscanf(line,"%d %d",&sc_thresh,&sc_width);
  }

  if(find_tag_line(file,"*TS_REG*",line,sizeof(line))==0)
  {
    if(get_next_line(file,line,sizeof(line))==0)
    sscanf(line,"%x %x %x %x",&ts_csr,&ts_control,&ts_roc_enable,&ts_synch);
  }

  if(find_tag_line(file,"*TS_TIMERS*",line,sizeof(line))==0)
  {
    if(get_next_line(file,line,sizeof(line))==0)
    sscanf(line,"%d %d %d %d %d",
    &ts_timer[0],&ts_timer[1],&ts_timer[2],&ts_timer[3],&ts_timer[4]);
  }

  if(find_tag_line(file,"*TS_PRESCALE*",line,sizeof(line))==0)
  {
    if(get_next_line(file,line,sizeof(line))==0)
    sscanf(line,"%d %d %d %d %d %d %d %d",
    &prescale[0],&prescale[1],&prescale[2],&prescale[3],
    &prescale[4],&prescale[5],&prescale[6],&prescale[7]);
  }

  if(find_tag_line(file,"*EPICS*",line,sizeof(line))==0)
  {
    ii = 0;
    while(get_next_line(file,line,sizeof(line))==0)
	{
      sscanf(line,"%f %d",&epics_val[ii],db_name[ii]);
      ii++;
    }
	if(ii != nepics) printf("WARN: ii=%d != nepics=%d\n",ii,nepics); 
  }

  if(find_tag_line(file,"*POLAR*",line,sizeof(line))==0)
  {
    ii = 0;
    while(get_next_line(file,line,sizeof(line))==0)
	{
      sscanf(line,"%f %d",&polar_epics_val[ii],polar_db_name[ii]);
      ii++;
    }
	if(ii != npolepics) printf("WARN: ii=%d != npolepics=%d\n",ii,npolepics); 
  }

#ifdef _RTPC_RUN_LOG_
  if(find_tag_line(file,"*RTPC*",line,sizeof(line))==0)
  {
    ii = 0;
    while(get_next_line(file,line,sizeof(line))==0)
	{
	  sscanf(line,"%f %d",&rtpc_epics_val[ii],rtpc_db_name[ii]);
	  ii++;
	}
	if(ii != nrtpcepics) printf("WARN: ii=%d != nrtpcepics=%d\n",ii,nrtpcepics); 
  }
#endif
#ifdef RUN_LOG_TPE
  if(find_tag_line(file,"*TPE*",line,sizeof(line))==0)
  {
    ii = 0;
    while(get_next_line(file,line,sizeof(line))==0)
	{
	  sscanf(line,"%f %d",&rtpc_epics_val[ii],rtpc_db_name[ii]);
	  ii++;
	}
	if(ii != nrtpcepics) printf("WARN: ii=%d != nrtpcepics=%d\n",ii,nrtpcepics); 
  }
#endif



  file.close();

  return(0);
}


//--------------------------------------------------------------------------


void
get_epics_data()
{
  int i, result;

  for(i=0; i<nepics; i++)
  {
    epics_val[i]=-9999.0;
  }
  result = getepics(nepics, epics_chan, epics_val);
  /*
  printf("after: nepics=%d\n",nepics);
  for(i=0; i<nepics; i++)
  {
    printf("[%3d] name=>%30.30s< value=%13f\n",i,epics_chan[i],epics_val[i]);
  }
  */
  return;
}


//---------------------------------------------------------------------------


void
get_polar_epics_data()
{
  int i, result;

  for(i=0; i<npolepics; i++)
  {
    polar_epics_val[i]=-9999.0;
  }
  result = getepics(npolepics, polar_epics_chan, polar_epics_val);
  /*
  printf("after: npolepics=%d\n",npolepics);
  for(i=0; i<npolepics; i++)
  {
    printf("[%3d] name=>%30.30s< value=%13f\n",i,polar_epics_chan[i],polar_epics_val[i]);
  }
  */
  return;
}

//---------------------------------------------------------------------------


#ifdef _RTPC_RUN_LOG_

void
get_rtpc_epics_data()
{
  int i, result;

  for(i=0; i<nrtpcepics; i++)
  {
    rtpc_epics_val[i]=-9999.0;
  }
  result = getepics(nrtpcepics, rtpc_epics_chan, rtpc_epics_val);
  /*
  printf("after: nrtpcepics=%d\n",nrtpcepics);
  for(i=0; i<nrtpcepics; i++)
  {
    printf("[%3d] name=>%30.30s< value=%13f\n",i,rtpc_epics_chan[i],rtpc_epics_val[i]);
  }
  */
  return;
}

#endif


#ifdef RUN_LOG_TPE
void
get_tpe_epics_data()
{
  int i, result;

  for(i=0; i<nrtpcepics; i++)
  {
    rtpc_epics_val[i]=-9999.0;
  }
  result = getepics(nrtpcepics, rtpc_epics_chan, rtpc_epics_val);
  /*
  printf("after: nrtpcepics=%d\n",nrtpcepics);
  for(i=0; i<nrtpcepics; i++)
  {
    printf("[%3d] name=>%30.30s< value=%13f\n",i,rtpc_epics_chan[i],rtpc_epics_val[i]);
  }
  */
  return;
}

#endif


//---------------------------------------------------------------------------

#ifdef _RTPC_RUN_LOG_

void
create_sql(strstream &rlb, strstream &rlp, strstream &rlr)
{
  int i;
  const char *comma = ",", *prime = "'";

  // run_log_begin
  rlb << "insert into run_log_begin ("
      << "run,start_date,session_name,configuration,trigger_config,channel_config,"
      << "ts_file,l1_program,ts_csr,ts_control,ts_roc_enable,ts_synch,"
      << "ts_timer_1,ts_timer_2,ts_timer_3,ts_timer_4,ts_timer_5,"
      << "sc_spar,cc_spar,ec1_spar,ec2_spar,lac_spar,"
      << "ec_inner_hi,ec_inner_lo,ec_outer_hi,ec_outer_lo,ec_total_hi,ec_total_lo,"
      << "cc_hi,cc_lo,sc_thresh,sc_width";

  // prescale registers
  for (i=0; i<sizeof(prescale)/sizeof(int); i++){
    rlb << comma << "prescale_" << i+1;
  }
  
  // epics data
  for (i=0; i<nepics; i++)
  {
    rlb << comma << db_name[i];
  }

  rlb << ") values ("
      << run
      << comma	<< prime << startdate  		       << prime
      << comma	<< prime << session    		       << prime
      << comma	<< prime << config     		       << prime
      << comma	<< prime << name_only(trigger_config)  << prime
      << comma	<< prime << name_only(channel_config)  << prime
      << comma	<< prime << name_only(ts_file)         << prime
      << comma	<< prime << l1_prog                    << prime
      << comma << (long)ts_csr << comma << (long)ts_control 
      << comma << (long)ts_roc_enable << comma << (long)ts_synch;
  for (i=0; i<5; i++) 
  {
    rlb << comma << (long)ts_timer[i];
  }

  rlb 
      << comma << prime	<< name_only(sc_spar)   << prime 
      << comma << prime	<< name_only(cc_spar)   << prime 
      << comma << prime	<< name_only(ec1_spar)  << prime 
      << comma << prime	<< name_only(ec2_spar)  << prime 
      << comma << prime	<< name_only(lac_spar)  << prime 
      << comma << ec_inner_hi << comma << ec_inner_lo
      << comma << ec_outer_hi << comma << ec_outer_lo
      << comma << ec_total_hi << comma << ec_total_lo
      << comma << cc_hi       << comma << cc_lo
      << comma << sc_thresh   << comma << sc_width;

  for (i=0; i<sizeof(prescale)/sizeof(int); i++)
  {
    rlb << comma << prescale[i];
  }
  for (i=0; i<nepics; i++){
    rlb << comma << epics_val[i];
  }

  rlb << ")" << ends;
  

  // run_log_polar -------------------------------
  rlp << "insert into run_log_polar ("
      << "session_name,run";

  // polar epics names
  for (i=0; i<npolepics; i++){
    rlp << comma << polar_db_name[i];
  }

  rlp << ") values ("
       << prime << session << prime << comma << run;

  // polar epics data
  for (i=0; i<npolepics; i++){
    rlp << comma << polar_epics_val[i];
  }

  rlp << ")" << ends;


  // run_log_rtpc -------------------------------
  rlr << "insert into run_log_rtpc ("
      << "session_name,run";

  // rtpc epics names
  for (i=0; i<nrtpcepics; i++){
    rlr << comma << rtpc_db_name[i];
  }

  rlr << ") values ("
       << prime << session << prime << comma << run;

  // rtpc epics data
  for (i=0; i<nrtpcepics; i++){
    rlr << comma << rtpc_epics_val[i];
  }

  rlr << ")" << ends;


  return;
}


//--------------------------------------------------------------------------

#else

#ifdef RUN_LOG_TPE
void
create_sql(strstream &rlb, strstream &rlp, strstream &rlr)
{
  int i;
  const char *comma = ",", *prime = "'";

  // run_log_begin
  rlb << "insert into run_log_begin ("
      << "run,start_date,session_name,configuration,trigger_config,channel_config,"
      << "ts_file,l1_program,ts_csr,ts_control,ts_roc_enable,ts_synch,"
      << "ts_timer_1,ts_timer_2,ts_timer_3,ts_timer_4,ts_timer_5,"
      << "sc_spar,cc_spar,ec1_spar,ec2_spar,lac_spar,"
      << "ec_inner_hi,ec_inner_lo,ec_outer_hi,ec_outer_lo,ec_total_hi,ec_total_lo,"
      << "cc_hi,cc_lo,sc_thresh,sc_width";

  // prescale registers
  for (i=0; i<sizeof(prescale)/sizeof(int); i++){
    rlb << comma << "prescale_" << i+1;
  }
  
  // epics data
  for (i=0; i<nepics; i++)
  {
    rlb << comma << db_name[i];
  }

  rlb << ") values ("
      << run
      << comma	<< prime << startdate  		       << prime
      << comma	<< prime << session    		       << prime
      << comma	<< prime << config     		       << prime
      << comma	<< prime << name_only(trigger_config)  << prime
      << comma	<< prime << name_only(channel_config)  << prime
      << comma	<< prime << name_only(ts_file)         << prime
      << comma	<< prime << l1_prog                    << prime
      << comma << (long)ts_csr << comma << (long)ts_control 
      << comma << (long)ts_roc_enable << comma << (long)ts_synch;
  for (i=0; i<5; i++) 
  {
    rlb << comma << (long)ts_timer[i];
  }

  rlb 
      << comma << prime	<< name_only(sc_spar)   << prime 
      << comma << prime	<< name_only(cc_spar)   << prime 
      << comma << prime	<< name_only(ec1_spar)  << prime 
      << comma << prime	<< name_only(ec2_spar)  << prime 
      << comma << prime	<< name_only(lac_spar)  << prime 
      << comma << ec_inner_hi << comma << ec_inner_lo
      << comma << ec_outer_hi << comma << ec_outer_lo
      << comma << ec_total_hi << comma << ec_total_lo
      << comma << cc_hi       << comma << cc_lo
      << comma << sc_thresh   << comma << sc_width;

  for (i=0; i<sizeof(prescale)/sizeof(int); i++)
  {
    rlb << comma << prescale[i];
  }
  for (i=0; i<nepics; i++){
    rlb << comma << epics_val[i];
  }

  rlb << ")" << ends;
  

  // run_log_polar -------------------------------
  rlp << "insert into run_log_polar ("
      << "session_name,run";

  // polar epics names
  for (i=0; i<npolepics; i++){
    rlp << comma << polar_db_name[i];
  }

  rlp << ") values ("
       << prime << session << prime << comma << run;

  // polar epics data
  for (i=0; i<npolepics; i++){
    rlp << comma << polar_epics_val[i];
  }

  rlp << ")" << ends;


  // run_log_rtpc -------------------------------
  rlr << "insert into run_log_tpe ("
      << "session_name,run";

  // rtpc epics names
  for (i=0; i<nrtpcepics; i++){
    rlr << comma << rtpc_db_name[i];
  }

  rlr << ") values ("
       << prime << session << prime << comma << run;

  // rtpc epics data
  for (i=0; i<nrtpcepics; i++){
    rlr << comma << rtpc_epics_val[i];
  }

  rlr << ")" << ends;


  return;
}

#else

void
create_sql(strstream &rlb, strstream &rlp)
{
  int i;
  const char *comma = ",", *prime = "'";

  // run_log_begin
  rlb << "insert into run_log_begin ("
      << "run,start_date,session_name,configuration,trigger_config,channel_config,"
      << "ts_file,l1_program,ts_csr,ts_control,ts_roc_enable,ts_synch,"
      << "ts_timer_1,ts_timer_2,ts_timer_3,ts_timer_4,ts_timer_5,"
      << "sc_spar,cc_spar,ec1_spar,ec2_spar,lac_spar,"
      << "ec_inner_hi,ec_inner_lo,ec_outer_hi,ec_outer_lo,ec_total_hi,ec_total_lo,"
      << "cc_hi,cc_lo,sc_thresh,sc_width";

  // prescale registers
  for (i=0; i<sizeof(prescale)/sizeof(int); i++){
    rlb << comma << "prescale_" << i+1;
  }
  
  // epics data
  for (i=0; i<nepics; i++)
  {
    rlb << comma << db_name[i];
  }

  rlb << ") values ("
      << run
      << comma	<< prime << startdate  		       << prime
      << comma	<< prime << session    		       << prime
      << comma	<< prime << config     		       << prime
      << comma	<< prime << name_only(trigger_config)  << prime
      << comma	<< prime << name_only(channel_config)  << prime
      << comma	<< prime << name_only(ts_file)         << prime
      << comma	<< prime << l1_prog                    << prime
      << comma << (long)ts_csr << comma << (long)ts_control 
      << comma << (long)ts_roc_enable << comma << (long)ts_synch;
  for (i=0; i<5; i++) 
  {
    rlb << comma << (long)ts_timer[i];
  }

  rlb 
      << comma << prime	<< name_only(sc_spar)   << prime 
      << comma << prime	<< name_only(cc_spar)   << prime 
      << comma << prime	<< name_only(ec1_spar)  << prime 
      << comma << prime	<< name_only(ec2_spar)  << prime 
      << comma << prime	<< name_only(lac_spar)  << prime 
      << comma << ec_inner_hi << comma << ec_inner_lo
      << comma << ec_outer_hi << comma << ec_outer_lo
      << comma << ec_total_hi << comma << ec_total_lo
      << comma << cc_hi       << comma << cc_lo
      << comma << sc_thresh   << comma << sc_width;

  for (i=0; i<sizeof(prescale)/sizeof(int); i++)
  {
    rlb << comma << prescale[i];
  }
  for (i=0; i<nepics; i++){
    rlb << comma << epics_val[i];
  }

  rlb << ")" << ends;
  

  // run_log_polar -------------------------------
  rlp << "insert into run_log_polar ("
      << "session_name,run";

  // polar epics names
  for (i=0; i<npolepics; i++){
    rlp << comma << polar_db_name[i];
  }

  rlp << ") values ("
       << prime << session << prime << comma << run;

  // polar epics data
  for (i=0; i<npolepics; i++){
    rlp << comma << polar_epics_val[i];
  }

  rlp << ")" << ends;

  return;
}

#endif
#endif

//--------------------------------------------------------------------------


void
insert_into_ipc(void)
{
  int i;

  if(no_info==0)
  {
	printf("IPCing ...\n");
    TipcMsg info = TipcMsg((T_STR)"info_server");
    info.Dest((T_STR)"info_server/in/run_log_begin");
    info.Sender(uniq_subj);

	//printf("IPCing: run=%d\n",run);

    info << (T_STR)"run_log_begin"
	 << (T_INT4) run << startdate << session << config 
	 << name_only(trigger_config) << name_only(channel_config)
	 << name_only(ts_file) << l1_prog
	 << (T_INT4)ts_csr << (T_INT4)ts_control << (T_INT4)ts_roc_enable << (T_INT4)ts_synch;
    for (i=0; i<5; i++) {
      info << (T_INT4)ts_timer[i];
    }
    info << name_only(sc_spar) << name_only(cc_spar) << name_only(ec1_spar) 
	 << name_only(ec2_spar) << name_only(lac_spar)
	 << (T_INT4)ec_inner_hi << (T_INT4)ec_inner_lo << (T_INT4)ec_outer_hi << (T_INT4)ec_outer_lo 
	 << (T_INT4)ec_total_hi << (T_INT4)ec_total_lo
	 << (T_INT4)cc_hi << (T_INT4)cc_lo << (T_INT4)sc_thresh << (T_INT4)sc_width;

    for (i=0; i<sizeof(prescale)/sizeof(int); i++) { info << (T_INT4)prescale[i];}
    for (i=0; i<nepics; i++) {info << (T_REAL8)epics_val[i];}
    for (i=0; i<npolepics; i++) {info << (T_REAL8)polar_epics_val[i];}
#ifdef _RTPC_RUN_LOG_
    for (i=0; i<nrtpcepics; i++) {info << (T_REAL8)rtpc_epics_val[i];}
#endif

	//printf("IPCing: run=%d\n",run);

    // send and flush
    server.Send(info);
    server.Flush();
  }

  return;
}

//----------------------------------------------------------------
  

void
insert_into_database(const char *entry)
{
  if(no_dbr==0)
  {
    // disable gmd timeout
    T_OPTION opt=TutOptionLookup((T_STR)"Server_Delivery_Timeout");
    TutOptionSetNum(opt,0.0);

    TipcMsg dbr = TipcMsg((T_STR)"dbr_request");
    dbr.Dest((T_STR)"dbrouter");
    dbr.Sender(uniq_subj);
    dbr.UserProp(0.0);
    dbr.DeliveryMode(T_IPC_DELIVERY_ALL);
    dbr << (T_INT4) 1 << (T_STR)entry;

    // send and flush, wait for gmd
    server.Send(dbr);
    server.Flush();
    dbr_check((double) gmd_time);
  }

  return;
}

//----------------------------------------------------------------
  

void
insert_into_datastream(const char *entry, int banknum)
{
  char dest[128];

  if(no_data==0)
  {

    TipcMsg msg((T_STR)"evt_bosbank");
    msg.Sender((T_STR)"run_log_begin");
    sprintf(dest,"evt_bosbank/%s",session);
    msg.Dest(dest);
    int buflen=(strlen(entry)+3)/4;
    msg << (T_STR)"RNLG" << (T_INT4)banknum << (T_STR)"(A)" << (T_INT4)1 
	<< (T_INT4)1 << (T_INT4)buflen
	<< SetSize(buflen) << (T_INT4*)entry;
    server.Send(msg);
    server.Flush();
  }

  return;
}

  
//----------------------------------------------------------------


void
insert_into_file()
{
  int i;

  if(no_file==0)
  {
    // current run file needed by logbook...temporary
    sprintf(filename,"%s/run_log/current_run_%s.txt",getenv("CLON_PARMS"),session);
    ofstream cfile(filename);
    if(!cfile.is_open()) return;
    cfile << run << " " << config << endl;
    cfile.close();
  

    // run log archive file
    sprintf(filename,archive_file_name,session,run);
    ofstream file(env_name("CLON_PARMS",filename));
    if(!file.is_open()) return;
    

    file << "\n*RUN*\n"       	 << run << endl;
    file << "\n*STARTDATE*\n" 	 << startdate << endl;
    file << "\n*SESSION*\n"   	 << session << endl;
    file << "\n*CONFIG*\n"    	 << config << endl;
    file << "\n*TRIGGER_CONFIG*\n" << name_only(trigger_config) << endl;     
    file << "\n*CHANNEL_CONFIG*\n" << name_only(channel_config) << endl;     
    file << "\n*TS_FILE*\n"   	 << name_only(ts_file) << endl;     
    file << "\n*L1_PROG*\n"   	 << l1_prog << endl;
    file << "\n*SCSPAR*\n"    	 << name_only(sc_spar) << endl;
    file << "\n*CCSPAR*\n"    	 << name_only(cc_spar) << endl;
    file << "\n*EC1SPAR*\n"   	 << name_only(ec1_spar) << endl;
    file << "\n*EC2SPAR*\n"   	 << name_only(ec2_spar) << endl;
    file << "\n*LACSPAR*\n"   	 << name_only(lac_spar) << endl;
    
    file << "\n*ECDISCR*\n" << ec_inner_hi << " " << ec_inner_lo << " " 
	 << ec_outer_hi << " " << ec_outer_lo << " " 
	 << ec_total_hi << " " << ec_total_lo << endl;
    
    file << "\n*CCDISCR*\n" << cc_hi << " " << cc_lo << endl;
    file << "\n*SCDISCR*\n" << sc_thresh << " " << sc_width << endl;;
    
    file << "\n*TS_REG*\n" << hex 
	 << "0x" << ts_csr << "  " 
	 << "0x" << ts_control << "  " 
	 << "0x" << ts_roc_enable << "  " 
	 << "0x" << ts_synch << dec << endl;
    
    file << "\n*TS_TIMERS*\n";
    for (i=0; i<5; i++) file << ts_timer[i] << " ";
    file << endl;
    
    file << "\n*TS_PRESCALE*" << endl;
    for (i=0; i<sizeof(prescale)/sizeof(int); i++) { file << prescale[i] << "  ";}
    file << endl;
    
    file << "\n*EPICS*" << endl;
    for (i=0; i<nepics; i++){ file << setw(25) << epics_val[i] << "   " << db_name[i] << endl;}
    file << endl;
    
    file << "\n*POLAR*" << endl;
    for (i=0; i<npolepics; i++) { 
      file << setw(25) << polar_epics_val[i] << "   " << polar_db_name[i] << endl;
    }
    file << endl << endl;

#ifdef _RTPC_RUN_LOG_
    file << "\n*RTPC*" << endl;
    for (i=0; i<nrtpcepics; i++) { 
      file << setw(25) << rtpc_epics_val[i] << "   " << rtpc_db_name[i] << endl;
    }
    file << endl << endl;
#endif

    file.close();
  }

  return;
}


//----------------------------------------------------------------
  

void
decode_command_line(int argc, char **argv)
{
  int i=1;
  const char *help="\nusage:\n\n  run_log_begin [-a application] [-u uniq_subj] [-i id_string]\n"
       "              [-debug] [-m msql_database]  [-s session] [-no_dbr] [-no_data]\n"
       "              [-no_info] [-no_file] [-g gmd_time] file1 file2 ... \n\n\n";

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
    else if (strncasecmp(argv[i],"-no_file",8)==0){
      no_file=1;
      i=i+1;
    }
    else if (strncasecmp(argv[i],"-a",2)==0){
      application=strdup(argv[i+1]);
      i=i+2;
    }
    else if (strncasecmp(argv[i],"-u",2)==0){
      uniq_subj=strdup(argv[i+1]);
      i=i+2;
    }
    else if (strncasecmp(argv[i],"-i",2)==0){
      id_string=strdup(argv[i+1]);
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


//---------------------------------------------------------------------


char *
name_only(char *fullname)
{
  char *p;

  p=strstr(fullname,"/parms/");
  if(p!=NULL) {
    return(p+7);
  } else {
    return(fullname);
  }
}


//---------------------------------------------------------------------

