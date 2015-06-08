/* TimerThread.java */

package clonjava;

import java.lang.*;
import java.util.Date;
import java.math.*;

class TimerThread extends Thread{
    private Alarm alarm;
    public TimerThread(Alarm parent){
        super();
        alarm = parent;
        this.setPriority(MIN_PRIORITY);
        this.setDaemon(true);
    }

    public void run(){
        while(true){
            try{Date now = new Date();
                
                if((now.getTime()- alarm.now.getTime()) >= 180000){
                    alarm.loadDefaults();
                    alarm.setStatus1("Defaults Automatically Set...");
                    alarm.now = new Date();
                }
                
                this.sleep(10000);
            }catch(Exception e){System.out.println(e);}
        }
    }
}

