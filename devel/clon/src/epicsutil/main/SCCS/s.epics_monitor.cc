h39852
s 00050/00009/01280
d D 1.120 10/03/19 12:18:31 boiarino 121 120
c enforce all ENUM types to be reported as integers rather then strings
c 
e
s 00042/00007/01247
d D 1.119 07/11/07 16:25:48 boiarino 120 119
c *** empty log message ***
e
s 00008/00327/01246
d D 1.118 07/11/07 13:24:25 boiarino 119 118
c add 4 new scalers to scaler_thread()
c 
e
s 00248/00052/01325
d D 1.117 07/10/16 23:36:48 boiarino 118 117
c add gamma_monitor
c 
e
s 00001/00001/01376
d D 1.116 07/10/16 21:32:20 boiarino 117 116
c *** empty log message ***
e
s 00029/00045/01348
d D 1.115 07/10/16 21:24:57 boiarino 116 115
c add ipc_server
c 
e
s 00013/00310/01380
d D 1.114 07/10/16 19:57:46 boiarino 115 114
c remove cdev stuff; still need more work
c 
e
s 00253/00126/01437
d D 1.113 07/10/16 18:08:08 boiarino 114 113
c *** empty log message ***
e
s 00100/00044/01463
d D 1.112 07/10/16 04:33:09 boiarino 113 112
c *** empty log message ***
e
s 00787/00260/00720
d D 1.111 07/10/15 22:43:32 boiarino 112 111
c use CA instead of cdev - first CA-based version, probably need fixes,
c definitely needs cleanup from cdev stuff
c 
e
s 00006/00004/00974
d D 1.110 07/10/12 10:20:05 boiarino 111 110
c *** empty log message ***
e
s 00003/00001/00975
d D 1.109 07/10/12 09:57:58 boiarino 110 109
c *** empty log message ***
e
s 00001/00001/00975
d D 1.108 06/11/28 10:24:09 boiarino 109 108
c *** empty log message ***
e
s 00000/00000/00976
d D 1.107 06/02/15 15:18:14 boiarino 108 107
c *** empty log message ***
e
s 00002/00002/00974
d D 1.106 05/06/19 23:20:28 fklein 107 106
c epics_monitor didn't find new Tagger E counter 'jscaler' record
c 
e
s 00014/00009/00962
d D 1.105 05/06/19 22:11:00 fklein 106 105
c change to new Tagger E-counter scalers in epics
c 
e
s 00002/00002/00969
d D 1.104 01/07/17 11:45:39 wolin 105 104
c Minor mods
e
s 00002/00140/00969
d D 1.103 01/07/17 11:21:46 wolin 104 103
c Removed bpm stuff, moved to gamma_monitor
e
s 00015/00004/01094
d D 1.102 01/07/17 10:32:14 wolin 103 102
c Checking status of bpm requests, other minor mods
e
s 00004/00002/01094
d D 1.101 01/07/16 18:53:01 fklein 102 101
c add channels for photon running
e
s 00008/00004/01088
d D 1.100 01/07/14 05:57:58 wolin 101 100
c Added 4 collimator channels to fast readout
e
s 00018/00011/01074
d D 1.99 01/07/14 02:30:52 wolin 100 99
c Switched to clock_gettime for BPM every 1/2 second
e
s 00116/00004/00969
d D 1.98 01/07/14 01:22:17 wolin 99 98
c Added bpm_thread, runs sort of slow...
e
s 00016/00013/00957
d D 1.97 01/07/09 14:12:22 wolin 98 97
c Processes scalers even if some do not respond
e
s 00001/00001/00969
d D 1.96 01/07/03 13:42:36 wolin 97 96
c Missing cast to double in pend caused hangs in scaler_thread (I hope)
e
s 00046/00019/00924
d D 1.95 01/06/19 16:15:56 wolin 96 95
c Added new epics scalers
e
s 00013/00006/00930
d D 1.94 01/05/29 10:59:24 wolin 95 94
c Added no_scalers
e
s 00003/00003/00933
d D 1.93 01/05/29 10:46:31 wolin 94 93
c e_scalers now working
e
s 00009/00005/00927
d D 1.92 01/02/08 14:37:48 wolin 93 92
c Seems to work with cdev 1.7.4
e
s 00002/00002/00930
d D 1.91 01/01/03 10:46:27 wolin 92 91
c is_open()
e
s 00064/00062/00868
d D 1.90 00/10/26 14:46:57 wolin 91 90
c const
e
s 00001/00005/00929
d D 1.89 00/10/12 12:30:34 wolin 90 89
c No epics data in datastream since vsn 1.79, 11-nov-99
e
s 00001/00000/00933
d D 1.88 00/10/12 12:27:14 wolin 89 88
c Major bug...epics data not sent to datastream correctly...all vals zero!
e
s 00007/00007/00926
d D 1.87 00/09/08 13:07:08 wolin 88 87
c New clon_root
e
s 00053/00137/00880
d D 1.86 00/05/26 10:57:34 wolin 87 86
c Now uses cfg file
e
s 00013/00000/01004
d D 1.85 00/04/13 10:30:59 wolin 86 85
c Added channels for E5
e
s 00001/00001/01003
d D 1.84 00/01/19 13:28:55 wolin 85 84
c No more /group/clas/online
e
s 00001/00001/01003
d D 1.83 00/01/18 12:28:05 wolin 84 83
c Removed helicity file
e
s 00003/00004/01001
d D 1.82 00/01/12 13:18:42 wolin 83 82
c Disabled scaler thread...only used for photon runs
e
s 00010/00008/00995
d D 1.81 99/11/10 15:50:17 wolin 82 81
c Forgot to extract data!
e
s 00262/00177/00741
d D 1.80 99/11/10 15:34:18 wolin 81 80
c Added TESC bank and reorganized code
e
s 00001/00000/00917
d D 1.79 99/11/03 10:32:53 wolin 80 79
c Added hallb helicity
e
s 00009/00009/00908
d D 1.78 99/09/09 15:14:00 wolin 79 78
c IPM2C22A renamed to IPM2C21A
c 
e
s 00006/00006/00911
d D 1.77 99/06/03 11:25:27 wolin 78 77
c Added insert_msg
c 
e
s 00001/00001/00916
d D 1.76 99/05/13 09:53:08 wolin 77 76
c Increased pend to 8 secs
c 
e
s 00013/00009/00904
d D 1.75 99/04/14 11:40:13 wolin 76 75
c Minor debug mods
c 
e
s 00003/00000/00910
d D 1.74 99/04/13 15:49:33 wolin 75 74
c Added 3 tgt channels
c 
e
s 00014/00004/00896
d D 1.73 99/04/05 12:14:27 wolin 74 73
c Added Hall A,C and additional Hall B info
c 
e
s 00033/00036/00867
d D 1.72 99/03/09 15:58:35 wolin 73 72
c Added cryo channels
c 
e
s 00030/00028/00873
d D 1.71 99/03/09 14:28:31 wolin 72 71
c Updated to work with et
e
s 00001/00000/00900
d D 1.70 99/01/21 09:47:24 wolin 71 70
c Added beam phase
c 
e
s 00004/00004/00896
d D 1.69 99/01/19 16:49:37 wolin 70 69
c Reenabled MT channels
c 
e
s 00001/00001/00899
d D 1.68 99/01/06 09:41:02 wolin 69 68
c Debug statements
c 
e
s 00045/00037/00855
d D 1.67 98/12/22 12:38:04 wolin 68 67
c Redid attach/detach stratagy to fix IOCHLB problem
c 
e
s 00091/00087/00801
d D 1.66 98/12/15 10:30:35 wolin 67 66
c Only detach if not attached, minor code reorganization
c 
e
s 00004/00002/00884
d D 1.65 98/12/11 11:00:15 wolin 66 65
c Minor mod to solve problem in Accelerator IOC
c 
e
s 00010/00004/00876
d D 1.64 98/10/23 17:05:03 wolin 65 64
c More output format improvements for channel name
c 
e
s 00016/00004/00864
d D 1.63 98/10/23 17:00:26 wolin 64 63
c Improved channel name output format
c 
e
s 00024/00006/00844
d D 1.62 98/10/23 10:39:06 wolin 63 62
c Added moller channels
c 
e
s 00004/00003/00846
d D 1.61 98/10/22 13:04:57 wolin 62 61
c 32nd char is null in epics name
c 
e
s 00036/00001/00813
d D 1.60 98/10/14 15:06:35 wolin 61 60
c Added helicity file, no run status
c 
e
s 00016/00004/00798
d D 1.59 98/09/25 11:42:42 wolin 60 59
c Attach and detach each time
c 
e
s 00009/00000/00793
d D 1.58 98/09/09 14:19:45 wolin 59 58
c Added bpm scales
c 
e
s 00008/00002/00785
d D 1.57 98/06/12 11:27:10 wolin 58 57
c Trailing blanks in EPIC bank
c 
e
s 00001/00001/00786
d D 1.56 98/06/02 16:31:56 wolin 57 56
c 9 cols, not 2
c 
e
s 00002/00002/00785
d D 1.55 98/05/26 13:40:46 wolin 56 55
c I think it is working...
c 
e
s 00027/00298/00760
d D 1.54 98/05/22 15:36:33 wolin 55 54
c Completely revised to use ipcbank2dd
c 
e
s 00001/00001/01057
d D 1.53 98/05/18 16:54:57 wolin 54 53
c Added extra rate
c 
e
s 00007/00006/01051
d D 1.52 98/05/18 16:44:57 wolin 53 52
c New dd_status
c 
e
s 00010/00000/01047
d D 1.51 98/05/12 11:33:59 wolin 52 51
c Added harp, improved status poll info
c 
e
s 00010/00004/01037
d D 1.50 98/05/07 10:24:06 wolin 51 50
c Added vacuum channels
c 
e
s 00020/00007/01021
d D 1.49 98/05/06 10:58:11 wolin 50 49
c Added cdev_pend_repeat
c 
e
s 00043/00065/00985
d D 1.48 98/05/04 14:29:07 wolin 49 48
c No once_only, enabled auto-restart if lost_connnection
c 
e
s 00079/00042/00971
d D 1.47 98/05/01 11:24:36 wolin 48 47
c Testing detach
c 
e
s 00021/00012/00992
d D 1.46 98/04/30 17:36:25 wolin 47 46
c Added restart option
c 
e
s 00146/00135/00858
d D 1.45 98/04/30 17:23:21 wolin 46 45
c Added once_only
c 
e
s 00002/00002/00991
d D 1.44 98/03/30 17:20:16 wolin 45 44
c Doc mods
c 
e
s 00002/00000/00991
d D 1.43 98/03/30 16:35:29 wolin 44 43
c Added tagger and mt readbacks
c 
e
s 00001/00001/00990
d D 1.42 98/03/23 17:15:50 wolin 43 42
c Typo
c 
e
s 00002/00001/00989
d D 1.41 98/03/23 16:41:54 wolin 42 41
c Added host
c 
e
s 00010/00002/00980
d D 1.40 98/03/17 13:09:30 wolin 41 40
c Added nev_no_run, other minor mods
c 
e
s 00002/00000/00980
d D 1.39 98/03/17 12:53:31 wolin 40 39
c Added error msg when dd connection lost
c 
e
s 00005/00002/00975
d D 1.38 98/03/13 11:55:05 wolin 39 38
c Additional bombproofing
c 
e
s 00068/00068/00909
d D 1.37 98/03/11 15:21:29 wolin 38 37
c Improved labels
c 
e
s 00001/00000/00976
d D 1.36 98/03/11 14:55:11 wolin 37 36
c Added dd_reconnect to tcl
c 
e
s 00003/00012/00973
d D 1.35 98/03/11 11:25:08 wolin 36 35
c Removed unneeded cdev_retry_time
c 
e
s 00013/00001/00972
d D 1.34 98/03/11 10:01:55 wolin 35 34
c Additional bombproofing, added dd_xxx to tcl
c 
e
s 00001/00001/00972
d D 1.33 98/03/10 15:09:01 wolin 34 33
c Default pend time now 3 sec
c 
e
s 00001/00000/00972
d D 1.32 98/03/10 13:12:52 wolin 33 32
c Added dd_ok to tcl
c 
e
s 00001/00000/00971
d D 1.31 98/03/10 12:19:10 wolin 32 31
c Using dd_control subject
c 
e
s 00092/00036/00879
d D 1.30 98/03/09 13:32:36 wolin 31 30
c Added dd_reconnect logic, other minor mods
c 
e
s 00025/00001/00890
d D 1.29 98/03/06 14:12:50 wolin 30 29
c Using 0,0,0,0 for event control words
c 
e
s 00008/00015/00883
d D 1.28 98/02/24 17:00:06 wolin 29 28
c Minor dd mods
c 
e
s 00014/00013/00884
d D 1.27 98/02/23 15:08:10 wolin 28 27
c Using new dd death detection and recovery
c 
e
s 00049/00041/00848
d D 1.26 98/02/19 15:27:50 wolin 27 26
c Implemented new recovery scheme using monitorOn status
c 
e
s 00006/00004/00883
d D 1.25 98/02/10 12:09:16 wolin 26 25
c New bpm channels, other minor mods
c 
e
s 00003/00001/00884
d D 1.24 98/02/09 16:51:50 wolin 25 24
c Added hall a,c slits
c 
e
s 00009/00009/00876
d D 1.23 98/02/09 16:49:01 wolin 24 23
c Added bpm channels
c 
e
s 00003/00002/00882
d D 1.22 98/02/05 12:08:38 wolin 23 22
c Using double array for info server message
c 
e
s 00001/00001/00883
d D 1.21 98/02/05 12:00:03 wolin 22 21
c Minor mod to info server message format
c 
e
s 00013/00002/00871
d D 1.20 98/01/22 11:54:20 wolin 21 20
c Env vars
c 
e
s 00043/00022/00830
d D 1.19 97/12/23 16:47:59 wolin 20 19
c Implemented summary file
c 
e
s 00002/00000/00850
d D 1.18 97/12/10 05:56:13 wolin 19 18
c Forgot to count nev_to_dd
c 
e
s 00009/00005/00841
d D 1.17 97/12/10 05:30:03 wolin 18 17
c Doesn't die if no DD at startup
c 
e
s 00109/00072/00737
d D 1.16 97/11/26 17:01:51 wolin 17 16
c Improved i/o, still working on dd death recovery
c 
e
s 00003/00003/00806
d D 1.15 97/11/19 09:28:15 wolin 16 15
c New head bank scheme
c 
e
s 00037/00010/00772
d D 1.14 97/11/17 16:09:58 wolin 15 14
c create_header args now include name1,name2
c 
e
s 00022/00000/00760
d D 1.13 97/10/24 16:33:07 wolin 14 13
c Added tcl dump command
c 
e
s 00003/00030/00757
d D 1.12 97/10/24 16:12:22 wolin 13 12
c DC gas channels commented out, otherwise ready for production
c 
e
s 00031/00030/00756
d D 1.11 97/09/30 13:12:22 wolin 12 11
c Added force dd flag
c 
e
s 00005/00002/00781
d D 1.10 97/09/24 16:11:35 wolin 11 10
c Now checking for dd death
c 
e
s 00065/00060/00718
d D 1.9 97/09/19 12:33:45 wolin 10 9
c Reordered to ensure callback is used
c 
e
s 00059/00042/00719
d D 1.8 97/09/09 14:29:12 wolin 9 8
c Removed torus voltage
c 
e
s 00054/00059/00707
d D 1.7 97/06/13 17:12:44 wolin 8 7
c get_run_status enabled (debug vsn), still some dummy channels
e
s 00175/00080/00591
d D 1.6 97/06/13 12:50:36 wolin 7 6
c clasgas channels disabled
e
s 00004/00004/00667
d D 1.5 97/05/21 15:24:49 wolin 6 5
c Works with new cmlog
e
s 00106/00065/00565
d D 1.4 97/05/21 10:42:22 wolin 5 4
c New cdev,cmlog, no cdevUserFdCallback yet...
e
s 00027/00001/00603
d D 1.3 97/05/15 14:02:15 wolin 4 3
c Added info_server message
e
s 00028/00008/00576
d D 1.2 97/05/15 13:25:38 wolin 3 1
c Needs final channel names and insert_msg
e
s 00000/00000/00000
d R 1.2 97/05/15 13:04:23 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 2 1 epicsutil/s/epics_monitor.cc
c Name history : 1 0 s/epics_monitor.cc
e
s 00584/00000/00000
d D 1.1 97/05/15 13:04:22 wolin 1 0
c Inserts epics data into data stream periodically
e
u
U
f e 0
t
T
I 118
// 
//  epics_monitor.cc
//
//  inserts epics data into data stream periodically
//  broadcasts data to info_server
//  automatically reconnects if cdev channel drops out
//
//  ejw, 12-oct-2000
//
/* Sergey: on October 16, 2007 original cdev-based Elliott Wolin's code was changed
 to CA-based; catools/camonitor.c was used as an prototype, some parts of it were
 copied and modified */
E 118
I 112
D 115
#undef USECDEV
E 115

I 115
D 118
/* epics_monitor.cc */

E 115
/* see catools/camonitor.c as an example howto process without cdev */

/* ca functions: */

#ifdef NNNUUU
void *ca_puser(CHID); /* macros: */
unsigned ca_element_count(CHID); /* macros: */
chtype ca_field_type(CHID); /* macros: */
 ca_create_subscription();
 ca_context_create(ca_disable_preemptive_callback);
char *ca_message(STATUS); /* macros: */
int ca_pend_event(double timeout); /*Flushes the send buffer and waits for asynchronous
   events for TIMEOUT seconds (TIMEOUT of zero is forever). This routine will not
   return before the time-out expires and all unfinished channel access labor has been
   processed. */
void ca_context_destroy(); /*Flushes the send buffer and waits for asynchronous events
   for TIMEOUT seconds (TIMEOUT of zero is forever). This routine will not return
   before the time-out expires and all unfinished channel access labor has been
   processed.*/
#endif

D 115
#ifndef USECDEV
E 115

I 115

E 118
E 115
#include <stdio.h>
#include <epicsStdlib.h>
#include <string.h>
D 119
#define epicsAlarmGLOBAL
#include <alarm.h>
#undef epicsAlarmGLOBAL
E 119
I 119

E 119
#include <cadef.h>
#include <epicsGetopt.h>

D 118
#include "tool_lib.h"
E 118
I 118
#include "tool_lib_1.h"
E 118

D 119
/* Time stamps for program start, previous value (used for relative resp.
 * incremental times with monitors) */
static epicsTimeStamp tsStart, tsPrevious;
E 119

D 119
static int tsInit = 0;               /* Flag: Timestamps init'd */

TimeT tsType = absolute;             /* Timestamp type flag (-riI options) */
IntFormatT outType = dec_;            /* For -0.. output format option */

char dblFormatStr[30] = "%g"; /* Format string to print doubles (-efg options) */
char timeFormatStr[30] = "%Y-%m-%d %H:%M:%S.%06f"; /* Time format string */

int enumAsNr = 0;        /* used for -n option - get DBF_ENUM as number */
double caTimeout = 1.0;  /* wait time default (see -w option) */

#define TIMETEXTLEN 28   /* Length of timestamp text buffer */
E 119
#define VALSTRLEN 128    /* Length of value in a form of string */

D 119

E 119
#define VALID_DOUBLE_DIGITS 18  /* Max usable precision for a double */

D 121
static unsigned long reqElems = 0;
E 121
I 121

/*sergey: reqElems cannot be global variable, otherwise first channel will set it to
non-zero value, and 'connection_handler' will not take right value from nElems */
#define REQELEMS 0
/*static unsigned long reqElems = 0;*/


E 121
static unsigned long eventMask = DBE_VALUE | DBE_ALARM;   /* Event mask used */
static int floatAsString = 0;                             /* Flag: fetch floats as string */
static int nConn = 0;                                     /* Number of connected PVs */

D 115
#endif
E 115

D 118



E 112
I 1
// 
//  epics_monitor
//
I 49
D 55
//  stops and restarts if dd connection lost
//
E 49
//  inserts epics data into data stream periodically
I 4
//  also broadcasts data to info_server
E 55
I 55
D 72
//  inserts epics data into data stream periodically via ipcbank2dd
// broadcasts data to info_server
E 72
I 72
//  inserts epics data into data stream periodically
//  broadcasts data to info_server
E 72
E 55
I 12
D 17
//  stops if dd system disappears
E 17
I 15
D 36
//  if cdev channel drops out, attempts reconnection every cdev_retry_time seconds
E 36
I 36
//  automatically reconnects if cdev channel drops out
E 36
E 15
E 12
E 4
//
I 7
D 90
//
E 7
I 5
//  still to do:
I 48
D 83
//    enable cmlog?...causes epics to fail?
E 48
I 7
D 27
//    enable all channels
I 15
D 17
//    how to recreate request objects upon failure ??? 
E 17
I 17
//    recover from channel failure
D 18
//    no death on first init_dd, careful of signals
E 18
I 18
//    careful of signals
//    get stuck in "go" if run ends improperly
E 27
I 27
//    enable dc channels?
E 83
I 83
//    enable scaler thread for photon runs
E 83
E 27
E 18
E 17
E 15
E 7
D 9
//    use cdevUserFdxxx when cdev 1.6 arrives
E 9
I 7
D 12
//    what if DD dies while program is running?
//    is halo up/downstream backwards?
E 12
E 7
//
I 7
D 27
//
E 7
E 5
D 12
//  ejw, 15-may-97
E 12
I 12
D 17
//  ejw, 30-sep-97
E 17
I 17
//  ejw, 26-nov-97
E 27
I 27
D 49
//  ejw, 19-feb-98
E 49
I 49
D 55
//  ejw, 4-may-98
E 55
I 55
D 72
//  ejw, 22-may-98
E 72
I 72
D 83
//  ejw, 17-feb-99
E 83
I 83
//  ejw, 5-jan-2000
E 90
I 90
//  ejw, 12-oct-2000
E 90
E 83
E 72
E 55
E 49
E 27
E 17
E 12


E 118
// for posix
#define _POSIX_SOURCE_ 1
#define __EXTENSIONS__

I 4

I 87
D 109
#define MAX_EPICS 200
E 109
I 109
#define MAX_EPICS 250
I 114
#define MAX_CHANNELS_LENGTH 1000
E 114
E 109


E 87
E 4
D 55
// dd buffer size
D 7
#define DDBUFFERSIZE  1000     // longwords
E 7
I 7
#define DDBUFFERSIZE  4000     // longwords
E 7


E 55
// for smartsockets
#include <rtworks/cxxipc.hxx>


I 4
// CLAS ipc
#include <clas_ipc_prototypes.h>


E 4
D 81
// for i/o
E 81
I 81
// misc
I 110
using namespace std;
#include <strstream>

E 110
#include <pthread.h>
E 81
#include <fstream.h>
#include <iomanip.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
D 110
#include <strstream.h>
E 110
I 58
D 111
#include <macros.h>
E 111
E 58


D 112
// for tcl
extern "C" {
#include <tcl.h>
Tcl_Interp *tclinterp_init(void);
void tclipc_init(Tcl_Interp *interp);
}


E 112
I 112
D 115
#ifdef USECDEV
E 112
D 5
// for CLAS ipc
#include <clas_ipc_prototypes.h>


E 5
D 55
// for dd
extern "C"{
I 7
#include <dd_dcom.h>
E 7
#include <dd_user.h>
}

E 55
// for cdev
#include <cdev.h>
#include <cdevData.h>
#include <cdevDevice.h>
#include <cdevRequestObject.h>
#include <cdevSystem.h>
I 112
#endif
E 112

E 115
I 111
#define MIN(a,b)  ( (a) < (b) ? (a) : (b) )
#define MAX(a,b)  ( (a) > (b) ? (a) : (b) )
E 111

I 111

E 111
D 87
// epics channel names, etc.
E 87
I 87
// holds epics channel names, etc.
E 87
D 3
static char epics_name[][32] = {"scaler.S1","scaler.S16"};
static char *epics_chan[] = {"scaler","scaler"};
static char *epics_get[]  = {"get S1","get S16"};
E 3
I 3
D 7
static char epics_name[][32] = {"Faraday cup",
D 5
				"Halo 1U","Halo 1L","Halo 1R","Halo 1D",
				"Halo 2U","Halo 2L","Halo 2R","Halo 2D",
E 5
I 5
				"torus_current", "torus_voltage",
				"mini_current", "mini_voltage",
				"tagger_current", "tagger_voltage",
				"Halo 1U","Halo 1D","Halo 1L","Halo 1R",
				"Halo 2U","Halo 2D","Halo 2L","Halo 2R",
E 5
				"CC O2 monitor lamp","CC O2 monitor gas",
D 5
				"Beam vacuum","Tgt vacuum",
E 5
I 5
				"Upstream beam vacuum","Target vacuum",
E 7
I 7
D 112
struct epics_struct {
E 112
I 112
struct epics_struct
{
E 112
  char *name;
  char *chan;
  char *get;
E 7
E 5
};
D 7
static char *epics_chan[] = {"scaler_calc1",
I 5
			     "scaler", "scaler",
			     "MTSETI", "MTVRBCK",
			     "TMSETI", "TMVRBCK",
E 5
			     "scaler","scaler","scaler","scaler",
			     "scaler","scaler","scaler","scaler",
			     "scaler","scaler",
			     "scaler","scaler",
E 7
I 7
D 87

static epics_struct epics[] = {
I 15
D 17
  "junk",          "unknown",             "get",
E 15
I 9
  "beam_energy",          "MBSY2C_energy",             "get",
  "slit_position",        "SMRPOSB",                   "get",
  "thermionic_gun",       "IGT0I00BIASET",             "get",
  "polarized_gun",        "unknown",                   "get",
  "beam_current",         "IBC2C24AVG",                "get",
E 9
  "torus current",	  "hallb_sf_xy560_0_5",        "get",
D 9
  "torus voltage",	  "scaler",                    "get",
E 9
  "mini current",	  "MTSETI",                    "get",
  "mini voltage",	  "MTVRBCK",                   "get",
  "tagger current",	  "TMSETI",                    "get",
  "tagger voltage",	  "TMVRBCK",                   "get",
  "Scaler clock",	  "scalerS1o",                 "get",
  "Faraday cup",	  "scaler_calc1",              "get",
  "Halo U upstream",	  "scalerS2o",                 "get",
  "Halo D upstream",	  "scalerS3o",                 "get",
  "Halo L upstream",	  "scalerS4o",                 "get",
  "Halo R upstream",	  "scalerS5o",                 "get",
  "Halo U downstream",	  "scalerS6o",                 "get",
  "Halo D downstream",	  "scalerS7o",                 "get",
  "Halo L downstream",	  "scalerS8o",                 "get",
  "Halo R downstream",	  "scalerS9o",                 "get",
D 9
  "CC O2 monitor lamp",	  "scaler",                    "get",
  "CC O2 monitor gas",	  "scaler",                    "get",
  "Upstream beam vacuum", "scaler",                    "get",
  "Target vacuum",	  "scaler",                    "get",
E 9
I 9
  "CC O2 monitor lamp",	  "cc_o2_ref",                 "get",
  "CC O2 monitor gas",	  "cc_o2_sig",                 "get",
  "Upstream beam vacuum", "unknown",                   "get",
  "Target vacuum",	  "unknown",                   "get",
E 9
  "cryo pressure",	  "B_cryotarget_pressure",     "get",
  "cryo temperature",	  "B_cryotarget_temperature",  "get",
  "cryo status",	  "B_cryotarget_status",       "get",
  "torus he buf p",	  "hallb_sf_xy560_0_6",        "get",
  "torus he tank p",	  "hallb_sf_xy560_0_7",        "get",
  "torus cold he t",	  "hallb_sf_xy560_0_8",        "get",
D 9
  "torus ln2 pres",       "hallb_sf_xy560_0_9",        "get",
  "torus ln2 level",      "hallb_sf_xy560_0_10",       "get",
  "torus ln2 tank p",     "hallb_sf_xy560_0_11",       "get",
  "torus he level",       "hallb_sf_xy560_0_12",       "get",
E 9
I 9
  "torus ln2 tank pres",  "hallb_sf_xy560_0_9",        "get",
  "torus ln2 tank level", "hallb_sf_xy560_0_10",       "get",
  "torus he tank level",  "hallb_sf_xy560_0_11",       "get",
  "torus svc module vac", "hallb_sf_xy560_0_12",       "get",
E 17
I 17
D 20
  "beam_energy",         	 	"MBSY2C_energy",             "get",
  "slit_position",           	 	"SMRPOSB",                   "get",
  "thermionic_gun",          	 	"IGT0I00BIASET",             "get",
  "polarized_gun",           	 	"unknown",                   "get",
  "beam_current",            	 	"IBC2C24AVG",                "get",
E 20
I 20
D 38
  "beam energy",         	 	"MBSY2C_energy",             "get",
D 25
  "slit position",           	 	"SMRPOSB",                   "get",
E 25
I 25
  "A slit position",           	 	"SMRPOSA",                   "get",
  "B slit position",           	 	"SMRPOSB",                   "get",
  "C slit position",           	 	"SMRPOSC",                   "get",
E 25
  "thermionic gun",          	 	"IGT0I00BIASET",             "get",
  "polarized gun",           	 	"unknown",                   "get",
  "beam current",            	 	"IBC2C24AVG",                "get",
E 20
  "torus current",	     	 	"hallb_sf_xy560_0_5",        "get",
  "mini current",	     	 	"MTSETI",                    "get",
  "mini voltage",	     	 	"MTVRBCK",                   "get",
  "tagger current",	     	 	"TMSETI",                    "get",
  "tagger voltage",	     	 	"TMVRBCK",                   "get",
  "Scaler clock",	     	 	"scalerS1o",                 "get",
  "Faraday cup",	     	 	"scaler_calc1",              "get",
  "Halo U upstream",	     	 	"scalerS2o",                 "get",
  "Halo D upstream",	     	 	"scalerS3o",                 "get",
  "Halo L upstream",	     	 	"scalerS4o",                 "get",
  "Halo R upstream",	     	 	"scalerS5o",                 "get",
  "Halo U downstream",	     	 	"scalerS6o",                 "get",
  "Halo D downstream",	     	 	"scalerS7o",                 "get",
  "Halo L downstream",	     	 	"scalerS8o",                 "get",
  "Halo R downstream",	     	 	"scalerS9o",                 "get",
  "CC O2 monitor lamp",	     	 	"cc_o2_ref",                 "get",
  "CC O2 monitor gas",	     	 	"cc_o2_sig",                 "get",
  "Upstream beam vacuum",    	 	"unknown",                   "get",
  "Target vacuum",	     	 	"unknown",                   "get",
I 21
D 24
  "bpm_1_x",     	     	 	"unknown",                   "get",
  "bpm_1_y",     	     	 	"unknown",                   "get",
  "bpm_1_i",     	     	 	"unknown",                   "get",
  "bpm_2_x",     	     	 	"unknown",                   "get",
  "bpm_2_y",     	     	 	"unknown",                   "get",
  "bpm_2_i",     	     	 	"unknown",                   "get",
  "bpm_3_x",     	     	 	"unknown",                   "get",
  "bpm_3_y",     	     	 	"unknown",                   "get",
  "bpm_3_i",     	     	 	"unknown",                   "get",
E 24
I 24
  "bpm_1_x",     	     	 	"IPM2H01.XPOS",              "get",
  "bpm_1_y",     	     	 	"IPM2H01.YPOS",              "get",
D 26
  "bpm_1_i",     	     	 	"IPM2H01.VAL",               "get",
E 26
I 26
  "bpm_1_i",     	     	 	"IPM2H01",                   "get",
E 26
  "bpm_2_x",     	     	 	"IPM2C24A.XPOS",             "get",
  "bpm_2_y",     	     	 	"IPM2C24A.YPOS",             "get",
D 26
  "bpm_2_i",     	     	 	"IPM2C24A.VAL",              "get",
E 26
I 26
  "bpm_2_i",     	     	 	"IPM2C24A",                  "get",
E 26
  "bpm_3_x",     	     	 	"IPM2C22A.XPOS",             "get",
  "bpm_3_y",     	     	 	"IPM2C22A.YPOS",             "get",
D 26
  "bpm_3_i",     	     	 	"IPM2C22A.VAL",              "get",
E 26
I 26
  "bpm_3_i",     	     	 	"IPM2C22A",                  "get",
E 26
E 24
E 21
  "cryo pressure",	     	 	"B_cryotarget_pressure",     "get",
  "cryo temperature",	     	 	"B_cryotarget_temperature",  "get",
  "cryo status",	     	 	"B_cryotarget_status",       "get",
  "torus he buf p (atm)",    	 	"hallb_sf_xy560_0_6",        "get",
  "torus he tank p (atm)",   	 	"hallb_sf_xy560_0_7",        "get",
  "torus cold he t (kelvin)",	 	"hallb_sf_xy560_0_8",        "get",
  "torus ln2 tank pres (atm)", 	 	"hallb_sf_xy560_0_9",        "get",
  "torus ln2 tank level (%)",    	"hallb_sf_xy560_0_10",       "get",
  "torus he tank level (%)",     	"hallb_sf_xy560_0_11",       "get",
  "torus svc module vac (-log torr)",	"hallb_sf_xy560_0_12",       "get",
I 30
  "heat exc pressure",                  "B_cryotarget_PT_2KH",       "get",
  "cryostat pressure",                  "B_cryotarget_PT_5KH",       "get",
  "He3 tank pressure",                  "B_cryotarget_PT_HES",       "get",
  "target pressure",                    "B_cryotarget_PT_T",         "get",
  "beam screen temperature",            "B_cryotarget_TT_EBP",       "get",
  "20k heater temperature",             "B_cryotarget_TT_E1",        "get",
  "300k heater temperature",            "B_cryotarget_TT_ECR",       "get",
  "D2 tank pressure",                   "B_cryotarget_PT_D2S",       "get",
  "H2 tank pressure",                   "B_cryotarget_PT_H2S",       "get",
  "He cryostat level",                  "B_cryotarget_LT_CR",        "get",
  "primary target vac",                 "B_cryotarget_PT_VT",        "get",
  "primary chamber vac",                "B_cryotarget_PT_CVP",       "get",
  "secondary chamber vac",              "B_cryotarget_PT_CVS",       "get",
  "joule-thompson exchanger",           "B_cryotarget_JT_2K",        "get",
  "target temperature",                 "B_cryotarget_TT_T",         "get",
  "target heat exc temperature",        "B_cryotarget_TT_ET",        "get",
  "heat exc exhaust temperature",       "B_cryotarget_TT_ER1",       "get",
  "target level low",                   "B_cryotarget_LS_TB",        "get",
  "target level high",                  "B_cryotarget_LS_TH",        "get",
  "target level low display",           "B_cryotarget_NIV_C_BAS",    "get",
  "target level high display",          "B_cryotarget_NIV_C_HAUT",   "get",
  "target level high by PT_T",          "B_cryotarget_NIV_BAS",      "get",
  "targer level low by PT_T",           "B_cryotarget_NIV_HAUT",     "get",
E 38
I 38
D 74
  "beam energy",         	 	    	  "MBSY2C_energy",             "get",
E 74
I 74
  "Hall A energy",         	 	    	  "MBSY1C_energy",             "get",
  "Hall B energy",         	 	    	  "MBSY2C_energy",             "get",
  "Hall C energy",         	 	    	  "MBSY3C_energy",             "get",
  "Hall A beam current",                          "IBC1H03AAVG",               "get",
  "Hall B beam current",       	 	    	  "IBC2C24AVG",                "get",
  "Hall C beam current",                          "IBC3H00AVG",                "get",
  "A Laser Duty Factor",                          "IGL1I00HALLADF",            "get",
  "B Laser Duty Factor",                          "IGL1I00HALLBDF",            "get",
  "C Laser Duty Factor",                          "IGL1I00HALLCDF",            "get",
D 76
  "Hall A status",                                "PLC_HLA",                   "get",
  "Hall B status",                                "PLC_HLB",                   "get",
  "Hall C status",                                "PLC_HLC",                   "get",
E 76
I 76
  //  "Hall A status",                                "PLC_HLA",                   "get",
  //  "Hall B status",                                "PLC_HLB",                   "get",
  //  "Hall C status",                                "PLC_HLC",                   "get",
E 76
E 74
  "A slit position",           	 	    	  "SMRPOSA",                   "get",
  "B slit position",           	 	    	  "SMRPOSB",                   "get",
  "C slit position",           	 	    	  "SMRPOSC",                   "get",
D 74
  "thermionic gun",          	 	    	  "IGT0I00BIASET",             "get",
I 71
  "beam phase",                                   "IGL1I00OD16_16",            "get",
E 74
E 71
D 51
  "polarized gun",           	 	    	  "unknown",                   "get",
E 51
I 51
  "A polarized gun",           	 	    	  "IGL1I00DAC0",               "get",
  "B polarized gun",           	 	    	  "IGL1I00DAC2",               "get",
  "C polarized gun",           	 	    	  "IGL1I00DAC4",               "get",
I 75
  "e2_target_a_status",                           "e2_target_a_status",        "get",
  "e2_target_b_status",				  "e2_target_b_status",        "get",
  "e2_target_c_status",				  "e2_target_c_status",        "get",
E 75
E 51
D 74
  "beam current",            	 	    	  "IBC2C24AVG",                "get",
E 74
I 74
  "thermionic gun",          	 	    	  "IGT0I00BIASET",             "get",
  "beam phase",                                   "IGL1I00OD16_16",            "get",
E 74
  "torus current",	     	 	    	  "hallb_sf_xy560_0_5",        "get",
D 45
  "mini current",	     	 	    	  "MTSETI",                    "get",
E 45
I 45
D 68
  "mini current setpoint",     	 	    	  "MTSETI",                    "get",
E 45
I 44
  "mini current readback",     	 	    	  "MTIRBCK",                   "get",
E 44
  "mini voltage",	     	 	    	  "MTVRBCK",                   "get",
E 68
I 68
D 70
//   "mini current setpoint",     	 	    	  "MTSETI",                    "get",
//   "mini current readback",     	 	    	  "MTIRBCK",                   "get",
//   "mini voltage",	     	 	    	  "MTVRBCK",                   "get",
E 70
I 70
  "mini current setpoint",     	 	    	  "MTSETI",                    "get",
  "mini current readback",     	 	    	  "MTIRBCK",                   "get",
  "mini voltage",	     	 	    	  "MTVRBCK",                   "get",
E 70
E 68
D 45
  "tagger current",	     	 	    	  "TMSETI",                    "get",
E 45
I 45
  "tagger current setpoint",   	 	    	  "TMSETI",                    "get",
E 45
I 44
D 51
  "tagger current readback",	 	    	  "TMIRBCK",                    "get",
E 51
I 51
  "tagger current readback",	 	    	  "TMIRBCK",                   "get",
E 51
E 44
  "tagger voltage",	     	 	    	  "TMVRBCK",                   "get",
I 80
  "Hall B Helicity",     			  "hallb:helicity",  	       "get",    
E 80
I 52
  "Harp",	        	 	    	  "harp",                      "get DRBV",
E 52
  "Scaler clock",	     	 	    	  "scalerS1o",                 "get",
  "Faraday cup",	     	 	    	  "scaler_calc1",              "get",
  "Halo U upstream",	     	 	    	  "scalerS2o",                 "get",
  "Halo D upstream",	     	 	    	  "scalerS3o",                 "get",
  "Halo L upstream",	     	 	    	  "scalerS4o",                 "get",
  "Halo R upstream",	     	 	    	  "scalerS5o",                 "get",
  "Halo U downstream",	     	 	    	  "scalerS6o",                 "get",
  "Halo D downstream",	     	 	    	  "scalerS7o",                 "get",
  "Halo L downstream",	     	 	    	  "scalerS8o",                 "get",
  "Halo R downstream",	     	 	    	  "scalerS9o",                 "get",
  "CC O2 monitor lamp",	     	 	    	  "cc_o2_ref",                 "get",
  "CC O2 monitor gas",	     	 	    	  "cc_o2_sig",                 "get",
D 51
  "Upstream beam vacuum",    	 	    	  "unknown",                   "get",
  "Target vacuum",	     	 	    	  "unknown",                   "get",
E 51
I 51
  "Tagger turbo speed (%)",    	    	          "VMP2C24SPD",                "get",
  "Moeller vacuum (V)",	     	 	    	  "VCG2C21",                   "get",
  "Upstream beam vacuum (V)",  	 	    	  "VCG2C24",                   "get",
  "Target vacuum (V)",	     	 	    	  "VCG2H01",                   "get",
  "Faraday cup ion pump (KV)",  	    	  "VIP2H01DV",                 "get",
  "Faraday cup ion pump (ma)",    	          "VIP2H01DI",                 "get",
E 51
  "bpm_1_x",     	     	 	    	  "IPM2H01.XPOS",              "get",
  "bpm_1_y",     	     	 	    	  "IPM2H01.YPOS",              "get",
  "bpm_1_i",     	     	 	    	  "IPM2H01",                   "get",
D 63
  "bpm_2_x",     	     	 	    	  "IPM2C24A.XPOS",             "get",
  "bpm_2_y",     	     	 	    	  "IPM2C24A.YPOS",             "get",
  "bpm_2_i",     	     	 	    	  "IPM2C24A",                  "get",
  "bpm_3_x",     	     	 	    	  "IPM2C22A.XPOS",             "get",
  "bpm_3_y",     	     	 	    	  "IPM2C22A.YPOS",             "get",
  "bpm_3_i",     	     	 	    	  "IPM2C22A",                  "get",
E 63
I 63
  "bpm_1_x_gain",     	     	 	    	  "IPM2H01",                   "get XKMF",
  "bpm_1_y_gain",     	     	 	    	  "IPM2H01",                   "get YKMF",
  "bpm_1_i_gain",     	     	 	    	  "IPM2H01",                   "get IKMF",
E 63
I 59
  "bpm_1_x_scale",	  			  "IPM2H01XSENrbstr",	       "get STR1",
  "bpm_1_y_scale",	  			  "IPM2H01YSENrbstr",	       "get STR1",
  "bpm_1_i_scale",	  			  "IBC2H01ISENrbstr",	       "get STR1",
I 63
  "bpm_2_x",     	     	 	    	  "IPM2C24A.XPOS",             "get",
  "bpm_2_y",     	     	 	    	  "IPM2C24A.YPOS",             "get",
  "bpm_2_i",     	     	 	    	  "IPM2C24A",                  "get",
  "bpm_2_i_gain",     	     	 	    	  "IPM2C24A",                  "get XKMF",
  "bpm_2_x_gain",     	     	 	    	  "IPM2C24A",                  "get YKMF",
  "bpm_2_y_gain",     	     	 	    	  "IPM2C24A",                  "get IKMF",
E 63
  "bpm_2_x_scale",	  			  "IPM2C24AXSENrbstr",	       "get STR1",
  "bpm_2_y_scale",	  			  "IPM2C24AYSENrbstr",	       "get STR1",
  "bpm_2_i_scale",	  			  "IBC2C24AISENrbstr",	       "get STR1",
I 63
D 79
  "bpm_3_x",     	     	 	    	  "IPM2C22A.XPOS",             "get",
  "bpm_3_y",     	     	 	    	  "IPM2C22A.YPOS",             "get",
  "bpm_3_i",     	     	 	    	  "IPM2C22A",                  "get",
  "bpm_3_x_gain",     	     	 	    	  "IPM2C22A",                  "get XKMF",
D 65
  "bpm_3_y_gain",     	     	 	    	  "IPM2C22A",                  "get XKMF",
  "bpm_3_i_gain",     	     	 	    	  "IPM2C22A",                  "get XKMF",
E 65
I 65
  "bpm_3_y_gain",     	     	 	    	  "IPM2C22A",                  "get YKMF",
  "bpm_3_i_gain",     	     	 	    	  "IPM2C22A",                  "get IKMF",
E 65
E 63
  "bpm_3_x_scale",	  			  "IPM2C22AXSENrbstr",	       "get STR1",
  "bpm_3_y_scale",	  			  "IPM2C22AYSENrbstr",	       "get STR1",
  "bpm_3_i_scale",	  			  "IBC2C22AISENrbstr",	       "get STR1",
E 79
I 79
  "bpm_3_x",     	     	 	    	  "IPM2C21A.XPOS",             "get",
  "bpm_3_y",     	     	 	    	  "IPM2C21A.YPOS",             "get",
  "bpm_3_i",     	     	 	    	  "IPM2C21A",                  "get",
  "bpm_3_x_gain",     	     	 	    	  "IPM2C21A",                  "get XKMF",
  "bpm_3_y_gain",     	     	 	    	  "IPM2C21A",                  "get YKMF",
  "bpm_3_i_gain",     	     	 	    	  "IPM2C21A",                  "get IKMF",
  "bpm_3_x_scale",	  			  "IPM2C21AXSENrbstr",	       "get STR1",
  "bpm_3_y_scale",	  			  "IPM2C21AYSENrbstr",	       "get STR1",
  "bpm_3_i_scale",	  			  "IBC2C21AISENrbstr",	       "get STR1",
E 79
I 63
  "Moller Quad1 current",   			  "hallb_sf_xy560_0_14",       "get",
  "Moller Quad2 current",   			  "hallb_sf_xy560_0_18",       "get",
  "Helmholtz Coils current",			  "hallb_sf_xy560_0_19",       "get",
  "Moller target position", 			  "moeller_target",            "get RBV",
  "Raster set energy",   			  "RASTSETENERGY",   	       "get",
  "Raster set pattern X",			  "RASTSETPATTERNX", 	       "get",
  "Raster set pattern Y",			  "RASTSETPATTERNY", 	       "get",
  "Raster X offset",     			  "RASTSETXOFFSET",  	       "get",
  "Raster Y offset",     			  "RASTSETYOFFSET",  	       "get",    
I 73
  "cryo target pressure",	 	    	  "B_cryotarget_pressure",     "get",
  "cryo target temperature",   	 	    	  "B_cryotarget_temperature",  "get",
  "cryo target status",	     	 	    	  "B_cryotarget_status",       "get",
  "torus he buf p (atm)",    	 	    	  "hallb_sf_xy560_0_6",        "get",
  "torus he tank p (atm)",   	 	    	  "hallb_sf_xy560_0_7",        "get",
  "torus cold he t (kelvin)",	 	    	  "hallb_sf_xy560_0_8",        "get",
  "torus ln2 tank pres (atm)", 	 	    	  "hallb_sf_xy560_0_9",        "get",
  "torus ln2 tank level (%)",    	    	  "hallb_sf_xy560_0_10",       "get",
  "torus he tank level (%)",     	    	  "hallb_sf_xy560_0_11",       "get",
  "torus svc module vac (-log torr)",	    	  "hallb_sf_xy560_0_12",       "get",
  "cryo target heat exch pressure",          	  "B_cryotarget_PT_2KH",       "get",
  "cryo target cryostat pressure",          	  "B_cryotarget_PT_5KH",       "get",
  "cryo target He3 tank pressure",          	  "B_cryotarget_PT_HES",       "get",
  "cryo target target pressure",            	  "B_cryotarget_PT_T",         "get",
  "cryo target beam screen temperature",    	  "B_cryotarget_TT_EBP",       "get",
  "cryo target 20k heater temperature",     	  "B_cryotarget_TT_E1",        "get",
  "cryo target 300k heater temperature",    	  "B_cryotarget_TT_ECR",       "get",
  "cryo target D2 tank pressure",           	  "B_cryotarget_PT_D2S",       "get",
  "cryo target H2 tank pressure",           	  "B_cryotarget_PT_H2S",       "get",
  "cryo target He cryostat level",          	  "B_cryotarget_LT_CR",        "get",
  "cryo target primary target vac",         	  "B_cryotarget_PT_VT",        "get",
  "cryo target primary chamber vac",        	  "B_cryotarget_PT_CVP",       "get",
  "cryo target secondary chamber vac",      	  "B_cryotarget_PT_CVS",       "get",
  "cryo target joule-thompson exch",    	  "B_cryotarget_JT_2K",        "get",
  "cryo target temperature",                	  "B_cryotarget_TT_T",         "get",
  "cryo target heat exch temperature",       	  "B_cryotarget_TT_ET",        "get",
  "cryo target heat exch exhaust temperature",	  "B_cryotarget_TT_ER1",       "get",
  "cryo target level low",                  	  "B_cryotarget_LS_TB",        "get",
  "cryo target level high",                 	  "B_cryotarget_LS_TH",        "get",
  "cryo target level low display",          	  "B_cryotarget_NIV_C_BAS",    "get",
  "cryo target level high display",         	  "B_cryotarget_NIV_C_HAUT",   "get",
  "cryo target level high by PT_T",         	  "B_cryotarget_NIV_BAS",      "get",
D 81
  "cryo targer level low by PT_T",          	  "B_cryotarget_NIV_HAUT",     "get",
E 81
I 81
  "cryo target level low by PT_T",          	  "B_cryotarget_NIV_HAUT",     "get",
I 86
  "Hall A dp/p",      	      			  "halla_dpp",     	       "get",
  "Hall A dp/p Corr", 	      			  "halla_dppCorr", 	       "get",
  "Hall A dp/p BPM",  	      			  "halla_dppBpm",  	       "get",
  "Hall C dp/p",      	      			  "hallc_dpp",     	       "get",
  "Hall C dp/p Corr", 	      			  "hallc_dppCorr", 	       "get",
  "Hall C dp/p BPM",  	      			  "hallc_dppBpm",  	       "get",
  "Arc1 dp/p",        	      			  "Arc1_dpp",      	       "get",
  "Arc2 dp/p",        	      			  "Arc2_dpp",      	       "get",
  "Hall A Tiefenbach energy", 			  "halla_MeV",     	       "get",      
  "E5 target pressure",         		  "E5_PT_T_C2",   	       "get",
  "E5 heat shield temperature", 		  "E5_TT_S",      	       "get",
  "E5 cell temperature top"   , 		  "E5_TT_T_C2_H", 	       "get",
  "E5 cell temperature bottom", 		  "E5_TT_T_C2_L", 	       "get",       
E 86
E 81
E 73
E 63
E 59
D 68
  "cryo target pressure",	 	    	  "B_cryotarget_pressure",     "get",
  "cryo target temperature",   	 	    	  "B_cryotarget_temperature",  "get",
  "cryo targer status",	     	 	    	  "B_cryotarget_status",       "get",
  "torus he buf p (atm)",    	 	    	  "hallb_sf_xy560_0_6",        "get",
  "torus he tank p (atm)",   	 	    	  "hallb_sf_xy560_0_7",        "get",
  "torus cold he t (kelvin)",	 	    	  "hallb_sf_xy560_0_8",        "get",
  "torus ln2 tank pres (atm)", 	 	    	  "hallb_sf_xy560_0_9",        "get",
  "torus ln2 tank level (%)",    	    	  "hallb_sf_xy560_0_10",       "get",
  "torus he tank level (%)",     	    	  "hallb_sf_xy560_0_11",       "get",
  "torus svc module vac (-log torr)",	    	  "hallb_sf_xy560_0_12",       "get",
  "cryo target heat exch pressure",          	  "B_cryotarget_PT_2KH",       "get",
  "cryo target cryostat pressure",          	  "B_cryotarget_PT_5KH",       "get",
  "cryo target He3 tank pressure",          	  "B_cryotarget_PT_HES",       "get",
  "cryo target target pressure",            	  "B_cryotarget_PT_T",         "get",
  "cryo target beam screen temperature",    	  "B_cryotarget_TT_EBP",       "get",
  "cryo target 20k heater temperature",     	  "B_cryotarget_TT_E1",        "get",
  "cryo target 300k heater temperature",    	  "B_cryotarget_TT_ECR",       "get",
  "cryo target D2 tank pressure",           	  "B_cryotarget_PT_D2S",       "get",
  "cryo target H2 tank pressure",           	  "B_cryotarget_PT_H2S",       "get",
  "cryo target He cryostat level",          	  "B_cryotarget_LT_CR",        "get",
  "cryo target primary target vac",         	  "B_cryotarget_PT_VT",        "get",
  "cryo target primary chamber vac",        	  "B_cryotarget_PT_CVP",       "get",
  "cryo target secondary chamber vac",      	  "B_cryotarget_PT_CVS",       "get",
  "cryo target joule-thompson exch",    	  "B_cryotarget_JT_2K",        "get",
  "cryo target temperature",                	  "B_cryotarget_TT_T",         "get",
  "cryo target heat exch temperature",       	  "B_cryotarget_TT_ET",        "get",
  "cryo target heat exch exhaust temperature",	  "B_cryotarget_TT_ER1",       "get",
  "cryo target level low",                  	  "B_cryotarget_LS_TB",        "get",
  "cryo target level high",                 	  "B_cryotarget_LS_TH",        "get",
  "cryo target level low display",          	  "B_cryotarget_NIV_C_BAS",    "get",
  "cryo target level high display",         	  "B_cryotarget_NIV_C_HAUT",   "get",
  "cryo target level high by PT_T",         	  "B_cryotarget_NIV_BAS",      "get",
  "cryo targer level low by PT_T",          	  "B_cryotarget_NIV_HAUT",     "get",
E 68
E 38
E 30
E 17
E 9
I 8
D 10
  "dc_ar1_flow",	  "dc_ar1_flow",               "get",
  "dc_ar2_flow",	  "dc_ar2_flow",               "get",
  "dc_buf1_pres",	  "dc_buf1_pres",              "get",
  "dc_buf2_pres",	  "dc_buf2_pres",              "get",
  "dc_co21_flow",	  "dc_co21_flow",              "get",
  "dc_co22_flow",	  "dc_co22_flow",              "get",
  "dc_r1in_flow",	  "dc_r1in_flow",              "get",
  "dc_r1in_h2o",	  "dc_r1in_h2o",               "get",
  "dc_r1in_o2",		  "dc_r1in_o2",                "get",
  "dc_r1in_pres",	  "dc_r1in_pres",              "get",
  "dc_r1in_temp",	  "dc_r1in_temp",              "get",
  "dc_r1io_pres",	  "dc_r1io_pres",              "get",
  "dc_r1out_h2o",	  "dc_r1out_h2o",              "get",
  "dc_r1out_o2",	  "dc_r1out_o2",               "get",
  "dc_r1out_pres",	  "dc_r1out_pres",             "get",
  "dc_r1out_temp",	  "dc_r1out_temp",             "get",
  "dc_r1r3_co2",	  "dc_r1r3_co2",               "get",
  "dc_r2_co2",		  "dc_r2_co2",                 "get",
  "dc_r2exh_flow",	  "dc_r2exh_flow",             "get",
  "dc_r2in_flow",	  "dc_r2in_flow",              "get",
  "dc_r2in_h2o",	  "dc_r2in_h2o",               "get",
  "dc_r2in_o2",		  "dc_r2in_o2",                "get",
  "dc_r2in_pres",	  "dc_r2in_pres",              "get",
  "dc_r2in_temp",	  "dc_r2in_temp",              "get",
  "dc_r2io_pres",	  "dc_r2io_pres",              "get",
  "dc_r2out_h2o",	  "dc_r2out_h2o",              "get",
  "dc_r2out_o2",	  "dc_r2out_o2",               "get",
  "dc_r2out_pres",	  "dc_r2out_pres",             "get",
  "dc_r2out_temp",	  "dc_r2out_temp",             "get",
  "dc_r3exh_flow",	  "dc_r3exh_flow",             "get",
  "dc_r3in_flow",	  "dc_r3in_flow",              "get",
  "dc_r3in_h2o",	  "dc_r3in_h2o",               "get",
  "dc_r3in_o2",		  "dc_r3in_o2",                "get",
  "dc_r3in_pres",	  "dc_r3in_pres",              "get",
  "dc_r3in_temp",	  "dc_r3in_temp",              "get",
  "dc_r3io_pres",	  "dc_r3io_pres",              "get",
  "dc_r3out_h2o",	  "dc_r3out_h2o",              "get",
  "dc_r3out_o2",	  "dc_r3out_o2",               "get",
  "dc_r2out_pres",	  "dc_r2out_pres",             "get",
  "dc_r2out_temp",	  "dc_r2out_temp",             "get",
  "dc_r3exh_flow",	  "dc_r3exh_flow",             "get",
  "dc_r3in_flow",	  "dc_r3in_flow",              "get",
  "dc_r3in_h2o",	  "dc_r3in_h2o",               "get",
  "dc_r3in_o2",		  "dc_r3in_o2",                "get",
  "dc_r3in_pres",	  "dc_r3in_pres",              "get",
  "dc_r3in_temp",	  "dc_r3in_temp",              "get",
  "dc_r3io_pres",	  "dc_r3io_pres",              "get",
  "dc_r3out_h2o",	  "dc_r3out_h2o",              "get",
  "dc_r3out_o2",	  "dc_r3out_o2",               "get",
  "dc_r3out_pres",	  "dc_r3out_pres",             "get",
  "dc_r3out_temp",	  "dc_r3out_temp",             "get",
E 10
E 8
E 7
};
E 87
I 87
static epics_struct epics[MAX_EPICS];
I 114
static epics_struct scalers[MAX_EPICS];
I 118
static epics_struct gammas[MAX_EPICS];
E 118
E 114
E 87
D 7
static char *epics_get[]  = {"get",
I 5
			     "get S1","get S2",
			     "get","get",
			     "get","get",
E 5
			     "get S1","get S2","get S3","get S4",
			     "get S5","get S6","get S7","get S8",
			     "get S9","get S10",
			     "get S11","get S12",
};
E 3
static int epics_val[sizeof(epics_chan)/sizeof(char *)];
E 7
I 7
D 73

I 30

I 68
//   "cryo target pressure",	 	    	  "B_cryotarget_pressure",     "get",
//   "cryo target temperature",   	 	    	  "B_cryotarget_temperature",  "get",
//   "cryo targer status",	     	 	    	  "B_cryotarget_status",       "get",
//   "torus he buf p (atm)",    	 	    	  "hallb_sf_xy560_0_6",        "get",
//   "torus he tank p (atm)",   	 	    	  "hallb_sf_xy560_0_7",        "get",
//   "torus cold he t (kelvin)",	 	    	  "hallb_sf_xy560_0_8",        "get",
//   "torus ln2 tank pres (atm)", 	 	    	  "hallb_sf_xy560_0_9",        "get",
//   "torus ln2 tank level (%)",    	    	  "hallb_sf_xy560_0_10",       "get",
//   "torus he tank level (%)",     	    	  "hallb_sf_xy560_0_11",       "get",
//   "torus svc module vac (-log torr)",	    	  "hallb_sf_xy560_0_12",       "get",
//   "cryo target heat exch pressure",          	  "B_cryotarget_PT_2KH",       "get",
//   "cryo target cryostat pressure",          	  "B_cryotarget_PT_5KH",       "get",
//   "cryo target He3 tank pressure",          	  "B_cryotarget_PT_HES",       "get",
//   "cryo target target pressure",            	  "B_cryotarget_PT_T",         "get",
//   "cryo target beam screen temperature",    	  "B_cryotarget_TT_EBP",       "get",
//   "cryo target 20k heater temperature",     	  "B_cryotarget_TT_E1",        "get",
//   "cryo target 300k heater temperature",    	  "B_cryotarget_TT_ECR",       "get",
//   "cryo target D2 tank pressure",           	  "B_cryotarget_PT_D2S",       "get",
//   "cryo target H2 tank pressure",           	  "B_cryotarget_PT_H2S",       "get",
//   "cryo target He cryostat level",          	  "B_cryotarget_LT_CR",        "get",
//   "cryo target primary target vac",         	  "B_cryotarget_PT_VT",        "get",
//   "cryo target primary chamber vac",        	  "B_cryotarget_PT_CVP",       "get",
//   "cryo target secondary chamber vac",      	  "B_cryotarget_PT_CVS",       "get",
//   "cryo target joule-thompson exch",    	  "B_cryotarget_JT_2K",        "get",
//   "cryo target temperature",                	  "B_cryotarget_TT_T",         "get",
//   "cryo target heat exch temperature",       	  "B_cryotarget_TT_ET",        "get",
//   "cryo target heat exch exhaust temperature",	  "B_cryotarget_TT_ER1",       "get",
//   "cryo target level low",                  	  "B_cryotarget_LS_TB",        "get",
//   "cryo target level high",                 	  "B_cryotarget_LS_TH",        "get",
//   "cryo target level low display",          	  "B_cryotarget_NIV_C_BAS",    "get",
//   "cryo target level high display",         	  "B_cryotarget_NIV_C_HAUT",   "get",
//   "cryo target level high by PT_T",         	  "B_cryotarget_NIV_BAS",      "get",
//   "cryo targer level low by PT_T",          	  "B_cryotarget_NIV_HAUT",     "get",
// };
E 73


E 68
E 30
I 10
D 81
//   "dc_ar1_flow",	  "dc_ar1_flow",               "get",
//   "dc_ar2_flow",	  "dc_ar2_flow",               "get",
//   "dc_buf1_pres",	  "dc_buf1_pres",              "get",
//   "dc_buf2_pres",	  "dc_buf2_pres",              "get",
//   "dc_co21_flow",	  "dc_co21_flow",              "get",
//   "dc_co22_flow",	  "dc_co22_flow",              "get",
//   "dc_r1in_flow",	  "dc_r1in_flow",              "get",
//   "dc_r1in_h2o",	  "dc_r1in_h2o",               "get",
//   "dc_r1in_o2",		  "dc_r1in_o2",                "get",
//   "dc_r1in_pres",	  "dc_r1in_pres",              "get",
//   "dc_r1in_temp",	  "dc_r1in_temp",              "get",
//   "dc_r1io_pres",	  "dc_r1io_pres",              "get",
//   "dc_r1out_h2o",	  "dc_r1out_h2o",              "get",
//   "dc_r1out_o2",	  "dc_r1out_o2",               "get",
//   "dc_r1out_pres",	  "dc_r1out_pres",             "get",
//   "dc_r1out_temp",	  "dc_r1out_temp",             "get",
//   "dc_r1r3_co2",	  "dc_r1r3_co2",               "get",
//   "dc_r2_co2",		  "dc_r2_co2",                 "get",
//   "dc_r2exh_flow",	  "dc_r2exh_flow",             "get",
//   "dc_r2in_flow",	  "dc_r2in_flow",              "get",
//   "dc_r2in_h2o",	  "dc_r2in_h2o",               "get",
//   "dc_r2in_o2",		  "dc_r2in_o2",                "get",
//   "dc_r2in_pres",	  "dc_r2in_pres",              "get",
//   "dc_r2in_temp",	  "dc_r2in_temp",              "get",
//   "dc_r2io_pres",	  "dc_r2io_pres",              "get",
//   "dc_r2out_h2o",	  "dc_r2out_h2o",              "get",
//   "dc_r2out_o2",	  "dc_r2out_o2",               "get",
//   "dc_r2out_pres",	  "dc_r2out_pres",             "get",
//   "dc_r2out_temp",	  "dc_r2out_temp",             "get",
//   "dc_r3exh_flow",	  "dc_r3exh_flow",             "get",
//   "dc_r3in_flow",	  "dc_r3in_flow",              "get",
//   "dc_r3in_h2o",	  "dc_r3in_h2o",               "get",
//   "dc_r3in_o2",		  "dc_r3in_o2",                "get",
//   "dc_r3in_pres",	  "dc_r3in_pres",              "get",
//   "dc_r3in_temp",	  "dc_r3in_temp",              "get",
//   "dc_r3io_pres",	  "dc_r3io_pres",              "get",
//   "dc_r3out_h2o",	  "dc_r3out_h2o",              "get",
//   "dc_r3out_o2",	  "dc_r3out_o2",               "get",
//   "dc_r2out_pres",	  "dc_r2out_pres",             "get",
//   "dc_r2out_temp",	  "dc_r2out_temp",             "get",
//   "dc_r3exh_flow",	  "dc_r3exh_flow",             "get",
//   "dc_r3in_flow",	  "dc_r3in_flow",              "get",
//   "dc_r3in_h2o",	  "dc_r3in_h2o",               "get",
//   "dc_r3in_o2",		  "dc_r3in_o2",                "get",
//   "dc_r3in_pres",	  "dc_r3in_pres",              "get",
//   "dc_r3in_temp",	  "dc_r3in_temp",              "get",
//   "dc_r3io_pres",	  "dc_r3io_pres",              "get",
//   "dc_r3out_h2o",	  "dc_r3out_h2o",              "get",
//   "dc_r3out_o2",	  "dc_r3out_o2",               "get",
//   "dc_r3out_pres",	  "dc_r3out_pres",             "get",
//   "dc_r3out_temp",	  "dc_r3out_temp",             "get",
// };
E 10

I 10

E 81
I 81
// for channel data
I 118
static int    nepics = sizeof(epics)/sizeof(epics_struct);
E 118
E 81
E 10
D 8
//   "dc_ar1_flow",	  "dc_ar1_flow",               "get",
//   "dc_ar2_flow",	  "dc_ar2_flow",               "get",
//   "dc_buf1_pres",	  "dc_buf1_pres",              "get",
//   "dc_buf2_pres",	  "dc_buf2_pres",              "get",
//   "dc_co21_flow",	  "dc_co21_flow",              "get",
//   "dc_co22_flow",	  "dc_co22_flow",              "get",
//   "dc_r1in_flow",	  "dc_r1in_flow",              "get",
//   "dc_r1in_h2o",	  "dc_r1in_h2o",               "get",
//   "dc_r1in_o2",		  "dc_r1in_o2",                "get",
//   "dc_r1in_pres",	  "dc_r1in_pres",              "get",
//   "dc_r1in_temp",	  "dc_r1in_temp",              "get",
//   "dc_r1io_pres",	  "dc_r1io_pres",              "get",
//   "dc_r1out_h2o",	  "dc_r1out_h2o",              "get",
//   "dc_r1out_o2",	  "dc_r1out_o2",               "get",
//   "dc_r1out_pres",	  "dc_r1out_pres",             "get",
//   "dc_r1out_temp",	  "dc_r1out_temp",             "get",
//   "dc_r1r3_co2",	  "dc_r1r3_co2",               "get",
//   "dc_r2_co2",		  "dc_r2_co2",                 "get",
//   "dc_r2exh_flow",	  "dc_r2exh_flow",             "get",
//   "dc_r2in_flow",	  "dc_r2in_flow",              "get",
//   "dc_r2in_h2o",	  "dc_r2in_h2o",               "get",
//   "dc_r2in_o2",		  "dc_r2in_o2",                "get",
//   "dc_r2in_pres",	  "dc_r2in_pres",              "get",
//   "dc_r2in_temp",	  "dc_r2in_temp",              "get",
//   "dc_r2io_pres",	  "dc_r2io_pres",              "get",
//   "dc_r2out_h2o",	  "dc_r2out_h2o",              "get",
//   "dc_r2out_o2",	  "dc_r2out_o2",               "get",
//   "dc_r2out_pres",	  "dc_r2out_pres",             "get",
//   "dc_r2out_temp",	  "dc_r2out_temp",             "get",
//   "dc_r3exh_flow",	  "dc_r3exh_flow",             "get",
//   "dc_r3in_flow",	  "dc_r3in_flow",              "get",
//   "dc_r3in_h2o",	  "dc_r3in_h2o",               "get",
//   "dc_r3in_o2",		  "dc_r3in_o2",                "get",
//   "dc_r3in_pres",	  "dc_r3in_pres",              "get",
//   "dc_r3in_temp",	  "dc_r3in_temp",              "get",
//   "dc_r3io_pres",	  "dc_r3io_pres",              "get",
//   "dc_r3out_h2o",	  "dc_r3out_h2o",              "get",
//   "dc_r3out_o2",	  "dc_r3out_o2",               "get",
//   "dc_r2out_pres",	  "dc_r2out_pres",             "get",
//   "dc_r2out_temp",	  "dc_r2out_temp",             "get",
//   "dc_r3exh_flow",	  "dc_r3exh_flow",             "get",
//   "dc_r3in_flow",	  "dc_r3in_flow",              "get",
//   "dc_r3in_h2o",	  "dc_r3in_h2o",               "get",
//   "dc_r3in_o2",		  "dc_r3in_o2",                "get",
//   "dc_r3in_pres",	  "dc_r3in_pres",              "get",
//   "dc_r3in_temp",	  "dc_r3in_temp",              "get",
//   "dc_r3io_pres",	  "dc_r3io_pres",              "get",
//   "dc_r3out_h2o",	  "dc_r3out_h2o",              "get",
//   "dc_r3out_o2",	  "dc_r3out_o2",               "get",
//   "dc_r3out_pres",	  "dc_r3out_pres",             "get",
//   "dc_r3out_temp",	  "dc_r3out_temp",             "get",
// };


E 8
D 9
static char epics_name[sizeof(epics)/sizeof(epics_struct)][32];
static int epics_val[sizeof(epics)/sizeof(epics_struct)];
E 9
I 9
D 12
float bosarray[sizeof(epics)/sizeof(epics_struct)][9];   // F,32A
E 12
D 13
// ??? static char epics_name[sizeof(epics)/sizeof(epics_struct)][32];
I 12
float              bosarray[sizeof(epics)/sizeof(epics_struct)][9];   // F,32A
E 13
I 13
D 99
float              bosarray[sizeof(epics)/sizeof(epics_struct)][9];   // F,8A
E 99
I 99
D 112
static float      bosarray[sizeof(epics)/sizeof(epics_struct)][9];   // F,8A
E 99
E 13
static float      epics_val[sizeof(epics)/sizeof(epics_struct)];
E 112
I 112
D 114
static float     bosarray[sizeof(epics)/sizeof(epics_struct)][9];   // F,8A
static float     epics_val[sizeof(epics)/sizeof(epics_struct)];
static char      epics_valstr[sizeof(epics)/sizeof(epics_struct)][VALSTRLEN];
E 114
I 114
static float  bosarray[sizeof(epics)/sizeof(epics_struct)][9];   // F,8A
static float  epics_val[sizeof(epics)/sizeof(epics_struct)];
static char   epics_valstr[sizeof(epics)/sizeof(epics_struct)][VALSTRLEN];
I 118

E 118
// for scaler data
I 118
static int    nscalers = sizeof(epics)/sizeof(epics_struct);
E 118
static int    scalers_nval[sizeof(epics)/sizeof(epics_struct)];
static float  scalers_val[sizeof(epics)/sizeof(epics_struct)][MAX_CHANNELS_LENGTH];
static char   scalers_valstr[sizeof(epics)/sizeof(epics_struct)][VALSTRLEN*MAX_CHANNELS_LENGTH];

E 114
D 115
#ifdef USECDEV
E 112
D 17
cdevRequestObject      *obj[sizeof(epics)/sizeof(epics_struct)];
E 17
I 17
cdevRequestObject      *get[sizeof(epics)/sizeof(epics_struct)];
D 27
cdevRequestObject      *mon[sizeof(epics)/sizeof(epics_struct)];
E 27
E 17
cdevCallback            *cb[sizeof(epics)/sizeof(epics_struct)];
I 112
#endif
E 115
E 112
I 27
D 81
cdevRequestObject      *mon[sizeof(epics)/sizeof(epics_struct)];
E 27
I 17
cdevCallback         *moncb[sizeof(epics)/sizeof(epics_struct)];
E 81
I 36
D 118
static int ncallback;  
I 48
D 114
static int nepics              	 = sizeof(epics)/sizeof(epics_struct);
E 114
I 114
static int nepics    = sizeof(epics)/sizeof(epics_struct);
static int nscalers  = sizeof(epics)/sizeof(epics_struct);
E 118
I 118
// for gamma data
static int    ngamma = sizeof(epics)/sizeof(char*);
static float  gammaarray[sizeof(epics)/sizeof(epics_struct)][9];   // F,8A
static float  gamma_val[sizeof(epics)/sizeof(epics_struct)];
static char   gamma_valstr[sizeof(epics)/sizeof(epics_struct)][VALSTRLEN];
E 118
E 114
E 48
E 36
E 17
I 15
D 27
time_t             last_try[sizeof(epics)/sizeof(epics_struct)];
E 27
E 15
E 12


I 99
D 104
// for bpm data
D 101
static const char *bpmname[] = {"IPM2C21A","IPM2C21A","IPM2C21A",
				"IPM2C24A","IPM2C24A","IPM2C24A"};
static const char *bpmget[] = {"get","get XPOS","get YPOS",
			       "get","get XPOS","get YPOS"};
E 101
I 101
static const char *bpmname[] = 
{"IPM2C21A","IPM2C21A","IPM2C21A",
 "IPM2C24A","IPM2C24A","IPM2C24A",
D 102
 "scaler_dS2b","scaler_dS3b","scaler_dS4b","scaler_dS14b"};
E 102
I 102
 "scaler_dS2b","scaler_dS3b","scaler_dS4b","scaler_dS14b",
 "coh_edge","gp_photons_x","gp_photons_y"};
E 102
static const char *bpmget[] = 
{"get","get XPOS","get YPOS",
 "get","get XPOS","get YPOS",
D 102
 "get","get","get","get"};
E 102
I 102
 "get","get","get","get",
 "get","get","get"};
E 102
E 101
static cdevRequestObject *bpmreq[sizeof(bpmname)/sizeof(char*)];
static float bpmarray[sizeof(bpmname)/sizeof(char*)][9];   // F,8A
static int nbpm = sizeof(bpmname)/sizeof(char*);


E 104
E 99
I 81
// for scaler data
#define MAXSCALER 1000
I 112
D 115
#ifdef USECDEV
E 112
D 96
cdevRequestObject *sreq;
E 96
I 96
static cdevRequestObject *sreq1;
static cdevRequestObject *sreq2;
static cdevRequestObject *sreq3;
I 106
static cdevRequestObject *sreq4;
I 112
#endif
E 115
E 112
E 106
E 96

D 112

E 112
E 81
D 12

static float epics_val[sizeof(epics)/sizeof(epics_struct)];
E 9
E 7
static int ncallback = 0;
D 7
cdevRequestObject *obj[sizeof(epics_chan)/sizeof(char *)];
cdevCallback *cb[sizeof(epics_chan)/sizeof(char *)];
E 7
I 7
cdevRequestObject *obj[sizeof(epics)/sizeof(epics_struct)];
cdevCallback *cb[sizeof(epics)/sizeof(epics_struct)];
E 7


E 12
// misc variables
D 20
static char *application       = "clastest";
static char *unique_id         = "epics_monitor";
D 5
static char *current_run_file  = "/usr/local/clas/parms/run_log/current_run_%s.txt";
static int wait_time           = 20;
E 5
I 5
D 9
static int wait_time           = 10;
E 9
I 9
static int wait_time           = 20;
E 9
E 5
static int cdev_pend_time      = 5;
I 15
static int cdev_retry_time     = 300;
E 15
static Tcl_Interp *interp;   
static char *init_tcl_script   = NULL;
I 5
static char *msql_database     = "clasrun";
E 5
static int done                = 0;
I 5
static int dump                = 0;
E 5
static int no_dd               = 0;
I 12
static int force_dd            = 0;
E 12
I 4
static int no_ipc              = 0;
I 7
static int nepics              = sizeof(epics)/sizeof(epics_struct);
I 12
static int ncallback;
I 18
static int nev_to_dd           = 0;
E 18
I 17
static int nev_no_dd           = 0;
E 20
I 20
D 91
static char *application       	 = "clastest";
E 91
I 91
static char *application       	 = (char*)"clastest";
E 91
I 47
D 72
char *session          		 = getenv("DD_NAME");
E 72
I 72
char *session          		 = NULL;
E 72
D 88
char *clon_root        		 = getenv("CLON_ROOT");
E 88
I 88
char *clon_parms       		 = getenv("CLON_PARMS");
E 88
E 47
I 42
static char *host         	 = getenv("HOST");
E 42
D 91
static char *unique_id         	 = "epics_monitor";
E 91
I 91
static char *unique_id         	 = (char*)"epics_monitor";
E 91
I 55
static char dest[132];
I 87
D 88
static char *epics_config_name	 = "parms/epics/epics_monitor.cfg" ;
E 87
E 55
D 21
static char *epics_summary_name	 = "/usr/local/clas/parms/epics/epics_summary.txt" ;
E 21
I 21
D 81
static char *epics_summary_name	 = "parms/epics/epics_summary.txt" ;
E 81
I 81
static char *epics_channel_name	 = "parms/epics/epic_summary.txt" ;
static char *epics_scaler_name	 = "parms/epics/tesc_summary.txt" ;
E 88
I 88
D 91
static char *epics_config_name	 = "epics/epics_monitor.cfg" ;
static char *epics_channel_name	 = "epics/epic_summary.txt" ;
static char *epics_scaler_name	 = "epics/tesc_summary.txt" ;
E 88
E 81
I 61
D 85
static char *hel_file_name	 = "/group/clas/online/eg1/eg12poltarg.dat" ;
E 85
I 85
static char *hel_file_name	 = "/group/clon/eg1/eg12poltarg.dat" ;
E 91
I 91
D 114
static char *epics_config_name	 = (char*)"epics/epics_monitor.cfg" ;
D 96
static char *epics_channel_name	 = (char*)"epics/epic_summary.txt" ;
static char *epics_scaler_name	 = (char*)"epics/tesc_summary.txt" ;
E 96
I 96
static char *epics_channel_name	 = (char*)"epics/EPIC_summary.txt" ;
I 99
D 104
static char *epics_bpm_name	 = (char*)"epics/BPM_summary.txt" ;
E 104
E 99
static char *epics_scaler_name	 = (char*)"%s/epics/%s_summary.txt" ;
E 96
D 112
static char *hel_file_name	 = (char*)"/group/clon/eg1/eg12poltarg.dat" ;
E 112
E 91
E 85
E 61
E 21
D 76
static int wait_time           	 = 20;
I 49
D 55
static int restart_time        	 = 30;
E 55
E 49
D 34
static int cdev_pend_time      	 = 5;
E 34
I 34
D 50
static int cdev_pend_time      	 = 3;
E 50
I 50
static int cdev_pend_time      	 = 1;
E 76
I 76
D 81
static int wait_time           	 = 14;
E 81
I 81
static int channel_wait_time   	 = 20;
I 99
D 100
static int bpm_wait_time   	 = 1;
E 100
I 100
D 103
static float delta_bpm           = 0.5;
E 103
E 100
E 99
D 104
static int scaler_wait_time   	 = 60;
E 104
I 104
D 105
static int scaler_wait_time   	 = 5;
E 104
E 81
static int cdev_pend_time      	 = 2;
E 105
I 105
static int scaler_wait_time   	 = 3;
E 114
I 114

static char *epics_config_name	       = (char*)"epics/epics_monitor.cfg";
static char *epics_channel_name	       = (char*)"epics/EPIC_summary.txt";
static int channel_wait_time   	       = 20;

static char *epics_scaler_config_name  = (char*)"epics/epics_scalers_monitor.cfg";
D 120
static char *epics_scaler_name	       = (char*)"%s/epics/%s_summary.txt";
E 120
static int scaler_wait_time   	       = 3;
I 118

static char *epics_gamma_config_name   = (char*)"epics/epics_gamma_monitor.cfg";
static char *epics_gamma_name	       = (char*)"epics/GAMMA_summary.txt" ;
static double gamma_wait_time          = 2.0;

E 118
D 116

E 114
static int cdev_pend_time      	 = 1;
E 105
E 76
D 77
static int cdev_pend_repeat   	 = 3;
E 77
I 77
static int cdev_pend_repeat   	 = 4;
E 116
I 116
static int ipc_pend_time         = 1;
E 116
I 103
D 104
static double bpm_wait_time      = 0.5;
static double bpm_pend_time      = 0.2;
E 104
E 103
I 81
static time_t last_channel       = 0;
I 99
D 100
static time_t last_bpm           = 0;
E 100
E 99
static time_t last_scaler        = 0;
I 118
static time_t last_gamma         = 0;
E 118
E 81
E 77
E 50
E 34
D 36
static int cdev_retry_time     	 = 300;
E 36
D 112
static Tcl_Interp *interp;     
static char *init_tcl_script   	 = NULL;
E 112
D 55
static char *msql_database     	 = "clasrun";
E 55
I 55
D 72
static int nev_to_dd             = 0;
E 72
I 72
static int nev_to_ipc            = 0;
E 72
E 55
static int done                	 = 0;
static int dump                	 = 0;
D 55
static int no_dd               	 = 0;
static int force_dd            	 = 0;
E 55
I 55
D 72
static int no_dd              	 = 0;
E 72
E 55
static int no_ipc              	 = 0;
I 72
static int no_info             	 = 0;
E 72
D 48
static int nepics              	 = sizeof(epics)/sizeof(epics_struct);
E 48
I 48
static int no_file             	 = 0;
I 61
D 84
static int no_hel_file         	 = 0;
E 84
I 84
D 112
static int no_hel_file         	 = 1;
E 112
I 95
static int no_scalers         	 = 0;
E 95
E 84
E 61
static int nepics_read         	 = 0;
I 118
static int ngamma_read         	 = 0;
E 118
E 48
D 36
static int ncallback;  
E 36
D 55
static int nev_to_dd           	 = 0;
static int nev_no_dd           	 = 0;
I 41
static int nev_no_run          	 = 0;
E 55
E 41
I 28
static int debug                 = 0;
I 46
D 49
static int once_only             = 0;
I 47
static int restart               = 0;
E 49
I 49
static int lost_connection       = 0;
E 49
E 47
E 46
I 31
D 55
static int dd_ok                 = 0;
D 46
static int dd_reconnect          = 1;
E 46
static int run                   = 0;
I 35
static int run_status            = 0;
E 55
I 47
static char temp[256];
I 58
D 62
static char *blanks              = "                                     ";
E 62
I 62
D 67
static char *blanks              = "                                      ";
E 67
E 62
I 61
D 91
static char *msql_database    	 = "clasrun";
E 91
I 91
static char *msql_database    	 = (char*)"clasrun";
E 91
static float *bpm1,*bpm3;
I 87
static char buffer[256];
E 87
E 61
E 58
E 47
E 35
E 31
E 28
E 20
E 17
E 12
E 7
E 4

I 12

I 31
D 55
// for DD
static int dd_mode                  = FMODE_ALL;
static int dd_wait                  = DD_WAIT_SLEEP;
static int dd_prescale              = 1;
static int dd_user                  = FMODE_MULTI_USER;
static int dd_ctl[FIFO_HDR_CTL_LEN] = {-1,-1,-1,-1};
E 55
D 58

E 58
D 55

E 55
E 31
E 12
D 7

E 7
I 5
D 9
//  ??? only available in cdev 1.6
E 9
E 5
D 116
// file descriptors for services registered with the cdev system object
D 5
static int ipcfd;
static cdevUserFdCbkId ipcid;
E 5
I 5
D 9
// static int ipcfd;
// static cdevUserFdCbkId ipcid;
E 9
I 9
static int ipcfd;
I 112
D 115
#ifdef USECDEV
E 115
I 115
/* ?????
E 115
E 112
static cdevUserFdCbkId ipcid;
I 112
D 115
#endif
E 115
I 115
*/
E 115
E 112
E 9
E 5

E 116
D 112

E 112
// other prototypes
void decode_command_line(int argc, char **argv);
D 112
void init_tcl(void);
E 112
I 112
int  init_epics(void);
D 115
#ifdef USECDEV
E 112
D 29
int init_dd(void);
E 29
I 29
D 72
void init_dd(void);
E 29
D 31
void insert_into_dd(int run);
E 31
I 31
void insert_into_dd();
E 72
E 31
void init_ipc_callbacks(void);
I 114
#endif
E 115
E 114
D 112
void init_epics(void);
E 112
I 112
D 113
void unpack_scaler_data(const char* bank, const int nrecord, cdevData result,
                        const char* channel);
E 113
I 113
void unpack_scaler_data(const char* bank, const int nrecord,
                        size_t nscaler, float *scalers, const char* channel);
E 113
D 114
#endif
E 114
E 112
I 48
D 67
void reinit_epics(void);
E 67
E 48
I 5
D 81
void process_loop(void);
E 81
I 81
void *channel_thread(void *param);
I 99
D 104
void *bpm_thread(void *param);
E 104
E 99
D 112
void *scaler_thread(void *param);
I 96
D 106
void unpack_scaler_data(const char* bank, cdevData result, const char* channel);
E 106
I 106
void unpack_scaler_data(const char* bank, const int nrecord, cdevData result, const char* channel);
E 112
E 106
E 96
E 81
I 67
D 115
void attach_epics(void);
E 67
E 5
void get_epics_data(void);
I 67
void detach_epics(void);
E 115
I 112
void *scaler_thread(void *param);
I 118
void *gamma_thread(void *param);
E 118
D 115
#ifdef USECDEV
E 112
E 67
D 81
void epics_callback_func(int status, void *userarg, cdevRequestObject &epics_req_obj,
E 81
I 81
void channel_callback_func(int status, void *userarg, cdevRequestObject &epics_req_obj,
E 81
			    cdevData& result);
I 112
#endif
E 115
E 112
I 99
D 104
void bpm_callback_func(int status, void *userarg, cdevRequestObject &epics_req_obj,
		       cdevData& result);
E 104
E 99
I 93
int find_tag_line(ifstream &file, const char *tag, char buffer[], int buflen);
int get_next_line(ifstream &file, char buffer[], int buflen);
E 93
I 91
extern "C" {
I 116
void *ipc_thread(void *param);
E 116
E 91
I 17
D 49
void monitor_callback_func(int status, void *userarg, cdevRequestObject &epics_req_obj,
			    cdevData& result);
E 49
E 17
void quit_callback(int sig);
D 8
int get_run_status(char *session);
E 8
void status_poll_callback(T_IPC_MSG msg);
D 116
int ipc_mainloop(int opened, int fd, void *arg);
E 116
D 112
int tcl_help(ClientData clientdata, Tcl_Interp *interp, 
		int argc, char **argv);
int tcl_quit(ClientData clientdata, Tcl_Interp *interp, 
		int argc, char **argv);
I 14
int tcl_dump(ClientData clientdata, Tcl_Interp *interp, 
		int argc, char **argv);
E 112
I 87
D 91
int find_tag_line(ifstream &file, char *tag, char buffer[], int buflen);
E 91
I 91
D 93
int find_tag_line(ifstream &file, const char *tag, char buffer[], int buflen);
E 91
int get_next_line(ifstream &file, char buffer[], int buflen);
E 93
E 87
E 14
I 12
D 31

E 31
I 31
D 55
void control_message_callback(T_IPC_CONN,
			      T_IPC_CONN_PROCESS_CB_DATA,
			      T_CB_ARG);
E 55
E 31
E 12
D 91
extern "C" {
I 61
int get_run_number(char *msql_database, char *session);
E 61
I 27
D 29
int ddu_exists(void);
E 29
E 27
I 5
D 55
int get_run_number(char *msql_database, char *session);
I 8
int get_run_status(char *session);
E 8
E 5
int create_header(int *p, int fevlen, int &banksize,
D 15
                  int nrun, int nevnt, int nphys, int trig);
E 15
I 15
                  int name1, int name2, int nrun, int nevnt, int nphys, int trig);
E 15
int add_bank(int *p2fev, int fevlen, 
      char *name, int num, char *format, int ncol, int nrow, int ndata, int &banksize, int *data);
int va_add_bank(int *p2fev, int fevlen, 
      char *name, int num, char *format, int ncol, int nrow, int ndata, int &banksize, ...);
E 55
D 6
int insert_msg(char *name, char *facility, char *process, char *msgclass, char *severity, 
                   int code, char *message);
E 6
I 6
int insert_msg(char *name, char *facility, char *process, char *msgclass, 
D 7
	       int severity, char *status, int code, char *message);
E 7
I 7
	       int severity, char *status, int code, char *text);
E 91
I 91
D 118
int get_run_number(const char *msql_database, const char *session);
E 118
I 118
//int get_run_number(const char *msql_database, const char *session);
E 118
int insert_msg(const char *name, const char *facility, const char *process, const char *msgclass, 
	       int severity, const char *status, int code, const char *text);
E 91
E 7
E 6
}


// ref to IPC server (connection created later)
TipcSrv &server=TipcSrv::Instance();

D 112

E 112
I 112
D 115
#ifdef USECDEV
E 112
// ref to cdev system object
cdevSystem &cdevsys = cdevSystem::defaultSystem ();
I 112
#endif
E 112

E 115
D 112

E 112
D 47
// session name is same as dd name
char *session          = getenv("DD_NAME");
I 31
char *clon_root        = getenv("CLON_ROOT");
E 31


E 47
//--------------------------------------------------------------------------

D 112

D 46
main(int argc,char **argv){
E 46
I 46
main(int argc,char **argv) {
E 46


E 112
I 112
int
main(int argc,char **argv)
{
E 112
D 5
  int status,run_status;
E 5
I 5
  int status;
I 81
D 100
  pthread_t t1,t2;
  int r1=0,r2=0;
E 100
I 100
D 116
  pthread_t t1,t2,t3;
  int r1=0,r2=0,r3=0;
E 116
I 116
  pthread_t t1;
E 116
E 100
E 81
E 5
D 47
  strstream temp;
E 47
I 47
  strstream temp2;
I 81
  time_t now;
I 118
  /* precise timing for gamma */
  timespec current  = {0,0};
  timespec lgamma     = {0,0};
  double dels,deln,deltat;
E 118
I 100
D 104
  timespec current  = {0,0};
  timespec lbpm     = {0,0};
  double dels,deln,deltat;
E 104
E 100
E 81
E 47
D 8
  char filename[120];
E 8
D 31
  int run;
E 31

I 118

E 118
D 115

E 115
I 35
  // synch with stdio
  ios::sync_with_stdio();

D 112

E 112
I 112
D 115
#ifdef USECDEV
E 112
I 46
  // only print cdev error messages
  cdevsys.setThreshold(CDEV_SEVERITY_ERROR);
I 112
#endif
E 112

E 115
D 112

E 46
E 35
  // decode command line...flags may be overridden in Tcl startup script
E 112
I 112
  // decode command line...
E 112
  decode_command_line(argc,argv);


  // set session name if not specified via env variable or on command line
D 91
  if(session==NULL)session="clasprod";
E 91
I 91
D 112
  if(session==NULL)session=(char*)"clasprod";
E 112
I 112
  if(session==NULL) session=(char*)"clasprod";
E 112
E 91


I 31
D 55
  // bombproof
  if(clon_root==NULL) {
    cerr << "?CLON_ROOT not defined...using default..." << endl;
    clon_root="/usr/local/clas";
  }
E 55
I 55
  // get event destination
D 72
  sprintf(dest,"dd_bosbank/%s",session);
E 72
I 72
  sprintf(dest,"evt_bosbank/%s",session);
E 72
E 55


E 31
D 112
  // get Tcl interp, create tcl commands, link vars, process Tcl startup script, etc.
  interp=tclinterp_init();
  init_tcl();
  if(init_tcl_script!=NULL)Tcl_EvalFile(interp,init_tcl_script);


E 112
D 17
  // connect to dd system, die on error (must be before ipc_init, as dd system messes up signals!)
E 17
I 17
D 18
  // connect to dd system, die on error
E 17
  if(no_dd==0){
    if(init_dd()!=0){ exit(EXIT_FAILURE); }
  }
E 18
I 18
D 55
  // connect to dd system
  if(no_dd==0)init_dd();
E 18


E 55
I 10
D 46
  // only print cdev error messages
  cdevsys.setThreshold(CDEV_SEVERITY_ERROR);


E 46
D 17
  // initialize epics callbacks
E 17
I 17
D 48
  // initialize epics request objects and callbacks
E 17
  init_epics();


E 48
E 10
  // set ipc parameters and connect to ipc system
I 81
  if(!TipcInitThreads()) {
    cerr << "Unable to init IPC thread package" << endl;
    exit(EXIT_FAILURE);
  }
E 81
  ipc_set_application(application);
  ipc_set_user_status_poll_callback(status_poll_callback);
I 31
D 55
  ipc_set_control_message_callback(control_message_callback);
E 55
E 31
  ipc_set_quit_callback(quit_callback);
  status=ipc_init(unique_id,"Epics monitor");
D 46
  if(status<0){
E 46
I 46
  if(status<0) {
E 46
    cerr << "\n?Unable to connect to server...probably duplicate unique id\n"
	 << "   ...check for another epics_monitor  using ipc_info\n"
	 << "   ...only one connection allowed!" << endl << endl;
    exit(EXIT_FAILURE);
  }
I 32
D 46
  server.SubjectSubscribe("dd_control",TRUE);
E 46
I 46
D 49
  if(once_only==0)server.SubjectSubscribe("dd_control",TRUE);
E 49
I 49
D 53
  server.SubjectSubscribe("dd_control",TRUE);
E 53
I 53
D 55
  server.SubjectSubscribe("dd_system",TRUE);
E 55
E 53
E 49
E 46
E 32


D 17
  // enable Tcl ipc control (i.e. create tcl_request callback)
E 17
I 17
D 112
  //  create tcl_request callback
E 17
D 46
  tclipc_init(interp);
E 46
I 46
D 49
  if(once_only==0)tclipc_init(interp);
E 49
I 49
  tclipc_init(interp);
E 49
E 46


E 112
I 112
D 115
#ifdef USECDEV
E 115
I 115
D 116
  /* ?????
E 115
E 112
D 10
  // only print cdev error messages
  cdevsys.setThreshold(CDEV_SEVERITY_ERROR);


I 7

E 7
  // set up all callbacks
E 10
I 10
  // set up ipc cdev callbacks
E 10
D 46
  init_ipc_callbacks();
E 46
I 46
D 49
  if(once_only==0)init_ipc_callbacks();
E 49
I 49
D 115
  init_ipc_callbacks();
I 112
#endif
E 115
I 115
  ipcfd=server.XtSource();
  cdevsys.addUserFdCallback(ipcfd,ipc_mainloop,(void *)0,ipcid);
  */
E 115
E 112
E 49
E 46
D 10
  init_epics();
E 10


I 112

E 116
  /* EPICS stuff */

D 115
#ifndef USECDEV
E 115
  for(int i=0; i<nepics; i++)
I 118
  {
E 118
I 114
    epics_val[i]=-9999.;
I 118
  }
I 121

E 121
E 118
  for(int i=0; i<nscalers; i++)
E 114
  {
D 114
    epics_val[i]=-9999.; /* ??? */
E 114
I 114
    scalers_nval[i] = 0;
    for(int j=0; j<MAX_CHANNELS_LENGTH; j++)
      scalers_val[i][j]=-9999.;
E 114
  }
I 121

E 121
I 118
  for(int i=0; i<ngamma; i++)
  {
    gamma_val[i]=-9999.;
  }
E 118
D 115
#endif
E 115

E 112
I 48
D 49
  // initialize epics request objects and callbacks
E 49
I 49
  // initialize epics callbacks, request objects reinitialized each iteration
I 87
  // also read in epics channel list
E 87
E 49
  init_epics();

I 112
  /* EPICS stuff */
E 112

I 112

I 116
  // launch thread(s)
#ifdef SunOS
  thr_setconcurrency(thr_getconcurrency()+4);
#endif
  pthread_create(&t1,NULL,ipc_thread,(void*)NULL);


E 116
E 112
E 48
I 10
D 12

E 10
  // post startup message
  temp << "Process startup:    epics_monitor starting in " << application << ends;
D 3
  status=insert_msg("epics_monitor","epics_monitor",unique_id,"status","START",0,temp.str());
E 3
I 3
D 6
  //   status=insert_msg("epics_monitor","epics_monitor",unique_id,"status","START",0,temp.str());
E 6
I 6
D 7
  status=insert_msg("epics_monitor","epics_monitor",unique_id,"status",0,"START",0,temp.str());
E 7
I 7
D 9
  status=insert_msg("epics_monitor","online",unique_id,"status",0,"START",0,temp.str());
E 9
I 9
D 10
  // status=insert_msg("epics_monitor","online",unique_id,"status",0,"START",0,temp.str());
E 10
I 10
  status=insert_msg("epics_monitor","online",unique_id,"status",0,"START",0,temp.str());
E 10
E 9
E 7
E 6
E 3


D 5
  // insert epics data every wait_time seconds
  while (done==0){
    cdevsys.pend((double)wait_time);
    
    if(done==0){
      //   run_status=get_run_status(session);  ???
      run_status=6;
      if((run_status>=4)&&(run_status<=8)){
	
	// get current run number from file
	sprintf(filename,current_run_file,session);
	ifstream file(filename);
	file >> run;
	file.close();
	
	// get epics data
	get_epics_data();
	
	// insert into event stream
	if(no_dd==0){
	  insert_into_dd(run);
	} else {
D 3
	  cout << "\n insert_into_dd: " << endl;
E 3
I 3
	  cout << endl;
	  cout.setf(ios::left);
	  for(int i=0; i<sizeof(epics_chan)/sizeof(char *); i++){
	    cout << setw(32) << epics_name[i] << "    " << epics_val[i] << endl;
	  }
	  cout << endl;
E 3
	}
E 5
I 5
D 7
  // periodically insert events
E 7
I 7

E 12
D 62
  // copy epics channel names into special bos array 
E 62
I 62
D 67
  // copy epics channel names (31 char max) into special bos array...32nd char is NULL
I 64
  // concatenate get string
E 64
E 62
D 13
  for(int i=0; i<nepics; i++){
D 9
    strcpy(&epics_name[i][0],epics[i].name);
E 9
I 9
    // ???    strcpy(&epics_name[i][0],epics[i].name);
D 12
    strcpy((char *)(&bosarray[i][1]),epics[i].name);
E 12
I 12
    strncpy((char *)(&bosarray[i][1]),epics[i].chan,32);
E 12
E 9
  }
E 13
I 13
D 46
  for(int i=0; i<nepics; i++) strncpy((char *)(&bosarray[i][1]),epics[i].chan,32);
E 46
I 46
D 58
  if(no_dd==0)for(int i=0; i<nepics; i++) strncpy((char *)(&bosarray[i][1]),epics[i].chan,32);
E 58
I 58
  if(no_dd==0) {
    int i;
I 62
    blanks[31]='\0';
E 62
    for(i=0; i<nepics; i++) strncpy((char *)(&bosarray[i][1]),blanks,32);
D 64
    for(i=0; i<nepics; i++) strncpy((char *)(&bosarray[i][1]),epics[i].chan,
D 62
					min(strlen(epics[i].chan),32));
E 62
I 62
					min(strlen(epics[i].chan),31));
E 64
I 64
    for(i=0; i<nepics; i++) {
      if(strlen(epics[i].get)>4) {
	sprintf(temp,"%s.%s",epics[i].chan,&(epics[i].get)[4]);
	strncpy((char *)(&bosarray[i][1]),temp,min(strlen(temp),31));
      } else {
	strncpy((char *)(&bosarray[i][1]),epics[i].chan,min(strlen(epics[i].chan),31));
      }
    }
E 64
E 62
  }
E 58
E 46
E 13


E 67
I 12
  // post startup message
D 46
  temp << "Process startup:    epics_monitor starting in " << application << ends;
  status=insert_msg("epics_monitor","online",unique_id,"status",0,"START",0,temp.str());
E 46
I 46
D 48
  if(once_only==0) {
D 47
    temp << "Process startup:    epics_monitor starting in " << application << ends;
    status=insert_msg("epics_monitor","online",unique_id,"status",0,"START",0,temp.str());
E 47
I 47
    temp2 << "Process startup:    epics_monitor starting in " << application << ends;
    status=insert_msg("epics_monitor","online",unique_id,"status",0,"START",0,temp2.str());
E 47
  }
E 48
I 48
D 49
//   if(once_only==0) {
E 49
D 78
//     temp2 << "Process startup:    epics_monitor starting in " << application << ends;
//     status=insert_msg("epics_monitor","online",unique_id,"status",0,"START",0,temp2.str());
D 49
//   }
E 49
E 48
E 46

E 78
I 78
  temp2 << "Process startup:    epics_monitor starting in " << application << ends;
  status=insert_msg("epics_monitor","online",unique_id,"status",0,"START",0,temp2.str());
  
E 78
I 67

E 67
D 46

E 46
I 35
  // flush output to log files, etc
  fflush(NULL);


I 49
D 67

E 67
E 49
E 35
E 12
D 81
  // main loop
E 7
  process_loop();
E 81
I 81
D 99
  // launch threads for channel and scalers
E 99
I 99
D 104
  // launch threads for channels, bpms, and scalers
E 104
I 104
  // launch threads for channels, and scalers
E 104
E 99
  // debug...thread stuff not working yet with cdev and ca
D 100

E 100
D 113
  while (done==0) {
E 113
I 113
  while (done==0)
  {
E 113
D 118

E 118
I 118
    /* channels */
E 118
    now=time(NULL);
I 99
D 100

E 100
E 99
D 113
    if((now-last_channel)>channel_wait_time) {
E 113
I 113
    if((now-last_channel)>channel_wait_time)
    {
I 118
      /*printf("call channel_thread\n");*/
E 118
E 113
      channel_thread(0);
      last_channel=now;
    }

I 118
    /* scalers */
E 118
I 99
D 100
    if((now-last_bpm)>bpm_wait_time) {
E 100
I 100
D 104

    clock_gettime(CLOCK_REALTIME,&current);
    dels=(double)(current.tv_sec-lbpm.tv_sec);
    deln=(double)(current.tv_nsec-lbpm.tv_nsec)/1.0e9;
    deltat=(deln>0)?(dels+deln):(dels-1.-deln);
D 103
    if(deltat>delta_bpm) {
E 103
I 103
    if(deltat>bpm_wait_time) {
E 103
E 100
      bpm_thread(0);
D 100
      last_bpm=now;
E 100
I 100
      clock_gettime(CLOCK_REALTIME,&lbpm);
E 100
    }

I 100

E 104
E 100
E 99
D 95
    now=time(NULL);
    if((now-last_scaler)>scaler_wait_time) {
D 83
      scaler_thread(0);
E 83
I 83
D 94
      //      scaler_thread(0);
E 94
I 94
      scaler_thread(0);
E 94
E 83
      last_scaler=now;
E 95
I 95
D 113
    if(no_scalers==0) {
E 113
I 113
    if(no_scalers==0)
    {
E 113
      now=time(NULL);
D 113
      if((now-last_scaler)>scaler_wait_time) {
E 113
I 113
      if((now-last_scaler)>scaler_wait_time)
      {
I 118
        /*printf("call scaler_thread\n");*/
E 118
E 113
D 112
	scaler_thread(0);
E 112
I 112
D 116
		scaler_thread(0); /* ?????????????????????????????? */
E 116
I 116
		scaler_thread(0);
E 116
E 112
D 113
	last_scaler=now;
E 113
I 113
	    last_scaler=now;
E 113
      }
E 95
    }

I 118
    /* use precise clock */
    clock_gettime(CLOCK_REALTIME,&current);
    dels=(double)(current.tv_sec-lgamma.tv_sec);
    deln=(double)(current.tv_nsec-lgamma.tv_nsec)/1.0e9;
    deltat=(deln>0)?(dels+deln):(dels-1.-deln);
    if(deltat>gamma_wait_time)
    {
      /*printf("call gamma_thread\n");*/
      gamma_thread(0);
      clock_gettime(CLOCK_REALTIME,&lgamma);
    }





E 118
I 112
D 115
#ifdef USECDEV
E 112
D 99
    cdevsys.pend(1.0);
E 99
I 99
D 100
    cdevsys.pend(0.1);
E 100
I 100
    cdevsys.pend(0.05);
I 112
#else
E 115
    /* Read and print data forever */
D 116
    ca_pend_event(0.05/*0*/);
E 116
I 116
D 117
    ca_pend_event(/*0.05*/0);
E 117
I 117
    ca_pend_event(0.05/*0*/);
E 117
E 116
D 115
#endif
E 115
E 112
E 100
E 99
  }

D 100
  //  thr_setconcurrency(thr_getconcurrency()+5);
E 100
I 100
D 112
  //  thr_setconcurrency(thr_getconcurrency()+6);
E 100
  //  pthread_create(&t1,NULL,channel_thread,(void *)&r1);
D 95
  //  pthread_create(&t2,NULL,scaler_thread,(void *)&r2);
E 95
I 95
D 100
  //  if(no_scalers==0)pthread_create(&t2,NULL,scaler_thread,(void *)&r2);
E 100
I 100
D 104
  //  pthread_create(&t2,NULL,bpm_thread,(void *)&r2);
E 104
  //  if(no_scalers==0)pthread_create(&t3,NULL,scaler_thread,(void *)&r3);
E 100
E 95
  //  pthread_join(t1,NULL);
E 112
I 112
D 115
#ifndef USECDEV
    /* Shut down Channel Access */
    ca_context_destroy();
#endif
E 115
I 115
  /* Shut down Channel Access */
  ca_context_destroy();
E 115
E 112
E 81
E 5
I 4

D 5
	// create epics message
	if(no_ipc==0){
	  TipcMsg message("info_server");
	  message.Sender("epics_monitor");
	  message.Dest("info_server");
	  message << "epics_monitor";
	  for(int i=0; i<sizeof(epics_chan)/sizeof(char *); i++){
	    message << (T_STR) epics_name[i] << (T_INT4) epics_val[i];
	  }
	  server.Send(message);
	  server.Flush();
	}
E 5
D 115

E 115
I 47
D 48
  if((once_only==1)&&(restart>0)) {
    sprintf(temp,"%s/start_epics_monitor %d %s %s &",getenv("CLON_BIN"),restart,session,application);
    if(debug==1)cout << "Restarting: " << temp << endl;
    system(temp);
  }
E 48
I 48
D 49
//   if((once_only==1)&&(restart>0)) {
//     sprintf(temp,"%s/start_epics_monitor %d %s %s &",getenv("CLON_BIN"),restart,session,application);
//     if(debug==1)cout << "Restarting: " << temp << endl;
//     system(temp);
//   }
E 49
E 48
D 67

E 67
I 49
D 55
  // restart if lost dd connection
  if(lost_connection>0) {
    time_t now = time(NULL);
    cout << "lost connection to DD, restarting at " << ctime(&now) << endl;
    sprintf(temp,"%s/start_epics_monitor %d %s %s &",getenv("CLON_BIN"),restart_time,session,application);
    if(debug==1) {
      cout << "Restarting: " << temp << endl;
    } else {
      system(temp);
    }
  }
E 49
I 48

E 48
E 47
E 4
D 5
      }
    }

  }

E 5
D 12
  // done
E 12
I 12
D 49
  // done...clean up and post shutdown message
E 49
I 49

E 55
  // post shutdown message
D 78
//   temp2.seekp(0,ios::beg);
//   temp2 << "Process shutdown:  epics_monitor" << ends;
//   status=insert_msg("epics_monitor","online",unique_id,"status",0,"STOP",0,temp2.str());
E 78
I 78
  temp2.seekp(0,ios::beg);
  temp2 << "Process shutdown:  epics_monitor" << ends;
  status=insert_msg("epics_monitor","online",unique_id,"status",0,"STOP",0,temp2.str());
E 78
  
D 115

E 115
  // done...clean up
E 49
E 12
D 46
  if(no_dd==0)ddu_close();
E 46
I 46
D 55
  if((no_dd==0)&&(dd_ok==1))ddu_close();
E 55
E 46
  ipc_close();
D 46
  temp.seekp(0,ios::beg);
  temp << "Process shutdown:  epics_monitor" << ends;
D 3
  status=insert_msg("epics_monitor","epics_monitor",unique_id,"status","STOP",0,temp.str());
E 3
I 3
D 6
  //   status=insert_msg("epics_monitor","epics_monitor",unique_id,"status","STOP",0,temp.str());
E 6
I 6
D 7
  status=insert_msg("epics_monitor","epics_monitor",unique_id,"status",0,"STOP",0,temp.str());
E 7
I 7
D 9
  status=insert_msg("epics_monitor","online",unique_id,"status",0,"STOP",0,temp.str());
E 9
I 9
D 10
  // status=insert_msg("epics_monitor","online",unique_id,"status",0,"STOP",0,temp.str());
E 10
I 10
  status=insert_msg("epics_monitor","online",unique_id,"status",0,"STOP",0,temp.str());
E 46
I 46
D 49
  if(once_only==0) {
D 47
    temp.seekp(0,ios::beg);
    temp << "Process shutdown:  epics_monitor" << ends;
    status=insert_msg("epics_monitor","online",unique_id,"status",0,"STOP",0,temp.str());
E 47
I 47
    temp2.seekp(0,ios::beg);
    temp2 << "Process shutdown:  epics_monitor" << ends;
    status=insert_msg("epics_monitor","online",unique_id,"status",0,"STOP",0,temp2.str());
E 47
  }
E 46
E 10
E 9
E 7
E 6
E 3

E 49
  exit(EXIT_SUCCESS);
D 12

E 12
}
       

//--------------------------------------------------------------------------


D 46
void init_ipc_callbacks(){
E 46
I 46
D 67
void init_ipc_callbacks() {
E 46

D 5
  ipcfd=server.XtSource();
  cdevsys.addUserFdCallback(ipcfd,ipc_mainloop,(void *)0,ipcid);
E 5
I 5
D 9
//   ipcfd=server.XtSource();
//   cdevsys.addUserFdCallback(ipcfd,ipc_mainloop,(void *)0,ipcid);
E 9
I 9
  ipcfd=server.XtSource();
  cdevsys.addUserFdCallback(ipcfd,ipc_mainloop,(void *)0,ipcid);
E 9
E 5

  return;
}


//--------------------------------------------------------------------------


D 46
void init_epics(){
E 46
I 46
void init_epics() {
E 46

D 15
  // create all request objects and callbacks
E 15
I 15
D 17
  // create all request objects, callbacks, and zero last_try array
E 17
I 17
D 49
  int status;
E 49
D 48
  time_t now = time(NULL);
E 48

D 49
  // create all request objects, callbacks, etc.
E 49
I 49
  // create callback objects once only
E 49
E 17
E 15
D 7
  for(int i=0; i<sizeof(epics_chan)/sizeof(char *); i++){
    obj[i] = cdevRequestObject::attachPtr(epics_chan[i],epics_get[i]);
E 7
I 7
D 46
  for(int i=0; i<nepics; i++){
E 46
I 46
  for(int i=0; i<nepics; i++) {
E 46
D 17
    obj[i] = cdevRequestObject::attachPtr(epics[i].chan,epics[i].get);
E 7
    cb[i] = new cdevCallback(epics_callback_func,(void*)i);
I 15
    last_try[i]=0;
E 17
I 17
D 48
    get[i] = cdevRequestObject::attachPtr(epics[i].chan,epics[i].get);
E 48
I 48
D 49
//     get[i] = cdevRequestObject::attachPtr(epics[i].chan,epics[i].get);
E 49
E 48
    cb[i]    = new cdevCallback(epics_callback_func,(void*)i);
I 61

    if(strcmp(epics[i].name,"bpm_1_i")==0) bpm1=&(epics_val[i]);    // for helicity file
    if(strcmp(epics[i].name,"bpm_3_i")==0) bpm3=&(epics_val[i]);
E 61
D 27
    // mon[i] = cdevRequestObject::attachPtr(epics[i].chan,"monitorOn status");
    // moncb[i] = new cdevCallback(monitor_callback_func,(void*)i);
    // status=mon[i]->sendCallback(NULL,*moncb[i]);
    last_try[i]=now;
E 27
I 27
D 48
    mon[i] = cdevRequestObject::attachPtr(epics[i].chan,"monitorOn status");
    moncb[i] = new cdevCallback(monitor_callback_func,(void*)i);
    status=mon[i]->sendCallback(NULL,*moncb[i]);
E 48
I 48
D 49
//     mon[i] = cdevRequestObject::attachPtr(epics[i].chan,"monitorOn status");
//     moncb[i] = new cdevCallback(monitor_callback_func,(void*)i);
//     status=mon[i]->sendCallback(NULL,*moncb[i]);
E 49
E 48
E 27
E 17
E 15
  }
D 5

E 5
I 5
  
E 5
  return;
}


//--------------------------------------------------------------------------


I 48
D 60
void reinit_epics() {
E 60
I 60
void attach_epics() {
E 60

D 60
  // detach, then recreate all request objects
E 60
  for(int i=0; i<nepics; i++) {
D 60
    if((get[i]!=NULL)&&(get[i]->getState()==CDEV_STATE_CONNECTED)) cdevRequestObject::detach(get[i]);
E 60
    get[i] = cdevRequestObject::attachPtr(epics[i].chan,epics[i].get);
  }
  
  return;
}


//--------------------------------------------------------------------------


I 60
void detach_epics() {

  for(int i=0; i<nepics; i++) {
I 66
    //    if(get[i].getState()==CDEV_STATE_CONNECTED)cdevRequestObject::detach(get[i]);
E 66
    cdevRequestObject::detach(get[i]);
  }
  
  return;
}


//--------------------------------------------------------------------------


E 67
E 60
E 48
I 5
D 46
void process_loop(void){
E 46
I 46
D 81
void process_loop(void) {
E 81
I 81
D 112
void *channel_thread(void *param) {
E 112
I 112
void *
channel_thread(void *param)
{
E 112
E 81
E 46

I 81
D 115
  static int first=0;
E 81
D 12
  int run_status,run;
E 12
I 12
D 31
  int run;
E 31
E 12
D 112

D 7
  // insert epics data every wait_time seconds
  while (done==0){
    cdevsys.pend((double)wait_time);
    ipc_mainloop(1,0,(void *)0);              //  ??? until cdev 1.6 arrives
    
E 7
I 7
D 31

E 31
D 81
  while (done==0) {
I 48
    
D 49
    nepics_read++;
E 49
E 48

D 9
    // get epics data every wait_time seconds
    server.MainLoop((double)wait_time);        //  ??? until cdev 1.6 arrives
    //   cdevsys.pend((double)wait_time);
E 9
I 9
D 46
    // get epics data every wait_time seconds, also handle ipc messages
    cdevsys.pend((double)wait_time);
E 9

E 7
    if(done==0) {

D 7
      //   run_status=get_run_status(session);  ???
      run_status=6;
      if((run_status>=4)&&(run_status<=8)){
	
	// get current run number from msql
	run=get_run_number(msql_database,session);
	
	// get epics data
	get_epics_data();
	
	// insert into event stream
	if(no_dd==0){
E 7
I 7
      // get epics data
      get_epics_data();
      
I 27

E 27
D 12
      // insert into event stream if run in progress
E 12
I 12
      // insert into event stream if run in progress (GO state, status 11)
      //  or if force_dd set
E 12
      if(no_dd==0){
D 8
	//   run_status=get_run_status(session);  ??? rcserver broken
	run_status=6;
	if((run_status>=4)&&(run_status<=8)){
E 8
I 8
D 12
	run_status=get_run_status(session);
	if((run_status>=8)&&(run_status<=11)){
E 12
I 12
D 35
	if((force_dd==1)||(get_run_status(session)==11)){
E 35
I 35
	run_status=get_run_status(session);
D 41
	if((force_dd==1)||(run_status==11)){
E 41
I 41
	if((force_dd==1)||(run_status==11)) {
E 41
E 35
E 12
E 8
	  run=get_run_number(msql_database,session);
E 7
D 31
	  insert_into_dd(run);
E 31
I 31
	  insert_into_dd();
I 41
	} else {
	  nev_no_run++;
E 41
E 31
	}
E 46
I 46
D 67
    // get epics data
E 67
I 67
    // get epics data...detach after wait
E 67
I 48
D 49
    cout << "nepics_read is " << nepics_read << endl;
E 49
I 49
    nepics_read++;
I 66
D 67

E 67
E 66
E 49
D 60
    reinit_epics();
E 60
I 60
    attach_epics();
E 60
E 48
    get_epics_data();
I 60
D 66
    detach_epics();
E 66
E 60
    
    
D 55
    // insert into event stream if run in progress (GO state, status 11)
    //  or if force_dd set
E 55
I 55
D 72
    // ship to ipcbank2dd
E 55
    if(no_dd==0) {
D 55
      run_status=get_run_status(session);
      if((force_dd==1)||(run_status==11)) {
	run=get_run_number(msql_database,session);
	insert_into_dd();
      } else {
	nev_no_run++;
E 46
I 7
      }
E 55
I 55
      nev_to_dd++;
E 72
I 72
    // ship data
    if(no_ipc==0) {
      nev_to_ipc++;
E 72
      for(int i=0; i<nepics; i++) bosarray[i][0] = epics_val[i];
D 72
      TipcMsg msg("dd_bosbank");
E 72
I 72
      TipcMsg msg("evt_bosbank");
E 72
      msg.Sender("epics_monitor");
      msg.Dest(dest);
D 57
      msg << "EPIC" << (T_INT4)0 << "(F,8A)" << (T_INT4)2 << (T_INT4)nepics << (T_INT4)(nepics*9)
E 57
I 57
      msg << "EPIC" << (T_INT4)0 << "(F,8A)" << (T_INT4)9 << (T_INT4)nepics << (T_INT4)(nepics*9)
E 57
D 56
	  << (T_INT4*)bosarray;
E 56
I 56
	  << SetSize(nepics*9) << (T_INT4*)bosarray;
E 56
      server.Send(msg);
      server.Flush();
      
E 55
D 46
      
E 46
I 46
    }
E 81
I 81
  if(first==1) {
    detach_epics();
  } else {
E 112
I 112
  if(first==1)
  {
    detach_epics(); /* cdev only ??? */
  }
  else
  {
E 112
    first=1;
  }
E 81
D 55
    
E 55
E 46
E 7

I 55

E 55
D 7
	// create epics info server message
	if(no_ipc==0){
	  TipcMsg message("info_server");
	  message.Sender("epics_monitor");
	  message.Dest("info_server");
	  message << "epics_monitor";
	  for(int i=0; i<sizeof(epics_chan)/sizeof(char *); i++){
	    message << (T_STR) epics_name[i] << (T_INT4) epics_val[i];
	  }
	  server.Send(message);
	  server.Flush();
E 7
I 7
D 12
      // create epics info server message
E 12
I 12
D 46
      // create and send epics info_server message
E 12
D 23
      if(no_ipc==0){
E 23
I 23
      if(no_ipc==0) {
E 23
	TipcMsg message("info_server");
	message.Sender("epics_monitor");
	message.Dest("info_server");
	message << "epics_monitor";
	for(int i=0; i<nepics; i++){
D 9
	  message << (T_STR) epics[i].name << (T_INT4) epics_val[i];
E 9
I 9
D 17
	  message << (T_STR) epics[i].name << (T_REAL4) epics_val[i];
E 17
I 17
D 22
	  message << (T_STR) epics[i].chan << (T_REAL4) epics_val[i];
E 22
I 22
D 23
	  message << (T_STR) epics[i].chan << SetSize(1) << &(epics_val[i]) << Check;
E 23
I 23
	  double temp[1] = {(double)epics_val[i]};
	  message << (T_STR) epics[i].chan << SetSize(1) << temp << Check;
E 23
E 22
E 17
E 9
E 7
	}
I 7
	server.Send(message);
	server.Flush();
E 46
I 46
D 67
    // create and send epics info_server message
E 67
I 67
D 81
    // ship to info_server
E 67
D 72
    if(no_ipc==0) {
E 72
I 72
    if(no_info==0) {
E 72
      TipcMsg message("info_server");
      message.Sender("epics_monitor");
D 70
      message.Dest("info_server");
E 70
I 70
      message.Dest("info_server/in/epics_monitor");
E 70
      message << "epics_monitor";
      for(int i=0; i<nepics; i++) {
D 64
	double temp[1] = {(double)epics_val[i]};
	message << (T_STR) epics[i].chan << SetSize(1) << temp << Check;
E 64
I 64
	double tval[1] = {(double)epics_val[i]};
	if(strlen(epics[i].get)>4) {
	  sprintf(temp,"%s.%s",epics[i].chan,&(epics[i].get)[4]);
	  message << (T_STR) temp << SetSize(1) << tval << Check;
	} else {
	  message << (T_STR) epics[i].chan << SetSize(1) << tval << Check;
	}
E 64
E 46
      }
D 46
      
E 7

I 20
      // dump to file
D 21
      ofstream file(epics_summary_name);
E 21
I 21
      strstream fn;
D 31
      fn << getenv("CLON_ROOT") << "/" << epics_summary_name << ends; 
E 31
I 31
      fn << clon_root << "/" << epics_summary_name << ends; 
E 31
      ofstream file(fn.str());
E 21
      file.setf(ios::left);
I 26
      file.setf(ios::showpoint);
E 26
      time_t now=time(NULL);
      file << "\n#  epics summary created by epics_monitor on " << ctime(&now) << endl ;
      file << "#     ejw, 23-dec-97"  << endl << endl << endl;
      file << "*epics*" << endl;
      file << endl 
	   << setw(27) << "#Channel name" << setw(20) << "Channel value" 
	   << "Channel description" << endl;
      file << setw(27) << "#------------" << setw(20) << "-------------" 
	   << "-------------------" << endl;
      for(int i=0; i<nepics; i++){
	file << setw(27) << epics[i].chan << setw(20) << epics_val[i] 
	     << epics[i].name << endl;
E 46
I 46
      server.Send(message);
      server.Flush();
    }
    
E 81
I 81
  // get epics data
E 115
  nepics_read++;
D 112
  attach_epics();
  get_epics_data();
E 112
I 112
D 115
  attach_epics(); /* cdev only ??? */
  get_epics_data(); /* cdev only ??? */
E 115
E 112
  
I 113
D 115





E 113
  
E 115
  // ship data
D 112
  if(no_ipc==0) {
E 112
I 112
  if(no_ipc==0)
  {
E 112
    nev_to_ipc++;
I 89
    for(int i=0; i<nepics; i++) bosarray[i][0] = epics_val[i];
E 89
D 91
    TipcMsg msg("evt_bosbank");
    msg.Sender("epics_monitor");
E 91
I 91
    TipcMsg msg((T_STR)"evt_bosbank");
    msg.Sender((T_STR)"epics_monitor");
E 91
    msg.Dest(dest);
D 91
    msg << "EPIC" << (T_INT4)0 << "(F,8A)" << (T_INT4)9 << (T_INT4)nepics << (T_INT4)(nepics*9)
E 91
I 91
    msg << (T_STR)"EPIC" << (T_INT4)0 << (T_STR)"(F,8A)" << (T_INT4)9 << (T_INT4)nepics 
	<< (T_INT4)(nepics*9)
E 91
	<< SetSize(nepics*9) << (T_INT4*)bosarray;
    server.Send(msg);
    server.Flush();
E 81
D 99
    
E 99
D 81
    // dump to file
D 48
    strstream fn;
    fn << clon_root << "/" << epics_summary_name << ends; 
    ofstream file(fn.str());
    file.setf(ios::left);
    file.setf(ios::showpoint);
    time_t now=time(NULL);
    file << "\n#  epics summary created by epics_monitor on " << ctime(&now) << endl ;
    file << "#     ejw, 23-dec-97"  << endl << endl << endl;
    file << "*epics*" << endl;
    file << endl 
	 << setw(27) << "#Channel name" << setw(20) << "Channel value" 
	 << "Channel description" << endl;
    file << setw(27) << "#------------" << setw(20) << "-------------" 
	 << "-------------------" << endl;
    for(int i=0; i<nepics; i++) {
      file << setw(27) << epics[i].chan << setw(20) << epics_val[i] 
	   << epics[i].name << endl;
E 48
I 48
    if(no_file==0) {
      strstream fn;
      fn << clon_root << "/" << epics_summary_name << ends; 
      ofstream file(fn.str());
      file.setf(ios::left);
      file.setf(ios::showpoint);
      time_t now=time(NULL);
      file << "\n#  epics summary created by epics_monitor on " << ctime(&now) << endl ;
      file << "#     ejw, 23-dec-97"  << endl << endl << endl;
      file << "*epics*" << endl;
      file << endl 
	   << setw(27) << "#Channel name" << setw(20) << "Channel value" 
	   << "Channel description" << endl;
      file << setw(27) << "#------------" << setw(20) << "-------------" 
	   << "-------------------" << endl;
      for(int i=0; i<nepics; i++) {
D 65
	file << setw(27) << epics[i].chan << setw(20) << epics_val[i] 
	     << epics[i].name << endl;
E 65
I 65
	if(strlen(epics[i].get)>4) {
	  sprintf(temp,"%s.%s",epics[i].chan,&(epics[i].get)[4]);
	  file << setw(27) << temp << setw(20) << epics_val[i] 
	       << epics[i].name << endl;
	} else {
	  file << setw(27) << epics[i].chan << setw(20) << epics_val[i] 
	       << epics[i].name << endl;
	}
E 81
I 81
  }
  
  
  // ship to info_server
D 112
  if(no_info==0) {
E 112
I 112
  if(no_info==0)
  {
E 112
D 91
    TipcMsg message("info_server");
    message.Sender("epics_monitor");
    message.Dest("info_server/in/epics_monitor");
    message << "epics_monitor";
E 91
I 91
    TipcMsg message((T_STR)"info_server");
    message.Sender((T_STR)"epics_monitor");
    message.Dest((T_STR)"info_server/in/epics_monitor");
    message << (T_STR)"epics_monitor";
E 91
D 112
    for(int i=0; i<nepics; i++) {
E 112
I 112
    for(int i=0; i<nepics; i++)
    {
E 112
      double tval[1] = {(double)epics_val[i]};
D 112
      if(strlen(epics[i].get)>4) {
	sprintf(temp,"%s.%s",epics[i].chan,&(epics[i].get)[4]);
	message << (T_STR) temp << SetSize(1) << tval << Check;
      } else {
	message << (T_STR) epics[i].chan << SetSize(1) << tval << Check;
E 112
I 112
      if(strlen(epics[i].get)>4)
      {
	    sprintf(temp,"%s.%s",epics[i].chan,&(epics[i].get)[4]);
	    message << (T_STR) temp << SetSize(1) << tval << Check;
E 112
E 81
E 65
      }
I 112
      else
      {
	    message << (T_STR) epics[i].chan << SetSize(1) << tval << Check;
      }
E 112
D 81
      file << endl;
      file.close();
E 81
E 48
    }
D 48
    file << endl;
    file.close();
E 48
D 81
    
    
I 61
    // dump to helicity file
    if(no_hel_file==0) {
      ofstream file(hel_file_name);
      if(!file.bad()) {
	int run=get_run_number(msql_database,session);
	time_t now=time(NULL);
	file << run << "," << now << "," << *bpm3 << "," << *bpm1 << endl << endl << endl;
	file << "#  epics helicity data created by epics_monitor on " << ctime(&now) << endl ;
	file << "#     ejw, 14-oct-98"  << endl << endl << endl;
	file.close();
E 81
I 81
    server.Send(message);
    server.Flush();
  }
  
  
  // dump to file
D 112
  if(no_file==0) {
E 112
I 112
  if(no_file==0)
  {
E 112
    strstream fn;
D 88
    fn << clon_root << "/" << epics_channel_name << ends; 
E 88
I 88
    fn << clon_parms<< "/" << epics_channel_name << ends; 
E 88
    ofstream file(fn.str());
    file.setf(ios::left);
    file.setf(ios::showpoint);
    time_t now=time(NULL);
    file << "\n#  epics EPIC summary created by epics_monitor on " << ctime(&now) << endl ;
D 118
    file << "#     ejw, 23-dec-97"  << endl << endl << endl;
E 118
I 118
    file << "#     xxx, 17-Oct-07"  << endl << endl << endl;
E 118
    file << "*epics*" << endl;
    file << endl 
	 << setw(27) << "#Channel name" << setw(20) << "Channel value" 
	 << "Channel description" << endl;
    file << setw(27) << "#------------" << setw(20) << "-------------" 
	 << "-------------------" << endl;
    for(int i=0; i<nepics; i++) {
      if(strlen(epics[i].get)>4) {
	sprintf(temp,"%s.%s",epics[i].chan,&(epics[i].get)[4]);
	file << setw(27) << temp << setw(20) << epics_val[i] 
	     << epics[i].name << endl;
      } else {
	file << setw(27) << epics[i].chan << setw(20) << epics_val[i] 
	     << epics[i].name << endl;
E 81
      }
    }
D 81
    
    
E 61
    // dump to screen
    if(dump!=0) {
      cout.setf(ios::left);
      cout.setf(ios::showpoint);
      cout << endl;
      for(int i=0; i<nepics; i++) {
	cout << setw(45) << epics[i].name << "    " << epics_val[i] << endl;
E 46
      }
D 46
      file << endl;
      file.close();


E 20
D 7
	// dump to screen
	if(dump!=0){
	  cout.setf(ios::left);
	  cout << endl;
	  for(int i=0; i<sizeof(epics_chan)/sizeof(char *); i++){
	    cout << setw(32) << epics_name[i] << "    " << epics_val[i] << endl;
	  }
	  cout << endl;
E 7
I 7
      // dump to screen
      if(dump!=0){
	cout.setf(ios::left);
I 26
	cout.setf(ios::showpoint);
E 26
	cout << endl;
	for(int i=0; i<nepics; i++){
	  cout << setw(32) << epics[i].name << "    " << epics_val[i] << endl;
E 7
	}
D 7

E 7
I 7
	cout << endl;
E 7
      }
I 7
      
E 46
I 46
      cout << endl;
E 81
I 81
    file << endl;
    file.close();
  }
  
  
D 112
  // dump to helicity file
  if(no_hel_file==0) {
    ofstream file(hel_file_name);
D 92
    if(!file.bad()) {
E 92
I 92
    if(file.is_open()) {
E 92
      int run=get_run_number(msql_database,session);
      time_t now=time(NULL);
      file << run << "," << now << "," << *bpm3 << "," << *bpm1 << endl << endl << endl;
      file << "#  epics helicity data created by epics_monitor on " << ctime(&now) << endl ;
      file << "#     ejw, 14-oct-98"  << endl << endl << endl;
      file.close();
E 81
E 46
E 7
    }
D 81
    
I 46

D 49
    // check if done
E 49
I 49
D 66
    // get epics data every wait_time seconds
E 66
I 66
D 67
    // wait wait_time seconds, detach after wait
E 66
E 49
    cdevsys.pend(0.0);
D 49
    if(once_only) {
      done=1;
    } else if(done==0) {  
      // get epics data every wait_time seconds, also handle ipc messages
      cdevsys.pend((double)wait_time);
    }
E 49
I 49
    if(done==0) cdevsys.pend((double)wait_time);
I 66
    detach_epics();
E 66
E 49
E 46
  }
E 67
I 67
    // pend,detach unless done
    if(done==0) {
      cdevsys.pend((double)wait_time);
      detach_epics();
    } else {
      cdevsys.pend(0.0);
E 81
I 81
  }
  
  
E 112
  // dump to screen
D 112
  if(dump!=0) {
E 112
I 112
  if(dump!=0)
  {
E 112
    cout.setf(ios::left);
    cout.setf(ios::showpoint);
    cout << endl;
D 118
    for(int i=0; i<nepics; i++) {
E 118
I 118
    for(int i=0; i<nepics; i++)
    {
E 118
      cout << setw(45) << epics[i].name << "    " << epics_val[i] << endl;
E 81
    }
D 81

  }  // done loop
E 67
I 46

E 81
I 81
    cout << endl;
  }
  
  return((void*)0);
E 81
E 46
}


//--------------------------------------------------------------------------


I 67
D 112
void attach_epics() {

E 112
I 112
D 115
void
attach_epics()
{
#ifdef USECDEV
E 112
  for(int i=0; i<nepics; i++) {
    get[i] = cdevRequestObject::attachPtr(epics[i].chan,epics[i].get);
  }
D 112
  
E 112
I 112
#endif
E 112
  return;
}


//--------------------------------------------------------------------------


E 67
E 5
D 46
void get_epics_data(){
E 46
I 46
D 112
void get_epics_data() {
E 46

E 112
I 112
void
get_epics_data()
{
#ifdef USECDEV
E 112
I 5
  int status;
I 15
D 27
  int nepics_ok;
E 27
E 15
E 5
  cdevGroup group;
I 50
  int repeat=0;
E 50
I 15
D 27
  time_t now=time(NULL);
E 27
E 15


D 15
  // count number of callbacks received
E 15
I 15
  // count number of callbacks sent,received
D 27
  nepics_ok=0;
E 27
E 15
  ncallback=0;


D 15
  // create callback for each channel in one group (for efficiency in CA)
E 15
I 15
D 27
  // create callbacks, check if channels still connected, reconnect if not and enough time elapsed
E 27
I 27
D 49
  // create callbacks
E 49
I 49
  // create request callbacks
E 49
E 27
D 17
  //  ??? logic not correct ???
E 17
E 15
  group.start();
D 7
  for(int i=0; i<sizeof(epics_chan)/sizeof(char *); i++){
E 7
I 7
D 17
  for(int i=0; i<nepics; i++){
E 17
I 17
  for(int i=0; i<nepics; i++) {
E 17
E 7
D 9
    epics_val[i]=-1;
E 9
I 9
D 26
    epics_val[i]=-1.0;
E 26
I 26
    epics_val[i]=-9999.;
E 26
E 9
D 5
    obj[i]->sendCallback(NULL,*cb[i]);
  }
E 5
I 5
D 13
    if(obj[i]!=NULL){
      status=obj[i]->sendCallback(NULL,*cb[i]);
I 9
D 12
      if(status!=CDEV_SUCCESS)obj[i]=NULL;   // ???
E 12
I 12
      // if(status!=CDEV_SUCCESS)obj[i]=NULL;   // ???
E 12
E 9
D 7
      //      if(status!=CDEV_SUCCESS)obj[i]=NULL;   ???
E 7
    }
E 13
I 13
D 15
    if(obj[i]!=NULL) status=obj[i]->sendCallback(NULL,*cb[i]);
E 15
I 15
D 17
    if((obj[i]==NULL)&&(now-last_try[i]>cdev_retry_time)) {
      obj[i] = cdevRequestObject::attachPtr(epics[i].chan,epics[i].get);
E 17
I 17
D 27
    if((get[i]!=NULL)&&(get[i]->getState()==CDEV_STATE_CONNECTED)) {
      nepics_ok++;
      status=get[i]->sendCallback(NULL,*cb[i]);
    } else if (now-last_try[i]>cdev_retry_time) {
      get[i] = cdevRequestObject::attachPtr(epics[i].chan,epics[i].get);
      last_try[i]=time(NULL);
E 17
    }
E 27
I 27
    status=get[i]->sendCallback(NULL,*cb[i]);
E 27
D 17
    if(obj[i]!=NULL) {
      status=obj[i]->getState();
      if(status==CDEV_STATE_CONNECTED) {
	nepics_ok++;
	status=obj[i]->sendCallback(NULL,*cb[i]);
      } else {
	//	cdevRequestObject::detach(obj[i]);
	obj[i]=NULL;
	last_try[i]=now;
      }
    }
E 17
E 15
E 13
D 7
}
E 7
I 7
  }
E 7
E 5
D 17

E 17
I 17
    
I 28

E 28
E 17
D 15

E 15
  // process group of callbacks
D 50
  group.pend((double)cdev_pend_time);
E 50
I 50
  while(repeat++<cdev_pend_repeat) {
    group.pend((double)cdev_pend_time);
    if(ncallback>=nepics)break;
  }
E 50

  
  // check if all callbacks received
D 7
  if(ncallback<sizeof(epics_chan)/sizeof(char *))
E 7
I 7
D 15
  if(ncallback<nepics)
E 15
I 15
D 27
  if(ncallback<nepics_ok)
E 27
I 27
D 28
  if(ncallback<nepics)
E 28
I 28
D 76
  if((debug==1)&&(ncallback<nepics))
E 28
E 27
E 15
E 7
    cerr << "?only received " << ncallback << " callbacks" << " out of " 
D 7
	 << sizeof(epics_chan)/sizeof(char *) << endl;
E 7
I 7
D 15
	 << nepics << endl;
E 15
I 15
D 27
	 << nepics_ok << " attempted" << endl;
E 27
I 27
D 28
	 << nepics << " attempted" << endl;
E 28
I 28
  	 << nepics << " attempted" << endl;
  
E 76
I 76
D 98
  if(debug==1) {
E 98
I 98
  if(debug!=0) {
E 98
    if(ncallback<nepics) {
      cerr << "?only received " << ncallback << " callbacks" << " out of " 
	   << nepics << " attempted" << endl;
    } else {
      cerr << "received all " << ncallback << " callbacks" << endl;
    }  
  }
I 112
#endif
E 112
E 76
E 28
E 27
E 15
E 7

  return;
}


//---------------------------------------------------------------------------

D 112

D 81
void epics_callback_func(int status, void *userarg, cdevRequestObject &epics_req_obj,
E 81
I 81
void channel_callback_func(int status, void *userarg, cdevRequestObject &epics_req_obj,
E 81
D 46
			    cdevData& result){
E 46
I 46
			    cdevData& result) {
E 46

E 112
I 112
#ifdef USECDEV
void
channel_callback_func(int status, void *userarg, cdevRequestObject &epics_req_obj,
			    cdevData& result)
{
E 112
  ncallback++;
D 9
  epics_val[(int)userarg] = (int) result;
E 9
I 9
D 67
  epics_val[(int)userarg] =    (float) result;
E 67
I 67
  epics_val[(int)userarg] = (float) result;
E 67
E 9

  return;
}

I 112
#endif
E 112

//---------------------------------------------------------------------------
I 67

D 112

void detach_epics() {

E 112
I 112
void
detach_epics()
{
#ifdef USECDEV
E 112
  for(int i=0; i<nepics; i++) {
D 68
    if(get[i]->getState()==CDEV_STATE_CONNECTED)cdevRequestObject::detach(get[i]);
E 68
I 68
    if(get[i]->getState()==CDEV_STATE_CONNECTED) {
      cdevRequestObject::detach(get[i]);
    } else {
      time_t now = time(NULL);
D 69
      cerr << "Unattached channel: " << epics[i].chan << " at " << ctime(&now);
E 69
I 69
D 98
      if(debug==1)cerr << "Unattached channel: " << epics[i].chan << " at " << ctime(&now);
E 98
I 98
      if(debug!=0)cerr << "Unattached channel: " << epics[i].chan << " at " << ctime(&now);
E 98
E 69
    }
E 68
D 81
    //cdevRequestObject::detach(get[i]);
E 81
  }
D 112
  
E 112
I 112
#endif  
E 112
  return;
}

D 112

E 112
//--------------------------------------------------------------------------

I 114
#ifdef USECDEV

E 115
E 114
D 112

I 99
D 104
void *bpm_thread(void *param) {

  static int first = 0;
  int i;
  float bpmval[sizeof(bpmname)/sizeof(char*)];
I 103
  int status[sizeof(bpmname)/sizeof(char*)];
E 103
  cdevData result[sizeof(bpmname)/sizeof(char*)];
  cdevGroup group;


  // detach
  if(first==1) {
    for(i=0; i<nbpm; i++)cdevRequestObject::detach(bpmreq[i]);
  } else {
    first=1;
  }


  // read bpm channels
  nepics_read++;
  for(i=0; i<nbpm; i++) 
    bpmreq[i] = cdevRequestObject::attachPtr((char*)bpmname[i],(char*)bpmget[i]);
  group.start();
  for(i=0; i<nbpm; i++) bpmreq[i]->sendNoBlock(NULL,result[i]);
D 103
  group.pend(0.1);
  for(int i=0; i<nbpm; i++) bpmval[i] = (float)result[i];
E 103
I 103
  group.pend(bpm_pend_time);
  group.status(status,nbpm);
  for(int i=0; i<nbpm; i++) {
    if(status[i]==0) {
      bpmval[i] = (float)result[i];
    } else {
      bpmval[i] = -9999.;
    }
  }
E 103


  // ship data
  if(no_ipc==0) {
    nev_to_ipc++;
    for(int i=0; i<nbpm; i++) bpmarray[i][0] = bpmval[i];
    TipcMsg msg((T_STR)"evt_bosbank");
    msg.Sender((T_STR)"epics_monitor");
    msg.Dest(dest);
    msg << (T_STR)"EPIC" << (T_INT4)1 << (T_STR)"(F,8A)" << (T_INT4)9 << (T_INT4)nbpm
	<< (T_INT4)(nbpm*9)
	<< SetSize(nbpm*9) << (T_INT4*)bpmarray;
    server.Send(msg);
    server.Flush();
  }
  
  // dump to file
  if(no_file==0) {
    strstream fn;
    fn << clon_parms<< "/" << epics_bpm_name << ends; 
    ofstream file(fn.str());
    file.setf(ios::left);
    file.setf(ios::showpoint);
    time_t now=time(NULL);
    file << "\n#  epics BPM summary created by epics_monitor on " << ctime(&now) << endl ;
    file << "#     ejw, 14-jul-01"  << endl << endl << endl;
    file << "*epics*" << endl;

    file << endl 
	 << setw(27) << "#Channel name" << setw(20) << "Channel value" << endl;
    file << setw(27) << "#------------" << setw(20) << "-------------" << endl;
    for(int i=0; i<nbpm; i++) {
      if(strlen(bpmget[i])>4) {
	sprintf(temp,"%s.%s",bpmname[i],&(bpmget[i])[4]);
	file << setw(27) << temp << setw(20) << bpmval[i] << endl;
      } else {
	file << setw(27) << bpmname[i] << setw(20) << bpmval[i] << endl;
      }
    }
    file << endl;
    file.close();
  }
  
  
  return((void*)0);
}


//--------------------------------------------------------------------------


E 104
E 99
I 81
void *scaler_thread(void *param) {

E 112
I 112
void *
scaler_thread(void *param)
{
E 112
D 98
  int i,j;
E 98
I 98
D 115
  static int first=0;
  int i,j,num;
  int status[10];
I 112
D 114
#ifdef USECDEV
E 114
E 112
E 98
D 96
  size_t nscaler;
  cdevData result;
E 96
I 96
D 106
  cdevData result1,result2,result3;
E 106
I 106
  cdevData result1,result2,result3,result4;
E 106
E 96
  cdevGroup group;
E 115
I 112
D 114
#endif
E 114
E 112
D 98
  static int first=0;
E 98

D 112

E 112
D 115
  // detach
D 112
  if(first==1) {
E 112
I 112
  if(first==1)
  {
D 114
#ifdef USECDEV
E 114
E 112
D 96
    cdevRequestObject::detach(sreq);
E 96
I 96
    cdevRequestObject::detach(sreq1);
    cdevRequestObject::detach(sreq2);
    cdevRequestObject::detach(sreq3);
I 106
    cdevRequestObject::detach(sreq4);
E 106
E 96
D 112
  } else {
E 112
I 112
D 114
#endif
    ;
E 114
  }
  else
  {
E 112
    first=1;
  }


D 98
  // try to get epics data...do nothing if can't read scaler
E 98
I 98
  // try to get epics data...do nothing if can't read scalers
E 98
  nepics_read++;
I 112
D 114
#ifdef USECDEV
E 114
E 112
D 96
  sreq = cdevRequestObject::attachPtr("e_scalers","get");
E 96
I 96
D 106
  sreq1 = cdevRequestObject::attachPtr((char*)"e_scalers",(char*)"get");
E 106
I 106
D 107
  sreq1 = cdevRequestObject::attachPtr((char*)"jscaler.SCALER_0",(char*)"get");
E 107
I 107
  sreq1 = cdevRequestObject::attachPtr((char*)"jscaler",(char*)"get SCALER_0");
E 107
E 106
  sreq2 = cdevRequestObject::attachPtr((char*)"gp_x",(char*)"get");
  sreq3 = cdevRequestObject::attachPtr((char*)"gp_y",(char*)"get");
I 106
D 107
  sreq4 = cdevRequestObject::attachPtr((char*)"jscaler.SCALER_1",(char*)"get");
E 107
I 107
  sreq4 = cdevRequestObject::attachPtr((char*)"jscaler",(char*)"get SCALER_1");
E 107
E 106
E 96
  group.start();
D 96
  sreq->sendNoBlock(NULL,result);
  group.pend(cdev_pend_time);
E 96
I 96
  sreq1->sendNoBlock(NULL,result1);
  sreq2->sendNoBlock(NULL,result2);
  sreq3->sendNoBlock(NULL,result3);
I 106
  sreq4->sendNoBlock(NULL,result4);
E 106
D 97
  group.pend(2*cdev_pend_time);
E 97
I 97
  group.pend((double)(2*cdev_pend_time));
E 97
E 96
  if(group.allFinished()==0) {
D 96
    if(debug==1)cerr << "?Unable to read scaler" << endl;
E 96
I 96
D 98
    if(debug==1)cerr << "?Unable to read scalers" << endl;
E 96
    return((void*)0);
E 98
I 98
    if(debug!=0)cerr << "?Unable to read scalers" << endl;
E 98
  }
I 112
D 114
#endif
E 114
E 112

D 112

E 112
  // unpack data
I 96
D 98
  unpack_scaler_data("TESC",result1,"e_scalers");
  unpack_scaler_data("GP_X",result2,"gp_x");
  unpack_scaler_data("GP_Y",result3,"gp_y");
E 98
I 98
D 106
  num=3;
E 106
I 106
  num=4;
I 112
D 114
#ifdef USECDEV
E 114
E 112
E 106
  group.status(status,num);
D 106
  if(status[0]==0)unpack_scaler_data("TESC",result1,"e_scalers");
  if(status[1]==0)unpack_scaler_data("GP_X",result2,"gp_x");
  if(status[2]==0)unpack_scaler_data("GP_Y",result3,"gp_y");
E 106
I 106
D 112
  if(status[0]==0)unpack_scaler_data("TESC",0,result1,"jscaler.SCALER_0");
  if(status[1]==0)unpack_scaler_data("GP_X",0,result2,"gp_x");
  if(status[2]==0)unpack_scaler_data("GP_Y",0,result3,"gp_y");
  if(status[3]==0)unpack_scaler_data("TESC",1,result4,"jcsaler.SCALER_1");
E 112
I 112
D 113
  if(status[0]==0) unpack_scaler_data("TESC",0,result1,"jscaler.SCALER_0");
  if(status[1]==0) unpack_scaler_data("GP_X",0,result2,"gp_x");
  if(status[2]==0) unpack_scaler_data("GP_Y",0,result3,"gp_y");
  if(status[3]==0) unpack_scaler_data("TESC",1,result4,"jcsaler.SCALER_1");
E 113
I 113


  if(status[0]==0)
  {
    size_t nscaler;
    result1.getElems((char*)"value",&nscaler);
    nscaler=MIN(MAXSCALER,nscaler);
    if(nscaler>0)
    {    
      float *scalers = new float[nscaler];
      result1.get((char*)"value",scalers);
      unpack_scaler_data("TESC",0,nscaler,scalers,"jscaler.SCALER_0");
      delete[] scalers;
    }
  }

  if(status[1]==0)
  {
    size_t nscaler;
    result2.getElems((char*)"value",&nscaler);
    nscaler=MIN(MAXSCALER,nscaler);
    if(nscaler>0)
    {    
      float *scalers = new float[nscaler];
      result2.get((char*)"value",scalers);
      unpack_scaler_data("GP_X",0,nscaler,scalers,"gp_x");
      delete[] scalers;
    }
  }

  if(status[2]==0)
  {
    size_t nscaler;
    result3.getElems((char*)"value",&nscaler);
    nscaler=MIN(MAXSCALER,nscaler);
    if(nscaler>0)
    {    
      float *scalers = new float[nscaler];
      result3.get((char*)"value",scalers);
      unpack_scaler_data("GP_Y",0,nscaler,scalers,"gp_y");
      delete[] scalers;
    }
  }

  if(status[3]==0)
  {
    size_t nscaler;
    result4.getElems((char*)"value",&nscaler);
    nscaler=MIN(MAXSCALER,nscaler);
    if(nscaler>0)
    {    
      float *scalers = new float[nscaler];
      result4.get((char*)"value",scalers);
      unpack_scaler_data("TESC",1,nscaler,scalers,"jcsaler.SCALER_1");
      delete[] scalers;
    }
  }

E 113
D 114
#endif
E 114
I 114
  return((void*)0);
}
E 114
E 112
E 106
E 98

I 114
#else

void *
scaler_thread(void *param)
{

E 115
  unpack_scaler_data("TESC",0,scalers_nval[0],scalers_val[0],"jscaler.SCALER_0");
  unpack_scaler_data("GP_X",0,scalers_nval[1],scalers_val[1],"gp_x");
  unpack_scaler_data("GP_Y",0,scalers_nval[2],scalers_val[2],"gp_y");
D 120
  unpack_scaler_data("TESC",1,scalers_nval[3],scalers_val[3],"jcsaler.SCALER_1");
E 120
I 120
  unpack_scaler_data("TESC",1,scalers_nval[3],scalers_val[3],"jscaler.SCALER_1");
E 120
D 121

I 119
  /* added on Eugene request Nov 7, 2007 */
E 121
D 120
  unpack_scaler_data("FROST",0,scalers_nval[4],scalers_val[4],"tag_t_left.VAL");
  unpack_scaler_data("FROST",1,scalers_nval[5],scalers_val[5],"tag_t_right.VAL");
  unpack_scaler_data("FROST",2,scalers_nval[6],scalers_val[6],"tag_t_high");
  unpack_scaler_data("FROST",3,scalers_nval[7],scalers_val[7],"jscaler.PERIOD");
E 120
I 120
  unpack_scaler_data("TLSC",0,scalers_nval[4],scalers_val[4],"tag_t_left.VAL");
  unpack_scaler_data("TRSC",0,scalers_nval[5],scalers_val[5],"tag_t_right.VAL");
E 120

E 119
E 114
D 112

E 112
  return((void*)0);
}

I 114
D 115
#endif
E 115
E 114

//--------------------------------------------------------------------------

I 118

void *
gamma_thread(void *param)
{
  ngamma_read ++;

  // ship data
  if(no_ipc==0)
  {
    nev_to_ipc++;
    for(int i=0; i<ngamma; i++) gammaarray[i][0] = gamma_val[i];
    TipcMsg msg((T_STR)"evt_bosbank");
    msg.Sender((T_STR)"gamma_monitor"); /* ??? should we use 'epics_monitor' */
    msg.Dest(dest);
    msg << (T_STR)"EPIC" << (T_INT4)1 << (T_STR)"(F,8A)" << (T_INT4)9 << (T_INT4)ngamma
	<< (T_INT4)(ngamma*9)
	<< SetSize(ngamma*9) << (T_INT4*)gammaarray;
    server.Send(msg);
    server.Flush();
  }
  
  // dump to file
  if(no_file==0)
  {
    strstream fn;
    fn << clon_parms<< "/" << epics_gamma_name << ends; 
    ofstream file(fn.str());
    file.setf(ios::left);
    file.setf(ios::showpoint);
    time_t now=time(NULL);
    file << "\n#  epics GAMMA summary created by epics_monitor on " << ctime(&now) << endl ;
    file << "#     xxx, 17-Oct-07"  << endl << endl << endl;
    file << "*epics*" << endl;

    file << endl 
	 << setw(27) << "#Channel name" << setw(20) << "Channel value" << endl;
    file << setw(27) << "#------------" << setw(20) << "-------------" << endl;
    for(int i=0; i<ngamma; i++) {
      if(strlen(gammas[i].get)>4) {
	sprintf(temp,"%s.%s",gammas[i].name,&(gammas[i].get)[4]);
	file << setw(27) << temp << setw(20) << gamma_val[i] << endl;
      } else {
	file << setw(27) << gammas[i].name << setw(20) << gamma_val[i] << endl;
      }
    }
    file << endl;
    file.close();
  }
  
  // dump to screen
  if(dump!=0)
  {
    cout.setf(ios::left);
    cout.setf(ios::showpoint);
    cout << endl;
    for(int i=0; i<ngamma; i++)
    {
      cout << setw(45) << gammas[i].name << "    " << gamma_val[i] << endl;
    }
    cout << endl;
  }
  
  return((void*)0);
}


//--------------------------------------------------------------------------

E 118
D 112

D 106
void unpack_scaler_data(const char* bank, cdevData result, const char* channel) {
E 106
I 106
void unpack_scaler_data(const char* bank, const int nrecord, cdevData result, const char* channel) {
E 106

E 112
I 112
D 113
#ifdef USECDEV
E 113
void
D 113
unpack_scaler_data(const char* bank, const int nrecord, cdevData result,
                   const char* channel)
E 113
I 113
unpack_scaler_data(const char* bank, const int nrecord,
                   size_t nscaler, float *scalers, const char* channel)
E 113
{
E 112
  int i,j;
D 113
  size_t nscaler;
E 113

D 113

E 113
  // unpack data
E 96
D 113
  result.getElems((char*)"value",&nscaler);
D 111
  nscaler=min(MAXSCALER,nscaler);
E 111
I 111
  nscaler=MIN(MAXSCALER,nscaler);
E 111
  if(nscaler>0) {
    
    float *scalers = new float[nscaler];
I 82
D 96
    result.get("value",scalers);
E 96
I 96
    result.get((char*)"value",scalers);
E 96

E 82

E 113
I 113
  if(nscaler>0)
  {
E 113
    // ship data
D 113
    if(no_ipc==0) {
E 113
I 113
    if(no_ipc==0)
    {
E 113
      nev_to_ipc++;
D 91
      TipcMsg msg("evt_bosbank");
      msg.Sender("epics_monitor");
E 91
I 91
      TipcMsg msg((T_STR)"evt_bosbank");
      msg.Sender((T_STR)"epics_monitor");
E 91
      msg.Dest(dest);
D 91
      msg << "TESC" << (T_INT4)0 << "(F)" << (T_INT4)1 << (T_INT4)nscaler << (T_INT4)nscaler
E 91
I 91
D 96
      msg << (T_STR)"TESC" << (T_INT4)0 << (T_STR)"(F)" << (T_INT4)1 
E 96
I 96
D 106
      msg << (T_STR)bank << (T_INT4)0 << (T_STR)"(F)" << (T_INT4)1 
E 106
I 106
      msg << (T_STR)bank << (T_INT4)nrecord << (T_STR)"(F)" << (T_INT4)1 
E 106
E 96
	  << (T_INT4)nscaler << (T_INT4)nscaler
E 91
	  << SetSize(nscaler) << (T_INT4*)scalers;
      server.Send(msg);
      server.Flush();
    }
    
    
    // dump to file
D 113
    if(no_file==0) {
E 113
I 113
    if(no_file==0)
    {
E 113
D 96
      strstream fn;
D 88
      fn << clon_root << "/" << epics_scaler_name << ends; 
E 88
I 88
      fn << clon_parms<< "/" << epics_scaler_name << ends; 
E 88
      ofstream file(fn.str());
E 96
I 96
D 120
      sprintf(temp,epics_scaler_name,clon_parms,bank);
E 120
I 120
      if(nrecord==0)
        sprintf(temp,"%s/epics/%s_summary.txt",clon_parms,bank);
      else
        sprintf(temp,"%s/epics/%s_%1d_summary.txt",clon_parms,bank,nrecord);

	  /*printf("temp >%s< ( >%s< >%s< >%1d<)\n",temp,clon_parms,bank,nrecord);*/
E 120
      ofstream file(temp);
E 96
      file.setf(ios::left);
      file.setf(ios::showpoint);
      time_t now=time(NULL);
D 96
      file << "\n#  epics TESC summary created by epics_monitor on " << ctime(&now) << endl ;
E 96
I 96
      file << "\n#  epics " << bank << " summary created by epics_monitor on " 
	   << ctime(&now) << endl ;
E 96
D 94
      file << "#     ejw, 10-nov-99"  << endl << endl << endl;
      file << "*epics_scalers*" << endl;
E 94
I 94
      file << "#     ejw, 29-aug-01"  << endl << endl << endl;
D 96
      file << "*epics_scalers* -- channel = \"e_scalers\",  nscaler = " << nscaler << endl;
E 96
I 96
      file << "*epics_scalers* -- channel = \"" << channel << "\",  nscaler = " << nscaler << endl;
E 96
E 94
D 113
      for(i=0; i<nscaler; i+=8) {
	for(j=i; j<i+8; j++) {
D 96
	  file << scalers[j] << "  ";
E 96
I 96
	  file << setw(11) << scalers[j] << " ";
E 96
	}
	file<< endl;
E 113
I 113
      for(i=0; i<nscaler; i+=8)
      {
	    for(j=i; j<i+8; j++)
        {
	      file << setw(11) << scalers[j] << " ";
	    }
	    file<< endl;
E 113
      }
      file << endl;
      file.close();
    }
    
    
    // dump to screen
D 113
    if(dump!=0) {
E 113
I 113
    if(dump!=0)
    {
E 113
      cout.setf(ios::left);
      cout.setf(ios::showpoint);
      cout << endl;
D 113
      for(i=0; i<nscaler; i+=8) {
	for(j=i; j<i+8; j++) {
	  cout << scalers[j] << "  ";
	}
	cout << endl;
E 113
I 113
      for(i=0; i<nscaler; i+=8)
      {
	    for(j=i; j<i+8; j++)
        {
	      cout << scalers[j] << "  ";
	    }
	    cout << endl;
E 113
      }
      cout << endl;
D 113
    }
    

    // free memory
    delete[] scalers;
    
E 113
I 113
    }    
E 113
  }

D 96
  return((void*)0);
E 96
I 96
  return;
E 96
}
I 112
D 113
#endif
E 113
E 112

I 113

E 113
I 112
//--------------------------------------------------------------------------
E 112

I 112
D 115
/*???*/
#ifdef USECDEV
void
init_ipc_callbacks()
{
  ipcfd=server.XtSource();
  cdevsys.addUserFdCallback(ipcfd,ipc_mainloop,(void *)0,ipcid);
  return;
}
#endif
E 115

E 112
D 115
//--------------------------------------------------------------------------


E 81
D 112
void init_ipc_callbacks() {
E 112
I 112
#ifndef USECDEV
E 112

E 115
D 112
  ipcfd=server.XtSource();
  cdevsys.addUserFdCallback(ipcfd,ipc_mainloop,(void *)0,ipcid);
E 112
I 112
/*******************************************/
/*******************************************/
D 119
/*********** from tool_lib.c ***************/
E 119
I 119
/****************** from $EPICS_BASE/src/catools/camonitor.c *************************/
E 119
E 112

D 112
  return;
E 112
I 112
D 119
void
sprint_long (char *ret, long val)
{
    long i, bit, skip=-1L;   /* used only for printing bits */
    switch (outType) {
    case hex_: sprintf(ret, "0x%lX", val); break;
    case oct_: sprintf(ret, "0o%lo", val); break;
    case bin_:
        for (i=31; i>=0 ; i--)
        {
            bit = (val>>i) & 0x1L;
            if (skip<0 && bit)
            {
                skip = 31 - i;          /* skip leading 0's */
                ret[i+1] = '\0';
            }
            if (skip >= 0)
            {
                ret[31-i-skip] = (bit) ? '1' : '0';
            }
        }
        break;
    default:  sprintf(ret, "%ld", val); /* decimal */
    }
E 112
}
E 119

D 119

E 119
D 112
//--------------------------------------------------------------------------
E 112
I 112
/*+**************************************************************************
 *
D 119
 * Function:	val2str
 *
 * Description:	Print (convert) value to a string
 *
 * Arg(s) In:	v      -  Pointer to dbr_... structure
 *              type   -  Numeric dbr type
 *              index  -  Index of element to print (for arrays) 
 *
 * Return(s):	Pointer to static output string
 *
 **************************************************************************-*/
E 112

I 112
char *
val2str (const void *v, unsigned type, int index, float *valfloat)
{
    static char str[500];
    char ch;
    void *val_ptr;
    unsigned base_type;
E 112

D 112
void init_epics() {
E 112
I 112
    if (!dbr_type_is_valid(type)) {
        strcpy (str, "*** invalid type");
        return str;
    }
E 112

D 87
  char *blanks              = "                                      ";
E 87
I 87
D 91
  char *blanks = "                                      ";
E 91
I 91
D 93
  char *blanks = (char*)"                                      ";
E 93
I 93
D 112
  char *blanks = (char*)malloc(64);
E 112
I 112
    base_type = type % (LAST_TYPE+1);

    if (type == DBR_STSACK_STRING || type == DBR_CLASS_NAME)
        base_type = DBR_STRING;

    val_ptr = dbr_value_ptr(v, type);

	/*printf("index=%d base_type=%d\n",index,base_type);*/
    switch (base_type) {
    case DBR_STRING:
        sprintf(str, "%s",  ((dbr_string_t*) val_ptr)[index]);
        *valfloat = 0.0; /* must use 'str' */
        break;
    case DBR_FLOAT:
        sprintf(str, dblFormatStr, ((dbr_float_t*) val_ptr)[index]);
        *valfloat = ((dbr_float_t*) val_ptr)[index];
        break;
    case DBR_DOUBLE:
        sprintf(str, dblFormatStr, ((dbr_double_t*) val_ptr)[index]);
        *valfloat = ((dbr_double_t*) val_ptr)[index];
        break;
    case DBR_CHAR:
        ch = ((dbr_char_t*) val_ptr)[index];
        *valfloat = 0.0; /* must use 'str' */
        if(ch > 31 )
            sprintf(str, "%d \'%c\'",ch,ch);
        else
            sprintf(str, "%d",ch);
        break;
    case DBR_INT:
        sprint_long(str, ((dbr_int_t*) val_ptr)[index]);
        *valfloat = ((dbr_int_t*) val_ptr)[index];
        break;
    case DBR_LONG:
        sprint_long(str, ((dbr_long_t*) val_ptr)[index]);
        break;
        *valfloat = ((dbr_long_t*) val_ptr)[index];
    case DBR_ENUM:
    {
        dbr_enum_t *val = (dbr_enum_t *)val_ptr;
        *valfloat = 0.0; /* must use 'str' */
        if (dbr_type_is_GR(type))
            sprintf(str,"%s (%d)", 
                    ((struct dbr_gr_enum *)v)->strs[val[index]],val[index]);
        else if (dbr_type_is_CTRL(type))
            sprintf(str,"%s (%d)", 
                    ((struct dbr_ctrl_enum *)v)->strs[val[index]],val[index]);
        else
            sprintf(str, "%d", val[index]);
    }
    }
    return str;
}


/*+**************************************************************************
 *
E 119
 * Function:	event_handler
 *
 * Description:	CA event_handler for request type callback
 * 		Allocates the dbr structure and copies the data
 *
 * Arg(s) In:	args  -  event handler args (see CA manual)
 *
 **************************************************************************-*/

/* 'pv' defined in tool_lib.h
   'evargs' defined in cadef.h */
/* called if channel is changed */
void
event_handler (evargs args)
{
  pv *pvv;
  char *valstr;
D 114
  float valfloat;
E 114
I 114
  float valfloatarray[MAX_CHANNELS_LENGTH];
D 118
  int index1, nelems = 0;
E 118
I 118
  int index1, index2, nelems = 0;
E 118
E 114

I 121
  int *valintarray;
  valintarray = (int *)valfloatarray;

E 121
  /* args.usr contains pointer to the 'ppv' from ca_create_subscription() call
D 121
  (see connaction_handler) */
E 121
I 121
  (see connection_handler) */
E 121
  pvv = (pv *)args.usr;

  pvv->status = args.status;
  if (args.status == ECA_NORMAL)
  {
    pvv->dbrType = args.type;
D 121
    memcpy(pvv->value, args.dbr, dbr_size_n(args.type, args.count));
E 121
D 114
    valstr = print_time_val_sts(pvv, pvv->reqElems, &valfloat);
E 114
I 114

D 121
    /*valstr = print_time_val_sts(pvv, pvv->reqElems, valfloatarray);*/
E 121
I 121

/*sergey
if(pvv->index==206)
{
  printf("index=%d nelems=%d %d, count=%d\n",pvv->index,pvv->reqElems,pvv->nElems,args.count);
}
*/
	/*
printf("[%3d] ------------------------------> count=%d (from %d %d)\n",
pvv->index,dbr_size_n(args.type, args.count),args.type, args.count);
	*/

    memcpy(pvv->value, args.dbr, dbr_size_n(args.type, args.count)); /*normally 24*/

E 121
    nelems = MIN(MAX_CHANNELS_LENGTH,pvv->nElems);
I 121
    if(nelems != pvv->nElems)
    {
      printf("event_handler: ERROR: pvv->nElems=%d != nelems=%d\n",pvv->nElems,nelems);
	}
E 121
    valstr = print_time_val_sts(pvv, nelems, valfloatarray);
E 114
  }
I 118
  else
  {
    printf("event_handler: ERROR: args.status != ECA_NORMAL\n");
    return;
  }
E 118
  /*
D 121
  printf("===>%s< %f\n",valstr,valfloat);
E 121
I 121
  printf("===>%s< %f\n",valstr,valfloatarray[0],valfloatarray[1],valfloatarray[2]);
E 121
  */
D 118
  ncallback++;
E 118
D 114
  strncpy((char *)epics_valstr[pvv->index],valstr,VALSTRLEN-1);
  epics_val[pvv->index] = valfloat;
E 114

I 114
  index1 = pvv->index - nepics;
I 118
  index2 = pvv->index - nepics - nscalers;
E 118
  if(pvv->index < nepics)
  {
    strncpy((char *)epics_valstr[pvv->index],valstr,VALSTRLEN-1); /* not in use yet .. */
    epics_val[pvv->index] = valfloatarray[0];
  }
  else if(index1 < nscalers)
  {
D 118
    printf("scaler pvv: index=%d len=%d\n",index1,nelems);
E 118
I 118
D 121
    /*printf("scaler pvv: index=%d len=%d\n",index1,nelems);*/
E 121
I 121
	/*
if(index1==0) printf("scaler pvv: index=%d len=%d vals=%f %f %f %f %f %f\n",index1,nelems,
valfloatarray[0],valfloatarray[1],valfloatarray[2],
valfloatarray[3],valfloatarray[4],valfloatarray[5]);
	*/
	
E 121
E 118
    scalers_nval[index1] = nelems;
    for(int i=0; i<nelems; i++)
    {
      scalers_val[index1][i] = valfloatarray[i];
	}
  }
I 118
  else if(index2 < ngamma)
  {
    /*printf("gamma pvv: index=%d\n",index2);*/
    strncpy((char *)gamma_valstr[index2],valstr,VALSTRLEN-1); /* not in use yet .. */
    gamma_val[index2] = valfloatarray[0];
  }
E 118
  else
  {
    printf("event_handler: ERROR: unknown index=%d\n",pvv->index);
  }
E 114

}

/*+**************************************************************************
 *
 * Function:	connection_handler
 *
 * Description:	CA connection_handler 
 *
 * Arg(s) In:	args  -  connection_handler_args (see CA manual)
 *
 **************************************************************************-*/
void
connection_handler(struct connection_handler_args args)
{
  pv *ppv = (pv *) ca_puser(args.chid);
  char *valstr;
D 114
  float valfloat;
E 114
I 114
  float valfloatarray[MAX_CHANNELS_LENGTH];
I 121
  unsigned long reqElems = REQELEMS;
E 121
E 114

I 121
/* sergey: enforce all ENUM types to be reported as integers rather then strings */
enumAsNr = 1;

E 121
  if(args.op == CA_OP_CONN_UP)
  {
    int dbrType;

    /* Set up pv structure */
    /* ------------------- */

    /* Get natural type and array count */
    ppv->nElems  = ca_element_count(ppv->ch_id);
    ppv->dbfType = ca_field_type(ppv->ch_id);

    /* Set up value structures */
    dbrType = dbf_type_to_DBR_TIME(ppv->dbfType); /* Use native type */
    if(dbr_type_is_ENUM(dbrType))                 /* Enums honour -n option */
    {
      if(enumAsNr) dbrType = DBR_TIME_INT;
      else         dbrType = DBR_TIME_STRING;
    }
D 121
        
E 121
I 121

E 121
    else if(floatAsString &&
                 (dbr_type_is_FLOAT(dbrType) || dbr_type_is_DOUBLE(dbrType)))
    {
      dbrType = DBR_TIME_STRING;
    }

I 121
/*sergey
if(!strcmp(ppv->name,"gp_x")) printf("connection_handler: >%s< set reqElems to %d (nElems=%d)\n",
ppv->name,reqElems,ppv->nElems);
*/

E 121
    /* Adjust array count */
    if (reqElems == 0 || ppv->nElems < reqElems)
I 114
	{
E 114
      reqElems = ppv->nElems;
I 114
	}
E 114

    /* Remember dbrType and reqElems */
    ppv->dbrType  = dbrType;
    ppv->reqElems = reqElems;

    ppv->onceConnected = 1;
    nConn++;

    /* Issue CA request */
    /* ---------------- */
/* 'event_handler' WILL BE CALLED IF CHANNEL VALUE IS CHANGED */
    /* install monitor once with first connect */
    if ( ! ppv->value )
    {
      /* Allocate value structure */
      ppv->value = calloc(1, dbr_size_n(dbrType, reqElems));           
      if ( ppv->value )
      {
        ppv->status = ca_create_subscription(dbrType,
                                             reqElems,
                                             ppv->ch_id,
                                             eventMask,
                                             event_handler,
                                             (void*)ppv,
                                             NULL);
        if ( ppv->status != ECA_NORMAL )
        {
          free ( ppv->value );
        }
      }

    }
  }
  else if ( args.op == CA_OP_CONN_DOWN )
  {
    nConn--;
    ppv->status = ECA_DISCONN;
I 121
    printf("DISCONNECTED\n");
E 121
D 114
    valstr = print_time_val_sts(ppv, ppv->reqElems, &valfloat);
E 114
I 114
    valstr = print_time_val_sts(ppv, ppv->reqElems, valfloatarray);
E 114
  }
}

D 119
/*+**************************************************************************
 *
 * Function:	print_time_val_sts
 *
 * Description:	Print (to stdout) one wide output line
 *              (name, timestamp, value, status, severity)
 *
 * Arg(s) In:	pv      -  Pointer to pv structure
 *              nElems  -  Number of elements (array)
 *
 **************************************************************************-*/
 
/* all prints are commented out: uncomment if want to debug */
#define PRN_TIME_VAL_STS(TYPE,TYPE_ENUM)                                        \
    printAbs = 0;                                                               \
                                                                                \
    switch (tsType) {                                                           \
    case relative:                                                              \
        ptsRef = &tsStart;                                                      \
        break;                                                                  \
    case incremental:                                                           \
        ptsRef = &tsPrevious;                                                   \
        break;                                                                  \
    case incrementalByChan:                                                     \
        ptsRef = &pv->tsPrevious;                                               \
        break;                                                                  \
    default :                                                                   \
        printAbs = 1;                                                           \
    }                                                                           \
                                                                                \
    if (!printAbs) {                                                            \
        if (pv->firstStampPrinted)                                              \
        {                                                                       \
D 114
            /*printf("%10.4fs ", epicsTimeDiffInSeconds(                          \
			  &(((struct TYPE *)value)->stamp), ptsRef) )*/;             \
E 114
I 114
            /*printf("%10.4fs ", epicsTimeDiffInSeconds(                        \
			  &(((struct TYPE *)value)->stamp), ptsRef) )*/;                    \
E 114
        } else {                    /* First stamp is always absolute */        \
            printAbs = 1;                                                       \
            pv->firstStampPrinted = 1;                                          \
        }                                                                       \
    }                                                                           \
                                                                                \
    if (tsType == incrementalByChan)                                            \
        pv->tsPrevious = ((struct TYPE *)value)->stamp;                         \
                                                                                \
    if (printAbs) {                                                             \
        epicsTimeToStrftime(timeText, TIMETEXTLEN, timeFormatStr,               \
                            &(((struct TYPE *)value)->stamp));                  \
D 114
        /*printf("%s ", timeText)*/;                                                \
E 114
I 114
        /*printf("%s ", timeText)*/;                                            \
E 114
    }                                                                           \
                                                                                \
    tsPrevious = ((struct TYPE *)value)->stamp;                                 \
                                                                                \
                             /* Print count if array */                         \
D 114
    if ( nElems > 1 ) /*printf("%lu ", nElems)*/;                                   \
E 114
I 114
    if ( nElems > 1 ) /*printf("%lu ", nElems)*/;                               \
E 114
                             /* Print Values */                                 \
    for (i=0; i<nElems; ++i) {                                                  \
D 114
        valstr = val2str(value, TYPE_ENUM, i, valfloat);                        \
E 114
I 114
        valstr = val2str(value, TYPE_ENUM, i, &valfloatarray[i]);               \
E 114
        /*printf ("%s ", valstr)*/;                                             \
    }                                                                           \
                             /* Print Status, Severity - if not NO_ALARM */     \
    if ( ((struct TYPE *)value)->status || ((struct TYPE *)value)->severity )   \
    {                                                                           \
D 114
        /*printf("%s %s\n",                                                       \
E 114
I 114
        /*printf("%s %s\n",                                                     \
E 114
               stat_to_str(((struct TYPE *)value)->status),                     \
D 114
               sevr_to_str(((struct TYPE *)value)->severity))*/;                  \
E 114
I 114
               sevr_to_str(((struct TYPE *)value)->severity))*/;                \
E 114
    } else {                                                                    \
D 114
        /*printf("\n")*/;                                                           \
E 114
I 114
        /*printf("\n")*/;                                                       \
E 114
    }
E 119

D 119

char *
D 114
print_time_val_sts (pv* pv, unsigned long nElems, float *valfloat)
E 114
I 114
print_time_val_sts (pv* pv, unsigned long nElems, float *valfloatarray)
E 114
{
D 114
    char timeText[TIMETEXTLEN];
    int i, printAbs;
    void* value = pv->value;
    epicsTimeStamp *ptsRef = &tsStart;
    epicsTimeStamp tsNow;
    char *valstr;
E 114
I 114
  char timeText[TIMETEXTLEN];
  int i, printAbs;
  void* value = pv->value;
  epicsTimeStamp *ptsRef = &tsStart;
  epicsTimeStamp tsNow;
  char *valstr;
E 114

D 114
    if (!tsInit)                /* Initialize start timestamp */
    {
        epicsTimeGetCurrent(&tsStart);
        tsPrevious = tsStart;
        tsInit = 1;
    }
E 114
I 114
  if (!tsInit)                /* Initialize start timestamp */
  {
    epicsTimeGetCurrent(&tsStart);
    tsPrevious = tsStart;
    tsInit = 1;
  }
E 114

D 114
    epicsTimeGetCurrent(&tsNow);
    epicsTimeToStrftime(timeText, TIMETEXTLEN, timeFormatStr, &tsNow);
E 114
I 114
  epicsTimeGetCurrent(&tsNow);
  epicsTimeToStrftime(timeText, TIMETEXTLEN, timeFormatStr, &tsNow);
E 114

D 114
    if (!pv->onceConnected)
E 114
I 114
  if (!pv->onceConnected)
  {
    valfloatarray[0] = -9999.0;
    printf("%-30s ", pv->name);
    printf("*** Not connected (PV not found)\n");
  }
  else if (pv->status == ECA_DISCONN)
  {
    valfloatarray[0] = -9999.0;
    printf("%-30s ", pv->name);
    printf("%s *** disconnected\n", timeText);
  }
  else if (pv->status == ECA_NORDACCESS)
  {
    valfloatarray[0] = -9999.0;
    printf("%-30s ", pv->name);
    printf("%s *** no read access\n", timeText);
  }
  else if (pv->status != ECA_NORMAL)
  {
    valfloatarray[0] = -9999.0;
    printf("%-30s ", pv->name);
    printf("%s *** CA error %s\n", timeText, ca_message(pv->status));
  }
  else if (pv->value == 0)
  {
    valfloatarray[0] = -9999.0;
    printf("%-30s ", pv->name);
    printf("%s *** no data available (timeout)\n", timeText);
  }
  else
  {
    /*printf("%-30s ", pv->name);*/
    switch (pv->dbrType)
E 114
    {
D 114
      *valfloat = -9999.0;
      printf("%-30s ", pv->name);
      printf("*** Not connected (PV not found)\n");
E 114
I 114
      case DBR_TIME_STRING:
          PRN_TIME_VAL_STS(dbr_time_string, DBR_TIME_STRING);
          break;
      case DBR_TIME_SHORT:
          PRN_TIME_VAL_STS(dbr_time_short, DBR_TIME_SHORT);
          break;
      case DBR_TIME_FLOAT:
          PRN_TIME_VAL_STS(dbr_time_float, DBR_TIME_FLOAT);
          break;
      case DBR_TIME_ENUM:
          PRN_TIME_VAL_STS(dbr_time_enum, DBR_TIME_ENUM);
          break;
      case DBR_TIME_CHAR:
          PRN_TIME_VAL_STS(dbr_time_char, DBR_TIME_CHAR);
          break;
      case DBR_TIME_LONG:
          PRN_TIME_VAL_STS(dbr_time_long, DBR_TIME_LONG);
          break;
      case DBR_TIME_DOUBLE:
          PRN_TIME_VAL_STS(dbr_time_double, DBR_TIME_DOUBLE);
          break;
      default: printf("can't print data type\n");
E 114
    }
D 114
    else if (pv->status == ECA_DISCONN)
    {
      *valfloat = -9999.0;
      printf("%-30s ", pv->name);
      printf("%s *** disconnected\n", timeText);
    }
    else if (pv->status == ECA_NORDACCESS)
    {
      *valfloat = -9999.0;
      printf("%-30s ", pv->name);
      printf("%s *** no read access\n", timeText);
    }
    else if (pv->status != ECA_NORMAL)
    {
      *valfloat = -9999.0;
      printf("%-30s ", pv->name);
      printf("%s *** CA error %s\n", timeText, ca_message(pv->status));
    }
    else if (pv->value == 0)
    {
      *valfloat = -9999.0;
      printf("%-30s ", pv->name);
      printf("%s *** no data available (timeout)\n", timeText);
    }
    else
    {
      /*printf("%-30s ", pv->name);*/
      switch (pv->dbrType)
      {
        case DBR_TIME_STRING:
            PRN_TIME_VAL_STS(dbr_time_string, DBR_TIME_STRING);
            break;
        case DBR_TIME_SHORT:
            PRN_TIME_VAL_STS(dbr_time_short, DBR_TIME_SHORT);
            break;
        case DBR_TIME_FLOAT:
            PRN_TIME_VAL_STS(dbr_time_float, DBR_TIME_FLOAT);
            break;
        case DBR_TIME_ENUM:
            PRN_TIME_VAL_STS(dbr_time_enum, DBR_TIME_ENUM);
            break;
        case DBR_TIME_CHAR:
            PRN_TIME_VAL_STS(dbr_time_char, DBR_TIME_CHAR);
            break;
        case DBR_TIME_LONG:
            PRN_TIME_VAL_STS(dbr_time_long, DBR_TIME_LONG);
            break;
        case DBR_TIME_DOUBLE:
            PRN_TIME_VAL_STS(dbr_time_double, DBR_TIME_DOUBLE);
            break;
        default: printf("can't print data type\n");
      }
    }
E 114
I 114
  }
E 114

D 114
	return(valstr);
E 114
I 114
  return(valstr);
E 114
}



/*+**************************************************************************
 *
 * Function:	create_pvs
 *
 * Description:	Creates an arbitrary number of PVs
 *
 * Arg(s) In:	pvs   -  Pointer to an array of pv structures
 *              nPvs  -  Number of elements in the pvs array
 *              pCB   -  Connection state change callback
 *
 * Arg(s) Out:	none
 *
 * Return(s):	Error code:
 *                  0  -  All PVs created
 *                  1  -  Some PV(s) not created
 *
 **************************************************************************-*/

int
create_pvs (pv* pvs, int nPvs, caCh *pCB)
{
  int n;
  int result;
  int returncode = 0;
                                 /* Issue channel connections */
  for(n=0; n<nPvs; n++)
  {
    result = ca_create_channel (pvs[n].name,
                                pCB,
                                &pvs[n],
                                CA_PRIORITY,
                                &pvs[n].ch_id);
    if (result != ECA_NORMAL)
    {
      fprintf(stderr, "CA error %s occurred while trying "
              "to create channel '%s'.\n", ca_message(result), pvs[n].name);
      pvs[n].status = result;
      returncode = 1;
    }
  }

  return(returncode);
}
D 115
#endif
E 115

/*********** from tool_lib.c ***************/
E 119
/*******************************************/
/*******************************************/


/* main() calls it to setup EPICS callbacks */
int
init_epics()
{
  int i;
  char *blanks = (char*) malloc(64);
E 112
E 93
E 91
  char *p1,*p2;
E 87

I 87

I 114
  /****************************************/
  /* read and process channel config file */

E 114
I 93
  // fill blanks
D 112
  for(int i=0;i<31; i++)blanks[i]=' ';
E 112
I 112
  for(i=0; i<31; i++) blanks[i]=' ';
E 112
  blanks[31]='\0';

D 114

E 93
  // open config file
E 114
I 114
  // open channel config file
E 114
  strstream fn;
D 88
  fn << clon_root << "/" << epics_config_name << ends; 
E 88
I 88
  fn << clon_parms<< "/" << epics_config_name << ends; 
E 88
  ifstream file(fn.str());
D 92
  if(file.bad()) {
E 92
I 92
D 112
  if(!file.is_open()) {
E 112
I 112
  if(!file.is_open())
  {
E 112
E 92
    cerr << "?unable to open " << fn << endl;
    exit(EXIT_FAILURE);
  }
D 114
    
E 114
  
  // find beginning of channel list
D 112
  if(find_tag_line(file,"*CHANNELS*",buffer,sizeof(buffer))!=0) {
E 112
I 112
  if(find_tag_line(file,"*CHANNELS*",buffer,sizeof(buffer))!=0)
  {
E 112
D 93
    cerr << "Can't find *CHANNELS* tag in " << fn << endl;
E 93
I 93
    cerr << "Can't find *CHANNELS* tag in " << fn.str() << endl;
E 93
    exit(EXIT_FAILURE);
  }

D 114

E 114
  // read in all channels
  nepics=0;
D 112
  while (get_next_line(file,buffer,sizeof(buffer))==0) {
E 112
I 112
  while (get_next_line(file,buffer,sizeof(buffer))==0)
  {
E 112
    nepics++;

    p1=strchr(buffer,'"')+1;  p2=strchr(p1,'"');  
    strncpy(temp,p1,p2-p1);   temp[p2-p1]=NULL;
D 112
    epics[nepics-1].name=strdup(temp);
E 112
I 112
    epics[nepics-1].name = strdup(temp);
E 112

    p1=strchr(p2+1,'"')+1;    p2=strchr(p1,'"');  
    strncpy(temp,p1,p2-p1);   temp[p2-p1]=NULL;
D 112
    epics[nepics-1].chan=strdup(temp);
E 112
I 112
    epics[nepics-1].chan = strdup(temp);
E 112

    p1=strchr(p2+1,'"')+1;    p2=strchr(p1,'"');  
    strncpy(temp,p1,p2-p1);   temp[p2-p1]=NULL;
D 112
    epics[nepics-1].get=strdup(temp);
E 112
I 112
    epics[nepics-1].get = strdup(temp);
E 112

D 112
    if(nepics>=MAX_EPICS) {
E 112
I 112
    if(nepics>=MAX_EPICS)
    {
E 112
      cerr << "Too many epics channels in " << fn << ", excess ignored" << endl;
      break;
    }
  }
D 114
	   
E 114
E 87

  // copy epics channel names (31 char max) into special bos array...32nd char is NULL
  // concatenate get string
D 72
  if(no_dd==0) {
E 72
I 72
D 112
  if(no_ipc==0) {
E 112
I 112
  if(no_ipc==0)
  {
E 112
E 72
    int i;
D 93
    blanks[31]='\0';
E 93
    for(i=0; i<nepics; i++) strncpy((char *)(&bosarray[i][1]),blanks,32);
D 112
    for(i=0; i<nepics; i++) {
      if(strlen(epics[i].get)>4) {
	sprintf(temp,"%s.%s",epics[i].chan,&(epics[i].get)[4]);
D 111
	strncpy((char *)(&bosarray[i][1]),temp,min(strlen(temp),31));
E 111
I 111
	strncpy((char *)(&bosarray[i][1]),temp,MIN(strlen(temp),31));
E 111
      } else {
D 111
	strncpy((char *)(&bosarray[i][1]),epics[i].chan,min(strlen(epics[i].chan),31));
E 111
I 111
	strncpy((char *)(&bosarray[i][1]),epics[i].chan,MIN(strlen(epics[i].chan),31));
E 112
I 112
    for(i=0; i<nepics; i++)
    {
      if(strlen(epics[i].get)>4)
      {
	    sprintf(temp,"%s.%s",epics[i].chan,&(epics[i].get)[4]);
	    strncpy((char *)(&bosarray[i][1]),temp,MIN(strlen(temp),31));
E 112
E 111
I 99
      }
I 112
      else
      {
	    strncpy((char *)(&bosarray[i][1]),epics[i].chan,MIN(strlen(epics[i].chan),31));
      }
E 112
    }
  }

D 114

E 114
I 112
  /*print*/
I 114
  printf("\nepics %3d channels =====================\n",nepics);
E 114
  for(i=0; i<nepics; i++)
  {
    printf("[%3d] name=>%s<\t\tchan=>%s<\t\tget=>%s<\n",i,
      epics[i].name,epics[i].chan,epics[i].get);
  }
I 114
  printf("======================================\n");
E 114



I 114




  /***************************************/
  /* read and process scaler config file */

  // open channel config file
  strstream fns;
  fns << clon_parms<< "/" << epics_scaler_config_name << ends; 
  ifstream files(fns.str());
  if(!files.is_open())
  {
    cerr << "?unable to open " << fns << endl;
    exit(EXIT_FAILURE);
  }
  
  // find beginning of channel list
  if(find_tag_line(files,"*CHANNELS*",buffer,sizeof(buffer))!=0)
  {
    cerr << "Can't find *CHANNELS* tag in " << fns.str() << endl;
    exit(EXIT_FAILURE);
  }

  // read in all channels
  nscalers=0;
  while (get_next_line(files,buffer,sizeof(buffer))==0)
  {
    nscalers++;

    p1=strchr(buffer,'"')+1;  p2=strchr(p1,'"');  
    strncpy(temp,p1,p2-p1);   temp[p2-p1]=NULL;
    scalers[nscalers-1].name = strdup(temp);

    p1=strchr(p2+1,'"')+1;    p2=strchr(p1,'"');  
    strncpy(temp,p1,p2-p1);   temp[p2-p1]=NULL;
    scalers[nscalers-1].chan = strdup(temp);

    p1=strchr(p2+1,'"')+1;    p2=strchr(p1,'"');  
    strncpy(temp,p1,p2-p1);   temp[p2-p1]=NULL;
    scalers[nscalers-1].get = strdup(temp);

    if(nscalers>=MAX_EPICS)
    {
      cerr << "Too many scaler channels in " << fns << ", excess ignored" << endl;
      break;
    }
  }

  /*print*/
  printf("\nscaler %3d channels =====================\n",nscalers);
  for(i=0; i<nscalers; i++)
  {
    printf("[%3d] name=>%s<\t\tchan=>%s<\t\tget=>%s<\n",i,
      scalers[i].name,scalers[i].chan,scalers[i].get);
  }
  printf("=======================================\n");

I 118


  /**************************************/
  /* read and process gamma config file */

  // fill blanks
  for(i=0; i<31; i++) blanks[i]=' ';
  blanks[31]='\0';

  // open channel config file
  strstream fng;
  fng << clon_parms<< "/" << epics_gamma_config_name << ends; 
  ifstream fileg(fng.str());
  if(!fileg.is_open())
  {
    cerr << "?unable to open " << fng << endl;
    exit(EXIT_FAILURE);
  }
  
  // find beginning of channel list
  if(find_tag_line(fileg,"*CHANNELS*",buffer,sizeof(buffer))!=0)
  {
    cerr << "Can't find *CHANNELS* tag in " << fng.str() << endl;
    exit(EXIT_FAILURE);
  }

  // read in all channels
  ngamma=0;
  while (get_next_line(fileg,buffer,sizeof(buffer))==0)
  {
    ngamma++;

    p1=strchr(buffer,'"')+1;  p2=strchr(p1,'"');  
    strncpy(temp,p1,p2-p1);   temp[p2-p1]=NULL;
    gammas[ngamma-1].name = strdup(temp);

    p1=strchr(p2+1,'"')+1;    p2=strchr(p1,'"');  
    strncpy(temp,p1,p2-p1);   temp[p2-p1]=NULL;
    gammas[ngamma-1].chan = strdup(temp);

    p1=strchr(p2+1,'"')+1;    p2=strchr(p1,'"');  
    strncpy(temp,p1,p2-p1);   temp[p2-p1]=NULL;
    gammas[ngamma-1].get = strdup(temp);

    if(ngamma>=MAX_EPICS)
    {
      cerr << "Too many gamma channels in " << fng << ", excess ignored" << endl;
      break;
    }
  }

  // copy gamma channel names (31 char max) into special bos array...32nd char is NULL
  // concatenate get string
  if(no_ipc==0)
  {
    int i;
    for(i=0; i<ngamma; i++) strncpy((char *)(&gammaarray[i][1]),blanks,32);
    for(i=0; i<ngamma; i++)
    {
      if(strlen(gammas[i].get)>4)
      {
	    sprintf(temp,"%s.%s",gammas[i].chan,&(gammas[i].get)[4]);
	    strncpy((char *)(&gammaarray[i][1]),temp,MIN(strlen(temp),31));
      }
      else
      {
	    strncpy((char *)(&gammaarray[i][1]),gammas[i].chan,MIN(strlen(gammas[i].chan),31));
      }
    }
  }

  /*print*/
  printf("\ngamma %3d channels =====================\n",ngamma);
  for(i=0; i<ngamma; i++)
  {
    printf("[%3d] name=>%s<\t\tchan=>%s<\t\tget=>%s<\n",i,
      gammas[i].name,gammas[i].chan,gammas[i].get);
  }
  printf("======================================\n");




E 118
  /***************************************/
  /***************************************/









E 114
D 115
#ifdef USECDEV
E 115
I 115
  /* ?????
E 115
E 112
D 104
  // copy bpm channel names
  if(no_ipc==0) {
    int i;
    for(i=0; i<nbpm; i++) strncpy((char *)(&bpmarray[i][1]),blanks,32);
    for(i=0; i<nbpm; i++) {
      if(strlen(bpmget[i])>4) {
	sprintf(temp,"%s.%s",bpmname[i],&(bpmget[i])[4]);
	strncpy((char *)(&bpmarray[i][1]),temp,min(strlen(temp),31));
      } else {
	strncpy((char *)(&bpmarray[i][1]),bpmname[i],min(strlen(bpmname[i]),31));
E 99
      }
    }
  }


E 104
D 81
  // create callback objects once only
E 81
I 81
  // create channel callback objects once only
E 81
D 112
  for(int i=0; i<nepics; i++) {
E 112
I 112
  for(i=0; i<nepics; i++)
  {
E 112
D 81
    cb[i]    = new cdevCallback(epics_callback_func,(void*)i);
E 81
I 81
    cb[i]    = new cdevCallback(channel_callback_func,(void*)i);
E 81
D 112
    
    if(strcmp(epics[i].name,"bpm_1_i")==0) bpm1=&(epics_val[i]);    // for helicity file
E 112
I 112
    if(strcmp(epics[i].name,"bpm_1_i")==0) bpm1=&(epics_val[i]); // for helicity file
E 112
    if(strcmp(epics[i].name,"bpm_3_i")==0) bpm3=&(epics_val[i]);
  }
I 112
D 115
#else
E 115
I 115
  */
E 115



  /*****************************************************/
  /* setting callbacks in case of channel value change */
  {
    int returncode = 0;
D 114
    int n = 0;
    int result;         /* CA result */
    int nPvs = nepics;  /* Number of PVs */
    pv* pvs = 0;        /* Array of PV structures */
E 114
I 114
    int n, nn;
    int result;    /* CA result */
    int nPvs;      /* Number of PVs */
    pv *pvs = 0;   /* Array of PV structures */
E 114
    char *valstr;
D 114
    float valfloat;
E 114
I 114
    float valfloatarray[MAX_CHANNELS_LENGTH];
E 114

D 114
   /* check if we have at lease one pv */
    if (nPvs < 1)
E 114
I 114
    /* check if we have at lease one pv */
D 118
    nPvs = nepics + nscalers;
E 118
I 118
    nPvs = nepics + nscalers + ngamma;
E 118
    if(nPvs < 1)
E 114
    {
D 114
        fprintf(stderr, "epics_monitor: No pv names specified\n");
        return(1);
E 114
I 114
      fprintf(stderr, "epics_monitor: No pv names specified\n");
      return(1);
E 114
    }

    /* Start up Channel Access */
    result = ca_context_create(ca_disable_preemptive_callback);
    if(result != ECA_NORMAL)
    {
      fprintf(stderr, "epics_monitor: CA error %s occurred while trying "
              "to start channel access '%s'.\n", ca_message(result), pvs[n].name);
      return(1);
    }

    /* Allocate PV structure array */
    pvs = (pv *)calloc(nPvs, sizeof(pv));
    if (!pvs)
    {
      fprintf(stderr, "Memory allocation for channel structures failed.\n");
      return(1);
    }

D 114
    /* Connect channels */
E 114
I 114
    /* Connect channels; we'll put all 'epics' and 'scalers' and whatever
    into one 'pvs' array with through enumeration; event_handler will check
    '.index' and recognize if it is 'epics' or 'scalers' etc */
E 114

    /* Copy PV names from epics[].name and assign index starting from 0 */
I 114
    nn = 0;
E 114
    for(n=0; n<nepics; n++)
    {
D 114
      pvs[n].name = epics[n].chan;
      pvs[n].index = n;
E 114
I 114
      pvs[nn].name = epics[n].chan;
      pvs[nn].index = nn;
      nn ++;
E 114
    }
I 114
    /* same for scalers */
    for(n=0; n<nscalers; n++)
    {
      pvs[nn].name = scalers[n].chan;
      pvs[nn].index = nn;
      nn ++;
    }
I 118
    /* same for gamma */
    for(n=0; n<ngamma; n++)
    {
      pvs[nn].name = gammas[n].chan;
      pvs[nn].index = nn;
      nn ++;
    }
E 118
E 114

    /* Create CA connections */
    returncode = create_pvs(pvs, nPvs, connection_handler);
    if(returncode)
    {
      return(returncode);
    }

    /* Check for channels that didn't connect */
    ca_pend_event(caTimeout);
    for(n=0; n<nPvs; n++)
    {
      if(!pvs[n].onceConnected)
	  {
I 121
        /*printf("NOT CONNECTED\n");*/
E 121
D 114
        valstr = print_time_val_sts(&pvs[n], pvs[n].reqElems, &valfloat);
E 114
I 114
        valstr = print_time_val_sts(&pvs[n], pvs[n].reqElems, valfloatarray);
E 114
      }
    }


  }

D 115



#endif


E 112
  
I 81

E 115
E 81
D 112
  return;
E 112
I 112
  return(0);
E 112
}


D 112
//--------------------------------------------------------------------------
E 112
E 67
D 115


I 17
D 49
void monitor_callback_func(int status, void *userarg, cdevRequestObject &epics_req_obj,
			    cdevData& result) {
D 27
  int i = (int) userarg;
E 27

D 27
  get[i]=NULL;
E 27
I 27
D 28
  if (status==CDEV_DISCONNECTED) {
E 28
I 28
D 39
  if ((debug==1)&&(status==CDEV_DISCONNECTED)) {
E 28
    cerr << "*** disconnect on channel:  " << (epics_req_obj.device()).name() << endl;
E 39
I 39
  time_t now = time(NULL);
  if (status==CDEV_DISCONNECTED) {
    cerr << "*** disconnect on channel:  " << (epics_req_obj.device()).name() <<  " at " << ctime(&now) << flush;
  } else if (status==CDEV_RECONNECTED) {
    cerr << "***  reconnect on channel:  " << (epics_req_obj.device()).name() <<  " at " << ctime(&now) << flush;
E 39
  }

E 27
  return;
}


//---------------------------------------------------------------------------


E 49
E 17
D 46
void status_poll_callback(T_IPC_MSG msg){
E 46
I 46
D 112
void status_poll_callback(T_IPC_MSG msg) {
E 112
E 46

E 115
I 7
D 112
  
E 112
I 112

/********************************************************************/
/**************** UTILITY functions (no epics calls) ****************/
/********************************************************************/

//--------------------------------------------------------------------------
void
status_poll_callback(T_IPC_MSG msg)
{  
E 112
I 48
D 91
  TipcMsgAppendStr(msg,"number of epics reads");
E 91
I 91
  TipcMsgAppendStr(msg,(char*)"number of epics reads");
E 91
  TipcMsgAppendInt4(msg,nepics_read);
  
I 52
D 55
  TipcMsgAppendStr(msg,"run");
  TipcMsgAppendInt4(msg,run);
  
  TipcMsgAppendStr(msg,"run status");
  TipcMsgAppendInt4(msg,run_status);
  
E 52
E 48
I 18
  TipcMsgAppendStr(msg,"nevent sent to dd");
E 55
I 55
D 72
  TipcMsgAppendStr(msg,"nevent sent to ipcbank2dd");
E 55
  TipcMsgAppendInt4(msg,nev_to_dd);
E 72
I 72
D 91
  TipcMsgAppendStr(msg,"nevent sent to ipc");
E 91
I 91
  TipcMsgAppendStr(msg,(char*)"nevent sent to ipc");
E 91
  TipcMsgAppendInt4(msg,nev_to_ipc);
E 72
  
E 18
I 17
D 55
  TipcMsgAppendStr(msg,"nevent no dd");
  TipcMsgAppendInt4(msg,nev_no_dd);
E 55
I 55
D 91
  TipcMsgAppendStr(msg,"destination");
E 91
I 91
  TipcMsgAppendStr(msg,(char*)"destination");
E 91
  TipcMsgAppendStr(msg,dest);
E 55
  
I 41
D 55
  TipcMsgAppendStr(msg,"nevent no run");
  TipcMsgAppendInt4(msg,nev_no_run);
  
E 55
E 41
D 81
  TipcMsgAppendStr(msg,"wait_time");
  TipcMsgAppendInt4(msg,wait_time);
E 81
I 81
D 91
  TipcMsgAppendStr(msg,"channel_wait_time");
E 91
I 91
  TipcMsgAppendStr(msg,(char*)"channel_wait_time");
E 91
  TipcMsgAppendInt4(msg,channel_wait_time);

D 91
  TipcMsgAppendStr(msg,"scaler_wait_time");
E 91
I 91
  TipcMsgAppendStr(msg,(char*)"scaler_wait_time");
E 91
  TipcMsgAppendInt4(msg,scaler_wait_time);
E 81

I 118
  TipcMsgAppendStr(msg,(char*)"gamma_wait_time");
  TipcMsgAppendReal4(msg,gamma_wait_time);

E 118
D 91
  TipcMsgAppendStr(msg,"cdev_pend_time");
E 91
I 91
D 116
  TipcMsgAppendStr(msg,(char*)"cdev_pend_time");
E 91
  TipcMsgAppendInt4(msg,cdev_pend_time);
E 116
I 116
  TipcMsgAppendStr(msg,(char*)"ipc_pend_time");
  TipcMsgAppendInt4(msg,ipc_pend_time);
E 116

I 50
D 91
  TipcMsgAppendStr(msg,"cdev_pend_repeat");
E 91
I 91
D 116
  TipcMsgAppendStr(msg,(char*)"cdev_pend_repeat");
E 91
  TipcMsgAppendInt4(msg,cdev_pend_repeat);

E 116
E 50
D 36
  TipcMsgAppendStr(msg,"cdev_retry_time");
  TipcMsgAppendInt4(msg,cdev_retry_time);

E 36
D 72
  TipcMsgAppendStr(msg,"no_dd");
  TipcMsgAppendInt4(msg,no_dd);

E 72
D 91
  TipcMsgAppendStr(msg,"no_ipc");
E 91
I 91
  TipcMsgAppendStr(msg,(char*)"no_ipc");
E 91
  TipcMsgAppendInt4(msg,no_ipc);

I 72
D 91
  TipcMsgAppendStr(msg,"no_info");
E 91
I 91
  TipcMsgAppendStr(msg,(char*)"no_info");
E 91
  TipcMsgAppendInt4(msg,no_info);

E 72
I 48
D 91
  TipcMsgAppendStr(msg,"no_file");
E 91
I 91
  TipcMsgAppendStr(msg,(char*)"no_file");
E 91
  TipcMsgAppendInt4(msg,no_file);

I 61
D 91
  TipcMsgAppendStr(msg,"no_hel_file");
E 91
I 91
D 112
  TipcMsgAppendStr(msg,(char*)"no_hel_file");
E 91
  TipcMsgAppendInt4(msg,no_hel_file);

E 112
E 61
I 52
D 55
  TipcMsgAppendStr(msg,"dd_ok");
  TipcMsgAppendInt4(msg,dd_ok);

E 55
E 52
E 48
D 91
  TipcMsgAppendStr(msg,"");
  TipcMsgAppendStr(msg,"");
E 91
I 91
  TipcMsgAppendStr(msg,(char*)"");
  TipcMsgAppendStr(msg,(char*)"");
E 91

I 118
  /* ??? need same for gamma ??? */
E 118
E 17
D 46
  for(int i=0; i<nepics; i++){
E 46
I 46
  for(int i=0; i<nepics; i++) {
E 46
D 91
    TipcMsgAppendStr(msg,epics[i].name);
E 91
I 91
    TipcMsgAppendStr(msg,(char*)epics[i].name);
E 91
D 9
    TipcMsgAppendInt4(msg,epics_val[i]);
E 9
I 9
    TipcMsgAppendReal4(msg,epics_val[i]);
E 9
  }

E 7
  return;
}

D 112

E 112
//-------------------------------------------------------------------
D 112


D 46
void quit_callback(int sig){
E 46
I 46
void quit_callback(int sig) {
E 46

E 112
I 112
void
quit_callback(int sig)
{
E 112
  done=1;

  return;
}

I 116
//--------------------------------------------------------------------------
E 116

D 116
//-------------------------------------------------------------------
D 112


E 112
// process existing messages in Smartsockets receipt queue
D 46
int ipc_mainloop(int opened, int fd, void *arg){
E 46
I 46
D 112
int ipc_mainloop(int opened, int fd, void *arg) {
E 46
  
E 112
I 112
int
ipc_mainloop(int opened, int fd, void *arg)
{  
E 112
D 46
  if(opened==0){
E 46
I 46
  if(opened==0) {
E 46
    cerr << "Cdevsys unable to attach to ipc fd: " << fd << endl;
    return(-1);
E 116

D 116
  } else {
    server.MainLoop(0.0);
    return (0);
E 116
I 116
//  receives online info from other programs and dispatches to appropriate callbacks
void *
ipc_thread(void *param)
{
  while(done==0)
  {
D 118
    printf("ipc_thread is running ..\n");
E 118
I 118
    /*printf("ipc_thread is running ..\n");*/
E 118
    server.MainLoop((double)ipc_pend_time);
E 116
  }
I 116
  printf("ipc_thread exited\n");

  return((void*)NULL);
E 116
}


//-------------------------------------------------------------------

D 112

D 46
void init_tcl(){
E 46
I 46
void init_tcl() {
E 46

  // link c and Tcl variables
D 91
  Tcl_LinkVar(interp,"application",    	   (char *)&application,      	TCL_LINK_STRING);
  Tcl_LinkVar(interp,"unique_id",     	   (char *)&unique_id,          TCL_LINK_STRING);
  Tcl_LinkVar(interp,"session",     	   (char *)&session,            TCL_LINK_STRING);
D 5
  Tcl_LinkVar(interp,"current_run_file",   (char *)&current_run_file,   TCL_LINK_STRING);
E 5
I 5
D 55
  Tcl_LinkVar(interp,"msql_database",  	   (char *)&msql_database,      TCL_LINK_STRING);
E 55
I 55
D 56
  Tcl_LinkVar(interp,"dest",     	   (char *)&dest,               TCL_LINK_STRING|TCL_LINK_READ_ONLY);
E 56
I 56
  //  Tcl_LinkVar(interp,"dest",     	   (char *)&dest,               TCL_LINK_STRING|TCL_LINK_READ_ONLY);
E 56
E 55
E 5
D 81
  Tcl_LinkVar(interp,"wait_time",     	   (char *)&wait_time,          TCL_LINK_INT);
E 81
I 81
  Tcl_LinkVar(interp,"channel_wait_time",  (char *)&channel_wait_time,  TCL_LINK_INT);
  Tcl_LinkVar(interp,"scaler_wait_time",   (char *)&scaler_wait_time,   TCL_LINK_INT);
E 81
  Tcl_LinkVar(interp,"cdev_pend_time", 	   (char *)&cdev_pend_time,     TCL_LINK_INT);
I 50
  Tcl_LinkVar(interp,"cdev_pend_repeat",   (char *)&cdev_pend_repeat,   TCL_LINK_INT);
E 50
I 15
D 36
  Tcl_LinkVar(interp,"cdev_retry_time",	   (char *)&cdev_retry_time,    TCL_LINK_INT);
E 36
E 15
D 72
  Tcl_LinkVar(interp,"no_dd",     	   (char *)&no_dd,              TCL_LINK_INT);
E 72
I 12
D 55
  Tcl_LinkVar(interp,"force_dd",     	   (char *)&force_dd,           TCL_LINK_INT);
E 55
E 12
I 4
  Tcl_LinkVar(interp,"no_ipc",     	   (char *)&no_ipc,             TCL_LINK_INT);
I 72
  Tcl_LinkVar(interp,"no_info",     	   (char *)&no_info,            TCL_LINK_INT);
E 72
I 48
  Tcl_LinkVar(interp,"no_file",     	   (char *)&no_file,            TCL_LINK_INT);
I 61
  Tcl_LinkVar(interp,"no_hel_file",    	   (char *)&no_hel_file,        TCL_LINK_INT);
E 61
E 48
I 5
  Tcl_LinkVar(interp,"dump",     	   (char *)&dump,               TCL_LINK_INT);
I 48
  Tcl_LinkVar(interp,"nepics_read",    	   (char *)&nepics_read,        TCL_LINK_INT|TCL_LINK_READ_ONLY);
E 48
I 18
D 41
  Tcl_LinkVar(interp,"nev_to_dd",     	   (char *)&nev_to_dd,          TCL_LINK_INT);
E 41
I 41
D 72
  Tcl_LinkVar(interp,"nev_to_dd",     	   (char *)&nev_to_dd,          TCL_LINK_INT|TCL_LINK_READ_ONLY);
E 72
I 72
  Tcl_LinkVar(interp,"nev_to_ipc",     	   (char *)&nev_to_ipc,         TCL_LINK_INT|TCL_LINK_READ_ONLY);
E 91
I 91
  Tcl_LinkVar(interp,(char*)"application",    	   (char *)&application,      	TCL_LINK_STRING);
  Tcl_LinkVar(interp,(char*)"unique_id",     	   (char *)&unique_id,          TCL_LINK_STRING);
  Tcl_LinkVar(interp,(char*)"session",     	   (char *)&session,            TCL_LINK_STRING);
  //  Tcl_LinkVar(interp,(char*)"dest",     	   (char *)&dest,               TCL_LINK_STRING|TCL_LINK_READ_ONLY);
D 98
  Tcl_LinkVar(interp,(char*)"channel_wait_time",  (char *)&channel_wait_time,  TCL_LINK_INT);
  Tcl_LinkVar(interp,(char*)"scaler_wait_time",   (char *)&scaler_wait_time,   TCL_LINK_INT);
E 98
I 98
  Tcl_LinkVar(interp,(char*)"channel_wait_time",   (char *)&channel_wait_time,  TCL_LINK_INT);
  Tcl_LinkVar(interp,(char*)"scaler_wait_time",    (char *)&scaler_wait_time,   TCL_LINK_INT);
E 98
  Tcl_LinkVar(interp,(char*)"cdev_pend_time", 	   (char *)&cdev_pend_time,     TCL_LINK_INT);
D 98
  Tcl_LinkVar(interp,(char*)"cdev_pend_repeat",   (char *)&cdev_pend_repeat,   TCL_LINK_INT);
E 98
I 98
  Tcl_LinkVar(interp,(char*)"cdev_pend_repeat",    (char *)&cdev_pend_repeat,   TCL_LINK_INT);
I 103
D 104
  Tcl_LinkVar(interp,(char*)"bpm_wait_time",       (char *)&bpm_wait_time,      TCL_LINK_DOUBLE);
  Tcl_LinkVar(interp,(char*)"bpm_pend_time",       (char *)&bpm_pend_time,      TCL_LINK_DOUBLE);
E 104
E 103
E 98
  Tcl_LinkVar(interp,(char*)"no_ipc",     	   (char *)&no_ipc,             TCL_LINK_INT);
  Tcl_LinkVar(interp,(char*)"no_info",     	   (char *)&no_info,            TCL_LINK_INT);
  Tcl_LinkVar(interp,(char*)"no_file",     	   (char *)&no_file,            TCL_LINK_INT);
  Tcl_LinkVar(interp,(char*)"no_hel_file",    	   (char *)&no_hel_file,        TCL_LINK_INT);
  Tcl_LinkVar(interp,(char*)"dump",     	   (char *)&dump,               TCL_LINK_INT);
  Tcl_LinkVar(interp,(char*)"nepics_read",    	   (char *)&nepics_read,        TCL_LINK_INT|TCL_LINK_READ_ONLY);
  Tcl_LinkVar(interp,(char*)"nev_to_ipc",     	   (char *)&nev_to_ipc,         TCL_LINK_INT|TCL_LINK_READ_ONLY);
I 98
  Tcl_LinkVar(interp,(char*)"debug",     	   (char *)&debug,              TCL_LINK_BOOLEAN);
E 98
E 91
E 72
D 55
  Tcl_LinkVar(interp,"nev_no_dd",     	   (char *)&nev_no_dd,          TCL_LINK_INT|TCL_LINK_READ_ONLY);
D 48
  Tcl_LinkVar(interp,"nev_no_run",     	   (char *)&nev_no_run,          TCL_LINK_INT|TCL_LINK_READ_ONLY);
E 48
I 48
  Tcl_LinkVar(interp,"nev_no_run",     	   (char *)&nev_no_run,         TCL_LINK_INT|TCL_LINK_READ_ONLY);
E 48
E 41
I 33
  Tcl_LinkVar(interp,"dd_ok",     	   (char *)&dd_ok,              TCL_LINK_INT|TCL_LINK_READ_ONLY);
I 37
D 46
  Tcl_LinkVar(interp,"dd_reconnect",   	   (char *)&dd_reconnect,       TCL_LINK_INT);
E 46
E 37
I 35
  Tcl_LinkVar(interp,"run",     	   (char *)&run,                TCL_LINK_INT|TCL_LINK_READ_ONLY);
  Tcl_LinkVar(interp,"run_status",     	   (char *)&run_status,         TCL_LINK_INT|TCL_LINK_READ_ONLY);
E 55
E 35
E 33
E 18
I 17
D 27
  Tcl_LinkVar(interp,"nev_no_dd",     	   (char *)&nev_no_dd,          TCL_LINK_INT);
E 27
E 17
E 5
E 4


  // create Tcl commands
D 91
  Tcl_CreateCommand(interp,"help",tcl_help,
E 91
I 91
  Tcl_CreateCommand(interp,(char*)"help",tcl_help,
E 91
  		    (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
D 91
  Tcl_CreateCommand(interp,"quit",tcl_quit,
E 91
I 91
  Tcl_CreateCommand(interp,(char*)"quit",tcl_quit,
E 91
		    (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
D 91
  Tcl_CreateCommand(interp,"stop",tcl_quit,
E 91
I 91
  Tcl_CreateCommand(interp,(char*)"stop",tcl_quit,
E 91
		    (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
D 91
  Tcl_CreateCommand(interp,"exit",tcl_quit,
E 91
I 91
  Tcl_CreateCommand(interp,(char*)"exit",tcl_quit,
E 91
		    (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
I 14
D 91
  Tcl_CreateCommand(interp,"dump",tcl_dump,
E 91
I 91
  Tcl_CreateCommand(interp,(char*)"dump",tcl_dump,
E 91
		    (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
E 14

  return;

}


//--------------------------------------------------------------------------


D 29
int init_dd(){
E 29
I 29
D 46
void init_dd(){
E 46
I 46
D 55
void init_dd() {
E 46
E 29
  
  int status;
  struct fifo_mode fmode;
D 31
  int ctl[4] = {-1,-1,-1,-1};
E 31


  // connect to DD system INPUT fifo
D 31
  fmode.mode     = FMODE_ALL;
  fmode.wait     = FWAIT_SLEEP;
  fmode.suser    = FMODE_MULTI_USER;
  fmode.prescale = 1;
  fmode.p2ctl    = ctl;
E 31
I 31
  fmode.mode     = dd_mode;
  fmode.wait     = dd_wait;
  fmode.suser    = dd_user;
  fmode.prescale = dd_prescale;
  fmode.p2ctl    = dd_ctl;
E 31
D 27
  status=ddu_init("INPUT",fmode);
  if(status!=0){
    cerr << "?unable to attach to DD system INPUT fifo, status is: " << status << endl;
E 27
I 27

D 29
  if(ddu_attached()<=0) {
E 29
I 29

  if(ddu_exists()==1) {
E 29
    status=ddu_init("INPUT",fmode);
D 28
    if(status!=0){
E 28
I 28
D 29
    if((debug==1)&&(status!=0)) {
E 28
      cerr << "?unable to init DD system INPUT fifo, status is: " << status << endl;
      return(1);
    }
  } else {
E 27
    return(1);
  }
E 29
I 29
D 31
    if((debug==1)&&(status!=0)) cerr << "?unable to init DD system INPUT fifo, status is: " 
				     << status << endl;
  }    
E 31
I 31
D 46
    if(status==0) {
      dd_ok=1; 
      dd_reconnect=0;
    }    
E 46
I 46
    if(status==0) dd_ok=1; 
E 46
  }
E 31
E 29

I 27
D 29

E 27
  // successfully connected to dd system
  return(0);
I 27
D 28
  cerr << "successfully attached" << endl;
E 28
E 27

E 29
I 29
  return;
E 29
}


//---------------------------------------------------------------------


D 31
void insert_into_dd(int run){
E 31
I 31
void insert_into_dd() {
E 31

  int status;
  int *p,i,nused,banksize,nhead,buflen;
  fifo_entry fev;

  // for record segment header
  int nevnt  = 0;
  int nphys  = 0;
D 17
  int trig   = 0;                // fpack class, 17 for prestart event, 0 for normal event, etc.
E 17
I 17
  int trig   = 0;
E 17

  // constants for head bank
  int nvers  = 0;
D 16
  int type   = 1;
E 16
I 16
  int type   = 104;
E 16
  int rocst  = 0;
D 16
  int evcls  = 0;                // fpack class again?
  int presc  = 1;
E 16
I 16
  int evcls  = 0;
  int presc  = 0;
E 16

I 31

E 31
  // DD event control words
D 30
  int ddctl[4] = {-1,-1,-1,-1};
E 30
I 30
  int ddctl[4] = {0,0,0,0};
E 30


I 12
D 17
  // *** reattach not working yet...just set done and return if DD disappears ***
E 12
D 9

E 9
I 7
  // if no dd system, close and try to reattach
I 11
D 12
  // reattach not working yet...just set done and return
E 12
E 11
  if((dd_dcom==0)||(ddu_attached()<=0)){
D 11
    ddu_close();
    if(init_dd()!=0) return;
E 11
I 11
    done=1;
    return;
    //    ddu_close();
    //    if(init_dd()!=0) return;
E 17
I 17
D 46
  // if no dd system try to reattach, return if unable
I 31
  if((ddu_attached()<=0)&&(dd_reconnect==1)) init_dd(); 
E 46
I 46
  // if no dd system try to attach, return if unable
I 49
  // done if lost connection
E 49
E 46
E 31
I 27
D 28
  cerr << "checking if attached..." << endl;
E 28
E 27
  if(ddu_attached()<=0) {
I 40
D 46
    if(dd_ok==1)insert_msg("epics_monitor",unique_id,unique_id,"status",2,"ERROR",0,
			   "Lost connection to DD system");
E 40
D 27
    if(ddu_attach()==0) {
E 27
I 27
D 31
    if(ddu_exists()==1) {
D 28
      cerr << "   ...trying to reinit..." << endl;
E 28
E 27
      init_dd(); 
    } else {
I 27
D 28
      cerr << "   ...dd doesn't exist" << endl;
E 28
E 27
      nev_no_dd++;
      return;
    }
E 31
I 31
    dd_ok=0;
    nev_no_dd++;
    return;
E 46
I 46
    if(dd_ok==1) {
D 48
      if(once_only==0)insert_msg("epics_monitor",unique_id,unique_id,"status",2,"ERROR",0,
				 "epics_monitor lost connection to DD system...stopping");
E 48
I 48
D 49
//       if(once_only==0)insert_msg("epics_monitor",unique_id,unique_id,"status",2,"ERROR",0,
E 49
I 49
//       insert_msg("epics_monitor",unique_id,unique_id,"status",2,"ERROR",0,
E 49
// 				 "epics_monitor lost connection to DD system...stopping");
E 48
      dd_ok=0;
I 49
      lost_connection=1;
E 49
      done=1;
      return;
    }
    init_dd();
    if(ddu_attached()<=0) {
      nev_no_dd++;
      return;
    }
E 46
E 31
E 17
E 11
  }
I 27
D 28
  cerr << "   ...attached" << endl;
E 28
E 27


E 7
  // get free fifo event 
D 27
  status=ddu_req_fev(DDBUFFERSIZE, &fev);
  if(status!=0){
    cerr << "?unable to get fev, status is: " << status << endl;
E 27
I 27
D 31
  if(ddu_attached()>0) {
    status=ddu_req_fev(DDBUFFERSIZE, &fev);
D 28
    if(status!=0){
E 28
I 28
    if((debug==1)&&(status!=0) ){
E 28
      cerr << "?unable to get fev, status is: " << status << endl;
      return;
    }
  } else {
E 31
I 31
  status=ddu_req_fev(DDBUFFERSIZE, &fev);
D 46
  if((debug==1)&&(status!=0) ){
E 46
I 46
  if((debug==1)&&(status!=0) ) {
E 46
    cerr << "?unable to get fev, status is: " << status << endl;
    nev_no_dd++;
E 31
E 27
    return;
  }


  // set pointer, reset counts, fill ctl words, etc.
  p=fev.p2da;
  nused=0;
  nhead=0;
  fev.ctlw1=ddctl[0];
  fev.ctlb1=ddctl[1];
  fev.ctlw2=ddctl[2];
  fev.ctlb2=ddctl[3];


  // create segment header, then update pointer and counters
D 15
  status=create_header(p,DDBUFFERSIZE-nused,nhead,run,nevnt,nphys,trig);
E 15
I 15
  status=create_header(p,DDBUFFERSIZE-nused,nhead,'RUNP','ARMS',run,nevnt,nphys,trig);
E 15
D 46
  if(status==0){
E 46
I 46
  if(status==0) {
E 46
    p+=nhead;
    nused+=nhead;
  }  


  // head bank
  status=va_add_bank(p,DDBUFFERSIZE-nused,"HEAD",0,"I",8,1,8,banksize,
	      nvers,run,nevnt,(int)time(NULL),type,rocst,evcls,presc);
D 46
  if(status==0){
E 46
I 46
  if(status==0) {
E 46
    p+=banksize;
    nused+=banksize;
  }


D 9
  // epics channel title bank
D 7
  buflen=(sizeof(epics_chan)/sizeof(char *))*8;
E 7
I 7
  buflen=nepics*8;
E 7
  status=add_bank(p,DDBUFFERSIZE-nused,"EPIC",0,"8A",13,1,buflen,banksize,
		     (int *)epics_name);
E 9
I 9
  // fill epics bank
  for(i=0; i<nepics; i++) bosarray[i][0] = epics_val[i];
  buflen=nepics*9;
D 12
  status=add_bank(p,DDBUFFERSIZE-nused,"EPIC",0,"F,8A",1,nepics,buflen,banksize,
E 12
I 12
  status=add_bank(p,DDBUFFERSIZE-nused,"EPIC",0,"F,8A",2,nepics,buflen,banksize,
E 12
		     (int *)bosarray);
E 9
D 46
  if(status==0){
E 46
I 46
  if(status==0) {
E 46
    p+=banksize;
    nused+=banksize;
  }


D 9
  // epics data bank
D 7
  buflen=sizeof(epics_chan)/sizeof(char *);
E 7
I 7
  buflen=nepics;
E 7
  status=add_bank(p,DDBUFFERSIZE-nused,"EPIC",1,"I",13,1,buflen,banksize,
		     epics_val);
  if(status==0){
    p+=banksize;
    nused+=banksize;
  }
E 9
I 9
D 13
// ???  // epics channel title bank
//   buflen=nepics*8;
//   status=add_bank(p,DDBUFFERSIZE-nused,"EPIC",0,"8A",1,nepics,buflen,banksize,
// 		     (int *)epics_name);
//   if(status==0){
//     p+=banksize;
//     nused+=banksize;
//   }
E 9


I 9
//   // epics data bank
//   buflen=nepics;
//   status=add_bank(p,DDBUFFERSIZE-nused,"EPIC",1,"F",1,nepics,buflen,banksize,
// 		     (int *)epics_val);
//   if(status==0){
//     p+=banksize;
//     nused+=banksize;
//   }


E 13
E 9
  // all banks added...set fev and fpack overall word counts
  fev.len=nused;
  *(fev.p2da+10)=nused-nhead;


D 31
  // insert event into DD system
E 31
I 31
  // insert event into DD system and return
E 31
D 27
  status=ddu_put_fev(fev);
  if(status!=0){
    cerr << "?unable to put fev, status is: " << status << endl;
    return;
E 27
I 27
  if(ddu_attached()>0) {
    status=ddu_put_fev(fev);
D 28
    if(status!=0){
E 28
I 28
    if((debug==1)&&(status!=0)) {
E 28
      cerr << "?unable to put fev, status is: " << status << endl;
D 31
      return;
    } else {
E 31
      nev_to_dd++;
D 28
      cerr << "wrote event" << endl;
E 28
    }
E 27
I 19
  } else {
D 27
    nev_to_dd++;
E 27
I 27
D 31
    return;
E 31
I 31
    nev_to_dd++;
E 31
E 27
E 19
  }
D 31


E 31
D 49
  return;
E 49
I 31

I 49

  return;
E 49
E 31
}

  
//----------------------------------------------------------------


I 31
void control_message_callback(T_IPC_CONN conn,
			      T_IPC_CONN_PROCESS_CB_DATA data,
			      T_CB_ARG arg) {

  T_STR string;


  // get first string
  TipcMsgSetCurrent(data->msg,0);
  TipcMsgNextStr(data->msg,&string);
  
  
  //  dd status request
D 53
  if(strcasecmp(string,"dd_status")==0) {
E 53
I 53
  if(strcasecmp(string,"dd_status_poll")==0) {
E 53

I 53
    T_STR srvnode = server.Node();

E 53
    TipcMsg status("dd_status");
    status.Dest("/dd_system/status");
    status.Sender(unique_id);
D 42
    status << unique_id << session << (T_INT4) dd_ok << "INPUT"
E 42
I 42
D 53
    status << unique_id << host << session << (T_INT4) dd_ok << "INPUT"
E 42
	   << (T_INT4) nev_to_dd
	   << (T_INT4) dd_mode << (T_INT4) dd_wait << (T_INT4) dd_prescale << (T_INT4) dd_user
D 43
	   << SetSize(4) << dd_ctl;
E 43
I 43
	   << SetSize(4) << (T_INT4*) dd_ctl;
E 53
I 53
    status << unique_id << host << session << srvnode 
D 54
	   << (T_INT4) nepics_read << (T_INT4) nev_to_dd << (T_REAL8)0.0
E 54
I 54
	   << (T_INT4) nepics_read << (T_REAL8) -1.0 << (T_INT4) nev_to_dd << (T_REAL8) -1.0
E 54
	   << (T_INT4) dd_ok << "INPUT";
E 53
E 43
    server.Send(status,TRUE);
    server.Flush();


D 46
  //  only try to dd reconnect if NOT connected
E 46
I 46
  //  never reconnect
E 46
  } else if(strcasecmp(string,"dd_reconnect")==0) {
I 49
    // do nothing
E 49
D 46
    if(dd_ok==0)dd_reconnect=1;
E 46


  //  don't understand message...ship to smartsockets interpreter
  } else {
    TutCommandParseStr(string);
  }

  return;
}


//----------------------------------------------------------------------


E 55
E 31
D 46
void decode_command_line(int argc, char**argv){
E 46
I 46
void decode_command_line(int argc, char**argv) {
E 46

E 112
I 112
void
decode_command_line(int argc, char**argv)
{
E 112
D 3
  char *help = "\nusage:\n\n  epics_monitor [-a application] [-u unique_id]"
    " [-s session] [-t init_tcl_script] [-w wait_time] [-p cdev_pend_time] [-no_dd]\n";
E 3
I 3
D 46
  char *help = "\nusage:\n\n  epics_monitor [-a application] [-u unique_id]\n"
E 46
I 46
D 47
  char *help = "\nusage:\n\n  epics_monitor [-a application] [-u unique_id] [-once]\n"
E 47
I 47
D 49
  char *help = "\nusage:\n\n  epics_monitor [-a application] [-u unique_id] [-once] [-restart]\n"
E 49
I 49
D 55
  char *help = "\nusage:\n\n  epics_monitor [-a application] [-u unique_id] [-r restart_time]\n"
E 49
E 47
E 46
D 4
    "        [-s session] [-t init_tcl_script] [-w wait_time] [-p cdev_pend_time] [-no_dd]\n";
E 4
I 4
D 5
    "        [-s session] [-t init_tcl_script] [-w wait_time] [-p cdev_pend_time] [-no_dd] [no_ipc]\n";
E 5
I 5
    "        [-s session] [-m msql_database] [-t init_tcl_script] [-w wait_time]\n"
D 9
    "        [-p cdev_pend_time] [-no_dd] [no_ipc] [-dump]\n";
E 9
I 9
D 12
    "        [-p cdev_pend_time] [-no_dd] [-no_ipc] [-dump]\n";
E 12
I 12
D 15
    "        [-p cdev_pend_time] [-no_dd] [-force] [-no_ipc] [-dump]\n";
E 15
I 15
D 28
    "        [-p cdev_pend_time] [-r cdev_retry_time] [-no_dd] [-force] [-no_ipc] [-dump]\n";
E 28
I 28
D 36
    "        [-p cdev_pend_time] [-r cdev_retry_time] [-no_dd] [-force] [-no_ipc] [-dump] [-debug]\n";
E 36
I 36
D 48
    "        [-p cdev_pend_time] [-no_dd] [-force] [-no_ipc] [-dump] [-debug]\n";
E 48
I 48
D 50
    "        [-p cdev_pend_time] [-no_dd] [-force] [-no_ipc] [-no_file] [-dump] [-debug]\n";
E 50
I 50
    "        [-cp cdev_pend_time] [-cr cdev_pend_repeat] [-no_dd] [-force] [-no_ipc] [-no_file] [-dump] [-debug]\n";
E 55
I 55
D 72
  char *help = "\nusage:\n\n  epics_monitor [-a application] [-u unique_id]\n"
E 72
I 72
D 91
  char *help = "\nusage:\n\n  epics_monitor [-a application] [-s session] [-u unique_id]\n"
E 91
I 91
  const char *help = "\nusage:\n\n  epics_monitor [-a application] [-s session] [-u unique_id]\n"
E 91
E 72
D 81
    "        [-s session] [-t init_tcl_script] [-w wait_time]\n"
E 81
I 81
D 112
    "        [-t init_tcl_script] [-cw channel_wait_time] [-sw scaler_wait_time]\n"
E 112
I 112
D 118
    "        [-cw channel_wait_time] [-sw scaler_wait_time]\n"
E 118
I 118
    "        [-cw channel_wait_time] [-sw scaler_wait_time] [-gw gamma_wait_time]\n"
E 118
E 112
E 81
D 61
    "        [-cp cdev_pend_time] [-cr cdev_pend_repeat] [-no_dd] [-no_ipc] [-no_file] [-dump] [-debug]\n";
E 61
I 61
D 116
    "        [-cp cdev_pend_time] [-cr cdev_pend_repeat] [-m msql_database]\n"
E 116
I 116
    "        [-ipc ipc_pend_time] [-m msql_database]\n"
E 116
D 72
    "        [-no_dd] [-no_ipc] [-no_file] [-no_hel_file] [-dump] [-debug]\n";
E 72
I 72
D 95
    "        [-no_ipc] [-no_info] [-no_file] [-no_hel_file] [-dump] [-debug]\n";
E 95
I 95
D 112
    "        [-no_ipc] [-no_info] [-no_file] [-no_hel_file] [-no_scalers] [-dump] [-debug]\n";
E 112
I 112
    "        [-no_ipc] [-no_info] [-no_file] [-no_scalers] [-dump] [-debug]\n";
E 112
E 95
E 72
E 61
E 55
E 50
E 48
E 36
E 28
E 15
E 12
E 9
E 5
E 4
E 3


  // loop over all arguments, except the 1st (which is program name)
  int i=1;
  while(i<argc) {
D 46
    if(strncasecmp(argv[i],"-h",2)==0){
E 46
I 46
    if(strncasecmp(argv[i],"-h",2)==0) {
E 46
      cout << help << endl;
      exit(EXIT_SUCCESS);
I 95
    }
    else if (strncasecmp(argv[i],"-no_scalers",11)==0) {
      no_scalers=1;
      i=i+1;
E 95
    }
D 46
    else if (strncasecmp(argv[i],"-no_dd",6)==0){
E 46
I 46
D 72
    else if (strncasecmp(argv[i],"-no_dd",6)==0) {
E 46
      no_dd=1;
      i=i+1;
    }
E 72
I 12
D 46
    else if (strncasecmp(argv[i],"-force",6)==0){
E 46
I 46
D 55
    else if (strncasecmp(argv[i],"-force",6)==0) {
E 46
      force_dd=1;
      i=i+1;
    }
E 55
E 12
I 4
D 46
    else if (strncasecmp(argv[i],"-no_ipc",7)==0){
E 46
I 46
    else if (strncasecmp(argv[i],"-no_ipc",7)==0) {
E 46
      no_ipc=1;
      i=i+1;
    }
I 72
    else if (strncasecmp(argv[i],"-no_info",8)==0) {
      no_info=1;
      i=i+1;
    }
E 72
I 48
    else if (strncasecmp(argv[i],"-no_file",8)==0) {
      no_file=1;
      i=i+1;
    }
I 61
D 112
    else if (strncasecmp(argv[i],"-no_hel_file",12)==0) {
      no_hel_file=1;
      i=i+1;
    }
E 112
E 61
E 48
I 5
D 46
    else if (strncasecmp(argv[i],"-dump",5)==0){
E 46
I 46
    else if (strncasecmp(argv[i],"-dump",5)==0) {
E 46
      dump=1;
      i=i+1;
    }
I 28
D 46
    else if (strncasecmp(argv[i],"-debug",6)==0){
E 46
I 46
    else if (strncasecmp(argv[i],"-debug",6)==0) {
E 46
      debug=1;
      i=i+1;
    }
I 50
D 116
    else if (strncasecmp(argv[i],"-cp",3)==0) {
      cdev_pend_time=atoi(argv[i+1]);
E 116
I 116
    else if (strncasecmp(argv[i],"-ipc",3)==0) {
      ipc_pend_time=atoi(argv[i+1]);
E 116
      i=i+2;
    }
D 116
    else if (strncasecmp(argv[i],"-cr",3)==0) {
      cdev_pend_repeat=atoi(argv[i+1]);
      i=i+2;
    }
E 116
I 82
    else if (strncasecmp(argv[i],"-cw",3)==0) {
      channel_wait_time=atoi(argv[i+1]);
      i=i+2;
    }
    else if (strncasecmp(argv[i],"-sw",3)==0) {
      scaler_wait_time=atoi(argv[i+1]);
      i=i+2;
    }
I 118
    else if (strncasecmp(argv[i],"-gw",3)==0) {
      gamma_wait_time=atof(argv[i+1]);
      i=i+2;
    }
E 118
E 82
E 50
E 28
E 5
E 4
D 46
    else if (strncasecmp(argv[i],"-a",2)==0){
E 46
I 46
D 49
    else if (strncasecmp(argv[i],"-once",5)==0) {
      once_only=1;
      i=i+1;
    }
I 47
    else if (strncasecmp(argv[i],"-restart",8)==0) {
     restart=atoi(argv[i+1]);
E 49
I 49
D 55
    else if (strncasecmp(argv[i],"-r",2)==0) {
     restart_time=atoi(argv[i+1]);
E 49
      i=i+2;
    }
E 55
E 47
    else if (strncasecmp(argv[i],"-a",2)==0) {
E 46
      application=strdup(argv[i+1]);
I 72
      i=i+2;
    }
    else if (strncasecmp(argv[i],"-s",2)==0) {
      session=strdup(argv[i+1]);
E 72
      i=i+2;
    }
D 46
    else if (strncasecmp(argv[i],"-u",2)==0){
E 46
I 46
    else if (strncasecmp(argv[i],"-u",2)==0) {
E 46
      unique_id=strdup(argv[i+1]);
      i=i+2;
    }
D 46
    else if (strncasecmp(argv[i],"-s",2)==0){
E 46
I 46
    else if (strncasecmp(argv[i],"-s",2)==0) {
E 46
      session=strdup(argv[i+1]);
      i=i+2;
    }
I 5
D 46
    else if (strncasecmp(argv[i],"-m",2)==0){
E 46
I 46
D 55
    else if (strncasecmp(argv[i],"-m",2)==0) {
E 46
      msql_database=strdup(argv[i+1]);
      i=i+2;
    }
E 55
E 5
D 46
    else if (strncasecmp(argv[i],"-t",2)==0){
E 46
I 46
D 112
    else if (strncasecmp(argv[i],"-t",2)==0) {
E 46
      init_tcl_script=strdup(argv[i+1]);
D 82
      i=i+2;
    }
D 46
    else if (strncasecmp(argv[i],"-w",2)==0){
E 46
I 46
D 81
    else if (strncasecmp(argv[i],"-w",2)==0) {
E 46
      wait_time=atoi(argv[i+1]);
E 81
I 81
    else if (strncasecmp(argv[i],"-cw",3)==0) {
      channel_wait_time=atoi(argv[i+1]);
      i=i+2;
    }
    else if (strncasecmp(argv[i],"-sw",3)==0) {
      scaler_wait_time=atoi(argv[i+1]);
E 82
E 81
      i=i+2;
    }
E 112
I 61
    else if (strncasecmp(argv[i],"-m",2)==0) {
      msql_database=strdup(argv[i+1]);
      i=i+2;
    }
E 61
D 46
    else if (strncasecmp(argv[i],"-p",2)==0){
E 46
I 46
D 50
    else if (strncasecmp(argv[i],"-p",2)==0) {
E 46
      cdev_pend_time=atoi(argv[i+1]);
      i=i+2;
    }
E 50
I 15
D 36
    else if (strncasecmp(argv[i],"-r",2)==0){
      cdev_retry_time=atoi(argv[i+1]);
      i=i+2;
    }
E 36
E 15
    else if (strncasecmp(argv[i],"-",1)==0) {
      cout << "Unknown command line arg: " << argv[i] << argv[i+1] << endl << endl;
D 27
      i=i+2;
E 27
I 27
      i=i+1;
E 27
    }
  }

  return;
}

  
//----------------------------------------------------------------
D 112


int tcl_help(ClientData clientdata, Tcl_Interp *interp,
D 46
		int argc, char **argv){
E 46
I 46
		int argc, char **argv) {
E 46

D 91
    char *help =
E 91
I 91
    const char *help =
E 91
    "\nTcl commands available in the epics_monitor program:\n\n"
    " help                  print this message\n"
    " stop                  stop program\n"
    " quit                  stop program\n"
    " exit                  stop program\n"
I 14
    " dump                  dump all epics channel data\n"
E 14
    "\n\n Type command that require args with NO args for more information\n"
    "\n";

D 91
    Tcl_SetResult(interp,help,TCL_STATIC);
E 91
I 91
    Tcl_SetResult(interp,(char*)help,TCL_STATIC);
E 91

  return (TCL_OK);

}


//---------------------------------------------------------------------


int tcl_quit(ClientData clientdata, Tcl_Interp *interp,
D 46
	     int argc, char **argv){
E 46
I 46
	     int argc, char **argv) {
E 46
  
  done=1;
  
  return (TCL_OK);
}
I 14


//---------------------------------------------------------------------


int tcl_dump(ClientData clientdata, Tcl_Interp *interp,
D 46
	     int argc, char **argv){
E 46
I 46
	     int argc, char **argv) {
E 46
  
  char line[120];

D 46
  for(int i=0; i<nepics; i++){
E 46
I 46
  for(int i=0; i<nepics; i++) {
E 46
D 17
    sprintf(line,"%30s  =  %f\n",epics[i].name,epics_val[i]);
E 17
I 17
    sprintf(line,"%30s  =  %f\n",epics[i].chan,epics_val[i]);
E 17
D 91
    Tcl_AppendResult(interp,line,(char *)NULL);
E 91
I 91
    Tcl_AppendResult(interp,line,NULL);
E 91
  }

  return (TCL_OK);
}
E 14


//---------------------------------------------------------------------

E 112

I 120
/*
CAC TCP socket shutdown error was Transport endpoint is not connected
ipc_thread exited

ipc_close called...closing connection at Wed Nov  7 14:04:10 2007



IPC atexit handler called at Wed Nov  7 14:04:10 2007


IGT0I00BIASET                  *** Not connected (PV not found)
PSPECIRBCK                     *** Not connected (PV not found)
harp.DRBV                      *** Not connected (PV not found)
bom_sc_sum                     *** Not connected (PV not found)
hallbptdt                      *** Not connected (PV not found)
hallbpttgt                     *** Not connected (PV not found)
hallbptenc                     *** Not connected (PV not found)
hallbptlinenc                  *** Not connected (PV not found)
hallbptindex                   *** Not connected (PV not found)
hallbptcur                     *** Not connected (PV not found)
hallbpteio                     *** Not connected (PV not found)
hallbptcal                     *** Not connected (PV not found)
hallbptpol                     *** Not connected (PV not found)
LL8240                         *** Not connected (PV not found)
ITC502_2_T1                    *** Not connected (PV not found)
LongCellCharge                 *** Not connected (PV not found)
ShortCellCharge                *** Not connected (PV not found)
FI_He3_moles                   *** Not connected (PV not found)



*/
E 120
E 1
