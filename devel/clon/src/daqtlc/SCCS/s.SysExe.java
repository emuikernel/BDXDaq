h34909
s 00018/00000/00069
d D 1.4 01/10/12 08:54:07 gurjyan 5 4
c *** empty log message ***
e
s 00001/00001/00068
d D 1.3 00/10/17 13:39:52 gurjyan 4 3
c 
e
s 00001/00001/00068
d D 1.2 00/10/16 15:55:45 gurjyan 3 1
c 
e
s 00000/00000/00000
d R 1.2 70/01/01 00:00:02 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 daqtlc/SysExe.java
e
s 00069/00000/00000
d D 1.1 00/07/31 14:48:43 gurjyan 1 0
c date and time created 00/07/31 14:48:43 by gurjyan
e
u
U
f e 0
t
T
I 1
import java.lang.*;
import java.io.*;

/** Class SysExe designed to run UNIX and VxWorks executables. For VxWorks connection it is using java DPLight class by G. Heyes.
 * @author Vardan H Gyurjyan. 11.19.99 CLAS Online Group.
 */

public class SysExe {
  private static boolean connected;
  private static DPCmd d ;
  
  static  {
    if (connected != true ) {
      System.out.println("Making Dp connection");	
      try {
	d = new DPCmd("jdbc:msql://clon10.jlab.org:8101/clasrun",
		      "com.imaginary.sql.msql.MsqlDriver");
      } catch (DpliteException e) {
	System.out.println("exception: " + e);
      }
      connected = true;
      //      d.SetTimeout(10000);
    }
  }
  
  


  /** Method uses DPlight to execute vxWorks commands, and will store the results in the returned string.
   * @param tmp_target  - vxWorks ROC name
   * @param tmp_command - vxWorks executable name
   * @return DPresult - stdio of the vxWorks executable
   */
  public  synchronized String run_vxworks (String tmp_target, String tmp_command) {
    String DPresult = "UNDEFINED";
    try {
      DPresult = d.dpAsk(tmp_target,tmp_command);
    } catch  (Exception e) {
      System.out.println(" JDP   exception: " + e);
      DPresult = "UNDEFINED";
    }
D 3
    //  System.out.println(DPresult);
E 3
I 3
D 4
      System.out.println(DPresult);
E 4
I 4
    //xSystem.out.println(DPresult);
E 4
E 3
    return DPresult;
  } 
  



  /**Method is used to execute the UNIX void commands. Returned int value will indicate the status of the execution.
   * @param command UNIX executable name.
   * @return status  0: Ok, -1: failed
   */
  public  int run_unix (String command) {
    int status = 0;
    Runtime r = Runtime.getRuntime();
    try {
      Process p = r.exec (command);
      
    } catch (IOException e) {
      System.out.println("run_unix exception: " +e);
      status = -1;
    }
System.out.println(status);
    return status;
  }
I 5

    /**Method is used to execute the UNIX commands. Returned InputStream connected 
       to the standard output of the child process.
       * @param command UNIX executable name.
       * @return stream result
       */
  public  InputStream run_unix_stream (String command) {
    InputStream result = null;
    Process p = null;
    Runtime r = Runtime.getRuntime();
    try {
      p = r.exec (command);
      
    } catch (IOException e) {
      System.out.println("run_unix exception: " +e);
    }
    return p.getInputStream();
  }
E 5
  
}


E 1
