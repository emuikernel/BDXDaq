// alarm_mgr.java

//  utility functions for managing clon alarm system

//  ejw, 11-feb-00

package clonjava;

import java.lang.*;
import java.text.*;
import java.sql.*;
import java.util.Date;


//-----------------------------------------------------------------------------
//  begin class definition
//-----------------------------------------------------------------------------


public class alarm_mgr {

  
  private static int min_sev   = 1;
  private static String system = null;
  private static int count     = 0;
    private static int sleep     = 10;  // seconds


  static public void main (String[] args) {
    

    // decode command line
    decode_command_line(args);


    // magic url to connect to database on clondb1

    String driver         = "com.mysql.jdbc.Driver";
    String url            = "jdbc:mysql://clondb1:3306/";
    String database       = "clasprod";
    String account        = "clasrun";
    String password       = "";

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
    

    // just get count
    if(count!=0) {

     try {
	String sql = " select count(alarm_status) from clonalarm where alarm_status>0"; 
	Statement s = con.createStatement();
	ResultSet result = s.executeQuery(sql);
	result.next();
	int nalarm=result.getInt(1);
	System.out.println(nalarm);
	s.close();
	//	con.commit();
	
      }
      catch (SQLException e) {
	System.out.println(e.getMessage());
      }
      catch (Exception e) {
	e.printStackTrace();
	System.exit(-1);
      }



    // clear a system
    } else if(system!=null) {

      try {

	String sql = " update clonalarm set alarm_status=0,alarm_time=current_timestamp()" + 
	  "  where alarm_status>0 and system='" + system + "'";
	
	Statement s = con.createStatement();
	//	ResultSet result = s.executeQuery(sql);
	s.execute(sql);
	s.close();
	//	con.commit();
	System.out.println("\n   Cleared system: " + system + "\n");
	
      }
      catch (SQLException e) {
	System.out.println(e.getMessage());
      }
      catch (Exception e) {
	e.printStackTrace();
	System.exit(-1);
      }


    // monitor
    } else {
      try {
	
	String sql = "select alarm_status,alarm_time,system,alarm_text from clonalarm" + 
	  "  where alarm_status>=" + min_sev;
	
	while(1==1) {
	  
	  // query and get result
	  Statement s = con.createStatement();
	  ResultSet result = s.executeQuery(sql);
	  
	  
	  // print header
	  Date d = new Date();
	  System.out.println();
	  System.out.println("   Showing severity >= " + min_sev + "  on  " + d);
	  System.out.println();
	  System.out.println("Sev System       Time                  Text");
	  System.out.println("--- ------       ----                  ----");
	  System.out.println();
	  
	  
	  // print results
	  int sev;
	  String system;
	  String time;
	  String text;
	  while (result.next()) {
	    sev=result.getInt("alarm_status");
	    system=result.getString("system");
	    time=result.getString("alarm_time");
	    text=result.getString("alarm_text");
	    
	    StringBuffer sb = new StringBuffer();
	    for(int j=0; j<80; j++) {sb.append(' ');}
	    sb.insert( 0,sev);
	    sb.insert( 4,system);
	    sb.insert(17,time);
	    sb.insert(39,text);
	    sb.setLength(110);
	    System.out.println((sb.toString()).trim());
	  }
	  
	  
	  // no more rows
	  result.close();
	  s.close();
	  //	  con.commit();
	  System.out.println();
	  Thread.sleep(sleep*1000);
	}
	
      }
      catch (SQLException e) {
	System.out.println(e.getMessage());
      }
      catch (Exception e) {
	e.printStackTrace();
	System.exit(-1);
      }

    } // monitor or clear


    // done
    System.exit(-1);


  }  // main done

//-----------------------------------------------------------------------------


  static public void decode_command_line(String[] args) {
  
    String help = "\nUsage:\n\n" + 
      "   java alarm_mgr [-m min_sev] [-clear system] [-count] [-sleep sleep]\n\n";
    

    // loop over all args
    for(int i=0; i<args.length; i++) {

      if(args[i].equalsIgnoreCase("-h")) {
	System.out.println(help);
	System.exit(-1);
	
      } else if(args[i].equalsIgnoreCase("-m")) {
	min_sev=Integer.parseInt(args[i+1]);
	i=i+1;

      } else if(args[i].equalsIgnoreCase("-clear")) {
	system=args[i+1];
	i=i+1;

      } else if(args[i].equalsIgnoreCase("-count")) {
	count=1;

      } else if(args[i].equalsIgnoreCase("-sleep")) {
	sleep=Integer.parseInt(args[i+1]);
	i=i+1;
      }
    }

    
    return;
  }
  
}

//-----------------------------------------------------------------------------
//  end class definition
//-----------------------------------------------------------------------------
