h55810
s 00001/00001/00120
d D 1.5 06/07/09 09:36:04 boiarino 6 5
c add clonjava.
e
s 00016/00009/00105
d D 1.4 99/09/24 15:29:32 clasrun 5 4
c Added javadoc comments (E.Wolin)
e
s 00013/00006/00101
d D 1.3 98/11/13 10:29:14 wolin 4 3
c Quit working for a while...
e
s 00001/00001/00106
d D 1.2 98/06/17 13:53:13 wolin 3 1
c Renamed clon.util
c 
e
s 00000/00000/00000
d R 1.2 98/06/15 15:40:18 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 jar/clon/util/Format.java
e
s 00107/00000/00000
d D 1.1 98/06/15 15:40:17 wolin 1 0
c date and time created 98/06/15 15:40:17 by wolin
e
u
U
f e 0
t
T
I 1
//  Format.java

D 4
//  right or left justifies object into fixed with string
E 4
I 4
D 5
//  right or left justifies object into fixed-width string
//  just returns string if width too small
E 5
E 4

D 4
//  member of clas.util package
E 4
I 4
D 5
//  member of clon.util package
E 4

//  ejw, 15-jun-98


E 5
D 3
package clas.util;
E 3
I 3
D 6
package clon.util;
E 6
I 6
package clonjava.clon.util;
E 6
E 3
D 5

E 5
import java.lang.*;


//-----------------------------------------------------------------------------
//  begin class definition
//-----------------------------------------------------------------------------


D 5
public class Format {
E 5
I 5
/**
 *
 * Right or left justifies object into fixed-width string.
 * Just returns object.toString() if width too small.
 *
 * @version 1.0
 * @author E.Wolin
 */
public abstract class Format {
E 5


I 5
//-----------------------------------------------------------------------------


  // for javadoc bug
  private Format(){;}


E 5
I 4
//-----------------------------------------------------------------------------


E 4
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


D 4
  static private String justify(String s, int width) {
E 4
I 4
  static public String justify(String s, int width) {
E 4


    // check width
    int awid = Math.abs(width);
    if(awid==0)return null;


D 4
    // width too small
E 4
I 4
    // if width too small just return string
E 4
    if(s.length()>=awid)return s;


D 4
    // create buffer and fill with spaces
E 4
I 4
    // create string buffer and fill with spaces
E 4
    StringBuffer sb = new StringBuffer();
    for(int i=0; i<awid; i++) {sb.append(' ');}


D 4
    // left or right justify
E 4
I 4
    // left or right justify in buffer
E 4
    if(width<0) {
      sb.insert(awid-s.length(),s);
    } else {
      sb.insert(0,s);
    }
I 4


    // reset buffer length
E 4
    sb.setLength(awid);
    

    // return justified string
    return sb.toString();
  }
  

}
//-----------------------------------------------------------------------------
//  end class definition
//-----------------------------------------------------------------------------
E 1
