h10201
s 00001/00001/00200
d D 1.14 03/04/17 17:04:21 boiarino 17 16
c minor
e
s 00000/00000/00201
d D 1.13 02/09/30 14:04:41 gurjyan 16 15
c *** empty log message ***
e
s 00000/00001/00201
d D 1.12 02/07/16 09:43:12 wolin 15 14
i 13
c Accepted child'\''s version in workspace "/usr/local/clas/devel/source".
c 
e
s 00003/00001/00198
d D 1.11 02/05/09 17:27:58 gurjyan 14 11
c *** empty log message ***
e
s 00002/00001/00199
d D 1.10.1.2 02/05/09 16:38:57 gurjyan 13 12
c *** empty log message ***
e
s 00001/00000/00199
d D 1.10.1.1 02/05/09 16:34:36 gurjyan 12 11
c *** empty log message ***
e
s 00001/00001/00198
d D 1.10 02/04/18 14:36:17 gurjyan 11 10
c typo
c 
e
s 00008/00001/00191
d D 1.9 02/04/18 14:27:48 gurjyan 10 9
c added Jgetenv
c 
e
s 00002/00003/00190
d D 1.8 01/11/08 10:22:26 gurjyan 9 8
c info_update
c 
e
s 00002/00002/00191
d D 1.7 01/10/15 15:18:20 gurjyan 8 7
c *** empty log message ***
e
s 00070/00072/00123
d D 1.6 00/10/17 13:51:08 gurjyan 7 6
c 
e
s 00002/00001/00193
d D 1.5 00/10/16 15:55:38 gurjyan 6 5
c 
e
s 00003/00002/00191
d D 1.4 00/10/16 15:36:42 gurjyan 5 4
c added roc_skeep[i] = DataPool.reboot_timer; in the main status_info loop
c 
e
s 00001/00001/00192
d D 1.3 00/10/04 09:28:16 gurjyan 4 3
c 
e
s 00001/00001/00192
d D 1.2 00/08/04 16:51:29 gurjyan 3 1
c add env variable
c 
e
s 00000/00000/00000
d R 1.2 70/01/01 00:00:02 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 daqtlc/Roc.java
e
s 00193/00000/00000
d D 1.1 00/07/31 14:48:43 gurjyan 1 0
c date and time created 00/07/31 14:48:43 by gurjyan
e
u
U
f e 0
t
T
I 1
import java.lang.*;
import java.io.*;
import java.util.*;
I 12
import clon.util.*;
E 12
I 14
import clon.util.*;
E 14

/** Class Roc is designed to collect Readout Controller  hardware information and perform specific harware/control actions. This is our first attempt of creating DAQ expert system. 
 * @author Vardan H Gyurjyan. 11.19.99 CLAS Online Group.
 */
public class Roc implements Runnable {
  
  private final static boolean append = true;

  private  String[] status    = new String[39];
  private   int[]    numevt    = new int[32];
  private   double[] evtrate   = new double[32];
  private   double[] datarate  = new double[32];

  private String temp_line;
  
  private int[] roc_skeep = new int[39];
I 10

    private static String _clonlog;

    private static Jgetenv myJgetenv;
E 10
  
  public  SysExe mySysExe = new SysExe();
  
  
  
  public static void main(String argv[]) {
    new Thread ( new Roc() ).start();
I 10
	myJgetenv = new Jgetenv();
I 13
	try {
E 13
I 14
D 15
	try {
E 15
E 14
        _clonlog = myJgetenv.echo("CLON_LOG");
D 13
D 14

E 13
I 13
	} catch (JgetenvException e ) {}
E 13
E 14
I 14
	} catch (JgetenvException e ) {}
E 14
E 10
  }
  
  
  /**
   * Method is Intializing the instance variables 
   * from the DataPool.
   */
  public void clear_info () {

      for (int i=0; i<=38; i++) {
	if(DataPool.ts_rocbr_def[i].equals("not used"));
	else {
	  status[i] = "UNDEFINED";
	}
      }
  }
  
  
  
  /** 
   * Method runs JavaDP and gets the status of the ROC.
   * It will try once. In case of undefined unswere he will sleep
   * 3sec and try another time. If the status is steel undefined he will
   * return the status and will suggest to rebootthat particular ROC.
   * @param roc_skeep will count backwords to compensate the rebooting time.
   */
  public void status_info () {

    for(int i=0; i<=38;i++) {
D 7
            System.out.println(DataPool.ts_rocbr_def[i] +" " + i);
E 7
I 7
      System.out.println(DataPool.ts_rocbr_def[i] +" " + i);
E 7
      if(DataPool.ts_rocbr_def[i].equals("not used"));
      else {
	status[i] = "UNDEFINED";
I 5
D 7
	roc_skeep[i] = DataPool.reboot_timer;
E 7
E 5
	status[i] = mySysExe.run_vxworks(DataPool.ts_rocbr_def[i], "status");
D 9
	//update status information
D 8
	//	DataPool.roc_update = true; 
	//	DataPool.roc_status[i] = status[i];
E 8
I 8
		DataPool.roc_update = true; 
		DataPool.roc_status[i] = status[i];
E 9
E 8
	if(status[i].equals("UNDEFINED")) {
D 7

E 7
I 7
	  System.out.println("debug: " + "Sleeping for 3sec.");
E 7
	  //..............................sleep for 3sec. and check another time.
D 7
	  System.out.println("debug: " + i);
E 7
I 7

E 7
	  try {
	    Thread.sleep(3000);
	  } catch (InterruptedException e) {
	    System.out.println("interruped exception: " +e);
	  }
	  status[i] = mySysExe.run_vxworks(DataPool.ts_rocbr_def[i], "status");
	  System.out.println(DataPool.ts_rocbr_def[i] + " ====> " +status[i]);
	  if(status[i].equals("UNDEFINED")) {
I 9
	//update status information
E 9
D 6
	    roc_skeep[i] = roc_skeep[i] - 1;
E 6
I 6
	    System.out.println(roc_skeep[i]);
D 7
	    //  roc_skeep[i] = roc_skeep[i] - 1;
E 7
I 7
	    roc_skeep[i] = roc_skeep[i] - 1;
E 7
E 6
	  }
	} else  roc_skeep[i] = 0;
      }
    } 
  }
  
  
  
  /** 
   * Method will check the status variable and reboot the particular ROC .
   * After rebooting it sets the timer which will prevent multiple rebooting.
   */
D 7
    public void reboot () {
      for(int i=0; i<=38;i++) {
	if(DataPool.ts_rocbr_def[i].equals("not used"));
	else {

	  if(status[i].equals("UNDEFINED") && (roc_skeep[i] <= 0)) { 
System.out.println("Rebooting " +DataPool.ts_rocbr_def[i]+ " --->  " +status[i] + roc_skeep[i]);


D 5
	    roc_skeep[i] = DataPool.reboot_timer;
	    //.............................rebooting
E 5
I 5
             // roc_skeep[i] = DataPool.reboot_timer;
             //.............................rebooting
E 5
	    mySysExe.run_unix( "roc_reboot  " + DataPool.ts_rocbr_def[i]);
	    //.............................sleep after rebooting to prevent network traffic
	try {
D 3
	  final PrintWriter writer = new PrintWriter(new FileWriter("/usr/local/clas/parms/daqtlc/log/roc.log", append));
E 3
I 3
D 4
	  final PrintWriter writer = new PrintWriter(new FileWriter("$CLON_PARMS/daqtlc/log/roc.log", append));
E 4
I 4
	  final PrintWriter writer = new PrintWriter(new FileWriter("/usr/local/clas/parms/daqtlc/log/roc.log", append));
E 4
E 3
	  writer.println((new Date()) +" -> " + DataPool.ts_rocbr_def[i] +" was rebooted");
	  writer.close(); 
	} catch (final IOException e) {
	  System.err.println(e);
	}
	    try {
	      Thread.sleep(30000);
	    } catch (InterruptedException e) {
	      System.out.println("interruped exception: " +e);
	    }
E 7
I 7
  public void reboot () {
    for(int i=0; i<=38;i++) {
      if(DataPool.ts_rocbr_def[i].equals("not used"));
      else {
	
	if(status[i].equals("UNDEFINED") && (roc_skeep[i] <= 0)) { 
	  System.out.println("Rebooting " +DataPool.ts_rocbr_def[i]+ " --->  " +status[i] + roc_skeep[i]);
	  
	  roc_skeep[i] = DataPool.reboot_timer;
	  //.............................rebooting
	  mySysExe.run_unix( "roc_reboot  " + DataPool.ts_rocbr_def[i]);
	  //.............................sleep after rebooting to prevent network traffic
	  try {
D 10
	    final PrintWriter writer = new PrintWriter(new FileWriter("/usr/local/clas/parms/daqtlc/log/roc.log", append));
E 10
I 10
D 11
	    final PrintWriter writer = new PrintWriter(new FileWriter(_clonlog+"/roc.log", append));
E 11
I 11
	    final PrintWriter writer = new PrintWriter(new FileWriter(_clonlog+"/daqtlc/log/roc.log", append));
E 11
E 10
	    writer.println((new Date()) +" -> " + DataPool.ts_rocbr_def[i] +" was rebooted");
	    writer.close(); 
	  } catch (final IOException e) {
	    System.err.println(e);
E 7
	  }
D 7
	} 
      }
E 7
I 7
	  try {
	    Thread.sleep(30000);
	  } catch (InterruptedException e) {
	    System.out.println("interruped exception: " +e);
	  }
	}
      } 
E 7
    }
D 7
      
      
E 7
I 7
  }
  
  
E 7
  /** 
   * Method  will read the ROC hardware settings and update local
   * instance variables. 
   */
D 7
      public void info_collect () {
	for(int i=0; i<=20;i++) {
	  if(DataPool.ts_rocbr_def[i].equals("not used"));
	  else {
	    if(status.equals("UNDEFINED")) {
	      numevt[i] = 777;
	      evtrate[i] = 777.7;
	      datarate[i] = 777.7;
	    } else {
	      
	      temp_line = mySysExe.run_vxworks(DataPool.ts_rocbr_def[i], " statistics");	    
	      if ( temp_line.equals("UNDEFINED")) continue;
	      StringTokenizer tokens2 = new StringTokenizer(temp_line);
	      numevt[i] = Integer.parseInt(tokens2.nextToken());
	      evtrate[i] = Double.valueOf(tokens2.nextToken()).doubleValue();
	      tokens2.nextToken();
	      datarate[i] = Double.valueOf(tokens2.nextToken()).doubleValue();
	    }
	  }
E 7
I 7
  public void info_collect () {
    for(int i=0; i<=20;i++) {
      if(DataPool.ts_rocbr_def[i].equals("not used"));
      else {
	if(status.equals("UNDEFINED")) {
	  numevt[i] = 777;
	  evtrate[i] = 777.7;
	  datarate[i] = 777.7;
	} else {
	  
D 17
	  temp_line = mySysExe.run_vxworks(DataPool.ts_rocbr_def[i], " statistics");	    
E 17
I 17
	  temp_line = mySysExe.run_vxworks(DataPool.ts_rocbr_def[i]," statistics");
E 17
	  if ( temp_line.equals("UNDEFINED")) continue;
	  StringTokenizer tokens2 = new StringTokenizer(temp_line);
	  numevt[i] = Integer.parseInt(tokens2.nextToken());
	  evtrate[i] = Double.valueOf(tokens2.nextToken()).doubleValue();
	  tokens2.nextToken();
	  datarate[i] = Double.valueOf(tokens2.nextToken()).doubleValue();
E 7
	}
      }
D 7
        
E 7
I 7
    }
  }
E 7
  
I 7
  
E 7
  /** 
   * Method will use local instance variables to update DataPool.
   */
D 7
      public void info_update () {
	DataPool.roc_update = true; 
	for(int i=0; i<=38;i++) {
	  if(DataPool.ts_rocbr_def[i].equals("not used"));
	  else {
	    DataPool.roc_status[i] = status[i];
	    //System.out.println(DataPool.ts_rocbr_def[i]+ " status         -  " +DataPool.roc_status[i]);
	    if( i<=20) {
	      DataPool.roc_numevt[i] = numevt[i];
	      DataPool.roc_evtrate[i] = evtrate[i];
	      DataPool.roc_datarate[i] = datarate[i];
	      //System.out.println(DataPool.ts_rocbr_def[i]+ " numevts     -  " +DataPool.roc_numevt[i]);
	      //System.out.println(DataPool.ts_rocbr_def[i]+ " evtrate        -  " +DataPool.roc_evtrate[i]);
	      //System.out.println(DataPool.ts_rocbr_def[i]+ " datarate     -  " +DataPool.roc_datarate[i]);
	    }
	  }
E 7
I 7
  public void info_update () {
    DataPool.roc_update = true; 
    for(int i=0; i<=38;i++) {
      if(DataPool.ts_rocbr_def[i].equals("not used"));
      else {
	DataPool.roc_status[i] = status[i];
	//System.out.println(DataPool.ts_rocbr_def[i]+ " status         -  " +DataPool.roc_status[i]);
	if( i<=20) {
	  DataPool.roc_numevt[i] = numevt[i];
	  DataPool.roc_evtrate[i] = evtrate[i];
	  DataPool.roc_datarate[i] = datarate[i];
	  //System.out.println(DataPool.ts_rocbr_def[i]+ " numevts     -  " +DataPool.roc_numevt[i]);
	  //System.out.println(DataPool.ts_rocbr_def[i]+ " evtrate        -  " +DataPool.roc_evtrate[i]);
	  //System.out.println(DataPool.ts_rocbr_def[i]+ " datarate     -  " +DataPool.roc_datarate[i]);
E 7
	}
      }
D 7
      
E 7
I 7
    }
  }
E 7
  
  
  
I 7
  
E 7
  /** 
   * run method of the thread
   */
  public void run() {
    for(;;) {
D 7

E 7
I 7
      
E 7
      status_info();
I 9
      info_update();
E 9
      reboot();
      info_collect();
      info_update();
      try {
	Thread.sleep(DataPool.roc_sleep);
      } catch (InterruptedException e) {
	System.out.println("interruped exception: " +e);
      }
    }
  }
}




E 1
