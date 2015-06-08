h57727
s 00044/00043/00056
d D 1.3 03/04/17 17:03:52 boiarino 4 3
c minor
e
s 00001/00001/00098
d D 1.2 01/11/08 10:22:40 gurjyan 3 1
c *** empty log message ***
e
s 00000/00000/00000
d R 1.2 70/01/01 00:00:02 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 daqtlc/Ipc_roc.java
e
s 00099/00000/00000
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
D 4
      ipc_init.set_server_names("clon10,clon00,jlabs2,db6");
E 4
I 4
      ipc_init.set_server_names("clon10,clon00,clon05");
E 4
      TipcSrv srv=ipc_init.init("hardware_monitor_roc","Daqtlc");
    } catch (Exception e) {
      Tut.exitFailure(e.toString());
    }
    
D 4
    //...........................starting the Roc monitoring thread
E 4
I 4
    //.........................starting the Roc monitoring thread
E 4
    
    new Thread (myRoc).start();
D 4
    //...........................loop for accesing the datat form the DataPool. 
    //...........................This loop is async with the TS monitoring thread
E 4
I 4
    //.........................loop for accesing the data from the DataPool. 
    //.........................This loop is async with the TS monitoring thread
E 4
    
    for(;;) {
D 4
      System.out.println( "ROC _update  "+DataPool.roc_update);
E 4
I 4
      System.out.println("ROC_update "+DataPool.roc_update);
E 4
      if(DataPool.roc_update == true ) {      
D 4
	//.........................organizing the message
	//.........................get the server      
	roc_srv = TipcSvc.getSrv();
	//.........................create message 
	roc_msg = TipcSvc.createMsg(TipcSvc.lookupMt("info_server"));
	//.........................set destination 
	roc_msg.setDest("info_server/in/hardware_monitor/roc");
	roc_msg.appendStr("hardware_monitor/roc");
E 4
I 4
      System.out.println("=>ROC_update "+DataPool.roc_update);
		//.........................organizing the message
		//.........................get the server      
		roc_srv = TipcSvc.getSrv();
		//.........................create message 
		roc_msg = TipcSvc.createMsg(TipcSvc.lookupMt("info_server"));
		//.........................set destination 
		roc_msg.setDest("info_server/in/hardware_monitor/roc");
		roc_msg.appendStr("hardware_monitor/roc");
E 4
	
D 4
	for(int i=0; i<=38;i++) {
D 3
	  System.out.println( " status      - " +DataPool.roc_status[i]);
E 3
I 3
	  System.out.println( i+ " status      - " +DataPool.roc_status[i]);
E 3
	  if(DataPool.ts_rocbr_def[i].equals("not used")) RST[i]     = -2;
	  else if(DataPool.roc_status[i].equals("UNDEFINED")) RST[i]  = -1;
	  else if(DataPool.roc_status[i].equals("booted")) RST[i]     = 1;
	  else if(DataPool.roc_status[i].equals("configured")) RST[i] = 2;
	  else if(DataPool.roc_status[i].equals("downloaded")) RST[i] = 3;
	  else if(DataPool.roc_status[i].equals("prestarted")) RST[i] = 4;
	  else if(DataPool.roc_status[i].equals("active")) RST[i]     = 5;
	  else if(DataPool.roc_status[i].equals("paused")) RST[i]     = 6;
E 4
I 4
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
E 4
	  
D 4
	  if(i<=20) {
	    System.out.println(" numevts    -  " +DataPool.roc_numevt[i]);
	    System.out.println(" evtrate    -  " +DataPool.roc_evtrate[i]);
	    System.out.println(" datarate   -  " +DataPool.roc_datarate[i]);
	    DataPool.roc_update = false;
E 4
I 4
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
E 4
	  }
D 4
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
E 4
	
D 4
      }
E 4
I 4
	}
E 4
    try {
      ipc_init.check((double)DataPool.ipc_sleep);
    }
    catch (Exception e) {}
    }
  }
}




E 1
