//  logbook_text.java

//  makes entry into operator_log from command line

//  comment can come file file or command line:
//    if filename exists, uses file contents
//    if filename doesn't exist, uses "filename" as the comment

//  still to do:
//    get legal entry,system from database


//  E.Wolin, 3-nov-98

package clonjava;

import java.io.*;
import java.lang.*;
import java.sql.*;


public class logbook_text {


  // for jdbc
    //  private static String driver         = "com.cariboulake.jsql.JSQLDriver";
    //  private static String url            = "jdbc:caribou:jsqlingres://db5:6024/";

  private static String driver         = "com.mysql.jdbc.Driver";
  private static String url            = "jdbc:mysql://clonweb:3306/";
  private static String database       = "clasprod";
  private static String account        = "clasrun";
  private static String password       = "e1tocome";


  // message fields
  private static int run               = 0;
  private static String entry_type     = null;
  private static String system_type    = null;
  private static String subject        = null;
  private static String operators      = null;
  private static String email_address  = null;
  private static StringBuffer comment;


  //  legal values...eventually should get from database
  private static String legal_entry[]    =  {"routine","minor_error","severe_error","breakthrough",
					     "complaints","test","other"};
  private static String legal_system[]   = {"SAFETY","beam","target","tagger","magnets","trigger",
					    "st","dc","cc","sc","ec","lac","hardware","epics","coda",
					    "online","logbook","notebook","computer","general",
					    "documentation","other"};

  // misc
  private static int debug             = 0;
  
  
//-----------------------------------------------------------------------------


  static public void main (String[] args) {
    

    // decode command line
    decode_command_line(args);


    // query for logbook data
    get_logbook_data();


    // load jdbc driver
    try {
      Class.forName(driver);
    }
    catch(Exception e) {
      System.err.println("Unable to load driver: " + driver +"\n" + e);
      System.exit(-1);
    }
    

    // connect to database
    java.sql.Connection con = null;
    try  {
      con = DriverManager.getConnection(url+database,account,password);
    }
    catch (Exception e) {
      System.err.println("Unable to connect to database: " + url + database + "\n" + e);
      System.exit(-1);
    }
    

    // insert new entry
    try {

      // get statement object
      Statement s = con.createStatement();


      // start transaction
      con.setReadOnly(false);


      // get highest unique_key and increment if not in debug mode
      ResultSet r = s.executeQuery("select sequence_num from oplog_seq");
      r.next();
      int seq=r.getInt("sequence_num");
      seq++;
      if(debug==0)s.executeUpdate("update oplog_seq set sequence_num=" + seq);


      // make entry
      String sql = "insert into ingres.operator_log (" + 
	"entry_date,run,entry_type,system_type,subject,operators,email_address,comment,unique_key" + 
	") values (" +
	"'now'," + run + "," +
	sqlfmt(entry_type) + "," +
	sqlfmt(system_type) + "," +
	sqlfmt(subject) + "," +
	sqlfmt(operators) + "," +
	sqlfmt(email_address) + "," +
	sqlfmt(comment.toString()) + "," +
	seq + 
	")";

      if(debug==0) {
	s.executeUpdate(sql);
      } else {
	System.out.println(sql);
      }
			  
      // transaction completed
      con.commit();

    }
    catch (SQLException e) {
      System.out.println(e);
    }
    catch (Exception e) {
      e.printStackTrace();
      System.exit(-1);
    }


  }  // main done


//-----------------------------------------------------------------------------


  static public void decode_command_line(String[] args) {
  
    String help = "\nUsage:\n\n" + 
      "   java logbook_text [-driver driver] [-url url] [-db database]\n" + 
      "                    [-account account] [-pwd password] [-debug]\n\n";
    

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

      } else if(args[i].equalsIgnoreCase("-debug")) {
	debug=1;
      }
    }    
    
    return;
  }
  

//-----------------------------------------------------------------------------


  // formats string for sql
  static public String sqlfmt(String s) {

    if(s==null)return("''");

    StringBuffer sb = new StringBuffer();
    
    sb.append("'");
    for(int i=0; i<s.length(); i++)  {
      sb.append(s.charAt(i));
      if(s.charAt(i)=='\'')sb.append("'");
    }
    sb.append("'");
    
    return(sb.toString());
  }

//-----------------------------------------------------------------------------


  public static void get_logbook_data() {

    int i;
    boolean legal;
    String com;
    String line;
    char c;


    // for reading from System.in
    BufferedReader in = new BufferedReader(new InputStreamReader(System.in));


    // get run number
    System.out.print("\nenter run number: ");
    try {
      line=in.readLine();
      run=Integer.parseInt(line);
    }
    catch(IOException e) {
      System.err.println("Unable to read run: " + e);
      System.exit(-1);
    }
    
    
    // get entry_type
    legal=false;
    while(!legal) {
      System.out.print("Enter entry type (? for help): ");
      try {
	entry_type=in.readLine();
      }
      catch(IOException e) {
	System.err.println("Unable to read entry_type: " + e);
	System.exit(-1);
      }
      for(i=0; i<legal_entry.length; i++) if(entry_type.equals(legal_entry[i])){legal=true; break;}
      if(!legal) {
	System.out.println("\nLegal entry types:");
	for(i=0; i<legal_entry.length; i++) System.out.println("   "+legal_entry[i]);
	System.out.println();
      }
    }
    
    
    
    // get system_type
    legal=false;
    while(!legal) {
      System.out.print("Enter system type (? for help): ");
      try {
	system_type=in.readLine();
      }
      catch(IOException e) {
	System.err.println("Unable to read system_type: " + e);
	System.exit(-1);
      }
      for(i=0; i<legal_system.length; i++) if(system_type.equals(legal_system[i])){legal=true; break;}
      if(!legal) {
	System.out.println("\nLegal system types:");
	for(i=0; i<legal_system.length; i++) System.out.println("   "+legal_system[i]);
	System.out.println();
      }
    }
    
    
    // get subject
    System.out.print("Enter subject: ");
    try {
      subject=in.readLine();
    }
    catch(IOException e) {
      System.err.println("Unable to read subject: " + e);
      System.exit(-1);
    }
    
    
    // get operators
    legal=false;
    while(!legal) {
      System.out.print("Enter operators: ");
      try {
	operators=in.readLine();
	legal=operators.length()>1;
      }
      catch(IOException e) {
	System.err.println("Unable to read operators: " + e);
	System.exit(-1);
      }
    }
    
    
    // get email address
    legal=false;
    while(!legal) {
      System.out.print("Enter operator email address: ");
      try {
	email_address=in.readLine();
	legal=email_address.length()>1;
      }
      catch(IOException e) {
	System.err.println("Unable to read email address: " + e);
	System.exit(-1);
      }
    }

    
    // read comment from file or command line
    comment = new StringBuffer();
    System.out.print("Enter comment filename, or return to type in comment: ");
    try {
      com=in.readLine();
    } 
    catch(IOException e) {
      System.err.println("Unable to read comment filename: " + e);
      System.exit(-1);
      com=null;
    }
    if(com.length()>0) {
      try {
	BufferedReader b = new BufferedReader(new FileReader(com));
	String s;
	while((s=b.readLine())!=null) { comment.append(s); comment.append("\n"); }
	b.close();
      }
      catch(IOException e) {
	System.err.println("Unable to read comment file: " + e);
	System.exit(-1);
      }

    } else {
    System.out.println("Enter comment (last line ^D only):");
      try {
	for(;;) {
	  line=in.readLine(); 
	  if(line==null)break;
	  comment.append(line+"\n");}
      }
      catch(IOException e) {
	System.err.println("Unable to read comment: " + e);
	System.exit(-1);
      }
    }
    
    
    return;
  }
  

//-----------------------------------------------------------------------------
}        //  end class definition:  logbook_text
//-----------------------------------------------------------------------------
