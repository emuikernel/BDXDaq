// online_db_dump.java

// dumps online table info

//  E.Wolin, 6-jun-98

package clonjava;

import java.lang.*;
import java.sql.*;


//-----------------------------------------------------------------------------
//  begin class definition
//-----------------------------------------------------------------------------


public class online_db_dump {

  
  static public void main (String[] args) {
    
    String url = "jdbc:caribou:jsqlingres://db5:6024/clasprod";
    java.sql.Connection con = null;


    // load jdbc driver
    try {
      Class.forName("com.cariboulake.jsql.JSQLDriver");
    }
    catch(Exception e) {
      System.err.println(e.toString());
      System.exit(-1);
    }
    

    // connect to database
    try  {
      con = DriverManager.getConnection(url,"foo",null);
    }
    catch (Exception e) {
      System.err.println(e.toString());
      System.exit(-1);
    }
    

    // set access flags
    try {
      con.setAutoCommit(true);
      con.setReadOnly(true);
    }
    catch (Exception e) {
      System.err.println(e.toString());
      System.exit(-1);
    }


    // dump database info
    try {
      
      DatabaseMetaData dbmeta= con.getMetaData();
      
      
      // tables and views
      System.out.println("\nTables and Views in clasprod Online Database");
      System.out.println("-----------------------------------------------\n\n");
      

      //      ResultSet dbrs = dbmeta.getTables(null,null,"%",new String [] {"TABLE","VIEW"});
      ResultSet dbrs = dbmeta.getTables(null,null,"%",new String [] {"TABLE"});
      while(dbrs.next()) {
	

	String table = dbrs.getString("TABLE_NAME");


	// ignore system tables
	if(table.startsWith("dd_"))continue;
	if(table.endsWith("sha"))continue;
	if(table.endsWith("arc"))continue;


	// get owner
//  	System.out.println(table + "  (type: " + dbrs.getString("TABLE_TYPE") + ")");
	ResultSet prrs = dbmeta.getTablePrivileges(null,null,table);
	prrs.next();
	System.out.println(table + "  (type: " + dbrs.getString("TABLE_TYPE") + 
			   ", owner: " + prrs.getString("GRANTOR")+ ")");


	// column info
	ResultSet tbrs = dbmeta.getColumns(null,null,table,"%");
	while(tbrs.next()) {
	  StringBuffer sb = new StringBuffer();
	  for(int j=0; j<70; j++) {sb.append(' ');}
	  //	  sb.insert(7,tbrs.getString("COLUMN_NAME"));
	  sb.insert(7,tbrs.getString(4));
	  sb.insert(30,tbrs.getString("TYPE_NAME"));
	  sb.insert(50,tbrs.getString("COLUMN_SIZE"));
	  System.out.println(sb);
	}
	System.out.println("\n\n");
      }


//       // procedures
//       System.out.println("\n\n\nProcedures in clasprod Online Database");
//       System.out.println("--------------------------------------\n\n");
//       ResultSet prs = dbmeta.getProcedures(null,null,"%");
// 	while(prs.next()) {
// 	  System.out.println("      " + prs.getString("PROCEDURE_NAME"));
// 	}      


    }
    catch (SQLException e) {
      System.out.println(e.getMessage());
    }
    catch (Exception e) {
      System.err.println(e.toString());
      System.exit(-1);
    }
    

    System.exit(-1);
  }
  
} 

//-----------------------------------------------------------------------------
//  end class definition
//-----------------------------------------------------------------------------
