//  Format.java


package clonjava.clon.util;
import java.lang.*;


//-----------------------------------------------------------------------------
//  begin class definition
//-----------------------------------------------------------------------------


/**
 *
 * Right or left justifies object into fixed-width string.
 * Just returns object.toString() if width too small.
 *
 * @version 1.0
 * @author E.Wolin
 */
public abstract class Format {


//-----------------------------------------------------------------------------


  // for javadoc bug
  private Format(){;}


//-----------------------------------------------------------------------------


  static public String justify(Object o, int width) {
    
    return(justify(o.toString(),width));

  }


//-----------------------------------------------------------------------------


  static public String justify(long n, int width) {
    
    return(justify(Long.toString(n),width));

  }


//-----------------------------------------------------------------------------


  static public String justify(int n, int width) {
    
    return(justify(Integer.toString(n),width));

  }


//-----------------------------------------------------------------------------


  static public String justify(float n, int width) {
    
    return(justify(Float.toString(n),width));

  }


//-----------------------------------------------------------------------------


  static public String justify(double n, int width) {
    
    return(justify(Double.toString(n),width));

  }


//-----------------------------------------------------------------------------


  static public String justify(String s, int width) {


    // check width
    int awid = Math.abs(width);
    if(awid==0)return null;


    // if width too small just return string
    if(s.length()>=awid)return s;


    // create string buffer and fill with spaces
    StringBuffer sb = new StringBuffer();
    for(int i=0; i<awid; i++) {sb.append(' ');}


    // left or right justify in buffer
    if(width<0) {
      sb.insert(awid-s.length(),s);
    } else {
      sb.insert(0,s);
    }


    // reset buffer length
    sb.setLength(awid);
    

    // return justified string
    return sb.toString();
  }
  

}
//-----------------------------------------------------------------------------
//  end class definition
//-----------------------------------------------------------------------------
