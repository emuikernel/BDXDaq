h10364
s 00002/00002/00091
d D 1.4 03/04/17 17:04:06 boiarino 5 4
c minor
e
s 00001/00000/00092
d D 1.3 00/10/18 12:44:51 gurjyan 4 3
c 
e
s 00001/00000/00091
d D 1.2 00/09/05 16:33:31 gurjyan 3 1
c 
e
s 00000/00000/00000
d R 1.2 70/01/01 00:00:02 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 daqtlc/Ipc_ts.java
e
s 00091/00000/00000
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
public class Ipc_ts implements Runnable {
  
  public  TipcSrv ts_srv;
  public   TipcMsg ts_msg;
  
  
  public Ts myTs = new Ts();
  
  public static void main(String argv[]) {
    new Thread ( new Ipc_ts() ).start();
    
  }
  
  
  /** 
   * run method of the thread
   */
  public void run() {
    
    //...........................initializing the IPC
    try {
      ipc_init.set_project("clasprod");
D 5
      ipc_init.set_server_names("clon10,clon00,jlabs2,db6");
E 5
I 5
      ipc_init.set_server_names("clon10,clon00,clon05");
E 5
      TipcSrv srv=ipc_init.init("hardware_monitor_ts","Daqtlc");
    } catch (Exception e) {
      Tut.exitFailure(e.toString());
    }
    
    //...........................starting the Ts monitoring thread
    new Thread  (myTs).start();
    
    //...........................loop for accesing the datat form the DataPool. 
    //...........................This loop is async with the TS monitoring thread
    for(;;) {
      System.out.println( "TS_update  "+DataPool.ts_update);
      if(DataPool.ts_update == true) {
	//.........................organizing the message
	//.........................get the server      
	ts_srv = TipcSvc.getSrv();
	//.........................create message 
	ts_msg = TipcSvc.createMsg(TipcSvc.lookupMt("info_server"));
	//.........................set destination 
	ts_msg.setDest("info_server/in/hardware_monitor/ts");
	ts_msg.appendStr("hardware_monitor/ts");
	//.........................appending
I 3

E 3
	ts_msg.appendStr(DataPool.ts_status);
	ts_msg.appendStr(DataPool.ts_prescale);
	ts_msg.appendStr(DataPool.ts_scaler);
I 4
System.out.println("KUKU");
E 4
	ts_msg.appendInt8(DataPool.ts_control);
	ts_msg.appendInt8(DataPool.ts_cstatus);
	ts_msg.appendInt8(DataPool.ts_state);
	ts_msg.appendInt8(DataPool.ts_rocenable);
	//.........................sending
	try {
	  System.out.println("sending TS message");
	  ts_srv.send(ts_msg);
	  ts_srv.flush();
	} catch (TipcException te) {
	  Tut.warning(te);
	}
	
	//.........................std output      
D 5
	System.out.println("TS information from the DataPool");
E 5
I 5
	System.out.println("TS information from the DataPool1");
E 5
	System.out.println( "--------------------------\n");
	System.out.println( "Status                 - " + DataPool.ts_status);
	System.out.println( DataPool.ts_prescale);
	System.out.println( "Scalers                - " + DataPool.ts_scaler);
	System.out.println( "Control Register       - " + DataPool.ts_control);
	System.out.println( "Status Register        - " + DataPool.ts_cstatus);
	System.out.println( "State Register         - " + DataPool.ts_state);
	System.out.println( "RocEnable Register     - " + DataPool.ts_rocenable);
	DataPool.ts_update = false;
      }
    try {
      ipc_init.check((double)DataPool.ipc_sleep);
    }
    catch (Exception e) {}
    }
  }
}




E 1
