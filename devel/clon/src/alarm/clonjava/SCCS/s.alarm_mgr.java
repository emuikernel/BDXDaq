h41032
s 00003/00002/00226
d D 1.24 06/11/07 16:44:42 wolin 25 24
c Replace now with current_timestamp()
c 
e
s 00004/00004/00224
d D 1.23 06/11/07 16:17:39 wolin 24 23
c Updated for mySql, removed commit statements
c 
e
s 00001/00002/00227
d D 1.22 06/10/12 23:20:07 boiarino 23 22
c set "" password
c 
e
s 00013/00005/00216
d D 1.21 06/10/12 14:49:56 boiarino 22 21
c package clonjava;
c 
e
s 00002/00002/00219
d D 1.20 06/10/08 10:43:39 boiarino 21 20
c mark places to be changed
c 
e
s 00001/00001/00220
d D 1.19 02/01/30 16:22:48 wolin 20 19
c New cariboulake
e
s 00007/00002/00214
d D 1.18 00/04/18 16:22:50 wolin 19 18
c Added -sleep
e
s 00034/00006/00182
d D 1.17 00/02/11 16:31:08 wolin 18 17
c Added -count
e
s 00001/00001/00187
d D 1.16 99/07/01 15:10:45 wolin 17 16
c No min_sev requirement on clear
c 
e
s 00000/00007/00188
d D 1.15 98/11/23 11:00:40 wolin 16 15
c Eliminated debug msg
e
s 00011/00003/00184
d D 1.14 98/11/19 16:09:10 wolin 15 14
c Bug in decoding cmd line
c 
e
s 00005/00001/00182
d D 1.13 98/11/09 13:29:37 wolin 14 13
c Forget System.exit!
c 
e
s 00001/00001/00182
d D 1.12 98/10/09 09:45:36 wolin 13 12
c Typo
c 
e
s 00001/00002/00182
d D 1.11 98/10/09 09:38:56 wolin 12 11
c Minor mods
c 
e
s 00004/00004/00180
d D 1.10 98/10/09 09:37:55 wolin 11 10
c Switched name to alarm_mgr
c 
e
s 00115/00066/00069
d D 1.9 98/10/09 09:31:52 wolin 10 9
c Added clear capability
c 
e
s 00001/00002/00134
d D 1.8 98/10/08 09:36:33 wolin 9 8
c Switched to db5
c 
e
s 00005/00003/00131
d D 1.7 98/07/22 13:49:54 wolin 8 7
c Switched to Caribou Lake
c 
e
s 00005/00005/00129
d D 1.6 98/07/17 14:09:02 wolin 7 6
c Minor format changes
c 
e
s 00001/00001/00133
d D 1.5 98/07/17 14:04:53 wolin 6 5
c Still longer...
c 
e
s 00001/00001/00133
d D 1.4 98/07/17 14:03:51 wolin 5 4
c Increased string length
c 
e
s 00003/00001/00131
d D 1.3 98/07/14 17:28:46 wolin 4 3
c Added date
c 
e
s 00053/00047/00079
d D 1.2 98/07/14 12:01:26 wolin 3 1
c Added infinite loop and commit
c 
e
s 00000/00000/00000
d R 1.2 98/07/13 17:12:20 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 alarm_monitor/s/alarm_mgr.java
e
s 00126/00000/00000
d D 1.1 98/07/13 17:12:19 wolin 1 0
c 
e
u
U
f b 
f e 0
t
T
I 1
D 11
//  clonalarm.java
E 11
I 11
// alarm_mgr.java
E 11

D 3
//  dumps errors in clonalarm table
E 3
I 3
D 10
//  dumps clonalarm table
//     specify minimum level on command line...default is 2
E 10
I 10
D 11
//  manages clonalarms (display, clear, etc.)
E 11
I 11
D 18
//  manages clon alarms (display, clear, etc.)
E 18
I 18
//  utility functions for managing clon alarm system
E 18
E 11
E 10
E 3

D 10
//  ejw, 13-jul-98
E 10
I 10
D 18
//  ejw, 8-oct-98
E 18
I 18
//  ejw, 11-feb-00
E 18
E 10

I 22
package clonjava;
E 22

import java.lang.*;
import java.text.*;
import java.sql.*;
I 4
import java.util.Date;
E 4


//-----------------------------------------------------------------------------
//  begin class definition
//-----------------------------------------------------------------------------


D 11
public class clonalarm {
E 11
I 11
D 18
public class alarm_mgr{
E 18
I 18
public class alarm_mgr {
E 18
E 11

  
I 10
D 18
  private static int min_sev   = 2;
E 18
I 18
  private static int min_sev   = 1;
E 18
  private static String system = null;
I 18
  private static int count     = 0;
I 19
    private static int sleep     = 10;  // seconds
E 19
E 18


E 10
  static public void main (String[] args) {
    
I 3
D 10
    int min_sev=2;
    
E 3
    if((args.length>0)&&(args[0].equalsIgnoreCase("-h"))) {
D 3
      System.out.println("\nUsage:\n\n     java clonalarm [severity_min]\n\n");
E 3
I 3
      System.out.println("\nUsage:\n\n     java clonalarm [min_severity]\n\n");
E 3
      System.exit(-1);
    }
E 10

I 10
    // decode command line
    decode_command_line(args);
E 10

I 15
D 16
    System.out.println("system is: " + system);
    System.out.println("min_sev is: " + min_sev);


E 16
E 15
I 10

E 10
D 22
    // magic url to connect to database on db5
D 8
    String url = "jdbc:openlink://db5.cebaf.gov/DSN=dsn_clasprod";
E 8
I 8
D 9
    String url = "jdbc:caribou:jsqlingres://db3:6024/db5::clasprod";
    //    String url = "jdbc:openlink://db5.cebaf.gov/DSN=dsn_clasprod";
E 9
I 9
D 10
    String url = "jdbc:caribou:jsqlingres://db5:6024/db5::clasprod";
E 10
I 10
D 21
    String url = "jdbc:caribou:jsqlingres://db5:6024/clasprod";
E 21
I 21
    todo !!!!! String url = "jdbc:caribou:jsqlingres://db5:6024/clasprod";
E 22
I 22
    // magic url to connect to database on clondb1
D 23
    //String url = "jdbc:caribou:jsqlingres://db5:6024/clasprod";
E 23

    String driver         = "com.mysql.jdbc.Driver";
    String url            = "jdbc:mysql://clondb1:3306/";
    String database       = "clasprod";
    String account        = "clasrun";
D 23
    String password       = "e1tocome";
E 23
I 23
    String password       = "";
E 23

E 22
E 21
E 10
E 9
E 8
    java.sql.Connection con = null;


D 10
    // select alarms
D 3
    String sql = "select alarm_status,alarm_time,system,alarm_text from ingres.clonalarm" + 
      "  where alarm_status>=";
    if(args.length<1) {
      sql+=2;
    } else {
      sql+=args[0];
E 3
I 3
    if(args.length>=1) {
      min_sev=Integer.parseInt(args[0]);
E 3
    }
I 3
    String sql = "select alarm_status,alarm_time,system,alarm_text from ingres.clonalarm" + 
      "  where alarm_status>=" + min_sev;
E 3

I 3

E 10
E 3
D 12
    // load openlink jdbc driver...opljdbc.zip must be in CLASSPATH
E 12
I 12
    // load jdbc driver
E 12
    try {
D 8
      Class.forName("openlink.jdbc.Driver");
E 8
I 8
D 20
      Class.forName("COM.cariboulake.sql.ingres.JSQLDriver");
E 20
I 20
D 22
      Class.forName("com.cariboulake.jsql.JSQLDriver");
E 22
I 22
      Class.forName(driver);
E 22
E 20
D 12
      //  Class.forName("openlink.jdbc.Driver");
E 12
E 8
    }
    catch(Exception e) {
      System.err.println("DBManager: Unable to load JDBC driver" + url);
      System.err.println("DBManager: " + e.toString());
      System.exit(-1);
    }
    

    // connect to database
    try  {
D 8
      con = DriverManager.getConnection(url);
E 8
I 8
D 22
      con = DriverManager.getConnection(url,"foo",null);
E 22
I 22
      con = DriverManager.getConnection(url+database,account,password);
E 22
E 8
    }
    catch (Exception e) {
      e.printStackTrace(); System.exit(-1);
    }
    

I 18
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
D 24
	con.commit();
E 24
I 24
	//	con.commit();
E 24
	
      }
      catch (SQLException e) {
	System.out.println(e.getMessage());
      }
      catch (Exception e) {
	e.printStackTrace();
	System.exit(-1);
      }



E 18
D 10
    // execute query, print results
    try {
E 10
I 10
    // clear a system
D 18
    if(system!=null) {
E 18
I 18
    } else if(system!=null) {
E 18
E 10

D 3
      // create statement object
      Statement s = con.createStatement();
E 3
I 3
D 10
      while(1==1) {
E 10
I 10
      try {
E 10
E 3

D 3

      // query and get result and meta-data sets
      ResultSet result = s.executeQuery(sql);
      ResultSetMetaData meta = result.getMetaData();
      
      
      // print header
      System.out.println();
      System.out.println("Sev System            Time                  Text");
      System.out.println("--- ------            ----                  ----");
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
	sb.insert(22,time);
	sb.insert(44,text);
	sb.setLength(90);
	System.out.println(sb);
E 3
I 3
D 10
	// query and get result
E 10
I 10
D 21
	String sql = " update ingres.clonalarm set alarm_status=0,alarm_time='now'" + 
E 21
I 21
D 22
	todo !!! String sql = " update ingres.clonalarm set alarm_status=0,alarm_time='now'" + 
E 22
I 22
D 25
	String sql = " update clonalarm set alarm_status=0,alarm_time='now'" + 
E 25
I 25
	String sql = " update clonalarm set alarm_status=0,alarm_time=current_timestamp()" + 
E 25
E 22
E 21
D 17
	  "  where alarm_status>=" + min_sev + " and system='" + system + "'";
E 17
I 17
	  "  where alarm_status>0 and system='" + system + "'";
E 17
	
E 10
	Statement s = con.createStatement();
D 25
	ResultSet result = s.executeQuery(sql);
E 25
I 25
	//	ResultSet result = s.executeQuery(sql);
	s.execute(sql);
E 25
I 10
	s.close();
D 24
	con.commit();
E 24
I 24
	//	con.commit();
E 24
	System.out.println("\n   Cleared system: " + system + "\n");
E 10
	
I 10
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
E 10
	
D 10
	// print header
I 4
	Date d = new Date();
E 4
	System.out.println();
D 4
	System.out.println("      Showing severity >= " + min_sev);
E 4
I 4
	System.out.println("   Showing severity >= " + min_sev + "  on  " + d);
E 4
	System.out.println();
D 7
	System.out.println("Sev System            Time                  Text");
	System.out.println("--- ------            ----                  ----");
E 7
I 7
	System.out.println("Sev System       Time                  Text");
	System.out.println("--- ------       ----                  ----");
E 7
	System.out.println();
E 10
I 10
D 24
	String sql = "select alarm_status,alarm_time,system,alarm_text from ingres.clonalarm" + 
E 24
I 24
	String sql = "select alarm_status,alarm_time,system,alarm_text from clonalarm" + 
E 24
	  "  where alarm_status>=" + min_sev;
E 10
	
D 10
	
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
E 10
I 10
	while(1==1) {
E 10
	  
D 10
	  StringBuffer sb = new StringBuffer();
	  for(int j=0; j<80; j++) {sb.append(' ');}
	  sb.insert( 0,sev);
	  sb.insert( 4,system);
D 7
	  sb.insert(22,time);
	  sb.insert(44,text);
E 7
I 7
	  sb.insert(17,time);
	  sb.insert(39,text);
E 7
D 5
	  sb.setLength(90);
E 5
I 5
D 6
	  sb.setLength(100);
E 6
I 6
	  sb.setLength(110);
E 6
E 5
D 7
	  System.out.println(sb);
E 7
I 7
	  System.out.println((sb.toString()).trim());
E 10
I 10
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
D 24
	  con.commit();
E 24
I 24
	  //	  con.commit();
E 24
	  System.out.println();
D 19
	  Thread.sleep(30000);
E 19
I 19
	  Thread.sleep(sleep*1000);
E 19
E 10
E 7
	}
	
D 10
	
	// no more rows
	result.close();
	s.close();
	con.commit();
	System.out.println();
	Thread.sleep(30000);
E 10
E 3
      }
I 10
      catch (SQLException e) {
	System.out.println(e.getMessage());
      }
      catch (Exception e) {
	e.printStackTrace();
	System.exit(-1);
      }
E 10
D 3
      
E 3

D 3
      // no more rows
      result.close();
      s.close();
E 3
D 10
    }
D 3

E 3
    catch (SQLException e) {
      System.out.println(e.getMessage());
    }
    catch (Exception e) {
      e.printStackTrace();
      System.exit(-1);
    }
E 10
I 10
    } // monitor or clear
E 10

I 14

    // done
    System.exit(-1);


E 14
D 10

E 10
  }  // main done
D 14
  
E 14

D 10
}
E 10
I 10
//-----------------------------------------------------------------------------
E 10


I 10
  static public void decode_command_line(String[] args) {
  
    String help = "\nUsage:\n\n" + 
D 11
      "   java clonalarm [-m min_severity] [-clear system]";
E 11
I 11
D 13
      "   java alarm_mgr [-m min_severity] [-clear system]";
E 13
I 13
D 15
      "   java alarm_mgr [-m min_severity] [-clear system]\n\n";
E 15
I 15
D 18
      "   java alarm_mgr [-m min_sev] [-clear system]\n\n";
E 18
I 18
D 19
      "   java alarm_mgr [-m min_sev] [-clear system] [-count]\n\n";
E 19
I 19
      "   java alarm_mgr [-m min_sev] [-clear system] [-count] [-sleep sleep]\n\n";
E 19
E 18
E 15
E 13
E 11
    

    // loop over all args
    for(int i=0; i<args.length; i++) {
D 16

I 15
    System.out.println("i,arg is: " + i + " " + args[i]);

E 16

E 15
      if(args[i].equalsIgnoreCase("-h")) {
	System.out.println(help);
	System.exit(-1);
	
      } else if(args[i].equalsIgnoreCase("-m")) {
	min_sev=Integer.parseInt(args[i+1]);
D 15
	i=i+2;
E 15
I 15
	i=i+1;
E 15

      } else if(args[i].equalsIgnoreCase("-clear")) {
	system=args[i+1];
D 15
	i=i+2;
E 15
I 15
	i=i+1;
I 18

      } else if(args[i].equalsIgnoreCase("-count")) {
	count=1;
I 19

      } else if(args[i].equalsIgnoreCase("-sleep")) {
	sleep=Integer.parseInt(args[i+1]);
	i=i+1;
E 19
E 18
E 15
      }
    }
I 15

E 15
    
    return;
  }
  
}

E 10
//-----------------------------------------------------------------------------
//  end class definition
//-----------------------------------------------------------------------------
E 1
