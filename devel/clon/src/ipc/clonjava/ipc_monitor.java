//  ipc_monitor

// ascii dump of all ipc msg traffic including contents

package clonjava;

import java.io.*;
import com.smartsockets.*;
import clonjava.clon.ipcutil.*;
import clonjava.clon.util.*;


//-----------------------------------------------------------------------------
//  begin class definition
//-----------------------------------------------------------------------------


public class ipc_monitor implements TipcProcessCb {
    
    static int count      = 0;    
    static String project = "clastest";
    static int dump       = 0;
    static String type    = null;
    static String sender  = null;
    static String dest    = null;
	
    
    
    // user control callback just ignores control messages
    private static class myControl implements ipc_init.user_control_callback {
	
	public void process(TipcMsg msg, Object obj) {
	    try {
		msg.setNumFields(0);
	    } catch (Exception e) {
		System.out.println(e);
	    }
	    return;
	}
    }
    

//-----------------------------------------------------------------------------

    
    // message process callback
    public void process(TipcMsg msg, Object arg) {
	
	String thisSender = msg.getSender();
	String thisType   = (msg.getType()).getName();
	String thisDest   = msg.getDest();

	
	if( 
	   ((sender==null)||(sender.equalsIgnoreCase(thisSender))) &&
	   ((type  ==null)||(  type.equalsIgnoreCase(thisType  ))) &&
	   ((dest  ==null)||(  dest.equalsIgnoreCase(thisDest  ))) ) {

	    count++;
	    try {
		System.out.println(
				   Format.justify(thisType,18) + " " +
				   Format.justify(thisSender,35) + " " +
				   Format.justify(thisDest,45) + " " +
				   count);
		if(dump==1) {msg.print(); System.out.println();}
	    }
	    catch (Exception e) {
		Tut.exitFailure("process callback exception: " + e.getMessage());
	    }
	}

	return;
    }
    

//-----------------------------------------------------------------------------

    
    // main method
    public static void main(String[] argv) {
	
	
	// decode command line
	decode_command_line(argv);


	ipc_monitor ipc = new ipc_monitor();
	
	try {
	    
	    // connect to ipc
	    ipc_init.set_project(project);
	    ipc_init.set_server_names("clondb1,clondb2");
	    ipc_init.set_user_control_callback(new myControl());
	    TipcSrv srv = ipc_init.init(null,"ipc monitor");
	    

	    // subscribe to all possible subjects
	    srv.setSubjectSubscribe("/...", true);
	
	
	    // create callback
	    srv.addProcessCb(ipc,null,null,srv);
	    

	    // flush all messages
	    srv.flush();


	} catch (Exception e) {
	    e.printStackTrace();
	    Tut.exitFailure("exception in main: " + e.getMessage());
	}
	
	
	try {
	    ipc_init.check(TipcDefs.TIMEOUT_FOREVER);
	}
	catch (Exception e) {
	    Tut.exitFailure("mainLoop exception: " + e.getMessage());
	}
	
    }
    

//-------------------------------------------------------------------


    static public void decode_command_line(String[] args) {
	
	int i;
	String help = "\nUsage:\n\n" + 
	    "         java ipc_monitor [-a project] [-t type] [-s sender] [-d dest] [-dump]\n";


	// loop over all args
	for(i=0; i<args.length; i++) {
	    
	    if(args[i].equalsIgnoreCase("-h")) {
		System.out.println(help);
		System.exit(-1);
		
	    } else if(args[i].equalsIgnoreCase("-dump")) {
		dump=1;

	    } else if(args[i].equalsIgnoreCase("-a")) {
		project=args[i+1];
		i++;
	    } else if(args[i].equalsIgnoreCase("-s")) {
		sender=args[i+1];
		i++;
	    } else if(args[i].equalsIgnoreCase("-t")) {
		type=args[i+1];
		i++;
	    } else if(args[i].equalsIgnoreCase("-d")) {
		dest=args[i+1];
		i++;
	    }
	}

	return;
    }
    
}
//-----------------------------------------------------------------------------
//  end class definition
//-----------------------------------------------------------------------------
