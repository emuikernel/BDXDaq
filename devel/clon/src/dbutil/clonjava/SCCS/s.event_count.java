h16237
s 00002/00000/00151
d D 1.6 06/11/20 11:22:17 boiarino 7 6
c package clonjava;
c 
e
s 00001/00001/00150
d D 1.5 02/01/30 16:29:10 wolin 6 5
c New cariboulake
e
s 00010/00006/00141
d D 1.4 98/11/16 02:29:38 wolin 5 4
c Using cariboulake
c 
e
s 00001/00001/00146
d D 1.3 98/07/10 16:44:09 wolin 4 3
c Typo
c 
e
s 00001/00001/00146
d D 1.2 98/06/11 16:09:54 wolin 3 1
c No password
c 
e
s 00000/00000/00000
d R 1.2 98/06/09 10:52:53 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 dbutil/s/event_count.java
e
s 00147/00000/00000
d D 1.1 98/06/09 10:52:52 wolin 1 0
c date and time created 98/06/09 10:52:52 by wolin
e
u
U
f e 0
t
T
I 1
//  event_count.java

//  prints event counts by run number given date range

//  ejw, 9-jun-98


I 7
package clonjava;

E 7
import java.lang.*;
import java.text.*;
import java.sql.*;


//-----------------------------------------------------------------------------
//  begin class definition
//-----------------------------------------------------------------------------


public class event_count {

I 5
D 6
  private static String driver         = "COM.cariboulake.sql.ingres.JSQLDriver";
E 6
I 6
  private static String driver         = "com.cariboulake.jsql.JSQLDriver";
E 6
  private static String url            = "jdbc:caribou:jsqlingres://db5:6024/";
  private static String database       = "db5::clasprod";
  private static String account        = "foo";
  private static String password       = null;

E 5
  
  static public void main (String[] args) {
    
    if((args.length<=0)||args[0].equalsIgnoreCase("-h")) {
D 4
      System.out.println("\nUsage:\n\n     javac event_class start_date [end_date]\n\n");
E 4
I 4
D 5
      System.out.println("\nUsage:\n\n     java event_class start_date [end_date]\n\n");
E 5
I 5
      System.out.println("\nUsage:\n\n     java event_count start_date [end_date]\n\n");
E 5
E 4
      System.exit(-1);
    }


    // magic url to connect to database on db5
D 3
    String url = "jdbc:openlink://db5.cebaf.gov/DSN=dsn_clasprod/UID=riccardi/PWD=e4March";
E 3
I 3
D 5
    String url = "jdbc:openlink://db5.cebaf.gov/DSN=dsn_clasprod";
E 5
E 3
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
D 5
      Class.forName("openlink.jdbc.Driver");
E 5
I 5
      Class.forName(driver);
E 5
    }
    catch(Exception e) {
D 5
      System.err.println("DBManager: Unable to load JDBC driver" + url);
      System.err.println("DBManager: " + e.toString());
E 5
I 5
      System.err.println("Unable to load driver: " + driver +"\n" + e);
E 5
      System.exit(-1);
    }
    

    // connect to database
    try  {
D 5
      con = DriverManager.getConnection(url);
E 5
I 5
      con = DriverManager.getConnection(url+database,account,password);
E 5
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
E 1
