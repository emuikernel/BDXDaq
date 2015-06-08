h19489
s 00001/00000/00131
d D 1.6 06/11/20 11:24:09 boiarino 7 6
c package clonjava;
c 
e
s 00001/00001/00130
d D 1.5 02/01/30 16:31:14 wolin 6 5
c New cariboulake
e
s 00031/00029/00100
d D 1.4 99/03/22 11:38:31 wolin 5 4
c Table dump seems to be working
c 
e
s 00013/00013/00116
d D 1.3 98/11/05 12:21:42 wolin 4 3
c Seems to work with latest caribou lake
c 
e
s 00001/00001/00128
d D 1.2 98/06/11 16:10:00 wolin 3 1
c No password
c 
e
s 00000/00000/00000
d R 1.2 98/06/08 14:04:24 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 dbutil/s/online_db_dump.java
e
s 00129/00000/00000
d D 1.1 98/06/08 14:04:23 wolin 1 0
c date and time created 98/06/08 14:04:23 by wolin
e
u
U
f e 0
t
T
I 1
// online_db_dump.java

D 5
// dumps online tables, columns, and related info

D 4

E 4
// still to do:
D 4
//   add views, etc.
E 4
I 4
//   add procedures, views, etc.
E 4

E 5
I 5
// dumps online table info
E 5

//  E.Wolin, 6-jun-98

I 7
package clonjava;
E 7

D 5

// packages
E 5
import java.lang.*;
import java.sql.*;


//-----------------------------------------------------------------------------
//  begin class definition
//-----------------------------------------------------------------------------


public class online_db_dump {

  
  static public void main (String[] args) {
    
D 5

    // magic url to connect to database on db5
D 3
    String url = "jdbc:openlink://db5.cebaf.gov/DSN=dsn_clasprod/UID=riccardi/PWD=e4March";
E 3
I 3
D 4
    String url = "jdbc:openlink://db5.cebaf.gov/DSN=dsn_clasprod";
E 4
I 4
    String url = "jdbc:caribou:jsqlingres://db5:6024/db5::clasprod";
E 5
I 5
    String url = "jdbc:caribou:jsqlingres://db5:6024/clasprod";
E 5
E 4
E 3
    java.sql.Connection con = null;


D 5
    // load openlink jdbc driver...opljdbc.zip must be in CLASSPATH
E 5
I 5
    // load jdbc driver
E 5
    try {
D 4
      Class.forName("openlink.jdbc.Driver");
E 4
I 4
D 6
      Class.forName("COM.cariboulake.sql.ingres.JSQLDriver");
E 6
I 6
      Class.forName("com.cariboulake.jsql.JSQLDriver");
E 6
E 4
    }
    catch(Exception e) {
D 5
      System.err.println("DBManager: Unable to load JDBC driver" + url);
      System.err.println("DBManager: " + e.toString());
E 5
I 5
      System.err.println(e.toString());
E 5
      System.exit(-1);
    }
    

    // connect to database
    try  {
D 4
      con = DriverManager.getConnection(url);
E 4
I 4
      con = DriverManager.getConnection(url,"foo",null);
E 4
    }
    catch (Exception e) {
D 5
      e.printStackTrace();
E 5
I 5
      System.err.println(e.toString());
E 5
      System.exit(-1);
    }
    

    // set access flags
    try {
      con.setAutoCommit(true);
      con.setReadOnly(true);
    }
    catch (Exception e) {
D 5
      e.printStackTrace();
E 5
I 5
      System.err.println(e.toString());
E 5
      System.exit(-1);
    }


    // dump database info
    try {
D 5

E 5
I 5
      
E 5
      DatabaseMetaData dbmeta= con.getMetaData();
D 5


E 5
I 5
      
      
E 5
      // tables and views
      System.out.println("\nTables and Views in clasprod Online Database");
      System.out.println("-----------------------------------------------\n\n");
D 5
	
      ResultSet dbrs = dbmeta.getTables(null,null,"%",new String [] {"TABLE","VIEW"});
E 5
I 5
      

      //      ResultSet dbrs = dbmeta.getTables(null,null,"%",new String [] {"TABLE","VIEW"});
      ResultSet dbrs = dbmeta.getTables(null,null,"%",new String [] {"TABLE"});
E 5
      while(dbrs.next()) {
I 5
	
E 5

D 5
	// table name
E 5
	String table = dbrs.getString("TABLE_NAME");
I 5


	// ignore system tables
	if(table.startsWith("dd_"))continue;
	if(table.endsWith("sha"))continue;
	if(table.endsWith("arc"))continue;


	// get owner
//  	System.out.println(table + "  (type: " + dbrs.getString("TABLE_TYPE") + ")");
E 5
	ResultSet prrs = dbmeta.getTablePrivileges(null,null,table);
	prrs.next();
	System.out.println(table + "  (type: " + dbrs.getString("TABLE_TYPE") + 
			   ", owner: " + prrs.getString("GRANTOR")+ ")");

D 5
	// column data
E 5
I 5

	// column info
E 5
	ResultSet tbrs = dbmeta.getColumns(null,null,table,"%");
	while(tbrs.next()) {
	  StringBuffer sb = new StringBuffer();
	  for(int j=0; j<70; j++) {sb.append(' ');}
D 4
	  sb.insert(7,tbrs.getString("COLUMN_NAME"));
E 4
I 4
	  //	  sb.insert(7,tbrs.getString("COLUMN_NAME"));
	  sb.insert(7,tbrs.getString(4));
E 4
	  sb.insert(30,tbrs.getString("TYPE_NAME"));
	  sb.insert(50,tbrs.getString("COLUMN_SIZE"));
	  System.out.println(sb);
	}
	System.out.println("\n\n");
      }


D 4
      // procedures
      System.out.println("\n\n\nProcedures in clasprod Online Database");
      System.out.println("--------------------------------------\n\n");
E 4
I 4
//       // procedures
//       System.out.println("\n\n\nProcedures in clasprod Online Database");
//       System.out.println("--------------------------------------\n\n");
E 4
D 5
	
E 5
D 4
      ResultSet prs = dbmeta.getProcedures(null,null,"%");
	while(prs.next()) {
	  System.out.println("      " + prs.getString("PROCEDURE_NAME"));
	}      
E 4
I 4
//       ResultSet prs = dbmeta.getProcedures(null,null,"%");
// 	while(prs.next()) {
// 	  System.out.println("      " + prs.getString("PROCEDURE_NAME"));
// 	}      
E 4


    }
    catch (SQLException e) {
      System.out.println(e.getMessage());
    }
    catch (Exception e) {
D 5
      e.printStackTrace();
E 5
I 5
      System.err.println(e.toString());
E 5
      System.exit(-1);
    }
I 5
    
E 5

D 5

  }  // main done
E 5
I 5
    System.exit(-1);
  }
E 5
  
D 5

}

E 5
I 5
} 
E 5

//-----------------------------------------------------------------------------
//  end class definition
//-----------------------------------------------------------------------------
E 1
