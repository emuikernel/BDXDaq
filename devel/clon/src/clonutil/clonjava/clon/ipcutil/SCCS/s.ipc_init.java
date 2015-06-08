h43927
s 00005/00001/00609
d D 1.27 09/02/04 23:08:17 boiarino 28 27
c *** empty log message ***
e
s 00002/00002/00608
d D 1.26 08/12/09 22:01:31 boiarino 27 26
c *** empty log message ***
e
s 00001/00001/00609
d D 1.25 06/07/09 09:36:21 boiarino 26 25
c add clonjava.
e
s 00003/00004/00607
d D 1.24 03/06/23 16:25:08 wolin 25 24
c Minor
e
s 00002/00003/00609
d D 1.23 03/06/16 15:36:36 wolin 24 23
c minor
e
s 00002/00001/00610
d D 1.22 01/10/12 11:07:32 wolin 23 22
c Added ipc2timeline
e
s 00001/00001/00610
d D 1.21 00/01/11 12:10:11 wolin 22 21
c Hostname java_client
e
s 00055/00002/00556
d D 1.20 99/11/19 17:03:01 wolin 21 20
c Added user control callback
e
s 00005/00001/00553
d D 1.19 99/10/08 17:08:12 wolin 20 19
c Typo fixing null hostname
c 
e
s 00009/00005/00545
d D 1.18 99/10/08 14:42:47 wolin 19 18
c Bombproof against UnknownHostException
c 
e
s 00028/00037/00522
d D 1.17 99/09/27 11:05:05 wolin 18 17
c Updated exceptions, other minor mods
c 
e
s 00069/00049/00490
d D 1.16 99/09/23 13:59:34 wolin 17 16
c Added package and exceptions
e
s 00015/00012/00524
d D 1.15 99/09/17 16:17:40 wolin 16 15
c Minor bugs, pretty much done now!
c 
e
s 00020/00018/00516
d D 1.14 99/09/17 16:05:44 wolin 15 14
c Improved javadoc, etc.
c 
e
s 00062/00006/00472
d D 1.13 99/09/17 15:14:58 wolin 14 13
c Added javadoc comments, etc.
c 
e
s 00031/00031/00447
d D 1.12 99/09/17 14:14:08 wolin 13 12
c reconnect now working
c 
e
s 00021/00025/00457
d D 1.11 99/09/17 12:23:35 wolin 12 11
c Everything now static, almost done
c 
e
s 00109/00082/00373
d D 1.10 99/09/15 15:00:55 wolin 11 10
c More to come...
c 
e
s 00044/00030/00411
d D 1.9 99/09/15 14:29:04 wolin 10 9
c Closer...
c 
e
s 00006/00002/00435
d D 1.8 99/09/14 16:09:36 wolin 9 8
c More to go...
c 
e
s 00011/00006/00426
d D 1.7 99/09/14 15:38:57 wolin 8 7
c Getting better...
c 
e
s 00056/00034/00376
d D 1.6 99/09/14 15:01:47 wolin 7 6
c More to come...
c 
e
s 00005/00003/00405
d D 1.5 99/09/14 14:46:26 wolin 6 5
c Still working on it
c 
e
s 00077/00033/00331
d D 1.4 99/09/14 14:40:03 wolin 5 4
c Most features working
c 
e
s 00052/00024/00312
d D 1.3 99/09/14 12:01:38 wolin 4 3
c Still working on this...
c 
e
s 00077/00060/00259
d D 1.2 99/09/14 11:26:57 wolin 3 1
c Still working
c 
e
s 00000/00000/00000
d R 1.2 99/09/13 17:02:49 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 jar/clon/ipcutil/ipc_init.java
e
s 00319/00000/00000
d D 1.1 99/09/13 17:02:48 wolin 1 0
c 
e
u
U
f b 
f e 0
t
T
I 1
//  ipc_init.java
//    
//   standard initializion of Smartsockets IPC for CLAS
//
I 5
D 13
//  Still to do:
//    use of static all wrong
I 6
//    some features not working yet
E 6
//
E 13
I 10
//  main method at end for testing
//
E 10
E 5
D 11
//  EJW, 9-sep-99
E 11
I 11
D 13
//
E 13
//  EJW, 15-sep-99
E 11


D 3

E 3
I 3
D 5
package clon.ipcutil;
E 5
I 5
D 7
//package clon.ipcutil; ???
E 7
I 7
D 15
// package clon.ipcutil; ???
E 15
I 15
D 17
//package clon.ipcutil;
E 17
I 17
D 26
package clon.ipcutil;
E 26
I 26
package clonjava.clon.ipcutil;
E 26
E 17
E 15
I 8
D 10
import java.io.*;
E 10
I 9
import java.net.*;
E 9
E 8
E 7
E 5
E 3
import com.smartsockets.*;


D 17

E 17
//-----------------------------------------------------------------------------
D 5
//  begin class definition
E 5
I 5
//  begin main class definition
E 5
//-----------------------------------------------------------------------------

I 14
/**
D 17
 * Initializes CLAS standard SmartSockets IPC settings.
E 17
I 17
 * Initializes CLAS standard SmartSockets IPC settings.  Subscribes to standard 
 * subjects, sets default quit and user status poll callback, connects to server,
 * etc.  Should be called by all java ipc clients.
E 17
 *
 * @version 1.0
 * @author E.Wolin
 */
E 14
D 15
public class ipc_init {
E 15
I 15
public abstract class ipc_init {
E 15


D 3
  // class variables
  static boolean first_time         = true;
E 3
I 3
D 10
  // defaults
E 10
I 10
D 11
  // defaults for user-defined stuff
E 10
D 4
  static String project             = "clastest";
  static String server_names        = "clon00,clon10,jlabs2,db6";
E 3
  static String status_poll_group   = "clas_monitor";
D 3
  static int num_status_poll_req    = 0;
E 3
  static String username            = "java client";  // ???
  static String nodename            = "java node";  // ???
  static int  pid                   = 0;  // ???
  static int  uid                   = 0;  // ???
D 3
  static double start               = Tut.getWallTime();
E 3

I 3
  // misc
  static boolean first_time         = true;
  static double start               = Tut.getWallTime();
  static int num_status_poll_req    = 0;
E 4
I 4
D 5
  static String project              = "clastest";
  static String server_names         = "clon00,clon10,jlabs2,db6";
  static String status_poll_group    = "clas_monitor";
  static String username             = "java client";  // ???
  static String nodename             = "java node";  // ???
  static int  pid                    = 0;  // ???
  static int  uid                    = 0;  // ???
 
E 5
I 5
  private static String project              = "clastest";
  private static String server_names         = "clon00,clon10,jlabs2,db6";
  private static String status_poll_group    = "clas_monitor";
I 10
  private quit_callback quitCb               = new ipc_default_quit_callback();
  private user_status_poll_result userStatus = new ipc_default_user_status();
E 11
I 11
  // defaults for user-defined stuff...can only be changed before init method called
  private static String project              	    = "clastest";
D 13
  private static String server_names         	    = "clon00,clon10,jlabs2,db6";
E 13
I 13
D 24
  private static String server_names         	    = "db6";
E 24
I 24
D 27
  private static String server_names         	    = "clon10,clon00,clon05";
E 27
I 27
  private static String server_names         	    = "clondb1,clondb2";
E 27
E 24
E 13
  private static String status_poll_group    	    = "clas_monitor";
D 12
  private quit_callback quitCb        	            = new ipc_default_quit_callback();
  private user_status_poll_result userStatus        = new ipc_default_user_status();
E 12
I 12
D 13
  private static quit_callback quitCb        	    = new ipc_default_quit_callback();
  private static user_status_poll_result userStatus = new ipc_default_user_status();
E 13
I 13
  private static quit_callback quitCb        	    = new default_quit_callback();
  private static user_status_poll_result userStatus = new default_user_status();
I 21
  private static user_control_callback userControl  = new default_user_control();
E 21
E 13
E 12
E 11

  // other defaults
E 10
I 8
D 11
  private static String hostname             = null;
E 8
  private static String username             = System.getProperty("user.name");
D 8
  //private static String nodename             = System.getenv("HOST"); ???
  private static String nodename             = System.getProperty("os.name");
E 8
  private static int pid                     = 0;
I 7
D 10
  private quit_callback quitCb               = new ipc_default_quit_callback();
  private user_status_poll_result userStatus = new ipc_default_user_status();
E 10
E 7
D 6
  private static int uid                     = 0;
E 6
D 8
 	  
E 8
I 7

E 7
E 5
  // misc 
D 5
  static boolean first_time          = true;
  static double start                = Tut.getWallTime();
  static int num_status_poll_req     = 0;
E 5
I 5
  private static boolean first_time          = true;
  private static double start                = Tut.getWallTime();
  private static int num_status_poll_req     = 0;
E 11
I 11
  private static String hostname             	    = null;
  private static String username             	    = System.getProperty("user.name");
  private static int pid                     	    = 0;
I 12
D 13
  private static ipc_control_callback ctrlCb        = new ipc_control_callback();
E 13
I 13
  private static control_callback ctrlCb            = new control_callback();
E 13

E 12
       
  // misc 	 
  private static boolean first_time          	    = true;
  private static double start                	    = Tut.getWallTime();
  private static int num_status_poll_req     	    = 0;   
E 11
D 7
	  
E 7
  private static TipcSrv server;
D 7
  private user_status_poll_result userStatus = new ipc_default_user_status();
E 5

E 4
E 3
D 5
  static TipcSrv server;
I 4
  user_status_poll_result userStatus = new ipc_default_user_status();
E 5
I 5

//-----------------------------------------------------------------------
// for testing
//-----------------------------------------------------------------------


  public static void main(String[] args) {

    class mystatus implements user_status_poll_result {
      public void addUserStatus(TipcMsg msg) {
	msg.appendStr("Hello");
D 6
	msg.appendStr("there");
E 6
I 6
	msg.appendStr("World");
	msg.appendStr("integer");
	msg.appendInt4(123);
E 6
	return;
      }
    }
  
    ipc_init ipc = new ipc_init("java_test","testing");
    mystatus m = new mystatus();
    ipc.ipc_set_user_status_poll_callback(m);
    
    while(true) {
      ipc.ipc_check(10.0);
    }
  }
E 7
E 5
E 4
D 3
  static quit_callback;
  static status_poll_callback;
E 3


D 4
//-----------------------------------------------------------------
E 4
I 4
//-----------------------------------------------------------------------
D 5
//  methods
E 5
I 5
//  public methods
E 5
//-----------------------------------------------------------------------
E 4


I 15
D 16
  //  /** 
  //   *  Does nothing.
  //   */
  //  public ipc_init() {};


E 16
E 15
I 14
  /**
D 17
   *  Sets standard options, gets server, etc.
E 17
I 17
   *  Main initialization routine, sets options, gets server, etc.
   *  Passes all exceptions up to caller.
E 17
   *
   *  @param uniq_sub      unique_subject.
   *  @param ident_string  identification string (arbitrary).
   */
E 14
D 3
  public static int ipc_init(String project, String uniq_sub, String id_str) {
E 3
I 3
D 4
  public static boolean ipc_init(String prj, String uniq_sub, String id_str) {
E 4
I 4
D 5
  public boolean ipc_init(String prj, String uniq_sub, String id_str) {
E 5
I 5
D 10
  public ipc_init(String uniq_sub, String id_str) {
E 10
I 10
D 11
  public ipc_init(String uniq_sub, String ident_string) {
E 11
I 11
D 12
  public synchronized boolean init(String uniq_sub, String ident_string) {
E 12
I 12
D 17
  public static synchronized boolean init(String uniq_sub, String ident_string) {
E 12
E 11
E 10
E 5
E 4
E 3

E 17
I 17
D 18
  public static synchronized boolean init(String uniq_sub, String ident_string) 
       throws java.lang.Exception {
E 18
I 18
  public static synchronized TipcSrv init(String uniq_sub, String ident_string) 
D 19
  throws UnknownHostException,TipcException {
E 19
I 19
  throws TipcException {
E 19
E 18
E 17

D 16
    if(first_time==true) {
E 16
I 16
    if(first_time) {
E 16

D 3
      first_time=false;


E 3
      try {

I 8
D 10

E 10
	// get local host name
D 9
	Process p = Runtime.getRuntime().exec("printenv HOST");
	hostname=(new BufferedReader(new InputStreamReader(p.getInputStream()))).readLine();
E 9
I 9
D 10
	Process p = (Runtime.getRuntime()).exec("printenv HOST");
	hostname  = (new BufferedReader(new InputStreamReader(p.getInputStream()))).readLine();

	InetAddress in = InetAddress.getLocalHost();
	System.out.println(in.getHostName());
E 9

E 10
I 10
D 19
	String temp=(InetAddress.getLocalHost()).getHostName();
	hostname=temp.substring(0,temp.indexOf("."));
	
E 10
 	  
E 19
I 19
	try {
D 28
	  String temp=(InetAddress.getLocalHost()).getHostName();
E 28
I 28
//sergey: if hostname returns 'clon10' without '.jlab.org', then
// 'temp.substring' below fails; we will add dot in the end .. 
	  String temp=(InetAddress.getLocalHost()).getHostName() + ".";
E 28
D 20
	  hostname=temp.substring(0,temp.indexOf("."));
E 20
I 20
D 25
	  if(hostname!=null) {
E 25
I 25
	  if(temp!=null) {
I 28
        //System.out.println("\nnot null: \n" + temp);
E 28
E 25
	    hostname=temp.substring(0,temp.indexOf("."));
I 28
        //System.out.println("\nhostname: \n" + hostname);
E 28
	  } else {
D 22
	    hostname="unknown";
E 22
I 22
	    hostname="java_client";
E 22
	  }
E 20
	} catch (UnknownHostException e) {
	  hostname="unknown";
	}


E 19
E 8
D 11
	// set ident string
D 5
	Tut.setOption("ss.ident_string",id_str);
E 5
I 5
D 10
	//	Tut.setOption("ss.ident_string",id_str); ???
E 10
I 10
	TipcMon.setIdentStr(ident_string);
E 10
E 5
	
	
E 11
	// set server names
D 3
	Tut.setOption("ss.server_names","clon00,clon10,jlabs2,db6");
E 3
I 3
	Tut.setOption("ss.server_names",server_names);
E 3
	
	
	// set project
I 3
D 5
	if(prj!=null)project=prj;
E 5
E 3
	Tut.setOption("ss.project",project);
	
	
	// set unique subject
D 3
	Tut.setOption("ss.unique_subject",uniq_sub);
E 3
I 3
	if(uniq_sub!=null)Tut.setOption("ss.unique_subject",uniq_sub);
E 3
	
	
I 11
	// set ident string
	if(ident_string!=null)TipcMon.setIdentStr(ident_string);
	
	
E 11
I 10
	// subscribe to standard subjects
D 25
	Tut.setOption("ss.subjects","_all,"+project+",_"+hostname);
E 25
I 25
	Tut.setOption("ss.subjects","_all," + project + ",_" + hostname);
E 25


I 11
	// enable control commands
	Tut.setOption("ss.enable_control_msgs","quit,disconnect,connect");
	
	
E 11
E 10
	// create clas standard message types
	init_msg_types();
	
	
D 13
	// get server 
E 13
I 13
	// get server and connect
E 13
	server=TipcSvc.getSrv();
I 13
	server.create(TipcSrv.CONN_FULL);
E 13
	
	
D 3
	// create new control message callback
	// ???	
E 3
I 3
D 5
	// control message callback
E 5
I 5
D 12
	// destroy old control callback
D 10
	Object o = null;
	TipcCb oldCb = server.lookupProcessCb(null,TipcSvc.lookupMt(TipcMt.CONTROL),null,o);
	//	server.removeProcessCb(oldCb); ???
E 10
I 10
	//  Object o = null;
	//  TipcCb oldCb = server.lookupProcessCb(null,TipcSvc.lookupMt(TipcMt.CONTROL),null,o);
	//  server.removeProcessCb(oldCb); ???
E 10


E 12
D 10
	// create new control message callback
E 10
I 10
	// create new control message callback at higher priority (10) than default (0)
E 10
E 5
D 4
	//    ipc_control_callback ctrlCb = new ipc_control_callback();
	//	server.addProcessCb(TipcMt.CONTROL,ctrlCb,server);
E 4
I 4
D 12
	ipc_control_callback ctrlCb = new ipc_control_callback();
D 10
	server.addProcessCb(ctrlCb,TipcMt.CONTROL,server);
E 10
I 10
	server.addProcessCb(ctrlCb,TipcSvc.lookupMt(TipcMt.CONTROL),null,(short)10,server);
E 12
I 12
	server.addProcessCb(ctrlCb,TipcSvc.lookupMt(TipcMt.CONTROL),(short)10,server);
E 12
E 10
E 4
E 3
	

D 5
	// receive all standard subjects except "time"
E 5
I 5
D 10
	// receive standard subjects
	server.setSubjectSubscribe("_all",true);
D 8
	server.setSubjectSubscribe("_"+nodename,true);
E 8
I 8
	server.setSubjectSubscribe("_"+hostname,true);
E 8
E 5
D 3
	// ???	
E 3
	
	
	// receive "project" subject
	server.setSubjectSubscribe(project,true);
	
	
D 5
	// enable command interp
E 5
I 5
	// enable command interp ???
E 10
I 10
D 11
	// enable control commands (not sure if this is needed)
	Tut.setOption("ss.enable_control_msgs","quit,disconnect,connect");
E 10
E 5
D 3
	// ???	
E 3
	
	
E 11
	// flush all pending messages
	server.flush();

I 3

	// everything worked
	first_time=false;
I 11
D 18
	return(true);
E 18
I 18
	return(server);
E 18
E 11

D 18

E 3
      } catch (Exception e) {
D 3
	Tut.fatal(e);
E 3
I 3
	Tut.warning(e);
D 5
	return(false);
E 5
I 5
D 11
	return;
E 11
I 11
D 17
	return(false);
E 17
I 17
	throw(e);
E 18
I 18
      } finally {
E 18
E 17
E 11
E 5
E 3
      }
I 18

E 18
      
I 11
    } else {
D 18
      return(false);
E 18
I 18
      return(server);
E 18
E 11
    }
D 11

D 3
    return(0);
E 3
I 3
D 5
    return(true);
E 5
I 5
    return;
E 11
E 5
E 3
  }


//-----------------------------------------------------------------------


I 14
  /**
D 17
   *  Checks for and processes IPC messages.
E 17
I 17
   *  Processes IPC messages.
E 17
   *  @param timeout   Timeout in seconds.
   */
E 14
D 11
  public static void ipc_close() {
E 11
I 11
D 13
  public static boolean ipc_check(double timeout) {
E 13
I 13
D 17
  public static boolean check(double timeout) {
E 17
I 17
D 18
  public static boolean check(double timeout) throws Exception {
E 18
I 18
  public static void check(double timeout) throws TipcException {
E 18
E 17
E 13
E 11

D 11
    System.out.println("\nipc_close called...closing connection\n");
E 11
    try {
D 11
      server.destroy();
E 11
I 11
      server.mainLoop(timeout);
D 18
      return(true);
E 11
    } catch (Exception e) {
I 3
D 17
      Tut.warning(e);
I 11
      return(false);
E 17
I 17
      throw(e);
E 18
I 18
      return;
    } finally {
E 18
E 17
E 11
E 3
    }
D 11
      
    return;
E 11
  }


//-----------------------------------------------------------------------


I 14
  /**
D 17
   *  Closes connection.
E 17
I 17
   *  Closes connection to server.
E 17
   */
E 14
D 11
  public static void ipc_check(double timeout) {
E 11
I 11
D 13
  public static boolean ipc_close() {
E 13
I 13
D 17
  public static boolean close() {
E 17
I 17
D 18
  public static boolean close() throws Exception {
E 18
I 18
  public static void close() throws TipcException {
E 18
E 17
E 13
E 11

I 11
D 13
    System.out.println("\nipc_close called...closing connection\n");
E 13
I 13
    System.out.println("\nipc_init.close called...closing connection\n");
E 13
E 11
    try {
D 11
      server.mainLoop(timeout);
E 11
I 11
      server.destroy();
D 18
      return(true);
E 11
D 3
      return;
E 3
    } catch (Exception e) {
D 17
      Tut.warning(e);
I 11
      return(false);
E 17
I 17
      throw(e);
E 18
I 18
      return;
    } finally {
E 18
E 17
E 11
    }
I 3
D 11
    return;
E 11
E 3
  }


//-----------------------------------------------------------------------


I 14
  /**
   *  Sets project (must be called before init method).
D 15
   *  @param p   Project name.
E 15
I 15
   *  @param proj   Project name (default "clastest").
E 15
   */
E 14
D 11
  public static void init_msg_types() {
    
D 3
    // need to do this better
    // ???	

    class clas_ipc_mt {
      int id;
      String name;
    }

    clas_ipc_mt mtlist[] = new clas_ipc_mt[16];
    
//       (1,"status_poll_result"),
//       (2,"dbr_request"),
//       (3,"dbr_reply"),
//       (4,"dbr_request_rpc"),
//       (5,"dbr_reply_rpc"),
//       (6,"tcl_request"),
//       (7,"tcl_reply"),
//       (8,"cmlog"),
//       (9,"dd_event"),
//       (10,"dd_bosbank"),
//       (11,"dd_status"),
//       (12,"run_control"),
//       (13,"info_server"),
//       (14,"evt_event"),
//       (15,"evt_bosbank"),
//       (16,"evt_status")
//     };
E 3
I 3
    String mtname[] = {    
      "status_poll_result",
      "dbr_request",
      "dbr_reply",
      "dbr_request_rpc",
      "dbr_reply_rpc",
      "tcl_request",
      "tcl_reply",
      "cmlog",
      "dd_event",
      "dd_bosbank",
      "dd_status",
      "run_control",
      "info_server",
      "evt_event",
      "evt_bosbank",
      "evt_status"
    };
E 11
I 11
D 13
  public static boolean ipc_set_project(String p) {
E 13
I 13
D 15
  public static boolean set_project(String p) {
E 15
I 15
  public static boolean set_project(String proj) {
E 15
E 13
E 11
E 3

D 11
    try {
D 3
      for (int i=0; i<mtlist.length; i++) {
	TipcSvc.createMt(mtlist[i].name,mtlist[i].id,"verbose");
E 3
I 3
      for (int i=0; i<mtname.length; i++) {
	TipcSvc.createMt(mtname[i],i+1,"verbose");
E 3
      }
    } catch (Exception e) {
D 3
      Tut.fatal(e);
E 3
I 3
      Tut.warning(e);
E 11
I 11
D 16
    if(first_time=true) {
E 16
I 16
    if(first_time) {
E 16
D 15
      project=p;
E 15
I 15
      project=proj;
E 15
      return(true);
    } else {
I 16
      Tut.warning("set_project call ignored must be called before init method");
E 16
      return(false);
E 11
E 3
    }
D 11

E 11
  }

I 3

//-----------------------------------------------------------------------


I 14
  /**
   *  Sets server names (must be called before init method).
D 15
   *  @param s  Server name list.
E 15
I 15
   *  @param srvs  Server name list (default "db6").
E 15
   */
E 14
I 5
D 11
  public void ipc_set_project(String p) {
    project=p;
    return;
E 11
I 11
D 13
  public static boolean ipc_set_server_names(String s) {
E 13
I 13
D 15
  public static boolean set_server_names(String s) {
E 15
I 15
  public static boolean set_server_names(String srvs) {
E 15
E 13

D 16
    if(first_time=true) {
E 16
I 16
    if(first_time) {
E 16
D 15
      server_names=s;
E 15
I 15
      server_names=srvs;
E 15
      return(true);
    } else {
I 16
      Tut.warning("set_server_names call ignored, must be called before init method");
E 16
      return(false);
    }
E 11
  }


//-----------------------------------------------------------------------


I 14
  /**
   *  Sets status poll group (must be called before init method).
D 15
   *  @param g   Status poll group name.
E 15
I 15
   *  @param grp   Status poll group name (default "clas_monitor").
E 15
   */
E 14
I 10
D 11
  public void ipc_set_server_names(String s) {
    server_names=s;
    return;
E 11
I 11
D 13
  public static boolean ipc_set_status_poll_group(String g) {
E 13
I 13
D 15
  public static boolean set_status_poll_group(String g) {
E 15
I 15
  public static boolean set_status_poll_group(String grp) {
E 15
E 13

D 16
    if(first_time=true) {
E 16
I 16
    if(first_time) {
E 16
D 15
      status_poll_group=g;
E 15
I 15
      status_poll_group=grp;
E 15
      return(true);
    } else {
I 16
      Tut.warning("set_status_poll_group call ignored, must be called before init method");
E 16
      return(false);
    }
E 11
  }


//-----------------------------------------------------------------------


I 14
  /**
   *  Sets user status poll result callback (must be called before init method).
D 15
   *  @param u   Instance of class implementing user_status_poll_result.
E 15
I 15
   *  @param u   Callback object (default does nothing).
E 15
   */
E 14
E 10
E 5
I 4
D 11
  public void ipc_set_user_status_poll_callback(user_status_poll_result u) {
    userStatus=u;
    return;
E 11
I 11
D 12
  public boolean ipc_set_user_status_poll_callback(user_status_poll_result u) {
E 12
I 12
D 13
  public static boolean ipc_set_user_status_poll_callback(user_status_poll_result u) {
E 13
I 13
  public static boolean set_user_status_poll_callback(user_status_poll_result u) {
E 13
E 12

D 16
    if(first_time=true) {
E 16
I 16
    if(first_time) {
E 16
      userStatus=u;
      return(true);
    } else {
I 16
      Tut.warning("set_user_status_poll_callback call ignored, must be called before init method");
E 16
      return(false);
    }
E 11
  }


//-----------------------------------------------------------------------
I 7


I 14
  /**
I 21
   *  Sets user control poll callback (must be called before init method).
   *  @param u   Callback object (default does nothing).
   */
  public static boolean set_user_control_callback(user_control_callback u) {

    if(first_time) {
      userControl=u;
      return(true);
    } else {
      Tut.warning("set_user_control_callback call ignored, must be called before init method");
      return(false);
    }
  }


//-----------------------------------------------------------------------


  /**
E 21
   *  Sets quit callback (must be called before init method).
D 15
   *  @param q  Instance of class implementing quit_callback.
E 15
I 15
   *  @param q  Callback object (default closes connection and exits).
E 15
   */
E 14
D 11
  public void ipc_set_quit_callback(quit_callback q) {
    quitCb=q;
    return;
E 11
I 11
D 12
  public boolean ipc_set_quit_callback(quit_callback q) {
E 12
I 12
D 13
  public static boolean ipc_set_quit_callback(quit_callback q) {
E 13
I 13
  public static boolean set_quit_callback(quit_callback q) {
E 13
E 12

D 16
    if(first_time=true) {
E 16
I 16
    if(first_time) {
E 16
      quitCb=q;
      return(true);
    } else {
I 16
      Tut.warning("set_quit_callback call ignored, must be called before init method");
E 16
      return(false);
    }
E 11
  }


//-----------------------------------------------------------------------
I 10


I 14
  /**
D 17
   *  Initializes CLAS standard msg types (normally not called by user code).
E 17
I 17
   *  Initializes CLAS standard message types (normally not called by user code).
E 17
   */
E 14
D 11
  public void ipc_set_status_poll_group(String g) {
    status_poll_group=g;
    return;
E 11
I 11
D 17
  public static boolean init_msg_types() {
E 17
I 17
D 18
  public static boolean init_msg_types() throws Exception {
E 18
I 18
  public static void init_msg_types() throws TipcException {
E 18
E 17
    
    String mtname[] = {    
      "status_poll_result",
      "dbr_request",
      "dbr_reply",
      "dbr_request_rpc",
      "dbr_reply_rpc",
      "tcl_request",
      "tcl_reply",
      "cmlog",
      "dd_event",
      "dd_bosbank",
      "dd_status",
      "run_control",
      "info_server",
      "evt_event",
      "evt_bosbank",
D 23
      "evt_status"
E 23
I 23
      "evt_status",
      "ipc2timeline"
E 23
    };

    try {
      for (int i=0; i<mtname.length; i++) {
	TipcSvc.createMt(mtname[i],i+1,"verbose");
      }
D 18
      return(true);
    } catch (Exception e) {
D 17
      Tut.warning(e);
D 12
      return(true);
E 12
I 12
      return(false);
E 17
I 17
      throw(e);
E 18
I 18
      return;
    } finally {
E 18
E 17
E 12
    }

E 11
  }


//-----------------------------------------------------------------------
E 10
E 7
D 17
// public classes and interfaces
E 17
I 17
// public classes, interfaces, and exceptions
E 17
//-----------------------------------------------------------------------


I 14
  /**
   *  For defining user quit callback.
   */
E 14
E 4
  public interface quit_callback {
I 14
  /**
   *  Called when quit control message received.
   */
E 14
D 4
    void quitCb();
E 4
I 4
D 18
    void quit();
E 18
I 18
    void quit() throws TipcException;
E 18
E 4
  }


//-----------------------------------------------------------------------


I 14
  /**
   *  For defining user status poll result callback.
   */
E 14
  public interface user_status_poll_result {
I 14
  /**
D 15
   *  Sets user status poll callback.
E 15
I 15
   *  Adds user-supplied status poll information.
E 15
   */
E 14
D 4
    void userStatus();
E 4
I 4
D 5
    void addUserStatus();
E 5
I 5
    void addUserStatus(TipcMsg msg);
E 5
E 4
  }


E 3
//-----------------------------------------------------------------------
I 21

  /**
   *  For defining user control callback.
   */
  public interface user_control_callback {
  /**
   *  Adds user-supplied status poll information.
   */
    void process(TipcMsg msg, Object obj);
  }


//-----------------------------------------------------------------------
E 21
I 4
//-----------------------------------------------------------------------
E 4
I 3
// end public interface
E 3
//-----------------------------------------------------------------------
I 4
//-----------------------------------------------------------------------
E 4


I 17
  // causes javadoc to not list constructor
  protected ipc_init() {;}
  

//-----------------------------------------------------------------------

  
E 17
D 12
  private class ipc_control_callback implements TipcProcessCb {
E 12
I 12
D 13
 private static class ipc_control_callback implements TipcProcessCb {
E 13
I 13
 private static class control_callback implements TipcProcessCb {
E 13
E 12

    public void process(TipcMsg msg, Object obj) {

      String str;
    
D 18
      // get string and process
E 18
      try {
I 18

	// get string and process
E 18
	msg.setCurrent(0);
	str=msg.nextStr();
I 10
D 12
	msg.setNumFields(0);           // blank out message so default callback won't barf
E 12
E 10
D 18

E 18
I 18
	
E 18

D 7
	// QUIT...call quit_callback_func 
E 7
I 7
	// QUIT
E 7
	if(str.equals("quit")) {
D 3
	  quit_callback.process();
E 3
I 3
D 4
	  //	  quit_callback.process();
E 4
I 4
D 7
	  ipc_default_quit_callback quitCb = new ipc_default_quit_callback();
E 7
	  quitCb.quit();
I 12
	  msg.setNumFields(0);  // for default cb
E 12
E 4
E 3
	}
	
	
D 7
	// STATUS_POLL...send status_poll_result to "monitor" group 
E 7
I 7
	// STATUS_POLL
E 7
	else if(str.equals("status_poll")) {
D 13
	  ipc_send_status_poll_result();
E 13
I 13
	  send_status_poll_result();
E 13
I 12
	  msg.setNumFields(0);  // for default cb
E 12
	}
	
	
D 7
	// RECONNECT 
E 7
I 7
D 13
	// RECONNECT ???
E 13
I 13
	// RECONNECT
E 13
E 7
	else if(str.equals("reconnect")) {
D 13
	  Tut.warning("Unable to reconnect");
E 13
I 13
	  ((TipcSrv)obj).destroy();
	  ((TipcSrv)obj).create(TipcSrv.CONN_FULL);
E 13
I 12
	  msg.setNumFields(0);  // for default cb
E 12
	}
	
D 12
	
	// UNKNOWN
	else {
	  System.out.println("Unknown command: " + str);
	}
E 12
I 12

I 21
	// user control message callback
	else {
	    userControl.process(msg,obj);
	}


E 21
	// unknown...handled by default callback
E 12
D 13
	
E 13
I 13
	msg.setCurrent(0);
	return;

E 13
D 18

E 18
      } catch (TipcException e) {
D 18
	Tut.warning(e);
E 18
I 18
	Tut.warning("Unable to process control message: " + e);
E 18
      }
D 18
      
      return;
E 18
I 18

E 18
    }
D 6
  }    
E 6
I 6
  }
E 6


//-----------------------------------------------------------------------


D 3
  private void ipc_default_quit_callback() {
E 3
I 3
D 12
  private class ipc_default_quit_callback implements quit_callback {
E 12
I 12
D 13
  private static class ipc_default_quit_callback implements quit_callback {
E 13
I 13
  private static class default_quit_callback implements quit_callback {
E 13
E 12
E 3

D 3
    System.out.println("\n*** Default quit callback called...received QUIT control message\n"
		       + "...destroying connection to server ***\n");
    ipc_close();
    Tut.exitSuccess();
E 3
I 3
D 4
    public void quitCb() {
E 4
I 4
D 18
    public void quit() {
E 18
I 18
    public void quit() throws TipcException {
E 18
E 4
      System.out.println("\n*** Default quit callback called...received QUIT control message\n"
			 + "...destroying connection to server ***\n");
D 13
      ipc_close();
E 13
I 13
D 17
      close();
E 13
      Tut.exitSuccess();
E 17
I 17
      try {
	close();
	Tut.exitSuccess();
D 18
      } catch (Exception e) {
	Tut.exitFailure(e.toString());	
E 18
I 18
      } finally {
E 18
      }
E 17
    }
E 3
  }
D 18

E 18
I 18
  
E 18

//-----------------------------------------------------------------------


D 12
  private void ipc_send_status_poll_result() {
E 12
I 12
D 13
  private static void ipc_send_status_poll_result() {
E 13
I 13
D 18
  private static void send_status_poll_result() {
E 18
I 18
  private static void send_status_poll_result() throws TipcException {
E 18
E 13
E 12


    try {

      // increment request counter 
      num_status_poll_req++;
      
      
      // create message 
      TipcMsg msg = TipcSvc.createMsg(TipcSvc.lookupMt("status_poll_result"));
      
      
      // set destination 
      msg.setDest(status_poll_group);
      
      
      // append unique_subject 
      msg.appendStr(Tut.getOptionStr("ss.unique_subject"));
      
      
      // append username 
      msg.appendStr(username);
      
      
D 8
      // append nodename 
      msg.appendStr(nodename);
E 8
I 8
      // append hostname
      msg.appendStr(hostname);
E 8
      
      
      // append pid 
      msg.appendInt4(pid);
      
      
      // append start time 
D 5
      msg.appendInt4((int)(start/1000.0));
E 5
I 5
      msg.appendInt4((int)start);
E 5
      
      
      // append current time 
D 5
      msg.appendInt4((int)(Tut.getWallTime()/1000.0));
E 5
I 5
      msg.appendInt4((int)Tut.getWallTime());
E 5
      
      
D 5
      // append cputime
E 5
I 5
      // append cputime ???
E 5
D 3
      msg.appendReal4(0.0);
E 3
I 3
      msg.appendReal4((float)0.0);
E 3
      
      
      // append number of responses so far
      msg.appendInt4(num_status_poll_req);
      
      
      // append server node 
D 25
      msg.appendStr(server.getServerName());
E 25
I 25
      msg.appendStr(server.getUniqueSubject());
E 25
      
      
      // append system end key 
      msg.appendStr("end_system_data");
      
      
D 5
      // add extra user-defined information 
D 3
      if(user_status_poll_callback_func!=null){
	user_status_poll_callback_func.process();
      }
E 3
I 3
D 4
      //      if(user_status_poll_callback_func!=null){
      //	user_status_poll_callback_func.process();
      //      }
E 4
I 4
      //      ipc_default_user_status userStatus = new ipc_default_user_status();
      userStatus.addUserStatus();
E 5
I 5
      // add user-defined information 
      userStatus.addUserStatus(msg);
E 5
E 4
E 3
      
      
      // append user end key 
      msg.appendStr("end_user_data");
      
      
      // ship the message off 
      server.send(msg);
      server.flush();
I 18
      return;
E 18
      
I 3
D 18

E 3
    } catch (Exception e) {
      Tut.warning(e);
E 18
I 18
    } finally {
E 18
    }
D 18
    
    return;
E 18
I 18

E 18
  }

I 4

//-----------------------------------------------------------------------


D 12
  private class ipc_default_user_status implements user_status_poll_result {
E 12
I 12
D 13
  private static class ipc_default_user_status implements user_status_poll_result {
E 13
I 13
  private static class default_user_status implements user_status_poll_result {
E 13
E 12
    
D 5
    public void addUserStatus() {
E 5
I 5
    public void addUserStatus(TipcMsg msg) {
E 5
      return;
    }
  }

I 7

//-----------------------------------------------------------------------
I 21


  private static class default_user_control implements user_control_callback {

    public void process(TipcMsg msg, Object obj) {
      return;
    }
  }


//-----------------------------------------------------------------------
E 21
// for testing
//-----------------------------------------------------------------------


I 16
  /**
   * For testing only.
   */
E 16
I 14
D 15
  /**
   *  For testing only.
   */
E 15
E 14
  public static void main(String[] args) {

D 17
    class mystatus implements user_status_poll_result {
      public void addUserStatus(TipcMsg msg) {
	msg.appendStr("Hello");
	msg.appendStr("World");
	msg.appendStr("integer");
	msg.appendInt4(123);
	return;
E 17
I 17
    try {

      class mystatus implements user_status_poll_result {
	public void addUserStatus(TipcMsg msg) {
	  msg.appendStr("Hello");
	  msg.appendStr("World");
	  msg.appendStr("integer");
	  msg.appendInt4(123);
	  return;
	}
E 17
      }
D 17
    }
  
    class myquitCb implements quit_callback {
      public void quit() {
D 10
	System.out.println("hello");
E 10
I 10
D 12
	System.out.println("I'm quitting");
E 12
I 12
	System.out.println("Test quit callback called...");
D 13
	ipc_close();
E 13
I 13
	close();
E 13
	Tut.exitSuccess();
E 12
E 10
	return;
E 17
I 17
      
      class myquitCb implements quit_callback {
	public void quit() {
	  System.out.println("Test quit callback called...");
	  try {
	    close();
	    Tut.exitSuccess();
	  } catch (Exception e) {
	    Tut.exitFailure(e.toString());
	  }
	}
      }
      
      
I 21
      System.out.println("Starting ipc_init test");

E 21
      ipc_init.set_user_status_poll_callback(new mystatus());
      ipc_init.set_quit_callback(new myquitCb());
D 24
      ipc_init.set_server_names("clon10,clon00,jlabs2,db6");
E 24
I 24
D 27
      ipc_init.set_server_names("clon10,clon00,clon05");
E 27
I 27
      ipc_init.set_server_names("clondb1,clondb2");
E 27
E 24
D 21
      ipc_init.init("java_test","testing java ipc_init");
      //    ipc_init.set_server_names("clon10,clon00,jlabs2,db6");
E 21
I 21
      TipcSrv srv = ipc_init.init("java_test","testing java ipc_init");
D 25
      srv.setSubjectSubscribe("/*", true);
E 25
D 24
      //      ipc_init.set_server_names("clon10,clon00,jlabs2,db6");
E 24
E 21

      while(true) {
	ipc_init.check(10.0);
E 17
      }
I 17
      
    } catch (Exception e) {
      Tut.exitFailure(e.toString());
E 17
    }
D 17
  
D 15

E 15
D 10
    ipc_init ipc = new ipc_init("java_test","testing");
E 10
I 10
D 11
    ipc_init ipc = new ipc_init("java_test","testing java ipc_init");
E 11
I 11
D 14
    ipc_init ipc = new ipc_init();
E 11
E 10
D 13
    ipc.ipc_set_user_status_poll_callback(new mystatus());
    ipc.ipc_set_quit_callback(new myquitCb());
E 13
I 13
    ipc.set_user_status_poll_callback(new mystatus());
    ipc.set_quit_callback(new myquitCb());
    ipc.set_server_names("clon10,clon00,jlabs2,db6");
E 13
I 11
    ipc.init("java_test","testing java ipc_init");
E 14
I 14
    ipc_init.set_user_status_poll_callback(new mystatus());
    ipc_init.set_quit_callback(new myquitCb());
    ipc_init.set_server_names("clon10,clon00,jlabs2,db6");
    ipc_init.init("java_test","testing java ipc_init");
I 16
    ipc_init.set_server_names("clon10,clon00,jlabs2,db6");
E 17
E 16
E 14
E 11
    
D 17
    while(true) {
D 13
      ipc.ipc_check(10.0);
E 13
I 13
D 14
      ipc.check(10.0);
E 14
I 14
      ipc_init.check(10.0);
E 14
E 13
    }
	  
E 17
  }
D 17


E 17
I 17
  
  
E 17
E 7
E 4
}
//-----------------------------------------------------------------------------
D 4
//  end class definition
E 4
I 4
D 5
//  end top level class definition
E 5
I 5
//  end main class definition
E 5
E 4
//-----------------------------------------------------------------------------
I 7
D 11



E 7

E 11
E 1
