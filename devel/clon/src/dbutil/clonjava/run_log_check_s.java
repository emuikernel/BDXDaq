//  run_log_check.java

// dumps run log info for operators to check on shift

package clonjava;

import java.lang.*;
import java.text.DecimalFormat;
import java.sql.*;
import clonjava.clon.util.*;


//-----------------------------------------------------------------------------
//  begin class definition
//-----------------------------------------------------------------------------


public class run_log_check {

    private static String database       = "clasprod";
  
    private static String driver         = "com.mysql.jdbc.Driver";
    private static String url            = "jdbc:mysql://clondb1:3306/";
    private static String password       = "";
    //    private static String password       = "e1tocome";
    private static String account        = "clasrun";

    //   private static String driver         = "com.cariboulake.jsql.JSQLDriver";
    //   private static String url            = "jdbc:caribou:jsqlingres://db5:6024/";
    //   private static String account        = "foo";
    //   private static String password       = null;
  private static int run_start;
  private static int run_end           = 0;


//-----------------------------------------------------------------------------


  static public void main (String[] args) {
    
    java.sql.Connection con = null;
    int deltat,run,nevent,nfile,ec_hi,ec_lo,cc_hi;
    String start_date,target;
    int beam,torus,mini;
    float current;
    int fcup_active,fcup_live,clock_active,clock_live;
    int last_run;
    float livef,livec;
    DecimalFormat fmt  = new DecimalFormat("#0.00");


    // decode command line
    decode_command_line(args);


    // load jdbc driver
    try {
      Class.forName(driver);
    }
    catch(Exception e) {
      System.err.println("Unable to load JDBC driver " + driver);
      System.err.println(e.toString());
      System.exit(-1);
    }
    

    // connect to database
    try  {
      con = DriverManager.getConnection(url+database,account,password);
    }
    catch (Exception e) {
      System.err.println("Unable connect to database " + url + database);
      System.err.println(e.toString());
      System.exit(-1);
    }
    

    // create and execute query
    try {

      // create statement object
      Statement s = con.createStatement();


      // create sql string
      String sql = "select interval('mins',end_date-start_date)," + 
	" start_date," +
	" b.run,nfile,nevent,target,beam_energy,beam_current_request," +
	" torus_current,mini_current,ec_total_hi,ec_total_lo,cc_hi," +
	" fcup_active,fcup_live,clock_active,clock_live" +
	" from run_log_begin b, run_log_comment c, run_log_end e" +
	" where" +
	" b.session_name='clasprod' and c.session_name='clasprod' and e.session_name='clasprod'" + 
	" and b.run=e.run and b.run=c.run" + 
	" and b.run>=" + run_start;
      if(run_end>0)sql +=" and b.run<=" + run_end;
      sql += " order by b.run";


      // query and get result
      ResultSet result = s.executeQuery(sql);


      // print run info
      last_run=run_start-1;
      System.out.println("    Run       Start      " + "File" + "    Event  Time    Target  LiveF  LiveC  " + 
			 "Beam     I  Torus   Mini  ec_hi  ec_lo  cc_hi      FcupA");
      System.out.println("    ---       -----      " + "----" + "    -----  ----    ------  -----  -----  " + 
			 "----     _  -----   ----  -----  -----  -----      -----\n");
      while (result.next()) {

	// unpack result
	deltat=result.getInt(1);
	run=result.getInt("run");
	start_date=result.getString("start_date");
	nfile=result.getInt("nfile");
	nevent=result.getInt("nevent");
	target=result.getString("target");
	fcup_active=result.getInt("fcup_active");
	fcup_live=result.getInt("fcup_live");
	clock_active=result.getInt("clock_active");
	clock_live=result.getInt("clock_live");
	beam=result.getInt("beam_energy");
	current=result.getFloat("beam_current_request");
	torus=result.getInt("torus_current");
	mini=result.getInt("mini_current");
	ec_hi=result.getInt("ec_total_hi");
	ec_lo=result.getInt("ec_total_lo");
	cc_hi=result.getInt("cc_hi");


	// get livetimes
	livef=(float)((fcup_active>0)?((double)fcup_live)/fcup_active:0.);
	livec=(float)((clock_active>0)?((double)clock_live)/clock_active:0.);


	// check for gaps
	if(run>(last_run+1)) System.out.println("*** missing run " + (last_run+1) 
						+ " to run " + (run-1) + " ***");
	

	// print run info
	if (run==last_run) {
	  System.out.print("***");
	} else {
	  System.out.print("   ");
	}
	last_run=run;

	System.out.print(Format.justify(run,6));
	System.out.print(Format.justify(start_date.substring(2,16),-15));
	System.out.print(Format.justify(nfile,-5));
        System.out.print(Format.justify(nevent,-9));
        System.out.print(Format.justify(deltat,-6));
        System.out.print(Format.justify(target,-10));
        System.out.print(Format.justify(fmt.format(livef),-7));
        System.out.print(Format.justify(fmt.format(livec),-7));
	System.out.print(Format.justify(beam,-6) + Format.justify(current,-6));
	System.out.print(Format.justify(torus,-7) + Format.justify(mini,-7));
	System.out.print(Format.justify(ec_hi,-7) + Format.justify(ec_lo,-7));
	System.out.print(Format.justify(cc_hi,-7));
	System.out.print(Format.justify(fcup_active,-11));
	System.out.println();

      }
      

      // done...no more rows
      result.close();
      s.close();
      System.out.println();


    }
    catch (SQLException e) {
      System.out.println(e.getMessage());
    }
    catch (Exception e) {
      e.printStackTrace();
      System.exit(-1);
    }


    System.exit(-1);

  }  // main done


//-----------------------------------------------------------------------------
  

  static public void decode_command_line(String[] args) {
    
    int i;
    String help = "\nUsage:\n\n" + 
      "   java run_log_check [-driver driver] [-url url] [-db database]\n" + 
      "                      [-account account] [-pwd password] run_start [run_end]\n\n";
    
    
    // must have at least 1 arg
    if(args.length<1) {
      System.out.println(help);
      System.exit(-1);
    }


    // loop over all args
    for(i=0; i<args.length; i++) {
      
      if(!args[i].startsWith("-")) {
	break;

      } else if(args[i].equalsIgnoreCase("-h")) {
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
      }
      
    }


    // get start,end run numbers
    run_start=Integer.parseInt(args[i]);
    if((i+1)<args.length)run_end=Integer.parseInt(args[args.length-1]);
    
    return;
  }
  
}
//-----------------------------------------------------------------------------
//  end class definition
//-----------------------------------------------------------------------------
