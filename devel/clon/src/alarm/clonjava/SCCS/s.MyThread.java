h48670
s 00001/00001/00052
d D 1.3 06/07/09 09:36:33 boiarino 4 3
c add clonjava.
e
s 00006/00002/00047
d D 1.2 03/11/27 21:26:10 boiarino 3 1
c .
e
s 00000/00000/00000
d R 1.2 03/10/03 15:30:49 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 alarm_browser/s/MyThread.java
e
s 00049/00000/00000
d D 1.1 03/10/03 15:30:48 boiarino 1 0
c date and time created 03/10/03 15:30:48 by boiarino
e
u
U
f e 0
t
T
I 1
import java.lang.*;
import java.sql.*;
import java.util.Date;
import java.util.TimeZone;
D 4
import clon.ipcutil.*;
E 4
I 4
import clonjava.clon.ipcutil.*;
E 4


class MyThread extends Thread {
  
  private Alarm alarm;
  public  MyThread(Alarm parent) {
    super();
    this.setDaemon(true);
    this.setPriority(Thread.MIN_PRIORITY);
    alarm = parent;
    
    //{{INIT_CONTROLS
    //}}
  }
  
  
  public void run(){
    while(true){
      try{
	setQuery();
	Date d = new Date();
	String now = new String();
	now = d.toLocaleString();/*String.valueOf(d.getHours()) + ":" 
				   + String.valueOf(d.getMinutes()) + ":"
				   + String.valueOf(d.getSeconds());*/
	alarm.setStatus2("Last Updated: " + now);
	alarm.beepAckAlarms();
	ipc_init.check(30.0);
D 3
	//      this.sleep(30000);
      } catch(Exception e) {System.out.println(e);}
E 3
I 3
	//this.sleep(30000);
     } catch(Exception e) {System.out.println(e);}
E 3
    }
  }
  
  private synchronized void setQuery(){
    try{
      alarm.setQuery();
      
    }catch(Exception e){}
  }
  //{{DECLARE_CONTROLS
  //}}

}
I 3




E 3

E 1
