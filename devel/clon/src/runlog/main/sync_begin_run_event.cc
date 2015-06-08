
#define SSIPC


//  sync_begin_run_event.cc
//
//  inserts synchronous begin run events into data stream, currently
//     just epics values in BREP bank
// 
//
//  ejw, 30-apr-98
// sergey 29-oct-2007 migrate from cdev to ca


// for posix
#define _POSIX_SOURCE_ 1
#define __EXTENSIONS__

#ifdef SSIPC
// for smartsockets
#include <rtworks/cxxipc.hxx>
#endif

// system stuff
#include <iostream.h>
#include <iomanip.h>
#include <sys/types.h>

#ifdef SSIPC
// for ipc
#include <clas_ipc_prototypes.h>
#endif

#include "epicsutil.h"

// control params
int run;
static char *session          = NULL;
static char *project          = NULL;
static int no_data     	      = 0;
static int debug     	      = 0;     



// epics channel names, etc.
struct epics_struct {
  const char *name;
  const char *chan;
  const char *get;
};

static char *epics_name[] = {
 "beam_energy",
 "thermionic_gun",
 "polarized_gun",
 "a_slit_position",
 "b_slit_position",
 "c_slit_position",
 "radiator_position",
 "torus_current",
 "mini_current_set",
 "mini_current",
 "mini_voltage",
 "tagger_current_set",
 "tagger_current",
 "tagger_voltage",
 "cryo_pressure",
 "cryo_temperature",
 "cryo_status",
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
 "bpm_1_x_kmf",
 "bpm_1_y_kmf",
 "bpm_1_i_kmf",
 "bpm_1_x_pho",
 "bpm_1_y_pho",
 "bpm_1_i_pho",
 "bpm_1_x_ghi",
 "bpm_1_y_ghi",
 "bpm_1_i_ghi",
 "bpm_2_x_kmf",
 "bpm_2_y_kmf",
 "bpm_2_i_kmf",
 "bpm_2_x_pho",
 "bpm_2_y_pho",
 "bpm_2_i_pho",
 "bpm_2_x_ghi",
 "bpm_2_y_ghi",
 "bpm_2_i_ghi",
 "bpm_3_x_kmf",
 "bpm_3_y_kmf",
 "bpm_3_i_kmf",
 "bpm_3_x_pho",
 "bpm_3_y_pho",
 "bpm_3_i_pho",
 "bpm_3_x_ghi",
 "bpm_3_y_ghi",
 "bpm_3_i_ghi",
 "bpm_1_x_scale",
 "bpm_1_y_scale",
 "bpm_1_i_scale",
 "bpm_2_x_scale",
 "bpm_2_y_scale",
 "bpm_2_i_scale",
 "bpm_3_x_scale",
 "bpm_3_y_scale",
 "bpm_3_i_scale",
};

static char *epics_chan[] = {
 "MBSY2C_energy",
 "IGT0I00BIASET",
 "IGL1I00DAC2",
 "SMRPOSA",
 "SMRPOSB",
 "SMRPOSC",
 "harp",
 "torus_current",
 "MTSETI",
 "MTIRBCK",
 "MTVRBCK",
 "TMSETI",
 "TMIRBCK",
 "TMVRBCK",
 "B_cryotarget_pressure",
 "B_cryotarget_temperature",
 "B_cryotarget_status",
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
 "IPM2H01.XKMF",
 "IPM2H01.YKMF",
 "IPM2H01.IKMF",
 "IPM2H01.XPHO",
 "IPM2H01.YPHO",
 "IPM2H01.IPHO",
 "IPM2H01.XGHI",
 "IPM2H01.YGHI",
 "IPM2H01.IGHI",
 "IPM2C24A.XKMF",
 "IPM2C24A.YKMF",
 "IPM2C24A.IKMF",
 "IPM2C24A.XPHO",
 "IPM2C24A.YPHO",
 "IPM2C24A.IPHO",
 "IPM2C24A.XGHI",
 "IPM2C24A.YGHI",
 "IPM2C24A.IGHI",
 "IPM2C21A.XKMF",
 "IPM2C21A.YKMF",
 "IPM2C21A.IKMF",
 "IPM2C21A.XPHO",
 "IPM2C21A.YPHO",
 "IPM2C21A.IPHO",
 "IPM2C21A.XGHI",
 "IPM2C21A.YGHI",
 "IPM2C21A.IGHI",
 "IPM2H01XSENrbstr.STR1",
 "IPM2H01YSENrbstr.STR1",
 "IBC2H01ISENrbstr.STR1",
 "IPM2C24AXSENrbstr.STR1",
 "IPM2C24AYSENrbstr.STR1",
 "IBC2C24AISENrbstr.STR1",
 "IPM2C21AXSENrbstr.STR1",
 "IPM2C21AYSENrbstr.STR1",
 "IBC2C21AISENrbstr.STR1",
};

static int ncallback = 0;
static float      epics_val[sizeof(epics_chan)/sizeof(char *)];
static int nepics  = sizeof(epics_chan)/sizeof(char *);


// for record segment header
static int nevnt  = 0;
static int nphys  = 0;
static int trig   = 0;


// constants for head bank
static int nvers  = 0;
static int type   = 100;
static int rocst  = 0;
static int evcls  = 0;
static int presc  = 0;


// prototypes
void decode_command_line(int argc, char **argv);
void get_epics_data(void);

#ifdef SSIPC
void insert_into_data();
#endif


//--------------------------------------------------------------------------

int
main(int argc,char **argv)
{
  int status;


  // decode command line
  decode_command_line(argc,argv);


  // get session and project
  if(session==NULL)session=(char*)"clasprod";
  if(project==NULL)project=(char*)"clasprod";


  // collect data
  get_epics_data();

#ifdef SSIPC
  // insert into data stream
  if((no_data==0)&&(debug==0))insert_into_data();
#endif

  // done
  exit(EXIT_SUCCESS);
}
       

//----------------------------------------------------------------


void
get_epics_data()
{
  int i, result;


  // create request objects and callbacks
  ncallback=0;
  for(i=0; i<nepics; i++)
  {
    epics_val[i]=-9999.0;
  }

  result = getepics(nepics, epics_chan, epics_val);

  if(debug==1)
  {
    cout << "Epics channel data:" << endl;
    for(i=0; i<nepics; i++) 
      cout << setw(20) << epics_name[i] << "  " << setw(12) << epics_val[i] 
	   << "       " << epics_chan[i] << endl;
  }

  return;
}


//---------------------------------------------------------------------------


#ifdef SSIPC
void
insert_into_data()
{  
  char dest[128];

  TipcSrv &server=TipcSrv::Instance();
  dbr_init("sync_begin_run_event",project,"sync_begin_run_event");


  TipcMsg msg((T_STR)"evt_bosbank");
  msg.Sender((T_STR)"sync_begin_run_event");
  sprintf(dest,(T_STR)"evt_bosbank/%s",session);
  msg.Dest(dest);
  msg << (T_STR)"BREP" << (T_INT4)2 << (T_STR)"(F)" << (T_INT4)nepics << (T_INT4)1 << (T_INT4)nepics
      << SetSize(nepics) << (T_INT4*)&epics_val[0];
  server.Send(msg);
  server.Flush();


  dbr_close();

  return;
}
#endif
  
//----------------------------------------------------------------


void
decode_command_line(int argc, char **argv)
{
  int i=1;
  const char *help="\nusage:\n\n  sync_begin_run_event [-debug] [-a project] [-s session] [-no_data] "
    " \n\n\n";


  while(i<argc) {
    
    if(strncasecmp(argv[i],"-h",2)==0){
      printf(help);
      exit(EXIT_SUCCESS);
    }
    else if (strncasecmp(argv[i],"-debug",6)==0){
      debug=1;
      i=i+1;
    }
    else if (strncasecmp(argv[i],"-no_data",8)==0){
      no_data=1;
      i=i+1;
    }
    else if (strncasecmp(argv[i],"-a",2)==0){
      project=argv[i+1];
      i=i+2;
    }
    else if (strncasecmp(argv[i],"-s",2)==0){
      session=strdup(argv[i+1]);
      i=i+2;
    }
  }
}


//---------------------------------------------------------------------














