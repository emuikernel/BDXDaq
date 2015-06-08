h17909
s 00007/00007/00204
d D 1.11 03/04/17 17:04:42 boiarino 14 13
c minor
e
s 00000/00001/00211
d D 1.10 02/07/16 09:43:14 wolin 13 12
i 11
c Accepted child'\''s version in workspace "/usr/local/clas/devel/source".
c 
e
s 00003/00001/00208
d D 1.9 02/05/09 17:27:45 gurjyan 12 9
c *** empty log message ***
e
s 00002/00001/00209
d D 1.8.1.2 02/05/09 16:39:27 gurjyan 11 10
c *** empty log message ***
e
s 00001/00000/00209
d D 1.8.1.1 02/05/09 16:34:18 gurjyan 10 9
c *** empty log message ***
e
s 00001/00001/00208
d D 1.8 02/04/18 14:37:04 gurjyan 9 8
c typo
c 
e
s 00008/00001/00201
d D 1.7 02/04/18 14:28:50 gurjyan 8 7
c added Jgetenv
c 
e
s 00001/00001/00201
d D 1.6 01/10/19 13:25:13 gurjyan 7 6
c test
e
s 00001/00001/00201
d D 1.5 00/10/18 12:44:43 gurjyan 6 5
c 
e
s 00001/00001/00201
d D 1.4 00/10/04 09:28:08 gurjyan 5 4
c 
e
s 00001/00001/00201
d D 1.3 00/09/05 16:38:21 gurjyan 4 3
c 
e
s 00001/00001/00201
d D 1.2 00/08/04 16:51:55 gurjyan 3 1
c add env variables 
c 
e
s 00000/00000/00000
d R 1.2 70/01/01 00:00:02 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 daqtlc/Ts.java
e
s 00202/00000/00000
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
I 10
import clon.util.*;
E 10
I 12
import clon.util.*;
E 12

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
I 8

    private static String _clonlog;

    private static Jgetenv myJgetenv;
E 8
  
  public  SysExe mySysExe = new SysExe();
  
  
  
  public static void main(String argv[]) {
    new Thread ( new Ts() ).start();
I 8
	myJgetenv = new Jgetenv();
I 11
	try {
E 11
I 12
D 13
	try {
E 13
E 12
        _clonlog = myJgetenv.echo("CLON_LOG");
D 11
D 12

E 11
I 11
	} catch (JgetenvException e) {}
E 11
E 12
I 12
	} catch (JgetenvException e) {}
E 12
E 8
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
D 7
    //	DataPool.ts_update = true; 
E 7
I 7
    	DataPool.ts_update = true; 
E 7
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
D 3
	final PrintWriter writer = new PrintWriter(new FileWriter("/usr/local/clas/parms/daqtlc/log/ts.log", append));
E 3
I 3
D 5
	final PrintWriter writer = new PrintWriter(new FileWriter("$CLON_PARMS/daqtlc/log/ts.log", append));
E 5
I 5
D 8
	final PrintWriter writer = new PrintWriter(new FileWriter("/usr/local/clas/parms/daqtlc/log/ts.log", append));
E 8
I 8
D 9
	final PrintWriter writer = new PrintWriter(new FileWriter(_clonlog+"/ts.log", append));
E 9
I 9
	final PrintWriter writer = new PrintWriter(new FileWriter(_clonlog+"/daqtlc/log//ts.log", append));
E 9
E 8
E 5
E 3
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
D 4
	else if(temp_line.equals("61: SCALER=TRUE")) {
E 4
I 4
D 6
	else if(temp_line.equals("62: SCALER=TRUE")) {
E 6
I 6
	else if(temp_line.indexOf("SCALER=") >=0) {
E 6
E 4
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
              
D 14
	    System.out.println("TS information from the DataPool \n");
E 14
I 14
	    System.out.println("TS information from the DataPool2\n");
E 14
	    System.out.println( "--------------------------\n");
D 14
	    System.out.println( "Status                           - " + DataPool.ts_status);
	    System.out.println( "Prescale                       - " + DataPool.ts_prescale);
	    System.out.println( "Scalers                          - " + DataPool.ts_scaler);
	    System.out.println( "Control Register       - " + DataPool.ts_control);
	    System.out.println( "State Register            - " + DataPool.ts_state);
	    System.out.println( "RocEnable Register - " + DataPool.ts_control);
E 14
I 14
	    System.out.println( "Status             - " + DataPool.ts_status);
	    System.out.println( "Prescale           - " + DataPool.ts_prescale);
	    System.out.println( "Scalers            - " + DataPool.ts_scaler);
	    System.out.println( "Control Register   - " + DataPool.ts_control);
	    System.out.println( "State Register     - " + DataPool.ts_state);
	    System.out.println( "RocEnable Register - " + DataPool.ts_rocenable);
E 14
	        
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




E 1
