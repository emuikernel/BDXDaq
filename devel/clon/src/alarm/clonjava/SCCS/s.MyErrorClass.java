h11209
s 00000/00000/00000
d R 1.2 03/10/03 15:30:49 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 alarm_browser/s/MyErrorClass.java
e
s 00028/00000/00000
d D 1.1 03/10/03 15:30:48 wolin 1 0
c date and time created 03/10/03 15:30:48 by wolin
e
u
U
f e 0
t
T
I 1
import java.lang.*;
import java.awt.*;
//=======================================
//This class was implemented on 12-6-1998 
//=======================================
class MyErrorClass{

    public boolean errorExisted =false;
    public String dateText ="";
    public static int numErrors=0;
    public int helpID=0;
    public int alarmStatus=0;
    public String system= "";
     public String message= "";
    public int alarmID=0;
    public java.util.Date timeSet;
    public boolean ack = false;
    //this is to keep a record of which row in the table
    //the error is in so later we can change the text
    public int tableRow=999 ;
    public MyErrorClass(){
        numErrors++;
    }

}



E 1
