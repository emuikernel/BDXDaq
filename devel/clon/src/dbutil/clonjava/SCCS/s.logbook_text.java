h36091
s 00001/00000/00367
d D 1.8 06/11/20 11:23:14 boiarino 9 8
c package clonjava;
c 
e
s 00008/00005/00359
d D 1.7 03/04/07 14:07:46 wolin 8 7
c mysql
e
s 00001/00001/00363
d D 1.6 02/01/30 16:30:26 wolin 7 6
c New cariboulake
e
s 00002/00001/00362
d D 1.5 98/11/30 13:07:56 wolin 6 5
c Added documentation to system list
c 
e
s 00001/00001/00362
d D 1.4 98/11/16 02:26:06 wolin 5 4
c db5, not db3
c 
e
s 00064/00039/00299
d D 1.3 98/11/04 13:06:42 wolin 4 3
c Comment now working, still need to get entry and system types from database
c 
e
s 00152/00052/00186
d D 1.2 98/11/03 17:13:47 wolin 3 1
c Switched to command line version, still testing
c 
e
s 00000/00000/00000
d R 1.2 98/07/23 10:07:50 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 dbutil/s/logbook_text.java
e
s 00238/00000/00000
d D 1.1 98/07/23 10:07:49 wolin 1 0
c date and time created 98/07/23 10:07:49 by wolin
e
u
U
f e 0
t
T
I 1
D 3
//  oplog_entry.java
E 3
I 3
//  logbook_text.java
E 3

//  makes entry into operator_log from command line

//  comment can come file file or command line:
//    if filename exists, uses file contents
//    if filename doesn't exist, uses "filename" as the comment

D 3
//  E.Wolin, 23-jul-98
E 3
I 3
//  still to do:
//    get legal entry,system from database


//  E.Wolin, 3-nov-98
E 3

I 9
package clonjava;
E 9

import java.io.*;
import java.lang.*;
import java.sql.*;


D 3
public class oplog_entry {
E 3
I 3
public class logbook_text {
E 3


  // for jdbc
D 7
  private static String driver         = "COM.cariboulake.sql.ingres.JSQLDriver";
E 7
I 7
D 8
  private static String driver         = "com.cariboulake.jsql.JSQLDriver";
E 7
D 5
  private static String url            = "jdbc:caribou:jsqlingres://db3:6024/";
E 5
I 5
  private static String url            = "jdbc:caribou:jsqlingres://db5:6024/";
E 5
D 3
  private static String database       = "db5::clastest";
E 3
I 3
  private static String database       = "db5::clasprod";
E 3
  private static String account        = "foo";
  private static String password       = null;
E 8
I 8
    //  private static String driver         = "com.cariboulake.jsql.JSQLDriver";
    //  private static String url            = "jdbc:caribou:jsqlingres://db5:6024/";
E 8

I 8
  private static String driver         = "com.mysql.jdbc.Driver";
  private static String url            = "jdbc:mysql://clonweb:3306/";
  private static String database       = "clasprod";
  private static String account        = "clasrun";
  private static String password       = "e1tocome";
E 8

I 8

E 8
  // message fields
I 3
  private static int run               = 0;
E 3
  private static String entry_type     = null;
  private static String system_type    = null;
  private static String subject        = null;
  private static String operators      = null;
I 3
  private static String email_address  = null;
E 3
  private static StringBuffer comment;


I 3
  //  legal values...eventually should get from database
  private static String legal_entry[]    =  {"routine","minor_error","severe_error","breakthrough",
					     "complaints","test","other"};
  private static String legal_system[]   = {"SAFETY","beam","target","tagger","magnets","trigger",
					    "st","dc","cc","sc","ec","lac","hardware","epics","coda",
D 6
					    "online","logbook","notebook","computer","general","other"};
E 6
I 6
					    "online","logbook","notebook","computer","general",
					    "documentation","other"};
E 6

E 3
  // misc
  private static int debug             = 0;
  
  
//-----------------------------------------------------------------------------


  static public void main (String[] args) {
    

    // decode command line
    decode_command_line(args);


I 3
    // query for logbook data
    get_logbook_data();


E 3
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


D 3
      // get highest unique_key and increment
E 3
I 3
      // get highest unique_key and increment if not in debug mode
E 3
      ResultSet r = s.executeQuery("select sequence_num from oplog_seq");
      r.next();
      int seq=r.getInt("sequence_num");
      seq++;
      if(debug==0)s.executeUpdate("update oplog_seq set sequence_num=" + seq);


      // make entry
      String sql = "insert into ingres.operator_log (" + 
D 3
	"entry_date,run,entry_type,system_type,subject,operators,comment,unique_key" + 
E 3
I 3
	"entry_date,run,entry_type,system_type,subject,operators,email_address,comment,unique_key" + 
E 3
	") values (" +
D 3
	"'now',0," + 
E 3
I 3
	"'now'," + run + "," +
E 3
	sqlfmt(entry_type) + "," +
	sqlfmt(system_type) + "," +
	sqlfmt(subject) + "," +
	sqlfmt(operators) + "," +
I 3
	sqlfmt(email_address) + "," +
E 3
	sqlfmt(comment.toString()) + "," +
	seq + 
	")";
I 3

E 3
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
D 3
      "   java oplog_entry -e entry_type -s system_type -o operators -c file|comment \n" + 
      "                    [-sub subject] [-driver driver] [-url url] [-db database]\n" + 
E 3
I 3
      "   java logbook_text [-driver driver] [-url url] [-db database]\n" + 
E 3
      "                    [-account account] [-pwd password] [-debug]\n\n";
    

D 3
    // die if no args
    if(args.length<=0) {
      System.out.println(help);
      System.exit(-1);
    }


E 3
    // loop over all args
    for(int i=0; i<args.length; i++) {

      if(args[i].equalsIgnoreCase("-h")) {
	System.out.println(help);
	System.exit(-1);
	
D 3
      } else if(args[i].equalsIgnoreCase("-e")) {
	entry_type=args[i+1];
	i++;

      } else if(args[i].equalsIgnoreCase("-s")) {
	system_type=args[i+1];
	i++;

      } else if(args[i].equalsIgnoreCase("-o")) {
	operators=args[i+1];
	i++;

      } else if(args[i].equalsIgnoreCase("-c")) {
	comment = new StringBuffer();
	try {
	  BufferedReader b = new BufferedReader(new FileReader(args[i+1]));
	  String s;
	  while((s=b.readLine())!=null) { comment.append(s); comment.append("\n"); }
	  b.close();
	}
	catch(IOException e) {
	  comment.append(args[i+1]);
	}
	i++; 
	
      } else if(args[i].equalsIgnoreCase("-sub")) {
	subject=args[i+1];
	i++;

E 3
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
    
D 3
    // check for missing args
    if((entry_type==null)||(system_type==null)||(operators==null)||(comment==null)) {
      System.out.println(help);
      System.exit(-1);
    }
    
E 3
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

I 3
//-----------------------------------------------------------------------------


  public static void get_logbook_data() {
E 3

D 3
}
E 3
I 3
D 4
    int i,legal;
E 4
I 4
    int i;
    boolean legal;
E 4
    String com;
    String line;
I 4
    char c;
E 4
E 3


I 3
D 4
    // for reading from System.i
    DataInputStream in = new DataInputStream(System.in);
E 4
I 4
    // for reading from System.in
    BufferedReader in = new BufferedReader(new InputStreamReader(System.in));
E 4


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
D 4
    legal=0;
    while(legal==0) {
E 4
I 4
    legal=false;
    while(!legal) {
E 4
      System.out.print("Enter entry type (? for help): ");
      try {
	entry_type=in.readLine();
      }
      catch(IOException e) {
	System.err.println("Unable to read entry_type: " + e);
	System.exit(-1);
      }
D 4
      for(i=0; i<legal_entry.length; i++) if(entry_type.equals(legal_entry[i])){legal=1; break;}
      if(legal!=1) {
E 4
I 4
      for(i=0; i<legal_entry.length; i++) if(entry_type.equals(legal_entry[i])){legal=true; break;}
      if(!legal) {
E 4
	System.out.println("\nLegal entry types:");
	for(i=0; i<legal_entry.length; i++) System.out.println("   "+legal_entry[i]);
	System.out.println();
      }
    }
    
    
    
    // get system_type
D 4
    legal=0;
    while(legal==0) {
E 4
I 4
    legal=false;
    while(!legal) {
E 4
      System.out.print("Enter system type (? for help): ");
      try {
	system_type=in.readLine();
      }
      catch(IOException e) {
	System.err.println("Unable to read system_type: " + e);
	System.exit(-1);
      }
D 4
      for(i=0; i<legal_system.length; i++) if(system_type.equals(legal_system[i])){legal=1; break;}
      if(legal!=1) {
E 4
I 4
      for(i=0; i<legal_system.length; i++) if(system_type.equals(legal_system[i])){legal=true; break;}
      if(!legal) {
E 4
	System.out.println("\nLegal system types:");
	for(i=0; i<legal_system.length; i++) System.out.println("   "+legal_system[i]);
	System.out.println();
      }
    }
    
    
D 4
    
E 4
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
D 4
    System.out.print("Enter operators: ");
    try {
      operators=in.readLine();
    }
    catch(IOException e) {
      System.err.println("Unable to read operators: " + e);
      System.exit(-1);
E 4
I 4
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
E 4
    }
    
    
    // get email address
D 4
    System.out.print("Enter operator email address: ");
    try {
      email_address=in.readLine();
    }
    catch(IOException e) {
      System.err.println("Unable to read email address: " + e);
      System.exit(-1);
E 4
I 4
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
E 4
    }
I 4

E 4
    
D 4
    
    // read comment
    System.out.println("Enter comment or filename:");
E 4
I 4
    // read comment from file or command line
    comment = new StringBuffer();
    System.out.print("Enter comment filename, or return to type in comment: ");
E 4
    try {
      com=in.readLine();
    } 
    catch(IOException e) {
D 4
      System.err.println("Unable to read comment or filename: " + e);
E 4
I 4
      System.err.println("Unable to read comment filename: " + e);
E 4
      System.exit(-1);
      com=null;
    }
D 4
    comment = new StringBuffer();
    try {
      BufferedReader b = new BufferedReader(new FileReader(com));
      String s;
      while((s=b.readLine())!=null) { comment.append(s); comment.append("\n"); }
      b.close();
    }
    catch(IOException e) {
      comment.append(com);
E 4
I 4
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
E 4
    }
    
    
    return;
  }
  

E 3
//-----------------------------------------------------------------------------
I 3
}        //  end class definition:  logbook_text
E 3
//-----------------------------------------------------------------------------
E 1
