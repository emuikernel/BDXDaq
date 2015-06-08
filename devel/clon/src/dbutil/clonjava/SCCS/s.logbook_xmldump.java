h63379
s 00001/00000/00127
d D 1.6 06/11/20 11:23:43 boiarino 7 6
c package clonjava;
c 
e
s 00004/00004/00123
d D 1.5 03/04/07 14:10:02 wolin 6 5
c mysql
e
s 00002/00001/00125
d D 1.4 02/02/12 16:11:48 wolin 5 4
c Minor mods
e
s 00002/00002/00124
d D 1.3 02/02/12 11:00:42 wolin 4 3
c Minor
e
s 00002/00002/00124
d D 1.2 02/02/08 14:27:54 wolin 3 1
c Minor
e
s 00000/00000/00000
d R 1.2 02/02/08 14:22:57 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 dbutil/s/logbook_xmldump.java
e
s 00126/00000/00000
d D 1.1 02/02/08 14:22:56 wolin 1 0
c 
e
u
U
f b 
f e 0
t
T
I 1
// logbook_xmldump.java

//  dumps operator logbook in xml format

//  E.Wolin, 8-feb-2002

I 7
package clonjava;
E 7

import java.lang.*;
import java.sql.*;


//-----------------------------------------------------------------------------
//  begin class definition
//-----------------------------------------------------------------------------


public class logbook_xmldump {

  
D 6
  private static String driver         = "com.cariboulake.jsql.JSQLDriver";
  private static String url            = "jdbc:caribou:jsqlingres://db5:6024/";
E 6
I 6
  private static String driver         = "com.mysql.jdbc.Driver";
  private static String url            = "jdbc:mysql://clonweb:3306/";
E 6
  private static String database       = "clasprod";
D 6
  private static String account        = "foo";
  private static String password       = null;
E 6
I 6
  private static String account        = "clasrun";
  private static String password       = "e1tocome";
E 6


//-----------------------------------------------------------------------------


  static public void main (String[] args) {
    

    // help message
    if((args.length<=0)||args[0].equalsIgnoreCase("-h")) {
      System.out.println("\nUsage:\n\n     javac logbook_xmldump ukey_start [ukey_end]\n\n");
      System.exit(-1);
    }


    // magic JDBC url to connect to database on db5
    java.sql.Connection con = null;


    // load jdbc driver
    try {
      Class.forName(driver);
    }
    catch(Exception e) {
      System.err.println("DBManager: Unable to load JDBC driver" + url);
      System.err.println("DBManager: " + e.toString());
      System.exit(-1);
    }
    

    // connect to database
    try  {
      con = DriverManager.getConnection(url+database,account,password);
    }
    catch (Exception e) {
      e.printStackTrace(); System.exit(-1);
    }
    

    // create and execute query
    try {

      // create statement object
      Statement stmt = con.createStatement();

      // create sql string to execute
      String sql = "select * from operator_log" + 
	" where unique_key>=" + args[0];
      if(args.length>1)sql+=" and unique_key<=" + args[1];
      

      // query and get result and metadata objects
      ResultSet result = stmt.executeQuery(sql);
      ResultSetMetaData meta = result.getMetaData();


      // loop over rows...print results in xml format
D 3
      System.out.println("<dbdata>\n");
E 3
I 3
      System.out.println("<operator_log>\n");
E 3
      String s;
      int colCount = meta.getColumnCount();
      while (result.next()) {
	  System.out.println("<row>");
	  for (int i=1; i<=colCount; i++) {
	      s=result.getString(i);
D 5
	      if(meta.getColumnName(i).equals("comment")) {
E 5
I 5
	      if((meta.getColumnName(i).equals("comment")) || 
		 (meta.getColumnName(i).equals("subject"))) {
E 5
		  System.out.print("<" + meta.getColumnName(i) + ">" + "\n<![CDATA[" +
D 4
				   ((s==null)?"(null)":s.trim()) + "]]>\n" +
E 4
I 4
				   ((s==null)?"null":s.trim()) + "]]>\n" +
E 4
				   "</" + meta.getColumnName(i) + ">\n");
	      } else {
		  System.out.print("<" + meta.getColumnName(i) + ">" + 
D 4
				   ((s==null)?"(null)":s.trim()) + 
E 4
I 4
				   ((s==null)?"null":s.trim()) + 
E 4
				   "</" + meta.getColumnName(i) + ">\n");
	      }
	  }
	  System.out.println("</row>\n\n");
      }
D 3
      System.out.println("\n</dbdata>");
E 3
I 3
      System.out.println("\n</operator_log>");
E 3
      

      // done...no more rows
      result.close();
      stmt.close();
      System.out.println("\n\n");


    }
    catch (SQLException e) {
      System.out.println(e.getMessage());
    }
    catch (Exception e) {
      e.printStackTrace();
      System.exit(-1);
    }


    System.exit(-1);
  }  // main done
  
}
//-----------------------------------------------------------------------------
//  end class definition
//-----------------------------------------------------------------------------
E 1
