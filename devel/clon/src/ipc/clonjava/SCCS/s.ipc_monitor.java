h59061
s 00001/00001/00167
d D 1.13 08/12/09 21:56:43 boiarino 14 13
c *** empty log message ***
e
s 00001/00001/00167
d D 1.12 07/11/23 19:16:29 boiarino 13 12
c *** empty log message ***
e
s 00002/00000/00166
d D 1.11 06/11/14 13:21:02 boiarino 12 11
c add package clonjava;
c 
e
s 00002/00002/00164
d D 1.10 06/07/09 09:48:18 boiarino 11 10
c add clonjava.
e
s 00014/00014/00152
d D 1.9 03/10/03 16:52:25 wolin 10 9
c Now works with SS65
e
s 00001/00001/00165
d D 1.8 01/10/30 15:30:51 wolin 9 8
c Minor
e
s 00039/00020/00127
d D 1.7 01/10/30 15:23:49 wolin 8 7
c Added -dump,-t,-d,-s
e
s 00020/00003/00127
d D 1.6 99/12/10 14:38:12 wolin 7 6
c Added -all, default is no evt_events
e
s 00003/00003/00127
d D 1.5 99/12/07 15:31:14 wolin 6 5
c Minor reformatting
e
s 00001/00001/00129
d D 1.4 99/11/23 11:46:29 wolin 5 4
c More format mods
e
s 00002/00002/00128
d D 1.3 99/11/23 11:36:16 wolin 4 3
c Mods to output format
e
s 00040/00008/00090
d D 1.2 99/11/22 14:07:47 wolin 3 1
c Added command line args and help
e
s 00000/00000/00000
d R 1.2 99/11/22 12:36:51 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 ipcutil/s/ipc_monitor.java
e
s 00098/00000/00000
d D 1.1 99/11/22 12:36:50 wolin 1 0
c 
e
u
U
f b 
f e 0
t
T
I 1
//  ipc_monitor

D 10
// ascii dump of all ipc msg traffic
E 10
I 10
// ascii dump of all ipc msg traffic including contents
E 10

I 12
package clonjava;

E 12
D 10

E 10
import java.io.*;
import com.smartsockets.*;
D 11
import clon.ipcutil.*;
import clon.util.*;
E 11
I 11
import clonjava.clon.ipcutil.*;
import clonjava.clon.util.*;
E 11


D 3
//-------------------------------------------------------------------

E 3
I 3
//-----------------------------------------------------------------------------
//  begin class definition
//-----------------------------------------------------------------------------
E 3

I 7

E 7
public class ipc_monitor implements TipcProcessCb {
    
D 8
    static int count = 0;    
E 8
I 8
    static int count      = 0;    
E 8
I 3
    static String project = "clastest";
I 7
D 8
    static int all   = 0;
E 8
I 8
    static int dump       = 0;
    static String type    = null;
    static String sender  = null;
    static String dest    = null;
	
E 8
E 7
E 3
    
    
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
    
I 7

//-----------------------------------------------------------------------------

E 7
    
    // message process callback
    public void process(TipcMsg msg, Object arg) {
	
I 8
	String thisSender = msg.getSender();
	String thisType   = (msg.getType()).getName();
	String thisDest   = msg.getDest();
E 8
I 7

D 8
	// ignore certain message types
	if(all==0) {
	    if(msg.getType().getName().equals("evt_event"))return;
	}

E 8
I 8
	
	if( 
	   ((sender==null)||(sender.equalsIgnoreCase(thisSender))) &&
	   ((type  ==null)||(  type.equalsIgnoreCase(thisType  ))) &&
	   ((dest  ==null)||(  dest.equalsIgnoreCase(thisDest  ))) ) {
E 8

E 7
D 8
	count++;
	try {
	    System.out.println(
D 5
			       Format.justify((msg.getType()).getName(),15) + 
E 5
I 5
D 6
			       Format.justify((msg.getType()).getName(),18) + 
E 5
D 4
			       Format.justify(msg.getSender(),30) +
			       Format.justify(msg.getDest(),30) +
E 4
I 4
			       Format.justify(msg.getSender(),17) +
			       Format.justify(msg.getDest(),50) +
E 6
I 6
			       Format.justify((msg.getType()).getName(),18) + " " +
			       Format.justify(msg.getSender(),35) + " " +
			       Format.justify(msg.getDest(),45) + " " +
E 6
E 4
			       count);
	}
	catch (Exception e) {
	    Tut.exitFailure("process callback exception: " + e.getMessage());
E 8
I 8
	    count++;
	    try {
		System.out.println(
				   Format.justify(thisType,18) + " " +
				   Format.justify(thisSender,35) + " " +
				   Format.justify(thisDest,45) + " " +
				   count);
D 9
		if(dump==1) {msg.print();}
E 9
I 9
		if(dump==1) {msg.print(); System.out.println();}
E 9
	    }
	    catch (Exception e) {
		Tut.exitFailure("process callback exception: " + e.getMessage());
	    }
E 8
	}
I 8

E 8
	return;
    }
    

I 7
//-----------------------------------------------------------------------------

E 7
    
    // main method
    public static void main(String[] argv) {
	
I 3
D 10

E 10
I 10
	
E 10
	// decode command line
	decode_command_line(argv);


E 3
	ipc_monitor ipc = new ipc_monitor();
	
	try {
	    
	    // connect to ipc
D 3
	    ipc_init.set_project("clasprod");
E 3
I 3
	    ipc_init.set_project(project);
E 3
D 10
	    ipc_init.set_server_names("clon00,clon10,jlabs2,db6");
E 10
I 10
D 13
	    ipc_init.set_server_names("clon10,clon00,clon05,jlabs2,ollie");
E 13
I 13
D 14
	    ipc_init.set_server_names("clon10,clon00");
E 14
I 14
	    ipc_init.set_server_names("clondb1,clondb2");
E 14
E 13
E 10
	    ipc_init.set_user_control_callback(new myControl());
	    TipcSrv srv = ipc_init.init(null,"ipc monitor");
	    
D 10
	    
	    // subscribe to subjects
	    srv.setSubjectSubscribe("*", true);
	    srv.setSubjectSubscribe("...", true);
D 3
	    srv.setSubjectSubscribe("info_server/in/*", true);
E 3
	    
	    
E 10
I 10

	    // subscribe to all possible subjects
	    srv.setSubjectSubscribe("/...", true);
	
	
E 10
	    // create callback
D 3
	    srv.addProcessCb(ipc,"*",srv);
	    srv.addProcessCb(ipc,"...",srv);
	    srv.addProcessCb(ipc,"info_server/in/*",srv);
E 3
I 3
	    srv.addProcessCb(ipc,null,null,srv);
E 3
	    
D 10
	    
E 10
I 10

E 10
	    // flush all messages
	    srv.flush();
I 10


	} catch (Exception e) {
	    e.printStackTrace();
	    Tut.exitFailure("exception in main: " + e.getMessage());
E 10
	}
D 10
	catch (Exception e) {
	    Tut.exitFailure("exception: " + e.getMessage());
	}
E 10
	
	
	try {
	    ipc_init.check(TipcDefs.TIMEOUT_FOREVER);
	}
	catch (Exception e) {
	    Tut.exitFailure("mainLoop exception: " + e.getMessage());
	}
	
    }
    
D 3
}
E 3

//-------------------------------------------------------------------
I 3


    static public void decode_command_line(String[] args) {
	
	int i;
	String help = "\nUsage:\n\n" + 
D 7
	    "         java ipc_monitor [-a project]\n";
E 7
I 7
D 8
	    "         java ipc_monitor [-a project] [-all]\n";
E 8
I 8
	    "         java ipc_monitor [-a project] [-t type] [-s sender] [-d dest] [-dump]\n";
E 8
E 7


	// loop over all args
	for(i=0; i<args.length; i++) {
	    
	    if(args[i].equalsIgnoreCase("-h")) {
		System.out.println(help);
		System.exit(-1);
		
I 7
D 8
	    } else if(args[i].equalsIgnoreCase("-all")) {
		all=1;
E 8
I 8
	    } else if(args[i].equalsIgnoreCase("-dump")) {
		dump=1;
E 8

E 7
	    } else if(args[i].equalsIgnoreCase("-a")) {
		project=args[i+1];
I 8
		i++;
	    } else if(args[i].equalsIgnoreCase("-s")) {
		sender=args[i+1];
		i++;
	    } else if(args[i].equalsIgnoreCase("-t")) {
		type=args[i+1];
		i++;
	    } else if(args[i].equalsIgnoreCase("-d")) {
		dest=args[i+1];
E 8
		i++;
	    }
D 7

	    return;
E 7
	}
I 7

	return;
E 7
    }
    
}
//-----------------------------------------------------------------------------
//  end class definition
//-----------------------------------------------------------------------------
E 3
E 1
