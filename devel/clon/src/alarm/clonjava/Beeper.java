/* Beeper.java */

package clonjava;

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
