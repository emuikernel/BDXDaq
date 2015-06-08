//  event_count.java

//  prints event counts by run number given date range

//  ejw, 9-jun-98


package clonjava;

import java.lang.*;
import java.text.*;
import java.sql.*;


//-----------------------------------------------------------------------------
//  begin class definition
//-----------------------------------------------------------------------------


public class event_count {

  private static String driver         = "com.cariboulake.jsql.JSQLDriver";
  private static String url            = "jdbc:caribou:jsqlingres://db5:6024/";
  private static String database       = "db5::clasprod";
  private static String account        = "foo";
  private static String password       = null;

  
  static public void main (String[] args) {
    
    if((args.length<=0)||args[0].equalsIgnoreCase("-h")) {
      System.out.println("\nUsage:\n\n     java event_count start_date [end_date]\n\n");
      System.exit(-1);
    }


    // magic url to connect to database on db5
    java.sql.Connection con = null;


    // sql join on rlb,rlc,rle tables, fill in start,end dates
    String sql = "select b.run,start_date,run_type,nevent,nlong" + 
      " from run_log_begin b, run_log_comment c, run_log_end e" + 
      " where b.session_name='clasprod' and " + 
      " b.session_name=c.session_name and b.session_name=e.session_name and " + 
      " b.run=c.run and b.run=e.run and " + 
      " (run_type like 'photon%' or run_type like 'beam%') and " + 
      " nevent>0 and nlong>0 and " +
      " start_date>'" + args[0] + "'";
    if(args.length>1)sql+=" and start_date<'" + args[1] + "'";


    // load openlink jdbc driver...opljdbc.zip must be in CLASSPATH
    try {
      Class.forName(driver);
    }
    catch(Exception e) {
      System.err.println("Unable to load driver: " + driver +"\n" + e);
      System.exit(-1);
    }
    

    // connect to database
    try  {
      con = DriverManager.getConnection(url+database,account,password);
    }
    catch (Exception e) {
      e.printStackTrace(); System.exit(-1);
    }
    

    // execute query, print results
    try {

      // create statement object
      Statement s = con.createStatement();


      // query and get result and meta-data sets
      ResultSet result = s.executeQuery(sql);
      ResultSetMetaData meta = result.getMetaData();
      
      
      // print header
      StringBuffer sbh = new StringBuffer();
      for(int j=0; j<80; j++) {sbh.append(' ');}
      sbh.insert( 0,"# Run");
      sbh.insert(8,"Run_type");
      sbh.insert(25,"start_date");
      sbh.insert(50,"nevent");
      sbh.insert(60,"event_sum");
      sbh.insert(70,"nlong");
      sbh.insert(80,"long_sum");
      sbh.setLength(90);
      System.out.println();
      System.out.println(sbh);
      System.out.println();
      

      // print results
      int run;
      String type,start_date;
      int nevent;
      int nlong;
      double nevsum = 0.0;
      double nlsum = 0.0;
      DecimalFormat fmt = new DecimalFormat("#########0.00");
      while (result.next()) {
	type=result.getString("run_type");
	run=result.getInt("run");
	start_date=result.getString("start_date");
	nevent=result.getInt("nevent");
	nlong=result.getInt("nlong");
	nevsum+=(double)nevent/1000.0;
	nlsum+=(double)nlong;

	StringBuffer sb = new StringBuffer();
	for(int j=0; j<80; j++) {sb.append(' ');}
	sb.insert( 0,run);
	sb.insert(8,type);
	sb.insert(25,start_date);
	sb.insert(50,nevent);
	sb.insert(60,fmt.format(nevsum/1.0e3));
	sb.insert(70,fmt.format(nlong/1.e3));
	sb.insert(80,fmt.format(nlsum/1.0e6));
	sb.setLength(90);
	System.out.println(sb);
      }
      

      // no more rows
      result.close();
      s.close();
    }

    catch (SQLException e) {
      System.out.println(e.getMessage());
    }
    catch (Exception e) {
      e.printStackTrace();
      System.exit(-1);
    }


  }  // main done
  

}


//-----------------------------------------------------------------------------
//  end class definition
//-----------------------------------------------------------------------------
