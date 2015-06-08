import java.lang.*;
import java.io.*;
import java.util.*;
import clon.util.*;

/** Class TS is designed to collect Trigger supervisor  hardware information and perform specific harware/control actions. This is our first attempt of creating DAQ expert system. 
 * @author Vardan H Gyurjyan. 11.19.99 CLAS Online Group.
 */
public class Ts implements Runnable {
  
  private final static boolean append = true;
  private String status;
  private String prescale;
  private String scaler;
  private long control;
  private long cstatus;
  private long state;
  private long rocenable;
  
  private String temp_line;
  
  private int ts_skeep;

    private static String _clonlog;

    private static Jgetenv myJgetenv;
  
  public  SysExe mySysExe = new SysExe();
  
  
  
  public static void main(String argv[]) {
    new Thread ( new Ts() ).start();
	myJgetenv = new Jgetenv();
	try {
        _clonlog = myJgetenv.echo("CLON_LOG");
	} catch (JgetenvException e) {}
  }
  
  
  
  /**
   * Method is Intializing the instance variables 
   * from the DataPool.
   */
  public void clear_info () {
    status             = "UNDEFINED";
    prescale           = "NOINFO";
    scaler             = "NOINFO";
    control            = -1;
    cstatus            = -1;
    state           = -1;
    rocenable       = -1;
  }
  
  
  
  /** 
   * Method runs JavaDP and gets the status of the TS.
   * It will try once. In case of undefined unswere he will sleep
   * 3sec and try another time. If the status is steel undefined he will
   * return the status and will suggest to reboot clastrig2.
   * @param ts_skeep will count backwords to compensate the rebooting time.
   */
  public void status_info () {
    status = "UNDEFINED";
    status = mySysExe.run_vxworks("clastrig2", "status");
	//update status information
    	DataPool.ts_update = true; 
	DataPool.ts_status    = status;
    if(status.equals("UNDEFINED")) {
      //..............................sleep for 3sec. and check another time.
      try {
	Thread.sleep(3000);
      } catch (InterruptedException e) {
	System.out.println("interruped exception: " +e);
      }
      status = mySysExe.run_vxworks("clastrig2", "status");
      if(status.equals("UNDEINED")) {
	ts_skeep = ts_skeep - 1;
      }
    } else  ts_skeep = 0;
  } 
  
  
  
  
  /** 
   * Method will check the status variable and reboot the TS .
   * After rebooting it sets the timer which will prevent multiple rebooting.
   */
  public void reboot () {
    if(status.equals("UNDEFINED") && (ts_skeep <= 0)) { 
      //.............................audio alarm
      //      mySysExe.run_unix("door_bell");
      ts_skeep = DataPool.reboot_timer;
      //.............................rebooting
      System.out.println("Rebooting  clastrig2" );
      mySysExe.run_unix( "roc_reboot clastrig2");
      try {
	final PrintWriter writer = new PrintWriter(new FileWriter(_clonlog+"/daqtlc/log//ts.log", append));
	writer.println((new Date()) +" -> " + " TS was rebooted");
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
  
  
  
  /** 
   * Method  will read the TS hardware settings and update local
   * instance variables. 
   */
  public void info_collect () {
    
    if(status.equals("UNDEFINED")) {
      scaler = "NoInfo";
      prescale = "NoInfo";
      control = 777;
      cstatus = 777;
      state = 777;
      rocenable = 777;
    } else {
      
      temp_line = mySysExe.run_vxworks("clastrig2", "exec envShow");
      StringTokenizer line_token = new StringTokenizer(temp_line,"\n");
      while(line_token.hasMoreTokens() == true) {
	temp_line = line_token.nextToken();
	if(temp_line.equals("UNDEFINED")) return;
	else if(temp_line.indexOf("SCALER=") >=0) {
	  StringTokenizer tokens = new StringTokenizer(temp_line,"=");
	  while (tokens.hasMoreTokens() == true) 
	    scaler = tokens.nextToken();
	}
      }
      temp_line = mySysExe.run_vxworks("clastrig2", " exec ts_prescale");
      if(temp_line.equals("UNDEFINED")) return;
      else prescale = temp_line;
      temp_line = mySysExe.run_vxworks("clastrig2", "exec ts_control");
      if(temp_line.equals("UNDEFINED")) return;	  
      else control = Long.parseLong(temp_line,16);
      temp_line = mySysExe.run_vxworks("clastrig2", "exec ts_cstatus");
      if(temp_line.equals("UNDEFINED")) return;
      else cstatus = Long.parseLong(temp_line,16);
      temp_line = mySysExe.run_vxworks("clastrig2", "exec ts_state");
      if(temp_line.equals("UNDEFINED")) return;
      else state = Long.parseLong(temp_line,16);
      temp_line = mySysExe.run_vxworks("clastrig2", "exec ts_rocenable");
      if(temp_line.equals("UNDEFINED")) return;
      else rocenable = Long.parseLong(temp_line,16);
    }
  }
  
  
  
  /** 
   * Method will use local instance variables to update DataPool.
   */
  public void info_update () {
    DataPool.ts_status    = status;
    DataPool.ts_prescale  = prescale;
    DataPool.ts_scaler    = scaler;
    DataPool.ts_control   = control;
    DataPool.ts_cstatus   = cstatus;
    DataPool.ts_state     = state;
    DataPool.ts_rocenable = rocenable;
    DataPool.ts_update    = true;
              
	    System.out.println("TS information from the DataPool2\n");
	    System.out.println( "--------------------------\n");
	    System.out.println( "Status             - " + DataPool.ts_status);
	    System.out.println( "Prescale           - " + DataPool.ts_prescale);
	    System.out.println( "Scalers            - " + DataPool.ts_scaler);
	    System.out.println( "Control Register   - " + DataPool.ts_control);
	    System.out.println( "State Register     - " + DataPool.ts_state);
	    System.out.println( "RocEnable Register - " + DataPool.ts_rocenable);
	        
  }
  
  
  
  
  
  /** 
   * run method of the thread
   */
  public void run() {
    for(;;) {
      status_info();
      reboot();
      info_collect();
      info_update();
      try {
	Thread.sleep(DataPool.ts_sleep);
      } catch (InterruptedException e) {
	System.out.println("interruped exception: " +e);
      }
    }
  }
}




