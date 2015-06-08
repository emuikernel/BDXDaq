h40781
s 00000/00000/00000
d R 1.2 03/10/03 15:30:48 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 alarm_browser/s/Beeper.java
e
s 00015/00000/00000
d D 1.1 03/10/03 15:30:47 wolin 1 0
c date and time created 03/10/03 15:30:47 by wolin
e
u
U
f e 0
t
T
I 1
import java.lang.*;
import java.util.Date;

class Beeper{
    public Date beepStopDate =new Date();
    public boolean beepState =true;

    public Beeper(Date d, boolean b){
    beepStopDate =d;
    beepState =b;
    }

    public Beeper(){
    }
}
E 1
