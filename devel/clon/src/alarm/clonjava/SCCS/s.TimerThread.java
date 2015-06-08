h07891
s 00000/00000/00000
d R 1.2 03/10/03 15:30:50 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 alarm_browser/s/TimerThread.java
e
s 00029/00000/00000
d D 1.1 03/10/03 15:30:49 wolin 1 0
c date and time created 03/10/03 15:30:49 by wolin
e
u
U
f e 0
t
T
I 1
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

E 1
