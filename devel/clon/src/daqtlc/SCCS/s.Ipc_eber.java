h19206
s 00001/00001/00096
d D 1.3 04/02/03 20:49:14 boiarino 4 3
c *** empty log message ***
e
s 00002/00000/00095
d D 1.2 01/10/19 13:25:12 gurjyan 3 1
c test
e
s 00000/00000/00000
d R 1.2 70/01/01 00:00:02 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 daqtlc/Ipc_eber.java
e
s 00095/00000/00000
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
import com.smartsockets.*;
import clon.ipcutil.*;

/** Class Ipc is designed to read the DataPool class variables and broadcast IPC smartsoket messages.
 * @author Vardan H Gyurjyan. 11.19.99 CLAS Online Group.
 */
public class Ipc_eber implements Runnable {
  
  public  TipcSrv eber_srv;
  public   TipcMsg eber_msg;
  
  public EbEr myEbEr = new EbEr();
  
  public static void main(String argv[]) {
    new Thread( new Ipc_eber()).start();
  }
  
   /** 
   * run method of the thread
   */
  public void run() {
    
    //...........................initializing the IPC
    try {
      ipc_init.set_project("clasprod");
D 4
      ipc_init.set_server_names("clon10,clon00,jlabs2,db6");
E 4
I 4
      ipc_init.set_server_names("clon10,clon00,clon05");
E 4
      TipcSrv srv=ipc_init.init("hardware_monitor_eber","Daqtlc");
    } catch (Exception e) {
      Tut.exitFailure(e.toString());
    }
    
    //...........................starting the EbEr monitoring thread
    
    new Thread (myEbEr).start();
    //...........................loop for accesing the datat form the DataPool. 
    //...........................This loop is async with the TS monitoring thread
    
    for(;;) {

      System.out.println( "EbEr_update  "+DataPool.eber_update); 
      if(DataPool.eber_update == true ) {      
	//.........................organizing the message
	//.........................get the server      
	eber_srv = TipcSvc.getSrv();
	//.........................create message 
	eber_msg = TipcSvc.createMsg(TipcSvc.lookupMt("info_server"));
	//.........................set destination 
	eber_msg.setDest("info_server/in/hardware_monitor/eber");
	eber_msg.appendStr("hardware_monitor/eber");
	//.........................appending
I 3
	try {
E 3
	eber_msg.appendStr(DataPool.CODA_config);
	eber_msg.appendStr(DataPool.eb_status);
	eber_msg.appendInt4(DataPool.eb_numevt);
	eber_msg.appendReal8(DataPool.eb_evtrate);
	eber_msg.appendReal8(DataPool.eb_datarate);
	eber_msg.appendInt4Array(DataPool.ebbuf);

	eber_msg.appendStr(DataPool.er_status);
	eber_msg.appendInt4(DataPool.er_numevt);
	eber_msg.appendReal8(DataPool.er_evtrate);
	eber_msg.appendReal8(DataPool.er_datarate);
I 3
	} catch (java.lang.NullPointerException ne) { }
E 3
	//.........................sending
	try {
	  System.out.println("sending EBER message");
	  eber_srv.send(eber_msg);
	  eber_srv.flush();
	} catch (TipcException te) {
	  Tut.warning(te);
	}
	/*	
    System.out.println("EB");
    System.out.println("------");
    System.out.println(DataPool.CODA_config);
    System.out.println(DataPool.eb_status);
    System.out.println(DataPool.eb_numevt);
    System.out.println(DataPool.eb_evtrate);
    System.out.println(DataPool.eb_datarate);
    
    System.out.println("ER");
    System.out.println("------");
    System.out.println(DataPool.er_status);
    System.out.println(DataPool.er_numevt);
    System.out.println(DataPool.er_evtrate);
    System.out.println(DataPool.er_datarate);
    */
	DataPool.eber_update = false;
      }
    try {
      ipc_init.check((double)DataPool.ipc_sleep);
    }
    catch (Exception e) {}
    }
  }
}
E 1
