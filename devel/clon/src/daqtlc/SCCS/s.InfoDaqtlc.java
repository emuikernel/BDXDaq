h56459
s 00003/00003/00230
d D 1.12 04/02/03 20:49:42 boiarino 14 13
c *** empty log message ***
e
s 00000/00003/00233
d D 1.11 02/07/16 09:43:10 wolin 13 12
i 11
c Accepted child'\''s version in workspace "/usr/local/clas/devel/source".
c 
e
s 00003/00001/00230
d D 1.10 02/05/09 17:28:06 gurjyan 12 10
c *** empty log message ***
e
s 00003/00001/00230
d D 1.9.1.1 02/05/09 16:37:40 gurjyan 11 10
c *** empty log message ***
e
s 00001/00001/00230
d D 1.9 02/04/18 14:40:39 gurjyan 10 9
c typo
c 
e
s 00008/00001/00223
d D 1.8 02/04/18 14:35:10 gurjyan 9 8
c added Jgetenv
c 
e
s 00004/00001/00220
d D 1.7 01/10/30 15:44:25 gurjyan 8 7
c *** empty log message ***
e
s 00003/00003/00218
d D 1.6 01/10/30 13:40:35 gurjyan 7 6
c *** empty log message ***
e
s 00001/00001/00220
d D 1.5 01/10/23 09:06:08 gurjyan 6 5
c ipc_init with infodaqtlc
c 
e
s 00001/00001/00220
d D 1.4 01/10/23 08:59:35 gurjyan 5 4
c only activehelp
c 
e
s 00001/00001/00220
d D 1.3 01/10/22 10:03:34 gurjyan 4 3
c *** empty log message ***
e
s 00000/00000/00221
d D 1.2 01/10/19 13:42:36 gurjyan 3 1
c *** empty log message ***
e
s 00000/00000/00000
d R 1.2 01/10/19 13:42:19 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 daqtlc/InfoDaqtlc.java
e
s 00221/00000/00000
d D 1.1 01/10/19 13:42:18 gurjyan 1 0
c 
e
u
U
f e 0
t
T
I 1
import java.awt.*;
import javax.swing.*;
import symantec.itools.multimedia.*;
import com.smartsockets.*;
import clon.ipcutil.*;
I 11
import clon.util.*;
E 11
I 12
D 13
import clon.util.*;
E 13
E 12

D 11
D 12

E 11
E 12
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

I 9
    private static String _clonparms;

    private static Jgetenv myJgetenv;

E 9
  //constructor
  public InfoDaqtlc () {
    super("DAQTLC message window");
    setLayout( null );
    setSize(580,600);
    setLocation(300,100);
    setVisible( false );
I 9

	myJgetenv = new Jgetenv();
I 11
	try {
E 11
I 12
D 13
	try {
E 13
E 12
D 10
        _clonlog = myJgetenv.echo("CLON_PARMS");
E 10
I 10
        _clonparms = myJgetenv.echo("CLON_PARMS");
I 11
D 13
	} catch (JgetenvException e) { }
E 13
E 11
I 12
	} catch (JgetenvException e) { }
E 12
E 10
E 9
    
    //button with image
D 9
    Icon myicon = new ImageIcon("/usr/local/clas/parms/daqtlc/boy.gif"); 
E 9
I 9
    Icon myicon = new ImageIcon(_clonparms+"/daqtlc/boy.gif"); 
E 9
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
D 14
      srv.setSubjectSubscribe("info_server/in/hardware_monitor/roc", true);
E 14
I 14
      //boy srv.setSubjectSubscribe("info_server/in/hardware_monitor/roc", true);
E 14
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
D 14
	else if(s.equals("hardware_monitor/roc")) alarm_roc(msg);
E 14
I 14
	//boy else if(s.equals("hardware_monitor/roc")) alarm_roc(msg);
E 14
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
I 8
	    if (((mek << i)&DataPool.ts_rocenable)!=0) global_status = -1;
	    /*
E 8
D 7
	    if (((mek << i)&DataPool.ts_rocenable)!=0) {
		global_status = -1;
	    }
E 7
I 7
	    if(DataPool.ts_rocbr_def[i].equals("not used")) { }
	       else global_status = -1;
D 8
	    
E 8
I 8
	    */

E 8
E 7
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
D 14
      ipc_init.set_server_names("clon10,clon00,jlabs2,db6");
E 14
I 14
      ipc_init.set_server_names("clon10,clon00,clon05");
E 14
D 4
      srv=ipc_init.init(null,"ActiveHelp");
E 4
I 4
D 5
      srv=ipc_init.init("ActiveMan","ActiveHelp");
E 5
I 5
D 6
      srv=ipc_init.init("ActiveHelp","ActiveHelp");
E 6
I 6
      srv=ipc_init.init("infodaqtlc","infodaqtlc");
E 6
E 5
E 4
    } catch (Exception e) {
      Tut.exitFailure(e.toString());
    }
    new Thread ( myobj = new InfoDaqtlc() ).start();
    myobj.pack();

  }

}

E 1
