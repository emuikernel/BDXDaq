h47230
s 00003/00002/00685
d D 1.29 07/10/12 10:19:23 boiarino 30 29
c *** empty log message ***
e
s 00005/00005/00682
d D 1.28 06/10/17 16:05:59 boiarino 29 28
c EPICS3.14-related fixes
c 
e
s 00001/00001/00686
d D 1.27 03/07/11 09:53:22 wolin 28 27
c Minor
e
s 00050/00779/00637
d D 1.26 03/07/10 16:41:10 wolin 27 26
c Working
e
s 00003/00009/01413
d D 1.25 03/07/03 13:04:18 wolin 26 25
c Minor, ready for the great split
e
s 00010/00010/01412
d D 1.24 03/07/03 11:32:51 wolin 25 24
c Fixed setAlarm algorithm
e
s 00130/00117/01292
d D 1.23 03/07/03 10:13:55 wolin 24 23
c Eliminated myIndex, all fields in myPV
e
s 00055/00056/01354
d D 1.22 03/07/02 15:31:26 wolin 23 22
c Many more name changes
e
s 00147/00141/01263
d D 1.21 03/07/02 15:16:03 wolin 22 21
c Many name changes, preparing for split into lib and user code
e
s 00173/00297/01231
d D 1.20 03/07/02 14:55:48 wolin 21 20
c Split out user-defined stuff, moved fillPV and setAlarm into clasPV
e
s 00016/00002/01512
d D 1.19 03/07/01 14:56:42 wolin 20 19
c Added PREC
e
s 00003/00003/01511
d D 1.18 03/07/01 12:52:11 wolin 19 18
c Minor
e
s 00044/00011/01470
d D 1.17 03/07/01 11:37:11 wolin 18 17
c Use ER values, added DVRH,DRVL
e
s 00005/00005/01476
d D 1.16 03/06/30 14:22:52 wolin 17 16
c gdd *value inside loop, fixed alarm algorithm
e
s 00049/00028/01432
d D 1.15 03/06/30 08:17:29 wolin 16 15
c Minor
e
s 00008/00000/01452
d D 1.14 03/06/26 16:54:01 wolin 15 14
c Minor
e
s 00180/00127/01272
d D 1.13 03/06/26 16:10:03 wolin 14 13
c Everything may be working, including STAT,SEVR,etc...
e
s 00058/00005/01341
d D 1.12 03/06/26 09:28:10 wolin 13 12
c About to add clasAttrPV
e
s 00063/00051/01283
d D 1.11 03/06/25 15:50:30 wolin 12 11
c Mostly working except for multiple access
e
s 00200/00017/01134
d D 1.10 03/06/25 13:16:08 wolin 11 10
c Read/write on attributes working
e
s 00003/00003/01148
d D 1.9 03/06/24 14:14:01 wolin 10 9
c Typo clasUpdate in fillPV
e
s 00002/00002/01149
d D 1.8 03/06/23 12:12:07 wolin 9 8
c Mostly working
e
s 00258/00049/00893
d D 1.7 03/06/17 12:20:03 wolin 8 7
c Everything working, maybe...
e
s 00063/00022/00879
d D 1.6 03/06/16 16:09:59 wolin 7 6
c Smartsockets message working
e
s 00140/00061/00761
d D 1.5 03/06/12 16:33:39 wolin 6 5
c Almost all working, need to finish gathering data
e
s 00016/00017/00806
d D 1.4 03/06/12 11:15:04 wolin 5 4
c All working except gdd objects
e
s 00114/00191/00709
d D 1.3 03/06/10 16:53:16 wolin 4 3
c Getting there...
e
s 00552/00082/00348
d D 1.2 03/06/06 15:17:19 wolin 3 1
c Not quite there yet...
e
s 00000/00000/00000
d R 1.2 03/04/10 14:50:15 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 epicsutil/s/epics_server.cc
e
s 00430/00000/00000
d D 1.1 03/04/10 14:50:14 wolin 1 0
c 
e
u
U
f b 
f e 0
t
T
I 1
D 27
//  epics_server
//
D 3
//  serves software-generated values as epics channels using portable ca server
E 3
I 3
D 8
//  CLAS portable ca server serves assorted DAQ/Online info as epics channels 
E 8
I 8
D 22
//  CLAS portable ca server cross-posts assorted DAQ/Online info 
E 22
I 22
// CLAS portable ca server cross-posts assorted DAQ/Online info 
E 22
//   from ipc and files as epics channels 
E 27
I 27
//  epics_server.cc
E 27
E 8
E 3

I 27
// pv classes for creating a PCAS
E 27
I 16

I 27

E 27
E 16
I 4
D 12
// notes
E 12
I 12
// notes:
I 16

E 16
E 12
D 22
//   PCAS probably not thread safe so need clasUpdate flags
E 22
I 22
D 24
//   PCAS probably not thread safe so need pvUpdate flags
E 24
I 24
//   PCAS probably not thread safe so need myUpdate flags
E 24
E 22
I 11
D 12
//   only allows writes to alarm variables: 
E 12
E 11

I 12
D 16
//   only allows writes to alarm variables: 
E 12
I 11
//  	 PCAS name:     Alarm    HI     LO     WHI    WLO
//  	 extension:     ALRM     HIHI   LOLO   HI     LO
E 16
I 16
D 24
//   uses permanent array of pv's to access data
I 22

E 22
//   creates volatile attrPV's to handle r/w to (case insensitive) fields:
E 24
I 24
//   creates attrPV's (on the fly) to handle r/w to (case insensitive) fields:
E 24
D 18
//       HIHI,HI,LOLO,LO,HOPR,LOPR,ALRM,STAT,SEVR
E 18
I 18
D 20
//       HIHI,HI,LOLO,LO,HOPR,LOPR,DRVH,DRVL,ALRM,STAT,SEVR
E 20
I 20
D 23
//       HIHI,HI,LOLO,LO,HOPR,LOPR,DRVH,DRVL,ALRM,STAT,SEVR,PREC
E 23
I 23
//       HIHI,HIGH,LOLO,LOW,HOPR,LOPR,DRVH,DRVL,ALRM,STAT,SEVR,PREC
E 23
E 20
E 18
D 24
//       (n.b. VAL request maps to regular data pv)
E 24
I 24
//       (n.b. VAL maps to regular data pv)
E 24
E 16
E 11

I 11
D 12
// implements epics attributes:  HIHI,HI,LOLO,LO,ALRM,STAT,SEVR,VAL
E 12
I 12
D 16
//   implements epics attributes:  HIHI,HI,LOLO,LO,ALRM,STAT,SEVR,VAL
E 16
I 16
//   write only allowed to following fields:
D 18
//        epics:     ALRM     HIHI   LOLO   HI     LO    HOPR   LOPR
//    PCAS name:     Alarm    HI     LO     WHI    WLO   GHI    GLO
E 18
I 18
D 23
//        epics:     ALRM     HIHI   LOLO   HIGH   LOW   HOPR   LOPR   DRVH   DRVL
//    PCAS name:     Alarm    HI     LO     WHI    WLO   GHI    GLO    CHI    CLO
E 23
I 23
//       ALRM,HIHI,LOLO,HIGH,LOW,HOPR,LOPR,DRVH,DRVL
E 23
E 18
E 16
E 12

I 12
D 16
//   all alarm limits are integers
E 16
I 16
//   all alarm limits are integers, set ALRM to 0(1) to turn alarms off(on)
E 16
E 12

I 16
//   have to set EPICS_CAS_INTF_ADDR_LIST to limit server to single network interface
E 16
I 12

I 16

E 16
E 12
E 11
E 4
D 3
//  ejw, 7-apr-2003
E 3
I 3
// to do:
I 12
D 13
//  myAttr and multiple requests???
E 13
I 13
D 14
//  myAttr and multiple requests...create pv's on the fly???
E 14
E 13
E 12
D 4
//     scaler statistics monitor callback
E 4
I 4
D 8
//     clasXXX vs myXXX
E 8
E 4
E 3

I 8
D 11
//  ejw, 17-jun-2003
E 11
E 8

I 11
D 12
//  ejw, 24-jun-2003
E 12
I 12
D 24
//  ejw, 25-jun-2003
E 24
I 24
//  ejw, 3-jul-2003
E 24
E 12
E 11
I 3
D 4
//  ejw, 6-jun-2003
E 4
I 4
D 8
//  ejw, 9-jun-2003
E 8
E 4

I 11

I 24

E 24
E 11
D 8

E 8
E 3
// for posix
#define _POSIX_SOURCE_ 1
#define __EXTENSIONS__


D 21
// for smartsockets
#include <rtworks/cxxipc.hxx>


// CLAS ipc
#include <clas_ipc_prototypes.h>


E 21
// for ca
#include <casdef.h>
I 3
#include <alarm.h>
E 3
D 27
#include <fdManager.h>
E 27
I 3
D 6
#include <gddAppFuncTable.h>
E 6
I 4
#include <gdd.h>
I 6
D 27
#include <gddApps.h>
#include <gddAppFuncTable.h>
E 27
E 6
E 4
E 3


I 21
D 24
//  needed for header files
D 22
class clasPV;
E 22
I 22
class myPV;
E 22


//  user-defined channel names, units, etc.
D 23
#include <epics_server.h>
E 23
I 23
#include <epics_server_defs.h>
E 23


I 22
//  array of pv's, update flags
static myPV *pPV[sizeof(pvNames)/sizeof(char*)];
static int pvUpdate[sizeof(pvNames)/sizeof(char*)];


E 24
E 22
D 27
// for smartsockets
#include <rtworks/cxxipc.hxx>
E 27
I 27
// for epics_server
#include <epics_server.h>
E 27


D 27
// CLAS ipc
#include <clas_ipc_prototypes.h>


E 27
E 21
// misc
D 27
#include <pthread.h>
D 3
#include <fstream.h>
E 3
I 3
#include <thread.h>
E 27
#include <iostream.h>
I 8
D 27
#include <fstream.h>
E 27
E 8
E 3
#include <iomanip.h>
I 3
#include <time.h>
#include <string.h>
E 3
#include <stdio.h>
D 27
#include <stdlib.h>
E 27
I 11
D 30
#include <macros.h>
E 30
E 11
D 3
#include <time.h>
E 3

I 30
#define MIN(a,b)  ( (a) < (b) ? (a) : (b) )
#define MAX(a,b)  ( (a) > (b) ? (a) : (b) )
E 30

D 27
// for tcl
extern "C" {
#include <tcl.h>
Tcl_Interp *tclinterp_init(void);
void tclipc_init(Tcl_Interp *interp);
}


// misc variables
D 8
static char *application       	 = (char*)"clastest";
D 5
char *session          		 = NULL;
E 5
static char *unique_id         	 = (char*)"epics_server";
static Tcl_Interp *interp;     
static char *init_tcl_script   	 = NULL;
static char *msql_database    	 = (char*)"clasrun";
I 3
D 4
static int main_sleep_time       = 1;
static int data_sleep_time       = 3;
static int ca_sleep_time         = 10;
E 4
I 4
D 6
static int ca_sleep_time         = 1;
static int data_sleep_time       = 10;
E 4
static int ipc_sleep_time        = 10;
E 6
I 6
static int ca_pend_time          = 1;
static int data_pend_time        = 1;
static int ipc_pend_time         = 1;
E 6
I 5
static int read_count            = 0;
static int callback_count        = 0;
E 5
E 3
static int done                	 = 0;
static int debug                 = 0;
E 8
I 8
static char *application       	    = (char*)"clastest";
static char *unique_id         	    = (char*)"epics_server";
static const char *ec_pretrig_file  = "diman/mondat/mon_ec_p.txt";
static const char *cc_pretrig_file  = "diman/mondat/mon_cc_p.txt";
static const char *sc_pretrig_file  = "diman/mondat/mon_sc_p.txt";
static const char *tdc_file         = "discr/archive/discr_clasprod";
static char *init_tcl_script   	    = NULL;
D 24
static char *msql_database    	    = (char*)"clasrun";
E 24
static char *clon_parms             = getenv("CLON_PARMS");
static Tcl_Interp *interp;     	    
static int ca_pend_time             = 5;
static int data_sleep_time          = 30;
static int ipc_pend_time            = 1;
static int read_count               = 0;
static int callback_count           = 0;
static int done                	    = 0;
static int debug                    = 0;
static int run_number               = 0;
E 27
I 27
//  misc variables
static int debug = 0;
E 27
E 8
D 3
static char temp[512];
E 3
I 3
static char temp[4096];
I 8
D 27
static char filename[256];
static char line[1024];
E 27
E 8
I 4
static int epicsToLocalTime  = 20*(365*24*60*60) + 5*(24*60*60) - (60*60); //?daylight savings?
E 4
E 3


I 24
D 27
//  user-defined channel names, units, etc and array of pv's
#include <epics_server_defs.h>
class myPV;
static myPV *pPV[sizeof(pvNames)/sizeof(char*)];


E 24
I 3
D 4
// epics base data
static int run_number            = 0;
static int event_count           = 0;
static int event_rate            = 0;
static double livetime           = 0.0;
static unsigned long ts_reg[128];
static unsigned long ts_csr,ts_control,ts_roc_enable,ts_synch,ts_timer[5];
static int prescale[8];
static int discr;  // channel and pretrig discr thresholds
int epicsToLocalTime  = 20*(365*24*60*60) + 5*(24*60*60) - (60*60); //?daylight savings?


E 4
E 3
// other prototypes
void decode_command_line(int argc, char **argv);
void init_tcl(void);
I 3
D 4
void scaler_callback(
		  T_IPC_CONN conn,
		  T_IPC_CONN_PROCESS_CB_DATA data,
		  T_CB_ARG arg);
E 4
I 4
void epics_server_callback(
			   T_IPC_CONN conn,
			   T_IPC_CONN_PROCESS_CB_DATA data,
			   T_CB_ARG arg);
I 6
D 8
void fillPV(int pvIndex, long l, time_t t);
I 7
void fillPV(int pvIndex, unsigned long ul, time_t t);
E 8
I 8
D 21
void fillPV(int pvIndex, int i, time_t t);
void fillPV(int pvIndex, unsigned int u, time_t t);
E 8
E 7
void fillPV(int pvIndex, double d, time_t t);
I 12
D 14
void setAlarm(int pvIndex, int ival);
E 14
I 14
void setAlarm(int pvIndex);
E 21
E 14
E 12
I 8
void get_ec_p(ifstream &f);
void get_cc_p(ifstream &f);
void get_sc_p(ifstream &f);
void get_tdc_info(ifstream &f);
int find_tag_line(ifstream &file, const char *tag, char buffer[], int buflen);
int get_next_line(ifstream &file, char buffer[], int buflen);
E 8
E 6
E 4
extern "C" {
E 3
void *ipc_thread(void *param);
I 3
void *data_thread(void *param);
E 3
D 4
void *ca_thread(void *param);
E 4
D 3
extern "C" {
E 3
void quit_callback(int sig);
void status_poll_callback(T_IPC_MSG msg);
int tcl_help(ClientData clientdata, Tcl_Interp *interp, 
D 3
		int argc, char **argv);
E 3
I 3
	       int argc, char **argv);
E 3
int tcl_quit(ClientData clientdata, Tcl_Interp *interp, 
D 3
		int argc, char **argv);
E 3
I 3
	       int argc, char **argv);
E 3
D 4
int get_run_number(const char *msql_database, const char *session);
E 4
I 3
D 24
void DP_cmd_init(char *msql_tcp_host);
D 4
void DP_cmd(char *roc, char *cmd, char *buf, int timeout);
E 4
I 4
int DP_cmd(char *roc, char *cmd, char *buf, int timeout);
E 24
E 4
E 3
int insert_msg(const char *name, const char *facility, const char *process, const char *msgclass, 
	       int severity, const char *status, int code, const char *text);
}


D 3
// ref to IPC server (connection created later)
TipcSrv &server=TipcSrv::Instance();
E 3
I 3
D 4
// ca process variable information
const char *pvnames[] = {
E 4
I 4
D 6
// pv info
E 6
I 6
// ref to IPC server (connection created later)
TipcSrv &server=TipcSrv::Instance();


E 27
D 21
// epics channel info
E 6
static const char *pvnames[] = {
E 4
D 6
  "hallb_run_number","hallb_event_count","hallb_livetime","hallb_event_rate",
  "hallb_ts_csr","hallb_run_state","hallb_trig_enable","hallb_roc_enable",
  "hallb_prescale_1","hallb_prescale_2","hallb_prescale_3","hallb_prescale_4",
  "hallb_prescale_5","hallb_prescale_6","hallb_prescale_7","hallb_prescale_8",
D 4
  "hallb_cc_thresh",
E 4
I 4
  "hallb_ec_inner_hi","hallb_ec_inner_lo","hallb_ec_outer_hi","hallb_ec_outer_lo",
  "hallb_ec_total_hi","hallb_ec_total_lo",
  "hallb_sc_hi","hallb_sc_lo","hallb_cc_hi","hallb_cc_lo",
  "hallb_ec_tdc_thresh","hallb_ec_tdc_width","hallb_sc_tdc_thresh","hallb_sc_tdc_width",
  "hallb_cc_tdc_thresh","hallb_cc_tdc_width",
E 6
I 6
  "hallb_run_number", 	 "hallb_event_count",  "hallb_event_rate",    "hallb_livetime",
D 8
  "hallb_ts_csr",     	 "hallb_run_state",    "hallb_trig_enable",   "hallb_roc_enable",
E 8
I 8
  "hallb_ts_csr",     	 "hallb_go_bit",       "hallb_trig_enable",   "hallb_roc_enable",
E 8
  "hallb_prescale_1", 	 "hallb_prescale_2",   "hallb_prescale_3",    "hallb_prescale_4",
  "hallb_prescale_5", 	 "hallb_prescale_6",   "hallb_prescale_7",    "hallb_prescale_8",
D 8
  "hallb_ec_inner_hi",	 "hallb_ec_inner_lo",  "hallb_ec_outer_hi",   "hallb_ec_outer_lo",
  "hallb_ec_total_hi",	 "hallb_ec_total_lo", 
  "hallb_sc_hi",      	 "hallb_sc_lo",        "hallb_cc_hi",         "hallb_cc_lo",
E 8
I 8
  "hallb_ec_inner_hi",	 "hallb_ec_outer_hi",  "hallb_ec_total_hi",   "hallb_ec_inner_lo",
  "hallb_ec_outer_lo",	 "hallb_ec_total_lo", 
  "hallb_cc_hi",      	 "hallb_cc_lo",        "hallb_sc_hi",         "hallb_sc_lo",
E 8
D 9
  "hallb_ec_tdc_thresh", "hallb_ec_tdc_width", "hallb_sc_tdc_thresh", "hallb_sc_tdc_width",
  "hallb_cc_tdc_thresh", "hallb_cc_tdc_width",
E 9
I 9
  "hallb_ec_tdc_thresh", "hallb_ec_tdc_width", "hallb_cc_tdc_thresh", "hallb_cc_tdc_width",
  "hallb_sc_tdc_thresh", "hallb_sc_tdc_width",
I 18
  "hallb_data_rate",
E 18
E 9
E 6
E 4
};
E 3

I 3
D 4
aitEnum clasType[sizeof(pvnames)/sizeof(char*)] = {
E 4
I 4

class clasPV;
D 14
static clasPV *pv[sizeof(pvnames)/sizeof(char*)];
E 14
I 14
static clasPV *pclasPV[sizeof(pvnames)/sizeof(char*)];
E 14
static int clasUpdate[sizeof(pvnames)/sizeof(char*)];


static aitEnum clasType[sizeof(pvnames)/sizeof(char*)] = {
E 4
D 6
  aitEnumInt32,aitEnumInt32,aitEnumFloat64,aitEnumInt32,
  aitEnumInt32,aitEnumInt32,aitEnumInt32,aitEnumInt32,
  aitEnumInt32,aitEnumInt32,aitEnumInt32,aitEnumInt32,
  aitEnumInt32,aitEnumInt32,aitEnumInt32,aitEnumInt32,
D 4
  aitEnumInt32,
E 4
I 4
  aitEnumInt32,aitEnumInt32,aitEnumInt32,aitEnumInt32,
  aitEnumInt32,aitEnumInt32,
  aitEnumInt32,aitEnumInt32,aitEnumInt32,aitEnumInt32,
  aitEnumInt32,aitEnumInt32,aitEnumInt32,aitEnumInt32,
  aitEnumInt32,aitEnumInt32,
E 6
I 6
D 13
  aitEnumInt32,	  aitEnumInt32,   aitEnumFloat64, aitEnumFloat64,
E 13
I 13
  aitEnumInt32,	  aitEnumInt32,   aitEnumInt32,   aitEnumInt32,
E 13
I 7
  aitEnumUint32,  aitEnumUint32,  aitEnumUint32,  aitEnumUint32,
E 7
  aitEnumInt32,	  aitEnumInt32,   aitEnumInt32,   aitEnumInt32,
  aitEnumInt32,	  aitEnumInt32,   aitEnumInt32,   aitEnumInt32,
  aitEnumInt32,	  aitEnumInt32,   aitEnumInt32,   aitEnumInt32,
D 7
  aitEnumInt32,	  aitEnumInt32,   aitEnumInt32,   aitEnumInt32,
E 7
  aitEnumInt32,	  aitEnumInt32, 	    
  aitEnumInt32,	  aitEnumInt32,   aitEnumInt32,   aitEnumInt32,
  aitEnumInt32,	  aitEnumInt32,   aitEnumInt32,   aitEnumInt32,
  aitEnumInt32,	  aitEnumInt32,
I 18
  aitEnumInt32,
E 18
E 6
E 4
};
E 3

D 3
// for ca
class myPV;
myPV *pv;
E 3
I 3
D 4
const char *clasUnits[] = {
E 4
I 4

static const char *clasUnits[] = {
E 4
D 6
  "run","events","fraction","evt/sec",
  "hex","pause/go","hex","hex",
D 4
  "int","int","int","int",
  "int","int","int","int",
  "channels",
E 4
I 4
  "trig","trig","trig","trig",
  "trig","trig","trig","trig",
  "count","count","count","count",
  "count","count",
  "count","count","count","count",
  "count","count","count","count",
  "count","count",
E 6
I 6
D 13
  "run",    "events",   "fraction", "evt/sec",
E 13
I 13
  "run",    "events",   "evt/sec",  "percent",
E 13
D 19
  "hex",    "pause/go", "hex", 	    "hex",
E 19
I 19
  "hex",    "off/on",   "hex", 	    "hex",
E 19
  "trig",   "trig",	"trig",	    "trig",
  "trig",   "trig",	"trig",	    "trig",
  "count",  "count",    "count",    "count",
  "count",  "count",
  "count",  "count",	"count",    "count",
  "count",  "count",	"count",    "count",
  "count",  "count",
I 18
  "MB/sec",
E 18
E 6
E 4
};
E 3

I 3
D 4
int clasHI[sizeof(pvnames)/sizeof(char*)];
int clasLO[sizeof(pvnames)/sizeof(char*)];
int clasGHI[sizeof(pvnames)/sizeof(char*)];
int clasGLO[sizeof(pvnames)/sizeof(char*)];
int clasCHI[sizeof(pvnames)/sizeof(char*)];
int clasCLO[sizeof(pvnames)/sizeof(char*)];
int clasWHI[sizeof(pvnames)/sizeof(char*)];
int clasWLO[sizeof(pvnames)/sizeof(char*)];
E 4
E 3

I 4
D 11
// needs to be filled in ???
static int clasHI[sizeof(pvnames)/sizeof(char*)];
static int clasLO[sizeof(pvnames)/sizeof(char*)];
static int clasGHI[sizeof(pvnames)/sizeof(char*)];
static int clasGLO[sizeof(pvnames)/sizeof(char*)];
static int clasCHI[sizeof(pvnames)/sizeof(char*)];
static int clasCLO[sizeof(pvnames)/sizeof(char*)];
static int clasWHI[sizeof(pvnames)/sizeof(char*)];
static int clasWLO[sizeof(pvnames)/sizeof(char*)];
E 11
I 11
// alarm info
static int clasAlarm[sizeof(pvnames)/sizeof(char*)] = {
D 19
    0,1,0,0,0,0,0,0,
E 19
    0,0,0,0,0,0,0,0,
I 19
    0,0,0,0,0,0,0,0,
E 19
    0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,
I 18
    0,
E 18
};
static int clasHI[sizeof(pvnames)/sizeof(char*)] = {
    0,100,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,
I 18
    0,
E 18
};
static int clasLO[sizeof(pvnames)/sizeof(char*)] = {
    0,5,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,
I 18
    0,
E 18
};
static int clasGHI[sizeof(pvnames)/sizeof(char*)] = {
I 16
D 19
    0,0,0,100,0,0,0,0,
E 19
I 19
    0,0,0,100,0,1,0,0,
E 19
E 16
    0,0,0,0,0,0,0,0,
D 16
    0,0,0,0,0,0,0,0,
E 16
    0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,
I 18
    0,
E 18
};
static int clasGLO[sizeof(pvnames)/sizeof(char*)] = {
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,
I 18
    0,
E 18
};
static int clasCHI[sizeof(pvnames)/sizeof(char*)] = {
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,
I 18
    0,
E 18
};
static int clasCLO[sizeof(pvnames)/sizeof(char*)] = {
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,
I 18
    0,
E 18
};
static int clasWHI[sizeof(pvnames)/sizeof(char*)] = {
    0,50,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,
I 18
    0,
E 18
};
static int clasWLO[sizeof(pvnames)/sizeof(char*)] = {
    0,10,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,
I 20
    0,0,0,0,0,0,0,0,0,0,
E 20
I 18
    0,
I 20
};
static int clasPREC[sizeof(pvnames)/sizeof(char*)] = {
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,
E 20
E 18
    0,0,0,0,0,0,0,0,0,0,
I 20
    0,
E 20
};
E 11
E 4

I 3
D 4
class clasPV;
clasPV *pv[sizeof(pvnames)/sizeof(char*)];
time_t clasTime[sizeof(pvnames)/sizeof(char*)];
int clasStat[sizeof(pvnames)/sizeof(char*)];
int clasSevr[sizeof(pvnames)/sizeof(char*)];
int monitorOn[sizeof(pvnames)/sizeof(char*)];
E 4

E 21
D 4

E 4
D 6
// ref to IPC server (connection created later)
TipcSrv &server=TipcSrv::Instance();


E 6
E 3
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------


D 3
class myPV : public casPV {
E 3
I 3
D 14
//  channel objects for setting read/write access for all channels
E 14
I 14
//  channel objects for setting read/write access for all data channels
E 14
D 22
class clasChannel : public casChannel {
E 22
I 22
class myChannel : public casChannel {
E 22
E 3

I 3
public:
E 3

I 3
D 22
  clasChannel(const casCtx &ctxIn) : casChannel(ctxIn) {
E 22
I 22
  myChannel(const casCtx &ctxIn) : casChannel(ctxIn) {
E 22
  }


//---------------------------------------------------


D 29
  aitBool readAccess() const {
E 29
I 29
  /*sergey aitBool*/bool readAccess() const {
E 29
    return aitTrue;
  }


//---------------------------------------------------


D 29
  aitBool writeAccess() const {
E 29
I 29
  /*sergey aitBool*/bool writeAccess() const {
E 29
D 11
    return aitFalse;
E 11
I 11
D 14
    return aitTrue;
E 14
I 14
    return aitFalse;
E 14
E 11
  }

};


//---------------------------------------------------------------------------
//---------------------------------------------------------------------------


D 27
//  PV objects
D 22
class clasPV : public casPV {
E 22
I 22
class myPV : public casPV {
E 27
I 27
  myPV::myPV(const char *name, aitEnum type, const char *units, int alarm, int hihi, int lolo, 
	     int high, int low, int hopr, int lopr, int drvh, int drvl, int prec) {
E 27
E 22

D 27

E 3
private:
D 3
    char *myName;
    
    
E 3
I 3
D 24
  int myIndex;
E 24
  static int ft_is_initialized;
D 22
  static gddAppFuncTable<clasPV> ft;
E 22
I 22
  static gddAppFuncTable<myPV> ft;
E 22
  
  
E 3
public:
I 4
D 24
  time_t myTime;
E 24
I 24
  char *myName;
  aitEnum myType;
  char *myUnits;
  int myAlarm;
  int myHIHI;
  int myLOLO;
  int myHIGH;
  int myLOW;
  int myHOPR;
  int myLOPR;
  int myDRVH;
  int myDRVL;
  int myPREC;
  
E 24
D 6
  gdd *myValue;
E 6
I 6
D 7
  int myIValue;
E 7
I 7
D 8
  long myLValue;
  unsigned long myULValue;
E 8
I 8
  int myIValue;
  unsigned int myUIValue;
E 8
E 7
  double myDValue;
I 24
  time_t myTime;
E 24
  int myStat;
  int mySevr;
E 6
  int myMonitor;
I 24
  int myUpdate;
E 24
I 11
D 14
  char myAttr[4];
E 14
E 11

E 4
D 3
    
    myPV(const char *name) {
	myName=strdup(name);
    }
    
E 3
I 3
  
D 22
  clasPV(int i) {
E 22
I 22
D 24
  myPV(int i) {
E 22
    myIndex=i;
E 24
I 24
  myPV(const char *name, aitEnum type, const char *units, int alarm, int hihi, int lolo, 
       int high, int low, int hopr, int lopr, int drvh, int drvl, int prec) {

E 27
    myName=strdup(name);
    myType=type;
    myUnits=strdup(units);
    myAlarm=alarm;
    myHIHI=hihi;
    myLOLO=lolo;
    myHIGH=high;
    myLOW=low;
    myHOPR=hopr;
    myLOPR=lopr;
    myDRVH=drvh;
    myDRVL=drvl;

E 24
D 4
    monitorOn[myIndex]=0;
    clasTime[myIndex]=0;
    clasStat[myIndex]=epicsAlarmNone;
    clasSevr[myIndex]=epicsSevNone;
    if(debug!=0)cout << "clasPV constructor for " << pvnames[myIndex] << endl;
E 4
I 4
    myTime=0;
D 6
    myValue = new gdd(); // ???
    myValue->setStat(epicsAlarmNone);
    myValue->setSevr(epicsSevNone);
E 6
I 6
D 7
    myIValue=0;
E 7
I 7
D 8
    myLValue=0;
    myULValue=0;
E 8
I 8
    myIValue=0;
    myUIValue=0;
E 8
E 7
    myDValue=0.0;
    myStat=epicsAlarmNone;
    mySevr=epicsSevNone;
E 6
    myMonitor=0;
I 24
    myUpdate=0;
E 24
I 6

E 6
E 4
    initFT();
I 4
D 21
    if(debug!=0)cout << "clasPV constructor for " << pvnames[myIndex] << endl;
E 21
I 21
D 22
    if(debug!=0)cout << "clasPV constructor for " << pvNames[myIndex] << endl;
E 22
I 22
D 24
    if(debug!=0)cout << "myPV constructor for " << pvNames[myIndex] << endl;
E 24
I 24
    if(debug!=0)cout << "myPV constructor for " << myName << endl;
E 24
E 22
E 21
E 4
  }
  
E 3

//---------------------------------------------------


I 21
D 27
    void fillPV(int i) {
E 27
I 27
    void myPV::fillPV(int i) {
E 27

D 26
	time_t now=time(NULL)-epicsToLocalTime;

E 26
	if(i!=myIValue) {
D 22
	    clasUpdate[myIndex]=1;
E 22
I 22
D 24
	    pvUpdate[myIndex]=1;
E 24
I 24
	    myUpdate=1;
E 24
E 22
	    myIValue=i;
D 26
	    myTime=now;
E 26
I 26
	    myTime=time(NULL)-epicsToLocalTime;
E 26
	}
	setAlarm();
    }


//------------------------------------------------------------------


D 27
    void fillPV(uint ui) {
E 27
I 27
    void myPV::fillPV(uint ui) {
E 27

D 26
	time_t now=time(NULL)-epicsToLocalTime;

E 26
	if(ui!=myUIValue) {
D 22
	    clasUpdate[myIndex]=1;
E 22
I 22
D 24
	    pvUpdate[myIndex]=1;
E 24
I 24
	    myUpdate=1;
E 24
E 22
	    myUIValue=ui;
D 26
	    myTime=now;
E 26
I 26
	    myTime=time(NULL)-epicsToLocalTime;
E 26
	}
	setAlarm();
    }


//------------------------------------------------------------------


D 27
    void fillPV(double d) {
E 27
I 27
    void myPV::fillPV(double d) {
E 27

D 26
	time_t now=time(NULL)-epicsToLocalTime;

E 26
	if(d!=myDValue) {
D 22
	    clasUpdate[myIndex]=1;
E 22
I 22
D 24
	    pvUpdate[myIndex]=1;
E 24
I 24
	    myUpdate=1;
E 24
E 22
	    myDValue=d;
D 26
	    myTime=now;
E 26
I 26
	    myTime=time(NULL)-epicsToLocalTime;
E 26
	}
	setAlarm();
    }


//------------------------------------------------------------------


D 27
    void setAlarm() {
E 27
I 27
    void myPV::setAlarm() {
E 27

	int ival;
	int oldStat = myStat;
	int oldSevr = mySevr;
	

D 22
	if(clasType[myIndex]==aitEnumInt32) {
E 22
I 22
D 24
	if(pvTypes[myIndex]==aitEnumInt32) {
E 24
I 24
	if(myType==aitEnumInt32) {
E 24
E 22
	    ival=myIValue;
	    
D 22
	} else if (clasType[myIndex]==aitEnumUint32) {
E 22
I 22
D 24
	} else if (pvTypes[myIndex]==aitEnumUint32) {
E 24
I 24
	} else if (myType==aitEnumUint32) {
E 24
E 22
	    ival=(int)myUIValue;
	    
D 22
	} else if (clasType[myIndex]==aitEnumFloat64) {
E 22
I 22
D 24
	} else if (pvTypes[myIndex]==aitEnumFloat64) {
E 24
I 24
	} else if (myType==aitEnumFloat64) {
E 24
E 22
	    ival=(int)myDValue;
	    
	} else {
D 22
	    cerr << "setalarm...unknown ait type " << clasType[myIndex] << endl;
E 22
I 22
D 24
	    cerr << "setalarm...unknown ait type " << pvTypes[myIndex] << endl;
E 24
I 24
	    cerr << "setalarm...unknown ait type for " << myName << endl;
E 24
E 22
	    return;
	}
	
	
D 22
	if(clasAlarm[myIndex]!=0) {
	    if(ival>clasHI[myIndex]) {
E 22
I 22
D 24
	if(pvAlarm[myIndex]!=0) {
D 23
	    if(ival>pvHI[myIndex]) {
E 23
I 23
	    if(ival>pvHIHI[myIndex]) {
E 24
I 24
	if(myAlarm!=0) {
D 25
	    if(ival>myHIHI) {
E 25
I 25
	    if(ival>=myHIHI) {
E 25
E 24
E 23
E 22
		myStat=epicsAlarmHiHi;
		mySevr=epicsSevMajor;
D 22
	    } else if (ival>clasWHI[myIndex]) {
E 22
I 22
D 23
	    } else if (ival>pvWHI[myIndex]) {
E 23
I 23
D 24
	    } else if (ival>pvHIGH[myIndex]) {
E 24
I 24
D 25
	    } else if (ival>myHIGH) {
E 25
I 25
	    } else if (ival<=myLOLO) {
		myStat=epicsAlarmLoLo;
		mySevr=epicsSevMajor;
	    } else if (ival>=myHIGH) {
E 25
E 24
E 23
E 22
		myStat=epicsAlarmHigh;
		mySevr=epicsSevMinor;
D 22
	    } else if (ival>=clasWLO[myIndex]) {
E 22
I 22
D 23
	    } else if (ival>=pvWLO[myIndex]) {
E 23
I 23
D 24
	    } else if (ival>=pvLOW[myIndex]) {
E 24
I 24
D 25
	    } else if (ival>=myLOW) {
E 24
E 23
E 22
		myStat=epicsAlarmNone;
		mySevr=epicsSevNone;
D 22
	    } else if (ival>=clasLO[myIndex]) {
E 22
I 22
D 23
	    } else if (ival>=pvLO[myIndex]) {
E 23
I 23
D 24
	    } else if (ival>=pvLOLO[myIndex]) {
E 24
I 24
	    } else if (ival>=myLOLO) {
E 25
I 25
	    } else if (ival<=myLOW) {
E 25
E 24
E 23
E 22
		myStat=epicsAlarmLow;
		mySevr=epicsSevMinor;
	    } else {
D 25
		myStat=epicsAlarmLoLo;
		mySevr=epicsSevMajor;
E 25
I 25
		myStat=epicsAlarmNone;
		mySevr=epicsSevNone;
E 25
	    }
D 25
	    
E 25
I 25

E 25
	} else {
	    myStat=epicsAlarmNone;
	    mySevr=epicsSevNone;
	}
	
	
	// force update if alarm state changed
D 22
	if((oldStat!=myStat)||(oldSevr!=mySevr))clasUpdate[myIndex]=1;
E 22
I 22
D 24
	if((oldStat!=myStat)||(oldSevr!=mySevr))pvUpdate[myIndex]=1;
E 24
I 24
	if((oldStat!=myStat)||(oldSevr!=mySevr))myUpdate=1;
E 24
E 22
    }


//------------------------------------------------------------------


E 21
D 3
    caStatus read(const casCtx &ctx, gdd &value) {
	value=1;
	return S_casApp_success;
E 3
I 3
D 27
  casChannel *createChannel(const casCtx &ctx,
E 27
I 27
  casChannel *myPV::createChannel(const casCtx &ctx,
E 27
			    const char * const pUserName, 
			    const char * const pHostName) {
D 22
    return new clasChannel(ctx);
E 22
I 22
    return new myChannel(ctx);
E 22
  }


//---------------------------------------------------


D 27
  caStatus read(const casCtx &ctx, gdd &prototype) {
I 5
    read_count++;
E 27
I 27
  caStatus myPV::read(const casCtx &ctx, gdd &prototype) {
E 27
E 5
    return(ft.read(*this, prototype));
}


//---------------------------------------------------


D 27
  static void initFT() {
E 27
I 27
  void myPV::initFT() {
E 27
  
    if(ft_is_initialized!=0)return;
    ft_is_initialized=1;

D 22
    ft.installReadFunc("value",    	    &clasPV::getVAL);
    ft.installReadFunc("status",   	    &clasPV::getSTAT);
    ft.installReadFunc("severity", 	    &clasPV::getSEVR);
    ft.installReadFunc("graphicHigh", 	    &clasPV::getGHI);
    ft.installReadFunc("graphicLow",  	    &clasPV::getGLO);
    ft.installReadFunc("controlHigh", 	    &clasPV::getCHI);
    ft.installReadFunc("controlLow",  	    &clasPV::getCLO);
    ft.installReadFunc("alarmHigh",   	    &clasPV::getHI);
    ft.installReadFunc("alarmLow",    	    &clasPV::getLO);
    ft.installReadFunc("alarmHighWarning",  &clasPV::getWHI);
    ft.installReadFunc("alarmLowWarning",   &clasPV::getWLO);
    ft.installReadFunc("units",       	    &clasPV::getUNITS);
    ft.installReadFunc("precision",   	    &clasPV::getPREC);
    ft.installReadFunc("enums",       	    &clasPV::getENUM);
E 22
I 22
    ft.installReadFunc("value",    	    &myPV::getVAL);
    ft.installReadFunc("status",   	    &myPV::getSTAT);
    ft.installReadFunc("severity", 	    &myPV::getSEVR);
D 23
    ft.installReadFunc("graphicHigh", 	    &myPV::getGHI);
    ft.installReadFunc("graphicLow",  	    &myPV::getGLO);
    ft.installReadFunc("controlHigh", 	    &myPV::getCHI);
    ft.installReadFunc("controlLow",  	    &myPV::getCLO);
    ft.installReadFunc("alarmHigh",   	    &myPV::getHI);
    ft.installReadFunc("alarmLow",    	    &myPV::getLO);
    ft.installReadFunc("alarmHighWarning",  &myPV::getWHI);
    ft.installReadFunc("alarmLowWarning",   &myPV::getWLO);
E 23
I 23
    ft.installReadFunc("graphicHigh", 	    &myPV::getHOPR);
    ft.installReadFunc("graphicLow",  	    &myPV::getLOPR);
    ft.installReadFunc("controlHigh", 	    &myPV::getDRVH);
    ft.installReadFunc("controlLow",  	    &myPV::getDRVL);
    ft.installReadFunc("alarmHigh",   	    &myPV::getHIHI);
    ft.installReadFunc("alarmLow",    	    &myPV::getLOLO);
    ft.installReadFunc("alarmHighWarning",  &myPV::getHIGH);
    ft.installReadFunc("alarmLowWarning",   &myPV::getLOW);
E 23
    ft.installReadFunc("units",       	    &myPV::getUNITS);
    ft.installReadFunc("precision",   	    &myPV::getPREC);
    ft.installReadFunc("enums",       	    &myPV::getENUM);
E 22
}


//--------------------------------------------------


D 27
  aitEnum bestExternalType() const {
E 27
I 27
  aitEnum myPV::bestExternalType() const {
E 27
D 22
    return(clasType[myIndex]);
E 22
I 22
D 24
    return(pvTypes[myIndex]);
E 24
I 24
    return(myType);
E 24
E 22
}


//---------------------------------------------------


D 27
  gddAppFuncTableStatus getUNITS(gdd &value) {
E 27
I 27
  gddAppFuncTableStatus myPV::getUNITS(gdd &value) {
E 27
D 21
    if(debug!=0) cout << "...clasPV getUNITS for " << pvnames[myIndex] << endl;
E 21
I 21
D 22
    if(debug!=0) cout << "...clasPV getUNITS for " << pvNames[myIndex] << endl;
E 21
    value.putConvert(clasUnits[myIndex]);
E 22
I 22
D 24
    if(debug!=0) cout << "...myPV getUNITS for " << pvNames[myIndex] << endl;
    value.putConvert(pvUnits[myIndex]);
E 24
I 24
    if(debug!=0) cout << "...myPV getUNITS for " << myName << endl;
    value.putConvert(myUnits);
E 24
E 22
    return S_casApp_success;
  }
  

//---------------------------------------------------


D 27
  gddAppFuncTableStatus getVAL(gdd &value) {
E 27
I 27
  gddAppFuncTableStatus myPV::getVAL(gdd &value) {
E 27
D 11
    if(debug!=0) cout << "...clasPV getVAL for " << pvnames[myIndex] << endl;
E 11
I 11
D 14
    if(debug!=0) cout << "...clasPV getVAL for " << pvnames[myIndex] 
		      << ", attr: \"" << myAttr << "\"" << endl;
E 14
I 14
D 21
    if(debug!=0) cout << "...clasPV getVAL for " << pvnames[myIndex] << endl;
E 21
I 21
D 22
    if(debug!=0) cout << "...clasPV getVAL for " << pvNames[myIndex] << endl;
E 22
I 22
D 24
    if(debug!=0) cout << "...myPV getVAL for " << pvNames[myIndex] << endl;
E 24
I 24
    if(debug!=0) cout << "...myPV getVAL for " << myName << endl;
E 24
E 22
E 21
E 14
E 11

I 4
D 5
    value=*myValue;
E 5
I 5
D 6
    //    value=*myValue;
    value=myIndex;
E 6
I 6
D 11
    if(clasType[myIndex]==aitEnumInt32) {
E 11
I 11
D 14

    if(strncasecmp(myAttr,"ALRM",4)==0) {
      value.putConvert(clasAlarm[myIndex]);
      return(S_casApp_success);

    } else if (strncasecmp(myAttr,"HIHI",4)==0) {
      return(getHI(value));

    } else if (strncasecmp(myAttr,"LOLO",4)==0) {
      return(getLO(value));
      
    } else if (strncasecmp(myAttr,"HI  ",4)==0) {
      return(getWHI(value));
      
    } else if (strncasecmp(myAttr,"LO  ",4)==0) {
      return(getWLO(value));
      
    } else if (strncasecmp(myAttr,"STAT",4)==0) {
      return(getSTAT(value));

    } else if (strncasecmp(myAttr,"SEVR",4)==0) {
      return(getSEVR(value));

D 12
    } else if ((strncmp(myAttr,"    ",4)!=0)&&(strncmp(myAttr,"VAL ",4)!=0)) {
E 12
I 12
    } else if ((strncasecmp(myAttr,"    ",4)!=0)&&(strncasecmp(myAttr,"VAL ",4)!=0)) {
E 12
      cerr << "No read support for attribute: \"" << myAttr << "\"" << endl;
      return(S_casApp_noSupport);

D 12
    } else if(clasType[myIndex]==aitEnumInt32) {
E 12
I 12
    } else {
	return(getRealVAL(value));
    }
  }
  

//---------------------------------------------------


  gddAppFuncTableStatus getRealVAL(gdd &value) {
    if(debug!=0) cout << "...clasPV getRealVAL for " << pvnames[myIndex] << endl;


E 14
D 22
    if(clasType[myIndex]==aitEnumInt32) {
E 22
I 22
D 24
    if(pvTypes[myIndex]==aitEnumInt32) {
E 24
I 24
    if(myType==aitEnumInt32) {
E 24
E 22
E 12
E 11
D 7
      value.putConvert(myIValue);
E 7
I 7
D 8
      value.putConvert((int)myLValue);
E 8
I 8
      value.putConvert(myIValue);
I 11

E 11
E 8
D 22
    } else if (clasType[myIndex]==aitEnumUint32) {
E 22
I 22
D 24
    } else if (pvTypes[myIndex]==aitEnumUint32) {
E 24
I 24
    } else if (myType==aitEnumUint32) {
E 24
E 22
D 8
      value.putConvert((unsigned int)myULValue);
E 8
I 8
      value.putConvert(myUIValue);
I 11

E 11
E 8
E 7
D 22
    } else if (clasType[myIndex]==aitEnumFloat64) {
E 22
I 22
D 24
    } else if (pvTypes[myIndex]==aitEnumFloat64) {
E 24
I 24
    } else if (myType==aitEnumFloat64) {
E 24
E 22
      value.putConvert(myDValue);
I 11

E 11
    } else {
D 11
      cerr << "unknown ait type " << clasType[myIndex] << endl;
E 11
I 11
D 22
      cerr << "Unknown ait type " << clasType[myIndex] << endl;
E 22
I 22
D 24
      cerr << "Unknown ait type " << pvTypes[myIndex] << endl;
E 24
I 24
      cerr << "Unknown ait type " << myType << endl;
E 24
E 22
E 11
      value.putConvert(0);
    }
E 6
E 5
E 4

I 6
    value.setStat(myStat);
    value.setSevr(mySevr);

E 6
D 4
    switch (myIndex) {
    case 0:
      value.putConvert((int)run_number);
      break;

    case 1:
      value.putConvert((int)event_count);
      break;

    case 2:
      value.putConvert((double)livetime);
      break;

    case 3:
      value.putConvert((uint)ts_csr);
      break;

    case 4:
      value.putConvert((uint)ts_csr&0x1);
      break;

    case 5:
      value.putConvert((uint)((ts_control>>1)&0xfff));
      break;

    case 6:
    case 7:
    case 8:
    case 9:
    case 10:
    case 11:
    case 12:
    case 13:
      value.putConvert((int)prescale[myIndex-6]);
      break;

    case 14:
      value.putConvert((uint)ts_roc_enable);
      break;

    default:
      value.putConvert(0);
      break;
E 3
    }
D 3
    
E 3

I 3
    value.setStat(clasStat[myIndex]);
    value.setSevr(clasSevr[myIndex]);

E 4
    struct timespec t;
D 4
    t.tv_sec = clasTime[myIndex];
E 4
I 4
    t.tv_sec = myTime;
E 4
    t.tv_nsec=0;
    value.setTimeStamp(&t);

D 4

    if(debug!=0)cout << "clasPV read for " << pvnames[myIndex] 
		     << ", val is " << (int)value << endl;

E 4
    return S_casApp_success;
  }
  

E 3
//---------------------------------------------------


D 3
    epicsShareFunc const char *getName() const {
	return myName;
E 3
I 3
D 27
  gddAppFuncTableStatus getSTAT(gdd &value) {
E 27
I 27
  gddAppFuncTableStatus myPV::getSTAT(gdd &value) {
E 27
D 21
    if(debug!=0) cout << "...clasPV getSTAT for " << pvnames[myIndex] << endl;
E 21
I 21
D 22
    if(debug!=0) cout << "...clasPV getSTAT for " << pvNames[myIndex] << endl;
E 22
I 22
D 24
    if(debug!=0) cout << "...myPV getSTAT for " << pvNames[myIndex] << endl;
E 24
I 24
    if(debug!=0) cout << "...myPV getSTAT for " << myName << endl;
E 24
E 22
E 21
D 4
    value.put(clasStat[myIndex]);
E 4
I 4
D 6
    value.put(myValue->getStat());
E 6
I 6
    value.putConvert(myStat);
E 6
E 4
    return S_casApp_success;
  }
  

//---------------------------------------------------


D 27
  gddAppFuncTableStatus getSEVR(gdd &value) {
E 27
I 27
  gddAppFuncTableStatus myPV::getSEVR(gdd &value) {
E 27
D 21
    if(debug!=0) cout << "...clasPV getSEVR for " << pvnames[myIndex] << endl;
E 21
I 21
D 22
    if(debug!=0) cout << "...clasPV getSEVR for " << pvNames[myIndex] << endl;
E 22
I 22
D 24
    if(debug!=0) cout << "...myPV getSEVR for " << pvNames[myIndex] << endl;
E 24
I 24
    if(debug!=0) cout << "...myPV getSEVR for " << myName << endl;
E 24
E 22
E 21
D 4
    value.put(clasSevr[myIndex]);
E 4
I 4
D 6
    value.put(myValue->getSevr());
E 6
I 6
    value.putConvert(mySevr);
E 6
E 4
    return S_casApp_success;
  }
  

//---------------------------------------------------


I 11
D 14
  caStatus write(const casCtx &ctx, gdd &value) {
    if(debug!=0) cout << "...clasPV write for " << pvnames[myIndex] 
		      << ", attr: \"" << myAttr << "\"" << endl;
      
    if(strncasecmp(myAttr,"ALRM",4)==0) {
	clasAlarm[myIndex]=(int)value;

    } else if (strncasecmp(myAttr,"HIHI",4)==0) {
	clasHI[myIndex]=(int)value;

    } else if (strncasecmp(myAttr,"LOLO",4)==0) {
	clasLO[myIndex]=(int)value;
      
    } else if (strncasecmp(myAttr,"HI  ",4)==0) {
	clasWHI[myIndex]=(int)value;
      
    } else if (strncasecmp(myAttr,"LO  ",4)==0) {
	clasWLO[myIndex]=(int)value;

    } else {
	cerr << "No write support for attr: \"" << myAttr << "\"" << endl;
	return S_casApp_noSupport;
    }

D 12
      return S_casApp_success;
E 12
I 12
    clasUpdate[myIndex]=1;
    return S_casApp_success;
E 12
  }
  

//---------------------------------------------------


E 14
E 11
D 23
  gddAppFuncTableStatus getHI(gdd &value) {
D 21
    if(debug!=0) cout << "...clasPV getHI for " << pvnames[myIndex] << endl;
E 21
I 21
D 22
    if(debug!=0) cout << "...clasPV getHI for " << pvNames[myIndex] << endl;
E 21
    value.putConvert(clasHI[myIndex]);
E 22
I 22
    if(debug!=0) cout << "...myPV getHI for " << pvNames[myIndex] << endl;
    value.putConvert(pvHI[myIndex]);
E 23
I 23
D 27
  gddAppFuncTableStatus getHIHI(gdd &value) {
E 27
I 27
  gddAppFuncTableStatus myPV::getHIHI(gdd &value) {
E 27
D 24
    if(debug!=0) cout << "...myPV getHIHI for " << pvNames[myIndex] << endl;
    value.putConvert(pvHIHI[myIndex]);
E 24
I 24
    if(debug!=0) cout << "...myPV getHIHI for " << myName << endl;
    value.putConvert(myHIHI);
E 24
E 23
E 22
    return S_casApp_success;
  }
  

//---------------------------------------------------


D 23
  gddAppFuncTableStatus getLO(gdd &value) {
D 21
    if(debug!=0) cout << "...clasPV getLO for " << pvnames[myIndex] << endl;
E 21
I 21
D 22
    if(debug!=0) cout << "...clasPV getLO for " << pvNames[myIndex] << endl;
E 21
    value.putConvert(clasLO[myIndex]);
E 22
I 22
    if(debug!=0) cout << "...myPV getLO for " << pvNames[myIndex] << endl;
    value.putConvert(pvLO[myIndex]);
E 23
I 23
D 27
  gddAppFuncTableStatus getLOLO(gdd &value) {
E 27
I 27
  gddAppFuncTableStatus myPV::getLOLO(gdd &value) {
E 27
D 24
    if(debug!=0) cout << "...myPV getLOLO for " << pvNames[myIndex] << endl;
    value.putConvert(pvLOLO[myIndex]);
E 24
I 24
    if(debug!=0) cout << "...myPV getLOLO for " << myName << endl;
    value.putConvert(myLOLO);
E 24
E 23
E 22
    return S_casApp_success;
  }
  

//---------------------------------------------------


D 23
  gddAppFuncTableStatus getGHI(gdd &value) {
D 21
    if(debug!=0) cout << "...clasPV getGHI for " << pvnames[myIndex] << endl;
E 21
I 21
D 22
    if(debug!=0) cout << "...clasPV getGHI for " << pvNames[myIndex] << endl;
E 21
    value.putConvert(clasGHI[myIndex]);
E 22
I 22
    if(debug!=0) cout << "...myPV getGHI for " << pvNames[myIndex] << endl;
    value.putConvert(pvGHI[myIndex]);
E 23
I 23
D 27
  gddAppFuncTableStatus getHOPR(gdd &value) {
E 27
I 27
  gddAppFuncTableStatus myPV::getHOPR(gdd &value) {
E 27
D 24
    if(debug!=0) cout << "...myPV getHOPR for " << pvNames[myIndex] << endl;
    value.putConvert(pvHOPR[myIndex]);
E 24
I 24
    if(debug!=0) cout << "...myPV getHOPR for " << myName << endl;
    value.putConvert(myHOPR);
E 24
E 23
E 22
    return S_casApp_success;
  }
  

//---------------------------------------------------


D 23
  gddAppFuncTableStatus getGLO(gdd &value) {
D 11
    if(debug!=0) cout << "...clasPV getCLO for " << pvnames[myIndex] << endl;
E 11
I 11
D 21
    if(debug!=0) cout << "...clasPV getGLO for " << pvnames[myIndex] << endl;
E 21
I 21
D 22
    if(debug!=0) cout << "...clasPV getGLO for " << pvNames[myIndex] << endl;
E 21
E 11
    value.putConvert(clasGLO[myIndex]);
E 22
I 22
    if(debug!=0) cout << "...myPV getGLO for " << pvNames[myIndex] << endl;
    value.putConvert(pvGLO[myIndex]);
E 23
I 23
D 27
  gddAppFuncTableStatus getLOPR(gdd &value) {
E 27
I 27
  gddAppFuncTableStatus myPV::getLOPR(gdd &value) {
E 27
D 24
    if(debug!=0) cout << "...myPV getLOPR for " << pvNames[myIndex] << endl;
    value.putConvert(pvLOPR[myIndex]);
E 24
I 24
    if(debug!=0) cout << "...myPV getLOPR for " << myName << endl;
    value.putConvert(myLOPR);
E 24
E 23
E 22
    return S_casApp_success;
  }
  

//---------------------------------------------------


D 23
  gddAppFuncTableStatus getCHI(gdd &value) {
D 21
    if(debug!=0) cout << "...clasPV getCHI for " << pvnames[myIndex] << endl;
E 21
I 21
D 22
    if(debug!=0) cout << "...clasPV getCHI for " << pvNames[myIndex] << endl;
E 21
    value.putConvert(clasCHI[myIndex]);
E 22
I 22
    if(debug!=0) cout << "...myPV getCHI for " << pvNames[myIndex] << endl;
    value.putConvert(pvCHI[myIndex]);
E 23
I 23
D 27
  gddAppFuncTableStatus getDRVH(gdd &value) {
E 27
I 27
  gddAppFuncTableStatus myPV::getDRVH(gdd &value) {
E 27
D 24
    if(debug!=0) cout << "...myPV getDRVH for " << pvNames[myIndex] << endl;
    value.putConvert(pvDRVH[myIndex]);
E 24
I 24
    if(debug!=0) cout << "...myPV getDRVH for " << myName << endl;
    value.putConvert(myDRVH);
E 24
E 23
E 22
    return S_casApp_success;
  }
  

//---------------------------------------------------


D 23
  gddAppFuncTableStatus getCLO(gdd &value) {
D 21
    if(debug!=0) cout << "...clasPV getCLO for " << pvnames[myIndex] << endl;
E 21
I 21
D 22
    if(debug!=0) cout << "...clasPV getCLO for " << pvNames[myIndex] << endl;
E 21
    value.putConvert(clasCLO[myIndex]);
E 22
I 22
    if(debug!=0) cout << "...myPV getCLO for " << pvNames[myIndex] << endl;
    value.putConvert(pvCLO[myIndex]);
E 23
I 23
D 27
  gddAppFuncTableStatus getDRVL(gdd &value) {
E 27
I 27
  gddAppFuncTableStatus myPV::getDRVL(gdd &value) {
E 27
D 24
    if(debug!=0) cout << "...myPV getDRVL for " << pvNames[myIndex] << endl;
    value.putConvert(pvDRVL[myIndex]);
E 24
I 24
    if(debug!=0) cout << "...myPV getDRVL for " << myName << endl;
    value.putConvert(myDRVL);
E 24
E 23
E 22
    return S_casApp_success;
  }
  

//---------------------------------------------------


D 23
  gddAppFuncTableStatus getWHI(gdd &value) {
D 21
    if(debug!=0) cout << "...clasPV getWHI for " << pvnames[myIndex] << endl;
E 21
I 21
D 22
    if(debug!=0) cout << "...clasPV getWHI for " << pvNames[myIndex] << endl;
E 21
    value.putConvert(clasWHI[myIndex]);
E 22
I 22
    if(debug!=0) cout << "...myPV getWHI for " << pvNames[myIndex] << endl;
    value.putConvert(pvWHI[myIndex]);
E 23
I 23
D 27
  gddAppFuncTableStatus getHIGH(gdd &value) {
E 27
I 27
  gddAppFuncTableStatus myPV::getHIGH(gdd &value) {
E 27
D 24
    if(debug!=0) cout << "...myPV getHIGH for " << pvNames[myIndex] << endl;
    value.putConvert(pvHIGH[myIndex]);
E 24
I 24
    if(debug!=0) cout << "...myPV getHIGH for " << myName << endl;
    value.putConvert(myHIGH);
E 24
E 23
E 22
    return S_casApp_success;
  }
  

//---------------------------------------------------


D 23
  gddAppFuncTableStatus getWLO(gdd &value) {
D 21
    if(debug!=0) cout << "...clasPV getWLO for " << pvnames[myIndex] << endl;
E 21
I 21
D 22
    if(debug!=0) cout << "...clasPV getWLO for " << pvNames[myIndex] << endl;
E 21
    value.putConvert(clasWLO[myIndex]);
E 22
I 22
    if(debug!=0) cout << "...myPV getWLO for " << pvNames[myIndex] << endl;
    value.putConvert(pvWLO[myIndex]);
E 23
I 23
D 27
  gddAppFuncTableStatus getLOW(gdd &value) {
E 27
I 27
  gddAppFuncTableStatus myPV::getLOW(gdd &value) {
E 27
D 24
    if(debug!=0) cout << "...myPV getLOW for " << pvNames[myIndex] << endl;
    value.putConvert(pvLOW[myIndex]);
E 24
I 24
    if(debug!=0) cout << "...myPV getLOW for " << myName << endl;
    value.putConvert(myLOW);
E 24
E 23
E 22
    return S_casApp_success;
  }
  

//---------------------------------------------------


D 27
  gddAppFuncTableStatus getPREC(gdd &value) {
E 27
I 27
  gddAppFuncTableStatus myPV::getPREC(gdd &value) {
E 27
D 21
    if(debug!=0) cout << "...clasPV getPREC for " << pvnames[myIndex] << endl;
E 21
I 21
D 22
    if(debug!=0) cout << "...clasPV getPREC for " << pvNames[myIndex] << endl;
E 21
D 20
    value.putConvert(1);
E 20
I 20
    value.putConvert(clasPREC[myIndex]);
E 22
I 22
D 24
    if(debug!=0) cout << "...myPV getPREC for " << pvNames[myIndex] << endl;
    value.putConvert(pvPREC[myIndex]);
E 24
I 24
    if(debug!=0) cout << "...myPV getPREC for " << myName << endl;
    value.putConvert(myPREC);
E 24
E 22
E 20
    return S_casApp_success;
  }
  

//---------------------------------------------------


D 27
  gddAppFuncTableStatus getENUM(gdd &value) {
E 27
I 27
  gddAppFuncTableStatus myPV::getENUM(gdd &value) {
E 27
D 21
    if(debug!=0) cout << "...clasPV getNUM for " << pvnames[myIndex] << endl;
E 21
I 21
D 22
    if(debug!=0) cout << "...clasPV getNUM for " << pvNames[myIndex] << endl;
E 22
I 22
D 24
    if(debug!=0) cout << "...myPV getNUM for " << pvNames[myIndex] << endl;
E 24
I 24
    if(debug!=0) cout << "...myPV getNUM for " << myName << endl;
E 24
E 22
E 21
    value.putConvert(0);
    return S_casApp_success;
  }
  

//---------------------------------------------------


D 4
  caStatus write(const casCtx &ctx, gdd &value) {
    if(debug!=0) cout << "...clasPV write value for " << pvnames[myIndex] << endl;

    // ???
    clasTime[myIndex]=time(NULL)-epicsToLocalTime;

    if(monitorOn[myIndex]!=0) {
      caServer *pCAS = getCAS();
      casEventMask select(pCAS->valueEventMask | pCAS->logEventMask);
      postEvent(select,value);
E 3
    }
    
I 3
    return S_casApp_success;
  }
  
E 3

//---------------------------------------------------


E 4
D 3
    void destroy() {
    }
E 3
I 3
D 27
  caStatus interestRegister() {
E 27
I 27
  caStatus myPV::interestRegister() {
E 27
D 4
    if(debug!=0) cout << "...clasPV interestRegister" << endl;
    monitorOn[myIndex]=1;
E 4
I 4
D 6
    if(debug!=0) cout << "...clasPV interestRegister for" << pvnames[myIndex] << endl;
E 6
I 6
D 21
    if(debug!=0) cout << "...clasPV interestRegister for " << pvnames[myIndex] << endl;
E 21
I 21
D 22
    if(debug!=0) cout << "...clasPV interestRegister for " << pvNames[myIndex] << endl;
E 22
I 22
D 24
    if(debug!=0) cout << "...myPV interestRegister for " << pvNames[myIndex] << endl;
E 24
I 24
    if(debug!=0) cout << "...myPV interestRegister for " << myName << endl;
E 24
E 22
E 21
E 6
    myMonitor=1;
E 4
    return S_casApp_success;
  }


//---------------------------------------------------


D 27
  void interestDelete() {
E 27
I 27
  void myPV::interestDelete() {
E 27
D 4
    if(debug!=0) cout << "...clasPV interestDelete" << endl;
    monitorOn[myIndex]=0;
E 4
I 4
D 6
    if(debug!=0) cout << "...clasPV interestDelete for" << pvnames[myIndex] << endl;
E 6
I 6
D 21
    if(debug!=0) cout << "...clasPV interestDelete for " << pvnames[myIndex] << endl;
E 21
I 21
D 22
    if(debug!=0) cout << "...clasPV interestDelete for " << pvNames[myIndex] << endl;
E 22
I 22
D 24
    if(debug!=0) cout << "...myPV interestDelete for " << pvNames[myIndex] << endl;
E 24
I 24
    if(debug!=0) cout << "...myPV interestDelete for " << myName << endl;
E 24
E 22
E 21
E 6
    myMonitor=0;
E 4
D 6
    return;
E 6
  }


//---------------------------------------------------


D 27
  epicsShareFunc const char *getName() const {
E 27
I 27
  epicsShareFunc const char *myPV::getName() const {
E 27
D 21
    return(pvnames[myIndex]);
E 21
I 21
D 24
    return(pvNames[myIndex]);
E 24
I 24
    return(myName);
E 24
E 21
  }
  

//---------------------------------------------------


D 27
  void destroy() {
E 27
I 27
  void myPV::destroy() {
E 27
D 21
    if(debug!=0)cout << "clasPV destroy for " << pvnames[myIndex] << endl;
E 21
I 21
D 22
    if(debug!=0)cout << "clasPV destroy for " << pvNames[myIndex] << endl;
E 22
I 22
D 24
    if(debug!=0)cout << "myPV destroy for " << pvNames[myIndex] << endl;
E 24
I 24
    if(debug!=0)cout << "myPV destroy for " << myName << endl;
E 24
E 22
E 21
  }
E 3
    

//---------------------------------------------------


D 3
    ~myPV() {
	return;
    }
E 3
I 3
D 22
  ~clasPV() {
D 21
    if(debug!=0)cout << "clasPV destructor for " << pvnames[myIndex] << endl;
E 21
I 21
    if(debug!=0)cout << "clasPV destructor for " << pvNames[myIndex] << endl;
E 22
I 22
D 27
  ~myPV() {
E 27
I 27
  myPV::~myPV() {
E 27
D 24
    if(debug!=0)cout << "myPV destructor for " << pvNames[myIndex] << endl;
E 24
I 24
    if(debug!=0)cout << "myPV destructor for " << myName << endl;
E 24
E 22
E 21
D 6
    return;
E 6
  }
E 3

D 27
};
E 27

D 27

E 27
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------


I 14
//  channel objects for setting read/write access for all attribute channels
D 22
class clasAttrChannel : public casChannel {
E 22
I 22
class myAttrChannel : public casChannel {
E 22

public:

D 22
  clasAttrChannel(const casCtx &ctxIn) : casChannel(ctxIn) {
E 22
I 22
  myAttrChannel(const casCtx &ctxIn) : casChannel(ctxIn) {
E 22
  }


//---------------------------------------------------


D 29
  aitBool readAccess() const {
E 29
I 29
  /*sergey aitBool*/bool readAccess() const {
E 29
    return aitTrue;
  }


//---------------------------------------------------


D 29
  aitBool writeAccess() const {
E 29
I 29
  /*sergey aitBool*/bool writeAccess() const {
E 29
    return aitTrue;
  }

};


//---------------------------------------------------------------------------
//---------------------------------------------------------------------------


E 14
I 13
D 22
class clasAttrPV : public casPV {
E 22
I 22
D 27
class myAttrPV : public casPV {
E 22


public:
D 16
    int myPV;
E 16
I 16
D 24
    int myIndex;
E 24
I 24
    myPV *myPtr;
E 24
E 16
    char myAttr[4];


D 14

  clasAttrPV(int ref, char* attr) {
E 14
I 14
D 16
  clasAttrPV(int ref, const char* attr, int len) {
E 14
      myPV=ref;
E 16
I 16
D 22
  clasAttrPV(int index, const char* attr, int len) {
E 22
I 22
D 24
  myAttrPV(int index, const char* attr, int len) {
E 22
      myIndex=index;
E 24
I 24
  myAttrPV(myPV *ptr, const char* attr, int len) {
E 27
I 27
  myAttrPV::myAttrPV(myPV *ptr, const char* attr, int len) {
E 27
      myPtr=ptr;
E 24
E 16
D 14
      strncpy(myAttr,attr,4);
E 14
I 14
      strncpy(myAttr,"    ",4);
D 30
      strncpy(myAttr,attr,min(4,len));
E 30
I 30
      strncpy(myAttr,attr,MIN(4,len));
E 30
E 14
D 16
      if(debug!=0)cout << "...clasAttrPV constructor for " << pvnames[myPV] 
E 16
I 16
D 21
      if(debug!=0)cout << "...clasAttrPV constructor for " << pvnames[myIndex] 
E 21
I 21
D 22
      if(debug!=0)cout << "...clasAttrPV constructor for " << pvNames[myIndex] 
E 22
I 22
D 24
      if(debug!=0)cout << "...myAttrPV constructor for " << pvNames[myIndex] 
E 24
I 24
      if(debug!=0)cout << "myAttrPV constructor for " << myPtr->myName 
E 24
E 22
E 21
E 16
		       << ", attr: " << myAttr << endl;
  }
  

//---------------------------------------------------


I 14
D 27
  casChannel *createChannel(const casCtx &ctx,
			    const char * const pUserName, 
			    const char * const pHostName) {
E 27
I 27
  casChannel *myAttrPV::createChannel(const casCtx &ctx,
				      const char * const pUserName, 
				      const char * const pHostName) {
E 27
D 22
    return new clasAttrChannel(ctx);
E 22
I 22
    return new myAttrChannel(ctx);
E 22
  }


//---------------------------------------------------


I 15
D 27
  aitEnum bestExternalType() const {
E 27
I 27
  aitEnum myAttrPV::bestExternalType() const {
E 27
    return(aitEnumInt32);
}


//---------------------------------------------------


E 15
E 14
D 27
  caStatus read(const casCtx &ctx, gdd &value) {
E 27
I 27
  caStatus myAttrPV::read(const casCtx &ctx, gdd &value) {
E 27
D 16
    if(debug!=0)cout << "...clasAttrPV read for " << pvnames[myPV] << endl;
E 16
I 16
D 21
    if(debug!=0)cout << "...clasAttrPV read for " << pvnames[myIndex] << endl;
E 21
I 21
D 22
    if(debug!=0)cout << "...clasAttrPV read for " << pvNames[myIndex] << endl;
E 22
I 22
D 24
    if(debug!=0)cout << "...myAttrPV read for " << pvNames[myIndex] << endl;
E 24
I 24
      if(debug!=0)cout << "...myAttrPV read for " << myPtr->myName << ", attr: " << myAttr << endl;
E 24
E 22
E 21
E 16
D 14
    return S_casApp_success;
E 14
I 14


    if(strncasecmp(myAttr,"ALRM",4)==0) {
D 16
      value.putConvert(clasAlarm[myPV]);
E 16
I 16
D 22
      value.putConvert(clasAlarm[myIndex]);
E 22
I 22
D 24
      value.putConvert(pvAlarm[myIndex]);
E 24
I 24
      value.putConvert(myPtr->myAlarm);
E 24
E 22
E 16
      return(S_casApp_success);

    } else if (strncasecmp(myAttr,"HIHI",4)==0) {
D 16
      value.putConvert(clasHI[myPV]);
E 16
I 16
D 22
      value.putConvert(clasHI[myIndex]);
E 22
I 22
D 23
      value.putConvert(pvHI[myIndex]);
E 23
I 23
D 24
      value.putConvert(pvHIHI[myIndex]);
E 24
I 24
      value.putConvert(myPtr->myHIHI);
E 24
E 23
E 22
E 16
      return(S_casApp_success);

    } else if (strncasecmp(myAttr,"LOLO",4)==0) {
D 16
      value.putConvert(clasLO[myPV]);
E 16
I 16
D 22
      value.putConvert(clasLO[myIndex]);
E 22
I 22
D 23
      value.putConvert(pvLO[myIndex]);
E 23
I 23
D 24
      value.putConvert(pvLOLO[myIndex]);
E 24
I 24
      value.putConvert(myPtr->myLOLO);
E 24
E 23
E 22
E 16
      return(S_casApp_success);
      
D 18
    } else if (strncasecmp(myAttr,"HI  ",4)==0) {
E 18
I 18
    } else if (strncasecmp(myAttr,"HIGH",4)==0) {
E 18
D 16
      value.putConvert(clasWHI[myPV]);
E 16
I 16
D 22
      value.putConvert(clasWHI[myIndex]);
E 22
I 22
D 23
      value.putConvert(pvWHI[myIndex]);
E 23
I 23
D 24
      value.putConvert(pvHIGH[myIndex]);
E 24
I 24
      value.putConvert(myPtr->myHIGH);
E 24
E 23
E 22
E 16
      return(S_casApp_success);
      
D 18
    } else if (strncasecmp(myAttr,"LO  ",4)==0) {
E 18
I 18
    } else if (strncasecmp(myAttr,"LOW ",4)==0) {
E 18
D 16
      value.putConvert(clasWLO[myPV]);
E 16
I 16
D 22
      value.putConvert(clasWLO[myIndex]);
E 22
I 22
D 23
      value.putConvert(pvWLO[myIndex]);
E 23
I 23
D 24
      value.putConvert(pvLOW[myIndex]);
E 24
I 24
      value.putConvert(myPtr->myLOW);
E 24
E 23
E 22
E 16
      return(S_casApp_success);
      
I 16
    } else if (strncasecmp(myAttr,"HOPR",4)==0) {
D 22
      value.putConvert(clasGHI[myIndex]);
E 22
I 22
D 23
      value.putConvert(pvGHI[myIndex]);
E 23
I 23
D 24
      value.putConvert(pvHOPR[myIndex]);
E 24
I 24
      value.putConvert(myPtr->myHOPR);
E 24
E 23
E 22
      return(S_casApp_success);
      
    } else if (strncasecmp(myAttr,"LOPR",4)==0) {
D 22
      value.putConvert(clasGLO[myIndex]);
E 22
I 22
D 23
      value.putConvert(pvGLO[myIndex]);
E 23
I 23
D 24
      value.putConvert(pvLOPR[myIndex]);
E 24
I 24
      value.putConvert(myPtr->myLOPR);
E 24
E 23
E 22
      return(S_casApp_success);
      
I 18
    } else if (strncasecmp(myAttr,"DRVH",4)==0) {
D 22
      value.putConvert(clasCHI[myIndex]);
E 22
I 22
D 23
      value.putConvert(pvCHI[myIndex]);
E 23
I 23
D 24
      value.putConvert(pvDRVH[myIndex]);
E 24
I 24
      value.putConvert(myPtr->myDRVH);
E 24
E 23
E 22
      return(S_casApp_success);
      
    } else if (strncasecmp(myAttr,"DRVL",4)==0) {
D 22
      value.putConvert(clasCLO[myIndex]);
E 22
I 22
D 23
      value.putConvert(pvCLO[myIndex]);
E 23
I 23
D 24
      value.putConvert(pvDRVL[myIndex]);
E 24
I 24
      value.putConvert(myPtr->myDRVL);
E 24
E 23
E 22
      return(S_casApp_success);
      
E 18
E 16
    } else if (strncasecmp(myAttr,"STAT",4)==0) {
D 16
	value.putConvert(pclasPV[myPV]->myStat);
E 16
I 16
D 22
	value.putConvert(pclasPV[myIndex]->myStat);
E 22
I 22
D 24
	value.putConvert(pPV[myIndex]->myStat);
E 24
I 24
      value.putConvert(myPtr->myStat);
E 24
E 22
E 16
      return(S_casApp_success);

    } else if (strncasecmp(myAttr,"SEVR",4)==0) {
D 16
	value.putConvert(pclasPV[myPV]->mySevr);
E 16
I 16
D 22
	value.putConvert(pclasPV[myIndex]->mySevr);
E 22
I 22
D 24
	value.putConvert(pPV[myIndex]->mySevr);
E 24
I 24
      value.putConvert(myPtr->mySevr);
E 24
E 22
E 16
      return(S_casApp_success);

I 20
    } else if (strncasecmp(myAttr,"PREC",4)==0) {
D 22
      value.putConvert(clasPREC[myIndex]);
E 22
I 22
D 24
      value.putConvert(pvPREC[myIndex]);
E 24
I 24
      value.putConvert(myPtr->myPREC);
E 24
E 22
      return(S_casApp_success);

E 20
    } else {
      cerr << "No read support for attribute: \"" << myAttr << "\"" << endl;
      return(S_casApp_noSupport);
    }

E 14
  }
  

//---------------------------------------------------


D 27
  caStatus write(const casCtx &ctx, gdd &value) {
E 27
I 27
D 29
  caStatus myAttrPV::write(const casCtx &ctx, gdd &value) {
E 29
I 29
  caStatus myAttrPV::write(const casCtx &ctx, const gdd &value) {
E 29
E 27
D 14
    if(debug!=0)cout << "...clasAttrPV write for " << pvnames[myPV] << endl;
    clasUpdate[myPV]=1;
E 14
I 14
D 16
    if(debug!=0)cout << "...clasAttrPV write for " << pvnames[myPV] 
E 16
I 16
D 21
    if(debug!=0)cout << "...clasAttrPV write for " << pvnames[myIndex] 
E 21
I 21
D 22
    if(debug!=0)cout << "...clasAttrPV write for " << pvNames[myIndex] 
E 22
I 22
D 24
    if(debug!=0)cout << "...myAttrPV write for " << pvNames[myIndex] 
E 22
E 21
E 16
		     << ",  attr: " << myAttr << endl;
E 24
I 24
    if(debug!=0)cout << "...myAttrPV write for " << myPtr->myName 
		     << ",  attr: " << myAttr << ",  value: " << (int)value << endl;
E 24

D 16
    cout << "clasAttr write for myPv " << myPV << "  value: " << (int)value << endl;
E 16
I 16
D 22
    cout << "clasAttr write for myIndex " << myIndex << "  value: " << (int)value << endl;
E 22
I 22
D 24
    cout << "myAttr write for myIndex " << myIndex << "  value: " << (int)value << endl;
E 24
E 22
E 16

    if(strncasecmp(myAttr,"ALRM",4)==0) {
D 16
	clasAlarm[myPV]=(int)value;
E 16
I 16
D 22
	clasAlarm[myIndex]=(int)value;
E 22
I 22
D 24
	pvAlarm[myIndex]=(int)value;
E 24
I 24
	myPtr->myAlarm=(int)value;
E 24
E 22
E 16

    } else if (strncasecmp(myAttr,"HIHI",4)==0) {
D 16
	clasHI[myPV]=(int)value;
E 16
I 16
D 22
	clasHI[myIndex]=(int)value;
E 22
I 22
D 23
	pvHI[myIndex]=(int)value;
E 23
I 23
D 24
	pvHIHI[myIndex]=(int)value;
E 24
I 24
	myPtr->myHIHI=(int)value;
E 24
E 23
E 22
E 16

    } else if (strncasecmp(myAttr,"LOLO",4)==0) {
D 16
	clasLO[myPV]=(int)value;
E 16
I 16
D 22
	clasLO[myIndex]=(int)value;
E 22
I 22
D 23
	pvLO[myIndex]=(int)value;
E 23
I 23
D 24
	pvLOLO[myIndex]=(int)value;
E 24
I 24
	myPtr->myLOLO=(int)value;
E 24
E 23
E 22
E 16
      
D 18
    } else if (strncasecmp(myAttr,"HI  ",4)==0) {
E 18
I 18
    } else if (strncasecmp(myAttr,"HIGH",4)==0) {
E 18
D 16
	clasWHI[myPV]=(int)value;
E 16
I 16
D 22
	clasWHI[myIndex]=(int)value;
E 22
I 22
D 23
	pvWHI[myIndex]=(int)value;
E 23
I 23
D 24
	pvHIGH[myIndex]=(int)value;
E 24
I 24
	myPtr->myHIGH=(int)value;
E 24
E 23
E 22
E 16
      
D 18
    } else if (strncasecmp(myAttr,"LO  ",4)==0) {
E 18
I 18
    } else if (strncasecmp(myAttr,"LOW ",4)==0) {
E 18
D 16
	clasWLO[myPV]=(int)value;
E 16
I 16
D 22
	clasWLO[myIndex]=(int)value;
E 22
I 22
D 23
	pvWLO[myIndex]=(int)value;
E 23
I 23
D 24
	pvLOW[myIndex]=(int)value;
E 24
I 24
	myPtr->myLOW=(int)value;
E 24
E 23
E 22
E 16

I 16
    } else if (strncasecmp(myAttr,"HOPR",4)==0) {
D 22
	clasGHI[myIndex]=(int)value;
E 22
I 22
D 23
	pvGHI[myIndex]=(int)value;
E 23
I 23
D 24
	pvHOPR[myIndex]=(int)value;
E 24
I 24
	myPtr->myHOPR=(int)value;
E 24
E 23
E 22

    } else if (strncasecmp(myAttr,"LOPR",4)==0) {
D 22
	clasGLO[myIndex]=(int)value;
E 22
I 22
D 23
	pvGLO[myIndex]=(int)value;
E 23
I 23
D 24
	pvLOPR[myIndex]=(int)value;
E 24
I 24
	myPtr->myLOPR=(int)value;
E 24
E 23
E 22

I 18
    } else if (strncasecmp(myAttr,"DRVH",4)==0) {
D 22
	clasCHI[myIndex]=(int)value;
E 22
I 22
D 23
	pvCHI[myIndex]=(int)value;
E 23
I 23
D 24
	pvDRVH[myIndex]=(int)value;
E 24
I 24
	myPtr->myDRVH=(int)value;
E 24
E 23
E 22

    } else if (strncasecmp(myAttr,"DRVL",4)==0) {
D 22
	clasCLO[myIndex]=(int)value;
E 22
I 22
D 23
	pvCLO[myIndex]=(int)value;
E 23
I 23
D 24
	pvDRVL[myIndex]=(int)value;
E 24
I 24
	myPtr->myDRVL=(int)value;
E 24
E 23
E 22

I 20
    } else if (strncasecmp(myAttr,"PREC",4)==0) {
D 22
	clasPREC[myIndex]=(int)value;
E 22
I 22
D 24
	pvPREC[myIndex]=(int)value;
E 24
I 24
	myPtr->myPREC=(int)value;
E 24
E 22

E 20
E 18
E 16
    } else {
	cerr << "No write support for attr: \"" << myAttr << "\"" << endl;
	return S_casApp_noSupport;
    }

D 16
    setAlarm(myPV);
E 16
I 16
D 21
    setAlarm(myIndex);
E 21
I 21
D 22
    pclasPV[myIndex]->setAlarm();
E 22
I 22
D 24
    pPV[myIndex]->setAlarm();
E 24
I 24
    myPtr->setAlarm();
E 24
E 22
E 21
E 16

E 14
    return S_casApp_success;
  }
  

//---------------------------------------------------


D 27
  epicsShareFunc const char *getName() const {
E 27
I 27
  epicsShareFunc const char *myAttrPV::getName() const {
E 27
D 16
    if(debug!=0)cout << "...clasAttrPV getName for " << pvnames[myPV] << endl;
    sprintf(temp,"%s.%4s",pvnames[myPV],myAttr);
E 16
I 16
D 21
    if(debug!=0)cout << "...clasAttrPV getName for " << pvnames[myIndex] << endl;
    sprintf(temp,"%s.%4s",pvnames[myIndex],myAttr);
E 21
I 21
D 22
    if(debug!=0)cout << "...clasAttrPV getName for " << pvNames[myIndex] << endl;
E 22
I 22
D 24
    if(debug!=0)cout << "...myAttrPV getName for " << pvNames[myIndex] << endl;
E 22
    sprintf(temp,"%s.%4s",pvNames[myIndex],myAttr);
E 24
I 24
    sprintf(temp,"%s.%4s",myPtr->myName,myAttr);
E 24
E 21
E 16
    return(temp);
  }


I 14
//---------------------------------------------------


D 22
  ~clasAttrPV() {
    cout << "...clasAttrPV destructor" << endl;
E 22
I 22
D 27
  ~myAttrPV() {
E 27
I 27
  myAttrPV::~myAttrPV() {
E 27
D 24
    cout << "...myAttrPV destructor" << endl;
E 24
I 24
D 28
    cout << "myAttrPV destructor" << endl;
E 28
I 28
    if(debug!=0)cout << "myAttrPV destructor" << endl;
E 28
E 24
E 22
  }
D 27
  
E 27

E 14
D 27
};
E 27

D 27

E 27
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
D 27


E 13
D 3
class myServer : public caServer {
E 3
I 3
D 22
class clasServer : public caServer {
E 22
I 22
class myServer : public caServer {
E 22
  
E 3

D 3

E 3
public:
    
D 3
    pvExistReturn pvExistTest(const casCtx &ctx, const char *pPVName) {
	
	if(a) {
	    return pverExistsHere;
	} else {
	    return pverDoesNotExistHere;
	}
	
    }
E 3
I 3
  pvExistReturn pvExistTest(const casCtx &ctx, const char *pPVName) {
E 3
    
I 11
    int PVLen = strcspn(pPVName,".");

E 11
D 3
    
E 3
I 3
D 21
    for(int i=0; i<sizeof(pvnames)/sizeof(char*); i++) {
D 11
      if(strcmp(pPVName,pvnames[i])==0)return pverExistsHere;
E 11
I 11
      if((PVLen==strlen(pvnames[i]))&&(strncasecmp(pPVName,pvnames[i],PVLen)==0))
E 21
I 21
    for(int i=0; i<sizeof(pvNames)/sizeof(char*); i++) {
      if((PVLen==strlen(pvNames[i]))&&(strncasecmp(pPVName,pvNames[i],PVLen)==0))
E 21
	 return pverExistsHere;
E 11
    }
    return pverDoesNotExistHere;
  }
  
  
E 3
//---------------------------------------------------


D 3
    pvCreateReturn createPV(const casCtx &ctx, const char *pPVName) {
	
	if(a) {
	    return(*pv);
	} else {
	    return(S_casApp_pvNotFound);
	}
	
E 3
I 3
  pvCreateReturn createPV(const casCtx &ctx, const char *pPVName) {
D 14
    
I 11
    int PVLen = strcspn(pPVName,".");

E 14
E 11
    if(debug!=0)cout << "createPV for " << pPVName << endl;
I 14

    int PVNameLen      = strlen(pPVName);
    int PVLen          = strcspn(pPVName,".");
    const char *pattr  = pPVName+PVLen+1;
    int lattr          = PVNameLen-PVLen-1;

E 14
D 21
    for(int i=0; i<sizeof(pvnames)/sizeof(char*); i++) {
D 11
      if(strcmp(pPVName,pvnames[i])==0)return(*pv[i]);
E 11
I 11
	if((PVLen==strlen(pvnames[i]))&&(strncasecmp(pPVName,pvnames[i],PVLen)==0)) {
E 21
I 21
    for(int i=0; i<sizeof(pvNames)/sizeof(char*); i++) {
	if((PVLen==strlen(pvNames[i]))&&(strncasecmp(pPVName,pvNames[i],PVLen)==0)) {
E 21
D 14
            for(int j=0; j<4; j++) pv[i]->myAttr[j]=' ';
	    if(strlen(pPVName)>PVLen) {
		strncpy(pv[i]->myAttr,pPVName+PVLen+1,min(4,strlen(pPVName)-PVLen-1));
		if(debug!=0)cout << "myAttr is \"" << pv[i]->myAttr << "\"" << endl;
E 14
I 14
	    if( (PVNameLen==PVLen) || ((lattr==3)&&(strncasecmp(pattr,"VAL",3)==0)) ) {
D 22
		return(*pclasPV[i]);
E 22
I 22
		return(*pPV[i]);
E 22
	    } else {
D 22
		return(*(new clasAttrPV(i,pattr,lattr)));
E 22
I 22
D 24
		return(*(new myAttrPV(i,pattr,lattr)));
E 24
I 24
		return(*(new myAttrPV(pPV[i],pattr,lattr)));
E 24
E 22
E 14
	    }
D 14
	    return(*pv[i]);
E 14
	}
E 11
E 3
    }
I 3
    return(S_casApp_pvNotFound);
  }
E 3
  
    
//---------------------------------------------------
    
    
D 3
    ~myServer() {
	return;
E 3
I 3
D 22
    ~clasServer() {
      if(debug!=0)cout << "clasServer destructor" << endl;
E 22
I 22
    ~myServer() {
      if(debug!=0)cout << "myServer destructor" << endl;
E 22
      return;
E 3
    }
    
};


E 27
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------


I 27

E 27
I 3
//  must declare all static vars outside of class definition to allocate storage
D 22
int clasPV::ft_is_initialized=0;
gddAppFuncTable<clasPV> clasPV::ft;
E 22
I 22
int myPV::ft_is_initialized=0;
gddAppFuncTable<myPV> myPV::ft;
E 22


I 27
//---------------------------------------------------------------------------
E 27

E 3
D 27
main(int argc,char **argv) {
E 27

D 27

  int status;
D 3
  pthread_t t1,t2,t3;
  time_t now;
E 3
I 3
  pthread_t t1,t2;
E 3


  // synch with stdio
  ios::sync_with_stdio();


  // decode command line...flags may be overridden in Tcl startup script
  decode_command_line(argc,argv);


D 5
  // set session name if not specified via env variable or on command line
  if(session==NULL)session=(char*)"clasprod";


E 5
  // get Tcl interp, create tcl commands, link vars, process Tcl startup script, etc.
  interp=tclinterp_init();
  init_tcl();
  if(init_tcl_script!=NULL)Tcl_EvalFile(interp,init_tcl_script);
I 3
  
E 3

D 3

E 3
  // set ipc parameters and connect to ipc system
  if(!TipcInitThreads()) {
    cerr << "Unable to init IPC thread package" << endl;
    exit(EXIT_FAILURE);
  }
  ipc_set_application(application);
  ipc_set_user_status_poll_callback(status_poll_callback);
  ipc_set_quit_callback(quit_callback);
  status=ipc_init(unique_id,"Epics Server");
  if(status<0) {
    cerr << "\n?Unable to connect to server...probably duplicate unique id\n"
	 << "   ...check for another epics_server  using ipc_info\n"
	 << "   ...only one connection allowed!" << endl << endl;
    exit(EXIT_FAILURE);
  }


I 3
D 4
  //  smartsockets callbacks by message type and/or subject
  TipcMt mt((T_STR)"info_server");
  server.SubjectSubscribe((T_STR)"/info_server/in/scaler_statistics_monitor",TRUE);
  server.SubjectCbCreate((T_STR)"",mt,scaler_callback,NULL);
E 4
I 4
  //  setup smartsockets callbacks
  server.SubjectSubscribe((T_STR)"/info_server/out/epics_server",TRUE);
  server.SubjectCbCreate((T_STR)"/info_server/out/epics_server",NULL,epics_server_callback,NULL);
E 4


E 3
  //  create tcl_request callback
  tclipc_init(interp);


I 3
D 12
  // create array pv objects
E 12
I 12
  // create array of pv objects
E 12
D 21
  for(int i=0; i<sizeof(pvnames)/sizeof(char*); i++) {
E 21
I 21
  for(int i=0; i<sizeof(pvNames)/sizeof(char*); i++) {
E 21
D 12
    pv[i] = new clasPV(i);
E 12
I 12
D 14
      pv[i] = new clasPV(i);
E 14
I 14
D 22
      pclasPV[i] = new clasPV(i);
E 22
I 22
D 24
      pPV[i] = new myPV(i);
E 24
I 24
      pPV[i] = new myPV(pvNames[i],pvTypes[i],pvUnits[i],pvAlarm[i],pvHIHI[i],pvLOLO[i],
			pvHIGH[i],pvLOW[i],pvHOPR[i],pvLOPR[i],pvDRVH[i],pvDRVL[i],pvPREC[i]);
E 24
E 22
E 14
E 12
  }

I 12

E 12
  // create ca server
D 22
  clasServer *cas = new clasServer();
E 22
I 22
  myServer *cas = new myServer();
E 22


  // launch threads
  thr_setconcurrency(thr_getconcurrency()+3);
  pthread_create(&t1,NULL,data_thread,(void*)NULL);
D 4
  pthread_create(&t2,NULL,ca_thread,(void*)NULL);
E 4
I 4
  pthread_create(&t2,NULL,ipc_thread,(void*)NULL);
E 4


E 3
  // post startup message
  sprintf(temp,"Process startup:    epics_server starting in %s",application);
  status=insert_msg("epics_server","online",unique_id,"status",0,"START",0,temp);
  

D 3
  // create ca server
  myServer *cas = new myServer();


E 3
  // flush output to log files, etc
  fflush(NULL);


D 3
  // launch info thread
  thr_setconcurrency(thr_getconcurrency()+3);
  pthread_create(&t1,NULL,ipc_thread,NULL);
  pthread_create(&t2,NULL,ca_thread,NULL);


  // ca server loop
  while (done==0) {
    now=time(NULL);
E 3
I 3
D 4
  //  main loop
E 4
I 4
D 6
  //  ca loop
E 6
I 6
  //  ca server loop
D 17
  gdd *value = new gdd();
E 17
E 6
E 4
  while(done==0) {
D 4
    sleep(main_sleep_time);
E 4
I 4

D 6
    fileDescriptorManager.process((double)ca_sleep_time);
E 6
I 6
    fileDescriptorManager.process((double)ca_pend_time);
E 6

    // check for updates and satisfy monitorOn requests
D 21
    for(int i=0; i<sizeof(pvnames)/sizeof(char*); i++) {
E 21
I 21
    for(int i=0; i<sizeof(pvNames)/sizeof(char*); i++) {
E 21
D 14
      if((pv[i]->myMonitor!=0)&&(clasUpdate[i]!=0)) {
E 14
I 14
D 22
      if((pclasPV[i]->myMonitor!=0)&&(clasUpdate[i]!=0)) {
E 22
I 22
D 24
      if((pPV[i]->myMonitor!=0)&&(pvUpdate[i]!=0)) {
E 24
I 24
      if((pPV[i]->myMonitor!=0)&&(pPV[i]->myUpdate!=0)) {
E 24
E 22
E 14
I 6
D 21
	if(debug!=0)cout << "monitorOn response for " << pvnames[i] << endl;
E 21
I 21
D 25
	if(debug!=0)cout << "monitorOn response for " << pvNames[i] << endl;
E 25
I 25
	  if(debug!=0)cout << "(monitorOn response for " << pvNames[i] << ")" << endl;
E 25
E 21
I 17
	gdd *value = new gdd();
E 17
E 6
D 14
	caServer *pCAS = pv[i]->getCAS();
E 14
I 14
D 22
	caServer *pCAS = pclasPV[i]->getCAS();
E 22
I 22
	caServer *pCAS = pPV[i]->getCAS();
E 22
E 14
	casEventMask select(pCAS->valueEventMask | pCAS->logEventMask);
D 6
	pv[i]->postEvent(select,*pv[i]->myValue);
E 6
I 6
D 12
	pv[i]->getVAL(*value);
E 12
I 12
D 14
	pv[i]->getRealVAL(*value);
E 12
	pv[i]->postEvent(select,*value);
E 14
I 14
D 22
	pclasPV[i]->getVAL(*value);
	pclasPV[i]->postEvent(select,*value);
E 14
	clasUpdate[i]=0;
E 22
I 22
	pPV[i]->getVAL(*value);
	pPV[i]->postEvent(select,*value);
D 24
	pvUpdate[i]=0;
E 24
I 24
	pPV[i]->myUpdate=0;
E 24
E 22
E 6
      }
    }
E 4
E 3
  }


D 3

E 3
  // post shutdown message
  sprintf(temp,"Process shutdown:  epics_server");
  status=insert_msg("epics_server","online",unique_id,"status",0,"STOP",0,temp);
  
D 3

E 3
I 3
  
E 3
  // done...clean up
  ipc_close();
  exit(EXIT_SUCCESS);
E 27
I 27
void setDebug(int val) {
  debug=val;
E 27
}
D 27
       
E 27

D 27
//--------------------------------------------------------------------------
E 27

D 27

I 4
D 8
//  periodically retrieves online info not in smartsockets
E 8
I 8
//  periodically retrieves online info from files
E 8
E 4
D 3
void *ipc_thread(void *param) {
E 3
I 3
void *data_thread(void *param) {
E 3

D 3
  for(;;;) {
    server.MainLoop(10.0);
E 3
I 3
D 4
  int i,ind;
  char *p;
E 4
I 4
D 8
  time_t now;
E 8
I 8
    ifstream file;
E 8
E 4

D 4

  // init DP
  DP_cmd_init(getenv("MSQL_TCP_HOST"));


E 4
  while(done==0) {
I 4
D 8
    now=time(NULL)-epicsToLocalTime;
E 8
E 4

D 4
    // read run number from msql database
    run_number=get_run_number(msql_database,session);
E 4
I 4
D 8
    // get pretrig and discr info
E 8
I 8
    // ec pretrig
    sprintf(filename,"%s/%s",clon_parms,ec_pretrig_file);
    file.open(filename);
    if(!file.is_open()) {
	cerr << "?unable to open " << filename << endl;
    } else {
	get_ec_p(file);
	file.close();
    }
E 8
E 4


I 8
    // cc pretrig
    sprintf(filename,"%s/%s",clon_parms,cc_pretrig_file);
    file.open(filename);
    if(!file.is_open()) {
	cerr << "?unable to open " << filename << endl;
    } else {
	get_cc_p(file);
	file.close();
    }


    // sc pretrig
    sprintf(filename,"%s/%s",clon_parms,sc_pretrig_file);
    file.open(filename);
    if(!file.is_open()) {
	cerr << "?unable to open " << filename << endl;
    } else {
	get_sc_p(file);
	file.close();
    }


    // tdc info
    if(run_number>0) {
	sprintf(filename,"%s/%s_%06d.txt",clon_parms,tdc_file,run_number);
	file.open(filename);
	if(!file.is_open()) {
	    cerr << "?unable to open " << filename << endl;
	} else {
	    get_tdc_info(file);
	    file.close();
	}
    }


E 8
D 4
    // get event count from EB
    DP_cmd((char*)"EB1",(char*)"statistics",temp,2);
    sscanf(temp,"%d %d",&event_count,&event_rate);
E 4
I 4
D 6
    
E 4

D 4

    // get ts info
    DP_cmd((char*)"clastrig2",(char*)"exec ts_reg()",temp,3);
    p=temp;
    for(i=0; i<128; i++) {p=strchr(p,'x')+1; sscanf(p,"%x %n",&ts_reg[i],&ind); p+=ind;}

    //??? failed DP_cmd ???


    // pull out a few interesting numbers
    ts_csr=ts_reg[0];
    ts_control=ts_reg[1]&0xffff;
    ts_roc_enable=ts_reg[2];
    ts_synch=ts_reg[3]&0xffff;
    for(i=0; i<4; i++) ts_timer[i]=ts_reg[16+i]&0xffff;
    ts_timer[4]=ts_reg[20]&0xff;
    for(i=0; i<4; i++) prescale[i]=ts_reg[ 8+i]&0xffffff;
    for(i=4; i<8; i++) prescale[i]=ts_reg[ 8+i]&0xffff;
E 4
    

E 6
    // until next time...
D 6
    sleep(data_sleep_time);
E 6
I 6
D 8
    sleep(data_pend_time);
E 8
I 8
    sleep(data_sleep_time);
E 8
E 6
E 3
  }  
I 3

  return((void*)NULL);
E 3
}


//--------------------------------------------------------------------------


I 8
//  reads ec pretrig info
void get_ec_p(ifstream &f) {
    
    int i,j;
    int avg,p[6];
D 21
    time_t now=time(NULL)-epicsToLocalTime;
E 21
    

    // skip some lines
    for(i=0; i<5; i++)f.getline(line,sizeof(line));

    // inner,outer,total for hi,lo
    for(i=0; i<6; i++) {
	avg=0;
	if(f.is_open()&&f.good()&&!f.eof()&&!f.fail()&&!f.bad()) {
	    f.getline(line,sizeof(line));
	    sscanf(line,"%d %d %d %d %d %d",&p[0],&p[1],&p[2],&p[3],&p[4],&p[5]);
	    for(j=0; j<6; j++) avg+=p[j]; avg/=6;
	}
D 21
	fillPV(16+i,avg,now);
E 21
I 21
D 22
	pclasPV[16+i]->fillPV(avg);
E 22
I 22
	pPV[16+i]->fillPV(avg);
E 22
E 21
    }
    
    return;
}


//--------------------------------------------------------------------------


//  reads cc pretrig info
void get_cc_p(ifstream &f) {
    
    int i,p[4]={0,0,0,0};
D 21
    time_t now=time(NULL)-epicsToLocalTime;
E 21
    

    // skip some lines, then read
    for(i=0; i<3; i++)f.getline(line,sizeof(line));
    if(f.is_open()&&f.good()&&!f.eof()&&!f.fail()&&!f.bad()) {
	f.getline(line,sizeof(line));
	sscanf(line,"%d %d %d %d",&p[0],&p[1],&p[2],&p[3]);
    }
D 21
    fillPV(22,(p[0]+p[2])/2,now);
    fillPV(23,(p[1]+p[3])/2,now);
E 21
I 21
D 22
    pclasPV[22]->fillPV((p[0]+p[2])/2);
    pclasPV[23]->fillPV((p[1]+p[3])/2);
E 22
I 22
    pPV[22]->fillPV((p[0]+p[2])/2);
    pPV[23]->fillPV((p[1]+p[3])/2);
E 22
E 21
    
    return;
}


//--------------------------------------------------------------------------


//  reads sc pretrig info
void get_sc_p(ifstream &f) {
    
    int i,p[2]={0,0};
D 21
    time_t now=time(NULL)-epicsToLocalTime;
E 21
    

    // skip some lines, then read
    for(i=0; i<3; i++)f.getline(line,sizeof(line));

    if(f.is_open()&&f.good()&&!f.eof()&&!f.fail()&&!f.bad()) {
	f.getline(line,sizeof(line));
	sscanf(line,"%d %d",&p[0],&p[1]);
    }
D 21
    fillPV(24,p[0],now);
    fillPV(25,p[1],now);
E 21
I 21
D 22
    pclasPV[24]->fillPV(p[0]);
    pclasPV[25]->fillPV(p[1]);
E 22
I 22
    pPV[24]->fillPV(p[0]);
    pPV[25]->fillPV(p[1]);
E 22
E 21
    
    return;
}


//--------------------------------------------------------------------------


//  reads tdc info
void get_tdc_info(ifstream &f) {
    
    int i,j,count;
    int t_avg,w_avg,p[5];
D 21
    time_t now=time(NULL)-epicsToLocalTime;
E 21
    

    // ec
    count=0;
    t_avg=0;
    w_avg=0;
    if(find_tag_line(f,"*ec*",line,sizeof(line))==0) {
	f.getline(line,sizeof(line));
	while(f.is_open()&&f.good()&&!f.eof()&&!f.fail()&&!f.bad()) {
	    f.getline(line,sizeof(line));
	    if(strlen(line)==0)continue;
	    if(strncmp(line,"*",1)==0)break;
	    count++;
	    sscanf(line,"%d %d %d %d %d",&p[0],&p[1],&p[2],&p[3],&p[4]);
	    t_avg+=p[3];
	    w_avg+=p[4];
	}
    }
D 21
    fillPV(26,(count!=0)?t_avg/count:0,now);
    fillPV(27,(count!=0)?w_avg/count:0,now);
E 21
I 21
D 22
    pclasPV[26]->fillPV((count!=0)?t_avg/count:0);
    pclasPV[27]->fillPV((count!=0)?w_avg/count:0);
E 22
I 22
    pPV[26]->fillPV((count!=0)?t_avg/count:0);
    pPV[27]->fillPV((count!=0)?w_avg/count:0);
E 22
E 21
    

    // cc
    count=0;
    t_avg=0;
    w_avg=0;
    if(find_tag_line(f,"*cc*",line,sizeof(line))==0) {
	f.getline(line,sizeof(line));
	while(f.is_open()&&f.good()&&!f.eof()&&!f.fail()&&!f.bad()) {
	    f.getline(line,sizeof(line));
	    if(strlen(line)==0)continue;
	    if(strncmp(line,"*",1)==0)break;
	    count++;
	    sscanf(line,"%d %d %d %d %d",&p[0],&p[1],&p[2],&p[3],&p[4]);
	    t_avg+=p[3];
	    w_avg+=p[4];
	}
    }
D 21
    fillPV(28,(count!=0)?t_avg/count:0,now);
    fillPV(29,(count!=0)?w_avg/count:0,now);
E 21
I 21
D 22
    pclasPV[28]->fillPV((count!=0)?t_avg/count:0);
    pclasPV[29]->fillPV((count!=0)?w_avg/count:0);
E 22
I 22
    pPV[28]->fillPV((count!=0)?t_avg/count:0);
    pPV[29]->fillPV((count!=0)?w_avg/count:0);
E 22
E 21

    
    // sc
    count=0;
    t_avg=0;
    w_avg=0;
    if(find_tag_line(f,"*sc*",line,sizeof(line))==0) {
	f.getline(line,sizeof(line));
	while(f.is_open()&&f.good()&&!f.eof()&&!f.fail()&&!f.bad()) {
	    f.getline(line,sizeof(line));
	    if(strlen(line)==0)continue;
	    if(strncmp(line,"*",1)==0)break;
	    count++;
	    sscanf(line,"%d %d %d %d %d",&p[0],&p[1],&p[2],&p[3],&p[4]);
	    t_avg+=p[3];
	    w_avg+=p[4];
	}
    }
D 21
    fillPV(30,(count!=0)?t_avg/count:0,now);
    fillPV(31,(count!=0)?w_avg/count:0,now);
E 21
I 21
D 22
    pclasPV[30]->fillPV((count!=0)?t_avg/count:0);
    pclasPV[31]->fillPV((count!=0)?w_avg/count:0);
E 22
I 22
    pPV[30]->fillPV((count!=0)?t_avg/count:0);
    pPV[31]->fillPV((count!=0)?w_avg/count:0);
E 22
E 21
    

    return;
}


//--------------------------------------------------------------------------


E 8
D 4
void *ca_thread(void *param) {
E 4
I 4
//  receives online info from other programs and dispatches to appropriate callbacks
void *ipc_thread(void *param) {
E 4

D 3
  for(;;;) {
      fileDescriptorManager.process(10.0);
E 3
I 3
  while(done==0) {
D 4
    fileDescriptorManager.process((double)ca_sleep_time);
E 4
I 4
D 6
    server.MainLoop((double)ipc_sleep_time);
E 6
I 6
    server.MainLoop((double)ipc_pend_time);
E 6
E 4
E 3
  }  
I 3
  return((void*)NULL);
E 3
}


//--------------------------------------------------------------------------


I 3
D 4
void *ipc_thread(void *param) {
E 4
I 4
void epics_server_callback(T_IPC_CONN conn,
			   T_IPC_CONN_PROCESS_CB_DATA data,
			   T_CB_ARG arg) {
  
  long l;
I 7
  long pre[8];
  long long ll;
E 7
  double d;
E 4
D 6

E 6
I 6
  T_STR s;
I 7
  T_INT4 *trgd;
  T_INT4 len;
I 8

E 8
E 7
D 21
  time_t t = time(NULL)-epicsToLocalTime;
E 21
E 6
D 4
  while(done==0) {
    server.MainLoop((double)ipc_sleep_time);
  }  
  return((void*)NULL);
E 4
I 4
  TipcMsg msg(data->msg);
  
I 6

E 6
I 5
  callback_count++;
E 5

I 5

E 5
  // run number
D 5
  msg >> l;  pv[0]->myValue->put((aitInt32)l);
E 5
I 5
D 6
  //  msg >> l;  pv[0]->myValue->put((aitInt32)l);
E 6
I 6
D 7
  msg.Current(0);  msg >> l; fillPV(0,l,t);
E 7
I 7
  msg.Current(0);  
  msg >> l; 
D 8
  fillPV(0,l,t);
E 8
I 8
D 21
  fillPV(0,(int)l,t);
E 21
I 21
D 22
  pclasPV[0]->fillPV((int)l);
E 22
I 22
  pPV[0]->fillPV((int)l);
E 22
E 21
  run_number=l;
E 8
E 7
E 6
E 5

I 7

E 7
I 6
D 18
  // event count
D 7
  msg.Current(7);  msg >> l; fillPV(1,l,t);
E 7
I 7
  msg.Current(7);
E 18
I 18
  // event count (ER)
  msg.Current(10);
E 18
D 8
  msg >> l; 
  fillPV(1,l,t);
E 8
I 8
  msg >> l;
D 21
  fillPV(1,(int)l,t);
E 21
I 21
D 22
  pclasPV[1]->fillPV((int)l);
E 22
I 22
  pPV[1]->fillPV((int)l);
E 22
E 21
E 8
E 7
E 6

I 7

E 7
I 6
D 18
  // event rate
D 7
  msg.Current(8);  msg >> d; fillPV(2,d,t);
E 7
I 7
  msg.Current(8);
E 18
I 18
  // event rate (ER)
  msg.Current(11);
E 18
  msg >> d;
D 13
  fillPV(2,d,t);
E 13
I 13
D 21
  fillPV(2,(int)d,t);
E 21
I 21
D 22
  pclasPV[2]->fillPV((int)d);
E 22
I 22
  pPV[2]->fillPV((int)d);
E 22
E 21
E 13
E 7

I 7

E 7
  // livetime
I 7
  msg.Current(14);
  msg.Next(&trgd,&len);
  d=(trgd[19]>0)?(((double)trgd[35])/trgd[19]):0.0;
D 13
  fillPV(3,d,t);
E 13
I 13
D 21
  fillPV(3,(int)(100.0*d),t);
E 21
I 21
D 22
  pclasPV[3]->fillPV((int)(100.0*d));
E 22
I 22
  pPV[3]->fillPV((int)(100.0*d));
E 22
E 21
E 13
  
E 7

D 7
  // csr
  msg.Current(2);  msg >> l; fillPV(4,l,t);
E 7
I 7
  // csr and state
  msg.Current(1);
  msg >> ll;
D 8
  fillPV(4,(unsigned long)(ll&0xffffffff),t);
  fillPV(5,(unsigned long)(ll&0x1),t);
E 8
I 8
D 21
  fillPV(4,(unsigned int)(ll&0xffffffff),t);
  fillPV(5,(unsigned int)(ll&0x1),t);
E 21
I 21
D 22
  pclasPV[4]->fillPV((unsigned int)(ll&0xffffffff));
  pclasPV[5]->fillPV((unsigned int)(ll&0x1));
E 22
I 22
  pPV[4]->fillPV((unsigned int)(ll&0xffffffff));
  pPV[5]->fillPV((unsigned int)(ll&0x1));
E 22
E 21
E 8
E 7

D 7
  // state
E 7

  // trig enable
I 7
  msg.Current(4);
  msg >> ll;
D 8
  fillPV(6,(unsigned long)((ll>>1)&0xfff),t);
E 8
I 8
D 21
  fillPV(6,(unsigned int)((ll>>1)&0xfff),t);
E 21
I 21
D 22
  pclasPV[6]->fillPV((unsigned int)((ll>>1)&0xfff));
E 22
I 22
  pPV[6]->fillPV((unsigned int)((ll>>1)&0xfff));
E 22
E 21
E 8
E 7

I 7

E 7
  // roc enable
I 7
  msg.Current(6);
  msg >> ll;
D 8
  fillPV(7,(unsigned long)(ll&0xffffffff),t);
E 8
I 8
D 21
  fillPV(7,(unsigned int)(ll&0xffffffff),t);
E 21
I 21
D 22
  pclasPV[7]->fillPV((unsigned int)(ll&0xffffffff));
E 22
I 22
  pPV[7]->fillPV((unsigned int)(ll&0xffffffff));
E 22
E 21
E 8
E 7

D 7
  // prescales 1 to 8
  msg.Current(3);  msg >> s; 
  for(long i=0; i<8; i++) {
    // ??? parse string
    fillPV(8+i,i+1,t);
  }
E 7

E 6
D 7
  // run_number, csr, status, prescale, control, state, roc_enable
  // eb_evt, eb_evt_rate, eb_data_rate
  // er_evt, er_evt_rate, er_data_rate
  // str,trgs00, str,trgd00
E 7
I 7
  // 8 prescales
  msg.Current(3);
  msg >> s; 
  sscanf(s,"%ld %ld %ld %ld %ld %ld %ld %ld",
	 &pre[0],&pre[1],&pre[2],&pre[3],&pre[4],&pre[5],&pre[6],&pre[7]);
D 8
  for(long i=0; i<8; i++) fillPV(8+i,pre[i],t);
E 8
I 8
D 21
  for(long i=0; i<8; i++) fillPV(8+i,(int)pre[i],t);
E 21
I 21
D 22
  for(long i=0; i<8; i++) pclasPV[8+i]->fillPV((int)pre[i]);
E 22
I 22
  for(long i=0; i<8; i++) pPV[8+i]->fillPV((int)pre[i]);
E 22
E 21
E 8
E 7

I 18

  // data rate (ER)
  msg.Current(12);
  msg >> d;
D 21
  fillPV(32,(int)d,t);
E 21
I 21
D 22
  pclasPV[32]->fillPV((int)d);
E 22
I 22
  pPV[32]->fillPV((int)d);
E 22
E 21


E 18
  return;
E 4
}


D 4
//--------------------------------------------------------------------------
E 4
I 4
//------------------------------------------------------------------
E 4


I 6
D 8
void fillPV(int pvIndex, long l, time_t t) {
E 8
I 8
D 21
void fillPV(int pvIndex, int i, time_t t) {
E 8

D 7
  if(l!=pv[pvIndex]->myIValue) {
E 7
I 7
D 8
  if(l!=pv[pvIndex]->myLValue) {
E 8
I 8
D 14
  if(i!=pv[pvIndex]->myIValue) {
E 14
I 14
  if(i!=pclasPV[pvIndex]->myIValue) {
E 14
E 8
E 7
D 10
    clasUpdate[0]=1;
E 10
I 10
    clasUpdate[pvIndex]=1;
E 10
D 7
    pv[pvIndex]->myIValue=l;
E 7
I 7
D 8
    pv[pvIndex]->myLValue=l;
E 8
I 8
D 14
    pv[pvIndex]->myIValue=i;
E 8
E 7
    pv[pvIndex]->myTime=t;
E 14
I 14
    pclasPV[pvIndex]->myIValue=i;
    pclasPV[pvIndex]->myTime=t;
E 14
I 11
D 12
    if(clasAlarm[pvIndex]!=0) {
	if(i>clasHI[pvIndex]) {
	    pv[pvIndex]->myStat=epicsAlarmHiHi;
	    pv[pvIndex]->mySevr=epicsSevMajor;
	} else if (i>clasWHI[pvIndex]) {
	    pv[pvIndex]->myStat=epicsAlarmHigh;
	    pv[pvIndex]->mySevr=epicsSevMinor;
	} else if (i>clasWLO[pvIndex]) {
	    pv[pvIndex]->myStat=epicsAlarmNone;
	    pv[pvIndex]->mySevr=epicsSevNone;
	} else if (i>clasLO[pvIndex]) {
	    pv[pvIndex]->myStat=epicsAlarmHigh;
	    pv[pvIndex]->mySevr=epicsSevMinor;
	} else {
	    pv[pvIndex]->myStat=epicsAlarmHiHi;
	    pv[pvIndex]->mySevr=epicsSevMajor;
	}
    }
E 12
E 11
  }
I 12

D 14
  setAlarm(pvIndex,i);
E 14
I 14
  setAlarm(pvIndex);
E 14
  
E 12
I 7
}


//------------------------------------------------------------------


D 8
void fillPV(int pvIndex, unsigned long ul, time_t t) {
E 8
I 8
void fillPV(int pvIndex, unsigned int ui, time_t t) {
E 8

D 8
  if(ul!=pv[pvIndex]->myULValue) {
E 8
I 8
D 14
  if(ui!=pv[pvIndex]->myUIValue) {
E 14
I 14
  if(ui!=pclasPV[pvIndex]->myUIValue) {
E 14
E 8
D 10
    clasUpdate[0]=1;
E 10
I 10
    clasUpdate[pvIndex]=1;
E 10
D 8
    pv[pvIndex]->myULValue=ul;
E 8
I 8
D 14
    pv[pvIndex]->myUIValue=ui;
E 8
    pv[pvIndex]->myTime=t;
E 14
I 14
    pclasPV[pvIndex]->myUIValue=ui;
    pclasPV[pvIndex]->myTime=t;
E 14
I 11
D 12
    if(clasAlarm[pvIndex]!=0) {
	if(ui>clasHI[pvIndex]) {
	    pv[pvIndex]->myStat=epicsAlarmHiHi;
	    pv[pvIndex]->mySevr=epicsSevMajor;
	} else if (ui>clasWHI[pvIndex]) {
	    pv[pvIndex]->myStat=epicsAlarmHigh;
	    pv[pvIndex]->mySevr=epicsSevMinor;
	} else if (ui>clasWLO[pvIndex]) {
	    pv[pvIndex]->myStat=epicsAlarmNone;
	    pv[pvIndex]->mySevr=epicsSevNone;
	} else if (ui>clasLO[pvIndex]) {
	    pv[pvIndex]->myStat=epicsAlarmHigh;
	    pv[pvIndex]->mySevr=epicsSevMinor;
	} else {
	    pv[pvIndex]->myStat=epicsAlarmHiHi;
	    pv[pvIndex]->mySevr=epicsSevMajor;
	}
    }
E 12
E 11
  }
I 12

D 14
  setAlarm(pvIndex,(int)ui);
E 14
I 14
  setAlarm(pvIndex);
E 14

E 12
E 7
}


//------------------------------------------------------------------


void fillPV(int pvIndex, double d, time_t t) {

D 14
  if(d!=pv[pvIndex]->myDValue) {
E 14
I 14
  if(d!=pclasPV[pvIndex]->myDValue) {
E 14
D 10
    clasUpdate[0]=1;
E 10
I 10
    clasUpdate[pvIndex]=1;
E 10
D 14
    pv[pvIndex]->myDValue=d;
    pv[pvIndex]->myTime=t;
E 14
I 14
    pclasPV[pvIndex]->myDValue=d;
    pclasPV[pvIndex]->myTime=t;
E 14
I 12
  }

D 14
  setAlarm(pvIndex,(int)d);
E 14
I 14
  setAlarm(pvIndex);
E 14

}


//------------------------------------------------------------------


D 14
void setAlarm(int pvIndex, int ival) {
E 14
I 14
void setAlarm(int pvIndex) {
E 14

D 14
    int oldStat = pv[pvIndex]->myStat;
    int oldSevr = pv[pvIndex]->mySevr;
E 14
I 14
    int ival;
    int oldStat = pclasPV[pvIndex]->myStat;
    int oldSevr = pclasPV[pvIndex]->mySevr;
E 14

I 14

    if(clasType[pvIndex]==aitEnumInt32) {
	ival=pclasPV[pvIndex]->myIValue;

    } else if (clasType[pvIndex]==aitEnumUint32) {
	ival=(int)pclasPV[pvIndex]->myUIValue;

    } else if (clasType[pvIndex]==aitEnumFloat64) {
	ival=(int)pclasPV[pvIndex]->myDValue;

    } else {
      cerr << "checkalarm...unknown ait type " << clasType[pvIndex] << endl;
      return;
    }


E 14
E 12
I 11
    if(clasAlarm[pvIndex]!=0) {
D 12
	if(d>clasHI[pvIndex]) {
E 12
I 12
	if(ival>clasHI[pvIndex]) {
E 12
D 14
	    pv[pvIndex]->myStat=epicsAlarmHiHi;
	    pv[pvIndex]->mySevr=epicsSevMajor;
E 14
I 14
	    pclasPV[pvIndex]->myStat=epicsAlarmHiHi;
	    pclasPV[pvIndex]->mySevr=epicsSevMajor;
E 14
D 12
	} else if (d>clasWHI[pvIndex]) {
E 12
I 12
	} else if (ival>clasWHI[pvIndex]) {
E 12
D 14
	    pv[pvIndex]->myStat=epicsAlarmHigh;
	    pv[pvIndex]->mySevr=epicsSevMinor;
E 14
I 14
	    pclasPV[pvIndex]->myStat=epicsAlarmHigh;
	    pclasPV[pvIndex]->mySevr=epicsSevMinor;
E 14
D 12
	} else if (d>clasWLO[pvIndex]) {
E 12
I 12
D 17
	} else if (ival>clasWLO[pvIndex]) {
E 17
I 17
	} else if (ival>=clasWLO[pvIndex]) {
E 17
E 12
D 14
	    pv[pvIndex]->myStat=epicsAlarmNone;
	    pv[pvIndex]->mySevr=epicsSevNone;
E 14
I 14
	    pclasPV[pvIndex]->myStat=epicsAlarmNone;
	    pclasPV[pvIndex]->mySevr=epicsSevNone;
E 14
D 12
	} else if (d>clasLO[pvIndex]) {
E 12
I 12
D 17
	} else if (ival>clasLO[pvIndex]) {
E 12
D 14
	    pv[pvIndex]->myStat=epicsAlarmHigh;
	    pv[pvIndex]->mySevr=epicsSevMinor;
E 14
I 14
	    pclasPV[pvIndex]->myStat=epicsAlarmHigh;
E 17
I 17
	} else if (ival>=clasLO[pvIndex]) {
	    pclasPV[pvIndex]->myStat=epicsAlarmLow;
E 17
	    pclasPV[pvIndex]->mySevr=epicsSevMinor;
E 14
	} else {
D 14
	    pv[pvIndex]->myStat=epicsAlarmHiHi;
	    pv[pvIndex]->mySevr=epicsSevMajor;
E 14
I 14
D 17
	    pclasPV[pvIndex]->myStat=epicsAlarmHiHi;
E 17
I 17
	    pclasPV[pvIndex]->myStat=epicsAlarmLoLo;
E 17
	    pclasPV[pvIndex]->mySevr=epicsSevMajor;
E 14
	}
I 12
	
    } else {
D 14
	pv[pvIndex]->myStat=epicsAlarmNone;
	pv[pvIndex]->mySevr=epicsSevNone;
E 14
I 14
	pclasPV[pvIndex]->myStat=epicsAlarmNone;
	pclasPV[pvIndex]->mySevr=epicsSevNone;
E 14
E 12
    }
E 11
D 12
  }
E 12
I 12
    

    // force update if alarm state changed
D 14
    if((oldStat!=pv[pvIndex]->myStat)||(oldSevr!=pv[pvIndex]->mySevr))clasUpdate[pvIndex]=1;
E 14
I 14
    if((oldStat!=pclasPV[pvIndex]->myStat)||(oldSevr!=pclasPV[pvIndex]->mySevr))
	clasUpdate[pvIndex]=1;
E 14

E 12
}


//------------------------------------------------------------------


E 21
E 6
E 3
void status_poll_callback(T_IPC_MSG msg) {

  
I 5
  TipcMsgAppendStr(msg,(char*)"number of epics channels served");
D 21
  TipcMsgAppendInt4(msg,sizeof(pvnames)/sizeof(char*));
E 21
I 21
  TipcMsgAppendInt4(msg,sizeof(pvNames)/sizeof(char*));
E 21
  
E 5
  TipcMsgAppendStr(msg,(char*)"number of epics reads");
D 3
  TipcMsgAppendInt4(msg,???);
E 3
I 3
D 5
  TipcMsgAppendInt4(msg,0);
E 5
I 5
  TipcMsgAppendInt4(msg,read_count);
E 5
E 3
  
D 5
  TipcMsgAppendStr(msg,(char*)"");
  TipcMsgAppendStr(msg,(char*)"");
E 5
I 5
  TipcMsgAppendStr(msg,(char*)"number of smartsockets callbacks");
  TipcMsgAppendInt4(msg,callback_count);
  
E 5

D 5

E 5
  return;
}


//-------------------------------------------------------------------


void quit_callback(int sig) {

  done=1;

  return;
}


//-------------------------------------------------------------------


I 3
D 4
void scaler_callback(T_IPC_CONN conn,
		     T_IPC_CONN_PROCESS_CB_DATA data,
		     T_CB_ARG arg) {
  
  T_IPC_MSG msg = data->msg;
  
  // ???

  return;
}


//------------------------------------------------------------------


E 4
E 3
void init_tcl() {

  // link c and Tcl variables
  Tcl_LinkVar(interp,(char*)"application",    	   (char *)&application,      	TCL_LINK_STRING);
  Tcl_LinkVar(interp,(char*)"unique_id",     	   (char *)&unique_id,          TCL_LINK_STRING);
D 5
  Tcl_LinkVar(interp,(char*)"session",     	   (char *)&session,            TCL_LINK_STRING);
E 5
  Tcl_LinkVar(interp,(char*)"debug",     	   (char *)&debug,              TCL_LINK_BOOLEAN);
D 3


E 3
I 3
  
  
E 3
  // create Tcl commands
  Tcl_CreateCommand(interp,(char*)"help",tcl_help,
  		    (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
  Tcl_CreateCommand(interp,(char*)"quit",tcl_quit,
		    (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
  Tcl_CreateCommand(interp,(char*)"stop",tcl_quit,
		    (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
  Tcl_CreateCommand(interp,(char*)"exit",tcl_quit,
		    (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);

  return;

}


//--------------------------------------------------------------------------


void decode_command_line(int argc, char**argv) {

D 5
  const char *help = "\nusage:\n\n  epics_server [-a application] [-s session] [-u unique_id]\n"
E 5
I 5
D 6
  const char *help = "\nusage:\n\n  epics_server [-a application] [-u unique_id]\n"
E 5
D 3
    "        [-t init_tcl_script] [-m msql_database] [-no_ipc] [-debug]\n";
E 3
I 3
    "        [-t init_tcl_script] [-m msql_database] [-debug]\n";
E 6
I 6
  const char *help = "\nusage:\n\n epics_server [-a application] [-u unique_id]\n"
D 8
    "              [-ca ca_pend_time] [-ipc ipc_pend_time] [-data data_pend_time]\n"
E 8
I 8
    "              [-ca ca_pend_time] [-ipc ipc_pend_time] [-data data_sleep_time]\n"
E 8
D 24
    "              [-t init_tcl_script] [-m msql_database] [-debug]\n";
E 24
I 24
    "              [-t init_tcl_script] [-debug]\n";
E 24
E 6
E 3


  // loop over all arguments, except the 1st (which is program name)
  int i=1;
  while(i<argc) {
    if(strncasecmp(argv[i],"-h",2)==0) {
      cout << help << endl;
      exit(EXIT_SUCCESS);
    }
    else if (strncasecmp(argv[i],"-debug",6)==0) {
      debug=1;
      i=i+1;
    }
I 6
    else if (strncasecmp(argv[i],"-ca",3)==0) {
      ca_pend_time=atoi(argv[i+1]);
      i=i+2;
    }
    else if (strncasecmp(argv[i],"-ipc",4)==0) {
      ipc_pend_time=atoi(argv[i+1]);
      i=i+2;
    }
    else if (strncasecmp(argv[i],"-data",4)==0) {
D 8
      data_pend_time=atoi(argv[i+1]);
E 8
I 8
      data_sleep_time=atoi(argv[i+1]);
E 8
      i=i+2;
    }
E 6
    else if (strncasecmp(argv[i],"-a",2)==0) {
      application=strdup(argv[i+1]);
      i=i+2;
    }
    else if (strncasecmp(argv[i],"-u",2)==0) {
      unique_id=strdup(argv[i+1]);
      i=i+2;
    }
D 5
    else if (strncasecmp(argv[i],"-s",2)==0) {
      session=strdup(argv[i+1]);
      i=i+2;
    }
E 5
    else if (strncasecmp(argv[i],"-t",2)==0) {
      init_tcl_script=strdup(argv[i+1]);
      i=i+2;
    }
D 24
    else if (strncasecmp(argv[i],"-m",2)==0) {
      msql_database=strdup(argv[i+1]);
      i=i+2;
    }
E 24
    else if (strncasecmp(argv[i],"-",1)==0) {
      cout << "Unknown command line arg: " << argv[i] << argv[i+1] << endl << endl;
      i=i+1;
    }
  }

  return;
}

  
//----------------------------------------------------------------


int tcl_help(ClientData clientdata, Tcl_Interp *interp,
		int argc, char **argv) {

    const char *help =
    "\nTcl commands available in the epics_server program:\n\n"
    " help                  print this message\n"
    " stop                  stop program\n"
    " quit                  stop program\n"
    " exit                  stop program\n"
    "\n\n Type command that require args with NO args for more information\n"
    "\n";

    Tcl_SetResult(interp,(char*)help,TCL_STATIC);

  return (TCL_OK);

}


//---------------------------------------------------------------------


int tcl_quit(ClientData clientdata, Tcl_Interp *interp,
	     int argc, char **argv) {
  
  done=1;
  
  return (TCL_OK);
}


//---------------------------------------------------------------------


E 27
I 27
//---------------------------------------------------------------------------
E 27
E 1
