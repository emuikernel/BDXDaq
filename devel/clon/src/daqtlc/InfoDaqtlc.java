import java.awt.*;
import javax.swing.*;
import symantec.itools.multimedia.*;
import com.smartsockets.*;
import clon.ipcutil.*;
import clon.util.*;

public class InfoDaqtlc extends Frame implements TipcProcessCb, Runnable
{
    public static InfoDaqtlc myobj;
  public  SysExe mySysExe = new SysExe();
  public Label mytext1;
  public TextArea message;
  public boolean ts_flag = false;
  public boolean eber_flag = false;
  public boolean roc_flag = false;

  public int ebbuf [] = new int[32];
  
  private static final long mek = 1;

  public static TipcSrv srv; 

    private static String _clonparms;

    private static Jgetenv myJgetenv;

  //constructor
  public InfoDaqtlc () {
    super("DAQTLC message window");
    setLayout( null );
    setSize(580,600);
    setLocation(300,100);
    setVisible( false );

	myJgetenv = new Jgetenv();
	try {
        _clonparms = myJgetenv.echo("CLON_PARMS");
	} catch (JgetenvException e) { }
    
    //button with image
    Icon myicon = new ImageIcon(_clonparms+"/daqtlc/boy.gif"); 
    JButton mybutton = new JButton(myicon);
    mybutton.setBounds(24,12,132,132);
    mybutton.setBorderPainted(false);
    mybutton.setBackground( new Color(195,195,195));
    add(mybutton);

    //running text    
    ScrollingText scrollingText1 = new ScrollingText(); 
    try { 
      { 
	String[] tempString = new String[1]; 
	tempString[0] = "Do not Panic !!!"; 
	scrollingText1.setMessageList(tempString); 
      } 
    } 
    catch(java.beans.PropertyVetoException e) { } 
    scrollingText1.setForeground(new Color(0,128,128)); 
    scrollingText1.setFont(new Font("Serif", Font.PLAIN, 24)); 
    scrollingText1.setBounds(0,156,560,18); 
    add(scrollingText1); 
    
    //title text
    mytext1 = new Label();
    mytext1.setFont(new Font("Serif", Font.ITALIC, 32)); 
    mytext1.setBackground( new Color(195,195,195)); 
    mytext1.setForeground(Color.magenta); 
    mytext1.setBounds(168,24,299,96); 
    add(mytext1);
    
    //main meessage text
    message = new TextArea();
    message.setFont(new Font("Serif", Font.ITALIC, 18)); 
    message.setBackground( new Color(195,195,195)); 
    message.setForeground(Color.blue);
    message.setEditable( false );
    message.setBounds(12,192,560,400);
    add(message);


    //    message.setText(DataPool.roc_help_text);



    try {
	System.out.println("we got the server");
      // subscribe to subjects
      srv.setSubjectSubscribe("info_server/in/hardware_monitor/ts", true);
      //boy srv.setSubjectSubscribe("info_server/in/hardware_monitor/roc", true);
      srv.setSubjectSubscribe("info_server/in/hardware_monitor/eber", true);

      
      // create message types and callbacks
      srv.addProcessCb(this,"info_server/in/...",null);
      
      // flush all messages
      srv.flush();
      
    } catch (TipcException e) {
      Tut.fatal(e);
    }
  }

  

  // run method for IPC mainloop
  public void run() {
    System.out.println("Ipc loop is active");    
    try {
      ipc_init.check(TipcDefs.TIMEOUT_FOREVER);
    }
    catch (Exception e) {}
  }

  // message process callback
  public void process(TipcMsg msg, Object arg) {
      String s = null;
      try {
	msg.setCurrent(0);
	s = msg.nextStr();
      } catch (TipcException e) {} 	
	System.out.println(s);
	if(s.equals("hardware_monitor/ts")) alarm_ts(msg);
	//boy else if(s.equals("hardware_monitor/roc")) alarm_roc(msg);
	else if(s.equals("hardware_monitor/eber")) alarm_eber(msg);
	System.out.println("Debug printout");
	close_ui();

  }


  public void close_ui() {
      System.out.println(ts_flag + " " + roc_flag + " " +eber_flag);
    if ( ts_flag == false && roc_flag == false && eber_flag == false)
      myobj.setVisible( false );
    return;
  }
  
  public void alarm_ts(TipcMsg msg) {
    String ts_status = null;
    try {
      ts_status = msg.nextStr();
      if(ts_status.equals("UNDEFINED")) {
	ts_flag = true;
	mytext1.setText("TS_Problem");       
	message.setText(DataPool.ts_help_text);
	if(myobj.isVisible() == false) 
	  {
	    myobj.setVisible( true );	
	    mySysExe.run_unix("door_bell");
	  }
      } else ts_flag = false;
    } catch (TipcException e) { System.out.println(e); return;  }
    return;
  }
  
  
  public void alarm_roc(TipcMsg msg) {
    int roc_status [] = new int[39];
    int global_status = 0;
    int i;
    try {
      roc_status = msg.nextInt4Array();
    } catch (TipcException e) { System.out.println(e); return;}
    for (i=0;i<=31;i++){ 
	if(roc_status[i] == -1) {
	    if (((mek << i)&DataPool.ts_rocenable)!=0) global_status = -1;
	    /*
	    if(DataPool.ts_rocbr_def[i].equals("not used")) { }
	       else global_status = -1;
	    */

	}
    }
    if(global_status <0 ) {
    System.out.println( " Hoplia "+i+ " " +roc_status[i]);
      roc_flag = true;
    mytext1.setText("ROC_Problem");       
    message.setText(DataPool.roc_help_text);
    if(myobj.isVisible() == false)
      {
       	myobj.setVisible( true );	
	mySysExe.run_unix("door_bell");
      }
    } else roc_flag = false;
    return;
  }
  

  public void alarm_eber(TipcMsg msg) {
    String er_status = null;
    String eb_status = null;
    try {
	String a = msg.nextStr();
	eb_status = msg.nextStr();

      int eb_numevt = msg.nextInt4();
      double eb_evtrate = msg.nextReal8();
      double eb_datarate = msg.nextReal8();
      ebbuf = msg.nextInt4Array();

      er_status = msg.nextStr();	 
    } catch (TipcException e) {  System.out.println(e); return;}
    if((eb_status.equals("UNDEFINED")) || (er_status.equals("UNDEFINED"))) {
      eber_flag = true;
      mytext1.setText("EBER_Problem");       
      message.setText(DataPool.eber_help_text);
      if(myobj.isVisible() == false) 
	{
	  myobj.setVisible( true );	
	  mySysExe.run_unix("door_bell");
	}
    } else eber_flag = false;
    return;
  }
 
   public static void main(String argv[]) {
    //...........................initializing the IPC
    try {
      ipc_init.set_project("clasprod");
      ipc_init.set_server_names("clon10,clon00,clon05");
      srv=ipc_init.init("infodaqtlc","infodaqtlc");
    } catch (Exception e) {
      Tut.exitFailure(e.toString());
    }
    new Thread ( myobj = new InfoDaqtlc() ).start();
    myobj.pack();

  }

}

