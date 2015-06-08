// update_oplog_comment.java

// updates operator_log comment

//  E.Wolin, 30-nov-99

package clonjava;

import java.lang.*;
import java.util.*;
import java.sql.*;
import java.io.*;


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------


public class update_oplog_comment {

  private static String driver         = "com.mysql.jdbc.Driver";
  private static String url            = "jdbc:mysql://clonweb:3306/";
  private static String database       = "clasprod";
  private static String account        = "clasrun";
  private static String password       = "e1tocome";

  private static int unique_key        = 0;
  private static String comment_file   = null;
  private static String comment        = "";
  private static boolean debug         = false;



//-----------------------------------------------------------------------------


  static public void main (String[] args) {
    

    java.sql.Connection con = null;
    String stmt;
    

    // decode command line
    decode_command_line(args);


    // check input
    if((unique_key==0)||(comment_file==null)) {
	System.out.println("\n *** Missing unique_key and/or comment_file ***\n");
	System.exit(-1);
    }


    // read comment from file
    try {
	BufferedReader b = new BufferedReader(new FileReader(comment_file));
	String s;
	while((s=b.readLine())!=null) { 
	    comment+=s; 
	    comment+="\n"; 
	}
	b.close();
    }
    catch(IOException e) {
	System.err.println("Unable to read comment file: " + e);
	System.exit(-1);
    }
    

    // update database
    if(debug==false) {

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


    } else {

	System.out.println("\nupdate operator_log set comment='" + comment +
			   "' where unique_key=" + unique_key);
	System.exit(-1);
    }


    // done
    System.exit(-1);
  }
  

//-----------------------------------------------------------------------------

  

  static public void decode_command_line(String[] args) {
  
    String help = "\nUsage:\n\n" + 
      "   java update_oplog_comment  [-driver driver] [-url url] [-db database]\n" + 
      "              [-account account] [-pwd password] [-debug] [-u unique_key] [-c comment_file]\n\n";
    

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
	comment_file=args[i+1];
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
