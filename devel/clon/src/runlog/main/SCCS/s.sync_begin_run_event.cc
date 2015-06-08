h10676
s 00200/00154/00170
d D 1.15 07/10/29 10:26:32 boiarino 18 17
c migrate from cdev to ca
c 
e
s 00011/00011/00313
d D 1.14 00/12/05 17:27:57 wolin 17 16
c New CC
e
s 00001/00000/00323
d D 1.13 00/12/05 16:55:57 wolin 16 14
i 15
c Accepted child's version in workspace "/usr/local/clas/devel/source".
c 
e
s 00001/00001/00323
d D 1.11.1.2 00/12/05 15:11:52 wolin 15 13
c torus_current
e
s 00001/00001/00322
d D 1.12 00/12/05 15:16:37 wolin 14 12
c torus_current
e
s 00001/00000/00323
d D 1.11.1.1 00/09/05 14:46:49 wolin 13 12
c New CC
e
s 00015/00015/00308
d D 1.11 99/09/09 15:16:41 wolin 12 11
c IPM2C22A renamed to IPM2C21A
c 
e
s 00001/00001/00322
d D 1.10 99/05/05 15:54:06 wolin 11 10
c Increased pend to 6 sec
c 
e
s 00037/00123/00286
d D 1.9 99/03/11 13:19:13 wolin 10 9
c Uses ipcbank2et instead of dd
c 
e
s 00009/00000/00400
d D 1.8 98/09/09 14:11:40 wolin 9 8
c Added bpm constants
c 
e
s 00027/00000/00373
d D 1.7 98/09/09 13:55:13 wolin 8 7
c Added polar info
c 
e
s 00051/00032/00322
d D 1.6 98/09/09 13:42:10 wolin 7 6
c Switched to epics_struct
c 
e
s 00001/00001/00353
d D 1.5 98/05/12 11:26:05 wolin 6 5
c Epics channels
c 
e
s 00003/00003/00351
d D 1.4 98/05/07 17:19:09 clasrun 5 4
c Epics channel names
c 
e
s 00002/00002/00352
d D 1.3 98/05/07 10:45:06 wolin 4 3
c New epics channels
c 
e
s 00004/00002/00350
d D 1.2 98/04/30 11:12:31 wolin 3 1
c Added a few channels
c 
e
s 00000/00000/00000
d R 1.2 98/04/30 11:06:16 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 run_log/s/sync_begin_run_event.cc
e
s 00352/00000/00000
d D 1.1 98/04/30 11:06:15 wolin 1 0
c 
e
u
U
f e 0
t
T
I 18

#define SSIPC


E 18
I 1
//  sync_begin_run_event.cc
//
D 3
//  inserts synchronous begin run events into data stream

E 3
I 3
//  inserts synchronous begin run events into data stream, currently
//     just epics values in BREP bank
// 
//
E 3
//  ejw, 30-apr-98
I 18
// sergey 29-oct-2007 migrate from cdev to ca
E 18


// for posix
#define _POSIX_SOURCE_ 1
#define __EXTENSIONS__

D 18

E 18
I 18
#ifdef SSIPC
E 18
D 10
// DD buffer size
#define DDBUFFERSIZE  4000     // longwords
E 10
I 10
// for smartsockets
#include <rtworks/cxxipc.hxx>
I 18
#endif
E 18
E 10

D 18

E 18
// system stuff
I 16
#include <iostream.h>
E 16
I 13
#include <iostream.h>
E 13
#include <iomanip.h>
#include <sys/types.h>

D 18

E 18
I 18
#ifdef SSIPC
E 18
I 10
// for ipc
#include <clas_ipc_prototypes.h>
I 18
#endif
E 18

I 18
#include "epicsutil.h"
E 18

D 18

E 10
// for cdev
#include <cdev.h>
#include <cdevData.h>
#include <cdevDevice.h>
#include <cdevRequestObject.h>
#include <cdevSystem.h>


E 18
D 10
// for DD 
extern "C"{
#include <dd_user.h>
}


// DD event control words
static int ddctl[4] = {0,0,0,0};


E 10
// control params
int run;
D 10
static char *session          =  getenv("DD_NAME");
static char *msql_database    = "clasrun";
E 10
I 10
static char *session          = NULL;
static char *project          = NULL;
E 10
D 11
static int cdev_pend_time     = 2;
E 11
I 11
D 18
static int cdev_pend_time     = 6;
E 18
E 11
D 10
static int no_dd     	      = 0;
E 10
I 10
static int no_data     	      = 0;
E 10
static int debug     	      = 0;     



D 7
// epics data
static char *db_name[] = {
  "beam_energy", "thermionic_gun","polarized_gun",
  "a_slit_position", "b_slit_position", "c_slit_position", "radiator_position",
  "torus_current", "mini_current_set", "mini_current", "mini_voltage", 
  "tagger_current_set", "tagger_current", "tagger_voltage",
  "cryo_pressure", "cryo_temperature", "cryo_status",
  "upstream_beam_vac","target_vac",
  "halo_up_up", "halo_up_down", "halo_up_left", "halo_up_right", 
  "halo_down_up", "halo_down_down", "halo_down_left", "halo_down_right", 
  "bpm_1_x", "bpm_1_y", "bpm_1_i",
  "bpm_2_x", "bpm_2_y", "bpm_2_i", 
  "bpm_3_x", "bpm_3_y", "bpm_3_i",
E 7
I 7
// epics channel names, etc.
struct epics_struct {
D 17
  char *name;
  char *chan;
  char *get;
E 17
I 17
  const char *name;
  const char *chan;
  const char *get;
E 17
E 7
};
D 7
static char *epics_chan[] = {
D 4
  "MBSY2C_energy", "IGT0I00BIASET", "unknown",
E 4
I 4
  "MBSY2C_energy", "IGT0I00BIASET", "IGL1I00DAC2",
E 4
D 5
  "SMRPOSA", "SMRPOSB", "SMRPOSC", "unknown",
  "hallb_sf_xy560_0_5", "MTSETI", "MTIRDBK", "MTVRBCK", 
  "TMSETI", "TMIRDBK", "TMVRBCK",
E 5
I 5
D 6
  "SMRPOSA", "SMRPOSB", "SMRPOSC", "harp.DRBV",
E 6
I 6
  "SMRPOSA", "SMRPOSB", "SMRPOSC", "harp",
E 6
  "hallb_sf_xy560_0_5", "MTSETI", "MTIRBCK", "MTVRBCK", 
  "TMSETI", "TMIRBCK", "TMVRBCK",
E 5
  "B_cryotarget_pressure", "B_cryotarget_temperature", "B_cryotarget_status",
D 4
  "unknown","unknown",
E 4
I 4
  "VCG2C24","VCG2H01",
E 4
  "scalerS2o", "scalerS3o", "scalerS4o", "scalerS5o", 
  "scalerS6o", "scalerS7o", "scalerS8o", "scalerS9o", 
  "IPM2H01.XPOS","IPM2H01.YPOS","IPM2H01",
  "IPM2C24A.XPOS","IPM2C24A.YPOS","IPM2C24A",
  "IPM2C22A.XPOS","IPM2C22A.YPOS","IPM2C22A",
E 7
I 7

D 18
static epics_struct epics[] = {
"beam_energy",            "MBSY2C_energy",              "get",
"thermionic_gun",	  "IGT0I00BIASET",              "get",
"polarized_gun",	  "IGL1I00DAC2",                "get",
"a_slit_position",	  "SMRPOSA", 			"get",
"b_slit_position",	  "SMRPOSB",                    "get",
"c_slit_position",	  "SMRPOSC", 			"get",
"radiator_position",	  "harp", 			"get",
D 14
D 15
"torus_current",	  "hallb_sf_xy560_0_5", 	"get",
E 14
I 14
"torus_current",	  "torus_current", 	        "get",
E 14
E 15
I 15
"torus_current",	  "torus_current",      	"get",
E 15
"mini_current_set",	  "MTSETI",                     "get",
"mini_current",		  "MTIRBCK", 			"get",
"mini_voltage", 	  "MTVRBCK", 			"get",
"tagger_current_set",	  "TMSETI", 			"get",
"tagger_current",	  "TMIRBCK", 			"get",
"tagger_voltage",	  "TMVRBCK", 			"get",
"cryo_pressure",	  "B_cryotarget_pressure", 	"get",
"cryo_temperature",	  "B_cryotarget_temperature", 	"get",
"cryo_status",		  "B_cryotarget_status",        "get",
"upstream_beam_vac",	  "VCG2C24", 			"get",
"target_vac",		  "VCG2H01", 			"get",
"halo_up_up",		  "scalerS2o", 			"get",
"halo_up_down",		  "scalerS3o", 			"get",
"halo_up_left",		  "scalerS4o", 			"get",
"halo_up_right", 	  "scalerS5o", 			"get",
"halo_down_up",		  "scalerS6o", 			"get",
"halo_down_down",	  "scalerS7o", 			"get",
"halo_down_left",	  "scalerS8o", 			"get",
"halo_down_right",	  "scalerS9o", 			"get",
"bpm_1_x",		  "IPM2H01",   			"get XPOS",
"bpm_1_y",		  "IPM2H01", 			"get YPOS",
"bpm_1_i",		  "IPM2H01", 			"get",
"bpm_2_x",		  "IPM2C24A", 			"get XPOS",
"bpm_2_y",		  "IPM2C24A", 			"get YPOS",
"bpm_2_i", 		  "IPM2C24A",                   "get",
D 12
"bpm_3_x",		  "IPM2C22A", 			"get XPOS",
"bpm_3_y",		  "IPM2C22A", 			"get YPOS",
"bpm_3_i",		  "IPM2C22A", 			"get",
E 12
I 12
"bpm_3_x",		  "IPM2C21A", 			"get XPOS",
"bpm_3_y",		  "IPM2C21A", 			"get YPOS",
"bpm_3_i",		  "IPM2C21A", 			"get",
E 12
I 8
"bpm_1_x_kmf",		  "IPM2H01", 			"get XKMF",
"bpm_1_y_kmf",		  "IPM2H01", 			"get YKMF",
"bpm_1_i_kmf",		  "IPM2H01", 			"get IKMF",
"bpm_1_x_pho",		  "IPM2H01", 			"get XPHO",
"bpm_1_y_pho",		  "IPM2H01", 			"get YPHO",
"bpm_1_i_pho",		  "IPM2H01", 			"get IPHO",
"bpm_1_x_ghi",		  "IPM2H01", 			"get XGHI",
"bpm_1_y_ghi",		  "IPM2H01", 			"get YGHI",
"bpm_1_i_ghi",		  "IPM2H01", 			"get IGHI",
"bpm_2_x_kmf",		  "IPM2C24A", 			"get XKMF",
"bpm_2_y_kmf",		  "IPM2C24A", 			"get YKMF",
"bpm_2_i_kmf",		  "IPM2C24A", 			"get IKMF",
"bpm_2_x_pho",		  "IPM2C24A", 			"get XPHO",
"bpm_2_y_pho",		  "IPM2C24A", 			"get YPHO",
"bpm_2_i_pho",		  "IPM2C24A", 			"get IPHO",
"bpm_2_x_ghi",		  "IPM2C24A", 			"get XGHI",
"bpm_2_y_ghi",		  "IPM2C24A", 			"get YGHI",
"bpm_2_i_ghi",		  "IPM2C24A", 			"get IGHI",
D 12
"bpm_3_x_kmf",		  "IPM2C22A", 			"get XKMF",
"bpm_3_y_kmf",		  "IPM2C22A", 			"get YKMF",
"bpm_3_i_kmf",		  "IPM2C22A", 			"get IKMF",
"bpm_3_x_pho",		  "IPM2C22A", 			"get XPHO",
"bpm_3_y_pho",		  "IPM2C22A", 			"get YPHO",
"bpm_3_i_pho",		  "IPM2C22A", 			"get IPHO",
"bpm_3_x_ghi",		  "IPM2C22A", 			"get XGHI",
"bpm_3_y_ghi",		  "IPM2C22A", 			"get YGHI",
"bpm_3_i_ghi",		  "IPM2C22A", 			"get IGHI",
E 12
I 12
"bpm_3_x_kmf",		  "IPM2C21A", 			"get XKMF",
"bpm_3_y_kmf",		  "IPM2C21A", 			"get YKMF",
"bpm_3_i_kmf",		  "IPM2C21A", 			"get IKMF",
"bpm_3_x_pho",		  "IPM2C21A", 			"get XPHO",
"bpm_3_y_pho",		  "IPM2C21A", 			"get YPHO",
"bpm_3_i_pho",		  "IPM2C21A", 			"get IPHO",
"bpm_3_x_ghi",		  "IPM2C21A", 			"get XGHI",
"bpm_3_y_ghi",		  "IPM2C21A", 			"get YGHI",
"bpm_3_i_ghi",		  "IPM2C21A", 			"get IGHI",
E 12
I 9
"bpm_1_x_scale",	  "IPM2H01XSENrbstr",		"get STR1",
"bpm_1_y_scale",	  "IPM2H01YSENrbstr",		"get STR1",
"bpm_1_i_scale",	  "IBC2H01ISENrbstr",		"get STR1",
"bpm_2_x_scale",	  "IPM2C24AXSENrbstr",		"get STR1",
"bpm_2_y_scale",	  "IPM2C24AYSENrbstr",		"get STR1",
"bpm_2_i_scale",	  "IBC2C24AISENrbstr",		"get STR1",
D 12
"bpm_3_x_scale",	  "IPM2C22AXSENrbstr",		"get STR1",
"bpm_3_y_scale",	  "IPM2C22AYSENrbstr",		"get STR1",
"bpm_3_i_scale",	  "IBC2C22AISENrbstr",		"get STR1",
E 12
I 12
"bpm_3_x_scale",	  "IPM2C21AXSENrbstr",		"get STR1",
"bpm_3_y_scale",	  "IPM2C21AYSENrbstr",		"get STR1",
"bpm_3_i_scale",	  "IBC2C21AISENrbstr",		"get STR1",
E 18
I 18
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
E 18
E 12
E 9
E 8
E 7
};
D 7
static float epics_val[sizeof(epics_chan)/sizeof(char *)];
E 7
I 7

I 18
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

E 18
E 7
static int ncallback = 0;
D 7
static int nepics    = sizeof(epics_chan)/sizeof(char *);
cdevRequestObject *obj[sizeof(epics_chan)/sizeof(char *)];
cdevCallback *cb[sizeof(epics_chan)/sizeof(char *)];
E 7
I 7
D 18
static int nepics    = sizeof(epics)/sizeof(epics_struct);
static float epics_val[sizeof(epics)/sizeof(epics_struct)];
cdevRequestObject *obj[sizeof(epics)/sizeof(epics_struct)];
cdevCallback       *cb[sizeof(epics)/sizeof(epics_struct)];
E 18
I 18
static float      epics_val[sizeof(epics_chan)/sizeof(char *)];
static int nepics  = sizeof(epics_chan)/sizeof(char *);
E 18
E 7


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
I 18

#ifdef SSIPC
E 18
D 10
void insert_into_dd();
E 10
I 10
void insert_into_data();
E 10
D 18
void epics_callback_func(int status, void *userarg, cdevRequestObject &epics_req_obj,
			    cdevData& result);
E 18
I 18
#endif
E 18
D 10
extern "C" {
int create_header(int *p, int fevlen, int &banksize,
		  int name1, int name2, int run, int nevnt, int nphys, int trig);
int add_bank(int *p2fev, int fevlen, 
      char *name, int num, char *format, int ncol, int nrow, int ndata, int &banksize, int *data);
int va_add_bank(int *p2fev, int fevlen, 
      char *name, int num, char *format, int ncol, int nrow, int ndata, int &banksize, ...);
int get_run_number(char *msql_database, char *session);
}


// program start time
static time_t start=time(NULL);

E 10

D 18
// ref to cdev system object
cdevSystem &cdevsys = cdevSystem::defaultSystem ();
E 18

D 18

E 18
//--------------------------------------------------------------------------

D 18

main(int argc,char **argv){

E 18
I 18
int
main(int argc,char **argv)
{
E 18
  int status;


  // decode command line
  decode_command_line(argc,argv);


D 10
  // set session name if not specified via env variable or on command line
E 10
I 10
  // get session and project
E 10
D 17
  if(session==NULL)session="clasprod";
I 10
  if(project==NULL)project="clasprod";
E 17
I 17
  if(session==NULL)session=(char*)"clasprod";
  if(project==NULL)project=(char*)"clasprod";
E 17
E 10


D 18
  // only print cdev error messages
  cdevsys.setThreshold(CDEV_SEVERITY_ERROR);


E 18
  // collect data
D 10
  run=get_run_number(msql_database,session);
E 10
  get_epics_data();

D 18

E 18
I 18
#ifdef SSIPC
E 18
D 10
  // insert into DD system
  if((no_dd==0)&&(debug==0))insert_into_dd();
E 10
I 10
  // insert into data stream
  if((no_data==0)&&(debug==0))insert_into_data();
I 18
#endif
E 18
E 10

D 18

E 18
  // done
  exit(EXIT_SUCCESS);
D 18

E 18
}
       

//----------------------------------------------------------------


D 18
void get_epics_data(){
E 18
I 18
void
get_epics_data()
{
  int i, result;
E 18

D 18
  cdevGroup group;
  int i;
E 18

D 18

E 18
D 10
  // count number of callbacks received
  ncallback=0;


E 10
  // create request objects and callbacks
I 10
  ncallback=0;
E 10
D 18
  for(i=0; i<nepics; i++){
E 18
I 18
  for(i=0; i<nepics; i++)
  {
E 18
    epics_val[i]=-9999.0;
D 7
    obj[i] = cdevRequestObject::attachPtr(epics_chan[i],"get");
E 7
I 7
D 17
    obj[i] = cdevRequestObject::attachPtr(epics[i].chan,epics[i].get);
E 17
I 17
D 18
    obj[i] = cdevRequestObject::attachPtr((char*)epics[i].chan,(char*)epics[i].get);
E 17
E 7
    cb[i] = new cdevCallback(epics_callback_func,(void*)i);
E 18
  }

I 18
  result = getepics(nepics, epics_chan, epics_val);
E 18

D 18
  // create group
  group.start();
  for(i=0; i<nepics; i++){
    if(obj[i]!=NULL)obj[i]->sendCallback(NULL,*cb[i]);
  }


  // process group of callbacks
  group.pend((double)cdev_pend_time);

  
  // check if all callbacks received
  if(ncallback<nepics){
    cerr << "?only received " << ncallback << " callbacks" << " out of " << nepics << endl;
  }

  if(debug==1) {
E 18
I 18
  if(debug==1)
  {
E 18
    cout << "Epics channel data:" << endl;
    for(i=0; i<nepics; i++) 
D 7
      cout << setw(20) << db_name[i] << "  " << setw(12) << epics_val[i] 
	   << "   " << setw(20) << epics_chan[i] << endl;
E 7
I 7
D 18
      cout << setw(20) << epics[i].name << "  " << setw(12) << epics_val[i] 
	   << "       " << epics[i].chan << " (" << epics[i].get << ")"  << endl;
E 18
I 18
      cout << setw(20) << epics_name[i] << "  " << setw(12) << epics_val[i] 
	   << "       " << epics_chan[i] << endl;
E 18
E 7
  }

  return;
}


//---------------------------------------------------------------------------


D 18
void epics_callback_func(int status, void *userarg, cdevRequestObject &epics_req_obj,
			    cdevData& result){

  ncallback++;
  epics_val[(int)userarg] = (float) result;

  return;
}


//---------------------------------------------------------------------------


D 10
void insert_into_dd(){
E 10
I 10
void insert_into_data() {
E 10
  
E 18
I 18
#ifdef SSIPC
void
insert_into_data()
{  
E 18
D 10
  int status;
  int *p,i,nused,banksize,nhead,buflen;
  struct fifo_mode fmode;
  fifo_entry fev;
  int ctl[4] = {-1,-1,-1,-1};


  // connect to DD system INPUT fifo
  fmode.mode     = FMODE_ALL;
  fmode.wait     = FWAIT_SLEEP;
  fmode.suser    = FMODE_MULTI_USER;
  fmode.prescale = 1;
  fmode.p2ctl    = ctl;
  status=ddu_init("INPUT",fmode);
  if(status!=0){
    cerr << "?unable to attach to DD system INPUT fifo, status is: " << status << endl;
    return;
  }
E 10
I 10
  char dest[128];
E 10

D 10

  // get free fifo event
  status=ddu_req_fev(DDBUFFERSIZE,&fev);
  if(status!=0){
    cerr << "?unable to get fev, status is: " << status << endl;
    return;
  }
E 10
I 10
  TipcSrv &server=TipcSrv::Instance();
  dbr_init("sync_begin_run_event",project,"sync_begin_run_event");
E 10


D 10
  // set pointer, reset counts, fill ctl words, etc.
  p=fev.p2da;
  nused=0;
  nhead=0;
  fev.ctlw1=ddctl[0];
  fev.ctlb1=ddctl[1];
  fev.ctlw2=ddctl[2];
  fev.ctlb2=ddctl[3];


  // create segment header, then update pointer and counters
  status=create_header(p,DDBUFFERSIZE-nused,nhead,'RUNP','ARMS',run,nevnt,nphys,trig);
  if(status==0){
    p+=nhead;
    nused+=nhead;
  }  


  // head bank
  status=va_add_bank(p,DDBUFFERSIZE-nused,"HEAD",0,"I",8,1,8,banksize,
	      nvers,run,nevnt,start,type,rocst,evcls,presc);
  if(status==0){
    p+=banksize;
    nused+=banksize;
  }
E 10
I 10
D 17
  TipcMsg msg("evt_bosbank");
  msg.Sender("sync_begin_run_event");
  sprintf(dest,"evt_bosbank/%s",session);
E 17
I 17
  TipcMsg msg((T_STR)"evt_bosbank");
  msg.Sender((T_STR)"sync_begin_run_event");
  sprintf(dest,(T_STR)"evt_bosbank/%s",session);
E 17
  msg.Dest(dest);
D 17
  msg << "BREP" << (T_INT4)2 << "(F)" << (T_INT4)nepics << (T_INT4)1 << (T_INT4)nepics
E 17
I 17
  msg << (T_STR)"BREP" << (T_INT4)2 << (T_STR)"(F)" << (T_INT4)nepics << (T_INT4)1 << (T_INT4)nepics
E 17
      << SetSize(nepics) << (T_INT4*)&epics_val[0];
  server.Send(msg);
  server.Flush();
E 10


D 10
  // fill special begin run bank
  status=add_bank(p,DDBUFFERSIZE-nused,"BREP",2,"F",nepics,1,nepics,banksize,(int *)&epics_val[0]);
  if(status==0) {
    p+=banksize;
    nused+=banksize;
  }



  // all banks added...set fev and fpack overall word counts
  fev.len=nused;
  *(fev.p2da+10)=nused-nhead;


  // insert event into DD system
  status=ddu_put_fev(fev);
  if(status!=0){
    cerr << "?unable to put fev, status is: " << status << endl;
    return;
  }


  // disconnect from DD system
  ddu_close();

E 10
I 10
  dbr_close();
E 10

  return;
}
D 18

E 18
I 18
#endif
E 18
  
//----------------------------------------------------------------


D 10
void decode_command_line(int argc, char **argv)
{
E 10
I 10
D 18
void decode_command_line(int argc, char **argv) {
E 10

E 18
I 18
void
decode_command_line(int argc, char **argv)
{
E 18
  int i=1;
D 10
  char *help="\nusage:\n\n  sync_begin_run_event [-debug] [-m msql_database] [-s session] [-no_dd] "
E 10
I 10
D 17
  char *help="\nusage:\n\n  sync_begin_run_event [-debug] [-a project] [-s session] [-no_data] "
E 17
I 17
  const char *help="\nusage:\n\n  sync_begin_run_event [-debug] [-a project] [-s session] [-no_data] "
E 17
E 10
D 18
    " [-c cdev_pend_time]\n\n\n";
E 18
I 18
    " \n\n\n";
E 18


  while(i<argc) {
    
    if(strncasecmp(argv[i],"-h",2)==0){
      printf(help);
      exit(EXIT_SUCCESS);
    }
    else if (strncasecmp(argv[i],"-debug",6)==0){
      debug=1;
      i=i+1;
    }
D 10
    else if (strncasecmp(argv[i],"-no_dd",6)==0){
      no_dd=1;
E 10
I 10
    else if (strncasecmp(argv[i],"-no_data",8)==0){
      no_data=1;
E 10
      i=i+1;
    }
D 18
    else if (strncasecmp(argv[i],"-c",2)==0){
      cdev_pend_time=atoi(argv[i+1]);
      i=i+2;
    }
E 18
D 10
    else if (strncasecmp(argv[i],"-s",2)==0){
      session=strdup(argv[i+1]);
E 10
I 10
    else if (strncasecmp(argv[i],"-a",2)==0){
      project=argv[i+1];
E 10
      i=i+2;
    }
D 10
    else if (strncasecmp(argv[i],"-m",2)==0){
      msql_database=strdup(argv[i+1]);
E 10
I 10
    else if (strncasecmp(argv[i],"-s",2)==0){
      session=strdup(argv[i+1]);
E 10
      i=i+2;
    }
  }
}


//---------------------------------------------------------------------


I 18












E 18
E 1
