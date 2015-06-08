/* MyErrorClass.java */

package clonjava;

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



