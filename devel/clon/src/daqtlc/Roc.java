import java.lang.*;
import java.io.*;
import java.util.*;
import clon.util.*;

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

    private static String _clonlog;

    private static Jgetenv myJgetenv;
  
  public  SysExe mySysExe = new SysExe();
  
  
  
  public static void main(String argv[]) {
    new Thread ( new Roc() ).start();
	myJgetenv = new Jgetenv();
	try {
        _clonlog = myJgetenv.echo("CLON_LOG");
	} catch (JgetenvException e ) {}
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
      System.out.println(DataPool.ts_rocbr_def[i] +" " + i);
      if(DataPool.ts_rocbr_def[i].equals("not used"));
      else {
	status[i] = "UNDEFINED";
	status[i] = mySysExe.run_vxworks(DataPool.ts_rocbr_def[i], "status");
	if(status[i].equals("UNDEFINED")) {
	  System.out.println("debug: " + "Sleeping for 3sec.");
	  //..............................sleep for 3sec. and check another time.

	  try {
	    Thread.sleep(3000);
	  } catch (InterruptedException e) {
	    System.out.println("interruped exception: " +e);
	  }
	  status[i] = mySysExe.run_vxworks(DataPool.ts_rocbr_def[i], "status");
	  System.out.println(DataPool.ts_rocbr_def[i] + " ====> " +status[i]);
	  if(status[i].equals("UNDEFINED")) {
	//update status information
	    System.out.println(roc_skeep[i]);
	    roc_skeep[i] = roc_skeep[i] - 1;
	  }
	} else  roc_skeep[i] = 0;
      }
    } 
  }
  
  
  
  /** 
   * Method will check the status variable and reboot the particular ROC .
   * After rebooting it sets the timer which will prevent multiple rebooting.
   */
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
	    final PrintWriter writer = new PrintWriter(new FileWriter(_clonlog+"/daqtlc/log/roc.log", append));
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
	}
      } 
    }
  }
  
  
  /** 
   * Method  will read the ROC hardware settings and update local
   * instance variables. 
   */
  public void info_collect () {
    for(int i=0; i<=20;i++) {
      if(DataPool.ts_rocbr_def[i].equals("not used"));
      else {
	if(status.equals("UNDEFINED")) {
	  numevt[i] = 777;
	  evtrate[i] = 777.7;
	  datarate[i] = 777.7;
	} else {
	  
	  temp_line = mySysExe.run_vxworks(DataPool.ts_rocbr_def[i]," statistics");
	  if ( temp_line.equals("UNDEFINED")) continue;
	  StringTokenizer tokens2 = new StringTokenizer(temp_line);
	  numevt[i] = Integer.parseInt(tokens2.nextToken());
	  evtrate[i] = Double.valueOf(tokens2.nextToken()).doubleValue();
	  tokens2.nextToken();
	  datarate[i] = Double.valueOf(tokens2.nextToken()).doubleValue();
	}
      }
    }
  }
  
  
  /** 
   * Method will use local instance variables to update DataPool.
   */
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
    }
  }
  
  
  
  
  /** 
   * run method of the thread
   */
  public void run() {
    for(;;) {
      
      status_info();
      info_update();
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




