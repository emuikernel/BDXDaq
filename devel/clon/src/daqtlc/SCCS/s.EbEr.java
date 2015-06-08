h18098
s 00000/00001/00254
d D 1.12 02/07/16 09:43:07 wolin 15 14
i 13
c Accepted child'\''s version in workspace "/usr/local/clas/devel/source".
c 
e
s 00003/00001/00251
d D 1.11 02/05/09 17:27:37 gurjyan 14 11
c *** empty log message ***
e
s 00002/00001/00252
d D 1.10.1.2 02/05/09 16:38:24 gurjyan 13 12
c *** empty log message ***
e
s 00001/00000/00252
d D 1.10.1.1 02/05/09 16:33:56 gurjyan 12 11
c *** empty log message ***
e
s 00002/00002/00250
d D 1.10 02/04/18 14:36:44 gurjyan 11 10
c typo
c 
e
s 00010/00003/00242
d D 1.9 02/04/18 14:25:55 gurjyan 10 9
c added Jgetenv
c 
e
s 00002/00002/00243
d D 1.8 01/10/19 13:25:12 gurjyan 9 8
c test
e
s 00016/00016/00229
d D 1.7 01/08/20 14:18:54 gurjyan 8 7
c undefined state 0.0 instead of 777.7
c 
e
s 00002/00002/00243
d D 1.6 01/08/20 14:01:30 gurjyan 7 6
c typo
c 
e
s 00218/00214/00027
d D 1.5 01/08/20 13:59:29 gurjyan 6 5
c catch NoSuchElementException
c 
e
s 00026/00023/00215
d D 1.4 00/10/16 10:10:36 gurjyan 5 4
c new algorithm to monitor EB
c 
e
s 00001/00001/00237
d D 1.3 00/10/04 14:52:20 gurjyan 4 3
c remove env variable setr
c 
e
s 00002/00002/00236
d D 1.2 00/08/04 16:50:29 gurjyan 3 1
c add env variables
c 
e
s 00000/00000/00000
d R 1.2 70/01/01 00:00:02 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 daqtlc/EbEr.java
e
s 00238/00000/00000
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
import java.io.*;
import java.util.*;
I 12
import clon.util.*;
E 12
I 14
import clon.util.*;
E 14

/** Class EbEr is designed to collect CODA Unix process information. 
 * This is our first attempt of creating DAQ expert system. 
 * @author Vardan H Gyurjyan. 11.19.99 CLAS Online Group.
 */
public class EbEr implements Runnable {
D 6
  
  private final static boolean append = true;
  private String CODA_config;
  private String eb_status;
  private double eb_evtrate;
  private double eb_datarate;
  private int eb_numevt;
D 5

E 5
I 5
  
E 5
  private String er_status;
  private double er_evtrate;
  private double er_datarate;
  private int er_numevt;
  private int ebbuf [] = new int[32];
  
  private String temp_line;
  private String work_line;
  private String work_str;
  
  private int ts_skeep;
  
  public  SysExe mySysExe = new SysExe();
  
  
  
  public static void main(String argv[]) {
    new Thread ( new EbEr() ).start();
  }
  
  
  
  /**
   * Method is Intializing the instance variables 
   * from the DataPool.
   */
  public void clear_info () {
    eb_status             = "UNDEFINED";
    er_status             = "UNDEFINED";
  }
  
  
  
  /** 
   * Method runs JavaDP and gets the status of the EB1 and ER1.
   * It will try once. In case of undefined unswere he will sleep
   * 7sec and try another time. If the status is steel undefined he will
   * return the status.
   */
  public void status_info () {
    eb_status = "UNDEFINED";
    eb_status = mySysExe.run_vxworks("EB1", "status");
    //    DataPool.eber_update = true;
    //    DataPool.eb_status   = eb_status;
    if(eb_status.equals("UNDEFINED")) {
      //..............................sleep for 7sec. and check another time.
      try {
	Thread.sleep(7000);
      } catch (InterruptedException e) {
	System.out.println("interruped exception: " +e);
      }
      eb_status = mySysExe.run_vxworks("EB1", "status");
E 6
I 6
    
    private final static boolean append = true;
    private String CODA_config;
    private String eb_status;
    private double eb_evtrate;
    private double eb_datarate;
    private int eb_numevt;
    
    private String er_status;
    private double er_evtrate;
    private double er_datarate;
    private int er_numevt;
    private int ebbuf [] = new int[32];
    
    private String temp_line;
    private String work_line;
    private String work_str;
    
    private int ts_skeep;
D 10
    
E 10
I 10

    private static String _clonlog;

    private static Jgetenv myJgetenv;

E 10
    public  SysExe mySysExe = new SysExe();
    
    
    
    public static void main(String argv[]) {
	new Thread ( new EbEr() ).start();
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
	} catch (JgetenvException e) {}
E 13
E 14
I 14
	} catch (JgetenvException e) {}
E 14
E 10
E 6
    }
    
D 6
    er_status = "UNDEFINED";
    er_status = mySysExe.run_vxworks("ER1", "status");
    //    DataPool.eber_update = true;
    //    DataPool.eb_status   = eb_status;
    if(er_status.equals("UNDEFINED")) {
      //..............................sleep for 7sec. and check another time.
      try {
	Thread.sleep(7000);
      } catch (InterruptedException e) {
	System.out.println("interruped exception: " +e);
      }
      er_status = mySysExe.run_vxworks("ER1", "status");
E 6
I 6
    
    
    /**
     * Method is Intializing the instance variables 
     * from the DataPool.
     */
    public void clear_info () {
	eb_status             = "UNDEFINED";
	er_status             = "UNDEFINED";
E 6
    }
D 6
  } 
  
  
  
  
  /** 
   * Method  will read the EB1 and ER1 information and update local
   * instance variables. 
   */
  public void info_collect () {
    //..........................EB statistics    
    if(eb_status.equals("UNDEFINED")) {
      CODA_config = "unknown";
      eb_evtrate = 777.7;
      eb_datarate = 777.7;
      eb_numevt = 777;
      try {
D 3
	final PrintWriter writer = new PrintWriter(new FileWriter("/usr/local/clas/parms/daqtlc/log/eb.log", append));
E 3
I 3
D 4
	final PrintWriter writer = new PrintWriter(new FileWriter("$CLON_PARMS/daqtlc/log/eb.log", append));
E 4
I 4
	final PrintWriter writer = new PrintWriter(new FileWriter("/usr/local/clas/parms/daqtlc/log/eb.log", append));
E 4
E 3
	writer.println((new Date()) +" -> " + " EB crashed");
	writer.close(); 
      } catch (final IOException e) {
	System.err.println(e);
      }
      
    } else {
      CODA_config = mySysExe.run_vxworks("EB1","EB1 cget -config");
      temp_line = mySysExe.run_vxworks("EB1", "statistics");
      if(temp_line.equals("UNDEFINED")) {
	CODA_config = "unknown";
	eb_evtrate = 777.7;
	eb_datarate = 777.7;
	eb_numevt = 777;
      } else {
	StringTokenizer line_token = new StringTokenizer(temp_line);
	while(line_token.hasMoreTokens() == true) {
	  eb_numevt = Integer.parseInt(line_token.nextToken());
	  eb_evtrate = (Double.valueOf(line_token.nextToken()).doubleValue());
	  line_token.nextToken();
	  eb_datarate = (Double.valueOf(line_token.nextToken()).doubleValue())/1000.0;
E 6
I 6
    
    
    
    /** 
     * Method runs JavaDP and gets the status of the EB1 and ER1.
     * It will try once. In case of undefined unswere he will sleep
     * 7sec and try another time. If the status is steel undefined he will
     * return the status.
     */
    public void status_info () {
	eb_status = "UNDEFINED";
	eb_status = mySysExe.run_vxworks("EB1", "status");
D 9
	//    DataPool.eber_update = true;
	//    DataPool.eb_status   = eb_status;
E 9
I 9
	    DataPool.eber_update = true;
	    DataPool.eb_status   = eb_status;
E 9
	if(eb_status.equals("UNDEFINED")) {
	    //..............................sleep for 7sec. and check another time.
	    try {
		Thread.sleep(7000);
	    } catch (InterruptedException e) {
		System.out.println("interruped exception: " +e);
	    }
	    eb_status = mySysExe.run_vxworks("EB1", "status");
E 6
	}
D 6
      }
D 5

E 5
      //EB buffer occupancy monitoring
      temp_line = mySysExe.run_vxworks("EB1", "frag_sized");
D 5
      for (int i=0; i<32; i++) {
	if(temp_line.indexOf(DataPool.ts_rocbr_def[i]) >=0) {
	  work_line = mySysExe.run_vxworks("EB1",DataPool.ts_rocbr_def[i] + " fifo_stats");
	  StringTokenizer line_token = new StringTokenizer(work_line);     
	  while (line_token.hasMoreTokens() == true) {
	    work_str = line_token.nextToken();
	    work_str = line_token.nextToken();
	    work_str = line_token.nextToken();
	    work_str = line_token.nextToken();
	    work_str = line_token.nextToken();
	    ebbuf[i] = Integer.parseInt(line_token.nextToken());
E 5
I 5
      if(temp_line.equals("UNDEFINED")) {
	CODA_config = "unknown";
	eb_evtrate = 777.7;
	eb_datarate = 777.7;
	eb_numevt = 777;
      } else {
	for (int i=0; i<32; i++) {
	  if(temp_line.indexOf(DataPool.ts_rocbr_def[i]) >=0) {
	    work_line = mySysExe.run_vxworks("EB1",DataPool.ts_rocbr_def[i] + " fifo_stats");
	    StringTokenizer line_token = new StringTokenizer(work_line);     
	    while (line_token.hasMoreTokens() == true) {
	      work_str = line_token.nextToken();
	      work_str = line_token.nextToken();
	      work_str = line_token.nextToken();
	      work_str = line_token.nextToken();
	      work_str = line_token.nextToken();
	      ebbuf[i] = Integer.parseInt(line_token.nextToken());
E 6
I 6
	
	er_status = "UNDEFINED";
	er_status = mySysExe.run_vxworks("ER1", "status");
	//    DataPool.eber_update = true;
	//    DataPool.eb_status   = eb_status;
	if(er_status.equals("UNDEFINED")) {
	    //..............................sleep for 7sec. and check another time.
	    try {
		Thread.sleep(7000);
	    } catch (InterruptedException e) {
		System.out.println("interruped exception: " +e);
E 6
	    }
E 5
D 6
	  }
E 6
I 6
	    er_status = mySysExe.run_vxworks("ER1", "status");
E 6
	}
D 5
      }
      
      
E 5
I 5
D 6
      }     
E 5
    }
E 6
I 6
    } 
E 6
I 5
    
E 5
D 6
    //..........................ER statistics
    if(er_status.equals("UNDEFINED")) {
      er_evtrate = 777.7;
      er_datarate = 777.7;
      er_numevt = 777;
      try {
D 3
	final PrintWriter writer = new PrintWriter(new FileWriter("/usr/local/clas/parms/daqtlc/log/er.log", append));
E 3
I 3
	final PrintWriter writer = new PrintWriter(new FileWriter("$CLON_PARMS/daqtlc/log/er.log", append));
E 3
	writer.println((new Date()) +" -> " + " ER crashed");
	writer.close(); 
      } catch (final IOException e) {
	System.err.println(e);
      }
      
    } else {
      temp_line = mySysExe.run_vxworks("ER1", "statistics");
      if(temp_line.equals("UNDEFINED")) {
	CODA_config = "unknown";
	eb_evtrate = 777.7;
	eb_datarate = 777.7;
	eb_numevt = 777;
      } else {
	StringTokenizer line_token = new StringTokenizer(temp_line);
	while(line_token.hasMoreTokens() == true) {
	  er_numevt = Integer.parseInt(line_token.nextToken());
	  er_evtrate = (Double.valueOf(line_token.nextToken()).doubleValue());
	  line_token.nextToken();
	  er_datarate = (Double.valueOf(line_token.nextToken()).doubleValue())/1000.0;
E 6
I 6
    
    
    
    /** 
     * Method  will read the EB1 and ER1 information and update local
     * instance variables. 
     */
    public void info_collect () {
	//..........................EB statistics    
	if(eb_status.equals("UNDEFINED")) {
	    CODA_config = "unknown";
D 8
	    eb_evtrate = 777.7;
	    eb_datarate = 777.7;
	    eb_numevt = 777;
E 8
I 8
	    eb_evtrate = 0.0;
	    eb_datarate = 0.0;
	    eb_numevt = 0;
E 8
	    try {
D 10
		final PrintWriter writer = new PrintWriter(new FileWriter("/usr/local/clas/parms/daqtlc/log/eb.log", append));
E 10
I 10
D 11
		final PrintWriter writer = new PrintWriter(new FileWriter(_clonlog+"/eb.log", append));
E 11
I 11
		final PrintWriter writer = new PrintWriter(new FileWriter(_clonlog+"/daqtlc/log//eb.log", append));
E 11
E 10
		writer.println((new Date()) +" -> " + " EB crashed");
		writer.close(); 
	    } catch (final IOException e) {
		System.err.println(e);
	    }
	    
	} else {
	    CODA_config = mySysExe.run_vxworks("EB1","EB1 cget -config");
	    temp_line = mySysExe.run_vxworks("EB1", "statistics");
	    if(temp_line.equals("UNDEFINED")) {
		CODA_config = "unknown";
D 8
		eb_evtrate = 777.7;
		eb_datarate = 777.7;
		eb_numevt = 777;
E 8
I 8
		eb_evtrate = 0.0;
		eb_datarate = 0.0;
		eb_numevt = 0;
E 8
	    } else {
		StringTokenizer line_token = new StringTokenizer(temp_line);
		while(line_token.hasMoreTokens() == true) {
		    try {
			eb_numevt = Integer.parseInt(line_token.nextToken());
			eb_evtrate = (Double.valueOf(line_token.nextToken()).doubleValue());
			line_token.nextToken();
			eb_datarate = (Double.valueOf(line_token.nextToken()).doubleValue())/1000.0;
D 7
		    } catch (NoSuchElementExceptio a ) { break;} 
E 7
I 7
		    } catch (NoSuchElementException a ) { break;} 
E 7
		}
	    }
	    //EB buffer occupancy monitoring
	    temp_line = mySysExe.run_vxworks("EB1", "frag_sized");
	    if(temp_line.equals("UNDEFINED")) {
		CODA_config = "unknown";
D 8
		eb_evtrate = 777.7;
		eb_datarate = 777.7;
		eb_numevt = 777;
E 8
I 8
		eb_evtrate = 0.0;
		eb_datarate = 0.0;
		eb_numevt = 0;
E 8
	    } else {
		for (int i=0; i<32; i++) {
		    if(temp_line.indexOf(DataPool.ts_rocbr_def[i]) >=0) {
			work_line = mySysExe.run_vxworks("EB1",DataPool.ts_rocbr_def[i] + " fifo_stats");
			StringTokenizer line_token = new StringTokenizer(work_line);     
			while (line_token.hasMoreTokens() == true) {
			    try {
				work_str = line_token.nextToken();
				work_str = line_token.nextToken();
				work_str = line_token.nextToken();
				work_str = line_token.nextToken();
				work_str = line_token.nextToken();
				ebbuf[i] = Integer.parseInt(line_token.nextToken());
D 7
			    } catch (NoSuchElementExceptio a ) { break;} 
E 7
I 7
			    } catch (NoSuchElementException a ) { break;} 
E 7
			}
		    }
		}
	    }     
E 6
	}
D 6
      }
E 6
I 6
	
	//..........................ER statistics
	if(er_status.equals("UNDEFINED")) {
D 8
	    er_evtrate = 777.7;
	    er_datarate = 777.7;
	    er_numevt = 777;
E 8
I 8
	    er_evtrate = 0.0;
	    er_datarate = 0.0;
	    er_numevt = 0;
E 8
	    try {
D 8
		final PrintWriter writer = new PrintWriter(new FileWriter("$CLON_PARMS/daqtlc/log/er.log", append));
E 8
I 8
D 10
		final PrintWriter writer = new PrintWriter(new FileWriter("/usr/local/clas/parms/daqtlc/log/er.log", append));
E 10
I 10
D 11
		final PrintWriter writer = new PrintWriter(new FileWriter(_clonlog+"/er.log", append));
E 11
I 11
		final PrintWriter writer = new PrintWriter(new FileWriter(_clonlog+"/daqtlc/log//er.log", append));
E 11
E 10
E 8
		writer.println((new Date()) +" -> " + " ER crashed");
		writer.close(); 
	    } catch (final IOException e) {
		System.err.println(e);
	    }
	    
	} else {
	    temp_line = mySysExe.run_vxworks("ER1", "statistics");
	    if(temp_line.equals("UNDEFINED")) {
		CODA_config = "unknown";
D 8
		eb_evtrate = 777.7;
		eb_datarate = 777.7;
		eb_numevt = 777;
E 8
I 8
		eb_evtrate = 0.0;
		eb_datarate = 0.0;
		eb_numevt = 0;
E 8
	    } else {
		StringTokenizer line_token = new StringTokenizer(temp_line);
		while(line_token.hasMoreTokens() == true) {
		    er_numevt = Integer.parseInt(line_token.nextToken());
		    er_evtrate = (Double.valueOf(line_token.nextToken()).doubleValue());
		    line_token.nextToken();
		    er_datarate = (Double.valueOf(line_token.nextToken()).doubleValue())/1000.0;
		}
	    }
	}
E 6
    }
D 6
  }
  
  
D 5
  
  
E 5
  /** 
   * Method will use local instance variables to update DataPool.
   */
  public void info_update () {
    DataPool.CODA_config = CODA_config;
    DataPool.eb_status   = eb_status;
    DataPool.eb_evtrate  = eb_evtrate; 
    DataPool.eb_datarate = eb_datarate;
    DataPool.eb_numevt   = eb_numevt;
    DataPool.er_status   = er_status;
    DataPool.er_evtrate  = er_evtrate; 
    DataPool.er_datarate = er_datarate;
    DataPool.er_numevt   = er_numevt;
    DataPool.eber_update = true;
D 5
      for (int i=0; i<32; i++) 
	DataPool.ebbuf[i] = ebbuf[i];
        
E 5
I 5
    for (int i=0; i<32; i++) 
      DataPool.ebbuf[i] = ebbuf[i];
E 6
    
E 5
D 6
    System.out.println("EB");
    System.out.println("------");
    System.out.println(DataPool.CODA_config);
    System.out.println(DataPool.eb_status);
    System.out.println(DataPool.eb_evtrate);
    System.out.println(DataPool.eb_datarate);
    System.out.println(DataPool.eb_numevt);
D 5
      for (int i=0; i<32; i++) 
	System.out.println(DataPool.ebbuf[i]);
E 5
I 5
    for (int i=0; i<32; i++) 
      System.out.println(DataPool.ebbuf[i]);
E 6
E 5
    
D 6
    System.out.println("ER");
    System.out.println("------");
    System.out.println(DataPool.er_status);
    System.out.println(DataPool.er_evtrate);
    System.out.println(DataPool.er_datarate);
    System.out.println(DataPool.er_numevt);
E 6
I 6
    /** 
     * Method will use local instance variables to update DataPool.
     */
    public void info_update () {
	DataPool.CODA_config = CODA_config;
	DataPool.eb_status   = eb_status;
	DataPool.eb_evtrate  = eb_evtrate; 
	DataPool.eb_datarate = eb_datarate;
	DataPool.eb_numevt   = eb_numevt;
	DataPool.er_status   = er_status;
	DataPool.er_evtrate  = er_evtrate; 
	DataPool.er_datarate = er_datarate;
	DataPool.er_numevt   = er_numevt;
	DataPool.eber_update = true;
	for (int i=0; i<32; i++) 
	    DataPool.ebbuf[i] = ebbuf[i];
	
	System.out.println("EB");
	System.out.println("------");
	System.out.println(DataPool.CODA_config);
	System.out.println(DataPool.eb_status);
	System.out.println(DataPool.eb_evtrate);
	System.out.println(DataPool.eb_datarate);
	System.out.println(DataPool.eb_numevt);
	for (int i=0; i<32; i++) 
	    System.out.println(DataPool.ebbuf[i]);
	
	System.out.println("ER");
	System.out.println("------");
	System.out.println(DataPool.er_status);
	System.out.println(DataPool.er_evtrate);
	System.out.println(DataPool.er_datarate);
	System.out.println(DataPool.er_numevt);
	
    }
E 6
    
D 6
  }
  
  
  
  /** 
   * run method of the thread
   */
  public void run() {
    for(;;) {
      status_info();
      info_collect();
      info_update();
      try {
	Thread.sleep(DataPool.eber_sleep);
      } catch (InterruptedException e) {
	System.out.println("interruped exception: " +e);
      }
E 6
I 6
    
    
    /** 
     * run method of the thread
     */
    public void run() {
	for(;;) {
	    status_info();
	    info_collect();
	    info_update();
	    try {
		Thread.sleep(DataPool.eber_sleep);
	    } catch (InterruptedException e) {
		System.out.println("interruped exception: " +e);
	    }
	}
E 6
    }
D 6
  }
E 6
}




E 1
