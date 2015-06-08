//  ipc_init.java
//    
//   standard initializion of Smartsockets IPC for CLAS
//
//  main method at end for testing
//
//  EJW, 15-sep-99


package clonjava.clon.ipcutil;
import java.net.*;
import com.smartsockets.*;


//-----------------------------------------------------------------------------
//  begin main class definition
//-----------------------------------------------------------------------------

/**
 * Initializes CLAS standard SmartSockets IPC settings.  Subscribes to standard 
 * subjects, sets default quit and user status poll callback, connects to server,
 * etc.  Should be called by all java ipc clients.
 *
 * @version 1.0
 * @author E.Wolin
 */
public abstract class ipc_init {


  // defaults for user-defined stuff...can only be changed before init method called
  private static String project              	    = "clastest";
  private static String server_names         	    = "clondb1,clondb2";
  private static String status_poll_group    	    = "clas_monitor";
  private static quit_callback quitCb        	    = new default_quit_callback();
  private static user_status_poll_result userStatus = new default_user_status();
  private static user_control_callback userControl  = new default_user_control();

  // other defaults
  private static String hostname             	    = null;
  private static String username             	    = System.getProperty("user.name");
  private static int pid                     	    = 0;
  private static control_callback ctrlCb            = new control_callback();

       
  // misc 	 
  private static boolean first_time          	    = true;
  private static double start                	    = Tut.getWallTime();
  private static int num_status_poll_req     	    = 0;   
  private static TipcSrv server;


//-----------------------------------------------------------------------
//  public methods
//-----------------------------------------------------------------------


  /**
   *  Main initialization routine, sets options, gets server, etc.
   *  Passes all exceptions up to caller.
   *
   *  @param uniq_sub      unique_subject.
   *  @param ident_string  identification string (arbitrary).
   */
  public static synchronized TipcSrv init(String uniq_sub, String ident_string) 
  throws TipcException {

    if(first_time) {

      try {

	// get local host name
	try {
//sergey: if hostname returns 'clon10' without '.jlab.org', then
// 'temp.substring' below fails; we will add dot in the end .. 
	  String temp=(InetAddress.getLocalHost()).getHostName() + ".";
	  if(temp!=null) {
        //System.out.println("\nnot null: \n" + temp);
	    hostname=temp.substring(0,temp.indexOf("."));
        //System.out.println("\nhostname: \n" + hostname);
	  } else {
	    hostname="java_client";
	  }
	} catch (UnknownHostException e) {
	  hostname="unknown";
	}


	// set server names
	Tut.setOption("ss.server_names",server_names);
	
	
	// set project
	Tut.setOption("ss.project",project);
	
	
	// set unique subject
	if(uniq_sub!=null)Tut.setOption("ss.unique_subject",uniq_sub);
	
	
	// set ident string
	if(ident_string!=null)TipcMon.setIdentStr(ident_string);
	
	
	// subscribe to standard subjects
	Tut.setOption("ss.subjects","_all," + project + ",_" + hostname);


	// enable control commands
	Tut.setOption("ss.enable_control_msgs","quit,disconnect,connect");
	
	
	// create clas standard message types
	init_msg_types();
	
	
	// get server and connect
	server=TipcSvc.getSrv();
	server.create(TipcSrv.CONN_FULL);
	
	
	// create new control message callback at higher priority (10) than default (0)
	server.addProcessCb(ctrlCb,TipcSvc.lookupMt(TipcMt.CONTROL),(short)10,server);
	

	// flush all pending messages
	server.flush();


	// everything worked
	first_time=false;
	return(server);

      } finally {
      }

      
    } else {
      return(server);
    }
  }


//-----------------------------------------------------------------------


  /**
   *  Processes IPC messages.
   *  @param timeout   Timeout in seconds.
   */
  public static void check(double timeout) throws TipcException {

    try {
      server.mainLoop(timeout);
      return;
    } finally {
    }
  }


//-----------------------------------------------------------------------


  /**
   *  Closes connection to server.
   */
  public static void close() throws TipcException {

    System.out.println("\nipc_init.close called...closing connection\n");
    try {
      server.destroy();
      return;
    } finally {
    }
  }


//-----------------------------------------------------------------------


  /**
   *  Sets project (must be called before init method).
   *  @param proj   Project name (default "clastest").
   */
  public static boolean set_project(String proj) {

    if(first_time) {
      project=proj;
      return(true);
    } else {
      Tut.warning("set_project call ignored must be called before init method");
      return(false);
    }
  }


//-----------------------------------------------------------------------


  /**
   *  Sets server names (must be called before init method).
   *  @param srvs  Server name list (default "db6").
   */
  public static boolean set_server_names(String srvs) {

    if(first_time) {
      server_names=srvs;
      return(true);
    } else {
      Tut.warning("set_server_names call ignored, must be called before init method");
      return(false);
    }
  }


//-----------------------------------------------------------------------


  /**
   *  Sets status poll group (must be called before init method).
   *  @param grp   Status poll group name (default "clas_monitor").
   */
  public static boolean set_status_poll_group(String grp) {

    if(first_time) {
      status_poll_group=grp;
      return(true);
    } else {
      Tut.warning("set_status_poll_group call ignored, must be called before init method");
      return(false);
    }
  }


//-----------------------------------------------------------------------


  /**
   *  Sets user status poll result callback (must be called before init method).
   *  @param u   Callback object (default does nothing).
   */
  public static boolean set_user_status_poll_callback(user_status_poll_result u) {

    if(first_time) {
      userStatus=u;
      return(true);
    } else {
      Tut.warning("set_user_status_poll_callback call ignored, must be called before init method");
      return(false);
    }
  }


//-----------------------------------------------------------------------


  /**
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
   *  Sets quit callback (must be called before init method).
   *  @param q  Callback object (default closes connection and exits).
   */
  public static boolean set_quit_callback(quit_callback q) {

    if(first_time) {
      quitCb=q;
      return(true);
    } else {
      Tut.warning("set_quit_callback call ignored, must be called before init method");
      return(false);
    }
  }


//-----------------------------------------------------------------------


  /**
   *  Initializes CLAS standard message types (normally not called by user code).
   */
  public static void init_msg_types() throws TipcException {
    
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
      "evt_status",
      "ipc2timeline"
    };

    try {
      for (int i=0; i<mtname.length; i++) {
	TipcSvc.createMt(mtname[i],i+1,"verbose");
      }
      return;
    } finally {
    }

  }


//-----------------------------------------------------------------------
// public classes, interfaces, and exceptions
//-----------------------------------------------------------------------


  /**
   *  For defining user quit callback.
   */
  public interface quit_callback {
  /**
   *  Called when quit control message received.
   */
    void quit() throws TipcException;
  }


//-----------------------------------------------------------------------


  /**
   *  For defining user status poll result callback.
   */
  public interface user_status_poll_result {
  /**
   *  Adds user-supplied status poll information.
   */
    void addUserStatus(TipcMsg msg);
  }


//-----------------------------------------------------------------------

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
//-----------------------------------------------------------------------
// end public interface
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------


  // causes javadoc to not list constructor
  protected ipc_init() {;}
  

//-----------------------------------------------------------------------

  
 private static class control_callback implements TipcProcessCb {

    public void process(TipcMsg msg, Object obj) {

      String str;
    
      try {

	// get string and process
	msg.setCurrent(0);
	str=msg.nextStr();
	

	// QUIT
	if(str.equals("quit")) {
	  quitCb.quit();
	  msg.setNumFields(0);  // for default cb
	}
	
	
	// STATUS_POLL
	else if(str.equals("status_poll")) {
	  send_status_poll_result();
	  msg.setNumFields(0);  // for default cb
	}
	
	
	// RECONNECT
	else if(str.equals("reconnect")) {
	  ((TipcSrv)obj).destroy();
	  ((TipcSrv)obj).create(TipcSrv.CONN_FULL);
	  msg.setNumFields(0);  // for default cb
	}
	

	// user control message callback
	else {
	    userControl.process(msg,obj);
	}


	// unknown...handled by default callback
	msg.setCurrent(0);
	return;

      } catch (TipcException e) {
	Tut.warning("Unable to process control message: " + e);
      }

    }
  }


//-----------------------------------------------------------------------


  private static class default_quit_callback implements quit_callback {

    public void quit() throws TipcException {
      System.out.println("\n*** Default quit callback called...received QUIT control message\n"
			 + "...destroying connection to server ***\n");
      try {
	close();
	Tut.exitSuccess();
      } finally {
      }
    }
  }
  

//-----------------------------------------------------------------------


  private static void send_status_poll_result() throws TipcException {


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
      
      
      // append hostname
      msg.appendStr(hostname);
      
      
      // append pid 
      msg.appendInt4(pid);
      
      
      // append start time 
      msg.appendInt4((int)start);
      
      
      // append current time 
      msg.appendInt4((int)Tut.getWallTime());
      
      
      // append cputime ???
      msg.appendReal4((float)0.0);
      
      
      // append number of responses so far
      msg.appendInt4(num_status_poll_req);
      
      
      // append server node 
      msg.appendStr(server.getUniqueSubject());
      
      
      // append system end key 
      msg.appendStr("end_system_data");
      
      
      // add user-defined information 
      userStatus.addUserStatus(msg);
      
      
      // append user end key 
      msg.appendStr("end_user_data");
      
      
      // ship the message off 
      server.send(msg);
      server.flush();
      return;
      
    } finally {
    }

  }


//-----------------------------------------------------------------------


  private static class default_user_status implements user_status_poll_result {
    
    public void addUserStatus(TipcMsg msg) {
      return;
    }
  }


//-----------------------------------------------------------------------


  private static class default_user_control implements user_control_callback {

    public void process(TipcMsg msg, Object obj) {
      return;
    }
  }


//-----------------------------------------------------------------------
// for testing
//-----------------------------------------------------------------------


  /**
   * For testing only.
   */
  public static void main(String[] args) {

    try {

      class mystatus implements user_status_poll_result {
	public void addUserStatus(TipcMsg msg) {
	  msg.appendStr("Hello");
	  msg.appendStr("World");
	  msg.appendStr("integer");
	  msg.appendInt4(123);
	  return;
	}
      }
      
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
      
      
      System.out.println("Starting ipc_init test");

      ipc_init.set_user_status_poll_callback(new mystatus());
      ipc_init.set_quit_callback(new myquitCb());
      ipc_init.set_server_names("clondb1,clondb2");
      TipcSrv srv = ipc_init.init("java_test","testing java ipc_init");

      while(true) {
	ipc_init.check(10.0);
      }
      
    } catch (Exception e) {
      Tut.exitFailure(e.toString());
    }
    
  }
  
  
}
//-----------------------------------------------------------------------------
//  end main class definition
//-----------------------------------------------------------------------------
