/* MyThread.java */

package clonjava;

import java.lang.*;
import java.sql.*;
import java.util.Date;
import java.util.TimeZone;
import clonjava.clon.ipcutil.*;


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
	//this.sleep(30000);
     } catch(Exception e) {System.out.println(e);}
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





