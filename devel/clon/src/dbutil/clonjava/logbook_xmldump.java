// logbook_xmldump.java

//  dumps operator logbook in xml format

//  E.Wolin, 8-feb-2002

package clonjava;

import java.lang.*;
import java.sql.*;


//-----------------------------------------------------------------------------
//  begin class definition
//-----------------------------------------------------------------------------


public class logbook_xmldump {

  
  private static String driver         = "com.mysql.jdbc.Driver";
  private static String url            = "jdbc:mysql://clonweb:3306/";
  private static String database       = "clasprod";
  private static String account        = "clasrun";
  private static String password       = "e1tocome";


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
      System.out.println("<operator_log>\n");
      String s;
      int colCount = meta.getColumnCount();
      while (result.next()) {
	  System.out.println("<row>");
	  for (int i=1; i<=colCount; i++) {
	      s=result.getString(i);
	      if((meta.getColumnName(i).equals("comment")) || 
		 (meta.getColumnName(i).equals("subject"))) {
		  System.out.print("<" + meta.getColumnName(i) + ">" + "\n<![CDATA[" +
				   ((s==null)?"null":s.trim()) + "]]>\n" +
				   "</" + meta.getColumnName(i) + ">\n");
	      } else {
		  System.out.print("<" + meta.getColumnName(i) + ">" + 
				   ((s==null)?"null":s.trim()) + 
				   "</" + meta.getColumnName(i) + ">\n");
	      }
	  }
	  System.out.println("</row>\n\n");
      }
      System.out.println("\n</operator_log>");
      

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
