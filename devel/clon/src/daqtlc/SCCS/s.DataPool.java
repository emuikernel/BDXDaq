h02521
s 00002/00002/00268
d D 1.18 04/01/15 13:42:19 gurjyan 19 18
c scaler4 and scaler3 are removed from the monitoring cycle
e
s 00002/00002/00268
d D 1.17 03/04/24 14:19:38 gurjyan 18 17
c *** empty log message ***
e
s 00001/00001/00269
d D 1.16 03/04/24 14:18:18 gurjyan 17 16
c *** empty log message ***
e
s 00001/00001/00269
d D 1.15 03/04/24 14:17:54 gurjyan 16 15
c *** empty log message ***
e
s 00001/00001/00269
d D 1.14 03/04/17 17:03:01 boiarino 15 14
c no tage
e
s 00001/00001/00269
d D 1.13 02/09/30 13:17:40 gurjyan 14 13
c included tage
c 
e
s 00001/00001/00269
d D 1.12 01/10/27 05:56:21 gurjyan 13 12
c no tage
e
s 00000/00000/00270
d D 1.11 01/10/19 14:56:09 gurjyan 12 11
c removed tage from the control
c 
e
s 00008/00001/00262
d D 1.10 01/10/19 13:25:12 gurjyan 11 10
c test
e
s 00004/00000/00259
d D 1.9 01/10/12 08:53:39 gurjyan 10 9
c *** empty log message ***
e
s 00001/00001/00258
d D 1.8 01/02/14 08:42:03 gurjyan 9 8
c remove sc-laser1 from the control
c 
e
s 00002/00002/00257
d D 1.7 00/12/15 11:47:57 gurjyan 8 7
c added pretrig1 and pretrig2
c 
e
s 00002/00002/00257
d D 1.6 00/11/06 11:44:25 gurjyan 7 6
c pretrig1 and pretrig2 is out from the control chain. They do not have remote reset hardware.
c 
e
s 00001/00001/00258
d D 1.5 00/10/18 12:44:24 gurjyan 6 5
c added tage
c 
e
s 00001/00001/00258
d D 1.4 00/10/17 14:14:26 gurjyan 5 4
c 
e
s 00001/00001/00258
d D 1.3 00/10/17 12:59:40 gurjyan 4 3
c 5-th roc after dc10 tage is removed from the ts_rocbr_def
c 
e
s 00003/00000/00256
d D 1.2 00/10/17 11:33:35 gurjyan 3 1
c 
e
s 00000/00000/00000
d R 1.2 70/01/01 00:00:02 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 daqtlc/DataPool.java
e
s 00256/00000/00000
d D 1.1 00/07/31 14:48:42 gurjyan 1 0
c date and time created 00/07/31 14:48:42 by gurjyan
e
u
U
f e 0
t
T
I 1
import java.lang.*;

/** Class DataPool is providing data structure for DaqTlc.
 @author Vardan H Gyurjyan. 11.19.99 CLAS Online Group.
 */
public class DataPool {

  //final variables
I 10
    //public static final int MaxProgress = 1000;
    //public static final int ProgressStep  = 50;
D 11
    
E 11

I 11
    public static int daq_recover_clicked = 0;

E 11
E 10
  public  static final String[] ts_rocbr_def = {
    "dc9",
    "dc3",
    "dc4",
    "dc10", 
I 14
D 15
    "tage",
E 15
I 15
    "not used", //"tage",
E 15
E 14
I 6
D 13
    "tage",
E 13
E 6
D 4
    "tage",
E 4
    "not used",
    "not used",
I 13
D 14
    "not used",
E 14
E 13
I 4
D 6
    "not used",
E 6
E 4
    "lac1",
    "dc2",
    "dc8",
    "dc5",
    "dc1",
    "dc7",
    "dc6",
    "dc11",
    "polar",
    "sc1",
    "cc1",
    "ec1",
    "ec2",
    "tlv1",
    "not used",
    "not used",
    "not used",
    "not used",
    "not used",
    "not used",
    "not used",
D 19
    "scaler4",
D 16
    "scaler3", 
E 16
I 16
D 17
    "not usedc", 
E 17
I 17
D 18
    "not used", 
E 18
I 18
    "scaler3", 
E 19
I 19
    "not used", //scaler4
    "not used", //scaler3
E 19
E 18
E 17
E 16
    "scaler1",
    "scaler2",
    "camac1",
    "camac2",
D 18
    "camac3",
E 18
I 18
    "not used",
E 18
D 7
    "pretrig1",
    "pretrig2",
E 7
I 7
D 8
    "not used",
    "not used",
E 8
I 8
    "pretrig1",
    "pretrig2",
E 8
E 7
    "dccntrl",
D 9
    "sc-laser1"
E 9
I 9
    "not used"
E 9
  };

  public  static final String[] roc_graph_name = {
    "9",
    "3",
    "4",
    "10", 
    "tg",
    "",
    "",
    "l",
    "2",
    "8",
    "5",
    "1",
    "7",
    "6",
    "11",
    "p",
    "s",
    "c",
    "e1",
    "e2",
    "tl",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "s4",
    "s3", 
    "s1",
    "s2",
    "c1",
    "c2",
    "c3",
    "p1",
    "p2",
    "dc",
    "sl"
  };

  public  static final String[] roc_description = {
    "FASTBUS crate. Space Frame Level3.   Bargraph X=1 \n Includes TDC1877 4-9, 11-16, 18-23 ",
    "FASTBUS crate. Space Frame Level1.   Bargraph X=2 \n Includes TDC1877 4-9, 11-16, 18-23 ",
    "FASTBUS crate. Space Frame Level1.   Bargraph X=3 \n Includes TDC1877 3-9,(3-l2TDC) 11-16, 18-23 ",
    "FASTBUS crate. Space Frame Level3.   Bargraph X=4 \n Includes TDC1877 4-9, 11-16, 18-23 ", 
    "FASTBUS crate. Space Frame Level0.   Bargraph X=5 \n Includes TDC1877 3-6", 
    "not used",
    "not used",
    "FASTBUS crate. South Clamshell .   Bargraph X=8 \n Includes ADC1881M  5-8,20-23, TDC1875 10-13,15-18",  
    "FASTBUS crate. Space Frame Level1.   Bargraph X=9 \n Includes TDC1877 4-9, 11-16, 18-23 ", 
    "FASTBUS crate. Space Frame Level3.   Bargraph X=10 \n Includes TDC1877 4-9, 11-16, 18-23 ", 
    "FASTBUS crate. Space Frame Level2.   Bargraph X=11 \n Includes TDC1877 4-9, 11-16, 18-23 ", 
    "FASTBUS crate. Space Frame Level1.   Bargraph X=12 \n Includes TDC1877 4-9, 11-16, 18-23 ", 
    "FASTBUS crate. Space Frame Level3.   Bargraph X=13 \n Includes TDC1877 4-9, 11-16, 18-23 ", 
    "FASTBUS crate. Space Frame Level2.   Bargraph X=14 \n Includes TDC1877 4-9, 11-16, 18-23 ", 
    "FASTBUS crate. Space Frame Level2.   Bargraph X=15 \n Includes TDC1877 4-9, 11-16, 18-23 ", 
    "VME crate. Space Frame Level1.   Bargraph X=16 \n Includes Moller polarimeter and beam helicity scalers",
    "FASTBUS crate. Forward Carriage Level1.   Bargraph X=17 \n Includes ADC1881M 3-7,21-25, TDC1872 8-17",
    "FASTBUS crate. Forward Carriage Level1.   Bargraph X=18 \n Includes ADC1881M 2-6, TDC1872 7-8,10,15,17,19-20",
    "FASTBUS crate. Forward Carriage Level1.   Bargraph X=19 \n Includes ADC1881M 4-14, TDC1872 15-25",
    "FASTBUS crate. Forward Carriage Level1.   Bargraph X=20 \n Includes ADC1881M 15-25, TDC1872 4-14",
    "FASTBUS crate. Forward Carriage Level2.   Bargraph X=21 \n Includes TDC1877 21-25",
    "not used",
    "not used",
    "not used",
    "not used",
    "not used",
    "not used",
    "not used",
    "VME crate. Forward Carriage Level1.   Bargraph X=29\n Includes CAEN scalers",
    "VME crate. Forward Carriage Level1.   Bargraph X=30\n Includes CAEN scalers",
    "VME crate. Forward Carriage Level1.   Bargraph X=31\n Includes CAEN scalers, and L1/L2 latches and MLU.",
    "VME crate. Forward Carriage Level1.   Bargraph X=32\n Includes CAEN scalers",
    "VME crate. Forward Carriage Level1.\n CAMAC branch driver.",
    "VME crate. South Clamshell.",
    "VME crate. Forward Carriage Level2.",
    "VXI crate. Forward Carriage Level1.\n Includes SC pretrigger discriminators",
    "VXI crate. Forward Carriage Level1.\n Includes SC pretrigger discriminators",
    "VME crate. Space Frame Level1. CAENET controller.",
    "VME crate. Space Frame Level1."
  };

  public static final String[] ts_csr_def = { 
    "GO",
    "PAUSE ON NEXT SYNC",
    "PAUSE AND SYNC", 
    "FORCE SYNC",
    "ENABLE SYNC",
    "ENABLE LEVEL1 HARDWARE",
    "ENABLE CLEAR TIMER",
    "USE FRONT BUSY TIMER",
    "USE CLEAR HOLD TIMER", 
    "ROC LOCK", 
    "ROC LOCK 4", 
    "OVERRIDE INHIBIT", 
    "HOLD STATE", 
    "TEST MODE", 
    " ", 
    " ", 
    "INHIBIT OCCURRED", 
    "LATE FAIL OCCURRED", 
    "SYNC OCCURRED",  
    "WRITE ACCESS ERROR OCCURRED", 
    "READ ACCESS ERROR OCCURRED", 
    "VME CYCLE ERROR OCCURRED",
    " ",
    " ",
    " ",
    " ",
    " ",
    " ",
    " ",
    " ",
    " ",
    " "
  };
  
  public static final String[] ts_state_def = {
    "LEVEL 1 ACCEPT",
    "LEVEL 2 START",
    "LEVEL 2 PASS",
    "LEVEL 2 FAIL", 
    "LEVEL 2 ACCEPT",
    "LEVEL 3 START",
    "LEVEL 3 PASS",
    "LEVEL 3 FAIL",
    "LEVEL 3 ACCEPT",
    "CLEAR",
    "FRONT END BUSY",
    "EXTERNAL INHIBIT",
    "LATCHED TRIGGER",
    "TS BUSY",
    "SEQUENCER ACTIVE",
    "READY",
    "ACTIVE",
    "SYNC",
    "BUFFERS EMPTY",
    "BUFFER FULL",
    "ROC B1 STROB",
    "ROC B1 AND_ACK",
    "ROC B1 OR_ACK",
    "ROC B2 STROB",
    "ROC B2 AND_ACK",
    "ROC B2 OR_ACK",
    "ROC B3 STROB",
    "ROC B3 AND_ACK",
    "ROC B3 OR_ACK",
    "ROC B4 STROB",
    "ROC B4 AND_ACK",
    "ROC B4 OR_ACK"
  };
  
  public static int auto_recover =1;

I 3
  //for online processes
  public static int[] CheckIfUndefined = new int[6];

E 3
  // in ns.
  public static final int ipc_sleep  = 5;
  public static final int roc_sleep  = 20000;
  public static final int ts_sleep   = 20000;
  public static final int eber_sleep = 20000;
  
  public static final int reboot_timer = 10;
  
  public static   String  ts_status;
  public static   String  ts_prescale;
  public static   String  ts_scaler;
  public static   long    ts_control;
  public static   long    ts_cstatus;
  public static   long    ts_state;
  public static   long    ts_rocenable;
D 5
  public static   boolean ts_update; 
E 5
I 5
  public static   boolean ts_update = false; 
E 5
  
  public static   String[] roc_status    = new String[39];
  public static   int[]    roc_numevt    = new int[32];
  public static   double[] roc_evtrate   = new double[32];
  public static   double[] roc_datarate  = new double[32];
  public static   int[]    roc_febuf     = new int[32];
  public static   boolean  roc_update; 

  public static   String  CODA_config;
  public static   String  eb_status;
  public static   double  eb_evtrate; 
  public static   double  eb_datarate;
  public static   int     eb_numevt;
  public static   int[]   ebbuf          = new int[32];

  public static   String  er_status;
  public static   double  er_evtrate; 
  public static   double  er_datarate;
  public static   int     er_numevt;
  public static   boolean eber_update; 
  
  public static   long[] ltrgs      = new long[64];
  public static   long[] ltrgs_diff = new long[64];
  public static   long[] ltrgs_prev = new long[64];
  public static   int[]  s1st ;
  public static   int[]  trgs ;
  
I 11
  public static String ts_help_text = "\nTrigger Supervisor (TS) is in off-line state.\n\nJUST CLICK DAQ_RECOVER_BUTTON ON DAQTLC.\n\nSit back and relax.Recovery procedure conducted\n by DAQTLC will take a few minutes.After you click \ndaq_recover button Runcontrol GUI, as well as EB, ER,\nET, L3 and RC terminals will disappear. Don't wary \nthey will be back shortly. The time that this message \nwindow will disappear the system will be ready again to take data.\nOn RunControl window click:\n\nConnect : Connect to CODA Run control server.\nConfigure : Select CODA configuration.\nDownload  : Download all DAQ software components.\nSelect appropriate Run-Configuration file using file chooser window.";

  public static String roc_help_text = "\nOne of the Readout Controllers (ROC) is disconnected.\n \nJUST CLICK DAQ_RECOVER_BUTTON ON DAQTLC.\n\nSit back and relax.Recovery procedure conducted\n by DAQTLC will take a few minutes.After you click \ndaq_recover button Runcontrol GUI, as well as EB, ER,\nET, L3 and RC terminals will disappear. Don't wary \nthey will be back shortly. The time that this message \nwindow will disappear the system will be ready again to take data.\nOn RunControl window click:\n\nConnect : Connect to CODA Run control server.\nConfigure : Select CODA configuration.\nDownload  : Download all DAQ software components.\nSelect appropriate Run-Configuration file using file chooser window.";

  public static String eber_help_text = "\nDAQTLC detected problem in one of the UNIX components.\n \nJUST CLICK DAQ_RECOVER_BUTTON ON DAQTLC.\n\nSit back and relax.Recovery procedure conducted\n by DAQTLC will take a few minutes.After you click \ndaq_recover button Runcontrol GUI, as well as EB, ER,\nET, L3 and RC terminals will disappear. Don't wary \nthey will be back shortly. The time that this message \nwindow will disappear the system will be ready again to take data.\nOn RunControl window click:\n\nConnect : Connect to CODA Run control server.\nConfigure : Select CODA configuration.\nDownload  : Download all DAQ software components.\nSelect appropriate Run-Configuration file using file chooser window.";

E 11
}






E 1
