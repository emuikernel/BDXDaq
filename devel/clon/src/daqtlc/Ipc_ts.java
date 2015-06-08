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
      ipc_init.set_server_names("clon10,clon00,clon05");
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

	ts_msg.appendStr(DataPool.ts_status);
	ts_msg.appendStr(DataPool.ts_prescale);
	ts_msg.appendStr(DataPool.ts_scaler);
System.out.println("KUKU");
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
	System.out.println("TS information from the DataPool1");
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




