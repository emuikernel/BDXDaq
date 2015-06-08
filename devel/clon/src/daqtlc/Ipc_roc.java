import java.lang.*;
import com.smartsockets.*;
import clon.ipcutil.*;

/** Class Ipc is designed to read the DataPool class variables and broadcast IPC smartsoket messages.
 * @author Vardan H Gyurjyan. 11.19.99 CLAS Online Group.
 */
public class Ipc_roc implements Runnable {
  
  public  TipcSrv roc_srv;
  public   TipcMsg roc_msg;

  private int[] RST = new int[39];
  
  public Roc myRoc = new Roc();
  
  
  public static void main(String argv[]) {
    new Thread ( new Ipc_roc() ).start();
    
  }
  
  
  /** 
   * run method of the thread
   */
  public void run() {
    
    //...........................initializing the IPC
    try {
      ipc_init.set_project("clasprod");
      ipc_init.set_server_names("clon10,clon00,clon05");
      TipcSrv srv=ipc_init.init("hardware_monitor_roc","Daqtlc");
    } catch (Exception e) {
      Tut.exitFailure(e.toString());
    }
    
    //.........................starting the Roc monitoring thread
    
    new Thread (myRoc).start();
    //.........................loop for accesing the data from the DataPool. 
    //.........................This loop is async with the TS monitoring thread
    
    for(;;) {
      System.out.println("ROC_update "+DataPool.roc_update);
      if(DataPool.roc_update == true ) {      
      System.out.println("=>ROC_update "+DataPool.roc_update);
		//.........................organizing the message
		//.........................get the server      
		roc_srv = TipcSvc.getSrv();
		//.........................create message 
		roc_msg = TipcSvc.createMsg(TipcSvc.lookupMt("info_server"));
		//.........................set destination 
		roc_msg.setDest("info_server/in/hardware_monitor/roc");
		roc_msg.appendStr("hardware_monitor/roc");
	
		for(int i=0; i<=38;i++) {
		  System.out.println( i+ " status      - " +DataPool.roc_status[i]);
		  if(DataPool.ts_rocbr_def[i].equals("not used")) RST[i]     = -2;
		  else if(DataPool.roc_status[i].equals("UNDEFINED")) RST[i]  = -1;
		  else if(DataPool.roc_status[i].equals("booted")) RST[i]     = 1;
		  else if(DataPool.roc_status[i].equals("configured")) RST[i] = 2;
		  else if(DataPool.roc_status[i].equals("downloaded")) RST[i] = 3;
		  else if(DataPool.roc_status[i].equals("prestarted")) RST[i] = 4;
		  else if(DataPool.roc_status[i].equals("active")) RST[i]     = 5;
		  else if(DataPool.roc_status[i].equals("paused")) RST[i]     = 6;
	  
		  if(i<=20) {
			System.out.println(" numevts    -  " +DataPool.roc_numevt[i]);
			System.out.println(" evtrate    -  " +DataPool.roc_evtrate[i]);
			System.out.println(" datarate   -  " +DataPool.roc_datarate[i]);
			DataPool.roc_update = false;
		  }
		}
		//.........................appending
		roc_msg.appendInt4Array(RST);
		roc_msg.appendInt4Array(DataPool.roc_numevt);
		roc_msg.appendReal8Array(DataPool.roc_evtrate);
		roc_msg.appendReal8Array(DataPool.roc_datarate);
		//.........................sending
		try {
		  System.out.println("sending ROC message");
		  roc_srv.send(roc_msg);
		  roc_srv.flush();
		} catch (TipcException te) {
		  Tut.warning(te);
	  }
	
	}
    try {
      ipc_init.check((double)DataPool.ipc_sleep);
    }
    catch (Exception e) {}
    }
  }
}




