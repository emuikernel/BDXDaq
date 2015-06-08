import java.lang.*;

/** Class DataPool is providing data structure for DaqTlc.
 @author Vardan H Gyurjyan. 11.19.99 CLAS Online Group.
 */
public class DataPool {

  //final variables
    //public static final int MaxProgress = 1000;
    //public static final int ProgressStep  = 50;

    public static int daq_recover_clicked = 0;

  public  static final String[] ts_rocbr_def = {
    "dc9",
    "dc3",
    "dc4",
    "dc10", 
    "not used", //"tage",
    "not used",
    "not used",
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
    "not used", //scaler4
    "not used", //scaler3
    "scaler1",
    "scaler2",
    "camac1",
    "camac2",
    "not used",
    "pretrig1",
    "pretrig2",
    "dccntrl",
    "not used"
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

  //for online processes
  public static int[] CheckIfUndefined = new int[6];

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
  public static   boolean ts_update = false; 
  
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
  
  public static String ts_help_text = "\nTrigger Supervisor (TS) is in off-line state.\n\nJUST CLICK DAQ_RECOVER_BUTTON ON DAQTLC.\n\nSit back and relax.Recovery procedure conducted\n by DAQTLC will take a few minutes.After you click \ndaq_recover button Runcontrol GUI, as well as EB, ER,\nET, L3 and RC terminals will disappear. Don't wary \nthey will be back shortly. The time that this message \nwindow will disappear the system will be ready again to take data.\nOn RunControl window click:\n\nConnect : Connect to CODA Run control server.\nConfigure : Select CODA configuration.\nDownload  : Download all DAQ software components.\nSelect appropriate Run-Configuration file using file chooser window.";

  public static String roc_help_text = "\nOne of the Readout Controllers (ROC) is disconnected.\n \nJUST CLICK DAQ_RECOVER_BUTTON ON DAQTLC.\n\nSit back and relax.Recovery procedure conducted\n by DAQTLC will take a few minutes.After you click \ndaq_recover button Runcontrol GUI, as well as EB, ER,\nET, L3 and RC terminals will disappear. Don't wary \nthey will be back shortly. The time that this message \nwindow will disappear the system will be ready again to take data.\nOn RunControl window click:\n\nConnect : Connect to CODA Run control server.\nConfigure : Select CODA configuration.\nDownload  : Download all DAQ software components.\nSelect appropriate Run-Configuration file using file chooser window.";

  public static String eber_help_text = "\nDAQTLC detected problem in one of the UNIX components.\n \nJUST CLICK DAQ_RECOVER_BUTTON ON DAQTLC.\n\nSit back and relax.Recovery procedure conducted\n by DAQTLC will take a few minutes.After you click \ndaq_recover button Runcontrol GUI, as well as EB, ER,\nET, L3 and RC terminals will disappear. Don't wary \nthey will be back shortly. The time that this message \nwindow will disappear the system will be ready again to take data.\nOn RunControl window click:\n\nConnect : Connect to CODA Run control server.\nConfigure : Select CODA configuration.\nDownload  : Download all DAQ software components.\nSelect appropriate Run-Configuration file using file chooser window.";

}






