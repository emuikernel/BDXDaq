h50678
s 00001/00000/00225
d D 1.8 06/11/20 11:13:50 boiarino 9 8
c package clonjava;
c 
e
s 00009/00009/00216
d D 1.7 03/12/19 13:22:37 wolin 8 7
c Switched back to mysql, needed by logbook_mail_ack
e
s 00008/00006/00217
d D 1.6 03/10/03 17:10:00 wolin 7 6
c Switched back to ingres
e
s 00008/00004/00215
d D 1.5 03/04/07 13:47:16 wolin 6 5
c Updated for mysql
e
s 00001/00001/00218
d D 1.4 02/01/30 16:25:27 wolin 5 4
c New cariboulake
e
s 00002/00002/00217
d D 1.3 99/08/17 14:31:30 wolin 4 3
c Typo
c 
e
s 00008/00002/00211
d D 1.2 98/12/18 15:28:11 wolin 3 1
c Now handles null result
c 
e
s 00000/00000/00000
d R 1.2 98/12/18 15:03:34 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 dbutil/s/jsql.java
e
s 00213/00000/00000
d D 1.1 98/12/18 15:03:33 wolin 1 0
c date and time created 98/12/18 15:03:33 by wolin
e
u
U
f e 0
t
T
I 1
//  jsql.java

// general purpose interface to database

//  E.Wolin, 19-dec-98

I 9
package clonjava;
E 9

import java.lang.*;
import java.util.*;
import java.sql.*;
import java.io.*;


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------


public class jsql {

  
D 5
  private static String driver         = "COM.cariboulake.sql.ingres.JSQLDriver";
E 5
I 5
D 6
  private static String driver         = "com.cariboulake.jsql.JSQLDriver";
E 5
  private static String url            = "jdbc:caribou:jsqlingres://db5:6024/";
E 6
I 6
D 7
    //  private static String driver         = "com.cariboulake.jsql.JSQLDriver";
    //  private static String url            = "jdbc:caribou:jsqlingres://db5:6024/";
E 7
I 7
D 8
    private static String driver         = "com.cariboulake.jsql.JSQLDriver";
    private static String url            = "jdbc:caribou:jsqlingres://db5:6024/";
    private static String password       = null;
    private static String account        = "foo";
E 8
I 8
//      private static String driver         = "com.cariboulake.jsql.JSQLDriver";
//      private static String url            = "jdbc:caribou:jsqlingres://db5:6024/";
//      private static String password       = null;
//      private static String account        = "foo";
E 8
E 7

D 7
  private static String driver         = "com.mysql.jdbc.Driver";
  private static String url            = "jdbc:mysql://clonweb:3306/";
E 7
I 7
D 8
    //  private static String driver         = "com.mysql.jdbc.Driver";
    //  private static String url            = "jdbc:mysql://clonweb:3306/";
    //  private static String password       = "e1tocome";
    //  private static String account        = "clasrun";
E 7

E 8
I 8
    private static String driver         = "com.mysql.jdbc.Driver";
    private static String url            = "jdbc:mysql://clonweb:3306/";
    private static String password       = "e1tocome";
    private static String account        = "clasrun";
    
E 8
E 6
  private static String database       = "clasprod";
D 6
  private static String account        = "foo";
  private static String password       = null;
E 6
I 6
D 7
  private static String account        = "clasrun";
  private static String password       = "e1tocome";
E 7
E 6
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
I 3

E 3
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
D 3
      if(!silent)System.out.print("\n"+output+"End Sql"+"\n");
E 3
I 3
      if(!silent&&(output!=null))System.out.print("\n"+output+"\nEnd Sql"+"\n");
E 3
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
      

D 3
      // query and get result set and meta data
E 3
I 3
      // execute query 
E 3
      ResultSet result = s.executeQuery(stmt);
I 3
      if(result==null)return null;


      // get result set and meta data
E 3
      ResultSetMetaData meta = result.getMetaData();
I 3
      if(meta==null)return null;
E 3


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
D 4
      "   java logbook_text [-driver driver] [-url url] [-db database]\n" + 
      "                    [-account account] [-pwd password] [-c command] [-silent] [-debug]\n\n";
E 4
I 4
      "   java jsql  [-driver driver] [-url url] [-db database]\n" + 
      "              [-account account] [-pwd password] [-c command] [-silent] [-debug]\n\n";
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
E 1
