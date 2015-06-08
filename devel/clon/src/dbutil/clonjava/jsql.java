//  jsql.java

// general purpose interface to database

//  E.Wolin, 19-dec-98

package clonjava;

import java.lang.*;
import java.util.*;
import java.sql.*;
import java.io.*;


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------


public class jsql {

  
//      private static String driver         = "com.cariboulake.jsql.JSQLDriver";
//      private static String url            = "jdbc:caribou:jsqlingres://db5:6024/";
//      private static String password       = null;
//      private static String account        = "foo";

    private static String driver         = "com.mysql.jdbc.Driver";
    private static String url            = "jdbc:mysql://clonweb:3306/";
    private static String password       = "e1tocome";
    private static String account        = "clasrun";
    
  private static String database       = "clasprod";
  private static String command        = null;
  private static boolean silent        = false;
  private static boolean debug         = false;



//-----------------------------------------------------------------------------


  static public void main (String[] args) {
    

    java.sql.Connection con = null;
    String output;
    

    // decode command line
    decode_command_line(args);


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
    


    // read sql from command line
    if(command==null) {

      // read and execute sql command
      BufferedReader input = new BufferedReader(new InputStreamReader(System.in));
      while (true) {
	try {
	  System.out.println("\nEnter sql command, or quit to stop:\n");
	  command = input.readLine();
	  if((command==null)||(command.equals("quit")))break;
	  output = exec_sql(con, command);
	  if(!silent)System.out.print("\n"+output+"End Sql"+"\n");
	}
	catch(IOException e){
	  e.printStackTrace();
	  System.exit(-1);
	}
      }


    // sql specified on command line
    } else {
      output = exec_sql(con, command);
      if(!silent&&(output!=null))System.out.print("\n"+output+"\nEnd Sql"+"\n");
    }
      

    // done
    System.exit(-1);
  }
  

//-----------------------------------------------------------------------------


  static public String exec_sql(Connection con, String stmt) {

    char fieldDelim = '&';
    char rowDelim = '\n';
    StringBuffer outString = new StringBuffer();
    
    try {

      // create statement
      Statement s = con.createStatement();
      

      // execute query 
      ResultSet result = s.executeQuery(stmt);
      if(result==null)return null;


      // get result set and meta data
      ResultSetMetaData meta = result.getMetaData();
      if(meta==null)return null;


      // append column names
      int colCount = meta.getColumnCount();
      outString.append(colCount).append(fieldDelim);
      for (int i = 1; i <= colCount; i++) {
	outString.append(meta.getColumnName(i)+fieldDelim);
      }
      outString.append(rowDelim+"\n");
      

      // append column data
      while (result.next()) {
	for (int j = 1; j <= colCount; j++) {
	  Object id = result.getObject(j);
	  outString.append(id); outString.append(fieldDelim);
	}
	outString.append(rowDelim);
      }
      

      // no more data
      result.close();
      s.close();
    }

    
    catch (SQLException e) {
      outString.append(e.getMessage());
    }

    catch (Exception e) {
      e.printStackTrace();
      System.exit(-1);
    }


    // done
    return outString.toString();
  }
  

//-----------------------------------------------------------------------------

  

  static public void decode_command_line(String[] args) {
  
    String help = "\nUsage:\n\n" + 
      "   java jsql  [-driver driver] [-url url] [-db database]\n" + 
      "              [-account account] [-pwd password] [-c command] [-silent] [-debug]\n\n";
    

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

      } else if(args[i].equalsIgnoreCase("-c")) {
	command=args[i+1];
	i++;

      } else if(args[i].equalsIgnoreCase("-silent")) {
	silent=true;

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
