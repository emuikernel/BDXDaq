import java.lang.*;
import java.io.*;

/** Class SysExe designed to run UNIX and VxWorks executables. For VxWorks connection it is using java DPLight class by G. Heyes.
 * @author Vardan H Gyurjyan. 11.19.99 CLAS Online Group.
 */

public class UnixExe {

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
  
}
