h61681
s 00001/00000/00178
d D 1.6 06/11/20 11:24:53 boiarino 7 6
c package clonjava;
c 
e
s 00006/00006/00172
d D 1.5 03/04/07 14:12:30 wolin 6 5
c mysql
e
s 00001/00001/00177
d D 1.4 02/01/30 16:28:10 wolin 5 4
c New cariboulake
e
s 00059/00027/00119
d D 1.3 01/01/11 15:09:24 wolin 4 3
c Now reads comment from file
e
s 00001/00001/00145
d D 1.2 99/11/30 16:32:58 wolin 3 1
c Typo
e
s 00000/00000/00000
d R 1.2 99/11/30 15:44:20 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 dbutil/s/update_oplog_comment.java
e
s 00146/00000/00000
d D 1.1 99/11/30 15:44:19 wolin 1 0
c date and time created 99/11/30 15:44:19 by wolin
e
u
U
f e 0
t
T
I 1
// update_oplog_comment.java

// updates operator_log comment

//  E.Wolin, 30-nov-99

I 7
package clonjava;
E 7

import java.lang.*;
import java.util.*;
import java.sql.*;
import java.io.*;


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------


public class update_oplog_comment {

D 6
  
D 5
  private static String driver         = "COM.cariboulake.sql.ingres.JSQLDriver";
E 5
I 5
  private static String driver         = "com.cariboulake.jsql.JSQLDriver";
E 5
  private static String url            = "jdbc:caribou:jsqlingres://db5:6024/";
  private static String database       = "clastest";
  private static String account        = "foo";
  private static String password       = null;
E 6
I 6
  private static String driver         = "com.mysql.jdbc.Driver";
  private static String url            = "jdbc:mysql://clonweb:3306/";
  private static String database       = "clasprod";
  private static String account        = "clasrun";
  private static String password       = "e1tocome";

E 6
  private static int unique_key        = 0;
D 4
  private static String comment        = null;
E 4
I 4
  private static String comment_file   = null;
  private static String comment        = "";
E 4
  private static boolean debug         = false;



//-----------------------------------------------------------------------------


  static public void main (String[] args) {
    

    java.sql.Connection con = null;
    String stmt;
    

    // decode command line
    decode_command_line(args);


D 4
    // load driver
    try {
      Class.forName(driver);
    }
    catch(Exception e) {
      System.err.println("Unable to load JDBC driver: " + e.toString()); System.exit(-1);
E 4
I 4
    // check input
    if((unique_key==0)||(comment_file==null)) {
	System.out.println("\n *** Missing unique_key and/or comment_file ***\n");
	System.exit(-1);
E 4
    }
D 4
    
E 4

D 4
    // connect to database
    try  {
      con = DriverManager.getConnection(url+database,account,password);
E 4
I 4

    // read comment from file
    try {
	BufferedReader b = new BufferedReader(new FileReader(comment_file));
	String s;
	while((s=b.readLine())!=null) { 
	    comment+=s; 
	    comment+="\n"; 
	}
	b.close();
E 4
    }
D 4
    catch (Exception e) {
      e.printStackTrace(); System.exit(-1);
E 4
I 4
    catch(IOException e) {
	System.err.println("Unable to read comment file: " + e);
	System.exit(-1);
E 4
    }
    

D 4
    // update comment
    try {

      stmt="update operator_log set comment=? where unique_key=?";
      PreparedStatement s = con.prepareStatement(stmt);
E 4
I 4
    // update database
    if(debug==false) {
E 4

I 4
	// load driver
	try {
	    Class.forName(driver);
	}
	catch(Exception e) {
	    System.err.println("Unable to load JDBC driver: " + e.toString()); System.exit(-1);
	}
	
	
	// connect to database
	try  {
	    con = DriverManager.getConnection(url+database,account,password);
	}
	catch (Exception e) {
	    e.printStackTrace(); System.exit(-1);
	}
	
	
	// update comment
	try {
	    stmt="update operator_log set comment=? where unique_key=?";
	    PreparedStatement s = con.prepareStatement(stmt);
	    
	    // execute query 
	    s.setString(1,comment);
	    s.setInt(2,unique_key);
	    s.executeUpdate();
	    
	} catch (Exception e) {
	    e.printStackTrace();
	    System.exit(-1);
	}
E 4

D 4
      // execute query 
      s.setString(1,comment);
      s.setInt(2,unique_key);
      s.executeUpdate();
E 4

I 4
    } else {
E 4

D 4
    } catch (Exception e) {
      e.printStackTrace();
      System.exit(-1);
E 4
I 4
	System.out.println("\nupdate operator_log set comment='" + comment +
			   "' where unique_key=" + unique_key);
	System.exit(-1);
E 4
    }


    // done
    System.exit(-1);
  }
  

//-----------------------------------------------------------------------------

  

  static public void decode_command_line(String[] args) {
  
    String help = "\nUsage:\n\n" + 
D 3
      "   java jsql  [-driver driver] [-url url] [-db database]\n" + 
E 3
I 3
      "   java update_oplog_comment  [-driver driver] [-url url] [-db database]\n" + 
E 3
D 4
      "              [-account account] [-pwd password] [-debug] [-u unique_key] [-c comment]\n\n";
E 4
I 4
      "              [-account account] [-pwd password] [-debug] [-u unique_key] [-c comment_file]\n\n";
E 4
    

    // loop over all args
    for(int i=0; i<args.length; i++) {

      if(args[i].equalsIgnoreCase("-h")) {
	System.out.println(help);
	System.exit(-1);
	
      } else if(args[i].equalsIgnoreCase("-driver")) {
	driver=args[i+1];
	i++;

      } else if(args[i].equalsIgnoreCase("-url")) {
	url=args[i+1];
	i++;

      } else if(args[i].equalsIgnoreCase("-db")) {
	database=args[i+1];
	i++;

      } else if(args[i].equalsIgnoreCase("-account")) {
	account=args[i+1];
	i++;

      } else if(args[i].equalsIgnoreCase("-pwd")) {
	password=args[i+1];
	i++;

      } else if(args[i].equalsIgnoreCase("-u")) {
	unique_key=Integer.parseInt(args[i+1]);
	i++;

      } else if(args[i].equalsIgnoreCase("-c")) {
D 4
	comment=args[i+1];
E 4
I 4
	comment_file=args[i+1];
E 4
	i++;

      } else if(args[i].equalsIgnoreCase("-debug")) {
	debug=true;
      }
    }    
    
    return;
  }
  

}
//-----------------------------------------------------------------------------
//  end class
//-----------------------------------------------------------------------------
E 1
