import java.lang.*;
import java.io.*;
import java.util.*;
import clon.util.*;

/** Class EbEr is designed to collect CODA Unix process information. 
 * This is our first attempt of creating DAQ expert system. 
 * @author Vardan H Gyurjyan. 11.19.99 CLAS Online Group.
 */
public class EbEr implements Runnable {
    
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

    private static String _clonlog;

    private static Jgetenv myJgetenv;

    public  SysExe mySysExe = new SysExe();
    
    
    
    public static void main(String argv[]) {
	new Thread ( new EbEr() ).start();
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
	    DataPool.eber_update = true;
	    DataPool.eb_status   = eb_status;
	if(eb_status.equals("UNDEFINED")) {
	    //..............................sleep for 7sec. and check another time.
	    try {
		Thread.sleep(7000);
	    } catch (InterruptedException e) {
		System.out.println("interruped exception: " +e);
	    }
	    eb_status = mySysExe.run_vxworks("EB1", "status");
	}
	
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
	}
    } 
    
    
    
    
    /** 
     * Method  will read the EB1 and ER1 information and update local
     * instance variables. 
     */
    public void info_collect () {
	//..........................EB statistics    
	if(eb_status.equals("UNDEFINED")) {
	    CODA_config = "unknown";
	    eb_evtrate = 0.0;
	    eb_datarate = 0.0;
	    eb_numevt = 0;
	    try {
		final PrintWriter writer = new PrintWriter(new FileWriter(_clonlog+"/daqtlc/log//eb.log", append));
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
		eb_evtrate = 0.0;
		eb_datarate = 0.0;
		eb_numevt = 0;
	    } else {
		StringTokenizer line_token = new StringTokenizer(temp_line);
		while(line_token.hasMoreTokens() == true) {
		    try {
			eb_numevt = Integer.parseInt(line_token.nextToken());
			eb_evtrate = (Double.valueOf(line_token.nextToken()).doubleValue());
			line_token.nextToken();
			eb_datarate = (Double.valueOf(line_token.nextToken()).doubleValue())/1000.0;
		    } catch (NoSuchElementException a ) { break;} 
		}
	    }
	    //EB buffer occupancy monitoring
	    temp_line = mySysExe.run_vxworks("EB1", "frag_sized");
	    if(temp_line.equals("UNDEFINED")) {
		CODA_config = "unknown";
		eb_evtrate = 0.0;
		eb_datarate = 0.0;
		eb_numevt = 0;
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
			    } catch (NoSuchElementException a ) { break;} 
			}
		    }
		}
	    }     
	}
	
	//..........................ER statistics
	if(er_status.equals("UNDEFINED")) {
	    er_evtrate = 0.0;
	    er_datarate = 0.0;
	    er_numevt = 0;
	    try {
		final PrintWriter writer = new PrintWriter(new FileWriter(_clonlog+"/daqtlc/log//er.log", append));
		writer.println((new Date()) +" -> " + " ER crashed");
		writer.close(); 
	    } catch (final IOException e) {
		System.err.println(e);
	    }
	    
	} else {
	    temp_line = mySysExe.run_vxworks("ER1", "statistics");
	    if(temp_line.equals("UNDEFINED")) {
		CODA_config = "unknown";
		eb_evtrate = 0.0;
		eb_datarate = 0.0;
		eb_numevt = 0;
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
    }
    
    
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
    }
}




