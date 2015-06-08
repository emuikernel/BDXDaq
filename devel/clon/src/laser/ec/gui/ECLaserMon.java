import java.io.*;
import java.lang.*;
import java.util.*;
import java.awt.*;
import java.awt.event.*;
import javax.swing.*;
import javax.swing.event.*;
import com.smartsockets.*;
import clon.ipcutil.*;


public class ECLaserMon extends ECLaserMonGui implements Runnable {

    public static TipcSrv srv;
    public int numSeq;
    public int checkSeq = 0 ;
    public int checkTime = -1;

    // color definition
    public Color color1[] = {Color.red, Color.green};
    public Color color2[] = {Color.orange, Color.green};
    
    // message process callback; status, progress, daqstatus
    //
    public class StatusCb implements TipcProcessCb {
	public void process(TipcMsg msg, Object arg) {
	    String var_name;
	    try {
		msg.setCurrent(0);
		while(true) {
		    var_name = msg.nextStr();
		    int var_value = msg.nextInt4();
		    System.out.println(var_name + " " + var_value);
		    visual_status(var_name, var_value);
		}
	    } catch (TipcException e) {} 	
	}
    }
    public class ProgressCb implements TipcProcessCb {
	public void process(TipcMsg msg, Object arg) {
	    String info;
	    try {
		msg.setCurrent(0);
		info = msg.nextStr();
		// System.out.println(info);
		progressTex.insert(info + "\n",0);
	    } catch (TipcException e) {} 	
	}
    }
    public class DaqstatusCb implements TipcProcessCb {
	public void process(TipcMsg msg, Object arg) {
	    String info;
	    try {
		msg.setCurrent(0);
		info = msg.nextStr();
		if(info.equals("hardware_monitor/eber")) visual_eber(msg);
		else if(info.equals("update_run_control/clasprod")) visual_run(msg);
	    } catch (TipcException e) {} 	
	}
    }
    
    public ECLaserMon() {
	setVisible(true);
    } // ECLaserMon()


    // update of each status as changing color, text, etc.
    //
    public void visual_status(String id, int val) {
	
	if(id.equals("TimeInf")) {
	    if((val - checkTime) == 1)
		checkTime = val;
	}
        else if(id.equals("broadcast")) {
	    checkTime = -1;
	    if(val == 1) recover();
	}
	else if(id.equals("clon10_connection")) rocBut[0].setBackground(color1[val]);
	else if(id.equals("config_file_read")) rocBut[1].setBackground(color1[val]);
	else if(id.equals("coda_ready")) rocBut[2].setBackground(color1[val]);
	else if(id.equals("coda_config_ok")) rocBut[3].setBackground(color1[val]);
	else if(id.equals("camac1_connection")) rocBut[4].setBackground(color1[val]);
	else if(id.equals("trigger_bit_11_set")) rocBut[5].setBackground(color1[val]);	
	 
	else if(id.equals("laser_power_on"))  laserBut[0].setBackground(color1[val]);
	else if(id.equals("laser_ready"))  laserBut[1].setBackground(color2[val]);
	else if(id.equals("laser_enabled")) laserBut[2].setBackground(color2[val]);
       
	else if(id.equals("pulser_ready"))  pulserBut[0].setBackground(color1[val]);
	else if(id.equals("jorway_pulser")) pulserBut[1].setBackground(color2[val]);
	else if(id.equals("pulser_loaded")) pulserBut[2].setBackground(color2[val]);

	else if(id.equals("laser_filter_ready"))  filterBut1.setBackground(color1[val]);
	
	else if(id.equals("sequence_number")) {
	    sequenceTex.setText(Integer.toString(val));
	}
	else if(id.equals("currentSequence_number")) {
	    currentSequenceTex.setText(Integer.toString(val));
	}
	else if(id.equals("current_filter_position")) {
	    filterTex.setText(Integer.toString(val));
	}
	
	else return;

    } // visul_status()
    

    // display the status of sequence
    //
    public void sequence_check(TipcMsg msg, int val) {
	int CurrentTotal = 0;
	int SeqTotal = 0;
	
	try {
	    String seq_nmb = msg.nextStr();
	    String current_seq = msg.nextStr();
	    System.out.println(seq_nmb + " " + current_seq);
	    sequenceTex.setText(seq_nmb);
	    currentSequenceTex.setText(current_seq);
	} catch (TipcException e) {}
	
	if(val == 1 ) {
	    int checkSeq = 1 ;
	    for(int i = 0; i < numSeq; i++) {
		CurrentTotal = CurrentTotal + checkSeq;
		SeqTotal = SeqTotal + i + 1;
	    }
	    if(CurrentTotal == SeqTotal) {
		sequenceDoneBut.setBackground(color2[1]);
		run_unix("./door_bell");
	    }
	} 
	
    } //  sequence_check()
    
    public void visual_eber(TipcMsg msg) {
	try {
	    String run_config = msg.nextStr();
	    String eb_status = msg.nextStr();
	    System.out.println(run_config + " " + eb_status);
	    daqTex[1].setText(run_config);
	    daqTex[2].setText(eb_status);
	} catch (TipcException e) {}
    }
    
    public void visual_run(TipcMsg msg) {
	try {
	    int rn = msg.nextInt4();
	    System.out.println(rn);
	    daqTex[0].setText(Integer.toString(rn));
	} catch (TipcException e) {}
    }
    
    public void run() {    
	TipcMsg msg = null;
	try {
	    // create message types and callbacks
	    StatusCb statusCb = new StatusCb();
	    TipcCb rcbh1 = srv.addProcessCb(statusCb,"/EClaser/status", null);
	    if (null == rcbh1) {
		Tut.exitFailure("Couldn't register subject callback!\n");
	    } 
	    ProgressCb progressCb = new ProgressCb();
	    TipcCb rcbh2 = srv.addProcessCb(progressCb,"/EClaser/progress", null);
	    if (null == rcbh2) {
		Tut.exitFailure("Couldn't register subject callback!\n");
	    } 
	    DaqstatusCb daqstatusCb = new DaqstatusCb(); 
	    TipcCb rcbh3 = srv.addProcessCb(daqstatusCb,"info_server/in/...", null);
	    if (null == rcbh3) {
		Tut.exitFailure("Couldn't register subject callback!\n");
	    } 
	    
	    // subscribe to subjects
	    srv.setSubjectSubscribe("/EClaser/progress", true);
	    srv.setSubjectSubscribe("/EClaser/status", true);
	    srv.setSubjectSubscribe("info_server/in/update_run_control/clasprod", true);
	    srv.setSubjectSubscribe("info_server/in/hardware_monitor/eber", true);
	    
	    // read and process all incoming messages
	    while (srv.mainLoop(TipcDefs.TIMEOUT_FOREVER)) {
	    } 
	    
	    // unregister the callbacks
	    srv.removeProcessCb(rcbh1);
	    srv.removeProcessCb(rcbh2);
	    srv.removeProcessCb(rcbh3);
	 
	    // disconnect from RTserver
	    srv.destroy();
	    
	} catch (TipcException e) { 
	    Tut.fatal(e); 
	}
    } // run
  
   public void run_unix (String command) {
       Runtime r = Runtime.getRuntime();
       try {
	   r.exec (command);    
       } catch (IOException e) {
	   System.out.println("run_unix exception: " +e);
       }
  }
  
    public static void main(String argv[]) {
	try {
	    Tut.setOption("ss.project", "clasprod");
	    Tut.setOption("ss.server_names","clon00,clon10");
            
	    srv = TipcSvc.getSrv();
	    if(!srv.create()) { 
		Tut.exitFailure("Couldn't connect to RTserver!\n");
	    } 
	} catch (TipcException e) {
	    Tut.fatal(e);
	} 

	new Thread(new ECLaserMon()).start();
	new Thread(new SendTimeInf(srv)).start();

    } // main
} // ECLaserMon


class SendTimeInf implements Runnable {
    
    public static TipcSrv srv;
  
    public SendTimeInf(TipcSrv s){
	srv = s;
    } // SendTimeInf()
    
    public void run() {
	boolean go_loop = true;
	int count = 1;
	int timeInterval = 30 ;
	try {
	    // create a message 
	    TipcMsg msg = TipcSvc.createMsg(TipcMt.INFO);
	 
	    // set the destination subject of the message
	    msg.setDest("/EClaser/status");

	    while (go_loop) {
		// delay 
		try {
		    Thread.sleep(1000*timeInterval);
		} catch (InterruptedException e) {}
		
		// build a message with time information.
		msg.setNumFields(0);
		msg.appendStr("TimeInf");
		msg.appendInt4(count);
		
		// send the message
		srv.send(msg);
		srv.flush();
	    
		count++;
	    } // while

	    //destroy the message
	    msg.destroy();
       	    
	    //disconnect from server
	    srv.destroy();
	} catch (TipcException e) {
	    Tut.warning(e);
	} // catch
    } // run
} // SendTimeInf



